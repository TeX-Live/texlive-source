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
 * Panel 2 (Fonts and colors) for xdvik preferences dialog.
 */

#include "xdvi-config.h"
#include "xdvi.h"

#include "x_util.h"
#include "xm_colorsel.h"
#include "topic-window.h"
#include "util.h"
#include "events.h"
#include "hypertex.h"
#include "my-snprintf.h"

#include "xm_prefsP.h"
#include "xm_prefs_fonts.h"

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

#if USE_SPINBOX
#include <Xm/SpinB.h>
#endif

/*
 * Handy defaults
 */
static Arg v_off = { XmNtopOffset, 10 };
static Arg n_of_many = { XmNindicatorType, XmN_OF_MANY };
static Arg frame_title = { XmNchildType, XmFRAME_TITLE_CHILD };


static void
underline_toggle_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    struct topic_info *tinfo = (struct topic_info *)client_data;
    struct prefs_choice *prefs;
    Boolean is_set;
    
    UNUSED(call_data);

    ASSERT(tinfo != NULL, "struct topic_info * in underline_toggle_cb mustn't be NULL!");
    prefs = (struct prefs_choice *)tinfo->data;
    
    XtVaGetValues(w, XmNset, &is_set, NULL);
    if (is_set)
	resource.link_style |= 1;
    else
	resource.link_style &= ~1;

    store_preference(&(prefs->db), "linkStyle", "%d", resource.link_style);

    /* update display */
    globals.ev.flags |= EV_NEWPAGE;
    XFlush(DISP);
}

static void
colorspecial_toggle_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    struct topic_info *tinfo = (struct topic_info *)client_data;
    struct prefs_choice *prefs;

    UNUSED(w);
    /*     UNUSED(client_data); */
    UNUSED(call_data);

    ASSERT(tinfo != NULL, "struct topic_info * in colorspecial_toggle_cb mustn't be NULL!");
    prefs = (struct prefs_choice *)tinfo->data;
    
    do_toggle_color(False);
    store_preference(&(prefs->db), "color", "%s", resource.use_color ? "True" : "False");
}

static void
darkness_text_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    char *text;
    int val;
    struct topic_info *info = (struct topic_info *)client_data;
    struct prefs_choice *prefs = (struct prefs_choice *)(info->data);
    
#if USE_SPINBOX
    /* synchronize internal spinbox value. Apparently this loses the insertion position,
       so we need to save it and set it again. */
    XmTextPosition pos = XmTextFieldGetInsertionPosition(w);
    
    UNUSED(call_data);
    text = XmTextFieldGetString(w);
    val = strtol(text, (char **)NULL, 10);

    if (val != 0) {
	XtVaSetValues(w, XmNposition, val, NULL);
	XmTextFieldSetInsertionPosition(w, pos);
    }
#else
    Widget text_field;
    UNUSED(call_data);

    XtVaGetValues(w, XmNuserData, &text_field, NULL);
    XtVaGetValues(text_field, XmNvalue, &text, NULL);
    val = strtol(text, (char **)NULL, 10);
#endif
    if (XtIsRealized(w)) {
	do_set_density(val / 100.0, True, False);
	store_preference(&(prefs->db), "gamma", "%f", val / 100.0);
    }
    XtFree(text);
}


#if 0 /* currently unused */
static void
darkness_spinbox_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    int val;
    struct topic_info *info = (struct topic_info *)client_data;
    struct prefs_choice *prefs = (struct prefs_choice *)(info->data);
    UNUSED(client_data);
    UNUSED(call_data);

    XtVaGetValues(w, XmNposition, &val, NULL);
    if (XtIsRealized(w)) {
	do_set_density(val / 100.0, True, False);
	/* fprintf(stderr, "storing pref: %d\n", val); */
	store_preference(&(prefs->db), "gamma", "%f", val / 100.0);
    }
	
}
#endif /* currently unused */


