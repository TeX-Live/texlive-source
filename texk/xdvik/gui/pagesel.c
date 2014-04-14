/*
 * Page selector for xdvi
 *
 * Copyright (c) 2001-2004 xdvik development team
 *
 * This code is derived from the page selector in xdvik-j, and
 * parts of it are Copyright (c) 1993, 1995
 *      MATSUURA Syun           syun@fuka.info.waseda.ac.jp
 *      HIRAHARA Atsushi        hirahara@fuka.info.waseda.ac.jp
 *      ONO Kouichi             onono@fuka.info.waseda.ac.jp
 * All rights reserved.
 *
 *
 * (SU: I was unsure how to interpret the `All rights reserved' in the
 * previous line, so emailed Ono Kouichi about this.  Here's a
 * verbatim quote of the relevant part of his answer (which was CC'ed
 * to Hirahara Atsushi - all three of them had left Waseda university
 * around '95):
 *
 *    You can modify, embed, copy and distribute a part of or the
 *    entire of our source code when you specify our copyright in your
 *    xdvik version.
 *
 * IANAL, but I think this is compatible with the X consortium
 * license, which follows.)
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
  BUGS:

  - with Xaw, the highlighting for the selected page (XawListHighlight)
  vanishes when mouse is dragged outside the widget and then released
  (but the respective page is selected, which is IMHO the desired behaviour).
     
  - with Xaw, scrolling the list with PgUp/PgDown until the current page
  gets `out of focus' should un-highlight the current page number
     
  - The ASCII-based marks are *ugly*. Pixmaps (for the marked state)
  would be better. The viewer gv has one (but it's Xaw only). Some
  file directory widgets like
  e.g. http://ftp.xfree86.org/pub/X.Org/contrib/widgets/ListTree-3.0b3.tar.gz
  also have facilities for that, but most suffer from other
  inadequacies (e.g. no such ting as browseSelection) and all kinds
  of bitrot ...  Another alternative would be using XmContainer
  (see e.g. the `filemanager' example in demos/programs/filemanagers
  in the openmotif distribution), but that's available for Motif >= 2.1 only.

*/

#include "xdvi-config.h"
#include "xdvi.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>

#include "xm_toolbar.h"
#include "xm_menu.h"
#include "xaw_menu.h"

#include "x_util.h"

#ifdef MOTIF
# include <Xm/Xm.h>
# include <Xm/List.h>
# include <Xm/ScrollBar.h> /* for XmScrollBarGetValues */
#else /* MOTIF */
# include <X11/Xaw/Dialog.h>
# include <X11/Xaw/Cardinals.h>
# include <X11/Xaw/Command.h>
# include <X11/Xaw/List.h>
# include <X11/Xaw/Viewport.h>
#endif /* MOTIF */

#include "message-window.h"
#include "pagesel.h"
#include "util.h"
#include "string-utils.h"
#include "dvi-init.h"
#include "statusline.h"
#include "events.h"
#include "print-dialog.h"
#include "search-internal.h"
#include "pagehist.h"

#define	PAGENUMLEN 128
#define SCROLL_LIST_SCROLLBAR 0

#ifndef	MAX_PAGE
# define MAX_PAGE    (1024)
#endif /* MAX_PAGE */

#define	LONGESTPAGENUM  55


/* for saving the GC of the pagelist widget
   (when un-highlighting items in highlight_page_callback, and
   drawing the `current' marker)
*/
static struct page_gc {
    GC fore;
    GC back;
} m_page_gc;

#define MOTIF_IDX_OFFSET 1 /* motif index starts at 1, not 0 */
#if !defined(LESSTIF_VERSION)
static Boolean my_list_pos_to_bounds(Widget widget, int idx, Position *x, Position *y, Dimension *w, Dimension *h);
static void refresh_highlight_marker(Widget widget, GC gc, Position x, Position y, Dimension w, Dimension h);
#endif /* !defined(LESSTIF_VERSION) */



#ifdef MOTIF

/* make button2 mark instead of drag&drop */
static void xm_list_set_mark(Widget widget, XEvent *event, String *params, Cardinal *num_params);
static void xm_list_drag_mark(Widget widget, XEvent *event, String *params, Cardinal *num_params);
static XtActionsRec CustomListActions[] = {
    { "ListSetMark",		xm_list_set_mark	},
    { "ListDragMark",		xm_list_drag_mark	},
};
static char *motif_custom_translations =
"#override \n"
"s <Btn2Down>:                   ListDragMark(ListButtonMotion)\n"
"<Btn2Motion>:                   ListSetMark(ListButtonMotion)\n"
"<Btn2Up>:                       ListSetMark(ListButtonMotion)\n"
"<Btn2Down>:                     ListSetMark(ListButtonMotion)\n"
"<Btn4Down>,<Btn4Up>:            scroll-list-up()\n"
"<Btn5Down>,<Btn5Up>:            scroll-list-down()\n"
/* /\*     "s ~m ~a <Btn2Down>:             ListMyProcessBtn2(ListBeginExtend)\n" *\/ */
/* /\*     "s ~m ~a <Btn2Up>:               ListMyProcessBtn2(ListEndExtend)\n" *\/ */
/* /\*     "~c ~s ~m ~a <Btn2Down>:         ListMyProcessBtn2(ListBeginSelect)\n" *\/ */
/* /\*     "~c ~s ~m ~a <Btn2Up>:           ListMyProcessBtn2(ListEndSelect)\n" *\/ */
/* /\*     "c ~s ~m ~a <Btn2Down>:          ListMyProcessBtn2(ListBeginToggle)\n" *\/ */
/* /\*     "c ~s ~m ~a <Btn2Up>:            ListMyProcessBtn2(ListEndToggle)\n" *\/ */
/* /\*     "c ~s ~m a <Btn2Down>:           ListProcessDrag()\n" *\/ */
/* /\*     "~c s ~m a <Btn2Down>:           ListProcessDrag()\n" *\/ */
;

#define LIST_WIDGET page_list
/* motif pagenumber is a string */
static const char* const pageno_format = "%c %s  ";

#else /* MOTIF */

static int view_y;
extern Widget panel_widget;
static Widget list_widget = NULL;
static Widget viewport = NULL;
#define LIST_WIDGET list_widget
/* Xaw pagenumber is an integer, and we need to left-pad it */
static const char* const pageno_format = "%c %*d  ";

