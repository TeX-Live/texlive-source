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

#ifndef PTEXLIB_H
#  define PTEXLIB_H

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
#  include "image/writeimg.h"
#  include "openbsd-compat.h"
#  include "pdf/pagetree.h"
#  include "pdf/pdfpage.h"
#  include "font/luatexfont.h"
#  include "font/mapfile.h"
#  include "utils/utils.h"
#  include "image/writejbig2.h"
#  include "image/pdftoepdf.h"

/**********************************************************************/

typedef short shalfword;

/* loadpool.c */
int loadpoolstrings(integer spare_size);

/* tex/filename.c */
extern void scan_file_name(void);
extern void pack_job_name(char *s);
extern void prompt_file_name(char *s, char *e);
extern str_number make_name_string(void);
extern void print_file_name(str_number, str_number, str_number);

/* lua/luainit.c */
extern void write_svnversion(char *a);

/* utils/writezip.c */
extern void write_zip(boolean);
extern void zip_free(void);

/**********************************************************************/

typedef enum {
    no_print = 16,
    term_only = 17,
    log_only = 18,
    term_and_log = 19,
    pseudo = 20,
    new_string = 21
} selector_settings;

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

#  define push_dir(a)                           \
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

#  define pop_dir_node()                        \
  { dir_tmp=dir_ptr;                            \
    dir_ptr=vlink(dir_tmp);                     \
    flush_node(dir_tmp);                        \
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

/* lang/texlang.c */

void dump_language_data(void);
void undump_language_data(void);
char *exception_strings(struct tex_language *lang);

/* lua/llualib.c */

void dump_luac_registers(void);
void undump_luac_registers(void);

/* lua/ltexlib.c */
void luacstring_start(int n);
void luacstring_close(int n);
integer luacstring_cattable(void);
int luacstring_input(void);
int luacstring_partial(void);
int luacstring_final_line(void);

/* lua/luatoken.c */
void do_get_token_lua(integer callback_id);

/* lua/luanode.c */
int visible_last_node_type(int n);
void print_node_mem_stats(void);

/* lua/limglib.c */
void vf_out_image(unsigned i);

/* lua/ltexiolib.c */
void flush_loggable_info(void);

/* lua/luastuff.c */
void luacall(int s, int nameptr);
void luatokencall(int p, int nameptr);

extern void check_texconfig_init(void);

scaled divide_scaled(scaled s, scaled m, integer dd);
scaled divide_scaled_n(double s, double m, double d);

/* tex/mlist.c */
void run_mlist_to_hlist(pointer p, integer m_style, boolean penalties);
void fixup_math_parameters(integer fam_id, integer size_id, integer f,
                           integer lvl);

/* tex/texpdf.c */
void pdf_print_char(internal_font_number f, integer c);
void pdf_print(str_number n);
void pdf_print_str(str_number n);
void pdf_print_int(longinteger n);
void pdf_print_real(integer m, integer d);

/* tex/textoken.c */

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

/* tex/texdeffont.c */

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

#endif                          /* PTEXLIB_H */
