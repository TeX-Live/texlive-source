/*========================================================================*\

Copyright (c) 1994-2019  Paul Vojta

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
PAUL VOJTA OR ANY OTHER AUTHOR OF THIS SOFTWARE BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

\*========================================================================*/

#ifdef PS_GS	/* entire file */


#include "xdvi-config.h"
#include "xdvi.h"
#include "psgs.h"

#include <setjmp.h>
#include <X11/Xatom.h>
#include <sys/time.h>	/* for timeval */

#include <signal.h>

/* Condition for retrying a write */
#include <errno.h>
#include <string.h>

#include "kpathsea/c-pathmx.h"
#include "dvi-init.h"
#include "dvi-draw.h"
#include "events.h"
#include "statusline.h"
#include "special.h"
#include "util.h"


#ifdef	X_NOT_STDC_ENV
extern	int	errno;
#endif

#ifdef	EWOULDBLOCK
# ifdef	EAGAIN
#  define AGAIN_CONDITION	(errno == EWOULDBLOCK || errno == EAGAIN)
# else
#  define AGAIN_CONDITION	(errno == EWOULDBLOCK)
# endif
#else
# ifdef	EAGAIN
#  define AGAIN_CONDITION	(errno == EAGAIN)
# endif
#endif

#if HAVE_POLL
# include <poll.h>
# define XIO_IN POLLIN
# define XIO_OUT POLLOUT
#else
# define XIO_IN 1
# define XIO_OUT 2
#endif

#ifdef MOTIF
# include <Xm/Xm.h>
#endif

extern const char psheader[];
extern unsigned psheaderlen;

Boolean gs_postpone_prescan = False;

/* global procedures (besides initGS) */

static void toggle_gs(int flag);
static void destroy_gs(void);
static void interrupt_gs(void);
static void endpage_gs(void);
static void drawbegin_gs(int, int, const char *);
static void drawbegin_gs_box(int, int, const char *);
void drawraw_gs(const char *);
static void drawfile_gs(const char *, FILE *);
static void drawend_gs(const char *);
static void beginheader_gs(void);
static void endheader_gs(void);
static void newdoc_gs(void);

static struct psprocs gs_procs = {
    /* toggle */ toggle_gs,
    /* destroy */ destroy_gs,
    /* interrupt */ interrupt_gs,
    /* endpage */ endpage_gs,
    /* drawbegin */ drawbegin_gs,
    /* drawraw */ drawraw_gs,
    /* drawfile */ drawfile_gs,
    /* drawend */ drawend_gs,
    /* beginheader */ beginheader_gs,
    /* endheader */ endheader_gs,
    /* newdoc */ newdoc_gs
};

static int std_io[2];
Pixmap bpixmap;

#define	GS_fd	(std_io[0])

/* some arguments are filled in later */
static char arg4[] = "-dDEVICEWIDTH=xxxxxxxxxx";
static char arg5[] = "-dDEVICEHEIGHT=xxxxxxxxxx";

static const char *argv[] = {
    NULL, NULL, "-dNOPAUSE", "-q", arg4, arg5,
    "-dDEVICEXRESOLUTION=72",
    "-dDEVICEYRESOLUTION=72",
    "-dNOSAFER", "-dNOEPS", NULL, NULL, NULL, NULL, NULL, NULL
};

static unsigned int GS_page_w;	/* how big our current page is */
static unsigned int GS_page_h;
static Boolean GS_alpha;	/* remember if we are using the alpha driver */
static unsigned long GS_mag;	/* magnification currently in use */
static int GS_shrink;		/* shrink factor currently in use */
static Boolean GS_active;	/* if we've started a page yet */
static int GS_pending;		/* number of ack's we're expecting */
static char GS_outb[257];	/* circular output buffer */
static char *GS_outb_in;	/* next position in output buffer */
static char *GS_outb_out;	/* next byte to come out of buffer */
#define	GS_outb_limit	(GS_outb + sizeof GS_outb)	/* last+1 byte */
static int GS_write_ack;	/* flag to set when done writing */
static Boolean GS_in_header;	/* if we're sending a header */
static Boolean GS_in_doc;	/* if we've sent header information */
static Boolean GS_page_dirty = False; /* if we've drawn since last erase */
static int GS_ev_mask;		/* events for which we'll stop */
static int GS_die_ack = 0;	/* flags to set when GS dies */
static Boolean GS_timer_set;	/* if there's a timer set */

