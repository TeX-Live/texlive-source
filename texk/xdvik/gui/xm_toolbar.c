/*
 * Copyright (c) 2001 Marcin Dalecki
 * Copyright (c) 2002-2004 the xdvik development team
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

/*
 * Tool bar implementation for the Motif widget set.
 */

#include "xdvi-config.h"
#include "xdvi.h"

#include "pagesel.h"
#include "help-window.h"
#include "message-window.h"
#include "kpathsea/tex-file.h"
#include "kpathsea/expand.h"
#include "statusline.h"
#include "dvi-init.h"
#include "events.h"
#include "dvi-draw.h"
#include "xm_menu.h"
#include "xm_toolbar.h"
#include "util.h"
#include "x_util.h"
#include "Tip.h"

#include <stdlib.h>

#include "xdvi-config.h"
#include "c-openmx.h"

/* default toolbar pixmap file */
#include "pixmaps/toolbar.xpm"

#ifdef MOTIF
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>

#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/Frame.h>
#include <Xm/PushB.h>
#include <Xm/Form.h>
#include <Xm/Separator.h>
#include <Xm/CascadeB.h>
#include <Xm/MainW.h>
#include <Xm/DialogS.h>
#include <Xm/MessageB.h>
#include <Xm/Label.h>

#if defined(HAVE_X11_XPM_H)
# include <X11/xpm.h>
#elif defined(HAVE_XPM_H)
# include <xpm.h>
#elif defined(HAVE_XM_XPMP_H)
# include <Xm/XpmP.h>
#endif

#include <sys/stat.h>

/*
 * The following needs to be keept in sync with the actual pixmap sizes in
 * tools.xpm. When editing the pixmaps, take care that the symbolic names
 * are not messed up (e.g. it works with "pixmap" from ftp.x11.org, but
 * not with xv).
 */

#define PIXMAP_WIDTH	18
#define PIXMAP_HEIGHT	18
/*  #define PIXMAP_COUNT	13 */


static Widget tool_bar_frame;

#define SEP_CHAR ':' /* character separating entries in translations lines */

void
toggle_scrollbars(void)
{
    Widget x_bar = XtNameToWidget(globals.widgets.main_window, "HorScrollBar");
    Widget y_bar = XtNameToWidget(globals.widgets.main_window, "VertScrollBar");
    
    if ((resource.expert_mode & XPRT_SHOW_SCROLLBARS) == 0) {
	XtUnmanageChild(x_bar);
	XtUnmanageChild(y_bar);
    }
    else {
	XtManageChild(x_bar);
	XtManageChild(y_bar);
    }

    set_menu(&resource.expert_mode, Act_set_expert_mode, check_resource_expert);
}

void
toggle_toolbar(void)
{
#if !HAVE_XPM
    statusline_error(STATUS_LONG,
		     "Compiled without XPM support; no toolbar available.");
#else
    if (resource.toolbar_unusable) {
	statusline_error(STATUS_LONG,
			 "Toolbar pixmap file not found; toolbar is disabled.");
	return;
    }

    if ((resource.expert_mode & XPRT_SHOW_TOOLBAR) == 0)
	XtUnmanageChild(tool_bar_frame);
    else
	XtManageChild(tool_bar_frame);

    set_menu(&resource.expert_mode, Act_set_expert_mode, check_resource_expert);

#endif /* not HAVE_XPM */
}

typedef enum { TB_BUTTON, TB_SEPARATOR } toolbarButtonT;

/* global array of button infos */
static struct toolbar_button_info {
    Widget button;
    toolbarButtonT type;
    char *tip;
} *toolbar_buttons = NULL;

#if HAVE_XPM  /* remainder of file: toolbar is disabled without XPM support */

/* to save current values to */
static Pixel m_background = 0, m_top_shadow_color = 0, m_bottom_shadow_color = 0;

/* to save resource value of XmNshadowThickness before overriding it if
   toolbar_buttons_raised is false; it will be used to raise the button on enter:
*/
static int m_shadow_thickness; 
extern Boolean get_int_arg(String *param, Cardinal *num_params, int *res);

