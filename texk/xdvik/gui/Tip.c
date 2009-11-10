/*
 * Copyright (c) 2001-2004 Marcin Dalecki and others
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
 */

#include "xdvi-config.h"
#include "xdvi.h"

#include "Tip.h"
#include "xdvi-debug.h"
#include "util.h"

#ifdef MOTIF /* needed for `make depend' */

#ifndef UNUSED
#define UNUSED(x) ((void)(x))
#endif

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <Xm/XmP.h>

#include <Xm/PushB.h>
#include <X11/ShellP.h>

#define TIP_NUM 1024 /* FIXME: remove this hard-coded value */

typedef struct {
    int __empty;
} TipClassPart;

/* Full class record declaration */
typedef struct _TipClassRec {
    CoreClassPart core_class;
    CompositeClassPart composite_class;
    ShellClassPart shell_class;
    OverrideShellClassPart override_shell_class;
    TipClassPart tip_class;
} TipClassRec;

/* keep information about each widget we are keeping track of */
struct tip_context {
    Widget watched;		/* the widget we are watching */
    Window window;		/* Window of the object we are monitoring */
    TipWidget tw;		/* pointer back to the tip widget */
    Position abs_x, abs_y;
    Boolean active;		/* if False, tip is suppressed */
    char *text;			/* text to display */
    short size;			/* its size */
};

/* New fields for the widget record */
typedef struct {
    /* resources */
    Pixel foreground;
    XFontSet fontset;		/* the font for text in box */
    int waitPeriod;		/* the delay resource - pointer must be
				 * in watched widget this long before
				 * help is popped up - in millisecs
				 */
    unsigned int cwp;		/* after help is popped down - normal
				 * wait period is cancelled for this
				 * period - in millisecs
				 */

    /* private state */
    struct tip_context twl[TIP_NUM];	/* list of widgets we are liteClue-ing */
    Cardinal nr_twl;		/* number of widgets we have attached */
    Dimension font_width;	/* width of '1' character */
    Dimension font_height;	/* height of font, rows are spaced using this */
    Dimension font_baseline;	/* relative displacement to baseline from top */
    GC text_GC;			/* for drawing text */
    XtIntervalId tid;		/* holds timer id */
    Widget isup;		/* the help popup is up on this widget */
    Time HelpPopDownTime;	/* the time at which help popup
				   was popped down */
} TipPart;

/*
 * Full instance record declaration
 */
typedef struct _TipRec {
    CorePart core;
    CompositePart composite;
    ShellPart shell;
    OverrideShellPart override;
    TipPart tip;
} TipRec;

#define CheckWidgetClass(routine) \
	if (XtClass(w) != tipWidgetClass) \
		wrong_widget(routine)

static void initialize(Widget, Widget, ArgList, Cardinal *);
static Boolean set_values(Widget, Widget, Widget, ArgList, Cardinal *);
static void destroy(Widget);

/*
 * Widget resources: eg to set tip box background: *tipShell.background: yellow.
 */

#define offset(field) XtOffsetOf(TipRec, field)
static XtResource resources[] = {
    {XtNforeground, XtCForeground,
     XtRPixel, sizeof(Pixel), offset(tip.foreground),
     XtRString, "black"},
    {XtNfontSet, XtCFontSet,
     XtRFontSet, sizeof(XFontSet), offset(tip.fontset),
     XtRString, "fixed"},
    {XmNwaitPeriod, XmCWaitPeriod,
     XtRInt, sizeof(int), offset(tip.waitPeriod),
     XtRImmediate, (XtPointer)(ptrdiff_t)800},
    {XmNcancelWaitPeriod, XmCCancelWaitPeriod,
     XtRInt, sizeof(int), offset(tip.cwp),
     XtRImmediate, (XtPointer)(ptrdiff_t)250},
};
#undef offset

