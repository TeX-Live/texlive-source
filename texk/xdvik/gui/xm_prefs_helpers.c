/*
 * Copyright (c) 2004 Stefan Ulrich
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
 * Panel 3 (Helper Applications) for xdvik preferences dialog.
 */

#include "xdvi-config.h"
#include "xdvi.h"

#include "x_util.h"
#include "xm_colorsel.h"
#include "topic-window.h"
#include "util.h"
#include "string-utils.h"
#include "string_list.h"
#include "message-window.h"
#include "events.h"
#include "xm_menu.h"

#include "xm_prefsP.h"
#include "xm_prefs.h"
#include "xm_prefs_helpers.h"

#ifdef MOTIF /* entire file */

#include <X11/Xatom.h>

#include <Xm/Xm.h>
#include <Xm/Protocols.h>
#include <Xm/DialogS.h>
#include <Xm/LabelG.h>
#include <Xm/PushB.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/PanedW.h>
#include <Xm/DrawnB.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>
#include <Xm/ToggleBG.h>
#include <Xm/PushBG.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/FileSB.h>
#include <Xm/SelectioB.h>

struct choice_dialog_info {
    struct topic_info *tinfo;
    /*     struct prefs_choice *prefs; */
    Widget combo_box;
    Widget message_popup;
};

static char **m_browser_list = NULL;
static char **m_editor_list = NULL;

#if USE_COMBOBOX
#include <Xm/ComboBox.h>
#include <Xm/List.h>
#else
#include <Xm/CascadeBG.h>
#endif

static void select_browser_cb(Widget w, XtPointer client_data, XtPointer call_data);
static void select_editor_cb(Widget w, XtPointer client_data, XtPointer call_data);

/*
 * User clicked on `Help' in browser text input prompt
 */
static void
help_browser_dialog_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    Widget popup = popup_message(XtParent(w),
				 MSG_HELP,
				 NULL,
				 "The browser is used by xdvi to browse remote documents. "
				 "Please enter the name of the browser (i.e. the executable program) "
				 "you want to use. The browser command may optionally contain a string "
				 "`%%s' which is replaced by the current URL. If no `%%s' is present, "
				 "the URL argument is simply appended to the command.");
    UNUSED(client_data);
    UNUSED(call_data);

    TRACE_GUI((stderr, "setting user data to %p", (void *)popup));
    XtVaSetValues(w, XmNuserData, popup, NULL);
}

/*
 * User clicked on `Help' in editor text input prompt
 */
static void
help_editor_dialog_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    Widget popup = popup_message(XtParent(w),
				 MSG_HELP,
				 NULL,
				 "The editor is used by reverse search to open the TeX source for a DVI file "
				 "(see `Help' -> `Source Specials' for details)."
				 "Please enter the name of the editor executable you want to use. "
				 "The editor command may optionally contain two format strings: "
				 "`%%l' is replaced by the line number in the TeX file, and `%%f' by the file name. "
				 "(If the format strings are not present, they are appended implicitly.)");
    UNUSED(client_data);
    UNUSED(call_data);

    TRACE_GUI((stderr, "setting user data to %p", (void *)popup));
    XtVaSetValues(w, XmNuserData, popup, NULL);
}

/*
 * User cancelled browser or editor text input prompt
 */
static void
destroy_dialog_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    /*     Widget browser_combo = (Widget)client_data; */
    struct choice_dialog_info *info = (struct choice_dialog_info *)client_data;
    Widget browser_combo = NULL;
    Widget help_popup;
#if USE_COMBOBOX
    Widget browser_list_w;
