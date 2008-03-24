/*
  Copyright (c) 1996-2006 Han The Thanh, <thanh@pdftex.org>

This file is part of luatex.

luatex is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

luatex is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with luatex; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

$Id$
*/

#ifndef LUATEXLIB
#  define LUATEXLIB

/* WEB2C macros and prototypes */
#  if !defined(LUATEXCOERCE)
#    ifdef luatex
#      undef luatex             /* to avoid warning about redefining luatex in luatexd.h */
#    endif                      /* luatex */
#    define EXTERN extern
#    include "luatexd.h"
#  endif

/* pdftexlib macros */
#  include "ptexmac.h"

#  include "avlstuff.h"
#  include "image/image.h"

#  include "openbsd-compat.h"

/* pdftexlib type declarations */
typedef struct {
    const char *pdfname;
    const char *t1name;
    boolean valid;
} key_entry;

struct _subfont_entry;
typedef struct _subfont_entry subfont_entry;

struct _subfont_entry {
    char *infix;                /* infix for this subfont, eg "01" */
    long charcodes[256];        /* the mapping for this subfont as read from sfd */
    subfont_entry *next;
};

typedef struct {
    char *name;                 /* sfd name, eg "Unicode" */
    subfont_entry *subfont;     /* linked list of subfonts */
} sfd_entry;

typedef struct {
    integer fe_objnum;          /* object number */
    char *name;                 /* encoding file name */
    char **glyph_names;         /* array of glyph names */
    struct avl_table *tx_tree;  /* tree of encoding positions marked as used by TeX */
} fe_entry;

typedef struct {
    char *name;                 /* glyph name */
    long code;                  /* -1 = undefined; -2 = multiple codes, stored
                                   as string in unicode_seq; otherwise unicode value */
    char *unicode_seq;          /* multiple unicode sequence */
} glyph_unicode_entry;

typedef struct {
    /* parameters scanned from the map file: */
    char *tfm_name;             /* TFM file name (1st field in map line) */
    char *sfd_name;             /* subfont directory name, like @sfd_name@ */
    char *ps_name;              /* PostScript name (optional 2nd field in map line) */
    integer fd_flags;           /* font descriptor /Flags (PDF Ref. section 5.7.1) */
    integer slant;              /* SlantFont */
    integer extend;             /* ExtendFont */
    char *encname;              /* encoding file name */
    char *ff_name;              /* font file name */
    unsigned short type;        /* various flags */
    short pid;                  /* Pid for truetype fonts */
    short eid;                  /* Eid for truetype fonts */
    /* parameters NOT scanned from the map file: */
    subfont_entry *subfont;     /* subfont mapping */
    unsigned short links;       /* link flags from tfm_tree and ps_tree */
    boolean in_use;             /* true if this structure has been referenced already */
} fm_entry;

typedef struct glw_entry_ {     /* subset glyphs for inclusion in CID-based fonts */
    unsigned short id;          /* glyph CID */
    unsigned short wd;          /* glyph width in 1/1000 em parts */
} glw_entry;


/**********************************************************************/

typedef struct {
    int val;                    /* value */
    boolean set;                /* true if parameter has been set */
} intparm;

typedef struct fd_entry_ {
    integer fd_objnum;          /* object number of the font descriptor object */
    char *fontname;             /* /FontName (without subset tag) */
    char *subset_tag;           /* 6-character subset tag */
    boolean ff_found;
    integer ff_objnum;          /* object number of the font program stream */
    integer fn_objnum;          /* font name object number (embedded PDF) */
    boolean all_glyphs;         /* embed all glyphs? */
    boolean write_ttf_glyph_names;
    intparm font_dim[FONT_KEYS_NUM];
    fe_entry *fe;               /* pointer to encoding structure */
    char **builtin_glyph_names; /* builtin encoding as read from the Type1 font file */
    fm_entry *fm;               /* pointer to font map structure */
    struct avl_table *tx_tree;  /* tree of non-reencoded TeX characters marked as used */
    struct avl_table *gl_tree;  /* tree of all marked glyphs */
} fd_entry;

