/* Copyright (c) 1987, 1989, 2012, 2020 University of Maryland Department of
   Computer Science.
   
   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation, the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions: The above copyright notice, this permission
   notice and the disclaimer statement shall be included in all copies
   or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
 * DVI page rearrangement program
 *
 * Reads DVI version 2 files and rearranges pages,
 * writing a new DVI file.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef KPATHSEA
#include <kpathsea/config.h>
#include <kpathsea/c-fopen.h>
#include <kpathsea/getopt.h>
#if defined(WIN32)
#include <kpathsea/variable.h>
#endif
#else
#define FOPEN_RBIN_MODE  "rb"
#define FOPEN_WBIN_MODE  "wb"
#define SET_BINARY(x) (void)0
extern char *optarg;
extern int   optind;
#endif

#include "types.h"
#include "dviclass.h"
#include "dvicodes.h"
#include "error.h"
#include "fio.h"
#include "gripes.h"
#include "search.h"
#include <stdio.h>
#include <ctype.h>
#include "seek.h"
#include "common.h"

#define white(x) ((x) == ' ' || (x) == '\t' || (x) == ',')

char  *ProgName;

/* Globals */
char	serrbuf[BUFSIZ];	/* buffer for stderr */

/*
 * We will try to keep output lines shorter than MAXCOL characters.
 */
#define MAXCOL	75

/*
 * We use the following structure to keep track of fonts we have seen.
 * The final DVI file lists only the fonts it uses.
 */
struct fontinfo {
	i32	fi_newindex;	/* font number in output file */
	int	fi_reallyused;	/* true => used on a page we copied */
	i32	fi_checksum;	/* the checksum */
	i32	fi_mag;		/* the magnification */
	i32	fi_designsize;	/* the design size */
	short	fi_n1;		/* the name header length */
	short	fi_n2;		/* the name body length */
	char	*fi_name;	/* the name itself */
};


i32     Width;                  /* width of page */
i32     Height;                 /* height of page */
i32     Magnification;          /* Magnification of pages */
int     Modulo;                 /* page spec modulo */
struct pagespec *PageSpecs;     /* page specification list */

int	SFlag;			/* true => -s, silent operation */

struct	search *FontFinder;	/* maps from input indicies to fontinfo */
i32	NextOutputFontIndex;	/* generates output indicies */
i32	CurrentFontIndex;	/* current (old) index in input */
i32	OutputFontIndex;	/* current (new) index in ouput */

const char	*DVIFileName;		/* name of input DVI file */
FILE	*inf;			/* the input file itself */
FILE	*outf;			/* the output DVI file */

long	*StartOfPage;		/* The file positions of the input pages */
long	StartOfPageSpace;	/* Number of entries of StartOfPage array */

long	StartOfLastPage;	/* The file position just before we
				   started the last page */
long	CurrentPosition;	/* The current position of the file */

int	UseThisPage;		/* true => current page is selected */

int	WritingPage;		/* true while writing a page */

i32	InputPageNumber;	/* current absolute page in old DVI file */
int	NumberOfOutputPages;	/* number of pages in new DVI file */
#ifdef	ASCIIPTEX
int	ptexdvi;		/* true if dvi file is extended (TATEKUMI) */
#endif

i32	Numerator;		/* numerator from DVI file */
i32	Denominator;		/* denominator from DVI file */
i32	DVIMag;			/* magnification from DVI file */

i32	Count[10];		/* the 10 \count variables */

/* save some string space: we use this a lot */
char	writeerr[] = "error writing DVI file";
#ifndef KPATHSEA
void	*malloc(), *realloc();
void	free();
#endif

static void WriteFont(struct fontinfo *fi);
static void PutFontSelector(i32 index);
static void ScanDVIFile(void);
static void HandleDVIFile(void);

/*
 * You may get lint warnings about sprintf's return value.
 * Older versions of 4BSD have `char *sprintf()'.  ANSI and
 * SysV use `int sprintf()'; so ignore the warnings.
 */

/*
 * Lint gets somewhat confused over putc.
 */
#ifdef lint
#undef putc
#ifdef ultrix /* grr */
#define putc(c, f) fputc((char)(c), f)
#else
#define putc(c, f) fputc((int)(c), f)
#endif
#endif

static void
specusage(void)
{
   error(1, -1, "page specification error:\n\
  <pagespecs> = [modulo:][mag@]<spec>\n\
  <spec>      = [-]pageno[(xoff,yoff)][,spec|+spec]\n\
                modulo>=1, 0<=pageno<modulo");
}

