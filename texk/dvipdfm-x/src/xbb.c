/*  

    This is extractbb, a bounding box extraction program.

    Copyright (C) 2008-2012 by Jin-Hwan Cho and Matthias Franz

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

#include "numbers.h"
#include "system.h"
#include "mem.h"
#include "error.h"
#include "mfileio.h"
#include "pdfobj.h"
#include "pdfdev.h"
#include "pdfdoc.h"
#include "pdfparse.h"

#include "config.h"

#include "jpegimage.h"
#include "pngimage.h"

#include "dvipdfmx.h"

#define XBB_PROGRAM "extractbb"
#define XBB_VERSION VERSION

static int  really_quiet = 0;

static void show_version(void)
{
  if (really_quiet) return;

  fprintf (stdout, "%s, version %s, Copyright (C) 2009 by Jin-Hwan Cho and Matthias Franz\n",
	   XBB_PROGRAM, XBB_VERSION);
  fprintf (stdout, "A bounding box extraction utility from PDF, PNG, and JPEG.\n");
  fprintf (stdout, "\nThis is free software; you can redistribute it and/or modify\n");
  fprintf (stdout, "it under the terms of the GNU General Public License as published by\n");
  fprintf (stdout, "the Free Software Foundation; either version 2 of the License, or\n");
  fprintf (stdout, "(at your option) any later version.\n");
}

static void show_usage(void)
{
  if (really_quiet) return;

  fprintf (stdout, "\nUsage: %s [-v|-q] [-O] [-m|-x] [files]\n", XBB_PROGRAM);
  fprintf (stdout, "\t-v\tBe verbose\n");
  fprintf (stdout, "\t-q\tBe quiet\n");
  fprintf (stdout, "\t-O\tWrite output to stdout\n");
  if(compat_mode) {
    fprintf (stdout, "\t-m\tOutput .bb  file used in DVIPDFM (default)\n");
    fprintf (stdout, "\t-x\tOutput .xbb file used in DVIPDFMx\n");
  } else {
    fprintf (stdout, "\t-m\tOutput .bb  file used in DVIPDFM\n");
    fprintf (stdout, "\t-x\tOutput .xbb file used in DVIPDFMx (default)\n");
  }
}

static void usage(void)
{
  if (really_quiet) return;

  fprintf(stdout, "\nTry \"%s --help\" for more information.\n", XBB_PROGRAM);
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

static int xbb_to_file = 1;

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
    WARN("%s: Filename does not end in a recognizable extension.\n", name);
    result = NEW(strlen(name)+5, char);  /* 5 = ".xbb" + trailing 0 */
    strcpy(result, name);
  } else { /* Remove extension */
    result = NEW(strlen(name)-strlen(extensions[i])+5, char);  /* 5 = ".xbb" + trailing 0 */
    strncpy(result, name, strlen(name)-strlen(extensions[i]));
    result[strlen(name)-strlen(extensions[i])] = 0;
  }
  strcat(result, (compat_mode ? ".bb" : ".xbb"));
  return result;
}

static void write_xbb(char *fname,
		      double bbllx_f, double bblly_f,
		      double bburx_f, double bbury_f,
		      int pdf_version, long pagecount) 
{
  char *outname = NULL;
  FILE *fp;

  long bbllx = ROUND(bbllx_f, 1.0), bblly = ROUND(bblly_f, 1.0);
  long bburx = ROUND(bburx_f, 1.0), bbury = ROUND(bbury_f, 1.0);

  if (xbb_to_file) {
    outname = make_xbb_filename(fname);
    if ((fp = MFOPEN(outname, FOPEN_W_MODE)) == NULL) {
      ERROR("Unable to open output file: %s\n", outname);
    }
  } else {
    fp = stdout;
#ifdef WIN32
    setmode(fileno(fp), _O_BINARY);
#endif
  }

  if (verbose) {
    MESG("Writing to %s: ", xbb_to_file ? outname : "stdout");
    MESG("Bounding box: %d %d %d %d\n", bbllx, bblly, bburx, bbury);
  }

  fprintf(fp, "%%%%Title: %s\n", fname);
  fprintf(fp, "%%%%Creator: %s %s\n", XBB_PROGRAM, XBB_VERSION);
  fprintf(fp, "%%%%BoundingBox: %ld %ld %ld %ld\n", bbllx, bblly, bburx, bbury);

  if (!compat_mode) {
    /* Note:
     * According to Adobe Technical Note #5644, the arguments to
     * "%%HiResBoundingBox:" must be of type real. And according
     * to the PostScript Language Reference, a real number must
     * be written with a decimal point (or an exponent). Hence
     * it seems illegal to replace "0.0" by "0".
     */
    fprintf(fp, "%%%%HiResBoundingBox: %f %f %f %f\n",
	    bbllx_f, bblly_f, bburx_f, bbury_f);
    if (pdf_version >= 0) {
      fprintf(fp, "%%%%PDFVersion: 1.%d\n", pdf_version);
      fprintf(fp, "%%%%Pages: %ld\n", pagecount);
    }
  }

  do_time(fp);

  if (xbb_to_file) {
    RELEASE(outname);
    MFCLOSE(fp);
  }
}

