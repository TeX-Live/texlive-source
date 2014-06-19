/*
 * Copyright (c) 2001-2004 the xdvik development team
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
 * Menu bar implementation for the Athena widget set.
 */
#include "xdvi-config.h"
#include "xdvi.h"

#include "c-openmx.h"
#include "events.h"
#include "dvi-draw.h"
#include "dvi-init.h"
#include "statusline.h"
#include "pagesel.h"
#include "util.h"
#include "x_util.h"
#include "xaw_menu.h"
#include "message-window.h"
#include "my-snprintf.h"
#include "filehist.h"
#include "menu.h"

#ifndef MOTIF /* entire file */

#include <ctype.h>

#include <X11/Intrinsic.h>
#include <X11/Xatom.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>	/* needed for def. of XtNiconX */

#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/Sme.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/SmeLine.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/Text.h>
#include <X11/Xaw/Panner.h>	
#include <X11/Xaw/Porthole.h>	
#include <X11/Xaw/Command.h>

#ifndef	MAX
# define MAX(i, j)       ( (i) > (j) ? (i) : (j) )
#endif


static Widget line_widget, panel_widget;

/* width of button panel */
static int m_panel_width = 0;

/* used for communication with the pagelist in xaw_create_pagelist */
static int m_y_pos;


/* access method for panel width */
int
get_panel_width(void)
{
    /*      int retval = 0; */
    /*      if (resource.expert_mode & XPRT_SHOW_BUTTONS) */
    /*  	retval = m_panel_width; */
    /*      TRACE_GUI((stderr, "get_panel_width: %d", retval)); */
    /*      return retval; */
    return m_panel_width;
}

/*
  ================================================================================
  Pixmaps indicating the state of menu buttons (radiobutton/checkbox
  on/off, cascading menu). Inspired by menu.c, `check_bits' in the xterm source.
  ================================================================================
*/
#include "xaw_bitmaps.h"
static Pixmap menu_check_on_bitmap;
static Pixmap menu_check_off_bitmap;
static Pixmap menu_radio_on_bitmap;
static Pixmap menu_radio_off_bitmap;
static Pixmap menu_arrow_bitmap;

/*
  ============================================================
  Hack for pullright menus part I: data
  ============================================================
*/

/* There are a few custom widgets for pullright menus out there, but
 * these are old and potentially buggy, so just do it manually via an
 * event handler, similar to Motif tooltips.
 */
static XtIntervalId m_timeout = 0;
static Widget m_active_submenu = NULL;  /* if not NULL, the currently active pullright */
static Widget m_submenu = NULL;		/* parent of the currently active pullright
					   (i.e. the menu label in the parent window) */

static void ActPopdownSubmenus(Widget w, XEvent *event, String *params, Cardinal *num_params);

/* to safely pop down the pullright, this callback is added to its parent menu */
static XtActionsRec menu_actions[] = {
    { "popdown-submenus", ActPopdownSubmenus }
};

struct pullright_position_info {
    Position y;
    Dimension w;
    Dimension h;
    Dimension border_width;
    Widget menu;
};


/*
 * Set all pixmaps indicating the state of the wigdet pointed to by `elems'.
 */
void
xaw_set_button_state(struct button_elems *elems, Boolean on)
{
    static Arg args[] = {
	{ XtNleftBitmap, (XtArgVal)0  },
	{ XtNrightBitmap, (XtArgVal)0 }
    };

    if (elems->type == BT_CHECK)
	args[0].value = on ? menu_check_on_bitmap : menu_check_off_bitmap;
    else if (elems->type == BT_RADIO)
	args[0].value = on ? menu_radio_on_bitmap : menu_radio_off_bitmap;
    if (elems->submenu != NULL)
	args[1].value = menu_arrow_bitmap;

    XtSetValues(elems->widget, args, XtNumber(args));
}

/*
 * Initialize the bitmaps.
 */
