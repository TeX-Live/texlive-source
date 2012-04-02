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

#include "xdvi-config.h"
#include "xdvi.h"
#include "events.h"
#include "menu.h"
#include "util.h"

#ifdef MOTIF
#  include <Xm/RowColumn.h>
#  include <Xm/ToggleB.h>
#else
#  include <X11/Intrinsic.h>
#  include <X11/Xatom.h>
#  include <X11/StringDefs.h>
#  include <X11/Xaw/Label.h>
#endif


/* translate string argument into corresponding buttonTypeT */
static buttonTypeT
get_type(const char *str)
{
    if (strcmp(str, "PUSH") == 0)
	return BT_PUSH;
    else if (strcmp(str, "RADIO") == 0)
	return BT_RADIO;
    else if (strcmp(str, "CHECK") == 0)
	return BT_CHECK;
    else if (strcmp(str, "SEP") == 0)
	return BT_SEP;
    else
	return BT_INVALID;
}

static struct button_info *m_button_info = NULL; /* toplevel node of pulldown menu structure */


static void
set_menu_info(void *val, XtActionProc proc, Boolean (*cmp)(), struct button_info *item)
{
    size_t i;
    ASSERT(item != NULL, "item in set_menu_info musn't be NULL!");
    for (i = 0; i < item->size; i++) {
	if ((item->elems[i].type == BT_RADIO || item->elems[i].type == BT_CHECK)
	    && item->elems[i].action != NULL
	    && item->elems[i].action->proc != NULL
	    && item->elems[i].action->proc == proc
	    && item->elems[i].action->num_params > 0
	    && item->elems[i].action->params[0] != NULL) {
	    Boolean state;
	    ASSERT(cmp != NULL, "comparison function musn't be NULL!");
	    state = cmp(val, item->elems[i].action->params[0]);
#ifdef MOTIF
	    ASSERT(item->elems[i].widget != 0, "Widget musn't be NULL!");
	    XmToggleButtonSetState(item->elems[i].widget, state, False);
#else
	    if (item->elems[i].widget == 0) {
		XDVI_WARNING((stderr, "Widget for menu `%s' is null!", item->elems[i].title));
		continue;
	    }
	    xaw_set_button_state(item->elems + i, state);
#endif
	}
	if (item->elems[i].submenu != NULL) { /* invoke it recursively */
	    set_menu_info(val, proc, cmp, item->elems[i].submenu);
	}
    }
}


/* set a menu according to val and the compare function cmp */
void
set_menu(void *val, XtActionProc proc, Boolean (*cmp)())
{
    /* removed following since cast from function pointer to void pointer is not supported by ANSI C */
    /* TRACE_GUI((stderr, "set_menu_info: %d, %p, %p", *(int *)val, (void *)proc, (void *)cmp)); */
    set_menu_info(val, proc, cmp, m_button_info);
}

static void
initialize_menus(void)
{
    int use_gs;
    int shrinkval;
    
    /* initialize tickmarks for all possible actions */
    use_gs = resource.postscript;
#ifdef PS_GS
    if (!resource.useGS)
	use_gs = 0;
#endif

    set_menu(&use_gs, Act_set_ps, check_int);
#ifdef PS_GS
    set_menu(&resource.gs_alpha, Act_set_gs_alpha, check_toggle);
#endif
    set_menu(&resource.keep_flag, Act_set_keep_flag, check_toggle);
    shrinkval = resource.pixels_per_inch / mane.shrinkfactor;
    set_menu(&shrinkval, Act_shrink_to_dpi, check_int);
    set_menu(&mane.shrinkfactor, Act_set_shrink_factor, check_int);
    set_menu(&resource.use_tex_pages, Act_use_tex_pages, check_toggle);
#if 0
    set_menu((char *)resource.paper,  Act_set_paper_landscape, check_paper_landscape);
    set_menu((char *)resource.paper, Act_set_papersize, check_papersize);
#endif /* 0 */
    set_menu(&resource.mouse_mode, Act_switch_mode, check_int);
    set_menu(&resource.expert_mode, Act_set_expert_mode, check_resource_expert);
}

static void
free_items(char **items, size_t len)
{
    size_t curr = 0;
    while(curr < len) {
	free(items[curr++]);
    }
    free(items);
}

