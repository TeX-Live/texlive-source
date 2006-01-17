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

#if defined(MOTIF) /* entire file */

#include <Xm/FileSB.h>

static Widget dialog = NULL;
static char *browse_fname = NULL;

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
    if (browse_fname != NULL)
    {
	XtFree(browse_fname);
	browse_fname = NULL;
    }
#endif

    XtUnmanageChild(dialog);
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

    if (browse_fname != NULL) {
	XtFree(browse_fname);
	browse_fname = NULL;
    }
    fcb = (XmFileSelectionBoxCallbackStruct *)call_data;

    /* get the filename from the file selection box */
    XmStringGetLtoR(fcb->value, G_charset, &browse_fname);

    if (callback->must_exist) {
	FILE *tmp_fp = XFOPEN(browse_fname, "r");
	dviErrFlagT errflag = NO_ERROR;
	if (tmp_fp == NULL) {
	    popup_message(XtParent(dialog),
			  MSG_ERR, NULL, "Could not open %s: %s.\n",
			  browse_fname, strerror(errno));
	    /* leave file selection box open */
	    return;
	}
	else if (!process_preamble(tmp_fp, &errflag)
		 || !find_postamble(tmp_fp, &errflag)
		 || !read_postamble(tmp_fp, &errflag, True)) {
	    popup_message(XtParent(dialog),
			  MSG_ERR, NULL, "Error opening %s:\n%s.",
			  browse_fname, get_dvi_error(errflag));
	    fclose(tmp_fp);
	    /* leave file selection box open */
	    return;
	}
	else { /* file is OK */
	    fclose(tmp_fp);
	}
    }

    /* success; close dialog, and call our callback */
    XtUnmanageChild(dialog);
    callback->func_ptr(browse_fname, callback->data);
}

void raise_file_selector(void)
{
    /* dummy */
    return;
}

void
XsraSelFile(Widget parent, struct filesel_callback *callback)
{
#define ARG_CNT 4
    XmString filemask = NULL;
    XmString directory = NULL;
    Arg args[ARG_CNT];
    int i = 0;

    if (dialog == NULL) {
	dialog = XmCreateFileSelectionDialog(parent, "file", NULL, 0);
	/* Set ret_dialog to the DialogShell parent, not the command widget
	   returned by the Motif routines.  We need the DialogShell, otherwise
	   Motif will crash if ret_dialog is used as parent for popup_message()
	   windows!!!
	*/
	XtVaSetValues(XtParent(dialog), XmNtitle, callback->title, NULL);
	
	/*      my_must_exist = must_exist; */
	/*      XtAddCallback(dialog, XmNokCallback, accept_callback, (XtPointer)&my_must_exist); */
	XtAddCallback(dialog, XmNokCallback, accept_callback, (XtPointer)callback);
	XtAddCallback(dialog, XmNcancelCallback, cancel_callback, (XtPointer)callback);
	/* We have no help in this window, so hide help button */
	XtUnmanageChild(XmFileSelectionBoxGetChild(dialog, (unsigned char)XmDIALOG_HELP_BUTTON));
    }
    else if (XtIsManaged(dialog)) {
	XBell(DISP, 10);
	XRaiseWindow(DISP, XtWindow(dialog));
	return;
    }

    /* only show files matching our mask */
    filemask = XmStringCreateLtoR((char *)callback->filemask, G_charset);
    XtSetArg(args[i], XmNpattern, filemask); i++;
    
    /* set directory to last directory used */
    if (browse_fname != NULL) {
	char *path = xstrdup(browse_fname);
	char *ptr = strrchr(path, '/');
	if (ptr != NULL)
	    *ptr = '\0';
	directory = XmStringCreateLtoR(path, G_charset);
	XtSetArg(args[i], XmNdirectory, directory); i++;
	free(path);
    }
    
    ASSERT(i < ARG_CNT, "args list too short");
    XtSetValues(dialog, args, (Cardinal)i);

    free(filemask);
    free(directory);
    
    XtManageChild(dialog);
#undef ARG_CNT
}

#else
/* silence `empty compilation unit' warnings */
static void bar(void); static void foo() { bar(); } static void bar(void) { foo(); }
#endif /* defined(MOTIF) */
