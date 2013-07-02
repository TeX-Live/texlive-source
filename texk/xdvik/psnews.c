/*========================================================================*\

Copyright (c) 1994-2004  Paul Vojta

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

NOTES:
This code was originally written by Ricardo Telichevesky
(ricardo@rle-vlsi-mit.edu) and Luis Miguel Silveira
(lms@rle-vlsi-mit.edu).
It was largely influenced by similar code in the SeeTeX/XTeX
package by Dirk Grunwald (grunwald@colorado.edu).

\*========================================================================*/

    /* ||| To do:
     *	ALWAYS_CLOSE_SERVER_CONNECTION?
     *	Is there some way of interrupting a process?
     *	fork
     *	extra bytes on input
     */

#ifdef PS_NEWS	/* whole file */

#include "xdvi-config.h"
#include "xdvi.h"
#include "events.h"
#include "dvi-init.h"
#include "dvi-draw.h"

#include <signal.h>
#include <sys/file.h>	/* this defines FASYNC */
#include <X11/X.h>
#include <X11/Xlib.h>
#undef SYSV	/* To avoid defined SYSV_{WAIT,UCONTEXT} in xview/notify.h.  */
#include <NeWS/psio.h>
#include <xvps/pscanvas.h>

    /* Condition for retrying a write */
#include <errno.h>
#include <setjmp.h>

    /* if POSIX O_NONBLOCK is not available, use O_NDELAY */
#if !defined(O_NONBLOCK) && defined(O_NDELAY)
#define	O_NONBLOCK O_NDELAY
#endif

#ifdef	X_NOT_STDC_ENV
    extern int errno;
#endif

#ifdef	EWOULDBLOCK
#ifdef	EAGAIN
#define	AGAIN_CONDITION	(errno == EWOULDBLOCK || errno == EAGAIN)
#else /* EAGAIN */
#define	AGAIN_CONDITION	(errno == EWOULDBLOCK)
#endif /* EAGAIN */
#else /* EWOULDBLOCK */
#ifdef	EAGAIN
#define	AGAIN_CONDITION	(errno == EAGAIN)
#endif /* EAGAIN */
#endif /* EWOULDBLOCK */

#if HAVE_POLL
# include <poll.h>
# define XIO_IN POLLIN
# define XIO_OUT POLLOUT
#else
# define XIO_IN 1
# define XIO_OUT 2
#endif


#if !defined(FLAKY_SIGPOLL) && !HAVE_STREAMS && !defined(FASYNC)
# define FLAKY_SIGPOLL	1
#endif

char *strtok(char *, const char *);

/* define ALWAYS_CLOSE_SERVER_CONNECTION if you want to close the server
   connection all the time */
#undef	ALWAYS_CLOSE_SERVER_CONNECTION


/*
 * Some setup code.
 */
static const char str0[] =
"/OW2? version cvi 2 eq def "
"OW2? "
"{ /setlinewidth { pop } def} "
"{ /NeWS 3 0 findpackage beginpackage "
"  /X11 3 0 findpackage beginpackage} "
"ifelse "
"currentcanvas /Color get "
"currentcanvas /Colormap get getcubedescription null eq and "
"   {8 {{currentcanvas /Colormap get 1 index dup dup dup newcube} stopped "
"    {pop pop pop pop pop} {exit} ifelse "
"    2 div cvi dup 1 eq {exit} if} loop pop} "
"if\n";
/*
 * This string reads chunks (delimited by %%xdvimark).
 * The first character of a chunk tells whether a given chunk
 * is to be done within save/restore or not.
 * The `H' at the end tells it that the first group is a
 * header; i.e., no save/restore.
 */
static const char preamble[] =
"/xdvi$line 81 string def "
"/xdvi$run {$error null ne {$error /newerror false put} if "
" {currentfile cvx stopped "
" $error null eq {false} {$error /newerror get} ifelse and "
" {handleerror} if} stopped pop} def "
"/xdvi$dslen countdictstack def "
"{currentfile read not {exit} if 72 eq "
"    {xdvi$run} "
"    {/xdvi$sav save def xdvi$run "
"      clear countdictstack xdvi$dslen sub {end} repeat xdvi$sav restore} "
"  ifelse "
"  {(%%xdvimark) currentfile xdvi$line {readline} stopped "
"    {clear} {{eq {false exit} if} {true exit} ifelse} ifelse }loop {exit} if "
"  58 tagprint flush "
"}loop\nH";

