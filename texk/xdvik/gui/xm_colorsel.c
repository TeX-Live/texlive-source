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
 * Color selector window showing a range of colors, and sliders to customize them.
 * Heavily influenced by color_slide.c from ch. 13 of O'Reilly's Motif Programming Manual 1.2.
 */

#include "xdvi-config.h"
#include "xdvi.h"

#include "Tip.h"
#include "x_util.h"
#include "xm_colorsel.h"
#include "xm_prefsP.h"
#include "xm_prefs.h"
#include "events.h"
#include "dvi-draw.h"
#include "util.h"
#include "topic-window.h"
#include "search-internal.h"

#ifdef MOTIF /* entire file */

#include <X11/Xatom.h>

#include <Xm/Xm.h>
#include <Xm/Protocols.h>
#include <Xm/DialogS.h>
#include <Xm/LabelG.h>
#include <Xm/PushB.h>
#include <Xm/Form.h>
#include <Xm/PanedW.h>
#include <Xm/DrawnB.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>
#include <Xm/SelectioB.h>
# include <Xm/AtomMgr.h>

#include <Xm/MwmUtil.h>

#define ROWS 7
#define COLS 10

/* for passing around information - no global data here ;-) */
struct color_info {
    XColor new_color;
    Widget tooltip;
    Widget new_sample;
    Widget r_scale;
    Widget g_scale;
    Widget b_scale;
    struct color_button_info *button_info;
};

/*
 * These are the colors of the GTK/Mozilla color picker
 * (see e.g. http://lxr.mozilla.org/mozilla/source/toolkit/content/widgets/colorpicker.xml)
 * which has the colors more or less sorted by columns.
 * It's terminated by NULL just in case the table gets out of sync with
 * the row/column number.
 */
static const char *colors[] = {
    /* column 1 */
    "#FFFFFF", "#CCCCCC", "#C0C0C0", "#999999", "#666666", "#333333", "#000000",
    /* column 2 */
    "#FFCCCC", "#FF6666", "#FF0000", "#CC0000", "#990000", "#660000", "#330000",
    /* column 3 */
    "#FFCC99", "#FF9966", "#FF9900", "#FF6600", "#CC6600", "#993300", "#663300",
    /* column 4 */
    "#FFFF99", "#FFFF66", "#FFCC66", "#FFCC33", "#CC9933", "#996633", "#663333",
    /* column 5 */
    "#FFFFCC", "#FFFF33", "#FFFF00", "#FFCC00", "#999900", "#666600", "#333300",
    /* column 6 */
    "#99FF99", "#66FF99", "#33FF33", "#33CC00", "#009900", "#006600", "#003300",
    /* column 7 */
    "#99FFFF", "#33FFFF", "#66CCCC", "#00CCCC", "#339999", "#336666", "#003333",
    /* column 8 */
    "#CCFFFF", "#66FFFF", "#33CCFF", "#3366FF", "#3333FF", "#000099", "#000066",
    /* column 9 */
    "#CCCCFF", "#9999FF", "#6666CC", "#6633FF", "#6600CC", "#333399", "#330099",
    /* column 10 */
    "#FFCCFF", "#FF99FF", "#CC66CC", "#CC33CC", "#993399", "#663366", "#330033",
    /* safety */
    NULL
};

#define MOTIF 1

static void
popdown_cb(Widget widget, XtPointer client_data, XtPointer call_data)
{
    Widget dialog;
    struct prefs_choice *prefs = (struct prefs_choice *)client_data;

    UNUSED(call_data);

    ASSERT(prefs != NULL, "user_data in popdown_cb musn't be NULL!");
    if (get_widget_by_name(&dialog, widget, Xdvi_COLOR_DIALOG_NAME, True)) {
	/* 	fprintf(stderr, "popdown!\n"); */
	remove_from_deplist(prefs, dialog);
	XtUnmanageChild(dialog);
    }
}

static void
h_update_sliders(struct color_info *cinfo)
{
    unsigned r, g, b;

    r = cinfo->new_color.red >> 8;
    g = cinfo->new_color.green >> 8;
    b = cinfo->new_color.blue >> 8;

    XtVaSetValues(cinfo->r_scale, XmNvalue, r, NULL);
    XtVaSetValues(cinfo->g_scale, XmNvalue, g, NULL);
    XtVaSetValues(cinfo->b_scale, XmNvalue, b, NULL);
}