/*
 *   This function calculates approximately (whole + num/den) * sf.
 *   No need for real extreme accuracy; one ten thousandth of an
 *   inch should be sufficient.
 *
 *   Assumptions:
 *
 *      0 <= num < den <= 10000
 *      0 <= whole
 */
i32 defaultscale = 4736286 ;

static i32
scale(i32 whole, int num, int den, i32 sf)
{
   i32 v ;

   if (!sf)
      sf = defaultscale ;
   v = whole * sf + num * (sf / den) ;
   if (v / sf != whole || v < 0 || v > 0x40000000L)
      error(1, -1, "arithmetic overflow in dimension") ;
   sf = sf % den ;
   v += (sf * num * 2 + den) / (2 * den) ;
   return (v) ;
}

struct pagespec {
   int reversed, pageno, add;
   i32 xoff, yoff;
   struct pagespec *next;
};

static struct pagespec *
newspec(void)
{
   struct pagespec *temp = (struct pagespec *)malloc(sizeof(struct pagespec));
   temp->reversed = temp->pageno = temp->add = 0;
   temp->xoff = temp->yoff = 0;
   temp->next = NULL;
   return (temp);
}

static int
parseint(char **sp)
{
   char *s = *sp;
   int n = 0, neg = 0;

   if (*s == '-') {
      neg = 1;
      s++;
   }
   for (; isdigit((unsigned char)*s); s++)
      n = n*10 + (*s-'0');
   if (*sp == s) specusage();
   *sp = s;
   return (neg ? -n : n);
}

static i32
parsedimen(char **sp)
{
   i32 whole = 0;
   int num = 0, den = 1, neg = 0;
   i32 fac = 0L;
   char *s = *sp;

   if (*s == '-') {
      neg = 1;
      *sp = ++s;
   }
   for (; isdigit((unsigned char)*s); s++)
      whole = whole*10 + (*s-'0');

   if (*s == '.') {
      *sp = ++s;
      for (; isdigit((unsigned char)*s); s++) {
	 if (den < 10000) { /* limit precision for scale to work */
	    num = num*10 + (*s-'0');
	    den *= 10;
	 }
      }
   }
   if (*sp == s) specusage();
   *sp = s;

   /*
    *   Allowed units are `in', `cm', `mm', `pt', `sp', `cc', `dd', and `pc';
    *   must be in lower case.
    */
   if (*s == 'c' && s[1] == 'm') {
      /* centimeters need to be scaled by 72.27 * 216 / 2.54, or 1 864 680 */
      fac = 1864680L ;
      s += 2;
   } else if (*s == 'p' && s[1] == 't') {
      /*  real points need to be scaled by 65536 */
      fac = 65536L ;
      s += 2;
   } else if (*s == 'p' && s[1] == 'c') {
      /*  picas need to be scaled by 65536 * 12, or 786 432 */
      fac = 786432L ;
      s += 2;
   } else if (*s == 'm' && s[1] == 'm') {
      /*  millimeters need to be scaled by 72.27 * 216 / 25.4, or 186 468 */
      fac = 186468L ;
      s += 2;
   } else if (*s == 's' && s[1] == 'p') {
      /*  scaled points are already taken care of; simply round */
      fac = 1L ;
      s += 2;
   } else if (*s == 'b' && s[1] == 'p') {
      /*  big points need to be scaled by 72.27 * 65536 / 72, or 65782 */
      fac = 65782L ;
      s += 2;
   } else if (*s == 'd' && s[1] == 'd') {
      /*  didot points need to be scaled by 65536 * 1238 / 1157, or 70124 */
      fac = 70124L ;
      s += 2;
   } else if (*s == 'c' && s[1] == 'c') {
      /*  cicero need to be scaled by 65536 * 1238 / 1157 * 12, or 841 489 */
      fac = 841489L ;
      s += 2;
   } else if (*s == 'i' && s[1] == 'n') {
      /*  inches need to be scaled by 72.27 * 65536, or 4 736 286 */
      fac = 4736286L ;
      s += 2;
   } else if (*s == 'w') {
      fac = Width;
      s++;
   } else if (*s == 'h') {
      fac = Height;
      s++;
   }
   whole = scale(whole, num, den, fac) ;
   *sp = s;
   return (neg ? -whole : whole);
}

