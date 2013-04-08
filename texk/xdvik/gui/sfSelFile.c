/*
 * Copyright 1989 Software Research Associates, Inc., Tokyo, Japan
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Software Research Associates not be used
 * in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Software Research Associates
 * makes no representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * SOFTWARE RESEARCH ASSOCIATES DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL SOFTWARE RESEARCH ASSOCIATES BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Erik M. van der Poel
 *         Software Research Associates, Inc., Tokyo, Japan
 *         erik@sra.co.jp
 */

#include "xdvi-config.h"

#include "xdvi.h"
#include "dvi-init.h" /* for dviErrFlagT */
#include "message-window.h"

#include "kpathsea/c-stat.h"

#include <string.h>

#include "sfDir.h"
#include "sfPath.h"
#include "sfDraw.h"
#include "sfSelFile.h"
#include "util.h"
#include "xlwradio.h"
#include "x_util.h"

#include <ctype.h>
#include "kpathsea/c-fopen.h"
#include "kpathsea/c-stat.h"

#if !defined(MOTIF) /* entire file */

#include <errno.h>

#ifdef	X_NOT_STDC_ENV
extern int errno;
#endif

#if HAVE_XKB_BELL_EXT
# include <X11/XKBlib.h>
# define sfBell(display, window, percent)	\
	 XkbBell(display, window, percent, (Atom) None)
#else
# define sfBell(display, window, percent)	XBell(display, percent)
#endif

#define SEL_FILE_CANCEL		-1
#define SEL_FILE_OK		0
#define SEL_FILE_NULL		1
#define SEL_FILE_TEXT		2

/*
 * Author's address:
 *
 *     erik@sra.co.jp
 *                                            OR
 *     erik%sra.co.jp@uunet.uu.net
 *                                            OR
 *     erik%sra.co.jp@mcvax.uucp
 *                                            OR
 *     try junet instead of co.jp
 *                                            OR
 *     Erik M. van der Poel
 *     Software Research Associates, Inc.
 *     1-1-1 Hirakawa-cho, Chiyoda-ku
 *     Tokyo 102 Japan. TEL +81-3-234-2692
 */

#include <sys/param.h>
#include <X11/cursorfont.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Composite.h>
#include <X11/Shell.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Text.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Scrollbar.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Cardinals.h>


#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif /* ndef MAXPATHLEN */

/* global vars for communication with sfDraw.c */
char SFstartDir[MAXPATHLEN], SFcurrentPath[MAXPATHLEN], SFcurrentDir[MAXPATHLEN];
Widget selFileField, selFileForm, selFileHScroll, selFileHScrolls[3], selFileLists[3], selFileVScrolls[3];
Display *SFdisplay;
Pixel SFfore, SFback;
XSegment SFsegs[2], SFcompletionSegs[2];
XawTextPosition SFtextPos;
int SFupperX, SFlowerY, SFupperY;
int SFtextX, SFtextYoffset;
int SFentryWidth, SFentryHeight;
int SFlineToTextH = 3;
int SFlineToTextV = 3;
int SFbesideText = 3;
int SFaboveAndBelowText = 2;
int SFcharsPerEntry = 15;
int SFlistSize = 10;
int SFworkProcAdded = 0;
XtAppContext SFapp;
int SFpathScrollWidth, SFvScrollHeight, SFhScrollWidth;
char SFtextBuffer[MAXPATHLEN];
XtIntervalId SFdirModTimerId;
int (*SFfunc) ();

static int SFstatus = SEL_FILE_NULL;

static Widget selFile, selFileCancel, selFileOK, selFilePrompt;
/* For file filter. */
static Widget selFileLabel, selFileMask, selFileHide;

#define MASKWIDTH 16
static char fileMask[MASKWIDTH + 2] = "*.dvi";

static Atom SFwmDeleteWindow;

static char *oneLineTextEditTranslations =
"<Key>Return: redraw-display()\n"
"Ctrl<Key>M: redraw-display()\n";

#if !defined (HAVE_STRERROR) && !defined (strerror)
static char *
strerror(int errnum)
{
    return 0 < errnum && errnum <= sys_nerr
	? sys_errlist[errnum] : "Unknown system error";
}
#endif /* not HAVE_STRERROR && not strerror */

void
raise_file_selector(void)
{
    if (selFile != NULL && XtIsManaged(selFile)) {
	sfBell(DISP, XtWindow(selFile), 10);
	XRaiseWindow(DISP, XtWindow(selFile));
	return;
    }
}

static void
SFexposeList(Widget w, XtPointer n, XEvent *event, Boolean *cont)
{
    UNUSED(w);
    UNUSED(cont);
    
    if ((event->type == NoExpose) || event->xexpose.count) {
	return;
    }

    SFdrawList((ptrdiff_t)n, SF_DO_NOT_SCROLL);
}