/* indexes for named colors */
enum {
    BACKGROUND,
    FOREGROUND,
    BOTTOM_SHADOW,
    TOP_SHADOW,
    HIGHLIGHT
};

/* save info about buttons that need to change state */
static struct state_buttons_info {
    Widget back_button;			/* insensitive when current page is last page */
    Widget zoom_in_button;		/* insensitive when shrinkfactor is 1 */
    Widget forward_button;		/* insensitive when current page is first page */
    Widget hyperref_back_button;	/* insensitive when at end of href history */
    Widget hyperref_forward_button;	/* insensitive when at begin of href history */
} m_button_info;

static XtCallbackRec command_call[] = {
    { handle_command, NULL },
    { NULL, NULL },
};

static void
set_button_sensitivity(Widget w, Boolean sensitive)
{
    if (w == NULL) { /* if button hadn't been initialized properly */
	return;
    }
    XtVaSetValues(w, XmNsensitive, sensitive, NULL);
    /* also remove `raised' property on mouse over */
    if (!sensitive && !resource.toolbar_buttons_raised) {
	Dimension x, y, wd, ht;
	Pixel foreground;
	static GC backgroundGC = NULL;
	if (!XtIsRealized(w))
	    return;
	XtVaGetValues(w,
		      XmNx, &x,
		      XmNy, &y,
		      XmNwidth, &wd,
		      XmNheight, &ht,
		      NULL);
	if (backgroundGC == NULL) {
	    XtVaGetValues(w, XmNforeground, &foreground, NULL);
	    backgroundGC = set_or_make_gc(NULL, GXcopy, m_background, foreground);
	}
	XtVaSetValues(w,
		      XmNtopShadowColor, m_background,
		      XmNbottomShadowColor, m_background,
		      NULL);
#if 0
	fprintf(stderr, "drawing on %d,%d\n", x, y);
	XFillRectangle(XtDisplay(w), XtWindow(w), globals.gc.high, /* backgroundGC, */
		       x, y, wd, ht);
#endif
    }
    XSync(DISP, False);
}

void
tb_check_navigation_sensitivity(int pageno)
{
    if (!XtIsRealized(globals.widgets.top_level) || (resource.expert_mode & XPRT_SHOW_TOOLBAR) == 0)
	return;

    set_button_sensitivity(m_button_info.forward_button, pageno == total_pages - 1 ? False : True);
    set_button_sensitivity(m_button_info.back_button, pageno == 0 ? False : True);
}

void
tb_set_htex_back_sensitivity(Boolean sensitive)
{
    if (!XtIsRealized(globals.widgets.top_level) || (resource.expert_mode & XPRT_SHOW_TOOLBAR) == 0)
	return;
    set_button_sensitivity(m_button_info.hyperref_back_button, sensitive);
}

void
tb_set_htex_forward_sensitivity(Boolean sensitive)
{
    if (!XtIsRealized(globals.widgets.top_level) || (resource.expert_mode & XPRT_SHOW_TOOLBAR) == 0)
	return;
    
    set_button_sensitivity(m_button_info.hyperref_forward_button, sensitive);
}

void
tb_set_pagehist_back_sensitivity(Boolean sensitive)
{
    if (!XtIsRealized(globals.widgets.top_level) || (resource.expert_mode & XPRT_SHOW_TOOLBAR) == 0)
	return;
    set_button_sensitivity(m_button_info.hyperref_back_button, sensitive);
}

void
tb_set_pagehist_forward_sensitivity(Boolean sensitive)
{
    if (!XtIsRealized(globals.widgets.top_level) || (resource.expert_mode & XPRT_SHOW_TOOLBAR) == 0)
	return;
    
    set_button_sensitivity(m_button_info.hyperref_forward_button, sensitive);
}

