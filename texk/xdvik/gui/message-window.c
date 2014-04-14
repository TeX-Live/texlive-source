/*------------------------------------------------------------
  message-window.c: message popups for xdvi.

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



/*

============================================
Suggested Policy for using the GUI messages:
============================================

- Use the statusline for shorter messages, a message window for more
important messages or such where you'd like to give further help info
(see the `helptext' argument of popup_message()). When in doubt,
prefer the statusline (excessive use of popup windows is a nuisance
for the user).

- Don't use any of the GUI messages to report internal workings of
the program; for important internal information, there should be a
debugging setting to print it to stderr. Use the GUI messages
only in situations such as the following:

- to give the user feedback on actions that (s)he initiated

- to indicate that an internal action causes a delay perceptible
by the user (as a rough guide: a delay of more than half a second)

- to report situations that might require new actions by the user.

*/

#include "xdvi-config.h"
#include "xdvi.h"
#include "string-utils.h"

#include <ctype.h>

    /* Xaw specific stuff */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#ifdef MOTIF
# include <Xm/DialogS.h>
# include <Xm/MessageB.h>
# include <Xm/PushB.h>
# include <Xm/Label.h>
# include <Xm/Form.h>
# include <Xm/MenuShell.h>
# include <Xm/Protocols.h>
# include <Xm/AtomMgr.h>
#else
# include <X11/Xaw/Paned.h>
# include <X11/Xaw/Box.h>
# include <X11/Xaw/MenuButton.h>
# include <X11/Xaw/SimpleMenu.h>
# include <X11/Xaw/Sme.h>
# include <X11/Xaw/SmeBSB.h>
# include <X11/Xaw/AsciiText.h>
# include <X11/Xaw/Dialog.h>
#endif

#include <stdarg.h>
#include "xdvi.h"
#include "util.h"
#include "string-utils.h"
#include "x_util.h"
#include "message-window.h"

    /* have no more than MAX_POPUPS open simultaneously */
#define MAX_POPUPS 10

    /* offset for cascading popups */
#define POPUP_OFFSET ((my_popup_num * 20))

#ifdef MOTIF
    /* wrap messages after MSG_WRAP_LEN characters at whitespace */
#define MSG_WRAP_LEN 60
#endif

    /* array of active popups: */
    static int g_popup_array[MAX_POPUPS];

static Atom WM_DELETE_WINDOW;

/* arrays for saving the widgets of multiple popups;
 * same index as in g_popup_array:
 */
#ifdef MOTIF
static Widget popup_window[MAX_POPUPS], dialog[MAX_POPUPS];
#else
static Widget popup_window[MAX_POPUPS], message_box[MAX_POPUPS],
    message_text[MAX_POPUPS], message_paned[MAX_POPUPS],
    message_ok[MAX_POPUPS], message_help[MAX_POPUPS], message_not_ok[MAX_POPUPS];
#endif

/* map popupMessageT's to strings/motif dialog elements */
static const struct message_map {
    const char *window_title;
#ifdef MOTIF
    int motif_msg_type;
#endif
} my_msg_map[] = {
    { "Xdvi Question"
#ifdef MOTIF
      , XmDIALOG_QUESTION
#endif
    },
    { "Xdvi Help"
#ifdef MOTIF
      , XmDIALOG_INFORMATION
#endif
    },
    { "Xdvi Info"
#ifdef MOTIF
      , XmDIALOG_INFORMATION
#endif
    },
    { "Xdvi Warning"
#ifdef MOTIF
      , XmDIALOG_WARNING
#endif
    },
    { "Xdvi Error"
#ifdef MOTIF
      , XmDIALOG_ERROR
#endif
    },
};

struct ok_or_cancel_cb {
    message_cbT callback; /* callback function */
    XtPointer arg;	/* arg for callback function */
};

struct pre_ok_or_cancel_cb {
    pre_message_cbT callback; /* callback function */
    XtPointer arg;	/* arg for callback function */
};

static struct ok_or_cancel_cb yes_callbacks[MAX_POPUPS];
static struct ok_or_cancel_cb no_callbacks[MAX_POPUPS];
static struct ok_or_cancel_cb cancel_callbacks[MAX_POPUPS];
static struct pre_ok_or_cancel_cb pre_callbacks[MAX_POPUPS];


static void
popdown_cancel(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    size_t idx;
    
    UNUSED(w);
    UNUSED(event);
    UNUSED(params);
    UNUSED(num_params);

    ASSERT(*num_params == 1, "Wrong number of parameters in callback");
    idx = strtoul(*params, (char **)NULL, 10);

    /* First call pre_message_cb with window widget ID
     * as additional parameter.
     */
    if (pre_callbacks[idx].callback != NULL) {
	pre_callbacks[idx].callback(popup_window[idx], pre_callbacks[idx].arg);
    }
    
    /* Then pop down window and mark its position as free, then
     * invoke the OK callback.  The reason for this is that the callback
     * may need to wait for open windows.
     */
    XtPopdown(popup_window[idx]);
    XtDestroyWidget(popup_window[idx]);
    g_popup_array[idx] = 0;
    XSync(DISP, True);

    /* invoke the callback if present */
    if (cancel_callbacks[idx].callback != NULL) {
	cancel_callbacks[idx].callback(cancel_callbacks[idx].arg);
    }
}

