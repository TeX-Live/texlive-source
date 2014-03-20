/* This is (x)dvipdfmx, an extended version of...

    DVIPDFMx, an eXtended version of DVIPDFM by Mark A. Wicks.

    Copyright (C) 2002-2014 by Jin-Hwan Cho, Matthias Franz, and Shunsaku Hirata,
    the DVIPDFMx project team.
    
    Copyright (c) 2006 SIL. (xdvipdfmx extensions for XeTeX support)

    Copyright (C) 1998, 1999 by Mark A. Wicks <mwicks@kettering.edu>

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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

#include "system.h"
#include "mem.h"

#include "dpxconf.h"
#include "dpxfile.h"
#include "dpxutil.h"

#include "dvi.h"

#include "pdflimits.h"
#include "pdfdoc.h"
#include "pdfdev.h"
#include "pdfparse.h"
#include "pdfencrypt.h"

#include "spc_tpic.h"
#include "specials.h"

#include "mpost.h"

#include "fontmap.h"
#include "pdffont.h"
#include "pdfximage.h"
#include "cid.h"

#include "dvipdfmx.h"
#include "tt_aux.h"

#include "error.h"

#ifdef XETEX
int is_xetex = 1;
const char *my_name = "xdvipdfmx";
#else
int is_xetex = 0;
const char *my_name = "dvipdfmx";
#endif

int compat_mode = 0;     /* 0 = dvipdfmx, 1 = dvipdfm */

static int verbose = 0;

static int mp_mode = 0;

static long opt_flags = 0;

#define OPT_TPIC_TRANSPARENT_FILL (1 << 1)
#define OPT_CIDFONT_FIXEDPITCH    (1 << 2)
#define OPT_FONTMAP_FIRST_MATCH   (1 << 3)
#define OPT_PDFDOC_NO_DEST_REMOVE (1 << 4)

static char   ignore_colors = 0;
static double annot_grow    = 0.0;
static int    bookmark_open = 0;
static double mag           = 1.0;
static int    font_dpi      = 600;
static int    really_quiet  = 0;
/*
 * Precision is essentially limited to 0.01pt.
 * See, dev_set_string() in pdfdev.c.
 */
static int pdfdecimaldigits = 2;

/* Image cache life in hours */
/*  0 means erase all old images and leave new images */
/* -1 means erase all old images and also erase new images */
/* -2 means ignore image cache (default) */
static int image_cache_life = -2;

/* Encryption */
static int do_encryption    = 0;
static unsigned key_bits    = 40;
static unsigned permission  = 0x003C;

/* Page device */
double paper_width  = 595.0;
double paper_height = 842.0;
static double x_offset = 72.0;
static double y_offset = 72.0;
char   landscape_mode    = 0;

int always_embed = 0; /* always embed fonts, regardless of licensing flags */

char *dvi_filename = NULL, *pdf_filename = NULL;

static void
read_config_file (const char *config);

static void
set_default_pdf_filename(void)
{
  const char *dvi_base;

  dvi_base = xbasename(dvi_filename);
  if (mp_mode &&
      strlen(dvi_base) > 4 &&
      FILESTRCASEEQ(".mps", dvi_base + strlen(dvi_base) - 4)) {
    pdf_filename = NEW(strlen(dvi_base)+1, char);
    strncpy(pdf_filename, dvi_base, strlen(dvi_base) - 4);
    pdf_filename[strlen(dvi_base)-4] = '\0';
  } else if (strlen(dvi_base) > 4 &&
             (FILESTRCASEEQ(".dvi", dvi_base+strlen(dvi_base)-4) ||
              (is_xetex && FILESTRCASEEQ(".xdv", dvi_base+strlen(dvi_base)-4)))) {
    pdf_filename = NEW(strlen(dvi_base)+1, char);
    strncpy(pdf_filename, dvi_base, strlen(dvi_base)-4);
    pdf_filename[strlen(dvi_base)-4] = '\0';
  } else {
    pdf_filename = NEW(strlen(dvi_base)+5, char);
    strcpy(pdf_filename, dvi_base);
  }

  strcat (pdf_filename, ".pdf");
}

