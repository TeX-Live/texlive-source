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
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL PAUL VOJTA OR ANY OTHER AUTHOR OF THIS SOFTWARE BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
  List of recently visited files, initialized from X resource
  fileHistory and displayed in `File -> Open Recent' menu.
*/

#include "xdvi-config.h"

#include <ctype.h>

#include "xdvi.h"
#include "util.h"
#include "dl_list.h"
#include "my-snprintf.h"
#include "string-utils.h"
#include "dvi-init.h"
#include "events.h"
#include "message-window.h"
#include "xm_menu.h"
#include "xaw_menu.h"
#include "filehist.h"

/****************************************************************************
 *
 * File-scope globals
 *
 ****************************************************************************/

/* list of files */
static struct dl_list *m_file_history = NULL;

/* current size of the list */
static int m_file_history_length = 0;

/* /\* current position in the list *\/ */
/* static int m_file_history_currpos = 0; */

/* item in above list */
struct file_history {
    char *filename;
    int pageno;
};

#define DEBUG 1

/****************************************************************************
 *
 * Private functions
 *
 ****************************************************************************/

static void
file_history_show(struct dl_list *list)
{
    if (globals.debug & DBG_FILES) {
	int n;
	fprintf(stderr, "======= File history:\n");
	for (n = 0; list != NULL; list = list->next, n++) {
	    struct file_history *item = (struct file_history *)(list->item);
	    if (item == NULL) {
		fprintf(stderr, "item %d is NULL!\n", n);
		continue;
	    }
	    fprintf(stderr, "item %d: %d:%s\n", n, item->pageno, item->filename);
	}
    }
}


/****************************************************************************
 *
 * Exported functions
 *
 ****************************************************************************/

void
file_history_init(void)
{
    char **fileinfo;
    size_t i;
    struct dl_list *head = NULL;
    
    m_file_history_length = 0;

    if (resource.file_history == NULL)
	return;
	
    fileinfo = get_separated_list(resource.file_history, "\n", False);
    
    for (i = 0; fileinfo[i] != NULL && i < (size_t)resource.file_history_size; i++) {
	struct file_history *item = xmalloc(sizeof *item);
	char *ptr;
	int pageno = strtol(fileinfo[i], &ptr, 10);
	TRACE_FILES((stderr, "FILEINFO: %s", fileinfo[i]));
	if (ptr == fileinfo[i]) {
	    XDVI_WARNING((stderr, "Missing page number in resource line `%s'!\n",
			  fileinfo[i]));
	    pageno = 0;
	}
	else {
	    while (isspace((int)*ptr))
		ptr++;
	}

	item->pageno = pageno;
	item->filename = xstrdup(ptr);

	TRACE_FILES((stderr, "FILE: %d:%s", item->pageno, item->filename));
	
	m_file_history = dl_list_insert(m_file_history, item);
	if (head == NULL) /* remember head position */
	    head = m_file_history;
	
	TRACE_FILES((stderr, "NEW ELEM: %p", (void *)m_file_history));

	m_file_history_length++;

	free(fileinfo[i]);
    }
    free(fileinfo);

    m_file_history = head;    
    file_history_show(m_file_history);
}

static Boolean
equals_filename(const void *it, const void *fname)
{
    const struct file_history *item = (const struct file_history *)it;
    const char *filename = (const char *)fname;

    return strcmp(item->filename, filename) == 0;
}


/* put new elem for filename `filename' and page number `page' at the front
   of the list. Return True if the addition caused the history to grow, else
   False (in case the item was only moved to the front).
*/
Boolean
file_history_push(const char *filename)
{
    int i;
    struct file_history *item = NULL;
    void *removed_data = NULL;
    struct file_history *removed_item = NULL;
    int len_bak = m_file_history_length;
    int count = 0;

    TRACE_FILES((stderr, "Pushing: |%s|", filename));
    
    /* if list already contains an item with same filename, remove it */
    m_file_history = dl_list_remove(m_file_history, filename, &count, &removed_data, equals_filename);
    if (count == 0)
	m_file_history_length++;

    removed_item = (struct file_history *)removed_data;
    
    file_history_show(m_file_history); 

    TRACE_FILES((stderr, "current length: %d, max: %d", m_file_history_length, resource.file_history_size));
    
    /* truncate list if it has reached its max length */
    if (m_file_history_length > resource.file_history_size) {
	struct dl_list *listpos = m_file_history;
	struct dl_list *last_pos = listpos;
	for (i = 0; listpos != NULL && i < m_file_history_length; i++) {
	    last_pos = listpos;
	    listpos = listpos->next;
	}

	item = last_pos->item; /* reuse item */
	TRACE_FILES((stderr, "Re-using item: |%s| -> |%s|", item->filename, filename));
	item->filename = xrealloc(item->filename, strlen(filename) + 1);
	strcpy(item->filename, filename);
	
	(void)dl_list_remove_item(&last_pos);
	m_file_history_length--;
    }
    else if (removed_item != NULL) {
	TRACE_FILES((stderr, "Re-using item: |%s|\n", removed_item->filename));
	item = removed_item; /* reuse item */
    }
    else {
	item = xmalloc(sizeof *item);
	TRACE_FILES((stderr, "NEW item: |%s|\n", filename));
	item->filename = xstrdup(filename);
	item->pageno = 0;
    }

    /* add new element at front of list */
    m_file_history = dl_list_push_front(m_file_history, item);
    
    file_history_show(m_file_history);

    TRACE_FILES((stderr, "returning: %d < %d", len_bak, m_file_history_length));
    return len_bak < m_file_history_length;
}

size_t file_history_size(void)
{
    return m_file_history_length;
}

