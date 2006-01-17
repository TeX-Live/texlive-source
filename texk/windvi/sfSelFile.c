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

#ifdef HAVE_CONFIG_H
#include "c-auto.h"
#endif

#if !defined (NOSELFILE) && !defined (NOTOOL) /* for xdvik */

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

#include "xdvi-config.h"
#include <kpathsea/c-stat.h>

#include <sys/param.h>
#include <X11/cursorfont.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Composite.h>
#include <X11/Shell.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Scrollbar.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Cardinals.h>

#include "sfinternal.h"

#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif /* ndef MAXPATHLEN */

extern int SFchdir ();

int SFstatus = SEL_FILE_NULL;

char
	SFstartDir[MAXPATHLEN],
	SFcurrentPath[MAXPATHLEN],
	SFcurrentDir[MAXPATHLEN];

Widget
	selFile,
	selFileCancel,
	selFileField,
	selFileForm,
	selFileHScroll,
	selFileHScrolls[3],
	selFileLists[3],
	selFileOK,
	selFilePrompt,
	selFileVScrolls[3];

/* For file filter. */
Widget	selFileLabel, selFileMask, selFileHide;
#define MASKWIDTH 10
char 	fileMask[MASKWIDTH+2] = "*.dvi";

Display *SFdisplay;

Pixel SFfore, SFback;

Atom	SFwmDeleteWindow;

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

int (*SFfunc)();

static char *oneLineTextEditTranslations = "\
	<Key>Return:	redraw-display()\n\
	Ctrl<Key>M:	redraw-display()\n\
";
#if !defined (HAVE_STRERROR) && !defined (strerror)
static char *
strerror (errnum)
     int errnum;
{
  extern char *sys_errlist[];
  extern int sys_nerr;

  return 0 < errnum && errnum <= sys_nerr
         ? sys_errlist[errnum] : "Unknown system error";
}
#endif /* not HAVE_STRERROR && not strerror */

/* ARGSUSED */
static void
SFexposeList(w, n, event, cont)
	Widget		w;
	XtPointer	n;
        XEvent   	*event;
        Boolean         *cont;
{
	extern void SFdrawList ();
	
	if ((event->type == NoExpose) || event->xexpose.count) {
		return;
	}

	SFdrawList(n, SF_DO_NOT_SCROLL);
}

/* ARGSUSED */
static void
SFmodVerifyCallback(w, client_data, event, cont)
	Widget			w;
	XtPointer		client_data;
        XEvent	                *event;
        Boolean                 *cont;
{
	char	buf[2];

	if (
		(XLookupString(&(event->xkey), buf, 2, NULL, NULL) == 1) &&
		((*buf) == '\r')
	) {
		SFstatus = SEL_FILE_OK;
	} else {
		SFstatus = SEL_FILE_TEXT;
	}
}

/* ARGSUSED */
static void
SFokCallback(w, cl, cd)
	Widget	w;
        XtPointer cl, cd;
{
	SFstatus = SEL_FILE_OK;
}

static XtCallbackRec SFokSelect[] = {
	{ SFokCallback, (XtPointer) NULL },
	{ NULL, (XtPointer) NULL },
};

/* ARGSUSED */
static void
SFcancelCallback(w, cl, cd)
	Widget	w;
        XtPointer cl, cd;
{
	SFstatus = SEL_FILE_CANCEL;
}

static XtCallbackRec SFcancelSelect[] = {
	{ SFcancelCallback, (XtPointer) NULL },
	{ NULL, (XtPointer) NULL },
};

/* ARGSUSED */
static void
SFdismissAction(w, event, params, num_params)
	Widget	w;
	XEvent *event;
	String *params;
	Cardinal *num_params;
{
	if (event->type == ClientMessage &&
	    event->xclient.data.l[0] != SFwmDeleteWindow) return;

	SFstatus = SEL_FILE_CANCEL;
}