static void
show_version (void)
{
  fprintf (stdout, "This is %s Version " VERSION " by %s,\n",
                   my_name,
                   is_xetex ? "Jonathan Kew and Jin-Hwan Cho"
                            : "the DVIPDFMx project team");
  fprintf (stdout, "modified for TeX Live,\n");
  if (is_xetex)
    fprintf (stdout, "an extended version of DVIPDFMx, which in turn was\n");
  fprintf (stdout, "an extended version of dvipdfm-0.13.2c developed by Mark A. Wicks.\n");
  fprintf (stdout, "\nCopyright (C) 2002-2014 the DVIPDFMx project team\n");
  if (is_xetex)
    fprintf (stdout, "Copyright (C) 2006-2014 SIL International and Jin-Hwan Cho.\n");
  fprintf (stdout, "\nThis is free software; you can redistribute it and/or modify\n");
  fprintf (stdout, "it under the terms of the GNU General Public License as published by\n");
  fprintf (stdout, "the Free Software Foundation; either version 2 of the License, or\n");
  fprintf (stdout, "(at your option) any later version.\n");
}

static void
show_usage (void)
{
  fprintf (stdout, "\nUsage: %s [options] [dvifile[.dvi%s]]\n", my_name, is_xetex ? "|.xdv" : "");
  fprintf (stdout, "       %s --extractbb|--xbb|--ebb [options]\tBe \"extractbb\"\n", my_name);
  fprintf (stdout, "       %s --help|--version\n", my_name);
  fprintf (stdout, "\nOptions:\n"); 
  fprintf (stdout, "  -c \t\tIgnore color specials (for B&W printing)\n");
  if (!is_xetex)
    fprintf (stdout, "  --dvipdfm\tEnable DVIPDFM emulation mode\n");
  fprintf (stdout, "  -d number\tSet PDF decimal digits (0-5) [2]\n");
  fprintf (stdout, "  -f filename\tSet font map file name [cid-x.map]\n");
  fprintf (stdout, "  -g dimension\tAnnotation \"grow\" amount [0.0in]\n");
  fprintf (stdout, "  -h | --help \tShow this help message and exit\n");
  fprintf (stdout, "  -l \t\tLandscape mode\n");
  fprintf (stdout, "  -m number\tSet additional magnification\n");
  fprintf (stdout, "  -o filename\tSet output file name, \"-\" for stdout [dvifile.pdf]\n");
  fprintf (stdout, "  -p papersize\tSet papersize [a4]\n");
  fprintf (stdout, "  -q \t\tBe quiet\n");
  fprintf (stdout, "  -r resolution\tSet resolution (in DPI) for raster fonts [600]\n");
  fprintf (stdout, "  -s pages\tSelect page ranges (-)\n");
  fprintf (stdout, "  -t \t\tEmbed thumbnail images of PNG format [dvifile.1] \n");
  fprintf (stdout, "  --version\tOutput version information and exit\n");
  fprintf (stdout, "  -v \t\tBe verbose\n");
  fprintf (stdout, "  -vv\t\tBe more verbose\n");
  fprintf (stdout, "  --kpathsea-debug number\tSet kpathsearch debugging flags [0]\n");
  fprintf (stdout, "  -x dimension\tSet horizontal offset [1.0in]\n");
  fprintf (stdout, "  -y dimension\tSet vertical offset [1.0in]\n");
  fprintf (stdout, "  -z number  \tSet zlib compression level (0-9) [9]\n");

  fprintf (stdout, "  -C number\tSpecify miscellaneous option flags [0]:\n");
  fprintf (stdout, "\t\t  0x0001 reserved\n");
  fprintf (stdout, "\t\t  0x0002 Use semi-transparent filling for tpic shading command,\n");
  fprintf (stdout, "\t\t\t instead of opaque gray color. (requires PDF 1.4)\n");
  fprintf (stdout, "\t\t  0x0004 Treat all CIDFont as fixed-pitch font.\n");
  fprintf (stdout, "\t\t  0x0008 Do not replace duplicate fontmap entries.\n");
  fprintf (stdout, "\t\t  0x0010 Do not optimize PDF destinations.\n");
  fprintf (stdout, "\t\tPositive values are always ORed with previously given flags.\n");
  fprintf (stdout, "\t\tAnd negative values replace old values.\n");
  fprintf (stdout, "  -D template\tPS->PDF conversion command line template [none]\n");
  fprintf (stdout, "  -E \t\tAlways try to embed fonts, regardless of licensing flags.\n");
  fprintf (stdout, "  -I number\tImage cache life in hours [-2]\n");
  fprintf (stdout, "           \t 0: erase all old images and leave new images\n");
  fprintf (stdout, "           \t-1: erase all old images and also erase new images\n");
  fprintf (stdout, "           \t-2: ignore image cache\n");
  fprintf (stdout, "  -K number\tEncryption key length [40]\n");
  fprintf (stdout, "  -M \t\tProcess MetaPost PostScript output\n");
  fprintf (stdout, "  -O number\tSet maximum depth of open bookmark items [0]\n");
  fprintf (stdout, "  -P number\tSet permission flags for PDF encryption [0x003C]\n");
  fprintf (stdout, "  -S \t\tEnable PDF encryption\n");
  fprintf (stdout, "  -V number\tSet PDF minor version [4]\n");
  fprintf (stdout, "\nAll dimensions entered on the command line are \"true\" TeX dimensions.\n");
  fprintf (stdout, "Argument of \"-s\" lists physical page ranges separated by commas, e.g., \"-s 1-3,5-6\"\n");
  fprintf (stdout, "Papersize is specified by paper format (e.g., \"a4\") or by w<unit>,h<unit> (e.g., \"20cm,30cm\").\n");
}