#define	GS_MASK_NORMAL	EV_GE_NEWPAGE
#define	GS_MASK_HEADER	EV_GE_PS_TOGGLE
#define	GS_MASK_INIT	(EV_GE_TERM | EV_PS_TOGGLE)

static Atom gs_atom;
static Atom gs_colors_atom;

#define	Landscape	90

#define	LINELEN	81
static char line[LINELEN + 1];
static char *linepos = line;
static char ackstr[] = "\347\310\376";

static void gs_died(int, struct xchild *);

static char *read_from_gs(int, void *);
static void write_to_gs(int, void *);

static	struct xio	gs_xio		= {NULL, 0, XIO_IN,
#if HAVE_POLL
					   NULL,
#endif
					   read_from_gs, write_to_gs,
					   NULL /* data */
};

static struct xchild gs_child = { NULL, 0, NULL, NULL, NULL, SIGKILL, gs_died };
#define	GS_pid		(gs_child.pid)

/* It seems that XtTimers don't work reliably with xdvi's event treatment;
   so use custom timers for the time being. Search for TIMER_PROBLEM_FIXED
   to locate the relevant code parts.
*/

#if TIMER_PROBLEM_FIXED
static XtIntervalId gs_timeout_id = 0;
static void gs_timer_proc(XtPointer client_data, XtIntervalId * id);
static struct xtimer gs_timer = {NULL, {0, 0}, gs_timer_proc, NULL
#if XDVI_XT_TIMER_HACK
				 , NULL, NULL
#endif
};

#else
static void gs_alarm(struct xtimer *this, void *data);
static struct xtimer gs_timer = {NULL, {0, 0}, XTM_DEFAULT, gs_alarm, NULL
#if XDVI_XT_TIMER_HACK
				 , NULL, NULL
#endif
};
#endif


static void
showto(char *q)
{
    char *p = line;
    char *p1;

    while (p < q) {
	p1 = memchr(p, '\n', q - p);
	if (p1 == NULL)
	    p1 = q;
	*p1 = '\0';
	printf("gs: %s\n", p);
	p = p1 + 1;
    }
}

static char *
read_from_gs(int fd, void *data)
{
    int bytes;
    char *line_end = NULL;
    char *p;

    UNUSED(data);
    
    for (;;) {

	bytes = read(fd, linepos, line + LINELEN - linepos);
	if (bytes < 0) {
	    if (AGAIN_CONDITION)
		break;
	    perror("xdvik: read_from_gs");
	    break;
	}
	line_end = linepos + bytes;

	if (bytes == 0) {
	    if (GS_pid != 0)
		XDVI_WARNING((stderr, "Read_from_gs returned 0 bytes"));
	    break;
	}

	/* Check for ack strings */
	for (p = line; p < line_end - 2; ++p) {
	    p = memchr(p, '\347', line_end - p - 2);
	    if (p == NULL) break;
	    if (memcmp(p, ackstr, 3) == 0) {
		--GS_pending;
		if (GS_pending == 0)
		    globals.ev.flags |= EV_ACK;
		if (globals.debug & DBG_PS)
		    printf("Got GS ack; %d pending.\n", GS_pending);
	    } else
		continue;

	    showto(p);
	    p += 3;
	    memmove(line, p, line_end - p);
	    line_end -= p - line;
	    linepos = p = line;
	    --p;
	}
	*line_end = '\0';
	p = strrchr(linepos, '\n');
	if (p != NULL) {
	    ++p;
	    showto(p);
	    memmove(line, p, line_end - p);
	    line_end -= p - line;
	}
	linepos = line_end;
	/*
	 * Normally we'd hold text until a newline character, but the
	 * buffer is full.  So we flush it, being careful not to cut up an
	 * ack string.
	 */
	if (linepos >= line + LINELEN) {
	    p = line + LINELEN;
	    if ((*--p != '\347' && *--p != '\347' && *--p != '\347')
		|| memcmp(p, ackstr, line + LINELEN - p) != 0)
		p = line + LINELEN;
	    *p = '\0';
	    printf("gs: %s\n", line);
	    *p = '\347';
	    linepos = line;
	    while (p < line + LINELEN)
		*linepos++ = *p++;
	}
    }
    return line_end; /* dummy */
}

