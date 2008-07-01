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
 * `Topics' window framework used by help and preferences windows.
 * Contains a list of topics on the left-hand side, and corresponding
 * contents on the right-hand side.
 */

#include "xdvi-config.h"
#include "xdvi.h"

#if MOTIF
# include <Xm/Xm.h>
# include <Xm/Form.h>
# include <Xm/DialogS.h>
# include <Xm/PushB.h>
# include <Xm/Frame.h>
# include <Xm/PanedW.h>
# include <Xm/LabelG.h>
# include <Xm/Label.h>
# include <Xm/Protocols.h>
# include <Xm/List.h>
# include <Xm/AtomMgr.h>
#else /* MOTIF */
# include <X11/Xaw/Paned.h>
# include <X11/Xaw/Box.h>
# include <X11/Xaw/Form.h>
# include <X11/Xaw/Cardinals.h>
# include <X11/Xaw/Command.h>
# include <X11/Xaw/List.h>
#endif /* MOTIF */

#include "version.h"
#include "topic-window.h"
#include "string-utils.h"
#include "util.h"
#include "x_util.h"

#if MOTIF

/* special formatting for headings in right window, if desired */
const char *const TAG_NORMAL = "NORMAL";
const char *const TAG_TOPIC_HEADING = "TOPIC_HEADING";
const char *const TAG_TOPIC_LABEL = "TOPIC_LABEL";

# define SHELL_WIDGET_CLASS	xmDialogShellWidgetClass
# define PANED_WIDGET_CLASS	xmPanedWindowWidgetClass
# define FORM_WIDGET_CLASS	xmFormWidgetClass
  
# define FORM_ARGS		XmNhorizontalSpacing, 10,	\
				NULL
# define HELP_SHELL_ARGS	XmNdeleteResponse, XmDO_NOTHING,	\
  				XtNmappedWhenManaged, False,		\
				NULL
# define HELP_PANED_ARGS	XmNsashWidth, 1,	\
				XmNuserData, info,	\
				XmNsashHeight, 1,	\
				NULL
# define LEFT_FORM_ARGS		XmNtopAttachment, XmATTACH_FORM,	\
  				XmNleftAttachment, XmATTACH_FORM,	\
  				XmNbottomAttachment, XmATTACH_FORM,	\
				XmNtopOffset, 9,		\
				XmNbottomOffset, 10,		\
				NULL
# define RIGHT_FORM_ARGS	XmNtopAttachment, XmATTACH_FORM,	\
  				XmNleftAttachment, XmATTACH_WIDGET,		\
  				XmNleftWidget, left_form,			\
  				XmNrightAttachment, XmATTACH_FORM,		\
  				XmNbottomAttachment, XmATTACH_FORM,		\
				XmNbottomOffset, 10,		\
				NULL
# define ACTION_AREA_ARGS	NULL

#else /* MOTIF */

# define SHELL_WIDGET_CLASS	transientShellWidgetClass
# define PANED_WIDGET_CLASS	panedWidgetClass
# define FORM_WIDGET_CLASS	formWidgetClass
  
# define FORM_ARGS		XtNdefaultDistance, 14,			\
				NULL
# define HELP_SHELL_ARGS	XtNx, 60,				\
				XtNy, 80,				\
  				XtNtransientFor, globals.widgets.top_level,		\
  				XtNtranslations, xlats,			\
  				XtNtransientFor, parent,		\
  				XtNallowShellResize, False,		\
				NULL
# define HELP_PANED_ARGS	NULL
# define LEFT_FORM_ARGS		XtNtop, XtChainTop,		\
  				XtNbottom, XtChainBottom,	\
  				XtNleft, XtChainLeft,		\
  				XtNright, XtChainLeft,		\
  				XtNborderWidth, 0,		\
  				NULL
# define RIGHT_FORM_ARGS	XtNfromHoriz, left_form,	\
  				XtNtop, XtChainTop,		\
  				XtNbottom, XtChainBottom,	\
  				XtNleft, XtChainLeft,		\
  				XtNright, XtChainRight,		\
  				XtNborderWidth, 0,		\
  				NULL
# define ACTION_AREA_ARGS	XtNdefaultDistance, 6,	\
				XtNshowGrip, False,	\
				XtNskipAdjust, True,	\
				NULL
#endif /* MOTIF */


static void
ok_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    struct topic_info *info = (struct topic_info *)client_data;

    UNUSED(w);
    UNUSED(call_data);
    
    ASSERT(info != NULL, "No info passed to callback!");

    if (info->ok_callback != NULL)
	info->ok_callback(info);
    XtPopdown(info->shell);
}

static void
cancel_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    struct topic_info *info = (struct topic_info *)client_data;

    UNUSED(w);
    UNUSED(call_data);

    ASSERT(info != NULL, "No info passed to callback!");

    if (info->cancel_callback != NULL)
	info->cancel_callback(info);
    XtPopdown(info->shell);
}

#if MOTIF
/*
 * We are overriding this since otherwise the Text widget would attempt to
 * first move the invisible cursor instead of the scroll bars.
 */
/* FIXME: This gives a warning:
   Warning: Actions not found: scroll-one-line-down, scroll-one-line-up
*/
/* "<Key>osfUp: scroll-one-line-down()\n" */
/* "<Key>osfDown: scroll-one-line-up()\n" */
#endif

#if MOTIF
static XmString
#else
static char *
#endif
create_label_string(const char *title, const char *subtitle)
{
#if MOTIF
    XmString label;
    if (subtitle == NULL) { /* simple bold label */
	label = XmStringCreate((char *)title, (char *)TAG_TOPIC_LABEL);
    }
    else { /* two-part title with subheading */
	XmString s1, s2, s3, s4;
	s1 = XmStringCreate((char *)title, (char *)TAG_TOPIC_HEADING);
	s2 = XmStringCreate((char *)"   ", (char *)TAG_NORMAL);
	s3 = XmStringCreate((char *)subtitle, (char *)TAG_NORMAL);
	s4 = XmStringConcat(s1, s2);
	label = XmStringConcat(s4, s3);
	XmStringFree(s1);
	XmStringFree(s2);
	XmStringFree(s3);
	XmStringFree(s4);
    }
#else
    char *label = xstrdup(title);
    if (subtitle != NULL && strlen(subtitle) > 0) {
	label = xstrcat(label, "   -   ");
	label = xstrcat(label, subtitle);
    }
#endif
    return label;
}

static Widget
create_label_widget(Widget parent, const char *name, struct topic_info *info)
{
    Widget label;
    
#if MOTIF

    XmString label_string = create_label_string(info->items[0].topic, info->items[0].title);
    label = XtVaCreateWidget(name, xmLabelWidgetClass, parent,
			     XmNtopAttachment, XmATTACH_FORM,
			     XmNtopOffset, 14,
			     XmNleftAttachment, XmATTACH_FORM,
			     XmNrightAttachment, XmATTACH_FORM,
			     XmNlabelString, label_string,
			     XmNalignment, XmALIGNMENT_BEGINNING,
			     NULL);
    XmStringFree(label_string);
    
#else /* MOTIF */
    
    char *label_string = create_label_string(info->items[0].topic, info->items[0].title);
    label = XtVaCreateWidget(name, labelWidgetClass, parent,
			     XtNlabel, label_string,
			     XtNborderWidth, 0,
			     /* XtNinternalHeight, 2, */
			     XtNjustify, XtJustifyLeft,
			     NULL);
    free(label_string);
    
#endif /* MOTIF */
    
    return label;
}

void
select_topic(struct topic_info *info, size_t idx)
{
#if MOTIF
    XmString label;
#else
    char *label;
#endif

    /* change the heading in right window */
    label = create_label_string(info->items[idx].topic, info->items[idx].title);
    XtVaSetValues(info->topic_label,
#if MOTIF
		  XmNlabelString,
#else
		  XtNlabel,
#endif
		  label, NULL);

#if MOTIF
    XmStringFree(label);
    XmListSelectPos(info->topics_list, idx + 1, False);
#else
    free(label);
    XawListHighlight(info->topics_list, idx);
#endif
    
    if (info->curr_selected != 0) {
	XtUnmanageChild(info->curr_selected);
    }
    else {
	/* Note: doesn't matter if not managed yet */
	/* 	XtUnmanageChild(info->items[0].widget); */
    }

    XtManageChild(info->items[idx].widget);
    info->curr_selected = info->items[idx].widget;

#if MOTIF
    XmUpdateDisplay(info->shell);
#endif
}

static void
select_topic_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    struct topic_info *info = (struct topic_info *)client_data;
    size_t idx;

#if MOTIF
    idx = ((XmListCallbackStruct *)call_data)->item_position - 1;
#else
    idx = ((XawListReturnStruct *)call_data)->list_index;
#endif
    UNUSED(w);

    /* ASSERT(idx < info->items_size, "list position exceeds items_size!"); */

    select_topic(info, idx);
}

static Widget
create_button(Widget parent, const char *name, Boolean show_as_default, Boolean left_position)
{
    Widget button;
#if MOTIF
    XmString s1 = XmStringCreateLocalized((char *)name);
    button = XtVaCreateWidget(name, xmPushButtonWidgetClass, parent,
			      XmNlabelString, s1,
			      XmNdefaultButtonShadowThickness, 1,
			      XmNtopAttachment, XmATTACH_FORM,
			      XmNbottomAttachment, XmATTACH_FORM,
			      XmNmarginWidth, 6,
			      XmNmarginHeight, 4,
			      XmNtopOffset, 10,
			      XmNbottomOffset, 10,
			      NULL);
    if (left_position)
	XtVaSetValues(button,
		      XmNleftAttachment, XmATTACH_FORM,
		      XmNleftOffset, 10,
		      NULL);
    else
	XtVaSetValues(button,
		      XmNrightAttachment, XmATTACH_FORM,
		      XmNrightOffset, 10,
		      NULL);
    if (show_as_default)
	XtVaSetValues(button, XmNshowAsDefault, True, NULL);

    XtManageChild(button);
    XmStringFree(s1);
#else /* MOTIF */
    UNUSED(show_as_default);
    button = XtVaCreateManagedWidget(name, commandWidgetClass, parent,
				     XtNtop, XtChainTop,
				     XtNbottom, XtChainBottom,
				     NULL);
    if (left_position)
	XtVaSetValues(button,
		      XtNleft, XtChainLeft,
		      XtNright, XtChainLeft,
		      NULL);
    else
	XtVaSetValues(button,
		      XtNleft, XtChainRight,
		      XtNright, XtChainRight,
		      NULL);
#endif /* MOTIF */
    return button;
}

static void
close_topic_window(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    Widget button;
    void *ptr;
    struct topic_info *info;
    
    UNUSED(w);
    UNUSED(event);
    
    if (*num_params < 1) {
	XDVI_WARNING((stderr, "Wrong argument number (%d) in callback!", *num_params));
	return;
    }

    sscanf(*params, "%p", &ptr);
    info = (struct topic_info *)ptr;

    /* get a callback to close the button, in decreasing order of preference */
    if (get_widget_by_name(&button, info->shell, "Cancel", False)
	|| get_widget_by_name(&button, info->shell, "Close", False)
	|| get_widget_by_name(&button, info->shell, "OK", False)) {
	XtCallCallbacks(button,
#if MOTIF
			XmNactivateCallback,
#else
			XtNcallback,
#endif
			info);
    }
    else {
	XDVI_WARNING((stderr, "No button found for widget %p!\n", (void *)info->shell));
    }
}

static XtActionsRec popdown_actions[] = {
#if !MOTIF
    { "WM_popdown", close_topic_window },
#endif
    { "close-topic-window", close_topic_window },
};

static Widget
create_list_widget(Widget parent, const char *w_name, struct topic_info *info)
{
    Widget list;
    size_t tnum;
#if MOTIF
    XmString *topics = NULL;
#else
    static char **topics = NULL;
#endif

    for (tnum = 0; info->items[tnum].topic != NULL; tnum++) {
	/* 1 more for terminating NULL needed for Xaw */
	topics = xrealloc(topics, (tnum + 2) * sizeof *topics);
#if MOTIF
	topics[tnum] = XmStringCreateLocalized(info->items[tnum].topic);
#else
	topics[tnum] = xstrdup(info->items[tnum].topic);
#endif
    }

#ifndef MOTIF
    /* list needs to be terminated for Xaw */
    topics[tnum] = NULL;
#endif
    
#if MOTIF
    {
	size_t i;
	Arg args[20];
	int n = 0;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNlistSpacing, 4); n++;
	XtSetArg(args[n], XmNlistMarginHeight, 4); n++;
	XtSetArg(args[n], XmNlistMarginWidth, 4); n++;
	XtSetArg(args[n], XmNhighlightThickness, 0); n++;
	XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	
	list = XmCreateScrolledList(parent, (char *)w_name, args, n);

#if defined(__GNUC__) && DEVEL_MODE
#warning TODO: make up/down scroll the list, and PgUp/PgDown scroll help text
#warning TODO: add wheel mouse bindings
	/*
	 * also fix bug with arrow keys first moving invisible cursor, then scrollbars
	 */
#endif
	
	XmListDeleteAllItems(list);
	XmListAddItems(list, topics, tnum, 0);
	
	for (i = 0; i < tnum; ++i) {
	    XmStringFree(topics[i]);
	}
	free(topics);
    }
    XtAddCallback(list, XmNbrowseSelectionCallback, select_topic_cb, info);
    XmListSelectPos(list, 1, False); /* default position */
#else /* MOTIF */
    list = XtVaCreateWidget(w_name, listWidgetClass, parent,
			    XtNlist, topics,
			    XtNdefaultColumns, 1,
			    XtNforceColumns, True,
			    XtNverticalList, True,
			    XtNrowSpacing, 4,
			    XtNheight, 429,
			    NULL);
    XtAddCallback(list, XtNcallback, select_topic_cb, info);
    XawListHighlight(list, 0); /* default position */
#endif /* MOTIF */
    XtManageChild(list);
    return list;
}


Widget
create_topic_window(Widget parent,
		    const char *window_title,
		    const char *widget_name,
		    struct topic_info *info,
		    void (*init_items_func)(struct topic_info *info),
		    /* OK button/callbacks */
		    const char *ok_label,
		    /* Cancel button/callbacks (can be NULL) */
		    const char *cancel_label)
{
    Widget topic_window, topics_list, topic_label;
    Widget pane, form, left_form, right_container_form, right_form;
    Widget action_area, ok_button, cancel_button;
    XtTranslations xlats;

    XtAppContext app = NULL;
    Atom WM_DELETE_WINDOW;

    char *translation_str;
    size_t i;
    
#if !MOTIF
    translation_str = get_string_va("<Message>WM_PROTOCOLS: WM_popdown(%p)", info);
    xlats = XtParseTranslationTable(translation_str);
    free(translation_str);
#endif
	
    topic_window = XtVaCreatePopupShell(widget_name, SHELL_WIDGET_CLASS,
					parent,
					XtNtitle, window_title,
					HELP_SHELL_ARGS);

#if MOTIF
    /* make the window manager destroy action pop down the window */
    WM_DELETE_WINDOW = XmInternAtom(XtDisplay(topic_window), "WM_DELETE_WINDOW", False);
    XmAddWMProtocolCallback(topic_window, WM_DELETE_WINDOW, cancel_cb, info);
#else
    WM_DELETE_WINDOW = XInternAtom(XtDisplay(topic_window), "WM_DELETE_WINDOW", False);
#endif
    app = XtWidgetToApplicationContext(topic_window);
    XtAppAddActions(app, popdown_actions, XtNumber(popdown_actions));

    pane = XtVaCreateWidget("topic_pane", PANED_WIDGET_CLASS, topic_window, HELP_PANED_ARGS);

    form = XtVaCreateWidget("form", FORM_WIDGET_CLASS, pane, FORM_ARGS);

    /* left pane for topics selection */
    left_form = XtVaCreateWidget("left_form", FORM_WIDGET_CLASS, form, LEFT_FORM_ARGS);

    /* right form for topics display */
    right_container_form = XtVaCreateWidget("right_container_form", FORM_WIDGET_CLASS, form, RIGHT_FORM_ARGS);

    /*
      Initialize the topic label and title string with dummy values - the real values
      are only known inside init_items_func(). For Xaw, the size of the widget will be
      adjusted below (for Motif it's always the full width of the right form, which is
      what we want).
    */
    /* FIXME: Motif label is still chopped off if it's longer than right form -
       work around by putting longest possible lable here ... */
    /* Also, not all children will be sized correctly, e.g. the colors - work around
       by specifying a rather large dummy text ... */
    info->items[0].topic = "text text text";
    info->items[0].title = "text text text text text text text text text text text text text text text text text";
    topic_label = create_label_widget(right_container_form, "topic_label", info);
    info->topic_label = topic_label;

    right_form = XtVaCreateWidget("right_form", FORM_WIDGET_CLASS,
				  right_container_form,
#if MOTIF
				  XmNtopAttachment, XmATTACH_WIDGET,
				  XmNtopWidget, topic_label,
				  XmNtopOffset, 10,
				  XmNleftAttachment, XmATTACH_FORM,
				  XmNrightAttachment, XmATTACH_FORM,
				  XmNbottomAttachment, XmATTACH_FORM,
#else
				  XtNborderWidth, 0,
				  XtNfromVert, topic_label,
				  XtNvertDistance, 6,
#endif /* MOTIF */
				  NULL);
    info->right_form = right_form;
    
    /*
      Call the init_items_func callback. This will populate the info->items list
      with the appropriate info, and create the forms (info->items[i].widget)
      which are the children of right_form (which is accessed inside init_items_func()
      through info->right_form), and all their subchildren.

      It is important that every child form is already managed inside init_items_func
      so that the total size of the preferences window is set to the maximum size of
      the children. (The children will be unmanaged again below). All these widgets
      are never destroyed, and are managed by demand via the list selection callback
      (select_topic_cb).
    */
    init_items_func(info);

    XtManageChild(topic_label);
    
    topics_list = create_list_widget(left_form, "topics_list", info);
    info->topics_list = topics_list;

    translation_str = get_string_va("#override \n"
				    "<Key>q:close-topic-window(%p)\n"
#ifdef MOTIF
				    "<Key>osfCancel:close-topic-window(%p)\n"
#else
				    "<Key>Escape:close-topic-window(%p)\n"
#endif
				    "<Key>Return:close-topic-window(%p)",
				    info, info, info);
    xlats = XtParseTranslationTable(translation_str);
    free(translation_str);
    translation_str = NULL;

    XtOverrideTranslations(pane, xlats);
    XtOverrideTranslations(topics_list, xlats);

    XtManageChild(left_form);
    
    XtManageChild(right_form);
    XtManageChild(right_container_form);

    /* action area at bottom */
    action_area = XtVaCreateWidget("action_area", FORM_WIDGET_CLASS, pane, ACTION_AREA_ARGS);
	
    ok_button = create_button(action_area, ok_label, True, True);
    XtOverrideTranslations(ok_button, xlats);
    
    if (cancel_label != NULL) {
	cancel_button = create_button(action_area, cancel_label, False, False);
	adjust_width(ok_button, cancel_button);
#if MOTIF
	XtAddCallback(cancel_button, XmNactivateCallback, cancel_cb, info);
#else
	XtAddCallback(cancel_button, XtNcallback, cancel_cb, info);
#endif
	XtOverrideTranslations(cancel_button, xlats);
    }
	
    XtManageChild(action_area);

    XtManageChild(form);

    XtManageChild(pane);
	
    XtRealizeWidget(topic_window);
    
    /* Now unmanage all children of right_form, as described above. */
    for (i = 0; info->items[i].topic != NULL; i++) {
	XtUnmanageChild(info->items[i].widget);
    }
    info->curr_selected = 0;
    
#if MOTIF
    /* enable OK button */
    XmProcessTraversal(ok_button, XmTRAVERSE_CURRENT);
    XtOverrideTranslations(ok_button,
			   XtParseTranslationTable("<Key>Return:ArmAndActivate()\n"
						   "<Key>q:ArmAndActivate()"));
    XtAddCallback(ok_button, XmNactivateCallback, ok_cb, info);
    XmAddWMProtocolCallback(topic_window, WM_DELETE_WINDOW, cancel_cb, info);
    { /* disable resizing of lower part of pane (and button) */
	Dimension h;
	XtVaGetValues(ok_button, XmNheight, &h, NULL);
	XtVaSetValues(action_area,
		      XmNpaneMaximum, h + 20,
		      XmNpaneMinimum, h + 20,
		      NULL);
    }
#else
    XSetWMProtocols(XtDisplay(topic_window), XtWindow(topic_window), &WM_DELETE_WINDOW, 1);
    XtAddCallback(ok_button, XtNcallback, ok_cb, info);
#endif
    return topic_window;
}