static int xaw_maybe_scroll_pagelist(int new_page, Boolean force_recenter, int old);

#define REDRAW_CURRENT_MARKER_HACK 1


/*
  The following hack tries to address the following 2 bugs with the
  self-made page highlighting marker:
  
  - the marker overlaps with the ordinary XawListHighlight marker;
  when un-highlighting a page, 1 pixel (vertically) at the edge of
  the ordinary marker is overdrawn.

  - When the XawListHighlight crosses the self-drawn rectangle,
  the vertical bars remain visible, but the horizontal bars
  are erased.

  The hack just redraws the appropriate items whenever one of
  the above can happen, i.e. when the two markers are 2 or less
  pages apart from each other.
*/
#if REDRAW_CURRENT_MARKER_HACK
/* Store index of currently marked (with our own marker) list
   item, or -1 if none is marked. */
static int g_current_highlighted = -1;

/* redraw the default Xaw list highlight (XawListHighlight()) */
static void
xaw_maybe_redraw_highlight(int idx)
{
    XawListReturnStruct *ret;
    int high;
    
    if (LIST_WIDGET == NULL)
	return;
    
    ret = XawListShowCurrent(LIST_WIDGET);
    high = ret->list_index;
    if (high != XAW_LIST_NONE && abs(idx - (high + MOTIF_IDX_OFFSET)) <= 2) {
	/* re-highlight it */
	XawListHighlight(LIST_WIDGET, high);
    }
    g_current_highlighted = -1;
}

/* redraw our own rectangle highlight marker: */
static void
xaw_maybe_redraw_current_marker(int idx)
{
    Position x, y;
    Dimension w, h;

    /*     fprintf(stderr, "idx: %d, high: %d; diff: %d\n", idx + MOTIF_IDX_OFFSET, g_current_highlighted, */
    /* 	    abs(idx + MOTIF_IDX_OFFSET - g_current_highlighted)); */
    if (abs((idx + MOTIF_IDX_OFFSET) - g_current_highlighted) <= 2
	&& my_list_pos_to_bounds(LIST_WIDGET, g_current_highlighted, &x, &y, &w, &h)) {
	refresh_highlight_marker(LIST_WIDGET, m_page_gc.fore, x, y, w, h);
    }
}
#endif /* REDRAW_CURRENT_MARKER_HACK */

#endif /* MOTIF */

/*
 * Table of page offsets in DVI file, indexed by page number - 1,
 * marked pages, and page sizes.
 * Initialized in prepare_pages().
 */
struct page_index {
    long offset;
    int number;
    Boolean marked;
    unsigned int pw, ph; /* page size */
    unsigned int ww, wh; /* window size */
};

struct page_index_info {
    struct page_index *index; /* above struct */
    size_t index_size;	 /* size of currently allocated index */
    char **page_labels;  /* label strings */
};

static struct page_index_info page_info;

/* access functions used by dvi-draw.c and dvi-init.c */
long
pageinfo_get_offset(int page)
{
    ASSERT(page >= 0 && page < (int)page_info.index_size, "Page number out of range");
    /*      fprintf(stderr, "offset for page %d is %ld\n", page, page_info.index[page].offset); */
    return page_info.index[page].offset;
}

/* access functions used by dvi-draw.c and dvi-init.c */
unsigned int
pageinfo_get_page_width(int page)
{
    ASSERT(page >= 0 && page < (int)page_info.index_size, "Page number out of range");
    return page_info.index[page].pw;
}

unsigned int
pageinfo_get_page_height(int page)
{
    ASSERT(page >= 0 && page < (int)page_info.index_size, "Page number out of range");
    return page_info.index[page].ph;
}

unsigned int
pageinfo_get_window_width(int page)
{
    ASSERT(page >= 0 && page < (int)page_info.index_size, "Page number out of range");
    return page_info.index[page].ww;
}

unsigned int
pageinfo_get_window_height(int page)
{
    ASSERT(page >= 0 && page < (int)page_info.index_size, "Page number out of range");
    return page_info.index[page].wh;
}

void
pageinfo_set_page_width(int page, unsigned int width)
{
    ASSERT(page >= 0 && page < (int)page_info.index_size, "Page number out of range");
    page_info.index[page].pw = width;
}

void
pageinfo_set_page_height(int page, unsigned int height)
{
    ASSERT(page >= 0 && page < (int)page_info.index_size, "Page number out of range");
    page_info.index[page].ph = height;
}

void
pageinfo_set_window_width(int page, unsigned int width)
{
    ASSERT(page >= 0 && page < (int)page_info.index_size, "Page number out of range");
    page_info.index[page].ww = width;
}

void
pageinfo_set_window_height(int page, unsigned int height)
{
    ASSERT(page >= 0 && page < (int)page_info.index_size, "Page number out of range");
    page_info.index[page].wh = height;
}

int
pageinfo_get_number(int page)
{
    ASSERT(page >= 0 && page < (int)page_info.index_size, "Page number out of range");
    return page_info.index[page].number;
}

/* search for page with TeX number `number', and return its index, or -1 if it's not found. */
int
pageinfo_get_index_of_number(int number)
{
    size_t i;
    for (i = 0; i < page_info.index_size - 1; i++) {
	if (number == page_info.index[i].number)
	    return i;
    }
    return -1;
}

void
pageinfo_set_offset(int index, long offset)
{
    ASSERT(index >= 0 && index < (int)page_info.index_size, "");
    page_info.index[index].offset = offset;
}

void
pageinfo_set_number(int index, int number)
{
    ASSERT(index >= 0 && index < (int)page_info.index_size, "");
    page_info.index[index].number = number;
}

void
pageinfo_allocate(int total_pages)
{
    int i;
    page_info.index = xmalloc(total_pages * sizeof *(page_info.index));
    for (i = 0; i < total_pages; i++) {
	page_info.index[i].marked = False;
    }
    /* following initializations are handled by the respective Motif/Xaw functions */
    page_info.page_labels = NULL;
    page_info.index_size = total_pages;
}

/*
  Deallocate page_info. NOTE: We mustn't free the page_labels here,
  since the page list might survive quite some time (e.g. while fonts
  for the new file are being generated) and needs the labels.
*/
void
pageinfo_deallocate(void)
{
    free(page_info.index);
    page_info.index_size = 0;
    page_info.index = NULL;
}