static void
write_to_gs(int fd, void *data)
{
    char *send_end;
    int	bytes;

    UNUSED(fd);
    UNUSED(data);
    
    for (;;) {
	send_end = GS_outb_in;
	if (send_end < GS_outb_out)
	    send_end = GS_outb_limit;
	bytes = write(GS_fd, GS_outb_out, send_end - GS_outb_out);

	if (bytes < 0) {
	    if (AGAIN_CONDITION)
		break;
	    perror("xdvik: write_to_gs");
	    break;
	}

	GS_outb_out += bytes;
	if (GS_outb_out == GS_outb_limit)
	    GS_outb_out = GS_outb;
	if (GS_outb_out == GS_outb_in) {	/* if buffer is empty */
	    gs_xio.xio_events = XIO_IN;
#if HAVE_POLL
	    if (gs_xio.pfd != NULL)	/* write_to_gs is called directly */
		gs_xio.pfd->events = XIO_IN;
#endif
	    break;
	}
    }
    globals.ev.flags |= GS_write_ack;
    GS_write_ack = 0;
}

/*
 *	Main routine for writing to the GS interpreter.
 */

static void
gs_send(const char *cp, size_t len)
{
    const char *cp_end = cp + len;
    char *send_end;
    size_t bytes;
    char *old_out;
    Boolean interrupting;

    if (GS_pid == 0 || (globals.ev.flags & GS_ev_mask))
	return;

    /*
     * Because cp might reside on the stack, don't return until we've
     * copied all of it to our circular output buffer.
     * Note that GS_outb_out == GS_outb_in means that the buffer is empty.
     */

    GS_timer_set = interrupting = False;
    for (;;) {
	send_end = GS_outb_out;
	if (send_end == GS_outb)
	    send_end = GS_outb_limit;
	--send_end;
	if (send_end < GS_outb_in)
	    send_end = GS_outb_limit;
	bytes = send_end - GS_outb_in;
	if (bytes > 0) {
	    if (bytes >= (unsigned)(cp_end - cp))
		bytes = cp_end - cp;
	    memcpy(GS_outb_in, cp, bytes);
	    cp += bytes;
	    GS_outb_in += bytes;
	    if (GS_outb_in == GS_outb_limit)
		GS_outb_in = GS_outb;
	    if (cp < cp_end)
		continue;
	}

	/* The buffer is now full --or-- we've run out of data */
	old_out = GS_outb_out;
	if (!(gs_xio.xio_events & XIO_OUT)) {	/* restart output */
	    gs_xio.xio_events = XIO_IN | XIO_OUT;
#if HAVE_POLL
	    if (gs_xio.pfd != NULL)
		gs_xio.pfd->events = POLLIN | POLLOUT;
#endif
	    write_to_gs(GS_fd, NULL);
	    if (GS_outb_out != old_out) {
		if (cp == cp_end)
		    break;
		else
		    continue;
	    }
	}

	if (cp == cp_end)
	    break;

	GS_die_ack = GS_write_ack = EV_ACK;
	for (;;) {	/* loop because there may be stray ACKs */
	    if (!interrupting) {
		(void) read_events(GS_ev_mask | EV_ACK);
		globals.ev.flags &= ~EV_ACK;

		if (GS_pid == 0) {	/* if GS died */
		    GS_die_ack = 0;
		    return;
		}
		
		if (GS_outb_out != old_out)	/* if more room in buffer */
		    break;

		if (globals.ev.flags & GS_ev_mask) {	/* if a serious event */
		    if (globals.debug & DBG_PS)
			puts("Setting timeout in gs_send()");

		    set_timer(&gs_timer, resource.gs_timeout);
		    GS_timer_set = interrupting = True;
		}
	    }
	    else {
		(void) read_events(EV_GE_TERM | EV_PS_TOGGLE | EV_ACK);
		globals.ev.flags &= ~EV_ACK;

		if (GS_outb_out != old_out)	/* if more room in buffer */
		    break;

		if (GS_timer_set)	/* if timer still set */
		    cancel_timer(&gs_timer);

		destroy_gs();
		GS_die_ack = 0;
		return;
	    }
	}
    }

    if (GS_timer_set)	/* if timer still set */
	cancel_timer(&gs_timer);

    GS_die_ack = GS_write_ack = 0;
}

