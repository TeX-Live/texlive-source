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

#ifdef PS_DPS	/* entire file */


#include "xdvi-config.h"
#include "special.h"
#include "util.h"
#include "xdvi.h"

#include <setjmp.h>
#include <signal.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <DPS/XDPSlib.h>
#include <DPS/dpsXclient.h>
#include <DPS/dpsexcept.h>
#include <DPS/dpsclient.h>

#ifdef	X_NOT_STDC_ENV
    extern	int	errno;
#endif

#if defined(sun) || defined(__sun)
#ifndef SUNHACK
#define	SUNHACK	1
#endif
#endif


#include "dvi-init.h"
#include "dvi-draw.h"
#include "events.h"

/*
 * This string reads chunks (delimited by %%xdvimark).
 * The first character of a chunk tells whether a given chunk
 * is to be done within save/restore or not.
 * The `H' at the end tells it that the first group is a
 * header; i.e., no save/restore.
 */
#if !SUNHACK
static char preamble[] =
"/xdvi$line 81 string def "
"/xdvi$run {{$error null ne {$error /newerror false put}if "
" currentfile cvx stopped "
" $error null eq {false} {$error /newerror get} ifelse and "
" {handleerror} if} stopped pop} def "
"/xdvi$dslen countdictstack def "
"{currentfile read not {exit} if 72 eq "
" {xdvi$run} "
" {/xdvi$sav save def xdvi$run "
"  clear countdictstack xdvi$dslen sub {end} repeat xdvi$sav restore} "
" ifelse "
" {(%%xdvimark) currentfile xdvi$line {readline} stopped "
"  {clear} {pop eq {exit} if} ifelse }loop "
" (xdvi$Ack\n) print flush "
"}loop\nH";
#else /* SUNHACK */
static char preamble[] =
"/xdvi$line 81 string def "
"/xdvi$run {{$error null ne {$error /newerror false put}if "
"currentfile cvx stopped "
"$error null eq {false} {$error /newerror get} ifelse and "
"{handleerror} if} stopped pop} def "
"/xdvi$dslen countdictstack def "
"/xdvi$ack {{(%%xdvimark) currentfile xdvi$line {readline} stopped "
"  {clear} {pop eq {exit} if} ifelse }loop "
"  (xdvi$Ack\n) print flush} bind def "
"errordict begin /interrupt{(xdvi$Int\n) print flush stop}bind def "
"end "
"{{currentfile read not {exit} if 72 eq "
"   {xdvi$run} "
"   {/xdvi$sav save def xdvi$run "
"    clear countdictstack xdvi$dslen sub {end} repeat xdvi$sav restore} "
"  ifelse "
" xdvi$ack "
" }loop "
"xdvi$ack "
"}loop\nH";
#endif /* SUNHACK */

extern const char psheader[];
extern unsigned psheaderlen;


/* global procedures (besides initDPS) */

static void toggleDPS(int flag);
static void destroyDPS(void);
static void interruptDPS(void);
static void endpageDPS(void);
static void drawbeginDPS(int, int, const char *);
static void drawbeginDPS_box(int, int, const char *);
static void drawrawDPS(const char *);
static void drawfileDPS(const char *, FILE *);
static void drawendDPS(const char *);
static void beginheaderDPS(void);
static void endheaderDPS(void);
static void newdocDPS(void);

static struct psprocs dps_procs = {
    /* toggle */ toggleDPS,
    /* destroy */ destroyDPS,
    /* interrupt */ interruptDPS,
    /* endpage */ endpageDPS,
    /* drawbegin */ drawbeginDPS,
    /* drawraw */ drawrawDPS,
    /* drawfile */ drawfileDPS,
    /* drawend */ drawendDPS,
    /* beginheader */ beginheaderDPS,
    /* endheader */ endheaderDPS,
    /* newdoc */ newdocDPS
};

#define	DPS_MASK_NORMAL	EV_GE_NEWPAGE
#define	DPS_MASK_HEADER	EV_GE_PS_TOGGLE
#define	DPS_MASK_INIT	(EV_GE_TERM | EV_PS_TOGGLE)

