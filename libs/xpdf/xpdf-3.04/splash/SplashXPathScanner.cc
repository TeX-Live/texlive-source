//========================================================================
//
// SplashXPathScanner.cc
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
#include "GList.h"
#include "SplashMath.h"
#include "SplashXPath.h"
#include "SplashXPathScanner.h"

//------------------------------------------------------------------------

#define minVertStep 0.05

//------------------------------------------------------------------------

SplashXPathScanner::SplashXPathScanner(SplashXPath *xPathA, GBool eoA,
				       int yMinA, int yMaxA) {
  xPath = xPathA;
  eo = eoA;
  yMin = yMinA;
  yMax = yMaxA;

  activeSegs = new GList();
  nextSeg = 0;
  yNext = xPath->yMin;
}

SplashXPathScanner::~SplashXPathScanner() {
  delete activeSegs;
}

void SplashXPathScanner::getSpan(Guchar *line, int y, int x0, int x1) {
  SplashXPathSeg *seg, *seg0;
  SplashCoord y0, y1, y1p;
  GBool intersect, last;
  int eoMask, state0, state1, count, i;

  //--- clear the scan line buffer
  memset(line + x0, 0, x1 - x0 + 1);

  //--- reset the path
  if (yNext != y) {
    delete activeSegs;
    activeSegs = new GList();
    nextSeg = 0;
    while (nextSeg < xPath->length) {
      seg = &xPath->segs[nextSeg];
      if (seg->y0 >= y) {
	break;
      }
      if (seg->y0 != seg->y1 && seg->y1 > y) {
	if (seg->y0 == y) {
	  seg->xCur0 = seg->x0;
	} else {
	  seg->xCur0 = seg->x0 + ((SplashCoord)y - seg->y0) * seg->dxdy;
	}
	activeSegs->append(seg);
      }
      ++nextSeg;
    }
    activeSegs->sort(&SplashXPathSeg::cmpXi);
  }

  //--- process the scan line
  y0 = y;
  while (y0 < y + 1) {

    //--- delete finished segs
    i = 0;
    while (i < activeSegs->getLength()) {
      seg = (SplashXPathSeg *)activeSegs->get(i);
      if (seg->y1 <= y0) {
	activeSegs->del(i);
      } else {
	++i;
      }
    }

    //--- check for bottom of path
    if (!activeSegs->getLength() && nextSeg >= xPath->length) {
      break;
    }

    //--- sort activeSegs
    sortActiveSegs();

    //--- add waiting segs
    while (nextSeg < xPath->length) {
      seg = &xPath->segs[nextSeg];
      if (seg->y0 > y0) {
	break;
      }
      if (seg->y0 != seg->y1) {
	seg->xCur0 = seg->x0;
	insertActiveSeg(seg);
      }
      ++nextSeg;
    }

    //--- get the next "interesting" y value
    y1 = y + 1;
    if (nextSeg < xPath->length && xPath->segs[nextSeg].y0 < y1) {
      y1 = xPath->segs[nextSeg].y0;
    }
    for (i = 0; i < activeSegs->getLength(); ++i) {
      seg = (SplashXPathSeg *)activeSegs->get(i);
      if (seg->y1 < y1) {
	y1 = seg->y1;
      }
    }

    //--- compute xCur1 values, check for intersections
    seg0 = NULL;
    intersect = gFalse;
    for (i = 0; i < activeSegs->getLength(); ++i) {
      seg = (SplashXPathSeg *)activeSegs->get(i);
      if (seg->y1 == y1) {
	seg->xCur1 = seg->x1;
      } else {
	seg->xCur1 = seg->x0 + (y1 - seg->y0) * seg->dxdy;
      }
      if (seg0 && seg0->xCur1 > seg->xCur1) {
	intersect = gTrue;
      }
      seg0 = seg;
    }

    //--- draw rectangles
    if (intersect) {
      for (; y0 < y1; y0 += minVertStep) {
	if ((y1p = y0 + minVertStep) >= y1) {
	  y1p = y1;
	  last = gTrue;
	} else {
	  last = gFalse;
	}
	state0 = state1 = count = 0;
	seg0 = NULL;
	eoMask = eo ? 1 : 0xffffffff;
	for (i = 0; i < activeSegs->getLength(); ++i) {
	  seg = (SplashXPathSeg *)activeSegs->get(i);
	  if (last && seg->y1 == y1) {
	    seg->xCur1 = seg->x1;
	  } else {
	    seg->xCur1 = seg->x0 + (y1p - seg->y0) * seg->dxdy;
	  }
	  count += seg->count;
	  state1 = count & eoMask;
	  if (!state0 && state1) {
	    seg0 = seg;
	  } else if (state0 && !state1) {
	    drawRectangle(line, x0, x1, y0, y1p, seg0->xCur0, seg->xCur0);
	  }
	  state0 = state1;
	}
	for (i = 0; i < activeSegs->getLength(); ++i) {
	  seg = (SplashXPathSeg *)activeSegs->get(i);
	  seg->xCur0 = seg->xCur1;
	}
	sortActiveSegs();
      }

    //--- draw trapezoids
    } else {
      state0 = state1 = count = 0;
      seg0 = NULL;
      eoMask = eo ? 1 : 0xffffffff;
      for (i = 0; i < activeSegs->getLength(); ++i) {
	seg = (SplashXPathSeg *)activeSegs->get(i);
	count += seg->count;
	state1 = count & eoMask;
	if (!state0 && state1) {
	  seg0 = seg;
	} else if (state0 && !state1) {
	  drawTrapezoid(line, x0, x1, y0, y1,
			seg0->xCur0, seg0->xCur1, seg0->dydx,
			seg->xCur0, seg->xCur1, seg->dydx);
	}
	state0 = state1;
      }
      for (i = 0; i < activeSegs->getLength(); ++i) {
	seg = (SplashXPathSeg *)activeSegs->get(i);
	seg->xCur0 = seg->xCur1;
      }
    }

    //--- next slice
    y0 = y1;
  }

  yNext = y + 1;
}

