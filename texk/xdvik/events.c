/*======================================================================*\

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
xdvi is based on prior work, as noted in the modification history
in xdvi.c.

\*========================================================================*/

#include "xdvi-config.h"

#ifdef STDC_HEADERS
# include <unistd.h>
# include <fcntl.h>
#endif
#include <signal.h>
#include <sys/file.h>	/* this defines FASYNC */
#ifdef HAVE_SYS_FCNTL_H
# include <sys/fcntl.h>  /* Or this might define FASYNC */
#endif
#include <sys/ioctl.h>	/* this defines SIOCSPGRP and FIOASYNC */
#include <sys/wait.h>	/* ignore HAVE_SYS_WAIT_H -- we always need WNOHANG */

#include "xdvi.h" /* this includes Xlib and Xutil are already included */
#include "xdvi-debug.h"


#ifndef MOTIF
#include <X11/Xaw/Form.h> /* for XtNresizable */
#endif

#include <setjmp.h>
#include <sys/time.h>	/* for gettimeofday */

#include <X11/IntrinsicP.h>

#include <X11/Xatom.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>	/* needed for def. of XtNiconX(??) */

#include "pagesel.h"
#include "filehist.h"
#include "special.h"
#include "psgs.h"


#include <errno.h>

#include <ctype.h>

#include "util.h"
#include "x_util.h"
#include "string-utils.h"
#include "print-dialog.h"
#include "search-dialog.h"
#include "sfSelFile.h"
#include "mag.h"
#include "help-window.h"
#include "message-window.h"
#include "dvi-draw.h"
#include "statusline.h"
#include "hypertex.h"
#include "dvi-init.h"
#include "exit-handlers.h"
#include "Tip.h"
#include "browser.h"
#include "search-internal.h"
#include "my-snprintf.h"

#include "events.h"
#include "selection.h"
#include "encodings.h"
#include "pagehist.h"
#include "xm_colorsel.h"
#include "xm_toolbar.h"
#include "xaw_menu.h"
#include "xm_menu.h"
#include "xm_prefs.h"

#include "xm_prefs_appearance.h" /* for update_preferences_expert() */
#include "xm_prefs_fonts.h"	 /* for update_preferences_color() */
#include "xm_prefs_page.h"	 /* for update_preferences_shrink() */

#ifdef	X_NOT_STDC_ENV
extern int errno;
#endif /* X_NOT_STDC_ENV */

#if HAVE_X11_INTRINSICI_H
# include <X11/IntrinsicI.h>
#else

/* Taken from <X11/TranslateI.h> in libXt-1.1.3 (June 2012) */
typedef struct _LateBindings {
    unsigned int knot:1;
    unsigned int pair:1;
    unsigned short ref_count;	/* garbage collection */
    KeySym keysym;
} LateBindings, *LateBindingsPtr;

extern Boolean _XtComputeLateBindings(
    Display*		/* dpy */,
    LateBindingsPtr	/* lateModifiers */,
    Modifiers*		/* computed */,
    Modifiers*		/* computedMask */
);

#endif /* not HAVE_X11_INTRINSICI_H */

#if HAVE_XKB_BELL_EXT
# include <X11/XKBlib.h>
# define XdviBell(display, window, percent)	\
	 XkbBell(display, window, percent, (Atom) None)
#else
# define XdviBell(display, window, percent)	XBell(display, percent)
#endif

/* Linux prefers O_ASYNC over FASYNC; SGI IRIX does the opposite.  */
#if !defined(FASYNC) && defined(O_ASYNC)
# define FASYNC	O_ASYNC
#endif

#if !defined(FLAKY_SIGPOLL) && !HAVE_STREAMS && !defined(FASYNC)
# if !defined(SIOCSPGRP) || !defined(FIOASYNC)
#  define FLAKY_SIGPOLL	1
# endif
#endif

#ifndef FLAKY_SIGPOLL

# ifndef SIGPOLL
#  define SIGPOLL	SIGIO
# endif

# ifndef SA_RESTART
#  define SA_RESTART 0
# endif

# if HAVE_STREAMS
#  include <stropts.h>

#  ifndef S_RDNORM
#   define S_RDNORM S_INPUT
#  endif

#  ifndef S_RDBAND
#   define S_RDBAND  0
#  endif

#  ifndef S_HANGUP
#   define S_HANGUP  0
#  endif

#  ifndef S_WRNORM
#   define S_WRNORM  S_OUTPUT
#  endif
# endif /* HAVE_STREAMS */

#endif /* not FLAKY_SIGPOLL */

#if HAVE_SIGACTION && !defined SA_RESETHAND
# ifdef SA_ONESHOT
#  define SA_RESETHAND SA_ONESHOT
# else
#  undef HAVE_SIGACTION         /* Needed for Mac OS X < 10.2 (9/2002) */
# endif
#endif

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
# define XIO_IN 1
# define XIO_OUT 2
#endif /* HAVE_POLL */

/* cannot be const since Strings in Action routines aren't either */
static char *Act_true_retval = "true";
static char *Act_false_retval = "false";


#if HAVE_XI21

int		xi2_opcode;
Boolean		xi2_active	= False;
struct xi2_master *xi2_masters;		/* linked list of master devs */
struct xi2_master *xi2_current;		/* current master device */
struct xi2_slave *xi2_slaves;		/* linked list of slave devs */

struct xi2_slave xi2_no_slave;		/* if no slave assigned yet */

/*
 *	XInput 2.1 creates spurious enter/leave events around the time it
 *	creates fake wheel button events.  So, we keep track of the last
 *	such fake button event, and if an enter/leave occurs too soon after
 *	that event, then we ignore the enter event.
 *	I've managed to get rid of this (by making sure that the relevant
 *	events come from the drawing window if possible), but I left the code
 *	here just in case.
 */

static	Time	xi2_ign_time;	/* time of last XI 2.1 wheel button event */

#endif
    

static sigset_t all_signals;

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

/*
 *	Signal flags
 */

/* This could be static volatile, but we want to avoid any optimizer bugs.  */
VOLATILE unsigned int sig_flags	= 0;

#define	SF_USR	1
#define	SF_ALRM	2
#define	SF_POLL	4
#define	SF_CHLD	8
#define	SF_TERM	16
#define	SF_SEGV	32

static void do_sigusr(void);
static void do_sigalrm(void);
static void do_sigpoll(void);
static void do_sigchld(void);
static void do_sigterm(void);
static void do_sigsegv(void);

/* these must be in the same order as SF_*.
   The higher flags have higher priority, since these are
   checked first when resolving flags_to_sigproc[sig_flags].

   Example: flag := SF_TERM | SF_CHLD = 24
   
   flags_to_sigproc[24] =>
   do_sigterm, which sets flag to 24 & ~SF_TERM == 8
   then, in next check:       
   flags_to_sigproc[8] =>
   do_sigchld, which sets flag 8 & ~SF_CHLD == 0.
*/
#define	SP0	do_sigusr
#define	SP1	do_sigalrm
#define	SP2	do_sigpoll
#define	SP3	do_sigchld
#define	SP4	do_sigterm
#define	SP5	do_sigsegv

typedef	void	(*signalproc)(void);

static const signalproc flags_to_sigproc[64] = {
    NULL,
    /* 1 */
    SP0,
    /* 2 - 3 */
    SP1, SP1,
    /* 4 - 7 */
    SP2, SP2, SP2, SP2,
    /* 8 - 15 */
    SP3, SP3, SP3, SP3, SP3, SP3, SP3, SP3,
    /* 16 - 31 */
    SP4, SP4, SP4, SP4, SP4, SP4, SP4, SP4,
    SP4, SP4, SP4, SP4, SP4, SP4, SP4, SP4,
    /* 32 - 63 */
    SP5, SP5, SP5, SP5, SP5, SP5, SP5, SP5,
    SP5, SP5, SP5, SP5, SP5, SP5, SP5, SP5,
    SP5, SP5, SP5, SP5, SP5, SP5, SP5, SP5,
    SP5, SP5, SP5, SP5, SP5, SP5, SP5, SP5
};

#undef	SP0
#undef	SP2
#undef	SP3
#undef	SP4
#undef	SP5

/* file-static variables for prefix argument mechanism */
static Boolean m_have_arg = False; /* flag whether we have a possible prefix arg */
static int m_number = 0;        /* prefix arg value, without the sign */
static int m_sign = 1;          /* is prefix arg negative? */


/* to remember the scrollbar positions so that we can restore them
   when `keep position' is active and window is resized to full size
   (removing scrollbars) and then back (#810501) */
static int m_x_scroll = 0, m_y_scroll = 0;

static int source_reverse_x, source_reverse_y;
static int source_show_all;

/* globals for color stuff */
Pixel plane_masks[4];
XColor color_data[2];
struct pagecolor_info page_colors = { 0, NULL };

struct rgb *color_bottom;
unsigned int color_bot_size;		/* number of entries */
struct colorframe *rcs_top;
struct rgb fg_initial;			/* Initial fg (from command line) */
struct rgb bg_initial;			/* Initial bg */
struct bgrec *bg_head = NULL;		/* head of list */
struct bgrec *bg_current = NULL;	/* current bg value */
struct fgrec *fg_current;		/* current fg value */
struct fgrec *fg_active = NULL;		/* where the GCs are */
Pixel *color_list;			/* list of colors */
unsigned int color_list_len = 0;	/* current len of list*/
unsigned int color_list_max = 0;	/* allocated size */
Boolean color_warned = False;

void null_mouse(XEvent *event)
{
    UNUSED(event);
}

mouse_proc mouse_motion = null_mouse;
mouse_proc mouse_release = null_mouse;

static void Act_digit(Widget, XEvent *, String *, Cardinal *);
static void Act_find(Widget, XEvent *, String *, Cardinal *);
static void Act_incremental_find(Widget, XEvent *, String *, Cardinal *);
static void Act_find_next(Widget, XEvent *, String *, Cardinal *);
static void Act_minus(Widget, XEvent *, String *, Cardinal *);
static void Act_quit(Widget, XEvent *, String *, Cardinal *);
static void Act_quit_confirm(Widget, XEvent *, String *, Cardinal *);
static void Act_print(Widget, XEvent *, String *, Cardinal *);
static void Act_save(Widget, XEvent *, String *, Cardinal *);
static void Act_help(Widget, XEvent *, String *, Cardinal *);
static void Act_goto_page(Widget, XEvent *, String *, Cardinal *);
static void Act_declare_page_number(Widget, XEvent *, String *, Cardinal *);
static void Act_toggle_mark(Widget, XEvent *, String *, Cardinal *);
static void Act_home(Widget, XEvent *, String *, Cardinal *);
static void Act_home_or_top(Widget, XEvent *, String *, Cardinal *);
static void Act_end_or_bottom(Widget, XEvent *, String *, Cardinal *);
static void Act_center(Widget, XEvent *, String *, Cardinal *);
static void Act_left(Widget, XEvent *, String *, Cardinal *);
static void Act_right(Widget, XEvent *, String *, Cardinal *);
static void Act_up(Widget, XEvent *, String *, Cardinal *);
static void Act_down(Widget, XEvent *, String *, Cardinal *);
static void Act_up_or_previous(Widget, XEvent *, String *, Cardinal *);
static void Act_down_or_next(Widget, XEvent *, String *, Cardinal *);
static void Act_set_margins(Widget, XEvent *, String *, Cardinal *);
static void Act_show_display_attributes(Widget, XEvent *, String *, Cardinal *);
static void Act_set_density(Widget, XEvent *, String *, Cardinal *);
static void Act_change_density(Widget, XEvent *, String *, Cardinal *);
static void Act_fullscreen(Widget, XEvent *, String *, Cardinal *);
#ifdef GREY
static void Act_set_greyscaling(Widget, XEvent *, String *, Cardinal *);
#endif
#if COLOR
static void Act_set_color(Widget, XEvent *, String *, Cardinal *);
#endif

static void Act_htex_anchorinfo(Widget, XEvent *, String *, Cardinal *);

static void Act_reread_dvi_file(Widget, XEvent *, String *, Cardinal *);
static void Act_select_dvi_file(Widget, XEvent *, String *, Cardinal *);
static void Act_discard_number(Widget, XEvent *, String *, Cardinal *);
static void Act_drag(Widget, XEvent *, String *, Cardinal *);
static void Act_wheel(Widget, XEvent *, String *, Cardinal *);
static void Act_hwheel(Widget, XEvent *, String *, Cardinal *);
static void Act_press(Widget, XEvent *, String *, Cardinal *);
static void Act_motion(Widget, XEvent *, String *, Cardinal *);
static void Act_release(Widget, XEvent *, String *, Cardinal *);
static void Act_toggle_grid_mode(Widget, XEvent *, String *, Cardinal *);
static void Act_source_special(Widget, XEvent *, String *, Cardinal *);
static void Act_show_source_specials(Widget, XEvent *, String *, Cardinal *);
static void Act_source_what_special(Widget, XEvent *, String *, Cardinal *);
static void Act_unpause_or_next(Widget, XEvent *, String *, Cardinal *);
static void Act_user_exec(Widget, XEvent *, String *, Cardinal *);
static void Act_ruler_snap_origin(Widget w, XEvent *event, String *params, Cardinal *num_params);
static void Act_load_url(Widget w, XEvent *event, String *params, Cardinal *num_params);
#ifdef MOTIF
static void Act_prefs_dialog(Widget w, XEvent *event, String *params, Cardinal *num_params);
#endif

static XtActionsRec m_actions[] = {
    {"digit", Act_digit},
    {"mouse-modes", Act_mouse_modes},
    {"switch-mode", Act_switch_mode},
    {"minus", Act_minus},
    {"recent-files", Act_recent_files},
    {"quit", Act_quit},
    {"quit-confirm", Act_quit_confirm},
    {"print", Act_print},
    {"save", Act_save},
    {"find", Act_find},
    {"incremental-find", Act_incremental_find},
    {"find-next", Act_find_next},
    {"help", Act_help},
    {"goto-page", Act_goto_page},
    {"use-tex-pages", Act_use_tex_pages},
    {"forward-page", Act_forward_page},
    {"back-page", Act_back_page},
    {"toggle-mark", Act_toggle_mark},
    {"declare-page-number", Act_declare_page_number},
    {"home", Act_home},
    {"home-or-top", Act_home_or_top},
    {"end-or-bottom", Act_end_or_bottom},
    {"center", Act_center},
    {"set-keep-flag", Act_set_keep_flag},
    {"left", Act_left},
    {"right", Act_right},
    {"up", Act_up},
    {"down", Act_down},
    {"up-or-previous", Act_up_or_previous},
    {"down-or-next", Act_down_or_next},
    {"set-margins", Act_set_margins},
    {"show-display-attributes", Act_show_display_attributes},
    {"set-shrink-factor", Act_set_shrink_factor},
    {"shrink-to-dpi", Act_shrink_to_dpi},
    {"set-density", Act_set_density},
    {"change-density", Act_change_density},
    {"fullscreen", Act_fullscreen},
#ifdef GREY
    {"set-greyscaling", Act_set_greyscaling},
#endif
#if COLOR
    {"set-color", Act_set_color},
#endif
#ifdef PS
    {"set-ps", Act_set_ps},
#endif
    {"htex-back", Act_htex_back},
    {"htex-forward", Act_htex_forward},
    {"htex-anchorinfo", Act_htex_anchorinfo},
#ifdef PS_GS
    {"set-gs-alpha", Act_set_gs_alpha},
#endif
    {"set-expert-mode", Act_set_expert_mode},
    {"reread-dvi-file", Act_reread_dvi_file},
    {"select-dvi-file", Act_select_dvi_file},
    {"discard-number", Act_discard_number},
    {"drag", Act_drag},
    {"wheel", Act_wheel},
    {"hwheel", Act_hwheel},
    {"press", Act_press},
    {"motion", Act_motion},
    {"release", Act_release},
    {"toggle-grid-mode", Act_toggle_grid_mode},
    {"source-special", Act_source_special},
    {"show-source-specials", Act_show_source_specials},
    {"source-what-special", Act_source_what_special},
    {"unpause-or-next", Act_unpause_or_next},
    {"user-exec", Act_user_exec},
#if 0 /* not implemented yet */
    {"set-papersize", Act_set_papersize},
    {"set-paper-landscape", Act_set_paper_landscape},
#endif
    {"load-url", Act_load_url},
    {"scroll-list-up", Act_scroll_list_up},
    {"scroll-list-down", Act_scroll_list_down},
    {"pagehistory-clear", Act_pagehistory_clear},
    {"pagehistory-back", Act_pagehistory_back},
    {"pagehistory-forward", Act_pagehistory_forward},
    {"pagehistory-delete-backward", Act_pagehistory_delete_backward},
    {"pagehistory-delete-forward", Act_pagehistory_delete_forward},    
#ifdef MOTIF
    {"prefs-dialog", Act_prefs_dialog},    
#endif
    {"magnifier", Act_magnifier},
    {"ruler", Act_ruler},
    {"ruler-snap-origin", Act_ruler_snap_origin},
    {"text-selection", Act_text_selection},
    {"do-href", Act_href},
    {"do-href-newwindow", Act_href_newwindow},
    {"switch-magnifier-units", Act_switch_magnifier_units},
};


Boolean
compile_action(const char *str, struct xdvi_action **app)
{
    const char *p, *p1, *p2, *end_cmd;
    XtActionsRec *actp;
    struct xdvi_action *ap;
    String *params;
    Cardinal num_params;

    for (;;) {

	while (*str == ' ' || *str == '\t')
	    ++str;

	if (*str == '\0' || *str == '\n')
	    break;

	p = str;

	/* find end of command name */
	while (isalnum((int)*p) || *p == '-' || *p == '_')
	    ++p;
	
	end_cmd = p;
	
	for (actp = m_actions; ; ++actp) {
	    if (actp >= m_actions + XtNumber(m_actions)) {
		const char *tmp = strchr(str, '\0');
		if (tmp == NULL) {
		    tmp = p;
		}
		XDVI_WARNING((stderr, "Cannot compile action \"%.*s\".",
			     (int) (tmp - str), str));
		*app = NULL;
		return False;
	    }
	    if (memcmp(str, actp->string, p - str) == 0
	      && actp->string[p - str] == '\0')
		break;
	}

	while (*p == ' ' || *p == '\t')
	    ++p;
	if (*p != '(') {
	    while (*p != '\0' && *p != '\n')
		++p;
	    XDVI_WARNING((stderr, "Syntax error in action %.*s.",
			 (int) (p - str), str));
	    *app = NULL;
	    return False;
	}

	do {++p;}
	while (*p == ' ' || *p == '\t');

	num_params = 0;
	if (*p == ')')
	    params = NULL;
	else {
	    Cardinal max_params = 4;
	    
	    params = xmalloc(max_params * sizeof (String));

	    for (;;) {	/* loop over params */
		if (*p == '"') {
		    ++p;
		    p1 = strchr(p, '"');
		    if (p1 == NULL) {
			p1 = strchr(p, '\n');
			if (p1 == NULL)
			    p1 = p + strlen(p);
			XDVI_WARNING((stderr, "Syntax error in action %.*s.",
				     (int) (p1 - str), str));
			while (num_params > 0) {
			    --num_params;
			    free(params[num_params]);
			}
			free(params);
			*app = NULL;
			return False;
		    }
		    params[num_params++] = xstrndup(p, p1 - p);

		    do {++p1;}
		    while (*p1 == ' ' || *p1 == '\t');

		    if (*p1 != ')' && *p1 != ',') {
			p2 = strchr(p1, '\n');
			if (p2 == NULL)
			    p2 = p1 + strlen(p1);
			XDVI_WARNING((stderr, "Syntax error in action %.*s.",
				     (int) (p2 - str), str));
			while (num_params > 0) {
			    --num_params;
			    free(params[num_params]);
			}
			free(params);
			*app = NULL;
			return False;
		    }
		}
		else {	/* param is not quoted */
		    for (p1 = p;; ++p1) {
			if (*p1 == '\0' || *p1 == '\n') {
			    XDVI_WARNING((stderr,
					 "Syntax error in action %.*s.",
					 (int) (p1 - str), str));
			    while (num_params > 0) {
				--num_params;
				free(params[num_params]);
			    }
			    free(params);
			    *app = NULL;
			    return False;
			}
			if (*p1 == ')' || *p1 == ',')
			    break;
		    }
		    p2 = p1;
		    while (p2 > p && (p2[-1] == ' ' || p2[-1] == '\t'))
			--p2;
		    params[num_params++] = xstrndup(p, p2 - p);
		}

		p = p1;
		if (*p == ')')
		    break;

		do {++p;}
		while (*p == ' ' || *p == '\t');

		if (num_params >= max_params) {
		    max_params *= 2;
		    params = xrealloc(params, max_params * sizeof (String));
		}
	    }
	} /* end if */

	ap = xmalloc(sizeof *ap);
	ap->proc = actp->proc;
	ap->command = xstrndup(str, end_cmd - str);
	ap->params = params;
	ap->num_params = num_params;

	*app = ap;
	app = &ap->next;

	str = p + 1;
    }

    *app = NULL;
    return True;
}

/*
 *	cached_compile_action does the same as compile_action, but retains
 *	the compiled actions.  It is used only in Act_mouse_modes(), which
 *	otherwise would recompile the same actions over and over again.
 */

struct avl_cached_action {
	AVL_COMMON;
	struct xdvi_action *ap;
};


static void
cached_compile_action(const char *str, struct xdvi_action **app)
{
	static struct avl_cached_action *avl_ca_head = NULL;
	struct avl_cached_action *avl_ca;

	avl_ca = (struct avl_cached_action *)
	  avladd(str, strlen(str), (struct avl **) &avl_ca_head,
	  sizeof (struct avl_cached_action));

	if (avl_ca->key == str) {	/* if new record */
	    avl_ca->key = xstrdup(str);	/* copy string to new storage */
	    compile_action(str, &avl_ca->ap);
	}
	*app = avl_ca->ap;
}


/*
 * Access to m_actions
 */
int get_num_actions(void)
{
    return XtNumber(m_actions);
}

XtActionsRec *get_actions(void)
{
    return m_actions;
}

/*
 *	Data for buffered events.
 */

#ifndef FLAKY_SIGPOLL
static VOLATILE int event_freq = 70;
#else
#define	event_freq	70
#endif

static void can_exposures(struct WindowRec *windowrec);


/*
 *	Set the flag so that termination occurs, via the above routine.
 *	This should be used in place of xdvi_exit() when there may be a
 *	non-killable process running (e.g., anytime within read_events()).
 */

static void
xdvi_normal_exit(void)
{
    sig_flags |= SF_TERM;
}

static void
xdvi_normal_exit_cb(XtPointer arg)
{
    UNUSED(arg);
    sig_flags |= SF_TERM;
}

void
xdvi_exit_callback(Widget w, XtPointer client_data, XtPointer call_data)
{
    UNUSED(w);
    UNUSED(client_data);
    UNUSED(call_data);

    sig_flags |= SF_TERM;
}

/*
 * Event-handling routines.
 */


void
expose(struct WindowRec *windowrec,
       int x, int y,
       unsigned int w, unsigned int h)
{
    if (windowrec->min_x > x)
	windowrec->min_x = x;
    if (windowrec->max_x < (int)(x + w))
	windowrec->max_x = x + w;
    if (windowrec->min_y > y)
	windowrec->min_y = y;
    if (windowrec->max_y < (int)(y + h))
	windowrec->max_y = y + h;

    globals.ev.flags |= EV_EXPOSE;
}

void
clearexpose(struct WindowRec *windowrec,
	    int x, int y,
	    unsigned w, unsigned h)
{
    XClearArea(DISP, windowrec->win, x, y, w, h, False);
    expose(windowrec, x, y, w, h);
}

/*
 *	Routines for X11 toolkit.
 */

static Position m_window_x, m_window_y;
static Arg arg_xy[] = {
    {XtNx, (XtArgVal) &m_window_x},
    {XtNy, (XtArgVal) &m_window_y},
};

#define	get_xy() XtGetValues(globals.widgets.draw_widget, arg_xy, XtNumber(arg_xy))


static void
warn_num_params(const char *act_name, String *params, int num_params, int max_params)
{
    if (num_params > max_params) {
	XDVI_WARNING((stderr, "Too many parameters (%d) for action \"%s\", ignoring all after \"%s\"",
		      num_params, act_name, params[max_params - 1]));
    }
}


void
handle_command(Widget widget, XtPointer client_data, XtPointer call_data)
{
    struct xdvi_action *actp;

    UNUSED(call_data);

    /* call all actions registered for this event */
    for (actp = (struct xdvi_action *)client_data; actp != NULL; actp = actp->next) {
	if (globals.debug & DBG_EVENT)
	    fprintf(stderr, "calling action with param: %s\n",
		    actp->num_params ? actp->params[0] : "(null)");
	(actp->proc)(widget, NULL, actp->params, &actp->num_params);
    }
}