#endif
    UNUSED(call_data);

    XtVaGetValues(w, XmNuserData, &help_popup, NULL);
    TRACE_GUI((stderr, "GOT help_popup: %p", (void *)help_popup));

    if (info != NULL)
	browser_combo = info->combo_box;
    
    if (browser_combo != NULL) {
#if USE_COMBOBOX
	/* Need to unselect the `Other...' entry from the list; to do this,
	   we select the index from XmNuserData if it's >= 0, or the first item. */
	ptrdiff_t idx;
	XtPointer p;
	XtVaGetValues(browser_combo,
		      XmNlist, &browser_list_w,
		      XmNuserData, &p,
		      NULL);
	idx = (ptrdiff_t) p;
	if (browser_list_w == 0)
	    XDVI_ERROR((stderr, "couldn't get list component of combo box!\n"));
	else {
	    if (idx < 0)
		idx = 0;
	    XmListSelectPos(browser_list_w, idx + 1, True);
	}
#else
	Widget parent;
	if ((parent = XtParent(w)) != NULL) {
	    Widget rowcol, child;
	    if (strcmp(XtName(parent), Xdvi_BROWSER_POPUP) == 0) {
		if (get_widget_by_name(&rowcol, globals.widgets.top_level, Xdvi_BROWSER_COMBO_NAME, True)) {
		    XtVaGetValues(rowcol, XmNuserData, &child, NULL);
		    XtVaSetValues(rowcol, XmNmenuHistory, child, NULL);
		}
	    }
	    else if (strcmp(XtName(parent), Xdvi_EDITOR_POPUP) == 0) {
		if (get_widget_by_name(&rowcol, globals.widgets.top_level, Xdvi_EDITOR_COMBO_NAME, True)) {
		    XtVaGetValues(rowcol, XmNuserData, &child, NULL);
		    XtVaSetValues(rowcol, XmNmenuHistory, child, NULL);
		}
	    }
	    else {
		XDVI_WARNING((stderr, "unexpected widget name `%s' in destroy_dialog_cb", XtName(parent)));
	    }
	}
#endif
    }

    /*
     * Also popdown dependent help window if it exists before destroying the widget;
     * otherwise, Motif may even crash in _XmIsFastSubclass() when the help window is moved!
     * 
     * Since `help_popup' is the top-level xmDialogShellWidget, we need to get its
     * xmMessageBoxWidget child (by name ...). Don't report an error if it doesn't
     * exist in case the help window has already been closed.
     */
    if (help_popup != NULL) {
	static Widget message;
	if (get_widget_by_name(&message, help_popup, Xdvi_MESSAGE_DIALOG_NAME, False))
	    XtCallCallbacks(message, XmNokCallback, NULL);
    }

    if (info != NULL) {
	struct prefs_choice *prefs = (struct prefs_choice *)info->tinfo->data;
	remove_from_deplist(prefs, w);
	free(info);
    }
    
    /* destroy the parent of this dialog (the shell) */
    XtDestroyWidget(XtParent(w));
}

