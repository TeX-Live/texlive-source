/*========================================================================*\

Copyright (c) 1990-2019  Paul Vojta and others

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
PAUL VOJTA OR ANY OTHER AUTHOR OF THIS SOFTWARE BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

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


#include "xdvi-config.h"
#include "xdvi.h"

#include <setjmp.h>

#include <ctype.h>

#include "kpathsea/c-fopen.h"
#include "kpathsea/c-stat.h"
#include "kpathsea/line.h"
#include "kpathsea/tex-file.h"

#include "special.h"
#include "hypertex.h"
#include "dvi.h"
#include "message-window.h"
#include "events.h"
#include "dvi-init.h"
#include "dvi-draw.h"
#include "statusline.h"
#include "util.h"
#include "image-magick.h"
#include "pagesel.h"
#include "my-snprintf.h"
#include "string-utils.h"

#if PS
# ifdef	PS_DPS
# include "psdps.h"
# endif
# ifdef	PS_NEWS
# include "psnews.h"
# endif
# ifdef	PS_GS
# include "psgs.h"
# endif

/*
  Code inside BUG_888087_FIXED improve the appearance of pic specials at magstep 1, but
  _decreases_ it at other magnifications (bug #888087; see
  http://sourceforge.net/tracker/index.php?func=detail&aid=888087&group_id=23164&atid=377580)
  Disabled until this is fixed.
*/
#define BUG_888087_FIXED 0


/*
  Flag whether this page contains raw PS material;
  used to warn user about this:
*/
Boolean have_raw_postscript = False;

#endif


#if COLOR

/*
 *	Color stack used when scanning.  These records stay around, to ease
 *	the burden on xmalloc().  The first entry is a dummy entry, giving
 *	the default foreground color (as modified).
 */

struct colorframe {
    struct colorframe *next, *prev;
    struct rgb color;
};

static Boolean m_have_papersize_special = False;

void reset_papersize_special(void)
{
    m_have_papersize_special = False;
}

Boolean have_papersize_special(void)
{
    return m_have_papersize_special;
}

static struct colorframe scanstack_head;
static int scanstack_len;
static struct colorframe *scanstack_current;

/*
 *	Page stack when displaying.  Again, the records stay around once
 *	allocated.  Bottom entries in the stack (inherited from previous
 *	pages) are stored in an array instead (see comments in xdvi.h).
 */

static struct colorframe *rcs_head;

static Boolean parse_color (const char *cp0, const char *cp, struct rgb *rgbp, Boolean generic_ps_flag);

#endif /* COLOR */


#ifndef S_IRUSR
# define S_IRUSR 0400
#endif
#ifndef S_IWUSR
# define S_IWUSR 0200
#endif

# if HAVE_SYS_WAIT_H
#  include <sys/wait.h>
# endif
# ifndef WEXITSTATUS
#  define WEXITSTATUS(stat_val) ((unsigned)(stat_val) >> 8)
# endif
# ifndef WIFEXITED
#  define WIFEXITED(stat_val) (((stat_val) & 255) == 0)
# endif

#define	MAXPOINTS	300	/* Max points in a path */
#define	MAX_PEN_SIZE	7	/* Max pixels of pen width */
#define	TWOPI		(3.14159265359 * 2.0)

extern double floor(double);
#define	rint(x)	floor((x) + 0.5)

static int xx[MAXPOINTS], yy[MAXPOINTS];	/* Path in milli-inches */
static int path_len = 0;	/* # points in current path */
static int pen_size = 1;	/* Pixel width of lines drawn */

static Boolean whiten = False;
static Boolean shade = False;
static Boolean blacken = False;

Boolean psfig_begun = False;

#define	CMD(x, y)	((x) << 8 | (y))


/*
 *	X drawing routines
 */

#define	toint(x)	((int) ((x) + 0.5))
#define	xconv(x)	(toint(tpic_conv*(x))/currwin.shrinkfactor + PXL_H)
#define	yconv(y)	(toint(tpic_conv*(y))/currwin.shrinkfactor + PXL_V)

/*
 *	Draw a line from (fx,fy) to (tx,ty).
 */
static void
line_btw(int fx, int fy, int tx, int ty)
{
    int fcx = xconv(fx);
    int tcx = xconv(tx);
    int fcy = yconv(fy);
    int tcy = yconv(ty);

    if ((fcx < globals.win_expose.max_x || tcx < globals.win_expose.max_x) && (fcx >= globals.win_expose.min_x || tcx >= globals.win_expose.min_x) &&
	(fcy < globals.win_expose.max_y || tcy < globals.win_expose.max_y) && (fcy >= globals.win_expose.min_y || tcy >= globals.win_expose.min_y)) {
#if COLOR
	if (fg_active != fg_current)
	    do_color_change();
#endif
	XDrawLine(DISP, currwin.win, globals.gc.rule,
		  fcx - currwin.base_x, fcy - currwin.base_y,
		  tcx - currwin.base_x, tcy - currwin.base_y);
    }
}

/*
 *	Draw a dot at (x,y)
 */
static void
dot_at(int x, int y)
{
    int cx = xconv(x);
    int cy = yconv(y);

    if (cx < globals.win_expose.max_x && cx >= globals.win_expose.min_x && cy < globals.win_expose.max_y && cy >= globals.win_expose.min_y){
#if COLOR
	if (fg_active != fg_current)
	    do_color_change();
#endif
	XDrawPoint(DISP, currwin.win, globals.gc.rule,
		   cx - currwin.base_x, cy - currwin.base_y);
    }
}

/*
 *	Apply the requested attributes to the last path (box) drawn.
 *	Attributes are reset.
 *	(Not currently implemented.)
 */
static void
do_attribute_path(int last_min_x, int last_max_x, int last_min_y, int last_max_y)
{
    UNUSED(last_min_x);
    UNUSED(last_max_x);
    UNUSED(last_min_y);
    UNUSED(last_max_y);
}

/*
 *	Set the size of the virtual pen used to draw in milli-inches
 */
static void
set_pen_size(char *cp)
{
    int ps;

    if (sscanf(cp, " %d ", &ps) != 1) {
	XDVI_WARNING((stderr, "invalid .ps command format: %s", cp));
	return;
    }
    pen_size = (2 * ps * resource.pixels_per_inch / currwin.shrinkfactor + 1000) / 2000;
    if (pen_size < 1)
	pen_size = 1;
    else if (pen_size > MAX_PEN_SIZE)
	pen_size = MAX_PEN_SIZE;

#if BUG_888087_FIXED
    if (globals.gc.rule) {
	XGCValues values;
	values.line_width = pen_size;
	XChangeGC(DISP, globals.gc.rule, GCLineWidth, &values);
    }
#endif
}


/*
 *	Print the line defined by previous path commands
 */

static void
flush_path(void)
{
    int i;
    int last_min_x, last_max_x, last_min_y, last_max_y;

    last_min_x = 30000;
    last_min_y = 30000;
    last_max_x = -30000;
    last_max_y = -30000;
    for (i = 1; i < path_len; i++) {
	if (xx[i] > last_max_x)
	    last_max_x = xx[i];
	if (xx[i] < last_min_x)
	    last_min_x = xx[i];
	if (yy[i] > last_max_y)
	    last_max_y = yy[i];
	if (yy[i] < last_min_y)
	    last_min_y = yy[i];
	line_btw(xx[i], yy[i], xx[i + 1], yy[i + 1]);
    }
    if (xx[path_len] > last_max_x)
	last_max_x = xx[path_len];
    if (xx[path_len] < last_min_x)
	last_min_x = xx[path_len];
    if (yy[path_len] > last_max_y)
	last_max_y = yy[path_len];
    if (yy[path_len] < last_min_y)
	last_min_y = yy[path_len];
    path_len = 0;
    do_attribute_path(last_min_x, last_max_x, last_min_y, last_max_y);
}


/*
 *	Print a dashed line along the previously defined path, with
 *	the dashes/inch defined.
 */

