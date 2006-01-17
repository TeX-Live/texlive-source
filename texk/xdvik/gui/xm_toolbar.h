/*
 * Copyright (c) 2001 Marcin Dalecki
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

#ifndef XM_TOOLBAR_H_
#define XM_TOOLBAR_H_

#ifdef MOTIF

extern Widget create_toolbar(Widget parent, Widget menu_bar);

extern Boolean toolbar_visible, scrollbars_visible, pagelist_visible;
extern void create_tips(Widget parent);

#if HAVE_XPM
extern void tb_check_navigation_sensitivity(int pageno);
extern void tb_check_navigation_sensitivity(int pageno);
extern void tb_set_htex_forward_sensitivity(Boolean sensitive);
extern void tb_set_htex_back_sensitivity(Boolean sensitive);
extern void tb_set_pagehist_forward_sensitivity(Boolean sensitive);
extern void tb_set_pagehist_back_sensitivity(Boolean sensitive);
extern void tb_set_zoom_sensitivity(Boolean sensitive);
#endif /* HAVE_XPM */

extern void toggle_toolbar(void);
extern void toggle_scrollbars(void);

#endif /* MOTIF */

#endif /* XM_TOOLBAR_H_ */