static void
usage (void)
{
  fprintf (stderr, "\nTry \"%s --help\" for more information.\n", my_name);
  exit(1);
}


static int
read_length (double *vp, const char **pp, const char *endptr)
{
  char   *q;
  const char *p = *pp;
  double  v, u = 1.0;
  const char *_ukeys[] = {
#define K_UNIT__PT  0
#define K_UNIT__IN  1
#define K_UNIT__CM  2
#define K_UNIT__MM  3
#define K_UNIT__BP  4
    "pt", "in", "cm", "mm", "bp",
     NULL
  };
  int     k, error = 0;

  q = parse_float_decimal(&p, endptr);
  if (!q) {
    *vp = 0.0; *pp = p;
    return  -1;
  }

  v = atof(q);
  RELEASE(q);

  skip_white(&p, endptr);
  q = parse_c_ident(&p, endptr);
  if (q) {
    char *qq = q;
    if (strlen(q) >= strlen("true") &&
        !memcmp(q, "true", strlen("true"))) {
      q += strlen("true"); /* just skip "true" */
    }
    if (strlen(q) == 0) {
      RELEASE(qq);
      skip_white(&p, endptr);
      qq = q = parse_c_ident(&p, endptr);
    }
    if (q) {
      for (k = 0; _ukeys[k] && strcmp(_ukeys[k], q); k++);
      switch (k) {
      case K_UNIT__PT: u *= 72.0 / 72.27; break;
      case K_UNIT__IN: u *= 72.0; break;
      case K_UNIT__CM: u *= 72.0 / 2.54 ; break;
      case K_UNIT__MM: u *= 72.0 / 25.4 ; break;
      case K_UNIT__BP: u *= 1.0 ; break;
      default:
        WARN("Unknown unit of measure: %s", q);
        error = -1;
        break;
      }
      RELEASE(qq);
    }
    else {
      WARN("Missing unit of measure after \"true\"");
      error = -1;
    }
  }

  *vp = v * u; *pp = p;
  return  error;
}

static void
select_paper (const char *paperspec)
{
  const struct paper *pi;
  int   error = 0;

  pi = paperinfo(paperspec);
  if (pi && papername(pi)) {
    paper_width  = paperpswidth (pi);
    paper_height = paperpsheight(pi);
  } else {
    const char  *p = paperspec, *endptr, *comma;
    comma  = strchr(p, ',');
    endptr = p + strlen(p);
    if (!comma)
      ERROR("Unrecognized paper format: %s", paperspec);
    error = read_length(&paper_width,  &p, comma);
    p = comma + 1;
    error = read_length(&paper_height, &p, endptr);
  }
  if (error || paper_width <= 0.0 || paper_height <= 0.0)
    ERROR("Invalid paper size: %s (%.2fx%.2f)", paperspec, paper_width, paper_height);
}

struct page_range 
{
  long first, last;
} *page_ranges = NULL;

int num_page_ranges = 0;
int max_page_ranges = 0;

