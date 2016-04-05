/*
 * Copyright (c) 1990-2015  Paul Vojta and the xdvik development team
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

#ifndef EVENTS_H_
#define EVENTS_H_

/*
 *	Flag values and masks for event_flags
 */

#define	EV_IDLE			(1<<0)	/* 	1 - non-event */
#define	EV_CURSOR		(1<<1)	/* 	2 - cursor needs to revert back to ready */
#define	EV_EXPOSE		(1<<2)	/* 	4 - expose occurred somewhere */
#define	EV_MAG_MOVE		(1<<3)	/* 	8 - magnifier moved */
#define	EV_MAG_GONE		(1<<4)	/*     16 - magnifier gone while being drawn */
#define	EV_ACK			(1<<5)	/*     32 - used internally */
#define	EV_SRC			(1<<6)	/*     64 - source special operation is pending */
#define	EV_ANCHOR		(1<<7)	/*    128 - anchor search is pending - should maybe move this up? */
#define	EV_FIND			(1<<8)	/*    256 - string search */
#define	EV_FIND_CANCEL		(1<<9)	/*    512 - string search cancelled */
#define	EV_FILEHIST_GOTO_PAGE	(1<<10)	/*   1024 - get page from file history */
#define	EV_PAGEHIST_INSERT	(1<<11)	/*   2048 - get page from file history */
#define	EV_PAGEHIST_GOTO_PAGE	(1<<12)	/*   4096 - go to page from file history */
#define	EV_NEWPAGE		(1<<13)	/*   8192 - new page requested */
#define	EV_PS_TOGGLE		(1<<14)	/*  16384 - PostScript toggled on or off */
#define	EV_RELOAD		(1<<15)	/*  32768 - reload dvi file */
#define	EV_NEWDOC		(1<<16)	/*  65536 - new dvi file requested */
#define	EV_TERM			(1<<17)	/* 131072 - quit */
#define	EV_MAXPLUS1		(1<<18) /* 262144 - marker for highest element */

#define	EV_GE_IDLE		(EV_MAXPLUS1 - EV_IDLE)
#define	EV_GT_IDLE		(EV_MAXPLUS1 - EV_CURSOR)
#define	EV_GE_CURSOR		(EV_MAXPLUS1 - EV_CURSOR)
#define	EV_GE_EXPOSE		(EV_MAXPLUS1 - EV_EXPOSE)
#define	EV_GE_MAG_MOVE		(EV_MAXPLUS1 - EV_MAG_MOVE)
#define	EV_GE_MAG_GONE		(EV_MAXPLUS1 - EV_MAG_GONE)
#define	EV_GE_ACK		(EV_MAXPLUS1 - EV_ACK)
#define	EV_GE_FIND		(EV_MAXPLUS1 - EV_FIND)
#define	EV_GE_FIND_CANCEL	(EV_MAXPLUS1 - EV_FIND_CANCEL)
#define	EV_GE_NEWPAGE		(EV_MAXPLUS1 - EV_NEWPAGE)
#define	EV_GE_PS_TOGGLE		(EV_MAXPLUS1 - EV_PS_TOGGLE)
#define	EV_GE_NEWDOC		(EV_MAXPLUS1 - EV_NEWDOC)
#define	EV_GE_RELOAD		(EV_MAXPLUS1 - EV_RELOAD)
#define	EV_GE_TERM		(EV_MAXPLUS1 - EV_TERM)

#define	EV_NOWAIT		EV_GE_IDLE


struct xio {
    struct xio *next;	/* link to next in list */
    int fd;		/* file descriptor */
    int xio_events;	/* same as in struct pollfd
			   (can't call it events because poll.h
			   on AIX defines events to something else) */
#if HAVE_POLL
    struct pollfd *pfd;
#endif
    char *(*read_proc) (int, void *);	/* call to read from fd, or NULL */
    void (*write_proc) (int, void *);	/* call to write to fd, or NULL */
    void *data; /* data passed as second argument to read_proc()/write_proc() */
};

struct xchild; /* forward declaration */

typedef void (*childProcT)(int exitval, struct xchild *this);

struct xchild {
    struct xchild *next;	/* link to next in list */
    pid_t pid;		/* pid of process, or 0 */
    char *name;		/* name of process, for printing error message */
    struct xio *io;	/* pointer to i/o structure for reading error msg. */
    void *data;		/* arbitrary data passed to proc */
    int killsig;	/* signal to use when killing it, or 0 */

    /* proc is a pointer to a function to call when the child exits; it will be
     * called with 2 arguments:
     * - The return status of the child
     * - A pointer to the current struct (so that the procedure can free()
     *   (elements of) the struct when needed, or have access to the data field).
     */
    childProcT proc;
};

typedef enum xtimerT_ {
    XTM_DEFAULT = 0,
    XTM_STATUSLINE,
    XTM_HREF
} xtimerT;

struct xtimer {
    struct xtimer *next;	/* link to next in chain */
    struct timeval when;	/* when to call the routine */
    xtimerT type;		/* timer type */
    void (*proc) (struct xtimer *this, void *data);	/* procedure to call */
    void *data;
#if XDVI_XT_TIMER_HACK
    XtTimerCallbackProc xt_proc;	/* additional data for Xt callbacks */
    XtPointer closure;
#endif
};

