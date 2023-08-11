/*
 *   This program converts dvi files to dvi files;
 *   the command line options are:
 *
 *      page n is first page selected         -f n
 *      page n is last page selected          -l n
 *      print at most n pages                 -n n
 *      include pages (ranges allowed)        -i { n1..n2 | n1 }[,...]
 *      exclude pages (ranges allowed)        -x { n1..n2 | n1 }[,...]
 *      work in quiet mode                    -q
 *      reverse pages                         -r
 *      select even                           -m 2:1
 *      select odd                            -m 2:0
 *      print both on same page               -m 2:0,1(5.5in,0in)
 *      do folded brochures                   -m 4:-3,0(5.5in,0in)
 *                                            -m 4:1,-2(5.5in,0in)
 *      round up number of pages to a
 *        multiple of n                       -p n
 *      sort pages to print one from top      -j
 *        next one from end
 *      etc.
 *
 *    The original program is by Tomas Rokicki (version 0.5) but it was
 *    modified and improved by Esteban ZIMANYI ezimanyi@rc1.vub.ac.be
 *    to give version 1.0.
 *    To give version 1.1 it was improved by Markus_Kohm@ka.maus.de.
 *
 *    This version has been tested for the IBM PC and compatibles under
 *    compilers Turbo C 2.0 and Microsoft C 6.0 and for Atari ST under
 *    compiler Turbo C 2.0 with MiNTLIB.
 *
 */
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define BANNER   "\nThis is dvidvi " VERSION " (" TL_VERSION ")," \
                 " Copyright 1988-1991 Radical Eye Software\n" \
                 "Improved from 1.0 to 1.1, 1994, Markus Kohm\n"
#define BUG_ADDRESS  PACKAGE_BUGREPORT
#define STRINGSIZE (500L)  /* maximum number of strings in program */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef VMS
#include <stdlib.h>
#ifdef WIN32
#include <malloc.h>
#endif
#include <stdio.h>
#include <string.h>
#else /* VMS */
#include "sys$library:stdio.h"       /* AKT: added sys$library: */
#include <alloc.h>
#endif /* VMS */
#define MAXPPERP (32)

void error(const char *);

/* defines READBIN, WRITEBIN, PATHSEP and IS_DIR_SEP*/

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef KPATHSEA
#include <kpathsea/c-fopen.h>
#define READBIN         FOPEN_RBIN_MODE
#ifdef VMS
#define WRITEBIN        "wb"	/* VMS doesn't have FOPEN_WBIN_MODE defined */
#else
#define WRITEBIN        FOPEN_WBIN_MODE
#endif
#include <kpathsea/c-pathch.h>
#define PATHSEP         ENV_SEP
#ifdef WIN32
#include <kpathsea/config.h>
#include <kpathsea/variable.h>
#undef fopen
#undef fprintf
#define fopen    fsyscp_fopen
#define fprintf  win32_fprintf
#endif
#else  /* not KPATHSEA */
#if defined(__TOS__)
#define READBIN         "rb"    /* TOS must use binary mode */
#define WRITEBIN        "wb"
#define PATHSEP         ';'
#define IS_DIR_SEP(c)   ((c) == '\\')
#elif defined (MSDOS) || defined(WIN32)
#define READBIN         "rb"    /* MSDOS must use binary mode */
#define WRITEBIN        "wb"
#define PATHSEP         ';'
#define IS_DIR_SEP(c)   ((c) == '\\' || (c) == '/')
#elif defined(VMS)
#define READBIN         "rb"    /* VMS must use binary mode */
#define WRITEBIN        "wb"
#define PATHSEP         ','
#define IS_DIR_SEP      ((c) == ':')
#else
#define READBIN         "r"     /* UNIX doesn't care */
#define WRITEBIN        "w"
#define PATHSEP         ':'
#define IS_DIR_SEP      ((c) == '/')
#endif
#endif /* not KPATHSEA */

/*
 *   Type declarations.  integer must be a 32-bit signed; shalfword must
 *   be a sixteen-bit signed; halfword must be a sixteen-bit unsigned;
 *   quarterword must be an eight-bit unsigned.
 */
typedef long integer;
typedef short shalfword ;
typedef unsigned short halfword ;
typedef unsigned char quarterword ;
typedef short Boolean ;

/*
 *   Some globals to keep everyone happy.
 */
integer numpages ;      /* the total number of pages in the dvi file. */
integer TeXfonts[256] ; /* information about each font */
char fontseen[256] ; /* have we defined this font yet? */
int modulo ;         /* our mod value */
integer pagemodulo ;     /* additional mod value for number of pages */
struct pagespec {
   int pageno, reversed ;
   long hoffset, voffset ; /* in scaled points */
} pages[MAXPPERP] ;  /* the organization of the pages on output */
int pagesperpage ;   /* how many pages crammed onto each page? */
FILE *infile ;       /* input dvi file (cannot be a stream) */
FILE *outfile ;      /* output dvi file */

char *temp ;    /* a temporary place to put things */
char *nextstring, *maxstring ;
const char *oname=NULL ;      /* output dvi file name */
char *iname ;                 /* input dvi file name */
char *strings ;               /* pointer of the string pool */