void
tb_set_zoom_sensitivity(Boolean sensitive)
{
    if (!XtIsRealized(globals.widgets.top_level) || (resource.expert_mode & XPRT_SHOW_TOOLBAR) == 0)
	return;
    
    set_button_sensitivity(m_button_info.zoom_in_button, sensitive);
}

#if 0
static void
search_callback(Widget w,
		XtPointer client_data,
		XmAnyCallbackStruct *call_data)
{
    UNUSED(w);
    UNUSED(client_data);
    UNUSED(call_data);
    
    popup_message(globals.widgets.top_level,
		  MSG_ERR, NULL, "Sorry, not yet implemented");
}
#endif /* 0 */



/*
 * If successful, this extracts a square pixmap from resource.toolbar_pixmap_file
 * and returns True. 
 * In this case, the pixmap `sen' will contain the actual
 * pixmap and `insen' will contain a drawn pixmap for the insensitive state of
 * the button. We are emulating a shaded state derived from the monochrome 'm'
 * color attributes of the XPM.
 *
 * If unsuccessful, it returns False, and the Pixmap pointers are undefined.
 */

static Boolean
create_pixmap(Widget parent, int iconidx, Pixmap *sen, Pixmap *insen)
{
    static Boolean first_time = True;
    static Window rootWindow;
    static XpmColorSymbol color[5] = {
	{"none", "none", 0},
	{"iconColor1", NULL, 0},
	{"bottomShadowColor", NULL, 0},
	{"topShadowColor", NULL, 0},
	{"selectColor", NULL, 0}
    };
    static int screenNum;
    static Pixmap tools_map;
    static Pixmap tools_mask;
    static Pixmap shade_map;
    static Pixmap shade_mask;
    
    int		    status = 0;
    XpmAttributes   attr;
    Pixmap	    map;
    Pixmap	    mask;
    static String pixmap_file_path = NULL; /* note: never free()d */
	
    ASSERT(iconidx >= 0, "index must be positive");
    
    if (first_time) {
	/* FIXME: We use a dummy window here to get the correct depth/visual for the
	   pixmap creation (cant use globals.widgets.top_level since it's not realized
	   yet and realizing it now would result in wrong dimensions) ... */
	Widget dummy = XtVaAppCreateShell("xdvi", "Xdvi",
					  transientShellWidgetClass, DISP,
					  XtNdepth, G_depth,
					  XtNvisual, G_visual,
					  XtNcolormap, G_colormap,
					  XtNmappedWhenManaged, False,
					  NULL);
	XtRealizeWidget(dummy); /* note: doesn't pop it up */
	rootWindow = XtWindow(dummy);
	screenNum = DefaultScreen(XtDisplay(globals.widgets.top_level));
	ASSERT(rootWindow != 0, "");
	XtVaGetValues(parent,
		      XmNbackground, &color[BACKGROUND].pixel,
		      XmNforeground, &color[FOREGROUND].pixel,
		      XmNbottomShadowColor, &color[BOTTOM_SHADOW].pixel,
		      XmNtopShadowColor, &color[TOP_SHADOW].pixel,
		      XmNhighlightColor, &color[HIGHLIGHT].pixel,
		      NULL);
	/* try to locate the XPM file with the toolbar pixmaps */
	pixmap_file_path = XtResolvePathname(DISP,
					     "pixmaps",
					     resource.toolbar_pixmap_file,
					     (String)NULL,		/* suffix */
					     (String)NULL,		/* use default path */
					     (Substitution)NULL,	/* substitutions */
					     0,				/* number of substitutions */
					     (XtFilePredicate)NULL);	/* return True iff file exists */

	TRACE_GUI((stderr, "pixmap file search via XtResolvePathname: %s => %s",
		   resource.toolbar_pixmap_file, pixmap_file_path ? (char*)pixmap_file_path : "<NULL>"));
	if (pixmap_file_path == NULL) {
	    pixmap_file_path = (String)kpse_find_file(resource.toolbar_pixmap_file,
						      kpse_program_text_format,
						      0);
	    TRACE_GUI((stderr,
		       "pixmap file search via kpse_find_file: %s => %s",
		       resource.toolbar_pixmap_file,
		       pixmap_file_path ? (char*)pixmap_file_path : "<NULL>"));
	    if (pixmap_file_path == NULL) {
		TRACE_GUI((stderr, "No installed toolbar pixmap found, using built-in pixmap."));
	    }
	}
    }
    
    /* Setup the color subsititution table */
    attr.valuemask = XpmColorSymbols | XpmCloseness | XpmColormap | XpmDepth | XpmVisual;
    attr.closeness = 65535;	/* accuracy isn't crucial */
    attr.colorsymbols = color;
    attr.numsymbols = XtNumber(color);
    attr.visual = G_visual;
    attr.colormap = G_colormap;
    attr.depth = G_depth;
    
    /* Create the "sensitive" pixmap */
    if (!tools_map) {
	if (pixmap_file_path != NULL) {
	    status = XpmReadFileToPixmap(XtDisplay(globals.widgets.top_level), rootWindow,
					 pixmap_file_path, &tools_map, &tools_mask, &attr);
	}
	else {
	    status = XpmCreatePixmapFromData(XtDisplay(globals.widgets.top_level), rootWindow,
					     (char **)toolbar_xpm, &tools_map, &tools_mask, &attr);
	}
    }
    else
	status = XpmSuccess;

    map = tools_map;
    mask = tools_mask;

    if (status == XpmSuccess) {
	static Pixmap tmp_mask;
	static GC gc;

	if (first_time) {
	    tmp_mask = XCreatePixmap(XtDisplay(globals.widgets.top_level), rootWindow, PIXMAP_WIDTH, PIXMAP_HEIGHT, 1);
	    gc = XCreateGC(XtDisplay(globals.widgets.top_level), tmp_mask, 0, NULL);
	}
	XCopyArea(XtDisplay(globals.widgets.top_level),
		  mask, tmp_mask, gc, iconidx * PIXMAP_WIDTH, 0, PIXMAP_WIDTH, PIXMAP_HEIGHT, 0, 0);

	mask = tmp_mask;
    }
    else { /* something went wrong */
	popup_message(globals.widgets.top_level,
		      MSG_ERR,
		      "Something's wrong with your XPM file - "
		      "try to load it into an image editor and fix the problem.",
		      "Xpm error: %s - switching toolbar off.",
		      XpmGetErrorString(status));
	sen = insen = NULL;
	resource.expert_mode ^= XPRT_SHOW_TOOLBAR;
	return False;
    }

    XpmFreeAttributes(&attr);
    
    if (map != 0) {
	static GC back_gc, bots_gc;

	if (first_time) {
	    XGCValues   gcvalues;

	    gcvalues.foreground = color[BACKGROUND].pixel;
	    back_gc = XCreateGC(XtDisplay(globals.widgets.top_level), rootWindow, GCForeground, &gcvalues);

	    gcvalues.foreground = color[BOTTOM_SHADOW].pixel;
	    bots_gc = XCreateGC(XtDisplay(globals.widgets.top_level), rootWindow, GCForeground, &gcvalues);
	}

	/* Need to create new Pixmaps with the mask applied. */
	XSetClipMask(XtDisplay(globals.widgets.top_level), bots_gc, mask);

	/* Create the "sensitive" pixmap. */
	*sen = XCreatePixmap(XtDisplay(globals.widgets.top_level), rootWindow, PIXMAP_WIDTH, PIXMAP_HEIGHT,
			     G_depth);
	XFillRectangle(XtDisplay(globals.widgets.top_level), *sen, back_gc, 0, 0, PIXMAP_WIDTH, PIXMAP_HEIGHT);
	if (iconidx != -1)
	    XCopyArea(XtDisplay(globals.widgets.top_level), map, *sen, bots_gc,
		      iconidx * PIXMAP_WIDTH, 0, PIXMAP_WIDTH, PIXMAP_HEIGHT, 0, 0);
	else
	    XCopyArea(XtDisplay(globals.widgets.top_level), map, *sen, bots_gc,
		      0, 0, PIXMAP_WIDTH, PIXMAP_HEIGHT, 0, 0);

	if (iconidx == -1)
	    XFreePixmap(XtDisplay(globals.widgets.top_level), map);

	/* Create the "insensitive" pixmap. */
	if (insen != NULL) {
	    Pixmap map;
	    Pixmap mask;

	    attr.valuemask = XpmColorSymbols | XpmCloseness | XpmColorKey | XpmColormap | XpmDepth | XpmVisual;
	    attr.closeness = 65535;	/* accuracy isn't crucial */
	    attr.colorsymbols = color;
	    attr.numsymbols = XtNumber(color);
	    attr.color_key = XPM_MONO;
	    attr.visual = G_visual;
	    attr.colormap = G_colormap;
	    attr.depth = G_depth;


	    if (!shade_map) {
		if (pixmap_file_path != NULL) {
		    status = XpmReadFileToPixmap(XtDisplay(globals.widgets.top_level), rootWindow,
						 pixmap_file_path, &shade_map, &shade_mask, &attr);
		}
		else {
		    status = XpmCreatePixmapFromData(XtDisplay(globals.widgets.top_level), rootWindow,
						     (char **)toolbar_xpm, &shade_map, &shade_mask, &attr);
		}
	    }
	    else
		status = XpmSuccess;

	    map = shade_map;
	    mask = shade_mask;

	    if (status == XpmSuccess) {
		static Pixmap tmp_mask;
		static GC gc;

		if (first_time) {
		    tmp_mask = XCreatePixmap(XtDisplay(globals.widgets.top_level), rootWindow,
					     PIXMAP_WIDTH, PIXMAP_HEIGHT, 1);
		    gc = XCreateGC(XtDisplay(globals.widgets.top_level), tmp_mask, 0, NULL);
		}

		XCopyArea(XtDisplay(globals.widgets.top_level), mask, tmp_mask, gc,
			  iconidx * PIXMAP_WIDTH, 0,
			  PIXMAP_WIDTH, PIXMAP_HEIGHT,
			  0, 0);

		mask = tmp_mask;
	    }
	    else { /* something went wrong */
		popup_message(globals.widgets.top_level,
			      MSG_ERR,
			      "Something's wrong with your XPM file - "
			      "try to load it into an image editor and fix the problem.",
			      "Xpm error: %s - switching toolbar off.",
			      XpmGetErrorString(status));
		sen = insen = NULL;
		resource.expert_mode ^= XPRT_SHOW_TOOLBAR;
		return False;
	    }

	    if (mask != 0) {
		static GC   tops_gc;

		if (first_time) {
		    XGCValues   gcvalues;

		    gcvalues.foreground = color[TOP_SHADOW].pixel;
		    tops_gc = XCreateGC(XtDisplay(globals.widgets.top_level), rootWindow, GCForeground, &gcvalues);
		}

		/* Need to create new Pixmaps with the mask applied. */
		XSetClipMask(XtDisplay(globals.widgets.top_level), bots_gc, mask);
		XSetClipMask(XtDisplay(globals.widgets.top_level), tops_gc, mask);
		XSetClipOrigin(XtDisplay(globals.widgets.top_level), tops_gc, 1, 1);

		*insen = XCreatePixmap(XtDisplay(globals.widgets.top_level), rootWindow, PIXMAP_WIDTH, PIXMAP_HEIGHT,
				       G_depth);

		XFillRectangle(XtDisplay(globals.widgets.top_level), *insen, back_gc, 0, 0,
			       PIXMAP_WIDTH, PIXMAP_HEIGHT);
		XFillRectangle(XtDisplay(globals.widgets.top_level), *insen, tops_gc, 1, 1,
			       PIXMAP_WIDTH - 1, PIXMAP_HEIGHT - 1);
		XFillRectangle(XtDisplay(globals.widgets.top_level), *insen, bots_gc, 0, 0, PIXMAP_WIDTH, PIXMAP_HEIGHT);

		if (iconidx == -1)
		    XFreePixmap(XtDisplay(globals.widgets.top_level), map);
	    }

	    XpmFreeAttributes(&attr);
	}
    }
    
    first_time = False;
    return True;
}

