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
#ifdef NEW_MENU_CREATION
#include "menu.h"
#endif /* NEW_MENU_CREATION */

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


#ifndef NEW_MENU_CREATION
#define SEP_CHAR ':' /* character separating entries in translations lines */
#endif

/* needed to count the callbacks */
static int destroy_count = 0;

/* width of button panel */
static int my_panel_width = 0;

/* access method for panel width */
int
get_panel_width(void)
{
    int retval = 0;
    if (resource.expert_mode & XPRT_SHOW_BUTTONS)
	retval = my_panel_width;
    TRACE_GUI((stderr, "get_panel_width: %d", retval));
    return retval;
}

#ifndef NEW_MENU_CREATION
typedef enum { INVALID, /* some error */
	       NONE,	/* no new button created */
	       MENU_SEPARATOR, MENU_BUTTON, MENU_ENTRY, COMMAND_BUTTON } buttonTypeT;

struct button_info {
    struct button_info *next;
    char *label;
    struct xdvi_action *action;
    Widget widget;
    buttonTypeT type;
};

static Widget panner = 0;
static struct button_info *b_head = NULL;

#define MAX_MENU_NUM 128
#define MAX_BUTTON_NUM 128

static struct menu_list_ {
    char *name;
    Widget button_widget;
    Widget popup_widget;
} menu_list[MAX_MENU_NUM];

#endif /* not NEW_MENU_CREATION */

#ifdef NEW_MENU_CREATION
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
#else
#include "xaw_bitmaps.h"
/* lifted from menu.c, check_bits in xterm */
static Pixmap menu_check_bitmap;
static Pixmap menu_arrow_bitmap;
#endif /* NEW_MENU_CREATION */

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

static XtAccelerators menu_accels;

struct pullright_position_info {
    Position y;
    Dimension w;
    Dimension h;
    Dimension border_width;
    Widget menu;
};


#ifdef NEW_MENU_CREATION
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

#else

/* toggle `checked' symbol on xaw menu w */
void
toggle_tick(Boolean val, Widget w)
{
    static Arg args = { XtNleftBitmap, (XtArgVal) 0 };
    if (val)
	args.value = (XtArgVal) menu_check_bitmap;
    else
	args.value = None;
    XtSetValues(w, &args, 1);
}

void
toggle_menu(int val, XtActionProc proc)
{
    struct button_info *bp;

    for (bp = b_head; bp != NULL; bp = bp->next) {
	if (bp->action != NULL && bp->action->proc == proc && bp->action->param != NULL) {
	    TRACE_GUI((stderr, "found proc; param: |%s|", bp->action->param));
	    if (strcmp(bp->action->param, "toggle") == 0) {
		if (val != 0)
		    toggle_tick(True, bp->widget);
		else
		    toggle_tick(False, bp->widget);
	    }
	    else if (strcmp(bp->action->param, "a") == 0) {
		if (val == shrink_to_fit())
		    toggle_tick(True, bp->widget);
		else
		    toggle_tick(False, bp->widget);
	    }
	    else {
		int testval = strtoul(bp->action->param, (char **)NULL, 10);
		if (testval == val) {
		    TRACE_GUI((stderr, "enabling param |%s|", bp->action->param));
		    toggle_tick(True, bp->widget);
		}
		else
		    toggle_tick(False, bp->widget);
	    }
	}
    }
}

/* initialize `checked' symbol on xaw menu w */
static void
initialize_tick_marks(void)
{
    int use_gs;
    if (menu_check_bitmap == None) {
	int check_width = 9;
	int check_height = 8;
	unsigned char check_bits[] = {
	    0x00, 0x01, 0x80, 0x01, 0xc0, 0x00, 0x60, 0x00,
	    0x31, 0x00, 0x1b, 0x00, 0x0e, 0x00, 0x04, 0x00
	};

	menu_check_bitmap = XCreateBitmapFromData(XtDisplay(globals.widgets.top_level),
						  RootWindowOfScreen(XtScreen(globals.widgets.top_level)),
						  (char *)check_bits, check_width, check_height);
    }
    
    /* initialize tickmarks for all possible actions */
     use_gs = resource.postscript;
#ifdef PS_GS
    if (!resource.useGS)
	use_gs = 0;
#endif

    toggle_menu(use_gs, Act_set_ps);
    toggle_menu(resource.gs_alpha, Act_set_gs_alpha);
    toggle_menu(resource.keep_flag, Act_set_keep_flag);
    toggle_menu(resource.pixels_per_inch / mane.shrinkfactor, Act_shrink_to_dpi);
    toggle_menu(mane.shrinkfactor, Act_set_shrink_factor);
    toggle_menu(resource.use_tex_pages, Act_use_tex_pages);
    toggle_menu(resource.mouse_mode, Act_switch_mode);
}
#endif /* NEW_MENU_CREATION */