TipClassRec tipClassRec = {
    {
	/* superclass           */ (WidgetClass) & overrideShellClassRec,
	/* class_name           */ "Tip",
	/* widget size          */ (Cardinal) sizeof(TipRec),
	/* class_init           */ NULL,
	/* class_part_init      */ (XtWidgetClassProc) NULL,
	/* class_inited         */ (XtEnum) FALSE,
	/* initialize           */ (XtInitProc) initialize,
	/* init_hook            */ (XtArgsProc) NULL,
	/* realize              */ XtInheritRealize,
	/* actions              */ (XtActionList) 0,
	/* num_actions          */ (Cardinal) 0,
	/* resources            */ (XtResourceList) resources,
	/* num_resources        */ (Cardinal) XtNumber(resources),
	/* xrm_class            */ NULLQUARK,
	/* compress_motion      */ TRUE,
	/* compress_exposur     */ (XtEnum) FALSE,
	/* compress enterleave  */ TRUE,
	/* visibility_interest  */ FALSE,
	/* destroy              */ destroy,
	/* resize               */ XtInheritResize,
	/* expose,              */ XtInheritExpose,
	/* set_values           */ (XtSetValuesFunc) set_values,
	/* set_values_hook      */ (XtArgsFunc) NULL,
	/* set_values_almost    */ XtInheritSetValuesAlmost,
	/* get_values_hook      */ (XtArgsProc) NULL,
	/* accept_focus         */ XtInheritAcceptFocus,
	/* version              */ XtVersion,
	/* callback_private     */ (XtPointer) NULL,
	/* translations         */ XtInheritTranslations,
	/* query_geometry       */ XtInheritQueryGeometry,
	/* display_accelerator  */ XtInheritDisplayAccelerator,
	/* extension            */ (XtPointer) 0,
    },
    /* composite part */
    {
	/* geometry_manager     */ XtInheritGeometryManager,
	/* change_managed       */ XtInheritChangeManaged,
	/* insert_child         */ XtInheritInsertChild,
	/* delete_child         */ XtInheritDeleteChild,
	/* extension            */ NULL
    },
    /* Shell */
    {
	(XtPointer) NULL,
    },
    /* Override Shell */
    {
	0,
    },
    /* tip */
    {
	0,
    }
};

WidgetClass tipWidgetClass = (WidgetClass) & tipClassRec;

/*
 * The font_information is derived.
 */
static void compute_font_info(TipWidget cw)
{
    XRectangle ink;
    XRectangle logical;

    if (!cw->tip.fontset)
	return;
    XmbTextExtents(cw->tip.fontset, "1", 1, &ink, &logical);

    cw->tip.font_baseline = -logical.y;	/* y offset from top to baseline, don't
					   know why this is returned as
					   negative */

    cw->tip.font_width = logical.width;	/* the width and height of the object */

    cw->tip.font_height = logical.height;
    TRACE_GUI((stderr, "baseline: %d, width: %d, height: %d\n",
	       cw->tip.font_baseline, cw->tip.font_width, cw->tip.font_height));
}

/*
 * Creates the various graphic contexts we will need.
 */
static void create_GC(TipWidget cw)
{
    XtGCMask valuemask;
    XGCValues myXGCV;

    valuemask = GCForeground | GCBackground | GCFillStyle;
    myXGCV.foreground = cw->tip.foreground;
    myXGCV.background = cw->core.background_pixel;
    myXGCV.fill_style = FillSolid;

    if (cw->tip.text_GC)
	XtReleaseGC((Widget) cw, cw->tip.text_GC);
    cw->tip.text_GC = XtGetGC((Widget) cw, valuemask, &myXGCV);
}

/*
 * A routine to halt execution and force a core dump for debugging analysis
 * when a public routine is called with the wrong class of widget.
 */

static void wrong_widget(char *routine)
{
    XDVI_ABORT((stderr, "Wrong class of widget passed to %s", routine));
}

/*
 * Global list of shells for tips that are in use.
 */

static TipWidget *shells = NULL;
static int nr_shells = 0;

/****************************************************************************
 * Widget Methods
 */

static void initialize(Widget treq, Widget tnew, ArgList args,
		       Cardinal * nargs)
{
    TipWidget tw = (TipWidget) tnew;

    UNUSED(treq);
    UNUSED(args);
    UNUSED(nargs);
    
    tw->tip.text_GC = NULL;
    tw->tip.isup = NULL;
    tw->tip.HelpPopDownTime = 0;
    tw->tip.tid = (XtIntervalId) 0;
    tw->tip.nr_twl = 0;
    compute_font_info(tw);
    create_GC(tw);

    /* Add to our list of tip shells.
     */
    if (!shells)
	shells = (TipWidget *)XtMalloc(sizeof(TipWidget));
    else
	shells = (TipWidget *)XtRealloc((char *)shells,
					sizeof(TipWidget) * (nr_shells + 1));

    shells[nr_shells++] = tw;
}

static Boolean set_values(Widget _current, Widget _request, Widget _new,
			  ArgList args, Cardinal * nargs)
{
    TipWidget cw_new = (TipWidget) _new;
    TipWidget cw_cur = (TipWidget) _current;

    UNUSED(_request);
    UNUSED(args);
    UNUSED(nargs);
    
    /* values of cw_new->tip.cwp and
       cw_new->tip.waitPeriod are accepted without checking */

    if (cw_new->tip.foreground != cw_cur->tip.foreground
	|| cw_new->core.background_pixel !=
	cw_cur->core.background_pixel) {
	create_GC(cw_new);
    }
    return FALSE;
}

