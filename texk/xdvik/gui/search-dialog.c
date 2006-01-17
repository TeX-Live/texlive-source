/*
 * Copyright (c) 2003-2004 Stefan Ulrich
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
 */

#include "xdvi-config.h"
#include "xdvi.h"

#include "events.h"
#include "dvi-init.h"
#include "search-dialog.h"
#include "search-internal.h"
#include "xlwradio.h"
#include "statusline.h"
#include "string-utils.h"
#include "util.h"
#include "message-window.h"
#include "x_util.h"

#include <X11/Xatom.h>
#include <X11/StringDefs.h>

#ifdef MOTIF
#  include <Xm/BulletinB.h>
#  include <Xm/DialogS.h>
#  include <Xm/PanedW.h>
#  include <Xm/MessageB.h>
#  include <Xm/LabelG.h>
#  include <Xm/Form.h>
#  include <Xm/Frame.h>
#  include <Xm/ToggleBG.h>
#  include <Xm/Text.h>
#  include <Xm/TextF.h>
#  include <Xm/PushB.h>
#  include <Xm/Protocols.h>
#  include <Xm/AtomMgr.h>
#else /* MOTIF */
#  include <X11/Shell.h>
#  include <X11/Xaw/Paned.h>
#  include <X11/Xaw/Box.h>
#  include <X11/Xaw/Form.h>
#  include <X11/Xaw/Label.h>
#  include <X11/Xaw/Command.h>
#  include <X11/Xaw/Toggle.h>
#  include <X11/Xaw/AsciiText.h>
#endif /* MOTIF */

/* #defines for widgets that are used in common code */
#ifdef MOTIF
#  define SHELL_WIDGET xmDialogShellWidgetClass
#  define PANED_WIDGET xmPanedWindowWidgetClass
#  define FORM_WIDGET xmFormWidgetClass
#  define LABEL_WIDGET xmLabelGadgetClass
#  define TEXT_WIDGET xmTextFieldWidgetClass
#  define CHECKBOX_WIDGET xmToggleButtonGadgetClass
#  define CHECKBUTTON_IS_SET XmNset
#  define PUSHBUTTON_WIDGET xmPushButtonWidgetClass
#  define VALUE_CALLBACK_NAME XmNvalueChangedCallback
#  define ACTIVATE_CALLBACK_NAME XmNactivateCallback
#else /* MOTIF */
#  define SHELL_WIDGET transientShellWidgetClass
#  define PANED_WIDGET panedWidgetClass
#  define FORM_WIDGET formWidgetClass
#  define LABEL_WIDGET labelWidgetClass
#  define TEXT_WIDGET asciiTextWidgetClass
#    ifdef XAW
#      define CHECKBOX_WIDGET radioWidgetClass
#    else /* XAW */
#      define CHECKBOX_WIDGET toggleWidgetClass
#    endif /* XAW */
#  define CHECKBUTTON_IS_SET XtNstate
#  define PUSHBUTTON_WIDGET commandWidgetClass
#  define VALUE_CALLBACK_NAME XtNcallback
#  define ACTIVATE_CALLBACK_NAME XtNcallback
#endif /* MOTIF */

static Boolean m_find_popup_active = False;

/* flags for setting single bits in resource.search_window_defaults */
static const int SETTINGS_USE_REGEXP_FLAG = 1;
static const int SETTINGS_CASE_SENSITIVE_FLAG = 2;
static const int SETTINGS_BACKWARDS_FLAG = 4;
static const int SETTINGS_IGNORE_LINEBREAKS_FLAG = 8;
	

static void
cb_search_go(Widget w, XtPointer client_data, XtPointer call_data)
{
    struct search_settings *settings = (struct search_settings *)client_data;
    Widget find_popup;
    Position x = 0, y = 0;
    
    ASSERT(settings != NULL, "client_data cb_search_go mustn't be NULL!");
    UNUSED(call_data);

    TRACE_FIND((stderr, "cb_search_go: settings: searchterm=|%s|, down = %d, re = %d, case = %d",
		settings->term, settings->direction, settings->use_regexp, settings->case_sensitive));

    /* retrieve the find popup window */
    if ((find_popup = XtNameToWidget(globals.widgets.top_level, "*find_popup")) == 0) {
	XDVI_WARNING((stderr, "Couldn't find \"find_popup\" widget!"));
	x = y = 0;
    }
    else {
	XtVaGetValues(find_popup, XtNx, &x, XtNy, &y, NULL);
	/* add some offsets */
	settings->x_pos = x + 10;
	settings->y_pos = y + 10;
	TRACE_GUI((stderr, "SHELL: %ld; x: %d, y: %d", (unsigned long)w, x, y));
    }

    settings->from_page = current_page;
    settings->message_window = 0;
    TRACE_FIND((stderr, "starting search from page: %d", settings->from_page));
    search_dvi((XtPointer)settings);
}

