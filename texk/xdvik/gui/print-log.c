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

#include <string.h>
#include "xdvi-config.h"
#include "xdvi.h"

#include "xdvi-debug.h"
#include "print-dialog.h"
#include "print-log.h" /* for adjust_vertically() */
#include "util.h"
#include "x_util.h"
#include "xlwradio.h"

#include <X11/Xatom.h>
#include <X11/StringDefs.h>

/* FIXME: change #if XAW to #if !defined(MOTIF) to make it consistent with other files */

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

static Boolean printlog_active = False; /* if the print log is active */
static Widget printlog_shell = NULL;

/* global callbacks - FIXME: avoid this, by passing the callback as argument to the action? */
static void (*g_close_callback)(Widget w, XtPointer client_data, XtPointer call_data);
static void (*g_cancel_callback)(Widget w, XtPointer client_data, XtPointer call_data);
static void (*g_destroy_callback)(Widget w, XtPointer client_data, XtPointer call_data);

static void printlog_act_close(Widget, XEvent *, String *, Cardinal *);
static void printlog_act_keep(Widget, XEvent *, String *, Cardinal *);
static void printlog_act_unkeep(Widget, XEvent *, String *, Cardinal *);
static void printlog_act_cancel(Widget, XEvent *, String *, Cardinal *);
static void printlog_act_destroy(Widget, XEvent *, String *, Cardinal *);

#ifdef MOTIF
static void xm_printlog_act_destroy(Widget w, XtPointer a, XtPointer b)
{
    UNUSED(w);
    UNUSED(a);
    UNUSED(b);
    ASSERT(g_destroy_callback != NULL, "Callback not initialized");
    (g_destroy_callback)(NULL, NULL, NULL);
}
#endif

static Atom WM_DELETE_WINDOW;

static XtActionsRec printlog_actions[] = {
    {"printlogIntClose",	printlog_act_close},
    {"printlogIntKeep",		printlog_act_keep},
    {"printlogIntUnkeep",	printlog_act_unkeep},
    {"printlogIntCancel",	printlog_act_cancel},
    {"WM_destroy",		printlog_act_destroy },
};

static char *text_translations = \
	"<Key>Return:printlogIntClose()\n"
	"^<Key>c:printlogIntCancel()\n"
	"^<Key>s:printlogIntKeep()\n"
	"^<Key>q:printlogIntUnkeep()";

static Widget printlog_text;
static Widget printlog_close;
static Widget printlog_unkeep;
static Widget printlog_cancel;


#ifdef MOTIF
static XmTextPosition printlog_length;
#else
static XawTextPosition printlog_length;
#endif

Boolean
printlog_raise_active(void)
{
    if (printlog_active) {
	XRaiseWindow(DISP, XtWindow(printlog_shell));
	return True;
    }
    return False;
}

void
printlog_popup(void)
{
    printlog_length = 0;
    center_window(printlog_shell, globals.widgets.top_level);
    XtMapWidget(printlog_shell);
/*     XtPopup(printlog_shell, XtGrabNone); */
    printlog_active = True;
#ifndef MOTIF
    XSetWMProtocols(XtDisplay(printlog_shell), XtWindow(printlog_shell), &WM_DELETE_WINDOW, 1);
#else
    XmProcessTraversal(printlog_cancel, XmTRAVERSE_CURRENT);
    XtOverrideTranslations(printlog_text, XtParseTranslationTable(text_translations));
#endif
}

void
printlog_reset(void)
{
    printlog_length = 0;
}



