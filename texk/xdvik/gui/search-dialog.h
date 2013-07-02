/*
 * Copyright (c) 2003-2004 Stefan Ulrich
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

#ifndef SEARCH_DIALOG_H_
#define SEARCH_DIALOG_H_

#include "xdvi.h"

typedef enum { SEARCH_UP = 0, SEARCH_DOWN, SEARCH_UNINITIALIZED } searchDirectionT;

/* bounding box info for words. */
struct bbox {
    int ulx, uly, lrx, lry;
};

struct search_info {
    Boolean have_match;
    Boolean locked;	/* to block multiple searches */
    Boolean search_wrapped;	/* if restarting from start (or end) */
    int from_pos;
    int to_pos;
    int scan_page;
    int page_offset;	/* start of current scan */
};


struct search_settings {
    const char *term; /* original search term */
    char *utf8_term;  /* term in UTF-8 encoding */
    char *posix_term; /* utf8_term with Perl abbreviations mapped to POSIX */
    Boolean use_regexp;
    Boolean case_sensitive;
    Boolean ignore_hyphens;
    Boolean ignore_linebreaks;
    Boolean wrap;
    int wrapcnt;	/* > 0 if search has wrapped */
    char *isearchterm;  /* non-NULL when doing isearch */
    searchDirectionT direction;
    struct search_info *searchinfo;
    /* internal state management */
    int x_pos, y_pos;	/* position of the search popup window */
    int from_page;	/* page to start search from */
    int curr_page;	/* current page of search */
    int to_page;	/* highest page already scanned */
    int hyphen_delta;	/* difference in offsets when hyphenation is removed */
    Widget message_window; /* warning popup, or NULL if it doesn't exist */
};

extern void dvi_find_string(const char *str, Boolean find_next);

#endif /* SEARCH_DIALOG_H_ */
