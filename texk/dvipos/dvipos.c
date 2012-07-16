/*  dvipos-20070107

    Copyright (C) 2003 by Jin-Hwan <chofchof@ktug.or.kr>
    
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

#include "utils.h"
#include "dvicore.h"

#include <limits.h>
#ifdef MIKTEX
#include <getopt.h>
#endif

/* Global variables */
int verbose = M_FAIL, do_smashchars = 0; /* output message only if failed */
FILE *infp = NULL, *outfp = NULL, *bbxfp = NULL, *frmfp = NULL;
char *infname = NULL, *bbxfname = NULL, *frmfname = NULL;
SIGNED_QUAD dbg_location = 0, rule_width = 6554; /* .1pt */

/* Internal variables */
static char *outfname = NULL;
static char deffname[] = "default";
static int nmag = 0, resol = 300, bpage = 1, epage = INT_MAX;

#ifdef MIKTEX
#define PLATFORM "(MiKTeX 2.2)"
#else
#define PLATFORM "(KPATHSEA)"
#endif

static void title (void)
{
  msg_out(M_VERBOSE, "This is DVIpos, Version %s %s\nby Jin-Hwan Cho <chofchof@ktug.or.kr>\n\n", VERSION, PLATFORM);
}

static void usage (void)
{
  fprintf(stdout, "Usage: dvipos [OPTION...] infile[.dvi]\n\n"
                  "-h,  --help           print usage\n"
                  "-W,  --warning        print warnings\n"
                  "-v,  --verbose        print verbose output\n"
                  "-d,  --debug          print 'dvitype' debugging data\n"
                  "-s,  --smashchars     regard height and depth as zero\n"
                  "-o,  --output=FILE    send all output to FILE\n"
                  "-b,  --bbox[=FILE]    send bounding box to FILE (default FILE is infile.pos)\n"
                  "-f,  --framed[=BASE]  request copy of DVI file, BASE.dvi, with bounding boxes\n"
		  "                      *framed* (default FILE is infile_frames.dvi)\n"
                  "-m,  --mag=INT        override TeX magnification by INT\n"
//                "-r,  --resolution=INT set resolution by INT\n"
                  "-w,  --framesize=INT   set frame rule size by INT (default 6554 = .1pt)\n"
                  "-p,  --pages=X:Y      set page ranges from X to Y\n\n");
}

static struct option long_options[] = {
  /* Options without arguments: */
  { "debug", no_argument, NULL, 'd' },
  { "help", no_argument, NULL, 'h' },
  { "verbose", no_argument, NULL, 'v' },
  { "warning", no_argument, NULL, 'W' },
  { "smashchars", no_argument, NULL, 's' },
  /* Options accepting an argument: */
  { "mag", required_argument, NULL, 'm' },
  { "framesize", required_argument, NULL, 'w' },
  { "pages", required_argument, NULL, 'p' },
  { "resolution", required_argument, NULL, 'r' },
  { "output", optional_argument, NULL, 'o' },
  /* Options accepting an optional argument: */
  { "bbox", optional_argument, NULL, 'b' },
  { "framed", optional_argument, NULL, 'f' },
  { 0, 0, 0, 0 }
};