static void
show_settings(const char *func_name, struct search_settings *settings)
{
    TRACE_FIND((stderr, "%s: settings: str=|%s|, down = %d, re = %d, "
		"case = %d, ignore_hyphens = %d, ignore_linebreaks = %d\n",
		func_name,
		settings->term,
		settings->direction,
		settings->use_regexp,
		settings->case_sensitive,
		settings->ignore_hyphens,
		settings->ignore_linebreaks));

}


static void
cb_regexp_search(Widget w, XtPointer client_data, XtPointer call_data)
{
    struct search_settings *settings = (struct search_settings *)client_data;
    /* Hack: Since XawToggleUnsetCurrent() will invoke this callback again,
       return every second time to avoid multiple popups. This is done by
       setting the flag `do_return'.
    */
#if !HAVE_REGEX_H
    static Boolean do_return = False;
#endif
    ASSERT(settings != NULL, "client_data cb_search_go mustn't be NULL!");
    UNUSED(call_data);

#if !HAVE_REGEX_H
    if (do_return) {
	do_return = False;
	return;
    }
    
    XBell(DISP, 0);
    popup_message(get_matching_parent(w, globals.widgets.top_level, "find_popup", NULL),
		  MSG_ERR,
		  NULL,
		  "Sorry, regular expression support (regex.h) is not available on this platform.");
    do_return = True;
#ifdef MOTIF
    XmToggleButtonGadgetSetState(w, True, False);
#else
    XawToggleUnsetCurrent(w);
#endif
    
#else
    UNUSED(w);
    settings->use_regexp = !(settings->use_regexp);

    /* force re-initialization of search term by resetting utf8_term: */
    free(settings->utf8_term);
    settings->utf8_term = NULL;

    show_settings("cb_regexp_search", settings);
#endif

    if (settings->use_regexp)
	resource.search_window_defaults |= SETTINGS_USE_REGEXP_FLAG;
    else
	resource.search_window_defaults &= ~SETTINGS_USE_REGEXP_FLAG;
    store_preference(NULL, "searchWindowDefaults", "%u", resource.search_window_defaults);
}

static void
cb_backwards_search(Widget w, XtPointer client_data, XtPointer call_data)
{
    struct search_settings *settings = (struct search_settings *)client_data;
    ASSERT(settings != NULL, "client_data cb_search_go mustn't be NULL!");
    UNUSED(w);
    UNUSED(call_data);

    switch (settings->direction) {
    case SEARCH_UP: settings->direction = SEARCH_DOWN; break;
    case SEARCH_DOWN: settings->direction = SEARCH_UP; break;
    default: ASSERT(0, "shouldn't happen: settings->direction is neither SEARCH_UP nor SEARCH_DOWN!"); break;
    }

    show_settings("cb_backwards_search", settings);

    if (settings->direction == SEARCH_UP)
	resource.search_window_defaults |= SETTINGS_BACKWARDS_FLAG;
    else
	resource.search_window_defaults &= ~SETTINGS_BACKWARDS_FLAG;
    store_preference(NULL, "searchWindowDefaults", "%u", resource.search_window_defaults);
}

static void
cb_match_case(Widget w, XtPointer client_data, XtPointer call_data)
{
    struct search_settings *settings = (struct search_settings *)client_data;
    ASSERT(settings != NULL, "client_data cb_search_go mustn't be NULL!");
    UNUSED(w);
    UNUSED(call_data);

    settings->case_sensitive = !(settings->case_sensitive);

    show_settings("cb_match_case", settings);

    if (settings->case_sensitive)
	resource.search_window_defaults |= SETTINGS_CASE_SENSITIVE_FLAG;
    else
	resource.search_window_defaults &= ~SETTINGS_CASE_SENSITIVE_FLAG;
    store_preference(NULL, "searchWindowDefaults", "%u", resource.search_window_defaults);
}

static void
cb_linebreaks(Widget w, XtPointer client_data, XtPointer call_data)
{
    struct search_settings *settings = (struct search_settings *)client_data;
    ASSERT(settings != NULL, "client_data cb_search_go mustn't be NULL!");
    UNUSED(w);
    UNUSED(call_data);

    settings->ignore_linebreaks = !(settings->ignore_linebreaks);
    settings->ignore_hyphens = !(settings->ignore_hyphens);
    
    show_settings("cb_linebreaks", settings);

    if (settings->ignore_linebreaks)
	resource.search_window_defaults |= SETTINGS_IGNORE_LINEBREAKS_FLAG;
    else
	resource.search_window_defaults &= ~SETTINGS_IGNORE_LINEBREAKS_FLAG;
    store_preference(NULL, "searchWindowDefaults", "%u", resource.search_window_defaults);
}

