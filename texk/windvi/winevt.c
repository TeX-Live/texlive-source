/*
 * Copyright (c) 1994 Paul Vojta.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * NOTE:
 *      xdvi is based on prior work as noted in the modification history, below.
 */

/*
 * DVI previewer for X.
 *
 * Eric Cooper, CMU, September 1985.
 *
 * Code derived from dvi-imagen.c.
 *
 * Modification history:
 * 1/1986       Modified for X.10       --Bob Scheifler, MIT LCS.
 * 7/1988       Modified for X.11       --Mark Eichin, MIT
 * 12/1988      Added 'R' option, toolkit, magnifying glass
 *                                      --Paul Vojta, UC Berkeley.
 * 2/1989       Added tpic support      --Jeffrey Lee, U of Toronto
 * 4/1989       Modified for System V   --Donald Richardson, Clarkson Univ.
 * 3/1990       Added VMS support       --Scott Allendorf, U of Iowa
 * 7/1990       Added reflection mode   --Michael Pak, Hebrew U of Jerusalem
 * 1/1992       Added greyscale code    --Till Brychcy, Techn. Univ. Muenchen
 *                                        and Lee Hetherington, MIT
 * 7/1992       Added extra menu buttons--Nelson H. F. Beebe <beebe@math.utah.edu>
 * 4/1994       Added DPS support, bounding box
 *                                      --Ricardo Telichevesky
 *                                        and Luis Miguel Silveira, MIT RLE.
 * 2/1995       Added rulers support    --Nelson H. F. Beebe <beebe@math.utah.edu>
 *
 *      Compilation options:
 *      SYSV    compile for System V
 *      VMS     compile for VMS
 *      NOTOOL  compile without toolkit
 *      BUTTONS compile with buttons on the side of the window (needs toolkit)
 *      MSBITFIRST      store bitmaps internally with most significant bit first
 *      BMSHORT store bitmaps in shorts instead of bytes
 *      BMLONG  store bitmaps in longs instead of bytes
 *      ALTFONT default for -altfont option
 *      A4      use European size paper
 *      TEXXET  support reflection dvi codes (right-to-left typesetting)
 *      GREY    use grey levels to shrink fonts
 *      PS_DPS  use display postscript to render pictures/bounding boxes
 *      PS_NEWS use the NeWS server to render pictures/bounding boxes
 *      PS_GS   use Ghostscript to render pictures/bounding boxes
 *      GS_PATH path to call the Ghostscript interpreter by
 */

#if 0
static  char    copyright[] =
"@(#) Copyright (c) 1994 Paul Vojta.  All rights reserved.\n";
#endif

#define EXTERN
#define INIT(x) =x

#include <windows.h>
#include <commdlg.h>
#include <direct.h>
#include "wingui.h"
#include "xdvi-config.h"

#include "dvi.h"

#define get_xy()
static Position window_x, window_y;
#define clip_w mane.width
#define clip_h mane.height

struct  mg_size_rec     mg_size[5]      = {{200, 150}, {400, 250}, {700, 500},
                                           {1000, 800}, {1200, 1200}};

struct WindowRec mane   = {(Window) 0, 1, 0, 0, 0, 0, MAXDIM, 0, MAXDIM, 0};
struct WindowRec alt    = {(Window) 0, 1, 0, 0, 0, 0, MAXDIM, 0, MAXDIM, 0};
/*      currwin is temporary storage except for within redraw() */
struct WindowRec currwin = {(Window) 0, 3, 0, 0, 0, 0, MAXDIM, 0, MAXDIM, 0};


/*
 *	Mechanism to keep track of the magnifier window.  The problems are,
 *	(a) if the button is released while the window is being drawn, this
 *	could cause an X error if we continue drawing in it after it is
 *	destroyed, and
 *	(b) creating and destroying the window too quickly confuses the window
 *	manager, which is avoided by waiting for an expose event before
 *	destroying it.
 */
static	short	alt_stat;	/* 1 = wait for expose, */
/* -1 = destroy upon expose */
static	Boolean	alt_canit;	/* stop drawing this window */

/*
 *	Data for buffered events.
 */

static	VOLATILE short	event_freq	= 70;

static	void	can_exposures(), keystroke();


/* We split this help string into smaller pieces, because some C
implementations have been known to croak on large strings. */