#ifndef MOTIF
static void
xaw_popdown(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    size_t idx;

    UNUSED(w);
    UNUSED(event);
    UNUSED(params);
    UNUSED(num_params);

    ASSERT(*num_params == 1, "Wrong number of parameters in callback");
    idx = strtoul(*params, (char **)NULL, 10);

    /*
      NOTE: first pop down window and mark its position as free, then
      invoke the callback.  The reason for this is that the callback
      may need to wait for open windows.
    */
    XtPopdown(popup_window[idx]);
    XtDestroyWidget(popup_window[idx]);
    g_popup_array[idx] = 0;
    XSync(DISP, True);
}

#endif

static XtActionsRec popdown_actions[] = {
    {"close-popup-cancel",	popdown_cancel },
#if !MOTIF
    {"WM_popdown",		popdown_cancel },
    {"close-popup",		xaw_popdown	   },
#endif
};

static void
ok_action(Widget w, XtPointer client_data, XtPointer call_data)
{
    XtPointer p;
    ptrdiff_t idx = -1;
    
    UNUSED(call_data);
    
#if MOTIF
    UNUSED(client_data);
    XtVaGetValues(w, XmNuserData, &p, NULL);
    idx = (ptrdiff_t)p;
    ASSERT(idx >= 0, "Couldn't get idx from XmNuserData!");
#else
    UNUSED(p);
    UNUSED(w);
    idx = (ptrdiff_t)client_data;
#endif
    
#if DEBUG
    fprintf(stderr, "ok_action called for popup %ld\n", idx);
#endif
    ASSERT(idx >= 0 && idx < MAX_POPUPS, "Invalid widget index in ok_action()");

    /* First call pre_message_cb with window widget ID
     * as additional parameter.
     */
    if (pre_callbacks[idx].callback != NULL) {
	pre_callbacks[idx].callback(popup_window[idx], pre_callbacks[idx].arg);
    }
    
    /* Then pop down window and mark its position as free, then
     * invoke the OK callback.  The reason for this is that the callback
     * may need to wait for open windows.
     */
    XtPopdown(popup_window[idx]);
    XtDestroyWidget(popup_window[idx]);
    g_popup_array[idx] = 0;
    XSync(DISP, True);

    if (yes_callbacks[idx].callback != NULL) {
	yes_callbacks[idx].callback(yes_callbacks[idx].arg);
    }
}

/*------------------------------------------------------------
 *  help_action
 *
 *  Arguments:
 *	Widget w, XtPointer call_data
 *		- (ignored)
 *	XtPointer client_data
 *		- the help string
 *
 *  Returns:
 *	void
 *
 *  Purpose:
 *	Callback for the `Help' button; opens another window
 *	containing the help text. The new window won't have
 *	another `Help' button.
 *------------------------------------------------------------*/

static void
help_action(Widget w, XtPointer client_data, XtPointer call_data)
{
    UNUSED(call_data);

    /* open another window with the help text */
    popup_message(get_matching_parent(w, globals.widgets.top_level,
	"message_popup", NULL), MSG_HELP, NULL, "%s", client_data);
}


/*
 * Callback for cancel button in choice_dialog.
 */
static void
cancel_action(Widget w, XtPointer client_data, XtPointer call_data)
{
    XtPointer p;
    ptrdiff_t idx = -1;
    
    UNUSED(call_data);

#if MOTIF
    /* for motif, the index is in XmNuserData */
    UNUSED(client_data);
    if (strcmp(XtName(w), Xdvi_MESSAGE_SHELL_NAME) == 0) {
	/* invoked by the WM, get the messagebox child */
	Widget child;
	if (get_widget_by_name(&child, w, Xdvi_MESSAGE_DIALOG_NAME, True)) {
	    XtVaGetValues(child, XmNuserData, &p, NULL);
	    idx = (ptrdiff_t)p;
	}
    }
    else {
	XtVaGetValues(w, XmNuserData, &p, NULL);
	idx = (ptrdiff_t)p;
    }
    ASSERT(idx >= 0, "Couldn't get idx from XmNuserData!");
#else
    UNUSED(p);
    UNUSED(w);
    idx = (ptrdiff_t)client_data;
#endif
    ASSERT(idx >= 0 && idx < MAX_POPUPS, "Invalid widget index in cancel_action()");

    /* First call pre_message_cb with window widget ID
     * as additional parameter.
     */
    if (pre_callbacks[idx].callback != NULL) {
	pre_callbacks[idx].callback(popup_window[idx], pre_callbacks[idx].arg);
    }
    
    /* Then pop down window and mark its position as free, then
     * invoke the OK callback.  The reason for this is that the callback
     * may need to wait for open windows.
     */
    XtPopdown(popup_window[idx]);
    XtDestroyWidget(popup_window[idx]);
    g_popup_array[idx] = 0;
    XSync(DISP, True);

    /* invoke the callback if present */
    if (cancel_callbacks[idx].callback != NULL) {
	cancel_callbacks[idx].callback(cancel_callbacks[idx].arg);
    }
}