static char *wmDeleteWindowTranslation = "\
	<Message>WM_PROTOCOLS:	SelFileDismiss()\n\
";

static XtActionsRec actions[] = {
	{"SelFileDismiss",	SFdismissAction},
};

/* Don't show files that don't get through the filter.  */

int
maskFile(mask, filename)
   char	*mask, *filename;
/* return 1 if file is masked (mask does not match filename), 0 otherwise */
{
int 	c, c1;

	while  ((c = *mask++)) {
	  if (c == '*') {
	    while ((c1 = *mask++)) {
	      if (c1 != '*') {
		if (!(*filename)) return 1;
		if (c1 != '?') {
		  while ((filename = strchr(filename, c1))) {
		    if (!maskFile(mask, ++filename)) return 0;
		  }
		  return 1;
		}
		else filename++;
	      }
	    }
	    return 0;
	  }
	  if (c == '?') { if (!*filename) return 1; }
	  else if (c != *filename) return 1;
	  filename++;
	}
	return (*filename)? 1 : 0;
}

int	hideFlag = 1;
int
showEntry(entryReal, entryShown, statBuf)
    char	*entryReal, **entryShown;
    struct	stat	*statBuf;
{
	if (!(S_ISDIR(statBuf->st_mode))) {
	  if (hideFlag)
	    if (entryReal[0] == '.') return 0;
	  if (maskFile(fileMask, entryReal)) return 0;
	}
	entryReal[strlen(entryReal)] = SFstatChar(statBuf);
        return 1;
}

/* ARGSUSED */
void
maskChanged(w, client_data, event)
    Widget		w;
    XtPointer	client_data;
    XKeyPressedEvent	*event;
{
    char	buf[2];
    register SFDir		*dir;
    extern void SFupdatePath ();

	if ((XLookupString(event, buf, 2, NULL, NULL) == 1) &&
	    ((*buf) == '\r')) {
	  for (dir = &(SFdirs[SFdirEnd - 1]); dir >= SFdirs; dir--)
	    *(dir->dir) = 0; /* force a re-read */
	  SFupdatePath();
	}
}

/* ARGSUSED */
void
hideFiles(w, client_data, event)
    Widget	w;
    XtPointer	client_data;
    XEvent	*event;
{
    register SFDir		*dir;
    register SFEntry	*entry;
    extern void SFupdatePath (), SFdrawLists ();

	hideFlag = 1 - hideFlag;
	if (hideFlag) {
	  XtVaSetValues(w, XtNlabel, "hidden", NULL);
	  for (dir = &(SFdirs[SFdirEnd - 1]); dir >= SFdirs; dir--) {
	    if (!(dir->nEntries)) continue;
	    dir->vOrigin = 0;
	    for (entry = &(dir->entries[dir->nEntries - 1]);
		 entry >= dir->entries; entry--)
	      entry->statDone = 0;
	    SFdrawLists(SF_DO_SCROLL);
	  }
	} else {
	  XtVaSetValues(w, XtNlabel, "shown", NULL);
	  for (dir = &(SFdirs[SFdirEnd - 1]); dir >= SFdirs; dir--)
	    *(dir->dir) = 0;	/* force a re-read */
	  SFupdatePath();
	}
}