static struct pagespec *
ParseSpecs(char *str, int make)
{
   struct pagespec *result, *head, *tail;
   int other = 0;
   int num = -1;
   struct pagespec spare;

   if (make)
      result = head = tail = newspec();
   else {
      result = NULL;
      head = tail = &spare;
   }
   while (*str) {
      if (isdigit((unsigned char)*str)) {
	 num = parseint(&str);
      } else {
	 switch (*str++) {
	 case ':':
	    if (other || head != tail || num < 1) specusage();
	    Modulo = num;
	    num = -1;
	    break;
	 case '@':
	    if (other || head != tail || num < 1) specusage();
	    Magnification = num;
	    num = -1;
	    break;
	 case '-':
	    tail->reversed = !tail->reversed;
	    other = 1;
	    break;
	 case '(':
	    tail->xoff += parsedimen(&str);
	    if (*str++ != ',') specusage();
	    tail->yoff += parsedimen(&str);
	    if (*str++ != ')') specusage();
	    other = 1;
	    break;
	 case '+':
	    tail->add = 1;
	 case ',':
	    if (num < 0 || num >= Modulo) specusage();
	    tail->pageno = num;
	    if (make) {
	       tail->next = newspec();
	       tail = tail->next;
	    }
	    num = -1;
	    other = 1;
	    break;
	 default:
	    specusage();
	 }
      }
   }
   if (num >= Modulo)
      specusage();
   else if (num >= 0)
      tail->pageno = num;
   return result;
}

static i32
singledimen(char *str)
{
   i32 num = parsedimen(&str);
   if (*str) return (0);
   return (num);
}

/*
 * Print a message to stderr, with an optional leading space, and handling
 * long line wraps.
 */
static void
message(int space, const char *str, int len)
{
	static int beenhere;
	static int col;

	if (!beenhere)
		space = 0, beenhere++;
	if (len == 0)
		len = strlen(str);
	col += len;
	if (space) {
		if (col >= MAXCOL)
			(void) putc('\n', stderr), col = len;
		else
			(void) putc(' ', stderr), col++;
	}
	while (--len >= 0)
		(void) putc(*str++, stderr);
	(void) fflush(stderr);
}

/*
 * Start a page (process a DVI_BOP).
 */
static void
BeginPage(int really)
{
	register i32 *i;

	OutputFontIndex = -1;	/* new page requires respecifying font */
	for (i = Count; i < &Count[10]; i++)
		fGetLong(inf, *i);
	(void) GetLong(inf);	/* previous page pointer */

	if (!UseThisPage || !really)
		return;

	putbyte(outf, DVI_BOP);
	for (i = Count; i < &Count[10]; i++)
		PutLong(outf, *i);
	PutLong(outf, StartOfLastPage);
	if (ferror(outf))
		error(1, -1, writeerr);

	StartOfLastPage = CurrentPosition;
	CurrentPosition += 45;	/* we just wrote this much */

	if (!SFlag) {		/* write nice page usage messages */
		register int z = 0;
		register int mlen = 0;
		char msg[80];

		(void) sprintf(msg, "[%ld", (long)Count[0]);
		mlen = strlen(msg);
		for (i = &Count[1]; i < &Count[10]; i++) {
			if (*i == 0) {
				z++;
				continue;
			}
			while (--z >= 0)
				msg[mlen++] = '.', msg[mlen++] = '0';
			z = 0;
			(void) sprintf(msg + mlen, ".%ld", (long)*i);
			mlen += strlen(msg + mlen);
		}
		message(1, msg, mlen);
	}
	WritingPage = 1;
}

/*
 * End a page (process a DVI_EOP).
 */
static void
EndPage(int really)
{

	if (!UseThisPage || !really)
		return;
	if (!SFlag)
		message(0, "]", 1);
	putbyte(outf, DVI_EOP);
	if (ferror(outf))
		error(1, -1, writeerr);
	CurrentPosition++;
	NumberOfOutputPages++;
	WritingPage = 0;
}

/*
 * For each of the fonts used in the new DVI file, write out a definition.
 */
/* ARGSUSED */
static void
PostAmbleFontEnumerator(char *addr, i32 key)
{

	if (((struct fontinfo *)addr)->fi_reallyused)
		WriteFont((struct fontinfo *)addr);
}

