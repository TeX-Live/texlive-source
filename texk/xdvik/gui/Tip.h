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

#ifndef TIP_H_
#define TIP_H_

#ifdef MOTIF
/* Tooltip widget handling.  This has somewhat of a library character,
 * but we don't want to go into the trouble of making a library for just
 * one function.
 */
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

/* New resource names
 */
#define XmNcancelWaitPeriod	 "cancelWaitPeriod"
#define XmNwaitPeriod		 "waitPeriod"

/* New resource classes
 */
#define XmCCancelWaitPeriod	"CancelWaitPeriod"
#define XmCWaitPeriod		"WaitPeriod"

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

    extern WidgetClass tipWidgetClass;
    typedef struct _TipClassRec *TipWidgetClass;
    typedef struct _TipRec *TipWidget;

    extern void TipAddWidget(Widget, Widget, const String);
    extern void TipAppMainLoop(XtAppContext);
    extern void TipAppHandle(XtAppContext, XEvent *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif /* MOTIF */
#endif /* TIP_H_ */
