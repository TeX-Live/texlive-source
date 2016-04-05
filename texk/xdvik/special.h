/*
 * Copyright (c) 2002-2016 the xdvik development team
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

#ifndef SPECIAL_H_
#define SPECIAL_H_

#include "xdvi.h"
#include "events.h"

extern Boolean scan_special(char *str, int str_len, void *data);

#if PS

struct psprocs {
    void (*toggle)	(int flag);
    void (*destroy)	(void);
    void (*interrupt)	(void);
    void (*endpage)	(void);
    void (*drawbegin)	(int, int, const char *);
    void (*drawraw)	(const char *);
    void (*drawfile)	(const char *, FILE *);
    void (*drawend)	(const char *);
    void (*beginheader)	(void);
    void (*endheader)	(void);
    void (*newdoc)	(void);
};

extern void ps_destroy(void);
extern void ps_destroy_nofree(void);
extern void drawbegin_none(int, int, const char *);
extern void draw_bbox(void);
extern void display_bboxes(void);
extern void clear_bboxes(void);
extern void save_bbox(void);

#endif

extern void applicationDoSpecial(char *str, size_t len);
extern void geom_do_special(struct scan_info *, char *, double);
extern void color_special(const char *colorspec);

extern void init_prescan(void);

/*
  info on whether this DVI file contains papersize specials,
  and a resetting method.
*/
extern void reset_papersize_special(void);
extern Boolean have_papersize_special(void);

#if COLOR
extern void init_page_colors(struct rgb *foreground, struct rgb *background);
extern void scan_color_eop(void);
extern void scan_color(const char *color);
struct rgb; /* forward declaration */
extern void set_fg_color(const struct rgb *);
#endif

#endif /* SPECIAL_H_ */