static void
HandlePostAmble(void)
{
	register i32 c;

	(void) GetLong(inf);	/* previous page pointer */
	if (GetLong(inf) != Numerator)
		GripeMismatchedValue("numerator");
	if (GetLong(inf) != Denominator)
		GripeMismatchedValue("denominator");
	if (GetLong(inf) * Magnification / 1000 != DVIMag)
		GripeMismatchedValue("\\magnification");

	putbyte(outf, DVI_POST);
	PutLong(outf, StartOfLastPage);
	PutLong(outf, Numerator);
	PutLong(outf, Denominator);
	PutLong(outf, DVIMag);
	c = GetLong(inf);
	PutLong(outf, c);	/* tallest page height */
	c = GetLong(inf);
	PutLong(outf, c);	/* widest page width */
	c = GetWord(inf)+1;
	PutWord(outf, c);	/* DVI stack size */
	PutWord(outf, NumberOfOutputPages);
	StartOfLastPage = CurrentPosition;	/* point at post */
	CurrentPosition += 29;	/* count all those `put's */
#ifdef notdef
	(void) GetWord(inf);	/* skip original number of pages */
#endif

	/*
	 * just ignore all the incoming font definitions; we are done with
	 * input file 
	 */

	/*
	 * run through the FontFinder table and dump definitions for the
	 * fonts we have used. 
	 */
	SEnumerate(FontFinder, PostAmbleFontEnumerator);

	putbyte(outf, DVI_POSTPOST);
	PutLong(outf, StartOfLastPage);	/* actually start of postamble */
#ifdef	ASCIIPTEX
	if(ptexdvi)
		putbyte(outf, DVI_PTEXVERSION);
	else
#endif
	putbyte(outf, DVI_VERSION);
	putbyte(outf, DVI_FILLER);
	putbyte(outf, DVI_FILLER);
	putbyte(outf, DVI_FILLER);
	putbyte(outf, DVI_FILLER);
	CurrentPosition += 10;
	while (CurrentPosition & 3) {
		putbyte(outf, DVI_FILLER);
		CurrentPosition++;
	}
	if (ferror(outf))
		error(1, -1, writeerr);
}

/*
 * Write a font definition to the output file
 */
static void
WriteFont(struct fontinfo *fi)
{
	register int l;
	register char *s;

	if (fi->fi_newindex < 256) {
		putbyte(outf, DVI_FNTDEF1);
		putbyte(outf, fi->fi_newindex);
		CurrentPosition += 2;
	} else if (fi->fi_newindex < 65536) {
		putbyte(outf, DVI_FNTDEF2);
		PutWord(outf, fi->fi_newindex);
		CurrentPosition += 3;
	} else if (fi->fi_newindex < 16777216) {
		putbyte(outf, DVI_FNTDEF3);
		Put3Byte(outf, fi->fi_newindex);
		CurrentPosition += 4;
	} else {
		putbyte(outf, DVI_FNTDEF4);
		PutLong(outf, fi->fi_newindex);
		CurrentPosition += 5;
	}
	PutLong(outf, fi->fi_checksum);
	PutLong(outf, fi->fi_mag);
	PutLong(outf, fi->fi_designsize);
	putbyte(outf, fi->fi_n1);
	putbyte(outf, fi->fi_n2);
	l = fi->fi_n1 + fi->fi_n2;
	CurrentPosition += 14 + l;
	s = fi->fi_name;
	while (--l >= 0)
		putbyte(outf, *s++);
}

/*
 * Handle the preamble.  Someday we should update the comment field.
 */
static void
HandlePreAmble(void)
{
	register int n, c;

	c = getc(inf);
	if (c == EOF)
		GripeUnexpectedDVIEOF();
	if (c != DVI_PRE)
		GripeMissingOp("PRE");
	if (getc(inf) != DVI_VERSION)
		error(1, 0, "%s is not a DVI version %d file",
		    DVIFileName, DVI_VERSION);
	Numerator = GetLong(inf);
	Denominator = GetLong(inf);
	DVIMag = GetLong(inf) * Magnification / 1000;
	putbyte(outf, DVI_PRE);
	putbyte(outf, DVI_VERSION);
	PutLong(outf, Numerator);
	PutLong(outf, Denominator);
	PutLong(outf, DVIMag);

	n = UnSign8(GetByte(inf));
	CurrentPosition = 15 + n;	/* well, almost */
	putbyte(outf, n);
	while (--n >= 0) {
		c = GetByte(inf);
		putbyte(outf, c);
	}
}

