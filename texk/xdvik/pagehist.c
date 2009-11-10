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
  Very simple page history (i.e. stack of visited pages) for xdvik
*/

#include "xdvi-config.h"
#include "xdvi.h"
#include "util.h"
#include "dl_list.h"
#include "string-utils.h"
#include "events.h"
#include "dvi-init.h"
#include "statusline.h"
#include "message-window.h"
#include "xm_toolbar.h"
#include "pagehist.h"

/****************************************************************************
 *
 * File-scope globals
 *
 ****************************************************************************/

/* maximum number of items to print before truncating (10 left and right) */
static const int HISTORY_MAX_CONTEXT = 21;

/* list of pages in history */
static struct dl_list *m_page_history = NULL;

/* pointer to the head of the list, which only gets changed when
   truncating the head (when list has reached its max size). */
static struct dl_list *m_page_history_head = NULL;

/* current size of the list */
static int m_page_history_length = 0;

/* current position in the list */
static int m_page_history_currpos = 0;

/* lookup table of filenames visited so far */
static char **m_filename_list = NULL;
static size_t m_filename_size = 0;

/* item in above list */
struct page_history {
    int pageno;
    int file_idx; /* index in file_list */
};

#define DEBUG 0

/****************************************************************************
 *
 * Private functions
 *
 ****************************************************************************/

static void
page_history_show(struct dl_list *head, const struct dl_list *curr)
{
#if DEBUG
    int n;
    for (n = 0; head != NULL; head = head->next, n++) {
	struct page_history *item = (struct page_history *)(head->item);
	if (head == curr) {
	    fprintf(stderr, "item %d: <%d>\n", n, item->pageno);
	}
	else {
	    fprintf(stderr, "item %d: %d\n", n, item->pageno);
	}
    }
#else
    UNUSED(head);
    UNUSED(curr);
#endif /* DEBUG */
}

