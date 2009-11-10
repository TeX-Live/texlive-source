/* adapted from xlwradio.c in the XEmacs distribution.
   Changes are Copyright (C) 2002-2004 the xdvik development team.

   Note SU: This widget doesn't work with Xaw3d, and I have little
   inclination to make it work - the design would need to be utterly
   different, e.g. the radio would need to be a diamond instead of a
   circle, since 3d-shadowed circles don't look circular; colormap
   issues would need to be settled, etc. And besides, I don't like Xaw3d ;)
   
   So instead, the caller should just do:

   myWidget = XtVaCreateManagedWidget("foo",
   #ifdef XAW
   radioWidgetClass,
   #else
   toggleWidgetClass,
   #endif
   ... arglist ...);

   Feel free to submit patches if you want this implemented.
   
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

/* Synched up with: Radio.c 1.1 */

/*
 * Radio.c - Radio button widget
 *
 * Author: Edward A. Falk
 *         falk@falconer.vip.best.com
 *
 * Date:   June 30, 1997
 *
 *
 * Overview:  This widget is identical to the Toggle widget in behavior,
 * but completely different in appearance.  This widget looks like a small
 * diamond-shaped button with a label to the right.
 *
 * To make this work, we subclass the Toggle widget to inherit its behavior
 * and to inherit the label-drawing function from which Toggle is
 * subclassed.  We then completely replace the Expose, Set, Unset
 * and Highlight member functions.
 *
 * The Set and Unset actions are slightly unorthodox.  In Toggle's
 * ClassInit function, Toggle searches the Command actions list and
 * "steals" the Set and Unset functions, caching pointers to them in its
 * class record.  It then calls these functions from its own ToggleSet
 * and Toggle actions.
 *
 * We, in turn, override the Set() and Unset() actions in our own ClassRec.
 */

#include "xdvi-config.h"
#include "xdvi.h"
#include "xdvi-debug.h"
#include "util.h"
#include "xaw_bitmaps.h" /* bitmaps for radio/checkbuttons */

#include "xlwradioP.h"

#ifndef MOTIF /* entire file */

#include <stdio.h>

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xos.h>
#include <X11/Xaw/Text.h>
#include <X11/Xaw/AsciiText.h>
/*  #include "../src/xmu.h" */

#define	BOX_SIZE	16
#define PIXMAP_OFFSET 2    /* additional space between pixmap and label */

#ifndef UNUSED
# define UNUSED(x) ((void)(x))
#endif

#ifndef	MAX
# define MAX(i, j)       ( (i) > (j) ? (i) : (j) )
#endif


#define	rclass(w)	((RadioWidgetClass)((w)->core.widget_class))


#ifdef _ThreeDP_h
#define	swid(rw)	((rw)->threeD.shadow_width)
#else
#define	swid(rw)	((rw)->core.border_width)
#endif

#define	bsize(rw)	(rclass(rw)->radio_class.dsize)
#define	bs(rw)		(bsize(rw) + PIXMAP_OFFSET + 2*swid(rw))

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* The translations table from Toggle do not need to be
 * overridden by Radio
 */


/* Member functions */

static void RadioInit (Widget, Widget, ArgList, Cardinal *);
static void RadioExpose (Widget, XEvent *, Region);
static void RadioResize (Widget);
static void RadioDestroy (Widget);
static void RadioClassInit (void);
static void RadioClassPartInit (WidgetClass);
static Boolean RadioSetValues (Widget, Widget, Widget, ArgList, Cardinal *);
static void DrawDiamond (Widget);
static XtGeometryResult RadioQueryGeometry (Widget, XtWidgetGeometry *,
					    XtWidgetGeometry *);

/* Action procs */

static void RadioHighlight   (Widget, XEvent *, String *, Cardinal *);
static void RadioUnhighlight (Widget, XEvent *, String *, Cardinal *);

/* internal privates */

static void RadioSize (RadioWidget, Dimension *, Dimension *);

/* The actions table from Toggle is almost perfect, but we need
 * to override Highlight, Set, and Unset.
 */