static void
cb_search_cancel(Widget w, XtPointer client_data, XtPointer call_data)
{
    Widget find_popup;
    struct search_settings *settings = (struct search_settings *)client_data;
    
    UNUSED(w);
    UNUSED(call_data);

    /* retrieve the find popup window */
    if ((find_popup = XtNameToWidget(globals.widgets.top_level, "*find_popup")) == 0) {
	XDVI_WARNING((stderr, "Couldn't find \"find_popup\" widget!"));
	return;
    }

    /* This flag is checked in the scanning routines, and
       will be eventually reset by do_pages() in events.c.
       (The scanning routines musn't reset it, since they
       might not be called again after this point!) */
    globals.ev.flags |= EV_FIND_CANCEL;

    search_signal_page_changed(); /* Hack to make search restart anew */
    search_erase_highlighting(True);
    if (settings->message_window != 0) {
	TRACE_GUI((stderr, "kill_message_window: %p\n", (void *)settings->message_window));
	kill_message_window(settings->message_window);
    }
    XtPopdown(find_popup);
    m_find_popup_active = False;
}

static void
cb_search_get_term_button(Widget w, XtPointer client_data, XtPointer call_data)
{
    struct search_settings *settings = (struct search_settings *)client_data;
    char *searchterm = NULL;

    Widget searchbox_input, paned, box;
    UNUSED(call_data);

    box = XtParent(w);
    ASSERT(box != 0, "Parent of button widget mustn't be NULL!");
    paned = XtParent(box);
    ASSERT(paned != 0, "Parent of box widget mustn't be NULL!");

    if ((searchbox_input = XtNameToWidget(paned, "*searchbox_input")) == 0) {
	XDVI_WARNING((stderr, "Couldn't find \"searchbox_input\" widget!"));
	return;
    }
    
#ifdef MOTIF
    XtVaGetValues(searchbox_input, XmNvalue, &searchterm, NULL);
#else
    XtVaGetValues(searchbox_input, XtNstring, &searchterm, NULL);
#endif
    if (searchterm == NULL) {
	XDVI_WARNING((stderr, "Searchterm in cb_search_get_term callback shouldn't be NULL!"));
	return;
    }
    TRACE_FIND((stderr, "searchterm1: |%s|", searchterm));
    settings->term = searchterm;
    
    cb_search_go(w, settings, NULL);
}

static void
search_cancel(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    struct search_settings *settings = NULL;
    void *ptr;
    UNUSED(event);

    ASSERT(*num_params > 0, "params in search_cancel must be > 0!");
    ASSERT(*params != NULL, "params in search_cancel mustn't be NULL!");

    /*
     * the *params char pointer contains the pointer value (address) of `settings';
     * convert it back to a pointer:
     */
    TRACE_GUI((stderr, "Pointer string value: |%s|", *params));
    sscanf(*params, "%p", &ptr);
    settings = (struct search_settings *)ptr;
    TRACE_GUI((stderr, "Pointer address: |%p|", (void *)settings));
    ASSERT(settings != NULL, "Shouldn't happen: Couldn't get string representation of argument pointer.");

    cb_search_cancel(w, settings, NULL);
}

#ifdef MOTIF

static void
cb_search_get_term(Widget w, XtPointer client_data, XtPointer call_data)
{
    struct search_settings *settings = (struct search_settings *)client_data;
    char *searchterm = NULL;
    Widget form, paned, searchbox_form;
    Widget button;
    XEvent ev;
    UNUSED(call_data);

    /* retrieve the `Find' button widget */
    searchbox_form = XtParent(w);
    ASSERT(searchbox_form != 0, "Parent of button widget mustn't be NULL!");
    form = XtParent(searchbox_form);
    ASSERT(form != 0, "Parent of searchbox_form widget mustn't be NULL!");
    paned = XtParent(form);
    ASSERT(paned != 0, "Parent of form widget mustn't be NULL!");
    if ((button = XtNameToWidget(paned, "*find_button")) == 0) {
	XDVI_WARNING((stderr, "Couldn't find \"find_button\" widget!"));
	return;
    }

    if (settings != NULL) {
	/* retrieve search term from text input field */
	XtVaGetValues(w, XmNvalue, &searchterm, NULL);
	if (searchterm == NULL) {
	    XDVI_WARNING((stderr, "Searchterm in cb_search_get_term callback shouldn't be NULL!"));
	    return;
	}
	
	TRACE_FIND((stderr, "searchterm2: |%s|", searchterm));
	settings->term = (const char *)searchterm;
    }

    /* make the `Find' button think it got pushed.
       Also synthetisize an event, just to be sure ... */
    synthetisize_event(&ev, button);
    
    XtCallActionProc(button, "ArmAndActivate", &ev, NULL, 0);
    /* the following don't make the button appear visually pressed: */
    /*     XtCallCallbacks(button, XmNarmCallback, NULL); */
    /*     XtCallCallbacks(button, XmNactivateCallback, NULL); */
}