void
h_update_hyperlinks(Widget w, Pixel pix)
{
    const char *name = XtName(w);
    int type = 0;

    if (strcmp(name, Xdvi_UNVISITED_LINKS_BTN) == 0) {
	type = 1;
    }
    else if (strcmp(name, Xdvi_VISITED_LINKS_BTN) == 0) {
	type = 2;
    }

    /* fprintf(stderr, "WIDGET: |%s|; type: %d\n", name, type); */

    if (type != 0) {
	GC new_gc = set_or_make_gc(NULL, GXcopy, pix, resource.back_Pixel);
	GC old_gc;
	double r, g, b;
	double factor = 65535.0;
	XColor color;

	pixel_to_color(pix, &color, DISP, G_colormap);
	r = color.red / factor;
	g = color.green / factor;
	b = color.blue / factor;

	if (type == 1) {
	    old_gc = globals.gc.linkcolor;
	    sprintf(g_link_color_rgb, "push rgb %.2f %.2f %.2f", r, g, b);
	}
	else {
	    old_gc = globals.gc.visited_linkcolor;
	    sprintf(g_visited_link_color_rgb, "push rgb %.2f %.2f %.2f", r, g, b);
	}

	if (type == 1) {
	    globals.gc.linkcolor = new_gc;
	    XFreeGC(XtDisplay(w), old_gc);
	}
	else {
	    globals.gc.visited_linkcolor = new_gc;
	    XFreeGC(XtDisplay(w), old_gc);
	}
	/* update display */
	globals.ev.flags |= EV_NEWPAGE;
	XFlush(DISP);
    }
}

static void
redraw_or_push_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    XmDrawnButtonCallbackStruct *cbs = (XmDrawnButtonCallbackStruct *)call_data;
    static GC border_gc = 0;
    struct color_button_info *cinfo;
    Pixel pix = (Pixel)client_data;

    if (cbs == NULL) {
	XDVI_WARNING((stderr, "cinfo mustn't be NULL in redraw_or_push_cb!\n"));
	return;
    }

    /* this can also be called via XtCallCallbacks, without call_data */
    if (cbs->reason == XmCR_EXPOSE) {
	/* redraw button label */
	Dimension high_thick, st, wd, ht;
	Dimension inner_w, inner_h, off_x, off_y;
	GC label_gc;
	XGCValues values;
	
	XtVaGetValues(w,
		      XmNhighlightThickness, &high_thick,
		      XmNshadowThickness, &st,
		      XmNwidth, &wd,
		      XmNheight, &ht,
		      XmNuserData, &cinfo,
		      NULL);
	
	values.function = GXcopy;
	
	if (border_gc == 0) {
	    /* create the static GC for drawing the border - Note: never free'd! */
	    values.foreground = BlackPixelOfScreen(XtScreen(w));
	    /* values.line_width = 2; */
	    /* note: never free'd! */
	    border_gc = XCreateGC(XtDisplay(w), XtWindow(w),
				  GCFunction | GCForeground /*  | GCLineWidth */, &values);
	}
	values.foreground = cinfo->pixel;
	label_gc = XCreateGC(XtDisplay(w), XtWindow(w),
			     GCFunction | GCForeground, &values);
	
	off_x = high_thick + st + 4;
	inner_w = wd - 2 * off_x;
	off_y = high_thick + st + 4;
	inner_h = ht - 2 * off_y;
#if 0
	fprintf(stderr, "shadow: %d, high: %d\n", st, high_thick);
	fprintf(stderr, "width: %d, height: %d\n", wd, ht);
	fprintf(stderr, "inner w: %d, inner h: %d\n", inner_w, inner_h);
#endif
	
	XFillRectangle(XtDisplay(w), XtWindow(w), label_gc,
		       off_x, off_y, inner_w, inner_h);
	XDrawRectangle(XtDisplay(w), XtWindow(w), border_gc,
		       off_x - 1, off_y - 1, inner_w + 1, inner_h + 1);
	XFreeGC(XtDisplay(w), label_gc);

	if (pix != cinfo->pixel) {
	    h_update_hyperlinks(w, cinfo->pixel);
	}
    }
    else if (cbs->reason == XmCR_ACTIVATE) {
	/* pushed, open color dialog */
	XtVaGetValues(w, XmNuserData, &cinfo, NULL);
	if (cinfo != NULL) {
	    cinfo->w = w;
	    popup_color_dialog(globals.widgets.top_level, cinfo);
	}
	else {
	    XDVI_WARNING((stderr, "cinfo mustn't be NULL in redraw_or_push_cb!\n"));
	    return;
	}
    }
    /* else, XmCR_RESIZE - nothing to do */
}

