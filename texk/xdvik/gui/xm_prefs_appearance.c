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
 * Panel 1 (Apperance) for xdvik preferences dialog.
 */

#include "xdvi-config.h"
#include "xdvi.h"

#include "x_util.h"
#include "my-snprintf.h"
#include "xm_colorsel.h"
#include "topic-window.h"
#include "message-window.h"
#include "util.h"
#include "events.h"

#include "xm_toolbar.h"
#include "xm_menu.h"
#include "statusline.h"
#include "pagesel.h"

#include "xm_prefsP.h"
#include "xm_prefs_appearance.h"

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
#include <Xm/Text.h>
#include <Xm/TextF.h>

/*
 * Handy defaults
 */
static Arg one_of_many = { XmNindicatorType, XmONE_OF_MANY };
static Arg n_of_many = { XmNindicatorType, XmN_OF_MANY };
static Arg frame_title = { XmNchildType, XmFRAME_TITLE_CHILD };
static Arg two_cols[] = {
    { XmNpacking, XmPACK_TIGHT },
    { XmNnumColumns, 2 },
    { XmNorientation, XmHORIZONTAL }
};
static Arg two_cols_fixed[] = {
    { XmNpacking, XmPACK_COLUMN },
    { XmNnumColumns, 2 }
};

static void
match_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    struct topic_info *info = (struct topic_info *)client_data;
    struct prefs_choice *prefs = (struct prefs_choice *)(info->data);
    Widget inverted, boxed;
    
    UNUSED(w);
    UNUSED(call_data);

    resource.match_highlight_inverted = !resource.match_highlight_inverted;

    /* force a redraw so that a current match is updated if there was one */
    globals.ev.flags |= EV_NEWPAGE;
    
    store_preference(&(prefs->db), "matchInverted", "%s",
		     resource.match_highlight_inverted ? "True" : "False");
    
    if (get_widget_by_name(&inverted, info->shell, Xdvi_MATCH_INVERTED_STR, True)
	&& get_widget_by_name(&boxed, info->shell, Xdvi_MATCH_BOXED_STR, True)) {

	XmToggleButtonGadgetSetState(inverted, resource.match_highlight_inverted, False);
	XmToggleButtonGadgetSetState(boxed, !resource.match_highlight_inverted, False);
    }
}

static void
tooltips_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    struct topic_info *info = (struct topic_info *)client_data;
    struct prefs_choice *prefs = (struct prefs_choice *)(info->data);
    const char *w_name = XtName(w);

    UNUSED(call_data);

    if (strcmp(w_name, Xdvi_TIPS_STATUSLINE) == 0) {
	resource.tooltips_in_statusline = !resource.tooltips_in_statusline;
	store_preference(&(prefs->db), "tooltipsInStatusline", "%s",
			 resource.tooltips_in_statusline ? "True" : "False");
    }
    else if (strcmp(w_name, Xdvi_TIPS_POPUPS) == 0) {
	Widget label1, text, label2;

	resource.show_tooltips = !resource.show_tooltips;

	if (!resource.show_tooltips && resource.tooltips_wait_period >= 0) {
	    if (resource.tooltips_wait_period == 0)
		resource.tooltips_wait_period = -1;
	    else
		resource.tooltips_wait_period = -resource.tooltips_wait_period;
	}
	else if (resource.show_tooltips && resource.tooltips_wait_period < 0) {
	    resource.tooltips_wait_period = -resource.tooltips_wait_period;
	}
	store_preference(&(prefs->db), "showTooltips", "%s", resource.show_tooltips ? "True" : "False");
	
	if (get_widget_by_name(&label1, info->shell, Xdvi_TIPS_DELAY_LABEL1, True)
	    && get_widget_by_name(&text, info->shell, Xdvi_TIPS_DELAY_TEXT, True)
	    && get_widget_by_name(&label2, info->shell, Xdvi_TIPS_DELAY_LABEL2, True)) {

	    XtSetSensitive(label1, resource.show_tooltips);
	    XtSetSensitive(text, resource.show_tooltips);
	    XtSetSensitive(label2, resource.show_tooltips);
	}
    }
    else if (strcmp(w_name, Xdvi_TIPS_DELAY_TEXT) == 0) {
	char *buf = XmTextFieldGetString(w);
	int val = strtol(buf, (char **)NULL, 10);
	XtFree((char *)buf);
	TRACE_GUI((stderr, "tooltips_cb wait period2: %d\n", val));

	resource.tooltips_wait_period = val;
	store_preference(&(prefs->db), "tipShell.waitPeriod", "%d", val);
	
    }
    else {
	XDVI_WARNING((stderr, "unexpected widget name `%s' in tooltips_cb", XtName(w)));
    }
    
}