static XtActionsRec actionsList[] =
    {
	{"highlight",		RadioHighlight},
	{"unhighlight",	RadioUnhighlight},
    };

#define offset(field) XtOffset(RadioWidget, radio.field)

static XtResource resources[] = {
    {XtNisRadio,  XtCIsRadio, XtRBoolean, sizeof(Boolean),
     offset(isRadio), XtRImmediate, (XtPointer)(ptrdiff_t)True },
};
#undef offset

#define SuperClass ((ToggleWidgetClass)&toggleClassRec)

RadioClassRec radioClassRec = {
    {
	(WidgetClass) SuperClass,		/* superclass		*/
	"Radio",				/* class_name		*/
	sizeof(RadioRec),			/* size			*/
	RadioClassInit,			/* class_initialize	*/
	RadioClassPartInit,			/* class_part_initialize  */
	FALSE,				/* class_inited		*/
	RadioInit,				/* initialize		*/
	NULL,				/* initialize_hook	*/
	XtInheritRealize,			/* realize		*/
	actionsList,			/* actions		*/
	XtNumber(actionsList),		/* num_actions		*/
	resources,				/* resources		*/
	XtNumber(resources),		/* resource_count	*/
	NULLQUARK,				/* xrm_class		*/
	TRUE,				/* compress_motion	*/
	TRUE,				/* compress_exposure	*/
	TRUE,				/* compress_enterleave	*/
	FALSE,				/* visible_interest	*/
	RadioDestroy,			/* destroy		*/
	RadioResize,			/* resize		*/
	RadioExpose,			/* expose		*/
	RadioSetValues,			/* set_values		*/
	NULL,				/* set_values_hook	*/
	XtInheritSetValuesAlmost,		/* set_values_almost	*/
	NULL,				/* get_values_hook	*/
	NULL,				/* accept_focus		*/
	XtVersion,				/* version		*/
	NULL,				/* callback_private	*/
	XtInheritTranslations,		/* tm_table		*/
	RadioQueryGeometry,			/* query_geometry	*/
	XtInheritDisplayAccelerator,	/* display_accelerator	*/
	NULL				/* extension		*/
    },  /* CoreClass fields initialization */
    {
	XtInheritChangeSensitive		/* change_sensitive	*/
#ifndef HAVE_OLD_XAW
	, NULL  
#endif
    },  /* SimpleClass fields initialization */
#ifdef _ThreeDP_h
    {
	XtInheritXaw3dShadowDraw		/* field not used	*/
    },  /* ThreeDClass fields initialization */
#endif
    {
	0					  /* field not used	*/
    },  /* LabelClass fields initialization */
    {
	0					  /* field not used	*/
    },  /* CommandClass fields initialization */
    {
	RadioSet,				/* Set Procedure.	*/
	RadioUnset,			/* Unset Procedure.	*/
	NULL				/* extension.		*/
    },  /* ToggleClass fields initialization */
    {
	BOX_SIZE,
	DrawDiamond,			/* draw procedure */
	None,				/* selected radiobutton */
	None,				/* unselected radiobutton */
	None,				/* selected menubutton */
	None,				/* unselected menubutton */
	NULL				/* extension. */
    }  /* RadioClass fields initialization */
};

/* for public consumption */
WidgetClass radioWidgetClass = (WidgetClass) &radioClassRec;






/****************************************************************
 *
 * Class Methods
 *
 ****************************************************************/

static void
RadioClassInit (void)
{
    XawInitializeWidgetSet();
}

static	void
RadioClassPartInit (WidgetClass class)
{
    RadioWidgetClass c     = (RadioWidgetClass) class;
    RadioWidgetClass super = (RadioWidgetClass)c->core_class.superclass;

    if( c->radio_class.drawDiamond == NULL  ||
	c->radio_class.drawDiamond == XtInheritDrawDiamond )
    {
	c->radio_class.drawDiamond = super->radio_class.drawDiamond;
    }

}