static void
flush_dashed(char *cp, Boolean dotted)
{
    int i;
    int numdots;
    int lx0, ly0, lx1, ly1;
    int cx0, cy0, cx1, cy1;
    float inchesperdash;
    double d, spacesize, a, b = 0.0, dx, dy, milliperdash;

    if (sscanf(cp, " %f ", &inchesperdash) != 1) {
	XDVI_WARNING((stderr, "invalid format for dotted/dashed line: %s", cp));
	return;
    }
    if (path_len <= 1 || inchesperdash <= 0.0) {
	XDVI_WARNING((stderr, "invalid conditions for dotted/dashed line"));
	return;
    }
    milliperdash = inchesperdash * 1000.0;
    lx0 = xx[1];
    ly0 = yy[1];
    lx1 = xx[2];
    ly1 = yy[2];
    dx = lx1 - lx0;
    dy = ly1 - ly0;
    if (dotted) {
	numdots = sqrt(dx * dx + dy * dy) / milliperdash + 0.5;
	if (numdots == 0)
	    numdots = 1;
	for (i = 0; i <= numdots; i++) {
	    a = (float)i / (float)numdots;
	    cx0 = lx0 + a * dx + 0.5;
	    cy0 = ly0 + a * dy + 0.5;
	    dot_at(cx0, cy0);
	}
    }
    else {
	d = sqrt(dx * dx + dy * dy);
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
    path_len = 0;
}


/*
 *	Add a point to the current path
 */

static void
add_path(char *cp)
{
    int pathx, pathy;

    if (++path_len >= MAXPOINTS)
	XDVI_FATAL((stderr, "Too many points"));
    if (sscanf(cp, " %d %d ", &pathx, &pathy) != 2)
	XDVI_FATAL((stderr, "Malformed path command"));
    xx[path_len] = pathx;
    yy[path_len] = pathy;
}


/*
 *	Draw to a floating point position
 */

static void
im_fdraw(double x, double y)
{
    if (++path_len >= MAXPOINTS)
	XDVI_FATAL((stderr, "Too many arc points"));
    xx[path_len] = x + 0.5;
    yy[path_len] = y + 0.5;
}


/*
 *	Draw an ellipse with the indicated center and radices.
 */

static void
draw_ellipse(int xc, int yc, int xr, int yr)
{
    double angle, theta;
    int n;
    int px0, py0, px1, py1;

    angle = (xr + yr) / 2.0;
    theta = sqrt(1.0 / angle);
    n = TWOPI / theta + 0.5;
    if (n < 12)
	n = 12;
    else if (n > 80)
	n = 80;
    n /= 2;
    theta = TWOPI / n;

    angle = 0.0;
    px0 = xc + xr;	/* cos(0) = 1 */
    py0 = yc;	/* sin(0) = 0 */
    while ((angle += theta) <= TWOPI) {
	px1 = xc + xr * cos(angle) + 0.5;
	py1 = yc + yr * sin(angle) + 0.5;
	line_btw(px0, py0, px1, py1);
	px0 = px1;
	py0 = py1;
    }
    line_btw(px0, py0, xc + xr, yc);
}

/*
 *	Draw an arc
 */

static void
arc(char *cp, Boolean invis)
{
    int xc, yc, xrad, yrad, n;
    float start_angle, end_angle, angle, theta, r;
    double xradius, yradius, xcenter, ycenter;

    n = sscanf(cp, " %d %d %d %d %f %f ", &xc, &yc, &xrad, &yrad,
	       &start_angle, &end_angle);

    if (n != 6) {
	XDVI_WARNING((stderr, "invalid arc specification: %s", cp));
	return;
    }

    if (invis)
	return;

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
#if BUG_888087_FIXED
    n = (pen_size * TWOPI) / (theta * currwin.shrinkfactor) + 0.5;
#else
    n = 0.3 * TWOPI / theta + 0.5;
#endif
    if (n < 12)
	n = 12;
    else if (n > 80)
	n = 80;
    n /= 2;
    theta = TWOPI / n;
    flush_path();
    im_fdraw(xcenter + xradius * cos(start_angle),
	     ycenter + yradius * sin(start_angle));
    angle = start_angle + theta;
    if (end_angle < start_angle)
	end_angle += TWOPI;
    while (angle < end_angle) {
	im_fdraw(xcenter + xradius * cos(angle),
		 ycenter + yradius * sin(angle));
	angle += theta;
    }
    im_fdraw(xcenter + xradius * cos(end_angle),
	     ycenter + yradius * sin(end_angle));
    flush_path();
}


/*
 *	APPROXIMATE integer distance between two points
 */

#define	dist(x0, y0, x1, y1)	(abs(x0 - x1) + abs(y0 - y1))


/*
 *	Draw a spline along the previously defined path
 */

static void
flush_spline(void)
{
    int xp, yp;
    int N;
    int lastx = -1, lasty = -1;
    Boolean lastvalid = False;
    int t1, t2, t3;
    int steps;
    int j;
    int i, w;

    N = path_len + 1;
    xx[0] = xx[1];
    yy[0] = yy[1];
    xx[N] = xx[N - 1];
    yy[N] = yy[N - 1];
    for (i = 0; i < N - 1; i++) {	/* interval */
	steps = (dist(xx[i], yy[i], xx[i + 1], yy[i + 1]) +
		 dist(xx[i + 1], yy[i + 1], xx[i + 2], yy[i + 2])) / 80;
	for (j = 0; j < steps; j++) {	/* points within */
	    w = (j * 1000 + 500) / steps;
	    t1 = w * w / 20;
	    w -= 500;
	    t2 = (750000 - w * w) / 10;
	    w -= 500;
	    t3 = w * w / 20;
	    xp =
		(t1 * xx[i + 2] + t2 * xx[i + 1] + t3 * xx[i] + 50000) / 100000;
	    yp =
		(t1 * yy[i + 2] + t2 * yy[i + 1] + t3 * yy[i] + 50000) / 100000;
	    if (lastvalid)
		line_btw(lastx, lasty, xp, yp);
	    lastx = xp;
	    lasty = yp;
	    lastvalid = True;
	}
    }
    path_len = 0;
}


/*
 *	Shade the last box, circle, or ellipse
 */

static void
shade_last(void)
{
    blacken = whiten = False;
    shade = True;
}


/*
 *	Make the last box, circle, or ellipse, white inside (shade with white)
 */

static void
whiten_last(void)
{
    whiten = True;
    blacken = shade = False;
}


/*
 *	Make last box, etc, black inside
 */

static void
blacken_last(void)
{
    blacken = True;
    whiten = shade = False;
}


/*
 *	Code for PostScript<tm> specials begins here.
 */

#if PS

/*
 *	Information on how to search for PS header and figure files.
 */

static void ps_startup(int, int, const char *);
static void ps_startup2(void);

/* dummy procedures */
static void NullProc(void) { }
static void NullProcInt(int flag) { UNUSED(flag); }
static void NullProcStr(const char *cp) { UNUSED(cp); }


struct psprocs psp = {	/* used for lazy startup of the ps machinery */
    /* toggle */ NullProcInt,
    /* destroy */ NullProc,
    /* interrupt */ NullProc,
    /* endpage */ NullProc,
    /* drawbegin */ ps_startup,
    /* drawraw */ NullProcStr,
    /* drawfile */ NULL,
    /* drawend */ NullProcStr,
    /* beginheader */ ps_startup2,
    /* endheader */ NullProc,
    /* newdoc */ NullProc
};

struct psprocs no_ps_procs = {	/* used if postscript is unavailable */
    /* toggle */ NullProcInt,
    /* destroy */ NullProc,
    /* interrupt */ NullProc,
    /* endpage */ NullProc,
    /* drawbegin */ drawbegin_none,
    /* drawraw */ NullProcStr,
    /* drawfile */ NULL,
    /* drawend */ NullProcStr,
    /* beginheader */ NullProc,
    /* endheader */ NullProc,
    /* newdoc */ NullProc
};

#endif /* PS */

#ifdef MAGICK
int bbox_angle;
Boolean bbox_valid;
unsigned int bbox_width;
unsigned int bbox_height;
int bbox_voffset;
#else
static int bbox_angle;
static Boolean bbox_valid;
static unsigned int bbox_width;
static unsigned int bbox_height;
static int bbox_voffset;
#endif

/* info on bboxes on this page */
struct bbox_info {
    int x;
    int y;
    int w;
    int h;
    int angle;
};
    
static struct bbox_info *g_bbox_info = NULL;
static size_t g_bbox_info_size = 0;
static size_t g_bbox_info_max_size = 0;

/*
  Append the current coordinates to g_bbox_info, unless it already
  contains these coordinates.
*/

static void
append_bbox_info(int x, int y, int w, int h, int angle)
{
    Boolean found = False;
    size_t i;
    const size_t SIZE_STEP = 16;
    
    /* is this box already present? */
    for (i = 0; i < g_bbox_info_size; i++) {
	if (g_bbox_info[i].x == x
	    && g_bbox_info[i].y == y
	    && g_bbox_info[i].w == w
	    && g_bbox_info[i].h == h
	    && g_bbox_info[i].angle == angle) {
	    found = True;
	    break;
	}
    }

    if (!found) {
	g_bbox_info_size++;
	
	while (g_bbox_info_size >= g_bbox_info_max_size) {
	    g_bbox_info_max_size += SIZE_STEP;
	}
	
	g_bbox_info = xrealloc(g_bbox_info, g_bbox_info_max_size * sizeof *g_bbox_info);
	g_bbox_info[g_bbox_info_size - 1].x = x;
	g_bbox_info[g_bbox_info_size - 1].y = y;
	g_bbox_info[g_bbox_info_size - 1].w = w;
	g_bbox_info[g_bbox_info_size - 1].h = h;
	g_bbox_info[g_bbox_info_size - 1].angle = angle;
    }
}

static void
draw_bbox0(int xcorner, int ycorner)
{
    if (bbox_valid) {
#if COLOR
	if (fg_active != fg_current)
	    do_color_change();
#endif

	if (bbox_angle == 0) {
	    ycorner -= bbox_voffset;
	    XDrawRectangle(DISP, currwin.win, globals.gc.high, xcorner, ycorner, bbox_width, bbox_height);
	    if (resource.postscript == 0) {
		if (htex_inside_href) {
		    htex_set_anchorsize(xcorner, ycorner, xcorner + bbox_width, ycorner + bbox_height);
		    htex_set_objecttype(HTEX_IMG);
		}
	    }
	}
	else {
	    float sin_a = sin(bbox_angle * (TWOPI / 360));
	    float cos_a = cos(bbox_angle * (TWOPI / 360));
	    float a, b, c, d;

	    a = cos_a * bbox_width;
	    b = -sin_a * bbox_width;
	    c = -sin_a * bbox_height;
	    d = -cos_a * bbox_height;

	    XDrawLine(DISP, currwin.win, globals.gc.high,
		      xcorner, ycorner,
		      xcorner + (int)rint(a), ycorner + (int)rint(b));
	    XDrawLine(DISP, currwin.win, globals.gc.high,
		      xcorner + (int)rint(a), ycorner + (int)rint(b),
		      xcorner + (int)rint(a + c), ycorner + (int)rint(b + d));
	    XDrawLine(DISP, currwin.win, globals.gc.high,
		      xcorner + (int)rint(a + c), ycorner + (int)rint(b + d),
		      xcorner + (int)rint(c), ycorner + (int)rint(d));
	    XDrawLine(DISP, currwin.win, globals.gc.high,
		      xcorner + (int)rint(c), ycorner + (int)rint(d),
		      xcorner, ycorner);
	}
	bbox_valid = False;
    }
}

/*
  Display the items in bbox_info.
  Invoked from draw_page() in dvi-draw.c.
*/
void
display_bboxes(void)
{
    size_t i;
    
    for (i = 0; i < g_bbox_info_size; i++) {
	if (globals.debug & DBG_PS) {
	    fprintf(stderr, "drawing bbox %lu at %d %d, %d x %d, angle %d\n",
		    (unsigned long)i,
		    g_bbox_info[i].x,
		    g_bbox_info[i].y,
		    g_bbox_info[i].w,
		    g_bbox_info[i].h,
		    g_bbox_info[i].angle);
	}
#if 0
	XDrawRectangle(DISP, currwin.win, globals.gc.high,
		       g_bbox_info[i].x,
		       g_bbox_info[i].y,
		       g_bbox_info[i].w,
		       g_bbox_info[i].h);
#else
	bbox_valid = True;
	bbox_width = g_bbox_info[i].w;
	bbox_height = bbox_voffset = g_bbox_info[i].h;
	bbox_angle = g_bbox_info[i].angle;
	draw_bbox0(g_bbox_info[i].x, g_bbox_info[i].y + bbox_height);
#endif
    }
    bbox_angle = 0;
    bbox_valid = False;
}

void
clear_bboxes(void)
{
    free(g_bbox_info);
    g_bbox_info = NULL;
    g_bbox_info_size = 0;
    g_bbox_info_max_size = 0;
}

void
save_bbox(void)
{
    int xcorner, ycorner;

    if (bbox_valid) {
	xcorner = PXL_H - currwin.base_x;
	ycorner = PXL_V - currwin.base_y;
	
	ycorner -= bbox_voffset;
	append_bbox_info(xcorner, ycorner, bbox_width, bbox_height, bbox_angle);

	/* register boundaries of this box as anchor boundaries */
	if (htex_inside_href) {
	    htex_set_anchorsize(xcorner, ycorner, xcorner + bbox_width, ycorner + bbox_height);
	    htex_set_objecttype(HTEX_IMG);
	}
    }
}


void
draw_bbox(void)
{
    draw_bbox0(PXL_H - currwin.base_x, PXL_V - currwin.base_y);
}

#if PS

static XIOErrorHandler oldhandler;

static int XDviIOErrorHandler(Display * disp)
{
    ps_destroy();
    return oldhandler(disp);
}

static void
actual_startup(int flag)
{
    UNUSED(flag);
    oldhandler = XSetIOErrorHandler(XDviIOErrorHandler);

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
#endif /* PS_DPS */
#ifdef	PS_NEWS
	  (resource.useNeWS && initNeWS())
#ifdef	PS_GS
	  ||
#endif
#endif /* PS_NEWS */
#ifdef	PS_GS
	  (resource.useGS && initGS())
#endif
	  ))
	psp = no_ps_procs;
}