#ifdef MOTIF
int
set_bar_value(Widget bar, int value, int max)
{
    XmScrollBarCallbackStruct call_data;

#ifdef TEST_SCROLLING
    fprintf(stderr, "set_bar_value: val %d, max %d\n", value, max);
#endif
    if (value > max)
	value = max;
    if (value < 0)
	value = 0;
    call_data.value = value;
    XtVaSetValues(bar, XmNvalue, value, NULL);
    XtCallCallbacks(bar, XmNvalueChangedCallback, &call_data);
    return value;
}
#endif

void
home(wide_bool scrl)
{
    if (!scrl)
	XUnmapWindow(DISP, mane.win);
# ifdef MOTIF
    {
	int value;

	if (!resource.keep_flag) {
	    value = (globals.page.w - mane.width) / 2;
	    if (value > resource.sidemargin_int / mane.shrinkfactor)
		value = resource.sidemargin_int / mane.shrinkfactor;
	    (void)set_bar_value(globals.widgets.x_bar, value, (int)(globals.page.w - mane.width));
	}

	value = (globals.page.h - mane.height) / 2;
	if (value > resource.topmargin_int / mane.shrinkfactor)
	    value = resource.topmargin_int / mane.shrinkfactor;
	(void)set_bar_value(globals.widgets.y_bar, value, (int)(globals.page.h - mane.height));
    }
# else
    get_xy();
    if (!resource.keep_flag && globals.widgets.x_bar != NULL) {
	int coord = (globals.page.w - mane.width) / 2;

	if (coord > resource.sidemargin_int / mane.shrinkfactor)
	    coord = resource.sidemargin_int / mane.shrinkfactor;
	XtCallCallbacks(globals.widgets.x_bar, XtNscrollProc, cast_int_to_XtPointer(m_window_x + coord));
    }
    if (globals.widgets.y_bar != NULL) {
	int coord = (globals.page.h - mane.height) / 2;

	if (coord > resource.topmargin_int / mane.shrinkfactor)
	    coord = resource.topmargin_int / mane.shrinkfactor;
	XtCallCallbacks(globals.widgets.y_bar, XtNscrollProc, cast_int_to_XtPointer(m_window_y + coord));
    }
# endif /* MOTIF */
    if (!scrl) {
	XMapWindow(DISP, mane.win);
	/* Wait for the server to catch up---this eliminates flicker. */
	XSync(DISP, False);
    }
#ifdef USE_PANNER
    handle_x_scroll(NULL, NULL, NULL, NULL);
    handle_y_scroll(NULL, NULL, NULL, NULL);
#endif
}

/*
 *	Same as home(), except move to the bottom of the page.
 */

static void
home_bottom(wide_bool scrl)
{
    UNUSED(scrl);
    XUnmapWindow(DISP, mane.win);
#ifdef MOTIF
    {
	int value;

	if (!resource.keep_flag) {
	    value = (globals.page.w - mane.width) / 2;
	    if (value > resource.sidemargin_int / mane.shrinkfactor)
		value = resource.sidemargin_int / mane.shrinkfactor;
	    (void)set_bar_value(globals.widgets.x_bar, value, (int)(globals.page.w - mane.width));
	}

	(void)set_bar_value(globals.widgets.y_bar, (int)(globals.page.h - mane.height), (int)(globals.page.h - mane.height));
    }
#else /* MOTIF */
    get_xy();
    if (!resource.keep_flag && globals.widgets.x_bar != NULL) {
	int coord = (globals.page.w - mane.width) / 2;

	if (coord > resource.sidemargin_int / mane.shrinkfactor)
	    coord = resource.sidemargin_int / mane.shrinkfactor;
	XtCallCallbacks(globals.widgets.x_bar, XtNscrollProc, cast_int_to_XtPointer(m_window_x + coord));
    }
    if (globals.widgets.y_bar != NULL)
	XtCallCallbacks(globals.widgets.y_bar, XtNscrollProc, cast_int_to_XtPointer(m_window_y + (globals.page.h - mane.height)));
#endif /* MOTIF */
    XMapWindow(DISP, mane.win);
    /* Wait for the server to catch up---this eliminates flicker. */
    XSync(DISP, False);

#ifdef USE_PANNER
    handle_x_scroll(NULL, NULL, NULL, NULL);
    handle_y_scroll(NULL, NULL, NULL, NULL);
#endif
}


#ifndef MOTIF
static void
handle_destroy_bar(Widget w, XtPointer client_data, XtPointer call_data)
{
    UNUSED(w);
    UNUSED(call_data);
    *(Widget *) client_data = NULL;
}
#endif

static Boolean resized = False;

static void
get_geom(void)
{
    static Dimension new_clip_w, new_clip_h;
    
    static Arg arg_wh_clip[] = {
	{XtNwidth, (XtArgVal) &new_clip_w},
	{XtNheight, (XtArgVal) &new_clip_h},
    };

    static Dimension window_w, window_h;

    static Arg arg_wh[] = {
	{XtNwidth, (XtArgVal) &window_w},
	{XtNheight, (XtArgVal) &window_h},
    };

    
    int old_clip_w;

#ifdef MOTIF
    /* event handlers for Motif scrollbars have already been added
       in create_initialize_widgets(), xdvi.c */
    XtGetValues(globals.widgets.main_window, arg_wh, XtNumber(arg_wh));
#else
    XtGetValues(globals.widgets.vport_widget, arg_wh, XtNumber(arg_wh));
    /* Note:  widgets may be destroyed but not forgotten */
    if (globals.widgets.x_bar == NULL) {
	globals.widgets.x_bar = XtNameToWidget(globals.widgets.vport_widget, "horizontal");
	if (globals.widgets.x_bar != NULL) {
	    XtAddCallback(globals.widgets.x_bar, XtNdestroyCallback, handle_destroy_bar,
			  (XtPointer)&globals.widgets.x_bar);
#ifdef USE_PANNER
	    XtAddEventHandler(globals.widgets.x_bar, ButtonMotionMask | ButtonPressMask | ButtonReleaseMask,
			      False, handle_x_scroll, NULL);
#endif
	}
    }
    if (globals.widgets.y_bar == NULL) {
	globals.widgets.y_bar = XtNameToWidget(globals.widgets.vport_widget, "vertical");
	if (globals.widgets.y_bar != NULL) {
	    XtAddCallback(globals.widgets.y_bar, XtNdestroyCallback, handle_destroy_bar,
			  (XtPointer)&globals.widgets.y_bar);
#ifdef USE_PANNER
	    XtAddEventHandler(globals.widgets.y_bar, ButtonMotionMask | ButtonPressMask | ButtonReleaseMask,
			      False, handle_y_scroll, NULL);
#endif
	}
    }
#endif
    XtGetValues(globals.widgets.clip_widget, arg_wh_clip, XtNumber(arg_wh_clip));

    old_clip_w = mane.width;

    /* we need to do this because 
       sizeof(Dimension) != sizeof(int)
    */
    mane.width = new_clip_w;
    mane.height = new_clip_h;
    if (old_clip_w == 0) {
	globals.ev.flags |= EV_NEWPAGE;
    }

    if (resource.keep_flag) {
#ifndef MOTIF
	Dimension d;
	int curr_scroll;
	if ((globals.widgets.x_bar != NULL && m_x_scroll != 0) || (globals.widgets.y_bar != NULL && m_y_scroll != 0)) {
	    get_xy();
	}
#endif
	if (globals.widgets.x_bar != NULL && m_x_scroll != 0) {
#ifdef MOTIF
	    if (m_x_scroll > 0)
		(void)set_bar_value(globals.widgets.x_bar, m_x_scroll, (int)(globals.page.w - mane.width));
#else
	    XtVaGetValues(globals.widgets.clip_widget, XtNy, &d, NULL);
	    curr_scroll = d - m_window_x;
	    if (m_x_scroll > curr_scroll) {
		TRACE_GUI((stderr, "======== diff: %d", m_x_scroll - curr_scroll));
		XtCallCallbacks(globals.widgets.x_bar, XtNscrollProc, cast_int_to_XtPointer(m_x_scroll - curr_scroll));
	    }
#endif
	}
	if (globals.widgets.y_bar != NULL && m_y_scroll != 0) {
#ifdef MOTIF
	    if (m_y_scroll > 0)
		(void)set_bar_value(globals.widgets.y_bar, m_y_scroll, (int)(globals.page.h - mane.height));
#else
	    XtVaGetValues(globals.widgets.clip_widget, XtNy, &d, NULL);
	    curr_scroll = d - m_window_y;
	    if (m_y_scroll > curr_scroll) {
		TRACE_GUI((stderr, "======== diff: %d", m_y_scroll - curr_scroll));
		XtCallCallbacks(globals.widgets.y_bar, XtNscrollProc, cast_int_to_XtPointer(m_y_scroll - curr_scroll));
	    }
#endif
	}
    }
    /*     home(False); */
    resized = False;
}

/*
 * Callback routines
 */

void
handle_resize(Widget widget, XtPointer junk, XEvent *event, Boolean *cont)
{
    UNUSED(widget);
    UNUSED(junk);
    UNUSED(event);
    UNUSED(cont);
    
    resized = True;
#ifndef MOTIF
    handle_statusline_resize();
    handle_pagelist_resize();
#endif
}

void
reconfig(void)
{
    /*      Dimension x, y; */

    if (globals.dvi_file.bak_fp == NULL)
	return;
    
#ifndef MOTIF
    XtVaSetValues(globals.widgets.vport_widget, XtNresizable, (XtArgVal)False, NULL);
#endif
    TRACE_GUI((stderr, "globals.widgets.draw_widget: w %d, h %d", globals.page.w, globals.page.h));
    XtVaSetValues(globals.widgets.draw_widget, XtNwidth, (XtArgVal)globals.page.w, XtNheight, (XtArgVal)globals.page.h, NULL);
    
#ifdef TEST_SCROLLING
    /*     XtVaSetValues(globals.widgets.draw_background, XtNwidth, (XtArgVal)globals.page.w, XtNheight, (XtArgVal)globals.page.h, NULL); */
#endif
    
#ifndef MOTIF
    handle_statusline_resize(); /* without this, statusline will disappear */
    /* following not needed? */
    /*     handle_pagelist_resize(); */
#endif
    
    get_geom();

    /*     set_windowsize(&x, &y */
    /* #ifndef MOTIF */
    /* 		   , get_panel_width() */
    /* #endif */
    /* 		   ); */
    /*     XResizeWindow(DISP, XtWindow(globals.widgets.top_level), x, y); */
    /*      reconfigure_window(False, x, y, True); */
    /*      reconfig_window(); */
}


int
check_goto_page(int pageno, Boolean insert_into_pagehist)
{
    int retval;
    if (pageno < 0) {
	xdvi_bell();
/* 	statusline_info(STATUS_SHORT, "Can't go to page %d, going to first page instead", pageno + 1); */
	retval = 0;
    }
    else if (pageno >= total_pages) {
	xdvi_bell();
/* 	statusline_info(STATUS_SHORT, */
/* 			 "Can't go to page %d, going to last page (%d) instead", */
/* 			 pageno + 1, total_pages); */
	retval = total_pages - 1;
    }
    else
	retval = pageno;

    if (insert_into_pagehist) {
	page_history_insert(retval);
    }
    return retval;
}


static int
check_goto_tex_page(int pageno)
{
    /*
      Translate from TeX page number to `real' page number if
      needed. Note that pageno is a C-style 0-based number, hence we
      add 1 for the argument of pageinfo_get_index_of_number().
    */
    int retval;
    if (resource.use_tex_pages) {
	int res = pageinfo_get_index_of_number(pageno + 1);
	if (res >= 0)
	    retval = res;
	else {
	    xdvi_bell();
	    if (pageno < 1) {
/* 		statusline_info(STATUS_SHORT, "Can't go to page %d, going to first page instead", pageno + 1); */
		retval = 0;
	    }
	    else {
		/* there is no quick way to determine the last page number in the TeX page index */
/* 		statusline_info(STATUS_SHORT, */
/* 				 "Can't go to page %d, going to last page instead", */
/* 				 pageno + 1); */
		retval = total_pages - 1;
	    }
	}
	page_history_insert(retval);
    }
    else {
	retval = check_goto_page(pageno, True);
    }
    return retval;
}

/* |||
 *	Currently the event handler does not coordinate XCopyArea requests
 *	with GraphicsExpose events.  This can lead to problems if the window
 *	is partially obscured and one, for example, drags a scrollbar.
 */

/*
 *	Actions for the translation mechanism.
 */

/* if there are global prefixes, return them in res and reset them to defaults */
static Boolean
get_prefix_arg(int *res)
{
    Boolean ret;

    *res = m_sign * m_number;
    ret = m_have_arg;
    /* reset global flags */
    m_have_arg = False;
    m_number = 0;
    m_sign = 1;
    return ret;
}

Boolean
get_int_arg(String *param, Cardinal *num_params, int *res)
{
    if (*num_params > 0) {
	*res = atoi(*param);
	return True;
    }
    else {
	if (get_prefix_arg(res)) { /* prefix argument? */
	    return True;
	}
    }
    return False;
}


Boolean
toggle_arg(int arg, String *param, Cardinal *num_params)
{
    if (*num_params > 0) {
	if (**param != 't' && (atoi(*param) != 0) == arg)
	    return False;
    }
    else {
	if (m_have_arg) {
	    int	tmparg = m_number;

	    m_have_arg = False;
	    m_number = 0;
	    m_sign = 1;

	    if ((tmparg != 0) == arg)
		return False;
	}
    }
    return True;
}

Boolean
check_resource_expert(void *val, const char *param)
{
    int j = strtol(param, (char **)NULL, 10);
    /* check if the j-1th bit is set: */
    return (*(int *)val >> (j - 1)) & 1;
}

Boolean
check_papersize(void *val, const char *param)
{
    UNUSED(val);
    UNUSED(param);
    return False; /* TODO */
}

Boolean
check_paper_landscape(void *val, const char *param)
{
    UNUSED(val);
    UNUSED(param);
    return False; /* TODO */
}

/* comparison functions for the menu setting code */
Boolean
check_toggle(void *val, const char *param)
{
    Boolean *on = val;
    if (strcmp(param, "toggle") == 0) {
	return *on;
    }
    else {
	fprintf(stderr, "TODO: check_toggle: arg |%s|, curr: %d\n", param, *(int *)val);
	return *on;
    }
}

Boolean
check_int(void *val, const char *param)
{
    int i = strtol(param, (char **)NULL, 10);
    return i == *(int *)val;
}

void
Act_ruler(Widget w, XEvent *event,
	  String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(params);
    UNUSED(num_params);

    MYTRACE((stderr, "ruler!\n"));
    
    if (dvi_file_changed()) {
	globals.ev.flags |= EV_RELOAD;
	return;
    }

#ifdef MOTIF
    /* see xm_menu.c for an explanation of this */
    if (event && pulldown_menu_active(event->xany.serial)) {
	return;
    }
#endif
    
    if (bg_current == NULL) {
	/*
	  HACK ALERT: we can arrive here after loading a new file via the file selector
	  for which not all fonts have been generated. In that case, dereferencing
	  bg_current would bomb. Try to recover by simply returning here.
	*/
	return;
    }

    show_ruler(event);
    globals.curr_mode = RULER_MODE_ACTIVE;
    globals.ev.flags |= EV_CURSOR;
    XFlush(DISP);
    show_distance_from_ruler(event, False);
}

void
Act_text_selection(Widget w, XEvent *event,
		   String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(params);
    UNUSED(num_params);

    MYTRACE((stderr, "text selection!\n"));
    
    if (dvi_file_changed()) {
	globals.ev.flags |= EV_RELOAD;
	return;
    }

#ifdef MOTIF
    /* see xm_menu.c for an explanation of this */
    if (pulldown_menu_active(event->xany.serial)) {
	return;
    }
#endif
    
    if (bg_current == NULL) {
	/*
	  HACK ALERT: we can arrive here after loading a new file via the file selector
	  for which not all fonts have been generated. In that case, dereferencing
	  bg_current would bomb. Try to recover by simply returning here.
	*/
	return;
    }

    globals.curr_mode = TEXT_MODE_ACTIVE;
    globals.ev.flags |= EV_CURSOR;
    XFlush(DISP);
    text_selection_start(event);
    text_motion(event);
}

/****************************************************************************
 * Actions specific to the handling of the magnifier
 */

void
Act_magnifier(Widget w, XEvent *event,
	      String *params, Cardinal *num_params)
{
    UNUSED(w);

    MYTRACE((stderr, "magnifier!\n"));
    
    if (dvi_file_changed()) {
	globals.ev.flags |= EV_RELOAD;
	return;
    }

#ifdef MOTIF
    /* see xm_menu.c for an explanation of this */
    if (pulldown_menu_active(event->xany.serial)) {
	return;
    }
#endif
    
    if (bg_current == NULL) {
	/*
	  HACK ALERT: we can arrive here after loading a new file via the file selector
	  for which not all fonts have been generated. In that case, dereferencing
	  bg_current would bomb. Try to recover by simply returning here.
	*/
	return;
    }

    if (event->type != ButtonPress || mouse_release != null_mouse
	|| MAGNIFIER_ACTIVE || mane.shrinkfactor == 1 || *num_params != 1) {
	XdviBell(DISP, event->xany.window, 0);
	if (mane.shrinkfactor == 1) {
	    statusline_info(STATUS_SHORT,
			     "No magnification available at shrink factor 1");
	}
	return;
    }
    
    magnifier_move(*params, event);
}

void
Act_switch_magnifier_units(Widget w, XEvent *event,
			   String *params, Cardinal *num_params)
{
    size_t k = 0;
    static char *TeX_units[] = {
	"mm", "pt", "in", "sp", "bp", "cc", "dd", "pc", "px",
    };

    UNUSED(w);
    UNUSED(event);
    UNUSED(params);
    UNUSED(num_params);
    
    for (k = 0; k < XtNumber(TeX_units); ++k)
	if (strcmp(resource.tick_units, TeX_units[k]) == 0)
	    break;
    k++;
    if (k >= XtNumber(TeX_units))
	k = 0;
    resource.tick_units = TeX_units[k];
    if (globals.curr_mode == RULER_MODE_ACTIVE) {
	show_distance_from_ruler(event, False);
    }
    else {
	statusline_info(STATUS_SHORT, "Ruler units: %s\n", resource.tick_units);
    }
}

void
Act_href(Widget w, XEvent *event,
	 String *params, Cardinal *num_params)
{
    int x, y;
    Window dummy;

    UNUSED(w);
    UNUSED(num_params);

    (void)XTranslateCoordinates(DISP, event->xkey.window, mane.win,
				event->xkey.x, event->xkey.y, &x, &y, &dummy);

    if (params) {
	if (htex_handleref(x, y, False))
	    *params = Act_true_retval;
	else
	    *params = Act_false_retval;
    }
}

void
Act_href_newwindow(Widget w, XEvent *event,
		   String *params, Cardinal *num_params)
{
    int x, y;
    Window dummy;
    
    UNUSED(w);
    UNUSED(num_params);
    
    (void)XTranslateCoordinates(DISP, event->xkey.window, mane.win,
				event->xkey.x, event->xkey.y, &x, &y, &dummy);

    if (params) {
	if (htex_handleref(x, y, True))
	    *params = Act_true_retval;
	else
	    *params = Act_false_retval;
    }
}

static void
Act_digit(Widget w, XEvent *event,
	  String *params, Cardinal *num_params)
{
    int digit;
    /* for overflow checks */
    static const int MAXINT_QUOT = INT_MAX / 10;
    static const int MAXINT_MOD = INT_MAX % 10;


    UNUSED(w);
    UNUSED(event);

    if (*num_params != 1 || (digit = **params - '0') > 9) {
	xdvi_bell();
	return;
    }
    m_have_arg = True;

    /* don't increment m_number if it would overflow */
    if (m_number < MAXINT_QUOT || (m_number == MAXINT_QUOT && digit <= MAXINT_MOD)) {
	m_number = m_number * 10 + digit;
	if (resource.expert_mode & XPRT_SHOW_STATUSLINE) /* too distracting for stdout */
	    statusline_info(STATUS_SHORT, "numerical prefix: %s%d", m_sign < 0 ? "-" : "", m_number);
    }
    else {
	xdvi_bell();
	statusline_info(STATUS_SHORT, "numerical prefix: %s%d: no larger value possible", m_sign < 0 ? "-" : "", m_number);
    }
}

static void
Act_minus(Widget w, XEvent *event,
	  String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(event);
    UNUSED(params);
    UNUSED(num_params);
    
    m_have_arg = True;
    m_sign = -m_sign;
    if (m_number > 0) {
	if (resource.expert_mode & XPRT_SHOW_STATUSLINE) /* too distracting for stdout */
	    statusline_info(STATUS_SHORT, "numerical prefix: %s%d", m_sign < 0 ? "-" : "", m_number);
    }
    else {
	if (resource.expert_mode & XPRT_SHOW_STATUSLINE) /* too distracting for stdout */
	    statusline_info(STATUS_SHORT, "numerical prefix: %s", m_sign < 0 ? "-" : "");
    }
}

static void
Act_quit(Widget w, XEvent *event,
	 String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(event);
    UNUSED(params);
    UNUSED(num_params);
    
#ifndef FLAKY_SIGPOLL
    if (globals.debug & DBG_EVENT)
	puts(event_freq < 0
	     ? "SIGPOLL is working"
	     : "no SIGPOLL signals received");
#endif
    xdvi_normal_exit();
}

static void
Act_quit_confirm(Widget w, XEvent *event,
		 String *params, Cardinal *num_params)
{
    static Widget dialog = 0;
    
    UNUSED(w);
    UNUSED(event);
    UNUSED(params);
    UNUSED(num_params);

#ifndef FLAKY_SIGPOLL
    if (globals.debug & DBG_EVENT)
	puts(event_freq < 0
	     ? "SIGPOLL is working"
	     : "no SIGPOLL signals received");
#endif

    /* already a quit dialog open? */
    if (dialog != 0) {
	/* HACK ALERT: use brute force, since tests for XtIsRealized()
	   or XtIsMapped() don't work?? Grabbing the server apparently
	   has problems with Xaw, and it's not a nice solution anyway ... */
	if (kill_message_window(dialog))
	    xdvi_bell();
    }

    dialog = choice_dialog_sized(globals.widgets.top_level,
				 MSG_QUESTION,
				 SIZE_SMALL,
				 NULL,
#ifndef MOTIF
				 "quit",
#endif
				 NULL, NULL, /* no pre_callbacks */
				 "OK", xdvi_normal_exit_cb, NULL,
				 "Cancel", NULL, NULL,
				 "Really quit xdvi?");
}

static void
Act_load_url(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(event);
    UNUSED(num_params);
    launch_browser(*params);
}

static void
Act_print(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    /* static so that we can pass them around to callbacks etc. */
    static struct save_or_print_info info;
    static struct select_pages_info pinfo = {
	0, 0, check_marked, { 0, NULL }, NO_ERROR
    };
    static struct file_info finfo = {
	NULL, NULL,
	NULL, NULL,
	NULL
    };
    static Boolean first_time = True;

    UNUSED(w);
    UNUSED(event);
    UNUSED(params);
    UNUSED(num_params);
    
    if (first_time) {
	info.shell = info.printlog = NULL;
	info.act = FILE_PRINT;
	info.pinfo = &pinfo;
	info.finfo = &finfo;
	info.callbacks = NULL;
	first_time = False;
    }
    save_or_print_callback(&info);
}

static void
Act_save(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    /* static so that we can pass them around to callbacks etc. */
    static struct save_or_print_info info;
    static struct select_pages_info pinfo = {
	0, 0, check_marked, { 0, NULL }, NO_ERROR
    };
    static struct file_info finfo = {
	NULL, NULL,
	NULL, NULL,
	NULL
    };
    static Boolean first_time = True;

    UNUSED(w);
    UNUSED(event);
    UNUSED(params);
    UNUSED(num_params);
    
    if (first_time) {
	info.shell = info.printlog = NULL;
	info.act = FILE_SAVE;
	info.pinfo = &pinfo;
	info.finfo = &finfo;
	info.callbacks = NULL;
	first_time = False;
    }
    save_or_print_callback(&info);
}

static void
Act_find(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(event);
    UNUSED(params);
    UNUSED(num_params);
    dvi_find_string(NULL, False);
}

static void
Act_incremental_find(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(event);
    UNUSED(params);
    UNUSED(num_params);
    isearch_start();
}

static void
Act_find_next(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(event);
    UNUSED(params);
    UNUSED(num_params);
    dvi_find_string(NULL, True);
}

static void
Act_help(Widget w, XEvent *event,
	 String *params, Cardinal *num_params)
{
    char *arg = NULL;

    UNUSED(w);
    UNUSED(event);
    
    if (*num_params > 0)
	arg = *params;
    
    show_help(globals.widgets.top_level, arg);
}

static home_proc home_action = NULL;

