/*
 * Copyright 1989 Dirk Grunwald
 * 
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Dirk Grunwald or M.I.T.
 * not be used in advertising or publicity pertaining to distribution of
 * the software without specific, written prior permission.  Dirk
 * Grunwald and M.I.T. makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 * 
 * DIRK GRUNWALD AND M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL M.I.T.  BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * Author:
 * 	Dr. Dirk Grunwald
 * 	Dept. of Computer Science
 * 	Campus Box 430
 * 	Univ. of Colorado, Boulder
 * 	Boulder, CO 80309
 * 
 * 	grunwald@colorado.edu
 * 	
 */ 

#include <stdio.h>
#include <ctype.h>

#include "dvistuff.h"	/* includes types & fonts */

static char *rcsid="$Header: /home/reed/grunwald/Iptex/drivers/RCS/dvistuff.c,v 1.17 88/07/07 23:25:32 grunwald Exp Locker: grunwald $" ;

#include "dviclass.h"
#include "dvicodes.h"
#include "postamble.h"
#include "search.h"
#include "fio.h"

int	dviHH = -1;		/* current horizontal position, in DEVs */
int	dviVV = -1;		/* current vertical position, in DEVs */
long	*dviCount[DVI_COUNT];	/* values of count */
DviStack dvi_current;
int	dviHHMargin = -1;	/* horizontal margin (in DEVs) */
int	dviVVMargin = -1;	/* vertical margin (in DEVs) */
int	dviTallestPage = -1;
int	dviWidestPage = -1 ;
int	dviCurrentPage = -1;
int	dviTotalPages = -1;
int	dviDPI = -1;
int	dviMaxDrift = -1;
int	dviUserMag = -1;	/* user-specified magnification */
int	dviBlackness = -1;
int	dviFontRotation = -1;
char	*DVIFileName;
static char *dviTmpFileName = 0;
static char dviTmpFileNameBuffer[256];
FILE	*dviFile;				/* users file */
char	*dviPrintEngine = "canon";

char	*ProgName;

/*
 * Similar to dvi_stack, but includes `dviHH and `dviVV, which are usually
 * but not always the same as fromSP(h) and fromSP(v):
 */
struct localstack {
	int stack_hh;
	int stack_vv;
	struct dvi_stack stack_dvi;
};

static struct localstack *dvi_stack = 0;/* base of stack */
static struct localstack *dvi_stackp;	/* current place in stack */

/*
 * DVI preamble and postamble information.
 */

static long Magnification;
static long	Numerator;		/* numerator from DVI file */
static long	Denominator;		/* denominator from DVI file */

/*
 *	Font related things.
 *	We keep track of the current font, use the fontfiner
 *	and record errors in the dvi postable in FontErros. 
 *	the NoFont is a used when we cant find a font,
 *	to avoid errors.
 */

struct fontinfo *dviCurrentFont = 0;

/*
 *	imagen1-special uses the NextFamilyNumber. *sigh*
 */

int NextFamilyNumber = 0;
static int MaxFontFamily = MAX_FONTFAMILY;

static struct search *FontFinder = 0;
int FontErrors = 0;
static struct fontinfo NoFont;
/*
 *	We keep a list of pointers to the beginning of pages.
 *	Each page is headed by a DVI_BOP + 44 bytes of information.
 */

static long *pageOffset = 0;

#undef FIXDRIFT
/*
 * Correct devpos (the actual device position) to be within MaxDrift pixels
 * of dvipos (the virtual DVI position).
 */
#define FIXDRIFT(devpos, dvipos) \
	if (ABS((devpos) - (dvipos)) <= dviMaxDrift) \
		/* void */; \
	else \
		if ((devpos) < (dvipos)) \
			(devpos) = (dvipos) - dviMaxDrift; \
		else \
			(devpos) = (dvipos) + dviMaxDrift

#define	ABS(X)	( (X) < 0 ? -(X) : (X) )


/*
 * Store the relevant information from the DVI postamble, and set up
 * various internal things.
 */