static void
ps_startup(int xul, int yul, const char *cp)
{
    if (resource.postscript == 0) {
	draw_bbox();
	return;
    }
    actual_startup(0);
    psp.drawbegin(xul, yul, cp);
}

static void
ps_startup2(void)
{
    actual_startup(0);
    psp.beginheader();
}


/*
 * dumb parsing of PostScript - search for rotation H. Zeller 1/97
 * Returns true if we find a potentially non-supported command that
 * we want to warn users about.
 */
static Boolean
ps_parseraw(const char *PostScript_cmd)
{
    const char *p;

    bbox_angle = 0;
    p = strstr(PostScript_cmd, "rotate");
    if (p != NULL) {
	while (*p != '\0' && !isdigit((int)*p))
	    --p;
	while (*p != '\0' && isdigit((int)*p))
	    --p;
	if (*p != '+' && *p != '-')
	    ++p;
	sscanf(p, "%d neg rotate", &bbox_angle);
	return True;
    }
    if (strstr(PostScript_cmd, " scale ") != NULL)
	return True;
    return False;
}


void
drawbegin_none(int xul, int yul, const char *cp)
{
    UNUSED(xul);
    UNUSED(yul);
    UNUSED(cp);
    
    draw_bbox();
}


/*
 *	Mechanism to keep track of included PS files.
 */

struct avl_psinfo {
    AVL_COMMON;
    char *fullpath;
    char *tempname;
    dev_t ps_dev;
    ino_t ps_ino;
    time_t ps_mtime;
    Boolean is_new;	/* set if previous use failed because of !allow_shell */
};

static struct avl_psinfo *psinfo_head = NULL;


/*
 *	ps_clear_cache() resets the cache to the empty state.
 */

static void
clear_psinfo_node(struct avl_psinfo *psinfp)
{
    if (psinfp->fullpath != NULL) {
	free(psinfp->fullpath);
	psinfp->fullpath = NULL;
    }
    if (psinfp->tempname != NULL) {
	if (unlink(psinfp->tempname) < 0)
	    perror(psinfp->tempname);
	free(psinfp->tempname);
	psinfp->tempname = NULL;
    }
}

static void
clear_psinfo_tree(struct avl_psinfo *psinfp)
{
    if (psinfp == NULL)
	return;

    clear_psinfo_tree((struct avl_psinfo *) psinfp->left);
    clear_psinfo_tree((struct avl_psinfo *) psinfp->right);
    free((char *) psinfp->key);
    clear_psinfo_node(psinfp);
    free(psinfp);
}

static void
ps_clear_cache(void)
{
    clear_psinfo_tree(psinfo_head);
    psinfo_head = NULL;
}


/*
 *	ps_clear_cache_nofree() does the same thing as ps_clear_cache(), but
 *	without ever calling free().  This is because it may be called
 *	following a segmentation fault, in which case the heap may be corrupted.
 */

static void
clear_psinfo_node_nofree(struct avl_psinfo *psinfp)
{
    if (psinfp->fullpath != NULL) {
	psinfp->fullpath = NULL;
    }
    if (psinfp->tempname != NULL) {
	if (unlink(psinfp->tempname) < 0)
	    perror(psinfp->tempname);
	psinfp->tempname = NULL;
    }
}

static void
clear_psinfo_tree_nofree(struct avl_psinfo *psinfp)
{
    if (psinfp == NULL)
	return;

    clear_psinfo_tree_nofree((struct avl_psinfo *) psinfp->left);
    clear_psinfo_tree_nofree((struct avl_psinfo *) psinfp->right);
    clear_psinfo_node_nofree(psinfp);
}

static void
ps_clear_cache_nofree(void)
{
    clear_psinfo_tree_nofree(psinfo_head);
    psinfo_head = NULL;
}


/*
 *	ps_new_tempfile() - Opens a new temporary file.
 *		Returns the fd, or -1 if an error occurred.
 */

static int
ps_new_tempfile(struct avl_psinfo *psinfp)
{
    int fd;

    fd = xdvi_temp_fd(&psinfp->tempname);
    if (fd == -1) {
	XDVI_ERROR((stderr,
	  "Cannot create temporary file for PostScript figure file: %s",
	  strerror(errno)));
	free(psinfp->tempname);
	psinfp->tempname = NULL;
    }
    return fd;
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

static void send_ps_file(const char *filename, kpse_file_format_type pathinfo);

static void
enable_specials_send_ps_file(XtPointer data)
{
    const char *filename = (const char *)data;
    resource.allow_shell = True;
    redraw_page(); /* to erase the bounding box */
    statusline_info(STATUS_MEDIUM,
		     "Shell specials enabled for this session.",
		     filename);
    send_ps_file(filename, kpse_pict_format);
}

static void
try_open_tempname(int status, struct xchild *this)
{
    if (WIFEXITED(status)) { /* child exited normally */
	if (WEXITSTATUS(status) != 0) {
	    /* default error procedure */
	    handle_child_exit(status, this);
	    this = NULL;	/* prevent freeing it twice */
	}
	else {
	    struct avl_psinfo *psinfp = (struct avl_psinfo *) this->data;
	    FILE *f = XFOPEN(psinfp->tempname, OPEN_MODE);

	    fprintf(stderr, "FILE: %s\n", psinfp->tempname);
	    if (f == NULL) {
		perror(psinfp->tempname);
	    }
	    else {
		fprintf(stderr, "sending file: %s, %p\n", psinfp->tempname, (void *)f);
		/* There's no point in invoking
		   psp.drawfile(psinfp->tempname, f);
		   here, since usually it will be already too late (draw_part() which
		   had called us via applicationDoSpecial() will already have terminated).
		   So instead, we just close the file and force a redraw of the entire page.
		*/
		fclose(f);
		globals.ev.flags |= EV_NEWPAGE;
	    }
	}
    }
    else if (WIFSIGNALED(status)) {
	popup_message(globals.widgets.top_level,
		      MSG_WARN,
		      NULL,
		      "Process `%s' terminated abnormally with signal %d.", this->name, WTERMSIG(status));
    }
    else if (WIFSTOPPED(status)) {
	popup_message(globals.widgets.top_level,
		      MSG_WARN,
		      NULL,
		      "Process `%s' stopped by signal %d.", this->name, WSTOPSIG(status));
    }
    else {
	popup_message(globals.widgets.top_level,
		      MSG_WARN,
		      NULL,
		      "Process `%s' terminated with unknown status.", this->name);
    }
    if (this != NULL) {	/* if it wasn't freed already */
	free(this->name);
	free(this->io);
	free(this);
    }
}

static void
send_ps_file(const char *filename, kpse_file_format_type pathinfo)
{
    FILE *f;
    int fd;
    static const char *argv[] = { NULL, "-c", NULL, NULL };
    char *bufp = NULL;
    size_t len;
    struct avl_psinfo *volatile psinfp;
    char magic1, magic2, magic3;
    static Boolean warned_about_shellescape = False;
    static size_t ffline_len = 0;
    static char *ffline = NULL;
    const size_t FFLINE_STEP = 128;

    if (psp.drawfile == NULL || resource.postscript == 0) {
	return;
    }

    len = strlen(filename);
    psinfp = (struct avl_psinfo *) avladd(filename, len,
      (struct avl **) &psinfo_head, sizeof(struct avl_psinfo));

    if (psinfp->key == filename) {	/* if new record */
	psinfp->is_new = True;
	psinfp->key = xmemdup(filename, len + 1);
	psinfp->fullpath = psinfp->tempname = NULL;
    }

    if (filename[0] == '`') {
	/* To test this code, use \special{psfile="`cat file.ps"} */
	if (!resource.allow_shell) {
	    if (!warned_about_shellescape) {
		choice_dialog_sized(globals.widgets.top_level,
				    MSG_INFO,
				    SIZE_MEDIUM,
				    /* helptext */
				    "To enable shell specials, use the \"-allowshell\" command "
				    "line option.\n\n"
				    "WARNING: You should NOT use shell specials like\n"
				    "`gunzip -c file.eps.gz\n"
				    "(e.g. via \\DeclareGraphicsRule{...}) "
				    "to uncompress .eps.gz files, even though some obsolete "
				    "LaTeX documentation might suggest it. Current versions "
				    "of xdvi and dvips will handle .eps.gz files just fine without this trick.\n",
#ifndef MOTIF
				    NULL,
#endif
				    NULL, NULL, /* no pre_callbacks */
				    /* Cancel label/callback */
				    "Cancel", NULL, (XtPointer)NULL,
				    /* Enable label/callback */
				    "Enable", enable_specials_send_ps_file, (XtPointer)filename,
				    /* msg */
				    "This page contains a shell special \"%s\", but execution of shell specials "
				    "is disabled.\n\nYou can now click "
				    "\"Enable\" to enable shell specials for this session only, "
				    "\"Cancel\" to leave the specials disabled, or \"Help\" for more help "
				    "on this topic.\n\n"
				    "Please note that shell specials are a security risk, since they "
				    "allow execution of arbitrary shell commands from the TeX file. "
				    "You should enable them only for DVI files that you created yourself.",
				    filename);
		warned_about_shellescape = True;
	    }
	    else {
		statusline_error(STATUS_MEDIUM,
				 "Info: Shell special \"%s\" disabled.",
				 filename);
	    }
	    draw_bbox();
	    return;
	}

	if (!psinfp->is_new) {	/* if we already have it */
	    if (psinfp->tempname == NULL) {	/* if previously known error */
		draw_bbox();
		return;
	    }
	    f = XFOPEN(psinfp->tempname, OPEN_MODE);
	    if (f == NULL) {
		perror(psinfp->tempname);
		/* WARN1(XmDIALOG_ERROR,
		  "Cannot open temporary PostScript file:\n%s",
		  psinfp->tempname); */
		clear_psinfo_node(psinfp);
		draw_bbox();
		return;
	    }
	    psp.drawfile(psinfp->tempname, f);
	}
	else {	/* it is new */
	    char *argv[4];

	    psinfp->is_new = False;

	    /* Create the file (it wasn't cached).
	       Fork so that we can collect the process's error messages. */

	    fd = ps_new_tempfile(psinfp);
	    if (fd == -1) {	/* if error */
		draw_bbox();
		return;
	    }

	    /* FIXME: Insecure - this is a /tmp race;
	       shouldn't close(fd), just re-use it */
	    close(fd);

	    len = strlen(filename) + strlen(psinfp->tempname) + (4 - 1);
	    if (len > ffline_len) {
		ffline_len += FFLINE_STEP;
		ffline = xrealloc(ffline, ffline_len);
	    }
	    
	    sprintf(ffline, "%s > %s", filename + 1, psinfp->tempname);
	    argv[0] = "/bin/sh";
	    argv[1] = "-c";
	    argv[2] = ffline;
	    argv[3] = NULL;
	    fork_process("/bin/sh", False, globals.dvi_file.dirname,
	      try_open_tempname, psinfp, -SIGKILL, argv);
	}
    }
    else {	/* not shell escape */
	char *expanded_filename = NULL;
	struct stat statbuf;

	if (psinfp->fullpath != NULL) {	/* if existing rec., no error */
	    /* Check file modification time */
	    if (stat(psinfp->fullpath, &statbuf) != 0) {
		perror(psinfp->fullpath);
		if (psinfp->ps_mtime != 0) {
		    clear_psinfo_node(psinfp);
		    psinfp->is_new = True;
		}
	    }
	    else {
		if (psinfp->ps_dev != statbuf.st_dev
		  || psinfp->ps_ino != statbuf.st_ino
		  || psinfp->ps_mtime != statbuf.st_mtime) {
		    clear_psinfo_node(psinfp);
		    psinfp->is_new = True;
		}
	    }
	}

	if (!psinfp->is_new) {
	    bufp = psinfp->tempname;
	    if (bufp == NULL) {
		bufp = psinfp->fullpath;
		if (bufp == NULL) {		/* if error already noted */
		    draw_bbox();
		    return;
		}
	    }
	    f = XFOPEN(bufp, OPEN_MODE);
	    if (f == NULL) {
		XDVI_WARNING((stderr, "Could not find graphics or temporary file \"%s\"",
			      bufp));
		statusline_info(STATUS_MEDIUM, "Warning: Could not find graphics or temporary file \"%s\"",
				bufp);
		clear_psinfo_node(psinfp);
		draw_bbox();
		return;
	    }
	}
	else {	/* node is new */
	    psinfp->is_new = False;

	    expanded_filename = find_file(filename, &statbuf, pathinfo);

	    if (expanded_filename == NULL &&
		(pathinfo == kpse_enc_format || pathinfo == kpse_type1_format))
	    {
		/* in this case, we also kpathsea-search in the `old' place for
		   backwards compatibility: kpse_tex_ps_header_format
		   (see comment for load_vector(), dvi-draw.c for details) */
		expanded_filename = kpse_find_file(filename,
					kpse_tex_ps_header_format, True);
	    }
	    if (expanded_filename == NULL) {
		expanded_filename = kpse_find_file(filename,
					kpse_program_text_format, True);
	    }
	    if (expanded_filename == NULL) { /* still no success, complain */
		/* FIXME: this warning may be overwritten by warning about raw
		   PS specials, so additinally dump to stderr.
		   TODO: make statusline printing respect more important
		   messages. */
		XDVI_WARNING((stderr, "Could not find graphics file \"%s\"",
			      filename));
		statusline_info(STATUS_MEDIUM,
			         "Warning: Could not find graphics file \"%s\"",
				 filename);
		draw_bbox();
		return;
	    }
	    if (globals.debug & DBG_OPEN)
		printf("%s:%d: |%s| expanded to |%s|\n", __FILE__, __LINE__,
		       filename, expanded_filename);

	    f = XFOPEN(expanded_filename, OPEN_MODE);
	    if (f == NULL) {
		XDVI_WARNING((stderr, "Could not open graphics file \"%s\": %s",
			      expanded_filename, strerror(errno)));
		statusline_info(STATUS_MEDIUM,
			"Warning: Could not open graphics file \"%s\": %s",
			expanded_filename, strerror(errno));
		free(expanded_filename);
		draw_bbox();
		return;
	    }

	    bufp = psinfp->fullpath = expanded_filename;

	    if (fstat(fileno(f), &statbuf) == 0) {	/* get mod. time */
		psinfp->ps_dev = statbuf.st_dev;
		psinfp->ps_ino = statbuf.st_ino;
		psinfp->ps_mtime = statbuf.st_mtime;
	    }
	    else {
		perror(bufp);
		psinfp->ps_dev = 0;
		psinfp->ps_ino = 0;
		psinfp->ps_mtime = 0;
	    }

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
		if (getc(f) != magic1 || (char)getc(f) != magic2
		    || (magic3 != '\0' && getc(f) != magic3)) {
		    rewind(f);
		}
		else {
		    pid_t pid;
		    int status;

		    fclose(f);

		    fd = ps_new_tempfile(psinfp);
		    if (fd == -1) {	/* if error */
			clear_psinfo_node(psinfp);
			return;
		    }
		    argv[2] = bufp;
		    fflush(stderr);	/* avoid double flushing */
		    pid = vfork();
		    if (pid == 0) {	/* if child */
			(void)dup2(fd, 1);
			(void)execvp(argv[0], (char **)argv);
			XDVI_ERROR((stderr, "Execvp of %s failed: %s", argv[0],
				    strerror(errno)));
			_exit(EXIT_FAILURE);
		    }
		    (void)close(fd);

		    for (;;) {
#if HAVE_WAITPID
			if (waitpid(pid, &status, 0) != -1)
			    break;
#else
# if HAVE_WAIT4
			if (wait4(pid, &status, 0, (struct rusage *)NULL)
			    != -1)
			    break;
# else
			int retval;

			retval = wait(&status);
			if (retval == pid)
			    break;
			if (retval != -1)
			    continue;
# endif	/* HAVE_WAIT4 */
#endif /* HAVE_WAITPID */
			if (errno == EINTR)
			    continue;
			perror("[xdvik] waitpid");
			return;
		    }
		    f = XFOPEN(psinfp->tempname, OPEN_MODE);
		    if (f == NULL) {
			perror(psinfp->tempname);
			draw_bbox();
			return;
		    }
		    bufp = psinfp->tempname;
		}
	    }
	}
	/* Success! */
	psp.drawfile(bufp, f); /* this is supposed to close the file */
    }
}