#if 0
static void
toolbar_buttons_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    struct topic_info *info = (struct topic_info *)client_data;
    struct x_resources *res = (struct x_resources *)(info->data);
    Boolean is_flat = False, is_raised = False;

    Widget flat_b, raised_b;

    UNUSED(call_data);
    
    if (!get_widget_by_name(&flat_b, XtParent(w), TB_BUTTONS_FLAT_STR, True) ||
	!get_widget_by_name(&raised_b, XtParent(w), TB_BUTTONS_RAISED_STR, True))
	return;
    
    if (w == flat_b) {
	is_flat = True;
    }
    else if (w == raised_b) {
	is_raised = True;
    }
    else {
	unexpected_widget_in_callback(w, "toolbar_buttons_cb()");
	return;
    }

    res->toolbar_buttons_raised = is_raised ? True : False;
    
    XmToggleButtonGadgetSetState(raised_b, is_raised, False);
    XmToggleButtonGadgetSetState(flat_b, is_flat, False);
}
#endif /* 0 */

void update_preferences_expert(void)
{
    Widget shell;
    Widget statusline_b, toolbar_b, pagelist_b, scrollbars_b;

    /* it's not an error if the prefs dialog doesn't exist yet */
    if (get_widget_by_name(&shell, globals.widgets.top_level, Xdvi_PREFS_DIALOG_NAME, False)
	&& get_widget_by_name(&statusline_b, shell, Xdvi_GUI_STATUSLINE_STR, True)
	&& get_widget_by_name(&toolbar_b, shell, Xdvi_GUI_TOOLBAR_STR, True)
	&& get_widget_by_name(&pagelist_b, shell, Xdvi_GUI_PAGELIST_STR, True)
	&& get_widget_by_name(&scrollbars_b, shell, Xdvi_GUI_SCROLLBARS_STR, True)) {

	XmToggleButtonGadgetSetState(statusline_b, resource.expert_mode & XPRT_SHOW_STATUSLINE ? True : False, False);
	XmToggleButtonGadgetSetState(toolbar_b, resource.expert_mode & XPRT_SHOW_TOOLBAR ? True : False, False);
	XmToggleButtonGadgetSetState(pagelist_b, resource.expert_mode & XPRT_SHOW_PAGELIST ? True : False, False);
	XmToggleButtonGadgetSetState(scrollbars_b, resource.expert_mode & XPRT_SHOW_SCROLLBARS ? True : False, False);
    }
}

void update_preferences_tooltips(void)
{
    Widget shell;
    Widget statusline_b, popup_b, label1, text, label2;

    if (resource.toolbar_unusable)
	return;
     
    if (get_widget_by_name(&shell, globals.widgets.top_level, Xdvi_PREFS_DIALOG_NAME, True)
	&& get_widget_by_name(&statusline_b, shell, Xdvi_TIPS_STATUSLINE, True)
	&& get_widget_by_name(&popup_b, shell, Xdvi_TIPS_POPUPS, True)
	&& get_widget_by_name(&label1, shell, Xdvi_TIPS_DELAY_LABEL1, True)
	&& get_widget_by_name(&text, shell, Xdvi_TIPS_DELAY_TEXT, True)
	&& get_widget_by_name(&label2, shell, Xdvi_TIPS_DELAY_LABEL2, True)) {

	char buf[LENGTH_OF_INT];

	SNPRINTF(buf, LENGTH_OF_INT, "%d", ABS(resource.tooltips_wait_period));
	XtVaSetValues(text, XmNvalue, buf, NULL);
	 
	XmToggleButtonGadgetSetState(statusline_b, resource.tooltips_in_statusline, False);
	XmToggleButtonGadgetSetState(popup_b, resource.show_tooltips, False);

	XtSetSensitive(label1, resource.show_tooltips);
	XtSetSensitive(text, resource.show_tooltips);
	XtSetSensitive(label2, resource.show_tooltips);
    }
}

void update_preferences_search(void)
{
    Widget shell;
    Widget inverted, boxed;

    if (get_widget_by_name(&shell, globals.widgets.top_level, Xdvi_PREFS_DIALOG_NAME, True)
	&& get_widget_by_name(&inverted, shell, Xdvi_MATCH_INVERTED_STR, True)
	&& get_widget_by_name(&boxed, shell, Xdvi_MATCH_BOXED_STR, True)) {

	XmToggleButtonGadgetSetState(inverted, resource.match_highlight_inverted, False);
	XmToggleButtonGadgetSetState(boxed, !resource.match_highlight_inverted, False);
    }
}

