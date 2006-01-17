/*========================================================================*\

Copyright (c) 1990-1999  Paul Vojta

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
PAUL VOJTA BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

NOTE:
	This module is based on prior work as noted below.

\*========================================================================*/

/*
 * Support drawing routines for TeXsun and TeX
 *
 *      Copyright, (C) 1987, 1988 Tim Morgan, UC Irvine
 *	Adapted for xdvi by Jeffrey Lee, U. of Toronto
 *
 * At the time these routines are called, the values of hh and vv should
 * have been updated to the upper left corner of the graph (the position
 * the \special appears at in the dvi file).  Then the coordinates in the
 * graphics commands are in terms of a virtual page with axes oriented the
 * same as the Imagen and the SUN normally have:
 *
 *                      0,0
 *                       +-----------> +x
 *                       |
 *                       |
 *                       |
 *                      \ /
 *                       +y
 *
 * Angles are measured in the conventional way, from +x towards +y.
 * Unfortunately, that reverses the meaning of "counterclockwise"
 * from what it's normally thought of.
 *
 * A lot of floating point arithmetic has been converted to integer
 * arithmetic for speed.  In some places, this is kind-of kludgy, but
 * it's worth it.
 */

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN 1
#include "wingui.h"
#endif
#undef HAVE_BOOLEAN
#include "xdvi-config.h"
#include <kpathsea/c-fopen.h>
#include <kpathsea/c-ctype.h>
#include <kpathsea/c-pathmx.h>
#include <kpathsea/c-stat.h>
#include <kpathsea/line.h>
#include <kpathsea/tex-file.h>
#ifndef S_IRUSR
#define S_IRUSR 0400
#endif
#ifndef S_IWUSR
#define S_IWUSR 0200
#endif
#include "dvi.h" // added SU

#ifdef WIN32
#include <zlib.h>
#include <bzlib.h>
#else
# if HAVE_SYS_WAIT_H   
#  include <sys/wait.h> 
# endif 
# ifndef WEXITSTATUS
#  define WEXITSTATUS(stat_val) ((unsigned)(stat_val) >> 8)
# endif
# ifndef WIFEXITED
#  define WIFEXITED(stat_val) (((stat_val) & 255) == 0)
# endif
extern	char	*tempnam ARGS((_Xconst char *, _Xconst char *));
extern	double	floor ARGS((double));
#endif

#define	MAXPOINTS	300	/* Max points in a path */
#define	MAX_PEN_SIZE	72	/* Max pixels of pen width */
#define	TWOPI		(3.14159265359 * 2.0)

#define	rint(x)	floor((x) + 0.5)

static double  shadetp = 0.5;
             /* shading level, initialized as requested by tpic 2.0 -- MJ */

static	int	xx[MAXPOINTS], yy[MAXPOINTS];	/* Path in milli-inches */
static	int	path_len = 0;	/* # points in current path */
/* static */	int	pen_size = 1;	/* Pixel width of lines drawn */

static	Boolean	whiten = False;
static	Boolean	shade = False;
static	Boolean	blacken = False;
static  Boolean begin_path = False;

/* Unfortunately, these values also appear in dvisun.c */
#define	xRESOLUTION	(pixels_per_inch/shrink_factor)
#define	yRESOLUTION	(pixels_per_inch/shrink_factor)

# if HAVE_VFORK_H
#  include <vfork.h>
# endif

#define	CMD(x, y)	((x) << 8 | (y))

/*
 *	Issue warning messages
 */

static	void
Warning(fmt, msg)
	char	*fmt, *msg;
{
	Fprintf(stderr, "%s: ", prog);
	Fprintf(stderr, fmt, msg);
	(void) fputc('\n', stderr);
}


/*
 *	X drawing routines
 */

#define	toint(x)	((int) ((x) + 0.5))
#define	xconv(x)	(toint(tpic_conv*(x))/shrink_factor + PXL_H)
#define	yconv(y)	(toint(tpic_conv*(y))/shrink_factor + PXL_V)

/*
 *	Draw a line from (fx,fy) to (tx,ty).
 *	Right now, we ignore pen_size.
 */
static	void
line_btw(fx, fy, tx, ty)
int fx, fy, tx, ty;
{
	int	fcx = xconv(fx),
			tcx = xconv(tx),
			fcy = yconv(fy),
			tcy = yconv(ty);

	HPEN oldPen;
	oldPen = SelectObject(ruleGC, foreTPicPen);

	if ((fcx < max_x || tcx < max_x) && (fcx >= min_x || tcx >= min_x) &&
	    (fcy < max_y || tcy < max_y) && (fcy >= min_y || tcy >= min_y))
		XDrawLine(DISP, currwin.win, ruleGC,
		    fcx - currwin.base_x, fcy - currwin.base_y,
		    tcx - currwin.base_x, tcy - currwin.base_y);

	SelectObject(ruleGC, oldPen);
}

/*
 *	Draw a dot at (x,y)
 */
static	void
dot_at(x, y)
	int	x, y;
{
	int	cx = xconv(x),
			cy = yconv(y);
	HPEN oldPen;
	oldPen = SelectObject(ruleGC, foreTPicPen);

	if (cx < max_x && cx >= min_x && cy < max_y && cy >= min_y)
	    XDrawPoint(DISP, currwin.win, ruleGC,
		cx - currwin.base_x, cy - currwin.base_y);

	SelectObject(ruleGC, oldPen);

}

/*
 *	Apply the requested attributes to the last path (box) drawn.
 *	Attributes are reset.
 *	(Not currently implemented.)
 */
	/* ARGSUSED */
static	void
do_attribute_path(last_min_x, last_max_x, last_min_y, last_max_y)
int last_min_x, last_max_x, last_min_y, last_max_y;
{
  HPEN oldPen;
  HBRUSH oldBrush;

  if (!begin_path) {
    Warning("%s", "Tpic specials: no Beginpath encountered!\n");
    return;
  }

  begin_path = False;
#if DEBUG_TPIC
  fprintf(stderr, "Flushing path\n");
#endif
  if (EndPath(ruleGC) == 0) {
    Win32Error("do_attribute_path/EndPath");
    return;
  }

  oldPen = SelectObject(ruleGC, foreTPicPen);
  oldBrush = SelectObject(ruleGC, foreTPicBrush);

  if (blacken || whiten || shade) {
#if DEBUG_TPIC
    fprintf(stderr, "Filling ...\n");
#endif
    blacken = whiten = shade = False;
    if (StrokeAndFillPath(ruleGC) == 0) {
      Win32Error("do_attribute_path/StrokeAndFillPath");
    }
  }
  else {
#if DEBUG_TPIC
    fprintf(stderr, "Empty ...\n");
#endif
    if (StrokePath(ruleGC) == 0) {
      Win32Error("do_attribute_path/StrokePath");
    }
  }

  SelectObject(ruleGC, oldPen);
  SelectObject(ruleGC, oldBrush);
}

/*
 *	Set the size of the virtual pen used to draw in milli-inches
 */

/* ARGSUSED */
static	void
set_pen_size(cp)
	char	*cp;
{
	int	ps;
	HPEN newForePen, newBackPen;

	if (sscanf(cp, " %d ", &ps) != 1) {
	    Warning("invalid .ps command format: %s", cp);
	    return;
	}
	pen_size = ((float)ps * ((float)pixels_per_inch / 1000.0) + 0.5) / shrink_factor;

#if DEBUG_TPIC
	fprintf(stderr, "Pen size is %d\n", pen_size);
#endif
	if (pen_size < 1) pen_size = 1;
	else if (pen_size > MAX_PEN_SIZE) pen_size = MAX_PEN_SIZE;
	if (foreTPicPen)
	  if (!DeleteObject(foreTPicPen))
	    Win32Error("set_pen_size/DeleteObject/foreTPicPen");
	foreTPicPen = CreatePen(PS_SOLID, pen_size, fore_Pixel);
	if (backTPicPen)
	  if (!DeleteObject(backTPicPen))
	    Win32Error("set_pen_size/DeleteObject/backTPicPen");
	backTPicPen = CreatePen(PS_NULL, pen_size, back_Pixel);
#if DEBUG_TPIC
	fprintf(stderr, "Pen size set to %d points\n", pen_size);
#endif
}