static DPSContext DPS_ctx = NULL;
static DPSSpace DPS_space = NULL;
static unsigned long DPS_mag;	/* magnification currently in use */
static int DPS_shrink;	/* shrink factor currently in use */
static Boolean DPS_active;	/* if we've started a page */
static int DPS_pending;	/* number of ack's we're expecting */
static Boolean DPS_in_header;	/* if we're sending a header */
static Boolean DPS_in_doc;	/* if we've sent header information */

static int DPS_ev_mask = DPS_MASK_NORMAL; /* events for which we'll stop */


#if	0
static void DPSErrorProcHandler(void);
#else
#define	DPSErrorProcHandler	DPSDefaultErrorProc
#endif


static char ackstr[] = "xdvi$Ack\n";
#if SUNHACK
static char intstr[] = "xdvi$Int\n";
static char intstr2[] = "xdvi$In2\n";
#endif

#define	LINELEN	81
#define	BUFLEN	(LINELEN + sizeof ackstr)
static char line[BUFLEN + 1];
static char *linepos = line;

static void
TextProc(DPSContext ctxt, char *buf, unsigned long count)
{
    unsigned long i;
    char *p;
    char *p0;

    UNUSED(ctxt);
    
    while (count > 0) {
	i = line + BUFLEN - linepos;
	if (i > count)
	    i = count;
	memmove(linepos, buf, i);
	linepos += i;
	buf += i;
	count -= i;
	p0 = line;
	for (;;) {
	    if (p0 >= linepos) {
		linepos = line;
		break;
	    }
	    p = memchr(p0, '\n', linepos - p0);
	    if (p == NULL) {
		if (p0 != line) {
		    memmove(line, p0, linepos - p0);
		    linepos -= p0 - line;
		}
		else if (linepos == line + BUFLEN) {
		    char c;

		    c = line[LINELEN];
		    line[LINELEN] = '\0';
		    printf("DPS: %s\n", line);
		    line[LINELEN] = c;
		    linepos -= LINELEN;
		    memmove(line, line + LINELEN, linepos - line);
		}
		break;
	    }
	    if (p >= p0 + 8 && memcmp(p - 8, ackstr, 9) == 0) {
		--DPS_pending;
		if (DPS_pending == 0)
		    globals.ev.flags |= EV_ACK;
		if (globals.debug & DBG_PS)
		    printf("Got DPS ack; %d pending.\n", DPS_pending);
		++p;
		memmove(p, p - 9, p, linepos - p);
		linepos -= 9;
		continue;
	    }
#if SUNHACK
	    if (p >= p0 + 8 && memcmp(p - 8, intstr, 9) == 0) {
		if (globals.debug & DBG_PS)
		    puts("Got DPS int.");
		++p;
		memmove(p - 9, p, linepos - p);
		linepos -= 9;
		globals.ev.flags |= EV_ACK;
		continue;
	    }
	    if (p >= p0 + 8 && memcmp(p - 8, intstr2, 9) == 0) {
		if (globals.debug & DBG_PS)
		    puts("Got DPS int 2.");
		++p;
		memmove(p - 9, p, linepos - p);
		linepos -= 9;
		DPS_pending = 3;

		continue;
	    }
#endif /* SUNHACK */
	    *p = '\0';
	    printf("DPS: %s\n", p0);
	    p0 = p + 1;
	}
    }
}


/*---------------------------------------------------------------------------*
  waitack()

  Arguments: none.

  Returns: (void)

  Description:
  Waits until the requisite number of acknowledgements has been received from
  the context.

  +----------------------------------------------------------------------------*/

#if SUNHACK
static void DPS_alarm(struct xtimer *this, void *data);

static struct xtimer DPS_timer = {NULL, {0, 0}, XTM_DEFAULT, DPS_alarm, NULL
#if XDVI_XT_TIMER_HACK
				  , NULL, NULL
#endif
};

static Boolean DPS_timer_set;

static void DPS_alarm(struct xtimer *this, void *data)
{
    UNUSED(this);
    UNUSED(data);
    if (globals.debug & DBG_PS)
	puts("Received DPS alarm");

    DPS_timer_set = False;
    globals.ev.flags |= EV_ACK;
}
#endif /* SUNHACK */

