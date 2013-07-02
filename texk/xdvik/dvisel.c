/*
 * select pages from a DVI file.
 *
 * Copyright (c) 1999
 *      WATANABE Takeshi        watanabe@komadori.planet.sci.kobe-u.ac.jp
 * Copyright (c) 2002-2004 the xdvik development team
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL ANY AUTHO OF THIS SOFTWARE BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "xdvi-config.h"

#include <string.h>
#include <ctype.h>

#include "dvi.h"
#include "pagesel.h"
#include "dvisel.h"
#include "util.h"
#include "dvi-init.h"
#include "events.h"
#include "dvi-draw.h"
#include "message-window.h"
#include "statusline.h"
#include "print-dialog.h"

/* stack for HTEX / Color stack */
typedef enum { IS_A_HREF, IS_A_COLOR } stackElemT;

/*
 * Length of BOP command, without p[4] (previous BOP pointer):
 * bop c_0[4] ... c_9[4]
 */
static const int BOP_PART_LEN = 1 + 10 * 4;
/*
 * Like above but with p[4]:
 */
static const int BOP_LEN = 1 + 10 * 4 + 4;
/*
 * Lenght of postamble:
 * post p[4] num[4] den[4] mag[4] l[4] u[4] s[2] t[2]
 */
static const int POSTAMBLE_LEN = 1 + 4 * 6 + 2 + 2;
/*
 * Postamble without p[4] (final bop offset) and t[2] (total page number):
 * num[4] den[4] mag[4] l[4] u[4] s[2]
 */
static const int POSTAMBLE_PART_LEN = 4 * 5 + 2; 

static char *dvips_papersize_special = NULL;

/*
 * Struct to save specials that need to be copied verbatim into the output file
 * (things like `draftcopy'; see dvips manual, `Literal headers')
 */
struct literal_headers {
    size_t size;
    char **headers;
};
static struct literal_headers literal_headers = { 0, NULL };

static struct specials_stack color_stack = { 0, NULL};
static struct specials_stack href_stack = { 0, NULL};

/*
  stacks saved from the previous page (i.e. active at the
  beginning of the page)
*/
static struct specials_stack color_save_stack = { 0, NULL };
static struct specials_stack href_save_stack = { 0, NULL };

static void
push_stack(struct specials_stack *stack, const char *content)
{
    stack->items = xrealloc(stack->items, (stack->stack_len + 1) * sizeof *(stack->items));
    stack->items[stack->stack_len].content = xstrdup(content);
    stack->stack_len++;
}

static Boolean
stack_contains_item(struct specials_stack *stack, const char *str)
{
    size_t i;
    for (i = 0; i < stack->stack_len; i++) {
	if (strcmp(str, stack->items[i].content) == 0) {
	    return True;
	}
    }
    return False;
}

static void
pop_stack(struct specials_stack *stack)
{
    ASSERT(stack->stack_len >= 1, "Attempt to pop empty stack");
    free(stack->items[stack->stack_len - 1].content);
    stack->stack_len--;
}

static void
empty_stack(struct specials_stack *stack)
{
    while(stack->stack_len > 0) {
	pop_stack(stack);
    }
}

/*
  This is used instead of struct tn, and struct font* from dvi-init.h,
  since it appears that we don't need all the information from there.
*/
static struct fontinfo {
    long TeXnumber;
    unsigned char info[14];
    char *fontname;
    struct fontinfo *next;
    Boolean used;
} *fontinfo_head;

#define PutFour(num, fp) {		\
    putc(((num) >> 24) & 0xff, (fp));	\
    putc(((num) >> 16) & 0xff, (fp));	\
    putc(((num) >> 8)  & 0xff, (fp));	\
    putc( (num)        & 0xff, (fp));	\
}

#define CopyNum(fin, fout, num) {		\
    int m = num;				\
    while (--m >= 0) putc(getc(fin), fout);	\
}