static void
#if TIMER_PROBLEM_FIXED
gs_timer_proc(XtPointer client_data, XtIntervalId * id)
#else
    gs_alarm(struct xtimer *this, void *data)
#endif
{
    UNUSED(this);
    UNUSED(data);
    
    if (globals.debug & DBG_PS)
	puts("GS timeout expired");

    globals.ev.flags |= EV_ACK;
    GS_timer_set = False;
#if TIMER_PROBLEM_FIXED
    gs_timeout_id = 0;
#endif
}

/*
 *	Wait for acknowledgement from GS.
 */

static	void
waitack(void)
{
    if (GS_pending == 0) {
	globals.ev.flags &= ~EV_ACK;
	return;
    }

    GS_die_ack = EV_ACK;

    for (;;) {	/* loop because there might be stray ACKs. */
	/*  	fprintf(stderr, "looping for stray ACKs, page %d\n", current_page); */
	(void) read_events(EV_GE_ACK);
	globals.ev.flags &= ~EV_ACK;

	if (GS_pending == 0) {
	    GS_die_ack = 0;
	    return;
	}
	if (globals.ev.flags & EV_GE_ACK)
	    break;
    }

    if (globals.debug & DBG_PS)
	puts("Setting timeout in waitack()");

#if TIMER_PROBLEM_FIXED
    if (gs_timeout_id)
	XtRemoveTimeOut(gs_timeout_id);
    gs_timeout_id = XtAppAddTimeOut(globals.app, resource.gs_timeout,
				    gs_timer_proc, (XtPointer) NULL);
#else
    set_timer(&gs_timer, resource.gs_timeout);
#endif
    GS_timer_set = True;

    (void) read_events(EV_GE_TERM | EV_PS_TOGGLE | EV_ACK);
    globals.ev.flags &= ~EV_ACK;

    if (GS_timer_set) {
#if TIMER_PROBLEM_FIXED
	XtRemoveTimeOut(gs_timeout_id);
	gs_timeout_id = 0;
#else
	cancel_timer(&gs_timer);
#endif
    }
    GS_die_ack = 0;

    if (GS_pending > 0)
	destroy_gs();
}


/*
 *	Fork a process to run ghostscript.  This is done using the
 *	x11 device (which needs to be compiled in).  Normally the x11
 *	device uses ClientMessage events to communicate with the calling
 *	program, but we don't do this.  The reason for using the ClientMessage
 *	events is that otherwise ghostview doesn't know when a non-conforming
 *	postscript program calls showpage.   That doesn't affect us here,
 *	since in fact we disable showpage.
 *
 *	SAFER mode is handled by providing both the -dNOSAFER and -dSAFER
 *	options. Ghostscript versions 7.04 and earlier ignore -dNOSAFER and use
 *	-dSAFER; the code in strsafe is ignored since .locksafe is not present.
 *	In versions 7.04 and higher, -dNOSAFER overrides -dSAFER; SAFER mode is
 *	optionally turned on by sending the strsafe string.  It is possible in some
 *	versions of gs prior to 7.04 to use -dDELAYSAFER instead of -dNOSAFER, but
 *	there's no point in doing that since .locksafe is not defined in those
 *	versions.  I don't know where 7.03 fits in on all of this (but it works with
 *	that version as well).
 */