typedef struct fo_entry_ {
    integer fo_objnum;          /* object number of the font dictionary */
    internalfontnumber tex_font;        /* needed only for \pdffontattr{} */
    fm_entry *fm;               /* pointer to font map structure for this font dictionary */
    fd_entry *fd;               /* pointer to /FontDescriptor object structure */
    fe_entry *fe;               /* pointer to encoding structure */
    integer cw_objnum;          /* object number of the font program object */
    integer first_char;         /* first character used in this font */
    integer last_char;          /* last character used in this font */
    struct avl_table *tx_tree;  /* tree of non-reencoded TeX characters marked as used */
    integer tounicode_objnum;   /* object number of ToUnicode */
} fo_entry;

/**********************************************************************/

typedef struct {
    char *ff_name;              /* base name of font file */
    char *ff_path;              /* full path to font file */
} ff_entry;

typedef short shalfword;
typedef struct {
    integer charcode, cwidth, cheight, xoff, yoff, xescape, rastersize;
    halfword *raster;
} chardesc;

/* pdftexlib variable declarations */
extern boolean true_dimen;
extern char **t1_glyph_names, *t1_builtin_glyph_names[];
extern char *cur_file_name;
extern const char notdef[];
extern integer t1_length1, t1_length2, t1_length3;
extern integer ttf_length;
extern strnumber last_tex_string;
extern size_t last_ptr_index;

/* loadpool.c */

int loadpoolstrings(integer spare_size);

/* pdftexlib function prototypes */

/* epdf.c */
extern integer get_fontfile_num(int);
extern integer get_fontname_num(int);
extern void epdf_free(void);

/* mapfile.c */
extern fm_entry *lookup_fontmap(char *);
extern boolean hasfmentry(internalfontnumber);
extern void fm_free(void);
extern void fm_read_info(void);
extern ff_entry *check_ff_exist(char *, boolean);
extern void pdfmapfile(integer);
extern void pdfmapline(integer);
extern void pdf_init_map_file(string map_name);
extern fm_entry *new_fm_entry(void);
extern void delete_fm_entry(fm_entry *);
extern int avl_do_entry(fm_entry *, int);

/* papersiz.c */
extern integer myatodim(char **);
extern integer myatol(char **);

/* pkin.c */
extern int readchar(boolean, chardesc *);

/* subfont.c */
extern void sfd_free(void);
extern boolean handle_subfont_fm(fm_entry *, int);

/* tounicode.c */
extern void glyph_unicode_free(void);
extern void def_tounicode(strnumber, strnumber);
extern integer write_tounicode(char **, char *);

/* utils.c */
extern boolean str_eq_cstr(strnumber, char *);
extern char *makecstring(integer);
extern char *makeclstring(integer, int *);
extern void print_string(char *j);
extern void append_string(char *s);
extern void getcreationdate(void);
extern void tconfusion(char *s);
extern void tprint(char *s);
extern void tprint_nl(char *s);
extern void tprint_esc(char *s);

#  define overflow_string(a,b) { overflow(maketexstring(a),b); flush_str(last_tex_string); }