static Widget
h_create_colorsample(Widget parent, Widget left,
		     const char *name,
		     const char *resource_name,
		     Pixel pix,
		     struct topic_info *info)
{
    struct color_button_info *cinfo = xmalloc(sizeof *cinfo); /* note: never free()d */
    Widget button;
    
    UNUSED(left);

    cinfo->pixel = pix;
    cinfo->tinfo = info;
    cinfo->resource_name = xstrdup(resource_name); /* note: never free()d */
    
    button = XtVaCreateManagedWidget(name, xmDrawnButtonWidgetClass, parent,
				     XtNwidth, 42,
				     XtNheight, 28,
				     XmNuserData, cinfo,
				     /* we want the normal button look here to
					make it obvious that users can press it */
				     XmNshadowType, XmSHADOW_OUT,
				     XmNpushButtonEnabled, True,
				     NULL);
    XtAddCallback(button, XmNactivateCallback, redraw_or_push_cb, (XtPointer)pix);
    XtAddCallback(button, XmNexposeCallback, redraw_or_push_cb, (XtPointer)pix);
    XtAddCallback(button, XmNresizeCallback, redraw_or_push_cb, (XtPointer)pix);

    return button;
}


static Widget
h_create_fonts_frame(Widget top, struct topic_info *info)
{
    Widget darkness_form, darkness_label;

    UNUSED(info);
    
    darkness_form = XmCreateForm(top, "darkness_form", NULL, 0);
    XtVaSetValues(darkness_form,
		  XmNverticalSpacing, 10,
		  XmNhorizontalSpacing, 10,
		  NULL);

    
    darkness_label = XmCreateLabelGadget(darkness_form, "Font Darkness: ", NULL, 0);
    XtVaSetValues(darkness_label,
		  XmNtopAttachment, XmATTACH_FORM,
		  XmNleftAttachment, XmATTACH_FORM,
		  XmNbottomAttachment, XmATTACH_FORM,
		  NULL);
    XtManageChild(darkness_label);
    {
	Widget darkness_text;
#if USE_SPINBOX
	Widget darkness_spinbox = XmCreateSpinBox(darkness_form, Xdvi_DARKNESS_SPINBOX, NULL, 0);
	darkness_text = XmCreateTextField(darkness_spinbox, Xdvi_DARKNESS_TEXT, NULL, 0);
	XtVaSetValues(darkness_spinbox,
		      XmNtopAttachment, XmATTACH_FORM,
		      XmNleftAttachment, XmATTACH_WIDGET,
		      XmNleftWidget, darkness_label,
		      XmNinitialDelay, 200,
		      XmNrepeatDelay, 50,
		      NULL);
	XtVaSetValues(darkness_text,
		      XmNspinBoxChildType, XmNUMERIC,
		      XmNminimumValue, 0,
		      XmNmaximumValue, 1000,
		      XmNeditable, True,
		      XmNcolumns, 4,
		      XmNincrementValue, 1,
		      XmNwrap, False, /* too confusing */
		      XmNposition, (int)(resource.gamma * 100.0 + 0.5),
		      NULL);
	XtAddCallback(darkness_text, XmNactivateCallback, darkness_text_cb, (XtPointer)info);
	XtAddCallback(darkness_text, XmNvalueChangedCallback, darkness_text_cb, (XtPointer)info);

	adjust_heights(darkness_spinbox, darkness_text, darkness_label, NULL);
	
	XtManageChild(darkness_text);
	XtManageChild(darkness_spinbox);
#else
	char buf[LENGTH_OF_INT];
	Widget darkness_button;
	darkness_text = XmCreateTextField(darkness_form, Xdvi_DARKNESS_TEXT, NULL, 0);
	SNPRINTF(buf, LENGTH_OF_INT, "%d", (int)(resource.gamma * 100.0 + 0.5));
	XtVaSetValues(darkness_text,
		      XmNtopAttachment, XmATTACH_FORM,
		      XmNleftAttachment, XmATTACH_WIDGET,
		      XmNleftWidget, darkness_label,
		      XmNcolumns, 4,
		      XmNvalue, buf,
		      XmNuserData, darkness_text,
		      NULL);
	XtOverrideTranslations(darkness_text, XtParseTranslationTable("<Key>Return:activate()"));
	XtAddCallback(darkness_text, XmNactivateCallback, darkness_text_cb, (XtPointer)info);

	darkness_button = XmCreatePushButton(darkness_form, Xdvi_APPLY_STR, NULL, 0);
	XtVaSetValues(darkness_button,
		      XmNtopAttachment, XmATTACH_FORM,
		      XmNleftAttachment, XmATTACH_WIDGET,
		      XmNleftWidget, darkness_text,
		      XmNuserData, darkness_text,
		      NULL);
	XtAddCallback(darkness_button, XmNactivateCallback, darkness_text_cb, (XtPointer)info);
	
	adjust_heights(darkness_label, darkness_text, darkness_button, NULL);
	XtManageChild(darkness_text);
	XtManageChild(darkness_button);
#endif
    }
    
    /*     XtManageChild(darkness_form); */

    return darkness_form;
}