Boolean
initGS(void)
{
    char buf[100];
    static Boolean did_putenv = False;
    /*
     * This string reads chunks (delimited by %%xdvimark).
     * The first character of a chunk tells whether a given chunk
     * is to be done within save/restore or not.
     * The `H' at the end tells it that the first group is a
     * header; i.e., no save/restore.
     * `execute' is unique to ghostscript.
     */

    static const char strsafe[] =
	"{ << /PermitFileReading [ (*) ] /PermitFileWriting [ ] /PermitFileControl [ ] "
	">> setuserparams .locksafe "
	"} stopped pop\n";
    static const char str1[] =
        "/xdvi$run "
        "{$error /newerror false put currentfile cvx stopped {handleerror} if} "
	"def "
	"/xdvi$ack (\347\310\376) def "
	"/xdvi$dslen countdictstack def "
	"{currentfile read pop 72 eq "
	"{xdvi$run} "
	"{/xdvi$sav save def xdvi$run "
	"clear countdictstack xdvi$dslen sub {end} repeat xdvi$sav restore} "
	"ifelse "
	"{(%%xdvimark) currentfile =string {readline} stopped "
	"{clear} {pop eq {exit} if} ifelse }loop "
	"flushpage xdvi$ack print flush "
	"}loop\nH";
    
    static const char str2[] =
	"[0 1 1 0 0 0] concat\n"
	"stop\n%%xdvimark\n";
    
    /*
     * If we're prescanning *before* setting up the widgets (to get the
     * page size, for example), then postpone starting up ghostscript.
     */
    if (mane.win == (Window)0) {
	if (globals.debug & DBG_PS)
	    puts("Hit PS header in early prescan; postponing.");
	psp = no_ps_procs;
	gs_postpone_prescan = True;
	return True;
    }
    
    if (globals.debug & DBG_PS)
	puts("Running initGS ...");
    
    gs_atom = XInternAtom(DISP, "GHOSTVIEW", False);
    /* send bpixmap, orientation, bbox (in pixels), and h & v resolution */
    sprintf(buf, "%ld %d 0 0 %u %u 72 72",
	    bpixmap,		/* bpixmap */
	    Landscape,	/* orientation */
	    GS_page_h = globals.page.h, GS_page_w = globals.page.w);

    XChangeProperty(DISP, mane.win, gs_atom, XA_STRING, 8,
		    PropModeReplace, (unsigned char *)buf, strlen(buf));
    GS_alpha = resource.gs_alpha;

    gs_colors_atom = XInternAtom(DISP, "GHOSTVIEW_COLORS", False);
    sprintf(buf, "%s %ld %ld", resource.gs_palette,
	    color_data[0].pixel, color_data[1].pixel);
    
    XChangeProperty(DISP, mane.win, gs_colors_atom, XA_STRING, 8,
		    PropModeReplace, (unsigned char *)buf, strlen(buf));

    if (!did_putenv) {
	sprintf(buf, "%ld", mane.win);
	xputenv("GHOSTVIEW", buf);
	did_putenv = True;
    }

    XSync(DISP, False);	/* update the window */

    if (xpipe(std_io) != 0) {
	perror("[xdvik] pipe");
	return False;
    }
    fflush(stderr);	/* to avoid double flushing */
    GS_pid = vfork();
    if (GS_pid == 0) {	/* child */
	const char **argvp = argv + 10;

	argv[1] = resource.gs_alpha ? "-sDEVICE=x11alpha" : "-sDEVICE=x11";
	sprintf(arg4 + 14, "%u", GS_page_w);
	sprintf(arg5 + 15, "%u", GS_page_h);
	if (resource.gs_safer)
	    *argvp++ = "-dSAFER";

#if 0
	if (resource.gs_alpha) {
	    *argvp++ = "-dGraphicsAlphaBits=4";
	    *argvp++ = "-dTextAlphaBits=4";
	    *argvp++ = "-dMaxBitmap=0";
	}
#endif
	
	*argvp = "-";
	(void) close(std_io[0]);
	(void) dup2(std_io[1], 0);
	(void) dup2(std_io[1], 1);
	(void) dup2(std_io[1], 2);
	(void) close(std_io[1]);

	(void)execvp(argv[0] = resource.gs_path, (char *const *)argv);
	/*TODO: use fork_process here?? */
	XDVI_ERROR((stderr, "Execvp of %s failed: %s", argv[0], strerror(errno)));
	_exit(EXIT_FAILURE);
    }

    (void)close(std_io[1]);

    if (GS_pid == -1) {	/* error */
	GS_pid = 0;
	perror("[xdvik] vfork");
	(void)close(GS_fd);

	return False;
    }

    prep_fd(GS_fd, True);	/* Set file descriptor for non-blocking I/O */

    gs_child.name = xstrdup("gs");
    gs_child.io = &gs_xio;
    set_chld(&gs_child);

    psp = gs_procs;
    GS_active = False;
    GS_in_header = True;
    GS_page_dirty = False;
    GS_pending = 1;
    GS_mag = GS_shrink = -1;
    gs_xio.fd = GS_fd;
    gs_xio.xio_events = XIO_IN;
    GS_write_ack = 0;
    GS_outb_in = GS_outb_out = GS_outb;
    set_io(&gs_xio);
    GS_ev_mask = GS_MASK_INIT;
    (void) signal(SIGPIPE, SIG_IGN);
    
    if (resource.gs_safer)
	gs_send(strsafe, (sizeof strsafe) - 1);
    gs_send(str1, (sizeof str1) - 1);
    gs_send(psheader, psheaderlen);
    gs_send(str2, (sizeof str2) - 1);
    waitack();
    GS_in_header = False;
    GS_ev_mask = GS_MASK_NORMAL;

    if (GS_pid == 0) {	/* if something happened */
	destroy_gs();
	return False;
    }
    if (resource.postscript == 0)
	toggle_gs(0);	/* if we got a 'v' already */
    else {
	scanned_page = scanned_page_ps = scanned_page_reset;
	globals.ev.flags |= EV_NEWPAGE;	/* ||| redraw the page */
	longjmp(globals.ev.canit, 1);
    }
    return True;
}