const char banner[] = BANNER ;      /* the startup message */
integer inlength ;      /* the length of the input dvi file */
integer postloc ;       /* location of the postamble */
integer mag ;           /* magnification factor */
integer pagecount ;     /* number of actual pages */
integer landscape = 0;     /* if landscape special, here it is! */
int rem0special ;       /* should we remove the first first-page special? */
integer prevpp = -1 ;      /* previous page pointer on output */
integer outputpages ;   /* number of pages output */
integer dviloc ;        /* our position in the output file */
integer pagefake ;      /* number of pages, rounded up to multiple of 
			   pagemodulo */

Boolean firsttransf = 0, lasttransf = 0;
Boolean jumpingpages = 0;
integer firstpage ;     /* first page selected (option -p) */
integer lastpage ;      /* last page selected (option -l) */
integer maxpages ;      /* maximum number of page selected (option -n) */
short quiet ;             /* quiet mode (option -q) */
Boolean exctransf[40][2] ; /* if the ranges of pages to exclude (option -x)
				have to be transformed */
integer exclude[40][2] ; /* the ranges of pages to exclude (option -x)
                         It is supposed that there are at most 40 ranges
                         to exclude in the command line */
short excludeseq ;       /* number of ranges to exclude (option -x) */
Boolean inctransf[40][2] ; /* if the ranges of pages to exclude (option -x)
				have to be transformed */
integer include[40][2] ; /* the ranges of pages to include (option -i)
                         It is supposed that there are at most 40 ranges
			 to include in the command line */
short includeseq ;       /* number of ranges to include (option -i) */
integer *pageloc ;
integer *pagenumbers ;
int prettycolumn ;       /* the column we are at when running pretty */

#ifdef ASCIIPTEX
int ptexdvi ;            /* true if dvi file is extended (TATEKUMI) */
#endif

/*
 *   This array holds values that indicate the length of a command, if
 *   we aren't concerned with that command (which is most of them) or
 *   zero, if it is a special case.  This makes running through the
 *   dvi file a lot easier (and probably faster) than any form of
 *   dispatch table, especially since we really don't care what the
 *   pages are made of.
 */
short comlen[256] = {
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0-15 */
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 16-31 */
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 32-47 */
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 48-63 */
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 64-79 */
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 80-95 */
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 96-111 */
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 112-127 */
   2, 3, 4, 5, 9, 2, 3, 4, 5, 9, 1, 0, 0, 1, 1, 2, /* 128-143 */
   3, 4, 5, 1, 2, 3, 4, 5, 1, 2, 3, 4, 5, 2, 3, 4, /* 144-159 */
   5, 1, 2, 3, 4, 5, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, /* 160-175 */
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 176-191 */
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 192-207 */
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 208-223 */
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 4, 5, 0, /* 224-239 */
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };/* 240-255 */

/*
 *   Input bytes from the dvi file.
 *   These routines could probably be sped up significantly; but they are
 *   very machine dependent, so I will leave such tuning to the installer.
 *   They simply get and return bytes in batches of one, two, three, and four,
 *   updating the current position as necessary.
 */

static void
abortpage(void)
{
   error("! unexpected eof on DVI file") ;
}

static shalfword dvibyte(void)
{
  register shalfword i ;
  if ((i=getc(infile))==EOF)
    abortpage() ;
  return(i) ;
}

static halfword  twobytes(void)
{
  register halfword i ;
  i = dvibyte() ;
  return(i*256+dvibyte()) ; }

#if 0 /* not used */
static integer   threebytes(void)
{
  register integer i ;
  i = twobytes() ;
  return(i*256+dvibyte()) ; }
#endif

static shalfword
signedbyte(void)
{
  register shalfword i ;
  if ((i=getc(infile))==EOF)
    abortpage() ;
  if (i<128) return(i) ;
  else return(i-256) ;
}

static shalfword
signedpair(void)
{
  register shalfword i ;
  i = signedbyte() ;
  return(i*256+dvibyte()) ;
}

#if 0 /* not used */
static integer
signedtrio(void)
{
  register integer i ;
  i = signedpair() ;
  return(i*256+dvibyte()) ;
}
#endif

static integer
signedquad(void)
{
  register integer i ;
  i = signedpair() ;
  return(i*65536+twobytes()) ;
}

/*
 * Routines for the transformation of the pages specified with
 * the @ parameter into the actual number of page from the file
 *
 * trans(p,q) looks for the q'th occurrence of the page numbered p
 * transformpages makes the transformation for the options -f -i -l -x
 */

static integer transf(integer p, halfword q)
{
int i=0, j=0;

   while( j != q && i < pagecount )
      if ( pagenumbers[i++] == p ) j++;
   if ( j != q ) error ("! Page specified not found");
   return (i++);
}

