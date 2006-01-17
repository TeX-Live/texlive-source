/* 
   wintransf.c : implementation file
   Time-stamp: "03/05/21 23:50:21 popineau"
   
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

#include "wingui.h"
#include "xdvi-config.h"

#define DEBUG_TRANSF 0

#define XFRM_INITIAL 100

XFORM xfrmTransf = {1.0, 0.0, 0.0, 1.0, 0.0, 0.0};
XFORM xfrmUnit = {1.0, 0.0, 0.0, 1.0, 0.0, 0.0};

typedef struct _xfrm_stack {
  long dviptr;
  XFORM xfrm;
} xfrm_stack;

xfrm_stack *xfrmStack;
int top_xfrm;
int xfrmStack_len;

BOOL bTransfInUse = FALSE;
XFORM xfrm_unit = { 1.0, 0.0, 0.0, 1.0, 0.0,0.0 };
XFORM current_transf;

void init_xfrm_stack()
{
  top_xfrm = 0;
  xfrmStack_len = XFRM_INITIAL;
  xfrmStack = xmalloc(sizeof(xfrm_stack)*XFRM_INITIAL);
  current_transf = xfrm_unit;
}

void reset_xfrm_stack()
{
  XFORM xfrm;
  top_xfrm = 0;
#if DEBUG_TRANSF
  fprintf(stderr, "Resetting transformation stack.\n");
#endif
  if (ModifyWorldTransform(foreGC, &xfrm, MWT_IDENTITY) == 0)
    Win32Error("reset_xfrm_stack/ModifyWorldTransform");
  bTransfInUse = FALSE;
  current_transf = xfrm_unit;
}

void xfrm_dump_stack()
{
  int i;
  for (i = 0; i < top_xfrm; i++) {
    fprintf(stderr, "xfrmStack[%d] = [%f %f %f %f %f %f] pos = %ld\n", i,
	    xfrmStack[i].xfrm.eM11, xfrmStack[i].xfrm.eM12,
	    xfrmStack[i].xfrm.eM21, xfrmStack[i].xfrm.eM22, 
	    xfrmStack[i].xfrm.eDx, xfrmStack[i].xfrm.eDy,
	    xfrmStack[i].dviptr);
  }
}

void xfrm_record(XFORM *x)
{
#if 0
  __asm int 3;
#endif
  int i;

  if (!resource.use_xform || !x) return;

  for (i = 0; i < top_xfrm; i++) {
    if (xfrmStack[i].dviptr == currinf.pos) {
      /* Already in there, return */
      /* FIXME : should we do any check for the *same* transf ? */
      return;
    }
    else if(xfrmStack[i].dviptr > currinf.pos) {
      /* insert it at this position and move up everything else */
      break;
    }
  }
  if (top_xfrm+1 > xfrmStack_len) {
      xfrmStack_len *= 2;
      xfrmStack = xrealloc(xfrmStack, sizeof(xfrm_stack)*xfrmStack_len);
  }
  if (i == top_xfrm) {
    xfrmStack[top_xfrm].dviptr = currinf.pos;
    xfrmStack[top_xfrm].xfrm = *x;
    top_xfrm++;
  }
  else {
    memmove(xfrmStack+(i+1), xfrmStack+i, (top_xfrm - i)*sizeof(xfrm_stack));
    xfrmStack[i].dviptr = currinf.pos;
    xfrmStack[i].xfrm = *x;
    top_xfrm++;
  }
#if DEBUG_TRANSF
  fprintf(stderr, "inserting (pos = %ld) xfrmStack[%d] = [%f %f %f %f %f %f]\n",
	  currinf.pos, i,
	  xfrmStack[i].xfrm.eM11, xfrmStack[i].xfrm.eM12,
	  xfrmStack[i].xfrm.eM21, xfrmStack[i].xfrm.eM22, 
	  xfrmStack[i].xfrm.eDx, xfrmStack[i].xfrm.eDy);
  xfrm_dump_stack();
#endif
}

void xfrm_apply()
{
  int i = -1;
  long pos = currinf.pos;
  POINT mane_points[3];
  XFORM xfrm = xfrm_unit;
#if 0
  __asm int 3;
  fprintf(stderr, "Entering xfrm_apply\n");
#endif
  if (!resource.use_xform) return;

#if DEBUG_TRANSF
  fprintf(stderr, "xfrm_apply: pos = %ld\n", pos);
  xfrm_dump_stack();
#endif
  for (i = 0; i < top_xfrm; i++) {
    if (pos == xfrmStack[i].dviptr) {
      xfrm = xfrmStack[i].xfrm;
      break;
    }
  }
  if (i < top_xfrm) {
    if (foreGC == magMemDC || foreGC == magDC) {
      extern int magWidth, magHeight;
#if 1
      /* FIXME : I'm afraid it can be solved only by applying
	 transformations in XPutImage !
      */

#if 0
 xfrm.eDx += ((xfrm.eM11 -1)*(currwin.base_x + magWidth) + xfrm.eM21*(currwin.base_y + magHeight))/mane.shrinkfactor;
      xfrm.eDy += (xfrm.eM12*(currwin.base_x + magWidth) + (xfrm.eM22-1)*(currwin.base_y + magHeight))/mane.shrinkfactor;
#endif
      xfrm.eDx += (xfrm.eM11 -1)*(currwin.base_x) + xfrm.eM21*(currwin.base_y);
      xfrm.eDy += xfrm.eM12*(currwin.base_x) + (xfrm.eM22-1)*(currwin.base_y);

      bTransfInUse = (fabs(xfrm.eM11 - 1.0) 
		      + fabs(xfrm.eM12)
		      + fabs(xfrm.eM21)
		      + fabs(xfrm.eM22 - 1.0)
		      + fabs(xfrm.eDx)
		      + fabs(xfrm.eDy)) > .0001 ;
#else
      bTransfInUse =  FALSE;
#endif
    }
    else {
      bTransfInUse = (fabs(xfrm.eM11 - 1.0) 
		      + fabs(xfrm.eM12)
		      + fabs(xfrm.eM21)
		      + fabs(xfrm.eM22 - 1.0)
		      + fabs(xfrm.eDx)
		      + fabs(xfrm.eDy)) > .0001 ;
    }
    if (!bTransfInUse) {
      if (ModifyWorldTransform(foreGC, &xfrm, MWT_IDENTITY) == 0)
	Win32Error("xfrm_apply/ModifyWorldTransform");
      current_transf = xfrm_unit;
    }
    else {
#if 0
      fprintf(stderr, "Applying transf [%f %f %f %f %f %f] to foreGC\n",
	      xfrm.eM11, xfrm.eM12,
	      xfrm.eM21, xfrm.eM22, 
	      xfrm.eDx, xfrm.eDy);
      fprintf(stderr, "currwin orig: (%d %d)\n", currwin.base_x, currwin.base_y);
#endif
#if 0
      if (SetWorldTransform(foreGC, &(xfrm)) == 0)
	Win32Error("xfrm_apply/SetWorldTransform");
#else
      current_transf = xfrm;
#endif
    }
  }
}