static void
SFcreateWidgets(toplevel, prompt, ok, cancel)
	Widget	toplevel;
	char	*prompt;
	char	*ok;
	char	*cancel;
{
	Cardinal	i, n;
	int		listWidth, listHeight;
	int		listSpacing = 10;
	int		scrollThickness = 15;
	int		hScrollX, hScrollY;
	int		vScrollX, vScrollY;
	Cursor
			xtermCursor,
			sbRightArrowCursor,
			dotCursor;
	Arg		arglist[20];
	extern void SFinitFont (), SFcreateGC ();

	i = 0;
	XtSetArg(arglist[i], XtNtransientFor, toplevel);		i++;

	selFile = XtAppCreateShell("selFile", "SelFile",
		transientShellWidgetClass, SFdisplay, arglist, i);

	/* Add WM_DELETE_WINDOW protocol */
	XtAppAddActions(XtWidgetToApplicationContext(selFile),
		actions, XtNumber(actions));
	XtOverrideTranslations(selFile,
		XtParseTranslationTable(wmDeleteWindowTranslation));

	i = 0;
	XtSetArg(arglist[i], XtNdefaultDistance, 30);			i++;
	selFileForm = XtCreateManagedWidget("selFileForm",
		formWidgetClass, selFile, arglist, i);

	i = 0;
	XtSetArg(arglist[i], XtNlabel, prompt);				i++;
	XtSetArg(arglist[i], XtNresizable, True);			i++;
	XtSetArg(arglist[i], XtNtop, XtChainTop);			i++;
	XtSetArg(arglist[i], XtNbottom, XtChainTop);			i++;
	XtSetArg(arglist[i], XtNleft, XtChainLeft);			i++;
	XtSetArg(arglist[i], XtNright, XtChainLeft);			i++;
	XtSetArg(arglist[i], XtNborderWidth, 0);			i++;
	selFilePrompt = XtCreateManagedWidget("selFilePrompt",
		labelWidgetClass, selFileForm, arglist, i);

	i = 0;
	XtSetArg(arglist[i], XtNforeground, &SFfore);			i++;
	XtSetArg(arglist[i], XtNbackground, &SFback);			i++;
	XtGetValues(selFilePrompt, arglist, i);

	SFinitFont();

	SFentryWidth = SFbesideText + SFcharsPerEntry * SFcharWidth +
			SFbesideText;
	SFentryHeight = SFaboveAndBelowText + SFcharHeight +
			SFaboveAndBelowText;

	listWidth = SFlineToTextH + SFentryWidth + SFlineToTextH + 1 +
			scrollThickness;
	listHeight = SFlineToTextV + SFentryHeight + SFlineToTextV + 1 +
			SFlineToTextV + SFlistSize * SFentryHeight +
			SFlineToTextV + 1 + scrollThickness;

	SFpathScrollWidth = 3 * listWidth + 2 * listSpacing + 4;

	hScrollX = -1;
	hScrollY = SFlineToTextV + SFentryHeight + SFlineToTextV + 1 +
			SFlineToTextV + SFlistSize * SFentryHeight +
			SFlineToTextV;
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
	XtSetArg(arglist[i], XtNwidth, 3 * listWidth + 2 * listSpacing + 4);
									i++;
	XtSetArg(arglist[i], XtNborderColor, SFfore);			i++;

	XtSetArg(arglist[i], XtNfromVert, selFilePrompt);		i++;
	XtSetArg(arglist[i], XtNvertDistance, 10);			i++;
	XtSetArg(arglist[i], XtNresizable, True);			i++;
	XtSetArg(arglist[i], XtNtop, XtChainTop);			i++;
	XtSetArg(arglist[i], XtNbottom, XtChainTop);			i++;
	XtSetArg(arglist[i], XtNleft, XtChainLeft);			i++;
	XtSetArg(arglist[i], XtNright, XtChainLeft);			i++;
	XtSetArg(arglist[i], XtNstring, SFtextBuffer);			i++;
	XtSetArg(arglist[i], XtNlength, MAXPATHLEN);			i++;
	XtSetArg(arglist[i], XtNeditType, XawtextEdit);			i++;
	XtSetArg(arglist[i], XtNwrap, XawtextWrapWord);			i++;
	XtSetArg(arglist[i], XtNresize, XawtextResizeHeight);		i++;
	XtSetArg(arglist[i], XtNuseStringInPlace, True);		i++;
	selFileField = XtCreateManagedWidget("selFileField",
		asciiTextWidgetClass, selFileForm, arglist, i);

	XtOverrideTranslations(selFileField,
		XtParseTranslationTable(oneLineTextEditTranslations));
/*	XtSetKeyboardFocus(selFileForm, selFileField);
	need focus for selFileMask wigget to set the filter */

	i = 0;
	XtSetArg(arglist[i], XtNorientation, XtorientHorizontal);	i++;
	XtSetArg(arglist[i], XtNwidth, SFpathScrollWidth);		i++;
	XtSetArg(arglist[i], XtNheight, scrollThickness);		i++;
	XtSetArg(arglist[i], XtNborderColor, SFfore);			i++;
	XtSetArg(arglist[i], XtNfromVert, selFileField);		i++;
	XtSetArg(arglist[i], XtNvertDistance, 30);			i++;
	XtSetArg(arglist[i], XtNtop, XtChainTop);			i++;
	XtSetArg(arglist[i], XtNbottom, XtChainTop);			i++;
	XtSetArg(arglist[i], XtNleft, XtChainLeft);			i++;
	XtSetArg(arglist[i], XtNright, XtChainLeft);			i++;
	selFileHScroll = XtCreateManagedWidget("selFileHScroll",
		scrollbarWidgetClass, selFileForm, arglist, i);

	XtAddCallback(selFileHScroll, XtNjumpProc,
		SFpathSliderMovedCallback, (XtPointer) NULL);
	XtAddCallback(selFileHScroll, XtNscrollProc,
		SFpathAreaSelectedCallback, (XtPointer) NULL);

	i = 0;
	XtSetArg(arglist[i], XtNwidth, listWidth);			i++;
	XtSetArg(arglist[i], XtNheight, listHeight);			i++;
	XtSetArg(arglist[i], XtNborderColor, SFfore);			i++;
	XtSetArg(arglist[i], XtNfromVert, selFileHScroll);		i++;
	XtSetArg(arglist[i], XtNvertDistance, 10);			i++;
	XtSetArg(arglist[i], XtNtop, XtChainTop);			i++;
	XtSetArg(arglist[i], XtNbottom, XtChainTop);			i++;
	XtSetArg(arglist[i], XtNleft, XtChainLeft);			i++;
	XtSetArg(arglist[i], XtNright, XtChainLeft);			i++;
	selFileLists[0] = XtCreateManagedWidget("selFileList1",
		compositeWidgetClass, selFileForm, arglist, i);

	i = 0;
	XtSetArg(arglist[i], XtNwidth, listWidth);			i++;
	XtSetArg(arglist[i], XtNheight, listHeight);			i++;
	XtSetArg(arglist[i], XtNborderColor, SFfore);			i++;
	XtSetArg(arglist[i], XtNfromHoriz, selFileLists[0]);		i++;
	XtSetArg(arglist[i], XtNfromVert, selFileHScroll);		i++;
	XtSetArg(arglist[i], XtNhorizDistance, listSpacing);		i++;
	XtSetArg(arglist[i], XtNvertDistance, 10);			i++;
	XtSetArg(arglist[i], XtNtop, XtChainTop);			i++;
	XtSetArg(arglist[i], XtNbottom, XtChainTop);			i++;
	XtSetArg(arglist[i], XtNleft, XtChainLeft);			i++;
	XtSetArg(arglist[i], XtNright, XtChainLeft);			i++;
	selFileLists[1] = XtCreateManagedWidget("selFileList2",
		compositeWidgetClass, selFileForm, arglist, i);

	i = 0;
	XtSetArg(arglist[i], XtNwidth, listWidth);			i++;
	XtSetArg(arglist[i], XtNheight, listHeight);			i++;
	XtSetArg(arglist[i], XtNborderColor, SFfore);			i++;
	XtSetArg(arglist[i], XtNfromHoriz, selFileLists[1]);		i++;
	XtSetArg(arglist[i], XtNfromVert, selFileHScroll);		i++;
	XtSetArg(arglist[i], XtNhorizDistance, listSpacing);		i++;
	XtSetArg(arglist[i], XtNvertDistance, 10);			i++;
	XtSetArg(arglist[i], XtNtop, XtChainTop);			i++;
	XtSetArg(arglist[i], XtNbottom, XtChainTop);			i++;
	XtSetArg(arglist[i], XtNleft, XtChainLeft);			i++;
	XtSetArg(arglist[i], XtNright, XtChainLeft);			i++;
	selFileLists[2] = XtCreateManagedWidget("selFileList3",
		compositeWidgetClass, selFileForm, arglist, i);

	for (n = 0; n < 3; n++) {

		i = 0;
		XtSetArg(arglist[i], XtNx, vScrollX);			i++;
		XtSetArg(arglist[i], XtNy, vScrollY);			i++;
		XtSetArg(arglist[i], XtNwidth, scrollThickness);	i++;
		XtSetArg(arglist[i], XtNheight, SFvScrollHeight);	i++;
		XtSetArg(arglist[i], XtNborderColor, SFfore);		i++;
		selFileVScrolls[n] = XtCreateManagedWidget("selFileVScroll",
			scrollbarWidgetClass, selFileLists[n], arglist, i);

		XtAddCallback(selFileVScrolls[n], XtNjumpProc,
			SFvFloatSliderMovedCallback, (XtPointer) n);
		XtAddCallback(selFileVScrolls[n], XtNscrollProc,
			SFvAreaSelectedCallback, (XtPointer) n);

		i = 0;

		XtSetArg(arglist[i], XtNorientation, XtorientHorizontal);
									i++;
		XtSetArg(arglist[i], XtNx, hScrollX);			i++;
		XtSetArg(arglist[i], XtNy, hScrollY);			i++;
		XtSetArg(arglist[i], XtNwidth, SFhScrollWidth);		i++;
		XtSetArg(arglist[i], XtNheight, scrollThickness);	i++;
		XtSetArg(arglist[i], XtNborderColor, SFfore);		i++;
		selFileHScrolls[n] = XtCreateManagedWidget("selFileHScroll",
			scrollbarWidgetClass, selFileLists[n], arglist, i);

		XtAddCallback(selFileHScrolls[n], XtNjumpProc,
			SFhSliderMovedCallback, (XtPointer) n);
		XtAddCallback(selFileHScrolls[n], XtNscrollProc,
			SFhAreaSelectedCallback, (XtPointer) n);
	}

	i = 0;
	XtSetArg(arglist[i], XtNlabel, ok);				i++;
	XtSetArg(arglist[i], XtNresizable, True);			i++;
	XtSetArg(arglist[i], XtNcallback, SFokSelect);			i++;
	XtSetArg(arglist[i], XtNborderColor, SFfore);			i++;
	XtSetArg(arglist[i], XtNfromVert, selFileLists[0]);		i++;
	XtSetArg(arglist[i], XtNvertDistance, 30);			i++;
	XtSetArg(arglist[i], XtNtop, XtChainTop);			i++;
	XtSetArg(arglist[i], XtNbottom, XtChainTop);			i++;
	XtSetArg(arglist[i], XtNleft, XtChainLeft);			i++;
	XtSetArg(arglist[i], XtNright, XtChainLeft);			i++;
	selFileOK = XtCreateManagedWidget("selFileOK", commandWidgetClass,
		selFileForm, arglist, i);

	i = 0;
	XtSetArg(arglist[i], XtNlabel, cancel);				i++;
	XtSetArg(arglist[i], XtNresizable, True);			i++;
	XtSetArg(arglist[i], XtNcallback, SFcancelSelect);		i++;
	XtSetArg(arglist[i], XtNborderColor, SFfore);			i++;
	XtSetArg(arglist[i], XtNfromHoriz, selFileOK);			i++;
	XtSetArg(arglist[i], XtNfromVert, selFileLists[0]);		i++;
	XtSetArg(arglist[i], XtNhorizDistance, 30);			i++;
	XtSetArg(arglist[i], XtNvertDistance, 30);			i++;
	XtSetArg(arglist[i], XtNtop, XtChainTop);			i++;
	XtSetArg(arglist[i], XtNbottom, XtChainTop);			i++;
	XtSetArg(arglist[i], XtNleft, XtChainLeft);			i++;
	XtSetArg(arglist[i], XtNright, XtChainLeft);			i++;
	selFileCancel = XtCreateManagedWidget("selFileCancel",
		commandWidgetClass, selFileForm, arglist, i);

        /* Do the file filter stuff.  */
	selFileLabel = XtVaCreateManagedWidget("selFileLabel",
		labelWidgetClass, selFileForm,
		XtNfromVert, selFileLists[0],
		XtNvertDistance, 30,
		XtNfromHoriz, selFileCancel,
		XtNhorizDistance, 60,
		XtNlabel,  "File Mask:",
		XtNborderWidth, 0,
		XtNtop, XtChainTop,
		XtNbottom, XtChainTop,
		NULL);
	
	selFileMask = XtVaCreateManagedWidget("selFileMask",
		asciiTextWidgetClass, selFileForm,
		XtNwidth, MASKWIDTH*SFcharWidth,
		XtNfromVert, selFileLists[0],
		XtNvertDistance, 30,
		XtNfromHoriz, selFileLabel,
		XtNhorizDistance, 0,
		XtNtop, XtChainTop,
		XtNbottom, XtChainTop,
		XtNstring, fileMask,
		XtNlength, sizeof(fileMask),
		XtNeditType, XawtextEdit,
		XtNwrap, XawtextWrapNever,
		XtNuseStringInPlace, True,
		NULL);

	for (i = 0; i < 3; i++)
	  XtSetKeyboardFocus(selFileLists[i], selFileField);

	XtOverrideTranslations(selFileMask,
		XtParseTranslationTable(oneLineTextEditTranslations));

	XtAddEventHandler(selFileMask, KeyPressMask, False,
		maskChanged, (XtPointer) NULL);

	selFileLabel = XtVaCreateManagedWidget("selFileLabel",
		labelWidgetClass, selFileForm,
		XtNfromVert, selFileLists[0],
		XtNvertDistance, 30,
		XtNfromHoriz, selFileMask,
		XtNhorizDistance, 40,
		XtNlabel,  "dot files",
		XtNborderWidth, 0,
		XtNtop, XtChainTop,
		XtNbottom, XtChainTop,
		NULL);

	selFileHide = XtVaCreateManagedWidget("selFileHide",
		commandWidgetClass, selFileForm,
		XtNfromVert, selFileLists[0],
		XtNvertDistance, 30,
		XtNfromHoriz, selFileLabel,
		XtNhorizDistance, 2,
		XtNlabel, "hidden",
		XtNborderWidth, 0,
		XtNtop, XtChainTop,
		XtNbottom, XtChainTop,
#if 0 /* missing in R4, says 
         pete@lovelace.thi.informatik.uni-frankfurt.de (Peter Dyballa) */
		XtNcursorName, "dot",
#endif
	       	NULL);
	XtAddCallback(selFileHide, XtNcallback, hideFiles, NULL);

	XtSetMappedWhenManaged(selFile, False);
	XtRealizeWidget(selFile);

	/* Add WM_DELETE_WINDOW protocol */
	SFwmDeleteWindow = XInternAtom(SFdisplay, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(SFdisplay, XtWindow(selFile), &SFwmDeleteWindow, 1);

	SFcreateGC();

	xtermCursor = XCreateFontCursor(SFdisplay, XC_xterm);

	sbRightArrowCursor = XCreateFontCursor(SFdisplay, XC_sb_right_arrow);
	dotCursor = XCreateFontCursor(SFdisplay, XC_dot);

	XDefineCursor(SFdisplay, XtWindow(selFileForm), xtermCursor);
	XDefineCursor(SFdisplay, XtWindow(selFileField), xtermCursor);

	for (n = 0; n < 3; n++) {
		XDefineCursor(SFdisplay, XtWindow(selFileLists[n]),
			sbRightArrowCursor);
	}
	XDefineCursor(SFdisplay, XtWindow(selFileOK), dotCursor);
	XDefineCursor(SFdisplay, XtWindow(selFileCancel), dotCursor);

	for (n = 0; n < 3; n++) {
		XtAddEventHandler(selFileLists[n], ExposureMask, True,
			SFexposeList, (XtPointer) n);
		XtAddEventHandler(selFileLists[n], EnterWindowMask, False,
			SFenterList, (XtPointer) n);
		XtAddEventHandler(selFileLists[n], LeaveWindowMask, False,
			SFleaveList, (XtPointer) n);
		XtAddEventHandler(selFileLists[n], PointerMotionMask, False,
			SFmotionList, (XtPointer) n);
		XtAddEventHandler(selFileLists[n], ButtonPressMask, False,
			SFbuttonPressList, (XtPointer) n);
		XtAddEventHandler(selFileLists[n], ButtonReleaseMask, False,
			SFbuttonReleaseList, (XtPointer) n);
	}

	XtAddEventHandler(selFileField, KeyPressMask, False,
		SFmodVerifyCallback, (XtPointer) NULL);

	SFapp = XtWidgetToApplicationContext(selFile);

}

/* position widget under the cursor */
void
SFpositionWidget(w)
    Widget w;
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
    XtSetArg(args[num_args], XtNwidth, &width); num_args++;
    XtSetArg(args[num_args], XtNheight, &height); num_args++;
    XtSetArg(args[num_args], XtNborderWidth, &b_width); num_args++;
    XtGetValues(w, args, num_args);

    width += 2 * b_width;
    height += 2 * b_width;

    x -= ( (Position) width/2 );
    if (x < 0) x = 0;
    if ( x > (max_x = (Position) (XtScreen(w)->width - width)) ) x = max_x;

    y -= ( (Position) height/2 );
    if (y < 0) y = 0;
    if ( y > (max_y = (Position) (XtScreen(w)->height - height)) ) y = max_y;
    
    num_args = 0;
    XtSetArg(args[num_args], XtNx, x); num_args++;
    XtSetArg(args[num_args], XtNy, y); num_args++;
    XtSetValues(w, args, num_args);
}