static void
RadioInit (Widget   request,
	   Widget   new,
	   ArgList  args,
	   Cardinal *num_args)
{
    RadioWidget rw = (RadioWidget) new;
    RadioWidget rw_req = (RadioWidget) request;
    Dimension	w,h;

    UNUSED(args);
    UNUSED(num_args);

    /* FIXME: should pixmap initialization be here?? */

    /* Select initial size for the widget */
    if( rw_req->core.width == 0  ||  rw_req->core.height == 0 ) {
	RadioSize(rw, &w,&h);
	if( rw_req->core.width == 0 )
	    rw->core.width = w;
	if( rw_req->core.height == 0 )
	    rw->core.height = h;
	rw->core.widget_class->core_class.resize(new);
    }
    /*
      FIXME: access to XtWindow(rw) fails in the init method, so
      I moved the bitmap creation here -- is there a better way??
    */
    /* create pixmaps */
    rclass(rw)->radio_class.sel_radio
	= XCreateBitmapFromData(XtDisplay(rw), RootWindowOfScreen(XtScreen(rw)),
				(char *)button_radio_on_bits, BUTTON_BITMAP_W, BUTTON_BITMAP_H);
    rclass(rw)->radio_class.unsel_radio
	= XCreateBitmapFromData(XtDisplay(rw), RootWindowOfScreen(XtScreen(rw)),
				(char *)button_radio_off_bits, BUTTON_BITMAP_W, BUTTON_BITMAP_H);
    rclass(rw)->radio_class.sel_menu
	= XCreateBitmapFromData(XtDisplay(rw), RootWindowOfScreen(XtScreen(rw)),
				(char *)button_check_on_bits, BUTTON_BITMAP_W, BUTTON_BITMAP_H);
    rclass(rw)->radio_class.unsel_menu
	= XCreateBitmapFromData(XtDisplay(rw), RootWindowOfScreen(XtScreen(rw)),
				(char *)button_check_off_bits, BUTTON_BITMAP_W, BUTTON_BITMAP_H);
}

/*	Function Name: RadioDestroy
 *	Description: Destroy Callback for radio widget.
 *	Arguments: w - the radio widget that is being destroyed.
 *	Returns: none.
 */

static void
RadioDestroy (Widget w)
{
    RadioWidget rw = (RadioWidget)w;
    /* de-allocate bitmaps */
    XFreePixmap(XtDisplay(w), rclass(rw)->radio_class.sel_radio);
    XFreePixmap(XtDisplay(w), rclass(rw)->radio_class.unsel_radio);
    XFreePixmap(XtDisplay(w), rclass(rw)->radio_class.sel_menu);
    XFreePixmap(XtDisplay(w), rclass(rw)->radio_class.unsel_menu);
}


/* React to size change from manager.  Label widget will compute some internal
 * stuff, but we need to override.  This code requires knowledge of the
 * internals of the Label widget.
 */

static	void
RadioResize (Widget w)
{
    RadioWidget rw = (RadioWidget)w;

    /* call parent resize proc */
    SuperClass->core_class.resize(w);

    /* override label offset */

    switch( rw->label.justify ) {
    case XtJustifyLeft:
	rw->label.label_x += (bs(rw) + rw->label.internal_width);
	break;
    case XtJustifyRight:
	break;
    case XtJustifyCenter:
    default:
	rw->label.label_x += (bs(rw) + rw->label.internal_width)/2;
	break;
    }
}


/*
 * Repaint the widget window.
 */