void
xaw_initialize_menu_bitmaps(void)
{
    static Boolean initialized = False;
    if (!initialized) {
	initialized = True;
	menu_check_on_bitmap
	    = XCreateBitmapFromData(XtDisplay(globals.widgets.top_level),
				    RootWindowOfScreen(XtScreen(globals.widgets.top_level)),
				    (char *)menu_check_on_bits, MENU_BITMAP_W, MENU_BITMAP_H);
	menu_check_off_bitmap
	    = XCreateBitmapFromData(XtDisplay(globals.widgets.top_level),
				    RootWindowOfScreen(XtScreen(globals.widgets.top_level)),
				    (char *)menu_check_off_bits, MENU_BITMAP_W, MENU_BITMAP_H);
	menu_radio_on_bitmap
	    = XCreateBitmapFromData(XtDisplay(globals.widgets.top_level),
				    RootWindowOfScreen(XtScreen(globals.widgets.top_level)),
				    (char *)menu_radio_on_bits, MENU_BITMAP_W, MENU_BITMAP_H);
	menu_radio_off_bitmap
	    = XCreateBitmapFromData(XtDisplay(globals.widgets.top_level),
				    RootWindowOfScreen(XtScreen(globals.widgets.top_level)),
				    (char *)menu_radio_off_bits, MENU_BITMAP_W, MENU_BITMAP_H);
	menu_arrow_bitmap
	    = XCreateBitmapFromData(XtDisplay(globals.widgets.top_level),
				    RootWindowOfScreen(XtScreen(globals.widgets.top_level)),
				    (char *)menu_arrow_bits, MENU_ARROW_W, MENU_ARROW_H);
    }
}


void
xaw_create_pagelist(void)
{
    Dimension height, width;

    XtVaGetValues(globals.widgets.clip_widget, XtNheight, &height, NULL);
    XtVaGetValues(panel_widget, XtNwidth, &width, NULL);

    width = MAX(width - 2 * (resource.btn_side_spacing + resource.btn_border_width),
		xaw_get_pagelist_size());
    height -= resource.btn_top_spacing + resource.btn_border_width + m_y_pos;
    xaw_create_pagelist_widgets(height, width, m_y_pos, panel_widget);
}


static XtCallbackRec command_call[] = {
    {handle_command, NULL},
    {NULL, NULL},
};

#ifdef USE_PANNER
void
scroll_x_panner(int x)
{
    static int old_x = 0;
    if (panner != 0 && ABS(x - old_x) > 8) {
	XtVaSetValues(panner, XtNsliderX, x, NULL);
	old_x = x;
    }
}

void
scroll_y_panner(int y)
{
    static int old_y = 0;
    if (panner != 0 && ABS(y - old_y) > 8) {
	XtVaSetValues(panner, XtNsliderY, y, NULL);
	old_y = y;
    }
}

static void
panner_cb(Widget widget, XtPointer closure, XtPointer report_ptr)
{
    XawPannerReport *report = (XawPannerReport *)report_ptr;
    static int orig_x = 0, orig_y = 0;
    int x = report->slider_x;
    int y = report->slider_y;
    static Dimension w, h;
    static Arg arg_wh_clip[] = {
	{XtNwidth, (XtArgVal) &w},
	{XtNheight, (XtArgVal) &h},
    };

    UNUSED(closure);
    
    XtGetValues(globals.widgets.clip_widget, arg_wh_clip, XtNumber(arg_wh_clip));
    
    fprintf(stderr, "w: %d, h: %d, globals.page.w: %d, globals.page.h: %d\n",
	    w, h, globals.page.w, globals.page.h);
    XtVaSetValues(widget,
		  XtNsliderWidth, w, XtNsliderHeight, h,
		  XtNcanvasWidth, globals.page.w, XtNcanvasHeight, globals.page.h,
		  NULL);

    fprintf(stderr, "panner moved: %d, %d\n", report->slider_x, report->slider_y);
    if (globals.widgets.x_bar != NULL)
	XtCallCallbacks(globals.widgets.x_bar, XtNscrollProc, cast_int_to_XtPointer(x - orig_x));
    if (globals.widgets.y_bar != NULL)
	XtCallCallbacks(globals.widgets.y_bar, XtNscrollProc, cast_int_to_XtPointer(y - orig_y));
    orig_x = x;
    orig_y = y;
}
#endif /* USE_PANNER */