static char* help_string[] =
{
    "",
    "----------------------------Help and Exiting----------------------------",
    "",
    "h or H or ? or Help",
    "     Displays this help text.",
    "",
    "q or Control-C or Control-D or Cancel or Stop or Control-Z (VAX VMS)",
    "     Quits the program.",
    "",
    "------------------------------Page Motion-------------------------------",
    "",
    "n or f or Space or Return or LineFeed or PgDn",
    "     Moves to the next page (or to the nth next page if a",
    "     number is given).",
    "",
    "p or b or Control-H or BackSpace or DELete or PgUp",
    "     Moves to the previous page (or back n pages).",
    "",
    "u or Up-arrow",
    "     Moves up two thirds of a window-full.",
    "",
    "d or Down-arrow",
    "     Moves down two thirds of a window-full.",
    "",
    "l or Left-arrow",
    "     Moves left two thirds of a window-full.",
    "",
    "r or Right-arrow",
    "     Moves right two thirds of a window-full.",
    "",
    "g or j or End",
    "     Moves to the page with the given number.  Initially,",
    "     the first page is assumed to be page number 1, but this",
    "     can be changed with the `P' keystroke, below.  If no",
    "     page number is given, then it goes to the last page.",
    "",
    "<    Move to first page in document.",
    ">    Move to last page in document.",
    "",
    "-----------------------------Other Commands-----------------------------",
    "",
    "Control-A or Again",
    "     Repeat last command (including its argument).",
    "",
    "Control-L or Clear",
    "     Redisplays the current page.",
    "",
    "Control-P",
    "     Prints bitmap unit, bit order, and byte order.",
    "",
    "^ or Home",
    "     Move to the ``home'' position of the page.  This is",
    "     normally the upper left-hand corner of the page,",
    "     depending on the margins as described in the -margins",
    "     option, above.",
    "",
    "c    Moves the page so that the point currently beneath the",
    "     cursor is moved to the middle of the window.  It also",
    "     (gasp!) warps the cursor to the same place.",
    "",
    "G    This key toggles the use of greyscale anti-aliasing for",
    "     displaying shrunken bitmaps.  In addition, the key",
    "     sequences `0G' and `1G' clear and set this flag,",
    "     respectively.  See also the -nogrey option.",
    "",
    "k    Normally when xdvi switches pages, it moves to the home",
    "     position as well.  The `k' keystroke toggles a `keep-",
    "     position' flag which, when set, will keep the same",
    "     position when moving between pages.  Also `0k' and `1k'",
    "     clear and set this flag, respectively.  See also the",
    "     -keep option.",
    "",
    "M    Sets the margins so that the point currently under the",
    "     cursor is the upper left-hand corner of the text in the",
    "     page.  Note that this command itself does not move the",
    "     image at all.  For details on how the margins are used,",
    "     see the -margins option.",
    "",
    "P    ``This is page number n.''  This can be used to make",
    "     the `g' keystroke refer to actual page numbers instead",
    "     of absolute page numbers.",
    "",
    "R    Forces the dvi file to be reread.  This allows you to",
    "     preview many versions of the same file while running",
    "     xdvi only once.",
    "",
    "s    Changes the shrink factor to the given number.  If no",
    "     number is given, the smallest factor that makes the",
    "     entire page fit in the window will be used.  (Margins",
    "     are ignored in this computation.)",
    "",
    "S    Sets the density factor to be used when shrinking",
    "     bitmaps.  This should be a number between 0 and 100;",
    "     higher numbers produce lighter characters.",
    "",
    "t    Toggles to the next unit in a sorted list of TeX dimension",
    "     units for the popup magnifier ruler.",
    "",
    "x    Toggles expert mode (in which the buttons do not appear).",
    "     Also `0x' and `1x' clear and enable this mode,",
    "     respectively.  See also the -expert option.",
    "",
    "V    Toggles Ghostscript anti-aliasing.  Also `0V' and `1V' clear",
    "     and enables this mode, respectively.  See also the the -gsalpha",
    "     option.",
    "",
#ifdef SELFILE
    "Control-F     Find another DVI file.",
    "",
#endif
#ifdef HTEX
    "B    Go back to the previous anchor.",
    "F    Follow hyperlink forward.",
    "",
#endif
    "------------------------------------------------------------------------",
};

static char *TmpDir=NULL;

void
show_help()
{
    int k;

    for (k = 0; k < (sizeof(help_string)/sizeof(help_string[0])); ++k)
	(void)puts(help_string[k]);
}

/*
 *	Ruler routines
 */

static int
tick_scale(k)
     int		k;
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


