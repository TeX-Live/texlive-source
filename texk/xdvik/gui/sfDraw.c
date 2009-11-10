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

#include "kpathsea/c-auto.h"
#include "kpathsea/config.h"
#include "kpathsea/c-stat.h"
#include "xdvi.h"
#include "x_util.h"

#include "sfDraw.h"
#include "sfDir.h"
#include "sfPath.h"

#if !defined(MOTIF)	/* for xdvik */

#define SF_DEFAULT_FONT "9x15"

typedef struct {
    char *font;
} TextData, *textPtr;

int SFcharWidth, SFcharAscent, SFcharHeight;

int SFcurrentInvert[3] = { -1, -1, -1 };

static GC SFlineGC, SFscrollGC, SFinvertGC, SFtextGC;

static XtResource textResources[] = {
    {XtNfont, XtCFont, XtRString, sizeof(char *),
     XtOffset(textPtr, font), XtRString, SF_DEFAULT_FONT},
};

static XFontStruct *SFfont;

static int SFcurrentListY;

static XtIntervalId SFscrollTimerId;

void
SFinitFont(void)
{
    TextData *data;

    data = XtNew(TextData);

    XtGetApplicationResources(selFileForm, (XtPointer) data, textResources,
			      XtNumber(textResources), (Arg *) NULL, ZERO);

    SFfont = XLoadQueryFont(SFdisplay, data->font);
    if (!SFfont) {
	SFfont = XLoadQueryFont(SFdisplay, SF_DEFAULT_FONT);
	if (!SFfont) {
	    char sbuf[256];

	    (void)sprintf(sbuf, "XsraSelFile: can't get font %s",
			  SF_DEFAULT_FONT);

	    XtAppError(SFapp, sbuf);
	}
    }

    TRACE_GUI((stderr, "FONT: width %d, %d\n", SFfont->max_bounds.width, SFfont->min_bounds.width));
    SFcharWidth = (SFfont->max_bounds.width + SFfont->min_bounds.width) / 2;
    SFcharAscent = SFfont->max_bounds.ascent;
    SFcharHeight = SFcharAscent + SFfont->max_bounds.descent;
    if (SFcharWidth == 0) {
	/* if min_bounds.width = -max_bounds.width, we probably
	   have a scalable TT font; try to determine its actual
	   width by measuring the letter `x':
	*/
	SFcharWidth = XTextWidth(SFfont, "x", 1);
    }
    if (SFcharWidth == 0) { /* last resort */
	SFcharWidth = SFfont->max_bounds.width / 2;
    }
    TRACE_GUI((stderr,
	       "Using font measures: charwidth %d, ascent %d, height %d",
	       SFcharWidth, SFcharAscent, SFcharHeight));
}

void
SFcreateGC(void)
{
    XGCValues gcValues;
    XRectangle rectangles[1];

    /*     XtVaGetValues(selFileLists[0], */
    /* 		  XtNforeground, &(gcValues.foreground), */
    /* 		  XtNbackground, &(gcValues.background), */
    /* 		  NULL); */
    gcValues.foreground = SFfore;

    SFlineGC = XtGetGC(selFileLists[0], (XtGCMask)
		       GCForeground | 0, &gcValues);

    SFscrollGC = XtGetGC(selFileLists[0], (XtGCMask)
			 0, &gcValues);

    gcValues.function = GXinvert;
    /*     gcValues.plane_mask = (gcValues.foreground ^ gcValues.background); */
    gcValues.plane_mask = (SFfore ^ SFback);

    SFinvertGC = XtGetGC(selFileLists[0], (XtGCMask)
			 GCFunction | GCPlaneMask | 0, &gcValues);

    gcValues.foreground = SFfore;
    gcValues.background = SFback;
    gcValues.font = SFfont->fid;

    SFtextGC = XCreateGC(SFdisplay, XtWindow(selFileLists[0]), (unsigned long)
			 GCForeground | GCBackground | GCFont | 0, &gcValues);

    rectangles[0].x = SFlineToTextH + SFbesideText;
    rectangles[0].y = 0;
    rectangles[0].width = SFcharsPerEntry * SFcharWidth;
    rectangles[0].height = SFupperY + 1;

    XSetClipRectangles(SFdisplay, SFtextGC, 0, 0, rectangles, 1, Unsorted);
}