void
ps_destroy(void)
{
    /* Note:  old NeXT systems (at least) lack atexit/on_exit.  */
    psp.destroy();
    ps_clear_cache();
}


/*
 *	Same as above, but refrains from calling free() (may be called
 *	following a seg fault).
 */

void
ps_destroy_nofree(void)
{
    psp.destroy();
    ps_clear_cache_nofree();
}


#endif	/* PS */


void
init_prescan(void)
{
    scanned_page = scanned_page_reset = resource.prescan ? -1 : total_pages + 1;
#if PS
    scanned_page_ps = scanned_page_ps_bak = scanned_page;
#if COLOR
    scanned_page_color = scanned_page;
#endif /* COLOR */
#endif /* PS */

    TRACE_FILES((stderr, "init_prescan: scanned_page = %d", scanned_page));
#if PS
    if (resource.postscript == 0)
	scanned_page_ps = total_pages + 1;

    ps_clear_cache();
    psp.newdoc();
#endif

#if COLOR
    if (!resource.use_color)
	scanned_page_color = total_pages + 1;
#endif

    if (ignore_papersize_specials) {
#if PS && COLOR
	scanned_page = scanned_page_ps < scanned_page_color ? scanned_page_ps : scanned_page_color;
#elif PS
	scanned_page = scanned_page_ps;
#elif COLOR
	scanned_page = scanned_page_color;
#else
	scanned_page = total_pages + 1;
#endif
    }
}


static void
psfig_special(char *cp)
{
    char *filename;
    int raww, rawh;

    if (strncmp(cp, ":[begin]", 8) == 0) {
	cp += 8;
	bbox_valid = False;
	bbox_angle = 0;
	if (sscanf(cp, "%d %d\n", &raww, &rawh) >= 2) {
	    bbox_valid = True;
	    bbox_width = pixel_conv(spell_conv(raww));
	    bbox_height = pixel_conv(spell_conv(rawh));
	    bbox_voffset = 0;
	}
	if (INSIDE_MANE_WIN) {
#if	PS
	    psp.drawbegin(PXL_H - currwin.base_x, PXL_V - currwin.base_y, cp);
#else
	    draw_bbox();
#endif
	}
	psfig_begun = True;
    }
    else if (strncmp(cp, " plotfile ", 10) == 0) {
	cp += 10;
	while (isspace((int)*cp))
	    cp++;
	/* handle "`zcat file". Borrowed from dvipsk... */
	if (*cp == '"') {
	    cp++;
	    for (filename = cp; *cp && (*cp != '"'); ++cp);
	}
	else {
	    for (filename = cp; *cp && !isspace((int)*cp); ++cp);
	}
	*cp = '\0';
#if	PS
	if (INSIDE_MANE_WIN)
	    send_ps_file(filename, kpse_pict_format);
#endif
    }
    else if (strncmp(cp, ":[end]", 6) == 0) {
	cp += 6;
#if	PS
	if (INSIDE_MANE_WIN) {
	    psp.drawend(cp);
	}
#endif
	bbox_valid = False;
	psfig_begun = False;
    }
    else if (*cp == ':') {
	/* I am going to send some raw postscript stuff */
	++cp;	/* skip the colon */
#if	PS
	if (ps_parseraw(cp))
	    have_raw_postscript = True;
	if (INSIDE_MANE_WIN)
	    psp.drawraw(cp);
#endif
    }
    else { /* attempt to parse pstricks color specials */
#if COLOR
	struct rgb color;
	if (parse_color(cp, cp, &color, True)) {
	    /* clear stack */
	    if (rcs_head == NULL) {
		rcs_head = xmalloc(sizeof *rcs_head);
		rcs_head->prev = rcs_head->next = NULL;
	    }
	    rcs_top = rcs_head;
	    color_bot_size = 0;
	    
	    /* Change top of stack */
	    rcs_top->color = color;
	    
	    set_fg_color(&color);
	}
#endif	    
	/* also raw PostScript, but no extra colon to skip */
#if PS
	if (INSIDE_MANE_WIN) {
	    if (ps_parseraw(cp))
		have_raw_postscript = True;
	    
	    if (psfig_begun)
		psp.drawraw(cp);
	    else {
		psp.drawbegin(PXL_H - currwin.base_x,
			      PXL_V - currwin.base_y, cp);
		psp.drawend("");
	    }
	}
#endif
    }
}


/*	Keys for epsf specials */

static const char *keytab[] = { "clip",
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
				"angle"
};

#define	KEY_LLX	keyval[0]
#define	KEY_LLY	keyval[1]
#define	KEY_URX	keyval[2]
#define	KEY_URY	keyval[3]
#define	KEY_RWI	keyval[4]
#define	KEY_RHI	keyval[5]

#define	NKEYS	(sizeof keytab /sizeof *keytab)
#define	N_ARGLESS_KEYS 1

