#include "xdvi-config.h"

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>


#if !MOTIF
# include <X11/Xaw/Form.h>
#endif

#include "x_util.h"
#include "string-utils.h"
#include "util.h"
#include "statusline.h"
#include "message-window.h"
#include "events.h" /* for set_bar_value() */

#define DEBUG_SCROLL_IF_NEEDED 0

Boolean do_autoscroll = False;


/* Center window wa over window wb */
void
center_window(Widget wa, Widget wb)
{
    Position x, y;
    Dimension w1, h1, w2, h2;

    if (!XtIsRealized(wa) || !XtIsRealized(wb))
	return;
    
    XtVaGetValues(wa,
		  XtNwidth, &w1,
		  XtNheight, &h1,
		  NULL);
    XtVaGetValues(wb,
		  XtNwidth, &w2,
		  XtNheight, &h2,
		  XtNx, &x,
		  XtNy, &y,
		  NULL);
    XtVaSetValues(wa, XtNx, x + (w2 - w1) / 2,
		  XtNy, y + (h2 - h1) / 2,
		  NULL);
}

/* Position window w at coordinates x, y */
void
position_window(Widget w, Position x, Position y)
{
    if (!XtIsRealized(w))
	return;

    TRACE_GUI((stderr, "positioning %ld at %d, %d", (unsigned long)w, x, y));
    XtVaSetValues(w, XtNx, x, XtNy, y, NULL);
}

/*
  Used for the hyperref and forward search markers: scroll the page
  to make the marker visible.
*/
void
scroll_page_if_needed(int x_min, int x_max, int y_min, int y_max)
{
    Position drawing_x, drawing_y, drawing_h, clip_x, clip_y, clip_h, clip_w;
    int test_scroll, need_v_scroll = 0, need_h_scroll = 0;

    if (!do_autoscroll)
	return;
    
    XtVaGetValues(globals.widgets.clip_widget,
		  XtNx, &clip_x, XtNy, &clip_y,
		  XtNheight, &clip_h, XtNwidth, &clip_w,
		  NULL);
    XtVaGetValues(globals.widgets.draw_widget,
		  XtNx, &drawing_x, XtNy, &drawing_y,
		  XtNheight, &drawing_h,
		  NULL);

#if DEBUG_SCROLL_IF_NEEDED
    fprintf(stderr, "y: %d, drawing_y: %d, clip_h: %d\n", y, drawing_y, clip_h);
#endif
    /* check if we need to scroll vertically; first, down for y_min */
    test_scroll = y_min + drawing_y - clip_h;
    if ((resource.expert_mode & XPRT_SHOW_STATUSLINE) != 0)
	test_scroll += get_statusline_height();

    TRACE_SRC((stderr, "test_scroll vertically: %d", test_scroll));
#if DEBUG_SCROLL_IF_NEEDED
    fprintf(stderr, "%d + %d > %d?\n", drawing_y, y_min, clip_h);
#endif
    if (test_scroll > 0) { /* need to scroll down? */
	need_v_scroll = test_scroll;
	TRACE_SRC((stderr, "need_v_scroll down: %d", need_v_scroll));
    }
    else if (abs(drawing_y) + 2 > y_max) { /* need to scroll up? */
	need_v_scroll = -((abs(drawing_y) - y_max) + 1);
	TRACE_SRC((stderr, "need_v_scroll up: %d (%d > %d; %d)", need_v_scroll, abs(drawing_y), y_max, clip_y));
    }

    /* check if we need to scroll horizontally; x_min < 0 blocks this
       (e.g. for hyperref, where we don't want it) */
    if (x_min >= 0) {
	test_scroll = x_min + drawing_x - clip_w + 1;
	TRACE_SRC((stderr, "test_scroll horizontally: %d", test_scroll));
	
	if (test_scroll > 0) {
	    /* need to scroll to right (i.e. make stuff on right-hand side visible)? */
	    need_h_scroll = test_scroll;
	    TRACE_SRC((stderr, "need_h_scroll right: %d", need_h_scroll));
	}
	else if (abs(drawing_x) > x_max) {
	    /* need to scroll to left (i.e. make stuff on left-hand side visible)? */
	    need_h_scroll =  -(abs(drawing_x) - x_max);
	    TRACE_SRC((stderr, "need_h_scroll left: %d", need_h_scroll));
	}
    }

    /* FIXME: should we not scroll if keep_flag is active? */
    if (need_v_scroll != 0 && globals.widgets.y_bar != NULL) {
#ifdef MOTIF
	/* need to add new value to current one */
	XtVaGetValues(globals.widgets.y_bar, XmNvalue, &test_scroll, NULL);
	(void)set_bar_value(globals.widgets.y_bar, test_scroll + need_v_scroll, (int)(globals.page.h - mane.height));
#else
	XtCallCallbacks(globals.widgets.y_bar, XtNscrollProc, cast_int_to_XtPointer(need_v_scroll));
#endif
    }

    if (need_h_scroll != 0 && globals.widgets.x_bar != NULL) {
#ifdef MOTIF
	/* need to add new value to current one */
	XtVaGetValues(globals.widgets.x_bar, XmNvalue, &test_scroll, NULL);
	(void)set_bar_value(globals.widgets.x_bar, test_scroll + need_h_scroll, (int)(globals.page.w - mane.width));
#else
	XtCallCallbacks(globals.widgets.x_bar, XtNscrollProc, cast_int_to_XtPointer(need_h_scroll));
#endif
    }

    do_autoscroll = False;
}

