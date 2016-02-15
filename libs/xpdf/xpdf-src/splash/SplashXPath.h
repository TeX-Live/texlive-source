//========================================================================
//
// SplashXPath.h
//
// Copyright 2003-2013 Glyph & Cog, LLC
//
//========================================================================

#ifndef SPLASHXPATH_H
#define SPLASHXPATH_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "SplashTypes.h"

class SplashPath;
struct SplashXPathPoint;
struct SplashPathHint;

//------------------------------------------------------------------------

#define splashMaxCurveSplits (1 << 10)

//------------------------------------------------------------------------
// SplashXPathSeg
//------------------------------------------------------------------------

struct SplashXPathSeg {
  SplashCoord x0, y0;		// first endpoint (y0 <= y1)
  SplashCoord x1, y1;		// second endpoint
  SplashCoord dxdy;		// slope: delta-x / delta-y
  SplashCoord dydx;		// slope: delta-y / delta-x
  int count;			// EO/NZWN counter increment

  //----- used by SplashXPathScanner
  SplashCoord xCur0, xCur1;	// current x values

#if HAVE_STD_SORT
  static bool cmpY(const SplashXPathSeg &seg0,
		   const SplashXPathSeg &seg1) {
    return seg0.y0 < seg1.y0;
  }
#else
  static int cmpY(const void *seg0, const void *seg1) {
    SplashCoord cmp;

    cmp = ((SplashXPathSeg *)seg0)->y0
          - ((SplashXPathSeg *)seg1)->y0;
    return (cmp > 0) ? 1 : (cmp < 0) ? -1 : 0;
  }
#endif

  static int cmpX(SplashXPathSeg *seg0, SplashXPathSeg *seg1) {
    SplashCoord cmp;

    if ((cmp = seg0->xCur0 - seg1->xCur0) == 0) {
      cmp = seg0->dxdy - seg1->dxdy;
    }
    return (cmp > 0) ? 1 : (cmp < 0) ? -1 : 0;
  }

  static int cmpXi(const void *p0, const void *p1) {
    return cmpX(*(SplashXPathSeg **)p0, *(SplashXPathSeg **)p1);
  }

};

//------------------------------------------------------------------------
// SplashXPath
//------------------------------------------------------------------------

class SplashXPath {
public:

  // Expands (converts to segments) and flattens (converts curves to
  // lines) <path>.  Transforms all points from user space to device
  // space, via <matrix>.  If <closeSubpaths> is true, closes all open
  // subpaths.
  SplashXPath(SplashPath *path, SplashCoord *matrix,
	      SplashCoord flatness, GBool closeSubpaths);

  // Copy an expanded path.
  SplashXPath *copy() { return new SplashXPath(this); }

  ~SplashXPath();

  int getXMin() { return xMin; }
  int getXMax() { return xMax; }
  int getYMin() { return yMin; }
  int getYMax() { return yMax; }

private:

  SplashXPath(SplashXPath *xPath);
  void transform(SplashCoord *matrix, SplashCoord xi, SplashCoord yi,
		 SplashCoord *xo, SplashCoord *yo);
  void strokeAdjust(SplashXPathPoint *pts,
		    SplashPathHint *hints, int nHints);
  void grow(int nSegs);
  void addCurve(SplashCoord x0, SplashCoord y0,
		SplashCoord x1, SplashCoord y1,
		SplashCoord x2, SplashCoord y2,
		SplashCoord x3, SplashCoord y3,
		SplashCoord flatness,
		GBool first, GBool last, GBool end0, GBool end1);
  void addSegment(SplashCoord x0, SplashCoord y0,
		  SplashCoord x1, SplashCoord y1);

  SplashXPathSeg *segs;
  int length, size;		// length and size of segs array
  int xMin, xMax;
  int yMin, yMax;

  friend class SplashXPathScanner;
  friend class SplashClip;
  friend class Splash;
};

#endif
