/*
 * Copyright (c) 1990-2004  Paul Vojta and others
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * PAUL VOJTA OR ANY OTHER AUTHOR OF THIS SOFTWARE BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * NOTE: xdvi is based on prior work, as noted in the modification history in
 * xdvi.c.
 *
 */

/*
 * Implementation of the magnifier window.
 */

#include "xdvi-config.h"
#include <math.h>

/* one of the following should define OPEN_MAX: */
#include <limits.h>
#include "c-openmx.h"

#include "xdvi.h"

#ifdef MOTIF
#include <Xm/Xm.h>
#endif

#include "events.h"
#include "dvi-draw.h"
#include "dvi-init.h"
#include "statusline.h"
#include "hypertex.h"
#include "mag.h"
#include "xm_toolbar.h"
#include "xm_menu.h" /* for get_last_ungrab() */
#include "util.h"
#include "pagesel.h"

#if HAVE_XKB_BELL_EXT
# include <X11/XKBlib.h>
# define XdviBell(display, window, percent)	\
	 XkbBell(display, window, percent, (Atom) None)
#else
# define XdviBell(display, window, percent)	XBell(display, percent)
#endif

/* to measure distance of pointer from ruler in ruler mode */
static int g_ruler_pos_x = 0, g_ruler_pos_y = 0;

struct WindowRec magnifier = { (Window) 0, 1, 0, 0, 0, 0, MAXDIM, 0, MAXDIM, 0 };

/*
 * Mechanism to keep track of the magnifier window.  The problems are,
 *
 * - if the button is released while the window is being drawn, this could
 *   cause an X error if we continue drawing in it after it is destroyed, and
 *
 * - creating and destroying the window too quickly confuses the window
 *   manager, which is avoided by waiting for an expose event before destroying
 *   it.
 */

short magnifier_stat;	/* 1 = wait for expose, -1 = destroy upon expose */

static Position main_x;
static Position main_y;

static Position mag_x = 0;
static Position mag_y = 0;

/* Under Motif the following two remain always constant */
static int mag_conv_x = 0;
static int mag_conv_y = 0;

static Position new_mag_x = 0;
static Position new_mag_y = 0;


/* default magnifier dimensions */
static struct mg_size_rec {
    int w;
    int h;
} mg_size[] = {
    {200, 150}, {400, 250}, {700, 500}, {1000, 800}, {1200, 1200}
};

size_t get_magglass_items(void) {
    return XtNumber(mg_size);
}

int get_magglass_width(int idx) {
    return mg_size[idx].w;
}

int get_magglass_height(int idx) {
    return mg_size[idx].h;
}

void set_magglass_widht(int idx, int w) {
    mg_size[idx].w = w;
}

void set_magglass_height(int idx, int h) {
    mg_size[idx].h = h;
}


static void
can_exposures(struct WindowRec *windowrec)
{
    windowrec->min_x = windowrec->min_y = MAXDIM;
    windowrec->max_x = windowrec->max_y = 0;
}

static void
mag_motion(XEvent * event)
{
    MYTRACE((stderr, "mag_motion!\n"));
    new_mag_x = event->xmotion.x + mag_conv_x;
    main_x = event->xmotion.x_root - new_mag_x;
    new_mag_y = event->xmotion.y + mag_conv_y;
    main_y = event->xmotion.y_root - new_mag_y;

    if (new_mag_x != mag_x || new_mag_y != mag_y)
	globals.ev.flags |= EV_MAG_MOVE;
    else
	globals.ev.flags &= ~EV_MAG_MOVE;
}