static void
FontWrite(FILE *fout, struct fontinfo *fontp, long *fout_pos)
{
    if (fontp->TeXnumber > 0xff) {
	if (fontp->TeXnumber > 0xffff) {
	    if (fontp->TeXnumber > 0xffffff) {
		putc(FNTDEF4, fout);
		putc((fontp->TeXnumber >> 24) & 0xff, fout);
		(*fout_pos)++;
	    }
	    else {
		putc(FNTDEF3, fout);
	    }
	    putc((fontp->TeXnumber >> 16) & 0xff, fout);
	    (*fout_pos)++;
	}
	else {
	    putc(FNTDEF2, fout);
	}
	putc((fontp->TeXnumber >> 8) & 0xff, fout);
	(*fout_pos)++;
    }
    else {
	putc(FNTDEF1, fout);
    }
    putc(fontp->TeXnumber & 0xff, fout);
    (void)fwrite(fontp->info, sizeof(char), 14, fout);
    (void)fwrite(fontp->fontname, sizeof(char), fontp->info[12] + fontp->info[13], fout);
    (*fout_pos) += 2 + 14 + fontp->info[12] + fontp->info[13];
}

/*
  Invoked by spcl_scan; saves file names referenced by special commands
  into the `warn_files' field of `info'.
*/
static Boolean
scan_for_included_files(char *special, int str_len, void *my_info)
{
    struct select_pages_info *info = (struct select_pages_info *)my_info;
    UNUSED(str_len);
    if (info->warn_files.items == NULL) /* no info needed, e.g. when printing */
	return True;
    
    while (*special == ' ' || *special == '\t')
	++special;

    if (memicmp(special, "psfile=", strlen("psfile=")) == 0) {
	char *b_ptr = special + strlen("psfile=");
	char *e_ptr;
	
	while (*b_ptr == '"')
	    b_ptr++;
	
	if ((e_ptr = strchr(b_ptr, '"')) != NULL) {
	    size_t len = e_ptr - b_ptr;
	    char *tmp = xmalloc(len + 1);
	    memcpy(tmp, b_ptr, len);
	    tmp[len] = '\0';
	    if (!stack_contains_item(&(info->warn_files), tmp)) {
		push_stack(&(info->warn_files), tmp);
	    }
	    free(tmp);
	}
    }
    
    return True; /* dummy */
}

/*
  Invoked by spcl_scan; saves hyperref and color commands
  into the global stacks `color_stack' and `hyperref_stack'.
  FIXME: In reality char *special is treated as const, but the declaration
  of spcl_scan doesn't allow for this.
*/
static Boolean
stack_save(char *special, int str_len, void *data)
{
    char *ptr;
    size_t len;

    UNUSED(data);
    UNUSED(str_len);
    
    while (isspace((int)*special))
	++special;
    
    ptr = special;
    
    if (memicmp(special, "color ", len = strlen("color ")) == 0) {
	special += len;
	while (*special == ' ' || *special == '\t')
	    ++special;
	if (memicmp(special, "push ", 5) == 0) {
	    push_stack(&color_stack, ptr);
	}
	else if (memicmp(special, "pop", 3) == 0) {
	    pop_stack(&color_stack);
	}
    }
    else if (memicmp(special, "html:", len = strlen("html:")) == 0) {
	/* 	fprintf(stderr, "++++ special: %s; fd: %ld\n", special + 5, ftell(globals.dvi_file.bak_fp)); */
	special += len;
	if (memicmp(special, "<a href=", 8) == 0
	    || memicmp(special, "<a name=", 8) == 0) {
	    push_stack(&href_stack, ptr);
	}
	else if (memicmp(special, "</a>", 4) == 0) {
	    pop_stack(&href_stack);
	}
    }
    else if (memicmp(special, "papersize", len = strlen("papersize")) == 0) {
	free(dvips_papersize_special);
	dvips_papersize_special = xstrdup(special);
    }
    else  if (*special == '!'
	      || memcmp(special, "header", strlen("header")) == 0) {
	size_t idx = literal_headers.size++;
	if (globals.debug & DBG_GUI)
	    fprintf(stderr, "(literal) header %lu: |%s|\n", (unsigned long)idx, special);
	literal_headers.headers = xrealloc(literal_headers.headers,
					   literal_headers.size * sizeof *(literal_headers.headers));
	literal_headers.headers[idx] = xstrdup(special);
    }
    return True; /* dummy */
}

