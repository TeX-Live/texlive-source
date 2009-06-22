/*------------------------------------------------------------
  statusline for the xdvi(k) previewer

  written by S. Ulrich (ulrich@cis.uni-muenchen.de)  2000/02/25

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to
  deal in the Software without restriction, including without limitation the
  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
  sell copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL PAUL VOJTA OR ANY OTHER AUTHOR OF THIS SOFTWARE BE
  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  ------------------------------------------------------------*/


#include "xdvi-config.h"
#include "xdvi.h"
#include "version.h"
#include "statusline.h"
#include "xm_menu.h"
#include "x_util.h"
#include "pagehist.h"
#include "util.h"

#include <stdarg.h>
#include "my-vsnprintf.h"

#include <ctype.h>
#include <X11/Xatom.h>
#include <X11/StringDefs.h>

# ifdef MOTIF
#  include <Xm/Label.h>
#  include <Xm/Frame.h>
#  include <Xm/Text.h>
#  include <Xm/TextF.h>
# else
#  include <X11/Xaw/Viewport.h>
#  include <X11/Xaw/Label.h>
# endif


Widget statusline;

static Boolean initialized = False;

/*
 * only print MAX_LEN characters to the statusline
 * (it's only 1 line after all)
 */
#define MAX_LEN 512

/* for saving the statusline string if the statusline is
 * destroyed and recreated
 */
static char g_string_savebuf[MAX_LEN + 2];

static int m_statusline_h = 20;

/* access method */
int get_statusline_height(void)
{
    return m_statusline_h;
}

#if MOTIF
static void
handle_statusline_event(Widget w, XtPointer client_data,
			XEvent *ev, Boolean *cont)
{
    /*      const char *text = (const char *)client_data; */
    UNUSED(w);
    UNUSED(client_data);
    UNUSED(cont);

    /*      fprintf(stderr, "text: |%s|; event: %p\n", text, ev); */
    /* only used to do this if page history was already active, but it's probably
       nicer to be able to get the history by clicking on the statusline ...
    */
    if (/* strncmp(text, "Page history:", sizeof "Page history:" - 1) == 0 && */ ev != NULL) {
	XmTextPosition pos = XmTextGetCursorPosition(statusline);
	char *ptr1, *ptr2;
	int diff = 0;
	/*  	fprintf(stderr, "pos: %d\n", pos); */
	if (pos == 0) { /* just display the page history */
	    page_history_move(0);
	    return;
	}
	ptr1 = g_string_savebuf + pos;
	ptr2 = strchr(g_string_savebuf, '[');
	if (ptr2 == NULL) { /* some other string, also display the page history */
	    page_history_move(0);
	    return;
	}
	/* 	fprintf(stderr, "ptr1: |%s|; ptr2: |%s|\n", ptr1, ptr2); */
	while (ptr1 < ptr2) {
	    if (*ptr1 == ' ' && *(ptr1 + 1) != '-') /* separator */
		diff--;
	    ptr1++;
	}

	while (ptr1 > ptr2) {
	    if (*ptr1 == ' ' && *(ptr1 - 1) != '-') /* separator */
		diff++;
	    ptr1--;
	}
	/* 	fprintf(stderr, "diff: %d\n", diff); */
	page_history_move(diff);
    }
}
#endif /* MOTIF */

/*
 * Create the statusline widget. To be called at the beginning
 * of the program, and when expert mode is switched off.
 *
 *  Side effects:
 *	sets <m_statusline_h> to the height of the statusline in pixels.
 */

