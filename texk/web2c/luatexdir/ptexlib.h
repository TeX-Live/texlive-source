/* ptexlib.h
   
   Copyright 1996-2006 Han The Thanh <thanh@pdftex.org>
   Copyright 2006-2009 Taco Hoekwater <taco@luatex.org>

   This file is part of LuaTeX.

   LuaTeX is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 2 of the License, or (at your
   option) any later version.

   LuaTeX is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
   License for more details.

   You should have received a copy of the GNU General Public License along
   with LuaTeX; if not, see <http://www.gnu.org/licenses/>. */

/* $Id$ */

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

#  ifdef MSVC
extern double rint(double x);
#  endif

/* pdftexlib macros */
#  include "ptexmac.h"

/* synctex */
#  include "utils/synctex.h"

#  include "utils/avlstuff.h"
#  include "image/image.h"

#  include "openbsd-compat.h"

#  include "pdf/pagetree.h"
#  include "pdf/pdfpage.h"

/* pdftexlib type declarations */
typedef struct {
    const char *pdfname;
    const char *t1name;
    boolean valid;
} key_entry;

typedef struct _subfont_entry {
    char *infix;                /* infix for this subfont, eg "01" */
    long charcodes[256];        /* the mapping for this subfont as read from sfd */
    struct _subfont_entry *next;
} subfont_entry;

typedef struct {
    char *name;                 /* sfd name, eg "Unicode" */
    subfont_entry *subfont;     /* linked list of subfonts */
} sfd_entry;

#  include "font/mapfile.h"

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

#  define FD_FLAGS_NOT_SET_IN_MAPLINE -1
#  define FD_FLAGS_DEFAULT_EMBED  4     /* a symbol font */
#  define FD_FLAGS_DEFAULT_NON_EMBED 0x22
                                        /* a nonsymbolic serif font */

typedef struct glw_entry_ {     /* subset glyphs for inclusion in CID-based fonts */
    unsigned int id;            /* glyph CID */
    signed int wd;              /* glyph width in 1/1000 em parts */
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

/* filename.c */
extern void scan_file_name(void);
extern void pack_job_name(char *s);
extern void prompt_file_name(char *s, char *e);
extern str_number make_name_string(void);
extern void print_file_name(str_number, str_number, str_number);

/* luainit.c */
extern void write_svnversion(char *a);

/* pdftexlib function prototypes */

/* epdf.c */
extern integer get_fontfile_num(int);
extern integer get_fontname_num(int);
extern void epdf_free(void);

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
extern char *makecstring(integer);
extern char *makeclstring(integer, size_t *);
extern void print_string(char *j);
extern void append_string(char *s);
extern void getcreationdate(void);
extern void tconfusion(char *s);
extern void tprint(char *s);
extern void tprint_nl(char *s);
extern void tprint_esc(char *s);
extern char *stripzeros(char *);

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
extern void write_stream_length(integer, longinteger);
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
extern void checkpdfsave(scaledpos pos);
extern void checkpdfrestore(scaledpos pos);
extern void pdfshipoutbegin(boolean shipping_page);
extern void pdfshipoutend(boolean shipping_page);
extern void pdfsetmatrix(poolpointer in, scaledpos pos);
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
extern boolean is_png_image(integer);
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
extern integer image_groupref(integer img);
extern void scale_image(integer);
extern void set_image_dimensions(integer, integer, integer, integer);
extern void set_image_index(integer, integer);
extern void out_image(integer, scaled, scaled);
extern void dumpimagemeta(void);
extern void undumpimagemeta(integer, integer);
extern void pdf_print_resname_prefix(void);

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
    int pre_exhyphen_char;
    int post_exhyphen_char;
} lang_variables;


#  include "hyphen.h"

struct tex_language {
    HyphenDict *patterns;
    int exceptions;             /* lua registry pointer, should be replaced */
    int id;
    int pre_hyphen_char;
    int post_hyphen_char;
    int pre_exhyphen_char;
    int post_exhyphen_char;
};

#  define MAX_WORD_LEN 256      /* in chars */

extern struct tex_language *new_language(int n);
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

