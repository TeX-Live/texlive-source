/*  $Header$

    This is dvipdfm, a DVI to PDF translator.
    Copyright (C) 1998, 1999, 2007 by Mark A. Wicks

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
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include "config.h"
#include "system.h"
#include "mem.h"
#include "mfileio.h"
#include "dvi.h"
#include "pdfdoc.h"
#include "pdfdev.h"
#include "encodings.h"
#include "type1.h"
#include "ttf.h"
#include "colorsp.h"
#include "pdfspecial.h"
#include "pdfparse.h"
#include "vf.h"
#include "pkfont.h"
#include "thumbnail.h"
#include "psimage.h"
#include "tfm.h"

struct rect 
{
  double width;
  double height;
};
typedef struct rect rect;

struct 
{
  char *s;
  struct rect data;
} paper_sizes[] = {
  {"letter" , { 612.0, 792.0}},
  {"legal" , { 612.0, 1008.0}},
  {"ledger" , { 1224.0, 792.0}},
  {"tabloid" , { 792.0, 1224.0}},
  {"a5" , { 420.91, 595.27}},
  {"a4" , { 595.27, 841.82}},
  {"a3" , { 841.82, 1190.16}}};

static rect get_paper_size (char *string)
{
  int i;
  for (i=0; i<sizeof(paper_sizes)/sizeof(paper_sizes[0]); i++) {
    if (!strcmp (string, paper_sizes[i].s))
      break;
  }
  if (i == sizeof(paper_sizes)/sizeof(paper_sizes[0]))
    ERROR ("Paper size is invalid");
  return paper_sizes[i].data;
}


char *dvi_filename = NULL, *pdf_filename = NULL;
static void set_default_pdf_filename(void)
{
  const char *dvi_base;
#ifdef HAVE_XBASENAME
  dvi_base = xbasename (dvi_filename);
#else   
  dvi_base = basename (dvi_filename);
#endif   
  if (strlen (dvi_base) < 5 || strncmp (".dvi", dvi_base+strlen(dvi_base)-4, 4)) 
  {
    pdf_filename = NEW (strlen(dvi_base)+5, char);
    strcpy (pdf_filename, dvi_base);
    strcat (pdf_filename, ".pdf");
  } else
  {
    pdf_filename = NEW (strlen(dvi_base)+1, char);
    strncpy (pdf_filename, dvi_base, strlen(dvi_base)-4);
    strcpy (pdf_filename+strlen(dvi_base)-4, ".pdf");
  }
}
static void usage (void)
{
   fprintf (stdout, "\n%s, version %s, Copyright (C) 1998, 1999, 2007 by Mark A. Wicks\n", PACKAGE, VERSION);
   fprintf (stdout, "dvipdfm comes with ABSOLUTELY NO WARRANTY.\n");
   fprintf (stdout, "This is free software, and you are welcome to redistribute it\n");
   fprintf (stdout, "under certain conditions.  Details are distributed with the software.\n");
   fprintf (stdout, "\nUsage: dvipdfm [options] dvifile\n");
   fprintf (stdout, "-c      \tIgnore color specials (for B&W printing)\n");
   fprintf (stdout, "-f filename\tSet font map file name [t1fonts.map]\n");
   fprintf (stdout, "-o filename\tSet output file name [dvifile.pdf]\n");
   fprintf (stdout, "-l \t\tLandscape mode\n");
   fprintf (stdout, "-m number\tSet additional magnification\n");
   fprintf (stdout, "-p papersize\tSet papersize (letter, legal,\n");
   fprintf (stdout, "            \tledger, tabloid, a4, or a3) [letter]\n");
   fprintf (stdout, "-r resolution\tSet resolution (in DPI) for raster fonts [600]\n");
   fprintf (stdout, "-s pages\tSelect page ranges (-)\n");
   fprintf (stdout, "-t      \tEmbed thumbnail images\n");
   fprintf (stdout, "-d      \tRemove thumbnail images when finished\n");
   fprintf (stdout, "-x dimension\tSet horizontal offset [1.0in]\n");
   fprintf (stdout, "-y dimension\tSet vertical offset [1.0in]\n");
   fprintf (stdout, "-e          \tDisable partial font embedding [default is enabled])\n");
   fprintf (stdout, "-z number\tSet compression level (0-9) [default is 9])\n");
   fprintf (stdout, "-v          \tBe verbose\n");
   fprintf (stdout, "-vv         \tBe more verbose\n");
   fprintf (stdout, "\nAll dimensions entered on the command line are \"true\" TeX dimensions.\n");
   fprintf (stdout, "Argument of \"-s\" lists physical page ranges separated by commas, e.g., \"-s 1-3,5-6\"\n\n");
   exit(1);
}

static double paper_width = 612.0, paper_height = 792.0;
static char landscape_mode = 0;
static char ignore_colors = 0;
static double mag = 1.0, x_offset=72.0, y_offset=72.0;
static int font_dpi = 600;
static int really_quiet = 0;

struct page_range 
{
  unsigned int first, last;
} *page_ranges = NULL;
int max_page_ranges = 0, num_page_ranges = 0;

void set_landscape_mode (void)
{
  dev_set_page_size (paper_height, paper_width);
}

#define pop_arg() {argv += 1; argc -= 1;}
static void do_args (int argc, char *argv[])
{
  char *flag;
  while (argc > 0 && *argv[0] == '-') {
    for (flag=argv[0]+1; *flag != 0; flag++) {
      switch (*flag) {
      case 'D':
	if (argc < 2) {
	  fprintf (stderr, "PS->PDF conversion command line template missing\n\n");
	  usage();
	}
	set_distiller_template(argv[1]);
	pop_arg();
	break;
      case 'r':
	if (argc < 2) {
	  fprintf (stderr, "\nResolution specification missing a number\n\n");
	  usage();
	}
	{
	  char *result, *end, *start = argv[1];
	  end = start + strlen(argv[1]);
	  result = parse_number (&start, end);
	  if (result != NULL && start == end) {
	    font_dpi = (int) atof (result);
	  }
	  else {
	    fprintf (stderr, "\nError in number following resolution specification\n\n");
	    usage();
	  }
	  if (result != NULL) {
	    RELEASE (result);
	  }
	}
	pop_arg();
	break;
      case 'm':
	if (argc < 2) {
	  fprintf (stderr, "\nMagnification specification missing a number\n\n");
	  usage();
	}
	{
	  char *result, *end, *start = argv[1];
	  end = start + strlen(argv[1]);
	  result = parse_number (&start, end);
	  if (result != NULL && start == end) {
	    mag = atof (result);
	  }
	  else {
	    fprintf (stderr, "\nError in number following magnification specification\n\n");
	    usage();
	  }
	  if (result != NULL) {
	    RELEASE (result);
	  }
	}
	pop_arg();
	break;
      case 'g':
	if (argc < 2) {
	  fprintf (stderr, "\nAnnotation \"grow\" specification missing a number\n\n");
	  usage();
	}
	{
	  char *result, *end, *start = argv[1];
	  end = start + strlen(argv[1]);
	  result = parse_number (&start, end);
	  if (result != NULL && start == end) {
	    pdf_special_set_grow (atof (result));
	  }
	  else {
	    fprintf (stderr, "\nError in number following magnification specification\n\n");
	    usage();
	  }
	  if (result != NULL) {
	    RELEASE (result);
	  }
	}
	pop_arg();
	break;
      case 'x':
	if (argc < 2) {
	  fprintf (stderr, "\nX Offset specification missing a number\n\n");
	  usage();
	}
	{
	  char *result, *end, *start = argv[1];
	  double unit;
	  end = start + strlen(argv[1]);
	  result = parse_number (&start, end);
	  if (result != NULL) {
	    x_offset = atof (result);
	  }
	  else {
	    fprintf (stderr, "\nError in number following xoffset specification\n\n");
	    usage();
	  }
	  if (result != NULL) {
	    RELEASE (result);
	  }
	  unit = parse_one_unit(&start, end);
	  if (unit > 0.0) {
	    x_offset *= unit;
	  }
	  else {
	    fprintf (stderr, "\nError in dimension specification following xoffset\n\n");
	    usage();
	  }
	}
	pop_arg();
	break;
      case 'y':
	if (argc < 2) {
	  fprintf (stderr, "\nY offset specification missing a number\n\n");
	  usage();
	}
	{
	  char *result, *end, *start = argv[1];
	  double unit;
	  end = start + strlen(argv[1]);
	  result = parse_number (&start, end);
	  if (result != NULL) {
	    y_offset = atof (result);
	  }
	  else {
	    fprintf (stderr, "\nError in number following yoffset specification\n\n");
	    usage();
	  }
	  if (result != NULL) {
	    RELEASE (result);
	  }
	  unit = parse_one_unit(&start, end);
	  if (unit > 0.0) {
	    y_offset *= unit;
	  }
	  else {
	    fprintf (stderr, "\nError in dimension specification following yoffset\n\n");
	    usage();
	  }
	}
	pop_arg();
	break;
      case 'o':  
	if (argc < 2)
	  ERROR ("Missing output file name");
	pdf_filename = NEW (strlen(argv[1])+1,char);
	strcpy (pdf_filename, argv[1]);
	pop_arg();
	break;
      case 's':
	{
	  char *result, *end, *start = argv[1];
	  if (argc < 2)
	    ERROR ("Missing page selection specification");
	  end = start + strlen (argv[1]);
	  while (start < end) {
	    /* Enlarge page range table if necessary */
	    if (num_page_ranges >= max_page_ranges) {
	      max_page_ranges += 4;
	      page_ranges = RENEW (page_ranges, max_page_ranges,
				   struct page_range);
	    }
	    skip_white (&start, end);
	    page_ranges[num_page_ranges].first = 0;
	    if ((result = parse_unsigned (&start, end))) {
	      page_ranges[num_page_ranges].first = atoi(result)-1;
	      page_ranges[num_page_ranges].last =
		page_ranges[num_page_ranges].first;
	      RELEASE(result);
	    }
	    skip_white (&start, end);
	    if (*start == '-') {
	      start += 1;
	      page_ranges[num_page_ranges].last = UINT_MAX;
	      skip_white (&start, end);
	      if (start < end &&
		  ((result = parse_unsigned (&start, end)))) {
		page_ranges[num_page_ranges].last = atoi(result)-1;
		RELEASE (result);
	      }
	    }
	    num_page_ranges += 1;
	    skip_white (&start, end);
	    if (start < end && *start == ',') {
	      start += 1;
	      continue;
	    }
	    skip_white (&start, end);
	    if (start < end) {
	      fprintf (stderr, "Page selection? %s", start);
	      ERROR ("Bad page range specification");
	    }
	  }
	  pop_arg();
	}
	break;
      case 't':
	{
#ifdef HAVE_LIBPNG
	  pdf_doc_enable_thumbnails ();
#else
	  ERROR ("The thumbnail option requires libpng, which you apparently don't have.");
#endif /* HAVE_LIBPNG */
	}
	break;
      case 'd':
	{
#ifdef HAVE_LIBPNG
	  thumb_remove ();
#else
	  ERROR ("The thumbnail option requires libpng, which you apparently don't have.");
#endif /* HAVE_LIBPNG */
	}
	break;
      case 'p':
	{
	  struct rect paper_size;
	  if (argc < 2)
	    ERROR ("Missing paper size");
	  paper_size = get_paper_size (argv[1]);
	  paper_width = paper_size.width;
	  paper_height = paper_size.height;
	  pop_arg();
	}
	break;
      case 'c':
	ignore_colors = 1;
	break;
      case 'l':
	landscape_mode = 1;
	break;
      case 'f':
	dev_read_mapfile (argv[1]);
	pop_arg();
	break;
      case 'e':
	type1_disable_partial();
	break;
      case 'q':
	really_quiet = 1;
	break;
      case 'v':
	encoding_set_verbose();
	dev_set_verbose();
	dvi_set_verbose();
	type1_set_verbose();
	vf_set_verbose();
	pk_set_verbose();
	pdf_obj_set_verbose();
	pdf_doc_set_verbose();
	tfm_set_verbose();
