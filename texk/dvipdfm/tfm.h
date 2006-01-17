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

	
#ifndef TFM_H

#define TFM_H

#include "numbers.h"

extern void tfm_set_verbose(void);
extern void tfm_set_debug(void);

extern int tfm_open(const char * tex_font_name);
extern void tfm_close_all (void);

extern double tfm_get_width (int font_id, UNSIGNED_QUAD ch);
extern double tfm_get_height (int font_id, UNSIGNED_QUAD ch);
extern double tfm_get_depth (int font_id, UNSIGNED_QUAD ch);
extern fixword tfm_get_fw_width (int font_id, UNSIGNED_QUAD ch);
extern fixword tfm_get_fw_height (int font_id, UNSIGNED_QUAD ch);
extern fixword tfm_get_fw_depth (int font_id, UNSIGNED_QUAD ch);
extern fixword tfm_string_width (int font_id, unsigned char *s,
				 unsigned len);
extern fixword tfm_string_depth (int font_id, unsigned char *s,
				 unsigned len);
extern fixword tfm_string_height (int font_id, unsigned char *s,
				  unsigned len);
extern double tfm_get_space (int font_id);
extern double tfm_get_it_slant (int font_id);
extern double tfm_get_x_height (int font_id);

extern UNSIGNED_PAIR tfm_get_firstchar (int font_id);
extern UNSIGNED_PAIR tfm_get_lastchar (int font_id);

extern double tfm_get_design_size (int font_id);
extern double tfm_get_max_height (int font_id);
extern double tfm_get_max_width (int font_id);
extern int tfm_is_fixed_width (int font_id);
extern double tfm_get_max_depth (int font_id);

#endif /* TFM_H */
