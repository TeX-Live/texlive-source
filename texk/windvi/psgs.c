/*========================================================================*\

Copyright (c) 1994-1999  Paul Vojta

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
PAUL VOJTA BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

\*========================================================================*/

#ifdef PS_GS /* whole file */

#include "xdvi-config.h"
#include "kpathsea/c-pathmx.h"
#include <X11/Xatom.h>
#include <sys/time.h> /* for timeval */

#include <signal.h>
#include <sys/file.h>	/* this defines FASYNC */

/* if POSIX O_NONBLOCK is not available, use O_NDELAY */
#if !defined(O_NONBLOCK) && defined(O_NDELAY)
#define	O_NONBLOCK O_NDELAY
#endif

#ifdef	EWOULDBLOCK
#ifdef	EAGAIN
#define	AGAIN_CONDITION	(errno == EWOULDBLOCK || errno == EAGAIN)
#else	/* EAGAIN */
#define	AGAIN_CONDITION	(errno == EWOULDBLOCK)
#endif	/* EAGAIN */
#else	/* EWOULDBLOCK */
#ifdef	EAGAIN
#define	AGAIN_CONDITION	(errno == EAGAIN)
#endif	/* EAGAIN */
#endif	/* EWOULDBLOCK */

#if HAVE_POLL
# include <poll.h>
#else
# if HAVE_SYS_SELECT_H
#  include <sys/select.h>
# else
#  if HAVE_SELECT_H
#   include <select.h>
#  endif
# endif
#endif

#if !defined(FLAKY_SIGPOLL) && !HAVE_STREAMS && !defined(FASYNC)
#define	FLAKY_SIGPOLL	1
#endif

#if HAVE_VFORK_H
# include <vfork.h>
#endif

#ifndef	X_GETTIMEOFDAY
#define	X_GETTIMEOFDAY(t) gettimeofday(t, (struct timezone*) NULL)
#endif

#ifndef	timercmp
#define	timercmp(a, b, cmp)	((a)->tv_sec cmp (b)->tv_sec || \
		((a)->tv_sec == (b)->tv_sec && (a)->tv_usec cmp (b)->tv_usec))
#endif	/* timercmp */

extern	_Xconst	char	psheader[];
extern	int		psheaderlen;

#define	postscript	resource._postscript
#define	fore_Pixel	resource._fore_Pixel
#define	back_Pixel	resource._back_Pixel

/* global procedures (besides initGS) */

static	void	toggle_gs	ARGS((void));
static	void	destroy_gs	ARGS((void));
static	void	interrupt_gs	ARGS((void));
static	void	endpage_gs	ARGS((void));
static	void	drawbegin_gs	ARGS((int, int, _Xconst char *));
static	void	drawraw_gs	ARGS((_Xconst char *));
static	void	drawfile_gs	ARGS((_Xconst char *, FILE *));
static	void	drawend_gs	ARGS((_Xconst char *));
static	void	beginheader_gs	ARGS((void));
static	void	endheader_gs	ARGS((void));
static	void	newdoc_gs	ARGS((void));

static	struct psprocs	gs_procs = {
	/* toggle */		toggle_gs,
	/* destroy */		destroy_gs,
	/* interrupt */		interrupt_gs,
	/* endpage */		endpage_gs,
	/* drawbegin */		drawbegin_gs,
	/* drawraw */		drawraw_gs,
	/* drawfile */		drawfile_gs,
	/* drawend */		drawend_gs,
	/* beginheader */	beginheader_gs,
	/* endheader */		endheader_gs,
	/* newdoc */		newdoc_gs};

static	int	std_in[2];
static	int	std_out[2];

#define	GS_in	(std_in[1])
#define	GS_out	(std_out[0])

		/* some arguments are filled in later */
static	char	arg4[]	= "-dDEVICEWIDTH=xxxxxxxxxx";
static	char	arg5[]	= "-dDEVICEHEIGHT=xxxxxxxxxx";