/* ================================================================================
   ================================================================================
*/

static Widget line_widget, panel_widget;

/* used for communication with the pagelist in xaw_create_pagelist */
static int my_y_pos;

/* access function: panel, height of buttons etc. */
void
xaw_create_pagelist(void)
{
    Dimension height, width;
    int button_width = get_panel_width() - 2 * (resource.btn_side_spacing + resource.btn_border_width);
    
    XtVaGetValues(globals.widgets.clip_widget, XtNheight, &height, NULL);
    width = MAX(button_width, xaw_get_pagelist_size());
    height -= resource.btn_top_spacing + resource.btn_border_width + my_y_pos;
    xaw_create_pagelist_widgets(height, width, my_y_pos, panel_widget);
}


static XtCallbackRec command_call[] = {
    {handle_command, NULL},
    {NULL, NULL},
};

#ifdef NEW_MENU_CREATION
void xaw_create_menu(struct button_info *items, Widget parent, int menu_depth, int *ret_width)
{
    
}

Widget
xaw_create_menu_widgets(Widget parent)
{
    /* hack to disable the magnifier in the panel: */
    XtTranslations panel_translations = XtParseTranslationTable("#augment <ButtonPress>:");
    
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

#else /* NEW_MENU_CREATION */


static void
filehist_select_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    char *label, *ptr;
    int idx;

    UNUSED(client_data);
    UNUSED(call_data);
    
    XtVaGetValues(w, XtNlabel, &label, NULL);

    idx = strtol(label, &ptr, 10) - 1;
    while (isspace(*ptr))
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

static Widget
create_pulldown_entry(const char *menu_name,
		      Widget parent,
		      const char *item_name,
		      const char *accelerator, /* accelerator, or NULL */
		      struct xdvi_action *action,
		      buttonTypeT *type)
{
    Widget w;
    char buf[1024];
    char *m_name = xstrdup(menu_name);
    m_name = xstrcat(m_name, item_name);

#ifdef SHOW_ACCELERATORS
    if (accelerator != NULL && *accelerator != '\0')
	SNPRINTF(buf, 1024, "%s  [%s]", item_name, accelerator);
    else
	strncpy(buf, item_name, 1024);
#else
    UNUSED(accelerator);
    strncpy(buf, item_name, 1024);
#endif

/*      fprintf(stderr, "creating pulldown for parent %p: `%s', `%s'\n", */
/*  	    (void *)parent, menu_name, item_name); */
    
    if (strcmp(item_name, "SEP") == 0) { /* it's a separator */
	*type = MENU_SEPARATOR;
	w = XtCreateManagedWidget(m_name, smeLineObjectClass, parent, NULL, 0);
    }
    else if (action->proc != NULL && action->proc == Act_recent_files) { /* special case: submenu with recent files */
/*  	w = recent_files_submenu(m_name, parent, buf); */
/*  	*type = MENU_ENTRY; */
	w = XtVaCreateManagedWidget(m_name, smeBSBObjectClass, parent,
				    XtNlabel, buf,
				    XtNleftMargin, 20,
				    XtNrightMargin, 16,
				    XtNrightBitmap, menu_arrow_bitmap,
				    NULL);
	m_submenu = w;
	/*  fprintf(stderr, "setting translations for %p\n", (void *)XtParent(parent)); */
	XtOverrideTranslations(parent, menu_accels);
	/*  	XtOverrideTranslations(XtParent(parent), menu_accels); */
			       
	*type = MENU_ENTRY;
    }
    else { /* normal menu item */
	w = XtVaCreateManagedWidget(m_name, smeBSBObjectClass, parent,
				    XtNlabel, buf,
				    XtNleftMargin, 20,
				    NULL);
	*type = MENU_ENTRY;
	XtAddCallback(w, XtNcallback, handle_command, action);
    }
    free(m_name);
    return w;
}

static Widget
create_pulldown_menu(const char *menu_name,
		     Dimension *width,
		     int *index,
		     Dimension *y_pos,
		     buttonTypeT *type,
		     int *menu_number)
{
    int i;
    
    /* check whether a pulldown menu with this name already exists,
       and if yes, return its position in index: */
    for (i = 0; i < *menu_number; i++) {
	if (menu_list[i].name != NULL && (strcmp(menu_list[i].name, menu_name)) == 0) {
	    *index = i;
	    break;
	}
    }
    if (i == *menu_number) { /* doesn't exist, create one */
	Dimension w, h;
	Widget n_button_widget, n_popup_widget;
	char *button_widget_name = NULL;

	(*menu_number)++;
	if (*menu_number > MAX_MENU_NUM) {
	    XDVI_WARNING((stderr, "Too many menus (max=%d), skipping all from \"%s\"",
			  MAX_MENU_NUM, menu_name));
	    *type = INVALID;
	    return 0;
	}
	
	/* save menu_name to list for later comparison */
	menu_list[i].name = xrealloc(menu_list[i].name, strlen(menu_name) + 1);
	strcpy(menu_list[i].name, menu_name);

	button_widget_name = xstrdup(menu_name);
	button_widget_name = xstrcat(button_widget_name, "B"); /* make it unique */

	if (globals.debug & DBG_ALL) {
	    fprintf(stderr, "ypos: %d; h: %d; between: %d; border: %d\n",
		    *y_pos,
		    h,
		    resource.btn_between_spacing,
		    resource.btn_border_width);
	}

	n_button_widget = XtVaCreateWidget(menu_list[i].name, menuButtonWidgetClass, panel_widget,
					   XtNmenuName, button_widget_name,
					   XtNx, resource.btn_side_spacing,
					   XtNy, *y_pos,
					   XtNborderWidth, resource.btn_border_width,
					   NULL);
	n_popup_widget = XtVaCreatePopupShell(button_widget_name, simpleMenuWidgetClass, n_button_widget,
					      NULL);
	/* Mustn't free this one?? */
/*  	free(button_widget_name); */
	menu_list[i].button_widget = n_button_widget;
	menu_list[i].popup_widget = n_popup_widget;
	XtVaGetValues(n_button_widget, XtNwidth, &w, XtNheight, &h, NULL);
	*width = w;
	*index = i;
	*y_pos += h + resource.btn_between_spacing + 2 * resource.btn_border_width;
	*type = MENU_BUTTON;
	return n_button_widget;
    }
    *type = NONE;
    return 0;
}

static struct button_info *
create_button_info(const char *label, struct xdvi_action *action, Widget w, buttonTypeT type)
{
    struct button_info *info = xmalloc(sizeof *info);
    info->label = xstrdup(label);
    info->action = action;
    info->widget = w;
    info->type = type;
    return info;
}

typedef enum shrinkArgT_ { NO_SHRINK_ARG, HASH, PERCENT, UNDERSCORE } shrinkArgT;

static shrinkArgT
test_for_shrink_arg(const char *item, int *idx)
{
    const char *ptr = item;
    size_t i;
    for (i = 0; ptr[i] != '\0'; i++) {
	if (ptr[i] == '$') {
	    i++;
	    if (ptr[i]  == '#') {
		*idx = i;
		return HASH;
	    }
	    else if (ptr[i]  == '%') {
		*idx = i;
		return PERCENT;
	    }
	    else if (ptr[i]  == '_') {
		*idx = i;
		return UNDERSCORE;
	    }
	}
    }
    *idx = 0;
    return NO_SHRINK_ARG;
}

static Boolean
create_shrink_button(char **label, int offset, shrinkArgT type, struct xdvi_action *action)
{
    int shrink_arg = 0;
#define TEMPSTR_LEN 128 /* enough for printing a number into */
    char tempstr[TEMPSTR_LEN];
    char *new_label = NULL;
    
    for (;; action = action->next) {
	if (action == NULL) {
	    return False;
	}
	if (action->proc == Act_set_shrink_factor || action->proc == Act_shrink_to_dpi) {
	    /* NOTE: the shrinkbutton[] resource isn't implemented. It probably makes
	       less sense with the pulldown menu resources??
	    */
	    if (action->num_params > 0) {
		shrink_arg = atoi(action->param);
		if (action->proc == Act_shrink_to_dpi)
		    shrink_arg = (double)resource.pixels_per_inch / shrink_arg + 0.5;
		if (shrink_arg < 1)
		    shrink_arg = 1;
		else if (shrink_arg > 99)
		    shrink_arg = 99;
	    }
	    break; /* found shrink action */
	}
    }

    if (type == HASH)
	SNPRINTF(tempstr, TEMPSTR_LEN, "%d", shrink_arg);
    else if (type == PERCENT) {
	if (shrink_arg <= 15)
	    SNPRINTF(tempstr, TEMPSTR_LEN, "%d", (int)(100.0 / shrink_arg + .5));
	else
	    SNPRINTF(tempstr, TEMPSTR_LEN, "%.2f", 100.0 / shrink_arg);
    }
    /* NOTE: I think UNDERSCORE is related to the shrinkbutton[] resource,
       which is not implemented */

    /* return resized label */
    new_label = xmalloc(offset - 1  + strlen(tempstr) + strlen(*label + offset + 1) + 1);
    memcpy(new_label, *label, offset - 1);
    strcpy(new_label + offset - 1, tempstr); /* now it's null-terminated */
    strcat(new_label, *label + offset + 1);
    free(*label);
    *label = new_label;

#undef TEMPSTR_LEN
    return True;
}

static Dimension
create_button(int button_number,	/* number of this button */
	      int *menu_num,		/* number of submenus in this button */
	      char **items,		/* description of label and action strings */
	      size_t item_count,	/* number of items in description */
	      Widget parent,		/* parent of widget to create */
	      struct button_info ***bipp,	/* button info to insert new button into */
	      Dimension *y_pos,		/* vertical position after creating button */
	      const char *c_ptr,	/* entire rest of resource string, for error messages */
	      size_t len)		/* length of current line in resource string, for error messages*/
{
    int menu_idx = -1, label_idx = -1, accelerator_idx = -1, action_idx = -1;
#define NAME_LEN 16
    char name[NAME_LEN];
    struct button_info *bp;
    shrinkArgT shrink_arg_type;
    struct xdvi_action *action;
    Widget widget;
    buttonTypeT b_type;
/*     static int menu_num = 0; /\* count number of created menus *\/ */
    Dimension width = 0;

    ASSERT(item_count >= 2, "Too few items");
    
    if (item_count == 4) { /* submenu for a button */
	menu_idx = 0;
	label_idx = 1;
	accelerator_idx = 2;
	action_idx = 3;
    }
    else if (item_count == 3) { /* command button */
	label_idx = 0;
	accelerator_idx = 1;
	action_idx = 2;
    }
    else { /* separator */
	if (strcmp(items[1], "SEP") == 0) { /* separator */
	    menu_idx = 0;
	    label_idx = 1;
	}
	else {
	    XDVI_WARNING((stderr, "Wrong number of items (%d) in translations line:\n\"%.*s\" (skipping this line).",
			  item_count, (int)len, c_ptr));
	    return 0;
	}
    }

    if (action_idx != -1) {
	int offset;
	shrink_arg_type = test_for_shrink_arg(items[label_idx], &offset);

/* 	fprintf(stderr, "compiling action: |%s|\n", items[action_idx]); */
	if ((action = compile_action(items[action_idx])) == NULL) {
	    XDVI_WARNING((stderr, "Invalid action \"%s\" in translations line:\n\"%.*s\" (skipping this line).",
			  items[action_idx], (int)len, c_ptr));
	    return 0;
	}
    
	if (shrink_arg_type != NO_SHRINK_ARG) { /* search for corresponding shrink action */
	    if (!create_shrink_button(&(items[label_idx]), offset, shrink_arg_type, action)) {
		XDVI_WARNING((stderr, "Non-existent shrink action \"%s\" in translations line:\n\"%.*s\"",
			      items[action_idx], (int)len, c_ptr));
		return 0;
	    }
	}
    }
    else {
	action = NULL;
    }
    
    command_call[0].closure = (XtPointer) action;
    SNPRINTF(name, NAME_LEN, "button%d", button_number);

    if (menu_idx != -1) { /* it's a pulldown menu */
	int index;

	/* create a new pulldown menu if it doesn't exist yet */
	widget = create_pulldown_menu(items[menu_idx], &width, &index, y_pos, &b_type, menu_num);
	if (b_type == INVALID)
	    return 0;
	if (b_type != NONE) { /* it didn't exist yet */
	    bp = create_button_info(items[label_idx], NULL, widget, b_type);
	    **bipp = bp;
	    *bipp = &bp->next;
	}

	/* create an entry for this item */
	widget = create_pulldown_entry(menu_list[index].name,
				       menu_list[index].popup_widget,
				       items[label_idx],
				       accelerator_idx > 0 ? items[accelerator_idx] : NULL,
				       action,
				       &b_type);
	bp = create_button_info(items[label_idx], action, widget, b_type);
	**bipp = bp;
	*bipp = &bp->next;
    }
    else {  /* not a pulldown menu, but a simple command button */
	command_call[0].closure = (XtPointer) action;
	widget = XtVaCreateWidget(name, commandWidgetClass, parent,
				  XtNlabel, (XtArgVal)items[label_idx],
				  XtNx, resource.btn_side_spacing,
				  XtNy, (XtArgVal)*y_pos,
				  XtNborderWidth, resource.btn_border_width,
				  XtNcallback, (XtArgVal)command_call,
				  NULL);
	bp = create_button_info(items[label_idx], action, widget, COMMAND_BUTTON);
	**bipp = bp;
	*bipp = &bp->next;
    }
#undef NAME_LEN
    return width;
}

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

#ifdef USE_PANNER
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
	XtCallCallbacks(globals.widgets.x_bar, XtNscrollProc, (XtPointer)(x - orig_x));
    if (globals.widgets.y_bar != NULL)
	XtCallCallbacks(globals.widgets.y_bar, XtNscrollProc, (XtPointer)(y - orig_y));
    orig_x = x;
    orig_y = y;
}
#endif /* USE_PANNER */