static void transformpages(void)
{
int i;

   if (firsttransf) firstpage=transf(firstpage,firsttransf);
   if (lasttransf) lastpage=transf(lastpage,lasttransf);
   for ( i= 0; i< 40; i++) {
      if (exctransf[i][0]) exclude[i][0]=transf(exclude[i][0],exctransf[i][0]);
      if (exctransf[i][1]) exclude[i][1]=transf(exclude[i][1],exctransf[i][1]);
      if (inctransf[i][0]) include[i][0]=transf(include[i][0],inctransf[i][0]);
      if (inctransf[i][1]) include[i][1]=transf(include[i][1],inctransf[i][1]);
      }
}
/*
 *
 */
static integer fontdeflen(integer p)
{
   fseek(infile,p+14,SEEK_SET);
   return (16L+dvibyte()+dvibyte());
}

/*
 *       Simulation of dvibuf, get the character located at the position
 *       p in the input dvi file
 */
static unsigned char dvibuf(integer p)
{
  fseek(infile,p,SEEK_SET);
  return(dvibyte());
}

/*
 *       Read a string of length n from the file into the string temp
 */
static void stringdvibuf(integer p, integer n)
{
  fseek(infile,p,SEEK_SET);
  while ( n-- > 0 )
    *temp++ = dvibyte();
}

/*
 *   Print a usage error messsage, and quit.
 */
static void usage(void) {
   (void)fprintf(stderr,banner);
   (void)fprintf(stderr,"Usage:  dvidvi [options] input[.dvi] [output]\n");
   (void)fprintf(stderr,"where options are:\n");
   (void)fprintf(stderr,"    [-f n] first page printed     [-l n] last page printed\n");
   (void)fprintf(stderr,"    [-n n] print at most n pages  \n");
   (void)fprintf(stderr,"    [-i {n1..n2 | n3}[,...]] include pages\n");
   (void)fprintf(stderr,"    [-x {n1..n2 | n3}[,...]] exclude pages\n");
   (void)fprintf(stderr,"    [-q] quiet mode               [-r] reverse pages\n");
   (void)fprintf(stderr,"    [-m modulo:pagespecs]         [-j] jumping pages\n");
   (void)fprintf(stderr,"    [-p pagemodulo]\n");
   (void)fprintf(stderr,"\nEmail bug reports to %s.\n", BUG_ADDRESS);
#ifndef VMS
   exit(1) ;
#else /* VMS */
   exit(0x10000002) ;
   /* AKT: was exit(1) */
#endif /* VMS */
}
/*
 *   Print an error message, and exit if it is fatal.
 */
void error(const char *s)
{
   (void)fprintf(stderr, "%s\n", s) ;    /* AKT: was dvidvi: %s */
   if (*s == '!')
#ifndef VMS
      exit(1) ;
#else /* VMS */
      exit(0x10000002) ;
      /* AKT: was exit(1) */
#endif /* VMS */
}
/*
 *   This function calculates approximately (whole + num/den) * sf.
 *   No need for real extreme accuracy; one ten thousandth of an
 *   inch should be sufficient.
 *
 *   No `sf' parameter means to use an old one; inches are assumed
 *   originally.
 *
 *   Assumptions:
 *
 *      0 <= num < den <= 10000
 *      0 <= whole
 */
integer defaultscale = 4736286 ;
static integer scale(integer whole, integer num, integer den, integer sf)
{
   integer v ;

   if (sf)
      defaultscale = sf ;
   else
      sf = defaultscale ;
   v = whole * sf + num * (sf / den) ;
   if (v / sf != whole || v < 0 || v > 0x40000000L)
      error("! arithmetic overflow in parameter") ;
   sf = sf % den ;
   v += (sf * num * 2 + den) / (2 * den) ;
   return(v) ;
}
/*
 *   Multiplies *p by 1000 and divides it by mag.  Avoiding overflow.
 *
 *   1 <= mag <= 1000000 ;
 *   0 <= *p <= 2^30
 *
 *   (Could blow up if a parameter * mag / 1000 > 2^30 sp.)
 */
static void scalemag(long *p)
{
   int negative ;

   negative = 0 ;
   if (*p < 0) {
      negative = 1 ;
      *p = - *p ;
   }
   *p = 1000 * (*p / mag) + (2000 * (*p % mag) + mag) / (2 * mag) ;
   if (negative)
      *p = - *p ;
}
/*
 *   Convert a sequence of digits into an integer; return -1 if no digits.
 *   Advance the passed pointer as well.
 */
static integer myatol(char **s)
{
   register char *p ;
   register integer result ;

   result = 0 ;
   p = *s ;
   while ('0' <= *p && *p <= '9') {
      if (result > 100000000)
         error("! arithmetic overflow in parameter") ;
      result = 10 * result + *p++ - '0' ;
   }
   if (p == *s)
      usage() ;
   *s = p ;
   return(result) ;
}
/*
 *   Get a dimension, allowing all the various extensions, and
 *   defaults.
 */