static Widget
h_create_colors_form(Widget top, struct topic_info *info)
{
    Widget text_bg_frame, text_bg_label, text_bg_form;
    Widget links_frame, links_label, links_form;
    Widget fg_label, fg_button;
    Widget bg_label, bg_button;
    Widget unvisited_label, unvisited_button;
    Widget visited_label, visited_button;
    Widget underline_toggle;
    Widget colorspecials_toggle;
    
    Widget form = XmCreateForm(top, "colors_form", NULL, 0);
    Pixel visited_link_pix, link_pix;

    str_to_pixel(top, resource.visited_link_color, &visited_link_pix);
    str_to_pixel(top, resource.link_color, &link_pix);
    
    text_bg_frame = XmCreateFrame(form, "text_bg_frame", NULL, 0);
    XtVaSetValues(text_bg_frame,
		  XmNmarginWidth, 10,
		  XmNmarginHeight, 10,
		  XmNtopAttachment, XmATTACH_FORM,
		  XmNleftAttachment, XmATTACH_FORM,
		  XmNrightAttachment, XmATTACH_POSITION,
		  XmNrightPosition, 50,
		  XmNrightOffset, 10,
		  XmNbottomAttachment, XmATTACH_FORM,
		  NULL);

    text_bg_label = XmCreateLabelGadget(text_bg_frame, "Text Color", &frame_title, 1);
    XtManageChild(text_bg_label);

    text_bg_form = XmCreateForm(text_bg_frame, "fg_form", NULL, 0);
    XtVaSetValues(text_bg_form, XmNverticalSpacing, 2, NULL);
    
    fg_label = XmCreateLabelGadget(text_bg_form, Xdvi_FG_COLOR_STR, NULL, 0);
    fg_button = h_create_colorsample(text_bg_form, fg_label,
				     Xdvi_FG_COLOR_BTN, "foreground",
				     resource.fore_Pixel, info);
    XtVaSetValues(fg_label,
		  XmNleftAttachment, XmATTACH_FORM,
		  XmNtopAttachment, XmATTACH_FORM,
		  NULL);
    XtVaSetValues(fg_button,
		  XmNrightAttachment, XmATTACH_FORM,
		  XmNleftAttachment, XmATTACH_NONE,
		  NULL);
    adjust_heights(fg_label, fg_button, NULL);
    XtManageChild(fg_label);
    XtManageChild(fg_button);
    
    bg_label = XmCreateLabelGadget(text_bg_form, Xdvi_BG_COLOR_STR, NULL, 0);
    bg_button = h_create_colorsample(text_bg_form, bg_label,
				     Xdvi_BG_COLOR_BTN, "background",
				     resource.back_Pixel, info);
    XtVaSetValues(bg_label,
		  XmNleftAttachment, XmATTACH_FORM,
		  XmNtopAttachment, XmATTACH_WIDGET,
		  XmNtopWidget, fg_label,
		  NULL);
    XtVaSetValues(bg_button,
		  XmNrightAttachment, XmATTACH_FORM,
		  XmNleftAttachment, XmATTACH_NONE,
		  XmNtopAttachment, XmATTACH_WIDGET,
		  XmNtopWidget, fg_label,
		  NULL);
    adjust_heights(bg_label, bg_button, NULL);
    XtManageChild(bg_label);
    XtManageChild(bg_button);

    colorspecials_toggle = XmCreateToggleButtonGadget(text_bg_form, Xdvi_DOCUMENT_COLORS_STR, &n_of_many, 1);
    XtVaSetValues(colorspecials_toggle,
		  XmNleftAttachment, XmATTACH_FORM,
		  XmNtopAttachment, XmATTACH_WIDGET,
		  XmNtopWidget, bg_label,
		  NULL);
    XtManageChild(colorspecials_toggle);
    XmToggleButtonGadgetSetState(colorspecials_toggle, resource.use_color, False);
    XtAddCallback(colorspecials_toggle, XmNvalueChangedCallback, colorspecial_toggle_cb, (XtPointer)info);
    
    links_frame = XmCreateFrame(form, "links_frame", NULL, 0);
    XtVaSetValues(links_frame,
		  XmNmarginWidth, 10,
		  XmNmarginHeight, 10,
		  XmNtopAttachment, XmATTACH_FORM,
		  XmNleftAttachment, XmATTACH_WIDGET,
		  XmNleftWidget, text_bg_frame,
		  XmNleftOffset, 10,
		  XmNrightAttachment, XmATTACH_FORM,
		  XmNbottomAttachment, XmATTACH_FORM,
		  NULL);

    links_label = XmCreateLabelGadget(links_frame, "Hyperlinks", &frame_title, 1);
    XtManageChild(links_label);

    links_form = XmCreateForm(links_frame, "links_form", NULL, 0);
    XtVaSetValues(links_form,
		  XmNverticalSpacing, 2,
		  NULL);

    unvisited_label = XmCreateLabelGadget(links_form, Xdvi_UNVISITED_LINKS_STR, NULL, 0);
    unvisited_button = h_create_colorsample(links_form, unvisited_label,
					    Xdvi_UNVISITED_LINKS_BTN, "linkColor",
					    link_pix, info);
    XtVaSetValues(unvisited_label,
		  XmNleftAttachment, XmATTACH_FORM,
		  XmNtopAttachment, XmATTACH_FORM,
		  NULL);
    XtVaSetValues(unvisited_button,
		  XmNrightAttachment, XmATTACH_FORM,
		  XmNleftAttachment, XmATTACH_NONE,
		  XmNtopAttachment, XmATTACH_FORM,
		  NULL);
    adjust_heights(unvisited_label, unvisited_button, NULL);
    XtManageChild(unvisited_label);
    XtManageChild(unvisited_button);
    
    visited_label = XmCreateLabelGadget(links_form, Xdvi_VISITED_LINKS_STR, NULL, 0);
    visited_button = h_create_colorsample(links_form, visited_label,
					  Xdvi_VISITED_LINKS_BTN, "visitedLinkColor",
					  visited_link_pix, info);
    XtVaSetValues(visited_label,
		  XmNleftAttachment, XmATTACH_FORM,
		  XmNtopAttachment, XmATTACH_WIDGET,
		  XmNtopWidget, unvisited_label,
		  NULL);
    XtVaSetValues(visited_button,
		  XmNrightAttachment, XmATTACH_FORM,
		  XmNleftAttachment, XmATTACH_NONE,
		  XmNtopAttachment, XmATTACH_WIDGET,
		  XmNtopWidget, unvisited_label,
		  NULL);
    adjust_heights(visited_label, visited_button, NULL);
    XtManageChild(visited_label);
    XtManageChild(visited_button);

    underline_toggle = XmCreateToggleButtonGadget(links_form, Xdvi_LINKS_UNDERLINED_STR, &n_of_many, 1);
    XtVaSetValues(underline_toggle,
		  XmNleftAttachment, XmATTACH_FORM,
		  XmNtopAttachment, XmATTACH_WIDGET,
		  XmNtopWidget, visited_label,
		  NULL);
    XtManageChild(underline_toggle);

    /*     if (res->link_style & 1) */
    if (resource.link_style & 1)
	XmToggleButtonGadgetSetState(underline_toggle, True, False);
    else
	XmToggleButtonGadgetSetState(underline_toggle, False, False);
    
    XtAddCallback(underline_toggle, XmNvalueChangedCallback, underline_toggle_cb, (XtPointer)info);
    
    XtManageChild(links_frame);
    XtManageChild(text_bg_frame);

    XtManageChild(text_bg_form);
    XtManageChild(links_form);

    return form;
}