static void
create_menu_entries(struct button_info *item, Widget parent)
{
    size_t i;

    /* add our own popdown-submenus() action to the default translations of this menu */
    XtAccelerators menu_accels = XtParseAcceleratorTable("<BtnUp>:MenuPopdown()notify()unhighlight()popdown-submenus()");

    for (i = 0; item != NULL && i < item->size; i++) {
	Widget w;

	if (item->elems[i].submenu != NULL) { /* another submenu */
	    XDVI_ERROR((stderr, "Xaw menus don't support nested pulldown menus (ignoring)"));
	    continue;
	}
	
	if (item->elems[i].type == BT_SEP) { /* separator */
	    w = XtCreateManagedWidget(item->elems[i].title, smeLineObjectClass, parent, NULL, 0);
	}
	else if (item->elems[i].action != NULL && item->elems[i].action->proc == Act_recent_files) {
	    /* special case: submenu with recent files */
	    w = XtVaCreateManagedWidget(item->elems[i].title, smeBSBObjectClass, parent,
					XtNlabel, item->elems[i].title,
					XtNleftMargin, 20,
					XtNrightMargin, 16,
					XtNrightBitmap, menu_arrow_bitmap,
					NULL);
	    m_submenu = w;
	    XtOverrideTranslations(parent, menu_accels);
	}
	else { /* normal menu entry */
	    w = XtVaCreateManagedWidget(item->elems[i].title, smeBSBObjectClass, parent,
					XtNlabel, item->elems[i].title,
					XtNleftMargin, 20,
					NULL);
	    XtAddCallback(w, XtNcallback, handle_command, item->elems[i].action);
	}
	item->elems[i].widget = w;
    }
}

void
xaw_create_menu(struct button_info *item, Widget parent, int *ret_width)
{
    Dimension y_pos = resource.btn_top_spacing;
    size_t i;

    XtAppAddActions(XtWidgetToApplicationContext(globals.widgets.form_widget), menu_actions, XtNumber(menu_actions));

    for (i = 0; item != NULL && i < item->size; i++) {
	Widget button;
	Dimension w, h;

	if (item->elems[i].type == BT_SEP) { /* separator */
	    XDVI_ERROR((stderr, "Cannot have a separator on a toplevel Xaw menu (ignoring)"));
	    /* y_pos += resource.btn_between_extra; */
	    continue;
	}	
	else if (item->elems[i].submenu != NULL) { /* menu button, create a pulldown menu */
	    Widget shell;

	    button = XtVaCreateWidget("button", menuButtonWidgetClass, parent,
				      XtNmenuName, item->elems[i].title,
				      XtNlabel, item->elems[i].title,
				      XtNx, resource.btn_side_spacing,
				      XtNy, y_pos,
				      XtNborderWidth, resource.btn_border_width,
				      NULL);
	    shell = XtCreatePopupShell(item->elems[i].title, simpleMenuWidgetClass, button, NULL, 0);
    
	    create_menu_entries(item->elems[i].submenu, shell);
	}
	else { /* command button */
	    command_call[0].closure = (XtPointer)item->elems[i].action;
	    button = XtVaCreateWidget(item->elems[i].title, commandWidgetClass, parent,
				      XtNlabel, item->elems[i].title,
				      XtNx, resource.btn_side_spacing,
				      XtNy, y_pos,
				      XtNborderWidth, resource.btn_border_width,
				      XtNcallback, (XtArgVal)command_call,
				      NULL);
	}
	XtVaGetValues(button, XtNwidth, &w, XtNheight, &h, NULL);
	y_pos += h + resource.btn_between_spacing + 2 * resource.btn_border_width;
	if (w > m_panel_width)
	    m_panel_width = w;
	item->elems[i].widget = button;
    }

    /* adjust button sizes, and manage buttons (managing them earlier may result
       in `parent has no geometry manager' error) */
    for (i = 0; item != NULL && i < item->size; i++) {
	XtVaSetValues(item->elems[i].widget, XtNwidth, m_panel_width, NULL);
	XtManageChild(item->elems[i].widget);
    }
    
    m_y_pos = y_pos - resource.btn_between_spacing + resource.btn_top_spacing + 2 * resource.btn_border_width;
    m_panel_width += 2 * resource.btn_side_spacing + resource.btn_border_width;
    XtVaSetValues(panel_widget, XtNwidth, m_panel_width, NULL);
    *ret_width = m_panel_width;
}