void
mag_release(XEvent * event)
{
    UNUSED(event);
    if (magnifier.win != (Window) 0) {
	if (magnifier_stat) {
	    magnifier_stat = -1;	/* destroy upon expose */
	}
	else {
	    XDestroyWindow(DISP, magnifier.win);
	    if (drawing_mag) {
		globals.ev.flags |= EV_MAG_GONE;
	    }
	    magnifier.win = (Window) 0;
	    mouse_motion = mouse_release = null_mouse;
	    globals.ev.flags &= ~EV_MAG_MOVE;
	    globals.cursor.flags &= ~CURSOR_MAG;
	    globals.ev.flags |= EV_CURSOR;
	    can_exposures(&magnifier);
	    /*
	      Workaround for bug #703304:
	      For obscure reasons, XFree 3.3.5 (and apparently also Solaris 8)
	      doesn't generate an expose event after the magnifier has beed
	      destroyed. But only a redraw() event caused by expose would reset
	      currwin.win back to mane.win, which is needed e.g. for getting the
	      hyperlink info updated (otherwise, the mouse will not become active
	      over a hyperlink).

	      Forcing a redraw with
	      redraw(&mane);
	      may cause a `BadDrawable' X error with color material (e.g. from #702288),
	      or a `draw_part: shouldn't happen: POST encountered' error. Neither do
	      the following work:
	      globals.ev.flags |= EV_EXPOSE;		(doesn't fix the bug)
	      draw_page();			(same effect as redraw(&mane))
	      globals.ev.flags |= EV_NEWPAGE;	(works, but is crude and causes flicker)
		 
	      So I decided to use expose() for the time being, which 
	      sets mane.min_x to the current x point (which doesn't happen
	      with EV_EXPOSE; this causes the test for `mane.min_x < MAXDIM'
	      to fail in events.c; look for `see comment in mag.c').
	    */
	    /*  	    fprintf(stderr, "========triggering expose!\n"); */
	    expose(&mane, event->xbutton.x_root, event->xbutton.y_root, 10, 10);
	}
    }
}

static int
tick_scale(int k)
{
    if (k == 0)
	return 3;
    else if ((k % 1000) == 0)
	return 7;
    else if ((k % 500) == 0)
	return 6;
    else if ((k % 100) == 0)
	return 5;
    else if ((k % 50) == 0)
	return 4;
    else if ((k % 10) == 0)
	return 3;
    else if ((k % 5) == 0)
	return 2;
    else
	return 1;
}