void SplashXPathScanner::getSpanBinary(Guchar *line, int y, int x0, int x1) {
  SplashXPathSeg *seg;
  int xx0, xx1, xx;
  int eoMask, state0, state1, count, i;

  //--- clear the scan line buffer
  memset(line + x0, 0, x1 - x0 + 1);

  //--- reset the path
  if (yNext != y) {
    delete activeSegs;
    activeSegs = new GList();
    nextSeg = 0;
    while (nextSeg < xPath->length) {
      seg = &xPath->segs[nextSeg];
      if (seg->y0 >= y) {
	break;
      }
      if (seg->y1 > y) {
	if (seg->y0 == y) {
	  seg->xCur0 = seg->x0;
	} else {
	  seg->xCur0 = seg->x0 + ((SplashCoord)y - seg->y0) * seg->dxdy;
	}
	activeSegs->append(seg);
      }
      ++nextSeg;
    }
    activeSegs->sort(&SplashXPathSeg::cmpXi);
  }

  //--- delete finished segs
  i = 0;
  while (i < activeSegs->getLength()) {
    seg = (SplashXPathSeg *)activeSegs->get(i);
    if (seg->y1 <= y) {
      activeSegs->del(i);
    } else {
      ++i;
    }
  }

  //--- sort activeSegs
  sortActiveSegs();

  //--- add waiting segs
  while (nextSeg < xPath->length) {
    seg = &xPath->segs[nextSeg];
    if (seg->y0 >= y + 1) {
      break;
    }
    seg->xCur0 = seg->x0;
    insertActiveSeg(seg);
    ++nextSeg;
  }

  //--- compute xCur1 values
  for (i = 0; i < activeSegs->getLength(); ++i) {
    seg = (SplashXPathSeg *)activeSegs->get(i);
    if (seg->y1 <= y + 1) {
      seg->xCur1 = seg->x1;
    } else {
      seg->xCur1 = seg->x0 + ((SplashCoord)(y + 1) - seg->y0) * seg->dxdy;
    }
  }

  //--- draw spans
  state0 = state1 = count = 0;
  eoMask = eo ? 1 : 0xffffffff;
  xx0 = xx1 = 0; // make gcc happy
  for (i = 0; i < activeSegs->getLength(); ++i) {
    seg = (SplashXPathSeg *)activeSegs->get(i);
    if (seg->y0 <= y && seg->y0 < seg->y1) {
      count += seg->count;
      state1 = count & eoMask;
    }
    if (state0) {
      xx = splashCeil(seg->xCur0) - 1;
      if (xx > xx1) {
	xx1 = xx;
      }
      xx = splashFloor(seg->xCur1);
      if (xx < xx0) {
	xx0 = xx;
      }
      xx = splashCeil(seg->xCur1) - 1;
      if (xx > xx1) {
	xx1 = xx;
      }
    } else {
      if (seg->xCur0 < seg->xCur1) {
	xx0 = splashFloor(seg->xCur0);
	xx1 = splashCeil(seg->xCur1) - 1;
      } else {
	xx0 = splashFloor(seg->xCur1);
	xx1 = splashCeil(seg->xCur0) - 1;
      }
    }
    if (!state1) {
      if (xx0 < x0) {
	xx0 = x0;
      }
      if (xx1 > x1) {
	xx1 = x1;
      }
      for (xx = xx0; xx <= xx1; ++xx) {
	line[xx] = 0xff;
      }
    }
    state0 = state1;
  }

  //--- update xCur0 values
  for (i = 0; i < activeSegs->getLength(); ++i) {
    seg = (SplashXPathSeg *)activeSegs->get(i);
    seg->xCur0 = seg->xCur1;
  }

  yNext = y + 1;
}

