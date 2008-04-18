/*
Copyright (c) 1996-2006 Han The Thanh, <thanh@pdftex.org>

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

$Id: writefont.c 1139 2008-04-09 14:44:20Z taco $
*/

#include "ptexlib.h"

extern void writetype0 (fd_entry * fd) ;
extern void writetype2 (fd_entry * fd) ;
extern unsigned long cidtogid_obj;

#ifdef DO_TYPE1C
extern void writet1c(fd_entry * fd); /* in writecff.c */
#endif

extern void writet1w   (fd_entry * fd); /* in writecff.c */
extern void writetype1w (fd_entry * fd);
extern integer write_cid_tounicode(fo_entry * fo, internalfontnumber f); /* in tounicode.c */


void write_cid_fontdictionary(fo_entry * fo, internalfontnumber f);
void create_cid_fontdictionary(fm_entry * fm, integer font_objnum, internalfontnumber f);

/**********************************************************************/

struct avl_table *fo_tree = NULL;       /* tree of font dictionaries */
struct avl_table *fd_tree = NULL;       /* tree of font descriptor objects */

static int comp_fo_entry(const void *pa, const void *pb, void *p)
{
    return strcmp(((const fo_entry *) pa)->fm->tfm_name,
                  ((const fo_entry *) pb)->fm->tfm_name);
}

static int comp_fd_entry(const void *pa, const void *pb, void *p)
{
    int i;
    const fd_entry *p1 = (const fd_entry *) pa, *p2 = (const fd_entry *) pb;
    assert(p1->fm != NULL && is_fontfile(p1->fm) &&
           p2->fm != NULL && is_fontfile(p2->fm));
    if ((i = strcmp(p1->fm->ff_name, p2->fm->ff_name)) != 0)
        return i;
    cmp_return(p1->fm->slant, p2->fm->slant);
    cmp_return(p1->fm->extend, p2->fm->extend);
    return 0;
}

/**********************************************************************/
/* initialize data structure for /Type /Font */

fo_entry *new_fo_entry(void)
{
    fo_entry *fo;
    fo = xtalloc(1, fo_entry);
    fo->fo_objnum = 0;
    fo->tex_font = 0;
    fo->fm = NULL;
    fo->fd = NULL;
    fo->fe = NULL;
    fo->cw_objnum = 0;
    fo->first_char = 1;
    fo->last_char = 0;
    fo->tx_tree = NULL;
    fo->tounicode_objnum = 0;
    return fo;
}

/* initialize data structure for /Type /FontDescriptor */

fd_entry *new_fd_entry(void)
{
    fd_entry *fd;
    int i;
    fd = xtalloc(1, fd_entry);
    fd->fd_objnum = 0;
    fd->fontname = NULL;
    fd->subset_tag = NULL;
    fd->ff_found = false;
    fd->ff_objnum = 0;
    fd->fn_objnum = 0;
    fd->all_glyphs = false;
    fd->write_ttf_glyph_names = false;
    for (i = 0; i < INT_KEYS_NUM; i++) {
        fd->font_dim[i].val = 0;
        fd->font_dim[i].set = false;
    }
    fd->fe = NULL;
    fd->builtin_glyph_names = NULL;
    fd->fm = NULL;
    fd->tx_tree = NULL;
    fd->gl_tree = NULL;
    return fd;
}

/**********************************************************************/
/*
 * Only fallback values of font metrics are taken from the TFM info
 * of f by preset_fontmetrics(). During reading of the font file,
 * these values are replaced by metrics from the font, if available.
 */

