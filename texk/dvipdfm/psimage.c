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

#include <stdio.h>
#include "system.h"
#include "config.h"
#include "mem.h"
#include "mfileio.h"
#include "pdfobj.h"
#include "psimage.h"
#include "epdf.h"

static char * distiller_template = NULL;

void set_distiller_template (char *s) 
{
  distiller_template = NEW (strlen(s)+1, char);
  strcpy (distiller_template, s);
  return;
}

#define need(n) { unsigned k=(n); \
                 if (size+k>max_size) { \
                     max_size += k+128; \
                     result=RENEW(result,max_size,char); \
                       }}

static char *last_dot (char *s)
{
  char *end;
  end = s+strlen(s);
  while (--end > s) {
    if (*end == '.')
      return end;
  }
  return NULL;
}

#ifdef HAVE_SYSTEM  /* No need to build a command line if we don't
		       have system() */
static char *build_command_line (char *psname, char *pdfname)
{
  char *result = NULL, *current;
  int size = 0, max_size = 0;
  if (distiller_template) {
    need(strlen(distiller_template)+1);
    for (current =distiller_template; *current != 0; current ++) {
      if (*current == '%') {
	switch (*(++current)) {
	case 'o': /* Output file name */
	  need(strlen(pdfname));
	  strcpy (result+size, pdfname);
	  size+=strlen(pdfname);
	  break;
	case 'i': /* Input filename */
	  need(strlen(psname));
	  strcpy (result+size, psname);
	  size+=strlen(psname);
	  break;
	case 'b': 
	  {
	    char *last;
	    need(strlen(psname));
	    if ((last = last_dot (psname))) {
	      strncpy (result+size, psname, last-psname);
	      size += last-psname;
	    } else {
	      strcpy (result+size, psname);
	      size += strlen(psname);
	    }
	  }
	case 0:
	  break;
	case '%':
	  result[size++] = '%';
	}
      } else {
	result[size++] = *current;
      }
      result[size] = 0;
    }
  } else {
    fprintf (stderr, "\nConfig file contains no template to perform PS -> PDF conversion\n");
  }
  return result;
}
#endif

pdf_obj *ps_include (char *file_name, 
		     struct xform_info *p,
		     char *res_name, double x_user, double y_user)
{
#ifdef HAVE_SYSTEM
  pdf_obj *result = NULL;
  char *tmp, *cmd;
  FILE *pdf_file = NULL;
  /* Get a full qualified tmp name */
  tmp = tmpnam (NULL);
  if ((cmd = build_command_line (file_name, tmp))) {
    if (!system (cmd) && (pdf_file = MFOPEN (tmp, FOPEN_RBIN_MODE))) {
      result = pdf_include_page (pdf_file, p, res_name);
    } else {
      fprintf (stderr, "\nConversion via ->%s<- failed\n", cmd);
    }
    if (pdf_file) {
      MFCLOSE (pdf_file);
      remove (tmp);
    }
    RELEASE (cmd);
  }
  return result;
#else
  fprintf (stderr, "\n\nCannot include PS/EPS files unless you have and enable system() command.\n");
  return NULL;
#endif
}

int check_for_ps (FILE *image_file) 
{
  rewind (image_file);
  mfgets (work_buffer, WORK_BUFFER_SIZE, image_file);
  if (!strncmp (work_buffer, "%!", 2))
    return 1;
  return 0;
}

void psimage_close(void)
{
  if (distiller_template)
    RELEASE (distiller_template);
}