Widget
create_statusline(
#ifdef MOTIF
		  Widget parent
#else
		  void
#endif		  
		  )
{
#ifndef MOTIF
    Position vport_h;
    Position clip_x;
    Position clip_w;
    static Position my_h = 0;
#endif

    /*
     * FIXME: is there a better way to set the y position depending on
     * the height of the widget?
     * It doesn't work to change the value of XtNy *after* creating
     * the widget!
     */

    if (!initialized) {
#ifndef MOTIF
	/*
	 * determine height of statusline (depending on the font used).
	 * This is not changeable at runtime, so it's determined once and
	 * for all at program start.
	 */
	statusline = XtVaCreateWidget("statusline", labelWidgetClass, globals.widgets.vport_widget,
				      XtNlabel, (XtArgVal) "test",
				      NULL);
	XtVaGetValues(statusline, XtNheight, &my_h, NULL);
	m_statusline_h = my_h;
	XtDestroyWidget(statusline);
#endif
	initialized = True;
	/* initialize g_string_savebuf */
	sprintf(g_string_savebuf, "This is xdvik %s", XDVI_TERSE_VERSION_INFO);
    }
#ifndef MOTIF
    /* determine position and width of statusline */
    XtVaGetValues(globals.widgets.clip_widget, XtNx, &clip_x, XtNwidth, &clip_w, NULL);
    XtVaGetValues(globals.widgets.vport_widget, XtNheight, &vport_h, NULL);
    if (vport_h - my_h <= 0) {
	XDVI_FATAL((stderr, "Window height too small for statusline (minimum value: %d).", my_h));
	return NULL;
    }
    statusline = XtVaCreateManagedWidget("statusline",
					 labelWidgetClass, globals.widgets.vport_widget,
					 XtNlabel, (XtArgVal) g_string_savebuf,
					 XtNwidth, clip_w,
					 XtNx, clip_x - 1,	/* so that left border becomes invisible */
					 XtNy, vport_h - my_h,
					 XtNjustify, XtJustifyLeft,
					 /* same as for the buttons line */
					 XtNborder, (XtArgVal) resource.fore_Pixel,
					 NULL);
#else
    statusline = XtVaCreateManagedWidget("statusline",
					 xmTextFieldWidgetClass, parent,
					 XmNalignment, XmALIGNMENT_END,
					 XmNdepth, (XtArgVal) G_depth,
					 XmNbottomAttachment, XmATTACH_FORM,
					 XmNleftAttachment, XmATTACH_FORM,
					 XmNrightAttachment, XmATTACH_FORM,
					 XmNleftOffset, 1,
					 XmNrightOffset, 1,
					 XmNbottomOffset, 1,
					 XmNtopOffset, 0,
					 XmNcursorPositionVisible, False,
					 XmNautoShowCursorPosition, False,
					 XmNmarginWidth, 4,
					 XmNmarginHeight, 1,
					 XmNeditable, False,
					 XmNtraversalOn, False,
					 XmNvalue, g_string_savebuf,
					 NULL);

    /* Block processing of most interactive events on this widget, except
     * for button events that should navigate the page history.
     */
    XtInsertEventHandler(statusline,
			 KeyPressMask | KeyReleaseMask |
			 PointerMotionMask| PointerMotionHintMask |
			 ButtonMotionMask |
#if !MOTIF
			 ButtonPressMask | ButtonReleaseMask |
#endif
			 FocusChangeMask,
			 /* ButtonPressMask | ButtonReleaseMask | */
			 /* 			 PointerMotionMask| PointerMotionHintMask | */
			 /* 			 ButtonMotionMask | */
			 True, block_event_callback,
			 (XtPointer)0, 0);
#if MOTIF
    XtInsertEventHandler(statusline,
			 /* suboptimal, but needs to be release not press
			  * since we want to query the current cursor position,
			  * and that may not be set yet in the press event(?).
			  */
			 ButtonReleaseMask,
			 True, handle_statusline_event,
			 (XtPointer)g_string_savebuf, XtListTail);

#endif /* MOTIF */

#endif

    return statusline;
}


void
toggle_statusline(void)
{
#ifdef MOTIF
    if ((resource.expert_mode & XPRT_SHOW_STATUSLINE) == 0)
	XtUnmanageChild(statusline);
    else
	XtManageChild(statusline);

    set_menu(&resource.expert_mode, Act_set_expert_mode, check_resource_expert);
#else
    static Boolean initialized = False;
    static Boolean statusline_mapped = False;

    Boolean make_statusline_visible = False;
    Boolean make_statusline_invisible = False;
    
    if (!initialized) {
	statusline_mapped = (resource.expert_mode & XPRT_SHOW_STATUSLINE) != 0;
	initialized = True;
    }

    if ((resource.expert_mode & XPRT_SHOW_STATUSLINE) == 0) {
	if (statusline_mapped)
	    make_statusline_invisible = True;
    }
    else {
	if (!statusline_mapped)
	    make_statusline_visible = True;
    }

    if (make_statusline_invisible) {
	XtDestroyWidget(statusline);
	statusline_mapped = False;
    }
    if (make_statusline_visible) {
	static Dimension window_w, window_h;

	static Arg arg_wh[] = {
	    {XtNwidth, (XtArgVal) &window_w},
	    {XtNheight, (XtArgVal) &window_h},
	};
#ifdef MOTIF
	XtGetValues(globals.widgets.main_window, arg_wh, XtNumber(arg_wh));
#else
	XtGetValues(globals.widgets.vport_widget, arg_wh, XtNumber(arg_wh));
#endif
	XtVaSetValues(globals.widgets.vport_widget, XtNresizable, (XtArgVal)True, NULL);
	TRACE_GUI((stderr, "statusline: w %d, h %d", window_w, window_h));
	XtVaSetValues(globals.widgets.vport_widget, XtNwidth, (XtArgVal)window_w, XtNheight, (XtArgVal)window_h, NULL);
	TRACE_GUI((stderr, "after statusline"));
	create_statusline();
	statusline_mapped = True;
    }
#endif /* MOTIF */
}


