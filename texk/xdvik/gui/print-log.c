/*
 * Copyright (c) 2002-2004  Paul Vojta and the xdvik development team
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


/*
  FIXME: The printlog presents too much detail and is confusing for users.
  Re-implement it using an `printing page n of m' message,
  i.e. according to the spec on:
  http://xdvi.sourceforge.net/gui.html#file-print
  Possible save the detailed log to a different window that can be viewed
  via `File -> logs', as described on:
  http://xdvi.sourceforge.net/gui.html#file-logs
*/

#include "xdvi-config.h"
#include <string.h>
#include "xdvi.h"

#include "xdvi-debug.h"
#include "print-dialog.h"
#include "print-internal.h"
#include "print-log.h" /* for adjust_vertically() */
#include "util.h"
#include "string-utils.h"
#include "x_util.h"
#include "xlwradio.h"

#include <X11/Xatom.h>
#include <X11/StringDefs.h>

#ifdef MOTIF
# include <Xm/BulletinB.h>
# include <Xm/DialogS.h>
# include <Xm/MessageB.h>
# include <Xm/LabelG.h>
# include <Xm/Form.h>
# include <Xm/Frame.h>
# include <Xm/ToggleBG.h>
# include <Xm/PanedW.h>
# include <Xm/Text.h>
# include <Xm/TextF.h>
# include <Xm/PushB.h>
# include <Xm/Protocols.h>
# include <Xm/AtomMgr.h>
#else
# include <X11/Shell.h>
# include <X11/Xaw/Paned.h>
# include <X11/Xaw/Box.h>
# include <X11/Xaw/Form.h>
# include <X11/Xaw/Label.h>
# include <X11/Xaw/Command.h>
# include <X11/Xaw/Toggle.h>
# include <X11/Xaw/AsciiText.h>

/* XawFmt8Bit is only available starting with X11R6: */
# if XtSpecificationRelease < 6
#  define XawFmt8Bit    FMT8BIT
# endif

#endif /* MOTIF */


static void printlog_act_close(Widget, XEvent *, String *, Cardinal *);
static void printlog_act_keep(Widget, XEvent *, String *, Cardinal *);
static void printlog_act_unkeep(Widget, XEvent *, String *, Cardinal *);
static void printlog_act_cancel(Widget, XEvent *, String *, Cardinal *);
static void printlog_act_cancel_or_destroy(Widget, XEvent *, String *, Cardinal *);

static Atom WM_DELETE_WINDOW;

static XtActionsRec printlog_actions[] = {
    {"printlogIntClose",	printlog_act_close},
    {"printlogIntKeep",		printlog_act_keep},
    {"printlogIntUnkeep",	printlog_act_unkeep},
    {"printlogIntCancel",	printlog_act_cancel},
    {"WM_cancel_or_destroy",	printlog_act_cancel_or_destroy },
};

static const char *const PRINTLOG_WINDOW_TRANSLATIONS =
#ifdef MOTIF
"<Key>osfCancel:WM_cancel_or_destroy(%p)"
#else
"<Key>Escape:WM_cancel_or_destroy(%p)"
#endif
;

#ifdef MOTIF
static const char *const PRINTLOG_WINDOW_OVERRIDE_TRANSLATIONS =
"#override\n"
"<Key>osfCancel:WM_cancel_or_destroy(%p)";
#endif


static const char *const PRINTLOG_TEXT_TRANSLATIONS =
#ifdef MOTIF
"<Key>osfCancel:WM_cancel_or_destroy(%p)\n"
#else
"<Key>Escape:WM_cancel_or_destroy(%p)\n"
#endif
"<Key>Return:printlogIntClose(%p)\n"
"^<Key>c:printlogIntCancel(%p)\n"
"^<Key>s:printlogIntKeep(%p)\n"
"^<Key>q:printlogIntUnkeep(%p)\n"
"<Key>q:printlogIntClose(%p)";

#define Xdvi_PRINTLOG_TEXT_NAME "text"
#define Xdvi_PRINTLOG_CLOSE_NAME "close"
#define Xdvi_PRINTLOG_KEEP_NAME "keep"
#define Xdvi_PRINTLOG_CANCEL_NAME "cancel"