static void
h_get_input_wrapper(const char *listbox_name,
		    Widget w, XtPointer client_data, XtPointer call_data)
{
    /*     Widget combobox = (Widget)client_data; */
    struct choice_dialog_info *info = (struct choice_dialog_info *)client_data;
    struct topic_info *tinfo = info->tinfo;
    struct prefs_choice *prefs = (struct prefs_choice *)tinfo->data;
    Widget combobox, child;
    /* following need to be allocated since we want to set resource.xyz to it */
    static char *browser_choice= NULL, *editor_choice = NULL;
#if !USE_COMBOBOX
    XtCallbackProc select_cb = NULL;
#endif
    XmSelectionBoxCallbackStruct *cbs = (XmSelectionBoxCallbackStruct *)call_data;
    static char *ptr = NULL;
    char *tmp_list;

    if (call_data == NULL) /* widget already being destroyed? */
	return;

    ASSERT(info != NULL, "client_data in h_get_input_wrapper mustn't be NULL!");
    combobox = info->combo_box;
    child = info->message_popup;
    XtVaGetValues(w, XmNuserData, &child, NULL);
    TRACE_GUI((stderr, "GOT child: %p", (void *)child));

    if (ptr)
	XtFree((XtPointer)ptr);
    ptr = NULL;
    XmStringGetLtoR(cbs->value, G_charset, &ptr);
    if (strlen(ptr) == 0 || is_spaces_only(ptr)) {
	popup_message(XtParent(w),
		      MSG_ERR,
		      NULL,
		      "Empty input string");
	return;
    }

    if (strcmp(listbox_name, Xdvi_BROWSER_COMBO_NAME) == 0) {
#if 0
	int i;
#endif
	
	free(browser_choice);
	browser_choice = xstrdup(ptr);
	resource.browser = browser_choice;

#if 0
	for (i = 0; m_browser_list[i] != NULL; i++) {
	    fprintf(stderr, "list %d: |%s|\n", i, m_browser_list[i]);
	}
#endif
	m_browser_list = string_list_prepend(m_browser_list, ptr);
#if 0
	for (i = 0; m_browser_list[i] != NULL; i++) {
	    fprintf(stderr, "NEW list %d: |%s|\n", i, m_browser_list[i]);
	}
#endif
	tmp_list = string_list_to_str(m_browser_list, "\n");
#if 0
	fprintf(stderr, "TMP LIST: |%s|\n", tmp_list);
#endif /* 0 */
	store_preference(&(prefs->db), "prefsBrowserList", "%s", tmp_list);
	free(tmp_list);
	
	store_preference(&(prefs->db), "wwwBrowser", "%s", ptr);
#if !USE_COMBOBOX
	select_cb = select_browser_cb;
#endif
    }
    else if (strcmp(listbox_name, Xdvi_EDITOR_COMBO_NAME) == 0) {
	free(editor_choice);
	editor_choice = xstrdup(ptr);
	resource.editor = editor_choice;

	m_editor_list = string_list_prepend(m_editor_list, ptr);
	tmp_list = string_list_to_str(m_editor_list, "\n");
	store_preference(&(prefs->db), "prefsEditorList", "%s", tmp_list);
	free(tmp_list);
	
	store_preference(&(prefs->db), "editor", "%s", ptr);
#if !USE_COMBOBOX
	select_cb = select_editor_cb;
#endif
    }
    else
	XDVI_WARNING((stderr, "Unexpected name in h_get_input_wrapper: `%s'", listbox_name));

#if USE_COMBOBOX
    UNUSED(listbox_name);
    /* add user input to the combo box list, and make it current */
    XmComboBoxAddItem(combobox, cbs->value, 1, True);
    XmComboBoxSelectItem(combobox, cbs->value);
    {   /* if more than 9 items, add scrollbar */
	size_t n;
	XtVaGetValues(combobox, XmNitemCount, &n, NULL);
	XtVaSetValues(combobox, XmNvisibleItemCount, n > 10 ? 10 : n, NULL);
    }
#else
    {
	Widget new_menu, parent, grandparent, rowcol;
	if ((parent = XtParent(w)) != NULL && (grandparent = XtParent(parent)) != NULL) {
	    /* add new item to front of list ... */
	    new_menu = XtVaCreateManagedWidget(ptr, xmPushButtonGadgetClass, grandparent,
					       XmNpositionIndex, 0,
					       XmNuserData, tinfo,
					       NULL);
	    if (select_cb != NULL) {
		XtAddCallback(new_menu, XmNactivateCallback, select_cb, grandparent);
	    }
	    /* ... and make it current! */
	    if (get_widget_by_name(&rowcol, globals.widgets.top_level, listbox_name, True)) {
		XtVaSetValues(rowcol, XmNmenuHistory, new_menu, NULL);
	    }
	}
    }
#endif

    /* as above: popdown help window */
    if (child != NULL) {
	Widget message;
	if (get_widget_by_name(&message, child, Xdvi_MESSAGE_DIALOG_NAME, False))
	    XtCallCallbacks(message, XmNokCallback, NULL);
    }

    remove_from_deplist(prefs, w);
    free(info);
    
    /* destroy the parent of this dialog (the shell) */
    XtDestroyWidget(XtParent(w));
}

