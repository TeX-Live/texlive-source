//========================================================================
//
// SplashXPath.cc
//
// Copyright 2003-2013 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <stdlib.h>
#include <string.h>
#if HAVE_STD_SORT
#include <algorithm>
#endif
#include "gmem.h"
#include "SplashMath.h"
#include "SplashPath.h"
#include "SplashXPath.h"

//------------------------------------------------------------------------

struct SplashXPathPoint {
  SplashCoord x, y;
};

struct SplashXPathAdjust {
  int firstPt, lastPt;		// range of points
  GBool vert;			// vertical or horizontal hint
  SplashCoord x0a, x0b,		// hint boundaries
              xma, xmb,
              x1a, x1b;
  SplashCoord x0, x1, xm;	// adjusted coordinates
};

//------------------------------------------------------------------------

// Transform a point from user space to device space.
inline void SplashXPath::transform(SplashCoord *matrix,
				   SplashCoord xi, SplashCoord yi,
				   SplashCoord *xo, SplashCoord *yo) {
  //                          [ m[0] m[1] 0 ]
  // [xo yo 1] = [xi yi 1] *  [ m[2] m[3] 0 ]
  //                          [ m[4] m[5] 1 ]
  *xo = xi * matrix[0] + yi * matrix[2] + matrix[4];
  *yo = xi * matrix[1] + yi * matrix[3] + matrix[5];
}

//------------------------------------------------------------------------
// SplashXPath
//------------------------------------------------------------------------

SplashXPath::SplashXPath(SplashPath *path, SplashCoord *matrix,
			 SplashCoord flatness, GBool closeSubpaths) {
  SplashXPathPoint *pts;
  SplashCoord x0, y0, x1, y1, x2, y2, x3, y3, xsp, ysp;
  SplashCoord xMinFP, xMaxFP, yMinFP, yMaxFP;
  int curSubpath, i;

  // transform the points
  pts = (SplashXPathPoint *)gmallocn(path->length, sizeof(SplashXPathPoint));
  for (i = 0; i < path->length; ++i) {
    transform(matrix, path->pts[i].x, path->pts[i].y, &pts[i].x, &pts[i].y);
  }

  // do stroke adjustment
  if (path->hints) {
    strokeAdjust(pts, path->hints, path->hintsLength);
  }

  segs = NULL;
  length = size = 0;

  x0 = y0 = xsp = ysp = 0; // make gcc happy
  curSubpath = 0;
  i = 0;
  while (i < path->length) {

    // first point in subpath - skip it
    if (path->flags[i] & splashPathFirst) {
      x0 = pts[i].x;
      y0 = pts[i].y;
      xsp = x0;
      ysp = y0;
      curSubpath = i;
      ++i;

    } else {

      // curve segment
      if (path->flags[i] & splashPathCurve) {
	x1 = pts[i].x;
	y1 = pts[i].y;
	x2 = pts[i+1].x;
	y2 = pts[i+1].y;
	x3 = pts[i+2].x;
	y3 = pts[i+2].y;
	addCurve(x0, y0, x1, y1, x2, y2, x3, y3,
		 flatness,
		 (path->flags[i-1] & splashPathFirst),
		 (path->flags[i+2] & splashPathLast),
		 !closeSubpaths &&
		   (path->flags[i-1] & splashPathFirst) &&
		   !(path->flags[i-1] & splashPathClosed),
		 !closeSubpaths &&
		   (path->flags[i+2] & splashPathLast) &&
		   !(path->flags[i+2] & splashPathClosed));
	x0 = x3;
	y0 = y3;
	i += 3;

      // line segment
      } else {
	x1 = pts[i].x;
	y1 = pts[i].y;
	addSegment(x0, y0, x1, y1);
	x0 = x1;
	y0 = y1;
	++i;
      }

      // close a subpath
      if (closeSubpaths &&
	  (path->flags[i-1] & splashPathLast) &&
	  (pts[i-1].x != pts[curSubpath].x ||
	   pts[i-1].y != pts[curSubpath].y)) {
	addSegment(x0, y0, xsp, ysp);
      }
    }
  }

  gfree(pts);

#if HAVE_STD_SORT
  std::sort(segs, segs + length, SplashXPathSeg::cmpY);
#else
  qsort(segs, length, sizeof(SplashXPathSeg), &SplashXPathSeg::cmpY);
#endif

  if (length == 0) {
    xMin = yMin = xMax = yMax = 0;
  } else {
    if (segs[0].x0 < segs[0].x1) {
      xMinFP = segs[0].x0;
      xMaxFP = segs[0].x1;
    } else {
      xMinFP = segs[0].x1;
      xMaxFP = segs[0].x0;
    }
    yMinFP = segs[0].y0;
    yMaxFP = segs[0].y1;
    for (i = 1; i < length; ++i) {
      if (segs[i].x0 < xMinFP) {
	xMinFP = segs[i].x0;
      } else if (segs[i].x0 > xMaxFP) {
	xMaxFP = segs[i].x0;
      }
      if (segs[i].x1 < xMinFP) {
	xMinFP = segs[i].x1;
      } else if (segs[i].x1 > xMaxFP) {
	xMaxFP = segs[i].x1;
      }
      if (segs[i].y1 > yMaxFP) {
	yMaxFP = segs[i].y1;
      }
    }
    xMin = splashFloor(xMinFP);
    yMin = splashFloor(yMinFP);
    xMax = splashFloor(xMaxFP);
    yMax = splashFloor(yMaxFP);
  }
}