static void
cb_printlog_act_cancel_or_destroy(Widget w, XtPointer client_data, XtPointer call_data)
{
    struct save_or_print_info *info = (struct save_or_print_info *)client_data;
    Widget cancel_button = NULL;
    
    UNUSED(w);
    UNUSED(call_data);

    ASSERT(info->callbacks != NULL && info->callbacks->cb_destroy != NULL, "Callback not initialized");

    /* If the `Cancel' button is still active, cancel printing, else close the window. */
    if (get_widget_by_name(&cancel_button, info->printlog, Xdvi_PRINTLOG_CANCEL_NAME, True)) {
	Boolean sensitive = False;
	
	XtVaGetValues(cancel_button,
#ifdef MOTIF
		      XmNsensitive,
#else
		      XtNsensitive,
#endif
		      &sensitive,
		      NULL);
	fprintf(stderr, "Sensitive: %d\n", sensitive);
	if (sensitive)
	    info->callbacks->cb_cancel(w, info, NULL);
	else
	    info->callbacks->cb_destroy(w, info, NULL);
    }    
}


Boolean
printlog_raise_active(struct save_or_print_info *info)
{
    Widget w = info->printlog;
    if (w != NULL && window_is_mapped(XtWindow(w), DISP)) {
	XRaiseWindow(DISP, XtWindow(w));
	return True;
    }
    return False;
}

void
printlog_popup(struct save_or_print_info *info)
{
    Widget w = info->printlog;
#ifdef MOTIF
    Widget cancel, text;
#endif

#ifdef MOTIF
    char *text_translations = get_string_va(PRINTLOG_TEXT_TRANSLATIONS, info, info, info, info, info, info);
    char *window_translations = get_string_va(PRINTLOG_WINDOW_TRANSLATIONS, info);
#endif
    
    printlog_reset(info);

    center_window(w, globals.widgets.top_level);
    XtMapWidget(w);
    /*     XtPopup(w, XtGrabNone); */
#ifndef MOTIF
    XSetWMProtocols(XtDisplay(w), XtWindow(w), &WM_DELETE_WINDOW, 1);
#else
    XtOverrideTranslations(w, XtParseTranslationTable(window_translations));
    
    if (get_widget_by_name(&cancel, w, Xdvi_PRINTLOG_CANCEL_NAME, True)
	&& get_widget_by_name(&text, w, Xdvi_PRINTLOG_TEXT_NAME, True)) {

	/* This breaks PRINTLOG_WINDOW_OVERRIDE_TRANSLATIONS??
	 * We don't want it activated on Return anyway ...
	 */
	/* XmProcessTraversal(cancel, XmTRAVERSE_CURRENT); */
	XtOverrideTranslations(text, XtParseTranslationTable(text_translations));
	XtOverrideTranslations(text, XtParseTranslationTable(window_translations));
	free(text_translations);
	free(window_translations);
    }
#endif
}

void
printlog_reset(struct save_or_print_info *info)
{
    Widget w = info->printlog;
    Widget text;

    if (get_widget_by_name(&text, w, Xdvi_PRINTLOG_TEXT_NAME, True)) {
#ifndef MOTIF
	XtVaSetValues(text, XtNstring, "", NULL);
	XawTextSetInsertionPoint(text, 0);
#else
	XmTextSetString(text, "");
	XtVaSetValues(text, XmNcursorPosition, 0, NULL);
#endif
    }
}


