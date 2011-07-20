/* $XConsortium: type1.c,v 1.5 91/10/10 11:20:06 rws Exp $ */
/* Copyright International Business Machines, Corp. 1991
 * All Rights Reserved
 * Copyright Lexmark International, Inc. 1991
 * All Rights Reserved
 * Portions Copyright (c) 1990 Adobe Systems Incorporated.
 * All Rights Reserved
 *
 * License to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM or Lexmark or Adobe
 * not be used in advertising or publicity pertaining to distribution of
 * the software without specific, written prior permission.
 *
 * IBM, LEXMARK, AND ADOBE PROVIDE THIS SOFTWARE "AS IS", WITHOUT ANY
 * WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT
 * LIMITED TO ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NONINFRINGEMENT OF THIRD PARTY RIGHTS.  THE
 * ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE SOFTWARE, INCLUDING
 * ANY DUTY TO SUPPORT OR MAINTAIN, BELONGS TO THE LICENSEE.  SHOULD ANY
 * PORTION OF THE SOFTWARE PROVE DEFECTIVE, THE LICENSEE (NOT IBM,
 * LEXMARK, OR ADOBE) ASSUMES THE ENTIRE COST OF ALL SERVICING, REPAIR AND
 * CORRECTION.  IN NO EVENT SHALL IBM, LEXMARK, OR ADOBE BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
 
/*********************************************************************/
/*                                                                   */
/* Type 1 module - Converting fonts in Adobe Type 1 Font Format      */
/*                 to scaled and hinted paths for rasterization.     */
/*                 Files: type1.c, type1.h, and blues.h.             */
/*                                                                   */
/* Authors:   Sten F. Andler, IBM Almaden Research Center            */
/*                 (Type 1 interpreter, stem & flex hints)           */
/*                                                                   */
/*            Patrick A. Casey, Lexmark International, Inc.          */
/*                 (Font level hints & stem hints)                   */
/*                                                                   */
/*********************************************************************/


/* Write debug info into a PostScript file? */
/* #define DUMPDEBUGPATH */
/* If Dumping a debug path, should we dump both character and
   outline path? Warning: Do never enable this, unless, your name
   is Rainer Menzner and you know what you are doing! */
/* #define DUMPDEBUGPATHBOTH */

/* Generate a bunch of stderr output to understand and debug
   the generation of outline surrounding curves */
/* #define DEBUG_OUTLINE_SURROUNDING */

#define SUBPATH_CLOSED     1
#define SUBPATH_OPEN       0

/******************/
/* Include Files: */
/******************/
#include  "types.h"
#include  <stdio.h>          /* a system-dependent include, usually */
#include  <math.h>
#include  <stdlib.h>

#include  "objects.h"
#include  "spaces.h"
#include  "paths.h"
#include  "fonts.h"        /* understands about TEXTTYPEs */
#include  "pictures.h"     /* understands about handles */
 
typedef struct xobject xobject;
#include  "util.h"       /* PostScript objects */
#include  "fontfcn.h"
#include  "blues.h"          /* Blues structure for font-level hints */


/* Considerations about hinting (2002-07-11, RMz (Author of t1lib))

   It turns out that the hinting code as used until now produces some
   artifacts in which may show up in suboptimal bitmaps. I have therefore
   redesigned the algorithm. It is generally a bad idea to hint every
   point that falls into a stem hint.

   The idea is to hint only points for
   which at least one of the two neighboring curve/line segments is aligned
   with the stem in question. For curves, we are speaking about the
   tangent line, that is, the line defined by (p1-p2) or (p3-p4).

   For vertical stems this means, that only points which are connected
   exactly into vertical direction are hinted. That is, the dx of the
   respective curve vanishes. For horizontal stems, accordingly, dy must
   vanish at least on one hand side of the point in order to be considered
   as a stem.

   Unfortunately this principle requires information about both sides of the
   neighborhood of the point in question. In other words, it is not possible
   to define a segment completely until the next segment has been inspected.
   The idea thus is not compatible with the code in this file.

   Furthermore, if certain points of a character outline are hinted according
   to the stem hint info from the charstring, the non-hinted points may not be
   left untouched. This would lead to very strong artifacts at small sizes,
   especially if characters are defined in terms of curves. This is predominantly
   the case for ComputerModern, for example.

   To conclude, it is best to build a point list from the character description
   adjust the non-hinted points after hinting has been completely finished. 

   
   Another rule we should state is
   
   We can work around this by not directly connecting the path segments at
   the end of the lineto/curveto's, but rather deferring this to the beginning
   of the next path constructing function. It's not great but it should work.

   The functions that produce segments are

   1) RMoveTo()
   2) RLineto()
   3) RRCurveTo()
   4) DoClosePath()

   Their code is moved into the switch statement of the new function
   handleCurrentSegment(). This function is called when a new segment generating
   operation has been decoded from the charstring. At this point a serious
   decision about how to hint the points is possible.

   ...
*/


/* The following struct is used to record points that define a path
   in absolute charspace coordinates. x and y describe the location and
   hinted, if greater 0, indicates that this point has been hinted. Bit 0
   (0x1) indicates vertically adjusted and Bit 1 (0x2) indicates
   horizontally adjusted. If hinted == -1, this point is not to be hinted
   at all. This, for example, is the case for a a (H)SBW command.

   The member type can be one of

   PPOINT_SBW                --> initial path point as setup by (H)SBW
   PPOINT_MOVE               --> point that finishes a MOVE segment
   PPOINT_LINE               --> point that finishes a LINE segment
   PPOINT_BEZIER_B           --> second point of a BEZIER segment
   PPOINT_BEZIER_C           --> third point of a BEZIER segment
   PPOINT_BEZIER_D           --> fourth point of a BEZIER segment
   PPOINT_CLOSEPATH          --> a ClosePath command
   PPOINT_ENDCHAR            --> an EndChar command
   PPOINT_SEAC               --> a Standard Encoding Accented Char command
   PPOINT_NONE               --> an invalid entry
   

   Note: BEZIER_B and BEZIER_C points generally cannot be flagged as
   being hinted because are off-curve points.
*/
typedef struct 
{
  double x;              /* x-coordinate */
  double y;              /* y-coordinate */
  double ax;             /* adjusted x-coordinate */
  double ay;             /* adjusted y-coordinate */
  double dxpr;           /* x-shift in right path due to incoming segment (previous) */
  double dypr;           /* y-shift in right path due to incoming segment (previous) */
  double dxnr;           /* x-shift in right path due to outgoing segment (next) */
  double dynr;           /* y-shift in right path due to incoming segment (next) */
  double dxir;           /* x-shift in right path resulting from prologation of the linkend tangents (intersect) */
  double dyir;           /* y-shift in right path resulting from prologation of the linkend tangents (intersect) */
  double dist2prev;      /* distance to the previous point in path (used only for stroking) */
  double dist2next;      /* distance to the next point in path (used only for stroking) */
  enum 
  {
    PPOINT_SBW,
    PPOINT_MOVE,
    PPOINT_LINE,
    PPOINT_BEZIER_B,
    PPOINT_BEZIER_C,
    PPOINT_BEZIER_D,
    PPOINT_CLOSEPATH,
    PPOINT_ENDCHAR,
    PPOINT_SEAC,
    PPOINT_NONE
  } type;                /* type of path point */
  signed char   hinted;  /* is this point hinted? */
  unsigned char shape;   /* is the outline concave or convex or straight at this point? This flag
			    is only relevant for onCurve points in the context of stroking! */
} PPOINT;

#define CURVE_NONE            0x00
#define CURVE_STRAIGHT        0x01
#define CURVE_CONVEX          0x02
#define CURVE_CONCAVE         0x03

#ifdef DEBUG_OUTLINE_SURROUNDING
static char* pptypes[] = {
  "PPOINT_SBW",
  "PPOINT_MOVE",
  "PPOINT_LINE",
  "PPOINT_BEZIER_B",
  "PPOINT_BEZIER_C",
  "PPOINT_BEZIER_D",
  "PPOINT_CLOSEPATH",
  "PPOINT_ENDCHAR",
  "PPOINT_SEAC"
};
static char* ppshapes[] = {
  "SHAPE_OFFCURVE",
  "SHAPE_STRAIGHT",
  "SHAPE_CONVEX",
  "SHAPE_CONCAVE"
};
#endif


/* The PPOINT structs are organized in an array which is allocated
   in chunks of 256 entries. A new point is allocated by a call to
   nextPPoint and returns the index in the array of the newly
   allocated point. */
static PPOINT* ppoints       = NULL;
static long numppoints       = 0;
static long numppointchunks  = 0;
static int  closepathatfirst = 0;

static long nextPPoint( void) 
{
  ++numppoints;
  /* Check whether to reallocate */
  if ( numppoints > (numppointchunks * 256) ) {
    ++numppointchunks;
    ppoints = (PPOINT*) realloc( ppoints, (numppointchunks * 256) * sizeof( PPOINT));
  }
  /* return the current index */
  return numppoints-1;
}

static void createFillPath( void);
static void createStrokePath( double strokewidth, int subpathclosed);
static void createClosedStrokeSubPath( long startind, long stopind,
				       double strokewidth, int subpathclosed);
static long computeDistances( long startind, long stopind, int subpathclosed);
static void transformOnCurvePathPoint( double strokewidth,
				       long prevind, long currind, long lastind);
static void transformOffCurvePathPoint( double strokewidth, long currind);
/* values for flag:
   INTERSECT_PREVIOUS:     only take previous path segment into account.
   INTERSECT_NEXT:         only take next path segment into account. 
   INTERSECT_BOTH:         do a real intersection
*/
#define INTERSECT_PREVIOUS    -1
#define INTERSECT_NEXT         1
#define INTERSECT_BOTH         0
static void intersectRight( long index, double halfwidth, long flag);
/* values for orientation:
   PATH_LEFT:              we are constructing the left path.
   PATH_RIGHT:             we are constructing the right path.
*/
#define PATH_LEFT              1
#define PATH_RIGHT             0
/* values for position:
   PATH_START:             current point starts the current path (use next-values).
   PATH_END:               current point ends the current path (use prev-values).
*/
#define PATH_START             0
#define PATH_END               1
static void linkNode( long index, int position, int orientation);


static long handleNonSubPathSegments( long pindex);
static void handleCurrentSegment( long pindex);
static void adjustBezier( long pindex);

static double  size;
static double scxx, scyx, scxy, scyy;
static double  up;

#ifdef DUMPDEBUGPATH
static FILE*   psfile = NULL;
static void PSDumpProlog( FILE* fp);
static void PSDumpEpilog( FILE* fp);
#endif

/* variables for querying SEAC from external */
static int isseac                = 0;
static unsigned char seacbase    = 0;
static unsigned char seacaccent  = 0;


/**********************************/
/* Type1 Constants and Structures */
/**********************************/
#define MAXSTACK 24        /* Adobe Type1 limit */
#define MAXCALLSTACK 10    /* Adobe Type1 limit */
#define MAXPSFAKESTACK 32  /* Max depth of fake PostScript stack (local) */
#define MAXSTRLEN 512      /* Max length of a Type 1 string (local) */
#define MAXLABEL 256       /* Maximum number of new hints */
#define MAXSTEMS 512       /* Maximum number of VSTEM and HSTEM hints */
#define EPS 0.001          /* Small number for comparisons */

/************************************/
/* Adobe Type 1 CharString commands */
/************************************/
#define HSTEM        1
#define VSTEM        3
#define VMOVETO      4
#define RLINETO      5
#define HLINETO      6
#define VLINETO      7
#define RRCURVETO    8
#define CLOSEPATH    9
#define CALLSUBR    10
#define RETURN      11
#define ESCAPE      12
#define HSBW        13
#define ENDCHAR     14
#define RMOVETO     21
#define HMOVETO     22
#define VHCURVETO   30
#define HVCURVETO   31

/* The following charstring code appears in some old Adobe font files
   in space and .notdef character and does not seems to do anything
   useful aside from taking two args from the stack. We allow this
   command and ignore it. The source code of ghostscript states that
   this command is obsolete *and* undocumented.
   This code may also appear in an Escape-sequence! */
#define UNKNOWN_15  15

/*******************************************/
/* Adobe Type 1 CharString Escape commands */
/*******************************************/
#define DOTSECTION       0
#define VSTEM3           1
#define HSTEM3           2
#define SEAC             6
#define SBW              7
#define DIV             12
#define CALLOTHERSUBR   16
#define POP             17
#define SETCURRENTPOINT 33
 

/* Note: We use routines from libm because in the original macro definitions,
   the evaluation order of tmpx is undefined by C-standard! */
#define USE_MATHLIB_ROUTINES

#ifdef USE_MATHLIB_ROUTINES

#define FABS(x) (fabs (x))
#define CEIL(x) ((LONG) ceil (x))
#define FLOOR(x) ((LONG) floor (x))

#else

/*****************/
/* Useful macros */
/*****************/
static DOUBLE tmpx;  /* Store macro argument in tmpx to avoid re-evaluation */
static LONG tmpi;    /* Store converted value in tmpi to avoid re-evaluation */
#define FABS(x) (((tmpx = (x)) < 0.0) ? -tmpx : tmpx)
#define CEIL(x) (((tmpi = (LONG) (tmpx = (x))) < tmpx) ? ++tmpi : tmpi)
#define FLOOR(x) (((tmpi = (LONG) (tmpx = (x))) > tmpx) ? --tmpi : tmpi)

#endif

#define ROUND(x) FLOOR((x) + 0.5)
#define ODD(x) (((int)(x)) & 01)

#define CC IfTrace1(TRUE, "Char \"%s\": ", currentchar)

/* To make some compiler happy we have to care about return types! */
#define Errori {errflag = TRUE; return 0;}    /* integer */
#define Errord {errflag = TRUE; return 0.0;}  /* double */
#define Errorv {errflag = TRUE; return;}      /* void */
 
#define Error0i(errmsg) { CC; IfTrace0(TRUE, errmsg); Errori;}
#define Error0d(errmsg) { CC; IfTrace0(TRUE, errmsg); Errord;}
#define Error0v(errmsg) { CC; IfTrace0(TRUE, errmsg); Errorv;}
 
#define Error1i(errmsg,arg) { CC; IfTrace1(TRUE, errmsg, arg); Errori;}
#define Error1d(errmsg,arg) { CC; IfTrace1(TRUE, errmsg, arg); Errord;}
#define Error1v(errmsg,arg) { CC; IfTrace1(TRUE, errmsg, arg); Errorv;}
 
/********************/
/* global variables */
/********************/
struct stem {              /* representation of a STEM hint */
  int vertical;                 /* TRUE if vertical, FALSE otherwise */
  DOUBLE x, dx;                 /* interval of vertical stem */
  DOUBLE y, dy;                 /* interval of horizontal stem */
  DOUBLE alx, aldx;             /* interval of grid-aligned vertical stem */
  DOUBLE aly, aldy;             /* interval of grid-aligned horizontal stem */
  double lbhintval;             /* adjustment value for left or bottom hint */
  double rthintval;             /* adjustment value for right ir top hint */
};
 
/******************************************************/
/* Subroutines and statics for the Type1Char routines */
/******************************************************/
 
static int strindex; /* index into PostScript string being interpreted */
static double currx, curry;           /* accumulated x and y values */
static double hcurrx, hcurry;         /* accumulated values with hinting */


struct callstackentry {
  psobj *currstrP;        /* current CharStringP */
  int currindex;          /* current strindex */
  unsigned short currkey; /* current decryption key */
  };
 
static DOUBLE Stack[MAXSTACK];
static int Top;
static struct callstackentry CallStack[MAXCALLSTACK];
static int CallTop;
static DOUBLE PSFakeStack[MAXPSFAKESTACK];
static int PSFakeTop;
 

extern struct XYspace *IDENTITY;
 
static DOUBLE escapementX, escapementY;
static DOUBLE sidebearingX, sidebearingY;
static DOUBLE accentoffsetX, accentoffsetY;
 
static struct segment *path;    /* path of basechar */
static struct segment *apath;   /* pass of accent char */
static int errflag;
 
/*************************************************/
/* Global variables to hold Type1Char parameters */
/*************************************************/
static char *Environment;
static char *currentchar;
static struct XYspace *CharSpace;
static psobj *CharStringP, *SubrsP, *OtherSubrsP;
static int *ModeP;
 
/************************/
/* Forward declarations */
/************************/
static DOUBLE Div();
static DOUBLE PSFakePop();
static int DoCommand();
static int Escape();
static int HStem();
static int VStem();
static int RLineTo();
static int RRCurveTo();
static int DoClosePath();
static int CallSubr();
static int Return();
static int EndChar();
static int RMoveTo();
static int DotSection();
static int Seac();
static int Sbw();
static int CallOtherSubr();
static int SetCurrentPoint();

/******************************************************/
/* statics for Font level hints (Blues) (see blues.h) */
/******************************************************/
static struct blues_struct *blues; /* the blues structure */
static struct alignmentzone alignmentzones[MAXALIGNMENTZONES];
int numalignmentzones;          /* total number of alignment zones */
 
/****************************************************************/
/* Subroutines for the Font level hints (Alignment zones, etc.) */
/****************************************************************/



/* Flags to control the rasterizer */
#define T1_IGNORE_FORCEBOLD           0x0001
#define T1_IGNORE_FAMILYALIGNMENT     0x0002
#define T1_IGNORE_HINTING             0x0004

#define T1_DEBUG_LINE                 0x0100
#define T1_DEBUG_REGION               0x0200
#define T1_DEBUG_PATH                 0x0400
#define T1_DEBUG_FONT                 0x0800
#define T1_DEBUG_HINT                 0x1000

int T1_Type1OperatorFlags; /* for manipulation from t1lib */


static void SetRasterFlags( void)
{

  if (T1_Type1OperatorFlags & T1_IGNORE_HINTING)
    ProcessHints=0;
  else
    ProcessHints=1;
  
  if ( T1_Type1OperatorFlags & T1_DEBUG_LINE)
    LineDebug=1;
  else
    LineDebug=0;
  if ( T1_Type1OperatorFlags & T1_DEBUG_REGION)
    RegionDebug=1;
  else
    RegionDebug=0;
  if ( T1_Type1OperatorFlags & T1_DEBUG_PATH)
    PathDebug=1;
  else
    PathDebug=0;
  if ( T1_Type1OperatorFlags & T1_DEBUG_FONT)
    FontDebug=1;
  else
    FontDebug=0;
  if ( T1_Type1OperatorFlags & T1_DEBUG_HINT)
    HintDebug=1;
  else
    HintDebug=0;
  return;
  
}


/******************************************/
/* Fill in the alignment zone structures. */
/******************************************/
static int ComputeAlignmentZones()
{
  int i;
  DOUBLE dummy, bluezonepixels, familyzonepixels;
  struct segment *p;
 
  numalignmentzones = 0;     /* initialize total # of zones */

  /* Remarks by RMz (Author of t1lib): The handling of substitution of
     the BlueValues by the FamilyBlues and correspondingly for the
     OtherBlues and FamilyOtherBlues is not clearly documented.
     These are the problems:
  
     1) Does the number of FamilyBlues entries need to be identical to
        that of BlueValues?

     2) Obviously, the order of the alignment zones in the BlueValues
        and the FamilyBlues need not be same (see TimesBold.pfa)

     3) Is it wise/recommended to make the substitution on a per
        alignment-zone level or global, i.e., if once then for all
	zones?

     4) The principle found below, checking the delta-height of an
        alignment-zone and making a decision based on this is incorrect.
	The decision has to be done according to absolute pixel values
	at which a feature would be rendered with the BlueValues and the
	FamilyBlues respectively.

     To conclude, it seems better to disable the Family-feature until
     these things are well-known/defined.
     */
  
  /* do the BlueValues zones */
  for (i = 0; i < blues->numBlueValues; i +=2, ++numalignmentzones) {
    /* the 0th & 1st numbers in BlueValues are for a bottom zone */
    /* the rest are topzones */
    if (i == 0)           /* bottom zone */
      alignmentzones[numalignmentzones].topzone = FALSE;
    else                  /* top zone */
      alignmentzones[numalignmentzones].topzone = TRUE;
    /* Check FamilyAlignment suppression */
    if ( (T1_Type1OperatorFlags & T1_IGNORE_FAMILYALIGNMENT)==0) {
      if (i < blues->numFamilyBlues) {    /* we must consider FamilyBlues */
	p = ILoc(CharSpace,0,blues->BlueValues[i] - blues->BlueValues[i+1]);
	QueryLoc(p, IDENTITY, &dummy, &bluezonepixels);
	Destroy(p);
	p = ILoc(CharSpace,0,blues->FamilyBlues[i] - blues->FamilyBlues[i+1]);
	QueryLoc(p, IDENTITY, &dummy, &familyzonepixels);
	Destroy(p);
	/* is the difference in size of the zones less than 1 pixel? */
	if (FABS(bluezonepixels - familyzonepixels) < 1.0) {
	  /* use the Family zones */
	  alignmentzones[numalignmentzones].bottomy =
	    blues->FamilyBlues[i];
	  alignmentzones[numalignmentzones].topy =
	    blues->FamilyBlues[i+1];
#ifdef DUMPDEBUGPATH
	  if ( psfile != NULL ) {
	    if ( alignmentzones[numalignmentzones].topzone == TRUE )
	      fprintf( psfile, "%f %f t1topzone\n", (blues->FamilyBlues[i])*up,
		       (blues->BlueValues[i+1])*up);
	    else
	      fprintf( psfile, "%f %f t1bottomzone\n", (blues->FamilyBlues[i])*up,
		       (blues->BlueValues[i+1])*up);
	  }
#endif
	  continue;
	}
      }
    }
    /* use this font's Blue zones */
    alignmentzones[numalignmentzones].bottomy = blues->BlueValues[i];
    alignmentzones[numalignmentzones].topy = blues->BlueValues[i+1];
#ifdef DUMPDEBUGPATH
    if ( psfile != NULL ) {
      if ( alignmentzones[numalignmentzones].topzone == TRUE )
	fprintf( psfile, "%f %f t1topzone\n", (blues->BlueValues[i])*up,
		 (blues->BlueValues[i+1])*up);
      else
	fprintf( psfile, "%f %f t1bottomzone\n", (blues->BlueValues[i])*up,
		 (blues->BlueValues[i+1])*up);
    }
#endif
  }
 
  /* do the OtherBlues zones */
  for (i = 0; i < blues->numOtherBlues; i +=2, ++numalignmentzones) {
    /* all of the OtherBlues zones are bottom zones */
    alignmentzones[numalignmentzones].topzone = FALSE;
    /* Check FamilyAlignment suppression */
    if ( (T1_Type1OperatorFlags & T1_IGNORE_FAMILYALIGNMENT)==0) {
      if (i < blues->numFamilyOtherBlues) {/* consider FamilyOtherBlues  */
	p = ILoc(CharSpace,0,blues->OtherBlues[i] - blues->OtherBlues[i+1]);
	QueryLoc(p, IDENTITY, &dummy, &bluezonepixels);
	Destroy(p);
	p = ILoc(CharSpace,0,blues->FamilyOtherBlues[i] -
		 blues->FamilyOtherBlues[i+1]);
	QueryLoc(p, IDENTITY, &dummy, &familyzonepixels);
	Destroy(p);
	/* is the difference in size of the zones less than 1 pixel? */
	if (FABS(bluezonepixels - familyzonepixels) < 1.0) {
	  /* use the Family zones */
	  alignmentzones[numalignmentzones].bottomy =
	    blues->FamilyOtherBlues[i];
	  alignmentzones[numalignmentzones].topy =
	    blues->FamilyOtherBlues[i+1];
#ifdef DUMPDEBUGPATH
	  if ( psfile != NULL ) {
	    fprintf( psfile, "%f %f t1bottomzone\n", (blues->FamilyOtherBlues[i])*up,
		     (blues->FamilyOtherBlues[i+1])*up);
	  }
#endif
	  continue;
	}
      }
    }
    /* use this font's Blue zones (as opposed to the Family Blues */
    alignmentzones[numalignmentzones].bottomy = blues->OtherBlues[i];
    alignmentzones[numalignmentzones].topy = blues->OtherBlues[i+1];
#ifdef DUMPDEBUGPATH
    if ( psfile != NULL ) {
      fprintf( psfile, "%f %f t1bottomzone\n", (blues->OtherBlues[i])*up,
	       (blues->OtherBlues[i+1])*up);
    }
#endif
  }
  return(0);
  
}
 