static void preset_fontmetrics(fd_entry * fd, internalfontnumber f)
{
    int i;
    fd->font_dim[ITALIC_ANGLE_CODE].val =
        divide_scaled(-atan(get_slant(f) / 65536.0) * (180 / M_PI),
					  pdf_font_size(f), 3);
    fd->font_dim[ASCENT_CODE].val =
	  divide_scaled(char_height(f, 'h'), pdf_font_size(f), 3);
    fd->font_dim[CAPHEIGHT_CODE].val =
	  divide_scaled(char_height(f, 'H'), pdf_font_size(f), 3);
    i = -divide_scaled(char_depth(f, 'y'), pdf_font_size(f), 3);
    fd->font_dim[DESCENT_CODE].val = i < 0 ? i : 0;
    fd->font_dim[STEMV_CODE].val =
	  divide_scaled(char_width(f, '.') / 3, pdf_font_size(f), 3);
    fd->font_dim[XHEIGHT_CODE].val =
	  divide_scaled(get_x_height(f), pdf_font_size(f), 3);
    fd->font_dim[FONTBBOX1_CODE].val = 0;
    fd->font_dim[FONTBBOX2_CODE].val = fd->font_dim[DESCENT_CODE].val;
    fd->font_dim[FONTBBOX3_CODE].val =
	  divide_scaled(get_quad(f), pdf_font_size(f), 3);
    fd->font_dim[FONTBBOX4_CODE].val =
        fd->font_dim[CAPHEIGHT_CODE].val > fd->font_dim[ASCENT_CODE].val ?
        fd->font_dim[CAPHEIGHT_CODE].val : fd->font_dim[ASCENT_CODE].val;
    for (i = 0; i < INT_KEYS_NUM; i++)
        fd->font_dim[i].set = true;
}

static void write_fontmetrics(fd_entry * fd)
{
    int i;
    if (fd->font_dim[FONTBBOX1_CODE].set && fd->font_dim[FONTBBOX2_CODE].set
        && fd->font_dim[FONTBBOX3_CODE].set && fd->font_dim[FONTBBOX4_CODE].set)
        pdf_printf("/%s [%i %i %i %i]\n", font_key[FONTBBOX1_CODE].pdfname,
                   (int) fd->font_dim[FONTBBOX1_CODE].val,
                   (int) fd->font_dim[FONTBBOX2_CODE].val,
                   (int) fd->font_dim[FONTBBOX3_CODE].val,
                   (int) fd->font_dim[FONTBBOX4_CODE].val);
    for (i = 0; i < GEN_KEY_NUM; i++)
        if (fd->font_dim[i].set)
            pdf_printf("/%s %i\n", font_key[i].pdfname, fd->font_dim[i].val);
}

/**********************************************************************/

static void preset_fontname(fo_entry * fo)
{
    if (fo->fm->ps_name != NULL)
        fo->fd->fontname = xstrdup(fo->fm->ps_name);    /* just fallback */
    else
        fo->fd->fontname = xstrdup(fo->fm->tfm_name);
}

static void write_fontname(fd_entry * fd, char *key)
{
    assert(fd->fontname != NULL);
    pdf_puts("/");
    if (key != NULL)
        pdf_printf("%s /", key);
    if (fd->subset_tag != NULL)
        pdf_printf("%s+", fd->subset_tag);
    pdf_printf("%s\n", fd->fontname);
}

static void write_fontname_object(fd_entry * fd)
{
    assert(fd->fn_objnum != 0);
    pdf_begin_obj(fd->fn_objnum, 1);
    write_fontname(fd, NULL);
    pdf_end_obj();
}

/**********************************************************************/

fd_entry *lookup_fd_entry(char *s, integer slant, integer extend)
{
    fd_entry fd;
    fm_entry fm;
    assert(s != NULL);
    fm.ff_name = s;
    fm.slant = slant;
    fm.extend = extend;
    fd.fm = &fm;
    if (fd_tree == NULL) {
        fd_tree = avl_create(comp_fd_entry, NULL, &avl_xallocator);
        assert(fd_tree != NULL);
    }
    return (fd_entry *) avl_find(fd_tree, &fd);
}

fd_entry *lookup_fontdescriptor(fo_entry * fo)
{
    assert(fo != NULL);
    assert(fo->fm != NULL);
    assert(is_fontfile(fo->fm));
    return lookup_fd_entry(fo->fm->ff_name, fo->fm->slant, fo->fm->extend);
}

void register_fd_entry(fd_entry * fd)
{
    void **aa;
    if (fd_tree == NULL) {
        fd_tree = avl_create(comp_fd_entry, NULL, &avl_xallocator);
        assert(fd_tree != NULL);
    }
    assert(fd != NULL && fd->fm != NULL && is_fontfile(fd->fm));
    assert(lookup_fd_entry(fd->fm->ff_name, fd->fm->slant, fd->fm->extend) == NULL);    /* font descriptor not yet registered */
    aa = avl_probe(fd_tree, fd);
    assert(aa != NULL);
}