static integer myatodim(char **s)
{
   register integer w, num, den ;
   register char *p ;
   int negative = 0 ;

   p = *s ;
   if (**s == '-') {
      (*s)++ ;
      negative = 1 ;
   }
   w = myatol(s) ;
   if (w < 0)
      usage() ;
   p = *s ;
   num = 0 ;
   den = 1 ;
   if (*p == '.') {
      p++ ;
      while ('0' <= *p && *p <= '9') {
         if (den < 1000) {
            den *= 10 ;
            num = num * 10 + *p - '0' ;
         }
         p++ ;
      }
   }
/*
 *   Allowed units are `in', `cm', `mm', `pt', `sp', `cc', `dd', and `pc';
 *   must be in lower case.
 */
   if (*p == 'c' && p[1] == 'm') {
/*  centimeters need to be multiplied by 72.27 * 2^16 / 2.54, or 1 864 680 */
      w = scale(w, num, den, 1864680L) ;
   } else if (*p == 'p' && p[1] == 't') {
/*  real points need to be multiplied by 2^16 */
      w = scale(w, num, den, 65536L) ;
   } else if (*p == 'p' && p[1] == 'c') {
/*  picas need to be multiplied by 65536 * 12, or 786 432 */
      w = scale(w, num, den, 786432L) ;
   } else if (*p == 'm' && p[1] == 'm') {
/*  millimeters need to be multiplied by 72.27 * 2^16 / 25.4, or 186 468 */
      w = scale(w, num, den, 186468L) ;
   } else if (*p == 's' && p[1] == 'p') {
/*  scaled points are already taken care of; simply round */
      w = scale(w, num, den, 1L) ;
   } else if (*p == 'b' && p[1] == 'p') {
/*  big points need to be multiplied by 72.27 * 65536 / 72, or 65782 */
      w = scale(w, num, den, 65782L) ;
   } else if (*p == 'd' && p[1] == 'd') {
/*  didot points need to be multiplied by 65536 * 1238 / 1157, or 70124 */
      w = scale(w, num, den, 70124L) ;
   } else if (*p == 'c' && p[1] == 'c') {
/*  cicero need to be multiplied by 65536 * 1238 / 1157 * 12, or 841 489 */
      w = scale(w, num, den, 841489L) ;
   } else if (*p == 'i' && p[1] == 'n') {
/*  inches need to be multiplied by 72.27 * 65536, or 4 736 286 */
      w = scale(w, num, den, 4736286L) ;
   } else {
/*  use default values */
      w = scale(w, num, den, 0L) ;
      p -= 2 ;
   }
   p += 2 ;
   *s = p ;
   return(negative?-w:w) ;
}
/*
 * This function determine if the page has to be printed
 * depending on the values of options -f, -l, -i and -x
 */

static short selectedpage (integer n)
{
short i;

   if ( firstpage > n || n > lastpage ) return(0);
   for ( i=0 ; i < excludeseq ; i++ )
      if ( exclude[i][0] <= n && n <= exclude[i][1] )
	 return(0);
   if (includeseq == 0) return (1);
   for ( i=0 ; i < includeseq ; i++ )
      if ( include[i][0] <= n && n <= include[i][1] )
	 return(1);
   return(0);
}

/*
 *   Initialize sets up all the globals and data structures.
 */
static void
initialize(void)
{
int i;
/* initialize values in case of option -m is not specified */
   modulo = 1 ;
   pagemodulo = 0;
   pages[0].hoffset = 0 ;
   pages[0].voffset = 0 ;
   pages[0].pageno = 0 ;
   pages[0].reversed = 0 ;
   pagesperpage = 1 ;
   for ( i= 0; i< 40; i++) {
	exctransf[i][0] = exctransf[i][1] = 0;
	inctransf[i][0] = inctransf[i][1] = 0;
	}
   firsttransf = lasttransf = 0;

   excludeseq = 0;
   includeseq = 0;
   firstpage = 1;
   lastpage = 1000000;
   maxpages = 1000000;
   quiet = 0;
   strings =(char *) malloc(STRINGSIZE) ;
   if (strings == 0)
      error("! no memory for strings") ;
   maxpages = 100000 ;
   nextstring = strings ;
   iname = strings ;
   *nextstring++ = 0 ;
    maxstring = strings + STRINGSIZE - 200;
}

/*
 *   Parse the arguments to the routine, and stuff everything away
 *   into those globals above.
 */