/**********************************************************************/
/* Subroutines and statics for handling of the VSTEM and HSTEM hints. */
/**********************************************************************/
int InDotSection;             /* DotSection flag */
struct stem stems[MAXSTEMS];  /* All STEM hints */
int numstems;                 /* Number of STEM hints */
int currstartstem;            /* The current starting stem. */
int oldvert, oldhor;          /* Remember hint in effect */
int oldhorhalf, oldverthalf;  /* Remember which half of the stem */
DOUBLE wsoffsetX, wsoffsetY;  /* White space offset - for VSTEM3,HSTEM3 */
int wsset;                    /* Flag for whether we've set wsoffsetX,Y */
 
static int InitStems()  /* Initialize the STEM hint data structures */
{
  InDotSection = FALSE;
  currstartstem = numstems = 0;
  oldvert = oldhor = -1;
  return(0);
  
}
 

/*******************************************************************/
/* Compute the dislocation that a stemhint should cause for points */
/* inside the stem.                                                */
/*******************************************************************/
static int ComputeStem(stemno)
int stemno;
{
  int verticalondevice, idealwidth;
  DOUBLE stemstart, stemwidth;
  struct segment *p;
  int i;
  DOUBLE stembottom, stemtop, flatposition;
  DOUBLE Xpixels, Ypixels;
  DOUBLE unitpixels, onepixel;
  int suppressovershoot, enforceovershoot;
  DOUBLE stemshift, flatpospixels, overshoot;
  DOUBLE widthdiff; /* Number of character space units to adjust width */
  DOUBLE lbhintvalue, rthintvalue;
  DOUBLE cxx, cyx, cxy, cyy; /* Transformation matrix */
  int rotated; /* TRUE if character is on the side, FALSE if upright */
 
  /************************************************/
  /* DETERMINE ORIENTATION OF CHARACTER ON DEVICE */
  /************************************************/
 
  QuerySpace(CharSpace, &cxx, &cyx, &cxy, &cyy); /* Transformation matrix */
 
  if (FABS(cxx) < 0.00001 || FABS(cyy) < 0.00001)
    rotated = TRUE; /* Char is on side (90 or 270 degrees), possibly oblique. */
  else if (FABS(cyx) < 0.00001 || FABS(cxy) < 0.00001)
    rotated = FALSE; /* Char is upright (0 or 180 degrees), possibly oblique. */
  else {
    stems[stemno].lbhintval = 0.0; /* Char is at non-axial angle, ignore hints. */
    stems[stemno].rthintval = 0.0;
    ProcessHints = 0;
    return(0);
  }
 
  /* Determine orientation of stem */
 
  if (stems[stemno].vertical) {
    verticalondevice = !rotated;
    stemstart = stems[stemno].x;
    stemwidth = stems[stemno].dx;
#ifdef DUMPDEBUGPATH
    if ( psfile != NULL )
      fprintf( psfile, "%f %f t1vstem\n", stemstart*up, stemwidth*up);
#endif
  } else {
    verticalondevice = rotated;
    stemstart = stems[stemno].y;
    stemwidth = stems[stemno].dy;
#ifdef DUMPDEBUGPATH
    if ( psfile != NULL )
      fprintf( psfile, "%f %f t1hstem\n", stemstart*up, stemwidth*up);
#endif
  }
 
  /* Determine how many pixels (non-negative) correspond to 1 character space
     unit (unitpixels), and how many character space units (non-negative)
     correspond to one pixel (onepixel). */
 
  if (stems[stemno].vertical)
    p = ILoc(CharSpace, 1, 0);
  else
    p = ILoc(CharSpace, 0, 1);
  QueryLoc(p, IDENTITY, &Xpixels, &Ypixels);
  Destroy(p);
  if (verticalondevice)
    unitpixels = FABS(Xpixels);
  else
    unitpixels = FABS(Ypixels);
 
  onepixel = 1.0 / unitpixels;

  /**********************/
  /* ADJUST STEM WIDTHS */
  /**********************/
 
  widthdiff = 0.0;
 
  /* Find standard stem with smallest width difference from this stem */
  if (stems[stemno].vertical) { /* vertical stem */
    if (blues->StdVW != 0)      /* there is an entry for StdVW */
      widthdiff = blues->StdVW - stemwidth;
    for (i = 0; i < blues->numStemSnapV; ++i) { /* now look at StemSnapV */
      if (FABS(blues->StemSnapV[i] - stemwidth) < FABS(widthdiff))
        /* this standard width is the best match so far for this stem */
        widthdiff = blues->StemSnapV[i] - stemwidth;
    }
  } else {                      /* horizontal stem */
    if (blues->StdHW != 0)      /* there is an entry for StdHW */
      widthdiff = blues->StdHW - stemwidth;
    for (i = 0; i < blues->numStemSnapH; ++i) { /* now look at StemSnapH */
      if (FABS(blues->StemSnapH[i] - stemwidth) < FABS(widthdiff))
        /* this standard width is the best match so far for this stem */
        widthdiff = blues->StemSnapH[i] - stemwidth;
    }
  }

  /* Only expand or contract stems if they differ by less than 1 pixel from
     the closest standard width, otherwise make the width difference = 0. */
  if (FABS(widthdiff) > onepixel)
    widthdiff = 0.0;
 
  /* Expand or contract stem to the nearest integral number of pixels. */
  idealwidth = ROUND((stemwidth + widthdiff) * unitpixels);
  /* Ensure that all stems are at least one pixel wide. */
  if (idealwidth == 0)
    idealwidth = 1;

  /* Apply ForceBold to vertical stems. */
  if (blues->ForceBold && stems[stemno].vertical &&
      ((T1_Type1OperatorFlags & T1_IGNORE_FORCEBOLD)==0))
    /* Force this vertical stem to be at least DEFAULTBOLDSTEMWIDTH wide. */
    if (idealwidth < DEFAULTBOLDSTEMWIDTH)
      idealwidth = DEFAULTBOLDSTEMWIDTH;
  /* Now compute the number of character space units necessary */
  widthdiff = idealwidth * onepixel - stemwidth;

  /*********************************************************************/
  /* ALIGNMENT ZONES AND OVERSHOOT SUPPRESSION - HORIZONTAL STEMS ONLY */
  /*********************************************************************/
 
  stemshift = 0.0;
 
  if ( !stems[stemno].vertical ) {
 
    /* Get bottom and top boundaries of the stem. */
    stembottom = stemstart;
    stemtop = stemstart + stemwidth;
 
    /* Find out if this stem intersects an alignment zone (the BlueFuzz  */
    /* entry in the Private dictionary specifies the number of character */
    /* units to extend (in both directions) the effect of an alignment   */
    /* zone on a horizontal stem.  The default value of BlueFuzz is 1.   */
    for (i = 0; i < numalignmentzones; ++i) {
      if (alignmentzones[i].topzone) {
        if (stemtop >= alignmentzones[i].bottomy &&
            stemtop <= alignmentzones[i].topy + blues->BlueFuzz) {
          break; /* We found a top-zone */
        }
      } else {
        if (stembottom <= alignmentzones[i].topy &&
            stembottom >= alignmentzones[i].bottomy - blues->BlueFuzz) {
          break; /* We found a bottom-zone */
        }
      }
    }
 
    if (i < numalignmentzones) { /* We found an intersecting zone (number i). */
      suppressovershoot = FALSE;
      enforceovershoot = FALSE;
 
      /* When 1 character space unit is rendered smaller than BlueScale
         device units (pixels), we must SUPPRESS overshoots.  Otherwise,
         if the top (or bottom) of this stem is more than BlueShift character
         space units away from the flat position, we must ENFORCE overshoot. */
      
      if (unitpixels < blues->BlueScale){
        suppressovershoot = TRUE;
      }
      else{
        if (alignmentzones[i].topzone){
          if (stemtop >= alignmentzones[i].bottomy + blues->BlueShift){
            enforceovershoot = TRUE;
	  }
	}
        else
          if (stembottom <= alignmentzones[i].topy - blues->BlueShift){
            enforceovershoot = TRUE;
	  }
      }
      
      
      /*************************************************/
      /* ALIGN THE FLAT POSITION OF THE ALIGNMENT ZONE */
      /*************************************************/
 
      /* Compute the position of the alignment zone's flat position in
         device space and the amount of shift needed to align it on a
         pixel boundary. Move all stems this amount. */
 
      if (alignmentzones[i].topzone)
        flatposition = alignmentzones[i].bottomy;
      else
        flatposition = alignmentzones[i].topy;
 
      /* Find the flat position in pixels */
      flatpospixels = flatposition * unitpixels;
 
      /* Find the stem shift necessary to align the flat
         position on a pixel boundary, and use this shift for all stems */
      stemshift = (ROUND(flatpospixels) - flatpospixels) * onepixel;
 
      /************************************************/
      /* HANDLE OVERSHOOT ENFORCEMENT AND SUPPRESSION */
      /************************************************/
 
      /* Compute overshoot amount (non-negative) */
      if (alignmentzones[i].topzone)
        overshoot = stemtop - flatposition;
      else
        overshoot = flatposition - stembottom;
 
      if (overshoot > 0.0) {
        /* ENFORCE overshoot by shifting the entire stem (if necessary) so that
           it falls at least one pixel beyond the flat position. */
 
        if (enforceovershoot){
          if (overshoot < onepixel){
            if (alignmentzones[i].topzone)
              stemshift += onepixel - overshoot;
            else
              stemshift -= onepixel - overshoot;
	  }
	}
	
 
        /* SUPPRESS overshoot by aligning the stem to the alignment zone's
           flat position. */
 
        if (suppressovershoot){
          if (alignmentzones[i].topzone)
            stemshift -= overshoot;
          else
            stemshift += overshoot;
	}
      }
 
      /************************************************************/
      /* COMPUTE HINT VALUES FOR EACH SIDE OF THE HORIZONTAL STEM */
      /************************************************************/
 
      /* If the stem was aligned by a topzone, we expand or contract the stem
         only at the bottom - since the stem top was aligned by the zone.
         If the stem was aligned by a bottomzone, we expand or contract the stem
         only at the top - since the stem bottom was aligned by the zone. */
      if (alignmentzones[i].topzone) {
        lbhintvalue = stemshift - widthdiff; /* bottom */
        rthintvalue = stemshift;             /* top    */
      } else {
        lbhintvalue = stemshift;             /* bottom */
        rthintvalue = stemshift + widthdiff; /* top    */
      }

      stems[stemno].lbhintval = lbhintvalue;
      stems[stemno].rthintval = rthintvalue;

      /* store grid-aligned stems values */
      stems[stemno].aly       = stemstart + lbhintvalue;
      stems[stemno].aldy      = stemwidth + widthdiff;
      
#ifdef DUMPDEBUGPATH
      if ( psfile != NULL )
	fprintf( psfile, "%f %f t1alignedhstem\n", (stems[stemno].aly)*up,
		 (stems[stemno].aldy)*up);
#endif
      return(0);
 
    } /* endif (i < numalignmentzones) */
 
    /* We didn't find any alignment zones intersecting this stem, so
       proceed with normal stem alignment below. */
 
  } /* endif (!stems[stemno].vertical) */
 
  /* Align stem with pixel boundaries on device */
  stemstart = stemstart - widthdiff / 2;
  stemshift = ROUND(stemstart * unitpixels) * onepixel - stemstart;
 
  /* Adjust the boundaries of the stem */
  lbhintvalue = stemshift - widthdiff / 2; /* left  or bottom */
  rthintvalue = stemshift + widthdiff / 2; /* right or top    */
 
  if (stems[stemno].vertical) {
    stems[stemno].lbhintval = lbhintvalue;
    stems[stemno].rthintval = rthintvalue;

    /* store grid-aligned stem values */
    stems[stemno].alx       = stemstart + stemshift;
    stems[stemno].aldx      = stemwidth + widthdiff;
      
#ifdef DUMPDEBUGPATH
    if ( psfile != NULL )
      fprintf( psfile, "%f %f t1alignedvstem\n", (stems[stemno].alx)*up,
	       (stems[stemno].aldx)*up);
#endif
  } else {
    stems[stemno].lbhintval = lbhintvalue;
    stems[stemno].rthintval = rthintvalue;

    /* store grid-aligned stem values */
    stems[stemno].aly       = stemstart + stemshift;
    stems[stemno].aldy      = stemwidth + widthdiff;
      
#ifdef DUMPDEBUGPATH
    if ( psfile != NULL )
      fprintf( psfile, "%f %f t1alignedhstem\n", (stems[stemno].aly)*up,
    	       (stems[stemno].aldy)*up);
#endif
  }
  return(0);
  
}
 

#define LEFT   1
#define RIGHT  2
#define BOTTOM 3
#define TOP    4


/***********************************************************************/
/* Find the vertical and horizontal stems that the current point       */
/* (x, y) may be involved in.  At most one horizontal and one vertical */
/* stem can apply to a single point, since there are no overlaps       */
/* allowed.                                                            */
/*   The point list updated by this function.                          */
/* Hints are ignored inside a DotSection.                              */
/***********************************************************************/
static void FindStems( double x, double y,
		       double dx, double dy,
		       double nextdx, double nextdy)
{
  int i;
  int newvert, newhor;
  int newhorhalf, newverthalf;

  /* The following values will be used to decide whether a curve
     crosses or touches a stem in an aligned manner or not */
  double dtana     = 0.0;   /* tangent of pre-delta against horizontal line */ 
  double dtanb     = 0.0;   /* tangent of pre-delta against vertical line */ 
  double nextdtana = 0.0;   /* tangent of post-delta against horizontal line */ 
  double nextdtanb = 0.0;   /* tangent of post-delta against vertical line */ 
  
 
  /* setup default hinted position */
  ppoints[numppoints-1].ax     = ppoints[numppoints-1].x;
  ppoints[numppoints-1].ay     = ppoints[numppoints-1].y;
  if ( ppoints[numppoints-1].hinted == -1 )
    /* point is not to be hinted! */
    return;
  else
    ppoints[numppoints-1].hinted = 0;

  if ( InDotSection )
    return;

  if ( ProcessHints == 0 ) {
    return;
  }

  /* setup (absolute) tangent values and define limits that indicate nearly
     horizontal or nearly vertical alignment */
#define NEARLYVERTICAL     0.2   /* This corresponds to about 11.3 degress deviation */
#define NEARLYHORIZONTAL   0.2   /* from the ideal direction.                        */
  if ( dy != 0 ) {
    dtana = dx/dy;
    if ( dtanb < 0.0 )
      dtana = -dtana;
  }
  else
    dtana = NEARLYHORIZONTAL;
  if ( dx != 0 ) {
    dtanb = dy/dx;
    if ( dtanb < 0.0 )
      dtanb = -dtanb;
  }
  else
    dtanb = NEARLYVERTICAL;
  if ( nextdy != 0 ) {
    nextdtana = nextdx/nextdy;
    if ( nextdtana < 0.0 )
      nextdtana = -nextdtana;
  }
  else
    nextdtana = NEARLYHORIZONTAL;
  if ( nextdx != 0 ) {
    nextdtanb = nextdy/nextdx;
    if ( nextdtanb < 0.0 )
      nextdtanb = -nextdtanb;
  }
  else
    nextdtanb = NEARLYVERTICAL;
  
  newvert = newhor = -1;
  newhorhalf = newverthalf = -1;

  for (i = currstartstem; i < numstems; i++) {
    if (stems[i].vertical) { /* VSTEM hint */
      /* OK, stem is crossed in an aligned way */
      if ( (dtana <= NEARLYVERTICAL) || (nextdtana <= NEARLYVERTICAL)) {
	if ((x >= stems[i].x ) &&
	    (x <= stems[i].x+stems[i].dx )) {
	  newvert = i;
	  if (x < stems[i].x+stems[i].dx / 2)
	    newverthalf = LEFT;
	  else
	    newverthalf = RIGHT;
	}
      }
    }
    else {                 /* HSTEM hint */
      if ( (dtanb <= NEARLYHORIZONTAL) || (nextdtanb <= NEARLYHORIZONTAL)) {
	/* OK, stem is crossed in an aligned way */
	if ((y >= stems[i].y ) &&
	    (y <= stems[i].y+stems[i].dy )) {
	  newhor = i;
	  if (y < stems[i].y+stems[i].dy / 2)
	    newhorhalf = BOTTOM;
	  else
	    newhorhalf = TOP;
	}
      }
    }
  }

  if ( newvert != -1 ) {
    /* mark the latest point in the point list to be v-hinted! */
    if ( newverthalf == LEFT ) {
      /* left hint */
      ppoints[numppoints-1].ax  += stems[newvert].lbhintval;
    }
    else {
       /* right hint */
      ppoints[numppoints-1].ax  += stems[newvert].rthintval;
    }
    ppoints[numppoints-1].hinted |= 0x01;
  }
  if ( newhor != -1 ) {
    /* mark the latest point in the point list to be h-hinted! */
    if ( newhorhalf == BOTTOM ) {
      /* bottom hint */
      ppoints[numppoints-1].ay  += stems[newhor].lbhintval;
    }
    else {
       /* top hint */
      ppoints[numppoints-1].ay  += stems[newhor].rthintval;
    }
    ppoints[numppoints-1].hinted |= 0x02;
  }
  
  return;
  
}


/* Type 1 internal functions */
static int ClearStack()
{
  Top = -1;
  return(0);
  
}
 
static int Push(Num)
        DOUBLE Num;
{
  if (++Top < MAXSTACK) Stack[Top] = Num;
  else Error0i("Push: Stack full\n");
  return(0);
  
}
 
static int ClearCallStack()
{
  CallTop = -1;
  return(0);
}
 
static int PushCall(CurrStrP, CurrIndex, CurrKey)
  psobj *CurrStrP;
  int CurrIndex;
  unsigned short CurrKey;
{
  if (++CallTop < MAXCALLSTACK) {
    CallStack[CallTop].currstrP = CurrStrP;   /* save CharString pointer */
    CallStack[CallTop].currindex = CurrIndex; /* save CharString index */
    CallStack[CallTop].currkey = CurrKey;     /* save decryption key */
  }
  else Error0i("PushCall: Stack full\n");
  return(0);
}
 
static int PopCall(CurrStrPP, CurrIndexP, CurrKeyP)
  psobj **CurrStrPP;
  int *CurrIndexP;
  unsigned short *CurrKeyP;
{
  if (CallTop >= 0) {
    *CurrStrPP = CallStack[CallTop].currstrP; /* restore CharString pointer */
    *CurrIndexP = CallStack[CallTop].currindex; /* restore CharString index */
    *CurrKeyP = CallStack[CallTop--].currkey;   /* restore decryption key */
  }
  else Error0i("PopCall: Stack empty\n");
  return(0);
}

 
static int ClearPSFakeStack()
{
  PSFakeTop = -1;
  return(0);
}
 
/* PSFakePush: Pushes a number onto the fake PostScript stack */
static int PSFakePush(Num)
  DOUBLE Num;
{
  if (++PSFakeTop < MAXPSFAKESTACK) PSFakeStack[PSFakeTop] = Num;
  else Error0i("PSFakePush: Stack full\n");
  return(0);
}
 
/* PSFakePop: Removes a number from the top of the fake PostScript stack */
static DOUBLE PSFakePop ()
{
  if (PSFakeTop >= 0) return(PSFakeStack[PSFakeTop--]);
  
  else Error0d("PSFakePop : Stack empty\n");

  /*NOTREACHED*/
}
 
/***********************************************************************/
/* Center a stem on the pixel grid -- used by HStem3 and VStem3        */
/***********************************************************************/
static struct segment *CenterStem(edge1, edge2)
    DOUBLE edge1;
    DOUBLE edge2;
{
  int idealwidth, verticalondevice;
  DOUBLE leftx, lefty, rightx, righty, center, width;
  DOUBLE widthx, widthy;
  DOUBLE shift, shiftx, shifty;
  DOUBLE Xpixels, Ypixels;
  struct segment *p;
 
  p = Loc(CharSpace, edge1, 0.0);
  QueryLoc(p, IDENTITY, &leftx, &lefty);
 
  p = Join(p, Loc(CharSpace, edge2, 0.0));
  QueryLoc(p, IDENTITY, &rightx, &righty);
  Destroy(p);
 
  widthx = FABS(rightx - leftx);
  widthy = FABS(righty - lefty);
 
  if (widthy <= EPS) { /* verticalondevice hint */
    verticalondevice = TRUE;
    center = (rightx + leftx) / 2.0;
    width = widthx;
  }
  else if (widthx <= EPS) { /* horizontal hint */
    verticalondevice = FALSE;
    center = (righty + lefty) / 2.0;
    width = widthy;
  }
  else { /* neither horizontal nor verticalondevice and not oblique */
    return (NULL);
  }
 
  idealwidth = ROUND(width);
  if (idealwidth == 0) idealwidth = 1;
  if (ODD(idealwidth)) {       /* is ideal width odd? */
    /* center stem over pixel */
    shift = FLOOR(center) + 0.5 - center;
  }
  else {
    /* align stem on pixel boundary */
    shift = ROUND(center) - center;
  }
 
  if (verticalondevice) {
    shiftx = shift;
    shifty = 0.0;
  } else {
    shifty = shift;
    shiftx = 0.0;
  }
 
  p = Loc(IDENTITY, shiftx, shifty);
  QueryLoc(p, CharSpace, &Xpixels, &Ypixels);
  wsoffsetX = Xpixels; wsoffsetY = Ypixels;
  currx += wsoffsetX; curry += wsoffsetY;
 
  return (p);
}
 
