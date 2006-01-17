/*  $Header$

    This is dvipdfm, a DVI to PDF translator.
    Copyright (C) 1998, 1999 by Mark A. Wicks

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    
    The author may be contacted via the e-mail address

	mwicks@kettering.edu
*/

	
#ifndef PDFDEV_H
#define PDFDEV_H

#include "numbers.h"
#include "pdfobj.h"

typedef signed long spt_t;

extern void dev_init(double scale, double x_offset, double y_offset);
extern void dev_set_verbose (void);
extern double pdf_dev_scale (void);
extern void dev_close (void);
extern void dev_read_mapfile (char *filename);
extern int dev_locate_font (const char *name, spt_t ptsize);
extern int dev_font_tfm (int dev_font_id);
extern spt_t dev_font_sptsize (int dev_font_id);
extern void dev_close_all_fonts (void);
extern void dev_bop (void);
extern void dev_eop (void);
extern void dev_reselect_font (void);
extern void dev_set_string (spt_t xpos, spt_t ypos, unsigned char *ch,
			    int len, spt_t width, int font_id);
extern void dev_set_page_size (double width, double height);
extern void dev_rule (spt_t xpos, spt_t ypos, spt_t width, spt_t height);
extern double dev_phys_x (void);
extern double dev_phys_y (void);
extern void dev_begin_rgb_color (double r, double g, double b);
extern void dev_set_def_rgb_color (double r, double g, double b);
extern void dev_begin_cmyk_color (double c, double m, double y, double k);
extern void dev_set_def_cmyk_color (double c, double m, double y, double k);
extern void dev_begin_gray (double value);
extern void dev_set_def_gray (double value);
extern void dev_begin_named_color (char *s);
extern void dev_set_def_named_color (char *s);
extern void dev_end_color (void);
extern void dev_do_color(void);
extern void dev_bg_rgb_color (double r, double g, double b);
extern void dev_bg_cmyk_color (double c, double m, double y, double k);
extern void dev_bg_gray (double value);
extern void dev_bg_named_color (char *s);
extern void dev_begin_xform (double xscale, double yscale, double
			     rotate, double x_user, double y_user);
extern void dev_end_xform (void);
extern int  dev_xform_depth (void);
extern void dev_close_all_xforms (int depth);

extern void dev_add_comment (char *comment);
extern void dev_do_special (void *buffer, UNSIGNED_QUAD size,
			    spt_t x_user, spt_t y_user);
extern double dev_page_height(void);
extern double dev_page_width(void);
/* graphics_mode() would normally be local, but it is needed by the
   MetaPost inclusion routine */
extern void graphics_mode (void);
extern void dev_stack_depth (unsigned int depth);
extern void dev_tag_depth (void);
extern void dev_set_box (void);
extern void dev_untag_depth (void);
extern void dev_expand_box (spt_t width, spt_t height, spt_t depth);
extern void dev_link_annot (unsigned char flag);
#endif /* PDFDEV_H */





