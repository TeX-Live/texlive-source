/*  

    This is extractbb, a bounding box extraction program.

    Copyright (C) 2008-2012 by Jin-Hwan Cho <chofchof@ktug.or.kr>

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
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <time.h>
#include <string.h>
#include "system.h"
#include "mem.h"
#include "error.h"
#include "mfileio.h"
#include "numbers.h"
#include "pdfobj.h"
#include "pdfparse.h"

#include "config.h"

#ifdef HAVE_LIBPNG
#include <png.h>
#endif

#include "xbb.h"

#define XBB_PROGRAM "extractbb"
#define XBB_VERSION "Version 0.2"

static int xbb_output_mode = XBB_OUTPUT;

static void usage(void)
{
  fprintf (stderr, "%s, version %s, Copyright (C) 2008 by Jin-Hwan Cho\n",
	   XBB_PROGRAM, XBB_VERSION);
  fprintf (stderr, "A bounding box extraction utility from PDF, PNG, and JPG.\n");
  fprintf (stdout, "\nThis is free software; you can redistribute it and/or modify\n");
  fprintf (stdout, "it under the terms of the GNU General Public License as published by\n");
  fprintf (stdout, "the Free Software Foundation; either version 2 of the License, or\n");
  fprintf (stdout, "(at your option) any later version.\n");
  fprintf (stderr, "\nUsage: %s [-v] [-b] [-m|-x] [files]\n", XBB_PROGRAM);
  fprintf (stderr, "\t-b\tWrite output file in binary mode\n");
  if(xbb_output_mode == EBB_OUTPUT) {
    fprintf (stderr, "\t-m\tOutput .bb  file used in DVIPDFM (default)\n");
    fprintf (stderr, "\t-x\tOutput .xbb file used in DVIPDFMx\n");
  } else {
    fprintf (stderr, "\t-m\tOutput .bb  file used in DVIPDFM\n");
    fprintf (stderr, "\t-x\tOutput .xbb file used in DVIPDFMx (default)\n");
  }
  fprintf (stderr, "\t-v\tVerbose\n");
  exit(1);
}

static char verbose = 0;

static void do_time(FILE *file)
{
  time_t current_time;
  struct tm *bd_time;

  time(&current_time);
  bd_time = localtime(&current_time);
  fprintf(file, "%%%%CreationDate: %s\n", asctime(bd_time));
}

const char *extensions[] = {
  ".jpeg", ".JPEG", ".jpg", ".JPG", ".pdf", ".PDF", ".png", ".PNG"
};

static char *make_xbb_filename(const char *name)
{
  int i;
  char *result;

  for (i = 0; i < sizeof(extensions) / sizeof(extensions[0]); i++) {
    if (strlen(extensions[i]) < strlen(name) &&
	strncmp(name+strlen(name)-strlen(extensions[i]), extensions[i], strlen(extensions[i])) == 0)
      break;
  }
  if (i == sizeof(extensions) / sizeof(extensions[0])) {
    fprintf(stderr, "Warning: %s: Filename does not end in a recognizeable extension.\n", name);
    result = NEW(strlen(name)+3, char);
    strcpy(result, name);
  } else { /* Remove extension */
    result = NEW(strlen(name)+3-strlen(extensions[i])+1, char);
    strncpy(result, name, strlen(name)-strlen(extensions[i]));
    result[strlen(name)-strlen(extensions[i])] = 0;
  }
  strcat(result, (xbb_output_mode == XBB_OUTPUT ? ".xbb" : ".bb"));
  return result;
}

static const char *xbb_file_mode = FOPEN_W_MODE;

static void write_xbb(char *fname, int bbllx, int bblly, int bburx, int bbury) 
{
  char *outname;
  FILE *fp;

  outname = make_xbb_filename(fname);
  if ((fp = MFOPEN(outname, xbb_file_mode)) == NULL) {
    fprintf(stderr, "Unable to open output file: %s\n", outname);
    RELEASE(outname);
    return;
  }
  if (verbose) {
    fprintf(stderr, "Writing to %s: ", outname);
    fprintf(stderr, "Bounding box: %d %d %d %d\n", bbllx, bblly, bburx, bbury);
  }
  fprintf(fp,"%%%%Title: %s\n", fname);
  fprintf(fp,"%%%%Creator: %s %s\n", XBB_PROGRAM, XBB_VERSION);
  fprintf(fp,"%%%%BoundingBox: %d %d %d %d\n", bbllx, bblly, bburx, bbury);
  do_time(fp);
  RELEASE(outname);
  MFCLOSE(fp);
}

