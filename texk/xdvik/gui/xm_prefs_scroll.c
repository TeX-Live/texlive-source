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
 * Panel 5 (Scrolling behaviour) for xdvik preferences dialog.
 */

#include "xdvi-config.h"
#include "xdvi.h"

#include "x_util.h"
#include "xm_colorsel.h"
#include "topic-window.h"
#include "util.h"
#include "events.h"
#include "dvi-init.h"
#include "statusline.h"

#include "xm_prefsP.h"
#include "xm_prefs_scroll.h"
#include "my-snprintf.h"

#ifdef MOTIF /* entire file */

#include <ctype.h>

#include <X11/Xatom.h>

#include <Xm/Xm.h>
#include <Xm/Protocols.h>
#include <Xm/DialogS.h>
#include <Xm/LabelG.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/PanedW.h>
#include <Xm/DrawnB.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>
#include <Xm/ToggleBG.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/CascadeBG.h>

/*
 * Handy defaults
 */
static Arg one_of_many = { XmNindicatorType, XmONE_OF_MANY };

static void
select_unit_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    struct topic_info *info = (struct topic_info *)client_data;
    struct prefs_choice *prefs = (struct prefs_choice *)(info->data);

    Widget pulldown = XtParent(w);
    Widget form = XtParent(XtParent(pulldown));
    Widget text_x, text_y;
    
    const char *w_name = XtName(w);

    UNUSED(call_data);
    
    fprintf(stderr, "setting name: %s\n", w_name);
    XtVaSetValues(pulldown,  XmNuserData, (XtPointer)w_name, NULL);

    if (get_widget_by_name(&text_x, form, Xdvi_HOME_POSITION_X_OFF_TEXT, True)
	&& get_widget_by_name(&text_y, form, Xdvi_HOME_POSITION_Y_OFF_TEXT, True)) {
	const char *buf_x = XmTextFieldGetString(text_x);
	const char *buf_y = XmTextFieldGetString(text_y);

	int val_x = strtol(buf_x, (char **)NULL, 10);
	int val_y = strtol(buf_y, (char **)NULL, 10);
	
	static char x_off[LENGTH_OF_INT + 16];
	static char y_off[LENGTH_OF_INT + 16];

	SNPRINTF(x_off, LENGTH_OF_INT + 16, "%s%s", buf_x, w_name);
	SNPRINTF(y_off, LENGTH_OF_INT + 16, "%s%s", buf_y, w_name);

	resource.sidemargin = x_off;
	resource.topmargin = y_off;

	resource.sidemargin_int = val_x;
	resource.topmargin_int = val_y;

	store_preference(&(prefs->db), "sideMargin", "%s", x_off);
	store_preference(&(prefs->db), "topMargin", "%s", y_off);

	globals.ev.flags |= EV_NEWPAGE;
	XFlush(DISP);
	
	XtFree((char *)buf_x);
	XtFree((char *)buf_y);
    }
}