#ifdef HAVE_TTF_FORMATS	 
	ttf_set_verbose();
#endif	 
	break;
      case 'V':
	{
	  unsigned level = 2;
	  if (isdigit (*(flag+1))) {
	    level = *(++flag) - '0';
	  } else {
	    char *result, *end, *start = argv[1];
	    if (argc < 2) {
	      fprintf (stderr, "\nVersion specification missing number (2 or 3)\n\n");
	      usage();
	    }
	    end = start + strlen(argv[1]);
	    result = parse_number (&start, end);
	    if (result != NULL && start == end) {
	      level = (int) atof (result);
	    }
	    else {
	      fprintf (stderr, "\nError in number following version specification\n\n");
	      usage();
	    }
	    if (result != NULL) {
	      RELEASE (result);
	    }
	    pop_arg();
	  }
	  if (level >= 2 && level <= 3) {
	    pdf_set_version(level);
	  } else {
	    fprintf (stderr, "\nNumber following version specification is out of range\n\n");
	  }
	}
	break;
      case 'z': 
	{
	  int level = 9;
#ifndef HAVE_ZLIB
	  fprintf (stderr, "\nYou don't have compression compiled in.  Possibly libz wasn't found by configure.\nCompression specification will be ignored.\n\n");
#endif  /* HAVE_ZLIB */
	  if (isdigit (*(flag+1))) {
	    level = *(++flag) - '0';
	  } else {
	    char *result, *end, *start = argv[1];
	    if (argc < 2) {
	      fprintf (stderr, "\nCompression specification missing number for level\n\n");
	      usage();
	    }
	    end = start + strlen(argv[1]);
	    result = parse_number (&start, end);
	    if (result != NULL && start == end) {
	      level = (int) atof (result);
	    }
	    else {
	      fprintf (stderr, "\nError in number following compression specification\n\n");
	      usage();
	    }
	    if (result != NULL) {
	      RELEASE (result);
	    }
	    pop_arg();
	  }
	  if (level >= 0 && level <= 9) {
	    pdf_obj_set_compression(level);
	  } else {
	    fprintf (stderr, "\nNumber following compression specification is out of range\n\n");
	  }
	}
	break;
      default:
	usage();
      }
    }
    argc -= 1 ;
    argv += 1;
  }
  if (argc > 1) {
    fprintf (stderr, "\nMultiple dvi filenames?\n\n");
    usage();
  }
  /* The only legitimate way to have argc == 0 here is
     is do_args was called from config file.  In that case, there is
     no dvi file name.  Check for that case  */
  if (argc > 0) {
    if (strncmp (".dvi", argv[0]+strlen(argv[0])-4, 4)) {
      dvi_filename = NEW (strlen (argv[0])+1+4, char);
      strcpy (dvi_filename, argv[0]);
      strcat (dvi_filename, ".dvi");
    }
    else {
      dvi_filename = NEW (strlen (argv[0])+1, char);
      strcpy (dvi_filename, argv[0]);
    }
  }
}