static void
waitack(void)
{
    if (DPS_pending <= 0)
	return;

    DPSFlushContext(DPS_ctx);
    if (read_events(DPS_ev_mask | EV_ACK) & EV_ACK) {
	globals.ev.flags &= ~EV_ACK;
	return;
    }

    if (globals.debug & DBG_PS)
	printf("Interrupting DPS in waitack(); code is now %d %x\n",
	       XDPSGetContextStatus(DPS_ctx), globals.ev.flags);

    DPS_active = DPS_in_header = DPS_in_doc = False;
    DPS_ev_mask = DPS_MASK_NORMAL;

    /*
     * I would really like to use DPSInterruptContext() here, but (at least
     * on an RS6000) I can't get it to work.
     */

#if SUNHACK

    /*
     * On the other hand, under OpenWindows 3.3 (at least), destroying and
     * re-creating contexts has a nasty habit of crashing the server.
     */

    DPS_pending = 32767;
    DPSInterruptContext(DPS_ctx);
    DPSFlushContext(DPS_ctx);
    (void) read_events(EV_GE_TERM | EV_ACK);
    globals.ev.flags &= ~EV_ACK;

    XSync(DISP, False);
    DPSPrintf(DPS_ctx, " stop\n%%%%xdvimark\n stop\n%%%%xdvimark\n");
    DPSPrintf(DPS_ctx, " (xdvi$In2\n) print flush stop\n%%%%xdvimark\n");
    DPSPrintf(DPS_ctx, " stop\n%%%%xdvimark\n stop\n%%%%xdvimark\n");
    DPSFlushContext(DPS_ctx);

    if (globals.debug & DBG_PS)
	puts("Setting DPS timer");

    DPS_timer_set = True;
    set_timer(&DPS_timer, 500);
    (void) read_events(EV_GE_TERM | EV_ACK);
    globals.ev.flags &= ~EV_ACK;

    if (DPS_pending <= 0) {
	if (DPS_timer_set) {
	    cancel_timer(&DPS_timer);
	}

	return;
    }

    /*
     * However, under Solaris 2.6 (at least), sometimes interrupting the
     * context leaves it in an uncommunicative state, so destruction
     * is the only alternative.
     */

    if (globals.debug & DBG_PS)
	puts("Plan B:  Destroying DPS context");

#endif /* SUNHACK */

    DPSDestroyContext(DPS_ctx);
    DPS_ctx = NULL;
    DPS_pending = 0;
}


/*---------------------------------------------------------------------------*
  initDPS()

  Arguments: (none)
  Returns: (void)
  Side-Effects: DPS_ctx may be set as well as other static variables.

  Description:
  Initializes variables from the application main loop.  Checks to see if
  a connection to the DPS server can be opened.

  +----------------------------------------------------------------------------*/

static int
get_shift(Pixel mask)
{
    int k;

    for (k = 0; (mask & 1) == 0; ++k)
	mask >>= 1;
    return k;
}