static void
PostAmbleHeader(p)
    register struct PostAmbleInfo *p;
{
    int i;
    int page;
    int pageSize;
    int stackSize;
    long prevPagePointer;
    long wuzAt;
    
    Numerator = p->pai_Numerator;
    Denominator = p->pai_Denominator;
    Magnification = p->pai_DVIMag;
    
/*
 *	Set the conversion factor.  This must be done before using
 *	any fonts or the fromSP routine.
 */
    
    SetConversion(dviDPI, dviUserMag, Numerator, Denominator, Magnification);
    
    dviTotalPages = p -> pai_NumberOfPages;
    dviWidestPage = fromSP(p -> pai_WidestPageWidth);
    dviTallestPage = fromSP(p -> pai_TallestPageHeight);

/*
 *	Set up the DVI stack
 */
    stackSize = p -> pai_DVIStackSize * sizeof(*dvi_stack);
    if (dvi_stack != 0) {
	free(dvi_stack);
    }
    dvi_stack = (struct localstack *) malloc((unsigned) stackSize);  

/*
 *	Set of the table of pointers to pages and room for the
 *	count variables.
 */
    
    pageSize = dviTotalPages * sizeof(long);
    if (pageOffset != 0) {
	free(pageOffset);
    }
    pageOffset = (long *) malloc(pageSize);
    
    if (pageOffset == NULL) {
	fprintf(stderr,"xdvi: Can not allocate page directory (%d pages)",
		dviTotalPages);
	exit(1);
    }

    for (i = 0; i < DVI_COUNT; i++) {
	if (dviCount[i] != 0) {
	    free(dviCount[i]);
	}
	dviCount[i] = (long*) malloc(pageSize);
	if (dviCount[i] == NULL) {
	    fprintf(stderr,"xdvi: Can not allocate count directory (%d pages)",
		    dviTotalPages);
	    exit(1);
	}
    }
	    
    
/*
 * Follow back pointers through pages in the DVI file,
 * storing the offsets in the pageOffset table.
 */

    prevPagePointer = p->pai_PrevPagePointer;
    wuzAt = (long) ftell(dviFile);
    
    for (page = dviTotalPages - 1; page >= 0 ; page--) {

/*
 *	Skip to previous page, but skip over BOP marker.
 */
	pageOffset[page] = prevPagePointer;
	if( fseek(dviFile, prevPagePointer+1, 0) < 0) {
	    perror("fseek");
	    fprintf(stderr,"[postamble] improper seek looking up pages\n");
	    fprintf(stderr,"prevPagePointer = %lx\n",
		    prevPagePointer);
	    exit(1);
	}

/*
 *	Read the other counters at the beginning of the page
 */

	for (i = 0; i < DVI_COUNT; i++) {
	    long value;
	    fGetLong(dviFile, value);
	    dviCount[i][page] = value;
	    }
	fGetLong(dviFile, prevPagePointer);
    }
    fseek(dviFile, wuzAt, 0);
}