static void
xm_search_go(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    struct search_settings *settings = NULL;
    void *ptr;
    Widget input;

    UNUSED(w);
    UNUSED(event);

    if (params != NULL) { /* re-initialize search term */
	ASSERT(*num_params == 1, "num_params in xm_search_go should be 1 if *params != NULL");
	/*
	 * the *params char pointer contains the pointer value (address) of `settings';
	 * convert it back to a pointer:
	 */
	TRACE_GUI((stderr, "Pointer string value: |%s|", *params));
	sscanf(*params, "%p", &ptr);
	settings = (struct search_settings *)ptr;
	TRACE_GUI((stderr, "Pointer address: |%p|", (void *)settings));
	ASSERT(settings != NULL, "Shouldn't happen: Couldn't get string representation of argument pointer.");
    }	

    if ((input = XtNameToWidget(globals.widgets.top_level, "*searchbox_input")) == 0) {
	XDVI_WARNING((stderr, "Couldn't find \"searchbox_input\" widget!"));
	return;
    }
    cb_search_get_term(input, settings, NULL);    
}

#else /* MOTIF */

static void
xaw_unset_button(XtPointer client_data, XtIntervalId *id)
{
    XEvent ev;
    Widget button = (Widget)client_data;

    UNUSED(id);
    
    synthetisize_event(&ev, button);
    XtCallActionProc(button, "unset", &ev, NULL, 0);
}

static void
xaw_search_go(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    struct search_settings *settings = NULL;
    void *ptr;
    char *searchterm = NULL;
    Widget button, input;
    XtIntervalId timeout;
    
    UNUSED(w);

    /* retrieve the `Find' button widget */
    if ((button = XtNameToWidget(globals.widgets.top_level, "*find_button")) == 0) {
	XDVI_WARNING((stderr, "Couldn't find \"find_button\" widget!"));
	return;
    }
    if ((input = XtNameToWidget(globals.widgets.top_level, "*searchbox_input")) == 0) {
	XDVI_WARNING((stderr, "Couldn't find \"searchbox_input\" widget!"));
	return;
    }

    XtVaGetValues(input, XtNstring, &searchterm, NULL);
    if (searchterm == NULL) {
	XDVI_WARNING((stderr, "Searchterm in xaw_search_go callback shouldn't be NULL!"));
	return;
    }

    if (params != NULL) { /* re-initialize search term */
	ASSERT(*num_params == 1, "num_params in xaw_search_go should be 1 if *params != NULL");
	/*
	 * the *params char pointer contains the pointer value (address) of `settings';
	 * convert it back to a pointer:
	 */
	TRACE_GUI((stderr, "Pointer string value: |%s|", *params));
	sscanf(*params, "%p", &ptr);
	settings = (struct search_settings *)ptr;
	TRACE_GUI((stderr, "Pointer address: |%p|", (void *)settings));
	ASSERT(settings != NULL, "Shouldn't happen: Couldn't get string representation of argument pointer.");
	
	settings->term = (const char *)searchterm;
    }

    /*
     * Again, we want the button to appear activated when <RETURN> is pressed.
     * For this, we explicitly invoke set()notify(), and unset() after a timeout
     * of 150 milliseconds, which seems to correspond to the Motif default.
     */
    XtCallActionProc(button, "set", event, NULL, 0);
    XtCallActionProc(button, "notify", event, NULL, 0);
    XSync(DISP, False);
    timeout = XtAppAddTimeOut(app, 150, xaw_unset_button, (XtPointer)button);
}


/*
 * Restart search from within another popup (confirmation) window.
 */
static void
xaw_search_restart(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    void *ptr;
    struct search_settings *settings;

    UNUSED(event);
    UNUSED(w);

    ASSERT(*num_params > 0, "params in xaw_search_restart must be > 0!");
    ASSERT(*params != NULL, "params in xaw_search_restart mustn't be NULL!");

    TRACE_GUI((stderr, "Pointer string value: |%s|", *params));
    sscanf(*params, "%p", &ptr);
    settings = (struct search_settings *)ptr;
    
    search_restart((XtPointer)settings);
}

static XtActionsRec xaw_search_actions[] = {
    { "do-search-restart", xaw_search_restart },
    { "do-search", xaw_search_go },
};
#endif /* MOTIF */

static XtActionsRec search_actions[] = {
    { "do-search",
#ifdef MOTIF
      xm_search_go
#else
      xaw_search_go
#endif
    },
    { "cancel-search", search_cancel },
};