static void
toggle_gs(int flag)	/* this routine is callable from within read_events().  */
{
    if (globals.debug & DBG_PS)
	fprintf(stdout, "Toggling GS to %d", flag);

    switch (flag) {
    case 0:
	psp.drawbegin = drawbegin_none;
	break;
    case 1:
	psp.drawbegin = drawbegin_gs;
	break;
    default:
	psp.drawbegin = drawbegin_gs_box;
	break;
    }
}

void
gs_resume_prescan(void)
{
    if (globals.debug & DBG_PS)
	puts("Resuming prescan");
    
    gs_postpone_prescan = False;
    if (!initGS())	/* this may not return */
	psp = no_ps_procs;
}

void
gs_erase_page(void)
{
    static const char str[] = " erasepage stop\n%%xdvimark\n";

    if (!GS_page_dirty) return;

    if (globals.debug & DBG_PS)
	puts("Erasing gs page");

    if (GS_active)
	XDVI_FATAL((stderr, "Internal error in gs_erase_page()."));

    ++GS_pending;
    gs_send(str, sizeof(str) - 1);
    waitack();
}

static void
gs_died(int status, struct xchild *child)
{
    UNUSED(status);
    if (globals.debug & DBG_PS)
	fprintf(stderr, "process %s died\n", child->name);
    GS_pid = 0;
    (child->io->read_proc)(child->io->fd, NULL);
    if (linepos > line) {
	*linepos = '\0';
	printf("%s: %s\n", child->name, line);
	linepos = line;
    }
    clear_io(child->io);
    (void)close(child->io->fd);

    scanned_page = scanned_page_ps = scanned_page_reset;
    GS_active = GS_in_doc = GS_page_dirty = False;
    GS_pending = 0;
    globals.ev.flags |= GS_die_ack;
}

static	void
destroy_gs(void)
{
    if (globals.debug & DBG_PS)
	puts("Destroying GS process");
    if (GS_pid != 0) {
	if (kill(GS_pid, SIGKILL) < 0 && errno != ESRCH)
	    perror("xdvik: destroy_gs");
	GS_pid = 0;
	clear_chld(&gs_child);
	read_from_gs(GS_fd, NULL);
	if (linepos > line) {
	    *linepos = '\0';
	    printf("gs: %s\n", line);
	    linepos = line;
	}
	clear_io(&gs_xio);
	(void) close(GS_fd);

	scanned_page = scanned_page_ps = scanned_page_reset;
    }
    GS_active = GS_in_doc = GS_page_dirty = False;
    GS_pending = 0;
}