/* like above, but output goes to the statusline, and it prints only up to HISTORY_MAX_CONTEXT
   items around the current one.
*/
static void
page_history_show_statusline(struct dl_list *head,
			     const struct dl_list *curr,
			     const char *msg)
{
#define HIST_LEN 1024 /* should be ample since statusline is limited to 512 MAX_LEN */

    int file_idx = 0;
    
    int n;
    char history[HIST_LEN];
    char *ptr = history;
    int tot_len = m_page_history_length;
    int curr_pos = m_page_history_currpos;
    int initial_offset = 0;
    int printed_len = 0;
    
#if DEBUG
    fprintf(stderr, "tot_len: %d, curr_pos: %d\n", tot_len, curr_pos);
#endif

    if (!(resource.expert_mode & XPRT_SHOW_STATUSLINE)) {
	/* too distracting for stdout */
	return;
    }
    
    if (head == NULL){
	strcpy(ptr, "Page history empty.");
	ptr += strlen("Page history empty.");
    }
    else {
	strcpy(ptr, "Page history:");
	ptr += strlen("Page history:");
    }

    /* check if we need to truncate at the beginning or end */
    if (tot_len > HISTORY_MAX_CONTEXT) {
	/* need to truncate, try to make first and second chunk around current position of same length */
	int good_pos = HISTORY_MAX_CONTEXT / 2.0 + 0.5;
	while (curr_pos > good_pos /*  && */
	       /*  	       m_page_history_length - m_page_history_currpos > good_pos */) {
#if DEBUG
	    fprintf(stderr, "%d > %d; %d > %d\n", curr_pos, good_pos,
		    m_page_history_length - m_page_history_currpos, good_pos);
#endif
	    curr_pos--;
	    initial_offset++;
	}
#if DEBUG
	fprintf(stderr, "initial offset: %d\n", initial_offset);
#endif
	/* if we're more to the end, adjust good_pos and initial_offset */
	while (good_pos - 1 > m_page_history_length - m_page_history_currpos) {
#if DEBUG
	    fprintf(stderr, "%d > %d\n", m_page_history_length - m_page_history_currpos, good_pos);
#endif
	    initial_offset--;
	    good_pos--;
	}
#if DEBUG
	fprintf(stderr, "initial offset adjusted: %d\n", initial_offset);
#endif
    }

    for (n = 0; head != NULL; head = head->next, n++) {
	struct page_history *item;
	/* skip initial offset, and insert truncation marker at beginning/end */
	if (initial_offset == 1 || printed_len >= HISTORY_MAX_CONTEXT) {
	    strcpy(ptr, " ...");
	    ptr += strlen(" ...");
	    if (printed_len >= HISTORY_MAX_CONTEXT)
		break;
	}
	if (initial_offset > 0) {
	    initial_offset--;
	    continue;
	}

	printed_len++;

	item = (struct page_history *)(head->item);

	/* insert a marker if item is in different file ... */
	if (item->file_idx != file_idx) {
	    if (n > 0) { /* ... but only if we're not at the beginning of the list */
#if 1
		strcpy(ptr, " -");
		ptr += 2;
#else
		char *fname = m_filename_list[item->file_idx];
		char *tmp;
		if ((tmp = strrchr(m_filename_list[item->file_idx], '/')) != NULL)
		    fname = tmp + 1;
		strcpy(ptr, " [");
		ptr += 2;
		strcpy(ptr, fname);
		ptr += strlen(fname);
		strcpy(ptr, "]");
		ptr++;
#endif
	    }
	    file_idx = item->file_idx;
	}
	
	if (head == curr) {
	    xdvi_assert(XDVI_VERSION_INFO, __FILE__, __LINE__,
			m_page_history_currpos == n + 1,
			"%d == %d + 1", m_page_history_currpos, n);
	    sprintf(ptr, " [%d]", item->pageno + 1);
	    ptr += 3 + length_of_int(item->pageno + 1);
	}
	else {
	    sprintf(ptr, " %d", item->pageno + 1);
	    ptr += 1 + length_of_int(item->pageno + 1);
	}
    }
#if DEBUG
    fprintf(stderr, "Statusline string: |%s|; printed len: %d\n", history, printed_len);
#endif
    statusline_info(STATUS_MEDIUM, "%s %s", history, msg ? msg : "");
#undef HIST_LEN
}

static void
goto_location(const char *filename)
{
#if DEBUG
    fprintf(stderr, "going to file %s\n", filename);
#endif
    if (strcmp(globals.dvi_name, filename) != 0) { /* it's a different file */
	Boolean tried_dvi_ext = True;
	char *new_dvi_name;
#if DEBUG
	fprintf(stderr, "different file: |%s|\n", filename);
#endif
	if ((new_dvi_name = open_dvi_file_wrapper(filename, True, False,
						  &tried_dvi_ext, True)) == NULL) {
	    statusline_append(STATUS_MEDIUM,
			      "Re-opening file",
			      "Re-opening file \"%s\" failed!", filename);
#if DEBUG
	    fprintf(stderr, "Re-opening file \"%s\" failed!\n", filename);
#endif
	    page_history_delete(1);
	    return;
	}
	else {
	    dviErrFlagT errflag;
	    if (load_dvi_file(
#if !DELAYED_MKTEXPK
			      True,
#endif
			      &errflag)) {
		set_dvi_name(new_dvi_name);

		globals.ev.flags |= EV_NEWDOC;
		globals.ev.flags |= EV_PAGEHIST_GOTO_PAGE;
#if DEBUG
		fprintf(stderr, "Back to file: \"%s\"\n", globals.dvi_name);
#endif
	    }
	    else { /* re-open old file */
		popup_message(globals.widgets.top_level,
			      MSG_ERR,
			      NULL,
			      "Could not open `%s': %s.\n"
			      /* "Removing this file from the history." */,
			      globals.dvi_name, get_dvi_error(errflag));

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
		}
		else {
		    globals.ev.flags |= EV_NEWPAGE;
		}
		page_history_delete(1);
	    }
	}
    }
    else {
	globals.ev.flags |= EV_PAGEHIST_GOTO_PAGE;
    }
}