void create_fontdescriptor(fo_entry * fo, internalfontnumber f)
{
    assert(fo != NULL);
    assert(fo->fm != NULL);
    assert(fo->fd == NULL);
    fo->fd = new_fd_entry();
    preset_fontname(fo);
    preset_fontmetrics(fo->fd, f);
    fo->fd->fe = fo->fe;        /* encoding needed by TrueType writing */
    fo->fd->fm = fo->fm;        /* map entry needed by TrueType writing */
    fo->fd->gl_tree = avl_create(comp_string_entry, NULL, &avl_xallocator);
    assert(fo->fd->gl_tree != NULL);
}

/**********************************************************************/
/*
 * For all used characters of TeX font f, get corresponding glyph names
 * from external reencoding (.enc) file and collect these in the glyph
 * tree gl_tree of font descriptor fd referenced by font dictionary fo.
 */

void mark_reenc_glyphs(fo_entry * fo, internalfontnumber f)
{
    int i;
    char **g;
    void **aa;
    assert(fo->fe != NULL);
    if (is_subsetted(fo->fm)) {
        assert(is_included(fo->fm));
        /* mark glyphs from TeX (externally reencoded characters) */
        g = fo->fe->glyph_names;
        for (i = fo->first_char; i <= fo->last_char; i++) {
            if (pdf_char_marked(f, i) && g[i] != notdef
                && (char *) avl_find(fo->fd->gl_tree, g[i]) == NULL) {
                aa = avl_probe(fo->fd->gl_tree, xstrdup(g[i]));
                assert(aa != NULL);
            }
        }
    }
}

/*
 * Function mark_chars() has 2 uses:
 * 1. Mark characters as chars on TeX level.
 * 2. Mark encoding pairs used by TeX to optimize encoding vector.
 */

struct avl_table *mark_chars(fo_entry * fo, struct avl_table *tx_tree,
                             internalfontnumber f)
{
    int i, *j;
    void **aa;
    if (tx_tree == NULL) {
        tx_tree = avl_create(comp_int_entry, NULL, &avl_xallocator);
        assert(tx_tree != NULL);
    }
    for (i = fo->first_char; i <= fo->last_char; i++) {
        if (pdf_char_marked(f, i) && (int *) avl_find(tx_tree, &i) == NULL) {
            j = xtalloc(1, int);
            *j = i;
            aa = avl_probe(tx_tree, j);
            assert(aa != NULL);
        }
    }
    return tx_tree;
}

/**********************************************************************/

void get_char_range(fo_entry * fo, internalfontnumber f)
{
    int i;
    assert(fo != NULL);
    for (i = font_bc(f); i <= font_ec(f); i++)    /* search for first_char and last_char */
        if (pdf_char_marked(f, i))
            break;
    fo->first_char = i;
    for (i = font_ec(f); i >= font_bc(f); i--)
        if (pdf_char_marked(f, i))
            break;
    fo->last_char = i;
    if ((fo->first_char > fo->last_char)
        || !pdf_char_marked(f, fo->first_char)) { /* no character used from this font */
        fo->last_char = 0;
        fo->first_char = fo->last_char + 1;
    }
}

static void write_charwidth_array(fo_entry * fo, internalfontnumber f)
{
    int i, j, *ip, *fip, width;
    struct avl_traverser t;
    assert(fo->tx_tree != NULL);
    assert(fo->cw_objnum == 0);
    fo->cw_objnum = pdf_new_objnum();
    pdf_begin_obj(fo->cw_objnum, 1);
    avl_t_init(&t, fo->tx_tree);
    fip = (int *) avl_t_first(&t, fo->tx_tree);
    assert(fip != NULL);
    pdf_puts("[");
    for (ip = fip, j = *ip; ip != NULL; ip = (int *) avl_t_next(&t)) {
        if (ip != fip)
            pdf_puts(" ");
        i = *ip;
        while (j < i - 1) {
            pdf_puts("0 ");
            j++;
        }
        j = i;
        width = divide_scaled(char_width(f, i), pdf_font_size(f), 4);
        pdf_printf("%i", (int) width / 10);     /* see adv_char_width() in pdftex.web */
        if ((width = width % 10) != 0)
            pdf_printf(".%i", width);
    }
    pdf_puts("]\n");
    pdf_end_obj();
}