/*-----------------------------------------------------------------------
  Decrypt - From Adobe Type 1 book page 63, with some modifications
-----------------------------------------------------------------------*/
#define KEY 4330 /* Initial key (seed) for CharStrings decryption */
#define C1 52845 /* Multiplier for pseudo-random number generator */
#define C2 22719 /* Constant for pseudo-random number generator */
 
static unsigned short r; /* Pseudo-random sequence of keys */
 
static unsigned char Decrypt(cipher)
unsigned char cipher;
{
  unsigned char plain;
 
  plain = cipher ^ (r >> 8);
  r = (cipher + r) * C1 + C2;
  return plain;
}
 
/* Get the next byte from the codestring being interpreted */
static int DoRead(CodeP)
  int *CodeP;
{
  if (strindex >= CharStringP->len) return(FALSE); /* end of string */
  /* We handle the non-documented Adobe convention to use lenIV=-1 to
     suppress charstring encryption. */
  if (blues->lenIV==-1) {
    *CodeP = (unsigned char) CharStringP->data.stringP[strindex++];
  }
  else { 
    *CodeP = Decrypt((unsigned char) CharStringP->data.stringP[strindex++]);
  }
  
  return(TRUE);
}
 
/* Strip blues->lenIV bytes from CharString and update encryption key */
/* (the lenIV entry in the Private dictionary specifies the number of */
/* random bytes at the beginning of each CharString; default is 4)    */
static void StartDecrypt()
{
  int Code;
 
  r = KEY; /* Initial key (seed) for CharStrings decryption */
  for (strindex = 0; strindex < blues->lenIV;){
    if (!DoRead(&Code)) /* Read a byte and update decryption key */
      Error0v("StartDecrypt: Premature end of CharString\n");
  }
  
}

#undef DecodeDebug

static int Decode(Code)
  int Code;
{
  int Code1, Code2, Code3, Code4;
 
  if (Code <= 31){                           /* Code is [0,31]    */
#ifdef DecodeDebug
    fprintf(stderr, "Decode: Code=%d -> Command\n", Code);
#endif
    DoCommand(Code);
  }
  else if (Code <= 246){                     /* Code is [32,246]  */
#ifdef DecodeDebug
    fprintf(stderr, "Decode: Code=%d -> number=%f\n",
	    Code, (DOUBLE)(Code-139));
#endif
    Push((DOUBLE)(Code - 139));
  }
  else if (Code <= 250) {                   /* Code is [247,250] */
    if (!DoRead(&Code2)) goto ended;
#ifdef DecodeDebug
    fprintf(stderr, "Decode: Code=%d next Code=%d -> number=%f\n",
	    Code, Code2, (DOUBLE)(((Code - 247) << 8) + Code2 + 108));
#endif
    Push((DOUBLE)(((Code - 247) << 8) + Code2 + 108));
  }
  else if (Code <= 254) {                   /* Code is [251,254] */
    if (!DoRead(&Code2)) goto ended;
#ifdef DecodeDebug
    fprintf(stderr, "Decode: Code=%d, next Code=%d -> number=%f\n",
	    Code, Code2, (DOUBLE)( -((Code - 251) << 8) - Code2 - 108));
#endif
    Push((DOUBLE)( -((Code - 251) << 8) - Code2 - 108));
  }
  else {                                    /* Code is 255 */
    if (!DoRead(&Code1)) goto ended;
    if (!DoRead(&Code2)) goto ended;
    if (!DoRead(&Code3)) goto ended;
    if (!DoRead(&Code4)) goto ended;
#ifdef DecodeDebug
    fprintf(stderr, "Decode: Code=255, Code1=%d, Code2=%d, Code3=%d, Code4=%d -> number=%f\n",
	    Code1, Code2, Code3, Code4,
	    (DOUBLE)((((((Code1<<8) + Code2)<<8) + Code3)<<8) + Code4));
#endif
    Push((DOUBLE)((((((Code1<<8) + Code2)<<8) + Code3)<<8) + Code4));
  }
  return(0);
 
ended: Error0i("Decode: Premature end of Type 1 CharString");
}

#undef DoCommandDebug 

/* Interpret a command code */
static int DoCommand(Code)
  int Code;
{
  switch(Code) {
    case HSTEM: /* |- y dy HSTEM |- */
#ifdef DoCommandDebug
      printf("DoCommand: HStem\n");
#endif
      /* Vertical range of a horizontal stem zone */
      if (Top < 1) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      HStem(Stack[0], Stack[1]);
      ClearStack();
      break;
    case VSTEM: /* |- x dx VSTEM |- */
#ifdef DoCommandDebug
      printf("DoCommand: VStem\n");
#endif
      /* Horizontal range of a vertical stem zone */
      if (Top < 1) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      VStem(Stack[0], Stack[1]);
      ClearStack();
      break;
    case VMOVETO: /* |- dy VMOVETO |- */
#ifdef DoCommandDebug
      printf("DoCommand: VMoveto\n");
#endif
      /* Vertical MOVETO, equivalent to 0 dy RMOVETO */
      if (Top < 0) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      RMoveTo(0.0, Stack[0]);
      ClearStack();
      break;
    case RLINETO: /* |- dx dy RLINETO |- */
#ifdef DoCommandDebug
      printf("DoCommand: RLineto\n");
#endif
      /* Like RLINETO in PostScript */
      if (Top < 1) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      RLineTo(Stack[0], Stack[1]);
      ClearStack();
      break;
    case HLINETO: /* |- dx HLINETO |- */
#ifdef DoCommandDebug
      printf("DoCommand: HLineto\n");
#endif
      /* Horizontal LINETO, equivalent to dx 0 RLINETO */
      if (Top < 0) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      RLineTo(Stack[0], 0.0);
      ClearStack();
      break;
    case VLINETO: /* |- dy VLINETO |- */
#ifdef DoCommandDebug
      printf("DoCommand: VLineto\n");
#endif
      /* Vertical LINETO, equivalent to 0 dy RLINETO */
      if (Top < 0) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      RLineTo(0.0, Stack[0]);
      ClearStack();
      break;
    case RRCURVETO:
#ifdef DoCommandDebug
      printf("DoCommand: RRCurveto\n");
#endif
      /* |- dx1 dy1 dx2 dy2 dx3 dy3 RRCURVETO |- */
      /* Relative RCURVETO, equivalent to dx1 dy1 */
      /* (dx1+dx2) (dy1+dy2) (dx1+dx2+dx3) */
      /* (dy1+dy2+dy3) RCURVETO in PostScript */
      if (Top < 5) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      RRCurveTo(Stack[0], Stack[1], Stack[2], Stack[3],
        Stack[4], Stack[5]);
      ClearStack();
      break;
    case CLOSEPATH: /* - CLOSEPATH |- */
#ifdef DoCommandDebug
      printf("DoCommand: ClosePath\n");
#endif
      /* Closes a subpath without repositioning the */
      /* current point */
      DoClosePath();
      ClearStack();
      break;
    case CALLSUBR: /* subr# CALLSUBR - */
#ifdef DoCommandDebug
      printf("DoCommand: CallSubr\n");
#endif
      /* Calls a CharString subroutine with index */
      /* subr# from the Subrs array */
      if (Top < 0) Error1i("DoCommand: Stack low\n (Code=%d)", Code);
      CallSubr((int)Stack[Top--]);
      break;
    case RETURN: /* - RETURN - */
#ifdef DoCommandDebug
      printf("DoCommand: Return\n");
#endif
      /* Returns from a Subrs array CharString */
      /* subroutine called with CALLSUBR */
      Return();
      break;
    case ESCAPE: /* ESCAPE to two-byte command code */
#ifdef DoCommandDebug
      printf("DoCommand: Escape to 2 Byte Code (Code=%d)\n", Code);
#endif
      if (!DoRead(&Code)) Error0i("DoCommand: ESCAPE is last byte\n");
      Escape(Code);
      break;
    case HSBW: /* |- sbx wx HSBW |- */
#ifdef DoCommandDebug
      printf("DoCommand: HSBW\n");
#endif
      /* Set the left sidebearing point to (sbx,0), */
      /* set the character width vector to (wx,0). */
      /* Equivalent to sbx 0 wx 0 SBW.  Space */
      /* character should have sbx = 0 */
      if (Top < 1) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      Sbw(Stack[0], 0.0, Stack[1], 0.0);
      ClearStack();
      break;
    case ENDCHAR: /* - ENDCHAR |- */
#ifdef DoCommandDebug
      printf("DoCommand: EndChar\n");
#endif
      /* Finishes a CharString outline */
      EndChar();
      ClearStack();
      break;
    case RMOVETO: /* |- dx dy RMOVETO |- */
#ifdef DoCommandDebug
      printf("DoCommand: RMoveto\n");
#endif
      /* Behaves like RMOVETO in PostScript */
      if (Top < 1) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      RMoveTo(Stack[0], Stack[1]);
      ClearStack();
      break;
    case HMOVETO: /* |- dx HMOVETO |- */
#ifdef DoCommandDebug
      printf("DoCommand: HMovetoUnassigned\n");
#endif
      /* Horizontal MOVETO. Equivalent to dx 0 RMOVETO */
      if (Top < 0) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      RMoveTo(Stack[0], 0.0);
      ClearStack();
      break;
    case VHCURVETO: /* |- dy1 dx2 dy2 dx3 VHCURVETO |- */
#ifdef DoCommandDebug
      printf("DoCommand: VHCurveto\n");
#endif
      /* Vertical-Horizontal CURVETO, equivalent to */
      /* 0 dy1 dx2 dy2 dx3 0 RRCURVETO */
      if (Top < 3) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      RRCurveTo(0.0, Stack[0], Stack[1], Stack[2],
              Stack[3], 0.0);
      ClearStack();
      break;
    case HVCURVETO: /* |- dx1 dx2 dy2 dy3 HVCURVETO |- */
#ifdef DoCommandDebug
      printf("DoCommand: HCurveto\n");
#endif
      /* Horizontal-Vertical CURVETO, equivalent to */
      /* dx1 0 dx2 dy2 0 dy3 RRCURVETO */
      if (Top < 3) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      RRCurveTo(Stack[0], 0.0, Stack[1], Stack[2], 0.0, Stack[3]);
      ClearStack();
      break;
  case UNKNOWN_15:
    if (Top < 1) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
    ClearStack();
    break;
    default: /* Unassigned command code */
#ifdef DoCommandDebug
      printf("DoCommand: Unassigned\n");
#endif
      ClearStack();
      Error1i("DoCommand: Unassigned code %d\n", Code);
  }
  return(0);
  
}
 
static int Escape(Code)
  int Code;
{
  int i, Num;
  struct segment *p;
 
  switch(Code) {
    case DOTSECTION: /* - DOTSECTION |- */
      /* Brackets an outline section for the dots in */
      /* letters such as "i", "j", and "!". */
      DotSection();
      ClearStack();
      break;
    case VSTEM3: /* |- x0 dx0 x1 dx1 x2 dx2 VSTEM3 |- */
      /* Declares the horizontal ranges of three */
      /* vertical stem zones between x0 and x0+dx0, */
      /* x1 and x1+dx1, and x2 and x2+dx2. */
      if (Top < 5) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      if (!wsset && ProcessHints) {
        /* Shift the whole character so that the middle stem is centered. */
        p = CenterStem(Stack[2] + sidebearingX, Stack[3]);
        path = Join(path, p);
        wsset = 1;
      }
 
      VStem(Stack[0], Stack[1]);
      VStem(Stack[2], Stack[3]);
      VStem(Stack[4], Stack[5]);
      ClearStack();
      break;
    case HSTEM3: /* |- y0 dy0 y1 dy1 y2 dy2 HSTEM3 |- */
      /* Declares the vertical ranges of three hori- */
      /* zontal stem zones between y0 and y0+dy0, */
      /* y1 and y1+dy1, and y2 and y2+dy2. */
      if (Top < 5) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      HStem(Stack[0], Stack[1]);
      HStem(Stack[2], Stack[3]);
      HStem(Stack[4], Stack[5]);
      ClearStack();
      break;
    case SEAC: /* |- asb adx ady bchar achar SEAC |- */
      /* Standard Encoding Accented Character. */
      if (Top < 4) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      Seac(Stack[0], Stack[1], Stack[2],
        (unsigned char) Stack[3],
        (unsigned char) Stack[4]);
      ClearStack();
      break;
    case SBW: /* |- sbx sby wx wy SBW |- */
      /* Set the left sidebearing point to (sbx,sby), */
      /* set the character width vector to (wx,wy). */
      if (Top < 3) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      Sbw(Stack[0], Stack[1], Stack[2], Stack[3]);
      ClearStack();
      break;
    case DIV: /* num1 num2 DIV quotient */
      /* Behaves like DIV in the PostScript language */
      if (Top < 1) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      Stack[Top-1] = Div(Stack[Top-1], Stack[Top]);
      Top--;
      break;
    case CALLOTHERSUBR:
      /* arg1 ... argn n othersubr# CALLOTHERSUBR - */
      /* Make calls on the PostScript interpreter */
      if (Top < 1) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      Num = Stack[Top-1];
      if (Top < Num+1) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
      for (i = 0; i < Num; i++) PSFakePush(Stack[Top - i - 2]);
      Top -= Num + 2;
      CallOtherSubr((int)Stack[Top + Num + 2]);
      break;
    case POP: /* - POP number */
      /* Removes a number from the top of the */
      /* PostScript interpreter stack and pushes it */
      /* onto the Type 1 BuildChar operand stack */
      Push(PSFakePop());
      break;
  case SETCURRENTPOINT: /* |- x y SETCURRENTPOINT |- */
    /* Sets the current point to (x,y) in absolute */
    /* character space coordinates without per- */
    /* forming a CharString MOVETO command */
    if (Top < 1) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
    SetCurrentPoint(Stack[0], Stack[1]);
    ClearStack();
    break;
  case UNKNOWN_15:
    if (Top < 1) Error1i("DoCommand: Stack low (Code=%d)\n", Code);
    ClearStack();
    break;
  default: /* Unassigned escape code command */
    ClearStack();
    Error1i("Escape: Unassigned code %d\n", Code);
  }
  return(0);

}
 
/* |- y dy HSTEM |- */
/* Declares the vertical range of a horizontal stem zone */
/* between coordinates y and y + dy */
/* y is relative to the left sidebearing point */
static int HStem(y, dy)
  DOUBLE y, dy;
{
  IfTrace2((FontDebug), "Hstem %f %f\n", y, dy);
  if (ProcessHints) {
    if (numstems >= MAXSTEMS) Error0i("HStem: Too many hints\n");
    if (dy < 0.0) {y += dy; dy = -dy;}
    stems[numstems].vertical = FALSE;
    stems[numstems].x = 0.0;
    stems[numstems].y = sidebearingY + y + wsoffsetY;
    stems[numstems].dx = 0.0;
    stems[numstems].dy = dy;
    ComputeStem(numstems);
    numstems++;
  }
  return(0);
}
 
/* |- x dx VSTEM |- */
/* Declares the horizontal range of a vertical stem zone */
/* between coordinates x and x + dx */
/* x is relative to the left sidebearing point */
static int VStem(x, dx)
  DOUBLE x, dx;
{
  IfTrace2((FontDebug), "Vstem %f %f\n", x, dx);
  if (ProcessHints) {
    if (numstems >= MAXSTEMS) Error0i("VStem: Too many hints\n");
    if (dx < 0.0) {x += dx; dx = -dx;}
    stems[numstems].vertical = TRUE;
    stems[numstems].x = sidebearingX + x + wsoffsetX;
    stems[numstems].y = 0.0;
    stems[numstems].dx = dx;
    stems[numstems].dy = 0.0;
    ComputeStem(numstems);
    numstems++;
  }
  return(0);
}
 
/* |- dx dy RLINETO |- */
/* Behaves like RLINETO in PostScript */
static int RLineTo(dx, dy)
  DOUBLE dx, dy;
{
  long pindex = 0;
  
  /* compute hinting for previous segment! */
  FindStems( currx, curry, currx-ppoints[numppoints-2].x, curry-ppoints[numppoints-2].y, dx, dy);

  /* Allocate a new path point and pre-setup data */
  pindex = nextPPoint();
  ppoints[pindex].x       = currx + dx;
  ppoints[pindex].y       = curry + dy;
  ppoints[pindex].ax      = ppoints[pindex].x;
  ppoints[pindex].ay      = ppoints[pindex].y;
  ppoints[pindex].type    = PPOINT_LINE;
  ppoints[pindex].hinted  = 0;

  /* update ideal position */
  currx                  += dx;
  curry                  += dy;

  return(0);
}
 
/* |- dx1 dy1 dx2 dy2 dx3 dy3 RRCURVETO |- */
/* Relative RCURVETO, equivalent to dx1 dy1 */
/* (dx1+dx2) (dy1+dy2) (dx1+dx2+dx3) */
/* (dy1+dy2+dy3) RCURVETO in PostScript */
static int RRCurveTo(dx1, dy1, dx2, dy2, dx3, dy3)
  DOUBLE dx1, dy1, dx2, dy2, dx3, dy3;
{
  long pindex = 0;
  
  /* compute hinting for previous point! */
  FindStems( currx, curry, currx-ppoints[numppoints-2].x, curry-ppoints[numppoints-2].y, dx1, dy1);

  /* Allocate three new path points and pre-setup data */
  pindex = nextPPoint();
  ppoints[pindex].x       = currx + dx1;
  ppoints[pindex].y       = curry + dy1;
  ppoints[pindex].ax      = ppoints[pindex].x;
  ppoints[pindex].ay      = ppoints[pindex].y;
  ppoints[pindex].type    = PPOINT_BEZIER_B;
  ppoints[pindex].hinted  = 0;

  /* update ideal position */
  currx                  += dx1;
  curry                  += dy1;

  /* compute hinting for previous point! */
  FindStems( currx, curry, currx-ppoints[numppoints-2].x, curry-ppoints[numppoints-2].y, dx2, dy2);

  pindex = nextPPoint();
  ppoints[pindex].x       = currx + dx2;
  ppoints[pindex].y       = curry + dy2;
  ppoints[pindex].ax      = ppoints[pindex].x;
  ppoints[pindex].ay      = ppoints[pindex].y;
  ppoints[pindex].type    = PPOINT_BEZIER_C;
  ppoints[pindex].hinted  = 0;

  /* update ideal position */
  currx                  += dx2;
  curry                  += dy2;

  /* compute hinting for previous point! */
  FindStems( currx, curry, currx-ppoints[numppoints-2].x, curry-ppoints[numppoints-2].y, dx3, dy3);

  pindex = nextPPoint();
  ppoints[pindex].x       = currx + dx3;
  ppoints[pindex].y       = curry + dy3;
  ppoints[pindex].ax      = ppoints[pindex].x;
  ppoints[pindex].ay      = ppoints[pindex].y;
  ppoints[pindex].type    = PPOINT_BEZIER_D;
  ppoints[pindex].hinted  = 0;

  /* update ideal position */
  currx                  += dx3;
  curry                  += dy3;

  return(0);
}
 
/* - CLOSEPATH |- */
/* Closes a subpath WITHOUT repositioning the */
/* current point */
static int DoClosePath()
{
  long pindex = 0;
  long i = 0;
  long tmpind;
  double deltax = 0.0;
  double deltay = 0.0;
  
  /* If this ClosePath command together with the starting point of this
     path completes to a segment aligned to a stem, we would miss
     hinting for this point. --> Check and explicitly care for this! */
  /* 1. Step back in the point list to the last moveto-point */
  i = numppoints - 1;
  while ( (i > 0) && (ppoints[i].type != PPOINT_MOVE ) ) { 
    --i; 
  }
  
  /* 2. Re-hint starting point and hint current point */
  if ( ppoints[i].type == PPOINT_MOVE) {
    deltax = ppoints[i].x - ppoints[numppoints-1].x;
    deltay = ppoints[i].y - ppoints[numppoints-1].y;

    /* save nummppoints and reset to move point */
    tmpind = numppoints;
    numppoints = i + 1;
    
    /* re-hint starting point of current subpath (uses the value of numppoints!) */
    FindStems( ppoints[i].x, ppoints[i].y, deltax, deltay,
	       ppoints[i+1].x-ppoints[i].x, ppoints[i+1].y-ppoints[i].y);

    /* restore numppoints and setup hinting for current point */
    numppoints = tmpind;
    FindStems( currx, curry, currx-ppoints[numppoints-2].x, curry-ppoints[numppoints-2].y,
	       deltax, deltay);
  }
  
  /* Allocate a new path point and pre-setup data */
  pindex = nextPPoint();
  ppoints[pindex].x       = currx;
  ppoints[pindex].y       = curry;
  ppoints[pindex].ax      = ppoints[pindex-1].x;
  ppoints[pindex].ay      = ppoints[pindex-1].y;
  ppoints[pindex].type    = PPOINT_CLOSEPATH;
  ppoints[pindex].hinted  = 0;

  return(0);
}
 
/* subr# CALLSUBR - */
/* Calls a CharString subroutine with index */
/* subr# from the Subrs array */
static int CallSubr(subrno)
  int subrno;
{
  IfTrace2((FontDebug), "CallSubr %d (CallStackSize=%d)\n", subrno, CallTop);
  if ((subrno < 0) || (subrno >= SubrsP->len))
    Error0i("CallSubr: subrno out of range\n");
  PushCall(CharStringP, strindex, r);
  CharStringP = &SubrsP->data.arrayP[subrno];
  StartDecrypt();
  return(0);
}
 