void
create_menu_buttons(Widget form, int *ret_panel_width)
{
    Dimension max_button_width = 0, curr_width;
    Dimension y_pos;
    struct button_info **bipp;
    struct button_info *bp;
    const char *c_ptr, *e_ptr;
    int max_size;
    int button_number = 0;
    int menu_number = 0; /* number of menus created; passed through to create_pulldown_menu */
    
    /* disable the magnifier in the panel: */
    XtTranslations panel_translations = XtParseTranslationTable("#augment <ButtonPress>:");

    XtAppAddActions(XtWidgetToApplicationContext(form), menu_actions, XtNumber(menu_actions));
    /* add our own popdown-submenus() action to the default translations of this menu */
    menu_accels = XtParseAcceleratorTable("<BtnUp>:MenuPopdown()notify()unhighlight()popdown-submenus()");
    
    line_widget = XtVaCreateWidget("line", widgetClass, form,
				   XtNbackground, (XtArgVal)resource.fore_Pixel,
				   XtNwidth, (XtArgVal) 1,
				   XtNfromHoriz, (XtArgVal)globals.widgets.vport_widget,
				   XtNborderWidth, (XtArgVal)0,
				   XtNtop, (XtArgVal)XtChainTop,
				   XtNbottom, (XtArgVal)XtChainBottom,
				   XtNleft, (XtArgVal)XtChainRight,
				   XtNright, (XtArgVal)XtChainRight,
				   NULL);
    panel_widget = XtVaCreateWidget("panel", compositeWidgetClass, form,
				    XtNborderWidth, (XtArgVal)0,
				    XtNfromHoriz, (XtArgVal)line_widget,
 				    XtNtranslations, (XtArgVal)panel_translations,
				    XtNtop, (XtArgVal)XtChainTop,
				    XtNbottom, (XtArgVal)XtChainBottom,
				    XtNleft, (XtArgVal)XtChainRight,
				    XtNright, (XtArgVal)XtChainRight,
				    NULL);
    menu_arrow_bitmap
	= XCreateBitmapFromData(XtDisplay(globals.widgets.top_level),
				RootWindowOfScreen(XtScreen(globals.widgets.top_level)),
				(char *)menu_arrow_bits, MENU_ARROW_W, MENU_ARROW_H);
    
    b_head = NULL;
    bipp = &b_head;

    *ret_panel_width = 0;
    y_pos = resource.btn_top_spacing;

#define FREE_LINE_ITEMS do { \
	    for (curr = 0; curr < item_count; curr++) { \
	        free(line_items[curr]); \
	    } \
	    free(line_items); \
	} while (0)
    
    for (c_ptr = resource.menu_translations;
	 c_ptr != NULL && *c_ptr != '\0';
	 c_ptr = e_ptr + 1) {
	e_ptr = strchr(c_ptr, '\n');
	if (e_ptr != NULL) {
	    char **line_items = NULL;
	    size_t len, curr, item_count = 0;

	    if (e_ptr == c_ptr) { /* single '\n' marks additional space between buttons */
		y_pos += resource.btn_between_extra;
		continue;
	    }
	    len = e_ptr - c_ptr;

	    if (++button_number > MAX_BUTTON_NUM) {
		XDVI_WARNING((stderr, "Too many buttons (max=%d), skipping all from:\n\"%.*s\"",
			      MAX_BUTTON_NUM, (int)len, c_ptr));
		break;
	    }
	    
	    line_items = split_line(c_ptr, SEP_CHAR, 0, len, &item_count);
	    if (item_count < 3 && !(item_count > 1 && strcmp(line_items[1], "SEP") == 0)) {
		XDVI_WARNING((stderr, "Too few separators \"%c\" in translations line:\n\"%.*s\" (skipping this line).\n",
			      SEP_CHAR, (int)len, c_ptr));
		FREE_LINE_ITEMS;
		continue;
	    }
	    curr_width = create_button(button_number,
				       &menu_number,
				       line_items,
				       item_count,
				       panel_widget,
				       &bipp,
				       &y_pos,
				       c_ptr, len);
	    if (curr_width == 0) { /* no button created */
		FREE_LINE_ITEMS;
		continue;
	    }
	    /* adjust max_button_width to this new entry */
	    if (curr_width > max_button_width)
		max_button_width = curr_width;

	    FREE_LINE_ITEMS;
	}
    }