static void
PostAmbleFontDef(p)
register struct PostAmbleFont *p;
{
    register struct fontinfo *fi;
    register struct font *f;
    
    char *fname;
    int def = S_CREATE | S_EXCL;
    
    fi = (struct fontinfo *) SSearch(FontFinder, p->paf_DVIFontIndex,
				     &def);
    if (fi == NULL) {
/*
 *	If the font already exists, lets just check that its really the
 *	same thing & return
 */

	if ((def & S_FOUND) | (def & S_COLL)) {

	    def = S_LOOKUP;
	    if ((fi = (struct fontinfo *) SSearch(FontFinder,
						  p -> paf_DVIFontIndex,
						  &def)) == 0){
		GripeNoSuchFont(p -> paf_DVIFontIndex);
		exit(1);
	    }

	    if (fi -> f == NULL) {
		GripeCannotGetFont(p->paf_name, p->paf_DVIMag,
				   p->paf_DVIDesignSize,
				   dviPrintEngine, fname);
	    }
	    f = fi -> f;
	    if (p->paf_DVIChecksum && f->f_checksum &&
		p->paf_DVIChecksum != f->f_checksum) {
		    GripeDifferentChecksums(fname, p->paf_DVIChecksum,
					    f->f_checksum);
		    exit(1);
		}
	    return;	/* font already defined */
	}
	else {
	    fprintf(stderr, "Can not stash font %ld (out of memory?)\n",
		    p -> paf_DVIFontIndex);
	    exit(1);
	}
    }

    if (NextFamilyNumber == MaxFontFamily) {
	fprintf(stderr,"Out of font family!\n");
	exit(1);
    }

    fi->family = NextFamilyNumber;
    NextFamilyNumber++;

    f = GetFont(p->paf_name, p->paf_DVIMag, p->paf_DVIDesignSize,
		dviPrintEngine, &fname);

/*
 *	Give the application a chance to bang on this if they want to.
 */

    f = applicationNewFont(f, fi -> family);

    if ((fi->f = f) == NULL) {
	GripeCannotGetFont(p->paf_name, p->paf_DVIMag,
			   p->paf_DVIDesignSize, dviPrintEngine, fname);
	FontErrors++;
	return;
    }

    /* match checksums, if not zero */
    if (p->paf_DVIChecksum && f->f_checksum &&
	p->paf_DVIChecksum != f->f_checksum)
	GripeDifferentChecksums(fname, p->paf_DVIChecksum,
				f->f_checksum);
    
    fi->pspace = p->paf_DVIMag / 6;	/* a three-unit "thin space" */
    fi->nspace = -4 * fi->pspace;
    fi->vspace = 5 * fi->pspace;
}

static void
ReadPostAmble()
{
    
    if (FontFinder == 0) {
	if ((FontFinder = SCreate(sizeof(struct fontinfo))) == 0) {
	    fprintf(stderr, "can not create FontFinder (out of memory?)");
	    exit(1);
	}
    }

    ScanPostAmble(dviFile, PostAmbleHeader, PostAmbleFontDef);

    if (FontErrors)
	GripeMissingFontsPreventOutput(FontErrors);
}

/*
 *	Read the preamble and do a few sanity checks
 */

static void
ReadPreAmble()
{
	register int n;

	rewind(dviFile);
	if (GetByte(dviFile) != Sign8(DVI_PRE))
		GripeMissingOp("PRE");

	if (GetByte(dviFile) != Sign8(DVI_VERSION))
		GripeMismatchedValue("version numbers");

	if (GetLong(dviFile) != Numerator)
		GripeMismatchedValue("numerator");

	if (GetLong(dviFile) != Denominator)
		GripeMismatchedValue("denominator");

	if (GetLong(dviFile) != Magnification)
		GripeMismatchedValue("\\magfactor");

	n = UnSign8(GetByte(dviFile));
	while (--n >= 0)
		(void) GetByte(dviFile);
}

