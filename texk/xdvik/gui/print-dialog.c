/*
 * Copyright (c) 2002-2004 Paul Vojta and the xdvik development team
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
  SU: Adapted from non-k xdvi's popups.c and added saving functionality.
*/

#include "xdvi-config.h"
#include <string.h>
#include "xdvi.h"
#include "my-vsnprintf.h"
#include <ctype.h>

#include "print-dialog.h"
#include "print-internal.h"
#include "print-log.h"
#include "print-internal.h"
#include "events.h"
#include "dvi-init.h"
#include "string-utils.h"
#include "util.h"
#include "x_util.h"
#include "message-window.h"
#include "pagesel.h"
#include "my-snprintf.h"
#include "sfSelFile.h"
#include "dvisel.h" /* for select_marked_pages() */
#include "xlwradio.h"
#include "statusline.h"
#include "search-dialog.h"
#include "search-internal.h"
#include "special.h"

/* Xlib and Xutil are already included */


#include <X11/Xatom.h>
#include <X11/StringDefs.h>

#ifdef MOTIF
# include <Xm/BulletinB.h>
# include <Xm/DialogS.h>
# include <Xm/PanedW.h>
# include <Xm/MessageB.h>
# include <Xm/LabelG.h>
# include <Xm/Form.h>
# include <Xm/Frame.h>
# include <Xm/ToggleBG.h>
# include <Xm/Text.h>
# include <Xm/TextF.h>
# include <Xm/PushB.h>
# include <Xm/Protocols.h>
# include <Xm/AtomMgr.h>
#define XTranslations XmNtranslations
#else /* MOTIF */
# include <X11/Shell.h>
# include <X11/Xaw/Paned.h>
# include <X11/Xaw/Box.h>
# include <X11/Xaw/Form.h>
# include <X11/Xaw/Label.h>
# include <X11/Xaw/Command.h>
# include <X11/Xaw/Toggle.h>
# include <X11/Xaw/AsciiText.h>
# include <X11/Xaw/SimpleMenu.h>
# include <X11/Xaw/MenuButton.h>
# include <X11/Xaw/Sme.h>
# include <X11/Xaw/SmeBSB.h>
#if 0
#include "xaw_bitmaps.h"
static Pixmap menu_double_arrow_bitmap;
#endif
#define XTranslations XtNtranslations
#endif /* MOTIF */

#if HAVE_XKB_BELL_EXT
# include <X11/XKBlib.h>
# define XBell(dpy, percent) XkbBell(dpy, mane.win, percent, (Atom) None)
#endif

struct output_format_mapping {
    const char *fmt_string;
    outputFormatT fmt;
    const char *extension;
} output_format_mapping[] = {
    { "Postscript",		FMT_PS ,	".ps"	},
    { "PDF",			FMT_PS2PDF,	".pdf"	},
    { "DVI",			FMT_DVI,	".dvi"	},
    { "Plain Text (ISO_8859-1)",FMT_ISO_8859_1,	".txt"	},
    { "Plain Text (UTF-8)",	FMT_UTF8,	".txt"	},
    { NULL,			FMT_NONE,	NULL	}
};

/* Widget names that are used in callbacks ... */
#define Xdvi_SAVE_SHELL_NAME	"save_popup"
#define Xdvi_PRINT_SHELL_NAME	"print_popup"

#define Xdvi_PAGE_RANGE_FORM_NAME "page_range_form"

#define Xdvi_PAGES_ALL_RADIO_NAME "pages_all"
#define Xdvi_PAGES_MARKED_RADIO_NAME "pages_marked"
#define Xdvi_PAGES_RANGE_RADIO_NAME "pages_range"

#define Xdvi_PAGES_RANGE_FROM_LABEL_NAME "range_from_lab"
#define Xdvi_PAGES_RANGE_FROM_TEXT_NAME "range_from_text"
#define Xdvi_PAGES_RANGE_TO_LABEL_NAME "range_to_lab"
#define Xdvi_PAGES_RANGE_TO_TEXT_NAME "range_to_text"
#define Xdvi_PAGES_RANGE_OF_LABEL_NAME "range_of_lab"

#define Xdvi_TO_PRINTER_NAME "to_printer"
#define Xdvi_TO_PRINTER_TEXT "printer_text"
#define Xdvi_TO_FILE_NAME "to_file"
#define Xdvi_TO_FILE_TEXT "file_text"
#define Xdvi_BROWSE_BUTTON "file_button"

#define Xdvi_DVIPS_OPTIONS_NAME "dvips_options_name"
#define Xdvi_DVIPS_OPTIONS_TEXT "dvips_options_text"
#define Xdvi_FORMAT_SELECTION_BUTTON_NAME "format_selection_button"

/* number of items in `format' pulldown menu */
#define FORMAT_SEL_CNT 5

static void print_check_page_values(struct save_or_print_info *info);
static void print_check_dvi_file(struct save_or_print_info *info);
static void print_check_target_file(XtPointer info);

#ifndef MOTIF
static void xaw_print_save_act_go(Widget, XEvent *, String *, Cardinal *);
#endif /* not MOTIF */

static void wm_cancel(Widget, XEvent *, String *, Cardinal *);

static XtActionsRec print_actions[] = {
#ifndef MOTIF
    {"xaw_print_or_save",	xaw_print_save_act_go },
#endif
    {"WM_cancel",		wm_cancel },
};

/*
  ============================================================
  generic utility functions
  ============================================================
*/


/* return basename of `old_name' with `old_ext' replaced by `new_ext', in fresh memory */
static char *
get_new_file_name(const char *old_name, const char *old_ext, const char *new_ext)
{
    /* old_name contains the normalized DVI file name, with path and extension */
    char *basename, *new_name;
    size_t len;

    basename = strrchr(old_name, DIR_SEPARATOR);
		       
    if (basename != NULL) {
	basename++;
	new_name = xmalloc(strlen(basename) + strlen(new_ext) + 1);
	new_name = strcpy(new_name, basename);
    }
    else {
	new_name = xmalloc(strlen(old_name) + strlen(new_ext) + 1);
	new_name = strcpy(new_name, old_name);
    }

    len = strlen(new_name);
    if (old_ext == NULL) {
	strcpy(new_name + len, new_ext);
    }
    else {
	strcpy(new_name + len - strlen(old_ext), new_ext);
    }
    return new_name;
}

/*
 * Translate `non-standard' paper sizes for xdvi into options suitable
 * for dvips. We don't do this always, since it would override the papersize
 * specials inserted by e.g.
 * \usepackage[dvips]{geometry}
 * which is the preferred way to specify the papersize. If the papersize
 * has been explicitly set by such a special, this function returns NULL.
 * Else it returns the value of the `paper' resource, without the trailing
 * `r', and sets the `landscape' argument to True if a trailing `r' was
 * present.
 *
 * Note that we don't check/warn about whether the current paper
 * resource is valid for dvips; dvips will ignore invalid paper sizes.
 */
static char *
get_dvips_papersize(Boolean *landscape)
{
    char *papersize;
    if (have_papersize_special())
	return NULL;
    
    papersize = xstrdup(resource.paper);
    /* fprintf(stderr, "PAPERSIZE: |%s|\n", papersize); */
    *landscape = False;
    if (papersize[strlen(papersize) - 1] == 'r') { /* landscape size */
	papersize[strlen(papersize) - 1] = '\0';
	*landscape = True;
    }
    TRACE_GUI((stderr, "Using dvips arguments: `%s', %s", papersize, *landscape ? "landscape" : "portrait"));
    return papersize;
}

static void
update_dvips_options_sensitivity(int idx, Widget top)
{
    Widget dvips_label;

#if MOTIF    
    Widget dvips_options;

    if (get_widget_by_name(&dvips_label, top, Xdvi_DVIPS_OPTIONS_NAME, True) &&
	get_widget_by_name(&dvips_options, top, Xdvi_DVIPS_OPTIONS_TEXT, True)) {
	if (output_format_mapping[idx].fmt == FMT_PS ||
	    output_format_mapping[idx].fmt == FMT_PS2PDF) {
	    XtSetSensitive(dvips_label, True);
	    XtSetSensitive(dvips_options, True);
	}
	else {
	    XtSetSensitive(dvips_label, False);
	    XtSetSensitive(dvips_options, False);
	}
    }
#else
    if (get_widget_by_name(&dvips_label, top, Xdvi_DVIPS_OPTIONS_NAME, True)) {
	XtSetSensitive(XtParent(dvips_label),
		       output_format_mapping[idx].fmt == FMT_PS ||
		       output_format_mapping[idx].fmt == FMT_PS2PDF);
    }
#endif
}

/* callbacks */

static void
cb_select_format(Widget w, XtPointer client_data, XtPointer call_data)
{
    struct save_or_print_info *info = NULL;
    Widget file_text;
    char *filename;
    ptrdiff_t i;

#ifndef MOTIF
    Widget button;
    char *old_label, *new_label;
    char tmp_label[1024];
#endif
    
    UNUSED(call_data);

#ifdef MOTIF
    i = (ptrdiff_t) client_data;
    XtVaGetValues(XtParent(w), XmNuserData, &info, NULL);
    ASSERT(info != NULL, "Expected struct save_or_print_info * in XmNuserData of button!");
    
    if (get_widget_by_name(&file_text, info->shell, Xdvi_TO_FILE_TEXT, True)) {
	XtVaGetValues(file_text, XmNvalue, &filename, NULL);
	if (strrchr(filename, '.') != NULL) {
	    char buf[1024];
	    replace_extension(filename, output_format_mapping[i].extension, buf, sizeof buf);
	    XtVaSetValues(file_text, XmNvalue, buf, XmNcursorPosition, strlen(buf), NULL);
	}
    }
#else /* MOTIF */
    info = (struct save_or_print_info *)client_data;

    i = 0;
    if (get_widget_by_name(&file_text, info->shell, Xdvi_TO_FILE_TEXT, True)
	&& get_widget_by_name(&button, info->shell, Xdvi_FORMAT_SELECTION_BUTTON_NAME, True)) {

	XtVaGetValues(file_text, XtNstring, &filename, NULL);

	/* swap title strings */
	XtVaGetValues(button, XtNlabel, &old_label, NULL);
	XtVaGetValues(w, XtNlabel, &new_label, NULL);
	strncpy(tmp_label, old_label, sizeof tmp_label);

	/* update the filename with new format */
	for (i = 0; output_format_mapping[i].fmt_string != NULL; i++) {
	    if (strcmp(new_label, output_format_mapping[i].fmt_string) == 0) {
		char buf[1024];
		replace_extension(filename, output_format_mapping[i].extension, buf, sizeof buf);
		XtVaSetValues(file_text, XtNstring, buf, NULL);
		XawTextSetInsertionPoint(file_text, strlen(buf));
	    
		break;
	    }
	}
	/* format not found, complain: */
	if (output_format_mapping[i].fmt_string == NULL) {
	    popup_message(info->shell,
			  MSG_WARN,
			  REPORT_XDVI_BUG_TEMPLATE,
			  "Unrecognized string in format selector: |%s|", new_label);
	}
	XtVaSetValues(button, XtNlabel, new_label, NULL);
	XtVaSetValues(w, XtNlabel, tmp_label, NULL);
    }
#endif /* MOTIF */

    update_dvips_options_sensitivity(i, info->shell);

    info->fmt = resource.default_saving_format = output_format_mapping[i].fmt;
}

static void
cb_popdown(Widget w, XtPointer client_data, XtPointer call_data)
{
    struct save_or_print_info *info = (struct save_or_print_info *)client_data;
    UNUSED(w);
    UNUSED(call_data);
    
    ASSERT(info != NULL, "client_data in cb_popdown musn't be NULL!");
    if (info->message_popup != 0) {
	kill_message_window(info->message_popup);
    }
    /* fprintf(stderr, "Popping down shell: %p\n", (void *)info->shell); */
    XtPopdown(info->shell);
}

static void
cb_print_or_save(Widget w, XtPointer client_data, XtPointer call_data)
{
    struct save_or_print_info *info = (struct save_or_print_info *)client_data;
    UNUSED(w);
    UNUSED(call_data);

    print_check_page_values(info);
}

/* access from outside the module */
void
cancel_saving(struct save_or_print_info *info)
{
    cb_popdown(NULL, info, NULL);
}


