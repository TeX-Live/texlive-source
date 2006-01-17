/*
Copyright (c) 1996-2005 Han The Thanh, <thanh@pdftex.org>

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

$Id: //depot/Build/source.development/TeX/texk/web2c/pdftexdir/mapfile.c#30 $

source code indentation by "indent -kr -nut"
*/

#include <math.h>
#include "ptexlib.h"
#include <kpathsea/c-auto.h>
#include <kpathsea/c-memstr.h>
#include "avlstuff.h"

static const char perforce_id[] =
    "$Id: //depot/Build/source.development/TeX/texk/web2c/pdftexdir/mapfile.c#30 $";

#define FM_BUF_SIZE     1024

static FILE *fm_file;

#define fm_open()       \
    open_input (&fm_file, kpse_fontmap_format, FOPEN_RBIN_MODE)
#define fm_close()      xfclose(fm_file, cur_file_name)
#define fm_getchar()    xgetc(fm_file)
#define fm_eof()        feof(fm_file)

enum _mode { FM_DUPIGNORE, FM_REPLACE, FM_DELETE };
enum _ltype { MAPFILE, MAPLINE };
enum _tfmavail { TFM_UNCHECKED, TFM_FOUND, TFM_NOTFOUND };

typedef struct mitem {
    int mode;                   /* FM_DUPIGNORE or FM_REPLACE or FM_DELETE */
    int type;                   /* map file or map line */
    char *line;                 /* pointer to map file name or map line */
    int lineno;                 /* line number in map file */
    struct mitem *next;         /* pointer to next item, or NULL */
} mapitem;
mapitem *miptr, *mapitems = NULL;

fm_entry *fm_cur;
static const char nontfm[] = "<nontfm>";
static fm_entry *loaded_tfm_found;
static fm_entry *avail_tfm_found;
static fm_entry *non_tfm_found;
static fm_entry *not_avail_tfm_found;

#define read_field(r, q, buf) do {                     \
    for (q = buf; *r != ' ' && *r != 10; *q++ = *r++); \
    *q = 0;                                            \
    skip (r, ' ');                                     \
} while (0)

#define set_field(F) do {                              \
    if (q > buf)                                       \
        fm->F = xstrdup(buf);                          \
    if (*r == 10)                                      \
        goto done;                                     \
} while (0)

static fm_entry *new_fm_entry(void)
{
    fm_entry *fm;
    fm = xtalloc(1, fm_entry);
    fm->tfm_name = NULL;
    fm->ps_name = NULL;
    fm->flags = 0;
    fm->ff_name = NULL;
    fm->subset_tag = NULL;
    fm->encoding = NULL;
    fm->tfm_num = getnullfont();
    fm->tfm_avail = TFM_UNCHECKED;
    fm->type = 0;
    fm->slant = 0;
    fm->extend = 0;
    fm->ff_objnum = 0;
    fm->fn_objnum = 0;
    fm->fd_objnum = 0;
    fm->charset = NULL;
    fm->all_glyphs = false;
    fm->links = 0;
    return fm;
}

static void delete_fm_entry(fm_entry * fm)
{
    xfree(fm->tfm_name);
    xfree(fm->ps_name);
    xfree(fm->ff_name);
    xfree(fm->subset_tag);
    xfree(fm->charset);
    xfree(fm);
}

static ff_entry *new_ff_entry(void)
{
    ff_entry *ff;
    ff = xtalloc(1, ff_entry);
    ff->ff_name = NULL;
    ff->ff_path = NULL;
    return ff;
}

static void delete_ff_entry(ff_entry * ff)
{
    xfree(ff->ff_name);
    xfree(ff->ff_path);
    xfree(ff);
}

static fm_entry *dummy_fm_entry()
{
    static const fm_entry const_fm_entry;
    return (fm_entry *) & const_fm_entry;
}

char *mk_base_tfm(char *tfmname, int *i)
{
    static char buf[SMALL_BUF_SIZE];
    char *p = tfmname, *r = strend(p) - 1, *q = r;
    while (q > p && isdigit(*q))
        --q;
    if (!(q > p) || q == r || (*q != '+' && *q != '-'))
        return NULL;
    check_buf(q - p + 1, SMALL_BUF_SIZE);
    strncpy(buf, p, (unsigned) (q - p));
    buf[q - p] = 0;
    *i = atoi(q);
    return buf;
}

static fmentryptr fmlookup(internalfontnumber);

boolean hasfmentry(internalfontnumber f)
{
    if (pdffontmap[f] == NULL)
        pdffontmap[f] = fmlookup(f);
    assert(pdffontmap[f] != NULL);
    return pdffontmap[f] != (fmentryptr) dummy_fm_entry();
}

/**********************************************************************/

struct avl_table *tfm_tree = NULL;
struct avl_table *ps_tree = NULL;
struct avl_table *ff_tree = NULL;