/**********************************************************************/
/*
 * Remark: Font objects from embedded PDF files are never registered
 * into fo_tree; they are individually written out.
 */

fo_entry *lookup_fo_entry(char *s)
{
    fo_entry fo;
    fm_entry fm;
    assert(s != NULL);
    fm.tfm_name = s;
    fo.fm = &fm;
    if (fo_tree == NULL) {
        fo_tree = avl_create(comp_fo_entry, NULL, &avl_xallocator);
        assert(fo_tree != NULL);
    }
    return (fo_entry *) avl_find(fo_tree, &fo);
}

void register_fo_entry(fo_entry * fo)
{
    void **aa;
    if (fo_tree == NULL) {
        fo_tree = avl_create(comp_fo_entry, NULL, &avl_xallocator);
        assert(fo_tree != NULL);
    }
    assert(fo != NULL);
    assert(fo->fm != NULL);
    assert(fo->fm->tfm_name != NULL);
	assert (lookup_fo_entry(fo->fm->tfm_name) == NULL) ;
	aa = avl_probe(fo_tree, fo);
	assert(aa != NULL);
}

/**********************************************************************/

static void write_fontfile(fd_entry * fd)
{
  assert(is_included(fd->fm));
  if (is_cidkeyed(fd->fm)) {
    if (is_opentype(fd->fm))
      writetype0(fd);
    else if (is_truetype(fd->fm))
      writetype2(fd);
    else if (is_type1(fd->fm))
      writetype1w(fd);
    else 
      assert(0);
  } else {
    if (is_type1(fd->fm))
#ifdef DO_TYPE1C
      writet1c(fd);
#else
    writet1(fd);
#endif
    else if (is_truetype(fd->fm))
      writettf(fd);
    else if (is_opentype(fd->fm))
      writeotf(fd);
    else
      assert(0);
  }
  if (!fd->ff_found)
    return;
  assert(fd->ff_objnum == 0);
  fd->ff_objnum = pdf_new_objnum();
  pdf_begin_dict(fd->ff_objnum, 0);     /* font file stream */
  if (is_cidkeyed(fd->fm)) {
    /* No subtype is used for TrueType-based OpenType fonts */
    if (is_opentype(fd->fm) || is_type1(fd->fm))
      pdf_puts("/Subtype /CIDFontType0C\n");
    /* else
       pdf_puts("/Subtype /OpenType\n");*/
  } else {
    if (is_type1(fd->fm))
      pdf_printf("/Length1 %i\n/Length2 %i\n/Length3 %i\n",
		 (int) t1_length1, (int) t1_length2, (int) t1_length3);
    else if (is_truetype(fd->fm))
      pdf_printf("/Length1 %i\n", (int) ttf_length);
    else if (is_opentype(fd->fm))
      pdf_puts("/Subtype /Type1C\n");
    else
      assert(0);
    }
  pdf_begin_stream();
  fb_flush();
  pdf_end_stream();
}

/**********************************************************************/

