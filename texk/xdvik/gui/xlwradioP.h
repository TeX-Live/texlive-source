/* adapted from xlwradio.c in the XEmacs distribution.
   Changes are Copyright (C) 2002-2004 the xdvik development team
   Original copyright follows:
*/

/* Radio Widget for XEmacs.
   Copyright (C) 1999 Edward A. Falk

This file is part of XEmacs.

XEmacs is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

XEmacs is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with XEmacs; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

/*
 * RadioP.h - Private definitions for Radio widget
 * 
 * Author: Edward A. Falk
 *         falk@falconer.vip.best.com
 *  
 * Date:   June 30, 1997
 *
 */

#ifndef _XawRadioP_h
#define _XawRadioP_h

#include "xlwradio.h"

#ifndef MOTIF

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xos.h>
#include <X11/Xaw/Text.h>
#include <X11/Xaw/AsciiText.h>

#include <X11/Xaw/ToggleP.h>
#include <X11/Xaw/XawInit.h>

/***********************************************************************
 *
 * Radio Widget Private Data
 *
 ***********************************************************************/

typedef void (*XawDiamondProc) (Widget);

void RadioSet (Widget   w,
	       XEvent   *event,
	       String   *params,      /* unused */
	       Cardinal *num_params); /* unused */

void RadioUnset (Widget   w,
		 XEvent   *event,
		 String   *params,      /* unused */
		 Cardinal *num_params); /* unused */

/************************************
 *
 *  Class structure
 *
 ***********************************/

/* New fields for the Radio widget class record */
typedef struct _RadioClass  {
    Dimension	dsize;		/* diamond size */
    XawDiamondProc drawDiamond;
    /* pixmaps for the button */
    Pixmap sel_radio;		/* selected state */
    Pixmap unsel_radio;		/* unselected state */
    Pixmap sel_menu;		/* selected state */
    Pixmap unsel_menu;		/* unselected state */
    /* TODO: 3-d and xaw-xpm features? */
    XtPointer	extension;
} RadioClassPart;

#define	XtInheritDrawDiamond	((XawDiamondProc)_XtInherit)

/* Full class record declaration */
typedef struct _RadioClassRec {
    CoreClassPart	core_class;
    SimpleClassPart	simple_class;
#ifdef _ThreeDP_h
    ThreeDClassPart	threeD_class;
#endif
    LabelClassPart	label_class;
    CommandClassPart	command_class;
    ToggleClassPart	toggle_class;
    RadioClassPart	radio_class;
} RadioClassRec;

extern RadioClassRec radioClassRec;

/***************************************
 *
 *  Instance (widget) structure 
 *
 **************************************/

/* New fields for the Radio widget record */
typedef struct {
    /* resources */
    Boolean isRadio;		/* radio if True, checkbox else */
    /* TODO: 3-d and xaw-xpm features? */

    /* private data */
    XtPointer	extension;
} RadioPart;

   /* Full widget declaration */
typedef struct _RadioRec {
    CorePart	core;
    SimplePart	simple;
#ifdef _ThreeDP_h
    ThreeDPart	threeD;
#endif
    LabelPart	label;
    CommandPart	command;
    TogglePart	toggle;
    RadioPart	radio;
} RadioRec;

#endif /* MOTIF */

#endif /* _XawRadioP_h */