/* AVL sort fm_entry into tfm_tree by tfm_name */

static int comp_fm_entry_tfm(const void *pa, const void *pb, void *p)
{
    return strcmp(((const fm_entry *) pa)->tfm_name,
                  ((const fm_entry *) pb)->tfm_name);
}

#define cmp_return(a, b) \
    if (a > b)           \
        return 1;        \
    if (a < b)           \
        return -1

/* AVL sort fm_entry into ps_tree by ps_name, slant, and extend */

static int comp_fm_entry_ps(const void *pa, const void *pb, void *p)
{
    const fm_entry *p1 = (const fm_entry *) pa, *p2 =
        (const fm_entry *) pb;
    int i;
    assert(p1->ps_name != NULL && p2->ps_name != NULL);
    if ((i = strcmp(p1->ps_name, p2->ps_name)) != 0)
        return i;
    cmp_return(p1->slant, p2->slant);
    cmp_return(p1->extend, p2->extend);
    if (p1->tfm_name != NULL && p2->tfm_name != NULL &&
        (i = strcmp(p1->tfm_name, p2->tfm_name)) != 0)
        return i;
    return 0;
}

/* AVL sort ff_entry into ff_tree by ff_name */

static int comp_ff_entry(const void *pa, const void *pb, void *p)
{
    return strcmp(((const ff_entry *) pa)->ff_name,
                  ((const ff_entry *) pb)->ff_name);
}

static void create_avl_trees()
{
    if (tfm_tree == NULL) {
        tfm_tree = avl_create(comp_fm_entry_tfm, NULL, &avl_xallocator);
        assert(tfm_tree != NULL);
    }
    if (ps_tree == NULL) {
        ps_tree = avl_create(comp_fm_entry_ps, NULL, &avl_xallocator);
        assert(ps_tree != NULL);
    }
    if (ff_tree == NULL) {
        ff_tree = avl_create(comp_ff_entry, NULL, &avl_xallocator);
        assert(ff_tree != NULL);
    }
}

/*
The function avl_do_entry() is not completely symmetrical with regards
to tfm_name and ps_name handling, e. g. a duplicate tfm_name gives a
"goto exit", and no ps_name link is tried. This is to keep it compatible
with the original version.
*/

static int avl_do_entry(fm_entry * fp, int mode)
{
    fm_entry *p, *p2;
    void *a;
    void **aa;
    struct avl_traverser trav;

    /* handle tfm_name link */

    if (strcmp(fp->tfm_name, nontfm) != 0) {
        p = (fm_entry *) avl_find(tfm_tree, fp);
        if (p != NULL) {
            if (mode == FM_DUPIGNORE) {
                pdftex_warn
                    ("fontmap entry for `%s' already exists, duplicates ignored",
                     fp->tfm_name);
                goto exit;
            } else {            /* mode == FM_REPLACE / FM_DELETE */
                if (fontused[p->tfm_num]) {
                    pdftex_warn
                        ("fontmap entry for `%s' has been used, replace/delete not allowed",
                         fp->tfm_name);
                    goto exit;
                }
                a = avl_delete(tfm_tree, p);
                assert(a != NULL);
                unset_tfmlink(p);
                if (!has_pslink(p))
                    delete_fm_entry(p);
            }
        }
        if (mode != FM_DELETE) {
            aa = avl_probe(tfm_tree, fp);
            assert(aa != NULL);
            set_tfmlink(fp);
        }
    }

    /* handle ps_name link */

    if (fp->ps_name != NULL) {
        assert(fp->tfm_name != NULL);
        p = (fm_entry *) avl_find(ps_tree, fp);
        if (p != NULL) {
            if (mode == FM_DUPIGNORE) {
                pdftex_warn
                    ("ps_name entry for `%s' already exists, duplicates ignored",
                     fp->ps_name);
                goto exit;
            } else {            /* mode == FM_REPLACE / FM_DELETE */
                if (fontused[p->tfm_num]) {
                    /* REPLACE/DELETE not allowed */
                    pdftex_warn
                        ("fontmap entry for `%s' has been used, replace/delete not allowed",
                         p->tfm_name);
                    goto exit;
                }
                a = avl_delete(ps_tree, p);
                assert(a != NULL);
                unset_pslink(p);
                if (!has_tfmlink(p))
                    delete_fm_entry(p);
            }
        }
        if (mode != FM_DELETE) {
            aa = avl_probe(ps_tree, fp);
            assert(aa != NULL);
            set_pslink(fp);
        }
    }
  exit:
    if (!has_tfmlink(fp) && !has_pslink(fp))    /* e. g. after FM_DELETE */
        return 1;               /* deallocation of fm_entry structure required */
    else
        return 0;
}

/**********************************************************************/
/* consistency check for map entry, with warn flag */