static void
select_pages (const char *pagespec)
{
  char  *q;
  const char *p = pagespec;

  while (*p != '\0') {
    /* Enlarge page range table if necessary */
    if (num_page_ranges >= max_page_ranges) {
      max_page_ranges += 4;
      page_ranges = RENEW(page_ranges, max_page_ranges, struct page_range);
    }

    page_ranges[num_page_ranges].first = 0;
    page_ranges[num_page_ranges].last  = 0;

    for ( ; *p && isspace(*p); p++);
    q = parse_unsigned(&p, p + strlen(p)); /* Can't be signed. */
    if (q) { /* '-' is allowed here */
      page_ranges[num_page_ranges].first = atoi(q) - 1;
      page_ranges[num_page_ranges].last  = page_ranges[num_page_ranges].first;
      RELEASE(q);
    }
    for ( ; *p && isspace(*p); p++);

    if (*p == '-') {
      for (++p; *p && isspace(*p); p++);
      page_ranges[num_page_ranges].last = -1;
      if (*p) {
        q = parse_unsigned(&p, p + strlen(p));
        if (q) {
          page_ranges[num_page_ranges].last = atoi(q) - 1;
          RELEASE(q);
        }
        for ( ; *p && isspace(*p); p++);
      }
    } else {
      page_ranges[num_page_ranges].last = page_ranges[num_page_ranges].first;
    }

    num_page_ranges++;

    if (*p == ',')
      p++;
    else  {
      for ( ; *p && isspace(*p); p++);
      if (*p)
        ERROR("Bad page range specification: %s", p);
    }
  }
  return;
}

#define POP_ARG() {argv += 1; argc -= 1;}
/* It doesn't work as expected (due to dvi filename). */
#define CHECK_ARG(n,m) if (argc < (n) + 1) {\
  fprintf (stderr, "\nMissing %s after \"-%c\".\n", (m), *flag);\
  usage();\
}

static void
set_verbose (int argc, char *argv[])
{
  while (argc > 0) {
    if(*argv[0] == '-') {
      char *flag;

      for (flag = argv[0] + 1; *flag != 0; flag++) {
        if (*flag == '-')
          break;
        if (*flag == 'q')
          really_quiet = 1;
        if (*flag == 'v')
          verbose++;
      }
    }
    POP_ARG();
  }

  if (!really_quiet) {
    int i;

    for (i = 0; i < verbose; i++) {
      dvi_set_verbose();
      pdf_dev_set_verbose();
      pdf_doc_set_verbose();
      pdf_enc_set_verbose();
      pdf_obj_set_verbose();
      pdf_fontmap_set_verbose();
      dpx_file_set_verbose();
      tt_aux_set_verbose();
    }
  }
}