void
printlog_create(const char *title,
		const char *close_label,
		void (*close_callback)(Widget w, XtPointer client_data, XtPointer call_data),
		void (*cancel_callback)(Widget w, XtPointer client_data, XtPointer call_data),
		void (*destroy_callback)(Widget w, XtPointer client_data, XtPointer call_data),
		void (*unkeep_callback)(Widget w, XtPointer client_data, XtPointer call_data))
{
    /*
      FIXME BETA: The printlog presents too much detail and is confusing for users.
      Re-implement it using an `printing page n of m' message,
      i.e. according to the spec on:
      http://xdvi.sourceforge.net/gui.html#file-print
      Possible save the detailed log to a different window that can be viewed
      via `File -> logs', as described on:
      http://xdvi.sourceforge.net/gui.html#file-logs
     */
    XtAddActions(printlog_actions, XtNumber(printlog_actions));
    g_destroy_callback = destroy_callback;
    
#ifndef MOTIF
    /* Create popup window */
    if (printlog_shell == NULL) {
	Widget form, box, paned;
/*  	int ddist; */
/*  	Dimension w0, w1, w2, w3, b; */
	
	XtTranslations wm_translations = XtParseTranslationTable("<Message>WM_PROTOCOLS: WM_destroy()");
	printlog_shell = XtVaCreatePopupShell("printlog",
					      transientShellWidgetClass, globals.widgets.top_level,
					      XtNtitle, title,
					      XtNtransientFor, globals.widgets.top_level,
					      XtNmappedWhenManaged, False,
					      XtNtranslations, wm_translations,
					      XtNtransientFor, globals.widgets.top_level,
					      NULL);

	WM_DELETE_WINDOW = XInternAtom(XtDisplay(printlog_shell), "WM_DELETE_WINDOW", False);
	
	paned = XtVaCreateManagedWidget("paned", panedWidgetClass, printlog_shell, NULL);
	
	form = XtVaCreateManagedWidget("form", formWidgetClass,
				     paned,
				     XtNdefaultDistance, 6,
				     NULL);
	printlog_text = XtVaCreateManagedWidget("text",
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
	XtOverrideTranslations(printlog_text, XtParseTranslationTable(text_translations));

	printlog_unkeep = XtVaCreateManagedWidget("keep",
#ifdef XAW
						  radioWidgetClass,
#else
						  toggleWidgetClass,
#endif
						  form,
						  XtNlabel, close_label,
						  XtNborderWidth, 0,
						  XtNisRadio, False,
						  XtNhighlightThickness, 1,
						  XtNfromVert, printlog_text,
						  XtNleft, XawChainLeft,
						  XtNright, XawChainLeft,
						  XtNtop, XawChainBottom,
						  XtNbottom, XawChainBottom,
						  NULL);
	XtVaSetValues(printlog_unkeep,
		      XtNstate, resource.dvips_hang > 0 && resource.dvips_fail_hang > 0,
		      NULL);
	XtAddCallback(printlog_unkeep, XtNcallback, unkeep_callback, NULL);
	
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
	
	printlog_close = XtVaCreateManagedWidget("close",
						 commandWidgetClass, box,
						 XtNlabel, "Close",
						 XtNsensitive, False,
						 XtNleft, XawChainLeft,
						 XtNright, XawChainLeft,
						 XtNtop, XawChainBottom,
						 XtNbottom, XawChainBottom,
						 NULL);
	XtAddCallback(printlog_close, XtNcallback, close_callback, NULL);
	g_close_callback = close_callback;
	
	printlog_cancel = XtVaCreateManagedWidget("cancel",
						  commandWidgetClass, box,
						  XtNlabel, "Cancel",
						  XtNfromHoriz, printlog_unkeep,
						  XtNleft, XawChainRight,
						  XtNright, XawChainRight,
						  XtNtop, XawChainBottom,
						  XtNbottom, XawChainBottom,
						  NULL);
	XtAddCallback(printlog_cancel, XtNcallback, cancel_callback, NULL);
	g_cancel_callback = cancel_callback;
	XtManageChild(printlog_shell);
    }
    else {
	XtSetSensitive(printlog_close, False);
	XtSetSensitive(printlog_cancel, True);
    }
    
#else /* MOTIF */

    /* Create popup window */
    if (printlog_shell == NULL) {
	Widget form, pane, box;
	XmString str;
	Arg args[10];
	int n;
	
	printlog_shell = XtVaCreatePopupShell("printlog",
					      xmDialogShellWidgetClass, globals.widgets.top_level,
					      XmNdeleteResponse, XmDO_NOTHING, /* we'll take care of that ourselves */
					      XmNtitle, title,
					      XtNmappedWhenManaged, False,
					      NULL);

	WM_DELETE_WINDOW = XmInternAtom(XtDisplay(printlog_shell), "WM_DELETE_WINDOW", False);
	XmAddWMProtocolCallback(printlog_shell, WM_DELETE_WINDOW, xm_printlog_act_destroy, NULL);

	pane = XtVaCreateWidget("printlog_pane", xmPanedWindowWidgetClass, printlog_shell,
				/* make sashes invisible */
				XmNsashWidth, 1,
				XmNsashHeight, 1,
				NULL);
	
	form = XtVaCreateWidget("form", xmFormWidgetClass, pane,
				XmNhorizontalSpacing, DDIST,
				XmNverticalSpacing, DDIST,
				XmNautoUnmanage, False,
				NULL);
	/* force vertical scrollbars. Under Motif 2.x (2.1.0 and 2.2.2 at least),
	   XmNeditMode must be set early in order to have an effect. */
	n = 0;
	XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT);	n++;
	XtSetArg(args[n], XmNeditable, False);			n++;
	XtSetArg(args[n], XmNrows, 24);				n++;
	XtSetArg(args[n], XmNcolumns, 80);			n++;
	
	printlog_text = XmCreateScrolledText(form, "text", args, n);

	XtVaSetValues(XtParent(printlog_text),
		      XmNtopAttachment, XmATTACH_FORM,
		      XmNleftAttachment, XmATTACH_FORM,
		      XmNrightAttachment, XmATTACH_FORM,
		      NULL);

	XtManageChild(printlog_text);

	str = XmStringCreateLocalized((char *)close_label);
	printlog_unkeep = XtVaCreateManagedWidget("keep",
						xmToggleButtonGadgetClass, form,
						XmNlabelString, str,
						XmNnavigationType, XmTAB_GROUP,
						XmNtopAttachment, XmATTACH_WIDGET,
						XmNtopWidget, XtParent(printlog_text),
						XmNleftAttachment, XmATTACH_FORM,
						NULL);
	XmStringFree(str);
	XtAddCallback(printlog_unkeep, XmNvalueChangedCallback, unkeep_callback, NULL);
	XmToggleButtonGadgetSetState(printlog_unkeep, resource.dvips_hang > 0 && resource.dvips_fail_hang > 0, False);

	/* box for Close/Cancel button */
	box = XtVaCreateManagedWidget("box", xmFormWidgetClass,
				      pane,
				      NULL);
	
	str = XmStringCreateLocalized("Close");
	printlog_close = XtVaCreateManagedWidget("close",
						 xmPushButtonWidgetClass, box,
						 XmNlabelString, str,
						 XmNshowAsDefault, True,
						 XmNsensitive, False,
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
						 NULL);
	XmStringFree(str);
	XtAddCallback(printlog_close, XmNactivateCallback, close_callback, NULL);
	g_close_callback = close_callback;

	str = XmStringCreateLocalized("Cancel");
	printlog_cancel = XtVaCreateManagedWidget("cancel",
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
						  NULL);
	XmStringFree(str);
	XtAddCallback(printlog_cancel, XmNactivateCallback, cancel_callback, NULL);
	g_cancel_callback = cancel_callback;

	XtManageChild(box);
	XtManageChild(form);
	XtManageChild(pane);
/* 	center_window(printlog_shell, globals.widgets.top_level); */
/* 	XtMapWidget(printlog_shell); */
	/* this doesn't help */
	/* XtOverrideTranslations(form, XtParseTranslationTable(text_translations)); */
	/* XtOverrideTranslations(pane, XtParseTranslationTable(text_translations)); */
    }
    else {
	XtSetSensitive(printlog_close, False);
	XtSetSensitive(printlog_cancel, True);
    }
#endif
    adjust_width(printlog_close, printlog_cancel);
}

