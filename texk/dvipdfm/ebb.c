/*  $Header$

    This is ebb, a bounding box extraction program.
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
#include <time.h>
#include <string.h>
#include "system.h"
#include "mem.h"
#include "mfileio.h"
#include "numbers.h"
#include "pdfobj.h"
#include "jpeg.h"
#include "pdfparse.h"

#include "config.h"

#ifdef HAVE_LIBPNG
#include "pngimage.h"
#include <png.h>
#endif

#define EBB_PROGRAM "ebb"
#define EBB_VERSION "Version 0.5.2"

static void usage (void)
{
  fprintf (stderr, "%s, version %s, Copyright (C) 1998, 1999 by Mark A. Wicks\n",
	   EBB_PROGRAM, EBB_VERSION);
  fprintf (stderr, "ebb comes with ABSOLUTELY NO WARRANTY.\n");
  fprintf (stderr, "This is free software, and you are welcome to redistribute it\n");
  fprintf (stderr, "under certain conditions.  Details are distributed with the software.\n");
  fprintf (stderr, "\nUsage: ebb [-v] [-b] [files]\n");
  fprintf (stderr, "\t-b\t\tWrite .bb file in binary mode\n");
  fprintf (stderr, "\t-v\t\tVerbose\n");
  exit(1);
}

static char verbose = 0;

static void do_time(FILE *file)
{
  time_t current_time;
  struct tm *bd_time;
  time(&current_time);
  bd_time = localtime(&current_time);
  fprintf (file, "%%%%CreationDate: %s\n", asctime (bd_time));
}

char *extensions[] = {
  ".jpeg", ".JPEG", ".jpg", ".JPG", ".pdf", ".PDF", ".png", ".PNG"
};

static char *make_bb_filename (const char *name)
{
  int i;
  char *result;
#ifdef HAVE_XBASENAME   
  name = xbasename (name);
#else   
  name = basename (name);
#endif
  for (i=0; i<sizeof(extensions)/sizeof(extensions[0]); i++) {
    if (strlen (extensions[i]) < strlen(name) &&
	!strncmp (name+strlen(name)-strlen(extensions[i]),
		  extensions[i], strlen(extensions[i])))
      break;
  }
  if (i == sizeof(extensions)/sizeof(extensions[0])) {
    fprintf (stderr,
	     "ebb: Warning: %s: Filename does not end in a recognizeable extension.\n",
	     name);
    result = NEW (strlen(name)+3, char);
    strcpy (result, name);
  }
  else { /* Remove extension */
    result = NEW (strlen(name)+3-strlen(extensions[i])+1, char);
    strncpy (result, name, strlen(name)-strlen(extensions[i]));
    result[strlen(name)-strlen(extensions[i])] = 0;
  }
    strcat (result, ".bb");
  return result;
}

static char *bb_file_mode = FOPEN_W_MODE;

static void write_bb (char *filename, int bbllx, int bblly, int bburx,
		      int bbury) 
{
  char *bbfilename;
  FILE *bbfile;
  if (verbose)
    fprintf (stderr, "okay\n");
  bbfilename = make_bb_filename (filename);
  if ((bbfile = MFOPEN (bbfilename, bb_file_mode)) == NULL) {
    fprintf (stderr, "Unable to open output file: %s\n", bbfilename);
    return;
  }
  if (verbose) {
    fprintf (stderr, "Writing to %s:  ", bbfilename);
    fprintf (stderr, "Bounding box:  %d %d %d %d\n", bbllx, bblly,
	     bburx, bbury);
  }
  fprintf (bbfile, "%%%%Title: %s\n", filename);
  fprintf (bbfile, "%%%%Creator: %s %s\n", EBB_PROGRAM, EBB_VERSION);
  fprintf (bbfile, "%%%%BoundingBox: %d %d %d %d\n",
	   bbllx, bblly, bburx, bbury);
  do_time(bbfile);
  RELEASE (bbfilename);
  MFCLOSE (bbfile);
  return;
}


#define PIX2PT (72.0/100.0)
void do_jpeg (FILE *file, char *filename)
{
  struct jpeg *jpeg;
  if (verbose) {
    fprintf (stderr, "%s looks like a JPEG file...", filename);
  }
  jpeg = NEW (1, struct jpeg);
  jpeg -> file = file;
  if (!jpeg_headers (jpeg)) {
        fprintf (stderr, "\n%s: Corrupt JPEG file?\n", filename);
    MFCLOSE (file);
    RELEASE (jpeg);
    return;
  }
  write_bb (filename, 0, 0,
	    ROUND(jpeg -> width * PIX2PT,1.0),
	    ROUND(jpeg -> height * PIX2PT,1.0));
  RELEASE (jpeg);
  return;
}
#ifdef HAVE_LIBPNG
void do_png (FILE *file, char *filename)
{
  png_structp png_ptr;
  png_infop info_ptr;
  unsigned long width, height;
  rewind (file);
  if (verbose) {
    fprintf (stderr, "%s looks like a PNG file...", filename);
  }
  if (!(png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING,    
					  NULL, NULL, NULL)) ||
      !(info_ptr = png_create_info_struct (png_ptr))) {
    fprintf (stderr, "\n\n%s: Corrupt PNG file?\n", filename);
    if (png_ptr)
      png_destroy_read_struct(&png_ptr, NULL, NULL);
    MFCLOSE (file);
    return;
  }
  png_init_io (png_ptr, file);
  /* Read PNG header */
  png_read_info (png_ptr, info_ptr);
  width = png_get_image_width(png_ptr, info_ptr);
  height = png_get_image_height(png_ptr, info_ptr);
  write_bb (filename, 0, 0,
	    ROUND(width * PIX2PT,1.0),
	    ROUND(height * PIX2PT,1.0));
  if (info_ptr)
    png_destroy_info_struct(png_ptr, &info_ptr);
  if (png_ptr)
    png_destroy_read_struct(&png_ptr, NULL, NULL);
  return;
}
#endif /* HAVE_LIBPNG */