static	_Xconst	char	*argv[]	= {NULL, NULL, "-dNOPAUSE", "-q", arg4, arg5,
				   "-dDEVICEXRESOLUTION=72",
				   "-dDEVICEYRESOLUTION=72",
				   NULL, NULL, NULL};

static	pid_t		GS_pid;
static	unsigned int	GS_page_w;	/* how big our current page is */
static	unsigned int	GS_page_h;
static	Boolean		GS_alpha;	/* if we are using the alpha driver */
static	int		GS_mag;		/* magnification currently in use */
static	int		GS_shrink;	/* shrink factor currently in use */
static	Boolean		GS_active;	/* if we've started a page yet */
static	int		GS_pending;	/* number of ack's we're expecting */
static	_Xconst char	*GS_send_byte;	/* next byte to send to gs */
static	_Xconst char	*GS_send_end;	/* last + 1 byte to send to gs */
static	Boolean		GS_in_header;	/* if we're sending a header */
static	Boolean		GS_in_doc;	/* if we've sent header information */
static	Boolean		GS_pending_int;	/* if interrupt rec'd while in gs_io()*/
static	Boolean		GS_timeout_set;	/* if there's a timeout set */
static	struct timeval	GS_timeout;	/* when to time out */
static	Boolean		GS_old;		/* if we're using gs 2.xx */

static	Atom		gs_atom;
static	Atom		gs_colors_atom;

#define	Landscape	90

/*
 *	ghostscript I/O code.  This should send PS code to ghostscript,
 *	receive acknowledgements, and receive X events in the meantime.
 *	It also checks for SIGPIPE errors.
 */

#if HAVE_POLL
static	struct pollfd	fds[3] = {{0, POLLOUT, 0},
				  {0, POLLIN, 0},
				  {0, POLLIN, 0}};
#define	XDVI_ISSET(a, b, c)	(fds[c].revents)
#else
static	int		numfds;
static	fd_set		readfds;
static	fd_set		writefds;
#define	XDVI_ISSET(a, b, c)	FD_ISSET(a, b)
#endif

#define	LINELEN	81
static	char	line[LINELEN + 1];
static	char	*linepos	= line;
static	char	ackstr[]	= "\347\310\376";
static	char	oldstr[]	= "\347\310\375";

static	void
showto(q)
	char	*q;
{
	char	*p	= line;
	char	*p1;

	while (p < q) {
	    p1 = memchr(p, '\n', q - p);
	    if (p1 == NULL) p1 = q;
	    *p1 = '\0';
	    Printf("gs: %s\n", p);
	    p = p1 + 1;
	}
}

static	void
read_from_gs() {
	int	bytes;
	char	*line_end;
	char	*p;

	bytes = read(GS_out, linepos, line + LINELEN - linepos);
	if (bytes < 0) return;
	line_end = linepos + bytes;
	/* Check for ack strings */
	for (p = line; p < line_end - 2; ++p) {
	    p = memchr(p, '\347', line_end - p - 2);
	    if (p == NULL) break;
	    if (memcmp(p, ackstr, 3) == 0) {
		--GS_pending;
		if (debug & DBG_PS)
		    Printf("Got GS ack; %d pending.\n", GS_pending);
	    }
	    else if (memcmp(p, oldstr, 3) == 0) {
		if (debug & DBG_PS)
		    Puts("Using old GS version.");
		GS_old = True;
	    }
	    else continue;

	    showto(p);
	    p += 3;
	    (void) bcopy(p, line, line_end - p);
	    line_end -= p - line;
	    linepos = p = line;
	    --p;
	}
	*line_end = '\0';
	p = strrchr(linepos, '\n');
	if (p != NULL) {
	    ++p;
	    showto(p);
	    (void) bcopy(p, line, line_end - p);
	    line_end -= p - line;
	}
	linepos = line_end;
	/*
	 * Normally we'd hold text until a newline character, but the buffer
	 * is full.  So we flush it, being careful not to cut up an ack string.
	 */
	if (linepos >= line + LINELEN) {
	    p = line + LINELEN;
	    if ((*--p != '\347' && *--p != '\347' && *--p != '\347')
		    || (memcmp(p, ackstr, line + LINELEN - p) != 0
		    && memcmp(p, oldstr, line + LINELEN - p) != 0))
		p = line + LINELEN;
	    *p = '\0';
	    Printf("gs: %s\n", line);
	    *p = '\347';
	    linepos = line;
	    while (p < line + LINELEN) *linepos++ = *p++;
	}
}