static void
popdown_apply_cb(Widget widget, XtPointer client_data, XtPointer call_data)
{
    struct color_info *cinfo = NULL;
    static XmDrawnButtonCallbackStruct cbs;
    struct prefs_choice *prefs = (struct prefs_choice *)client_data;

    UNUSED(call_data);
    ASSERT(prefs != NULL, "struct prefs_choice * in popdown_apply_cb mustn't be NULL!");

    /* To update button to new color, set its button_info color
       and call the button's expose callback */
    XtVaGetValues(widget, XmNuserData, &cinfo, NULL);
    ASSERT(cinfo != NULL, "XmNuserData in popdown_apply_cb musn't be NULL!");
    cinfo->button_info->pixel = cinfo->new_color.pixel;

    cbs.reason = XmCR_EXPOSE;
    XtCallCallbacks(cinfo->button_info->w, XmNexposeCallback, &cbs);
    
    remove_from_deplist(prefs, widget);

    /* Store the current setting -
       FIXME: better way of converting color or pixel to string? pixel_to_str() in x_util.c is broken! */
    store_preference(&(prefs->db), cinfo->button_info->resource_name,
		     "#%.2x%.2x%.2x",
		     cinfo->new_color.red >> 8,
		     cinfo->new_color.green >> 8,
		     cinfo->new_color.blue >> 8);

    /* Update the display to use the new foreground/background.
     * I don't really understand the entire color interface, but
     * apparently the following works, and nothing else I tried did ... */
    if (strcmp(cinfo->button_info->resource_name, "foreground") == 0) {
	XGCValues values;

	fg_initial.r = cinfo->new_color.red;
	fg_initial.g = cinfo->new_color.green;
	fg_initial.b = cinfo->new_color.blue;

	values.foreground = resource.fore_Pixel = resource.rule_pixel = cinfo->new_color.pixel;
	XChangeGC(DISP, globals.gc.ruler, GCForeground, &values);

	scanned_page = scanned_page_color = scanned_page_reset;
	globals.ev.flags |= EV_NEWPAGE;

    }
    else if (strcmp(cinfo->button_info->resource_name, "background") == 0) {
	bg_initial.r = cinfo->new_color.red;
	bg_initial.g = cinfo->new_color.green;
	bg_initial.b = cinfo->new_color.blue;

	resource.back_Pixel = cinfo->new_color.pixel;
	scanned_page = scanned_page_color = scanned_page_reset;
	globals.ev.flags |= EV_RELOAD; /* EV_NEWPAGE not sufficient with color code ... */
    }
    else if (strcmp(cinfo->button_info->resource_name, "highlight") == 0) {
	XGCValues values;
	values.foreground = resource.hl_Pixel = cinfo->new_color.pixel;
	XChangeGC(DISP, globals.gc.high, GCForeground, &values);
	/* hack to update match GC: fake change in inverted property, redraw
	   so that GC is cleared, then change inverted property back */
	resource.match_highlight_inverted = !resource.match_highlight_inverted;
	search_draw_inverted_regions();
	resource.match_highlight_inverted = !resource.match_highlight_inverted;
	/* end of hack to update match GC */
	globals.ev.flags |= EV_NEWPAGE; /* force redraw */
    }
    
    XtUnmanageChild(widget);
}

static void
popdown_cancel_cb(Widget widget, XtPointer client_data, XtPointer call_data)
{
    struct prefs_choice *prefs = (struct prefs_choice *)client_data;
    
    /*     fprintf(stderr, "popdown cancel; call_data: %p!\n", call_data); */
    if (call_data != NULL) {
	/* 	fprintf(stderr, "removing from deplist: %p!!!\n", prefs); */
	remove_from_deplist(prefs, widget);
    }
    
    XtUnmanageChild(widget);
    /*     XtPopdown(XtParent(widget)); */
}