void
draw_rulers(width, height, ourGC)
     unsigned int	width, height;
     GC		ourGC;
{
#ifdef WIN32
  int		k;			/* tick counter */
  double	old_pixels_per_tick;
  double	pixels_per_tick;
  int		scale;
  int		tick_offset;		/* offset along axes */
  int		x;			/* coordinates of top-left popup */
  int		y;			/* window corner */
  double	xx;			/* coordinates of tick */
  double	yy;			/* coordinates of tick */
  static char *last_tick_units = "";	/* memory of last tick units */
  
  if (tick_length <= 0)		/* user doesn't want tick marks */
    return;

  x = 0;		/* the pop-up window always has origin (0,0)  */
  y = 0;

  /* We need to clear the existing window to remove old rulers.  I think
     that this could be avoided if draw_rulers() could be invoked earlier.
     The expose argument in XClearArea() must be True to force redrawing
     of the text inside the popup window. Also, it would be better to draw
     the rulers before painting the text, so that rulers would not
     overwrite the text, but I haven't figured out yet how to arrange
     that. */

#if 1
  XClearArea(DISP, alt.win, x, y, width, height, True);
#endif
  /* The global resource._pixels_per_inch tells us how to find the ruler
     scale.  For example, 300dpi corresponds to these TeX units:

     1 TeX point (pt) 	=   4.151      pixels
     1 big point (bp) 	=   4.167      pixels
     1 pica (pc) 		=  49.813      pixels
     1 cicero (cc) 		=  53.501      pixels
     1 didot point (dd) 	=   4.442      pixels
     1 millimeter (mm) 	=  11.811      pixels
     1 centimeter (cm) 	= 118.110      pixels
     1 inch (in) 		= 300.000      pixels
     1 scaled point (sp) 	=   0.00006334 pixels

     The user can select the units via a resource (e.g. XDvi*tickUnits: bp),
     or a command-line option (e.g. -xrm '*tickUnits: cm').  The length of
     the ticks can be controlled by a resource (e.g. XDvi*tickLength: 10), or
     a command-line option (e.g. -xrm '*tickLength: 10000').  If the tick
     length exceeds the popup window size, then a graph-paper grid is drawn
     over the whole window.  Zero, or negative, tick length completely
     suppresses rulers. */

  pixels_per_tick = (double)resource._pixels_per_inch;
  if (strcmp(tick_units,"pt") == 0)
    pixels_per_tick /= 72.27;
  else if (strcmp(tick_units,"bp") == 0)
    pixels_per_tick /= 72.0;
  else if (strcmp(tick_units,"in") == 0)
    /* NO-OP */;
  else if (strcmp(tick_units,"cm") == 0)
    pixels_per_tick /= 2.54;
  else if (strcmp(tick_units,"mm") == 0)
    pixels_per_tick /= 25.4;
  else if (strcmp(tick_units,"dd") == 0)
    pixels_per_tick *= (1238.0 / 1157.0) / 72.27;
  else if (strcmp(tick_units,"cc") == 0)
    pixels_per_tick *= 12.0 * (1238.0 / 1157.0) / 72.27;
  else if (strcmp(tick_units,"pc") == 0)
    pixels_per_tick *= 12.0 /72.27;
  else if (strcmp(tick_units,"sp") == 0)
    pixels_per_tick /= (65536.0 * 72.27);
  else
    {
      Printf("Unrecognized tickUnits [%s]: defaulting to TeX points [pt]\n",
	     tick_units);
      tick_units = "pt";
      pixels_per_tick /= 72.27;
    }

  /* To permit accurate measurement in the popup window, we can reasonably
     place tick marks about 3 to 10 pixels apart, so we scale the computed
     pixels_per_tick by a power of ten to bring it into that range. */

  old_pixels_per_tick = pixels_per_tick; /* remember the original scale */
  while (pixels_per_tick < 3.0)
    pixels_per_tick *= 10.0;
  while (pixels_per_tick > 30.0)
    pixels_per_tick /= 10.0;
  if (strcmp(last_tick_units, tick_units) != 0)
    { /* tell user what the ruler scale is, but only when it changes */
      if (old_pixels_per_tick != pixels_per_tick)
	Printf("Ruler tick interval adjusted to represent %.2f%s\n",
	       pixels_per_tick/old_pixels_per_tick, tick_units);
      else if (debug & DBG_EVENT)
	Printf("Ruler tick interval represents 1%s\n", tick_units);
    }

  /* In order to make the ruler as accurate as possible, given the coarse
     screen resolution, we compute tick positions in floating-point
     arithmetic, then round to nearest integer values. */

  for (k = 0, xx = 0.0; xx < (double)width; k++, xx += pixels_per_tick)
    {				/* draw vertical ticks on top and bottom */
      tick_offset = (int)(0.5 + xx);	/* round to nearest pixel */
      scale = tick_scale(k);
      XDrawLine(DISP, alt.win, ourGC,
		x + tick_offset, y,
		x + tick_offset, y + scale*tick_length);
      XDrawLine(DISP, alt.win, ourGC,
		x + tick_offset, y + height,
		x + tick_offset, y + height - scale*tick_length);
    }

  for (k = 0, yy = 0.0; yy < (double)height; k++, yy += pixels_per_tick)
    {				/* draw horizontal ticks on left and right */
      tick_offset = (int)(0.5 + yy);	/* round to nearest pixel */
      scale = tick_scale(k);
      XDrawLine(DISP, alt.win, ourGC,
		x,                     y + tick_offset,
		x + scale*tick_length, y + tick_offset);
      XDrawLine(DISP, alt.win, ourGC,
		x + width,                     y + tick_offset,
		x + width - scale*tick_length, y + tick_offset);
    }

  last_tick_units = tick_units;

#if 0
  XFlush(DISP);			/* bring window up-to-date */
#endif
#endif
}

  /* The global resource._pixels_per_inch tells us how to find the ruler
     scale.  For example, 300dpi corresponds to these TeX units:

     1 TeX point (pt) 	=   4.151      pixels
     1 big point (bp) 	=   4.167      pixels
     1 pica (pc) 		=  49.813      pixels
     1 cicero (cc) 		=  53.501      pixels
     1 didot point (dd) 	=   4.442      pixels
     1 millimeter (mm) 	=  11.811      pixels
     1 centimeter (cm) 	= 118.110      pixels
     1 inch (in) 		= 300.000      pixels
     1 scaled point (sp) 	=   0.00006334 pixels

     The user can select the units via a resource (e.g. XDvi*tickUnits: bp),
     or a command-line option (e.g. -xrm '*tickUnits: cm').  The length of
     the ticks can be controlled by a resource (e.g. XDvi*tickLength: 10), or
     a command-line option (e.g. -xrm '*tickLength: 10000').  If the tick
     length exceeds the popup window size, then a graph-paper grid is drawn
     over the whole window.  Zero, or negative, tick length completely
     suppresses rulers. */
const_string pos_format;
double p2u_factor;