void
goto_page(int new_page, home_proc proc, Boolean force)
{
    /* SU: added clearing the window here, otherwise old window content
       will survive switching pages as long as globals.pausing.flag is active
       (i.e. when accidentally changing page instead of unpausing)
    */
    if (globals.pausing.flag) {
#if PS_GS
	gs_erase_page();
#endif
	XClearWindow(DISP, mane.win);
    }
	    
    if (globals.dvi_file.bak_fp == NULL) {
	current_page = new_page; /* so that xdvi gets the page change */
	return;
    }
    
    xdvi_assert(XDVI_VERSION_INFO, __FILE__, __LINE__,
		new_page >= 0 && new_page < total_pages,
		"%d >= 0 && %d < %d", new_page, new_page, total_pages);
    
    if (current_page != new_page || force) {
	globals.cursor.flags &= ~CURSOR_LINK; /* disable link cursor if needed */

	current_page = new_page;
	home_action = proc;
	globals.warn_spec_now = resource.warn_spec;
	/* this seems unneccessary */
	/* 	if (!resource.keep_flag) */
	/* 	    home(False); */
#if defined(MOTIF) && HAVE_XPM
	tb_check_navigation_sensitivity(current_page);
	/*  	page_history_update_toolbar_navigation(); */
#endif
	maybe_scroll_pagelist(current_page, False);

	if (globals.pausing.num_save)
	    globals.pausing.num = globals.pausing.num_save[new_page];
	else
	    globals.pausing.num = 0;
	/* Control-L (and changing the page) clears this box */
	globals.src.fwd_box_page = -1;

	globals.ev.flags |= EV_NEWPAGE;
	XFlush(DISP);       
    }
}


static void
Act_goto_page(Widget w, XEvent *event,
	      String *params, Cardinal *num_params)
{
    int arg;
    Boolean clear_statusline = False;

    UNUSED(w);
    UNUSED(event);

    warn_num_params("goto-page()", params, *num_params, 1);
    
    if (*num_params > 0) {
	if (**params == 'e') {
	    arg = total_pages - 1;
	}
	else
	    arg = atoi(*params) - globals.pageno_correct;
    }
    else {
	if (get_prefix_arg(&arg)) {
	    clear_statusline = True;
	    arg -= globals.pageno_correct;
	}
	else {
	    arg = total_pages - 1;
	}
    }
    if (arg == total_pages - 1) {
	/* with TeX numbers, total_pages - 1 might not be found in the page list,
	   so don't use check_goto_tex_page() in this case */
	goto_page(check_goto_page(arg, True), resource.keep_flag ? NULL : home, False);
    }
    else {
	goto_page(check_goto_tex_page(arg), resource.keep_flag ? NULL : home, False);
    }
    search_signal_page_changed();
    if (clear_statusline)
	statusline_clear();
    statusline_erase("Page history:");
}

void
Act_recent_files(Widget w, XEvent *event,
		 String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(event);
    UNUSED(params);
    UNUSED(num_params);
}

void
Act_use_tex_pages(Widget w, XEvent *event,
		  String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(event);

    
    if (*num_params == 0) {
	if (m_have_arg) {
	    resource.use_tex_pages = (m_number != 0);
	    m_have_arg = False;
	    m_number = 0;
	    m_sign = 1;
	}
	else {
	    resource.use_tex_pages = !resource.use_tex_pages;
	}
    }
    else
	resource.use_tex_pages = (**params == 't' ? !resource.use_tex_pages : atoi(*params));
    
    if (resource.use_tex_pages) {
	statusline_info(STATUS_SHORT, "Using TeX page numbers for \"g\", goto-page()");
    }
    else {
	statusline_info(STATUS_SHORT, "Using physical page numbers for \"g\", goto-page()");
    }

    store_preference(NULL, "useTeXPages", "%s", resource.use_tex_pages ? "True" : "False");
    
    set_menu(&resource.use_tex_pages, Act_use_tex_pages, check_toggle);

    refresh_pagelist(total_pages, current_page);
}

void
Act_forward_page(Widget w, XEvent *event,
		 String *params, Cardinal *num_params)
{
    int arg;
    Boolean clear_statusline = False;

    UNUSED(w);
    UNUSED(event);

    if (!get_int_arg(params, num_params, &arg))
	arg = 1;
    else
	clear_statusline = True;
    
    arg += current_page;

    if (arg == current_page) { /* zero argument -> redraw page */
	globals.src.fwd_box_page = -1;
	search_reset_info();
	globals.ev.flags |= EV_NEWPAGE;
	XFlush(DISP);
	statusline_info(STATUS_SHORT, "Page redrawn.");
	return;
    }
    else if (current_page >= total_pages - 1) {
	xdvi_bell();
/* 	statusline_info(STATUS_SHORT, "Last page of DVI file"); */
	return;
    }
    
    goto_page(check_goto_page(arg, True), resource.keep_flag ? NULL : home, False);
    if (clear_statusline)
	statusline_clear();
    statusline_erase("Page history:");
    search_signal_page_changed();
}

void
Act_back_page(Widget w, XEvent *event,
	      String *params, Cardinal *num_params)
{
    int arg;
    Boolean clear_statusline = False;
    
    UNUSED(w);
    UNUSED(event);

    if (!get_int_arg(params, num_params, &arg))
	arg = 1;
    else
	clear_statusline = True;
    
    arg = current_page - arg;

    if (current_page == 0) {
	xdvi_bell();
/* 	statusline_info(STATUS_SHORT, "First page of DVI file"); */
	return;
    }

    goto_page(check_goto_page(arg, True), resource.keep_flag ? NULL : home, False);
    if (clear_statusline)
	statusline_clear();
    statusline_erase("Page history:");
    search_signal_page_changed();
}

static void
Act_declare_page_number(Widget w, XEvent *event,
			String *params, Cardinal *num_params)
{
    int arg;

    UNUSED(w);
    UNUSED(event);
    
    if (globals.curr_mode == RULER_MODE_ACTIVE) {
	show_distance_from_ruler(event, True);
	return;
    }
    
    if (!get_int_arg(params, num_params, &arg)) {
	arg = 0;
    }
    globals.pageno_correct = arg - current_page;
    statusline_info(STATUS_SHORT, "Current page number set to %d", arg);
}

static void
Act_toggle_mark(Widget w, XEvent *event,
		String *params, Cardinal *num_params)
{
    int arg;

    UNUSED(w);
    UNUSED(event);

    if (globals.dvi_file.bak_fp == NULL)
	return;
    
    if (
#ifdef MOTIF
	(resource.expert_mode & XPRT_SHOW_PAGELIST) == 0
#else
	(resource.expert_mode & XPRT_SHOW_BUTTONS) == 0
#endif
	) {
	xdvi_bell();
	statusline_info(STATUS_SHORT,
			 "Expert mode: Page list not available.");
	return;
    }
    
    if (*num_params > 0) {
	arg = atoi(*params);
	if (arg < -1 || arg > 2) {
	    xdvi_bell();
	    statusline_error(STATUS_SHORT,
			     "Possible arguments: none (toggle current), "
			     "-1 (mark all), 0 (unmark all), 1 (toggle odd), 2 (toggle even)");
	}
	list_toggle_marks(arg);
    }
    else if (get_prefix_arg(&arg)) {
	list_toggle_marks(arg);
	statusline_clear();
    }
    else {
	arg = current_page;
	list_toggle_current(arg);
    }
}

static void
Act_home(Widget w, XEvent *event,
	 String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(event);
    UNUSED(params);
    UNUSED(num_params);

    home(True);
}

static void
Act_home_or_top(Widget w, XEvent *event,
		String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(event);
    UNUSED(params);
    UNUSED(num_params);
    
    if (resource.keep_flag) {
	String args[1];
	args[0] = "10"; /* should be large enough ... */
	XtCallActionProc(globals.widgets.top_level, "up", NULL, args, 1);
    }
    else {
	home(True);
    }
}

static void
Act_end_or_bottom(Widget w, XEvent *event,
		  String *params, Cardinal *num_params)
{
    String args[1];

    UNUSED(w);
    UNUSED(event);
    UNUSED(params);
    UNUSED(num_params);

    args[0] = "10"; /* should be large enough ... */
    XtCallActionProc(globals.widgets.top_level, "down", NULL, args, 1);
    
    if (!resource.keep_flag
#ifndef MOTIF
	&& globals.widgets.x_bar != NULL
#endif
	) {
	XtCallActionProc(globals.widgets.top_level, "right", NULL, args, 1);
    }
}

static void
Act_center(Widget w, XEvent *event,
	   String *params, Cardinal *num_params)
{
    int x, y;

    UNUSED(w);
    UNUSED(params);
    UNUSED(num_params);

#ifdef MOTIF
    /* The clip widget gives a more exact value. */
    x = event->xkey.x - mane.width / 2;
    y = event->xkey.y - mane.height / 2;

    x = set_bar_value(globals.widgets.x_bar, x, (int)(globals.page.w - mane.width));
    y = set_bar_value(globals.widgets.y_bar, y, (int)(globals.page.h - mane.height));
    get_xy();
    XWarpPointer(DISP, None, None, 0, 0, 0, 0, -x - m_window_x, -y - m_window_y);
#else

    if (event == NULL)
	return;	/* button actions do not provide events */

    x = event->xkey.x - mane.width / 2;
    y = event->xkey.y - mane.height / 2;
    /* The clip widget gives a more exact value. */
    if (globals.widgets.x_bar != NULL)
	XtCallCallbacks(globals.widgets.x_bar, XtNscrollProc, cast_int_to_XtPointer(x));
    if (globals.widgets.y_bar != NULL)
	XtCallCallbacks(globals.widgets.y_bar, XtNscrollProc, cast_int_to_XtPointer(y));
    XWarpPointer(DISP, None, None, 0, 0, 0, 0, -x, -y);
#endif
#ifdef USE_PANNER
    handle_x_scroll(NULL, NULL, NULL, NULL);
    handle_y_scroll(NULL, NULL, NULL, NULL);
#endif
}

void
Act_ruler_snap_origin(Widget w, XEvent *event,
		      String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(params);
    UNUSED(num_params);

    if (globals.curr_mode == RULER_MODE_ACTIVE)
	ruler_snap_origin(event);
}

#if 0
void
Act_set_papersize(Widget w, XEvent *event,
		  String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(event);
    UNUSED(num_params);

    fprintf(stderr, "set_paperformat: %s\n", *params);
    resource.paper = *params;
    set_menu((void *)resource.paper, Act_set_papersize, check_papersize);
}

void
Act_set_paper_landscape(Widget w, XEvent *event,
			String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(event);
    UNUSED(num_params);

    resource.paper_landscape = !resource.paper_landscape;
    fprintf(stderr, "set_paper_landscape: %s\n", *params);

    set_menu((char *)resource.paper,  Act_set_paper_landscape, check_paper_landscape);
}
#endif /* 0 */


void
Act_set_keep_flag(Widget w, XEvent *event,
		  String *params, Cardinal *num_params)
{
    UNUSED(event);
    UNUSED(w);
    
    if (*num_params == 0) {
	if (m_have_arg) {
	    resource.keep_flag = (m_number != 0);
	    m_have_arg = False;
	    m_number = 0;
	    m_sign = 1;
	}
	else {
	    resource.keep_flag = !resource.keep_flag;
	}
    }
    else
	resource.keep_flag = (**params == 't'
			      ? !resource.keep_flag : atoi(*params));
    if (resource.keep_flag) {
	statusline_info(STATUS_SHORT, "Keeping position when switching pages");
    }
    else {
	statusline_info(STATUS_SHORT,
			 "Not keeping position when switching pages");
    }

    store_preference(NULL, "keepPosition", "%s", resource.keep_flag ? "True" : "False");
    
    set_menu(&resource.keep_flag, Act_set_keep_flag, check_toggle);
}

static void
Act_left(Widget w, XEvent *event,
	 String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(event);

    do_autoscroll = False;
    
    warn_num_params("left()", params, *num_params, 1);

#ifdef MOTIF
    get_xy();
#ifdef TEST_SCROLLING
    fprintf(stderr, "left for %p\n", (void *)w);
    fprintf(stderr, "window x: %d, y: %d\n", m_window_x, m_window_y);
#endif

    (void)set_bar_value(globals.widgets.x_bar, (*num_params == 0 ? (-2 * (int)mane.width / 3)
						: (int)(-my_atof(*params) * mane.width)) - m_window_x,
			(int)(globals.page.w - mane.width));
#else
    if (globals.widgets.x_bar != NULL)
	XtCallCallbacks(globals.widgets.x_bar, XtNscrollProc,
			cast_int_to_XtPointer(*num_params == 0 ? (-2 * (int)mane.width / 3)
					      : (int)(-my_atof(*params) * mane.width)));
    else {
	xdvi_bell();
/* 	statusline_info(STATUS_SHORT, "Horizontal scrolling not possible"); */
    }
#endif
#ifdef USE_PANNER
    handle_x_scroll(NULL, NULL, NULL, NULL);
#endif
}

static void
Act_right(Widget w, XEvent *event,
	  String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(event);

    do_autoscroll = False;
    
    warn_num_params("right()", params, *num_params, 1);

#ifdef MOTIF
    get_xy();
#ifdef TEST_SCROLLING
    fprintf(stderr, "right for %p\n", (void *)w);
    fprintf(stderr, "window x: %d, y: %d\n", m_window_x, m_window_y);
#endif

    (void)set_bar_value(globals.widgets.x_bar, (*num_params == 0 ? (2 * (int)mane.width / 3)
						: (int)(my_atof(*params) * mane.width)) - m_window_x,
			(int)(globals.page.w - mane.width));
#else
    if (globals.widgets.x_bar != NULL)
	XtCallCallbacks(globals.widgets.x_bar, XtNscrollProc,
			cast_int_to_XtPointer(*num_params == 0 ? (2 * (int)mane.width / 3)
					      : (int)(my_atof(*params) * mane.width)));
    else {
	xdvi_bell();
/* 	statusline_info(STATUS_SHORT, "Horizontal scrolling not possible"); */
    }
#endif
#ifdef USE_PANNER
    handle_x_scroll(NULL, NULL, NULL, NULL);
#endif
}

static void
Act_up(Widget w, XEvent *event,
       String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(event);

    do_autoscroll = False;
    
    warn_num_params("up()", params, *num_params, 1);
    
#ifdef MOTIF
#ifdef TEST_SCROLLING
    fprintf(stderr, "up for %p\n", (void *)w);
#endif
    get_xy();
    (void)set_bar_value(globals.widgets.y_bar, (*num_params == 0 ? (-2 * (int)mane.height / 3)
						: (int)(-my_atof(*params) * mane.height)) - m_window_y,
			(int)(globals.page.h - mane.height));
#else
    if (globals.widgets.y_bar != NULL)
	XtCallCallbacks(globals.widgets.y_bar, XtNscrollProc,
			cast_int_to_XtPointer(*num_params == 0 ? (-2 * (int)mane.height / 3)
					      : (int)(-my_atof(*params) * mane.height)));
    else {
	xdvi_bell();
/* 	statusline_info(STATUS_SHORT, "Vertical scrolling not possible"); */
    }
#endif
#ifdef USE_PANNER
    handle_y_scroll(NULL, NULL, NULL, NULL);
#endif
}

static void
Act_down(Widget w, XEvent *event,
	 String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(event);

    do_autoscroll = False;

    warn_num_params("down()", params, *num_params, 1);

#ifdef MOTIF
#ifdef TEST_SCROLLING
    fprintf(stderr, "down for %p\n", (void *)w);
#endif
    get_xy();
    (void)set_bar_value(globals.widgets.y_bar, (*num_params == 0 ? (2 * (int)mane.height / 3)
						: (int)(my_atof(*params) * mane.height)) - m_window_y,
			(int)(globals.page.h - mane.height));
#else
    if (globals.widgets.y_bar != NULL)
	XtCallCallbacks(globals.widgets.y_bar, XtNscrollProc,
			cast_int_to_XtPointer(*num_params == 0 ? (2 * (int)mane.height / 3)
					      : (int)(my_atof(*params) * mane.height)));
    else {
	xdvi_bell();
/* 	statusline_info(STATUS_SHORT, "Vertical scrolling not possible"); */
    }
#endif
#ifdef USE_PANNER
    handle_y_scroll(NULL, NULL, NULL, NULL);
#endif
}

static void
Act_down_or_next(Widget w, XEvent *event,
		 String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(event);

    do_autoscroll = False;

    warn_num_params("down-or-next()", params, *num_params, 1);

#ifdef TEST_SCROLLING
    fprintf(stderr, "down-or-next for %p\n", (void *)w);
#endif

#ifdef MOTIF
    get_xy();
    if (m_window_y > (int)mane.height - (int)globals.page.h) {
	(void)set_bar_value(globals.widgets.y_bar, (*num_params == 0 ? (2 * (int)mane.height / 3)
						    : (int)(my_atof(*params) * mane.height)) -
			    m_window_y, (int)(globals.page.h - mane.height));
	return;
    }
#else
    if (globals.widgets.y_bar != NULL) {
	get_xy();
	if (m_window_y > (int)mane.height - (int)globals.page.h) {
	    XtCallCallbacks(globals.widgets.y_bar, XtNscrollProc,
			    cast_int_to_XtPointer(*num_params ==
						  0 ? (2 * (int)mane.height / 3)
						  : (int)(my_atof(*params) * mane.height)));
	    return;
	}
    }
#endif

    if (current_page < total_pages - 1) {
	goto_page(current_page + 1, home, False);
	search_signal_page_changed();
    }
    else {
	xdvi_bell();
/* 	statusline_info(STATUS_SHORT, "At bottom of last page"); */
    }
#ifdef USE_PANNER
    handle_y_scroll(NULL, NULL, NULL, NULL);
#endif
    statusline_erase("Page history:");
}


static void
Act_up_or_previous(Widget w, XEvent *event,
		   String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(event);

    do_autoscroll = False;

    warn_num_params("up-or-previous()", params, *num_params, 1);

#ifdef TEST_SCROLLING
    fprintf(stderr, "up-or-previous for %p\n", (void *)w);
#endif
#ifdef MOTIF
    get_xy();
    if (m_window_y < 0) {
	(void)set_bar_value(globals.widgets.y_bar,
			    (*num_params == 0 ? (-2 * (int)mane.height / 3)
			     : (int)(-my_atof(*params) * mane.height)) - m_window_y,
			    (int)(globals.page.h - mane.height));
	return;
    }
#else
    if (globals.widgets.y_bar != NULL) {
	get_xy();
	if (m_window_y < 0) {
	    XtCallCallbacks(globals.widgets.y_bar, XtNscrollProc,
			    cast_int_to_XtPointer(*num_params ==
						  0 ? (-2 * (int)mane.height / 3)
						  : (int)(-my_atof(*params) * mane.height)));
	    return;
	}
    }
#endif

    if (current_page > 0) {
	goto_page(current_page - 1, home_bottom, False);
	search_signal_page_changed();
    }
    else {
	xdvi_bell();
/* 	statusline_info(STATUS_SHORT, "At top of first page"); */
    }
    statusline_erase("Page history:");
#ifdef USE_PANNER
    handle_y_scroll(NULL, NULL, NULL, NULL);
#endif
}


static void
Act_set_margins(Widget w, XEvent *event,
		String *params, Cardinal *num_params)
{
    Window dummy;

    UNUSED(w);
    UNUSED(params);
    UNUSED(num_params);

#ifndef MOTIF
    if (event == NULL)
	return;	/* button actions do not provide events */
#endif

    (void)XTranslateCoordinates(DISP, event->xkey.window, mane.win,
				event->xkey.x, event->xkey.y, &resource.sidemargin_int, &resource.topmargin_int, &dummy);	/* throw away last argument */
    statusline_info(STATUS_SHORT, "Margins set to cursor position (%d, %d)", resource.sidemargin_int, resource.topmargin_int);
    resource.sidemargin_int *= mane.shrinkfactor;
    resource.topmargin_int *= mane.shrinkfactor;
}

static void
Act_show_display_attributes(Widget w, XEvent *event,
			    String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(event);
    UNUSED(params);
    UNUSED(num_params);

    statusline_error(STATUS_SHORT, "Unit = %d, bitord = %d, byteord = %d",
		     BitmapUnit(DISP), BitmapBitOrder(DISP),
		     ImageByteOrder(DISP));
}

int
shrink_to_fit(void)
{
    int value1;
    int value2;
    static Dimension window_w;
#ifndef MOTIF
    static Dimension window_h;
#endif
    
    static Arg arg_wh[] = {
	{XtNwidth, (XtArgVal) &window_w}
#ifndef MOTIF
	, {XtNheight, (XtArgVal) &window_h}
#endif
    };

    
#ifdef MOTIF
    XtGetValues(globals.widgets.main_window, arg_wh, XtNumber(arg_wh));
#else
    XtGetValues(globals.widgets.vport_widget, arg_wh, XtNumber(arg_wh));
#endif
    
    value1 = ROUNDUP(globals.page.unshrunk_w, window_w - 2);

#ifdef  MOTIF
    {	/* account for menubar */
	static Dimension new_h;

	/* get rid of scrollbar */
	XtVaSetValues(globals.widgets.draw_widget, XtNwidth, (XtArgVal)1, XtNheight, (XtArgVal)1, NULL);
	XtVaGetValues(globals.widgets.clip_widget, XtNheight, &new_h, NULL);
	value2 = ROUNDUP(globals.page.unshrunk_h, new_h - 2);
    }
#else
    /* FIXME: value seems too small here! */
    value2 = ROUNDUP(globals.page.unshrunk_h, window_h - 2);
#endif

    return value1 > value2 ? value1 : value2;
}

void
do_set_shrinkfactor(int arg, Boolean set_resource)
{
    static int shrink_bak = -1;
    
    /* We don't store this as preference since it may affect initial window geometry. */
    /*  store_preference(NULL, "shrinkFactor", "%d", arg); */
    
    if (globals.curr_mode == TEXT_MODE_ACTIVE) {
	text_change_region(TEXT_SEL_CLEAR, NULL);
    }
    
    mane.shrinkfactor = arg;
    if (set_resource)
	resource.shrinkfactor = mane.shrinkfactor;
    
    set_menu(&arg, Act_set_shrink_factor, check_int);
#if HAVE_XMP
    tb_set_zoom_sensitivity(arg != 1);
#endif

    if (arg != 1 && arg != shrink_bak) {
	shrink_bak = arg;
#if GREY
#if COLOR
	if (resource.use_grey)
	    fg_active = NULL;
#else 
	if (resource.use_grey)
	    init_pix();
#endif
#endif /* GREY */
	reset_fonts();
    }
    init_page();
    reconfig();

    htex_resize_page();

    /* this seems unneccessary */
    /*     if (!resource.keep_flag) */
    /* 	home(False); */

    globals.ev.flags |= EV_NEWPAGE;
    XFlush(DISP);
}


/* A 'meta' action that is bound to mouse buttons and invokes actions depending on current mouse mode */
void
Act_mouse_modes(Widget w, XEvent *event,
		String *params, Cardinal *num_params)
{
    int i;
    size_t mode_idx = 0; /* default: one action for all modes */
    struct xdvi_action *my_action = NULL, *ap = NULL;

    if (*num_params > 1) { /* different actions */
	if (resource.mouse_mode >= *num_params) {
	    size_t k;
	    XDVI_WARNING((stderr, "Only %d parameters in X resource 'mouse-modes', should be %d",
			  *num_params, resource.mouse_mode + 1));
	    for (k = 0; k < *num_params; k++) {
		XDVI_WARNING((stderr, "Param %d: `%s'", (int)(k + 1), params[k]));
	    }
	    return;
	}
	mode_idx = resource.mouse_mode;
    }

    cached_compile_action(params[mode_idx], &my_action);
    for (i = 0, ap = my_action; ap; ap = ap->next, i++) {
	String *args;
	String retval;

	TRACE_EVENTS((stderr, "Action %d for mode %lu: '%s', %d args |%s| maps to proc |%p|",
		      i, (unsigned long) mode_idx, ap->command, ap->num_params,
		      ap->num_params > 0 ? ap->params[0] : "(none)",
		      ap->proc));

	/* copy args because the action proc may modify them (see below) */
	if (ap->num_params) {
	    args = xmalloc(ap->num_params * sizeof (String));
	    memcpy(args, ap->params, ap->num_params * sizeof (String));
	}
	else args = xmalloc(sizeof (String));

	/* now call the action proc directly */
	XtCallActionProc(w, ap->command, event, args, ap->num_params);

	retval = args[0];
	free(args);
	if (retval == Act_true_retval) { /* we may compare the pointers here */
	    /* Special case: Only invoke first action, not subsequent ones;
	     * e.g. if mouse is over a link and action is Act_href or
	     * Act_href_newwindow (currently the only cases where this is used).
	     */
	    break;
	}
    }
}