void SplashXPath::strokeAdjust(SplashXPathPoint *pts,
			       SplashPathHint *hints, int nHints) {
  SplashXPathAdjust *adjusts, *adjust;
  SplashPathHint *hint;
  SplashCoord x0, y0, x1, y1, x2, y2, x3, y3;
  SplashCoord adj0, adj1, d;
  int xi0, xi1;
  int i, j;

  // set up the stroke adjustment hints
  adjusts = (SplashXPathAdjust *)gmallocn(nHints, sizeof(SplashXPathAdjust));
  for (i = 0; i < nHints; ++i) {
    hint = &hints[i];
    x0 = pts[hint->ctrl0    ].x;    y0 = pts[hint->ctrl0    ].y;
    x1 = pts[hint->ctrl0 + 1].x;    y1 = pts[hint->ctrl0 + 1].y;
    x2 = pts[hint->ctrl1    ].x;    y2 = pts[hint->ctrl1    ].y;
    x3 = pts[hint->ctrl1 + 1].x;    y3 = pts[hint->ctrl1 + 1].y;
    if (x0 == x1 && x2 == x3) {
      adjusts[i].vert = gTrue;
      adj0 = x0;
      adj1 = x2;
    } else if (y0 == y1 && y2 == y3) {
      adjusts[i].vert = gFalse;
      adj0 = y0;
      adj1 = y2;
    } else {
      goto done;
    }
    if (adj0 > adj1) {
      x0 = adj0;
      adj0 = adj1;
      adj1 = x0;
    }
    d = adj1 - adj0;
    if (d > 0.04) {
      d = 0.01;
    } else {
      d *= 0.25;
    }
    adjusts[i].x0a = adj0 - d;
    adjusts[i].x0b = adj0 + d;
    adjusts[i].xma = (SplashCoord)0.5 * (adj0 + adj1) - d;
    adjusts[i].xmb = (SplashCoord)0.5 * (adj0 + adj1) + d;
    adjusts[i].x1a = adj1 - d;
    adjusts[i].x1b = adj1 + d;
    splashStrokeAdjust(adj0, adj1, &xi0, &xi1);
    adjusts[i].x0 = (SplashCoord)xi0;
    // the "minus epsilon" thing here is needed when vector
    // antialiasing is turned off -- otherwise stroke adjusted lines
    // will touch an extra pixel on one edge
    adjusts[i].x1 = (SplashCoord)xi1 - 0.001;
    adjusts[i].xm = (SplashCoord)0.5 * (adjusts[i].x0 + adjusts[i].x1);
    adjusts[i].firstPt = hint->firstPt;
    adjusts[i].lastPt = hint->lastPt;
  }

  // perform stroke adjustment
  for (i = 0, adjust = adjusts; i < nHints; ++i, ++adjust) {
    for (j = adjust->firstPt; j <= adjust->lastPt; ++j) {
      if (adjust->vert) {
	x0 = pts[j].x;
	if (x0 > adjust->x0a && x0 < adjust->x0b) {
	  pts[j].x = adjust->x0;
	} else if (x0 > adjust->xma && x0 < adjust->xmb) {
	  pts[j].x = adjust->xm;
	} else if (x0 > adjust->x1a && x0 < adjust->x1b) {
	  pts[j].x = adjust->x1;
	}
      } else {
	y0 = pts[j].y;
	if (y0 > adjust->x0a && y0 < adjust->x0b) {
	  pts[j].y = adjust->x0;
	} else if (y0 > adjust->xma && y0 < adjust->xmb) {
	  pts[j].y = adjust->xm;
	} else if (y0 > adjust->x1a && y0 < adjust->x1b) {
	  pts[j].y = adjust->x1;
	}
      }
    }
  }

 done:
  gfree(adjusts);
}

SplashXPath::SplashXPath(SplashXPath *xPath) {
  length = xPath->length;
  size = xPath->size;
  segs = (SplashXPathSeg *)gmallocn(size, sizeof(SplashXPathSeg));
  memcpy(segs, xPath->segs, length * sizeof(SplashXPathSeg));
  xMin = xPath->xMin;
  yMin = xPath->yMin;
  xMax = xPath->xMax;
  yMax = xPath->yMax;
}

SplashXPath::~SplashXPath() {
  gfree(segs);
}

// Add space for <nSegs> more segments
void SplashXPath::grow(int nSegs) {
  if (length + nSegs > size) {
    if (size == 0) {
      size = 32;
    }
    while (size < length + nSegs) {
      size *= 2;
    }
    segs = (SplashXPathSeg *)greallocn(segs, size, sizeof(SplashXPathSeg));
  }
}