static void
wm_cancel(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    struct save_or_print_info *info = NULL;
    void *ptr;
    
    UNUSED(w);
    UNUSED(event);

    ASSERT(*num_params > 0, "params in wm_cancel must be > 0!");
    ASSERT(*params != NULL, "params in wm_cancel mustn't be NULL!");

    TRACE_GUI((stderr, "Pointer string value: |%s|", *params));
    sscanf(*params, "%p", &ptr);
    info = (struct save_or_print_info *)ptr;

    cb_popdown(NULL, info, NULL);
}
    
/*
 * Callbacks to enable the corresponding radio button if the user clicks on a text field to
 * edit it. This is more user-friendly than making the text field insensitive
 * until the radio button has been clicked. There are 2 separate callbacks, one for
 * printing vs. saving (only used when actually printing), and one for the page range.
 */
static void
text_print_vs_save_callback(Widget widget, XtPointer closure, XEvent *ev, Boolean *cont)
{
    struct save_or_print_info *info = (struct save_or_print_info *)closure;
    Widget to_printer; /* used to check where we have been called from */
    Widget radio_printer, radio_file;

    UNUSED(ev);
    UNUSED(cont);

    if (get_widget_by_name(&to_printer, info->shell, Xdvi_TO_PRINTER_TEXT, True)
	&& get_widget_by_name(&radio_printer, info->shell, Xdvi_TO_PRINTER_NAME, True)
	&& get_widget_by_name(&radio_file, info->shell, Xdvi_TO_FILE_NAME, True)) {
#ifdef MOTIF
	XmToggleButtonGadgetSetState(radio_printer, widget == to_printer, True);
	XmToggleButtonGadgetSetState(radio_file, widget != to_printer, True);
#else
	XawToggleSetCurrent(radio_printer, widget == to_printer ? cast_int_to_XtPointer(TO_PRINTER) : cast_int_to_XtPointer(TO_FILE));
#endif
    }
}

/* This case is slightly simpler, since the callback only needs to enable
 * the radio button for the page range, never disable it.
 */
static void
text_page_range_callback(Widget widget, XtPointer closure, XEvent *ev, Boolean *cont)
{
    struct save_or_print_info *info = (struct save_or_print_info *)closure;
    Widget radio_all, radio_selected, radio_range;

    UNUSED(ev);
    UNUSED(cont);
    UNUSED(widget);

    if (get_widget_by_name(&radio_all, info->shell, Xdvi_PAGES_ALL_RADIO_NAME, True)
	&& get_widget_by_name(&radio_selected, info->shell, Xdvi_PAGES_MARKED_RADIO_NAME, True)
	&& get_widget_by_name(&radio_range, info->shell, Xdvi_PAGES_RANGE_RADIO_NAME, True)) {
#ifdef MOTIF
	XmToggleButtonGadgetSetState(radio_all, False, True);
	XmToggleButtonGadgetSetState(radio_selected, False, True);
	XmToggleButtonGadgetSetState(radio_range, True, True);
#else
	XawToggleSetCurrent(radio_all, cast_int_to_XtPointer(PAGE_RANGE));
#endif
    }
}

#ifdef MOTIF

/* Motif type callbacks for the above. */
static void
cb_text_print_vs_save(Widget w, XtPointer client_data, XtPointer call_data)
{
    UNUSED(call_data);
    /* fprintf(stderr, "************ cb_text_print_vs_save!\n"); */
    text_print_vs_save_callback(w, client_data, NULL, NULL);
}

static void
cb_text_page_range(Widget w, XtPointer client_data, XtPointer call_data)
{
    UNUSED(call_data);
    text_page_range_callback(w, client_data, NULL, NULL);
}

#else /* MOTIF */

static void
xaw_print_save_act_go(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    struct save_or_print_info *info = NULL;
    void *ptr;
    
    UNUSED(w);
    UNUSED(event);

    ASSERT(*num_params > 0, "params in xaw_print_save_act_go must be > 0!");
    ASSERT(*params != NULL, "params in xaw_print_save_act_go mustn't be NULL!");

    TRACE_GUI((stderr, "Pointer string value: |%s|", *params));
    sscanf(*params, "%p", &ptr);
    info = (struct save_or_print_info *)ptr;

    print_check_page_values(info);
}

#endif /* MOTIF */

static void
cb_print_vs_save(Widget w, XtPointer client_data, XtPointer call_data)
{
    struct save_or_print_info *info = (struct save_or_print_info *)client_data;
    Widget radio_printer, radio_file;
    /*      Widget file_button; */

    UNUSED(call_data);

    if (get_widget_by_name(&radio_printer, info->shell, Xdvi_TO_PRINTER_NAME, True)
	&& get_widget_by_name(&radio_file, info->shell, Xdvi_TO_FILE_NAME, True)) {
#ifdef MOTIF
	Widget text;
	
	if (radio_printer == w) {
	    XmToggleButtonGadgetSetState(radio_printer, True, False);
	    XmToggleButtonGadgetSetState(radio_file, False, False);
	    if (get_widget_by_name(&text, info->shell, Xdvi_TO_PRINTER_TEXT, True))
		XmProcessTraversal(text, XmTRAVERSE_CURRENT);
	}
	else {
	    XmToggleButtonGadgetSetState(radio_printer, False, False);
	    XmToggleButtonGadgetSetState(radio_file, True, False);
	    if (get_widget_by_name(&text, info->shell, Xdvi_TO_FILE_TEXT, True))
		XmProcessTraversal(text, XmTRAVERSE_CURRENT);
	}
#endif
	if (radio_printer == w)
	    info->print_target = TO_PRINTER;
	else
	    info->print_target = TO_FILE;
    }
}

static void
cb_range(Widget w, XtPointer client_data, XtPointer call_data)
{
    struct save_or_print_info *info = (struct save_or_print_info *)client_data;
    Widget radio_all, radio_selected, radio_range;
    
    UNUSED(call_data);
#ifndef MOTIF
    UNUSED(w);
#endif

    if (get_widget_by_name(&radio_all, info->shell, Xdvi_PAGES_ALL_RADIO_NAME, True)
	&& get_widget_by_name(&radio_selected, info->shell, Xdvi_PAGES_MARKED_RADIO_NAME, True)
	&& get_widget_by_name(&radio_range, info->shell, Xdvi_PAGES_RANGE_RADIO_NAME, True)) {

#ifdef MOTIF
	XmToggleButtonGadgetSetState(radio_all, radio_all == w, False);
	XmToggleButtonGadgetSetState(radio_selected, radio_selected == w, False);
	XmToggleButtonGadgetSetState(radio_range, radio_range == w, False);
#endif
    }
    
    /*      update_page_range_sensitivity(info->shell, radio_range == w); */
}

static void
set_filename_callback(const char *fname, void *data)
{
    /* fprintf(stderr, "Filename: |%s|; widget: %p\n", fname, data); */
    if (fname != NULL) {
	Widget w = (Widget)data;
#ifdef MOTIF
	XtVaSetValues(w, XmNvalue, fname, XmNcursorPosition, strlen(fname), NULL);
#else
	XtVaSetValues(w, XtNstring, fname, NULL);
	XawTextSetInsertionPoint(w, strlen(fname));
#endif
    }
}


static void
cb_browse(Widget w, XtPointer client_data, XtPointer call_data)
{
    struct save_or_print_info *info = (struct save_or_print_info *)client_data;
    printOrSaveActionT act = info->act;
    Widget file_text;
    /* static so that we can pass its address */
    static struct filesel_callback cb[2] = {
	{ NULL, NULL,
	  "xdvik: Save to file", "Save to file:", "OK", "Cancel",
	  NULL, NULL, False, False, NULL, NULL },
	{ NULL, NULL,
	  "xdvik: Print to file", "Print to file:", "OK", "Cancel",
	  NULL, "*.ps", False, False, NULL, NULL }
    };
    
    UNUSED(call_data);

    /* switch on `to file' radio button */
    if (act == FILE_PRINT)
	text_print_vs_save_callback(w, (XtPointer)info, NULL, NULL);

    if (!get_widget_by_name(&file_text, info->shell, Xdvi_TO_FILE_TEXT, True))
	return;
    
    if (act == FILE_SAVE) {
	if (resource.default_saving_format == FMT_PS)
	    cb[0].filemask = "*.ps";
	else if (resource.default_saving_format == FMT_PS2PDF)
	    cb[0].filemask = "*.pdf";
	else if (resource.default_saving_format == FMT_DVI)
	    cb[0].filemask = "*.dvi";
	else
	    cb[0].filemask = "*.txt";
	cb[0].func_ptr = set_filename_callback;
	cb[0].data = file_text;
	cb[0].browse_fname = xt_strdup(globals.cwd);

	/* fprintf(stderr, "==== text widget save: %p; shell: %p\n", (void *)file_text, (void *)info->shell); */
	if (cb[0].shell == NULL)
	    cb[0].shell = XsraSelFile(info->shell, &cb[0]);
	XsraSelFilePopup(&cb[0]);
	/* 	fprintf(stderr, "done saving: return widget = %p\n", ret_widget); */
    }
    else {
	cb[1].func_ptr = set_filename_callback;
	cb[1].data = file_text;
	cb[1].init_path = globals.cwd;
	
	/* fprintf(stderr, "==== text widget print: %p; shell: %p\n", (void *)file_text, (void *)info->shell); */
	/*  	if (!get_widget_by_name(&parent, globals.widgets.top_level, Xdvi_PRINT_SHELL_NAME, True)) */
	/*  	    parent = globals.widgets.top_level; */
	if (cb[1].shell == NULL)
	    cb[1].shell = XsraSelFile(info->shell, &cb[1]);
	XsraSelFilePopup(&cb[1]);
    }
}

/* create a dialog for printing OR saving */
#ifdef MOTIF