extern int xfflush(FILE *);
extern int xgetc(FILE *);
extern int xputc(int, FILE *);
extern scaled ext_xn_over_d(scaled, scaled, scaled);
extern size_t xfwrite(void *, size_t size, size_t nmemb, FILE *);
extern strnumber get_resname_prefix(void);
extern strnumber maketexstring(const char *);
extern strnumber maketexlstring(const char *, size_t);
extern integer fb_offset(void);
extern void fb_flush(void);
extern void fb_putchar(eight_bits b);
extern void fb_seek(integer);
extern void libpdffinish(void);
extern char *makecfilename(strnumber s);
extern void make_subset_tag(fd_entry *);
__attribute__ ((format(printf, 1, 2)))
extern void pdf_printf(const char *, ...);
extern void pdf_puts(const char *);
__attribute__ ((noreturn, format(printf, 1, 2)))
extern void pdftex_fail(const char *, ...);
__attribute__ ((format(printf, 1, 2)))
extern void pdftex_warn(const char *, ...);
extern void set_job_id(int, int, int, int);
__attribute__ ((format(printf, 1, 2)))
extern void tex_printf(const char *, ...);
extern void write_stream_length(integer, integer);
extern char *convertStringToPDFString(const char *in, int len);
extern void print_ID(strnumber);
extern void print_creation_date();
extern void print_mod_date();
extern void escapename(poolpointer in);
extern void escapestring(poolpointer in);
extern void escapehex(poolpointer in);
extern void unescapehex(poolpointer in);
extern void make_pdftex_banner(void);
extern void init_start_time();
extern void remove_pdffile(void);
extern void garbage_warning(void);
extern void initversionstring(char **versions);
extern int newcolorstack(integer s, integer literal_mode, boolean pagestart);
extern int colorstackused();
extern integer colorstackset(int colstack_no, integer s);
extern integer colorstackpush(int colstack_no, integer s);
extern integer colorstackpop(int colstack_no);
extern integer colorstackcurrent(int colstack_no);
extern integer colorstackskippagestart(int colstack_no);
extern void checkpdfsave(int cur_h, int cur_v);
extern void checkpdfrestore(int cur_h, int cur_v);
extern void pdfshipoutbegin(boolean shipping_page);
extern void pdfshipoutend(boolean shipping_page);
extern void pdfsetmatrix(poolpointer in, scaled cur_h, scaled cur_v);
extern void matrixtransformpoint(scaled x, scaled y);
extern void matrixtransformrect(scaled llx, scaled lly, scaled urx, scaled ury);
extern boolean matrixused();
extern void matrixrecalculate(scaled urx);
extern scaled getllx();
extern scaled getlly();
extern scaled geturx();
extern scaled getury();

/* writeenc.c */
extern fe_entry *get_fe_entry(char *);
extern void enc_free(void);
extern void write_fontencodings(void);

/* writefont.c */
extern void do_pdf_font(integer, internalfontnumber);
extern fd_entry *lookup_fd_entry(char *, integer, integer);
extern fd_entry *new_fd_entry(void);
extern void write_fontstuff();

/* writeimg.c */
extern boolean check_image_b(integer);
extern boolean check_image_c(integer);
extern boolean check_image_i(integer);
extern boolean is_pdf_image(integer);
extern integer epdforigx(integer);
extern integer epdforigy(integer);
extern integer image_pages(integer);
extern integer image_index(integer);
extern integer image_width(integer);
extern integer image_height(integer);
extern integer image_depth(integer);
extern integer image_objnum(integer);
extern integer image_imgnum(integer);
extern integer new_image_entry(void);
extern integer read_image(integer, integer, strnumber, integer, strnumber,
                          strnumber, integer, integer, integer, integer);
extern void img_free(void);
extern void update_image_procset(integer);
extern void write_image(integer);
extern integer image_colordepth(integer img);
extern void scale_image(integer);
extern void set_image_dimensions(integer, integer, integer, integer);
extern void set_image_index(integer, integer);
extern void out_image(integer, scaled, scaled);

/* writejbig2.c */
extern void flush_jbig2_page0_objects();

/* writet1.c */
extern boolean t1_subset(char *, char *, unsigned char *);
extern char **load_enc_file(char *);
extern void writet1(fd_entry *);
extern void t1_free(void);

/* writet3.c */
extern void writet3(int, internalfontnumber);
extern scaled get_pk_char_width(internalfontnumber, scaled);

/* writettf.c */
extern void writettf(fd_entry *);
extern void writeotf(fd_entry *);
extern void ttf_free(void);

/* writezip.c */
extern void write_zip(boolean);
extern void zip_free(void);

/* avlstuff.c */
extern int comp_int_entry(const void *, const void *, void *);
extern int comp_string_entry(const void *, const void *, void *);
extern void avl_put_obj(integer, integer);
extern integer avl_find_obj(integer, integer, integer);

/**********************************************************************/
static const key_entry font_key[FONT_KEYS_NUM] = {
    {"Ascent", "Ascender", 1}
    , {"CapHeight", "CapHeight", 1}
    , {"Descent", "Descender", 1}
    , {"ItalicAngle", "ItalicAngle", 1}
    , {"StemV", "StdVW", 1}
    , {"XHeight", "XHeight", 1}
    , {"FontBBox", "FontBBox", 1}
    , {"", "", 0}
    , {"", "", 0}
    , {"", "", 0}
    , {"FontName", "FontName", 1}
};

/**********************************************************************/