void adjust_width(Widget a, Widget b)
{
    Dimension w1, w2;
    
    XtVaGetValues(a, XtNwidth, &w1, NULL);
    XtVaGetValues(b, XtNwidth, &w2, NULL);
    
    if (w1 < w2)
	XtVaSetValues(a, XtNwidth, w2, NULL);
    else if (w2 > w1)
	XtVaSetValues(b, XtNwidth, w1, NULL);
}

/* change a GC and return it, or create one if it doesn't exist yet */
GC
set_or_make_gc(GC gc, int function, Pixel fg, Pixel bg)
{
    XGCValues values;
    values.function = function;
    values.foreground = fg;
    values.background = bg;

    /* Since print is in round dots we make drawings as "smooth" as possible. */
    values.cap_style = CapRound;
    values.join_style = JoinRound;

    if (gc != NULL)
	XChangeGC(DISP, gc, GCFunction | GCForeground | GCBackground
		  | GCCapStyle | GCJoinStyle, &values);
    else
	gc = XCreateGC(DISP, XtWindow(globals.widgets.top_level), GCFunction | GCForeground | GCBackground
		       | GCCapStyle | GCJoinStyle, &values);

    return gc;
}

/*
 *	Atom handling.
 */

static char *atom_names[] = {
    "XDVI_WINDOWS",
    "DVI_NAME",
    "SRC_GOTO",
    "RELOAD",
    "NEWDOC",
    "NEWPAGE",
    "RAISE",
    "FIND_STRING",
    "REREAD_PREFS"
};
static Atom atoms[XtNumber(atom_names)];

/*
 *	On 64-bit platforms, XGetWindowProperty and related functions convert
 *	properties with format=32 to arrays of longs.  This function keeps that
 *	convention.
 *	The return value is the total number of bytes in the buffer.
 */

#if defined(WORD64) || defined(LONG64)
# define LONG_CONV_64(bytes, format)	((bytes) << ((format) >> 5))
#else
# define LONG_CONV_64(bytes, format)	(bytes)
#endif

size_t
property_get_data(Window w, Atom a, char **ret_buf,
		  int (*x_get_property)(Display *, Window, Atom, long,
					long, Bool, Atom,
					Atom *, int *, unsigned long *,
					unsigned long *, unsigned char **))
{
    /* all of these are in 8-bit units */
    unsigned long byte_offset = 0;
    Atom type_ret;
    int format_ret;
    unsigned long nitems_ret;
    unsigned long bytes_after_ret = 0;
    unsigned char *prop_ret = NULL;
    
    /*
     * buffer for collecting returned data; this is static to
     * avoid expensive malloc()s at every call (which is often!)
     */
    static unsigned char *buffer = NULL;
    static size_t buffer_len = 0;

    while (x_get_property(DISP, w,
			  a, byte_offset / 4, (bytes_after_ret + 3) / 4, False,
			  a, &type_ret, &format_ret, &nitems_ret,
			  &bytes_after_ret, &prop_ret)
	   == Success) {

	if (type_ret != a || format_ret == 0)
	    break;

	nitems_ret *= (format_ret / 8);	/* convert to bytes */

	if (LONG_CONV_64(byte_offset + nitems_ret, format_ret) >= buffer_len) {
	    buffer_len += 256
			  * ((LONG_CONV_64(byte_offset + nitems_ret, format_ret)
			      - buffer_len) / 256 + 1);
	    buffer = (buffer == NULL ? xmalloc(buffer_len)
				     : xrealloc(buffer, buffer_len));
	}

	/* the +1 captures the extra '\0' that Xlib puts after the end.  */
	memcpy(buffer + LONG_CONV_64(byte_offset, format_ret), prop_ret,
	       LONG_CONV_64(nitems_ret, format_ret) + 1);
	byte_offset += nitems_ret;

	XFree(prop_ret);
	prop_ret = NULL;

	if (bytes_after_ret == 0)	/* got all data */
	    break;
    }

    if (prop_ret != NULL)
	XFree(prop_ret);

    *ret_buf = (char *)buffer;
    return LONG_CONV_64(byte_offset, format_ret);
}

