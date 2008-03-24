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

$Id $
*/

/* some code array functions */

extern void     set_math_code (integer n, halfword v, quarterword grouplevel);
extern halfword get_math_code (integer n);

extern void     set_del_code (integer n, halfword v,  halfword w, quarterword grouplevel);
extern halfword get_del_code_a (integer n);
extern halfword get_del_code_b (integer n);

extern void unsavemathcodes (quarterword grouplevel);
extern void initializemathcodes ();
extern void dumpmathcodes ();
extern void undumpmathcodes ();

extern void     set_lc_code  (integer n, halfword v, quarterword grouplevel);
extern halfword get_lc_code  (integer n);
extern void     set_uc_code  (integer n, halfword v, quarterword grouplevel);
extern halfword get_uc_code  (integer n);
extern void     set_sf_code  (integer n, halfword v, quarterword grouplevel);
extern halfword get_sf_code  (integer n);
extern void     set_cat_code (integer h, integer n, halfword v, quarterword grouplevel);
extern halfword get_cat_code (integer h, integer n);

extern void unsavetextcodes (quarterword grouplevel);
extern void unsavecatcodes (integer h,quarterword grouplevel);
extern void copy_cat_codes (int from, int to);
extern void initex_cat_codes (int h);
extern void clearcatcodestack (integer h);
extern boolean validcatcodetable (int h);

extern void initializetextcodes ();
extern void dumptextcodes ();
extern void undumptextcodes ();

typedef enum {
  escape, left_brace, right_brace, math_shift, 
  tab_mark, car_ret, mac_param, sup_mark, 
  sub_mark, ignore, spacer, letter, 
  other_char, active_char, comment, invalid_char } cat_codes;


extern void do_vf(internal_font_number tmp_f);

extern int readbinfile(FILE *f, unsigned char **b, integer *s);

#define read_tfm_file  readbinfile
#define read_vf_file   readbinfile
#define read_ocp_file  readbinfile
#define read_data_file readbinfile

extern int **ocp_tables;

extern void allocate_ocp_table();
extern void dump_ocp_table();
extern void undump_ocp_table();

extern void run_external_ocp();
extern void b_test_in();

/* Additions to texmfmp.h for pdfTeX */

/* mark a char in font */
#define pdf_mark_char(f,c) set_char_used(f,c,true)

/* test whether a char in font is marked */
#define pdf_char_marked char_used

/* writepdf() always writes by fwrite() */
#define       write_pdf(a, b) \
  (void) fwrite ((char *) &pdf_buf[a], sizeof (pdf_buf[a]), \
                 (int) ((b) - (a) + 1), pdf_file)

#define tex_b_open_in(f) \
    open_input (&(f), kpse_tex_format, FOPEN_RBIN_MODE)
#define ovf_b_open_in(f) \
    open_input (&(f), kpse_ovf_format, FOPEN_RBIN_MODE)
#define vf_b_open_in(f) \
    open_input (&(f), kpse_vf_format, FOPEN_RBIN_MODE)

extern int open_outfile(FILE **f, char *name, char *mode);

#define do_a_open_out(f) open_outfile(&(f),(char *)(nameoffile+1),FOPEN_W_MODE)
#define do_b_open_out(f) open_outfile(&(f),(char *)(nameoffile+1),FOPEN_WBIN_MODE)

#define pdfassert assert
#define voidcast(a) (void *)(a)
#define varmemcast(a) (memory_word *)(a)
#define fixmemcast(a) (smemory_word *)(a)
extern volatile memory_word *varmem;
extern halfword var_mem_min;
extern halfword var_mem_max;
extern halfword  get_node (integer s);
extern void free_node (halfword p, integer s) ;
extern void init_node_mem (integer r, integer s) ;
extern void dump_node_mem (void) ;
extern void undump_node_mem (void);


typedef enum {
  find_write_file_callback = 1,
  find_output_file_callback,
  find_image_file_callback,
  find_format_file_callback,
  find_read_file_callback,      open_read_file_callback,
  find_ocp_file_callback,       read_ocp_file_callback,
  find_vf_file_callback,        read_vf_file_callback,
  find_data_file_callback,      read_data_file_callback,
  find_font_file_callback,      read_font_file_callback,
  find_map_file_callback,       read_map_file_callback,
  find_enc_file_callback,       read_enc_file_callback,
  find_type1_file_callback,     read_type1_file_callback,
  find_truetype_file_callback,  read_truetype_file_callback,
  find_opentype_file_callback,  read_opentype_file_callback,
  find_sfd_file_callback,       read_sfd_file_callback,
  find_pk_file_callback,        read_pk_file_callback,
  show_error_hook_callback,
  process_input_buffer_callback,
  start_page_number_callback,  stop_page_number_callback,
  start_run_callback,          stop_run_callback,
  define_font_callback,
  token_filter_callback,
  pre_output_filter_callback,
  buildpage_filter_callback,
  hpack_filter_callback,  vpack_filter_callback,
  char_exists_callback, 
  hyphenate_callback,
  ligaturing_callback,
  kerning_callback,
  pre_linebreak_filter_callback,
  linebreak_filter_callback, 
  post_linebreak_filter_callback,
  total_callbacks } callback_callback_types;

extern int callback_set[];

#define callback_defined(a) callback_set[a]

extern int  run_callback          (int i, char *values, ...);
extern int  run_saved_callback     (int i, char *name, char *values, ...);
extern int  run_and_save_callback   (int i, char *values, ...);
extern void destroy_saved_callback (int i);

extern void get_saved_lua_boolean   (int i, char *name, boolean *target);
extern void get_saved_lua_number    (int i, char *name, integer *target);
extern void get_saved_lua_string    (int i, char *name, char **target);

extern void get_lua_boolean        (char *table, char *name, boolean *target);
extern void get_lua_number         (char *table, char *name, integer *target);
extern void get_lua_string         (char *table, char *name, char **target);

#include <luatexdir/ptexlib.h>