static void
h_selector(const char *prompt_name,
	   const char *title_str, const char *label_str,
	   XtCallbackProc ok_cb, 
	   XtCallbackProc destroy_cb, 
	   XtCallbackProc help_cb, 
	   Widget w, XtPointer client_data, XtPointer call_data)
{
    struct topic_info *tinfo = NULL;
    struct prefs_choice *prefs = NULL;
    struct choice_dialog_info *info = NULL;
    char *choice;
    /* following need to be allocated since we want to set resource.xyz to it */
    static char *browser_choice= NULL, *editor_choice = NULL;

#if USE_COMBOBOX
    XmComboBoxCallbackStruct *cb;
#else
    XmString str;
    UNUSED(client_data);
    UNUSED(call_data);
#endif
    
#if USE_COMBOBOX
    tinfo = (struct topic_info *)client_data;
    prefs = (struct prefs_choice *)tinfo->data;
#endif
    info = xmalloc(sizeof *info);
#if USE_COMBOBOX
    cb = (XmComboBoxCallbackStruct *)call_data;

    if (cb->event == NULL) /* only browsing, no selection */
	return;
    
    choice = (char *)XmStringUnparse(cb->item_or_text, XmFONTLIST_DEFAULT_TAG,
				     XmCHARSET_TEXT, XmCHARSET_TEXT,
				     NULL, 0,
				     XmOUTPUT_ALL);
#else
    XtVaGetValues(w, XmNlabelString, &str, NULL);
    XmStringGetLtoR(str, G_charset, &choice);
    
    XtVaGetValues(w, XmNuserData, &tinfo, NULL);
    ASSERT(tinfo != NULL, "XmNuserData in callback musn't be NULL!");
    prefs = (struct prefs_choice *)tinfo->data;
#endif

    if (strcmp(choice, Xdvi_ADD_COMMAND_STR) == 0) {
	Widget prompt_widget;
	Arg args[8];
	int n = 0;
	XmString title = XmStringCreateLocalized((char *)title_str);
	XmString label = XmStringCreateLocalized((char *)label_str);
	
	XtSetArg(args[n], XmNselectionLabelString, label); n++;
	XtSetArg(args[n], XmNautoUnmanage, False); n++;
	XtSetArg(args[n], XmNdialogTitle, title); n++;
	XtSetArg(args[n], XmNuserData, NULL); n++;
	prompt_widget = XmCreatePromptDialog(
#if USE_COMBOBOX
					     w,
#else
					     XtParent(w),
#endif
					     (char *)prompt_name, args, n);
	/* 	XmStringFree(label); */
	
	add_to_deplist(prefs, prompt_widget);
	
	info->tinfo = tinfo;
	info->combo_box = w;
	info->message_popup = NULL;
	
	/* Note: w is the browser_combo widget */
	XtAddCallback(prompt_widget, XmNokCallback, ok_cb, (XtPointer)info);
	XtAddCallback(prompt_widget, XmNcancelCallback, destroy_cb, (XtPointer)info);
	XtAddCallback(prompt_widget, XmNhelpCallback, help_cb, (XtPointer)info);
	
	XtManageChild(prompt_widget);
    }
    else { /* normal item */
#if USE_COMBOBOX
	int i;
#endif

	if (strcmp(prompt_name, Xdvi_BROWSER_POPUP_NAME) == 0) {
#if !USE_COMBOBOX
	    Widget rowcol;
	    if (get_widget_by_name(&rowcol, globals.widgets.top_level, Xdvi_BROWSER_COMBO_NAME, True)) {
		XtVaSetValues(rowcol, XmNuserData, w, NULL);
	    }
#endif
	    free(browser_choice);
	    browser_choice = xstrdup(choice);
	    resource.browser = browser_choice;
	    store_preference(&(prefs->db), "wwwBrowser", "%s", browser_choice);
	}
	else if (strcmp(prompt_name, Xdvi_EDITOR_POPUP_NAME) == 0) {
#if !USE_COMBOBOX
	    Widget rowcol;
	    if (get_widget_by_name(&rowcol, globals.widgets.top_level, Xdvi_EDITOR_COMBO_NAME, True)) {
		XtVaSetValues(rowcol, XmNuserData, w, NULL);
	    }
#endif
	    free(editor_choice);
	    editor_choice = xstrdup(choice);
	    resource.editor = editor_choice;
	    store_preference(&(prefs->db), "editor", "%s", editor_choice);
	}
	else
	    XDVI_ERROR((stderr, "Unknown category `%s' in h_selector()!", prompt_name));


#if USE_COMBOBOX
	/* update the currently selected value */
	XtVaGetValues(w, XmNselectedPosition, &i, NULL);
	XtVaSetValues(w, XmNuserData, cast_int_to_XtPointer(i), NULL);
#endif
    }
#if USE_COMBOBOX
    XtFree(choice);
#endif
}