int
dviInit()
{
    extern char *ProgName;
    FILE *tmpFile;
#ifndef WIN32
    char *mktemp();
#endif

    dviFini();	/* clean up the old files */

    if (DVIFileName == 0) {
	dviFile = stdin;
	DVIFileName = "<stdin>";
    }else if ((dviFile = fopen(DVIFileName, "r")) == NULL) {
	int n = strlen(DVIFileName);
	char *dvi_name;
	
	if (strcmp(DVIFileName + n - sizeof(".dvi") + 1, ".dvi") == 0) {
	    perror(DVIFileName);
	    return(1);
	}

	dvi_name = (char *) malloc((unsigned) n + sizeof(".dvi"));
	sprintf(dvi_name, "%s.dvi", DVIFileName);

	if ((dviFile = fopen(dvi_name, "r")) == NULL) {
	    perror(dvi_name);
	    return(1);
	}
	DVIFileName = dvi_name;
    }

/*
 *	node, copy the file to a temporary location. This lets the person
 *	peruse the file while theyre re-texing it
 */
    strcpy(dviTmpFileNameBuffer,"/tmp/dvistuff.XXXXXX");
    dviTmpFileName = mktemp(dviTmpFileNameBuffer);

    if (!(tmpFile = fopen(dviTmpFileName,"w+"))) {
	perror("fopen");
	fprintf(stderr,"Unable to create temporary file");
	dviTmpFileName = 0;
    } else {
	char buffer[BUFSIZ];
	int b;

	rewind(dviFile);
	do {
	    b = fread(buffer, 1, BUFSIZ, dviFile);
	    fwrite(buffer, 1, b, tmpFile);
	} while (! feof(dviFile));

	fclose(dviFile);
	dviFile = tmpFile;
	rewind(dviFile);
    }
/*
 *	Unlink the temporary file. This keeps tmp files from cluddering
 *	up /tmp and it does it in a very application-independent way.
 *	You can't reopen the tmp file, but we don't really allow that
 *	anyway (the tmp file is hidden from the user).
 */
    if (dviTmpFileName != 0 &&
	strncmp(dviTmpFileName,"/tmp/",5) == 0) {
	    unlink(dviTmpFileName);
	}

    if (dviUserMag == -1) {
	dviUserMag = 1000;
    }

    if (dviMaxDrift == -1) {
	dviMaxDrift = DEFAULT_MAX_DRIFT;
    }

    if (dviDPI == -1) {
	dviDPI = DEFAULT_DPI;
    }

    if (dviBlackness == -1) {
	dviBlackness = DEFAULT_BLACKNESS;
    }

    if (dviFontRotation == -1) {
	dviFontRotation = ROT_NORM;
    }

    ReadPostAmble();
    ReadPreAmble();

    /* Margins -- needs work! */

    if (dviHHMargin == -1) {
	dviHHMargin = DEFAULT_HHMARGIN;
    }

    if (dviVVMargin == -1) {
	dviVVMargin = DEFAULT_VVMARGIN;
    }

    dviCurrentFont = &NoFont; /* fake font, all zeros */

    return(0);
}

void
dviFini()
{
    if (dviFile != 0) {
	fclose(dviFile);
    }

    if (dviTmpFileName != 0 &&
	strncmp(dviTmpFileName,"/tmp/",5) == 0) {
	    unlink(dviTmpFileName);
	}
}
	

static void
SelectFont(n)
i32 n;
{
    int x = S_LOOKUP;

    if ((dviCurrentFont = (struct fontinfo *)SSearch(FontFinder, n, &x)) == 0)
	GripeNoSuchFont(n);
}

/* Read the postamble. */

static void
doDviChar(c, advance)
int c;
int advance;
{
    register struct glyph *g;
    register struct font *f;
    int p;

    f = dviCurrentFont->f;

    if (c < f -> f_lowch || c > f -> f_highch) {
	fprintf(stderr,"Character out of range: %d\n",
		c);
	return;
    }

    g = GLYPH(f, c);
    if (!GVALID(g)) {
	fprintf(stderr, "there is no character %d in %s",
	      c, f->f_path);
	exit(1);
    } else {

	if (HASRASTER(g)) {	/* workaround for Imagen bug */
	    applicationPutChar(dviHH,dviVV,c);
	}
	
	if (advance) {
	    dviHH += g->g_pixwidth;
	    dvi_h += g->g_tfmwidth;
	    p = fromSP(dvi_h);
	    FIXDRIFT(dviHH, p);
	}
    }
}

/*
 *	The next two routines are used to reset all the glyphs.
 *	the dviResetFont routine is called for each font in the font table.
 *	It de-allocates the rasters & glyphs for the shrunken fonts, leaving
 *	the original glyph data untouched.
 */


static void
dviResetFont(fi, key)
struct fontinfo *fi;
int key;
{
    if (fi != 0) {
	applicationResetFont(fi, fi -> family);
    }
}
	    
void
dviResetAll()
{
    SEnumerate(FontFinder, dviResetFont);
}

/*
 *	Process the DVI commands for page #page. If the page number is
 *	out of range, it is cropped to the end.
 *
 *	Updates dviCurrentPage
 */