static void read_options (int argc, char **argv)
{
  int op;

  while ((op = getopt_long(argc, argv, "dhvwm:s:r:p:o:b::f::", long_options, (int *)0)) != EOF) {
    char *p, *q;
    switch (op) {
    case 'd': /* debug */
      verbose |= M_DEBUG;
      break;
    case 'h': /* help */
      verbose |= M_VERBOSE; title(); usage(); exit(1);
      break;
    case 'v': /* verbose */
      verbose |= M_VERBOSE;
      break;
    case 'W': /* warning */
      verbose |= M_WARNING;
      break;
    case 's': /* smashchars */
      do_smashchars = 1;
      break;
    case 'm': /* mag */
      if (*(q = optarg) == '=') q++;
      nmag = (int)strtol(q, &p, 0);
      if (p == q || nmag < 0)
        msg_out(M_FAIL, "Missing a number for magnification.\n");
      break;
    case 'r': /* resolution */
      if (*(q = optarg) == '=') q++;
      resol = (int)strtol(q, &p, 0);
      if (p == q || resol < 0)
        msg_out(M_FAIL, "Missing a number for resolution.\n");
      break;
    case 'w': /* framesize */
      if (*(q = optarg) == '=') q++;
      rule_width = (int)strtol(q, &p, 0);
      if (p == q || rule_width < 0)
        msg_out(M_FAIL, "Missing a number for frame rule size.\n");
      break;
    case 'p': /* pages */
      if (*(q = optarg) == '=') q++;
      bpage = (int)strtol(q, &p, 0);
      if (p == q) msg_out(M_FAIL, "Missing a page range.\n");
      if (*p == ':') {
        epage = (int)strtol(q = p + 1, &p, 0);
	if (p == q) msg_out(M_FAIL, "Missing a 2nd page number.\n");
      }
      if (bpage > epage) {
        int sw = bpage; bpage = epage; epage = sw;
      }
      break;
    case 'o': /* output */
      if (*(q = optarg) == '=') q++;
      if (strlen(q) > 0) outfname = xstrdup(q);
      else msg_out(M_FAIL, "Missing an output file name.\n");
      break;
    case 'b': /* bbox */
      bbxfname = deffname;
      if (optarg) {
        if (*(q = optarg) == '=') q++;
        if (strlen(q) > 0) bbxfname = xstrdup(q);
      }
      break;
    case 'f': /* framed */
      frmfname = deffname;
      if (optarg) {
        if (*(q = optarg) == '=') q++;
        if (strlen(q) > 0) frmfname = xstrdup(q);
      }
      break;
    default:
      msg_out(M_FAIL, "Unparsed option (%d)\n", op);
    }
  }

  /* The first non-optional argument is to be used as the DVI file name */
  if (optind < argc) {
    infname = make_suffix(argv[optind], "dvi");
    optind++;
    while (optind < argc)
      msg_out(M_WARNING, "Warning: Ignore unparsed argument (%s).\n", argv[optind++]);
  }

  /* set up all the options */
  msg_out(M_DEBUG, "Options selected:\n");
  msg_out(M_DEBUG, "  Starting page = * \n");
  msg_out(M_DEBUG, "  Maximum number of pages = 1000000\n");
  msg_out(M_DEBUG, "  Output level = 4 (the works)\n");
  msg_out(M_DEBUG, "  Resolution = %d.00000000 pixels per inch\n", resol);
  if (nmag > 0)
    msg_out(M_DEBUG, "  New magnification factor = %-8.03f\n", nmag / 1000.);
}

int main (int argc, char *argv[]) 
{
  int i;

  if (argc > 1)
    read_options(argc, argv);

  title();

  if (infname == NULL)
    msg_out(M_FAIL, "No DVI filename specified.\n");

//  if (outfname == NULL)
//    outfname = make_suffix(infname, "tuo");

  if (bbxfname == deffname)
    bbxfname = make_suffix(infname, "pos");

  if (frmfname) {
    if (frmfname == deffname) {
      frmfname = make_suffix(infname, "frames.dvi");
      *(frmfname+strlen(frmfname)-11) = '_';
    } else {
      char *p = frmfname;
      frmfname = make_suffix(p, "dvi");
      free(p);
    }
  }

  /* Open the input DVI file */
  if ((infp = fopen(infname, "rb")) == NULL)
    msg_out(M_FAIL, "Could not open the input file %s.", infname);

  /* Open the output file */
  if (outfname == NULL) {
    outfp = stdout;
#ifdef WIN32
    setmode(fileno(stdout), _O_BINARY);
#endif
  } else if ((outfp = fopen(outfname, "ab")) == NULL)
    msg_out(M_FAIL, "Could not open the output file %s.\n", outfname);

  /* Open the boundingbox file */
  if (bbxfname && (bbxfp = fopen(bbxfname, "wb")) == NULL)
    msg_out(M_FAIL, "Could not open the boundingbox file %s.\n", bbxfname);

  /* Open the framed DVI file */
  if (frmfname && (frmfp = fopen(frmfname, "wb")) == NULL)
    msg_out(M_FAIL, "Could not open the framed DVI file %s.\n", frmfname);

#ifdef KPATHSEA
  kpse_set_program_name(argv[0], "dvipos");
#elif MIKTEX
  miktex_initialize();
#endif

  dvi_init(nmag, resol);

  if (bpage <= 0) bpage = 1;
  if (epage >= dvi_pages) epage = dvi_pages;

  for (i = bpage; i <= epage; i++)
    dvi_do_page(i);

  dvi_close();

  if (infname) free(infname);
  if (outfname) free(outfname);
  if (bbxfname) free(frmfname);
  if (frmfname) free(frmfname);

  if (outfp) fclose(outfp);
  if (bbxfp) fclose(bbxfp);
  if (frmfp) fclose(frmfp);

#ifdef MIKTEX
  miktex_uninitialize();
#endif

  exit(0);
}