int
main(int argc, char **argv)
{
	register int c;
	register char *s;
	char *outname = NULL;
	char *specstring = NULL;
#if defined(WIN32) && defined(KPATHSEA)
	int ac;
	char **av, *enc;

	kpse_set_program_name(argv[0], "dvitodvi");
	enc = kpse_var_value("command_line_encoding");
	if (get_command_line_args_utf8(enc, &ac, &av)) {
		argc = ac;
		argv = av;
	}
#endif

	Width = 0;
	Height = 0;
	Magnification = 1000;
	Modulo = 1;

	ProgName = *argv;
	setbuf(stderr, serrbuf);

	while ((c = getopt(argc, argv, "i:o:w:h:q")) != EOF) {
		switch (c) {

		case 'q':	/* silent */
			SFlag++;
			break;

		case 'i':
			if (DVIFileName != NULL)
				goto usage;
			DVIFileName = optarg;
			break;

		case 'o':
			if (outname != NULL)
				goto usage;
			outname = optarg;
			break;

		case 'w':
			if (Width != 0)
				goto usage;
			Width = singledimen(optarg);
			if (Width <= 0)
			   error(1, -1, "-w parameter must be > 0");
			break;

		case 'h':
			if (Height != 0)
				goto usage;
			Height = singledimen(optarg);
			if (Height <= 0)
			   error(1, -1, "-h parameter must be > 0");
			break;

		case '?':
usage:
			(void) fprintf(stderr,
				"dvitodvi  in SeeTeX Ver.%s (%s)\n", VERSION, TL_VERSION);
			(void) fprintf(stderr, "\
Usage: %s [-q] [-i infile] [-o outfile] [-w width] [-h height] <pagespecs> [infile [outfile]]\n",
				ProgName);
			(void) fprintf(stderr,
				"\nEmail bug reports to %s.\n", BUG_ADDRESS);
			(void) fflush(stderr);
			exit(1);
		}
	}

	while (optind < argc) {
		s = argv[optind++];
		c = *s;
		if (specstring == NULL)
		        (void) ParseSpecs((specstring = s), 0);
		else if (DVIFileName == NULL)
			DVIFileName = s;
		else if (outname == NULL)
			outname = s;
		else
			goto usage;
	}

	if (specstring == NULL)
	        goto usage;

	if (DVIFileName == NULL) {
		DVIFileName = "`stdin'";
		inf = stdin;
		if (!isatty(fileno(inf)))
		  SET_BINARY(fileno(inf));
		else
		  goto usage;
	} else if ((inf = fopen(DVIFileName, FOPEN_RBIN_MODE)) == 0)
		error(1, -1, "cannot read %s", DVIFileName);
	if (outname == NULL) {
		outf = stdout;
		if (!isatty(fileno(outf)))
		  SET_BINARY(fileno(outf));
	}
	else if ((outf = fopen(outname, FOPEN_WBIN_MODE)) == 0)
		error(1, -1, "cannot write %s", outname);

	if ((FontFinder = SCreate(sizeof(struct fontinfo))) == 0)
		error(1, 0, "cannot create font finder (out of memory?)");

	/* copy inf to TEMP file if not seekable */
	if ((inf = SeekFile(inf)) == NULL) {
	        error(1, 0, "can't seek file");
	}

#ifdef	ASCIIPTEX
	ptexdvi = 0;
#endif
	InputPageNumber = 0;
	StartOfPageSpace = 32;
	StartOfLastPage = -1;
	StartOfPage = malloc(sizeof(long) * StartOfPageSpace);
	if (!StartOfPage) {
		error(1, -1, "cannot allocate list of pages; out of memory");
	}

	HandlePreAmble();
	ScanDVIFile();
	if (fseek(inf, 16L, 1) == -1)
	        error(1, -1, "can't seek postamble");
	if (Height == 0)         /* get height from postamble */
	   Height = GetLong(inf);
	else
	   (void) GetLong(inf); /* ignore height */
	if (Width == 0)          /* get width from postamble */
	   Width = GetLong(inf);
	PageSpecs = ParseSpecs(specstring, 1);

	HandleDVIFile();
	free(StartOfPage);
	if (WritingPage)
	   EndPage(1);
	HandlePostAmble();
	if (!SFlag)
		(void) fprintf(stderr, "\nWrote %d page%s, %ld bytes\n",
		    NumberOfOutputPages, NumberOfOutputPages == 1 ? "" : "s",
		    (long)CurrentPosition);
	return 0;
}

/*
 * Handle a font definition.
 */
static void
HandleFontDef(i32 index)
{
	register struct fontinfo *fi;
	register int i;
	register char *s;
	int def = S_CREATE | S_EXCL;

	if (!UseThisPage) {
		if ((fi = (struct fontinfo *)SSearch(FontFinder, index, &def)) == 0) {
			if (def & S_COLL)
				error(1, 0, "font %ld already defined", (long)index);
			else
				error(1, 0, "cannot stash font %ld (out of memory?)",
					(long)index);
		}
		fi->fi_reallyused = 0;
		fi->fi_checksum = GetLong(inf);
		fi->fi_mag = GetLong(inf);
		fi->fi_designsize = GetLong(inf);
		fi->fi_n1 = UnSign8(GetByte(inf));
		fi->fi_n2 = UnSign8(GetByte(inf));
		i = fi->fi_n1 + fi->fi_n2;
		if ((s = malloc((unsigned)i)) == 0)
			GripeOutOfMemory(i, "font name");
		fi->fi_name = s;
		while (--i >= 0)
			*s++ = GetByte(inf);
	} else {
	        (void) GetLong(inf);
	        (void) GetLong(inf);
	        (void) GetLong(inf);
		i = UnSign8(GetByte(inf));
		i += UnSign8(GetByte(inf));
		while (--i >= 0)
			(void) GetByte(inf);
	}
}