static void write_fontdescriptor(fd_entry * fd)
{
    char *glyph;
    struct avl_traverser t;
    assert(fd != NULL && fd->fm != NULL);

    if (is_fontfile(fd->fm))
        write_fontfile(fd);     /* this will set fd->ff_found if font file is found */
    if (fd->fn_objnum != 0)
        write_fontname_object(fd);
    if (fd->fd_objnum == 0)
        fd->fd_objnum = pdf_new_objnum();
    pdf_begin_dict(fd->fd_objnum, 1);
    pdf_puts("/Type /FontDescriptor\n");
    write_fontname(fd, "FontName");
    if (!fd->ff_found && fd->fm->fd_flags == 4)
        pdf_puts("/Flags 34\n");        /* assumes a roman sans serif font */
    else
        pdf_printf("/Flags %i\n", (int) fd->fm->fd_flags);
    write_fontmetrics(fd);
    if (is_cidkeyed(fd->fm)) {
        if (is_type1(fd->fm)) 
            pdf_printf("/FontFile3 %i 0 R\n", (int) fd->ff_objnum);
        else if (is_truetype(fd->fm))
            pdf_printf("/FontFile2 %i 0 R\n", (int) fd->ff_objnum);
        else if (is_opentype(fd->fm))
            pdf_printf("/FontFile3 %i 0 R\n", (int) fd->ff_objnum);
        else
            assert(0);
    } else {
      if (fd->ff_found) {
        if (is_subsetted(fd->fm) && is_type1(fd->fm)) {
            /* /CharSet is optional; names may appear in any order */
            assert(fd->gl_tree != NULL);
            avl_t_init(&t, fd->gl_tree);
            pdf_puts("/CharSet (");
            for (glyph = (char *) avl_t_first(&t, fd->gl_tree); glyph != NULL;
                 glyph = (char *) avl_t_next(&t))
                pdf_printf("/%s", glyph);
            pdf_puts(")\n");
        }
        if (is_type1(fd->fm)) 
            pdf_printf("/FontFile %i 0 R\n", (int) fd->ff_objnum);
        else if (is_truetype(fd->fm))
            pdf_printf("/FontFile2 %i 0 R\n", (int) fd->ff_objnum);
        else if (is_opentype(fd->fm))
            pdf_printf("/FontFile3 %i 0 R\n", (int) fd->ff_objnum);
        else
            assert(0);
      }
    }
    /* TODO: Optional keys for CID fonts. 
    
       The most interesting ones are
          /Style << /Panose <12-byte string>>>
       and
          /CIDSET <stream>
       the latter can be used in subsets, to give the included CIDs
       as a bitmap on the whole list.
    */
    pdf_end_dict();
}

void write_fontdescriptors()
{
    fd_entry *fd;
    struct avl_traverser t;
    if (fd_tree == NULL)
        return;
    avl_t_init(&t, fd_tree);
    for (fd = (fd_entry *) avl_t_first(&t, fd_tree); fd != NULL;
         fd = (fd_entry *) avl_t_next(&t))
        write_fontdescriptor(fd);
}

/**********************************************************************/

void write_fontdictionary(fo_entry * fo)
{
    assert(fo != NULL);
    assert(fo->fm != NULL);
    assert(fo->fo_objnum != 0); /* reserved as pdf_font_num(f) in pdftex.web */

    /* write ToUnicode entry if needed */
    if (fixed_gen_tounicode > 0 && fo->fd != NULL) {
        if (fo->fe != NULL) {
            fo->tounicode_objnum =
                write_tounicode(fo->fe->glyph_names, fo->fe->name);
        } else if (is_type1(fo->fm)) {
            assert(fo->fd->builtin_glyph_names != NULL);
            fo->tounicode_objnum =
                write_tounicode(fo->fd->builtin_glyph_names, fo->fm->tfm_name);
        }
    }

    pdf_begin_dict(fo->fo_objnum, 1);
    pdf_puts("/Type /Font\n");
    pdf_puts("/Subtype /");
    if (is_type1(fo->fm))
#ifdef DO_TYPE1C
        pdf_printf("%s\n", "Type1C");
#else
        pdf_printf("%s\n", "Type1");
#endif
    else if (is_truetype(fo->fm))
        pdf_printf("%s\n", "TrueType");
    else if (is_opentype(fo->fm))
        pdf_printf("%s\n", "Type1");
    else
        assert(0);
    assert(fo->fd != NULL && fo->fd->fd_objnum != 0);
    write_fontname(fo->fd, "BaseFont");
    pdf_printf("/FontDescriptor %i 0 R\n", (int) fo->fd->fd_objnum);
    assert(fo->cw_objnum != 0);
    pdf_printf("/FirstChar %i\n/LastChar %i\n/Widths %i 0 R\n",
               (int) fo->first_char, (int) fo->last_char,
               (int) fo->cw_objnum);
    if ((is_type1(fo->fm) || is_opentype(fo->fm)) && fo->fe != NULL && fo->fe->fe_objnum != 0)
        pdf_printf("/Encoding %i 0 R\n", (int) fo->fe->fe_objnum);
    if (fo->tounicode_objnum != 0)
        pdf_printf("/ToUnicode %i 0 R\n", (int) fo->tounicode_objnum);
    if (pdf_font_attr(fo->tex_font) != get_nullstr()) {
	    pdf_print(pdf_font_attr(fo->tex_font));
        pdf_puts("\n");
    }
    pdf_end_dict();
}