static void
revert_color_cb(Widget widget, XtPointer client_data, XtPointer call_data)
{
    struct color_info *cinfo = (struct color_info *)client_data;
    Pixel pix;

    UNUSED(call_data);
    
    /* get color of old_sample button */
    XtVaGetValues(widget, XmNbackground, &pix, NULL);
    /*  fprintf(stderr, "Color: 0x%.6lx\n", pix); */

    /* use this instead of XtVaSetValues on XmNbackground so that
       the foreground color gets also changed appropriately. */
#if XmVersion >= 1002
    XmChangeColor(cinfo->new_sample, pix);
#else
    XtVaSetValues(cinfo->new_sample, XmNbackground, pix, NULL);
#endif

    /* update sliders */
    XFreeColors(DISP, G_colormap, &(cinfo->new_color.pixel), 1, 0);
    pixel_to_color(pix, &(cinfo->new_color), DISP, G_colormap);
    h_update_sliders(cinfo);
}

static void
show_color_cb(Widget widget, XtPointer client_data, XtPointer call_data)
{
    struct color_info *cinfo = (struct color_info *)client_data;
    Pixel pix;
    
    UNUSED(call_data);
    XtVaGetValues(widget, XmNbackground, &pix, NULL);
    /*      fprintf(stderr, "Color: 0x%.6lx\n", pix); */

    /* use this instead of XtVaSetValues on XmNbackground so that
       the foreground color gets also changed appropriately. */
#if XmVersion >= 1002
    XmChangeColor(cinfo->new_sample, pix);
#else
    XtVaSetValues(cinfo->new_sample, XmNbackground, pix, NULL);
#endif

    /* update sliders */
    XFreeColors(DISP, G_colormap, &(cinfo->new_color.pixel), 1, 0);
    pixel_to_color(pix, &(cinfo->new_color), DISP, G_colormap);
    h_update_sliders(cinfo);
}

static void
slider_cb(Widget widget, XtPointer client_data, XtPointer call_data)
{
    struct color_info *cinfo = (struct color_info *)client_data;
    XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *)call_data;

    /*      fprintf(stderr, "Pixel1 : 0x%.6lx\n", cinfo->new_color.pixel); */
    /* reuse cinfo->new_color */
    XFreeColors(DISP, G_colormap, &(cinfo->new_color.pixel), 1, 0);
    
    if (widget == cinfo->r_scale) {
	cinfo->new_color.red = cbs->value << 8; /* << 8  ==  * 65535 */
    }
    else if (widget == cinfo->g_scale) {
	cinfo->new_color.green = cbs->value << 8;
    }
    else if (widget == cinfo->b_scale) {
	cinfo->new_color.blue = cbs->value << 8;
    }

    if (!XAllocColor(DISP, G_colormap, &(cinfo->new_color))) {
	XDVI_ERROR((stderr, "Couldn't XAllocColor\n"));
	return;
    }
    /*      fprintf(stderr, "Pixel: 0x%.6lx\n", cinfo->new_color.pixel); */

#if XmVersion >= 1002
    /* if avaliable, use this so that the foreground color also
       gets updated appropriately: */
    XmChangeColor(cinfo->new_sample, cinfo->new_color.pixel);
#else
    XtVaSetValues(cinfo->new_sample, XmNbackground, cinfo->new_color.pixel, NULL);
#endif
}

static Widget
h_create_color_matrix(Widget parent, Widget top, struct color_info *cinfo)
{
    int i, j;
    const char **color_ptr = colors;
    	
    Widget matrix =
	XtVaCreateManagedWidget("color_matrix", xmRowColumnWidgetClass,
				parent,
				XmNtopAttachment, XmATTACH_WIDGET,
				XmNtopWidget, top,
				XmNleftAttachment, XmATTACH_FORM,
				XmNrightAttachment, XmATTACH_FORM,
				XmNpacking, XmPACK_COLUMN,
				XmNnumColumns, COLS,
				XmNorientation, XmVERTICAL,
				XmNbackground, BlackPixelOfScreen(XtScreen(parent)),
				XmNspacing, 1,
				XmNmarginWidth, 1,
				XmNmarginHeight, 1,
				NULL);
    
    for (i = 0; i < ROWS; i++) {
	for (j = 0; j < COLS; j++) {
	    Pixel pix = BlackPixelOfScreen(XtScreen(parent));
	    Widget button;
	    if (*color_ptr == NULL) /* safety */
		break;
	    str_to_pixel(top, *color_ptr++, &pix);
	    /*  fprintf(stderr, "creating button %d, %d - %s\n", i, j, k % 2 ? "b" : "w"); */
	    button = XtVaCreateManagedWidget("c_button", xmDrawnButtonWidgetClass,
					     matrix,
					     XmNbackground, pix,
					     XmNheight, 19,
					     XmNwidth, 19,
					     XmNmarginWidth, 0,
					     XmNmarginHeight, 0,
					     XmNhighlightThickness, 0,
					     XmNshadowThickness, 0,
					     NULL);
	    XtAddCallback(button, XmNactivateCallback, show_color_cb, cinfo);
	}
    }
    return matrix;
}