/*
 * User OK'ed browser text input prompt
 */
static void
get_browser_text_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    if (XtIsRealized(w)) {
	h_get_input_wrapper(Xdvi_BROWSER_COMBO_NAME, w, client_data, call_data);
    }
}


/*
 * User OK'ed editor text input prompt
 */
static void
get_editor_text_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    if (XtIsRealized(w)) {
	h_get_input_wrapper(Xdvi_EDITOR_COMBO_NAME, w, client_data, call_data);
    }
}


/*
 * User selected an item from browser combo box pulldown list
 */
static void
select_browser_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    h_selector(Xdvi_BROWSER_POPUP_NAME,
	       "xdvik: Add Browser Command",
	       "Browser Command (optional `%s' is replaced by URL): ",
	       get_browser_text_cb, destroy_dialog_cb, help_browser_dialog_cb,
	       w, client_data, call_data);
}

/*
 * User selected an item from editor combo box pulldown list
 */
static void
select_editor_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    UNUSED(client_data);
    h_selector(Xdvi_EDITOR_POPUP_NAME,
	       "xdvik: Add Editor Command",
	       "Editor Command (optional: `%l' = line number, `%f' = file name): ",
	       get_editor_text_cb, destroy_dialog_cb, help_editor_dialog_cb,
	       w, client_data, call_data);
}