static	void
RadioExpose (Widget w,
	     XEvent *event,
	     Region region)
{
    RadioWidget	rw = (RadioWidget) w;
    Display		*dpy = XtDisplay(w);
    Window		win = XtWindow(w);
    GC		gc;
    Pixmap		left_bitmap;
    extern WidgetClass labelWidgetClass;

    /* Note: the Label widget examines the region to decide if anything
     * needs to be drawn.  I'm not sure that this is worth the effort,
     * but it bears thinking on.
     */

    /* Command widget may sometimes override the label GC in order
     * to draw inverse video.  We don't use inverse video, so we need
     * to restore the label's normal GC.
     */
    rw->label.normal_GC = rw->command.normal_GC;


    /* Let label widget draw the label.  If there was an lbm_x
     * field, we could let Label draw the bitmap too.  But there
     * isn't, so we need to temporarily remove the bitmap and
     * draw it ourself later.
     */
    left_bitmap = rw->label.left_bitmap;
    rw->label.left_bitmap = None;
    labelWidgetClass->core_class.expose(w,event,region);
    rw->label.left_bitmap = left_bitmap;

    /* now manually draw the left bitmap.  TODO: 3-d look, xaw-xpm */
    gc = XtIsSensitive(w) ? rw->label.normal_GC : rw->label.gray_GC;
    if( left_bitmap != None && rw->label.lbm_width > 0 )
    {
	/* TODO: handle pixmaps */
	XCopyPlane(dpy, left_bitmap, win, gc,
		   0,0, rw->label.lbm_width, rw->label.lbm_height,
		   (int) rw->label.internal_width*2 + bs(rw),
		   (int) rw->label.internal_height + rw->label.lbm_y,
		   1UL);
    }
    DrawDiamond(w);
    /* Finally, the button itself */
    ((RadioWidgetClass)(w->core.widget_class))->radio_class.drawDiamond(w);
}




/************************************************************
 *
 * Set specified arguments into widget
 *
 ***********************************************************/


/* ARGSUSED */
static Boolean
RadioSetValues (Widget   current,
		Widget   request,
		Widget   new,
		ArgList  args,
		Cardinal *num_args)
{
    RadioWidget oldrw = (RadioWidget) current;
    RadioWidget newrw = (RadioWidget) new;

    UNUSED(request);
    UNUSED(args);
    UNUSED(num_args);
    
    /* Need to find out if the size of the widget changed.  Set new size
     * if it did and resize is permitted.  One way to determine of the
     * widget changed size would be to scan the args list.  Another way
     * is to compare the old and new widgets and see if any of several
     * size-related fields have been changed.  The Label widget chose the
     * former method, but I choose the latter.
     */

    if( newrw->label.resize &&
	( newrw->core.width != oldrw->core.width ||
	  newrw->core.height != oldrw->core.height ||
	  newrw->core.border_width != oldrw->core.border_width ) )
    {
	RadioSize(newrw, &newrw->core.width, &newrw->core.height);
    }

    /* The label set values routine can resize the widget. We need to
     * recalculate if this is true.
     */
    if (newrw->label.label_x != oldrw->label.label_x)
    {
	RadioResize (new);
    }
    return FALSE;
}

static XtGeometryResult
RadioQueryGeometry (Widget w,
		    XtWidgetGeometry *intended,
		    XtWidgetGeometry *preferred)
{
    RadioWidget rw = (RadioWidget) w;

    preferred->request_mode = CWWidth | CWHeight;
    RadioSize(rw, &preferred->width, &preferred->height);

    if (  ((intended->request_mode & (CWWidth | CWHeight))
	   == (CWWidth | CWHeight)) &&
	  intended->width == preferred->width &&
	  intended->height == preferred->height)
	return XtGeometryYes;
    else if (preferred->width == w->core.width &&
	     preferred->height == w->core.height)
	return XtGeometryNo;
    else
	return XtGeometryAlmost;
}





/************************************************************
 *
 *  Action Procedures
 *
 ************************************************************/

/*
 * Draw the highlight border around the widget.  The Command widget
 * did this by drawing through a mask.  We do it by just drawing the
 * border.
 */

static void
DrawHighlight (Widget w,
	       GC gc)
{
    RadioWidget	rw = (RadioWidget)w;
    XRectangle	rects[4];
    Dimension	ht = rw->command.highlight_thickness;

    if( ht <= 0 ||
	ht > rw->core.width/2  ||
	ht > rw->core.height/2 )
	return;

    if( ! XtIsRealized(w) )
	return;

    rects[0].x = 0; rects[0].y = 0;
    rects[0].width = rw->core.width; rects[0].height = ht;
    rects[1].x = 0; rects[1].y = rw->core.height - ht;
    rects[1].width = rw->core.width; rects[1].height = ht;
    rects[2].x = 0; rects[2].y = ht;
    rects[2].width = ht; rects[2].height = rw->core.height - ht*2;
    rects[3].x = rw->core.width - ht; rects[3].y = ht;
    rects[3].width = ht; rects[3].height = rw->core.height - ht*2;
    XFillRectangles( XtDisplay(w), XtWindow(w), gc, rects, 4);
}