void
SFclearList(int n, int doScroll)
{
    SFDir *dir;

    SFcurrentInvert[n] = -1;

    XClearWindow(SFdisplay, XtWindow(selFileLists[n]));

    XDrawSegments(SFdisplay, XtWindow(selFileLists[n]), SFlineGC, SFsegs, 2);

    if (doScroll) {
	dir = &(SFdirs[SFdirPtr + n]);

	if ((SFdirPtr + n < SFdirEnd) && dir->nEntries && dir->nChars) {
	    XawScrollbarSetThumb(selFileVScrolls[n],
				 (double)dir->vOrigin / dir->nEntries,
				 (double)(dir->nEntries < SFlistSize
					  ? dir->nEntries
					  : SFlistSize)
				 / dir->nEntries);

	    XawScrollbarSetThumb(selFileHScrolls[n],
				 (double)(dir->hOrigin) / dir->nChars,
				 (double)(dir->nChars < SFcharsPerEntry
					  ? dir-> nChars : SFcharsPerEntry)
				 / dir->nChars);
	}
	else {
	    XawScrollbarSetThumb(selFileVScrolls[n], 0.0, 1.0);
	    XawScrollbarSetThumb(selFileHScrolls[n], 0.0, 1.0);
	}
    }
}

static void
SFdeleteEntry(SFDir *dir, SFEntry *entry)
{
    SFEntry *e;
    SFEntry *end;
    int n;
    int idx;

    idx = entry - dir->entries;

    if (idx < dir->beginSelection) {
	dir->beginSelection--;
    }
    if (idx <= dir->endSelection) {
	dir->endSelection--;
    }
    if (dir->beginSelection > dir->endSelection) {
	dir->beginSelection = dir->endSelection = -1;
    }

    if (idx < dir->vOrigin) {
	dir->vOrigin--;
    }

    XtFree(entry->real);

    end = &(dir->entries[dir->nEntries - 1]);

    for (e = entry; e < end; e++) {
	*e = *(e + 1);
    }

    if (!(--dir->nEntries)) {
	return;
    }

    n = dir - &(SFdirs[SFdirPtr]);
    if ((n < 0) || (n > 2)) {
	return;
    }

    XawScrollbarSetThumb(selFileVScrolls[n],
			 (double)(dir->vOrigin) / dir->nEntries,
			 (double)(dir->nEntries < SFlistSize
				  ? dir->nEntries : SFlistSize)
			 / dir->nEntries);
}

static void
SFwriteStatChar(char *name, int last, struct stat *statBuf)
{
    name[last] = SFstatChar(statBuf);
}

static int
SFstatAndCheck(SFDir *dir, SFEntry *entry)
{
    struct stat statBuf;
    char save;
    int last;
    extern int SFchdir();

    /*
     * must be restored before returning
     */
    save = *(dir->path);
    *(dir->path) = 0;

    if (!SFchdir(SFcurrentPath)) {
	last = strlen(entry->real) - 1;
	entry->real[last] = 0;
	entry->statDone = 1;
	if ((!stat(entry->real, &statBuf))
#ifdef S_IFLNK
	    || (!lstat(entry->real, &statBuf))
#endif /* ndef S_IFLNK */
	    ) {
	    if (SFfunc) {
		char *shown;

		shown = NULL;
		if (SFfunc(entry->real, &shown, &statBuf)) {
		    if (shown) {
			int len;

			len = strlen(shown);
			entry->shown = XtMalloc((unsigned)(len + 2)
						);
			(void)strcpy(entry->shown, shown);
			SFwriteStatChar(entry->shown, len, &statBuf);
			entry->shown[len + 1] = 0;
		    }
		}
		else {
		    SFdeleteEntry(dir, entry);

		    *(dir->path) = save;
		    return 1;
		}
	    }
	    SFwriteStatChar(entry->real, last, &statBuf);
	}
	else {
	    entry->real[last] = ' ';
	}
    }

    *(dir->path) = save;
    return 0;
}