void
printlog_create(struct save_or_print_info *info,
		const char *title,
		const char *close_label)
{
    Widget printlog_text, printlog_close, printlog_keep, printlog_cancel;
    Widget form, paned, box;
    char *window_translation_str;

#ifdef MOTIF
    XmString str;
    XtTranslations window_override_translations;
#else
    char *text_translation_str = NULL;
    XtTranslations wm_translations;
    XtTranslations window_translations;
#endif

#ifdef MOTIF
    window_translation_str = get_string_va(PRINTLOG_WINDOW_OVERRIDE_TRANSLATIONS, info);
    window_override_translations = XtParseTranslationTable(window_translation_str);
    free(window_translation_str);
#else
    window_translation_str = get_string_va(PRINTLOG_WINDOW_TRANSLATIONS, info);
    window_translations = XtParseTranslationTable(window_translation_str);
    free(window_translation_str);
#endif
    
    XtAddActions(printlog_actions, XtNumber(printlog_actions));
    
#ifndef MOTIF
	
    wm_translations = XtParseTranslationTable("<Message>WM_PROTOCOLS: WM_cancel_or_destroy()");
    info->printlog = XtVaCreatePopupShell("printlog",
					  transientShellWidgetClass, globals.widgets.top_level,
					  XtNtitle, title,
					  XtNtransientFor, globals.widgets.top_level,
					  XtNmappedWhenManaged, False,
					  XtNtranslations, wm_translations,
					  XtNtransientFor, globals.widgets.top_level,
					  NULL);

    WM_DELETE_WINDOW = XInternAtom(XtDisplay(info->printlog), "WM_DELETE_WINDOW", False);

    XtOverrideTranslations(info->printlog, window_translations);
	
    paned = XtVaCreateManagedWidget("paned", panedWidgetClass, info->printlog, NULL);
	
    form = XtVaCreateManagedWidget("form", formWidgetClass,
				   paned,
				   XtNdefaultDistance, 6,
				   NULL);
    printlog_text = XtVaCreateManagedWidget(Xdvi_PRINTLOG_TEXT_NAME,
					    asciiTextWidgetClass, form,
					    XtNstring, "",
					    XtNdataCompression, False,
					    XtNeditType, XawtextAppend,
					    XtNscrollHorizontal, XawtextScrollAlways,
					    XtNscrollVertical, XawtextScrollAlways,
					    XtNwidth, 600,
					    XtNheight, 400,
					    XtNleft, XawChainLeft,
					    XtNright, XawChainRight,
					    XtNtop, XawChainTop,
					    XtNbottom, XawChainBottom,
					    NULL);
    text_translation_str = get_string_va(PRINTLOG_TEXT_TRANSLATIONS, info, info, info, info, info, info);
    XtOverrideTranslations(printlog_text, XtParseTranslationTable(text_translation_str));
    free(text_translation_str);

    printlog_keep = XtVaCreateManagedWidget(Xdvi_PRINTLOG_KEEP_NAME,
#ifdef XAW
					    radioWidgetClass,
#else
					    toggleWidgetClass,
#endif
					    form,
					    XtNlabel, close_label,
					    XtNborderWidth, 0,
#ifdef XAW
					    XtNisRadio, False,
#endif
					    XtNhighlightThickness, 1,
					    XtNfromVert, printlog_text,
					    XtNleft, XawChainLeft,
					    XtNright, XawChainLeft,
					    XtNtop, XawChainBottom,
					    XtNbottom, XawChainBottom,
					    NULL);
    XtVaSetValues(printlog_keep,
		  XtNstate, resource.dvips_hang > 0 && resource.dvips_fail_hang > 0,
		  NULL);
    XtAddCallback(printlog_keep, XtNcallback, info->callbacks->cb_keep, (XtPointer)info);
	
    /* box for the Close/Cancel button */
    box = XtVaCreateManagedWidget("box", formWidgetClass,
				  paned,
				  /* resizing by user isn't needed */
				  XtNshowGrip, False,
				  XtNdefaultDistance, 6, /* some padding */
				  /* resizing the window shouldn't influence this box,
				   * but  only the pane widget
				   */
				  XtNskipAdjust, True,
				  XtNaccelerators, G_accels_cr,
				  NULL);
	
    printlog_close = XtVaCreateManagedWidget(Xdvi_PRINTLOG_CLOSE_NAME,
					     commandWidgetClass, box,
					     XtNlabel, "Close",
					     XtNsensitive, False,
					     XtNleft, XawChainLeft,
					     XtNright, XawChainLeft,
					     XtNtop, XawChainBottom,
					     XtNbottom, XawChainBottom,
					     NULL);
    XtAddCallback(printlog_close, XtNcallback, info->callbacks->cb_close, (XtPointer)info);
	
    printlog_cancel = XtVaCreateManagedWidget(Xdvi_PRINTLOG_CANCEL_NAME,
					      commandWidgetClass, box,
					      XtNlabel, "Cancel",
					      XtNfromHoriz, printlog_keep,
					      XtNleft, XawChainRight,
					      XtNright, XawChainRight,
					      XtNtop, XawChainBottom,
					      XtNbottom, XawChainBottom,
					      NULL);
    XtAddCallback(printlog_cancel, XtNcallback, info->callbacks->cb_cancel, (XtPointer)info);
    XtManageChild(info->printlog);

    printlog_enable_cancelbutton(info);
	
#else /* MOTIF */
    info->printlog = XtVaCreatePopupShell("printlog",
					  xmDialogShellWidgetClass, globals.widgets.top_level,
					  XmNdeleteResponse, XmDO_NOTHING, /* we'll take care of that ourselves */
					  XmNtitle, title,
					  XtNmappedWhenManaged, False,
					  XmNtranslations, window_override_translations,
					  NULL);

    WM_DELETE_WINDOW = XmInternAtom(XtDisplay(info->printlog), "WM_DELETE_WINDOW", False);
    XmAddWMProtocolCallback(info->printlog, WM_DELETE_WINDOW, cb_printlog_act_cancel_or_destroy, (XtPointer)info);

    paned = XtVaCreateWidget("printlog_pane", xmPanedWindowWidgetClass, info->printlog,
			     /* make sashes invisible */
			     XmNsashWidth, 1,
			     XmNsashHeight, 1,
			     XmNtranslations, window_override_translations,
			     NULL);
	
    form = XtVaCreateWidget("form", xmFormWidgetClass, paned,
			    XmNhorizontalSpacing, DDIST,
			    XmNverticalSpacing, DDIST,
			    XmNautoUnmanage, False,
			    XmNtranslations, window_override_translations,
			    NULL);
    /* force vertical scrollbars. Under Motif 2.x (2.1.0 and 2.2.2 at least),
       XmNeditMode must be set early in order to have an effect. */
    {
	Arg args[10];
	int n = 0;
	XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT);	n++;
	XtSetArg(args[n], XmNeditable, False);			n++;
	XtSetArg(args[n], XmNrows, 24);				n++;
	XtSetArg(args[n], XmNcolumns, 80);			n++;
	
	printlog_text = XmCreateScrolledText(form, Xdvi_PRINTLOG_TEXT_NAME, args, n);
    }

    XtVaSetValues(XtParent(printlog_text),
		  XmNtopAttachment, XmATTACH_FORM,
		  XmNleftAttachment, XmATTACH_FORM,
		  XmNrightAttachment, XmATTACH_FORM,
		  XmNtranslations, window_override_translations,
		  NULL);

    XtManageChild(printlog_text);

    str = XmStringCreateLocalized((char *)close_label);
    printlog_keep = XtVaCreateManagedWidget(Xdvi_PRINTLOG_KEEP_NAME,
					    xmToggleButtonGadgetClass, form,
					    XmNlabelString, str,
					    XmNindicatorType, XmN_OF_MANY,
					    XmNnavigationType, XmTAB_GROUP,
					    XmNtopAttachment, XmATTACH_WIDGET,
					    XmNtopWidget, XtParent(printlog_text),
					    XmNleftAttachment, XmATTACH_FORM,
					    XmNtranslations, window_override_translations,
					    NULL);
    XmStringFree(str);
    XtAddCallback(printlog_keep, XmNvalueChangedCallback, info->callbacks->cb_keep, (XtPointer)info);
    XmToggleButtonGadgetSetState(printlog_keep, resource.dvips_hang > 0 && resource.dvips_fail_hang > 0, False);

    /* box for Close/Cancel button */
    box = XtVaCreateManagedWidget("box", xmFormWidgetClass,
				  paned,
				  NULL);
	
    str = XmStringCreateLocalized("Close");
    printlog_close = XtVaCreateManagedWidget(Xdvi_PRINTLOG_CLOSE_NAME,
					     xmPushButtonWidgetClass, box,
					     XmNlabelString, str,
					     XmNshowAsDefault, True,
					     XmNsensitive, False,
					     XmNdefaultButtonShadowThickness, 1,
					     XmNtopAttachment, XmATTACH_FORM,
					     XmNbottomAttachment, XmATTACH_FORM,
					     XmNleftAttachment, XmATTACH_FORM,
					     /* mimick appearance of native dialog buttons (better way would be
						to extend existing dialog with custom widgets, as in search-dialog.c) */
					     XmNmarginWidth, 6,
					     XmNmarginHeight, 4,
					     XmNtopOffset, 10,
					     XmNbottomOffset, 10,
					     XmNleftOffset, 10,
					     XmNtranslations, window_override_translations,
					     NULL);
    XmStringFree(str);
    XtAddCallback(printlog_close, XmNactivateCallback, info->callbacks->cb_close, (XtPointer)info);
    XtOverrideTranslations(printlog_close, XtParseTranslationTable("<Key>Return:ArmAndActivate()\n"));


    str = XmStringCreateLocalized("Cancel");
    printlog_cancel = XtVaCreateManagedWidget(Xdvi_PRINTLOG_CANCEL_NAME,
					      xmPushButtonWidgetClass, box,
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
					      XmNtranslations, window_override_translations,
					      NULL);
    XmStringFree(str);
    XtAddCallback(printlog_cancel, XmNactivateCallback, info->callbacks->cb_cancel, (XtPointer)info);
    XtOverrideTranslations(printlog_close, XtParseTranslationTable("<Key>Return:ArmAndActivate()\n"));

    XtManageChild(box);
    XtManageChild(form);
    XtManageChild(paned);
    /* 	center_window(info->printlog, globals.widgets.top_level); */
    /* 	XtMapWidget(info->printlog); */
    /* this doesn't help */
    /* XtOverrideTranslations(form, XtParseTranslationTable(text_translation_str)); */
    /* XtOverrideTranslations(paned, XtParseTranslationTable(text_translation_str)); */

    printlog_enable_cancelbutton(info);