extern void set_pre_exhyphen_char(integer lan, integer val);
extern void set_post_exhyphen_char(integer lan, integer val);
extern integer get_pre_exhyphen_char(integer lan);
extern integer get_post_exhyphen_char(integer lan);
extern halfword compound_word_break(halfword t, int clang);

extern halfword new_ligkern(halfword head, halfword tail);
extern halfword handle_ligaturing(halfword head, halfword tail);
extern halfword handle_kerning(halfword head, halfword tail);

#  define push_dir(a)                               \
  { dir_tmp=new_dir((a));                       \
    vlink(dir_tmp)=dir_ptr; dir_ptr=dir_tmp;    \
    dir_ptr=dir_tmp;                            \
  }

#  define push_dir_node(a)                      \
  { dir_tmp=new_node(whatsit_node,dir_node);    \
    dir_dir(dir_tmp)=dir_dir((a));              \
    dir_level(dir_tmp)=dir_level((a));          \
    dir_dvi_h(dir_tmp)=dir_dvi_h((a));          \
    dir_dvi_ptr(dir_tmp)=dir_dvi_ptr((a));      \
    vlink(dir_tmp)=dir_ptr; dir_ptr=dir_tmp;    \
  }

#  define pop_dir_node()                    \
  { dir_tmp=dir_ptr;                    \
    dir_ptr=vlink(dir_tmp);             \
    flush_node(dir_tmp);                \
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
                       int broken_penalty, halfword final_par_glue,
                       halfword pdf_ignored_dimen);

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
                         scaled first_indent, halfword best_line,
                         halfword pdf_ignored_dimen);

halfword lua_hpack_filter(halfword head_node, scaled size, int pack_type,
                          int extrainfo);
void lua_node_filter(int filterid, int extrainfo, halfword head_node,
                     halfword * tail_node);
halfword lua_vpack_filter(halfword head_node, scaled size, int pack_type,
                          scaled maxd, int extrainfo);
void lua_node_filter_s(int filterid, char *extrainfo);
int lua_linebreak_callback(int is_broken, halfword head_node,
                           halfword * new_head);

void lua_pdf_literal(int i);
void copy_pdf_literal(pointer r, pointer p);
void free_pdf_literal(pointer p);
void show_pdf_literal(pointer p);

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
void initex_cat_codes(int h);
void unsave_text_codes(quarterword grouplevel);
void initialize_text_codes(void);
void dump_text_codes(void);
void undump_text_codes(void);
void copy_cat_codes(int from, int to);
void free_math_codes(void);
void free_text_codes(void);

/* mathcodes.c */

#  define no_mathcode 0         /* this is a flag for |scan_delimiter| */
#  define tex_mathcode 8
#  define aleph_mathcode 16
#  define xetex_mathcode 21
#  define xetexnum_mathcode 22

typedef struct mathcodeval {
    integer class_value;
    integer origin_value;
    integer family_value;
    integer character_value;
} mathcodeval;

void set_math_code(integer n,
                   integer commandorigin,
                   integer mathclass,
                   integer mathfamily, integer mathcharacter, quarterword gl);

mathcodeval get_math_code(integer n);
integer get_math_code_num(integer n);
mathcodeval scan_mathchar(int extcode);
mathcodeval scan_delimiter_as_mathchar(int extcode);

mathcodeval mathchar_from_integer(integer value, int extcode);
void show_mathcode_value(mathcodeval d);


typedef struct delcodeval {
    integer class_value;
    integer origin_value;
    integer small_family_value;
    integer small_character_value;
    integer large_family_value;
    integer large_character_value;
} delcodeval;

void set_del_code(integer n,
                  integer commandorigin,
                  integer smathfamily,
                  integer smathcharacter,
                  integer lmathfamily, integer lmathcharacter, quarterword gl);

delcodeval get_del_code(integer n);

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
integer luacstring_cattable(void);
int luacstring_input(void);
int luacstring_partial(void);
int luacstring_final_line(void);

/* luatoken.c */
void do_get_token_lua(integer callback_id);

/* luanode.c */
int visible_last_node_type(int n);
void print_node_mem_stats(int n, int o);

/* writeimg.c */
integer epdf_xsize(integer i);
integer epdf_ysize(integer i);
integer epdf_orig_y(integer i);
integer epdf_orig_x(integer i);