#if MOTIF
static void
not_ok_action(Widget w, XtPointer client_data, XtPointer call_data)
{
    /* Note: unmanages the parent of the button */
    XtPointer p;
    ptrdiff_t idx = -1;

    UNUSED(client_data);

    UNUSED(call_data);
    XtVaGetValues(w, XmNuserData, &p, NULL);
    idx = (ptrdiff_t)p;
    ASSERT(idx >= 0, "Couldn't get idx from XmNuserData!");
    ASSERT(idx >= 0 && idx < MAX_POPUPS, "Invalid widget index in ok_action()");

    /* First call pre_message_cb with window widget ID
     * as additional parameter.
     */
    if (pre_callbacks[idx].callback != NULL) {
	pre_callbacks[idx].callback(popup_window[idx], pre_callbacks[idx].arg);
    }
    
    /* Then pop down window and mark its position as free, then
     * invoke the OK callback.  The reason for this is that the callback
     * may need to wait for open windows.
     */
    XtPopdown(popup_window[idx]);
    XtDestroyWidget(popup_window[idx]);
    g_popup_array[idx] = 0;
    XSync(DISP, True);

    if (no_callbacks[idx].callback != NULL) {
	no_callbacks[idx].callback(no_callbacks[idx].arg);
    }
}
#endif /* MOTIF */

/*------------------------------------------------------------
 *  create_dialogs
 *
 *  Arguments:
 *	Widget toplevel - parent for the dialog window
 *
 *	helptext	- if not-NULL, create an additional
 *			  `help' button
 *
 *	cnt		- number of current popup dialog
 *
 *  Returns:
 *	void
 *
 *  Purpose:
 *	Create message dialog widgets
 *------------------------------------------------------------*/