/*
 *	For handling of SIGPIPE signals from gs_io()
 */

static	Boolean	sigpipe_error = False;

/* ARGSUSED */
static	RETSIGTYPE
gs_sigpipe_handler(sig, code, scp, addr)
	int	sig;
	int	code;
	struct sigcontext *scp;
	char	*addr;
{
	sigpipe_error = True;
}

#if HAVE_SIGACTION
static	struct sigaction sigpipe_handler_struct;
	/* initialized to {gs_sigpipe_handler, (sigset_t) 0, 0} in initGS */
#endif

/*
 *	Clean up after gs_io()
 */

static	void
post_send()
{
	if (sigpipe_error) {
	    Fputs("ghostscript died unexpectedly.\n", stderr);
	    destroy_gs();
	    draw_bbox();
	}

	if (GS_pending_int) {
	    GS_pending_int = False;
	    interrupt_gs();
	}
}

/*
 *	This routine does two things.  It either sends a string of bytes to
 *	the GS interpreter, or waits for acknowledgement from GS.
 */

#define	waitack()	gs_io(NULL, 0)

static	void
gs_io(cp, len)
	_Xconst	char	*cp;
	int		len;
{
	int		bytes;
#if HAVE_SIGACTION
	struct sigaction orig;
#else
	RETSIGTYPE	(*orig)();
#endif
	struct timeval	tv;
#if HAVE_POLL
	int		timeout	= -1;
	int		retval;
	int		offset	= 0;
#else
	struct timeval	*timeout = (struct timeval *) NULL;
#endif
#if !defined(FLAKY_SIGPOLL) && defined(FASYNC)
	int		oldflags;
#endif

	if (GS_pid < 0)
	    return;

	if (cp != NULL) {	/* if sending bytes */
#if HAVE_SIGACTION
	    (void) sigaction(SIGPIPE, &sigpipe_handler_struct, &orig);
#else
	    orig = signal(SIGPIPE, gs_sigpipe_handler);
#endif
	    sigpipe_error = False;
	    GS_send_byte = cp;
	    GS_send_end = cp + len;
	}
	else {		/* if waiting for acknowledgement */
	    if (GS_pending == 0)
		return;
#if HAVE_POLL
	    offset = 1;
#endif
	}

#if !defined(FLAKY_SIGPOLL) && defined(FASYNC)
	oldflags = fcntl(ConnectionNumber(DISP), F_GETFL, 0);
	(void) fcntl(ConnectionNumber(DISP), F_SETFL, oldflags & ~FASYNC);
#endif

	for (;;) {

	    /* Handle timeout. */

	    if (GS_timeout_set) {
		(void) X_GETTIMEOFDAY(&tv);
#if HAVE_POLL
		timeout = 1000 * (int) (GS_timeout.tv_sec - tv.tv_sec)
		    + ((long) GS_timeout.tv_usec - (long) tv.tv_usec) / 1000;
		if (timeout <= 0) {
		    destroy_gs();
		    break;
		}
#else
		if (!timercmp(&tv, &GS_timeout, <)) {
		    destroy_gs();
		    break;
		}
		tv.tv_sec = GS_timeout.tv_sec - tv.tv_sec;
		tv.tv_usec = GS_timeout.tv_usec + 1000000 - tv.tv_usec;
		if (tv.tv_usec >= 1000000) tv.tv_usec -= 1000000;
		else --tv.tv_sec;
		timeout = &tv;
#endif
	    }

	    if (terminate_flag) {
		ps_destroy();
		exit(0);
	    }

#if HAVE_POLL
	    fds[0].revents = 0;
	    for (;;) {
		retval = poll(fds + offset, XtNumber(fds) - offset, timeout);
		if (retval >= 0 || errno != EAGAIN) break;
	    }
	    if (retval < 0) {
		if (errno == EINTR)
		    continue;
		else {
		    perror("poll (xdvik gs_io)");
		    break;
		}
	    }
#else
	    FD_ZERO(&readfds);
	    FD_SET(ConnectionNumber(DISP), &readfds);
	    FD_SET(GS_out, &readfds);

	    FD_ZERO(&writefds);
	    if (GS_send_byte != NULL) FD_SET(GS_in, &writefds);

	    if (select(numfds, &readfds, &writefds, (fd_set *) NULL, timeout)
		    < 0 && errno != EINTR) {
		perror("select (xdvik gs_io)");
		break;
	    }
#endif

	    if (XDVI_ISSET(GS_out, &readfds, 1)) {
		read_from_gs();
		if (GS_pending <= 0) {
		    GS_timeout_set = False;
		    break;
		}
	    }
	    if (XDVI_ISSET(GS_in, &writefds, 0)) {
		bytes = write(GS_in, GS_send_byte, GS_send_end - GS_send_byte);
		if (bytes == -1) {
		    if (!AGAIN_CONDITION) perror("xdvik gs_io");
		}
		else {
		    GS_send_byte += bytes;
		    if (GS_send_byte == GS_send_end) {
			GS_send_byte = NULL;
			break;
		    }
		}
		if (sigpipe_error) break;
	    }
	    if (XDVI_ISSET(ConnectionNumber(DISP), &readfds, 2)) {
		ps_read_events(False, False);
		if (GS_pid < 0) break;	/* if timeout occurred */
		if (GS_pending <= 0) {
		    GS_timeout_set = False;
		    break;
		}
	    }
	}

#if !defined(FLAKY_SIGPOLL) && defined(FASYNC)
	(void) fcntl(ConnectionNumber(DISP), F_SETFL, oldflags);
#endif

	if (cp != NULL) {
	    /* put back generic handler for SIGPIPE */
#if HAVE_SIGACTION
	    (void) sigaction(SIGPIPE, &orig, (struct sigaction *) NULL);
#else
	    (void) signal(SIGPIPE, orig);
#endif
	    if (!GS_in_header)
		post_send();
	}
}