inline void SplashXPathScanner::addArea(Guchar *line, int x, SplashCoord a) {
  int a2, t;

  a2 = splashRound(a * 255);
  if (a2 <= 0) {
    return;
  }
  t = line[x] + a2;
  if (t > 255) {
    t = 255;
  }
  line[x] = t;
}

// Draw a trapezoid with edges:
//   top:    (xa0, y0) - (xb0, y0)
//   left:   (xa0, y0) - (xa1, y1)
//   right:  (xb0, y0) - (xb1, y1)
//   bottom: (xa1, y1) - (xb1, y1)
void SplashXPathScanner::drawTrapezoid(Guchar *line, int xMin, int xMax,
				       SplashCoord y0, SplashCoord y1,
				       SplashCoord xa0, SplashCoord xa1,
				       SplashCoord dydxa,
				       SplashCoord xb0, SplashCoord xb1,
				       SplashCoord dydxb) {
  SplashCoord a, dy;
  int x0, x1, x2, x3, x;

  // check for a rectangle
  if (dydxa == 0 && dydxb == 0 && xa0 >= xMin && xb0 <= xMax) {
    x0 = splashFloor(xa0);
    x3 = splashFloor(xb0);
    dy = y1 - y0;
    if (x0 == x3) {
      addArea(line, x0, (xb0 - xa0) * dy);
    } else {
      addArea(line, x0, ((SplashCoord)1 - (xa0 - x0)) * dy);
      for (x = x0 + 1; x <= x3 - 1; ++x) {
	addArea(line, x, y1 - y0);
      }
      addArea(line, x3, (xb0 - x3) * (y1 - y0));
    }
    return;
  }

  if (dydxa > 0) {
    x0 = splashFloor(xa0);
    x1 = splashFloor(xa1);
  } else {
    x0 = splashFloor(xa1);
    x1 = splashFloor(xa0);
  }
  if (x0 < xMin) {
    x0 = xMin;
  }
  if (dydxb > 0) {
    x2 = splashFloor(xb0);
    x3 = splashFloor(xb1);
  } else {
    x2 = splashFloor(xb1);
    x3 = splashFloor(xb0);
  }
  if (x3 > xMax) {
    x3 = xMax;
  }
  for (x = x0; x <= x3; ++x) {
    a = y1 - y0;
    if (x <= x1) {
      a -= areaLeft(x, xa0, y0, xa1, y1, dydxa);
    }
    if (x >= x2) {
      a -= areaRight(x, xb0, y0, xb1, y1, dydxb);
    }
    addArea(line, x, a);
  }
}

// Compute area within a pixel slice ((xp,y0)-(xp+1,y1)) to the left
// of a trapezoid edge ((x0,y0)-(x1,y1)).
SplashCoord SplashXPathScanner::areaLeft(int xp,
					 SplashCoord x0, SplashCoord y0,
					 SplashCoord x1, SplashCoord y1,
					 SplashCoord dydx) {
  SplashCoord a, ya, yb;

  if (dydx >= 0) {
    if (x0 >= xp) {
      if (x1 <= xp + 1) {
	a = ((x0 + x1) * 0.5 - xp) * (y1 - y0);
      } else {
	yb = y0 + ((SplashCoord)(xp + 1) - x0) * dydx;
	a = (y1 - y0) - ((SplashCoord)(xp + 1) - x0) * (yb - y0) * 0.5;
      }
    } else {
      if (x1 <= xp + 1) {
	ya = y0 + ((SplashCoord)xp - x0) * dydx;
	a = (x1 - xp) * (y1 - ya) * 0.5;
      } else {
	// ya = y1 - (x1 - xp - 0.5) * dydx;
	// a = y1 - ya;
	a = (x1 - xp - 0.5) * dydx;
      }
    }
  } else {
    if (x0 <= xp + 1) {
      if (x1 >= xp) {
	a = ((x0 + x1) * 0.5 - xp) * (y1 - y0);
      } else {
	ya = y0 + ((SplashCoord)xp - x0) * dydx;
	a = (x0 - xp) * (ya - y0) * 0.5;
      }
    } else {
      if (x1 >= xp) {
	yb = y0 + ((SplashCoord)(xp + 1) - x0) * dydx;
	a = (y1 - y0) - ((SplashCoord)(xp + 1) - x1) * (y1 - yb) * 0.5;
      } else {
	// ya = y0 + (xp - x0 + 0.5) * dydx;
	// a = ya - y0;
	a = ((SplashCoord)xp - x0 + 0.5) * dydx;
      }
    }
  }
  return a;
}