void pixel_to_unit() 
{
  double pixels_per_tick = (double)resource._pixels_per_inch;

  if (strcmp(tick_units,"pt") == 0) {
    p2u_factor = 72.27 / pixels_per_tick;
    pos_format = "%10.3fpt x %10.3fpt";
  }
  else if (strcmp(tick_units,"bp") == 0) {
    p2u_factor = 72.0 / pixels_per_tick;
    pos_format = "%10.3fbp x %10.3fbp";
  }
  else if (strcmp(tick_units,"in") == 0) {
    p2u_factor = 1.0 / pixels_per_tick;
    pos_format = "%10.3fin x %10.3fin";
  }
  else if (strcmp(tick_units,"cm") == 0) {
    p2u_factor = 2.54 / pixels_per_tick;
    pos_format = "%10.3lfcm x %10.3lfcm";
  }
  else if (strcmp(tick_units,"mm") == 0) {
    p2u_factor = 25.4 / pixels_per_tick;
    pos_format = "%10.3lfmm x %10.3lfmm";
  }
  else if (strcmp(tick_units,"dd") == 0) {
    p2u_factor = 1.0 / pixels_per_tick / ((1238.0 / 1157.0) / 72.27);
    pos_format = "%10.3lfdd x %10.3lfdd";
  }
  else if (strcmp(tick_units,"cc") == 0) {
    p2u_factor = 1.0 / pixels_per_tick / (12.0 * (1238.0 / 1157.0) / 72.27);
    pos_format = "%10.3lfcc x %10.3lfcc";
  }
  else if (strcmp(tick_units,"pc") == 0) {
    p2u_factor = 1.0 / pixels_per_tick / (12.0 /72.27);
    pos_format = "%10.3lfpc x %10.3lfpc";
  }
  else if (strcmp(tick_units,"sp") == 0) {
    p2u_factor = 1.0 / pixels_per_tick * (65536.0 * 72.27);
    pos_format = "%10.3lfsp x %10.3lfsp";
  }
  else
    {
      Printf("Unrecognized tickUnits [%s]: defaulting to TeX points [pt]\n",
	     tick_units);
      tick_units = "pt";
    p2u_factor = 72.27 / pixels_per_tick;
    pos_format = "%10.3fpt x %10.3fpt";
    }
}



#ifndef WIN32
/*
 *	Event-handling routines
 */

static	void
expose(windowrec, x, y, w, h)
     register struct WindowRec *windowrec;
     int		x, y;
     unsigned int	w, h;
{
  if (windowrec->min_x > x) windowrec->min_x = x;
  if (windowrec->max_x < x + w)
    windowrec->max_x = x + w;
  if (windowrec->min_y > y) windowrec->min_y = y;
  if (windowrec->max_y < y + h)
    windowrec->max_y = y + h;
}

static	void
clearexpose(windowrec, x, y, w, h)
     struct WindowRec *windowrec;
     int		x, y;
     unsigned int	w, h;
{
  XClearArea(DISP, windowrec->win, x, y, w, h, False);
  expose(windowrec, x, y, w, h);
}

static	void
scrollwindow(windowrec, x0, y0)
     register struct WindowRec *windowrec;
     int	x0, y0;
{
  int	x, y;
  int	x2 = 0, y2 = 0;
  int	ww, hh;

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
  if (windowrec->min_x < 0) windowrec->min_x = 0;
  windowrec->max_x -= x;
  if (windowrec->max_x > windowrec->width)
    windowrec->max_x = windowrec->width;
  windowrec->min_y -= y;
  if (windowrec->min_y < 0) windowrec->min_y = 0;
  windowrec->max_y -= y;
  if (windowrec->max_y > windowrec->height)
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
    XCopyArea(DISP, windowrec->win, windowrec->win,
	      DefaultGCOfScreen(SCRN), x, y,
	      (unsigned int) ww, (unsigned int) hh, x2, y2);
    if (x > 0)
      clearexpose(windowrec, ww, 0,
		  (unsigned int) x, windowrec->height);
    if (x2 > 0)
      clearexpose(windowrec, 0, 0,
		  (unsigned int) x2, windowrec->height);
    if (y > 0)
      clearexpose(windowrec, 0, hh,
		  windowrec->width, (unsigned int) y);
    if (y2 > 0)
      clearexpose(windowrec, 0, 0,
		  windowrec->width, (unsigned int) y2);
  }
}
#endif



_Xconst char    *paper_types[] = {
  "us",           "8.5in x 11in",
  "usr",          "11in x 8.5in",
  "legal",        "8.5in x 14in",
  "foolscap",     "13.5in x 17.0in",    /* ??? */

  /* ISO `A' formats, Portrait */
  "a1",           "59.4cm x 84.0cm",
  "a2",           "42.0cm x 59.4cm",
  "a3",           "29.7cm x 42.0cm",
  "a4",           "21.0cm x 29.7cm",
  "a5",           "14.85cm x 21.0cm",
  "a6",           "10.5cm x 14.85cm",
  "a7",           "7.42cm x 10.5cm",

  /* ISO `A' formats, Landscape */
  "a1r",          "84.0cm x 59.4cm",
  "a2r",          "59.4cm x 42.0cm",
  "a3r",          "42.0cm x 29.7cm",
  "a4r",          "29.7cm x 21.0cm",
  "a5r",          "21.0cm x 14.85cm",
  "a6r",          "14.85cm x 10.5cm",
  "a7r",          "10.5cm x 7.42cm",

  /* ISO `B' formats, Portrait */
  "b1",           "70.6cm x 100.0cm",
  "b2",           "50.0cm x 70.6cm",
  "b3",           "35.3cm x 50.0cm",
  "b4",           "25.0cm x 35.3cm",
  "b5",           "17.6cm x 25.0cm",
  "b6",           "13.5cm x 17.6cm",
  "b7",           "8.8cm x 13.5cm",

  /* ISO `B' formats, Landscape */
  "b1r",          "100.0cm x 70.6cm",
  "b2r",          "70.6cm x 50.0cm",
  "b3r",          "50.0cm x 35.3cm",
  "b4r",          "35.3cm x 25.0cm",
  "b5r",          "25.0cm x 17.6cm",
  "b6r",          "17.6cm x 13.5cm",
  "b7r",          "13.5cm x 8.8cm",

  /* ISO `C' formats, Portrait */
  "c1",           "64.8cm x 91.6cm",
  "c2",           "45.8cm x 64.8cm",
  "c3",           "32.4cm x 45.8cm",
  "c4",           "22.9cm x 32.4cm",
  "c5",           "16.2cm x 22.9cm",
  "c6",           "11.46cm x 16.2cm",
  "c7",           "8.1cm x 11.46cm",

  /* ISO `C' formats, Landscape */
  "c1r",          "91.6cm x 64.8cm",
  "c2r",          "64.8cm x 45.8cm",
  "c3r",          "45.8cm x 32.4cm",
  "c4r",          "32.4cm x 22.9cm",
  "c5r",          "22.9cm x 16.2cm",
  "c6r",          "16.2cm x 11.46cm",
  "c7r",          "11.46cm x 8.1cm",
};