static size_t
property_get_window_list(long **window_list)
{
    size_t len = property_get_data(DefaultRootWindow(DISP),
				   atom_xdvi_windows(), (char **) window_list,
				   XGetWindowProperty);
    if (len == 0) {
	TRACE_CLIENT((stderr, "No \"xdvi windows\" property found"));
	return 0;
    }
    
    if (len % sizeof(long) != 0) {
	TRACE_CLIENT((stderr,
		"\"XDVI_WINDOWS\" property had incorrect size; deleting it."));
	XDeleteProperty(DISP, DefaultRootWindow(DISP), atom_xdvi_windows());
	return 0;
    }

    return len / sizeof (long);
}

/**
 **	set_dvi_property sets the appropriate property for the main
 **	window (used in source special handoff).
 **/

void
set_dvi_property(void)
{
    XChangeProperty(DISP, XtWindow(globals.widgets.top_level), atom_dvi_file(), atom_dvi_file(),
		    8, PropModeReplace, (unsigned char *)dvi_property, dvi_property_length);
}


/*
 * Delete all occurrences of window w from the window list property.
 * Then, if `prepend' is true, prepend the window ID to the existing list.
 */
void
update_window_property(Window w, Boolean prepend)
{
    long *wlist;
    size_t wlist_len;
    long *wlist_end;
    long *wp;
#if 0
    int i;
#endif /* 0 */

    /* this allocates wlist */
    if ((wlist_len = property_get_window_list(&wlist)) == 0)
	return;

    /* Loop over list of windows.  */
    wlist_end = wlist + wlist_len;

#if 0
    for (i = 0, wp = wlist; wp < wlist_end; ++wp, ++i) {
	fprintf(stderr, "WIN %d: %08lx; len: %d\n", i, *wp, wlist_len);
    }
#endif /* 0 */
    
    for (wp = wlist; wp < wlist_end; ++wp) {
	if (*wp == w) { /* match, remove our ID */
	    --wlist_len;
	    --wlist_end;
	    memmove(wp, wp + 1, (wlist_end - wp) * sizeof (long));
	    --wp; /* new item is now at wp; don't skip it in next iteration */
	}
    }
    
    if (prepend) { /* add our ID again to front */
	/* Note: no need to realloc wlist, since the original length
	   was sufficient for all elements.
	*/
	memmove(wlist + 1, wlist, wlist_len * sizeof (long));
	++wlist_len;
	*wlist = w;
    }
	    
    if (wlist_len == 0)
	XDeleteProperty(DISP, DefaultRootWindow(DISP),
			atom_xdvi_windows());
    else
	XChangeProperty(DISP, DefaultRootWindow(DISP),
			atom_xdvi_windows(), atom_xdvi_windows(), 32,
			PropModeReplace, (unsigned char *)wlist, wlist_len);
    
    XFlush(DISP);
}

void
property_initialize(void)
{
    size_t i;
    
#if XlibSpecificationRelease >= 6
    if (!XInternAtoms(DISP, atom_names, XtNumber(atom_names), False, atoms))
	XDVI_FATAL((stderr, "XtInternAtoms failed."));
#else
    for (i = 0; i < XtNumber(atom_names); i++) {
	if ((atoms[i] = XInternAtom(DISP, atom_names[i], False)) == None)
	    XDVI_FATAL((stderr, "XtInternAtoms failed."));
    }
#endif

    if (globals.debug & DBG_CLIENT) {
	for (i = 0; i < XtNumber(atom_names); i++)
	    TRACE_CLIENT((stderr, "Atom(%s) = %lu", atom_names[i], atoms[i]));
    }
}


Atom atom_xdvi_windows(void)
{
    return atoms[0];
}

Atom atom_dvi_file(void)
{
    return atoms[1];
}

Atom atom_src_goto(void)
{
    return atoms[2];
}

Atom atom_reload(void)
{
    return atoms[3];
}

Atom atom_newdoc(void)
{
    return atoms[4];
}

Atom atom_newpage(void)
{
    return atoms[5];
}

Atom atom_raise(void)
{
    return atoms[6];
}

Atom atom_find_string(void)
{
    return atoms[7];
}

Atom atom_reread_prefs(void)
{
    return atoms[8];
}

/*
 * Syntesize a mouse-1 down event for the Widget w passed as second argument.
 * This can be, e.g. a dialog button, or some other window.
 */
void
synthesize_event(XEvent *ev, Widget w)
{
    memset(ev, 0, sizeof(XButtonPressedEvent));
    ev->type = ButtonPress;
    ev->xbutton.serial = 1;
    ev->xbutton.send_event = True;
    ev->xbutton.button = 1;
    ev->xbutton.display = XtDisplayOfObject(w);
    ev->xbutton.window = XtWindowOfObject(w);
}

#ifdef MOTIF
int xm_get_height(Widget w)
{
    int ret_h = 0;
    static Dimension h0, h1, h2;
    static Arg args[] = {
	{XmNheight, (XtArgVal) &h0},
	{XmNmarginHeight, (XtArgVal) &h1},
	{XmNshadowThickness, (XtArgVal) &h2},
    };
    ASSERT(w != NULL, "widget in xm_get_width mustn't be NULL!");

    XtGetValues(w, args, XtNumber(args));
    ret_h = h0 + 2 * h1 + 2 * h2;
    TRACE_GUI((stderr, "xm_get_height: %d", ret_h));
    return ret_h;
}

