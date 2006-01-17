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
	xdvi is based on prior work, as noted in the modification history
	in xdvi.c.

\*========================================================================*/

#include "xdvi-config.h"


/* Xlib and Xutil are already included */

#ifdef	TOOLKIT

#ifdef	OLD_X11_TOOLKIT
#include <X11/Atoms.h>
#else /* not OLD_X11_TOOLKIT */
#include <X11/Xatom.h>
#include <X11/StringDefs.h>
#endif /* not OLD_X11_TOOLKIT */

#include <X11/Shell.h>	/* needed for def. of XtNiconX */

#ifndef	XtSpecificationRelease
#define	XtSpecificationRelease	0
#endif
#if	XtSpecificationRelease >= 4
#ifndef MOTIF
#include <X11/Xaw/Viewport.h>
#ifdef HTEX
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/Text.h>
#endif
#else /* MOTIF */
#include <Xm/MainW.h>
#include <Xm/ToggleB.h>
#include <Xm/RowColumn.h>
#include <Xm/MenuShell.h>
#endif /* MOTIF */

#ifdef	BUTTONS
#ifndef MOTIF
#include <X11/Xaw/Command.h>
#define	PANEL_WIDGET_CLASS	compositeWidgetClass
#define	BUTTON_WIDGET_CLASS	commandWidgetClass
#else /* MOTIF */
#include <Xm/Form.h>
#include <Xm/BulletinB.h>
#include <Xm/PushB.h>
#define	PANEL_WIDGET_CLASS	xmBulletinBoardWidgetClass
#define	BUTTON_WIDGET_CLASS	xmPushButtonWidgetClass
#endif /* MOTIF */
#endif /* BUTTONS */

#else	/* XtSpecificationRelease < 4 */

#define	XtPointer caddr_t
#include <X11/Viewport.h>
#ifdef	BUTTONS
#include <X11/Command.h>
#define	PANEL_WIDGET_CLASS	compositeWidgetClass
#define	BUTTON_WIDGET_CLASS	commandWidgetClass
#endif

#endif	/* XtSpecificationRelease */

#else	/* not TOOLKIT */

typedef	int		Position;
#define	XtPending()	XPending(DISP)

#endif	/* not TOOLKIT */

#include <signal.h>
#include <sys/file.h>	/* this defines FASYNC */

#if !defined(FLAKY_SIGPOLL) && !HAVE_STREAMS && !defined(FASYNC)
#define	FLAKY_SIGPOLL	1
#endif

#ifndef FLAKY_SIGPOLL

#ifndef SIGPOLL
#define	SIGPOLL	SIGIO
#endif

#ifndef SA_RESTART
#define SA_RESTART 0
#endif

#if HAVE_STREAMS
#include <stropts.h>

#ifndef S_RDNORM
#define	S_RDNORM S_INPUT
#endif

#ifndef S_RDBAND
#define	S_RDBAND 0
#endif

#ifndef S_HANGUP
#define	S_HANGUP 0
#endif

#ifndef S_WRNORM
#define	S_WRNORM S_OUTPUT
#endif
#endif /* HAVE_STREAMS */

#endif /* not FLAKY_SIGPOLL */

#if HAVE_POLL
# include <poll.h>
#else
# if HAVE_SYS_SELECT_H
#  include <sys/select.h>
# else
#  if HAVE_SELECT_H
#   include <select.h>
#  endif
# endif
#endif

#include <errno.h>

#ifdef	X_NOT_STDC_ENV
extern	int	errno;
#endif	/* X_NOT_STDC_ENV */

#ifndef	X11HEIGHT
#define	X11HEIGHT	8	/* Height of server default font */
#endif

#define	MAGBORD	1	/* border size for magnifier */

/*
 * Command line flags.
 */

#define delay_rulers	resource._delay_rulers
#define tick_units	resource._tick_units
#define tick_length	resource._tick_length
#define	fore_Pixel	resource._fore_Pixel
#define	back_Pixel	resource._back_Pixel
#define	high_Pixel	resource._high_Pixel
#ifdef	TOOLKIT
extern	struct _resource	resource;
#define	brdr_Pixel	resource._brdr_Pixel
#endif	/* TOOLKIT */

#define	clip_w	mane.width
#define	clip_h	mane.height
static	Position main_x, main_y;
static	Position mag_x, mag_y, new_mag_x, new_mag_y;
static	Boolean	mag_moved	= False;
static	Boolean	busycurs	= False;

#ifndef FLAKY_SIGPOLL
sigset_t		sigpollusrterm;
#endif

#if HAVE_POLL
static	struct pollfd	fds[1]	= {{0, POLLIN, 0}};
#else
static	fd_set		readfds;
#endif

#ifdef	TOOLKIT
#ifdef	BUTTONS
static	Widget	line_widget;
static	Widget	panel_widget;
static	int	destroy_count	= 0;
#endif /* BUTTONS */

#ifndef MOTIF
static	Widget	x_bar, y_bar;	/* horizontal and vertical scroll bars */
#endif

static	Arg	resize_args[] = {
	{XtNwidth,	(XtArgVal) 0},
	{XtNheight,	(XtArgVal) 0},
};

#define	XdviResizeWidget(widget, w, h)	\
		(resize_args[0].value = (XtArgVal) (w), \
		resize_args[1].value = (XtArgVal) (h), \
		XtSetValues(widget, resize_args, XtNumber(resize_args)) )

#ifdef	BUTTONS

#ifndef MOTIF
static	Arg	resizable_on[] = {
	{XtNresizable,	(XtArgVal) True},
};

static	Arg	resizable_off[] = {
	{XtNresizable,	(XtArgVal) False},
};
#endif

static	Arg	line_args[] = {
	{XtNbackground,		(XtArgVal) 0},
	{XtNwidth,		(XtArgVal) 1},
#ifndef MOTIF
	{XtNheight,		(XtArgVal) 0},
	{XtNfromHoriz,		(XtArgVal) NULL},
	{XtNborderWidth,	(XtArgVal) 0},
	{XtNtop,		(XtArgVal) XtChainTop},
	{XtNbottom,		(XtArgVal) XtChainBottom},
	{XtNleft,		(XtArgVal) XtChainRight},
	{XtNright,		(XtArgVal) XtChainRight},
#else /* MOTIF */
	{XmNleftWidget,		(XtArgVal) NULL},
	{XmNleftAttachment,	(XtArgVal) XmATTACH_WIDGET},
	{XmNtopAttachment,	(XtArgVal) XmATTACH_FORM},
	{XmNbottomAttachment,	(XtArgVal) XmATTACH_FORM},
#endif /* MOTIF */
};

static	Arg	panel_args[] = {
	{XtNwidth,		(XtArgVal) (XTRA_WID - 1)},
#ifndef MOTIF
	{XtNheight,		(XtArgVal) 0},
	{XtNborderWidth,	(XtArgVal) 0},
	{XtNfromHoriz,		(XtArgVal) NULL},
	{XtNtop,		(XtArgVal) XtChainTop},
	{XtNbottom,		(XtArgVal) XtChainBottom},
	{XtNleft,		(XtArgVal) XtChainRight},
	{XtNright,		(XtArgVal) XtChainRight},
#else /* MOTIF */
	{XmNleftAttachment,	(XtArgVal) XmATTACH_FORM},
	{XmNtopAttachment,	(XtArgVal) XmATTACH_FORM},
	{XmNbottomAttachment,	(XtArgVal) XmATTACH_FORM},
#endif /* MOTIF */
};

static	struct {
	_Xconst	char	*label;
	_Xconst	char	*name;
	int	closure;
	int	y_pos;
	}
#define BPOS(n) (10 + (n)*28)
	command_table[] = {
		{"Quit",	"quit",		'q',		BPOS(1)},
		{"Abort",	"abort",	'Q',		BPOS(2)},
		{"Again",	"again",	'\001',		BPOS(4)},
		{"Help",	"help",		'?',		BPOS(4)},
		{"Reread",	"reread",	'R',		BPOS(5)},
		{"100%",	"sh1",		1 << 8 | 's',	BPOS(7)},
#define LOWRES_s_COMMANDS	6
		/* Good for 300dpi */
		{"50%",	"sh2",		2 << 8 | 's',	BPOS(8)},
		{"33%",	"sh3",		3 << 8 | 's',	BPOS(9)},
		{"25%",	"sh4",		4 << 8 | 's',	BPOS(10)},
#define HIRES_s_COMMANDS	9
		/* Good for 600dpi */
		{"33%",	"sh2",		3 << 8 | 's',	BPOS(8)},
		{"25%",	"sh3",		4 << 8 | 's',	BPOS(9)},
		{"17%",	"sh4",		6 << 8 | 's',	BPOS(10)},

		/* What's good for 1200dpi? */

#define PAGE_BUTTONS 12
		{"First",	"next0",	1 << 8 | 'g',   BPOS(12)},
		{"Page-10",	"prev10",	10 << 8 | 'p',	BPOS(13)},
		{"Page-5",	"prev5",	5 << 8 | 'p',	BPOS(14)},
		{"Prev",	"prev",		'p',		BPOS(15)},
		{"Next",	"next",		'n',		BPOS(16)},
		{"Page+5",	"next5",	5 << 8 | 'n',	BPOS(17)},
		{"Page+10",	"next10",	10 << 8 | 'n',	BPOS(18)},
		{"Last",	"next999",	'g',            BPOS(19)},
#if	PS
		{"View PS",	"postscript",	'v',		BPOS(21)},
#endif
#ifdef HTEX
		{"Back",	"back",		'B',		BPOS(23)},
#endif
#ifdef SELFILE
		{"File",	"file", 	'\006',	        BPOS(24)},
#endif  /* SELFILE */
};

static	void	handle_command();

static	XtCallbackRec	command_call[] = {
	{handle_command, NULL},
	{NULL,		NULL},
};

static	Arg	command_args[] = {
#ifndef MOTIF
	{XtNlabel,	(XtArgVal) NULL},
#else
	{XmNlabelString, (XtArgVal) NULL},
#endif
	{XtNx,		(XtArgVal) 6},
	{XtNy,		(XtArgVal) 0},
#ifndef MOTIF
	{XtNwidth,	(XtArgVal) 64},
#else
	{XtNwidth,	(XtArgVal) 90},
#endif
	{XtNheight,	(XtArgVal) 30},
#ifndef MOTIF
	{XtNcallback,	(XtArgVal) command_call},
#else
	{XmNactivateCallback, (XtArgVal) command_call},
#endif
};