#ifdef MOTIF
void
toggle_pagelist(void)
{
    Dimension curr_w, curr_x;
    XtVaGetValues(globals.widgets.main_window,
		  XmNwidth, &curr_w,
		  XmNx, &curr_x,
		  NULL);

    if ((resource.expert_mode & XPRT_SHOW_PAGELIST) != 0) {
	XtManageChild(XtParent(page_list));
	XtManageChild(page_list);
	
	curr_x += resource.pagelist_width;
	curr_w -= resource.pagelist_width;
	XtVaSetValues(globals.widgets.main_window,
		      XtNwidth, curr_w,
		      XtNx, curr_x,
		      XmNleftAttachment, XmATTACH_WIDGET,
		      XmNleftWidget, XtParent(page_list),
		      NULL);
    }
    else {
	XtUnmanageChild(XtParent(page_list));
	XtUnmanageChild(page_list);
	
	curr_x -= resource.pagelist_width;
	curr_w += resource.pagelist_width;
	XtVaSetValues(globals.widgets.main_window,
		      XmNwidth, curr_w,
		      XmNx, curr_x,
		      XmNleftAttachment, XmATTACH_FORM,
		      NULL);
    }

    set_menu(&resource.expert_mode, Act_set_expert_mode, check_resource_expert);
}
#endif


Boolean
pageinfo_have_marked_pages(void)
{
    int i;
    for (i = 0; i < total_pages; i++) {
	if (page_info.index[i].marked) {
	    return True;
	}
    }
    return False;
}


/* return True if page i is marked, False else */
Boolean
pageinfo_is_marked(int i)
{
    ASSERT(i <= (int)page_info.index_size, "");
    return page_info.index[i].marked;
}


typedef enum { SCROLL_UP, SCROLL_DOWN, CLICK } saveCmdT;


static void internal_process_button2(Widget widget, XEvent *event);
static int get_item_index(Widget w, int mouse_y);

static int
get_page_size(void)
{
    int offset = 0;
    int min_page = 0;
    int max_page = 0;
    int min_pageno_len = 0;
    int max_pageno_len = 0;
    int i;

    if (globals.dvi_file.bak_fp == NULL)
	return 0;
    
    for (i = 0; i < total_pages; i++) {
	max_page = MAX(page_info.index[i].number, max_page);
	min_page = MIN(page_info.index[i].number, min_page);
    }

    if (min_page >= 0) {
	offset = 0;	/* plus symbol is hidden */
    } else {
	offset = 1;	/* offset for minus symbol */
	min_page = -min_page;
    }
    for (min_pageno_len = offset; min_page > 0;
	 min_page /= 10, min_pageno_len++);

    if (max_page >= 0) {
	offset = 0;	/* plus symbol is hidden */
    } else {
	offset = 1;	/* offset for minus symbol */
	max_page = -max_page;
    }
    for (max_pageno_len = offset; max_page > 0;
	 max_page /= 10, max_pageno_len++);

    return MAX(min_pageno_len, max_pageno_len);
    /* Plus 1 for minus symbol */
}

#if !defined(LESSTIF_VERSION)
/* draw or erase highlight marker at position x, y with widht w an height h,
   using suitable offsets (for sake of consistency of the latter, and because
   of the differences Xaw/Motif, this is a separate function).
*/
static void
refresh_highlight_marker(Widget widget, GC gc,
			 Position x, Position y, Dimension w, Dimension h)
{
    XDrawRectangle(XtDisplay(widget), XtWindow(widget), gc,
#ifdef MOTIF
		   x + 1, y + 1, w - 3, h - 3
#else
		   x + 2, y, w - 1, h - 1
#endif
		   );
}
#endif /* !defined(LESSTIF_VERSION) */

#ifndef MOTIF

/* update (redisplay) the list, saving the currently highlighted item.
 * This is invoked whenever marking the page, and the re-construction of the
 * entire list causes considerable flicker; but I guess that's unavoidable
 * with the current simplistic labelling scheme (with changing the list items
 * themselves). Gv does this considerably better (using custom widgets).
 */
static void
xaw_update_list(void)
{
    static int pagelist_width = -1;
    static int total_pages_bak = -1;

    XawListReturnStruct *ret;
    int idx, button_width;
    
    if (pagelist_width == -1 || total_pages != total_pages_bak) {
	pagelist_width = xaw_get_pagelist_size();
	total_pages_bak = total_pages;
    }
    
    /* save selected item */
    ret = XawListShowCurrent(LIST_WIDGET);
    idx = ret->list_index;
    button_width = get_panel_width() - 2 * (resource.btn_side_spacing + resource.btn_border_width);
    /* delete and re-create list */
    ASSERT(total_pages <= (int)page_info.index_size, "");
    XawListChange(LIST_WIDGET, page_info.page_labels, 0,
		  MAX(button_width, pagelist_width), False);
    /* restore selected item */
    if (idx != XAW_LIST_NONE) {
	XawListHighlight(LIST_WIDGET, idx);
    }
}

/*
  return height of a row in the list widget, and the initial offset of XtNinternalHeight
  in parameters row_height and internal_h
*/
static void
xaw_get_row_height(Widget w, Dimension *row_height, Dimension *internal_h)
{
    Dimension row_space;
    XFontStruct *font;
    Arg arglist[5];
    int i = 0;

    if (w == NULL || !XtIsRealized(w))
	return;

    XtSetArg(arglist[i], XtNfont, &font); ++i;
    XtSetArg(arglist[i], XtNrowSpacing, &row_space); ++i;
    XtSetArg(arglist[i], XtNinternalHeight, internal_h); i++;
    XtGetValues(w, arglist, i);
    *row_height = font->max_bounds.ascent + font->max_bounds.descent + row_space;
}

/*
 * Get pagelist width.
 */
int
xaw_get_pagelist_size(void)
{
    Widget w;
    XFontStruct *font;

    w = XtVaCreateWidget("list", listWidgetClass, globals.widgets.top_level, NULL);
    XtVaGetValues(w, XtNfont, &font, NULL);
    XtDestroyWidget(w);

    /* have space for max. pageno + space + current-marker,
       plus a few pixels for right margin */
    return (get_page_size() + 2) * get_avg_font_width(font) + 6;
}

