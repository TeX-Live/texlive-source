/*
 * Copyright (c) 2001-2004 the xdvik development team
 *
 * Page selector for xdvi
 *
 * Copyright (c) 1993, 1995
 *      MATSUURA Syun           syun@fuka.info.waseda.ac.jp
 *      HIRAHARA Atsushi        hirahara@fuka.info.waseda.ac.jp
 *      ONO Kouichi             onono@fuka.info.waseda.ac.jp
 *
 * All rights reserved.
 */

/* SU: I was unsure how to interpret the `All rights reserved' in the
 * previous line, so emailed Ono Kouichi about this.  Here's (the
 * relevant part of) his answer (which was CC'ed to Hirahara Atsushi -
 * all three of them had left Waseda university around '95):
 *
 *    You can modify, embed, copy and distribute a part of or the
 *    entire of our source code when you specify our copyright in your
 *    xdvik version.
 *
 * IANAL, but I think this is compatible with the X consortium
 * license as specified in the other files.
 */

#ifndef PAGESEL_H_
#define PAGESEL_H_

#include "xdvi.h"

extern void refresh_pagelist(int newsize, int newpage);
extern void maybe_scroll_pagelist(int newpage, Boolean force_recenter);
extern void create_pagelist(void);
extern void list_toggle_current(int arg);
extern void list_toggle_marks(int arg);

# ifdef MOTIF
extern Widget page_list;
extern void toggle_pagelist(void);
# else
extern int xaw_get_pagelist_size(void);
extern void xaw_create_pagelist_widgets(Dimension height, Dimension width, Position y, Widget parent);
extern void handle_destroy_pagelist(Widget w, XtPointer client_data, XtPointer call_data);
extern void handle_pagelist_resize(void);
# endif

/* pageinfo access methods */
extern long pageinfo_get_offset(int page);
extern int pageinfo_get_number(int page);
extern int pageinfo_get_index_of_number(int number);

extern unsigned int pageinfo_get_page_width(int page);
extern unsigned int pageinfo_get_page_height(int page);
extern unsigned int pageinfo_get_window_width(int page);
extern unsigned int pageinfo_get_window_height(int page);

extern void pageinfo_set_page_width(int page, unsigned int width);
extern void pageinfo_set_page_height(int page, unsigned int height);
extern void pageinfo_set_window_width(int page, unsigned int width);
extern void pageinfo_set_window_height(int page, unsigned int height);

extern void pageinfo_set_offset(int index, long offset);
extern void pageinfo_set_number(int index, int number);

extern void pageinfo_allocate(int total_pages);
extern void pageinfo_deallocate(void);
extern Boolean pageinfo_is_marked(int i);
extern Boolean pageinfo_have_marked_pages(void);

#endif /* PAGESEL_H_ */