static void processargs(int argc, char *argv[])
{
   char *q ;
   int i, pageno, lastext = -1 ;
   long hoffset, voffset ;
   int reversed ;

   initialize ();
   if (argc < 2 || argc > 8)
      usage() ;

/*
 *   This next whole big section of code is straightforward; we just scan
 *   the options.  An argument can either immediately follow its option letter
 *   or be separated by spaces.  Any argument not preceded by '-' and an
 *   option letter is considered a file name.
 */
   for (i=1; i<argc; i++) {
      if (*argv[i]=='-') {
         char *p=argv[i]+2 ;
         char c=argv[i][1] ;
         switch (c) {
case 'f' :
            if (*p == 0 && argv[i+1])
               p = argv[++i] ;
	    if (*p == '(' && *(p+1) == '@' ) {
	       p+=2; firsttransf = myatol(&p); p++;
	       }
	    else if (*p == '@') {
	       p++; firsttransf = 1 ;
	       }
            if(sscanf(p, "%ld", &firstpage)==0)
	       error("! Bad first page option (-f).") ;
            break ;
case 'i' :
            if (*p == 0 && argv[i+1])
               p = argv[++i] ;
            while (*p != 0) {
	       if (*p == '(' && *(p+1) == '@' ) {
		  p+=2; inctransf[includeseq][0] = myatol(&p); p++;
		  }
	       else if (*p == '@') {
		  p++; inctransf[includeseq][0] = 1 ;
		  }
	       include[includeseq][0]=myatol(&p);
               if (*p == '.' && *(p+1) == '.' ) {
                  p += 2;
		  if (*p == '(' && *(p+1) == '@' ) {
		     p+=2; inctransf[includeseq][1] = myatol(&p); p++;
		     }
		  else if (*p == '@') {
		     p++; inctransf[includeseq][1] = 1 ;
		     }
		  include[includeseq][1] = myatol(&p);
               }
	       else {
		  include[includeseq][1] = include[includeseq][0];
		  inctransf[includeseq][0] = inctransf[includeseq][1] ;
		  }
	       includeseq++ ;
               if (*p != ',' && *p !=0 )
		  error("! Bad page range option (-i).") ;
               if (*p == ',' )
                    p++ ;
            } /* while */
            break ;
case 'l':
            if (*p == 0 && argv[i+1])
               p = argv[++i] ;
	    if (*p == '(' && *(p+1) == '@' ) {
	       p+=2; lasttransf = myatol(&p); p++;
	       }
	    else if (*p == '@') {
	       p++; lasttransf = 1 ;
	       }
            if(sscanf(p, "%ld", &lastpage)==0)
               error("! Bad last page option (-l).") ;
            break ;
case 'p' :
            if (*p == 0 && argv[i+1])
               p = argv[++i] ;
            if (sscanf(p, "%ld", &pagemodulo)==0)
               error("! Bad pagemodulo option (-p).") ;
            break ;
case 'm' :
            if (*p == 0 && argv[i+1])
               p = argv[++i] ;
/*
 *   Is there a modulo supplied?  Grab it if so; otherwise default to 1.
 */
           for (q=p; *q != 0; q++)
              if (*q == ':')
                 break ;
           if (*q == ':') {
              modulo = myatol(&p) ;
              if (*p != ':')
                 usage() ;
              if (modulo < 1 || modulo > MAXPPERP)
                 error("! modulo must lie between 1 and 32") ;
              p++ ;
           }
/*
 *   This loop grabs all of the page specifications.
 */
           pagesperpage = 0 ;
           while (*p != 0) {
              if (pagesperpage >= MAXPPERP)
                 error("! too many page specifications") ;
              if (*p == '-') {
                 reversed = 1 ;
                 p++ ;
              } else
                 reversed = 0 ;
              if (*p == 0 || *p == '(' || *p == ',')
                 pageno = 0 ;
              else
                 pageno = myatol(&p) ;
              if (*p == '(') {
                 p++ ;
                 hoffset = myatodim(&p) ;
                 if (*p++ != ',')
                    usage() ;
                 voffset = myatodim(&p) ;
                 if (*p++ != ')')
                    usage() ;
              } else {
                 hoffset = 0 ;
                 voffset = 0 ;
              }
              pages[pagesperpage].hoffset = hoffset ;
              pages[pagesperpage].voffset = voffset ;
              pages[pagesperpage].pageno = pageno ;
              pages[pagesperpage].reversed = reversed ;
              pagesperpage++ ;
              if (*p == ',')
                 p++ ;
           } /* while */
           break ;
case 'n' :
            if (*p == 0 && argv[i+1])
               p = argv[++i] ;
            if (sscanf(p, "%ld", &maxpages)==0)
               error("! Bad number of pages option (-n).") ;
            break ;
case 'q' : case 'Q' :
            quiet = 0 ;
            break ;
case 'r' : case 'R' :
            pages[0].reversed = 1;
            break ;
case 'j' : case 'J' :
            jumpingpages = 1;
            break;
case 'x' :
            if (*p == 0 && argv[i+1])
               p = argv[++i] ;
            while (*p != 0) {
	       if (*p == '(' && *(p+1) == '@' ) {
		  p+=2; exctransf[excludeseq][0] = myatol(&p); p++;
		  }
	       else if (*p == '@') {
		  p++; exctransf[excludeseq][0] = 1 ;
		  }
	       exclude[excludeseq][0]=myatol(&p);
               if (*p == '.' && *(p+1) == '.' ) {
                  p += 2;
		  if (*p == '(' && *(p+1) == '@' ) {
		     p+=2; exctransf[excludeseq][1] = myatol(&p); p++;
		     }
		  else if (*p == '@') {
		     p++; exctransf[excludeseq][1] = 1 ;
		     }
		  exclude[excludeseq][1] = myatol(&p);
               }
	       else {
		  exclude[excludeseq][1] = exclude[excludeseq][0];
		  exctransf[excludeseq][0] = exctransf[excludeseq][1] ;
		  }
	       excludeseq++ ;
               if (*p != ',' && *p !=0 )
		  error("! Bad page range option (-x).") ;
               if (*p == ',' )
                    p++ ;
            } /* while */
            break ;
case '?' :
            usage() ;
            break ;
default:
	    fprintf(stderr, "dvidvi: Bad option %c\n", c);
            error("! Try -? for more information") ;
         }  /* switch c */
      } else {  /* this a file name */
         if (*iname == 0) { /* input file name */
            register char *p ;

            lastext = 0 ;
            iname = nextstring ;
            p = argv[i] ;
            while (*p) {
               *nextstring = *p++ ;
               if (*nextstring == '.')
                  lastext = nextstring - iname ;
               else if (IS_DIR_SEP(*nextstring)	|| *nextstring == ':')
                  lastext = 0 ;
               nextstring++ ;
            }
            if (lastext == 0) {
               lastext = nextstring - iname ;
               *nextstring++ = '.' ;
               *nextstring++ = 'd' ;
               *nextstring++ = 'v' ;
               *nextstring++ = 'i' ;
            }
            *nextstring++ = 0 ;
            if ((infile=fopen(iname, READBIN))==NULL)
                 error("! can't open input file") ;

         } else { /* output file name */
            register char *p ;

            lastext = 0 ;
            oname = nextstring ;
            p = argv[i] ;
            while (*p) {
               *nextstring = *p++ ;
               if (*nextstring == '.')
                  lastext = nextstring - oname ;
               else if (IS_DIR_SEP(*nextstring)	|| *nextstring == ':')
                  lastext = 0 ;
               nextstring++ ;
            }
            if (lastext == 0) {
               lastext = nextstring - oname ;
               *nextstring++ = '.' ;
               *nextstring++ = 'd' ;
               *nextstring++ = 'v' ;
               *nextstring++ = 'i' ;
            }
            *nextstring++ = 0 ;
           if (freopen(oname, WRITEBIN, stdout)==NULL)
                error("! can't open output file") ;
         }  /* end output file name */
      } /* else argument with '-' */
   }  /* for */
   if (*iname == 0) {
       (void)fprintf(stderr, banner) ;
       error("! no input file specified");
   }
   /* Inserted by djc@dsmail.hmi.de 3.8.1994 */
   if(!oname) {
     oname="stdout";
     if (!isatty(fileno(stdout)))
       SET_BINARY(fileno(stdout));
   }

   if (*oname != 0 && !quiet) {
       (void)fprintf(stderr, banner) ;
       (void)fprintf(stderr, "%s -> %s\n",iname,oname);
       temp = nextstring ;
   }
}
/*
 *   Grabs a pointer, and checks it for validity.
 */