/* - RETURN - */
/* Returns from a Subrs array CharString */
/* subroutine called with CALLSUBR */
static int Return()
{
  IfTrace0((FontDebug), "Return\n");
  PopCall(&CharStringP, &strindex, &r);
  return(0);
}
 
/* - ENDCHAR |- */
/* Finishes a CharString outline */
/* Executes SETCHACHEDEVICE using a bounding box */
/* it computes directly from the character outline */
/* and using the width information acquired from a previous */
/* HSBW or SBW.  It then calls a special version of FILL */
/* or STROKE depending on the value of PaintType in the */
/* font dictionary */
static int EndChar()
{
  long pindex = 0;
  
  IfTrace0((FontDebug), "EndChar\n");
 
  /* There is no need to compute and set bounding box for
     the cache, since XIMAGER does that on the fly. */
 
  /* Allocate a new path point and pre-setup data.
     Note: For this special case, we use the variables that usually
     store hinted coordinates for the escapement of the character.
     It is required in handleCurrentSegment().
  */
  pindex = nextPPoint();
  ppoints[pindex].x       = currx;
  ppoints[pindex].y       = curry;
  ppoints[pindex].ax      = escapementX;
  ppoints[pindex].ay      = escapementY;
  ppoints[pindex].type    = PPOINT_ENDCHAR;
  ppoints[pindex].hinted  = -1;

  return(0);
 
}
 
/* |- dx dy RMOVETO |- */
/* Behaves like RMOVETO in PostScript */
static int RMoveTo(dx,dy)
  DOUBLE dx,dy;
{
  long pindex = 0;

  /* Compute hinting for this path point! */
  if ( numppoints == 1 ) {
    /* Since RMoveTo for this case starts a new path segment
       (flex-constructs have already been handled), the current
       point is hinted here only taking the next point into account,
       but not the previous. Later on, in DoClosePath(), we'll step
       back to this point and the position might be rehinted. */
    FindStems( currx, curry, 0, 0, dx, dy);
  }
  else {
    FindStems( currx, curry, ppoints[numppoints-2].x, ppoints[numppoints-2].y, dx, dy);
  }
  


  /* Allocate a new path point and pre-setup data */
  pindex = nextPPoint();
  ppoints[pindex].x       = currx + dx;
  ppoints[pindex].y       = curry + dy;
  ppoints[pindex].ax      = ppoints[pindex].x;
  ppoints[pindex].ay      = ppoints[pindex].y;
  ppoints[pindex].type    = PPOINT_MOVE;
  ppoints[pindex].hinted  = 0;

  /* update ideal position */
  currx                  += dx;
  curry                  += dy;
  
  return 0;
}
 
/* - DOTSECTION |- */
/* Brackets an outline section for the dots in */
/* letters such as "i", "j", and "!". */
static int DotSection()
{
  IfTrace0((FontDebug), "DotSection\n");
  InDotSection = !InDotSection;
  return(0);
}
 
/* |- asb adx ady bchar achar SEAC |- */
/* Standard Encoding Accented Character. */
static int Seac(asb, adx, ady, bchar, achar)
  DOUBLE asb, adx, ady;
  unsigned char bchar, achar;
{
  int Code;
  long pindex = 0;
  
  isseac      = 1;
  seacaccent  = achar;
  seacbase    = bchar;

  IfTrace4((FontDebug), "SEAC %f %f %f %d ", asb, adx, ady, bchar);
  IfTrace1((FontDebug), "%d\n", achar);
 
  /* Move adx - asb, ady over and up from base char's sbpoint. */
  /* (We use adx - asb to counteract the accents sb shift.) */
  /* The variables accentoffsetX/Y modify sidebearingX/Y in Sbw(). */
  /* Note that these incorporate the base character's sidebearing shift by */
  /* using the current sidebearingX, Y values. */
  accentoffsetX = adx - asb;
  accentoffsetY = ady;

  /* Set path = NULL to avoid complaints from Sbw(). */
  path = NULL;
 
  /* Go find the CharString for the accent's code via an upcall */
  CharStringP = GetType1CharString(Environment, achar);
  if (CharStringP == NULL) {
     Error1i("Invalid accent ('%03o) in SEAC\n", achar);
  }
  StartDecrypt();
 
  ClearStack();
  ClearPSFakeStack();
  ClearCallStack();
 
  for (;;) {
    if (!DoRead(&Code)) break;
    Decode(Code);
    if (errflag) return(0);
  }

  /* Allocate a new path point. Data in this case is not relevant
     in handleSegment(), we merely insert a snap() in order to return
     to origin of the accent char. */
  pindex = nextPPoint();
  ppoints[pindex].x       = accentoffsetX;
  ppoints[pindex].y       = accentoffsetY;
  ppoints[pindex].ax      = accentoffsetX;
  ppoints[pindex].ay      = accentoffsetY;
  ppoints[pindex].type    = PPOINT_SEAC;
  ppoints[pindex].hinted  = 0;

  /* We must reset these to null now. */
  accentoffsetX = accentoffsetY = 0;
 
  /* go find the CharString for the base char's code via an upcall */
  CharStringP = GetType1CharString(Environment, bchar);
  StartDecrypt();
 
  ClearStack();
  ClearPSFakeStack();
  ClearCallStack();
 
  InitStems();
 
  for (;;) {
    if (!DoRead(&Code)) break;
    Decode(Code);
    if (errflag) return(0);
  }

  return(0);
}
 
 
/* |- sbx sby wx wy SBW |- */
/* Set the left sidebearing point to (sbx,sby), */
/* set the character width vector to (wx,wy). */
static int Sbw(sbx, sby, wx, wy)
  DOUBLE sbx, sby, wx, wy;
{
  long pindex = 0;
  
  
  IfTrace4((FontDebug), "SBW %f %f %f %f\n", sbx, sby, wx, wy);
 
  escapementX = wx; /* Character width vector */
  escapementY = wy;
 
  /* Sidebearing values are sbx, sby args, plus accent offset from Seac(). */
  sidebearingX = sbx + accentoffsetX;
  sidebearingY = sby + accentoffsetY;
 
  currx = sidebearingX;
  curry = sidebearingY;
  /*
  path = Join(path, Loc(CharSpace, sidebearingX, sidebearingY));
  if (ProcessHints) {
    hcurrx = sidebearingX;
    hcurry = sidebearingY;
  }
  */
  
  /* Allocate a path point and setup.
     Note: In this special case, we store the char escapement in the members
           ax and ay. They are required in handleCurrentSegment(). Hinting
	   is not required for SBW, anyhow!
  */
  pindex = nextPPoint();
  ppoints[pindex].x       = currx;
  ppoints[pindex].y       = curry;
  ppoints[pindex].ax      = wx;
  ppoints[pindex].ay      = wy;
  ppoints[pindex].type    = PPOINT_SBW;
  ppoints[pindex].hinted  = -1; /* indicate that point is not to be hinted */
 
  return(0);
}
 
 /* num1 num2 DIV quotient */
/* Behaves like DIV in the PostScript language */
static DOUBLE Div(num1, num2)
  DOUBLE num1, num2;
{
  IfTrace2((FontDebug), "Div %f %f\n", num1, num2);
  return(num1 / num2);
}
 
/*
  The following four subroutines (FlxProc, FlxProc1, FlxProc2, and
  HintReplace) are C versions of the OtherSubrs Programs, which were
  were published in the Adobe Type 1 Font Format book.
 
  The Flex outline fragment is described by
    c1: (x0, y0) = c3: (x0, yshrink(y0)) or (xshrink(x0), y0)
     "  (x1, y1) =  "  (x1, yshrink(y1)) or (xshrink(x1), y1)
     "  (x2, y2) - reference point
    c2: (x0, y0) = c4: (x0, yshrink(y0)) or (xshrink(x0), y0)
     "  (x1, y1) =  "  (x1, yshrink(y1)) or (xshrink(x1), y1)
     "  (x2, y2) =  "  (x2, y2), rightmost endpoint
    c3: (x0, y0) - control point, 1st Bezier curve
     "  (x1, y1) - control point,      -"-
     "  (x2, y2) - end point,          -"-
    c4: (x0, y0) - control point, 2nd Bezier curve
     "  (x1, y1) - control point,      -"-
     "  (x2, y2) - end point,          -"-
    ep: (epY, epX) - final endpoint (should be same as c4: (x2, y2))
    idmin - minimum Flex height (1/100 pixel) at which to render curves
*/
 
#define dtransform(dxusr,dyusr,dxdev,dydev) { \
  register struct segment *point = Loc(CharSpace, dxusr, dyusr); \
  QueryLoc(point, IDENTITY, dxdev, dydev); \
  Destroy(point); \
}
 
#define itransform(xdev,ydev,xusr,yusr) { \
  register struct segment *point = Loc(IDENTITY, xdev, ydev); \
  QueryLoc(point, CharSpace, xusr, yusr); \
  Destroy(point); \
}
 
#define transform(xusr,yusr,xdev,ydev) dtransform(xusr,yusr,xdev,ydev)
 
#define PaintType (0)
 
#define xshrink(x) ((x - c4x2) * shrink +c4x2)
#define yshrink(y) ((y - c4y2) * shrink +c4y2)
 
#define PickCoords(flag) \
  if (flag) { /* Pick "shrunk" coordinates */ \
    x0 = c1x0; y0 = c1y0; \
    x1 = c1x1; y1 = c1y1; \
    x2 = c1x2; y2 = c1y2; \
    x3 = c2x0; y3 = c2y0; \
    x4 = c2x1; y4 = c2y1; \
    x5 = c2x2; y5 = c2y2; \
  } else { /* Pick original coordinates */ \
    x0 = c3x0; y0 = c3y0; \
    x1 = c3x1; y1 = c3y1; \
    x2 = c3x2; y2 = c3y2; \
    x3 = c4x0; y3 = c4y0; \
    x4 = c4x1; y4 = c4y1; \
    x5 = c4x2; y5 = c4y2; \
  }
 
/* FlxProc() = OtherSubrs[0]; Main part of Flex          */
/*   Calling sequence: 'idmin epX epY 3 0 callothersubr' */
/*   Computes Flex values, and renders the Flex path,    */
/*   and returns (leaves) ending coordinates on stack    */
static void FlxProc(c1x2, c1y2, c3x0, c3y0, c3x1, c3y1, c3x2, c3y2,
             c4x0, c4y0, c4x1, c4y1, c4x2, c4y2, epY, epX, idmin)
  DOUBLE c1x2, c1y2;
  DOUBLE c3x0, c3y0, c3x1, c3y1, c3x2, c3y2;
  DOUBLE c4x0, c4y0, c4x1, c4y1, c4x2, c4y2;
  DOUBLE epX, epY;
  int idmin;
{
  DOUBLE dmin;
  DOUBLE c1x0, c1y0, c1x1, c1y1;
  DOUBLE c2x0, c2y0, c2x1, c2y1, c2x2, c2y2;
  char yflag;
  DOUBLE x0, y0, x1, y1, x2, y2, x3, y3, x4, y4, x5, y5;
  DOUBLE cxx, cyx, cxy, cyy; /* Transformation matrix */
  int flipXY;
  DOUBLE x, y;
  DOUBLE erosion = 1; /* Device parameter */
    /* Erosion may have different value specified in 'internaldict' */
  DOUBLE shrink;
  DOUBLE dX, dY;
  char erode;
  DOUBLE eShift;
  DOUBLE cx, cy;
  DOUBLE ex, ey;


  /* Our PPOINT list now contains 7 moveto commands which
     are about to be consumed by the Flex mechanism. --> Remove these
     seven elements (their values already reside on the PSFakeStack!)
     and approriately restore the current accumulated position. */
  numppoints -= 7;
  currx = ppoints[numppoints-1].x;
  curry = ppoints[numppoints-1].y;
 
  if (ProcessHints) {
    dmin = TYPE1_ABS(idmin) / 100.0; /* Minimum Flex height in pixels */
 
    c2x2 = c4x2; c2y2 = c4y2; /* Point c2 = c4 */
 
    yflag = FABS(c1y2 - c3y2) > FABS(c1x2 - c3x2); /* Flex horizontal? */
 
    QuerySpace(CharSpace, &cxx, &cyx, &cxy, &cyy); /* Transformation matrix */
 
    if (FABS(cxx) < 0.00001 || FABS(cyy) < 0.00001)
      flipXY = -1; /* Char on side */
    else if (FABS(cyx) < 0.00001 || FABS(cxy) < 0.00001)
      flipXY = 1; /* Char upright */
    else
      flipXY = 0; /* Char at angle */
 
    if (yflag) { /* Flex horizontal */
      if (flipXY == 0 || c3y2 == c4y2) { /* Char at angle or Flex height = 0 */
        PickCoords(FALSE); /* Pick original control points */
      } else {
        shrink = FABS((c1y2 - c4y2) / (c3y2 - c4y2)); /* Slope */
 
        c1x0 = c3x0; c1y0 = yshrink(c3y0);
        c1x1 = c3x1; c1y1 = yshrink(c3y1);
        c2x0 = c4x0; c2y0 = yshrink(c4y0);
        c2x1 = c4x1; c2y1 = yshrink(c4y1);
 
        dtransform(0.0, ROUND(c3y2-c1y2), &x, &y); /* Flex height in pixels */
        dY = FABS((flipXY == 1) ? y : x);
        PickCoords(dY < dmin); /* If Flex small, pick 'shrunk' control points */
 
        if (FABS(y2 - c1y2) > 0.001) { /* Flex 'non-zero'? */
          transform(c1x2, c1y2, &x, &y);
 
          if (flipXY == 1) {
            cx = x; cy = y;
          } else {
            cx = y; cy = x;
          }
 
          dtransform(0.0, ROUND(y2-c1y2), &x, &y);
          dY = (flipXY == 1) ? y : x;
          if (ROUND(dY) != 0)
            dY = ROUND(dY);
          else
            dY = (dY < 0) ? -1 : 1;
 
          erode = PaintType != 2 && erosion >= 0.5;
          if (erode)
            cy -= 0.5;
          ey = cy + dY;
          ey = CEIL(ey) - ey + FLOOR(ey);
          if (erode)
            ey += 0.5;
 
          if (flipXY == 1) {
            itransform(cx, ey, &x, &y);
          } else {
            itransform(ey, cx, &x, &y);
          }
 
          eShift = y - y2;
          y1 += eShift;
          y2 += eShift;
          y3 += eShift;
        }
      }
    } else { /* Flex vertical */
      if (flipXY == 0 || c3x2 == c4x2) { /* Char at angle or Flex height = 0 */
        PickCoords(FALSE); /* Pick original control points */
      } else {
        shrink = FABS((c1x2 - c4x2) / (c3x2 - c4x2)); /* Slope */
 
        c1x0 = xshrink(c3x0); c1y0 = c3y0;
        c1x1 = xshrink(c3x1); c1y1 = c3y1;
        c2x0 = xshrink(c4x0); c2y0 = c4y0;
        c2x1 = xshrink(c4x1); c2y1 = c4y1;
 
        dtransform(ROUND(c3x2 - c1x2), 0.0, &x, &y); /* Flex height in pixels */
        dX = FABS((flipXY == -1) ? y : x);
        PickCoords(dX < dmin); /* If Flex small, pick 'shrunk' control points */
 
        if (FABS(x2 - c1x2) > 0.001) {
          transform(c1x2, c1y2, &x, &y);
          if (flipXY == -1) {
            cx = y; cy = x;
          } else {
            cx = x; cy = y;
          }
 
          dtransform(ROUND(x2-c1x2), 0.0, &x, &y);
          dX = (flipXY == -1) ? y : x;
          if (ROUND(dX) != 0)
            dX = ROUND(dX);
          else
            dX = (dX < 0) ? -1 : 1;
 
          erode = PaintType != 2 && erosion >= 0.5;
          if (erode)
            cx -= 0.5;
          ex = cx + dX;
          ex = CEIL(ex) - ex + FLOOR(ex);
          if (erode)
            ex += 0.5;
 
          if (flipXY == -1) {
            itransform(cy, ex, &x, &y);
          } else {
            itransform(ex, cy, &x, &y);
          }
 
          eShift = x - x2;
          x1 += eShift;
          x2 += eShift;
          x3 += eShift;
        }
      }
    }
 
    if (x2 == x5 || y2 == y5) {
      RLineTo( x5 - currx, y5 - curry); \
    } else {
      RRCurveTo( x0 - currx, y0 - curry,
		 x1 - x0, y1 - y0,
		 x2 - x1,
		 y2 - y1); 
      RRCurveTo( x3 - currx, y3 - curry,
		 x4 - x3, y4 - y3,
		 x5 - x4, y5 - y4); 
    }
  } else { /* ProcessHints is off */
    PickCoords(FALSE); /* Pick original control points */
    RRCurveTo( x0 - currx, y0 - curry,
	       x1 - x0, y1 - y0,
	       x2 - x1,
	       y2 - y1); 
    RRCurveTo( x3 - currx, y3 - curry,
	       x4 - x3, y4 - y3,
	       x5 - x4, y5 - y4); 
  }
 
  PSFakePush(epY);
  PSFakePush(epX);
}
 
/* FlxProc1() = OtherSubrs[1]; Part of Flex            */
/*   Calling sequence: '0 1 callothersubr'             */
/*   Saves and clears path, then restores currentpoint */
static void FlxProc1()
{
  /* Since we are now building the path point list,
     there's nothing to do here! */
  return;
}
 
/* FlxProc2() = OtherSubrs[2]; Part of Flex */
/*   Calling sequence: '0 2 callothersubr'  */
/*   Returns currentpoint on stack          */
static void FlxProc2()
{
  /* Push CurrentPoint on fake PostScript stack */
  PSFakePush( ppoints[numppoints-1].x);
  PSFakePush( ppoints[numppoints-1].y);
}
 
/* HintReplace() = OtherSubrs[3]; Hint Replacement            */
/*   Calling sequence: 'subr# 1 3 callothersubr pop callsubr' */
/*   Reinitializes stem hint structure                        */
static void HintReplace()
{
  /* Effectively retire the current stems, but keep them around for */
  /* revhint use in case we are in a stem when we replace hints. */
  currstartstem = numstems;
 
  /* 'subr#' is left on PostScript stack (for 'pop callsubr') */
}
 
/* arg1 ... argn n othersubr# CALLOTHERSUBR - */
/* Make calls on the PostScript interpreter (or call equivalent C code) */
/* NOTE: The n arguments have been pushed on the fake PostScript stack */
static int CallOtherSubr(othersubrno)
  int othersubrno;
{
  IfTrace1((FontDebug), "CallOtherSubr %d\n", othersubrno);
  
  switch(othersubrno) {
  case 0: /* OtherSubrs[0]; Main part of Flex */
    if (PSFakeTop < 16) Error0i("CallOtherSubr: PSFakeStack low");
    ClearPSFakeStack();
    FlxProc(
	    PSFakeStack[0],  PSFakeStack[1],  PSFakeStack[2],  PSFakeStack[3],
	    PSFakeStack[4],  PSFakeStack[5],  PSFakeStack[6],  PSFakeStack[7],
	    PSFakeStack[8],  PSFakeStack[9],  PSFakeStack[10], PSFakeStack[11],
	    PSFakeStack[12], PSFakeStack[13], PSFakeStack[14], PSFakeStack[15],
	    (int) PSFakeStack[16]
	    );
    break;
  case 1: /* OtherSubrs[1]; Part of Flex */
    FlxProc1();
    break;
  case 2: /* OtherSubrs[2]; Part of Flex */
    FlxProc2();
    break;
  case 3: /* OtherSubrs[3]; Hint Replacement */
    HintReplace();
    break;
  case 12: /* OtherSubrs[12]: Counter Control Hinting */
    /* We do nothing, because later OtherSubrs[13] will
       remove data from the PS-stack */
    break;
  case 13:
    /* We pop all elements from PSFakeStack. They may either have been left
       from previous calls to OtherSubr #12 or have been pushed for this
       call */
    ClearPSFakeStack();
  break;
  default: { /* call OtherSubrs[4] or higher if PostScript is present */
    
  }
  }
  return(0);
}
 
/* |- x y SETCURRENTPOINT |- */
/* Sets the current point to (x,y) in absolute */
/* character space coordinates without per- */
/* forming a CharString MOVETO command */
static int SetCurrentPoint(x, y)
  DOUBLE x, y;
{
  IfTrace2((FontDebug), "SetCurrentPoint %f %f\n", x, y);
 
  currx = x;
  curry = y;
  return(0);
}
 
