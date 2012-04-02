/* The rest of the code has the following copyright:

Copyright (c) 1990-2004  Paul Vojta and others

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
PAUL VOJTA OR ANY OTHER AUTHOR OF THIS SOFTWARE BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

NOTE:
xdvi is based on prior work, as noted in the modification history
in xdvi.c.

\*========================================================================*/

#include "xdvi-config.h"
#include "xdvi.h"

#include <stdarg.h>

#include <stdlib.h>
#include <ctype.h>

#include <setjmp.h>

#define USE_HASH

#include <ctype.h>
#include "kpathsea/c-fopen.h"
#include "kpathsea/c-stat.h"
#include "kpathsea/magstep.h"
#include "kpathsea/tex-file.h"

#include <string.h>

#include "dvi.h"
#include "string-utils.h"
#include "util.h"
#include "x_util.h"
#include "events.h"
#include "dvi-init.h"
#include "statusline.h"
#include "hypertex.h"
#include "special.h"
#include "tfmload.h"
#include "read-mapfile.h"
#include "my-snprintf.h"
#include "kpathsea/tex-file.h"
#include "mag.h"
#include "message-window.h"

#include "dvi-draw.h"
#include "search-internal.h"
#include "encodings.h"
#include "pagesel.h"
#include "pagehist.h"

#ifdef RGB_ANTI_ALIASING
#define TRACE_AA 0
#define TRACE_AA1 1
#define TRACE_AA3 0
#else
#define TRACE_AA 0
#define TRACE_AA1 0
#define TRACE_AA3 0
#endif

#define MY_DEBUG 0

#if MY_DEBUG
# define TRACE_FIND_VERBOSE(x) TRACE_FIND(x)
#else
# define TRACE_FIND_VERBOSE(x) /* as nothing */
#endif

#if PS && PS_GS
#include "psgs.h"
#endif /* PS && PS_GS */

#ifdef	DOPRNT	/* define this if vfprintf gives you trouble */
# define vfprintf(stream, message, args)	_doprnt(message, args, stream)
#endif

#define BUF_SIZE 1024

/*
 *	All calculations are done with shrink factor = 1, so we re-do some
 *	macros accordingly.  Many of these are also defined in special.c.
 */

#define	xpixel_conv(x)	((int) ((x) >> 16))
#define	xpixel_round(x)	((int) ROUNDUP(x, 1 << 16))

#define	G_PXL_H		xpixel_conv(currinf.data.dvi_h)
#define	G_OFFSET_X	(resource.xoffset_int << 16) + (3 << 15)
#define	G_OFFSET_Y	(resource.yoffset_int << 16) + (3 << 15)

#ifdef T1LIB
/* hashing stuff to speed up checking whether a font or tfminfo file
   has already been loaded.
*/
#define T1FONTS_INITIAL_HASHTABLE_SIZE 1031
static hashTableT t1fonts_hash;
static hashTableT tfminfo_hash;
static hashTableT fontmaps_hash;
/* static hashTableT font_warn_hash; */

typedef enum { FAILURE_BLANK = -2, FAILURE_PK = -1, SUCCESS = 0 } t1FontLoadStatusT;
#endif

#if PS
int scanned_page_ps;
int scanned_page_ps_bak;
#endif

#if COLOR
int scanned_page_color;
#endif

int scanned_page_reset, scanned_page;

struct drawinf currinf;
Boolean htex_inside_href = False; /* whether we're inside a href */

Boolean	drawing_mag = False;

#ifdef T1LIB

# include "t1lib.h"

/* Datastructures:

encodings[n]: Loaded from map file
[0] = 8r,	<vector>
[1] = 8c,	<vector>

The vectors are demand-loaded in 'load_encoded_font'
*/

struct encoding {
    char *enc;	/* This may be NULL.  From dvips maps we get
		   encodings loaded by filename, and no name given */
    char *file;
    char **vector;
};

static struct encoding *encodings = NULL;	/* Dynamic array = realloc when it gets full */
static size_t enclidx = 0;			/* global, for communication with find_T1_font */

/*
  t1fonts[n]: Built as xdvi loads t1 fonts
  idx	 file			short	t1id	loaded
  [0] = /path/cmr10.pfb,	cmr10	0	1
  [1] = /path/prmr.pfa,	prmr	1	0
  [2] = /path/pcrr8a.pfa,	pcrr8a	2	0
  
  This array enumerates the loaded t1 fonts by full path name.
  The integer `t1id' is the t1lib font id.  The font will not be
  extended/slanted/encoded.  The `loaded' field indicates if
  T1_LoadFont has been called for the font yet.  Texfonts that need a
  modified version of the t1 font must use T1_CopyFont to copy the
  raw font and obtain the id of a new t1 font which can then be
  modified.  As described in the t1 docs a font must be loaded before
  it can be copied.
  
  Fonts that are copies of other fonts have NULL filenames.  If any
  font files are to be reused based on the file name, the raw font
  must be reused, not a copy.
  
  The `short' field contains the font as it was identified in the
  fontmap.  This is used to save searching; if two fonts are
  identified the same way in the fontmap, they are the same font.
  
*/

struct t1font_info {
    char *file;
    char *shortname;
    int t1id;
    int loaded;
};

static struct t1font_info *t1fonts = NULL;	/* Dynamic array */

/*
  fontmaps[n]: Loaded from map file, and extended with 'implied'
  fonts as needed.
  idx	 texname enc	exten.	slant	filena.	t1libid	pathn.	tfmidx
  [0] = pcrr8rn 0	750	0	pcrr8a	2	...	2
  [1] = putro8r 0	0	0	putro8r 3	...	3

  The first 5 fields of this table are loaded from the font map
  files, or filled out with default values when implied fonts are
  set-up.  The t1libid is -1 until the font is loaded or copied as
  described under t1fonts.  Once the dvi file reveals that the font
  is needed, it is located on disk to ensure it exists, and the
  pathname field is filled out.  The t1 font is not loaded or
  copied before it's needed because set_t1_char has been called to
  draw a glyph from the font.  The late loading is necessiated by the
  high cost of loading fonts.  If the font loading is concentrated at
  startup of the program the startup-time becomes ecessively high due
  to the high number of fonts used by the average LaTeX document.

  A value of 0 for `extension'/`slant' means no extension/slant.  If
  the input values are decimal (less than 10) they're multiplied by
  1000 and 10000 respectively to obtain fixed-point integer values.
  (This means that xdvi will translate entries like `0.81' to 810.)
  Integer values have the advantage that they can be tested for
  exact equality on all architectures.
*/

struct fontmap {
    char *texname;
    int enc;	/* Index in encoding array */
    int extension;	/* Fixed point, *1000 */
    int slant;	/* Fixed point, *10000, some font slantings have 4
		   significant digits, all after the decimalpoint */
    char *filename;	/* Name of the t1 font file as given in map/dvi */

    int t1libid;	/* The t1lib id of this font, or -1 if not set up */
    char *pathname;	/* Full path of the font, once needed and located */
    int tfmidx;	/* Index in tfminfo array, or -1 */
    Boolean warned_about;	/* whether a warning message about this font has been displayed yet */
    /*
      if the font file was not readable for some reason (e.g. corrupt,
      or encrypted for proprietry distribution), load_font_now() will
      try to load a PK version via load_font() instead. If this is
      successful, it will set force_pk to true, which will tell
      get_t1_glyph() to return immediately, and just use the standard
      set_char() routine to set the PK glyph. If even that fails,
      get_t1_glyph() returns an error status in a special flag that's
      evaluated by set_t1_char().
    */
    Boolean force_pk;		
};

static struct fontmap *fontmaps = NULL;
static size_t g_maplidx = 0; /* global, for communication with find_texfont */

/*
  widthinfo[n]: TFM width info, loaded on demand as the fonts are used.
  idx	 texname	widths
  [0] = cmr10		...
  [1] = ptmr8r		...
  [2] = pcrr8rn	...
  [3] = putro8r	...

  The widthinfo is loaded from the font tfm file because the type1
  width information isn't precise enough to avoid accumulating
  rounding errors that make things visibly misaligned.  This is
  esp. noticeable in tables (for the lines separating the columns).

  For other than "design" sizes the widths are scaled up or down.

*/

struct tfminfos {
    const char *texname;
    long designsize;
    long widths[256];
    long fontdimen2; /* width of word spacing in this font */
};

static struct tfminfos *tfminfo = NULL;

/* For file reading */
# define BUFFER_SIZE 1024

/* This is the conversion factor from TeXs pt to TeXs bp (big point).
   Maximum machine precision is needed to keep it accurate. */
/* UNUSED */
/* static double bp_fac=72.0/72.27; */

/* Try to convert from PS charspace units to DVI units with minimal
   loss of significant digits */
# define t1_dvi_conv(x)	((((int) (x)) << 16)/1000)

/* Convert from TFM units to DVI units */
# define tfm_dvi_conv(x) (((int) (x)) >> 1)

static Boolean padMismatch = False; /* Does t1lib padding match xdvi required padding? */
static int archpad;
# define T1PAD(bits, pad)  (((bits) + (pad) - 1) & -(pad))

static struct glyph *get_t1_glyph(
# ifdef TEXXET
				  wide_ubyte cmd,
# endif
				  wide_ubyte ch, t1FontLoadStatusT *flag,
				  Boolean is_geom_scan);

# ifdef WORDS_BIGENDIAN
unsigned char rbits[] = {
    0x00,   /* 00000000 -> 00000000 */
    0x80,   /* 00000001 -> 10000000 */
    0x40,   /* 00000010 -> 01000000 */
    0xC0,   /* 00000011 -> 11000000 */
    0x20,   /* 00000100 -> 00100000 */
    0xA0,   /* 00000101 -> 10100000 */
    0x60,   /* 00000110 -> 01100000 */
    0xE0,   /* 00000111 -> 11100000 */
    0x10,   /* 00001000 -> 00010000 */
    0x90,   /* 00001001 -> 10010000 */
    0x50,   /* 00001010 -> 01010000 */
    0xD0,   /* 00001011 -> 11010000 */
    0x30,   /* 00001100 -> 00110000 */
    0xB0,   /* 00001101 -> 10110000 */
    0x70,   /* 00001110 -> 01110000 */
    0xF0,   /* 00001111 -> 11110000 */
    0x08,   /* 00010000 -> 00001000 */
    0x88,   /* 00010001 -> 10001000 */
    0x48,   /* 00010010 -> 01001000 */
    0xC8,   /* 00010011 -> 11001000 */
    0x28,   /* 00010100 -> 00101000 */
    0xA8,   /* 00010101 -> 10101000 */
    0x68,   /* 00010110 -> 01101000 */
    0xE8,   /* 00010111 -> 11101000 */
    0x18,   /* 00011000 -> 00011000 */
    0x98,   /* 00011001 -> 10011000 */
    0x58,   /* 00011010 -> 01011000 */
    0xD8,   /* 00011011 -> 11011000 */
    0x38,   /* 00011100 -> 00111000 */
    0xB8,   /* 00011101 -> 10111000 */
    0x78,   /* 00011110 -> 01111000 */
    0xF8,   /* 00011111 -> 11111000 */
    0x04,   /* 00100000 -> 00000100 */
    0x84,   /* 00100001 -> 10000100 */
    0x44,   /* 00100010 -> 01000100 */
    0xC4,   /* 00100011 -> 11000100 */
    0x24,   /* 00100100 -> 00100100 */
    0xA4,   /* 00100101 -> 10100100 */
    0x64,   /* 00100110 -> 01100100 */
    0xE4,   /* 00100111 -> 11100100 */
    0x14,   /* 00101000 -> 00010100 */
    0x94,   /* 00101001 -> 10010100 */
    0x54,   /* 00101010 -> 01010100 */
    0xD4,   /* 00101011 -> 11010100 */
    0x34,   /* 00101100 -> 00110100 */
    0xB4,   /* 00101101 -> 10110100 */
    0x74,   /* 00101110 -> 01110100 */
    0xF4,   /* 00101111 -> 11110100 */
    0x0C,   /* 00110000 -> 00001100 */
    0x8C,   /* 00110001 -> 10001100 */
    0x4C,   /* 00110010 -> 01001100 */
    0xCC,   /* 00110011 -> 11001100 */
    0x2C,   /* 00110100 -> 00101100 */
    0xAC,   /* 00110101 -> 10101100 */
    0x6C,   /* 00110110 -> 01101100 */
    0xEC,   /* 00110111 -> 11101100 */
    0x1C,   /* 00111000 -> 00011100 */
    0x9C,   /* 00111001 -> 10011100 */
    0x5C,   /* 00111010 -> 01011100 */
    0xDC,   /* 00111011 -> 11011100 */
    0x3C,   /* 00111100 -> 00111100 */
    0xBC,   /* 00111101 -> 10111100 */
    0x7C,   /* 00111110 -> 01111100 */
    0xFC,   /* 00111111 -> 11111100 */
    0x02,   /* 01000000 -> 00000010 */
    0x82,   /* 01000001 -> 10000010 */
    0x42,   /* 01000010 -> 01000010 */
    0xC2,   /* 01000011 -> 11000010 */
    0x22,   /* 01000100 -> 00100010 */
    0xA2,   /* 01000101 -> 10100010 */
    0x62,   /* 01000110 -> 01100010 */
    0xE2,   /* 01000111 -> 11100010 */
    0x12,   /* 01001000 -> 00010010 */
    0x92,   /* 01001001 -> 10010010 */
    0x52,   /* 01001010 -> 01010010 */
    0xD2,   /* 01001011 -> 11010010 */
    0x32,   /* 01001100 -> 00110010 */
    0xB2,   /* 01001101 -> 10110010 */
    0x72,   /* 01001110 -> 01110010 */
    0xF2,   /* 01001111 -> 11110010 */
    0x0A,   /* 01010000 -> 00001010 */
    0x8A,   /* 01010001 -> 10001010 */
    0x4A,   /* 01010010 -> 01001010 */
    0xCA,   /* 01010011 -> 11001010 */
    0x2A,   /* 01010100 -> 00101010 */
    0xAA,   /* 01010101 -> 10101010 */
    0x6A,   /* 01010110 -> 01101010 */
    0xEA,   /* 01010111 -> 11101010 */
    0x1A,   /* 01011000 -> 00011010 */
    0x9A,   /* 01011001 -> 10011010 */
    0x5A,   /* 01011010 -> 01011010 */
    0xDA,   /* 01011011 -> 11011010 */
    0x3A,   /* 01011100 -> 00111010 */
    0xBA,   /* 01011101 -> 10111010 */
    0x7A,   /* 01011110 -> 01111010 */
    0xFA,   /* 01011111 -> 11111010 */
    0x06,   /* 01100000 -> 00000110 */
    0x86,   /* 01100001 -> 10000110 */
    0x46,   /* 01100010 -> 01000110 */
    0xC6,   /* 01100011 -> 11000110 */
    0x26,   /* 01100100 -> 00100110 */
    0xA6,   /* 01100101 -> 10100110 */
    0x66,   /* 01100110 -> 01100110 */
    0xE6,   /* 01100111 -> 11100110 */
    0x16,   /* 01101000 -> 00010110 */
    0x96,   /* 01101001 -> 10010110 */
    0x56,   /* 01101010 -> 01010110 */
    0xD6,   /* 01101011 -> 11010110 */
    0x36,   /* 01101100 -> 00110110 */
    0xB6,   /* 01101101 -> 10110110 */
    0x76,   /* 01101110 -> 01110110 */
    0xF6,   /* 01101111 -> 11110110 */
    0x0E,   /* 01110000 -> 00001110 */
    0x8E,   /* 01110001 -> 10001110 */
    0x4E,   /* 01110010 -> 01001110 */
    0xCE,   /* 01110011 -> 11001110 */
    0x2E,   /* 01110100 -> 00101110 */
    0xAE,   /* 01110101 -> 10101110 */
    0x6E,   /* 01110110 -> 01101110 */
    0xEE,   /* 01110111 -> 11101110 */
    0x1E,   /* 01111000 -> 00011110 */
    0x9E,   /* 01111001 -> 10011110 */
    0x5E,   /* 01111010 -> 01011110 */
    0xDE,   /* 01111011 -> 11011110 */
    0x3E,   /* 01111100 -> 00111110 */
    0xBE,   /* 01111101 -> 10111110 */
    0x7E,   /* 01111110 -> 01111110 */
    0xFE,   /* 01111111 -> 11111110 */
    0x01,   /* 10000000 -> 00000001 */
    0x81,   /* 10000001 -> 10000001 */
    0x41,   /* 10000010 -> 01000001 */
    0xC1,   /* 10000011 -> 11000001 */
    0x21,   /* 10000100 -> 00100001 */
    0xA1,   /* 10000101 -> 10100001 */
    0x61,   /* 10000110 -> 01100001 */
    0xE1,   /* 10000111 -> 11100001 */
    0x11,   /* 10001000 -> 00010001 */
    0x91,   /* 10001001 -> 10010001 */
    0x51,   /* 10001010 -> 01010001 */
    0xD1,   /* 10001011 -> 11010001 */
    0x31,   /* 10001100 -> 00110001 */
    0xB1,   /* 10001101 -> 10110001 */
    0x71,   /* 10001110 -> 01110001 */
    0xF1,   /* 10001111 -> 11110001 */
    0x09,   /* 10010000 -> 00001001 */
    0x89,   /* 10010001 -> 10001001 */
    0x49,   /* 10010010 -> 01001001 */
    0xC9,   /* 10010011 -> 11001001 */
    0x29,   /* 10010100 -> 00101001 */
    0xA9,   /* 10010101 -> 10101001 */
    0x69,   /* 10010110 -> 01101001 */
    0xE9,   /* 10010111 -> 11101001 */
    0x19,   /* 10011000 -> 00011001 */
    0x99,   /* 10011001 -> 10011001 */
    0x59,   /* 10011010 -> 01011001 */
    0xD9,   /* 10011011 -> 11011001 */
    0x39,   /* 10011100 -> 00111001 */
    0xB9,   /* 10011101 -> 10111001 */
    0x79,   /* 10011110 -> 01111001 */
    0xF9,   /* 10011111 -> 11111001 */
    0x05,   /* 10100000 -> 00000101 */
    0x85,   /* 10100001 -> 10000101 */
    0x45,   /* 10100010 -> 01000101 */
    0xC5,   /* 10100011 -> 11000101 */
    0x25,   /* 10100100 -> 00100101 */
    0xA5,   /* 10100101 -> 10100101 */
    0x65,   /* 10100110 -> 01100101 */
    0xE5,   /* 10100111 -> 11100101 */
    0x15,   /* 10101000 -> 00010101 */
    0x95,   /* 10101001 -> 10010101 */
    0x55,   /* 10101010 -> 01010101 */
    0xD5,   /* 10101011 -> 11010101 */
    0x35,   /* 10101100 -> 00110101 */
    0xB5,   /* 10101101 -> 10110101 */
    0x75,   /* 10101110 -> 01110101 */
    0xF5,   /* 10101111 -> 11110101 */
    0x0D,   /* 10110000 -> 00001101 */
    0x8D,   /* 10110001 -> 10001101 */
    0x4D,   /* 10110010 -> 01001101 */
    0xCD,   /* 10110011 -> 11001101 */
    0x2D,   /* 10110100 -> 00101101 */
    0xAD,   /* 10110101 -> 10101101 */
    0x6D,   /* 10110110 -> 01101101 */
    0xED,   /* 10110111 -> 11101101 */
    0x1D,   /* 10111000 -> 00011101 */
    0x9D,   /* 10111001 -> 10011101 */
    0x5D,   /* 10111010 -> 01011101 */
    0xDD,   /* 10111011 -> 11011101 */
    0x3D,   /* 10111100 -> 00111101 */
    0xBD,   /* 10111101 -> 10111101 */
    0x7D,   /* 10111110 -> 01111101 */
    0xFD,   /* 10111111 -> 11111101 */
    0x03,   /* 11000000 -> 00000011 */
    0x83,   /* 11000001 -> 10000011 */
    0x43,   /* 11000010 -> 01000011 */
    0xC3,   /* 11000011 -> 11000011 */
    0x23,   /* 11000100 -> 00100011 */
    0xA3,   /* 11000101 -> 10100011 */
    0x63,   /* 11000110 -> 01100011 */
    0xE3,   /* 11000111 -> 11100011 */
    0x13,   /* 11001000 -> 00010011 */
    0x93,   /* 11001001 -> 10010011 */
    0x53,   /* 11001010 -> 01010011 */
    0xD3,   /* 11001011 -> 11010011 */
    0x33,   /* 11001100 -> 00110011 */
    0xB3,   /* 11001101 -> 10110011 */
    0x73,   /* 11001110 -> 01110011 */
    0xF3,   /* 11001111 -> 11110011 */
    0x0B,   /* 11010000 -> 00001011 */
    0x8B,   /* 11010001 -> 10001011 */
    0x4B,   /* 11010010 -> 01001011 */
    0xCB,   /* 11010011 -> 11001011 */
    0x2B,   /* 11010100 -> 00101011 */
    0xAB,   /* 11010101 -> 10101011 */
    0x6B,   /* 11010110 -> 01101011 */
    0xEB,   /* 11010111 -> 11101011 */
    0x1B,   /* 11011000 -> 00011011 */
    0x9B,   /* 11011001 -> 10011011 */
    0x5B,   /* 11011010 -> 01011011 */
    0xDB,   /* 11011011 -> 11011011 */
    0x3B,   /* 11011100 -> 00111011 */
    0xBB,   /* 11011101 -> 10111011 */
    0x7B,   /* 11011110 -> 01111011 */
    0xFB,   /* 11011111 -> 11111011 */
    0x07,   /* 11100000 -> 00000111 */
    0x87,   /* 11100001 -> 10000111 */
    0x47,   /* 11100010 -> 01000111 */
    0xC7,   /* 11100011 -> 11000111 */
    0x27,   /* 11100100 -> 00100111 */
    0xA7,   /* 11100101 -> 10100111 */
    0x67,   /* 11100110 -> 01100111 */
    0xE7,   /* 11100111 -> 11100111 */
    0x17,   /* 11101000 -> 00010111 */
    0x97,   /* 11101001 -> 10010111 */
    0x57,   /* 11101010 -> 01010111 */
    0xD7,   /* 11101011 -> 11010111 */
    0x37,   /* 11101100 -> 00110111 */
    0xB7,   /* 11101101 -> 10110111 */
    0x77,   /* 11101110 -> 01110111 */
    0xF7,   /* 11101111 -> 11110111 */
    0x0F,   /* 11110000 -> 00001111 */
    0x8F,   /* 11110001 -> 10001111 */
    0x4F,   /* 11110010 -> 01001111 */
    0xCF,   /* 11110011 -> 11001111 */
    0x2F,   /* 11110100 -> 00101111 */
    0xAF,   /* 11110101 -> 10101111 */
    0x6F,   /* 11110110 -> 01101111 */
    0xEF,   /* 11110111 -> 11101111 */
    0x1F,   /* 11111000 -> 00011111 */
    0x9F,   /* 11111001 -> 10011111 */
    0x5F,   /* 11111010 -> 01011111 */
    0xDF,   /* 11111011 -> 11011111 */
    0x3F,   /* 11111100 -> 00111111 */
    0xBF,   /* 11111101 -> 10111111 */
    0x7F,   /* 11111110 -> 01111111 */
    0xFF    /* 11111111 -> 11111111 */
};
# endif /* WORDS_BIGENDIAN */
#endif /* T1LIB */

static struct frame frame0;	/* dummy head of list */
#ifdef	TEXXET
static struct frame *scan_frame;	/* head frame for scanning */
#endif

static const char *const reverse_search_helptext =
"Forward/reverse search allows you to jump from a point "
"in the DVI file to the corresponding location in the .tex source file, and vice versa. "
"To make this possible, the .tex file needs to be compiled with source special support. "
"This can be done by using either a package like \"srcltx.sty\" or \"srctex.sty\", "
"or a command-line switch like \"-src\" for the TeX executable. "
"See the xdvi man page (section SOURCE SPECIALS) "
"for more information about this.";

static ubyte dvi_buffer[DVI_BUFFER_LEN];
ubyte *G_dvi_buf_ptr = dvi_buffer;
static struct frame *current_frame;

/* Points to drawinf record containing current dvi file location (for update by
   geom_scan).  */
static	struct drawinf	*dvi_pointer_frame = NULL;

#ifdef TEXXET
#define	DIR	currinf.dir
#else
#define	DIR	1
#endif

/*
 *	Explanation of the following constant:
 *	offset_[xy]   << 16:	margin (defaults to one inch)
 *	currwin.shrinkfactor << 16:	one pixel page border
 *	currwin.shrinkfactor << 15:	rounding for pixel_conv
 */
#define OFFSET_X	(resource.xoffset_int << 16) + (currwin.shrinkfactor * 3 << 15)
#define OFFSET_Y	(resource.yoffset_int << 16) + (currwin.shrinkfactor * 3 << 15)

#if (BMBYTES == 1)
bmUnitT bit_masks[] = {
    0x0, 0x1, 0x3, 0x7,
    0xf, 0x1f, 0x3f, 0x7f,
    0xff
};
#else
#if (BMBYTES == 2)
bmUnitT bit_masks[] = {
    0x0, 0x1, 0x3, 0x7,
    0xf, 0x1f, 0x3f, 0x7f,
    0xff, 0x1ff, 0x3ff, 0x7ff,
    0xfff, 0x1fff, 0x3fff, 0x7fff,
    0xffff
};
#else /* BMBYTES == 4 */
bmUnitT bit_masks[] = {
    0x0, 0x1, 0x3, 0x7,
    0xf, 0x1f, 0x3f, 0x7f,
    0xff, 0x1ff, 0x3ff, 0x7ff,
    0xfff, 0x1fff, 0x3fff, 0x7fff,
    0xffff, 0x1ffff, 0x3ffff, 0x7ffff,
    0xfffff, 0x1fffff, 0x3fffff, 0x7fffff,
    0xffffff, 0x1ffffff, 0x3ffffff, 0x7ffffff,
    0xfffffff, 0x1fffffff, 0x3fffffff, 0x7fffffff,
    0xffffffff
};
#endif
#endif

#ifdef	VMS
#define	off_t	int
#endif
extern off_t lseek();

#ifndef	SEEK_SET	/* if <unistd.h> is not provided (or for <X11R5) */
#define	SEEK_SET	0
#define	SEEK_CUR	1
#define	SEEK_END	2
#endif

static void draw_part(FILE *fp, struct frame *minframe, double current_dimconv);

static void source_fwd_draw_box(void);



/*
 *	X routines.
 */

/*
 *	Put a rectangle on the screen.
 */
static void
put_rule(int x, int y, unsigned int w, unsigned int h)
{
    if (htex_inside_href)
	htex_record_position(x, y, w, h);
    if (x < globals.win_expose.max_x && x + (int)w >= globals.win_expose.min_x && y < globals.win_expose.max_y && y + (int)h >= globals.win_expose.min_y) {
	if (--globals.ev.ctr == 0) {
	    if (read_events(EV_NOWAIT) & EV_GE_MAG_GONE) {
		/* 		fprintf(stderr, "longjmp1!\n"); */
		longjmp(globals.ev.canit, 1);
	    }
	}
#if COLOR
	if (fg_active != fg_current)
	    do_color_change();
#endif
	XFillRectangle(DISP, currwin.win,
		       globals.gc.rule,
		       x - currwin.base_x,
		       y - currwin.base_y,
		       w ? w : 1,
		       h ? h : 1);
    }
}

static void
put_bitmap(struct bitmap *bitmap, int x, int y)
{
    if (globals.debug & DBG_BITMAP)
	printf("X(%d,%d)\n", x - currwin.base_x, y - currwin.base_y);
    if (htex_inside_href)
	htex_record_position(x, y, bitmap->w, bitmap->h);
    if (x < globals.win_expose.max_x && x + (int)bitmap->w >= globals.win_expose.min_x &&
	y < globals.win_expose.max_y && y + (int)bitmap->h >= globals.win_expose.min_y) {
	if (--globals.ev.ctr == 0)
	    if (read_events(EV_NOWAIT) & EV_GE_MAG_GONE) {
		/* 		fprintf(stderr, "longjmp2!\n"); */
		longjmp(globals.ev.canit, 1);
	    }
#if COLOR
	if (fg_active != fg_current)
	    do_color_change();
#endif
	G_image->width = bitmap->w;
	G_image->height = bitmap->h;
	G_image->data = bitmap->bits;
	G_image->bytes_per_line = bitmap->bytes_wide;
	XPutImage(DISP, currwin.win, globals.gc.fore, G_image,
		  0, 0,
		  x - currwin.base_x, y - currwin.base_y, bitmap->w, bitmap->h);
	if (globals.gc.fore2) {
	    XPutImage(DISP, currwin.win, globals.gc.fore2, G_image,
		      0, 0,
		      x - currwin.base_x, y - currwin.base_y,
		      bitmap->w, bitmap->h);
	}
    }
}

#if GREY

/* Anti-aliasing stuff.
   The method used here is supersampling of the unshrunk glyph (this
   also means that no anti-aliasing happens at shrink 1). A sample
   of the number of bits that are `on' in the unsrunk glyph determines
   the grey level of the shrunk image.
*/

/* Pixel lookup tables for anti-aliasing: These store all possible
   supersampling values (i.e. number of bits set in the unshrunk
   image) for the given shrink factor. E.g. at shrink level 2, the
   size of the pixel table is 4; in other words, 1 pixel in the shrunk
   image corresponds to 4 pixels in the unshrunk image. Thus, the possible
   values for shink level 2 are: black, 1/4 (0x404040), 1/2 (0x808080),
   3/4 (0xc0c0c0) and white (0xffffff).
*/
static Pixel *pixeltbl;
static Pixel *pixeltbl_gc2; /* drawing to globals.gc.fore2 (compare pixmap2_gc2) */

static void shrink_glyph_grey(struct glyph *);