/*
 * If successful, this returns True, and sets *button to a button with
 * a square pixmap on it (which is cut out form `index' position in
 * resource.toolbar_pixmap_file).  If unsuccessful, it returns false,
 * and *button is undefined.
 */
static Boolean
create_toolbar_button(Widget parent, Widget *button,
		      const Pixmap *image_sens, const Pixmap *image_insens)
{
    Boolean sensitive = True; /* dummy */
    *button = XtVaCreateManagedWidget("button", xmPushButtonWidgetClass, parent,
				      XmNhighlightOnEnter, True,
				      XmNlabelPixmap, *image_sens,
				      XmNlabelInsensitivePixmap, *image_insens,
				      XmNsensitive, sensitive,
				      XmNlabelType, XmPIXMAP,
				      NULL);
    XtVaGetValues(*button, XmNshadowThickness, &m_shadow_thickness, NULL);
    if (!resource.toolbar_buttons_raised) {
	if (m_background == 0) { /* initialize values */
	    XtVaGetValues(*button,
			  /* should we rather get background of parent widget? */
			  XmNbackground, &m_background,
			  XmNtopShadowColor, &m_top_shadow_color,
			  XmNbottomShadowColor, &m_bottom_shadow_color,
			  NULL);
	}
	/* remove shadows, setting them later when mouse enters button */
	XtVaSetValues(*button,
		      XmNtopShadowColor, m_background,
		      XmNbottomShadowColor, m_background,
		      NULL);
    }
    return True;
}

