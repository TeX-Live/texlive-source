/* extensions.h

   Copyright 2009 Taco Hoekwater <taco@luatex.org>

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


#ifndef EXTENSIONS_H
#  define EXTENSIONS_H

extern alpha_file write_file[16];
extern halfword write_file_mode[16];
extern halfword write_file_translation[16];
extern boolean write_open[18];
extern scaled neg_wd;
extern scaled pos_wd;
extern scaled neg_ht;

extern halfword write_loc;

extern void do_extension(PDF pdf);

/* Three extra node types carry information from |main_control|. */

/*
User defined whatsits can be inserted into node lists to pass data
along from one lua call to anotherb without interference from the
typesetting engine itself. Each has an id, a type, and a value. The
type of the value depends on the |user_node_type| field.
*/

extern void new_whatsit(int s);
extern void new_write_whatsit(int w);
extern void scan_pdf_ext_toks(void);
extern halfword prev_rightmost(halfword s, halfword e);
extern int pdf_last_xform;
extern int pdf_last_ximage;
extern int pdf_last_ximage_pages;
extern int pdf_last_ximage_colordepth;
extern int pdf_last_annot;
extern int pdf_last_link;
extern scaledpos pdf_last_pos;
extern halfword concat_tokens(halfword q, halfword r);
extern int pdf_retval;

extern halfword make_local_par_node(void);


/*
The \.{\\pagediscards} and \.{\\splitdiscards} commands share the
command code |un_vbox| with \.{\\unvbox} and \.{\\unvcopy}, they are
distinguished by their |chr_code| values |last_box_code| and
|vsplit_code|.  These |chr_code| values are larger than |box_code| and
|copy_code|.
*/

extern boolean *eof_seen;       /* has eof been seen? */
extern void print_group(boolean e);
extern void group_trace(boolean e);
extern save_pointer *grp_stack; /* initial |cur_boundary| */
extern halfword *if_stack;      /* initial |cond_ptr| */
extern void group_warning(void);
extern void if_warning(void);
extern void file_warning(void);

extern halfword last_line_fill; /* the |par_fill_skip| glue node of the new paragraph */

#  define get_tex_dimen_register(j) dimen(j)
#  define get_tex_skip_register(j) skip(j)
#  define get_tex_count_register(j) count(j)
#  define get_tex_attribute_register(j) attribute(j)
#  define get_tex_box_register(j) box(j)

extern int set_tex_dimen_register(int j, scaled v);
extern int set_tex_skip_register(int j, halfword v);
extern int set_tex_count_register(int j, scaled v);
extern int set_tex_box_register(int j, scaled v);
extern int set_tex_attribute_register(int j, scaled v);
extern int get_tex_toks_register(int l);
extern int set_tex_toks_register(int j, lstring s);
extern scaled get_tex_box_width(int j);
extern int set_tex_box_width(int j, scaled v);
extern scaled get_tex_box_height(int j);
extern int set_tex_box_height(int j, scaled v);
extern scaled get_tex_box_depth(int j);
extern int set_tex_box_depth(int j, scaled v);

/* Synctex variables */

extern int synctexoption;
extern int synctexoffset;

/* Here are extra variables for Web2c. */

extern pool_pointer edit_name_start;
extern int edit_name_length, edit_line;
extern int ipcon;
extern boolean stop_at_space;
extern int shellenabledp;
extern int restrictedshell;
extern char *output_comment;
extern boolean debug_format_file;

#endif