/* auto-scroll pagelist when mouse-1 is down and moved above top or below bottom of
   page list.
*/
static void
xaw_drag_page_callback(Widget widget, XtPointer data, XEvent *event, Boolean *cont)
{
    int y, idx, actual_idx = 0;

    UNUSED(data);
    UNUSED(cont);
    
    if (event->xany.type == ButtonPress || ((event->xbutton.state & Button1Mask) == 0))
	return;
    if (event->xany.type == MotionNotify)
	y = (int)event->xmotion.y;
    else
	y = (int)event->xbutton.y;

    idx = get_item_index(widget, y);
    
    if (idx <= 0) {
	idx = 1;
    }
    else if (idx > total_pages) {
	idx = total_pages;
    }

    actual_idx = xaw_maybe_scroll_pagelist(idx, False, actual_idx);
    XawListHighlight(LIST_WIDGET, idx - MOTIF_IDX_OFFSET);

    /*      if (event->xany.type == ButtonRelease) { */
    /*  	fprintf(stderr, "1\n"); */
    /*  	page_history_insert(idx - MOTIF_IDX_OFFSET); */
    /*  	goto_page(idx - MOTIF_IDX_OFFSET, resource.keep_flag ? NULL : home); */
    /*  	search_signal_page_changed(); */
    /*      } */
}

static void
xaw_SendReportProc(Widget w, XtPointer closure, XtPointer call_data)
{
    XawPannerReport *rep = (XawPannerReport *) call_data;

    UNUSED(w);
    UNUSED(closure);
    
    view_y = rep->slider_y;
}

#endif /* not MOTIF */



/* returns the index of the current item in `Motif'-style, i.e. first item has index 1, not 0 */
static int
get_item_index(Widget w, int mouse_y)
{
#ifdef MOTIF
    return XmListYToPos(w, mouse_y);
#else
    Dimension row_height, internal_height;
    
    xaw_get_row_height(w, &row_height, &internal_height);
    return (mouse_y - internal_height) / row_height + MOTIF_IDX_OFFSET;
#endif
}

#if !defined(LESSTIF_VERSION)
/* idx is Motif-style index, i.e. 1 for 1st item, not 0 */
static Boolean
my_list_pos_to_bounds(Widget widget, int idx, Position *x, Position *y, Dimension *w, Dimension *h)
{
#ifdef MOTIF
    Position x1, y1;
    Dimension w1, h1;
    if (XmListPosToBounds(widget, idx, &x1, &y1, &w1, &h1)) {
	*x = x1;
	*y = y1;
	*w = w1;
	*h = h1;
	return True;
    }
    return False;
#else
    Dimension row_height, internal_height;
    /* FIXME: Remove this hard-coded offset! */
    const int X_OFFSET = 9;
    const int RULE_OFFSET = 2;

    if (idx <= 0 || idx > total_pages) {
	return False;
    }

    if (viewport != NULL && XtIsRealized(viewport))
	XtVaGetValues(viewport, XtNx, x, NULL);
    xaw_get_row_height(widget, &row_height, &internal_height);
    XtVaGetValues(widget, XtNwidth, w, NULL);
    
    *x -= X_OFFSET;
    *y = row_height * idx + internal_height - row_height - 1;
    *w -= RULE_OFFSET;
    *h = row_height + RULE_OFFSET;
    
    return True;
#endif
}


/* draw a hightlight rectangle around the page the mouse is currently over, to
   make e.g. marking easier.
*/
static void
highlight_page_callback(Widget widget, XtPointer data, XEvent *event, Boolean *cont)
{
    int curr_idx = get_item_index(widget, event->xmotion.y);
    Position x, y;
    Dimension w, h;
    static int idx_bak = -1;
    
    UNUSED(data);
    UNUSED(cont);

    switch(event->xany.type) {
    case ButtonPress:
    case ButtonRelease:
    case MotionNotify:
	/* might need to un-highlight previous one */
	if (idx_bak >= 0 && idx_bak != curr_idx
	    && my_list_pos_to_bounds(widget, idx_bak, &x, &y, &w, &h)) {
	    /* 	    fprintf(stderr, "index: %d, %d, h: %d, w: %d\n", x, y, h, w); */
	    refresh_highlight_marker(widget, m_page_gc.back, x, y, w, h);
#if REDRAW_CURRENT_MARKER_HACK
	    xaw_maybe_redraw_highlight(curr_idx);
#endif
	}
	idx_bak = curr_idx;
	/* redraw unless out of bounds (when pagelist is shorter than view area) */
	if (my_list_pos_to_bounds(widget, curr_idx, &x, &y, &w, &h)) {
	    refresh_highlight_marker(widget, m_page_gc.fore, x, y, w, h);
#if REDRAW_CURRENT_MARKER_HACK
	    g_current_highlighted = curr_idx;
#endif
	}
	break;
    case LeaveNotify:
	/* this might look overly complicated, but is neccessary to cover all
	   cases of 1-pixel movement up/down before leaving the list, or no
	   movement at all before leaving it. */
	if ((idx_bak >= 0 && idx_bak != curr_idx && my_list_pos_to_bounds(widget, idx_bak, &x, &y, &w, &h))
	    || my_list_pos_to_bounds(widget, curr_idx, &x, &y, &w, &h)) {
	    refresh_highlight_marker(widget, m_page_gc.back, x, y, w, h);
#if REDRAW_CURRENT_MARKER_HACK
	    xaw_maybe_redraw_highlight(curr_idx);
#endif
	}
	break;
    default:
	break;
    }
}
#endif /* !defined(LESSTIF_VERSION) */

/*
 * invoked on Button-1 Down.
 */
static void
select_page_callback(Widget w, XtPointer closure, XtPointer call_data)
{
#ifdef MOTIF
    XmListCallbackStruct *cbs = (XmListCallbackStruct *) call_data;
    int new = cbs->item_position;

    UNUSED(w);
    UNUSED(closure);

    maybe_scroll_pagelist(new - MOTIF_IDX_OFFSET, False);
    page_history_insert(new - MOTIF_IDX_OFFSET);
    goto_page(new - MOTIF_IDX_OFFSET, resource.keep_flag ? NULL : home, False);
#else
    XawListReturnStruct *item = (XawListReturnStruct *) call_data;
    int new = item->list_index;

    UNUSED(w);
    UNUSED(closure);

    if (globals.debug & DBG_EVENT)
	fprintf(stderr, "got: button-1 for `%d'\n", new);

#if 0
    fprintf(stderr, "select page: %d\n", new);
#endif
    maybe_scroll_pagelist(new, False);
    page_history_insert(new);
    goto_page(new, resource.keep_flag ? NULL : home, False);
    statusline_erase("Page history:");
#endif
    search_signal_page_changed();
}