void
Act_set_shrink_factor(Widget w, XEvent *event,
		      String *params, Cardinal *num_params)
{
    int arg;
    /* Set a limit to the shrink factors allowed; otherwise xdvi may
       consume a lot of memory (even run out of memory in malloc) in
       the allocation for the subpixel lookup table pixeltbl,
       dvi-draw.c. The `correct' solution would be to recognize that at a
       certain shrink, the only pixel for a character will be always
       set to `on' anyway, so that we could do without the lookup table for
       very large shrink factors ... */
    static const int SHRINK_MAX = 999;
     
    UNUSED(w);
    UNUSED(event);

    warn_num_params("set-shrink-factor()", params, *num_params, 1);

    if (*num_params > 0) {
	if (**params == 'a')
	    arg = shrink_to_fit();
	else if (**params == '-')
	    arg = mane.shrinkfactor + 1;
	else if (**params == '+')
	    arg = mane.shrinkfactor - 1;
	else
	    arg = atoi(*params);
    }
    else if (!get_prefix_arg(&arg)) {
	arg = shrink_to_fit();
    }

    if (arg <= 0) {
	xdvi_bell();
/* 	statusline_info(STATUS_SHORT, */
/* 			 "No more enlarging possible"); */
	return;
    }
    
    if (arg > SHRINK_MAX) {
	xdvi_bell();
	statusline_info(STATUS_SHORT,
			 "Shrink factor %d too large (maximum: %d)", arg, SHRINK_MAX);
	return;
    }
    
    /* SU: added clearing the window here, else old window content
       will survive changing shrink
    */
    if (globals.pausing.flag) {
#if PS_GS
	gs_erase_page();
#endif
	XClearWindow(DISP, mane.win);
    }
    
    statusline_info(STATUS_SHORT, "shrink factor: %d", arg);
#if 0
    /* Uncommented this, otherwise selecting `shrink to fit' with same value as
       current mane.shrinkfactor will clear the canvas (only fixed by changing shrink
       factor again). That bug is also present in xdvik-22.48-alpha3. */
    if (arg == mane.shrinkfactor) {
	return;
    }
#endif

    do_set_shrinkfactor(arg, True);

#if MOTIF    
    /* note: mustn't do that inside do_set_shrinkfactor(),
       or we get into an endless loop! */
    update_preferences_shrink();
#endif
}

void
Act_shrink_to_dpi(Widget w, XEvent *event,
		  String *params, Cardinal *num_params)
{
    int arg;
    
    UNUSED(w);
    UNUSED(event);

    warn_num_params("shrink-to-dpi()", params, *num_params, 1);
    
    if (!get_int_arg(params, num_params, &arg))
	arg = 0;
    
    if (arg > 0)
	arg = (double)resource.pixels_per_inch / arg + 0.5;

    if (arg <= 0) {
	xdvi_bell();
	statusline_info(STATUS_SHORT,
			 "shrink-to-dpi requires a positive argument");
	return;
    }

    /* like elsewhere */
    if (globals.pausing.flag) {
#if PS_GS
	gs_erase_page();
#endif
	XClearWindow(DISP, mane.win);
    }
#if 0
    /* Uncommented this, otherwise selecting `shrink to fit' with same value as
       current mane.shrinkfactor will clear the canvas (only fixed by changing shrink
       factor again). That bug is also present in xdvik-22.48-alpha3. */
    if (arg == mane.shrinkfactor)
	return;
#endif
    do_set_shrinkfactor(arg, True);

#if MOTIF    
    /* note: mustn't do that inside do_set_shrinkfactor(),
       or we get into an endless loop! */
    update_preferences_shrink();
#endif
}

void
do_set_density(double newgamma, Boolean force, Boolean update_resource)
{
    
    /* like elsewhere */
    if (globals.pausing.flag) {
	XClearWindow(DISP, mane.win);
    }

#ifdef GREY
    if (resource.use_grey) {
	if (newgamma == resource.gamma && !force) {
	    statusline_info(STATUS_SHORT, "density value: %.3f", newgamma);
	    return;
	}
	resource.gamma = newgamma;
	if (update_resource)
	    globals.curr_gamma = resource.gamma;
	
#if COLOR
	fg_active = NULL;
	reset_colors();
#else
	init_pix();
	if (G_visual->class != TrueColor) {
	    return;
	}
	reset_fonts();
#endif /* COLOR */
	statusline_info(STATUS_SHORT, "density value: %.3f", newgamma);
    } else
#endif /* GREY */
    {
	reset_fonts();
	if (mane.shrinkfactor == 1) {
	    statusline_info(STATUS_SHORT,
			     "set-density ignored at magnification 1");
	    return;
	}
	statusline_info(STATUS_SHORT, "density value: %.3f", newgamma);
    }

    store_preference(NULL, "gamma", "%f", resource.gamma);

    globals.ev.flags |= EV_NEWPAGE;
    XFlush(DISP);
}

static void
Act_set_density(Widget w, XEvent *event,
		String *params, Cardinal *num_params)
{
    int arg;

    UNUSED(w);
    UNUSED(event);

    warn_num_params("set-density()", params, *num_params, 1);
    
    if (!get_int_arg(params, num_params, &arg)) {
	xdvi_bell();
	return;
    }
    /*     if (arg < 0) { */
    /* 	XBell(DISP, 0); */
    /* 	statusline_info(STATUS_SHORT, */
    /* 			 "set-density requires a positive value"); */
    /* 	return; */
    /*     } */
    do_set_density(arg != 0 ? arg / 100.0 : 1.0, False, True);
#if MOTIF
    update_preferences_darkness();
#else
    store_preference(NULL, "gamma", "%f", resource.gamma);
#endif
}


static void
Act_change_density(Widget w, XEvent *event,
		   String *params, Cardinal *num_params)
{
    int arg;
    double diff;
    double oldgamma = resource.gamma;
    UNUSED(w);
    UNUSED(event);

    warn_num_params("change-density()", params, *num_params, 1);
    
    if (!get_int_arg(params, num_params, &arg)) {
	xdvi_bell();
	return;
    }
    diff = oldgamma / arg;
    if (oldgamma + diff <= 0.0)
	oldgamma = 0.0;
    else
	oldgamma += diff;
    do_set_density(oldgamma, False, True);
#if MOTIF
    update_preferences_darkness();
#endif
}

static void h_restore_scroll_position(int orig_pos,
#ifndef MOTIF
				      Dimension *get_pos,
#endif
				      Widget scrollbar)
{
    if (resource.keep_flag && orig_pos != 0 && scrollbar != NULL) {
#ifdef MOTIF
	if (!resource.fullscreen) {
	    /* only scroll back in non-fullscreen mode */
	    (void)set_bar_value(scrollbar, orig_pos, INT_MAX);
	}
#else
	int curr_pos;
	get_xy();
	XtVaGetValues(globals.widgets.clip_widget, XtNx, get_pos, NULL);
	curr_pos = -((*get_pos) - m_window_x);
	if (curr_pos - orig_pos > 0) {
	    XtCallCallbacks(scrollbar, XtNscrollProc, cast_int_to_XtPointer(curr_pos - orig_pos));
	}
#endif /* MOTIF */
    }
}

static void
Act_fullscreen(Widget w, XEvent *event,
	       String *params, Cardinal *num_params)
{
    Dimension main_win_w, main_win_h;
    static int orig_x = 0, orig_y = 0;
#ifndef MOTIF
    static Dimension get_x, get_y;
#endif
    static Dimension old_w = 0, old_h = 0;
    Dimension panel_width = 0;
#if 0
    static Dimension w_old = 0, h_old = 0;
#endif

    UNUSED(w);
    UNUSED(event);
    
    if (!toggle_arg(resource.fullscreen, params, num_params)) {
	return;
    }
    
    resource.fullscreen = !resource.fullscreen;
    
#ifndef MOTIF
    panel_width = get_panel_width();
#endif
    
    if (resource.fullscreen) {
	/* when toggling to fullscreen, save current scrollbar values
	   and window geometry */
	XtVaGetValues(globals.widgets.top_level, XtNwidth, &old_w, XtNheight, &old_h, NULL);
	/*  	fprintf(stderr, "saved geometry: %dx%d\n", old_w, old_h); */
#ifdef MOTIF
	if (globals.widgets.x_bar != NULL)
	    XtVaGetValues(globals.widgets.x_bar, XmNvalue, &orig_x, NULL);
	if (globals.widgets.y_bar != NULL)
	    XtVaGetValues(globals.widgets.y_bar, XmNvalue, &orig_y, NULL);
#else
	get_xy();
	if (globals.widgets.x_bar != NULL)
	    XtVaGetValues(globals.widgets.clip_widget, XtNx, &get_x, NULL);
	if (globals.widgets.y_bar != NULL)
	    XtVaGetValues(globals.widgets.clip_widget, XtNy, &get_y, NULL);
	orig_x = -(get_x - m_window_x);
	orig_y = -(get_y - m_window_y);
#endif /* MOTIF */
	/*  	fprintf(stderr, "Current offsets: %d, %d, %d, %d\n", orig_x, m_window_x, orig_y, m_window_y); */
    }
    if (resource.fullscreen) {
	set_windowsize(&main_win_w, &main_win_h, panel_width, 0, False);
    }
    else { /* re-use old window sizes, or initialize them if started in fullscreen mode */
	if (old_w == 0 || old_h == 0) /* started in fullscreen mode */
	    set_windowsize(&old_w, &old_h, panel_width, 0, False);
	else
	    set_windowsize(&old_w, &old_h, panel_width, 0, True);
    }

    /*      fprintf(stderr, "reconfigure window!\n"); */
    if (resource.fullscreen)
	reconfigure_window(resource.fullscreen, main_win_w, main_win_h, True);
    else {
	reconfigure_window(resource.fullscreen, old_w, old_h, True);
    }

    /* restore horizontal/vertical scroll positions */
    h_restore_scroll_position(orig_x,
#ifndef MOTIF
			      &get_x,
#endif
			      globals.widgets.x_bar);
    h_restore_scroll_position(orig_y,
#ifndef MOTIF			      
			      &get_y,
#endif
			      globals.widgets.y_bar);
    
#ifdef USE_PANNER
    handle_x_scroll(NULL, NULL, NULL, NULL);
    handle_y_scroll(NULL, NULL, NULL, NULL);
#endif
}

#ifdef GREY

static void
Act_set_greyscaling(Widget w, XEvent *event,
		    String *params, Cardinal *num_params)
{
    int arg;

    UNUSED(w);
    UNUSED(event);

    warn_num_params("set-greyscaling()", params, *num_params, 1);
    
    if (!get_int_arg(params, num_params, &arg)) { /* no arg, toggle */
	if (!toggle_arg(resource.use_grey, params, num_params)) {
	    return;
	}
	resource.use_grey = !resource.use_grey;
	if (resource.use_grey) {
	    statusline_info(STATUS_SHORT, "greyscaling on");
	}
	else {
	    statusline_info(STATUS_SHORT, "greyscaling off");
	}
	globals.ev.flags |= EV_NEWPAGE;
	XFlush(DISP);
	return;
    }

    switch (arg) {
    case 0:
	resource.use_grey = False;
	statusline_info(STATUS_SHORT, "greyscaling off");
	break;
    case 1:
	resource.use_grey = True;
	statusline_info(STATUS_SHORT, "greyscaling on");
	break;
    default:
	{
	    float newgamma = arg != 0 ? arg / 100.0 : 1.0;
	    resource.use_grey = newgamma;
	    statusline_info(STATUS_SHORT, "greyscale value: %.1f", newgamma);
	}
    }

    /* like elsewhere */
    if (globals.pausing.flag) {
	XClearWindow(DISP, mane.win);
    }
    
    if (resource.use_grey) {
	if (G_visual->class != TrueColor)
	    init_plane_masks();
#if COLOR
	fg_active = NULL;
#else
	init_pix();
#endif
    }
    reset_fonts();
    globals.ev.flags |= EV_NEWPAGE;
    XFlush(DISP);
}

#endif

#if COLOR

void
do_toggle_color(Boolean update_resource)
{
    if (resource.use_color) {
	resource.use_color = False;
	full_reset_colors();
	scanned_page_color = total_pages;
#if PS
	if (ignore_papersize_specials || scanned_page_ps <= total_pages) {
	    scanned_page = scanned_page_ps;
	}
#endif
	statusline_info(STATUS_SHORT, "color specials off");
    }
    else {
	resource.use_color = True;
	scanned_page = scanned_page_color = scanned_page_reset;
	statusline_info(STATUS_SHORT, "color specials on");
    }
    if (update_resource)
	globals.curr_use_color = resource.use_color;
    
#ifdef MOTIF
    update_preferences_color();
#endif
    globals.ev.flags |= EV_NEWPAGE;
}

static void
Act_set_color(Widget w, XEvent *event,
	      String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(event);

    if (!toggle_arg(resource.use_color, params, num_params)) {
	return;
    };
	
    /* like elsewhere */
    if (globals.pausing.flag) {
	XClearWindow(DISP, mane.win);
    }

    do_toggle_color(True);
}

#endif /* COLOR */

#if PS

void
Act_set_ps(Widget w, XEvent *event,
	   String *params, Cardinal *num_params)
{
    int arg;

    UNUSED(event);
    UNUSED(w);
    
    if (!get_int_arg(params, num_params, &arg))
	resource.postscript++;
    else
	resource.postscript = arg;

    if (resource.postscript > 2)
	resource.postscript = 0;

#ifdef PS_GS
    if (!resource.useGS) {
	if (resource.postscript > 0) {
	    popup_message(globals.widgets.top_level,
			  MSG_WARN,
			  "This version of xdvi depends on ghostscript for rendering Postscript images. "
			  "Postscript rendering cannot be activated if the option ``-noghostscript'' is used "
			  "or if the resource ``Ghostscript'' is set to false.",
			  /* message */
			  "Option ``-noghostscript'' is active; "
			  "cannot enable Postscript rendering without ghostscript.");
	}
	resource.postscript = 0;
    }
#endif
    if (resource.postscript > 0) {
	scanned_page_ps = scanned_page_ps_bak;
	if (scanned_page > scanned_page_ps)
	    scanned_page = scanned_page_ps;
	if (resource.postscript == 1) {
	    statusline_info(STATUS_SHORT, "Postscript rendering on");
	}
	else {
	    statusline_info(STATUS_SHORT, "Postscript rendering on (with bounding box)");
	}
    }
    else {
	scanned_page_ps_bak = scanned_page_ps;
	scanned_page_ps = total_pages;
#if COLOR
	if (ignore_papersize_specials || scanned_page_color <= total_pages) {
	    scanned_page = scanned_page_color;
	}
#endif
	statusline_info(STATUS_SHORT, "Postscript rendering off; displaying bounding box instead");
    }

    store_preference(NULL, "postscript", "%d", resource.postscript);
    
    psp.toggle(resource.postscript);

    set_menu(&resource.postscript, Act_set_ps, check_int);

    /* like elsewhere */
    if (globals.pausing.flag) {
	XClearWindow(DISP, mane.win);
    }
    
    globals.ev.flags |= EV_PS_TOGGLE;
    XFlush(DISP);
}

#endif /* PS */

void
Act_htex_back(Widget w, XEvent *event,
	      String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(event);
    UNUSED(params);
    UNUSED(num_params);

    htex_back();
}

void
Act_htex_forward(Widget w, XEvent *event,
		 String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(event);
    UNUSED(params);
    UNUSED(num_params);

    htex_forward();
}

static void
Act_htex_anchorinfo(Widget w, XEvent *event,
		    String *params, Cardinal *num_params)
{
    int x, y;
    Window dummy;

    UNUSED(w);
    UNUSED(params);
    UNUSED(num_params);

    (void)XTranslateCoordinates(DISP, event->xkey.window, mane.win,
				event->xkey.x, event->xkey.y, &x, &y, &dummy);
    htex_displayanchor(x, y);
}

#ifdef PS_GS
void
Act_set_gs_alpha(Widget w, XEvent *event,
		 String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(event);

    if (!toggle_arg(resource.gs_alpha, params, num_params)) {
	return;
    }
    resource.gs_alpha = !resource.gs_alpha;

    if (resource.gs_alpha) {
	statusline_info(STATUS_SHORT, "ghostscript alpha active");
    }
    else {
	statusline_info(STATUS_SHORT, "ghostscript alpha inactive");
    }

    store_preference(NULL, "gsAlpha", "%s", resource.gs_alpha ? "True" : "False");
    
    set_menu(&resource.gs_alpha, Act_set_gs_alpha, check_toggle);

    /* like elsewhere */
    if (globals.pausing.flag) {
#if PS_GS
	gs_erase_page();
#endif
	XClearWindow(DISP, mane.win);
    }
    
    globals.ev.flags |= EV_PS_TOGGLE;
    XFlush(DISP);
}
#endif

static void
Act_reread_dvi_file(Widget w, XEvent *event,
		    String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(event);
    UNUSED(params);
    UNUSED(num_params);

    /*      fprintf(stderr, "reread file!\n"); */
    globals.ev.flags |= EV_RELOAD;
}

static void
Act_discard_number(Widget w, XEvent *event,
		   String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(event);
    UNUSED(params);
    UNUSED(num_params);

    m_have_arg = False;
    m_number = 0;
    m_sign = 1;

    statusline_info(STATUS_SHORT, "numerical prefix discarded");
}

/*
 * Actions to support dragging the image.
 */

static int drag_last_x, drag_last_y;	/* last position of cursor */

static int text_last_x = -1;
static int text_last_y = -1;
static int text_last_page = -1;

static void drag_motion(XEvent *);
static void drag_release(XEvent *);

static void
text_release(XEvent * event)
{
    int ulx, uly, w, h;
    static char *text;
    int text_len = 0;
    
    UNUSED(event);

    MYTRACE((stderr, "text_release!\n"));
    
    if (mouse_motion == null_mouse)
	return;
    mouse_motion = mouse_release = null_mouse;

    w = text_last_x - drag_last_x;
    h = text_last_y - drag_last_y;

    if (w == 0 || h == 0) {
	text_last_x = text_last_y = -1;
	drag_last_x = drag_last_y = -1;
	text_last_page = -1;
	return;
    }

    if (w < 0) {
	ulx = drag_last_x + w;
	w = -w;
    }
    else
	ulx = drag_last_x;

    if (h < 0) {
	uly = drag_last_y + h;
	h = -h;
    }
    else
	uly = drag_last_y;

    free(text);
    /* this allocates text */
    text = get_text_selection(&text_len,
			      ulx * currwin.shrinkfactor,
			      uly * currwin.shrinkfactor,
			      (ulx + w) * currwin.shrinkfactor,
			      (uly + h) * currwin.shrinkfactor);
    if (text[0] == '\0') {
	/* fprintf(stdout, "Text selection empty.\n"); */
	return;
    }

    TRACE_GUI((stderr, "Selected `%s'", text));

    if (text_len > 4 * XMaxRequestSize(DISP) - 32) {
	xdvi_bell();
	statusline_error(STATUS_MEDIUM, "Selection too large (%d bytes, maximum %d bytes)",
			 text_len, 4 * XMaxRequestSize(DISP) - 32);
	return;
    }
    
    if (!set_selection(text, globals.widgets.top_level)) {
	xdvi_bell();
	statusline_error(STATUS_MEDIUM, "Could not set primary selection!");
	text_change_region(TEXT_SEL_CLEAR, NULL);
    }
}

void
text_motion(XEvent *event)
{
    MYTRACE((stderr, "motion!\n"));
    text_change_region(TEXT_SEL_MOVE, event);
}

static void
get_rectangle(XRectangle *rect, int x, int y, int last_x, int last_y)
{
    rect->width = ABS(x - last_x);
    rect->height = ABS(y - last_y);
    rect->x = (x < last_x) ? x : last_x;
    rect->y = (y < last_y) ? y : last_y;
}

static void
crop_to_window(int *x, int *y)
{
    if (*x < -currwin.base_x + 1)
	*x = -currwin.base_x + 1;
    else if (*x > -currwin.base_x + (int)ROUNDUP(pageinfo_get_page_width(current_page), currwin.shrinkfactor) + 1)
	*x = -currwin.base_x + ROUNDUP(pageinfo_get_page_width(current_page), currwin.shrinkfactor) + 1;
    
    if (*y < -currwin.base_y + 1)
	*y = -currwin.base_y + 1;
    else if (*y > -currwin.base_y + (int)ROUNDUP(pageinfo_get_page_height(current_page), currwin.shrinkfactor) + 1)
	*y = -currwin.base_y + (int)ROUNDUP(pageinfo_get_page_height(current_page), currwin.shrinkfactor) + 1;
}

void
text_change_region(textSelectionT mode, XEvent *event)
{
    static GC bboxGC = 0;
    static GC redrawGC = 0; /* needed since bboxGC is clipped to diff of old and new region */

    if (bboxGC == 0) {
	XGCValues values;
	unsigned long valuemask;

	values.function = GXinvert; /* as in search code */
	if (values.function == GXinvert) {
	    valuemask = GCFunction;
	}
	else {
	    values.foreground = WhitePixelOfScreen(SCRN) ^ BlackPixelOfScreen(SCRN);
	    /* 		fprintf(stderr, "foreground: 0x%lx, white pixel: 0x%lx, black pixel: 0x%lx\n", */
	    /* 			values.foreground, WhitePixelOfScreen(SCRN), BlackPixelOfScreen(SCRN)); */
	    valuemask = GCFunction | GCForeground;
	}
	values.line_width = 1;
	valuemask |= GCLineWidth;
	bboxGC = XCreateGC(DISP, XtWindow(globals.widgets.top_level), valuemask, &values);
	redrawGC = XCreateGC(DISP, XtWindow(globals.widgets.top_level), valuemask, &values);
    }

    MYTRACE((stderr, "mode: %d\n", mode));
    switch (mode) {
    case TEXT_SEL_MOVE:
	{
	    int x, y;
	    
	    Window dummy;

	    XRectangle redraw = { -1, -1, 0, 0 };

	    ASSERT(event != NULL, "event in text_change_region() musn't be NULL for TEXT_SEL_MOVE");

	    (void)XTranslateCoordinates(DISP, event->xkey.window, mane.win,
					event->xkey.x, event->xkey.y,
					&x, &y,
					&dummy);

	    crop_to_window(&x, &y);

	    get_rectangle(&redraw, x, y, drag_last_x, drag_last_y);

	    /*
	     * If we have an old region, we want to clip the GC to the area:
	     *
	     * (clip \cup redraw) - (clip \cap redraw)
	     *
	     * and redraw both rectangle areas; otherwise, just draw the redraw area.
	     */

	    if (text_last_x != -1 && text_last_y != -1) {

		XRectangle clip = { -1, -1, 0, 0 };
		
		Region clip_region = XCreateRegion();
		Region redraw_region = XCreateRegion();
		Region union_region = XCreateRegion();
		Region intersect_region = XCreateRegion();

		get_rectangle(&clip, text_last_x, text_last_y, drag_last_x, drag_last_y);

		XUnionRectWithRegion(&clip, clip_region, clip_region);
		XUnionRectWithRegion(&redraw, redraw_region, redraw_region);

		XUnionRegion(clip_region, redraw_region, union_region);
		XIntersectRegion(clip_region, redraw_region, intersect_region);
		
		XSubtractRegion(union_region, intersect_region, redraw_region);

		XSetRegion(DISP, bboxGC, redraw_region);

		XDestroyRegion(clip_region);
		XDestroyRegion(redraw_region); 
		XDestroyRegion(union_region); 
		XDestroyRegion(intersect_region); 
		
		XFillRectangle(DISP, mane.win, bboxGC, clip.x, clip.y, clip.width, clip.height);
	    }
	    
	    XFillRectangle(DISP, mane.win, bboxGC, redraw.x, redraw.y, redraw.width, redraw.height);

	    text_last_x = x;
	    text_last_y = y;
	    text_last_page = current_page;
	}
	break;
    case TEXT_SEL_CLEAR:
	unset_selection(globals.widgets.top_level);
    case TEXT_SEL_ERASE:
    case TEXT_SEL_REDRAW:
	if (text_last_page == current_page
	    && text_last_x != -1 && text_last_y != -1
	    && drag_last_x != -1 && drag_last_y != -1) {

	    XRectangle clear = { -1, -1, 0, 0 };

	    get_rectangle(&clear, text_last_x, text_last_y, drag_last_x, drag_last_y);
	    
	    if (mode == TEXT_SEL_CLEAR) {
		text_last_x = text_last_y = drag_last_x = drag_last_y = -1;
		text_last_page = -1;
		/* Note ZLB: the region is erased instead of inverted to avoid
		 * multiple inverting problem. An exposure is generated to
		 * make the region redrawn */
 		clearexpose(&mane, clear.x, clear.y, clear.width, clear.height);
	    }
	    else if (clip_region_to_rect(&clear)) {
		if (mode == TEXT_SEL_ERASE) {
		    /* If width or height are 0, XClearArea will clear entire window
		     * from coordinates x, y, so check for that: */
		    if (clear.width > 0 && clear.height > 0)
		    	XClearArea(DISP, mane.win, clear.x, clear.y, clear.width, clear.height, False);
		}
		else
		    XFillRectangle(DISP, mane.win, redrawGC, clear.x, clear.y, clear.width, clear.height);
	    }
	}
	break;
    }
}

