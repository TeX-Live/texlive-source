/* --------------------------------------------------------------------
 * jpeg2ps: convert JPEG files to compressed PostScript Level 2 EPS
 *
 * (C) 1994-1999 Thomas Merz 
 *
 * ------------------------------------------------------------------*/

#define VERSION		"V1.8"

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#define DOS
#include <oldnames.h>
#endif

#ifndef DOS
#include <unistd.h>
#endif

#ifdef DOS
#include <dos.h>
#include <io.h>
#include <fcntl.h>
#endif

/* try to identify Mac compilers */
#if __POWERPC__ || __CFM68K__ || __MC68K_
#define MAC
#endif

#ifdef MAC
#include "Main.h"		/* Required for DropUNIX */
#endif

#include "psimage.h"

#if (defined(DOS) || defined (MAC))
#define READMODE  "rb"           /* read JPEG files in binary mode */
#define WRITEMODE "wb"           /* write (some) PS files in binary mode */
#else
#define READMODE  "r"
#define WRITEMODE "w"           /* write (some) PS files in binary mode */
#endif

int Margin     	= 20;           /* safety margin */
BOOL quiet	= FALSE;	/* suppress informational messages */
BOOL autorotate = FALSE;	/* disable automatic rotation */

extern BOOL	AnalyzeJPEG P1(imagedata *, image);
extern int	ASCII85Encode P2(FILE *, in, FILE *, out);
extern void     ASCIIHexEncode P2(FILE *, in, FILE *, out);

#ifndef MAC
extern char *optarg;
extern int optind;
#endif

#ifdef DOS
extern int      getopt P3(int, nargc, char **, nargv, char *, ostr);
#endif

#define BUFFERSIZE 1024
static char buffer[BUFFERSIZE];
static char *ColorSpaceNames[] = {"", "Gray", "", "RGB", "CMYK" };

/* Array of known page sizes including name, width, and height */

typedef struct { const char *name; int width; int height; } PageSize_s;

PageSize_s PageSizes[] = {
    {"a0",	2380, 3368},
    {"a1",	1684, 2380},
    {"a2",	1190, 1684},
    {"a3",	842, 1190},
    {"a4",	595, 842},
    {"a5",	421, 595},
    {"a6",	297, 421},
    {"b5",	501, 709},
    {"letter",	612, 792},
    {"legal",	612, 1008},
    {"ledger",	1224, 792},
    {"p11x17",	792, 1224}
};

#define PAGESIZELIST	(sizeof(PageSizes)/sizeof(PageSizes[0]))

#ifdef A4
int PageWidth  = 595;           /* page width A4 */
int PageHeight = 842;           /* page height A4 */
#else
int PageWidth  = 612;           /* page width letter */
int PageHeight = 792;           /* page height letter */
#endif