typedef enum {
  JM_SOF0  = 0xc0, JM_SOF1  = 0xc1, JM_SOF2  = 0xc2, JM_SOF3  = 0xc3,
  JM_SOF5  = 0xc5, JM_DHT   = 0xc4, JM_SOF6  = 0xc6, JM_SOF7  = 0xc7,
  JM_SOF9  = 0xc9, JM_SOF10 = 0xca, JM_SOF11 = 0xcb, JM_DAC   = 0xcc,
  JM_SOF13 = 0xcd, JM_SOF14 = 0xce, JM_SOF15 = 0xcf,

  JM_RST0  = 0xd0, JM_RST1  = 0xd1, JM_RST2  = 0xd2, JM_RST3  = 0xd3,
  JM_RST4  = 0xd4, JM_RST5  = 0xd5, JM_RST6  = 0xd6, JM_RST7  = 0xd7,

  JM_SOI   = 0xd8, JM_EOI   = 0xd9, JM_SOS   = 0xda, JM_DQT   = 0xdb,
  JM_DNL   = 0xdc, JM_DRI   = 0xdd, JM_DHP   = 0xde, JM_EXP   = 0xdf,

  JM_APP0  = 0xe0, JM_APP2  = 0xe2, JM_APP14 = 0xee, JM_APP15 = 0xef,

  JM_COM   = 0xfe
} JPEG_marker;

static JPEG_marker JPEG_get_marker (FILE *fp)
{
  int c = fgetc(fp);
  if (c != 255) return -1;
  for (;;) {
    c = fgetc(fp);
    if (c < 0) return -1;
    else if (c > 0 && c < 255) return c;
  }
  return -1;
}

static int check_for_jpeg (FILE *fp)
{
  unsigned char jpeg_sig[2];
  rewind(fp);
  if (fread(jpeg_sig, sizeof(unsigned char), 2, fp) != 2) return 0;
  else if (jpeg_sig[0] != 0xff || jpeg_sig[1] != JM_SOI) return 0;
  return 1;
} 

static int jpeg_get_info (FILE *fp, int *width, int *height)
{
  JPEG_marker marker;
  unsigned short length;
  int  count;
  float xdensity = 1.0, ydensity = 1.0;
  char app_sig[128];
      
  if (!check_for_jpeg(fp)) {
    rewind(fp);
    return -1;
  } 
  rewind(fp);
  count = 0;
  while ((marker = JPEG_get_marker(fp)) >= 0) {
    if (marker == JM_SOI  || (marker >= JM_RST0 && marker <= JM_RST7)) {
      count++; continue;
    }
    length = get_unsigned_pair(fp) - 2;
    switch (marker) {
    case JM_SOF0:  case JM_SOF1:  case JM_SOF2:  case JM_SOF3:
    case JM_SOF5:  case JM_SOF6:  case JM_SOF7:  case JM_SOF9:
    case JM_SOF10: case JM_SOF11: case JM_SOF13: case JM_SOF14:
    case JM_SOF15:
      get_unsigned_byte(fp);
      if (xbb_output_mode != XBB_OUTPUT) { /* EBB_OUTPUT */
        xdensity = ydensity = 72.0 / 100.0;
      }
      *height = (int)(get_unsigned_pair(fp) * ydensity + 0.5);
      *width  = (int)(get_unsigned_pair(fp) * xdensity + 0.5);
      return 0;
    case JM_APP0:
      if (length > 5) {
	if (fread(app_sig, sizeof(char), 5, fp) != 5) return -1;
	length -= 5;
	if (!memcmp(app_sig, "JFIF\000", 5)) {
          int units, xden, yden;
          get_unsigned_pair(fp);
          units = (int)get_unsigned_byte(fp);
          xden = (int)get_unsigned_pair(fp);
          yden = (int)get_unsigned_pair(fp);
          switch (units) {
          case 1: /* pixels per inch */
            xdensity = 72.0 / xden;
            ydensity = 72.0 / yden;
            break;
          case 2: /* pixels per centimeter */
            xdensity = 72.0 / 2.54 / xden;
            ydensity = 72.0 / 2.54 / yden;
            break;
          default:
            break;
          }
          length -= 7;
	}
      }
      seek_relative(fp, length);
      break;
    default:
      seek_relative(fp, length);
      break;
    }
    count++;
  }
  return -1; 
}