static integer ptr(integer where)
{
   fseek(infile,where,SEEK_SET);
   where = signedquad() ;
   if (where < -1L || where > inlength)
      error("! dvi file malformed; impossible pointer") ;
   return(where) ;
}

/*
 *   This routine store the page locations by tracing the pointers backwards.
 */
static void searchpageloc(void)
{
   integer p,num ;

   num=pagecount-1;
   for (p = ptr(postloc+1); num > 0; num--) {
      if (dvibuf(p) != 139)
         error("! missed a bop somehow") ;
      else {
	 pageloc[num]=p;
	 pagenumbers[num]=signedquad();
	 }
      p = ptr(p+41) ;
   }
   pageloc[num]=p;
   (void)dvibuf(p);
   pagenumbers[num]=signedquad();
}
/*
 *   This routine simply reads the entire dvi file, and then initializes
 *   some values about it.
 */
static void readdvifile(void) {
   integer p ;
   unsigned char c,d,e ;

   fseek(infile, 0L, 2) ;
   inlength = ftell(infile) ;
   if (inlength < 10)
      error("! dvi file too short") ;
   fseek(infile,-3L,SEEK_CUR);
   for (p=inlength - 3; p > 0; p--) {
      c = dvibyte (); d = dvibyte (); e = dvibyte ();
      if (c == 2 && d == 0xdf /* dave fuchs */ &&
                            e == 0xdf)
         break ;
#ifdef	ASCIIPTEX
      if (c == 3 && d == 0xdf /* dave fuchs */ &&
                            e == 0xdf)
         break ;
#endif
      fseek(infile,-4L,SEEK_CUR);
      }
   if (p < 10)
      error("! rather short dvi file, ain't it?") ;
   postloc = ptr(p-4) ;
   fseek(infile,postloc+5L,SEEK_SET);
   if (signedquad() != 25400000 ||
        signedquad() != 473628672)
      error("! change this program to support non-TeX num/den values") ;
   mag = signedquad() ;
   if (mag < 1 || mag > 1000000)
      error("! impossible magnification value") ;
   fseek(infile,postloc+27L,SEEK_SET);
   pagecount = twobytes() ;
   if (pagecount < 1 || pagecount > 1000000)
      error("! impossible page count value") ;
/*
 *   That's enough error checking; we probably have a correct dvi file.
 *   Let's convert all the values we got from the command line into
 *   units that we can actually use in the dvi file.
 */
   for (p=0; p<pagesperpage; p++) {
      scalemag(&(pages[p].hoffset)) ;
      scalemag(&(pages[p].voffset)) ;
   }
/*
 *   Now let's grab us some font pointers.
 */
   pageloc = (integer *) calloc (pagecount, sizeof (integer));
   if (pageloc == NULL)
      error("!  Not enough memory for allocation of page pointers");
   pagenumbers = (integer *) calloc (pagecount, sizeof (integer));
   if (pagenumbers == NULL)
      error("! Not enough memory for allocation of page numbers array");
   searchpageloc();
   transformpages();
   free(pagenumbers);
   p = postloc + 29 ;
   while (1) {
      c=dvibuf(p);
      if (c == 249)
         break ;
      if (c == 138)
         p++ ;
      else if (c == 243) {
         TeXfonts[dvibyte()] = p ;
         p += fontdeflen(p) ;
      } else
         error("! only nop's and font def's allowed in postamble") ;
   }
/*
 *   Now we check for a landscape special.  It should be the
 *   *first* thing in the page that is at all complicated.
 */

   p = pageloc[0L] + 45 ;
   c=dvibuf(p);
   while (comlen[c]) {
      p += comlen[c] ;
      c = dvibuf(p);
   }
   if (c == 239) {
      stringdvibuf(p+2L,9L);
      if (strncmp(temp, "landscape", 9)==0) {
        landscape = p ;
        rem0special = 1 ;
      }
   }
}
/*
 *   Output a single byte, keeping track of where we are.
 */