static void
init_pagelabels(int start, int end)
{
    int i;
    char s[PAGENUMLEN];
#if 0
    fprintf(stderr, "===== init_pagelabels from %d to %d\n", start, end);
#endif
    ASSERT(end < (int)page_info.index_size, "");
    
    page_info.page_labels = xrealloc(page_info.page_labels, sizeof *(page_info.page_labels) * (end + 2));
    for (i = start; i < end; i++) {
	if (page_info.index[i].marked)
	    sprintf(s, "* %*d  ", get_page_size(),
		    resource.use_tex_pages ? page_info.index[i].number : i + 1);
	else
	    sprintf(s, "  %*d  ", get_page_size(),
		    resource.use_tex_pages ? page_info.index[i].number : i + 1);

	page_info.page_labels[i] = xstrdup(s);
    }
    page_info.page_labels[i] = NULL; /* terminate - important for creating the widget. */
}


#ifdef MOTIF


static int
xm_get_top_visible(int start)
{
    int top = start;
    while (top < total_pages && !XmListPosToBounds(LIST_WIDGET, top, NULL, NULL, NULL, NULL))
        top++;
    return top;
}

static int
xm_get_bottom_visible(int start)
{
    int bot = start;
    while (bot < total_pages && XmListPosToBounds(LIST_WIDGET, bot, NULL, NULL, NULL, NULL))
        bot++;
    bot--;
    return bot;
}

/* Scroll pagelist down or up if needed, and update top_visible and
   bot_visible.  List is always scrolled so that 1 element is still
   visible below or above pointer, to make it possible to flip through
   document by repeatedly clicking on first/last.
*/
static void
xm_maybe_scroll_pagelist(int current, saveCmdT curr_cmd, int *top_visible, int *bot_visible)
{
#if 0
    fprintf(stderr, "topmost visible: %d, bottom: %d, current: %d, total: %d\n",
	    top_visible, bottom_visible, current, total_pages); 
#endif

    if (current < *top_visible && curr_cmd != SCROLL_DOWN) {
	XmListSetPos(LIST_WIDGET,
		     current < 1
		     ? 1
		     : current);
	(*top_visible)--;
	(*bot_visible)--;
    }
    else if (current + MOTIF_IDX_OFFSET >= *bot_visible && curr_cmd != SCROLL_UP) {
	XmListSetBottomPos(LIST_WIDGET,
			   current + MOTIF_IDX_OFFSET >= total_pages
			   ? current + MOTIF_IDX_OFFSET
			   : current + MOTIF_IDX_OFFSET + 1);
	(*top_visible)++;
	(*bot_visible)++;
    }
}

static void
xm_set_page_labels(void)
{
    int i;
    char buf[PAGENUMLEN];

    XmString *motif_page_labels = xmalloc((total_pages + 2) * sizeof *motif_page_labels);
    
    for (i = 0; i < total_pages; ++i) {
	sprintf(buf, pageno_format, ' ', page_info.page_labels[i]);
	motif_page_labels[i] = XmStringCreateLocalized(buf);
    }
    XmListDeleteAllItems(LIST_WIDGET);
    XmListAddItems(LIST_WIDGET, motif_page_labels, total_pages, 0);

    XmListSelectPos(LIST_WIDGET, current_page + MOTIF_IDX_OFFSET, False);
    for (i = 0; i < total_pages; ++i) {
	XmStringFree(motif_page_labels[i]);
    }
    free(motif_page_labels);
}

static void
xm_toggle_label(Widget widget, int idx, Boolean update)
{
    /* TODO: use `update' to update all labels at once when toggling multiple */
    XmString str;
    char *mark_font, buf[128];

    if (widget == NULL)
	return;
    
    UNUSED(update);

    /*     ensure_labelinfo_size(idx); */
    ASSERT(idx < (int)page_info.index_size, "");
    if (!page_info.index[idx].marked) {
	sprintf(buf, pageno_format, '*', page_info.page_labels[idx]);
	mark_font = "MARKED";
	page_info.index[idx].marked = True;
    }
    else {
	sprintf(buf, pageno_format, ' ', page_info.page_labels[idx]);
	mark_font = "UNMARKED";
	page_info.index[idx].marked = False;
    }
    /*     str = XmStringCreateLocalized(buf); */
    str = XmStringCreateLtoR(buf, mark_font);
    XmListReplaceItemsPos(widget, &str, 1, idx + MOTIF_IDX_OFFSET);
    XmStringFree(str);
}


#else /* MOTIF */

static void
mark_page_callback(Widget w, XtPointer data, XEvent *event, Boolean *cont)
{
    UNUSED(data);
    UNUSED(cont);

    /* moving button2 generates MotionNotify events for button0 */
    if (event->type == MotionNotify || event->xbutton.button == Button2)
	internal_process_button2(w, event);

    if (event->type != ButtonPress)
	notify_print_dialog_have_marked();
}

void
xaw_create_pagelist_widgets(Dimension height, Dimension width, Position y, Widget parent)
{
    viewport = XtVaCreateWidget("viewport",
				viewportWidgetClass, parent,
				XtNallowVert, True,
				/* this is not related to the scroll bar: */
				/* XtNforceBars, True, */
				XtNx, resource.btn_side_spacing,
				XtNy, y,
				XtNheight, height,
				XtNwidth, width,
				NULL);
    LIST_WIDGET = XtVaCreateWidget("list",
				   listWidgetClass, viewport,
				   XtNlist, page_info.page_labels,
				   XtNdefaultColumns, 1,
				   XtNforceColumns, True,
				   XtNx, 10,
				   XtNy, 10,
				   XtNheight, height,
				   XtNwidth, width - 10,
				   XtNlongest, LONGESTPAGENUM,
				   XtNverticalList, True,
				   NULL);
    XtManageChild(LIST_WIDGET);
    XtManageChild(viewport);
    XtAddCallback(LIST_WIDGET, XtNcallback, select_page_callback,
		  (XtPointer) NULL);
    /* for scrolling the list */
    XtAddCallback(viewport, XtNreportCallback, xaw_SendReportProc,
		  (XtPointer) NULL);
    XtAddEventHandler(LIST_WIDGET,
		      ButtonPressMask | ButtonReleaseMask | Button2MotionMask,
		      False, mark_page_callback, (XtPointer)NULL);

    if (resource.pagelist_highlight_current)
	XtAddEventHandler(LIST_WIDGET,
			  ButtonPressMask | ButtonReleaseMask | PointerMotionMask | LeaveWindowMask,
			  False, highlight_page_callback, (XtPointer)NULL);

    
    {
	Widget y_bar;
	XtTranslations xlats = XtParseTranslationTable(
	  "<Btn4Down>,<Btn4Up>: scroll-list-up()\n"
	  "<Btn5Down>,<Btn5Up>: scroll-list-down()\n");

	XtOverrideTranslations(LIST_WIDGET, xlats);

	y_bar = XtNameToWidget(viewport, "vertical");
	if (y_bar != NULL)
	    XtOverrideTranslations(y_bar, xlats);
    }

    XtAddEventHandler(LIST_WIDGET,
		      /* FIXME: We should add PointerMotionMask here, but handling PointerMotionMask
			 currently doesn't work with the Xaw list widget: the auto-scrolling code doesn't
			 realize when the mouse direction of the pointer movement changes, and continues
			 to scroll into the same direction. This will be rather annoying for users, so
			 we disabled PointerMotionMask for the time being.
		      */
		      ButtonReleaseMask /* | PointerMotionMask */ | Button1MotionMask,
		      False, xaw_drag_page_callback, (XtPointer)NULL);
}