void do_pdf (FILE *file, char *filename)
{
  pdf_obj *trailer, *catalog, *page_tree, *media_box, *crop_box;
  pdf_obj *kids_ref, *kids, *tmp1;
  if (verbose) {
    fprintf (stderr, "%s looks like a PDF file...", filename);
  }
  if ((trailer = pdf_open (file)) == NULL) {
    fprintf (stderr, "%s: Corrupt PDF file?\n", filename);
    return;
  };
  if ((catalog = pdf_deref_obj(pdf_lookup_dict (trailer,"Root"))) ==
      NULL) {
    fprintf (stderr, "\nCatalog isn't where I expect it.\n");
    return;
  }
  /* Got catalog, so done with trailer */
  pdf_release_obj (trailer);
  /* Lookup page tree in catalog */
  page_tree = pdf_deref_obj (pdf_lookup_dict (catalog, "Pages"));
  pdf_release_obj (catalog);
  /* Media box can be inherited so start looking for it now */
  media_box = pdf_deref_obj (pdf_lookup_dict (page_tree, "MediaBox"));
  crop_box = pdf_deref_obj (pdf_lookup_dict (page_tree, "CropBox"));
  while ((kids_ref = pdf_lookup_dict (page_tree, "Kids")) != NULL) {
    kids = pdf_deref_obj (kids_ref);
    pdf_release_obj (page_tree);
    page_tree = pdf_deref_obj (pdf_get_array(kids, 0));
    pdf_release_obj (kids);
    /* Replace MediaBox if it's here */
    tmp1 = pdf_deref_obj(pdf_lookup_dict (page_tree, "MediaBox"));
    if (tmp1 && media_box)
      pdf_release_obj (media_box);
    if (tmp1) 
      media_box = tmp1;
    /* Likewise for CropBox */
    tmp1 = pdf_deref_obj(pdf_lookup_dict (page_tree, "CropBox"));
    if (tmp1 && crop_box)
      pdf_release_obj (crop_box);
    if (tmp1) 
      crop_box = tmp1;
  }
  /* At this point, we should have the media box for the first page */ 
  {
    pdf_obj *bbllx, *bblly, *bburx, *bbury;
    if (crop_box) {
      pdf_release_obj (media_box);
      media_box = crop_box;
      crop_box = NULL;
    }
    if ((bbllx = pdf_get_array (media_box, 0)) == NULL ||
	(bblly = pdf_get_array (media_box, 1)) == NULL ||
	(bburx = pdf_get_array (media_box, 2)) == NULL ||
	(bbury = pdf_get_array (media_box, 3)) == NULL) {
      fprintf (stderr, "Invalid mediabox\n");
    } else
      write_bb (filename,
		(int) pdf_number_value (bbllx), (int) pdf_number_value (bblly),
		(int) pdf_number_value (bburx), (int) pdf_number_value (bbury));
  }
  pdf_release_obj (media_box);
  pdf_release_obj (page_tree);
  pdf_close();
}

FILE *inputfile;

int main (int argc, char *argv[]) 
{
  kpse_set_program_name (argv[0], NULL);
  argc -= 1;
  argv += 1;
  if (argc == 0)
    usage();
  while (argc > 0 && *argv[0] == '-') {
    switch (*(argv[0]+1)) {
    case 'b':
      bb_file_mode = FOPEN_WBIN_MODE;
    case 'v':
      verbose = 1;
      argc -= 1;
      argv += 1;
      break;
    case 'h':  
      usage();
      argc -= 1;
      argv += 1;
      break;
    default:
      usage();
    }
  }
  for (; argc > 0; argc--, argv++) {
    char *kpse_file_name;
    if (!(kpse_file_name = kpse_find_pict(argv[0])) ||
        (inputfile = MFOPEN (kpse_file_name, FOPEN_RBIN_MODE)) == NULL)  {
      fprintf (stderr, "Can't find file (%s)...skipping\n", argv[0]);
      continue;
    }
    if (check_for_jpeg (inputfile)) {
      do_jpeg(inputfile, kpse_file_name);
      MFCLOSE (inputfile);
      continue;
    }
    if (check_for_pdf (inputfile)) {
      do_pdf(inputfile, kpse_file_name);
      MFCLOSE (inputfile);
      continue;
    }
#ifdef HAVE_LIBPNG
    if (check_for_png (inputfile)) {
      do_png(inputfile, kpse_file_name);
      MFCLOSE (inputfile);
      continue;
    }
#endif /* HAVE_LIBPNG */
    fprintf (stderr, "Can't handle file type for file named %s\n",
	     argv[0]);
  }
  return 0;
}

/* The following is here to prevent a link error.  Other routines
   that get linked with ebb call this, but it isn't actually needed
   for ebb  */

pdf_obj *get_reference (char **start, char *end)
{
  fprintf (stderr, "get_reference():  This should never have been called\n");
  return NULL;
}

void error_cleanup(void)
{
  /* For now, this is a do nothing routine, but the stub is required
     by error.h */
  return;
}