/*
 *	Fork a process to run ghostscript.  This is done using the
 *	x11 device (which needs to be compiled in).  Normally the x11
 *	device uses ClientMessage events to communicate with the calling
 *	program, but we don't do this.  The reason for using the ClientMessage
 *	events is that otherwise ghostview doesn't know when a non-conforming
 *	postscript program calls showpage.   That doesn't affect us here,
 *	since in fact we disable showpage.
 */

Boolean
initGS()
{
	char			buf[100];
	static	Boolean		did_putenv	= False;
		/*
		 * This string reads chunks (delimited by %%xdvimark).
		 * The first character of a chunk tells whether a given chunk
		 * is to be done within save/restore or not.
		 * The `H' at the end tells it that the first group is a
		 * header; i.e., no save/restore.
		 * `execute' is unique to ghostscript.
		 */
	static	_Xconst	char	str1[]	= "\
/xdvi$run {$error /newerror false put {currentfile cvx execute} stopped pop} \
  def \
/xdvi$ack (\347\310\376) def \
/xdvi$dslen countdictstack def \
{currentfile read pop 72 eq \
    {xdvi$run} \
    {xdvi$run \
      clear countdictstack xdvi$dslen sub {end} repeat } \
  ifelse \
  {(%%xdvimark) currentfile =string {readline} stopped \
    {clear} {pop eq {exit} if} ifelse }loop \
  flushpage xdvi$ack print flush \
}loop\nH";
	static	_Xconst	char	str2[]	= "[0 1 1 0 0 0] concat\n\
revision 300 lt{(\347\310\375) print flush}if\n\
stop\n%%xdvimark\n";

	gs_atom = XInternAtom(DISP, "GHOSTVIEW", False);
	/* send bpixmap, orientation, bbox (in pixels), and h & v resolution */
	Sprintf(buf, "%ld %d 0 0 %u %u 72 72",
	    None,		/* bpixmap */
	    Landscape,		/* orientation */
	    GS_page_h = page_h, GS_page_w = page_w);
	XChangeProperty(DISP, mane.win, gs_atom, XA_STRING, 8,
	    PropModeReplace, (unsigned char *) buf, strlen(buf));
	GS_alpha = resource.gs_alpha;

	gs_colors_atom = XInternAtom(DISP, "GHOSTVIEW_COLORS", False);
	Sprintf(buf, "%s %ld %ld", resource.gs_palette, fore_Pixel, back_Pixel);
	XChangeProperty(DISP, mane.win, gs_colors_atom, XA_STRING, 8,
	    PropModeReplace, (unsigned char *) buf, strlen(buf));

	if (!did_putenv) {
	    Sprintf(buf, "%ld", mane.win);
	    xputenv("GHOSTVIEW", buf);
	    xputenv("DISPLAY", XDisplayString(DISP));
	    did_putenv = True;
	}

	XSync(DISP, False);		/* update the window */

	if (xpipe(std_in) != 0 || xpipe(std_out) != 0) {
	    perror("[xdvik] pipe");
	    return False;
	}
	Fflush(stderr);		/* to avoid double flushing */
	GS_pid = vfork();
	if (GS_pid == 0) {		/* child */
	    _Xconst char **argvp = argv + 8;

	    argv[1] = resource.gs_alpha ? "-sDEVICE=x11alpha" : "-sDEVICE=x11";
	    Sprintf(arg4 + 14, "%u", GS_page_w);
	    Sprintf(arg5 + 15, "%u", GS_page_h);
	    if (resource.gs_safer) *argvp++ = "-dSAFER";
	    *argvp = "-";
	    (void) close(std_in[1]);
	    (void) dup2(std_in[0], 0);
	    (void) close(std_in[0]);
	    (void) close(std_out[0]);
	    (void) dup2(std_out[1], 1);
	    (void) dup2(std_out[1], 2);
	    (void) close(std_out[1]);
	    (void) execvp(argv[0] = resource.gs_path, (char * _Xconst *) argv);
	    Fprintf(stderr, "Execvp of %s failed.\n", argv[0]);
	    Fflush(stderr);
	    _exit(1);
	}

	(void) close(std_in[0]);
	(void) close(std_out[1]);

	if (GS_pid == -1) {	/* error */
	    perror("[xdvik] vfork");
	    (void) close(GS_in);
	    (void) close(GS_out);
	    return False;
	}

	/* Set std_in for non-blocking I/O */
	(void) fcntl(std_in[1], F_SETFL,
	    fcntl(std_in[1], F_GETFL, 0) | O_NONBLOCK);

#ifdef	_POSIX_SOURCE
	sigpipe_handler_struct.sa_handler = gs_sigpipe_handler;
	sigemptyset(&sigpipe_handler_struct.sa_mask);
#endif

#if HAVE_POLL
	fds[0].fd = std_in[1];
	fds[1].fd = std_out[0];
	fds[2].fd = ConnectionNumber(DISP);
#else
	numfds = ConnectionNumber(DISP);
	if (numfds < std_in[1]) numfds = std_in[1];
	if (numfds < std_out[0]) numfds = std_out[0];
	++numfds;
#endif

	psp = gs_procs;
	GS_active = GS_pending_int = GS_timeout_set = False;
	GS_send_byte = NULL;
	GS_in_header = True;
	GS_pending = 1;
	GS_mag = GS_shrink = -1;

	gs_io(str1, sizeof(str1) - 1);
	gs_io(psheader, psheaderlen);
	gs_io(str2, sizeof(str2) - 1);
	GS_in_header = False;
	post_send();
	waitack();

	if (GS_pid < 0) {		/* if something happened */
	    destroy_gs();
	    return False;
	}
	if (!postscript) toggle_gs();	/* if we got a 'v' already */
	else {
	    canit = True;		/* ||| redraw the page */
	    scanned_page = scanned_page_bak = scanned_page_reset;
	    longjmp(canit_env, 1);
	}
	return True;
}