static void
do_args (int argc, char *argv[])
{
  while (argc > 0 && *argv[0] == '-') {
    char *flag, *nextptr;
    const char *nnextptr;

    for (flag = argv[0] + 1; *flag != 0; flag++) {
      switch (*flag) {
      case '-':
        if (flag == argv[0] + 1 && ++flag) {
          if (!strcmp(flag, "help")) {
            show_usage();
            exit(0);
          } else if (!strcmp(flag, "version")) {
            show_version();
            exit(0);
          } else if (!is_xetex && !strcmp(flag, "dvipdfm")) {
            compat_mode = 1;
            goto Out_of_For_Loop;
          } else if (!strcmp(flag, "kpathsea-debug")) {
            CHECK_ARG(1, "kpathsearch debugging flags");
            kpathsea_debug = atoi(argv[1]);
            if (kpathsea_debug < 0)
              ERROR("Invalid kpathsearch debugging flags specified: %s", argv[1]);
            POP_ARG();
            goto Out_of_For_Loop;
          }
        }
        fprintf(stderr, "Unknown option \"%s\"", argv[0]);
        usage();
      case 'D':
        CHECK_ARG(1, "PS->PDF conversion command line template");
        set_distiller_template(argv[1]);
        POP_ARG();
        break;
      case 'r':
        CHECK_ARG(1, "bitmap font dpi");
        font_dpi = atoi(argv[1]);
        if (font_dpi <= 0)
          ERROR("Invalid bitmap font dpi specified: %s", argv[1]);
        POP_ARG();
        break;
      case 'm':
        CHECK_ARG(1, "magnification value");
        mag = strtod(argv[1], &nextptr);
        if (mag < 0.0 || nextptr == argv[1])
          ERROR("Invalid magnification specifiied: %s", argv[1]);
        POP_ARG();
        break;
      case 'g':
        CHECK_ARG(1, "annotation \"grow\" amount");
        nnextptr = nextptr = argv[1];
        read_length(&annot_grow, &nnextptr, nextptr + strlen(nextptr));
        POP_ARG();
        break;
      case 'x':
        CHECK_ARG(1, "horizontal offset value");
        nnextptr = nextptr = argv[1];
        read_length(&x_offset, &nnextptr, nextptr + strlen(nextptr));
        POP_ARG();
        break;
      case 'y':
        CHECK_ARG(1, "vertical offset value");
        nnextptr = nextptr = argv[1];
        read_length(&y_offset, &nnextptr, nextptr + strlen(nextptr));
        POP_ARG();
        break;
      case 'o':
        CHECK_ARG(1, "output file name");
        pdf_filename = NEW (strlen(argv[1])+1,char);
        strcpy(pdf_filename, argv[1]);
        POP_ARG();
        break;
      case 's':
        CHECK_ARG(1, "page selection specification");
        select_pages(argv[1]);
        POP_ARG();
        break;
      case 't':
        pdf_doc_enable_manual_thumbnails();
        break;
      case 'p':
        CHECK_ARG(1, "paper format/size");
        select_paper(argv[1]);
        POP_ARG();
        break;
      case 'c':
        ignore_colors = 1;
        break;
      case 'l':
        landscape_mode = 1;
        break;
      case 'f':
        CHECK_ARG(1, "fontmap file name");
        if (opt_flags & OPT_FONTMAP_FIRST_MATCH)
          pdf_load_fontmap_file(argv[1], FONTMAP_RMODE_APPEND);
        else
          pdf_load_fontmap_file(argv[1], FONTMAP_RMODE_REPLACE);
        POP_ARG();
        break;
      case 'i':
        CHECK_ARG(1, "subsidiary config file");
        read_config_file(argv[1]);
        POP_ARG();
        break;
      case 'q': case 'v':
        break;
      case 'V':
      {
        int ver_minor;

        if (isdigit(*(flag+1))) {
          flag++;
          ver_minor = atoi(flag);
        } else {
          CHECK_ARG(1, "PDF minor version number");
          ver_minor = atoi(argv[1]);
          POP_ARG();
        }
        if (ver_minor < PDF_VERSION_MIN) {
          WARN("PDF version 1.%d not supported. Using PDF 1.%d instead.",
               ver_minor, PDF_VERSION_MIN);
          ver_minor = PDF_VERSION_MIN;
        } else if (ver_minor > PDF_VERSION_MAX) {
          WARN("PDF version 1.%d not supported. Using PDF 1.%d instead.",
               ver_minor, PDF_VERSION_MAX);
          ver_minor = PDF_VERSION_MAX;
        }
        pdf_set_version((unsigned) ver_minor);
      }
      break;
      case 'z':
      {
        int level;

        if (isdigit(*(flag+1))) {
          flag++;
          level = atoi(flag);
        } else {
          CHECK_ARG(1, "compression level");
          level = atoi(argv[1]);
          POP_ARG();
        }
        pdf_set_compression(level);
      }
      break;
      case 'd':
        if (isdigit(*(flag+1))) {
          flag++;
          pdfdecimaldigits = atoi(flag);
        } else {
          CHECK_ARG(1, "number of fractional digits");
          pdfdecimaldigits = atoi(argv[1]);
          POP_ARG();
        }
        break;
      case 'I':
        CHECK_ARG(1, "image cache life in hours");
        image_cache_life = atoi(argv[1]);
        POP_ARG();
        break;
      case 'S':
        do_encryption = 1;
        break;
      case 'K': 
        CHECK_ARG(1, "encryption key length");
        key_bits = (unsigned) atoi(argv[1]);
        if (key_bits < 40 || key_bits > 128 || (key_bits & 0x7))
          ERROR("Invalid encryption key length specified: %s", argv[1]);
        POP_ARG();
        break;
      case 'P': 
        CHECK_ARG(1, "encryption permission flag");
        permission = (unsigned) strtoul(argv[1], &nextptr, 0);
        if (nextptr == argv[1])
          ERROR("Invalid encryption permission flag: %s", argv[1]);
        POP_ARG();
        break;
      case 'O':
        /* Bookmark open level */
        CHECK_ARG(1, "bookmark open level");
        bookmark_open = atoi(argv[1]);
        POP_ARG();
        break;
      case 'M':
        mp_mode = 1;
        break;
      case 'C':
        CHECK_ARG(1, "a number");
        {
          long flags;

          flags = (unsigned) strtol(argv[1], &nextptr, 0);
          if (nextptr == argv[1])
            ERROR("Invalid flag: %s", argv[1]);
          if (flags < 0)
            opt_flags  = -flags;
          else
            opt_flags |=  flags;
        }
        POP_ARG();
        break;
      case 'E':
        always_embed = 1;
        break;
      case 'h':
        show_usage();
        exit(0);
      case 'e':
        if (compat_mode) {
          WARN("dvipdfm \"-e\" option not supported.");
          break;
        } /* else fall through */
      default:
        fprintf (stderr, "Unknown option in \"%s\"", argv[0]);
        usage();
      }
    }
  Out_of_For_Loop:
    POP_ARG();
  }

  if (argc > 1) {
    fprintf(stderr, "Multiple dvi filenames?");
    usage();
  } else if (argc > 0) {
    dvi_filename = NEW(strlen(argv[0]) + 5, char);  /* space to append ".dvi" */
    strcpy(dvi_filename, argv[0]);
  }
}