static Widget
create_search_window(struct search_settings *settings)
{
    Widget top_level_shell;
    Atom WM_DELETE_WINDOW;
    Widget form, find_paned, box;
    unsigned char curr_state;

    Widget searchbox_form, searchbox_label, searchbox_input;
    /* left and right column */
    Widget options_left_form, options_right_form;
    /* checkboxes in left column */
    Widget regexp_checkbox, matchcase_checkbox, backwards_checkbox;
    /* checkboxes in right column */
    Widget linebreaks_checkbox;
    Widget find_button, cancel_button;

    XtTranslations xlats;
    char *translation_str = NULL;
#ifdef MOTIF
    XmString str;
#else /* MOTIF */
    XtTranslations wm_translations;
    int ddist;
#endif

    XtAddActions(search_actions, XtNumber(search_actions));

#ifndef MOTIF

    XtAddActions(xaw_search_actions, XtNumber(xaw_search_actions));
#define HORIZONTAL_RESIZING_NO	XtNleft, XtChainLeft, XtNright, XtChainLeft
#define HORIZONTAL_RESIZING_YES XtNleft, XtChainLeft, XtNright, XtChainRight
#define VERTICAL_RESIZING_NO	XtNtop, XtChainTop, XtNbottom, XtChainTop

    /*
     * hacky: Since translations can only contain strings, pass
     * the pointer address as string in the callback. But the only
     * portability problem should be the size of the pointer
     * representation, and we deal with that by using VSNPRINTF().
     */
    translation_str = get_string_va("<Message>WM_PROTOCOLS: cancel-search(%p)\nCtrl<Key>g:find-next()",
				    (void *)settings);
    wm_translations = XtParseTranslationTable(translation_str);
    free(translation_str);
    translation_str = NULL;
#endif /* not MOTIF */
    
    top_level_shell = XtVaCreatePopupShell("find_popup",
					   SHELL_WIDGET,
					   globals.widgets.top_level,
					   XtNtitle, "xdvik: Find in DVI file",
					   XtNmappedWhenManaged, False, /* so that we can center it first */
					   XtNtransientFor, globals.widgets.top_level,
					   XtNallowShellResize, True,
#ifdef MOTIF
					   XmNdeleteResponse, XmDO_NOTHING, /* we'll take care of that ourselves */
#else
					   XtNtranslations, wm_translations,
#endif
					   NULL);

    TRACE_GUI((stderr, "toplevel: %ld", (unsigned long)top_level_shell));
    
    WM_DELETE_WINDOW = XInternAtom(XtDisplay(top_level_shell), "WM_DELETE_WINDOW", False);
#ifdef MOTIF
    XmAddWMProtocolCallback(top_level_shell, WM_DELETE_WINDOW, cb_search_cancel, settings);
#endif
    
    find_paned = XtVaCreateWidget("find_paned",
				  PANED_WIDGET,
				  top_level_shell,
#ifdef MOTIF
				  /* make sashes invisible */
				  XmNsashWidth, 1,
				  XmNsashHeight, 1,
#endif
				  NULL);
	
    form = XtVaCreateWidget("form",
			    FORM_WIDGET,
			    find_paned,
#ifdef MOTIF
			    XmNhorizontalSpacing, DDIST_MAJOR,
			    XmNverticalSpacing, DDIST_MAJOR,
			    XmNautoUnmanage, False,
#else
			    XtNallowResize, True,
#endif
			    NULL);

#ifdef MOTIF
    str = XmStringCreateLocalized("Find:");
#else
    XtVaGetValues(form, XtNdefaultDistance, &ddist, NULL);
#endif
    
    /* search term input field */
    searchbox_form = XtVaCreateWidget("searchbox_form",
				      FORM_WIDGET,
				      form,
#ifdef MOTIF
				      XmNleftAttachment, XmATTACH_FORM,
				      XmNrightAttachment, XmATTACH_FORM,
				      XmNhorizontalSpacing, DDIST,
				      XmNverticalSpacing, DDIST,
#else
				      XtNborderWidth, 0,
				      HORIZONTAL_RESIZING_YES,
#endif
				      NULL);
    searchbox_label = XtVaCreateManagedWidget("searchbox_label",
					      LABEL_WIDGET,
					      searchbox_form,
#ifdef MOTIF
					      XmNlabelString, str,
					      XmNtopAttachment, XmATTACH_FORM,
					      XmNleftAttachment, XmATTACH_FORM,
					      XmNbottomAttachment, XmATTACH_FORM,
					      XmNalignment, XmALIGNMENT_BEGINNING,
#else
					      XtNlabel, "Find:",
					      XtNborderWidth, 0,
					      HORIZONTAL_RESIZING_NO,
					      VERTICAL_RESIZING_NO,
#endif
					      NULL);
    
    searchbox_input = XtVaCreateManagedWidget("searchbox_input",
					      TEXT_WIDGET,
					      searchbox_form,
#ifdef MOTIF
					      XmNtopAttachment, XmATTACH_FORM,
					      XmNrightAttachment, XmATTACH_FORM,
					      XmNleftAttachment, XmATTACH_WIDGET,
					      XmNleftWidget, searchbox_label,
					      XmNbottomAttachment, XmATTACH_FORM,
#else
					      XtNwidth, 260,
					      XtNdataCompression, False,
					      XtNeditType, XawtextEdit,
					      XtNfromHoriz, searchbox_label,
					      HORIZONTAL_RESIZING_YES,
					      VERTICAL_RESIZING_NO,
#endif
					      NULL);


    if (settings->term != NULL) {
	XtVaSetValues(searchbox_input,
#ifdef MOTIF
		      XmNvalue,
#else
		      XtNstring,
#endif
		      settings->term, NULL);
    }
    
    XtManageChild(searchbox_form);
    
#ifdef MOTIF
    XtAddCallback(searchbox_input, XmNactivateCallback, cb_search_get_term, settings);
#endif
    
    /*
     * form for left row of options checkbuttons
     */
    options_left_form = XtVaCreateManagedWidget("options_left_form",
						FORM_WIDGET,
						form,
#ifdef MOTIF
						XmNhorizontalSpacing, DDIST,
						XmNverticalSpacing, DDIST,
						XmNresizable, True,
						XmNtopAttachment, XmATTACH_WIDGET,
						XmNtopWidget, searchbox_form,
						XmNleftAttachment, XmATTACH_FORM,
/* 						XmNrightAttachment, XmATTACH_FORM, */
/*  						XmNbottomAttachment, XmATTACH_FORM, */
#else
						XtNborderWidth, 0,
						XtNfromVert, searchbox_form,
						HORIZONTAL_RESIZING_NO,
#endif
						NULL);
    /*
     * form for right row of options checkbuttons
     */
    options_right_form = XtVaCreateManagedWidget("options_right_form",
						 FORM_WIDGET,
						 form,
#ifdef MOTIF
						 XmNhorizontalSpacing, DDIST,
						 XmNverticalSpacing, DDIST,
						 XmNresizable, True,
						 XmNtopAttachment, XmATTACH_WIDGET,
						 XmNtopWidget, searchbox_form,
						 XmNleftAttachment, XmATTACH_WIDGET,
						 XmNleftWidget, options_left_form,
						 XmNrightAttachment, XmATTACH_FORM,
/*  						 XmNbottomAttachment, XmATTACH_FORM, */
#else
						 XtNborderWidth, 0,
						 XtNfromVert, searchbox_form,
						 XtNfromHoriz, options_left_form,
						 HORIZONTAL_RESIZING_NO,
#endif
						 NULL);
#ifdef MOTIF
    XmStringFree(str);
    str = XmStringCreateLocalized("Regular expression");
#endif
    
    regexp_checkbox = XtVaCreateManagedWidget("regexp_checkbox",
					      CHECKBOX_WIDGET,
					      options_left_form,
#ifdef MOTIF
					      XmNlabelString, str,
					      XmNindicatorType, XmN_OF_MANY,
					      XmNtopAttachment, XmATTACH_FORM,
					      XmNleftAttachment, XmATTACH_FORM,
#else
					      XtNlabel, "Regular expression",
					      XtNborderWidth, 0,
					      XtNisRadio, False,
					      XtNhighlightThickness, 1,
					      HORIZONTAL_RESIZING_NO,
					      VERTICAL_RESIZING_NO,
#endif
					      NULL);

    XtAddCallback(regexp_checkbox, VALUE_CALLBACK_NAME, cb_regexp_search, settings);
    
#ifdef MOTIF
    XmStringFree(str);
    str = XmStringCreateLocalized("Find backwards");
#endif

    backwards_checkbox = XtVaCreateManagedWidget("backwards_checkbox",
						 CHECKBOX_WIDGET,
						 options_left_form,
#ifdef MOTIF
						 XmNlabelString, str,
						 XmNindicatorType, XmN_OF_MANY,
						 XmNtopAttachment, XmATTACH_WIDGET,
						 XmNtopWidget, regexp_checkbox,
						 XmNleftAttachment, XmATTACH_FORM,
						 XmNbottomAttachment, XmATTACH_FORM,
#else
						 XtNlabel, "Find backwards",
						 XtNfromVert, regexp_checkbox,
						 XtNborderWidth, 0,
						 XtNisRadio, False,
						 XtNhighlightThickness, 1,
						 HORIZONTAL_RESIZING_NO,
						 VERTICAL_RESIZING_NO,
#endif
						 NULL);

    XtAddCallback(backwards_checkbox, VALUE_CALLBACK_NAME, cb_backwards_search, settings);

#ifdef MOTIF
    XmStringFree(str);
    str = XmStringCreateLocalized("Case sensitive");
#endif

    matchcase_checkbox = XtVaCreateManagedWidget("matchcase_checkbox",
						 CHECKBOX_WIDGET,
						 options_right_form,
#ifdef MOTIF
						 XmNlabelString, str,
						 XmNindicatorType, XmN_OF_MANY,
						 XmNtopAttachment, XmATTACH_FORM,
						 XmNleftAttachment, XmATTACH_FORM,
#else
						 XtNlabel, "Case sensitive",
						 XtNborderWidth, 0,
						 XtNisRadio, False,
						 XtNhighlightThickness, 1,
						 HORIZONTAL_RESIZING_NO,
						 VERTICAL_RESIZING_NO,
#endif
						 NULL);

    XtAddCallback(matchcase_checkbox, VALUE_CALLBACK_NAME, cb_match_case, settings);
    
#ifdef MOTIF
    XmStringFree(str);
    str = XmStringCreateLocalized("Ignore newlines/hyphens");
#endif

    linebreaks_checkbox = XtVaCreateManagedWidget("linebreaks_checkbox",
						  CHECKBOX_WIDGET,
						  options_right_form,
#ifdef MOTIF
						  XmNlabelString, str,
						  XmNindicatorType, XmN_OF_MANY,
						  XmNtopAttachment, XmATTACH_WIDGET,
						  XmNtopWidget, matchcase_checkbox,
						  XmNleftAttachment, XmATTACH_FORM,
#else
						  XtNlabel, "Ignore newlines/hyphens",
						  XtNfromVert, matchcase_checkbox,
						  XtNborderWidth, 0,
						  XtNisRadio, False,
						  XtNhighlightThickness, 1,
						  HORIZONTAL_RESIZING_NO,
						  VERTICAL_RESIZING_NO,
#endif
						  NULL);
    
    XtAddCallback(linebreaks_checkbox, VALUE_CALLBACK_NAME, cb_linebreaks, settings);

    /*
     * box for Find/Cancel buttons
     */
    box = XtVaCreateManagedWidget("box",
				  FORM_WIDGET,
				  find_paned,
#ifdef MOTIF
				  XmNskipAdjust, True, /* don't resize this area */
#else
				  /* resizing by user isn't needed */
				  XtNshowGrip, False,
				  XtNdefaultDistance, 6, /* some padding */
				  /* resizing the window shouldn't influence this box,
				   * but only the pane widget
				   */
				  XtNskipAdjust, True,
				  XtNaccelerators, G_accels_cr,
#endif
				  NULL);
    
#ifdef MOTIF
    XmStringFree(str);
    str = XmStringCreateLocalized("Find");
#endif

    find_button = XtVaCreateManagedWidget("find_button",
					  PUSHBUTTON_WIDGET,
					  box,
#ifdef MOTIF
					  XmNlabelString, str,
					  XmNshowAsDefault, True,
					  XmNdefaultButtonShadowThickness, 1,
					  XmNtopAttachment, XmATTACH_FORM,
					  XmNbottomAttachment, XmATTACH_FORM,
					  XmNleftAttachment, XmATTACH_FORM,
					  /* to mimick appearance of native dialog buttons: */
					  XmNmarginWidth, 6,
					  XmNmarginHeight, 4,
					  XmNtopOffset, 10,
					  XmNbottomOffset, 10,
					  XmNleftOffset, 10,
#else
					  XtNlabel, "Find",
					  XtNaccelerators, G_accels_cr,
					  XtNtop, XtChainTop,
					  XtNbottom, XtChainBottom,
					  HORIZONTAL_RESIZING_NO,
#endif
					  NULL);
#ifdef MOTIF
    XmStringFree(str);
    str = XmStringCreateLocalized("Cancel");
#endif

    cancel_button = XtVaCreateManagedWidget("cancel_button",
					    PUSHBUTTON_WIDGET,
					    box,
#ifdef MOTIF
					    XmNlabelString, str,
					    XmNdefaultButtonShadowThickness, 1,
					    XmNtopAttachment, XmATTACH_FORM,
					    XmNbottomAttachment, XmATTACH_FORM,
					    XmNrightAttachment, XmATTACH_FORM,
					    /* to mimick appearance of native dialog buttons: */
					    XmNmarginWidth, 6,
					    XmNmarginHeight, 4,
					    XmNtopOffset, 10,
					    XmNbottomOffset, 10,
					    XmNrightOffset, 10,
#else
					    XtNlabel, "Cancel",
					    XtNfromHoriz, find_button,
					    XtNbottom, XtChainBottom,
					    XtNjustify, XtJustifyRight,
					    XtNleft, XtChainRight,
					    XtNright, XtChainRight,
#endif
					    NULL);
    
#ifdef MOTIF
    XmStringFree(str);
#endif
    
    XtAddCallback(cancel_button, ACTIVATE_CALLBACK_NAME, cb_search_cancel, settings);
    XtAddCallback(find_button, ACTIVATE_CALLBACK_NAME, cb_search_get_term_button, settings);

    translation_str = get_string_va(
#ifdef MOTIF
				    "<Key>osfCancel:cancel-search(%p)\n"
#else
				    "<Key>Escape:cancel-search(%p)\n"
#endif
				    "<Key>Return:do-search(%p)\n"
				    "Ctrl<Key>g:find-next()",
				    (void *)settings, (void *)settings);
    
    xlats = XtParseTranslationTable(translation_str);
    free(translation_str);

    XtOverrideTranslations(searchbox_form, xlats);
    XtOverrideTranslations(options_left_form, xlats);
    XtOverrideTranslations(options_right_form, xlats);
    XtOverrideTranslations(regexp_checkbox, xlats);
    XtOverrideTranslations(matchcase_checkbox, xlats);
    XtOverrideTranslations(backwards_checkbox, xlats);
    XtOverrideTranslations(box, xlats);
    XtOverrideTranslations(find_button, xlats);
    XtOverrideTranslations(cancel_button, xlats);
    XtOverrideTranslations(searchbox_input, xlats);

    /* following doesn't help to force input to textbox in xaw: */
    /* XtInstallAllAccelerators(find_paned, find_paned); */
    /* XtInstallAllAccelerators(searchbox_input, find_paned); */
    
    XtManageChild(form);
    XtManageChild(find_paned);

    { /* set all buttons to same size */
	Dimension w1, w2, max;
	XtVaGetValues(find_button, XtNwidth, &w1, NULL);
	XtVaGetValues(cancel_button, XtNwidth, &w2, NULL);
	max = MAX(w1, w2);
	XtVaSetValues(find_button, XtNwidth, max, NULL);
	XtVaSetValues(cancel_button, XtNwidth, max, NULL);
    }
    
    XtManageChild(top_level_shell);
    /* don't center this one - would just get in the way in this case. */
    /* center_window(top_level_shell, globals.widgets.top_level); */
    XtPopup(top_level_shell, XtGrabNone);
    m_find_popup_active = True;
    
#ifdef MOTIF
    XmProcessTraversal(searchbox_input, XmTRAVERSE_CURRENT);
/*     XmProcessTraversal(find_button, XmTRAVERSE_CURRENT); */
#endif
    XSetWMProtocols(XtDisplay(top_level_shell), XtWindow(top_level_shell), &WM_DELETE_WINDOW, 1);

    /* check if we have default values from resource.search_window_defaults */
    if (resource.search_window_defaults & SETTINGS_USE_REGEXP_FLAG)
	XtVaSetValues(regexp_checkbox, CHECKBUTTON_IS_SET, True, NULL);
    if (resource.search_window_defaults & SETTINGS_CASE_SENSITIVE_FLAG)
	XtVaSetValues(matchcase_checkbox, CHECKBUTTON_IS_SET, True, NULL);
    if (resource.search_window_defaults & SETTINGS_BACKWARDS_FLAG)
	XtVaSetValues(backwards_checkbox, CHECKBUTTON_IS_SET, True, NULL);
    if (resource.search_window_defaults & SETTINGS_IGNORE_LINEBREAKS_FLAG)
	XtVaSetValues(linebreaks_checkbox, CHECKBUTTON_IS_SET, True, NULL);
	
    /* initialize `settings' values according to the checkbox states
       (in case user has assigned values via X defaults): */
    XtVaGetValues(regexp_checkbox, CHECKBUTTON_IS_SET, &curr_state, NULL);
    settings->use_regexp = curr_state;
    XtVaGetValues(matchcase_checkbox, CHECKBUTTON_IS_SET, &curr_state, NULL);
    settings->case_sensitive = curr_state;
    XtVaGetValues(backwards_checkbox, CHECKBUTTON_IS_SET, &curr_state, NULL);
    settings->direction = curr_state ? SEARCH_UP : SEARCH_DOWN;
    XtVaGetValues(linebreaks_checkbox, CHECKBUTTON_IS_SET, &curr_state, NULL);
    settings->ignore_hyphens = settings->ignore_linebreaks = curr_state;

#ifndef MOTIF
    
#undef HORIZONTAL_RESIZING_NO
#undef HORIZONTAL_RESIZING_YES
#undef VERTICAL_RESIZING_NO

#endif /* MOTIF */
    return top_level_shell;
}