/*
 * Handle a \special.
 */
static void
HandleSpecial(int c, int l, i32 p)
{
	register int i;

	if (UseThisPage) {
		putbyte(outf, c);
		switch (l) {

		case DPL_UNS1:
			putbyte(outf, p);
			CurrentPosition += 2;
			break;

		case DPL_UNS2:
			PutWord(outf, p);
			CurrentPosition += 3;
			break;

		case DPL_UNS3:
			Put3Byte(outf, p);
			CurrentPosition += 4;
			break;

		case DPL_SGN4:
			PutLong(outf, p);
			CurrentPosition += 5;
			break;

		default:
			panic("HandleSpecial l=%d", l);
			/* NOTREACHED */
		}
		CurrentPosition += p;
		while (--p >= 0) {
			i = getc(inf);
			putbyte(outf, i);
		}
		if (feof(inf))
			GripeUnexpectedDVIEOF();
		if (ferror(outf))
			error(1, -1, writeerr);
	} else
		while (--p >= 0)
			(void) getc(inf);
}

static void
ReallyUseFont(void)
{
	register struct fontinfo *fi;
	int look = S_LOOKUP;

	fi = (struct fontinfo *)SSearch(FontFinder, CurrentFontIndex, &look);
	if (fi == NULL)
		error(1, 0, "DVI file requested font %ld without defining it",
		    (long)CurrentFontIndex);
	if (fi->fi_reallyused == 0) {
		fi->fi_reallyused++;
		fi->fi_newindex = NextOutputFontIndex++;
		WriteFont(fi);
	}
	if (fi->fi_newindex != OutputFontIndex) {
		PutFontSelector(fi->fi_newindex);
		OutputFontIndex = fi->fi_newindex;
	}
}

/*
 * Write a font selection command to the output file
 */
static void
PutFontSelector(i32 index)
{

	if (index < 64) {
		putbyte(outf, index + DVI_FNTNUM0);
		CurrentPosition++;
	} else if (index < 256) {
		putbyte(outf, DVI_FNT1);
		putbyte(outf, index);
		CurrentPosition += 2;
	} else if (index < 65536) {
		putbyte(outf, DVI_FNT2);
		PutWord(outf, index);
		CurrentPosition += 3;
	} else if (index < 16777216) {
		putbyte(outf, DVI_FNT3);
		Put3Byte(outf, index);
		CurrentPosition += 4;
	} else {
		putbyte(outf, DVI_FNT4);
		PutLong(outf, index);
		CurrentPosition += 5;
	}
}

/*
 * The following table describes the length (in bytes) of each of the DVI
 * commands that we can simply copy, starting with DVI_SET1 (128).
 */