static void
gui_buttons_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    struct topic_info *info = (struct topic_info *)client_data;
    struct prefs_choice *prefs = (struct prefs_choice *)(info->data);
    char *name = XtName(w);
    
    UNUSED(call_data);

    if (strcmp(name, Xdvi_GUI_STATUSLINE_STR) == 0) {
	resource.expert_mode ^= XPRT_SHOW_STATUSLINE;
	toggle_statusline();
	update_expert_mode();
    }
    else if (strcmp(name, Xdvi_GUI_TOOLBAR_STR) == 0) {
	resource.expert_mode ^= XPRT_SHOW_TOOLBAR;
	toggle_toolbar();
	update_expert_mode();
    }
    else if (strcmp(name, Xdvi_GUI_PAGELIST_STR) == 0) {
	resource.expert_mode ^= XPRT_SHOW_PAGELIST;
	toggle_pagelist();
	update_expert_mode();
    }
    else if (strcmp(name, Xdvi_GUI_SCROLLBARS_STR) == 0) {
#if defined(LESSTIF_VERSION)
	static Boolean warned_about_lesstif = False;
#endif
	resource.expert_mode ^= XPRT_SHOW_SCROLLBARS;
	toggle_scrollbars();
	update_expert_mode();
#if defined(LESSTIF_VERSION)
	if (!warned_about_lesstif) {
	    warned_about_lesstif = True;
	    popup_message(globals.widgets.top_level,
			  MSG_INFO,
			  NULL,
			  "This version has been compiled with LessTif; "
			  "toggling the scrollbars won't work with LessTif.");
	}
#endif
    }
    else {
	popup_message(globals.widgets.top_level,
		      MSG_ERR,
		      REPORT_XDVI_BUG_TEMPLATE,
		      "Unexpected label in gui buttons: |%s|!\n", name);
    }
    store_preference(&(prefs->db), "expertMode", "%d", resource.expert_mode);
}


#if 0
static void
toolbar_buttons_init(struct topic_info *info, Widget raised, Widget flat)
{
    Boolean is_flat = False, is_raised = False;
    struct x_resources *res = (struct x_resources *)(info->data);

    if (res->toolbar_buttons_raised)
	is_raised = True;

    XmToggleButtonGadgetSetState(raised, is_raised, False);
    XmToggleButtonGadgetSetState(flat, is_flat, False);
    
    XtAddCallback(flat, XmNvalueChangedCallback, toolbar_buttons_cb, (XtPointer)info);
    XtAddCallback(raised, XmNvalueChangedCallback, toolbar_buttons_cb, (XtPointer)info);
}
#endif /* 0 */