/*------------------------------------------------------------
 *  handle_statusline_resize
 *
 *  Arguments:
 *	void
 *
 *  Returns:
 *	void
 *
 *  Purpose:
 *	Resize the statusline when the total window size changes.
 *
 *------------------------------------------------------------*/

void
handle_statusline_resize(void)
{
#ifndef MOTIF
    if ((resource.expert_mode & XPRT_SHOW_STATUSLINE) == 0) {
	return;
    }

    if (!statusline)
	return;

    /* apparently the x,y values of a widget can only be set at creation time, so
     * the following won't work:
     */

#if 0
    /*
      BROKEN  Position vport_h, clip_x, clip_w;
      BROKEN  static Position my_h = 0;
      BROKEN
      BROKEN  XtVaGetValues(globals.widgets.clip_widget,
      BROKEN                XtNx, &clip_x,
      BROKEN                XtNwidth, &clip_w,
      BROKEN                NULL);
      BROKEN  XtVaGetValues(globals.widgets.vport_widget,
      BROKEN                XtNheight, &vport_h,
      BROKEN                NULL);
      BROKEN
      BROKEN  XtUnmanageChild(statusline);
      BROKEN  XtVaSetValues(statusline,
      BROKEN                             XtNlabel, (XtArgVal) "",
      BROKEN                XtNwidth, clip_w,
      BROKEN                XtNx, clip_x - 1,
      BROKEN                XtNy, vport_h - my_h,
      BROKEN                XtNborderWidth, 1,
      BROKEN                XtNjustify, XtJustifyLeft,
      BROKEN                XtNborder, (XtArgVal) resource.fore_Pixel,
      BROKEN                NULL);
      BROKEN  XtManageChild(statusline);
      BROKEN  XFlush(DISP);
    */
#endif

    /* only this will: */
    XtDestroyWidget(statusline);
    create_statusline();
#endif
}




/*
 * clear statusline by printing an empty message to it.
 */


static void
clear_statusline(void)
{
    if ((resource.expert_mode & XPRT_SHOW_STATUSLINE) != 0) {
# ifdef MOTIF
	XmTextFieldSetString(statusline, " ");
# else
	XtVaSetValues(statusline, XtNlabel, " ", NULL);
# endif
	XFlush(DISP);
    }
    strcpy(g_string_savebuf, " ");
}


/* force a statusline update, no matter how busy the application is.
   Use this with care (only for important messages).
*/
void
force_statusline_update(void)
{
#ifdef MOTIF
    XmUpdateDisplay(globals.widgets.top_level);
#else
    XEvent event;
    XSync(DISP, False);
    while (XCheckMaskEvent(DISP, ExposureMask, &event))
        XtDispatchEvent(&event);
#endif /* MOTIF */
}


/*
 * timeout - if > 0, timeout in seconds after which the message will
 *	     be deleted again. If < 0, message will remain (until
 *	     another message overprints it)
 * fmt     - message, a C format string
 *
 * If expert mode is off, print <fmt> to the statusline; else, print
 * <fmt> to stdout, unless the `hushstdout' option is specified.
 */
static XtIntervalId clear_timeout_id = 0;

static void
clear_statusline_timer_proc(XtPointer client_data, XtIntervalId *id)
{
    UNUSED(client_data);
    UNUSED(id);
    
    if (clear_timeout_id) {
	clear_statusline();
	clear_timeout_id = 0;
    }
}