void
text_selection_start(XEvent *event)
{
    int x, y;
    Window dummy;

    /* erase existing region */
    text_change_region(TEXT_SEL_CLEAR, NULL);

    (void)XTranslateCoordinates(DISP, event->xkey.window, mane.win,
				event->xkey.x, event->xkey.y,
				&x, &y,
				&dummy);	/* throw away last argument */

    crop_to_window(&x, &y);
    
    drag_last_x = x;
    drag_last_y = y;

    text_last_x = text_last_y = -1;

    MYTRACE((stderr, "selection start; mouse_release: %p; null: %p!\n", mouse_release, null_mouse));
    if (mouse_release == null_mouse) {
	MYTRACE((stderr, "init text_motion!\n"));
	mouse_motion = text_motion;
	mouse_release = text_release;
    }
/*     resource.mouse_mode_text_selection_active = True; */
}

void
Act_switch_mode(Widget w, XEvent *event,
		String *params, Cardinal *num_params)
{
/*     mouseModeT prev_mode = resource.mouse_mode; */
    int arg = -1;
/*     char *ptr; */
    const char *mode_name = NULL;
    const char *mode_description = NULL;
/*     XEvent ev; */

    /* if called the first time from main(), this is only used
     * to initialize the menu according to the mode.
     */
    Boolean initialization = (w == globals.widgets.top_level);
    
    UNUSED(event);

    if (get_int_arg(params, num_params, &arg)) {
	if (arg < MOUSE_MODE1 || arg >= MOUSE_MODE_MAX) {
	    statusline_info(STATUS_SHORT, "Argument for Act_switch_mode outside of range from %d to %d",
			     MOUSE_MODE1, MOUSE_MODE_MAX - 1);
	    resource.mouse_mode++;
	}
	else
	    resource.mouse_mode = (mouseModeT)arg;
    }
    else
	resource.mouse_mode++;

    /* check if wrapped */
    if (resource.mouse_mode >= MOUSE_MODE_MAX)
	resource.mouse_mode = MOUSE_MODE1;

    if (globals.curr_mode == RULER_MODE_ACTIVE) {
	clear_ruler();
	mouse_motion = mouse_release = null_mouse;
    }
    else if (globals.curr_mode == TEXT_MODE_ACTIVE) {
	text_change_region(TEXT_SEL_CLEAR, NULL);
	mouse_motion = mouse_release = null_mouse;
    }
    globals.curr_mode = NO_MODE_ACTIVE;
    
    switch(resource.mouse_mode) {
    case MOUSE_MODE1:
	mode_name = resource.mouse_mode1_name;
	mode_description = resource.mouse_mode1_description;
	break;
    case MOUSE_MODE2:
	mode_name = resource.mouse_mode2_name;
	mode_description = resource.mouse_mode2_description;
/* 	globals.cursor.flags |= CURSOR_TEXT; /\*XXX FIXME: don't hardcode flag here; instead, have Act_text_selection set it *\/ */
	break;
    case MOUSE_MODE3:
	mode_name = resource.mouse_mode3_name;
	mode_description = resource.mouse_mode3_description;
/* 	globals.cursor.flags |= CURSOR_RULER; */
	break;
    default: /* can't happen */
	XDVI_WARNING((stderr, "resource.mouse_mode larger than %d!", resource.mouse_mode));
	break;
    }

    globals.ev.flags |= EV_CURSOR;
    XFlush(DISP);

    if (!initialization) {
	statusline_info(STATUS_SHORT, "%s mode; %s", mode_name, mode_description);
	store_preference(NULL, "mouseMode", "%d", resource.mouse_mode);
    }
    set_menu(&resource.mouse_mode, Act_switch_mode, check_int);

    
    /*XXX FIXME: Activate mode by synthesizing a mouse event??? */
/*     synthesize_event(&ev, globals.widgets.draw_widget); */
/*     Act_mouse_modes(w, event, params, num_params); */
}

static void
Act_drag(Widget w, XEvent *event,
	 String *params, Cardinal *num_params)
{
    UNUSED(w);

    if (mouse_release != null_mouse && mouse_release != drag_release)
	return;

    if ((globals.curr_mode == TEXT_MODE_ACTIVE) && text_last_page != -1
	/* this was:
	   && (text_last_x != drag_last_x || text_last_y != drag_last_y)
	   but that isn't restrictive enough */
	) { /* dragging would mess up region drawing */
	xdvi_bell();
	statusline_info(STATUS_SHORT, "Cannot drag page when selection is active");
	return;
    }
    
    if (*num_params != 1) {
	XDVI_WARNING((stderr, "drag() requires 1 argument (got %d)", *num_params));
	return;
    }
    switch (**params) {
    case '|':
	globals.cursor.flags |= CURSOR_DRAG_V;
	break;
    case '-':
	globals.cursor.flags |= CURSOR_DRAG_H;
	break;
    case '+':
	globals.cursor.flags |= CURSOR_DRAG_A;
	break;
    default:
	XDVI_WARNING((stderr, "drag(): Valid arguments are `+', `|' or `-'"));
    }
    
    globals.ev.flags |= EV_CURSOR;
    
    if (mouse_release == null_mouse) {
	mouse_motion = drag_motion;
	mouse_release = drag_release;
	drag_last_x = event->xbutton.x_root;
	drag_last_y = event->xbutton.y_root;
    }
    else
	drag_motion(event);

    XFlush(DISP);
}


static void
drag_motion(XEvent * event)
{
#ifdef MOTIF
    get_xy();
#endif

    if (globals.cursor.flags & (CURSOR_DRAG_H | CURSOR_DRAG_A)) { /* horizontal motion */
#ifdef MOTIF
	(void)set_bar_value(globals.widgets.x_bar,
			    drag_last_x - event->xbutton.x_root - m_window_x,
			    (int)(globals.page.w - mane.width));
#else
	if (globals.widgets.x_bar != NULL) {
	    XtCallCallbacks(globals.widgets.x_bar, XtNscrollProc,
			    cast_int_to_XtPointer(drag_last_x - event->xbutton.x_root));
	}
#endif
	drag_last_x = event->xbutton.x_root;
    }

    if (globals.cursor.flags & (CURSOR_DRAG_V | CURSOR_DRAG_A)) { /* vertical motion */
#ifdef MOTIF
	(void)set_bar_value(globals.widgets.y_bar,
			    drag_last_y - event->xbutton.y_root - m_window_y,
			    (int)(globals.page.h - mane.height));
#else
	if (globals.widgets.y_bar != NULL) {
	    XtCallCallbacks(globals.widgets.y_bar, XtNscrollProc,
			    cast_int_to_XtPointer(drag_last_y - event->xbutton.y_root));
	}
#endif
	drag_last_y = event->xbutton.y_root;
    }
#ifdef USE_PANNER
    handle_x_scroll(NULL, NULL, NULL, NULL);
    handle_y_scroll(NULL, NULL, NULL, NULL);
#endif
}


static void
drag_release(XEvent * event)
{
    drag_motion(event);
    mouse_motion = mouse_release = null_mouse;

    globals.cursor.flags &= ~(CURSOR_DRAG_H | CURSOR_DRAG_V | CURSOR_DRAG_A);
    globals.ev.flags |= EV_CURSOR;
    XFlush(DISP);
}



/* Wheel mouse support.  */

static int wheel_button = -1;

static void
Act_wheel(Widget w, XEvent *event,
	  String *params, Cardinal *num_params)
{
    int dist;

    UNUSED(w);

    if (*num_params != 1) {
	XDVI_WARNING((stderr, "wheel() requires 1 argument (got %d)", *num_params));
	return;
    }
    dist = (strchr(*params, '.') == NULL) ? atoi(*params)
	: (int)(my_atof(*params) * resource.wheel_unit);
#ifdef MOTIF
    get_xy();
    set_bar_value(globals.widgets.y_bar, dist - m_window_y, (int)(globals.page.h - mane.height));
#else
    if (globals.widgets.y_bar != NULL)
	XtCallCallbacks(globals.widgets.y_bar, XtNscrollProc, cast_int_to_XtPointer(dist));
#endif

    if (event != NULL)
	wheel_button = event->xbutton.button;

#ifdef USE_PANNER
    handle_y_scroll(NULL, NULL, NULL, NULL);
#endif
}

static int wheel_h_button = -1;

static void
Act_hwheel(Widget w, XEvent *event,
	  String *params, Cardinal *num_params)
{
    int dist;

    UNUSED(w);

    if (*num_params != 1) {
	XDVI_WARNING((stderr, "wheel() requires 1 argument (got %d)", *num_params));
	return;
    }
    dist = (strchr(*params, '.') == NULL) ? atoi(*params)
	: (int)(my_atof(*params) * resource.wheel_unit);
#ifdef MOTIF
    get_xy();
    set_bar_value(globals.widgets.x_bar, dist - m_window_x,
		  (int) (globals.page.w - mane.width));
#else
    if (globals.widgets.x_bar != NULL)
	XtCallCallbacks(globals.widgets.x_bar, XtNscrollProc,
			cast_int_to_XtPointer(dist));
#endif

    if (event != NULL)
	wheel_h_button = event->xbutton.button;

#ifdef USE_PANNER
    handle_x_scroll(NULL, NULL, NULL, NULL);
#endif
}


/* Internal mouse actions.  */

/*
 * Here xdvi does its own parsing and processing of translations for mouse
 * button presses.  This is necessary because the X Toolkit does not recognize
 * <Btn6Down> or <Btn7Down> event types.
 */

#if HAVE_X11_INTRINSICI_H
# include <X11/IntrinsicI.h>
#else

/* From <X11/TranslateI.h> */
extern Boolean _XtComputeLateBindings(
    Display*		/* dpy */,
    struct _LateBindings* /* lateModifiers */,
    Modifiers*		/* computed */,
    Modifiers*		/* computedMask */
);

#endif /* HAVE_X11_INTRINSICI_H */


static void
Act_press(Widget w, XEvent *event,
	  String *params, Cardinal *num_params)
{
    struct mouse_acts *mactp;
    struct xdvi_action *actp;

#if HAVE_XI21
    if (xi2_active
      && (xi2_current->slave->btn_mask & (1 << event->xbutton.button))) {
	TRACE_EVENTS((stderr, "Ignoring button %d press (XI 2.1 active)",
	  event->xbutton.button));
	if (event->xbutton.button <= 5)
	    wheel_button = event->xbutton.button;
	else
	    wheel_h_button = event->xbutton.button;
	xi2_ign_time = event->xbutton.time;
	return;
    }
#endif

    for (mactp = mouse_actions; mactp != NULL; mactp = mactp->next) {
	if (event->xbutton.button == mactp->button || mactp->button == 0) {
	    Modifiers mask = 0;
	    Modifiers value = 0;

	    if (mactp->late_bindings == NULL
	      || _XtComputeLateBindings(DISP, mactp->late_bindings,
	      &value, &mask)) {
		mask |= mactp->mask;
		value |= mactp->value;
		if (((value ^ event->xbutton.state) & mask) == 0) {
		    for (actp = mactp->action; actp != NULL;
		      actp = actp->next)
			(actp->proc)(w, event, actp->params, &actp->num_params);
		    return;
		}
	    }
	}
    }
}


static void
Act_motion(Widget w, XEvent *event,
	   String *params, Cardinal *num_params)
{
    /* remember last position, to do this only when pointer is actually moved */
    static int old_x = -1, old_y = -1;
    int x, y;

    UNUSED(w);
    UNUSED(params);
    UNUSED(num_params);

    MYTRACE((stderr, "act_motion!\n"));

    if (globals.curr_mode == RULER_MODE_ACTIVE) {
	show_distance_from_ruler(event, False);
    }
    /* This used to be:
       (abs(x - old_x) > x_threshold || abs(y - old_y) > y_threshold))
       but that didn't work too well either. Just change it whenever user
       moves the mouse. */
    if (!MAGNIFIER_ACTIVE && !(globals.curr_mode == RULER_MODE_ACTIVE)
	&& pointerlocate(&x, &y) && (x != old_x || y != old_y)) {
	htex_displayanchor(x, y);
	old_x = x;
	old_y = y;
    }

    if ((int)(event->xbutton.button) != wheel_button
	&& ((int) (event->xbutton.button) != wheel_h_button)) {
	MYTRACE((stderr, "mouse_motion!\n"));
	mouse_motion(event);
    }
}


static void
Act_release(Widget w, XEvent *event,
	    String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(params);
    UNUSED(num_params);

    if ((int)(event->xbutton.button) == wheel_button) {
	wheel_button = -1;
	return;
    }

    if ((int)(event->xbutton.button) == wheel_h_button) {
	wheel_h_button = -1;
	return;
    }

    mouse_release(event);
}

static void
Act_toggle_grid_mode(Widget w, XEvent *event,
		     String *params, Cardinal *num_params)
{
    int arg;

    UNUSED(w);
    UNUSED(event);

    if (!get_int_arg(params, num_params, &arg)) {
	arg = -1;
    }
    switch (arg) {
    case -1:
	resource.grid_mode = !resource.grid_mode;
	if (resource.grid_mode) {
	    statusline_info(STATUS_SHORT, "Grid mode on");
	}
	else {
	    statusline_info(STATUS_SHORT, "Grid mode off");
	}
	break;
    case 1: /* fall through */
    case 2: /* fall through */
    case 3:
	resource.grid_mode = arg;
	statusline_info(STATUS_SHORT, "Grid mode %d", arg);
	break;
    default:
	xdvi_bell();
	statusline_info(STATUS_SHORT,
			 "Valid arguments for grid mode are: none (toggles), 1, 2, 3");
	return;
    }
    init_page();
    reconfig();
    globals.ev.flags |= EV_NEWPAGE;
    XFlush(DISP);
}


/* Actions for source specials.  */

void
Act_source_special(Widget w, XEvent *event,
		   String *params, Cardinal *num_params)
{
    Window dummy;

    UNUSED(w);
    UNUSED(params);
    UNUSED(num_params);

    if ((event->type == ButtonPress && mouse_release != null_mouse)
	|| MAGNIFIER_ACTIVE) {
	xdvi_bell();
	return;
    }

    source_reverse_x = event->xbutton.x;
    source_reverse_y = event->xbutton.y;
    if (event->xbutton.window != mane.win)
	(void)XTranslateCoordinates(DISP,
				    RootWindowOfScreen(SCRN), mane.win,
				    event->xbutton.x_root,
				    event->xbutton.y_root,
				    &source_reverse_x,
				    &source_reverse_y,
				    &dummy);	/* throw away last argument */

    source_reverse_x = (source_reverse_x + mane_base_x) * mane.shrinkfactor;
    source_reverse_y = (source_reverse_y + mane_base_y) * mane.shrinkfactor;

    globals.ev.flags |= EV_SRC;
}

void
Act_show_source_specials(Widget w, XEvent *event,
			 String *params, Cardinal *num_params)
{
    int arg;
    Boolean clear_statusline = False;

    UNUSED(w);

    if (!get_int_arg(params, num_params, &arg))
	arg = -1;
    else
	clear_statusline = True;

    if ((event->type == ButtonPress && mouse_release != null_mouse)
	|| MAGNIFIER_ACTIVE) {
	xdvi_bell();
	return;
    }

    if (!(globals.ev.flags & EV_SRC)) {
	source_reverse_x = -1;
	source_show_all = (arg == 1 ? True : False);

	globals.ev.flags |= EV_SRC;
    }
    if (clear_statusline)
	statusline_clear();
}

void
Act_source_what_special(Widget w, XEvent *event,
			String *params, Cardinal *num_params)
{
    int my_x, my_y;
    Window dummy;

    UNUSED(w);
    UNUSED(params);
    UNUSED(num_params);

    (void)XTranslateCoordinates(DISP, event->xkey.window, mane.win,
				event->xkey.x, event->xkey.y, &my_x, &my_y, &dummy);	/* throw away last argument */
    my_x = (my_x + mane_base_x) * mane.shrinkfactor;
    my_y = (my_y + mane_base_y) * mane.shrinkfactor;
    source_reverse_search(my_x, my_y, False);
}

static const char *user_exec_help =
"The user-exec() action lets you run a child process. "
"It takes a single string, which is tokenized on whitespace.";

void
Act_user_exec(Widget w, XEvent *event,
	      String *params, Cardinal *num_params)
{
    char **argv;
    int i;
    char *errmsg = NULL;

    if (*num_params == 0)
	errmsg = "No arguments supplied to the user-exec() action.";
    else if (*num_params > 1)
	errmsg = "Too many arguments supplied to the user-exec() action.";
    else if (setenv("XDVI_FILE", globals.dvi_name, 1) == -1)
        errmsg = strerror(errno);

    if (errmsg) {
	popup_message(globals.widgets.top_level,
		      MSG_ERR,
		      /* helptext */
		      user_exec_help,
		      /* popup */
		      errmsg);
	return;
    }

    argv = get_separated_list(params[0], " \t", True);
    fork_process(argv[0], False, NULL, NULL, NULL, 0, argv);
    for (i = 0; argv[i] != NULL; i++)
	free(argv[i]);	
    free(argv);
}

static void
select_cb(const char *filename, void *data)
{
    UNUSED(data);
    if (filename != NULL) {
	TRACE_FILES((stderr, "new filename: |%s|", filename));
		
	if (resource.filesel_open_new_window) {
	    /*  	    char *fname = expand_filename_append_dvi(filename); */
	    launch_xdvi(filename, NULL);
	}
	else {
	    /* the filename should be complete already, so don't append .dvi */
	    set_dvi_name(expand_filename(filename, USE_CWD_PATH));
	    current_page = 0; /* switch to first page */
	    close_old_filep();
	    globals.ev.flags |= EV_NEWDOC;
	    globals.ev.flags |= EV_PAGEHIST_INSERT;
	}
    }
}

static void
Act_select_dvi_file(Widget w, XEvent *event,
		    String *params, Cardinal *num_params)
{
    /* static so that we can pass its address */
    static struct filesel_callback cb = {
	NULL, NULL, "xdvik: Open File", "Open file:", "OK", "Cancel",
	NULL, "*.dvi", True, False, NULL, NULL
    };
    int arg = -1;

    UNUSED(w);
    UNUSED(event);

    if (get_int_arg(params, num_params, &arg)) { /* try item from file history */
	char *fname;
	int dummy_page;

	if (arg < 1) {
	    xdvi_bell();
	    statusline_info(STATUS_MEDIUM, "Error: File history number must be >= 1");
	}
	else if ((fname = file_history_get_elem(arg - 1, &dummy_page)) == NULL) {
	    statusline_info(STATUS_MEDIUM, "No file number %d in history (history size: %lu)",
			     arg, (unsigned long)file_history_size());
	}
	else {
	    file_history_open(fname);
	}
	return;
    }

    file_history_set_page(current_page);
    
    cb.func_ptr = select_cb;
    cb.init_path = globals.dvi_file.dirname;
    if (cb.shell == NULL)
	cb.shell = XsraSelFile(globals.widgets.top_level, &cb);
    XsraSelFilePopup(&cb);
}

/*
 * If all GUI elements have been turned on/off, make this synonymous
 * with expert mode off/on, so that next `x' keystroke does something
 * reasonable.
 */
void
update_expert_mode(void)
{
    if ((resource.expert_mode & (XPRT_SHOW_STATUSLINE | XPRT_SHOW_SCROLLBARS
#ifdef MOTIF
				 | XPRT_SHOW_PAGELIST | XPRT_SHOW_TOOLBAR | XPRT_SHOW_MENUBAR
#else
				 | XPRT_SHOW_BUTTONS
#endif
				 )) == XPRT_SHOW_ALL) {
	resource.expert = False;
    }
    else if ((resource.expert_mode & (XPRT_SHOW_STATUSLINE
#ifdef MOTIF
				      | XPRT_SHOW_SCROLLBARS | XPRT_SHOW_PAGELIST
				      | XPRT_SHOW_TOOLBAR | XPRT_SHOW_MENUBAR
#else
				      | BROKEN_RECONFIG ? 0 : XPRT_SHOW_SCROLLBARS | XPRT_SHOW_BUTTONS
#endif
				      )) == XPRT_SHOW_NONE) {
	resource.expert = True;
    }
}

void
Act_set_expert_mode(Widget w, XEvent *event,
		    String *params, Cardinal *num_params)
{
    int arg;
    Boolean clear_statusline = False;

    UNUSED(w);
    UNUSED(event);

    if (!get_int_arg(params, num_params, &arg))
	arg = -1;
    else
	clear_statusline = True;
    
    switch(arg) {
    case 1:
	resource.expert_mode ^= XPRT_SHOW_STATUSLINE;
	toggle_statusline();
	update_expert_mode();
	break;
    case 2:
#ifndef MOTIF
	/* show this warning only when not toggling global expert mode
	   (that's why it can't be inside toggle_scrollbars) */
	if (BROKEN_RECONFIG) {
	    popup_message(globals.widgets.top_level,
			  MSG_WARN,
			  NULL,
			  "Sorry - cannot toggle scrollbars with this X Version.\n"
			  "This version of XFree has a broken implementation of the viewportWidget, "
			  "which would break the layout if the scrollbars are toggled. "
			  "Versions that are known to work have a VendorRelease version below 4000 or above 4002. "
			  "You will need to update your XFree server to fix this.");
	    return;
	}
#endif
	resource.expert_mode ^= XPRT_SHOW_SCROLLBARS;
	toggle_scrollbars();
	update_expert_mode();
	break;
	
#ifdef MOTIF
    case 3:
	resource.expert_mode ^= XPRT_SHOW_PAGELIST;
	toggle_pagelist();
	update_expert_mode();
	break;
	
    case 4: /* toolbar */
	resource.expert_mode ^= XPRT_SHOW_TOOLBAR;
	toggle_toolbar();
	update_expert_mode();
	break;
	
    case 5:
	resource.expert_mode ^= XPRT_SHOW_MENUBAR;
	toggle_menubar();
	update_expert_mode();
	break;
#else
    case 3:
	/*  	fprintf(stderr, "sidebar\n"); */
	resource.expert_mode ^= XPRT_SHOW_BUTTONS;
	toggle_buttons();
	update_expert_mode();
	break;
#endif
    default:
	/* warn 'em */
	if (
#ifdef MOTIF
	    arg > 5
#else
	    arg > 3
#endif
	    ) {
	    statusline_info(STATUS_SHORT, "Number %d too large for `set-expert-mode', using 0 (= toggle) instead.",
			     arg);
	}
	/* toggle all items */
	resource.expert = !resource.expert;
	if (resource.expert)
	    resource.expert_mode = XPRT_SHOW_NONE;
	else
	    resource.expert_mode = XPRT_SHOW_ALL;
	
	toggle_statusline();
#ifndef MOTIF
	if (!BROKEN_RECONFIG)
	    toggle_scrollbars();
#else
	toggle_scrollbars();
#endif
	
#ifdef MOTIF
	toggle_pagelist();
	toggle_toolbar();
	toggle_menubar();
#else
	toggle_buttons();
#endif
    }

#ifdef MOTIF
    update_preferences_expert();
#endif

    store_preference(NULL, "expertMode", "%d", resource.expert_mode);
    
    if (clear_statusline)
	statusline_clear();
}

Boolean have_src_specials = False;
static Boolean do_update_property = False;

#ifdef USE_PANNER
void
handle_x_scroll(Widget w, XtPointer closure, XEvent *ev, Boolean *cont)
{
#if !defined(MOTIF)
    Dimension get_x = 0;
#endif

    UNUSED(w);
    UNUSED(closure);
    UNUSED(ev);
    UNUSED(cont);

    if (/* !resource.keep_flag || */ globals.widgets.x_bar == NULL)
	return;

#ifdef MOTIF
    XtVaGetValues(globals.widgets.x_bar, XmNvalue, &m_x_scroll, NULL);
#else
    get_xy();
    XtVaGetValues(globals.widgets.clip_widget, XtNx, &get_x, NULL);
    m_x_scroll = get_x - m_window_x;
    scroll_x_panner(m_x_scroll);
#endif /* MOTIF */
}

void
handle_y_scroll(Widget w, XtPointer closure, XEvent *ev, Boolean *cont)
{
#if !defined(MOTIF)
    Dimension get_y = 0;
#endif
    
    UNUSED(w);
    UNUSED(closure);
    UNUSED(ev);
    UNUSED(cont);
    
    if (/* !resource.keep_flag || */ globals.widgets.y_bar == NULL)
	return;
    
#ifdef MOTIF
    XtVaGetValues(globals.widgets.y_bar, XmNvalue, &m_y_scroll, NULL);
#else
    get_xy();
    XtVaGetValues(globals.widgets.clip_widget, XtNy, &get_y, NULL);
    m_y_scroll = get_y - m_window_y;
    scroll_y_panner(m_y_scroll);
#endif /* MOTIF */
}
#endif /* USE_PANNER */