static void 
JPEGtoPS P2(imagedata *, JPEG, FILE *, PSfile) {
  int llx, lly, urx, ury;        /* Bounding box coordinates */
  size_t n;
  float scale, sx, sy;           /* scale factors            */
  time_t t;
  int i;

  /* read image parameters and fill JPEG struct*/
  if (!AnalyzeJPEG(JPEG)) {
    fprintf(stderr, "Error: '%s' is not a proper JPEG file!\n", JPEG->filename);
    return;
  }

  if (!quiet)
      fprintf(stderr, "Note on file '%s': %dx%d pixel, %d color component%s\n",
	JPEG->filename, JPEG->width, JPEG->height, JPEG->components,
	(JPEG->components == 1 ? "" : "s"));

  /* "Use resolution from file" was requested, but we couldn't find any */
  if (JPEG->dpi == DPI_USE_FILE && !quiet) { 
    fprintf(stderr,
    	"Note: no resolution values found in JPEG file - using standard scaling.\n");
    JPEG->dpi = DPI_IGNORE;
  }

  if (JPEG->dpi == DPI_IGNORE) {
    if (JPEG->width > JPEG->height && autorotate) {	/* switch to landscape if needed */
      JPEG->landscape = TRUE;
      if (!quiet)
	  fprintf(stderr, 
	    "Note: image width exceeds height - producing landscape output!\n");
    }
    if (!JPEG->landscape) {       /* calculate scaling factors */
      sx = (float) (PageWidth - 2*Margin) / JPEG->width;
      sy = (float) (PageHeight - 2*Margin) / JPEG->height;
    }else {
      sx = (float) (PageHeight - 2*Margin) / JPEG->width;
      sy = (float) (PageWidth - 2*Margin) / JPEG->height;
    }
    scale = min(sx, sy);	/* We use at least one edge of the page */
  } else {
    if (!quiet)
	fprintf(stderr, "Note: Using resolution %d dpi.\n", (int) JPEG->dpi);
    scale = 72 / JPEG->dpi;     /* use given image resolution */
  }

  if (JPEG->landscape) {
    /* landscape: move to (urx, lly) */
    urx = PageWidth - Margin;
    lly = Margin;
    ury = (int) (Margin + scale*JPEG->width + 0.9);    /* ceiling */
    llx = (int) (urx - scale * JPEG->height);          /* floor  */
  }else {
    /* portrait: move to (llx, lly) */
    llx = lly = Margin;
    urx = (int) (llx + scale * JPEG->width + 0.9);     /* ceiling */
    ury = (int) (lly + scale * JPEG->height + 0.9);    /* ceiling */
  }

  time(&t);

  /* produce EPS header comments */
  fprintf(PSfile, "%%!PS-Adobe-3.0 EPSF-3.0\n");
  fprintf(PSfile, "%%%%Creator: jpeg2ps %s by Thomas Merz\n", VERSION);
  fprintf(PSfile, "%%%%Title: %s\n", JPEG->filename);
  fprintf(PSfile, "%%%%CreationDate: %s", ctime(&t));
  fprintf(PSfile, "%%%%BoundingBox: %d %d %d %d\n", 
                   llx, lly, urx, ury);
  fprintf(PSfile, "%%%%DocumentData: %s\n", 
                  JPEG->mode == BINARY ? "Binary" : "Clean7Bit");
  fprintf(PSfile, "%%%%LanguageLevel: 2\n");
  fprintf(PSfile, "%%%%EndComments\n");
  fprintf(PSfile, "%%%%BeginProlog\n");
  fprintf(PSfile, "%%%%EndProlog\n");
  fprintf(PSfile, "%%%%Page: 1 1\n");

  fprintf(PSfile, "/languagelevel where {pop languagelevel 2 lt}");
  fprintf(PSfile, "{true} ifelse {\n");
  fprintf(PSfile, "  (JPEG file '%s' needs PostScript Level 2!",
                  JPEG->filename);
  fprintf(PSfile, "\\n) dup print flush\n");
  fprintf(PSfile, "  /Helvetica findfont 20 scalefont setfont ");
  fprintf(PSfile, "100 100 moveto show showpage stop\n");
  fprintf(PSfile, "} if\n");

  fprintf(PSfile, "save\n");
  fprintf(PSfile, "/RawData currentfile ");

  if (JPEG->mode == ASCIIHEX)            /* hex representation... */
    fprintf(PSfile, "/ASCIIHexDecode filter ");
  else if (JPEG->mode == ASCII85)        /* ...or ASCII85         */
    fprintf(PSfile, "/ASCII85Decode filter ");
  /* else binary mode: don't use any additional filter! */

  fprintf(PSfile, "def\n");

  fprintf(PSfile, "/Data RawData << ");
  fprintf(PSfile, ">> /DCTDecode filter def\n");

  /* translate to lower left corner of image */
  fprintf(PSfile, "%d %d translate\n", (JPEG->landscape ? 
                   PageWidth - Margin : Margin), Margin);

  if (JPEG->landscape)                 /* rotation for landscape */
    fprintf(PSfile, "90 rotate\n");
      
  fprintf(PSfile, "%.2f %.2f scale\n", /* scaling */
                   JPEG->width * scale, JPEG->height * scale);
  fprintf(PSfile, "/Device%s setcolorspace\n", 
                  ColorSpaceNames[JPEG->components]);
  fprintf(PSfile, "{ << /ImageType 1\n");
  fprintf(PSfile, "     /Width %d\n", JPEG->width);
  fprintf(PSfile, "     /Height %d\n", JPEG->height);
  fprintf(PSfile, "     /ImageMatrix [ %d 0 0 %d 0 %d ]\n",
                  JPEG->width, -JPEG->height, JPEG->height);
  fprintf(PSfile, "     /DataSource Data\n");
  fprintf(PSfile, "     /BitsPerComponent %d\n", 
                  JPEG->bits_per_component);

  /* workaround for color-inverted CMYK files produced by Adobe Photoshop:
   * compensate for the color inversion in the PostScript code
   */
  if (JPEG->adobe && JPEG->components == 4) {
    if (!quiet)
	fprintf(stderr, "Note: Adobe-conforming CMYK file - applying workaround for color inversion.\n");
    fprintf(PSfile, "     /Decode [1 0 1 0 1 0 1 0]\n");
  }else {
    fprintf(PSfile, "     /Decode [0 1");
    for (i = 1; i < JPEG->components; i++) 
      fprintf(PSfile," 0 1");
    fprintf(PSfile, "]\n");
  }

  fprintf(PSfile, "  >> image\n");
  fprintf(PSfile, "  Data closefile\n");
  fprintf(PSfile, "  RawData flushfile\n");
  fprintf(PSfile, "  showpage\n");
  fprintf(PSfile, "  restore\n");
  fprintf(PSfile, "} exec");

  /* seek to start position of JPEG data */
  fseek(JPEG->fp, JPEG->startpos, SEEK_SET);

  switch (JPEG->mode) {
  case BINARY:
    /* important: ONE blank and NO newline */
    fprintf(PSfile, " ");
#ifdef DOS
    fflush(PSfile);         	  /* up to now we have CR/NL mapping */
    setmode(fileno(PSfile), O_BINARY);    /* continue in binary mode */
#endif
    /* copy data without change */
    while ((n = fread(buffer, 1, sizeof(buffer), JPEG->fp)) != 0)
      fwrite(buffer, 1, n, PSfile);
#ifdef DOS
    fflush(PSfile);                  	/* binary yet */
    setmode(fileno(PSfile), O_TEXT);    /* text mode */
#endif
    break;

  case ASCII85:
    fprintf(PSfile, "\n");

    /* ASCII85 representation of image data */
    if (ASCII85Encode(JPEG->fp, PSfile)) {
      fprintf(stderr, "Error: internal problems with ASCII85Encode!\n");
      exit(1);
    }
    break;

  case ASCIIHEX:
    /* hex representation of image data (useful for buggy dvips) */
    ASCIIHexEncode(JPEG->fp, PSfile);
    break;
  }
  fprintf(PSfile, "\n%%%%EOF\n");
}