Widget
xaw_create_menu_widgets(Widget parent)
{
    /* hack to disable the magnifier in the panel: */
    XtTranslations panel_translations = XtParseTranslationTable("#augment <ButtonPress>:");
    
    XtAppAddActions(XtWidgetToApplicationContext(parent), menu_actions, XtNumber(menu_actions));
    
    line_widget = XtVaCreateWidget("line", widgetClass, parent,
				   XtNbackground, (XtArgVal)resource.fore_Pixel,
				   XtNwidth, (XtArgVal)1,
				   XtNfromHoriz, (XtArgVal)globals.widgets.vport_widget,
				   XtNborderWidth, (XtArgVal)0,
				   XtNtop, (XtArgVal)XtChainTop,
				   XtNbottom, (XtArgVal)XtChainBottom,
				   XtNleft, (XtArgVal)XtChainRight,
				   XtNright, (XtArgVal)XtChainRight,
				   NULL);
    panel_widget = XtVaCreateWidget("panel", compositeWidgetClass, parent,
				    XtNborderWidth, (XtArgVal)0,
				    XtNfromHoriz, (XtArgVal)line_widget,
 				    XtNtranslations, (XtArgVal)panel_translations,
				    XtNtop, (XtArgVal)XtChainTop,
				    XtNbottom, (XtArgVal)XtChainBottom,
				    XtNleft, (XtArgVal)XtChainRight,
				    XtNright, (XtArgVal)XtChainRight,
				    NULL);
    return panel_widget;
}

static void
filehist_select_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    char *label, *ptr;
    int idx;

    UNUSED(client_data);
    UNUSED(call_data);
    
    XtVaGetValues(w, XtNlabel, &label, NULL);

    idx = strtol(label, &ptr, 10) - 1;
    while (isspace((unsigned char)*ptr))
	ptr++;
    TRACE_GUI((stderr, "User selected: %d, `%s'", idx, ptr));
    if (idx == 0) {
	globals.ev.flags |= EV_RELOAD;
	return;
    }
    file_history_open(ptr);
}

static void
update_menu_labels(Widget menu)
{
    WidgetList children;
    int num_children;
    int i;

    static char *buf = NULL;
    static size_t buf_len = 0;
    size_t new_len;
    
    XtVaGetValues(menu,
		  XtNnumChildren, &num_children,
		  XtNchildren, &children,
		  NULL);
    for (i = 0; i < (int)file_history_size(); i++) {
	int dummy_page;
	char *filename;
	
	if ((filename = file_history_get_elem(i, &dummy_page)) == NULL) {
	    XDVI_ERROR((stderr, "Error accessing element %d of file history", i));
	    continue;
	}

	new_len = LENGTH_OF_INT + strlen(filename) + 1;
	if (new_len > buf_len) {
	    buf = xrealloc(buf, new_len);
	    buf_len = new_len;
	}
	
	sprintf(buf, "%d %s", i + 1, filename);
	XtVaSetValues(children[i], XtNlabel, buf, NULL);
	TRACE_GUI((stderr, "child %d: `%s'", i, buf));
    }

    /* if history size < number of menu entries, destroy excess menu entries */
    for (; i < num_children; i++) {
	XtDestroyWidget(children[i]);
    }
}

void
filehist_menu_add_entry(const char *filename)
{
    static char *buf = NULL;
    static size_t buf_len = 0;
    size_t new_len = LENGTH_OF_INT + strlen(filename) + 1;
    
    Widget menu;
    /* Don't report an error here, since "filehist_pullright" is only created on-demand
       when user clicks on menu, but this may be called before from the event loop.
       (The menu will still contain this entry when it's created later.) */
    if (get_widget_by_name(&menu, globals.widgets.top_level, "filehist_pullright", False)) {
	int num_children;
	Widget w;

	if (new_len > buf_len) {
	    buf = xrealloc(buf, new_len);
	    buf_len = new_len;
	}
	
	XtVaGetValues(menu, XtNnumChildren, &num_children, NULL);
	sprintf(buf, "%d %s", num_children + 1, filename);
	
	w = XtVaCreateManagedWidget("_filehist", smeBSBObjectClass, menu,
				    XtNlabel, buf,
				    XtNleftMargin, 10,
				    NULL);
	XtAddCallback(w, XtNcallback, filehist_select_cb, NULL);
	update_menu_labels(menu);
    }
}