typedef enum {
    no_print = 16,
    term_only = 17,
    log_only = 18,
    term_and_log = 19,
    pseudo = 20,
    new_string = 21
} selector_settings;


#  include "font/texfont.h"

/* language stuff */

typedef struct _lang_variables {
    int pre_hyphen_char;
    int post_hyphen_char;
} lang_variables;


#  include "hyphen.h"

struct tex_language {
    HyphenDict *patterns;
    int exceptions;             /* lua registry pointer, should be replaced */
    int id;
    int pre_hyphen_char;
    int post_hyphen_char;
};

#  define MAX_WORD_LEN 256      /* in chars */

extern struct tex_language *new_language(void);
extern struct tex_language *get_language(int n);
extern void load_patterns(struct tex_language *lang, unsigned char *buf);
extern void load_hyphenation(struct tex_language *lang, unsigned char *buf);
extern int hyphenate_string(struct tex_language *lang, char *w, char **ret);

extern void new_hyphenation(halfword h, halfword t);
extern void clear_patterns(struct tex_language *lang);
extern void clear_hyphenation(struct tex_language *lang);
extern char *clean_hyphenation(char *buffer, char **cleaned);
extern void hnj_hyphenation(halfword head, halfword tail);

extern void set_pre_hyphen_char(integer lan, integer val);
extern void set_post_hyphen_char(integer lan, integer val);
extern integer get_pre_hyphen_char(integer lan);
extern integer get_post_hyphen_char(integer lan);

extern halfword new_ligkern(halfword head, halfword tail);
extern halfword handle_ligaturing(halfword head, halfword tail);
extern halfword handle_kerning(halfword head, halfword tail);

#  define push_dir(a)								\
  { dir_tmp=new_dir((a));						\
	vlink(dir_tmp)=dir_ptr; dir_ptr=dir_tmp;	\
	dir_ptr=dir_tmp;							\
  }

#  define push_dir_node(a)						\
  { dir_tmp=new_node(whatsit_node,dir_node);	\
    dir_dir(dir_tmp)=dir_dir((a));				\
    dir_level(dir_tmp)=dir_level((a));			\
    dir_dvi_h(dir_tmp)=dir_dvi_h((a));			\
    dir_dvi_ptr(dir_tmp)=dir_dvi_ptr((a));		\
    vlink(dir_tmp)=dir_ptr; dir_ptr=dir_tmp;	\
  }

#  define pop_dir_node()					\
  { dir_tmp=dir_ptr;					\
    dir_ptr=vlink(dir_tmp);				\
    flush_node(dir_tmp);				\
  }



#  define dir_parallel(a,b) (((a) % 2)==((b) % 2))
#  define dir_orthogonal(a,b) (((a) % 2)!=((b) % 2))

#  define is_rotated(a) dir_parallel(dir_secondary[(a)],dir_tertiary[(a)])

void initialize_active(void);

halfword find_protchar_left(halfword l, boolean d);
halfword find_protchar_right(halfword l, halfword r);

void ext_do_line_break(boolean d,
                       int pretolerance,
                       int tracing_paragraphs,
                       int tolerance,
                       scaled emergency_stretch,
                       int looseness,
                       int hyphen_penalty,
                       int ex_hyphen_penalty,
                       int pdf_adjust_spacing,
                       halfword par_shape_ptr,
                       int adj_demerits,
                       int pdf_protrude_chars,
                       int line_penalty,
                       int last_line_fit,
                       int double_hyphen_demerits,
                       int final_hyphen_demerits,
                       int hang_indent,
                       int hsize,
                       int hang_after,
                       halfword left_skip,
                       halfword right_skip,
                       int pdf_each_line_height,
                       int pdf_each_line_depth,
                       int pdf_first_line_height,
                       int pdf_last_line_depth,
                       halfword inter_line_penalties_ptr,
                       int inter_line_penalty,
                       int club_penalty,
                       halfword club_penalties_ptr,
                       halfword display_widow_penalties_ptr,
                       halfword widow_penalties_ptr,
                       int display_widow_penalty,
                       int widow_penalty,
                       int broken_penalty, halfword final_par_glue);