static void
cleanup (void)
{
  if (dvi_filename)
    RELEASE(dvi_filename);
  if (pdf_filename)
    RELEASE(pdf_filename);
  if (page_ranges)
    RELEASE(page_ranges);
}

static void
read_config_file (const char *config)
{
  const char *start, *end;
  char *option;
  FILE *fp;

  fp = DPXFOPEN(config, DPX_RES_TYPE_TEXT);
  if (!fp) {
    WARN("Could not open config file \"%s\".", config);
    return;
  }
  while ((start = mfgets (work_buffer, WORK_BUFFER_SIZE, fp)) != NULL) {
    char *argv[2];
    int   argc;

    argc = 0;
    end = work_buffer + strlen(work_buffer);
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
  if (fp)
    MFCLOSE(fp);
}

static void
system_default (void)
{
  if (systempapername() != NULL) {
    select_paper(systempapername());
  } else if (defaultpapername() != NULL) {
    select_paper(defaultpapername());
  }
}

void
error_cleanup (void)
{
  pdf_close_images();  /* delete temporary files */
  pdf_error_cleanup();
  if (pdf_filename) {
    remove(pdf_filename);
    fprintf(stderr, "\nOutput file removed.\n");
  }
}

#define SWAP(v1,v2) do {\
   double _tmp = (v1);\
   (v1) = (v2);\
   (v2) = _tmp;\
 } while (0)

static void
do_dvi_pages (void)
{
  long     page_no, page_count, i, step;
  double   page_width, page_height;
  double   init_paper_width, init_paper_height;
  pdf_rect mediabox;

  spc_exec_at_begin_document();

  if (num_page_ranges == 0) {
    if (!page_ranges) {
      page_ranges = NEW(1, struct page_range);
      max_page_ranges = 1;
    }
    page_ranges[0].first = 0;
    page_ranges[0].last  = -1; /* last page */
    num_page_ranges = 1;
  }

  init_paper_width  = page_width  = paper_width;
  init_paper_height = page_height = paper_height;
  page_count  = 0;

  mediabox.llx = 0.0;
  mediabox.lly = 0.0;
  mediabox.urx = paper_width;
  mediabox.ury = paper_height;

  pdf_doc_set_mediabox(0, &mediabox); /* Root node */

  for (i = 0; i < num_page_ranges && dvi_npages(); i++) {
    if (page_ranges[i].last < 0)
      page_ranges[i].last += dvi_npages();

    step    = (page_ranges[i].first <= page_ranges[i].last) ? 1 : -1;
    page_no = page_ranges[i].first;
    while (dvi_npages()) {
      if (page_no < dvi_npages()) {
        double w, h, xo, yo;
        char   lm;

        MESG("[%d", page_no+1);
        /* Users want to change page size even after page is started! */
        page_width = paper_width; page_height = paper_height;
        w = page_width; h = page_height; lm = landscape_mode;
        xo = x_offset; yo = y_offset;
        dvi_scan_specials(page_no, &w, &h, &xo, &yo, &lm, NULL, NULL, NULL, NULL, NULL, NULL);
        if (lm != landscape_mode) {
          SWAP(w, h);
          landscape_mode = lm;
        }
        if (page_width  != w || page_height != h) {
          page_width  = w;
          page_height = h;
        }
        if (x_offset != xo || y_offset != yo) {
          x_offset = xo;
          y_offset = yo;
        }
        if (page_width  != init_paper_width ||
            page_height != init_paper_height) {
          mediabox.llx = 0.0;
          mediabox.lly = 0.0;
          mediabox.urx = page_width;
          mediabox.ury = page_height;
          pdf_doc_set_mediabox(page_count+1, &mediabox);
        }
        dvi_do_page(page_no,
                    page_width, page_height, x_offset, y_offset);
        page_count++;
        MESG("]");
      }

      if (step > 0 &&
          page_no >= page_ranges[i].last)
        break;
      else if (step < 0 &&
               page_no <= page_ranges[i].last)
        break;
      else {
        page_no += step;
      }
    }
  }

  if (page_count < 1) {
    ERROR("No pages fall in range!");
  }

  spc_exec_at_end_document();
}