/*
 *	Print the line defined by previous path commands
 */

static	void
flush_path()
{
	int i;
	int	last_min_x, last_max_x, last_min_y, last_max_y;

#if 0
	HPEN oldPen;
	HBRUSH oldBrush;

	oldPen = SelectObject(ruleGC, foreTPicPen);
	oldBrush = SelectObject(ruleGC, foreTPicBrush);
#endif

	last_min_x = 30000;
	last_min_y = 30000;
	last_max_x = -30000;
	last_max_y = -30000;
	for (i = 1; i < path_len; i++) {
	    if (xx[i] > last_max_x) last_max_x = xx[i];
	    if (xx[i] < last_min_x) last_min_x = xx[i];
	    if (yy[i] > last_max_y) last_max_y = yy[i];
	    if (yy[i] < last_min_y) last_min_y = yy[i];
	    line_btw(xx[i], yy[i], xx[i+1], yy[i+1]);
	}
	if (xx[path_len] > last_max_x) last_max_x = xx[path_len];
	if (xx[path_len] < last_min_x) last_min_x = xx[path_len];
	if (yy[path_len] > last_max_y) last_max_y = yy[path_len];
	if (yy[path_len] < last_min_y) last_min_y = yy[path_len];
	path_len = 0;
	do_attribute_path(last_min_x, last_max_x, last_min_y, last_max_y);

#if 0
	SelectObject(ruleGC, oldPen);
	SelectObject(ruleGC, oldBrush);
#endif
}


/*
 *	Print a dashed line along the previously defined path, with
 *	the dashes/inch defined.
 */

static	void
flush_dashed(cp, dotted)
	char	*cp;
	Boolean	dotted;
{
	int	i, k;
	int	numdots;
	int	lx0, ly0, lx1, ly1;
	int	cx0, cy0, cx1, cy1;
	float	inchesperdash;
	double	d, spacesize, a, b, dx, dy, milliperdash;
	int	last_min_x, last_max_x, last_min_y, last_max_y;
	HPEN oldPen;
	
	if (sscanf(cp, " %f ", &inchesperdash) != 1) {
	    Warning("invalid format for dotted/dashed line: %s", cp);
	    return;
	}
	if (path_len <= 1 || inchesperdash <= 0.0) {
	    Warning("invalid conditions for dotted/dashed line", "");
	    return;
	}

	if (begin_path) {
	  begin_path = False;
	  EndPath(ruleGC);
	}

	oldPen = SelectObject(ruleGC, backTPicPen);

	if (whiten || blacken || shade) {
	  /* FIXME : add checks to win32 calls ! */
	  BeginPath(ruleGC);
	  for (i = 1; i < path_len; i++) {
	    line_btw(xx[i], yy[i], xx[i+1], yy[i+1]);
	  }
	  EndPath(ruleGC);
	  FillPath(ruleGC);
	  whiten = blacken = shade = False;

	}

	SelectObject(ruleGC, foreTPicPen);

	milliperdash = inchesperdash * 1000.0;

	for (k = 1; k < path_len; k++) {
	  lx0 = xx[k];	ly0 = yy[k];
	  lx1 = xx[k+1];ly1 = yy[k+1];
	  dx = lx1 - lx0;
	  dy = ly1 - ly0;
	  if (dotted) {
	    numdots = sqrt(dx*dx + dy*dy) / milliperdash + 0.5;
	    if (numdots == 0) numdots = 1;
	    for (i = 0; i <= numdots; i++) {
	      a = (float) i / (float) numdots;
	      cx0 = lx0 + a * dx + 0.5;
	      cy0 = ly0 + a * dy + 0.5;
	      dot_at(cx0, cy0);
	    }
	  }
	  else {
	    d = sqrt(dx*dx + dy*dy);
	    numdots = d / (2.0 * milliperdash) + 1.0;
	    if (numdots <= 1)
	      line_btw(lx0, ly0, lx1, ly1);
	    else {
	      spacesize = (d - numdots * milliperdash) / (numdots - 1);
	      for (i = 0; i < numdots - 1; i++) {
		a = i * (milliperdash + spacesize) / d;
		b = a + milliperdash / d;
		cx0 = lx0 + a * dx + 0.5;
		    cy0 = ly0 + a * dy + 0.5;
		    cx1 = lx0 + b * dx + 0.5;
		    cy1 = ly0 + b * dy + 0.5;
		    line_btw(cx0, cy0, cx1, cy1);
		    b += spacesize / d;
	      }
	      cx0 = lx0 + b * dx + 0.5;
	      cy0 = ly0 + b * dy + 0.5;
	      line_btw(cx0, cy0, lx1, ly1);
	    }
	  }
	}
	path_len = 0;	  

	SelectObject(ruleGC, oldPen);
}


/*
 *	Add a point to the current path
 */

static	void
add_path(cp)
	char	*cp;
{
	int	pathx, pathy;
	HPEN oldPen;

#if 0
	oldPen = SelectObject(ruleGC, foreTPicPen);
#endif
	if (path_len == 0 && !begin_path) {
	  BeginPath(ruleGC);
	  begin_path = True;
	}

	if (++path_len >= MAXPOINTS) oops("Too many points");
	if (sscanf(cp, " %d %d ", &pathx, &pathy) != 2)
	    oops("Malformed path command");
	xx[path_len] = pathx;
	yy[path_len] = pathy;

#if 0
	SelectObject(ruleGC, oldPen);
#endif
}


/*
 *	Draw to a floating point position
 */

static void
im_fdraw(x, y)
	double	x,y;
{
	if (++path_len >= MAXPOINTS) oops("Too many arc points");
	xx[path_len] = x + 0.5;
	yy[path_len] = y + 0.5;
}


/*
 *	Draw an ellipse with the indicated center and radices.
 */

static	void
draw_ellipse(xc, yc, xr, yr)
	int	xc, yc, xr, yr;
{
	double	angle, theta;
	int	n;
	int	px0, py0, px1, py1;
	HPEN oldPen;

	oldPen = SelectObject(ruleGC, foreTPicPen);

	angle = (xr + yr) / 2.0;
	theta = sqrt(1.0 / angle);
	n = TWOPI / theta + 0.5;
	if (n < 12) n = 12;
	else if (n > 80) n = 80;
	n /= 2;
	theta = TWOPI / n;

	angle = 0.0;
	px0 = xc + xr;		/* cos(0) = 1 */
	py0 = yc;		/* sin(0) = 0 */
	while ((angle += theta) <= TWOPI) {
	    px1 = xc + xr*cos(angle) + 0.5;
	    py1 = yc + yr*sin(angle) + 0.5;
	    line_btw(px0, py0, px1, py1);
	    px0 = px1;
	    py0 = py1;
	}
	line_btw(px0, py0, xc + xr, yc);
	flush_path();

	SelectObject(ruleGC, oldPen);
}

/*
 *	Draw an arc
 */