void
create_buttons(h)
	XtArgVal	h;
{
  int i;

  line_args[0].value = (XtArgVal) resource._hl_Pixel;
#ifndef MOTIF
  line_args[2].value = h;
  line_args[3].value = (XtArgVal) vport_widget;
  line_widget = XtCreateManagedWidget("line", widgetClass,
				      form_widget, line_args, 
				      XtNumber(line_args));
  panel_args[1].value = h;
  panel_args[3].value = (XtArgVal) line_widget;
#endif
  panel_widget = XtCreateManagedWidget("panel", PANEL_WIDGET_CLASS,
				       form_widget, panel_args, 
				       XtNumber(panel_args));

  /* This adjusts the command table according to any shrinkbutton
     resources */
  for (i = 0; i <= XtNumber(command_table) - 4; ++i) {
    if (strncmp(command_table[i].name, "sh1",2) == 0) {
      /* It's a shrink command.  Check if the resources tell us to 
	 modify it, and how */
      int j,k;

      j = atoi(&command_table[i].name[2]);
      k = resource.shrinkbutton[j-1];
	
      if (k != 0) {
	char *s = xmalloc(9);
	  
	if (k < 1)
	  k = 1;
	else if (k > 99)
	  k = 99;

	if (k<16) {
	  Sprintf(s, "%d%%", (int) 100/k);
	} else {
	  /* Need more precition */
	  Sprintf(s, "%.2f%%", (double) 100/k);
	}
	command_table[i].label = s;
	command_table[i].closure = k << 8 | 's';
      } /* if k != 0 */
    } /* if strncmp */
  } /* For */

  command_args[2].value = (XtArgVal) vport_widget;

  /* This loop sets up the buttons */
  for (i = 0; i < XtNumber(command_table); ++i) {
    if ( ((i==HIRES_s_COMMANDS)&&(pixels_per_inch<600)) ||
	 ((i==LOWRES_s_COMMANDS)&&(pixels_per_inch>=600)) ) i+=3;
#ifndef MOTIF
    command_args[0].value = (XtArgVal) command_table[i].label;
#else /* MOTIF */
    command_args[0].value = (XtArgVal)
      XmCvtCTToXmString((char *) command_table[i].label);
#endif /* MOTIF */
    command_args[2].value = (XtArgVal) command_table[i].y_pos;
    command_call[0].closure = (XtPointer) &command_table[i].closure;
    (void) XtCreateManagedWidget(command_table[i].name,
				 BUTTON_WIDGET_CLASS, panel_widget,
				 command_args, XtNumber(command_args));
  }
#ifdef MOTIF
  line_args[2].value = (XtArgVal) panel_widget;
  line_widget = XtCreateManagedWidget("line", widgetClass, form_widget,
				      line_args, XtNumber(line_args));
  XtVaSetValues(vport_widget, XmNleftAttachment, XmATTACH_WIDGET,
		XmNleftWidget, line_widget, NULL);
#endif
}
#endif	/* BUTTONS */

#else	/* !TOOLKIT */
static	Window	x_bar, y_bar;
static	int	x_bgn, x_end, y_bgn, y_end;	/* scrollbar positions */
#endif	/* TOOLKIT */

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

static void
show_help()
{
    int k;

    for (k = 0; k < (sizeof(help_string)/sizeof(help_string[0])); ++k)
	(void)puts(help_string[k]);
}

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

#ifndef FLAKY_SIGPOLL
static	VOLATILE short	event_freq	= 70;
#else
#define	event_freq	70
#endif

static	void	can_exposures(), keystroke();

#ifdef	GREY
#define	gamma	resource._gamma

static	void
mask_shifts(mask, pshift1, pshift2)
	Pixel	mask;
	int	*pshift1, *pshift2;
{
	int	k, l;

	for (k = 0; (mask & 1) == 0; ++k)
	    mask >>= 1;
	for (l = 0; (mask & 1) == 1; ++l)
	    mask >>= 1;
	*pshift1 = sizeof(short) * 8 - l;
	*pshift2 = k;
}

/*
 *	Try to allocate 4 color planes for 16 colors (for GXor drawing)
 */

void
init_plane_masks()
{
	Pixel	pixel;

	if (copy || plane_masks[0] != 0)
	    return;

	if (XAllocColorCells(DISP, our_colormap, False, plane_masks, 4,
	  &pixel, 1)) {
	    /* Make sure fore_Pixel and back_Pixel are a part of the palette */
	    back_Pixel = pixel;
	    fore_Pixel = pixel | plane_masks[0] | plane_masks[1]
	      | plane_masks[2] | plane_masks[3];
	    if (mane.win != (Window) 0)
		XSetWindowBackground(DISP, mane.win, back_Pixel);
	}
	else {
	    copy = True;
	    puts("\
Greyscaling is running in copy mode:  your display can only display\n\
a limited number of colors at a time (typically 256), and other applications\n\
(such as netscape) are using many of them.  Running in copy mode will\n\
cause overstrike characters to appear incorrectly, and may result in\n\
poor display quality.\n\
\n\
See the section ``GREYSCALING AND COLORMAPS'' in the xdvi manual page\n\
for more details.");
	    fflush(stdout);	/* useful if called by netscape */
	}
}

extern	double	pow();

#ifndef TOOLKIT
#define	XtWindow(win)	win
#endif

#define	MakeGC(fcn, fg, bg)	(values.function = fcn, \
	  values.foreground=fg, values.background=bg, \
	  XCreateGC(DISP, XtWindow(top_level), \
	    GCFunction | GCForeground | GCBackground, &values))

void
init_pix()
{
	static	int	shrink_allocated_for = 0;
	static	float	oldgamma	= 0.0;
	static	Pixel	palette[17];
	XGCValues	values;
	int	i;

	if (fore_color_data.pixel == back_color_data.pixel) {
	    /* get foreground and background RGB values for interpolating */
	    fore_color_data.pixel = fore_Pixel;
	    XQueryColor(DISP, our_colormap, &fore_color_data);
	    back_color_data.pixel = back_Pixel;
	    XQueryColor(DISP, our_colormap, &back_color_data);
	}

	if (our_visual->class == TrueColor) {
	    /* This mirrors the non-grey code in xdvi.c */
	    static int		shift1_r, shift1_g, shift1_b;
	    static int		shift2_r, shift2_g, shift2_b;
	    static Pixel	set_bits;
	    static Pixel	clr_bits;
	    unsigned int	sf_squared;

	    if (oldgamma == 0.0) {
		mask_shifts(our_visual->red_mask,   &shift1_r, &shift2_r);
		mask_shifts(our_visual->green_mask, &shift1_g, &shift2_g);
		mask_shifts(our_visual->blue_mask,  &shift1_b, &shift2_b);

		set_bits = fore_color_data.pixel & ~back_color_data.pixel;
		clr_bits = back_color_data.pixel & ~fore_color_data.pixel;

		if (set_bits & our_visual->red_mask)
		    set_bits |= our_visual->red_mask;
		if (clr_bits & our_visual->red_mask)
		    clr_bits |= our_visual->red_mask;
		if (set_bits & our_visual->green_mask)
		    set_bits |= our_visual->green_mask;
		if (clr_bits & our_visual->green_mask)
		    clr_bits |= our_visual->green_mask;
		if (set_bits & our_visual->blue_mask)
		    set_bits |= our_visual->blue_mask;
		if (clr_bits & our_visual->blue_mask)
		    clr_bits |= our_visual->blue_mask;

		/*
		 * Make the GCs
		 */

		foreGC = foreGC2 = ruleGC = 0;
		copyGC = MakeGC(GXcopy, fore_Pixel, back_Pixel);
		if (copy || (set_bits && clr_bits)) {
		    ruleGC = copyGC;
		    if (!resource.thorough) copy = True;
		}
		if (copy) {
		    foreGC = ruleGC;
		    if (!resource.copy)
			Puts("Note:  overstrike characters may be incorrect.");
		}
		else {
		    if (set_bits)
			foreGC = MakeGC(GXor,
			  set_bits & fore_color_data.pixel, 0);
		    if (clr_bits || !set_bits)
			*(foreGC ? &foreGC2 : &foreGC) = MakeGC(GXandInverted,
			  clr_bits & ~fore_color_data.pixel, 0);
		    if (!ruleGC) ruleGC = foreGC;
		}

		oldgamma = gamma;
	    }

	    if (mane.shrinkfactor == 1) return;
	    sf_squared = mane.shrinkfactor * mane.shrinkfactor;

	    if (shrink_allocated_for < mane.shrinkfactor) {
		if (pixeltbl != NULL) {
		    free((char *) pixeltbl);
		    if (pixeltbl_t != NULL)
			free((char *) pixeltbl_t);
		}
		pixeltbl = xmalloc((sf_squared + 1) * sizeof(Pixel));
		if (foreGC2 != NULL)
		    pixeltbl_t = xmalloc((sf_squared + 1) * sizeof(Pixel));
		shrink_allocated_for = mane.shrinkfactor;
	    }

	    /*
	     * Compute pixel values directly.
	     */

#define	SHIFTIFY(x, shift1, shift2)	((((Pixel)(x)) >> (shift1)) << (shift2))

	    for (i = 0; i <= sf_squared; ++i) {
		double		frac	= gamma > 0
		    ? pow((double) i / sf_squared, 1 / gamma)
		    : 1 - pow((double) (sf_squared - i) / sf_squared, -gamma);
		unsigned int	red, green, blue;
		Pixel		pixel;

		red = frac
		  * ((double) fore_color_data.red - back_color_data.red)
		  + back_color_data.red;
		green = frac
		  * ((double) fore_color_data.green - back_color_data.green)
		  + back_color_data.green;
		blue = frac
		  * ((double) fore_color_data.blue - back_color_data.blue)
		  + back_color_data.blue;

		pixel = SHIFTIFY(red,   shift1_r, shift2_r) |
			SHIFTIFY(green, shift1_g, shift2_g) |
			SHIFTIFY(blue,  shift1_b, shift2_b);

		if (copy) pixeltbl[i] = pixel;
		else {
		    pixeltbl[i] = set_bits ? pixel & set_bits
			: ~pixel & clr_bits;
		    if (pixeltbl_t != NULL)
			pixeltbl_t[i] = ~pixel & clr_bits;
		}
	    }

#undef	SHIFTIFY

	    return;
	}

	/* if not TrueColor ... */

	if (gamma != oldgamma) {
	    XColor	color;

	    if (oldgamma == 0.0)
		init_plane_masks();

	    for (i = 0; i < 16; ++i) {
		double	frac = gamma > 0 ? pow((double) i / 15, 1 / gamma)
		    : 1 - pow((double) (15 - i) / 15, -gamma);

		color.red = frac
		  * ((double) fore_color_data.red - back_color_data.red)
		  + back_color_data.red;
		color.green = frac
		  * ((double) fore_color_data.green - back_color_data.green)
		  + back_color_data.green;
		color.blue = frac
		  * ((double) fore_color_data.blue - back_color_data.blue)
		  + back_color_data.blue;

		color.pixel = back_Pixel;
		color.flags = DoRed | DoGreen | DoBlue;

		if (!copy) {
		    if (i & 1) color.pixel |= plane_masks[0];
		    if (i & 2) color.pixel |= plane_masks[1];
		    if (i & 4) color.pixel |= plane_masks[2];
		    if (i & 8) color.pixel |= plane_masks[3];
		    XStoreColor(DISP, our_colormap, &color);
		    palette[i] = color.pixel;
		}
		else {
		    if (XAllocColor(DISP, our_colormap, &color))
			palette[i] = color.pixel;
		    else
			palette[i] = (i * 100 >= density * 15)
			    ? fore_Pixel : back_Pixel;
		}
	    }

	    copyGC = MakeGC(GXcopy, fore_Pixel, back_Pixel);
	    foreGC = ruleGC = copy ? copyGC
	      : MakeGC(GXor, fore_Pixel, back_Pixel);
	    foreGC2 = 0;

	    oldgamma = gamma;
	}

	if (mane.shrinkfactor == 1) return;

	if (shrink_allocated_for < mane.shrinkfactor) {
	    if (pixeltbl != NULL) free((char *) pixeltbl);
	    pixeltbl = xmalloc((unsigned)
		(mane.shrinkfactor * mane.shrinkfactor + 1) * sizeof(Pixel));
	    shrink_allocated_for = mane.shrinkfactor;
	}

	for (i = 0; i <= mane.shrinkfactor * mane.shrinkfactor; ++i)
	    pixeltbl[i] =
		palette[(i * 30 + mane.shrinkfactor * mane.shrinkfactor)
		    / (2 * mane.shrinkfactor * mane.shrinkfactor)];
}