static void do_jpeg (FILE *fp, char *filename)
{
  int width, height;

  if (jpeg_get_info(fp, &width, &height) < 0) {
    fprintf (stderr, "%s does not look like a JPEG file...\n", filename);
    return;
  }
  write_xbb(filename, 0, 0, width, height);
  return;
}

#ifdef HAVE_LIBPNG
static int check_for_png (FILE *png_file) 
{
  unsigned char sigbytes[4];
  rewind (png_file);
  if (fread(sigbytes, 1, sizeof(sigbytes), png_file) != sizeof(sigbytes) ||
      (png_sig_cmp (sigbytes, 0, sizeof(sigbytes)))) return 0;
  else return 1;
}             

static int png_get_info(FILE *png_file, int *width, int *height)
{
  png_structp png_ptr;
  png_infop   png_info_ptr;
  png_uint_32 xppm, yppm;

  rewind(png_file);

  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL || (png_info_ptr = png_create_info_struct(png_ptr)) == NULL) {
    if (png_ptr) png_destroy_read_struct(&png_ptr, NULL, NULL);
    return -1;
  }
  png_init_io(png_ptr, png_file);
  png_read_info(png_ptr, png_info_ptr);

  *width  = (int)png_get_image_width(png_ptr, png_info_ptr);
  *height = (int)png_get_image_height(png_ptr, png_info_ptr);

  if (xbb_output_mode == XBB_OUTPUT) {
    xppm = png_get_x_pixels_per_meter(png_ptr, png_info_ptr);
    yppm = png_get_y_pixels_per_meter(png_ptr, png_info_ptr);
    if (xppm > 0)
      *width = (int)(*width * 72.0 / 0.0254 / xppm + 0.5);
    if (yppm > 0)
      *height = (int)(*height * 72.0 / 0.0254 / yppm + 0.5);
  } else { /* EBB_OUTPUT */
    *width = (int)(*width * 72.0 / 100.0 + 0.5);
    *height = (int)(*height * 72.0 / 100.0 + 0.5);
  }
  return 0;
}

static void do_png (FILE *fp, char *filename)
{
  int width, height;

  if (png_get_info(fp, &width, &height) < 0) {
    fprintf (stderr, "%s does not look like a PNG file...\n", filename);
    return;
  }
  write_xbb(filename, 0, 0, width, height);
  return;
}
#endif /* HAVE_LIBPNG */

static int rect_equal (pdf_obj *rect1, pdf_obj *rect2)
{
  int i;
  if (!rect1 || !rect2) return 0;
  for (i = 0; i < 4; i++) {
    if (pdf_number_value(pdf_get_array(rect1, i)) != pdf_number_value(pdf_get_array(rect2, i))) return 0;
  }
  return 1;
}