static void
do_mps_pages (void)
{
  FILE  *fp;

  /* _FIXME_ */
  fp = MFOPEN(dvi_filename, FOPEN_RBIN_MODE);
  if (fp) {
    mps_do_page(fp);
    MFCLOSE(fp);
  } else {
    long  i, page_no, step, page_count = 0;
    char *filename;
    /* Process filename.1, filename.2,... */
    filename = NEW(strlen(dvi_filename) + 16 + 1, char);
    for (i = 0; i < num_page_ranges; i++) {
      step    = (page_ranges[i].first <= page_ranges[i].last) ? 1 : -1;
      page_no = page_ranges[i].first;
      for (;;) {
        if (page_no < 0)
          ERROR("Invalid page number for MPS input: %d", page_no);

        sprintf(filename, "%s.%ld", dvi_filename, page_no + 1);
        fp = MFOPEN(filename, FOPEN_RBIN_MODE);
        if (fp) {
          MESG("[%ld<%s>", page_no + 1, filename);
          mps_do_page(fp);
          page_count++;
          MESG("]");
          MFCLOSE(fp);
        }
        if (step > 0 &&
            page_no >= page_ranges[i].last)
          break;
        else if (step < 0 &&
                 page_no <= page_ranges[i].last)
          break;
        else {
          page_no += step;
        }
      }
    }
    RELEASE(filename);
    if (page_count == 0)
      ERROR("No page output for \"%s\".", dvi_filename);
  }
}