static void
put_image(struct glyph *g, int x, int y)
{
    XImage *img = g->image2;

    if (htex_inside_href)
	htex_record_position(x, y, img->width, img->height);
    if (x < globals.win_expose.max_x && x + img->width >= globals.win_expose.min_x &&
	y < globals.win_expose.max_y && y + img->height >= globals.win_expose.min_y) {
	if (--globals.ev.ctr == 0)
	    if (read_events(EV_NOWAIT) & EV_GE_MAG_GONE) {
		/* 		fprintf(stderr, "longjmp3!\n"); */
		longjmp(globals.ev.canit, 1);
	    }

#if COLOR
	if (g->fg != fg_current)	/* if color change since last use */
	    shrink_glyph_grey(g);
	else if (fg_active != fg_current)	/* if GCs need updating */
	    do_color_change();
#endif
	/* TODO: Can we increase gamma locally to make the inverted text more readable?
	   
	and to draw the background, so something like this:
	XFillRectangle(DISP, currwin.win, globals.gc.fore, x - currwin.base_x, y - currwin.base_y,
	(unsigned int)img->width * 2, (unsigned int)img->height * 2);
	   
	test this with color changes!!
	*/
	/*TEST_DELAY("check point 1 ...")*/
	XPutImage(DISP, currwin.win, globals.gc.fore, img,
		  0, 0,
		  x - currwin.base_x, y - currwin.base_y,
		  (unsigned int)img->width, (unsigned int)img->height);
	/*TEST_DELAY("check point 2 ...")*/

	if (globals.gc.fore2 != NULL) {
	    img->data = g->pixmap2_gc2;
	    XPutImage(DISP, currwin.win, globals.gc.fore2, img,
		      0, 0,
		      x - currwin.base_x, y - currwin.base_y,
		      (unsigned int)img->width, (unsigned int)img->height);
	    img->data = g->pixmap2;
	}
	/*TEST_DELAY("check point 3 ...")*/
    }
}
#endif /* GREY */

/*
 *	Draw the border of a rectangle on the screen.
 *	This should be replaced by a grey background both in Xaw and Motif
 *	as soon as #470325 is fixed (see also FIXED_FLUSHING_PAGING).
 */
static void
draw_border(int x, int y, unsigned int width, unsigned int height, GC ourGC)
{
    --width;
    --height;
    XDrawRectangle(DISP, currwin.win, ourGC, x, y, width, height);
}

/* draw the grid */
static void
put_grid(int x, int y,
	 unsigned int width, unsigned int height, unsigned int unit,
	 GC gc)
{
    int i;
    float sep;
    unsigned int tmp;
    
    --width;
    --height;
    
    
    /* draw vertial grid */
#define DRAWGRID_VER(gc) for (i = 1; \
			      (tmp = x + (int)(i * sep)) < x + width; \
			      i++) \
			  XFillRectangle(DISP, currwin.win, (gc), \
				   tmp, y, 1, height)
    /* draw horizontal grid */
#define DRAWGRID_HOR(gc) for (i = 1; \
			      (tmp = y + (int)(i * sep)) < y + height; \
			      i++) \
		          XFillRectangle(DISP, currwin.win, (gc), \
				   x, tmp, width, 1)

    if (resource.grid_mode > 2) {	/* third level grid */
	sep = (float)unit / 4.0;
	DRAWGRID_VER(gc);
	DRAWGRID_HOR(gc);
    }

    if (resource.grid_mode > 1) {	/* second level grid */
	sep = (float)unit / 2.0;
	DRAWGRID_VER(gc);
	DRAWGRID_HOR(gc);
    }

    if (resource.grid_mode > 0) {	/* first level grid */
	sep = (float)unit;
	DRAWGRID_VER(gc);
	DRAWGRID_HOR(gc);
    }
}
#undef DRAWGRID_VER
#undef DRAWGRID_HOR

/*
 *	Byte reading routines for dvi file.
 */

#define	xtell(fp, pos)	(lseek(fileno(fp), 0L, SEEK_CUR) - \
			    (currinf.end - (pos)))

static ubyte
xxone(FILE *fp)
{
    if (currinf.virtual) {
	++currinf.pos;
	return EOP;
    }

    currinf.end = G_dvi_buf_ptr
	+ read(fileno(fp), (char *)(currinf.pos = G_dvi_buf_ptr), DVI_BUFFER_LEN);
    return currinf.end > G_dvi_buf_ptr ? *(currinf.pos)++ : EOF;
}

#define	xone(fp)  (currinf.pos < currinf.end ? *(currinf.pos)++ : xxone(fp))

static unsigned long
xnum(FILE *fp, ubyte size)
{
    long x = 0;

    while (size--)
	x = (x << 8) | xone(fp);
    return x;
}

static long
xsnum(FILE *fp, ubyte size)
{
    long x;

#if	__STDC__
    x = (signed char)xone(fp);
#else
    x = xone(fp);
    if (x & 0x80)
	x -= 0x100;
#endif
    while (--size)
	x = (x << 8) | xone(fp);
    return x;
}

#define	xsfour(fp)	xsnum(fp, 4)

static void
xskip(FILE *fp, long offset)
{
    currinf.pos += offset;
    if (!currinf.virtual && currinf.pos > currinf.end)
	(void)lseek(fileno(fp), (long)(currinf.pos - currinf.end), SEEK_CUR);
}

void
dvi_fmt_error(const char *message, ...)
{
    
    va_list args;
    va_start(args, message);
    fprintf(stderr, "%s: ", globals.program_name);
    (void)vfprintf(stderr, message, args);
    va_end(args);
    if (currinf.virtual)
	fprintf(stderr, " in virtual font %s\n", currinf.virtual->fontname);
    else
	fprintf(stderr, ", offset %ld\n", (long)xtell(globals.dvi_file.bak_fp, currinf.pos - 1));
    /* #ifndef NDEBUG */
    /*     xdvi_exit(EXIT_FAILURE); */
    /* #else */
    XDVI_ABORT((stderr, "I'll abort now, to help you debugging this."));
    /* #endif */
}


/*
 *	Code for debugging options.
 */

static void
print_bitmap(struct bitmap *bitmap)
{
    bmUnitT *ptr = (bmUnitT *)bitmap->bits;
    int x, y, i;

    ASSERT(ptr != NULL, "Invalid bitmap bits");

    printf("w = %d, h = %d, bytes wide = %d\n",
	   bitmap->w, bitmap->h, bitmap->bytes_wide);
    for (y = 0; y < (int)bitmap->h; ++y) {
	for (x = bitmap->bytes_wide; x > 0; x -= BMBYTES) {
#ifdef WORDS_BIGENDIAN
	    for (i = BMBITS - 1; i >= 0; --i)
		putchar((*ptr & (1 << i)) ? '@' : '.');
#else
	    for (i = 0; i < BMBITS; ++i)
		putchar((*ptr & (1 << i)) ? '@' : '.');
#endif
	    ++ptr;
	}
	putchar('\n');
    }
}

static void
print_char(ubyte ch, struct glyph *g)
{
    printf("char %d", ch);
    if (isprint(ch))
	printf(" (%c)", ch);
    putchar('\n');
    printf("x = %d, y = %d, dvi = %ld\n", g->x, g->y, g->dvi_adv);
    print_bitmap(&g->bitmap);
}

static const char *dvi_table1[] = {
    "SET1", "SET2", NULL, NULL, "SETRULE", "PUT1", "PUT2", NULL,
    NULL, "PUTRULE", "NOP", "BOP", "EOP", "PUSH", "POP", "RIGHT1",
    "RIGHT2", "RIGHT3", "RIGHT4", "W0", "W1", "W2", "W3", "W4",
    "X0", "X1", "X2", "X3", "X4", "DOWN1", "DOWN2", "DOWN3",
    "DOWN4", "Y0", "Y1", "Y2", "Y3", "Y4", "Z0", "Z1",
    "Z2", "Z3", "Z4"
};

static const char *dvi_table2[] = {
    "FNT1", "FNT2", "FNT3", "FNT4", "XXX1", "XXX2", "XXX3", "XXX4",
    "FNTDEF1", "FNTDEF2", "FNTDEF3", "FNTDEF4", "PRE", "POST", "POSTPOST",
    "SREFL", "EREFL", NULL, NULL, NULL, NULL
};

static void
print_dvi(ubyte ch)
{
    const char *s;

    fprintf(stderr, "%4d %4d ", PXL_H, PXL_V);
    if (ch <= (ubyte) (SETCHAR0 + 127)) {
	fprintf(stderr, "SETCHAR%-3d", ch - SETCHAR0);
	if (isprint(ch))
	    fprintf(stderr, " (%c)", ch);
	fputc('\n', stderr);
	return;
    }
    else if (ch < FNTNUM0)
	s = dvi_table1[ch - 128];
    else if (ch <= (ubyte) (FNTNUM0 + 63)) {
	fprintf(stderr, "FNTNUM%d\n", ch - FNTNUM0);
	return;
    }
    else
	s = dvi_table2[ch - (FNTNUM0 + 64)];
    if (s) {
	fputs(s, stderr);
	fputc('\n', stderr);
    }
    else
	XDVI_FATAL((stderr, "print_dvi: unknown op-code %d", ch));
}


/*
 *	Count the number of set bits in a 4x4-region of the bitmap
 */

static char sample_count[] = {
    0, 1, 1, 2,
    1, 2, 2, 3,
    1, 2, 2, 3,
    2, 3, 3, 4
};

/*
 * For greyscaling, return the number of bits that are set in a given region
 * of width w and height h of the bitmap `bits', starting horizontally after
 * `bit_skip' bits, where `bytes_wide' is the same as the `bytes_wide' field
 * in the bitmap struct (scan line width in bytes).
 * Note that `bits' is really a one-dimensional array, i.e. all the rows
 * are put in sequence into one single row.
 */
static int
sample(bmUnitT *bits, int bytes_wide, int bit_skip, int w, int h)
{
    bmUnitT *beg_ptr, *end_ptr, *curr_ptr;
    int bits_left;
    int n, bit_shift, wid;

#if TRACE_AA
    fprintf(stderr, "sample: %d bytes wide, %d skip, %d w, %d h\n", bytes_wide, bit_skip, w, h);
#endif

    beg_ptr = bits + bit_skip / BMBITS;
    end_ptr = ADD(bits, h * bytes_wide);
    
#if TRACE_AA
    fprintf(stderr, "beg_ptr: %p, end: %p\n", (void *)beg_ptr, (void *)end_ptr);
#endif
    
    bits_left = w;
#ifdef	WORDS_BIGENDIAN
    bit_shift = BMBITS - bit_skip % BMBITS;
#else
    bit_shift = bit_skip % BMBITS;
#endif
#if TRACE_AA
    fprintf(stderr, "shift: %d\n", bit_shift);
#endif
    n = 0;
	
    while (bits_left) {
#ifdef	WORDS_BIGENDIAN
	wid = bit_shift;
#else
	wid = BMBITS - bit_shift;
#endif
	if (wid > bits_left)
	    wid = bits_left;
	if (wid > 4) /* why? */
	    wid = 4;
#ifdef	WORDS_BIGENDIAN
	bit_shift -= wid;
#endif
	for (curr_ptr = beg_ptr;
	     curr_ptr < end_ptr;
	     curr_ptr = ADD(curr_ptr, bytes_wide)) {
#if TRACE_AA
	    int i;
	    for (i = 0; i < bytes_wide; i++) {
		fprintf(stderr, "%d ", *(curr_ptr + i));
	    }
	    fprintf(stderr, "\nMask: %d; count: %d\n", wid,
		    sample_count[ (*curr_ptr >> bit_shift) & bit_masks[wid] ]);
#endif
	    n += sample_count[ (*curr_ptr >> bit_shift) & bit_masks[wid] ];
	}
#ifdef	WORDS_BIGENDIAN
	if (bit_shift == 0) {
	    bit_shift = BMBITS;
	    ++beg_ptr;
	}
#else
	bit_shift += wid;
	if (bit_shift == BMBITS) {
	    bit_shift = 0;
	    ++beg_ptr;
	}
#endif
	bits_left -= wid;
	/*  	    fprintf(stderr, "bits_left: %d\n", bits_left); */
    }
    return n;
}

static void
shrink_glyph(struct glyph *g)
{
    int shrunk_bytes_wide, shrunk_height;
    int rows_left, rows, init_cols;
    int cols_left;
    int cols;
    bmUnitT *unshrunk_ptr, *shrunk_ptr;
    bmUnitT m, *cp;
    /* threshold for which a bit will be set to `on' in the shrunken bitmap */
    int min_sample = currwin.shrinkfactor * currwin.shrinkfactor * resource.density / 100;
    int row_num;

    /* These machinations ensure that the character is shrunk according to
       its hot point, rather than its upper left-hand corner. */
    g->x2 = g->x / currwin.shrinkfactor;
    init_cols = g->x - g->x2 * currwin.shrinkfactor;
    if (init_cols <= 0)
	init_cols += currwin.shrinkfactor;
    else
	++g->x2;
    g->bitmap2.w = g->x2 + ROUNDUP((int)g->bitmap.w - g->x, currwin.shrinkfactor);
    /* include row zero with the positively numbered rows */
    row_num = g->y + 1;
    g->y2 = row_num / currwin.shrinkfactor;
    rows = row_num - g->y2 * currwin.shrinkfactor;
    if (rows <= 0) {
	rows += currwin.shrinkfactor;
	--g->y2;
    }
    g->bitmap2.h = shrunk_height
	= g->y2 + ROUNDUP((int)g->bitmap.h - row_num, currwin.shrinkfactor) + 1;
    alloc_bitmap(&g->bitmap2);

    unshrunk_ptr = (bmUnitT *) g->bitmap.bits;
    shrunk_ptr = (bmUnitT *) g->bitmap2.bits;
    shrunk_bytes_wide = g->bitmap2.bytes_wide;
    rows_left = g->bitmap.h;
    memset((char *)shrunk_ptr, '\0', shrunk_bytes_wide * shrunk_height);
    
    while (rows_left) {
	if (rows > rows_left)
	    rows = rows_left;
	cols_left = g->bitmap.w;
#ifndef	WORDS_BIGENDIAN
	m = 1; /* XXX was (1 << 0) */
#else
	m = ((bmUnitT)1 << (BMBITS - 1));
#endif
	cp = shrunk_ptr;
	cols = init_cols;
	while (cols_left) {
	    /* ??? */
	    if (cols > cols_left)
		cols = cols_left;
	    /* set a bit to `on' if it's over the threshold */
	    if (sample(unshrunk_ptr, g->bitmap.bytes_wide,
		       (int)g->bitmap.w - cols_left, cols, rows)
		>= min_sample)
		*cp |= m;
#ifndef	WORDS_BIGENDIAN
	    if (m == ((bmUnitT)1 << (BMBITS - 1))) {
		m = 1; /* XXX was (1 << 0) */
		++cp;
	    }
	    else
		m <<= 1;
#else
	    if (m == 1) { /* XXX was (1 << 0) */
		m = ((bmUnitT)1 << (BMBITS - 1));
		++cp;
	    }
	    else
		m >>= 1;
#endif
	    cols_left -= cols;
	    cols = currwin.shrinkfactor;
	}
	shrunk_ptr += shrunk_bytes_wide / sizeof(bmUnitT);
	unshrunk_ptr += rows * g->bitmap.bytes_wide / sizeof(bmUnitT);
	/* 	*((char **)&shrunk_ptr) += shrunk_bytes_wide; */
	/* 	*((char **)&unshrunk_ptr) += rows * g->bitmap.bytes_wide; */
	rows_left -= rows;
	rows = currwin.shrinkfactor;
    }
    g->y2 = g->y / currwin.shrinkfactor;
    if (globals.debug & DBG_BITMAP)
	print_bitmap(&g->bitmap2);
}

#ifdef	GREY

#ifdef RGB_ANTI_ALIASING
/* void */
/* filter_colors(Pixel *p1, Pixel *p2) */
/* { */
/*     Pixel res1, res2; */

/*     int res1_r = *p1 & G_visual->red_mask; */
/*     int res1_g = *p1 & G_visual->green_mask; */
/*     int res1_b = *p1 & G_visual->blue_mask; */

/*     int res2_r = *p2 & G_visual->red_mask; */
/*     int res2_g = *p2 & G_visual->green_mask; */
/*     int res2_b = *p2 & G_visual->blue_mask; */
/* } */

static void
color_filter_image(struct glyph *g)
{
    int rows = g->bitmap2.h;
    int cols = g->bitmap2.w;
    int i, j, k;
    int arr_size = rows * cols * 3;
    unsigned char *arr = xmalloc(arr_size * sizeof *arr);
    unsigned char *arr2 = xmalloc(arr_size * sizeof *arr2);

    fprintf(stderr, "total: %d\n", arr_size);
    
    for (i = 0, j = 0; j < rows; j++) {
	for (k = 0; k < cols; k++) {
	    Pixel p = XGetPixel(g->image2, k, j);
#if TRACE_AA3
	    fprintf(stderr, "k: %d, j: %d, idx: %d\n", k, j, i);
#endif
	    arr[i] = (p & G_visual->red_mask) >> 16;
	    arr[i + 1] = (p & G_visual->green_mask) >> 8;
	    arr[i + 2] = (p & G_visual->blue_mask);
#if TRACE_AA3
	    fprintf(stderr, "0x%.6lX -> %d, %d, %d\n", p, arr[i], arr[i + 1], arr[i + 2]);
#endif
	    i += 3;
	}
    }

    for (i = 0; i < arr_size; i++) {
	if (i == 0) { /* merge two elems */
	    if (i == arr_size - 1) /* only one elem, do nothing */
		break;
	    arr2[i] = (int)(resource.subpixel_energy[0] * arr[i] +
			    resource.subpixel_energy[1] * arr[i + 1] + 0.5);
	}
	else if (i == arr_size - 1) { /* merge two elems */
	    if (i == 0) /* only one elem, do nothing */
		break;
	    arr2[i] = (int)(resource.subpixel_energy[1] * arr[i - 1] +
			    resource.subpixel_energy[0] * arr[i] + 0.5);
	}
	else { /* merge three elems */
	    arr2[i] = (int)(resource.subpixel_energy[1] * arr[i - 1] +
			    resource.subpixel_energy[0] * arr[i] +
			    resource.subpixel_energy[1] * arr[i + 1] + 0.5);
#if TRACE_AA3
	    fprintf(stderr, "%d, %d, %d -> %d\n", arr[i - 1], arr[i], arr[i + 1], arr2[i]);
#endif
	}
    }

    for (i = 0, j = 0; j < rows; j++) {
	for (k = 0; k < cols; k++) {
	    Pixel p = arr2[i] << 16 | (arr2[i + 1] << 8) | arr2[i + 2];
#if TRACE_AA3
	    fprintf(stderr, "%d, %d, %d -> 0x%.6lX\n", arr2[i], arr2[i + 1], arr2[i + 2], p);
#endif
	    XPutPixel(g->image2, k, j, p);
	    i += 3;
	}
    }
    
    free(arr);
    free(arr2);
}

#endif

static void
shrink_glyph_grey(struct glyph *g)
{
    int rows_left, rows, init_cols;
    int cols_left;
    int cols;
    int x, y;
    long thesample;
    /*     int min_sample = currwin.shrinkfactor * currwin.shrinkfactor * resource.density / 100; */
    Pixel onoff, onoff2;
    bmUnitT *unshrunk_ptr;
    unsigned int size;
    int row_num;
    int actual_w;

#if COLOR
    if (fg_active != fg_current) {
	do_color_change();
    }
#endif
    if (pixeltbl == NULL) { /* fix #1611508 (segfault when starting with -nogrey) */
	do_color_change();
    }

    /* TODO: rounding errors causing color fringing (see HACK comment below):
       
    \documentclass{article}
    \pagestyle{empty}

    \begin{document}
    l
    \end{document}

    With ./xdvi-xaw.bin -name xdvi -subpixel rgb -s 4 ./test.dvi:

    subpixel order: rgb = 1
    g->x2: 0, init_cols: -2
    AFTER: g->x2: 0, init_cols: 2

    but with ./xdvi-xaw.bin -name xdvi -s 4 ./test.dvi:

    g->x2: 0, init_cols: -3
    AFTER: g->x2: 0, init_cols: 1
    */
    
    /* These machinations ensure that the character is shrunk according to
       its hot point, rather than its upper left-hand corner. */
#ifdef RGB_ANTI_ALIASING
    if (resource.subpixel_order == SUBPIXEL_NONE)
	g->x2 = g->x / currwin.shrinkfactor;
    else {
	if (g->x < 0)
	    g->x2 = (int)(g->x / 3.0 - 0.5) / currwin.shrinkfactor;
	else
	    g->x2 = (int)(g->x / 3.0 + 0.5) / currwin.shrinkfactor;
    }
    
    if (resource.subpixel_order == SUBPIXEL_NONE)
	init_cols = g->x - g->x2 * currwin.shrinkfactor;
    else {
	if (g->x < 0)
	    init_cols = (int)(g->x / 3.0 - 0.5) - g->x2 * currwin.shrinkfactor;
	else
	    init_cols = (int)(g->x / 3.0 + 0.5) - g->x2 * currwin.shrinkfactor;
	fprintf(stderr, "g->x: %d, g->x2: %d, init_cols: %d\n", g->x, g->x2, init_cols);
    }
#else
    g->x2 = g->x / currwin.shrinkfactor;
    init_cols = g->x - g->x2 * currwin.shrinkfactor;
#endif
    
    if (init_cols <= 0)
	init_cols += currwin.shrinkfactor;
    else
	++(g->x2);

#ifdef RGB_ANTI_ALIASING
    
    if (resource.subpixel_order == SUBPIXEL_NONE)
	g->bitmap2.w = g->x2 + ROUNDUP((int)g->bitmap.w - g->x, currwin.shrinkfactor);
    else {
	fprintf(stderr, "AFTER: g->x2: %d, init_cols: %d\n", g->x2, init_cols);
	/* 	fprintf(stderr, "g->bitmap.w / 3.0 + 0.5: %d; g->x / 3.0: %d; all: %d, %d\n", */
	/* 		(int)(g->bitmap.w / 3.0 + 0.5), */
	/* 		(int)(g->x / 3.0), */
	/* 		(int)((g->bitmap.w / 3.0 + 0.5) - g->x / 3.0), */
	/* 		g->x2 + (int)((((g->bitmap.w / 3.0 + 0.5) - g->x / 3.0) + currwin.shrinkfactor - 1) / currwin.shrinkfactor)); */
	g->bitmap2.w = g->x2 + ROUNDUP((int)((g->bitmap.w / 3.0 + 0.5) - g->x / 3.0), currwin.shrinkfactor);
	/* 	fprintf(stderr, "g->bitmap.w: %d\n", g->bitmap2.w); */
    }
#else
    g->bitmap2.w = g->x2 + ROUNDUP((int)g->bitmap.w - g->x, currwin.shrinkfactor);
#endif

    /* include row zero with the positively numbered rows */
    row_num = g->y + 1;
#ifdef DBG_AA
    fprintf(stderr, "row_num: %d\n", row_num);
#endif /* DBG_AA */
    /* g->y2 is the new height of the image: */
    g->y2 = row_num / currwin.shrinkfactor;
#ifdef DBG_AA
    fprintf(stderr, "g->y2: %d\n", g->y2);
#endif /* DBG_AA */
    /* in C89 and before, result of division can `truncate towards negative infinity'
       (i.e., round to the larger digit) for negative quotients, hence we need
       to test for rows < 0. OTOH, if rows = 0, use currwin.shrinkfactor instead
       (why?)
    */
    if ((rows = row_num - g->y2 * currwin.shrinkfactor) <= 0) {
	rows += currwin.shrinkfactor;
	--(g->y2);
    }
    g->bitmap2.h = g->y2 + ROUNDUP((int)g->bitmap.h - row_num, currwin.shrinkfactor) + 1;

#if TRACE_AA1
    if (resource.subpixel_order != SUBPIXEL_NONE) {
	fprintf(stderr, "\nbitmap.h: %d, bitmap.w: %d\n", g->bitmap2.h, g->bitmap2.w);
    }
#endif /* DBG_AA */

    /* allocate pixmap for antialiasing ... */
    if (g->pixmap2 == NULL) {
	g->image2 = XCreateImage(DISP, G_visual, G_depth, ZPixmap,
				 0, (char *)NULL, g->bitmap2.w, g->bitmap2.h,
				 BMBITS, 0);
	size = g->image2->bytes_per_line * g->bitmap2.h;
	g->pixmap2 = g->image2->data = xmalloc(size != 0 ? size : 1);
    }
    /* ... and the pixmap used for globals.gc.fore2: */
    if (globals.gc.fore2 != NULL && g->pixmap2_gc2 == NULL) {
	size = g->image2->bytes_per_line * g->bitmap2.h;
	g->pixmap2_gc2 = xmalloc(size != 0 ? size : 1);
    }

#if 0
    if (resource.subpixel_order != SUBPIXEL_NONE) {
	fprintf(stderr, "\n============= BITMAP ==============\n");
	print_bitmap(&g->bitmap);
    }
#endif
    
    unshrunk_ptr = (bmUnitT *)g->bitmap.bits;
    actual_w = g->bitmap.w;
    rows_left = g->bitmap.h;
    y = 0;
    /* the basic algorithm is the same as in the nogrey code, with the main
       exception that the return value of sample() is used. */
    while (rows_left) {
#ifdef RGB_ANTI_ALIASING
	Pixel pixel = 0;
	Pixel pixel2 = 0;
#endif
	x = 0;
	if (rows > rows_left) /* why - extra safety? */
	    rows = rows_left;
	cols_left = g->bitmap.w;
	/*  	fprintf(stderr, "init_cols: %d\n", init_cols); */
	cols = init_cols;
	while (cols_left) {
	    if (cols > cols_left) /* why - extra safety? */
		cols = cols_left;
	    
	    thesample = sample(unshrunk_ptr, g->bitmap.bytes_wide,
			       (int)g->bitmap.w - cols_left, cols, rows);
	    
	    /* 	    if (resource.subpixel_order != SUBPIXEL_NONE && resource.subpixel_energy[2] != 0) */
	    /* 		onoff = thesample >= min_sample ? 0xffffff : 0; */
	    /* 	    else */
	    onoff = pixeltbl[thesample];

#ifdef XSERVER_INFO
	    if (globals.debug & DBG_PK) {
		int c;
		/* 		fprintf(stderr, "onoff: %d\n", onoff); */
		if (onoff > 65536)
		    c = onoff / 65536;
		else if (onoff > 256)
		    c = onoff / 256;
		else
		    c = onoff;
		if (c == 0)
		    fprintf(stdout, ",..");
		else
		    fprintf(stdout, ",%.2x", c);
	    }
#endif

#ifdef RGB_ANTI_ALIASING
	    if (resource.subpixel_order != SUBPIXEL_NONE) {
		int div = x / 3;
		int rest = x % 3;

		if (resource.subpixel_order == SUBPIXEL_RGB) {
		    if (rest == 0)
			pixel = onoff & G_visual->red_mask;
		    else if (rest == 1)
			pixel |= onoff & G_visual->green_mask;
		    else
			pixel |= onoff & G_visual->blue_mask;
		}
		else { /* SUBPIXEL_BGR */
		    if (rest == 0)
			pixel = onoff & G_visual->blue_mask;
		    else if (rest == 1)
			pixel |= onoff & G_visual->green_mask;
		    else
			pixel |= onoff & G_visual->red_mask;
		}
		
#if 0 && TRACE_AA1
		fprintf(stderr, "sample: %ld; row %d, col %d, left %d: 0x%.6lx; pixel: 0x%.6lx at pos %d,%d (x:%d)\n",
			thesample, rows, cols, cols_left, onoff, pixel, div, y, x);
#endif /* TRACE_AA */
		fprintf(stderr, "pixel at %d\n", div);
		XPutPixel(g->image2, div, y, pixel);

		/* HACK to fix color fringing problem */
		if (div + 1 < g->bitmap2.w) {
		    fprintf(stderr, "rest pixel at %d\n", div);
		    XPutPixel(g->image2, div + 1, y, 0);
		}
	    }
	    else {
#if 0 && TRACE_AA1
		fprintf(stderr, "sample: %ld; row %d, col %d, left %d: 0x%.6lx at pos %d, %d\n",
			thesample, rows, cols, cols_left, onoff, x, y);
#endif /* TRACE_AA */
		XPutPixel(g->image2, x, y, onoff);
	    }
#else
	    XPutPixel(g->image2, x, y, onoff);
#endif /* RGB_ANTI_ALIASING */
	    if (globals.gc.fore2 != NULL) {
		onoff2 = pixeltbl_gc2[thesample];
#ifdef RGB_ANTI_ALIASING
		if (resource.subpixel_order != SUBPIXEL_NONE) {
		    int div = x / 3;
		    int rest = x % 3;
		    
		    if (resource.subpixel_order == SUBPIXEL_RGB) {
			if (rest == 0)
			    pixel2 = onoff2 & G_visual->red_mask;
			else if (rest == 1)
			    pixel2 |= onoff2 & G_visual->green_mask;
			else
			    pixel2 |= onoff2 & G_visual->blue_mask;
		    }
		    else { /* SUBPIXEL_BGR */
			if (rest == 0)
			    pixel2 = onoff2 & G_visual->blue_mask;
			else if (rest == 1)
			    pixel2 |= onoff2 & G_visual->green_mask;
			else
			    pixel2 |= onoff2 & G_visual->red_mask;
		    }
		    
		    g->image2->data = g->pixmap2_gc2;
#if TRACE_AA1
		    fprintf(stderr, "fore2 at %d, %d\n", div, y);
#endif
		    XPutPixel(g->image2, div, y, pixel2);
		    g->image2->data = g->pixmap2;
		}
		else {
#else /* RGB_ANTI_ALIASING */
		    g->image2->data = g->pixmap2_gc2;
		    XPutPixel(g->image2, x, y, onoff2);
		    g->image2->data = g->pixmap2;
#endif
#ifdef RGB_ANTI_ALIASING
		}
#endif
	    }
	    
#if 0 && TRACE_AA1
	    fprintf(stderr, "subtracting %d from %d: %d\n", cols, cols_left, cols_left - cols);
#endif
	    cols_left -= cols;
	    cols = currwin.shrinkfactor;
	    x++;
	}
	/* advance pointer by the number of rows covered */
	/* 	fprintf(stderr, "++: %d; %d, %d\n", rows * g->bitmap.bytes_wide, g->bitmap.bytes_wide, sizeof(bmUnitT)); */
	unshrunk_ptr += rows * g->bitmap.bytes_wide / sizeof(bmUnitT);
	/* 	*((char **)&unshrunk_ptr) += rows * g->bitmap.bytes_wide; */
	rows_left -= rows;
	rows = currwin.shrinkfactor;
	y++;
#ifdef XSERVER_INFO
	if (globals.debug & DBG_PK)
	    fprintf(stdout, "\n");
#endif
    }
#ifdef XSERVER_INFO
    if (globals.debug & DBG_PK)
	fprintf(stdout, "\n");
#endif

#if 0
    fprintf(stderr, "y: %d, bitmap.h: %d\n", y, (int)g->bitmap2.h);
#endif /* DBG_AA */

    /* fill remaining rows not covered before (how?) */
    while (y < (int)g->bitmap2.h) {
	for (x = 0; x < (int)g->bitmap2.w; x++) {
	    /*  	    int c = *pixeltbl; */
#if TRACE_AA1
	    fprintf(stderr, "Remaining at %d, %d: 0x%.6lx\n", x, y, *pixeltbl);
#endif
	    XPutPixel(g->image2, x, y, *pixeltbl);
	    /*  	    if (c == 0) */
	    /*  		fprintf(stdout, ",.."); */
	    /*  	    else */
	    /*  		fprintf(stdout, ",%.2x", c); */
	    if (globals.gc.fore2 != NULL) {
		g->image2->data = g->pixmap2_gc2;
#if TRACE_AA1
		fprintf(stderr, "image2 at %d, %d: 0x%.6lx\n", x, y, *pixeltbl_gc2);
#endif
		XPutPixel(g->image2, x, y, *pixeltbl_gc2);
		g->image2->data = g->pixmap2;
	    }
	}
	y++;
    }

#ifdef RGB_ANTI_ALIASING
    if (resource.subpixel_order != SUBPIXEL_NONE && resource.subpixel_energy[1] != 0)
	color_filter_image(g);
    if (resource.subpixel_order != SUBPIXEL_NONE && resource.subpixel_energy[2] != 0)
	color_filter_image(g);
#endif
    
    g->y2 = g->y / currwin.shrinkfactor;
#if COLOR
    g->fg = fg_current;
#endif
}
#endif /* GREY */

