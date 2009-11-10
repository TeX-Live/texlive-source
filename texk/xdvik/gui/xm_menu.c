/*
 * Copyright (c) 2001 Marcin Dalecki and others
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

#include "xm_menu.h"
#include "xm_toolbar.h"
#include "my-snprintf.h"
#include "util.h"
#include "x_util.h"
#include "version.h"
#include "statusline.h"
#include "pagehist.h"
#include "dvi-init.h"
#include "filehist.h"
#include "c-openmx.h"
#include "message-window.h"

#ifdef MOTIF /* needed for `make depend' */

#include <Xm/RowColumn.h>
#include <Xm/Frame.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/CascadeBG.h>
#include <Xm/SeparatoG.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Xm/Separator.h>


/*
 * There's an apparent bug in Motif related to the interaction between
 * the menubar menus and the magnifier.
 *
 * If you click on a menu on the menubar and then click on the drawing
 * widget to pop up a magnifier, the keyboard and pointer are still
 * grabbed, leading to a weird situation in which the magnifier stays
 * around even after you release the pointer.  The ungrab_serial
 * counter works around this bug by ignoring such pointer events.
 *
 * This bug occurs in Motif 1.2 and OpenMotif 2.2.2 (at least).  It
 * does not occur in Lesstif.
 */
static unsigned long ungrab_event_num = 0;

void
popdown_callback(Widget w, XtPointer client_data, XtPointer call_data)
{
    UNUSED(w);
    UNUSED(client_data);
    /* Lesstif gives call_data == NULL */
    if (call_data != NULL && *((XtGrabKind *) call_data) != XtGrabNone) {
	ungrab_event_num = LastKnownRequestProcessed(DISP);
    }
}

Boolean
pulldown_menu_active(unsigned long event_num)
{
    return event_num < ungrab_event_num;
}


void
toggle_menubar(void)
{
    if ((resource.expert_mode & XPRT_SHOW_MENUBAR) == 0)
	XtUnmanageChild(globals.widgets.menu_bar);
    else
	XtManageChild(globals.widgets.menu_bar);
}    

static void
update_menu_labels(Widget menu)
{
    WidgetList children;
    int num_children;
    int i;

    static char *buf = NULL;
    static size_t buf_len = 0;
    
    XtVaGetValues(menu,
		  XmNnumChildren, &num_children,
		  XmNchildren, &children,
		  NULL);

    /*      for (i = 0; i < num_children; i++) { */
    for (i = 0; i < (int)file_history_size(); i++) {
	int dummy_page;
	char *filename;
	size_t new_len;
	XmString str;
	
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
	str = XmStringCreateLocalized(buf);
	XtVaSetValues(children[i],
		      XmNlabelString, str,
		      NULL);
	if (i + 1 < 10) {
	    XtVaSetValues(children[i], XmNmnemonic, buf[0], NULL);
	}
	XmStringFree(str);
    }

    /* if history size < number of menu entries, destroy excess menu entries */
    for (; i < num_children; i++) {
	XtDestroyWidget(children[i]);
    }
}

static void
filehist_select_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    Widget menu;
    XmString label;
    char *label_ptr;
    int pageno;
    char *fname;
    int idx;
    
    /*     UNUSED(w); */
    UNUSED(call_data);
    UNUSED(client_data);
    
    XtVaGetValues(w,
		  XmNuserData, &menu,
		  XmNlabelString, &label,
		  NULL);

    XmStringGetLtoR(label, G_charset, &label_ptr);
    idx = strtol(label_ptr, (char **)NULL, 10) - 1;
    XtFree(label_ptr);
    
    ASSERT(menu != NULL, "XmNuserData in filehist_select_cb musn't be NULL!");

    if (idx == 0) { /* user re-selected current file: reload */
	globals.ev.flags |= EV_RELOAD;
	return;
    }
    if ((fname = file_history_get_elem(idx, &pageno)) == NULL) {
	statusline_error(STATUS_MEDIUM, "Error accessing file %d of history", idx);
	return;
    }

    file_history_open(fname);
}

void
filehist_menu_refresh(void)
{
    Widget menu;
    if (get_widget_by_name(&menu, globals.widgets.menu_bar, Xdvi_FILEHIST_MENU, True)) {
	update_menu_labels(menu);
    }
}

void
filehist_menu_add_entry(const char *filename)
{
    Widget menu;

    static char *buf = NULL;
    static size_t buf_len = 0;
    size_t new_len = LENGTH_OF_INT + strlen(filename) + 1;
    
    if (get_widget_by_name(&menu, globals.widgets.menu_bar, Xdvi_FILEHIST_MENU, True)) {
	int num_children;
	Widget w;

	if (new_len > buf_len) {
	    buf = xrealloc(buf, new_len);
	    buf_len = new_len;
	}
	
	XtVaGetValues(menu, XmNnumChildren, &num_children, NULL);

	sprintf(buf, "%d %s", num_children + 1, filename);
	
	w = XtVaCreateManagedWidget(buf, xmPushButtonGadgetClass, menu,
				    XmNuserData, menu,
				    NULL);
	if (num_children + 1 < 10) {
	    XtVaSetValues(w, XmNmnemonic, buf[0], NULL);
	}
	XtAddCallback(w, XmNactivateCallback, filehist_select_cb, cast_int_to_XtPointer(num_children + 1));

	update_menu_labels(menu);
	
    }
}