int check_fm_entry(fm_entry * fm, boolean warn)
{
    int a = 0;
    assert(fm != NULL);
    if (fm->ps_name != NULL) {
        if (is_basefont(fm)) {
            if (is_fontfile(fm) && !is_included(fm)) {
                if (warn)
                    pdftex_warn
                        ("invalid entry for `%s': font file must be included or omitted for base fonts",
                         fm->tfm_name);
                a += 1;
            }
        } else {                /* not a base font */
            /* if no font file given, drop this entry */
            if (!is_fontfile(fm)) {
                if (warn)
                    pdftex_warn
                        ("invalid entry for `%s': font file missing",
                         fm->tfm_name);
                a += 2;
            }
        }
    }
    if (is_truetype(fm) && is_reencoded(fm) && !is_subsetted(fm)) {
        if (warn)
            pdftex_warn
                ("invalid entry for `%s': only subsetted TrueType font can be reencoded",
                 fm->tfm_name);
        a += 4;
    }
    if ((fm->slant != 0 || fm->extend != 0) &&
        (!is_included(fm) || is_truetype(fm))) {
        if (warn)
            pdftex_warn
                ("invalid entry for `%s': SlantFont/ExtendFont can be used only with embedded T1 fonts",
                 fm->tfm_name);
        a += 8;
    }
    if (abs(fm->slant) > 1000) {
        if (warn)
            pdftex_warn
                ("invalid entry for `%s': too big value of SlantFont (%g)",
                 fm->tfm_name, fm->slant / 1000.0);
        a += 16;
    }
    if (abs(fm->extend) > 2000) {
        if (warn)
            pdftex_warn
                ("invalid entry for `%s': too big value of ExtendFont (%g)",
                 fm->tfm_name, fm->extend / 1000.0);
        a += 32;
    }
    return a;
}

/**********************************************************************/
/* returns true if s is one of the 14 std. font names; speed-trimmed. */

boolean check_basefont(char *s)
{
    static const char *basefont_names[] = {
        "Courier",              /* 0:7 */
        "Courier-Bold",         /* 1:12 */
        "Courier-Oblique",      /* 2:15 */
        "Courier-BoldOblique",  /* 3:19 */
        "Helvetica",            /* 4:9 */
        "Helvetica-Bold",       /* 5:14 */
        "Helvetica-Oblique",    /* 6:17 */
        "Helvetica-BoldOblique",        /* 7:21 */
        "Symbol",               /* 8:6 */
        "Times-Roman",          /* 9:11 */
        "Times-Bold",           /* 10:10 */
        "Times-Italic",         /* 11:12 */
        "Times-BoldItalic",     /* 12:16 */
        "ZapfDingbats"          /* 13:12 */
    };
    static const int index[] =
        { -1, -1, -1, -1, -1, -1, 8, 0, -1, 4, 10, 9, -1, -1, 5, 2, 12, 6,
        -1, 3, -1, 7
    };
    int n = strlen(s), k = -1;
    if (n > 21)
        return false;
    if (n == 12) {              /* three names have length 12 */
        switch (*s) {
        case 'C':
            k = 1;              /* Courier-Bold */
            break;
        case 'T':
            k = 11;             /* Times-Italic */
            break;
        case 'Z':
            k = 13;             /* ZapfDingbats */
            break;
        default:
            return false;
        }
    } else
        k = index[n];
    if (k > -1 && strcmp(basefont_names[k], s) == 0)
        return true;
    return false;
};

/**********************************************************************/