static	void
arc(cp, invis)
	char	*cp;
	Boolean	invis;
{
	int	xc, yc, xrad, yrad, n;
	float	start_angle, end_angle, angle, theta, r;
	double	xradius, yradius, xcenter, ycenter;
	HPEN oldPen;
	HBRUSH oldBrush;

	if (sscanf(cp, " %d %d %d %d %f %f ", &xc, &yc, &xrad, &yrad,
		&start_angle, &end_angle) != 6) {
	    Warning("invalid arc specification: %s", cp);
	    return;
	}

	/* FIXME: the interior might need to be filled in */
	if (invis) return;

	oldPen = SelectObject(ruleGC, foreTPicPen);
	oldBrush = SelectObject(ruleGC, foreTPicBrush);

	if (!begin_path) {
	  BeginPath(ruleGC);
	  begin_path = True;
	}

	/* We have a specialized fast way to draw closed circles/ellipses */
	if (start_angle <= 0.0 && end_angle >= 6.282) {
	    draw_ellipse(xc, yc, xrad, yrad);
	    return;
	}
	xcenter = xc;
	ycenter = yc;
	xradius = xrad;
	yradius = yrad;
	r = (xradius + yradius) / 2.0;
	theta = sqrt(1.0 / r);
	n = 0.3 * TWOPI / theta + 0.5;
	if (n < 12) n = 12;
	else if (n > 80) n = 80;
	n /= 2;
	theta = TWOPI / n;
	flush_path();
	
	if (!begin_path) {
	  BeginPath(ruleGC);
	  begin_path = True;
	}

	im_fdraw(xcenter + xradius * cos(start_angle),
	    ycenter + yradius * sin(start_angle));
	angle = start_angle + theta;
	if (end_angle < start_angle) end_angle += TWOPI;
	while (angle < end_angle) {
	    im_fdraw(xcenter + xradius * cos(angle),
		ycenter + yradius * sin(angle));
	    angle += theta;
	}
	im_fdraw(xcenter + xradius * cos(end_angle),
	    ycenter + yradius * sin(end_angle));
	flush_path();

	SelectObject(ruleGC, oldPen);
	SelectObject(ruleGC, oldBrush);

}


/*
 *	APPROXIMATE integer distance between two points
 */

#define	dist(x0, y0, x1, y1)	(abs(x0 - x1) + abs(y0 - y1))


/*
 *	Draw a spline along the previously defined path
 */

static	void
flush_spline(char *cp)
{
	int	xp, yp;
	int	N;
	int	lastx, lasty;
	Boolean	lastvalid = False;
	int	t1, t2, t3;
	int	steps;
	int	j;
	int i, w;
	int	last_min_x, last_max_x, last_min_y, last_max_y;

	HPEN oldPen;
	HBRUSH oldBrush;

	oldPen = SelectObject(ruleGC, foreTPicPen);
	oldBrush = SelectObject(ruleGC, foreTPicBrush);

#ifdef	lint
	lastx = lasty = -1;
#endif

	last_min_x = 30000;
	last_min_y = 30000;
	last_max_x = -30000;
	last_max_y = -30000;
	for (i = 1; i <= path_len; i++) {
	    if (xx[i] > last_max_x) last_max_x = xx[i];
	    if (xx[i] < last_min_x) last_min_x = xx[i];
	    if (yy[i] > last_max_y) last_max_y = yy[i];
	    if (yy[i] < last_min_y) last_min_y = yy[i];
	}


	N = path_len + 1;
	xx[0] = xx[1];
	yy[0] = yy[1];
	xx[N] = xx[N-1];
	yy[N] = yy[N-1];
	for (i = 0; i < N - 1; i++) {	/* interval */
	    steps = (dist(xx[i], yy[i], xx[i+1], yy[i+1]) +
		dist(xx[i+1], yy[i+1], xx[i+2], yy[i+2])) / 80;
	    for (j = 0; j < steps; j++) {	/* points within */
		w = (j * 1000 + 500) / steps;
		t1 = w * w / 20;
		w -= 500;
		t2 = (750000 - w * w) / 10;
		w -= 500;
		t3 = w * w / 20;
		xp = (t1*xx[i+2] + t2*xx[i+1] + t3*xx[i] + 50000) / 100000;
		yp = (t1*yy[i+2] + t2*yy[i+1] + t3*yy[i] + 50000) / 100000;
		if (lastvalid) line_btw(lastx, lasty, xp, yp);
		lastx = xp;
		lasty = yp;
		lastvalid = True;
	    }
	}
	path_len = 0;
	do_attribute_path(last_min_x, last_max_x, last_min_y, last_max_y);

	SelectObject(ruleGC, oldPen);
	SelectObject(ruleGC, oldBrush);
}


/*
 *	Shade the last box, circle, or ellipse
 */

static	void
shade_last(char *cp)
{
  blacken = whiten = False;
  shade = True;

  if (*cp) {
      float tempShadetp;

      if (sscanf(cp, "%f ", &tempShadetp) != 1) {
	  Warning ("Illegal format for shade level: %s", cp);
	  return;
      }
      else if (tempShadetp < 0.0 || tempShadetp > 1.0) {
	  Warning ("Invalid shade level: %s", cp);
	  return;
      }
      else
	  /* if "sh" has an argument we can safely assume that tpic 2.0 is used
	     so that all subsequent "sh" commands will come with an explicit
	     argument.  Hence we may overwrite shadetp's old value */
	  /* Also note the inversion of gray levels for tpic 2.0 (0 = white,
	     1 = black) w.r.t. PostScript (0 = black, 1 = white) */
	  shadetp = 1.0 - tempShadetp;
  }
    if (foreTPicBrush)
      if (!DeleteObject(foreTPicBrush))
	Win32Error("shade_last/DeleteObject/foreTPicBrush");
    foreTPicBrush = CreateSolidBrush(RGB((shadetp*255), (shadetp*255), (shadetp*255)));


}


/*
 *	Make the last box, circle, or ellipse, white inside (shade with white)
 */

static	void
whiten_last()
{
	whiten = True;
	blacken = shade = False;
        if (foreTPicBrush)
	  if (!DeleteObject(foreTPicBrush))
	    Win32Error("whiten_last/DeleteObject/foreTPicBrush");
	foreTPicBrush = CreateSolidBrush(back_Pixel);
}


/*
 *	Make last box, etc, black inside
 */

static	void
blacken_last()
{
	blacken = True;
	whiten = shade = False;
        if (foreTPicBrush)
	  if (!DeleteObject(foreTPicBrush))
	    Win32Error("blacken_last/DeleteObject/foreTPicBrush");
	foreTPicBrush = CreateSolidBrush(fore_Pixel);
}

/*
 *      Set current shading factor
 */

/* Taken from dvips ! */
/* set shading level (used with  Fig 1.4-TFX). priol@irisa.fr, 89/04 */
/* A better trial at setting shading level -- jourdan@minos.inria.fr */
/* Count number of black bits in the pattern together with total number, */
/* compute the average and use this as the PostScript gray level */
static void
set_shade (char *cp)
{
    int blackbits = 0, totalbits = 0;

    while (*cp) {
	switch (*cp) {
	case '0':
	    totalbits += 4;
	    break;
	case '1':
	case '2':
	case '4':
	case '8':
	    blackbits += 1;
	    totalbits += 4;
	    break;
	case '3':
	case '5':
	case '6':
	case '9':
	case 'a':
	case 'A':
	case 'c':
	case 'C':
	    blackbits += 2;
	    totalbits += 4;
	    break;
	case '7':
	case 'b':
	case 'B':
	case 'd':
	case 'D':
	case 'e':
	case 'E':
	    blackbits += 3;
	    totalbits += 4;
	    break;
	case 'f':
	case 'F':
	    blackbits += 4;
	    totalbits += 4;
	    break;
	case ' ':
	    break;
	default:
	    Warning("Invalid character in .tx patte: %s\n", cp);
	    return;
	    break;
	}
	cp++;
    }
    shadetp = 1.0 - ((double) blackbits / (double) totalbits);
    if (foreTPicBrush)
      if (!DeleteObject(foreTPicBrush))
	Win32Error("set_shade/DeleteObject/foreTPicBrush");
    foreTPicBrush = CreateSolidBrush(RGB((shadetp*255), (shadetp*255), (shadetp*255)));
}                               /* end of SetShade       */