extern const char psheader[];
extern int psheaderlen;

static const char preamble2[] = " stop\n%%xdvimark\n";
#define	stopstring	preamble2

/* global procedures (besides initNeWS) */

static void toggleNeWS(int flag);
static void destroyNeWS(void);
static void interruptNeWS(void);
static void endpageNeWS(void);
static void drawbeginNeWS(int, int, const char *);
static void drawbeginNeWS_box(int, int, const char *);
static void drawrawNeWS(const char *);
static void drawrawNeWS_box(const char *);
static void drawfileNeWS(const char *, FILE *);
static void drawendNeWS(const char *);
static void beginheaderNeWS(void);
static void endheaderNeWS(void);
static void newdocNeWS(void);

static struct psprocs news_procs = {
    /* toggle */ toggleNeWS,
    /* destroy */ destroyNeWS,
    /* interrupt */ interruptNeWS,
    /* endpage */ endpageNeWS,
    /* drawbegin */ drawbeginNeWS,
    /* drawraw */ drawrawNeWS,
    /* drawfile */ drawfileNeWS,
    /* drawend */ drawendNeWS,
    /* beginheader */ beginheaderNeWS,
    /* endheader */ endheaderNeWS,
    /* newdoc */ newdocNeWS
};

/* define local static variables */
static int NeWS_mag;	/* magnification currently in use */
static int NeWS_shrink;	/* shrink factor currently in use */
static unsigned int NeWS_page_w;	/* how big our current page is */
static unsigned int NeWS_page_h;
static Boolean NeWS_active;	/* if we've started a page yet */
static int NeWS_pending;	/* number of ack's we're expecting */
static const char *NeWS_send_byte;	/* next byte to send to NeWS */
static const char *NeWS_send_end;	/* last + 1 byte to send */
static Boolean NeWS_in_header;	/* if we're sending a header */
static Boolean NeWS_in_doc;	/* if we've sent header information */
static int NeWS_ev_mask;	/* events for which we'll stop */
static Boolean NeWS_destroyed = False;

#define	NEWS_MASK_NORMAL	EV_GE_NEWPAGE
#define	NEWS_MASK_HEADER	EV_GE_PS_TOGGLE
#define	NEWS_MASK_INIT		(EV_GE_TERM | EV_PS_TOGGLE)


/*
 *	NeWS I/O code.  This should send PS code to NeWS,
 *	receive acknowledgements, and receive X events in the meantime.
 *	It also checks for SIGPIPE errors.
 */

static void read_from_NeWS(void);
static void write_to_NeWS(void);

static	struct xio	NeWS_xout	= {NULL, 0, XIO_IN,
#if HAVE_POLL
					   NULL,
#endif
					   read_from_NeWS, write_to_NeWS, NULL};

static	struct xio	NeWS_xin	= {NULL, 0, XIO_IN,
#if HAVE_POLL
					   NULL,
#endif
					   read_from_NeWS, NULL, NULL};


/*---------------------------------------------------------------------------*
  psio_sigpipe_handler  ()

  Arguments: sig, code, scp, addr (see man page for signal)
  Returns: (void)
  Side-Effects: SIGPIPE signal is flagged as sigpipe_error variable is set.

  Description:
  Handler for SIGPIPE error generated by a broken pipe in the connection
  to the NeWS server; this may be duer to some abnormal condition, or some
  hairy PostScript code containing commands not implemented by the server.

  +----------------------------------------------------------------------------*/

static Boolean sigpipe_error = False;

static struct sigaction psio_sigpipe_handler_struct;
/* initialized to {psio_sigpipe_handler, (sigset_t) 0, 0} in initNeWS */

static RETSIGTYPE
psio_sigpipe_handler(int sig, int code, struct sigcontext *scp, char *addr)
{
    sigpipe_error = True;
}