static int pdf_get_info (FILE *image_file, char *filename, int *llx, int *lly, int *urx, int *ury)
{
  pdf_obj *page_tree;
  pdf_obj *bbox;
  pdf_file *pf;

  page_tree = NULL;
  {
    pdf_obj *trailer, *catalog;

    pf = pdf_open(filename, image_file);
    if (!pf)
      return -1;

    trailer = pdf_file_get_trailer(pf);

    if (pdf_lookup_dict(trailer, "Encrypt")) {
      WARN("This PDF document is encrypted.");
      pdf_release_obj(trailer);
      pdf_close(pf);
      return -1;
    }
    catalog = pdf_deref_obj(pdf_lookup_dict(trailer, "Root"));
    if (!catalog) {
      WARN("Catalog isn't where I expect it.");
      pdf_close(pf);
      return -1;
    }
    pdf_release_obj(trailer);
    page_tree = pdf_deref_obj(pdf_lookup_dict(catalog, "Pages"));
    pdf_release_obj(catalog);
  }
  if (!page_tree) {
    WARN("Page tree not found.");
    pdf_close(pf);
    return -1;
  }
  {
    pdf_obj *kids_ref, *kids;
    pdf_obj *crop_box;
    pdf_obj *tmp;

    tmp  = pdf_lookup_dict(page_tree, "MediaBox");
    bbox = tmp ? pdf_deref_obj(tmp) : NULL;
    tmp  = pdf_lookup_dict(page_tree, "CropBox");
    crop_box = tmp ? pdf_deref_obj(tmp) : NULL;

    while ((kids_ref = pdf_lookup_dict(page_tree, "Kids")) != NULL) {
      kids = pdf_deref_obj(kids_ref);
      pdf_release_obj(page_tree);
      page_tree = pdf_deref_obj(pdf_get_array(kids, 0));
      pdf_release_obj(kids);

      if ((tmp = pdf_deref_obj(pdf_lookup_dict(page_tree, "MediaBox")))) {
	if (bbox)
	  pdf_release_obj(bbox);
	bbox = tmp;
      }
      if ((tmp = pdf_deref_obj(pdf_lookup_dict(page_tree, "BleedBox")))) {
        if (!rect_equal(tmp, bbox)) {
	  if (bbox)
	    pdf_release_obj(bbox);
	  bbox = tmp;
        } else
          pdf_release_obj(tmp);
      }
      if ((tmp = pdf_deref_obj(pdf_lookup_dict(page_tree, "TrimBox")))) {
        if (!rect_equal(tmp, bbox)) {
	  if (bbox)
	    pdf_release_obj(bbox);
	  bbox = tmp;
        } else
          pdf_release_obj(tmp);
      }
      if ((tmp = pdf_deref_obj(pdf_lookup_dict(page_tree, "ArtBox")))) {
        if (!rect_equal(tmp, bbox)) {
	  if (bbox)
	    pdf_release_obj(bbox);
	  bbox = tmp;
        } else
          pdf_release_obj(tmp);
      }
      if ((tmp = pdf_deref_obj(pdf_lookup_dict(page_tree, "CropBox")))) {
	if (crop_box)
	  pdf_release_obj(crop_box);
	crop_box = tmp;
      }
    }
    if (crop_box) {
      pdf_release_obj(bbox);
      bbox = crop_box;
    }
  }
  pdf_release_obj(page_tree);

  if (!bbox) {
    WARN("No BoundingBox information available.");
    pdf_close(pf);
    return -1;
  }

  *llx = (int)pdf_number_value(pdf_get_array(bbox, 0));
  *lly = (int)pdf_number_value(pdf_get_array(bbox, 1));
  *urx = (int)pdf_number_value(pdf_get_array(bbox, 2));
  *ury = (int)pdf_number_value(pdf_get_array(bbox, 3));

  pdf_release_obj(bbox);

  pdf_close(pf);
  return 0;
}

static void do_pdf (FILE *fp, char *filename)
{
  int llx, lly, urx, ury;

  if (pdf_get_info(fp, filename, &llx, &lly, &urx, &ury) < 0) {
    fprintf (stderr, "%s does not look like a PDF file...\n", filename);
    return;
  }
  write_xbb(filename, llx, lly, urx, ury);
  return;
}

int extractbb (int argc, char *argv[], int mode) 
{
  xbb_output_mode = mode;

  pdf_files_init();

  pdf_set_version(5);

  kpse_set_program_name(argv[0], NULL);

  argc -= 1; argv += 1;
  if (argc == 0)
    usage();

  while (argc > 0 && *argv[0] == '-') {
    switch (*(argv[0]+1)) {
    case 'b':
      xbb_file_mode = FOPEN_WBIN_MODE;
      argc -= 1; argv += 1;
      break;
    case 'm':
      xbb_output_mode = EBB_OUTPUT;
      argc -= 1; argv += 1;
      break;
    case 'x':
      xbb_output_mode = XBB_OUTPUT;
      argc -= 1; argv += 1;
      break;
    case 'v':
      verbose = 1;
      argc -= 1; argv += 1;
      break;
    case 'h':  
      usage();
      argc -= 1; argv += 1;
      break;
    default:
      usage();
    }
  }
  for (; argc > 0; argc--, argv++) {
    FILE *infile = NULL;
    char *kpse_file_name;
    if (!(kpse_file_name = kpse_find_pict(argv[0])) ||
        (infile = MFOPEN(kpse_file_name, FOPEN_RBIN_MODE)) == NULL) {
      fprintf(stderr, "Can't find file (%s)...skipping\n", argv[0]);
      goto cont;
    }
    if (check_for_jpeg(infile)) {
      do_jpeg(infile, kpse_file_name);
      goto cont;
    }
    if (check_for_pdf(infile)) {
      do_pdf(infile, kpse_file_name);
      goto cont;
    }
#ifdef HAVE_LIBPNG
    if (check_for_png(infile)) {
      do_png(infile, kpse_file_name);
      goto cont;
    }
#endif /* HAVE_LIBPNG */
    fprintf(stderr, "Can't handle file type for file named %s\n", argv[0]);
  cont:
    if (kpse_file_name)
      RELEASE(kpse_file_name);
    if (infile)
      MFCLOSE(infile);
  }

  pdf_files_close();

  return 0;
}
