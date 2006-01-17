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

NOTES:
	This code was originally written by Ricardo Telichevesky
	(ricardo@rle-vlsi-mit.edu) and Luis Miguel Silveira
	(lms@rle-vlsi-mit.edu).
	It was largely influenced by similar code in the SeeTeX/XTeX
	package by Dirk Grunwald (grunwald@colorado.edu).

\*========================================================================*/

#ifdef PS_DPS /* whole file */

#include "xdvi-config.h"
#include <signal.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <DPS/XDPSlib.h>
#include <DPS/dpsXclient.h>
#include <DPS/dpsexcept.h>
#include <DPS/dpsclient.h>

		/*
		 * This string reads chunks (delimited by %%xdvimark).
		 * The first character of a chunk tells whether a given chunk
		 * is to be done within save/restore or not.
		 * The `H' at the end tells it that the first group is a
		 * header; i.e., no save/restore.
		 */
#ifndef	SUNHACK
static	char	preamble[]	= "\
/xdvi$line 81 string def \
/xdvi$run {{$error null ne {$error /newerror false put}if \
 currentfile cvx stopped \
 $error null eq {false} {$error /newerror get} ifelse and \
 {handleerror} if} stopped pop} def \
/xdvi$dslen countdictstack def \
{currentfile read not {exit} if 72 eq \
  {xdvi$run} \
  {/xdvi$sav save def xdvi$run \
   clear countdictstack xdvi$dslen sub {end} repeat xdvi$sav restore} \
 ifelse \
 {(%%xdvimark) currentfile xdvi$line {readline} stopped \
  {clear} {pop eq {exit} if} ifelse }loop \
 (xdvi$Ack\n) print flush \
}loop\nH";
#else	/* SUNHACK */
static	char	preamble[]	= "\
/xdvi$line 81 string def \
/xdvi$run {{$error null ne {$error /newerror false put}if \
 currentfile cvx stopped \
 $error null eq {false} {$error /newerror get} ifelse and \
 {handleerror} if} stopped pop} def \
/xdvi$dslen countdictstack def \
/xdvi$ack {{(%%xdvimark) currentfile xdvi$line {readline} stopped \
   {clear} {pop eq {exit} if} ifelse }loop \
  (xdvi$Ack\n) print flush} bind def \
errordict begin /interrupt{(xdvi$Int\n) print flush stop}bind def \
end \
{{currentfile read not {exit} if 72 eq \
   {xdvi$run} \
   {/xdvi$sav save def xdvi$run \
    clear countdictstack xdvi$dslen sub {end} repeat xdvi$sav restore} \
  ifelse \
 xdvi$ack \
 }loop \
xdvi$ack \
}loop\nH";
#endif	/* SUNHACK */

extern	char	psheader[];
extern	int	psheaderlen;

#define	postscript	resource._postscript


/* global procedures (besides initDPS) */

static	void	toggleDPS ARGS((void));
static	void	destroyDPS ARGS((void));
static	void	interruptDPS ARGS((void));
static	void	endpageDPS ARGS((void));
static	void	drawbeginDPS ARGS((int, int, _Xconst char *));
static	void	drawrawDPS ARGS((_Xconst char *));
static	void	drawfileDPS ARGS((_Xconst char *, FILE *));
static	void	drawendDPS ARGS((_Xconst char *));
static	void	beginheaderDPS ARGS((void));
static	void	endheaderDPS ARGS((void));
static	void	newdocDPS ARGS((void));

static	struct psprocs	dps_procs = {
	/* toggle */		toggleDPS,
	/* destroy */		destroyDPS,
	/* interrupt */		interruptDPS,
	/* endpage */		endpageDPS,
	/* drawbegin */		drawbeginDPS,
	/* drawraw */		drawrawDPS,
	/* drawfile */		drawfileDPS,
	/* drawend */		drawendDPS,
	/* beginheader */	beginheaderDPS,
	/* endheader */		endheaderDPS,
	/* newdoc */		newdocDPS};

static	DPSContext DPS_ctx = NULL;
static	DPSSpace DPS_space = NULL;
static	int	DPS_mag;		/* magnification currently in use */
static	int	DPS_shrink;		/* shrink factor currently in use */
static	Boolean	DPS_active;		/* if we've started a page */
static	int	DPS_pending;		/* number of ack's we're expecting */
static	Boolean	DPS_in_header;		/* if we're sending a header */
static	Boolean	DPS_in_doc;		/* if we've sent header information */