static void fm_scan_line(mapitem * mitem)
{
    int a, b, c, j, u = 0, v = 0;
    float d;
    char fm_line[FM_BUF_SIZE], buf[FM_BUF_SIZE];
    fm_entry *fm;
    char *p, *q, *r, *s;
    p = fm_line;
    switch (mitem->type) {
    case MAPFILE:
        do {
            c = fm_getchar();
            append_char_to_buf(c, p, fm_line, FM_BUF_SIZE);
        }
        while (c != 10);
        break;
    case MAPLINE:
        s = mitem->line;
        while ((c = *s++) != 0)
            append_char_to_buf(c, p, fm_line, FM_BUF_SIZE);
        break;
    default:
        assert(0);
    }
    append_eol(p, fm_line, FM_BUF_SIZE);
    if (is_cfg_comment(*fm_line))
        return;
    r = fm_line;
    read_field(r, q, buf);
    fm = new_fm_entry();
    set_field(tfm_name);
    p = r;
    read_field(r, q, buf);
    if (*buf != '<' && *buf != '"')
        set_field(ps_name);
    else
        r = p;                  /* unget the field */
    if (isdigit(*r)) {          /* font flags given */
        fm->flags = atoi(r);
        while (isdigit(*r))
            r++;
    } else
        fm->flags = 4;          /* treat as Symbol font */
    while (1) {                 /* loop through "specials", encoding, font file */
        skip(r, ' ');
        switch (*r) {
        case 10:
            goto done;
        case '"':               /* opening quote */
            r++;
            u = v = 0;
            do {
                skip(r, ' ');
                if (sscanf(r, "%f %n", &d, &j) > 0) {
                    s = r + j;  /* jump behind number, eat also blanks, if any */
                    if (*(s - 1) == 'E' || *(s - 1) == 'e')
                        s--;    /* e. g. 0.5ExtendFont: %f = 0.5E */
                    if (strncmp(s, "SlantFont", strlen("SlantFont")) == 0) {
                        d *= 1000.0;    /* correct rounding also for neg. numbers */
                        fm->slant = (integer) (d > 0 ? d + 0.5 : d - 0.5);
                        r = s + strlen("SlantFont");
                    } else
                        if (strncmp(s, "ExtendFont", strlen("ExtendFont"))
                            == 0) {
                        d *= 1000.0;
                        fm->extend = (integer) (d > 0 ? d + 0.5 : d - 0.5);
                        if (fm->extend == 1000)
                            fm->extend = 0;
                        r = s + strlen("ExtendFont");
                    } else {    /* unknown name */
                        for (r = s; *r != ' ' && *r != '"' && *r != 10; r++);   /* jump over name */
                        c = *r; /* remember char for temporary end of string */
                        *r = '\0';
                        pdftex_warn
                            ("invalid entry for `%s': unknown name `%s' ignored",
                             fm->tfm_name, s);
                        *r = c;
                    }
                } else
                    for (; *r != ' ' && *r != '"' && *r != 10; r++);
            }
            while (*r == ' ');
            if (*r == '"')      /* closing quote */
                r++;
            else {
                pdftex_warn
                    ("invalid entry for `%s': closing quote missing",
                     fm->tfm_name);
                goto bad_line;
            }
            break;
        default:                /* encoding or font file specification */
            a = b = 0;
            if (*r == '<') {
                a = *r++;
                if (*r == '<' || *r == '[')
                    b = *r++;
            }
            read_field(r, q, buf);
            /* encoding, formats: '8r.enc' or '<8r.enc' or '<[8r.enc' */
            if (strlen(buf) > 4
                && strcasecmp(strend(buf) - 4, ".enc") == 0) {
                fm->encoding = add_enc(buf);
                u = v = 0;      /* u, v used if intervening blank: "<< foo" */
            } else if (strlen(buf) > 0) {       /* file name given */
                /* font file, formats:
                 * subsetting:    '<cmr10.pfa'
                 * no subsetting: '<<cmr10.pfa'
                 * no embedding:  'cmr10.pfa'
                 */
                if (a == '<' || u == '<') {
                    set_included(fm);
                    if ((a == '<' && b == 0) || (a == 0 && v == 0))
                        set_subsetted(fm);
                    /* otherwise b == '<' (or '[') => no subsetting */
                }
                set_field(ff_name);
                u = v = 0;
            } else {
                u = a;
                v = b;
            }
        }
    }
  done:
    if (fm->ps_name != NULL && check_basefont(fm->ps_name))
        set_basefont(fm);
    if (is_fontfile(fm)
        && strcasecmp(strend(fm_fontfile(fm)) - 4, ".ttf") == 0)
        set_truetype(fm);
    if (check_fm_entry(fm, true) != 0)
        goto bad_line;
    /*
       Until here the map line has been completely scanned without errors;
       fm points to a valid, freshly filled-out fm_entry structure.
       Now follows the actual work of registering/deleting.
     */
    if (avl_do_entry(fm, mitem->mode) == 0)     /* if success */
        return;
  bad_line:
    delete_fm_entry(fm);
}

/**********************************************************************/

void fm_read_info()
{
    mapitem *tmp;
    create_avl_trees();
    while (mapitems != NULL) {
        assert(mapitems->line != NULL);
        mapitems->lineno = 1;
        switch (mapitems->type) {
        case MAPFILE:
            set_cur_file_name(mapitems->line);
            if (!fm_open()) {
                pdftex_warn("cannot open font map file");
            } else {
                cur_file_name = (char *) nameoffile + 1;
                tex_printf("{%s", cur_file_name);
                while (!fm_eof()) {
                    fm_scan_line(mapitems);
                    mapitems->lineno++;
                }
                fm_close();
                tex_printf("}");
                fm_file = NULL;
            }
            break;
        case MAPLINE:
            cur_file_name = NULL;       /* makes pdftex_warn() shorter */
            fm_scan_line(mapitems);
            break;
        default:
            assert(0);
        }
        tmp = mapitems;
        mapitems = mapitems->next;
        xfree(tmp->line);
        xfree(tmp);
    }
    cur_file_name = NULL;
    return;
}

/**********************************************************************/