static void
SFdrawStrings(Window w, SFDir *dir, int from, int to)
{
    int i;
    SFEntry *entry;
    int x;

    x = SFtextX - dir->hOrigin * SFcharWidth;

    if (dir->vOrigin + to >= dir->nEntries) {
	to = dir->nEntries - dir->vOrigin - 1;
    }
    for (i = from; i <= to; i++) {
	entry = &(dir->entries[dir->vOrigin + i]);
	if (!(entry->statDone)) {
	    if (SFstatAndCheck(dir, entry)) {
		if (dir->vOrigin + to >= dir->nEntries) {
		    to = dir->nEntries - dir->vOrigin - 1;
		}
		i--;
		continue;
	    }
	}
	XDrawImageString(SFdisplay,
			 w,
			 SFtextGC,
			 x,
			 SFtextYoffset + i * SFentryHeight,
			 entry->shown, strlen(entry->shown));
	if (dir->vOrigin + i == dir->beginSelection) {
	    XDrawLine(SFdisplay,
		      w,
		      SFlineGC,
		      SFlineToTextH + 1,
		      SFlowerY + i * SFentryHeight,
		      SFlineToTextH + SFentryWidth - 2,
		      SFlowerY + i * SFentryHeight);
	}
	if (
	    (dir->vOrigin + i >= dir->beginSelection) &&
	    (dir->vOrigin + i <= dir->endSelection)) {
	    SFcompletionSegs[0].y1 = SFcompletionSegs[1].y1 =
		SFlowerY + i * SFentryHeight;
	    SFcompletionSegs[0].y2 = SFcompletionSegs[1].y2 =
		SFlowerY + (i + 1) * SFentryHeight - 1;
	    XDrawSegments(SFdisplay, w, SFlineGC, SFcompletionSegs, 2);
	}
	if (dir->vOrigin + i == dir->endSelection) {
	    XDrawLine(SFdisplay,
		      w,
		      SFlineGC,
		      SFlineToTextH + 1,
		      SFlowerY + (i + 1) * SFentryHeight - 1,
		      SFlineToTextH + SFentryWidth - 2,
		      SFlowerY + (i + 1) * SFentryHeight - 1);
	}
    }
}

void
SFdrawList(int n, int doScroll)
{
    SFDir *dir;
    Window w;

    SFclearList(n, doScroll);

    if (SFdirPtr + n < SFdirEnd) {
	dir = &(SFdirs[SFdirPtr + n]);
	w = XtWindow(selFileLists[n]);
	XDrawImageString(SFdisplay,
			 w,
			 SFtextGC,
			 SFtextX - dir->hOrigin * SFcharWidth,
			 SFlineToTextV + SFaboveAndBelowText + SFcharAscent,
			 dir->dir, strlen(dir->dir)
			 );
	SFdrawStrings(w, dir, 0, SFlistSize - 1);
    }
}

void
SFdrawLists(int doScroll)
{
    int i;

    for (i = 0; i < 3; i++) {
	SFdrawList(i, doScroll);
    }
}

static void
SFinvertEntry(int n)
{
    XFillRectangle(SFdisplay,
		   XtWindow(selFileLists[n]),
		   SFinvertGC,
		   SFlineToTextH,
		   SFcurrentInvert[n] * SFentryHeight + SFlowerY,
		   SFentryWidth, SFentryHeight);
}

static unsigned long
SFscrollTimerInterval(void)
{
    static int maxVal = 200;
    static int varyDist = 50;
    static int minDist = 50;
    int t;
    int dist;

    if (SFcurrentListY < SFlowerY) {
	dist = SFlowerY - SFcurrentListY;
    }
    else if (SFcurrentListY > SFupperY) {
	dist = SFcurrentListY - SFupperY;
    }
    else {
	return (unsigned long)1;
    }

    t = maxVal - ((maxVal / varyDist) * (dist - minDist));

    if (t < 1) {
	t = 1;
    }

    if (t > maxVal) {
	t = maxVal;
    }

    return (unsigned long)t;
}

static void
SFscrollTimer(XtPointer p, XtIntervalId *id)
{
    SFDir *dir;
    int save;
    ptrdiff_t n;

    UNUSED(id);
    
    n = (ptrdiff_t)p;

    fprintf(stderr, "SFscrollTimer called!\n");
    
    dir = &(SFdirs[SFdirPtr + n]);
    save = dir->vOrigin;

    if (SFcurrentListY < SFlowerY) {
	if (dir->vOrigin > 0) {
	    SFvSliderMovedCallback(selFileVScrolls[n], n, dir->vOrigin - 1);
	}
    }
    else if (SFcurrentListY > SFupperY) {
	if (dir->vOrigin < dir->nEntries - SFlistSize) {
	    SFvSliderMovedCallback(selFileVScrolls[n], n, dir->vOrigin + 1);
	}
    }

    if (dir->vOrigin != save) {
	if (dir->nEntries) {
	    XawScrollbarSetThumb(selFileVScrolls[n],
				 (double)(dir->vOrigin) / dir->nEntries,
				 (double)(dir->nEntries < SFlistSize
					  ? dir-> nEntries : SFlistSize)
				 / dir->nEntries);
	}
    }

    if (SFbuttonPressed) {
	SFscrollTimerId = XtAppAddTimeOut(SFapp,
					  SFscrollTimerInterval(),
					  SFscrollTimer, (XtPointer)n);
    }
}