static void
set_offset_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    struct topic_info *info = (struct topic_info *)client_data;
    struct prefs_choice *prefs = (struct prefs_choice *)(info->data);

    Widget form = XtParent(w);
    Widget pulldown;
    
    UNUSED(call_data);

    if (get_widget_by_name(&pulldown, form, Xdvi_HOME_POSITION_UNITS_PULLDOWN, True)) {
	char *ptr = NULL;
	const char *w_name = XtName(w);
	
	XtVaGetValues(pulldown, XmNuserData, &ptr, NULL);

	if (ptr == NULL) {
	    XDVI_WARNING((stderr, "XmNuserData for %s is NULL in set_offset_cb()\n",
			  Xdvi_HOME_POSITION_UNITS_PULLDOWN));
	    return;
	}
	fprintf(stderr, "UNIT: |%s|\n", ptr);
	
	if (strcmp(w_name, Xdvi_HOME_POSITION_X_OFF_TEXT) == 0) {
	    static char x_off[LENGTH_OF_INT + 16];
	    const char *buf = XmTextFieldGetString(w);
	    int val = strtol(buf, (char **)NULL, 10);
	    
	    SNPRINTF(x_off, LENGTH_OF_INT + 16, "%s%s", buf, ptr);

	    XtFree((char *)buf);
	    
	    resource.sidemargin = x_off;
	    resource.sidemargin_int = val;
	    store_preference(&(prefs->db), "sideMargin", "%s", x_off);

	    goto_page(current_page, resource.keep_flag ? NULL : home, True);
	}
	else if (strcmp(w_name, Xdvi_HOME_POSITION_Y_OFF_TEXT) == 0) {
	    static char y_off[LENGTH_OF_INT + 16];
	    const char *buf = XmTextFieldGetString(w);
	    int val = strtol(buf, (char **)NULL, 10);
	    
	    SNPRINTF(y_off, LENGTH_OF_INT + 16, "%s%s", buf, ptr);

	    XtFree((char *)buf);
	    
	    resource.topmargin = y_off;
	    resource.topmargin_int = val;
	    store_preference(&(prefs->db), "topMargin", "%s", y_off);

	    goto_page(current_page, resource.keep_flag ? NULL : home, True);
	}
	else {
	    XDVI_WARNING((stderr, "unexpected widget name `%s' in set_offset_cb()", w_name));
	}
    }
}

static void
home_position_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    struct topic_info *info = (struct topic_info *)client_data;
    struct prefs_choice *prefs = (struct prefs_choice *)(info->data);

    Widget parent = XtParent(XtParent(w));
    Widget toggle_home, toggle_curr, label_x, label_y, text_x, text_y, units_menu;
    
    UNUSED(call_data);

    resource.use_current_offset = !resource.use_current_offset;

    if (get_widget_by_name(&toggle_home, parent, Xdvi_HOME_POSITION_STR, True)
	&& get_widget_by_name(&toggle_curr, parent, Xdvi_SCROLL_CURRENT_STR, True)
	&& get_widget_by_name(&label_x, parent, Xdvi_HOME_POSITION_X_STR, True)
	&& get_widget_by_name(&label_y, parent, Xdvi_HOME_POSITION_Y_STR, True)
	&& get_widget_by_name(&text_x, parent, Xdvi_HOME_POSITION_X_OFF_TEXT, True)
	&& get_widget_by_name(&text_y, parent, Xdvi_HOME_POSITION_Y_OFF_TEXT, True)
	&& get_widget_by_name(&units_menu, parent, Xdvi_HOME_POSITION_UNITS_MENU, True)) {

	XmToggleButtonGadgetSetState(toggle_home, !resource.use_current_offset, False);
	XmToggleButtonGadgetSetState(toggle_curr, resource.use_current_offset, False);

	XtSetSensitive(label_x, !resource.keep_flag && !resource.use_current_offset);
	XtSetSensitive(label_y, !resource.keep_flag && !resource.use_current_offset);
	XtSetSensitive(text_x, !resource.keep_flag && !resource.use_current_offset);
	XtSetSensitive(text_y, !resource.keep_flag && !resource.use_current_offset);
	XtSetSensitive(units_menu, !resource.keep_flag && !resource.use_current_offset);	
    }

    store_preference(&(prefs->db), "useCurrentOffset", "%s", resource.use_current_offset ? "True" : "False");
}