int paper_types_number = sizeof(paper_types)/sizeof(char *);

Boolean
set_paper_type() {
  _Xconst char    *arg, *arg1;
  char    temp[64];
  _Xconst char    **p;
  char    *q;

  /*  fprintf(stderr, "Setting paper type to %s\n", resource.paper); */
  if (!resource.paper) {
#ifdef A4
    resource.paper = strdup("a4");
#else
    resource.paper = strdup("us");
#endif
  }
  if (strlen(resource.paper) > sizeof(temp) - 1) return False;
  arg = resource.paper;
  q = temp;
  for (;;) {      /* convert to lower case */
    char c = *arg++;
    if (c >= 'A' && c <= 'Z') c ^= ('a' ^ 'A');
    *q++ = c;
    if (c == '\0') break;
  }
  arg = temp;
  /* perform substitutions */
  for (p = paper_types; p < paper_types + XtNumber(paper_types); p += 2)
    if (strcmp(temp, *p) == 0) {
      arg = p[1];
      break;
    }

  arg1 = strchr(arg, 'x');
  if (arg1 == NULL) return False;
  unshrunk_paper_w = atopix(arg);
  unshrunk_paper_h = atopix(arg1 + 1);
#ifdef GRID
  unshrunk_paper_unit = atopixunit(arg);
#endif /* GRID */

  unshrunk_page_w = unshrunk_dvifile_page_w;
  unshrunk_page_h = unshrunk_dvifile_page_h;
  if (unshrunk_page_h < unshrunk_paper_h)
    unshrunk_page_h = unshrunk_paper_h;
  if (unshrunk_page_w < unshrunk_paper_w)
    unshrunk_page_w = unshrunk_paper_w;
  init_page();

#if 0
  fprintf(stderr, "paper (%d x %d)\n", unshrunk_paper_w, unshrunk_paper_h);
  fprintf(stderr, "dvi unshrunk page (%d x %d)\n", unshrunk_page_w, unshrunk_page_h);
  fprintf(stderr, "dvi shrunk page (%d x %d)\n", page_w, page_h);
#endif

  if (! isPrinting && hWndDraw)
    SendMessage(hWndDraw, WM_ERASEBKGND, (WPARAM)GetDC(currwin.win), 0);

  return (unshrunk_paper_w != 0 && unshrunk_paper_h != 0);
}

/* Set the icon name and title name standard properties on `top_level'
   (which we don't pass in because it is a different type for TOOLKIT
   !and TOOLKIT).  We use the basename of the DVI file (without the
   .dvi), so different xdvi invocations can be distinguished, yet 
   do not use up too much real estate.  */

void
set_icon_and_title (dvi_name, icon_ret, title_ret, set_std)
     char *dvi_name;
     char **icon_ret;
     char **title_ret;
     int set_std;
{
  /* Use basename of DVI file for name in icon and title.  */
  unsigned baselen;
  char *icon_name, *title_name;

  icon_name = strrchr(dvi_name, '/');
  if (icon_name != NULL) ++icon_name; else icon_name = dvi_name;
  baselen = strlen(icon_name);
  if (baselen >= sizeof(".dvi")
      && strcmp(icon_name + baselen - sizeof(".dvi") + 1, ".dvi")
      == 0) {         /* remove the .dvi */
    char *p;

    baselen -= sizeof(".dvi") - 1;
    p = xmalloc(baselen + 1);
    (void) strncpy(p, icon_name, (int) baselen);
    p[baselen] = '\0';
    icon_name = p;
  }
#ifdef Omega
        title_name = xmalloc(baselen + sizeof("(Omega) OWindvi:  "));
        Strcpy(title_name, "(Omega) OWindvi:  ");
#else
        title_name = xmalloc(baselen + sizeof("Windvi:  "));
        Strcpy(title_name, "Windvi:  ");
#endif
  Strcat(title_name, icon_name);

  if (icon_ret) *icon_ret = icon_name;
  if (title_ret) *title_ret = title_name;
  if (set_std) {
#ifdef WIN32
    SetWindowText(hWndMain, title_name);
#else
    Window top_window =
#ifdef TOOLKIT
      XtWindow (top_level);
#else
    top_level;
#endif

    XSetStandardProperties(DISP, top_window, title_name, icon_name,
			   (Pixmap) 0, NULL, 0, NULL);
#endif
  }
}