static	void
toggle_gs()
{
	if (debug & DBG_PS) Puts("Toggling GS on or off");
	if (postscript) psp.drawbegin = drawbegin_gs;
	else {
	    interrupt_gs();
	    psp.drawbegin = drawbegin_none;
	}
}

static	void
destroy_gs()
{
	if (debug & DBG_PS) Puts("Destroying GS process");
	if (linepos > line) {
	    *linepos = '\0';
	    Printf("gs: %s\n", line);
	    linepos = line;
	}
	if (GS_pid >= 0) {
	    if (kill(GS_pid, SIGKILL) < 0 && errno != ESRCH)
		perror("xdvik destroy_gs");
	    GS_pid = -1;
	    scanned_page = scanned_page_bak = scanned_page_reset;
	    (void) close(GS_in);
	    (void) close(GS_out);
	}
	GS_active = GS_pending_int = GS_timeout_set = GS_in_doc = False;
	GS_send_byte = NULL;
	GS_pending = 0;
}

static	void
interrupt_gs()
{
	static	_Xconst	char	str[]	= " stop\n%%xdvimark\n";

	if (debug & DBG_PS) Puts("Running interrupt_gs()");
	if (GS_pending <= 0) return;	/* nothing to do */
	if (GS_timeout_set) return;	/* we've done this already */
	(void) X_GETTIMEOFDAY(&GS_timeout);	/* set timeout */
	GS_timeout.tv_sec += 5;
	GS_timeout_set = True;
	if (GS_send_byte != NULL) GS_pending_int = True;
	else {
	    if (GS_active) {
		/*
		 * ||| what I'd really like to do here is cause gs to execute
		 * the interrupt routine in errordict.  But so far (gs 2.6.1)
		 * that has not been implemented in ghostscript.
		 */
		gs_io(str, sizeof(str) - 1);
		GS_active = False;
	    }
	    psp.interrupt = NullProc;	/* prevent deep recursion in waitack */
	    waitack();
	    psp.interrupt = interrupt_gs;
	}
}

