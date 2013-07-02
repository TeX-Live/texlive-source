/*
 * Copyright (c) 2003-2004 the xdvik development team
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
 * Common code for Xaw and Motif menu bar creation.
 */
#ifndef MENU_H_
#define MENU_H_

#include "events.h"
#include "xdvi.h"

typedef enum {
    BT_INVALID = -1, /* error code */
    BT_NONE = 0,
    BT_PUSH,	/* pushbutton */
    BT_RADIO,	/* radio button (1 of n) */
    BT_CHECK,	/* check button (m of n) */
    BT_SEP	/* separator */
} buttonTypeT;


/* structures for menu data */
struct button_elems {
    char *title;		/* menu item label string */
    buttonTypeT type;		/* button type */
    char mnemonic;		/* 0 if none */
    char *accelerator;		/* NULL if none */
    struct button_info *submenu;/* submenu, or NULL */
    Widget widget;		/* the widget in this item, for later use, or 0 */
    struct xdvi_action *action; /* translated action, for later use, or NULL */
};

struct button_info {
    size_t size;
    struct button_elems *elems;
};

#include "xm_menu.h"	
#include "xaw_menu.h"

extern void create_menu_buttons(Widget parent,
#ifdef MOTIF
				Widget *child
#else
				int *ret_panel_width
#endif
				);

extern void set_menu(void *val, XtActionProc proc, Boolean (*cmp)());

#endif /* MENU_H_ */