void update_preferences_color(void)
{
    Widget shell, button;

    /* it's not an error if the prefs dialog doesn't exist yet */
    if (get_widget_by_name(&shell, globals.widgets.top_level, Xdvi_PREFS_DIALOG_NAME, False)
	&& get_widget_by_name(&button, shell, Xdvi_DOCUMENT_COLORS_STR, True)) {
	XmToggleButtonGadgetSetState(button, resource.use_color, False);
    }
}

void update_preferences_hyperlinks(void)
{
    Widget shell, button;

    /* it's not an error if the prefs dialog doesn't exist yet */
    if (get_widget_by_name(&shell, globals.widgets.top_level, Xdvi_PREFS_DIALOG_NAME, False)
	&& get_widget_by_name(&button, shell, Xdvi_LINKS_UNDERLINED_STR, True)) {
	XmToggleButtonGadgetSetState(button, resource.link_style & 1 ? True : False, False);
    }
}

void update_preferences_darkness(void)
{
    Widget shell, text;

    if (get_widget_by_name(&shell, globals.widgets.top_level, Xdvi_PREFS_DIALOG_NAME, False)
	&& get_widget_by_name(&text, shell, Xdvi_DARKNESS_TEXT, True)) {

	char buf[LENGTH_OF_INT];
	SNPRINTF(buf, LENGTH_OF_INT, "%d", (int)(resource.gamma * 100.0 + 0.5));
#if USE_SPINBOX
	XmTextFieldSetString(text, buf);
	XtVaSetValues(text, XmNposition, (int)(resource.gamma * 100.0 + 0.5), NULL);
#else
	XtVaSetValues(text, XmNvalue, buf, NULL);
#endif
    }
}