/*
 *	read_from_NeWS - This does the actual retrieving of acknowledgements.
 *	If other bytes appear on the file - tough.
 */

static void
read_from_NeWS(void)
{
    for (;;) {
	int retval;

	retval = ps_checkfor(PostScriptInput, PSIO_FIND_TAG, 58);
	if (retval == 0)
	    break;
	if (retval < 0) {
	    fprintf(stderr, "[xdvik] ps_checkfor: %d\n", retval);
	    return;
	}
	(void)ps_checkfor(PostScriptInput, PSIO_WAIT_TAG, 58);
	--NeWS_pending;
	if (NeWS_pending == 0)
	    globals.ev.flags |= EV_ACK;
	if (globals.debug & DBG_PS)
	    printf("Got NeWS ack; %d pending.\n", NeWS_pending);
    }
}


/*
 *	write_to_NeWS - Write to the PostScript interpreter.
 */

static	void
write_to_NeWS(void)
{
    int	old_flags;
    int	bytes;

    old_flags = fcntl(PostScript->file, F_GETFL, 0);
    if (old_flags < 0) return;
    /* set to be non-blocking */
    if (fcntl(PostScript->file, F_SETFL, old_flags | O_NONBLOCK) < 0)
	return;

    for (;;) {
	bytes = write(PostScript->file, NeWS_send_byte,
		      NeWS_send_end - NeWS_send_byte);
	if (bytes < 0) {
	    if (AGAIN_CONDITION)
		break;
	    perror("xdvi NeWS_send");
	    break;
	}
	NeWS_send_byte += bytes;
	if (NeWS_send_byte == NeWS_send_end) {
	    NeWS_send_byte = NULL;
	    NeWS_xout.xio_events &= ~XIO_OUT;
#if HAVE_POLL
	    /* This check is necessary, since write_to_NeWS can be called
	       directly. */
	    if (NeWS_xout.pfd != NULL)
		NeWS_xout.pfd->events &= ~POLLOUT;
	    globals.ev.flags |= EV_ACK;
	    break;
#endif
	}
    }

    fcntl(PostScript->file, F_SETFL, old_flags);
}


/*
 *	Clean up after NeWS_send()
 */

static void
post_send(void)
{
    if (sigpipe_error) {
	fputs("NeWS died unexpectedly.\n", stderr);
	destroyNeWS();
	draw_bbox();
    }
}


/*
 *	Main routine for writing to the NeWS interpreter.
 */

static	void
NeWS_send(const char *cp, size_t len)
{
    struct sigaction orig;

    if (PostScript == (PSFILE *) NULL || (globals.ev.flags & NeWS_ev_mask))
	return;

    (void) sigaction(SIGPIPE, &psio_sigpipe_handler_struct, &orig);
    sigpipe_error = False;

    NeWS_send_byte = cp;
    NeWS_send_end = cp + len;
    NeWS_xout.xio_events |= XIO_OUT;
#if HAVE_POLL
    if (NeWS_xout.pfd != NULL)
	NeWS_xout.pfd->events |= POLLOUT;
#endif

    write_to_NeWS();
    (void) read_events(NeWS_ev_mask | EV_ACK);

    if (!(globals.ev.flags & EV_ACK)) {	/* if interrupted */
	/* ||| Do somthing more severe here */
    }

    globals.ev.flags &= ~EV_ACK;

    /* put back generic handler for SIGPIPE */
    (void) sigaction(SIGPIPE, &orig, (struct sigaction *) NULL);

    if (!NeWS_in_header)
	post_send();
}

/*
 *	Wait for acknowledgement from NeWS.  With NeWS we have no choice but
 *	to wait (||| I think).
 */

static void
waitack(void)
{
    if (PostScript == (PSFILE *) NULL)
	return;

    if (NeWS_pending > 0)
	(void) read_events(EV_GE_ACK);

    if (globals.ev.flags & EV_ACK) {
	globals.ev.flags &= ~EV_ACK;
	return;
    }

    /* ||| Do something more serious here */
}