static Widget
create_dialogs(popupMessageSizeHintT size,
	       Widget parent,
	       int cnt,
	       const char *helptext,
	       pre_message_cbT pre_cb, XtPointer arg,
	       const char *yes_button, message_cbT yes_cb, XtPointer yes_arg,
	       const char *no_button, message_cbT no_cb, XtPointer no_arg,
	       const char *cancel_button, message_cbT cancel_cb, XtPointer cancel_arg)
{
    Widget new_popup_window;
    char *translations_str = NULL;
#ifdef MOTIF
    Widget new_dialog;
    UNUSED(size);
#else
    char *key_translations_str = NULL;
    int msg_w = 400, msg_h = 100;
    Widget new_message_paned, new_message_text, new_message_box, new_message_ok,
	new_message_help = 0, new_message_not_ok;
    XtTranslations wm_translations, key_translations;
#endif

    /* save callbacks to global arrays */
    pre_callbacks[cnt].callback = pre_cb;
    pre_callbacks[cnt].arg = arg;
    
    yes_callbacks[cnt].callback = yes_cb;
    yes_callbacks[cnt].arg = yes_arg;

    no_callbacks[cnt].callback = no_cb;
    no_callbacks[cnt].arg = no_arg;

    cancel_callbacks[cnt].callback = cancel_cb;
    cancel_callbacks[cnt].arg = cancel_arg;

    XtAddActions(popdown_actions, XtNumber(popdown_actions));

#ifndef MOTIF
    /* get index into WM_popdown arg */
    translations_str = get_string_va("<Message>WM_PROTOCOLS: WM_popdown(%d)", cnt);
    wm_translations = XtParseTranslationTable(translations_str);
    free(translations_str);
#endif

    if (!XtIsRealized(globals.widgets.top_level)) {
	/* If toplevel window hasn't been realized yet, create a new toplevel shell
	   (otherwise, setting visual/color map wouldn't work); use same application names
	   so that resource settings will also apply to this window.
	*/
	new_popup_window = XtVaAppCreateShell("xdvi", "Xdvi",
					      transientShellWidgetClass, DISP,
					      NULL);
    }
    else {
	new_popup_window = XtVaCreatePopupShell(Xdvi_MESSAGE_SHELL_NAME,
#ifdef MOTIF
						xmDialogShellWidgetClass, parent,
						XmNdeleteResponse, XmDO_NOTHING, /* we'll take care of that ourselves */
#else
						transientShellWidgetClass, parent,
						XtNx, 60,
						XtNy, 80,
						XtNtranslations, wm_translations,
						XtNaccelerators, G_accels_cr,
#endif
						XtNtransientFor, parent,
						XtNmappedWhenManaged, False,
						NULL);
    }
    
#ifdef MOTIF

    WM_DELETE_WINDOW = XmInternAtom(XtDisplay(new_popup_window), "WM_DELETE_WINDOW", False);
    XmAddWMProtocolCallback(new_popup_window, WM_DELETE_WINDOW, cancel_action, NULL);
    
    /* We also need to override the default ESC binding to use our internal
       housekeeping functions */
    translations_str = get_string_va("#override\n<Key>osfCancel:close-popup-cancel(%d)", cnt);
    /*      { */
    /*  	XtTranslations xlats; */
    /*  	char *translation_str = get_string_va("<Key>osfCancel:close-popup-cancel(%d)", cnt); */
    /*  	xlats = XtParseTranslationTable(translation_str); */
    /*  	free(translation_str); */
    /*  	XtOverrideTranslations(new_dialog, xlats); */
    /*      } */

    new_dialog = XtVaCreateWidget(Xdvi_MESSAGE_DIALOG_NAME, xmMessageBoxWidgetClass, new_popup_window,
				  XmNdialogType, XmDIALOG_WARNING, /* default */
				  XmNtraversalOn, True,
				  XmNhighlightOnEnter, True,
				  XmNuserData, cast_int_to_XtPointer(cnt),
				  XmNtranslations, XtParseTranslationTable(translations_str),
				  NULL);
    free(translations_str);
    XtAddCallback(new_dialog, XmNokCallback, ok_action, NULL);

    if (no_button != NULL) {
	Arg args[4];
	Widget b;
	XmString b_str = XmStringCreateLocalized((char *)no_button);
	XtSetArg(args[0], XmNlabelString, b_str);
	b = XmCreatePushButton(new_dialog, "no_button", args, 1);
	XtAddCallback(b, XmNactivateCallback, not_ok_action, NULL);
	XtManageChild(b);
    }
    
    if (cancel_button != NULL) {
	XmString cancel_label = XmStringCreateLtoR((char *)cancel_button, G_charset);
	XtVaSetValues(XmMessageBoxGetChild(new_dialog, XmDIALOG_CANCEL_BUTTON),
		      XmNlabelString, cancel_label, NULL);
	XmStringFree(cancel_label);
	XtAddCallback(new_dialog, XmNcancelCallback, cancel_action, NULL);
    }
    else {
	XtUnmanageChild(XmMessageBoxGetChild(new_dialog, XmDIALOG_CANCEL_BUTTON));
    }
    XtInstallAllAccelerators(new_dialog,
			     XmMessageBoxGetChild(new_dialog, XmDIALOG_OK_BUTTON));

    if (helptext != NULL) {
	XtAddCallback(new_dialog, XmNhelpCallback, help_action, (XtPointer)helptext);
    }
    else {
	XtUnmanageChild(XmMessageBoxGetChild(new_dialog, XmDIALOG_HELP_BUTTON));
    }

    if (yes_button != NULL) { /* change `OK' button label */
	XmString yes_label;
	yes_label = XmStringCreateLtoR((char *)yes_button, G_charset);
	XtVaSetValues(XmMessageBoxGetChild(new_dialog, XmDIALOG_OK_BUTTON),
		      XmNlabelString, yes_label, NULL);
	XmStringFree(yes_label);
    }
    
    /* insert the new widgets into the global arrays */
    dialog[cnt] = new_dialog;

#else /* MOTIF */
    switch (size) {
    case SIZE_SMALL:
	msg_w = 300;
	msg_h = 100;
	break;
    case SIZE_MEDIUM:
	msg_w = 430;
	msg_h = 160;
	break;
    case SIZE_LARGE:
	msg_w = 450;
	msg_h = 180;
	break;
    }
    WM_DELETE_WINDOW = XInternAtom(XtDisplay(new_popup_window), "WM_DELETE_WINDOW", False);
    
    new_message_paned = XtVaCreateManagedWidget("message_paned", panedWidgetClass, new_popup_window,
						XtNaccelerators, G_accels_cr,
						NULL);

    new_message_text = XtVaCreateManagedWidget("message_text", asciiTextWidgetClass, new_message_paned,
					       /* 					       XtNheight, 100, */
					       /* 					       XtNwidth, 400, */
					       XtNwidth, msg_w,
					       XtNheight, msg_h,
					       /* wrap horizontally instead of scrolling
						* TODO: this won't work for the first widget instance?
						*/
					       XtNwrap, XawtextWrapWord,
					       XtNscrollVertical, XAW_SCROLL_ALWAYS,
					       XtNeditType, XawtextRead,
					       XtNinput, True,
					       XtNdisplayCaret, False,
					       XtNleftMargin, 5,
					       XtNaccelerators, G_accels_cr,
					       NULL);

    /* box for the OK/Cancel button */
    new_message_box = XtVaCreateManagedWidget("message_box", formWidgetClass, new_message_paned,
					      /* resizing by user isn't needed */
					      XtNshowGrip, False,
					      XtNdefaultDistance, 6, /* some padding */
					      /* resizing the window shouldn't influence this box,
					       * but only the text widget
					       */
					      XtNskipAdjust, True,
					      XtNaccelerators, G_accels_cr,
					      NULL);

    new_message_ok = XtVaCreateManagedWidget(yes_button == NULL ? "OK" : yes_button,
					     commandWidgetClass, new_message_box,
					     XtNtop, XtChainTop,
 					     XtNbottom, XtChainBottom,
 					     XtNleft, XtChainLeft,
 					     XtNright, XtChainLeft,
					     XtNaccelerators, G_accels_cr,
					     NULL);
    /* add quit_action callback for the "OK" button */
    /* FIXME: how to make accelerators be accepted by new_popup_window as well? */
    key_translations_str = get_string_va("<Key>q:close-popup-cancel(%d)\n"
					 "<Key>Return:close-popup-cancel(%d)\n"
					 "<Key>Escape:close-popup-cancel(%d)\n",
					 cnt, cnt, cnt);
    key_translations = XtParseTranslationTable(key_translations_str);
    free(key_translations_str);
    XtOverrideTranslations(new_popup_window, key_translations);
    XtOverrideTranslations(new_message_paned, key_translations);
    XtOverrideTranslations(new_message_text, key_translations);
    
    XtInstallAllAccelerators(new_message_box, new_message_ok);
    XtAddCallback(new_message_ok, XtNcallback, ok_action, cast_int_to_XtPointer(cnt));

    /* we create additional buttons in any case,
       to make the sizing more consistent */
    new_message_help = XtVaCreateManagedWidget("Help", commandWidgetClass, new_message_box,
					       XtNtop, XtChainTop,
					       XtNfromHoriz, new_message_ok,
					       XtNbottom, XtChainBottom,
					       XtNleft, XtChainRight,
					       XtNright, XtChainRight,
					       XtNaccelerators, G_accels_cr,
					       NULL);
    message_help[cnt] = new_message_help;
    
    /* add cancel button */
    new_message_not_ok = XtVaCreateManagedWidget(cancel_button == NULL ? "Cancel" : cancel_button,
						 commandWidgetClass, new_message_box,
						 XtNtop, XtChainTop,
						 XtNfromHoriz, new_message_ok,
						 XtNbottom, XtChainBottom,
						 XtNleft, helptext == NULL ? XtChainRight : XtChainLeft,
						 XtNright, helptext == NULL ? XtChainRight : XtChainLeft,
						 XtNaccelerators, G_accels_cr,
						 NULL);
    message_not_ok[cnt] = new_message_not_ok;

    if (no_button != NULL) {
	ASSERT(0, "third button not yet implemented in Xaw!!!");
    }

    adjust_width_to_max(new_message_ok, new_message_help, new_message_not_ok, NULL);
    
    /* if helptext argument is not-NULL, add help_action callback,
       else unmanage help button */
    if (helptext != NULL) {
	XtAddCallback(new_message_help, XtNcallback, help_action, (XtPointer)helptext);
    }
    else {
	XtUnmanageChild(new_message_help);
    }

    if (cancel_button != NULL) {
	XtAddCallback(new_message_not_ok, XtNcallback, cancel_action, cast_int_to_XtPointer(cnt));
    }
    else {
	XtUnmanageChild(new_message_not_ok);
    }
    /* insert the new widgets into the global arrays */
    message_box[cnt] = new_message_box;
    message_paned[cnt] = new_message_paned;
    message_text[cnt] = new_message_text;
    message_ok[cnt] = new_message_ok;

#endif /* MOTIF */
    popup_window[cnt] = new_popup_window;

    return new_popup_window;
}