Widget
prefs_fonts_and_colors(struct topic_info *info)
{
    Widget form;
    Widget colors_form;
    Widget other_colors_frame, fonts_frame;

    form = XmCreateForm(info->right_form, "fonts_colors_form", NULL, 0);
    h_attach_below(form, NULL);
    
    colors_form = h_create_colors_form(form, info);
    h_attach_below(colors_form, NULL);
    XtManageChild(colors_form);

    {	/* other colors - currently only highlight color */
	Widget other_colors_form, other_colors_label;
	Widget hl_color_text, hl_color_button;
	
	other_colors_frame = XmCreateFrame(form, "other_colors_frame", &v_off, 1);
	XtVaSetValues(other_colors_frame,
		      XmNmarginWidth, 10,
		      NULL);
	h_attach_below(other_colors_frame, colors_form);

	other_colors_label = XmCreateLabelGadget(other_colors_frame, "Highlight Color", &frame_title, 1);
	other_colors_form = XmCreateForm(other_colors_frame, "other_colors_form", NULL, 0);
	XtVaSetValues(other_colors_form, XmNverticalSpacing, 2, NULL);

	hl_color_text = XmCreateLabelGadget(other_colors_form,
#if defined(LESSTIF_VERSION) /* stupid LessTif doesn't wrap Labels at '\n' */
					    "Color of page border, rulers and bounding boxes.",
#else					    
					    "Color used for page border, rulers in `ruler mode', and\n"
					    "bounding boxes for forward search and EPS images.",
#endif
					    NULL, 0);
	hl_color_button = h_create_colorsample(other_colors_form, hl_color_text,
					       Xdvi_HL_COLOR_BTN, "highlight",
					       resource.hl_Pixel, info);
	XtVaSetValues(hl_color_text,
		      XmNleftAttachment, XmATTACH_FORM,
		      XmNtopAttachment, XmATTACH_FORM,
		      XmNbottomAttachment, XmATTACH_FORM,
		      XmNbottomOffset, 10,
		      XmNalignment, XmALIGNMENT_BEGINNING,
		      NULL);
	XtVaSetValues(hl_color_button,
		      XmNtopAttachment, XmATTACH_FORM,
		      XmNrightAttachment, XmATTACH_FORM,
		      NULL);
	
	XtManageChild(hl_color_text);
	XtManageChild(hl_color_button);
	XtManageChild(other_colors_form);
	XtManageChild(other_colors_label);
	XtManageChild(other_colors_frame);
    }
    
    fonts_frame = h_create_fonts_frame(form, info);
    h_attach_below(fonts_frame, other_colors_frame);
    XtManageChild(fonts_frame);

    XtManageChild(form);

    return form;
}

#else
/* silence `empty compilation unit' warnings */
static void bar(void); static void foo(void) { bar(); } static void bar(void) { foo(); }
#endif /* MOTIF */
