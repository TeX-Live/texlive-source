/*******************************************************************
 *
 *  gw_win16.c  graphics driver for 16-bit Windows platform.     0.1
 *
 *  This is the driver for displaying inside a window under 16-bit
 *  Microsoft Windows, used by the graphics utility of the
 *  FreeType test suite.
 *
 *  Written by Antoine Leca.
 *  Copyright 1999-2001 by Antoine Leca,
 *  David Turner, Robert Wilhelm, and Werner Lemberg.
 *
 *  Borrowing liberally from the other FreeType drivers.
 *
 *  This file is part of the FreeType project, and may only be used
 *  modified and distributed under the terms of the FreeType project
 *  license, LICENSE.TXT. By continuing to use, modify or distribute
 *  this file you indicate that you have read the license and
 *  understand and accept it fully.
 *
 ******************************************************************/

#include <windows.h>  /* should be #included before everything else */
                      /* because it #defines NULL in a peculiar way */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gdriver.h"
#include "gevents.h"
#include "gmain.h"


/* The following #ifdef are used to define the following macros :          */
/*                                                                         */
/*  - hInst : variable containing the handle of the current instance.      */
/*  - hPrev : variable containing the handle of the previous instance.     */
/*                                                                         */

/* ---- Microsoft C compilers support ------------------------------------ */

#if defined( M_I86 ) || defined( _M_I86 )

extern HINSTANCE _hInstance, _hPrevInstance;
#define hInst    _hInstance
#define hPrev    _hPrevInstance

#endif

/* ---- Borland C compiler support --------------------------------------- */

#ifdef __TURBOC__

#pragma option -A-

extern HINSTANCE _hInstance, _hPrev;
#define hInst    _hInstance
#define hPrev    _hPrev

#endif


#if !defined ( hInst ) || !defined ( hPrev )
#error Your compiler is not (yet) supported.  Check the source file!
#endif

/* ---- Common initialisations ------------------------------------------- */

/*  Size of the window. */
#define WIN_WIDTH   640u
#define WIN_HEIGHT  400u
/* The values will be divided by 2 for gray-scale rendering.           */

/* These values can be changed, but WIN_WIDTH should remain for now a  */
/* multiple of 32 to avoid padding issues.                             */

/* Also, to avoid 16-bit overflowing issues, the product               */
/* WIN_WIDTH * WIN_HEIGHT should not excess 512K for monochrome        */
/* rendering, and 256K for gray-scale rendering.                       */


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

  /* handle of the window. */
  static HWND   hwndGraphic;

  /* bitmap information */
  static LPBITMAPINFO pbmi;
  static HBITMAP      hbm;

  /* local event to pass on */
  static TEvent ourevent = { event_Quit, 0 };
  static int    eventToProcess = 0;

  /* array defined in the test programs */
  extern char   Header[];


/* restores screen to its original state */

int  Driver_Restore_Mode()
  {
    /* The graphical window has perhaps already destroyed itself */
    if ( hwndGraphic ) {
      DestroyWindow ( hwndGraphic );
      PostMessage ( hwndGraphic, WM_QUIT, 0, 0 );
    }

    if ( pbmi )    free ( pbmi );

    return 1;
  }


/*
 * set graphics mode
 * and create the window class and the message handling.
 */

/* Declarations of the Windows-specific functions that are below. */
static BOOL RegisterTheClass ( void );
static BOOL CreateTheWindow ( int width, int height );