static void
keep_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    struct topic_info *info = (struct topic_info *)client_data;
    struct prefs_choice *prefs = (struct prefs_choice *)(info->data);

    Widget parent, keep, unkeep, toggle_home, toggle_curr, label_x, label_y, text_x, text_y, units_menu;

    UNUSED(call_data);
    
    resource.keep_flag = !resource.keep_flag;
    
    parent = XtParent(w);

    if (get_widget_by_name(&keep, parent, Xdvi_SCROLL_KEEP_STR, True)
	&& get_widget_by_name(&unkeep, parent, Xdvi_SCROLL_UNKEEP_STR, True)
	&& get_widget_by_name(&toggle_home, parent, Xdvi_HOME_POSITION_STR, True)
	&& get_widget_by_name(&toggle_curr, parent, Xdvi_SCROLL_CURRENT_STR, True)
	&& get_widget_by_name(&label_x, parent, Xdvi_HOME_POSITION_X_STR, True)
	&& get_widget_by_name(&label_y, parent, Xdvi_HOME_POSITION_Y_STR, True)
	&& get_widget_by_name(&text_x, parent, Xdvi_HOME_POSITION_X_OFF_TEXT, True)
	&& get_widget_by_name(&text_y, parent, Xdvi_HOME_POSITION_Y_OFF_TEXT, True)
	&& get_widget_by_name(&units_menu, parent, Xdvi_HOME_POSITION_UNITS_MENU, True)) {

	XmToggleButtonGadgetSetState(keep, resource.keep_flag, False);
	XmToggleButtonGadgetSetState(unkeep, !resource.keep_flag, False);

	XtSetSensitive(label_x, !resource.keep_flag && !resource.use_current_offset);
	XtSetSensitive(label_y, !resource.keep_flag && !resource.use_current_offset);
	XtSetSensitive(text_x, !resource.keep_flag && !resource.use_current_offset);
	XtSetSensitive(text_y, !resource.keep_flag && !resource.use_current_offset);
	XtSetSensitive(units_menu, !resource.keep_flag && !resource.use_current_offset);
	XtSetSensitive(toggle_home, !resource.keep_flag);
	XtSetSensitive(toggle_curr, !resource.keep_flag);
    }

    store_preference(&(prefs->db), "keepPosition", "%s", resource.keep_flag ? "True" : "False");

}