#undef MakeGC

void
init_colors()
{
	static	Boolean	warn_done = False;
	Boolean	warn = False;

	copy = resource.copy;

#ifdef	GREY
	if (use_grey)
	    init_pix();
	else
#endif
	{
	    XGCValues	values;
	    Pixel	set_bits = (Pixel) (fore_Pixel & ~back_Pixel);
	    Pixel	clr_bits = (Pixel) (back_Pixel & ~fore_Pixel);

	    if (copy == Maybe)
		copy = (!resource.thorough
		    && DefaultDepthOfScreen(SCRN) > 1);

#define	MakeGC(fcn, fg, bg)	(values.function = fcn, values.foreground=fg,\
		values.background=bg,\
		XCreateGC(DISP, RootWindowOfScreen(SCRN),\
			GCFunction|GCForeground|GCBackground, &values))

	    foreGC = foreGC2 = ruleGC = NULL;
	    if (copy || (set_bits && clr_bits))
		ruleGC = MakeGC(GXcopy, fore_Pixel, back_Pixel);
	    if (copy) foreGC = ruleGC;
	    else if (!resource.thorough && ruleGC) {
		foreGC = ruleGC;
		warn = True;
	    }
	    else {
		if (set_bits) foreGC = MakeGC(GXor, set_bits, 0);
		if (clr_bits || !set_bits)
		    *(foreGC ? &foreGC2 : &foreGC) =
			MakeGC(GXandInverted, clr_bits, 0);
		if (!ruleGC) ruleGC = foreGC;
	    }
	}

	if (warn && !warn_done) {
	    Puts("Note:  overstrike characters may be incorrect.");
	    warn_done = True; /* Print warning only once. */
	}
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


static void
draw_rulers(width, height, ourGC)
	unsigned int	width, height;
	GC		ourGC;
{
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

    XClearArea(DISP, alt.win, x, y, width, height, True);

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

    XFlush(DISP);			/* bring window up-to-date */
}

#undef	MakeGC

#endif	/* GREY */

/*
 *	Event-handling routines
 */

static	void
expose(windowrec, x, y, w, h)
	struct WindowRec *windowrec;
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
	struct WindowRec *windowrec;
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
	    XCopyArea(DISP, windowrec->win, windowrec->win, copyGC,
		x, y, (unsigned int) ww, (unsigned int) hh, x2, y2);
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

#ifdef	TOOLKIT

/*
 *	routines for X11 toolkit
 */

static	Arg	arg_wh[] = {
	{XtNwidth,	(XtArgVal) &window_w},
	{XtNheight,	(XtArgVal) &window_h},
};

static	Position	window_x, window_y;
static	Arg	arg_xy[] = {
	{XtNx,		(XtArgVal) &window_x},
	{XtNy,		(XtArgVal) &window_y},
};

#define	get_xy()	XtGetValues(draw_widget, arg_xy, XtNumber(arg_xy))

#define	mane_base_x	0
#define	mane_base_y	0


#ifdef MOTIF

static	int
set_bar_value(bar, value, max)
	Widget	bar;
	int	value;
	int	max;
{
	XmScrollBarCallbackStruct	call_data;

	if (value > max) value = max;
	if (value < 0) value = 0;
	call_data.value = value;
	XtVaSetValues(bar, XmNvalue, value, NULL);
	XtCallCallbacks(bar, XmNvalueChangedCallback, &call_data);
	return value;
}

#endif /* MOTIF */

/* not static because SELFILE stuff in dvi_init.c needs it */
void 
home(scrl)
	Boolean	scrl;
{
#if	PS
	psp.interrupt();
#endif
	if (!scrl) XUnmapWindow(DISP, mane.win);
#ifndef MOTIF
	get_xy();
	if (x_bar != NULL) {
	    int coord = (page_w - clip_w) / 2;

	    if (coord > home_x / mane.shrinkfactor)
		coord = home_x / mane.shrinkfactor;
	    XtCallCallbacks(x_bar, XtNscrollProc,
		(XtPointer) (window_x + coord));
	}
	if (y_bar != NULL) {
	    int coord = (page_h - clip_h) / 2;

	    if (coord > home_y / mane.shrinkfactor)
		coord = home_y / mane.shrinkfactor;
	    XtCallCallbacks(y_bar, XtNscrollProc,
		(XtPointer) (window_y + coord));
	}
#else /* MOTIF */
	{
	    int value;

	    value = (page_w - clip_w) / 2;
	    if (value > home_x / mane.shrinkfactor)
		value = home_x / mane.shrinkfactor;
	    (void) set_bar_value(x_bar, value, (int) (page_w - clip_w));

	    value = (page_h - clip_h) / 2;
	    if (value > home_y / mane.shrinkfactor)
		value = home_y / mane.shrinkfactor;
	    (void) set_bar_value(y_bar, value, (int) (page_h - clip_h));
	}
#endif /* MOTIF */
	if (!scrl) {
	    XMapWindow(DISP, mane.win);
	    /* Wait for the server to catch up---this eliminates flicker. */
	    XSync(DISP, False);
	}
}


#ifndef MOTIF
	/*ARGSUSED*/
static	void
handle_destroy_bar(w, client_data, call_data)
	Widget		w;
	XtPointer	client_data;
	XtPointer	call_data;
{
	* (Widget *) client_data = NULL;
}
#endif


static	Boolean resized = False;

static	void
get_geom()
{
	static	Dimension	new_clip_w, new_clip_h;
	static	Arg		arg_wh_clip[] = {
		{XtNwidth,	(XtArgVal) &new_clip_w},
		{XtNheight,	(XtArgVal) &new_clip_h},
	};
	int	old_clip_w;

	XtGetValues(vport_widget, arg_wh, XtNumber(arg_wh));
	XtGetValues(clip_widget, arg_wh_clip, XtNumber(arg_wh_clip));
#ifndef MOTIF
	/* Note:  widgets may be destroyed but not forgotten */
	if (x_bar == NULL) {
	    x_bar = XtNameToWidget(vport_widget, "horizontal");
	    if (x_bar != NULL)
		XtAddCallback(x_bar, XtNdestroyCallback, handle_destroy_bar,
		    (XtPointer) &x_bar);
	}
	if (y_bar == NULL) {
	    y_bar = XtNameToWidget(vport_widget, "vertical");
	    if (y_bar != NULL)
		XtAddCallback(y_bar, XtNdestroyCallback, handle_destroy_bar,
		    (XtPointer) &y_bar);
	}
#endif
	old_clip_w = clip_w;
			/* we need to do this because */
			/* sizeof(Dimension) != sizeof(int) */
	clip_w = new_clip_w;
	clip_h = new_clip_h;
	if (old_clip_w == 0) home(False);
	resized = False;
}

/*
 *	callback routines
 */

	/*ARGSUSED*/
void
handle_resize(widget, junk, event, cont)
	Widget	widget;
	XtPointer junk;
	XEvent	*event;
	Boolean	*cont;		/* unused */
{
	resized = True;
}

#ifdef	BUTTONS
	/*ARGSUSED*/
static	void
handle_command(widget, client_data_p, call_data)
	Widget	widget;
	XtPointer client_data_p;
	XtPointer call_data;
{
	int	client_data	= * (int *) client_data_p;

	keystroke((client_data) & 0xff, (client_data) >> 8,
		((client_data) >> 8) != 0, (XEvent *) NULL);
}

	/*ARGSUSED*/
static	void
handle_destroy_buttons(w, client_data, call_data)
	Widget		w;
	XtPointer	client_data;
	XtPointer	call_data;
{
	if (--destroy_count != 0) return;
#ifndef MOTIF
	XtSetValues(vport_widget, resizable_on, XtNumber(resizable_on));
	if (resource.expert) {
	    XtGetValues(form_widget, arg_wh, XtNumber(arg_wh));
	    XdviResizeWidget(vport_widget, window_w, window_h);
	}
	else {
	    XdviResizeWidget(vport_widget, window_w -= XTRA_WID, window_h);
	    create_buttons((XtArgVal) window_h);
	}
#else /* MOTIF */
	if (resource.expert)
	    XtVaSetValues(vport_widget,
	      XmNleftAttachment, XmATTACH_FORM,
	      XmNleftOffset, 0,
	      NULL);
	else {
	    create_buttons((XtArgVal) 0);	/* the argument is ignored */
	    window_w -= XTRA_WID;
	}
#endif /* MOTIF */
}

#endif	/* BUTTONS */

void
reconfig()
{
#ifdef	BUTTONS
#ifndef MOTIF
	XtSetValues(vport_widget, resizable_off, XtNumber(resizable_off));
#endif
#endif
	XdviResizeWidget(draw_widget, page_w, page_h);
	get_geom();
}


#ifdef HTEX

#define BIGBUF 32768
char anchorpaintstring[BIGBUF];
static Arg new_paint_args[] = {
	{XtNstring,	(XtArgVal) anchorpaintstring},
};

/* Using Text widget ??? - see if this works... */
void paint_anchor(text)
char *text;
{
	static long pos = 0; /* Keep track of how many characters we've added */
	int len;
	char *firstnl;

	if (text == NULL) { /* Not on anchor any more */
		if ((pos > 0) && (pos < BIGBUF) && (anchorpaintstring[pos-1] != '\n')) {
			anchorpaintstring[pos]= '\n'; /* But we were */
			anchorpaintstring[pos+1]= '\0';
			pos += 1;
			XtSetValues(anchor_info, new_paint_args,
 				XtNumber(new_paint_args));
			XawTextSetInsertionPoint(anchor_info, pos);
		}
		return;
	} /* There IS some anchor text under the mouse */
	len = strlen(text);
	if (!strcmp(text, anchorpaintstring+pos-len)) { /* Still on same anchor */
		return;
	}
	if (len > BIGBUF) { /* Try it anyway: */
		text[BIGBUF-1] = '\0';
		len = BIGBUF;
	}
	if ((pos + len) > BIGBUF) { /* Fix in case of overflows... */
		firstnl = strchr(anchorpaintstring+pos+len-BIGBUF, '\n');
		if (firstnl == NULL) {
			pos = 0;
		} else {
			strcpy(anchorpaintstring,firstnl+1);
			pos -= 1 + firstnl - anchorpaintstring;
		}
	}
	if (pos < 0) pos = 0; /* Shouldn't happen but... */
	sprintf(anchorpaintstring+pos,"\n%s", text);
 	pos += len+1;

	XtSetValues(anchor_info, new_paint_args, XtNumber(new_paint_args));
	XawTextSetInsertionPoint(anchor_info, pos);
	XawTextDisplay(anchor_info);
}

extern char anchorsearchstring[]; /* not being used? */

/* Callback for anchor search: */
void search_callback(w, c_p, call_data)
Widget w;
XtPointer c_p, call_data;
{
	int len;
	char *cp;

	cp = XawDialogGetValueString(anchor_search);
	if (cp == NULL) return;
	len = strlen(cp);
	strcpy(anchorsearchstring,cp);
	if (strchr(cp,'\n') == NULL) {
		anchorsearchstring[len] = '\n';
		anchorsearchstring[len+1] = '\0';
	}
	len = strchr(anchorsearchstring,'\n') - anchorsearchstring + 1;
	add_search(anchorsearchstring, len);
}
#endif