static void
draw_ticks(unsigned int width, unsigned int height, GC ourGC)
{
    int k;				/* tick counter */
    double old_pixels_per_tick;
    double pixels_per_tick;
    int scale;
    int tick_offset;			/* offset along axes */
    int x;				/* coordinates of top-left popup */
    int y;				/* window corner */
    double xx;				/* coordinates of tick */
    double yy;				/* coordinates of tick */
    static char *last_tick_units = "";	/* memory of last tick units */

    if (resource.tick_length <= 0)	/* user doesn't want tick marks */
	return;

    x = 0;	/* the pop-up window always has origin (0,0)  */
    y = 0;

    /* We need to clear the existing window to remove old rulers.  I think
       that this could be avoided if draw_ticks() could be invoked earlier.
       The expose argument in XClearArea() must be True to force redrawing
       of the text inside the popup window. Also, it would be better to draw
       the rulers before painting the text, so that rulers would not
       overwrite the text, but I haven't figured out yet how to arrange
       that. */

    XClearArea(DISP, magnifier.win, x, y, width, height, True);

    /* The global resource.pixels_per_inch tells us how to find the ruler
       scale.  For example, 300dpi corresponds to these TeX units:

       1 TeX point (pt)     =   4.151      pixels
       1 big point (bp)     =   4.167      pixels
       1 pica (pc)          =  49.813      pixels
       1 cicero (cc)                =  53.501      pixels
       1 didot point (dd)   =   4.442      pixels
       1 millimeter (mm)    =  11.811      pixels
       1 centimeter (cm)    = 118.110      pixels
       1 inch (in)          = 300.000      pixels
       1 scaled point (sp)  =   0.00006334 pixels

       The user can select the units via a resource (e.g. XDvi*tickUnits: bp),
       or a command-line option (e.g. -xrm '*tickUnits: cm').  The length of
       the ticks can be controlled by a resource (e.g. XDvi*tickLength: 10), or
       a command-line option (e.g. -xrm '*tickLength: 10000').  Zero, or negative,
       tick length completely suppresses rulers. */

    pixels_per_tick = (double)resource.pixels_per_inch;
    if (strcmp(resource.tick_units, "pt") == 0)
	pixels_per_tick /= 72.27;
    else if (strcmp(resource.tick_units, "bp") == 0)
	pixels_per_tick /= 72.0;
    else if (strcmp(resource.tick_units, "in") == 0)
	/* NO-OP */ ;
    else if (strcmp(resource.tick_units, "cm") == 0)
	pixels_per_tick /= 2.54;
    else if (strcmp(resource.tick_units, "mm") == 0)
	pixels_per_tick /= 25.4;
    else if (strcmp(resource.tick_units, "dd") == 0)
	pixels_per_tick *= (1238.0 / 1157.0) / 72.27;
    else if (strcmp(resource.tick_units, "cc") == 0)
	pixels_per_tick *= 12.0 * (1238.0 / 1157.0) / 72.27;
    else if (strcmp(resource.tick_units, "pc") == 0)
	pixels_per_tick *= 12.0 / 72.27;
    else if (strcmp(resource.tick_units, "sp") == 0)
	pixels_per_tick /= (65536.0 * 72.27);
    else if (strcmp(resource.tick_units, "px") == 0)
	pixels_per_tick = 10;
    else {
	XDVI_WARNING((stderr, "Unrecognized tickUnits [%s]: defaulting to TeX points [pt]",
		      resource.tick_units));
	resource.tick_units = "pt";
	pixels_per_tick /= 72.27;
    }

    /* To permit accurate measurement in the popup window, we can reasonably
     * place tick marks about 3 to 10 pixels apart, so we scale the computed
     * pixels_per_tick by a power of ten to bring it into that range.
     */

    old_pixels_per_tick = pixels_per_tick;	/* remember the original scale */
    while (pixels_per_tick < 3.0)
	pixels_per_tick *= 10.0;
    while (pixels_per_tick > 30.0)
	pixels_per_tick /= 10.0;

    /* tell user what the ruler scale is, but only when it changes */
    if (strcmp(last_tick_units, resource.tick_units) != 0) {
	if (old_pixels_per_tick != pixels_per_tick)
	    printf("Ruler tick interval adjusted to represent %.2f%s\n",
		   pixels_per_tick / old_pixels_per_tick, resource.tick_units);
	else if (globals.debug & DBG_EVENT)
	    printf("Ruler tick interval represents 1%s\n", resource.tick_units);
    }

    /* In order to make the ruler as accurate as possible, given the coarse
     * screen resolution, we compute tick positions in floating-point
     * arithmetic, then round to nearest integer values.
     */

    /* draw vertical ticks on top and bottom */
    for (k = 0, xx = 0.0; xx < (double)width; k++, xx += pixels_per_tick) {
	tick_offset = (int)(0.5 + xx);	/* round to nearest pixel */
	scale = tick_scale(k);
	XDrawLine(DISP, magnifier.win, ourGC,
		  x + tick_offset, y, x + tick_offset, y + scale * resource.tick_length);
	XDrawLine(DISP, magnifier.win, ourGC,
		  x + tick_offset, y + height,
		  x + tick_offset, y + height - scale * resource.tick_length);
    }

    /* draw horizontal ticks on left and right */
    for (k = 0, yy = 0.0; yy < (double)height; k++, yy += pixels_per_tick) {
	tick_offset = (int)(0.5 + yy);	/* round to nearest pixel */
	scale = tick_scale(k);
	XDrawLine(DISP, magnifier.win, ourGC,
		  x, y + tick_offset, x + scale * resource.tick_length, y + tick_offset);
	XDrawLine(DISP, magnifier.win, ourGC,
		  x + width, y + tick_offset,
		  x + width - scale * resource.tick_length, y + tick_offset);
    }

    last_tick_units = resource.tick_units;

    XFlush(DISP);	/* bring window up-to-date */
}

