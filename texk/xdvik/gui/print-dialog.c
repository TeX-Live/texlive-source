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
  SU: Adapted from non-k xdvi's popups.c

  This implements a print popup (and a `save to file' popup).  My idea
  was to use the same window layout framework for both tasks, since
  there's a big overlap in the widgets needed.  However after looking
  at the mess I've created, I'm no longer convinced this was a good
  idea. (I reckon it's slightly better than the code duplication when
  doing it in separate files, but only *slightly* ...)
  
  We need a more intelligent approach that bans this ugly kind of
  `data proliferation' (those global arrays of widgets and helper
  structures).  Custom Widgets? Factory methods? Needs investigation.

  Also, in the long run, I guess we want the `Save' window to be more
  similar to the file browser (i.e. to contain a file browser e.g.
  in the upper part); in that case, the code will be moved into another
  module anyway.

  Moreover, the GUI code should be completely separated from the backend
  code.  
*/

#include <string.h>
#include "xdvi-config.h"
#include "xdvi.h"
#include "my-vsnprintf.h"
#include <ctype.h>

#include "print-dialog.h"
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

typedef enum printRadioT_ {
    NO_PRINT_VAL = -1,
    PRINT_TO_PRINTER = 1,
    PRINT_TO_FILE,
    SAVE_TO_FILE
} printRadioT;


typedef enum pageRadioT_ {
    NO_PAGE_VAL = -1,
    PRINT_ALL = 1,
    PRINT_MARKED,
    PRINT_RANGE,
    SAVE_ALL,
    SAVE_MARKED,
    SAVE_RANGE
} pageRadioT;

static Boolean is_print_button(pageRadioT flag) {
    return (flag <= PRINT_RANGE);
}

struct dialog_info {
    char *printer_name;
    char *file_name;
    /* fully canonicalized filename, not visible in window, but used in error messages.
       It is either NULL, or a malloc()ed buffer (which is free()d immediately before use). */
    char *expanded_filename;
    char *dvips_options;
    char *from_page;
    char *to_page;
    printRadioT print_radio;
    pageRadioT range_radio;
    outputFormatT output_format;
};

/*
  global widgets, and arrays of global widgets (mosly, one
  for the printing and one for the saving dialog). Needs
  cleanup!
*/

#define FORMAT_SEL_CNT 5

/* top-level widgets: one for printing, one for saving to file */
static Widget print_shell = NULL;
static Widget save_shell = NULL;

/* global structures for saving print info between invocations */
static struct dialog_info save_pr_info = {
    NULL, NULL, NULL, NULL, NULL, NULL, NO_PRINT_VAL, NO_PAGE_VAL, FMT_NONE
};
static struct dialog_info save_save_info = {
    NULL, NULL, NULL, NULL, NULL, NULL, NO_PRINT_VAL, NO_PAGE_VAL, FMT_NONE
};

/*
  these contain the currently selected values, or defaults;
  used in communication between callbacks
*/
static struct dialog_info curr_pr_info = {
    NULL, NULL, NULL, NULL, NULL, NULL, PRINT_TO_PRINTER, PRINT_ALL, FMT_NONE
};
static struct dialog_info curr_save_info = {
    NULL, NULL, NULL, NULL, NULL, NULL, SAVE_TO_FILE, SAVE_ALL, FMT_NONE
};

static void print_precheck1(XtPointer info);
static void print_precheck2(XtPointer info);

/* access methods ... */
const char *
get_printer_options(void)
{
    return save_pr_info.printer_name;
}

char *
get_dvips_options(printOrSaveActionT act)
{
    if (act == FILE_PRINT)
	return save_pr_info.dvips_options;
    else
	return save_save_info.dvips_options;
}

#ifdef MOTIF
static Widget from_label[FILE_ALL] = { None, None };
static Widget format_selection_option;
XmString format_selection_texts[FORMAT_SEL_CNT];

#else /* MOTIF */

static Widget format_selection_label;
static Widget format_selection_button;
#if 0
static Widget format_selection_b1;
#endif
static Widget format_selection_popup;
static Widget format_selection_menu[FORMAT_SEL_CNT];

static void xaw_print_save_act_go(Widget, XEvent *, String *, Cardinal *);

#endif /* MOTIF */

static void print_cancel(Widget, XEvent *, String *, Cardinal *);
static void save_cancel(Widget, XEvent *, String *, Cardinal *);

static Widget to_printer_radio;
static Widget to_file_radio_or_label[FILE_ALL] = { None, None };
static Widget to_label[FILE_ALL] = { None, None };
static Widget of_label[FILE_ALL] = { None, None };
static Widget range_all_radio[FILE_ALL] = { None, None };
static Widget range_marked_radio[FILE_ALL] = { None, None };
static Widget range_from_to_radio[FILE_ALL] = { None, None };
static Widget cancel_button[FILE_ALL] = { None, None };
static Widget ok_button[FILE_ALL] = { None, None };
static Widget print_to_printer_text;
static Widget print_to_file_text[FILE_ALL] = { None, None };
static Widget print_to_file_button[FILE_ALL] = { None, None };
static Widget dvips_options_text[FILE_ALL] = { None, None };
static Widget page_from_text[FILE_ALL] = { None, None };
static Widget page_to_text[FILE_ALL] = { None, None };

static int print_is_active = False;	/* False if window is unmanaged or not created yet, True if managed */
static int save_is_active = False;	/* False if window is unmanaged or not created yet, True if managed */


static XtActionsRec print_actions[] = {
#ifndef MOTIF
    {"saveInternal",		xaw_print_save_act_go },
    {"printInternal",		xaw_print_save_act_go},
#endif
    {"WM_print_cancel",		print_cancel },
    {"WM_save_cancel",		save_cancel  },
};

/*
  ============================================================
  generic utility functions
  ============================================================
*/

/*
  Return the printer name, eventually followed by `-P$PRINTER',
  since the `-o' option of dvips doesn't evaluate the environment
  variable.
*/
static char *
get_printer_name(void)
{
    char *printcmd, *printer;

    if (resource.dvips_printer_str != NULL && strlen(resource.dvips_printer_str) > 0) {
	return xstrdup(resource.dvips_printer_str);
    }
    printcmd = xstrdup("lpr");
    printer = getenv("PRINTER");
    if (printer != NULL) {
	printcmd = xstrcat(printcmd, " -P");
	printcmd = xstrcat(printcmd, printer);
    }
    return printcmd;
}

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
    strcpy(new_name + len - strlen(old_ext), new_ext);
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
    *landscape = False;
    if (papersize[strlen(papersize) - 1] == 'r') { /* landscape size */
	papersize[strlen(papersize) - 1] = '\0';
	*landscape = True;
    }
    TRACE_GUI((stderr, "Using dvips arguments: `%s', %s", papersize, *landscape ? "landscape" : "portrait"));
    return papersize;
}

static void
set_outfile_name(struct dialog_info *info, struct dialog_info *save_info, printOrSaveActionT act)
{
    free(info->file_name);
    if (act == FILE_PRINT) {
	info->file_name = get_new_file_name(globals.dvi_name, ".dvi", ".ps");
    }
    else { /* saving to file */
	if (save_info->range_radio == SAVE_ALL) {
	    info->file_name = get_new_file_name(globals.dvi_name, ".dvi",
						output_format_mapping[resource.default_saving_format].extension);
	}
	else {
	    char *tmp = xstrdup("_pp");
	    tmp = xstrcat(tmp, output_format_mapping[resource.default_saving_format].extension);
	    
	    info->file_name = get_new_file_name(globals.dvi_name, ".dvi", tmp);
	    free(tmp);
	}
    }
}

/* get initial values for dialog texts */
static void
get_initial_dialog_values(struct dialog_info *info, printOrSaveActionT act)
{
    if (act == FILE_PRINT && info->printer_name == NULL)
	    info->printer_name = get_printer_name();

    if (info->dvips_options == NULL) {
	info->dvips_options = xstrdup("");
	
	/* add -t option if not already present */
	if (resource.dvips_options_str != NULL
	    && strstr(resource.dvips_options_str, "-t ") == NULL) {
	    Boolean dvips_landscape = False;
	    char *dvips_paper_size = get_dvips_papersize(&dvips_landscape);
	    if (dvips_paper_size != NULL) {
		info->dvips_options = xstrdup("-t ");
		info->dvips_options = xstrcat(info->dvips_options, dvips_paper_size);
		free(dvips_paper_size);
		if (dvips_landscape) {
		    info->dvips_options = xstrcat(info->dvips_options, " -t landscape");
		}
	    }
	}
	
	if (resource.dvips_options_str != NULL) {
	    if (strlen(info->dvips_options) > 0)
		info->dvips_options = xstrcat(info->dvips_options, " ");
	    info->dvips_options = xstrcat(info->dvips_options, resource.dvips_options_str);
	}
    }
    
    if (info->from_page == NULL) {
	info->from_page = xmalloc(LENGTH_OF_INT + 1);
	sprintf(info->from_page, "%d", current_page + globals.pageno_correct);
    }

    if (info->to_page == NULL) {
	info->to_page = xmalloc(LENGTH_OF_INT + 1);
	sprintf(info->to_page, "%d", current_page + globals.pageno_correct);
    }
}


#if MOTIF
static void
set_sensitivity(int i)
{
    Boolean sensitivity;
    
/*     if (i == curr_pr_info.print_radio) /\* if no change *\/ */
/* 	return; */

    curr_pr_info.print_radio = i;

    sensitivity = (i == PRINT_TO_PRINTER);
    XtSetSensitive(print_to_printer_text, sensitivity);

/*     sensitivity ^= (True ^ False); */
    XtSetSensitive(print_to_file_text[FILE_PRINT], !sensitivity);
    XtSetSensitive(print_to_file_button[FILE_PRINT], !sensitivity);
}
#endif /* MOTIF */

static void
update_dvips_options_sensitivity(int idx, Widget top)
{
    Widget dvips_label;

#if MOTIF    
    Widget dvips_options;

    if (get_widget_by_name(&dvips_label, top, "dvips_options_name", True) &&
	get_widget_by_name(&dvips_options, top, "dvips_options_text", True)) {
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
    if (get_widget_by_name(&dvips_label, top, "dvips_options_name", True)) {
	XtSetSensitive(XtParent(dvips_label),
		       output_format_mapping[idx].fmt == FMT_PS ||
		       output_format_mapping[idx].fmt == FMT_PS2PDF);
    }
#endif
}

/* callbacks */

static void
select_format_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    Widget file_text, top;
    
    char *filename;
    int i;
#ifdef MOTIF
    Widget f1, f2;
    char *ptr;
    
    i = (int)client_data;
    UNUSED(call_data);
    f1 = XtParent(w);
    ASSERT(f1 != 0, "Parent of widget musn't be NULL!\n");
    f2 = XtParent(f1);
    ASSERT(f2 != 0, "Parent of f1 musn't be NULL!\n");
    top = XtParent(f2);
    ASSERT(top != 0, "Parent of f2 musn't be NULL!\n");

    if ((file_text = XtNameToWidget(top, "*file_text")) == 0) {
	XDVI_WARNING((stderr, "Couldn't find \"file_text\" widget!"));
	return;
    }
    XtVaGetValues(file_text, XmNvalue, &filename, NULL);
    if ((ptr = strrchr(filename, '.')) != NULL) {
	char buf[1024];
	replace_extension(filename, output_format_mapping[i].extension, buf, sizeof buf);
	XtVaSetValues(file_text, XmNvalue, buf, NULL);
    }
#else /* MOTIF */
    
    Widget popup, button;
    char *ptr1, *ptr2;
    char buf[1024];
    
    UNUSED(call_data);
    popup = XtParent(w);
    ASSERT(popup != 0, "Parent of menu musn't be NULL!\n");
    button = XtParent(popup);
    ASSERT(button != 0, "Parent of popup musn't be NULL!\n");

    top = (Widget)client_data;

    if ((file_text = XtNameToWidget(top, "*file_text")) == 0) {
	XDVI_WARNING((stderr, "Couldn't find \"file_text\" widget!"));
	return;
    }

    XtVaGetValues(file_text, XtNstring, &filename, NULL);
/*     fprintf(stderr, "file text: |%s|\n", filename); */

    /* swap title strings */
    XtVaGetValues(button, XtNlabel, &ptr1, NULL);
    XtVaGetValues(w, XtNlabel, &ptr2, NULL);
    strncpy(buf, ptr1, sizeof buf);

    for (i = 0; output_format_mapping[i].fmt_string != NULL; i++) {
	if (strcmp(ptr2, output_format_mapping[i].fmt_string) == 0) {
	    char buf2[1024];
	    replace_extension(filename, output_format_mapping[i].extension, buf2, sizeof buf2);
	    XtVaSetValues(file_text, XtNstring, buf2, NULL);
	    
	    break;
	}
    }
    if (output_format_mapping[i].fmt_string == NULL) {
	popup_message(get_matching_parent(w, globals.widgets.top_level, "print_popup", "save_popup", NULL),
		      MSG_WARN,
		      REPORT_XDVI_BUG_TEMPLATE,
		      "Unrecognized string in format selector: |%s|", ptr2);
    }
    XtVaSetValues(button, XtNlabel, ptr2, NULL);
    XtVaSetValues(w, XtNlabel, buf, NULL);

#endif /* MOTIF */

    save_save_info.output_format = curr_save_info.output_format = output_format_mapping[i].fmt;

    update_dvips_options_sensitivity(i, save_shell);
    
    resource.default_saving_format = save_save_info.output_format;
    set_outfile_name(&save_save_info, &curr_save_info, FILE_SAVE);
}

