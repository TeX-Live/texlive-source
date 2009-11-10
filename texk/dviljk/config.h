/* $Id: config.h,v 2.1 1996/05/20 11:05:07 neumann Exp $ */

/* gcc -ansi doesn't predefine `unix', since ANSI forbids it.  And AIX
   generally doesn't predefine unix, who knows why.  HP-UX is, of course,
   also different.  Apple's MacOsX is also unix-like.  */
#ifndef unix
#if defined (__unix__) || defined (_AIX) || defined (_HPUX_SOURCE)
#define unix
#elif defined (__APPLE__) && defined (__MACH__)
#define unix
#elif defined (__NetBSD__)
#define unix
#endif
#endif

#define USEPXL

#ifdef __riscos
/* #define RISC_USE_OSL */        /* use file functions of OSlib */
/* #define RISC_BUFFER  */        /* buffer output */
#endif

/*
 *  default font path
 *  can be overridden by environment variable TEXPXL
 *  or -a command line option
 *  popular choice:
 * #define  FONTAREA       "/usr/local/lib/tex/fonts/pk"
 */
#ifndef FONTAREA
# ifdef __riscos
#  define FONTAREA    "TeXFonts:"
# else
#  ifdef vms
#   define FONTAREA    "tex$pkdir:"
#  else
#   ifdef _AMIGA
#    define FONTAREA    "TeXMF:pk"
#   else
#    define FONTAREA    "/usr/local/lib/tex/fonts/pk"
#   endif
#  endif
# endif
#endif

/* if DO_SUBDIRECTORIES is specified, search to this depth */
#ifndef MAX_SUBDIR_SEARCH_DEPTH
#define MAX_SUBDIR_SEARCH_DEPTH 10
#endif

/*
 * if your LaserJet II P or LaserJet III or LaserJet 2000
 * complains about not enough memory, you might try to reduce
 * the value below or uncomment to use the default settings
 */
#if defined (LJ2P) || defined (LJ4)
#define  MAX_FONTS_PER_PAGE 255         /* maximum number of fonts per page */
#endif


/* Timing is not very portable.... if you have troubles, use
 * -DNO_TIMING in the Makefile
 */

#ifdef u3b2
#define NO_TIMING
#endif
#ifdef _AMIGA
#define NO_TIMING
#endif
#ifdef KPATHSEA
#define NO_TIMING
#define MAKETEXPK
#endif
#ifdef __riscos
#define NO_TIMING
#endif

#ifndef NO_TIMING
#define TIMING
#endif

/*
 * per default use mktexpk in unix environments unless it is not wanted
 */
#ifdef unix
#ifndef MAKETEXPK
/* name of the program which is called to generate missing pk files
 */
#define MAKETEXPK "mktexpk"
#endif
#endif

#ifdef _AMIGA
#ifndef MAKETEXPK
/* name of the program which is called to generate missing pk files
 */
#define MAKETEXPK "mktexpk"
#endif
#endif

#ifdef __riscos
#ifndef MAKETEXPK
#define MAKETEXPK "mktexpk"
#endif
#endif

#ifdef NO_MAKETEXPK
#undef MAKETEXPK
#endif

/*
 * assure that LJ2P is defined when LJ4 is defined;
 * compile with support for LJ4's resident fonts
 */
#ifdef LJ4
#define LJ2P
#define LJ_RESIDENT_FONTS
#endif

/*
 * assure that LJ2 is defined when LJ2P is defined
 */
#ifdef LJ2P
#ifndef LJ2
#define LJ2
#endif
#endif

/*
 * assure that LJ is defined when LJ2 of LJ4 is defined
 */
#if defined(LJ2)
#ifndef LJ
#define LJ
#endif
#endif

/*
 * assure that IBM3812 is not defined when LJ is defined
 */
#ifdef LJ
#ifdef IBM3812
#undef IBM3812
#endif
#endif

#ifdef LJ_RESIDENT_FONTS
# ifndef DEFAULT_TFM_PATH
#  ifdef __riscos
#   define FONTAREA    "TeXFonts:"
#  else
#   define DEFAULT_TFM_PATH   "/usr/local/lib/tex/fonts"
#  endif
# endif
#endif