void
printlog_append(const char *str, size_t len)
{
#ifndef MOTIF
    
    static XawTextBlock block = {0, 0, NULL, 0};

    block.ptr = (char *) str;
    block.length = len;
    block.format = XawFmt8Bit;
    while (XawTextReplace(printlog_text, printlog_length, printlog_length, &block)
	   != XawEditDone) {
	int length;

	XtVaGetValues(printlog_text, XtNlength, &length, NULL);
	printlog_length = length;
    }
    printlog_length += len;
    XawTextSetInsertionPoint(printlog_text, printlog_length);

#else /* MOTIF */
    
    XmTextInsert(printlog_text, printlog_length, (char *) str);
    printlog_length += len;
    XtVaSetValues(printlog_text, XmNcursorPosition, printlog_length, NULL);
    XmTextShowPosition(printlog_text, printlog_length);
    
#endif /* MOTIF */
}

void
printlog_append_str(const char *str)
{
    printlog_append(str, strlen(str));
}

/* disable cancel button, enable close button */
void
printlog_enable_closebutton(void)
{
    XtSetSensitive(printlog_close, True);
    XtSetSensitive(printlog_cancel, False);
#if MOTIF
    XmProcessTraversal(printlog_close, XmTRAVERSE_CURRENT);
#endif
}

static void
printlog_internal_close(void)
{
    XtUnmapWidget(printlog_shell);
    printlog_active = False;
#ifndef MOTIF
    XtVaSetValues(printlog_text, XtNstring, "", NULL);
#else
    XmTextSetString(printlog_text, "");
#endif
}

void
printlog_popdown(Boolean force)
{
    if (force) { /* user clicked on `Close' */
	printlog_internal_close();
    }
    else { /* timer elapsed */
#ifndef MOTIF
	Boolean state;
	XtVaGetValues(printlog_unkeep, XtNstate, &state, NULL);
	if (state) {
	    printlog_internal_close();
	}
#else
	if (XmToggleButtonGadgetGetState(printlog_unkeep)) {
	    printlog_internal_close();
	}
#endif
    }    
}


static void
printlog_act_close(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(event);
    UNUSED(params);
    UNUSED(num_params);

    ASSERT(g_close_callback != NULL, "Callback not initialized");
    (g_close_callback)(NULL, NULL, NULL);
}

static void
printlog_act_keep(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(event);
    UNUSED(params);
    UNUSED(num_params);

#ifdef MOTIF
    XmToggleButtonGadgetSetState(printlog_unkeep, False, False);
#else
    XtVaSetValues(printlog_unkeep, XtNstate, False, NULL);
#endif
}



static void
printlog_act_unkeep(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(event);
    UNUSED(params);
    UNUSED(num_params);
#ifdef MOTIF
    XmToggleButtonGadgetSetState(printlog_unkeep, True, False);
#else
    XtVaSetValues(printlog_unkeep, XtNstate, True, NULL);
#endif
}

static void
printlog_act_cancel(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(event);
    UNUSED(params);
    UNUSED(num_params);

    ASSERT(g_cancel_callback != NULL, "Callback not initialized");
    (g_cancel_callback)(NULL, NULL, NULL);
}

static void
printlog_act_destroy(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(event);
    UNUSED(params);
    UNUSED(num_params);

    ASSERT((g_destroy_callback) != NULL, "Callback not initialized");
    (g_destroy_callback)(NULL, NULL, NULL);
}