#else	/* not TOOLKIT */

#ifdef HTEX
/* The anchor window stuff: */

void paint_anchor(text)
char *text;
{
	ClearArea(anchor_info, 1, 1, clip_w/2, BAR_THICK);
	if (text != NULL) XDrawString(DISP, anchor_info, foreGC,
		1 , BAR_WID, text, strlen(text));
}

#endif

/*
 *	brute force scrollbar routines
 */

static	void
paint_x_bar()
{
	int	new_x_bgn = mane.base_x * clip_w / page_w;
	int	new_x_end = (mane.base_x + clip_w) * clip_w / page_w;

	if (new_x_bgn >= x_end || x_bgn >= new_x_end) {	/* no overlap */
	    XClearArea(DISP, x_bar, x_bgn, 1, x_end - x_bgn, BAR_WID, False);
	    XFillRectangle(DISP, x_bar, ruleGC,
		new_x_bgn, 1, new_x_end - new_x_bgn, BAR_WID);
	}
	else {		/* this stuff avoids flicker */
	    if (x_bgn < new_x_bgn)
		XClearArea(DISP, x_bar, x_bgn, 1, new_x_bgn - x_bgn,
		    BAR_WID, False);
	    else
		XFillRectangle(DISP, x_bar, ruleGC,
		    new_x_bgn, 1, x_bgn - new_x_bgn, BAR_WID);
	    if (new_x_end < x_end)
		XClearArea(DISP, x_bar, new_x_end, 1, x_end - new_x_end,
		    BAR_WID, False);
	    else
		XFillRectangle(DISP, x_bar, ruleGC,
		    x_end, 1, new_x_end - x_end, BAR_WID);
	}
	x_bgn = new_x_bgn;
	x_end = new_x_end;
}

static	void
paint_y_bar()
{
	int	new_y_bgn = mane.base_y * clip_h / page_h;
	int	new_y_end = (mane.base_y + clip_h) * clip_h / page_h;

	if (new_y_bgn >= y_end || y_bgn >= new_y_end) {	/* no overlap */
	    XClearArea(DISP, y_bar, 1, y_bgn, BAR_WID, y_end - y_bgn, False);
	    XFillRectangle(DISP, y_bar, ruleGC,
		1, new_y_bgn, BAR_WID, new_y_end - new_y_bgn);
	}
	else {		/* this stuff avoids flicker */
	    if (y_bgn < new_y_bgn)
		XClearArea(DISP, y_bar, 1, y_bgn, BAR_WID, new_y_bgn - y_bgn,
		    False);
	    else
		XFillRectangle(DISP, y_bar, ruleGC,
		    1, new_y_bgn, BAR_WID, y_bgn - new_y_bgn);
	    if (new_y_end < y_end)
		XClearArea(DISP, y_bar, 1, new_y_end,
		    BAR_WID, y_end - new_y_end, False);
	    else
		XFillRectangle(DISP, y_bar, ruleGC,
		    1, y_end, BAR_WID, new_y_end - y_end);
	}
	y_bgn = new_y_bgn;
	y_end = new_y_end;
}

static	void
scrollmane(x, y)
	int	x, y;
{
	int	old_base_x = mane.base_x;
	int	old_base_y = mane.base_y;

#if	PS
	psp.interrupt();
#endif
	if (x > (int) (page_w - clip_w)) x = page_w - clip_w;
	if (x < 0) x = 0;
	if (y > (int) (page_h - clip_h)) y = page_h - clip_h;
	if (y < 0) y = 0;
	scrollwindow(&mane, x, y);
	if (old_base_x != mane.base_x && x_bar) paint_x_bar();
	if (old_base_y != mane.base_y && y_bar) paint_y_bar();
}

void
reconfig()
{
	int	x_thick = 0;
	int	y_thick = 0;

		/* determine existence of scrollbars */
	if (window_w < page_w) x_thick = BAR_THICK;
	if (window_h - x_thick < page_h) y_thick = BAR_THICK;
	clip_w = window_w - y_thick;
	if (clip_w < page_w) x_thick = BAR_THICK;
	clip_h = window_h - x_thick;

		/* process drawing (clip) window */
	if (mane.win == (Window) 0) {	/* initial creation */
	    XWindowAttributes attrs;

	    mane.win = XCreateSimpleWindow(DISP, top_level, y_thick, x_thick,
			(unsigned int) clip_w, (unsigned int) clip_h, 0,
			brdr_Pixel, back_Pixel);
	    XSelectInput(DISP, mane.win, ExposureMask |
#ifdef HTEX
			PointerMotionMask | PointerMotionHintMask |
			ButtonPressMask | ButtonReleaseMask);
#else
			ButtonPressMask | ButtonMotionMask | ButtonReleaseMask);
#endif
	    (void) XGetWindowAttributes(DISP, mane.win, &attrs);
	    backing_store = attrs.backing_store;
	    XMapWindow(DISP, mane.win);
	}
	else
	    XMoveResizeWindow(DISP, mane.win, y_thick, x_thick, clip_w, clip_h);

#ifdef HTEX
	if (anchor_info) {
		XMoveResizeWindow(DPY anchor_info,
		    y_thick - 1, x_thick - BAR_THICK - 1,
					 clip_w/2, BAR_THICK - 1);
		paint_anchor(NULL);
		XMoveResizeWindow(DPY anchor_search,
		    y_thick + clip_w/2 - 1, x_thick - BAR_THICK - 1,
						 clip_w/2, BAR_THICK - 1);
	} else {
		anchor_info = XCreateSimpleWindow(DISP, top_level, y_thick - 1,
				x_thick - BAR_THICK -1,
				(unsigned int) clip_w/2, BAR_THICK - 1, 1,
				brdr_Pixel, back_Pixel);
		XSelectInput(DISP, anchor_info, ExposureMask);
		anchor_search = XCreateSimpleWindow(DISP, top_level,
				y_thick + clip_w/2 - 1,
				x_thick - BAR_THICK -1,
				(unsigned int) clip_w/2, BAR_THICK - 1, 1,
				brdr_Pixel, back_Pixel);
		XSelectInput(DISP, anchor_search, ExposureMask|KeyPressMask);
		XMapWindow(DPY anchor_info);
		XMapWindow(DPY anchor_search);
	}
#endif
		/* process scroll bars */
	if (x_thick) {
	    if (x_bar) {
		XMoveResizeWindow(DISP, x_bar,
		    y_thick - 1, -1, clip_w, BAR_THICK - 1);
		paint_x_bar();
	    }
	    else {
		x_bar = XCreateSimpleWindow(DISP, top_level, y_thick - 1, -1,
				(unsigned int) clip_w, BAR_THICK - 1, 1,
				brdr_Pixel, back_Pixel);
		XSelectInput(DISP, x_bar,
			ExposureMask | ButtonPressMask | Button2MotionMask);
		XMapWindow(DISP, x_bar);
	    }
	    x_bgn = mane.base_x * clip_w / page_w;
	    x_end = (mane.base_x + clip_w) * clip_w / page_w;
	}
	else
	    if (x_bar) {
		XDestroyWindow(DISP, x_bar);
		x_bar = (Window) 0;
	    }

	if (y_thick) {
	    if (y_bar) {
		XMoveResizeWindow(DISP, y_bar,
		    -1, x_thick - 1, BAR_THICK - 1, clip_h);
		paint_y_bar();
	    }
	    else {
		y_bar = XCreateSimpleWindow(DISP, top_level, -1, x_thick - 1,
				BAR_THICK - 1, (unsigned int) clip_h, 1,
				brdr_Pixel, back_Pixel);
		XSelectInput(DISP, y_bar,
			ExposureMask | ButtonPressMask | Button2MotionMask);
		XMapWindow(DISP, y_bar);
	    }
	    y_bgn = mane.base_y * clip_h / page_h;
	    y_end = (mane.base_y + clip_h) * clip_h / page_h;
	}
	else
	    if (y_bar) {
		XDestroyWindow(DISP, y_bar);
		y_bar = (Window) 0;
	    }
}

static	void
home(scrl)
	Boolean	scrl;
{
	int	x = 0, y = 0;

	if (page_w > clip_w) {
	    x = (page_w - clip_w) / 2;
	    if (x > home_x / mane.shrinkfactor)
		x = home_x / mane.shrinkfactor;
	}
	if (page_h > clip_h) {
	    y = (page_h - clip_h) / 2;
	    if (y > home_y / mane.shrinkfactor)
		y = home_y / mane.shrinkfactor;
	}
	if (scrl)
	    scrollmane(x, y);
	else {
	    mane.base_x = x;
	    mane.base_y = y;
	    if (currwin.win == mane.win) {
		currwin.base_x = x;
		currwin.base_y = y;
	    }
	    if (x_bar) paint_x_bar();
	    if (y_bar) paint_y_bar();
	}
}

#define	get_xy()
#define	window_x 0
#define	window_y 0
#define	mane_base_x	mane.base_x
#define	mane_base_y	mane.base_y
#endif	/* not TOOLKIT */

static	void
compute_mag_pos(xp, yp)
	int	*xp, *yp;
{
	int t;

	t = mag_x + main_x - alt.width/2;
	if (t > WidthOfScreen(SCRN) - (int) alt.width - 2*MAGBORD)
	    t = WidthOfScreen(SCRN) - (int) alt.width - 2*MAGBORD;
	if (t < 0) t = 0;
	*xp = t;
	t = mag_y + main_y - alt.height/2;
	if (t > HeightOfScreen(SCRN) - (int) alt.height - 2*MAGBORD)
	    t = HeightOfScreen(SCRN) - (int) alt.height - 2*MAGBORD;
	if (t < 0) t = 0;
	*yp = t;
}


#define	TRSIZE	100

/* BEGIN CHUNK events.c 1 */
#ifdef SRC_SPECIALS
#define src_jumpButton resource._src_jumpButton
#endif
/* END CHUNK events.c 1 */
#ifdef	TOOLKIT
	/*ARGSUSED*/
void
handle_key(widget, junk, eventp, cont)
	Widget	widget;
	XtPointer junk;
	XEvent	*eventp;
	Boolean	*cont;		/* unused */
#else	/* !TOOLKIT */
void
handle_key(eventp)
	XEvent *eventp;
#endif	/* TOOLKIT */
{
	static	Boolean	has_arg		= False;
	static	int	number		= 0;
	static	int	sign		= 1;
	char	ch;
	Boolean	arg0;
	int	number0;
	char	trbuf[TRSIZE];
	int	nbytes;

	nbytes = XLookupString(&eventp->xkey, trbuf, TRSIZE, (KeySym *) NULL,
	    (XComposeStatus *) NULL);
	if (nbytes == 0) return;
	ch = '\0';
	if (nbytes == 1) ch = *trbuf;
	if (ch >= '0' && ch <= '9') {
	    has_arg = True;
	    number = number * 10 + sign * (ch - '0');
	    return;
	}
	else if (ch == '-') {
	    has_arg = True;
	    sign = -1;
	    number = 0;
	    return;
	}
	number0 = number;
	number = 0;
	sign = 1;
	arg0 = has_arg;
	has_arg = False;
	keystroke(ch, number0, arg0, eventp);
}