char	oplen[128] = {
	0, 0, 0, 0,		/* DVI_SET1 .. DVI_SET4 */
	9,			/* DVI_SETRULE */
	0, 0, 0, 0,		/* DVI_PUT1 .. DVI_PUT4 */
	9,			/* DVI_PUTRULE */
	1,			/* DVI_NOP */
	0,			/* DVI_BOP */
	0,			/* DVI_EOP */
	1,			/* DVI_PUSH */
	1,			/* DVI_POP */
	2, 3, 4, 5,		/* DVI_RIGHT1 .. DVI_RIGHT4 */
	1,			/* DVI_W0 */
	2, 3, 4, 5,		/* DVI_W1 .. DVI_W4 */
	1,			/* DVI_X0 */
	2, 3, 4, 5,		/* DVI_X1 .. DVI_X4 */
	2, 3, 4, 5,		/* DVI_DOWN1 .. DVI_DOWN4 */
	1,			/* DVI_Y0 */
	2, 3, 4, 5,		/* DVI_Y1 .. DVI_Y4 */
	1,			/* DVI_Z0 */
	2, 3, 4, 5,		/* DVI_Z1 .. DVI_Z4 */
	0,			/* DVI_FNTNUM0 (171) */
	0, 0, 0, 0, 0, 0, 0, 0,	/* 172 .. 179 */
	0, 0, 0, 0, 0, 0, 0, 0,	/* 180 .. 187 */
	0, 0, 0, 0, 0, 0, 0, 0,	/* 188 .. 195 */
	0, 0, 0, 0, 0, 0, 0, 0,	/* 196 .. 203 */
	0, 0, 0, 0, 0, 0, 0, 0,	/* 204 .. 211 */
	0, 0, 0, 0, 0, 0, 0, 0,	/* 212 .. 219 */
	0, 0, 0, 0, 0, 0, 0, 0,	/* 220 .. 227 */
	0, 0, 0, 0, 0, 0, 0,	/* 228 .. 234 */
	0, 0, 0, 0,		/* DVI_FNT1 .. DVI_FNT4 */
	0, 0, 0, 0,		/* DVI_XXX1 .. DVI_XXX4 */
	0, 0, 0, 0,		/* DVI_FNTDEF1 .. DVI_FNTDEF4 */
	0,			/* DVI_PRE */
	0,			/* DVI_POST */
	0,			/* DVI_POSTPOST */
#ifdef	ASCIIPTEX
	0, 0, 0, 0, 0,	 	/* 250 .. 254 */
	0,			/* DVI_DIR */
#else
	0, 0, 0, 0, 0, 0,	/* 250 .. 255 */
#endif
};

static int
HandlePage(int first, int last, i32 hoffset, i32 voffset)
{
	register int c, l;
	register i32 p = 0;	/* avoid uninitialized warning */
	register int CurrentFontOK = 0;
	int doingpage = 0;

	/* Only way out is via "return" statement */
	for (;;) {
		c = getc(inf);	/* getc() returns unsigned values */
		if (DVI_IsChar(c)) {
			/*
			 * Copy chars, note font usage, but ignore if
			 * page is not interesting.
			 */
			if (!UseThisPage)
				continue;
			if (!CurrentFontOK) {
				ReallyUseFont();
				CurrentFontOK++;
			}
			putbyte(outf, c);
			CurrentPosition++;
			continue;
		}
		if (DVI_IsFont(c)) {	/* note font change */
			CurrentFontIndex = c - DVI_FNTNUM0;
			CurrentFontOK = 0;
			continue;
		}
		if (c == EOF)
			GripeUnexpectedDVIEOF();
		if ((l = (oplen - 128)[c]) != 0) {	/* simple copy */
			if (!UseThisPage) {
				while (--l > 0)
					(void) getc(inf);
				continue;
			}
			CurrentPosition += l;
			putbyte(outf, c);
			while (--l > 0) {
				c = getc(inf);
				putbyte(outf, c);
			}
			if (ferror(outf))
				error(1, -1, writeerr);
			continue;
		}
		if ((l = DVI_OpLen(c)) != 0) {
			/*
			 * Handle other generics.
			 * N.B.: there should only be unsigned parameters
			 * here (save SGN4), for commands with negative
			 * parameters have been taken care of above.
			 */
			switch (l) {

			case DPL_UNS1:
				p = getc(inf);
				break;

			case DPL_UNS2:
				fGetWord(inf, p);
				break;

			case DPL_UNS3:
				fGet3Byte(inf, p);
				break;

			case DPL_SGN4:
				fGetLong(inf, p);
				break;

			default:
				panic("HandleDVIFile l=%d", l);
			}

			/*
			 * Now that we have the parameter, perform the
			 * command.
			 */
			switch (DVI_DT(c)) {

			case DT_SET:
			case DT_PUT:
				if (!UseThisPage)
					continue;
				if (!CurrentFontOK) {
					ReallyUseFont();
					CurrentFontOK++;
				}
				putbyte(outf, c);
				switch (l) {

				case DPL_UNS1:
					putbyte(outf, p);
					CurrentPosition += 2;
					continue;

				case DPL_UNS2:
					PutWord(outf, p);
					CurrentPosition += 3;
					continue;

				case DPL_UNS3:
					Put3Byte(outf, p);
					CurrentPosition += 4;
					continue;

				case DPL_SGN4:
					PutLong(outf, p);
					CurrentPosition += 5;
					continue;
				}

			case DT_FNT:
				CurrentFontIndex = p;
				CurrentFontOK = 0;
				continue;

			case DT_XXX:
				HandleSpecial(c, l, p);
				continue;

			case DT_FNTDEF:
				HandleFontDef(p);
				continue;

#ifdef	ASCIIPTEX
			case DT_DIR:
				if (!UseThisPage)
					continue;
				ptexdvi = 1;

				putbyte(outf, c);
				putbyte(outf, p);
				CurrentPosition += 2;
				continue;
#endif

			default:
				panic("HandleDVIFile DVI_DT(%d)=%d",
				      c, DVI_DT(c));
			}
			continue;
		}

		switch (c) {	/* handle the few remaining cases */

		case DVI_BOP:
			if (doingpage)
				GripeUnexpectedOp("BOP (during page)");
			BeginPage(first);
			if (UseThisPage) {
			   if (!last) {
			      putbyte(outf, DVI_PUSH);
			      CurrentPosition++;
			   }
			   if (hoffset != 0) {
			      putbyte(outf, DVI_RIGHT4) ;
			      PutLong(outf, hoffset) ;
			      CurrentPosition += 5;
			   }
			   if (voffset != 0) {
			      putbyte(outf, DVI_DOWN4) ;
			      PutLong(outf, voffset) ;
			      CurrentPosition += 5;
			   }
			}
			doingpage = 1;
			break;

		case DVI_EOP:
			if (!doingpage)
				GripeUnexpectedOp("EOP (outside page)");
			if (!last && UseThisPage) {
			   putbyte(outf, DVI_POP);
			   CurrentPosition++;
			}
			EndPage(last);
			doingpage = 0;
			return(1);

		case DVI_PRE:
			GripeUnexpectedOp("PRE");
			/* NOTREACHED */

		case DVI_POST:
			if (doingpage)
				GripeUnexpectedOp("POST (inside page)");
			return(0);

		case DVI_POSTPOST:
			GripeUnexpectedOp("POSTPOST");
			/* NOTREACHED */

		default:
			GripeUndefinedOp(c);
			/* NOTREACHED */
		}
	}
}

