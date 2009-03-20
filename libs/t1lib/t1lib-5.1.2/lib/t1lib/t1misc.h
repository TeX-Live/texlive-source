/*--------------------------------------------------------------------------
  ----- File:        t1misc.h
  ----- Author:      Rainer Menzner (Rainer.Menzner@web.de)
  ----- Date:        2004-11-27
  ----- Description: This file is part of the t1-library. It contains
                     some miscellaneous definitions.
  ----- Copyright:   t1lib is copyrighted (c) Rainer Menzner, 1996-2004.
                     As of version 0.5, t1lib is distributed under the
		     GNU General Public Library Lincense. The
		     conditions can be found in the files LICENSE and
		     LGPL, which should reside in the toplevel
		     directory of the distribution.  Please note that 
		     there are parts of t1lib that are subject to
		     other licenses:
		     The parseAFM-package is copyrighted by Adobe Systems
		     Inc.
		     The type1 rasterizer is copyrighted by IBM and the
		     X11-consortium.
  ----- Warranties:  Of course, there's NO WARRANTY OF ANY KIND :-)
  ----- Credits:     I want to thank IBM and the X11-consortium for making
                     their rasterizer freely available.
		     Also thanks to Piet Tutelaers for his ps2pk, from
		     which I took the rasterizer sources in a format
		     independent from X11.
                     Thanks to all people who make free software living!
--------------------------------------------------------------------------*/
  

#include <setjmp.h>

#define LINE_BUF 512
#define BUF_SIZE 512
#define NULLPTR  (void *) 0


/* t1lib-glyphs are padded to this value by default: */
#define T1GLYPH_PAD   8
#define T1GLYPH_BIT   0 
#define T1GLYPH_BYTE  0 
#define PAD(bits, pad)  (((bits)+(pad)-1)&-(pad))


/* For debugging purposes: */
#define SLEEP for(j=0; j<50; j++){for (k=0; k<65536; k++);}


#define KERN_FLAG      0x01
#define LIGATURE_FLAG  0x02


/* common 'yes'/'no' */
#define T1_YES              0x1
#define T1_NO               0x0


/* The maximum length of a PostScript name of a font: */
#define MAXPSNAMELEN 256
#ifndef MAXPATHLEN
#    ifdef PATH_MAX
#        define MAXPATHLEN   PATH_MAX
#    else
#        define MAXPATHLEN   4096
#    endif
#endif

/* The default resolution used by the library */
#define DEFAULT_RES           72.0
#define BIGPOINTSPERINCH      72.0

/* Defaults for configuration file searching: */
/* (Note: The directory where the global config will be located is
   determined by configure at compile time) */
#define T1_CONFIGFILENAME    ".t1librc"
#define ENV_CONF_STRING      "T1LIB_CONFIG"
#define GLOBAL_CONFIG_FILE   "t1lib.config"
/* Directory separator is operating system dependent */
#if defined(VMS)
# define DIRECTORY_SEP        "]"
# define DIRECTORY_SEP_CHAR   ']'
#elif defined(MSDOS) | defined(_WIN32) | defined(_MSC_VER)
# define DIRECTORY_SEP        "\\"
# define DIRECTORY_SEP_CHAR   '\\'
#else
# define DIRECTORY_SEP        "/"
# define DIRECTORY_SEP_CHAR   '/'
#endif
/* For EMX-, WIN32-, MSDOS-environment, we define the default path separator to
   be ';' instead of ':' */
#if defined(MSDOS) | defined(_WIN32) | defined(__EMX__) | defined(_MSC_VER) | defined(VMS) 
#define PATH_SEP             ";"
#define PATH_SEP_CHAR        ';'
#else
#define PATH_SEP             ":"
#define PATH_SEP_CHAR        ':'
#endif
/* This non printable character is used internally */
#define INTERNAL_PATH_SEP            "\001"
#define INTERNAL_PATH_SEP_CHAR       '\1'


/* Default name for t1lib-logfile: */
#define ENV_LOG_STRING    "T1LIB_LOGMODE"
#define T1_LOG_FILE       "t1lib.log"
/* logfile levels: */
#define T1LOG_ERROR     1
#define T1LOG_WARNING   2
#define T1LOG_STATISTIC 3
#define T1LOG_DEBUG     4