static void destroy(Widget w)
{
    TipWidget tw = (TipWidget) w;
    int i;
    Boolean copy = False;

    /* Remove this tip shell from our global list.
     */
    for (i = 0; i < nr_shells; ++i) {
	if (shells[i] == tw) {
	    copy = True;
	    --nr_shells;
	}
	if (copy && nr_shells)
	    shells[i] = shells[i + 1];
    }
    if (!nr_shells) {
	XtFree((char *) shells);
	shells = NULL;
    }
}

/****************************************************************************
 * Event handlers
 */

/* callback to popup the tip window
 */
static void timeout_event(XtPointer client_data, XtIntervalId *id)
{
#define HBorderPix 3
#define VBorderPix 3
    struct tip_context *obj = (struct tip_context *) client_data;
    TipWidget tw = obj->tw;
    Position abs_x, abs_y;
    int ptr_x, ptr_y;

    XRectangle ink;
    XRectangle logical;
    Position w_height, w_width;
    Widget w;
    
    UNUSED(id);

    TRACE_GUI((stderr, "timeout called!"));
    
    if (tw->tip.tid == (XtIntervalId) 0)
	return;			/* timeout was removed but callback happened
				   anyway */

    tw->tip.tid = (XtIntervalId) 0;
    if (obj->active == False)
	return;

    w = obj->watched;

    if (!XtIsManaged(w))
	return;

    { /* perform additional check that pointer is really still over the widget;
	 else, tooltips will sometimes pop up if window had received an Enter
	 event before (for some reason, not all Enters are followed by Leaves).
	 This is especially apparent when running xdvi from a remote display over
	 a slow connection.
      */
	Window root, child;
	int root_x, root_y;
	unsigned int keys_buttons;
	if (!XQueryPointer(DISP, RootWindowOfScreen(SCRN), &root, &child,
			   &root_x, &root_y, &ptr_x, &ptr_y, &keys_buttons))
	    return;

	TRACE_GUI((stderr, "Pointerlocate: %d, %d", root_x, root_y));
	
	XtVaGetValues(w, XtNheight, &w_height, XtNwidth, &w_width, NULL);
	XtTranslateCoords(w, 0, 0, &abs_x, &abs_y);
	
	TRACE_GUI((stderr, "Window: %d,%d - %d,%d",
		   abs_x, abs_y, abs_x + w_width, abs_y + w_height));
	
	if (root_x < abs_x || root_x > abs_x + w_width
	    || root_y < abs_y || root_y > abs_y + w_height) {
	    TRACE_GUI((stderr, "not really over toolbutton - returning!"));
	    return;
	}
    }

    /* position just below the pointer
     * (NOT the widget, in case the widget is large!)
     */
    ptr_y += 20;
    /*     abs_x += w_width / 2; */
    /*     abs_y += w_height; */
    
    XmbTextExtents(tw->tip.fontset, obj->text, obj->size, &ink, &logical);

    XtRealizeWidget((Widget)tw); /* so that setting the size etc. works */
	
    XtResizeWidget((Widget) tw,
		   2 * HBorderPix + logical.width,
		   2 * VBorderPix + tw->tip.font_height,
		   tw->core.border_width);
    TRACE_GUI((stderr, "Popup size: %d x %d (hborder: %d, vborder: %d)\n",
	       2 * HBorderPix + logical.width, 2 * VBorderPix + tw->tip.font_height,
	       HBorderPix, VBorderPix));
    XtMoveWidget((Widget)tw, ptr_x, ptr_y);

    XtPopup((Widget) tw, XtGrabNone);
    tw->tip.isup = obj->watched;

    XmbDrawImageString(XtDisplay((Widget) tw),
		       XtWindow((Widget) tw),
		       tw->tip.fontset,
		       tw->tip.text_GC,
		       HBorderPix,
		       VBorderPix + tw->tip.font_baseline,
		       obj->text, obj->size);
}

/*
 * Pointer enters watched widget, set a timer to popup the help.
 */
static void enter(struct tip_context *obj, XEvent * xevent,
		  XtAppContext app)
{
    TipWidget tw = obj->tw;
    XEnterWindowEvent *event = &xevent->xcrossing;
    int current_waitPeriod;

    /* this doesn't help against the Enter/Leave problem mentioned above,
       so it's not related to Widget creation ... */
    if (!XtIsManaged(obj->watched)) {
	TRACE_GUI((stderr, "%s:%d: Not yet managed!", __FILE__, __LINE__));
	return;
    }
    
    TRACE_GUI((stderr, "%s:%d: Enter!", __FILE__, __LINE__));
    
    if (obj->active == False)
	return;

    /* check for two enters in a row - happens when widget is
       exposed under a pop-up */
    if (tw->tip.tid != (XtIntervalId) 0)
	return;

    if (event->mode != NotifyNormal)
	return;

    /* it seems that this makes the tooltips somewhat unpredictable (they
       don't show when hovering fast over several buttons, then staying on
       one button); disabled this for the time being. */
    /*     if ((event->time - tw->tip.HelpPopDownTime) > tw->tip.cwp) */
    /* 	current_waitPeriod = tw->tip.waitPeriod; */
    /*     else */
    /* 	current_waitPeriod = 0; */

    /*     current_waitPeriod = tw->tip.waitPeriod; */
    current_waitPeriod =  resource.tooltips_wait_period;
    if (current_waitPeriod >= 0) {
	tw->tip.tid = XtAppAddTimeOut(app, current_waitPeriod, timeout_event,
				      (XtPointer) obj);
    }
}