static void
deactivate(void)
{
    static const char str[] = " stop\n%%xdvimark\n";
    int	saved_mask;

    saved_mask = GS_ev_mask;
    GS_ev_mask = 0;
    gs_send(str, (sizeof str) - 1);
    GS_ev_mask = saved_mask;

    GS_active = False;
}

static void
interrupt_gs(void)
{
    if (globals.debug & DBG_PS)
	puts("Running interrupt_gs()");
    /*
      FIXME: added this special case, since otherwise EV_ACK might not get
      reset at all, and hang xdvi forever when scrolling quickly through
      a file that should be reloaded.
    */
    if (GS_pending == 0) {
	/*  	fprintf(stderr, "+++++++++ 1st case; waitack: %d\n", globals.ev.flags & EV_ACK); */
	globals.ev.flags &= ~EV_ACK;
	return;
    }
    else if (GS_pending < 0) {
	/*  	fprintf(stderr, "GS_pending: %d; waitack: %d\n", GS_pending, globals.ev.flags & EV_ACK); */
	return;	/* nothing to do */
    }

    /*
     * ||| what I'd really like to do here is cause gs to execute
     * the interrupt routine in errordict.  But so far (gs 2.6.1)
     * that has not been implemented in ghostscript.
     */

    if (GS_active)
	deactivate();
    waitack();
}

static void
endpage_gs(void)
{
    if (globals.debug & DBG_PS)
	puts("Running endpage_gs()");
    if (GS_active) {
	deactivate();
	waitack();
    }
}

/*
 *	Checks that the GS interpreter is running correctly.
 */

static void
checkgs(Boolean in_header)
{
    char buf[150];

    /*
     * For geometry changes, we pretty much have to restart Ghostscript.
     * This is what Ghostview does (for example, in gv-3.7.4
     * this occurs in Setup() in Ghostview.c).
     */
    if (globals.page.w > GS_page_w || globals.page.h > GS_page_h
	|| GS_alpha != resource.gs_alpha)
	destroy_gs();

    if (GS_pid == 0)
	(void)initGS();

    if (!GS_active) {
	if (magnification != GS_mag) {
	    if (globals.ev.flags & GS_ev_mask)
		longjmp(globals.ev.canit, 1);
	    ++GS_pending;
	    sprintf(buf,
		    "H TeXDict begin /DVImag %ld 1000 div def end stop\n"
		    "%%%%xdvimark\n",
		    GS_mag = magnification);
	    gs_send(buf, strlen(buf));
	}

	if (mane.shrinkfactor != GS_shrink) {
	    if (globals.ev.flags & GS_ev_mask)
		longjmp(globals.ev.canit, 1);
	    ++GS_pending;
	    sprintf(buf,
		    "H TeXDict begin %d %d div dup /Resolution X /VResolution X end stop\n"
		    "%%%%xdvimark\n",
		    resource.pixels_per_inch,
		    GS_shrink = mane.shrinkfactor);
	    gs_send(buf, strlen(buf));
	}
    }
}

static void
drawbegin_gs(int xul, int yul, const char *cp)
{
    char buf[32];
    static const char str[] = " TeXDict begin\n";
    
    checkgs(False);

    if (!GS_active) {
	if (globals.ev.flags & GS_ev_mask)
	    longjmp(globals.ev.canit, 1);
	++GS_pending;
	gs_send(str, (sizeof str) - 1);
	GS_active = True;
    }

    /* This allows the X side to clear the page */
    XSync(DISP, False);

    sprintf(buf, "%d %d moveto\n", xul, yul);
    gs_send(buf, strlen(buf));
    /*     gs_send(clear_bg, strlen(clear_bg)); */
    if (globals.debug & DBG_PS)
	printf("drawbegin at %d,%d:  sending `%s'\n", xul, yul, cp);

    /*
     * added SU 2000/12/18:
     * Try to detect some simple PS rotating commands, and warn about them.
     * There are surely more than that ...
     */
    if (strstr(cp, "rotate") != NULL	/* used by graphics.sty */
	|| strstr(cp, "RotBegin") != NULL	/* used by pstricks */
	) {
	statusline_error(STATUS_SHORT,
			 "Warning: PS code on page %d may contain rotation, "
			 "which is not supported by xdvi", current_page + 1);
    }

    gs_send(cp, strlen(cp));
}