/*
char *mk_exname(char *basename, int e)
{
    static char buf[SMALL_BUF_SIZE];
    char *p = basename, *r;
    if ((r = strrchr(p, '.')) == NULL)
        r = strend(p);
    check_buf(r - p + strlen(r) + 10, SMALL_BUF_SIZE);
    strncpy(buf, p, (unsigned) (r - p));
    sprintf(buf + (r - p), "%+i", e);
    strcat(buf, r);
    return buf;
}
*/

internalfontnumber tfmoffm(fmentryptr fm_pt)
{
    return ((fm_entry *) fm_pt)->tfm_num;
}

static fm_entry *mk_ex_fm(internalfontnumber f, fm_entry * basefm, int ex)
{
    fm_entry *fm;
    integer e = basefm->extend;
    if (e == 0)
        e = 1000;
    fm = new_fm_entry();
    fm->flags = basefm->flags;
    fm->encoding = basefm->encoding;
    fm->type = basefm->type;
    fm->slant = basefm->slant;
    fm->extend = roundxnoverd(e, 1000 + ex, 1000);      /* modify ExtentFont to simulate expansion */
    if (fm->extend == 1000)
        fm->extend = 0;
    fm->tfm_name = xstrdup(makecstring(fontname[f]));
    if (basefm->ps_name != NULL)
        fm->ps_name = xstrdup(basefm->ps_name);
    fm->ff_name = xstrdup(basefm->ff_name);
    fm->ff_objnum = pdfnewobjnum();
    fm->tfm_num = f;
    fm->tfm_avail = TFM_FOUND;
    assert(strcmp(fm->tfm_name, nontfm) != 0);
    return fm;
}

static void init_fm(fm_entry * fm, internalfontnumber f)
{
    if (fm->fd_objnum == 0 && !no_font_desc(fm))
        fm->fd_objnum = pdfnewobjnum();
    if (fm->ff_objnum == 0 && is_included(fm))
        fm->ff_objnum = pdfnewobjnum();
    if (fm->tfm_num == getnullfont()) {
        fm->tfm_num = f;
        fm->tfm_avail = TFM_FOUND;
    }
}

static fmentryptr fmlookup(internalfontnumber f)
{
    char *tfm, *p;
    fm_entry *fm, *exfm;
    fm_entry tmp;
    int ai, e;
    if (tfm_tree == NULL || mapitems != NULL)
        fm_read_info();
    tfm = makecstring(fontname[f]);
    assert(strcmp(tfm, nontfm) != 0);

    /* Look up for full <tfmname>[+-]<expand> */
    tmp.tfm_name = tfm;
    fm = (fm_entry *) avl_find(tfm_tree, &tmp);
    if (fm != NULL) {
        init_fm(fm, f);
        return (fmentryptr) fm;
    }
    tfm = mk_base_tfm(makecstring(fontname[f]), &e);
    if (tfm == NULL)            /* not an expanded font, nothing to do */
        return (fmentryptr) dummy_fm_entry();

    tmp.tfm_name = tfm;
    fm = (fm_entry *) avl_find(tfm_tree, &tmp);
    if (fm != NULL) {           /* found an entry with the base tfm name, e.g. cmr10 */
        if (!is_t1fontfile(fm) || !is_included(fm)) {
            pdftex_warn
                ("font %s cannot be expanded (not an included Type1 font)",
                 tfm);
            return (fmentryptr) dummy_fm_entry();
        }
        exfm = mk_ex_fm(f, fm, e);      /* copies all fields from fm except tfm name */
        init_fm(exfm, f);
        ai = avl_do_entry(exfm, FM_DUPIGNORE);
        assert(ai == 0);
        return (fmentryptr) exfm;
    }
    return (fmentryptr) dummy_fm_entry();
}

/*
   Early check whether a font file exists. Used e. g. for replacing fonts
   of embedded PDF files: Without font file, the font within the embedded
   PDF-file is used. Search tree ff_tree is used in 1st instance, as it
   may be faster than the kpse_find_file(), and kpse_find_file() is called
   only once per font file name + expansion parameter. This might help
   keeping speed, if many PDF pages with same fonts are to be embedded.

   The ff_tree contains only font files, which are actually needed,
   so this tree typically is much smaller than the tfm_tree or ps_tree.
*/

ff_entry *check_ff_exist(fm_entry * fm)
{
    ff_entry *ff;
    ff_entry tmp;
    char *ex_ffname;
    void **aa;

    assert(fm->ff_name != NULL);
    tmp.ff_name = fm->ff_name;
    ff = (ff_entry *) avl_find(ff_tree, &tmp);
    if (ff == NULL) {           /* not yet in database */
        ff = new_ff_entry();
        ff->ff_name = xstrdup(fm->ff_name);
        if (is_truetype(fm))
            ff->ff_path =
                kpse_find_file(fm->ff_name, kpse_truetype_format, 0);
        else
            ff->ff_path =
                kpse_find_file(fm->ff_name, kpse_type1_format, 0);
        aa = avl_probe(ff_tree, ff);
        assert(aa != NULL);
    }
    return ff;
}

