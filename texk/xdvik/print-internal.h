/*
 * Copyright (c) 2002-2004 Paul Vojta and the xdvik development team
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
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL ANY AUTHO OF THIS SOFTWARE BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef PRINT_INTERNAL_H_
#define PRINT_INTERNAL_H_

#include "dvi-init.h"

typedef enum { FMT_PS, FMT_PS2PDF, FMT_DVI, FMT_ISO_8859_1, FMT_UTF8, FMT_NONE } outputFormatT;

/* collection of all file IOs used in printing/saving */
struct file_info {
    char *tmp_dvi_file;		/* temporary DVI file */
    FILE *tmp_dvi_fp;		/* FILE * for temporary DVI file */
    char *tmp_ps_file;		/* temporary PS file */
    char *out_file;		/* final output file */
    /*      FILE *out_fp; */		/* FILE * for final output file */
    FILE *in_fp;		/* FILE * for input file */
};

/* stacks for communication with selection routines */
struct specials_stack_elem {
    char *content;		/* string content */
};

struct specials_stack {
    size_t stack_len;
    struct specials_stack_elem *items;
};

typedef enum printRadioT_ {
    NO_PRINT_VAL = -1,
    TO_PRINTER = 1,
    TO_FILE
} printRadioT;


typedef enum pageRadioT_ {
    NO_PAGE_VAL = -1,
    PAGE_ALL = 1,
    PAGE_MARKED,
    PAGE_RANGE
} pageRadioT;

struct save_or_print_info; /* forward declaration */

/* wrapper struct which stores information about values the user has
   selected in the dialog.
*/
struct select_pages_info {
    int from;			/* lower bound of page range to be selected */
    int to;			/* upper bound of page range to be selected */
/*      struct file_info *finfo;	\/\* additional file info pointer \*\/ */
    /* callback function that says whether a page should be selected or not;
       will be passed a pointer to the current struct save_or_print_info,
       and the current page */
    Boolean (*callback)(struct save_or_print_info *info, int page);
    struct specials_stack warn_files;	/* collect warnings about included files */
    dviErrFlagT errflag;	/* collect all kinds of errors that can happen while copying */
};

typedef enum { FILE_PRINT = 0, FILE_SAVE = 1 } printOrSaveActionT;


struct callback_info {
    XtCallbackProc cb_close;
    XtCallbackProc cb_cancel;
    XtCallbackProc cb_destroy;
    XtCallbackProc cb_keep;
};

struct save_or_print_info {
    printOrSaveActionT act;	/* whether we're printing or saving */
    outputFormatT fmt;		/* DVI/PS/... */
    printRadioT print_target;	/* to printer or to file */
    pageRadioT page_selection;	/* which pages: all/marked/range */
    char *printer_options;	/* printer name + options from text field */
    char *dvips_options;	/* dvips options from text field */
    Widget shell;
    Widget message_popup;
    Widget printlog;
    struct select_pages_info *pinfo;
    struct callback_info *callbacks;
    struct file_info *finfo;
};

extern void internal_print(struct save_or_print_info *info);
extern void internal_save(struct save_or_print_info *info);

#endif /* PRINT_INTERNAL_H_ */