#ifdef	TOOLKIT
	/*ARGSUSED*/
void
handle_button(widget, junk, ev, cont)
	Widget	widget;
	XtPointer junk;
	XEvent *ev;
#define	event	(&(ev->xbutton))
	Boolean	*cont;		/* unused */
#else	/* !TOOLKIT */
void
handle_button(event)
	XButtonEvent *event;
#endif	/* TOOLKIT */
{
	int	x, y;
	struct mg_size_rec	*size_ptr = mg_size + event->button - 1;
	XSetWindowAttributes attr;

/* BEGIN CHUNK events.c 2 */
#ifdef SRC_SPECIALS	
	/* If src specials are evaluated, make src_JumpButton jump to the next special. */
	if (src_evalMode) {
	  if (event->button == src_jumpButton) {
		src_find_special(1, event->x, event->y);
		return;
	  }
	}
#endif
/* END CHUNK events.c 2 */
#ifdef HTEX
	if ((event->button == 1)||(event->button == 2)) {
		if (pointerlocate(&x, &y)) {
	/* Only do this if there's actually an href right there */
			int ret;
			/* screen_to_page(&mane,x,y,&page,&px,&py); */
			if (event->button == 2) HTeXnext_extern = 1;
			ret = htex_handleref(current_page, x, y);
			HTeXnext_extern = 0;
			if (ret == 1) return;
		}
	}
#endif
	if (alt.win != (Window) 0 || mane.shrinkfactor == 1 || size_ptr->w <= 0)
	    XBell(DISP, 20);
	else {
	    mag_x = event->x;
	    mag_y = event->y;
	    alt.width = size_ptr->w;
	    alt.height = size_ptr->h;
	    main_x = event->x_root - mag_x;
	    main_y = event->y_root - mag_y;
	    compute_mag_pos(&x, &y);
	    alt.base_x = (event->x + mane_base_x) * mane.shrinkfactor -
		alt.width/2;
	    alt.base_y = (event->y + mane_base_y) * mane.shrinkfactor -
		alt.height/2;
	    attr.save_under = True;
	    attr.border_pixel = brdr_Pixel;
	    attr.background_pixel = back_Pixel;
	    attr.override_redirect = True;
#ifdef GREY
	    attr.colormap = our_colormap;
#endif
	    alt.win = XCreateWindow(DISP, RootWindowOfScreen(SCRN),
			x, y, alt.width, alt.height, MAGBORD,
			our_depth, InputOutput, our_visual,
			CWSaveUnder | CWBorderPixel | CWBackPixel |
#ifdef GREY
			CWColormap |
#endif
			CWOverrideRedirect, &attr);
	    XSelectInput(DISP, alt.win, ExposureMask);
	    XMapWindow(DISP, alt.win);

	    /*****************************************************************
	    This call will draw the point rulers when the magnifier first
	    pops up, if the XDvi*delayRulers resource is false.  Some users
	    may prefer rulers to remain invisible until the magnifier is
	    moved, so the default is true.  Rulers can be suppressed entirely
	    by setting the XDvi*tickLength resource to zero or negative.

	    It is better to use highGC for draw_rulers() than foreGC,
	    because this allows the ruler to be drawn in a different color,
	    helping to distinguish ruler ticks from normal typeset text.
	    *****************************************************************/
	    if (!delay_rulers)
		draw_rulers(alt.width, alt.height, highGC);

	    alt_stat = 1;	/* waiting for exposure */
	}
}

#ifdef	TOOLKIT
#undef	event

	/*ARGSUSED*/
void
handle_motion(widget, junk, ev, cont)
	Widget	widget;
	XtPointer junk;
	XEvent *ev;
#define	event	(&(ev->xmotion))
	Boolean	*cont;		/* unused */
{
#ifdef HTEX
	int x, y;

/* APS Modification required because of changes in motion events selected: */
	/* We asked only for hints, so check where pointer is: */
	if (pointerlocate(&x, &y)) {
		if ((event->state&(Button1Mask|Button2Mask|Button3Mask|
					Button4Mask|Button5Mask)) == 0) {
			htex_displayanchor(current_page, x, y);
			return;
		}
	}
#endif
	new_mag_x = event->x;
	main_x = event->x_root - new_mag_x;
	new_mag_y = event->y;
	main_y = event->y_root - new_mag_y;
	mag_moved = (new_mag_x != mag_x || new_mag_y != mag_y);
}

#undef	event
#endif	/* TOOLKIT */

static	void
movemag(x, y)
	int	x, y;
{
	int	xx, yy;

	mag_x = x;
	mag_y = y;
	if (mag_x == new_mag_x && mag_y == new_mag_y) mag_moved = False;
	compute_mag_pos(&xx, &yy);
	XMoveWindow(DISP, alt.win, xx, yy);
	scrollwindow(&alt,
	    (x + mane_base_x) * mane.shrinkfactor - (int) alt.width/2,
	    (y + mane_base_y) * mane.shrinkfactor - (int) alt.height/2);
	draw_rulers(alt.width, alt.height, highGC);
}

#ifdef	TOOLKIT
	/*ARGSUSED*/
void
handle_release(widget, junk, ev, cont)
	Widget	widget;
	XtPointer junk;
	XEvent *ev;
#define	event	(&(ev->xbutton))
	Boolean	*cont;		/* unused */
#else	/* !TOOLKIT */
void
handle_release()
#endif	/* TOOLKIT */
{
	if (alt.win != (Window) 0)
	    if (alt_stat) alt_stat = -1;	/* destroy upon expose */
	    else {
		XDestroyWindow(DISP, alt.win);
		if (currwin.win == alt.win) alt_canit = True;
		alt.win = (Window) 0;
		mag_moved = False;
		can_exposures(&alt);
	    }
}

#ifdef	TOOLKIT
#undef	event

	/*ARGSUSED*/
void
handle_exp(widget, closure, ev, cont)
	Widget	widget;
	XtPointer closure;
	XEvent *ev;
#define	event	(&(ev->xexpose))
	Boolean	*cont;		/* unused */
{
	struct WindowRec *windowrec = (struct WindowRec *) closure;

	if (windowrec == &alt)
	    if (alt_stat < 0) {	/* destroy upon exposure */
		alt_stat = 0;
		handle_release(widget, (caddr_t) NULL, ev, (Boolean *) NULL);
		return;
	    }
	    else
		alt_stat = 0;
	expose(windowrec, event->x, event->y,
	    (unsigned int) event->width, (unsigned int) event->height);
}

#undef	event
#endif	/* TOOLKIT */

#ifdef MOTIF

/* ARGSUSED */
void
file_pulldown_callback(w, client_data, call_data)
	Widget		w;
	XtPointer	client_data;
	XtPointer	call_data;
{
	switch ((int) client_data) {
	case 0:
	    keystroke('R', 0, False, (XEvent *) NULL);
	    break;
	case 1:
	    keystroke('q', 0, False, (XEvent *) NULL);
	    break;
	}
}

/* ARGSUSED */
void
navigate_pulldown_callback(w, client_data, call_data)
	Widget		w;
	XtPointer	client_data;
	XtPointer	call_data;
{
	switch ((int) client_data) {
	case 0:
	    keystroke('p', 10, True, (XEvent *) NULL);
	    break;
	case 1:
	    keystroke('p', 5, True, (XEvent *) NULL);
	    break;
	case 2:
	    keystroke('p', 0, False, (XEvent *) NULL);
	    break;
	case 3:
	    keystroke('n', 0, False, (XEvent *) NULL);
	    break;
	case 4:
	    keystroke('n', 5, True, (XEvent *) NULL);
	    break;
	case 5:
	    keystroke('n', 10, True, (XEvent *) NULL);
	    break;
	}
}

/* ARGSUSED */
void
scale_pulldown_callback(w, client_data, call_data)
	Widget		w;
	XtPointer	client_data;
	XtPointer	call_data;
{
	int	data	= (int) client_data;

	if (data >= 0 && data < 4)
	    keystroke('s', data + 1, True, (XEvent *) NULL);
}

void
set_shrink_factor(shrink)
	int	shrink;
{
	static	Widget	active_shrink_button	= NULL;
	Widget		new_shrink_button;

	mane.shrinkfactor = shrink;
	new_shrink_button = (shrink > 0 && shrink <= XtNumber(shrink_button)
	  ? shrink_button[shrink - 1] : NULL);
	if (new_shrink_button != active_shrink_button) {
	    if (active_shrink_button != NULL)
		XmToggleButtonSetState(active_shrink_button, False, False);
	    if (new_shrink_button != NULL)
		XmToggleButtonSetState(new_shrink_button, True, False);
	    active_shrink_button = new_shrink_button;
	}
}

#endif /* MOTIF */

void
showmessage(message)
	_Xconst	char	*message;
{
	get_xy();
	XDrawImageString(DISP, mane.win, copyGC,
	    5 - window_x, 5 + X11HEIGHT - window_y, message, strlen(message));
}

/* |||
 *	Currently the event handler does not coordinate XCopyArea requests
 *	with GraphicsExpose events.  This can lead to problems if the window
 *	is partially obscured and one, for example, drags a scrollbar.
 */