/*
 *	Code for PostScript<tm> specials begins here.
 */

#if	PS

/*
 *	Information on how to search for PS header and figure files.
 */

static	_Xconst	char	no_f_str_ps[]	= "/%f";

static	void	ps_startup ARGS((int, int, _Xconst char *));
static	void	ps_startup2 ARGS((void));
void	NullProc ARGS((void)) {}
/* ARGSUSED */
static	void	NullProc2 ARGS((_Xconst char *));

struct psprocs	psp = {		/* used for lazy startup of the ps machinery */
	/* toggle */		NullProc,
	/* destroy */		NullProc,
	/* interrupt */		NullProc,
	/* endpage */		NullProc,
	/* drawbegin */		ps_startup,
	/* drawraw */		NullProc2,
	/* drawfile */		NULL,
	/* drawend */		NullProc2,
	/* beginheader */	ps_startup2,
	/* endheader */		NullProc,
	/* newdoc */		NullProc};

struct psprocs	no_ps_procs = {		/* used if postscript is unavailable */
	/* toggle */		NullProc,
	/* destroy */		NullProc,
	/* interrupt */		NullProc,
	/* endpage */		NullProc,
	/* drawbegin */		drawbegin_none,
	/* drawraw */		NullProc2,
	/* drawfile */		NULL,
	/* drawend */		NullProc2,
	/* beginheader */	NullProc,
	/* endheader */		NullProc,
	/* newdoc */		NullProc};

#endif	/* PS */

static	Boolean		bbox_valid;
static	unsigned int	bbox_width;
static	unsigned int	bbox_height;
static	int		bbox_angle;
static	int		bbox_voffset;

void
draw_bbox()
{
	int	xcorner, ycorner;

	if (bbox_valid) {

	    xcorner = PXL_H - currwin.base_x;
	    ycorner = PXL_V - currwin.base_y;

	    if (bbox_angle == 0) {
		ycorner -= bbox_voffset;
		XDrawLine(DISP, currwin.win, ruleGC,
		  xcorner, ycorner,
		  xcorner + bbox_width, ycorner);
		XDrawLine(DISP, currwin.win, ruleGC,
		  xcorner + bbox_width, ycorner,
		  xcorner + bbox_width, ycorner + bbox_height);
		XDrawLine(DISP, currwin.win, ruleGC,
		  xcorner + bbox_width, ycorner + bbox_height,
		  xcorner, ycorner + bbox_height);
		XDrawLine(DISP, currwin.win, ruleGC,
		  xcorner, ycorner + bbox_height,
		  xcorner, ycorner);
	    }
	    else {
		float	sin_a	= sin(bbox_angle * (TWOPI / 360));
		float	cos_a	= cos(bbox_angle * (TWOPI / 360));
		float	a, b, c, d;

		a = cos_a * bbox_width;
		b = -sin_a * bbox_width;
		c = -sin_a * bbox_height;
		d = -cos_a * bbox_height;

		XDrawLine(DISP, currwin.win, ruleGC,
		  xcorner, ycorner,
		  xcorner + (int) rint(a), ycorner + (int) rint(b));
		XDrawLine(DISP, currwin.win, ruleGC,
		  xcorner + (int) rint(a), ycorner + (int) rint(b),
		  xcorner + (int) rint(a + c), ycorner + (int) rint(b + d));
		XDrawLine(DISP, currwin.win, ruleGC,
		  xcorner + (int) rint(a + c), ycorner + (int) rint(b + d),
		  xcorner + (int) rint(c), ycorner + (int) rint(d));
		XDrawLine(DISP, currwin.win, ruleGC,
		  xcorner + (int) rint(c), ycorner + (int) rint(d),
		  xcorner, ycorner);
	    }
	    bbox_valid = False;
	}
}

#if	PS

static	void
actual_startup()
{
	/*
	 * Figure out what we want to use to display postscript figures
	 * and set at most one of the following to True:
	 * resource.useGS, resource.useDPS, resource.useNeWS
	 *
	 * Choose DPS then NEWS then Ghostscript if they are available
	 */
	if (!(
#ifdef	PS_DPS
	    (resource.useDPS && initDPS())
#if	defined(PS_NEWS) || defined(PS_GS)
	    ||
#endif
#endif	/* PS_DPS */

#ifdef	PS_NEWS
	    (resource.useNeWS && initNeWS())
#ifdef	PS_GS
	    ||
#endif
#endif	/* PS_NEWS */

#ifdef	PS_GS
	    (resource.useGS && initGS())
#endif

	    ))
	    psp = no_ps_procs;
}

static	void
ps_startup(xul, yul, cp)
	int		xul, yul;
	_Xconst	char	*cp;
{
	if (!resource._postscript) {
	    psp.toggle = actual_startup;
	    draw_bbox();
	    return;
	}
	actual_startup();
	psp.drawbegin(xul, yul, cp);
}

static	void
ps_startup2()
{
	actual_startup();
	psp.beginheader();
}

/* ARGSUSED */
static	void
NullProc2(cp)
	_Xconst	char	*cp;
{}

static	void
ps_parseraw(PostScript_cmd)
	_Xconst	char	*PostScript_cmd;
{
	_Xconst	char	*p;

	/* dumb parsing of PostScript - search for rotation H. Zeller 1/97 */
	bbox_angle = 0;
	p = strstr(PostScript_cmd, "rotate");
	if (p != NULL) {
	    while (*p != '\0' && !isdigit(*p)) --p;
	    while (*p != '\0' && isdigit(*p)) --p;
	    if (*p != '+' && *p != '-') ++p;
	    sscanf(p, "%d neg rotate", &bbox_angle);
	}
}


/* ARGSUSED */
void
#if	NeedFunctionPrototypes
drawbegin_none(int xul, int yul, _Xconst char *cp)
#else	/* !NeedFunctionPrototypes */
drawbegin_none(xul, yul, cp)
	int		xul, yul;
	_Xconst	char	*cp;
#endif	/* NeedFunctionPrototypes */
{
	draw_bbox();
}


struct tickrec {
	struct tickrec	*next;
	int		pageno;
	char	*command;
	_Xconst char	*tmpname;
};

static	struct tickrec	*tickhead	= NULL;	/* head of linked list of */
						/* cached information */
static	int		nticks		= 0;	/* number of records total */

#ifndef	TICKCACHESIZE
#define	TICKCACHESIZE	3
#endif

#ifndef	TICKTMP
#define	TICKTMP		"/tmp"
#endif

#ifdef WIN32
char tick_tmp[PATH_MAX];
#endif