static void
xaw_toggle_label(Widget w, int idx, Boolean update)
{
    if (w == NULL)
	return;

    /*     ensure_labelinfo_size(idx); */
    ASSERT(idx < (int)page_info.index_size, "");
    if (!page_info.index[idx].marked) {
	/* 	sprintf(toc[idx], "* %*d  ", get_page_size(), page_index[idx].number);  */
	sprintf(page_info.page_labels[idx], pageno_format, '*', get_page_size(),
		resource.use_tex_pages ? page_info.index[idx].number : idx + 1);
	page_info.index[idx].marked = True;
    }
    else {
	sprintf(page_info.page_labels[idx], pageno_format, ' ', get_page_size(),
		resource.use_tex_pages ? page_info.index[idx].number : idx + 1);
	/* 	sprintf(toc[idx], "  %*d  ", get_page_size(), page_index[idx].number);  */
	page_info.index[idx].marked = False;
    }
    
    if (update)
	xaw_update_list();
}

static int
xaw_maybe_scroll_pagelist(int new_page, Boolean force_recenter, int idx_bak)
{
    Position x;
    Position y, new_y, bot_y;
    Dimension view_height, row_height, internal_height;
    /* Dimension clip_height; */
    /* static Widget list_clip = 0; */
    
    if (LIST_WIDGET == NULL || (resource.expert_mode & XPRT_SHOW_BUTTONS) == 0)
	return idx_bak;

    /*     if (list_clip == 0) { */
    /* 	list_clip = XtNameToWidget(viewport, "clip"); */
    /*     } */
    /*     if (XtIsRealized(list_clip)) */
    /* 	XtVaGetValues(list_clip, XtNheight, &clip_height, XtNx, &cx, XtNy, &y1, NULL); */
    if (viewport != NULL && XtIsRealized(viewport))
	XtVaGetValues(viewport, XtNheight, &view_height, XtNx, &x, NULL);
    /*     fprintf(stderr, "diff: %d, %d, %d, %d, %d\n", cx - x, clip_height, view_height, y1, (int)y2); */
    xaw_get_row_height(LIST_WIDGET, &row_height, &internal_height);
    y = row_height * new_page;

#if DEBUG
    fprintf(stderr, "###### xaw_maybe_scroll_pagelist: y %d, view_y %d, view_height %d, row_height %d, internal_height %d; actual %d\n",
	    y, view_y, view_height, row_height, internal_height, idx_bak);
#endif
    /*FIXME: when page list is destroyed, view_y will be 0 until user scrolls page list */
    bot_y = view_y + view_height;
    if (force_recenter || ((y >= bot_y - row_height))) {
#if DEBUG
	fprintf(stderr, "scrolled below bottom; incrementing %d to %d\n", view_y, view_y + row_height);
#endif
	y += row_height;
	XawViewportSetCoordinates(viewport, x, y - view_height > 0 ? y - view_height : 0);
	return new_page + 1;
    }
    else if (force_recenter || ((y <= view_y + row_height + internal_height))) {
#if DEBUG
	fprintf(stderr, "scrolled over top; new_y: %d\n", y - row_height);
#endif
	new_y = y - 2 * row_height;
	XawViewportSetCoordinates(viewport, x, new_y);
	return new_page - 1;
    }
    /* not scrolled */
    return -2;
}

#endif /* MOTIF */


/* idx is C-style index (0-based), not Motif one (1-based) */
static void
toggle_label(Widget widget, int idx, Boolean update)
{
    if (idx >= total_pages)
	return;
    ASSERT(idx < total_pages, "");
    ASSERT(idx >= 0, "");
#ifdef MOTIF
    xm_toggle_label(widget, idx, update);
#else
    xaw_toggle_label(widget, idx, update);
#endif
}


void
list_toggle_marks(int arg)
{
    int i;

    if (arg < 0) { /* mark all */
	for (i = 0; i < total_pages; i++) {
	    ASSERT(i < (int)page_info.index_size, "");
	    /* 	    ensure_labelinfo_size(i); */
	    if (!page_info.index[i].marked) {
		toggle_label(LIST_WIDGET, i, False);
	    }
	}
    }
    else if (arg == 0) { /* unmark all */
	for (i = 0; i < total_pages; i++) {
	    ASSERT(i < (int)page_info.index_size, "");
	    /* 	    ensure_labelinfo_size(i); */
	    if (page_info.index[i].marked) {
		toggle_label(LIST_WIDGET, i, False);
	    }
	}
    }
    else { /* toggle odd/even */
	if (arg == 2) /* toggle even */
	    arg = 0;
	for (i = 0; i < total_pages; i++) {
	    if ((i + 1) % 2 == arg) {
		toggle_label(LIST_WIDGET, i, False);
	    }
	}
    }
    /* TODO: update widget once for Motif as well! */
#ifndef MOTIF
    xaw_update_list();
#endif
    notify_print_dialog_have_marked();
}

static Boolean PagelistInitialized = False;

