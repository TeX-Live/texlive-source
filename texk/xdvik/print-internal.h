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
#include "print-dialog.h"

typedef enum { FMT_PS, FMT_PS2PDF, FMT_DVI, FMT_ISO_8859_1, FMT_UTF8, FMT_NONE } outputFormatT;

/* file IO help struct */
struct file_IO {
    FILE *fp;
    char *fname;
};

/* collection of all file IOs used in printing/saving */
struct file_info {
    struct file_IO dvi_in;
    struct file_IO dvi_tmp;
    struct file_IO dvi_out;
    struct file_IO ps_out;
    struct file_IO pdf_out;
    struct file_IO txt_out;
};

/* stacks for communication with selection routines */
struct specials_stack_elem {
    char *content;		/* string content */
};

struct specials_stack {
    size_t stack_len;
    struct specials_stack_elem *items;
};


/* wrapper struct for all kinds of information about selected pages. */
struct select_pages_info {
    printOrSaveActionT act;	/* whether we're printing or saving */
    int from;			/* lower bound of page range to be selected */
    int to;			/* upper bound of page range to be selected */
    struct file_info *finfo;	/* additional file info pointer */
    /* callback function that says whether a page should be selected or not;
       will be passed a pointer to the current struct select_pages_info,
       and the current page */
    Boolean (*callback)(struct select_pages_info *info, int page);
    struct specials_stack warn_files;	/* collect warnings about included files */
    dviErrFlagT errflag;	/* collect all kinds of errors that can happen while copying */
};

extern void internal_print(struct select_pages_info *pinfo);
extern void internal_save(struct select_pages_info *pinfo,
			  outputFormatT output_format);

#endif /* PRINT_INTERNAL_H_ */