/* The Type1Char routine for use by PostScript. */
/************************************************/
struct xobject *Type1Char(psfont *env, struct XYspace *S,
			  psobj *charstrP, psobj *subrsP,
			  psobj *osubrsP,
			  struct blues_struct *bluesP,
			  int *modeP, char *charname,
			  float strokewidth,
			  int decodeonly)
{
  int Code;
  long i = 0;
  
  double xp, yp;
#ifdef DUMPDEBUGPATH
  char fnbuf[128];
#endif
  struct segment* p;

  /* Reset SEAC querying variables */
  isseac     = 0;
  seacbase   = 0;
  seacaccent = 0;
  
  /* Reset point list */
  ppoints          = NULL;
  numppoints       = 0;
  numppointchunks  = 0;

  /* disable hinting for stroking */
  if ( strokewidth != 0.0f )
    ProcessHints = 0;
  
  if ( env->fontInfoP[PAINTTYPE].value.data.integer == 1 )
    ProcessHints = 0;

  path    = NULL;
  apath   = NULL;
  errflag = FALSE;

  if ( S == NULL ) {
    S=(struct XYspace *) IDENTITY;
  }
 
  /* Make parameters available to all Type1 routines */
  currentchar=charname;
  Environment = (char *)env;
  CharSpace = S; /* used when creating path elements */
  CharStringP = charstrP;
  SubrsP = subrsP;
  OtherSubrsP = osubrsP;
  ModeP = modeP;
 
  blues = bluesP;

  if ( decodeonly == 0 ) {
    QuerySpace( S, &scxx, &scyx, &scxy, &scyy); /* Transformation matrix */
    p = ILoc( S, 1, 0);
    QueryLoc(p, IDENTITY, &xp, &yp);
    up = FABS(xp);
    
    size = scxx * 1000.0;
#ifdef DUMPDEBUGPATH
    sprintf( fnbuf, "t1dump_%s_%f.eps", charname, size); 
    psfile = fopen( fnbuf, "wb");
    if ( psfile != NULL ) {
      PSDumpProlog( psfile);
      fprintf( psfile, "T1LibDict begin\n\ngsave\n%f t1SetupSize\nt1PreparePage\n", size);
    }
#endif
  }

  /* compute the alignment zones */
  SetRasterFlags();
  ComputeAlignmentZones();
  StartDecrypt();
  ClearStack();
  ClearPSFakeStack();
  ClearCallStack();
  InitStems();

  /* reset variables */
  currx = curry = 0.0;
  hcurrx = hcurry = 0.0;
  escapementX = escapementY = 0;
  sidebearingX = sidebearingY = 0;
  accentoffsetX = accentoffsetY = 0;
  wsoffsetX = wsoffsetY = 0;           /* No shift to preserve whitspace. */
  wsset = 0;                           /* wsoffsetX,Y haven't been set yet. */

  for (;;) {
    if (!DoRead(&Code)) break;
    Decode(Code);
    if (errflag) break;
  }

  if ( decodeonly != 0 ) {
    /* OK, character description is now on stack, finish ... */
    return NULL;
  }

  /* We now have a point list in absolute charspace coordinates. Adjust
     non-hinted points to suppress hinting artifacts and generate path. */
  for ( i=0; i<numppoints; i++ ) {
    if ( ppoints[i].type == PPOINT_BEZIER_D)
      adjustBezier( i);
  }
  /* Create path elements */
#if defined(DUMPDEBUGPATH) & defined(DUMPDEBUGPATHBOTH)
  if ( env->fontInfoP[PAINTTYPE].value.data.integer == 0 ) {
    /* For this special debug case, we generate both a filled and a stroked
       path!. */
      createStrokePath( strokewidth, SUBPATH_CLOSED);
      createFillPath();
  }
  else if ( env->fontInfoP[PAINTTYPE].value.data.integer == 1 ) {
    /* PaintType = 1 indicates stroked font. If strokewidth is 0.0f,
       we stroke using the font's internal strokewidth. Otherwise, the
       user supplied value is used. */
    if ( strokewidth != 0.0f )
      createStrokePath( strokewidth, SUBPATH_OPEN);
    else
      createStrokePath( env->fontInfoP[STROKEWIDTH].value.data.real, SUBPATH_OPEN);
  }
#else
  if ( env->fontInfoP[PAINTTYPE].value.data.integer == 0 ) {
    /* PaintType = 0 indicates filled font. Hence, a strokewidth value
       other than 0.0 indicates the character is to be stroked instead
       of to be filled. */
    if ( strokewidth != 0.0f )
      createStrokePath( strokewidth, SUBPATH_CLOSED);
    else
      createFillPath();
  }
  else if ( env->fontInfoP[PAINTTYPE].value.data.integer == 1 ) {
    /* PaintType = 1 indicates stroked font. If strokewidth is 0.0f,
       we stroke using the font's internal strokewidth. Otherwise, the
       user supplied value is used. */
    if ( strokewidth != 0.0f )
      createStrokePath( strokewidth, SUBPATH_OPEN);
    else
      createStrokePath( env->fontInfoP[STROKEWIDTH].value.data.real, SUBPATH_OPEN);
  }
#endif
  
  /* check and handle accented char */
  if ( apath != NULL ) {
    path = Join( apath, path);
  }
  
  /* Report a possible error: */
  *modeP=errflag;
  
  /* Clean up if an error has occurred */
  if (errflag) {
    if (path != NULL) {
      Destroy(path); /* Reclaim storage */
      path = NULL;   /* Indicate that character could not be built */
    }
  }
 
#ifdef DUMPDEBUGPATH
  if ( psfile != NULL ) {
    PSDumpEpilog( psfile);
    fclose( psfile);
    psfile = 0;
  }
#endif

  /* Cleanup ppoints */
  if ( ppoints != NULL ) {
    free( ppoints);
    ppoints = NULL;
    numppoints = 0;
  }
  
  return((struct xobject *) path);
}
 

/* We add a function to implement underlining in Type1-Routines. */
struct xobject *Type1Line(psfont *env, struct XYspace *S,
			  float line_position,
			  float line_thickness,
			  float line_length,
			  float strokewidth)
{

  /* Reset point list */
  ppoints          = NULL;
  numppoints       = 0;
  numppointchunks  = 0;
  
  path    = NULL;
  apath   = NULL;
  errflag = FALSE;

  
  /* Make parameters available to all Type1 routines */
  Environment = (char *)env;
  CharSpace = S; /* used when creating path elements */

  currx = curry = 0;
  escapementX = escapementY = 0;
  sidebearingX = sidebearingY = 0;
  /* We have to initialize the stems-structures because
     the lineto commands refer to them! */
  SetRasterFlags();
  InitStems();
  
  /* Draw Character */
  Sbw(-1.0*line_length, 0.0, 0.0, 0.0);
  RMoveTo( 0.0,  (double) (line_position+0.5*line_thickness));
  RLineTo( 0.0, (double) -line_thickness);
  RLineTo( (double) line_length, 0.0);
  RLineTo( 0.0, (double) line_thickness);
  DoClosePath();  
  EndChar();

  /* Create path */
  if ( strokewidth != 0.0f )
    createStrokePath( strokewidth, SUBPATH_CLOSED);
  else
    createFillPath();
  
  /* Cleanup ppoints */
  if ( ppoints != NULL ) {
    free( ppoints);
    ppoints = NULL;
  }

  return((struct xobject *)path);
  
}


/* Adjust the points of a given cubic Bezier Spline so that the
   geometric relation of points B and C to A and D remain
   qualitatively the same. This reduces hinting artifacts
   at low resolutions.
*/
static void adjustBezier( long pindex)
{
  double deltax  = 0.0;      /* x distance between point A and D */
  double deltay  = 0.0;      /* y distance between point A and D */
  double adeltax = 0.0;      /* x distance between hinted point A and D */
  double adeltay = 0.0;      /* y distance between hinted point A and D */

  deltax  = ppoints[pindex].x - ppoints[pindex-3].x;
  deltay  = ppoints[pindex].y - ppoints[pindex-3].y;
  adeltax = ppoints[pindex].ax - ppoints[pindex-3].ax;
  adeltay = ppoints[pindex].ay - ppoints[pindex-3].ay;

  if ( deltax == 0 || deltay == 0 )
    return;
  
  ppoints[pindex-1].ax = ppoints[pindex-3].ax +
    (adeltax / deltax * (ppoints[pindex-1].x - ppoints[pindex-3].x));
  ppoints[pindex-1].ay = ppoints[pindex-3].ay +
    (adeltay / deltay * (ppoints[pindex-1].y - ppoints[pindex-3].y));
  ppoints[pindex-2].ax = ppoints[pindex-3].ax +
    (adeltax / deltax * (ppoints[pindex-2].x - ppoints[pindex-3].x));
  ppoints[pindex-2].ay = ppoints[pindex-3].ay +
    (adeltay / deltay * (ppoints[pindex-2].y - ppoints[pindex-3].y));

  return;
  
}



/* This function actually generates path segments. It is called
   after all hinting and adjustments have been performed.
*/
static void handleCurrentSegment( long pindex)
{
  struct segment* B;
  struct segment* C;
  struct segment* D;
  struct segment* tmpseg;
  double dx1;
  double dy1;
  double dx2;
  double dy2;
  double dx3;
  double dy3;
  double adx1;
  double ady1;
  double adx2;
  double ady2;
  double adx3;
  double ady3;
  

  /* handle the different segment types in a switch-statement */
  switch ( ppoints[pindex].type ) {

  case PPOINT_MOVE:
    /* handle a move segment */
    dx1  = ppoints[pindex].x - ppoints[pindex-1].x;
    dy1  = ppoints[pindex].y - ppoints[pindex-1].y;
    adx1 = ppoints[pindex].ax - ppoints[pindex-1].ax;
    ady1 = ppoints[pindex].ay - ppoints[pindex-1].ay;
    
#ifdef DUMPDEBUGPATH
    if ( psfile != NULL )
      fprintf( psfile, "%f %f t1rmoveto %% pindex = %ld\n", dx1*up, dy1*up, pindex);
#endif
    if ( ProcessHints ) {
      IfTrace2((FontDebug), "RMoveTo(h) %f %f\n", adx1, ady1);
      B = Loc(CharSpace, adx1, ady1);
#ifdef DUMPDEBUGPATH
      if ( psfile != NULL ) {
	fprintf( psfile, "%f %f t1hintedrmoveto %% pindex = %ld\n", adx1*up, ady1*up, pindex);
      }
#endif
    }
    else {
      IfTrace2((FontDebug), "RMoveTo %f %f\n", dx1, dy1);
      B = Loc(CharSpace, dx1, dy1);
    }
    path = Join(path, B);
    break;


  case PPOINT_LINE:
    /* handle a line segment */
    dx1  = ppoints[pindex].x - ppoints[pindex-1].x;
    dy1  = ppoints[pindex].y - ppoints[pindex-1].y;
    adx1 = ppoints[pindex].ax - ppoints[pindex-1].ax;
    ady1 = ppoints[pindex].ay - ppoints[pindex-1].ay;
    
#ifdef DUMPDEBUGPATH
    if ( psfile != NULL )
      fprintf( psfile, "%f %f t1rlineto %% pindex = %ld\n", dx1*up, dy1*up, pindex);
#endif
    if ( ProcessHints ) {
      IfTrace2((FontDebug), "RLineTo(h) %f %f\n", adx1, ady1);
      B = Loc(CharSpace, adx1, ady1);
#ifdef DUMPDEBUGPATH
      if ( psfile != NULL ) {
	fprintf( psfile, "%f %f t1hintedrlineto %% pindex = %ld\n", adx1*up, ady1*up, pindex);
      }
#endif
    }
    else {
      IfTrace2((FontDebug), "RLineTo %f %f\n", dx1, dy1);
      B = Loc(CharSpace, dx1, dy1);
    }
    path = Join(path, Line(B));
    break;


  case PPOINT_BEZIER_B:
    /* handle a bezier segment (given by this and the following points) */
    dx1  = ppoints[pindex].x - ppoints[pindex-1].x;
    dy1  = ppoints[pindex].y - ppoints[pindex-1].y;
    adx1 = ppoints[pindex].ax - ppoints[pindex-1].ax;
    ady1 = ppoints[pindex].ay - ppoints[pindex-1].ay;
    dx2  = ppoints[pindex+1].x - ppoints[pindex].x;
    dy2  = ppoints[pindex+1].y - ppoints[pindex].y;
    adx2 = ppoints[pindex+1].ax - ppoints[pindex].ax;
    ady2 = ppoints[pindex+1].ay - ppoints[pindex].ay;
    dx3  = ppoints[pindex+2].x - ppoints[pindex+1].x;
    dy3  = ppoints[pindex+2].y - ppoints[pindex+1].y;
    adx3 = ppoints[pindex+2].ax - ppoints[pindex+1].ax;
    ady3 = ppoints[pindex+2].ay - ppoints[pindex+1].ay;


#ifdef DUMPDEBUGPATH
    if ( psfile != NULL )
      fprintf( psfile, "%f %f %f %f %f %f t1rrcurveto %% pindex = %ld\n",
	       dx1*up, dy1*up,
	       dx2*up, dy2*up,
	       dx3*up, dy3*up,
	       pindex);
#endif
    if (ProcessHints) {
      IfTrace4((FontDebug), "RRCurveTo %f %f %f %f ",
	       adx1, ady1, adx2, ady2);
      IfTrace2((FontDebug), "%f %f\n", adx3, ady3);
      B = Loc(CharSpace, adx1, ady1);
      C = Loc(CharSpace, adx2, ady2);
      D = Loc(CharSpace, adx3, ady3);
#ifdef DUMPDEBUGPATH
      if ( psfile != NULL ) {
	fprintf( psfile, "%f %f %f %f %f %f t1hintedrrcurveto %% pindex = %ld\n",
		 adx1*up, ady1*up,
		 adx2*up, ady2*up,
		 adx3*up, ady3*up,
		 pindex);
      }
#endif
    }
    else {
      IfTrace4((FontDebug), "RRCurveTo %f %f %f %f ",
	       dx1, dy1, dx2, dy2);
      IfTrace2((FontDebug), "%f %f\n", dx3, dy3);
      B = Loc(CharSpace, dx1, dy1);
      C = Loc(CharSpace, dx2, dy2);
      D = Loc(CharSpace, dx3, dy3);
    }
    
    /* Since XIMAGER is not completely relative, */
    /* we need to add up the delta values */
    C = Join(C, (struct segment *)Dup(B));
    D = Join(D, (struct segment *)Dup(C));
    path = Join(path, (struct segment *)Bezier(B, C, D));
    break;


  case PPOINT_SBW:
#ifdef DUMPDEBUGPATH
  if ( psfile != NULL )
    fprintf( psfile, "%f %f %f %f t1sbw %% pindex = %ld\n",
	     ppoints[pindex].x*up, ppoints[pindex].y*up,   /* sidebearings */
	     ppoints[pindex].ax*up, ppoints[pindex].ay*up,  /* escapements  */
	     pindex
	     );
#endif
    path = Join(path, Loc(CharSpace, ppoints[pindex].x, ppoints[pindex].y));
    break;
    
    
  case PPOINT_CLOSEPATH:
    IfTrace0((FontDebug), "DoClosePath\n");
#ifdef DUMPDEBUGPATH
    if ( psfile != NULL ) {
      fprintf( psfile, "t1closepath %% pindex = %ld\n", pindex);
      if ( ProcessHints )
	fprintf( psfile, "t1hintedclosepath %% pindex = %ld\n", pindex);
    }
#endif
    
    tmpseg = Phantom(path);
    path = ClosePath(path);
    path = Join(Snap(path), tmpseg);
    break;
    
    
  case PPOINT_ENDCHAR:
    /* Perform a Closepath just in case the command was left out */
    path = ClosePath(path);
    
    /* Set character width / escapement. It is stored in the vars for
       hinted coordinates. */
    path = Join(Snap(path), Loc(CharSpace, ppoints[pindex].ax, ppoints[pindex].ay));
    
#ifdef DUMPDEBUGPATH
    if ( psfile != NULL )
      fprintf( psfile, "t1FinishPage\ngrestore %% pindex = %ld\n", pindex);
#endif
    break;
    

  case PPOINT_SEAC:
    /* return to origin of accent */
    apath = Snap(path);
    /* reset path to NULL */
    path  = NULL;
    break;
    
    
  default:
    /* We must be at the beginning of the path, that is, there is
       nothing to do! */
    ;
  }

  return;
  
}


#ifdef DUMPDEBUGPATH
static void PSDumpProlog( FILE* fp)
{
#include "t1chardump"
}


static void PSDumpEpilog( FILE* fp)
{
  fputs( "\nend\n", fp);
}

#endif /* #ifdef DUMPDEBUGPATH */



/* Take the accumulated path point list and construct the path that is
   to be filled. */
static void createFillPath( void)
{
  long i;
  
  for ( i=0; i<numppoints; i++ ) {
    handleCurrentSegment( i);
  }
  return;
}


/* Take the accumulated path point list and construct a path so that the
   character appears as a stroked outline, where the virtual pen has a diameter
   of strokewidth (measured in big points [bp]). This function works analogously
   to PostScripts charpath operator. */
static void createStrokePath( double strokewidth, int subpathclosed)
{
  long pindex   = 0;
  long startind = 0;
  long stopind  = 0;


  /* For each subpath in the path list (some sub path is closed!), we construct 
     one path interior and one path exterior to the path under consideration in
     a way, that the resulting thick curve has a thickness of strokewidth. */

#ifdef DEBUG_OUTLINE_SURROUNDING
  fprintf( stderr, "\ncreateStrokePath(strokewidth=%f, subpathclosed=%d): Searching partial paths ...\n\n",
	   strokewidth, subpathclosed);
#endif
  if ( subpathclosed == 0 ) {
    /* We have a stroked font */
    /* loop through all subpaths */
    while ( pindex < numppoints ) {
      /* First, handle any non-subpath commands. */
      if ( handleNonSubPathSegments( pindex) != 0 ) {
#ifdef DEBUG_OUTLINE_SURROUNDING
	fprintf( stderr, "PPoint %ld: (%f,%f), Type=%s\n", pindex,
		 ppoints[pindex].x, ppoints[pindex].y, pptypes[ppoints[pindex].type]);
#endif
	++pindex;
	continue;
      }
      
      if ( ppoints[pindex].type == PPOINT_LINE ||
	   ppoints[pindex].type == PPOINT_BEZIER_B ) {
	if ( ppoints[pindex-1].type == PPOINT_MOVE ) {
	  /* A line or curve segment following a move segment indicates a
	     new subpath. */
	  startind = pindex - 1;
	  while ( (pindex < numppoints) &&
		  (ppoints[pindex].type != PPOINT_CLOSEPATH) &&
		  (ppoints[pindex].type != PPOINT_MOVE) && 
		  (ppoints[pindex].type != PPOINT_ENDCHAR) 
		  ) {
#ifdef DEBUG_OUTLINE_SURROUNDING
	    fprintf( stderr, "PPoint %ld: (%f,%f), Type=%s\n", pindex,
		   ppoints[pindex].x, ppoints[pindex].y, pptypes[ppoints[pindex].type]);
#endif
	    ++pindex;
	  }
	  if ( (ppoints[pindex].type == PPOINT_ENDCHAR) ||  /* for outline fonts */
	       (ppoints[pindex].type == PPOINT_MOVE)          /* for stroked fonts */
	       ) {
#ifdef DEBUG_OUTLINE_SURROUNDING
	    fprintf( stderr, "PPoint %ld: (%f,%f), Type=%s\n", pindex,
		   ppoints[pindex].x, ppoints[pindex].y, pptypes[ppoints[pindex].type]);
#endif
	    stopind = --pindex;
#ifdef DEBUG_OUTLINE_SURROUNDING
	    fprintf( stderr, "Found subpath from index %ld to %ld inclusively\n", startind, stopind);
#endif
	    /* We have found a subpath defined by the path points indexed by
	       the interval from startind to stopind. */
	    createClosedStrokeSubPath( startind, stopind, strokewidth, subpathclosed);
	  }
	}
      }
      ++pindex;
    }
  }
  else {
    /* We have a filled font */
    /* loop through all subpaths */
    while ( pindex < numppoints ) {
      /* First, handle any non-subpath commands. */
      if ( handleNonSubPathSegments( pindex) != 0 ) {
#ifdef DEBUG_OUTLINE_SURROUNDING
	fprintf( stderr, "PPoint %ld: (%f,%f), Type=%s\n", pindex,
	       ppoints[pindex].x, ppoints[pindex].y, pptypes[ppoints[pindex].type]);
#endif
	++pindex;
	continue;
      }
      
      if ( ppoints[pindex].type == PPOINT_LINE ||
	   ppoints[pindex].type == PPOINT_BEZIER_B ) {
	if ( ppoints[pindex-1].type == PPOINT_MOVE ) {
	  /* A line or curve segment following a move segment indicates a
	     new subpath. */
	  startind = --pindex;
	  while ( (pindex < numppoints) &&
		  (ppoints[pindex].type != PPOINT_CLOSEPATH) 
		  ) {
#ifdef DEBUG_OUTLINE_SURROUNDING
	    fprintf( stderr, "PPoint %ld: (%f,%f), Type=%s\n", pindex,
		   ppoints[pindex].x, ppoints[pindex].y, pptypes[ppoints[pindex].type]);
#endif
	    ++pindex;
	  }
	  if ( ppoints[pindex].type == PPOINT_CLOSEPATH ) {
#ifdef DEBUG_OUTLINE_SURROUNDING
	    fprintf( stderr, "PPoint %ld: (%f,%f), Type=%s\n", pindex,
		   ppoints[pindex].x, ppoints[pindex].y, pptypes[ppoints[pindex].type]);
#endif
	    stopind = pindex;
#ifdef DEBUG_OUTLINE_SURROUNDING
	    fprintf( stderr, "Found subpath from index %ld to %ld inclusively\n", startind, stopind);
#endif
	    /* We have found a subpath defined by the path points indexed by
	       the interval from startind to stopind. */
	    createClosedStrokeSubPath( startind, stopind, strokewidth, subpathclosed);
	  }
	}
      }
      ++pindex;
    }
  }
  
  return;
  
}



/* Create two closed paths that surround the the current subpath of the
   charpath in a centered fashion. */