void
dvi_find_string(const char *str, Boolean find_next)
{
    /* Synthesize a RET keystroke for the find dialog.
     * Also pops up the find dialog, to make it easier for user to
     * edit options, change direction etc. */
    Widget find_popup;
    if ((find_popup = XtNameToWidget(globals.widgets.top_level, "*find_popup")) == 0) {
	static struct search_settings settings;
	static struct search_info searchinfo = { False, False, False, 0, 0, 0, 0 };
	settings.term = str;
	settings.use_regexp = False;
	settings.case_sensitive = False;
	settings.direction = SEARCH_DOWN;
	settings.ignore_hyphens = False;
	settings.ignore_linebreaks = False;
	settings.x_pos = -1;
	settings.y_pos = -1;
	settings.searchinfo = &searchinfo;
	settings.hyphen_delta = 0;

	find_popup = create_search_window(&settings);

	if (find_next)
	    return;
    }
    else if (str != NULL) { /* change the search term */
	Widget searchbox_input;
	if ((searchbox_input = XtNameToWidget(find_popup, "*searchbox_input")) == 0) {
	    XDVI_WARNING((stderr, "Couldn't find \"searchbox_input\" widget!"));
	    return;
	}
	XtVaSetValues(searchbox_input,
#ifdef MOTIF
		      XmNvalue,
#else
		      XtNstring,
#endif
		      str, NULL);
    }
    if (m_find_popup_active) {
	XRaiseWindow(DISP, XtWindow(find_popup));
    }
    else {
	XtPopup(find_popup, XtGrabNone);
	m_find_popup_active = True;
    }

    if (str != NULL || find_next) {
#ifdef MOTIF
	xm_search_go(NULL, NULL, NULL, NULL);
#else
	xaw_search_go(NULL, NULL, NULL, NULL);
#endif
    }
}