static void cleanup(void)
{
  RELEASE (dvi_filename);
  RELEASE (pdf_filename);
  if (page_ranges)
    RELEASE (page_ranges);
  psimage_close();
}

static char *config_file_name = "config";
static void read_config_file (void)
{
  char *full_config_name, *start, *end;
  char *argv[2], *option;
  FILE *config_file;
  
  if ((full_config_name = kpse_find_file (config_file_name,
				   kpse_program_text_format,
				   true)) == NULL) {
    return;
  }
  if (!(config_file = MFOPEN (full_config_name, FOPEN_R_MODE))) {
    fprintf (stderr, "\nError opening configuration file.  Continuing with defaults.\n");
    return;
  }
  while ((start = mfgets (work_buffer, WORK_BUFFER_SIZE,
			  config_file))) {
    int argc = 0;
    end = work_buffer+strlen(work_buffer);
    skip_white (&start, end);
    if (start >= end)
      continue;
    /* Build up an argument list as if it were passed on the command
       line */
    if ((option = parse_ident (&start, end))) {
      argc = 1;
      argv[0] = NEW (strlen(option)+2, char);
      strcpy (argv[0]+1, option);
      RELEASE (option);
      *argv[0] = '-';
      skip_white (&start, end);
      if (start < end) {
	argc += 1;
	if (*start == '"') {
	  argv[1] = parse_c_string (&start, end);
	}
	else
	  argv[1] = parse_ident (&start, end);
      }
    }
    do_args (argc, argv);
    while (argc > 0) {
      RELEASE (argv[--argc]);
    }
  }
}