#if 0
static void
show_items(char *descr, char **items, size_t len)
{
    size_t i;
    for (i = 0; i < len; i++) {
	fprintf(stderr, "%s %d: |%s|\n", descr, i, items[i]);
    }
}
#endif

static void
add_info(struct button_info **info, buttonTypeT bt_type,
	 char mnemonic, const char *title,
	 const char *accelerator, struct xdvi_action *action)
{
    size_t idx = (*info)->size++;
    (*info)->elems = xrealloc((*info)->elems, (*info)->size * sizeof *((*info)->elems));
    (*info)->elems[idx].title = xstrdup(title);
    (*info)->elems[idx].type = bt_type;
    if (accelerator == NULL || accelerator[0] == '\0')
	(*info)->elems[idx].accelerator = NULL;
    else
	(*info)->elems[idx].accelerator = xstrdup(accelerator);
    (*info)->elems[idx].mnemonic = mnemonic;
    (*info)->elems[idx].action = action;
    (*info)->elems[idx].widget = 0;
    (*info)->elems[idx].submenu = NULL;    
}

static void
insert_items(struct button_info **info, char **items, size_t num_items,
	     const char *button_type, const char *accelerator, const char *action)
{
    const char ENTRY_SEP = '|';
    size_t i = 0;
    size_t entry_len;
    size_t entry_count = 0;
    char **entry_items = NULL;
    size_t idx;
    
    Boolean found = False;
    Boolean have_error = False;

    if (*items == NULL) {
	/* should be a separator, which is treated as a special case since
	   there's no menu title: */
	if (strcmp (button_type, "SEP") == 0)
	    add_info(info, BT_SEP, '\0', "SEP", NULL, NULL);
	else
	    XDVI_WARNING((stderr, "Shouldn't happen: items == NULL!"));
	return;
    }
	
    entry_len = strlen(items[0]);
    entry_items = split_line(items[0], ENTRY_SEP, 0, entry_len, &entry_count);

    if (entry_count < 2) {
	XDVI_WARNING((stderr, "Missing Mnemonic in button info `%s'", items[0]));
	entry_count++;
	entry_items = xrealloc(entry_items, entry_count * sizeof *entry_items);
	entry_items[1] = xstrdup("");
	entry_items[2] = NULL;
    }
    
    for (i = 0; i < (*info)->size; i++) {
	if (strcmp(entry_items[0], (*info)->elems[i].title) == 0) {
	    found = true;
	    break;
	}
    }

    idx = i;

    if (!found) { /* new item, resize info and add this item */
	struct xdvi_action *my_action = NULL;
	buttonTypeT my_type = BT_NONE;

	/* if it's a `leaf' in the menu hierarchy, compile the action and set the button type */
	if (num_items == 1) {
	    char *fmt = strchr(entry_items[0], '$');
	    if (fmt != NULL
		&& (fmt == entry_items[0] || (fmt > entry_items[0] && *(fmt - 1) != '\\'))
		&& (fmt[1] == '#' || fmt[1] == '%' ||fmt[1] == '_')) {
		XDVI_WARNING((stderr, "Xdvik doesn't support format characters in button labels; "
			      "skipping button \"%s\"", items[0]));
		have_error = True;
	    }
	    
	    if (strlen(action) == 0 || (!compile_action(action, &my_action))) {
		XDVI_WARNING((stderr, "Invalid action \"%s\" for button \"%s\" (skipping this line).",
			      action, items[0]));
		have_error = True;
	    }

	    if ((my_type = get_type(button_type)) == BT_INVALID) {
		XDVI_WARNING((stderr, "Invalid type \"%s\" for button \"%s\" (skipping this line).",
			      button_type, items[0]));
		have_error = True;
	    }
	}
	
	if (!have_error) {
	    add_info(info, my_type, entry_items[1][0], entry_items[0], accelerator, my_action);
	}
    }
    free_items(entry_items, entry_count);

    if (num_items > 1 || (num_items == 1 && strcmp(button_type, "SEP") == 0)) { /* not a leaf, invoke recursivly for next level */
	if ((*info)->elems[idx].submenu == NULL) { /* submenu didn't exist yet, create it */
	    struct button_info *new_submenu = xmalloc(sizeof *new_submenu);
	    new_submenu->elems = NULL;
	    new_submenu->size = 0;
	    (*info)->elems[idx].submenu = new_submenu;
	}
	insert_items(&((*info)->elems[idx].submenu), items + 1, num_items - 1, button_type, accelerator, action);
    }
}