#ifdef SRC_SPECIALS
// TEST !!!!!
extern void test_print_out_list ARGS((void));
extern void test_print_out_reversed_list ARGS((void));
#endif
static	void
keystroke(ch, number0, arg0, eventp)
	char	ch;
	int	number0;
	Boolean	arg0;
	XEvent	*eventp;
{
	static int last_number = 0;	/* memory for Again command */
	static char last_ch = '\0';	/* ditto */
	static Boolean last_arg = False; /* ditto */

/* BEGIN CHUNK events.c 3 */
	int src_x, src_y;
/* END CHUNK events.c 3 */
	
	int	next_page;
#ifdef	TOOLKIT
	Window	ww;
#endif

	next_page = current_page;

#if DEBUG
	if (isprint(ch))
	    fprintf(stderr,"<%c>",(int)ch);
	else
	    fprintf(stderr,"<%03o>",(int)ch);
#endif

	if (ch == '\001')		/* Ctl-A or Again */
	{
	    ch = last_ch;
	    number0 = last_number;
	    arg0 = last_arg;
	}
	else			/* remember key and arg for Again command */
	{
	    last_ch = ch;
	    last_number = number0;
	    last_arg = arg0;
	}

	switch (ch) {
	    case 'q':
	    case 'Q':
	    case '\003':	/* control-C */
	    case '\004':	/* control-D */
#ifdef	VMS
	    case '\032':	/* control-Z */
#endif
#if	PS
		ps_destroy();
#endif
#ifndef FLAKY_SIGPOLL
		if (debug & DBG_EVENT)
		    puts(event_freq < 0
		      ? "SIGPOLL is working" : "no SIGPOLL signals received");
#endif
		exit((ch == 'Q') ? 2 : 0);

	    case 'n':
	    case 'f':
	    case ' ':
	    case '\r':
	    case '\n':
		/* scroll forward; i.e. go to relative page */
		next_page = current_page + (arg0 ? number0 : 1);
		break;
#ifdef HTEX
	    case 'F': /* Follow link forward! */
		{
			int x, y;
			if (pointerlocate(&x, &y)) {
			/* screen_to_page(&mane,s_x,s_y,&page,&px,&py); */
				(void) htex_handleref(current_page, x, y);
			}
		}
		return; /* Should goto bad if problem arises? */
	    case 'B': /* Go back to previous anchor. */
		htex_goback(); /* Should goto bad if problem arises? */
		return;
#endif
	    case 'p':
	    case 'b':
	    case '\b':
	    case '\177':	/* Del */
		/* scroll backward */
		next_page = current_page - (arg0 ? number0 : 1);
		break;
	    case '<':
	       next_page = 0;
	       break;
	    case 'g':
	    case 'j':
	    case '>':
		/* go to absolute page (last by default) */
		next_page = (arg0 ? number0 - pageno_correct :
		    total_pages - 1);
		break;
	    case '?':
	    case 'h':
	    case 'H':			/* Help */
		show_help();
		return;
	    case 'P':		/* declare current page */
		pageno_correct = arg0 * number0 - current_page;
		return;
	    case 'k':		/* toggle keep-position flag */
		resource.keep_flag = (arg0 ? number0 : !resource.keep_flag);
		return;
	    case '\f':
		/* redisplay current page */
		break;
	    case '^':
		home(True);
		return;
#ifdef	TOOLKIT
#ifndef MOTIF
	    case 'l':
		if (!x_bar) goto bad;
		XtCallCallbacks(x_bar, XtNscrollProc,
		    (XtPointer) (-2 * (int) clip_w / 3));
		return;
	    case 'r':
		if (!x_bar) goto bad;
		XtCallCallbacks(x_bar, XtNscrollProc,
		    (XtPointer) (2 * (int) clip_w / 3));
		return;
	    case 'u':
		if (!y_bar) goto bad;
		XtCallCallbacks(y_bar, XtNscrollProc,
		    (XtPointer) (-2 * (int) clip_h / 3));
		return;
	    case 'd':
		if (!y_bar) goto bad;
		XtCallCallbacks(y_bar, XtNscrollProc,
		    (XtPointer) (2 * (int) clip_h / 3));
		return;
	    case 'c':
		{
		    int x, y;

		    x = eventp->xkey.x - clip_w / 2;
		    y = eventp->xkey.y - clip_h / 2;
		    /* The clip widget gives a more exact value. */
		    if (x_bar != NULL)
			XtCallCallbacks(x_bar, XtNscrollProc, (XtPointer) x);
		    if (y_bar != NULL)
			XtCallCallbacks(y_bar, XtNscrollProc, (XtPointer) y);
		    XWarpPointer(DISP, None, None, 0, 0, 0, 0, -x, -y);
		}
		return;
#else /* MOTIF */
	    case 'c':
		{
		    int x, y;

		    get_xy();
		    /* The clip widget gives a more exact value. */
		    x = eventp->xkey.x - clip_w / 2;
		    y = eventp->xkey.y - clip_h / 2;

		    x = set_bar_value(x_bar, x, (int) (page_w - clip_w));
		    y = set_bar_value(y_bar, y, (int) (page_h - clip_h));
		    XWarpPointer(DISP, None, None, 0, 0, 0, 0,
		      -x - window_x, -y - window_y);
		}
		return;
	    /* These four won't be needed once translations are put in. */
	    case 'l':
		get_xy();
		(void) set_bar_value(x_bar, -2 * (int) clip_w / 3 - window_x,
		  (int) (page_w - clip_w));
		return;
	    case 'r':
		get_xy();
		(void) set_bar_value(x_bar, 2 * (int) clip_w / 3 - window_x,
		  (int) (page_w - clip_w));
		return;
	    case 'u':
		get_xy();
		(void) set_bar_value(y_bar, -2 * (int) clip_h / 3 - window_y,
		  (int) (page_h - clip_h));
		return;
	    case 'd':
		get_xy();
		(void) set_bar_value(y_bar, 2 * (int) clip_h / 3 - window_y,
		  (int) (page_h - clip_h));
		return;
#endif /* MOTIF */
	    case 'M':
		(void) XTranslateCoordinates(DISP, eventp->xkey.window,
			mane.win, eventp->xkey.x, eventp->xkey.y,
			&home_x, &home_y, &ww);	/* throw away last argument */
		home_x *= mane.shrinkfactor;
		home_y *= mane.shrinkfactor;
		return;
/* BEGIN CHUNK events.c 4 */
#ifdef SRC_SPECIALS
    	case 'X':
		  (void) XTranslateCoordinates(DISP, eventp->xkey.window,
									   mane.win, eventp->xkey.x, eventp->xkey.y,
									   &src_x, &src_y, &ww);	/* throw away last argument */
		  /* just highlight next special without calling editor for it */
		  src_find_special(0, src_x, src_y);
		  return; /* or `break'? */
#endif		
/* END CHUNK events.c 4 */
#ifdef	BUTTONS
	    case 'x':
		if (arg0 && resource.expert == (number0 != 0)) return;
		if (resource.expert) {	/* create buttons */
		    resource.expert = False;
		    if (destroy_count != 0) return;
#ifndef MOTIF
		    XtSetValues(vport_widget, resizable_on,
			XtNumber(resizable_on));
		    XdviResizeWidget(vport_widget,
			window_w -= XTRA_WID, window_h);
		    create_buttons((XtArgVal) window_h);
#else /* MOTIF */
		    create_buttons((XtArgVal) 0);	/* the argument is ignored */
		    window_w -= XTRA_WID;
#endif /* MOTIF */
		}
		else {		/* destroy buttons */
		    resource.expert = True;
		    if (destroy_count != 0) return;
		    destroy_count = 2;
		    XtAddCallback(panel_widget, XtNdestroyCallback,
			handle_destroy_buttons, (XtPointer) 0);
		    XtAddCallback(line_widget, XtNdestroyCallback,
			handle_destroy_buttons, (XtPointer) 0);
		    XtDestroyWidget(panel_widget);
		    XtDestroyWidget(line_widget);
		    window_w += XTRA_WID;
		}
		return;
#endif	/* BUTTONS */
#else	/* not TOOLKIT */
	    case 'l':
		if (mane.base_x <= 0) goto bad;
		scrollmane(mane.base_x - 2 * (int) clip_w / 3, mane.base_y);
		return;
	    case 'r':
		if (mane.base_x >= page_w - clip_w) goto bad;
		scrollmane(mane.base_x + 2 * (int) clip_w / 3, mane.base_y);
		return;
	    case 'u':
		if (mane.base_y <= 0) goto bad;
		scrollmane(mane.base_x, mane.base_y - 2 * (int) clip_h / 3);
		return;
	    case 'd':
		if (mane.base_y >= page_h - clip_h) goto bad;
		scrollmane(mane.base_x, mane.base_y + 2 * (int) clip_h / 3);
		return;
	    case 'c':	/* unchecked scrollmane() */
		scrollwindow(&mane, mane.base_x + eventp->xkey.x - clip_w/2,
		    mane.base_y + eventp->xkey.y - clip_h/2);
		if (x_bar) paint_x_bar();
		if (y_bar) paint_y_bar();
		XWarpPointer(DISP, None, None, 0, 0, 0, 0,
		    clip_w/2 - eventp->xkey.x, clip_h/2 - eventp->xkey.y);
		return;
	    case 'M':
		home_x = (eventp->xkey.x - (y_bar ? BAR_THICK : 0)
		    + mane.base_x) * mane.shrinkfactor;
		home_y = (eventp->xkey.y - (x_bar ? BAR_THICK : 0)
		    + mane.base_y) * mane.shrinkfactor;
		return;
/* BEGIN CHUNK events.c 5 */
#ifdef SRC_SPECIALS
	    case 'X':
		  /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		   * !!! WARNING: THIS IS UNTESTED !!!
		   * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		   */
		  src_x = (eventp->xkey.x - (y_bar ? BAR_THICK : 0)
				   + mane.base_x);
		  src_y = (eventp->xkey.y - (x_bar ? BAR_THICK : 0)
				   + mane.base_y);
		  src_find_special(0, src_x, src_y);
		  return;
#endif /* SRC_SPECIALS */		
/* END CHUNK events.c 5 */
#endif	/* not TOOLKIT */

/* BEGIN CHUNK events.c 6 */
#ifdef SRC_SPECIALS
		/*
		 * Control-S toggles visibility of src specials
		 * (mnemonic for isearch in Emacs ;-)
		 * Also changes the cursor to emphasize the new mode.
		 */
 	    case '\023': /* Control-S */
		  if (src_evalMode) {
			if (src_warn_verbosity >= SRC_WARNINGS_MEDIUM) {
			  Fprintf(stdout, "SRC specials OFF\n");
			}
			/* free src_arr */
			src_cleanup();
			src_evalMode = False;
			XDefineCursor(DISP, mane.win, ready_cursor);
		  }
		  else {
			if (src_warn_verbosity >= SRC_WARNINGS_MEDIUM) {
			  Fprintf(stdout, "SRC specials ON\n");
			}
			src_evalMode = True;
			/* used to have this in addition to emphasize the mode,
			 * but usage seems more coherent without it: */
			/*	src_tickVisibility = True; */
			XDefineCursor(DISP, mane.win, src_cursor);
		  }
		  redraw_page();
		  break;
	    case 'T':
		  /*
		   * change shape of specials, but only when they're visible;
		   * this makes the key usable for other purposes in ordinary mode.
		   * However, it seems that in ordinary mode `T' already does the same
		   * as Ctrl-p: print the Unit/bitord/byteord stuff; what's
		   * the reason for this ???
		   */
		  if (src_evalMode) {
			src_tickShape++;
			if (src_tickShape > SPECIAL_SHAPE_MAX_NUM) {
			  src_tickShape = 0;
			}
			if (src_warn_verbosity >= SRC_WARNINGS_MEDIUM) {
			  Fprintf(stdout, "changing shape to \"%d\"\n", src_tickShape);
			}
			if (src_tickVisibility) {
			  redraw_page();
			}
			break;
		  }
#ifndef PS_GS
	case 'V':
	  if (src_evalMode) {
		/*
		 * toggle visibility of src specials
		 */
		if (src_tickVisibility) {
		  if (src_warn_verbosity >= SRC_WARNINGS_MEDIUM) {
			Fprintf(stdout, "SRC special visibility OFF\n");
		  }
		  src_tickVisibility = False;
		}
		else {
		  if (src_warn_verbosity >= SRC_WARNINGS_MEDIUM) {
			Fprintf(stdout, "SRC special visibility ON\n");
		  }
		  src_tickVisibility = True;
		}
		redraw_page();
		return;
	  }
#endif  /* PS_GS */		  
#endif  /* SRC_SPECIALS */
/* END CHUNK events.c 6 */
	    case '\020':	/* Control P */
		Printf("Unit = %d, bitord = %d, byteord = %d\n",
		    BitmapUnit(DISP), BitmapBitOrder(DISP),
		    ImageByteOrder(DISP));
		return;
	    case 's':
		if (!arg0) {
		    int temp;

		    number0 = ROUNDUP(unshrunk_page_w, window_w - 2);
#ifndef MOTIF
		    temp = ROUNDUP(unshrunk_page_h, window_h - 2);
#else /* MOTIF */
		    {	/* account for menubar */
			static	Dimension	new_h;

			/* get rid of scrollbar */
			XdviResizeWidget(draw_widget, 1, 1);
			XtVaGetValues(clip_widget, XtNheight, &new_h, NULL);
			temp = ROUNDUP(unshrunk_page_h, new_h - 2);
		    }
#endif /* MOTIF */
		    if (number0 < temp) number0 = temp;
		}
		if (number0 <= 0) goto bad;
		if (number0 == mane.shrinkfactor) return;
#ifndef MOTIF
		mane.shrinkfactor = number0;
#else
		set_shrink_factor(number0);
#endif
		init_page();
		if (number0 != 1 && number0 != bak_shrink) {
		    bak_shrink = number0;
#ifdef	GREY
		    if (use_grey) init_colors();
#endif
		    reset_fonts();
		}
		reconfig();
		home(False);
		break;
	    case 'S':
		if (!arg0) goto bad;
#ifdef	GREY
		if (use_grey) {
		    float newgamma = number0 != 0 ? number0 / 100.0 : 1.0;

		    if (newgamma == gamma) return;
		    gamma = newgamma;
		    init_colors();
		    return;
		}
#endif
		if (number0 < 0) goto bad;
		if (number0 == density) return;
		density = number0;
		reset_fonts();
		if (mane.shrinkfactor == 1) return;
		break;

 	    case 't':
 		{		/* toggle through magnifier ruler tick units */
 		    int k = 0;
 		    static char *TeX_units[] = {
 			"bp", "cc", "cm", "dd", "in", "mm", "pc", "pt", "sp",
 		    };
 
 		    for (k = 0; k < sizeof(TeX_units)/sizeof(TeX_units[0]); ++k)
 			if (strcmp(tick_units,TeX_units[k]) == 0)
 			    break;
 		    k++;
 		    if (k >= sizeof(TeX_units)/sizeof(TeX_units[0]))
 			k = 0;
 		    tick_units = TeX_units[k];
 		    Printf("Ruler units = %s\n", tick_units);
 		}
 		return;

#ifdef	GREY
	    case 'G':
		use_grey = (arg0 ? number0 : !use_grey);
		if (use_grey) init_colors();
		reset_fonts();
		break;
#endif
#ifdef GRID
	    case 'D':
		grid_mode = (arg0 ? number0 : !grid_mode );
		init_page();
		reconfig();
		break;
#endif /* GRID */

#if	PS
	    case 'v':
		if (!arg0 || resource._postscript != !number0) {
		    resource._postscript = !resource._postscript;
		    if (resource._postscript) scanned_page = scanned_page_bak;
		    psp.toggle();
		}
		break;
#endif

#ifdef SELFILE
            case '\006': /* control-f */
		++dvi_time ; /* notice we want a new file in check_dvi_file */
		break ;
#endif  /* SELFILE */

#if PS_GS
	    case 'V':
/* BEGIN CHUNK events.c 7 */
#ifdef SRC_SPECIALS	  
	  if (src_evalMode) {
		/*
		 * toggle visibility of src specials
		 */
		if (src_tickVisibility) {
		  if (src_warn_verbosity >= SRC_WARNINGS_MEDIUM) {
			Fprintf(stdout, "SRC special visibility OFF\n");
		  }
		  src_tickVisibility = False;
		}
		else {
		  if (src_warn_verbosity >= SRC_WARNINGS_MEDIUM) {
			Fprintf(stdout, "SRC special visibility ON\n");
		  }
		  src_tickVisibility = True;
		}
		redraw_page();
		return;
	  }
	  else {
#endif		
/* END CHUNK events.c 7 */
		if (!arg0 || resource.gs_alpha != !number0)
		    resource.gs_alpha = !resource.gs_alpha;
		break;
/* BEGIN CHUNK events.c 8 */
#ifdef SRC_SPECIALS		
	  }
#endif
/* END CHUNK events.c 8 */
#endif

	    case 'R':
		/* reread DVI file */
		--dvi_time;	/* then it will notice a change */
		break;
	    default:
		goto bad;
	}
	if (0 <= next_page && next_page < total_pages) {
	    if (current_page != next_page) {
		  /* BEGIN CHUNK events.c 0 */
#ifdef SRC_SPECIALS
		  src_delete_all_specials();
#endif
/* END CHUNK events.c 0 */
		  current_page = next_page;
		  warn_spec_now = warn_spec;
		  if (!resource.keep_flag) home(False);
	    }
	    canit = True;
	    XFlush(DISP);
	    return;	/* Don't use longjmp here:  it might be called from
			 * within the toolkit, and we don't want to longjmp out
			 * of Xt routines. */
	}
	bad:  XBell(DISP, 10);
}