/*
 *	Find font #n.
 */

static void
change_font(unsigned long n)
{
    struct tn *tnp;

    if (n < currinf.tn_table_len)
	currinf.fontp = currinf.tn_table[n];
    else {
	currinf.fontp = NULL;
	for (tnp = currinf.tn_head; tnp != NULL; tnp = tnp->next)
	    if (tnp->TeXnumber == n) {
		currinf.fontp = tnp->fontp;
		break;
	    }
    }
    if (currinf.fontp == NULL)
	XDVI_FATAL((stderr, "Non-existent font number %ld", n));
    if (currinf.fontp->set_char_p == NULL)
	XDVI_FATAL((stderr, "No procedure to set font %ld, %s", n, currinf.fontp->fontname));
    maxchar = currinf.fontp->maxchar;
    currinf.set_char_p = currinf.fontp->set_char_p;
}


/*
 *	Open a font file.
 */

static void
open_font_file(struct font *fontp)
{
    if (fontp->file == NULL) {
	fontp->file = XFOPEN(fontp->filename, OPEN_MODE);
	if (fontp->file == NULL)
	    XDVI_FATAL((stderr, "Couldn't re-locate font file `%s'", fontp->filename));
    }
}

/*
 * Read and return a 0-terminated special string allocated in static memory
 * (i.e. contents will be overwritten by next call of this function).
 */

static char *
read_special(FILE *fp, long nbytes)
{
    static char *spcl = NULL;
    static long spcl_len = -1;
    char *p;

    if (nbytes > spcl_len) {
	spcl = xrealloc(spcl, (unsigned)nbytes + 1);
	spcl_len = nbytes;
    }
    p = spcl;
    for (;;) {
	int i = currinf.end - currinf.pos;

	if (i > nbytes)
	    i = nbytes;
	memcpy(p, (char *)currinf.pos, i);
	currinf.pos += i;
	p += i;
	nbytes -= i;
	if (nbytes == 0)
	    break;
	(void)xxone(fp);
	--currinf.pos;
    }
    *p = '\0';
    return spcl;
}

/*
 *	Table used for scanning.  If >= 0, then skip that many bytes.
 *	M1 means end of page, M2 means special, M3 means FNTDEF,
 *	M4 means unrecognizable, and M5 means doesn't belong here.
 */

#define	M1	255
#define	M2	254
#define	M3	253
#define	M4	252
#define	M5	251
#define	MM	251

static ubyte scantable[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* chars 0 - 127 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 2,	/* SET1,SET2 (128,129) */
    /* -,-,SETRULE,PUT1,PUT2,-,-,PUTRULE,NOP,BOP (130-139) */
    M4, M4, 8, 1, 2, M4, M4, 8, 0, 44,
    M1, 0, 0, 1, 2, 3, 4, 0, 1, 2,	/* EOP,PUSH,POP,RIGHT1-4,W0M2 (140-149) */
    3, 4, 0, 1, 2, 3, 4, 1, 2, 3,	/* W3-4,X0-4,DOWN1-3 (150-159) */
    4, 0, 1, 2, 3, 4, 0, 1, 2, 3,	/* DOWN4,Y0-4,Z0-3 (160-169) */
    4,	/* Z4 (170) */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* change font 171 - 234 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 2, 3, 4, M2,	/* FNT1-4,XXX1 (235-239) */
    /* XXX2-4,FNTDEF1-4,PRE,POST,POSTPOST (240-249) */
    M2, M2, M2, M3, M3, M3, M3, M5, M5, M5,
    0, 0, M4, M4, M4, M4
};	/* SREFL,EREFL,-,-,-,- (250-255) */

/*
 *	This is the generic scanning routine.  It assumes that currinf, etc.
 *	are ready to go at the start of the page to be scanned.
 */



Boolean
spcl_scan(Boolean (*spcl_proc)(char *str, int str_len, void *data), void *data, Boolean return_if_found, FILE *fp)
{
    ubyte ch;
    ubyte n;
    long a;

    for (;;) {
	ch = xone(fp);
	/*  	print_dvi(ch); */
	n = scantable[ch];
	if (n < MM)
	    while (n-- != 0)
		(void)xone(fp);
	else if (n == M1)
	    break;	/* end of page */
	else
	    switch (n) {
	    case M2:	/* special */
		a = xnum(fp, ch - XXX1 + 1);
		if (a > 0) {
		    if (spcl_proc(read_special(fp, a), a, data) && return_if_found) {
			return True;
		    }
		}
		break;
	    case M3:	/* FNTDEF */
		xskip(fp, (long)(12 + ch - FNTDEF1 + 1));
		ch = xone(fp);
		xskip(fp, (long)ch + (long)xone(fp));
		break;
	    case M4:	/* unrecognizable */
		XDVI_FATAL((stderr, "unknown op-code %d", ch));
		break;
	    case M5:	/* doesn't belong */
		dvi_fmt_error("spcl_scan: shouldn't happen: %s encountered",
			      dvi_table2[ch - (FNTNUM0 + 64)]);
		break;
	    }
    }
    return False;
}

#define	xspell_conv(n)	spell_conv0(n, current_dimconv)

/*
 *	Prescanning routine for dvi file.  This looks for specials like
 *	`header=' and `!'.
 */

void
prescan(FILE *fp)
{
    if (fp == NULL) {
	return;
    }

    TRACE_FILES((stderr, "prescan on %p", (void *)fp));
    
    (void)lseek(fileno(fp), pageinfo_get_offset(scanned_page + 1), SEEK_SET);
    G_dvi_buf_ptr = dvi_buffer;
    currinf.pos = currinf.end = G_dvi_buf_ptr;
    for (;;) {
	if (scanned_page == -1) { /* on first page */
	    TRACE_FILES((stderr, "prescan on page 1"));
	    pageinfo_set_page_width(scanned_page + 1, pageinfo_get_page_width(total_pages));
	    pageinfo_set_page_height(scanned_page + 1, pageinfo_get_page_height(total_pages));
	    pageinfo_set_window_width(scanned_page + 1, pageinfo_get_window_width(total_pages));
	    pageinfo_set_window_height(scanned_page + 1, pageinfo_get_window_height(total_pages));
	}
	else {
	    TRACE_FILES((stderr, "prescan on page %d", scanned_page));
	    pageinfo_set_page_width(scanned_page + 1, pageinfo_get_page_width(scanned_page));
	    pageinfo_set_page_height(scanned_page + 1, pageinfo_get_page_height(scanned_page));
	    pageinfo_set_window_width(scanned_page + 1, pageinfo_get_window_width(scanned_page));
	    pageinfo_set_window_height(scanned_page + 1, pageinfo_get_window_height(scanned_page));
	}
	if (read_events(EV_NOWAIT) & EV_GE_NEWPAGE) {
	    break;
	}
	/* NOTE:  longjmp(globals.ev.canit) should not be done within
	   read_events(). */
	htex_prescan_save();
	htex_prescan_initpage();

	if (!setjmp(globals.ev.canit)) {
	    struct htex_prescan_data data;
	    int pre_depth, depth;
	    data.pageno = scanned_page + 1;
	    data.scan_type = HTEX_ANCHOR_NUM; /* just for the anchor numbers */
	    pre_depth = htex_prescan_get_depth();
	    (void)spcl_scan(scan_special, &data, False, fp);
	    
	    depth = htex_prescan_get_depth();

	    if (depth > pre_depth) {
		/* we have a mismatched anchor. We currently don't deal with
		   _nested_ mismatched anchors (ugh), so there's only one
		   anchor string that can be used as info on the next page. */
		int anchor_num = htex_prescan_get_mismatched_anchor_num(depth);
		/* scan again to find the anchor string at anchor number `anchor_num' */
		(void)lseek(fileno(fp), pageinfo_get_offset(scanned_page + 1), SEEK_SET);
		currinf.pos = currinf.end = G_dvi_buf_ptr;
		data.anchor_num = anchor_num;
		data.scan_type = HTEX_ANCHOR_STRING;
		htex_prescan_reset_firstpass();
		(void)spcl_scan(scan_special, &data, False, fp);
		depth = htex_prescan_get_depth();
	    }
	    else if (depth > 0 && scanned_page >= 0) { /* mismatch was on a previous page */
		htex_prescan_carry_over(scanned_page, scanned_page + 1);
	    }
	}
	else { /* if interrupted */
	    htex_prescan_restore(scanned_page + 1);
#if PS
	    psp.interrupt();
#endif
	    break;
	}
	if (globals.ev.flags & EV_GE_NEWPAGE)
	    break;
	++scanned_page;
#if COLOR
	if (scanned_page_color < scanned_page) {
	    scan_color_eop();
	    scanned_page_color = scanned_page;
	}
#endif
#if PS
	if (scanned_page_ps < scanned_page)
	    scanned_page_ps = scanned_page;
#endif
	if (scanned_page >= current_page)
	    break;
    }
    
#if PS
    if (!(globals.ev.flags & EV_GE_NEWPAGE))
	psp.endheader();
#endif
}

/*
 *	Routines to print characters.
 */

setcharRetvalT
set_char(
#ifdef TEXXET
	 wide_ubyte cmd,
#endif
	 wide_ubyte ch)
{
    struct glyph *g;
#ifdef TEXXET
    long dvi_h_sav;
#endif

    if (ch > maxchar)
	realloc_font(currinf.fontp, (wide_ubyte)ch);
    if ((g = &currinf.fontp->glyph[ch])->bitmap.bits == NULL) {
	if (g->addr == 0) {
	    if (!resource.hush_chars)
		XDVI_WARNING((stderr, "Character %d not defined in font %s", ch, currinf.fontp->fontname));
	    g->addr = -1;
#ifdef TEXXET
	    return;
#else
	    return 0L;
#endif
	}
	if (g->addr == -1) {
#ifdef TEXXET
	    return;
#else
	    return 0L; /* previously flagged missing char */
#endif
	}
	open_font_file(currinf.fontp);
	fseek(currinf.fontp->file, g->addr, SEEK_SET);
	(*currinf.fontp->read_char) (currinf.fontp, ch);
	if (globals.debug & DBG_BITMAP)
	    print_char((ubyte) ch, g);
	currinf.fontp->timestamp = ++current_timestamp;
    }

#ifdef TEXXET
    dvi_h_sav = DVI_H;
    if (currinf.dir < 0)
	DVI_H -= g->dvi_adv;

    if (scan_frame == NULL) {
#endif
	
#ifdef RGB_ANTI_ALIASING
	if (currwin.shrinkfactor == -1) {
	    put_bitmap(&g->bitmap, PXL_H - g->x, PXL_V - g->y);
	}
#ifdef __GNUC__
#warning TODO: implement horizontal AA at shrink 1
#endif
#else
	if (currwin.shrinkfactor == 1) {
	    put_bitmap(&g->bitmap, PXL_H - g->x, PXL_V - g->y);
	}
#endif
	else {
#ifdef	GREY
	    if (resource.use_grey) {
		if (g->pixmap2 == NULL) {
#ifdef DBG_AA
		    fprintf(stderr, "shinking the bitmap!\n");
#endif /* DBG_AA */
		    /*  		    print_bitmap(&g->bitmap); */
		    shrink_glyph_grey(g);
		}
		put_image(g, PXL_H - g->x2, PXL_V - g->y2);
	    }
	    else {
		if (g->bitmap2.bits == NULL) {
		    shrink_glyph(g);
		}
		put_bitmap(&g->bitmap2, PXL_H - g->x2, PXL_V - g->y2);
	    }
#else
	    if (g->bitmap2.bits == NULL) {
		shrink_glyph(g);
	    }
	    put_bitmap(&g->bitmap2, PXL_H - g->x2, PXL_V - g->y2);
#endif
	}
#ifdef TEXXET
    }
    if (cmd == PUT1 || (resource.omega && cmd == PUT2))
	DVI_H = dvi_h_sav;
    else if (currinf.dir > 0)
	DVI_H += g->dvi_adv;
    return;
#else
    return g->dvi_adv;
#endif
}

/*
 *	Routines to print characters.
 */

static setcharRetvalT
set_empty_char(
#ifdef TEXXET
	       wide_ubyte cmd,
#endif
	       wide_ubyte ch)
{
#ifdef TEXXET
    UNUSED(cmd);
#endif
    UNUSED(ch);
#ifdef TEXXET
    return;
#else
    return 0L;
#endif
}

setcharRetvalT
load_n_set_char(
#ifdef TEXXET
		wide_ubyte cmd,
#endif
		wide_ubyte ch)
{
    if (!load_font(currinf.fontp, resource.t1lib
#if DELAYED_MKTEXPK
		   , True
#endif
		   )) {	/* if not found */
	if (globals.ev.flags & EV_GE_NEWDOC) {	/* if abort */
	    longjmp(globals.ev.canit, 1);
	}

	/* FIXME: does this need to be replaced by GUI warning, or has this case already been covered? */
	XDVI_WARNING((stderr, "load_n_set_char: Character(s) will be left blank."));
	currinf.set_char_p = currinf.fontp->set_char_p = set_empty_char;

#ifdef TEXXET
	return;
#else
	return 0L;
#endif
    }
    maxchar = currinf.fontp->maxchar;
    currinf.set_char_p = currinf.fontp->set_char_p;
#ifdef TEXXET
    (*currinf.set_char_p) (cmd, ch);
    return;
#else
    return (*currinf.set_char_p) (ch);
#endif
}

setcharRetvalT
set_vf_char(
#ifdef TEXXET
	    wide_ubyte cmd,
#endif
	    wide_ubyte ch)
{
    struct macro *m;
    struct drawinf oldinfo;
    wide_ubyte oldmaxchar;
    static ubyte c;
#ifdef TEXXET
    long dvi_h_sav;
#endif

    if (ch > maxchar)
	realloc_virtual_font(currinf.fontp, ch);
    if ((m = &currinf.fontp->macro[ch])->pos == NULL) {
	if (!resource.hush_chars)
	    XDVI_WARNING((stderr, "Character %d not defined in font %s", ch, currinf.fontp->fontname));
	m->pos = m->end = &c;
#ifdef TEXXET
	return;
#else
	return 0L;
#endif
    }
#ifdef TEXXET
    dvi_h_sav = DVI_H;
    if (currinf.dir < 0)
	DVI_H -= m->dvi_adv;
    if (scan_frame == NULL) {
#endif
	oldinfo = currinf;
	if (!currinf.virtual)
	    dvi_pointer_frame = &oldinfo;
	oldmaxchar = maxchar;
	WW = XX = YY = ZZ = 0;
	currinf.tn_table_len = VFTABLELEN;
	currinf.tn_table = currinf.fontp->vf_table;
	currinf.tn_head = currinf.fontp->vf_chain;
	currinf.pos = m->pos;
	currinf.end = m->end;
	currinf.virtual = currinf.fontp;

	draw_part(globals.dvi_file.bak_fp, current_frame, currinf.fontp->dimconv);
	if (currinf.pos != currinf.end + 1)
	    dvi_fmt_error("virtual character macro does not end correctly");
	currinf = oldinfo;
	if (!currinf.virtual)
	    dvi_pointer_frame = &currinf;
	maxchar = oldmaxchar;
#ifdef TEXXET
    }
    if (cmd == PUT1 || (resource.omega && cmd == PUT2))
	DVI_H = dvi_h_sav;
    else if (currinf.dir > 0)
	DVI_H += m->dvi_adv;
    return;
#else
    return m->dvi_adv;
#endif
}

static setcharRetvalT
set_no_char(
#ifdef TEXXET
	    wide_ubyte cmd,
#endif
	    wide_ubyte ch)
{
    if (currinf.virtual) {
	currinf.fontp = currinf.virtual->first_font;
	if (currinf.fontp != NULL) {
	    maxchar = currinf.fontp->maxchar;
	    currinf.set_char_p = currinf.fontp->set_char_p;
#ifdef TEXXET
	    (*currinf.set_char_p) (cmd, ch);
	    return;
#else
	    return (*currinf.set_char_p) (ch);
#endif
	}
    }
    dvi_fmt_error("set_no_char: attempt to set character of unknown font");
    exit(0);
    /* NOTREACHED */
}


/*
 *	Set rule.  Arguments are coordinates of lower left corner.
 */

static void
set_rule(int h, int w)
{
#ifdef TEXXET
    put_rule(PXL_H - (currinf.dir < 0 ? w - 1 : 0), PXL_V - h + 1,
	     (unsigned int)w, (unsigned int)h);
#else
    put_rule(PXL_H, PXL_V - h + 1, (unsigned int)w, (unsigned int)h);
#endif
}


/*
 *	Interpret a sequence of dvi bytes (either the page from the dvi file,
 *	or a character from a virtual font).
 */

static void
draw_part(FILE *fp, struct frame *minframe, double current_dimconv)
{
    ubyte ch = 0;
#ifdef TEXXET
    struct drawinf oldinfo;
    wide_ubyte oldmaxchar = 0;
    off_t file_pos = 0;
    int refl_count = 0;
#endif
    int pause_cnt = 0;

    globals.pausing.flag = False;

    currinf.fontp = NULL;
    currinf.set_char_p = set_no_char;
#ifdef TEXXET
    currinf.dir = 1;
    scan_frame = NULL;	/* indicates we're not scanning */
#endif

    for (;;) {
	ch = xone(fp);
	if (globals.debug & DBG_DVI) {
	    print_dvi(ch);
	}
	if (ch <= (ubyte)(SETCHAR0 + 127)) {
#ifdef TEXXET
	    (*currinf.set_char_p) (ch, ch);
#else
	    DVI_H += (*currinf.set_char_p) (ch);
#endif
	}
	else if (FNTNUM0 <= ch && ch <= (ubyte) (FNTNUM0 + 63)) {
	    change_font((unsigned long)(ch - FNTNUM0));
	}
	else {
	    long a, b;

	    switch (ch) {
	    case SET1:
	    case PUT1:
#ifdef TEXXET
		(*currinf.set_char_p) (ch, xone(fp));
#else
		a = (*currinf.set_char_p) (xone(fp));
		if (ch != PUT1)
		    DVI_H += a;
#endif
		break;

	    case SET2:
	    case PUT2:
		if (!resource.omega)
		    dvi_fmt_error("%s:%d: draw_part: op-code %d only works with the \"-omega\" option",
				  __FILE__, __LINE__, ch);
		else {
#ifdef TEXXET
		    (*currinf.set_char_p) (ch, xnum(fp, 2));
#else
		    a = (*currinf.set_char_p) (xnum(fp, 2));
		    if (ch != PUT2)
			DVI_H += a;
#endif
		}
		break;

	    case SETRULE:
		/* Be careful, dvicopy outputs rules with
		   height = 0x80000000.  We don't want any
		   SIGFPE here. */
		a = xsfour(fp);
		b = xspell_conv(xsfour(fp));
		if (a > 0 && b > 0
#ifdef TEXXET
		    && scan_frame == NULL
#endif
		    ) {
		    set_rule(pixel_round(xspell_conv(a)), pixel_round(b));
		}
		DVI_H += DIR * b;
		break;

	    case PUTRULE:
		a = xspell_conv(xsfour(fp));
		b = xspell_conv(xsfour(fp));
		if (a > 0 && b > 0
#ifdef TEXXET
		    && scan_frame == NULL
#endif
		    ) {
		    set_rule(pixel_round(a), pixel_round(b));
		}
		break;

	    case NOP:
		break;

	    case BOP:
		xskip(fp, (long)11 * 4);
		DVI_H = OFFSET_X;
		DVI_V = OFFSET_Y;
		PXL_V = pixel_conv(DVI_V);
		WW = XX = YY = ZZ = 0;
		break;

	    case EOP:
		if (current_frame != minframe)
		    dvi_fmt_error("%s:%d: draw_part: stack not empty at EOP", __FILE__, __LINE__);
		return;

	    case PUSH:
		if (current_frame->next == NULL) {
		    struct frame *newp = xmalloc(sizeof *newp);

		    current_frame->next = newp;
		    newp->prev = current_frame;
		    newp->next = NULL;
		}
		current_frame = current_frame->next;
		current_frame->data = currinf.data;
		break;

	    case POP:
		if (current_frame == minframe)
		    dvi_fmt_error("%s:%d: draw_part: more POPs than PUSHes", __FILE__, __LINE__);
		currinf.data = current_frame->data;
		current_frame = current_frame->prev;
		break;

#ifdef TEXXET
	    case SREFL:
		if (scan_frame == NULL) {
		    /* we're not scanning:  save some info. */
		    oldinfo = currinf;
		    oldmaxchar = maxchar;
		    if (!currinf.virtual)
			file_pos = xtell(fp, currinf.pos);
		    scan_frame = current_frame;	/* now we're scanning */
		    refl_count = 0;
		    break;
		}
		/* we are scanning */
		if (current_frame == scan_frame)
		    ++refl_count;
		break;

	    case EREFL:
		if (scan_frame != NULL) {	/* if we're scanning */
		    if (current_frame == scan_frame && --refl_count < 0) {
			/* we've hit the end of our scan */
			scan_frame = NULL;
			/* first:  push */
			if (current_frame->next == NULL) {
			    struct frame *newp = xmalloc(sizeof *newp);

			    current_frame->next = newp;
			    newp->prev = current_frame;
			    newp->next = NULL;
			}
			current_frame = current_frame->next;
			current_frame->data = currinf.data;
			/* next:  restore old file position, XX, etc. */
			if (!currinf.virtual) {
			    off_t bgn_pos = xtell(fp, G_dvi_buf_ptr);

			    if (file_pos >= bgn_pos) {
				oldinfo.pos = dvi_buffer + (file_pos - bgn_pos);
				oldinfo.end = currinf.end;
			    }
			    else {
				(void)lseek(fileno(fp), file_pos, SEEK_SET);
				oldinfo.pos = oldinfo.end;
			    }
			}
			currinf = oldinfo;
			maxchar = oldmaxchar;
			/* and then:  recover position info. */
			DVI_H = current_frame->data.dvi_h;
			DVI_V = current_frame->data.dvi_v;
			PXL_V = current_frame->data.pxl_v;
			/* and finally, reverse direction */
			currinf.dir = -currinf.dir;
		    }
		    break;
		}
		/* we're not scanning, */
		/* so just reverse direction and then pop */
		currinf.dir = -currinf.dir;
		currinf.data = current_frame->data;
		current_frame = current_frame->prev;
		break;
#endif /* TEXXET */

	    case RIGHT1:
	    case RIGHT2:
	    case RIGHT3:
	    case RIGHT4:
		DVI_H += DIR * xspell_conv(xsnum(fp, ch - RIGHT1 + 1));
		break;

	    case W1:
	    case W2:
	    case W3:
	    case W4:
		WW = xspell_conv(xsnum(fp, ch - W0));
	    case W0:
		DVI_H += DIR * WW;
		break;

	    case X1:
	    case X2:
	    case X3:
	    case X4:
		XX = xspell_conv(xsnum(fp, ch - X0));
	    case X0:
		DVI_H += DIR * XX;
		break;

	    case DOWN1:
	    case DOWN2:
	    case DOWN3:
	    case DOWN4:
		DVI_V += xspell_conv(xsnum(fp, ch - DOWN1 + 1));
		PXL_V = pixel_conv(DVI_V);
		break;

	    case Y1:
	    case Y2:
	    case Y3:
	    case Y4:
		YY = xspell_conv(xsnum(fp, ch - Y0));
	    case Y0:
		DVI_V += YY;
		PXL_V = pixel_conv(DVI_V);
		break;

	    case Z1:
	    case Z2:
	    case Z3:
	    case Z4:
		ZZ = xspell_conv(xsnum(fp, ch - Z0));
	    case Z0:
		DVI_V += ZZ;
		PXL_V = pixel_conv(DVI_V);
		break;

	    case FNT1:
	    case FNT2:
	    case FNT3:
	    case FNT4:
		change_font(xnum(fp, ch - FNT1 + 1));
		break;

	    case XXX1:
	    case XXX2:
	    case XXX3:
	    case XXX4:
		a = xnum(fp, ch - XXX1 + 1);
		if (a > 0) {
		    char *p = read_special(fp, a);
		    if (resource.pause && strcmp(p, resource.pause_special) == 0) {
			if (++pause_cnt > globals.pausing.num) {
			    globals.pausing.flag = True;
			    /* can't use longjmp(globals.ev.canit, 1); */
			    return;
			}
		    }
		    applicationDoSpecial(p, a);
		}
		break;

	    case FNTDEF1:
	    case FNTDEF2:
	    case FNTDEF3:
	    case FNTDEF4:
		xskip(fp, (long)(12 + ch - FNTDEF1 + 1));
		a = (long)xone(fp);
		xskip(fp, a + (long)xone(fp));
		break;

#ifndef TEXXET
	    case SREFL:
	    case EREFL:
#endif
	    case PRE:
	    case POST:
	    case POSTPOST:
		dvi_fmt_error("%s:%d: draw_part: shouldn't happen: %s encountered",
			      __FILE__, __LINE__, dvi_table2[ch - (FNTNUM0 + 64)]);
		break;

	    default:
		dvi_fmt_error("%s:%d: draw_part: unknown op-code %d", __FILE__, __LINE__, ch);
	    }	/* end switch */
	}	/* end else (ch not a SETCHAR or FNTNUM) */
    }	/* end for */
}

extern int waiting_for_anchor;

static void
warn_raw_postscript(void)
{
    static int *pagelist = NULL;
    static int pagelist_size = 0;
#ifdef PS
    if (!resource.hush_stdout) {
	if (total_pages >= pagelist_size) {
	    pagelist_size = total_pages + 1;
	    pagelist = xrealloc(pagelist, pagelist_size * sizeof *pagelist);
	    memset(pagelist, 0, pagelist_size * sizeof *pagelist);
	}
	
	ASSERT(pagelist_size > current_page, "pagelist_size too small");
	if (pagelist[current_page] == 0) {
	    XDVI_WARNING((stderr, "Raw Postscript commands on page %d may be rendered incorrectly.",
			  current_page + 1));
	    pagelist[current_page] = 1;
	}
	
	/* too likely to overdraw important information */
	/* 	statusline_info(STATUS_MEDIUM, */
	/* 			 "Warning: Postscript commands on this page may not display correctly."); */
    }
#endif /* PS */
}