/* write a special to the FILE *fp, updating offset as appropriate. */
static void
write_special(const char *str, FILE *fp, long *offset)
{
    unsigned int len = strlen(str);
    if (len < 256) {
	unsigned char c = len;
	putc(XXX1, fp);
	putc(c, fp);
	*offset += 2;
    }
    else { /* how about xxx2, xxx3? It seems that TeX doesn't use them either? */
	putc(XXX4, fp);
	PutFour(len, fp);
	*offset += 5;
    }
    fputs(str, fp);
    *offset += len;
}

/*
 * Dump the headers in the global list `literal_headers',
 * and free up resources
 */
static void
dump_literal_headers(FILE *fp, long *pos)
{
    size_t i;
    for (i = 0; i < literal_headers.size; i++) {
	write_special(literal_headers.headers[i], fp, pos);
	free(literal_headers.headers[i]);
    }
    free(literal_headers.headers);
    literal_headers.headers = NULL;
    literal_headers.size = 0;
}


static void
scan_page_for_specials(FILE *fp, int page, Boolean save_stack,
		       Boolean (*special_proc)(char *str, int str_len, void *data),
		       struct select_pages_info *info)
{
    off_t pos_save;
    static ubyte my_scan_buffer[DVI_BUFFER_LEN];

    struct drawinf currinf_save;
    ubyte maxchar_save;
    size_t i;
    TRACE_GUI((stderr, "parsing page: %d", page));
    if (save_stack) {
	/*
	  first, copy the current stacks (which reflect the state on the
	  preceding page) to color_save_stack and href_save_stack:
	*/
	empty_stack(&color_save_stack);
	empty_stack(&href_save_stack);
	for (i = 0; i < color_stack.stack_len; i++) {
	    TRACE_GUI((stderr, "saving stack: |%s|", color_stack.items[i].content));
	    push_stack(&color_save_stack, color_stack.items[i].content);
	}
	for (i = 0; i < href_stack.stack_len; i++) {
	    TRACE_GUI((stderr, "saving stack: |%s|", href_stack.items[i].content));
	    push_stack(&href_save_stack, href_stack.items[i].content);
	}
    }
    
    /*     (void)fseek(fp, pageinfo_get_offset(page), SEEK_SET); */
    /*
      The datastructures in dvi-draw are rather weird in requiring
      us to provide a buffer, and point the global currinf.pos and end
      pointers to that buffer, for spcl_scan to be able to scan the file.
      First save the contents of the exising currinf, pointing to the main file:
    */
    pos_save = save_file_status(fp, &currinf_save, &maxchar_save);

    lseek(fileno(fp), pageinfo_get_offset(page), SEEK_SET);
    memset((char *)&currinf.data, 0, sizeof currinf.data);
    currinf.tn_table_len = TNTABLELEN;
    currinf.tn_table = tn_table;
    currinf.tn_head = tn_head;

    /* then point currinf to our own buffer: */
    G_dvi_buf_ptr = my_scan_buffer;
    currinf.pos = currinf.end = G_dvi_buf_ptr;
    lseek(fileno(fp), pageinfo_get_offset(page), SEEK_SET);
    /*
      finally we may invoke spcl_scan(). I hope we can do without
      the
      (!setjmp(some_env))
      black magic since there shouldn't be any interruptions ...
    */
#if 1    
    spcl_scan(special_proc, info, False, fp);
#else
    /* this is an attempt at using setjmp(), but it's just too ugly ... */
    for (;;) {
	int page_bak;
	if (read_events(EV_NOWAIT) & EV_GE_NEWPAGE)
	    break;
	page_bak = page;
	if (!setjmp(globals.ev.canit)) {
	    fprintf(stderr, "current position: %lu; seeking to: %lu\n",
		    (unsigned long)pos_save,
		    (unsigned long)pageinfo_get_offset(page));
	    (void) fprintf(stderr, "seeking to offset %ld, page %d of file %d\n",
			   pageinfo_get_offset(page),
			   page, fileno(globals.dvi_file.bak_fp));
	    fseek(fp, pageinfo_get_offset(page), SEEK_SET);
	    spcl_scan(special_proc, info, False);
	}
	else { /* if interrupted */
	    fprintf(stderr, "========= interrupted!\n");
	    if (page >= page_bak)
		page = page_bak;
	    break;
	}
	if (page >= current_page)
	    break;
	page++;
    }
#endif
    /* now we restore those pesky globals, hoping that nobody
       has stamped upon them in the meantime. */
    restore_file_status(fp, currinf_save, maxchar_save, pos_save);

