/*
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

#ifndef MESSAGE_WINDOW_H_
#define MESSAGE_WINDOW_H_

#define Xdvi_MESSAGE_DIALOG_NAME	"dialog"
#define Xdvi_MESSAGE_SHELL_NAME		"message_popup"


/*
 * Callback before the message window is closed. It is passed
 * the message window widget ID, and the next XtPointer argument
 * in the message dialog calls.
 */
typedef void (*pre_message_cbT)(Widget w, XtPointer arg);
/*
 * Additional callbacks passed in as argument. It is passed
 * the next XtPointer argument in the message dialog calls.
 */
typedef void (*message_cbT)(XtPointer arg);

/* preferred size of dialogs, only used for Xaw ... not really hints, but
   hard-coded values ;-) */
typedef enum popupMessageSizeHintT_ { SIZE_SMALL, SIZE_MEDIUM, SIZE_LARGE } popupMessageSizeHintT;
typedef enum popupMessageT_ { MSG_QUESTION, MSG_HELP, MSG_INFO, MSG_WARN, MSG_ERR } popupMessageT;

extern Widget popup_message(Widget parent,
			    popupMessageT type,
			    const char *helptext,
			    const char *format, ...);

#if 0	/* This function is currently unused. */
extern Widget popup_message_sized(Widget parent,
				  popupMessageT type,
				  popupMessageSizeHintT size,
				  const char *helptext,
				  const char *format, ...);
#endif

extern Widget positioned_popup_message(Widget parent,
				       popupMessageT type,
				       int x, int y,
				       const char *helptext, const char *format, ...);

extern Widget choice_dialog(Widget parent,
			    popupMessageT type,
			    const char *helptext,
#ifndef MOTIF
			    const char *ret_action_str,
#endif
			    pre_message_cbT pre_cb, XtPointer arg,
			    const char *ok_label, message_cbT ok_cb, XtPointer ok_args,
			    const char *cancel_label, message_cbT cancel_cb, XtPointer cancel_args,
			    const char *format, ...);

#if MOTIF
/* 3 buttons currently only implemented for Motif */
extern Widget choice3_dialog(Widget parent,
			     popupMessageT type,
			     const char *helptext,
			     pre_message_cbT pre_cb, XtPointer arg,
			     const char *yes_label, message_cbT yes_cb, XtPointer yes_args,
			     const char *no_label, message_cbT no_cb, XtPointer no_args,
			     const char *cancel_label, message_cbT cancel_cb, XtPointer cancel_args,
			     const char *format, ...);
#endif

extern Widget choice_dialog_sized(Widget parent,
				  popupMessageT type,
				  popupMessageSizeHintT size,
				  const char *helptext,
#ifndef MOTIF
				  const char *ret_action_str,
#endif
				  pre_message_cbT pre_cb, XtPointer arg,
				  const char *ok_label, message_cbT ok_cb, XtPointer ok_args,
				  const char *cancel_label,  message_cbT cancel_cb, XtPointer cancel_args,
				  const char *format, ...);

extern Widget positioned_choice_dialog(Widget parent,
				       popupMessageT type,
				       int x_pos, int y_pos,
				       const char *helptext,
#ifndef MOTIF
				       const char *ret_action_str,
#endif
				       pre_message_cbT pre_cb, XtPointer arg,
				       const char *ok_label, message_cbT ok_cb, XtPointer ok_args,
				       const char *cancel_label, message_cbT cancel_cb, XtPointer cancel_args,
				       const char *format, ...);
extern void warn_overstrike(void);
extern Boolean raise_message_windows(void);
extern Boolean is_message_window(Widget w);
extern Boolean kill_message_window(Widget w);

#endif /* MESSAGE_WINDOW_H_ */