static void createClosedStrokeSubPath( long startind, long stopind,
				       double strokewidth, int subpathclosed)
{
  long i;
  long inext;
  long iprev;
  long ip = 0;
  long in = 0;
  long segstartind;
  long segendind;
  
  long lastind     = 0; /* Index of last point whose location is different from first
			   point. After this point there may follow an explicit line-
			   or curveto to the starting point and also the ClosePath-point
			   may be and usually is identical to the starting point. */

  double dx1;
  double dy1;
  double dx2;
  double dy2;
  double dx3;
  double dy3;
  
  struct segment* B;
  struct segment* C;
  struct segment* D;
  struct segment* tmpseg;

  int type;
  

  /* The ClosePath operator is somewhat problematic, because it adds a point
     to the defining points of a path, without actually having a distance to
     the previous or the next point. This causes problems with the distance
     handling. As a remedy, we check whether ClosePath is located at the first
     point or the last point of the path. In the latter case, ClosePath causes
     an additional line segment. */
  if ( (ppoints[stopind].x == ppoints[startind].x) &&
       (ppoints[stopind].y == ppoints[startind].y)
       ) {
    closepathatfirst = 1;
  }
  else {
    closepathatfirst = 0;
  }
  
#ifdef DEBUG_OUTLINE_SURROUNDING
  if ( closepathatfirst == 1 ) {
    fprintf( stderr, "createClosedStrokeSubPath(): Starting up, ClosePath is at first PPoint (does not cause a line segment) ...\n");
  }
  else {
    fprintf( stderr, "createClosedStrokeSubPath(): Starting up, ClosePath is not at first PPoint (causes a closing line segment) ...\n");
  }
#endif

  
  /* For each path point in the list, we have to define a set of points, to the
     left and to the right of the current curve. The positions of these
     new points depend on the coordinates of the previous path point, the current
     path and the next path point. */

  /* For the computations, the distance from the start and end points of curves
     and lines to the neighbouring points is required. We start by calculating
     these and by filling in the path point entries dist2prev and dist2next for
     the respective points. */
#ifdef DEBUG_OUTLINE_SURROUNDING
  fprintf( stderr, "Computing geometric distances between path points ...\n");
#endif
  lastind = computeDistances( startind, stopind, subpathclosed);
#ifdef DEBUG_OUTLINE_SURROUNDING
  fprintf( stderr, "startind=%ld, lastind=%ld, stopind=%ld\n", startind, lastind, stopind);
#endif


  /********************************************************************************
   ********************************************************************************
   ***** 
   ***** Path point transformation
   ***** 
   ********************************************************************************
   ********************************************************************************/

  /* Next we step through the path points of the current subpath and compute the 
     points' transformations. From these newly computed points,
     the path is constructed. */
#ifdef DEBUG_OUTLINE_SURROUNDING
  fprintf( stderr, "Computing geometric transformation points and resulting intersection points for right path ...\n\n");
#endif
  for ( i=startind; i<=lastind; ) {
    /* Be aware of cycling! */
    if ( i == startind ) {
      iprev = lastind;
      inext = i + 1;
    }
    else if ( i == lastind ) {
      iprev = i - 1;
      inext = startind;
    }
    else {
      iprev = i - 1;
      inext = i + 1;
    }
    
    
    switch ( ppoints[i].type ) {
    case PPOINT_MOVE:
      /* The first segment always is of type PPOINT_MOVE. It is defined by the first,
	 the second and the last point. */
      transformOnCurvePathPoint( strokewidth, iprev, i, inext);

      /* Compute one point which results from prolongating the linked segment and
	 and computing the intersection. The result values are stored in dxres,
	 dyres. */
      if ( subpathclosed == 0 ) {
	/* open subpath --> stroked font */
	if ( i == startind ) {
	  intersectRight( i, 0.5*strokewidth, INTERSECT_NEXT);
	}
	else if ( i == lastind ) {
	  intersectRight( i, 0.5*strokewidth, INTERSECT_PREVIOUS);
	}
	else {
	  intersectRight( i, 0.5*strokewidth, INTERSECT_BOTH);
	}
      }
      else {
	intersectRight( i, 0.5*strokewidth, INTERSECT_BOTH);
      }

#ifdef DEBUG_OUTLINE_SURROUNDING
      fprintf( stderr, "\nCurrent PathPoint: PP %ld (%s): (%f,%f), shape=%s;\n", i, pptypes[ppoints[i].type], 
	       ppoints[i].x, ppoints[i].y, ppshapes[ppoints[i].shape]);
      fprintf( stderr, "    RightPath: prev (%f,%f); next (%f,%f); res (%f,%f)\n",
	       ppoints[i].x+ppoints[i].dxpr, ppoints[i].y+ppoints[i].dypr,
	       ppoints[i].x+ppoints[i].dxnr, ppoints[i].y+ppoints[i].dynr,
	       ppoints[i].x+ppoints[i].dxir, ppoints[i].y+ppoints[i].dyir);
      fprintf( stderr, "    LeftPath:  prev (%f,%f); next (%f,%f); res (%f,%f)\n\n",
	       ppoints[i].x-ppoints[i].dxpr, ppoints[i].y-ppoints[i].dypr,
	       ppoints[i].x-ppoints[i].dxnr, ppoints[i].y-ppoints[i].dynr,
	       ppoints[i].x-ppoints[i].dxir, ppoints[i].y-ppoints[i].dyir);
#endif
      
      break;

      
    case PPOINT_LINE:
      transformOnCurvePathPoint( strokewidth, iprev, i, inext);
      if ( subpathclosed == 0 ) {
	/* open subpath --> stroked font */
	if ( i == startind )
	  intersectRight( i, 0.5*strokewidth, INTERSECT_NEXT);
	else if ( i == lastind )
	  intersectRight( i, 0.5*strokewidth, INTERSECT_PREVIOUS);
	else
	  intersectRight( i, 0.5*strokewidth, INTERSECT_BOTH);
      }
      else {
	intersectRight( i, 0.5*strokewidth, INTERSECT_BOTH);
      }
      
#ifdef DEBUG_OUTLINE_SURROUNDING
      fprintf( stderr, "\nCurrent PathPoint: PP %ld (%s): (%f,%f), shape=%s;\n", i, pptypes[ppoints[i].type], 
	       ppoints[i].x, ppoints[i].y, ppshapes[ppoints[i].shape]);
      fprintf( stderr, "    RightPath: prev (%f,%f); next (%f,%f); res (%f,%f)\n",
	       ppoints[i].x+ppoints[i].dxpr, ppoints[i].y+ppoints[i].dypr,
	       ppoints[i].x+ppoints[i].dxnr, ppoints[i].y+ppoints[i].dynr,
	       ppoints[i].x+ppoints[i].dxir, ppoints[i].y+ppoints[i].dyir);
      fprintf( stderr, "    LeftPath:  prev (%f,%f); next (%f,%f); res (%f,%f)\n\n",
	       ppoints[i].x-ppoints[i].dxpr, ppoints[i].y-ppoints[i].dypr,
	       ppoints[i].x-ppoints[i].dxnr, ppoints[i].y-ppoints[i].dynr,
	       ppoints[i].x-ppoints[i].dxir, ppoints[i].y-ppoints[i].dyir);
#endif

      break;

      
    case PPOINT_BEZIER_B:
      break;

    case PPOINT_BEZIER_C:
      break;

    case PPOINT_BEZIER_D:
      transformOnCurvePathPoint( strokewidth, iprev, i, inext);
      if ( subpathclosed == 0 ) {
	/* open subpath --> stroked font */
	if ( i == startind )
	  intersectRight( i, 0.5*strokewidth, INTERSECT_NEXT);
	else if ( i == lastind )
	  intersectRight( i, 0.5*strokewidth, INTERSECT_PREVIOUS);
	else
	  intersectRight( i, 0.5*strokewidth, INTERSECT_BOTH);
      }
      else {
	intersectRight( i, 0.5*strokewidth, INTERSECT_BOTH);
      }
      
#ifdef DEBUG_OUTLINE_SURROUNDING
      fprintf( stderr, "\nCurrent PathPoint: PP %ld (%s): (%f,%f), shape=%s;\n", i, pptypes[ppoints[i].type], 
	       ppoints[i].x, ppoints[i].y, ppshapes[ppoints[i].shape]);
      fprintf( stderr, "    RightPath: prev (%f,%f); next (%f,%f); res (%f,%f)\n",
	       ppoints[i].x+ppoints[i].dxpr, ppoints[i].y+ppoints[i].dypr,
	       ppoints[i].x+ppoints[i].dxnr, ppoints[i].y+ppoints[i].dynr,
	       ppoints[i].x+ppoints[i].dxir, ppoints[i].y+ppoints[i].dyir);
      fprintf( stderr, "    LeftPath:  prev (%f,%f); next (%f,%f); res (%f,%f)\n\n",
	       ppoints[i].x-ppoints[i].dxpr, ppoints[i].y-ppoints[i].dypr,
	       ppoints[i].x-ppoints[i].dxnr, ppoints[i].y-ppoints[i].dynr,
	       ppoints[i].x-ppoints[i].dxir, ppoints[i].y-ppoints[i].dyir);
#endif
      
      /* transform the preceding two offCurve points */
      transformOffCurvePathPoint( strokewidth, i-2);

      break;
      
    case PPOINT_CLOSEPATH:

      break;
      
    default:
      break;
    }
    ++i;
  }

  /* copy the shift values from starting point to ending points that
     have not yet been handled */
  for ( ; i<=stopind; i++) {
    ppoints[i].dxpr      = ppoints[startind].dxpr;
    ppoints[i].dypr      = ppoints[startind].dypr;
    ppoints[i].dxnr      = ppoints[startind].dxnr;
    ppoints[i].dynr      = ppoints[startind].dynr;
    ppoints[i].dxir      = ppoints[startind].dxir;
    ppoints[i].dyir      = ppoints[startind].dyir;
    ppoints[i].dist2prev = ppoints[startind].dist2prev;
    ppoints[i].dist2next = ppoints[startind].dist2next;
    if ( ppoints[i].type == PPOINT_BEZIER_D ) {
      transformOffCurvePathPoint( strokewidth, i-2);
    }
    ppoints[i].shape     = ppoints[startind].shape;
  }
  

#ifdef DEBUG_OUTLINE_SURROUNDING
	fprintf( stderr, "\nTransformation of PathPoints finished. Starting to construct paths ...\n\n");
#endif
  
  /* We now have computed the resulting shift values for each path point of the current
     subpath's right path. The values for the left path follow by negation.
     The path is still to be build!
  */

  /********************************************************************************
   ********************************************************************************
   ***** 
   ***** Construction of right path
   ***** 
   ********************************************************************************
   ********************************************************************************/

  /* The leading move segment is treated separately. First check from which
     point the leading Moveto was called. This is safe even in cases where
     multiple moveto appear in a series. */
  i = startind;
  while ( ppoints[i].type == PPOINT_MOVE )
    --i;
  dx1  = ppoints[startind].x - (ppoints[i].x);
  dy1  = ppoints[startind].y - (ppoints[i].y);
  /* If the first node in the subpath is not concave, we may directly jump
     to the intersection right path point. Otherwise, we remain at the onCurve
     point because later, prolongation will happen. */
  if ( ppoints[startind].shape != CURVE_CONCAVE ) {
    dx1  += ppoints[startind].dxir;
    dy1  += ppoints[startind].dyir;
  }
  
#ifdef DUMPDEBUGPATH
  if ( psfile != NULL )
    fprintf( psfile, "%f %f t1srmoveto %% pindex = %ld\n", dx1*up, dy1*up, startind);
#endif
  B = Loc(CharSpace, dx1, dy1);
  path = Join(path, B);
  
  
  /* Now, handle the remaining path in a loop */
  for ( i=startind+1; i<=stopind; ) {
    switch ( ppoints[i].type ) {
    case PPOINT_LINE:
      /* handle a line segment */
      
      /* 1. Check and handle starting node */
      linkNode( i-1, PATH_START, PATH_RIGHT);

      /* 2. Draw ideal isolated line segment */
#ifdef DEBUG_OUTLINE_SURROUNDING
	fprintf( stderr, "RP:  Line from point %ld to %ld\n", i-1, i);
#endif
      dx1  = ppoints[i].x + ppoints[i].dxpr - (ppoints[i-1].x + ppoints[i-1].dxnr);
      dy1  = ppoints[i].y + ppoints[i].dypr - (ppoints[i-1].y + ppoints[i-1].dynr);
#ifdef DUMPDEBUGPATH
      if ( psfile != NULL )
	fprintf( psfile, "%f %f t1srlineto %% pindex = %ld\n", dx1*up, dy1*up, i);
#endif
      B = Loc(CharSpace, dx1, dy1);
      path = Join(path, Line(B));

      /* 3. Check and handle ending node */
      linkNode( i, PATH_END, PATH_RIGHT);

      break;

    case PPOINT_BEZIER_B:
      break;
    case PPOINT_BEZIER_C:
      break;
    case PPOINT_BEZIER_D:
      /* handle a bezier segment (given by this and the previous 3 path points)! */

      /* 1. Check and handle starting node */
      linkNode( i-3, PATH_START, PATH_RIGHT);

      /* 2. Draw curve based on ideal point locations */
#ifdef DEBUG_OUTLINE_SURROUNDING
      fprintf( stderr, "RP:  Curve from PP %ld to PP %ld to PP %ld to PP %ld\n",
	       i-3, i-2, i-1, i);
#endif
      dx1  = ppoints[i-2].x + ppoints[i-2].dxir - (ppoints[i-3].x + ppoints[i-3].dxnr);
      dy1  = ppoints[i-2].y + ppoints[i-2].dyir - (ppoints[i-3].y + ppoints[i-3].dynr);
      dx2  = ppoints[i-1].x + ppoints[i-1].dxir - (ppoints[i-2].x + ppoints[i-2].dxir);
      dy2  = ppoints[i-1].y + ppoints[i-1].dyir - (ppoints[i-2].y + ppoints[i-2].dyir);
      dx3  = ppoints[i].x +   ppoints[i].dxpr   - (ppoints[i-1].x + ppoints[i-1].dxir);
      dy3  = ppoints[i].y +   ppoints[i].dypr   - (ppoints[i-1].y + ppoints[i-1].dyir);
      
#ifdef DUMPDEBUGPATH
      if ( psfile != NULL )
	fprintf( psfile, "%f %f %f %f %f %f t1srrcurveto %% pindex = %ld\n",
		 dx1*up, dy1*up,
		 dx2*up, dy2*up,
		 dx3*up, dy3*up,
		 i);
#endif
      IfTrace4((FontDebug), "RRCurveTo %f %f %f %f ",
	       dx1, dy1, dx2, dy2);
      IfTrace2((FontDebug), "%f %f\n", dx3, dy3);
      B = Loc(CharSpace, dx1, dy1);
      C = Loc(CharSpace, dx2, dy2);
      D = Loc(CharSpace, dx3, dy3);
    
      C = Join(C, (struct segment *)Dup(B));
      D = Join(D, (struct segment *)Dup(C));
      path = Join(path, (struct segment *)Bezier(B, C, D));

      /* 3. Check and handle starting node */
      linkNode( i, PATH_END, PATH_RIGHT);

      break;

      
    case PPOINT_CLOSEPATH:
#ifdef DEBUG_OUTLINE_SURROUNDING
      fprintf( stderr, "RP:  ClosePath command ignored\n");
#endif
      
      break;

    default:
      break;
      
    }
    ++i;
  }

  /********************************************************************************
   ********************************************************************************
   ***** 
   ***** Close right path
   ***** 
   ********************************************************************************
   ********************************************************************************/

  if ( subpathclosed != 0 ) {
    /* We are stroking an outline font to be filled */ 
    if ( closepathatfirst == 0 ) {
      /* Because of the concavity issue, we may not simply use
	 the closepath operator here. Instead we have to manage a possible
	 prolongation manually if the closepath would cause a line segment. */

      /* 1. Check and handle starting node */
      linkNode( lastind, PATH_START, PATH_RIGHT);

      /* 2. Draw ideal isolated line segment */
      dx1  = ppoints[startind].x + ppoints[startind].dxpr - (ppoints[lastind].x + ppoints[lastind].dxnr);
      dy1  = ppoints[startind].y + ppoints[startind].dypr - (ppoints[lastind].y + ppoints[lastind].dynr);
#ifdef DUMPDEBUGPATH
      if ( psfile != NULL )
	fprintf( psfile, "%f %f t1srlineto %% pindex = %ld\n", dx1*up, dy1*up, startind);
#endif
      B = Loc(CharSpace, dx1, dy1);
      path = Join(path, Line(B));

      /* 3. Check and handle ending node */
      linkNode( startind, PATH_END, PATH_RIGHT);

    } /* if ( closepathatfirst == 0) */

    /* Now close path formally. Anyhow, this won't cause a line segment! */
#ifdef DUMPDEBUGPATH
    if ( psfile != NULL ) {
      fprintf( psfile, "t1sclosepath %% Right Path finished, stepping to Left Path\n");
    }
#endif
    tmpseg = Phantom(path);
    path = ClosePath(path);
    path = Join(Snap(path), tmpseg);

    
    /********************************************************************************
     ********************************************************************************
     ***** 
     ***** Stepping to beginning of left path
     ***** 
     ********************************************************************************
     ********************************************************************************/
    
    /* If curve is concave at the subpath's starting point, the location is onCurve
       and the left path is convex, there. Conversely, if the curve is convex, the
       location is at the right intersection point and the left path will be concave
       so that the initial location must be onCurve. Hence, for both cases, we have
       to translate back once the intersection shift.

       If the curve is straight at the starting point, we directly jump from the right
       intersection point ot he left intersection point.
    */
    if ( (ppoints[startind].shape == CURVE_CONCAVE) ||
	 (ppoints[startind].shape == CURVE_CONVEX) ) { 
      dx1 = - ppoints[startind].dxir;
      dy1 = - ppoints[startind].dyir;
    }
    else {
      dx1 = - 2.0 * ppoints[startind].dxir;
      dy1 = - 2.0 * ppoints[startind].dyir;
    }

#ifdef DUMPDEBUGPATH
    if ( psfile != NULL )
      fprintf( psfile, "%f %f t1srmoveto %% pindex = %ld\n", dx1*up, dy1*up, startind);
#endif
    B = Loc(CharSpace, dx1, dy1);
    path = Join(path, B);
  } /* if ( subpathclose != 0 */
  else {
    /* We have a stroked font. In this case, a line segment has to be drawn */
    if ( (ppoints[stopind].shape == CURVE_CONCAVE) ||
	 (ppoints[stopind].shape == CURVE_CONVEX) ) { 
      dx1 = - ppoints[stopind].dxir;
      dy1 = - ppoints[stopind].dyir;
    }
    else {
      dx1 = - 2.0 * ppoints[stopind].dxir;
      dy1 = - 2.0 * ppoints[stopind].dyir;
    }

#ifdef DUMPDEBUGPATH
    if ( psfile != NULL )
      fprintf( psfile, "%f %f t1srlineto %% pindex = %ld\n", dx1*up, dy1*up, stopind);
#endif
    B = Loc(CharSpace, dx1, dy1);
    path = Join(path, Line(B));
    
  }
  
  
  /********************************************************************************
   ********************************************************************************
   ***** 
   ***** Construction of left path
   ***** 
   ********************************************************************************
   ********************************************************************************/
  
  /* Create left path. This is somewhat more complicated, because the
     order/direction has to be exchanged. */
#ifdef DEBUG_OUTLINE_SURROUNDING
  fprintf( stderr, "Constructing LeftPath: stopind=%ld, lastind=%ld, closepathatfirst=%d\n",
	   stopind, lastind, closepathatfirst);
#endif
  for ( i=stopind; i>=startind; ) {
    if ( subpathclosed != 0 ) {
      /* closed subpath --> filled font */
      if ( i == stopind ) {
	ip   = startind;
	if ( (closepathatfirst != 0) )
	  type = ppoints[ip].type;
	else
	  type = PPOINT_NONE;
      }
      else if ( i == startind ) {
	ip   = startind + 1;
	type = ppoints[ip].type;
      }
      else {
	ip   = i + 1;
	type = ppoints[ip].type;
      }
    }
    else {
      /* open subpath --> stroked font */
      type   = ppoints[i].type;
      in     = i - 1;
    }

    /* Step through path in inverted direction.
       Note: - ip is the index of the starting point, i the index of the
               ending point of the current segment.
             - If the path point is flagged "concave", then this reverts into
               "convex" in the left path and vice versa!
	     - there is an index shift of 1 between closed and open subpaths.
    */
    switch ( type ) {
    case PPOINT_MOVE:
      
      break;
      
    case PPOINT_LINE:

      /* handle a line segment */
      if ( subpathclosed != 0 ) {
	segendind    = i;
	segstartind  = ip;
      }
      else {
	segstartind  = i;
	segendind    = in;
      }
      
      /* 1. Check and handle starting node */
      linkNode( segstartind, PATH_START, PATH_LEFT);

      /* 2. Draw ideal isolated line segment */
#ifdef DEBUG_OUTLINE_SURROUNDING
      fprintf( stderr, "LP:  Line from point %ld to %ld\n", segstartind, segendind);
#endif
      dx1  = ppoints[segendind].x - ppoints[segendind].dxnr -
	(ppoints[segstartind].x - ppoints[segstartind].dxpr);
      dy1  = ppoints[segendind].y - ppoints[segendind].dynr -
	(ppoints[segstartind].y - ppoints[segstartind].dypr);
#ifdef DUMPDEBUGPATH
      if ( psfile != NULL )
	fprintf( psfile, "%f %f t1srlineto %% pindex = %ld\n", dx1*up, dy1*up, segendind);
#endif
      B = Loc(CharSpace, dx1, dy1);
      path = Join(path, Line(B));

      /* 3. Check and handle ending node */
      linkNode( segendind, PATH_END, PATH_LEFT);

      break;

      
    case PPOINT_BEZIER_B:
      break;

    case PPOINT_BEZIER_C:
      break;

    case PPOINT_BEZIER_D:
      /* handle a bezier segment (given by this and the previous 3 path points)!
	 For bezier segments, we may not simply apply the intersection of previous
	 and next candidate because that would damage the curve's layout. Instead,
	 in cases where the candidate produced by intersection is not identical to
	 the ideal point, we prolongate and link the distance with a line segment.
      */

      /* 1. Check and handle starting node */
      linkNode( ip, PATH_START, PATH_LEFT);

      /* 2. Draw ideal curve segment */
#ifdef DEBUG_OUTLINE_SURROUNDING
      fprintf( stderr, "LP:  Curve from PP %ld to PP %ld to PP %ld to PP %ld\n",
	       ip, ip-1, ip-2, ip-3);
#endif
      /* Use ideal point locations for curve at starting and ending point: */
      dx1  = ppoints[ip-1].x - ppoints[ip-1].dxir - (ppoints[ip].x   - ppoints[ip].dxpr);
      dy1  = ppoints[ip-1].y - ppoints[ip-1].dyir - (ppoints[ip].y   - ppoints[ip].dypr);
      dx2  = ppoints[ip-2].x - ppoints[ip-2].dxir - (ppoints[ip-1].x - ppoints[ip-1].dxir);
      dy2  = ppoints[ip-2].y - ppoints[ip-2].dyir - (ppoints[ip-1].y - ppoints[ip-1].dyir);
      dx3  = ppoints[ip-3].x - ppoints[ip-3].dxnr - (ppoints[ip-2].x - ppoints[ip-2].dxir);
      dy3  = ppoints[ip-3].y - ppoints[ip-3].dynr - (ppoints[ip-2].y - ppoints[ip-2].dyir);

#ifdef DUMPDEBUGPATH
      if ( psfile != NULL )
	fprintf( psfile, "%f %f %f %f %f %f t1srrcurveto %% pindex = %ld\n",
		 dx1*up, dy1*up,
		 dx2*up, dy2*up,
		 dx3*up, dy3*up,
		 i);
#endif
      IfTrace4((FontDebug), "RRCurveTo %f %f %f %f ",
	       dx1, dy1, dx2, dy2);
      IfTrace2((FontDebug), "%f %f\n", dx3, dy3);
      B = Loc(CharSpace, dx1, dy1);
      C = Loc(CharSpace, dx2, dy2);
      D = Loc(CharSpace, dx3, dy3);
    
      C = Join(C, (struct segment *)Dup(B));
      D = Join(D, (struct segment *)Dup(C));
      path = Join(path, (struct segment *)Bezier(B, C, D));

      /* 3. Check and handle ending node */
      linkNode( ip-3, PATH_END, PATH_LEFT);

      break;

      
    case PPOINT_CLOSEPATH:
      
      /* Handle a ClosePath segment, if it had
	 caused a line segment. Hence, actually, we handle
	 a line segment here. */
      if ( closepathatfirst == 1 ) {
	/* ignore this command */
	break;
      }

      /* 1. Check and handle starting node */
      linkNode( startind, PATH_START, PATH_LEFT);

      /* 2. Draw ideal isolated line segment */
#ifdef DEBUG_OUTLINE_SURROUNDING
      fprintf( stderr, "LP:  Inverted ClosePath from point %ld to %ld\n", startind, lastind);
#endif
      if ( subpathclosed != 0 ) {
	dx1  = ppoints[lastind].x - ppoints[lastind].dxnr - (ppoints[startind].x - ppoints[startind].dxpr);
	dy1  = ppoints[lastind].y - ppoints[lastind].dynr - (ppoints[startind].y - ppoints[startind].dypr);
      }
      else {
	dx1  = -(ppoints[i].x - ppoints[i].dxnr - (ppoints[ip].x - ppoints[ip].dxpr));
	dy1  = -(ppoints[i].y - ppoints[i].dynr - (ppoints[ip].y - ppoints[ip].dypr));
      }
      
#ifdef DUMPDEBUGPATH
      if ( psfile != NULL ) {
	if ( subpathclosed != 0 ) {
	  fprintf( psfile, "%f %f t1srlineto %% (Inverted ClosePath, subpathclosed=1) pindex = %ld\n",
		   dx1*up, dy1*up, lastind);
	}
	else {
	  fprintf( psfile, "%f %f t1srlineto %% (Inverted ClosePath, subpathclosed=0) pindex = %ld\n",
		   dx1*up, dy1*up, i);
	}
      }
#endif
      B = Loc(CharSpace, dx1, dy1);
      path = Join(path, Line(B));

      /* 3. Check and handle ending node */
      linkNode( lastind, PATH_END, PATH_LEFT);
      
      break;
      
    default:
      break;
      
    }
    --i;
  }

#ifdef DUMPDEBUGPATH
  if ( psfile != NULL ) {
    fprintf( psfile, "t1sclosepath\n");
  }
#endif
  tmpseg = Phantom(path);
  path = ClosePath(path);
  path = Join(Snap(path), tmpseg);
  
  
  /********************************************************************************
   ********************************************************************************
   ***** 
   ***** Move to final position
   ***** 
   ********************************************************************************
   ********************************************************************************/
  
  /* Step to back to starting point of this subpath. If closepathatfirst==0, the
     final closepath caused a line segment. In this case, we first have to step
     back that segment and proceed from this point. */
  if ( ppoints[startind].shape == CURVE_CONVEX ) {
    /* In this case, left path is concave and the current location is at
       the onCurve point */
    dx1  = 0.0;
    dy1  = 0.0;
  }
  else {
    /* OK, it seems to be the intersection point */
    dx1  = ppoints[startind].dxir;
    dy1  = ppoints[startind].dyir;
  }
  /* We are now onCurve. If necessary step to the point where the closepath
     appeared. */
  if ( closepathatfirst == 0 ) {
    dx1 += ppoints[lastind].x - ppoints[startind].x;
    dy1 += ppoints[lastind].y - ppoints[startind].y;
  }

  
#ifdef DUMPDEBUGPATH
  if ( psfile != NULL )
    fprintf( psfile, "%f %f t1srmoveto %% pindex = %ld\n", dx1*up, dy1*up, lastind);
#endif
  B = Loc(CharSpace, dx1, dy1);
  path = Join(path, B);

  return;
  
}