/*
 *	Since redrawing the screen is (potentially) a slow task, xdvi checks
 *	for incoming events while this is occurring.  It does not register
 *	a work proc that draws and returns every so often, as the toolkit
 *	documentation suggests.  Instead, it checks for events periodically
 *	(or not, if SIGPOLL can be used instead) and processes them in
 *	a subroutine called by the page drawing routine.  This routine (below)
 *	checks to see if anything has happened and processes those events and
 *	signals.  (Or, if it is called when there is no redrawing that needs
 *	to be done, it blocks until something happens.)
 */

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
	XEvent	event;

	alt_canit = False;
	for (;;) {
	    event_counter = event_freq;
	    /*
	     * The above line clears the flag indicating that an event is
	     * pending.  So if an event comes in right now, the flag will be
	     * set again needlessly, but we just end up making an extra call.
	     * Also, be careful about destroying the magnifying glass while
	     * drawing on it.
	     */

#ifndef FLAKY_SIGPOLL

	    if (event_freq < 0) {	/* if SIGPOLL works */

		if (!XtPending()) {
		    sigset_t oldsig;

		    (void) sigprocmask(SIG_BLOCK, &sigpollusrterm, &oldsig);
		    if (terminate_flag) {
#if	PS
			ps_destroy();
#endif
			exit(0);
		    }
		    while (!XtPending())
		    {
			/*
			 * The following code eliminates unnecessary calls to
			 * XDefineCursor, since this is a slow operation on 
			 * some hardware (e.g., S3 chips).
			 */
			if (busycurs && wait && !canit && !mag_moved
			  && alt.min_x == MAXDIM && mane.min_x == MAXDIM) {
			    XSync(DISP, False);
			    if (XtPending()) break;
/* BEGIN CHUNK events.c 9 */
#ifdef SRC_SPECIALS
				if (src_evalMode) {
				  XDefineCursor(DISP, mane.win, src_cursor);
				}
				else {
#endif
/* END CHUNK events.c 9 */
				  XDefineCursor(DISP, mane.win, ready_cursor);
/* BEGIN CHUNK events.c 10 */
#ifdef SRC_SPECIALS
				}
#endif
/* END CHUNK events.c 10 */
			    XFlush(DISP);
			    busycurs = False;
			}
			if (!wait && (canit | alt_canit)) {
#if	PS
			    psp.interrupt();
#endif
			    if (allow_can) {
				(void) sigprocmask(SIG_SETMASK, &oldsig,
				    (sigset_t *)NULL);
				longjmp(canit_env, 1);
			    }
			}
			if (!wait || canit || mane.min_x < MAXDIM
				|| alt.min_x < MAXDIM || mag_moved) {
			    (void) sigprocmask(SIG_SETMASK, &oldsig,
				(sigset_t *) NULL);
			    return;
			}
			(void) sigsuspend(&oldsig);
			if (terminate_flag) {
#if	PS
			    ps_destroy();
#endif
			    exit(0);
			}

		    }
		    (void) sigprocmask(SIG_SETMASK, &oldsig, (sigset_t *) NULL);
		}
	    }
	    else

#endif /* not FLAKY_SIGPOLL */

	    {
		for (;;) {
#if HAVE_POLL
		    int	retval;
#endif

		    if (terminate_flag) {
#if	PS
			ps_destroy();
#endif
			exit(0);
		    }

		    if (XtPending())
			break;

		    /*
		     * The following code eliminates unnecessary calls to
		     * XDefineCursor, since this is a slow operation on some
		     * hardware (e.g., S3 chips).
		     */
		    if (busycurs && wait && !canit && !mag_moved
			    && alt.min_x == MAXDIM && mane.min_x == MAXDIM) {
			XSync(DISP, False);
			if (XtPending()) break;
/* BEGIN CHUNK events.c 11 */
#ifdef SRC_SPECIALS
			if (src_evalMode) {
			  XDefineCursor(DISP, mane.win, src_cursor);
			}
			else {
#endif
/* END CHUNK events.c 11 */
			  XDefineCursor(DISP, mane.win, ready_cursor);
/* BEGIN CHUNK events.c 12 */
#ifdef SRC_SPECIALS
			}
#endif
/* END CHUNK events.c 12 */
			XFlush(DISP);
			busycurs = False;
		    }
		    if (!wait && (canit | alt_canit)) {
#if	PS
			psp.interrupt();
#endif
			if (allow_can) longjmp(canit_env, 1);
		    }
		    if (!wait || canit
		      || mane.min_x < MAXDIM || alt.min_x < MAXDIM || mag_moved)
			return;
		    /* If a SIGUSR1 signal comes right now, then it will wait
		       until an X event or another SIGUSR1 signal arrives. */

#if HAVE_POLL
		    do {
			retval = poll(fds, XtNumber(fds), -1);
		    } while (retval < 0 && errno == EAGAIN);

		    if (retval < 0 && errno != EINTR)
			perror("poll (xdvi read_events)");
#else
		    FD_SET(ConnectionNumber(DISP), &readfds);
		    if (select(ConnectionNumber(DISP) + 1, &readfds,
			    (fd_set *) NULL, (fd_set *) NULL,
			    (struct timeval *) NULL) < 0 && errno != EINTR)
			perror("select (xdvi read_events)");
#endif
		}
	    }

#ifdef	TOOLKIT

	    XtNextEvent(&event);
	    if (resized) get_geom();
	    if (event.xany.window == alt.win && event.type == Expose) {
		handle_exp((Widget) NULL, (XtPointer) &alt, &event,
		    (Boolean *) NULL);
		continue;
	    }
	    (void) XtDispatchEvent(&event);

#else	/* not TOOLKIT */

	    XNextEvent(DISP, &event);
	    if (event.xany.window == mane.win || event.xany.window == alt.win) {
		struct WindowRec *wr = &mane;

		if (event.xany.window == alt.win) {
		    wr = &alt;
		    /* check in case we already destroyed the window */
		    if (alt_stat < 0) { /* destroy upon exposure */
			alt_stat = 0;
			handle_release();
			continue;
		    }
		    else
			alt_stat = 0;
		}
		switch (event.type) {
		case GraphicsExpose:
		case Expose:
		    expose(wr, event.xexpose.x, event.xexpose.y,
			event.xexpose.width, event.xexpose.height);
		    break;

		case MotionNotify:
#ifdef HTEX
/* APS Only using motion hints, so get the last pointer location: */
		    while (XCheckMaskEvent(DISP, PointerMotionMask, &event));
		    if ((keys_buttons&(Button1Mask|Button2Mask|Button3Mask|
					Button4Mask|Button5Mask)) == 0) {
	/* None of the buttons was down, so this isn't a mag motion event: */
			int x, y;
		    	if (!pointerlocate(&x, &y)) break;
			htex_displayanchor(current_page, x, y);
			break;
		    } 
#endif
		    new_mag_x = event.xmotion.x;
		    new_mag_y = event.xmotion.y;
		    mag_moved = (new_mag_x != mag_x || new_mag_y != mag_y);
		    break;

		case ButtonPress:
		    handle_button(&event.xbutton);
		    break;

		case ButtonRelease:
		    handle_release();
		    break;
		}	/* end switch */
	    }	/* end if window == {mane,alt}.win */

	    else if (event.xany.window == x_bar) {
		if (event.type == Expose)
		    XFillRectangle(DISP, x_bar, ruleGC,
			x_bgn, 1, x_end - x_bgn, BAR_WID);
		else if (event.type == MotionNotify)
		    scrollmane(event.xmotion.x * page_w / clip_w,
			mane.base_y);
		else switch (event.xbutton.button)
		{
		    case 1:
			scrollmane(mane.base_x + event.xbutton.x, mane.base_y);
			break;
		    case 2:
			scrollmane(event.xbutton.x * page_w / clip_w,
			    mane.base_y);
			break;
		    case 3:
			scrollmane(mane.base_x - event.xbutton.x, mane.base_y);
		}
	    }

	    else if (event.xany.window == y_bar) {
		if (event.type == Expose)
		    XFillRectangle(DISP, y_bar, ruleGC,
			1, y_bgn, BAR_WID, y_end - y_bgn);
		else if (event.type == MotionNotify)
		    scrollmane(mane.base_x,
			event.xmotion.y * page_h / clip_h);
		else switch (event.xbutton.button)
		{
		    case 1:
			scrollmane(mane.base_x, mane.base_y + event.xbutton.y);
			break;
		    case 2:
			scrollmane(mane.base_x,
			    event.xbutton.y * page_h / clip_h);
			break;
		    case 3:
			scrollmane(mane.base_x, mane.base_y - event.xbutton.y);
		}
	    }

	    else if (event.xany.window == top_level)
		switch (event.type) {
		case ConfigureNotify:
		    if (event.xany.window == top_level &&
			(event.xconfigure.width != window_w ||
			event.xconfigure.height != window_h)) {
			    Window old_mane_win = mane.win;

			    window_w = event.xconfigure.width;
			    window_h = event.xconfigure.height;
			    reconfig();
			    if (old_mane_win == (Window) 0) home(False);
		    }
		    break;

		case MapNotify:		/* if running w/o WM */
		    if (mane.win == (Window) 0) {
			reconfig();
			home(False);
		    }
		    break;

		case KeyPress:
		    handle_key(&event);
		    break;
		}

#endif	/* not TOOLKIT */

	}
}

