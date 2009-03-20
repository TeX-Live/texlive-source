/*******************************************************************
 *
 *  gfsamiga.c  graphics utility fullscreen Amiga driver.       1.0
 *
 *  This is the driver for fullscreen Amiga display, used by the
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

/* standard includes */

#include <stdio.h>
#include <stdlib.h>

/* AmigaOS includes */

#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>

#ifdef __GNUC__

#include <inline/exec.h>
#include <inline/intuition.h>
#include <inline/graphics.h>
#include <inline/dos.h>

#else

#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>
#include <clib/graphics_protos.h>
#include <clib/dos_protos.h>

#endif

/* FreeType includes */

#include "gdriver.h"
#include "gevents.h"
#include "gmain.h"

/* some screen definitions */

#define MONO_SCREEN_WIDTH   640
#define MONO_SCREEN_HEIGHT  512
#define MONO_SCREEN_DEPTH     1

#define GRAY_SCREEN_WIDTH   320
#define GRAY_SCREEN_HEIGHT  256
#define GRAY_SCREEN_DEPTH     3

#define DISPLAY_MEM         ( 1024 * 64 )


  /* external variables */

  extern struct Library*  SysBase;
  extern struct Library*  DOSBase;

  extern int    vio_ScanLineWidth;
  extern char*  Vio;
  extern char   gray_palette[5];

  /* global variables */  

  struct Library*  IntuitionBase = NULL;
  struct Library*  GfxBase = NULL;

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


  /* local variables */

  static struct Screen*  fts = NULL;
  static struct Window*  ftw = NULL;

  static int  graphx_mode;


  /* exit gracefully */

  static void  AmigaCleanUp( void )
  {
    if ( ftw )
      CloseWindow( ftw );

    if ( fts )
      CloseScreen( fts );

    if ( IntuitionBase )
      CloseLibrary( IntuitionBase );
  
    if ( GfxBase )
      CloseLibrary( GfxBase );
  
    if ( graphx_mode == Graphics_Mode_Gray )
      if ( Vio )
        FreeMem( Vio, DISPLAY_MEM );
  }


  static  void SetPalette( void )
  {
    short  color[] = { 0x0000,
                       0x0333,
                       0x0777,
                       0x0BBB,
                       0x0FFF,
                       0x0A00,
                       0x00A0,
                       0x000A
                     };
    short  i;


    for ( i = 0; i < 8; i++ )
    {
      if ( i < 5 )
        gray_palette[i] = i;

      SetRGB4( &fts->ViewPort, i, (UBYTE)(color[i] >> 8 & 0x0f),
                                  (UBYTE)(color[i] >> 4 & 0x0f), 
                                  (UBYTE)(color[i] & 0x0f ) );
    }
  }


  /* open libraries & custom screen */

  static int  AmigaInit( void )
  {
    /* cleanup at exit */
    if ( atexit( AmigaCleanUp ) )
    {
      PutStr( "atexit() failed\n" );
      return -1;
    }

    /* open intuition library */

    IntuitionBase = (struct Library*)OpenLibrary( "intuition.library", 37L );
    if ( IntuitionBase == NULL )
    {
      PutStr( "Could not open intuition library\n" );
      return -1;
    }
  
    /* Open graphics library */

    GfxBase = OpenLibrary( "graphics.library", 37L );
    if ( GfxBase == NULL )
    {
      PutStr( "Could not open graphics library\n" );
      return -1;
    }                  

    if ( graphx_mode == Graphics_Mode_Gray )
    {
      /* open custom screen */
      fts = (struct Screen*)OpenScreenTags(
                              NULL,
                              SA_DisplayID, (PAL_MONITOR_ID | LORES_KEY),
                              SA_Width,     GRAY_SCREEN_WIDTH,
                              SA_Height,    GRAY_SCREEN_HEIGHT,
                              SA_Depth,     GRAY_SCREEN_DEPTH,
                              SA_ShowTitle, FALSE,
                              TAG_DONE );

      if ( fts == NULL )
      {
        PutStr( "Could not open custom screen\n" );
        return -1;
      }

     /* set gray palette */
      SetPalette();
    }
    else
    {
      /* open custom screen */
      fts = (struct Screen*)OpenScreenTags(
                              NULL,
                              SA_DisplayID, (PAL_MONITOR_ID | HIRESLACE_KEY),
                              SA_Width,     MONO_SCREEN_WIDTH,
                              SA_Height,    MONO_SCREEN_HEIGHT,
                              SA_Depth,     MONO_SCREEN_DEPTH,
                              SA_ShowTitle, FALSE,
                              TAG_DONE );

      if ( fts == NULL )
      {
        PutStr( "Could not open custom screen\n" );
        return -1;
      }
    }

    /* open intuition window */
    ftw = OpenWindowTags(
            NULL,
            WA_Left,         0,
            WA_Width,        fts->Width,
            WA_Top,          0,
            WA_Height,       fts->Height,
            WA_IDCMP,        IDCMP_VANILLAKEY | IDCMP_MOUSEBUTTONS,
            WA_Flags,        WFLG_BACKDROP | WFLG_BORDERLESS |
                             WFLG_RMBTRAP | WFLG_ACTIVATE,
            WA_Gadgets,      NULL,
            WA_Title,        NULL,
            WA_CustomScreen, fts,
            TAG_DONE );

    if ( ftw == NULL )
    {
      PutStr( "Could not open intuition window\n" );
      return -1;
    }

    if ( graphx_mode == Graphics_Mode_Gray )
    {
      Vio = (char*)AllocMem( DISPLAY_MEM, MEMF_ANY );
  
      if ( !Vio )
      {
        PutStr( "Cannot AllocMem() display memory\n" );
        return -1;
      }

      vio_Width = vio_ScanLineWidth = GRAY_SCREEN_WIDTH;
      vio_Height = GRAY_SCREEN_HEIGHT;
    }
    else
    {
      Vio = (char*)fts->BitMap.Planes[0];
      vio_ScanLineWidth = fts->BitMap.BytesPerRow;
      vio_Width         = MONO_SCREEN_WIDTH;
      vio_Height        = MONO_SCREEN_HEIGHT;
    }

    return 0;
  }


  /* get events in the window */

  static char  Get_Intuition_Event( void )
  {
    struct IntuiMessage*  msg;
    ULONG                 class;
    USHORT                code;


    WaitPort( ftw->UserPort );

    while ( ( msg = (struct IntuiMessage*)GetMsg( ftw->UserPort ) ) )
    {
      class = msg->Class;
      code  = msg->Code;

      ReplyMsg( (struct Message*)msg );

      switch ( class )
      {
      case IDCMP_MOUSEBUTTONS:
        return (char)27;

      case IDCMP_VANILLAKEY:
        return (char)code;
      }
    }

    return '\0';
  }


  /* set Amiga graphics mode */

  int  Driver_Set_Graphics( int  mode )
  {
    graphx_mode = mode;

    
    if ( AmigaInit() == -1 )
      return 0;     /* failure */
  
    return 1;       /* success */
  }


  /* restore screen to its original state */

  int  Driver_Restore_Mode( void )
  {
    /* do nothing */
        
    return 1;       /* success */
  }


  /* display bitmap */

  int  Driver_Display_Bitmap( char*  buffer, int  line, int  col )
  {
    int    y, z;
    char*  target;
    char   old = -1;


    target = Vio + ( line - 1 ) * vio_ScanLineWidth;

    for ( y = 0; y < line; y++ )
    {
      CopyMem( buffer, target, col );
      target -= vio_ScanLineWidth;
      buffer += col;
    }
    
    if ( graphx_mode == Graphics_Mode_Gray )
    {
      /* clear screen */
      SetRast( &fts->RastPort, 0 );

      /* draw glyph */
      for ( y = 0; y < line; y++ )
      {
        for ( z = 0; z < col; z++ )
        {
          int c = Vio[y * vio_ScanLineWidth + z];

          if ( c != 0 )
          {
            if ( old != c )
            {
              if ( c < 0 || c > 5 )
              {
                PutStr( "Unexpected value!\n" );
                SetAPen( &fts->RastPort, 7 );
              }
              else
              {
                old = c;
                SetAPen( &fts->RastPort, c );
              }
            }

            WritePixel( &fts->RastPort, z, y );
          }
        }
      }
    }
    
    return 1;       /* success */
  }


  void  Get_Event( TEvent*  event )
  {
    int   i;
    char  c;

    
    c = Get_Intuition_Event();

    if ( c != '\0' )
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