void ext_post_line_break(boolean d,
                         int right_skip,
                         int left_skip,
                         int pdf_protrude_chars,
                         halfword par_shape_ptr,
                         int pdf_adjust_spacing,
                         int pdf_each_line_height,
                         int pdf_each_line_depth,
                         int pdf_first_line_height,
                         int pdf_last_line_depth,
                         halfword inter_line_penalties_ptr,
                         int inter_line_penalty,
                         int club_penalty,
                         halfword club_penalties_ptr,
                         halfword display_widow_penalties_ptr,
                         halfword widow_penalties_ptr,
                         int display_widow_penalty,
                         int widow_penalty,
                         int broken_penalty,
                         halfword final_par_glue,
                         halfword best_bet,
                         halfword last_special_line,
                         scaled second_width,
                         scaled second_indent,
                         scaled first_width,
                         scaled first_indent, halfword best_line);

halfword lua_hpack_filter(halfword head_node, scaled size, int pack_type,
                          int extrainfo);
void lua_node_filter(int filterid, int extrainfo, halfword head_node,
                     halfword * tail_node);
halfword lua_vpack_filter(halfword head_node, scaled size, int pack_type,
                          scaled maxd, int extrainfo);
void lua_node_filter_s(int filterid, char *extrainfo, halfword head_node,
                       halfword * tail_node);

void load_tex_patterns(int curlang, halfword head);
void load_tex_hyphenation(int curlang, halfword head);

/* textcodes.c */
void set_lc_code(integer n, halfword v, quarterword gl);
halfword get_lc_code(integer n);
void set_uc_code(integer n, halfword v, quarterword gl);
halfword get_uc_code(integer n);
void set_sf_code(integer n, halfword v, quarterword gl);
halfword get_sf_code(integer n);
void set_cat_code(integer h, integer n, halfword v, quarterword gl);
halfword get_cat_code(integer h, integer n);
void unsave_cat_codes(integer h, quarterword gl);
int valid_catcode_table(int h);
void unsave_text_codes(quarterword grouplevel);
void initialize_text_codes(void);
void dump_text_codes(void);
void undump_text_codes(void);

/* mathcodes.c */

void set_math_code(integer n, halfword v, quarterword gl);
halfword get_math_code(integer n);
void set_del_code(integer n, halfword v, halfword w, quarterword gl);
halfword get_del_code_a(integer n);
halfword get_del_code_b(integer n);
void unsave_math_codes(quarterword grouplevel);
void initialize_math_codes(void);
void dump_math_codes(void);
void undump_math_codes(void);

/* texlang.c */

void dump_language_data(void);
void undump_language_data(void);
char *exception_strings(struct tex_language *lang);

/* llualib.c */

void dump_luac_registers(void);
void undump_luac_registers(void);

/* ltexlib.c */
void luacstring_start(int n);
void luacstring_close(int n);
int luacstring_detokenized(void);
int luacstring_defaultcattable(void);
integer luacstring_cattable(void);
int luacstring_simple(void);
int luacstring_penultimate(void);
int luacstring_input(void);


/* luatoken.c */
void do_get_token_lua(integer callback_id);

/* luanode.c */
int visible_last_node_type(int n);
void print_node_mem_stats(int n, int o);

/* writeimg.c */
integer epdf_orig_y(integer i);
integer epdf_orig_x(integer i);

/* limglib.c */
void lua_setximage(integer i);

/* vfovf.c */
void vf_expand_local_fonts(internal_font_number f);
internal_font_number letter_space_font(halfword u, internal_font_number f,
                                       integer e);
internal_font_number auto_expand_font(internal_font_number f, integer e);
str_number expand_font_name(internal_font_number f, integer e);
void pdf_check_vf_cur_val(void);

/* ltexiolib.c */
void flush_loggable_info(void);

/* luastuff.c */
void closelua(int n);
void luacall(int n, int s);
void luatokencall(int n, int p);

void tex_error(char *msg, char **hlp);

scaled divide_scaled(scaled s, scaled m, integer dd);
scaled divide_scaled_n(double s, double m, double d);

/* texpdf.c */
void pdf_print_char(internal_font_number f, integer c);
void pdf_print(str_number n);
void pdf_print_str(str_number n);
void pdf_print_int(integer n);

/* textoken.c */

void get_next(void);
void get_token_lua(void);

#endif                          /* PDFTEXLIB */