void
dviPreparePage(page)
int page;
{
    register int c;
    register i32 p;
    int advance;

    if (page < 0) {
	page = 0;
    }

    if (page >= dviTotalPages) {
	page = dviTotalPages - 1;
    }

    dviCurrentPage = page;

    fseek(dviFile, (long) pageOffset[page], 0);

    dviHH = dviHHMargin;
    dviVV = dviVVMargin;

    dvi_h = toSP(dviHH);
    dvi_v = toSP(dviVV);

    dvi_w = 0;
    dvi_x = 0;
    dvi_y = 0;
    dvi_z = 0;

    dvi_stackp = dvi_stack;
    
    for(;;) {
/*
 * Get the DVI byte, and switch on its parameter length and type.
 * Note that getchar() returns unsigned values.
 */

	if (feof(dviFile)) {
	    fprintf(stderr,"Unexpected end-of-file\n");
	    exit(1);
	}

	c = getc(dviFile);

	if (DVI_IsChar(c)) {
	    advance = 1;
	    doDviChar(c, advance);
	} else {
	
	switch (DVI_OpLen(c)) {
	    
	case DPL_NONE:
	    break;
	    
	case DPL_SGN1:
	    p = getc(dviFile);
	    p = Sign8(p);
	    break;
	    
	case DPL_SGN2:
	    fGetWord(dviFile, p);
	    p = Sign16(p);
	    break;
	    
	case DPL_SGN3:
	    fGet3Byte(dviFile, p);
	    p = Sign24(p);
	    break;
	    
	case DPL_SGN4:
	    fGetLong(dviFile, p);
	    break;
	    
	case DPL_UNS1:
	    p = UnSign8(getc(dviFile));
	    break;
	    
	case DPL_UNS2:
	    fGetWord(dviFile, p);
	    p = UnSign16(p);
	    break;
	    
	case DPL_UNS3:
	    fGet3Byte(dviFile, p);
	    p = UnSign24(p);
	    break;
	    
	default:
	    panic("DVI_OpLen(%d) = %d", c, DVI_OpLen(c));
	    /* NOTREACHED */
	}
	
	switch (DVI_DT(c)) {
	    
	case DT_SET:
	    c = p;
	    doDviChar(c, 1);
	    break;
	    
	case DT_PUT:
	    c = p;
	    doDviChar(c, 0);
	    break;

	case DT_SETRULE:
	case DT_PUTRULE:
	    
	{
	    i32 h, w, rw;
	    
	    fGetLong(dviFile, h);
	    fGetLong(dviFile, rw);
	    
	    h = ConvRule(h);
	    w = ConvRule(rw);
	    
	    applicationSetRule(dviHH, dviVV, h,w);
	    
	    if (DVI_DT(c) == DT_SETRULE)  {
		dviHH += w;
		dvi_h += rw;
		w = fromSP(dvi_h);
		FIXDRIFT(dviHH, w);
	    }
	}
	    break;
	    
	case DT_NOP:
	    break;
	    
	case DT_BOP:
/*
 *	Each beginning of page has 11 4-byte words telling us things
 *	about the page. We ignore them.
 */
	{
	    fseek(dviFile, (long) (11 * 4), 1);
	}

	    break;
	    
	case DT_EOP:
	    return;
	    
	case DT_PUSH:
	    dvi_stackp->stack_hh = dviHH;
	    dvi_stackp->stack_vv = dviVV;
	    dvi_stackp->stack_dvi = dvi_current;
	    dvi_stackp++;
	    break;
	    
	case DT_POP:
	    dvi_stackp--;
	    dviHH = dvi_stackp->stack_hh;
	    dviVV = dvi_stackp->stack_vv;
	    dvi_current = dvi_stackp->stack_dvi;
	    break;
	    
	case DT_W0:	/* there should be a way to make these pretty */
	    p = dvi_w;
	    goto move_right;
	    
	case DT_W:
	    dvi_w = p;
	    goto move_right;
	    
	case DT_X0:
	    p = dvi_x;
	    goto move_right;
	    
	case DT_X:
	    dvi_x = p;
	    goto move_right;
	    
	case DT_RIGHT:
	move_right:
	    dvi_h += p;
/*
 * DVItype tells us that we must round motions in this way:
 * `When the horizontal motion is small, like a kern, hh
 * changes by rounding the kern; but when the motion is
 * large, hh changes by rounding the true position so that
 * accumulated rounding errors disappear. 
 */
	    if (p >= dviCurrentFont->pspace || p <= dviCurrentFont->nspace)
		dviHH = fromSP(dvi_h);
	    else {
		dviHH += fromSP(p);
		p = fromSP(dvi_h);
		FIXDRIFT(dviHH, p);
	    }
	    break;
	    
	case DT_Y0:
	    p = dvi_y;
	    goto move_down;
	    
	case DT_Y:
	    dvi_y = p;
	    goto move_down;
	    
	case DT_Z0:
	    p = dvi_z;
	    goto move_down;
	    
	case DT_Z:
	    dvi_z = p;
	    goto move_down;
	    
	case DT_DOWN:
	move_down:
	    dvi_v += p;
/*
 * `Vertical motion is done similarly, but with the threshold
 * between ``small and ``large increased by a factor of
 * 5.  The idea is to make fractions like $1\over2$ round
 * consistently, but to absorb accumulated rounding errors in
 * the baseline-skip moves. 
 */
	    if (ABS(p) >= dviCurrentFont->vspace)
		dviVV = fromSP(dvi_v);
	    else {
		dviVV += fromSP(p);
		p = fromSP(dvi_v);
		FIXDRIFT(dviVV, p);
	    }
	    break;
	    
	case DT_FNTNUM:
	    SelectFont((i32) (c - DVI_FNTNUM0));
	    break;
	    
	case DT_FNT:
	    SelectFont(p);
	    break;
	    
	case DT_XXX:
	{
	    char specialBuffer [2048];
	    register char *cp;
	    int sweetp = 0;

	    if (p > 2047) {
		sweetp = p - 2047;
		p = 2047;
	    }
	    
	    for (cp = specialBuffer ; p > 0; p--) {
		*cp = getc(dviFile);
		cp++;
	    }
	    *(cp) = 0;

	    while(sweetp > 0) {
		getc(dviFile);
	    }

	    applicationDoSpecial(specialBuffer);
	}
	    break;
	    
	case DT_FNTDEF:
	    SkipFontDef(dviFile);
	    break;
	    
	case DT_PRE:
	    GripeUnexpectedOp("PRE");
	    /* NOTREACHED */
	    
	case DT_POST:
	    GripeUnexpectedOp("POST");
	    /* NOTREACHED */
	    
	case DT_POSTPOST:
	    GripeUnexpectedOp("POSTPOST");
	    /* NOTREACHED */
	    
	case DT_UNDEF:
	    GripeUndefinedOp(c);
	    /* NOTREACHED */
	    
	default:
	    panic("DVI_DT(%d) = %d", c, DVI_DT(c));
	    /* NOTREACHED */
	}
    }
    }
}

