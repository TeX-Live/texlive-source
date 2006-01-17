/* ------------------------- readjpeg.c ------------------------- */
#include <stdio.h>
#include <string.h>

#if !defined(DOS) && !defined(WIN32)
#include <unistd.h>
#endif

#include "psimage.h"

/* The following enum is stolen from the IJG JPEG library
 * Comments added by tm
 * This table contains far too many names since jpeg2ps
 * is rather simple-minded about markers
 */

extern BOOL quiet;

typedef enum {		/* JPEG marker codes			*/
  M_SOF0  = 0xc0,	/* baseline DCT				*/
  M_SOF1  = 0xc1,	/* extended sequential DCT		*/
  M_SOF2  = 0xc2,	/* progressive DCT			*/
  M_SOF3  = 0xc3,	/* lossless (sequential)		*/
  
  M_SOF5  = 0xc5,	/* differential sequential DCT		*/
  M_SOF6  = 0xc6,	/* differential progressive DCT		*/
  M_SOF7  = 0xc7,	/* differential lossless		*/
  
  M_JPG   = 0xc8,	/* JPEG extensions			*/
  M_SOF9  = 0xc9,	/* extended sequential DCT		*/
  M_SOF10 = 0xca,	/* progressive DCT			*/
  M_SOF11 = 0xcb,	/* lossless (sequential)		*/
  
  M_SOF13 = 0xcd,	/* differential sequential DCT		*/
  M_SOF14 = 0xce,	/* differential progressive DCT		*/
  M_SOF15 = 0xcf,	/* differential lossless		*/
  
  M_DHT   = 0xc4,	/* define Huffman tables		*/
  
  M_DAC   = 0xcc,	/* define arithmetic conditioning table	*/
  
  M_RST0  = 0xd0,	/* restart				*/
  M_RST1  = 0xd1,	/* restart				*/
  M_RST2  = 0xd2,	/* restart				*/
  M_RST3  = 0xd3,	/* restart				*/
  M_RST4  = 0xd4,	/* restart				*/
  M_RST5  = 0xd5,	/* restart				*/
  M_RST6  = 0xd6,	/* restart				*/
  M_RST7  = 0xd7,	/* restart				*/
  
  M_SOI   = 0xd8,	/* start of image			*/
  M_EOI   = 0xd9,	/* end of image				*/
  M_SOS   = 0xda,	/* start of scan			*/
  M_DQT   = 0xdb,	/* define quantization tables		*/
  M_DNL   = 0xdc,	/* define number of lines		*/
  M_DRI   = 0xdd,	/* define restart interval		*/
  M_DHP   = 0xde,	/* define hierarchical progression	*/
  M_EXP   = 0xdf,	/* expand reference image(s)		*/
  
  M_APP0  = 0xe0,	/* application marker, used for JFIF	*/
  M_APP1  = 0xe1,	/* application marker			*/
  M_APP2  = 0xe2,	/* application marker			*/
  M_APP3  = 0xe3,	/* application marker			*/
  M_APP4  = 0xe4,	/* application marker			*/
  M_APP5  = 0xe5,	/* application marker			*/
  M_APP6  = 0xe6,	/* application marker			*/
  M_APP7  = 0xe7,	/* application marker			*/
  M_APP8  = 0xe8,	/* application marker			*/
  M_APP9  = 0xe9,	/* application marker			*/
  M_APP10 = 0xea,	/* application marker			*/
  M_APP11 = 0xeb,	/* application marker			*/
  M_APP12 = 0xec,	/* application marker			*/
  M_APP13 = 0xed,	/* application marker			*/
  M_APP14 = 0xee,	/* application marker, used by Adobe	*/
  M_APP15 = 0xef,	/* application marker			*/
  
  M_JPG0  = 0xf0,	/* reserved for JPEG extensions		*/
  M_JPG13 = 0xfd,	/* reserved for JPEG extensions		*/
  M_COM   = 0xfe,	/* comment				*/
  
  M_TEM   = 0x01,	/* temporary use			*/

  M_ERROR = 0x100	/* dummy marker, internal use only	*/
} JPEG_MARKER;