static Widget
h_create_scrolling(Widget parent, struct topic_info *info)
{
    Widget form, form1, form2, size_option, curr_option;
    Widget x_off_label, x_off_text;
    Widget y_off_label, y_off_text;
    Widget off_units_menu, cascade, entry1, entry2, entry3;

    Widget scroll_keep, scroll_unkeep;
    
    char x_buf[LENGTH_OF_INT + 16];
    char y_buf[LENGTH_OF_INT + 16];
    int n;
    Arg args[8];

    fprintf(stderr, "sidemargin: %s, topmargin: %s\n", resource.sidemargin, resource.topmargin);
    
    if (resource.sidemargin != NULL) {
	char *ptr;
	strcpy(x_buf, resource.sidemargin);
	ptr = x_buf;
	if (*ptr == '-')
	    ptr++;
	while (isdigit((int)*ptr))
	    ptr++;
	*ptr = '\0';
    }
    else
	strcpy(x_buf, "0");
    
    if (resource.topmargin != NULL) {
	char *ptr;
	strcpy(y_buf, resource.topmargin);
	ptr = y_buf;
	if (*ptr == '-')
	    ptr++;
	while (isdigit((int)*ptr))
	    ptr++;
	*ptr = '\0';
    }
    else
	strcpy(y_buf, "0");
    
    form = XmCreateForm(parent, "offsets_form", NULL, 0);

    scroll_keep = XmCreateToggleButtonGadget(form, Xdvi_SCROLL_KEEP_STR, &one_of_many, 1);
    XtVaSetValues(scroll_keep,
		  XmNtopAttachment, XmATTACH_FORM,
		  XmNleftAttachment, XmATTACH_FORM,
		  NULL);
    scroll_unkeep = XmCreateToggleButtonGadget(form, Xdvi_SCROLL_UNKEEP_STR, &one_of_many, 1);
    XtVaSetValues(scroll_unkeep,
		  XmNtopAttachment, XmATTACH_WIDGET,
		  XmNtopWidget, scroll_keep,
		  XmNleftAttachment, XmATTACH_FORM,
		  NULL);

    XmToggleButtonGadgetSetState(scroll_keep, resource.keep_flag, False);
    XmToggleButtonGadgetSetState(scroll_unkeep, !resource.keep_flag, False);

    XtAddCallback(scroll_keep, XmNvalueChangedCallback, keep_cb, (XtPointer)info);
    XtAddCallback(scroll_unkeep, XmNvalueChangedCallback, keep_cb, (XtPointer)info);
    
    XtManageChild(scroll_keep);
    XtManageChild(scroll_unkeep);
    
    form1 = XmCreateForm(form, "offsets_form1", NULL, 0);
    XtVaSetValues(form1,
		  XmNtopAttachment, XmATTACH_WIDGET,
		  XmNtopWidget, scroll_unkeep,
		  XmNleftAttachment, XmATTACH_FORM,
		  XmNleftOffset, 20,
		  NULL);
    
    size_option = XmCreateToggleButtonGadget(form1, Xdvi_HOME_POSITION_STR, &one_of_many, 1);
    XtVaSetValues(size_option,
		  XmNtopAttachment, XmATTACH_FORM,
		  XmNleftAttachment, XmATTACH_FORM,
		  XmNbottomAttachment, XmATTACH_FORM,
		  NULL);
    x_off_label = XmCreateLabelGadget(form1, Xdvi_HOME_POSITION_X_STR, NULL, 0);
    XtVaSetValues(x_off_label,
		  XmNtopAttachment, XmATTACH_FORM,
		  XmNleftAttachment, XmATTACH_WIDGET,
		  XmNleftWidget, size_option,
		  XmNbottomAttachment, XmATTACH_FORM,
		  NULL);
    x_off_text = XtVaCreateManagedWidget(Xdvi_HOME_POSITION_X_OFF_TEXT,
					 xmTextFieldWidgetClass, form1,
					 XmNcolumns, 4,
					 XmNtopAttachment, XmATTACH_FORM,
					 XmNleftAttachment, XmATTACH_WIDGET,
					 XmNleftWidget, x_off_label,
					 XmNbottomAttachment, XmATTACH_FORM,
					 XmNvalue, x_buf,
					 /* 					 XmNvalue, buf, */
					 NULL);
    y_off_label = XmCreateLabelGadget(form1, Xdvi_HOME_POSITION_Y_STR, NULL, 0);
    XtVaSetValues(y_off_label,
		  XmNtopAttachment, XmATTACH_FORM,
		  XmNleftAttachment, XmATTACH_WIDGET,
		  XmNleftWidget, x_off_text,
		  XmNleftOffset, 10,
		  XmNbottomAttachment, XmATTACH_FORM,
		  NULL);
    y_off_text = XtVaCreateManagedWidget(Xdvi_HOME_POSITION_Y_OFF_TEXT,
					 xmTextFieldWidgetClass, form1,
					 XmNcolumns, 4,
					 XmNtopAttachment, XmATTACH_FORM,
					 XmNleftAttachment, XmATTACH_WIDGET,
					 XmNleftWidget, y_off_label,
					 XmNbottomAttachment, XmATTACH_FORM,
					 XmNvalue, y_buf,
					 NULL);
    n = 0;
    XtSetArg(args[n], XmNuserData, (XtPointer)"in"); n++;
    off_units_menu = XmCreatePulldownMenu(form1, Xdvi_HOME_POSITION_UNITS_PULLDOWN, args, n);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, off_units_menu); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget, y_off_text); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    cascade = XmCreateOptionMenu(form1, Xdvi_HOME_POSITION_UNITS_MENU, args, n);
    entry1 = XtVaCreateManagedWidget("in", xmPushButtonGadgetClass, off_units_menu,
				     NULL);
    XtAddCallback(entry1, XmNactivateCallback, select_unit_cb, info);
    entry2 = XtVaCreateManagedWidget("cm", xmPushButtonGadgetClass, off_units_menu,
				     NULL);
    XtAddCallback(entry2, XmNactivateCallback, select_unit_cb, info);
    entry3 = XtVaCreateManagedWidget("Pixel", xmPushButtonGadgetClass, off_units_menu,
				     NULL);
    XtAddCallback(entry3, XmNactivateCallback, select_unit_cb, info);

    XtAddCallback(x_off_text, XmNvalueChangedCallback, set_offset_cb, (XtPointer)info);
    XtAddCallback(y_off_text, XmNvalueChangedCallback, set_offset_cb, (XtPointer)info);
    
    XtManageChild(size_option);
    XtManageChild(x_off_label);
    XtManageChild(x_off_text);
    XtManageChild(y_off_label);
    XtManageChild(y_off_text);
    XtManageChild(cascade);

    form2 = XmCreateForm(form, "offsets_form2", NULL, 0);
    XtVaSetValues(form2,
		  XmNtopAttachment, XmATTACH_WIDGET,
		  XmNtopWidget, form1,
		  XmNleftAttachment, XmATTACH_FORM,
		  XmNleftOffset, 20,
		  NULL);

    curr_option = XmCreateToggleButtonGadget(form2, Xdvi_SCROLL_CURRENT_STR, &one_of_many, 1);
    XtVaSetValues(curr_option,
		  XmNtopAttachment, XmATTACH_FORM,
		  XmNleftAttachment, XmATTACH_FORM,
		  XmNbottomAttachment, XmATTACH_FORM,
		  NULL);
    XtManageChild(curr_option);
    
    XtSetSensitive(x_off_label, !resource.keep_flag && !resource.use_current_offset);
    XtSetSensitive(x_off_text, !resource.keep_flag && !resource.use_current_offset);
    XtSetSensitive(y_off_label, !resource.keep_flag && !resource.use_current_offset);
    XtSetSensitive(y_off_text, !resource.keep_flag && !resource.use_current_offset);
    XtSetSensitive(cascade, !resource.keep_flag && !resource.use_current_offset);
    XtSetSensitive(size_option, !resource.keep_flag);
    XtSetSensitive(curr_option, !resource.keep_flag);
    
    XmToggleButtonGadgetSetState(size_option, !resource.use_current_offset, False);
    XmToggleButtonGadgetSetState(curr_option, resource.use_current_offset, False);

    XtAddCallback(size_option, XmNvalueChangedCallback, home_position_cb, (XtPointer)info);
    XtAddCallback(curr_option, XmNvalueChangedCallback, home_position_cb, (XtPointer)info);
    
    XtManageChild(form1);
    XtManageChild(form2);
    
    return form;
}