/**********************************************************************/

static boolean used_tfm(fm_entry * p)
{
    internalfontnumber f;
    strnumber s;
    ff_entry *ff;

    /* check if the font file is not a TrueType font */
    /* font replacement makes sense only for included Type1 files */
    if (is_truetype(p) || !is_included(p))
        return false;

    /* check if the font file is available */
    ff = check_ff_exist(p);
    if (ff->ff_path == NULL)
        return false;

    /* check whether this font has been used */
    if (fontused[p->tfm_num])
        return true;
    assert(p->tfm_name != NULL);

    /* check whether we didn't find a loaded font yet,
     * and this font has been loaded */
    if (loaded_tfm_found == NULL && strcmp(p->tfm_name, nontfm) != 0) {
        s = maketexstring(p->tfm_name);
        if ((f = tfmlookup(s, 0)) != getnullfont()) {
            loaded_tfm_found = p;
            if (pdffontmap[f] == NULL)
                pdffontmap[f] = (fmentryptr) p;
            if (p->tfm_num == getnullfont())
                p->tfm_num = f;
            assert(p->tfm_num == f);
            /* don't call flushstr() here as it has been called by tfmlookup() */
        } else
            flushstr(s);
    }

    /* check whether we didn't find either a loaded or a loadable font yet,
     * and this font is loadable */
    if (avail_tfm_found == NULL && loaded_tfm_found == NULL &&
        strcmp(p->tfm_name, nontfm) != 0) {
        if (p->tfm_avail == TFM_UNCHECKED) {
            if (kpse_find_file(p->tfm_name, kpse_tfm_format, 0) != NULL) {
                avail_tfm_found = p;
                p->tfm_avail = TFM_FOUND;
            } else {
                p->tfm_avail = TFM_NOTFOUND;
                if (not_avail_tfm_found == NULL)
                    not_avail_tfm_found = p;
            }
        }
    }

    /* check whether the current entry is a <nontfm> entry */
    if (non_tfm_found == NULL && strcmp(p->tfm_name, nontfm) == 0)
        non_tfm_found = p;

    return false;
}

/* lookup_ps_name looks for an entry with a given ps name + slant + extend;
 * return NULL if not found.
 *
 * As there may exist several such entries, we need to select the `right'
 * one. We do so by checking all such entries and return the first one that
 * fulfils the following criteria (in descending priority):
 *
 * - the tfm has been used (some char from this font has been typeset)
 * - the tfm has been loaded (but not used yet)
 * - the tfm can be loaded (but not loaded yet)
 * - the tfm is present in map files, but cannot be loaded. In this case a
 *   dummy tfm can be loaded instead, and a warning should be written out
 */

static fm_entry *lookup_ps_name(fm_entry * fm)
{
    fm_entry *p, *p2;
    struct avl_traverser t, t2;
    strnumber s;
    int a;

    loaded_tfm_found = NULL;
    avail_tfm_found = NULL;
    non_tfm_found = NULL;
    not_avail_tfm_found = NULL;

    assert(fm->tfm_name == NULL);
    p = (fm_entry *) avl_t_find(&t, ps_tree, fm);
    if (p == NULL)
        return NULL;
    t2 = t;
    p2 = avl_t_prev(&t2);

    /* search forward */
    do {
        if (used_tfm(p))
            return p;
        p = avl_t_next(&t);
    }
    while (p != NULL && comp_fm_entry_ps(fm, p, NULL) == 0);

    /* search backward */
    while (p2 != NULL && comp_fm_entry_ps(fm, p2, NULL) == 0) {
        if (used_tfm(p2))
            return p2;
        p2 = avl_t_prev(&t2);
    }

    if (loaded_tfm_found != NULL)
        p = loaded_tfm_found;
    else if (avail_tfm_found != NULL) {
        p = avail_tfm_found;
        p->tfm_num = readfontinfo(getnullcs(), maketexstring(p->tfm_name),
                                  getnullstr(), -1000);
        p->tfm_avail = TFM_FOUND;
    } else if (non_tfm_found != NULL) {
        p = non_tfm_found;
        p->tfm_num = newdummyfont();
        p->tfm_avail = TFM_FOUND;
    } else if (not_avail_tfm_found != NULL) {
        p = not_avail_tfm_found;
        pdftex_warn("`%s' not loadable, use a dummy tfm instead",
                    p->tfm_name);
        p2 = new_fm_entry();
        p2->flags = p->flags;
        p2->encoding = p->encoding;
        p2->type = p->type;
        p2->slant = p->slant;
        p2->extend = p->extend;
        p2->tfm_name = xstrdup(nontfm);
        p2->ps_name = xstrdup(p->ps_name);
        if (p->ff_name != NULL)
            p2->ff_name = xstrdup(p->ff_name);
        p2->tfm_num = newdummyfont();
        p2->tfm_avail = TFM_FOUND;
        a = avl_do_entry(p2, FM_DUPIGNORE);
        assert(a == 0);
        p = p2;
    } else
        return NULL;
    assert(p->tfm_num != getnullfont());
    return p;
}

