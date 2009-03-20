/*
 *  $Id: guage.c,v 1.40 2007/02/24 00:06:25 tom Exp $
 *
 * guage.c -- implements the gauge dialog
 *
 * Copyright 2000-2006,2007 Thomas E. Dickey
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
 *  An earlier version of this program lists as authors
 *	Marc Ewing, Red Hat Software
 */

#include <dialog.h>
#include <errno.h>

#define MY_LEN (MAX_LEN)/2

#define MIN_HIGH (4)
#define MIN_WIDE (10 + 2 * (2 + MARGIN))

#define isMarker(buf) !strncmp(buf, "XXX", 3)

#define if_FINISH(status,statement) if ((status) == 0) statement

#ifdef KEY_RESIZE
#define if_RESIZE(status,statement) if ((status) < 0) statement
#else
#define if_RESIZE(status,statement)	/* nothing */
#endif

static int
read_data(char *buffer, FILE *fp)
{
    int result;

    if (feof(fp)) {
	result = 0;
    } else if (fgets(buffer, MY_LEN, fp) != 0) {
	dlg_trim_string(buffer);
	result = 1;
    } else {
	result = -1;
#ifdef KEY_RESIZE
	/*
	 * Since we weren't doing getch's before, we won't necessarily
	 * have a KEY_RESIZE returned.  Call getch() anyway to force it
	 * to call resizeterm as-needed.
	 */
	if (errno == EINTR) {
	    (void) getch();	/* allow it to call resizeterm() */
	    refresh();
	    dlg_clear();
	    (void) getch();
	} else {
	    result = 0;
	}
#endif
    }
    return result;
}

static int
decode_percent(char *buffer)
{
    char *tmp = 0;
    long value = strtol(buffer, &tmp, 10);

    if (tmp != 0 && (*tmp == 0 || isspace(UCH(*tmp))) && value >= 0) {
	return TRUE;
    }
    return FALSE;
}

/*
 * Display a gauge, or progress meter.  Starts at percent% and reads stdin.  If
 * stdin is not XXX, then it is interpreted as a percentage, and the display is
 * updated accordingly.  Otherwise the next line is the percentage, and
 * subsequent lines up to another XXX are used for the new prompt.  Note that
 * the size of the window never changes, so the prompt can not get any larger
 * than the height and width specified.
 */
int
dialog_gauge(const char *title,
	     const char *cprompt,
	     int height,
	     int width,
	     int percent)
{
#ifdef KEY_RESIZE
    int old_height = height;
    int old_width = width;
#endif
    int i, x, y;
    int status;
    char buf[MY_LEN];
    char prompt_buf[MY_LEN];
    char *prompt = dlg_strclone(cprompt);
    WINDOW *dialog;

    curs_set(0);

    dlg_tab_correct_str(prompt);

#ifdef KEY_RESIZE
    nodelay(stdscr, TRUE);
    goto first_try;
  retry:
    dlg_del_window(dialog);
    height = old_height;
    width = old_width;
  first_try:
#endif

    dlg_auto_size(title, prompt, &height, &width, MIN_HIGH, MIN_WIDE);
    dlg_print_size(height, width);
    dlg_ctl_size(height, width);

    /* center dialog box on screen */
    x = dlg_box_x_ordinate(width);
    y = dlg_box_y_ordinate(height);

    dialog = dlg_new_window(height, width, y, x);

    do {
	(void) werase(dialog);
	dlg_draw_box(dialog, 0, 0, height, width, dialog_attr, border_attr);

	dlg_draw_title(dialog, title);

	wattrset(dialog, dialog_attr);
	dlg_print_autowrap(dialog, prompt, height, width);

	dlg_draw_box(dialog,
		     height - 4, 2 + MARGIN,
		     2 + MARGIN, width - 2 * (2 + MARGIN),
		     dialog_attr,
		     border_attr);

	/*
	 * Clear the area for the progress bar by filling it with spaces
	 * in the title-attribute, and write the percentage with that
	 * attribute.
	 */
	(void) wmove(dialog, height - 3, 4);
	wattrset(dialog, title_attr);

	for (i = 0; i < (width - 2 * (3 + MARGIN)); i++)
	    (void) waddch(dialog, ' ');

	(void) wmove(dialog, height - 3, (width / 2) - 2);
	(void) wprintw(dialog, "%3d%%", percent);

	/*
	 * Now draw a bar in reverse, relative to the background.
	 * The window attribute was useful for painting the background,
	 * but requires some tweaks to reverse it.
	 */
	x = (percent * (width - 2 * (3 + MARGIN))) / 100;
	if ((title_attr & A_REVERSE) != 0) {
	    wattroff(dialog, A_REVERSE);
	} else {
	    wattrset(dialog, A_REVERSE);
	}
	(void) wmove(dialog, height - 3, 4);
	for (i = 0; i < x; i++) {
	    chtype ch = winch(dialog);
	    if (title_attr & A_REVERSE) {
		ch &= ~A_REVERSE;
	    }
	    (void) waddch(dialog, ch);
	}

	(void) wrefresh(dialog);

	status = read_data(buf, dialog_state.pipe_input);
	if_FINISH(status, break);
	if_RESIZE(status, goto retry);

	if (isMarker(buf)) {
	    /*
	     * Historically, next line should be percentage, but one of the
	     * worse-written clones of 'dialog' assumes the number is missing.
	     * (Gresham's Law applied to software).
	     */
	    status = read_data(buf, dialog_state.pipe_input);
	    if_FINISH(status, break);
	    if_RESIZE(status, goto retry);

	    prompt_buf[0] = '\0';
	    if (decode_percent(buf))
		percent = atoi(buf);
	    else
		strcpy(prompt_buf, buf);

	    /* Rest is message text */
	    while ((status = read_data(buf, dialog_state.pipe_input)) > 0
		   && !isMarker(buf)) {
		if (strlen(prompt_buf) + strlen(buf) < sizeof(prompt_buf) - 1) {
		    strcat(prompt_buf, buf);
		}
	    }
	    if_FINISH(status, break);
	    if_RESIZE(status, goto retry);

	    if (prompt != prompt_buf)
		free(prompt);
	    prompt = prompt_buf;
	} else if (decode_percent(buf)) {
	    percent = atoi(buf);
	}
    } while (1);

#ifdef KEY_RESIZE
    nodelay(stdscr, FALSE);
#endif

    fclose(dialog_state.pipe_input);
    dialog_state.pipe_input = 0;

    curs_set(1);
    dlg_del_window(dialog);
    if (prompt != prompt_buf)
	free(prompt);
    return (DLG_EXIT_OK);
}