static int
SFnewInvertEntry(int n, XMotionEvent *event)
{
    int x, y;
    int new;
    static int SFscrollTimerAdded = 0;

    x = event->x;
    y = event->y;

    if (SFdirPtr + n >= SFdirEnd) {
	return -1;
    }
    else if ((x >= 0) && (x <= SFupperX) && (y >= SFlowerY) && (y <= SFupperY)
	     ) {
	SFDir *dir = &(SFdirs[SFdirPtr + n]);

	if (SFscrollTimerAdded) {
	    SFscrollTimerAdded = 0;
	    XtRemoveTimeOut(SFscrollTimerId);
	}

	new = (y - SFlowerY) / SFentryHeight;
	if (dir->vOrigin + new >= dir->nEntries) {
	    return -1;
	}
	return new;
    }
    else {
	if (SFbuttonPressed) {
	    SFcurrentListY = y;
	    if (!SFscrollTimerAdded) {
		SFscrollTimerAdded = 1;
		SFscrollTimerId = XtAppAddTimeOut(SFapp,
						  SFscrollTimerInterval(),
						  SFscrollTimer, cast_int_to_XtPointer(n));
	    }
	}

	return -1;
    }
}

void
SFenterList(Widget w, int n, XEnterWindowEvent *event)
{
    int new;

    UNUSED(w);
    
    /* sanity */
    if (SFcurrentInvert[n] != -1) {
	SFinvertEntry(n);
	SFcurrentInvert[n] = -1;
    }

    new = SFnewInvertEntry(n, (XMotionEvent *) event);
    if (new != -1) {
	SFcurrentInvert[n] = new;
	SFinvertEntry(n);
    }
}

void
SFleaveList(Widget w, int n, XEvent *event)
{
    UNUSED(w);
    UNUSED(event);
    
    if (SFcurrentInvert[n] != -1) {
	SFinvertEntry(n);
	SFcurrentInvert[n] = -1;
    }
}

void
SFmotionList(Widget w, int n, XMotionEvent *event)
{
    int new;

    UNUSED(w);
    
    new = SFnewInvertEntry(n, event);

    if (new != SFcurrentInvert[n]) {
	if (SFcurrentInvert[n] != -1) {
	    SFinvertEntry(n);
	}
	SFcurrentInvert[n] = new;
	if (new != -1) {
	    SFinvertEntry(n);
	}
    }
}

void
SFvFloatSliderMovedCallback(Widget w, int n, float *fnew)
{
    int new;

    new = (*fnew) * SFdirs[SFdirPtr + n].nEntries;

    SFvSliderMovedCallback(w, n, new);
}

void
SFvSliderMovedCallback(Widget w, int n, int new)
{
    int old;
    Window win;
    SFDir *dir;

    UNUSED(w);
    
    dir = &(SFdirs[SFdirPtr + n]);

    old = dir->vOrigin;
    dir->vOrigin = new;

    if (old == new) {
	return;
    }

    win = XtWindow(selFileLists[n]);

    if (ABS(new - old) < SFlistSize) {
	if (new > old) {
	    XCopyArea(SFdisplay,
		      win,
		      win,
		      SFscrollGC,
		      SFlineToTextH,
		      SFlowerY + (new - old) * SFentryHeight,
		      SFentryWidth + SFlineToTextH,
		      (SFlistSize - (new - old)) * SFentryHeight,
		      SFlineToTextH, SFlowerY);
	    XClearArea(SFdisplay,
		       win,
		       SFlineToTextH,
		       SFlowerY + (SFlistSize - (new - old)) *
		       SFentryHeight,
		       SFentryWidth + SFlineToTextH,
		       (new - old) * SFentryHeight, False);
	    SFdrawStrings(win, dir, SFlistSize - (new - old), SFlistSize - 1);
	}
	else {
	    XCopyArea(SFdisplay,
		      win,
		      win,
		      SFscrollGC,
		      SFlineToTextH,
		      SFlowerY,
		      SFentryWidth + SFlineToTextH,
		      (SFlistSize - (old - new)) * SFentryHeight,
		      SFlineToTextH, SFlowerY + (old - new) * SFentryHeight);
	    XClearArea(SFdisplay,
		       win,
		       SFlineToTextH,
		       SFlowerY,
		       SFentryWidth + SFlineToTextH,
		       (old - new) * SFentryHeight, False);
	    SFdrawStrings(win, dir, 0, old - new);
	}
    }
    else {
	XClearArea(SFdisplay,
		   win,
		   SFlineToTextH,
		   SFlowerY,
		   SFentryWidth + SFlineToTextH,
		   SFlistSize * SFentryHeight, False);
	SFdrawStrings(win, dir, 0, SFlistSize - 1);
    }
}