static	void
endpage_gs()
{
	static	_Xconst	char	str[]	= "stop\n%%xdvimark\n";

	if (debug & DBG_PS) Puts("Running endpage_gs()");
	if (GS_active) {
	    gs_io(str, sizeof(str) - 1);
	    GS_active = False;
	    waitack();
	}
}

/*
 *	Checks that the GS interpreter is running correctly.
 */

static	void
checkgs(in_header)
	Boolean	in_header;
{
	char	buf[150];

	/* For gs 2, we pretty much have to start over to enlarge the window. */
	if ((GS_old && (page_w > GS_page_w || page_h > GS_page_h))
	  || GS_alpha != resource.gs_alpha)
	    destroy_gs();

	if (GS_pid < 0)
	    (void) initGS();

	if (!GS_active) {
	    /* check whether page_w or page_h have increased */
	    if (page_w > GS_page_w || page_h > GS_page_h) {
		++GS_pending;
		Sprintf(buf, "H mark /HWSize [%d %d] /ImagingBBox [0 0 %d %d] \
currentdevice putdeviceprops pop\n\
initgraphics [0 1 1 0 0 0] concat stop\n%%%%xdvimark\n",
		    GS_page_w = page_w, GS_page_h = page_h, page_h, page_w);
		gs_io(buf, strlen(buf));
		if (!in_header) {
		    canit = True;		/* ||| redraw the page */
		    longjmp(canit_env, 1);
		}
	    }

	    if (magnification != GS_mag) {
		++GS_pending;
		Sprintf(buf, "H TeXDict begin /DVImag %d 1000 div def \
end stop\n%%%%xdvimark\n",
		    GS_mag = magnification);
		gs_io(buf, strlen(buf));
	    }

	    if (mane.shrinkfactor != GS_shrink) {
		++GS_pending;
		Sprintf(buf,
		    "H TeXDict begin %d %d div dup \
/Resolution X /VResolution X \
end stop\n%%%%xdvimark\n",
		    pixels_per_inch, GS_shrink = mane.shrinkfactor);
		gs_io(buf, strlen(buf));
	    }
	}
}

