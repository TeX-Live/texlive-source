/* 
   psgsw32.c : implementation file
   Time-stamp: "03/05/12 22:30:37 popineau"
   
   Copyright (C) 1999
      Fabrice Popineau <Fabrice.Popineau@supelec.fr>

   This file is part of Windvi.

   Windvi is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.
   
   Windvi is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.
   
   You should have received a copy of the GNU General Public License
   along with Windvi; if not, write to the Free Software Foundation, 59
   Temple Place - Suite 330, Boston, MA 02111-1307, USA. */


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

/*
  FP: 22/07/99
  All this mess should be cleaned up.
  Two instances of gsdll are needed for windvi.
  One for displaying PS code in hWndDraw,
  the other one for the magnifying glass or the band printing.
  A third one might be needed for sending code from dvips to printer,
  but I expect it to be different.

  So trying to sort out what is needed to get 2 instances
  of gsdll, and which variables must be duplicated.

  Now: we  can  wonder if it  is  realistic to  allow  PS code  in the
  mag. glass.  This could be done however.

  OK. Yap does not draw specials either in the magnifying glass.

  The result would probably be far too slow to be useful.

  So up to now, stick with rules and glyphs unless we are printing.
  That does not mean the code should not be made more modular and clean !

  */

#include <win32lib.h>

#define HAVE_BOOLEAN
#include "xdvi-config.h"

#if defined(PS_GS) && defined(WIN32) /* whole file */

#ifdef KPATHSEA
#include <kpathsea/c-proto.h>
#include <kpathsea/c-pathmx.h>
#include <kpathsea/fn.h>
#endif
#ifndef	PATH_MAX
#define	PATH_MAX 512
#endif

#define _Windows
#include "gsdll.h"

#if 0
extern	_Xconst	char	*psheader[];
extern	int		psheaderlen;
#endif

char buf[2048];

#define	postscript	resource._postscript
#define	fore_Pixel	resource._fore_Pixel
#define	back_Pixel	resource._back_Pixel

/* global procedures (besides initGS) */

static	void	toggle_gs ARGS((void));
static	void	destroy_gs ARGS((void));
static	void	interrupt_gs ARGS((void));
static	void	endpage_gs ARGS((void));
static	void	drawbegin_gs ARGS((int, int, _Xconst char *));
static	void	drawraw_gs ARGS((_Xconst char *));
static	void	drawfile_gs ARGS((_Xconst char *, FILE *, _Xconst char *));
static	void	drawend_gs ARGS((_Xconst char *));
static	void	beginheader_gs ARGS((void));
static	void	endheader_gs ARGS((void));
static	void	newdoc_gs ARGS((void));