/*---------------------------------------------------------------------------*
  initNeWS()

  Arguments: None.
  Returns: True if and only if initialization succeeded
  Side-Effects: Static variables may be set.

  Description:
  Initializes variables for the application main loop.

  +----------------------------------------------------------------------------*/

Boolean initNeWS(void)
{
    static NeWStoken newstoken;

    /* now try to open the connection to the NeWS server */
    if (ps_open_PostScript() == (PSFILE *) NULL)
	return False;

    psio_sigpipe_handler_struct.sa_handler = psio_sigpipe_handler;
    sigemptyset(&psio_sigpipe_handler_struct.sa_mask);

#if !FLAKY_SIGPOLL
    if (fcntl(PostScript->file, F_SETOWN, getpid()) == -1)
	perror("xdvi: fcntl F_SETOWN");
    if (fcntl(PostScript->file, F_SETFL,
	      fcntl(PostScript->file, F_GETFL, 0) | FASYNC) == -1)
	perror("xdvi: fcntl F_SETFL");
#endif /* not FLAKY_SIGPOLL */
    if (PostScriptInput->file != PostScript->file) {
#if !FLAKY_SIGPOLL
	if (fcntl(PostScriptInput->file, F_SETOWN, getpid()) == -1)
	    perror("xdvi: fcntl F_SETOWN");
	if (fcntl(PostScriptInput->file, F_SETFL,
		  fcntl(PostScriptInput->file, F_GETFL, 0) | FASYNC) == -1)
	    perror("xdvi: fcntl F_SETFL");
#endif /* not FLAKY_SIGPOLL */
	NeWS_xout.xio_events &= ~XIO_IN;
	NeWS_xin.fd = PostScriptInput->file;
	set_io(&NeWS_xin);
    }
    NeWS_xout.fd = PostScript->file;
    set_io(&NeWS_xout);

    NeWS_active = False;
    NeWS_in_header = True;
    NeWS_ev_mask = NEWS_MASK_INIT;
    NeWS_pending = 1;

    ps_flush_PostScript();
    NeWS_send(str0, (sizeof str0) - 1);
    /* get xid of window, then make this window the NeWS canvas */
    (void)ps_token_from_xid(mane.win, &newstoken);
    if (newstoken != -1) {
	ps_setcanvas(newstoken);
	ps_flush_PostScript();

	NeWS_send(preamble, (sizeof preamble) - 1);
	NeWS_send(psheader, psheaderlen);
	NeWS_send(preamble2, (sizeof preamble2) - 1);
	NeWS_in_header = False;
	post_send();
	waitack();
    }

    if (NeWS_destroyed)
	return False;

    /* success */

    NeWS_mag = NeWS_shrink = -1;
    NeWS_page_w = globals.page.w;
    NeWS_page_h = globals.page.h;

    psp = news_procs;
    if (!resource.postscript)
	toggleNeWS(0);	/* if we got a 'v' already */

    return True;
}


/*---------------------------------------------------------------------------*
  toggleNeWS(int flag)

  Arguments: flag for toggling PostScript
  Returns: (void)
  Side-Effects: psp.drawbegin is changed

  Description:
  Used to toggle the rendering of PostScript by the NeWS server
  Callable from within read_events().

  +----------------------------------------------------------------------------*/

static void
toggleNeWS(int flag)
{
    if (globals.debug & DBG_PS)
	fprintf(stdout, "Toggling NeWS to %d", flag);

    switch (flag) {
    case 0:
	psp.drawbegin = drawbegin_none;
	break;
    case 1:
	psp.drawbegin = drawbeginNeWS;
	break;
    default:
	psp.drawbegin = drawbeginNeWS_box;
	break;
    }
}


/*---------------------------------------------------------------------------*
  destroyNeWS()

  Arguments: none
  Returns: (void)
  Side-Effects: the pointer to the NeWS file is nulled

  Description:
  Close the connection to the NeWS server; used when rendering is terminated
  in any way.

  +----------------------------------------------------------------------------*/