#if	0
static	void	DPSErrorProcHandler();
#else
#define	DPSErrorProcHandler	DPSDefaultErrorProc
#endif


static	char	ackstr[]	= "xdvi$Ack\n";
#ifdef	SUNHACK
static	char	intstr[]	= "xdvi$Int\n";
#endif

#define	LINELEN	81
#define	BUFLEN	(LINELEN + sizeof(ackstr))
static	char	line[BUFLEN + 1];
static	char	*linepos	= line;

static	void
TextProc(ctxt, buf, count)
	DPSContext	ctxt;
	char		*buf;
	unsigned long	count;
{
	int	i;
	char	*p;
	char	*p0;

	while (count > 0) {
	    i = line + BUFLEN - linepos;
	    if (i > count) i = count;
	    (void) bcopy(buf, linepos, i);
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
			(void) bcopy(p0, line, linepos - p0);
			linepos -= p0 - line;
		    }
		    else if (linepos == line + BUFLEN) {
			char	c;

			c = line[LINELEN];
			line[LINELEN] = '\0';
			Printf("DPS: %s\n", line);
			line[LINELEN] = c;
			linepos -= LINELEN;
			(void) bcopy(line + LINELEN, line, linepos - line);
		    }
		    break;
		}
		if (p >= p0 + 8 && memcmp(p - 8, ackstr, 9) == 0) {
		    --DPS_pending;
		    if (debug & DBG_PS)
			Printf("Got DPS ack; %d pending.\n", DPS_pending);
		    ++p;
		    (void) bcopy(p, p - 9, linepos - p);
		    linepos -= 9;
		    continue;
		}
#ifdef	SUNHACK
		if (p >= p0 + 8 && memcmp(p - 8, intstr, 9) == 0) {
		    DPS_pending = 1;
		    if (debug & DBG_PS)
			Puts("Got DPS int.");
		    ++p;
		    (void) bcopy(p, p - 9, linepos - p);
		    linepos -= 9;
		    continue;
		}
#endif	/* SUNHACK */
		*p = '\0';
		Printf("DPS: %s\n", p0);
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