static void
cb_print_cancel(Widget w, XtPointer client_data, XtPointer call_data)
{
    struct save_or_print_info *info = (struct save_or_print_info *)client_data;
    UNUSED(w);
    UNUSED(call_data);

    ASSERT(info != NULL, "client_data in cb_print_cancel musn't be NULL!");
    if (info->message_popup != 0) {
	kill_message_window(info->message_popup);
    }
    XtPopdown(print_shell);
    print_is_active = False;
}

static void
cb_save_cancel(Widget w, XtPointer client_data, XtPointer call_data)
{
    struct save_or_print_info *info = (struct save_or_print_info *)client_data;
    UNUSED(w);
    UNUSED(call_data);
    
    ASSERT(info != NULL, "client_data in cb_save_cancel musn't be NULL!");
    if (info->message_popup != 0) {
	kill_message_window(info->message_popup);
    }
    XtPopdown(save_shell);
    save_is_active = False;
}

static void
cb_print_or_save(Widget w, XtPointer client_data, XtPointer call_data)
{
    struct save_or_print_info *info = (struct save_or_print_info *)client_data;
    UNUSED(w);
    UNUSED(call_data);

    print_precheck1((XtPointer)info);
}

/* access from outside the module */
void
cancel_saving(struct save_or_print_info *info)
{
    cb_save_cancel(NULL, info, NULL);
}


static void
print_cancel(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    struct save_or_print_info *info = NULL;
    void *ptr;
    
    UNUSED(w);
    UNUSED(event);

    ASSERT(*num_params > 0, "params in print_cancel must be > 0!");
    ASSERT(*params != NULL, "params in print_cancel mustn't be NULL!");

    TRACE_GUI((stderr, "Pointer string value: |%s|", *params));
    sscanf(*params, "%p", &ptr);
    info = (struct save_or_print_info *)ptr;

    cb_print_cancel(NULL, info, NULL);
}
    
static void
save_cancel(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    struct save_or_print_info *info = NULL;
    void *ptr;
    
    UNUSED(w);
    UNUSED(event);

    ASSERT(*num_params > 0, "params in save_cancel must be > 0!");
    ASSERT(*params != NULL, "params in save_cancel mustn't be NULL!");

    TRACE_GUI((stderr, "Pointer string value: |%s|", *params));
    sscanf(*params, "%p", &ptr);
    info = (struct save_or_print_info *)ptr;

    cb_save_cancel(NULL, info, NULL);
}

#ifndef MOTIF

/*
  enable the `print to' button if user presses key or mouse button
  inside that input field
*/
static void
xaw_print_handle_key(Widget widget, XtPointer closure, XEvent *ev, Boolean *cont)
{
    char *number;
    int i;

    number = (char *)closure;
    i = atoi(number);
    ASSERT(i == 2 || i == 3, "Button index should be 2 or 3");
    UNUSED(widget);
    UNUSED(ev);
    UNUSED(cont);

    if (i == 2) {
	XawToggleSetCurrent(to_printer_radio, (XtPointer)PRINT_TO_PRINTER);
    }
    else {
	ASSERT(to_file_radio_or_label[FILE_PRINT] != NULL, "");
	XawToggleSetCurrent(to_file_radio_or_label[FILE_PRINT], (XtPointer)PRINT_TO_FILE);
    }
}

/*
  enable the `print range' button if user presses key or mouse button
  inside that input field
*/
static void
xaw_range_handle_key(Widget widget, XtPointer closure, XEvent *ev, Boolean *cont)
{
    printOrSaveActionT act = (printOrSaveActionT)closure;
    UNUSED(widget);
    UNUSED(ev);
    UNUSED(cont);
    
    if (act == FILE_PRINT && curr_pr_info.range_radio != PRINT_RANGE) {
	ASSERT(range_all_radio[act] != NULL, "button should have been initialized");
	XawToggleSetCurrent(range_all_radio[act], (XtPointer)PRINT_RANGE);
    }
    else if (act == FILE_SAVE && curr_save_info.range_radio != SAVE_RANGE) {
	ASSERT(range_all_radio[act] != NULL, "button should have been initialized");
	XawToggleSetCurrent(range_all_radio[act], (XtPointer)SAVE_RANGE);
    }
}


static void
xaw_print_save_act_go(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    struct save_or_print_info *info = NULL;
    void *ptr;
    
    UNUSED(w);
    UNUSED(event);

    ASSERT(*num_params > 0, "params in search_cancel must be > 0!");
    ASSERT(*params != NULL, "params in search_cancel mustn't be NULL!");

    TRACE_GUI((stderr, "Pointer string value: |%s|", *params));
    sscanf(*params, "%p", &ptr);
    info = (struct save_or_print_info *)ptr;

    print_precheck1((XtPointer)info);
}

#if 0
static void
xaw_popup_menu_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    Position x, y;
    Dimension h;
    int pos_x, pos_y;
    Window dummy;
    
    Widget popup = (Widget)client_data;
    fprintf(stderr, "popup: %p\n", popup);
    XtVaGetValues(w, XtNx, &x, XtNy, &y, XtNheight, &h, NULL);
    fprintf(stderr, "x: %d, y: %d, h: %d\n", x, y, h);
    XTranslateCoordinates(DISP, XtWindow(w), RootWindowOfScreen(SCRN),
			  x, y, &pos_x, &pos_y, &dummy);
    XtRealizeWidget(popup);
    /* WRONG - would need to use x position of its left widget ... */
    XtVaSetValues(popup, XtNx, pos_x, XtNy, pos_y + h + 1, NULL);
    XtPopup(popup, XtGrabNone);
}
#endif /* 0 */

#endif /* not MOTIF */

static void
cb_print_vs_file(Widget w, XtPointer client_data, XtPointer call_data)
{
#ifdef MOTIF
    Boolean to_printer = False, to_file = False;
    if (((XmToggleButtonCallbackStruct *)call_data)->set) {
	if (w == to_printer_radio)
	    to_printer = True;
	else
	    to_file = True;
	
    }
    else {
	if (w == to_printer_radio)
	    to_file = True;
	else
	    to_printer = True;
    }

    XmToggleButtonGadgetSetState(to_file_radio_or_label[FILE_PRINT], to_file, False);
    XmToggleButtonGadgetSetState(to_printer_radio, to_printer, False);
    
    set_sensitivity((int)client_data);
    XmProcessTraversal(curr_pr_info.print_radio == PRINT_TO_PRINTER
		       ? print_to_printer_text
		       : print_to_file_text[FILE_PRINT],
		       XmTRAVERSE_CURRENT);
#else /* MOTIF */
    int i = (int) XawToggleGetCurrent(to_printer_radio);

    UNUSED(w);
    UNUSED(client_data);
    UNUSED(call_data);

    /*
      NOTE: Unlike non-k xdvi, we don't use set_sensitivity for the
      Xaw version, since the text inside the textfield cannot be made
      insensitive, so it won't be obvious to users why they cannot
      change the text.  Also, the page range isn't set insensitive
      either.
    */
    if (i != 0)
	curr_pr_info.print_radio = i;
#endif /* MOTIF */
    
}



static void
cb_range(Widget w, XtPointer client_data, XtPointer call_data)
{
    pageRadioT flag = (pageRadioT)client_data;
    printOrSaveActionT act;
    
#ifdef MOTIF
    
    Boolean sensitivity;
    UNUSED(call_data);
    ASSERT(flag != PRINT_MARKED || flag != SAVE_MARKED || pageinfo_have_marked_pages(),
	   "PRINT_MARKED or SAVE_MARKED shouldn't be togglable if there are no marked pages!");

    if (is_print_button(flag)) {
	curr_pr_info.range_radio = (int)client_data;
	sensitivity = (curr_pr_info.range_radio == PRINT_RANGE);
	act = FILE_PRINT;
    }
    else {
	curr_save_info.range_radio = (int)client_data;
	sensitivity = (curr_save_info.range_radio == SAVE_RANGE);
	act = FILE_SAVE;
    }

    /* switch off other radio buttons, switch on current one */
    XmToggleButtonGadgetSetState(range_all_radio[act], False, False);
    XmToggleButtonGadgetSetState(range_marked_radio[act], False, False);
    XmToggleButtonGadgetSetState(range_from_to_radio[act], False, False);
    XmToggleButtonGadgetSetState(w, True, False);

    /* set from-to range to sensitive if appropriate */
    XtSetSensitive(from_label[act], sensitivity);
    XtSetSensitive(page_from_text[act], sensitivity);
    XtSetSensitive(to_label[act], sensitivity);
    XtSetSensitive(page_to_text[act], sensitivity);
    XtSetSensitive(of_label[act], sensitivity);
    
#else /* MOTIF */
    
    int i;
    UNUSED(call_data);
    if (is_print_button(flag)) {
	act = FILE_PRINT;
    }
    else {
	act = FILE_SAVE;
    }

    UNUSED(w);

    i = (int) XawToggleGetCurrent(range_all_radio[act]);

    if (i != 0) {
	ASSERT(i != PRINT_MARKED || i != SAVE_MARKED || pageinfo_have_marked_pages(),
	       "PRINT_MARKED or SAVE_MARKED shouldn't be togglable if there are no marked pages!");

	if (act == FILE_PRINT)
	    curr_pr_info.range_radio = i;
	else
	    curr_save_info.range_radio = i;
    }
    
#endif /* MOTIF */
}

static void
set_filename_callback(const char *fname, void *data)
{
    if (fname != NULL) {
	Widget w = (Widget)data;
	XtVaSetValues(w,
#ifdef MOTIF
		      XmNvalue, fname,
		      XmNcursorPosition, strlen(fname),
#else
		      XtNstring, fname,
#endif
		      NULL);
    }
}


static void
cb_browse(Widget w, XtPointer client_data, XtPointer call_data)
{
/*      char *fname; */
    struct save_or_print_info *info = (struct save_or_print_info *)client_data;
    printOrSaveActionT act = info->act;
/*      Widget ret_widget; */
    Widget parent;
    
    UNUSED(call_data);
#ifdef MOTIF
    UNUSED(w);
#else
    if (act == FILE_PRINT)
	xaw_print_handle_key(w, (XtPointer)"3", NULL, NULL);
#endif
    
    if (act == FILE_SAVE) {
	static struct filesel_callback cb; /* static so that we can pass its address */
	cb.title = "Xdvi: Save to file";
	cb.prompt = "Save to file:";
	cb.ok = "OK";
	cb.cancel = "Cancel";
	cb.init_path = NULL;
	cb.filemask = "*.dvi";
	cb.must_exist = False;
	cb.exit_on_cancel = False;
	cb.func_ptr = set_filename_callback;
	cb.data = print_to_file_text[act];
	
	if (!get_widget_by_name(&parent, globals.widgets.top_level, Xdvi_SAVE_SHELL_NAME, True))
	    parent = globals.widgets.top_level;
	XsraSelFile(parent, &cb);
/* 	fprintf(stderr, "done saving: return widget = %p\n", ret_widget); */
    }
    else {
	static struct filesel_callback cb; /* static so that we can pass its address */
	cb.title = "Xdvi: Print to file";
	cb.prompt = "Print to file:";
	cb.ok = "OK";
	cb.cancel = "Cancel";
	cb.init_path = NULL;
	cb.filemask = "*.ps";
	cb.must_exist = False;
	cb.exit_on_cancel = False;
	cb.func_ptr = set_filename_callback;
	cb.data = print_to_file_text[act];
	
	if (!get_widget_by_name(&parent, globals.widgets.top_level, Xdvi_PRINT_SHELL_NAME, True))
	    parent = globals.widgets.top_level;
	XsraSelFile(parent, &cb);
/*  	fname = XsraSelFile(parent, "Xdvi: Print to file", "Print to file:", */
/*  			    "OK", "Cancel", */
/*  			    NULL, */
/*  			    "*.ps", False, &ret_widget); */
/* 	fprintf(stderr, "done printing: return widget = %p\n", ret_widget); */
    }
    
/*      if (fname != NULL) { */
/*  	XtVaSetValues(print_to_file_text[act], */
/*  #ifdef MOTIF */
/*  		      XmNvalue, fname, XmNcursorPosition, strlen(fname), */
/*  #else */
/*  		      XtNstring, fname, */
/*  #endif */
/*  		      NULL); */
/*      } */
/*      free(fname); */
}