#endif
    adjust_width(printlog_close, printlog_cancel);
}

void
printlog_append(struct save_or_print_info *info, const char *str, size_t len)
{
    Widget text;
    Widget w = info->printlog;
    
    if (!get_widget_by_name(&text, w, Xdvi_PRINTLOG_TEXT_NAME, True))
	return;
    
#ifndef MOTIF
    {
	XawTextPosition point;
	static XawTextBlock block = {0, 0, NULL, 0};

	block.ptr = (char *) str;
	block.length = len;
	block.format = XawFmt8Bit;
	
	point = XawTextGetInsertionPoint(text);
	while (XawTextReplace(text, point, point, &block) != XawEditDone) {
	    int length;

	    XtVaGetValues(text, XtNlength, &length, NULL);
	    point = length;
	}
	point += len;
	XawTextSetInsertionPoint(text, point);
    }
#else /* MOTIF */
    {
	XmTextPosition point = XmTextGetInsertionPosition(text);
	XmTextInsert(text, point, (char *) str);
	point += len;
	XtVaSetValues(text, XmNcursorPosition, point, NULL);
	XmTextShowPosition(text, point);
    }    
#endif /* MOTIF */
}

void
printlog_append_str(struct save_or_print_info *info, const char *str)
{
    printlog_append(info, str, strlen(str));
}