void
update_preferences_helpers(void)
{
    Widget shell, browser_cascade, editor_cascade;

    if (get_widget_by_name(&shell, globals.widgets.top_level, Xdvi_PREFS_DIALOG_NAME, False)
	&& get_widget_by_name(&browser_cascade, shell, Xdvi_BROWSER_COMBO_NAME, True)
	&& get_widget_by_name(&editor_cascade, shell, Xdvi_EDITOR_COMBO_NAME, True)) {

#if USE_COMBOBOX

	Widget browser_list_w, editor_list_w;
	XmString str;
	int i;
	Boolean found = False;
	
	XtVaGetValues(browser_cascade, XmNlist, &browser_list_w, NULL);
	XtVaGetValues(editor_cascade, XmNlist, &editor_list_w, NULL);

	for (i = 0; m_browser_list[i] != NULL; i++) {
	    if (globals.curr_browser != NULL) { /* if this is set, ignore browser setting */
		if (strcmp(globals.curr_browser, m_browser_list[i]) == 0) {
		    found = True;
		    break;
		}
	    }
	    else if (resource.browser != NULL) {
		if (strcmp(resource.browser, m_browser_list[i]) == 0) {
		    found = True;
		    break;
		}
	    }
	}
	if (!found)
	    i = 0;
	str = XmStringCreateLtoR((char *)m_browser_list[i], "UNMARKED");
	XmComboBoxSelectItem(browser_cascade, str);
	XmStringFree(str);
	
	for (i = 0; m_editor_list[i] != NULL; i++) {
	    if (globals.curr_editor != NULL) { /* if this is set, ignore editor setting */
		if (strcmp(globals.curr_editor, m_editor_list[i]) == 0) {
		    found = True;
		    break;
		}
	    }
	    else if (resource.editor != NULL) {
		if (strcmp(resource.editor, m_editor_list[i]) == 0) {
		    found = True;
		    break;
		}
	    }
	}
	if (!found)
	    i = 0;
	str = XmStringCreateLtoR((char *)m_editor_list[i], "UNMARKED");
	XmComboBoxSelectItem(editor_cascade, str);
	XmStringFree(str);
	/* 	XmListSetPos(editor_list_w, i); */
	/* 	fprintf(stderr, "setting editor list index %d, %s\n", */
	/* 		i, editor_list[i-1]); */

	
	/* 	if (XmListGetSelectedPos(browser_list_w, &browser_items, &browser_cnt) */
	/* 	    && XmListGetSelectedPos(editor_list_w, &editor_items, &editor_cnt)) { */
	/* 	    if (browser_cnt > 0 && editor_cnt > 0) { */
	/* 		int browser_idx = browser_items[0] - 1; */
	/* 		int editor_idx = editor_items[0] - 1; */
	/* 		fprintf(stderr, "selected: %s, %s\n", */
	/* 			browser_list[browser_idx], */
	/* 			editor_list[editor_idx]); */
	/* 	    } */
	/* 	    else { */
	/* 		XDVI_WARNING((stderr, "Shouldn't happen: No items selected in browser list?")); */
	/* 		return; */
	/* 	    } */
	/* 	    XtFree((XtPointer)browser_items); */
	/* 	    XtFree((XtPointer)editor_items); */
	/* 	} */
#else
	Widget browser_menu, editor_menu;
	if (get_widget_by_name(&browser_menu, shell, Xdvi_BROWSER_MENU_NAME, True)
	    && get_widget_by_name(&editor_menu, shell, Xdvi_EDITOR_MENU_NAME, True)) {
	    int i, num_buttons;
	    WidgetList buttons;
	    
	    XtVaGetValues(browser_menu,
			  XmNnumChildren, &num_buttons,
			  XmNchildren, &buttons,
			  NULL);
	    for (i = 0; i < num_buttons; i++) {
		XmString str;
		char *ptr;
		
		XtVaGetValues(buttons[i], XmNlabelString, &str, NULL);
		XmStringGetLtoR(str, G_charset, &ptr);
		/* 		fprintf(stderr, "kid %d: %s\n", i, ptr); */
		if (globals.curr_browser != NULL) { /* if this is set, ignore browser setting */
		    if (strcmp(globals.curr_browser, ptr) == 0) {
			break;
		    }
		}
		else if (resource.browser != NULL) {
		    if (strcmp(resource.browser, ptr) == 0) {
			break;
		    }
		}
	    }
	    if (i >= num_buttons) /* not found */
		i = 0;
	    XtVaSetValues(browser_cascade, XmNmenuHistory, buttons[i], NULL);

	    /* same for editor */
	    XtVaGetValues(editor_menu,
			  XmNnumChildren, &num_buttons,
			  XmNchildren, &buttons, NULL);
	    for (i = 0; i < num_buttons; i++) {
		XmString str;
		char *ptr;
		
		XtVaGetValues(buttons[i], XmNlabelString, &str, NULL);
		XmStringGetLtoR(str, G_charset, &ptr);
		/* 		fprintf(stderr, "kid %d: %s\n", i, ptr); */
		if (globals.curr_editor != NULL) { /* if this is set, ignore editor setting */
		    if (strcmp(globals.curr_editor, ptr) == 0) {
			break;
		    }
		}
		else if (resource.editor != NULL) {
		    if (strcmp(resource.editor, ptr) == 0) {
			break;
		    }
		}
	    }
	    if (i >= num_buttons) /* not found */
		i = 0;
	    XtVaSetValues(editor_cascade, XmNmenuHistory, buttons[i], NULL);

	}
#endif
        /* for browser/editor, try:

	Widget      menu;
	int         num_buttons;
	WidgetList  buttons;
       
	XtVaGetValues( simple_option_widget, XmNsubMenuId, &menu, NULL);
       
	XtVaGetValues( menu, XmNnumChildren, &num_buttons,
	XmNchildren, &buttons, NULL ) ;
	
	*/
    }
}