void SplashXPath::addCurve(SplashCoord x0, SplashCoord y0,
			   SplashCoord x1, SplashCoord y1,
			   SplashCoord x2, SplashCoord y2,
			   SplashCoord x3, SplashCoord y3,
			   SplashCoord flatness,
			   GBool first, GBool last, GBool end0, GBool end1) {
  SplashCoord cx[splashMaxCurveSplits + 1][3];
  SplashCoord cy[splashMaxCurveSplits + 1][3];
  int cNext[splashMaxCurveSplits + 1];
  SplashCoord xl0, xl1, xl2, xr0, xr1, xr2, xr3, xx1, xx2, xh;
  SplashCoord yl0, yl1, yl2, yr0, yr1, yr2, yr3, yy1, yy2, yh;
  SplashCoord dx, dy, mx, my, d1, d2, flatness2;
  int p1, p2, p3;

#if USE_FIXEDPOINT
  flatness2 = flatness;
#else
  flatness2 = flatness * flatness;
#endif

  // initial segment
  p1 = 0;
  p2 = splashMaxCurveSplits;
  cx[p1][0] = x0;  cy[p1][0] = y0;
  cx[p1][1] = x1;  cy[p1][1] = y1;
  cx[p1][2] = x2;  cy[p1][2] = y2;
  cx[p2][0] = x3;  cy[p2][0] = y3;
  cNext[p1] = p2;

  while (p1 < splashMaxCurveSplits) {

    // get the next segment
    xl0 = cx[p1][0];  yl0 = cy[p1][0];
    xx1 = cx[p1][1];  yy1 = cy[p1][1];
    xx2 = cx[p1][2];  yy2 = cy[p1][2];
    p2 = cNext[p1];
    xr3 = cx[p2][0];  yr3 = cy[p2][0];

    // compute the distances from the control points to the
    // midpoint of the straight line (this is a bit of a hack, but
    // it's much faster than computing the actual distances to the
    // line)
    mx = (xl0 + xr3) * 0.5;
    my = (yl0 + yr3) * 0.5;
#if USE_FIXEDPOINT
    d1 = splashDist(xx1, yy1, mx, my);
    d2 = splashDist(xx2, yy2, mx, my);
#else
    dx = xx1 - mx;
    dy = yy1 - my;
    d1 = dx*dx + dy*dy;
    dx = xx2 - mx;
    dy = yy2 - my;
    d2 = dx*dx + dy*dy;
#endif

    // if the curve is flat enough, or no more subdivisions are
    // allowed, add the straight line segment
    if (p2 - p1 == 1 || (d1 <= flatness2 && d2 <= flatness2)) {
      addSegment(xl0, yl0, xr3, yr3);
      p1 = p2;

    // otherwise, subdivide the curve
    } else {
      xl1 = (xl0 + xx1) * 0.5;
      yl1 = (yl0 + yy1) * 0.5;
      xh = (xx1 + xx2) * 0.5;
      yh = (yy1 + yy2) * 0.5;
      xl2 = (xl1 + xh) * 0.5;
      yl2 = (yl1 + yh) * 0.5;
      xr2 = (xx2 + xr3) * 0.5;
      yr2 = (yy2 + yr3) * 0.5;
      xr1 = (xh + xr2) * 0.5;
      yr1 = (yh + yr2) * 0.5;
      xr0 = (xl2 + xr1) * 0.5;
      yr0 = (yl2 + yr1) * 0.5;
      // add the new subdivision points
      p3 = (p1 + p2) / 2;
      cx[p1][1] = xl1;  cy[p1][1] = yl1;
      cx[p1][2] = xl2;  cy[p1][2] = yl2;
      cNext[p1] = p3;
      cx[p3][0] = xr0;  cy[p3][0] = yr0;
      cx[p3][1] = xr1;  cy[p3][1] = yr1;
      cx[p3][2] = xr2;  cy[p3][2] = yr2;
      cNext[p3] = p2;
    }
  }
}

void SplashXPath::addSegment(SplashCoord x0, SplashCoord y0,
			     SplashCoord x1, SplashCoord y1) {
  grow(1);
  if (y0 <= y1) {
    segs[length].x0 = x0;
    segs[length].y0 = y0;
    segs[length].x1 = x1;
    segs[length].y1 = y1;
    segs[length].count = 1;
  } else {
    segs[length].x0 = x1;
    segs[length].y0 = y1;
    segs[length].x1 = x0;
    segs[length].y1 = y0;
    segs[length].count = -1;
  }
#if USE_FIXEDPOINT
  if (y0 == y1 || x0 == x1 ||
      !FixedPoint::divCheck(x1 - x0, y1 - y0, &segs[length].dxdy) ||
      !FixedPoint::divCheck(y1 - y0, x1 - x0, &segs[length].dydx)) {
    segs[length].dxdy = 0;
    segs[length].dydx = 0;
  }
#else
  if (y0 == y1 || x0 == x1) {
    segs[length].dxdy = 0;
    segs[length].dydx = 0;
  } else {
    segs[length].dxdy = (x1 - x0) / (y1 - y0);
    if (segs[length].dxdy == 0) {
      segs[length].dydx = 0;
    } else {
      segs[length].dydx = 1 / segs[length].dxdy;
    }
  }
#endif
  ++length;
}
