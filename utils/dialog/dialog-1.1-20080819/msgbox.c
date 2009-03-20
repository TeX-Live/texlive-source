/*
 *  $Id: msgbox.c,v 1.58 2007/03/25 20:08:23 Brian.Rolfe Exp $
 *
 *  msgbox.c -- implements the message box and info box
 *
 *  Copyright 2000-2006,2007	Thomas E. Dickey
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
 *
 *  An earlier version of this program lists as authors:
 *	Savio Lam (lam836@cs.cuhk.hk)
 */

#include <dialog.h>
#include <dlg_keys.h>

/*
 * Display the message in a scrollable window.  Actually the way it works is
 * that we create a "tall" window of the proper width, let the text wrap within
 * that, and copy a slice of the result to the dialog.
 *
 * It works for ncurses.  Other curses implementations show only blanks (Tru64)
 * or garbage (NetBSD).
 */
static int
show_message(WINDOW *dialog,
	     const char *prompt,
	     int offset,
	     int page,
	     int width,
	     int pauseopt)
{
#ifdef NCURSES_VERSION
    if (pauseopt) {
	int wide = width - (2 * MARGIN);
	int high = LINES;
	int y, x;
	int len;
	int percent;
	WINDOW *dummy;
	char buffer[5];

#if defined(NCURSES_VERSION_PATCH) && NCURSES_VERSION_PATCH >= 20040417
	/*
	 * If we're not limited by the screensize, allow text to possibly be
	 * one character per line.
	 */
	if ((len = dlg_count_columns(prompt)) > high)
	    high = len;
#endif
	dummy = newwin(high, width, 0, 0);
	wbkgdset(dummy, dialog_attr | ' ');
	wattrset(dummy, dialog_attr);
	werase(dummy);
	dlg_print_autowrap(dummy, prompt, high, width);
	getyx(dummy, y, x);

	copywin(dummy,		/* srcwin */
		dialog,		/* dstwin */
		offset + MARGIN,	/* sminrow */
		MARGIN,		/* smincol */
		MARGIN,		/* dminrow */
		MARGIN,		/* dmincol */
		page,		/* dmaxrow */
		wide,		/* dmaxcol */
		FALSE);

	delwin(dummy);

	/* if the text is incomplete, or we have scrolled, show the percentage */
	if (y > 0 && wide > 4) {
	    percent = (int) ((page + offset) * 100.0 / y);
	    if (percent < 0)
		percent = 0;
	    if (percent > 100)
		percent = 100;
	    if (offset != 0 || percent != 100) {
		(void) wattrset(dialog, position_indicator_attr);
		(void) wmove(dialog, MARGIN + page, wide - 4);
		(void) sprintf(buffer, "%d%%", percent);
		(void) waddstr(dialog, buffer);
		if ((len = strlen(buffer)) < 4) {
		    wattrset(dialog, border_attr);
		    whline(dialog, dlg_boxchar(ACS_HLINE), 4 - len);
		}
	    }
	}
	return (y - page);
    }
#endif
    (void) offset;
    wattrset(dialog, dialog_attr);
    dlg_print_autowrap(dialog, prompt, page + 1 + (3 * MARGIN), width);
    return 0;
}

/*
 * Display a message box. Program will pause and display an "OK" button
 * if the parameter 'pauseopt' is non-zero.
 */