Boolean initDPS(void)
{

    /* Try to create a context */

#if GREY

    if (G_colormap == DefaultColormapOfScreen(SCRN))
	DPS_ctx = XDPSCreateSimpleContext(DISP, mane.win, globals.gc.copy, 0, 0,
					  TextProc, DPSDefaultErrorProc, NULL);
    else {
	static XStandardColormap *ccube = NULL;
	static XStandardColormap *grayramp = NULL;
	int shift;

	if (grayramp == NULL) {
	    grayramp = XAllocStandardColormap();
	    if (grayramp == NULL)
		return False;
	}

	if (ccube == NULL) {
	    ccube = XAllocStandardColormap();
	    if (ccube == NULL)
		return False;
	}

	shift = get_shift(G_visual->red_mask);
	ccube->red_max = G_visual->red_mask >> shift;
	ccube->red_mult = 1 << shift;

	shift = get_shift(G_visual->green_mask);
	ccube->green_max = G_visual->green_mask >> shift;
	ccube->green_mult = 1 << shift;

	shift = get_shift(G_visual->blue_mask);
	ccube->blue_max = G_visual->blue_mask >> shift;
	ccube->blue_mult = 1 << shift;

	grayramp->red_max = ccube->red_max & ccube->green_max & ccube->blue_max;
	grayramp->red_mult = ccube->red_mult + ccube->green_mult
	    + ccube->blue_mult;

	ccube->colormap = grayramp->colormap = G_colormap;
	ccube->visualid = grayramp->visualid = G_visual->visualid;

	DPS_ctx = XDPSCreateContext(DISP, mane.win, globals.gc.copy, 0, 0,
				    0, grayramp, ccube,
				    /* actual */
				    (ccube->red_max + 1) * (ccube->green_max +
							    1) *
				    (ccube->blue_max + 1), TextProc,
				    DPSDefaultErrorProc, NULL);
    }

#else /* not GREY */

    DPS_ctx = XDPSCreateSimpleContext(DISP, mane.win, globals.gc.copy, 0, 0,
				      TextProc, DPSDefaultErrorProc, NULL);

#endif /* not GREY */

    if (DPS_ctx == NULL)
	return False;

    DPS_mag = DPS_shrink = -1;
    DPS_active = False;
    DPS_pending = 1;

    DPS_space = DPSSpaceFromContext(DPS_ctx);
    DPSWritePostScript(DPS_ctx, preamble, (sizeof preamble) - 1);
    DPSWritePostScript(DPS_ctx, (char *)psheader, psheaderlen);
    DPSPrintf(DPS_ctx, "matrix setmatrix stop\n%%%%xdvimark\n");
    DPSFlushContext(DPS_ctx);

    psp = dps_procs;
    return True;
}


/*---------------------------------------------------------------------------*
  toggleDPS(int flag)

  Arguments: flag for toggling PostScript
  Returns: (void)
  Side-Effects: psp.drawbegin is changed.

  Description:
  Used to toggle the rendering of PostScript by the DPS server
  This routine may be called from within read_events().

  +----------------------------------------------------------------------------*/

static void
toggleDPS(int flag)
{
    if (globals.debug & DBG_PS)
	fprintf(stderr, "Toggling DPS to %d", flag);

    switch (flag) {
    case 0:
	psp.drawbegin = drawbegin_none;
	break;
    case 1:
	psp.drawbegin = drawbeginDPS;
	break;
    default:
	psp.drawbegin = drawbeginDPS_box;
	break;
    }
}


/*---------------------------------------------------------------------------*
  destroyDPS()

  Arguments: none
  Returns: (void)
  Side-Effects: the context is nulled out and destroyed.

  Description:
  Close the connection to the DPS server; used when rendering is terminated
  in any way.

  +----------------------------------------------------------------------------*/

static void
destroyDPS(void)
{
    if (globals.debug & DBG_PS)
	puts("Calling destroyDPS()");
    if (linepos > line) {
	*linepos = '\0';
	printf("DPS: %s\n", line);
    }
    if (DPS_space != NULL)
	DPSDestroySpace(DPS_space);
    psp = no_ps_procs;
    scanned_page = scanned_page_ps = scanned_page_reset;
}


/*---------------------------------------------------------------------------*
  interruptDPS()

  Arguments: none
  Returns: (void)
  Side-Effects: the context may be nulled out and destroyed.

  Description:
  Close the connection to the DPS server; used when rendering is terminated
  because of an interruption in the viewing of the current page.

  +----------------------------------------------------------------------------*/

static	void
interruptDPS(void)
{
    if (globals.debug & DBG_PS)
	puts("Running interruptDPS()");

    if (DPS_pending <= 0)
	return;	/* nothing to do */

    if (DPS_active) {
	DPSPrintf(DPS_ctx, "stop\n%%%%xdvimark\n");
	DPS_active = False;
    }

    waitack();
}


/*---------------------------------------------------------------------------*
  endpageDPS()

  Arguments: none
  Returns: (void)
  Side-Effects: the DPS_active variable is cleared.

  Description:
  Should be called at the end of a page to end this chunk for the DPS server.

  +----------------------------------------------------------------------------*/