/* enable cancel button, disable close button */
void
printlog_enable_cancelbutton(struct save_or_print_info *info)
{
    Widget w = info->printlog;
    Widget close, cancel;
    
    if (get_widget_by_name(&cancel, w, Xdvi_PRINTLOG_CANCEL_NAME, True)
	&& get_widget_by_name(&close, w, Xdvi_PRINTLOG_CLOSE_NAME, True)) {
	
	XtSetSensitive(close, False);
	XtSetSensitive(cancel, True);
#if MOTIF
	/* This breaks PRINTLOG_WINDOW_OVERRIDE_TRANSLATIONS??
	 * We don't want it activated on Return anyway ...
	 */
	/* XmProcessTraversal(cancel, XmTRAVERSE_CURRENT); */
#endif
    }
}

/* disable cancel button, enable close button */
void
printlog_enable_closebutton(struct save_or_print_info *info)
{
    Widget w = info->printlog;
    Widget close, cancel;
    
    if (get_widget_by_name(&cancel, w, Xdvi_PRINTLOG_CANCEL_NAME, True)
	&& get_widget_by_name(&close, w, Xdvi_PRINTLOG_CLOSE_NAME, True)) {
#if MOTIF
	char *window_translations = get_string_va(PRINTLOG_WINDOW_TRANSLATIONS, info);
#endif
	XtSetSensitive(close, True);
	XtSetSensitive(cancel, False);
#if MOTIF
	XmProcessTraversal(close, XmTRAVERSE_CURRENT);
	XtOverrideTranslations(close, XtParseTranslationTable(window_translations));
	free(window_translations);
#endif
    }
}

