/*
 * Copyright (c) 1990-2004  Paul Vojta and others
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
 * NOTE: xdvi is based on prior work, as noted in the modification history in
 * xdvi.c.
 *
 */

#ifndef MAG_H_
#define MAG_H_

#define	MAGBORD	1	/* border size for magnifier */


extern size_t get_magglass_items(void);
extern int get_magglass_width(int idx);
extern int get_magglass_height(int idx);
extern void set_magglass_widht(int idx, int w);
extern void set_magglass_height(int idx, int h);

extern void show_distance_from_ruler(XEvent *event, Boolean to_stdout);
extern void drag_ruler_motion(XEvent *event);
extern void drag_ruler_release(XEvent *event);
extern void clear_ruler(void);
extern void show_ruler(XEvent *event);
extern void redraw_ruler(void);
extern void ruler_snap_origin(XEvent *event);

extern void magnifier_move(String params, XEvent *event);
extern void mag_release(XEvent * event);
extern void move_magnifier(void);
extern void create_magnifier(void);

#endif /* MAG_H_ */