static	struct tickrec *
cachetick(filename, pathinfo, fp)
	_Xconst char	*filename;
	kpse_file_format_type pathinfo;
	FILE		**fp;
{
	struct tickrec	**linkp;
	struct tickrec	*tikp;
	struct tickrec	**freerecp;

	static int fileno=0;

	linkp = &tickhead;
	freerecp = NULL;
	for (;;) {		/* see if we have it already */
	    tikp = *linkp;
	    if (tikp == NULL) {	/* if end of list */
		if (nticks >= TICKCACHESIZE && freerecp != NULL) {
		    tikp = *freerecp;
		    *freerecp = tikp->next;
		    free(tikp->command);
			unlink(tikp->tmpname);
		}
		else {
		  /* This will overflow when we get 10^9 temporary files.
		     If we have to worry I worry */
		    char *buffer=malloc(strlen(temporary_dir)+10);
		    
		    tikp = (struct tickrec *)
			xmalloc(sizeof(struct tickrec));
		    
		    sprintf(buffer,"%s/%d",temporary_dir,fileno);

		    fileno++;

		    tikp->tmpname = buffer;
		    tikp->pageno = -1;
		    if (tikp->tmpname == NULL) {
			Fputs("Cannot create temporary file name.\n", stderr);
			free((char *) tikp);
			return NULL;
		    }
		    ++nticks;
		}
		tikp->command = xmalloc((unsigned) strlen(filename) + 1);
		Strcpy(tikp->command, filename);
		*fp = NULL;
		break;
	    }
	    if (strcmp(filename, tikp->command) == 0) {	/* found it */
		*linkp = tikp->next;	/* unlink it */
		*fp = xfopen(tikp->tmpname, OPEN_MODE);
		if (*fp == NULL) {
		  fprintf(stderr,"%s: ("__FILE__" at line %d) ", prog, __LINE__);
		  perror(tikp->tmpname);
		}
		break;
	    }
	    if (tikp->pageno != current_page) freerecp = linkp;
	    linkp = &tikp->next;
	}
	tikp->next = tickhead;		/* link it in */
	tickhead = tikp;
	tikp->pageno = 
	  pathinfo != kpse_tex_ps_header_format ? current_page : -1;
	return tikp;
}

#ifndef	UNCOMPRESS
#define	UNCOMPRESS	"uncompress"
#endif

#ifndef	GUNZIP
#define	GUNZIP		"gunzip"
#endif

#ifndef	BUNZIP2
#define	BUNZIP2		"bunzip2"
#endif

#ifdef WIN32
/* ===========================================================================
 * Uncompress input to output then close both files.
 */
void gz_uncompress(gzFile in, FILE *out)
{
  char buf[16384];
  int len;
  int err;

  for (;;) {
    len = gzread(in, buf, sizeof(buf));
    if (len < 0) {
      fprintf(stderr, "gunzip: %s\n", (gzerror(in, &err)));
      return;
    }
    if (len == 0) break;

    if ((int)fwrite(buf, 1, (unsigned)len, out) != len) {
      fprintf(stderr, "gunzip: failed fwrite\n");
      return;
    }
  }
  if (fclose(out)) {
    fprintf(stderr, "gunzip: failed fclose\n");
    return;
  }

  if (gzclose(in) != Z_OK) {
    fprintf(stderr, "gunzip: failed gzclose\n");
    return;
  }
}
#endif

#ifdef USE_POPEN
static	char buf_popen[1024];
#endif

static	void
send_ps_file(filename, pathinfo)
	_Xconst char	*filename;
	kpse_file_format_type pathinfo;
{
	FILE		*f;
	static _Xconst char *argv[]	= {NULL, "-c", NULL, NULL};
	_Xconst char	*bufp;
	struct tickrec	*tikp;
	int		len;
	char		magic1, magic2, magic3;

	if (psp.drawfile == NULL || !resource._postscript) return;

#ifdef HTEX

	filename=urlocalize(filename);

	if (((URLbase != NULL) && htex_is_url(urlocalize(URLbase))) ||
	    (htex_is_url(filename))) {
	  /* xfopen knows how to fetch url things */
	  URL_aware=1;
	  f = xfopen(filename, OPEN_MODE);
	  bufp=filelist[lastwwwopen].file;
	  URL_aware=0;
	} else
#endif
	if (filename[0] == '`') {
	    if (!resource.allow_shell) {
		if (warn_spec_now)
		    Fprintf(stderr,
			"%s: shell escape disallowed for special \"%s\"\n",
			prog, filename);
		return;
	    }

	    tikp = cachetick(filename, pathinfo, &f);
	    if (tikp == NULL)
		return;
	    if (f == NULL) {
#ifdef USE_POPEN
	FILE *in, *out;
	int nb_read, nb_written;
	in = popen(filename+1, OPEN_MODE);
	out = xfopen(tikp->tmpname, "wb");
	if (!in) {
	  fclose(out);
	  fprintf(stderr, "Can't popen %s\n", filename+1);
	  return;
	}
	/* Copying the result of popen to tmpname */
	for ( ; !feof(in); ) {
	  nb_read = fread(buf_popen, 1, sizeof(buf_popen), in);
	  if (ferror(in)) {
	    perror(filename+1);
	    break;
	  }
	  nb_written = fwrite(buf_popen, 1, nb_read, out);
	  if (ferror(out)) {
	    perror(tikp->tmpname);
	    break;
	  }
	}
	pclose(in);
	fclose(out);
#else
	len = strlen(filename) + strlen(tikp->tmpname) + (4 - 1);
	if (len > ffline_len)
	  expandline(len);
	Sprintf(ffline, "%s > %s", filename + 1, tikp->tmpname);
	(void) system(ffline);
#endif
	f = xfopen(tikp->tmpname, OPEN_MODE);
	if (f == NULL) {
	  fprintf(stderr,"%s: ("__FILE__" at line %d) ", prog, __LINE__);
	  perror(tikp->tmpname);
	  return;
	}
      }
      bufp = tikp->tmpname;
    }
    else {

      bufp = kpse_find_file (filename, pathinfo, True);
      f = bufp ? xfopen (bufp, OPEN_MODE) : NULL;
      
      /* if still no luck, complain */
      if (f == NULL) {
	Fprintf(stderr, "%s: cannot find PS file `%s'.\n",
		prog, filename);
	draw_bbox();
	return;
      }
      
#ifdef WIN32
      /* check for compressed files */
      len = strlen(filename);
      magic1 = '\037';
      magic3 = '\0';
      if ((len > 2 && _stricmp(filename + len - 2, ".Z") == 0 && (magic2 = '\235', True))
	  || (len > 3 && _stricmp(filename + len - 3, ".gz") == 0 && (magic2 = '\213', True))) {
	/* Uncompress or Gzip */
	if (getc(f) != magic1 || (char) getc(f) != magic2)
	  rewind(f);
	else {
	  Fclose(f);
	  ++n_files_left;
	  tikp = cachetick(filename, pathinfo, &f);
	  if (tikp == NULL)
	    return;
	  if (f == NULL) {
	    FILE *outfile;
	    gzFile *infile;
	    infile = gzopen(bufp, "rb");
	    if (infile == NULL) {
	      fprintf(stderr, "%s: can't gzopen %s\n", prog, bufp);
	      return;
	    }
	    /* FIXME: Check that this is correct wrt to the unix case,
	     tmpname are reused. */
	    if ((outfile = fopen(tikp->tmpname, "wb")) == NULL) {
	      perror(tikp->tmpname);
	      return;
	    }
	    gz_uncompress(infile, outfile);
	    /* Files closing is cared of by gz_uncompress ! */
	    f = xfopen(tikp->tmpname, OPEN_MODE);
	    if (f == NULL) {
	      fprintf(stderr,"%s: ("__FILE__" at line %d) ",
				  prog, __LINE__);
	      perror(tikp->tmpname);
	      return;
	    }
	  }
	  bufp = tikp->tmpname;
	}
      }
      else if (len > 4 && _stricmp(filename + len - 4, ".bz2") == 0) {
	/* Bunzip2 */
	magic1 = 'B'; 
	magic2 = 'Z';
	magic3 = 'h';
	if (getc(f) != magic1 || (char) getc(f) != magic2
	    || getc(f) != magic3)
	  rewind(f);
	else {
	  Fclose(f);
	  ++n_files_left;
	  tikp = cachetick(filename, pathinfo, &f);
	  if (tikp == NULL)
	    return;
	  if (f == NULL) {
	    FILE *outfile;
	    BZFILE *infile;
	    unsigned char buf[4096];
	    size_t len;
	    infile = BZ2_bzopen(bufp, "rb");
	    if (infile == NULL) {
	      fprintf(stderr, "%s: can't BZ2_bzopen %s\n", prog, bufp);
	      return;
	    }
	    /* FIXME: Check that this is correct wrt to the unix case,
	     tmpname are reused. */
	    if ((outfile = fopen(tikp->tmpname, "wb")) == NULL) {
	      perror(tikp->tmpname);
	      return;
	    }
	    while ((len = BZ2_bzread(infile, buf, sizeof(buf))) > 0)
	      fwrite (buf, 1, len, outfile);
	    BZ2_bzclose (infile);
	    fclose(outfile);
	    f = xfopen(tikp->tmpname, OPEN_MODE);
	    if (f == NULL) {
	      fprintf(stderr,"%s: ("__FILE__" at line %d) ",
				  prog, __LINE__);
	      perror(tikp->tmpname);
	      return;
	    }
	  }
	  bufp = tikp->tmpname;
	}
      }
      else if (len > 3 && _stricmp(filename + len - 2, ".zip") == 0) {
	fprintf(stderr, "%s: Zip files are not supported (%s)\n", prog, filename);
	Fclose(f);
	return;
      }
      else {
	/* Not a compressed file ? */
	rewind(f);
      }
    }
#else
      /* check for compressed files */
      len = strlen(filename);
      magic1 = '\037';
      magic3 = '\0';
      if ((len > 2 && strcmp(filename + len - 2, ".Z") == 0
	   && (argv[0] = UNCOMPRESS, magic2 = '\235', True))
	  || (len > 3 && strcmp(filename + len - 3, ".gz") == 0
	      && (argv[0] = GUNZIP, magic2 = '\213', True))
	  || (len > 4 && strcmp(filename + len - 4, ".bz2") == 0
	      && (argv[0] = BUNZIP2, magic1 = 'B', magic2 = 'Z',
		  magic3 = 'h', True))) {
	if (getc(f) != magic1 || (char) getc(f) != magic2
	    || (magic3 != '\0' && getc(f) != magic3))
	  rewind(f);
	else {
	  Fclose(f);
	  ++n_files_left;
	  tikp = cachetick(filename, pathinfo, &f);
	  if (tikp == NULL)
	    return;
	  if (f == NULL) {
			pid_t	pid;
			int	handle;
			int	status;

			argv[2] = bufp;
			handle = open(tikp->tmpname, O_RDWR | O_CREAT | O_EXCL,
 				      S_IRUSR | S_IWUSR);
			if (handle == -1 && errno == EEXIST) {
			  /* The tmpnames are reused for each page so
			     unlink the tmpname file from the previous page */
			  unlink(tikp->tmpname);
			  handle = open(tikp->tmpname, O_RDWR | O_CREAT | 
					O_EXCL, S_IRUSR | S_IWUSR);
			}
 			if (handle == -1) {
			    fprintf(stderr,"%s: ("__FILE__" at line %d) ", prog, __LINE__);
			    perror(tikp->tmpname);
			    return;
			}
			Fflush(stderr);	/* avoid double flushing */
			pid = vfork();
			if (pid == 0) {	/* if child */
			    (void) dup2(handle, 1);
			    (void) execvp(argv[0], (char **) argv);
			    Fprintf(stderr, "Execvp of %s failed.\n", argv[0]);
			    Fflush(stderr);
			    _exit(1);
			}
			(void) close(handle);
			for (;;) {
#if HAVE_WAITPID
			    if (waitpid(pid, &status, 0) != -1) break;
#else
# if HAVE_WAIT4
			    if (wait4(pid, &status, 0, (struct rusage *) NULL)
				    != -1)
				break;
# else
			    int retval;

			    retval = wait(&status);
			    if (retval == pid) break;
			    if (retval != -1) continue;
# endif /* HAVE_WAIT4 */
#endif /* HAVE_WAITPID */
			    if (errno == EINTR) continue;
			    perror("[xdvi] waitpid");
			    return;
			}
			f = xfopen(tikp->tmpname, OPEN_MODE);
			if (f == NULL) {
			    fprintf(stderr,"%s: ("__FILE__" at line %d) ",
						prog, __LINE__);
			    perror(tikp->tmpname);
			    return;
			}
		    }
		    bufp = tikp->tmpname;
		}
	    }
	}