static void
compute_mag_pos(int *xp, int *yp)
{
    int t;

    t = mag_x + main_x - magnifier.width / 2;
    if (t > WidthOfScreen(SCRN) - (int)magnifier.width - 2 * MAGBORD)
	t = WidthOfScreen(SCRN) - (int)magnifier.width - 2 * MAGBORD;
    if (t < 0)
	t = 0;
    *xp = t;
    t = mag_y + main_y - magnifier.height / 2;
    if (t > HeightOfScreen(SCRN) - (int)magnifier.height - 2 * MAGBORD)
	t = HeightOfScreen(SCRN) - (int)magnifier.height - 2 * MAGBORD;
    if (t < 0)
	t = 0;
    *yp = t;
}

static void
scroll_window(struct WindowRec *windowrec, int x0, int y0)
{
    int x, y;
    int x2 = 0, y2 = 0;
    int ww, hh;

    x = x0 - windowrec->base_x;
    y = y0 - windowrec->base_y;
    ww = windowrec->width - x;
    hh = windowrec->height - y;
    windowrec->base_x = x0;
    windowrec->base_y = y0;
    if (currwin.win == windowrec->win) {
	currwin.base_x = x0;
	currwin.base_y = y0;
    }
    windowrec->min_x -= x;
    if (windowrec->min_x < 0)
	windowrec->min_x = 0;
    windowrec->max_x -= x;
    if ((unsigned int)windowrec->max_x > windowrec->width)
	windowrec->max_x = windowrec->width;
    windowrec->min_y -= y;
    if (windowrec->min_y < 0)
	windowrec->min_y = 0;
    windowrec->max_y -= y;
    if ((unsigned int)windowrec->max_y > windowrec->height)
	windowrec->max_y = windowrec->height;
    if (x < 0) {
	x2 = -x;
	x = 0;
	ww = windowrec->width - x2;
    }
    if (y < 0) {
	y2 = -y;
	y = 0;
	hh = windowrec->height - y2;
    }
    if (ww <= 0 || hh <= 0) {
	XClearWindow(DISP, windowrec->win);
	windowrec->min_x = windowrec->min_y = 0;
	windowrec->max_x = windowrec->width;
	windowrec->max_y = windowrec->height;
    }
    else {
	XCopyArea(DISP, windowrec->win, windowrec->win, globals.gc.copy,
		  x, y, (unsigned int)ww, (unsigned int)hh, x2, y2);
	if (x > 0)
	    clearexpose(windowrec, ww, 0, (unsigned int)x, windowrec->height);
	if (x2 > 0)
	    clearexpose(windowrec, 0, 0, (unsigned int)x2, windowrec->height);
	if (y > 0)
	    clearexpose(windowrec, 0, hh, windowrec->width, (unsigned int)y);
	if (y2 > 0)
	    clearexpose(windowrec, 0, 0, windowrec->width, (unsigned int)y2);
    }
}

static void
do_movemag(int x, int y)
{
    int xx, yy;

    mag_x = x;
    mag_y = y;
    if (mag_x == new_mag_x && mag_y == new_mag_y)
	globals.ev.flags &= ~EV_MAG_MOVE;
    compute_mag_pos(&xx, &yy);
    XMoveWindow(DISP, magnifier.win, xx, yy);
    scroll_window(&magnifier,
		  (x + mane_base_x) * mane.shrinkfactor - (int)magnifier.width / 2,
		  (y + mane_base_y) * mane.shrinkfactor - (int)magnifier.height / 2);
    draw_ticks(magnifier.width, magnifier.height, globals.gc.ruler);
}

extern jmp_buf next_env;