static Widget
h_create_command(const char *name,
		 const char *menu_name,
		 const char *label,
		 const char *curr_value,
		 Widget parent, Widget top,
		 char **command_list,
		 XtCallbackProc select_cb,
		 struct topic_info *tinfo)
{
    const char *ptr = NULL;
    size_t i;

#if USE_COMBOBOX
    XmStringTable str_list;
    Widget text_label, combo_box;
    size_t k, num;
    int curr_index = -1;
#else
    Widget menu, cascade, item;
    XmString str;
    Arg args[8];
    int n;
#endif
    
    /* check if we need to add the default resource name */
    ptr = curr_value;

    /* don't add it if it's already in our list */
    for (i = 0; ptr != NULL && command_list[i] != NULL; i++) {
	if (strcmp(command_list[i], ptr) == 0) {
	    ptr = NULL;
	}
    }
#if USE_COMBOBOX
    UNUSED(menu_name);
    text_label = XmCreateLabelGadget(parent, (char *)label, NULL, 0);
    XtVaSetValues(text_label,
		  XmNtopAttachment, XmATTACH_WIDGET,
		  XmNtopWidget, top,
		  XmNleftAttachment, XmATTACH_FORM,
		  /* 		  XmNrightAttachment, XmATTACH_FORM, */
		  NULL);

    /* count elements in command_list */
    for (num = 0; command_list[num] != NULL; num++) { ; }

    if (ptr != NULL)
	num++;
	
    str_list = (XmStringTable)XtMalloc(num * sizeof(XmString *));

    i = 0;
    if (ptr != NULL)
	str_list[i++] = XmStringCreateLtoR((char *)ptr, "UNMARKED");
    
    for (k = 0; i < num; i++, k++) {
	if (curr_value != NULL && strcmp(command_list[k], curr_value) == 0) {
	    curr_index = i;
	}
	if (strcmp(command_list[k], Xdvi_ADD_COMMAND_STR) == 0)
	    str_list[i] = XmStringCreateLtoR((char *)command_list[k], "MARKED");
	else
	    str_list[i] = XmStringCreateLtoR((char *)command_list[k], "UNMARKED");
    }
    combo_box = XtVaCreateWidget(name, xmComboBoxWidgetClass,
				 parent,
				 XmNtopAttachment, XmATTACH_WIDGET,
				 XmNtopWidget, top, /* if top == NULL, this uses XmATTACH_FORM */
				 XmNleftAttachment, XmATTACH_WIDGET,
				 XmNleftWidget, text_label,
				 XmNrightAttachment, XmATTACH_FORM,
				 XmNcomboBoxType, XmDROP_DOWN_LIST,
				 XmNitems, str_list,
				 XmNitemCount, num,
				 XmNvisibleItemCount, num > 10 ? 10 : num,
				 XmNuserData, cast_int_to_XtPointer(curr_index),
				 XmNarrowSize, Xdvi_COMBO_BOX_ARROW_SIZE,
				 NULL);
    if (top != NULL) /* FIXME: This assumes we only have 2 items ... */
	XtVaSetValues(combo_box,
		      XmNbottomAttachment, XmATTACH_FORM,
		      NULL);
    
    /* make resource setting current value */
    if (curr_index >= 0) {
	XmComboBoxSelectItem(combo_box, str_list[curr_index]);
    }
	
    for (i = 0; i < num; i++)
	XmStringFree(str_list[i]);
    XtFree((XtPointer)str_list);

    XtAddCallback(combo_box, XmNselectionCallback, select_cb, (XtPointer)tinfo); 
    
    /*
     * workaround for pointer grabbing bug (see xm_menu.c). We need to use
     * the popdownCallback of the internal `GrabShell' child of the combo box.
     */
    {
	Widget grab_shell;
	if (get_widget_by_name(&grab_shell, combo_box, "GrabShell", True))
	    XtAddCallback(grab_shell, XtNpopdownCallback, popdown_callback, NULL);
    }
    adjust_heights(text_label, combo_box, NULL);

    XtManageChild(text_label);
    XtManageChild(combo_box);

    return combo_box;
#else
    menu = XmCreatePulldownMenu(parent, (char *)menu_name, NULL, 0);
    /*     XtVaSetValues(menu, */
    /* 		  XmNtopAttachment, XmATTACH_WIDGET, */
    /* 		  XmNtopWidget, top, */
    /* 		  XmNleftAttachment, XmATTACH_FORM, */
    /* 		  XmNrightAttachment, XmATTACH_FORM, */
    /* 		  NULL); */