FILE *
SFopenFile(name, mode, prompt, failed)
    char *name;
    char *mode;
    char *prompt;
    char *failed;
{
    Arg args[1];
    FILE *fp;

    SFchdir(SFstartDir);
    errno = 0;
    if (!name || *name == 0 || (fp = fopen(name, mode)) == NULL) {
      char *buf;
      char *msg = errno ? strerror (errno) : "";
      buf = XtMalloc(strlen (failed) + strlen (msg) + strlen (prompt) + 2);
      strcpy(buf, failed);
      strcat(buf, msg);
      strcat(buf, " ");
      strcat(buf, prompt);
      XtSetArg(args[0], XtNlabel, buf);
      XtSetValues(selFilePrompt, args, ONE);
      XtFree(buf);
      fp = NULL;
    }
    
    return fp;
}

void
SFtextChanged()
{
	extern void SFupdatePath ();
	
	if ((SFtextBuffer[0] == '/') || (SFtextBuffer[0] == '~')) {
		(void) strcpy(SFcurrentPath, SFtextBuffer);

		SFtextPos = XawTextGetInsertionPoint(selFileField);
	} else {
		(void) strcat(strcpy(SFcurrentPath, SFstartDir), SFtextBuffer);

		SFtextPos = XawTextGetInsertionPoint(selFileField) +
			strlen(SFstartDir);
	}

	if (!SFworkProcAdded) {
		(void) XtAppAddWorkProc(SFapp, SFworkProc, NULL);
		SFworkProcAdded = 1;
	}

	SFupdatePath();
}

