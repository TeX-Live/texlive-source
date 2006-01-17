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

#ifndef PRINT_DIALOG_H_
#define PRINT_DIALOG_H_

#define Xdvi_SAVE_SHELL_NAME	"save_popup"
#define Xdvi_PRINT_SHELL_NAME	"print_popup"

typedef enum { FILE_PRINT = 0, FILE_SAVE = 1, FILE_ALL = 2 } printOrSaveActionT;

struct save_or_print_info {
    printOrSaveActionT act;
    Widget message_popup;
    struct select_pages_info *pinfo;
};

extern void save_or_print_callback(struct save_or_print_info *info);
extern void cancel_saving(struct save_or_print_info *info);
extern const char *get_printer_options(void);
extern char *get_dvips_options(printOrSaveActionT act);

extern void notify_print_dialog_have_marked(Boolean flag);

#endif /* PRINT_DIALOG_H_ */