#define  _TRUE      (bool) 1
#define  _FALSE     (bool) 0
#define  UNKNOWN     -1

#define  STRSIZE         255     /* stringsize for file specifications  */


#ifdef __riscos
# ifndef types_H                  /* for compatibility with OSLib */
typedef  int     bool;
# endif
#else
typedef  char    bool;
#endif


/* The smallest signed type: use `signed char' if ANSI C, `short' if
   char is unsigned, otherwise `char'.  */
#ifndef SCHAR_TYPE
#if __STDC__
#define SCHAR_TYPE signed char
#else /* not __STDC */
#ifdef __CHAR_UNSIGNED__
#define SCHAR_TYPE short
#else
#define SCHAR_TYPE char
#endif
#endif /* not __STDC__ */
#endif /* not SCHAR_TYPE */
typedef SCHAR_TYPE signed_char;

#if !defined(u3b2) && !defined(LONG_64_BITS)
#define  ARITHMETIC_RIGHT_SHIFT
#endif

#ifndef SIZEOF_LONG
# ifdef __riscos
#  define SIZEOF_LONG 8
# endif
#endif

#if SIZEOF_LONG > 4
# define long4 int
# define FMT_long4 "%d"
#else
# define long4 long
# define FMT_long4 "%ld"
#endif

#include <stdarg.h>

#ifndef KPATHSEA
#error "Would need changed findfile, dviljk has changed allocation semantic of name member in tfontptr"
#endif


/*
 * maximal number of characters in font file
 * #define  LASTFNTCHAR  127        7-bit classic version
 * #define  LASTFNTCHAR  255        8-bit fonts
 */
#ifdef SEVENBIT
# define LASTFNTCHAR 127
#else
# define LASTFNTCHAR  255
#endif



/* this information is needed in findfile.c and dvi2xx.c, NO CUSTOMIZATION */
#ifdef LJ
# ifndef MFMODE300
#  define MFMODE300 "cx"     /* mode definition for metafont 300dpi */
# endif
# ifdef LJ4
#  ifndef MFMODE600
#   define MFMODE600 "ljfour"    /* mode definition for metafont 600dpi */
#  endif
# else
#  define MFMODE MFMODE300       /* default mode */
# endif
#endif

#ifdef IBM3812
#define RESOLUTION    240
#ifndef MFMODE
#define MFMODE "IBMThreeEightOneTwo"    /* mode definition for metafont */
#endif
#endif

#if defined (unix) && !defined (__DJGPP__)
#define OS "Unix"
#define READ_BINARY     "r"
#define WRITE_BINARY    "w"
#ifndef labs
#define labs(x) abs(x)
#endif
#endif

#ifdef MSDOS
# define OS "MS-DOS"
# define READ_BINARY     "rb"
# define WRITE_BINARY    "wb"
# ifdef __DJGPP__
#  include <io.h>		/* for prototype of `setmode' */
#  define AssureBinary(f) setmode((f), O_BINARY)
# else
#  define MSC5			/* assume Microsoft C */
# endif
#endif
#ifdef OS2
#define OS "OS/2"
#define READ_BINARY     "rb"
#define WRITE_BINARY    "wb"
#define MSC5
#endif
#ifdef WIN32
#define OS "Win32"
#define READ_BINARY     "rb"
#define WRITE_BINARY    "wb"
#  include <io.h>		/* for prototype of `_setmode' */
#  define AssureBinary(f) _setmode((f), _O_BINARY)
#define MSC10
#endif

#ifdef vms
#define OS "VMS"
#include <ssdef.h>
#include <stsdef.h>
#define ftell vms_ftell		    /* use some external routines, because */
#define fseek vms_fseek		    /* of some bugs in the VMS run time    */
#define getchar vms_getchar	    /* library */
#define getenv vms_getenv
#define ungetc vms_ungetc
#define getname vms_getname
#define READ_BINARY     "rb"
#define WRITE_BINARY    "wb","rfm=fix","bls=512","mrs=512" /* fixed records */
#define labs(x) abs(x)
#endif

