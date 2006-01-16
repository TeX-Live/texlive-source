/*******************************************************************
 *
 *  gevents.h test programs events definition                 1.1
 *
 *  This file defines the events used by the FreeType test programs
 *  It is _not_ included by 'gmain.c'.  This file is also used by the
 *  drivers to translate their own events in GEvents.
 *
 *  Not a very good design, but we're not rewriting X...
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

#ifndef GEVENTS_H
#define GEVENTS_H

  typedef enum  _GEvent
  {
    event_None,
    event_Quit,          /* Quit program */

    event_Keyboard,      /* unknown keystroke */

    event_Change_Glyph,
    event_Rotate_Glyph,
    event_Scale_Glyph,

    event_Change_ScanType,
    event_Change_Instructions
  } GEvent;

#endif /* GEVENTS_H */


/* End */
