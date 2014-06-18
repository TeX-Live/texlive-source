//========================================================================
//
// SplashXPathScanner.h
//
// Copyright 2003-2013 Glyph & Cog, LLC
//
//========================================================================

#ifndef SPLASHXPATHSCANNER_H
#define SPLASHXPATHSCANNER_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "SplashTypes.h"

class GList;
class SplashXPath;

//------------------------------------------------------------------------
// SplashXPathScanner
//------------------------------------------------------------------------

class SplashXPathScanner {
public:

  // Create a new SplashXPathScanner object.  <xPathA> must be sorted.
  SplashXPathScanner(SplashXPath *xPathA, GBool eoA,
		     int yMinA, int yMaxA);

  ~SplashXPathScanner();

  // Compute shape values for a scan line.  Fills in line[] with shape
  // values for one scan line: ([x0, x1], y).  The values are in [0,
  // 255].
  void getSpan(Guchar *line, int y, int x0, int x1);

  // Like getSpan(), but uses the values 0 and 255 only.  Writes 255
  // for all pixels which include non-zero area inside the path.
  void getSpanBinary(Guchar *line, int y, int x0, int x1);

private:

  inline void addArea(Guchar *line, int x, SplashCoord a);
  void drawTrapezoid(Guchar *line, int xMin, int xMax,
		     SplashCoord y0, SplashCoord y1,
		     SplashCoord xa0, SplashCoord xa1, SplashCoord dydxa,
		     SplashCoord xb0, SplashCoord xb1, SplashCoord dydxb);
  SplashCoord areaLeft(int xp,
		       SplashCoord x0, SplashCoord y0,
		       SplashCoord x1, SplashCoord y1,
		       SplashCoord dydx);
  SplashCoord areaRight(int xp,
			SplashCoord x0, SplashCoord y0,
			SplashCoord x1, SplashCoord y1,
			SplashCoord dydx);
  void drawRectangle(Guchar *line, int xMin, int xMax,
		     SplashCoord y0, SplashCoord y1,
		     SplashCoord x0, SplashCoord x1);
  void sortActiveSegs();
  void insertActiveSeg(SplashXPathSeg *seg);

  SplashXPath *xPath;
  GBool eo;
  int yMin, yMax;

  GList *activeSegs;		// [SplashXPathSeg]
  int nextSeg;
  int yNext;
};

#endif