static void
h_create_sliders(Widget parent, Widget top, struct color_info *cinfo)
{
    XmString str;
    Widget r_label, g_label, b_label;
    Widget r_scale, g_scale, b_scale;
    Dimension curr, max;

    Arg scale_args[8];
    Arg label_args[8];
    int scale_n = 0;

    XtSetArg(scale_args[scale_n], XmNmaximum, 255); scale_n++;
    /* True is an older setting of XmNshowValue that should also work with 2.x */
    XtSetArg(scale_args[scale_n], XmNshowValue, True); scale_n++;
    XtSetArg(scale_args[scale_n], XmNorientation, XmHORIZONTAL); scale_n++;
#if XmVersion >= 2000
    XtSetArg(scale_args[scale_n], XmNshowArrows, XmEACH_SIDE); scale_n++;
    /* XmVersion < 2000 only had `True' which was the default anyway */
#endif
    
    str = XmStringCreateLocalized("Red");
    XtSetArg(label_args[0], XmNlabelString, str);
    r_label = XtCreateManagedWidget("r_label", xmLabelGadgetClass, parent, label_args, 1);
    XmStringFree(str);

    r_scale = XtCreateWidget("r_scale", xmScaleWidgetClass, parent, scale_args, scale_n);
    cinfo->r_scale = r_scale;
    XtAddCallback(r_scale, XmNdragCallback, slider_cb, cinfo);
    XtAddCallback(r_scale, XmNvalueChangedCallback, slider_cb, cinfo);

    str = XmStringCreateLocalized("Green");
    XtSetArg(label_args[0], XmNlabelString, str);
    g_label = XtCreateManagedWidget("g_label", xmLabelGadgetClass, parent, label_args, 1);
    XmStringFree(str);

    g_scale = XtCreateWidget("g_scale", xmScaleWidgetClass, parent, scale_args, scale_n);
    cinfo->g_scale = g_scale;
    XtAddCallback(g_scale, XmNdragCallback, slider_cb, cinfo);
    XtAddCallback(g_scale, XmNvalueChangedCallback, slider_cb, cinfo);

    str = XmStringCreateLocalized("Blue");
    XtSetArg(label_args[0], XmNlabelString, str);
    b_label = XtCreateManagedWidget("b_label", xmLabelGadgetClass, parent, label_args, 1);
    XmStringFree(str);
    
    b_scale = XtCreateWidget("b_scale", xmScaleWidgetClass, parent, scale_args, scale_n);
    cinfo->b_scale = b_scale;
    XtAddCallback(b_scale, XmNdragCallback, slider_cb, cinfo);
    XtAddCallback(b_scale, XmNvalueChangedCallback, slider_cb, cinfo);

    /* get max width of labels, and adjust scale size to what's left */
    XtVaGetValues(r_label, XmNwidth, &curr, NULL);
    XtVaGetValues(g_label, XmNwidth, &max, NULL);
    if (curr > max)
	max = curr;
    XtVaGetValues(b_label, XmNwidth, &curr, NULL);
    if (curr > max)
	max = curr;
    /* we know the width of the color selector is about 200 pixels (10 fields each 20 wide),
       so we use that as max length */
    curr = 200 - max - 2; /* some additional offset */
    
    /*  fprintf(stderr, "width of slider: %d\n", curr); */
    /*  fprintf(stderr, "width of string: %d\n", max); */
    
    /* attach widgets; needs to be done after creating them since r_label
       attachment references g_label etc. */
    XtVaSetValues(r_label,
		  XmNleftAttachment, XmATTACH_FORM,
		  XmNbottomAttachment, XmATTACH_WIDGET,
		  XmNbottomWidget, g_label,
		  XmNbottomOffset, 20, /* needed to make label visible */
		  NULL);
    XtVaSetValues(r_scale,
		  XmNtopAttachment, XmATTACH_WIDGET,
		  XmNtopWidget, top,
		  XmNrightAttachment, XmATTACH_FORM,
		  XmNbottomAttachment, XmATTACH_WIDGET,
		  XmNbottomWidget, g_label,
		  XmNbottomOffset, 20, /* needed to make label visible */
		  XmNscaleWidth, curr,
		  NULL);
    XtVaSetValues(g_label,
		  XmNleftAttachment, XmATTACH_FORM,
		  XmNbottomAttachment, XmATTACH_WIDGET,
		  XmNbottomWidget, b_label,
		  XmNbottomOffset, 20, /* needed to make label visible */
		  NULL);
    XtVaSetValues(g_scale,
		  XmNrightAttachment, XmATTACH_FORM,
		  XmNbottomAttachment, XmATTACH_WIDGET,
		  XmNbottomWidget, b_label,
		  XmNbottomOffset, 20, /* needed to make label visible */
		  XmNscaleWidth, curr,
		  NULL);
    XtVaSetValues(b_label,
		  XmNleftAttachment, XmATTACH_FORM,
		  XmNbottomAttachment, XmATTACH_FORM,
		  XmNbottomOffset, 15,
		  NULL);
    XtVaSetValues(b_scale,
		  XmNrightAttachment, XmATTACH_FORM,
		  XmNbottomAttachment, XmATTACH_FORM,
		  XmNbottomOffset, 15,
		  XmNscaleWidth, curr,
		  NULL);

    XtManageChild(r_scale);
    XtManageChild(g_scale);
    XtManageChild(b_scale);

    h_update_sliders(cinfo);
    
}