void
handle_expose(Widget w, XtPointer closure, XEvent *ev, Boolean *cont)
{
    struct WindowRec *windowrec = (struct WindowRec *)closure;

    UNUSED(w);
    UNUSED(cont);

    if (windowrec == &magnifier) {
	if (magnifier_stat < 0) { /* destroy upon exposure */
	    magnifier_stat = 0;
	    mag_release(ev);
	    return;
	}
	else
	    magnifier_stat = 0;
    }

    do_update_property = False;

    if (have_src_specials && !MAGNIFIER_ACTIVE) {
	do_update_property = True;
    }

    expose(windowrec, (&(ev->xexpose))->x, (&(ev->xexpose))->y,
	   (unsigned int)(&(ev->xexpose))->width, (unsigned int)(&(ev->xexpose))->height);
}


void
handle_property_change(Widget w, XtPointer junk,
		       XEvent *ev, Boolean *cont)
{
    char *prop_ret;
    size_t prop_len;

    UNUSED(w);
    UNUSED(junk);
    UNUSED(cont);

    if ((&(ev->xproperty))->window != XtWindow(globals.widgets.top_level)) /* if spurious event */
	return;
    
    if ((&(ev->xproperty))->atom == atom_src_goto()) {
	/* forward search requested */
	if ((prop_len = property_get_data(XtWindow(globals.widgets.top_level), atom_src_goto(),
					  &prop_ret,
					  XGetWindowProperty)) == 0) {
	    TRACE_CLIENT((stderr, "property_get_data() failed for atom_src_goto()!"));
	    return;
	}
	TRACE_CLIENT((stderr, "got back atom_src_goto: |%s|", prop_ret));
	globals.src.fwd_string = prop_ret;
	globals.ev.flags |= EV_SRC;
    }
    else if ((&(ev->xproperty))->atom == atom_find_string()) {
	/* string search requested */
	if ((prop_len = property_get_data(XtWindow(globals.widgets.top_level), atom_find_string(),
					  &prop_ret,
					  XGetWindowProperty)) == 0) {
	    TRACE_CLIENT((stderr, "property_get_data() failed for atom_find_string()!"));
	    return;
	}
	TRACE_FIND((stderr, "got back atom_find_string: |%s|", prop_ret));
	resource.find_string = prop_ret;
	globals.ev.flags |= EV_FIND;
    }
    else if ((&(ev->xproperty))->atom == atom_reload()) {
	/* like do_sigusr(); there's no data in this case. */
	TRACE_CLIENT((stderr, "atom_reload()"));
	globals.ev.flags |= EV_RELOAD;
    }
    else if ((&(ev->xproperty))->atom == atom_newdoc()) {
	/* loading a new file */
	FILE *new_fp;
	if ((prop_len = property_get_data(XtWindow(globals.widgets.top_level), atom_newdoc(),
					  &prop_ret,
					  XGetWindowProperty)) == 0) {
	    TRACE_CLIENT((stderr, "property_get_data() returned zero length for atom_newdoc()"));
	    /* just raise it in this case */
	    XMapRaised(XtDisplay(globals.widgets.top_level), XtWindow(globals.widgets.top_level));
	    raise_message_windows();
	    return;
	}
	TRACE_CLIENT((stderr, "got back atom_newdoc: |%s|", prop_ret));
	if ((new_fp = XFOPEN(prop_ret, "r")) == NULL) {
	    popup_message(globals.widgets.top_level,
			  MSG_ERR, NULL, "Loading %s failed: %s",
			  prop_ret, strerror(errno));
	    return;
	}
	set_dvi_name_expand(prop_ret);

	globals.ev.flags |= EV_NEWDOC;
    }
    else if ((&(ev->xproperty))->atom == atom_newpage()) {
	/* jumping to a different page */
	int newpage;
	char *testptr;
	if ((prop_len = property_get_data(XtWindow(globals.widgets.top_level), atom_newpage(),
					  &prop_ret,
					  XGetWindowProperty)) == 0) {
	    TRACE_CLIENT((stderr, "property_get_data() failed for atom_newpage(): |%s|", prop_ret));
	    return;
	}
	TRACE_CLIENT((stderr, "got back atom_newpage: |%s|", prop_ret));
	if (strcmp(prop_ret, "+") == 0) { /* special case: treat `+' as last page */
	    newpage = total_pages - 1;
	}
	else {
	    newpage = strtol(prop_ret, &testptr, 10) - 1;
	    if (*testptr != '\0') {
		XDVI_FATAL((stderr, "Invalid page number: `%s'.", prop_ret));
	    }
	}

	if (newpage == total_pages - 1) { /* as in Act_goto_page() */
	    goto_page(check_goto_page(newpage, True), resource.keep_flag ? NULL : home, False);
	    search_signal_page_changed();
	}
	else {
	    goto_page(check_goto_tex_page(newpage), resource.keep_flag ? NULL : home, False);
	    search_signal_page_changed();
	}
    }
    else if ((&(ev->xproperty))->atom == atom_raise()) {
	XMapRaised(XtDisplay(globals.widgets.top_level), XtWindow(globals.widgets.top_level));
	raise_message_windows();
    }
    else if ((&(ev->xproperty))->atom == atom_reread_prefs()) {
	read_user_preferences(globals.widgets.top_level, ".xdvirc.tmp");
    }
}


/*
 *	Signal routines.  At the signal level, all we do is set flags.
 */

#ifndef FLAKY_SIGPOLL
static RETSIGTYPE
handle_sigpoll(int signo)
{
    UNUSED(signo);
    
    globals.ev.ctr = 1;
    event_freq = -1;	/* forget Plan B */
    sig_flags |= SF_POLL;
# ifndef HAVE_SIGACTION
    (void) signal(SIGPOLL, handle_sigpoll);	/* reset the signal */
# endif
}
#endif

static RETSIGTYPE
handle_sigterm(int signo)
{
    UNUSED(signo);
    
    sig_flags |= SF_TERM;
}

static RETSIGTYPE
handle_sigchld(int signo)
{
    UNUSED(signo);

    sig_flags |= SF_CHLD;
}

static RETSIGTYPE
handle_sigalrm(int signo)
{
    UNUSED(signo);

    sig_flags |= SF_ALRM;
}

static RETSIGTYPE
ignore_sigusr(int signo)
{
    UNUSED(signo);
    fprintf(stderr, "IGNORE SIGUSR1!\n");
}

static RETSIGTYPE
handle_sigusr(int signo)
{
    UNUSED(signo);
    
    globals.ev.ctr = 1;
    sig_flags |= SF_USR;
}

static RETSIGTYPE
handle_sigsegv(int signo)
{
    UNUSED(signo);
    
    XDVI_ABORT((stderr, "Segmentation fault - trying to clean up and aborting ..."));
}

static Boolean sigalarm_initialized = False;

void
setup_sigalarm(void)
{
#if HAVE_SIGACTION
    struct sigaction a;

    a.sa_handler = handle_sigalrm;
    (void) sigemptyset(&a.sa_mask);
    (void) sigaddset(&a.sa_mask, SIGALRM);
    a.sa_flags = 0;
    sigaction(SIGALRM, &a, NULL);
#else /* not HAVE_SIGACTION */
    (void) signal(SIGALRM, handle_sigalrm);
#endif /* not HAVE_SIGACTION */

    sigalarm_initialized = True;
}

/*
 * Called from main to set up the signal handlers.
 */
void
setup_signal_handlers(Boolean early)
{
#ifndef FLAKY_SIGPOLL
    int	sock_fd	= 0;
#endif
#if HAVE_SIGACTION
    struct sigaction a;
#endif

    if (early) {
#if HAVE_SIGACTION
	a.sa_handler = ignore_sigusr;
	(void) sigemptyset(&a.sa_mask);
	(void) sigaddset(&a.sa_mask, SIGUSR1);
	a.sa_flags = 0;
	sigaction(SIGUSR1, &a, NULL);
#else /* not HAVE_SIGACTION */
	(void) signal(SIGUSR1, ignore_sigusr);
#endif /* not HAVE_SIGACTION */

	(void)sigemptyset(&all_signals);
	(void)sigaddset(&all_signals, SIGUSR1);
	return;
    }

    
#if HAVE_SIGACTION
    a.sa_handler = handle_sigusr;
    (void) sigemptyset(&a.sa_mask);
    (void) sigaddset(&a.sa_mask, SIGUSR1);
    a.sa_flags = 0;
    sigaction(SIGUSR1, &a, NULL);
#else /* not HAVE_SIGACTION */
    (void) signal(SIGUSR1, handle_sigusr);
#endif /* not HAVE_SIGACTION */

#ifndef FLAKY_SIGPOLL
#if HAVE_SIGACTION
    /* Subprocess handling, e.g., MakeTeXPK, fails on the Alpha without
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

    sock_fd = ConnectionNumber(DISP);
    prep_fd(sock_fd, False);
#endif	/* not FLAKY_SIGPOLL */

#if HAVE_SIGACTION
    a.sa_handler = handle_sigterm;
    (void) sigemptyset(&a.sa_mask);
    (void) sigaddset(&a.sa_mask, SIGINT);
    (void) sigaddset(&a.sa_mask, SIGQUIT);
    (void) sigaddset(&a.sa_mask, SIGTERM);
    (void) sigaddset(&a.sa_mask, SIGHUP);
    a.sa_flags = SA_RESETHAND;
    sigaction(SIGINT, &a, NULL);
    sigaction(SIGQUIT, &a, NULL);
    sigaction(SIGTERM, &a, NULL);
    sigaction(SIGHUP, &a, NULL);
    a.sa_handler = handle_sigsegv;
    (void)sigemptyset(&a.sa_mask);
    (void)sigaddset(&a.sa_mask, SIGSEGV);
    a.sa_flags = 0;
    sigaction(SIGSEGV, &a, NULL);
#else /* not HAVE_SIGACTION */
    (void) signal(SIGINT, handle_sigterm);
    (void) signal(SIGQUIT, handle_sigterm);
    (void) signal(SIGTERM, handle_sigterm);
    (void) signal(SIGHUP, handle_sigterm);
    (void)signal(SIGSEGV, handle_sigsegv);
#endif /* not HAVE_SIGACTION */

#if HAVE_SIGACTION
    a.sa_handler = handle_sigchld;
    (void) sigemptyset(&a.sa_mask);
    (void) sigaddset(&a.sa_mask, SIGCHLD);
    a.sa_flags = 0;
    sigaction(SIGCHLD, &a, NULL);
#else /* not HAVE_SIGACTION */
    (void) signal(SIGCHLD, handle_sigchld);
#endif /* not HAVE_SIGACTION */

    (void)sigemptyset(&all_signals);
    (void)sigaddset(&all_signals, SIGPOLL);
    (void)sigaddset(&all_signals, SIGINT);
    (void)sigaddset(&all_signals, SIGQUIT);
    (void)sigaddset(&all_signals, SIGTERM);
    (void)sigaddset(&all_signals, SIGHUP);
    (void)sigaddset(&all_signals, SIGCHLD);
    (void)sigaddset(&all_signals, SIGALRM);
    (void)sigaddset(&all_signals, SIGUSR1);
    (void)sigaddset(&all_signals, SIGSEGV);

}


/*
 *	Mid-level signal handlers.  These are called from within read_events(),
 *	and do the actual work appropriate for the given signal.
 */

/*
 *	Process-related routines.  Call set_chld(xchild *) to indicate
 *	that a given child process should be watched for when it
 *	terminates.  Call clear_chld() to remove the process from the
 *	list.  When the child terminates, the record is removed from
 *	the list and xchild->proc is called.
 *	The caller can set the `io' member in xchild to a pointer to
 *	an xio structure for reading from the file descriptor; see
 *	psgs.c and util.c for examples.
 */

static struct xchild *child_recs = NULL;	/* head of child process list */

void
set_chld(struct xchild *cp)
{
    cp->next = child_recs;
    child_recs = cp;
}

void
clear_chld(struct xchild *cp)
{
    struct xchild	**cpp;
    struct xchild	*cp2;

    if (child_recs == NULL) {
	if (globals.debug & DBG_EVENT)
	    fprintf(stderr, "child_recs: %p\n", (void *)child_recs);
	return;
    }
    for (cpp = &child_recs;;) {
	cp2 = *cpp;
	if (cp2 == cp)
	    break;
	cpp = &cp2->next;
    }
    *cpp = cp->next;
}

static void
do_sigchld(void)
{
    pid_t pid;
    int	status;

    sig_flags &= ~SF_CHLD;

#if ! HAVE_SIGACTION
    (void) signal(SIGCHLD, handle_sigchld);	/* reset the signal */
#endif
    for (;;) {
#if HAVE_WAITPID
	pid = waitpid(-1, &status, WNOHANG);
#else
	pid = wait3(&status, WNOHANG, NULL);
#endif
	if (pid == 0) break;

	if (pid != -1) {
	    struct xchild	**cpp;
	    struct xchild	*cp;

	    for (cpp = &child_recs;;) {
		cp = *cpp;
		if (cp == NULL)
		    break;
		if (cp->pid == pid) {
		    *cpp = cp->next;	/* unlink it */
		    /* call exit reporting procedure for this child */
		    (cp->proc)(status, cp);
		    break;
		}
		cpp = &cp->next;
	    }
	    break;
	}

	if (errno == EINTR) continue;
	if (errno == ECHILD) break;
#if HAVE_WAITPID
	perror("xdvi: waitpid");
#else
	perror("xdvi: wait3");
#endif
	break;
    }
}


/*
 *	File-related routines.  Call set_io() to indicate that a given fd
 *	should be watched for ability to input or output data.  Call clear_io()
 *	to remove it from the list.  When poll()/select() indicates that the fd
 *	is available for the indicated type of i/o, the corresponding routine
 *	is called.  Call clear_io() to remove an fd from the list.
 *	Both set_io() and clear_io() can be called from within read_proc or
 *	write_proc (although turning an io descriptor on or off is better
 *	accomplished by setting the events flag in the xio structure, and
 *	in the corresponding pollfd structure if the pfd pointer is not NULL
 *	(it is always non-NULL when read_proc and write_proc are called)).
 *	We allocate space for one additional record in the pollfd array, to
 *	accommodate the fd for the X connection; this is done by initializing
 *	num_fds to 1 instead of zero.
 */

static	struct xio	*iorecs	= NULL;	/* head of xio list */

#if HAVE_POLL
static	struct pollfd	*fds	= NULL;
static	int		num_fds	= 1;	/* current number of fds */
static	int		max_fds	= 0;	/* max allocated number of fds */
static	Boolean		io_dirty= True;	/* need to recompute fds[] array */
#else
static	int		numfds	= 0;
static	fd_set		readfds;
static	fd_set		writefds;
#endif

void
set_io(struct xio *ip)
{
    ip->next = iorecs;
    iorecs = ip;

#if HAVE_POLL
    ++num_fds;
    if (!io_dirty && num_fds <= max_fds) {
	fds[num_fds - 1].fd = ip->fd;
	fds[num_fds - 1].events = ip->xio_events;
	ip->pfd = &fds[num_fds - 1];
    }
    else {
	ip->pfd = NULL;
	io_dirty = True;
    }
#else
    if (numfds <= ip->fd) numfds = ip->fd + 1;
#endif
}

void
clear_io(struct xio *ip)
{
    struct xio	**ipp;

    for (ipp = &iorecs;;) {
	struct xio *ip2;

	ip2 = *ipp;
	if (ip2 == ip) break;
	ipp = &ip2->next;
    }
    *ipp = ip->next;

#if HAVE_POLL
    --num_fds;
    io_dirty = True;
#else
# if FLAKY_SIGPOLL
    numfds = ConnectionNumber(DISP);
# else
    numfds = (event_freq < 0 ? -1 : ConnectionNumber(DISP));
# endif
    for (ip = iorecs; ip != NULL; ip = ip->next)
	if (ip->fd > numfds)
	    numfds = ip->fd;
    ++numfds;
#endif /* !HAVE_POLL */
}

static void
do_sigpoll(void)
{
#if FLAKY_SIGPOLL
    sig_flags &= ~SF_POLL;
#else
    struct xio	*ip;

    sig_flags &= ~SF_POLL;

# if HAVE_POLL

    if (io_dirty) {
	struct pollfd *fp;

	if (num_fds > max_fds) {
	    if (fds != NULL) free(fds);
	    fds = xmalloc(num_fds * sizeof *fds);
	    memset(fds, 0, num_fds * sizeof *fds);
	    max_fds = num_fds;
	    fds->fd = ConnectionNumber(DISP);
	    fds->events = POLLIN;
	}
	fp = fds + 1;
	for (ip = iorecs; ip != NULL; ip = ip->next) {
	    fp->fd = ip->fd;
	    fp->events = ip->xio_events;
	    ip->pfd = fp;
	    ++fp;
	}
	io_dirty = False;
    }

    for (;;) {
	if (poll(fds + 1, num_fds - 1, 0) >= 0)
	    break;

	if (errno != EAGAIN && errno != EINTR) {
	    perror("xdvi: poll");
	    return;
	}
    }

    for (ip = iorecs; ip != NULL; ip = ip->next) {
	int revents = ip->pfd->revents;
	if (revents & POLLIN && ip->read_proc != NULL)
	    (void)(ip->read_proc)(ip->fd, ip->data);
	if (revents & POLLOUT && ip->write_proc != NULL)
	    (ip->write_proc)(ip->fd, ip->data);
    }

# else

    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    for (ip = iorecs; ip != NULL; ip = ip->next) {
	if (ip->xio_events & XIO_IN)
	    FD_SET(ip->fd, &readfds);
	if (ip->xio_events & XIO_OUT)
	    FD_SET(ip->fd, &writefds);
    }

    for (;;) {
	struct timeval tv;

	tv.tv_sec = tv.tv_usec = 0;
	if (select(numfds, &readfds, &writefds, (fd_set *) NULL, &tv) >= 0)
	    break;

	if (errno != EAGAIN && errno != EINTR) {
	    perror("select (xdvi read_events)");
	    return;
	}
    }

    for (ip = iorecs; ip != NULL; ip = ip->next) {
	if (FD_ISSET(ip->fd, &readfds) && ip->read_proc != NULL)
	    (void)(ip->read_proc)(ip->fd, ip->data);
	if (FD_ISSET(ip->fd, &writefds) && ip->write_proc != NULL)
	    (ip->write_proc)(ip->fd, ip->data);
    }

# endif
#endif /* not FLAKY_SIGPOLL */
}


/*
 *	Timer-related routines.  Call set_timer() to set a timer a given number
 *	of milliseconds in the future.  At that time, the timer will be cleared
 *	and the given procedure will be called with argument set to the struct
 *	passed to set_timer().  The timer routine may call set_timer() or
 *	cancel_timer().
 */


static struct xtimer *timers = NULL;	/* head of timer list */

static struct itimerval itv = {{0, 0}, {0, 0}};

#ifndef	timercmp
#define	timercmp(a, b, OP)	(((a)->tv_sec OP (b)->tv_sec || \
	((a)->tv_sec == (b)->tv_sec && (a)->tv_usec OP (b)->tv_usec)))
#endif	/* timercmp */


static void
show_timers(const char *what)
{
    struct xtimer *tp;
    fprintf(stderr, "=======%s; timers:\n", what);
    for (tp = timers; tp != NULL; tp = tp->next) {
	fprintf(stderr, "timer %p: %lu\n", (void *)tp, (unsigned long)tp->when.tv_sec);
    }
    fprintf(stderr, "=======\n");
}

void
set_timer(struct xtimer *tp, int ms)
{
    struct xtimer	**tpp;
    struct xtimer	*tp2;

    if (globals.debug & DBG_EVENT)
	fprintf(stderr, "%s:%d: set_timer\n", __FILE__, __LINE__);

    gettimeofday(&tp->when, NULL);
    itv.it_value.tv_sec = ms / 1000;
    itv.it_value.tv_usec = (ms % 1000) * 1000;
    tp->when.tv_sec += itv.it_value.tv_sec;
    tp->when.tv_usec += itv.it_value.tv_usec;
    if (tp->when.tv_usec >= 1000000) {
	tp->when.tv_usec -= 1000000;
	++tp->when.tv_sec;
    }

    for (tpp = &timers;;) {		/* add timer to list */
	tp2 = *tpp;
	if (tp2 == NULL || timercmp(&tp->when, &tp2->when, <))
	    break;
	tpp = &tp2->next;
    }
    tp->next = tp2;
    *tpp = tp;

    if (tpp == &timers) {
	setitimer(ITIMER_REAL, &itv, NULL);
	if (ms == 0)
	    sig_flags |= SF_ALRM;
    }
    if (globals.debug & DBG_EVENT)
	show_timers("after set_timer");
}

void
cancel_timer(struct xtimer *tp)
{
    struct xtimer **tpp;

    if (globals.debug & DBG_EVENT)
	show_timers("beginning of cancel_timer");
    
    if (timers == NULL) {
	fprintf(stderr, "%s:%d: BUG? timers == NULL!\n", __FILE__, __LINE__);
	return;
    }

    if (globals.debug & DBG_EVENT)
	fprintf(stderr, "%s:%d: cancel_timer %p from %p\n", __FILE__, __LINE__, (void *)&timers, (void *)tp);

    ASSERT(timers != NULL, "timers in cancel_timer() mustn't be NULL");
    for (tpp = &timers; ; ) {		/* remove from list */
	if (*tpp == tp)
	    break;
	tpp = &(*tpp)->next;
    }

    *tpp = (*tpp)->next;	/* unlink it */

    if (timers == NULL) {	/* cancel SIGALRM */
	itv.it_value.tv_sec = itv.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL, &itv, NULL);
    }
}

#if XDVI_XT_TIMER_HACK
/*
 * Original comment by Paul:
 *	Newer versions of the Motif toolkit use the timer facility
 *	(XtAppAddTimeOut(), etc.) in the X Toolkit.  Proper functioning of
 *	this mechanism, however, requires that the X Toolkit be in charge of
 *	blocking.  Since xdvi does its own blocking, this means that we need
 *	to provide working alternatives to these X Toolkit routines (by
 *	redefining them ...).
 *	One symptom of the above-mentioned bug is that the printlog window
 *	eventually stops showing dvips progress until you move the mouse.
 *
 * Comment SU:
 *	Xdvik also uses XtAppAddTimeOut() in other places (hyperref/statusline/...),
 *	so we also need these redefinitions also in the Xaw version.
 *
 */

static void xt_alarm(struct xtimer *this, void *data);

static struct xtimer *xt_free_timers = NULL;

XtIntervalId
XtAddTimeOut(unsigned long interval, XtTimerCallbackProc proc, XtPointer closure)
{
    return XtAppAddTimeOut(NULL, interval, proc, closure);
}

XtIntervalId
XtAppAddTimeOut(XtAppContext app, unsigned long interval, XtTimerCallbackProc proc, XtPointer closure)
{
    struct xtimer *tp;
    
    UNUSED(app);

    /* FIXME: better way of checking this instead of static boolean sigalarm_initialized?
       The following doesn't work, even after
       sigaddset(&all_signals, SIGALRM);
       
       static sigset_t sig_set;
       (void)sigprocmask(0, NULL, &sig_set);
       if (sigismember(&sig_set, SIGALRM))
       ... OK ...
       else
       ... NOT OK ...
    */
    ASSERT(sigalarm_initialized, "Shouldn't invoke XtAppAddTimeOut() before setup_sigalarm()");
    
    if (globals.debug & DBG_EVENT)
	fprintf(stderr, "XtAppAddTimeOut: %lu msecs\n", interval);
    
    if (xt_free_timers == NULL)
	tp = xmalloc(sizeof *tp);
    else {
	tp = xt_free_timers;
	xt_free_timers = xt_free_timers->next;
    }

    tp->proc = xt_alarm;
    tp->data = closure;
    tp->xt_proc = proc;
    tp->closure = closure;
    
    set_timer(tp, interval);

    if (globals.debug & DBG_EVENT)
	show_timers("XtAppAddTimeOut");

    return (XtIntervalId)tp;
}

void
XtRemoveTimeOut(XtIntervalId id)
{
    struct xtimer *tp;

    ASSERT(id != 0, "XtIntervalId argument in XtRemoveTimeOut() mustn't be NULL");
    tp = (struct xtimer *)id;

    /* Motif (2.1 on Solaris 9, 2003) sometimes calls XtRemoveTimeOut() after
       the timer event has occurred, so we need to be sure not to remove
       the timer record twice.  */
    if (tp->proc == NULL)
	return;
    
    cancel_timer(tp);
    
    tp->next = xt_free_timers;
    xt_free_timers = tp;

    if (globals.debug & DBG_EVENT)
	show_timers("XtRemoveTimeOut");
}

void
xt_alarm(struct xtimer *tp, void *data)
{
    XtIntervalId id;
    UNUSED(data);

    tp->proc = NULL;	/* flag timer as used-up */
    id = (XtIntervalId)tp;
    (tp->xt_proc)(tp->closure, &id);
    
    tp->next = xt_free_timers;
    xt_free_timers = tp;

    if (globals.debug & DBG_EVENT)
	show_timers("xt_alarm");
}
#endif /* XDVI_XT_TIMER_HACK */