/* Lookup fontmap for /BaseFont entries of embedded PDF-files;
 * return dummy_fm_entry() if not found */

fm_entry *lookup_fontmap(char *bname)
{
    fm_entry *fm, *fmx;
    fm_entry tmp, tmpx;
    ff_entry *ff;
    char buf[SMALL_BUF_SIZE];
    char *a, *b, *c, *d, *e, *s;
    strnumber str;
    int i, sl, ex, ai;
    if (tfm_tree == NULL || mapitems != NULL)
        fm_read_info();
    if (bname == NULL)
        return NULL;
    if (strlen(bname) >= SMALL_BUF_SIZE)
        pdftex_fail("Font name too long: `%s'", bname);
    strcpy(buf, bname);         /* keep bname untouched for later */
    s = buf;
    if (strlen(buf) > 7) {      /* check for subsetted name tag */
        for (i = 0; i < 6; i++, s++)
            if (*s < 'A' || *s > 'Z')
                break;
        if (i == 6 && *s == '+')
            s++;                /* if name tag found, skip behind it */
        else
            s = buf;
    }

    /*
       Scan -Slant_<slant> and -Extend_<extend> font name extensions;
       three valid formats:
       <fontname>-Slant_<slant>
       <fontname>-Slant_<slant>-Extend_<extend>
       <fontname>-Extend_<extend>
       Slant entry must come _before_ Extend entry
     */

    tmp.slant = 0;
    tmp.extend = 0;
    if ((a = strstr(s, "-Slant_")) != NULL) {
        b = a + strlen("-Slant_");
        sl = (int) strtol(b, &e, 10);
        if ((e != b) && (e == strend(b))) {
            tmp.slant = sl;
            *a = 0;             /* bname string ends before "-Slant_" */
        } else {
            if (e != b) {       /* only if <slant> is valid number */
                if ((c = strstr(e, "-Extend_")) != NULL) {
                    d = c + strlen("-Extend_");
                    ex = (int) strtol(d, &e, 10);
                    if ((e != d) && (e == strend(d))) {
                        tmp.slant = sl;
                        tmp.extend = ex;
                        *a = 0; /* bname string ends before "-Slant_" */
                    }
                }
            }
        }
    } else {
        if ((a = strstr(s, "-Extend_")) != NULL) {
            b = a + strlen("-Extend_");
            ex = (int) strtol(b, &e, 10);
            if ((e != b) && (e == strend(b))) {
                tmp.extend = ex;
                *a = 0;         /* bname string ends before "-Extend_" */
            }
        }
    }
    tmp.ps_name = s;
    tmp.tfm_name = NULL;
    fm = lookup_ps_name(&tmp);
    if (fm != NULL) {
        if (!(is_type1(fm) && is_included(fm))) {
            pdftex_warn
                ("PS name '%s' is found but ignored (not an included Type1 font)",
                 tmp.ps_name);
            return NULL;
        }
        i = fm->tfm_num;
        assert(i != getnullfont());
        if (pdffontmap[i] == NULL)
            pdffontmap[i] = (fmentryptr) fm;
        if (fm->ff_objnum == 0 && is_included(fm))
            fm->ff_objnum = pdfnewobjnum();
        if (!fontused[i])
            pdfinitfont(i);
        return fm;
    }
/*
   The following code snipplet handles fonts with "Slant" and "Extend"
   name extensions in embedded PDF files, which don't yet have an
   fm_entry. If such a font is found (e. g. CMR10-Extend_1020), and no
   fm_entry for this is found in the ps_tree (e. g. ps_name = "CMR10",
   extend = 1020), and if an unextended base font (e. g. CMR10) is found,
   a new <nontfm> fm_entry is created and put into the ps_tree. Then
   the lookup_fontmap() function is (recursively) called again, which
   then should find the new fm_entry. The same can be done manually by
   a map entry e. g.:

   \pdfmapline{+<nontfm> CMR10 "1.02 ExtendFont" <cmr10.pfb}

   This would also match the embedded font CMR10-Extend_1020, and replace
   it by an extended copy of cmr10.pfb. -- But not by an expanded version;
   no MM files (e.g. cmr10+20.pfb) would be used.
*/

    tmpx.ps_name = s;
    tmpx.tfm_name = NULL;
    tmpx.slant = 0;
    tmpx.extend = 0;
/*     fm = (fm_entry *) avl_find(ps_tree, &tmpx); */
    fm = lookup_ps_name(&tmpx);
    if (fm != NULL) {
        if (is_truetype(fm) || !is_included(fm))
            return NULL;
        ff = check_ff_exist(fm);
        if (ff->ff_path == NULL)
            return NULL;
        fmx = new_fm_entry();
        fmx->flags = fm->flags;
        fmx->encoding = fm->encoding;
        fmx->type = fm->type;
        fmx->slant = tmp.slant;
        fmx->extend = tmp.extend;
        fmx->tfm_name = xstrdup(nontfm);
        fmx->ps_name = xstrdup(s);
        fmx->ff_name = xstrdup(fm->ff_name);
        ai = avl_do_entry(fmx, FM_DUPIGNORE);
        assert(ai == 0);
        fm = lookup_fontmap(bname);     /* new try */
        assert(fm != NULL);
        return fm;
    }
    return NULL;
}

