#ifndef STATUSLINE_H_
#define STATUSLINE_H_
/*
 * Copyright (c) 2001-2004 the xdvik development team
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

#define STAT_BUF_LEN 512


/* statusline stuff. The following function and #defines are also
 * needed when compiling without statusline support.
 */
typedef enum statusTimerT_ {
    STATUS_FOREVER = 0,
    STATUS_VERYSHORT = 1,
    STATUS_SHORT = 5,
    STATUS_MEDIUM = 10,
    STATUS_LONG = 30
} statusTimerT;

extern int get_statusline_height(void);
extern void statusline_clear(void);
extern void statusline_erase(const char *pattern);
extern void statusline_info(statusTimerT timeout, const char *fmt, ...);
extern void statusline_error(statusTimerT timeout, const char *fmt, ...);
extern void statusline_append(statusTimerT timeout, const char *pattern, const char *fmt, ...);
#ifdef MOTIF
extern Widget create_statusline(Widget parent);
#else
extern Widget create_statusline(void);
#endif
extern void handle_statusline_resize(void);

extern void force_statusline_update(void);
extern void toggle_statusline(void);

#endif /* STATUSLINE_H_ */