static void
motif_create_dialog(struct save_or_print_info *info)
{
    printOrSaveActionT act = info->act;
    Atom WM_DELETE_WINDOW;
    char *ptr;
    char ofstring[1024];
    
    Widget form, pane, box;
    Widget print_or_save_frame, destination_form;
    Widget dvips_options_label;
    Widget pages_frame, pages_form;
    Widget print_to_file_text, to_file_radio_or_label;
    Widget dvips_options_text;
    Widget print_to_file_button;
    Widget range_marked_radio, range_from_to_radio, to_printer_radio = NULL,
	print_to_printer_text = NULL, format_selection_option = NULL,
	print_or_save_range_all_radio,
	page_from_text, to_label, page_to_text, of_label, ok_button, cancel_button;
    Widget from_label;
    XmString str;
    XmString format_selection_texts[FORMAT_SEL_CNT];
    XtTranslations xlats;

    XtAddActions(print_actions, XtNumber(print_actions));

    ptr = get_string_va("#override \n<Key>osfCancel:WM_cancel(%p)", info);
    xlats = XtParseTranslationTable(ptr);
    free(ptr);

    if (globals.pageno_correct == 1)
	sprintf(ofstring, "of %d physical pages", total_pages);
    else
	sprintf(ofstring, "of %d to %d pages", globals.pageno_correct, total_pages + globals.pageno_correct - 1);
    
    info->shell = XtVaCreatePopupShell(act == FILE_SAVE ? "save_popup" : "print_popup",
				       xmDialogShellWidgetClass, globals.widgets.top_level,
				       XmNtitle, act == FILE_SAVE ? "xdvik: Save File" : "xdvik: Print File",
				       XmNallowShellResize, True,
				       XmNdeleteResponse, XmDO_NOTHING, /* we'll take care of that ourselves */
				       XmNmappedWhenManaged, False, /* so that we can center it first */
				       NULL);

    /* make the window manager destroy action just pop down the dialog */
    WM_DELETE_WINDOW = XmInternAtom(XtDisplay(info->shell), "WM_DELETE_WINDOW", False);

    XmAddWMProtocolCallback(info->shell, WM_DELETE_WINDOW, cb_popdown, (XtPointer)info);
	

    pane = XtVaCreateWidget(act == FILE_SAVE ? "save_paned" : "print_paned",
			    xmPanedWindowWidgetClass, info->shell,
			    /* make sashes invisible */
			    XmNsashWidth, 1,
			    XmNsashHeight, 1,
			    /* turn separator off, since it gives visual noise with the frames */
			    XmNseparatorOn, False,
			    NULL);
    
    
    form = XtVaCreateWidget("form", xmFormWidgetClass,
			    pane,
			    XmNhorizontalSpacing, DDIST_MAJOR,
			    XmNverticalSpacing, DDIST_MAJOR,
			    XmNautoUnmanage, False,
			    XTranslations, xlats,
			    NULL);

    if (act == FILE_PRINT) {
	/*  First frame:  print to printer or file */

	print_or_save_frame = XtVaCreateWidget("print_to_frame", xmFrameWidgetClass, form,
					       XmNmarginWidth, DDIST,
					       XmNmarginHeight, DDIST,
					       XmNtopAttachment, XmATTACH_FORM,
					       XmNleftAttachment, XmATTACH_FORM,
					       XmNrightAttachment, XmATTACH_FORM,
					       XTranslations, xlats,
					       NULL);
	
	str = XmStringCreateLocalized("Print to:");
	XtVaCreateManagedWidget("title", xmLabelGadgetClass,
				print_or_save_frame,
				XmNchildType, XmFRAME_TITLE_CHILD,
				XmNlabelString, str,
				NULL);
	XmStringFree(str);
	
	destination_form = XtVaCreateWidget("destination_form", xmFormWidgetClass, print_or_save_frame,
					    XmNhorizontalSpacing, DDIST,
					    XmNverticalSpacing, DDIST,
					    XmNtopAttachment, XmATTACH_FORM,
					    XmNleftAttachment, XmATTACH_FORM,
					    XmNrightAttachment, XmATTACH_FORM,
					    XTranslations, xlats,
					    NULL);
	
	str = XmStringCreateLocalized("Printer:");
	to_printer_radio = XtVaCreateManagedWidget(Xdvi_TO_PRINTER_NAME, xmToggleButtonGadgetClass, destination_form,
						   XmNlabelString, str,
						   XmNindicatorType, XmONE_OF_MANY,
						   XmNset, resource.default_printing_target == TO_PRINTER,
						   XmNtopAttachment, XmATTACH_FORM,
						   XmNtopOffset, 0,
						   XmNleftAttachment, XmATTACH_FORM,
						   XmNleftOffset, 0,
						   NULL);
	XmStringFree(str);
	XtAddCallback(to_printer_radio, XmNvalueChangedCallback, cb_print_vs_save, (XtPointer)info);
	
	XtOverrideTranslations(to_printer_radio, xlats);
	
	print_to_printer_text = XtVaCreateManagedWidget(Xdvi_TO_PRINTER_TEXT, xmTextFieldWidgetClass, destination_form,
							XmNtopAttachment, XmATTACH_FORM,
							XmNtopOffset, 0,
							XmNleftAttachment, XmATTACH_WIDGET,
							XmNrightAttachment, XmATTACH_FORM,
							XmNleftWidget, to_printer_radio,
							/*  XmNsensitive, resource.default_printing_target == TO_PRINTER, */
							XTranslations, xlats,
							NULL);
	XtAddCallback(print_to_printer_text, XmNactivateCallback, cb_print_or_save, (XtPointer)info);
	XtAddCallback(print_to_printer_text, XmNfocusCallback, cb_text_print_vs_save, (XtPointer)info);

	adjust_heights(to_printer_radio, print_to_printer_text, NULL);

	str = XmStringCreateLocalized("PS File:");
	to_file_radio_or_label = XtVaCreateManagedWidget(Xdvi_TO_FILE_NAME, xmToggleButtonGadgetClass, destination_form,
							 XmNlabelString, str,
							 XmNindicatorType, XmONE_OF_MANY,
							 XmNtopAttachment, XmATTACH_WIDGET,
							 XmNtopWidget, print_to_printer_text,
							 XmNleftAttachment, XmATTACH_FORM,
							 XmNset, resource.default_printing_target == TO_FILE,
							 XmNleftOffset, 0,
							 NULL);
	XmStringFree(str);
	XtAddCallback(to_file_radio_or_label, XmNvalueChangedCallback, cb_print_vs_save, (XtPointer)info);
	
	str = XmStringCreateLocalized("Browse ...");
	print_to_file_button = XtVaCreateManagedWidget(Xdvi_BROWSE_BUTTON, xmPushButtonWidgetClass, destination_form,
						       XmNlabelString, str,
						       /*  XmNsensitive, resource.default_printing_target == TO_FILE, */
						       XmNtopAttachment, XmATTACH_WIDGET,
						       XmNtopWidget, print_to_printer_text,
						       XmNrightAttachment, XmATTACH_FORM,
						       NULL);
	XmStringFree(str);	
	
	XtOverrideTranslations(to_file_radio_or_label, xlats);
	XtOverrideTranslations(print_to_file_button, xlats);
    
	print_to_file_text = XtVaCreateManagedWidget(Xdvi_TO_FILE_TEXT, xmTextFieldWidgetClass, destination_form,
						     /*  XmNsensitive, resource.default_printing_target == TO_FILE, */
						     XmNtopAttachment, XmATTACH_WIDGET,
						     XmNtopWidget, print_to_printer_text,
						     XmNrightAttachment, XmATTACH_WIDGET,
						     XmNrightWidget, print_to_file_button,
						     XmNleftAttachment, XmATTACH_WIDGET,
						     XmNleftWidget, to_file_radio_or_label,
						     XTranslations, xlats,
						     NULL);
    
	adjust_heights(to_file_radio_or_label, print_to_file_button, print_to_file_text, NULL);

	XtAddCallback(print_to_file_text, XmNactivateCallback, cb_print_or_save, (XtPointer)info);
	XtAddCallback(print_to_file_text, XmNfocusCallback, cb_text_print_vs_save, (XtPointer)info);
	XtAddCallback(print_to_file_button, XmNactivateCallback, cb_browse, (XtPointer)info);
    
	XtManageChild(destination_form);
	XtManageChild(print_or_save_frame);
    
	/* initial value for printer name */
	XtVaSetValues(print_to_printer_text,
		      XmNvalue, info->printer_options,
		      XmNcursorPosition, strlen(info->printer_options),
		      NULL);
    }
    else {  /* saving, not printing */
	print_or_save_frame = XtVaCreateWidget("save_as_frame", xmFrameWidgetClass, form,
					       XmNmarginWidth, DDIST,
					       XmNmarginHeight, DDIST,
					       XmNtopAttachment, XmATTACH_FORM,
					       XmNleftAttachment, XmATTACH_FORM,
					       XmNrightAttachment, XmATTACH_FORM,
					       NULL);
    
	str = XmStringCreateLocalized("Save as:");
	XtVaCreateManagedWidget("title", xmLabelGadgetClass,
				print_or_save_frame,
				XmNchildType, XmFRAME_TITLE_CHILD,
				XmNlabelString, str,
				NULL);
	XmStringFree(str);

	destination_form = XtVaCreateWidget("destination_form", xmFormWidgetClass, print_or_save_frame,
					    XmNhorizontalSpacing, DDIST,
					    XmNverticalSpacing, DDIST,
					    XmNtopAttachment, XmATTACH_FORM,
					    XmNleftAttachment, XmATTACH_FORM,
					    XmNrightAttachment, XmATTACH_FORM,
					    XTranslations, xlats,
					    NULL);
	/* 	fprintf(stderr, "CHILD: 0x%x\n", (long)destination_form); */
	str = XmStringCreateLocalized("Format:");
	format_selection_texts[0] = XmStringCreateLocalized((char *)output_format_mapping[0].fmt_string);
	format_selection_texts[1] = XmStringCreateLocalized((char *)output_format_mapping[1].fmt_string);
	format_selection_texts[2] = XmStringCreateLocalized((char *)output_format_mapping[2].fmt_string);
	format_selection_texts[3] = XmStringCreateLocalized((char *)output_format_mapping[3].fmt_string);
	format_selection_texts[4] = XmStringCreateLocalized((char *)output_format_mapping[4].fmt_string);
	format_selection_option
	    = XmVaCreateSimpleOptionMenu(destination_form, "format_selection_option",
					 str,  'F',
					 resource.default_saving_format, /*initial menu selection*/
					 cb_select_format, /* callback */
					 XmVaPUSHBUTTON, format_selection_texts[0], 'P', NULL, NULL,
					 XmVaPUSHBUTTON, format_selection_texts[1], 'F', NULL, NULL,
					 XmVaPUSHBUTTON, format_selection_texts[2], 'D', NULL, NULL,
					 XmVaPUSHBUTTON, format_selection_texts[3], 'T', NULL, NULL,
					 XmVaPUSHBUTTON, format_selection_texts[4], 'U', NULL, NULL,
					 XmNuserData, (XtPointer)info,
					 NULL);

	XmStringFree(str);
	XmStringFree(format_selection_texts[0]);
	XmStringFree(format_selection_texts[1]);
	XmStringFree(format_selection_texts[2]);
	XmStringFree(format_selection_texts[3]);
	XmStringFree(format_selection_texts[4]);
	
	str = XmStringCreateLocalized("File name:");
	to_file_radio_or_label = XtVaCreateManagedWidget(Xdvi_TO_FILE_NAME, xmLabelGadgetClass, destination_form,
							 XmNtopAttachment, XmATTACH_WIDGET,
							 XmNtopWidget, format_selection_option,
							 XmNleftAttachment, XmATTACH_FORM,
							 /* XmNchildType, XmFRAME_TITLE_CHILD, */
							 XmNlabelString, str,
							 NULL);
	XmStringFree(str);
	str = XmStringCreateLocalized("Browse ...");
	print_to_file_button = XtVaCreateManagedWidget(Xdvi_BROWSE_BUTTON, xmPushButtonWidgetClass, destination_form,
						       XmNlabelString, str,
						       XmNtopAttachment, XmATTACH_WIDGET,
						       XmNtopWidget, format_selection_option,
						       XmNrightAttachment, XmATTACH_FORM,
						       XTranslations, xlats,
						       NULL);
	print_to_file_text = XtVaCreateManagedWidget(Xdvi_TO_FILE_TEXT, xmTextFieldWidgetClass, destination_form,
						     XmNtopAttachment, XmATTACH_WIDGET,
						     XmNtopWidget, format_selection_option,
						     XmNleftAttachment, XmATTACH_WIDGET,
						     XmNleftWidget, to_file_radio_or_label,
						     XmNrightAttachment, XmATTACH_WIDGET,
						     XmNrightWidget, print_to_file_button,
						     XTranslations, xlats,
						     NULL);

	adjust_heights(to_file_radio_or_label, print_to_file_button, print_to_file_text, NULL);
	XtAddCallback(print_to_file_button, XmNactivateCallback, cb_browse, (XtPointer)info);
    }
	
    /* initial value for filename */
    XtVaSetValues(print_to_file_text,
		  XmNvalue, info->finfo->out_file,
		  XmNcursorPosition, strlen(info->finfo->out_file),
		  NULL);

    { /* align left edges of widgets */
	Dimension w1, w2;
	
	XtVaGetValues(to_file_radio_or_label, XmNwidth, &w2, NULL);
	if (act == FILE_PRINT)
	    XtVaGetValues(to_printer_radio, XmNwidth, &w1, NULL);
	else {
	    Widget w;
	    if (get_widget_by_name(&w, format_selection_option, "OptionLabel", True)) {
		XtVaGetValues(w, XmNwidth, &w1, NULL);
	    }
	    else {
		w1 = w2;
	    }
	}

	if (w1 > w2) {
	    Dimension offset = DDIST;
	    offset += w1 - w2;
	    XtVaSetValues(print_to_file_text, XmNleftOffset, offset, NULL);
	}
	else if (w2 > w1) {
	    Dimension offset = DDIST;
	    offset += w2 - w1;
	    if (act == FILE_PRINT)
		XtVaSetValues(print_to_printer_text, XmNleftOffset, offset, NULL);
	    else {
		Widget w;
		if (get_widget_by_name(&w, format_selection_option, "OptionLabel", True)) {
		    /* FIXME: Setting width would be cleaner, but it doesn't work ...?
		       Dimension curr_w;
		       XtVaGetValues(w, XmNwidth, &curr_w, NULL);
		       XtVaSetValues(w, XmNwidth, curr_w + offset, NULL);
		    */
		    /*  fprintf(stderr, "Setting width from %d to: %d\n", curr_w, curr_w + offset); */
		    XtVaSetValues(w,
				  XmNmarginRight, offset,
				  XmNalignment, XmALIGNMENT_BEGINNING,
				  NULL);
		}
    
	    }			      
	}
    }

    if (act == FILE_SAVE) {
	XtManageChild(format_selection_option);
	XtManageChild(destination_form);
	XtManageChild(print_or_save_frame);
    }

    /* additional dvips options */
    str = XmStringCreateLocalized("Dvips Options:");
    dvips_options_label = XtVaCreateManagedWidget(Xdvi_DVIPS_OPTIONS_NAME, xmLabelGadgetClass, form,
						  XmNlabelString, str,
						  XmNtopAttachment, XmATTACH_WIDGET,
						  XmNtopWidget, print_or_save_frame,
						  XmNleftAttachment, XmATTACH_FORM,
						  NULL);
    XmStringFree(str);

    dvips_options_text = XtVaCreateManagedWidget(Xdvi_DVIPS_OPTIONS_TEXT, xmTextFieldWidgetClass, form,
						 XmNtopAttachment, XmATTACH_WIDGET,
						 XmNtopWidget, print_or_save_frame,
						 XmNleftAttachment, XmATTACH_WIDGET,
						 XmNrightAttachment, XmATTACH_FORM,
						 XmNleftWidget, dvips_options_label,
						 XmNcolumns, 20,
						 XTranslations, xlats,
						 NULL);
    XtAddCallback(dvips_options_text, XmNactivateCallback, cb_print_or_save, (XtPointer)info);
    adjust_heights(dvips_options_label, dvips_options_text, NULL);

    /* initial value for dvips options */
    XtVaSetValues(dvips_options_text,
		  XmNvalue, info->dvips_options,
		  XmNcursorPosition, strlen(info->dvips_options),
		  NULL);

    /* page selection */
    pages_frame = XtVaCreateWidget(act == FILE_PRINT ? "print_page_frame" : "save_page_frame", xmFrameWidgetClass, form,
				   XmNmarginWidth, DDIST,
				   XmNmarginHeight, DDIST,
				   XmNresizable, True,
				   XmNtopAttachment, XmATTACH_WIDGET,
				   XmNtopWidget, dvips_options_text,
				   XmNleftAttachment, XmATTACH_FORM,
				   XmNrightAttachment, XmATTACH_FORM,
				   XTranslations, xlats,
				   NULL);

    str = XmStringCreateLocalized("Pages:");
    XtVaCreateManagedWidget("title", xmLabelGadgetClass, pages_frame,
					  XmNchildType, XmFRAME_TITLE_CHILD,
					  XmNlabelString, str,
					  NULL);
    XmStringFree(str);

    pages_form = XtVaCreateWidget("pages_form", xmFormWidgetClass, pages_frame,
				  XmNhorizontalSpacing, DDIST,
				  XmNverticalSpacing, DDIST,
				  XmNresizable, True,
				  XTranslations, xlats,
				  NULL);

    str = XmStringCreateLocalized("All");
    print_or_save_range_all_radio = XtVaCreateManagedWidget(Xdvi_PAGES_ALL_RADIO_NAME, xmToggleButtonGadgetClass, pages_form,
							    XmNlabelString, str,
							    XmNindicatorType, XmONE_OF_MANY,
							    XmNset, True,
							    XmNtopAttachment, XmATTACH_FORM,
							    XmNtopOffset, 0,
							    XmNleftAttachment, XmATTACH_FORM,
							    XmNleftOffset, 0,
							    XTranslations, xlats,
							    NULL);
    XmStringFree(str);
    XtAddCallback(print_or_save_range_all_radio, XmNvalueChangedCallback, cb_range, (XtPointer)info);

    str = XmStringCreateLocalized("Marked");
    range_marked_radio =  XtVaCreateManagedWidget(Xdvi_PAGES_MARKED_RADIO_NAME, xmToggleButtonGadgetClass, pages_form,
						  XmNlabelString, str,
						  XmNindicatorType, XmONE_OF_MANY,
						  XmNtopAttachment, XmATTACH_WIDGET,
						  XmNsensitive, pageinfo_have_marked_pages(),
						  XmNtopWidget, print_or_save_range_all_radio,
						  XmNleftAttachment, XmATTACH_FORM,
						  XmNleftOffset, 0,
						  XTranslations, xlats,
						  NULL);
    XmStringFree(str);
    XtAddCallback(range_marked_radio, XmNvalueChangedCallback, cb_range, (XtPointer)info);

    str = XmStringCreateLocalized("Range:");
    range_from_to_radio = XtVaCreateManagedWidget(Xdvi_PAGES_RANGE_RADIO_NAME, xmToggleButtonGadgetClass, pages_form,
						  XmNlabelString, str,
						  XmNindicatorType, XmONE_OF_MANY,
						  XmNtopAttachment, XmATTACH_WIDGET,
						  XmNtopWidget, range_marked_radio,
						  XmNleftAttachment, XmATTACH_FORM,
						  XmNleftOffset, 0,
						  XTranslations, xlats,
						  NULL);
    XmStringFree(str);

    XtAddCallback(range_from_to_radio, XmNvalueChangedCallback, cb_range, (XtPointer)info);

    str = XmStringCreateLocalized("From");
    from_label = XtVaCreateManagedWidget(Xdvi_PAGES_RANGE_FROM_LABEL_NAME, xmLabelGadgetClass, pages_form,
					 XmNlabelString, str,
					 /*  XmNsensitive, info->page_selection == PAGE_RANGE, */
					 XmNtopAttachment, XmATTACH_WIDGET,
					 XmNtopWidget, range_marked_radio,
					 XmNleftAttachment, XmATTACH_WIDGET,
					 XmNleftWidget, range_from_to_radio,
					 NULL);
    XmStringFree(str);

    page_from_text = XtVaCreateManagedWidget(Xdvi_PAGES_RANGE_FROM_TEXT_NAME, xmTextFieldWidgetClass, pages_form,
					     XmNcolumns, 5,
					     /*  XmNsensitive, info->page_selection == PAGE_RANGE, */
					     XmNtopAttachment, XmATTACH_WIDGET,
					     XmNtopWidget, range_marked_radio,
					     XmNleftAttachment, XmATTACH_WIDGET,
					     XmNleftWidget, from_label,
					     XTranslations, xlats,
					     NULL);
    XtAddCallback(page_from_text, XmNactivateCallback, cb_print_or_save, (XtPointer)info);
    XtAddCallback(page_from_text, XmNfocusCallback, cb_text_page_range, (XtPointer)info);

    str = XmStringCreateLocalized("to");
    to_label = XtVaCreateManagedWidget(Xdvi_PAGES_RANGE_TO_LABEL_NAME, xmLabelGadgetClass, pages_form,
				       XmNlabelString, str,
				       /*  XmNsensitive, info->page_selection == PAGE_RANGE, */
				       XmNtopAttachment, XmATTACH_WIDGET,
				       XmNtopWidget, range_marked_radio,
				       XmNleftAttachment, XmATTACH_WIDGET,
				       XmNleftWidget, page_from_text,
				       NULL);
    XmStringFree(str);

    page_to_text = XtVaCreateManagedWidget(Xdvi_PAGES_RANGE_TO_TEXT_NAME, xmTextFieldWidgetClass, pages_form,
					   XmNcolumns, 5,
					   /*  XmNsensitive, info->page_selection == PAGE_RANGE, */
					   XmNtopAttachment, XmATTACH_WIDGET,
					   XmNtopWidget, range_marked_radio,
					   XmNleftAttachment, XmATTACH_WIDGET,
					   XmNleftWidget, to_label,
					   XTranslations, xlats,
					   NULL);
    XtAddCallback(page_to_text, XmNactivateCallback, cb_print_or_save, (XtPointer)info);
    XtAddCallback(page_to_text, XmNfocusCallback, cb_text_page_range, (XtPointer)info);

    str = XmStringCreateLocalized(ofstring);
    of_label = XtVaCreateManagedWidget(Xdvi_PAGES_RANGE_OF_LABEL_NAME, xmLabelGadgetClass, pages_form,
				       XmNlabelString, str,
				       /*  XmNsensitive, info->page_selection == PAGE_RANGE, */
				       XmNresizable, True,
				       XmNtopAttachment, XmATTACH_WIDGET,
				       XmNtopWidget, range_marked_radio,
				       XmNleftAttachment, XmATTACH_WIDGET,
				       XmNleftWidget, page_to_text,
				       NULL);
    XmStringFree(str);

    adjust_heights(range_from_to_radio,
		   from_label, page_from_text,
		   to_label, page_to_text,
		   of_label,
		   NULL);

    XtManageChild(pages_form);
    XtManageChild(pages_frame);

    /* box for the OK/Cancel button */
    box = XtVaCreateManagedWidget("box", xmFormWidgetClass, pane,
				  XmNskipAdjust, True, /* don't resize this area */
				  XTranslations, xlats,
				  NULL);

    str = XmStringCreateLocalized(act == FILE_PRINT ? "Print" : "Save");

    ok_button = XtVaCreateManagedWidget(act == FILE_PRINT ? "print_button" : "save_button", xmPushButtonWidgetClass, box,
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
					NULL);
    XmStringFree(str);

    XtAddCallback(ok_button, XmNactivateCallback, cb_print_or_save, (XtPointer)info);
    XtOverrideTranslations(ok_button, XtParseTranslationTable("<Key>Return:ArmAndActivate()"));

    str = XmStringCreateLocalized("Cancel");
    cancel_button = XtVaCreateManagedWidget("cancel", xmPushButtonWidgetClass, box,
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
	
    XtOverrideTranslations(print_or_save_range_all_radio, xlats);
    XtOverrideTranslations(range_marked_radio, xlats);
    XtOverrideTranslations(range_from_to_radio, xlats);
    XtOverrideTranslations(ok_button, xlats);
    XtOverrideTranslations(cancel_button, xlats);

    XtAddCallback(cancel_button, XmNactivateCallback, cb_popdown, (XtPointer)info);
    
    { /* make the buttons the same size: */
	Dimension w1, w2;
	XtVaGetValues(ok_button, XtNwidth, &w1, NULL);
	XtVaGetValues(cancel_button, XtNwidth, &w2, NULL);
	if (w1 < w2)
	    XtVaSetValues(ok_button, XtNwidth, w2, NULL);
	else if (w2 > w1)
	    XtVaSetValues(cancel_button, XtNwidth, w1, NULL);
    }
	
    if (pageinfo_have_marked_pages()) {
	XmToggleButtonGadgetSetState(range_marked_radio, True, True);
    }
    else if (info->page_selection == PAGE_MARKED) {
	XmToggleButtonGadgetSetState(print_or_save_range_all_radio, True, True);
    }

    XtManageChild(form);
    XtManageChild(pane);
    center_window(info->shell, globals.widgets.top_level);
    XtMapWidget(info->shell);
    XmProcessTraversal(ok_button, XmTRAVERSE_CURRENT);

    /*      if (act == FILE_PRINT) { */
    /*  	if (resource.default_printing_target == TO_PRINTER) */
    /*  	    XmProcessTraversal(print_to_printer_text, XmTRAVERSE_CURRENT); */
    /*  	else */
    /*  	    XmProcessTraversal(print_to_file_text, XmTRAVERSE_CURRENT); */
    /*      } */
}			      

#else /* MOTIF */
    
static void
xaw_create_dialog(struct save_or_print_info *info)
{
    printOrSaveActionT act = info->act;
    Atom WM_DELETE_WINDOW;
    char *ptr;
    char ofstring[1024];
    
    Widget form, paned, box;
    Widget dummy_label_form, dummy_pages_form; /* dummy forms to get indentation consistent */
    Widget save_to_file_form;
    Widget print_to_printer_form, print_to_file_form, dvips_options_form;
    Widget dvips_options_label;
    Widget page_range_form;
    Widget range_marked_form;
    Widget range_from_to_form;
    Widget print_to_file_text, to_file_radio_or_label;
    Widget dvips_options_text;
    Widget print_to_file_button;
    Widget format_selection_menu[FORMAT_SEL_CNT];
    Widget format_selection_label = NULL, format_selection_button = NULL, format_selection_popup;
    Widget range_marked_radio, range_from_to_radio, to_printer_radio = NULL, print_to_printer_text = NULL,
	print_or_save_range_all_radio,
	page_from_text, to_label, page_to_text, of_label, ok_button, cancel_button;
    
    XtTranslations xlats, xlats2, wm_translations;
    XtAccelerators accels2;
    int ddist;

    static Dimension w_avg = 220;
    Dimension w_curr;
    
    /* handy abbrevs */
#define HORIZONTAL_RESIZING_NO	XtNleft, XtChainLeft, XtNright, XtChainLeft
#define HORIZONTAL_RESIZING_YES XtNleft, XtChainLeft, XtNright, XtChainRight
#define VERTICAL_RESIZING_NO	XtNtop, XtChainTop, XtNbottom, XtChainTop

    XtAddActions(print_actions, XtNumber(print_actions));

    if (globals.pageno_correct == 1)
	sprintf(ofstring, "of %d physical pages", total_pages);
    else
	sprintf(ofstring, "of %d to %d pages", globals.pageno_correct, total_pages + globals.pageno_correct - 1);
   
    ptr = get_string_va("<Message>WM_PROTOCOLS: WM_cancel(%p)\n"
			"<Key>Escape:WM_cancel(%p)\n"
			"<Key>q:WM_cancel(%p)",
			info, info, info);
    wm_translations = XtParseTranslationTable(ptr);
    free(ptr);

    info->shell = XtVaCreatePopupShell(act == FILE_SAVE ? Xdvi_SAVE_SHELL_NAME : Xdvi_PRINT_SHELL_NAME,
				       transientShellWidgetClass, globals.widgets.top_level,
				       XtNtitle, act == FILE_SAVE ? "xdvik: Save File" : "xdvik: Print File",
				       XtNmappedWhenManaged, False,
				       XtNtransientFor, globals.widgets.top_level,
				       XtNallowShellResize, True,
				       XtNtranslations, wm_translations,
				       NULL);
    
    WM_DELETE_WINDOW = XInternAtom(XtDisplay(info->shell), "WM_DELETE_WINDOW", False);

    paned = XtVaCreateManagedWidget("paned", panedWidgetClass, info->shell, NULL);
	
    form = XtVaCreateManagedWidget("save_or_print_form", formWidgetClass, paned,
				   XtNallowResize, True,
				   NULL);
    XtVaGetValues(form, XtNdefaultDistance, &ddist, NULL);
    /*     fprintf(stderr, "form1: %ld\n", (long)form); */
    
    xlats = XtParseTranslationTable("<EnterWindow>:highlight(Always)\n"
				    "<LeaveWindow>:unhighlight()\n"
				    "<Btn1Down>,<Btn1Up>:set()notify()");

    ptr = get_string_va("<Key>Return:xaw_print_or_save(%p)", info);
    xlats2 = XtParseTranslationTable(ptr);
    free(ptr);

    if (act == FILE_PRINT) { /* selection `to printer/to file' */
	/* FIXME: set real sizes here */
	/*  	Dimension w_curr; */
	dummy_label_form = XtVaCreateManagedWidget("dummy_print_to_label_form", formWidgetClass, form,
						   XtNresizable, True,
						   XtNborderWidth, 0,
						   HORIZONTAL_RESIZING_YES,
						   NULL);
	             XtVaCreateManagedWidget("print_to", labelWidgetClass, dummy_label_form,
					     XtNlabel, "Print to: ",
					     XtNborderWidth, 0,
					     HORIZONTAL_RESIZING_NO,
					     NULL);

	print_to_printer_form = XtVaCreateManagedWidget("print_to_printer_form", formWidgetClass, form,
							XtNresizable, True,
							XtNborderWidth, 0,
							XtNvertDistance, 0,
							XtNfromVert, dummy_label_form,
							HORIZONTAL_RESIZING_YES,
							XtNallowResize, True,
							NULL);

	accels2 = XtParseAcceleratorTable("<Btn1Down>,<Btn1Up>:set()notify()");

	to_printer_radio = XtVaCreateManagedWidget(Xdvi_TO_PRINTER_NAME,
#ifdef XAW
						   radioWidgetClass,
#else
						   toggleWidgetClass,
#endif
						   print_to_printer_form,
						   XtNlabel, "Printer: ",
						   XtNborderWidth, 0,
						   XtNhighlightThickness, 1,
						   XtNradioData, TO_PRINTER,
						   XtNstate, resource.default_printing_target == TO_PRINTER,
						   XtNtranslations, xlats,
						   XtNaccelerators, accels2,
						   HORIZONTAL_RESIZING_NO,
						   VERTICAL_RESIZING_NO,
						   NULL);

	XtAddCallback(to_printer_radio, XtNcallback, cb_print_vs_save, (XtPointer)info);
	XtInstallAccelerators(print_to_printer_form, to_printer_radio);
	
	print_to_printer_text = XtVaCreateManagedWidget(Xdvi_TO_PRINTER_TEXT, asciiTextWidgetClass, print_to_printer_form,
							XtNwidth, w_avg,
							XtNdataCompression, False,
							XtNeditType, XawtextEdit,
							/*  XtNresize, XawtextResizeWidth, */
							XtNfromHoriz, to_printer_radio,
							HORIZONTAL_RESIZING_YES,
							VERTICAL_RESIZING_NO,
							/*  XtNscrollHorizontal, XawtextScrollAlways, */
							NULL);
	XtOverrideTranslations(print_to_printer_text, xlats2);
	XtAddEventHandler(print_to_printer_text, KeyPressMask | ButtonPressMask, False,
			  text_print_vs_save_callback, (XtPointer)info);

	adjust_vertically(to_printer_radio, print_to_printer_text, ddist);
	
	print_to_file_form = XtVaCreateManagedWidget("print_to_file_form", formWidgetClass, form,
						     XtNborderWidth, 0,
						     XtNvertDistance, -ddist,
						     XtNfromVert, print_to_printer_form,
						     HORIZONTAL_RESIZING_YES,
						     NULL);
	
	to_file_radio_or_label = XtVaCreateManagedWidget(Xdvi_TO_FILE_NAME,
#ifdef XAW
							 radioWidgetClass,
#else
							 toggleWidgetClass,
#endif
							 print_to_file_form,
							 XtNhighlightThickness, 1,
							 XtNborderWidth, 0,
							 XtNlabel, "PS File: ",
							 XtNstate, resource.default_printing_target == TO_FILE,
							 XtNradioGroup, to_printer_radio,
							 XtNradioData, TO_FILE,
							 XtNtranslations, xlats,
							 XtNaccelerators, accels2,
							 HORIZONTAL_RESIZING_NO,
							 VERTICAL_RESIZING_NO,
							 NULL);
	XtAddCallback(to_file_radio_or_label, XtNcallback, cb_print_vs_save, (XtPointer)info);
	XtInstallAccelerators(print_to_file_form, to_file_radio_or_label);

	print_to_file_text = XtVaCreateManagedWidget(Xdvi_TO_FILE_TEXT, asciiTextWidgetClass, print_to_file_form,
						     XtNwidth, w_avg,
						     XtNdataCompression, False,
						     XtNeditType, XawtextEdit,
						     XtNfromHoriz, to_file_radio_or_label,
						     HORIZONTAL_RESIZING_YES,
						     VERTICAL_RESIZING_NO,
						     NULL);
	XtOverrideTranslations(print_to_file_text, xlats2);
	XtAddEventHandler(print_to_file_text, KeyPressMask | ButtonPressMask, False,
			  text_print_vs_save_callback, (XtPointer)info);
	
	/* initial value for printer name */
	XtVaSetValues(print_to_printer_text, XtNstring, info->printer_options, NULL);
	XawTextSetInsertionPoint(print_to_printer_text, strlen(info->printer_options));
    }
    else { /* not printing, but saving */
	int i;
	Dimension max_entry_width = 0;
	dummy_label_form = XtVaCreateManagedWidget("dummy_save_as_form", formWidgetClass, form,
						   XtNresizable, True,
						   XtNborderWidth, 0,
						   HORIZONTAL_RESIZING_YES,
						   NULL);
	             XtVaCreateManagedWidget("save_as", labelWidgetClass, dummy_label_form,
					     XtNlabel, "Save as: ",
					     XtNborderWidth, 0,
					     HORIZONTAL_RESIZING_NO,
					     NULL);
	save_to_file_form = XtVaCreateManagedWidget("save_to_file_form", formWidgetClass, form,
						    XtNborderWidth, 0,
						    XtNfromVert, dummy_label_form,
						    HORIZONTAL_RESIZING_YES,
						    VERTICAL_RESIZING_NO,
						    NULL);
	/* 	fprintf(stderr, "formr1: %ld\n", (long)save_to_file_form); */
#if 0
	menu_double_arrow_bitmap
	    = XCreateBitmapFromData(XtDisplay(globals.widgets.top_level),
				    RootWindowOfScreen(XtScreen(globals.widgets.top_level)),
				    (char *)menu_double_arrow_bits, MENU_DOUBLE_ARROW_W, MENU_DOUBLE_ARROW_H);
#endif
	format_selection_label = XtVaCreateManagedWidget("format_selection_label", labelWidgetClass, save_to_file_form,
							 XtNlabel, "Format: ",
							 XtNborderWidth, 0,
							 HORIZONTAL_RESIZING_NO,
							 VERTICAL_RESIZING_NO,
							 NULL);
	format_selection_button = XtVaCreateManagedWidget(Xdvi_FORMAT_SELECTION_BUTTON_NAME, menuButtonWidgetClass, save_to_file_form,
							  XtNmenuName, "format_selection_popup",
							  XtNlabel, output_format_mapping[resource.default_saving_format].fmt_string,
							  XtNjustify, XtJustifyLeft,
							  XtNborderWidth, resource.btn_border_width,
							  XtNfromHoriz, format_selection_label,
#if 0
							  XtNhighlightThickness, 0,
#endif
							  NULL);
#if 0 /* Hack for arrow - menuButtonWidget cannot have a XtNrightBitmap!!! */
	format_selection_b1 = XtVaCreateManagedWidget("format_selection_b1", commandWidgetClass, save_to_file_form,
						      XtNborderWidth, resource.btn_border_width,
						      XtNfromHoriz, format_selection_button,
						      XtNhorizDistance, -1,
						      XtNhighlightThickness, 0,
						      XtNbitmap, menu_double_arrow_bitmap,
						      NULL);
	adjust_heights(format_selection_button, format_selection_b1, NULL);
#endif
	format_selection_popup = XtCreatePopupShell("format_selection_popup", simpleMenuWidgetClass, format_selection_button,
						    /* 						    globals.widgets.top_level, */
						    NULL, 0);
#if 0
	XtAddCallback(format_selection_b1, XtNcallback, xaw_popup_menu_cb, format_selection_popup);
#endif
	for (i = 1; i < FORMAT_SEL_CNT; i++) {
	    char name[1024];
	    Dimension curr_entry_width;
	    SNPRINTF(name, sizeof name, "format_selection_pulldown_%d", i);
	    format_selection_menu[i] = XtVaCreateManagedWidget(name, smeBSBObjectClass, format_selection_popup,
							       XtNjustify, XtJustifyLeft,
							       XtNlabel,
							       i == resource.default_saving_format
							       ? output_format_mapping[0].fmt_string
							       : output_format_mapping[i].fmt_string,
							       NULL);
	    XtVaGetValues(format_selection_menu[i], XtNwidth, &curr_entry_width, NULL);
	    if (curr_entry_width > max_entry_width)
		max_entry_width = curr_entry_width;
	    XtAddCallback(format_selection_menu[i], XtNcallback, cb_select_format, (XtPointer)info);
	}
	XtVaSetValues(format_selection_button, XtNwidth, max_entry_width, NULL);
	
	print_to_file_form = XtVaCreateManagedWidget("to_file_form", formWidgetClass, form,
						     XtNborderWidth, 0,
						     XtNfromVert, save_to_file_form,
						     XtNvertDistance, -ddist,
						     HORIZONTAL_RESIZING_YES,
						     VERTICAL_RESIZING_NO,
						     NULL);
	to_file_radio_or_label = XtVaCreateManagedWidget(Xdvi_TO_FILE_NAME, labelWidgetClass, print_to_file_form,
							 XtNlabel, "File name: ",
							 XtNborderWidth, 0,
							 HORIZONTAL_RESIZING_NO,
							 VERTICAL_RESIZING_NO,
							 NULL);
	print_to_file_text = XtVaCreateManagedWidget(Xdvi_TO_FILE_TEXT, asciiTextWidgetClass, print_to_file_form,
						     XtNwidth, w_avg,
						     XtNdataCompression, False,
						     XtNeditType, XawtextEdit,
						     XtNfromHoriz, to_file_radio_or_label,
						     HORIZONTAL_RESIZING_YES,
						     VERTICAL_RESIZING_NO,
						     NULL);
	XtOverrideTranslations(print_to_file_text, xlats2);
	
	
	{ /* align left edges of format selection pulldown and filename field */
	    Dimension w1, w2, w_max;
	    XtVaGetValues(format_selection_label, XtNwidth, &w1, NULL);
	    XtVaGetValues(to_file_radio_or_label, XtNwidth, &w2, NULL);
	    w_max = MAX(w1, w2);
	    XtVaSetValues(format_selection_button, XtNhorizDistance, ddist + (w_max - w1), NULL);
	    XtVaSetValues(print_to_file_text, XtNhorizDistance, ddist + (w_max - w2), NULL);
	}
    }

    /* initial value for filename */
    XtVaSetValues(print_to_file_text, XtNstring, info->finfo->out_file, NULL);
    XawTextSetInsertionPoint(print_to_file_text, strlen(info->finfo->out_file));

#define DIST_1 8 /* FIXME */
    print_to_file_button = XtVaCreateManagedWidget(Xdvi_BROWSE_BUTTON, commandWidgetClass, print_to_file_form,
						   XtNlabel, "Browse ...",
						   XtNfromHoriz, print_to_file_text,
						   XtNhorizDistance, DIST_1,
						   /*  XtNsensitive, act == FILE_PRINT ? resource.default_printing_target == TO_FILE : True, */
						   /* attach to right of form, no resizing: */
						   XtNleft, XtChainRight, XtNright, XtChainRight,
						   VERTICAL_RESIZING_NO,
						   NULL);

    XtVaGetValues(print_to_file_button, XtNwidth, &w_curr, NULL);
    XtVaSetValues(print_to_file_text, XtNwidth, w_avg - w_curr - DIST_1 - 2, NULL);
#undef DIST_1

    adjust_vertically(to_file_radio_or_label, print_to_file_text, ddist + 5);
    adjust_vertically(print_to_file_text, to_file_radio_or_label, ddist + 5);
    adjust_vertically(print_to_file_button, to_file_radio_or_label, ddist + 5);

    XtAddCallback(print_to_file_button, XtNcallback, cb_browse, (XtPointer)info);

    /* other dvips options */
    dvips_options_form = XtVaCreateManagedWidget("dvips_options_form", formWidgetClass, form,
						 XtNborderWidth, 0,
						 XtNfromVert, print_to_file_form,
						 XtNvertDistance, 3 * ddist,
						 HORIZONTAL_RESIZING_YES,
						 NULL);

    dvips_options_label = XtVaCreateManagedWidget(Xdvi_DVIPS_OPTIONS_NAME, labelWidgetClass, dvips_options_form,
						  XtNlabel, "Dvips Options:",
						  XtNborderWidth, 0,
						  HORIZONTAL_RESIZING_NO,
						  VERTICAL_RESIZING_NO,
						  NULL);
				  
    dvips_options_text = XtVaCreateManagedWidget(Xdvi_DVIPS_OPTIONS_TEXT, asciiTextWidgetClass, dvips_options_form,
						 XtNwidth, w_avg,
						 XtNdataCompression, False,
						 XtNeditType, XawtextEdit,
						 XtNfromHoriz, dvips_options_label,
						 HORIZONTAL_RESIZING_YES,
						 VERTICAL_RESIZING_NO,
						 NULL);
    XtOverrideTranslations(dvips_options_text, xlats2);

    { /* align left edges of input fields */
	Dimension w1, w2, w3, w_max;

	if (act == FILE_PRINT)
	    XtVaGetValues(to_printer_radio, XtNwidth, &w1, NULL);
	else
	    XtVaGetValues(format_selection_label, XtNwidth, &w1, NULL);

	XtVaGetValues(to_file_radio_or_label, XtNwidth, &w2, NULL);
	XtVaGetValues(dvips_options_label, XtNwidth, &w3, NULL);

	w_max = MAX(MAX(w1, w2), w3);

	if (act == FILE_PRINT)
	    XtVaSetValues(print_to_printer_text, XtNhorizDistance, ddist + (w_max - w1), NULL);
	else
	    XtVaSetValues(format_selection_button, XtNhorizDistance, ddist + (w_max - w1), NULL);
    
	XtVaSetValues(print_to_file_text, XtNhorizDistance, ddist + (w_max - w2), NULL);
	XtVaSetValues(dvips_options_text, XtNhorizDistance, ddist + (w_max - w3), NULL);
    }
    
    /* initial value for dvips options */
    XtVaSetValues(dvips_options_text, XtNstring, info->dvips_options, NULL);
    XawTextSetInsertionPoint(dvips_options_text, strlen(info->dvips_options));

    /* page selection */
    dummy_pages_form = XtVaCreateManagedWidget("dummy_pages_form", formWidgetClass, form,
					       XtNfromVert, dvips_options_form,
					       XtNresizable, True,
					       XtNborderWidth, 0,
					       XtNvertDistance, 3 * ddist,
					       HORIZONTAL_RESIZING_NO,
					       NULL);

                  XtVaCreateManagedWidget("range_lab", labelWidgetClass, dummy_pages_form,
					  XtNlabel, "Pages:",
					  XtNborderWidth, 0,
					  HORIZONTAL_RESIZING_NO,
					  NULL);

    page_range_form = XtVaCreateManagedWidget(Xdvi_PAGE_RANGE_FORM_NAME, formWidgetClass, form,
					      XtNborderWidth, 0,
					      XtNfromVert, dummy_pages_form,
					      XtNvertDistance, 0,
					      HORIZONTAL_RESIZING_NO,
					      NULL);
    
    accels2 = XtParseAcceleratorTable("<Btn1Down>,<Btn1Up>:set()notify()");
    print_or_save_range_all_radio = XtVaCreateManagedWidget(Xdvi_PAGES_ALL_RADIO_NAME,
#ifdef XAW
							    radioWidgetClass,
#else
							    toggleWidgetClass,
#endif
							    page_range_form,
							    XtNlabel, "All ",
							    XtNborderWidth, 0,
							    XtNhighlightThickness, 1,
							    XtNradioData, PAGE_ALL,
							    /* enable this button by default */
							    XtNstate, True,
							    XtNtranslations, xlats,
							    XtNaccelerators, accels2,
							    HORIZONTAL_RESIZING_NO,
							    VERTICAL_RESIZING_NO,
							    NULL);
    XtAddCallback(print_or_save_range_all_radio, XtNcallback, cb_range, (XtPointer)info);
    XtInstallAccelerators(page_range_form, print_or_save_range_all_radio);
    
    range_marked_form = XtVaCreateManagedWidget("range_marked_form", formWidgetClass, form,
						XtNborderWidth, 0,
						XtNfromVert, page_range_form,
						XtNvertDistance, 0,
						HORIZONTAL_RESIZING_NO,
						NULL);

    range_marked_radio = XtVaCreateManagedWidget(Xdvi_PAGES_MARKED_RADIO_NAME,
#ifdef XAW
						 radioWidgetClass,
#else
						 toggleWidgetClass,
#endif

						 range_marked_form,
						 XtNlabel, "Marked ",
						 XtNsensitive, pageinfo_have_marked_pages(),
						 XtNborderWidth, 0,
						 XtNhighlightThickness, 1,
						 XtNradioGroup, print_or_save_range_all_radio,
						 XtNradioData, PAGE_MARKED,
						 XtNtranslations, xlats,
						 XtNaccelerators, accels2,
						 HORIZONTAL_RESIZING_NO,
						 VERTICAL_RESIZING_NO,
						 NULL);
    XtAddCallback(range_marked_radio, XtNcallback, cb_range, (XtPointer)info);
    XtInstallAccelerators(range_marked_form, range_marked_radio);

    range_from_to_form = XtVaCreateManagedWidget("range_from_to_form", formWidgetClass, form,
						 XtNborderWidth, 0,
						 XtNfromVert, range_marked_form,
						 XtNvertDistance, 0,
						 XtNresizable, True,
						 /*  HORIZONTAL_RESIZING_NO, */
						 NULL);

    range_from_to_radio = XtVaCreateManagedWidget(Xdvi_PAGES_RANGE_RADIO_NAME,
#ifdef XAW
						  radioWidgetClass,
#else
						  toggleWidgetClass,
#endif
						  range_from_to_form,
						  XtNlabel, "From: ",
						  XtNborderWidth, 0,
						  XtNhighlightThickness, 1,
						  XtNradioGroup, print_or_save_range_all_radio,
						  XtNradioData, PAGE_RANGE,
						  XtNtranslations, xlats,
						  XtNaccelerators, accels2,
						  HORIZONTAL_RESIZING_NO,
						  VERTICAL_RESIZING_NO,
						  NULL);
    XtAddCallback(range_from_to_radio, XtNcallback, cb_range, (XtPointer)info);
    XtInstallAccelerators(range_from_to_form, range_from_to_radio);

    page_from_text = XtVaCreateManagedWidget(Xdvi_PAGES_RANGE_FROM_TEXT_NAME, asciiTextWidgetClass, range_from_to_form,
					     XtNdataCompression, False,
					     XtNeditType, XawtextEdit,
					     XtNwidth, 50,
					     XtNfromHoriz, range_from_to_radio,
					     HORIZONTAL_RESIZING_NO,
					     VERTICAL_RESIZING_NO,
					     NULL);
    XtOverrideTranslations(page_from_text, xlats2);
    XtAddEventHandler(page_from_text, KeyPressMask | ButtonPressMask, False,
		      text_page_range_callback, (XtPointer)info);

    to_label = XtVaCreateManagedWidget(Xdvi_PAGES_RANGE_TO_LABEL_NAME, labelWidgetClass, range_from_to_form,
				       XtNlabel, "to: ",
				       XtNborderWidth, 0,
				       XtNfromHoriz, page_from_text,
				       /*  XtNsensitive, info->page_selection == PAGE_RANGE, */
				       HORIZONTAL_RESIZING_NO,
				       VERTICAL_RESIZING_NO,
				       NULL);

    page_to_text = XtVaCreateManagedWidget(Xdvi_PAGES_RANGE_TO_TEXT_NAME, asciiTextWidgetClass, range_from_to_form,
					   XtNdataCompression, False,
					   XtNeditType, XawtextEdit,
					   XtNwidth, 50,
					   XtNfromHoriz, to_label,
					   HORIZONTAL_RESIZING_NO,
					   VERTICAL_RESIZING_NO,
					   NULL);
    XtOverrideTranslations(page_to_text, xlats2);
    XtAddEventHandler(page_to_text, KeyPressMask | ButtonPressMask, False,
		      text_page_range_callback, (XtPointer)info);

    of_label = XtVaCreateManagedWidget(Xdvi_PAGES_RANGE_OF_LABEL_NAME, labelWidgetClass, range_from_to_form,
				       XtNlabel, ofstring,
				       XtNborderWidth, 0,
				       /*  XtNsensitive, info->page_selection == PAGE_RANGE, */
				       XtNresizable, True,
				       XtNfromHoriz, page_to_text,
				       /*  HORIZONTAL_RESIZING_NO, */
				       XtNleft, XtChainLeft,
				       VERTICAL_RESIZING_NO,
				       NULL);

    adjust_vertically(range_from_to_radio, page_from_text, ddist);
    adjust_vertically(to_label, page_from_text, ddist);
    adjust_vertically(of_label, page_from_text, ddist);

    /* box for the OK/Cancel button */
    box = XtVaCreateManagedWidget("box", formWidgetClass, paned,
				  /* resizing by user isn't needed */
				  XtNshowGrip, False,
				  XtNdefaultDistance, 6, /* some padding */
				  /* resizing the window shouldn't influence this box,
				   * but only the pane widget
				   */
				  XtNskipAdjust, True,
				  XtNaccelerators, G_accels_cr,
				  NULL);
	
    ok_button = XtVaCreateManagedWidget(act == FILE_PRINT ? "print_button" : "save_button", commandWidgetClass, box,
					XtNlabel, act == FILE_PRINT ? "Print" : "Save",
					XtNaccelerators, G_accels_cr,
					XtNtop, XtChainTop,
					XtNbottom, XtChainBottom,
					HORIZONTAL_RESIZING_NO,
					NULL);
    XtAddCallback(ok_button, XtNcallback, cb_print_or_save, (XtPointer)info);
	
    XtInstallAccelerators(form, ok_button);
    if (act == FILE_PRINT)
	XtInstallAccelerators(print_to_printer_text, ok_button);
    XtInstallAccelerators(print_to_file_text, ok_button);

    cancel_button = XtVaCreateManagedWidget("cancel", commandWidgetClass, box,
					    XtNlabel, "Cancel",
					    XtNfromHoriz, ok_button,
					    XtNbottom, XtChainBottom,
					    XtNjustify, XtJustifyRight,
					    XtNleft, XtChainRight,
					    XtNright, XtChainRight,
					    NULL);
    XtAddCallback(cancel_button, XtNcallback, cb_popdown, (XtPointer)info);
	
    XtManageChild(info->shell);
    center_window(info->shell, globals.widgets.top_level);
    XtMapWidget(info->shell);
    XSetWMProtocols(XtDisplay(info->shell), XtWindow(info->shell), &WM_DELETE_WINDOW, 1);
	    
    if (pageinfo_have_marked_pages()) {
	XawToggleSetCurrent(print_or_save_range_all_radio, cast_int_to_XtPointer(PAGE_MARKED));
    }
    else if (info->page_selection == PAGE_MARKED) {
	XawToggleSetCurrent(print_or_save_range_all_radio, cast_int_to_XtPointer(PAGE_ALL));
    }
#undef HORIZONTAL_RESIZING_NO
#undef HORIZONTAL_RESIZING_YES
#undef VERTICAL_RESIZING_NO
}

#endif /* MOTIF */

static void
get_initial_values(struct save_or_print_info *info)
{
    const char *extension = NULL;
    char *tmp = NULL;
    
    info->finfo->tmp_dvi_file = NULL;
    info->finfo->tmp_ps_file = NULL;
    info->finfo->tmp_dvi_fp = NULL;
    /*      info->finfo->out_fp = NULL; */
        
    info->message_popup = NULL;

    info->dvips_options = info->printer_options = NULL;

    /*
     * dvips options
     */
    info->dvips_options = xstrdup(resource.dvips_options_str ? resource.dvips_options_str : "");

    /* add -t options if needed */
    if (info->dvips_options == NULL || info->dvips_options[0] == '\0' || strstr(info->dvips_options, "-t ") == NULL) {
	Boolean landscape = False;
	char *paper = get_dvips_papersize(&landscape);
	if (paper != NULL) {
	    if (info->dvips_options == NULL)
		info->dvips_options = xstrdup("-t ");
	    else
		info->dvips_options = xstrcat(info->dvips_options, " -t ");

	    info->dvips_options = xstrcat(info->dvips_options, paper);
	    free(paper);
	    if (landscape) {
		info->dvips_options = xstrcat(info->dvips_options, " -t landscape");
	    }
	}
    }

    /*
     * printer options
     */
    if (resource.dvips_printer_str != NULL)
	info->printer_options = xstrdup(resource.dvips_printer_str);
    else {
	/* initialize with default. We add the $PRINTER environment variable
	 * since dvips otherwise doesn't use it. */
	char *printer = getenv("PRINTER");
	info->printer_options = xstrdup("lpr");
	if (printer != NULL) {
	    info->printer_options = xstrcat(info->printer_options, " -P");
	    info->printer_options = xstrcat(info->printer_options, printer);
	}
    }

    info->print_target = resource.default_printing_target;
    info->fmt = resource.default_saving_format;

    if (pageinfo_have_marked_pages()) {
	info->page_selection = PAGE_MARKED;
    }
    else {
	info->page_selection = PAGE_ALL;
    }

    if (info->act == FILE_SAVE)
	extension = output_format_mapping[resource.default_saving_format].extension;
    else
	extension = ".ps";

    tmp = get_new_file_name(globals.dvi_name,
			    get_extension(globals.dvi_name),
			    extension);
    info->finfo->out_file = xmalloc(strlen(globals.cwd) + strlen(tmp) + 2); /* 1 extra for dir separator */
    sprintf(info->finfo->out_file, "%s/%s", globals.cwd, tmp);
    free(tmp);
    TRACE_FILES((stderr, "OUTFILE: |%s|\n", info->finfo->out_file));
}

static int
textfield_to_int(Widget w)
{
    char *s, *p;
    int pageno;
#ifndef MOTIF
    XtVaGetValues(w, XtNstring, &s, NULL);
#else /* MOTIF */
    s = XmTextFieldGetString(w);
#endif
    p = s;
    if (*p == '-')
	++p;
    if (!isdigit((int)*p)) {
	return 0;
    }
    do {
	++p;
    } while (isdigit((int)*p));
    if (*p != '\0')
	return 0;

    pageno = atoi(s) - globals.pageno_correct;
#ifdef MOTIF
    XtFree(s);
#endif
    return pageno;
}

/*
  Return a copy of the text value of Widget w in malloc'ed memory,
  or NULL if the text is empty.
*/

static char *
widgets_get_textfield(Widget w)
{
    char *ptr = NULL;
    XtVaGetValues(w,
#if MOTIF
		  XmNvalue,
#else
		  XtNstring,
#endif
		  &ptr, NULL);

    if (ptr == NULL || *ptr == '\0')
	return NULL;
    else
	return xstrdup(ptr);
}

static char *
widgets_get_textfield_by_name(const struct save_or_print_info *info, const char *widget_name)
{
    Widget w;
    if (get_widget_by_name(&w, info->shell, widget_name, True)) {
	return widgets_get_textfield(w);
    }
    return NULL;
}

static pageRadioT
widgets_get_page_selection(struct save_or_print_info *info)
{
    Widget radio_all;
#ifdef MOTIF
    Widget radio_marked;
#endif
    if (get_widget_by_name(&radio_all, info->shell, Xdvi_PAGES_ALL_RADIO_NAME, True)
#ifdef MOTIF
	&& get_widget_by_name(&radio_marked, info->shell, Xdvi_PAGES_MARKED_RADIO_NAME, True)
#endif
	) {
#ifdef MOTIF
	if (XmToggleButtonGadgetGetState(radio_all))
	    return PAGE_ALL;
	else if (XmToggleButtonGadgetGetState(radio_marked))
	    return PAGE_MARKED;
	else
	    return PAGE_RANGE;
#else
	return (pageRadioT)XawToggleGetCurrent(radio_all);
#endif
    }
    return NO_PAGE_VAL; /* dummy */
}

static int
widgets_get_from_page(struct save_or_print_info *info)
{
    Widget w;
    if (get_widget_by_name(&w, info->shell, Xdvi_PAGES_RANGE_FROM_TEXT_NAME, True)) {
	return textfield_to_int(w);
    }
    return 0;
}

static int
widgets_get_to_page(struct save_or_print_info *info)
{
    Widget w;
    if (get_widget_by_name(&w, info->shell, Xdvi_PAGES_RANGE_TO_TEXT_NAME, True)) {
	return textfield_to_int(w);
    }
    return 0;
}

static void
set_check_page_range(struct save_or_print_info *info, Boolean check)
{
    Widget text_from, text_to;

    if (get_widget_by_name(&text_from, info->shell, Xdvi_PAGES_RANGE_FROM_TEXT_NAME, True)
	&& get_widget_by_name(&text_to, info->shell, Xdvi_PAGES_RANGE_TO_TEXT_NAME, True)) {

	char page_string[LENGTH_OF_INT];

	if (check) { /* check validity of current values */
	    int curr_from = textfield_to_int(text_from);
	    int curr_to = textfield_to_int(text_to);

	    if (curr_from < total_pages && curr_to < total_pages) /* values are OK */
		return;
	}

	sprintf(page_string, "%d", current_page + globals.pageno_correct);

#ifdef MOTIF
	XtVaSetValues(text_from, XmNvalue, page_string, XmNcursorPosition, strlen(page_string), NULL);
	XtVaSetValues(text_to, XmNvalue, page_string, XmNcursorPosition, strlen(page_string), NULL);
#else
	XtVaSetValues(text_from, XtNstring, page_string, NULL);
	XtVaSetValues(text_to, XtNstring, page_string, NULL);
	XawTextSetInsertionPoint(text_from, strlen(page_string));
	XawTextSetInsertionPoint(text_to, strlen(page_string));
#endif
    }
}

static void
popdown_dialog_and_print_or_save(XtPointer myinfo)
{
    struct save_or_print_info *info = (struct save_or_print_info *)myinfo;
    
    cb_popdown(NULL, info, NULL);
    
    if (globals.dvi_file.bak_fp == NULL) { /* shouldn't happen */
	info->message_popup = popup_message(globals.widgets.top_level,
					    MSG_ERR, NULL,
					    "No active DVI file");
	return;
    }

    ASSERT(info->finfo->in_fp != NULL, "DVI input FILE * must have been set!");

    store_preference(NULL, "dvipsOptionsString", "%s", info->dvips_options ? info->dvips_options : "");
    if (info->act == FILE_PRINT) {
	store_preference(NULL, "defaultPrintingTarget", "%d", info->print_target);
	store_preference(NULL, "dvipsPrinterString", "%s", info->printer_options ? info->printer_options : "");
	internal_print(info);
    }
    else {
	store_preference(NULL, "defaultSavingFormat", "%d", info->fmt);
	internal_save(info);
    }
}

/* Clean up after user has aborted printing/saving */
static void
do_cleanup(XtPointer arg)
{
    struct save_or_print_info *info = (struct save_or_print_info *)arg;

    cancel_saving(info);
    if (info->finfo->tmp_dvi_file != NULL) {
	unlink(info->finfo->tmp_dvi_file);
	info->finfo->tmp_dvi_file = NULL;
    }
    if (info->finfo->tmp_ps_file != NULL) {
	unlink(info->finfo->tmp_ps_file);
	info->finfo->tmp_ps_file = NULL;
    }
}


/*
 * First round of sanity checks for page range etc.
 */
static void
print_check_page_values(struct save_or_print_info *info)
{
    struct select_pages_info *pinfo = info->pinfo;

    /* get current dvips and printer options */
    free(info->dvips_options);
    free(info->printer_options);
    info->dvips_options = info->printer_options = NULL;
    
    info->dvips_options = widgets_get_textfield_by_name(info, Xdvi_DVIPS_OPTIONS_TEXT);
    if (info->act == FILE_PRINT)
	info->printer_options = widgets_get_textfield_by_name(info, Xdvi_TO_PRINTER_TEXT);
    info->page_selection = widgets_get_page_selection(info);
    
    /* fprintf(stderr, "page selection: %d\n", info->page_selection); */
    
    if (info->page_selection == PAGE_ALL) {
	/* fprintf(stderr, "ALL!!!\n"); */
	pinfo->callback = NULL;
    }
    else if (info->page_selection == PAGE_MARKED) {
	/* fprintf(stderr, "MARKED!!!\n"); */
	pinfo->callback = check_marked;
    }
    else { /* PAGE_RANGE */
	/* fprintf(stderr, "RANGE!!!\n"); */
	pinfo->from = widgets_get_from_page(info);
	pinfo->to = widgets_get_to_page(info);
	pinfo->callback = check_pagerange;
	if (pinfo->from + 1 < 1) {
	    info->message_popup =
		popup_message(info->shell,
			      MSG_ERR,
			      "Please specify a valid page range.",
			      "Invalid page range: start (%d) < 1.",
			      pinfo->from + 1);
	    return;
	}
	else if (pinfo->from > pinfo->to) {
	    info->message_popup =
		popup_message(info->shell,
			      MSG_ERR,
			      "Please specify a valid page range.",
			      "Invalid page range: start (%d) > end (%d).",
			      pinfo->from + 1, pinfo->to + 1);
	    return;
	}
	else if (pinfo->to + 1 > total_pages) {
	    info->message_popup =
		popup_message(info->shell,
			      MSG_ERR,
			      "Please specify a valid page range.",
			      "Invalid page range: end (%d) > total_pages (%d).",
			      pinfo->to + 1, total_pages);
	    return;
	}
    }

    print_check_dvi_file(info);
}

/*
  Second round of sanity checks.
*/
static void
print_check_dvi_file(struct save_or_print_info *info)
{
    FILE *fin = NULL;

    /* Return if there are active popups; otherwise, the print process
       may get messed up */
    if (raise_message_windows()) {
	XBell(DISP, 0);
#if 0
	popup_message(info->shell,
		      MSG_WARN, NULL,
		      "Please close other open message windows first!");
#endif
	return;
    }

    /* If we want to print/save the marked pages, or save to a DVI file,
       we need to create a temporary DVI file containing the selected or all pages. */
    if (info->page_selection == PAGE_MARKED || info->fmt == FMT_DVI) {
	int tmp_fd;
	free(info->finfo->tmp_dvi_file);
	info->finfo->tmp_dvi_file = NULL;
	tmp_fd = xdvi_temp_fd(&(info->finfo->tmp_dvi_file)); /* this allocates info->finfo->tmp_dvi_file */

	if (tmp_fd == -1) {
	    info->message_popup =
		popup_message(info->shell,
			      MSG_ERR, NULL,
			      "Couldn't create temporary DVI file for printing: %s", strerror(errno));
	    return;
	}
	/* don't use XFOPEN here, since we don't want to treat an error in opening the file as fatal. */
	/* `b' in mode won't hurt ... */
	if ((info->finfo->tmp_dvi_fp = try_fdopen(tmp_fd, "wb+")) == NULL) { /* failure */
	    info->message_popup =
		popup_message(info->shell,
			      MSG_ERR,
			      "Xdvi needs to create a temporary file containing the "
			      "currently marked pages, but creating that file failed. "
			      "Try to fix the cause of the problem, or choose "
			      "\"Print selected pages\" as a workaround.",
			      "Could not open temp file for writing: %s.\n",
			      strerror(errno));
	    return;
	}
    }
    /* We try to use the original DVI file unless it's corrupted, in which case
       we ask the user if they want to save the cached copy instead */
    if ((fin = fopen(globals.dvi_name, "rb")) == NULL
	|| !find_postamble(fin, &(info->pinfo->errflag))) {
	Boolean could_not_open = False;
	if (fin == NULL)
	    could_not_open = True;
	if (fin)
	    fclose(fin);
	
	/* if we can't use the temporary file as source for our copy, this is a fatal error */
	if (!resource.use_temp_fp) {
	    info->message_popup =
		popup_message(info->shell,
			      MSG_ERR, NULL,
			      "Could not copy DVI file %s: The file %s",
			      globals.dvi_name, could_not_open ? "doesn't exist any more" : "seems to be corrupted");
	    return;
	}
	
	/* else, we'll try to use the cached copy of the DVI file */
	if ((fin = fopen(get_tmp_dvi_name(), "rb")) == NULL) {
	    info->message_popup =
		popup_message(info->shell,
			      MSG_ERR, NULL,
			      "Something's very wrong here - opening of both the "
			      "original DVI file and the cached copy failed!");
	    return;
	}
	
	if (!find_postamble(fin, &(info->pinfo->errflag))) {
	    info->message_popup =
		popup_message(info->shell,
			      MSG_ERR, NULL,
			      "Shouldn't happen: Cached copy of the DVI file seems corrupted!");
	    return;
	}
	info->finfo->in_fp = fin;
	
	/* tell user about it */
	info->message_popup =
	    choice_dialog(info->shell,
			  MSG_QUESTION, NULL,
#ifndef MOTIF
			  NULL, /* TODO: binding for RET? */
#endif /* MOTIF */
			  NULL, NULL, /* no pre_callbacks */
			  info->act == FILE_PRINT ? "Print Copy" : "Save Copy",
			  print_check_target_file, (XtPointer)info,
			  "Cancel", do_cleanup, (XtPointer)info,
			  "The DVI file %s %s. Do you want to %s a cached copy of the file?",
			  globals.dvi_name,
			  could_not_open ? "doesn't exist any more" : "seems to be corrupted",
			  info->act == FILE_PRINT ? "print" : "save");
    }
    else { /* original DVI file is OK, copy it */
	info->finfo->in_fp = fin;
	print_check_target_file((XtPointer)info);
    }
}

static void
print_check_target_file(XtPointer myinfo)
{
    struct save_or_print_info *info = (struct save_or_print_info *)myinfo;
    char *filename = widgets_get_textfield_by_name(info, Xdvi_TO_FILE_TEXT);
    char *expanded_filename;
    
    /* Return if there are active popups; otherwise, the print process
       will be messed up. */
    if (raise_message_windows()) {
	XBell(DISP, 0);
	return;
    }

    /* if we're just printing to printer, go ahead */
    if (!(info->act == FILE_SAVE || (info->act == FILE_PRINT && info->print_target == TO_FILE))) {
	popdown_dialog_and_print_or_save(info);
	return;
    }

    /* expand and canonicalize filename */
    if (filename == NULL) {
	info->message_popup =
	    popup_message(info->shell,
			  MSG_WARN,
			  NULL,
			  "No filename specified!");
	return;
    }
    expanded_filename = expand_homedir(filename); /* this allocates expanded_filename */
    
    if (expanded_filename == NULL) {
	info->message_popup =
	    popup_message(info->shell,
			  MSG_WARN,
			  "Please specify either a filename or a full path, without using \"~\" or \"~user\".",
			  "Couldn't expand filename \"%s\" to a full path.",
			  filename);
	free(filename);
	return;
    }
    free(filename);
    free(info->finfo->out_file);
    info->finfo->out_file = expanded_filename;
    
    if (info->act == FILE_SAVE && info->fmt == FMT_DVI) { /* canonicalize filename */
	char *tmp = expand_filename_append_dvi(info->finfo->out_file, USE_CWD_PATH, False); /* this mallocs tmp */
	if (strcmp(tmp, globals.dvi_name) == 0) {
	    info->message_popup =
		popup_message(info->shell,
			      MSG_ERR,
			      NULL,
			      "Cannot overwrite the current DVI file (%s). "
			      "Please choose a different file name.",
			      info->finfo->out_file);
	    free(tmp);
	    free(info->finfo->out_file);
	    info->finfo->out_file = NULL;
	    return;
	}
	free(info->finfo->out_file);
	info->finfo->out_file = tmp;
    }
    if (info->act == FILE_SAVE || (info->act == FILE_PRINT && info->print_target == TO_FILE)) {
	/* check whether to clobber existing file */
	struct stat statbuf;
	if (stat(info->finfo->out_file, &statbuf) == 0 && S_ISREG(statbuf.st_mode)) {
	    info->message_popup =
		choice_dialog(info->shell,
			      MSG_QUESTION, NULL,
#ifndef MOTIF
			      NULL, /* TODO: binding for RET */
#endif
			      NULL, NULL, /* no pre_callbacks */
			      "Replace", popdown_dialog_and_print_or_save, (XtPointer)info,
			      /*
				do nothing if user selects `cancel'; this will return to the
				printing dialog, since user hasn't changed their mind about
				printing, but would probably just like to choose another filename
			      */
			      "Cancel", NULL, (XtPointer)NULL,
			      "The file %s already exists.\nDo you want to replace it?",
			      info->finfo->out_file);
	    return;
	}
	else {
	    popdown_dialog_and_print_or_save(info);
	}
    }
    else {
	popdown_dialog_and_print_or_save(info);
    }
}

static void
notify_dialog(Widget w, pageRadioT *value_bak)
{
    Widget radio_all, radio_marked;
#ifdef MOTIF
    Widget radio_range;
#endif
    Boolean have_marked = pageinfo_have_marked_pages();
    pageRadioT value_curr;

    /*     fprintf(stderr, "have_marked: %d\n", have_marked); */
    
    if (get_widget_by_name(&radio_all, w, Xdvi_PAGES_ALL_RADIO_NAME, True)
	&& get_widget_by_name(&radio_marked, w, Xdvi_PAGES_MARKED_RADIO_NAME, True)
#ifdef MOTIF
	&& get_widget_by_name(&radio_range, w, Xdvi_PAGES_RANGE_RADIO_NAME, True)
#endif
	) {

	/* get the current value */
#ifdef MOTIF
	if (XmToggleButtonGadgetGetState(radio_all))
	    value_curr = PAGE_ALL;
	else if (XmToggleButtonGadgetGetState(radio_marked))
	    value_curr = PAGE_MARKED;
	else
	    value_curr = PAGE_RANGE;
#else
	value_curr = (pageRadioT)XawToggleGetCurrent(radio_all);
#endif

	/* fprintf(stderr, "current value: %d\n", value_curr); */
	
	if (value_curr == PAGE_MARKED && have_marked)
	    return;
	if (value_curr != PAGE_MARKED && !have_marked) {
	    XtVaSetValues(radio_marked,
#ifdef MOTIF
			  XmNsensitive, 
#else
			  XtNsensitive,
#endif
			  False, NULL);
	    return;
	}
    
	/* otherwise, if we have marked pages now, we want to switch from
	   the current value to `marked', saving the current value in *value_bak. */

	/* XXX FIXME: There's a bug with OpenMotif 2.1 if the following order is used:
	   - without marked pages, open dialog
	   - mark a page; button `marked' is selected
	   - select button `all'
	   - select button `range'
	   - unmark page; `marked' becomes insensitive
	   - mark page, `marked' becomes sensitive and is selected, `range' is unselected
	   - up to here, everything is as expected, but if you now select `range',
	   it doesn't work, no button changes its state. As a workaround, if you select
	   `marked' again and then `range', it works. Strangely, the callback when unsetting
	   `range' is invoked correctly, so the button should be in the correct internal state ...
	   I think this is an OpenMotif bug.
	*/	   
	if (have_marked) {
	    *value_bak = value_curr;
#ifdef MOTIF
	    if (value_curr == PAGE_ALL) {
		XmToggleButtonGadgetSetState(radio_all, False, True);
	    }
	    else {
		XmToggleButtonGadgetSetState(radio_range, False, True);
	    }
	    XmToggleButtonGadgetSetState(radio_marked, True, True);
	    XtVaSetValues(radio_marked, XmNsensitive, True, NULL);
	    XmProcessTraversal(radio_marked, XmTRAVERSE_CURRENT);
#else
	    XawToggleSetCurrent(radio_all, cast_int_to_XtPointer(PAGE_MARKED));
	    XtVaSetValues(radio_marked, XtNsensitive, True, NULL);
#endif
	}
	else {
	    /* no marked pages now; switch back to the value from *value_bak. */
	    /* fprintf(stderr, "switching back to button %d\n", *value_bak); */
#ifdef MOTIF
	    if (*value_bak == PAGE_ALL) {
		XmToggleButtonGadgetSetState(radio_all, True, True);
		XmProcessTraversal(radio_all, XmTRAVERSE_CURRENT);
	    }
	    else {
		XmToggleButtonGadgetSetState(radio_range, True, True);
		XmProcessTraversal(radio_range, XmTRAVERSE_CURRENT);
	    }
	    XmToggleButtonGadgetSetState(radio_marked, False, True);
	    XtVaSetValues(radio_marked, XmNsensitive, False, NULL);
#else
	    XawToggleSetCurrent(radio_all, (XtPointer)*value_bak);
	    XtVaSetValues(radio_marked, XtNsensitive, *value_bak == PAGE_MARKED, NULL);
#endif
	}
    }
}

/* access from pagesel.c if user marks or unmarks a page in the page list */
void
notify_print_dialog_have_marked(void)
{
    Widget print_shell, save_shell;
    static pageRadioT save_value_bak = PAGE_ALL;
    static pageRadioT print_value_bak = PAGE_ALL;
    
    /* notify *both* windows, save and print, but only if they already exist */
    if (get_widget_by_name(&print_shell, globals.widgets.top_level, Xdvi_PRINT_SHELL_NAME, False))
	notify_dialog(print_shell, &save_value_bak);
	
    if (get_widget_by_name(&save_shell, globals.widgets.top_level, Xdvi_SAVE_SHELL_NAME, False))
	notify_dialog(save_shell, &print_value_bak);
}

/* main entry point: pop up the dialog */
void
save_or_print_callback(struct save_or_print_info *info)
{
    if (info->shell != NULL && window_is_mapped(XtWindow(info->shell), DISP)) {
	XBell(DISP, 0);
	XRaiseWindow(DISP, XtWindow(info->shell));
	return;
    }
    
    if (info->act == FILE_PRINT && printlog_raise_active(info)) {
	XBell(DISP, 0);
	return;
    }

    if (globals.dvi_file.bak_fp == NULL) {
	popup_message(globals.widgets.top_level,
		      MSG_ERR,
		      NULL,
		      "Empty or incomplete DVI file. "
		      "Please select a new DVI file via File -> Open.");
	return;
    }

    if (info->shell == NULL) { /* first time */
	get_initial_values(info);
#ifdef MOTIF
	motif_create_dialog(info);
#else
	xaw_create_dialog(info);
#endif
	/* fprintf(stderr, "SHELL IS: %p\n", (void *)info->shell); */
	set_check_page_range(info, False); /* set initial values */

	if (info->act == FILE_SAVE)
	    update_dvips_options_sensitivity(resource.default_saving_format, info->shell);
    }
    else {
	set_check_page_range(info, False); /* was: True, use False to reset values to current page */
    }
    
    /* map the dialog */
    XtPopup(info->shell, XtGrabNone);
}