/* limglib.c */
void vf_out_image(unsigned i);

/* vfovf.c */
void vf_expand_local_fonts(internal_font_number f);
internal_font_number letter_space_font(halfword u, internal_font_number f,
                                       integer e);
internal_font_number auto_expand_font(internal_font_number f, integer e);
str_number expand_font_name(internal_font_number f, integer e);
void pdf_check_vf_cur_val(void);
internal_font_number copy_font_info(internal_font_number f);

/* ltexiolib.c */
void flush_loggable_info(void);

/* luastuff.c */
void luacall(int s, int nameptr);
void luatokencall(int p, int nameptr);

extern void check_texconfig_init(void);

void tex_error(char *msg, char **hlp);

scaled divide_scaled(scaled s, scaled m, integer dd);
scaled divide_scaled_n(double s, double m, double d);

/* mlist.c */
void run_mlist_to_hlist(pointer p, integer m_style, boolean penalties);
void fixup_math_parameters(integer fam_id, integer size_id, integer f,
                           integer lvl);

/* texpdf.c */
void pdf_print_char(internal_font_number f, integer c);
void pdf_print(str_number n);
void pdf_print_str(str_number n);
void pdf_print_int(longinteger n);
void pdf_print_real(integer m, integer d);

/* textoken.c */

#  define  NO_CAT_TABLE      -2
#  define  DEFAULT_CAT_TABLE -1

extern boolean str_eq_cstr(str_number, char *, size_t);
void get_next(void);
extern void check_outer_validity(void);
boolean scan_keyword(char *);
void scan_direction(void);
halfword active_to_cs(int, int);
void get_token_lua(void);
int get_char_cat_code(int);

/* texdeffont.c */

void tex_def_font(small_number a);

/* lcallbacklib.c */

#  include <../lua51/lua.h>


typedef enum {
    find_write_file_callback = 1,
    find_output_file_callback,
    find_image_file_callback,
    find_format_file_callback,
    find_read_file_callback, open_read_file_callback,
    find_ocp_file_callback, read_ocp_file_callback,
    find_vf_file_callback, read_vf_file_callback,
    find_data_file_callback, read_data_file_callback,
    find_font_file_callback, read_font_file_callback,
    find_map_file_callback, read_map_file_callback,
    find_enc_file_callback, read_enc_file_callback,
    find_type1_file_callback, read_type1_file_callback,
    find_truetype_file_callback, read_truetype_file_callback,
    find_opentype_file_callback, read_opentype_file_callback,
    find_sfd_file_callback, read_sfd_file_callback,
    find_pk_file_callback, read_pk_file_callback,
    show_error_hook_callback,
    process_input_buffer_callback,
    start_page_number_callback, stop_page_number_callback,
    start_run_callback, stop_run_callback,
    define_font_callback,
    token_filter_callback,
    pre_output_filter_callback,
    buildpage_filter_callback,
    hpack_filter_callback, vpack_filter_callback,
    char_exists_callback,
    hyphenate_callback,
    ligaturing_callback,
    kerning_callback,
    pre_linebreak_filter_callback,
    linebreak_filter_callback,
    post_linebreak_filter_callback,
    mlist_to_hlist_callback,
    total_callbacks
} callback_callback_types;

extern int callback_set[];
extern int lua_active;

#  define callback_defined(a) callback_set[a]

extern int run_callback(int i, char *values, ...);
extern int run_saved_callback(int i, char *name, char *values, ...);
extern int run_and_save_callback(int i, char *values, ...);
extern void destroy_saved_callback(int i);
extern boolean get_callback(lua_State * L, int i);

extern void get_saved_lua_boolean(int i, char *name, boolean * target);
extern void get_saved_lua_number(int i, char *name, integer * target);
extern void get_saved_lua_string(int i, char *name, char **target);

extern void get_lua_boolean(char *table, char *name, boolean * target);
extern void get_lua_number(char *table, char *name, integer * target);
extern void get_lua_string(char *table, char *name, char **target);

extern char *get_lua_name(int i);

#  include "texmath.h"
#  include "primitive.h"

#endif                          /* PDFTEXLIB */