int
dialog_msgbox(const char *title, const char *cprompt, int height, int width,
	      int pauseopt)
{
    /* *INDENT-OFF* */
    static DLG_KEYS_BINDING binding[] = {
	ENTERKEY_BINDINGS,
	DLG_KEYS_DATA( DLGK_ENTER,	' ' ),
	DLG_KEYS_DATA( DLGK_GRID_DOWN,	'J' ),
	DLG_KEYS_DATA( DLGK_GRID_DOWN,	'j' ),
	DLG_KEYS_DATA( DLGK_GRID_DOWN,	KEY_DOWN ),
	DLG_KEYS_DATA( DLGK_GRID_UP,	'K' ),
	DLG_KEYS_DATA( DLGK_GRID_UP,	'k' ),
	DLG_KEYS_DATA( DLGK_GRID_UP,	KEY_UP ),
	DLG_KEYS_DATA( DLGK_PAGE_FIRST,	'g' ),
	DLG_KEYS_DATA( DLGK_PAGE_FIRST,	KEY_HOME ),
	DLG_KEYS_DATA( DLGK_PAGE_LAST,	'G' ),
	DLG_KEYS_DATA( DLGK_PAGE_LAST,	KEY_END ),
	DLG_KEYS_DATA( DLGK_PAGE_NEXT,	'F' ),
	DLG_KEYS_DATA( DLGK_PAGE_NEXT,	'f' ),
	DLG_KEYS_DATA( DLGK_PAGE_NEXT,	KEY_NPAGE ),
	DLG_KEYS_DATA( DLGK_PAGE_PREV,	'B' ),
	DLG_KEYS_DATA( DLGK_PAGE_PREV,	'b' ),
	DLG_KEYS_DATA( DLGK_PAGE_PREV,	KEY_PPAGE ),
	DLG_KEYS_DATA( DLGK_FIELD_NEXT,	KEY_DOWN ),
	DLG_KEYS_DATA( DLGK_FIELD_NEXT, KEY_RIGHT ),
	DLG_KEYS_DATA( DLGK_FIELD_NEXT, TAB ),
	DLG_KEYS_DATA( DLGK_FIELD_PREV,	KEY_UP ),
	DLG_KEYS_DATA( DLGK_FIELD_PREV, KEY_BTAB ),
	DLG_KEYS_DATA( DLGK_FIELD_PREV, KEY_LEFT ),
	END_KEYS_BINDING
    };
    /* *INDENT-ON* */

    int x, y, last = 0, page;
    int button = 0;
    int key = 0, fkey;
    int result = DLG_EXIT_UNKNOWN;
    WINDOW *dialog = 0;
    char *prompt = dlg_strclone(cprompt);
    const char **buttons = dlg_ok_label();
    int offset = 0;
    int check;
    bool show = TRUE;

#ifdef KEY_RESIZE
    int req_high = height;
    int req_wide = width;
  restart:
#endif

    dlg_tab_correct_str(prompt);
    dlg_auto_size(title, prompt, &height, &width,
		  (pauseopt == 1 ? 2 : 0),
		  (pauseopt == 1 ? 12 : 0));
    dlg_print_size(height, width);
    dlg_ctl_size(height, width);

    x = dlg_box_x_ordinate(width);
    y = dlg_box_y_ordinate(height);

#ifdef KEY_RESIZE
    if (dialog != 0)
	dlg_move_window(dialog, height, width, y, x);
    else
#endif
    {
	dialog = dlg_new_window(height, width, y, x);
	dlg_register_window(dialog, "msgbox", binding);
	dlg_register_buttons(dialog, "msgbox", buttons);
    }
    page = height - (1 + 3 * MARGIN);

    dlg_mouse_setbase(x, y);

    dlg_draw_box(dialog, 0, 0, height, width, dialog_attr, border_attr);
    dlg_draw_title(dialog, title);

    wattrset(dialog, dialog_attr);

    if (pauseopt) {
	dlg_draw_bottom_box(dialog);
	mouse_mkbutton(height - 2, width / 2 - 4, 6, '\n');
	dlg_draw_buttons(dialog, height - 2, 0, buttons, button, FALSE, width);

	while (result == DLG_EXIT_UNKNOWN) {
	    if (show) {
		getyx(dialog, y, x);
		last = show_message(dialog, prompt, offset, page, width, pauseopt);
		wmove(dialog, y, x);
		wrefresh(dialog);
		show = FALSE;
	    }
	    key = dlg_mouse_wgetch(dialog, &fkey);
	    if (dlg_result_key(key, fkey, &result))
		break;

	    if (!fkey && (check = dlg_char_to_button(key, buttons)) >= 0) {
		result = check ? DLG_EXIT_HELP : DLG_EXIT_OK;
		break;
	    }

	    if (fkey) {
		switch (key) {
#ifdef KEY_RESIZE
		case KEY_RESIZE:
		    dlg_clear();
		    height = req_high;
		    width = req_wide;
		    show = TRUE;
		    goto restart;
#endif
		case DLGK_FIELD_NEXT:
		    button = dlg_next_button(buttons, button);
		    if (button < 0)
			button = 0;
		    dlg_draw_buttons(dialog,
				     height - 2, 0,
				     buttons, button,
				     FALSE, width);
		    break;
		case DLGK_FIELD_PREV:
		    button = dlg_prev_button(buttons, button);
		    if (button < 0)
			button = 0;
		    dlg_draw_buttons(dialog,
				     height - 2, 0,
				     buttons, button,
				     FALSE, width);
		    break;
		case DLGK_ENTER:
		    result = button ? DLG_EXIT_HELP : DLG_EXIT_OK;
		    break;
		case DLGK_PAGE_FIRST:
		    if (offset > 0) {
			offset = 0;
			show = TRUE;
		    }
		    break;
		case DLGK_PAGE_LAST:
		    if (offset < last) {
			offset = last;
			show = TRUE;
		    }
		    break;
		case DLGK_GRID_UP:
		    if (offset > 0) {
			--offset;
			show = TRUE;
		    }
		    break;
		case DLGK_GRID_DOWN:
		    if (offset < last) {
			++offset;
			show = TRUE;
		    }
		    break;
		case DLGK_PAGE_PREV:
		    if (offset > 0) {
			offset -= page;
			if (offset < 0)
			    offset = 0;
			show = TRUE;
		    }
		    break;
		case DLGK_PAGE_NEXT:
		    if (offset < last) {
			offset += page;
			if (offset > last)
			    offset = last;
			show = TRUE;
		    }
		    break;
		case DLGK_MOUSE(0):
		    result = DLG_EXIT_OK;
		    break;
		case DLGK_MOUSE(1):
		    result = DLG_EXIT_HELP;
		    break;
		default:
		    beep();
		    break;
		}
	    } else {
		beep();
	    }
	}
    } else {
	show_message(dialog, prompt, offset, page, width, pauseopt);
	wrefresh(dialog);
	result = DLG_EXIT_OK;
    }

    dlg_del_window(dialog);
    dlg_mouse_free_regions();
    free(prompt);
    return result;
}