#endif

	/* Success! */
	psp.drawfile(bufp, f, filename);	/* this is supposed to close the file */
}


void
ps_newdoc()
{
	struct tickrec	*tikp;

	scanned_page = scanned_page_bak = scanned_page_reset =
	    resource.prescan ? -1 : total_pages;
	for (tikp = tickhead; tikp != NULL; tikp = tikp->next)
	    tikp->pageno = -1;
	psp.newdoc();
}


void
ps_destroy()
{
  struct tickrec	*tikp;

  /* Note:  old NeXT systems (at least) lack atexit/on_exit.  */
  psp.destroy();
  for (tikp = tickhead; tikp != NULL; tikp = tikp->next)
    if (unlink(tikp->tmpname) < 0) {
      fprintf(stderr,"%s: ("__FILE__" at line %d) ", prog, __LINE__);
      perror(tikp->tmpname);
    }
}

#endif	/* PS */


static	void
psfig_special(cp)
	char	*cp;
{
	char	*filename;
	int	raww, rawh;

	if (strncmp(cp, ":[begin]", 8) == 0) {
	    cp += 8;
	    bbox_valid = False;
	    bbox_angle = 0;
	    if (sscanf(cp,"%d %d\n", &raww, &rawh) >= 2) {
		bbox_valid = True;
		bbox_width = pixel_conv(spell_conv(raww));
		bbox_height = pixel_conv(spell_conv(rawh));
		bbox_voffset = 0;
	    }
	    if (currwin.win == mane.win)
#if	PS
	      if (isPrinting) 
		psp.drawbegin(PXL_H, PXL_V,
			      cp);
	      else
		psp.drawbegin(PXL_H - currwin.base_x, PXL_V - currwin.base_y,
			      cp);
#else
		draw_bbox();
#endif
	    psfig_begun = True;
	} else if (strncmp(cp, " plotfile ", 10) == 0) {
	    cp += 10;
	    while (isspace(*cp)) cp++;
	    /* handle "`zcat file". Borrowed from dvipsk...*/
	    if (*cp == '"') {
		cp++;
		for (filename = cp; *cp && (*cp != '"'); ++cp);
	    } else {
		for (filename = cp; *cp && !isspace(*cp); ++cp);
	    }
	    *cp = '\0';
#if	PS
	    if (currwin.win == mane.win) 
	      send_ps_file(filename, kpse_pict_format);
#endif
	} else if (strncmp(cp, ":[end]", 6) == 0) {
	    cp += 6;
#if	PS
	    if (currwin.win == mane.win) psp.drawend(cp);
#endif
	    bbox_valid = False;
	    psfig_begun = False;
	} else { /* I am going to send some raw postscript stuff */
	  if (*cp == ':')
	    ++cp;	/* skip second colon in ps:: */
#if	PS
	    if (currwin.win == mane.win) {
		ps_parseraw(cp);
		if (psfig_begun) psp.drawraw(cp);
		else {
		  if (isPrinting) 
		    psp.drawbegin(PXL_H, PXL_V, cp);
		  else
		    psp.drawbegin(PXL_H - currwin.base_x,
			PXL_V - currwin.base_y, cp);
		    psp.drawend("");
		}
	    }
#endif
	}
}


/*	Keys for epsf specials */

static	_Xconst	char	*keytab[]	= {"clip",
					   "llx",
					   "lly",
					   "urx",
					   "ury",
					   "rwi",
					   "rhi",
					   "hsize",
					   "vsize",
					   "hoffset",
					   "voffset",
					   "hscale",
					   "vscale",
					   "angle"};

#define	KEY_LLX	keyval[0]
#define	KEY_LLY	keyval[1]
#define	KEY_URX	keyval[2]
#define	KEY_URY	keyval[3]
#define	KEY_RWI	keyval[4]
#define	KEY_RHI	keyval[5]