static void
cb_open_new_window(Widget w, XtPointer client_data, XtPointer call_data)
{
    Boolean set;
    
    UNUSED(client_data);
    UNUSED(call_data);

    XtVaGetValues(w, XtNstate, &set, NULL);

    if (set)
	resource.filesel_open_new_window = True;
    else
	resource.filesel_open_new_window = False;
    store_preference(NULL, "fileselOpenNewWindow", "%d", resource.filesel_open_new_window);
}

static void
SFmodVerifyCallback(Widget w, XtPointer client_data, XEvent *event, Boolean *cont)
{
    char buf[2];

    UNUSED(w);
    UNUSED(client_data);
    UNUSED(cont);
    
    if ((XLookupString(&(event->xkey), buf, 2, NULL, NULL) == 1) &&
	((*buf) == '\r')) {
	SFstatus = SEL_FILE_OK;
    }
    else {
	SFstatus = SEL_FILE_TEXT;
    }
}

static void
SFokCallback(Widget w, XtPointer cl, XtPointer cd)
{
    UNUSED(w);
    UNUSED(cl);
    UNUSED(cd);
    
    SFstatus = SEL_FILE_OK;
}

static XtCallbackRec SFokSelect[] = {
    {SFokCallback, (XtPointer) NULL},
    {NULL, (XtPointer) NULL},
};

static void
SFcancelCallback(Widget w, XtPointer cl, XtPointer cd)
{
    UNUSED(w);
    UNUSED(cl);
    UNUSED(cd);
    
    SFstatus = SEL_FILE_CANCEL;
}

static XtCallbackRec SFcancelSelect[] = {
    {SFcancelCallback, (XtPointer) NULL},
    {NULL, (XtPointer) NULL},
};

static void
SFdismissAction(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
    UNUSED(w);
    UNUSED(params);
    UNUSED(num_params);
    
    if (event->type == ClientMessage && (unsigned)(event->xclient.data.l[0]) != SFwmDeleteWindow)
	return;

    SFstatus = SEL_FILE_CANCEL;
}

static char *wmDeleteWindowTranslation = "<Message>WM_PROTOCOLS: SelFileDismiss()\n";

static XtActionsRec sf_actions[] = {
    {"SelFileDismiss", SFdismissAction},
};

/* Don't show files that don't get through the filter.  */

/* return 1 if file is masked (mask does not match filename), 0 otherwise */
static int
maskFile(const char *mask, char *filename)
{
    int c, c1;

    while ((c = *mask++)) {
	if (c == '*') {
	    while ((c1 = *mask++)) {
		if (c1 != '*') {
		    if (!(*filename))
			return 1;
		    if (c1 != '?') {
			while ((filename = strchr(filename, c1))) {
			    if (!maskFile(mask, ++filename))
				return 0;
			}
			return 1;
		    }
		    else
			filename++;
		}
	    }
	    return 0;
	}
	if (c == '?') {
	    if (!*filename)
		return 1;
	}
	else if (c != *filename)
	    return 1;
	filename++;
    }
    return (*filename) ? 1 : 0;
}

Boolean hideFlag = False;
static int
showEntry(char *entryReal, char **entryShown, struct stat *statBuf)
{
    UNUSED(entryShown);

    if ((hideFlag && entryReal[0] == '.') || (!S_ISDIR(statBuf->st_mode) && maskFile(fileMask, entryReal)))
	return 0;
    entryReal[strlen(entryReal)] = SFstatChar(statBuf);
    return 1;
}

static void
maskChanged(Widget w, XtPointer client_data, XEvent *call_data, Boolean *cont)
{
    char buf[2];
    SFDir *dir;

    UNUSED(w);
    UNUSED(client_data);
    UNUSED(cont);
    
    if ((XLookupString((XKeyPressedEvent *)call_data, buf, 2, NULL, NULL) == 1) && ((*buf) == '\r')) {
	for (dir = &(SFdirs[SFdirEnd - 1]); dir >= SFdirs; dir--)
	    *(dir->dir) = 0;	/* force a re-read */
	SFupdatePath();
    }
}

static void
hideFiles(Widget w, XtPointer client_data, XtPointer call_data)
{
    SFDir *dir;
    SFEntry *entry;

    UNUSED(client_data);
    UNUSED(call_data);
    
    hideFlag = !hideFlag;
    if (hideFlag) {
	XtVaSetValues(w, XtNlabel, "hidden", NULL);
	for (dir = &(SFdirs[SFdirEnd - 1]); dir >= SFdirs; dir--) {
	    if (!(dir->nEntries))
		continue;
	    dir->vOrigin = 0;
	    for (entry = &(dir->entries[dir->nEntries - 1]);
		 entry >= dir->entries; entry--)
		entry->statDone = 0;
	    SFdrawLists(SF_DO_SCROLL);
	}
    }
    else {
	XtVaSetValues(w, XtNlabel, "shown", NULL);
	for (dir = &(SFdirs[SFdirEnd - 1]); dir >= SFdirs; dir--)
	    *(dir->dir) = 0;	/* force a re-read */
	SFupdatePath();
    }
}


