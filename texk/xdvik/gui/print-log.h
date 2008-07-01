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

struct save_or_print_info *info; /* forward declaration */

/* printlog access functions */
extern void printlog_create(struct save_or_print_info *info,
			    const char *title,
			    const char *close_label);
extern Boolean printlog_raise_active(struct save_or_print_info *info);
extern void printlog_popup(struct save_or_print_info *info);
extern void printlog_reset(struct save_or_print_info *info);
extern void printlog_append(struct save_or_print_info *info, const char *str, size_t len);
extern void printlog_append_str(struct save_or_print_info *info, const char *str);
extern void printlog_enable_closebutton(struct save_or_print_info *info);
extern void printlog_enable_cancelbutton(struct save_or_print_info *info);
extern void printlog_popdown(struct save_or_print_info *info, Boolean override_timer);

#endif /* PRINT_LOG_H_ */