int Driver_Set_Graphics ( int mode )
  {
  int  i;
  static const RGBQUAD gray_scale[5] = {
      { 0xFF, 0xFF, 0xFF, 0 },     /* white */
      { 0xC0, 0xC0, 0xC0, 0 },
      { 0x80, 0x80, 0x80, 0 },
      { 0x40, 0x40, 0x40, 0 },
      {    0,    0,    0, 0 } };   /* black */

    if( ! RegisterTheClass() ) return 0;  /* if already running, fails. */

    /* find some memory for the bitmap header */
    if ( (pbmi = malloc ( sizeof ( BITMAPINFO ) + sizeof ( RGBQUAD ) * 256 ) )
                                /* 256 should really be 2 if not grayscale */
               == NULL )
      /* lack of memory; fails the process */
      return 0;

    /* initialize the header to appropriate values */
    memset( pbmi, 0, sizeof ( BITMAPINFO ) + sizeof ( RGBQUAD ) * 256 );

    switch ( mode )
    {
    case Graphics_Mode_Mono:
      pbmi->bmiHeader.biBitCount = 1;
      pbmi->bmiColors[0] = gray_scale[0];
      pbmi->bmiColors[1] = gray_scale[4];

      vio_ScanLineWidth = WIN_WIDTH / 8;
      vio_Width         = WIN_WIDTH;
      vio_Height        = WIN_HEIGHT;

      break;

    case Graphics_Mode_Gray:
      pbmi->bmiHeader.biBitCount = 8;
      pbmi->bmiHeader.biClrUsed  = 5;

      memcpy ( &pbmi->bmiColors[0], gray_scale, sizeof gray_scale );

      vio_ScanLineWidth = WIN_WIDTH / 2;
      vio_Width         = WIN_WIDTH / 2;
      vio_Height        = WIN_HEIGHT/ 2;

      for ( i = 0; i < 5; ++i )
        gray_palette[i] = i;

      break;

    default:

      free ( pbmi );
      return 0;         /* Unknown mode */
    }

    pbmi->bmiHeader.biSize   = sizeof ( BITMAPINFOHEADER );
    pbmi->bmiHeader.biWidth  = vio_Width;
    pbmi->bmiHeader.biHeight = vio_Height;
    pbmi->bmiHeader.biPlanes = 1;

    if ( (long) vio_Height * vio_ScanLineWidth > 0xFFE0ul )
        /* too big to work on 16-bit; fails the process */
    {
      free ( pbmi );
      return 0;
    }

    if( ! CreateTheWindow ( vio_Width, vio_Height ) )
    {
      free ( pbmi );
      return 0;
    }

    return 1;     /* success even if the window was not built. */
  }


int Driver_Display_Bitmap ( char* buffer, int lines, int cols )
  {
  HDC     hDC;

    if ( cols * 8 != pbmi->bmiHeader.biWidth * pbmi->bmiHeader.biBitCount )
      pbmi->bmiHeader.biWidth  = cols * 8 / pbmi->bmiHeader.biBitCount;

    hDC = GetDC ( hwndGraphic );
    SetDIBits ( hDC, hbm, 0, lines, buffer, pbmi, DIB_RGB_COLORS );
    ReleaseDC ( hwndGraphic, hDC );

    ShowWindow( hwndGraphic, SW_SHOW );
    InvalidateRect ( hwndGraphic, NULL, FALSE );
    UpdateWindow ( hwndGraphic );

    return 1;       /* success */
  }


void Get_Event( TEvent* event )
  {
  MSG msg;

    if ( hwndGraphic )
    {
      SetWindowText ( hwndGraphic, Header );
    }

    do {
      while ( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) ) {
        TranslateMessage ( &msg );
        DispatchMessage  ( &msg );
      }
      if ( ! eventToProcess )
        WaitMessage();
    } while ( ! eventToProcess );

    event->what = ourevent.what;
    event->info = ourevent.info;
    eventToProcess = 0;
    return;
  }


/* ---- Windows-specific stuff ------------------------------------------- */

LRESULT CALLBACK Message_Process( HWND, UINT, WPARAM, LPARAM );

static
BOOL RegisterTheClass ( void )
  {
  WNDCLASS ourClass = {
      /* UINT    style        */ 0,
      /* WNDPROC lpfnWndProc  */ Message_Process,
      /* int     cbClsExtra   */ 0,
      /* int     cbWndExtra   */ 0,
      /* HANDLE  hInstance    */ 0,
      /* HICON   hIcon        */ 0,
      /* HCURSOR hCursor      */ 0,
      /* HBRUSH  hbrBackground*/ 0,
      /* LPCTSTR lpszMenuName */ NULL,
      /* LPCTSTR lpszClassName*/ "FreeTypeTestGraphicDriver16"
  };

    if( hPrev )
      /* There is another instance of the same program. */
      /* No need to register the class.                 */
        return 1;

    ourClass.hInstance    = hInst;
    ourClass.hIcon        = LoadIcon(0, IDI_APPLICATION);
    ourClass.hCursor      = LoadCursor(0, IDC_ARROW);
    ourClass.hbrBackground= GetStockObject(BLACK_BRUSH);

    return RegisterClass(&ourClass) != 0;  /* return False if it fails. */
  }