static	void
redraw(windowrec)
	struct WindowRec *windowrec;
{

	currwin = *windowrec;
	min_x = currwin.min_x + currwin.base_x;
	min_y = currwin.min_y + currwin.base_y;
	max_x = currwin.max_x + currwin.base_x;
	max_y = currwin.max_y + currwin.base_y;
	can_exposures(windowrec);

	if (debug & DBG_EVENT)
	    Printf("Redraw %d x %d at (%d, %d) (base=%d,%d)\n", max_x - min_x,
		max_y - min_y, min_x, min_y, currwin.base_x, currwin.base_y);
	if (!busycurs) {
	    XDefineCursor(DISP, mane.win, redraw_cursor);
	    XFlush(DISP);
	    busycurs = True;
	}
	if (setjmp(dvi_env)) {
	    XClearWindow(DISP, mane.win);
	    showmessage(dvi_oops_msg);
	    if (dvi_file) {
		Fclose(dvi_file);
		dvi_file = NULL;
	    }
	}
	else {
	    draw_page();
		// fixme: kpse_pathname is sometimes corrupt when calling this function from here:
		// src_warn_outdated();
	    warn_spec_now = False;
	}
}

void
redraw_page()
{
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
}

/*
 *	Interrupt system for receiving events.  The program sets a flag
 *	whenever an event comes in, so that at the proper time (i.e., when
 *	reading a new dvi item), we can check incoming events to see if we
 *	still want to go on printing this page.  This way, one can stop
 *	displaying a page if it is about to be erased anyway.  We try to read
 *	as many events as possible before doing anything and base the next
 *	action on all events read.
 *	Note that the Xlib and Xt routines are not reentrant, so the most we
 *	can do is set a flag in the interrupt routine and check it later.
 *	Also, sometimes the interrupts are not generated (some systems only
 *	guarantee that SIGIO is generated for terminal files, and on the system
 *	I use, the interrupts are not generated if I use "(xdvi foo &)" instead
 *	of "xdvi foo").  Therefore, there is also a mechanism to check the
 *	event queue every 70 drawing operations or so.  This mechanism is
 *	disabled if it turns out that the interrupts do work.
 *	For a fuller discussion of some of the above, see xlife in
 *	comp.sources.x.
 */

static	void
can_exposures(windowrec)
	struct WindowRec *windowrec;
{
	windowrec->min_x = windowrec->min_y = MAXDIM;
	windowrec->max_x = windowrec->max_y = 0;
}

#ifndef FLAKY_SIGPOLL
/* ARGSUSED */
static	RETSIGTYPE
handle_sigpoll(signo)
	int	signo;
{
	event_counter = 1;
	event_freq = -1;	/* forget Plan B */
#if !HAVE_SIGACTION
	(void) signal(SIGPOLL, handle_sigpoll);	/* reset the signal */
#endif
}
#endif	/* not FLAKY_SIGPOLL */

/* ARGSUSED */
static	RETSIGTYPE
handle_sigusr(signo)
	int	signo;
{
	event_counter = 1;
	canit = True;
	dvi_time = 0;
#if ! HAVE_SIGACTION
	(void) signal(SIGUSR1, handle_sigusr);	/* reset the signal */
#endif
}

/* ARGSUSED */
static	RETSIGTYPE
handle_sigterm(signo)
	int	signo;
{
	terminate_flag = True;
}

static	void
enable_intr() {
#ifndef FLAKY_SIGPOLL
	int	sock_fd	= ConnectionNumber(DISP);
#endif
#if HAVE_SIGACTION
	struct sigaction a;
#endif

#ifndef FLAKY_SIGPOLL
#if HAVE_SIGACTION
	/* Subprocess handling, e.g., mktexpk, fails on the Alpha without
	   this, because SIGPOLL interrupts the call of system(3), since OSF/1
	   doesn't retry interrupted wait calls by default.  From code by
	   maj@cl.cam.ac.uk.  */
	a.sa_handler = handle_sigpoll;
	(void) sigemptyset(&a.sa_mask);
	(void) sigaddset(&a.sa_mask, SIGPOLL);
	a.sa_flags = SA_RESTART;
	sigaction(SIGPOLL, &a, NULL);
#else /* not HAVE_SIGACTION */
	(void) signal(SIGPOLL, handle_sigpoll);
#endif /* not HAVE_SIGACTION */

#if HAVE_STREAMS
	if (isastream(sock_fd) > 0) {
	    if (ioctl(sock_fd, I_SETSIG,
	      S_RDNORM | S_RDBAND | S_HANGUP | S_WRNORM) == -1)
		perror("ioctl I_SETSIG (xdvi)");
	}
	else
#endif
	{
#ifdef FASYNC
	    if (fcntl(sock_fd, F_SETOWN, getpid()) == -1)
		perror("fcntl F_SETOWN (xdvi)");
	    if (fcntl(sock_fd, F_SETFL, fcntl(sock_fd, F_GETFL, 0) | FASYNC)
	      == -1)
		perror("fcntl F_SETFL (xdvi)");
#elif defined(SIOCSPGRP) && defined(FIOASYNC)
	    /* For HP-UX B.10.10 and maybe others.  See "man 7 socket".  */
	    int arg;

	    arg = getpid();
	    if (ioctl(sock_fd, SIOCSPGRP, &arg) == -1)
		perror("ioctl SIOCSPGRP (xdvi)");
	    arg = 1;
	    if (ioctl(sock_fd, FIOASYNC, &arg) == -1)
		perror("ioctl FIOASYNC (xdvi)");
#endif
	}
#endif	/* not FLAKY_SIGPOLL */

#if HAVE_SIGACTION
	a.sa_handler = handle_sigusr;
	(void) sigemptyset(&a.sa_mask);
	(void) sigaddset(&a.sa_mask, SIGUSR1);
	a.sa_flags = 0;
	sigaction(SIGUSR1, &a, NULL);
#else /* not HAVE_SIGACTION */
	(void) signal(SIGUSR1, handle_sigusr);
#endif /* not HAVE_SIGACTION */

#if HAVE_SIGACTION
	a.sa_handler = handle_sigterm;
	(void) sigemptyset(&a.sa_mask);
	(void) sigaddset(&a.sa_mask, SIGINT);
	(void) sigaddset(&a.sa_mask, SIGQUIT);
	(void) sigaddset(&a.sa_mask, SIGTERM);
	a.sa_flags = 0;
	sigaction(SIGINT, &a, NULL);
	sigaction(SIGQUIT, &a, NULL);
	sigaction(SIGTERM, &a, NULL);
#else /* not HAVE_SIGACTION */
	(void) signal(SIGINT, handle_sigterm);
	(void) signal(SIGQUIT, handle_sigterm);
	(void) signal(SIGTERM, handle_sigterm);
#endif /* not HAVE_SIGACTION */

#ifndef FLAKY_SIGPOLL

	(void) sigemptyset(&sigpollusrterm);
	(void) sigaddset(&sigpollusrterm, SIGPOLL);
	(void) sigaddset(&sigpollusrterm, SIGUSR1);
	(void) sigaddset(&sigpollusrterm, SIGINT);
	(void) sigaddset(&sigpollusrterm, SIGQUIT);
	(void) sigaddset(&sigpollusrterm, SIGTERM);

#endif

#if HAVE_POLL
	fds[0].fd = ConnectionNumber(DISP);
#else
	FD_ZERO(&readfds);
#endif

}

void
do_pages()
{
	enable_intr();
	if (debug & DBG_BATCH) {
#ifdef	TOOLKIT
	    while (mane.min_x == MAXDIM) read_events(True);
#else	/* !TOOLKIT */
	    while (mane.min_x == MAXDIM)
		if (setjmp(canit_env)) break;
		else read_events(True);
#endif	/* TOOLKIT */
	    for (current_page = 0; current_page < total_pages; ++current_page) {
#ifdef	__convex__
		/* convex C turns off optimization for the entire function
		   if setjmp return value is discarded.*/
		if (setjmp(canit_env))	/*optimize me*/;
#else
		(void) setjmp(canit_env);
#endif
		canit = False;
		redraw_page();
	    }
	}
	else {	/* normal operation */
#ifdef	__convex__
	    /* convex C turns off optimization for the entire function
	       if setjmp return value is discarded.*/
	    if (setjmp(canit_env))	/*optimize me*/;
#else
	    (void) setjmp(canit_env);
#endif
	    for (;;) {
		read_events(True);
		if (canit) {
		    canit = False;
		    can_exposures(&mane);
		    can_exposures(&alt);
		    redraw_page();
		}
		else if (mag_moved) {
		    if (alt.win == (Window) 0) mag_moved = False;
		    else if (abs(new_mag_x - mag_x) >
			2 * abs(new_mag_y - mag_y))
			    movemag(new_mag_x, mag_y);
		    else if (abs(new_mag_y - mag_y) >
			2 * abs(new_mag_x - mag_x))
			    movemag(mag_x, new_mag_y);
		    else movemag(new_mag_x, new_mag_y);
		}
		else if (alt.min_x < MAXDIM) redraw(&alt);
		else if (mane.min_x < MAXDIM) redraw(&mane);
		XFlush(DISP);
	    }
	}
}