/* Transpose bytes in each pair of bytes  (0123 -> 1032) */
/* Reverse bits in a byte  (01234567 -> 76543210) */
/* needed for sun386i */
/* Gerhard Fleischanderl & Wolfgang Nejdl */

#ifdef sun386
static unsigned char pow2[]={1,2,4,8,16,32,64,128};

unsigned char reverseByte(oldByte)
     unsigned char oldByte;
{
  unsigned short i;
  unsigned char newByte=0;

  /*  build newByte with bits mirrored */

  for(i=0;i<8;i++)
     if (oldByte & pow2[i])
        newByte |= pow2[7-i];

  return newByte;
}

#ifdef UNDEF
#define transposeBytesMirrorBits(pairPtr) \
/* B16 *pairPtr; */ \
/* B16 comprises 2 bytes */ \
{ \
  unsigned char c0,c1, *bP = (unsigned char *) pairPtr; \
 \
  /* transpose neighbouring bytes and \
     reverse bits in each byte */ \
 \
  c0 = *bP; \
  c1 = *(bP+1); \
  *bP = ReverseByteTable[c1]; \
  *(bP+1) = ReverseByteTable[c0]; \
}

#else
#define transposeBytesMirrorBits(ptr) {*ptr = ReverseByteTable[ (*ptr)&0xff ];}
#endif /* undef */