static void
destroyNeWS(void)
{
    psp = no_ps_procs;
    NeWS_destroyed = True;
    scanned_page = scanned_page_ps = scanned_page_reset;
}


/*---------------------------------------------------------------------------*
  interruptNeWS()

  Arguments: none
  Returns: void

  Description:
  Close the connection to the NeWS server; used when rendering is terminated
  because of an interruption in the viewing of the current page.
  ||| It would be nice if we could asynchronously ``wake up'' a NeWS process
  (preferably by sending something along the X socket); then we could do
  better than just to wait.

  +----------------------------------------------------------------------------*/

static void
interruptNeWS(void)
{
    if (globals.debug & DBG_PS)
	puts("Running interruptNeWS()");
    if (NeWS_pending <= 0) return;	/* if nothing to do */

    if (NeWS_active) {
	NeWS_send(stopstring, (sizeof stopstring) - 1);
	NeWS_active = False;
    }

    for (;;) {
	ps_flush_PostScript();
	if (NeWS_pending <= 0) break;
	waitack();
    }
}


/*---------------------------------------------------------------------------*
  endpageNeWS()

  Arguments: none
  Returns: (void)
  Side-Effects: the NeWS_active variable is cleared.

  Description:
  Should be called at the end of a page to end this chunk for the NeWS server.

  +----------------------------------------------------------------------------*/

static void
endpageNeWS(void)
{
    if (globals.debug & DBG_PS)
	puts("endpage sent to NeWS Server");
    if (NeWS_active) {
	NeWS_send(stopstring, (sizeof stopstring) - 1);
	NeWS_active = False;
	waitack();
    }
}


/*---------------------------------------------------------------------------*
  drawbeginNeWS  ()

  Arguments: xul, yul - coordinates of the upper left corner of the figure
  cp - string with the bounding box line data
  Returns: (void)

  Description:
  Opens a connection to the NeWS server and send in the preamble and the
  bounding box information after correctly computing resolution factors.
  In case no rendering is to be done, outlines the figure.  An outline is
  also generated whenever the PostScript code is too hairy and generates
  a SIGPIPE signal.

  +----------------------------------------------------------------------------*/

static void
drawbeginNeWS(int xul, int yul, const char *cp)
{
    char buf[100];
    static const char str[] = " TeXDict begin\n";
    static const char str2[] = "Hinitgraphics stop\n%%xdvimark\n";

    if (globals.debug & DBG_PS) {
	printf("xul= %d yul= %d\n", xul, yul);
	printf("String = < %s >\n", cp);
    }

    /* catch up on the X side */
    XSync(DISP, False);

    if (!NeWS_active) {
	/* send initialization to NeWS server */
	if (NeWS_page_w < globals.page.w || NeWS_page_h < globals.page.h) {
	    if (globals.ev.flags & NEWS_MASK_NORMAL)
		longjmp(globals.ev.canit, 1);
	    ++NeWS_pending;
	    NeWS_page_w = globals.page.w;
	    NeWS_page_h = globals.page.h;
	    NeWS_send(str2, (sizeof str2) - 1);
	}
	if (magnification != NeWS_mag) {
	    if (globals.ev.flags & NEWS_MASK_NORMAL)
		longjmp(globals.ev.canit, 1);
	    ++NeWS_pending;
	    sprintf(buf, "H TeXDict begin /DVImag %d 1000 div def "
		    "end stop\n%%%%xdvimark\n",
		    NeWS_mag = magnification);
	    NeWS_send(buf, strlen(buf));
	}
	if (mane.shrinkfactor != NeWS_shrink) {
	    if (globals.ev.flags & NEWS_MASK_NORMAL)
		longjmp(globals.ev.canit, 1);
	    ++NeWS_pending;
	    sprintf(buf, "H TeXDict begin %d %d div dup "
		    "/Resolution X /VResolution X "
		    "end stop\n%%%%xdvimark\n",
		    resource.pixels_per_inch,
		    NeWS_shrink = mane.shrinkfactor);
	    NeWS_send(buf, strlen(buf));
	}
	if (globals.ev.flags & NEWS_MASK_NORMAL)
	    longjmp(globals.ev.canit, 1);
	++NeWS_pending;
	NeWS_send(str, (sizeof str) - 1);
	NeWS_active = True;
    }

    sprintf(buf, "%d %d moveto\n", xul, yul);
    NeWS_send(buf, strlen(buf));
    NeWS_send(cp, strlen(cp));
}