/*
 * Popup a window with wrapped text in it.
 * For Motif, the text is explicitly wrapped inside this method.
 */
static Widget
internal_popup_window(Widget parent,
		      popupMessageSizeHintT size,
		      popupMessageT type,
		      int x_coord, int y_coord,
		      const char *helptext,
		      char *msg_buf,
#ifndef MOTIF
		      const char *xaw_ret_action_str,
#endif
		      pre_message_cbT pre_cb, XtPointer arg,
		      const char *yes_button, message_cbT yes_cb, XtPointer yes_arg,
		      const char *no_button, message_cbT no_cb, XtPointer no_arg,
		      const char *cancel_button, message_cbT cancel_cb, XtPointer cancel_arg)
{
    int my_popup_num = 0;
#ifdef MOTIF
    XmString str;
#endif
    Widget ret;

    ASSERT(type < (sizeof my_msg_map / sizeof my_msg_map[0]), "too few elements in my_msg_map");

#if DEBUG
    fprintf(stderr, "internal_popup_window called with prompt: \"%s\"\n", msg_buf);
#endif

    if (globals.widgets.top_level == 0) {
	/* If toplevel window hasn't been created yet, dump messages to STDERR
	   and return.
	*/
	fprintf(stderr, "\n%s:\n%s\n", my_msg_map[type].window_title, msg_buf);
	if (helptext) {
	    fputs("---------- helptext ----------\n", stderr);
	    fputs(helptext, stderr);
	    fputs("\n---------- end of helptext ----------\n", stderr);
	}
	return NULL;
    }
    /* search for first free position in g_popup_array */
    while (my_popup_num < MAX_POPUPS && (g_popup_array[my_popup_num] == 1)) {
	my_popup_num++;
    }
    if (my_popup_num == MAX_POPUPS) {
	/* already enough popups on screen, just dump it to stderr */
	fprintf(stderr, "%s: %s\n", my_msg_map[type].window_title, msg_buf);
	/* Note: If a mad function continues to open popups, this will
	 * stop after MAX_POPUPS, but open a new window for each
	 * window the user pops down. Maybe we ought to do something
	 * about this.
	 */
	return NULL;
    }
    else {
	/* mark it as non-free */
	g_popup_array[my_popup_num] = 1;
    }
#if DEBUG
    fprintf(stderr, "first free position in g_popup_array: %d\n", my_popup_num);
#endif

    /* just to make sure ... */
    if (parent == NULL)
	parent = globals.widgets.top_level;

    /* create a new set of widgets for the additional popup. */
    ret = create_dialogs(size, parent,
			 my_popup_num,
			 helptext,
			 pre_cb, arg,
			 yes_button, yes_cb, yes_arg,
			 no_button, no_cb, no_arg,
			 cancel_button, cancel_cb, cancel_arg);
#ifdef MOTIF
    XtVaSetValues(popup_window[my_popup_num], XmNtitle,
	    my_msg_map[type].window_title, NULL);
    XtVaSetValues(dialog[my_popup_num], XmNdialogType,
	    my_msg_map[type].motif_msg_type, NULL);
    { /* wrap message at space before MSG_WRAP_LEN */
	char *testwrap = msg_buf;
	int ctr;
	for (ctr = 0; *testwrap++; ctr++) {
	    if (*testwrap == '\n') {
		ctr = 0;
	    }
	    else if (ctr > MSG_WRAP_LEN) {
		size_t before_len = 0, after_len = 0;
		char *before_ptr, *after_ptr;
		before_ptr = after_ptr = testwrap;
		/* try to find shortest sequence before or after point to wrap at;
		   this seems to give the most pleasing results.
		*/
		while (before_ptr > msg_buf && !isspace((int)*--before_ptr)) {
		    before_len++;
		}
		while (*after_ptr != '\0' && !isspace((int)*++after_ptr)) {
		    after_len++;
		}

		if (before_len < after_len && isspace((int)*before_ptr)) {
		    /* use last in sequence of multiple spaces */
		    while (isspace((int)*++before_ptr)) { ; }
		    /* back up, and wrap */
		    *--before_ptr = '\n';
		    ctr = 0;
		}
		else if (isspace((int)*after_ptr)) {
		    /* use last in sequence of multiple spaces */
		    while (isspace((int)*++after_ptr)) { ; }
		    /* back up, and wrap */
		    *--after_ptr = '\n';
		    ctr = 0;
		}
	    }
	}
    }
    str = XmStringCreateLtoR((char *)msg_buf, G_charset);
    XtVaSetValues(dialog[my_popup_num],
		  XmNmessageString, str,
		  XmNtraversalOn, True,
		  XmNhighlightOnEnter, True,
		  NULL);
    XmStringFree(str);

    XtManageChild(dialog[my_popup_num]);
    
    if (x_coord > 0 && y_coord > 0) {
	position_window(XtParent(dialog[my_popup_num]), (Position)x_coord, (Position)y_coord);
    }
    
    XtPopup(XtParent(dialog[my_popup_num]), XtGrabNone);
    /*      XtPopup(XtParent(dialog[my_popup_num]), XtGrabExclusive); */

#else /* MOTIF */

    /* add a binding of xaw_ret_action_str to <Return> to relevant widgets.
       The callbacks (xaw_ret_action_str) are responsible for parsing the
       passed arguments (pointers, or empty arguments).       
    */
    if (xaw_ret_action_str != NULL) {
	XtTranslations xlats;
	char *translation_str;

	if (yes_arg != NULL)
	    translation_str = get_string_va("<Key>Return:close-popup(%d)%s(%p)",
					    my_popup_num, xaw_ret_action_str, yes_arg);
	else
	    translation_str = get_string_va("<Key>Return:close-popup(%d)%s()",
					    my_popup_num, xaw_ret_action_str);
	
	xlats = XtParseTranslationTable(translation_str);
	free(translation_str);
	XtOverrideTranslations(popup_window[my_popup_num], xlats);
	XtOverrideTranslations(message_paned[my_popup_num], xlats);
	XtOverrideTranslations(message_text[my_popup_num], xlats);
    }
    
    XtVaSetValues(popup_window[my_popup_num], XtNtitle,
	    my_msg_map[type].window_title, NULL);
    XtVaSetValues(message_text[my_popup_num], XtNstring, msg_buf, NULL);
    XtRealizeWidget(popup_window[my_popup_num]);

    XSetWMProtocols(XtDisplay(popup_window[my_popup_num]), XtWindow(popup_window[my_popup_num]),
		    &WM_DELETE_WINDOW, 1);

    if (x_coord <= 0 || y_coord <= 0)
	center_window(popup_window[my_popup_num], parent);
    else
	position_window(popup_window[my_popup_num], (Position)x_coord, (Position)y_coord);
    
    if (my_popup_num > 0) {
	/* some window managers position new windows exactly above the
	   existing one; to prevent this, move it with some offset
	   from the previous one: */
	Position x = 0, y = 0;
	XtVaGetValues(popup_window[my_popup_num-1], XtNx, &x, XtNy, &y, NULL);
	XtVaSetValues(popup_window[my_popup_num], XtNx, x + POPUP_OFFSET, XtNy, y + POPUP_OFFSET, NULL);

    }
    XtPopup(popup_window[my_popup_num], XtGrabNone);
    /*      XtPopup(XtParent(popup_window[my_popup_num]), XtGrabExclusive); */
    if (XtIsManaged(message_not_ok[my_popup_num]) && XtIsManaged(message_help[my_popup_num])) {
	/* center the help button. This is something of a sham, since it won't
	   survive resizing; but in general most users won't resize dialogs ;-) */
	Position x1, x2, bw;
	int w, dist;
	
	XtVaGetValues(message_ok[my_popup_num], XtNx, &x1, XtNwidth, &w, XtNborderWidth, &bw, NULL);
	XtVaGetValues(message_help[my_popup_num], XtNx, &x2, NULL);
	/* following formula is measured, not calculated -
	   I have no idea why it's e.g. 2 * w, not 1.5 * w ... */
	dist = (x2 - x1 - 2 * w) / 2 - 2 * bw;
	XtVaSetValues(message_not_ok[my_popup_num], XtNhorizDistance, dist, NULL);
    }
    
#endif /* MOTIF */
    return ret;
}