#define	NKEYS	(sizeof(keytab)/sizeof(*keytab))
#define	N_ARGLESS_KEYS 1

static	void
epsf_special(cp)
	char	*cp;
{
	char	*filename;
	static	char		*buffer;
	static	unsigned int	buflen	= 0;
	unsigned int		len;
	char	*q;
	int	flags	= 0;
	double	keyval[6];

	filename = cp;
	if (*cp == '\'' || *cp == '"') {
	    do ++cp;
	    while (*cp != '\0' && *cp != *filename);
	    ++filename;
	}
	else
	    while (*cp != '\0' && *cp != ' ' && *cp != '\t') ++cp;
	if (*cp != '\0') *cp++ = '\0';
	while (*cp == ' ' || *cp == '\t') ++cp;
	len = strlen(cp) + NKEYS + 30;
	if (buflen < len) {
	    if (buflen != 0) free(buffer);
	    buflen = len;
	    buffer = xmalloc(buflen);
	}
	Strcpy(buffer, "@beginspecial");
	q = buffer + strlen(buffer);
	while (*cp != '\0') {
	    char *p1 = cp;
	    int keyno;

	    while (*p1 != '=' && !isspace(*p1) && *p1 != '\0') ++p1;
	    for (keyno = 0;; ++keyno) {
		if (keyno >= NKEYS) {
		    if (warn_spec_now)
			Fprintf(stderr,
			    "%s: unknown keyword (%*s) in \\special will be ignored\n",
			    prog, (int) (p1 - cp), cp);
		    break;
		}
		if (memcmp(cp, keytab[keyno], p1 - cp) == 0) {
		    if (keyno >= N_ARGLESS_KEYS) {
			while (isspace(*p1)) ++p1;
			if (*p1 == '=') {
			    ++p1;
			    while (isspace(*p1)) ++p1;
			}
			if (keyno < N_ARGLESS_KEYS + 6) {
			    keyval[keyno - N_ARGLESS_KEYS] = atof(p1);
			    flags |= (1 << (keyno - N_ARGLESS_KEYS));
			}
			*q++ = ' ';
			while (!isspace(*p1) && *p1 != '\0') *q++ = *p1++;
		    }
		    *q++ = ' ';
		    *q++ = '@';
		    Strcpy(q, keytab[keyno]);
		    q += strlen(q);
		    break;
		}
	    }
	    cp = p1;
	    while (!isspace(*cp) && *cp != '\0') ++cp;
	    while (isspace(*cp)) ++cp;
	}
	Strcpy(q, " @setspecial\n");

	bbox_valid = False;
	if ((flags & 0x30) == 0x30 || ((flags & 0x30) && (flags & 0xf) == 0xf)){
	    bbox_valid = True;
	    bbox_width = 0.1 * ((flags & 0x10) ? KEY_RWI
		: KEY_RHI * (KEY_URX - KEY_LLX) / (KEY_URY - KEY_LLY))
		* dimconv / shrink_factor + 0.5;
	    bbox_voffset = bbox_height = 0.1 * ((flags & 0x20) ? KEY_RHI
		: KEY_RWI * (KEY_URY - KEY_LLY) / (KEY_URX - KEY_LLX))
		* dimconv / shrink_factor + 0.5;
	}

	if (filename && currwin.win == mane.win) {
#if	PS
	  if (isPrinting)
	    psp.drawbegin(PXL_H, PXL_V,	buffer);
	  else
	    psp.drawbegin(PXL_H - currwin.base_x, PXL_V - currwin.base_y,
		buffer);
	    /* talk directly with the DPSHandler here */
	    send_ps_file(filename, kpse_pict_format);
	    psp.drawend(" @endspecial");
#else
	    draw_bbox();
#endif
	}
	bbox_valid = False;
}


static	void
quote_special(cp)
	char	*cp;
{
	bbox_valid = False;

#if	PS
	if (currwin.win == mane.win) {
	  if (isPrinting) 
	    psp.drawbegin(PXL_H, PXL_V,
			  "@beginspecial @setspecial ");
	  else
	    psp.drawbegin(PXL_H - currwin.base_x, PXL_V - currwin.base_y,
		"@beginspecial @setspecial ");
	    /* talk directly with the DPSHandler here */
	    psp.drawraw(cp + 1);
	    psp.drawend(" @endspecial");
	}
#endif

	/* nothing else to do--there's no bbox here */
}

#if	PS

static	void
scan_header(cp)
	char	*cp;
{
	char	*filename;

	filename = cp;
	if (*cp == '\'' || *cp == '"') {
	    do ++cp;
	    while (*cp != '\0' && *cp != *filename);
	    *cp = '\0';
	    ++filename;
	}

	psp.beginheader();
	send_ps_file(filename, kpse_tex_ps_header_format);
}

static	void
scan_bang(cp)
	char	*cp;
{
	psp.beginheader();
	psp.drawraw(cp + 1);
}

#endif	/* PS */

/*
  Color support.
  Basically, the primitives supported by dvips.
  Still do not work when printing. Colors are mixed up because we can't do
  transparent blt directly on the printer dc.
*/


void
background P1C(char *, p)
{
  /* this should be valid only for the current page */
  while ( *p <= ' ' ) p++ ;
#if DEBUG_COLOR
  fprintf(stderr, "Special : background %s\n", p);
#endif
  SetBackColor(string_to_colorref(p));
}

void
pushcolor P2C(char *, p, Boolean, outtops)
{
  COLORREF c = string_to_colorref(p);

#if DEBUG_COLOR
  fprintf(stderr, "Special : push color %s %8x\n", p, c);
#endif

  CRefPush(color_stack, c);
  SetForeColor(c);
}

void
popcolor P1C(Boolean, outtops)
{
  CRefPop(color_stack);
  SetForeColor(CRefTop(color_stack));
#if DEBUG_COLOR
  fprintf(stderr, "Special : pop color %8x\n", fore_Pixel);
#endif
}

void
resetcolorstack P2C(char *, p, int, outtops)
{
  COLORREF c = string_to_colorref(p);

  CRefResetInit(color_stack, c);
  SetForeColor(c);
}

/*
 *	The following copyright message applies to the rest of this file.  --PV
 */

/*
 *	This program is Copyright (C) 1987 by the Board of Trustees of the
 *	University of Illinois, and by the author Dirk Grunwald.
 *
 *	This program may be freely copied, as long as this copyright
 *	message remaines affixed. It may not be sold, although it may
 *	be distributed with other software which is sold. If the
 *	software is distributed, the source code must be made available.
 *
 *	No warranty, expressed or implied, is given with this software.
 *	It is presented in the hope that it will prove useful.
 *
 *	Hacked in ignorance and desperation by jonah@db.toronto.edu
 */

/*
 *      The code to handle the \specials generated by tpic was modified
 *      by Dirk Grunwald using the code Tim Morgan at Univ. of Calif, Irvine
 *      wrote for TeXsun.
 */

static	char *
endofcommand(cp)
	char	*cp;
{
	while (isspace(*cp)) ++cp;
	if (*cp != '=') return NULL;
	do ++cp; while (isspace(*cp));
	return cp;
}


// addition SU: copied from dvi-draw.c
#define	xtell(pos)	((long) (lseek(fileno(dvi_file), 0L, SEEK_CUR) - \
			    (currinf.end - (pos))))


