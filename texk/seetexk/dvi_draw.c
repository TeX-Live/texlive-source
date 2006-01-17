/*
 * Support drawing routines for TeXsun and TeX
 *
 * Copyright, (C) 1987, 1988 Tim Morgan, UC Irvine
 * Copyright 1989 Dirk Grunwald (extensions & fixes)
 * 
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Tim Morgan, Dirk
 * Grunwald or M.I.T. not be used in advertising or publicity
 * pertaining to distribution of the software without specific,
 * written prior permission.  Tim Morgan, Dirk Grunwald and M.I.T.
 * makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 * 
 * TIM MORGAN,
 * DIRK GRUNWALD AND M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL M.I.T.  BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * Author:
 * 	Tim Morgan
 *	Dept. of Computer Science
 *	Univ. of Calf. at Irvine
 *
 * 	Dirk Grunwald
 * 	Dept. of Computer Science
 * 	
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

static char *rcsid="$Header: /usr/local/src/Uiuctex/libtex/RCS/dvi_draw.c,v 1.3 88/02/19 17:00:34 grunwald Exp Locker: grunwald $" ;

#include	<math.h>
#include	<stdio.h>
#include	<ctype.h>

#define	MAXPOINTS	300	/* Max points in a path */
#define	TWOPI		(3.14159265359*2.0)
#define	MAX_PEN_SIZE	7	/* Max pixels of pen width */


static int xx[MAXPOINTS], yy[MAXPOINTS];	/* Path in milli-inches */
static int path_len = 0;	/* # points in current path */
int pen_size = 1;		/* Pixel width of lines drawn */
#define	FALSE	0
#define	TRUE	1
int whiten = FALSE, shade = FALSE, blacken = FALSE;

extern void dot_at(), line_btw(), do_attribute_path();

#define	shrink_factor	shrinkFactor[currentShrink]

/* Unfortunately, these values also appear in dvisun.c */
#define	xRESOLUTION	(dviDPI/shrink_factor)
#define	yRESOLUTION	(dviDPI/shrink_factor)

extern int dviDPI, currentShrink, shrinkFactor[];


/*
 * Issue warning/error messages
 */
