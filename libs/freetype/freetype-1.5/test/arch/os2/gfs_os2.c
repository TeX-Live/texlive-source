/*******************************************************************
 *
 *  gfs_os2.c  graphics utility fullscreen OS/2 driver.        1.0
 *
 *  This is the driver for fullscreen OS/2 display, used by the
 *  graphics utility of the FreeType test suite.
 *
 *  Copyright 1996-2000 by
 *  David Turner, Robert Wilhelm, and Werner Lemberg.
 *
 *  This file is part of the FreeType project, and may only be used
 *  modified and distributed under the terms of the FreeType project
 *  license, LICENSE.TXT. By continuing to use, modify or distribute
 *  this file you indicate that you have read the license and
 *  understand and accept it fully.
 *
 ******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INCL_SUB
#include <os2.h>

#ifdef __EMX__
#include <os2thunk.h>
#endif

#if defined(__EMX__)||defined(__IBMC__)
#include <conio.h>       /* for getch                                   */
  extern _read_kbd();    /* to avoid an ANSI warning during compilation */
#endif

#include "gdriver.h"
#include "gevents.h"
#include "gmain.h"


  typedef struct  _Translator
  {
    char    key;
    GEvent  event_class;
    int     event_info;
  } Translator;

#define NUM_Translators  20

  static const Translator  trans[NUM_Translators] =
  {
    { (char)27, event_Quit,              0 },
    { 'q',      event_Quit,              0 },

    { 'x',      event_Rotate_Glyph,     -1 },
    { 'c',      event_Rotate_Glyph,      1 },
    { 'v',      event_Rotate_Glyph,    -16 },
    { 'b',      event_Rotate_Glyph,     16 },

    { '{',      event_Change_Glyph, -10000 },
    { '}',      event_Change_Glyph,  10000 },
    { '(',      event_Change_Glyph,  -1000 },
    { ')',      event_Change_Glyph,   1000 },
    { '9',      event_Change_Glyph,   -100 },
    { '0',      event_Change_Glyph,    100 },
    { 'i',      event_Change_Glyph,    -10 },
    { 'o',      event_Change_Glyph,     10 },
    { 'k',      event_Change_Glyph,     -1 },
    { 'l',      event_Change_Glyph,      1 },

    { '+',      event_Scale_Glyph,      10 },
    { '-',      event_Scale_Glyph,     -10 },
    { 'u',      event_Scale_Glyph,       1 },
    { 'j',      event_Scale_Glyph,      -1 }
  };


  static VIOMODEINFO  OrgMode;
  static long         VioBufOfs;


  /* BIOS video modes */

  static VIOMODEINFO  VioMode_640x480x16 =
  {
    sizeof ( VIOMODEINFO ),
    VGMT_OTHER + VGMT_GRAPHICS,
    COLORS_16,
    80,
    35,
    640,
    480
  };

  static VIOMODEINFO  VioMode_320x200x256 =
  {
    sizeof ( VIOMODEINFO ),
    VGMT_OTHER + VGMT_GRAPHICS,
    8,
    40,
    25,
    320,
    200
  };

  static VIOPHYSBUF  VioBuf =
  {
    (void*)0xA0000L,
    64*1024
  };


  /* Restores screen to its original state */

  int  Driver_Restore_Mode()
  {
    VioSetMode( &OrgMode, 0 );
    return 1;
  }


  /* Sets graphics mode */

  int  Driver_Set_Graphics( int  mode )
  {
    int  rc;


    OrgMode.cb = sizeof ( VIOMODEINFO );
    VioGetMode( &OrgMode, 0 );

    switch ( mode )
    {
    case Graphics_Mode_Mono:
      rc = VioSetMode( &VioMode_640x480x16, 0 );
      vio_ScanLineWidth = 80;
      vio_Width         = 640;
      vio_Height        = 480;
      break;

    case Graphics_Mode_Gray:
      rc = VioSetMode( &VioMode_320x200x256, 0 );
      vio_ScanLineWidth = 320;
      vio_Width         = 320;
      vio_Height        = 200;

      /* default gray_palette takes the gray levels of the standard VGA */
      /* 256 colors mode                                                */

      gray_palette[0] = 0;
      gray_palette[1] = 23;
      gray_palette[2] = 27;
      gray_palette[3] = 29;
      gray_palette[4] = 31;
      break;

    default:
      rc = -1;
      break;
    }

    if ( rc ) return 0;     /* failure */

    if ( VioGetPhysBuf( &VioBuf, 0 ) ) return 0;  /* Could not access VRAM */

    VioBufOfs = (long)MAKEP( VioBuf.asel[0], 0 );

    memset( (void*)VioBufOfs, 0, 64 * 1024 );

    Vio = (char*)VioBufOfs;

    return 1;               /* success */
  }


  int  Driver_Display_Bitmap( char*  buffer, int  line, int  col )
  {
    int    y;
    char*  target;

    target = Vio + ( line - 1 ) * vio_ScanLineWidth;

    for ( y = 0; y < line; y++ )
    {
      memcpy( target, buffer, col );
      target -= vio_ScanLineWidth;
      buffer += col;
    }

    return 1;               /* success */
  }


  void  Get_Event( TEvent*  event )
  {
    int   i;
    char  c;


    c = getch();

    for ( i = 0; i < NUM_Translators; i++ )
    {
      if ( c == trans[i].key )
      {
        event->what = trans[i].event_class;
        event->info = trans[i].event_info;
        return;
      }
    }

    /* unrecognized keystroke */

    event->what = event_Keyboard;
    event->info = (int)c;
    return;
  }


/* End */