    str = XmStringCreateLocalized((char *)label);
    n = 0;
    XtSetArg(args[n], XmNsubMenuId, menu); n++;
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNuserData, NULL); n++;
    cascade = XmCreateOptionMenu(parent, (char *)name, args, n);
    if (top)
	XtVaSetValues(cascade,
		      XmNtopAttachment, XmATTACH_WIDGET,
		      XmNtopWidget, top,
		      XmNleftAttachment, XmATTACH_FORM,
		      NULL);
    else
	XtVaSetValues(cascade,
		      XmNtopAttachment, XmATTACH_FORM,
		      XmNleftAttachment, XmATTACH_FORM,
		      NULL);
    XmStringFree(str);

    if (ptr != NULL) {
	item = XtVaCreateManagedWidget(ptr, xmPushButtonGadgetClass, menu,
				       XmNuserData, tinfo,
				       NULL);
	XtAddCallback(item, XmNactivateCallback, select_cb, menu); 
    }
    
    for (i = 0; command_list[i] != NULL; i++) {
	item = XtVaCreateManagedWidget(command_list[i], xmPushButtonGadgetClass, menu,
				       XmNuserData, tinfo,
				       NULL);
	XtAddCallback(item, XmNactivateCallback, select_cb, menu); 
    }
    XtManageChild(cascade);
    return cascade;
#endif
}

Widget
prefs_helpers(struct topic_info *tinfo)
{
    /*     struct prefs_choice *prefs = (struct prefs_choice *)info->data; */
    Widget form, /* frame, */ form1;
    Widget browser_command;

    form = XmCreateForm(tinfo->right_form, "helpers_form", NULL, 0);

    /*     frame = XmCreateFrame(form, "commands_frame", NULL, 0); */

    form1 = XtVaCreateWidget("commands_form", xmFormWidgetClass,
			     form,
			     XmNverticalSpacing, 10,
			     XmNhorizontalSpacing, 0,
			     NULL);

    if (m_browser_list == NULL)
	m_browser_list = get_separated_list(resource.prefs_browser_list, "\n", False);
    
    if (m_editor_list == NULL)
	m_editor_list = get_separated_list(resource.prefs_editor_list, "\n", False);
    
    browser_command = h_create_command(Xdvi_BROWSER_COMBO_NAME,
				       Xdvi_BROWSER_MENU_NAME,
				       "Web Browser: ",
				       resource.browser,
				       form1, NULL,
				       m_browser_list,
				       select_browser_cb, tinfo);
    
    h_create_command(Xdvi_EDITOR_COMBO_NAME,
		     Xdvi_EDITOR_MENU_NAME,
		     "Editor for Source Specials: ",
		     resource.editor,
		     form1, browser_command,
		     m_editor_list,
		     select_editor_cb, tinfo);
    
    /* #if PS_GS */
    /*     Widget b2 = XmCreateLabelGadget(rowcol, "[x] Use Ghostscript to interpret PS specials", NULL, 0); */

    /* TODO: don't need this??? Similar: ps2pdf, dvips conversion?? */
    /*     Widget b3 = XmCreateLabelGadget(rowcol, "Path to Ghostscript: ____________", NULL, 0); */
    
    /* #endif */
    /*     Widget b4 = XmCreateLabelGadget(rowcol, "Editor for source specials: <pulldown> [Other ...]", NULL, 0); */
    /*     XtManageChild(b1); */
    /*     XtManageChild(b2); */
    /*     XtManageChild(b3); */
    /*     XtManageChild(b4); */
    /*     XtManageChild(rowcol); */

    XtManageChild(form1);
    /*     XtManageChild(frame); */

    return form;
}

#else
/* silence `empty compilation unit' warnings */
static void bar(void); static void foo(void) { bar(); } static void bar(void) { foo(); }
#endif /* MOTIF */