int xm_get_width(Widget w)
{
    int ret_w = 0;
    static Arg args = { XtNwidth, (XtArgVal)0 };
    ASSERT(w != NULL, "widget in xm_get_width mustn't be NULL!");

    args.value = (XtArgVal)&ret_w;
    XtGetValues(w, &args, 1);
    TRACE_GUI((stderr, "xm_get_width: %d", ret_w));
    return ret_w;
}

/*
 * Get pixel from color `colorname'. We try to keep this as
 * high-level as possible, with simple black as fallback.
 */
void
str_to_pixel(Widget w, const char *colorname, Pixel *ret)
{
    XrmValue from, to;

    from.addr = (char *)colorname;
    from.size = strlen(from.addr) + 1;
    to.addr = (XtPointer)ret;
    to.size = sizeof(Pixel);
    if (!XtConvertAndStore(w, XtRString, &from, XtRPixel, &to)) {
	fprintf(stderr, "String to pixel conversion failed for %s!\n", colorname);
	from.addr = (char *)"black";
	from.size = strlen(from.addr) + 1;
	to.addr = (XtPointer)ret;
	to.size = sizeof(Pixel);
	XtConvertAndStore(w, XtRString, &from, XtRPixel, &to);
    }
}

/*
 * Convert pixel value `pix' to color name passed as str,
 * of length len, or `black' if conversion failed.
 * -- FIXME: This is broken!!!
 */
void
pixel_to_str(Widget w, Pixel pix, char *str, size_t len)
{
    XrmValue from, to;

    from.addr = (XtPointer)&pix;
    from.size = sizeof(Pixel);
    to.addr = str;
    to.size = len;

    if (!XtConvertAndStore(w, XtRString, &from, XtRPixel, &to)) {
	fprintf(stderr, "Pixel to String conversion failed for %ld!\n", pix);
	sprintf(str, "white");
    }
}

/* Free color, and initialize it anew with pixel value `pix' */
void
pixel_to_color(Pixel pix, XColor *color, Display *display, Colormap colormap)
{
    XColor test;
    test.pixel = pix;
    XQueryColor(display, colormap, &test);

    color->red = test.red;
    color->green = test.green;
    color->blue = test.blue;
    
    if (!XAllocColor(display, colormap, color)) {
	fprintf(stderr, "Fatal: Couldn't XAllocColor!");
	exit(1);
    }
}
#endif /* MOTIF */

/* helper routine for get_matching_parent() */
static Widget
matches_parent(Widget w, const char *name)
{
    for (; w != NULL; w = XtParent(w)) {
	char *ptr;
	ptr = XtName(w);
	TRACE_GUI((stderr, "parent: %s", ptr == NULL ? "<NULL>" : ptr));
	if (ptr != NULL && strcmp(ptr, name) == 0) { /* found */
	    TRACE_GUI((stderr, "match!"));
	    break;
	}
    }
    TRACE_GUI((stderr, "returning: %p (0x%lx)", (void *)w, w ? XtWindow(w) : 0));
    return w;
}

/* Traverse widget hieararchy upwards until a widget matches
   a name in the (NULL-terminated) list fmt, or Widget d (`default')
   if none is found.
*/
Widget
get_matching_parent(Widget w, Widget d, const char *fmt, ...)
{
    Widget parent = d;
    const char *str = fmt;

    va_list argp;
    va_start(argp, fmt);

    TRACE_GUI((stderr, "get_matching_parent of %p (0x%lx)", (void *)w, XtWindow(w)));
    while (str != NULL) {
	Widget p;
	if ((p = matches_parent(w, str)) != NULL) {
	    parent = p;
	    break;
	}
	str = va_arg(argp, char *);
    }
	
    va_end(argp);

    return parent;
}

void
adjust_width_to_max(Widget w, ...)
{
    Dimension max = 0;
    Widget w1 = w;
    
    va_list argp;
    va_start(argp, w);

    /* get maximum width */
    while (w1 != NULL) {
	Dimension curr;
	XtVaGetValues(w1, XtNwidth, &curr, NULL);
	if (curr > max)
	    max = curr;
	w1 = va_arg(argp, Widget);
    }
    va_end(argp);

    /* set maximum width */
    va_start(argp, w);
    w1 = w;
    while (w1 != NULL) {
	XtVaSetValues(w1, XtNwidth, max, NULL);
	w1 = va_arg(argp, Widget);
    }
    va_end(argp);
}

/* Return True if p is a parent of widget w, stopping (and returning FALSE)
   if s is reached (which should be the toplevel window of the hierarchy).
*/
Boolean
widget_is_parent(Widget w, Widget p, Widget s)
{
    Widget curr = XtParent(w);
    while (curr != NULL && curr != s) {
	fprintf(stderr, "Comparing: %p - %p\n", (void *)curr, (void *)p);
	if (curr == p)
	    return True;
	curr = XtParent(curr);
    }
    return False;
}

