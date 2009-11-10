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
 * Preferences dialog for xdvik.
 */

#include "xdvi-config.h"
#include "xdvi.h"

#include "x_util.h"
#include "xm_colorsel.h"
#include "topic-window.h"
#include "message-window.h"
#include "util.h"
#include "events.h"

#include "xm_prefsP.h"
#include "xm_prefs.h"

#include "xm_prefs_appearance.h"
#include "xm_prefs_fonts.h"
#include "xm_prefs_helpers.h"
#include "xm_prefs_page.h"
#include "xm_prefs_scroll.h"

/* for reverting preferences, we need access to more prototypes ... */
#include "dvi-draw.h"
#include "search-internal.h"
#include "statusline.h"
#include "xm_toolbar.h"
#include "xm_menu.h"
#include "pagesel.h"

#ifdef MOTIF /* entire file */

#include <X11/Xatom.h>

#include <Xm/Xm.h>
#include <Xm/Protocols.h>
#include <Xm/DialogS.h>
#include <Xm/Form.h>

#define SCROLLING_DONE 0

#define NUM_PREFS_TOPICS 16 /* should be ample ... */

/* hmm, should maybe move these into xm_prefsP.c ... */
void
h_attach_below(Widget x, Widget y) {
    if (y == NULL) {
	/* no top widget, attach to form - could just pass NULL as XmNtopWidget, but lesstif warns in that case ... */
	XtVaSetValues(x,
		      XmNtopAttachment, XmATTACH_FORM,
		      XmNleftAttachment, XmATTACH_FORM,
		      XmNrightAttachment, XmATTACH_FORM,
		      NULL);
    }
    else {
	XtVaSetValues(x,
		      XmNtopAttachment, XmATTACH_WIDGET,
		      XmNtopWidget, y,
		      XmNleftAttachment, XmATTACH_FORM,
		      XmNrightAttachment, XmATTACH_FORM,
		      NULL);
    }
}


static void
initialize_items(struct topic_info *info)
{
    int i = 0;
    
    info->items[i].widget = prefs_appearance(info);
    info->items[i].topic = xstrdup("Appearance");
    info->items[i].title = xstrdup("Change the appearance of xdvi");

    i++;
    info->items[i].widget = prefs_fonts_and_colors(info);
    info->items[i].topic = xstrdup("Fonts and Colors");
    info->items[i].title = xstrdup("Display of fonts and colors in DVI files");

    i++;
    info->items[i].widget = prefs_paper(info);
    info->items[i].topic = xstrdup("Page Size");
    info->items[i].title = xstrdup("Customize the default window and page size");

#if SCROLLING_DONE /* not finished yet */
    i++;
    info->items[i].widget = prefs_scrolling(info);
    info->items[i].topic = xstrdup("Scrolling");
    info->items[i].title = xstrdup("Customize the scrolling behaviour when switching pages");
#endif

    i++;
    info->items[i].widget = prefs_helpers(info);
#ifdef LESSTIF_VERSION
    /* compensate for width computation bug by adding extra whitespace at end */
    info->items[i].topic = xstrdup("Helper Applications ");
#else
    info->items[i].topic = xstrdup("Helper Applications");
#endif
    info->items[i].title = xstrdup("External programs used by xdvi");

    /* terminate */
    i++;
    info->items[i].widget = 0;
    info->items[i].topic = info->items[i].title = NULL;

    ASSERT(i < NUM_PREFS_TOPICS, "NUM_PREFS_TOPICS too small!");
}

void
remove_from_deplist(struct prefs_choice *prefs, Widget w)
{
    size_t i;
    Boolean found = False;

    /* locate this widget */
    for (i = 0; i < prefs->depwin_cnt; i++) {
	if (prefs->depwin[i] == w) {
	    found = True;
	    break;
	}
    }
    
    if (found) {
	/* move later widgets down */
	for (; i < prefs->depwin_cnt - 1; i++) {
	    prefs->depwin[i] = prefs->depwin[i + 1];
	}
	prefs->depwin_cnt--;
    }

#if 0
    fprintf(stderr, "deplist after removal:\n");
    for (i = 0; i < prefs->depwin_cnt; i++) {
	fprintf(stderr, "%d: %p\n", i, prefs->depwin[i]);
    }
#endif    
}