#ifndef MOTIF
void
handle_pagelist_resize(void)
{
    /* TODO: the following will mess up the geometry of the list
       (doesn't increase height, and incrementally decreases width):
       if (list_widget) {
       Dimension height;
       --- without the (un)manage, I get an X Error:
       XtMakeGeometryRequest - parent has no geometry manager
       ---
       XtUnmanageChild(viewport);
       XtUnmanageChild(LIST_WIDGET);
       XtVaGetValues(globals.widgets.clip_widget, XtNheight, &height, NULL);
       height -= resource.btn_top_spacing + resource.btn_border_width + global_y_pos;
       XtVaSetValues(viewport, XtNheight, height, NULL);
       XtManageChild(LIST_WIDGET);
       XtManageChild(viewport);
       }
       ... so we use brute force instead: */
    handle_destroy_pagelist(LIST_WIDGET, NULL, NULL);
    create_pagelist();
}

void
handle_destroy_pagelist(Widget w, XtPointer client_data, XtPointer call_data)
{
    UNUSED(w);
    UNUSED(client_data);
    UNUSED(call_data);
    
    if (viewport != NULL) {
	XtDestroyWidget(viewport);
	viewport = NULL;
	LIST_WIDGET = NULL;
    }
    PagelistInitialized = False;
}
#endif /* MOTIF */

void
create_pagelist(void)
{
    Pixel background, foreground;
#ifdef MOTIF
    
    /*     items = xrealloc(items, sizeof *items * (total_pages + 2)); */
    init_pagelabels(0, total_pages);
    xm_set_page_labels();
    if (!PagelistInitialized) {
	XtAppContext app;

	XtVaGetValues(LIST_WIDGET, XmNforeground, &foreground, XmNbackground, &background, NULL);
	m_page_gc.back = set_or_make_gc(NULL, GXcopy, background, foreground);
	m_page_gc.fore = set_or_make_gc(NULL, GXcopy, foreground, background);
	XtManageChild(LIST_WIDGET);

	XtAddCallback(LIST_WIDGET, XmNbrowseSelectionCallback, select_page_callback, NULL);
#if !defined(LESSTIF_VERSION)
	/*
	  Don't use the highlighting hack with LessTif, since its XmListPosToBounds()
	  is too broken to be usable (as of 0.93.36):
	  - it returns generally too low values, apparently it doesn't take
	  XmNlistSpacing into account;
	  - it doesn't take scrollbar position into account.
	*/
	if (resource.pagelist_highlight_current)
	    XtAddEventHandler(LIST_WIDGET,
			      ButtonPressMask | ButtonReleaseMask | PointerMotionMask | LeaveWindowMask,
			      False, highlight_page_callback, (XtPointer)NULL);
#endif /* !defined(LESSTIF_VERSION) */
	app = XtWidgetToApplicationContext(globals.widgets.top_level);
	XtAppAddActions(app, CustomListActions, XtNumber(CustomListActions));
	XtOverrideTranslations(LIST_WIDGET, XtParseTranslationTable(motif_custom_translations));
	PagelistInitialized = True;
    }
#else /* MOTIF */
    if ((resource.expert_mode & XPRT_SHOW_BUTTONS) == 0) {
	PagelistInitialized = False; /* might need to re-create widgets in this case */
	return;
    }

    if (globals.debug & DBG_GUI)
	fprintf(stderr, "allocating list with %d pages\n", total_pages);

    init_pagelabels(0, total_pages);
    if (!PagelistInitialized) {
	xaw_create_pagelist();
	XtVaGetValues(LIST_WIDGET, XtNforeground, &foreground, XtNbackground, &background, NULL);
	m_page_gc.back = set_or_make_gc(NULL, GXcopy, background, foreground);
	m_page_gc.fore = set_or_make_gc(NULL, GXcopy, foreground, background);
	PagelistInitialized = True;
    }
#endif /* MOTIF */
    /* scroll to the current page if needed */
    maybe_scroll_pagelist(current_page, False);
}

#ifndef MOTIF
static void
free_pagelabels(void)
{
    int i;
    for (i = 0; page_info.page_labels != NULL && page_info.page_labels[i] != NULL; i++) {
	free(page_info.page_labels[i]);
    }
    free(page_info.page_labels);
    page_info.page_labels = NULL;
}
#endif /* not MOTIF */


void
refresh_pagelist(int newsize, int newpage)
{
    if (
#ifndef MOTIF
	(resource.expert_mode & XPRT_SHOW_BUTTONS) == 0 ||
#endif
	!XtIsRealized(globals.widgets.top_level))
	return;

#ifdef DEBUG
    fprintf(stderr, "=== refresh_pagelist: newsize %d, newpage %d\n", newsize, newpage);
#endif
#ifdef MOTIF
    /*     items = xrealloc(items, sizeof *items * (newsize + 2)); */
    init_pagelabels(0, newsize);
    xm_set_page_labels();
#else /* MOTIF */
    if ((resource.expert_mode & XPRT_SHOW_BUTTONS) == 0)
	return;

    /* FIXME - is this really neccessary?? The alternative:
       XawListChange(LIST_WIDGET, page_info.page_labels, newsize, 0, True);
       has problems when freeing the page labels afterwards.
    */
    handle_destroy_pagelist(LIST_WIDGET, NULL, NULL);

    free_pagelabels();
    init_pagelabels(0, newsize);

    xaw_create_pagelist();
#endif /* MOTIF */
    /* `True' since the pagelist is newly created */
    maybe_scroll_pagelist(newpage, True);
}

void
maybe_scroll_pagelist(int newpage, Boolean force_recenter)
{
#ifdef MOTIF
    int top_visible, bot_visible;
    UNUSED(force_recenter);
#endif

    if (
#ifndef MOTIF
	(resource.expert_mode & XPRT_SHOW_BUTTONS) == 0 ||
#endif
	!XtIsRealized(globals.widgets.top_level))
	return;

#ifdef MOTIF
    XmListSelectPos(LIST_WIDGET, newpage + MOTIF_IDX_OFFSET, False);

    top_visible = xm_get_top_visible(1);
    bot_visible = xm_get_bottom_visible(top_visible);

    xm_maybe_scroll_pagelist(newpage, CLICK, &top_visible, &bot_visible);
#if HAVE_XPM
    tb_check_navigation_sensitivity(current_page);
#endif
#else

    if (LIST_WIDGET == NULL)
	return;
    
    (void)xaw_maybe_scroll_pagelist(newpage + 1, force_recenter, 0);

    XawListHighlight(LIST_WIDGET, newpage);
#if REDRAW_CURRENT_MARKER_HACK
    /* if the XawListHighlight happens adjacent to the page that was
       last highlighted with our home-made `current selected'
       rectangle, it might overdraw that rectangle. In this case,
       restore it:
    */
    xaw_maybe_redraw_current_marker(newpage);
#endif
#endif
}