void
show_distance_from_ruler(XEvent *event, Boolean to_stdout)
{
    int loc_x, loc_y;
    int precision = 2;
    double factor;

    if (event == NULL) /* when option is toggled */
	return;

    loc_x = event->xbutton.x;
    loc_y = event->xbutton.y;
    if (event->xbutton.window != mane.win) {
	Window ww;
	(void)XTranslateCoordinates(DISP,
				    RootWindowOfScreen(SCRN), mane.win,
				    event->xbutton.x_root,
				    event->xbutton.y_root,
				    &loc_x,
				    &loc_y,
				    &ww);	/* throw away last argument */
    }

    /* map everything below 0 to the origin */
    if (loc_x < 0)
	loc_x = 0;
    if (loc_y < 0)
	loc_y = 0;

    if (strcmp(resource.tick_units, "pt") == 0) {
	factor = 72.27 * currwin.shrinkfactor / (double)resource.pixels_per_inch;
    }
    else if (strcmp(resource.tick_units, "bp") == 0) {
	factor = 72.0 * currwin.shrinkfactor / (double)resource.pixels_per_inch;
    }
    else if (strcmp(resource.tick_units, "in") == 0) {
	factor = currwin.shrinkfactor / (double)resource.pixels_per_inch;
    }
    else if (strcmp(resource.tick_units, "cm") == 0) {
	factor = 2.54 * currwin.shrinkfactor / (double)resource.pixels_per_inch;
	precision = 3;
    }
    else if (strcmp(resource.tick_units, "mm") == 0) {
	factor = 25.4 * currwin.shrinkfactor / (double)resource.pixels_per_inch;
    }
    else if (strcmp(resource.tick_units, "dd") == 0) {
	factor = 72.27 / (1238.0 / 1157.0) * currwin.shrinkfactor / (double)resource.pixels_per_inch;
    }
    else if (strcmp(resource.tick_units, "cc") == 0) {
	factor = 72.27 / (12.0 * (1238.0 / 1157.0)) * currwin.shrinkfactor / (double)resource.pixels_per_inch;
    }
    else if (strcmp(resource.tick_units, "pc") == 0) {
	factor = 72.27 / 12.0 * currwin.shrinkfactor / (double)resource.pixels_per_inch;
    }
    else if (strcmp(resource.tick_units, "sp") == 0) {
	factor = 65536.0 * 72.27 * currwin.shrinkfactor / (double)resource.pixels_per_inch;
	precision = 1;
    }
    else if (strcmp(resource.tick_units, "px") == 0) { /* pixel units */
	factor = 1;
    }
    else {
	XDVI_WARNING((stderr, "Unrecognized tickUnits [%s]: defaulting to TeX points [pt]",
		      resource.tick_units));
	resource.tick_units = "pt";
	factor = 72.27 * currwin.shrinkfactor / (double)resource.pixels_per_inch;
    }
    
    if (mouse_release != null_mouse) {
	if (to_stdout) {
	    XDVI_INFO((stdout, "Ruler/Point: %d,%d, dx: %.*f %s, dy: %.*f %s, dr: %.*f %s",
		       loc_x, loc_y,
		       precision, 0.000, resource.tick_units,
		       precision, 0.000, resource.tick_units,
		       precision, 0.000, resource.tick_units));
	}
	else {
	    statusline_info(STATUS_FOREVER,
			     "Ruler/Point: %d,%d, dx: %.*f %s, dy: %.*f %s, dt: %.*f %s",
			     loc_x, loc_y,
			     precision, 0.000, resource.tick_units,
			     precision, 0.000, resource.tick_units,
			     precision, 0.000, resource.tick_units);
	}
    }
    else {
	int d_x = loc_x - g_ruler_pos_x;
	int d_y = loc_y - g_ruler_pos_y;
	double d_z = sqrt((double)d_x * d_x + (double)d_y * d_y);
	double unit_x = (double)d_x * factor;
	double unit_y = (double)d_y * factor;
	double unit_z = d_z * factor;
	if (to_stdout) {
	    XDVI_INFO((stdout, "Ruler: %d,%d, Point: %d,%d, dx: %.*f %s, dy: %.*f %s, dr: %.*f %s",
		       g_ruler_pos_x, g_ruler_pos_y, loc_x, loc_y,
		       precision, unit_x, resource.tick_units,
		       precision, unit_y, resource.tick_units,
		       precision, unit_z, resource.tick_units));
	}
	else {
	    statusline_info(STATUS_FOREVER,
			     "Ruler: %d,%d, Point: %d,%d, dx: %.*f %s, dy: %.*f %s, dr: %.*f %s",
			     g_ruler_pos_x, g_ruler_pos_y, loc_x, loc_y,
			     precision, unit_x, resource.tick_units,
			     precision, unit_y, resource.tick_units,
			     precision, unit_z, resource.tick_units);
	}
    }
}

