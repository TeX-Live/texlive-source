/*
 *  $Id: arrows.c,v 1.19 2007/02/19 01:22:19 tom Exp $
 *
 *  arrows.c -- draw arrows to indicate end-of-range for lists
 *
 * Copyright 2000-2006,2007   Thomas E. Dickey
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License, version 2.1
 *  as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program; if not, write to
 *	Free Software Foundation, Inc.
 *	51 Franklin St., Fifth Floor
 *	Boston, MA 02110, USA.
 */

#include <dialog.h>

#ifdef USE_WIDE_CURSES
#define add_acs(win, code) wadd_wch(win, W ## code)
#else
#define add_acs(win, code) waddch(win, dlg_boxchar(code))
#endif

#ifdef HAVE_COLOR
static chtype
merge_colors(chtype foreground, chtype background)
{
    chtype result = foreground;
    if ((foreground & A_COLOR) != (background & A_COLOR)) {
	short fg_f, bg_f;
	short fg_b, bg_b;
	if (pair_content(PAIR_NUMBER(foreground), &fg_f, &bg_f) != ERR
	    && pair_content(PAIR_NUMBER(background), &fg_b, &bg_b) != ERR) {
	    result &= ~A_COLOR;
	    result |= dlg_color_pair(fg_f, bg_b);
	}
    }
    return result;
}
#else
#define merge_colors(f,b) (f)
#endif

void
dlg_draw_arrows2(WINDOW *dialog,
		 int top_arrow,
		 int bottom_arrow,
		 int x,
		 int top,
		 int bottom,
		 chtype attr,
		 chtype borderattr)
{
    chtype save = getattrs(dialog);
    int cur_x, cur_y;

    getyx(dialog, cur_y, cur_x);

    (void) wmove(dialog, top, x);
    if (top_arrow) {
	wattrset(dialog, merge_colors(uarrow_attr, attr));
	(void) add_acs(dialog, ACS_UARROW);
	(void) waddstr(dialog, "(-)");
    } else {
	wattrset(dialog, attr);
	(void) whline(dialog, dlg_boxchar(ACS_HLINE), 4);
    }
    mouse_mkbutton(top, x - 1, 6, KEY_PPAGE);

    (void) wmove(dialog, bottom, x);
    if (bottom_arrow) {
	wattrset(dialog, merge_colors(darrow_attr, attr));
	(void) add_acs(dialog, ACS_DARROW);
	(void) waddstr(dialog, "(+)");
    } else {
	wattrset(dialog, borderattr);
	(void) whline(dialog, dlg_boxchar(ACS_HLINE), 4);
    }
    mouse_mkbutton(bottom, x - 1, 6, KEY_NPAGE);

    (void) wmove(dialog, cur_y, cur_x);
    wrefresh(dialog);

    wattrset(dialog, save);
}

void
dlg_draw_arrows(WINDOW *dialog,
		int top_arrow,
		int bottom_arrow,
		int x,
		int top,
		int bottom)
{
    dlg_draw_arrows2(dialog,
		     top_arrow,
		     bottom_arrow,
		     x,
		     top,
		     bottom,
		     menubox_attr,
		     menubox_border_attr);
}