static void
update_button(Widget button, Pixel pix)
{
    struct color_button_info *cinfo;
    static XmDrawnButtonCallbackStruct cbs;
    
    XtVaGetValues(button, XmNuserData, &cinfo, NULL);
    cinfo->pixel = pix;
    cbs.reason = XmCR_EXPOSE;
    XtCallCallbacks(button, XmNexposeCallback, &cbs);
}

static void
update_button_by_name(Pixel pix, const char *name)
{
    static Widget pref_shell = 0;
    Widget button;

    if (pref_shell == 0
	&& !get_widget_by_name(&pref_shell, globals.widgets.top_level, Xdvi_PREFS_DIALOG_NAME, True))
	return;
    
    if (get_widget_by_name(&button, pref_shell, name, True)) {
	update_button(button, pix);
    }
}

static Boolean
revert_colors(Pixel fg, Pixel bg, Pixel hl)
{
    XColor color_data[2];
    Boolean need_redraw = False;    
    Widget button;
    Pixel visited, unvisited;
    
    color_data[0].pixel = resource.fore_Pixel;
    color_data[1].pixel = resource.back_Pixel;
    
    str_to_pixel(globals.widgets.top_level, resource.visited_link_color, &visited);
    str_to_pixel(globals.widgets.top_level, resource.link_color, &unvisited);

    XQueryColors(DISP, G_colormap, color_data, 2);

    if (fg != resource.fore_Pixel) {
	XGCValues values;
	
	fg_initial.r = color_data[0].red;
	fg_initial.g = color_data[0].green;
	fg_initial.b = color_data[0].blue;

	values.foreground = resource.rule_pixel = resource.fore_Pixel;
	XChangeGC(DISP, globals.gc.ruler, GCForeground, &values);

	scanned_page = scanned_page_color = scanned_page_reset;

	update_button_by_name(resource.fore_Pixel, Xdvi_FG_COLOR_BTN);

	need_redraw = True;
    }
    if (bg != resource.back_Pixel) {
	bg_initial.r = color_data[1].red;
	bg_initial.g = color_data[1].green;
	bg_initial.b = color_data[1].blue;

	scanned_page = scanned_page_color = scanned_page_reset;

	update_button_by_name(resource.back_Pixel, Xdvi_BG_COLOR_BTN);

	need_redraw = True;
    }
    if (hl != resource.hl_Pixel) {
	XGCValues values;
	values.foreground = resource.hl_Pixel;
	XChangeGC(DISP, globals.gc.high, GCForeground, &values);
	/* hack to update match GC: fake change in inverted property, redraw
	   so that GC is cleared, then change inverted property back */
	resource.match_highlight_inverted = !resource.match_highlight_inverted;
	search_draw_inverted_regions();
	resource.match_highlight_inverted = !resource.match_highlight_inverted;

	update_button_by_name(resource.hl_Pixel, Xdvi_HL_COLOR_BTN);

	need_redraw = True;
    }
    /* NOTE: pixels for hyperlinks are not stored anywhere; just update always: */
    if (get_widget_by_name(&button, globals.widgets.top_level, Xdvi_VISITED_LINKS_BTN, True)) {
	update_button(button, visited);
	h_update_hyperlinks(button, visited); /* this already triggers a redraw */
    }
    if (get_widget_by_name(&button, globals.widgets.top_level, Xdvi_UNVISITED_LINKS_BTN, True)) {
	update_button(button, unvisited);
	h_update_hyperlinks(button, unvisited); /* this already triggers a redraw */
    }
    return need_redraw;
}

