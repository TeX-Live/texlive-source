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

	
#ifndef JPEG_H

#define JPEG_H

#include <stdio.h>
#include "pdfobj.h"
#include "pdfspecial.h"

struct jpeg 
{
  unsigned width, height;
  unsigned bits_per_color;
  FILE *file;
  unsigned colors;
};

extern struct jpeg *jpeg_open (FILE *file);
extern int jpeg_headers (struct jpeg *jpeg);
extern void jpeg_close (struct jpeg *jpeg);
extern int check_for_jpeg (FILE *file);

#endif /* JPEG_H */