static void do_jpeg (FILE *fp, char *filename)
{
  long   width, height;
  double xdensity, ydensity;

  if (jpeg_get_bbox(fp, &width, &height, &xdensity, &ydensity) < 0) {
    WARN("%s does not look like a JPEG file...\n", filename);
    return;
  }

  write_xbb(filename, 0, 0, xdensity*width, ydensity*height, -1, -1);
  return;
}

#ifdef HAVE_LIBPNG
static void do_png (FILE *fp, char *filename)
{
  long   width, height;
  double xdensity, ydensity;

  if (png_get_bbox(fp, &width, &height, &xdensity, &ydensity) < 0) {
    WARN("%s does not look like a PNG file...\n", filename);
    return;
  }

  write_xbb(filename, 0, 0, xdensity*width, ydensity*height, -1, -1);
  return;
}
#endif /* HAVE_LIBPNG */

#ifdef XETEX
static int rect_equal (pdf_obj *rect1, pdf_obj *rect2)
{
  int i;
  if (!rect1 || !rect2) return 0;
  for (i = 0; i < 4; i++) {
    if (pdf_number_value(pdf_get_array(rect1, i)) != pdf_number_value(pdf_get_array(rect2, i))) return 0;
  }
  return 1;
}

static int pdf_get_info (FILE *image_file, char *filename, int *version,
                         double *llx, double *lly, double *urx, double *ury)
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

  *version = pdf_file_get_version(pf);

  *llx = pdf_number_value(pdf_get_array(bbox, 0));
  *lly = pdf_number_value(pdf_get_array(bbox, 1));
  *urx = pdf_number_value(pdf_get_array(bbox, 2));
  *ury = pdf_number_value(pdf_get_array(bbox, 3));

  pdf_release_obj(bbox);

  pdf_close(pf);
  return 0;
}

static void do_pdf (FILE *fp, char *filename)
{
  double llx, lly, urx, ury;
  int version;

  if (pdf_get_info(fp, filename, &version, &llx, &lly, &urx, &ury) < 0) {
    fprintf (stderr, "%s does not look like a PDF file...\n", filename);
    return;
  }
  write_xbb(filename, llx, lly, urx, ury, version, 1);
  return;
}
#else
static void do_pdf (FILE *fp, char *filename)
{
  pdf_obj *page;
  pdf_file *pf;
  long page_no = 1;
  long count;
  pdf_rect bbox;

  pf = pdf_open(filename, fp);
  if (!pf) {
    WARN("%s does not look like a PDF file...\n", filename);
    return;
  }

  page = pdf_doc_get_page(pf, page_no, &count, &bbox, NULL);

  pdf_close(pf);

  if (!page)
    return;

  pdf_release_obj(page);
  write_xbb(filename, bbox.llx, bbox.lly, bbox.urx, bbox.ury,
	    pdf_file_get_version(pf), count);
}
#endif

int extractbb (int argc, char *argv[]) 
{
  pdf_files_init();

  pdf_set_version(5);

  argc -= 1; argv += 1;
  if (argc == 0)
    usage();

  while (argc > 0 && *argv[0] == '-') {
    char *flag = argv[0] + 1;
    switch (*flag) {
    case '-':
      if (++flag) {
        if (!strcmp(flag, "help")) {
          show_version();
          show_usage();
          exit(0);
        } else if (!strcmp(flag, "version")) {
          show_version();
          exit(0);
        }
      }
      if (!really_quiet)
        fprintf(stderr, "Unknown option in \"--%s\"", flag);
      usage();
      break;
    case 'O':
      xbb_to_file = 0;
      argc -= 1; argv += 1;
      break;
    case 'm':
      compat_mode = 1;
      argc -= 1; argv += 1;
      break;
    case 'x':
      compat_mode = 0;
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
    case 'q':  
      really_quiet = 1;
      shut_up(1);
      argc -= 1; argv += 1;
      break;
    case 'b':
      if (compat_mode) {
	/* ignore obsolete "binary mode" option */
	argc -= 1; argv += 1;
	break;
      }
      /* else fall through */
    default:
      if (!really_quiet)
        fprintf(stderr, "Unknown option in \"-%s\"", flag);
      usage();
    }
  }

  for (; argc > 0; argc--, argv++) {
    FILE *infile = NULL;
    char *kpse_file_name;
    if (!(kpse_file_name = kpse_find_pict(argv[0])) ||
        (infile = MFOPEN(kpse_file_name, FOPEN_RBIN_MODE)) == NULL) {
      WARN("Can't find file (%s)...skipping\n", argv[0]);
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
    WARN("Can't handle file type for file named %s\n", argv[0]);
  cont:
    if (kpse_file_name)
      RELEASE(kpse_file_name);
    if (infile)
      MFCLOSE(infile);
  }

  pdf_files_close();

  return 0;
}
