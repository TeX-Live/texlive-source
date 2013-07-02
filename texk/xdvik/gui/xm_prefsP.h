/*
 * Copyright (c) 2004 Stefan Ulrich
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

/*
 * `Private' common header for preferences dialogs.
 */

#ifndef XM_PREFS_P_H_
#define XM_PREFS_P_H_

#include "xdvi.h"

#ifdef MOTIF
/*
 * For consistency when also using the names for XtNameToWidget() in callbacks
 */
/* widget names */
#define Xdvi_PREFS_DIALOG_NAME			"preferences_window"
#define Xdvi_COLOR_DIALOG_NAME			"color_dialog"
#define Xdvi_COLOR_DIALOG_OLD_SAMPLE_NAME	"old_sample"
#define Xdvi_BROWSER_MENU_NAME			"browser_menu"
#define Xdvi_BROWSER_COMBO_NAME			"browser_combo"
#define Xdvi_EDITOR_MENU_NAME			"editor_menu"
#define Xdvi_EDITOR_COMBO_NAME			"editor_combo"
#define Xdvi_EDITOR_POPUP_NAME			"editor_prompt"
#define Xdvi_EDITOR_POPUP			"editor_prompt_popup"
#define Xdvi_BROWSER_POPUP_NAME			"browser_prompt"
#define Xdvi_BROWSER_POPUP			"browser_prompt_popup"
#define Xdvi_TIPS_STATUSLINE			"tips_statusline"
#define Xdvi_TIPS_POPUPS			"tips_popups"
#define Xdvi_TIPS_DELAY_TEXT			"tips_delay_text"
#define Xdvi_TIPS_DELAY_LABEL1			"tips_delay_label1"
#define Xdvi_TIPS_DELAY_LABEL2			"tips_delay_label2"
#define Xdvi_HOME_POSITION_X_OFF_TEXT		"x_off_text"
#define Xdvi_HOME_POSITION_Y_OFF_TEXT		"y_off_text"
#define Xdvi_HOME_POSITION_UNITS_MENU		"units_menu"
#define Xdvi_HOME_POSITION_UNITS_PULLDOWN	"units_pulldown"
#define Xdvi_DARKNESS_SPINBOX			"darkness_spinbox"
#define Xdvi_DARKNESS_TEXT			"darkness_text"
#define Xdvi_SHRINK_SPINBOX			"shrink_spinbox"
#define Xdvi_SHRINK_TEXT			"shrink_text"
#define Xdvi_PAPER_CASCADE			"papersize_option"
#define Xdvi_PAPER_MENU				"papersize_menu"

/* widget labels that are also used as widget names */
#define Xdvi_TB_BUTTONS_FLAT_STR		"Flat"
#define Xdvi_TB_BUTTONS_RAISED_STR		"Raised"
#define Xdvi_GUI_STATUSLINE_STR			"Show Statusline"
#define Xdvi_GUI_TOOLBAR_STR			"Show Toolbar"
#define Xdvi_GUI_PAGELIST_STR			"Show Pagelist"
#define Xdvi_GUI_SCROLLBARS_STR			"Show Scrollbars"
#define Xdvi_MATCH_INVERTED_STR			"Inverted"
#define Xdvi_MATCH_BOXED_STR			"Boxed with Highlight Color"
#define Xdvi_FG_COLOR_STR			"Text:"
#define Xdvi_FG_COLOR_BTN			"fg_button"
#define Xdvi_BG_COLOR_STR			"Background:"
#define Xdvi_BG_COLOR_BTN			"bg_button"
#define Xdvi_HL_COLOR_BTN			"hl_button"
#define Xdvi_VISITED_LINKS_STR			"Visited Links:"
#define Xdvi_VISITED_LINKS_BTN			"visited_links_button"
#define Xdvi_UNVISITED_LINKS_STR		"Unvisited Links:"
#define Xdvi_UNVISITED_LINKS_BTN		"unvisited_links_button"
#define Xdvi_DOCUMENT_COLORS_STR		"Use Document Colors"
#define Xdvi_LINKS_UNDERLINED_STR		"Underlined"
#define Xdvi_ADD_COMMAND_STR			"Other ..."
#define Xdvi_SCROLL_KEEP_STR			"Keep current position"
#define Xdvi_SCROLL_UNKEEP_STR			"Scroll back to home position of page"
#define Xdvi_HOME_POSITION_STR			"Home position at:"
#define Xdvi_HOME_POSITION_X_STR		"x"
#define Xdvi_HOME_POSITION_Y_STR		"y"
#define Xdvi_SCROLL_CURRENT_STR			"Use current position as home position"
#define Xdvi_REMEMBER_WINDOWSIZE_STR		"Remember current window size"
#define Xdvi_PAPER_PORTRAIT_STR			"Portrait"
#define Xdvi_PAPER_LANDSCAPE_STR		"Landscape"
#define Xdvi_APPLY_STR				"Apply"

#define Xdvi_PREFS_BROWSER_DEFAULTS \
	"xdg-open %s\n" \
	"htmlview %s\n" \
	"firefox -remote \"openURL(%s,new-window)\"\n" \
	"mozilla -remote \"openURL(%s,new-window)\"\n" \
	"netscape -raise -remote \"openURL(%s,new-window)\"\n" \
	"xterm -e w3m %s\n" \
	"xterm -e lynx %s\n" \
	"xterm -e wget %s\n" \
	Xdvi_ADD_COMMAND_STR
#define Xdvi_PREFS_EDITOR_DEFAULTS \
	"gnuclient -q +%l %f\n" \
	"emacsclient --no-wait +%l %f\n" \
	"gvim --servername xdvi --remote +%l %f\n" \
	"nc -noask +%l %f\n" \
	"xterm -e vi +%l %f\n" \
	Xdvi_ADD_COMMAND_STR

struct prefs_choice {
    XrmDatabase db;
/*     struct x_resources *orig; */
/*     struct x_resources *changed; */
    Widget *depwin;
    size_t depwin_cnt;
};

extern void h_attach_below(Widget x, Widget y);
extern void h_update_hyperlinks(Widget w, Pixel pix); /* implemented in xm_prefs_fonts.c */

#endif /* MOTIF */

#endif /* XM_PREFS_P_H_ */

