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
#include <stdlib.h>
#include <kpathsea/config.h>
#include <kpathsea/c-ctype.h>
#include "system.h"
#include "config.h"
#include "mfileio.h"
#include "mem.h"
#include "pdfobj.h"
#include "thumbnail.h"

#ifdef HAVE_LIBPNG
#include <png.h>
#include "pngimage.h"

#define TMP "/tmp"

static char *guess_name (const char *thumb_filename)
{
  /* Build path name for anticipated thumbnail image */
  char *tmpdir, *tmpname;
  if (!(tmpdir = getenv ("TMPDIR")) &&
      !(tmpdir = getenv ("TEMP")) &&
      !(tmpdir = getenv ("TMP"))) 
    tmpdir = TMP;
  tmpname = NEW (strlen(tmpdir)+strlen(thumb_filename)+strlen(DIR_SEP_STRING)+1,
		 char);
  strcpy (tmpname, tmpdir);
  if (!IS_DIR_SEP (tmpname[strlen(tmpname)-1])) {
    strcat (tmpname, DIR_SEP_STRING);
  }
  strcat (tmpname, thumb_filename);
  return tmpname;
}

static char thumbnail_remove_opt = 0;
void thumb_remove(void)
{
  thumbnail_remove_opt = 1;
}

static unsigned char sigbytes[4];

pdf_obj *do_thumbnail (const char *thumb_filename) 
{
  pdf_obj *image_stream = NULL, *image_ref = NULL;
  int found_in_cwd = 0;
  FILE *thumb_file;
  char *guess_filename = NULL;
  guess_filename = guess_name (thumb_filename);
  if ((thumb_file = MFOPEN (thumb_filename, FOPEN_RBIN_MODE))) {
    found_in_cwd = 1;
  } else if (!(thumb_file = MFOPEN (guess_filename, FOPEN_RBIN_MODE))) {
    fprintf (stderr, "\nNo thumbnail file\n");
    return NULL;
  }
  if (fread (sigbytes, 1, sizeof(sigbytes), thumb_file) !=
      sizeof(sigbytes) ||
      (!png_check_sig (sigbytes, sizeof(sigbytes)))) {
    fprintf (stderr, "\nThumbnail not a png file! Skipping\n");
    return NULL;
  }
  rewind (thumb_file);

  if ((image_stream = start_png_image (thumb_file, NULL))) {
    image_ref = pdf_ref_obj (image_stream);
    pdf_release_obj (image_stream);
  } else {
    image_ref = NULL;
  }
  if (thumbnail_remove_opt && found_in_cwd) 
    remove (thumb_filename);
  else if (thumbnail_remove_opt)
    remove (guess_filename);
  if (guess_filename)
    RELEASE (guess_filename);
  MFCLOSE (thumb_file);
  return image_ref;
}

#endif /* HAVE_LIBPNG */



