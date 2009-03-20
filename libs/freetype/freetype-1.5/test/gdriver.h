/*******************************************************************
 *
 *  gdriver.h : Graphics utility driver generic interface       1.1
 *
 *  Generic interface for all drivers of the graphics utility used
 *  by the FreeType test programs.
 *
 *  Copyright 1996-2000 by
 *  David Turner, Robert Wilhelm, and Werner Lemberg.
 *
 *  This file is part of the FreeType project, and may only be used
 *  modified and distributed under the terms of the FreeType project
 *  license, LICENSE.TXT.  By continuing to use, modify or distribute
 *  this file you indicate that you have read the license and
 *  understand and accept it fully.
 *
 ******************************************************************/

#ifndef GDRIVER_H
#define GDRIVER_H

  /* Note that we now support an event based model, even with   */
  /* full-screen modes.  It is the responsability of the driver */
  /* to map its events to the TEvent structure when called      */
  /* through Get_Event.                                         */

  /* The event classes are defined in the file 'gevents.h' included */
  /* by the test programs, not by the graphics utility.             */

  typedef struct  _TEvent
  {
    int  what; /* event class     */
    int  info; /* event parameter */
  } TEvent;


  /* Get last event.  In full-screen modes, a keystroke must be */
  /* translated to an event class with a parameter.             */
  void  Get_Event( TEvent*  event );

  /* A call to this function must set the graphics mode, the Vio  */
  /* variable, as well as the values vio_ScanLineWidth, vio_Width */
  /* and vio_Height.                                              */
  int  Driver_Set_Graphics( int  mode );

  /* Restore previous mode or release display buffer/window */
  int  Driver_Restore_Mode( void );

  /* display bitmap on screen */
  int  Driver_Display_Bitmap( char*  buffer, int  line, int  col );

#endif /* GDRIVER_H */


/* End */