extern void set_timer(struct xtimer *tp, int ms);
extern void cancel_timer(struct xtimer *tp);

extern int get_num_actions(void);
extern XtActionsRec *get_actions(void);

extern int atopix(const char *);
extern int atopix_signed(const char *);

extern int check_goto_page(int pageno, Boolean insert_into_pagehist);
extern Boolean get_int_arg(String * param, Cardinal *num_params, int *res);
extern Boolean toggle_arg(int arg, String * param, Cardinal *num_params);

extern void clearexpose(struct WindowRec *windowrec, int x, int y, unsigned w,
			unsigned h);
extern void expose(struct WindowRec *windowrec, int x, int y, unsigned int w,
		   unsigned int h);
extern void home(wide_bool);
extern int set_bar_value(Widget bar, int value, int max);

extern void reconfig(void);
extern void redraw(struct WindowRec *windowrec);
extern void handle_resize(Widget, XtPointer, XEvent *, Boolean *);
extern void handle_expose(Widget, XtPointer, XEvent *, Boolean *);
extern void handle_property_change(Widget, XtPointer, XEvent *, Boolean *);
extern void handle_command(Widget widget, XtPointer client_data,
			   XtPointer call_data);
extern void showmessage(const char *);
extern void set_chld(struct xchild *);
extern void clear_chld(struct xchild *);
extern void set_io(struct xio *);
extern void clear_io(struct xio *);
extern void xdvi_exit(int);
extern unsigned int read_events(unsigned int);

typedef void (*home_proc) (wide_bool);
extern void goto_page(int page, home_proc proc, Boolean force);

extern void setup_sigalarm(void);
extern void setup_signal_handlers(Boolean early);
extern int shrink_to_fit(void);
extern void do_pages(void);
extern void do_set_density(double newgamma, Boolean force, Boolean update_resource);
extern void do_toggle_color(Boolean update_resource);

extern void Act_mouse_modes(Widget w, XEvent *event, String *params, Cardinal *num_params);

#ifdef PS
extern void Act_set_ps(Widget w, XEvent *event, String *params, Cardinal *num_params);
#endif

#ifdef PS_GS
extern void Act_set_gs_alpha(Widget w, XEvent *event, String *params, Cardinal *num_params);
#endif

extern void Act_recent_files(Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void Act_htex_back(Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void Act_htex_forward(Widget w, XEvent *event, String *params, Cardinal *num_params);

extern void Act_set_keep_flag(Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void Act_back_page(Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void Act_forward_page(Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void do_set_shrinkfactor(int arg, Boolean set_resource);
extern void Act_set_shrink_factor(Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void Act_shrink_to_dpi(Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void Act_set_expert_mode(Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void Act_use_tex_pages(Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void Act_ruler_mode(Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void Act_set_expert_mode(Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void Act_switch_mode(Widget w, XEvent *event, String *params, Cardinal *num_params);

#if defined(NEW_MENU_CREATION) || defined(MOTIF)
extern void Act_set_papersize(Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void Act_set_paper_landscape(Widget w, XEvent *event, String *params, Cardinal *num_params);
#endif /* NEW_MENU_CREATION */

extern void Act_scroll_list_up(Widget w, XEvent *event, String *params,
  Cardinal *num_params);
extern void Act_scroll_list_down(Widget w, XEvent *event, String *params,
  Cardinal *num_params);
extern void Act_pagehistory_clear(Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void Act_pagehistory_back(Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void Act_pagehistory_forward(Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void Act_pagehistory_delete_backward(Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void Act_pagehistory_delete_forward(Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void Act_magnifier(Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void Act_ruler(Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void Act_text_selection(Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void Act_switch_magnifier_units(Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void Act_href(Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void Act_href_newwindow(Widget w, XEvent *event, String *params, Cardinal *num_params);

extern void null_mouse(XEvent *ignored);
extern void text_motion(XEvent *event);

typedef enum { TEXT_SEL_MOVE, TEXT_SEL_CLEAR, TEXT_SEL_REDRAW, TEXT_SEL_ERASE } textSelectionT;
extern void text_change_region(textSelectionT mode, XEvent *event);

extern void text_selection_start(XEvent *event);

#if COLOR

extern void update_expert_mode(void);

extern Boolean check_resource_expert(void *val, const char *param);
extern Boolean check_paper_landscape(void *val, const char *param);
extern Boolean check_papersize(void *val, const char *param);
extern Boolean check_toggle(void *val, const char *param);
extern Boolean check_int(void *val, const char *param);

#endif /* COLOR */


struct xdvi_action {
    XtActionProc proc;
    String command;
    String *params;
    Cardinal num_params;
    struct xdvi_action *next;
};

extern Boolean compile_action(const char *str, struct xdvi_action **);
extern void watch_file_cb(XtPointer client_data, XtIntervalId * id);
extern void redraw_page(void);

#ifdef USE_PANNER
extern void handle_x_scroll(Widget w, XtPointer closure, XEvent *ev, Boolean *cont);
extern void handle_y_scroll(Widget w, XtPointer closure, XEvent *ev, Boolean *cont);
#endif
extern void xdvi_exit_callback(Widget w, XtPointer client_data, XtPointer call_data);

#endif /* EVENTS_H_ */