static void outdvibyte(unsigned char c)
{
   fputc(c, stdout) ;
   dviloc++ ;
}
/*
 *   Send out two bytes.
 */
static void outdvi2(integer v)
{
   outdvibyte((unsigned char)(v >> 8)) ;
   outdvibyte((unsigned char)(v & 255)) ;
}
/*
 *   Send out a longword.
 */
static void outdviquad(integer v)
{
   outdvi2(v >> 16) ;
   outdvi2(v & 65535) ;
}
/*
 *   This routine just copies some stuff from the buffer on out.
 *   Suppose the file is positioned correctly before
 */
static void putbuf(integer length)
{
   while ( length-- > 0 )
      outdvibyte((unsigned char)dvibyte()) ;
}
/*
 *   This routine outputs a string, terminated by null.
 */
static void putstr(unsigned const char *s)
{
   while (*s)
      outdvibyte(*s++) ;
}
/*
 *   Here we write the preamble to the dvi file.
 */
static void writepreamble(void) {
/*   just copy the first 14 bytes of the file */
   fseek(infile,0L,SEEK_SET);
   putbuf(14L) ;
/*   and put our identifier. */
   putstr((unsigned const char *)"\015dvidvi output") ;
}
/*
 *   This routine writes out a font definition.
 */
static void putfontdef(int f)
{
   integer p,q ;

   p = TeXfonts[f] ;
   q=fontdeflen(p) ;
   fseek(infile,p,SEEK_SET);
   putbuf(q) ;
}
/*
 *   The postamble is next.
 */
static void writepostamble(void) {
   int i ;
   integer p ;

   p = dviloc ;
   outdvibyte(248) ;
   outdviquad(prevpp) ;
   fseek(infile,postloc+5,SEEK_SET);
   putbuf(20L) ;
   outdvi2(twobytes()+1L) ; /* increase stack depth by 1 */
   outdvi2(outputpages) ;
   for (i=0; i<256; i++)
      if (fontseen[i])
         putfontdef(i) ;
   outdvibyte(249) ;
   outdviquad(p) ;
#ifdef ASCIIPTEX
   if (ptexdvi)
      outdvibyte(3) ;
   else
#endif
   outdvibyte(2) ;
   outdviquad(0xdfdfdfdfL) ;
   while (dviloc & 3)
      outdvibyte(0xdf) ;
   fclose(stdout) ;
}
/*
 *   This routine starts a page, by writing out a bop command.
 */
static void beginpage(void) {
   int i ;
   integer p ;

   p = dviloc ;
   outdvibyte(139) ;
   outdviquad(outputpages+1) ;
   for (i=0; i<9; i++)
      outdviquad(0L) ;
   outdviquad(prevpp) ;
   prevpp = p ;
}
/*
 *   This routine sends out a page.  We need to handle the
 *   landscape special, though.
 */