static char *
SFgetText()
{
	return strcpy(XtMalloc((unsigned) (strlen(SFtextBuffer) + 1)),
		SFtextBuffer);
}

static void
SFprepareToReturn()
{
	SFstatus = SEL_FILE_NULL;
	XtRemoveGrab(selFile);
	XtUnmapWidget(selFile);
	XtRemoveTimeOut(SFdirModTimerId);
	if (SFchdir(SFstartDir)) {
		XtAppError(
			SFapp,
			"XsraSelFile: can't return to current directory"
		);
	}
}

FILE *
XsraSelFile(toplevel, prompt, ok, cancel, failed,
	    init_path, mode, show_entry, name_return)
	Widget		toplevel;
	char		*prompt;
	char		*ok;
	char		*cancel;
	char		*failed;
	char		*init_path;
	char		*mode;
	int		(*show_entry)();
	char		**name_return;
{
	extern void SFsetText ();
	static int	firstTime = 1;
	Cardinal	i;
	Arg		arglist[20];
	XEvent		event;
	FILE		*fp;

	if (!prompt) {
		prompt = "Pathname:";
	}

	if (!ok) {
		ok = "OK";
	}

	if (!cancel) {
		cancel = "Cancel";
	}

	if (firstTime) {
		firstTime = 0;
		SFdisplay = XtDisplay(toplevel);
		SFcreateWidgets(toplevel, prompt, ok, cancel);
	} else {
		i = 0;

		XtSetArg(arglist[i], XtNlabel, prompt);			i++;
		XtSetValues(selFilePrompt, arglist, i);

		i = 0;
		XtSetArg(arglist[i], XtNlabel, ok);			i++;
		XtSetValues(selFileOK, arglist, i);

		i = 0;
		XtSetArg(arglist[i], XtNlabel, cancel);			i++;
		XtSetValues(selFileCancel, arglist, i);
	}

	SFpositionWidget(selFile);
	XtMapWidget(selFile);

        {
          char *cwd = xgetcwd ();
          strcpy (SFstartDir, cwd);
          free (cwd);
        }
	if (SFstartDir[0] == 0) {
		XtAppError(SFapp, "XsraSelFile: can't get current directory");
	}
	(void) strcat(SFstartDir, "/");
	(void) strcpy(SFcurrentDir, SFstartDir);

	if (init_path) {
		if (init_path[0] == '/') {
			(void) strcpy(SFcurrentPath, init_path);
			if (strncmp(
				SFcurrentPath,
				SFstartDir,
				strlen(SFstartDir)
			)) {
				SFsetText(SFcurrentPath);
			} else {
				SFsetText(&(SFcurrentPath[strlen(SFstartDir)]));
			}
		} else {
			(void) strcat(strcpy(SFcurrentPath, SFstartDir),
				init_path);
			SFsetText(&(SFcurrentPath[strlen(SFstartDir)]));
		}
	} else {
		(void) strcpy(SFcurrentPath, SFstartDir);
	}

/*	SFfunc = show_entry;
	disabled in order to implement file filter */
	SFfunc = showEntry;

	SFtextChanged();

	XtAddGrab(selFile, True, True);

	SFdirModTimerId = XtAppAddTimeOut(SFapp, (unsigned long) 1000,
		SFdirModTimer, (XtPointer) NULL);

	while (1) {
		XtAppNextEvent(SFapp, &event);
		XtDispatchEvent(&event);
		switch (SFstatus) {
		case SEL_FILE_TEXT:
			SFstatus = SEL_FILE_NULL;
			SFtextChanged();
			break;
		case SEL_FILE_OK:
			*name_return = SFgetText();
			if ((fp = SFopenFile(*name_return, mode,
					    prompt, failed))) {
				SFprepareToReturn();
				return fp;
			}
			SFstatus = SEL_FILE_NULL;
			break;
		case SEL_FILE_CANCEL:
			SFprepareToReturn();
			return NULL;
		case SEL_FILE_NULL:
			break;
		}
	}
}

#endif /* not NOSELFILE and not NOTOOL */