void
move_magnifier(void)
{
    if (magnifier.win == (Window) 0)
	globals.ev.flags &= ~EV_MAG_MOVE;
    else if (abs(new_mag_x - mag_x) > 2 * abs(new_mag_y - mag_y))
	do_movemag(new_mag_x, mag_y);
    else if (abs(new_mag_y - mag_y) > 2 * abs(new_mag_x - mag_x))
	do_movemag(mag_x, new_mag_y);
    else
	do_movemag(new_mag_x, new_mag_y);
}


void
clear_ruler(void)
{
    /* maybe we should do this only for mouse-1? */
    clearexpose(&mane, 0, g_ruler_pos_y,
		ROUNDUP(pageinfo_get_page_width(current_page), currwin.shrinkfactor) + 2, 1);
    clearexpose(&mane, g_ruler_pos_x, 0,
		1, ROUNDUP(pageinfo_get_page_height(current_page), currwin.shrinkfactor) + 2);
}

void
show_ruler(XEvent *event)
{
    if (mouse_release == null_mouse) {
	if (mouse_release != null_mouse && mouse_release != drag_ruler_release)
	    return;
	if (mouse_release == null_mouse) {
	    mouse_motion = drag_ruler_motion;
	    mouse_release = drag_ruler_release;
	}
	drag_ruler_motion(event);
    }
}

static void
draw_ruler(int x, int y)
{
    /* don't draw if outside page region (will be clipped automatically by Motif,
       but not by Xaw, where draw widget is entire window) */
    if (x > (int)ROUNDUP(pageinfo_get_page_width(current_page), currwin.shrinkfactor) + 1
	|| y > (int)ROUNDUP(pageinfo_get_page_height(current_page), currwin.shrinkfactor) + 1)
	return;
    
    XFillRectangle(DISP, mane.win, globals.gc.high,
		   0, y,
		   ROUNDUP(pageinfo_get_page_width(current_page), currwin.shrinkfactor) + 2, 1);
    XFillRectangle(DISP, mane.win, globals.gc.high,
		   x, 0,
		   1, ROUNDUP(pageinfo_get_page_height(current_page), currwin.shrinkfactor) + 2);
}


/* snap ruler back to origing (0,0) */
void
ruler_snap_origin(XEvent *event)
{
    clear_ruler();
    g_ruler_pos_x = g_ruler_pos_y = 0;
    draw_ruler(g_ruler_pos_x, g_ruler_pos_y);
    /* deactivate mouse dragging */
    mouse_motion = mouse_release = null_mouse;
    show_distance_from_ruler(event, False);
}

void
redraw_ruler(void)
{
    draw_ruler(g_ruler_pos_x, g_ruler_pos_y);
}