/* def's for T1_InitLib() */
#define NO_LOGFILE          0x0
#define LOGFILE             0x1
#define IGNORE_FONTDATABASE 0x2  /* Default is to read database */
#define IGNORE_CONFIGFILE   0x4  /* Default is to read config file */
#define T1_AA_CACHING       0x8  /* Cache aa-bytes */
#define T1_NO_AFM           0x10 /* Don't load AFM files or generate AFM data */


/* How may ligatures a character may have at maximum: */
#define MAX_LIGS 20


/* The maximum number of gray-levels in antialiasing */
#define AAMAXPLANES 17
#define T1_AA_NONE  1
#define T1_AA_LOW   2
#define T1_AA_HIGH  4

/* The default limits for no/low/high AA */
#define T1_AA_SMARTLIMIT1       20.0
#define T1_AA_SMARTLIMIT2       60.0

/* How much to advance the number of FONTPRIVATE structures when
   reallocating for new fonts? */
#define ADVANCE_FONTPRIVATE   10

#define NO_ANTIALIAS 0
#define ANTIALIAS    1


/* #define's for manipulating searchpaths */
#define T1_PFAB_PATH        0x01
#define T1_AFM_PATH         0x02
#define T1_ENC_PATH         0x04
#define T1_FDB_PATH         0x08
#define T1_APPEND_PATH      0x00
#define T1_PREPEND_PATH     0x01


/* #define's for character/string properties (not all implemented) */
#define T1_DEFAULT             0x0000
#define T1_UNDERLINE           0x0001
#define T1_OVERLINE            0x0002
#define T1_OVERSTRIKE          0x0004
#define T1_DOUBLEOVERSTRIKE    0x0008
#define T1_RIGHT_TO_LEFT       0x0010
#define T1_SUBSCRIPT           0x0100
#define T1_SUPERSCRIPT         0x0200
#define T1_STROKED             0x1000
#define T1_KERNING             0x2000


/* Error handling: */
/* These are from scanning a font file */
#define T1ERR_SCAN_FONT_FORMAT       -5
#define T1ERR_SCAN_FILE_OPEN_ERR     -4
#define T1ERR_SCAN_OUT_OF_MEMORY     -3
#define T1ERR_SCAN_ERROR             -2
#define T1ERR_SCAN_FILE_EOF          -1
/* These are from generating paths */
#define T1ERR_PATH_ERROR              1
#define T1ERR_PARSE_ERROR             2
#define T1ERR_TYPE1_ABORT             3
/* These are from t1lib */
#define T1ERR_INVALID_FONTID          10
#define T1ERR_INVALID_PARAMETER       11
#define T1ERR_OP_NOT_PERMITTED        12
#define T1ERR_ALLOC_MEM               13
#define T1ERR_FILE_OPEN_ERR           14
#define T1ERR_UNSPECIFIED             15
#define T1ERR_NO_AFM_DATA             16
#define T1ERR_X11                     17
#define T1ERR_COMPOSITE_CHAR          18
#define T1ERR_SCAN_ENCODING           19


/* The info_flags meaning */
#define AFM_SUCCESS               (short)0x0001
#define AFM_SLOPPY_SUCCESS        (short)0x0002
#define AFM_SELFGEN_SUCCESS       (short)0x0004
#define USES_STANDARD_ENCODING    (short)0x0008
#define RASTER_STROKED            (short)0x0010
#define CACHE_STROKED             (short)0x0020
#define FONT_NOCACHING            (short)0x0100

#ifndef PI
#define PI 3.1415927
#endif


/* Flags to control the rasterizer */
#define T1_IGNORE_FORCEBOLD           0x0001
#define T1_IGNORE_FAMILYALIGNMENT     0x0002
#define T1_IGNORE_HINTING             0x0004

#define T1_DEBUG_LINE                 0x0100
#define T1_DEBUG_REGION               0x0200
#define T1_DEBUG_PATH                 0x0400
#define T1_DEBUG_FONT                 0x0800
#define T1_DEBUG_HINT                 0x1000


/*   */
#define DO_RASTER                     0x1
#define DO_NOT_RASTER                 0x0


/* this definition reports a character substitution from fontfcn */
#define FF_NOTDEF_SUBST               -1