static Widget
SFcreateWidgets(Widget parent, struct filesel_callback *callback)
{
    Widget open_menu = NULL;
    Cardinal i, n;
    int listWidth, listHeight;
    int listSpacing = 10;
    int scrollThickness = 15;
    int hScrollX, hScrollY;
    int vScrollX, vScrollY;
    Cursor xtermCursor, sbRightArrowCursor, arrowCursor;
    Arg arglist[20];
    Widget paned, box;
    
    i = 0;
    XtSetArg(arglist[i], XtNtransientFor, parent); i++;
    XtSetArg(arglist[i], XtNtitle, callback->title);	   i++;

    selFile = XtAppCreateShell("xdviSelFile", "XdviSelFile",
			       transientShellWidgetClass, SFdisplay,
			       arglist, i);

    /* Add WM_DELETE_WINDOW protocol */
    XtAppAddActions(XtWidgetToApplicationContext(selFile),
		    sf_actions, XtNumber(sf_actions));
    XtOverrideTranslations(selFile,
			   XtParseTranslationTable(wmDeleteWindowTranslation));

    paned = XtVaCreateManagedWidget("paned", panedWidgetClass, selFile, NULL);

    i = 0;
    XtSetArg(arglist[i], XtNdefaultDistance, 6);	i++;
    
    selFileForm = XtCreateManagedWidget("selFileForm",
					formWidgetClass, paned, arglist, i);

    i = 0;
    XtSetArg(arglist[i], XtNlabel, callback->prompt);	i++;
    XtSetArg(arglist[i], XtNresizable, True);		i++;
    XtSetArg(arglist[i], XtNtop, XtChainTop);		i++;
    XtSetArg(arglist[i], XtNbottom, XtChainTop);	i++;
    XtSetArg(arglist[i], XtNleft, XtChainLeft);		i++;
    XtSetArg(arglist[i], XtNright, XtChainLeft);	i++;
    XtSetArg(arglist[i], XtNborderWidth, 0);		i++;
    XtSetArg(arglist[i], XtNvertDistance, 20);		i++;
    
    selFilePrompt = XtCreateManagedWidget("selFilePrompt",
					  labelWidgetClass, selFileForm,
					  arglist, i);

#if 1
    i = 0;
    XtSetArg(arglist[i], XtNforeground, &SFfore);		i++;
    XtSetArg(arglist[i], XtNbackground, &SFback);		i++;
    XtGetValues(selFilePrompt, arglist, i);
#endif

    SFinitFont();

    SFentryWidth = SFbesideText + SFcharsPerEntry * SFcharWidth + SFbesideText;
    SFentryHeight = SFaboveAndBelowText + SFcharHeight + SFaboveAndBelowText;

    listWidth = SFlineToTextH + SFentryWidth + SFlineToTextH + 1 +
	scrollThickness;
    listHeight = SFlineToTextV + SFentryHeight + SFlineToTextV + 1 +
	SFlineToTextV + SFlistSize * SFentryHeight +
	SFlineToTextV + 1 + scrollThickness;

    SFpathScrollWidth = 3 * listWidth + 2 * listSpacing + 4;

    hScrollX = -1;
    hScrollY = SFlineToTextV + SFentryHeight + SFlineToTextV + 1 +
	SFlineToTextV + SFlistSize * SFentryHeight + SFlineToTextV;
    SFhScrollWidth = SFlineToTextH + SFentryWidth + SFlineToTextH;

    vScrollX = SFlineToTextH + SFentryWidth + SFlineToTextH;
    vScrollY = SFlineToTextV + SFentryHeight + SFlineToTextV;
    SFvScrollHeight = SFlineToTextV + SFlistSize * SFentryHeight +
	SFlineToTextV;

    SFupperX = SFlineToTextH + SFentryWidth + SFlineToTextH - 1;
    SFlowerY = SFlineToTextV + SFentryHeight + SFlineToTextV + 1 +
	SFlineToTextV;
    SFupperY = SFlineToTextV + SFentryHeight + SFlineToTextV + 1 +
	SFlineToTextV + SFlistSize * SFentryHeight - 1;

    SFtextX = SFlineToTextH + SFbesideText;
    SFtextYoffset = SFlowerY + SFaboveAndBelowText + SFcharAscent;

    SFsegs[0].x1 = 0;
    SFsegs[0].y1 = vScrollY;
    SFsegs[0].x2 = vScrollX - 1;
    SFsegs[0].y2 = vScrollY;
    SFsegs[1].x1 = vScrollX;
    SFsegs[1].y1 = 0;
    SFsegs[1].x2 = vScrollX;
    SFsegs[1].y2 = vScrollY - 1;

    SFcompletionSegs[0].x1 = SFcompletionSegs[0].x2 = SFlineToTextH;
    SFcompletionSegs[1].x1 = SFcompletionSegs[1].x2 =
	SFlineToTextH + SFentryWidth - 1;

    i = 0;
    XtSetArg(arglist[i], XtNwidth,
	     3 * listWidth + 2 * listSpacing + 4);		i++;
    XtSetArg(arglist[i], XtNfromVert, selFilePrompt);		i++;
    XtSetArg(arglist[i], XtNresizable, True);			i++;
    XtSetArg(arglist[i], XtNtop, XtChainTop);			i++;
    XtSetArg(arglist[i], XtNbottom, XtChainTop);		i++;
    XtSetArg(arglist[i], XtNleft, XtChainLeft);			i++;
    XtSetArg(arglist[i], XtNright, XtChainLeft);		i++;
    XtSetArg(arglist[i], XtNstring, SFtextBuffer);		i++;
    XtSetArg(arglist[i], XtNlength, MAXPATHLEN);		i++;
    XtSetArg(arglist[i], XtNeditType, XawtextEdit);		i++;
    XtSetArg(arglist[i], XtNwrap, XawtextWrapWord);		i++;
    XtSetArg(arglist[i], XtNresize, XawtextResizeHeight);	i++;
    XtSetArg(arglist[i], XtNuseStringInPlace, True);		i++;
    XtSetArg(arglist[i], XtNvertDistance, 5);			i++;
    
    selFileField = XtCreateManagedWidget("selFileField",
					 asciiTextWidgetClass, selFileForm,
					 arglist, i);

    XtOverrideTranslations(selFileField,
			   XtParseTranslationTable
			   (oneLineTextEditTranslations));
    /*	XtSetKeyboardFocus(selFileForm, selFileField);
	need focus for selFileMask widget to set the filter */

    i = 0;
    XtSetArg(arglist[i], XtNorientation, XtorientHorizontal);	i++;
    XtSetArg(arglist[i], XtNwidth, SFpathScrollWidth);		i++;
    XtSetArg(arglist[i], XtNheight, scrollThickness);		i++;
    XtSetArg(arglist[i], XtNfromVert, selFileField);		i++;
    XtSetArg(arglist[i], XtNvertDistance, 30);			i++;
    XtSetArg(arglist[i], XtNtop, XtChainTop);			i++;
    XtSetArg(arglist[i], XtNbottom, XtChainTop);		i++;
    XtSetArg(arglist[i], XtNleft, XtChainLeft);			i++;
    XtSetArg(arglist[i], XtNright, XtChainLeft);		i++;
    
    selFileHScroll = XtCreateManagedWidget("selFileHScroll",
					   scrollbarWidgetClass, selFileForm,
					   arglist, i);

    XtAddCallback(selFileHScroll, XtNjumpProc,
		  (XtCallbackProc)SFpathSliderMovedCallback, (XtPointer) NULL);
    XtAddCallback(selFileHScroll, XtNscrollProc,
		  (XtCallbackProc)SFpathAreaSelectedCallback, (XtPointer) NULL);

    i = 0;
    XtSetArg(arglist[i], XtNwidth, listWidth);		i++;
    XtSetArg(arglist[i], XtNheight, listHeight);	i++;
    XtSetArg(arglist[i], XtNfromVert, selFileHScroll);	i++;
    XtSetArg(arglist[i], XtNvertDistance, 10);		i++;
    XtSetArg(arglist[i], XtNtop, XtChainTop);		i++;
    XtSetArg(arglist[i], XtNbottom, XtChainTop);	i++;
    XtSetArg(arglist[i], XtNleft, XtChainLeft);		i++;
    XtSetArg(arglist[i], XtNright, XtChainLeft);	i++;
    
    selFileLists[0] = XtCreateManagedWidget("selFileList1",
					    compositeWidgetClass, selFileForm,
					    arglist, i);

    i = 0;
    XtSetArg(arglist[i], XtNwidth, listWidth);			i++;
    XtSetArg(arglist[i], XtNheight, listHeight);		i++;
    XtSetArg(arglist[i], XtNfromHoriz, selFileLists[0]);	i++;
    XtSetArg(arglist[i], XtNfromVert, selFileHScroll);		i++;
    XtSetArg(arglist[i], XtNhorizDistance, listSpacing);	i++;
    XtSetArg(arglist[i], XtNvertDistance, 10);			i++;
    XtSetArg(arglist[i], XtNtop, XtChainTop);			i++;
    XtSetArg(arglist[i], XtNbottom, XtChainTop);		i++;
    XtSetArg(arglist[i], XtNleft, XtChainLeft);			i++;
    XtSetArg(arglist[i], XtNright, XtChainLeft);		i++;
    
    selFileLists[1] = XtCreateManagedWidget("selFileList2",
					    compositeWidgetClass, selFileForm,
					    arglist, i);

    i = 0;
    XtSetArg(arglist[i], XtNwidth, listWidth);			i++;
    XtSetArg(arglist[i], XtNheight, listHeight);		i++;
    XtSetArg(arglist[i], XtNfromHoriz, selFileLists[1]);	i++;
    XtSetArg(arglist[i], XtNfromVert, selFileHScroll);		i++;
    XtSetArg(arglist[i], XtNhorizDistance, listSpacing);	i++;
    XtSetArg(arglist[i], XtNvertDistance, 10);			i++;
    XtSetArg(arglist[i], XtNtop, XtChainTop);			i++;
    XtSetArg(arglist[i], XtNbottom, XtChainTop);		i++;
    XtSetArg(arglist[i], XtNleft, XtChainLeft);			i++;
    XtSetArg(arglist[i], XtNright, XtChainLeft);		i++;
    
    selFileLists[2] = XtCreateManagedWidget("selFileList3",
					    compositeWidgetClass, selFileForm,
					    arglist, i);

    for (n = 0; n < 3; n++) {

	i = 0;
	XtSetArg(arglist[i], XtNx, vScrollX);			i++;
	XtSetArg(arglist[i], XtNy, vScrollY);			i++;
	XtSetArg(arglist[i], XtNwidth, scrollThickness);	i++;
	XtSetArg(arglist[i], XtNheight, SFvScrollHeight);	i++;

	selFileVScrolls[n] = XtCreateManagedWidget("selFileVScroll",
						   scrollbarWidgetClass,
						   selFileLists[n], arglist, i);

	XtAddCallback(selFileVScrolls[n], XtNjumpProc,
		      (XtCallbackProc)SFvFloatSliderMovedCallback, cast_int_to_XtPointer(n));
	XtAddCallback(selFileVScrolls[n], XtNscrollProc,
		      (XtCallbackProc)SFvAreaSelectedCallback, cast_int_to_XtPointer(n));

	i = 0;
	XtSetArg(arglist[i], XtNorientation, XtorientHorizontal);	i++;
	XtSetArg(arglist[i], XtNx, hScrollX);				i++;
	XtSetArg(arglist[i], XtNy, hScrollY);				i++;
	XtSetArg(arglist[i], XtNwidth, SFhScrollWidth);			i++;
	XtSetArg(arglist[i], XtNheight, scrollThickness);		i++;

	selFileHScrolls[n] = XtCreateManagedWidget("selFileHScroll",
						   scrollbarWidgetClass,
						   selFileLists[n], arglist, i);

	XtAddCallback(selFileHScrolls[n], XtNjumpProc,
		      (XtCallbackProc)SFhSliderMovedCallback, cast_int_to_XtPointer(n));
	XtAddCallback(selFileHScrolls[n], XtNscrollProc,
		      (XtCallbackProc)SFhAreaSelectedCallback, cast_int_to_XtPointer(n));
    }

    /* When opening a DVI file, offer to open in new window */
    if (callback->must_exist) {
	open_menu = XtVaCreateManagedWidget(Xdvi_NEW_WINDOW_RADIO_NAME,
#ifdef XAW
					    radioWidgetClass,
#else
					    toggleWidgetClass,
#endif
					    selFileForm,
#ifdef XAW
					    XtNisRadio, False,
#endif
					    XtNfromVert, selFileLists[0],
					    XtNvertDistance, 30,
					    XtNhighlightThickness, 1,
					    XtNborderWidth, 0,
					    XtNlabel, "Open file in new window",
					    XtNstate, resource.filesel_open_new_window,
					    NULL);
	XtAddCallback(open_menu, XtNcallback, cb_open_new_window, (XtPointer)NULL);
    }

    /* Do the file filter stuff.  */
    selFileLabel = XtVaCreateManagedWidget("selFileLabel",
					   labelWidgetClass, selFileForm,
					   XtNfromVert, callback->must_exist ? open_menu : selFileLists[0],
					   XtNvertDistance, callback->must_exist ? 10 : 30,
					   /* 					   XtNfromHoriz, selFileCancel, */
					   /* 					   XtNhorizDistance, 60, */
					   XtNlabel, "File Mask:",
					   XtNborderWidth, 0,
					   XtNtop, XtChainTop,
					   XtNbottom, XtChainTop, NULL);

    selFileMask = XtVaCreateManagedWidget("selFileMask",
					  asciiTextWidgetClass, selFileForm,
					  XtNwidth, MASKWIDTH / 2 * SFcharWidth,
					  XtNfromVert, callback->must_exist ? open_menu : selFileLists[0],
					  XtNvertDistance, callback->must_exist ? 10 : 30,
					  XtNfromHoriz, selFileLabel,
					  XtNhorizDistance, 0,
					  XtNtop, XtChainTop,
					  XtNbottom, XtChainTop,
					  XtNstring, fileMask,
					  XtNlength, MASKWIDTH,
					  XtNeditType, XawtextEdit,
					  XtNwrap, XawtextWrapNever,
					  XtNuseStringInPlace, True, NULL);

    for (i = 0; i < 3; i++)
	XtSetKeyboardFocus(selFileLists[i], selFileField);

    XtOverrideTranslations(selFileMask,
			   XtParseTranslationTable
			   (oneLineTextEditTranslations));

    XtAddEventHandler(selFileMask, KeyPressMask, False,
		      (XtEventHandler)maskChanged, (XtPointer) NULL);

    selFileLabel = XtVaCreateManagedWidget("selFileLabel",
					   labelWidgetClass, selFileForm,
					   XtNfromVert, callback->must_exist ? open_menu : selFileLists[0],
					   XtNvertDistance, callback->must_exist ? 10 : 30,
					   XtNfromHoriz, selFileMask,
					   XtNhorizDistance, 40,
					   XtNlabel, "Dot files are:",
 					   XtNborderWidth, 0,
					   XtNtop, XtChainTop,
					   XtNbottom, XtChainTop, NULL);

    selFileHide = XtVaCreateManagedWidget("selFileHide",
					  commandWidgetClass, selFileForm,
					  /*  					  XtNwidth, 7 * SFcharWidth, */
					  XtNfromVert, callback->must_exist ? open_menu : selFileLists[0],
					  XtNvertDistance, callback->must_exist ? 10 : 30,
					  XtNfromHoriz, selFileLabel,
					  XtNhorizDistance, 2,
					  XtNlabel, hideFlag ? "hidden" : "shown",
					  /*  					  XtNborderWidth, 1, */
					  XtNtop, XtChainTop,
					  /*  					  XtNjustify, XtJustifyLeft, */
					  XtNbottom, XtChainTop,
					  NULL);
    XtAddCallback(selFileHide, XtNcallback, (XtCallbackProc)hideFiles, NULL);

    box = XtVaCreateManagedWidget("box", formWidgetClass,
				  paned,
				  XtNshowGrip, False,
				  XtNdefaultDistance, 6,
				  XtNskipAdjust, True,
				  XtNaccelerators, G_accels_cr,
				  NULL);
    selFileOK = XtVaCreateManagedWidget("selFileOK", commandWidgetClass,
					box,
					XtNlabel, callback->ok,
					XtNcallback, SFokSelect,
					XtNtop, XtChainTop,
					XtNbottom, XtChainBottom,
					XtNleft, XtChainLeft,
					XtNright, XtChainLeft,
					NULL);
    selFileCancel = XtVaCreateManagedWidget("selFileCancel", commandWidgetClass,
					    box,
					    XtNlabel, callback->cancel,
					    XtNcallback, SFcancelSelect,
					    /*  					    XtNborderColor, SFfore, */
					    XtNfromHoriz, selFileOK,
					    XtNbottom, XtChainBottom,
					    XtNleft, XtChainRight,
					    XtNright, XtChainRight,
					    NULL);

    XtSetMappedWhenManaged(selFile, False);
    XtRealizeWidget(selFile);

    /* Add WM_DELETE_WINDOW protocol */
    SFwmDeleteWindow = XInternAtom(SFdisplay, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(SFdisplay, XtWindow(selFile), &SFwmDeleteWindow, 1);

    SFcreateGC();

    xtermCursor = XCreateFontCursor(SFdisplay, XC_xterm);

    sbRightArrowCursor = XCreateFontCursor(SFdisplay, XC_sb_right_arrow);
    arrowCursor = XCreateFontCursor(SFdisplay, XC_left_ptr);

    XDefineCursor(SFdisplay, XtWindow(selFileForm), arrowCursor);
    XDefineCursor(SFdisplay, XtWindow(selFileField), xtermCursor);

    for (n = 0; n < 3; n++) {
	XDefineCursor(SFdisplay, XtWindow(selFileLists[n]), sbRightArrowCursor);
    }
    XDefineCursor(SFdisplay, XtWindow(selFileOK), arrowCursor);
    XDefineCursor(SFdisplay, XtWindow(selFileCancel), arrowCursor);

    for (n = 0; n < 3; n++) {
	XtAddEventHandler(selFileLists[n], ExposureMask, True,
			  (XtEventHandler)SFexposeList, cast_int_to_XtPointer(n));
	XtAddEventHandler(selFileLists[n], EnterWindowMask, False,
			  (XtEventHandler)SFenterList, cast_int_to_XtPointer(n));
	XtAddEventHandler(selFileLists[n], LeaveWindowMask, False,
			  (XtEventHandler)SFleaveList, cast_int_to_XtPointer(n));
	XtAddEventHandler(selFileLists[n], PointerMotionMask, False,
			  (XtEventHandler)SFmotionList, cast_int_to_XtPointer(n));
	XtAddEventHandler(selFileLists[n], ButtonPressMask, False,
			  (XtEventHandler)SFbuttonPressList, cast_int_to_XtPointer(n));
	XtAddEventHandler(selFileLists[n], ButtonReleaseMask, False,
			  (XtEventHandler)SFbuttonReleaseList, cast_int_to_XtPointer(n));
    }

    XtAddEventHandler(selFileField, KeyPressMask, False,
		      (XtEventHandler)SFmodVerifyCallback, (XtPointer) NULL);

    SFapp = XtWidgetToApplicationContext(selFile);
    return selFile;
}

/* position widget under the cursor */
void
SFpositionWidget(Widget w)
{
    Arg args[3];
    Cardinal num_args;
    Dimension width, height, b_width;
    int x, y, max_x, max_y;
    Window root, child;
    int dummyx, dummyy;
    unsigned int dummymask;

    XQueryPointer(XtDisplay(w), XtWindow(w), &root, &child, &x, &y,
		  &dummyx, &dummyy, &dummymask);
    num_args = 0;
    XtSetArg(args[num_args], XtNwidth, &width);
    num_args++;
    XtSetArg(args[num_args], XtNheight, &height);
    num_args++;
    XtSetArg(args[num_args], XtNborderWidth, &b_width);
    num_args++;
    XtGetValues(w, args, num_args);

    width += 2 * b_width;
    height += 2 * b_width;

    x -= ((Position) width / 2);
    if (x < 0)
	x = 0;
    if (x > (max_x = (Position) (XtScreen(w)->width - width)))
	x = max_x;

    y -= ((Position) height / 2);
    if (y < 0)
	y = 0;
    if (y > (max_y = (Position) (XtScreen(w)->height - height)))
	y = max_y;

    num_args = 0;
    XtSetArg(args[num_args], XtNx, x);
    num_args++;
    XtSetArg(args[num_args], XtNy, y);
    num_args++;
    XtSetValues(w, args, num_args);
}


FILE *
SFopenFile(const char *name, const char *mode, const char *prompt, const char *failed)
{
    Arg args[1];
    FILE *fp;

    UNUSED(args);
    UNUSED(prompt);
    UNUSED(failed);
    
    SFchdir(SFstartDir);
    errno = 0;
    if (!name || *name == 0 || (fp = XFOPEN(name, mode)) == NULL) {
	sfBell(DISP,
	       (selFile != NULL && XtIsManaged(selFile))
			? XtWindow(selFile) : (Window) NULL, 0);
	return NULL;
    }
    return fp;
}

void
SFtextChanged(void)
{
    if ((SFtextBuffer[0] == '/') || (SFtextBuffer[0] == '~')) {
	(void)strcpy(SFcurrentPath, SFtextBuffer);

	SFtextPos = XawTextGetInsertionPoint(selFileField);
    }
    else {
	(void)strcat(strcpy(SFcurrentPath, SFstartDir), SFtextBuffer);

	SFtextPos = XawTextGetInsertionPoint(selFileField) + strlen(SFstartDir);
    }

    if (!SFworkProcAdded) {
	(void)XtAppAddWorkProc(SFapp, (XtWorkProc)SFworkProc, NULL);
	SFworkProcAdded = 1;
    }

    SFupdatePath();
}

static void
SFprepareToReturn(void)
{
    SFstatus = SEL_FILE_NULL;
    /* XtRemoveGrab(selFile); */
    XtUnmapWidget(selFile);

    if (SFdirModTimerId) {
	XtRemoveTimeOut(SFdirModTimerId);
	SFdirModTimerId = 0;
    }
    if (SFchdir(SFstartDir)) {
	XtAppError(SFapp, "XsraSelFile: can't return to current directory");
    }
}

Widget
XsraSelFile(Widget parent, struct filesel_callback *callback)
{
    SFdisplay = XtDisplay(parent);
    return SFcreateWidgets(parent, callback);
}

void
XsraSelFilePopup(struct filesel_callback *callback)
{
    Cardinal i;
    Arg arglist[20];
    XEvent event;
    
    if (XtIsManaged(callback->shell)) {
	sfBell(DISP, XtWindow(callback->shell), 10);
	XRaiseWindow(DISP, XtWindow(callback->shell));
	return;
    }

    if (!callback->prompt)
	callback->prompt = "Pathname:";

    if (!callback->title)
	callback->title = "xdvik: select filename";

    if (!callback->ok)
	callback->ok = "OK";

    if (!callback->cancel)
	callback->cancel = "Cancel";

    /*     if (!callback->browse_fname) */
    /* 	callback->browse_fname = xt_strdup(xgetcwd()); */
    
    i = 0;
    XtSetArg(arglist[i], XtNlabel, callback->prompt); i++;
    XtSetValues(selFilePrompt, arglist, i);

    i = 0;
    XtSetArg(arglist[i], XtNlabel, callback->ok); i++;
    XtSetValues(selFileOK, arglist, i);

    i = 0;
    XtSetArg(arglist[i], XtNtitle, callback->title); i++;
    XtSetValues(selFile, arglist, i);

    i = 0;
    XtSetArg(arglist[i], XtNlabel, callback->cancel); i++;
    XtSetValues(selFileCancel, arglist, i);
    

    SFpositionWidget(selFile);
    XtMapWidget(selFile);

    {
	char *cwd = xgetcwd();
	strcpy(SFstartDir, cwd);
	free(cwd);
    }
    if (SFstartDir[0] == 0) {
	XtAppError(SFapp, "XsraSelFile: can't get current directory");
    }
    (void)strcat(SFstartDir, "/");
    (void)strcpy(SFcurrentDir, SFstartDir);

    if (callback->init_path) {
	if (callback->init_path[0] == '/') {
	    (void)strcpy(SFcurrentPath, callback->init_path);
	    if (strncmp(SFcurrentPath, SFstartDir, strlen(SFstartDir))) {
		SFsetText(SFcurrentPath);
	    }
	    else {
		SFsetText(&(SFcurrentPath[strlen(SFstartDir)]));
	    }
	}
	else {
	    (void)strcat(strcpy(SFcurrentPath, SFstartDir), callback->init_path);
	    SFsetText(&(SFcurrentPath[strlen(SFstartDir)]));
	}
    }
    else {
	SFsetText(SFcurrentDir);
	(void)strcpy(SFcurrentPath, SFstartDir);
    }

    SFfunc = showEntry;

    SFtextChanged();

    /* don't grab the pointer so that warning popups still work */
    /* XtAddGrab(selFile, True, True); */

    SFdirModTimerId = XtAppAddTimeOut(SFapp, 1200UL,
				      SFdirModTimer, (XtPointer) NULL);
    
    if (strcmp(fileMask, callback->filemask) != 0) { /* if mask changed */
	SFDir *dir;
	strncpy(fileMask, callback->filemask, MASKWIDTH);
	XtVaSetValues(selFileMask, XtNstring, fileMask, NULL);
	for (dir = &(SFdirs[SFdirEnd - 1]); dir >= SFdirs; dir--)
	    *(dir->dir) = 0;	/* force a re-read */
	SFupdatePath();
    }

    while (1) {
	XtAppNextEvent(SFapp, &event);
	switch (event.type) {
	    Widget w;
#if 0  /* DON'T do this, it may send the X server into a busy loop if the File selector
	  is positioned over a window that is `on top' by default */
	case Expose:
	    if (!raise_message_windows())
		raise_file_selector();
	    break;
#endif
	case KeyPress:
	case ButtonPress:
	    /* ignore keypress inside the main window (and beep to warn) */
	    w = XtWindowToWidget(DISP, event.xany.window);
	    while ((w != NULL) && (w != selFile)) {
		/* exception: message windows */
		if (is_message_window(w))
		    break;
		w = XtParent(w);
	    }
	    if (w == NULL || w == globals.widgets.top_level) {
		sfBell(DISP, event.xany.window, 0);
		continue;
	    }
	    break;
	}

	XtDispatchEvent(&event);

	switch (SFstatus) {
	case SEL_FILE_TEXT:
	    SFstatus = SEL_FILE_NULL;
	    SFtextChanged();
	    break;
	case SEL_FILE_OK:
	    if (callback->must_exist) {
		FILE *tmp_fp = XFOPEN(SFtextBuffer, "r");
		dviErrFlagT errflag = NO_ERROR;
		if (tmp_fp == NULL) {
		    popup_message(selFile,
				  MSG_ERR, NULL, "Could not open %s: %s.\n",
				  SFtextBuffer, strerror(errno));
		    SFstatus = SEL_FILE_NULL;
		    break;
		}
		else if (!process_preamble(tmp_fp, &errflag)
			 || !find_postamble(tmp_fp, &errflag)
			 || !read_postamble(tmp_fp, &errflag, False)) {
		    popup_message(selFile,
				  MSG_ERR, NULL, "Error opening %s:\n%s.",
				  SFtextBuffer, get_dvi_error(errflag));
		    fclose(tmp_fp);
		    SFstatus = SEL_FILE_NULL;
		    break;
		}
		else { /* file is OK */
		    fclose(tmp_fp);
		    SFprepareToReturn();
		    callback->func_ptr(SFtextBuffer, callback->data);
		    return;
		    /*  		    return xstrdup(SFtextBuffer); */
		}
	    }
	    else {
		SFprepareToReturn();
		callback->func_ptr(SFtextBuffer, callback->data);
		return;
		/*  		return xstrdup(SFtextBuffer); */
	    }
	case SEL_FILE_CANCEL:
	    SFprepareToReturn();
	    if (callback->exit_on_cancel)
		exit(0);
	    return;
	    /*  	    return NULL; */
	case SEL_FILE_NULL:
	    break;
	}
    }
}

#else
/* silence `empty compilation unit' warnings */
static void bar(void); static void foo(void) { bar(); } static void bar(void) { foo(); }
#endif /* !defined(MOTIF) */