/*------------------------------------------------------------
 *  popup_message
 *
 *  Arguments:
 *	popupMessageT - info, warning, error etc; see message-window.h for details
 *
 *	char *helptext
 *	      - if not-null, this will add a `Help'
 *		button to the message widget that pops
 *		up another message widget containing
 *		<helptext>.
 *
 *
 *	char *msg, ...
 *	      - format string followed by a variable
 *		number of arguments to be formatted.
 *
 *  Returns:
 *	void
 *
 *  Purpose:
 *	Pop up a message window containing <msg>.
 *	If there are already n popups open, will open
 *	a new one unless n >= MAX_POPUPS.
 *------------------------------------------------------------*/

Widget
popup_message(Widget parent, popupMessageT type, const char *helptext, const char *format, ...)
{
    char *msg_buf = NULL;
    Widget w;

    XDVI_GET_STRING_ARGP(msg_buf, format);

    w = internal_popup_window(parent,
			      SIZE_SMALL,
			      type,
			      -1, -1, /* just center it */
			      helptext, msg_buf,
#ifndef MOTIF
			      NULL,
#endif
			      /* no special callbacks here */
			      NULL, NULL,
			      NULL, NULL, NULL,
			      NULL, NULL, NULL,
			      NULL, NULL, NULL);
    free(msg_buf);
    return w;
}

