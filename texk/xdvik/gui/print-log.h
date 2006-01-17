/*
 * Copyright (c) 2002-2004  Paul Vojta and the xdvik development team
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

#ifndef PRINT_LOG_H_
#define PRINT_LOG_H_

/* printlog access functions */

extern void printlog_create(const char *title,
			    const char *close_label,
			    /* callback for `close' button */
			    void (*close_callback)(Widget w, XtPointer client_data, XtPointer call_data),
			    /* callback for `cancel' button */
			    void (*cancel_callback)(Widget w, XtPointer client_data, XtPointer call_data),
			    /* callback for destroying window via window manager */
			    void (*destroy_callback)(Widget w, XtPointer client_data, XtPointer call_data),
			    /* callback for `close automatically' checkbox */
			    void (*unkeep_callback)(Widget w, XtPointer client_data, XtPointer call_data));
extern Boolean printlog_raise_active(void);
extern void printlog_popup(void);
extern void printlog_reset(void);
extern void printlog_append(const char *str, size_t len);
extern void printlog_append_str(const char *str);
extern void printlog_enable_closebutton(void);
extern void printlog_popdown(Boolean override_timer);

#endif /* PRINT_LOG_H_ */