/* write an empty page to fill out space */
static void
PutEmptyPage(void)
{
        int i;

	putbyte(outf, DVI_BOP);
	PutLong(outf, -1L);
	for (i = 1; i < 10; i++)     /* set all sub counts to 0 */
		PutLong(outf, 0L);
	PutLong(outf, StartOfLastPage);
	putbyte(outf, DVI_EOP);
	if (!SFlag) {		/* write nice page usage messages */
		const char *msg = "[*]";
		message(1, msg, strlen(msg));
	}
	if (ferror(outf))
		error(1, -1, writeerr);

	StartOfLastPage = CurrentPosition;
	CurrentPosition += 46;	/* we just wrote this much */
	NumberOfOutputPages++;
}

/*
 * Here we scan the input DVI file and record pointers to the pages.
 */
static void
ScanDVIFile(void)
{
	long *tmp;

	UseThisPage = 0;

	StartOfPage[InputPageNumber] = ftell(inf);
	while (HandlePage(0, 0, 0, 0)) {  /* scan DVI file */
		++InputPageNumber;
		if (InputPageNumber >= StartOfPageSpace) {
			StartOfPageSpace *= 2;
			tmp = realloc(StartOfPage, sizeof(long) * StartOfPageSpace);
			if (!tmp) {
				error(1, -1, "cannot grow list of pages; out of memory");
			}
			StartOfPage = tmp;
		}

		StartOfPage[InputPageNumber] = ftell(inf);
	}
}

/*
 * Here we read the input DVI file and write relevant pages to the
 * output DVI file. We also keep track of font changes, handle font
 * definitions, and perform some other housekeeping.
 */
static void
HandleDVIFile(void)
{
        int CurrentPage, ActualPage, MaxPage;

	UseThisPage = 1;

	MaxPage = ((InputPageNumber+Modulo-1)/Modulo)*Modulo;

	for (CurrentPage = 0; CurrentPage < MaxPage; CurrentPage += Modulo) {
	   int add_last = 0;
	   struct pagespec *ps;
	   for (ps = PageSpecs; ps != NULL; ps = ps->next) {
	      int add_next = ps->add;
	      if (ps->reversed)
		 ActualPage = MaxPage-CurrentPage-Modulo+ps->pageno;
	      else
		 ActualPage = CurrentPage+ps->pageno;
	      if (ActualPage < InputPageNumber) {
		 if (fseek(inf, StartOfPage[ActualPage], 0) == -1)
		    error(1, -1,
			  "can't seek page %d", ActualPage+1);
		        HandlePage(!add_last, !add_next, ps->xoff, ps->yoff);
	      } else if (!add_last && !add_next)
    	                PutEmptyPage();
	      add_last = add_next;
	   }
	}
	if (fseek(inf, StartOfPage[InputPageNumber]+1, 0) == -1)
	        error(1, -1, "can't seek last page");
}