/****************************************************************************
 *
 * Exported functions
 *
 ****************************************************************************/

/*
  Move n elements in page history; n == 0 doesn't move,
  n < 0 moves n items back, n > 0 moves n items forward.
*/
void page_history_move(int n)
{
    struct dl_list *pos;
    struct page_history *item;
    const char *msg = NULL;

    page_history_show(m_page_history_head, m_page_history);
    
    if (resource.page_history_size == 0)
	return;
    
    if (m_page_history_head == NULL)
	m_page_history_head = m_page_history;
    
    if (n < 0) { /* move backwards */
	for (pos = NULL; n < 0; n++) {
	    if (m_page_history != NULL)
		pos = m_page_history->prev;
	    if (pos == NULL) {
		xdvi_bell();
		msg = " - at begin of page history.";
		break;
	    }
	    else {
		m_page_history = pos;
		m_page_history_currpos--;
	    }
	}
    }
    else { /* move forward */
	for (pos = NULL; n > 0; n--) {
	    if (m_page_history != NULL)
		pos = m_page_history->next;
	    if (pos == NULL) {
		xdvi_bell();
		msg = " - at end of page history.";
		break;
	    }
	    else {
		m_page_history = pos;
		m_page_history_currpos++;
	    }
	}
    }
    item = (struct page_history *)m_page_history->item;
#if DEBUG
    fprintf(stderr, "going to page %d\n", item->pageno);
#endif
    goto_location(m_filename_list[item->file_idx]);

#if defined(MOTIF) && HAVE_XPM
    tb_set_pagehist_back_sensitivity(m_page_history->prev != NULL);
    tb_set_pagehist_forward_sensitivity(m_page_history->next != NULL);
#endif
    
    page_history_show(m_page_history_head, m_page_history);
    page_history_show_statusline(m_page_history_head, m_page_history, msg);
    page_history_update_toolbar_navigation();
}

int
page_history_get_page(void)
{
    return ((struct page_history *)m_page_history->item)->pageno;
}

/* add page n to the page history */
void page_history_insert(int n)
{
    struct page_history *item = NULL;
    static char *current_filename = NULL;
    static size_t filename_idx = 0; /* index of current filename */

#if DEBUG
    fprintf(stderr, "inserting into history: %d\n", n);
#endif
    page_history_show(m_page_history_head, m_page_history);
    /* do nothing if no history is used */
    if (resource.page_history_size == 0)
	return;

    if (m_page_history_head == NULL)
	m_page_history_head = m_page_history;
    
    item = xmalloc(sizeof *item);
    /* first call, or filename changed -> update file_list */
    if (current_filename == NULL || strcmp(current_filename, globals.dvi_name) != 0) {
	size_t i;
	current_filename = xstrdup(globals.dvi_name);

	for (i = 0; i < m_filename_size; i++) {
#if DEBUG
	    fprintf(stderr, "comparing %d: |%s|%s|\n", i, current_filename, m_filename_list[i]);
#endif
	    if (strcmp(current_filename, m_filename_list[i]) == 0) { /* found */
		filename_idx = i;
		break;
	    }
	}

	if (i >= m_filename_size) { /* not found, insert into file list */
	    m_filename_list = xrealloc(m_filename_list, (m_filename_size + 1) * sizeof *m_filename_list);
	    m_filename_list[m_filename_size] = filename_append_dvi(current_filename);
	    filename_idx = m_filename_size++;
#if DEBUG
	    fprintf(stderr, "NEW file %d: %s\n", filename_idx, current_filename);
#endif
	}
    }
    
#if DEBUG
    fprintf(stderr, "-------- %d >= %d?\n", m_page_history_length, resource.page_history_size - 1);
#endif
    if (m_page_history_length >= resource.page_history_size - 1) { /* truncate history */
	free(m_page_history_head->item);
	m_page_history_head = dl_list_truncate_head(m_page_history_head);
    }
    else {
	m_page_history_length++;
    }
    
    item->pageno = n;
    item->file_idx = filename_idx;
    
#if DEBUG
    fprintf(stderr, "inserting %d\n", item->pageno);
#endif
    m_page_history = dl_list_insert(m_page_history, item);
    m_page_history_currpos++;

#if DEBUG
    fprintf(stderr, "head: %p, curr: %p\n", (void *)m_page_history_head, (void *)m_page_history);
#endif
    page_history_show(m_page_history_head, m_page_history);
    page_history_update_toolbar_navigation();
}