void write_fontdictionaries()
{
    fo_entry *fo;
    struct avl_traverser t;
    if (fo_tree == NULL)
        return;
    avl_t_init(&t, fo_tree);
    for (fo = (fo_entry *) avl_t_first(&t, fo_tree); fo != NULL;
         fo = (fo_entry *) avl_t_next(&t))
        write_fontdictionary(fo);
}

/**********************************************************************/
/*
 * Final flush of all font related stuff by call from
 * @<Output fonts definition@>= in pdftex.web.
 */

void write_fontstuff()
{
    write_fontdescriptors();
    write_fontencodings();      /* see writeenc.c */
    write_fontdictionaries();
}

/**********************************************************************/

void create_fontdictionary(fm_entry * fm, integer font_objnum,
                           internalfontnumber f)
{
    fo_entry *fo = new_fo_entry();
    get_char_range(fo, f);      /* set fo->first_char and fo->last_char from f */
    if (fo->last_char>255) fo->last_char=255; /* added 9-4-2008, mantis #25 */
    assert(fo->last_char >= fo->first_char);
    fo->fm = fm;
    fo->fo_objnum = font_objnum;
    fo->tex_font = f;
    if (is_reencoded(fo->fm)) { /* at least the map entry tells so */
        fo->fe = get_fe_entry(fo->fm->encname); /* returns NULL if .enc file couldn't be opened */
        if (fo->fe != NULL && (is_type1(fo->fm) || is_opentype(fo->fm))) {
            if (fo->fe->fe_objnum == 0)
                fo->fe->fe_objnum = pdf_new_objnum();     /* then it will be written out */
            /* mark encoding pairs used by TeX to optimize encoding vector */
            fo->fe->tx_tree = mark_chars(fo, fo->fe->tx_tree, f);
        }
    }
    fo->tx_tree = mark_chars(fo, fo->tx_tree, f);       /* for write_charwidth_array() */
    write_charwidth_array(fo, f);
    if (!is_builtin(fo->fm)) {
      if (is_type1(fo->fm)) {
	if ((fo->fd = lookup_fontdescriptor(fo)) == NULL) {
	  create_fontdescriptor(fo, f);
	  register_fd_entry(fo->fd);
	}
      } else
	create_fontdescriptor(fo, f);
      if (fo->fe != NULL) {
	mark_reenc_glyphs(fo, f);
	if (!is_type1(fo->fm)) {
	  /* mark reencoded characters as chars on TeX level */
	  assert(fo->fd->tx_tree == NULL);
	  fo->fd->tx_tree = mark_chars(fo, fo->fd->tx_tree, f);
	  if (is_truetype(fo->fm))
	    fo->fd->write_ttf_glyph_names = true;
	}
      } else
	/* mark non-reencoded characters as chars on TeX level */
	fo->fd->tx_tree = mark_chars(fo, fo->fd->tx_tree, f);
      if (!is_type1(fo->fm))
	write_fontdescriptor(fo->fd);
    } else {
      /* builtin fonts still need the /Widths array and /FontDescriptor
       * (to avoid error 'font FOO contains bad /BBox')
       */
      create_fontdescriptor(fo, f);
      write_fontdescriptor(fo->fd);
      if (!is_std_t1font(fo->fm))
	pdftex_warn("font `%s' is not a standard font; "
		    "I suppose it is available to your PDF viewer then",
		    fo->fm->ps_name);
    }
    if (is_type1(fo->fm))
        register_fo_entry(fo);
    else
        write_fontdictionary(fo);
}

/**********************************************************************/