void error_cleanup (void)
{
  pdf_error_cleanup();
  remove (pdf_filename);
  fprintf (stderr, "\nOutput file removed.\n");
}

int CDECL main (int argc, char *argv[]) 
{
  int i;
  int at_least_one_page = 0;
  if (argc < 2) {
    fprintf (stderr, "\nNo dvi filename specified.\n\n");
    usage();
    return 1;
  }
#ifdef HAVE_KPATHSEA
  kpse_set_program_name (argv[0], NULL);
#endif

  argv+=1;
  argc-=1;

  /* Process config file, if any */
  read_config_file();

  do_args (argc, argv);

#ifdef HAVE_KPATHSEA
  kpse_init_prog ("", font_dpi, NULL, NULL);
  pk_set_dpi (font_dpi);
  kpse_set_program_enabled (kpse_pk_format, true, kpse_src_texmf_cnf);
#endif

  if (!dvi_filename) {
    fprintf (stderr, "\nNo dvi filename specified.\n\n");
    usage();
  }

  /* Check for ".dvi" at end of argument name */
  if (pdf_filename == NULL)
    set_default_pdf_filename();
  
  if (!really_quiet)
    fprintf (stdout, "\n%s -> %s\n", dvi_filename, pdf_filename);

  dvi_init (dvi_filename, pdf_filename, mag, x_offset, y_offset);

  if (ignore_colors) {
    color_special_ignore_colors();
    pdf_special_ignore_colors();
  }
  
  if (landscape_mode)
    dev_set_page_size (paper_height, paper_width);
  else
    dev_set_page_size (paper_width, paper_height);

  if ((num_page_ranges))
    for (i=0; i<num_page_ranges; i++) {
      unsigned j;
      if (page_ranges[i].first <= page_ranges[i].last)
	for (j=page_ranges[i].first; j<=page_ranges[i].last && j<dvi_npages(); j++) {
 	  fprintf (stderr, "[%d", j+1);
   	  dvi_do_page (j);
	  at_least_one_page = 1;
	  fprintf (stderr, "]");
	}
      else
	for (j=page_ranges[i].first;
	     j>=page_ranges[i].last && j<dvi_npages(); j--) {
	  fprintf (stderr, "[%d", j+1);
	  dvi_do_page (j);
	  at_least_one_page = 1;
	  fprintf (stderr, "]");
	}
    }
  if (!at_least_one_page && num_page_ranges) {
    fprintf (stderr, "No pages fall in range!\nFalling back to entire document.\n");
  }
  if (!at_least_one_page) /* Fall back to entire document */
    for (i=0; i<dvi_npages(); i++) {
      fprintf (stderr, "[%d", i+1);
      dvi_do_page (i);
      fprintf (stderr, "]");
    }
  dvi_close();
  fprintf (stderr, "\n");
  cleanup();
  return 0;
}