#endif
  

/*
 *	The following routine is used to shrink a glyph by some
 *	shrink factor (in the width & height dimension).
 *
 *	These shrunken glyphs are more appropriate for previewers.
 *
 *	To do this, we simple scan the original raster, counting the
 *	number of pixels within a given area. If the number of on pixels is
 *	at least twice the total number of pixels, we turn the pixel in
 *	the shrunken glyph ON, else OFF.
 *
 *	We use a lookup table to hasten the process of counting pixels.
 *
 *	The method used should be independent of byte-order (I think).
 *
 *	You need to define two types. One should be 32-bits long, and
 *	the other 16 bits long.
 */

typedef unsigned char B8;
typedef unsigned short B16;
typedef unsigned long B32;

#define LOOKUP_BYTES	256
#define	LOOKUP_BITS	8
#define LOOKUP_MASK	0xff

static char dviLookUpTable[LOOKUP_BYTES];
static char tableNotInitialized = 1;

#ifdef sun386
static unsigned char ReverseByteTable[LOOKUP_BYTES];
#endif

struct glyph *
dviShrinkGlyph(gly, shrinkH, shrinkW)
DviGlyph *gly;
int shrinkH;
int shrinkW;
{
    int shrunkHeight;
    int shrunkWidth;
    int glyphWide;
    int glyphHigh;
    int glyphAdvance;
    int shrunkAdvance;

    int bytesWide;
    int shrunkBytesWide;

    struct glyph *ngly;

    B16 *shrunkRaster;
    int rasterSize;

    int x,y;
    B8 *cp;
    B8 *ptr;

    B32 shrinkMask;

    int sampleSize;

    if (gly == 0 || !HASRASTER(gly)) {
	return(0);
    }

    if (gly -> g_raster == 0) {
	gly-> g_raster = RASTER(gly, dviCurrentFont -> f, dviFontRotation);
    }

/*
 *	Initialize the lookup table of the number of bits in a given byte
 */

    if (tableNotInitialized) {
	register int i;
	register int j;
	register int k;
	register int acc;
	for (i = 0; i < LOOKUP_BYTES; i++) {
	    j = i;
	    acc = 0;
	    for (k = 0; j != 0 && k < LOOKUP_BITS ; k++) {
		acc += (j & 1);
		j >>= 1;
	    }
	    dviLookUpTable[i] = acc;
	}

#ifdef sun386
        for (i = 0; i < LOOKUP_BYTES; i++) {
          ReverseByteTable[i] = reverseByte(i);
        }
#endif

	tableNotInitialized = 0;
    }

/*
 *	When shrinking the fonts, we convert them from byte alligned
 *	to 16 bit-aligned fonts. I think that this is needed by X,
 *	and also SunWindows. This is done rather than changing the
 *	library routines because some device drivers, i.e. imagen1,
 *	depend on the byte-alligned fonts.
 */
    glyphHigh = gly -> g_height;
    glyphWide = gly -> g_width;

    gly -> g_pixwidth = fromSP(gly -> g_tfmwidth);
    glyphAdvance = gly -> g_pixwidth;

    shrunkHeight = (glyphHigh + shrinkH - 1) / shrinkH;
    shrunkWidth =  (glyphWide + shrinkW - 1) / shrinkW;
    shrunkAdvance =  (glyphAdvance + shrinkW - 1) / shrinkW;

    bytesWide = (gly -> g_width + 7) >> 3;
    
    shrunkBytesWide = ((shrunkWidth + 15) >> 4) * 2;

    rasterSize = (shrunkHeight + 1) * shrunkBytesWide;
    shrunkRaster = (B16 *) malloc(rasterSize);
    bzero(shrunkRaster, rasterSize);
    ptr = (B8 *) shrunkRaster;
    
    if (shrunkRaster == NULL) {
	fprintf(stderr, "Out of memory!\n");
	exit(1);
    }

    for (y = 0; y < glyphHigh; y+= shrinkH) {
	cp = (B8 *) ptr;
	shrinkMask = 0x80;
	for (x = 0; x < glyphWide; x += shrinkW) {
	    int i;
	    int samples;
	    B8 *baseP;
	    int upper;
	    register int thisShrinkW;

	    baseP = (B8 *) gly -> g_raster + (y * bytesWide);

/*
 *	Set the upper limit on the height iteration so we dont count
 *	off the end of the raster
 */

	    upper = y + shrinkH;
	    if (upper > glyphHigh) {
		upper = glyphHigh;
	    }

	    if (x + shrinkW > glyphWide) {
		thisShrinkW = glyphWide - x;
	    } else {
		thisShrinkW = shrinkW;
	    }

	    samples = 0;
	    sampleSize = thisShrinkW * (upper - y);

	    for (i = y; i < upper; i++) {
		register int acc;
		register B8 *p;
		register B8 *ep;
/*
 *	Determine how many bytes our shrink window crosses (we might
 *	overlap on byte-edges)
 */

		p = baseP + (x >> 3);
		ep = baseP + ( (x + thisShrinkW - 1) >> 3);
		baseP += bytesWide;

/*
 *	stuff everything in the accumulator
 */

		acc = 0;
		while (p <= ep) {
		    acc = ((acc << 8) & ~0xff) | *p;
		    p++;
		}

/*
 *	clean off the right hand-side of extra bits, then clean off
 *	the left hand side of extra bits, and then count them.
 */

		acc = acc >> ( 7 - ((x + thisShrinkW - 1) & 0x7));
		acc &= ~(-1 << thisShrinkW);
		while (acc != 0) {
		    samples += dviLookUpTable[ acc & LOOKUP_MASK ];
		    acc >>= LOOKUP_BITS;
		}
	    }
/*
 *	If at least 1/blackness of the bits are on, treat this entire sample as
 *	being on.
 */

	    if ((samples * dviBlackness) >= sampleSize) {
		*ptr |= shrinkMask;
	    } else {
		*ptr &= ~ shrinkMask;
	    }
	    shrinkMask >>= 1;
	    if (shrinkMask == 0) {
		shrinkMask = 0x80;
#ifdef sun386
	    transposeBytesMirrorBits(ptr)
#endif
		ptr ++;
	    }
	}
#ifdef sun386
	transposeBytesMirrorBits(ptr)
#endif
	ptr = (B8 *) (cp + shrunkBytesWide);
    }

/*
 *	Build a new glyph from the shrunken raster
 */

#ifdef	UNDEF
    if ( shrunkBytesWide != 2 )
      {
        printf("Old glyph:\n");
        seeGlyph(gly -> g_raster, glyphHigh, bytesWide);
        printf("%d ", shrunkBytesWide);
        printf("New glyph:\n");
        seeGlyph(shrunkRaster, shrunkHeight, shrunkBytesWide);
      }
#endif	UNDEF

    ngly = (struct glyph *) malloc(sizeof(struct glyph));
    bzero(ngly, sizeof(struct glyph));

    ngly -> g_raster = (char * ) shrunkRaster;
    ngly -> g_width = shrunkWidth;
    ngly -> g_pixwidth = shrunkAdvance;
    ngly -> g_height = shrunkHeight;

    ngly -> g_xorigin = gly -> g_xorigin / shrinkH;
    ngly -> g_yorigin = gly -> g_yorigin / shrinkW;

    ngly -> g_flags |= GF_SHRUNK;	/* yes, its been shrunk */

    return(ngly);
}

#ifdef	UNDEF

seeGlyph(c, h, w)
char *c;
int h;
int w;
{
    int i,j;

    for (i = 0; i < h; i++ ) {
	for (j = 0; j < w; j++) {
	    int k;
	    register int ch;
	    register int m;
	    char str[9];

	    ch = *(c++);
	    m = 0x80;
	    for (k = 0; k < 8; k++) {
		str[k] = '"' + ( (ch & m) ? 1 : 0 );
		m >>= 1;
	    }
	    str[8] = 0;
	    printf("%s", str);
	}
	printf("\n");
    }
}
		
		
#endif	UNDEF