void
page_history_update_toolbar_navigation(void)
{
#if defined(MOTIF) && HAVE_XPM
    tb_set_htex_back_sensitivity(m_page_history->prev != NULL);
    tb_set_htex_forward_sensitivity(m_page_history->next != NULL);
#endif
}

void
page_history_clear(void)
{
    struct dl_list *pos, *curr;

    if (resource.page_history_size == 0)
	return;
    
    m_page_history_head = m_page_history;

    for (curr = m_page_history; curr != NULL && curr->prev != NULL; curr = pos) {
	pos = curr->prev;
	free(curr->item);
	(void)dl_list_remove_item(&curr);
	m_page_history_length--;
	page_history_show(m_page_history_head, m_page_history);
	page_history_show_statusline(m_page_history_head, m_page_history, NULL);
    }

    /*     for (curr = m_page_history; curr != NULL && curr->next != NULL; curr = pos) { */
    /* 	pos = curr->next; */
    /* 	free(curr->item); */
    /* 	(void)dl_list_remove_item(&curr); */
    /* 	m_page_history_length--; */
    /* 	page_history_show(m_page_history_head, m_page_history); */
    /*     } */
    

}

/*
  Delete n elements from the page history.
  If n < 0, delete current and n-1 previous items and move to the item before them.
  If n > 0, delete current and n-1 next items and move to the item after them.
  E.g. (with current position marked by `<>'):

  a b <c> d e
  -> page_history_delete(-2)
  -> <a> d e

  a b <c> d e
  -> page_history_delete(2)
  -> a b <e>

  Prints an error to the statusline if number of deletions exceeds the limits
  of the list.
*/
void page_history_delete(int n)
{
    struct dl_list *pos;
    struct page_history *item;
    const char *msg = NULL;
    
    if (resource.page_history_size == 0)
	return;
    
    if (m_page_history_head == NULL)
	m_page_history_head = m_page_history;

    /*      fprintf(stderr, "deleting items: |%d|\n", n); */
    
    if (n < 0) { /* delete backwards */
	for (pos = NULL; n < 0; n++) {
	    if (m_page_history != NULL)
		pos = m_page_history->prev;
	    if (pos == NULL) {
		xdvi_bell();
		msg = " - at begin of page history.";
		break;
	    }
	    else {
		/* remove item */
		free(m_page_history->item);
		(void)dl_list_remove_item(&m_page_history);
		m_page_history = pos;
		m_page_history_currpos--;
		m_page_history_length--;
	    }
	}
    }
    else { /* delete forward */
	for (pos = NULL; n > 0; n--) {
	    if (m_page_history != NULL)
		pos = m_page_history->next;
	    if (pos == NULL) {
		xdvi_bell();
		msg = " - at end of page history.";
		break;
	    }
	    else {
		/* remove item */
		free(m_page_history->item);
		if (m_page_history->prev == NULL) { /* at head */
		    m_page_history_head = m_page_history = dl_list_truncate_head(m_page_history);
		}
		else {
		    (void)dl_list_remove_item(&m_page_history);
		    m_page_history = pos;
		}
		/* Note: m_page_history_currpos remains unchanged here */
		m_page_history_length--;
	    }
	}
    }
    item = (struct page_history *)m_page_history->item;
#if DEBUG
    fprintf(stderr, "going to page %d\n", item->pageno);
#endif
    goto_location(m_filename_list[item->file_idx]);
    page_history_show(m_page_history_head, m_page_history);
    page_history_show_statusline(m_page_history_head, m_page_history, msg);
}

