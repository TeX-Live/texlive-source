/*
Copyright (c) 2005-2006 Han The Thanh, <thanh@pdftex.org>

This file is part of pdfTeX.

pdfTeX is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

pdfTeX is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with pdfTeX; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "ptexlib.h"
#include <string.h>

static struct avl_table *sfd_tree = NULL;

static unsigned char *sfd_buffer = NULL;
static integer sfd_size = 0;
static integer sfd_curbyte = 0;

#define SFD_BUF_SIZE    SMALL_BUF_SIZE

#define sfd_close()     xfclose(sfd_file, cur_file_name)
#define sfd_open()      open_input(&sfd_file, kpse_sfd_format, FOPEN_RBIN_MODE)

#define sfd_read_file() readbinfile(sfd_file,&sfd_buffer,&sfd_size)
#define sfd_getchar()   sfd_buffer[sfd_curbyte++]
#define sfd_eof()      (sfd_curbyte>sfd_size)


static FILE *sfd_file;
static char sfd_line[SFD_BUF_SIZE];

static subfont_entry *new_subfont_entry (void)
{
    int i;
    subfont_entry *subfont;
    subfont = xtalloc (1, subfont_entry);
    subfont->infix = NULL;
    for (i = 0; i < 256; ++i)
        subfont->charcodes[i] = -1;     /* unassigned */
    subfont->next = NULL;
    return subfont;
}

static sfd_entry *new_sfd_entry (void)
{
    sfd_entry *sfd;
    sfd = xtalloc (1, sfd_entry);
    sfd->name = NULL;
    sfd->subfont = NULL;
    return sfd;
}

static void destroy_sfd_entry (void *pa, void *pb)
{
    subfont_entry *p, *q;
    sfd_entry *sfd;
    sfd = (sfd_entry *) pa;
    p = sfd->subfont;
    while (p != NULL) {
        q = p->next;
        xfree (p->infix);
        xfree (p);
        p = q;
    }
    xfree (sfd->name);
}

static int comp_sfd_entry (const void *pa, const void *pb, void *p)
{
    return strcmp (((const sfd_entry *) pa)->name,
                   ((const sfd_entry *) pb)->name);
}

void sfd_free (void)
{
    if (sfd_tree != NULL)
        avl_destroy (sfd_tree, destroy_sfd_entry);
}

static void sfd_getline (boolean expect_eof)
{
    char *p;
    int c;
  restart:
    if (sfd_eof ()) {
        if (expect_eof)
            return;
        else
            pdftex_fail ("unexpected end of file");
    }
    p = sfd_line;
    do {
        c = sfd_getchar ();
        append_char_to_buf (c, p, sfd_line, SFD_BUF_SIZE);
    } while (c != 10 && !sfd_eof());
    append_eol (p, sfd_line, SFD_BUF_SIZE);
    if (p - sfd_line < 2 || *sfd_line == '#')
        goto restart;
}