void Fatal(msg)
char *msg;
{
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

static void Warning(fmt, msg)
char *fmt, *msg;
{
    fprintf(stderr, fmt, msg);
    fputc ('\n', stderr);
}


/*
 * Set the size of the virtual pen used to draw in milli-inches
 */
/* ARGSUSED */
void set_pen_size(cp)
char *cp;
{
    int ps;

    if (sscanf(cp, " %d ", &ps) != 1) {
	Warning("illegal .ps command format: %s", cp);
	return;
    }
    pen_size = (ps*(xRESOLUTION+yRESOLUTION) + 1000) / 2000;
    if (pen_size < 1) pen_size = 1;
    else if (pen_size > MAX_PEN_SIZE) pen_size = MAX_PEN_SIZE;
}


/*
 * Print the line defined by previous path commands
 */
void flush_path()
{
    register int i;
    int last_min_x, last_max_x, last_min_y, last_max_y;

    last_min_x = 30000;	last_min_y = 30000;
    last_max_x = -30000; last_max_y = -30000;
    for (i=1; i<path_len; i++) {
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
}


/*
 * Print a dashed line along the previously defined path, with
 * the dashes/inch defined.
 */
void flush_dashed(cp, dotted)
char *cp;
int dotted;
{
    int i, numdots, x0, y0, x1, y1;
    int cx0, cy0, cx1, cy1;
    float inchesperdash;
    double d, spacesize, a, b, dx, dy, milliperdash;

    if (sscanf(cp, " %f ", &inchesperdash) != 1) {
	Warning("illegal format for dotted/dashed line: %s", cp);
	return;
    }
    if (path_len <= 1 || inchesperdash <= 0.0) {
	Warning("illegal conditions for dotted/dashed line%s", "");
	return;
    }
    milliperdash = inchesperdash * 1000.0;
    x0 = xx[1];	y0 = yy[1];
    x1 = xx[2];	y1 = yy[2];
    dx = x1 - x0;
    dy = y1 - y0;
    if (dotted) {
	numdots = sqrt(dx*dx + dy*dy) / milliperdash + 0.5;
	if ( numdots == 0 ) {	/* always draw at least one dot */
	  numdots = 1;
	}
	for (i=0; i <= numdots; i++) {
	    a = (float) i / (float) numdots;
	    cx0 = x0 + a*dx + 0.5;
	    cy0 = y0 + a*dy + 0.5;
	    dot_at(cx0, cy0);
	}
    }
    else {
	d = sqrt(dx*dx + dy*dy);
	if (d <= 2.0*milliperdash)
	    line_btw(x0, y0, x1, y1);
	else {
	    numdots = d / (2.0*milliperdash) + 1.0;
	    spacesize = (d - numdots * milliperdash) / (numdots - 1);
	    for (i=0; i<numdots-1; i++) {
		a = i * (milliperdash + spacesize) / d;
		b = a + milliperdash / d;
		cx0 = x0 + a*dx + 0.5;
		cy0 = y0 + a*dy + 0.5;
		cx1 = x0 + b*dx + 0.5;
		cy1 = y0 + b*dy + 0.5;
		line_btw(cx0, cy0, cx1, cy1);
		b += spacesize / d;
	    }
	    cx0 = x0 + b*dx + 0.5;
	    cy0 = y0 + b*dy + 0.5;
	    line_btw(cx0, cy0, x1, y1);
	}
    }
    path_len = 0;
}


/*
 * Add a point to the current path
 */
void add_path(cp)
char *cp;
{
    int pathx, pathy;

    if (++path_len >= MAXPOINTS) Fatal("Too many points");
    if (sscanf(cp, " %d %d ", &pathx, &pathy) != 2)
	Fatal("Malformed path command");
    xx[path_len] = pathx;
    yy[path_len] = pathy;
}


/*
 * Draw to a floating point position
 */
static void im_fdraw(x, y)
double x,y;
{
    if (++path_len >= MAXPOINTS) Fatal("Too many arc points");
    xx[path_len] = x + 0.5;
    yy[path_len] = y + 0.5;
}


/*
 * Draw an arc
 */
void arc(cp)
char *cp;
{
    int xc, yc, xrad, yrad, n;
    float start_angle, end_angle, angle, theta, r;
    double xradius, yradius, xcenter, ycenter;

    if (sscanf(cp, " %d %d %d %d %f %f ", &xc, &yc, &xrad, &yrad, &start_angle,
	&end_angle) != 6) {
	Warning("illegal arc specification: %s", cp);
	return;
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
    im_fdraw( xcenter + xradius*cos(start_angle),
		ycenter + yradius*sin(start_angle) );
    angle = start_angle + theta;
    while (angle < end_angle) {
	im_fdraw(xcenter + xradius*cos(angle),
		 ycenter + yradius*sin(angle) );
	angle += theta;
    }
    im_fdraw(xcenter + xradius*cos(end_angle),
		ycenter + yradius*sin(end_angle) );
    flush_path();
}


/*
 * APPROXIMATE integer distance between two points
 */
#define	dist(x0, y0, x1, y1)	(abs(x0-x1)+abs(y0-y1))


/*
 * Draw a spline along the previously defined path
 */
void flush_spline()
{
    int xp, yp, N, lastx=(-1), lasty;
    int t1, t2, t3, steps, j;
    register int i, w;

#ifdef	lint
    lasty = -1;
#endif
    N = path_len + 1;
    xx[0] = xx[1];	yy[0] = yy[1];
    xx[N] = xx[N-1];	yy[N] = yy[N-1];
    for (i=0; i<N-1; i++) {	/* interval */
	steps = (dist(xx[i], yy[i], xx[i+1], yy[i+1]) +
		 dist(xx[i+1], yy[i+1], xx[i+2], yy[i+2])) / 80;
	for (j=0; j<steps; j++) {	/* points within */
	    w = (j*1000 + 500) / steps;
	    t1 = w * w / 20;
	    w -= 500;
	    t2 = (750000 - w * w) / 10;
	    w -= 500;
	    t3 = w * w / 20;
	    xp = (t1*xx[i+2] + t2*xx[i+1] + t3*xx[i] + 50000) / 100000;
	    yp = (t1*yy[i+2] + t2*yy[i+1] + t3*yy[i] + 50000) / 100000;
	    if (lastx > -1) line_btw(lastx, lasty, xp, yp);
	    lastx = xp;
	    lasty = yp;
	}
    }
    path_len = 0;
}


/*
 * Shade the last box, circle, or ellipse
 */
void shade_last()
{
    blacken = whiten = FALSE;
    shade = TRUE;
}


/*
 * Make the last box, circle, or ellipse, white inside (shade with white)
 */
void whiten_last()
{
    whiten = TRUE;
    blacken = shade = FALSE;
}


/*
 * Make last box, etc, black inside
 */
void blacken_last()
{
    blacken = TRUE;
    whiten = shade = FALSE;
}


/*
 * Draw an ellipse with the indicated center and radices.
 */
draw_ellipse(xc, yc, xr, yr)
int xc, yc, xr, yr;
{
    double angle, theta;
    int n, px0, py0, px1, py1;

    angle = (xr + yr) / 2.0;
    theta = sqrt(1.0 / angle);
    n = TWOPI / theta + 0.5;
    if (n < 12) n = 12;
    else if (n > 80) n = 80;
    n /= 2;
    theta = TWOPI / n;

    angle = 0.0;
    px0 = xc + xr;	/* cos(0) = 1 */
    py0 = yc;		/* Sin(0) = 0 */
    while ((angle += theta) <= TWOPI) {
	px1 = xc + xr*cos(angle) + 0.5;
	py1 = yc + yr*sin(angle) + 0.5;
	line_btw(px0, py0, px1, py1);
	px0 = px1;
	py0 = py1;
    }
    line_btw(px0, py0, xc + xr, yc);
}