/*
  Adjust height in a NULL-terminated list of widgets. For Motif,
  this works better than the following adjust_vertically().
*/
void
adjust_heights(Widget w, ...)
{
    va_list ap;
    Widget curr;
    Dimension h, max;

    ASSERT(w != NULL, "Must have at least one element in va_list for adjust_heights!");

#if MOTIF
#define HEIGHT XmNheight
#else
#define HEIGHT XtNheight
#endif
    
    /* initialize maximum */
    XtVaGetValues(w, HEIGHT, &max, NULL);

    /* get maximum height */
    va_start(ap, w);
    while ((curr = va_arg(ap, Widget)) != NULL) {
	XtVaGetValues(curr, HEIGHT, &h, NULL);
	if (h > max)
	    max = h;
    }
    va_end(ap);

    /* set maximum height for all widgets */
    XtVaSetValues(w, HEIGHT, max, NULL);
    
    va_start(ap, w);
    while ((curr = va_arg(ap, Widget)) != NULL)
	XtVaSetValues(curr, HEIGHT, max, NULL);
    va_end(ap);

#undef HEIGHT

}

/*
  Adjust height in a NULL-terminated list of widgets. For Motif,
  this works better than the following adjust_vertically().
*/
void
adjust_heights_min(Widget w, ...)
{
    va_list ap;
    Widget curr;
    Dimension h, min;

    ASSERT(w != NULL, "Must have at least one element in va_list for adjust_heights!");

#if MOTIF
#define HEIGHT XmNheight
#else
#define HEIGHT XtNheight
#endif
    
    /* initialize minimum */
    XtVaGetValues(w, HEIGHT, &min, NULL);

    /* get minimum height */
    va_start(ap, w);
    while ((curr = va_arg(ap, Widget)) != NULL) {
	XtVaGetValues(curr, HEIGHT, &h, NULL);
	if (h < min)
	    min = h;
    }
    va_end(ap);

    /* set maximum height for all widgets */
    XtVaSetValues(w, HEIGHT, min, NULL);
    
    va_start(ap, w);
    while ((curr = va_arg(ap, Widget)) != NULL)
	XtVaSetValues(curr, HEIGHT, min, NULL);
    va_end(ap);

#undef HEIGHT

}

/* adjust two widgets vertically */
void
adjust_vertically(Widget w1, Widget w2, int default_dist)
{
    Dimension h1, h2;
#if MOTIF
    XtVaGetValues(w1, XmNheight, &h1, NULL);
    XtVaGetValues(w2, XmNheight, &h2, NULL);
    XtVaSetValues(w1, XmNtopOffset, default_dist + (h2 - h1) / 2, NULL);
    /*     XtVaSetValues(w2, XmNtopOffset, default_dist + (h2 - h1) / 2, NULL); */
#else
    XtVaGetValues(w1, XtNheight, &h1, NULL);
    XtVaGetValues(w2, XtNheight, &h2, NULL);
    XtVaSetValues(w1, XtNvertDistance, default_dist + (h2 - h1) / 2, NULL);
    XtVaSetValues(w2, XtNvertDistance, default_dist + (h2 - h1) / 2, NULL);
#endif /* MOTIF */
}


/*
 * This is a hack to block further processing of some events on widgets:
 * Add as an event handler for all mouse/key events for a specific widget.
 */
void
block_event_callback(Widget w, XtPointer client_data,
		     XEvent *ev, Boolean *cont)
{
    UNUSED(w);
    UNUSED(client_data);
    UNUSED(ev);
    
    /* Don't propagate this event further down... */
    *cont = False;

    return;
}


/* Get a widget with `name' somewhere in the widget hierarchy below `parent'
   (matching is done against `*name') and return it in `ret'.
   If `report_error' is True, a warning message is popped up if the widget isn't found.
*/
Boolean
get_widget_by_name(Widget *ret, Widget parent, const char *name, Boolean report_error)
{
    char buf[1024];
    Widget test;

    /*      if (parent == 0 || !XtIsManaged(parent)) { */
    /*  	fprintf(stderr, "Widget %p not managed!\n", parent); */
    /*  	return False; */
    /*      } */
    
    if (strlen(name) > 1023) {
	popup_message(globals.widgets.top_level,
		      MSG_ERR,
		      REPORT_XDVI_BUG_TEMPLATE,
		      "Widget name `%s' too long, couldn't get parent", name);
	return False;
    }

    buf[0] = '*'; /* add wildcard to also match paths */
    strcpy(buf + 1, name);

    if ((test = XtNameToWidget(parent, buf)) != NULL) {
	*ret = test;
	return True;
    }
    else {
	if (report_error)
	    popup_message(globals.widgets.top_level,
			  MSG_ERR,
			  REPORT_XDVI_BUG_TEMPLATE,
			  "XtNameToWidget failed for `%s', parent `%s'", name, XtName(parent));
	return False;
    }
}