/*
 * The following routine used to be a macro in its first incarnation:
 *  #define get_2bytes(fp) ((unsigned int) (getc(fp) << 8) + getc(fp))
 * However, this is bad programming since C doesn't guarantee
 * the evaluation order of the getc() calls! As suggested by
 * Murphy's law, there are indeed compilers which produce the wrong
 * order of the getc() calls, e.g. the Metrowerks C compilers for BeOS
 * and Macintosh.
 * Since there are only few calls we don't care about the performance 
 * penalty and use a simplistic C function.
 */

/* read two byte parameter, MSB first */
static unsigned int
get_2bytes(FILE *fp)
{
    unsigned int val;
    val = getc(fp) << 8;
    val += getc(fp);
    return val;
}

static int 
next_marker P1(FILE *, fp)
{ /* look for next JPEG Marker  */
  int c, nbytes = 0;

  if (feof(fp))
    return M_ERROR;                 /* dummy marker               */

  do {
    do {                            /* skip to FF 		  */
      nbytes++;
      c = getc(fp);
    } while (c != 0xFF);
    do {                            /* skip repeated FFs  	  */
      c = getc(fp);
    } while (c == 0xFF);
  } while (c == 0);                 /* repeat if FF/00 	      	  */

  return c;
}

/* analyze JPEG marker */
BOOL AnalyzeJPEG P1(imagedata *, image) {
  int b, c, unit;
  unsigned long i, length = 0;
#define APP_MAX 255
  unsigned char appstring[APP_MAX];
  BOOL SOF_done = FALSE;

  /* Tommy's special trick for Macintosh JPEGs: simply skip some  */
  /* hundred bytes at the beginning of the file!		  */
  do {
    do {                            /* skip if not FF 		  */
      c = getc(image->fp);
    } while (!feof(image->fp) && c != 0xFF);

    do {                            /* skip repeated FFs 	  */
      c = getc(image->fp);
    } while (c == 0xFF);

    /* remember start position */
    if ((image->startpos = ftell(image->fp)) < 0L) {
      fprintf(stderr, "Error: internal error in ftell()!\n");
      return FALSE;
    }
    image->startpos -= 2;           /* subtract marker length     */

    if (c == M_SOI) {
      fseek(image->fp, image->startpos, SEEK_SET);
      break;
    }
  } while (!feof(image->fp));

  if (feof(image->fp)) {
    fprintf(stderr, "Error: SOI marker not found!\n");
    return FALSE;
  }

  if (image->startpos > 0L && !quiet) {
    fprintf(stderr, "Note: skipped %ld bytes ", image->startpos);
    fprintf(stderr, "Probably Macintosh JPEG file?\n");
  }

  /* process JPEG markers */
  while (!SOF_done && (c = next_marker(image->fp)) != M_EOI) {
    switch (c) {
      case M_ERROR:
	fprintf(stderr, "Error: unexpected end of JPEG file!\n");
	return FALSE;

      /* The following are not officially supported in PostScript level 2 */
      case M_SOF2:
      case M_SOF3:
      case M_SOF5:
      case M_SOF6:
      case M_SOF7:
      case M_SOF9:
      case M_SOF10:
      case M_SOF11:
      case M_SOF13:
      case M_SOF14:
      case M_SOF15:
	fprintf(stderr, 
         "Warning: JPEG file uses compression method %X - proceeding anyway.\n",
		  c);
        fprintf(stderr, 
		"PostScript output does not work on all PS interpreters!\n");
	/* FALLTHROUGH */

      case M_SOF0:
      case M_SOF1:
	length = get_2bytes(image->fp);    /* read segment length  */

	image->bits_per_component = getc(image->fp);
	image->height             = get_2bytes(image->fp);
	image->width              = get_2bytes(image->fp);
	image->components         = getc(image->fp);

	SOF_done = TRUE;
	break;

      case M_APP0:		/* check for JFIF marker with resolution */
	length = get_2bytes(image->fp);

	for (i = 0; i < length-2; i++) {	/* get contents of marker */
	  b = getc(image->fp);
	  if (i < APP_MAX)			/* store marker in appstring */
	    appstring[i] = b;
	}

	/* Check for JFIF application marker and read density values
	 * per JFIF spec version 1.02.
	 * We only check X resolution, assuming X and Y resolution are equal.
	 * Use values only if resolution not preset by user or to be ignored.
	 */

#define ASPECT_RATIO	0	/* JFIF unit byte: aspect ratio only */
#define DOTS_PER_INCH	1	/* JFIF unit byte: dots per inch     */
#define DOTS_PER_CM	2	/* JFIF unit byte: dots per cm       */

	if (image->dpi == DPI_USE_FILE && length >= 14 &&
	    !strncmp((const char *)appstring, "JFIF", 4)) {
	  unit = appstring[7];		        /* resolution unit */
	  					/* resolution value */
	  image->dpi = (float) ((appstring[8]<<8) + appstring[9]);

	  if (image->dpi == 0.0) {
	    image->dpi = DPI_USE_FILE;
	    break;
	  }

	  switch (unit) {
	    /* tell the caller we didn't find a resolution value */
	    case ASPECT_RATIO:
	      image->dpi = DPI_USE_FILE;
	      break;

	    case DOTS_PER_INCH:
	      break;

	    case DOTS_PER_CM:
	      image->dpi *= (float) 2.54;
	      break;

	    default:				/* unknown ==> ignore */
	      fprintf(stderr, 
		"Warning: JPEG file contains unknown JFIF resolution unit - ignored!\n");
	      image->dpi = DPI_IGNORE;
	      break;
	  }
	}
        break;

      case M_APP14:				/* check for Adobe marker */
	length = get_2bytes(image->fp);

	for (i = 0; i < length-2; i++) {	/* get contents of marker */
	  b = getc(image->fp);
	  if (i < APP_MAX)			/* store marker in appstring */
	    appstring[i] = b;
	}

	/* Check for Adobe application marker. It is known (per Adobe's TN5116)
	 * to contain the string "Adobe" at the start of the APP14 marker.
	 */
	if (length >= 12 && !strncmp((const char *) appstring, "Adobe", 5))
	  image->adobe = TRUE;			/* set Adobe flag */

	break;

      case M_SOI:		/* ignore markers without parameters */
      case M_EOI:
      case M_TEM:
      case M_RST0:
      case M_RST1:
      case M_RST2:
      case M_RST3:
      case M_RST4:
      case M_RST5:
      case M_RST6:
      case M_RST7:
	break;

      default:			/* skip variable length markers */
	length = get_2bytes(image->fp);
	for (length -= 2; length > 0; length--)
	  (void) getc(image->fp);
	break;
    }
  }

  /* do some sanity checks with the parameters */
  if (image->height <= 0 || image->width <= 0 || image->components <= 0) {
    fprintf(stderr, "Error: DNL marker not supported in PostScript Level 2!\n");
    return FALSE;
  }

  /* some broken JPEG files have this but they print anyway... */
  if (length != (unsigned int) (image->components * 3 + 8))
    fprintf(stderr, "Warning: SOF marker has incorrect length - ignored!\n");

  if (image->bits_per_component != 8) {
    fprintf(stderr, "Error: %d bits per color component ",
	image->bits_per_component);
    fprintf(stderr, "not supported in PostScript level 2!\n");
    return FALSE;
  }

  if (image->components!=1 && image->components!=3 && image->components!=4) {
    fprintf(stderr, "Error: unknown color space (%d components)!\n",
      image->components);
    return FALSE;
  }

  return TRUE;
}