static void
create_toolbar_separator(Widget parent, Widget *separator, int width)
{
    *separator = XtVaCreateManagedWidget("toolbarSeparator",
					 xmSeparatorWidgetClass, parent,
					 XmNseparatorType, XmNO_LINE,
					 XmNminWidth, width,
					 XmNwidth, width,
					 XmNorientation, XmVERTICAL,
					 XmNleftAttachment, XmATTACH_WIDGET,
					 XmNrightAttachment, XmATTACH_WIDGET,
					 XmNtopAttachment, XmATTACH_FORM,
					 XmNbottomAttachment, XmATTACH_FORM,
					 NULL);
}

/* taken from shadow_trick in WlToolBar.c, mgv-3.1.5:
   Get a raised behaviour for armed buttons, by explicitly setting
   a shadow on entry and removing it on leave.
*/
static void
enter_leave(Widget w, XtPointer closure, XEvent *event, Boolean *cont)
{
    char *tooltip = (char *)closure;
    XCrossingEvent *ev = (XCrossingEvent *)event;
    static Boolean entered = False; /* to skip double leaves */

    UNUSED(cont);

    if (ev->type == EnterNotify) {
	if (!resource.toolbar_buttons_raised) {
	    /* draw shadows */
	    XtVaSetValues(w,
			  XmNtopShadowColor, m_top_shadow_color,
			  XmNbottomShadowColor, m_bottom_shadow_color,
			  NULL);
	}
	entered = True;
	if (resource.tooltips_in_statusline) {
	    statusline_info(STATUS_SHORT, tooltip);
	}
    }
    else if (ev->type == LeaveNotify && !resource.toolbar_buttons_raised) {
	/* remove shadows */
	if (!entered)
	    return;

	XtVaSetValues(w,
		      XmNtopShadowColor, m_background,
		      XmNbottomShadowColor, m_background,
		      NULL);
	entered = False;
    }
}