static	void
waitack()
{
	while (DPS_pending > 0) {
	    (void) XEventsQueued(DISP, QueuedAfterFlush);
	    ps_read_events(False, False);
	    if (DPS_ctx == NULL) break;	/* if interrupt occurred */
	}
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

static	int
get_shift(mask)
	Pixel	mask;
{
	int	k;

	for (k = 0; (mask & 1) == 0; ++k)
	    mask >>= 1;
	return k;
}

Boolean
initDPS()
{

	/* Try to create a context */

#if GREY

	if (our_colormap == DefaultColormapOfScreen(SCRN))
	    DPS_ctx = XDPSCreateSimpleContext(DISP, mane.win, ruleGC, 0, 0,
	      TextProc, DPSDefaultErrorProc, NULL);
	else {
	    static XStandardColormap	*ccube	= NULL;
	    static XStandardColormap	*grayramp = NULL;
	    int	shift;

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

	    shift = get_shift(our_visual->red_mask);
	    ccube->red_max = our_visual->red_mask >> shift;
	    ccube->red_mult = 1 << shift;

	    shift = get_shift(our_visual->green_mask);
	    ccube->green_max = our_visual->green_mask >> shift;
	    ccube->green_mult = 1 << shift;

	    shift = get_shift(our_visual->blue_mask);
	    ccube->blue_max = our_visual->blue_mask >> shift;
	    ccube->blue_mult = 1 << shift;

	    grayramp->red_max = ccube->red_max & ccube->green_max
	      & ccube->blue_max;
	    grayramp->red_mult = ccube->red_mult + ccube->green_mult
	      + ccube->blue_mult;

	    ccube->colormap = grayramp->colormap = our_colormap;
	    ccube->visualid = grayramp->visualid = our_visual->visualid;

	    DPS_ctx = XDPSCreateContext(DISP, mane.win, ruleGC, 0, 0,
	      0, grayramp, ccube,
	      /* actual */ (ccube->red_max + 1) * (ccube->green_max + 1)
		* (ccube->blue_max + 1),
	      TextProc, DPSDefaultErrorProc, NULL);
	}

#else	/* not GREY */

	DPS_ctx = XDPSCreateSimpleContext(DISP, mane.win, ruleGC, 0, 0,
	  TextProc, DPSDefaultErrorProc, NULL);

#endif	/* not GREY */

	if (DPS_ctx == NULL)
	    return False;

	DPS_mag = DPS_shrink = -1;
	DPS_active = False;
	DPS_pending = 1;

	DPS_space = DPSSpaceFromContext(DPS_ctx);
	DPSWritePostScript(DPS_ctx, preamble, sizeof(preamble) - 1);
	DPSWritePostScript(DPS_ctx, psheader, psheaderlen);
	DPSPrintf(DPS_ctx, "matrix setmatrix stop\n%%%%xdvimark\n");
	DPSFlushContext(DPS_ctx);

	psp = dps_procs;
	return True;
}


/*---------------------------------------------------------------------------*
  toggleDPS()

  Arguments: none
  Returns: (void)
  Side-Effects: psp.drawbegin is changed.

  Description:
  Used to toggle the rendering of PostScript by the DPS server

+----------------------------------------------------------------------------*/

static	void
toggleDPS()
{
  if (debug & DBG_PS) Puts("Toggling DPS on or off");
  if (postscript) psp.drawbegin = drawbeginDPS;
  else {
    interruptDPS();
    psp.drawbegin = drawbegin_none;
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

static	void
destroyDPS()
{
  if (debug & DBG_PS)
    Puts("Calling destroyDPS()");
  if (linepos > line) {
    *linepos = '\0';
    Printf("DPS: %s\n", line);
  }
  DPSDestroySpace(DPS_space);
  psp = no_ps_procs;
  scanned_page = scanned_page_bak = scanned_page_reset;
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
interruptDPS()
{
#ifdef	SUNHACK
  static Boolean interrupting = False;
#endif

  if (debug & DBG_PS)
    Puts("Running interruptDPS()");

#ifndef	SUNHACK
  if (DPS_pending > 0)
#else	/* SUNHACK */
  if (DPS_pending > 0 && !interrupting)
#endif	/* SUNHACK */
  {
    if (debug & DBG_PS)
      Printf("interruptDPS: code is now %d\n", XDPSGetContextStatus(DPS_ctx));

    /*
     * I would really like to use DPSInterruptContext() here, but (at least
     * on an RS6000) I can't get it to work.
     */

#ifdef	SUNHACK
    /*
     * On the other hand, under OpenWindows 3.3 (at least), destroying and
     * re-creating contexts has a nasty habit of crashing the server.
     */

    interrupting = True;
    DPSInterruptContext(DPS_ctx);
    DPS_pending = 32767;
    DPSPrintf(DPS_ctx, "%%%%xdvimark\n");
    DPSFlushContext(DPS_ctx);
    DPS_active = False;
    waitack();
    interrupting = False;
#else	/* SUNHACK */
    DPSDestroyContext(DPS_ctx);
    DPS_ctx = NULL;
    DPS_active = False;
    DPS_pending = 0;
#endif	/* SUNHACK */
  }
}


/*---------------------------------------------------------------------------*
  endpageDPS()

  Arguments: none
  Returns: (void)
  Side-Effects: the DPS_active variable is cleared.

  Description:
  Should be called at the end of a page to end this chunk for the DPS server.

+----------------------------------------------------------------------------*/

static	void
endpageDPS()
{
  if (DPS_active) {
    if (debug & DBG_PS)
      Puts("Endpage sent to context");
    DPSPrintf(DPS_ctx, "stop\n%%%%xdvimark\n");
    DPSFlushContext(DPS_ctx);
    DPS_active = False;
    waitack();
  }
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

static	void
drawbeginDPS(xul, yul, cp)
  int xul, yul;
  _Xconst char *cp;
{
  /* static char faulty_display_vs[]
   * ="DECWINDOWS DigitalEquipmentCorporation UWS4.2LA"; */

  if (debug & DBG_PS)
    Printf("Begin drawing at xul= %d, yul= %d.\n", xul, yul);

  /* we assume that I cannot write the file to the postscript context */
  if (DPS_ctx == NULL) {
    DPS_ctx = XDPSCreateSimpleContext(DISP, mane.win, ruleGC, 0, 0,
				     TextProc, DPSErrorProcHandler, DPS_space);
    if (DPS_ctx == NULL) {
      psp = no_ps_procs;
      draw_bbox();
      return;
    }
    DPSWritePostScript(DPS_ctx, preamble, sizeof(preamble) - 1);
    /* it already has psheader */
    DPSPrintf(DPS_ctx, "matrix setmatrix stop\n%%%%xdvimark\n");
    DPS_mag = DPS_shrink = -1;
    DPS_active = False;
    DPS_pending = 1;
  }

  if (!DPS_active) {
    /* send initialization to context */
    if (magnification != DPS_mag) {
	DPSPrintf(DPS_ctx, "H TeXDict begin /DVImag %d 1000 div def \
end stop\n%%%%xdvimark\n",
	    DPS_mag = magnification);
	++DPS_pending;
    }
    if (mane.shrinkfactor != DPS_shrink) {
	DPSPrintf(DPS_ctx, "H TeXDict begin %d %d div dup \
/Resolution X /VResolution X \
end stop\n%%%%xdvimark\n",
	    pixels_per_inch, DPS_shrink = mane.shrinkfactor);
	++DPS_pending;
    }
    DPSPrintf(DPS_ctx, " TeXDict begin\n");
    DPS_active = True;
    ++DPS_pending;
  }

  DPSPrintf(DPS_ctx, "%d %d moveto\n", xul, yul);
  DPSPrintf(DPS_ctx, "%s\n", cp);
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

static	void
drawrawDPS(cp)
  _Xconst char *cp;
{
  if (!DPS_active)
    return;

  if (debug & DBG_PS)
    Printf("Sending raw PS to context: %s\n", cp);

  read_events(False);
  DPSPrintf(DPS_ctx,"%s\n", cp);
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

static	void
drawfileDPS(cp, psfile)
  _Xconst char *cp;
  FILE *psfile;
{
  char buffer[1025];
  int blen;

  if (!DPS_active) {
    Fclose(psfile);
    ++n_files_left;
    return;
  }

  if (debug & DBG_PS)
    Printf("sending file %s\n", cp);
  for (;;) {
    ps_read_events(False, False);
    if (canit || !DPS_active) break;	/* alt_canit is not a factor here */
    blen = fread(buffer, sizeof(char), 1024, psfile);
    if (blen == 0) break;
    DPSWritePostScript(DPS_ctx, buffer, blen);
  }
  Fclose(psfile);
  ++n_files_left;
  if (canit) {
    interruptDPS();
    longjmp(canit_env, 1);
  }
}


/*---------------------------------------------------------------------------*
  drawendDPS()

  Arguments: cp - string with indication of the end of the special
  Returns: (void)
  Side-Effects: none

  Description:
  Sends the indication of end of the figure PostScript code.

+----------------------------------------------------------------------------*/

static	void
drawendDPS(cp)
  _Xconst char *cp;
{
  if (!DPS_active)
    return;

  if (debug & DBG_PS)
    Printf("End PS: %s\n", cp);
  read_events(False);
  DPSPrintf(DPS_ctx,"%s\n", cp);
}


/*---------------------------------------------------------------------------*
  beginheaderDPS()

  Arguments: none
  Returns: (void)

  Description:
  Prepares the PostScript interpreter for receipt of header code.

+----------------------------------------------------------------------------*/

static	void
beginheaderDPS()
{
  if (debug & DBG_PS) Puts("Running beginheaderDPS()");

  if (DPS_active) {
    if (!DPS_in_header)
      oops("Internal error in beginheaderDPS().\n");
    return;
  }

  DPS_in_header = True;
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

static	void
endheaderDPS()
{
  if (debug & DBG_PS) Puts("Running endheaderDPS()");

  if (DPS_active) {
    DPSPrintf(DPS_ctx, "stop\n%%%%xdvimark\n");
    DPS_active = False;
    DPS_in_header = False;
    DPSFlushContext(DPS_ctx);
    waitack();
  }
}


/*---------------------------------------------------------------------------*
  newdocDPS()

  Arguments: none
  Returns: (void)

  Description:
  Clears out headers stored from the previous document.

+----------------------------------------------------------------------------*/

static	void
newdocDPS()
{
  if (debug & DBG_PS) Puts("Running newdocDPS()");

  if (DPS_in_doc) {
    DPSPrintf(DPS_ctx, "H xdvi$doc restore stop\n%%%%xdvimark\n");
    ++DPS_pending;
    DPS_mag = DPS_shrink = -1;
    DPS_in_doc = False;
  }
}

#endif /* PS_DPS */