static sfd_entry *read_sfd (char *sfd_name)
{
    void **aa;
    sfd_entry *sfd, tmp_sfd;
    subfont_entry *sf;
	char *ftemp = NULL;
    char buf[SMALL_BUF_SIZE], *p;
    long int i, j, k;
    int n;
    int callback_id=0;
    int file_opened=0;
    /* check whether this sfd has been read */
    tmp_sfd.name = sfd_name;
    if (sfd_tree == NULL) {
        sfd_tree = avl_create (comp_sfd_entry, NULL, &avl_xallocator);
        assert (sfd_tree != NULL);
    }
    sfd = (sfd_entry *) avl_find (sfd_tree, &tmp_sfd);
    if (sfd != NULL)
        return sfd;
    set_cur_file_name (sfd_name);
    if (sfd_buffer!=NULL) {
      xfree(sfd_buffer);
      sfd_buffer=NULL;
    }
    sfd_curbyte=0;
    sfd_size=0;

	callback_id=callback_defined(find_sfd_file_callback);
	if (callback_id>0) {
	  if(run_callback(callback_id,"S->S",cur_file_name,&ftemp)) {
		if(ftemp!=NULL&&strlen(ftemp)) {
		  if (cur_file_name)
			free(cur_file_name);
		  cur_file_name = xstrdup(ftemp);
		  free(ftemp);
		}
	  }
	}
    callback_id=callback_defined(read_sfd_file_callback);
    if (callback_id>0) {
      if(! (run_callback(callback_id,"S->bSd",cur_file_name,
		       &file_opened, &sfd_buffer,&sfd_size) &&
	    file_opened && 
	    sfd_size>0 ) ) {
	pdftex_warn ("cannot open SFD file for reading");
	cur_file_name = NULL;
	return NULL;      
      }
      sfd_read_file();
      sfd_close();
    }
    tex_printf ("{");
    tex_printf (cur_file_name);
    sfd = new_sfd_entry ();
    sfd->name = xstrdup (sfd_name);
    while (!sfd_eof ()) {
        sfd_getline (true);
        if (*sfd_line == 10)    /* empty line indicating eof */
            break;
        sf = new_subfont_entry ();
        sf->next = sfd->subfont;
        sfd->subfont = sf;
        sscanf (sfd_line, "%s %n", buf, &n);
        sf->infix = xstrdup (buf);
        p = sfd_line + n;       /* skip to the next word */
        k = 0;
      read_ranges:
        for (;;) {
            if (*p == '\\') {   /* continue on next line */
                sfd_getline (false);
                p = sfd_line;
                goto read_ranges;
            } else if (*p == 0) /* end of subfont */
                break;
            if (sscanf (p, " %li %n", &i, &n) == 0)
                pdftex_fail ("invalid token:\n%s", p);
            p += n;
            if (*p == ':') {    /* offset */
                k = i;
                p++;
            } else if (*p == '_') {     /* range */
                if (sscanf (p + 1, " %li %n", &j, &n) == 0)
                    pdftex_fail ("invalid token:\n%s", p);
                if (i > j || k + (j - i) > 255)
                    pdftex_fail ("invalid range:\n%s", p);
                while (i <= j)
                    sf->charcodes[k++] = i++;
                p += n + 1;
            } else              /* codepoint */
                sf->charcodes[k++] = i;
        }
    }
    tex_printf ("}");
    aa = avl_probe (sfd_tree, sfd);
    assert (aa != NULL);
    return sfd;
}

boolean handle_subfont_fm (fm_entry * fm, int mode)
{
    size_t l;
    char *p, *q, *r;
    sfd_entry *sfd;
    subfont_entry *sf;
    fm_entry *fm2;
    char buf[SMALL_BUF_SIZE];
    assert (fm->tfm_name != NULL);
    p = fm->tfm_name;
    q = strchr (p, '@');        /* search for the first '@' */
    if (q == NULL)
        return false;
    r = strchr (q + 1, '@');    /* search for the second '@' */
    if (r == NULL)
        return false;
    if (q <= p || r <= q + 1    /* prefix or sfd name is empty */
        || r - p != strlen (p) - 1)     /* or the second '@' is not the last char yet */
        return false;
    l = r - (q + 1);            /* length of sfd name */
    strncpy (buf, q + 1, l);
    buf[l] = 0;
	check_buf(strlen(buf) + 4, SMALL_BUF_SIZE);
    strcat (buf, ".sfd");
    sfd = read_sfd (buf);
    if (sfd == NULL)
        return false;
    /* at this point we know fm is a subfont */
    set_subfont (fm);
    xfree (fm->ps_name);
    fm->ps_name = NULL;
    /* set default values for PidEid */
    if (fm->pid == -1) {
        fm->pid = 3;
        fm->eid = 1;
    }
    l = q - p;                  /* length of base tfm name (prefix) */
    for (sf = sfd->subfont; sf != NULL; sf = sf->next) {
        strncpy (buf, p, l);
        buf[l] = 0;
        strcat (buf, sf->infix);
        fm2 = new_fm_entry ();
        fm2->tfm_name = xstrdup (buf);
        fm2->ff_name = xstrdup (fm->ff_name);
        fm2->type = fm->type;
        fm2->pid = fm->pid;
        fm2->eid = fm->eid;
        fm2->subfont = sf;
        if (avl_do_entry (fm2, mode) != 0)      /* try to insert the entry */
            delete_fm_entry (fm2);      /* delete it if failed */
    }
    delete_fm_entry (fm);
    return true;
}
