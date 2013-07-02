/*
 * Copyright (c) 2003-2004 the xdvik development team
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

#ifndef DVI_SEARCH_H_
#define DVI_SEARCH_H_

#include "xdvi.h"

#include "search-dialog.h"
#include "dvisel.h"

/* Widget names that are used in callbacks */
#define Xdvi_SEARCHBOX_INPUT_NAME "searchbox_input"
#define Xdvi_SEARCH_POPUP_NAME "find_popup"

/* A sorted dynamic array that maps text buffer (i.e. word)
 * positions to DVI file offsets.
 */

struct pos_info {
    size_t buffer_pos;	/* position in text buffer */
    struct bbox bbox;	/* bounding box of this word */
};

struct page_mapping {
    int pageno;
    int offset;
};

struct word_info {
    char *txt_buf;	 /* contains scanned text as whitespace separated strings */
    size_t txt_buf_size; /* size of above buffer */
    size_t curr_buf_idx;	 /* current position in buffer */
    struct bbox *bboxes;	/* dynamic array of position infos */
    size_t bboxes_size;	/* size of above array */
    size_t bboxes_idx;	 /* current position in buffer */
    const struct page_mapping *page_mapping;		/* list of page offsets, for bbox_pass */
    struct search_settings *settings;	/* pointer to search settings, for bbox_pass */
    int buffer_offset;	/* offset to start of buffer from text on previous pages */
    Boolean bbox_pass;		/* whether we're scanning for bounding boxes of match */
    Boolean search_scan_pass;	/* whether we're scanning for text in string search */
    Boolean text_selection_pass; /* whether we're scanning for text selection */
};

struct save_or_print_info; /* forward declaration */

extern void isearch_start(void);
extern void search_dvi(XtPointer settings);
extern void search_restart(XtPointer settings);
extern Boolean search_extract_text(struct save_or_print_info *info);
extern Boolean search_have_match(int pageno);
extern int search_inside_bbox_match(int x, int y);
extern void search_draw_inverted_regions(void);
extern void search_signal_page_changed(void);
extern void search_reset_info(void);
extern void search_erase_highlighting(Boolean flag);
extern void search_putback_expose(void);
extern char *get_text_selection(int *len, int x, int y, int w, int h);

#if 0
#define TEST_DELAY(s)				\
    do {					\
	TRACE_FIND((stderr, s));		\
	int i, j;				\
	XSync(DISP, False);			\
	for (i = 0; i < 10000; i++)		\
	    for (j = 0; j < 20000; j++);	\
    } while (0)
#else
#define TEST_DELAY(s) /* as nothing */
#endif

#endif /* DVI_SEARCH_H_ */