void
filehist_menu_refresh(void)
{
    Widget menu;

    /* Don't report an error here, since "filehist_pullright" is only created on-demand
       when user clicks on menu, but this may be called before from the event loop.
       (The menu will still contain this entry when it's created later.) */
    if (get_widget_by_name(&menu, globals.widgets.top_level, "filehist_pullright", False)) {
	update_menu_labels(menu);
    }
}


static void
filehist_insert_submenu(int idx, const char *filename, int pageno, void *data)
{
    Widget menu = (Widget)data;
    Widget w;
    static char *buf = NULL;
    static size_t buf_len = 0;
    size_t new_len = LENGTH_OF_INT + strlen(filename) + 1;
    
    UNUSED(pageno);

    if (new_len > buf_len) {
	buf = xrealloc(buf, new_len);
	buf_len = new_len;
    }

    sprintf(buf, "%d %s", idx + 1, filename);
    TRACE_GUI((stderr, "Creating menu `%s'", buf));
    w = XtVaCreateManagedWidget("_filehist", smeBSBObjectClass, menu,
				XtNlabel, buf,
				XtNleftMargin, 10,
				NULL);
    XtAddCallback(w, XtNcallback, filehist_select_cb, NULL);
}

/*
  ============================================================
  Hack for pullright menus part II: callbacks and functions
  ============================================================
*/

/* callback to pop down the currently active pullright */
static void
ActPopdownSubmenus(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(event);
    UNUSED(params);
    UNUSED(num_params);
    
    if (m_timeout != 0)
	XtRemoveTimeOut(m_timeout);
    m_timeout = 0;
    if (m_active_submenu != NULL)
	XtPopdown(m_active_submenu);
}
/* create a parent shell for the pullright menu entries */
static Widget
create_files_submenu(void)
{
    Widget popup = XtCreatePopupShell("filehist_pullright", simpleMenuWidgetClass, globals.widgets.top_level,
				      NULL, 0);
    file_history_enumerate(filehist_insert_submenu, popup);
    return popup;
}

/* Acutally pop up the pullright menu */
static void 
popup_pullright(XtPointer client_data, XtIntervalId *id)
{
    int pos_x, pos_y;
    Dimension w1;
    Window dummy;
    static Widget files_submenu = NULL;
    struct pullright_position_info *info = (struct pullright_position_info *)client_data;
    
    UNUSED(id);

    if (files_submenu == NULL)
	files_submenu = create_files_submenu();
    /*  		    XtManageChild(files_submenu); */
    XTranslateCoordinates(DISP, XtWindow(XtParent(m_submenu)), RootWindowOfScreen(SCRN),
			  info->w, info->y, &pos_x, &pos_y, &dummy);
    XtRealizeWidget(files_submenu);
    XtVaGetValues(files_submenu, XtNwidth, &w1, NULL);
    TRACE_GUI((stderr, "Popping up at %d, %d, %d, %d", pos_x, pos_y, w1, WidthOfScreen(SCRN)));
    
    /* if not sufficient place on the right, pop it up on the left */
    /*  fprintf(stderr, "border_width: %d\n", info->border_width); */
    if (pos_x + w1 > WidthOfScreen(SCRN)) {
	/*  fprintf(stderr, "%d > %d!\n", pos_x + w1, WidthOfScreen(SCRN)); */
	pos_x -= (w1 + info->w + 3 * info->border_width);
	/*  fprintf(stderr, "new x: %d\n", pos_x); */
    }
    else {
	pos_x += info->border_width;
    }
    XtVaSetValues(files_submenu,
		  XtNx, pos_x,
		  XtNy, pos_y,
		  NULL);
    /* use XtPopupSpringLoaded() instead of XtPopup() since it does a few things
       that make the pullright behave like a proper menu, like highlighting the
       current selection, setting the cursor shape etc. */
    XtPopupSpringLoaded(files_submenu);
    m_active_submenu = files_submenu;
}

/*
 * This event handler (to be called by read_events(), the main event handling loop)
 * creates a timeout for the pullright to pop up.
 */