    /* reposition file pointer */
    (void)fseek(fp, pageinfo_get_offset(page), SEEK_SET);
}


/* dump the `open' commands on the current stack to the FILE *fp */
static void
stack_dump_open_commands(FILE *fp, long *pos)
{
    size_t i;
    TRACE_GUI((stderr, "length of stacks: %lu, %lu",
	       (unsigned long)color_save_stack.stack_len,
	       (unsigned long)href_save_stack.stack_len));
    for (i = 0; i < color_save_stack.stack_len; i++) {
	TRACE_GUI((stderr, "dumping: |%s|", color_save_stack.items[i].content));
	write_special(color_save_stack.items[i].content, fp, pos);
    }
    for (i = 0; i < href_save_stack.stack_len; i++) {
	TRACE_GUI((stderr, "dumping: |%s|", href_save_stack.items[i].content));
	write_special(href_save_stack.items[i].content, fp, pos);
    }
    if (dvips_papersize_special != NULL)
	write_special(dvips_papersize_special, fp, pos);
    TRACE_GUI((stderr, "end of dumping open\n"));
}

/* dump `close' commands for all the `open' commands on the
   current stack (not the saved one, but the one active at the end of the page)
   to the FILE *fp (closing tags for hyperref anchors,
   `pop' commands for color specials).
*/
static void
stack_dump_close_commands(FILE *fp, long *pos)
{
    size_t i;

    for (i = color_stack.stack_len; i > 0; i--) {
	TRACE_GUI((stderr, "===== color pop"));
	write_special("color pop", fp, pos);
    }
    for (i = href_stack.stack_len; i > 0; i--) {
	TRACE_GUI((stderr, "===== html:</a>"));
	write_special("html:</a>", fp, pos);
    }
}


static void
WriteDVI(FILE *fin, FILE *fout, long *fout_pos, int c)
{
    int i, n;
    struct fontinfo *fontp;

    if (c >= FNTNUM0 && c <= FNT4) {
	if (c >= FNT1)
	    n = get_bytes(fin, c - FNT1 + 1);
	else
	    n = c - FNTNUM0;
	for (fontp = fontinfo_head; fontp; fontp = fontp->next)
	    if (n == fontp->TeXnumber)
		break;
	if (fontp && fontp->used == False) {
	    fontp->used = True;
	    FontWrite(fout, fontp, fout_pos);
	}
	putc(c, fout);
	(*fout_pos)++;
	switch (c) {
	case FNT4:
	    putc((n >> 24) & 0xff, fout);
	    (*fout_pos)++;
	case FNT3:
	    putc((n >> 16) & 0xff, fout);
	    (*fout_pos)++;
	case FNT2:
	    putc((n >> 8) & 0xff, fout);
	    (*fout_pos)++;
	case FNT1:
	    putc(n & 0xff, fout);
	    (*fout_pos)++;
	default:
	    break;
	}
    }
    else if (c >= FNTDEF1 && c <= FNTDEF4) {
	n = get_bytes(fin, c - FNTDEF1 + 1);
	for (fontp = fontinfo_head; fontp; fontp = fontp->next)
	    if (n == fontp->TeXnumber)
		break;
	if (fontp && fontp->used == False) {
	    fontp->used = True;
	    FontWrite(fout, fontp, fout_pos);
	}
	(void) get_bytes(fin, 12);
	(void) get_bytes(fin, (int)(get_byte(fin) + get_byte(fin)));
    }
    else {
	putc(c, fout);
	(*fout_pos)++;
	n = 0;
	if (c <= XXX4) {
	    for (i = 0; i < c - XXX1 + 1; i++) {
		int x = get_byte(fin);
		putc(x, fout);
		(*fout_pos)++;
		n = (n << 8) | x;
	    }
	}
	switch (c) {
	case SETRULE:
	case PUTRULE:
	    n += 4;
	    /* fall through */
	    
	case RIGHT4:
	case W4:
	case X4:
	case DOWN4:
	case Y4:
	case Z4:
	    n++;
	    /* fall through */

	case RIGHT3:
	case W3:
	case X3:
	case DOWN3:
	case Y3:
	case Z3:
	    n++;
	    /* fall through */

	case SET2:
	case PUT2: 
	    if (!resource.omega) {
		dvi_fmt_error("%s:%d: WriteDVI: op-code %d only works with the \"-omega\" option",
			      __FILE__, __LINE__, c);
	    }
	case RIGHT2:
	case W2:
	case X2:
	case DOWN2:
	case Y2:
	case Z2:
	    n++;
	    /* fall through */

	case SET1:
	case PUT1:
	case RIGHT1:
	case W1:
	case X1:
	case DOWN1:
	case Y1:
	case Z1:
#ifdef PTEX
	case TDIR:
#endif
	    n++;
	    /* fall through */

	case XXX1:
	case XXX2:
	case XXX3:
	case XXX4:
	    CopyNum(fin, fout, n);
	    (*fout_pos) += n;
	    /* fall through */

	default:
	    break;
	}
    }
}