static void
epsf_special(char *cp)
{
    char *filename;
    static char *buffer;
    static unsigned int buflen = 0;
    unsigned int len;
    char *q;
    int flags = 0;
    double keyval[6] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

    filename = cp;
    if (*cp == '\'' || *cp == '"') {
	do
	    ++cp;
	while (*cp != '\0' && *cp != *filename);
	++filename;
    }
    else
	while (*cp != '\0' && !isspace((int)*cp))
	    ++cp;
    if (*cp != '\0')
	*cp++ = '\0';
    while (isspace((int)*cp))
	++cp;
    len = strlen(cp) + NKEYS + 30;
    if (buflen < len) {
	if (buflen != 0)
	    free(buffer);
	buflen = len;
	buffer = xmalloc(buflen);
    }
    strcpy(buffer, "@beginspecial");
    q = buffer + strlen(buffer);
    while (*cp != '\0') {
	char *p1 = cp;
	size_t keyno;

	while (*p1 != '=' && !isspace((int)*p1) && *p1 != '\0')
	    ++p1;
	for (keyno = 0;; ++keyno) {
	    if (keyno >= NKEYS) {
		if (globals.warn_spec_now)
		    XDVI_WARNING((stderr, "Ignoring unknown keyword (%.*s) in \\special",
				  (int)(p1 - cp), cp));
		break;
	    }
	    if (memcmp(cp, keytab[keyno], p1 - cp) == 0) {
		if (keyno >= N_ARGLESS_KEYS) {
		    while (isspace((int)*p1))
			++p1;
		    if (*p1 == '=') {
			++p1;
			while (isspace((int)*p1))
			    ++p1;
		    }
		    if (keyno < N_ARGLESS_KEYS + 6) {
			keyval[keyno - N_ARGLESS_KEYS] = atof(p1);
			flags |= (1 << (keyno - N_ARGLESS_KEYS));
		    }
		    *q++ = ' ';
		    while (!isspace((int)*p1) && *p1 != '\0')
			*q++ = *p1++;
		}
		*q++ = ' ';
		*q++ = '@';
		strcpy(q, keytab[keyno]);
		q += strlen(q);
		break;
	    }
	}
	cp = p1;
	while (!isspace((int)*cp) && *cp != '\0')
	    ++cp;
	while (isspace((int)*cp))
	    ++cp;
    }
    strcpy(q, " @setspecial\n");

    bbox_valid = False;
    /* Validate the case where both rwi and rhi are undefined
     * (and llx, lly, urx, ury are properly defined) */
    if (!(flags & 0x30) && (flags & 0xf) == 0xf) {
	KEY_RWI = (KEY_URX - KEY_LLX) * 10;
	flags |= 0x10;
    }
    if ((flags & 0x30) == 0x30 || ((flags & 0x30) && (flags & 0xf) == 0xf)) {
	bbox_valid = True;
	bbox_width = 0.1 * ((flags & 0x10) ? KEY_RWI
			    : KEY_RHI * (KEY_URX - KEY_LLX) / (KEY_URY -
							       KEY_LLY)) *
	    dimconv / currwin.shrinkfactor + 0.5;
	bbox_voffset = bbox_height =
	    0.1 * ((flags & 0x20) ? KEY_RHI : KEY_RWI * (KEY_URY - KEY_LLY) /
		   (KEY_URX - KEY_LLX))
	    * dimconv / currwin.shrinkfactor + 0.5;
    }

    if (INSIDE_MANE_WIN) {
#if	PS
	psp.drawbegin(PXL_H - currwin.base_x, PXL_V - currwin.base_y, buffer);
	/* talk directly with the DPSHandler here */
	send_ps_file(filename, kpse_pict_format);
	psp.drawend(" @endspecial");
#else
	draw_bbox();
#endif
    }
    bbox_valid = False;
}