struct psprocs	gs_procs = {
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

/* some arguments are filled in later */
static  char    arg2[]  = "-sDEVICE=xxxxxxxx";
static	char	arg4[]	= "-dDEVICEWIDTH=xxxxxxxxxx";
static	char	arg5[]	= "-dDEVICEHEIGHT=xxxxxxxxxx";
static  char    arg6[]  = "-dDEVICEXRESOLUTION=xxxxxxxxxx";
static  char    arg7[]  = "-dDEVICEYRESOLUTION=xxxxxxxxxx";

static	char	*argv[]	= {"gswin32", arg2, "-dNOPAUSE", 
			   "-dMaxBitmap=16000000", "-q",
			   arg4, arg5, arg6, arg7,
			   NULL, NULL, NULL, NULL};

static	pid_t		GS_pid;
static	unsigned int	GS_page_w;	/* how big our current page is */
static	unsigned int	GS_page_h;
static	Boolean		GS_alpha;	/* if we are using the alpha driver */
static	int		GS_mag;		/* magnification currently in use */
static	int		GS_shrink;	/* shrink factor currently in use */
static	Boolean		GS_active;	/* if we've started a page yet */
static	Boolean		GS_in_header;	/* if we're sending a header */
static	Boolean		GS_in_doc;	/* if we've sent header information */
static	Boolean		GS_old;		/* if we're using gs 2.xx */

#if 0
The messages used by the callback are:
#define GSDLL_STDIN 1   /* get count characters to str from stdin */
/* return number of characters read */
#define GSDLL_STDOUT 2  /* put count characters from str to stdout*/
/* return number of characters written */
#define GSDLL_DEVICE 3  /* device = str has been opened if count=1 */
/*                    or closed if count=0 */
#define GSDLL_SYNC 4    /* sync_output for device str */ 
#define GSDLL_PAGE 5    /* output_page for device str */
#define GSDLL_SIZE 6    /* resize for device str */
/* LOWORD(count) is new xsize */
/* HIWORD(count) is new ysize */
#define GSDLL_POLL 7    /* Called from gp_check_interrupt */
/* Can be used by caller to poll the message queue */
/* Normally returns 0 */
/* To abort gsdll_execute_cont(), return a */
/* non zero error code until gsdll_execute_cont() */
/* returns */
#endif

unsigned char *gs_device = NULL;
Boolean psToDisplay = False;

HANDLE hGsEvent = NULL;

/*
  Transformation matrix
  */

extern XFORM xfrmTransf;
extern XFORM xfrmUnit;

int __cdecl
gsdll_callback(int message, char *str, unsigned long count)
{
  char *p;

  switch (message) {
  case GSDLL_STDIN:
    p = fgets(str, count, stdin);
#if 0
    fprintf(stderr, "gs read: %s\n", p);
#endif
    if (p)
      return strlen(str);
    else
      return 0;
  case GSDLL_STDOUT:
    if (str != (char *)NULL) {
#ifdef TRANSFORM
      /* try to parse the current matrix */
      if (*str == '[' 
	  && sscanf(str+1, "%f %f %f %f %f %f",
		    &xfrmTransf.eM11, &xfrmTransf.eM12, 
		    &xfrmTransf.eM21, &xfrmTransf.eM22, 
		    &xfrmTransf.eDx, &xfrmTransf.eDy) == 6) {
	/* [ 1.0 0.0 0.0 -1.0 0.0 page_h ] is the default matrix for PS
	   Not for Win32 ! 
	   However, those transformation matrices are easy to invert
	   because almost idempotent.
	*/
	xfrmTransf.eM12 = - xfrmTransf.eM12;
	xfrmTransf.eM22 = - xfrmTransf.eM22;
	xfrmTransf.eDy = (isPrinting ? /* mane.base_y + */ mane.max_y - mane.min_y : page_h) - xfrmTransf.eDy;
	bMatrixUnit = (fabs(xfrmTransf.eM11 - 1.0) 
		       + fabs(xfrmTransf.eM12)
		       + fabs(xfrmTransf.eM21)
		       + fabs(xfrmTransf.eM22 - 1.0)
		       + fabs(xfrmTransf.eDx)
		       + fabs(xfrmTransf.eDy)) < .001 ;
	if (bMatrixUnit) {
	  xfrmTransf.eM11 = xfrmUnit.eM11;
	  xfrmTransf.eM12 = xfrmUnit.eM12;
	  xfrmTransf.eM21 = xfrmUnit.eM21;
	  xfrmTransf.eM22 = xfrmUnit.eM22;
	  xfrmTransf.eDx = xfrmUnit.eDx;
	  xfrmTransf.eDy = xfrmUnit.eDy;
	}
#if 0
	fprintf(stderr, "New %s matrix(%x) : [%f %f %f %f %f %f]\nw = %d h = %d %s\n",
		&xfrmTransf,
		(bMatrixUnit ? "" : "(not unit)"),
		xfrmTransf.eM11, xfrmTransf.eM12, 
		xfrmTransf.eM21, xfrmTransf.eM22, 
		xfrmTransf.eDx, xfrmTransf.eDy,
		(isPrinting ? mane.max_x - mane.min_x : page_w),
		(isPrinting ? mane.max_y - mane.min_y : page_h),
		(isPrinting ? "(print)" : "(display)"));
#endif
	/* Synchronize gs32dll with windvi */
/* 	SetEvent(hGsEvent); */
      }
      else {
	fwrite(str, 1, count, stdout);
      }
#else
      fwrite(str, 1, count, stdout);
#endif
    }
    return count;
  case GSDLL_DEVICE:
    gs_device = str;
#if 0
    fprintf(stdout,"Callback: DEVICE %p %s\n", str,
	    count ? "open" : "close");
#endif
    break;
  case GSDLL_SYNC:
#if 0
    fprintf(stdout,"Callback: SYNC %p\n", str);
#endif
    break;
  case GSDLL_PAGE:
    fprintf(stdout,"Callback: PAGE %p\n", str);
    break;
  case GSDLL_SIZE:
#if 0
    fprintf(stdout,"Callback: SIZE %p width=%d height=%d\n", str,
	    (int)(count & 0xffff), (int)((count>>16) & 0xffff) );
#endif
    break;
  case GSDLL_POLL:
#if 0
    fprintf(stderr, "GS: Poll sent!\n");
#endif
    return 0; /* no error */
  default:
    fprintf(stdout,"Callback: Unknown message=%d\n",message);
    break;
  }
  return 0;
}

extern HWND hWndDraw;

/*
 *	Clean up after gs_io()
 */

#define post_send() 

#define gs_stop() { gs_io("\nstop\n%%xdvimark\n"); }

/*
 *	This routine does two things.  It either sends a string of bytes to
 *	the GS interpreter, or waits for acknowledgement from GS.
 */

#define waitack()

void
gs_io(const char *cp)
{
  int ret;

#if 0
  if (cp && !(*cp == '\n' && *(cp+1) == '\0'))
    fprintf(stderr, "gs: sending %s\n", cp);
#endif

  if ((ret = (*pgsdll_execute_cont)(cp, strlen(cp))) != 0) {
    fprintf(stderr, "gs: error in executing\n%s\n", cp);
    if (ret <= -100) {
      fprintf(stderr, "gs: fatal error, exiting\n");
      (*pgsdll_exit)();
      GS_active = GS_in_doc = False;
    }
    else if (ret < 0) {
      fprintf(stderr, "gs: error, exiting\n");
      (*pgsdll_execute_end)();
      (*pgsdll_exit)();
      GS_active = GS_in_doc = False;
    }
  }
}

#ifdef TRANSFORM
XFORM *gs_getmatrix() 
{
  if (!GS_active || !resource.use_xform) return NULL;

/*   ResetEvent(hGsEvent); */
  fprintf(stderr, "requesting current matrix...\n");
  gs_io(" matrix currentmatrix == flush\n");
/*   switch (WaitForSingleObject(hGsEvent, 100)) { */
/*   case WAIT_OBJECT_0: */
/* #if 0 */
/*     fprintf(stderr, "gs_getmatrix() succeeded.\n"); */
/* #endif */
/*     break; */
/*   case WAIT_TIMEOUT: */
/*     fprintf(stderr, "Gs did not return on time from getmatrix.\n"); */
/*     break; */
/*   case WAIT_FAILED: */
/*     break; */
/*   default: */
/*     fprintf(stderr, "WaitForSingleObject failed for unknown reason.\n"); */
/*   } */
#if 0
	fprintf(stderr, "gs_getmatrix() : new %s matrix(%x) : [%f %f %f %f %f %f]\nw = %d h = %d %s\n",
		&xfrmTransf,
		(bMatrixUnit ? "" : "(not unit)"),
		xfrmTransf.eM11, xfrmTransf.eM12, 
		xfrmTransf.eM21, xfrmTransf.eM22, 
		xfrmTransf.eDx, xfrmTransf.eDy,
		(isPrinting ? mane.max_x - mane.min_x : page_w),
		(isPrinting ? mane.max_y - mane.min_y : page_h),
		(isPrinting ? "(print)" : "(display)"));
#endif
  return &xfrmTransf;
}
#else
#define gs_getmatrix()
#endif

void 
init_gs_colors()
{
  float fore_red, fore_green, fore_blue;
  float back_red, back_green, back_blue;
  COLORREF new_back_Pixel;
  extern COLORREF get_back_color(int);
  static	_Xconst	char	str3[]	= 
    " erasepage newpath 0 0 moveto %d 0 rlineto\n\
0 %d rlineto %d 0 rlineto %f %f %f setrgbcolor fill\n\
%f %f %f setrgbcolor\n\
TeXDict begin\n";

  new_back_Pixel = get_back_color(current_page);

#if 0
  fprintf(stderr, "init_gs_colors back %-8x fore %-8x\n",
	  new_back_Pixel, fore_Pixel);
  /* FIXME : fore_Pixel is not taken into account.
     Maybe try to send the order way before. Does not seem
     to be ok :-( */
#endif

  fore_red = GetRValue(fore_Pixel)/255;
  fore_green = GetGValue(fore_Pixel)/255;
  fore_blue = GetBValue(fore_Pixel)/255;
  back_red = GetRValue(new_back_Pixel)/255;
  back_green = GetGValue(new_back_Pixel)/255;
  back_blue = GetBValue(new_back_Pixel)/255;

  if (isPrinting)
    sprintf(buf, str3, page_w, mane.max_y - mane.min_y, -page_w,
	    back_red, back_green, back_blue,
	    fore_red, fore_green, fore_blue);
  else
    sprintf(buf, str3, page_w, page_h, -page_w,
	    back_red, back_green, back_blue,
	    fore_red, fore_green, fore_blue);
  gs_io(buf);
}        

void
init_ps_page(int llx, int lly, int urx, int ury, int height)
{
  const char str[] =
    " mark /HWSize [%d %d] /ImagingBBox [%d %d %d %d]\n\
/TextAlphaBits %d /GraphicsAlphaBits %d currentdevice putdeviceprops setdevice \n\
[1.0 0.0 0.0 -1.0 0 %u] setmatrix\n";
  sprintf(buf, str, GS_page_w, GS_page_h, llx, lly, urx, ury,
	  (use_grey && resource.gs_alpha ? 4 : 1), 
(use_grey && resource.gs_alpha ? 4 : 1), height);
  gs_io(buf);
#if 0
  fprintf(stderr, "init_ps_page (%d x %d) h = %d, isPrinting = %d\n", urx - llx, ury - lly, height, isPrinting);
  if (isPrinting)
    __asm int 3;
#endif
}

void
feed_gs_with (char *filename)
{
  FILE *f;
  static char buf[1024];
  int c, c_prev;
  unsigned long i = 0, doseps = 0, dosepsbegin = 0, dosepsend = 0;

  if (!filename) {
    fprintf(stderr, "Can't feed gs with (null)!\n");
    return;
  }
  f = fopen(filename, FOPEN_RBIN_MODE);
  if (!f) {
    fprintf(stderr, "Can't feed gs with %s: %s\n", filename, strerror(errno));
    return;
  }
#if 0
  fprintf(stderr, "opening %s\n", filename);
#endif

  i = 0; c_prev = '\0';
  while ((c = fgetc(f)) != EOF) {
    if (c == '\n') {
      if (c_prev == '\r') {
	i--;
      }
      buf[i++] = '\n';
      buf[i] = '\0';
      gs_io(buf);
      i = 0;
      c_prev = '\0';
    }
    else if (i == sizeof(buf) - 1) {
      buf[i] = '\0';
      gs_io(buf);
      i = 0;
      buf[i] = c;
      c_prev = c;
    }
    else {
      buf[i++] = c;
      c_prev = c;
    }
  }
  if (!feof(f)) {
    fprintf(stderr, "Can't feed gs with %s: %s\n", filename, strerror(errno));
  }
  fclose(f);
#if 0
  fprintf(stderr, "closing %s\n", filename);
#endif
}

Boolean
initGS()
{
  int ret, i;
  char **argvp = argv+9;
  char *gs_dir;

  /*
   * This string reads chunks (delimited by %%xdvimark).
   * The first character of a chunk tells whether a given chunk
   * is to be done within save/restore or not.
   * The `H' at the end tells it that the first group is a
   * header; i.e., no save/restore.
   * `execute' is unique to ghostscript.
   */
  static const char str1[] = "\
/xdvi$run {$error /newerror false put {currentfile cvx execute} stopped pop} def \
/xdvi$dslen countdictstack def \
{currentfile read not {exit} if 72 eq \
{xdvi$run} \
{xdvi$run \
clear countdictstack xdvi$dslen sub {end} repeat } \
ifelse \
{(%%xdvimark) currentfile =string {readline} stopped \
{clear} {pop eq {exit} if} ifelse }loop \
flushpage \
} loop\nH";

#if 0
  debug = DBG_PS;
#endif

  if (!hGsDll) {
    if (gs_locate() == NULL) {
      Win32Error("initGS: Can't find gsdll32.dll.");
      return False;
    }

    gs_dll_initialize();
  }

#if 0
  fprintf(stderr, "Initializing GS width = %d, height = %d\n",
          page_w, 
          (isPrinting ? mane.max_y - mane.min_y : page_h));
#endif
  if (isPrinting && isPrintingDvips) {
    Sprintf(arg2+strlen("-sDEVICE="), "%s", "mswinpr2");
    Sprintf(arg4+strlen("-dDEVICEWIDTH="), "%u", GS_page_w = page_w);
    Sprintf(arg5+strlen("-dDEVICEHEIGHT="), "%u", GS_page_h = page_h);
    Sprintf(arg6+strlen("-dDEVICEXRESOLUTION="), "%u", resource._pixels_per_inch /* maneLogPixelsX */);
    Sprintf(arg7+strlen("-dDEVICEYRESOLUTION="), "%u", resource._pixels_per_inch /* maneLogPixelsY */);
  }
  else {
    Sprintf(arg2+strlen("-sDEVICE="), "%s", "mswindll");
    Sprintf(arg4+strlen("-dDEVICEWIDTH="), "%u", GS_page_w = page_w);
    Sprintf(arg5+strlen("-dDEVICEHEIGHT="), "%u", GS_page_h = (isPrinting ? mane.max_y - mane.min_y : page_h));
    Sprintf(arg6+strlen("-dDEVICEXRESOLUTION="), "%u", 72 /* maneLogPixelsX */);
    Sprintf(arg7+strlen("-dDEVICEYRESOLUTION="), "%u", 72 /* maneLogPixelsY */);
  }
  
  GS_alpha = resource.gs_alpha;

  if (resource.gs_safer) *argvp++ = "-dSAFER";
  *argvp++ = "-dNOEPS";
  *argvp++ = "-";

#if 0
  fprintf(stderr, "gsdll_init :\n");
  for(i = 0; i < argvp - argv; i++)
    fprintf(stderr, "argv[%d] = %s\n", i, argv[i]);
#endif

  hGsEvent = CreateEvent(NULL, FALSE, FALSE, "gsEvent");

  if ((ret = (*pgsdll_init)(gsdll_callback, 
			    hWndDraw, argvp - argv - 1, argv)) != 0) {
    fprintf(stderr, "gsdll_init returned %d\n", ret);
    return False;
  }
  (*pgsdll_execute_begin)();

  psp = gs_procs;
  GS_active =  False;
  GS_in_header = True;
  GS_mag = GS_shrink = -1;
  
  gs_io(str1);

#if 1
  /* Since GS 8.00, files can't be opened from any location. */
  feed_gs_with(kpse_find_file("tex.pro", kpse_tex_ps_header_format, True));
  feed_gs_with(kpse_find_file("special.pro", kpse_tex_ps_header_format, True));
  feed_gs_with(kpse_find_file("color.pro", kpse_tex_ps_header_format, True));
#else
#if 0
  for(i = 0; psheader[i]; i++) {
    gs_io(psheader[i]);
    gs_io("\n");
  }
#else
  /* FIXME : better ensure that the files are found ! */
  Sprintf(buf, "(%s) run\n", kpse_find_file("tex.pro", kpse_tex_ps_header_format, True));
  gs_io(buf);
  Sprintf(buf, "(%s) run\n", kpse_find_file("special.pro", kpse_tex_ps_header_format, True));
  gs_io(buf);
  Sprintf(buf, "(%s) run\n", kpse_find_file("color.pro", kpse_tex_ps_header_format, True));
  gs_io(buf);
#endif
#endif

  if (isPrinting)
    init_ps_page(0, 0,
                 mane.max_x - mane.min_x, 
                 mane.max_y - mane.min_y,
		 mane.base_y + mane.max_y);
  else
    init_ps_page(0, 0, page_w, page_h, page_h);
  
  gs_io("revision 300 lt{(GS version ok.) print flush}if \n");
 
  gs_stop();

  if (!postscript) toggle_gs();	/* if we got a 'v' already */
		     else {
#if 0
		       /* FP : Makes the prescanning fail in case of psfrag for example ... */
		       canit = True;		/* ||| redraw the page */
		       scanned_page = scanned_page_bak = scanned_page_reset;
		       fprintf(stderr, "scanned_page = %d, scanned_page_reset = %d\n",
			       scanned_page, scanned_page_reset);
		       /* longjmp(canit_env, 1); */
#endif
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
  int ret;

  if (debug & DBG_PS) Puts("Destroying GS process");
  
  interrupt_gs();

  if ((ret = (*pgsdll_execute_end)()) != 0) {
    fprintf(stderr, "gsdll_execute_end returns %d\n", ret);
  }
  if ((ret = (*pgsdll_exit)()) != 0) {
    fprintf(stderr, "gsdll_exit returns %d\n", ret);
  }
  GS_active = GS_in_doc = False;

  if (hGsEvent)
	CloseHandleAndClear(&hGsEvent);

}

static	void
interrupt_gs()
{
  if (debug & DBG_PS) Puts("Running interrupt_gs()");
  if (GS_active) {
    gs_stop();
    GS_active = False;
  }
}

static	void
endpage_gs()
{
  if (debug & DBG_PS) Puts("Running endpage_gs()");
  if (GS_active) {
    gs_stop();
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
  static BOOL old_use_grey = False,
    old_gs_alpha = False;

  if (!GS_active) {
    /* check whether page_w or page_h have increased */
    /*    if (page_w > GS_page_w || page_h > GS_page_h) { */

    if (page_w != GS_page_w 
	|| page_h != GS_page_h
	|| old_use_grey != use_grey
	|| old_gs_alpha != resource.gs_alpha) {
      gs_io("H initgraphics\n");

      if (isPrinting) {
	GS_page_w = page_w;
	GS_page_h = mane.max_y - mane.min_y;
	init_ps_page(0,0,
                     mane.max_x - mane.min_x, 
                     mane.max_y - mane.min_y,
		     mane.base_y + mane.max_y);
      }
      else {
	GS_page_w = page_w; 
	GS_page_h = page_h;
	init_ps_page(0, 0, page_w, page_h, page_h);
      }
      gs_stop();

#if 0
      if (!in_header) {
	canit = True;		/* ||| redraw the page */
	longjmp(canit_env, 1);
      }
#endif
    }
    
    if (magnification != GS_mag) {
      Sprintf(buf, "H TeXDict begin /DVImag %d 1000 div def \
end\n",
	      GS_mag = magnification);
      gs_io(buf);
      gs_stop();
    }
    if (mane.shrinkfactor != GS_shrink) {
      Sprintf(buf, "H TeXDict begin %d %d div dup \
/Resolution X /VResolution X end\n",
	      pixels_per_inch, GS_shrink = mane.shrinkfactor);
      gs_io(buf);
      gs_stop();
    }
  }
}

static  void
drawbegin_gs(xul, yul, cp)
     int             xul, yul;
     char    *cp;
{
  checkgs(False);

  if (!GS_active) {
    init_gs_colors();
    GS_active = True;
  }

  Sprintf(buf, "%d %d moveto\n", xul, yul);
  gs_io(buf);
  if (debug & DBG_PS)
    Printf("drawbegin at %d,%d:  sending `%s'\n", xul, yul, cp);
  gs_io(cp);
  /*  gs_getmatrix(); */
}

static  void
drawraw_gs(cp)
     char *cp;
{
  if (!GS_active)
    return;
  if (debug & DBG_PS) Printf("raw ps sent to context: %s\n", cp);
#if 0
  gs_io("TeXDict begin @defspecial\n");
#endif
  gs_io(cp);
  gs_io("\n");
#if 0
  gs_io("@fedspecial end\n");
#endif
  /*  gs_getmatrix(); */
}

static  void
drawfile_gs(cp, f, truename)
     char *cp;
     FILE *f;
     char *truename;
{
  char *p;
  struct
  {
    unsigned char magic[4];
    unsigned char start[4];
    unsigned char length[4];
  } epsfheader;
  unsigned long start;
  unsigned long length;
  Boolean binary;

  if (!GS_active)
    return;

  /* cp should be open, f pointing on it */

  /* Is it a dos epsf file ? */
  if (fread (&epsfheader, sizeof (epsfheader), 1, f) == 1
      && epsfheader.magic[0] == 'E' + 0x80
      && epsfheader.magic[1] == 'P' + 0x80
      && epsfheader.magic[2] == 'S' + 0x80
      && epsfheader.magic[3] == 'F' + 0x80) {
    binary = True;
    start = epsfheader.start[0];
    start += epsfheader.start[1] * 256;
    start += epsfheader.start[2] * 256 * 256;
    start += epsfheader.start[3] * 256 * 256 * 256;
    length = epsfheader.length[0];
    length += epsfheader.length[1] * 256;
    length += epsfheader.length[2] * 256 * 256;
    length += epsfheader.length[3] * 256 * 256 * 256;
    length -= start;
#if 0
    fprintf(stderr, "File %s has a DOS binary header starting at %lu (length = %lu).\n", cp, start, length);
#endif
  }
  else {
    binary = False;
    start = length = 0;
  }

  if (fseek (f, start, SEEK_SET) != 0) {
    fprintf(stderr, "drawfile_gs: problem seeking to %ld in %s, aborting\n",
	    start, cp);
    if (f) Fclose(f);              /* don't need it */
    ++n_files_left;
    return;
  }

#if 0
  if (!binary) {
    if (f) Fclose(f);              /* don't need it */
    ++n_files_left;
    /* in case there are '\\' in the file name, replace them by '/' */
    for (p = cp; p && *p; p++)
      *p = (*p == '\\' ? '/' : *p);
    if (debug & DBG_PS) Printf("Drawing file %s\n", cp);
    Sprintf(buf, "(%s) run\n", cp);
    gs_io(buf);
  }
  else
#endif
    {
    unsigned long readsofar = 0;
    fn_type theline = fn_init();
    int ch, line_count = 0;

    while ((ch = fgetc (f)) != EOF && (! binary || (readsofar < length))) {
      readsofar++;
      if (ch == '\r' || ch == '\n') {
	if (ch == '\r') {
	  ch = fgetc (f);
	  if (ch == '\n')
	    readsofar++;
	  else if (ch != EOF)
	    ungetc (ch, f);
	}
	fn_1grow(&theline, '\n');
	fn_1grow(&theline, '\0');
	gs_io(FN_STRING(theline));
	line_count++;
	if (binary && readsofar + line_count == length) break;
	fn_free(&theline);
	theline = fn_init();
      }
      else {
	fn_1grow(&theline, ch);
	/* FIXME: is it safe ??? */
	if (binary && strncmp(FN_STRING(theline), "%%EOF", 5) == 0) break;
      }
    }
    if (FN_LENGTH(theline) > 0) {
      fn_1grow(&theline, '\n');
      fn_1grow(&theline, '\0');
      gs_io(FN_STRING(theline));
    }
  
    if (f) Fclose(f);              /* don't need it */
    ++n_files_left;
  }
}

static  void
drawend_gs(cp)
     char *cp;
{
  if (!GS_active)
    return;
  if (debug & DBG_PS) Printf("end ps: %s\n", cp);
  gs_io(cp);
  gs_io("\n");
  /* If we get there, allowDrawingChars is false ! */
  if (!bMagDisp) psToDisplay = True;
  psToDisplay = True;
  /*  gs_getmatrix(); */
}

static  void
beginheader_gs()
{
  static char str[] = "Hsave /xdvi$doc exch def\n";

  if (debug & DBG_PS) Puts("Running beginheader_gs()");
  
  checkgs(True);
  
  if (GS_active) {
    if (!GS_in_header)
      oops("Internal error in beginheader_gs().\n");
    return;
  }
  
  GS_in_header = True;
  if (GS_in_doc)
    gs_io("H");
  else {
    gs_io(str);
    GS_in_doc = True;
  }

  GS_active = True;
}

static  void
endheader_gs()
{
  if (debug & DBG_PS) Puts("Running endheader_gs()");
  
  if (GS_active) {
    gs_stop();
    GS_active = False;
    GS_in_header = False;
    post_send();
    waitack();
  }
}

static  void
newdoc_gs()
{
  static char str[] = "Hxdvi$doc restore\n";

  if (debug & DBG_PS) Puts("Running newdoc_gs()");
  
  if (GS_in_doc) {
    gs_io(str);
    gs_stop();
    GS_mag = GS_shrink = -1;
    GS_in_doc = False;
  }
}

#endif /* PS_GS && WIN32 */