#if 0	/* This function is currently unused. */
Widget
popup_message_sized(Widget parent,
		    popupMessageT type,
		    popupMessageSizeHintT sizehint,
		    const char *helptext,
		    const char *format, ...)
{
    char *msg_buf = NULL;
    Widget w;
    
    XDVI_GET_STRING_ARGP(msg_buf, format);

    w = internal_popup_window(parent,
			      sizehint,
			      type,
			      -1, -1, /* just center it */
			      helptext, msg_buf,
#ifndef MOTIF
			      NULL,
#endif
			      /* empty callbacks */
			      NULL, NULL,
			      NULL, NULL, NULL,
			      NULL, NULL, NULL,
			      NULL, NULL, NULL);
    free(msg_buf);
    return w;
}
#endif

Widget
positioned_popup_message(Widget parent,
			 popupMessageT type,
			 int x, int y,
			 const char *helptext, const char *format, ...)
{
    char *msg_buf = NULL;
    Widget w;
    
    XDVI_GET_STRING_ARGP(msg_buf, format);

    w = internal_popup_window(parent,
			      SIZE_SMALL,
			      type,
			      x, y, /* position at these coordinates */
			      helptext, msg_buf,
#ifndef MOTIF
			      NULL,
#endif
			      /* empty callbacks */
			      NULL, NULL,
			      NULL, NULL, NULL,
			      NULL, NULL, NULL,
			      NULL, NULL, NULL);
    free(msg_buf);
    return w;
}

Widget
choice_dialog(Widget parent,
	      popupMessageT type,
	      const char *helptext,
#ifndef MOTIF
	      const char *xaw_ret_action_str,
#endif
	      pre_message_cbT pre_cb, XtPointer arg,
	      const char *ok_label, message_cbT ok_cb, XtPointer ok_arg,
	      const char *cancel_label, message_cbT cancel_cb, XtPointer cancel_arg,
	      const char *format, ...)
{
    char *msg_buf = NULL;
    Widget w;
    
    XDVI_GET_STRING_ARGP(msg_buf, format);

    w = internal_popup_window(parent,
			      SIZE_SMALL,
			      type,
			      -1, -1, /* just center it */
			      helptext, msg_buf,
#ifndef MOTIF
			      xaw_ret_action_str,
#endif
			      pre_cb, arg,
			      ok_label, ok_cb, ok_arg,
			      NULL, NULL, NULL,
			      cancel_label, cancel_cb, cancel_arg);
    free(msg_buf);
    return w;
}

