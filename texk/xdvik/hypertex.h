/*
 * Copyright (c) 2002-2004 the xdvik development team
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

#ifndef HYPERTEX_H_
#define HYPERTEX_H_

/* fallbacks if parsing user-specified colors fails */
#define LINK_COLOR_FALLBACK "BLUE2"
#define VISITED_LINK_COLOR_FALLBACK "Purple4"

extern const char *is_local_file(const char *filename);
extern void launch_program(const char *filename);
extern void launch_xdvi(const char *filename, const char *anchor_name);
extern Boolean htex_handleref(int x, int y, Boolean newwindow);
extern void htex_reinit(void);
extern void htex_resize_page(void);
extern void htex_draw_anchormarkers(void);
extern void htex_record_position(int, int, int, int);

extern void htex_initpage(Boolean dvi_file_changed, Boolean size_changed, int pageno);

typedef enum { HTEX_ANCHOR_NUM, HTEX_ANCHOR_STRING } htexPrescanTypeT;

struct htex_prescan_data {
    int pageno;
    int anchor_num;
    htexPrescanTypeT scan_type;
};
    
typedef enum { HTEX_TEXT, HTEX_IMG } htexObjectT;

/* prescanning stuff */
extern Boolean htex_prescan_special(const char *str, int str_len, struct htex_prescan_data *data);
extern int htex_prescan_get_depth(void);
extern size_t htex_prescan_get_mismatched_anchor_num(size_t depth);
extern void htex_prescan_save(void);
extern void htex_prescan_reset_firstpass(void);
extern void htex_prescan_restore(int pageno);
extern void htex_prescan_initpage(void);
extern void htex_prescan_carry_over(int old_page, int new_page);
extern void htex_set_anchorsize(int x, int y, int w, int h);
extern void htex_set_objecttype(htexObjectT type);

extern void htex_reset_page(int pageno);
extern void htex_displayanchor(int, int);
extern void htex_set_anchormarker(int y);
extern void htex_back(void);
extern void htex_forward(void);

extern Boolean htex_scan_anchor(const char *special, size_t len);

#endif /* HTEXTEX_H_ */