/* Compute distance from OnCurve-points to their neighbouring points, fill in
   the respective entries dist2prev and dist2next in the ppoints[] structures
   and return the index of the last point in the current subpath which has
   a location different from the starting point of the subpath. */
static long computeDistances( long startind, long stopind, int subpathclosed)
{
  long   lastind       = 0;
  double dx            = 0.0;
  double dy            = 0.0;
  long   i             = 0;
  int    neighboured   = 0;


  /* Handle first point as a special case */
  /* distance to previous point. First, get index of previous point. */
  lastind = stopind;

  if ( subpathclosed != 0 ) {
    if ( (ppoints[startind].x == ppoints[stopind].x) &&
	 (ppoints[startind].y == ppoints[stopind].y) ) {
      while ( (ppoints[lastind].x == ppoints[stopind].x) &&
	      (ppoints[lastind].y == ppoints[stopind].y))
	--lastind;
    }
    else {
      lastind = stopind - 1;
    }
  }
  
#ifdef DEBUG_OUTLINE_SURROUNDING
  fprintf( stderr,
	   "computeDistance(): startind=%ld stopind=%ld, lastind=%ld, start.x=%f, start.y=%f, last.x=%f, last.y=%f\n",
	   startind, stopind, lastind, ppoints[startind].x, ppoints[startind].y,
	   ppoints[lastind].x, ppoints[lastind].y);
#endif
  
  dx = ppoints[startind].x - ppoints[lastind].x;
  dy = ppoints[startind].y - ppoints[lastind].y;
  ppoints[startind].dist2prev = sqrt( dx*dx + dy*dy );
  
  /* distance to next point */
  dx = ppoints[startind+1].x - ppoints[startind].x;
  dy = ppoints[startind+1].y - ppoints[startind].y;
  ppoints[startind].dist2next = sqrt( dx*dx + dy*dy );
  
#ifdef DEBUG_OUTLINE_SURROUNDING
  fprintf( stderr,
	   "Pre: Distance at point %ld: Prev=%f Next=%f\n",
	   startind, ppoints[startind].dist2prev, ppoints[startind].dist2next);
#endif
  
  for ( i = startind+1; i < lastind; i++ ) {
    if ( (ppoints[i].type == PPOINT_MOVE) ||
	 (ppoints[i].type == PPOINT_LINE) ||
	 (ppoints[i].type == PPOINT_BEZIER_D) ) {
      if ( neighboured ) {
	ppoints[i].dist2prev = ppoints[i-1].dist2next;
      }
      else {
	/* distance to previous point */
	dx = ppoints[i].x - ppoints[i-1].x;
	dy = ppoints[i].y - ppoints[i-1].y;
	/* Take care of degenerated curves */
	if ( (dx == 0.0) && (dy == 0.0) ) {
	  dx = ppoints[i].x - ppoints[i-2].x;
	  dy = ppoints[i].y - ppoints[i-2].y;
	  if ( (dx == 0.0) && (dy == 0.0) ) {
	    dx = ppoints[i].x - ppoints[i-3].x;
	    dy = ppoints[i].y - ppoints[i-3].y;
	  }
	}
	ppoints[i].dist2prev = sqrt( dx*dx + dy*dy );
      }
      /* distance to next point */
      dx = ppoints[i+1].x - ppoints[i].x;
      dy = ppoints[i+1].y - ppoints[i].y;
      /* Take care of degenerated curves */
      if ( (dx == 0.0) && (dy == 0.0) ) {
	dx = ppoints[i+2].x - ppoints[i].x;
	dy = ppoints[i+2].y - ppoints[i].y;
	if ( (dx == 0.0) && (dy == 0.0) ) {
	  dx = ppoints[i+3].x - ppoints[i].x;
	  dy = ppoints[i+3].y - ppoints[i].y;
	}
      }
      ppoints[i].dist2next = sqrt( dx*dx + dy*dy );
      neighboured = 1;
#ifdef DEBUG_OUTLINE_SURROUNDING
      fprintf( stderr, "     Distance at point %ld: Prev=%f Next=%f\n",
	       i, ppoints[i].dist2prev, ppoints[i].dist2next);
#endif
    }
    else {
      neighboured = 0;
    }
    
  }
  /* We still have to handle the last point */
  /* distance to previous point */
  dx = ppoints[lastind].x - ppoints[lastind-1].x;
  dy = ppoints[lastind].y - ppoints[lastind-1].y;
  /* Take care of degenerated curves */
  if ( (dx == 0.0) && (dy == 0.0) ) {
    dx = ppoints[lastind].x - ppoints[lastind-2].x;
    dy = ppoints[lastind].y - ppoints[lastind-2].y;
    if ( (dx == 0.0) && (dy == 0.0) ) {
      dx = ppoints[lastind].x - ppoints[lastind-3].x;
      dy = ppoints[lastind].y - ppoints[lastind-3].y;
    }
  }
  ppoints[lastind].dist2prev = sqrt( dx*dx + dy*dy );
  /* distance to next point */
  ppoints[lastind].dist2next = ppoints[startind].dist2prev;
#ifdef DEBUG_OUTLINE_SURROUNDING
  fprintf( stderr, "End: Distance at point %ld: Prev=%f Next=%f\n",
	   lastind, ppoints[lastind].dist2prev, ppoints[lastind].dist2next);
#endif
  
  return lastind;
  
}



/*

 */
static long handleNonSubPathSegments( long pindex)
{

  /* handle the different segment types in a switch-statement */
  switch ( ppoints[pindex].type ) {

  case PPOINT_SBW:
#ifdef DUMPDEBUGPATH
  if ( psfile != NULL )
    fprintf( psfile, "%f %f %f %f t1sbw %% pindex = %ld\n",
	     ppoints[pindex].x*up, ppoints[pindex].y*up,   /* sidebearings */
	     ppoints[pindex].ax*up, ppoints[pindex].ay*up,  /* escapements  */
	     pindex
	     );
#endif
    path = Join(path, Loc(CharSpace, ppoints[pindex].x, ppoints[pindex].y));
    return 1;
    break;
    

  case PPOINT_ENDCHAR:
    /* Perform a Closepath just in case the command was left out */
    path = ClosePath(path);
    
    /* Set character width / escapement. It is stored in the vars for
       hinted coordinates. */
    path = Join(Snap(path), Loc(CharSpace, ppoints[pindex].ax, ppoints[pindex].ay));
    
#ifdef DUMPDEBUGPATH
    if ( psfile != NULL )
      fputs( "t1FinishPage\n", psfile);
#endif
    return 1;
    break;
    

  case PPOINT_SEAC:
    /* return to origin of accent */
    apath = Snap(path);
    /* reset path to NULL */
    path  = NULL;
    return 1;
    break;
    
    
  default:
    /* not handled, return 0! */
    ;
  }
  
  return 0;
  
}



/* Transform a path point according to the path's incoming angle, the path's
   outgoing angle and the parameter strokewidth. The computation is based on
   simple geometric considerations and makes use of the distance from the
   current point to the previous point and the next point respectively.

   Generally, each link to a path point induces its own candidate by simply
   widening the respective link orthogonally to strokewidth/2. This yields
   two displacement vectors (dx,dy) for the link from the previous point to the
   point under consideration (dxp, dyp) and and for the link from the current
   point to the next point (dxn, dyn).

   Later on, the two candidates are used to compute the resulting displacement
   as the intersection of the prolongated lines from before and behind the
   current point.

   Additionally, check whether the curve is concave or convex at this point.
   This is required for prolongation in the context of stroking.
*/
static void transformOnCurvePathPoint( double strokewidth,
				       long prevind, long currind, long nextind)
{
  double distxp;
  double distyp;
  double distxn;
  double distyn;
  double det;

  /*
  distxp =  (ppoints[currind].y - ppoints[prevind].y);
  distyp = -(ppoints[currind].x - ppoints[prevind].x);
  distxn =  (ppoints[nextind].y - ppoints[currind].y);
  distyn = -(ppoints[nextind].x - ppoints[currind].x);

  ppoints[currind].dxpr = distxp * strokewidth * 0.5 / ppoints[currind].dist2prev;
  ppoints[currind].dypr = distyp * strokewidth * 0.5 / ppoints[currind].dist2prev;

  ppoints[currind].dxnr = distxn * strokewidth * 0.5 / ppoints[currind].dist2next;
  ppoints[currind].dynr = distyn * strokewidth * 0.5 / ppoints[currind].dist2next;
  */
  /* Note: When computing transformations of OnCurve points, we consider two
           special cases:

	   1) The OnCurve beginning or end point is equal to the neighboring
	      control point of a Bezier-Segment.

	   2) This holds for beginning *and* end point. In this case the curve
	      degenerates to a straight lines.

	   Although this is deprecated by Adobe, fonts that use such constructions
	   exist (e.g.m lower case 'n' of Univers 55). However, we do not care
	   for segments that do not any escapement at all!
  */
  
  distxp =  (ppoints[currind].y - ppoints[prevind].y);
  distyp = -(ppoints[currind].x - ppoints[prevind].x);
  if ( (distxp == 0.0) && (distyp == 0.0) ) {
    distxp =  (ppoints[currind].y - ppoints[prevind-1].y);
    distyp = -(ppoints[currind].x - ppoints[prevind-1].x);
    if ( (distxp == 0.0) && (distyp == 0.0) ) {
      distxp =  (ppoints[currind].y - ppoints[prevind-2].y);
      distyp = -(ppoints[currind].x - ppoints[prevind-2].x);
    }
  }
  ppoints[currind].dxpr = distxp * strokewidth * 0.5 / ppoints[currind].dist2prev;
  ppoints[currind].dypr = distyp * strokewidth * 0.5 / ppoints[currind].dist2prev;
  
  distxn =  (ppoints[nextind].y - ppoints[currind].y);
  distyn = -(ppoints[nextind].x - ppoints[currind].x);
  if ( (distxn == 0.0) && (distyn == 0.0) ) {
    distxn =  (ppoints[nextind+1].y - ppoints[currind].y);
    distyn = -(ppoints[nextind+1].x - ppoints[currind].x);
    if ( (distxn == 0.0) && (distyn == 0.0) ) {
      distxn =  (ppoints[nextind+2].y - ppoints[currind].y);
      distyn = -(ppoints[nextind+2].x - ppoints[currind].x);
    }
  }
  ppoints[currind].dxnr = distxn * strokewidth * 0.5 / ppoints[currind].dist2next;
  ppoints[currind].dynr = distyn * strokewidth * 0.5 / ppoints[currind].dist2next;
  
  /* Consider determinant of the two tangent vectors at this node in order to
     decide whether the curve is convex or cancave at this point. */
  if ( (det = ((distxp * distyn) - (distxn * distyp))) < 0.0 ) {
    /* curve turns to the right */
    ppoints[currind].shape = CURVE_CONCAVE;
  }
  else if ( det > 0.0 ) {
    /* curve turns to the left */
    ppoints[currind].shape = CURVE_CONVEX;
  }
  else {
    /* curve is straight */
    ppoints[currind].shape = CURVE_STRAIGHT;
  }
  
  return;
}


/* Compute a displacement for offCurve points, that is, for Bezier B and C points.
   
   This computation is not as simple as it might appear at a first glance and,
   depending on the actual curve parameters and the parameter strokewidth, it might
   be necessary to subdivide the curve. My mathematical background is not actually
   reliable in this context but it seems that in particular the angle that the curve
   runs through is important in this context. Since the Adobe Type 1 recommendations
   on font design include a rule which states that curves' end points should be located
   at extreme values, and consequently, that the angle of a curve segment should not
   be larger than 90 degrees, I have decided not to implement curve subdivision. This
   might lead to some deviations if fonts do not adhere to the Adobe recommendations.
   Anyways, I have never seen such fonts.

   This function is called for Bezier_B points and computes displacements for the B
   and C points at once. Fortunately, index cycling cannot happen here. When
   computing the B' C' off-curve points, special care against numerical instability
   is required. We assume that at least one of the two points can be computed
   in a stable way.

   The new Bezier B' and C' points can be considered as four degrees of freedom and we have
   to find 4 equations to be able to compute them. 

   1) We require the tangents slope at point A' to be identical to the slope at the
      point A of the ideally thin mathematical curve.

   2) The same holds for the tangents slope at point D' with respect to point D.

   3) We compute the following points

      P1:       Equally subdivides the line A - B
      P2:       Equally subdivides the line B - C
      P3:       Equally subdivides the line C - D

      P4:       Equally subdivides the line P1 - P2
      P5:       Equally subdivides the line P1 - P3

      P6:       Equally subdivides the line P4 - P5

      This latter point is part of the curve and, moreover, the line P4 - P5 is
      tangent to the curve at that point.
      From this point, we compute a displacement for P6, orthogonally to the curve
      at that point and with length strokewidth/2. The resulting point is part of
      the delimiting path that makes up the thick curve.

   4) We require that the tangent's slope at P6' equals the tangents slope at P6.

   Then, under certain further constraints as mentioned above, we compute the points
   B' and C' making use of the points A' and D' which have been transformed as onCurve
   points. By definition, for offCurve points, there is only one candidate.

 */