void
draw_page(void)
{
#if 0
    volatile double save_gamma = 0.0;
#endif /* 0 */
    /* Check for changes in dvi file. */
    if (dvi_file_changed()) {
	return;
    }

    if (globals.dvi_file.bak_fp == NULL) {
	return;
    }

#ifdef PS
    have_raw_postscript = False;
#endif
    
#if COLOR
    color_bottom = &fg_initial;
    color_bot_size = 1;

    if (page_colors.stack != NULL && current_page > 0) {
	color_bottom = page_colors.stack[current_page - 1].colorstack;
	color_bot_size = page_colors.stack[current_page - 1].stacksize;
    }
    rcs_top = NULL;
    ASSERT(color_bot_size > 0, "color_bot_size mustn't become negative!");
    ASSERT(color_bottom != NULL, "color_bottom mustn't become negative!");
    set_fg_color(&color_bottom[color_bot_size - 1]);
#endif /* COLOR */

#if !FIXED_FLUSHING_PAGING    
    draw_border(-currwin.base_x, -currwin.base_y,
		ROUNDUP(pageinfo_get_page_width(current_page), currwin.shrinkfactor) + 2,
		ROUNDUP(pageinfo_get_page_height(current_page), currwin.shrinkfactor) + 2, globals.gc.high);
#endif /* MOTIF */

    if (resource.grid_mode > 0)	{ /* grid is wanted */
	put_grid(-currwin.base_x, -currwin.base_y,
		 /* 		 ROUNDUP(globals.page.unshrunk_w, currwin.shrinkfactor) + 2, */
		 /* 		 ROUNDUP(globals.page.unshrunk_h, currwin.shrinkfactor) + 2, */
		 ROUNDUP(pageinfo_get_page_width(current_page), currwin.shrinkfactor) + 2,
		 ROUNDUP(pageinfo_get_page_height(current_page), currwin.shrinkfactor) + 2,
		 ROUNDUP(globals.grid_paper_unit, currwin.shrinkfactor),
		 globals.gc.high);
    }
    
    (void) lseek(fileno(globals.dvi_file.bak_fp), pageinfo_get_offset(current_page), SEEK_SET);

    memset((char *)&currinf.data, '\0', sizeof currinf.data);
    currinf.tn_table_len = TNTABLELEN;
    currinf.tn_table = tn_table;
    currinf.tn_head = tn_head;
    currinf.pos = currinf.end = dvi_buffer;
    currinf.virtual = NULL;
    dvi_pointer_frame = &currinf;
    drawing_mag = (currwin.win == magnifier.win);
    psfig_begun = False;

    htex_initpage(False, False, current_page);

    if (currwin.win == mane.win) {
	XRectangle rect;
	rect.x = globals.win_expose.min_x;
	rect.y = globals.win_expose.min_y;
	rect.width = globals.win_expose.max_x - globals.win_expose.min_x;
	rect.height = globals.win_expose.max_y - globals.win_expose.min_y;
	/*  	fprintf(stderr, "clip: %d, %d, %d, %d\n", */
	/* 		globals.win_expose.min_x, */
	/* 		globals.win_expose.min_y, */
	/* 		globals.win_expose.max_x - globals.win_expose.min_x, */
	/* 		globals.win_expose.max_y - globals.win_expose.min_y); */
#define SET_CLIP(gc)	if (gc != NULL) XSetClipRectangles(DISP, gc, 0, 0, &rect, 1, Unsorted)
#define CLEAR_CLIP(gc)  if (gc != NULL) XSetClipMask(DISP, gc, None)
	/* Set clip masks for all GC's */
	SET_CLIP(globals.gc.fore);
	SET_CLIP(globals.gc.fore2);
	SET_CLIP(globals.gc.fore2_bak);
	SET_CLIP(globals.gc.fore2_bak1);
	SET_CLIP(globals.gc.rule);
	SET_CLIP(globals.gc.high);
	SET_CLIP(globals.gc.linkcolor);
	SET_CLIP(globals.gc.copy);
    }
    
    if (!setjmp(globals.ev.canit)) {
	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! BUG ALERT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	ALL GLYPH DRAWING/RULE SETTING COMMANDS THAT MIGHT INVOKE
	longjmp(globals.ev.canit)
	MUST GO INSIDE THIS IF CASE, AND MUST NOT BE INVOKED FROM
	SOMEWHERE ELSE!
	   
	Failure to check whether a command could (indirectly) invoke
	such a drawing routine (like e.g. put_rule()) will result
	in *really* strange bugs (see e.g. #616920, and probably also #471021).
	   
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! BUG ALERT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	*/
	/* generate an expose event */
	if (search_have_match(current_page)) {
	    search_erase_highlighting(False);
	}
	if (globals.curr_mode == TEXT_MODE_ACTIVE) {
	    text_change_region(TEXT_SEL_ERASE, NULL);
	}
	draw_part(globals.dvi_file.bak_fp, current_frame = &frame0, dimconv);

	if (have_raw_postscript) {
	    warn_raw_postscript();
	}
    }
    else {
	/* If we were interrupted, put the expose event back, so that the
	 * region gets redrawn.  The if statement is necessary because the
	 * magnifier may have been destroyed as part of the interrupt.  */
	if (currwin.win == mane.win || currwin.win == magnifier.win) {
	    expose(currwin.win == mane.win ? &mane : &magnifier,
		   globals.win_expose.min_x - currwin.base_x, globals.win_expose.min_y - currwin.base_y,
		   globals.win_expose.max_x - globals.win_expose.min_x, globals.win_expose.max_y - globals.win_expose.min_y);
	}

#ifdef PS
	psp.interrupt();
	/* reset this flag too, just to make sure ... */
# if defined(PS_GS) && GS_PIXMAP_CLEARING_HACK
	had_ps_specials = False;
# endif
#endif
	globals.ev.flags &= ~EV_MAG_GONE;
#if 0
	if (search_have_match() && save_gamma != 0.0) {
	    resource.gamma = save_gamma;
	    do_color_change();
	    reset_fonts();
	}
#endif /* 0 */
    }

    drawing_mag = False;
    dvi_pointer_frame = NULL;
    if (currwin.win == mane.win) {
	if (globals.src.fwd_box_page >= 0) {
	    source_fwd_draw_box(); /* draw box showing found source line */
	}
	htex_draw_anchormarkers();
    }
#ifdef PS
    psp.endpage();
#endif
    if (currwin.win == mane.win && resource.postscript != 1) {
	display_bboxes();
    }
    if (search_have_match(current_page)) {
	/* highlight search match */
	search_draw_inverted_regions();
    }
    if (currwin.win == mane.win && (globals.curr_mode == TEXT_MODE_ACTIVE)) {
	/* highlight selection */
	text_change_region(TEXT_SEL_REDRAW, NULL);
    }
    if (globals.curr_mode == RULER_MODE_ACTIVE) {
	redraw_ruler();
    }

    clear_bboxes();

    if (currwin.win == mane.win) {
	CLEAR_CLIP(globals.gc.fore);
	CLEAR_CLIP(globals.gc.fore2);
	CLEAR_CLIP(globals.gc.fore2_bak);
	CLEAR_CLIP(globals.gc.fore2_bak1);
	CLEAR_CLIP(globals.gc.rule);
	CLEAR_CLIP(globals.gc.high);
	CLEAR_CLIP(globals.gc.linkcolor);
	CLEAR_CLIP(globals.gc.copy);
    }
#undef SET_CLIP
#undef CLEAR_CLIP

    currwin.win = (Window) 0;
}

/* this sets the file-scope htex_anchor_type to the type of the anchor just scanned,
   which is used by the drawing routines to determine whether current position is inside
   an anchor. Called from special.c.
*/
void
htex_do_special(const char *str, size_t len)
{
    if (INSIDE_MANE_WIN) {
	htex_inside_href = htex_scan_anchor(str, len);
    }
}

/*
 *	General dvi scanning routines.  These are used for:
 *	    o	source special lookups and
 *	    o	finding the dimensions of links (if compiling with support for
 *		hypertext specials).
 *	This routine can be a bit slower than draw_page()/draw_part(), since
 *	it is not run that often; that is why it is a separate routine in
 *	spite of much duplication.
 *
 *	Note that it does not use a separate copy of define_font().
 */

/*
 *	This set of routines can be called while draw_part() is active,
 *	so the global variables must be separate.
 */

static struct frame geom_frame0;	/* dummy head of list */

#ifdef TEXXET
static struct frame *geom_scan_frame;	/* head frame for scanning */
#endif

static struct frame *geom_current_frame;

static uint32_t
get_unicode_char(wide_ubyte ch, struct drawinf currinf, char *retbuf)
{
    /* attempt 1: if it's a Type1 font, get the Adobe character name
       from T1lib's T1_GetCharName */
    if (currinf.set_char_p == set_t1_char) {
	int id = currinf.fontp->t1id;
	int t1libid = fontmaps[id].t1libid;
	char *char_name;
	ASSERT(t1libid != -1, "Font must have been loaded at this point!");
	char_name = T1_GetCharName(t1libid, ch);
	TRACE_FIND_VERBOSE((stderr, "T1 char: %d = `%s' (id: %d; font: %s; enc: %d)",
			    ch, char_name,
			    currinf.fontp->t1id, fontmaps[currinf.fontp->t1id].texname,
			    fontmaps[currinf.fontp->t1id].enc));
	return adobe2unicode_name(char_name);
    }
    else {
	/* attempt 2: try to derive the encoding from the font name, by
	   looking it up in a list of known font names.
	*/
	return guess_encoding(ch, currinf.fontp->fontname, retbuf);
    }
}

#define MAX_CHARS 16 /* maximum number of unicode characters that one do_char can produce (rounded up ...) */
static const size_t ALLOC_STEP = 1024;

/* create a page break at BOP, unless we're at the start of the file */
static void
do_newpage(struct scan_info *info)
{
    struct word_info *w_info = (struct word_info *)info->data;
    if (w_info->curr_buf_idx > 0) { /* not at start of file */
	/* resize buffer if needed */
	while (w_info->curr_buf_idx + MAX_CHARS >= w_info->txt_buf_size) {
	    w_info->txt_buf_size += ALLOC_STEP;
	    w_info->txt_buf = xrealloc(w_info->txt_buf, w_info->txt_buf_size);
	}
	w_info->txt_buf[w_info->curr_buf_idx++] = '\n';
    }
}

static void
reset_bboxes(struct word_info *info)
{
    size_t i;
    for (i = 0; i < info->bboxes_size; i++) {
	info->bboxes[i].ulx = INT_MAX;
	info->bboxes[i].uly = INT_MAX;
	info->bboxes[i].lrx = 0;
	info->bboxes[i].lry = 0;
    }
}

static void
delete_last_bbox(struct word_info *info)
{
    info->bboxes[info->bboxes_idx].ulx = INT_MAX;
    info->bboxes[info->bboxes_idx].uly = INT_MAX;
    info->bboxes[info->bboxes_idx].lrx = 0;
    info->bboxes[info->bboxes_idx].lry = 0;
}

static void
finish_bbox(struct word_info *info)
{
    /*     if (info->bboxes_idx == 0) /\* nothing to do *\/ */
    /* 	return; */
    
    info->bboxes_idx++;
    while (info->bboxes_idx + 1 > info->bboxes_size) {
	/* re-allocate info */
	info->bboxes_size += 32;
	info->bboxes = xrealloc(info->bboxes, info->bboxes_size * sizeof *(info->bboxes));
    }
    info->bboxes[info->bboxes_idx].ulx = INT_MAX;
    info->bboxes[info->bboxes_idx].uly = INT_MAX;
    info->bboxes[info->bboxes_idx].lrx = 0;
    info->bboxes[info->bboxes_idx].lry = 0;

#if 0
    fprintf(stderr, "========= finish_bbox: index=%d, boxes:\n", info->bboxes_idx);
    {
	int i;
	for (i = 0; i < info->bboxes_idx; i++) {
	    fprintf(stderr, "%d: x %d, y %d, w %d, h %d\n",
		    i, info->bboxes[i].ulx, info->bboxes[i].uly, info->bboxes[i].lrx, info->bboxes[i].lry);
	}
    }
#endif /* 0 */
}

static void
create_bbox(struct word_info *info, int x, int y, int w, int h)
{
#if 0
    fprintf(stderr, "++++++++++++++ inside match: %d,%d,%d,%d!\n", x,y,w,h);
    XDrawRectangle(DISP, mane.win, globals.gc.high,
		   x / (double)currwin.shrinkfactor + 0.5,
		   y / (double)currwin.shrinkfactor + 0.5,
		   w / (double)currwin.shrinkfactor + 0.5,
		   h / (double)currwin.shrinkfactor + 0.5);
#endif
    while (info->bboxes_idx + 1 > info->bboxes_size) {
	/* re-allocate info */
	size_t old_size = info->bboxes_size;
	size_t i;
	info->bboxes_size += 32;
	info->bboxes = xrealloc(info->bboxes, info->bboxes_size * sizeof *(info->bboxes));
	for (i = old_size; i < info->bboxes_size; i++) {
	    info->bboxes[i].ulx = INT_MAX;
	    info->bboxes[i].uly = INT_MAX;
	    info->bboxes[i].lrx = 0;
	    info->bboxes[i].lry = 0;
	}
	
    }
    /* adjust size of box */
    if (x < info->bboxes[info->bboxes_idx].ulx)
	info->bboxes[info->bboxes_idx].ulx = x;
    if (y < info->bboxes[info->bboxes_idx].uly)
	info->bboxes[info->bboxes_idx].uly = y;
    if (x + w > info->bboxes[info->bboxes_idx].lrx)
	info->bboxes[info->bboxes_idx].lrx = x + w;
    if (y + h > info->bboxes[info->bboxes_idx].lry)
	info->bboxes[info->bboxes_idx].lry = y + h;
#if 0
    fprintf(stderr, "dimens: %d, %d, %d, %d\n",
	    info->bboxes[info->bboxes_idx].ulx,
	    info->bboxes[info->bboxes_idx].uly,
	    info->bboxes[info->bboxes_idx].lrx,
	    info->bboxes[info->bboxes_idx].lry);
    fprintf(stderr, "============ create_bbox: index=%d, boxes:\n", info->bboxes_idx);
    {
	int i;
	for (i = 0; i <= info->bboxes_idx; i++) {
	    fprintf(stderr, "%d: x %d, y %d, w %d, h %d\n",
		    i, info->bboxes[i].ulx, info->bboxes[i].uly, info->bboxes[i].lrx, info->bboxes[i].lry);
	}
    }
#endif /* 0 */
}

static void
map_index_positions(const struct search_info *searchinfo,
		    const struct page_mapping *page_mapping,
		    int *from, int *to)
{
    *from = searchinfo->from_pos;
    *to = searchinfo->to_pos;

    if (*from >= page_mapping[0].offset) { /* on second page of scan */
#if 0
	fprintf(stderr, "current_page: %d, from: %d, to: %d; subtracting %d for pageinfo at index 0\n",
		current_page, *from, *to, page_mapping[0].offset);
#endif /* 0 */
	if (page_mapping[0].offset != -1) {
	    ASSERT(page_mapping[0].offset != -1, "page_mapping not properly initialized?");
	    (*from) -= page_mapping[0].offset;
	    (*to) -= page_mapping[0].offset;
	}
	ASSERT(*from >= 0, "index must be > 0");
	ASSERT(*to >= 0, "index must be > 0");
	
    }
}

/* decrement curr_buf_idx so that the previous character in info
 * (which might be a multi-byte character) is erased.
 */
static void
erase_prev_char(struct word_info *info)
{
    /* erase the hyphen, which might be a multibyte character */
    if ((unsigned char)(info->txt_buf[info->curr_buf_idx - 1]) < 0x80) { /* single-byte */
	TRACE_FIND_VERBOSE((stderr, "1 XXX: %d",
			    (unsigned char)(info->txt_buf[info->curr_buf_idx - 1])));
	info->txt_buf[--(info->curr_buf_idx)] = '\0';
    }
    else { /* skip back over multi-byte sequence */
	while ((unsigned char)(info->txt_buf[info->curr_buf_idx - 1]) >= 0x80
	       && (unsigned char)(info->txt_buf[info->curr_buf_idx - 1]) < 0xC0) { /* 10xxxxxx bytes */
	    TRACE_FIND_VERBOSE((stderr, "2 XXX: %d",
				(unsigned char)(info->txt_buf[info->curr_buf_idx - 1])));
	    info->txt_buf[--(info->curr_buf_idx)] = '\0';
	}
	/* first byte (TODO: sanity check?) */
	info->txt_buf[--(info->curr_buf_idx)] = '\0';
    }
}


static Boolean
inside_text_match(int curr_pos, int from, int to)
{
    return from < curr_pos && curr_pos <= to;
}

static Boolean reinit_scan = False;

void
reinit_text_scan(void) {
    reinit_scan = True;
}

static Boolean
inside_bbox(int x, int y, int w, int h,
	    struct bbox *bboxes)
{
    int mid_x, mid_y;
#if 0
    XDrawRectangle(DISP, mane.win, globals.gc.high,
		   x / (double)currwin.shrinkfactor + 0.5,
		   y / (double)currwin.shrinkfactor + 0.5,
		   w / (double)currwin.shrinkfactor + 0.5,
		   h / (double)currwin.shrinkfactor + 0.5);
#endif
    /* already treat it as inside if there's an overlap of more than 50% */
    mid_x = x + w / 2.0;
    mid_y = y + h / 2.0;
#if 0
    fprintf(stderr, "------- check:\n%d, %d\n%d, %d, %d, %d\n",
	    mid_x, mid_y, bboxes->ulx, bboxes->uly, bboxes->lrx, bboxes->lry);
#endif
    if (bboxes->ulx <= mid_x &&
	bboxes->uly <= mid_y &&
	bboxes->lrx >= mid_x &&
	bboxes->lry >= mid_y) {
#if 0
	fprintf(stderr, "====== MATCH:\n%d, %d\n%d, %d, %d, %d\n",
		mid_x, mid_y, bboxes->ulx, bboxes->uly, bboxes->lrx, bboxes->lry);
#endif
	return True;
    }
    return False;
}

static Boolean
inside_bbox_line(int y, int h,
		 struct bbox *bboxes)
{
    int mid_y = y + h / 2.0;
    if (bboxes->uly <= mid_y &&
	bboxes->lry >= mid_y) {
#if 0
	fprintf(stderr, "MATCH:\n%d, %d, %d, %d\n%d, %d, %d, %d\n",
		x, y, w, h, bboxes->ulx, bboxes->uly, bboxes->lrx, bboxes->lry);
#endif
	return True;
    }
    return False;
}

static void
do_char(wide_ubyte ch,
	struct drawinf currinf,
	struct scan_info *info,
	long pxl_v1, long pxl_v2,
	long x1, long x2,
	struct glyph *g)
{
    struct word_info *w_info = (struct word_info *)info->data;
    struct search_settings *settings = NULL;
    const struct page_mapping *page_mapping = NULL;
    uint32_t u_glyph = 0, new_glyph = 0;
    Boolean convert_to_lowercase = False;
    /* if set to true, ignore whitespace before/after character (for CJK characters) */
    Boolean ignore_whitespace = False;
    
    /* default min space between words, in DVI units */
    /* TODO: for T1 fonts, should we look at fontdimen2 / fontdimen4?
       (e.g. tfminfo[fontmaps[currinf.fontp->t1id].tfmidx].fontdimen2)
    */
    long min_delta = (int)(1.5 * currinf.fontp->pixsize + 0.5) << 16;

    size_t buf_offset = w_info->buffer_offset;

    size_t i;
    /* for delayed insertion of newlines in text selection mode */
    static Boolean had_newline = False;
    /* for whitespace insertion in text selection mode */
    static Boolean had_chars_in_line = False;
    static long last_dvi_h1 = 0, last_pxl_v = 0;
    static long last_dvi_h2 = 0;
    static long last_x = 0;
    static uint32_t last_u_glyph = 0;
    static int last_page = -1;
    static int page_bak = -1;
    const char *expanded_lig = NULL;
    const uint32_t UNKNOWN_GLYPH_CODE = 0x003F; /* question mark */
    char retbuf[MAX_CHARS];
    retbuf[0] = '\0';
    
    if (w_info != NULL && w_info->settings != NULL) {
	settings = w_info->settings;
	if (!settings->case_sensitive)
	    convert_to_lowercase = True;
    }
    
    if (w_info->bbox_pass) {
	ASSERT(w_info != NULL, "");
	ASSERT(w_info->page_mapping != NULL, "");
	page_mapping = w_info->page_mapping;
    }
    
    if (reinit_scan || last_page != current_page) { /* reinit */
	last_dvi_h1 = last_pxl_v = last_dvi_h2 = last_x = 0;
	last_page = current_page;
	last_u_glyph = 0;
	w_info->bboxes_idx = 0;
	reset_bboxes(w_info);
	/* 	w_info->curr_buf_idx  = 0; */
	reinit_scan = False;
	had_newline = False;
	had_chars_in_line = False;
    }
    
    /*      TRACE_FIND((stderr, "\n--------- POSITIONS: %ld, %ld, %ld, %ld", x1, pxl_v2, x2, y2)); */

    if ((u_glyph = get_unicode_char(ch, currinf, retbuf)) == 0) {
	if (retbuf[0] == '\0') {
	    TRACE_FIND((stderr, "unknown glyph `%lu'\n", (unsigned long)ch));
	    u_glyph = UNKNOWN_GLYPH_CODE;
	}
	else /* several characters in retbuf, will be evaluated later */
	    u_glyph = 0;
    }

    TRACE_FIND_VERBOSE((stderr, "UNICODE: 0x%.4X; idx: %ld",
			(unsigned int)u_glyph,
			(unsigned long)w_info->curr_buf_idx));
    
    /* resize buffer if needed */
    while (w_info->curr_buf_idx + MAX_CHARS >= w_info->txt_buf_size) {
	w_info->txt_buf_size += ALLOC_STEP;
	w_info->txt_buf = xrealloc(w_info->txt_buf, w_info->txt_buf_size);
    }

    /* Copy text into buffer, applying heuristics for special glyphs.
       A `\0' is always appended, so that we have a valid C string;
       if we have more text, this '\0' will be overwritten in the next call. */

    /* apply accent/linebreak heuristics */
    /*     fprintf(stderr, "Checking: %ld > 0, %ld > 0\n", last_dvi_h1, last_pxl_v); */
    if (last_dvi_h1 > 0 && last_pxl_v > 0) { /* had at least 1 character */
	TRACE_FIND_VERBOSE((stderr, "++++ dvi_h: %ld, last_dvi_h1: %ld, w_info->curr_buf_idx: %ld",
			    DVI_H, last_dvi_h1, (unsigned long)w_info->curr_buf_idx));
	
	/* spaces after/before ideographic characters are ignored */
	if (is_ideograph(last_u_glyph) || is_ideograph(u_glyph))
	    ignore_whitespace = true;
	
	/* first, check for linebreaks since accents are also triggered by negative hspace.
	 * Usually, a linebreak is signalled by vertical movement down. However, in multicolumn
	 * mode, it can also be a movement up, which we try to catch with the second condition. */
	if (pxl_v2 > last_pxl_v + (int)(1.2 * currinf.fontp->pixsize + 0.5)
	    || (page_bak == current_page && pxl_v2 + (int)(6 * currinf.fontp->pixsize + 0.5) < last_pxl_v)) {
	    TRACE_FIND_VERBOSE((stderr, "linebreak (%ld > %ld + %d || %ld < %ld)!\n",
				pxl_v2, last_pxl_v, (int)(1.2 * currinf.fontp->pixsize + 0.5),
				pxl_v2 + (int)(6 * currinf.fontp->pixsize + 0.5), last_pxl_v));
	    
	    /* remove hyphen followed by newline if ignore_hyphens option is set,
	       and we're scanning for string search: */
	    if ((w_info->search_scan_pass || w_info->bbox_pass)
		&& settings->ignore_hyphens && w_info->curr_buf_idx > 0
		&& is_hyphenchar(last_u_glyph)) {
		
		erase_prev_char(w_info);
		TRACE_FIND_VERBOSE((stderr, "%d > %d + %d? offset: %lu",
				    settings->searchinfo->from_pos,
				    (int)w_info->curr_buf_idx,
				    settings->hyphen_delta, (unsigned long)buf_offset));
		if (!w_info->bbox_pass
		    && settings->searchinfo->from_pos
		    > (int)(w_info->curr_buf_idx + buf_offset + settings->hyphen_delta)) {
		    settings->hyphen_delta += 2;
		}
		TRACE_FIND((stderr, "erasing hyphen %lu at pos %lu with char %c; hyphen_delta is: %d",
			    (unsigned long)last_u_glyph, (unsigned long)w_info->curr_buf_idx, ch, settings->hyphen_delta));
		/* If the position of this hyphen had been at the start of the bounding
		   box, this bounding box info is now invalid - delete it */
		if (w_info->bbox_pass) {
		    int from, to;
		    map_index_positions(settings->searchinfo, page_mapping, &from, &to);
		    if ((int)w_info->curr_buf_idx == from) {
			delete_last_bbox(w_info);
		    }
		}
	    }
	    else { /* ignore_hyphens not set, insert newline */
		/* Also save hyphen_delta, in case we need to update from_pos and to_pos
		   if user switched from ignore hyphens to don't ignore hyphens */
		if (w_info->search_scan_pass && !w_info->bbox_pass
		    && is_hyphenchar(last_u_glyph)
		    && settings->searchinfo->from_pos + settings->hyphen_delta
		    >= (int)(w_info->curr_buf_idx + buf_offset)) {
		    settings->hyphen_delta += 2;
		    TRACE_FIND((stderr, "updating delta: %d at pos %d, curr_idx %d, offset: %lu",
				settings->hyphen_delta, settings->searchinfo->from_pos,
				(int)w_info->curr_buf_idx, (unsigned long)buf_offset));
		}
		if (w_info->text_selection_pass) {
		    if (inside_bbox(last_x, last_pxl_v, 5, 5, w_info->bboxes)
			|| (inside_bbox_line(last_pxl_v, 5, w_info->bboxes))) {
			had_newline = True;
			had_chars_in_line = False;
		    }
		}
		else if (w_info->search_scan_pass && settings->ignore_linebreaks && !ignore_whitespace)
		    w_info->txt_buf[w_info->curr_buf_idx++] = ' ';
		else if (!ignore_whitespace)
		    w_info->txt_buf[w_info->curr_buf_idx++] = '\n';
	    }
	    if (w_info->bbox_pass) {
		int from, to;
		map_index_positions(settings->searchinfo, page_mapping, &from, &to);
		if (inside_text_match((int)w_info->curr_buf_idx, from, to)) {
		    finish_bbox(w_info);
		}
	    }
	}
	else if (w_info->curr_buf_idx > 0 && last_u_glyph != 0 && last_x > 0
		 && (!w_info->text_selection_pass
		     || (w_info->text_selection_pass
			 && inside_bbox(last_x, pxl_v2, x1 - last_x, pxl_v1 - pxl_v2, w_info->bboxes)))
		 && x1 < last_x /* overlapping glyphs: check for diacritics */
		 && ((new_glyph = get_accented_glyph(last_u_glyph, u_glyph)) != 0)) {
	    erase_prev_char(w_info);
	    /* use new glyph for next writing operation: */
	    u_glyph = new_glyph;
	}
	else if (!ignore_whitespace && last_dvi_h2 > 0 && DVI_H > last_dvi_h2 + min_delta) {
	    TRACE_FIND_VERBOSE((stderr, "space (%ld > %ld + %ld)!", DVI_H, last_dvi_h2, min_delta));
	    if (!w_info->text_selection_pass
		|| (w_info->text_selection_pass
		    && had_chars_in_line
		    && inside_bbox(x1, pxl_v2, x2 - x1, g->bitmap.h, w_info->bboxes))) {
		w_info->txt_buf[w_info->curr_buf_idx++] = ' ';
		w_info->txt_buf[w_info->curr_buf_idx] = '\0';
		if (w_info->bbox_pass) {
		    int from, to;
		    map_index_positions(settings->searchinfo, page_mapping, &from, &to);
		    if (inside_text_match((int)w_info->curr_buf_idx, from, to)) {
			TRACE_FIND_VERBOSE((stderr, "bounding box for space: %ld, %ld, %ld, %ld",
					    last_x, last_pxl_v, x1 - last_x, pxl_v1 - pxl_v2));
			create_bbox(w_info, last_x, pxl_v2, x1 - last_x, pxl_v1 - pxl_v2);
		    }
		}
	    }
	}
    }

    last_page = current_page;
    if (retbuf[0] != '\0'
	|| (expanded_lig = expand_ligature(u_glyph)) != NULL
	/* when in search pass, normalize more characters */
	|| ((w_info->bbox_pass || w_info->search_scan_pass)
	    && (expanded_lig = search_normalize_chars(u_glyph)) != NULL)) {
	/* expanded ligature, which is always in 7-bit ASCII -> copy into buffer */
	size_t len;
	if (retbuf[0] != '\0')
	    expanded_lig = retbuf;
	len = strlen(expanded_lig);
	TRACE_FIND_VERBOSE((stderr, "I: %lu to %lu",
			    (unsigned long)w_info->curr_buf_idx,
			    (unsigned long)(w_info->curr_buf_idx + len)));

	if (!w_info->text_selection_pass
	    || (w_info->text_selection_pass && inside_bbox(x1, pxl_v2, x2 - x1, g->bitmap.h,
							   w_info->bboxes))) {

	    /* 	    fprintf(stderr, "inserting1 %s\n", expanded_lig); */
	    if (had_newline) {
		w_info->txt_buf[w_info->curr_buf_idx++] = '\n';
		had_newline = False;
	    }
	    memcpy(w_info->txt_buf + w_info->curr_buf_idx, expanded_lig, len);
	    had_chars_in_line = True;
	    for (i = 0; i < len; i++) {
		w_info->curr_buf_idx++;
		/* 		fprintf(stderr, "setting index to: %d\n", w_info->curr_buf_idx); */
		if (w_info->bbox_pass) {
		    int from, to;
		    map_index_positions(settings->searchinfo, page_mapping, &from, &to);
		    if (inside_text_match((int)w_info->curr_buf_idx, from, to)) {
			create_bbox(w_info, x1, pxl_v2, x2 - x1, g->bitmap.h);
		    }
		}
	    }
	    w_info->txt_buf[w_info->curr_buf_idx] = '\0';
	}
    }
    else if (!w_info->text_selection_pass
	     || (w_info->text_selection_pass && inside_bbox(x1, pxl_v2, x2 - x1, g->bitmap.h, w_info->bboxes))) {
	/* convert to utf8 */
	char utf8_buf[MAX_CHARS]; /* ample ... */
	size_t len;
	/* convert to utf8, eventually lowercasing */
	ucs4_to_utf8(u_glyph, utf8_buf, &len, convert_to_lowercase);
	/* 	fprintf(stderr, "inserting2 %lu\n", u_glyph); */
	if (had_newline) {
	    w_info->txt_buf[w_info->curr_buf_idx++] = '\n';
	    had_newline = False;
	}
	memcpy(w_info->txt_buf + w_info->curr_buf_idx, utf8_buf, len);
	had_chars_in_line = True;
	for (i = 0; i < len; i++) {
	    w_info->curr_buf_idx++;
	    /* 	    fprintf(stderr, "setting index2 to: %d\n", w_info->curr_buf_idx); */
	    if (w_info->bbox_pass) {
		int from, to;
#if 0
		int j;
		fprintf(stderr, "current page_mapping:\n");
		for (j = 0; j <= total_pages; j++) {
		    fprintf(stderr, "%d: %d\n", j, page_mapping[j]);
		}
#endif /* 0 */
		/* 		fprintf(stderr, "mapping!\n"); */
		map_index_positions(settings->searchinfo, page_mapping, &from, &to);
		if (inside_text_match((int)w_info->curr_buf_idx, from, to)) {
		    create_bbox(w_info, x1, pxl_v2, x2 - x1, g->bitmap.h);
		}
	    }
	}
	w_info->txt_buf[w_info->curr_buf_idx] = '\0'; /* ensure termination */
    }

    last_dvi_h1 = DVI_H;
    last_u_glyph = u_glyph;
    last_pxl_v = pxl_v1;
    last_dvi_h2 = DVI_H + currinf.fontp->glyph[ch].dvi_adv;
    last_x = x2;
}

/* handle a character in the text scanning routine */
long
text_do_char(FILE *fp, struct scan_info *info, wide_ubyte ch)
{
    if (currinf.set_char_p == set_no_char) {
	if (currinf.virtual == NULL
	    || (currinf.fontp = currinf.virtual->first_font) == NULL)
	    return 0;	/* error; we'll catch it later */
	maxchar = currinf.fontp->maxchar;
	currinf.set_char_p = currinf.fontp->set_char_p;
    }

    if (currinf.set_char_p == set_empty_char)
	return 0;	/* error; we'll catch it later */

    if (currinf.set_char_p == load_n_set_char) {
	if (globals.ev.flags & EV_GE_NEWDOC)	/* if abort */
	    return 0;
	if (!load_font(currinf.fontp, resource.t1lib
#if DELAYED_MKTEXPK
		       , True
#endif
		       )) {	/* if not found */
	    if (globals.ev.flags & EV_GE_NEWDOC)	/* if abort */
		return 0;

	    /* FIXME: replace by GUI warning! */
	    fputs("geom_do_char: Character(s) will be left blank.\n", stderr);
	    currinf.set_char_p = currinf.fontp->set_char_p = set_empty_char;
	    return 0;
	}
	maxchar = currinf.fontp->maxchar;
	currinf.set_char_p = currinf.fontp->set_char_p;
    }

    if (currinf.set_char_p == set_char) {
	struct glyph *g;
	long x, y;

	if (ch > maxchar)
	    return 0;	/* catch the error later */
	if ((g = &currinf.fontp->glyph[ch])->bitmap.bits == NULL) {
	    if (g->addr == 0)
		return 0;	/* catch the error later */
	    if (g->addr == -1)
		return 0;	/* previously flagged missing char */
	    open_font_file(currinf.fontp);
	    fseek(currinf.fontp->file, g->addr, SEEK_SET);
	    (*currinf.fontp->read_char) (currinf.fontp, ch);
	    if (globals.debug & DBG_BITMAP)
		print_char((ubyte) ch, g);
	    currinf.fontp->timestamp = ++current_timestamp;
	}
#ifdef TEXXET
	if (geom_scan_frame == NULL) {
	    long dvi_h_sav = DVI_H;
	    if (currinf.dir < 0)
		DVI_H -= g->dvi_adv;
#endif
	    x = G_PXL_H - g->x;
	    y = PXL_V - g->y;
	    do_char(ch, currinf, info, PXL_V, y, x, x + g->bitmap.w - 1, g);
#ifdef TEXXET
	    DVI_H = dvi_h_sav;
	}
#endif
	return DIR * g->dvi_adv;
    }
    else if (currinf.set_char_p == set_vf_char) {
	struct macro *m;
	struct drawinf oldinfo;
	wide_ubyte oldmaxchar;
#ifdef TEXXET
	long dvi_h_sav;
#endif

	if (ch > maxchar)
	    return 0;	/* catch the error later */
	if ((m = &currinf.fontp->macro[ch])->pos == NULL)
	    return 0;	/* catch the error later */
#ifdef TEXXET
	dvi_h_sav = DVI_H;
	if (currinf.dir < 0)
	    DVI_H -= m->dvi_adv;
	if (geom_scan_frame == NULL) {
#endif
	    oldinfo = currinf;
	    oldmaxchar = maxchar;
	    WW = XX = YY = ZZ = 0;
	    currinf.tn_table_len = VFTABLELEN;
	    currinf.tn_table = currinf.fontp->vf_table;
	    currinf.tn_head = currinf.fontp->vf_chain;
	    currinf.pos = m->pos;
	    currinf.end = m->end;
	    currinf.virtual = currinf.fontp;
	    geom_scan_part(text_do_char, fp, info, geom_current_frame, currinf.fontp->dimconv);
	    currinf = oldinfo;
	    maxchar = oldmaxchar;
#ifdef TEXXET
	    DVI_H = dvi_h_sav;
	}
#endif
	return DIR * m->dvi_adv;
    }
#ifdef T1LIB
    else if (currinf.set_char_p == set_t1_char) {
	struct glyph *g ;
	long x, y;
	t1FontLoadStatusT status;

#ifdef TEXXET
	g = get_t1_glyph(0, ch, &status, True);
	if (status == FAILURE_BLANK)
	    return 0;
	if (geom_scan_frame == NULL) {
	    long dvi_h_sav = DVI_H;
	    if (currinf.dir < 0)
		DVI_H -= g->dvi_adv;
	    x = G_PXL_H - g->x;
	    y = PXL_V - g->y;
	    do_char(ch, currinf, info, PXL_V, y, x, x + g->bitmap.w - 1, g);
	    DVI_H = dvi_h_sav;
	}
#else
	g = get_t1_glyph(ch, &status, True);
	if (status == FAILURE_BLANK)
	    return 0;
	x = G_PXL_H - g->x;
	y = PXL_V - g->y;
	do_char(ch, currinf, info, PXL_V, y, x, x + g->bitmap.w - 1, g);
#endif
	return DIR * g->dvi_adv;
    }
#endif /* T1LIB */
    else {
	XDVI_FATAL((stderr, "currinf.set_char_p not a registered routine!"));
    }
    /* NOTREACHED */
    return 0;
}

/*
 *	Handle a character in geometric scanning routine.
 */

static long
geom_do_char(FILE *fp, struct scan_info *info, wide_ubyte ch)
{
    struct geom_info *g_info = info->data;
    
    if (currinf.set_char_p == set_no_char) {
	if (currinf.virtual == NULL
	    || (currinf.fontp = currinf.virtual->first_font) == NULL)
	    return 0;	/* error; we'll catch it later */
	maxchar = currinf.fontp->maxchar;
	currinf.set_char_p = currinf.fontp->set_char_p;
    }

    if (currinf.set_char_p == set_empty_char)
	return 0;	/* error; we'll catch it later */

    if (currinf.set_char_p == load_n_set_char) {
	if (globals.ev.flags & EV_GE_NEWDOC)	/* if abort */
	    return 0;
	if (!load_font(currinf.fontp, resource.t1lib
#if DELAYED_MKTEXPK
		       , True
#endif
		       )) {	/* if not found */
	    if (globals.ev.flags & EV_GE_NEWDOC)	/* if abort */
		return 0;

	    /* FIXME: replace by GUI warning! */
	    fputs("geom_do_char: Character(s) will be left blank.\n", stderr);
	    currinf.set_char_p = currinf.fontp->set_char_p = set_empty_char;
	    return 0;
	}
	maxchar = currinf.fontp->maxchar;
	currinf.set_char_p = currinf.fontp->set_char_p;
    }

    if (currinf.set_char_p == set_char) {
	struct glyph *g;
	long x, y;

	if (ch > maxchar)
	    return 0;	/* catch the error later */
	if ((g = &currinf.fontp->glyph[ch])->bitmap.bits == NULL) {
	    if (g->addr == 0)
		return 0;	/* catch the error later */
	    if (g->addr == -1)
		return 0;	/* previously flagged missing char */
	    open_font_file(currinf.fontp);
	    fseek(currinf.fontp->file, g->addr, SEEK_SET);
	    (*currinf.fontp->read_char) (currinf.fontp, ch);
	    if (globals.debug & DBG_BITMAP)
		print_char((ubyte) ch, g);
	    currinf.fontp->timestamp = ++current_timestamp;
	}
#ifdef TEXXET
	if (geom_scan_frame == NULL) {
	    long dvi_h_sav = DVI_H;
	    if (currinf.dir < 0)
		DVI_H -= g->dvi_adv;
#endif
	    x = G_PXL_H - g->x;
	    y = PXL_V - g->y;
	    g_info->geom_box(info, x, y,
			     x + g->bitmap.w - 1, y + g->bitmap.h - 1);

#ifdef TEXXET
	    DVI_H = dvi_h_sav;
	}
#endif
	return DIR * g->dvi_adv;
    }
    else if (currinf.set_char_p == set_vf_char) {
	struct macro *m;
	struct drawinf oldinfo;
	wide_ubyte oldmaxchar;
#ifdef TEXXET
	long dvi_h_sav;
#endif

	if (ch > maxchar)
	    return 0;	/* catch the error later */
	if ((m = &currinf.fontp->macro[ch])->pos == NULL)
	    return 0;	/* catch the error later */
#ifdef TEXXET
	dvi_h_sav = DVI_H;
	if (currinf.dir < 0)
	    DVI_H -= m->dvi_adv;
	if (geom_scan_frame == NULL) {
#endif
	    oldinfo = currinf;
	    oldmaxchar = maxchar;
	    WW = XX = YY = ZZ = 0;
	    currinf.tn_table_len = VFTABLELEN;
	    currinf.tn_table = currinf.fontp->vf_table;
	    currinf.tn_head = currinf.fontp->vf_chain;
	    currinf.pos = m->pos;
	    currinf.end = m->end;
	    currinf.virtual = currinf.fontp;
	    geom_scan_part(geom_do_char, fp, info, geom_current_frame, currinf.fontp->dimconv);
	    currinf = oldinfo;
	    maxchar = oldmaxchar;
#ifdef TEXXET
	    DVI_H = dvi_h_sav;
	}
#endif
	return DIR * m->dvi_adv;
    }
#ifdef T1LIB
    else if (currinf.set_char_p == set_t1_char) {
	struct glyph *g ;
	long x, y;
	t1FontLoadStatusT status;

#ifdef TEXXET
	g = get_t1_glyph(0, ch, &status, False);
	if (status == FAILURE_BLANK)
	    return 0;
	if (geom_scan_frame == NULL) {
	    long dvi_h_sav = DVI_H;
	    if (currinf.dir < 0)
		DVI_H -= g->dvi_adv;
	    x = G_PXL_H - g->x;
	    y = PXL_V - g->y;
	    g_info->geom_box(info, x, y,
			     x + g->bitmap.w - 1, y + g->bitmap.h - 1);
	    DVI_H = dvi_h_sav;
	}
#else
	g = get_t1_glyph(ch, &status, False);
	if (status == FAILURE_BLANK)
	    return 0;
	x = G_PXL_H - g->x;
	y = PXL_V - g->y;
	g_info->geom_box(info, x, y,
			 x + g->bitmap.w - 1, y + g->bitmap.h - 1);
#endif
	return DIR * g->dvi_adv;
    }
#endif /* T1LIB */
    else {
	XDVI_FATAL((stderr, "currinf.set_char_p not a registered routine!"));
    }
    /* NOTREACHED */
    return 0;
}

/*
 *	Do a rule in the geometry-scanning routine.
 */

static void
geom_do_rule(struct scan_info *info, long h, long w)
{
    long x, y;
#ifdef TEXXET
    long dvi_h_save = DVI_H;
#endif
    struct geom_info *g_info = info->data;
    
#ifdef TEXXET
    if (currinf.dir < 0)
	DVI_H -= w - 1;
#endif
    x = G_PXL_H;
    y = PXL_V;
    g_info->geom_box(info, x, y - xpixel_round(h) + 1,
		     x + xpixel_round(w) - 1, y);
#ifdef TEXXET
    DVI_H = dvi_h_save;
#endif
}

/*
 *	Geometric dvi scanner work routine.  This does most of the work
 *	(a) reading from a page, and (b) executing vf macros.
 */

void
geom_scan_part(long(*char_proc)(FILE *, struct scan_info *, wide_ubyte),
	       FILE *fp, struct scan_info *info, struct frame *minframe, double current_dimconv)
{
    ubyte ch;
#ifdef TEXXET
    struct drawinf oldinfo;
    wide_ubyte oldmaxchar = 0;
    off_t file_pos = 0;
    int refl_count = 0;
#endif

    currinf.fontp = NULL;
    currinf.set_char_p = set_no_char;
#ifdef TEXXET
    currinf.dir = 1;
    geom_scan_frame = NULL;	/* indicates we're not scanning */
#endif
    for (;;) {
	ch = xone(fp);
	if (ch <= (ubyte)(SETCHAR0 + 127))
	    DVI_H += char_proc(fp, info, ch);
	else if (FNTNUM0 <= ch && ch <= (ubyte) (FNTNUM0 + 63)) {
	    change_font((unsigned long)(ch - FNTNUM0));
	}
	else {
	    long a, b;

	    switch (ch) {
	    case SET1:
	    case PUT1:
		a = char_proc(fp, info, xone(fp));
		if (ch != PUT1)
		    DVI_H += a;
		break;

	    case SET2:
	    case PUT2:
		if (!resource.omega)
		    dvi_fmt_error("%s:%d: draw_part: op-code %d only works with the \"-omega\" option",
				  __FILE__, __LINE__, ch);
		else {
#ifdef TEXXET
		    char_proc(fp, info, xnum(fp, 2));
#else
		    a = char_proc(fp, info, xnum(fp, 2));
		    if (ch != PUT2)
			DVI_H += a;
#endif
		}
		break;

	    case SETRULE:
		/* Be careful, dvicopy outputs rules with
		   height = 0x80000000.  We don't want any
		   SIGFPE here. */
		a = xsfour(fp);
		b = xspell_conv(xsfour(fp));
		if (a >= 0 && b >= 0
#ifdef TEXXET
		    && geom_scan_frame == NULL
#endif
		    ) {
		    /* is this a geom scan? */
		    if (info->geom_special != NULL)
			geom_do_rule(info, xspell_conv(a), b);
		}
		DVI_H += DIR * b;
		break;

	    case PUTRULE:
		a = xspell_conv(xsfour(fp));
		b = xspell_conv(xsfour(fp));
		if (a >= 0 && b >= 0
#ifdef TEXXET
		    && geom_scan_frame == NULL
#endif
		    ) {
		    /* is this a geom scan? */
		    if (info->geom_special != NULL)
			geom_do_rule(info, a, b);
		}
		break;

	    case NOP:
		break;

	    case BOP:
		xskip(fp, (long)11 * 4);
		DVI_H = G_OFFSET_X;
		DVI_V = G_OFFSET_Y;
		PXL_V = xpixel_conv(DVI_V);
		WW = XX = YY = ZZ = 0;
		/* create pagebreak in character scan */
		if (info->geom_special == NULL) {
		    do_newpage(info);
		}
		break;

	    case PUSH:
		if (geom_current_frame->next == NULL) {
		    struct frame *newp = xmalloc(sizeof *newp);

		    geom_current_frame->next = newp;
		    newp->prev = geom_current_frame;
		    newp->next = NULL;
		}
		geom_current_frame = geom_current_frame->next;
		geom_current_frame->data = currinf.data;
		break;

	    case POP:
		if (geom_current_frame == minframe)
		    dvi_fmt_error("more POPs than PUSHes");
		currinf.data = geom_current_frame->data;
		geom_current_frame = geom_current_frame->prev;
		break;

#ifdef TEXXET
	    case SREFL:
		if (geom_scan_frame == NULL) {
		    /* we're not scanning:  save some info. */
		    oldinfo = currinf;
		    oldmaxchar = maxchar;
		    if (!currinf.virtual)
			file_pos = xtell(fp, currinf.pos);
		    geom_scan_frame = geom_current_frame;	/* start scanning */
		    refl_count = 0;
		    break;
		}
		/* we are scanning */
		if (geom_current_frame == geom_scan_frame)
		    ++refl_count;
		break;

	    case EREFL:
		if (geom_scan_frame != NULL) {	/* if we're scanning */
		    if (geom_current_frame == geom_scan_frame
			&& --refl_count < 0) {
			/* we've hit the end of our scan */
			geom_scan_frame = NULL;
			/* first:  push */
			if (geom_current_frame->next == NULL) {
			    struct frame *newp = xmalloc(sizeof *newp);

			    geom_current_frame->next = newp;
			    newp->prev = geom_current_frame;
			    newp->next = NULL;
			}
			geom_current_frame = geom_current_frame->next;
			geom_current_frame->data = currinf.data;
			/* next:  restore old file position, XX, etc. */
			if (!currinf.virtual) {
			    off_t bgn_pos = xtell(fp, G_dvi_buf_ptr);

			    if (file_pos >= bgn_pos) {
				oldinfo.pos = dvi_buffer + (file_pos - bgn_pos);
				oldinfo.end = currinf.end;
			    }
			    else {
				(void)lseek(fileno(fp), file_pos,
					    SEEK_SET);
				oldinfo.pos = oldinfo.end;
			    }
			}
			currinf = oldinfo;
			maxchar = oldmaxchar;
			/* and then:  recover position info. */
			DVI_H = geom_current_frame->data.dvi_h;
			DVI_V = geom_current_frame->data.dvi_v;
			PXL_V = geom_current_frame->data.pxl_v;
			/* and finally, reverse direction */
			currinf.dir = -currinf.dir;
		    }
		    break;
		}
		/* we're not scanning, */
		/* so just reverse direction and then pop */
		currinf.dir = -currinf.dir;
		currinf.data = geom_current_frame->data;
		geom_current_frame = geom_current_frame->prev;
		break;
#endif /* TEXXET */

	    case RIGHT1:
	    case RIGHT2:
	    case RIGHT3:
	    case RIGHT4:
		DVI_H += DIR * xspell_conv(xsnum(fp, ch - RIGHT1 + 1));
		break;

	    case W1:
	    case W2:
	    case W3:
	    case W4:
		WW = xspell_conv(xsnum(fp, ch - W0));
	    case W0:
		DVI_H += DIR * WW;
		break;

	    case X1:
	    case X2:
	    case X3:
	    case X4:
		XX = xspell_conv(xsnum(fp, ch - X0));
	    case X0:
		DVI_H += DIR * XX;
		break;

	    case DOWN1:
	    case DOWN2:
	    case DOWN3:
	    case DOWN4:
		DVI_V += xspell_conv(xsnum(fp, ch - DOWN1 + 1));
		PXL_V = xpixel_conv(DVI_V);
		break;

	    case Y1:
	    case Y2:
	    case Y3:
	    case Y4:
		YY = xspell_conv(xsnum(fp, ch - Y0));
	    case Y0:
		DVI_V += YY;
		PXL_V = xpixel_conv(DVI_V);
		break;

	    case Z1:
	    case Z2:
	    case Z3:
	    case Z4:
		ZZ = xspell_conv(xsnum(fp, ch - Z0));
	    case Z0:
		DVI_V += ZZ;
		PXL_V = xpixel_conv(DVI_V);
		break;

	    case FNT1:
	    case FNT2:
	    case FNT3:
	    case FNT4:
		change_font(xnum(fp, ch - FNT1 + 1));
		break;

	    case XXX1:
	    case XXX2:
	    case XXX3:
	    case XXX4:
		a = xnum(fp, ch - XXX1 + 1);
		if (a > 0) {
		    char *str = read_special(fp, a);

		    /* is this a geom scan? */
		    if (info->geom_special != NULL) {
			/* process the bounding box */
			geom_do_special(info, str, current_dimconv);
			/* process the specials we're looking for */
			info->geom_special(info, str, a);
		    }
		}
		break;

	    case FNTDEF1:
	    case FNTDEF2:
	    case FNTDEF3:
	    case FNTDEF4:
		xskip(fp, (long)(12 + ch - FNTDEF1 + 1));
		a = (long)xone(fp);
		xskip(fp, a + (long)xone(fp));
		break;

#ifndef TEXXET
	    case SREFL:
	    case EREFL:
#endif
	    case PRE:
	    case POST:
	    case POSTPOST:
	    case EOP:
	    default:
		return;

	    }	/* end switch */
	}	/* end else (ch not a SETCHAR or FNTNUM) */
    }	/* end for */
}


/*
 *	Main scanning routine.
 */

void
geom_scan(long(*char_proc)(FILE *, struct scan_info *, wide_ubyte),
	  FILE *fp, struct scan_info *info, int pageno)
{
    volatile off_t pos_save = 0;
    struct drawinf currinf_save;
    ubyte maxchar_save;

#if PS
    if (info->geom_special != NULL && scanned_page < current_page) {
	fprintf(stderr, "shouldn't happen: %d >= %d!\n", scanned_page, current_page);
	return;	/* should not happen */
    }
#endif

    if (dvi_pointer_frame != NULL)
	pos_save = lseek(fileno(fp), 0L, SEEK_CUR) - (dvi_pointer_frame->end - dvi_pointer_frame->pos);

    (void) lseek(fileno(fp), pageinfo_get_offset(pageno), SEEK_SET);
    
    currinf_save = currinf;
    maxchar_save = maxchar;

    memset((char *)&currinf.data, '\0', sizeof currinf.data);
    currinf.tn_table_len = TNTABLELEN;
    currinf.tn_table = tn_table;
    currinf.tn_head = tn_head;
    currinf.pos = currinf.end = dvi_buffer;
    currinf.virtual = NULL;

    if (!setjmp(info->done_env)) {
	geom_scan_part(char_proc, fp, info, geom_current_frame = &geom_frame0, dimconv);
    }

    maxchar = maxchar_save;
    currinf = currinf_save;

    if (dvi_pointer_frame != NULL) {
	(void)lseek(fileno(fp), pos_save, SEEK_SET);
	dvi_pointer_frame->pos = dvi_pointer_frame->end = dvi_buffer;
    }
}


/*
 *	Routines for source special lookup
 */

struct src_spec_data {
    long x, y;				/* coordinates we're looking for */
    unsigned long distance;		/* best distance so far */
    Boolean recent_in_best;		/* most recent string == XXX->best */
    struct src_parsed_special best;	/* best special so far */
    struct src_parsed_special recent;	/* most recent special */
};

void
src_parse(const char *str, int str_len, struct src_parsed_special *parsed)
{
    const char *p = str;

    if (*p >= '0' && *p <= '9') {
	parsed->line = atoi(p);
	do {
	    ++p;
	    str_len--;
	}
	while (*p >= '0' && *p <= '9');
    }

    parsed->col = 0;
    if (*p == ':') {
	++p;
	str_len--;
	parsed->col = atoi(p);
	while (*p >= '0' && *p <= '9') {
	    ++p;
	    str_len--;
	}
    }

    if (*p == ' ') {
	++p;
	str_len--;
    }

    if (*p != '\0') {
	size_t len = str_len + 1;

	if (len > parsed->filename_len) {
	    if (parsed->filename_len != 0)
		free(parsed->filename);
	    parsed->filename_len = (len & -8) + 64; /* rounding? */
	    parsed->filename = xmalloc(parsed->filename_len);
	}
	memcpy(parsed->filename, p, len);
    }
}

static void
src_spec_box(struct scan_info *info, long ulx, long uly, long lrx, long lry)
{
    struct geom_info *g_info = (struct geom_info *)info->data;
    struct src_spec_data *data = g_info->geom_data;
    unsigned long distance;

    distance = 0;

    if (data->x < ulx)
	distance += (ulx - data->x) * (ulx - data->x);
    else if (data->x > lrx)
	distance += (data->x - lrx) * (data->x - lrx);

    if (data->y < uly)
	distance += (uly - data->y) * (uly - data->y);
    else if (data->y > lry)
	distance += (data->y - lry) * (data->y - lry);

    if (distance < data->distance) {
	data->distance = distance;

	/* Copy it over */
	if (!data->recent_in_best) {
	    data->best.line = data->recent.line;
	    data->best.col = data->recent.col;
	    if (data->recent.filename_len != 0) {
		if (data->best.filename_len < data->recent.filename_len) {
		    if (data->best.filename_len != 0)
			free(data->best.filename);
		    data->best.filename_len = data->recent.filename_len;
		    data->best.filename = xmalloc(data->best.filename_len);
		}
		memcpy(data->best.filename, data->recent.filename,
		       data->recent.filename_len);
	    }

	    data->recent_in_best = True;
	}

	/* Quit early if we've found our glyph.  */
	if (distance == 0 && data->best.filename_len != 0) {
	    longjmp(info->done_env, 1);
	}
    }
}

static void
src_spec_special(struct scan_info *info, const char *str, int str_len)
{
    struct geom_info *g_info = (struct geom_info *)info->data;
    struct src_spec_data *data = g_info->geom_data;

    if (memcmp(str, "src:", 4) != 0)
	return;

    src_parse(str + 4, str_len - 4, &data->recent);

    /*
     * If this is the first special on the page, we may already have
     * spotted the nearest box.
     */

    if (data->best.filename_len == 0) {
	data->best.line = data->recent.line;
	data->best.col = data->recent.col;
	if (data->recent.filename_len != 0) {
	    if (data->best.filename_len < data->recent.filename_len) {
		if (data->best.filename_len != 0)
		    free(data->best.filename);
		data->best.filename_len = data->recent.filename_len;
		data->best.filename = xmalloc(data->best.filename_len);
	    }
	    memcpy(data->best.filename, data->recent.filename,
		   data->recent.filename_len);

	    data->recent_in_best = True;
	}

	if (data->distance == 0) {
	    longjmp(info->done_env, 1);
	}
    }
    else
	data->recent_in_best = False;
}

/*
 *	Routines for reverse searches on other pages.
 */

static struct src_parsed_special found;
/* static jmp_buf scan_env; */

static Boolean
scan_first_src_spcl(char *str, int str_len, void *data)
{
    UNUSED(data);
    if (memcmp(str, "src:", 4) != 0)
	return False;

    src_parse(str + 4, str_len - 4, &found);

    return True;
}

static Boolean
scan_last_src_spcl(char *str, int str_len, void *data)
{
    UNUSED(data);
    if (memcmp(str, "src:", 4) != 0)
	return False;

    src_parse(str + 4, str_len - 4, &found);

    return True;
}


static void
src_spawn_editor(const struct src_parsed_special *parsed)
{
    char *expanded_filename;
    char **argv;
    const char *p;
    char *q;
    int i;

    struct stat buf;

    /* first, determine the editor if necessary */
    if (resource.editor == NULL || *resource.editor == '\0') {
	p = getenv("XEDITOR");
	if (p != NULL)
	    resource.editor = xstrdup(p);
	else {

	    p = getenv("VISUAL");
	    if (p == NULL) {
		p = getenv("EDITOR");
		if (p == NULL) {
		    popup_message(globals.widgets.top_level,
				  MSG_WARN,
				  /* help text */
				  "Use the \"-editor\" command-line opion, the X resource "
				  "\"xdvi.editor\" or one of the following environment variables "
				  "to select the editor for source specials: "
				  "\"XEDITOR\", \"VISUAL\" or \"EDITOR\".\n"
				  "See the xdvi man page for more information on source specials "
				  "and the editor options.",
				  /* message */
				  "No custom editor set - using vi as default.");
		    p = "vi";
		}
	    }
	    q = xmalloc(strlen(p) + 10);
	    memcpy(q, "xterm -e ", 9);
	    strcpy(q + 9, p);
	    resource.editor = q;
	}
    }

    /* now try to open the file; find_file allocates space for expanded_filename */
    if ((expanded_filename = find_file(parsed->filename, &buf, kpse_tex_format)) == NULL) {
	popup_message(globals.widgets.top_level,
		      MSG_ERR, NULL,
		      "File \"%s\" not found, couldn't jump to special\n\"%s:%d\"\n",
		      parsed->filename, parsed->filename, parsed->line);
    }
    else {
	TRACE_SRC((stderr, "source file \"%s\" expanded to \"%s\"\n", parsed->filename, expanded_filename));
	if (buf.st_mtime > globals.dvi_file.time) {
	    statusline_info(STATUS_FOREVER,
			     "Warning: TeX file is newer than dvi file - "
			     "source special information might be wrong.");
	}

	/* this allocates argv */
	argv = src_format_arguments(get_separated_list(resource.editor, " \t", True),
				    expanded_filename, parsed->line, parsed->col);
	
	fork_process(argv[0], False, NULL, NULL, NULL, argv);

	free(expanded_filename);
	for (i = 0; argv[i] != NULL; i++)
	    free(argv[i]);	
	free(argv);
    }
}

off_t
save_file_status(FILE *fp, struct drawinf *currinf_save, ubyte *maxchar_save)
{
    off_t pos_save = 0;
    if (dvi_pointer_frame != NULL)
	pos_save = lseek(fileno(fp), 0L, SEEK_CUR)
	    - (dvi_pointer_frame->end - dvi_pointer_frame->pos);
    
    *currinf_save = currinf;
    *maxchar_save = maxchar;
    return pos_save;
}

void
restore_file_status(FILE *fp, struct drawinf currinf_save, ubyte maxchar_save, off_t pos_save)
{
    maxchar = maxchar_save;
    currinf = currinf_save;
    
    if (dvi_pointer_frame != NULL) {
	(void)lseek(fileno(fp), pos_save, SEEK_SET);
	dvi_pointer_frame->pos = dvi_pointer_frame->end = dvi_buffer;
    }
}



/*
 *	The main routine for source specials (reverse search).
 */
void
source_reverse_search(int x, int y, wide_bool call_editor)
{
    struct scan_info info;
    struct geom_info g_info;
    struct src_spec_data data;
    struct src_parsed_special *foundp;
    
    info.geom_special = src_spec_special;

    g_info.geom_box = src_spec_box;
    g_info.geom_data = &data;

    info.data = &g_info;
    
    data.x = x;
    data.y = y;
    data.distance = 0xffffffff;
    data.recent_in_best = True;
    data.best.filename_len = data.recent.filename_len = 0;
    foundp = &data.best;

    geom_scan(geom_do_char, globals.dvi_file.bak_fp, &info, current_page);

    if (data.best.filename_len == 0) {
	/*
	 * nothing found on current page;
	 * scan next and previous pages with increasing offset
	 */
	volatile int upper, lower;
	volatile off_t pos_save;
	struct drawinf currinf_save;
	ubyte maxchar_save;

	/* Save file position */
	pos_save = save_file_status(globals.dvi_file.bak_fp, &currinf_save, &maxchar_save);
	
	upper = lower = current_page;
	found.filename_len = 0;	/* mark it as empty */
	for (;;) {
	    if (++upper < total_pages) {
		(void)lseek(fileno(globals.dvi_file.bak_fp), pageinfo_get_offset(upper), SEEK_SET);
		memset((char *)&currinf.data, '\0', sizeof currinf.data);
		currinf.tn_table_len = TNTABLELEN;
		currinf.tn_table = tn_table;
		currinf.tn_head = tn_head;
		currinf.pos = currinf.end = dvi_buffer;
		currinf.virtual = NULL;

		if (spcl_scan(scan_first_src_spcl, NULL, True, globals.dvi_file.bak_fp)) {
		    lower = upper;
		    break;
		}
	    }
	    else if (lower < 0)
		break;

	    if (--lower >= 0) {
		(void)lseek(fileno(globals.dvi_file.bak_fp), pageinfo_get_offset(lower), SEEK_SET);

		memset((char *)&currinf.data, '\0', sizeof currinf.data);
		currinf.tn_table_len = TNTABLELEN;
		currinf.tn_table = tn_table;
		currinf.tn_head = tn_head;
		currinf.pos = currinf.end = dvi_buffer;
		currinf.virtual = NULL;

		(void)spcl_scan(scan_last_src_spcl, NULL, False, globals.dvi_file.bak_fp);
		if (found.filename_len != 0)
		    break;
	    }
	}

	if (found.filename_len != 0)
	    statusline_info(STATUS_MEDIUM,
			     "No source specials on this page - nearest on page %d",
			     lower + globals.pageno_correct);
	else {
	    /* nothing found at all; complain */
	    xdvi_bell();
	    popup_message(globals.widgets.top_level,
			  MSG_ERR,
			  /* helptext */
			  reverse_search_helptext,
			  /* popup */
			  "No source specials in this DVI file - couldn't do reverse search.");
	}

	/* Restore file status.  */
	restore_file_status(globals.dvi_file.bak_fp, currinf_save, maxchar_save, pos_save);

	foundp = &found;
    }

    if (data.recent.filename_len != 0)
	free(data.recent.filename);

    if (foundp->filename_len != 0) {
	if (call_editor) {
	    src_spawn_editor(foundp);
	}
	else {
	    statusline_info(STATUS_MEDIUM, "nearest special at (%d,%d): \"%s:%d\"",
			     x / currwin.shrinkfactor, y / currwin.shrinkfactor, foundp->filename, foundp->line);
	}
	free(foundp->filename);
    }
}


/*
 *	Debug routines for source special display (highlight the first box
 *	after each source special, or highlight each box).
 */

struct src_spec_show_data {
    Boolean do_this_one;	/* flag set after source special */
    Boolean do_them_all;	/* flag to reset the above to */
};

static void
src_spec_show_box(struct scan_info *info, long ulx, long uly, long lrx, long lry)
{
    struct geom_info *g_info = (struct geom_info *)info->data;
    struct src_spec_show_data *data = g_info->geom_data;

    if (data->do_this_one) {
	long x = ulx / mane.shrinkfactor;
	long y = uly / mane.shrinkfactor;

	XDrawRectangle(DISP, mane.win, globals.gc.high,
		       x - mane_base_x, y - mane_base_y,
		       lrx / mane.shrinkfactor - x,
		       lry / mane.shrinkfactor - y);
	data->do_this_one = data->do_them_all;
    }
}

static void
src_spec_show_special(struct scan_info *info, const char *str, int str_len)
{
    /*     if (memcmp(str, "src:", 4) != 0) */
    /* 	return; */
    struct geom_info *g_info = (struct geom_info *)info->data;
    UNUSED(str_len);
    XDVI_INFO((stdout, "special: %s", str));
    ((struct src_spec_show_data *)g_info->geom_data)->do_this_one = True;
}

void
source_special_show(wide_bool do_them_all)
{
    struct scan_info info;
    struct geom_info g_info;
    struct src_spec_show_data src_data;

    info.geom_special = src_spec_show_special;

    g_info.geom_box = src_spec_show_box;
    g_info.geom_data = &src_data;

    info.data = &g_info;
    
    src_data.do_this_one = src_data.do_them_all = do_them_all;

    geom_scan(geom_do_char, globals.dvi_file.bak_fp, &info, current_page);
}


/*
 *	Routines for forward search (look up a source line).
 *
 *	The general procedure is:
 *	   (1)	Use spcl_scan() to find the page containing the line (or at
 *		least the closest line).  This could be optimized further.
 *	   (2)	Use geom_scan_part() to find the exact location of the source
 *		special, and the box to highlight.
 */

/* These variables are referenced by src_scan_special().  */

static int src_this_line;
static Boolean src_this_file_equal;
static int src_line;
static int src_col;
static const char *src_file;
static int src_page;
/* static jmp_buf src_env; */
static Boolean found_src;
static unsigned long best_distance;
static unsigned long best_col_dist;
static int best_line;
static int best_page;
static off_t best_offset;
static off_t max_offset;

/* Some of the above, plus these below, are used by geom_scan_part().  */

static Boolean src_fwd_active;

#define BBOX_INFO_MAXIDX 8 /* maximum number of bounding boxes */

/* list of bounding boxes */
struct bbox_info {
    long min_x, max_x, min_y, max_y;
    long spcl_min_x, spcl_max_x, spcl_min_y, spcl_max_y;
} g_bbox_info[BBOX_INFO_MAXIDX];

/* current index in g_bbox_info[] */
static size_t bbox_info_idx = 0;

static Boolean
src_scan_special(char *str, int str_len, void *data)
{
    char *p;
    int col = 0;
    unsigned long distance;
    unsigned long col_dist;
    
    UNUSED(data);
    if (memcmp(str, "src:", 4) != 0)
	return False;

    found_src = True;

    p = str + 4;
    str_len -= 4;
    
    if (*p >= '0' && *p <= '9') {
	src_this_line = atoi(p);
	do {
	    ++p;
	    str_len--;
	}
	while (*p >= '0' && *p <= '9');
    }

    if (*p == ':') {
	++p;
	str_len--;
	col = atoi(p);
	while (*p >= '0' && *p <= '9') {
	    ++p;
	    str_len--;
	}
    }

    if (*p == ' ') {
	++p;
	str_len--;
    }

    /* src specials might omit the file name when it had already
       been specified on the page; so skip the test when the
       special ends right here:
    */
    if (*p != '\0') {
	ASSERT(globals.dvi_file.dirname != NULL, "DVI name should be set here");
	src_this_file_equal = src_compare(p, str_len, src_file, globals.dvi_file.dirname, globals.dvi_file.dirlen);
    }
    
    if (!src_this_file_equal)
	return False;
    
    distance = (src_line > src_this_line
		? src_line - src_this_line
		: 2 * (src_this_line - src_line));	/* favor earlier lines */

    if (distance < best_distance) {	/* found a better line */
	best_distance = distance;
	best_line = src_this_line;
	best_page = src_page;
	max_offset = best_offset = xtell(globals.dvi_file.bak_fp, currinf.pos);
    }
    else if (distance == best_distance) { /* still on a good line, remember diff */
	max_offset = xtell(globals.dvi_file.bak_fp, currinf.pos);
    }

    if (distance == 0 && best_distance == 0) {	/* found a better col */
	col_dist = (src_col > col ? src_col - col : 2 * (col - src_col));

	if (col_dist < best_col_dist) {
	    best_col_dist = col_dist;
	    best_page = src_page;
	    max_offset = best_offset = xtell(globals.dvi_file.bak_fp, currinf.pos);
	}
	else if (col_dist == best_col_dist) {
	    max_offset = xtell(globals.dvi_file.bak_fp, currinf.pos);
	}
    }
    return True;
}

static Boolean
htex_scan_special(char *str, int str_len, void *data)
{
    UNUSED(data);

    if (g_anchor_pos == NULL)
	return False;
    
    /* first, hypertex specials */
    if (memicmp(str, "html:", strlen("html:")) == 0) {
	size_t offset = strlen("html:");
	str += offset;
	str_len -= offset;
	
	while (*str == ' ' || *str == '\t') {
	    str++;
	    str_len--;
	}

	if (memicmp(str, "<a name", strlen("<a name")) != 0)
	    return False;

	offset = strlen("<a name");
	str += offset;
	str_len -= offset;
	str_len--; /* for the closing > */
	
	/* skip over spaces, = and open quotes */
	while (*str == ' ' || *str == '=') {
	    str++;
	    str_len--;
	}
	if (*str == '"') {
	    str++;
	    str_len -= 2; /* don't compare closing quote */
	}

	/* fprintf(stderr, "comparing: |%s|%s|%d\n", str, g_anchor_pos, MAX((size_t)str_len, g_anchor_len)); */
	if (memcmp(str, g_anchor_pos, MAX((size_t)str_len, g_anchor_len)) == 0)
	    return True;
	else
	    return False;
    }
    /* then, hdvips specials */
    else if (memicmp(str, "ps:", 3) == 0 && g_anchor_pos != NULL) {
	str += 3;
	if (memicmp(str, "sdict begin ", strlen("sdict begin ")) == 0) {
	    char *ptr = NULL, *pptr = NULL;
	    str += strlen("sdict begin ");
	    if ((ptr = strstr(str, "/View")) != NULL
		&& (ptr = strchr(ptr, '(')) != NULL
		&& (pptr = strchr(ptr + 1, ')')) != NULL) {
		if (memcmp(ptr + 1, g_anchor_pos, pptr - ptr - 1) == 0
		    && g_anchor_pos[pptr - ptr - 1] == '\0') {
		    return True;
		}
	    }
	}
	return False;
    }
    else {
	return False;
    }
}

static void
htex_scan_special_noreturn(struct scan_info *info, const char *str, int str_len)
{
    struct geom_info *g_info = (struct geom_info *)info->data;

    /* first, hypertex specials */
    if (memcmp(str, "html:", strlen("html:")) == 0) {
	size_t offset = strlen("html:");
	str += offset;
	str_len -= offset;

	while (*str == ' ' || *str == '\t') {
	    str++;
	    str_len--;
	}
	
	if (memicmp(str, "<a name", strlen("<a name")) != 0)
	    return;
	
	offset = strlen("<a name");
	str += offset;
	str_len -= offset;
	str_len--; /* for the closing > */
	
	/* skip over spaces, = and open quotes */
	while (*str == ' ' || *str == '=') {
	    str++;
	    str_len--;
	}
	if (*str == '"') {
	    str++;
	    str_len -= 2; /* don't compare closing quote */
	}

	if (g_anchor_pos != NULL
	    && memcmp(str, g_anchor_pos, MAX((size_t)str_len, g_anchor_len)) == 0) {
	    *((int *)g_info->geom_data) = pixel_conv(DVI_V);
	    longjmp(info->done_env, 1);
	}
    }
    /* then, hdvips specials */
    else if (memicmp(str, "ps:", 3) == 0 && g_anchor_pos != NULL) {
	str += 3;
	if (memicmp(str, "sdict begin ", strlen("sdict begin ")) == 0) {
	    char *ptr = NULL, *pptr = NULL;
	    str += strlen("sdict begin ");
	    if ((ptr = strstr(str, "/View")) != NULL
		&& (ptr = strchr(ptr, '(')) != NULL
		&& (pptr = strchr(ptr + 1, ')')) != NULL) {
		if (memcmp(ptr + 1, g_anchor_pos, pptr - ptr - 1) == 0
		    && g_anchor_pos[pptr - ptr - 1] == '\0') {
		    *((int *)g_info->geom_data) = pixel_conv(DVI_V);
		    longjmp(info->done_env, 1);
		}
	    }
	}
    }
}

static void
src_spec_fwd_box(struct scan_info *info, long ulx, long uly, long lrx, long lry)
{
    /* Heuristic value to detect column breaks: A negative vertical
       offset to the next glyph larger than BBOX_OFF will create a new
       bounding box. The amount is in unshrunken pixels, i.e. 120
       \approx 1 Line of 12pt Text. */
    static const int BBOX_OFF = 360;
    
    UNUSED(info);
    if (src_fwd_active) {
	/* probably a column break, create new box */
	if (lry < g_bbox_info[bbox_info_idx].max_y - BBOX_OFF
	    && lrx > g_bbox_info[bbox_info_idx].max_x + 50) {
	    if (bbox_info_idx < BBOX_INFO_MAXIDX - 1) {
		bbox_info_idx++;

		g_bbox_info[bbox_info_idx].min_y = g_bbox_info[bbox_info_idx].min_x = LONG_MAX;
		g_bbox_info[bbox_info_idx].max_x = g_bbox_info[bbox_info_idx].max_y = 0;
	    }
	}
	if (ulx < g_bbox_info[bbox_info_idx].min_x) {
	    g_bbox_info[bbox_info_idx].min_x = ulx;
	}
	if (uly < g_bbox_info[bbox_info_idx].min_y) {
	    g_bbox_info[bbox_info_idx].min_y = uly;
	}
	if (lrx > g_bbox_info[bbox_info_idx].max_x)
	    g_bbox_info[bbox_info_idx].max_x = lrx;
	if (lry > g_bbox_info[bbox_info_idx].max_y) {
	    g_bbox_info[bbox_info_idx].max_y = lry;
	}
    }
}

/* dummy procedure for hyperlinks; we don't need geometry info for these. */
static void
htex_dummy_box(struct scan_info *info, long ulx, long uly, long lrx, long lry)
{
    UNUSED(info);
    UNUSED(ulx);
    UNUSED(uly);
    UNUSED(lrx);
    UNUSED(lry);
    return;
}

static void
src_spec_fwd_special(struct scan_info *info, const char *str, int str_len)
{
    long pos;
    UNUSED(str_len);
    
    if (memcmp(str, "src:", 4) != 0)	/* if no source special */
	return;

    pos = xtell(globals.dvi_file.bak_fp, currinf.pos);
    if (pos >= best_offset)
	src_fwd_active = True;

    if (src_fwd_active) {
	if (pos > max_offset)
	    longjmp(info->done_env, 1);
	
	if (G_PXL_H < g_bbox_info[bbox_info_idx].spcl_min_x)
	    g_bbox_info[bbox_info_idx].spcl_min_x = G_PXL_H;
	if (G_PXL_H > g_bbox_info[bbox_info_idx].spcl_max_x)
	    g_bbox_info[bbox_info_idx].spcl_max_x = G_PXL_H;
	if (PXL_V < g_bbox_info[bbox_info_idx].spcl_min_y)
	    g_bbox_info[bbox_info_idx].spcl_min_y = PXL_V;
	if (PXL_V > g_bbox_info[bbox_info_idx].spcl_max_y)
	    g_bbox_info[bbox_info_idx].spcl_max_y = PXL_V;

    }
}

/*
 *	Routine to actually draw the box.
 */

static void
source_fwd_draw_box(void)
{
    if (globals.src.fwd_box_page != current_page) {
	globals.src.fwd_box_page = -1;	/* different page---clear it */
    }
    else {
	size_t i;
	static const int padding = 15;

	/* fix oversplitting by heuristics, by merging box n with box n+1 if they overlap */
	for (i = 0; i < bbox_info_idx; i++) {
	    if (g_bbox_info[i].min_x == LONG_MAX)
		continue;
	    if ((g_bbox_info[i+1].min_x < g_bbox_info[i].max_x
		 && g_bbox_info[i+1].min_y < g_bbox_info[i].max_y) ||
		(g_bbox_info[i].min_x < g_bbox_info[i+1].max_x
		 && g_bbox_info[i].min_y < g_bbox_info[i+1].max_y)) { /* overlap */		
		g_bbox_info[i].min_x = MIN(g_bbox_info[i].min_x, g_bbox_info[i+1].min_x);
		g_bbox_info[i].min_y = MIN(g_bbox_info[i].min_y, g_bbox_info[i+1].min_y);
		g_bbox_info[i].max_x = MAX(g_bbox_info[i].max_x, g_bbox_info[i+1].max_x);
		g_bbox_info[i].max_y = MAX(g_bbox_info[i].max_y, g_bbox_info[i+1].max_y);
		bbox_info_idx--;
	    }
	}
	
	for (i = 0; i <= bbox_info_idx; i++) {
	    int min_x, min_y, max_x, max_y;
	    
	    volatile XPoint ul, ur, ll, lr;
	    
	    if (g_bbox_info[i].min_x == LONG_MAX) {
		/* If no glyphs or rules, use hot point of special instead.  */
		g_bbox_info[i].min_x = g_bbox_info[i].spcl_min_x;
		g_bbox_info[i].min_y = g_bbox_info[i].spcl_min_y;
		g_bbox_info[i].max_x = g_bbox_info[i].spcl_max_x;
		g_bbox_info[i].max_y = g_bbox_info[i].spcl_max_y;
	    }

	    min_x = (g_bbox_info[i].min_x - padding) / mane.shrinkfactor - mane_base_x;
	    min_y = (g_bbox_info[i].min_y - padding) / mane.shrinkfactor - mane_base_y;
	    max_x = (g_bbox_info[i].max_x + padding) / mane.shrinkfactor - mane_base_x;
	    max_y = (g_bbox_info[i].max_y + padding) / mane.shrinkfactor - mane_base_y;

	    ul.x = min_x;
	    ul.y = min_y;

	    ur.x = max_x;
	    ur.y = min_y;

	    ll.x = min_x;
	    ll.y = max_y;

	    lr.x = max_x;
	    lr.y = max_y;
	    
	    if (i == 0 && bbox_info_idx == 0) { /* only 1 bounding box */
		/* (2009-08-23) XDrawRectangle is broken in some X
		   implementations (see sourceforge bug #2841005),
		   so use XDrawLines instead
		   XDrawRectangle(DISP, mane.win, globals.gc.high,
		                  min_x, min_y, max_x - min_x, max_y - min_y);
		*/
		XPoint points[5];
		points[0] = ll;
		points[1] = ul;
		points[2] = ur;
		points[3] = lr;
		points[4] = ll;
		XDrawLines(DISP, mane.win, globals.gc.high, points, 5, CoordModeOrigin);
	    }
	    else if (i == 0) { /* draw first bbox with open bottom */
		XPoint points[4];
		points[0] = ll;
		points[1] = ul;
		points[2] = ur;
		points[3] = lr;
		XDrawLines(DISP, mane.win, globals.gc.high, points, 4, CoordModeOrigin);
	    }
	    else if (i > 0 && i < bbox_info_idx) { /* draw middle box with open top and open bottom */
		XPoint points[2];
		points[0] = ul;
		points[1] = ll;
		XDrawLines(DISP, mane.win, globals.gc.high, points, 2, CoordModeOrigin);
		points[0] = ur;
		points[1] = lr;
		XDrawLines(DISP, mane.win, globals.gc.high, points, 2, CoordModeOrigin);
	    }
	    else { /* draw last box with open top */
		XPoint points[4];
		points[0] = ul;
		points[1] = ll;
		points[2] = lr;
		points[3] = ur;
		XDrawLines(DISP, mane.win, globals.gc.high, points, 4, CoordModeOrigin);
	    }
	}
    }
}

#if 0
#include<asm/msr.h>
unsigned long time_start=0, time_end=0;
#endif

void
source_forward_search(const char *str)
{
    volatile off_t pos_save = 0;
    struct drawinf currinf_save;
    ubyte maxchar_save;
    struct scan_info info;
    struct geom_info g_info;
    
    TRACE_CLIENT((stderr, "Entering source_forward_search(%s)", str));

    max_offset = 0;
    src_file = str;
    while (*src_file == '0')
	++src_file;
    if (*src_file < '1' || *src_file > '9') {
	XDVI_WARNING((stderr, "Ignoring malformed source special \"%s\"", str));
	return;
    }
    src_line = atoi(src_file);
    while (*src_file >= '0' && *src_file <= '9')
	++src_file;

    src_col = 0;
    if (*src_file == ':') {
	++src_file;
	src_col = atoi(src_file);
	while (*src_file >= '0' && *src_file <= '9')
	    ++src_file;
    }

    if (*src_file == ' ')
	++src_file;

    TRACE_CLIENT((stderr, "File = \"%s\", line = %d, col = %d", src_file, src_line, src_col));

    /* Save status of dvi_file reading (in case we hit an error and resume
       drawing).  */

    if (dvi_pointer_frame != NULL)
	pos_save = lseek(fileno(globals.dvi_file.bak_fp), 0L, SEEK_CUR) - (dvi_pointer_frame->end - dvi_pointer_frame->pos);
    (void)lseek(fileno(globals.dvi_file.bak_fp), pageinfo_get_offset(0), SEEK_SET);

    currinf_save = currinf;
    maxchar_save = maxchar;

    memset((char *)&currinf.data, '\0', sizeof currinf.data);
    currinf.tn_table_len = TNTABLELEN;
    currinf.tn_table = tn_table;
    currinf.tn_head = tn_head;
    currinf.pos = currinf.end = dvi_buffer;
    currinf.virtual = NULL;

    /* Start search over pages */
#if 0
    rdtscl(time_start);
#endif

    found_src = False;
    best_distance = best_col_dist = ULONG_MAX;
    src_this_line = 0;	/* initialize things that are kept as defaults */
    src_this_file_equal = False;

    /* These two lines do the actual scanning (first pass).  */
    for (src_page = 0; src_page < total_pages; ++src_page)
	(void)spcl_scan(src_scan_special, NULL, False, globals.dvi_file.bak_fp);

    if (best_distance == ULONG_MAX) {
	if (!found_src) {
	    popup_message(globals.widgets.top_level,
			  MSG_WARN,
			  /* helptext */
			  reverse_search_helptext,
			  /* popup */
			  "No source specials in this DVI file - couldn't do reverse search.");
	}
	else {
	    popup_message(globals.widgets.top_level,
			  MSG_WARN,
			  /* helptext */
			  "To enable reverse search, the TeX file has to be compiled with source specials. "
			  "See the xdvi man page (section SOURCE SPECIALS) for details.",
			  /* popup */
			  "No references to source file \"%s\" in DVI file.",
			  src_file);
	}

	/* Restore file position.  */
	maxchar = maxchar_save;
	currinf = currinf_save;

	if (dvi_pointer_frame != NULL) {
	    (void)lseek(fileno(globals.dvi_file.bak_fp), pos_save, SEEK_SET);
	    dvi_pointer_frame->pos = dvi_pointer_frame->end = dvi_buffer;
	}

	return;
    }
    TRACE_CLIENT((stderr, "Match:  line %d on page %d, offset %lu",
		  best_line, best_page + globals.pageno_correct, (unsigned long)best_offset));

    /*
     * In this case we don't need to restore maxchar and currinf, since
     * we won't resume drawing -- we'll jump to a new page instead.
     */

    /* Move to that page.  */
    goto_page(best_page, resource.keep_flag ? NULL : home, False);
    page_history_insert(best_page);
    globals.ev.flags |= EV_NEWPAGE; /* so that existing mark gets erased */
    /* Now search that particular page.  */

    info.geom_special = src_spec_fwd_special;

    g_info.geom_box = src_spec_fwd_box;
    g_info.geom_data = NULL;

    src_fwd_active = False;
    bbox_info_idx = 0;

    g_bbox_info[bbox_info_idx].min_x =
	g_bbox_info[bbox_info_idx].min_y =
	g_bbox_info[bbox_info_idx].spcl_min_x =
	g_bbox_info[bbox_info_idx].spcl_min_y = LONG_MAX;
    g_bbox_info[bbox_info_idx].max_x =
	g_bbox_info[bbox_info_idx].max_y =
	g_bbox_info[bbox_info_idx].spcl_max_x =
	g_bbox_info[bbox_info_idx].spcl_max_y = 0;
    globals.src.fwd_box_page = -1;	/* in case of error, suppress box */

    (void)lseek(fileno(globals.dvi_file.bak_fp), pageinfo_get_offset(best_page), SEEK_SET);
    currinf.tn_table_len = TNTABLELEN;
    currinf.tn_table = tn_table;
    currinf.tn_head = tn_head;
    currinf.pos = currinf.end = dvi_buffer;
    currinf.virtual = NULL;

    info.data = &g_info;
    
    if (!setjmp(info.done_env))
	geom_scan_part(geom_do_char, globals.dvi_file.bak_fp, &info, geom_current_frame = &geom_frame0, dimconv);

    if (!src_fwd_active) {
	XDVI_ERROR((stderr, "%s:%d: shouldn't happen: geom_scan_part() failed to re-find the special.", __FILE__, __LINE__));
    }
    else {
	long x_min = g_bbox_info[bbox_info_idx].min_x;
	long y_min = g_bbox_info[bbox_info_idx].min_y;
	long x_max = g_bbox_info[bbox_info_idx].max_x;
	long y_max = g_bbox_info[bbox_info_idx].max_y;
	do_autoscroll = True;
	globals.src.fwd_box_page = current_page;
	if (x_min == LONG_MAX || x_max == LONG_MAX) {
	    /* If no glyphs or rules, use hot point of special instead.  */
	    x_min = g_bbox_info[bbox_info_idx].spcl_min_x;
	    y_min = g_bbox_info[bbox_info_idx].spcl_min_y;
	    x_max = g_bbox_info[bbox_info_idx].spcl_max_x;
	    y_max = g_bbox_info[bbox_info_idx].spcl_max_y;
	}
	scroll_page_if_needed((int)(x_min / currwin.shrinkfactor) + 2, (int)(x_max / currwin.shrinkfactor) - 2,
			      (int)(y_min / currwin.shrinkfactor) + 10, (int)(y_max / currwin.shrinkfactor) - 10);
    }
#if 0
    rdtscl(time_end);
    printf("time search: %lu\n", time_end - time_start);
#endif
}


void
anchor_search(const char *str)
{
    off_t pos_save = 0;
    struct drawinf currinf_save;
    ubyte maxchar_save;
    volatile int test_page = 0;
    Boolean found_anchor = False;
    int y_pos = -1;
    struct scan_info info;
    struct geom_info g_info;

    ASSERT(str != NULL, "Argument to anchor_search() musn't be NULL");
    TRACE_HTEX((stderr, "Entering anchor_search(%s)", str));

    /* Save status of dvi_file reading (in case we hit an error and resume drawing).  */
    if (dvi_pointer_frame != NULL)
	pos_save = lseek(fileno(globals.dvi_file.bak_fp), 0L, SEEK_CUR) - (dvi_pointer_frame->end - dvi_pointer_frame->pos);
    (void)lseek(fileno(globals.dvi_file.bak_fp), pageinfo_get_offset(0), SEEK_SET);

    currinf_save = currinf;
    maxchar_save = maxchar;

    memset((char *)&currinf.data, '\0', sizeof currinf.data);
    currinf.tn_table_len = TNTABLELEN;
    currinf.tn_table = tn_table;
    currinf.tn_head = tn_head;
    currinf.pos = currinf.end = dvi_buffer;
    currinf.virtual = NULL;

    /* Start search over pages */
    for (test_page = 0; test_page < total_pages; test_page++) {
	if (spcl_scan(htex_scan_special, NULL, True, globals.dvi_file.bak_fp)) {
	    TRACE_HTEX((stderr, "Found anchor on page %d", test_page));
	    found_anchor = True;
	    break;
	}
    }

    if (!found_anchor) {
	TRACE_HTEX((stderr, "Anchor not found"));
	/* Restore file position.  */
	maxchar = maxchar_save;
	currinf = currinf_save;
	
	if (dvi_pointer_frame != NULL) {
	    (void)lseek(fileno(globals.dvi_file.bak_fp), pos_save, SEEK_SET);
	    dvi_pointer_frame->pos = dvi_pointer_frame->end = dvi_buffer;
	}
	xdvi_bell();
	statusline_error(STATUS_MEDIUM, "Error: Anchor \"%s\" not found.", str);
	return;
    }

    /*
     * In this case we don't need to restore maxchar and currinf, since
     * we won't resume drawing -- we'll jump to a new page instead.
     */

    /* Move to that page.  */
    (void)lseek(fileno(globals.dvi_file.bak_fp), pageinfo_get_offset(test_page), SEEK_SET);
    currinf.tn_table_len = TNTABLELEN;
    currinf.tn_table = tn_table;
    currinf.tn_head = tn_head;
    currinf.pos = currinf.end = dvi_buffer;
    currinf.virtual = NULL;

    info.geom_special = htex_scan_special_noreturn;

    g_info.geom_box = htex_dummy_box;
    g_info.geom_data = &y_pos;

    info.data = &g_info;
    
    if (!setjmp(info.done_env))
	geom_scan_part(geom_do_char, globals.dvi_file.bak_fp, &info, geom_current_frame = &geom_frame0, dimconv);

    if (y_pos == -1) { /* not found */
	XDVI_ERROR((stderr, "%s:%d: shouldn't happen: geom_scan_part() failed to re-find the link.", __FILE__, __LINE__));
    }
    else {
	goto_page(test_page, resource.keep_flag ? NULL : home, False);
	page_history_insert(test_page);
	do_autoscroll = True;
	TRACE_HTEX((stderr, "Found anchor on position %d", y_pos));
	htex_set_anchormarker(y_pos);
    }
    /* reset info */
    free(g_anchor_pos);
    g_anchor_pos = NULL;
    g_anchor_len = 0;
}



#ifdef T1LIB

/* ********************** FONT AND ENCODING LOADING ******************** */

static char **
load_vector(const char *enc)
{
    char *filename;

    TRACE_T1((stderr, "loading vector: |%s|", enc));
    /*
     * With TDS 1.0/kpathsea 3.5.2(?), encoding files are in texmf/fonts/enc and accessed
     * via kpse_enc_format; see e.g.:
     * http://tug.org/mailman/htdig/tex-live/2004-January/004734.html
     *
     * The lookups under kpse_program_text_format and kpse_tex_ps_header_format
     * are kept for backwards compatibility.
     */
    do {
	filename = kpse_find_file(enc, kpse_enc_format, 0);
	if (filename != NULL)
	    break;
	filename = kpse_find_file(enc, kpse_program_text_format, 0);
	if (filename != NULL)
	    break;
	/* If that fails we try the standard place: postscript headers */
	filename = kpse_find_file(enc, kpse_tex_ps_header_format, 1);
    } while (0);
    
    if (filename != NULL) {
	return T1_LoadEncoding(filename);
    }
    return NULL;
}



static int
add_tfm(const char *texname)
{
    /* Load font metrics if not already loaded.  Return index into
       fontmetrics array. If it fails, fallback metrics must be
       loaded instead; if that fails as well, exit. */

    size_t i;
    int idx;
    static size_t wlidx = 0;
    static size_t wlidx_max = 0;
    static const size_t wlidx_step = 1024;

#ifdef USE_HASH
    if (tfminfo_hash.size == 0)
	tfminfo_hash = hash_create(T1FONTS_INITIAL_HASHTABLE_SIZE);

    if (find_str_int_hash(&tfminfo_hash, texname, &i)) {
	return i;
    }
#else
    for (i = 0; i <= wlidx; i++)
	if (strcmp(tfminfo[i].texname, texname) == 0)
	    return i;
#endif /* USE_HASH */
    
    while (wlidx >= wlidx_max) {
	wlidx_max += wlidx_step;
	tfminfo = xrealloc(tfminfo, wlidx_max * sizeof *tfminfo);
    }

    tfminfo[wlidx].texname = texname;
#ifdef USE_HASH
    put_str_int_hash(&tfminfo_hash, tfminfo[wlidx].texname, wlidx);
#endif /* USE_HASH */
    
    if (!tfmload(texname, &tfminfo[wlidx].designsize, tfminfo[wlidx].widths,
		 &(tfminfo[wlidx].fontdimen2))) {
	XDVI_FATAL((stderr, "Cannot find font metrics file %s.tfm, "
		    "and fallback metrics cmr10.tfm is missing as well - exiting.\n",
		    texname));
    }

    idx = wlidx++;

    if (fallbacktfm == 1) {
	fallbacktfm = 2;
	popup_message(globals.widgets.top_level,
		      MSG_ERR,
		      /* helptext */
		      "A font metrics file is missing and cmr10.tfm is used instead.  Either "
		      "you don't have the file, or your TeX system cannot find it.  Try to "
		      "invoke \"kpsewhich\" with the filename mentioned in the error "
		      "message and the \"-debug 2\" option to find out where Xdvik is "
		      "searching for your files.\n"
		      "To enable automatic creation of .tfm files for "
		      "Metafont fonts, "
		      /* FIXME: how about Type1 fonts?? */
		      "set the environment variable MKTEXTFM "
		      "to 1 and re-start xdvi. See the file texmf.cnf for the default settings "
		      "of MKTEXTFM and related variables.",
		      /* message */
		      "Could not find font metrics file %s.tfm; using cmr10.tfm instead. "
		      "Expect ugly output.",
		      texname);
    }

    return idx;
}

static int
add_t1font(const char *fontname, const char *filename)
{
    /* Add t1 font to list if it's not already there; return the
       (newly created, or already existing) t1lib font id.  The
       filename argument is optional, but is assumed to be a full path
       if it is given.
    */

    int id, idx;
    size_t i;
    char *path = NULL;
    
    static size_t t1lidx = 0;
    static size_t t1lidx_max = 0;
    static const size_t t1lidx_step = 1024;

    
#ifdef USE_HASH
    /* Already set up by that name? */
    if (t1fonts_hash.size == 0)
	t1fonts_hash = hash_create(T1FONTS_INITIAL_HASHTABLE_SIZE);
    
    if (find_str_int_hash(&t1fonts_hash, fontname, &i)) {
	return i;
    }
#else
    for (i = 0; i <= t1lidx; i++) {
	if (strcmp(t1fonts[i].shortname, fontname) == 0 ||
	    strcmp(t1fonts[i].file, fontname) == 0) {
	    TRACE_T1((stderr, "Type1 font %s already loaded from %s", fontname, filename));
	    return i;
	}
    }
#endif /* USE_HASH */

    /* Insert and set up new t1 font */
    while (t1lidx >= t1lidx_max) {
	t1lidx_max += t1lidx_step;
	t1fonts = xrealloc(t1fonts, t1lidx_max * sizeof *t1fonts);

	TRACE_T1((stderr, "Enlarged t1 table from %lu to %lu entries",
		  (unsigned long)t1lidx, (unsigned long)t1lidx_max));
    }

    if (filename == NULL) {
	/* We don't know the full path name yet, find it */
	path = kpse_find_file(fontname, kpse_type1_format, 0);
	if (path == NULL)
	    return -1;	/* xdvi will use substitution font */
    }
    else {
	path = xstrdup(filename);
    }

    t1fonts[t1lidx].file = xstrdup(path);
    t1fonts[t1lidx].t1id = id = T1_AddFont(path);
    t1fonts[t1lidx].shortname = xstrdup(fontname);
    t1fonts[t1lidx].loaded = 0;

    TRACE_T1((stderr, "Loaded font %lu/%s (%d) from %s", (unsigned long)t1lidx, fontname, id, path));

#ifdef USE_HASH
    /* also save the info in the hashtable (note: no duplication of the string here!) */
    put_str_int_hash(&t1fonts_hash, t1fonts[t1lidx].shortname, t1lidx);
#endif /* USE_HASH */

    idx = t1lidx++;
    
    free(path);

#if USE_AFM
    /* BZZT! AFM files are useless to us, their metric information is
       too inacurate for TeX use.  Never define USE_AFM */
    /* Set the afm filename.  Always before loading.  If no afm file
       found t1lib will use fallback code.  In fact the fallback code
       has been deactivated entierly, it's dog slow. */
    path = kpse_find_file(fontname, kpse_afm_format, 0);
    if (path != NULL) {
	TRACE_T1((stderr, "found afm file: %s", path));
	T1_SetAfmFileName(id, path);
	free(path);
    }
#endif

    return idx;
}

static int
find_texfont(const char *texname)
{
    /* Find fontmap index of texfont */
    size_t i;

#ifdef USE_HASH
    if (fontmaps_hash.size == 0)
	fontmaps_hash = hash_create(T1FONTS_INITIAL_HASHTABLE_SIZE);
    
    if (find_str_int_hash(&fontmaps_hash, texname, &i)) {
	return i;
    }
#else
    for (i = 0; i < g_maplidx; i++) {
	if (strcmp(fontmaps[i].texname, texname) == 0) {
	    TRACE_T1((stderr, "Type1 font already loaded at index %d: %s\n", i, texname));
	    return i;
	}
    }
#endif /* USE_HASH */
    return -1;
}


static int
setup_encoded_T1_font(const char *mapfile,
		      int lineno,
		      const char *texname,
		      const char *alias,
		      const char *filename,
		      int enc,
		      int ext,
		      int sl)
{
    /* xdvi T1 Font loading is done in two steps:

    1. At xdvi startup two things happen:

    a. The fontmaps are read.

    b. the dvi file is (partialy) scanned, and fonts are set up.
    In the case of t1 fonts we only set up the data structures,
    they are not actually loaded until they are used.

    2. At the time a T1 font is used it is loaded, encoded, extended
    and slanted as prescribed.

    This procedure takes care of step 1a.  It locates the font and
    sets up the data-structures so it may be assumed, by xdvi, to be
    loaded.  Return the fontmaps array index.

    The 'texname' param should be the texname of the font, such as
    ptmr8r or cmr10.

    The 'alias' param should be the file name of the font if it is
    different from the texname.  This is often the case with fonts
    defined in fontmaps.

    If, for some reason, the full filename of the font has already
    been looked up before we get here it is passed in the filename
    param so we don't have to look it up again.

    Implied encodings are not handled here.

    REMEMBER: THIS PROC IS CALLED FOR EACH FONT IN THE FONTMAPS, WE
    CANNOT DO ANY EXPENSIVE OPERATIONS HERE!!!

    */

    int test_idx;
    size_t curr_idx;
    static size_t maplidx_max = 0;
    static const size_t maplidx_step = 1024;

    /* fprintf(stderr, "setup font: |%s|, %s\n", texname, mapfile); */
    
    /* Already setup by that name? */
    test_idx = find_texfont(texname);
    ASSERT(test_idx == -1 || (test_idx > -1 && test_idx < (int)g_maplidx), "Result of find_texfont() out of range");
    
    if (mapfile != NULL && test_idx != -1) {
	/* font is already set up, and we're scanning the map file: replace existing font with new one */
	curr_idx = test_idx;
	XDVI_INFO((stdout, "%s: Entry for font \"%s\" on line %d overrides previous entry.",
		   mapfile, texname, lineno));
	
	free(fontmaps[curr_idx].filename);
	free(fontmaps[curr_idx].pathname);
    }
    else if (test_idx != -1) {
	/* font is already set up, but we're not scanning the map file - return font index */
	TRACE_T1_VERBOSE((stderr, "font |%s| already set up at index %d", texname, test_idx));
	return test_idx;
    }
    else { /* Not set up yet, do it. */
	curr_idx = g_maplidx++;		/* increment global index count */
	while (curr_idx >= maplidx_max) {
	    maplidx_max += maplidx_step;
	    fontmaps = xrealloc(fontmaps, maplidx_max * sizeof *fontmaps);
	    
	    TRACE_T1((stderr, "Enlarged the fontmap from %lu to %lu entries",
		      (unsigned long)curr_idx, (unsigned long)maplidx_max));
	}
	fontmaps[curr_idx].texname = xstrdup(texname);
    }

    if (alias == NULL)
	alias = texname;

#ifdef USE_HASH
    if (test_idx == -1) {
	if (fontmaps_hash.size == 0)
	    fontmaps_hash = hash_create(T1FONTS_INITIAL_HASHTABLE_SIZE);
	put_str_int_hash(&fontmaps_hash, fontmaps[curr_idx].texname, curr_idx);
    }
#endif /* USE_HASH */
    
    fontmaps[curr_idx].enc = enc;
    fontmaps[curr_idx].extension = ext;
    fontmaps[curr_idx].slant = sl;

    fontmaps[curr_idx].filename = xstrdup(alias);
    fontmaps[curr_idx].t1libid = -1;
    TRACE_T1((stderr,
	      "fontmaps[%lu]: \"%s\", enc=%d, ext=%d, sl=%d, alias \"%s\"",
	      (unsigned long)curr_idx, texname, enc, ext, sl, alias));

    if (filename != NULL) {
	fontmaps[curr_idx].pathname = xstrdup(filename);
    }
    else {
	fontmaps[curr_idx].pathname = NULL;
    }
    fontmaps[curr_idx].tfmidx = -1;
    fontmaps[curr_idx].warned_about = False;
    fontmaps[curr_idx].force_pk = False;

    return curr_idx;
}


static t1FontLoadStatusT
try_pk_fallback(int idx, struct font *fontp)
{
    Boolean success;

    success = load_font(fontp, False
#if DELAYED_MKTEXPK
			, True
#endif
			); /* loading non-t1 version of font */
    fontmaps[idx].force_pk = True;
    TRACE_T1((stderr,
	      "setting force_pk for %d to true; success for PK fallback: %d\n",
	      idx, success));
    
    if (!success) {
	/* this is probably serious enough for a GUI warning */
	popup_message(globals.widgets.top_level,
		      MSG_ERR,
		      /* helptext */
		      "Xdvi tries all of the following possibilities in turn, and all of them have failed:\n\n"
		      "  (1) If the resource t1lib is set, try a Postscript Type1 version of a font.\n\n"
		      "  (2) Otherwise, or if the Type1 version hasn't been found, try to "
		      "locate, or generate via mktexpk, a TeX Pixel (PK) version of the font.\n\n"
		      "  (3) Use the fallback font defined via the \"altfont\" resource (cmr10 by default), "
		      "both as Type1 and as PK version, at various resolutions.\n\n"
		      "It seems that your font setup is defective.\n",
		      /* errmsg */
		      "Error loading font %s: Neither a Type1 version nor "
		      "a pixel version could be found. The character(s) "
		      "will be left blank.",
		      fontmaps[idx].texname);
	currinf.set_char_p = currinf.fontp->set_char_p = set_empty_char;
	return FAILURE_BLANK;
    }
    /* free unneeded resources. We mustn't free
       fontmaps[idx].texname,
       else find_T1_font will try to load the font all over again.
    */
    free(fontmaps[idx].filename);
    free(fontmaps[idx].pathname);
    currinf.set_char_p = currinf.fontp->set_char_p = set_char;
    return FAILURE_PK;
}

static t1FontLoadStatusT
load_font_now(int idx, struct font *fontp)
{
    /* At this point xdvi needs to draw a glyph from this font.  But
       first it must be loaded/copied and modified if needed. */

    int t1idx;	/* The fontmap entry number */
    int t1id;	/* The id of the unmodified font */
    int cid;	/* The id of the copied font */
    int enc, sl, ext;

    /*      statusline_info(STATUS_SHORT, "Loading T1 font %s", fontmaps[idx].filename); */
    TRACE_T1((stderr, "adding %s %s",
	      fontmaps[idx].filename, fontmaps[idx].pathname));

    t1idx = add_t1font(fontmaps[idx].filename, fontmaps[idx].pathname);

    t1id = fontmaps[idx].t1libid = t1fonts[t1idx].t1id;

    errno = 0;
    if (!t1fonts[t1idx].loaded && T1_LoadFont(t1id) == -1) {
	/* this error is somehow better kept at stderr instead of
	   a popup (too annoying) or the statusline (might disappear
	   too fast) ... */
	XDVI_ERROR((stderr, "Could not load Type1 font %s from %s: %s %s (T1_errno = %d); "
		    "will try pixel version instead.\nPlease see the T1lib documentation for details about this.\n",
		    fontmaps[idx].texname, fontmaps[idx].pathname,
		    T1_StrError(T1_errno),
		    errno != 0 ? strerror(errno) : "",
		    T1_errno));
	return try_pk_fallback(idx, fontp);
    }

    fontmaps[idx].tfmidx = add_tfm(fontmaps[idx].texname);

    t1fonts[t1idx].loaded = 1;

    /* If there is nothing further to do, just return */
    enc = fontmaps[idx].enc;
    ext = fontmaps[idx].extension;
    sl = fontmaps[idx].slant;
    TRACE_T1((stderr, "lookup at idx[%d] yields: %d, %d, %d", idx, enc, ext, sl));

    if (enc == -1 && ext == 0 && sl == 0)
	return SUCCESS;

    /* The fontmap entry speaks of a modified font.  Copy it first. */
    cid = T1_CopyFont(t1id);

    fontmaps[idx].t1libid = cid;

    if (enc != -1) {
	TRACE_T1_VERBOSE((stderr, "trying to load vector from encodings index %d", enc));
	/* Demand load vector */
	if (encodings[enc].vector == NULL)
	    encodings[enc].vector = load_vector(encodings[enc].file);

	if (encodings[enc].vector == NULL) {
	    /* this error is somehow better kept at stderr instead of
	       a popup (too annoying) or the statusline (might disappear
	       too fast) ... */
	    XDVI_ERROR((stderr, "Could not load load encoding file %s for vector %s (font %s): %s (T1_errno = %d); "
			"will try pixel version instead.\n"
			"Please see the T1lib documentation for details about this.\n",
			encodings[enc].file, encodings[enc].enc, fontmaps[idx].texname,
			T1_StrError(T1_errno), T1_errno));
	    return try_pk_fallback(idx, fontp);
	}
	else {
	    if (T1_ReencodeFont(cid, encodings[enc].vector) != 0) {
		XDVI_ERROR((stderr, "Re-encoding of %s failed: %s (T1_errno = %d); "
			    "will try pixel version instead.\n"
			    "Please see the T1lib documentation for details about this.\n",
			    fontmaps[idx].texname,
			    T1_StrError(T1_errno), T1_errno));
		return try_pk_fallback(idx, fontp);
	    }
	}
    }

    if (ext)
	T1_ExtendFont(cid, ext / 1000.0);

    if (sl)
	T1_SlantFont(cid, sl / 10000.0);

    return SUCCESS;
}


int
find_T1_font(const char *texname)
{
    /* Step 1b in the scenario above.  xdvi knows that this font is
       needed.  Check if it is available, but do not load it yet.
       Return the fontmap index at which the font was found */

    int idx;	/* Iterator, t1 font id */
    char *filename;
    int fl, el;	/* Fontname length, encoding name length */
    int encoded = -1;
    char *mname;	/* Modified font name */
    size_t i;
    
    encoded = 0;

    /* First: Check the maps */
    idx = find_texfont(texname);

    if (idx != -1) {
	if (fontmaps[idx].force_pk) {
	    return idx;
	}
	if (fontmaps[idx].pathname == NULL) {
	    filename = kpse_find_file(fontmaps[idx].filename, kpse_type1_format, 0);
	    /* It should have been on disk according to the map, but never
	       fear, xdvi will try to find it other ways. */
	    if (filename == NULL) {
		if (!fontmaps[idx].warned_about) {
		    fontmaps[idx].warned_about = True;
		    XDVI_WARNING((stderr, "Font map calls for %s, but it was not found (will try PK version instead).",
				  fontmaps[idx].filename));
		}
		return -1;
	    }
	    fontmaps[idx].pathname = filename;
	}
	TRACE_T1((stderr, "find_T1_font map: %s, entered as #%d",texname,idx));
	return idx;
    }

#if 0
    /* SU: Fix for #1295829: If eg. plr10 is missing from ps2pk.map, xdvi
       will use plr10.pfb without reencoding it instead of calling mktexpk
       for plr10.mf which will use the correct encoding.
    */
    
    /* Second: the bare name */
    filename = kpse_find_file(texname, kpse_type1_format, 0);

    if (filename != NULL) {
	idx = setup_encoded_T1_font(NULL, 0, texname, NULL, filename, -1, 0, 0);
	TRACE_T1((stderr, "find_T1_font bare enc: %s, entered as #%d", texname, idx));
	return idx;
    }
#endif
    
    /* Third: Implied encoding? */
    fl = strlen(texname);

    for (i = 0; i < enclidx; i++) {
	if (encodings[i].enc == NULL)
	    continue;

	el = strlen(encodings[i].enc);
	
	/* Note: the encodings map is short, so the strcmp doesn't hurt here */
	if (strcmp(texname + (fl - el), encodings[i].enc) == 0) {
	    /* Found a matching encoding */
	    TRACE_T1((stderr, "Encoding match: %s - %s", texname,
		      encodings[i].enc));

	    mname = malloc(fl + 2);
	    strcpy(mname, texname);
	    mname[fl - el] = '\0';
	    /* If we had 'ptmr8r' the we now look for 'ptmr' */
	    filename = kpse_find_file(mname, kpse_type1_format, 0);
	    if (filename == NULL) {
		/* No? Look for ptmr8a. 8a postfix is oft used on raw fonts */
		strcat(mname, "8a");
		filename = kpse_find_file(mname, kpse_type1_format, 0);
	    }
	    if (filename != NULL) {
		idx = setup_encoded_T1_font(NULL, 0, texname, mname, filename, i, 0, 0);
		TRACE_T1((stderr, "find_T1_font implied enc: %s, is #%d", texname, idx));
		return idx;
	    }

	    free(mname);
	    /* Now we have tried everything.  Time to give up. */
	    return -1;

	}	/* If (strcmp...) */
    }	/* for */
    return -1;
}

/* ************************* CONFIG FILE READING ************************ */


static int
new_encoding(const char *enc, const char *file)
{
    /* (Possibly) new encoding entered from .cfg file, or from dvips
       map.  When entered from dvips map the enc is null/anonymous */

    size_t i;
    static size_t enclidx_max = 0;
    static const size_t enclidx_step = 16;

    /* Shirley!  You're jesting! */
    if (file == NULL)
	return -1;

    if (enc == NULL) {
	/* Enter by file name.  First check if the file has been entered
	   already. */
	for (i = 0; i < enclidx; i++) { 
	    if (strcmp(encodings[i].file, file) == 0) {
		return i;
	    }
	}
    }
    else {
	/* Enter by encoding name.  Check if already loaded first. */
	for (i = 0; i < enclidx; i++) {
	    if (encodings[i].enc != NULL && strcmp(encodings[i].enc, enc) == 0)
		return i;
	}
    }

    /* Bonafide new encoding */

    while (enclidx >= enclidx_max) {
	enclidx_max += enclidx_step;
	encodings = xrealloc(encodings, enclidx_max * sizeof *encodings);

	TRACE_T1((stderr, "Enlarged encoding map from %lu to %lu entries",
		  (unsigned long)enclidx, (unsigned long)enclidx_max));
    }

    TRACE_T1((stderr, "New encoding #%lu: '%s' -> '%s'", (unsigned long)enclidx, enc ? enc : "<NULL>", file));

    /* The encoding name is optional */
    encodings[enclidx].enc = NULL;
    if (enc != NULL)
	encodings[enclidx].enc = xstrdup(enc);

    /* The file name is required */
    encodings[enclidx].file = xstrdup(file);
    encodings[enclidx].vector = NULL;	/* Demand load later */

    return enclidx++;
}


void
add_T1_mapentry(int lineno,
		const char *mapfile,
		const char *name,
		const char *file,
		const char *vec,
		char *spec)
{
    /* This is called from read_map_file, once for each fontmap line `lineno'.  We
       will dutifully enter the information into our fontmap table */

    static char delim[] = "\t ";
    char *last, *current;
    float number;
    int extend = 0;
    int slant = 0;

    TRACE_T1_VERBOSE((stderr, "%s:%d: %s -> %s Enc: %s Spec: %s",
		      mapfile, lineno,
		      name, file,
		      vec == NULL ? "<none>" : vec,
		      spec == NULL ? "<none>" : spec));

    if (spec != NULL) {
	/* Try to analyze the postscript string.  We recognize two things:
	   "n ExtendFont" and "m SlantFont".  n can be a decimal number in
	   which case it's an extension factor, or a integer, in which
	   case it's a charspace unit? In any case 850 = .85 */

	last = strtok(spec, delim);
	current = strtok(NULL, delim);
	while (current != NULL) {
	    if (strcmp(current, "ExtendFont") == 0) {
		sscanf(last, "%f", &number);
		if (number < 10.0)
		    extend = number * 1000.0;
		else
		    extend = number;
	    }
	    else if (strcmp(current, "SlantFont") == 0) {
		sscanf(last, "%f", &number);
		slant = number * 10000.0;
	    }
	    last = current;
	    current = strtok(NULL, delim);
	}
    }

    setup_encoded_T1_font(mapfile, lineno,
			  name, file, NULL, new_encoding(NULL, vec), extend,
			  slant);
}

static void
read_cfg_file(const char *file)
{
    char *filename;
    FILE *fp;
    int len;
    char *keyword;
    char *ptr;
    char *buffer;
    char *enc;
    char *name;
    int i;
    static const char delim[] = "\t \n\r";
    Boolean found_no_map_files = True;
    
    if (file == NULL)
	file = "xdvi.cfg";
    
    filename = kpse_find_file(file, kpse_program_text_format, 1);
    if (filename == NULL) {
	statusline_error(STATUS_MEDIUM, "Warning: Unable to find \"%s\"!", file);
	return;
    }

    if ((fp = XFOPEN(filename, "r")) == NULL) {
	XDVI_ERROR((stderr, "Cannot open config file `%s' for reading: %s", filename, strerror(errno)));
	return;
    }

    TRACE_T1((stderr, "Reading cfg file %s", filename));

    buffer = xmalloc(BUFFER_SIZE);

    while (fgets(buffer, BUFFER_SIZE, fp) != NULL) {
	len = strlen(buffer);
	if (buffer[len - 1] != '\n') {
	    int c;
	    /* this really shouldn't happen ... */
	    XDVI_WARNING((stderr, "Skipping overlong line (> %d characters) in config file `%s':\n%s ...",
			  BUFFER_SIZE, filename, buffer));
	    /* read until end of this line */
	    while((c = fgetc(fp)) != '\n' && c != EOF) { ; }
	    continue;
	}

	keyword = buffer;

	/* Skip leading whitespace */
	while (keyword[0] != '\0' && (keyword[0] == ' ' || keyword[0] == '\t'))
	    keyword++;

	/* % in first column is a correct comment */
	if (keyword[0] == '%' || keyword[0] == '\0' || keyword[0] == '\n')
	    continue;

	keyword = strtok(keyword, delim);

	if (strcmp(keyword, "dvipsmap") == 0) {
	    if ((ptr = strtok(NULL, delim)) == NULL) {
		XDVI_WARNING((stderr, "Syntax error in entry \"%s\"", buffer));
		continue;
	    }
	    TRACE_T1((stderr, "DVIPSMAP: '%s'", ptr));

	    if (read_map_file(ptr)) {
		found_no_map_files = False;
	    }
	    else {
		TRACE_T1((stderr, "Read map file: '%s'", ptr));
	    }
	}
	else if (strcmp(keyword, "encmap") == 0) {
	    popup_message(globals.widgets.top_level,
			  MSG_ERR,
			  "Your xdvi.cfg file is for a previous version of xdvik. Please replace "
			  "it by the xdvi.cfg file in the current xdvik distribution.",
			  "Keyword \"encmap\" in xdvi.cfg is no longer supported, "
			  "please update the config file %s.",
			  filename);
	}
	else if (strcmp(keyword, "enc") == 0) {
	    enc = strtok(NULL, delim);
	    name = strtok(NULL, delim);
	    if ((ptr = strtok(NULL, delim)) == NULL) {
		XDVI_WARNING((stderr, "Syntax error in entry \"%s\" (skipping line)", buffer));
		continue;
	    }
	    i = new_encoding(enc, ptr);
	    TRACE_T1((stderr, "Encoding[%d]: '%s' = '%s' -> '%s'", i, enc, name, ptr));
	} else {
	    /* again, nag them with a popup so that they'll do something about this ... */
	    popup_message(globals.widgets.top_level,
			  MSG_ERR,
			  "Please check the syntax of your config file. "
			  "Valid keywords are: \"enc\" and \"dvipsmap\".",
			  "Skipping unknown keyword \"%s\" in config file %s.",
			  keyword, filename);
	}
    }

    if (found_no_map_files) {
	/* nag 'em with a popup so that they'll do something about this ... */
	popup_message(globals.widgets.top_level,
		      MSG_ERR,
		      "Direct Type 1 font rendering via T1lib gives you many benefits, such as:\n"
		      " - quicker startup time, since no bitmap fonts need to be generated;\n"
		      " - saving disk space for storing the bitmap fonts.\n"
		      "To fix this error, check that the file `ps2pk.map' is located somewhere "
		      "in your XDVIINPUTS path. Have a look at the xdvi wrapper shell script "
		      "(type \"which xdvi\" to locate that shell script) for the current setting "
		      "of XDVIINPUTS.",
		      "Could not load any of the map files listed in xdvi.cfg - disabling T1lib.");
	resource.t1lib = False;
    }
    fclose(fp);

    free(buffer);
    free(filename);
}


/* **************************** GLYPH DRAWING *************************** */


/* Set character# ch */

static struct glyph *
get_t1_glyph(
#ifdef TEXXET
	     wide_ubyte cmd,
#endif
	     wide_ubyte ch, t1FontLoadStatusT *flag,
	     Boolean is_geom_scan)
{

    /*
      A problem is that there are some gaps in math modes tall braces '{',
      (see for example the amstex users guide).  These gaps are seen less if a
      small size factor is applied, 1.03 seems nice.  This does not seem like
      the Right Thing though.  Also gaps do not show up in the fullsize
      window.

      All in all the factor has been dropped.  Despite the beauty flaw.
    */

    float size = currinf.fontp->pixsize * 72 / resource.pixels_per_inch;

    int id = currinf.fontp->t1id;
    int t1libid = fontmaps[id].t1libid;

    GLYPH *G;		/* t1lib glyph */
    struct glyph *g;	/* xdvi glyph */

#ifdef TEXXET
    UNUSED(cmd);
#endif
    *flag = SUCCESS;

#if COLOR
    if (!is_geom_scan && fg_active != fg_current)
	do_color_change();
#endif

    /* return immediately if we need to use the fallback PK */
    if (fontmaps[id].force_pk)
	return NULL;

    TRACE_T1((stderr, "pixsize: %f, ppi %d, sf: %d, size: %f",
	      currinf.fontp->pixsize, resource.pixels_per_inch,
	      currwin.shrinkfactor, size));

    if (t1libid == -1) {
	TRACE_T1((stderr, "trying to load font %d", id));
	*flag = load_font_now(id, currinf.fontp);
	if (*flag < 0) {
	    TRACE_T1((stderr, "load_font_now failed for T1 font %d", id));
	    return NULL;
	}
	t1libid = fontmaps[id].t1libid;
    }
    
    TRACE_T1((stderr, "Setting 0x%x `%c' of %d, at %f(%.2fpt), shrinkage is %d",
	      ch,
	      isprint(ch) ? ch : '?',
	      t1libid, currinf.fontp->pixsize,
	      size, currwin.shrinkfactor));

    /* Check if the glyph already has been rendered */
    if (ch > currinf.fontp->maxchar) {
	/* in this case, the replacement font is 'smaller' than the requested one.
	   This will later lead to a warning: "Character 9561 not defined in font cmr10".
	*/
	return NULL;
    }
    if ((g = &currinf.fontp->glyph[ch])->bitmap.bits == NULL) {
	int bitmapbytes;
	int h;
	unsigned char *f;
	unsigned char *t;
	/* Not rendered: Generate xdvi glyph from t1font */

	g->addr = 1; /* Dummy, should not provoke errors other places in xdvi */

	/* Use the width from the tfm file */
	g->dvi_adv = tfminfo[fontmaps[id].tfmidx].widths[ch] * currinf.fontp->dimconv;
	TRACE_T1((stderr, "0x%x, dvi_adv = %ld; dimconv: %f",
		  ch,
		  tfminfo[fontmaps[id].tfmidx].widths[ch],
		  currinf.fontp->dimconv));

	/* Render the glyph.  Size here should be postscript bigpoints */
	G = T1_SetChar(t1libid, ch, size, NULL);
	if (G == NULL) {
	    /* This can happen e.g. if font is too small; example with plain TeX:
	       \magnification=50 Hello, world!\bye
	       Correction (2010-01-05):  this doesn't happen anymore, now that
	       (*ahem*) the size is no longer rounded to an integer.
	    */
	    statusline_info(STATUS_FOREVER,
			     "Error rendering character 0x%x `%c' - replacing by whitespace",
			     ch,
			     isprint(ch) ? ch : '?');
	    XDVI_ERROR((stderr, "T1lib failed for character 0x%x `%c': %s. Replacing by whitespace.",
			ch, isprint(ch) ? ch : '?',
			T1_StrError(T1_errno)));
	    g->bitmap.w = 1;
	    g->bitmap.h = 1;
	    g->bitmap.bytes_wide = T1PAD(g->bitmap.w, archpad) / 8;
	    alloc_bitmap(&g->bitmap);
	    memset(g->bitmap.bits, 0, g->bitmap.h * g->bitmap.bytes_wide);
	    g->x = 0;
	    g->y = 0;
#if COLOR
	    g->fg = fg_current;
#endif
	    return g;
	}

	if (G->bits == NULL) {
#if 0
	    /* in this case the glyph can either be a whitespace or .notdef.
	       We would like to warn about the latter, but I haven't found a way
	       to distinguish between the two cases that works reliably.

	       If the font has built-in encoding, we can't use the following
	       test to check for the name, since the index i doesn't correspond to "ch":
	    */
	    char **glyph_names = T1_GetAllCharNames(t1libid);
	    for (i = 0; glyph_names[i] != NULL; i++) {
		fprintf(stderr, "CHAR %d: %s\n", i, glyph_names[i]);
	    }
	    free(glyph_names); // not sure about this ...

	    /*
	      For ~/projects/xdvik/bugs/cjk-notdef-problem, this prints:
	      
	      CHAR 0: .notdef
	      CHAR 1: uni3000
	      
	      even though the character at point 0 is uni3000.
	      
	      For external encodings, it should be possible to look at the encoding
	      vector:
	    */

	    int enc = fontmaps[id].enc;
	    if (enc != -1) {
		if (encodings[enc].vector == NULL) {
		    fprintf(stderr, "Unexpected: vector is NULL!\n");
		}
		else {
		    fprintf(stderr, "GOT CHAR: %s\n", encodings[enc].vector[ch]);
		}
	    }

	    /*
	      Also checking for the font dimens doesn't help in this case - the
	      results are identical for .notdef and uni3000:
	    */
	    
	    fprintf(stderr, "DIMENS: %d,%d,%d,%d\n",
		    G->metrics.rightSideBearing, G->metrics.leftSideBearing,
		    G->metrics.advanceX, G->metrics.advanceY);

	    /* the following simple test for whitespace doesn't work for Chinese fonts either: */
	    if (ch != ' ') {
		char index[128];
		char *fontname = xstrdup(currinf.fontp->fontname);
		size_t dummy = 0;
		
		SNPRINTF(index, sizeof index, "%d", ch);
		fontname = xstrcat(fontname, index);
		if (font_warn_hash.size == 0) {
		    /* TODO: print to statusline */
		    /* 		    popup_message(MSG_WARN, */
		    /* 				     NULL, */
		    /* 				     "Unexpected blank character (.notdef) in font %s, " */
		    /* 				     "replacing with whitespace (see output to stderr for details). " */
		    /* 				     "This might indicate that the font %s is missing " */
		    /* 				     "from the font map, or that it has been specified " */
		    /* 				     "with a wrong encoding.", */
		    /* 				     currinf.fontp->fontname, currinf.fontp->fontname); */
		    font_warn_hash = hash_create(T1FONTS_INITIAL_HASHTABLE_SIZE);
		}
		
		if (!find_str_int_hash(&font_warn_hash, fontname, &dummy)) {
		    if (!resource.hush_chars)
			XDVI_WARNING((stderr, "Character %d is mapped to .notdef in font %s (page %d), "
				      "replaced by whitespace.",
				      ch, currinf.fontp->fontname, current_page + 1));
		    put_str_int_hash(&font_warn_hash, fontname, dummy);
		}
		else {
		    free(fontname);
		}
	    }
#endif
	    /* *flag = FAILURE_BLANK; */
	    /* g->addr = -1; */
	    /* return NULL; */
	    
	    /* FIXME: g->dvi_adv is only used when invoking set_char(), not with
	       set_empty_char(), and the former apparently needs a valid bitmap.
	       Check how this is done without t1lib.
	    */
	    TRACE_T1((stderr, "replaced .notdef or whitespace char 0x%02x by space.\n", ch));
	    g->bitmap.w = 1;
	    g->bitmap.h = 1;
	    g->bitmap.bytes_wide = T1PAD(g->bitmap.w, archpad) / 8;
	    alloc_bitmap(&g->bitmap);
	    memset(g->bitmap.bits, 0, g->bitmap.h * g->bitmap.bytes_wide);
	    g->x = 0;
	    g->y = 0;
#if COLOR
	    g->fg = fg_current;
#endif
	    return g;
	}

	g->bitmap.w = abs(G->metrics.rightSideBearing - G->metrics.leftSideBearing);
	g->bitmap.h = abs(G->metrics.descent - G->metrics.ascent);
	g->bitmap.bytes_wide = T1PAD(g->bitmap.w, archpad) / 8;
	alloc_bitmap(&g->bitmap);

	bitmapbytes = g->bitmap.bytes_wide * g->bitmap.h;

	if (padMismatch) {
	    /* Fix alignment mismatch */
	    int from_bytes = T1PAD(g->bitmap.w, 8) / 8;

	    f = (unsigned char *)G->bits;
	    t = (unsigned char *)g->bitmap.bits;

	    memset(t, 0, bitmapbytes); /* OR ELSE! you get garbage */

	    for (h = 0 ; h < g->bitmap.h; h++) {
		memcpy(t, f, from_bytes);
		f += from_bytes;
		t += g->bitmap.bytes_wide;
	    }


	} else {
	    /* t1lib and arch alignment matches */
	    memcpy(g->bitmap.bits, G->bits, bitmapbytes);
	}

#ifdef WORDS_BIGENDIAN
	/* xdvi expects the bits to be reversed according to
	   endianness.  t1lib expects no such thing.  This loop reverses
	   the bit order in all the bytes. -janl 18/5/2001 */
	t = (unsigned char *)g->bitmap.bits;

	for (h = 0; h < bitmapbytes; h++) {
	    *t = rbits[*t];
	    t++;
	}
#endif

	g->x = -G->metrics.leftSideBearing;  /* Opposed x-axis */
	g->y = G->metrics.ascent;
#if COLOR
	g->fg = fg_current;
#endif
    }
    return g;
}

setcharRetvalT
set_t1_char(
#ifdef TEXXET
	    wide_ubyte cmd,
#endif
	    wide_ubyte ch)
{
    t1FontLoadStatusT flag;
    
    /*
      get_t1_glyph() will itself set currinf.fontp->set_char_p
      to set_empty_char() or set_char() if it failed to load the
      T1 version, but for the *current* call of set_t1_char,
      we still need to know whether (1) or (2) holds:
      (1) Type1 font hasn't been found but PK is working,
      use set_char() to set the PK version
      (2) neither Type1 nor PK have been found (this is the
      FAILURE_BLANK case), use set_empty_char().
      So we need the flag to pass this additional information.
    */
#ifdef TEXXET      
    (void)get_t1_glyph(cmd, ch, &flag, False);
    if (flag == FAILURE_BLANK)
	set_empty_char(cmd, ch);
    else
	set_char(cmd, ch);
    return;
#else
    (void)get_t1_glyph(ch, &flag, False);
    if (flag == FAILURE_BLANK)
	return set_empty_char(ch);
    else
	return set_char(ch);
#endif
}

void read_T1_char(struct font *fontp, wide_ubyte ch)
{
    UNUSED(fontp);
    UNUSED(ch);
    /* Should never be called */
    XDVI_ABORT((stderr, "%s:%d: asked to load %c", __FILE__, __LINE__, ch));
}

void init_t1(void)
{
    int i;
    void *success;
    /* Attempt to set needed padding.

    FIXME: This is not really the required alignment/padding.  On
    ix86 the requirement for int * is 8 bits, on sparc on the other
    hand it's 32 bits.  Even so, some operations will be faster if
    the bitmaps lines are alligned "better".  But on the other hand
    this requires a more complex glyph copying operation...

    - janl 16/5/2001
    */
    archpad = BMBYTES * 8;
    i = T1_SetBitmapPad(archpad);
    if (i == -1) {
	/* Failed, revert to 8 bits and compilicated bitmap copying */
	padMismatch = True;
	T1_SetBitmapPad(8);
    } else {
	padMismatch = False;
    }

    /* Initialize t1lib, use LOGFILE to get logging, NO_LOGFILE otherwise */
    /* Under NO circumstances remove T1_NO_AFM, it slows us down a LOT */

    if (globals.debug & DBG_T1) {
	XDVI_INFO((stdout, "Additional t1lib messages may be found in \"t1lib.log\"."));
	success = T1_InitLib(LOGFILE | T1_NO_AFM | IGNORE_CONFIGFILE | IGNORE_FONTDATABASE);
    }
    else
	success = T1_InitLib(NO_LOGFILE | T1_NO_AFM | IGNORE_CONFIGFILE | IGNORE_FONTDATABASE);
    if (success == NULL) {
	XDVI_FATAL((stderr, "Initialization of t1lib failed!"));
    }
    T1_SetLogLevel(T1LOG_DEBUG);

    if (resource.subpixel_order == SUBPIXEL_NONE)
	T1_SetDeviceResolutions(resource.pixels_per_inch,
	  resource.pixels_per_inch);
    else
	T1_SetDeviceResolutions(3 * resource.pixels_per_inch,
	  resource.pixels_per_inch);

    read_cfg_file(NULL);
}

#endif /* T1LIB */


#if GREY

extern	double	pow();

static void
mask_shifts(Pixel mask, int *pshift1, int *pshift2)
{
    int k, l;

    for (k = 0; (mask & 1) == 0; ++k)
	mask >>= 1;
    for (l = 0; (mask & 1) == 1; ++l)
	mask >>= 1;
    *pshift1 = sizeof(short) * 8 - l;
    *pshift2 = k;
}

/*
 *	Try to allocate 4 color planes for 16 colors (for GXor drawing).
 *	Not called if the visual type is TrueColor.
 *	When color specials are active, this is called exactly once.
 *	It is used for the first foreground/background pair displayed in
 *	the document.  For other documents, we act as if this call had failed.
 */


void
init_plane_masks(void)
{
    Pixel pixel;

    if (globals.gc.do_copy || plane_masks[0] != 0)
	return;

    if (XAllocColorCells(DISP, G_colormap, False, plane_masks, 4, &pixel, 1)) {
	/* Make sure fore_Pixel and back_Pixel are a part of the palette */
	resource.back_Pixel = pixel;
	resource.fore_Pixel = pixel | plane_masks[0] | plane_masks[1]
	    | plane_masks[2] | plane_masks[3];
	if (mane.win != (Window) 0)
	    XSetWindowBackground(DISP, mane.win, resource.back_Pixel);
    }
    else {
	globals.gc.do_copy = True;
	warn_overstrike();
    }
}

#endif /* GREY */

#if COLOR

/*
 *	Insert into list of colors to be freed upon opening new document.
 */

static	void
color_list_insert(Pixel pixel)
{
    if (color_list_len >= color_list_max) {
	if (color_list_max == 0)
	    color_list = xmalloc((color_list_max += 16) * sizeof *color_list);
	else
	    color_list = xrealloc(color_list,
				  (color_list_max += 16) * sizeof *color_list);
    }
    color_list[color_list_len++] = pixel;
}


/*
 *	Warn about colors not being correct.
 */

static	void
color_warn(void)
{
    if (!color_warned) {
	color_warned = True;
	popup_message(globals.widgets.top_level,
		      MSG_WARN,
		      /* helptext */
		      "Either this document is using too many "
		      "colors, or some other application is. "
		      "In the latter case, try to close that "
		      "application and re-read the DVI file.",
		      /* msg */
		      "Cannot allocate colormap entry, "
		      "displayed colors will not be exact.");
    }
}


/*
 *	Allocate a color and add it to our list of pixels to be returned
 *	upon opening a new document.
 */

#define	SHIFTIFY(x, shift1, shift2)	((((Pixel)(x)) >> (shift1)) << (shift2))

static	int	shift1_r, shift1_g, shift1_b;
static	int	shift2_r, shift2_g, shift2_b;
static	Boolean	shifts_good	= False;

Pixel
alloc_color(const struct rgb *colorp, Pixel fallback_pixel)
{
    XColor xcol;

    if (G_visual->class == TrueColor) {
	if (!shifts_good) {
	    mask_shifts(G_visual->red_mask,   &shift1_r, &shift2_r);
	    mask_shifts(G_visual->green_mask, &shift1_g, &shift2_g);
	    mask_shifts(G_visual->blue_mask,  &shift1_b, &shift2_b);
	    shifts_good = True;
	}
	return SHIFTIFY(colorp->r, shift1_r, shift2_r)
	    | SHIFTIFY(colorp->g, shift1_g, shift2_g)
	    | SHIFTIFY(colorp->b, shift1_b, shift2_b);
    }
    else {
	xcol.red = colorp->r;
	xcol.green = colorp->g;
	xcol.blue = colorp->b;
	xcol.flags = DoRed | DoGreen | DoBlue;
	if (XAllocColor(DISP, G_colormap, &xcol)) {
	    color_list_insert(xcol.pixel);
	    if (globals.debug & DBG_DVI)
		printf("alloc_color%6d%6d%6d --> %ld\n",
		       xcol.red, xcol.green, xcol.blue, xcol.pixel);
	    return xcol.pixel;
	}
	else {
	    if (globals.debug & DBG_DVI)
		printf("alloc_color%6d%6d%6d --> failed\n",
		       xcol.red, xcol.green, xcol.blue);
	    color_warn();
	    return fallback_pixel;
	}
    }
}

#undef SHIFTIFY


/*
 *	Switch colors of GCs, etc.  Called when we're about to use a GC.
 */

void
do_color_change(void)
{
    static int shrink_allocated_for = 0;
    Pixel set_bits;
    Pixel clr_bits;
    
    ASSERT(fg_current != NULL, "Current foreground mustn't be NULL");
    ASSERT(bg_current != NULL, "Current background mustn't be NULL");
#if GREY
    if (resource.use_grey) {
	if (G_visual->class == TrueColor) {
	    if (!fg_current->pixel_good) {
		fg_current->pixel = alloc_color(&fg_current->color,
						color_data[0].pixel);
		fg_current->pixel_good = True;
	    }

	    set_bits = fg_current->pixel & ~bg_current->pixel;
	    clr_bits = bg_current->pixel & ~fg_current->pixel;

	    if (set_bits & G_visual->red_mask)
		set_bits |= G_visual->red_mask;
	    if (clr_bits & G_visual->red_mask)
		clr_bits |= G_visual->red_mask;
	    if (set_bits & G_visual->green_mask)
		set_bits |= G_visual->green_mask;
	    if (clr_bits & G_visual->green_mask)
		clr_bits |= G_visual->green_mask;
	    if (set_bits & G_visual->blue_mask)
		set_bits |= G_visual->blue_mask;
	    if (clr_bits & G_visual->blue_mask)
		clr_bits |= G_visual->blue_mask;

	    /*
	     * Make the GCs
	     */

	    globals.gc.rule = set_or_make_gc(globals.gc.rule, GXcopy, fg_current->pixel, bg_current->pixel);
	    globals.gc.fore2 = NULL;

	    if (resource.copy
		|| (set_bits && clr_bits && !resource.thorough)) {
		if (!resource.copy) {
		    warn_overstrike();
		}
		globals.gc.fore = set_or_make_gc(globals.gc.fore, GXcopy, fg_current->pixel, bg_current->pixel);
	    }
	    else {
		if (set_bits) {
		    globals.gc.fore = set_or_make_gc(globals.gc.fore, GXor, fg_current->pixel & set_bits, 0);
		    if (clr_bits) {
			globals.gc.fore2 = globals.gc.fore2_bak = set_or_make_gc(globals.gc.fore2_bak, GXandInverted,
										 ~fg_current->pixel & clr_bits, 0);
		    }
		}
		else {
		    globals.gc.fore = set_or_make_gc(globals.gc.fore, GXandInverted,
						     ~fg_current->pixel & clr_bits, 0);
		}
	    }

	    if (globals.debug & DBG_DVI)
		printf("do_color_change: fg = %lx, bg = %lx, with%s globals.gc.fore2\n",
		       fg_current->pixel, bg_current->pixel,
		       globals.gc.fore2 == NULL ? "out" : "");

	    if (mane.shrinkfactor > 1) {
		unsigned int i;
		unsigned int sf_squared;

		sf_squared = mane.shrinkfactor * mane.shrinkfactor;

		if (shrink_allocated_for < mane.shrinkfactor) {
		    if (pixeltbl != NULL) {
			free((char *) pixeltbl);
			if (pixeltbl_gc2 != NULL) {
			    free((char *) pixeltbl_gc2);
			    pixeltbl_gc2 = NULL;
			}
		    }
		    pixeltbl = xmalloc((sf_squared + 1) * sizeof *pixeltbl);
		    shrink_allocated_for = mane.shrinkfactor;
		}
		if (globals.gc.fore2 != NULL && pixeltbl_gc2 == NULL) {
		    /* Can't use sf_squared (or mane.shrinkfactor) here */
		    pixeltbl_gc2 = xmalloc((shrink_allocated_for * shrink_allocated_for + 1) * sizeof *pixeltbl_gc2);
		}

		/*
		 * Initialize the pixel lookup table according to the gamma values.
		 */
#define	SHIFTIFY(x, shift1, shift2)	((((Pixel)(x)) >> (shift1)) << (shift2))

		for (i = 0; i <= sf_squared; ++i) {
		    double frac = resource.gamma > 0
			? pow((double)i / sf_squared, 1 / resource.gamma)
			: 1 - pow((double) (sf_squared - i) / sf_squared, -resource.gamma);
		    unsigned int red, green, blue;
		    Pixel pixel;
		    /*  		    fprintf(stderr, "frac: %f\n", frac); */
		    /* 		    fprintf(stderr, "fg_current: %d, bg_current: %d\n", fg_current->color.r, bg_current->color.r); */
		    red = frac
			* ((double) fg_current->color.r - bg_current->color.r)
			+ bg_current->color.r;
		    green = frac
			* ((double) fg_current->color.g - bg_current->color.g)
			+ bg_current->color.g;
		    blue = frac
			* ((double) fg_current->color.b - bg_current->color.b)
			+ bg_current->color.b;

		    pixel = SHIFTIFY(red,   shift1_r, shift2_r)
			| SHIFTIFY(green, shift1_g, shift2_g)
			| SHIFTIFY(blue,  shift1_b, shift2_b);

		    if (globals.gc.fore2 != NULL) {	/* if thorough */
			/*	fprintf(stderr, "++++++ pixeltable at %d: %ld\n", i, pixel & ~bg_current->pixel); */
			pixeltbl[i] = pixel & ~bg_current->pixel;
			pixeltbl_gc2[i] = ~pixel & bg_current->pixel;
		    }
		    else if (resource.copy || (set_bits && clr_bits)) {
			/*	fprintf(stderr, "++++++ pixeltable2 at %d: %ld\n", i, pixel); */
			pixeltbl[i] = pixel;
		    }
		    else {
			/*	fprintf(stderr, "++++++ pixeltable3 at %d: 0x%lx\n", i, ~pixel & clr_bits); */
			/*	fprintf(stderr, "++++++ pixeltable3 at %d: %ld\n", i, pixel & set_bits); */
			pixeltbl[i] = set_bits ? pixel & set_bits : ~pixel & clr_bits;
		    }
		}
#undef	SHIFTIFY
	    }

	}
	else {	/* not TrueColor */
	    int	i;
	    Boolean	using_planes;

	    using_planes = (fg_current == bg_head->fg_head && !globals.gc.do_copy);
	    if (!fg_current->palette_good) {
		XColor color;

		/*
		 * Initialize the pixel lookup table according to the gamma values.
		 */
		for (i = 0; i < 16; ++i) {
		    double frac;

		    frac = resource.gamma > 0
			? pow((double) i / 15, 1 / resource.gamma)
			: 1 - pow((double) (15 - i) / 15, -resource.gamma);
		    color.red = frac
			* ((double) fg_current->color.r - bg_current->color.r)
			+ bg_current->color.r;
		    color.green = frac
			* ((double) fg_current->color.g - bg_current->color.g)
			+ bg_current->color.g;
		    color.blue = frac
			* ((double) fg_current->color.b - bg_current->color.b)
			+ bg_current->color.b;

		    color.flags = DoRed | DoGreen | DoBlue;

		    if (using_planes) {
			color.pixel = resource.back_Pixel;	/* start of block */
			if (i & 1) color.pixel |= plane_masks[0];
			if (i & 2) color.pixel |= plane_masks[1];
			if (i & 4) color.pixel |= plane_masks[2];
			if (i & 8) color.pixel |= plane_masks[3];
			XStoreColor(DISP, G_colormap, &color);
			fg_current->palette[i] = color.pixel;
		    }
		    else {
			if (XAllocColor(DISP, G_colormap, &color)) {
			    fg_current->palette[i] = color.pixel;
			    color_list_insert(color.pixel);
			}
			else {
			    color_warn();
			    fg_current->palette[i] =
				(i * 100 >= resource.density * 15)
				? resource.fore_Pixel : bg_current->pixel;
			}
		    }
		}

		if (using_planes && bg_current->pixel != resource.back_Pixel) {
		    bg_current->pixel = resource.back_Pixel;
		    /* 		    XSetWindowBackground(DISP, mane.win, bg_current->pixel); */
#if MOTIF
		    fprintf(stderr, "setting window background!\n");
		    XSetWindowBackground(DISP, XtWindow(globals.widgets.main_window), bg_current->pixel);
		    XtVaSetValues(globals.widgets.main_window, XmNbackground, bg_current->pixel, NULL);
#else
		    XSetWindowBackground(DISP, mane.win, bg_current->pixel);
#endif
		    XClearWindow(DISP, mane.win);
		}

		fg_current->palette_good = True;
	    }

	    if (globals.debug & DBG_DVI)
		printf("do_color_change: fg = %ld, bg = %ld, using_planes = %d\n",
		       fg_current->palette[15], bg_current->pixel, using_planes);

	    if (using_planes) {
		globals.gc.rule = set_or_make_gc(globals.gc.rule, GXor, fg_current->palette[15],
						 bg_current->pixel);
		globals.gc.fore = set_or_make_gc(globals.gc.fore, GXor, fg_current->palette[15],
						 bg_current->pixel);
	    }
	    else {
		globals.gc.rule = set_or_make_gc(globals.gc.rule, GXcopy, fg_current->palette[15],
						 bg_current->pixel);
		globals.gc.fore = set_or_make_gc(globals.gc.fore, GXcopy, fg_current->palette[15],
						 bg_current->pixel);
	    }

	    globals.gc.fore2 = NULL;

	    if (mane.shrinkfactor > 1) {
		if (shrink_allocated_for < mane.shrinkfactor) {
		    if (pixeltbl != NULL)
			free((char *) pixeltbl);
		    pixeltbl = xmalloc((unsigned)(mane.shrinkfactor * mane.shrinkfactor + 1)
				       * sizeof *pixeltbl);
		    shrink_allocated_for = mane.shrinkfactor;
		}

		for (i = 0; i <= mane.shrinkfactor * mane.shrinkfactor; ++i) {
		    pixeltbl[i] = fg_current->palette[(i * 30 + mane.shrinkfactor * mane.shrinkfactor)
						      / (2 * mane.shrinkfactor * mane.shrinkfactor)];
		}
	    }
	}
    }	/* end if resource.use_grey */
    else
#endif /* GREY */
    {
	if (!fg_current->pixel_good) {
	    fg_current->pixel = alloc_color(&fg_current->color,
					    color_data[0].pixel);
	    fg_current->pixel_good = True;
	}

	if (globals.debug & DBG_DVI)
	    printf("do_color_change: fg = %lx, bg = %lx\n",
		   fg_current->pixel, bg_current->pixel);

	globals.gc.rule = set_or_make_gc(globals.gc.rule, GXcopy, fg_current->pixel, bg_current->pixel);

	set_bits = (Pixel) (fg_current->pixel & ~bg_current->pixel);
	clr_bits = (Pixel) (bg_current->pixel & ~fg_current->pixel);
	globals.gc.fore2 = NULL;

	if (resource.copy
	    || (set_bits && clr_bits && !resource.thorough)) {
	    if (!resource.copy) {
		/* I used to get a warning here which I didn't get for
		   xdvi-22.64/events.c, l.1330, but I can't reproduce
		   it any more ...
		*/
		warn_overstrike();
	    }
	    globals.gc.fore = set_or_make_gc(globals.gc.fore, GXcopy, fg_current->pixel, bg_current->pixel);
	}
	else {
	    if (set_bits) {
		globals.gc.fore = set_or_make_gc(globals.gc.fore, GXor, set_bits, 0);
		if (clr_bits) {
		    globals.gc.fore2 = globals.gc.fore2_bak1 = set_or_make_gc(globals.gc.fore2_bak1, GXandInverted, clr_bits, 0);
		}
	    }
	    else
		globals.gc.fore = set_or_make_gc(globals.gc.fore, GXandInverted, clr_bits, 0);
	}
    }

    fg_active = fg_current;
}

#elif GREY /* if COLOR */

void
init_pix(void)
{
    static int shrink_allocated_for = 0;
    static float oldgamma = 0.0;
    static Pixel palette[17];
    int i;

    if (G_visual->class == TrueColor) {
	/* This mirrors the non-grey code in xdvi.c */
	static int shift1_r, shift1_g, shift1_b;
	static int shift2_r, shift2_g, shift2_b;
	static Pixel set_bits;
	static Pixel clr_bits;
	unsigned int sf_squared;

	if (oldgamma == 0.0) {
	    mask_shifts(G_visual->red_mask, &shift1_r, &shift2_r);
	    mask_shifts(G_visual->green_mask, &shift1_g, &shift2_g);
	    mask_shifts(G_visual->blue_mask, &shift1_b, &shift2_b);

	    set_bits = color_data[0].pixel & ~(color_data[1].pixel);
	    clr_bits = color_data[1].pixel & ~(color_data[0].pixel);

	    if (set_bits & G_visual->red_mask)
		set_bits |= G_visual->red_mask;
	    if (clr_bits & G_visual->red_mask)
		clr_bits |= G_visual->red_mask;
	    if (set_bits & G_visual->green_mask)
		set_bits |= G_visual->green_mask;
	    if (clr_bits & G_visual->green_mask)
		clr_bits |= G_visual->green_mask;
	    if (set_bits & G_visual->blue_mask)
		set_bits |= G_visual->blue_mask;
	    if (clr_bits & G_visual->blue_mask)
		clr_bits |= G_visual->blue_mask;

	    /*
	     * Make the GCs
	     */

	    globals.gc.fore = globals.gc.fore2 = globals.gc.rule = 0;
	    globals.gc.copy = set_or_make_gc(NULL, GXcopy, resource.fore_Pixel, resource.back_Pixel);
	    if (globals.gc.do_copy || (set_bits && clr_bits)) {
		globals.gc.rule = globals.gc.copy;
		if (!resource.thorough)
		    globals.gc.do_copy = True;
	    }
	    if (globals.gc.do_copy) {
		globals.gc.fore = globals.gc.rule;
		if (!resource.copy) {
		    warn_overstrike();
		}
	    }
	    else {
		if (set_bits) {
		    globals.gc.fore = set_or_make_gc(NULL, GXor, set_bits & color_data[0].pixel, 0);
		}
		if (clr_bits || !set_bits) {
		    /* 		    fprintf(stderr, "using GXandInverted!\n"); */
		    *(globals.gc.fore ? &globals.gc.fore2 : &globals.gc.fore) =
			set_or_make_gc(NULL, GXandInverted, clr_bits & ~(color_data[0].pixel), 0);
		}
		if (!globals.gc.rule)
		    globals.gc.rule = globals.gc.fore;
	    }

	    oldgamma = resource.gamma;
	}

	if (mane.shrinkfactor == 1)
	    return;
	sf_squared = mane.shrinkfactor * mane.shrinkfactor;

	if (shrink_allocated_for < mane.shrinkfactor) {
	    if (pixeltbl != NULL) {
		free((char *)pixeltbl);
		if (pixeltbl_gc2 != NULL)
		    free((char *)pixeltbl_gc2);
	    }
	    pixeltbl = xmalloc((sf_squared + 1) * sizeof *pixeltbl);
	    shrink_allocated_for = mane.shrinkfactor;
	    if (globals.gc.fore2 != NULL) {
		pixeltbl_gc2 = xmalloc((sf_squared + 1) * sizeof *pixeltbl_gc2);
	    }
	}

	/*
	 * Initialize the pixel lookup table according to the gamma values.
	 */
#define	SHIFTIFY(x, shift1, shift2)	((((Pixel)(x)) >> (shift1)) << (shift2))

	for (i = 0; i <= sf_squared; ++i) {
	    double frac = resource.gamma > 0
		? pow((double)i / sf_squared, 1 / resource.gamma)
		: 1 - pow((double)(sf_squared - i) / sf_squared, -resource.gamma);
	    unsigned int red, green, blue;
	    Pixel pixel;

	    red = frac * ((double)color_data[0].red - color_data[1].red)
		+ color_data[1].red;
	    green = frac
		* ((double)color_data[0].green - color_data[1].green)
		+ color_data[1].green;
	    blue = frac * ((double)color_data[0].blue - color_data[1].blue)
		+ color_data[1].blue;

	    pixel = SHIFTIFY(red, shift1_r, shift2_r) |
		SHIFTIFY(green, shift1_g, shift2_g) |
		SHIFTIFY(blue, shift1_b, shift2_b);

	    if (globals.gc.do_copy) {
		pixeltbl[i] = pixel;
	    }
	    else if (globals.gc.fore2 != NULL) {	/* if thorough */
		pixeltbl[i] = pixel & ~(color_data[1].pixel);
		pixeltbl_gc2[i] = ~pixel & color_data[1].pixel;
	    }
	    else {
		pixeltbl[i] = set_bits ? pixel & set_bits : ~pixel & clr_bits;
	    }
	}

#undef	SHIFTIFY

	return;
    }

    /* if not TrueColor ... */

    if (resource.gamma != oldgamma) {
	XColor color;

	for (i = 0; i < 16; ++i) {
	    double frac = resource.gamma > 0
		? pow((double)i / 15, 1 / resource.gamma)
		: 1 - pow((double)(15 - i) / 15, -resource.gamma);

	    color.red = frac
		* ((double)color_data[0].red - color_data[1].red)
		+ color_data[1].red;
	    color.green = frac
		* ((double)color_data[0].green - color_data[1].green)
		+ color_data[1].green;
	    color.blue = frac
		* ((double)color_data[0].blue - color_data[1].blue)
		+ color_data[1].blue;

	    color.pixel = resource.back_Pixel;
	    color.flags = DoRed | DoGreen | DoBlue;

	    if (!globals.gc.do_copy) {
		if (i & 1)
		    color.pixel |= plane_masks[0];
		if (i & 2)
		    color.pixel |= plane_masks[1];
		if (i & 4)
		    color.pixel |= plane_masks[2];
		if (i & 8)
		    color.pixel |= plane_masks[3];
		XStoreColor(DISP, G_colormap, &color);
		palette[i] = color.pixel;
	    }
	    else {
		if (XAllocColor(DISP, G_colormap, &color))
		    palette[i] = color.pixel;
		else
		    palette[i] = (i * 100 >= resource.density * 15)
			? resource.fore_Pixel : resource.back_Pixel;
	    }
	}

	globals.gc.copy = set_or_make_gc(NULL, GXcopy, resource.fore_Pixel, resource.back_Pixel);
	globals.gc.rule = globals.gc.do_copy
	    ? globals.gc.copy
	    : set_or_make_gc(NULL, GXor, resource.fore_Pixel, resource.back_Pixel);
	globals.gc.fore = globals.gc.rule;
	globals.gc.fore2 = NULL;
	oldgamma = resource.gamma;
    }

    if (mane.shrinkfactor == 1)
	return;

    if (shrink_allocated_for < mane.shrinkfactor) {
	if (pixeltbl != NULL)
	    free((char *)pixeltbl);
	pixeltbl = xmalloc((unsigned)
			   (mane.shrinkfactor * mane.shrinkfactor + 1) * sizeof *pixeltbl);
	shrink_allocated_for = mane.shrinkfactor;
    }

    for (i = 0; i <= mane.shrinkfactor * mane.shrinkfactor; ++i) {
	pixeltbl[i] = palette[(i * 30 + mane.shrinkfactor * mane.shrinkfactor)
			      / (2 * mane.shrinkfactor * mane.shrinkfactor)];
    }
}

#endif /* COLOR */