void magnifier_move(String params, XEvent *event)
{
    int x, y;
    XSetWindowAttributes attr;
#ifndef MOTIF
    Window throwaway;
#endif
    const char *p = params;
    
    if (*p == '*') {
	int n = atoi(p + 1) - 1;

	if (n < 0 || n >= (int)get_magglass_items() || get_magglass_width(n) <= 0) {
	    XdviBell(DISP, event->xany.window, 0);
	    return;
	}
	magnifier.width = get_magglass_width(n);
	magnifier.height = get_magglass_height(n);
    }
    else {
	magnifier.width = magnifier.height = atoi(p);
	p = strchr(p, 'x');
	if (p != NULL) {
	    magnifier.height = atoi(p + 1);
	    if (magnifier.height == 0)
		magnifier.width = 0;
	}
	if (magnifier.width == 0) {
	    XdviBell(DISP, event->xany.window, 0);
	    return;
	}
    }
#ifndef MOTIF
    XTranslateCoordinates(DISP, event->xbutton.window, mane.win,
			  0, 0, &mag_conv_x, &mag_conv_y, &throwaway);
#endif

    mag_x = event->xbutton.x + mag_conv_x;
    mag_y = event->xbutton.y + mag_conv_y;
    main_x = event->xbutton.x_root - mag_x;
    main_y = event->xbutton.y_root - mag_y;
    compute_mag_pos(&x, &y);
    magnifier.base_x = (mag_x + mane_base_x) * mane.shrinkfactor - magnifier.width / 2;
    magnifier.base_y = (mag_y + mane_base_y) * mane.shrinkfactor - magnifier.height / 2;
    attr.save_under = True;
    attr.border_pixel = resource.rule_pixel;
#if COLOR
    attr.background_pixel = bg_current->pixel;
#else
    attr.background_pixel = resource.back_Pixel;
#endif
    attr.override_redirect = True;
#ifdef GREY
    attr.colormap = G_colormap;
#endif
    magnifier.win = XCreateWindow(DISP, RootWindowOfScreen(SCRN),
				  x, y, magnifier.width, magnifier.height, MAGBORD,
				  G_depth, InputOutput, G_visual,
				  CWSaveUnder | CWBorderPixel | CWBackPixel |
#ifdef GREY
				  CWColormap |
#endif
				  CWOverrideRedirect, &attr);
    XSelectInput(DISP, magnifier.win, ExposureMask);
    XMapWindow(DISP, magnifier.win);

    /*
     * This call will draw the point rulers when the magnifier first pops up,
     * if the XDvi*delayRulers resource is false.  Some users may prefer rulers
     * to remain invisible until the magnifier is moved, so the default is
     * true.  Rulers can be suppressed entirely by setting the XDvi*tickLength
     * resource to zero or negative.
     */

    if (!resource.delay_rulers)
	draw_ticks(magnifier.width, magnifier.height, globals.gc.ruler);

    globals.cursor.flags |= CURSOR_MAG;
    globals.ev.flags |= EV_CURSOR;
    
    magnifier_stat = 1;	/* waiting for exposure */
    mouse_motion = mag_motion;
    mouse_release = mag_release;
}

void
drag_ruler_motion(XEvent *event)
{
    int loc_x, loc_y;
    if (event == NULL) { /* toggled via menu */
	/* hack to avoid redrawing ruler at last g_* positions when mode is
	   toggled on via menu, then off via keystroke */
	g_ruler_pos_x = g_ruler_pos_y = 0;
	return;
    }
    
    loc_x = event->xbutton.x;
    loc_y = event->xbutton.y;

    if (event->xbutton.window != mane.win) {
	Window dummy;
	(void)XTranslateCoordinates(DISP,
				    RootWindowOfScreen(SCRN), mane.win,
				    event->xbutton.x_root,
				    event->xbutton.y_root,
				    &loc_x,
				    &loc_y,
				    &dummy);
    }

    /* map everything below 0 to the origin */
    if (loc_x < 0)
	loc_x = 0;
    if (loc_y < 0)
	loc_y = 0;
    
    clear_ruler();
    draw_ruler(loc_x, loc_y);
    g_ruler_pos_x = loc_x;
    g_ruler_pos_y = loc_y;
}

void
drag_ruler_release(XEvent *event)
{
    UNUSED(event);
    mouse_motion = mouse_release = null_mouse;
}

/* XtActionsRec mag_actions[] = { */
/*     {"magnifier", Act_magnifier}, */
/*     {"do-href", Act_href}, */
/*     {"do-href-newwindow", Act_href_newwindow}, */
/*     {"switch-magnifier-units", Act_switch_magnifier_units}, */
/* }; */

/*
 * This isn't creating the actual magnifier. It is created lazily on demand
 * if one of the corresponding actions is taken.  Therefore we are here
 * just adding the record of actions related to magnifier handling to the
 * application.
 */

/* void */
/* create_magnifier(void) */
/* { */
/*     XtAppAddActions(globals.app, mag_actions, XtNumber(mag_actions)); */
/* } */