static
BOOL CreateTheWindow ( int width, int height )
  {
    if ( ! (hwndGraphic = CreateWindow(
        /* LPCSTR lpszClassName;    */ "FreeTypeTestGraphicDriver16",
        /* LPCSTR lpszWindowName;   */ "FreeType Test Graphic Driver",
        /* DWORD dwStyle;           */  WS_OVERLAPPED | WS_SYSMENU,
        /* int x;                   */  CW_USEDEFAULT,
        /* int y;                   */  CW_USEDEFAULT,
        /* int nWidth;              */  width + 2*GetSystemMetrics(SM_CXBORDER),
        /* int nHeight;             */  height+ GetSystemMetrics(SM_CYBORDER)
                                              + GetSystemMetrics(SM_CYCAPTION),
        /* HWND hwndParent;         */  HWND_DESKTOP,
        /* HMENU hmenu;             */  0,
        /* HINSTANCE hinst;         */  hInst,
        /* void FAR* lpvParam;      */  NULL))
       )
         /*  creation failed... */
         return 0;

    return 1;
  }

  /* Message processing for our Windows class */
LRESULT CALLBACK Message_Process( HWND handle, UINT mess,
                                  WPARAM wParam, LPARAM lParam )
  {

    switch( mess )
    {
    case WM_DESTROY:
        /* warn the main thread to quit if it didn't know */
      ourevent.what = event_Quit;
      ourevent.info = 0;
      eventToProcess = 1;
      hwndGraphic = 0;
      PostQuitMessage ( 0 );
      DeleteObject ( hbm );
      break;

    case WM_CREATE:
      {
      HDC     hDC;

        hDC = GetDC ( handle );
        hbm = CreateDIBitmap (
          /* HDC hdc;     handle of device context        */ hDC,
          /* BITMAPINFOHEADER FAR* lpbmih;  addr.of header*/ &pbmi->bmiHeader,
          /* DWORD dwInit;  CBM_INIT to initialize bitmap */ 0,
          /* const void FAR* lpvBits;   address of values */ NULL,
          /* BITMAPINFO FAR* lpbmi;   addr.of bitmap data */ pbmi,
          /* UINT fnColorUse;      RGB or palette indices */ DIB_RGB_COLORS);
        ReleaseDC ( handle, hDC );
        break;
      }

    case WM_PAINT:
      {
      HDC     hDC, memDC;
      HANDLE  oldbm;
      PAINTSTRUCT ps;

        hDC = BeginPaint ( handle, &ps );
        memDC = CreateCompatibleDC(hDC);
        oldbm = SelectObject(memDC, hbm);
        BitBlt ( hDC, 0, 0, vio_Width, vio_Height, memDC, 0, 0, SRCCOPY);
        ReleaseDC ( handle, hDC );
        SelectObject ( memDC, oldbm );
        DeleteObject ( memDC );
        EndPaint ( handle, &ps );
      }

    case WM_KEYDOWN:
      switch ( wParam )
      {
      case VK_ESCAPE:
        ourevent.what = event_Quit;
        ourevent.info = 0;
        eventToProcess = 1;
        break;

      case VK_F1:           /*  bring up help and about dialog window */
        break;
      }
      break;

    case WM_CHAR:
      {
      char  c = wParam ;
      int   i;

        for ( i = 0; i < NUM_Translators; i++ )
        {
          if ( c == trans[i].key )
          {
            ourevent.what = trans[i].event_class;
            ourevent.info = trans[i].event_info;
            eventToProcess = 1;
            return 0;
          }
        }

        /* unrecognized keystroke */
        ourevent.what = event_Keyboard;
        ourevent.info = (int)c;
        eventToProcess = 1;
      }
      break;

    default:
       return DefWindowProc( handle, mess, wParam, lParam );
    }

    return 0;
  }

/* End */
