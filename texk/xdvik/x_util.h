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

#ifndef X_UTIL_H_
#define X_UTIL_H_

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include "xdvi.h"

/* nasty globals */
/*
  Whether program should autoscroll to make href target marker or
  forward search marker visible.
  This is disabled by ordinary user scrolling (otherwise xdvi would
  jump back to the old place, undoing the user scroll), so we need
  global access to it.
 */
extern Boolean do_autoscroll;


extern void center_window(Widget w1, Widget w2);
extern void position_window(Widget w, Position x, Position y);
extern void scroll_page_if_needed(int x_min, int x_max, int y_min, int y_max);
extern void adjust_width(Widget a, Widget b);
extern GC set_or_make_gc(GC gc, int function, Pixel fg, Pixel bg);

extern size_t property_get_data(Window, Atom, char **,
				int (*x_get_property) (Display *, Window, Atom, long,
						       long, Bool, Atom, Atom *, int *, unsigned long *,
						       unsigned long *, unsigned char **));

extern void set_dvi_property(void);
extern void update_window_property(Window w, Boolean prepend);
extern void update_dvi_property(void);
extern void property_initialize(void);

/*
 * atom_xdvi_windows() is attached to the root window of the default
 * screen of the display; it contains a list of (hopefully active)
 * xdvi windows.
 */
Atom atom_xdvi_windows(void);

/*
 * atom_dvi_file() is attached to the main xdvi window; it tells the
 * world what dvi file is being viewed.  It is set by that copy of
 * xdvi and read by this routine.  The first 8 bytes are the inode
 * number, and the rest is the file name.  We use 8 instead of
 * sizeof(ino_t) because the latter may vary from machine to machine,
 * and the format needs to be machine independent.
 */
Atom atom_dvi_file(void);

/* Atoms attached to the main xdvi window */

/* instruct the current instance to do a forward search
 * on the source string encoded in the property */
Atom atom_src_goto(void);

/* instruct the current instance to do a string search
 * on the string encoded in the property */
Atom atom_find_string(void);

/* instruct the current instance to reload the current file */
Atom atom_reload(void);

/* instruct the current instance to load the new file contained in the property */
Atom atom_newdoc(void);

/* instruct the current instance to go to the page encoded in the property */
Atom atom_newpage(void);

/* instruct the current instance to raise its window */
Atom atom_raise(void);

/* instruct the current instance to reread preferences from ~/.xdvirc.tmp */
Atom atom_reread_prefs(void);

extern void synthesize_event(XEvent *ev, Widget button);

#ifdef MOTIF
extern int xm_get_height(Widget w);
extern int xm_get_width(Widget w);
extern void str_to_pixel(Widget w, const char *colorname, Pixel *ret);
extern void pixel_to_str(Widget w, Pixel pix, char *str, size_t len);
extern void pixel_to_color(Pixel pix, XColor *color, Display *display, Colormap colormap);
#endif

extern void adjust_width_to_max(Widget w, ...);
extern Widget get_matching_parent(Widget w, Widget p, const char *fmt, ...);
extern Boolean widget_is_parent(Widget w, Widget p, Widget s);

extern void adjust_vertically(Widget w1, Widget w2, int default_dist);
extern void adjust_heights(Widget w1, ...);
extern void adjust_heights_min(Widget w1, ...);
/*  extern void adjust_widths(Widget w1, ...); */

void block_event_callback(Widget w, XtPointer client_data,
			  XEvent *ev, Boolean *cont);

extern void unexpected_widget_in_callback(Widget w, const char *callback);
extern Boolean get_widget_by_name(Widget *ret, Widget parent, const char *name, Boolean report_error);

extern void merge_into_user_db(XrmDatabase db);
extern void store_user_preference(const char *name, const char *fmt, ...);
extern void store_preference(XrmDatabase *db, const char *name, const char *fmt, ...);
extern Boolean save_user_preferences(Boolean full_save);
extern void read_user_preferences(Widget toplevel, const char *filename);

/* property related routines */
typedef void (*property_cbT)(Window w);
extern Window get_xdvi_window_id(Boolean same_file, property_cbT callback);
extern void set_string_property(const char *str, Atom prop, Window win);

extern Boolean clip_region(int *x, int *y, int *w, int *h);
extern Boolean clip_region_to_rect(XRectangle *rect);

extern Boolean window_is_mapped(Window w, Display *dpy);

extern XtPointer cast_int_to_XtPointer(int x);

#endif