/**********************************************************************/
/*
Add map file name or map line contents to the linked list "mapitems".
Items not beginning with [+-=] flush list with pending items. Leading
blanks and blanks immediately following [+-=] are ignored.
*/

char *add_map_item(char *s, int type)
{
    char *p;
    int l;                      /* length of map item (without [+-=]) */
    mapitem *tmp;
    int mode;
    for (; *s == ' '; s++);     /* ignore leading blanks */
    switch (*s) {
    case '+':                   /* +mapfile.map, +mapline */
        mode = FM_DUPIGNORE;    /* insert entry, if it is not duplicate */
        s++;
        break;
    case '=':                   /* =mapfile.map, =mapline */
        mode = FM_REPLACE;      /* try to replace earlier entry */
        s++;
        break;
    case '-':                   /* -mapfile.map, -mapline */
        mode = FM_DELETE;       /* try to delete entry */
        s++;
        break;
    default:
        mode = FM_DUPIGNORE;    /* also flush pending list */
        while (mapitems != NULL) {
            tmp = mapitems;
            mapitems = mapitems->next;
            xfree(tmp->line);
            xfree(tmp);
        }
    }
    for (; *s == ' '; s++);     /* ignore blanks after [+-=] */
    p = s;                      /* map item starts here */
    switch (type) {             /* find end of map item */
    case MAPFILE:
        for (; *p != 0 && *p != ' ' && *p != 10; p++);
        break;
    case MAPLINE:               /* blanks allowed */
        for (; *p != 0 && *p != 10; p++);
        break;
    default:
        assert(0);
    }
    l = p - s;
    if (l > 0) {                /* only if real item to add */
        tmp = xtalloc(1, mapitem);
        if (mapitems == NULL)
            mapitems = tmp;     /* start new list */
        else
            miptr->next = tmp;
        miptr = tmp;
        miptr->mode = mode;
        miptr->type = type;
        miptr->line = xtalloc(l + 1, char);
        *(miptr->line) = 0;
        strncat(miptr->line, s, l);
        miptr->next = NULL;
    }
    return p;
}

void pdfmapfile(integer t)
{
    add_map_item(makecstring(tokenstostring(t)), MAPFILE);
    flushstr(lasttokensstring);
}

void pdfmapline(integer t)
{
    add_map_item(makecstring(tokenstostring(t)), MAPLINE);
    flushstr(lasttokensstring);
}

void pdfinitmapfile(string map_name)
{
    if (mapitems == NULL) {
        mapitems = xtalloc(1, mapitem);
        miptr = mapitems;
        miptr->mode = FM_DUPIGNORE;
        miptr->type = MAPFILE;
        miptr->line = xstrdup(map_name);
        miptr->next = NULL;
    }
}

/**********************************************************************/
/* cleaning up... */

static void destroy_fm_entry_tfm(void *pa, void *pb)
{
    fm_entry *fm;
    fm = (fm_entry *) pa;
    if (!has_pslink(fm))
        delete_fm_entry(fm);
    else
        unset_tfmlink(fm);
}

static void destroy_fm_entry_ps(void *pa, void *pb)
{
    fm_entry *fm;
    fm = (fm_entry *) pa;
    if (!has_tfmlink(fm))
        delete_fm_entry(fm);
    else
        unset_pslink(fm);
}

static void destroy_ff_entry(void *pa, void *pb)
{
    ff_entry *ff;
    ff = (ff_entry *) pa;
    delete_ff_entry(ff);
}

void fm_free(void)
{
    if (tfm_tree != NULL)
        avl_destroy(tfm_tree, destroy_fm_entry_tfm);
    if (ps_tree != NULL)
        avl_destroy(ps_tree, destroy_fm_entry_ps);
    if (ff_tree != NULL)
        avl_destroy(ff_tree, destroy_ff_entry);
}

/**********************************************************************/
/* end of mapfile.c */