/* TODO: MetaPost mode */
#if defined(MIKTEX)
#  define main Main
#endif
int CDECL
main (int argc, char *argv[]) 
{
  double dvi2pts;
  char *base;
#ifdef WIN32
  int ac;
  char **av, *enc;
#endif

#ifdef MIKTEX
  miktex_initialize();
#else
  kpse_set_program_name(argv[0], "dvipdfmx"); /* we pretend to be dvipdfmx for kpse purposes */
#ifdef WIN32
  texlive_gs_init ();
  enc = kpse_var_value("command_line_encoding");
  if (get_command_line_args_utf8(enc, &ac, &av)) {
    argc = ac;
    argv = av;
  }
#endif
#endif

  /* Special-case single option --help or --version, to avoid possible
     diagnostics about config files, etc.  */
  if (argc == 2 && STREQ (argv[1], "--help")) {
    show_usage();
    exit(0);
  } else if (argc == 2 && STREQ (argv[1], "--version")) {
    show_version();
    exit(0);
  }

  if (argc > 1 &&
               (STREQ (argv[1], "--xbb") ||
                (!is_xetex && STREQ (argv[1], "--dvipdfm")) ||
                STREQ (argv[1], "--ebb"))) {
    argc--;
    base = argv++[1]+2;
  } else
    base = kpse_program_basename (argv[0]);
  
  if (FILESTRCASEEQ (base, "extractbb") || FILESTRCASEEQ (base, "xbb"))
    return extractbb (argc, argv);
  if (FILESTRCASEEQ (base, "ebb")) {
    compat_mode = 1;
    return extractbb (argc, argv);
  }
  
  if (!is_xetex && FILESTRCASEEQ (base, "dvipdfm"))
    compat_mode = 1;
  else
    free (base);

  paperinit();
  system_default();

  argv+=1;
  argc-=1;

  set_verbose(argc, argv);
  /* quiet mode cannot be set in config file */
  if (really_quiet)
    shut_up(really_quiet);

  pdf_init_fontmaps(); /* This must come before parsing options... */

  read_config_file(DPX_CONFIG_FILE);

  do_args (argc, argv);

#ifndef MIKTEX
  kpse_init_prog("", font_dpi, NULL, NULL);
  kpse_set_program_enabled(kpse_pk_format, true, kpse_src_texmf_cnf);
#endif
  pdf_font_set_dpi(font_dpi);
  dpx_delete_old_cache(image_cache_life);

  if (!dvi_filename) {
    if (verbose)
      MESG("No dvi filename specified, reading standard input.\n");
    if (!pdf_filename)
      if (verbose)
        MESG("No pdf filename specified, writing to standard output.\n");
  } else if (!pdf_filename)
    set_default_pdf_filename();

  if (pdf_filename && !strcmp(pdf_filename, "-")) {
    RELEASE(pdf_filename);
    pdf_filename = NULL;
  }

  MESG("%s -> %s\n", dvi_filename ? dvi_filename : "stdin",
                     pdf_filename ? pdf_filename : "stdout");

  pdf_enc_compute_id_string(dvi_filename, pdf_filename);
  if (do_encryption) {
    if (key_bits > 40 && pdf_get_version() < 4)
      ERROR("Chosen key length requires at least PDF 1.4. "
            "Use \"-V 4\" to change.");
    pdf_enc_set_passwd(key_bits, permission, NULL, NULL);
  }

  if (mp_mode) {
    x_offset = 0.0;
    y_offset = 0.0;
    dvi2pts  = 0.01; /* dvi2pts controls accuracy. */
  } else {
    unsigned ver_minor = 0;
    char owner_pw[MAX_PWD_LEN], user_pw[MAX_PWD_LEN];
    /* Dependency between DVI and PDF side is rather complicated... */
    dvi2pts = dvi_init(dvi_filename, mag);
    if (dvi2pts == 0.0)
      ERROR("dvi_init() failed!");

    pdf_doc_set_creator(dvi_comment());

    if (do_encryption)
      /* command line takes precedence */
      dvi_scan_specials(0, &paper_width, &paper_height, &x_offset, &y_offset, &landscape_mode,
			&ver_minor, NULL, NULL, NULL, NULL, NULL);
    else {
      dvi_scan_specials(0, &paper_width, &paper_height, &x_offset, &y_offset, &landscape_mode,
			&ver_minor, &do_encryption, &key_bits, &permission, owner_pw, user_pw);
      if (do_encryption) {
	if (key_bits < 40 || key_bits > 128 || (key_bits & 0x7))
	  ERROR("Invalid encryption key length specified: %u", key_bits);
	else if (key_bits > 40 && pdf_get_version() < 4)
	  ERROR("Chosen key length requires at least PDF 1.4. "
		"Use \"-V 4\" to change.");
	do_encryption = 1;
	pdf_enc_set_passwd(key_bits, permission, owner_pw, user_pw);
      }
    }
    if (ver_minor >= PDF_VERSION_MIN && ver_minor <= PDF_VERSION_MAX) {
      pdf_set_version(ver_minor);
    }
    if (landscape_mode) {
      SWAP(paper_width, paper_height);
    }
  }

  pdf_files_init();

  /* Set default paper size here so that all page's can inherite it.
   * annot_grow:    Margin of annotation.
   * bookmark_open: Miximal depth of open bookmarks.
   */
  pdf_open_document(pdf_filename, do_encryption,
                    paper_width, paper_height, annot_grow, bookmark_open,
                    !(opt_flags & OPT_PDFDOC_NO_DEST_REMOVE));

  /* Ignore_colors placed here since
   * they are considered as device's capacity.
   */
  pdf_init_device(dvi2pts, pdfdecimaldigits, ignore_colors);

  if (opt_flags & OPT_CIDFONT_FIXEDPITCH)
    CIDFont_set_flags(CIDFONT_FORCE_FIXEDPITCH);

  /* Please move this to spc_init_specials(). */
  if (opt_flags & OPT_TPIC_TRANSPARENT_FILL)
    tpic_set_fill_mode(1);

  if (mp_mode) {
    do_mps_pages();
  } else {
    do_dvi_pages();
  }

  pdf_files_close();

  /* Order of close... */
  pdf_close_device  ();
  /* pdf_close_document flushes XObject (image) and other resources. */
  pdf_close_document();

  pdf_close_fontmaps(); /* pdf_font may depend on fontmap. */

  if (!mp_mode)
    dvi_close();

  MESG("\n");
  cleanup();

  paperdone();
#ifdef MIKTEX
  miktex_uninitialize ();
#endif

  return 0;
}