void
SubMenuHandleEvent(XtAppContext app, XEvent *event)
{
    static int flag = 0;
    static struct pullright_position_info info = { -1, 0, 0, 0, NULL };
    
    UNUSED(app);

    if (m_submenu == NULL)
	return;
    
    if (event->type == EnterNotify
	|| event->type == MotionNotify
	|| event->type == LeaveNotify
	|| event->type == ButtonPress) {

	/*  	fprintf(stderr, "SubMenuHandleEvent: 0x%lx, 0x%lx\n", event->xany.window, XtWindow(m_submenu)); */
	
	/* Could also loop through a list of windows here ...
	   We need to check for the parent of the menu item, since smeBSBObject is not
	   a real window, and then manually check whether the pointer is inside the menu
	   item.
	*/
	if (XtParent(m_submenu) != NULL &&
	    event->xany.window == XtWindow(XtParent(m_submenu))) {
	    /* don't need to check for x coordinates since we already
	       know that pointer is inside the parent */
	    if (info.y == -1) { /* initialize info */
		XtVaGetValues(m_submenu,
			      XtNy, &(info.y),
			      XtNwidth, &(info.w),
			      XtNheight, &(info.h),
			      NULL);
		XtVaGetValues(XtParent(m_submenu),
			      XtNborderWidth, &(info.border_width),
			      NULL);

		info.menu = m_submenu;
	    }
	    if (info.y < event->xbutton.y && info.y + info.h > event->xbutton.y) {
		if (flag == 0) {
		    /* Create a timeout of 200ms to pop up the menu, so that it doesn't
		       pop up always when the cursor is only moved over the pulldown menu.
		       I think this is about the same delay as the one used by Motif.
		    */
		    flag = 1;
		    TRACE_GUI((stderr, "ENTER: %d, %d, %d; %d, %d",
			       info.y, info.w, info.h, event->xbutton.x, event->xbutton.y));
		    m_timeout = XtAppAddTimeOut(app, 200, popup_pullright, &info);
		    return;
		}
	    }
	    else if (flag == 1) {
		flag = 0;
		TRACE_GUI((stderr, "LEAVE!"));
		if (m_timeout != 0)
		    XtRemoveTimeOut(m_timeout);
		m_timeout = 0;
		if (m_active_submenu != NULL)
		    XtPopdown(m_active_submenu);
		m_active_submenu = NULL;
	    }
	}
    }
}

void
realize_button_panel(XtArgVal h)
{
    XtVaSetValues(line_widget, XtNheight, h, NULL);
    XtVaSetValues(panel_widget, XtNheight, h, NULL);
    
    if ((resource.expert_mode & XPRT_SHOW_BUTTONS) != 0) {
	XtManageChild(line_widget);
	XtManageChild(panel_widget);
    }
}

static void
reconfig_window(void) {
    Dimension x_top, y_top, h_top, w_top;
    XWindowChanges sizeconfigure;
    int sizeconfiguremask;
    
    /* brute-force method to bring the scrollbars back. Apparently a single XConfigureWindow()
       isn't enough to get the scrollbars back, we actually need to move the window a bit,
       and then move it back. */
    sizeconfiguremask = CWWidth | CWHeight;
    XtVaGetValues(globals.widgets.top_level, XtNx, &x_top, XtNy, &y_top, XtNheight, &h_top, XtNwidth, &w_top, NULL);
    sizeconfigure.width = w_top + 1;
    sizeconfigure.height = h_top + 1;
    XConfigureWindow(DISP, XtWindow(globals.widgets.top_level), sizeconfiguremask, &sizeconfigure);
    sizeconfigure.width = w_top;
    sizeconfigure.height = h_top;
    XConfigureWindow(DISP, XtWindow(globals.widgets.top_level), sizeconfiguremask, &sizeconfigure);
}

