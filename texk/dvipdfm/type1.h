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

	
#ifndef TYPE1_H
#define TYPE1_H

#include "pdfobj.h"

extern void type1_set_verbose(void);
extern pdf_obj *type1_font_resource (int type1_id);
extern char *type1_font_used (int type1_id);
extern void type1_disable_partial (void);
extern int type1_font (const char *tex_name, int tfm_font_id, char
		       *resource_name, int encoding_id, int remap);
extern void type1_set_mapfile (const char *name);
extern void type1_close_all (void);
#endif /* TYPE1_H */
