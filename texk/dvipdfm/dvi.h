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

#ifndef _DVI_H_
#define _DVI_H_	

#include "error.h"
#include "numbers.h"
#include "pdfdev.h"

extern error_t dvi_init (char *dvi_filename, char *pdf_filename, double mag, double x_offset,
			 double y_offset);

extern void dvi_set_verbose (void);
extern void dvi_set_debug (void);

extern void dvi_close (void);  /* Closes data structures created by dvi_open */
extern double dvi_tell_mag (void);
extern double dvi_unit_size (void);

extern void dvi_vf_init (int dev_font_id);
extern void dvi_vf_finish (void);

extern void dvi_set_font (int font_id);
extern void dvi_set (SIGNED_QUAD ch);
extern void dvi_rule (SIGNED_QUAD width, SIGNED_QUAD height);
extern void dvi_right (SIGNED_QUAD x);
extern void dvi_put (SIGNED_QUAD ch);
extern void dvi_push (void);
extern void dvi_pop (void);
extern void dvi_w0 (void);
extern void dvi_w (SIGNED_QUAD ch);
extern void dvi_x0(void);
extern void dvi_x (SIGNED_QUAD ch);
extern void dvi_down (SIGNED_QUAD y);
extern void dvi_y (SIGNED_QUAD ch);
extern void dvi_y0(void);
extern void dvi_z (SIGNED_QUAD ch);
extern void dvi_z0(void);
extern double dvi_dev_xpos(void);
extern double dvi_dev_ypos(void);
extern unsigned dvi_npages (void);
extern void dvi_do_page(unsigned n);
extern int dvi_locate_font (char *name, spt_t ptsize);
extern void dvi_compute_boxes (unsigned char boxes);
#endif /* _DVI_H_ */