static void
printlog_internal_close(struct save_or_print_info *info)
{
    Widget w = info->printlog;
    Widget text;
    
    XtUnmapWidget(w);
    
    if (get_widget_by_name(&text, w, Xdvi_PRINTLOG_TEXT_NAME, True)) {
#ifndef MOTIF
	XtVaSetValues(text, XtNstring, "", NULL);
#else
	XmTextSetString(text, "");
#endif
    }
}

void
printlog_popdown(struct save_or_print_info *info, Boolean force)
{
    Widget w = info->printlog;
    Widget keep;
    
    if (force) { /* user clicked on `Close' */
	printlog_internal_close(info);
    }
    else if (get_widget_by_name(&keep, w, Xdvi_PRINTLOG_KEEP_NAME, True)) { /* timer elapsed */
#ifndef MOTIF
	Boolean state;
	XtVaGetValues(keep, XtNstate, &state, NULL);
	if (state) {
	    printlog_internal_close(info);
	}
#else
	if (XmToggleButtonGadgetGetState(keep)) {
	    printlog_internal_close(info);
	}
#endif
    }    
}


static void
printlog_act_close(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    void *ptr;
    struct save_or_print_info *info;
    
    UNUSED(w);
    UNUSED(event);

    if (*num_params < 1) {
	XDVI_WARNING((stderr, "Wrong argument number (%d) in callback!", *num_params));
	return;
    }
    sscanf(*params, "%p", &ptr);
    info = (struct save_or_print_info *)ptr;

    ASSERT(info->callbacks != NULL && info->callbacks->cb_close != NULL, "Callback not initialized");
    info->callbacks->cb_close(w, info, NULL);
}

static void
printlog_act_keep(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    void *ptr;
    struct save_or_print_info *info;
    Widget keep;
    
    UNUSED(w);
    UNUSED(event);

    if (*num_params < 1) {
	XDVI_WARNING((stderr, "Wrong argument number (%d) in callback!", *num_params));
	return;
    }
    sscanf(*params, "%p", &ptr);
    info = (struct save_or_print_info *)ptr;

    ASSERT(info->callbacks != NULL && info->callbacks->cb_keep != NULL, "Callback not initialized");
    info->callbacks->cb_keep(w, info, NULL);

    if (get_widget_by_name(&keep, info->printlog, Xdvi_PRINTLOG_KEEP_NAME, True)) {
#ifdef MOTIF
	XmToggleButtonGadgetSetState(keep, False, False);
#else
	XtVaSetValues(keep, XtNstate, False, NULL);
#endif
    }
}


static void
printlog_act_unkeep(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    void *ptr;
    struct save_or_print_info *info;
    Widget keep;
    
    UNUSED(w);
    UNUSED(event);

    if (*num_params < 1) {
	XDVI_WARNING((stderr, "Wrong argument number (%d) in callback!", *num_params));
	return;
    }
    sscanf(*params, "%p", &ptr);
    info = (struct save_or_print_info *)ptr;

    if (get_widget_by_name(&keep, info->printlog, Xdvi_PRINTLOG_KEEP_NAME, True)) {
#ifdef MOTIF
	XmToggleButtonGadgetSetState(keep, True, False);
#else
	XtVaSetValues(keep, XtNstate, True, NULL);
#endif
    }
}


static void
printlog_act_cancel(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    void *ptr;
    struct save_or_print_info *info;
    
    UNUSED(w);
    UNUSED(event);

    if (*num_params < 1) {
	XDVI_WARNING((stderr, "Wrong argument number (%d) in callback!", *num_params));
	return;
    }
    sscanf(*params, "%p", &ptr);
    info = (struct save_or_print_info *)ptr;

    ASSERT(info->callbacks != NULL && info->callbacks->cb_cancel != NULL, "Callback not initialized");
    info->callbacks->cb_cancel(w, info, NULL);
}


static void
printlog_act_cancel_or_destroy(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    void *ptr;
    
    UNUSED(w);
    UNUSED(event);

    if (*num_params < 1) {
	XDVI_WARNING((stderr, "Wrong argument number (%d) in callback!", *num_params));
	return;
    }
    sscanf(*params, "%p", &ptr);

    cb_printlog_act_cancel_or_destroy(w, (XtPointer)ptr, NULL);
    
    /*      ASSERT(info->callbacks != NULL && info->callbacks->cb_cancel != NULL, "Callback not initialized"); */
    /*      info->callbacks->cb_cancel(w, info, NULL); */
}