static Widget
h_create_shell(Widget parent, struct color_info *cinfo)
{
    Widget shell, dialog, form, button_row, color_matrix;
    Widget old_sample, new_sample;
    Widget tip_shell;
    Atom WM_DELETE_WINDOW;
    Arg args[32];
    int n;
    
    XmString str;
    
    Pixel old_pixel = cinfo->button_info->pixel;
    /* hack to disable resizing of the shell, since this messes up the color matrix badly */
    int decoration = MWM_DECOR_ALL | MWM_DECOR_RESIZEH | MWM_DECOR_MENU | MWM_DECOR_MINIMIZE | MWM_DECOR_MAXIMIZE;
    int functions = MWM_FUNC_ALL | MWM_FUNC_RESIZE | MWM_FUNC_MAXIMIZE;

    struct color_button_info *binfo = cinfo->button_info;
    struct topic_info *tinfo = binfo->tinfo;
    struct prefs_choice *prefs = (struct prefs_choice *)tinfo->data;
    /*     struct prefs_choice *prefs; */
    
    n = 0;
    XtSetArg(args[n], XmNuserData, cinfo); n++;

    /*     XtSetArg(args[n], XmNdeleteResponse, XmDO_NOTHING); n++; */
    /*     XtSetArg(args[n], XmNnoResize, True); n++; */
    /*     XtSetArg(args[n], XmNmwmFunctions, functions); n++; */
    /*     XtSetArg(args[n], XmNmwmDecorations, decoration); n++; */
    XtSetArg(args[n], XtNtitle, "Xdvik: Select Color"); n++;
    
    dialog = XmCreatePromptDialog(parent, Xdvi_COLOR_DIALOG_NAME, args, n);
    /* unmanage stuff to get a `scratch' widget */
    XtUnmanageChild(XtNameToWidget(dialog, "Help"));
    XtUnmanageChild(XtNameToWidget(dialog, "Text"));
    XtUnmanageChild(XtNameToWidget(dialog, "Selection"));

    shell = XtParent(dialog);

    XtVaSetValues(shell,
		  XmNdeleteResponse, XmDO_NOTHING,
		  XmNnoResize, True,
		  XmNmwmFunctions, functions,
		  XmNmwmDecorations, decoration,
		  NULL);
    
    /* cinfo->new_color is a allocated/freed by request; initialize it with old_pixel */
    XAllocColor(DISP, G_colormap, &(cinfo->new_color));
    cinfo->new_color.flags = DoRed | DoGreen | DoBlue;

    XFreeColors(DISP, G_colormap, &(cinfo->new_color.pixel), 1, 0);
    pixel_to_color(old_pixel, &(cinfo->new_color), DISP, G_colormap);

    
    tip_shell = XtVaCreatePopupShell("tipShell", tipWidgetClass,
				     parent, XtNwidth, 1, XtNheight, 1,
				     NULL);
    cinfo->tooltip = tip_shell;
    
    form = XtVaCreateWidget("form", xmFormWidgetClass,
			    dialog,
			    XmNhorizontalSpacing, 10,
			    XmNverticalSpacing, 10,
			    XmNallowResize, False,
			    NULL);
    
    button_row = XtVaCreateManagedWidget("button_row", xmRowColumnWidgetClass,
					 form,
					 XmNtopAttachment, XmATTACH_FORM,
					 XmNleftAttachment, XmATTACH_FORM,
					 XmNorientation, XmHORIZONTAL,
					 XmNpacking, XmPACK_COLUMN,
					 XmNspacing, 8,
					 XmNmarginWidth, 1,
					 XmNmarginHeight, 1,
					 NULL);

    str = XmStringCreateLocalized("Old");
    old_sample = XtVaCreateManagedWidget(Xdvi_COLOR_DIALOG_OLD_SAMPLE_NAME, xmPushButtonWidgetClass,
					 button_row,
					 XmNbackground, old_pixel,
					 XmNlabelString, str,
					 XmNhighlightThickness, 0,
					 XmNshadowThickness, 1,
					 NULL);
    XtAddCallback(old_sample, XmNactivateCallback, revert_color_cb, cinfo);
    XmStringFree(str);
    str = XmStringCreateLocalized("New");
    new_sample = XtVaCreateManagedWidget("new_sample", xmPushButtonWidgetClass,
					 button_row,
					 XmNbackground, cinfo->new_color.pixel,
					 XmNlabelString, str,
					 XmNhighlightThickness, 0,
					 XmNshadowThickness, 1,
					 NULL);
    XmStringFree(str);
    /* disable clicking on the new_sample button */
    XtInsertEventHandler(new_sample,
			 KeyPressMask | KeyReleaseMask |
			 ButtonPressMask | ButtonReleaseMask,
			 True, block_event_callback,
			 (XtPointer)0, 0);
    cinfo->new_sample = new_sample;
    
    color_matrix = h_create_color_matrix(form, button_row, cinfo);

    h_create_sliders(form, color_matrix, cinfo);

    XtManageChild(form);
    
    XtAddCallback(dialog, XmNokCallback, popdown_apply_cb, prefs);
    XtAddCallback(dialog, XmNcancelCallback, popdown_cancel_cb, prefs);

    XtManageChild(dialog);

    add_to_deplist(prefs, dialog);

    TipAddWidget(tip_shell, old_sample, "Click to revert to old color");
    TipAddWidget(tip_shell, new_sample, "Preview of new color");
    /*  TipAddWidget(tip_shell, color_matrix, "Select color"); */

    WM_DELETE_WINDOW = XmInternAtom(DISP, "WM_DELETE_WINDOW", False);
    XmAddWMProtocolCallback(shell, WM_DELETE_WINDOW, popdown_cb, prefs);

    return dialog;
}