static	void
drawbegin_gs(xul, yul, cp)
	int		xul, yul;
	_Xconst	char	*cp;
{
	char	buf[32];
	static	_Xconst	char	str[]	= " TeXDict begin\n";

	checkgs(False);

	if (!GS_active) {
	    ++GS_pending;
	    gs_io(str, sizeof(str) - 1);
	    GS_active = True;
	}

	/* This allows the X side to clear the page */
	XSync(DISP, False);

	Sprintf(buf, "%d %d moveto\n", xul, yul);
	gs_io(buf, strlen(buf));
	if (debug & DBG_PS)
	    Printf("drawbegin at %d,%d:  sending `%s'\n", xul, yul, cp);
	gs_io(cp, strlen(cp));
}

static	void
drawraw_gs(cp)
	_Xconst	char	*cp;
{
	if (!GS_active)
	    return;
	if (debug & DBG_PS) Printf("raw ps sent to context: %s\n", cp);
	gs_io(cp, strlen(cp));
	gs_io("\n", 1);
}

static	void
drawfile_gs(cp, f)
	_Xconst	char	*cp;
	FILE		*f;
{
	Fclose(f);		/* don't need it */
	++n_files_left;

	if (!GS_active)
	    return;

	if (debug & DBG_PS) Printf("printing file %s\n", cp);

	gs_io("(", 1);
	gs_io(cp, strlen(cp));
	gs_io(")run\n", 5);
}

static	void
drawend_gs(cp)
	_Xconst	char	*cp;
{
	if (!GS_active)
	    return;
	if (debug & DBG_PS) Printf("end ps: %s\n", cp);
	gs_io(cp, strlen(cp));
	gs_io("\n", 1);
}

static	void
beginheader_gs()
{
	static	_Xconst	char	str[]	= "Hsave /xdvi$doc exch def\n";

	if (debug & DBG_PS) Puts("Running beginheader_gs()");

	checkgs(True);

	if (GS_active) {
	    if (!GS_in_header)
		oops("Internal error in beginheader_gs().\n");
	    return;
	}

	GS_in_header = True;
	++GS_pending;
	if (GS_in_doc)
	    gs_io("H", 1);
	else {
	    gs_io(str, sizeof(str) - 1);
	    GS_in_doc = True;
	}
	GS_active = True;
}

static	void
endheader_gs()
{
	static	_Xconst	char	str[]	= "stop\n%%xdvimark\n";

	if (debug & DBG_PS) Puts("Running endheader_gs()");

	if (GS_active) {
	    gs_io(str, sizeof(str) - 1);
	    GS_active = False;
	    GS_in_header = False;
	    post_send();
	    waitack();
	}
}

static	void
newdoc_gs()
{
	static	_Xconst	char	str[]	=
				"Hxdvi$doc restore stop\n%%xdvimark\n";

	if (debug & DBG_PS) Puts("Running newdoc_gs()");

	if (GS_in_doc) {
	    ++GS_pending;
	    gs_io(str, sizeof(str) - 1);
	    GS_mag = GS_shrink = -1;
	    GS_in_doc = False;
	}
}

#endif /* PS_GS */