static void
internal_print_statusline(Boolean error,
			  statusTimerT timeout,
			  const char *old_content,
			  const char *fmt,
			  va_list argp)
{
    if (!XtIsRealized(globals.widgets.top_level)
	|| !initialized
	|| (resource.expert_mode & XPRT_SHOW_STATUSLINE) == 0) {

	/* only print errors to stdout */
	if (!error)
	    return;
	
	if (!resource.hush_stdout && strlen(fmt) > 0) { /* check for strlen since sometimes we clear the statusline
							   by printing "" to it, and we don't want that on stdout */
	    fprintf(stdout, "xdvi: ");
	    if (old_content != NULL)
		(void)fputs(old_content, stdout);
	    (void)vfprintf(stdout, fmt, argp);
	    fputc('\n', stdout);
	    fflush(stdout);
	}
    }
    else {
	char buf[MAX_LEN + 1];
	size_t offset = 0;
	
	if (old_content != NULL && old_content[0] != '\0') {
	    offset += strlen(old_content);
	    strncpy(buf, old_content, MAX_LEN);
	    /* append separating space */
	    if (strlen(old_content) < MAX_LEN - 1) {
		strcat(buf, " ");
		offset++;
	    }
	}
	VSNPRINTF(buf + offset, MAX_LEN - offset, fmt, argp);	/* just throw away strings longer than MAX_LEN */
	buf[MAX_LEN] = '\0'; /* terminate buf */
	/*
	 * save current contents of statusline so that toggling the statusline
	 * on and off will display the same text again
	 */
	strcpy(g_string_savebuf, buf);
#ifdef MOTIF
	XmTextFieldSetString(statusline, buf);
#else
	XtVaSetValues(statusline, XtNlabel, buf, NULL);
#endif
	/* 	fprintf(stderr, "timeout: %d, id: %ld\n", timeout, clear_timeout_id); */
	if (timeout > 0) {
	    timeout *= 1000;	/* convert to miliseconds */

	    if (clear_timeout_id) {
		/* 		fprintf(stderr, "clearing!\n"); */
		if (globals.debug & DBG_EVENT)
		    fprintf(stderr, "%s:%d: removing timeout %ld\n", __FILE__, __LINE__, clear_timeout_id);
		XtRemoveTimeOut(clear_timeout_id);
	    }
	    clear_timeout_id = XtAppAddTimeOut(globals.app, timeout,
					       clear_statusline_timer_proc, (XtPointer) NULL);
	}
    }
}

/*
 * Append the varargs-string `fmt' to the currnent contents of the statusline
 * erasing it after `timeout' seconds if timeout > 0, unless the current statusline
 * contents matches pattern - in that case, overwrite the contents.
 */
void
statusline_append(statusTimerT timeout, const char *pattern, const char *fmt, ...)
{
    const char *buf = NULL;
    va_list argp;

    if (XtIsRealized(globals.widgets.top_level) && initialized && (resource.expert_mode & XPRT_SHOW_STATUSLINE) != 0) {
	/* get current statusline contents */
#ifdef MOTIF
	XtVaGetValues(statusline, XmNvalue, &buf, NULL);
#else
	XtVaGetValues(statusline, XtNlabel, &buf, NULL);
#endif
    }

    while (buf != NULL && isspace((int)*buf)) /* skip spaces inserted by statusline appending */
	buf++;
    va_start(argp, fmt);

    if (buf != NULL && memcmp(buf, pattern, strlen(pattern)) == 0) {
	buf = NULL;
    }
    internal_print_statusline(false, timeout, buf, fmt, argp);
    va_end(argp);
}

/*
 * Print the varargs-string `fmt' to the currnent contents of the statusline,
 * erasing it after `timeout' seconds if timeout > 0.
 */

void
statusline_info(statusTimerT timeout, const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    /* for the time being, we don't differentiate between info/error
     * wrt. printing to stdout/stderr. We could probably at some point
     * remove the printing to stdout altogether (it's a bit un-GUIsh),
     * but then there's already an option 'hushstdout' to suppress it ...
     */
    internal_print_statusline(True, timeout, NULL, fmt, argp);
    va_end(argp);
}

void
statusline_error(statusTimerT timeout, const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    internal_print_statusline(True, timeout, NULL, fmt, argp);
    va_end(argp);
}

void
statusline_clear(void)
{
    statusline_info(STATUS_SHORT, "");
}

/*
 * Erase the contents of the statusline if it starts with 'pattern'.
 */
void
statusline_erase(const char *pattern)
{
    const char *buf = NULL;
    if (XtIsRealized(globals.widgets.top_level) && initialized && (resource.expert_mode & XPRT_SHOW_STATUSLINE) != 0) {
	/* get current statusline contents */
#ifdef MOTIF
	XtVaGetValues(statusline, XmNvalue, &buf, NULL);
#else
	XtVaGetValues(statusline, XtNlabel, &buf, NULL);
#endif

	if (strncmp(buf, pattern, strlen(pattern)) == 0) {
	    statusline_clear();
	}
    }
}