static void
endpageDPS(void)
{
    if (DPS_active) {
	if (globals.debug & DBG_PS)
	    puts("Endpage sent to context");
	DPSPrintf(DPS_ctx, "stop\n%%%%xdvimark\n");
	DPS_active = False;
	waitack();
    }
}


/*
 *	checkDPS - Check that the DPS interpreter is still running.
 *	Return True for success, False for failure.
 */

static	Boolean
checkDPS(void)
{
    /* static char faulty_display_vs[]
     * ="DECWINDOWS DigitalEquipmentCorporation UWS4.2LA"; */

    if (DPS_ctx == NULL) {
	DPS_ctx = XDPSCreateSimpleContext(DISP, mane.win, globals.gc.copy, 0, 0,
					  TextProc, DPSErrorProcHandler, DPS_space);
	if (DPS_ctx == NULL) {
	    psp = no_ps_procs;
	    draw_bbox();
	    return False;
	}
	DPSWritePostScript(DPS_ctx, preamble, (sizeof preamble) - 1);
	/* it already has psheader */
	DPSPrintf(DPS_ctx, "matrix setmatrix stop\n%%%%xdvimark\n");
	DPS_mag = DPS_shrink = -1;
	DPS_active = False;
	DPS_pending = 1;
    }

    return True;
}


/*---------------------------------------------------------------------------*
  drawbeginDPS  ()

  Arguments: xul, yul - coordinates of the upper left corner of the figure
  cp - string with the bounding box line data
  Returns: (void)
  Side-Effects: DPS_ctx is set is set and connection to DPS server is
  opened.

  Description:
  Opens a connection to the DPS server and send in the preamble and the
  bounding box information after correctly computing resolution factors.
  In case no rendering is to be done, outlines the figure.
  An outline is also generated whenever the a context cannot be allocated

  +----------------------------------------------------------------------------*/

static void
drawbeginDPS(int xul, int yul, const char *cp)
{
    if (globals.debug & DBG_PS)
	printf("Begin drawing at xul= %d, yul= %d.\n", xul, yul);

    if (!checkDPS()) return;

    if (!DPS_active) {
	/* send initialization to context */
	if (magnification != DPS_mag) {
	    DPSPrintf(DPS_ctx, "H TeXDict begin /DVImag %d 1000 div def "
		      "end stop\n%%%%xdvimark\n", DPS_mag = magnification);
	    ++DPS_pending;
	}
	if (mane.shrinkfactor != DPS_shrink) {
	    DPSPrintf(DPS_ctx, "H TeXDict begin %d %d div dup "
		      "/Resolution X /VResolution X "
		      "end stop\n%%%%xdvimark\n", resource.pixels_per_inch, DPS_shrink = mane.shrinkfactor);
	    ++DPS_pending;
	}
	DPSPrintf(DPS_ctx, " TeXDict begin\n");
	DPS_active = True;
	++DPS_pending;
    }

    if (globals.ev.flags & DPS_MASK_NORMAL)
	longjmp(globals.ev.canit, 1);

    DPSPrintf(DPS_ctx, "%d %d moveto\n", xul, yul);
    DPSPrintf(DPS_ctx, "%s\n", cp);
}

static void
drawbeginDPS_box(int xul, int yul, const char *cp)
{
    drawbeginDPS(xul, yul, cp);
    draw_bbox();
}

/*---------------------------------------------------------------------------*

drawrawDPS()

Arguments: cp - the raw string to be sent to the postscript interpreter
Returns: (void)
Side-Effects: (none)

Description:
If there is a valid postscript context, just send the string to the
interpreter, else leave.

+----------------------------------------------------------------------------*/

static void
drawrawDPS(const char *cp)
{
    if (!DPS_active || (globals.ev.flags & DPS_ev_mask))
	return;

    if (globals.debug & DBG_PS)
	printf("Sending raw PS to context: %s\n", cp);

    DPSPrintf(DPS_ctx, "%s\n", cp);
}