#ifdef _AMIGA
#define OS "Amiga"
#define READ_BINARY     "r"
#define WRITE_BINARY    "w"
#ifdef __SASC
#define sys_errlist __sys_errlist
#include <stdlib.h>
#endif
#endif

#ifdef unix
# include <stdio.h>
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef __riscos
# define OS "RISC OS"
# ifdef RISC_USE_OSL
#  define READ_BINARY     0x43  /* if using fopen stuff reset to "rb", "wb" */
#  define WRITE_BINARY    0x83
# else
#  define READ_BINARY     "rb"
#  define WRITE_BINARY    "wb"
# endif
# define ERR_STREAM stdout   /* ???? */
# else
# define ERR_STREAM stderr
#endif

/*
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
*/

#ifdef KPATHSEA
#define BOPENCMD xfopen
#else
#define BOPENCMD fopen /* ???? */
#endif

/* Set up file stuff */
#ifdef RISC_USE_OSL
#include "fromosl.h"
typedef  os_f FILEPTR;
#define   read_byte(fp,char) os_bget(fp,&char)
#define  write_byte(fp,char) os_bput(char,fp)
#define  read_multi(buf,n,s,fp) osgbpb_read(fp,(byte*)buf,(s)*(n))
#define write_multi(buf,n,s,fp) osgbpb_write(fp,(byte*)buf,(s)*(n))
#define   BINOPEN(f) osfind_openin(READ_BINARY,f,NULL)
#define  BOUTOPEN(f) osfind_openout(WRITE_BINARY,f,NULL)
#define    BCLOSE(f) osfind_close(f)
#define      FEOF(f) osargs_read_eof_status(f)
#define FSEEK(f,pos,whence) os_seek(f,pos,whence)
#define     FTELL(f) osargs_read_ptr(f)
#define FPNULL 0
#else
typedef  FILE *FILEPTR;
#define   read_byte(fp,char) char = fgetc(fp)
#define  write_byte(fp,char) fputc(char,fp)
#define  read_multi(buf,n,s,fp) fread(buf,n,s,fp)
#define write_multi(buf,n,s,fp) fwrite(buf,n,s,fp)
#define   BINOPEN(f) BOPENCMD(f,READ_BINARY)
#define  BOUTOPEN(f) BOPENCMD(f,WRITE_BINARY)
#define    BCLOSE(f) fclose(f)
#define      FEOF(f) feof(f)
#define FSEEK(f,pos,whence) fseek(f,pos,whence)
#define     FTELL(f) ftell(f)
#define FPNULL NULL
#endif


#define BUFFSIZE 8192  /* ???? */

#ifndef SEEK_SET
# define SEEK_SET 0
#endif
#ifndef SEEK_CUR
# define SEEK_CUR 1
#endif
#ifndef SEEK_END
# define SEEK_END 2
#endif

#ifndef O_BINARY
# ifdef _O_BINARY
#  define O_BINARY _O_BINARY
# else
#  define O_BINARY 0
#endif
#endif

#if !defined (AssureBinary) && !defined (MSC5)
# define AssureBinary(f)  0
#endif

#ifndef HAVE_VPRINTF
#ifdef HAVE_DOPRNT
#define	vfprintf(stream, message, args)	_doprnt(message, args, stream)
/* If we have neither, should fall back to fprintf with fixed args.  */
#endif
#endif

/* If unlink and rmdir are not there, we don't delete the temporary files. */
#ifndef HAVE_RMDIR
#define rmdir(dir)
#endif
#ifndef HAVE_UNLINK
#define unlink(file)
#endif

/* If mkdtemp() does not exist, we have to use mktemp() or tmpnam()
   and mkdir(). For the latter, we need the declaration. */
#ifndef HAVE_MKDTEMP
#include <sys/types.h>
#include <sys/stat.h>
#endif

#ifndef KPATHSEA
/* FIXME: Should provide a strdup function. But currently this tree is
   only used in connection with kpathsea anyhow. */
#error "Need xstrdup and xmalloc function, e.g. from kpathsea"
#endif