void
popup_color_dialog(Widget parent, struct color_button_info *button_info)
{
    static Widget shell;
    static Boolean first_time = True;
    /* Must be static so that we can pass its address around */
    static struct color_info cinfo;
    struct topic_info *tinfo = button_info->tinfo;
    struct prefs_choice *prefs = (struct prefs_choice *)tinfo->data;
    
    cinfo.button_info = button_info;
    
    if (first_time) {
	shell = h_create_shell(parent, &cinfo);
	first_time = False;
    }
    else {
	/* Already created, but we may need to change the colors of old_sample
	   and new_sample. This is done by changing the value of old_sample and
	   invoking its activate callback, which will `revert' new_sample to
	   the same color. */
	Widget w;
	if (get_widget_by_name(&w, shell, Xdvi_COLOR_DIALOG_OLD_SAMPLE_NAME, True)) {
#if XmVersion >= 1002
	    XmChangeColor(w, cinfo.button_info->pixel);
#else
	    XtVaSetValues(w, XmNbackground, cinfo.button_info->pixel, NULL);
#endif
	    XtCallCallbacks(w, XmNactivateCallback, &cinfo);
	}
	add_to_deplist(prefs, shell);
	XtManageChild(shell);
    }
}

#else
/* silence `empty compilation unit' warnings */
static void bar(void); static void foo(void) { bar(); } static void bar(void) { foo(); }
#endif /* MOTIF */    