static	void
RadioHighlight (Widget   w,
		XEvent   *event,
		String   *params,
		Cardinal *num_params)
{
    RadioWidget	rw = (RadioWidget)w;
    UNUSED(event);
    UNUSED(params);
    UNUSED(num_params);
    
    DrawHighlight(w, rw->command.normal_GC);
}


static	void
RadioUnhighlight (Widget   w,
		  XEvent   *event,
		  String   *params,
		  Cardinal *num_params)
{
    RadioWidget	rw = (RadioWidget)w;
    UNUSED(event);
    UNUSED(params);
    UNUSED(num_params);
    
    DrawHighlight(w, rw->command.inverse_GC);
}


void
RadioSet (Widget   w,
	  XEvent   *event,
	  String   *params,
	  Cardinal *num_params)
{
    RadioWidget	rw = (RadioWidget)w;
    RadioWidgetClass class = (RadioWidgetClass) w->core.widget_class;

    UNUSED(event);
    UNUSED(params);
    UNUSED(num_params);
    
    if( rw->command.set )
	return;

    rw->command.set = TRUE;
    if( XtIsRealized(w) )
	class->radio_class.drawDiamond(w);
}


void
RadioUnset (Widget   w,
	    XEvent   *event,
	    String   *params,
	    Cardinal *num_params)
{
    RadioWidget	rw = (RadioWidget)w;
    RadioWidgetClass class = (RadioWidgetClass) w->core.widget_class;

    UNUSED(event);
    UNUSED(params);
    UNUSED(num_params);
    
    if( ! rw->command.set )
	return;

    rw->command.set = FALSE;
    if( XtIsRealized(w) )
	class->radio_class.drawDiamond(w);
}




/************************************************************
 *
 *  Internal Procedures
 *
 ************************************************************/


/* Size of widget.  Width is size of box plus width of border around
 * box plus width of label plus three margins plus the size of the left
 * bitmap, if any.  Height is max(box,bitmap,label) plus two margins.
 */

static	void
RadioSize (RadioWidget rw,
	   Dimension   *w,
	   Dimension   *h)
{
    *w = rw->label.label_width + bs(rw) + LEFT_OFFSET(rw) + 
	3 * rw->label.internal_width;
    *h = MAX( rw->label.label_height, bs(rw) ) +
	2 * rw->label.internal_width;
}


static void
DrawDiamond(Widget w)
{
    RadioWidget rw = (RadioWidget) w;
    Display *dpy = XtDisplay(w);
    Window win = XtWindow(w);
    
    Position x = rw->label.internal_width;
    Position y = rw->core.height/2;
    GC gc = XtIsSensitive(w) ? rw->command.normal_GC : rw->label.gray_GC;
    Pixmap selected, unselected;

    if (rw->radio.isRadio) { /* it's a radio button */
	selected = rclass(rw)->radio_class.sel_radio;
	unselected = rclass(rw)->radio_class.unsel_radio;
    }
    else {
	selected = rclass(rw)->radio_class.sel_menu;
	unselected = rclass(rw)->radio_class.unsel_menu;
    }
    
    ASSERT(selected != None, "");
    ASSERT(unselected != None, "");
    
    if(rw->command.set) {
	XCopyPlane(dpy, selected, win, gc,
		   0, 0, BUTTON_BITMAP_H, BUTTON_BITMAP_W,
		   x, y - BUTTON_BITMAP_H / 2, 1L);
    }
    else {
	XCopyPlane(dpy, unselected, win, gc,
		   0, 0, BUTTON_BITMAP_H, BUTTON_BITMAP_W,
		   x, y - BUTTON_BITMAP_H / 2, 1L);
    }
}


#else
/* silence `empty compilation unit' warnings */
static void bar(void); static void foo(void) { bar(); } static void bar(void) { foo(); }
#endif /* MOTIF */
