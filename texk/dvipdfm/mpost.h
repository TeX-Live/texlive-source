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

#include "pdfobj.h"
#include "pdfspecial.h"

#ifndef MPOST_H
# define MPOST_H
extern int check_for_mp (FILE *image_file);
extern pdf_obj *mp_include (FILE *image_file,  struct xform_info *p,
		     char *res_name, double x_user, double y_user);
extern int do_raw_ps_special (char **start, char *end, int cleanup,
			      double x_user, double y_user);
extern void mp_cleanup(int sloppy_is_okay);
extern struct xform_info *texfig_info (void);
extern void mp_eop_cleanup(void);
#endif /* MPOST_H */