static void
filehist_submenu(int idx, const char *filename, int pageno, void *data)
{
    Widget menu = (Widget)data;
    Widget w;
    static char *buf = NULL;
    static size_t buf_len = 0;
    size_t new_len = LENGTH_OF_INT + strlen(filename) + 1;

    if (new_len > buf_len) {
	buf = xrealloc(buf, new_len);
	buf_len = new_len;
    }
    
    UNUSED(pageno);

    sprintf(buf, "%d %s", idx + 1, filename);
    TRACE_GUI((stderr, "Creating menu `%s'", buf));
    w = XtVaCreateManagedWidget(buf, xmPushButtonGadgetClass, menu,
				/*  				XmNmnemonic, buf[0], */
				XmNuserData, menu,
				NULL);
    if (idx + 1 < 10) {
	XtVaSetValues(w, XmNmnemonic, buf[0], NULL);
    }
    XtAddCallback(w, XmNactivateCallback, filehist_select_cb, cast_int_to_XtPointer(idx));
}



static Widget
recent_files_submenu(Widget parent, char *title, char mnemonic)
{
    Widget menu = 0, cascade = 0;
    XmString str;

    menu = XmCreatePulldownMenu(parent, Xdvi_FILEHIST_MENU, NULL, 0);
    str = XmStringCreateLocalized(title);
    cascade =  XtVaCreateManagedWidget(title, xmCascadeButtonGadgetClass, parent,
 				       XmNsubMenuId, menu,
 				       XmNlabelString, str,
 				       XmNmnemonic, mnemonic,
 				       NULL);
    XmStringFree(str);

    file_history_enumerate(filehist_submenu, menu);
    return cascade;
}

Widget
xm_create_menu(Widget parent, char *title, char mnemonic, struct button_info *item)
{
    Widget menu = 0, cascade = 0, w = 0;
    size_t i;
    XmString str;

    menu = XmCreatePulldownMenu(parent, "_pulldown", NULL, 0);
    str = XmStringCreateLocalized(title);
    cascade =  XtVaCreateManagedWidget(title, xmCascadeButtonGadgetClass, parent,
 				       XmNsubMenuId, menu,
 				       XmNlabelString, str,
 				       XmNmnemonic, mnemonic,
 				       NULL);
    XmStringFree(str);
    
    /* add the menu items */
    for (i = 0; item != NULL && i < item->size; i++) {
	item->elems[i].widget = 0;
	/* if there's a subitem, call this function recursively */
	if (item->elems[i].submenu != NULL) {
	    w = xm_create_menu(menu, item->elems[i].title,
			       item->elems[i].mnemonic, item->elems[i].submenu);
	    /*
	      workaround for pointer grabbing bug; add additional callback to all menus
	      (they have the same parent)
	    */
	    XtAddCallback(XtParent(menu), XtNpopdownCallback,
			  popdown_callback, (XtPointer) 0);
	}
	else if (item->elems[i].action != NULL && item->elems[i].action->proc == Act_recent_files) {
	    /* special case: submenu with recent files */
	    w = recent_files_submenu(menu, item->elems[i].title, item->elems[i].mnemonic);
	    XtAddCallback(XtParent(menu), XtNpopdownCallback,
			  popdown_callback, (XtPointer) 0);
	}
	else {
	    switch(item->elems[i].type) {
	    case BT_PUSH:
		w = XtVaCreateManagedWidget(item->elems[i].title, xmPushButtonGadgetClass, menu,
					    NULL);
		break;
	    case BT_RADIO:
		w = XtVaCreateManagedWidget(item->elems[i].title, xmToggleButtonGadgetClass, menu,
					    XmNindicatorType, XmONE_OF_MANY,
					    NULL);
		break;
	    case BT_CHECK:
		w = XtVaCreateManagedWidget(item->elems[i].title, xmToggleButtonGadgetClass, menu,
					    XmNindicatorType, XmN_OF_MANY,
					    NULL);
		break;
	    case BT_SEP:
		w = XtVaCreateManagedWidget(item->elems[i].title, xmSeparatorGadgetClass, menu,
					    NULL);
		break;
	    default:
		XDVI_WARNING((stderr, "unrecognized button type %d in menu %s (skipping this item).\n",
			      item->elems[i].type, item->elems[i].title));
		break;
	    }
	    item->elems[i].widget = w;
	}

	if (item->elems[i].mnemonic != 0)
	    XtVaSetValues(w, XmNmnemonic, item->elems[i].mnemonic, NULL);
	
	if (item->elems[i].accelerator != NULL) {
	    str = XmStringCreateLocalized(item->elems[i].accelerator);
	    XtVaSetValues(w, XmNacceleratorText, str, NULL);
	    XmStringFree(str);
	}
	
	if (item->elems[i].action != NULL) {
	    String cb_type;
	    if (XmIsToggleButton(w) || XmIsToggleButtonGadget(w))
		cb_type = XmNvalueChangedCallback;
	    else
		cb_type = XmNactivateCallback;
	    XtAddCallback(w, cb_type, handle_command, item->elems[i].action);
	}
    }
    return cascade;
}

#else
/* silence `empty compilation unit' warnings */
static void bar(void); static void foo(void) { bar(); } static void bar(void) { foo(); }
#endif /* MOTIF */