static void
revert_resources(void)
{
    Pixel curr_fg = resource.fore_Pixel;
    Pixel curr_bg = resource.back_Pixel;
    Pixel curr_hl = resource.hl_Pixel;
    Boolean need_redraw = False;
    
    /* save some old values */
    int save_shrink  = resource.shrinkfactor;
        
    load_app_resources(True);

    /* revert from saved values */
    resource.use_color = globals.curr_use_color;
    resource.gamma = globals.curr_gamma;
    resource.paper = globals.curr_paper;
    resource.shrinkfactor = save_shrink;
    do_set_shrinkfactor(resource.shrinkfactor, True);
    
    update_preferences_darkness();

    /* revert expert mode */
    if (resource.expert)
	resource.expert_mode = XPRT_SHOW_NONE;
    update_expert_mode();

    toggle_statusline();
#ifndef MOTIF
    if (!BROKEN_RECONFIG)
	toggle_scrollbars();
#else
    toggle_scrollbars();
#endif
    
#ifdef MOTIF
    toggle_pagelist();
    toggle_toolbar();
    toggle_menubar();
#else
    toggle_buttons();
#endif

    /* reset tooltips_wait_period, similar to TipAddWidget() in Tip.c */
    resource.tooltips_wait_period = resource.tooltips_wait_period_bak;
    if (resource.tooltips_wait_period < 0) {
	resource.show_tooltips = False;
    }
    else if (!resource.show_tooltips) {
	if (resource.tooltips_wait_period == 0)
	    resource.tooltips_wait_period = -1;
	else
	    resource.tooltips_wait_period = -resource.tooltips_wait_period;
    }
    
    update_preferences_expert();
    update_preferences_tooltips();
    update_preferences_search();
    
    update_preferences_color();
    update_preferences_hyperlinks();

    update_preferences_windowsize();
    update_preferences_shrink();
    update_preferences_paper();

    update_preferences_helpers();
    
    need_redraw |= revert_colors(curr_fg, curr_bg, curr_hl);

    /*     if (need_redraw) */
    /* just reload it always, there's too many exceptions ...
       redraw isn't always sufficient if file has colors. */
    globals.ev.flags |= EV_RELOAD;

}

/* add widget w to list of dependent windows */
void
add_to_deplist(struct prefs_choice *prefs, Widget w)
{
#if 0
    size_t i;
#endif    
    prefs->depwin = xrealloc(prefs->depwin,
			     (prefs->depwin_cnt + 1) * sizeof *(prefs->depwin));
    prefs->depwin[prefs->depwin_cnt] = w;
    prefs->depwin_cnt++;

#if 0
    fprintf(stderr, "deplist after adding:\n");
    for (i = 0; i < prefs->depwin_cnt; i++) {
	fprintf(stderr, "%lu: %p\n", (unsigned long)i, prefs->depwin[i]);
    }
#endif    
}

static void
reread_prefs_cb(Window w)
{
    if (w != XtWindow(globals.widgets.top_level)) {
	XChangeProperty(DISP, w,
			atom_reread_prefs(), atom_reread_prefs(), 8,
			PropModeReplace,
			/* dummy values, since all the other instance needs to do is
			   reread the ~/.xdvirc.tmp file */
			(unsigned char *)"Y", 1);
    }
}