static void dopage(integer num)
{
   register integer p ;
   register int len ;
   integer v, oldp ;
   unsigned char c;

/*
 *   We want to take the base 10 log of the number.  It's probably
 *   small, so we do it quick.
 */
   if (! quiet) {
      int t = num+1, i = 0 ;
      if (t < 0) {
         t = -t ;
         i++ ;
      }
      do {
         i++ ;
         t /= 10 ;
      } while (t > 0) ;
      if (i + prettycolumn > 76) {
         fprintf(stderr, "\n") ;
         prettycolumn = 0 ;
      }
      prettycolumn += i + 1 ;
      (void)fprintf(stderr, "[%ld", num+1) ;
      (void)fflush(stderr) ;
   }
   p = pageloc[num] + 45 ;
   c=dvibuf(p);
   while (c != 140) {
      if ((len=comlen[c]) > 0) {    /* most commands are simple */
         outdvibyte(c);
         putbuf((long)len-1) ;
         p += len ;
      } else {   /* but there are a few we need to treat specially */
         len = c ;
         if (171 <= len && len <= 235) {
            p++ ;
            if (len == 235)
               { len = dvibyte (); p++ ;}
            else
               len -= 171 ;
            if (!fontseen[len]) {
               putfontdef(len) ;
               fontseen[len] = 1 ;
               fseek(infile,p,SEEK_SET);
            }
            if (len < 64)
               outdvibyte((unsigned char)(171 + len)) ;
            else {
               outdvibyte(235) ;
               outdvibyte((unsigned char)len) ;
            }
         } else {
            v = 0 ;
            oldp = p++ ;
            switch(len) {
case 242:      v = dvibyte() ; p++ ;  break ;
case 241:      v = (v << 8) + dvibyte() ; p++ ; break ;
case 240:      v = (v << 8) + dvibyte() ; p++ ; break ;
case 239:      v = (v << 8) + dvibyte() ; p++ ;
/*
 *   Remove a landscape special on page 0, if one is found.
 */            stringdvibuf(oldp + len - 327,9);
               if (num || ! rem0special ||
                    strncmp(temp, "landscape", 9)) {
                  fseek (infile,oldp,SEEK_SET);
                  putbuf(v + len - 237) ;
                  }
               p = oldp + v + len - 237 ;
               fseek(infile,p,SEEK_SET);
               break ;
case 243: case 244: case 245: case 246:
               p += len - 230 ;
               p += dvibuf(p) ;
               p += dvibyte() + 2 ;
               fseek(infile,p,SEEK_SET);
               break ;
#ifdef ASCIIPTEX
case 255:
               ptexdvi = 1 ;
               outdvibyte(len);
               break ;
#endif
default:       fprintf(stderr, "Bad dvi command was %d at %ld\n", len, p) ;
               error("! lost sync dvi in file lost dvi sync file in") ;
            }
         }
      }
      c=dvibyte();
   }
   if (! quiet) {
      (void)fprintf(stderr, "] ") ;
      (void)fflush(stderr) ;
      prettycolumn += 2 ;
   }
}
/*
 *   Here we end a page.  Simple enough.
 */
static void endpage(void) {
   outputpages++ ;
   outdvibyte(140) ;
}
/*
 *   This is our main routine for output, which runs through all the
 *   pages we need to output.
 */
static void writedvifile(void) {
   integer pagenum ;
   int ppp ;
   integer actualpageno, lastpageno ;
   struct pagespec *ps ;
   Boolean beginp ;

#ifdef ASCIIPTEX
   ptexdvi = 0 ;
#endif

   writepreamble() ;
   if ( !pagemodulo )
      pagefake = (pagecount + modulo - 1) / modulo * modulo ;
   else
      pagefake = (pagecount + pagemodulo - 1) / pagemodulo * pagemodulo ;
   if ( maxpages < pagefake ) pagefake = maxpages;
   for (lastpageno = -1, pagenum = 0; pagenum < pagefake / ( jumpingpages + 1); pagenum += modulo) {
      beginp = 1 ;
      for (ppp = 0, ps=pages; ppp < pagesperpage; ppp++, ps++) {
         if (ps->reversed)
            actualpageno = pagefake - pagenum - modulo + ps->pageno ;
         else
            actualpageno = pagenum + ps->pageno ;
         if ( jumpingpages )
            ps->reversed = !ps->reversed;
         if ( actualpageno == lastpageno ) {
            pagenum = pagefake;
            break;
         }
   if (actualpageno < pagecount && selectedpage(actualpageno+1) ) {
            if (beginp) {
                beginpage() ;
                beginp = 0 ;
            }
            if (landscape) {
                fseek(infile,landscape,SEEK_SET);
                putbuf(dvibuf(landscape+1)+2L) ;
                landscape = 0 ;
            }
            if (pagesperpage)
                outdvibyte(141) ;
            if (ps->hoffset) {
                outdvibyte(146) ;
                outdviquad(ps->hoffset) ;
            }
            if ( pagemodulo )
                lastpageno = actualpageno;
            if (ps->voffset) {
                outdvibyte(160) ;
                outdviquad(ps->voffset) ;
            }
            dopage(actualpageno) ;
            if (pagesperpage)
                outdvibyte(142) ;
         }
      }
      if (beginp != 1) endpage() ;
   }
   writepostamble() ;
   if (! quiet) {
      (void)fprintf(stderr, "\n") ;
      (void)fflush(stderr) ;
      prettycolumn = 0 ;
   }
}
int main(int argc, char *argv[])
{
#if defined(WIN32) && defined(KPATHSEA)
   int ac;
   char **av, *enc;
   if (argc>1) {
      kpse_set_program_name(argv[0], "dvidvi");
      enc = kpse_var_value("command_line_encoding");
      if (get_command_line_args_utf8(enc, &ac, &av)) {
         argc = ac;
         argv = av;
      }
   }
#endif
   processargs(argc, argv) ;
   readdvifile() ;
   writedvifile() ;
   return 0 ;
}

