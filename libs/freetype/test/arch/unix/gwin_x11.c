/*******************************************************************
 *
 *  gwin_x11.c  graphics utility X-Window driver.              1.0
 *
 *  This is the driver for windowed display under X11, used by the
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
#include <string.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>

#include "common.h"     /* for Panic() */
#include "freetype.h"

#include "gdriver.h"
#include "gevents.h"
#include "gmain.h"


  /* Translator added to ease changes to control keys */

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

  /* End of translators addition. See Get_Event() below */

  extern char      Header[];    /* defined in the test programs */

  static Window    win;
  static GC        gcblack;
  static XColor    color[5];

  Display*         display;
  static char*     displayname = "";

  static XImage*   image;

  static Visual*   visual;
  static Colormap  colormap;
  static int       depth;
  static Bool      gray;

  static Cursor    idle;
  static Cursor    busy;

  static int       win_origin_x, win_origin_y,
                   win_width, win_height;
  static int       image_width, image_height;

  long             vioBufOfs;



  /* restore acreen to its original state */

  int  Driver_Restore_Mode( void )
  {
    XUnmapWindow( display, win );
    XCloseDisplay( display );

    return 1;       /* success */
  }


  /* set graphics mode */

  void  x11init( void )
  {
    int                   screen_num, i;
    XTextProperty         xtp;
    XSizeHints            xsh;
    XSetWindowAttributes  xswa;

#if 1
    unsigned short colors[5] = { 0, 16, 32, 48, 63 }; /* gamma = 1.0 */
#else
    unsigned short colors[5] = { 0, 34, 46, 55, 63 }; /* gamma = 2.2 */
#endif


    XrmInitialize();

    if( !( display = XOpenDisplay( displayname ) ) )
      Panic( "ERROR: cannot open display\n" );

    screen_num = DefaultScreen  ( display );
    colormap   = DefaultColormap( display, screen_num );
    depth      = DefaultDepth   ( display, screen_num );
    visual     = DefaultVisual  ( display, screen_num );

    idle       = XCreateFontCursor( display, XC_left_ptr );
    busy       = XCreateFontCursor( display, XC_watch );

    if ( gray )
    {
      int                   count;
      XPixmapFormatValues*  formats;


      formats           = XListPixmapFormats( display, &count );
      vio_ScanLineWidth = 0;

      while ( count > 0 )
      {
        --count;
        if ( formats[count].depth == depth )
        {
          int  bits;

          bits = win_width * formats[count].bits_per_pixel;
          if ( bits % formats[count].scanline_pad )
          {
            bits -= bits % formats[count].scanline_pad;
            bits += formats[count].scanline_pad;
          }

          vio_ScanLineWidth = bits / 8;
          break;
        }
      }
      if ( !vio_ScanLineWidth )
        Panic( "ERROR: the display doesn't offer a suitable pixmap format\n" );

      XFree( formats );
    }

    Vio = (char*)malloc( win_height * vio_ScanLineWidth );

    if ( !Vio )
      Panic( "ERROR: cannot malloc display memory\n" );

    xswa.border_pixel     = BlackPixel( display, screen_num );
    xswa.background_pixel = WhitePixel( display, screen_num );
    xswa.cursor           = busy;

    xswa.event_mask = KeyPressMask | ExposureMask;
    
    win = XCreateWindow( display,
                         RootWindow( display, screen_num ),
                         win_origin_x,
                         win_origin_y,
                         win_width,
                         win_height,
                         10,
                         depth,
                         InputOutput, 
                         visual,
                         CWBackPixel | CWBorderPixel | CWEventMask | CWCursor,
                         &xswa );

    XMapWindow( display, win );
 
    gcblack = XCreateGC( display, RootWindow( display, screen_num ),
                         0L, NULL );
    XSetForeground( display, gcblack, BlackPixel( display, screen_num ) );
    XSetBackground( display, gcblack, WhitePixel( display, screen_num ) );

    /* allocate colors */

    if ( gray )
      for ( i = 0; i < 5; i++ )
      {
        gray_palette[i] = i;
  
        color[i].red   =
        color[i].green =
        color[i].blue  = 65535 - ( colors[i] * 65535 ) / 63;
  
        if ( !XAllocColor( display, colormap, &color[i] ) )
          Panic( "ERROR: cannot allocate Color\n" );
      }

    image = XCreateImage( display, 
                          visual,
                          gray ? depth   : 1, 
                          gray ? ZPixmap : XYBitmap,
                          0, 
                          (char*)Vio, 
                          win_width, 
                          win_height, 
                          8, 
                          0 );
    if ( !image )
      Panic( "ERROR: cannot create image\n" );

    if ( !gray )
    {
      image->byte_order       = MSBFirst;
      image->bitmap_bit_order = MSBFirst;
    }

    /* make window manager happy :-) */
    xtp.value    = (unsigned char*)"FreeType";
    xtp.encoding = 31;
    xtp.format   = 8;
    xtp.nitems   = strlen( (char*)xtp.value );

    xsh.x = win_origin_x;
    xsh.y = win_origin_y;

    xsh.width  = win_width;
    xsh.height = win_height;
    xsh.flags  = (PPosition | PSize);
    xsh.flags  = 0;

    XSetWMProperties( display, win, &xtp, &xtp, NULL, 0, &xsh, NULL, NULL );
  }


  int  Driver_Set_Graphics( int  mode )
  {
    if ( mode == Graphics_Mode_Gray )
    {
      gray = 1;
      vio_ScanLineWidth = 320;

      win_origin_x = 0;
      win_origin_y = 0;
      win_width  = 320;
      win_height = 200;
    }
    else if ( mode == Graphics_Mode_Mono )
    {
      gray = 0;
      vio_ScanLineWidth = 80;

      win_origin_x = 0;
      win_origin_y = 0;
      win_width  = 640;
      win_height = 450;
    }
    else
      Panic( "ERROR: mode %d not supported\n", mode );

    vio_Width  = win_width;
    vio_Height = win_height;

    x11init();

    return 1;       /* success */
  }


  void  Put_Image( int  x, int  y, int  w, int  h )
  {
    XPutImage( display, win, gcblack, image, x, y, x, y, w, h );
  }


  int  Driver_Display_Bitmap( char*  buffer, int  line, int  col )
  {
    int    z, y, used_col;
    char*  target;


    XClearWindow( display, win );

    /* this displays the Header string in the window title */
    XStoreName( display, win, Header );

    if ( line > win_height )
      line = win_height;

    if ( !gray )
    {
      if ( col > vio_ScanLineWidth )
        used_col = vio_ScanLineWidth;
      else
        used_col = col;

      target = Vio + ( line - 1 ) * vio_ScanLineWidth;

      for ( y = 0; y < line; y++ )
      {
        memcpy( (char*)target, buffer, used_col );
        target -= vio_ScanLineWidth;
        buffer += col;
      }

      Put_Image( 0, 0, used_col * 8, line );
      image_width  = used_col * 8;
      image_height = line;
    }
    else
    {
      if ( col > win_width )
        used_col = win_width;
      else
        used_col = col;

      for ( y = line - 1; y >= 0; y-- )
      {
        char*  bufp;


        bufp = buffer;

        for ( z = 0; z < used_col; z++ )
        {
          int  c;


          c = *bufp++;

          if ( c < 0 || c >= 5 ) /* security check */
          {
            /* Message( "weird grayshade: %d\n", c ); */
            c = 0;
          }
          XPutPixel( image, z, y, color[c].pixel );
        }

        buffer += col;
      }

      Put_Image( 0, 0, used_col, line );
      image_width  = used_col;
      image_height = line;
    }

    return 1;
  }


  /* This function maps X keystrokes into GEvents.  Note that */
  /* currently only keystrokes events exit this function.     */

  void  Get_Event( TEvent*  event )
  {
    static char     key_buffer[10];
    static int      key_cursor = 0;
    static int      key_number = 0;
    static XEvent   x_event;
           KeySym   key;

    int             i, bool_exit;
    char            c;

    XComposeStatus  compose;


    bool_exit = key_cursor < key_number;

    XDefineCursor( display, win, idle );

    while ( !bool_exit )
    {
      XNextEvent( display, &x_event );

      switch ( x_event.type )
      {
      case KeyPress:
        key_number = XLookupString( &x_event.xkey,
                                    key_buffer,
                                    sizeof ( key_buffer ),
                                    &key,
                                    &compose );
        key_cursor = 0;

        if ( key_number > 0 )
          bool_exit = 1;
        break;

      case MappingNotify:
        XRefreshKeyboardMapping( &x_event.xmapping );
        break;
      
      case Expose:
#if 0
        Put_Image( x_event.xexpose.x, 
                   x_event.xexpose.y,
                   x_event.xexpose.width, 
                   x_event.xexpose.height );
#else
        /* we always redraw the whole image */
        Put_Image( 0, 0, image_width, image_height );
#endif
        break;

      /* You should add more cases to handle mouse events, etc. */
      }
    }

    XDefineCursor( display, win, busy );
    XFlush       ( display );

    c = key_buffer[key_cursor++];

    for ( i = 0; i < NUM_Translators; i++ )
    {
      if ( c == trans[i].key )
      {
        event->what = trans[i].event_class;
        event->info = trans[i].event_info;
        return;
      }
    }

    event->what = event_Keyboard;
    event->info = (int)c;
  }


/* End */