#ifdef MOTIF
static void
set_all_marks(int from, int to)
{
    int i;
    for (i = from; i < to; i++) {
	/* 	ensure_labelinfo_size(i); */
	page_info.index[i].marked = True;
	toggle_label(LIST_WIDGET, i, False);
    }
}

static void
internal_process_button2_drag(Widget widget, XEvent *event)
{
    int i, idx, min = total_pages, max = 0;
    idx = get_item_index(widget, event->xbutton.y);

    for (i = 0; i < total_pages; i++) {
	/* 	ensure_labelinfo_size(i); */
	if (page_info.index[i].marked && i > max)
	    max = i;
    }
    for (i = total_pages; i > 0; i--) {
	/* 	ensure_labelinfo_size(i); */
	if (page_info.index[i].marked && i < min)
	    min = i;
    }

    if (min == total_pages)
	min = 0;
    if (max == 0)
	max = total_pages;

    if (idx < min) {
	set_all_marks(idx, min);
    }
    else if (idx > max) {
	set_all_marks(max + 1, idx);
    }
    else {
	set_all_marks(0, idx);
    }
}
#endif /* MOTIF */

static void
internal_process_button2(Widget widget, XEvent *event)
{
    int curr_idx;
    static int prev_idx = 0;
#ifndef MOTIF
    static int actual_idx = -2;
#endif
    static int top_visible = 0, bot_visible = 0;
    static int prev_y = 0, curr_y = 0;
    static saveCmdT prev_cmd = 0;	/* previous command (CLICK/SCROLL_UP/SCROLL_DOWN) */
    static saveCmdT curr_cmd = 0;	/* current command (CLICK/SCROLL_UP/SCROLL_DOWN) */
    static saveCmdT bak_cmd = 0;	/* last command that started inside the pagelist (CLICK/SCROLL_UP/SCROLL_DOWN) */
    static Boolean change_scroll_direction = False;
    
    switch(event->xany.type) {
    case ButtonPress:
	prev_y = event->xbutton.y;
	prev_idx = curr_idx = get_item_index(widget, prev_y);
#ifdef MOTIF
	top_visible = xm_get_top_visible(1);
	bot_visible = xm_get_bottom_visible(top_visible);
#endif
	toggle_label(widget, curr_idx - 1, True);
	prev_cmd = CLICK;
#ifdef MOTIF
	xm_maybe_scroll_pagelist(curr_idx - 1, CLICK, &top_visible, &bot_visible);
#else
	actual_idx = xaw_maybe_scroll_pagelist(curr_idx, False, actual_idx);
#endif
	break;
    case ButtonRelease:
	prev_cmd = CLICK;
	break;
    case MotionNotify:
	curr_y = (int)event->xmotion.y;
	curr_idx = get_item_index(widget, event->xmotion.y);
#ifndef MOTIF
	if (actual_idx > 0) {
	    curr_idx = actual_idx;
	}
#endif

	if (curr_y < prev_y)
	    curr_cmd = SCROLL_UP;
	else if (curr_y > prev_y)
	    curr_cmd = SCROLL_DOWN;
	prev_y = curr_y;

	if (prev_cmd != CLICK && curr_cmd != prev_cmd)
	    change_scroll_direction = True;

	if ((curr_idx != prev_idx && !change_scroll_direction)
	    || (change_scroll_direction
		/* last or first visible are always spared, unless they are really
		   the first or last page; this way, always 1 more page is visible
		   than is currently marked/selected
		*/
		&& !(curr_idx == top_visible) && !(curr_idx == bot_visible))) {
	    if (curr_idx <= 0) {
		/* When user has scrolled off, mark this by setting curr_idx to 1 more or less
		   than the pagelist has so that the last/first page don't oscillate between
		   marked/unmarked state when user continues to scroll.
		   Also, we continue scrolling as long as user drags in the same direction as
		   the last `real' scrolling event (saved as bak_cmd).
		*/
		if (curr_cmd == SCROLL_DOWN && bak_cmd == SCROLL_DOWN && prev_idx <= total_pages) {
		    curr_idx = prev_idx + 1;
		}
		else if (curr_cmd == SCROLL_UP && bak_cmd == SCROLL_UP && prev_idx > 0) {
		    curr_idx = prev_idx - 1;
		}
	    }


	    if (curr_idx > 0 && curr_idx <= total_pages) {
		toggle_label(widget, curr_idx - 1, True);
#ifdef MOTIF
		xm_maybe_scroll_pagelist(curr_idx - 1, curr_cmd, &top_visible, &bot_visible);
#else
		actual_idx = xaw_maybe_scroll_pagelist(curr_idx, False, actual_idx);
#endif
		prev_idx = curr_idx;
		bak_cmd = curr_cmd;
		change_scroll_direction = False;
	    }
	    else {
#ifndef MOTIF
		if (curr_idx > total_pages)
		    actual_idx = -2;
		else
		    actual_idx = xaw_maybe_scroll_pagelist(curr_idx, False, actual_idx);
#endif
	    }
	    prev_cmd = curr_cmd;
	}
	break;
    default:
	break;
    }
}

void
list_toggle_current(int arg)
{
    toggle_label(LIST_WIDGET, arg, True);

    notify_print_dialog_have_marked();
}

#ifdef MOTIF

static void
xm_list_set_mark(Widget widget,
		 XEvent *event,
		 String *params,
		 Cardinal *num_params)
{
    UNUSED(params);
    
    if ((*num_params != 1) || !XmIsList(widget))
	return;

    internal_process_button2(widget, event);

    if (event->type != ButtonPress)
	notify_print_dialog_have_marked();
}

static void
xm_list_drag_mark(Widget widget,
		  XEvent *event,
		  String *params,
		  Cardinal *num_params)
{
    UNUSED(params);
    
    if ((*num_params != 1) || !XmIsList(widget))
	return;

    internal_process_button2_drag(widget, event);

    if (event->type != ButtonPress)
	notify_print_dialog_have_marked();
}

#endif /* MOTIF */