#undef FREE_LINE_ITEMS
    
    /* null-terminate button info */
    *bipp = NULL;

    max_size = xaw_get_pagelist_size();
    if (max_size > max_button_width) {
	max_button_width = max_size;
    }

    *ret_panel_width = max_button_width + 2 * (resource.btn_side_spacing + resource.btn_border_width);
    TRACE_GUI((stderr, "panel_widget: w: %d", *ret_panel_width));
    XtVaSetValues(panel_widget, XtNwidth, (XtArgVal)*ret_panel_width, NULL);
    ++(*ret_panel_width);
    my_panel_width = *ret_panel_width; /* make it available */

    for (bp = b_head; bp != NULL; bp = bp->next) {
	if (bp->type == MENU_BUTTON || bp->type == COMMAND_BUTTON) {
	    TRACE_GUI((stderr, "bp->widget: w: %d", max_button_width));
	    XtVaSetValues(bp->widget, XtNwidth, (XtArgVal)max_button_width, NULL);
	    XtManageChild(bp->widget);
	}
    }

    my_y_pos = y_pos - resource.btn_between_spacing + resource.btn_top_spacing + 2 * resource.btn_border_width;
    initialize_tick_marks();
#ifdef USE_PANNER
    {
	static Dimension w, h;
	static Arg arg_wh_clip[] = {
	    {XtNwidth, (XtArgVal) &w},
	    {XtNheight, (XtArgVal) &h},
	};
	XtGetValues(globals.widgets.clip_widget, arg_wh_clip, XtNumber(arg_wh_clip));

	fprintf(stderr, "w: %d, h: %d, globals.page.w: %d, globals.page.h: %d\n",
		w, h, globals.page.w, globals.page.h);
	if (w == 0)
	    w = globals.page.w - 2;
	if (h == 0)
	    h = globals.page.h - 2;
	panner = XtVaCreateManagedWidget("panner", pannerWidgetClass, panel_widget,
					 XtNx, resource.btn_side_spacing,
					 XtNy, my_y_pos,
					 XtNheight, 60,
					 XtNwidth, max_button_width,
					 XtNshadowThickness, 0,
					 XtNsliderWidth, w,
					 XtNsliderHeight, h,
					 XtNcanvasWidth, globals.page.w,
					 XtNcanvasHeight, globals.page.h,
					 XtNsliderX, 5,
					 XtNsliderY, 7,
					 XtNinternalSpace, 0,
					 NULL);
    }
    my_y_pos += 60 + resource.btn_top_spacing + 2 * resource.btn_border_width;
    XtAddCallback(panner, XtNreportCallback, panner_cb, (XtPointer)NULL);

