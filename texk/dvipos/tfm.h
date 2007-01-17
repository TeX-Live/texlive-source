/*  dvipos-20070107

    Copyright (C) 2003 by Jin-Hwan Cho <chofchof@ktug.or.kr>

    Copyright (C) 2002 by Jin-Hwan Cho and Shunsaku Hirata,
    the dvipdfmx project team <dvipdfmx@project.ktug.or.kr>
    
    Copyright (C) 1998, 1999 by Mark A. Wicks <mwicks@kettering.edu>

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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
*/

#ifndef _TFM_H_
#define _TFM_H_

#include "dvicore.h"

extern int tfm_open (const char * tex_name, int must_exist);
extern void tfm_close_all (void);

extern double tfm_get_width (int font_id, UNSIGNED_QUAD ch);
extern double tfm_get_height (int font_id, UNSIGNED_QUAD ch);
extern double tfm_get_depth (int font_id, UNSIGNED_QUAD ch);
extern long tfm_get_fw_width (int font_id, UNSIGNED_QUAD ch);
extern long tfm_get_fw_height (int font_id, UNSIGNED_QUAD ch);
extern long tfm_get_fw_depth (int font_id, UNSIGNED_QUAD ch);
extern long tfm_string_width (int font_id, unsigned char *s,
				 unsigned len);
extern long tfm_string_depth (int font_id, unsigned char *s,
				 unsigned len);
extern long tfm_string_height (int font_id, unsigned char *s,
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
extern char is_jfm (int font_id);
extern char is_vertical (int font_id);

#endif /* _TFM_H_ */