Widget
prefs_appearance(struct topic_info *info)
{
    Widget form;
    Widget gui_frame, gui_label, gui_rowcol,
	gui_statusline, gui_toolbar, gui_pagelist, gui_scrollbars;
    Widget tips_frame, tips_label, tips_form, tips_statusline;
    Widget tips_popups, tips_delay_text, tips_delay_label1, tips_delay_label2;
    Widget match_frame, match_label, match_rowcol, match_inverted, match_boxed;
#if 0
    Widget tb_buttons_frame, tb_buttons_label, tb_buttons_rowcol,
	tb_buttons_flat, tb_buttons_raised;
#endif
    XmString str;
    char buf[LENGTH_OF_INT];
    Arg args[10];
    int n;
    
    form = XmCreateForm(info->right_form, "appearance_form", NULL, 0);
    h_attach_below(form, NULL);

    n = 0;
    XtSetArg(args[n], XmNmarginWidth, 8); n++;
    XtSetArg(args[n], XmNmarginHeight, 4); n++;
    gui_frame = XmCreateFrame(form, "gui_frame", args, n);
    h_attach_below(gui_frame, NULL);
    
    gui_label = XmCreateLabelGadget(gui_frame, "Window Configuration", &frame_title, 1);
    XtManageChild(gui_label);

    gui_rowcol = XmCreateRowColumn(gui_frame, "gui_rowcol", two_cols_fixed, XtNumber(two_cols_fixed));
    XtManageChild(gui_rowcol);

    gui_statusline = XmCreateToggleButtonGadget(gui_rowcol, Xdvi_GUI_STATUSLINE_STR, &n_of_many, 1);
    XmToggleButtonGadgetSetState(gui_statusline, resource.expert_mode & XPRT_SHOW_STATUSLINE ? True : False, False);
    XtManageChild(gui_statusline);

    gui_toolbar = XmCreateToggleButtonGadget(gui_rowcol, Xdvi_GUI_TOOLBAR_STR, &n_of_many, 1);
    XmToggleButtonGadgetSetState(gui_toolbar, resource.expert_mode & XPRT_SHOW_TOOLBAR ? True : False, False);
    XtManageChild(gui_toolbar);
    
    gui_pagelist = XmCreateToggleButtonGadget(gui_rowcol, Xdvi_GUI_PAGELIST_STR, &n_of_many, 1);
    XmToggleButtonGadgetSetState(gui_pagelist, resource.expert_mode & XPRT_SHOW_PAGELIST ? True : False, False);
    XtManageChild(gui_pagelist);
    
    gui_scrollbars = XmCreateToggleButtonGadget(gui_rowcol, Xdvi_GUI_SCROLLBARS_STR, &n_of_many, 1);
    XmToggleButtonGadgetSetState(gui_scrollbars, resource.expert_mode & XPRT_SHOW_SCROLLBARS ? True : False, False);
    XtManageChild(gui_scrollbars);

    XtAddCallback(gui_statusline, XmNvalueChangedCallback, gui_buttons_cb, (XtPointer)info);
    XtAddCallback(gui_toolbar, XmNvalueChangedCallback, gui_buttons_cb, (XtPointer)info);
    XtAddCallback(gui_pagelist, XmNvalueChangedCallback, gui_buttons_cb, (XtPointer)info);
    XtAddCallback(gui_scrollbars, XmNvalueChangedCallback, gui_buttons_cb, (XtPointer)info);

    n = 0;
    XtSetArg(args[n], XmNmarginWidth, 8); n++;
    XtSetArg(args[n], XmNmarginHeight, 4); n++;
    XtSetArg(args[n], XmNtopOffset, 10); n++;
    tips_frame = XmCreateFrame(form, "tips_frame", args, n);
    h_attach_below(tips_frame, gui_frame);
    
    tips_label = XmCreateLabelGadget(tips_frame, "Show Tooltips", &frame_title, 1);
    XtManageChild(tips_label);
    
    tips_form = XmCreateForm(tips_frame, "tips_form", NULL, 0);
    
    str = XmStringCreateLocalized("As Text in Statusline");
    tips_statusline = XtVaCreateManagedWidget(Xdvi_TIPS_STATUSLINE,
					      xmToggleButtonGadgetClass, tips_form,
					      XmNlabelString, str,
					      XmNindicatorType, XmN_OF_MANY,
					      XmNset, True,
					      XmNtopAttachment, XmATTACH_FORM,
					      XmNleftAttachment, XmATTACH_FORM,
					      NULL);
    XmStringFree(str);

    str = XmStringCreateLocalized("As Popups");
    tips_popups = XtVaCreateManagedWidget(Xdvi_TIPS_POPUPS,
					  xmToggleButtonGadgetClass, tips_form,
					  XmNlabelString, str,
					  XmNindicatorType, XmN_OF_MANY,
					  XmNtopAttachment, XmATTACH_WIDGET,
					  XmNtopWidget, tips_statusline,
					  XmNleftAttachment, XmATTACH_FORM,
					  NULL);
    XmStringFree(str);

    str = XmStringCreateLocalized("with");
    tips_delay_label1 = XtVaCreateManagedWidget(Xdvi_TIPS_DELAY_LABEL1,
						xmLabelGadgetClass, tips_form,
						XmNlabelString, tips_form,
						XmNlabelString, str,
						XmNtopAttachment, XmATTACH_WIDGET,
						XmNtopWidget, tips_statusline,
						XmNleftAttachment, XmATTACH_WIDGET,
						XmNleftWidget, tips_popups,
						XmNleftOffset, 0, /* no spacing to prev text */
						NULL);
    XmStringFree(str);

    SNPRINTF(buf, LENGTH_OF_INT, "%d", ABS(resource.tooltips_wait_period));
    
    tips_delay_text = XtVaCreateManagedWidget(Xdvi_TIPS_DELAY_TEXT,
					      xmTextFieldWidgetClass, tips_form,
					      XmNcolumns, 4,
					      XmNtopAttachment, XmATTACH_WIDGET,
					      XmNtopWidget, tips_statusline,
					      XmNleftAttachment, XmATTACH_WIDGET,
					      XmNleftWidget, tips_delay_label1,
					      XmNvalue, buf,
					      NULL);

    str = XmStringCreateLocalized("milliseconds delay");
    tips_delay_label2 = XtVaCreateManagedWidget(Xdvi_TIPS_DELAY_LABEL2,
						xmLabelGadgetClass, tips_form,
						XmNlabelString, str,
						XmNtopAttachment, XmATTACH_WIDGET,
						XmNtopWidget, tips_statusline,
						XmNleftAttachment, XmATTACH_WIDGET,
						XmNleftWidget, tips_delay_text,
						NULL);

    adjust_heights(tips_popups, tips_delay_label1, tips_delay_text, tips_delay_label2, NULL);

    XmToggleButtonGadgetSetState(tips_statusline, resource.tooltips_in_statusline, False);
    XmToggleButtonGadgetSetState(tips_popups, resource.show_tooltips, False);

    if (resource.toolbar_unusable) {
	XtSetSensitive(tips_frame, False);
    }
    else {
	XtSetSensitive(tips_delay_label1, resource.show_tooltips);
	XtSetSensitive(tips_delay_text, resource.show_tooltips);
	XtSetSensitive(tips_delay_label2, resource.show_tooltips);
    
	XtAddCallback(tips_statusline,  XmNvalueChangedCallback, tooltips_cb, (XtPointer)info);
	XtAddCallback(tips_popups,  XmNvalueChangedCallback, tooltips_cb, (XtPointer)info);
	XtAddCallback(tips_delay_text, XmNvalueChangedCallback, tooltips_cb, (XtPointer)info);
    }

#if 0
    tb_buttons_frame = XmCreateFrame(form, "tb_buttons_frame", &v_off, 1);
    h_attach_below(tb_buttons_frame, tips_frame);
    
    tb_buttons_label = XmCreateLabelGadget(tb_buttons_frame, "Toolbar Buttons:", &frame_title, 1);
    XtManageChild(tb_buttons_label);

    tb_buttons_rowcol = XmCreateRowColumn(tb_buttons_frame, "tb_buttons_rowcol", two_cols, XtNumber(two_cols));
    XtManageChild(tb_buttons_rowcol);
    
    tb_buttons_raised = XmCreateToggleButtonGadget(tb_buttons_rowcol, "Raised", &one_of_many, 1);
    XtManageChild(tb_buttons_raised);

    tb_buttons_flat = XmCreateToggleButtonGadget(tb_buttons_rowcol, "Flat", &one_of_many, 1);
    XtManageChild(tb_buttons_flat);
    toolbar_buttons_init((XtPointer)info, tb_buttons_raised, tb_buttons_flat);
#endif /* 0 */

    n = 0;
    XtSetArg(args[n], XmNmarginWidth, 8); n++;
    XtSetArg(args[n], XmNmarginHeight, 4); n++;
    XtSetArg(args[n], XmNtopOffset, 10); n++;
    match_frame = XmCreateFrame(form, "match_frame", args, n);
    XtVaSetValues(match_frame,
		  XmNtopAttachment, XmATTACH_WIDGET,
		  XmNtopWidget, tips_frame,
		  XmNleftAttachment, XmATTACH_FORM,
		  XmNrightAttachment, XmATTACH_FORM,
		  XmNbottomAttachment, XmATTACH_FORM,
		  NULL);

    match_label = XmCreateLabelGadget(match_frame, "String Matches are shown:", &frame_title, 1);
    XtManageChild(match_label);

    match_rowcol = XmCreateRowColumn(match_frame, "tb_buttons_rowcol", two_cols, XtNumber(two_cols));
    XtManageChild(match_rowcol);
    
    match_inverted = XmCreateToggleButtonGadget(match_rowcol, Xdvi_MATCH_INVERTED_STR, &one_of_many, 1);
    XtManageChild(match_inverted);

    match_boxed = XmCreateToggleButtonGadget(match_rowcol, Xdvi_MATCH_BOXED_STR, &one_of_many, 1);
    XtManageChild(match_boxed);

    XmToggleButtonGadgetSetState(match_inverted, resource.match_highlight_inverted, False);
    XmToggleButtonGadgetSetState(match_boxed, !resource.match_highlight_inverted, False);

    XtAddCallback(match_inverted, XmNvalueChangedCallback, match_cb, (XtPointer)info);
    XtAddCallback(match_boxed, XmNvalueChangedCallback, match_cb, (XtPointer)info);

    /* manage children (order shouldn't matter, since children are already managed, but ...) */
    XtManageChild(gui_frame);
    
    XtManageChild(tips_form);
    XtManageChild(tips_frame);

#if 0
    XtManageChild(tb_buttons_frame);
#endif
    
    XtManageChild(match_frame);

    XtManageChild(form);
    
    return form;
}

#else
/* silence `empty compilation unit' warnings */
static void bar(void); static void foo(void) { bar(); } static void bar(void) { foo(); }
#endif /* MOTIF */