static void transformOffCurvePathPoint( double strokewidth, long currind)
{
  double dtmp;
  double diameter;
  double dx;
  double dy;
  
  /* points defining the curve */
  double pax;
  double pay;
  double pbx;
  double pby;
  double pcx;
  double pcy;
  double pdx;
  double pdy;
  
  /* auxiliary points from iterative Bezier construction */
  double p1x;
  double p1y;
  double p2x;
  double p2y;
  double p3x;
  double p3y;
  double p4x;
  double p4y;
  double p5x;
  double p5y;
  double p6x;
  double p6y;
  
  /* already displaced / shifted onCurve points and the ones we are going
     to compute. */
  double paxs;
  double pays;
  double pbxs;
  double pbys;
  double pcxs;
  double pcys;
  double pdxs;
  double pdys;

  /* The normal vector on the curve at t=1/2 */
  double nabs;
  double nx;
  double ny;

  /* some variables for computations at point B' */
  double bloc1x;         
  double bloc1y;
  double bdir1x;
  double bdir1y;
  double bloc2x;
  double bloc2y;
  double bdir2x;
  double bdir2y;
  double bdet;
  double binvdet;
  double binvdir1x;
  double binvdir1y; /**/
  double binvdir2x;
  double binvdir2y; /**/
  double bmu;
  double bnu; /**/

  /* some variables for computations at point C' */
  double cloc1x;         
  double cloc1y;
  double cdir1x;
  double cdir1y;
  double cloc2x;
  double cloc2y;
  double cdir2x;
  double cdir2y;
  double cdet;
  double cinvdet;
  double cinvdir1x;
  double cinvdir1y; /**/
  double cinvdir2x;
  double cinvdir2y; /**/
  double cmu;
  double cnu; /**/
  
  diameter = strokewidth * 0.5;
  
  pax = ppoints[currind-1].x;
  pay = ppoints[currind-1].y;
  pbx = ppoints[currind].x;
  pby = ppoints[currind].y;
  pcx = ppoints[currind+1].x;
  pcy = ppoints[currind+1].y;
  pdx = ppoints[currind+2].x;
  pdy = ppoints[currind+2].y;
  
  p1x = (pax + pbx) * 0.5;
  p1y = (pay + pby) * 0.5;
  p2x = (pbx + pcx) * 0.5;
  p2y = (pby + pcy) * 0.5;
  p3x = (pcx + pdx) * 0.5;
  p3y = (pcy + pdy) * 0.5;
  p4x = (p1x + p2x) * 0.5;
  p4y = (p1y + p2y) * 0.5;
  p5x = (p2x + p3x) * 0.5;
  p5y = (p2y + p3y) * 0.5;
  p6x = (p4x + p5x) * 0.5;
  p6y = (p4y + p5y) * 0.5;

  
  /* We start by computing the shift of the onCurve points. It is not possible
     to use  dxr / dyr of the ppoints-stucture entries. These values have been
     computed by intersection of both links to a path point. Here we need the
     ideal points of the thick isolated curve segment. We are aware that for
     Bezier splines, control point and OnCurve point might be identical! */
  dx   =   (ppoints[currind].y - ppoints[currind-1].y) * strokewidth * 0.5 / ppoints[currind-1].dist2next;
  dy   = - (ppoints[currind].x - ppoints[currind-1].x) * strokewidth * 0.5 / ppoints[currind-1].dist2next;
  if ( (dx == 0.0) && (dy == 0.0) ) {
    /* Bezier_A and Bezier_B are identical */
    dx   =   (ppoints[currind+1].y - ppoints[currind-1].y) * strokewidth * 0.5 / ppoints[currind-1].dist2next;
    dy   = - (ppoints[currind+1].x - ppoints[currind-1].x) * strokewidth * 0.5 / ppoints[currind-1].dist2next;
  }
  paxs = ppoints[currind-1].x + dx;
  pays = ppoints[currind-1].y + dy;
  dx   =   (ppoints[currind+2].y - ppoints[currind+1].y) * strokewidth * 0.5 / ppoints[currind+2].dist2prev;
  dy   = - (ppoints[currind+2].x - ppoints[currind+1].x) * strokewidth * 0.5 / ppoints[currind+2].dist2prev;
  if ( (dx == 0.0) && (dy == 0.0) ) {
    /* Bezier_C and Bezier_D are identical */
    dx   =   (ppoints[currind+2].y - ppoints[currind].y) * strokewidth * 0.5 / ppoints[currind+2].dist2prev;
    dy   = - (ppoints[currind+2].x - ppoints[currind].x) * strokewidth * 0.5 / ppoints[currind+2].dist2prev;
  }
  pdxs = ppoints[currind+2].x + dx;
  pdys = ppoints[currind+2].y + dy;

  /* Next, we compute the right side normal vector at the curve point t=1/2,
   that is, at P6. */
  nabs    = diameter / sqrt(((p5x - p4x) * (p5x - p4x)) + ((p5y - p4y) * (p5y - p4y)));
  nx      = (p5y - p4y) * nabs;
  ny      = (p4x - p5x) * nabs;

#ifdef DEBUG_OUTLINE_SURROUNDING
  fprintf( stderr, "transformOffCurvePathPoint():\n");
  fprintf( stderr, "    A=(%f,%f), B=(%f,%f), C=(%f,%f), D=(%f,%f)\n",
	   pax, pay, pbx, pby, pcx, pcy, pdx, pdy);
  fprintf( stderr, "    PathInfo: Curve from PP %ld ... PP %ld ... PP %ld ... PP %ld. StrokeWidth=%f.\n",
	   currind-1, currind, currind+1, currind+2, strokewidth);
  /*
  fprintf( stderr, "/xa %f def\n/ya %f def\n/xb %f def\n/yb %f def\n/xc %f def\n/yc %f def\n/xd %f def\n/yd %f def\n",
	   pax, pay, pbx, pby, pcx, pcy, pdx, pdy);
  */
  fprintf( stderr, "    As=(%f,%f), Ds=(%f,%f)\n",
	   paxs, pays, pdxs, pdys);
  fprintf( stderr, "    p6=(%f,%f)\n", p6x, p6y);
  fprintf( stderr, "    nx=%f, ny=%f, nabs=%f\n", nx, ny, nabs);
  fprintf( stderr, "    p6s=(%f,%f)\n", p6x+nx, p6y+ny);
#endif

  /* Compute two lines whose intersection will define point B' */
  bloc1x = (4.0 * (nx + p6x) - (2 * paxs) + pdxs) / 3.0;
  bloc1y = (4.0 * (ny + p6y) - (2 * pays) + pdys) / 3.0;
  bdir1x = pcx + pdx - pax - pbx;
  bdir1y = pcy + pdy - pay - pby;
  bloc2x = paxs;
  bloc2y = pays;
  bdir2x = pbx - pax;
  bdir2y = pby - pay;
  bdet   = (bdir2x * bdir1y) - (bdir2y * bdir1x);

#define DET_QUOTIENT_UPPER_THRESHOLD      (1.05)
#define DET_QUOTIENT_LOWER_THRESHOLD      (1.0/DET_QUOTIENT_UPPER_THRESHOLD)

  /* Life has shown, that the "reliability" of the determinant has to be
     ensured. Otherwise, serious distortions might be introduced.
     In order to ensure numerical stability, we do not only check whether
     the detrminant is about zero, but we also check whether the two partial
     expressions that are subtracted when computing the determinant are of
     about the same size. If this is the case, we explicitly reset the
     determinant and eventually compute this off-curve point based on the
     other off-curve point later. */
  if ( (bdir2x != 0.0) && (bdir1y != 0.0) ) {
    dtmp = (bdir2y*bdir1x)/(bdir2x*bdir1y);
    if ( (DET_QUOTIENT_LOWER_THRESHOLD < dtmp) &&
	 (DET_QUOTIENT_UPPER_THRESHOLD > dtmp)
	 ) {
      /* Determinant appears to be unreliable, reset it exactly to zero. */
      bdet = 0.0;
#ifdef DEBUG_OUTLINE_SURROUNDING
      fprintf( stderr, "    Warning: Determinant quotient check for bdet failed: dtmp=%16.16f, lower limit=%f, upper limit=%f.\n    --> Determinant does not seem to be stable, resetting to zero.\n",
	       dtmp, DET_QUOTIENT_LOWER_THRESHOLD, DET_QUOTIENT_UPPER_THRESHOLD);
#endif
    }
  }
  else if ( (bdir2y != 0.0) && (bdir1x != 0.0) ) {
    dtmp = (bdir2x*bdir1y)/(bdir2y*bdir1x);
    if ( (DET_QUOTIENT_LOWER_THRESHOLD < dtmp) &&
	 (DET_QUOTIENT_UPPER_THRESHOLD > dtmp)
	 ) {
      /* Determinant appears to be unreliable, reset it exactly to zero. */
      bdet = 0.0;
#ifdef DEBUG_OUTLINE_SURROUNDING
      fprintf( stderr, "    Warning: Determinant quotient check for bdet failed: dtmp=%16.16f, lower limit=%f, upper limit=%f.\n    --> Determinant does not seem to be stable, resetting to zero.\n",
	       dtmp, DET_QUOTIENT_LOWER_THRESHOLD, DET_QUOTIENT_UPPER_THRESHOLD);
#endif
    }
  }
      
  
#ifdef DEBUG_OUTLINE_SURROUNDING
  fprintf( stderr, "    bloc1x=%f, bloc1y=%f, bloc2x,=%f bloc2y=%f\n",
	   bloc1x, bloc1y, bloc2x, bloc2y);
  fprintf( stderr, "    bdir1x=%f, bdir1y=%f, bdir2x,=%f bdir2y=%f\n",
	   bdir1x, bdir1y, bdir2x, bdir2y);
#endif

  /* Switch if determinant is zero; we then actually have a straight line */
  if ( fabs(bdet) < 0.001 ) {
    pbxs   = pbx + nx;
    pbys   = pby + ny;
    bmu    = 0.0;
    bnu    = 0.0;
#ifdef DEBUG_OUTLINE_SURROUNDING
    fprintf( stderr, "    Warning: Determinant check for bdet failed: bdet=%16.16f. Computing Bs based on normal vector, resetting bmu, bnu.\n",
	     bdet);
#endif
  }
  else {
    /* Calculate required part of inverse matrix */
    binvdet   =   1.0 / bdet;
    binvdir2x =   bdir1y * binvdet;
    binvdir2y = - bdir2y * binvdet; /**/
    binvdir1x = - bdir1x * binvdet;
    binvdir1y =   bdir2x * binvdet; /**/

    /* Calculate coefficient that describes intersection */
    bmu       =   (binvdir2x * (bloc1x - bloc2x)) + (binvdir1x * (bloc1y - bloc2y));
    bnu       =   (binvdir2y * (bloc1x - bloc2x)) + (binvdir1y * (bloc1y - bloc2y)); /**/

    /* Calculate B' */
    pbxs      =   bloc2x + (bmu * bdir2x);
    pbys      =   bloc2y + (bmu * bdir2y);
  }

  /* Compute two lines whose intersection will define point C' */
  cloc1x = (4.0 * (nx + p6x) - (2 * pdxs) + paxs) / 3.0;
  cloc1y = (4.0 * (ny + p6y) - (2 * pdys) + pays) / 3.0;
  cdir1x = bdir1x;
  cdir1y = bdir1y;
  cloc2x = pdxs;
  cloc2y = pdys;
  cdir2x = pcx - pdx;
  cdir2y = pcy - pdy;
  cdet   = (cdir2x * cdir1y) - (cdir2y * cdir1x);

  /* Life has shown, that the "reliability" of the determinant has to be
     ensured. Otherwise, serious distortions might be introduced.
     In order to ensure numerical stability, we do not only check whether
     the detrminant is about zero, but we also check whether the two partial
     expressions that are subtracted when computing the determinant are of
     about the same size. If this is the case, we explicitly reset the
     determinant and eventually compute this off-curve point based on the
     other off-curve point later. */
  if ( (cdir2x != 0.0) && (cdir1y != 0.0) ) {
    dtmp = (cdir2y*cdir1x)/(cdir2x*cdir1y);
    if ( (DET_QUOTIENT_LOWER_THRESHOLD < dtmp) &&
	 (DET_QUOTIENT_UPPER_THRESHOLD > dtmp)
	 ) {
      /* Determinant appears to be unreliable, reset it exactly to zero. */
      cdet = 0.0;
#ifdef DEBUG_OUTLINE_SURROUNDING
      fprintf( stderr, "    Warning: Determinant quotient check for cdet failed: dtmp=%16.16f, lower limit=%f, upper limit=%f.\n    --> Determinant does not seem to be stable, resetting to zero.\n",
	       dtmp, DET_QUOTIENT_LOWER_THRESHOLD, DET_QUOTIENT_UPPER_THRESHOLD);
#endif
    }
  }
  else if ( (cdir2y != 0.0) && (cdir1x != 0.0) ) {
    dtmp = (cdir2x*cdir1y)/(cdir2y*cdir1x);
    if ( (DET_QUOTIENT_LOWER_THRESHOLD < dtmp) &&
	 (DET_QUOTIENT_UPPER_THRESHOLD > dtmp)
	 ) {
      /* Determinant appears to be unreliable, reset it exactly to zero. */
      cdet = 0.0;
#ifdef DEBUG_OUTLINE_SURROUNDING
      fprintf( stderr, "    Warning: Determinant quotient check for cdet failed: dtmp=%16.16f, lower limit=%f, upper limit=%f.\n    --> Determinant does not seem to be stable, resetting to zero.\n",
	       dtmp, DET_QUOTIENT_LOWER_THRESHOLD, DET_QUOTIENT_UPPER_THRESHOLD);
#endif
    }
  }
      
#ifdef DEBUG_OUTLINE_SURROUNDING
  fprintf( stderr, "    cloc1x=%f, cloc1y=%f, cloc2x,=%f cloc2y=%f\n",
	   cloc1x, cloc1y, cloc2x, cloc2y);
  fprintf( stderr, "    cdir1x=%f, cdir1y=%f, cdir2x,=%f cdir2y=%f\n",
	   cdir1x, cdir1y, cdir2x, cdir2y);
#endif

  /* Switch if determinant is zero; we then actually have a straight line */
  if ( fabs( cdet) < 0.001 ) {
    pcxs   = pcx + nx;
    pcys   = pcy + ny;
    cmu    = 0.0;
    cnu    = 0.0;
#ifdef DEBUG_OUTLINE_SURROUNDING
    fprintf( stderr, "    Warning: Determinant check for cdet failed: cdet=%16.16f. Computing Cs based on normal vector, resetting cmu, cnu.\n",
	     cdet);
#endif
  }
  else {
    /* Calculate required part of inverse matrix */
    cinvdet   =   1.0 / cdet;
    cinvdir2x =   cdir1y * cinvdet;
    cinvdir2y = - cdir2y * cinvdet; /**/
    cinvdir1x = - cdir1x * cinvdet;
    cinvdir1y =   cdir2x * cinvdet; /**/

    /* Calculate coefficient that describes intersection */
    cmu       =   (cinvdir2x * (cloc1x - cloc2x)) + (cinvdir1x * (cloc1y - cloc2y));
    cnu       =   (cinvdir2y * (cloc1x - cloc2x)) + (cinvdir1y * (cloc1y - cloc2y)); /**/

    /* Calculate C' */
    pcxs      =   cloc2x + (cmu * cdir2x);
    pcys      =   cloc2y + (cmu * cdir2y);
  }

#ifdef DEBUG_OUTLINE_SURROUNDING
  fprintf( stderr, "    bdet=%f, cdet=%f, bmu=%f, bnu=%f, cmu=%f, cnu=%f\n",
	   bdet, cdet, bmu, bnu, cmu, cnu);
#endif

  /* Analyse coefficients and decide on numerical stability. If suggesting,
     overwrite, using another relation. Here, we assume that at least the
     solution at *one* end of the curve is stable. */
  if ( fabs(bmu) < 0.1 ) {
    pbxs = ((8 * (nx + p6x) - paxs - pdxs) / 3.0) - pcxs;
    pbys = ((8 * (ny + p6y) - pays - pdys) / 3.0) - pcys;
#ifdef DEBUG_OUTLINE_SURROUNDING
    fprintf( stderr, "    Warning: Coefficient check for bmu failed: bmu=%16.16f. Computing Bs based on Cs.\n",
	     bmu);
#endif
  }
  if ( fabs(cmu) < 0.1 ) {
    pcxs = ((8 * (nx + p6x) - paxs - pdxs) / 3.0) - pbxs;
    pcys = ((8 * (ny + p6y) - pays - pdys) / 3.0) - pbys;
#ifdef DEBUG_OUTLINE_SURROUNDING
    fprintf( stderr, "    Warning: Coefficient check for cmu failed: cmu=%16.16f. Computing Cs based on Bs.\n",
	     cmu);
#endif
  }
  
  
  /* Store the resulting displacement values in the ppoints-struct so
     they can be used for path construction. We use the "intersect" member
     because in this case nothing is related to "previous" or "next".*/
#ifdef DEBUG_OUTLINE_SURROUNDING
  fprintf( stderr, "    pbx=%f, pbxs=%f, bxshift=%f, pby=%f, pbys=%f, byshift=%f\n",
	   pbx, pbxs, pbxs-pbx, pby, pbys, pbys-pby);
  fprintf( stderr, "    pcx=%f, pcxs=%f, cxshift=%f, pcy=%f, pcys=%f, cyshift=%f\n",
	   pcx, pcxs, pcxs-pcx, pcy, pcys, pcys-pcy);
  fprintf( stderr, "    Summary:    A =(%f,%f), B =(%f,%f), C =(%f,%f), D =(%f,%f)\n",
	   pax, pay, pbx, pby, pcx, pcy, pdx, pdy);
  fprintf( stderr, "                As=(%f,%f), Bs=(%f,%f), Cs=(%f,%f), Ds=(%f,%f)\n\n",
	   paxs, pays, pbxs, pbys, pcxs, pcys, pdxs, pdys);
#endif
  ppoints[currind].dxir    = pbxs - pbx;
  ppoints[currind].dyir    = pbys - pby;
  ppoints[currind+1].dxir  = pcxs - pcx;
  ppoints[currind+1].dyir  = pcys - pcy;

  return;
  
}


static void intersectRight( long index, double halfwidth, long flag)
{
  double r2  = 0.0;
  double det = 0.0;
  double dxprev;
  double dyprev;
  double dxnext;
  double dynext;
  
  
  /* In order to determine the intersection between the two
     prolongations at the path point under consideration, we use
     the Hesse Normal Form, multiplied with r.

     dx * x + dy * y + r^2 = 0

     Here, r is the distance from the origin, that is, from the path point
     under consideration. */

  /* Check for start and ending of non-closed paths */
  if ( flag == INTERSECT_PREVIOUS ) {
    ppoints[index].dxir = ppoints[index].dxpr;
    ppoints[index].dyir = ppoints[index].dypr;
    /* Correct shape to be "straight" at ending point! */
    ppoints[index].shape = CURVE_STRAIGHT;
    return;
  }
  if ( flag == INTERSECT_NEXT ) {
    ppoints[index].dxir = ppoints[index].dxnr;
    ppoints[index].dyir = ppoints[index].dynr;
    /* Correct shape to be "straight" at starting point! */
    ppoints[index].shape = CURVE_STRAIGHT;
    return;
  }

  /* OK, we actually compute an intersection */
  dxprev = ppoints[index].dxpr;
  dyprev = ppoints[index].dypr;
  dxnext = ppoints[index].dxnr;
  dynext = ppoints[index].dynr;

  /* Compute distance square */
  r2 = halfwidth * halfwidth;

  /* Check the determinant. If it is zero, the two lines are parallel
     and also must touch at atleast one location,
     so that there are an infinite number of solutions. In this case,
     we compute the average position and are done. */
  if ( fabs( (det = ((dyprev * dxnext) - (dynext * dxprev))) ) < 0.00001 ) {
    ppoints[index].dxir = 0.5 * (dxprev + dxnext);
    ppoints[index].dyir = 0.5 * (dyprev + dynext);
#ifdef DEBUG_OUTLINE_SURROUNDING
    fprintf( stderr, "intersectRight(0):\n    dxprev=%f, dxnext=%f, dxres=%f,\n    dyprev=%f, dynext=%f, dyres=%f,\n    det=%16.16f\n",
	     dxprev, dxnext, ppoints[index].dxir, dyprev, dynext, ppoints[index].dyir, det);
    fprintf( stderr, "    --> Computation based on averaging [dxprev,dyprev] and [dxnext,dynext]\n");
    fprintf( stderr, "    Right intersection point shift: (%f,%f), absolute shift length: %f.\n\n",
	     ppoints[index].dxir, ppoints[index].dyir,
	     sqrt(ppoints[index].dxir*ppoints[index].dxir + ppoints[index].dyir*ppoints[index].dyir));
#endif
    return;
  }
  /* OK, there seems to be a unique solution, compute it */
  if ( dxprev != 0.0 ) {
    ppoints[index].dyir =  r2 * (dxnext - dxprev) / det;
    ppoints[index].dxir = (r2 - (dyprev * ppoints[index].dyir)) / dxprev; /* - ? */
#ifdef DEBUG_OUTLINE_SURROUNDING
    fprintf( stderr, "intersectRight(1):\n    dxprev=%f, dxnext=%f, dxres=%f,\n    dyprev=%f, dynext=%f, dyres=%f,\n    det=%16.16f\n",
	     dxprev, dxnext, ppoints[index].dxir, dyprev, dynext, ppoints[index].dyir, det);
    fprintf( stderr, "    --> Computation based on previous path point.\n");
    fprintf( stderr, "    Right intersection point shift: (%f,%f), absolute shift length: %f.\n\n",
	     ppoints[index].dxir, ppoints[index].dyir,
	     sqrt(ppoints[index].dxir*ppoints[index].dxir + ppoints[index].dyir*ppoints[index].dyir));
#endif
  }
  else {
    ppoints[index].dyir = -r2 * (dxprev - dxnext) / det;
    ppoints[index].dxir = (r2 - (dynext * ppoints[index].dyir)) / dxnext; /* - ? */
#ifdef DEBUG_OUTLINE_SURROUNDING
    fprintf( stderr, "intersectRight(2):\n    dxprev=%f, dxnext=%f, dxres=%f,\n    dyprev=%f, dynext=%f, dyres=%f,\n    det=%16.16f\n",
	     dxprev, dxnext, ppoints[index].dxir, dyprev, dynext, ppoints[index].dyir, det);
    fprintf( stderr, "    --> Computation based on next path point.\n");
    fprintf( stderr, "    Right intersection point shift: (%f,%f), absolute shift length: %f.\n\n",
	     ppoints[index].dxir, ppoints[index].dyir,
	     sqrt(ppoints[index].dxir*ppoints[index].dxir + ppoints[index].dyir*ppoints[index].dyir));
#endif
  }
  
  return;
     
}



/* linkNode(): Insert prolongation lines at nodes. */
static void linkNode( long index, int position, int orientation)
{
  struct segment* B;
  double dx = 0.0;
  double dy = 0.0;

  if ( orientation == PATH_RIGHT ) {
    /* We are constructing the right hand side path */
    if ( position == PATH_START ) {
      /* We are starting a new segment. Link from current point to ideally
	 next-shifted point of segment. */
      if ( ppoints[index].shape == CURVE_CONCAVE ) {
	/* prolongate from original curve point to ideally next-shifted point */
	dx = ppoints[index].dxnr;
	dy = ppoints[index].dynr;
#ifdef DEBUG_OUTLINE_SURROUNDING
	fprintf( stderr, "RP:  Concave at PP %ld. Prolongation from onCurve to ideal: (%f,%f)\n",
		 index, dx, dy);
#endif
      }
      else if ( ppoints[index].shape == CURVE_CONVEX ) {
	/* prolongate from intersecion point to ideally next-shifted point */
	dx = ppoints[index].dxnr - ppoints[index].dxir;
	dy = ppoints[index].dynr - ppoints[index].dyir;
#ifdef DEBUG_OUTLINE_SURROUNDING
	fprintf( stderr, "RP:  Convex at PP %ld. Prolongation from intersection to ideal: (%f,%f)\n",
		 index, dx, dy);
#endif
      }
    }
    else if ( position == PATH_END ) {
      /* We are ending the current segment. Link from ideally prev-shifted point
	 to the appropriate ending point. */
      if ( ppoints[index].shape == CURVE_CONCAVE ) {
	/* prolongate from ideally prev-shifted point to original curve point. */
	dx = - ppoints[index].dxpr;
	dy = - ppoints[index].dypr;
#ifdef DEBUG_OUTLINE_SURROUNDING
	fprintf( stderr, "RP:  Concave at PP %ld. Prolongation from ideal to onCurve: (%f,%f)\n",
		 index, dx, dy);
#endif
      }
      else if ( ppoints[index].shape == CURVE_CONVEX ) {
	/* prolongate from ideally prev-shifted point to intersection point. */
	dx = ppoints[index].dxir - ppoints[index].dxpr;
	dy = ppoints[index].dyir - ppoints[index].dypr;
#ifdef DEBUG_OUTLINE_SURROUNDING
	fprintf( stderr, "RP:  Convex at PP %ld. Prolongation from ideal to intersection: (%f,%f)\n",
		 index, dx, dy);
#endif
      }
    } /* if ( PATH_END ) */
  } /* if ( PATH_RIGHT ) */
  else if ( orientation == PATH_LEFT ) {

    /* We are constructing the left hand side path. Some notions have to be
       reverted (e.g. concavity vs. convexity and next vs. previous)! */
    if ( position == PATH_START ) {
      /* We are starting a new segment. Link from current point to ideally
	 next-shifted point of segment. */
      if ( ppoints[index].shape == CURVE_CONVEX ) {
	/* prolongate from original curve point to ideally next-shifted point.
	   Remember: next --> prev! */
	dx = - (ppoints[index].dxpr);
	dy = - (ppoints[index].dypr);
#ifdef DEBUG_OUTLINE_SURROUNDING
	fprintf( stderr, "LP:  Concave at PP %ld. Prolongation from onCurve to ideal: (%f,%f)\n",
		 index, dx, dy);
#endif
      }
      else if ( ppoints[index].shape == CURVE_CONCAVE ) {
	/* prolongate from intersecion point to ideally next-shifted point */
	dx = - (ppoints[index].dxpr - ppoints[index].dxir);
	dy = - (ppoints[index].dypr - ppoints[index].dyir);
#ifdef DEBUG_OUTLINE_SURROUNDING
	fprintf( stderr, "LP:  Convex at PP %ld. Prolongation from intersection to ideal: (%f,%f)\n",
		 index, dx, dy);
#endif
      }
    }/* if ( PATH_START ) */
    else if ( position == PATH_END ) {
      /* We are ending the current segment. Link from ideally prev-shifted point
	 to the appropriate ending point. */
      if ( ppoints[index].shape == CURVE_CONVEX ) {
	/* prolongate from ideally prev-shifted point to original curve point. */
	dx = ppoints[index].dxnr;
	dy = ppoints[index].dynr;
#ifdef DEBUG_OUTLINE_SURROUNDING
	fprintf( stderr, "LP:  Concave at PP %ld. Prolongation from ideal to onCurve: (%f,%f)\n",
		 index, dx, dy);
#endif
      }
      else if ( ppoints[index].shape == CURVE_CONCAVE ) {
	/* prolongate from ideally prev-shifted point to intersection point. */
	dx = - (ppoints[index].dxir - ppoints[index].dxnr);
	dy = - (ppoints[index].dyir - ppoints[index].dynr);
#ifdef DEBUG_OUTLINE_SURROUNDING
	fprintf( stderr, "LP:  Convex at PP %ld. Prolongation from ideal to intersection: (%f,%f)\n",
		 index, dx, dy);
#endif
      }
    } /* if ( PATH_END ) */
  } /* if ( PATH_LEFT ) */

  if ( (dx != 0.0) || (dy != 0.0) ) {
#ifdef DUMPDEBUGPATH
    if ( psfile != NULL )
      fprintf( psfile, "%f %f t1sprolongate %% pindex = %ld\n", dx*up, dy*up, index);
#endif
    B = Loc( CharSpace, dx, dy);
    path = Join(path, Line(B));
  }
  
  return;
  
}


int T1int_Type1QuerySEAC( unsigned char* base,
			  unsigned char* accent)
{
  if ( isseac == 0 ) {
    return 0;
  }
  
  *base   = seacbase;
  *accent = seacaccent;

  return isseac;
}