void
unexpected_widget_in_callback(Widget w, const char *callback)
{
    ASSERT(w != NULL, "Widget mustn't be NULL!");
    popup_message(globals.widgets.top_level,
		  MSG_ERR,
		  REPORT_XDVI_BUG_TEMPLATE,
		  "Unexpected widget `%s' in callback `%s'",
		  XtName(w), callback);
}

static XrmDatabase m_user_db = NULL;

/*
 * Merge the resource `name' with a value specified by `fmt' into
 * the database `db' (if db == NULL, use m_user_db).
 */
void
store_preference(XrmDatabase *db, const char *name, const char *fmt, ...)
{
    size_t offset = strlen("xdvi.");
    size_t name_len = strlen(name);
    char *name_buf = xmalloc(name_len + offset + 1);
    char *buf = NULL;
    XrmDatabase tmp_db = NULL;
    
    if (db == NULL)
	db = &m_user_db;

    XDVI_GET_STRING_ARGP(buf, fmt);

    memcpy(name_buf, "xdvi.", offset);
    strcpy(name_buf + offset, name);

    TRACE_GUI((stderr, "storing resource: `%s: %s'", name_buf, buf));
    XrmPutStringResource(&tmp_db, name_buf, buf);
    XrmMergeDatabases(tmp_db, db);
   
    free(buf);
    free(name_buf);
}

void
merge_into_user_db(XrmDatabase db)
{
    XrmMergeDatabases(db, &m_user_db);
}

const char *const xdvirc_filename = ".xdvirc";
const char *const xdvirc_signature_line = "!!! ~/.xdvirc, used by xdvi(1) to save user preferences.\n";
const char *const xdvirc_header = ""
"!!!\n"
"!!! Do not edit this file, it will be overwritten by xdvi.\n"
"!!! This file contains resources that have been set via the\n"
"!!! menus/dialogs. The contents of this file will override\n"
"!!! the entries in your ~/.Xdefaults file (but not the command\n"
"!!! line arguments passed to xdvi). Remove this file\n"
"!!! if you want to get rid of all these customizations,\n"
"!!! or start xdvi with the `-q' option to ignore this file.\n"
"!!!\n";

static char *
get_xdvirc_path(const char *basename)
{
    const char *homedir;
    size_t len;
    char *str;

    if (basename == NULL)
	return NULL;

    homedir = getenv("HOME");
    len = strlen(homedir) + strlen(basename) + 2; /* 1 more for '/' */
    str = xmalloc(len);
    
    sprintf(str, "%s/%s", homedir, basename);
    return str;
}

static void
save_geometry(void)
{
    int x_off, y_off;
    Dimension w, h;
    Window dummy;
    /*     char *geom_str; */
    
    (void)XTranslateCoordinates(DISP, XtWindow(globals.widgets.top_level),
				RootWindowOfScreen(SCRN),
				0, 0,
				&x_off, &y_off,
				&dummy);
    XtVaGetValues(globals.widgets.top_level,
		  XtNwidth, &w,
		  XtNheight, &h,
#ifdef MOTIF
		  /* 		  XmNgeometry, &geom_str, */
#endif
		  NULL);
    TRACE_GUI((stderr, "geometry: %dx%d+%d+%d", w, h, x_off, y_off));

    store_preference(NULL, "windowSize", "%dx%d", w, h);
}