/*
  save info about special buttons in m_button_info
  (see definition of that for details)
*/
static void
button_info_save(struct xdvi_action *action, Widget w)
{
    if (action->proc == Act_back_page
      && action->num_params > 0 && strcmp(action->params[0], "1") == 0) {
	m_button_info.back_button = w;
    }
    else if (action->proc == Act_forward_page
      && action->num_params > 0 && strcmp(action->params[0], "1") == 0) {
	m_button_info.forward_button = w;
    }
    else if (action->proc == Act_pagehistory_back) {
	set_button_sensitivity(w, False);
	m_button_info.hyperref_back_button = w;
    }
    else if (action->proc == Act_pagehistory_forward) {
	set_button_sensitivity(w, False);
	m_button_info.hyperref_forward_button = w;
    }
    else if (action->proc == Act_set_shrink_factor
	     && action->num_params > 0 && action->params[0][0] == '+') {
	m_button_info.zoom_in_button = w;
    }
}

#endif /* HAVE_XPM */

/*
 * Create a toolbar with buttons, return toolbar widget.
 */
Widget
create_toolbar(Widget parent, Widget menu_bar)
{
#if HAVE_XPM
    size_t alloc_len = 0, n;
    size_t alloc_step = 16;
    const char *c_ptr, *e_ptr;
#endif /* HAVE_XPM */
    Widget tool_bar;
    resource.toolbar_unusable = False;
    
    tool_bar_frame = XtVaCreateWidget("toolBarFrame",
				      xmFrameWidgetClass, parent,
				      XmNshadowType, XmSHADOW_OUT,
				      XmNleftAttachment, XmATTACH_FORM,
				      XmNrightAttachment, XmATTACH_FORM,
				      XmNtopAttachment, XmATTACH_WIDGET,
				      XmNtopWidget, menu_bar,
				      NULL);

    tool_bar = XtVaCreateManagedWidget("toolBar",
				       xmRowColumnWidgetClass, tool_bar_frame,
				       XmNchildType, XmFRAME_WORKAREA_CHILD,
				       XmNrowColumnType, XmWORK_AREA,
				       XmNorientation, XmHORIZONTAL,
				       XmNtraversalOn, False,
				       XmNisHomogeneous, False,
				       XmNpacking, XmPACK_TIGHT,
				       XmNspacing, 0, /* override to use SEPARATOR(n) instead */
				       XmNadjustLast, True,
				       NULL);

#if HAVE_XPM
    /* parse toolbar_translations, create the widgets and assign the actions */
    for (n = 0, c_ptr = resource.toolbar_translations;
	 c_ptr != NULL && *c_ptr != '\0';
	 c_ptr = e_ptr, n++) {
	char **line_items = NULL;
	int extra_space;
	size_t len, curr, item_count = 0;
	
	if ((e_ptr = strchr(c_ptr, '\n')) == NULL
	    /* ... and in case last line doesn't end with \n ... */
	    && (e_ptr = strchr(c_ptr, '\0')) == NULL) {
	    break;
	}

	if (e_ptr == c_ptr) {
	    XDVI_WARNING((stderr, "Skipping empty line in toolbarTranslations resource."));
	    e_ptr++;
	    continue;
	}
	len = e_ptr - c_ptr;
	TRACE_GUI((stderr, "LEN %lu: |%.*s|", (unsigned long)len, (int)len, c_ptr));

	line_items = split_line(c_ptr, SEP_CHAR, 0, len, &item_count);

	if (globals.debug & DBG_GUI) {
	    int k;
	    for (k = 0; line_items[k] != NULL; k++) {
		fprintf(stderr, "ITEM %d of %lu: |%s|\n", k, (unsigned long)item_count, line_items[k]);
	    }
	}
	while (alloc_len <= n + 1) {
	    alloc_len += alloc_step;
	    toolbar_buttons = xrealloc(toolbar_buttons, alloc_len * sizeof *toolbar_buttons);
	}
	    
	if (item_count == 1 && sscanf(line_items[0], "SPACER(%d)", &extra_space) == 1) {
	    TRACE_GUI((stderr, "creating spacer of witdh %d at %lu", extra_space, (unsigned long)n));
 	    create_toolbar_separator(tool_bar, &(toolbar_buttons[n].button), extra_space);
	    toolbar_buttons[n].type = TB_SEPARATOR;
	}
	else if (item_count == 4) {
	    Pixmap sens, insens;
	    int idx = strtoul(line_items[0], (char **)NULL, 10);
	    struct xdvi_action *action;

	    sens = insens = 0; /* make compiler happy ... */

	    TRACE_GUI((stderr, "creating pixmap at %d", idx));
	    if (!create_pixmap(tool_bar, idx, &sens, &insens)) {
		free(toolbar_buttons);
		toolbar_buttons = NULL;
		break;
	    }
	    TRACE_GUI((stderr, "creating button %ld", (unsigned long)n));
	    if (!create_toolbar_button(tool_bar, &(toolbar_buttons[n].button), &sens, &insens)) {
		free(toolbar_buttons);
		toolbar_buttons = NULL;
		break;
	    }
	    toolbar_buttons[n].type = TB_BUTTON;

	    if (compile_action(line_items[3], &action)) {
		char *long_tooltip = xstrdup(line_items[1]);
		toolbar_buttons[n].tip = xstrdup(line_items[2]);
		/* char *short_tooltip = xstrdup(line_items[2]); */
		command_call[0].closure = (XtPointer) action;

		/*
		  eventually save this widget in list of `special' buttons
		  that need to toggle between sensitive/insensitive
		*/
		button_info_save(action, toolbar_buttons[n].button);

		XtVaSetValues(toolbar_buttons[n].button, XmNactivateCallback, (XtArgVal)command_call, NULL);
		XtAddEventHandler(toolbar_buttons[n].button,
				  EnterWindowMask | LeaveWindowMask,
				  False,
				  enter_leave,
				  long_tooltip);
	    }
	    else {
		XDVI_WARNING((stderr, "Invalid action \"%s\" in toolbarTranslations resource:\n\"%.*s\"",
			      line_items[3], (int)len, c_ptr));
	    }
	}
	else {
	    XDVI_WARNING((stderr, "Skipping malformed line \"%.*s\" in toolbarTranslations resource "
			  "(%lu instead of 4 items).",
			  (int)len, c_ptr, (unsigned long)item_count));
	    toolbar_buttons[n].button = 0;
	}

	for (curr = 0; curr < item_count; curr++) {
	    free(line_items[curr]);
	}
	free(line_items);
	line_items = NULL;
	
	if (*e_ptr != '\0')
	    e_ptr++;
    }
#else
    if ((resource.expert_mode & XPRT_SHOW_SCROLLBARS) != 0) {
	XDVI_WARNING((stderr, "This version has been compiled without XPM support. "
		      "Disabling the toolbar, which needs XPM."));
    }
#endif /* HAVE_XPM */

    if (toolbar_buttons == NULL) {
	resource.toolbar_unusable = True;
	resource.expert_mode ^= XPRT_SHOW_TOOLBAR;
    }
    else {
#if HAVE_XPM	
	toolbar_buttons[n].button = 0; /* terminate info */
#endif
    }
    return tool_bar;
}

/*
 * Add the tips to the toolbar.  This has to be done after the toolbar has been
 * realized.
 */
void
create_tips(Widget toplevel)
{
#if HAVE_XPM
    Widget tip_shell;
    
    int i;

    if (resource.toolbar_unusable) /* don't create tips in this case */
	return;
    
    tip_shell = XtVaCreatePopupShell("tipShell", tipWidgetClass,
				     toplevel, XtNwidth, 1, XtNheight, 1,
				     NULL);

    for (i = 0; toolbar_buttons[i].button != 0; i++) {
	if (toolbar_buttons[i].type == TB_BUTTON) {
	    TipAddWidget(tip_shell, toolbar_buttons[i].button, toolbar_buttons[i].tip);
	}
    }
#else
    UNUSED(toplevel);
#endif /* HAVE_XPM */
}


#else /* MOTIF */
/* silence "empty compilation unit" and "`toolbar_xpm' defined but not used" warnings */
static void bar(void); static void foo(void) { UNUSED(toolbar_xpm); bar(); } static void bar(void) { foo(); }
#endif /* MOTIF */