void do_pdf_font(integer font_objnum, internalfontnumber f)
{
    fm_entry *fm;
    /* This is not 100% true: CID is actually needed whenever (and
	 * only) there are more than 256 separate glyphs used. But for
	 * now, just assume the user knows what he is doing;
	 */
    if (font_encodingbytes(f)==2) {
      /* Create a virtual font map entry, as this is needed by the
	   * rest of the font inclusion mechanism.
       */
      fm = new_fm_entry();
      fm->tfm_name = font_name(f);          /* or whatever, not a real tfm */
      fm->ff_name = font_filename(f);       /* the actual file */
      fm->encname = font_encodingname(f);   /* for the CIDSystemInfo */
      fm->ps_name = font_fullname(f);       /* the true name */
      fm->slant = font_slant(f);            /* slant factor */
      fm->extend = font_extend(f);          /* extension factor */
      fm->fd_flags = 4;                     /* can perhaps be done better */
      fm->in_use = true;

	  switch (font_format(f)) {
	  case opentype_format:  set_opentype(fm); break; 
	  case truetype_format:  set_truetype(fm); break; 
	  case type1_format:     set_type1(fm); break;
      default:
		pdftex_fail("writefont.c: The file format (%s) for font `%s' is incompatible with wide characters\n", 
					font_format_name(f),font_name(f));
      }
	  /* This makes "unknown" default to subsetted inclusion */
	  if (font_embedding(f)!=no_embedding) {
		set_included(fm);
		if (font_embedding(f)!=full_embedding) {
		  set_subsetted(fm);
		}
	  }
      set_cidkeyed(fm);
      create_cid_fontdictionary(fm, font_objnum, f);
      
    } else {
      fm = hasfmentry(f) ? (fm_entry *) font_map(f) : NULL;
      if (fm == NULL || (fm->ps_name == NULL && fm->ff_name == NULL))
		writet3(font_objnum, f);
      else
		create_fontdictionary(fm, font_objnum, f);
    }
}

/**********************************************************************/


/* 
   The glyph width is included in |glw_entry|, because that width
   depends on the value it has in the font where it is actually
   typeset from, not the font that is the 'owner' of the fd entry.

   TODO: It is possible that the user messes with the metric width,
   but handling that properly would require access to the 'hmtx' table
   at this point in the program.
*/

int comp_glw_entry(const void *pa, const void *pb, void *p)
{
  unsigned short i, j;
  i = (*(glw_entry *)pa).id;
  j = (*(glw_entry *)pb).id;
  cmp_return(i,j);
  return 0;
}

void create_cid_fontdescriptor(fo_entry * fo, internalfontnumber f)
{
    assert(fo != NULL);
    assert(fo->fm != NULL);
    assert(fo->fd == NULL);
    fo->fd = new_fd_entry();
    preset_fontname(fo);
    preset_fontmetrics(fo->fd, f);
    fo->fd->fe = fo->fe;        /* encoding needed by TrueType writing */
    fo->fd->fm = fo->fm;        /* map entry needed by TrueType writing */
    fo->fd->gl_tree = avl_create(comp_glw_entry, NULL, &avl_xallocator);
    assert(fo->fd->gl_tree != NULL);
}

/*
   The values |font_bc()| and |font_ec()| are potentially large 
   character ids, but the strings that are written out use CID
   indexes, and those are limited to 16-bit values.
*/

static void mark_cid_subset_glyphs(fo_entry *fo, internal_font_number f) 
{
  int i, k;
  glw_entry *j;
  void *aa;
  for (k = 1; k <= max_font_id(); k++) {
    if ( k == f || -f == pdf_font_num(k)) { 
      for (i = font_bc(k); i <= font_ec(k); i++) {
		if (char_exists(k,i) && char_used(k,i)) {
		  j = xtalloc(1,glw_entry);
		  j->id = char_index(k,i);
		  j->wd = divide_scaled_n(char_width(k, i), pdf_font_size(k), 10000.0);
		  if ((glw_entry *)avl_find(fo->fd->gl_tree,j) == NULL) {
		    	aa = avl_probe(fo->fd->gl_tree, j);
			assert(aa != NULL);
		  }
		}
      }
    }
  }
}

/* 
   It is possible to compress the widths array even better, by using the
   alternate 'range' syntax and possibly even using /DW to set 
   a default value.
  
   There is a some optimization here already: glyphs that are
   not used do not appear in the widths array at all. 

   We have to make sure that we do not output an (incorrect!)
   width for a character that exists in the font, but is not used
   in typesetting. An enormous negative width is used as sentinel value
*/