/*---------------------------------------------------------------------------*
  drawfileDPS()

  Arguments: cp - string with the postscript file pathname
  psfile - opened file pointer
  Returns: (void)
  Side-Effects: none

  Description:
  Postscript file containing the figure is opened and sent to the DPS server.

  +----------------------------------------------------------------------------*/

static void
drawfileDPS(const char *cp, FILE *psfile)
{
    char buffer[1025];
    int blen;

    if (DPS_active && !(read_events(EV_NOWAIT) & DPS_ev_mask)) {
	if (globals.debug & DBG_PS)
	    printf("sending file %s\n", cp);
	for (;;) {
	    blen = fread(buffer, sizeof(char), 1024, psfile);
	    if (blen == 0) break;
	    DPSWritePostScript(DPS_ctx, buffer, blen);
	    if (read_events(EV_NOWAIT) & DPS_ev_mask) {
		/* ||| stop at a good place */
		if (globals.debug & DBG_PS)
		    puts("Interrupting in drawfileDPS");
		break;
	    }
	}
    }

    fclose(psfile);

    if (globals.ev.flags & DPS_ev_mask)
	longjmp(globals.ev.canit, 1);
}


/*---------------------------------------------------------------------------*
  drawendDPS()

  Arguments: cp - string with indication of the end of the special
  Returns: (void)
  Side-Effects: none

  Description:
  Sends the indication of end of the figure PostScript code.

  +----------------------------------------------------------------------------*/

static void
drawendDPS(const char *cp)
{
    if (!DPS_active || (globals.ev.flags & DPS_MASK_NORMAL))
	return;

    if (globals.debug & DBG_PS)
	printf("End PS: %s\n", cp);
    DPSPrintf(DPS_ctx, "%s\n", cp);
}


/*---------------------------------------------------------------------------*
  beginheaderDPS()

  Arguments: none
  Returns: (void)

  Description:
  Prepares the PostScript interpreter for receipt of header code.

  +----------------------------------------------------------------------------*/

static void
beginheaderDPS(void)
{
    if (globals.ev.flags & DPS_MASK_HEADER || !checkDPS())
	return;

    if (globals.debug & DBG_PS)
	puts("Running beginheaderDPS()");

    if (DPS_active) {
	if (!DPS_in_header)
	    XDVI_FATAL((stderr, "Internal error in beginheaderDPS()."));
	return;
    }

    DPS_in_header = True;
    DPS_ev_mask = DPS_MASK_HEADER;

    if (DPS_in_doc)
	DPSPrintf(DPS_ctx, "H");
    else {
	DPSPrintf(DPS_ctx, "Hsave /xdvi$doc exch def\n");
	DPS_in_doc = True;
    }
    DPS_active = True;
    ++DPS_pending;
}


/*---------------------------------------------------------------------------*
  endheaderDPS()

  Arguments: none
  Returns: (void)

  Description:
  Prepares the PostScript interpreter for receipt of header code.

  +----------------------------------------------------------------------------*/

static void
endheaderDPS(void)
{
    if (globals.debug & DBG_PS)
	puts("Running endheaderDPS()");

    if (DPS_active) {
	DPSPrintf(DPS_ctx, "stop\n%%%%xdvimark\n");
	DPS_active = False;
	waitack();
	DPS_in_header = False;
	DPS_ev_mask = DPS_MASK_NORMAL;
    }
}


/*---------------------------------------------------------------------------*
  newdocDPS()

  Arguments: none
  Returns: (void)

  Description:
  Clears out headers stored from the previous document.

  +----------------------------------------------------------------------------*/

static void
newdocDPS(void)
{
    if (globals.debug & DBG_PS)
	puts("Running newdocDPS()");

    if (DPS_in_doc) {
	DPSPrintf(DPS_ctx, "H xdvi$doc restore stop\n%%%%xdvimark\n");
	++DPS_pending;
	DPS_mag = DPS_shrink = -1;
	DPS_in_doc = False;
    }
}

#else
/* silence `empty compilation unit' warnings */
static void bar(void); static void foo(void) { bar(); } static void bar(void) { foo(); }
#endif /* PS_DPS */
