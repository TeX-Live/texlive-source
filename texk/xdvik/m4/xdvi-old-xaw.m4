dnl ### Check for Xaw version
AC_DEFUN([XDVI_OLD_XAW],[
if test "${with_xdvi_x_toolkit}" = xaw; then
	AC_MSG_CHECKING(for Xaw library)
	save_CPPFLAGS="$CPPFLAGS"
	CPPFLAGS="$CPPFLAGS $X_CFLAGS"
	AC_TRY_COMPILE([
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include <X11/Xfuncs.h>
#include <X11/Intrinsic.h>
#include <X11/Xaw/Form.h>
	],[
	],
	xdvi_have_xaw=yes, xdvi_have_xaw=no
	)
	CPPFLAGS=$save_CPPFLAGS
	if test $xdvi_have_xaw = yes; then
	    AC_MSG_RESULT(yes)
	else
	    AC_MSG_RESULT(not found)
	    AC_MSG_ERROR([Sorry, you will need at least the Xaw header/library files to compile xdvik.])
	fi

	AC_MSG_CHECKING(version of Xaw library)
	save_CPPFLAGS="$CPPFLAGS"
	CPPFLAGS="$CPPFLAGS $X_CFLAGS"
	AC_TRY_COMPILE([
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xos.h>
#include <X11/Xaw/Text.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/ToggleP.h>
	    typedef void (*XawDiamondProc) (Widget);
	    
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
	    
	    /* Full class record declaration */
	    typedef struct _RadioClassRec {
	        CoreClassPart	core_class;
	        SimpleClassPart	simple_class;
	        LabelClassPart	label_class;
	        CommandClassPart	command_class;
	        ToggleClassPart	toggle_class;
	        RadioClassPart	radio_class;
	    } RadioClassRec;
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
	    
	    
	    /* The actions table from Toggle is almost perfect, but we need
	     * to override Highlight, Set, and Unset.
	     */
	    
#define SuperClass ((ToggleWidgetClass)&toggleClassRec)

#define	BOX_SIZE	16
#define PIXMAP_OFFSET 2    /* additional space between pixmap and label */
	],[
	    RadioClassRec radioClassRec = {
	      {
	        (WidgetClass) SuperClass,		/* superclass		*/
	        "Radio",				/* class_name		*/
	        sizeof(RadioRec),			/* size			*/
	        NULL,			/* class_initialize	*/
	        NULL,			/* class_part_initialize  */
	        FALSE,				/* class_inited		*/
	        NULL,				/* initialize		*/
	        NULL,				/* initialize_hook	*/
	        XtInheritRealize,			/* realize		*/
	        NULL,			/* actions		*/
	        0,		/* num_actions		*/
	        NULL,				/* resources		*/
	        0,		/* resource_count	*/
	        NULLQUARK,				/* xrm_class		*/
	        TRUE,				/* compress_motion	*/
	        TRUE,				/* compress_exposure	*/
	        TRUE,				/* compress_enterleave	*/
	        FALSE,				/* visible_interest	*/
	        NULL,			/* destroy		*/
	        NULL,			/* resize		*/
	        NULL,			/* expose		*/
	        NULL,			/* set_values		*/
	        NULL,				/* set_values_hook	*/
	        XtInheritSetValuesAlmost,		/* set_values_almost	*/
	        NULL,				/* get_values_hook	*/
	        NULL,				/* accept_focus		*/
	        XtVersion,				/* version		*/
	        NULL,				/* callback_private	*/
	        XtInheritTranslations,		/* tm_table		*/
	        NULL,			/* query_geometry	*/
	        XtInheritDisplayAccelerator,	/* display_accelerator	*/
	        NULL				/* extension		*/
	      },  /* CoreClass fields initialization */
	      {
	        XtInheritChangeSensitive,		/* change_sensitive	*/
		NULL
              },  /* SimpleClass fields initialization */
	      {
	        0					  /* field not used	*/
	      },  /* LabelClass fields initialization */
	      {
	        0					  /* field not used	*/
	      },  /* CommandClass fields initialization */
	      {
	          NULL,				/* Set Procedure.	*/
	          NULL,			/* Unset Procedure.	*/
	          NULL				/* extension.		*/
	      },  /* ToggleClass fields initialization */
	      {
	          BOX_SIZE,
	          NULL,			/* draw procedure */
	          None,				/* selected radiobutton */
	          None,				/* unselected radiobutton */
	          None,				/* selected menubutton */
	          None,				/* unselected menubutton */
	          NULL				/* extension. */
	      }  /* RadioClass fields initialization */
	    };
	    (void)radioClassRec;
	],
	xdvi_old_xaw=no, xdvi_old_xaw=yes
	)
	CPPFLAGS=$save_CPPFLAGS
if test $xdvi_old_xaw = yes; then
    AC_MSG_RESULT(old)
    AC_DEFINE([HAVE_OLD_XAW], 1,
              [Define if you have an old version of the Xaw library])
else
    AC_MSG_RESULT(current)
fi
fi
])
