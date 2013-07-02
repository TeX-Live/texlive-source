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

#ifndef TOPIC_WINDOW_H_
#define TOPIC_WINDOW_H_

#include "xdvi.h"
#include "version.h"

#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

struct topic_info; /* forward declaration */

struct topic_item {
    /* topic name */
    char *topic;
    /* longer title of topic, displayed as heading in the right window */
    char *title;
    /* the widget (subwindow of the right window) associated with the topic */
    Widget widget;
};

struct topic_info {
    /* the toplevel shell */
    Widget shell;
    /* the topics list */
    Widget topics_list;
    /* the right form, parent of the dummy form which
       contains the right topic-specific window */
    Widget right_form;
    /* the topic label in right window */
    Widget topic_label;
    /* the currently selected widget */
    Widget curr_selected;
    /* callbacks for OK button (may be NULL); it's invoked from within
       the `real' widget callback and is passed a pointer to this(!) struct topic_info */
    void (*ok_callback)(XtPointer arg);
    /* like ok_callback, for the Cancel button */
    void (*cancel_callback)(XtPointer arg);
    /* list of topic_items */
    struct topic_item *items;
    size_t items_size;
    /* additional data the callback may need ... */
    void *data;
};

extern Widget create_topic_window(Widget parent,
				  const char *window_title,
				  const char *widget_name,
				  struct topic_info *info,
				  void (*init_items_func)(struct topic_info *info),
				  /* OK button label (can be NULL) */
				  const char *ok_label,
				  /* Cancel button label (can be NULL) */
				  const char *cancel_label);
extern void select_topic(struct topic_info *info, size_t idx);
#endif /* TOPIC_WINDOW_H_ */