static
void usage P0(void) {
  fprintf(stderr, "jpeg2ps %s: convert JPEG files to PostScript Level 2.\n",
		   VERSION);
  fprintf(stderr, "(C) Thomas Merz 1994-1999\n\n");
  fprintf(stderr, "usage: jpeg2ps [options] jpegfile > epsfile\n");
  fprintf(stderr, "-a        auto rotate: produce landscape output if width > height\n");
  fprintf(stderr, "-b        binary mode: output 8 bit data (default: 7 bit with ASCII85)\n");
  fprintf(stderr, "-h        hex mode: output 7 bit data in ASCIIHex encoding\n");
  fprintf(stderr, "-o <name> output file name\n");
  fprintf(stderr, "-p <size> page size name. Known names are:\n");
  fprintf(stderr, "          a0, a1, a2, a3, a4, a5, a6, b5, letter, legal, ledger, p11x17\n");
  fprintf(stderr, "-q        quiet mode: suppress all informational messages\n");
  fprintf(stderr, "-r <dpi>  resolution value (dots per inch)\n");
  fprintf(stderr, "          0 means use value given in file, if any (disables autorotate)\n");
  exit(1);
}

int
main P2(int, argc, char **, argv) {
  imagedata image;
  FILE *outfile;

#ifdef MAC
  int i, bufLength;
  char *cp, outfilename[512];
#else
  int opt, pagesizeindex = -1;
#endif

  image.filename = NULL;
  image.mode     = ASCII85;
  image.startpos = 0L;
  image.landscape= FALSE;
  image.dpi      = DPI_IGNORE;
  image.adobe    = FALSE;

  outfile = stdout;

 if (argc == 1)
    usage();

#ifndef MAC
  while ((opt = getopt(argc, argv, "abho:p:qr:")) != -1)
    switch (opt) {
      case 'a':
          autorotate = TRUE;
          break;
      case 'b':
	  image.mode = BINARY;
	  break;
      case 'h':
	  image.mode = ASCIIHEX;
	  break;
      case 'o':
	  outfile = fopen(optarg, "w");
	  if (outfile == NULL) {
	    fprintf(stderr, "Error: cannot open output file %s.\n", optarg);
	    exit(-2);
	  } 
	  break;
      case 'p':
	  for(pagesizeindex=0; pagesizeindex < PAGESIZELIST; pagesizeindex++)
	    if (!strcmp((const char *) optarg, PageSizes[pagesizeindex].name)) {
		PageHeight = PageSizes[pagesizeindex].height;
		PageWidth = PageSizes[pagesizeindex].width;
		break;
	    }
	  if (pagesizeindex == PAGESIZELIST) {	/* page size name not found */
		fprintf(stderr, "Error: Unknown page size %s.\n", optarg);
		exit(-3);
	  }
	  break;
      case 'q':
          quiet = TRUE;
	  break;
      case 'r':
	  image.dpi = (float) atof(optarg);
	  if (image.dpi < 0) {
	    fprintf(stderr, "Error: bad resolution value %f !\n", image.dpi);
	    exit(1);
	  }
	  break;
      case '?':
	  usage();
    }

  if (pagesizeindex != -1 && ! quiet)	/* page size user option given */
      fprintf(stderr, "Note: Using %s page size.\n",
		    PageSizes[pagesizeindex].name);

  if (optind == argc)	/* filename missing */
    usage();
  else
    image.filename = argv[optind];

  if (!image.filename)
    usage();

  if ((image.fp = fopen(image.filename, READMODE)) == NULL) {
    fprintf(stderr, "Error: couldn't read JPEG file '%s'!\n", 
      image.filename),
    exit(1);
  }

  JPEGtoPS(&image, outfile);      /* convert JPEG data */
  fclose(image.fp);
  fclose(outfile);

#else /* MAC */

 for (i = 1; i < argc; i++) {
    image.filename = argv[i];

    strcpy(outfilename, image.filename);
    bufLength = strlen(outfilename);
    cp = outfilename;
    if (bufLength > 3)
    {
	cp += (bufLength - 4);
	/* strip .jpg from terminating string */
	if (strcmp(cp, ".jpg") == 0 || strcmp(cp, ".JPG") == 0) 
	    outfilename[bufLength - 4] = '\0'; 		
    }
    
    strcat(outfilename, ".eps");

     if ((image.fp = fopen(image.filename, READMODE)) == NULL) {
	fprintf(stderr, "Error: couldn't read JPEG file '%s'!\n", 
	image.filename),
	exit(1);
     }

    outfile = fopen(outfilename, WRITEMODE);

    JPEGtoPS(&image, outfile);      /* convert JPEG data */

    fclose(image.fp);
    fclose(outfile);
  }

#endif	/* not MAC */

  return 0;
}
