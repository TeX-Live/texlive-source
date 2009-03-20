/*--------------------------------------------------------------------------
  ----- File:        t1types.h
  ----- Author:      Rainer Menzner (Rainer.Menzner@web.de)
  ----- Date:        2004-12-09
  ----- Description: This file is part of the t1-library. It contains
                     type definitions used by the t1-library.
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
  

#define T1TYPES_H

#include "sysconf.h"

typedef struct
{
  int flags;
  int chars;
  int hkern;
} METRICS_ENTRY;


typedef struct 
{
  char *pFontFileName; /* Pointer to the font's filename */
  char *pAfmFileName;  /* Pointer to the afm filename, IFF set explicitly */
  FontInfo *pAFMData;  /* A pointer to a struct which gives access to all
			  the data contained in the .afm-file. If this
			  pointer is NULL, no .afm-file had been found.
			  => There's no advanced info on the font available.
		       */
  psfont *pType1Data;  /* A pointer to a struct giving access to all
			  information contained in the .pfa/.pfb-file. This
			  is needed! */
  int *pEncMap;           /* For fast mapping from charnames to encoding
			     indices */
  METRICS_ENTRY *pKernMap;   /* dito */
  int KernMapSize;
  char **pFontEnc;        /* This is the pointer to the encoding array
			     associated with that particular font. If
			     FontEnc=NULL, it means the internal
			     (fontspecific)  encoding is to be used. */
  char *vm_base;  /* The base address of the virtual memory area for this
		     font. It must be stored in order to be able to realloc
		     and free those memory areas later. */
  void *pFontSizeDeps;  /* This one points to a linked list of structures
			   which store all font size dependent data. */
  double FontMatrix[4]; /* Two matrices which store the font matrix and special
			   Transformation to be applied, such as slant and
			   extend or probably some rotation. */
  double FontTransform[4];
  float slant;    /* A slant factor for the font */
  float extend;   /* A extension factor for the font */
  float UndrLnPos;     /* Parameters for ~lining rules */
  float UndrLnThick;
  float OvrLnPos;
  float OvrLnThick;
  float OvrStrkPos;
  float OvrStrkThick;
  float StrokeWidth;
  float SavedStrokeWidth;
  
  unsigned short physical; /* This entry is used to decide, whether a
			      font is associated with an own physical
			      fontfile, or whether it has been created
			      as a "logical" font by copying another
			      "physical" font. */ 
  unsigned short refcount; /* At load time this counter is set to 1. Every
			      time, a T1_CopyFont() is executed on this font,
			      this counter is incremented by 1. This gives
			      the possibility to decide whether a physical
			      font is used by some logical font. */
  short space_position; /* The position where "space" is encoded, is saved
			   in this entry. The space character needs special
			   treatment. Saving its position here yields faster
			   execution during rastering of strings with a
			   user-supplied space-offset! */
  short info_flags;     /* Here some info may be stored */
} FONTPRIVATE;


/* A structure representing a matrix */
typedef struct
{
  double cxx;
  double cyx;
  double cxy;
  double cyy;
} T1_TMATRIX;


/* Following struct is used for storing all information for a particular
   rendered character glyph */
typedef struct
{
  char *bits;         /* A pointer to the characters local bitmap */
  struct              /* A struct containing diverse metric information */
  {
    int ascent;
    int descent;
    int leftSideBearing;
    int rightSideBearing;
    int advanceX;
    int advanceY;
  } metrics;
  void *pFontCacheInfo;
  unsigned long bpp;    /* The number of bits that represent 1 pixel */
} GLYPH;



/* Next comes the struct declaration for FontSizeDeps, which stores size
   specific data of a font */
typedef struct
{
  GLYPH    *pFontCache;         /* Pointer to the cache area of this
				   font at this size */
  void     *pNextFontSizeDeps;  /* A pointer to the next size's
				   FontSizeDeps-structure. */
  void     *pPrevFontSizeDeps;  /* A pointer to the previous size's
				   FontSizeDeps-structure or NULL if
				   the current is the first. */
  struct XYspace *pCharSpaceLocal;    /* This is a scaled version of the
					 global version for this font. */
  float    size;                /* The desired size, to be specified
				   in bp's. */
  int      antialias;           /* Switch for marking the current size
				   antialiased */
} FONTSIZEDEPS;



/* A data type that makes most important information available to user. */
typedef struct
{
  int      width;       /* The glyph's width */
  BBox     bbox;        /* The glyph's bounding box */
  int      numchars;    /* The number of characters in the glyph (string) */
  int      *charpos;    /* A pointer to an integer array were the horizontal
			   positions in (afm units) of the individual
			   characters in the string are stored */
} METRICSINFO;


/* Handling of outlines: These definitions  decouple from the type 1 rasterizers
   def's and make the necessary stuff available to end users */
 
#define   FRACTBITS     16   /* number of fractional bits in 'fractpel'      */
/* From/to conversion of pels/fractpels */
#define   T1_TOPATHPOINT(p)      (((T1_AA_TYPE32)p)<<FRACTBITS)
#define   PPHALF                 (1<<(FRACTBITS-1))
#define   T1_NEARESTPOINT(fp)    (((fp)+PPHALF)>>FRACTBITS)

/* A fractional point */
typedef struct {
  T1_AA_TYPE32 x;
  T1_AA_TYPE32 y;
} T1_PATHPOINT;


/* A straight outline segment, stroked or not stroked */
typedef struct pathsegment {  
  char type;                /* type of segment (line or move) */
  unsigned char flag;       /* type1 rasterizer internal stuff */
  short references;         /* type1 rasterizer internal stuff */
  unsigned char size;       /* size of the structure */
  unsigned char context;    /* index to device context */
  struct pathsegment *link; /* pointer to next structure in linked list */
  struct pathsegment *last; /* pointer to last structure in list */
  T1_PATHPOINT    dest;     /* relative ending location of path segment */
} T1_PATHSEGMENT;

/* A third order bezier segment */
typedef struct bezierpathsegment {
  char type;                /* type of segment (bezier) */
  unsigned char flag;       /* type1 rasterizer internal stuff */
  short references;         /* type1 rasterizer internal stuff */
  unsigned char size;       /* as with any 'segment' type */
  unsigned char context;    /* as with any 'segment' type */
  T1_PATHSEGMENT *link;     /* as with any 'segment' type */
  T1_PATHSEGMENT *last;     /* as with any 'segment' type */
  T1_PATHPOINT    dest;     /* ending point (D) */
  T1_PATHPOINT    B;        /* control point B */
  T1_PATHPOINT    C;        /* control point C */
} T1_BEZIERSEGMENT;

typedef T1_PATHSEGMENT  T1_OUTLINE;


/* Two structures for handling composite character data */
/* One structure for each symbol of the composite character */
typedef struct
{
  int piece;               /* the index of the current symbol */
  int deltax;              /* horizontal displacement of current symbol in CS */ 
  int deltay;              /* vertical displacement of current symbol in CS */ 
} T1_COMP_PIECE;

/* This one defines the composite character, the number of pieces and how to
   access their data. */
typedef struct 
{
  int compchar;             /* the base character in the current encoding */
  int numPieces;            /* the number of defined pieces including the base char */
  T1_COMP_PIECE *pieces;   /* a pointer to the pieces' information */
} T1_COMP_CHAR_INFO;