/* Save user preferences to ~/.xdvirc. If `backup_only' is True,
   it only writes to ~/.xdvirc.tmp and does not remove this temporary
   file (this is used for synchronization between several xdvi instances).
*/
Boolean
save_user_preferences(Boolean full_save)
{
    char testbuf[1024];
    char *xdvirc_name;
    char *tmpname;
    FILE *from_fp, *to_fp;
    int fd;

    if (resource.no_init_file
	|| m_user_db == NULL) /* nothing to do */
	return True;
    
    if (resource.remember_windowsize)
	save_geometry();
    
    xdvirc_name = get_xdvirc_path(xdvirc_filename);

    if ((to_fp = fopen(xdvirc_name, "r")) != NULL) {
	TRACE_GUI((stderr, "~/.xdvirc exists, checking file contents ..."));
	if (fgets(testbuf, 1024, to_fp) != NULL &&
	    memcmp(testbuf, xdvirc_signature_line, strlen(xdvirc_signature_line)) != 0) {
	    popup_message(globals.widgets.top_level,
			  MSG_WARN,
			  "Xdvi uses the file ~/.xdvirc to save the preferences set via "
			  "the menu bar or the preferences dialog (in the Motif version only). "
			  "To avoid overwriting files created by the user, the first line of the "
			  "file is compared with a special signature line. If that signature line "
			  "is not found, the preferences won't be written. Your file doesn't seem "
			  "to contain that signature line. You should move the file to a safe location, "
			  "and then try to quit xdvi again.",
			  /* message */
			  "The file `%s' was apparently not written by xdvi(k). "
			  "Please move or delete this file first, then try to exit xdvi again. ",
			  xdvirc_name);
	    return False;
	}
	fclose(to_fp);
    }

    /* don't use xdvi_temp_fd here, since XrmPutFileDatabase()
       closes the FILE*, creating a temp race */
    tmpname = xstrdup(xdvirc_name);
    tmpname = xstrcat(tmpname, ".tmp");

    /* since XrmPutFileDatabase doesn't give a useful error message if it fails,
       check that creating the file works beforehand. The file is created with 0600 permissions. */
    if ((fd = try_open_mode(tmpname, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) < 0) {
	XDVI_ERROR((stderr, "Could not save preferences!\nOpening %s for writing failed: %s", tmpname, strerror(errno)));
	return True;
    }
    close(fd);
    
    XrmPutFileDatabase(m_user_db, tmpname);

    if (full_save) {
	if ((from_fp = try_fopen(tmpname, "r")) == NULL) {
	    XDVI_ERROR((stderr, "Could not save preferences!\nOpening %s for reading failed: %s", tmpname, strerror(errno)));
	    return True;
	}
	
	/* again, create the file with 600 permissions */
	if ((fd = try_open_mode(xdvirc_name, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) < 0) {
	    XDVI_ERROR((stderr, "Could not save preferences!\nOpening %s for writing failed: %s", xdvirc_name, strerror(errno)));
	    return True;
	}
	
	if ((to_fp = fdopen(fd, "w")) == NULL) {
	    XDVI_ERROR((stderr, "Could not save preferences!\nfdopen for %s for writing failed: %s", xdvirc_name, strerror(errno)));
	    return True;
	}
	
	if (fputs(xdvirc_signature_line, to_fp) == EOF
	    || fputs(xdvirc_header, to_fp) == EOF
	    || !copy_fp(from_fp, to_fp)) {
	    XDVI_ERROR((stderr, "Could not save preferences!\nError writing to %s: %s", xdvirc_name, strerror(errno)));
	}

	fclose(from_fp);
	fclose(to_fp);
    }
    
    free(xdvirc_name);

    if (full_save)
	unlink(tmpname);
    free(tmpname);

    return True;
}

/*
 * Read the user preferences from xdvirc_filename and merge them into the
 * current resource database *and* into m_user_db so that all of them are
 * saved again when xdvi exits.
 */
void
read_user_preferences(Widget toplevel, const char *filename)
{
    char *fullpath;
    XrmDatabase db;
#if XtSpecificationRelease == 4
    XrmDatabase file_db;
#endif

    fullpath = get_xdvirc_path(filename);
    TRACE_GUI((stderr, "Reading resources from `%s'", fullpath));
    db = XtDatabase(XtDisplay(toplevel));
    
#if XtSpecificationRelease == 4
    file_db  = XrmGetFileDatabase(fullpath);
    XrmMergeDatabases(file_db, &db);
    XrmMergeDatabases(file_db, &m_user_db);
#else /* Xt >= X11R5 */
    XrmCombineFileDatabase(fullpath, &db, True);
    XrmCombineFileDatabase(fullpath, &m_user_db, True);
#endif
    free(fullpath);
}


/*
 *	Routines for running as source-special client.
 */

static unsigned long xdvi_next_request = 0;
static int xerrno;
static int (*XdviOldErrorHandler)(Display *, XErrorEvent *);

static int
XdviErrorHandler(Display *d, XErrorEvent *event)
{
    if (event->serial != xdvi_next_request || event->error_code != BadWindow)
	return XdviOldErrorHandler(d, event);

    xerrno = 1;
    return 0;
}

static int
XdviGetWindowProperty(Display *display,
		      Window w,
		      Atom property,
		      long long_offset,
		      long long_length,
		      Bool delete,
		      Atom req_type,
		      Atom *actual_type_return,
		      int *actual_format_return,
		      unsigned long *nitems_return,
		      unsigned long *bytes_after_return,
		      unsigned char **prop_return)
{
    int retval;

    xdvi_next_request = NextRequest(display);
    xerrno = 0;

    retval = XGetWindowProperty(display, w, property, long_offset,
				long_length, delete, req_type,
				actual_type_return, actual_format_return,
				nitems_return, bytes_after_return, prop_return);

    return (xerrno != 0 ? BadWindow : retval);
}

/* helper function to set a string property of type `prop' for window `win' */
void
set_string_property(const char *str, Atom prop, Window win)
{
    XChangeProperty(DISP, win, prop, prop, 8, PropModeReplace,
		    (const unsigned char *)str, strlen(str));
    XFlush(DISP);	/* necessary to get the property set */
}

/*
 * Check for another running copy of xdvi.
 * If same_file is true, return the window ID of an instance that has
 * currently loaded the same file, or 0 if none exists.
 * If same_file is false, return the first valid xdvi window ID.
 */

Window
get_xdvi_window_id(Boolean same_file, property_cbT callback)
{
    long *window_list;
    size_t window_list_len;
    long *window_list_end;
    long *wp;
    long *p;
    Boolean need_rewrite = False;
    Window ret_window = 0;

    /*
     * Get window list.
     * Copy it over (we'll be calling property_get_data() again).
     */
    if ((window_list_len = property_get_window_list(&p)) == 0)
	return 0;

    window_list = xmalloc(window_list_len * sizeof (long));
    memcpy(window_list, p, window_list_len * sizeof (long));

    XdviOldErrorHandler = XSetErrorHandler(XdviErrorHandler);

    /* Loop over list of windows.  */

    window_list_end = window_list + window_list_len;
    TRACE_CLIENT((stderr, "My property: `%s'", dvi_property));

    for (wp = window_list; wp < window_list_end; ++wp) {
	char *buf_ret;
	size_t len;

	TRACE_CLIENT((stderr, "Checking window %08lx", *wp));
	
	len = property_get_data((Window) *wp, atom_dvi_file(), &buf_ret,
				XdviGetWindowProperty);

	if (len == 0) {
	    /* not getting back info for a window usually indicates
	       that the application the window had belonged to had
	       been killed with signal 9
	    */
	    TRACE_CLIENT((stderr,
			"Window %08lx: doesn't exist any more, deleting", *wp));
	    --window_list_len;
	    --window_list_end;
	    memmove(wp, wp + 1, (window_list_end - wp) * sizeof (long));
	    --wp; /* new item is now at wp; don't skip it in next iteration */
	    need_rewrite = True;
	    continue;
	}
	else { /* window still alive */
	    if (globals.debug & DBG_CLIENT) {
		TRACE_CLIENT((stderr,
			      "Window %08lx: property: `%s'", *wp, buf_ret));
	    }

	    /* invoke callback if given */
	    if (callback != NULL) {
		callback((Window) *wp);
	    }
	    
	    if (!same_file && ret_window == 0) {
		ret_window = *wp;
		if (callback == 0) /* can return early */
		    break;
	    }
	    else if (strcmp(buf_ret, dvi_property) == 0 && ret_window == 0) { /* match */
		ret_window = *wp;
		if (callback == 0) /* can return early */
		    break;
	    }
	}
    }

    XSetErrorHandler(XdviOldErrorHandler);

    if (need_rewrite)
	XChangeProperty(DISP, DefaultRootWindow(DISP),
			atom_xdvi_windows(), atom_xdvi_windows(), 32,
			PropModeReplace, (unsigned char *)window_list,
			window_list_len);

    return ret_window;
}

Boolean
clip_region(int *x, int *y, int *w, int *h)
{
#if 0
    fprintf(stderr, "globals.win_expose.min_x: %d, globals.win_expose.max_x: %d, "
	    "globals.win_expose.min_y: %d, globals.win_expose.max_y: %d\n",
	    globals.win_expose.min_x, globals.win_expose.max_x,
	    globals.win_expose.min_y, globals.win_expose.max_y);
#endif
    /* check for <= so that we also return false if *w or *h == 0 */
    if (*x + *w <= globals.win_expose.min_x
	|| *x >= globals.win_expose.max_x
	|| *y + *h <= globals.win_expose.min_y
	|| *y >= globals.win_expose.max_y
	/* extra protection agains *w or *h == 0; don't know if this can actually happen ... */
	|| globals.win_expose.max_y == globals.win_expose.min_y
	|| globals.win_expose.max_x == globals.win_expose.min_x) {
	return False;
    }
    if (*x < globals.win_expose.min_x) {
	*w -= globals.win_expose.min_x - *x;
	*x = globals.win_expose.min_x;
    }
    if (*x + *w > globals.win_expose.max_x) {
	*w = globals.win_expose.max_x - *x;
    }
    if (*y < globals.win_expose.min_y) {
	*h -= globals.win_expose.min_y - *y;
	*y = globals.win_expose.min_y;
    }
    if (*y + *h > globals.win_expose.max_y) {
	*h = globals.win_expose.max_y - *y;
    }
    return True;
}

Boolean
clip_region_to_rect(XRectangle *rect)
{
    int x = rect->x;
    int y = rect->y;
    int w = rect->width;
    int h = rect->height;
    Boolean ret = clip_region(&x, &y, &w, &h);
    if (ret) {
	rect->x = x;
	rect->y = y;
	rect->width = w;
	rect->height = h;
    }
    return ret;
}

Boolean window_is_mapped(Window w, Display *dpy)
{
    XWindowAttributes xwa;
    return XGetWindowAttributes(dpy, w, &xwa) && xwa.map_state == IsViewable;
}

XtPointer cast_int_to_XtPointer(int i)
{
    return (XtPointer)(ptrdiff_t)i;
}
