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

	
/*
    This is dvipdf, a DVI to PDF translator.
    Copyright (C) 1998  by Mark A. Wicks

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

	
#ifndef PDFPARSE_H

#define PDFPARSE_H
#include "numbers.h"
#include "pdfobj.h"

extern int is_an_int(const char *s);
extern int is_a_number(const char *s);
extern char *parse_ident (char **start, char *end);
extern char *parse_val_ident (char **start, char *end);
extern char *parse_c_ident (char **start, char *end);
extern char *parse_number (char **start, char*end);
extern char *parse_unsigned (char **start, char*end);
extern void parse_crap (char **start, char *end);
extern void skip_white (char **start, char *end);
extern void skip_line (char **start, char *end);

extern char *parse_c_string (char **start, char*end);
extern pdf_obj *parse_pdf_string (char **start, char*end);
extern pdf_obj *parse_pdf_name (char **start, char*end);
extern pdf_obj *parse_pdf_array (char **start, char*end);
extern pdf_obj *parse_pdf_object (char **start, char*end);
extern pdf_obj *parse_pdf_dict (char **start, char*end);
extern pdf_obj *parse_pdf_boolean (char **start, char*end);
extern pdf_obj *parse_pdf_null (char **start, char*end);
extern char *parse_pdf_reference (char **start, char*end);
extern char *parse_opt_ident (char **start, char*end);

extern void parse_key_val (char **start, char *end, char **key, char **val);
extern void dump (char *start, char *end);

#endif  /* PDFPARSE_H */