void
showmessage(message)
     _Xconst	char	*message;
{
  UpdateStatusBar(message, 0, 0);
  /*	XDrawImageString(DISP, mane.win, foreGC,
	5 - window_x, 5 + X11HEIGHT - window_y, message, strlen(message)) */
}


void
home(Boolean scrl)
{
  if (resource.keep_flag) {
	/* Send messages to go to (0,0) */
	SendMessage(mane.win, WM_HSCROLL, 
				MAKEWPARAM(SB_THUMBPOSITION, (home_x/mane.shrinkfactor)), 0);
	SendMessage(mane.win, WM_VSCROLL,
				MAKEWPARAM(SB_THUMBPOSITION, (home_y/mane.shrinkfactor)), 0);
  }
  else {
	SendMessage(mane.win, WM_HSCROLL, 
				MAKEWPARAM(SB_THUMBPOSITION, 0), 0);
	SendMessage(mane.win, WM_VSCROLL,
				MAKEWPARAM(SB_THUMBPOSITION, 0), 0);
  }
}	

void
#if	PS
ps_read_events(wait, allow_can)
     wide_bool	wait;
     wide_bool	allow_can;
#else
     read_events(wait)
     wide_bool	wait;
#define	allow_can	True
#endif
{

}

void
redraw(windowrec)
     struct WindowRec *windowrec;
{
  bDrawKeep = FALSE;

  /* restore color state at the beginning of the new page */
  init_colors();

  currwin = *windowrec;
  min_x = currwin.min_x + currwin.base_x;
  min_y = currwin.min_y + currwin.base_y;
  max_x = currwin.max_x + currwin.base_x;
  max_y = currwin.max_y + currwin.base_y;
#ifndef WIN32
  can_exposures(windowrec);
#endif
  if (debug & DBG_EVENT)
    Printf("Redraw %d x %d at (%d, %d) (base=%d,%d)\n", max_x - min_x,
	   max_y - min_y, min_x, min_y, currwin.base_x, currwin.base_y);
#ifdef WIN32
  /* FIXME : change cursor */
  if (!bMagDisp)
    SetCursor(hCursWait);
#else
  if (!busycurs) {
    XDefineCursor(DISP, mane.win, redraw_cursor);
    XFlush(DISP);
    busycurs = True;
  }
#endif
  if (setjmp(dvi_env)) {
    XClearWindow(DISP, mane.win);
    showmessage(dvi_oops_msg);
    if (dvi_file) {
      Fclose(dvi_file);
      dvi_file = NULL;
    }
  }
  else {
#ifdef BOOKMODE
    if (resource.book_mode) {
      int old_min_x = min_x,
	old_max_x = max_x;
      if (min_x <= page_w) {
	if (max_x > page_w) {
	  max_x = currwin.max_x = page_w;
	}
	draw_page();
	max_x = currwin.max_x = old_max_x;
      }
      if (max_x > page_w) {
	if (min_x <= page_w) {
	  min_x = currwin.min_x = 0;
	}
	max_x = currwin.max_x = max_x - page_w;
	current_page += 1;
	draw_page();
	max_x = currwin.max_x = old_max_x;
	min_x = currwin.min_x = old_min_x;
      }
      warn_spec_now = True;
    }
    else {
      draw_page();
      warn_spec_now = True;
    }
#else
      draw_page();
      warn_spec_now = True;
#endif
  }
  if (!bMagDisp)
    SetCursor(hCursArrow);
}

void
redraw_page()
{
#ifdef WIN32
  RECT maneRect;
  GdiFlush();
  GetClientRect(mane.win, &maneRect);

  if (mane.win == hWndDraw && GetClientRect(hWndDraw, &maneRect)) {
    int xorg = (int)(page_w - maneRect.right)/2;
    int yorg = (int)(page_h - maneRect.bottom)/2;
    xorg = min(0, xorg);
    yorg = min(0, yorg);
#if 0
    fprintf(stderr, "New org %ld %ld page %d %d rect %d %d\n", 
	    xorg, yorg, page_w, page_h, maneRect.right, maneRect.bottom);
#endif
    if (SetWindowOrgEx(maneDC, xorg, yorg, NULL) == 0) {
      Win32Error("MsgDrawPaint/SetWindowOrgEx(x,y)");
    }
  }
  else {
    Win32Error("MsgDrawPaint/GetClientRect(hwnd)");
  }

  if (debug & DBG_EVENT) Fputs("Redraw page:  ", stdout);

#if 0
  if (resource.in_memory) {
    mane.base_x = 0;
    mane.base_y = 0;
    mane.min_x = mane.min_y = 0;
    mane.max_x = page_w;
    mane.max_y = page_h; 
    allowDrawingChars = True;
    maneRect.top = maneRect.left = 0;
    maneRect.right = page_w+1;
    maneRect.bottom = page_h+1;
#if 0
    if (!FillRect(foreGC, &maneRect, backBrush))
      Win32Error("Redraw Page background");
#endif
    redraw(&mane);
  }
#endif
  
  if (resource.keep_flag) home(False);

  bDrawKeep = FALSE;
  InvalidateRect(mane.win, &maneRect, TRUE);

#else /* ! WIN32 */
  if (debug & DBG_EVENT) Fputs("Redraw page:  ", stdout);
  XClearWindow(DISP, mane.win);
  if (backing_store != NotUseful) {
    mane.min_x = mane.min_y = 0;
    mane.max_x = page_w;
    mane.max_y = page_h;
  }
  else {
    get_xy();
    mane.min_x = -window_x;
    mane.max_x = -window_x + clip_w;
    mane.min_y = -window_y;
    mane.max_y = -window_y + clip_h;
  }
  redraw(&mane);
#endif /* ! WIN32 */
}