static void
drawbeginNeWS_box(int xul, int yul, const char *cp)
{
    drawbeginNeWS(xul, yul, cp);
    draw_bbox();
}


/*---------------------------------------------------------------------------*
  drawrawNeWS()

  Arguments: origcp - the raw string to be sent to the postscript interpreter
  Returns: (void)
  Side-Effects: (none)

  Description:
  If there is a valid connection to the NeWS server, just send the string to
  the interpreter, else leave.

  +----------------------------------------------------------------------------*/

static void
drawrawNeWS(const char *origcp)
{
    const char *pt, *ptm1, *ocp1;
    static char *cp;
    char *cp1;
    static unsigned int cplen = 0;
    unsigned int len;
    double angle;
    Boolean found = False;

    if (!NeWS_active)
	return;

    if (globals.debug & DBG_PS)
	printf("Raw PS sent to context: <%s>\n", origcp);

    /* take a look at the string:  NeWS bums on certain rotations */
    len = strlen(origcp) + 4;
    if (cplen < len) {
	if (cplen != 0)
	    free(cp);
	cplen = len;
	cp = xmalloc(cplen);
    }
    ocp1 = origcp;
    pt = origcp;
    while (*pt == ' ' || *pt == '\t')
	++pt;
    cp1 = cp;
    for (;;) {
	ptm1 = pt;
	while (*pt != '\0' && *pt != ' ' && *pt != '\t')
	    ++pt;
	if (*pt == '\0')
	    break;
	while (*pt == ' ' || *pt == '\t')
	    ++pt;
	if (strncmp(pt, "rotate", 6) == 0
	    && (pt[6] == '\0' || pt[6] == ' ' || pt[6] == '\t')) {
	    /* found rotate; check angle */
	    if (sscanf(ptm1, "%lf", &angle) >= 1) {
		found = True;
		while (angle > 360.0)
		    angle -= 360;
		while (angle < -360.0)
		    angle += 360;
		if (angle == 90.0) {
		    angle = 89.999;
		    (void)memcpy(cp1, ocp1, ptm1 - ocp1);
		    cp1 += ptm1 - ocp1;
		    strcpy(cp1, "89.999 rotate ");
		    cp1 += strlen(cp1);
		    while (*pt != '\0' && *pt != ' ' && *pt != '\t')
			++pt;
		    while (*pt == ' ' || *pt == '\t')
			++pt;
		    ocp1 = pt;
		}
		else if (angle == -90.0) {
		    angle = -89.999;
		    (void)memcpy(cp1, ocp1, ptm1 - ocp1);
		    cp1 += ptm1 - ocp1;
		    strcpy(cp1, "-89.999 rotate ");
		    cp1 += strlen(cp1);
		    while (*pt != '\0' && *pt != ' ' && *pt != '\t')
			++pt;
		    while (*pt == ' ' || *pt == '\t')
			++pt;
		    ocp1 = pt;
		}
		else if (angle == 0.0) {
		    (void)memcpy(cp1, ocp1, ptm1 - ocp1);
		    cp1 += ptm1 - ocp1;
		    while (*pt != '\0' && *pt != ' ' && *pt != '\t')
			++pt;
		    while (*pt == ' ' || *pt == '\t')
			++pt;
		    ocp1 = pt;
		}
	    }
	}
    }
    strcpy(cp1, ocp1);
    if ((globals.debug & DBG_PS) && found) {
	printf("String is now <%s>\n", cp);
	printf("Found rotate string.  Angle is %g degrees.\n", angle);
    }

    len = strlen(cp);
    cp[len] = '\n';
    NeWS_send(cp, len + 1);
}