/*
 * Remove timer if its pending. Then popdown help.
 */
static void leave(struct tip_context *obj, XEvent * xevent)
{
    TipWidget tw = obj->tw;
    XEnterWindowEvent *event = &xevent->xcrossing;

    TRACE_GUI((stderr, "%s:%d: Leave!", __FILE__, __LINE__));
    
    if (tw->tip.tid != (XtIntervalId) 0) {
	if (globals.debug & DBG_EVENT)
	    fprintf(stderr, "%s:%d: removing timeout %ld\n", __FILE__, __LINE__, tw->tip.tid);
	XtRemoveTimeOut(tw->tip.tid);
	tw->tip.tid = (XtIntervalId) 0;
    }

    if (obj->active == False)
	return;

    if (tw->tip.isup) {
	XtPopdown((Widget) tw);
	tw->tip.isup = NULL;
	tw->tip.HelpPopDownTime = event->time;
    }
}

/****************************************************************************
 * Public interface implementation.
 */

void TipAppHandle(XtAppContext app, XEvent *event)
{
    int i;

    if (!(event->type == EnterNotify
	  || event->type == MotionNotify
	  || event->type == LeaveNotify
	  || event->type == ButtonPress)) {
	return;
    }

    for (i = 0; i < nr_shells; ++i) {
	unsigned int j;

	for (j = 0; j < shells[i]->tip.nr_twl; ++j) {
	    if (event->xany.window == shells[i]->tip.twl[j].window) {
		if (event->type == EnterNotify)
		    enter(shells[i]->tip.twl + j, event, app);
		if (event->xany.type == LeaveNotify
		    || event->xany.type == MotionNotify /* FIXME: this doesn' work? */
		    /* might be useful to popdown tip when mouse is moved */
		    || event->xany.type == ButtonPress) {
		    leave(shells[i]->tip.twl + j, event);
		}
	    }
	}
    }
}

/*
 * This has to replace the XtAppMainLoop in the application using
 * tooltips.
 */
void TipAppMainLoop(XtAppContext app)
{
    XEvent event;

    for (;;) {
	XtAppNextEvent(app, &event);
	TipAppHandle(app, &event);
	XtDispatchEvent(&event);
    }
}

/*
 * Add a widget to be watched for tooltips.
 *
 * This function must be called after the widget has been realized!
 * Further on please make sure that this function will not be called twice
 * for one button!
 *
 * w            - tip widget
 * watch        - the widget to give tips for
 * text         - pointer to tip text
 */
void TipAddWidget(Widget w, Widget watch, const String text)
{
#define ROUTINE "TipAddWidget"
    TipWidget tw = (TipWidget) w;
    int i;

    CheckWidgetClass(ROUTINE);	/* make sure we are called with a tip widget */

    /* Make internal resource available via resource.tooltips_wait_period(_bak) and
     * resource.show_tooltips.
     */
    resource.tooltips_wait_period_bak = resource.tooltips_wait_period = ABS(tw->tip.waitPeriod);
    if (tw->tip.waitPeriod < 0) {
	resource.show_tooltips = False;
    }
    else if (!resource.show_tooltips) {
	if (resource.tooltips_wait_period == 0)
	    resource.tooltips_wait_period = -1;
	else
	    resource.tooltips_wait_period = -resource.tooltips_wait_period;
    }
    
    for (i = 0; i < nr_shells; ++i)
	if (shells[i] == tw) {
	    struct tip_context *obj;

	    if (tw->tip.nr_twl >= TIP_NUM) {
		XDVI_FATAL((stderr, "Too many tip widgets, cannot add new tip"));
		return;
	    }
	    obj = tw->tip.twl + tw->tip.nr_twl;
	    obj->text = XtNewString(text);
	    obj->size = strlen(text);
	    obj->watched = watch;
	    obj->window = XtWindow(watch);
	    obj->active = True;
	    obj->tw = tw;
	    tw->tip.nr_twl++;
	}
#undef ROUTINE
}

#else
/* silence `empty compilation unit' warnings */
static void bar(void); static void foo(void) { bar(); } static void bar(void) { foo(); }
#endif /* MOTIF */