static void
apply_prefs_cb(XtPointer arg)
{
    struct topic_info *info = (struct topic_info *)arg;
    struct prefs_choice *prefs = (struct prefs_choice *)info->data;
    size_t i;
    Widget colorsel;

    if (get_widget_by_name(&colorsel, globals.widgets.top_level, Xdvi_COLOR_DIALOG_NAME, False)) {
	XtPopdown(XtParent(colorsel));
    }

    /* pop down dependent windows */
    TRACE_GUI((stderr, "window count: %lu\n", (unsigned long)prefs->depwin_cnt));
    for (i = 0; i < prefs->depwin_cnt; i++) {
	TRACE_GUI((stderr, "popping down %lu: %p", (unsigned long)i, (void *)(prefs->depwin[i])));
	if (XtIsRealized(prefs->depwin[i])) {
	    XtCallCallbacks(prefs->depwin[i], XmNcancelCallback, NULL);
	    XSync(DISP, True); /* wait for server to catch up */
	    if (XtIsRealized(prefs->depwin[i])) {
		TRACE_GUI((stderr, "calling XmNokCallback of %lu: %p", (unsigned long)i, (void *)(prefs->depwin[i])));
		XtCallCallbacks(prefs->depwin[i], XmNokCallback, NULL);
	    }
	}
    }
    free(prefs->depwin);
    prefs->depwin = NULL;
    prefs->depwin_cnt = 0;

    if (prefs->db == NULL) /* callback invoked multiple times? */
	return;

    merge_into_user_db(prefs->db); /* this destroys prefs->db */
    prefs->db = NULL;

    /* remember some current values */
    free(globals.curr_paper);
    if (resource.paper != NULL)
	globals.curr_paper = xstrdup(resource.paper);
    
    free(globals.curr_editor);
    if (resource.editor != NULL)
	globals.curr_editor = xstrdup(resource.editor);
    
    free(globals.curr_browser);
    if (resource.browser != NULL)
	globals.curr_browser = xstrdup(resource.browser);

    /*     fprintf(stderr, "set curr_browser to: |%s|\n", globals.curr_browser); */
    /*     fprintf(stderr, "set curr_editor to: |%s|\n", globals.curr_editor); */
    
    if (get_xdvi_window_id(False, NULL) && save_user_preferences(False)) {
	/* if other instances of xdvi are running, make them reread the
	   changed preferences by writing them to ~/.xdvirc.tmp and having
	   them read that file; otherwise they would overwrite the file if
	   user quits them after the current instance.
	*/
	get_xdvi_window_id(False, reread_prefs_cb);
    }
}

static void
revert_prefs_cb(XtPointer arg)
{
    struct topic_info *info = (struct topic_info *)arg;
    struct prefs_choice *prefs = (struct prefs_choice *)info->data;
    size_t i;

    /* pop down dependent windows */
    TRACE_GUI((stderr, "window count: %lu", (unsigned long)prefs->depwin_cnt));
    for (i = 0; i < prefs->depwin_cnt; i++) {
	TRACE_GUI((stderr, "popping down %lu: %p", (unsigned long)i, (void *)(prefs->depwin[i])));
	if (XtIsRealized(prefs->depwin[i])) {
	    XtCallCallbacks(prefs->depwin[i], XmNcancelCallback, NULL);
	    XSync(DISP, True); /* wait for server to catch up */
	    if (XtIsRealized(prefs->depwin[i])) {
		TRACE_GUI((stderr, "calling XmNokCallback of %lu: %p", (unsigned long)i, (void *)(prefs->depwin[i])));
		XtCallCallbacks(prefs->depwin[i], XmNokCallback, NULL);
	    }
	}
    }
    free(prefs->depwin);
    prefs->depwin = NULL;
    prefs->depwin_cnt = 0;

    if (prefs->db == NULL) { /* callback invoked multiple times, or prefs not changed */
	return;
    }

    revert_resources();
    
    XrmDestroyDatabase(prefs->db);
    prefs->db = NULL;
}

static void
save_prefs_exit(XtPointer arg)
{
    struct topic_info *info = (struct topic_info *)arg;
    apply_prefs_cb(info);
    XtPopdown(info->shell);
    XSync(DISP, False);
    xdvi_exit(EXIT_SUCCESS);
}

static void
no_save_prefs_exit(XtPointer arg)
{
    struct topic_info *info = (struct topic_info *)arg;
    revert_prefs_cb(info);
    XtPopdown(info->shell);
    XSync(DISP, False);
    xdvi_exit(EXIT_SUCCESS);
}