/* toggle scrollbars to state `visible' */
void
toggle_scrollbars(void)
{
    Widget v_bar = XtNameToWidget(globals.widgets.vport_widget, "vertical");
    Widget h_bar = XtNameToWidget(globals.widgets.vport_widget, "horizontal");
    static Dimension bar_thick;
    static Boolean v_bar_mapped = False, h_bar_mapped = False;
    static Boolean initialized = False;

    Boolean make_v_bar_visible = False;
    Boolean make_v_bar_invisible = False;
    
    Boolean make_h_bar_visible = False;
    Boolean make_h_bar_invisible = False;

    if (v_bar != 0) {
	int test_v = 0;
	XtVaGetValues(v_bar, XtNwidth, &test_v, NULL);
	if (test_v > 1)
	    v_bar_mapped = True;
    }
    if (h_bar != 0) {
	int test_h = 0;
	XtVaGetValues(h_bar, XtNheight, &test_h, NULL);
	if (test_h > 1)
	    h_bar_mapped = True;
    }
    
    if (!initialized) {
	v_bar_mapped = h_bar_mapped = (resource.expert_mode & XPRT_SHOW_SCROLLBARS) != 0;
	initialized = True;
	if (v_bar != 0)
	    XtVaGetValues(v_bar, XtNwidth, &bar_thick, NULL);
	else if (h_bar != 0)
	    XtVaGetValues(h_bar, XtNheight, &bar_thick, NULL);
	else
	    bar_thick = 15; /* FIXME */
    }

    if ((resource.expert_mode & XPRT_SHOW_SCROLLBARS) == 0) {
	if (v_bar_mapped)
	    make_v_bar_invisible = True;
	if (h_bar_mapped)
	    make_h_bar_invisible = True;
    }
    else {
	if (!v_bar_mapped)
	    make_v_bar_visible = True;
	if (!h_bar_mapped)
	    make_h_bar_visible = True;
    }

    if (make_h_bar_visible || make_v_bar_visible) {
	if (make_h_bar_visible && h_bar != 0) {
	    TRACE_GUI((stderr, "h_bar: h %d", bar_thick));
	    XtVaSetValues(h_bar, XtNheight, bar_thick, XtNx, bar_thick, XtNy, 0, XtNborderWidth, 1, NULL);
	    XtManageChild(h_bar);
	    h_bar_mapped = True;
	}
	if (make_v_bar_visible && v_bar != 0) {
	    TRACE_GUI((stderr, "v_bar: w %d", bar_thick));
	    XtVaSetValues(v_bar, XtNwidth, bar_thick, XtNx, 0, XtNy, bar_thick, XtNborderWidth, 1, NULL);
	    XtManageChild(v_bar);
	    v_bar_mapped = True;
	}
	if (h_bar != 0 || v_bar != 0) { /* need to reconfigure screen */
	    reconfig_window();
	}
    }
    else if (make_h_bar_invisible || make_v_bar_invisible) {
	if (make_h_bar_invisible && h_bar != 0) {
	    XtUnmanageChild(h_bar);
	    XtVaSetValues(h_bar, XtNheight, 1, XtNx, 0, XtNy, 0, XtNborderWidth, 0, NULL);
	    h_bar_mapped = False;
	}
	if (make_v_bar_invisible && v_bar != 0) {
	    XtUnmanageChild(v_bar);
	    XtVaSetValues(v_bar, XtNwidth, 1, XtNy, 0, XtNy, 0, XtNborderWidth, 0, NULL);
	    v_bar_mapped = False;
	}
	if (h_bar != 0 || v_bar != 0) { /* need to reconfigure screen */
	    reconfig_window();
	}
    }
    
}

void
toggle_buttons(void)
{
    static Dimension panel_width = 0;
    Dimension w;
	    
    if (panel_width == 0) /* initialize */
	XtVaGetValues(panel_widget, XtNwidth, &panel_width, NULL);

    /* need to get current width of form in case user has resized the window */
    XtVaGetValues(globals.widgets.form_widget, XtNwidth, &w, NULL);
    XtVaSetValues(globals.widgets.vport_widget, XtNresizable, (XtArgVal)True, NULL);

    if (resource.expert_mode & XPRT_SHOW_BUTTONS) { /* show buttons */
	XtManageChild(panel_widget);
	XtManageChild(line_widget);
	XtVaSetValues(globals.widgets.vport_widget,
		      XtNwidth, w - panel_width - 1, /* -1 for line_widget */
		      NULL);
    }
    else { /* hide buttons */
	XtUnmanageChild(panel_widget);
	XtUnmanageChild(line_widget);
	XtVaSetValues(globals.widgets.vport_widget, XtNwidth, w, NULL);
    }
}

#else
/* silence `empty compilation unit' warnings */
static void bar(void); static void foo(void) { bar(); } static void bar(void) { foo(); }
#endif /* ifndef MOTIF */
