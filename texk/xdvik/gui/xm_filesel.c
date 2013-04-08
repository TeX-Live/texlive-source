/*
 * Copyright (c) 2001-2004 Marcin Dalecki and others
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
 * Implementation of the File selection dialogue for the Motif GUI.
 */

#include "xdvi-config.h"
#include "xdvi.h"
#include "sfSelFile.h"

#include "dvi.h"
#include "string-utils.h"
#include "util.h"
#include "events.h"
#include "message-window.h"
#include "dvi-init.h" /* for dviErrFlagT */
#include "x_util.h"

#if defined(MOTIF) /* entire file */

#include <Xm/FileSB.h>
#include <Xm/Form.h>
#include <Xm/ToggleBG.h>

#if HAVE_XKB_BELL_EXT
# include <X11/XKBlib.h>
# define XdviBell(display, window, percent)	\
	 XkbBell(display, window, percent, (Atom) None)
#else
# define XdviBell(display, window, percent)	XBell(display, percent)
#endif

/* static Widget dialog = NULL; */
/* static char *browse_fname = NULL; */

/*
 * Process callback from Dialog cancel actions.
 */
static void
cancel_callback(Widget w,
		XtPointer client_data,
		XtPointer call_data)
{
    struct filesel_callback *callback = (struct filesel_callback *)client_data;

    UNUSED(w);
    UNUSED(call_data);
#if 0
    /* DON'T reset browse_fname, so that user gets the current
       value as default next time when he cancels now */
    if (callback->browse_fname != NULL)
    {
	XtFree(callback->browse_fname);
	callback->browse_fname = NULL;
    }
#endif

    XtUnmanageChild(callback->shell);
    if (callback->exit_on_cancel) {
	exit(0);
    }
}

/*
 * Process callback from Dialog actions.
 */

static void
accept_callback(Widget w,
		XtPointer client_data,
		XtPointer call_data)
{
    XmFileSelectionBoxCallbackStruct *fcb;
    struct filesel_callback *callback;
    
    UNUSED(w);

    ASSERT(client_data != NULL, "struct filesel_callback pointer expected in client data");
    callback = (struct filesel_callback *)client_data;

    /* get the filename from the file selection box */
    fcb = (XmFileSelectionBoxCallbackStruct *)call_data;
    if (callback->browse_fname != NULL) {
	XtFree(callback->browse_fname);
	callback->browse_fname = NULL;
    }
    XmStringGetLtoR(fcb->value, G_charset, &callback->browse_fname);

    if (0 && callback->must_exist) {
	FILE *tmp_fp = XFOPEN(callback->browse_fname, "r");
	dviErrFlagT errflag = NO_ERROR;
	if (tmp_fp == NULL) {
	    popup_message(XtParent(callback->shell),
			  MSG_ERR, NULL, "Could not open %s: %s.\n",
			  callback->browse_fname, strerror(errno));
	    /* leave file selection box open */
	    return;
	}
	else if (!process_preamble(tmp_fp, &errflag)
		 || !find_postamble(tmp_fp, &errflag)
		 || !read_postamble(tmp_fp, &errflag, True
#if DELAYED_MKTEXPK
				    , False
#endif
				    )) {
	    popup_message(XtParent(callback->shell),
			  MSG_ERR, NULL, "Error opening %s:\n%s.",
			  callback->browse_fname, get_dvi_error(errflag));
	    fclose(tmp_fp);
	    /* leave file selection box open */
	    return;
	}
	else { /* file is OK */
	    fclose(tmp_fp);
	}
    }

    /* success; close dialog, and call our callback */
    XtUnmanageChild(callback->shell);
    callback->func_ptr(callback->browse_fname, callback->data);
}

static void
cb_open_new_window(Widget w, XtPointer client_data, XtPointer call_data)
{
    UNUSED(client_data);
    UNUSED(call_data);
    
    if (XmToggleButtonGadgetGetState(w))
	resource.filesel_open_new_window = True;
    else
	resource.filesel_open_new_window = False;
    store_preference(NULL, "fileselOpenNewWindow", "%d", resource.filesel_open_new_window);
}

void raise_file_selector(void)
{
    /* dummy */
    return;
}

void
XsraSelFilePopup(struct filesel_callback *callback)
{
    if (XtIsManaged(callback->shell)) {
	XdviBell(DISP, XtWindow(callback->shell), 10);
	XRaiseWindow(DISP, XtWindow(callback->shell));
	return;
    }
    else {
#define ARG_CNT 4
	XmString filemask = NULL;
	XmString directory = NULL;
	Arg args[ARG_CNT];
	int i = 0;
	char *path, *ptr;

	/* only show files matching our mask */
	filemask = XmStringCreateLtoR((char *)callback->filemask, G_charset);
	XtSetArg(args[i], XmNpattern, filemask); i++;

	/* set directory to last directory used */
	if (callback->browse_fname == NULL) {
	    ASSERT(callback->init_path != NULL, "callback->init_path mustn't be NULL!");
	    callback->browse_fname = xt_strdup(callback->init_path);
	}
	path = xstrdup(callback->browse_fname);
	ptr = strrchr(path, '/');
	if (ptr != NULL)
	    *ptr = '\0';
	directory = XmStringCreateLtoR(path, G_charset);
	XtSetArg(args[i], XmNdirectory, directory); i++;
	free(path);
	
    
	ASSERT(i < ARG_CNT, "args list too short");
	XtSetValues(callback->shell, args, (Cardinal)i);

	free(filemask);
	free(directory);
    
	XtManageChild(callback->shell);
#undef ARG_CNT
    }
}

Widget
XsraSelFile(Widget parent, struct filesel_callback *callback)
{
    Widget dialog = XmCreateFileSelectionDialog(parent, "file", NULL, 0);
    
    XtVaSetValues(XtParent(dialog), XmNtitle, callback->title, NULL);
    
    XtAddCallback(dialog, XmNokCallback, accept_callback, (XtPointer)callback);
    XtAddCallback(dialog, XmNcancelCallback, cancel_callback, (XtPointer)callback);

    /* When opening a DVI file, offer to open in new window */
    if (callback->must_exist) {
	Widget form, button;
	XmString test;
	form = XtVaCreateManagedWidget("new_window_form", xmFormWidgetClass, dialog, NULL);
	test = XmStringCreateLocalized("Open file in new window");
	button = XtVaCreateManagedWidget(Xdvi_NEW_WINDOW_RADIO_NAME, xmToggleButtonGadgetClass, form,
					 XmNlabelString, test,
					 XmNindicatorType, XmN_OF_MANY,
					 XmNset, resource.filesel_open_new_window,
					 NULL);
	XmStringFree(test);
	XtAddCallback(button, XmNvalueChangedCallback, cb_open_new_window, (XtPointer)NULL);
    }

    
    /* We have no help in this window, so hide help button */
    XtUnmanageChild(XmFileSelectionBoxGetChild(dialog, (unsigned char)XmDIALOG_HELP_BUTTON));

    return dialog;
}

#else
/* silence `empty compilation unit' warnings */
static void bar(void); static void foo(void) { bar(); } static void bar(void) { foo(); }
#endif /* defined(MOTIF) */