void
SFvAreaSelectedCallback(Widget w, int n, int pnew)
{
    SFDir *dir;
    int new;

    dir = &(SFdirs[SFdirPtr + n]);

    new = dir->vOrigin + ((double)pnew / SFvScrollHeight) * dir->nEntries;

    if (new > dir->nEntries - SFlistSize) {
	new = dir->nEntries - SFlistSize;
    }

    if (new < 0) {
	new = 0;
    }

    if (dir->nEntries) {
	float f;

	f = ((double)new) / dir->nEntries;

	XawScrollbarSetThumb(w, f,
			     (double)(dir->nEntries < SFlistSize
				      ? dir->nEntries
				      : SFlistSize)
			     / dir->nEntries);
    }

    SFvSliderMovedCallback(w, n, new);
}

void
SFhSliderMovedCallback(Widget w, int n, float *new)
{
    SFDir *dir;
    int save;

    UNUSED(w);
    
    dir = &(SFdirs[SFdirPtr + n]);
    save = dir->hOrigin;
    dir->hOrigin = (*new) * dir->nChars;
    if (dir->hOrigin == save) {
	return;
    }

    SFdrawList(n, SF_DO_NOT_SCROLL);
}

void
SFhAreaSelectedCallback(Widget w, int n, int pnew)
{
    SFDir *dir;
    int new;

    dir = &(SFdirs[SFdirPtr + n]);

    new = dir->hOrigin + (((double)pnew) / SFhScrollWidth) * dir->nChars;

    if (new > dir->nChars - SFcharsPerEntry) {
	new = dir->nChars - SFcharsPerEntry;
    }

    if (new < 0) {
	new = 0;
    }

    if (dir->nChars) {
	float f;

	f = (double)new / dir->nChars;

	XawScrollbarSetThumb(w, f,
			     (double)(dir->nChars < SFcharsPerEntry
				      ? dir->nChars
				      : SFcharsPerEntry)
			     / dir->nChars);

	SFhSliderMovedCallback(w, n, &f);
    }
}

void
SFpathSliderMovedCallback(Widget w, XtPointer client_data, float *new)
{
    SFDir *dir;
    int n;
    XawTextPosition pos;
    int SFdirPtrSave;

    UNUSED(w);
    UNUSED(client_data);
    
    SFdirPtrSave = SFdirPtr;
    SFdirPtr = (*new) * SFdirEnd;
    if (SFdirPtr == SFdirPtrSave) {
	return;
    }

    SFdrawLists(SF_DO_SCROLL);

    n = 2;
    while (SFdirPtr + n >= SFdirEnd) {
	n--;
    }

    dir = &(SFdirs[SFdirPtr + n]);

    pos = dir->path - SFcurrentPath;

    if (!strncmp(SFcurrentPath, SFstartDir, strlen(SFstartDir))) {
	pos -= strlen(SFstartDir);
	if (pos < 0) {
	    pos = 0;
	}
    }

    XawTextSetInsertionPoint(selFileField, pos);
}

void
SFpathAreaSelectedCallback(Widget w, XtPointer client_data, int pnew)
{
    int new;
    float f;

    UNUSED(client_data);
    
    new = SFdirPtr + (((double)pnew) / SFpathScrollWidth) * SFdirEnd;

    if (new > SFdirEnd - 3) {
	new = SFdirEnd - 3;
    }

    if (new < 0) {
	new = 0;
    }

    f = ((double)new) / SFdirEnd;

    XawScrollbarSetThumb(w, f,
			 (double)(SFdirEnd < 3 ? SFdirEnd : 3) / SFdirEnd);

    SFpathSliderMovedCallback(w, (XtPointer) NULL, &f);
}

Boolean
SFworkProc(void)
{
    SFDir *dir;
    SFEntry *entry;

    for (dir = &(SFdirs[SFdirEnd - 1]); dir >= SFdirs; dir--) {
	if (!(dir->nEntries)) {
	    continue;
	}
	for (entry = &(dir->entries[dir->nEntries - 1]);
	     entry >= dir->entries; entry--) {
	    if (!(entry->statDone)) {
		(void)SFstatAndCheck(dir, entry);
		return False;
	    }
	}
    }

    SFworkProcAdded = 0;

    return True;
}

#else
/* silence `empty compilation unit' warnings */
static void bar(void); static void foo(void) { bar(); } static void bar(void) { foo(); }
#endif /* !defined(MOTIF) */