/* public functions */


/* callback functions for selecting pages */
Boolean
check_pagerange(struct save_or_print_info *info, int page)
{
    return (page >= info->pinfo->from && page <= info->pinfo->to);
}

Boolean
check_marked(struct save_or_print_info *info, int page)
{
    UNUSED(info);
    return pageinfo_is_marked(page);
}

void
select_pages(struct save_or_print_info *info)
{
    struct select_pages_info *pinfo = info->pinfo;
    int c, n, page, pagecnt;
    long fout_pos = 0L;
    unsigned long curr_page_offset = 0xffffffff; /* pattern to be overwritten later */
    unsigned long post_cmd_offset = 0xffffffff; /* pattern to be overwritten later */
    struct fontinfo *fontp;
    FILE *in_fp = info->finfo->in_fp;
    FILE *out_fp = info->finfo->tmp_dvi_fp;
    
    Boolean headers_dumped = False;
    free(dvips_papersize_special); /* re-initialize */
    dvips_papersize_special = NULL;

    ASSERT(in_fp != NULL, "input file mustn't be NULL in select_pages()!");
    ASSERT(out_fp != NULL, "output file mustn't be NULL in select_pages()!");
    
    pinfo->errflag = NO_ERROR; /* reset errflag, in case we're recovering from a previous error */
    
    /* get font list from postamble; in_fp already has been positioned
       at correct position (start of postamble) in caller. */
    (void)fseek(in_fp, ftell(in_fp), SEEK_SET); /* paranoia */
    (void)get_bytes(in_fp, POSTAMBLE_LEN);
    fontinfo_head = NULL;
    for (;;) {
	if ((c = get_byte(in_fp)) < FNTDEF1 || c > FNTDEF4) /* maybe POSTPOST */
	    break;
	fontp = (struct fontinfo *) xmalloc(sizeof(struct fontinfo));
	fontp->TeXnumber = get_bytes(in_fp, c - FNTDEF1 + 1);
	(void)fread(fontp->info, sizeof(char), 14, in_fp);
	n = fontp->info[12] + fontp->info[13];
	fontp->fontname = xmalloc(n);
	(void)fread(fontp->fontname, sizeof(char), n, in_fp);
	fontp->next = fontinfo_head;
	fontinfo_head = fontp;
	fontp->used = False;
    }

    /* preamble */
    fseek(in_fp, 0L, SEEK_SET);
    fout_pos = pageinfo_get_offset(0);
    CopyNum(in_fp, out_fp, fout_pos);

    /* each page */
    pagecnt = 0;
    for (page = 0; page < total_pages; page++) {
	scan_page_for_specials(in_fp, page, True, stack_save, NULL);

	/* should the current page be selected? */
	if (pinfo->callback == NULL || pinfo->callback(info, page)) {
	    scan_page_for_specials(in_fp, page, False, scan_for_included_files, pinfo);

	    /* read BOP except for p[4] */
	    CopyNum(in_fp, out_fp, BOP_PART_LEN);
	    /* read p[4] (previous bop pointer */
	    (void)get_bytes(in_fp, 4);
	    /* write actual value of p[4] */
	    PutFour(curr_page_offset, out_fp);
	    /* remember offset of current page, for postamble */
	    curr_page_offset = fout_pos;
	    /* update fout_pos to current position */
	    fout_pos += BOP_LEN;
	    
	    if (!headers_dumped) {
		headers_dumped = True;
		dump_literal_headers(out_fp, &fout_pos);
	    }

	    /*  	    if (!written_pre_cmds) { */
	    stack_dump_open_commands(out_fp, &fout_pos);
	    /*  		written_pre_cmds = True; */
	    /*  	    } */

	    while ((c = getc(in_fp)) != EOF) {
		if ((c & 0x80) == 0) { /* ordinary character */
		    putc(c, out_fp);
		    fout_pos++;
		}
		else if (c == EOP) { /* End Of Page */
		    /* 		    fprintf(stderr, "EOP at %ld\n", ftell(fin)); */

		    /*  		    if (page == to) {  */ /* print close stack for last page */
		    stack_dump_close_commands(out_fp, &fout_pos);
		    /*  		    } */

		    putc(c, out_fp);
		    fout_pos++;
		    break;
		}
		else {
		    /* 		    fprintf(stderr, "WRITE_DVI before: %ld, in_fp before: %ld\n", fout_pos, ftell(in_fp)); */
		    WriteDVI(in_fp, out_fp, &fout_pos, c);
		    /* 		    fprintf(stderr, "WRITE_DVI after: %ld; in_fp after: %ld\n", fout_pos, ftell(in_fp)); */
		}
		/* HACK alert: force synchronization - why is this neccessary?
		   Seems to have something do with the fact that two FILE *'s on the same
		   file are open. POSIX.1,
		   `Interaction of File Descriptors and Standard I/O Streams' seems to
		   say the seek()s on one of them also affect the other, but I'm not sure
		   about this.
		*/
		fseek(in_fp, 0L, SEEK_CUR);
	    }
	    pagecnt++;
	}
    }

    /* postamble */
    if (!find_postamble(in_fp, &(pinfo->errflag))) {
	return;
    }

    /* read/write POST command */
    putc(get_byte(in_fp), out_fp);
    /* read over last page offset */
    get_bytes(in_fp, 4);
    /* write last page offset */
    PutFour(curr_page_offset, out_fp);
    /* remember start of POST */
    post_cmd_offset = fout_pos;
    /* copy part of postamble */
    CopyNum(in_fp, out_fp, POSTAMBLE_PART_LEN);
    /* read t[2], total number of pages */
    get_bytes(in_fp, 2);
    /* write t[2] (two calls) */
    putc((pagecnt >> 8) & 0xff, out_fp);
    putc(pagecnt & 0xff, out_fp);
    /* update fout_pos to current pos */
    fout_pos += POSTAMBLE_LEN;

    /* output font list */
    for (;;) {
	if ((c = get_byte(in_fp)) < FNTDEF1 || c > FNTDEF4) /* maybe POSTPOST */
	    break;
	n = get_bytes(in_fp, c - FNTDEF1 + 1);
	for (fontp = fontinfo_head; fontp; fontp = fontp->next)
	    if (n == fontp->TeXnumber)
		break;
	if (fontp && fontp->used == True)
	    FontWrite(out_fp, fontp, &fout_pos);
	(void) get_bytes(in_fp, 12);
	(void) get_bytes(in_fp, (int)(get_byte(in_fp) + get_byte(in_fp)));
    }

    /* free list */
    for (fontp = fontinfo_head; fontp;) {
	struct fontinfo *nextp;
	free(fontp->fontname);
	nextp = fontp->next;
	free(fontp);
	fontp = nextp;
    }

    /* POSTPOST */
    putc(c, out_fp);
    get_bytes(in_fp, 4);
    PutFour(post_cmd_offset, out_fp);
    CopyNum(in_fp, out_fp, 1 + 4);
    for (fout_pos += 1 + 4 + 1 + 4;
	 fout_pos & 3;
	 fout_pos++) {
	putc(TRAILER, out_fp);
    }

    /*      fclose(in_fp); */
    fflush(out_fp);
}