Boolean
reconfig()
{
  RECT maneRect;
  int x_thick = 0, y_thick = 0;

  /* FIXME : this should change the size of the drawing surface */
  /*	XdviResizeWidget(draw_widget, page_w, page_h);
	get_geom(); */

  if (resource.in_memory) {
    /* FIXME:
       - if the bitmap can't be generated, return en error, and switch back
       to the previous shrink_factor
       - at s=1, no antialiasing should occur
    */
    if (maneDrawMemDC == NULL)
      if ((maneDrawMemDC = CreateCompatibleDC(maneDC)) == NULL)
	Win32Error("CreateCompatibleDC maneDrawMemDC");
    foreGC = ruleGC = highGC = maneDrawDC = maneDrawMemDC;

    grid1GC = grid2GC = grid3GC = maneDrawDC;
    foreGC2 = NULL;		/* not used under Win32 */

#ifdef TRANSFORM
    if (IS_WIN98 || IS_NT)
      SetGraphicsMode(maneDrawDC, GM_ADVANCED);
#endif

    if (oldmaneDIB) {
      /* There is an old maneDIB, put it back in the DC
	 and delete the current one */
      if ((maneDIB = SelectObject(foreGC, oldmaneDIB)) == NULL)
	Win32Error("reconfig/SelectObject");
      if (DeleteObject(maneDIB) == FALSE)
	Win32Error("DeleteObject/maneDIB");
    }
    
    if ((maneDIB = CreateDIB(maneDC, page_w, page_h, numColors, NULL, NULL)) == NULL)
      return False;
    if ((oldmaneDIB = SelectObject(foreGC, maneDIB)) == NULL)
      Win32Error("reconfig/SelectObject");
    
#if 0
    if (PatBlt(foreGC, 0, 0, page_w, page_h, PATCOPY) == 0)
      Win32Error("Reconfig background");
#else
    maneRect.top = maneRect.left = 0;
    maneRect.right = page_w;
    maneRect.bottom = page_h;
#if 0
	fprintf(stderr, "reconfig() is erasing background\n");
#endif
    if (!FillRect(foreGC, &maneRect, backBrush))
      Win32Error("Reconfig background");
#endif
  }
  else {
        foreGC = ruleGC = highGC = maneDrawDC = GetDC(hWndDraw);
    grid1GC = grid2GC = grid3GC = foreGC;
    magMemDC = foreGC2 = NULL;		/* not used under Win32 */
  }

#ifdef HTEX
  if (anchor_info) {
    /* XMoveResizeWindow(DPY anchor_info,
		      y_thick - 1, x_thick - BAR_THICK - 1,
		      clip_w/2, BAR_THICK - 1); */
    if (SetWindowPos(anchor_info, HWND_TOPMOST, 
		     y_thick - 1, x_thick - BAR_THICK - 1,
		     clip_w/2, BAR_THICK - 1,
		     SWP_NOACTIVATE | SWP_NOSIZE /* |  SWP_SHOWWINDOW */
		     | SWP_NOOWNERZORDER | SWP_NOZORDER) == 0)
      Win32Error("SetWindowPos");
    paint_anchor(NULL);
    /* XMoveResizeWindow(DPY anchor_search,
		      y_thick + clip_w/2 - 1, x_thick - BAR_THICK - 1,
		      clip_w/2, BAR_THICK - 1); */
    if (SetWindowPos(anchor_search, HWND_TOPMOST, 
		     y_thick + clip_w/2 - 1, x_thick - BAR_THICK - 1,
		     clip_w/2, BAR_THICK - 1,
		     SWP_NOACTIVATE | SWP_NOSIZE /* |  SWP_SHOWWINDOW */
		     | SWP_NOOWNERZORDER | SWP_NOZORDER) == 0)
      Win32Error("SetWindowPos");
  } else {
    /* anchor_info = XCreateSimpleWindow(DISP, top_level, y_thick - 1,
				      x_thick - BAR_THICK -1,
				      (unsigned int) clip_w/2, BAR_THICK - 1, 1,
				      brdr_Pixel, back_Pixel); */
    anchor_info = CreateWindow("EDIT",
			       NULL,
			       WS_BORDER | WS_POPUP | WS_DISABLED,
			       /* | WS_CLIPSIBLINGS, */
			       y_thick - 1, x_thick - BAR_THICK -1,
			       (unsigned int) clip_w/2, BAR_THICK - 1,
			       hWndDraw,
			       NULL,
			       hInst, 
			       NULL);
    if (anchor_info == NULL) {
      Win32Error("CreateWindow/Magnify");
      return FALSE;
    }
    /*		XSelectInput(DISP, anchor_info, ExposureMask);
    anchor_search = XCreateSimpleWindow(DISP, top_level,
					y_thick + clip_w/2 - 1,
					x_thick - BAR_THICK -1,
					(unsigned int) clip_w/2, BAR_THICK - 1, 1,
					brdr_Pixel, back_Pixel); */
    anchor_search = CreateWindow("EDIT",
			       NULL,
			       WS_BORDER | WS_POPUP | WS_DISABLED,
			       /* | WS_CLIPSIBLINGS, */
			       y_thick + clip_w/2 - 1, x_thick - BAR_THICK -1,
			       (unsigned int) clip_w/2, BAR_THICK - 1,
			       hWndDraw,
			       NULL,
			       hInst, 
			       NULL);
    if (anchor_search == NULL) {
      Win32Error("CreateWindow/Magnify");
      return FALSE;
    }
    ShowWindow(anchor_info, SW_SHOW);
    ShowWindow(anchor_search, SW_SHOW);
#if 0
    XSelectInput(DISP, anchor_search, ExposureMask|KeyPressMask);
    XMapWindow(DPY anchor_info);
    XMapWindow(DPY anchor_search);
#endif
  }
#endif
  /* Now, the scrollbars */
  SetScrollBars(mane.win);
  return True;
}