void file_history_set_page(int pageno)
{
    struct dl_list *head;

    TRACE_FILES((stderr, "SETTING HEAD to %d", pageno));
    file_history_show(m_file_history);
    head = dl_list_head(m_file_history);
    if (head != NULL) {
	struct file_history *item = (struct file_history *)head->item;
	TRACE_FILES((stderr, "Setting page of |%s| to %d", item->filename, pageno));
	item->pageno = pageno;
    }
}

void file_history_set_page_at(int idx, int pageno)
{
    int i;
    struct dl_list *listpos = dl_list_head(m_file_history);
    struct file_history *item;
    
    for (i = 0; listpos != NULL && i < idx; i++) {
	listpos = listpos->next;
    }
    if (listpos == NULL) {
	TRACE_FILES((stderr, "Asked for file at position %d, but only %d elements in list",
		     idx, i - 1));
	return;
    }
    item = (struct file_history *)listpos->item;
    TRACE_FILES((stderr, "set_page_at index %d: Setting page of |%s| to %d", idx, item->filename, pageno));
    item->pageno = pageno;
}

int file_history_get_page(void)
{
    struct dl_list *head = dl_list_head(m_file_history);
    if (head != NULL) {
	struct file_history *item = (struct file_history *)head->item;
	TRACE_FILES((stderr, "Getting page of |%s|: %d", item->filename, item->pageno));
	return item->pageno;
    }
    return 0;
}

/*
 * Invoke `callback' for each elem of file history, passing
 * the current index, filename, page number, and data passed to
 * this function.
 */
void
file_history_enumerate(filehistCallbackT callback, void *data)
{
    int i;
    struct dl_list *listpos = dl_list_head(m_file_history);

    for (i = 0; listpos != NULL; i++) {
	struct file_history *item = (struct file_history *)listpos->item;
	callback(i, item->filename, item->pageno, data);
	listpos = listpos->next;
    }
}

char *
file_history_get_elem(int idx, int *ret_page)
{
    int i;
    struct dl_list *listpos = dl_list_head(m_file_history);
    struct file_history *item;
    
    for (i = 0; listpos != NULL && i < idx; i++) {
	listpos = listpos->next;
    }
    if (listpos == NULL) {
	XDVI_WARNING((stderr, "Asked for file at position %d, but only %d elements in list",
		      idx, i - 1));
	return NULL;
    }
    item = (struct file_history *)listpos->item;
    *ret_page = item->pageno;
    file_history_show(m_file_history);
    return item->filename;
}

char *
file_history_get_list(void)
{
    char buf[LENGTH_OF_INT];
    char *ret = xstrdup("");
    struct dl_list *listpos;
    
    for (listpos = dl_list_head(m_file_history);
	 listpos != NULL;
	 listpos = listpos->next) {
	
	struct file_history *item = (struct file_history *)listpos->item;
	SNPRINTF(buf, LENGTH_OF_INT, "%d ", item->pageno);
	ret = xstrcat(ret, buf);
	ret = xstrcat(ret, item->filename);
	ret = xstrcat(ret, "\n");	
    }

    ret[strlen(ret) - 1] = '\0'; /* chop off excess \n at end */
    return ret;
}

void
file_history_open(const char *fname)
{
    Boolean tried_dvi_ext = True;
    char *new_dvi_name = NULL;

    int dummy_cnt = 0;
    void *dummy_data = NULL;
    
    file_history_set_page(current_page);
    if ((new_dvi_name = open_dvi_file_wrapper(fname,
					      True, /* pretend file is from commandline, otherwise xdvi will
						       try to fork for non-existing files, leading to confusing
						       popup error messages */
					      False, &tried_dvi_ext,
					      True /* don't exit on error */)) == NULL) {
	/* remove this item from the file history */
	m_file_history = dl_list_remove(m_file_history, fname, &dummy_cnt, &dummy_data, equals_filename);
	m_file_history_length--;
	file_history_show(m_file_history); 

	filehist_menu_refresh();
	return;
    }
    else {
	dviErrFlagT errflag;
	if (load_dvi_file(
#if !DELAYED_MKTEXPK
			  True,
#endif
			  &errflag)) {
	    /* page_history_insert(pageno); */
	    set_dvi_name(new_dvi_name);
	    
	    globals.ev.flags |= EV_NEWDOC;
	    globals.ev.flags |= EV_FILEHIST_GOTO_PAGE;
	}
	else { /* re-open old file */
	    popup_message(globals.widgets.top_level,
			  MSG_ERR,
			  NULL,
			  "Could not open `%s': %s.\n"
			  /* "Removing this file from the history." */,
			  globals.dvi_name, get_dvi_error(errflag));
	    /* remove this item from the file history */
	    m_file_history = dl_list_remove(m_file_history, globals.dvi_name,
					    &dummy_cnt, &dummy_data,
					    equals_filename);
	    m_file_history_length--;
	    filehist_menu_refresh();
	    
	    if (!internal_open_dvi(globals.dvi_name, &errflag, True
#if DELAYED_MKTEXPK
				   , True
#endif
				   )) {
		popup_message(globals.widgets.top_level,
			      MSG_ERR,
			      NULL,
			      "Couldn't reopen `%s': %s.\n"
			      /* "Removing this file from the history." */,
			      globals.dvi_name, get_dvi_error(errflag));
		/* remove this item from the file history */
		m_file_history = dl_list_remove(m_file_history, globals.dvi_name, &dummy_cnt, &dummy_data, equals_filename);
		m_file_history_length--;
		filehist_menu_refresh();
	    }
	    else {
		globals.ev.flags |= EV_NEWPAGE;
	    }
	}
    }
}