/* create a popup dialog for printing a DVI file OR saving pages from a DVI file */
static Widget
create_print_or_save_window(struct save_or_print_info *info, char *ofstring)
{
    printOrSaveActionT act = info->act;
    Widget top_level_shell;
    Atom WM_DELETE_WINDOW;
#ifndef MOTIF
    Widget form, paned, box;
    Widget f01form, f02form; /* dummy forms to get indentation consistent */
    Widget r1form;
    Widget r1label;
    Widget r2form, r3form, r4form = 0;
    Widget r4label;
    Widget r5label;
    Widget r6form;
    Widget r61form;
    Widget r7form;
    char *str;

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

    if (act == FILE_SAVE) {
	str = get_string_va("<Message>WM_PROTOCOLS: WM_save_cancel(%p)\n"
			    "<Key>Escape:WM_save_cancel(%p)\n"
			    "<Key>q:WM_save_cancel(%p)",
			    info, info, info);
	wm_translations = XtParseTranslationTable(str);
	free(str);
    }
    else {
	str = get_string_va("<Message>WM_PROTOCOLS: WM_print_cancel(%p)\n"
			    "<Key>Escape:WM_print_cancel(%p)\n"
			    "<Key>q:WM_print_cancel(%p)",
			    info, info, info);
	wm_translations = XtParseTranslationTable(str);
	free(str);
    }

    top_level_shell = XtVaCreatePopupShell(act == FILE_SAVE ? Xdvi_SAVE_SHELL_NAME : Xdvi_PRINT_SHELL_NAME,
					   transientShellWidgetClass,
					   globals.widgets.top_level,
					   XtNtitle, act == FILE_SAVE ? "xdvik: Save file" : "xdvik: Print DVI file",
					   XtNmappedWhenManaged, False,
					   XtNtransientFor, globals.widgets.top_level,
					   XtNallowShellResize, True,
					   XtNtranslations, wm_translations,
					   NULL);

    WM_DELETE_WINDOW = XInternAtom(XtDisplay(top_level_shell), "WM_DELETE_WINDOW", False);

    paned = XtVaCreateManagedWidget(act == FILE_SAVE ? "save_paned" : "print_paned",
				    panedWidgetClass, top_level_shell, NULL);
	
    form = XtVaCreateManagedWidget("form", formWidgetClass,
				   paned,
				   XtNallowResize, True,
				   NULL);
    XtVaGetValues(form, XtNdefaultDistance, &ddist, NULL);
/*     fprintf(stderr, "form1: %ld\n", (long)form); */
    
    xlats = XtParseTranslationTable("<EnterWindow>:highlight(Always)\n"
				    "<LeaveWindow>:unhighlight()\n"
				    "<Btn1Down>,<Btn1Up>:set()notify()");

    if (act == FILE_PRINT) {
	str = get_string_va("<Key>Return:printInternal(%p)", info);
	xlats2 = XtParseTranslationTable(str);
    }
    else {
	str = get_string_va("<Key>Return:saveInternal(%p)", info);
	xlats2 = XtParseTranslationTable(str);
    }
    free(str);

    if (act == FILE_PRINT) { /* selection `to printer/to file' */
	/* FIXME: set real sizes here */
/*  	Dimension w_curr; */
	f01form = XtVaCreateManagedWidget("dummy_print_to_label_form", formWidgetClass,
					  form,
					  XtNresizable, True,
					  XtNborderWidth, 0,
					  /* 					  XtNvertDistance, 10, */
					  HORIZONTAL_RESIZING_YES,
					  NULL);
	r1label = XtVaCreateManagedWidget("print_to", labelWidgetClass,
					  f01form,
					  XtNlabel, "Print to: ",
					  XtNborderWidth, 0,
					  HORIZONTAL_RESIZING_NO,
					  NULL);
	
	r2form = XtVaCreateManagedWidget("print_to_printer_form", formWidgetClass,
					 form,
					 XtNresizable, True,
					 XtNborderWidth, 0,
					 XtNvertDistance, 0,
					 XtNfromVert, f01form,
					 HORIZONTAL_RESIZING_YES,
					 XtNallowResize, True,
					 NULL);
	
	accels2 = XtParseAcceleratorTable("<Btn1Down>,<Btn1Up>:set()notify()");

	to_printer_radio = XtVaCreateManagedWidget("to_printer",
#ifdef XAW
						   radioWidgetClass,
#else
						   toggleWidgetClass,
#endif
						   r2form,
						   XtNlabel, "Printer: ",
						   XtNborderWidth, 0,
						   XtNhighlightThickness, 1,
						   XtNradioData, PRINT_TO_PRINTER,
						   XtNstate, resource.default_printing_target == PRINT_TO_PRINTER,
						   XtNtranslations, xlats,
						   XtNaccelerators, accels2,
						   HORIZONTAL_RESIZING_NO,
						   VERTICAL_RESIZING_NO,
						   NULL);
	
	XtAddCallback(to_printer_radio, XtNcallback, cb_print_vs_file, NULL);
	XtInstallAccelerators(r2form, to_printer_radio);
	
	print_to_printer_text = XtVaCreateManagedWidget("printer_text", asciiTextWidgetClass,
							r2form,
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
			  xaw_print_handle_key, (XtPointer)"2");
	
	adjust_vertically(to_printer_radio, print_to_printer_text, ddist);
	
	r3form = XtVaCreateManagedWidget("print_to_file_form", formWidgetClass,
					 form,
					 XtNborderWidth, 0,
					 XtNvertDistance, 0,
					 XtNfromVert, r2form,
					 HORIZONTAL_RESIZING_YES,
					 NULL);

	to_file_radio_or_label[act] = XtVaCreateManagedWidget("to_file",
#ifdef XAW
							      radioWidgetClass,
#else
							      toggleWidgetClass,
#endif
							      r3form,
							      XtNhighlightThickness, 1,
							      XtNborderWidth, 0,
							      XtNlabel, "File: ",
							      XtNstate, resource.default_printing_target == PRINT_TO_FILE,
							      XtNradioGroup, to_printer_radio,
							      XtNradioData, PRINT_TO_FILE,
							      XtNtranslations, xlats,
							      XtNaccelerators, accels2,
							      HORIZONTAL_RESIZING_NO,
							      VERTICAL_RESIZING_NO,
							      NULL);
	XtAddCallback(to_file_radio_or_label[act], XtNcallback, cb_print_vs_file, NULL);
	XtInstallAccelerators(r3form, to_file_radio_or_label[act]);

	print_to_file_text[act] = XtVaCreateManagedWidget("file_text", asciiTextWidgetClass,
							  r3form,
							  XtNwidth, w_avg,
							  XtNdataCompression, False,
							  XtNeditType, XawtextEdit,
							  XtNfromHoriz, to_file_radio_or_label[act],
							  HORIZONTAL_RESIZING_YES,
							  VERTICAL_RESIZING_NO,
							  NULL);
	XtOverrideTranslations(print_to_file_text[act], xlats2);
	XtAddEventHandler(print_to_file_text[act], KeyPressMask | ButtonPressMask, False,
			  xaw_print_handle_key, (XtPointer)"3");
    }
    else { /* not printing, but saving */
	int i;
	Dimension max_entry_width = 0;
	f01form = XtVaCreateManagedWidget("dummy_save_as_form", formWidgetClass,
					  form,
					  XtNresizable, True,
					  XtNborderWidth, 0,
					  /* XtNvertDistance, 10, */
					  HORIZONTAL_RESIZING_YES,
					  NULL);
	r1label = XtVaCreateManagedWidget("save_as", labelWidgetClass,
					  f01form,
					  XtNlabel, "Save as: ",
					  XtNborderWidth, 0,
					  HORIZONTAL_RESIZING_NO,
					  NULL);
	r1form = XtVaCreateManagedWidget("save_to_file_form", formWidgetClass,
					 form,
					 XtNborderWidth, 0,
					 XtNfromVert, f01form,
					 HORIZONTAL_RESIZING_YES,
					 VERTICAL_RESIZING_NO,
					 NULL);
/* 	fprintf(stderr, "formr1: %ld\n", (long)r1form); */
#if 0
	menu_double_arrow_bitmap
	    = XCreateBitmapFromData(XtDisplay(globals.widgets.top_level),
				    RootWindowOfScreen(XtScreen(globals.widgets.top_level)),
				    (char *)menu_double_arrow_bits, MENU_DOUBLE_ARROW_W, MENU_DOUBLE_ARROW_H);
#endif
	format_selection_label = XtVaCreateManagedWidget("format_selection_label", labelWidgetClass,
							 r1form,
							 XtNlabel, "Format: ",
							 XtNborderWidth, 0,
							 HORIZONTAL_RESIZING_NO,
							 VERTICAL_RESIZING_NO,
							 NULL);
	format_selection_button = XtVaCreateManagedWidget("format_selection_button", menuButtonWidgetClass,
							  r1form,
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
	format_selection_b1 = XtVaCreateManagedWidget("format_selection_b1", commandWidgetClass,
						      r1form,
						      XtNborderWidth, resource.btn_border_width,
						      XtNfromHoriz, format_selection_button,
						      XtNhorizDistance, -1,
						      XtNhighlightThickness, 0,
						      XtNbitmap, menu_double_arrow_bitmap,
						      NULL);
	adjust_heights(format_selection_button, format_selection_b1, NULL);
#endif
	format_selection_popup = XtCreatePopupShell("format_selection_popup", simpleMenuWidgetClass,
						    format_selection_button,
/* 						    globals.widgets.top_level, */
						    NULL, 0);
#if 0
	XtAddCallback(format_selection_b1, XtNcallback, xaw_popup_menu_cb, format_selection_popup);
#endif
	for (i = 1; i < FORMAT_SEL_CNT; i++) {
	    char name[1024];
	    Dimension curr_entry_width;
	    SNPRINTF(name, sizeof name, "format_selection_pulldown_%d", i);
	    format_selection_menu[i] = XtVaCreateManagedWidget(name, smeBSBObjectClass,
							       format_selection_popup,
							       XtNjustify, XtJustifyLeft,
							       XtNlabel,
							       i == resource.default_saving_format
							       ? output_format_mapping[0].fmt_string
							       : output_format_mapping[i].fmt_string,
							       NULL);
	    XtVaGetValues(format_selection_menu[i], XtNwidth, &curr_entry_width, NULL);
	    if (curr_entry_width > max_entry_width)
		max_entry_width = curr_entry_width;
	    XtAddCallback(format_selection_menu[i], XtNcallback, select_format_cb, form);
	}
	XtVaSetValues(format_selection_button, XtNwidth, max_entry_width, NULL);
	
	r3form = XtVaCreateManagedWidget("to_file_form", formWidgetClass,
					 form,
					 XtNborderWidth, 0,
					 XtNfromVert, r1form,
					 HORIZONTAL_RESIZING_YES,
					 VERTICAL_RESIZING_NO,
					 NULL);
	to_file_radio_or_label[act] = XtVaCreateManagedWidget("to_file", labelWidgetClass,
							      r3form,
							      XtNlabel, "File name: ",
							      XtNborderWidth, 0,
							      HORIZONTAL_RESIZING_NO,
							      VERTICAL_RESIZING_NO,
							      NULL);
	print_to_file_text[act] = XtVaCreateManagedWidget("file_text", asciiTextWidgetClass,
							  r3form,
							  XtNwidth, w_avg,
							  XtNdataCompression, False,
							  /* 							  XtNvertDistance, ddist + 5, */
							  XtNeditType, XawtextEdit,
							  XtNfromHoriz, to_file_radio_or_label[act],
							  HORIZONTAL_RESIZING_YES,
							  VERTICAL_RESIZING_NO,
							  NULL);
	XtOverrideTranslations(print_to_file_text[act], xlats2);
	
	
	{ /* align left edges of format selection pulldown and filename field */
	    Dimension w1, w2, w_max;
	    XtVaGetValues(format_selection_label, XtNwidth, &w1, NULL);
	    XtVaGetValues(to_file_radio_or_label[act], XtNwidth, &w2, NULL);
	    w_max = MAX(w1, w2);
	    XtVaSetValues(format_selection_button, XtNhorizDistance, ddist + (w_max - w1), NULL);
	    XtVaSetValues(print_to_file_text[act], XtNhorizDistance, ddist + (w_max - w2), NULL);
	}
    }
    
#define DIST_1 8 /* FIXME */
    print_to_file_button[act] = XtVaCreateManagedWidget("file_button", commandWidgetClass,
							r3form,
							XtNlabel, "Browse ...",
							XtNfromHoriz, print_to_file_text[act],
							XtNhorizDistance, DIST_1,
							/* attach to right of form, no resizing: */
							XtNleft, XtChainRight, XtNright, XtChainRight,
							VERTICAL_RESIZING_NO,
							NULL);
    
    XtVaGetValues(print_to_file_button[act], XtNwidth, &w_curr, NULL);
    XtVaSetValues(print_to_file_text[act], XtNwidth, w_avg - w_curr - DIST_1 - 2, NULL);
#undef DIST_1

    adjust_vertically(to_file_radio_or_label[act], print_to_file_text[act], ddist + 5);
    adjust_vertically(print_to_file_text[act], to_file_radio_or_label[act], ddist + 5);
    adjust_vertically(print_to_file_button[act], to_file_radio_or_label[act], ddist + 5);
    
    XtAddCallback(print_to_file_button[act], XtNcallback, cb_browse, (XtPointer)info);
    
    /* other dvips options */
    r4form = XtVaCreateManagedWidget("dvips_options_form", formWidgetClass,
					 form,
					 XtNborderWidth, 0,
					 XtNfromVert, r3form,
					 XtNvertDistance, 0,
					 HORIZONTAL_RESIZING_YES,
					 NULL);
    
    r4label = XtVaCreateManagedWidget("dvips_options_name", labelWidgetClass,
				      r4form,
				      XtNlabel, "Dvips Options:",
				      XtNborderWidth, 0,
				      HORIZONTAL_RESIZING_NO,
				      VERTICAL_RESIZING_NO,
				      NULL);
    
    dvips_options_text[act] = XtVaCreateManagedWidget("dvips_options_text", asciiTextWidgetClass,
						      r4form,
						      XtNwidth, w_avg,
						      XtNdataCompression, False,
						      XtNeditType, XawtextEdit,
						      XtNfromHoriz, r4label,
						      HORIZONTAL_RESIZING_YES,
						      VERTICAL_RESIZING_NO,
						      NULL);
    XtOverrideTranslations(dvips_options_text[act], xlats2);

    { /* align left edges of input fields */
	Dimension w1, w2, w3, w_max;

	if (act == FILE_PRINT)
	    XtVaGetValues(to_printer_radio, XtNwidth, &w1, NULL);
	else
	    XtVaGetValues(format_selection_label, XtNwidth, &w1, NULL);

	XtVaGetValues(to_file_radio_or_label[act], XtNwidth, &w2, NULL);
	XtVaGetValues(r4label, XtNwidth, &w3, NULL);
	
	w_max = MAX(MAX(w1, w2), w3);

	if (act == FILE_PRINT)
	    XtVaSetValues(print_to_printer_text, XtNhorizDistance, ddist + (w_max - w1), NULL);
	else
	     XtVaSetValues(format_selection_button, XtNhorizDistance, ddist + (w_max - w1), NULL);
	
	XtVaSetValues(print_to_file_text[act], XtNhorizDistance, ddist + (w_max - w2), NULL);
	XtVaSetValues(dvips_options_text[act], XtNhorizDistance, ddist + (w_max - w3), NULL);
    }

    /* page selection */

    f02form = XtVaCreateManagedWidget("dummy_pages_form", formWidgetClass,
				      form,
				      XtNfromVert, r4form,
				      XtNresizable, True,
				      XtNborderWidth, 0,
				      /* 				      XtNvertDistance, 10, */
				      HORIZONTAL_RESIZING_NO,
				      NULL);
    
    r5label = XtVaCreateManagedWidget("range_lab", labelWidgetClass,
				      f02form,
				      XtNlabel, "Pages:",
				      XtNborderWidth, 0,
				      HORIZONTAL_RESIZING_NO,
				      NULL);

    r6form = XtVaCreateManagedWidget("page_range_form", formWidgetClass,
				     form,
				     XtNborderWidth, 0,
				     XtNfromVert, f02form,
				     /* 				     XtNvertDistance, 0, */
				     HORIZONTAL_RESIZING_NO,
				     NULL);

    accels2 = XtParseAcceleratorTable("<Btn1Down>,<Btn1Up>:set()notify()");
    range_all_radio[act] = XtVaCreateManagedWidget("range_all",
#ifdef XAW
						   radioWidgetClass,
#else
						   toggleWidgetClass,
#endif
						   r6form,
						   XtNlabel, "All ",
						   XtNborderWidth, 0,
						   XtNhighlightThickness, 1,
						   XtNradioData, act == FILE_PRINT ? PRINT_ALL : SAVE_ALL,
						   XtNstate, True, /* enable this button by default */
						   XtNtranslations, xlats,
						   XtNaccelerators, accels2,
						   /* 						   XtNvertDistance, 0, */
						   HORIZONTAL_RESIZING_NO,
						   VERTICAL_RESIZING_NO,
						   NULL);
    if (act == FILE_PRINT)
	XtAddCallback(range_all_radio[act], XtNcallback, cb_range, (XtPointer)PRINT_ALL);
    else
	XtAddCallback(range_all_radio[act], XtNcallback, cb_range, (XtPointer)SAVE_ALL);
    XtInstallAccelerators(r6form, range_all_radio[act]);

    r61form = XtVaCreateManagedWidget("range_marked_form", formWidgetClass,
				      form,
				      XtNborderWidth, 0,
				      XtNfromVert, r6form,
				      /* 				      XtNvertDistance, 0, */
				      HORIZONTAL_RESIZING_NO,
				      NULL);

    range_marked_radio[act] = XtVaCreateManagedWidget("range_marked",
#ifdef XAW
						      radioWidgetClass,
#else
						      toggleWidgetClass,
#endif

						      r61form,
						      XtNlabel, "Marked ",
						      XtNborderWidth, 0,
						      XtNhighlightThickness, 1,
						      XtNradioGroup, range_all_radio[act],
						      XtNradioData, act == FILE_PRINT ? PRINT_MARKED : SAVE_MARKED,
						      XtNtranslations, xlats,
						      XtNaccelerators, accels2,
						      /* 						      XtNvertDistance, 0, */
						      HORIZONTAL_RESIZING_NO,
						      VERTICAL_RESIZING_NO,
						      NULL);
    if (act == FILE_PRINT)
	XtAddCallback(range_marked_radio[act], XtNcallback, cb_range, (XtPointer)PRINT_RANGE);
    else
	XtAddCallback(range_marked_radio[act], XtNcallback, cb_range, (XtPointer)SAVE_RANGE);
    XtInstallAccelerators(r61form, range_marked_radio[act]);

    r7form = XtVaCreateManagedWidget("range_from_to_form", formWidgetClass,
				     form,
				     XtNborderWidth, 0,
				     XtNfromVert, r61form,
				     /* 				     XtNvertDistance, 0, */
				     XtNresizable, True,
				     /*  HORIZONTAL_RESIZING_NO, */
				     NULL);

    range_from_to_radio[act] = XtVaCreateManagedWidget("range_from_to",
#ifdef XAW
						       radioWidgetClass,
#else
						       toggleWidgetClass,
#endif
						       r7form,
						       XtNlabel, "From: ",
						       XtNborderWidth, 0,
						       XtNhighlightThickness, 1,
						       XtNradioGroup, range_all_radio[act],
						       XtNradioData, act == FILE_PRINT ? PRINT_RANGE : SAVE_RANGE,
						       XtNtranslations, xlats,
						       XtNaccelerators, accels2,
						       /* 						       XtNvertDistance, 0, */
						       HORIZONTAL_RESIZING_NO,
						       VERTICAL_RESIZING_NO,
						       NULL);
    if (act == FILE_PRINT)
	XtAddCallback(range_from_to_radio[act], XtNcallback, cb_range, (XtPointer)PRINT_RANGE);
    else
	XtAddCallback(range_from_to_radio[act], XtNcallback, cb_range, (XtPointer)SAVE_RANGE);
    XtInstallAccelerators(r7form, range_from_to_radio[act]);

    page_from_text[act] = XtVaCreateManagedWidget("range_from", asciiTextWidgetClass,
						  r7form,
						  XtNdataCompression, False,
						  XtNeditType, XawtextEdit,
						  XtNwidth, 50,
						  XtNfromHoriz, range_from_to_radio[act],
						  HORIZONTAL_RESIZING_NO,
						  VERTICAL_RESIZING_NO,
						  NULL);
    XtOverrideTranslations(page_from_text[act], xlats2);
    XtAddEventHandler(page_from_text[act], KeyPressMask | ButtonPressMask, False,
		      xaw_range_handle_key, (XtPointer)act);

    to_label[act] = XtVaCreateManagedWidget("range_to_lab", labelWidgetClass,
					    r7form,
					    XtNlabel, "to: ",
					    XtNborderWidth, 0,
					    XtNfromHoriz, page_from_text[act],
					    HORIZONTAL_RESIZING_NO,
					    VERTICAL_RESIZING_NO,
					    NULL);

    page_to_text[act] = XtVaCreateManagedWidget("range_to", asciiTextWidgetClass,
						r7form,
						XtNdataCompression, False,
						XtNeditType, XawtextEdit,
						XtNwidth, 50,
						XtNfromHoriz, to_label[act],
						HORIZONTAL_RESIZING_NO,
						VERTICAL_RESIZING_NO,
						NULL);
    XtOverrideTranslations(page_to_text[act], xlats2);
    XtAddEventHandler(page_to_text[act], KeyPressMask | ButtonPressMask, False,
		      xaw_range_handle_key, (XtPointer)act);

    of_label[act] = XtVaCreateManagedWidget("range_of", labelWidgetClass,
					    r7form,
					    XtNlabel, ofstring,
					    XtNborderWidth, 0,
					    XtNresizable, True,
					    XtNfromHoriz, page_to_text[act],
					    /*  HORIZONTAL_RESIZING_NO, */
					    XtNleft, XtChainLeft,
					    VERTICAL_RESIZING_NO,
					    NULL);

    adjust_vertically(range_from_to_radio[act], page_from_text[act], ddist);
    adjust_vertically(to_label[act], page_from_text[act], ddist);
    adjust_vertically(of_label[act], page_from_text[act], ddist);

    /* box for the OK/Cancel button */
    box = XtVaCreateManagedWidget("box", formWidgetClass,
				  paned,
				  /* resizing by user isn't needed */
				  XtNshowGrip, False,
				  XtNdefaultDistance, 6, /* some padding */
				  /* resizing the window shouldn't influence this box,
				   * but only the pane widget
				   */
				  XtNskipAdjust, True,
				  XtNaccelerators, G_accels_cr,
				  NULL);
	
    ok_button[act] = XtVaCreateManagedWidget(act == FILE_PRINT ? "print_button" : "save_button",
					     commandWidgetClass, box,
					     XtNlabel, act == FILE_PRINT ? "Print" : "Save",
					     XtNaccelerators, G_accels_cr,
					     XtNtop, XtChainTop,
					     XtNbottom, XtChainBottom,
					     HORIZONTAL_RESIZING_NO,
					     NULL);
    if (act == FILE_PRINT)
	XtAddCallback(ok_button[act], XtNcallback, cb_print_or_save, (XtPointer)info);
    else
	XtAddCallback(ok_button[act], XtNcallback, cb_print_or_save, (XtPointer)info);
    
    XtInstallAccelerators(form, ok_button[act]);
    if (act == FILE_PRINT)
	XtInstallAccelerators(print_to_printer_text, ok_button[act]);
    XtInstallAccelerators(print_to_file_text[act], ok_button[act]);

    cancel_button[act] = XtVaCreateManagedWidget("cancel", commandWidgetClass,
						 box,
						 XtNlabel, "Cancel",
						 XtNfromHoriz, ok_button[act],
						 XtNbottom, XtChainBottom,
						 XtNjustify, XtJustifyRight,
						 XtNleft, XtChainRight,
						 XtNright, XtChainRight,
						 NULL);
    if (act == FILE_PRINT)
	XtAddCallback(cancel_button[act], XtNcallback, cb_print_cancel, (XtPointer)info);
    else
	XtAddCallback(cancel_button[act], XtNcallback, cb_save_cancel, (XtPointer)info);

    XtManageChild(top_level_shell);
    center_window(top_level_shell, globals.widgets.top_level);
    XtMapWidget(top_level_shell);
    XSetWMProtocols(XtDisplay(top_level_shell), XtWindow(top_level_shell), &WM_DELETE_WINDOW, 1);

    if (pageinfo_have_marked_pages()) {
	XawToggleSetCurrent(range_all_radio[act],
			    act == FILE_PRINT ? (XtPointer)PRINT_MARKED : (XtPointer)SAVE_MARKED);
    }
    else if ((act == FILE_PRINT && save_pr_info.range_radio == PRINT_MARKED)
	     || (act == FILE_SAVE && save_save_info.range_radio == SAVE_MARKED)) {
	XawToggleSetCurrent(range_all_radio[act],
			    act == FILE_PRINT ? (XtPointer)PRINT_ALL : (XtPointer)SAVE_ALL);
    }
    
#else /* not MOTIF */
    Widget form, pane, box;
    Widget frame1, f1label, f1child;
    Widget r4label;
    Widget frame2, f2label, f2child;
    XmString str;
    XtTranslations xlats;
    
    XtAddActions(print_actions, XtNumber(print_actions));

    if (act == FILE_SAVE) {
	char *ptr = get_string_va("#override \n<Key>osfCancel:WM_save_cancel(%p)", info);
	xlats = XtParseTranslationTable(ptr);
	free(ptr);
    }
    else {
	char *ptr = get_string_va("#override \n<Key>osfCancel:WM_print_cancel(%p)", info);
	xlats = XtParseTranslationTable(ptr);
	free(ptr);
    }
    
    top_level_shell = XtVaCreatePopupShell(act == FILE_SAVE ? "save_popup" : "print_popup",
					   xmDialogShellWidgetClass, globals.widgets.top_level,
					   XmNtitle, act == FILE_SAVE ? "Save file" : "Print DVI file",
					   XmNallowShellResize, True,
					   XmNdeleteResponse, XmDO_NOTHING, /* we'll take care of that ourselves */
					   XmNmappedWhenManaged, False, /* so that we can center it first */
					   NULL);

    /* make the window manager destroy action just pop down the dialog */
    WM_DELETE_WINDOW = XmInternAtom(XtDisplay(top_level_shell), "WM_DELETE_WINDOW", False);
    if (act == FILE_SAVE)
	XmAddWMProtocolCallback(top_level_shell, WM_DELETE_WINDOW, cb_save_cancel, (XtPointer)info);
    else
	XmAddWMProtocolCallback(top_level_shell, WM_DELETE_WINDOW, cb_print_cancel, (XtPointer)info);


    pane = XtVaCreateWidget(act == FILE_SAVE ? "save_paned" : "print_paned",
			    xmPanedWindowWidgetClass, top_level_shell,
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

	frame1 = XtVaCreateWidget("print_to_frame", xmFrameWidgetClass, form,
				  XmNmarginWidth, DDIST,
				  XmNmarginHeight, DDIST,
				  XmNtopAttachment, XmATTACH_FORM,
				  XmNleftAttachment, XmATTACH_FORM,
				  XmNrightAttachment, XmATTACH_FORM,
				  XTranslations, xlats,
				  NULL);

	str = XmStringCreateLocalized("Print to:");
	f1label = XtVaCreateManagedWidget("title", xmLabelGadgetClass,
					  frame1,
					  XmNchildType, XmFRAME_TITLE_CHILD,
					  XmNlabelString, str,
					  NULL);
	XmStringFree(str);

	f1child = XtVaCreateWidget("form", xmFormWidgetClass, frame1,
				   XmNhorizontalSpacing, DDIST,
				   XmNverticalSpacing, DDIST,
				   XmNtopAttachment, XmATTACH_FORM,
				   XmNleftAttachment, XmATTACH_FORM,
				   XmNrightAttachment, XmATTACH_FORM,
				   XTranslations, xlats,
				   NULL);

	str = XmStringCreateLocalized("Printer:");
	to_printer_radio = XtVaCreateManagedWidget("to_printer",
						   xmToggleButtonGadgetClass, f1child,
						   XmNlabelString, str,
						   XmNindicatorType, XmONE_OF_MANY,
						   XmNset, resource.default_printing_target == PRINT_TO_PRINTER,
						   XmNtopAttachment, XmATTACH_FORM,
						   XmNtopOffset, 0,
						   XmNleftAttachment, XmATTACH_FORM,
						   XmNleftOffset, 0,
						   NULL);
	XmStringFree(str);
	XtAddCallback(to_printer_radio, XmNvalueChangedCallback, cb_print_vs_file, (XtPointer)1);

	XtOverrideTranslations(to_printer_radio, xlats);

	print_to_printer_text = XtVaCreateManagedWidget("print_text", xmTextFieldWidgetClass,
							f1child,
							XmNtopAttachment, XmATTACH_FORM,
							XmNtopOffset, 0,
							XmNleftAttachment, XmATTACH_WIDGET,
							XmNrightAttachment, XmATTACH_FORM,
							XmNleftWidget, to_printer_radio,
							XTranslations, xlats,
							NULL);
	XtAddCallback(print_to_printer_text, XmNactivateCallback, cb_print_or_save, info);

	adjust_heights(to_printer_radio, print_to_printer_text, NULL);

	str = XmStringCreateLocalized("File:");
	to_file_radio_or_label[act] = XtVaCreateManagedWidget("to_file", xmToggleButtonGadgetClass,
							      f1child,
							      XmNlabelString, str,
							      XmNindicatorType, XmONE_OF_MANY,
							      XmNtopAttachment, XmATTACH_WIDGET,
							      XmNtopWidget, print_to_printer_text,
							      XmNleftAttachment, XmATTACH_FORM,
							      XmNset, resource.default_printing_target == PRINT_TO_FILE,
							      XmNleftOffset, 0,
							      NULL);
	XmStringFree(str);
	XtAddCallback(to_file_radio_or_label[act], XmNvalueChangedCallback, cb_print_vs_file, (XtPointer)2);

	str = XmStringCreateLocalized("Browse ...");
	print_to_file_button[act] = XtVaCreateManagedWidget("file_button", xmPushButtonWidgetClass,
							    f1child,
							    XmNlabelString, str,
							    XmNsensitive, resource.default_printing_target == PRINT_TO_FILE,
							    XmNtopAttachment, XmATTACH_WIDGET,
							    XmNtopWidget, print_to_printer_text,
							    XmNrightAttachment, XmATTACH_FORM,
							    NULL);
	XmStringFree(str);	

	XtOverrideTranslations(to_file_radio_or_label[act], xlats);
	XtOverrideTranslations(print_to_file_button[act], xlats);

	print_to_file_text[act] = XtVaCreateManagedWidget("file_text", xmTextFieldWidgetClass,
							  f1child,
							  XmNsensitive, resource.default_printing_target == PRINT_TO_FILE,
							  XmNtopAttachment, XmATTACH_WIDGET,
							  XmNtopWidget, print_to_printer_text,
							  XmNrightAttachment, XmATTACH_WIDGET,
							  XmNrightWidget, print_to_file_button[act],
							  XmNleftAttachment, XmATTACH_WIDGET,
							  XmNleftWidget, to_file_radio_or_label[act],
							  XTranslations, xlats,
							  NULL);

	adjust_heights(to_file_radio_or_label[act], print_to_file_button[act], print_to_file_text[act], NULL);

	XtAddCallback(print_to_file_text[act], XmNactivateCallback, cb_print_or_save, (XtPointer)info);
	XtAddCallback(print_to_file_button[act], XmNactivateCallback, cb_browse, (XtPointer)info);

	XtManageChild(f1child);
	XtManageChild(frame1);

    } /* saving, not printing */
    else {
	frame1 = XtVaCreateWidget("save_as_frame", xmFrameWidgetClass,
				  form,
				  XmNmarginWidth, DDIST,
				  XmNmarginHeight, DDIST,
				  XmNtopAttachment, XmATTACH_FORM,
				  XmNleftAttachment, XmATTACH_FORM,
				  XmNrightAttachment, XmATTACH_FORM,
				  NULL);

	str = XmStringCreateLocalized("Save as:");
	f1label = XtVaCreateManagedWidget("title", xmLabelGadgetClass,
					  frame1,
					  XmNchildType, XmFRAME_TITLE_CHILD,
					  XmNlabelString, str,
					  NULL);
	XmStringFree(str);

	f1child = XtVaCreateWidget("form", xmFormWidgetClass, frame1,
				   XmNhorizontalSpacing, DDIST,
				   XmNverticalSpacing, DDIST,
				   XmNtopAttachment, XmATTACH_FORM,
				   XmNleftAttachment, XmATTACH_FORM,
				   XmNrightAttachment, XmATTACH_FORM,
				   XTranslations, xlats,
				   NULL);
/* 	fprintf(stderr, "CHILD: 0x%x\n", (long)f1child); */
	str = XmStringCreateLocalized("Format:");
	format_selection_texts[0] = XmStringCreateLocalized((char *)output_format_mapping[0].fmt_string);
	format_selection_texts[1] = XmStringCreateLocalized((char *)output_format_mapping[1].fmt_string);
	format_selection_texts[2] = XmStringCreateLocalized((char *)output_format_mapping[2].fmt_string);
	format_selection_texts[3] = XmStringCreateLocalized((char *)output_format_mapping[3].fmt_string);
	format_selection_texts[4] = XmStringCreateLocalized((char *)output_format_mapping[4].fmt_string);
	format_selection_option
	    = XmVaCreateSimpleOptionMenu(f1child, "format_selection_option",
					 str,  'F',
					 resource.default_saving_format, /*initial menu selection*/
					 select_format_cb, /* callback */
					 XmVaPUSHBUTTON, format_selection_texts[0], 'P', NULL, NULL,
					 XmVaPUSHBUTTON, format_selection_texts[1], 'F', NULL, NULL,
					 XmVaPUSHBUTTON, format_selection_texts[2], 'D', NULL, NULL,
					 XmVaPUSHBUTTON, format_selection_texts[3], 'T', NULL, NULL,
					 XmVaPUSHBUTTON, format_selection_texts[4], 'U', NULL, NULL,
					 NULL);
	
	XmStringFree(str);
	XmStringFree(format_selection_texts[0]);
	XmStringFree(format_selection_texts[1]);
	XmStringFree(format_selection_texts[2]);
	XmStringFree(format_selection_texts[3]);
	XmStringFree(format_selection_texts[4]);
	
	str = XmStringCreateLocalized("File name:");
	to_file_radio_or_label[act] = XtVaCreateManagedWidget("to_file", xmLabelGadgetClass,
							      f1child,
							      XmNtopAttachment, XmATTACH_WIDGET,
							      XmNtopWidget, format_selection_option,
							      XmNleftAttachment, XmATTACH_FORM,
							      /* XmNchildType, XmFRAME_TITLE_CHILD, */
							      XmNlabelString, str,
							      NULL);
	XmStringFree(str);
	str = XmStringCreateLocalized("Browse ...");
	print_to_file_button[act] = XtVaCreateManagedWidget("file_button", xmPushButtonWidgetClass,
							    f1child,
							    XmNlabelString, str,
							    XmNtopAttachment, XmATTACH_WIDGET,
							    XmNtopWidget, format_selection_option,
							    XmNrightAttachment, XmATTACH_FORM,
							    XTranslations, xlats,
							    NULL);
	print_to_file_text[act] = XtVaCreateManagedWidget("file_text", xmTextFieldWidgetClass,
							  f1child,
							  XmNtopAttachment, XmATTACH_WIDGET,
							  XmNtopWidget, format_selection_option,
							  XmNleftAttachment, XmATTACH_WIDGET,
							  XmNleftWidget, to_file_radio_or_label[act],
							  XmNrightAttachment, XmATTACH_WIDGET,
							  XmNrightWidget, print_to_file_button[act],
							  XTranslations, xlats,
							  NULL);

	adjust_heights(to_file_radio_or_label[act], print_to_file_button[act], print_to_file_text[act], NULL);
	XtAddCallback(print_to_file_button[act], XmNactivateCallback, cb_browse, (XtPointer)info);
    }
    
#if 1
    { /* align left edges of widgets */
	Dimension w1, w2;

	XtVaGetValues(to_file_radio_or_label[act], XmNwidth, &w2, NULL);
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
	    XtVaSetValues(print_to_file_text[act], XmNleftOffset, offset, NULL);
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
#endif /* 0 */

    if (act == FILE_SAVE) {
	XtManageChild(format_selection_option);
	XtManageChild(f1child);
	XtManageChild(frame1);
    }
    
    /* additional dvips options */
    str = XmStringCreateLocalized("Dvips Options:");
    r4label = XtVaCreateManagedWidget("dvips_options_name", xmLabelGadgetClass,
				      form,
				      XmNlabelString, str,
				      XmNtopAttachment, XmATTACH_WIDGET,
				      XmNtopWidget, frame1,
				      XmNleftAttachment, XmATTACH_FORM,
				      NULL);
    XmStringFree(str);
    
    dvips_options_text[act] = XtVaCreateManagedWidget("dvips_options_text", xmTextFieldWidgetClass,
						      form,
						      XmNtopAttachment, XmATTACH_WIDGET,
						      XmNtopWidget, frame1,
						      XmNleftAttachment, XmATTACH_WIDGET,
						      XmNrightAttachment, XmATTACH_FORM,
						      XmNleftWidget, r4label,
						      XmNcolumns, 20,
						      XTranslations, xlats,
						      NULL);
    XtAddCallback(dvips_options_text[act], XmNactivateCallback, cb_print_or_save, (XtPointer)info);
    adjust_heights(r4label, dvips_options_text[act], NULL);
    
    /* page selection */
    frame2 = XtVaCreateWidget(act == FILE_PRINT ? "print_page_frame" : "save_page_frame", xmFrameWidgetClass,
			      form,
			      XmNmarginWidth, DDIST,
			      XmNmarginHeight, DDIST,
			      XmNresizable, True,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, dvips_options_text[act],
			      XmNleftAttachment, XmATTACH_FORM,
			      XmNrightAttachment, XmATTACH_FORM,
			      XTranslations, xlats,
			      NULL);

    str = XmStringCreateLocalized("Pages:");
    f2label = XtVaCreateManagedWidget("title", xmLabelGadgetClass,
				      frame2,
				      XmNchildType, XmFRAME_TITLE_CHILD,
				      XmNlabelString, str,
				      NULL);
    XmStringFree(str);

    f2child = XtVaCreateWidget("form", xmFormWidgetClass, frame2,
			       XmNhorizontalSpacing, DDIST,
			       XmNverticalSpacing, DDIST,
			       XmNresizable, True,
			       XTranslations, xlats,
			       XTranslations, xlats,
			       NULL);

    str = XmStringCreateLocalized("All");
    range_all_radio[act] = XtVaCreateManagedWidget("all",
						   xmToggleButtonGadgetClass, f2child,
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
    if (act == FILE_PRINT) {
	XtAddCallback(range_all_radio[act], XmNvalueChangedCallback, cb_range, (XtPointer)PRINT_ALL);
    }
    else {
	XtAddCallback(range_all_radio[act], XmNvalueChangedCallback, cb_range, (XtPointer)SAVE_ALL);
    }

    str = XmStringCreateLocalized("Marked");
    range_marked_radio[act] =  XtVaCreateManagedWidget("marked",
						       xmToggleButtonGadgetClass, f2child,
						       XmNlabelString, str,
						       XmNindicatorType, XmONE_OF_MANY,
						       XmNtopAttachment, XmATTACH_WIDGET,
						       XmNtopWidget, range_all_radio[act],
						       XmNleftAttachment, XmATTACH_FORM,
						       XmNleftOffset, 0,
						       XTranslations, xlats,
						       NULL);
    XmStringFree(str);
    if (act == FILE_PRINT) {
	XtAddCallback(range_marked_radio[act], XmNvalueChangedCallback, cb_range, (XtPointer)PRINT_MARKED);
    }
    else {
	XtAddCallback(range_marked_radio[act], XmNvalueChangedCallback, cb_range, (XtPointer)SAVE_MARKED);
    }
	
    str = XmStringCreateLocalized("Range:");
    range_from_to_radio[act] = XtVaCreateManagedWidget("range",
						       xmToggleButtonGadgetClass, f2child,
						       XmNlabelString, str,
						       XmNindicatorType, XmONE_OF_MANY,
						       XmNtopAttachment, XmATTACH_WIDGET,
						       XmNtopWidget, range_marked_radio[act],
						       XmNleftAttachment, XmATTACH_FORM,
						       XmNleftOffset, 0,
						       XTranslations, xlats,
						       NULL);
    XmStringFree(str);

    if (act == FILE_PRINT) {
	XtAddCallback(range_from_to_radio[act], XmNvalueChangedCallback, cb_range, (XtPointer)PRINT_RANGE);
    }
    else {
	XtAddCallback(range_from_to_radio[act], XmNvalueChangedCallback, cb_range, (XtPointer)SAVE_RANGE);
    }
    str = XmStringCreateLocalized("From");
    from_label[act] = XtVaCreateManagedWidget("from", xmLabelGadgetClass,
					      f2child,
					      XmNlabelString, str,
					      XmNsensitive, False,
					      XmNtopAttachment, XmATTACH_WIDGET,
					      XmNtopWidget, range_marked_radio[act],
					      XmNleftAttachment, XmATTACH_WIDGET,
					      XmNleftWidget, range_from_to_radio[act],
					      NULL);
    XmStringFree(str);

    page_from_text[act] = XtVaCreateManagedWidget("from_page",
						  xmTextFieldWidgetClass, f2child,
						  XmNcolumns, 5,
						  XmNsensitive, False,
						  XmNtopAttachment, XmATTACH_WIDGET,
						  XmNtopWidget, range_marked_radio[act],
						  XmNleftAttachment, XmATTACH_WIDGET,
						  XmNleftWidget, from_label[act],
						  XTranslations, xlats,
						  NULL);
    XtAddCallback(page_from_text[act], XmNactivateCallback, cb_print_or_save, (XtPointer)info);

    str = XmStringCreateLocalized("to");
    to_label[act] = XtVaCreateManagedWidget("to", xmLabelGadgetClass,
					    f2child,
					    XmNlabelString, str,
					    XmNsensitive, False,
					    XmNtopAttachment, XmATTACH_WIDGET,
					    XmNtopWidget, range_marked_radio[act],
					    XmNleftAttachment, XmATTACH_WIDGET,
					    XmNleftWidget, page_from_text[act],
					    NULL);
    XmStringFree(str);

    page_to_text[act] = XtVaCreateManagedWidget("to_page", xmTextFieldWidgetClass,
						f2child,
						XmNcolumns, 5,
						XmNsensitive, False,
						XmNtopAttachment, XmATTACH_WIDGET,
						XmNtopWidget, range_marked_radio[act],
						XmNleftAttachment, XmATTACH_WIDGET,
						XmNleftWidget, to_label[act],
						XTranslations, xlats,
						NULL);
    XtAddCallback(page_to_text[act], XmNactivateCallback, cb_print_or_save, (XtPointer)info);

    str = XmStringCreateLocalized(ofstring);
    of_label[act] = XtVaCreateManagedWidget("of", xmLabelGadgetClass,
					    f2child,
					    XmNlabelString, str,
					    XmNsensitive, False,
					    XmNresizable, True,
					    XmNtopAttachment, XmATTACH_WIDGET,
					    XmNtopWidget, range_marked_radio[act],
					    XmNleftAttachment, XmATTACH_WIDGET,
					    XmNleftWidget, page_to_text[act],
					    NULL);
    XmStringFree(str);

    adjust_heights(range_from_to_radio[act],
		   from_label[act], page_from_text[act],
		   to_label[act], page_to_text[act],
		   of_label[act],
		   NULL);

    XtManageChild(f2child);
    XtManageChild(frame2);

    /* box for the OK/Cancel button */
    box = XtVaCreateManagedWidget("box", xmFormWidgetClass,
				  pane,
				  XmNskipAdjust, True, /* don't resize this area */
				  XTranslations, xlats,
				  NULL);
				  
    if (act == FILE_PRINT)
	str = XmStringCreateLocalized("Print");
    else
	str = XmStringCreateLocalized("Save");
    ok_button[act] = XtVaCreateManagedWidget(act == FILE_PRINT ? "print_button" : "save_button",
					     xmPushButtonWidgetClass, box,
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
    if (act == FILE_PRINT)
	XtAddCallback(ok_button[act], XmNactivateCallback, cb_print_or_save, (XtPointer)info);
    else
	XtAddCallback(ok_button[act], XmNactivateCallback, cb_print_or_save, (XtPointer)info);
    
    XtOverrideTranslations(ok_button[act], XtParseTranslationTable("<Key>Return:ArmAndActivate()"));

    str = XmStringCreateLocalized("Cancel");
    cancel_button[act] = XtVaCreateManagedWidget("cancel",
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

    XtOverrideTranslations(range_all_radio[act], xlats);
    XtOverrideTranslations(range_marked_radio[act], xlats);
    XtOverrideTranslations(range_from_to_radio[act], xlats);
    XtOverrideTranslations(ok_button[act], xlats);
    XtOverrideTranslations(cancel_button[act], xlats);

    if (act == FILE_PRINT)
	XtAddCallback(cancel_button[act], XmNactivateCallback, cb_print_cancel, (XtPointer)info);
    else
	XtAddCallback(cancel_button[act], XmNactivateCallback, cb_save_cancel, (XtPointer)info);
    
    { /* make the buttons the same size: */
	Dimension w1, w2;
	XtVaGetValues(ok_button[act], XtNwidth, &w1, NULL);
	XtVaGetValues(cancel_button[act], XtNwidth, &w2, NULL);
	if (w1 < w2)
	    XtVaSetValues(ok_button[act], XtNwidth, w2, NULL);
	else if (w2 > w1)
	    XtVaSetValues(cancel_button[act], XtNwidth, w1, NULL);
    }
    
    if (pageinfo_have_marked_pages()) {
	XmToggleButtonGadgetSetState(range_marked_radio[act], True, True);
    }
    else if (save_pr_info.range_radio == PRINT_MARKED) {
	XmToggleButtonGadgetSetState(range_all_radio[act], True, True);
    }

    XtManageChild(form);
    XtManageChild(pane);
    center_window(top_level_shell, globals.widgets.top_level);
    XtMapWidget(top_level_shell);
    XmProcessTraversal(ok_button[act], XmTRAVERSE_CURRENT);
#endif /* not MOTIF */

    /* set default output format */
    save_save_info.output_format = curr_save_info.output_format =
	output_format_mapping[resource.default_saving_format].fmt;

    if (act == FILE_SAVE)
	update_dvips_options_sensitivity(resource.default_saving_format, top_level_shell);
    
    return top_level_shell;
}
#undef HORIZONTAL_RESIZING_NO
#undef HORIZONTAL_RESIZING_YES
#undef VERTICAL_RESIZING_NO
    

static void
set_target_radio(struct dialog_info *save_info, struct dialog_info *curr_info, printOrSaveActionT act)
{
#ifdef MOTIF
    if (save_info->print_radio > 0 && save_info->print_radio != curr_info->print_radio) {
	XmToggleButtonGadgetSetState(save_info->print_radio == PRINT_TO_PRINTER
				     ? to_printer_radio
				     : to_file_radio_or_label[act], True, True);
    }
#else /* MOTIF */
    UNUSED(act);
    UNUSED(curr_info);
    XawToggleSetCurrent(to_printer_radio, (XtPointer)save_info->print_radio);
#endif /* MOTIF */
}

static void
set_range_radio(struct dialog_info *save_info, struct dialog_info *curr_info, printOrSaveActionT act)
{
#ifdef MOTIF
    if (save_info->range_radio > 0 && save_info->range_radio != curr_info->range_radio) {
	Widget selected = 0;
	switch (save_info->range_radio) {
	case PRINT_ALL:
	case SAVE_ALL:
	    selected = range_all_radio[act];
	    break;
	case PRINT_MARKED:
	case SAVE_MARKED:
	    selected = range_marked_radio[act];
	    break;
	case PRINT_RANGE:
	case SAVE_RANGE:
	    selected = range_from_to_radio[act];
	    break;
	default:
	    ASSERT(0, "wrong button type in set_range_radio()");
	}
	XmToggleButtonGadgetSetState(selected, True, True);
    }
#else /* MOTIF */
    UNUSED(curr_info);
    XawToggleSetCurrent(range_all_radio[act], (XtPointer)save_info->range_radio);
#endif /* MOTIF */
}

void
save_or_print_callback(struct save_or_print_info *info)
{
    printOrSaveActionT act = info->act;
    static char ofstring[1024]; /* should be ample */
    struct dialog_info *save_info = NULL, *curr_info = NULL;

    if (act == FILE_PRINT) {
	save_info = &save_pr_info;
	curr_info = &curr_pr_info;
	if (print_is_active) {
	    XBell(DISP, 0);
	    XRaiseWindow(DISP, XtWindow(print_shell));
	    return;
	}

	if (printlog_raise_active()) {
	    XBell(DISP, 0);
	    return;
	}
    }
    else if (act == FILE_SAVE) {
	save_info = &save_save_info;
	curr_info = &curr_save_info;
	if (save_is_active) {
	    XBell(DISP, 0);
	    XRaiseWindow(DISP, XtWindow(save_shell));
	    return;
	}
    }
    else {
	ASSERT(0, "wrong action type in save_or_print_callback()");
    }

    if (globals.dvi_file.bak_fp == NULL) {
	popup_message(globals.widgets.top_level,
		      MSG_ERR,
		      NULL,
		      "Empty or incomplete DVI file. "
		      "Please select a new DVI file via File -> Open.");
	return;
    }

    if (globals.pageno_correct == 1)
	sprintf(ofstring, "of %d physical pages", total_pages);
    else
	sprintf(ofstring, "of %d to %d pages", globals.pageno_correct, total_pages + globals.pageno_correct - 1);

    if (act == FILE_PRINT && print_shell == NULL) {
	curr_info->print_radio = resource.default_printing_target;
	print_shell = create_print_or_save_window(info, ofstring);
#ifdef MOTIF
	set_sensitivity(resource.default_printing_target);
#endif
    }
    else if (act == FILE_SAVE && save_shell == NULL)
	save_shell = create_print_or_save_window(info, ofstring);
    else { /* if the window was already created */
#ifdef MOTIF
	XmString str;
#endif
	/* XXX: is this actually needed??? */
	/* XXX: To test, remove the following */
	if (!pageinfo_have_marked_pages()) {
	    if (act == FILE_PRINT && save_pr_info.range_radio == PRINT_MARKED) {
		save_pr_info.range_radio = PRINT_ALL;
	    }
	    else if (act == FILE_SAVE && save_save_info.range_radio == SAVE_MARKED) {
		save_save_info.range_radio = SAVE_ALL;
	    }
	}
	
	if (act == FILE_PRINT)
	    set_target_radio(save_info, curr_info, act);
	set_range_radio(save_info, curr_info, act);
#ifdef MOTIF
	str = XmStringCreateLocalized(ofstring);
	XtVaSetValues(of_label[act], XmNlabelString, str, NULL);
	XmStringFree(str);

	/* FIXME: this seems neccessary, otherwise labels are misaligned
	   again if window is closed and opened a second time? */
	adjust_heights(range_from_to_radio[act],
		       from_label[act], page_from_text[act],
		       to_label[act], page_to_text[act],
		       of_label[act],
		       NULL);
	
	if (save_info->dvips_options != NULL) {
	    XtVaSetValues(dvips_options_text[act], XmNvalue, save_info->dvips_options,
			  XmNcursorPosition, strlen(save_info->dvips_options), NULL);
	}
	else {
	    XtVaSetValues(dvips_options_text[act], XmNvalue, "", NULL);
	}
#else /* MOTIF */
	XtVaSetValues(of_label[act], XtNlabel, ofstring, NULL);
	    
	if (save_info->dvips_options != NULL) {
	    XtVaSetValues(dvips_options_text[act], XtNstring, save_info->dvips_options, NULL);
	    XawTextSetInsertionPoint(dvips_options_text[act], strlen(save_info->dvips_options));
	}
	else {
	    XtVaSetValues(dvips_options_text[act], XtNstring, "", NULL);
	}
#endif /* MOTIF */
    }

    get_initial_dialog_values(save_info, act);
    
#ifdef MOTIF
    if (act == FILE_PRINT)
	XtVaSetValues(print_to_printer_text, XmNvalue, save_info->printer_name,
		      XmNcursorPosition, strlen(save_info->printer_name), NULL);

    XtVaSetValues(dvips_options_text[act], XmNvalue, save_info->dvips_options,
		  XmNcursorPosition, strlen(save_info->dvips_options), NULL);

    XtVaSetValues(page_from_text[act], XmNvalue, save_info->from_page,
		  XmNcursorPosition, strlen(save_info->from_page), NULL);

    XtVaSetValues(page_to_text[act], XmNvalue, save_info->to_page,
		  XmNcursorPosition, strlen(save_info->to_page),  NULL);
#else /* MOTIF */
    if (act == FILE_PRINT) {
	XtVaSetValues(print_to_printer_text, XtNstring, save_info->printer_name, NULL);
	XawTextSetInsertionPoint(print_to_printer_text, strlen(save_info->printer_name));
    }

    XtVaSetValues(dvips_options_text[act], XtNstring, save_info->dvips_options, NULL);
    XawTextSetInsertionPoint(dvips_options_text[act], strlen(save_info->dvips_options));
        
    XtVaSetValues(page_from_text[act], XtNstring, save_info->from_page, NULL);
    XawTextSetInsertionPoint(page_from_text[act], strlen(save_info->from_page));

    XtVaSetValues(page_to_text[act], XtNstring, save_info->to_page, NULL);
    XawTextSetInsertionPoint(page_to_text[act], strlen(save_info->to_page));
#endif /* MOTIF */

    if (act == FILE_PRINT) {
	if (pageinfo_have_marked_pages()) {
	    XtSetSensitive(range_marked_radio[FILE_PRINT], True);
	}
	else {
	    XtSetSensitive(range_marked_radio[FILE_PRINT], False);
	}
	ASSERT(save_pr_info.range_radio != PRINT_MARKED || pageinfo_have_marked_pages(),
	       "PRINT_MARKED shouldn't be togglable if there are no marked pages!");
    }
    else if (act == FILE_SAVE) {
	if (pageinfo_have_marked_pages()) {
	    XtSetSensitive(range_marked_radio[FILE_SAVE], True);
	}
	else {
	    XtSetSensitive(range_marked_radio[FILE_SAVE], False);
	}
	ASSERT(save_save_info.range_radio != SAVE_MARKED || pageinfo_have_marked_pages(),
	       "SAVE_MARKED shouldn't be togglable if there are no marked pages!");
    }

    set_outfile_name(save_info, &curr_save_info, act);

#ifdef MOTIF
    XtVaSetValues(print_to_file_text[act], XmNvalue, save_info->file_name,
		  XmNcursorPosition, strlen(save_info->file_name), NULL);
#else /* MOTIF */
    XtVaSetValues(print_to_file_text[act], XtNstring, save_info->file_name, NULL);
    XawTextSetInsertionPoint(print_to_file_text[act], strlen(save_info->file_name));
#endif /* MOTIF */

    if (act == FILE_PRINT) {
	XtPopup(print_shell, XtGrabNone);
	print_is_active = True;
    }
    else {
	XtPopup(save_shell, XtGrabNone);
	save_is_active = True;
    }
}

static int
getpageno(Widget w)
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
  return text value of Widget w in newly allocated memory,
  free()ing old_val as appropriate. Returns NULL if
  text value starts with '\0'.
*/
static char *
get_saved_command(char **old_val, Widget w)
{
    char *val = NULL;
    
    if (*old_val != NULL) {
#if MOTIF
	XtFree((char *)*old_val);
#else
	free((char*)*old_val);
#endif
	*old_val = NULL;
    }
#if MOTIF
    val = XmTextFieldGetString(w);
    if (*val == '\0') {
	XtFree((char *)val);
	val = NULL;
    }
#else
    XtVaGetValues(w, XtNstring, &val, NULL);
    if (*val == '\0') {
	val = NULL;
    }
    else {
	val = xstrdup(val);
    }
#endif
    return val;
}

/* fill curr_print_info and save_print_info with current values */
static void
get_save_window_state(struct dialog_info *save_info, struct dialog_info *curr_info, printOrSaveActionT act)
{
    save_info->print_radio = curr_info->print_radio;
    save_info->range_radio = curr_info->range_radio;

    if (act == FILE_PRINT) {
	save_info->printer_name = get_saved_command(&(save_info->printer_name), print_to_printer_text);
	curr_info->printer_name = get_saved_command(&(curr_info->printer_name), print_to_printer_text);
    }
    save_info->file_name = get_saved_command(&(save_info->file_name), print_to_file_text[act]);
    curr_info->file_name = get_saved_command(&(curr_info->file_name), print_to_file_text[act]);

    save_info->dvips_options = get_saved_command(&(save_info->dvips_options), dvips_options_text[act]);
    curr_info->dvips_options = get_saved_command(&(curr_info->dvips_options), dvips_options_text[act]);

    save_info->from_page = get_saved_command(&(save_info->from_page), page_from_text[act]);
    save_info->to_page = get_saved_command(&(save_info->to_page), page_to_text[act]);
    
    TRACE_GUI((stderr, "saved options:\nprinter: |%s|, file: |%s|, dvips: |%s|, from: |%s|, to: |%s|\n",
	       save_info->printer_name ? save_info->printer_name : "<NULL>", 
	       save_info->file_name ? save_info->file_name : "<NULL>",
	       save_info->dvips_options ? save_info->dvips_options : "<NULL>",
	       save_info->from_page ? save_info->from_page : "<NULL>",
	       save_info->to_page ? save_info->to_page : "<NULL>"));
}

static void
popdown_dialog_and_print_or_save(XtPointer myinfo)
{
    struct save_or_print_info *info = (struct save_or_print_info *)myinfo;
    struct select_pages_info *pinfo = (struct select_pages_info *)info->pinfo;
    struct dialog_info *curr_info = NULL;
    
    /* pop down the dialog */
    if (pinfo->act == FILE_PRINT) {
	curr_info = &save_pr_info;
	cb_print_cancel(NULL, info, NULL);
	/* when printing to file, set ps_out.fname to non-NULL; this will
	   notify called routines that printing should go to the file */
	if (curr_info->print_radio == PRINT_TO_FILE) {
	    pinfo->finfo->ps_out.fname = xstrdup(curr_info->expanded_filename);
	}
    }
    else {
	curr_info = &save_save_info;
	cb_save_cancel(NULL, info, NULL);
    }
    
    if (globals.dvi_file.bak_fp == NULL) { /* shouldn't happen */
	info->message_popup = popup_message(globals.widgets.top_level,
					    MSG_ERR, NULL,
					    "No active DVI file");
	return;
    }

    ASSERT(pinfo->finfo->dvi_in.fp != NULL, "DVI input FILE * must have been set!");

    /* when saving, set the final file names */
    if (curr_info->range_radio == SAVE_RANGE
	|| curr_info->range_radio == SAVE_MARKED
	|| curr_info->range_radio == SAVE_ALL) {
	switch (curr_info->output_format) {
	case FMT_DVI:
	    pinfo->finfo->dvi_out.fname = xstrdup(curr_info->expanded_filename);
	    TRACE_GUI((stderr, "DVI out_name: |%s|", pinfo->finfo->dvi_out.fname));
	    break;
	case FMT_PS:
	    pinfo->finfo->ps_out.fname = xstrdup(curr_info->expanded_filename);
	    TRACE_GUI((stderr, "PS out_name: |%s|", pinfo->finfo->ps_out.fname));
	    break;
	case FMT_PS2PDF:
	    pinfo->finfo->pdf_out.fname = xstrdup(curr_info->expanded_filename);
	    TRACE_GUI((stderr, "PDF out_name: |%s|", pinfo->finfo->pdf_out.fname));
	    break;
	case FMT_ISO_8859_1:
	case FMT_UTF8:
	    pinfo->finfo->txt_out.fname = xstrdup(curr_info->expanded_filename);
	    TRACE_GUI((stderr, "TXT out_name: |%s|", pinfo->finfo->txt_out.fname));
	    break;
	case FMT_NONE:
	    break;
	}
    }
    
    /* add info about page ranges */
    switch (curr_info->range_radio) {
    case SAVE_RANGE:
    case PRINT_RANGE:
	/* convert from 1-based to 0-based */
	pinfo->from = strtoul(curr_info->from_page, (char **)NULL, 10) - 1;
	pinfo->to = strtoul(curr_info->to_page, (char **)NULL, 10) - 1;
	pinfo->callback = check_pagerange;
	break;
    case SAVE_MARKED:
    case PRINT_MARKED:
	pinfo->callback = check_marked;
	break;
    case SAVE_ALL:
    case PRINT_ALL:
	pinfo->callback = NULL;
	break;
    case NO_PAGE_VAL:
	ASSERT(0, "Shouldn't happen: NO_PAGE_VAL passed to popdown_dialog_and_print_or_save()");
	break;
    }

    store_preference(NULL, "dvipsOptionsString", "%s", save_pr_info.dvips_options ? save_pr_info.dvips_options : "");
    if (pinfo->act == FILE_PRINT) {
	store_preference(NULL, "defaultPrintingTarget", "%d", save_pr_info.print_radio);
	store_preference(NULL, "dvipsPrinterString", "%s", save_pr_info.printer_name ? save_pr_info.printer_name : "");
	internal_print(pinfo);
    }
    else {
	store_preference(NULL, "defaultSavingFormat", "%d", save_save_info.output_format);
	internal_save(pinfo, curr_info->output_format);
    }
}

/* Clean up after user has aborted printing/saving */
static void
do_cleanup(XtPointer arg)
{
    struct save_or_print_info *info = (struct save_or_print_info *)arg;
    struct select_pages_info *pinfo = (struct select_pages_info *)info->pinfo;
    struct file_info *finfo = pinfo->finfo;

    cancel_saving(info);
    unlink(finfo->dvi_tmp.fname);
}

/* XXX: is the save_* stuff actually *needed*??? */
static Boolean
validate_and_save_values(struct save_or_print_info *info)
{
    int page_range_begin, page_range_end;
    printOrSaveActionT act = info->act;
    
    if ((act == FILE_PRINT && curr_pr_info.range_radio == PRINT_RANGE)
	|| (act == FILE_SAVE && curr_save_info.range_radio == SAVE_RANGE)) {
		page_range_begin = getpageno(page_from_text[act]);
	page_range_end = getpageno(page_to_text[act]);
	if (page_range_begin + 1 < 1) {
	    info->message_popup =
		popup_message(act == FILE_PRINT ? print_shell : save_shell,
			      MSG_ERR,
			      "Please specify a valid page range.",
			      "Invalid page range: start (%d) < 1.",
			      page_range_begin + 1);
	    return False;
	}
	else if (page_range_begin > page_range_end) {
	    info->message_popup =
		popup_message(act == FILE_PRINT ? print_shell : save_shell,
			      MSG_ERR,
			      "Please specify a valid page range.",
			      "Invalid page range: start (%d) > end (%d).",
			      page_range_begin + 1, page_range_end + 1);
	    return False;
	}
	else if (page_range_end + 1 > total_pages) {
	    info->message_popup =
		popup_message(act == FILE_PRINT ? print_shell : save_shell,
			      MSG_ERR,
			      "Please specify a valid page range.",
			      "Invalid page range: end (%d) > total_pages (%d).",
			      page_range_end + 1, total_pages);
	    return False;
	}

	if (act == FILE_PRINT) {
	    curr_pr_info.from_page = xrealloc(curr_pr_info.from_page, LENGTH_OF_INT + 1);
	    curr_pr_info.to_page = xrealloc(curr_pr_info.to_page, LENGTH_OF_INT + 1);
	    sprintf(curr_pr_info.from_page, "%d", page_range_begin);
	    sprintf(curr_pr_info.to_page, "%d", page_range_end);
	}
	else {
	    curr_save_info.from_page = xrealloc(curr_save_info.from_page, LENGTH_OF_INT + 1);
	    curr_save_info.to_page = xrealloc(curr_save_info.to_page, LENGTH_OF_INT + 1);
	    sprintf(curr_save_info.from_page, "%d", page_range_begin);
	    sprintf(curr_save_info.to_page, "%d", page_range_end);
	}
    }
    
    if (act == FILE_PRINT)
	get_save_window_state(&save_pr_info, &curr_pr_info, act);
    else
	get_save_window_state(&save_save_info, &curr_save_info, act);

    return True;
}

/*
 * Routines for freeing allocated resources in file_info:
 * close file pointers, and free file name memory.
 */
static void
free_file_info(struct file_IO *info)
{
    if (info->fp != NULL) {
	fclose(info->fp);
	info->fp = NULL;
    }
    free(info->fname);
    info->fname = NULL;
}

static void
reset_file_info(struct file_info *finfo)
{
    free_file_info(&(finfo->dvi_in));
    free_file_info(&(finfo->dvi_tmp));
    free_file_info(&(finfo->dvi_out));
    free_file_info(&(finfo->ps_out));
    free_file_info(&(finfo->pdf_out));
    free_file_info(&(finfo->txt_out));
}

/*
  First round of sanity checks. Note the use of curr_pr_info here;
  this is before the values are saved to save_pr_info.
*/
static void
print_precheck1(XtPointer myinfo)
{
    /* Initialize structures with defaults. Note: structures are
       static, so that they survive between calls, and to make memory
       management with callbacks (and all the stuff in print-internal.c)
       easier.
    */
    struct save_or_print_info *info = (struct save_or_print_info *)myinfo;
    printOrSaveActionT act = info->act;
    static struct select_pages_info pinfo = {
	FILE_ALL, 0, 0, NULL, check_marked, { 0, NULL }, NO_ERROR
    };
    static struct file_info finfo = {
	{ NULL, NULL },
	{ NULL, NULL },
	{ NULL, NULL },
	{ NULL, NULL },
	{ NULL, NULL },
	{ NULL, NULL }
    };
    char *ok_button_str = NULL;
    FILE *fin = NULL;

    /* Return if there are active popups; otherwise, the print process
       may get messed up */
    if (raise_message_windows()) {
	XBell(DISP, 0);
/* 	popup_message(pinfo.act == FILE_PRINT ? print_shell : save_shell, */
/* 		      MSG_WARN, NULL, */
/* 		      "Please close other open message windows first!"); */
	return;
    }

    /* free all pointers in case we've been called again */
    reset_file_info(&finfo);

    pinfo.finfo= &finfo; /* pass finfo in data field of pinfo to callbacks etc. */
    pinfo.act = act;
    info->pinfo = &pinfo;
    
    /* Validate page ranges, and if they are correct, save them to the
     * appropriate save_save_info/save_print_info structure.
     * NOTE: From here on, always use the save_* variants to retrieve values!
     */
    if (!validate_and_save_values(info))
	return;
    
    /* We need to create a temporary DVI file containing the selected or all pages
       if we want to do one of the following:
       - print the marked pages
       - save to a DVI file
       - print only the marked pages to a PS or a PDF file.
     */
    if ((pinfo.act == FILE_PRINT && curr_pr_info.range_radio == PRINT_MARKED)
	|| (pinfo.act == FILE_SAVE
	    && (save_save_info.output_format == FMT_DVI
		|| ((save_save_info.output_format == FMT_PS || save_save_info.output_format == FMT_PS2PDF)
		    && save_save_info.range_radio == SAVE_MARKED)))) {
	/* need to create a temporary DVI file containing the selected pages */
	int tmp_fd = xdvi_temp_fd(&(finfo.dvi_tmp.fname)); /* this allocates finfo.dvi_tmp.fname */

	if (tmp_fd == -1) {
	    info->message_popup =
		popup_message(pinfo.act == FILE_PRINT ? print_shell : save_shell,
			      MSG_ERR, NULL,
			      "Couldn't create temporary DVI file for printing: %s", strerror(errno));
	    return;
	}
	/* don't use XFOPEN here, since we don't want to treat an error in opening the file as fatal. */
	/* `b' in mode won't hurt ... */
	if ((finfo.dvi_tmp.fp = try_fdopen(tmp_fd, "wb+")) == NULL) { /* failure */
	    info->message_popup =
		popup_message(pinfo.act == FILE_PRINT ? print_shell : save_shell,
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
	|| !find_postamble(fin, &(pinfo.errflag))) {
	Boolean could_not_open = False;
	if (fin == NULL)
	    could_not_open = True;
	if (fin)
	    fclose(fin);
	
	/* if we can't use the temporary file as source for our copy, this is a fatal error */
	if (!resource.use_temp_fp) {
	    info->message_popup =
		popup_message(pinfo.act == FILE_PRINT ? print_shell : save_shell,
			      MSG_ERR, NULL,
			      "Could not copy DVI file %s: The file %s",
			      globals.dvi_name, could_not_open ? "doesn't exist any more" : "seems to be corrupted");
	    return;
	}
	
	/* else, we'll try to use the cached copy of the DVI file */
	if ((fin = fopen(get_tmp_dvi_name(), "rb")) == NULL) {
	    info->message_popup =
		popup_message(pinfo.act == FILE_PRINT ? print_shell : save_shell,
			      MSG_ERR, NULL,
			      "Something's very wrong here - opening of both the "
			      "original DVI file and the cached copy failed!");
	    return;
	}
	
	if (!find_postamble(fin, &(pinfo.errflag))) {
	    info->message_popup =
		popup_message(pinfo.act == FILE_PRINT ? print_shell : save_shell,
			      MSG_ERR, NULL,
			      "Shouldn't happen: Cached copy of the DVI file seems corrupted!");
	    return;
	}
	finfo.dvi_in.fp = fin;
	
	/* tell user about it */
	if (pinfo.act == FILE_PRINT)
	    ok_button_str = "Print Copy";
	else
	    ok_button_str = "Save Copy";
	
	info->message_popup =
	    choice_dialog(pinfo.act == FILE_PRINT ? print_shell : save_shell,
			  MSG_QUESTION, NULL,
#ifndef MOTIF
			  NULL, /* TODO: binding for RET? */
#endif /* MOTIF */
			  NULL, NULL, /* no pre_callbacks */
			  ok_button_str, print_precheck2, (XtPointer)info,
			  "Cancel", do_cleanup, (XtPointer)info,
			  "The DVI file %s %s. Do you want to %s a cached copy of the file?",
			  globals.dvi_name,
			  could_not_open ? "doesn't exist any more" : "seems to be corrupted",
			  pinfo.act == FILE_PRINT ? "print" : "save");
    }
    else { /* original DVI file is OK, copy it */
	finfo.dvi_in.fp = fin;
	print_precheck2((XtPointer)info);
    }
}

static void
print_precheck2(XtPointer myinfo)
{
    struct save_or_print_info *info = (struct save_or_print_info *)myinfo;
    char *expanded_filename = NULL;
    struct select_pages_info *pinfo = (struct select_pages_info *)info->pinfo;
    printOrSaveActionT act = pinfo->act;
    const char *fname;
    
    /* Return if there are active popups; otherwise, the print process
       will be messed up. */
    if (raise_message_windows()) {
	XBell(DISP, 0);
	return;
    }

    /* expand and canonicalize path name */
    if (act == FILE_PRINT)
	fname = save_pr_info.file_name;
    else
	fname = save_save_info.file_name;

    if (fname == NULL) {
	info->message_popup =
	    popup_message(act == FILE_PRINT ? print_shell : save_shell,
			  MSG_WARN,
			  NULL,
			  "No filename specified!");
	return;
    }
    expanded_filename = expand_homedir(fname); /* this allocates expanded_filename */
    if (expanded_filename == NULL) {
	info->message_popup =
	    popup_message(act == FILE_PRINT ? print_shell : save_shell,
			  MSG_WARN,
			  "Please specify either a filename or a full path, without using \"~\" or \"~user\".",
			  "Couldn't expand filename \"%s\" to a full path.",
			  fname);
	return;
    }
    
    if (act == FILE_PRINT) {
	free(save_pr_info.expanded_filename);
	save_pr_info.expanded_filename = expanded_filename;
    }
    else {
	free(save_save_info.expanded_filename);
	save_save_info.expanded_filename = expanded_filename;
    }
    

    if (act == FILE_SAVE && save_save_info.output_format == FMT_DVI) { /* canonicalize filename */
	char *tmp = expand_filename_append_dvi(save_save_info.expanded_filename, USE_CWD_PATH, False);
	if (strcmp(tmp, globals.dvi_name) == 0) {
	    info->message_popup =
		popup_message(act == FILE_PRINT ? print_shell : save_shell,
			      MSG_ERR,
			      NULL,
			      "Cannot overwrite the current DVI file (%s). "
			      "Please select a different file name.",
			      save_save_info.expanded_filename);
	    free(tmp);
	    return;
	}
	free(save_save_info.expanded_filename);
	save_save_info.expanded_filename = tmp;
    }
    if (act == FILE_SAVE || (act == FILE_PRINT && curr_pr_info.print_radio == PRINT_TO_FILE)) {
	/* check whether to clobber existing file */
	struct stat statbuf;

	if ((act == FILE_SAVE
	     && stat(save_save_info.expanded_filename, &statbuf) == 0
	     && S_ISREG(statbuf.st_mode))
	    || (act == FILE_PRINT
		&& stat(save_pr_info.expanded_filename, &statbuf) == 0
		&& S_ISREG(statbuf.st_mode))) {

	    info->message_popup =
		choice_dialog(act == FILE_PRINT ? print_shell : save_shell,
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
			      "%s already exists.\nDo you want to replace it?",
			      act == FILE_SAVE
			      ? save_save_info.expanded_filename
			      : save_pr_info.expanded_filename);
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

/* called from pagesel.c if user marks or unmarks a page in the page list */
void
notify_print_dialog_have_marked(Boolean flag)
{
    /* Note: check for *both* windows, save and print: */

    static pageRadioT old_value = NO_PAGE_VAL;
    
    if (save_is_active) {
	if (!flag && curr_save_info.range_radio == SAVE_MARKED) {
	    /* switch button from `marked' to its previous value */
	    switch (old_value) {
	    case SAVE_ALL:
#ifdef MOTIF
		XmToggleButtonGadgetSetState(range_all_radio[FILE_SAVE], True, True);
#else
		XawToggleSetCurrent(range_all_radio[FILE_SAVE], (XtPointer)old_value);
#endif
		break;
	    case SAVE_RANGE:
#ifdef MOTIF
		XmToggleButtonGadgetSetState(range_from_to_radio[FILE_SAVE], True, True);
#else
		XawToggleSetCurrent(range_from_to_radio[FILE_SAVE], (XtPointer)old_value);
#endif
		break;
	    default: break;
	    }
	}
	else if (flag) {
	    /* switch button to `marked', saving the current value */
	    if (curr_save_info.range_radio != SAVE_MARKED) /* guard for second invocation on mouse release */
		old_value = curr_save_info.range_radio;
#ifdef MOTIF
	    XmToggleButtonGadgetSetState(range_marked_radio[FILE_SAVE], True, True);
#else
	    XawToggleSetCurrent(range_marked_radio[FILE_SAVE], (XtPointer)SAVE_MARKED);
#endif /* MOTIF */
	}
	XtSetSensitive(range_marked_radio[FILE_SAVE], flag);
    }
    
    if (print_is_active) {
	if (!flag && curr_pr_info.range_radio == PRINT_MARKED) {
	    /* switch button from `marked' to its previous value */
	    switch (old_value) {
	    case PRINT_ALL:
#ifdef MOTIF
		XmToggleButtonGadgetSetState(range_all_radio[FILE_PRINT], True, True);
#else
		XawToggleSetCurrent(range_all_radio[FILE_PRINT], (XtPointer)old_value);
#endif
		break;
	    case PRINT_RANGE:
#ifdef MOTIF
		XmToggleButtonGadgetSetState(range_from_to_radio[FILE_PRINT], True, True);
#else
		XawToggleSetCurrent(range_from_to_radio[FILE_PRINT], (XtPointer)old_value);
#endif
		break;
	    default: break;
	    }
	}
	else if (flag) {
	    /* switch button to `marked', saving the current value */
	    if (curr_pr_info.range_radio != PRINT_MARKED) /* guard for second invocation on mouse release */
		old_value = curr_pr_info.range_radio;
#ifdef MOTIF
	    XmToggleButtonGadgetSetState(range_marked_radio[FILE_PRINT], True, True);
#else
	    XawToggleSetCurrent(range_marked_radio[FILE_PRINT], (XtPointer)PRINT_MARKED);
#endif /* MOTIF */
	}
	XtSetSensitive(range_marked_radio[FILE_PRINT], flag);
    }
}