static	void
do_sigalrm(void)
{
    struct timeval now;

    sig_flags &= ~SF_ALRM;
#ifndef HAVE_SIGACTION
    (void) signal(SIGALRM, handle_sigalrm);	/* reset the signal */
#endif

    gettimeofday(&now, NULL);

    while (timers != NULL && timercmp(&timers->when, &now, <=)) {
	struct xtimer *tp = timers;
	timers = timers->next;	/* unlink it _first_ */
	(tp->proc)(tp, tp->data);
    }

    if (timers != NULL) {		/* set next timer */
	int i;
	itv.it_value.tv_sec = timers->when.tv_sec - now.tv_sec;
	i = timers->when.tv_usec - now.tv_usec;
	if (i < 0) {
	    --itv.it_value.tv_sec;
	    i += 1000000;
	}
	itv.it_value.tv_usec = i;

	setitimer(ITIMER_REAL, &itv, NULL);
    }
}



/*
 *	Handle SIGUSR1 signal.  Pretty straightforward.
 */

static void
do_sigusr(void)
{
    sig_flags &= ~SF_USR;
#ifndef HAVE_SIGACTION
    (void) signal(SIGUSR1, handle_sigusr);	/* reset the signal */
#endif
    globals.ev.flags |= EV_RELOAD;
}


static void
do_sigsegv(void)
{
    sig_flags &= ~SF_SEGV;
#ifndef HAVE_SIGACTION
    (void) signal(SIGSEGV, handle_sigsegv);	/* reset the signal */
#endif
    handle_sigsegv(SIGSEGV);
}


/*
 *	Handle termination signals.  Just call xdvi_exit, which does all the
 *	work, since that is the common exit point.
 */

static void
do_sigterm(void)
{
    sig_flags &= ~SF_TERM;

    xdvi_exit(EXIT_SUCCESS);
}

/*
 * This routine should be used for all exits. (SU: This is different
 * from non-k xdvi, where it's only used for `non-early' exits; all
 * routines called here should be aware of their own state and either
 * perform cleanup or just return, unless xdvi_exit() itself checks for
 * the status).
 */

void
xdvi_exit(int status)
{
    struct xchild   *cp;

    /* do the following only if the window has been opened: */
    if (globals.widgets.top_level != 0 && XtIsRealized(globals.widgets.top_level)) {
	char *filehist;
	file_history_set_page(current_page);
	filehist = file_history_get_list();
	store_preference(NULL, "fileHistory", "%s", filehist);
	free(filehist);

#if MOTIF
	if (preferences_changed()) {
	    return;
	}
	/*      else { */
	/*  	fprintf(stderr, "Preferences not changed.\n"); */
	/*      } */
#endif
	/* try to save user preferences, unless we're exiting with an error */
	if (status == EXIT_SUCCESS && !save_user_preferences(True))
	    return;
    
	/* Clean up the "xdvi windows" property in the root window.  */
	update_window_property(XtWindow(globals.widgets.top_level), False);
    }

#if PS
    ps_destroy_nofree();
#endif

    /* loop through child processes */
    for (cp = child_recs; cp != NULL; cp = cp->next) {
	if (cp->killsig > 0)
	    kill(cp->pid, cp->killsig);
	else if (cp->killsig < 0)
	    kill(-cp->pid, -cp->killsig);
    }

    call_exit_handlers();

    exit(status);
}


#if HAVE_XI21

/*
 *	XInput 2.1 event processing.
 *
 *	The XInput system has (virtual) "master" devices, including a master
 *	pointer and master keyboard (and maybe others), and "slave" devices
 *	that can be assigned to a master.  Each master can have only one slave
 *	at a time.  If you have two mice plugged in, for example, each is a
 *	separate slave device, and the one assigned to the master pointer device
 *	is whichever one was moved or clicked most recently.  Moving or clicking
 *	the other mouse would then cause an XInput SlaveSwitch event, which
 *	notifies the X client that a different pointer is now active (and the
 *	previously active mouse would then become inactive).
 *
 *	The code here looks not just at events from the master pointer, it also
 *	tracks events from the slave pointer devices.  This is because the
 *	valuators for each slave pointer may have different scales.
 *
 *	The XI_Motion events contain the master and slave ids of the device
 *	sending the event, so in principle it wouldn't be necessary to track
 *	SlaveSwitch events.  However, we do track these events, because we need
 *	to know the valuator values at the time of the switch (otherwise, e.g.,
 *	the first click of a wheel mouse would be lost).
 *
 *	It is also possible that a valuator may change while the pointer is
 *	outside of the drawing window.  So, we need to reset the valuator
 *	values upon XI_Enter events.  For some strange reason, we get extra
 *	XI_Enter events when using a traditional wheel mouse, and so the code
 *	takes care to ignore those.  This only happens with the Xaw toolkit.
 *
 *	Slave devices have events that notify clients when they are (i) added
 *	or removed, (ii) attached or detached, and (iii) enabled or disabled.
 *	We only worry about (iii), and for disabled devices we don't bother to
 *	delete the data structure.
 *
 */

/*
 *	xi2_find_master:  set xi2_current to record for given device id
 *	Return True if found, False if not.
 */

static Boolean
xi2_find_master(int id)
{
	struct xi2_master *mp;

	for (mp = xi2_masters; mp != NULL; mp = mp->next)
	    if (mp->id == id) {
		xi2_current = mp;
		return True;
	    }
	return False;
}

/*
 *	xi2_find_slave:  set xi2_current->slave to record for given slave device
 *	Return True if found, False if not.
 */

static Boolean
xi2_find_slave(int id)
{
	struct xi2_slave *sp;

	for (sp = xi2_slaves; sp != NULL; sp = sp->next)
	    if (sp->id == id) {
		xi2_current->slave = sp;
		return True;
	    }
	return False;
}

/*
 *	Handle XI2 Enter event.  We need to look at these events because if the
 *	user scrolls in some other window, then the valuator will be changed
 *	by a large amount, and that scrolling should not occur also in the
 *	main xdvi window when the user resumes scrolling here.
 */

static void
xi2_ev_enter(XIEnterEvent *xi_event)
{
	XIDeviceInfo	*info;
	int		ndevices;
	int		i;

	if (xi_event->time < xi2_ign_time + 5) {
	    TRACE_EVENTS((stderr, "Ignoring XI_Enter event as spurious."));
	    return;
	}

	if (xi2_current->id != xi_event->deviceid
	  && !xi2_find_master(xi_event->deviceid)) {
	    TRACE_EVENTS((stderr,
		"Ignoring XI_Enter event: master device %d not found",
		  xi_event->deviceid));
	    return;
	}

	if (xi_event->sourceid == xi2_current->id) {
	    TRACE_EVENTS((stderr,
	      "Ignoring XI_Enter event for master device."));
	    return;
	}
	else if (xi_event->sourceid != xi2_current->slave->id) {
	    struct xi2_slave *prev_slave = xi2_current->slave;

	    if (!xi2_find_slave(xi_event->sourceid)) {
		TRACE_EVENTS((stderr,
		  "Ignoring XI_Enter event: device %d->%d not found.",
		  xi_event->deviceid, xi_event->sourceid));
		return;
	    }
	    TRACE_EVENTS((stderr,
	      "%s %d->%d",
	      prev_slave == &xi2_no_slave
		? "Implicit switch in XI_Enter to"
		: "Received out-of-turn XI_Enter event for",
	      xi_event->deviceid, xi_event->sourceid));
	    if (!xi2_current->slave->enabled)
		TRACE_EVENTS((stderr, "Caution: slave is not enabled."));
	}
	else {
	    TRACE_EVENTS((stderr, "Received XI_Enter event"));
	}

	info = XIQueryDevice(DISP, xi_event->sourceid, &ndevices);
	if (info == NULL || ndevices != 1) {
	    TRACE_EVENTS((stderr, "xi2_ev_enter:  XIQueryDevice failed for %d",
	      xi_event->sourceid));
	    return;
	}

	for (i = 0; i < info->num_classes; ++i)
	    if (info->classes[i]->type == XIValuatorClass) {
		XIValuatorClassInfo *valuator;

		valuator = (XIValuatorClassInfo *) info->classes[i];
		if (valuator->number == xi2_current->slave->vert.number) {
		    xi2_current->slave->vert.lastexact =
		    xi2_current->slave->vert.lastval = valuator->value;
		    xi2_current->slave->vert.serial =
		      LastKnownRequestProcessed(DISP);
		}
		else if (valuator->number == xi2_current->slave->horiz.number) {
		    xi2_current->slave->horiz.lastexact =
		    xi2_current->slave->horiz.lastval = valuator->value;
		    xi2_current->slave->horiz.serial =
		      LastKnownRequestProcessed(DISP);
		}
	    }
	XIFreeDeviceInfo(info);
}

static void
xi2_emulate_action(struct xdvi_action *actp, struct xi2_valinfo *valinfo,
	double value)
{
	double factor;
	int dist;

	for (; actp != NULL; actp = actp->next) {
	    if (actp->proc == Act_wheel || actp->proc == Act_hwheel) {
		if (actp->num_params == 0)
		    factor = resource.wheel_unit;
		else if (index(actp->params[0], '.') == NULL)
		    factor = atoi(actp->params[0]);
		else
		    factor = atof(actp->params[0]) * resource.wheel_unit;
		factor /= valinfo->increment;

		if (valinfo->factor != fabs(factor)) {
		    valinfo->lastval = valinfo->lastexact;
		    valinfo->factor = fabs(factor);
		}

		/*
		 * In extreme cases, the server may reset the valuator to zero
		 * to avoid large values.  The next few lines avoid the extreme
		 * scrolling event that would otherwise occur in this event.
		 */
		if (fabs(value - valinfo->lastval) > INT_MAX / 2) {
		    valinfo->lastval +=
		      value > valinfo->lastval ? INT_MAX : -INT_MAX;
		    TRACE_EVENTS((stderr,
		      "Adjusted for pointer overflow; lastval = %g, new = %g\n",
		      valinfo->lastval, value));
		}

		dist = (value - valinfo->lastval) * factor;
		if (dist == 0) {
		    valinfo->lastexact = value;
		    continue;
		}

		if (actp->proc == Act_wheel) {
#  if !MOTIF
		    if (globals.widgets.y_bar != NULL)
			XtCallCallbacks(globals.widgets.y_bar, XtNscrollProc,
			  cast_int_to_XtPointer(dist));
#  else /* MOTIF */
		    get_xy();
		    set_bar_value(globals.widgets.y_bar,
		      dist - m_window_y, (int) (globals.page.h - mane.height));
#  endif /* MOTIF */
		}
		else {	/* Act_hwheel */
#  if !MOTIF
		    if (globals.widgets.x_bar != NULL)
			XtCallCallbacks(globals.widgets.x_bar, XtNscrollProc,
			  cast_int_to_XtPointer(dist));
#  else /* MOTIF */
		    get_xy();
		    set_bar_value(globals.widgets.x_bar,
		      dist - m_window_x, (int) (globals.page.w - mane.width));
#  endif /* MOTIF */
		}
		/* The next line puts back any rounding error */
		valinfo->lastval += dist / factor;
		valinfo->lastexact = value;
	    }
	    else if (actp->proc == Act_mouse_modes) {
		size_t mode_idx = 0; /* default: one action for all modes */
		struct xdvi_action *my_action = NULL;

		if (actp->num_params > 1) { /* if different actions */
		    if (resource.mouse_mode >= actp->num_params) {
			size_t k;

			XDVI_WARNING((stderr, "X action 'mouse-modes' called "
			    "with only %d parameters, should be %d",
			  actp->num_params, resource.mouse_mode + 1));
			for (k = 0; k < actp->num_params; k++) {
			    XDVI_WARNING((stderr, "  Param %d: `%s'",
			      (int)(k + 1), actp->params[k]));
			}
			return;
		    }
		    mode_idx = resource.mouse_mode;
		}

		cached_compile_action(actp->params[mode_idx], &my_action);
		xi2_emulate_action(my_action, valinfo, value);
	    }
	    else
		TRACE_EVENTS((stderr, "Mouse_translations action is neither "
		    "wheel() nor hwheel(); ignoring."));
	}
}

static void
xi2_do_valuator(XIDeviceEvent *xi_event, struct xi2_valinfo *valinfo,
	int button, double value)
{
	struct mouse_acts *mactp;
	Modifiers state;

	state = xi_event->buttons.mask[0];
	if (xi_event->buttons.mask_len >= 2) {
	    state |= xi_event->buttons.mask[1] << 8;
	    if (xi_event->buttons.mask_len >= 3) {
		state |= xi_event->buttons.mask[2] << 16;
		if (xi_event->buttons.mask_len >= 4)
		    state |= xi_event->buttons.mask[3] << 24;
	    }
	}

	for (mactp = mouse_actions; mactp != NULL; mactp = mactp->next)
	    if (mactp->button == button || mactp->button == 0) {
		Modifiers mask = 0;
		Modifiers mods_value = 0;

		if (mactp->late_bindings == NULL
		  || _XtComputeLateBindings(DISP, mactp->late_bindings,
		  &mods_value, &mask)) {
		    mask |= mactp->mask;
		    mods_value |= mactp->value;
		    if (((mods_value ^ state) & mask) == 0) {
			xi2_emulate_action(mactp->action, valinfo, value);
			return;
		    }
		}
	    }
}

static void
xi2_ev_motion(XIDeviceEvent *xi_event)
{
	double *val;
	int i;
	XEvent event;

	if (xi2_current->id != xi_event->deviceid
	  && !xi2_find_master(xi_event->deviceid)) {
	    TRACE_EVENTS((stderr,
	      "Ignoring XI_Motion event: master device %d not found",
	      xi_event->deviceid));
	    return;
	}

	if (xi2_current->slave->id != xi_event->sourceid) {
	    struct xi2_slave *prev_slave = xi2_current->slave;

	    if (!xi2_find_slave(xi_event->sourceid)) {
		TRACE_EVENTS((stderr,
		  "Ignoring XI_Motion event: device %d->%d not found.",
		  xi_event->deviceid, xi_event->sourceid));
		return;
	    }
	    TRACE_EVENTS((stderr, "%s %d->%d\n",
	      prev_slave == &xi2_no_slave
		? "Implicit switch in XI_Motion to"
		: "Received out-of-turn XI_Motion event for",
	      xi_event->deviceid, xi_event->sourceid));
	    if (!xi2_current->slave->enabled)
		TRACE_EVENTS((stderr, "Caution: slave is not enabled."));
	}

	val = xi_event->valuators.values;
	for (i = 0; i < xi_event->valuators.mask_len * 8; ++i)
	    if (XIMaskIsSet(xi_event->valuators.mask, i)) {
		if (i == xi2_current->slave->vert.number) {
		    if ((long) (xi_event->serial
		      - xi2_current->slave->vert.serial) < 0) {
			TRACE_EVENTS((stderr,
			  "Vertical valuator in EnterEvent was outdated "
			    "(%lu < %lu)",
			  xi_event->serial, xi2_current->slave->vert.serial));
			xi2_current->slave->vert.serial = xi_event->serial;
			xi2_current->slave->vert.lastexact =
			xi2_current->slave->vert.lastval = *val;
		    }
		    else
			xi2_do_valuator(xi_event, &xi2_current->slave->vert, 5,
			  *val);
		}
		else if (i == xi2_current->slave->horiz.number) {
		    if ((long) (xi_event->serial
		      - xi2_current->slave->horiz.serial) < 0) {
			TRACE_EVENTS((stderr,
			  "Horizontal valuator in EnterEvent was outdated"));
			xi2_current->slave->horiz.serial = xi_event->serial;
			xi2_current->slave->horiz.lastexact =
			xi2_current->slave->horiz.lastval = *val;
		    }
		    else
			xi2_do_valuator(xi_event, &xi2_current->slave->horiz, 7,
			  *val);
		}
		++val;
	    }

	/* For some reason the X server doesn't want to send a core motion
	 * event if an XInput motion event is already being sent.
	 * So we need to fake it.  The routines called from Act_motion()
	 * should be sure to use only the fields provided here.  */
	event.xmotion.window = xi_event->event;
	event.xmotion.x = xi_event->event_x;
	event.xmotion.y = xi_event->event_y;
	event.xmotion.x_root = xi_event->root_x;
	event.xmotion.y_root = xi_event->root_y;
	Act_motion(NULL, &event, NULL, NULL);
}

static void
xi2_ev_devchange(XIDeviceChangedEvent *xi_event)
{
	struct xi2_slave *sp;

	if (xi_event->reason == XISlaveSwitch) {
	    struct xi2_master *mp;

	    TRACE_EVENTS((stderr,
	      "Received XI_DeviceChanged event, XISlaveSwitch, device = %d->%d",
	      xi_event->deviceid, xi_event->sourceid));

	    for (sp = xi2_slaves;; sp = sp->next) {
		if (sp == NULL) {
		    TRACE_EVENTS((stderr, "ignoring (device not found)"));
		    return;
		}
		if (sp->id == xi_event->sourceid)
		    break;
	    }

	    for (mp = xi2_masters;; mp = mp->next) {
		if (mp == NULL) {
		    TRACE_EVENTS((stderr,
			"Cannot switch slave (master device not found)"));
		    return;
		}
		if (mp->id == xi_event->deviceid)
		    break;
	    }
	    mp->slave = sp;

	    /* A valuator may change when a slave is switched off */
	    xi2_init_valuators(sp, xi_event->classes, xi_event->num_classes);

	    if (!sp->enabled)
		TRACE_EVENTS((stderr, "Caution: slave device is not enabled."));
	}
	else if (xi_event->reason == XIDeviceChange) {
	    /* In principle, this will add any device, but there's no "use" */
	    /* field, so no easy way to check if it's a suitable slave device.*/
	    TRACE_EVENTS((stderr,
	     "Received XI_DeviceChanged event, XIDeviceChange, device = %d->%d",
	      xi_event->deviceid, xi_event->sourceid));

	    for (sp = xi2_slaves;; sp = sp->next) {
		if (sp == NULL) {
		    TRACE_EVENTS((stderr, " (new device)"));
		    sp = xmalloc(sizeof (struct xi2_slave));
		    sp->id = xi_event->sourceid;
		    sp->enabled = 0;
		    sp->next = xi2_slaves;
		    xi2_slaves = sp;
		    break;
		}
		if (sp->id == xi_event->sourceid)
		    break;
	    }

	    xi2_init_valuators(sp, xi_event->classes, xi_event->num_classes);

	    if (!xi2_active && sp->btn_mask) {
		TRACE_EVENTS((stderr, "Activating XI2 now."));
		xi2_activate();
	    }
	}
	else {
	    TRACE_EVENTS((stderr,
	      "Ignoring XI_DeviceChanged event with unknown reason %x, "
		"device = %d->%d",
	      xi_event->reason, xi_event->deviceid, xi_event->sourceid));
	}
}


static void
xi2_ev_hierchange(XIHierarchyEvent *xi_event)
{
	XIHierarchyInfo	*infp, *inf_end;

	if (globals.debug & DBG_EVENT) {
	    fprintf(stderr, "%s:%d: XI_HierarchyChanged event: ",
	       __FILE__, __LINE__);
	    if (xi_event->flags & XIMasterAdded)
		fputs(" XIMasterAdded", stderr);
	    if (xi_event->flags & XIMasterRemoved)
		fputs(" XIMasterRemoved", stderr);
	    if (xi_event->flags & XISlaveAdded)
		fputs(" XISlaveAdded", stderr);
	    if (xi_event->flags & XISlaveRemoved)
		fputs(" XISlaveRemoved", stderr);
	    if (xi_event->flags & XISlaveAttached)
		fputs(" XISlaveAttached", stderr);
	    if (xi_event->flags & XISlaveDetached)
		fputs(" XISlaveDetached", stderr);
	    if (xi_event->flags & XIDeviceEnabled)
		fputs(" XIDeviceEnabled", stderr);
	    if (xi_event->flags & XIDeviceDisabled)
		fputs(" XIDeviceDisabled", stderr);
	    if (!xi_event->flags)
		fputs("no flags(?)", stderr);
	    fputc('\n', stderr);
	}

	if (!(xi_event->flags & (XIDeviceEnabled | XIDeviceDisabled)))
	    return;

	inf_end = xi_event->info + (xi_event->num_info - 1);
	for (infp = xi_event->info; infp <= inf_end; ++infp) {
	    struct xi2_slave *sp;

	    if (infp->use != XISlavePointer && infp->use != XIFloatingSlave)
		continue;

	    for (sp = xi2_slaves;; sp = sp->next) {
		if (sp == NULL) {
		    if (infp->enabled)
			TRACE_EVENTS((stderr,
			  "  Ignoring enable request for %d->%d (not found)",
			  infp->attachment, infp->deviceid));
		    break;
		}
		if (sp->id == infp->deviceid) {
		    if (infp->enabled != sp->enabled) {
			TRACE_EVENTS((stderr, "  %s %d->%d",
			  infp->enabled ? "Enabling" : "Disabling",
			  infp->attachment, infp->deviceid));
			sp->enabled = infp->enabled;
			if (!sp->enabled && xi2_current->slave == sp) {
			    TRACE_EVENTS((stderr,
				"  Removing from master device"));
			    xi2_current->slave = &xi2_no_slave;
			}
		    }
		    break;
		}
	    }
	}
}

#endif /* HAVE_XI21 */


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
 *
 *	Ultimately, the goal is to have this be the only place in xdvi where
 *	blocking occurs.
 *
 *	The argument to this function should be a mask of event types (EV_*)
 *	indicating which event types should cause read_events to return instead
 *	of waiting for more events.  This function will always process all
 *	pending events and signals before returning.
 *	The return value is the value of globals.ev.flags.
 */