Widget
prefs_scrolling(struct topic_info *info)
{
    Widget form;
    Widget scrolling_form, scrolling_frame;

    form = XtVaCreateWidget("form", xmFormWidgetClass,
			    info->right_form,
			    XmNtopAttachment, XmATTACH_FORM,
			    XmNleftAttachment, XmATTACH_FORM,
			    XmNrightAttachment, XmATTACH_FORM,
			    XmNbottomAttachment, XmATTACH_FORM,
			    NULL);

    scrolling_frame = XmCreateFrame(form, "scrolling_frame", NULL, 0);
    h_attach_below(scrolling_frame,  NULL);
    
    scrolling_form = h_create_scrolling(scrolling_frame, info);
    XtVaSetValues(scrolling_form,
		  XmNtopAttachment, XmATTACH_WIDGET,
		  XmNtopWidget, scrolling_frame,
		  XmNleftAttachment, XmATTACH_FORM,
		  XmNrightAttachment, XmATTACH_FORM,
		  XmNbottomAttachment, XmATTACH_FORM,
		  NULL);
    
    XtManageChild(scrolling_form);
    XtManageChild(scrolling_frame);

    return form;
}

#else
/* silence `empty compilation unit' warnings */
static void bar(void); static void foo(void) { bar(); } static void bar(void) { foo(); }
#endif /* MOTIF */