/*---------------------------------------------------------------------------*
  drawfileNeWS()

  Arguments: cp - string with the postscript file pathname
  psfile - file, already opened
  Returns: (void)
  Side-Effects: none

  Description:
  Postscript file containing the figure is opened and sent to the NeWS server.
  Figure is outlined in case hairy code produces a SIGPIPE signal.

  +----------------------------------------------------------------------------*/

static void
drawfileNeWS(const char *cp, FILE *psfile)
{
    char buffer[1025];
    int blen;
    struct sigaction orig;

    if (!NeWS_active) {
	fclose(psfile);
	return;
    }

    if (globals.debug & DBG_PS)
	printf("printing file %s\n", cp);

    sigpipe_error = False;
    for (;;) {
	blen = fread(buffer, sizeof(char), 1024, psfile);
	if (blen == 0) break;
	NeWS_send(buffer, blen);
	if (sigpipe_error || (globals.ev.flags & NeWS_ev_mask))
	    break;
    }
    fclose(psfile);

    if (sigpipe_error) {
	fputs("NeWS died unexpectedly.\n", stderr);
	destroyNeWS();
	draw_bbox();
    }
}


/*---------------------------------------------------------------------------*
  drawendNeWS()

  Arguments: cp - string with indication of the end of the special
  Returns: (void)

  Description:
  Sends the indication of end of the figure PostScript code.

  +----------------------------------------------------------------------------*/

static void
drawendNeWS(const char *cp)
{
    if (!NeWS_active)
	return;

    if (globals.debug & DBG_PS)
	puts("drawend sent to NeWS Server");
    NeWS_send(cp, strlen(cp));
    NeWS_send("\n", 1);
}


/*---------------------------------------------------------------------------*
  beginheaderNeWS()

  Arguments: none
  Returns: (void)

  Description:
  Prepares the PostScript interpreter for receipt of header code.

  +----------------------------------------------------------------------------*/

static void
beginheaderNeWS(void)
{
    static const char str[] = "Hsave /xdvi$doc exch def\n";

    if (globals.debug & DBG_PS)
	puts("Running beginheaderNeWS()");

    if (NeWS_active) {
	if (!NeWS_in_header)
	    XDVI_FATAL((stderr, "Internal error in beginheaderNeWS()."));
	return;
    }

    if (globals.ev.flags & NEWS_MASK_HEADER)
	longjmp(globals.ev.canit, 1);

    NeWS_in_header = True;
    NeWS_ev_mask = NEWS_MASK_HEADER;
    ++NeWS_pending;
    if (NeWS_in_doc)
	NeWS_send("H", 1);
    else {
	NeWS_send(str, (sizeof str) - 1);
	NeWS_in_doc = True;
    }
    NeWS_active = True;
}


/*---------------------------------------------------------------------------*
  endheaderNeWS()

  Arguments: none
  Returns: (void)

  Description:
  Prepares the PostScript interpreter for receipt of header code.

  +----------------------------------------------------------------------------*/

static void
endheaderNeWS(void)
{
    static const char str[] = "stop\n%%xdvimark\n";

    if (globals.debug & DBG_PS)
	puts("Running endheaderNeWS()");

    if (NeWS_active) {
	NeWS_send(str, (sizeof str) - 1);
	NeWS_active = False;
	post_send();
	waitack();
	NeWS_in_header = False;
	NeWS_ev_mask = NEWS_MASK_NORMAL;
    }
}


/*---------------------------------------------------------------------------*
  newdocNeWS()

  Arguments: none
  Returns: (void)

  Description:
  Clears out headers stored from the previous document.

  +----------------------------------------------------------------------------*/

static void
newdocNeWS(void)
{
    static const char str[] = "H xdvi$doc restore stop\n%%xdvimark\n";

    if (globals.debug & DBG_PS)
	puts("Running newdocNeWS()");

    if (NeWS_in_doc) {
	++NeWS_pending;
	NeWS_send(str, (sizeof str) - 1);
	NeWS_mag = NeWS_shrink = -1;
	NeWS_in_doc = False;
    }
}
#else
/* silence `empty compilation unit' warnings */
static void bar(void); static void foo(void) { bar(); } static void bar(void) { foo(); }
#endif /* PS_NEWS */
