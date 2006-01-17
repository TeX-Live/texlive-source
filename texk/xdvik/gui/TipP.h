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

#ifndef TIPP_h
#define TIPP_h

#include <X11/ShellP.h>

#include "Tip.h"

/* Doubly Linked List Processing.
 */
struct list_thread_str {
    struct list_thread_str *forw;
    struct list_thread_str *back;
};
typedef struct list_thread_str ListThread;

typedef struct {
    int __empty;
} TipClassPart;

/* Full class record declaration.
 */
typedef struct _TipClassRec {
    CoreClassPart core_class;
    CompositeClassPart composite_class;
    ShellClassPart shell_class;
    OverrideShellClassPart override_shell_class;
    TipClassPart tip_class;
} TipClassRec;

extern TipClassRec xmTipClassRec;

/* New fields for the widget record.
 */
typedef struct {
    /* resources */
    Pixel foreground;
    XFontSet fontset;		/* the font for text in box */
    int waitPeriod;		/* the delay resource - pointer must be
				   in watched widget this long before
				   tooltip is displayed - in milliseconds
				 */
    int cancelWaitPeriod;	/* after help is popped-down - normal
				   wait period is cancelled for this
				   period - in milliseconds
				 */

    /* -------- private state --------- */
    ListThread widget_list;	/* list of widgets we are liteClue-ing */
    Dimension font_width;	/* width of '1' character */
    Dimension font_height;	/* height of font, rows are spaced using this */
    Dimension font_baseline;	/* relative displacement to baseline from top */
    GC text_GC;			/* for drawing text */
    XtIntervalId tid;		/* holds timer id */
    XtIntervalId pid;		/* holds pooler id for insensitive widgets */
    Widget parent;
    Widget isup;		/* the help popup is up on this widget */
    Time HelpPopDownTime;	/* the time at which help popup was popped down */
} TipPart;


/*
 * Full instance record declaration
 */
typedef struct _TipRec {
    CorePart core;
    CompositePart composite;
    ShellPart shell;
    OverrideShellPart override;
    TipPart tip;
} TipRec;

#endif