static void write_cid_charwidth_array(fo_entry * fo)
{
  int i, j;
  glw_entry *glyph;
  struct avl_traverser t;

  assert(fo->cw_objnum == 0);
  fo->cw_objnum = pdf_new_objnum();
  pdf_begin_obj(fo->cw_objnum, 1);
  avl_t_init(&t, fo->fd->gl_tree);
  glyph = (glw_entry *) avl_t_first(&t, fo->fd->gl_tree);
  assert(glyph != NULL);
  i = glyph->id;
  pdf_printf("[ %i [", i);
  for (; glyph != NULL; glyph = (glw_entry *) avl_t_next(&t)) {
    j = glyph->wd;
    if (glyph->id>(i+1)) {
      pdf_printf("] %i [", glyph->id);
      j = glyph->wd;
    } 
    if (glyph->id==(i+1))
      pdf_puts(" ");
    pdf_printf("%i", (j/10));
    if ((j % 10) != 0)
      pdf_printf(".%i", (j % 10));
    i = glyph->id;
  }
  pdf_puts("]]\n");
  pdf_end_obj();
}


void create_cid_fontdictionary(fm_entry * fm, integer font_objnum,
			     internalfontnumber f)
{
    fo_entry *fo = new_fo_entry();
    get_char_range(fo, f);      /* set fo->first_char and fo->last_char from f */
    assert(fo->last_char >= fo->first_char);
    fo->fm = fm;
    fo->fo_objnum = font_objnum;
    fo->tex_font = f;
    create_cid_fontdescriptor (fo, f);
    mark_cid_subset_glyphs (fo,f);
    if (is_subsetted(fo->fm)) {
      /* 
	 this is a bit sneaky. |make_subset_tag()| actually expects the glyph tree
	 to contain strings instead of |glw_entry| items. However, all calculations
	 are done using explicit typecasts, so it works out ok.
      */
      make_subset_tag(fo->fd);
    }
    write_cid_charwidth_array(fo);
    write_fontdescriptor(fo->fd);
    
    write_cid_fontdictionary(fo,f);
}

void write_cid_fontdictionary(fo_entry * fo, internalfontnumber f)
{
    int i;

    fo->tounicode_objnum = write_cid_tounicode(fo,f);

    pdf_begin_dict(fo->fo_objnum, 1);
    pdf_puts("/Type /Font\n");
    pdf_puts("/Subtype /Type0\n");
    pdf_puts("/Encoding /Identity-H\n");
    write_fontname(fo->fd, "BaseFont");
    i  = pdf_new_objnum();
    pdf_printf("/DescendantFonts [%i 0 R]\n", i);
    /* todo: the ToUnicode CMap */
	if (fo->tounicode_objnum != 0)
	  pdf_printf("/ToUnicode %i 0 R\n", (int) fo->tounicode_objnum);
    
    pdf_end_dict();

    pdf_begin_dict(i, 1);
    pdf_puts("/Type /Font\n");
    if (is_opentype(fo->fm) || is_type1(fo->fm)) {
      pdf_puts("/Subtype /CIDFontType0\n");
    } else {
      pdf_puts("/Subtype /CIDFontType2\n");
	  pdf_printf("/CIDToGIDMap /Identity\n");
    }
    write_fontname(fo->fd, "BaseFont");
    pdf_printf("/FontDescriptor %i 0 R\n", (int) fo->fd->fd_objnum);
    pdf_printf("/W %i 0 R\n",(int) fo->cw_objnum);
    pdf_printf("/CIDSystemInfo <<\n");
    pdf_printf("/Registry (%s)\n", (font_cidregistry(f)? font_cidregistry(f): "Adobe"));
    pdf_printf("/Ordering (%s)\n",(font_cidordering(f)? font_cidordering(f) : "Identity")) ;
    pdf_printf("/Supplement %u\n",(unsigned int)font_cidsupplement(f));
    pdf_printf(">>\n");

    /* I doubt there is anything useful that could be written here */
    /*      
      if (pdf_font_attr(fo->tex_font) != get_nullstr()) {
      pdf_print(pdf_font_attr(fo->tex_font));
      pdf_puts("\n");
      }
    */
    pdf_end_dict();

}