static void
show_button_info(int depth, struct button_info *info)
{
    size_t i;
    for (i = 0; i < info->size; i++) {
	TRACE_GUI((stderr, "%*c-->%s; type=%d; mnemonic=%c; accel=%s; submenu=%p; w=%lu; action: %p",
		   depth, ' ',
		   info->elems[i].title,
		   info->elems[i].type,
		   info->elems[i].mnemonic,
		   info->elems[i].accelerator ? info->elems[i].accelerator : "<NULL>",
		   (void *)info->elems[i].submenu,
		   (unsigned long)info->elems[i].widget,
		   (void *)info->elems[i].action));
	if (info->elems[i].submenu != NULL) {
	    show_button_info(depth + 3, info->elems[i].submenu);
	}
    }
}

static void
parse_button_translations(struct button_info **info)
{
    const char *curr_p, *end_p;
    
    const char LINE_SEP = ':';
    const char MENU_SEP = '>';
    
    for (curr_p = resource.menu_translations;
	 curr_p != NULL && *curr_p != '\0';
	 curr_p = end_p + 1) {
	end_p = strchr(curr_p, '\n');
	if (end_p != NULL) {
	    size_t line_len = end_p - curr_p;
	    size_t line_count = 0;
	    char **line_items;
	    line_items = split_line(curr_p, LINE_SEP, 0, line_len, &line_count);
	    /*  	    fprintf(stderr, "length of line: %d; %d items\n", line_len, line_count); */
	    /*  	    show_items("LINE", line_items, line_count); */
	    if (line_count != 4) { /* error */
		XDVI_WARNING((stderr, "Wrong number of items (%lu) in translations line:\n\"%.*s\" "
			      "(skipping this line).\n",
			      (unsigned long)line_count, (int)line_len, curr_p));
		free_items(line_items, line_count);
		continue;
	    }
	    else { /* split first elem into menu description */
		size_t menu_len = strlen(line_items[0]);
		size_t menu_count = 0;
		char **menu_items;
		if (menu_len == 0) { /* error */
		    XDVI_WARNING((stderr, "Menu description (first item) mustn't be empty:\n\"%.*s\" "
				  "(skipping this line).\n",
				  (int)line_len, curr_p));
		    free_items(line_items, line_count);
		    continue;
		}
		menu_items = split_line(line_items[0], MENU_SEP, 0, menu_len, &menu_count);
		/*  		show_items("   MENU", menu_items, menu_count); */

		insert_items(info, menu_items, menu_count, line_items[1], line_items[2], line_items[3]);
		free_items(menu_items, menu_count);
	    }
	    free_items(line_items, line_count);
	}
    }
    show_button_info(0, *info);
}

/*
  Top-level routine: creates the pulldown menu buttons for Motif and Xaw.
  For Motif, sets `*menu_bar' to the address of the new widget created,
  for Xaw, sets *width to the width of the button panel created.
*/
void
create_menu_buttons(Widget parent,
#ifdef MOTIF
		    Widget *menu_bar
#else
		    int *width
#endif
		    )
{
#ifdef MOTIF
    Widget menu = 0;
    size_t i;
#else /* MOTIF */
    Widget panel = 0;
#endif
    
    m_button_info = xmalloc(sizeof *m_button_info);
    m_button_info->elems = NULL;
    m_button_info->size = 0;

#ifdef MOTIF
    *menu_bar = XmCreateMenuBar(parent, "menuBar", NULL, 0);
    parse_button_translations(&m_button_info);

    for (i = 0; i < m_button_info->size; i++) {
	menu = xm_create_menu(*menu_bar,
			      m_button_info->elems[i].title,
			      m_button_info->elems[i].mnemonic,
			      m_button_info->elems[i].submenu);
    }
    if (menu != 0) {
	XtVaSetValues(*menu_bar, XmNmenuHelpWidget, menu, NULL);
    }
#else /* MOTIF */
    xaw_initialize_menu_bitmaps();
    panel = xaw_create_menu_widgets(parent);
    parse_button_translations(&m_button_info);
    xaw_create_menu(m_button_info, panel, width);
#endif /* MOTIF */
    initialize_menus();
}