int
atopix(arg)
	_Xconst	char	*arg;
{
	int		len	= strlen(arg);
	_Xconst char	*arg_end = arg;
	char		tmp[11];
	double		factor;

	/* Skip whithe spaces */
	while (arg_end && *arg_end && isspace(*arg_end)) arg_end++;

	while ((*arg_end >= '0' && *arg_end <= '9') || *arg_end == '.')
	    if (arg_end >= arg + XtNumber(tmp) - 1) return 0;
	    else ++arg_end;
	bcopy(arg, tmp, arg_end - arg);
	tmp[arg_end - arg] = '\0';

#if A4
	factor = 1.0 / 2.54;	/* cm */
#else
	factor = 1.0;		/* inches */
#endif
	if (len > 2)
	    switch (arg[len - 2] << 8 | arg[len - 1]) {
		case 'i' << 8 | 'n':  factor = 1.0;			break;
		case 'c' << 8 | 'm':  factor = 1.0 / 2.54;		break;
		case 'm' << 8 | 'm':  factor = 1.0 / 25.4;		break;
		case 'p' << 8 | 't':  factor = 1.0 / 72.27;		break;
		case 'p' << 8 | 'c':  factor = 12.0 / 72.27;		break;
		case 'b' << 8 | 'p':  factor = 1.0 / 72.0;		break;
		case 'd' << 8 | 'd':  factor = 1238.0 / 1157.0 / 72.27;	break;
		case 'c' << 8 | 'c':  factor = 12 * 1238.0 / 1157.0 / 72.27;
									break;
		case 's' << 8 | 'p':  factor = 1.0 / 72.27 / 65536;	break;
	    }

	return factor * atof(tmp) * pixels_per_inch + 0.5;
}

string
pixtoa(int arg)
{
  char buf[64];
  sprintf(buf, "%.2fcm", (arg * 2.54) / pixels_per_inch);
  return strdup(buf);
}

#ifdef GRID
/* extract the unit used in paper size specification */
/* the information is used to decide the initial grid separation */
int
atopixunit(arg)
     _Xconst char    *arg;
{
  int     len     = strlen(arg);

  return (int)((len > 2 && arg[len - 2] == 'c' && arg[len - 1] == 'm' ?
	       1.0 / 2.54 : 1.0) * pixels_per_inch + 0.5);
}
#endif /* GRID */

void remove_temporary_dir(void)

{
  int err;

  if (TmpDir!=NULL) {
    err=rmdir(TmpDir);
    if (err!=0 && errno!=ENOENT) {
      char *buffer=malloc(160);
      if (buffer==NULL) oops("Please buy more RAM");
      sprintf(buffer,"xdvik: Warning: Could not remove temporary directory %s",
	      TmpDir);
      perror(buffer);
    }
  }
}

/* Create a temporary directory where we can write files from this program */
void
make_temporary_dir(tmpdir)
char **tmpdir;
{
  int ret, idval;
  char * tmp;

  *tmpdir = tempnam(NULL,"xdvi");

  if (*tmpdir == NULL)
    oops("Out of memory. Exiting.");

#ifdef HTEX
  if (debug & DBG_HYPER)
    fprintf(stderr,"Making temporary directory %s\n",*tmpdir);
#endif

#ifdef WIN32
  ret = mkdir(*tmpdir);
#else
  ret = mkdir(*tmpdir, 0700);
#endif

  /* The use of atexit and one variable to store the directory name
     makes this procedure a oneoff.  Enforce it */
  if (TmpDir!=NULL)
    oops("make_temporary_dir called twice!\n");

  TmpDir=strdup(*tmpdir);
#ifndef WIN32
  atexit(remove_temporary_dir);
#endif
  if (ret != 0)
    /* Didn't work, PANIC! */
    oops("Could not create temporary directory. Exiting.");

}

#ifdef HTEX
/* The anchor window stuff: */

void paint_anchor(text)
char *text;
{
#ifdef WIN32
  extern Widget anchor_info;
#if 0
  if (text != NULL)
    SetWindowText(anchor_info, text);
#endif
#else
	ClearArea(anchor_info, 1, 1, clip_w/2, BAR_THICK);
	if (text != NULL) XDrawString(DISP, anchor_info, foreGC,
		1 , BAR_WID, text, strlen(text));
#endif
}

#endif