// Compute area within a pixel slice ((xp,y0)-(xp+1,y1)) to the left
// of a trapezoid edge ((x0,y0)-(x1,y1)).
SplashCoord SplashXPathScanner::areaRight(int xp,
					  SplashCoord x0, SplashCoord y0,
					  SplashCoord x1, SplashCoord y1,
					  SplashCoord dydx) {
  SplashCoord a, ya, yb;

  if (dydx >= 0) {
    if (x0 >= xp) {
      if (x1 <= xp + 1) {
	a = ((SplashCoord)(xp + 1) - (x0 + x1) * 0.5) * (y1 - y0);
      } else {
	yb = y0 + ((SplashCoord)(xp + 1) - x0) * dydx;
	a = ((SplashCoord)(xp + 1) - x0) * (yb - y0) * 0.5;
      }
    } else {
      if (x1 <= xp + 1) {
	ya = y0 + ((SplashCoord)xp - x0) * dydx;
	a = (y1 - y0) - (x1 - xp) * (y1 - ya) * 0.5;
      } else {
	// ya = y0 + (xp - x0 + 0.5) * dydx;
	// a = ya - y0;
	a = ((SplashCoord)xp + 0.5 - x0) * dydx;
      }
    }
  } else {
    if (x0 <= xp + 1) {
      if (x1 >= xp) {
	a = ((SplashCoord)(xp + 1) - (x0 + x1) * 0.5) * (y1 - y0);
      } else {
	ya = y0 + ((SplashCoord)xp - x0) * dydx;
	a = (y1 - y0) - (x0 - xp) * (ya - y0) * 0.5;
      }
    } else {
      if (x1 >= xp) {
	yb = y0 + ((SplashCoord)(xp + 1) - x0) * dydx;
	a = ((SplashCoord)(xp + 1) - x1) * (y1 - yb) * 0.5;
      } else {
	// ya = y1 - (x1 - xp - 0.5) * dydx;
	// a = y1 - ya;
	a = (x1 - xp - 0.5) * dydx;
      }
    }
  }
  return a;
}

void SplashXPathScanner::drawRectangle(Guchar *line, int xMin, int xMax,
				       SplashCoord y0, SplashCoord y1,
				       SplashCoord x0, SplashCoord x1) {
  SplashCoord dy, a;
  int xx0, xx1, x;

  xx0 = splashFloor(x0);
  if (xx0 < xMin) {
    xx0 = xMin;
  }
  xx1 = splashFloor(x1);
  if (xx1 > xMax) {
    xx1 = xMax;
  }
  dy = y1 - y0;
  for (x = xx0; x <= xx1; ++x) {
    a = dy;
    if ((SplashCoord)x < x0) {
      a -= (x0 - x) * dy;
    }
    if ((SplashCoord)(x + 1) > x1) {
      a -= ((SplashCoord)(x + 1) - x1) * dy;
    }
    addArea(line, x, a);
  }
}

void SplashXPathScanner::sortActiveSegs() {
  SplashXPathSeg *seg0, *seg1;
  int i, j, k;

  if (activeSegs->getLength() < 2) {
    return;
  }
  seg0 = (SplashXPathSeg *)activeSegs->get(0);
  for (i = 1; i < activeSegs->getLength(); ++i) {
    seg1 = (SplashXPathSeg *)activeSegs->get(i);
    if (SplashXPathSeg::cmpX(seg0, seg1) > 0) {
      for (j = i - 1; j > 0; --j) {
	if (SplashXPathSeg::cmpX((SplashXPathSeg *)activeSegs->get(j - 1),
				 seg1) <= 0) {
	  break;
	}
      }
      for (k = i; k > j; --k) {
	activeSegs->put(k, activeSegs->get(k-1));
      }
      activeSegs->put(j, seg1);
    } else {
      seg0 = seg1;
    }
  }
}

void SplashXPathScanner::insertActiveSeg(SplashXPathSeg *seg) {
  int i;

  for (i = 0; i < activeSegs->getLength(); ++i) {
    if (SplashXPathSeg::cmpX(seg, (SplashXPathSeg *)activeSegs->get(i)) < 0) {
      break;
    }
  }
  activeSegs->insert(i, seg);
}