static void
quote_special(char *cp)
{
    bbox_valid = False;

#if	PS
    if (currwin.win == mane.win) {
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

static void
scan_header(char *cp)
{
    char *filename;
    /* default for .pro files: */
    kpse_file_format_type our_format;
    
#if PS_GS
    if (gs_postpone_prescan)
	return;
#endif
    filename = cp;
    if (*cp == '\'' || *cp == '"') {
	do
	    ++cp;
	while (*cp != '\0' && *cp != *filename);
	*cp = '\0';
	++filename;
    }

    psp.beginheader();
    /* check for suffixes other than `.pro' (default PS headers),
       case-insensitive */
    if (str_is_suffix(".pfa", filename, False)
	|| str_is_suffix(".pfb", filename, False))
	our_format = kpse_type1_format;
    else if (str_is_suffix(".enc", filename, False))
	our_format = kpse_enc_format;
    else
	our_format = kpse_tex_ps_header_format;
    
    send_ps_file(filename, our_format);
}

static void
scan_bang(char *cp)
{
    psp.beginheader();
    psp.drawraw(cp + 1);
}

#endif /* PS */

#if COLOR

/*
 *	Table of dvips color names.  Produced by passing the table in
 *	dvipsk/color.lpro through the following perl script, and then
 *	through sort.
 *
 *	#! /usr/bin/perl -w
 *
 *	sub cvpart {
 *	    return $_[0] < 1.0 ? 1.0 - $_[0] : 0.0;
 *	}
 *	
 *	$pat = "^\\/(\\w+)\\s*\\{\\s*([0-9.]+)\\s*([0-9.]+)\\s*([0-9.]+)"
 *	  . "\\s*([0-9.]+)\\s*setcmykcolor\\s*\\}\\s*DC\\s*\$";
 *	while (<STDIN>) {
 *	    chop;
 *	    if (/^%%/) {next;}
 *	    if (/$pat/o) {
 *		printf "\t\tDVIPSCOLORDESC(%2d, \"%s\", %g, %g, %g),\n",
 *		  length($1), $1,
 *		  cvpart($2 + $5), cvpart($3 + $5), cvpart($4 + $5);
 *	    }
 *	    else {
 *		print "Bad line: ", $_, "\n";
 *	    }
 *	}
 */

struct dvipscolor {
    const char *name;
    struct rgb color;
};

static Boolean
parse_color(const char *cp0, const char *cp,
	    struct rgb *rgbp, Boolean generic_ps_flag)
{
    double r, g, b;
    double k;
    double hue, sat, bri;
    char dummy[8];

#define	CVPART(x)	((int)((x) * 65535 + 0.5))
#define	COLOR_DESC(name, r, g, b) \
		{name, {CVPART(r), CVPART(g), CVPART(b)}}

    /* hash table to speed up lookup in following list of color names */
    static hashTableT colornames_hash;
    /*
      Use a prime close to sizof colornames / sizeof colornames[0].
      You might want to update this when extending the colornames array,
      and to check the filling factor and average chain length by uncommenting the
      hash_print(colornames_hash, True);
      below. I usually go for filling > 50% and avg. chain len < 2.
      
      The 68-elem array below results in:
      79 buckets, 43 nonempty (54%); 68 entries, average chain 1.6.
    */
    static const size_t colornames_hash_size = 79;
    
    static struct dvipscolor colornames[] = {
	COLOR_DESC("Red", 1, 0, 0),
	COLOR_DESC("Tan", 0.86, 0.58, 0.44),
	COLOR_DESC("Blue", 0, 0, 1),
	COLOR_DESC("Cyan", 0, 1, 1),
	COLOR_DESC("Gray", 0.5, 0.5, 0.5),
	COLOR_DESC("Plum", 0.5, 0, 1),
	COLOR_DESC("Black", 0, 0, 0),
	COLOR_DESC("Brown", 0.4, 0, 0),
	COLOR_DESC("Green", 0, 1, 0),
	COLOR_DESC("Melon", 1, 0.54, 0.5),
	COLOR_DESC("Peach", 1, 0.5, 0.3),
	COLOR_DESC("Sepia", 0.3, 0, 0),
	COLOR_DESC("White", 1, 1, 1),
	COLOR_DESC("Maroon", 0.68, 0, 0),
	COLOR_DESC("Orange", 1, 0.39, 0.13),
	COLOR_DESC("Orchid", 0.68, 0.36, 1),
	COLOR_DESC("Purple", 0.55, 0.14, 1),
	COLOR_DESC("Salmon", 1, 0.47, 0.62),
	COLOR_DESC("Violet", 0.21, 0.12, 1),
	COLOR_DESC("Yellow", 1, 1, 0),
	COLOR_DESC("Apricot", 1, 0.68, 0.48),
	COLOR_DESC("Emerald", 0, 1, 0.5),
	COLOR_DESC("Fuchsia", 0.45, 0.01, 0.92),
	COLOR_DESC("Magenta", 1, 0, 1),
	COLOR_DESC("SkyBlue", 0.38, 1, 0.88),
	COLOR_DESC("Thistle", 0.88, 0.41, 1),
	COLOR_DESC("BrickRed", 0.72, 0, 0),
	COLOR_DESC("Cerulean", 0.06, 0.89, 1),
	COLOR_DESC("Lavender", 1, 0.52, 1),
	COLOR_DESC("Mahogany", 0.65, 0, 0),
	COLOR_DESC("Mulberry", 0.64, 0.08, 0.98),
	COLOR_DESC("NavyBlue", 0.06, 0.46, 1),
	COLOR_DESC("SeaGreen", 0.31, 1, 0.5),
	COLOR_DESC("TealBlue", 0.12, 0.98, 0.64),
	COLOR_DESC("BlueGreen", 0.15, 1, 0.67),
	COLOR_DESC("CadetBlue", 0.38, 0.43, 0.77),
	COLOR_DESC("Dandelion", 1, 0.71, 0.16),
	COLOR_DESC("Goldenrod", 1, 0.9, 0.16),
	COLOR_DESC("LimeGreen", 0.5, 1, 0),
	COLOR_DESC("OrangeRed", 1, 0, 0.5),
	COLOR_DESC("PineGreen", 0, 0.75, 0.16),
	COLOR_DESC("RawSienna", 0.55, 0, 0),
	COLOR_DESC("RedOrange", 1, 0.23, 0.13),
	COLOR_DESC("RedViolet", 0.59, 0, 0.66),
	COLOR_DESC("Rhodamine", 1, 0.18, 1),
	COLOR_DESC("RoyalBlue", 0, 0.5, 1),
	COLOR_DESC("RubineRed", 1, 0, 0.87),
	COLOR_DESC("Turquoise", 0.15, 1, 0.8),
	COLOR_DESC("VioletRed", 1, 0.19, 1),
	COLOR_DESC("Aquamarine", 0.18, 1, 0.7),
	COLOR_DESC("BlueViolet", 0.1, 0.05, 0.96),
	COLOR_DESC("DarkOrchid", 0.6, 0.2, 0.8),
	COLOR_DESC("OliveGreen", 0, 0.6, 0),
	COLOR_DESC("Periwinkle", 0.43, 0.45, 1),
	COLOR_DESC("Bittersweet", 0.76, 0.01, 0),
	COLOR_DESC("BurntOrange", 1, 0.49, 0),
	COLOR_DESC("ForestGreen", 0, 0.88, 0),
	COLOR_DESC("GreenYellow", 0.85, 1, 0.31),
	COLOR_DESC("JungleGreen", 0.01, 1, 0.48),
	COLOR_DESC("ProcessBlue", 0.04, 1, 1),
	COLOR_DESC("RoyalPurple", 0.25, 0.1, 1),
	COLOR_DESC("SpringGreen", 0.74, 1, 0.24),
	COLOR_DESC("YellowGreen", 0.56, 1, 0.26),
	COLOR_DESC("MidnightBlue", 0, 0.44, 0.57),
	COLOR_DESC("YellowOrange", 1, 0.58, 0),
	COLOR_DESC("CarnationPink", 1, 0.37, 1),
	COLOR_DESC("CornflowerBlue", 0.35, 0.87, 1),
	COLOR_DESC("WildStrawberry", 1, 0.04, 0.61),
    };
#undef CVPART
#undef COLOR_DESC

    UNUSED(cp0);
    
    while (*cp == ' ') ++cp;

    if (generic_ps_flag) {
        /*
	  check for pstricks color specials.
	  The dummy buffer is to ensure that there are no other (general PS)
	  commands following (we wouldn't know how to deal with these).
	*/
	if (sscanf(cp, "%lf %lf %lf setrgbcolor %7s", &r, &g, &b, dummy) == 3
	    && r >= 0 && r <= 1 && g >= 0 && g <= 1 && b >= 0 && b <= 1) {
	    rgbp->r = r * 65535 + 0.5;
	    rgbp->g = g * 65535 + 0.5;
	    rgbp->b = b * 65535 + 0.5;
	    return True;
	}
	else if (sscanf(cp, "%lf %lf %lf %lf setcmykcolor %7s", &r, &g, &b, &k, dummy) == 4
		 && r >= 0 && r <= 1 && g >= 0 && g <= 1 && b >= 0 && b <= 1
		 && k >= 0 && k <= 1) {
	    r = 1.0 - r - k;	/* cyan --> red */
	    rgbp->r = (r < 0 ? 0 : r * 65535 + 0.5);
	    g = 1.0 - g - k;	/* magenta --> green */
	    rgbp->g = (g < 0 ? 0 : g * 65535 + 0.5);
	    b = 1.0 - b - k;	/* yellow --> blue */
	    rgbp->b = (b < 0 ? 0 : b * 65535 + 0.5);
	    return True;
	}
	else if (sscanf(cp, "%lf %lf %lf sethsbcolor %7s", &hue, &sat, &bri, dummy) == 3 && hue >= 0
		 && hue <= 1 && sat >= 0 && sat <= 1 && bri >= 0 && bri <= 1) {
	    int	h	= (int) (6 * hue);
	    double	p	= bri * (1 - sat);
	    double	q	= bri * (1 - sat * (6 * hue - h));
	    double	t	= p - q + bri;
	    
	    switch (h) {
	    case 0:  r = bri; g = t; b = p; break; /* Red - Yel */
	    case 1:  r = q; g = bri; b = p; break; /* Yel - Grn */
	    case 2:  r = p; g = bri; b = t; break; /* Grn - Cyn */
	    case 3:  r = p; g = q; b = bri; break; /* Cyn - Blu */
	    case 4:  r = t; g = p; b = bri; break; /* Blu - Mag */
	    case 5:  r = bri; g = p; b = q; break; /* Mag - Red */
	    case 6:  r = bri; g = b = p;    break; /* Red */
	    }
	    rgbp->r = r * 65535 + 0.5;
	    rgbp->g = g * 65535 + 0.5;
	    rgbp->b = b * 65535 + 0.5;
	    return True;
	}
	else if (sscanf(cp, "%lf setgray %7s", &r, dummy) == 1 && r >= 0 && r <= 1) {
	    rgbp->r = rgbp->g = rgbp->b = r * 65535 + 0.5;
	    return True;
	}
	else
	    return False;
    }

    /* no generic PS command; must be a dvips color special */
    
    if (memicmp(cp, "rgb ", 4) == 0) {
	if (sscanf(cp + 3, "%lf %lf %lf", &r, &g, &b) == 3
	    && r >= 0 && r <= 1 && g >= 0 && g <= 1 && b >= 0 && b <= 1) {
	    rgbp->r = r * 65535 + 0.5;
	    rgbp->g = g * 65535 + 0.5;
	    rgbp->b = b * 65535 + 0.5;
	    return True;
	}
    }
    else if (memicmp(cp, "gray ", 5) == 0) {
	if (sscanf(cp + 4, "%lf", &r) == 1 && r >= 0 && r <= 1) {
	    rgbp->r = rgbp->g = rgbp->b = r * 65535 + 0.5;
	    return True;
	}
    }
    else if (memicmp(cp, "cmyk ", 5) == 0) {
	if (sscanf(cp + 4, "%lf %lf %lf %lf", &r, &g, &b, &k) == 4
	    && r >= 0 && r <= 1 && g >= 0 && g <= 1 && b >= 0 && b <= 1
	    && k >= 0 && k <= 1) {
	    r = 1.0 - r - k;	/* cyan --> red */
	    rgbp->r = (r < 0 ? 0 : r * 65535 + 0.5);
	    g = 1.0 - g - k;	/* magenta --> green */
	    rgbp->g = (g < 0 ? 0 : g * 65535 + 0.5);
	    b = 1.0 - b - k;	/* yellow --> blue */
	    rgbp->b = (b < 0 ? 0 : b * 65535 + 0.5);
	    return True;
	}
    }
    else if (memicmp(cp, "hsb ", 4) == 0) {
	if (sscanf(cp + 3, "%lf %lf %lf", &hue, &sat, &bri) == 3 && hue >= 0
	    && hue <= 1 && sat >= 0 && sat <= 1 && bri >= 0 && bri <= 1) {
	    int	h	= (int) (6 * hue);
	    double	p	= bri * (1 - sat);
	    double	q	= bri * (1 - sat * (6 * hue - h));
	    double	t	= p - q + bri;

	    switch (h) {
	    case 0:  r = bri; g = t; b = p; break; /* Red - Yel */
	    case 1:  r = q; g = bri; b = p; break; /* Yel - Grn */
	    case 2:  r = p; g = bri; b = t; break; /* Grn - Cyn */
	    case 3:  r = p; g = q; b = bri; break; /* Cyn - Blu */
	    case 4:  r = t; g = p; b = bri; break; /* Blu - Mag */
	    case 5:  r = bri; g = p; b = q; break; /* Mag - Red */
	    case 6:  r = bri; g = b = p;    break; /* Red */
	    }
	    rgbp->r = r * 65535 + 0.5;
	    rgbp->g = g * 65535 + 0.5;
	    rgbp->b = b * 65535 + 0.5;
	    return True;
	}
    }
    else { /* is it a dvips color name? */
	size_t idx;
	size_t len = 0;
	static char *testname = NULL;
	static size_t testname_len = 0;
	size_t testname_step = 16;
	
	if (colornames_hash.size == 0) { /* initialize hash table */
	    size_t i;
	    colornames_hash = hash_create(colornames_hash_size);
	    /* insert color names and array indexes */
	    for (i = 0; i < (sizeof colornames / sizeof colornames[0]); i++) {
		put_str_int_hash(&colornames_hash, colornames[i].name, i);
	    }
	}
	/* uncomment the following to get statistics on hashing: */
	/* hash_print(colornames_hash, True); */

	/* need to copy to a null-terminated string for hash lookup ... */
	while (isalpha((int)cp[len])) {
	    ++len;	/* get length of color name */
	}
	while (len >= testname_len) {
	    testname_len += testname_step;
	    testname = xrealloc(testname, testname_len);
	}
	memcpy(testname, cp, len);
	testname[len] = '\0';
	
	if (find_str_int_hash(&colornames_hash, testname, &idx)) {
	    *rgbp = colornames[idx].color;
	    return True;
	}
    }

    /* not found */
    XDVI_WARNING((stderr, "Ignoring invalid color name in special `%s'", cp0 == NULL ? "<NULL>" : cp0));
    return False;
}

void
init_page_colors(struct rgb *foreground, struct rgb *background)
{
    int	i;
    
    page_colors.size = total_pages;
    page_colors.stack = xmalloc(page_colors.size * sizeof *page_colors.stack);

    for (i = 0; i <= scanned_page + 1; ++i) {
	page_colors.stack[i].bg = *background;
	page_colors.stack[i].colorstack = foreground;
	page_colors.stack[i].stacksize = 1;
    }
    for (; i < total_pages; i++) {
	page_colors.stack[i].colorstack = NULL;
    }

    scanstack_head.color = *foreground;
    scanstack_len = 1;			/* nothing yet */
    scanstack_current = &scanstack_head;	/* stack position */
}


static void
scan_bg_color(const char *cp)
{
    if (!resource.use_color)
	return;

    if (page_colors.stack == NULL)
	init_page_colors(&fg_initial, &bg_initial);

    ASSERT(scanned_page < (int)page_colors.size, "page_colors.size too small");
    (void) parse_color(cp, cp + 11, &(page_colors.stack[scanned_page + 1].bg), False);
}

void
scan_color(const char *cp)
{
    const char	*cp1 = cp + 6;

    if (!resource.use_color)
	return;

    while (*cp1 == ' ') ++cp1;

    if (page_colors.stack == NULL)
	init_page_colors(&fg_initial, &bg_initial);

    if (memicmp(cp1, "push ", 5) == 0) {
	if (scanstack_current->next == NULL) {	/* if at end */
	    scanstack_current->next = xmalloc(sizeof *scanstack_current);
	    scanstack_current->next->prev = scanstack_current;
	    scanstack_current->next->next = NULL;
	}
	scanstack_current = scanstack_current->next;
	++scanstack_len;
	if (!parse_color(cp, cp1 + 5, &scanstack_current->color, False))
	    scanstack_current->color = scanstack_current->prev->color;
    }
    else if (memicmp(cp1, "pop", 3) == 0) {
	if (scanstack_len <= 1) {
	    XDVI_WARNING((stderr, "Color pop occurred with empty color stack."));
	}
	else {
	    scanstack_current = scanstack_current->prev;
	    --scanstack_len;
	}
    }
    else {
	(void) parse_color(cp, cp1, &scanstack_head.color, False);
	if (scanstack_len > 1) {
	    struct colorframe	*cfp;

	    XDVI_WARNING((stderr, "Global color change occurred with non-empty color stack!\n"
			  "Trying to recover by setting all stack entries to that color."));
	    for (cfp = scanstack_head.next;; cfp = cfp->next) {
		cfp->color = scanstack_head.color;
		if (cfp == scanstack_current) break;
	    }
	}
    }
}

void
scan_color_eop(void)
{
    int i;
    struct rgb *prev, *p1, *rgbp;
    struct colorframe *cf;

    if (page_colors.stack == NULL)
	return;

    /* set background color for next page */
    if (scanned_page + 1 < total_pages) {
	ASSERT(scanned_page + 1 < (int)page_colors.size, "page_colors.size too small");
	page_colors.stack[scanned_page + 1].bg = page_colors.stack[scanned_page].bg;
    }
    ASSERT(scanned_page < (int)page_colors.size, "page_colors.size too small");
    /* save the stack contents */
    page_colors.stack[scanned_page].stacksize = scanstack_len;

    prev = &fg_initial;
    i = 1;
    if (scanned_page > 0) {
	prev = page_colors.stack[scanned_page - 1].colorstack;
	i = page_colors.stack[scanned_page - 1].stacksize;
    }
    if (scanstack_len <= i) {
	/* try to reuse the previous array */
	p1 = prev;
	cf = &scanstack_head;
	for (i = 0;;) {
	    if (p1->r != cf->color.r || p1->g != cf->color.g || p1->b != cf->color.b)
		break;
	    if (++i >= scanstack_len) {	/* end loop; reuse memory */
		page_colors.stack[scanned_page].colorstack = prev;
		return;	/* done */
	    }
	    ++p1;
	    cf = cf->next;
	}
    }
    page_colors.stack[scanned_page].colorstack = rgbp = xmalloc(scanstack_len * sizeof *rgbp);
    cf = &scanstack_head;
    for (i = 0; i < scanstack_len; ++i) {
	*rgbp++ = cf->color;
	cf = cf->next;
    }
}


/*
 *	We don't actually do any X calls yet to change colors; that can wait
 *	until a character or rule needs to be drawn.
 */

void
set_fg_color(const struct rgb *color)
{
    struct fgrec	**fgpp;

    if (fg_current != NULL
	&& color->r == fg_current->color.r
	&& color->g == fg_current->color.g
	&& color->b == fg_current->color.b) {
	return;
    }

    ASSERT(bg_current != NULL, "Background color not initialized");
    for (fgpp = &bg_current->fg_head;;) {
	fg_current = *fgpp;
	if (fg_current == NULL) {	/* if color is not in list */
	    fg_current = *fgpp = xmalloc(sizeof *fg_current);
	    fg_current->next = NULL;
	    fg_current->color = *color;
	    fg_current->pixel_good = False;
#if GREY
	    fg_current->palette_good = False;
#endif
	    break;
	}
	if (fg_current->color.r == color->r
	    && fg_current->color.g == color->g
	    && fg_current->color.b == color->b)
	    break;
	fgpp = &fg_current->next;
    }
    if (globals.debug & DBG_DVI)
	printf("Changing fg color to %5d %5d %5d\n",
	       color->r, color->g, color->b);
}

#if 0
static void
show_stack(void) {
    struct colorframe *ptr;
    int i;
    fprintf(stderr, "FG: %d,%d,%d\n", fg_current->color.r, fg_current->color.b, fg_current->color.g);
    for (i = 0, ptr = rcs_top; ptr != NULL; ptr = ptr->prev, i++) {
	fprintf(stderr, "stack %d: %d,%d,%d\n", i, ptr->color.r, ptr->color.b, ptr->color.g);
    }
}
#endif

void
color_special(const char *cp)
{
    while (*cp == ' ') ++cp;

    if (memicmp(cp, "push ", 5) == 0) {
	if (rcs_top == NULL) {
	    if (rcs_head == NULL) {
		rcs_head = xmalloc(sizeof *rcs_head);
		rcs_head->prev = rcs_head->next = NULL;
	    }
	    rcs_top = rcs_head;
	}
	else {
	    struct colorframe *rcs_next;

	    rcs_next = rcs_top->next;
	    if (rcs_next == NULL) {
		rcs_next = rcs_top->next = xmalloc(sizeof *rcs_next);
		rcs_next->prev = rcs_top;
		rcs_next->next = NULL;
	    }
	    rcs_top = rcs_next;
	}
	if (!parse_color(NULL, cp + 5, &rcs_top->color, False)) {
	    if (rcs_top->prev != NULL)
		rcs_top->color = rcs_top->prev->color;
	    else
		rcs_top->color = color_bottom[color_bot_size - 1];
	}
	set_fg_color(&rcs_top->color);
    }
    else if (memicmp(cp, "pop", 3) == 0) {
	/* Pop stack */
	if (rcs_top != NULL) {
	    if (color_bot_size > 0)
		rcs_top = rcs_top->prev;
	    else
		return;
	}
	else if (color_bot_size > 1)
	    --color_bot_size;
	else {
	    if (scanned_page_reset >= 0) {
		/* turn on scanning and redraw the page */
		scanned_page = scanned_page_color = scanned_page_reset = -1;
#if PS
		scanned_page_ps = scanned_page;
#endif
		/* 		fprintf(stderr, "forcing redraw!!!\n"); */
		globals.ev.flags |= EV_NEWPAGE;		/* force a redraw */
		longjmp(globals.ev.canit, 1);
	    }
	    return;
	}
	/* 	fprintf(stderr, "bot_size: %d\n", color_bot_size); */
	set_fg_color(rcs_top != NULL ? &rcs_top->color
		     : &color_bottom[color_bot_size - 1]);
    }
    else {
	struct rgb color;

	if (!parse_color(NULL, cp, &color, False))
	    return;

	/* clear stack */
	if (rcs_head == NULL) {
	    rcs_head = xmalloc(sizeof *rcs_head);
	    rcs_head->prev = rcs_head->next = NULL;
	}
	rcs_top = rcs_head;
	color_bot_size = 0;

	/* Change top of stack */
	rcs_top->color = color;

	set_fg_color(&color);
    }
}

#endif /* COLOR */

static unsigned int
myatopix(const char **pp)
{
#define SCR_LEN 16
    unsigned int value;
    const char *cp = *pp;
    char scr[16];
    const char *p0, *p1;

    p0 = cp;
    while ((*cp >= '0' && *cp <= '9') || *cp == '.')
	++cp;
    p1 = cp;
    while (isspace((int)*cp))
	++cp;
    if (*cp >= 'a' && *cp <= 'z' && cp[1] >= 'a' && cp[1] <= 'z') {
	/* if units are present */
	if (p1 - p0 <= SCR_LEN - 3) {
	    sprintf(scr, "%.*s%c%c", (int)(p1 - p0), p0, *cp, cp[1]);
	    value = atopix(scr);
	}
	else
	    value = 0;
	cp += 2;
    }
    else
	value = atopix(p0);

    *pp = cp;
    return value;
#undef SCR_LEN
}

static void
scan_papersize(const char *cp0)
{
    const char *cp = cp0;
    unsigned int w, h;
    double mag = 1.0;

    if (ignore_papersize_specials)
	return;

    if (*cp == '*') {
	do
	    ++cp;
	while (isspace((int)*cp));
	mag = magnification * .001;
    }
	    
    w = myatopix(&cp) * mag + 0.5;

    while (isspace((int)*cp))
	++cp;
    if (*cp == ',')
	do
	    ++cp;
	while (isspace((int)*cp));

    h = myatopix(&cp) * mag + 0.5;

    if (w == 0 || h == 0)
	XDVI_WARNING((stderr, "Invalid papersize special `%s'", cp0));
    else {
	/* we have a paper size special; disable xdvirc_geometry:
	   fprintf(stderr, "---------- scanned papersize: %dx%d\n", w, h);
	   resource.xdvirc_geometry = NULL;
	*/
	pageinfo_set_page_width(scanned_page + 1, w);
	pageinfo_set_window_width(scanned_page + 1, w);
	pageinfo_set_page_height(scanned_page + 1, h);
	pageinfo_set_window_height(scanned_page + 1, h);
    }
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

static char *
endofcommand(char *cp)
{
    while (isspace((int)*cp))
	++cp;
    if (*cp != '=')
	return NULL;
    do
	++cp;
    while (isspace((int)*cp));
    return cp;
}

#define	CMD(x, y)	((x) << 8 | (y))

void
applicationDoSpecial(char *cp, size_t len)
{
    char *p;

    if (globals.debug & DBG_DVI)
	printf("          `%s'\n", cp);

    while (isspace((int)*cp))
	++cp;

    /* Ignore initial "xdvi:" */
    if (memcmp(cp, "xdvi:", 5) == 0) {
	cp += 5;
	while (isspace((int)*cp))
	    ++cp;
    }
	
    /* PostScript specials */
#ifdef MAGICK
    if (resource.useMAGICK) {
	if (Magick_parse_special(cp))
	    return;
    }
#endif

    if (*cp == '"') {
	quote_special(cp);
	return;
    }
    if (memicmp(cp, "ps:", 3) == 0) {
	cp += 3;
	psfig_special(cp);
	/* check for hdvips hyperlinks */
	if (memicmp(cp, "sdict begin ", strlen("sdict begin ")) == 0) {
	    static Boolean warned_hypertex_too_old = False;
	    char *match = NULL;
	    if (warned_hypertex_too_old) /* don't continue evaluating links in this case */
		return;
	    cp += strlen("sdict begin ");
	    if (memcmp(cp, "H.S", 3) == 0
		|| memcmp(cp, "H.R", 3) == 0
		|| memcmp(cp, "H.B", 3) == 0
		/* following 2 conditions could be more restrictive: between `begin' and H.A/H.L,
		   there should be a single number (baselineskip in pt) */
		|| (match = strstr(cp, "H.A")) != NULL
		|| (match = strstr(cp, "H.L")) != NULL
		|| (match = strstr(cp, "/Action")) != NULL
		|| (match = strstr(cp, "/Link")) != NULL
		|| (match = strstr(cp, "/View")) != NULL) {
		if (match != NULL)
		    htex_do_special(match, len - 3 - (match - cp) - strlen("sdict begin "));
		else
		    htex_do_special(cp, len - strlen("sdict begin "));
	    }
	    else if (!warned_hypertex_too_old && strstr(cp, "HyperStart") != NULL) {
		popup_message(globals.widgets.top_level,
			      MSG_WARN, NULL,
			      "This DVI was created with a too old version of the `dvips' hyperref driver - "
			      "disabling hyperlinks.\n"
			      "To fix this, you should either upgrade to a current version of hyperref "
			      "(see http://www.tug.org/applications/hyperref/), "
			      "or use the `hypertex' package option, like this:\n\\usepackage[hypertex]{hyperref}\n"
			      "(Be aware though that this option won't work for PS->PDF conversion!)");
		warned_hypertex_too_old = True;
	    }
	}
	/* When not ignoring SDict entries, the distiller and pagecolor
	   code in lshort.dvi from CTAN:info/lshort/russian/lshrtdvi.zip
	   causes a failed assertion for 'color_bot_size > 0' in dvi-draw.c;
	   there's something wrong with the parsing order/event handling here
	   (see bug #856547).
	   But we also don't want those entries to trigger erasepage_gs()
	   [deleted in r3810], so it's correct to ignore them here.
	*/
	return;
    }
    if (memicmp(cp, "psfile", 6) == 0 && (p = endofcommand(cp + 6)) != NULL) {
    	epsf_special(p);
	return;
    }
    if (memicmp(cp, "html:", 5) == 0) {
	htex_do_special(cp + 5, len - 5);
	return;
    }

#if COLOR
    if (memicmp(cp, "color ", 6) == 0) {
	/* 	fprintf(stderr, "------------- color special\n"); */
	if (resource.use_color)
	    color_special(cp + 6);
	return;
    }
#endif
    
    /* these should have been scanned */
    if (*cp == '!'
	|| (memicmp(cp, "header", 6) == 0 && endofcommand(cp + 6) != NULL)) {
#ifdef PS
	if (resource.postscript != 0 && scanned_page_reset >= 0) {
	    /* turn on scanning and redraw the page */
	    scanned_page = scanned_page_ps = scanned_page_reset = -1;
# if COLOR
	    scanned_page_color = scanned_page;
# endif
	    globals.ev.flags |= EV_NEWPAGE;		/* force a redraw */
	    longjmp(globals.ev.canit, 1);
	}
#endif /* PS */
	return;
    }

    if (memicmp(cp, "background ", 11) == 0) {
#if COLOR
	if (resource.use_color && scanned_page_reset >= 0) {
	    /* turn on scanning and redraw the page */
	    scanned_page = scanned_page_color = scanned_page_reset = -1;
# if PS
	    scanned_page_ps = scanned_page;
# endif
	    /* 	    fprintf(stderr, "forcing redraw!\n"); */
	    globals.ev.flags |= EV_NEWPAGE;		/* force a redraw */
	    longjmp(globals.ev.canit, 1);
	}
#endif /* COLOR */
	return;
    }

    if (memcmp(cp, "papersize", 9) == 0 && endofcommand(cp + 9) != NULL) {
	m_have_papersize_special = True;
	if (scanned_page_reset >= 0) {
	    /* turn on scanning and redraw the page */
	    scanned_page = scanned_page_reset = -1;
#if PS
	    scanned_page_ps = scanned_page;
#endif
#if COLOR
	    scanned_page_color = scanned_page;
#endif
	    globals.ev.flags |= EV_NEWPAGE; /* force a redraw */
	    longjmp(globals.ev.canit, 1);
	}
	return;
    }

    /* tpic specials */

    if (*cp >= 'a' && *cp <= 'z' && cp[1] >= 'a' && cp[1] <= 'z' &&
	(isspace((int)cp[2]) || cp[2] == '\0')) {
	switch (CMD(*cp, cp[1])) {
	case CMD('p', 'n'):
	    set_pen_size(cp + 2);
	    return;
	case CMD('f', 'p'):
	    flush_path();
	    return;
	case CMD('d', 'a'):
	    flush_dashed(cp + 2, False);
	    return;
	case CMD('d', 't'):
	    flush_dashed(cp + 2, True);
	    return;
	case CMD('p', 'a'):
	    add_path(cp + 2);
	    return;
	case CMD('a', 'r'):
	    arc(cp + 2, False);
	    return;
	case CMD('i', 'a'):
	    arc(cp + 2, True);
	    return;
	case CMD('s', 'p'):
	    flush_spline();
	    return;
	case CMD('s', 'h'):
	    shade_last();
	    return;
	case CMD('w', 'h'):
	    whiten_last();
	    return;
	case CMD('b', 'k'):
	    blacken_last();
	    return;
	case CMD('i', 'p'):	/* throw away the path -- jansteen */
	    path_len = 0;
	    return;
	}
    }

    if (memcmp(cp, "src:", 4) == 0) {
	have_src_specials = True;
    }
    else if (globals.warn_spec_now)
	XDVI_WARNING((stderr, "Special \"%s\" not implemented.", cp));
}

#undef	CMD


Boolean
scan_special(char *cp, int cp_len, void *data)
{
    char *p;
    Boolean dummy_ret = True; /* currently unused;
				 FIXME: use return value to avoid redundant calls (instead of longjmp()) */
    UNUSED(cp_len); /* TODO: could probably utilize this in call to htex_prescan_special() */
    ASSERT(data != NULL, "Must pass a data pointer when using HTEX");
    
    if (globals.debug & DBG_PS)
	printf("Scanning special `%s'.\n", cp);

    while (isspace((int)*cp))
	++cp;
    
    /* Ignore initial "xdvi:" */
    if (memcmp(cp, "xdvi:", 5) == 0) {
	cp += 5;
	while (isspace((int)*cp))
	    ++cp;
    }

    if (memicmp(cp, "ps:", 3) == 0) {
	cp += 3;
	/* check for hdvips hyperlinks */
	if (memicmp(cp, "sdict begin ", strlen("sdict begin ")) == 0) {
	    static Boolean hypertex_too_old = False;
	    char *match = NULL;
	    cp += strlen("sdict begin ");

	    if (strstr(cp, "HyperStart") != NULL)
		hypertex_too_old = True;
	    if (hypertex_too_old)
		return False;
	    
	    if (memcmp(cp, "H.S", 3) == 0
		|| memcmp(cp, "H.R", 3) == 0
		|| memcmp(cp, "H.B", 3) == 0
		/* following 2 conditions could be more restrictive: between `begin' and H.A/H.L,
		   there should be a single number (baselineskip in pt) */
		|| (match = strstr(cp, "H.A")) != NULL
		|| (match = strstr(cp, "H.L")) != NULL
		|| (match = strstr(cp, "/Action")) != NULL
		|| (match = strstr(cp, "/Link")) != NULL
		|| (match = strstr(cp, "/View")) != NULL) {
		if (match != NULL)
		    htex_prescan_special(match, cp_len, data);
		else
		    htex_prescan_special(cp, cp_len, data);
	    }
	}
	    
    }
    else if (memicmp(cp, "html:", strlen("html:")) == 0) {
	size_t offset = strlen("html:");
#if 0
	Boolean found =
#endif
	htex_prescan_special(cp + offset, cp_len - offset, data);
#if 0
	/* if searching for a specific string, return as soon as it's found - not yet implemented */
 	if (my_data != NULL && my_data->scan_type == HTEX_ANCHOR_STRING && found) {
 	    return True;
 	}
#endif
    }
    /* do the following only if not searching for an anchor string */
    if (((struct htex_prescan_data *)data)->scan_type != HTEX_ANCHOR_STRING) {
#if PS
# if COLOR
	if (scanned_page_ps <= scanned_page)
# endif
	{
	    if (*cp == '!') {
		scan_bang(cp);
		return dummy_ret;
	    }
	    else if (memicmp(cp, "header", 6) == 0 && (p = endofcommand(cp + 6)) != NULL) {
		scan_header(p);
		return dummy_ret;
	    }
	}
#endif /* PS */
	
#if COLOR
# if PS
	if (scanned_page_color <= scanned_page)
# endif
	{
	    if (memicmp(cp, "background ", 11) == 0) {
		scan_bg_color(cp);
		return dummy_ret;
	    }
	    else if (memicmp(cp, "color ", 6) == 0) {
		scan_color(cp);
		return dummy_ret;
	    }
	}
#endif /* COLOR */
	if (memcmp(cp, "papersize", 9) == 0 && (p = endofcommand(cp + 9)) != NULL) {
	    m_have_papersize_special = True;
	    scan_papersize(p);
	    return dummy_ret;
	}
    }
    return dummy_ret;
}


/* #define	xspell_conv(n)	spell_conv0(n, current_dimconv) */
#define	xpixel_conv(x)	((int) ((x) >> 16))
#define	G_PXL_H		xpixel_conv(currinf.data.dvi_h)

/* cp is not const, because of endofcommand(). */
void
geom_do_special(struct scan_info *info, char *cp, double current_dimconv)
{
    const char *p;
    struct geom_info *g_info = (struct geom_info *)info->data;
    
    UNUSED(current_dimconv);
    
    while (isspace((int)*cp))
	++cp;

    /* Ignore initial "xdvi:" */
    if (memcmp(cp, "xdvi:", 5) == 0) {
	cp += 5;
	while (isspace((int)*cp))
	    ++cp;
    }

    if (memicmp(cp, "psfile", 6) == 0 && (p = endofcommand(cp + 6)) != NULL) {
	/* compute epsf bounding box */
	char c;
	int flags = 0;
	double keyval[6];

	c = *p;
	if (c == '\'' || c == '"') {
	    do
		++p;
	    while (*p != '\0' && *p != c);
	}
	else
	    while (*p != '\0' && !isspace((int)*p))
		++p;
	while (isspace((int)*p))
	    ++p;
	while (*p != '\0') {
	    const char *p1 = p;
	    size_t keyno;

	    while (*p1 != '=' && !isspace((int)*p1) && *p1 != '\0')
		++p1;
	    for (keyno = 0; keyno < NKEYS; ++keyno) {
		if (memcmp(p, keytab[keyno], p1 - p) == 0) {
		    if (keyno >= N_ARGLESS_KEYS) {
			while (isspace((int)*p1))
			    ++p1;
			if (*p1 == '=') {
			    ++p1;
			    while (isspace((int)*p1))
				++p1;
			}
			if (keyno < N_ARGLESS_KEYS + 6) {
			    keyval[keyno - N_ARGLESS_KEYS] = atof(p1);
			    flags |= (1 << (keyno - N_ARGLESS_KEYS));
			}
			while (!isspace((int)*p1) && *p1 != '\0')
			    ++p1;
		    }
		    break;
		}
	    }
	    p = p1;
	    while (!isspace((int)*p) && *p != '\0')
		++p;
	    while (isspace((int)*p))
		++p;
	}

	if ((flags & 0x30) == 0x30 || ((flags & 0x30) && (flags & 0xf) == 0xf)) {
	    long x = G_PXL_H;
	    long y = PXL_V;
	    long bbox_w;
	    long bbox_h;

	    bbox_w = 0.1 * ((flags & 0x10) ? KEY_RWI
			    : KEY_RHI * (KEY_URX - KEY_LLX) / (KEY_URY -
							       KEY_LLY)) *
		dimconv + 0.5;
	    bbox_h =
		0.1 * ((flags & 0x20) ? KEY_RHI : KEY_RWI *
		       (KEY_URY - KEY_LLY) / (KEY_URX - KEY_LLX))
		* dimconv + 0.5;

	    g_info->geom_box(info, x, y - bbox_h, x + bbox_w, y);
	}
    }
    else if (memicmp(cp, "ps::[begin]", 11) == 0) {
	/* compute psfig bounding box */
	long bbox_w, bbox_h;

	if (sscanf(cp + 11, "%ld %ld\n", &bbox_w, &bbox_h) >= 2) {
	    long x = G_PXL_H;
	    long y = PXL_V;

	    bbox_w = xpixel_conv(spell_conv(bbox_w));
	    bbox_h = xpixel_conv(spell_conv(bbox_h));

	    g_info->geom_box(info, x, y, x + bbox_w, y + bbox_h);
	}
    }
}