#endif /* USE_PANNER */

}
#endif /* NEW_MENU_CREATION */


void
set_button_panel_height(XtArgVal h)
{
    TRACE_GUI((stderr, "line_widget: h %d", (int)h));
    XtVaSetValues(line_widget, XtNheight, h, NULL);
    XtManageChild(line_widget);

    XtVaSetValues(panel_widget, XtNheight, h, NULL);
    XtManageChild(panel_widget);
}

/****************************************************************************
 * Action handling code.
 */


static void
handle_destroy_buttons(Widget widget, XtPointer client_data, XtPointer call_data)
{
    int panel_width;
    Dimension window_w, window_h;
    
    UNUSED(widget);
    UNUSED(client_data);
    UNUSED(call_data);
    
    if (--destroy_count != 0) {
	return;
    }
    XtVaSetValues(globals.widgets.vport_widget, XtNresizable, (XtArgVal)True, NULL);
    XtVaGetValues(globals.widgets.form_widget,
		      XtNwidth, &window_w,
		      XtNheight, &window_h,
		      NULL);
    
    if ((resource.expert_mode & XPRT_SHOW_BUTTONS) == 0) {
	/* destroy buttons */
	TRACE_GUI((stderr, "globals.widgets.vport_widget: h %d, w %d", window_w, window_h));
	XtVaSetValues(globals.widgets.vport_widget, XtNwidth, window_w, XtNheight, window_h, NULL);
    }
    else {
	create_menu_buttons(globals.widgets.form_widget, &panel_width);
	window_w -= panel_width;
	TRACE_GUI((stderr, "globals.widgets.vport_widget: h %d, w %d", window_h, window_w));
	XtVaSetValues(globals.widgets.vport_widget, XtNwidth, window_w, XtNheight, window_h, NULL);
	set_button_panel_height((XtArgVal) window_h);
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
    static Boolean buttons_active = False;
    static Boolean initialized = False;
    Dimension window_w, window_h;


    Boolean make_buttons_visible = False;
    Boolean make_buttons_invisible = False;

    int panel_width;
    
    if (!initialized) {
	buttons_active = (resource.expert_mode & XPRT_SHOW_BUTTONS) != 0;
	initialized = True;
    }

    if ((resource.expert_mode & XPRT_SHOW_BUTTONS) == 0) {
	if (buttons_active)
	    make_buttons_invisible = True;
    }
    else {
	if (!buttons_active)
	    make_buttons_visible = True;
    }
	    
    if (make_buttons_visible) {
	if (destroy_count != 0) {
	    return;
	}

	create_menu_buttons(globals.widgets.form_widget, &panel_width);
	XtVaGetValues(globals.widgets.form_widget,
		      XtNwidth, &window_w,
		      XtNheight, &window_h,
		      NULL);
	XtVaSetValues(globals.widgets.vport_widget, XtNresizable, (XtArgVal)True, NULL);
	window_w -= panel_width;
	TRACE_GUI((stderr, "globals.widgets.vport_widget: w: %d, h: %d", window_w, window_h));
	XtVaSetValues(globals.widgets.vport_widget, XtNwidth, window_w, XtNheight, window_h, NULL);
	set_button_panel_height((XtArgVal) window_h);
	buttons_active = True;
    }
    else if (make_buttons_invisible) {
	if (destroy_count != 0) {
	    return;
	}
	/* this counts the callback calls; 1 for the panel, 1 for the line */
	destroy_count = 2;

	XtAddCallback(panel_widget, XtNdestroyCallback,
		      handle_destroy_buttons, (XtPointer)0);
	XtAddCallback(panel_widget, XtNdestroyCallback,
		      handle_destroy_pagelist, (XtPointer)0);
	XtAddCallback(line_widget, XtNdestroyCallback,
		      handle_destroy_buttons, (XtPointer)0);
	XtDestroyWidget(panel_widget);
	XtDestroyWidget(line_widget);
	buttons_active = False;
/* 	window_w += get_panel_width(); */

	while (b_head != NULL) {
	    struct button_info *bp = b_head;
	    struct xdvi_action *action;

	    b_head = bp->next;
	    free(bp->label);
	    /* free bp->action */
	    for (action = bp->action; action != NULL;) {
		struct xdvi_action *act2 = action;
		action = act2->next;
		if (act2->num_params > 0) {
		    free(act2->param);
		}
		free(act2);
	    }
	    free(bp);
	}
    }
}

#else
/* silence `empty compilation unit' warnings */
static void bar(void); static void foo() { bar(); } static void bar(void) { foo(); }
#endif /* ifndef MOTIF */