void
applicationDoSpecial(cp, maybe_src)
	char	*cp;
	int		maybe_src;
{
  char	*p;
  boolean gs_allowed;
  struct drawinf	oldinfo;
  ubyte ch;
  float pt_size;
  off_t file_pos;
  static Position remember_H, remember_V;
#ifdef TRANSFORM
  extern XFORM *gs_getmatrix(void);
  extern void xfrm_record(XFORM *);
  extern void xfrm_apply(void);
#endif
  
  /* Skip white space */
  while (*cp == ' ' || *cp == '\t') ++cp;
  
  if (allowDrawingChars && maybe_src) {
/* BEGIN CHUNK special.c 1 */
#ifdef SRC_SPECIALS  
    if (memicmp(cp, "src:", 4) == 0) {
      /*	  oldinfo = currinf;
	  file_pos = xtell(currinf.pos);
	  pt_size = src_look_forward(dimconv);
	  fprintf(stderr, "returning.\n");
	  currinf = oldinfo;
	  (void) lseek(fileno(dvi_file), file_pos, SEEK_SET);
	  src_eval_special(cp + 4, PXL_H, PXL_V, pt_size); */
#if 0
      fprintf(stderr, "executing %d, %d for %s\n", remember_H, remember_V, cp + 4);
#endif
      src_eval_special(cp + 4, remember_H, remember_V);
      delay_src = False;
    }
    return;
#endif  
/* END CHUNK special.c 1 */
  }

  /* PostScript specials */
  
  gs_allowed = !allowDrawingChars && ((mane.shrinkfactor > 1) || isPrinting);
#if 0
  fprintf(stderr, "special found at %ld: %s, isPrinting = %d, gs_allowed = %d\n", 
	  currinf.pos, cp, isPrinting, gs_allowed);
#endif
  /* We do not allow to use gs at magnification 1 */
  if (*cp == '"') {
    if (gs_allowed) {
      quote_special(cp);
#ifdef TRANSFORM
      xfrm_record(gs_getmatrix());
#endif
    }
#ifdef TRANSFORM
    else if (allowDrawingChars) {
      xfrm_apply();
    }
#endif
    return;
  }
  if (memicmp(cp, "ps:", 3) == 0) {
    if (gs_allowed) {
      psfig_special(cp + 3);
#ifdef TRANSFORM
      xfrm_record(gs_getmatrix());
#endif
    }
#ifdef TRANSFORM
    else if (allowDrawingChars) {
      xfrm_apply();
    }
#endif
    return;
  }

  
  if (memicmp(cp, "src:", 4) == 0) {
    if (allowDrawingChars) {
#ifdef SRC_SPECIALS
      remember_V = PXL_V;
      remember_H = PXL_H;
#if 0
      fprintf(stderr, "remembering %d, %d for %s\n", remember_H, remember_V, cp + 4);
#endif
      delay_src = True;
#else
      fprintf(stderr, "special not handled: %s\n", cp);
#endif
    }
    return;
  }
    
  if (memicmp(cp, "psfile", 6) == 0
	&& (p = endofcommand(cp + 6)) != NULL) {
    if (gs_allowed) {
      epsf_special(p);
#ifdef TRANSFORM
      xfrm_record(gs_getmatrix());
#endif
    }
#ifdef TRANSFORM
    else if (allowDrawingChars) {
      xfrm_apply();
    }
#endif
    return;
  }

    /* these should have been scanned */
	  
    if (*cp == '!'
	|| (memicmp(cp, "header", 6) == 0
	    && endofcommand(cp + 6) != NULL)) {
#if	PS
      if (resource._postscript && scanned_page_reset >= 0) {
	/* turn on scanning and redraw the page */
	scanned_page = scanned_page_bak = scanned_page_reset = -1;
	psp.interrupt();
	canit = True;
	/* longjmp(canit_env, 1); */
	SendMessage(hWndDraw, WM_COMMAND, IDM_REDRAWPAGE, 0);
      }
#endif
      return;
    }

    /* Color specials */

    if (memicmp(cp, "background", 10) == 0) {
#if 0
      if (scanned_page_reset >= 0) {
	p = cp + 11;
	while ( *p <= ' ' ) p++ ;
	background(p) ;
	scanned_page = scanned_page_bak = scanned_page_reset = -1;
	if (! isPrinting)
	  SendMessage(hWndDraw, WM_COMMAND, IDM_REDRAWPAGE, 0);
      }
#else
      p = cp + 11;
      while ( *p <= ' ' ) p++ ;
      background(p) ;
#endif
      return ;
    }

    if (memicmp(cp, "landscape", 9) == 0) {
      fprintf(stderr, "special not handled: %s\n", cp);
      return;
    }

    if (memicmp(cp, "papersize", 9) == 0) {
      fprintf(stderr, "special not handled: %s\n", cp);
      return;
    }

    if (memicmp(cp, "pos:", 4) == 0) { 
      /* ignore positional specials */
      return;
    }

    if (memicmp(cp, "xtex:", 5) == 0) {
      return;
    }

    if (memicmp(cp, "em:", 3) == 0) {
      if (currwin.win == hWndDraw)
		emspecial(cp+3, PXL_H, PXL_V);
      return;
    }
    
    if (memicmp(cp, "color", 5) == 0) {
      p = cp + 6;
      while ( *p <= ' ' ) p++ ;
      if (memicmp(p, "push", 4) == 0) {
	p += 5;
	while ( *p <= ' ' ) p++ ;
	pushcolor(p, 0) ;
      } else if (memicmp(p, "pop", 3) == 0) {
	popcolor(0) ;
      } else {
	resetcolorstack(p,0) ;
      }
      return;
    }   /* IBM: color - end changes */

#ifdef HTEX
	if (checkHyperTeX(cp, current_page)) return;
#endif

	/* tpic specials */

	if (*cp >= 'a' && *cp <= 'z' && cp[1] >= 'a' && cp[1] <= 'z' &&
		(isspace(cp[2]) || cp[2] == '\0')) {
#if DEBUG_TPIC
	    fprintf(stderr, "Tpic special : %s\n", cp);
#endif
	    switch (CMD(*cp, cp[1])) {
		case CMD('p','n'): set_pen_size(cp + 2); return;
		case CMD('f','p'): flush_path(); return;
		case CMD('d','a'): flush_dashed(cp + 2, False); return;
		case CMD('d','t'): flush_dashed(cp + 2, True); return;
		case CMD('p','a'): add_path(cp + 2); return;
		case CMD('a','r'): arc(cp + 2, False); return;
		case CMD('i','a'): arc(cp + 2, True); return;
		case CMD('s','p'): flush_spline(cp+2); return;
		case CMD('s','h'): shade_last(cp+2); return;
		case CMD('w','h'): whiten_last(); return;
		case CMD('b','k'): blacken_last(); return;
		case CMD('i','p'): /* throw away the path -- jansteen */
		    path_len = 0; return;
	        case CMD('t','x'): set_shade(cp+2); return;
	    }
	}

	if (warn_spec_now)
	    Fprintf(stderr, "%s:  special \"%s\" not implemented\n", prog, cp);
}

#undef	CMD

#if	PS
void
scan_special(cp)
	char	*cp;
{
        char	*p;
	/* Skip white space */
	while (*cp == ' ' || *cp == '\t') ++cp;

	if (debug & DBG_PS)
	    Printf("Scanning special `%s'.\n", cp);

	if (*cp == '"' 
	    || memicmp(cp, "ps:", 3) == 0 
	    || (memicmp(cp, "psfile", 6) == 0 && (p = endofcommand(cp + 6)) != NULL)
	    || ((memicmp(cp, "header", 6) == 0) && ((p = endofcommand(cp + 6)) != NULL))) {
	  psToDisplay = TRUE;
	}
	
    /* these should have been scanned */
	  
  if (*cp == '!') 
    scan_bang(cp);
  else if ((memicmp(cp, "header", 6) == 0)
	   && ((p = endofcommand(cp + 6)) != NULL))
    scan_header(p);
  else if (memicmp(cp, "background", 10) == 0
	   || memicmp(cp, "color", 5) == 0) {
    scan_colors(cp);
  }

}
#endif	/* PS */
