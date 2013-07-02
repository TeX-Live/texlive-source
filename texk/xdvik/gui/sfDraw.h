/*
 * Copyright (c) 2002-2004 the xdvik development team
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

#ifndef SFDRAW_H_
#define SFDRAW_H_

#if !defined(MOTIF)	/* for xdvik */

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xos.h>
#include <X11/Xaw/Text.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Scrollbar.h>
#include <X11/Xaw/Cardinals.h>

#define SF_DO_SCROLL		1
#define SF_DO_NOT_SCROLL	0

extern Pixel SFfore, SFback;

void SFinitFont(void);
void SFcreateGC(void);
void SFclearList(int n, int doScroll);
void SFdrawList(int n, int doScroll);
void SFdrawLists(int doScroll);
void SFenterList(Widget w, int n, XEnterWindowEvent *event);
void SFleaveList(Widget w, int n, XEvent *event);
void SFmotionList(Widget w, int n, XMotionEvent *event);
void SFvFloatSliderMovedCallback(Widget w, int n, float *fnew);
void SFvSliderMovedCallback(Widget w, int n, int new);
void SFvAreaSelectedCallback(Widget w, int n, int pnew);
void SFhSliderMovedCallback(Widget w, int n, float *new);
void SFhAreaSelectedCallback(Widget w, int n, int pnew);
void SFpathSliderMovedCallback(Widget w, XtPointer client_data, float *new);
void SFpathAreaSelectedCallback(Widget w, XtPointer client_data, int pnew);
Boolean SFworkProc(void);
extern int (*SFfunc)();

extern Widget selFileField, selFileForm, selFileHScroll, selFileHScrolls[], selFileLists[], selFileVScrolls[];
extern Display *SFdisplay;
extern int SFcharWidth, SFcharHeight, SFcharAscent;
extern XSegment SFsegs[], SFcompletionSegs[];
extern XawTextPosition SFtextPos;

extern int SFupperX, SFlowerY, SFupperY;

extern int SFtextX, SFtextYoffset;

extern int SFentryWidth, SFentryHeight;

extern int SFlineToTextH, SFlineToTextV;

extern int SFbesideText, SFaboveAndBelowText;

extern int SFcharsPerEntry;

extern int SFlistSize;

extern int SFcurrentInvert[];

extern int SFworkProcAdded;

extern XtAppContext SFapp;

extern int SFpathScrollWidth, SFvScrollHeight, SFhScrollWidth;

extern char SFtextBuffer[];

extern int SFbuttonPressed;

extern XtIntervalId SFdirModTimerId;

#endif  /* !defined(MOTIF) */

#endif /* SFDRAW_H_ */