unsigned int
read_events(unsigned int ret_mask)
{
    XEvent event;

#if !HAVE_POLL
    if (numfds == 0)
	numfds = ConnectionNumber(DISP) + 1;
#endif

    if (globals.debug & DBG_EVENT)
	fprintf(stderr, "%s:%d: read_events %u\n", __FILE__, __LINE__, ret_mask);
    for (;;) {
	globals.ev.ctr = event_freq;
	/*
	 * The above line clears the flag indicating that an event is
	 * pending.  So if an event comes in right now, the flag will be
	 * set again needlessly, but we just end up making an extra call.
	 * Also, be careful about destroying the magnifying glass while
	 * drawing on it.
	 */

#if !FLAKY_SIGPOLL

	if (event_freq < 0) {	/* if SIGPOLL works */
	    if (!XtPending()) {
		sigset_t oldsig;

		(void) sigprocmask(SIG_BLOCK, &all_signals, &oldsig);
		for (;;) {
#ifdef SHOW_SIG_FLAGS
		    /* this gives HUGE output ... */
		    if (globals.debug & DBG_EVENT)
			fprintf(stderr, "%s:%d: sig_flags = %d\n",
				__FILE__, __LINE__, sig_flags);
#endif
		    while (sig_flags) {
			flags_to_sigproc[sig_flags]();
		    }

		    if (XtPending())
			break;

		    if (globals.ev.flags & ret_mask) {
			(void) sigprocmask(SIG_SETMASK, &oldsig, (sigset_t *) NULL);
			return globals.ev.flags;
		    }
		    (void) sigsuspend(&oldsig);
		}
		(void) sigprocmask(SIG_SETMASK, &oldsig, (sigset_t *) NULL);
	    }
	}
	else

#endif /* not FLAKY_SIGPOLL */

	{
	    for (;;) {
		struct xio	*ip;

		if (globals.debug & DBG_EVENT)
		    fprintf(stderr, "%s:%d: (flaky) sig_flags = %d\n",
			    __FILE__, __LINE__, sig_flags);
		while (sig_flags) {
		    sigset_t oldsig;

		    (void) sigprocmask(SIG_BLOCK, &all_signals, &oldsig);

		    while (sig_flags) {
			flags_to_sigproc[sig_flags]();
		    }

		    (void) sigprocmask(SIG_SETMASK, &oldsig,
				       (sigset_t *) NULL);
		}

		if (XtPending())
		    break;

		if (globals.ev.flags & ret_mask)
		    return globals.ev.flags;

		/* If a SIGUSR1 signal comes right now, then it will wait
		   until an X event or another SIGUSR1 signal arrives. */

#if HAVE_POLL
		if (globals.debug & DBG_EVENT)
		    fprintf(stderr, "%s:%d: have_poll!\n",
			    __FILE__, __LINE__);
		if (io_dirty) {
		    struct pollfd *fp;

		    if (num_fds > max_fds) {
			if (fds != NULL) free(fds);
			fds = xmalloc(num_fds * sizeof *fds);
			max_fds = num_fds;
			fds->fd = ConnectionNumber(DISP);
			fds->events = POLLIN;
		    }
		    fp = fds + 1;
		    for (ip = iorecs; ip != NULL; ip = ip->next) {
			fp->fd = ip->fd;
			fp->events = ip->xio_events;
			ip->pfd = fp;
			++fp;
		    }
		    io_dirty = False;
		}

		for (;;) {
		    if (poll(fds, num_fds, -1) >= 0) {
			for (ip = iorecs; ip != NULL; ip = ip->next) {
			    int revents = ip->pfd->revents;

			    if (revents & POLLIN && ip->read_proc != NULL)
				(ip->read_proc)(ip->fd, ip->data);
			    if (revents & POLLOUT && ip->write_proc != NULL)
				(ip->write_proc)(ip->fd, ip->data);
			}
			break;
		    }

		    if (errno == EINTR)
			break;

		    if (errno != EAGAIN) {
			perror("xdvi: poll");
			break;
		    }
		}
#else /* HAVE_POLL */
		if (globals.debug & DBG_EVENT)
		    fprintf(stderr, "%s:%d: NOT have_poll!\n",
			    __FILE__, __LINE__);
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		FD_SET(ConnectionNumber(DISP), &readfds);
		for (ip = iorecs; ip != NULL; ip = ip->next) {
		    if (ip->xio_events & XIO_IN)
			FD_SET(ip->fd, &readfds);
		    if (ip->xio_events & XIO_OUT)
			FD_SET(ip->fd, &writefds);
		}

		for (;;) {
		    if (select(numfds, &readfds, &writefds, (fd_set *) NULL,
			       (struct timeval *) NULL) >= 0) {
			for (ip = iorecs; ip != NULL; ip = ip->next) {
			    if (FD_ISSET(ip->fd, &readfds) && ip->read_proc != NULL) {
				if (globals.debug & DBG_EVENT)
				    fprintf(stderr, "%s:%d: reading from %d\n",
					    __FILE__, __LINE__, ip->fd);
				(ip->read_proc)(ip->fd, ip->data);
			    }
			    if (FD_ISSET(ip->fd, &writefds) && ip->write_proc != NULL) {
				if (globals.debug & DBG_EVENT)
				    fprintf(stderr, "%s:%d: writing to %d\n",
					    __FILE__, __LINE__, ip->fd);
				(ip->write_proc)(ip->fd, ip->data);
			    }
			}
			break;
		    }

		    if (errno == EINTR)
			break;

		    if (errno != EAGAIN) {
			perror("xdvi: select");
			break;
		    }
		}
#endif /* HAVE_POLL */
	    }
	}

	XtAppNextEvent(globals.app, &event);

#ifdef MOTIF
	if ((resource.expert_mode & XPRT_SHOW_TOOLBAR) != 0)
	    TipAppHandle(globals.app, &event);
#endif

#if HAVE_XI21
	if (event.xany.type == GenericEvent
	  && event.xcookie.extension == xi2_opcode) {
	    if (!XGetEventData(DISP, &event.xcookie)) {
		TRACE_EVENTS((stderr,
		  "Received XI2 event, of type %d, with no cookie",
		  event.xcookie.evtype));
	    }
	    else {
		switch (event.xcookie.evtype) {

		    case XI_HierarchyChanged:
			xi2_ev_hierchange(event.xcookie.data);
			break;

		    case XI_DeviceChanged:
			xi2_ev_devchange(event.xcookie.data);
			break;

		    case XI_Enter:
			xi2_ev_enter(event.xcookie.data);
			break;

		    case XI_Motion:
			/* This needs to be filled in by the client */
			((XIDeviceEvent *) event.xcookie.data)->serial
			  = event.xany.serial;
			xi2_ev_motion(event.xcookie.data);
			break;

		    default:
			TRACE_EVENTS((stderr,
			  "Received XI2 event of unknown type %d",
			  event.xcookie.evtype));

		}
		XFreeEventData(DISP, &event.xcookie);
	    }
	    continue;
	}
#endif /* HAVE_XI21 */

	if (resized)
	    get_geom();

	if (event.xany.window == magnifier.win && event.type == Expose) {
	    handle_expose((Widget) NULL, (XtPointer)&magnifier, &event,
			  (Boolean *) NULL);
	    continue;
	}
	else if (globals.broken_motif_event_handling &&
		 (globals.cursor.flags & (CURSOR_RULER | CURSOR_TEXT))) {
	    /* In this case, Act_motion() and Act_release() are not called properly
	     * for updating the ruler/text selection (it works with the magnifier though),
	     * so we need to invoke them ourselves here: */
	    if (event.type == MotionNotify)
		Act_motion(NULL, &event, NULL, NULL);
	    else if (event.type == ButtonRelease)
		Act_release(NULL, &event, NULL, NULL);
	}

#ifdef MOTIF
	if (XtIsRealized(globals.widgets.top_level)
	    && event.xany.window == XtWindow(globals.widgets.clip_widget)
	    && event.type == KeyPress) { /* workaround for #610206 */
	    motif_translations_hack();
	}
#else
	if (resource.expert_mode & XPRT_SHOW_BUTTONS)
	    SubMenuHandleEvent(globals.app, &event);
#endif
	XtDispatchEvent(&event);
    }
}


/*
 * Higher-level routines for managing events.
 */

static void
can_exposures(struct WindowRec *windowrec)
{
    windowrec->min_x = windowrec->min_y = MAXDIM;
    windowrec->max_x = windowrec->max_y = 0;
}

void
redraw(struct WindowRec *windowrec)
{
    currwin = *windowrec;
    globals.win_expose.min_x = currwin.min_x + currwin.base_x;
    globals.win_expose.min_y = currwin.min_y + currwin.base_y;
    globals.win_expose.max_x = currwin.max_x + currwin.base_x;
    globals.win_expose.max_y = currwin.max_y + currwin.base_y;
    can_exposures(windowrec);

    /* fix for bug #619070 - the complicated flags (and do_update_property)
       are needed to avoid too many updates at exposures, especially for
       a window of another xdvi instance when the magnifier intersects with
       that window.
    */
    if (have_src_specials && do_update_property
	&& globals.win_expose.min_x != 1
	&& globals.win_expose.max_y - globals.win_expose.min_y != 1
	&& currwin.base_x == 0 && currwin.base_y == 0) {
	update_window_property(XtWindow(globals.widgets.top_level), True);
    }

    TRACE_EVENTS((stderr, "Redraw %d x %d at (%d, %d) (base=%d,%d)",
		  globals.win_expose.max_x - globals.win_expose.min_x,
		  globals.win_expose.max_y - globals.win_expose.min_y,
		  globals.win_expose.min_x, globals.win_expose.min_y,
		  currwin.base_x, currwin.base_y));

    /* can't use ev_cursor here, since the event loop might not see this change quick enough */
    if (!(globals.ev.flags & EV_CURSOR)) {
	TRACE_EVENTS((stderr, "Cursor: %ld", globals.cursor.flags));
	if (!(globals.cursor.flags & (CURSOR_MAG | CURSOR_DRAG_H | CURSOR_DRAG_V | CURSOR_DRAG_A))) {
	    if (resource.mouse_mode == MOUSE_MODE3)
		XDefineCursor(DISP, CURSORWIN, globals.cursor.mode3);
	    else
		XDefineCursor(DISP, CURSORWIN, globals.cursor.wait);
	    XFlush(DISP);
	}
	globals.ev.flags |= EV_CURSOR;
    }

    /* No longer needed since windows are correctly transient now */
    /*      raise_message_windows(); */
    raise_file_selector();
    draw_page();
    globals.warn_spec_now = False;
}


void
redraw_page(void)
{
#if COLOR
    const struct rgb *rgbp;
#endif
    TRACE_FILES((stderr, "Redraw page on %p", (void *)globals.dvi_file.bak_fp));

    if (globals.dvi_file.bak_fp == NULL)
	return;

    if (scanned_page < current_page) {
	TRACE_FILES((stderr, "redraw_page: scanned_page = %d, current_page = %d, prescanning %p\n",
		     scanned_page, current_page, (void *)globals.dvi_file.bak_fp));

	prescan(globals.dvi_file.bak_fp);

	if (globals.ev.flags & EV_GE_NEWPAGE) {	/* if we need to re-prescan */
	    return;
	}
    }

    TRACE_FILES((stderr, "redraw_page: current_page = %d", current_page));
    if (pageinfo_get_window_width(current_page) != globals.page.unshrunk_w
	|| pageinfo_get_window_height(current_page) != globals.page.unshrunk_h) {
	TRACE_FILES((stderr, "NEW SIZE: %dx%d",
		     pageinfo_get_window_width(current_page), pageinfo_get_window_height(current_page)));
	init_page();
	reconfig();
    }
    
    /* We can't call home() without proper unshrunk_page_*, which requires
     * prescan(), which can't be done from within read_events() */

    /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! BUG ALERT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
       There's some complicated interaction with Postscript specials
       here: if home_action check comes before the gs stuff, psp.drawfile
       might not get initialized correctly, resulting in empty PS figures
       (bounding box instead of figure). This is different in xdvi, due to
       different handling of the home_action stuff, but at the moment I can't
       remember the reason for this ...
       !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! BUG ALERT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    */
#if PS_GS
    if (gs_postpone_prescan) {
	if (!setjmp(globals.ev.canit)) {
	    gs_resume_prescan();
	}
	else
	    return;
    }
#endif
    if (home_action != NULL) {
	home_action(False);
	home_action = NULL;
	/* This discards the expose event generated by home()
	   (1 for each page) */
	if (read_events(EV_NOWAIT) & EV_GE_NEWPAGE) {
	    return;
	}

	can_exposures(&mane);
    }

#if COLOR
    rgbp = &bg_initial;
    if (page_colors.stack != NULL) {
	ASSERT(current_page < (int)page_colors.size, "page_colors.size too small");
	rgbp = &page_colors.stack[current_page].bg;
    }

    /* Set background color */
    if (bg_current == NULL
	|| rgbp->r != bg_current->color.r
	|| rgbp->g != bg_current->color.g
	|| rgbp->b != bg_current->color.b) {
	struct bgrec **bgpp;

	for (bgpp = &bg_head;;) {
	    bg_current = *bgpp;
	    if (bg_current == NULL) {	/* if bg is not in list */
		bg_current = *bgpp = xmalloc(sizeof *bg_current);
		bg_current->next = NULL;
		bg_current->color = *rgbp;
		bg_current->fg_head = NULL;
		bg_current->pixel_good = False;
		break;
	    }
	    if (bg_current->color.r == rgbp->r
		&& bg_current->color.g == rgbp->g
		&& bg_current->color.b == rgbp->b)
		break;
	    bgpp = &bg_current->next;
	}
	fg_current = NULL;	/* force change of foreground color */
	/* globals.gc.high is only used in XDrawRectangle, so its background color
	   doesn't need to be changed.  */
	if (globals.debug & DBG_DVI)
	    printf("Changing background color to %5d %5d %5d\n",
		   bg_current->color.r, bg_current->color.g,
		   bg_current->color.b);

	if (!bg_current->pixel_good) {
	    bg_current->pixel = alloc_color(&bg_current->color,
					    color_data[1].pixel);
	    bg_current->pixel_good = True;
	}
	XSetWindowBackground(DISP, mane.win, bg_current->pixel);
#if MOTIF && !FIXED_FLUSHING_PAGING
	XtVaSetValues(XtParent(globals.widgets.draw_widget), XtNbackground, bg_current->pixel, NULL);
#endif
	/* 	XSetWindowBackground(DISP, mane.win, bg_current->pixel); */
	/*  	XClearWindow(DISP, mane.win); */
#if 0 /* don't recolor the cursor - gives too low contrast on color backgrounds,
	 and bad appearance when part of the background is white and cursor mask
	 is colored */
	{
	    XColor	bg_Color;
	    bg_Color.pixel = bg_current->pixel;
	    XQueryColor(DISP, G_colormap, &bg_Color);
	    XRecolorCursor(DISP, globals.cursor.ready, &globals.cr_color, &bg_Color);
	    XRecolorCursor(DISP, globals.cursor.wait, &globals.cr_color, &bg_Color);
	}
#endif
    }
#endif /* COLOR */
    
    if (!globals.pausing.flag) {
#if PS_GS
	gs_erase_page();
#endif
	XClearWindow(DISP, mane.win);
    }

    if (G_backing_store != NotUseful) {
	mane.min_x = mane.min_y = 0;
	mane.max_x = globals.page.w;
	mane.max_y = globals.page.h;
    }
    else {
	get_xy();
	mane.min_x = -m_window_x;
	mane.max_x = -m_window_x + mane.width;
	mane.min_y = -m_window_y;
	mane.max_y = -m_window_y + mane.height;
    }

    /*      update_TOC(); */
    redraw(&mane);
}

void
do_pages(void)
{
    if (globals.debug & DBG_BATCH) {
	
	(void)read_events(EV_GT_IDLE);
	for (current_page = 0; current_page < total_pages; ++current_page) {
	    if (resource.keep_flag) {
		home_action = NULL;
	    }
	    else {
		home_action = home;
	    }
	    globals.warn_spec_now = resource.warn_spec;
#if PS_GS
	    for (;;) {
		redraw_page();
		(void) read_events(EV_NOWAIT);
		if (!(globals.ev.flags & (EV_NEWPAGE | EV_NEWDOC | EV_RELOAD)))
		    break;
		globals.ev.flags = EV_IDLE;
	    }
#else
	    redraw_page();
#endif
	}
	xdvi_exit(EXIT_SUCCESS);
    }
    else {
	for (;;) {	/* normal operation */
	    (void) read_events(EV_GT_IDLE);
 	    TRACE_EVENTS((stderr, "globals.ev.flags: %d; ev_newpage: %d, ev_newdoc: %d, ev_reload: %d\n",
			  globals.ev.flags, EV_NEWPAGE, EV_NEWDOC, EV_RELOAD));
	    /* NOTE: reloading must be checked first! */
	    if (globals.ev.flags & (EV_NEWPAGE | EV_NEWDOC | EV_RELOAD | EV_PS_TOGGLE)) {
		TRACE_EVENTS((stderr, "EV_NEWPAGE | ..."));
		globals.ev.flags &= ~(EV_NEWPAGE | EV_EXPOSE | EV_PS_TOGGLE);
		if (globals.ev.flags & EV_RELOAD) {
		    dviErrFlagT errflag;

		    globals.ev.flags &= ~EV_RELOAD;
		    if (load_dvi_file(
#if !DELAYED_MKTEXPK
				      True,
#endif
				      &errflag)) {
			statusline_info(STATUS_SHORT, "File reloaded.");
		    }
		    /* 		    else { */
		    /* 			statusline_info(STATUS_MEDIUM, "File corrupted, not reloading."); */
		    /* 		    } */
		}
		if (globals.ev.flags & EV_NEWDOC) {
		    dviErrFlagT errflag;
		    TRACE_EVENTS((stderr, "EV_NEWDOC!"));
		    /*  		    fprintf(stderr, "newdoc!\n"); */
		    TRACE_FILES((stderr, "current page: %d", current_page));
		    /*  		    file_history_set_page(current_page); */
		    globals.ev.flags &= ~EV_NEWDOC;
		    if (load_dvi_file(
#if !DELAYED_MKTEXPK
				      True,
#endif
				      &errflag)) {
			statusline_append(STATUS_SHORT, "Opened ", "Opened \"%s\"", globals.dvi_name);
			/* 			statusline_info(STATUS_SHORT, "Opened \"%s\"", globals.dvi_name); */
			TRACE_FILES((stderr, "Adding to history: |%s|\n", globals.dvi_name));
			if (file_history_push(globals.dvi_name)) { /* it's a new file, add to history */
			    TRACE_FILES((stderr, "New entry!"));
			    filehist_menu_add_entry(globals.dvi_name);
			}
			else { /* only need to move existing elements to new positions */
			    TRACE_FILES((stderr, "Existing entry!\n"));
			    filehist_menu_refresh();
			}
		    }
		}

		can_exposures(&mane);
		can_exposures(&magnifier);

		if (globals.dvi_file.bak_fp != NULL) {
		    TRACE_EVENTS((stderr, "redraw_page()"));
		    redraw_page();
		}
		else {
		    TRACE_EVENTS((stderr, "dvi_file_changed()"));
		    (void)dvi_file_changed();
		}
	    }
	    else if (globals.ev.flags & EV_PAGEHIST_GOTO_PAGE) {
		int pageno;
		globals.ev.flags &= ~EV_PAGEHIST_GOTO_PAGE;
		pageno = check_goto_page(page_history_get_page(), False);
		goto_page(pageno, resource.keep_flag ? NULL : home, False);
		TRACE_FILES((stderr, "got page: %d", pageno));
	    }
	    else if (globals.ev.flags & EV_FILEHIST_GOTO_PAGE) {
		int pageno;
		globals.ev.flags &= ~EV_FILEHIST_GOTO_PAGE;
		pageno = check_goto_page(file_history_get_page(), True);
		goto_page(pageno, resource.keep_flag ? NULL : home, False);
		TRACE_FILES((stderr, "got page: %d", pageno));
	    }
	    else if (globals.ev.flags & EV_PAGEHIST_INSERT) {
		globals.ev.flags &= ~EV_PAGEHIST_INSERT;
		page_history_insert(current_page);
	    }
	    else if (globals.ev.flags & EV_FIND_CANCEL) {
		/* NOTE: This must be done before checking for expose() */
		globals.ev.flags &= ~EV_FIND_CANCEL;
	    }
	    else if (globals.ev.flags & EV_ANCHOR) {
		/*
		 * Similar to forward search: search for a htex anchor.
		 * This needs to come before the next case which does the redraw_page(),
		 * otherwise anchors for the current page might not be drawn at all:
		 * anchor_search() sets the info later used by htex_draw_anchormarkers(),
		 * which is invoked by redraw_page().
		 */
		
		/* switch off the link cursor */
		globals.cursor.flags &= ~CURSOR_LINK;

		if (dvi_file_changed())
		    continue;
		
		anchor_search(g_anchor_pos);
		
		/* added, otherwise anchors on same page may not be drawn */
		/* NOTE: This caused a crash when clicking on link "Langer" on p3 of diss.dvi
		   since the color stack wasn't allocated for the target pages. Removed for the
		   time being, since I can't reproduce the problem for which it was introduced.
		   CVS commit message was:
		   "fix anchor drawing for other window"
		*/
		/* redraw(&mane);  */
		
		globals.ev.flags &= ~EV_ANCHOR;
	    }
	    else if (globals.ev.flags & EV_SRC) {
		/*
		 * Source special operations are deferred to here because
		 * they call geom_scan(), which may call define_font(),
		 * which may call makefont(), which may call read_events()
		 * recursively.
		 */
		if (globals.src.fwd_string != NULL) {
		    const char *s = globals.src.fwd_string;

		    if (dvi_file_changed())
			continue;
		    
		    source_forward_search(s);
		    globals.ev.flags &= ~EV_SRC;
		    globals.src.fwd_string = NULL;

		    /* de-iconify window if needed, and raise it */
		    XMapRaised(XtDisplay(globals.widgets.top_level), XtWindow(globals.widgets.top_level));
		    raise_message_windows();
		}
		else if (source_reverse_x != -1) {
		    if (dvi_file_changed())
			continue;
		    
		    source_reverse_search(source_reverse_x, source_reverse_y, True);
		    globals.ev.flags &= ~EV_SRC;
		}
		else {
		    source_special_show(source_show_all);
		    globals.ev.flags &= ~EV_SRC;
		}
	    }
	    /* support for `-findstring' */
	    else if (globals.ev.flags & EV_FIND) {
		if (dvi_file_changed())
		    continue;

		if (resource.find_string != NULL) { /* not first call */
		    dvi_find_string(resource.find_string, False);
		    resource.find_string = NULL;
		}
		else { /* actually should never arrive here?? */
		    dvi_find_string(NULL, True);
		}
		globals.ev.flags &= ~EV_FIND;
	    }
	    else if (globals.ev.flags & EV_MAG_MOVE) {
		MYTRACE((stderr, "moving mag!\n"));
		move_magnifier();
	    }
	    else if (globals.ev.flags & EV_EXPOSE) {
		if (magnifier.min_x < MAXDIM) {
		    /*  		    fprintf(stderr, "magnifier < maxdim!\n"); */
		    if (mane.min_x >= MAXDIM) {
			/*  			fprintf(stderr, "mane >= maxdim!\n"); */
			globals.ev.flags &= ~EV_EXPOSE;
		    }
		    redraw(&magnifier);
		}
		else {
		    /* see comment in mag.c */
		    globals.ev.flags &= ~EV_EXPOSE;
		    if (mane.min_x < MAXDIM) {
			redraw(&mane);
		    }
		}
	    }
	    else if (globals.ev.flags & EV_CURSOR) {
		/*
		 * This code eliminates unnecessary calls to XDefineCursor,
		 * since this is a slow operation on some hardware (e.g., S3
		 * chips).
		 */
		XSync(DISP, False);
		if (!XtPending()) {
		    Cursor curr;
			
		    if (globals.cursor.flags & CURSOR_DRAG_V)
			curr = globals.cursor.drag_v;
		    else if (globals.cursor.flags & CURSOR_DRAG_H)
			curr = globals.cursor.drag_h;
		    else if (globals.cursor.flags & CURSOR_DRAG_A)
			curr = globals.cursor.drag_a;
		    else if (resource.mouse_mode == MOUSE_MODE3)
			curr = globals.cursor.mode3;
		    else if (resource.mouse_mode == MOUSE_MODE2)
			curr = globals.cursor.mode2;
		    else if (globals.cursor.flags & CURSOR_LINK)
			curr = globals.cursor.link;
		    else if (globals.cursor.flags & CURSOR_MAG)
			curr = globals.cursor.empty;
		    else if (globals.cursor.flags & CURSOR_CORRUPTED)
			curr = globals.cursor.corrupted;
		    else if (globals.pausing.flag)
			curr = globals.cursor.pause;
		    else
			curr = globals.cursor.mode1;
		    XDefineCursor(DISP, CURSORWIN, curr);
		    globals.ev.flags &= ~EV_CURSOR;
		}
	    }
	    XFlush(DISP);
	}
    }
}

static void
Act_unpause_or_next(Widget w, XEvent *event,
		    String *params, Cardinal *num_params)
{
    if (globals.pausing.flag) {
        globals.pausing.num++;
	if (globals.pausing.num_save)
	    globals.pausing.num_save[current_page] = globals.pausing.num; 
	redraw_page();
    }
    else {
	Act_down_or_next(w, event, params, num_params);
    }
}

/*
 * timer callback for watching the DVI file.
 */
void
watch_file_cb(XtPointer client_data, XtIntervalId *id)
{
    static XtIntervalId timer = 0;
    
    UNUSED(client_data);
    UNUSED(id);

    if (resource.watch_file > 0.0) {
	unsigned long watch_time_ms;

	(void)dvi_file_changed();
    
	if (timer) {
	    XtRemoveTimeOut(timer);
	    timer = (XtIntervalId)(XtIntervalId)0;
	}

	watch_time_ms = (unsigned long)(resource.watch_file * 1000);
	timer = XtAppAddTimeOut(globals.app, watch_time_ms, watch_file_cb, (XtPointer)NULL);
    }
}

void
Act_scroll_list_up(Widget w, XEvent *event, String *params,
	Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(event);
    UNUSED(params);
    UNUSED(num_params);

    if (current_page == 0) {
	xdvi_bell();
	/* statusline_info(STATUS_SHORT, "First page of DVI file"); */
	return;
    }
    goto_page(check_goto_page(current_page - 1, True),
      resource.keep_flag ? NULL : home, False);
    search_signal_page_changed();
    statusline_erase("Page history:");
}

void
Act_scroll_list_down(Widget w, XEvent *event, String *params,
	Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(event);
    UNUSED(params);
    UNUSED(num_params);

    if (current_page >= total_pages - 1) {
	xdvi_bell();
	/* statusline_info(STATUS_SHORT, "Last page of DVI file"); */
	return;
    }
    goto_page(check_goto_page(current_page + 1, True),
      resource.keep_flag ? NULL : home, False);
    search_signal_page_changed();
    statusline_erase("Page history:");
}

void Act_pagehistory_back(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    int arg;

    UNUSED(w);
    UNUSED(event);

    if (!get_int_arg(params, num_params, &arg)) {
	arg = 1;
    }
    page_history_move(-arg);
}

void Act_pagehistory_forward(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    int arg;

    UNUSED(w);
    UNUSED(event);

    if (!get_int_arg(params, num_params, &arg)) {
	arg = 1;
    }
    page_history_move(arg);
}

void Act_pagehistory_clear(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(event);
    UNUSED(params);
    UNUSED(num_params);

    page_history_clear();
}

void Act_pagehistory_delete_backward(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    int arg;

    UNUSED(w);
    UNUSED(event);

    if (!get_int_arg(params, num_params, &arg)) {
	arg = 1;
    }
    page_history_delete(-arg);
}

void Act_pagehistory_delete_forward(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    int arg;

    UNUSED(w);
    UNUSED(event);

    if (!get_int_arg(params, num_params, &arg)) {
	arg = 1;
    }
    page_history_delete(arg);
}

#ifdef MOTIF
void Act_prefs_dialog(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    int arg;
    
    UNUSED(w);
    UNUSED(event);

    if (!get_int_arg(params, num_params, &arg)) {
	arg = -1;
    }
    
    popup_preferences_dialog(globals.widgets.top_level, arg);
}
#endif /* MOTIF */