static void
close_prefs_exit(Widget w, XtPointer arg)
{
    struct topic_info *info = (struct topic_info *)arg;
    struct prefs_choice *prefs = (struct prefs_choice *)info->data;
    /*      Widget dialog; */
    /*      if (get_widget_by_name(&dialog, w, Xdvi_MESSAGE_DIALOG_NAME, True)) { */
    /*  	fprintf(stderr, "!!!!!!!!! removing window from deplist!\n"); */
    /*  	remove_from_deplist(prefs, dialog); */
    /*      } */

    remove_from_deplist(prefs, w);
    
}

Boolean
preferences_changed(void)
{
    Widget prefs_shell = 0, topic_pane = 0;
    
    if (get_widget_by_name(&prefs_shell, globals.widgets.top_level, "preferences_window", False)
	&& get_widget_by_name(&topic_pane, prefs_shell, "topic_pane", False)) {
	struct topic_info *info = NULL;
	struct prefs_choice *prefs = NULL;

	XtVaGetValues(topic_pane, XmNuserData, &info, NULL);
	if (info == NULL) {
	    return False;
	}
	prefs = (struct prefs_choice *)info->data;
	if (prefs->db != NULL) { /* prefs changed */

	    Widget popup = choice3_dialog(prefs_shell,
					  MSG_QUESTION, NULL,
#ifndef MOTIF				 
					  NULL,
#endif
					  close_prefs_exit, info, /* pre_callbacks */
					  "Save and Exit", save_prefs_exit, info,
					  "Exit, don't Save", no_save_prefs_exit, info,
					  "Cancel", NULL, NULL,
					  "Preferences have been changed, but not saved yet. "
					  "Save them now?");
	    add_to_deplist(prefs, popup);
	    return True;	    
	}
    }

    return False;
}

void
popup_preferences_dialog(Widget parent, int arg)
{
    static Widget preferences_shell = 0;
    static struct topic_info info;
    static struct topic_item items[NUM_PREFS_TOPICS];
    static struct prefs_choice *prefs = NULL;

    if (preferences_shell == 0) { /* called 1st time; create widget */
	info.ok_callback = apply_prefs_cb;
	info.cancel_callback = revert_prefs_cb;
	info.items = items;
	/* 	info.items_size = NUM_PREFS_TOPICS; */

	prefs = xmalloc(sizeof *prefs);
	prefs->depwin_cnt = 0;
	prefs->depwin = NULL;
	/* 	prefs->orig = orig_prefs; */
	/* apply_prefs_cb/revert_prefs_cb are responsible for copying
	   the changed preferences into the current preferences as
	   appropriate, and free()ing prefs.changed */
	/* 	prefs->changed = xmalloc(sizeof *(prefs->changed)); */
	/* 	copy_resources(orig_prefs, prefs->changed); */
	prefs->db = NULL; /*  XrmGetStringDatabase(""); */
	info.data = prefs;
	
	preferences_shell = create_topic_window(parent,
						"xdvik: Preferences",
						Xdvi_PREFS_DIALOG_NAME,
						&info,
						initialize_items,
						"OK", "Cancel");
	info.shell = preferences_shell;
	center_window(preferences_shell, parent);
	select_topic(&info, 0);
    }

    if (arg >= 0)
	select_topic(&info, arg);
    
    XtPopup(preferences_shell, XtGrabNone);

    if (resource.no_init_file) {
	popup_message(preferences_shell,
		      MSG_WARN,
		      NULL,
		      "You specified the resource `noInitFile' or the `-q' command-line option. "
		      "Any preferences that you set in this dialog will be lost when you exit xdvi.");
    }
}

#else
/* silence `empty compilation unit' warnings */
static void bar(void); static void foo(void) { bar(); } static void bar(void) { foo(); }
#endif /* MOTIF */