#if MOTIF
Widget
choice3_dialog(Widget parent,
	       popupMessageT type,
	       const char *helptext,
	       pre_message_cbT pre_cb, XtPointer arg,
	       const char *yes_label, message_cbT yes_cb, XtPointer yes_arg,
	       const char *no_label, message_cbT no_cb, XtPointer no_arg,
	       const char *cancel_label, message_cbT cancel_cb, XtPointer cancel_arg,
	       const char *format, ...)
{
    char *msg_buf = NULL;
    Widget w;
    
    XDVI_GET_STRING_ARGP(msg_buf, format);

    w = internal_popup_window(parent,
			      SIZE_SMALL,
			      type,
			      -1, -1, /* just center it */
			      helptext, msg_buf,
			      pre_cb, arg,
			      yes_label, yes_cb, yes_arg,
			      no_label, no_cb, no_arg,
			      cancel_label, cancel_cb, cancel_arg);
    free(msg_buf);
    return w;
}
#endif

Widget
choice_dialog_sized(Widget parent,
		    popupMessageT type,
		    popupMessageSizeHintT sizehint,
		    const char *helptext,
#ifndef MOTIF
		    const char *xaw_ret_action_str,
#endif
		    pre_message_cbT pre_cb, XtPointer arg,
		    const char *ok_label, message_cbT ok_cb, XtPointer ok_arg,
		    const char *cancel_label, message_cbT cancel_cb, XtPointer cancel_arg,
		    const char *format, ...)
{
    char *msg_buf = NULL;
    Widget w;
    
    XDVI_GET_STRING_ARGP(msg_buf, format);

    w = internal_popup_window(parent,
			      sizehint,
			      type,
			      -1, -1, /* just center it */
			      helptext, msg_buf,
#ifndef MOTIF
			      xaw_ret_action_str,
#endif
			      pre_cb, arg,
			      ok_label, ok_cb, ok_arg,
			      NULL, NULL, NULL,
			      cancel_label, cancel_cb, cancel_arg);
    free(msg_buf);
    return w;
}

Widget
positioned_choice_dialog(Widget parent,
			 popupMessageT type,
			 int x_pos, int y_pos,
			 const char *helptext,
#ifndef MOTIF
			 const char *xaw_ret_action_str,
#endif
			 pre_message_cbT pre_cb, XtPointer arg,
			 const char *ok_label, message_cbT ok_cb, XtPointer ok_arg,
			 const char *cancel_label, message_cbT cancel_cb, XtPointer cancel_arg,
			 const char *format, ...)
{
    char *msg_buf = NULL;
    Widget w;
    
    XDVI_GET_STRING_ARGP(msg_buf, format);

    w = internal_popup_window(parent,
			      SIZE_SMALL,
			      type,
			      x_pos, y_pos,
			      helptext, msg_buf,
#ifndef MOTIF
			      xaw_ret_action_str,
#endif
			      pre_cb, arg,
			      ok_label, ok_cb, ok_arg,
			      NULL, NULL, NULL,
			      cancel_label, cancel_cb, cancel_arg);
    free(msg_buf);
    return w;
}

void
warn_overstrike(void)
{
    static Boolean warned_overstrike = False;

    if (!warned_overstrike) {
	popup_message(globals.widgets.top_level,
		      MSG_WARN,
		      /* helptext */
		      "Greyscaling is running in copy mode; this will cause overstrike characters to "
		      "appear incorrectly, and may result in poor display quality.  "
		      "Possible fixes are:\n"
		      "- Use the ``-thorough'' command-line option.\n"
		      "- Quit some other color-hungry applications (e.g. Netscape).\n"
		      "- Use the ``-install'' command-line option.\n"
		      "See the section ``GREYSCALING AND COLORMAPS'' in the "
		      "xdvi manual page for more details.",
		      /* text */
		      "Couldn't allocate enough colors - expect low display quality.");
	warned_overstrike = True;
    }
}

Boolean
is_message_window(Widget w)
{
    int i;
    for (i = 0; i < MAX_POPUPS; i++) {
	if (w == popup_window[i])
	    return True;
    }
    return False;
}

Boolean
kill_message_window(Widget w)
{
    int i;
    for (i = 0; i < MAX_POPUPS; i++) {
	if (g_popup_array[i] != 0 && XtIsRealized(popup_window[i]) && w == popup_window[i]) {
	    g_popup_array[i] = 0;
	    XtPopdown(popup_window[i]);
	    XtDestroyWidget(popup_window[i]);
	    XSync(DISP, True);
	    return True;
	}
    }
    return False;
}

/*
  Raise any popups that currently exist; return True iff such popups
  exist, else False.
*/
Boolean
raise_message_windows(void)
{
    int i;
    Boolean have_popups = False;
    
    for (i = 0; i < MAX_POPUPS; i++) {
	if (g_popup_array[i] != 0 && XtIsRealized(popup_window[i])) {
	    XRaiseWindow(DISP, XtWindow(popup_window[i]));
	    have_popups = True;
	}
    }
    
    return have_popups;
}