static void
drawbegin_gs_box(int xul, int yul, const char *cp)
{
    drawbegin_gs(xul, yul, cp);
}


void
drawraw_gs(const char *cp)
{
    if (!GS_active)
	return;
    if (globals.debug & DBG_PS)
	printf("raw ps sent to context: %s\n", cp);
    GS_page_dirty = True;
    gs_send(cp, strlen(cp));
    gs_send("\n", 1);
}

static void
drawfile_gs(const char *cp, FILE *f)
{
    char canonical_path[MAXPATHLEN + 1];
    char *ret;

    fclose(f);	/* don't need it */

    if (!GS_active)
	return;

    if (globals.debug & DBG_PS)
	printf("original path: |%s|\n", cp);
    /*     drawraw_gs("newpath 0 0 moveto 0 1000 rlineto 1000 0 rlineto 0 -1000 rlineto closepath 1 setgray fill"); */
#if 0
    /* if expand_symlinks or remove_dots from kpathsea were public,
       we could use the following: */
    ret = remove_dots(expand_symlinks(cp));
#endif

    ret = REALPATH(cp, canonical_path);
    if (ret == NULL)
	XDVI_WARNING((stderr, "Couldn't canonicalize path \"%s\": %s. Sending to gs unchanged.",
		      cp, strerror(errno)));
    else
	cp = canonical_path;

    if (globals.debug & DBG_PS)
	printf("expanded path: |%s|\n", cp);

    GS_page_dirty = True;
    gs_send("(", 1);
    gs_send(cp, strlen(cp));
    gs_send(")run\n", 5);
    /*     { */
    /* 	char *clear_bg = "newpath 0 0 moveto 0 1000 rlineto 1000 0 rlineto 0 -1000 rlineto closepath .4 setgray fill\n"; */
    /* 	gs_send(clear_bg, strlen(clear_bg)); */
    /*     } */
}

static void
drawend_gs(const char *cp)
{
    if (!GS_active)
	return;
    if (globals.debug & DBG_PS)
	printf("end ps: %s\n", cp);
    gs_send(cp, strlen(cp));
    gs_send("\n", 1);
    save_bbox();
}

static void
beginheader_gs(void)
{
    static const char str[] = "Hsave /xdvi$doc exch def\n";

    if (globals.debug & DBG_PS)
	puts("Running beginheader_gs()");

    checkgs(True);

    if (GS_active) {
	if (!GS_in_header)
	    XDVI_FATAL((stderr, "Internal error in beginheader_gs()."));
	return;
    }

    if (globals.ev.flags & GS_ev_mask) {
	/* 	fprintf(stderr, "longjmp!"); */
	longjmp(globals.ev.canit, 1);
    }

    GS_in_header = True;
    GS_ev_mask = GS_MASK_HEADER;
    ++GS_pending;
    if (GS_in_doc)
	gs_send("H", 1);
    else {
	gs_send(str, (sizeof str) - 1);
	GS_in_doc = True;
    }
    GS_active = True;
}

static void
endheader_gs(void)
{
    if (globals.debug & DBG_PS)
	puts("Running endheader_gs()");

    if (GS_active) {
	deactivate();
	waitack();
	GS_in_header = False;
	GS_ev_mask = GS_MASK_NORMAL;
    }
}

static void
newdoc_gs(void)
{
    static const char str[] = "Hxdvi$doc restore stop\n%%xdvimark\n";

    if (globals.debug & DBG_PS)
	puts("Running newdoc_gs()");

    if (GS_in_doc) {
	++GS_pending;
	gs_send(str, (sizeof str) - 1);
	GS_mag = GS_shrink = -1;
	GS_in_doc = False;

	GS_page_w = GS_page_h = 0;
    }
}

#else
/* silence `empty compilation unit' warnings */
static void bar(void); static void foo(void) { bar(); } static void bar(void) { foo(); }
#endif /* PS_GS */
