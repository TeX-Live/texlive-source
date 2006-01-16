/*******************************************************************
 *
 *  gpm_os2.c  graphics OS/2 Presentation Manager Window driver. 0.2
 *
 *  This is the driver for windowed OS/2 display, used by the
 *  graphics utility of the FreeType test suite.
 *
 *  written by Eric Olson (eolson@imag.net) 
 *
 *  Borrowing liberally from the other FreeType drivers.
 *  Some bitmap manipulations are derived from fastgpi.c,
 *  a sample program written by Donald Graft (dgraft@gate.net).
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

#define INCL_DOS
#define INCL_WIN
#define INCL_GPI
#define INCL_SUB

#include <os2.h>

#include "gdriver.h"
#include "gevents.h"
#include "gmain.h"


#define VIO_WIDTH   640u /* these can be changed but VIO_WIDTH should remain */
#define VIO_HEIGHT  360u /* for now a multiple of 32 to avoid padding issues */
#define MAG_WIDTH   VIO_WIDTH     
#define MAG_HEIGHT  120u
#define MAX_MAG     16   /* should be less than Min(MAG_WIDTH, MAG_HEIGHT) */


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
 

  static HAB   habt;
  static HAB   hab;
  static HWND  hwndFrame, hwndClient;
  static HWND  hwndTitle;
  static HDC   hdcMemory;
  static HPS   hpsMemory = (HPS) NULL;

         /* Threads and semaphores */
         TID   MessageThread;                         
         HMTX  hmtxPSMemoryLock;
         HEV   hevKeyLock;

         /* Bitmap information */
  static PBITMAPINFO2  pbmi;                              
  static HBITMAP       hbm;
         BYTE          Bitmap[VIO_WIDTH * VIO_HEIGHT];
         BOOL          ready = FALSE;

  /* Coordinates for the bitblt of whole graphic area */
  POINTL  aptlFull[4] = {{ 0u, MAG_HEIGHT },
                         { VIO_WIDTH, VIO_HEIGHT + MAG_HEIGHT },
                         { 0u, 0u },
                         { VIO_WIDTH, VIO_HEIGHT }};

  /* Coordinates for the magnification bitblt */
  POINTL  aptlMagd[4] = {{ 0u, 0u },
                         { MAG_WIDTH, MAG_HEIGHT },       /* target */
                         { 0u, 0u },
                         { VIO_WIDTH, VIO_HEIGHT }};      /* source */

   /* level of magnification and center of magnification window */
   static int     magnification=1;
   static POINTL  view_target = {0, 0};
   static SIZEL   mag_win_size;

  /* local event to pass on */
  TEvent  ourevent = { event_Rotate_Glyph, 0 };

  /* grayscale vs b/w mode */
  int  Colourmode;

  /* array defined in the test programs */
  extern char Header[];


  void              RunPMWindow( ULONG );
  MRESULT EXPENTRY  Message_Process( HWND, ULONG, MPARAM, MPARAM );



  /* restores screen to its original state */

  int  Driver_Restore_Mode()
  {
    /* PMWindow has probably already destroyed itself */
    if ( hwndFrame ) 
      WinDestroyWindow( hwndFrame );

    WinReleasePS( hpsMemory );
    WinTerminate( habt );

    return 1;
  }


  /* set graphics mode */

  int  Driver_Set_Graphics( int  mode )
  {
    LONG    palette[5];
    int     x;
    POINTL  coords;
    SIZEL   sizl = { 0, 0 };

    PTIB  thread_block;
    PPIB  process_block;

    /* XXX : This is a very nasty hack, it fools OS/2 and let the program */
    /*       call PM functions, even though stdin/stdout/stderr are still */
    /*       directed to the standard i/o streams..                       */
    /*       The program must be compiled with WINDOWCOMPAT               */
    /*                                                                    */
    /*   Credits go to Michal for finding this !!                         */
    /*                                                                    */
    DosGetInfoBlocks( &thread_block, &process_block );
    process_block->pib_ultype = 3;

    /* save mono vs grayscale status */
    Colourmode = mode;

    /* event semaphore to signal reraster event */
    DosCreateEventSem( NULL, &hevKeyLock, 0, TRUE );

    /* mutex semaphore for access to the presentation space */
    DosCreateMutexSem( NULL, &hmtxPSMemoryLock, 0, FALSE );

    /* Start thread with Presentation Manager window */
    DosCreateThread( &MessageThread, (PFNTHREAD)RunPMWindow, 0UL, 0UL, 32920 );

    /* open anchor block to permit Gpi calls from this thread */
    habt = WinInitialize( 0 );

    /* create device context and presentation space for our graphic */
    hdcMemory = DevOpenDC( hab, OD_MEMORY, (PSZ)"*", 0L, 0L, 0L );

    DosRequestMutexSem( hmtxPSMemoryLock, SEM_INDEFINITE_WAIT );
    hpsMemory = GpiCreatePS(
                  habt, hdcMemory, &sizl,
                  PU_PELS | GPIT_MICRO | GPIA_ASSOC | GPIF_DEFAULT );
    GpiSetBackMix( hpsMemory, BM_OVERPAINT );

    /* create bitmap for raster image of graphic */

    /* find some memory for the bitmap header */
    DosAllocMem( (PPVOID)&pbmi,
                 sizeof ( BITMAPINFO2 ) + sizeof ( RGB2 ) * 256,
                 PAG_COMMIT | PAG_READ | PAG_WRITE);
                        /* 256 should really be 2 if not grayscale */

    /* initialize the header to appropriate values */
    memset( pbmi, 0, sizeof ( BITMAPINFO2 ) + sizeof ( RGB2 ) * 256 );

    pbmi->cbFix   = sizeof ( BITMAPINFOHEADER2 );
    pbmi->cx      = VIO_WIDTH;
    pbmi->cy      = VIO_HEIGHT;
    pbmi->cPlanes = 1;

    switch ( mode )
    {
    case Graphics_Mode_Mono:
      pbmi->cBitCount = 1;
      break;

    case Graphics_Mode_Gray:
      pbmi->cBitCount = 8;
      break;
    }

    hbm = GpiCreateBitmap( hpsMemory, (PBITMAPINFOHEADER2)pbmi,
                           0L, NULL, NULL );

    /* associate it with the presentation space */
    GpiSetBitmap( hpsMemory, hbm );
    pbmi->cbFix = sizeof ( BITMAPINFOHEADER2 );
    GpiQueryBitmapInfoHeader( hbm, (PBITMAPINFOHEADER2) pbmi );

    switch ( mode )
    {
    case Graphics_Mode_Mono:
      DosReleaseMutexSem( hmtxPSMemoryLock );

      vio_ScanLineWidth = VIO_WIDTH / 8;
      vio_Width         = VIO_WIDTH;
      vio_Height        = VIO_HEIGHT;

      gray_palette[0]   = 0;    /* to avoid testing for grayscale... */
      break;

    case Graphics_Mode_Gray:
      vio_ScanLineWidth = VIO_WIDTH;
      vio_Width         = VIO_WIDTH;
      vio_Height        = VIO_HEIGHT;

      /* set gray_palette by convoluted procedure */

      /* create logical color palette */
      palette[0] = 0xffffffL;       /* White */
      palette[1] = 0xbbbbbbL;
      palette[2] = 0x777777L;       /* Gray  */
      palette[3] = 0x333333L;
      palette[4] = 0L;              /* Black */

      GpiCreateLogColorTable( hpsMemory, (ULONG)LCOL_PURECOLOR,
                              (LONG)LCOLF_CONSECRGB, (LONG)0L,
                              (LONG)5L, (PLONG)palette );

      /* plot to presentation space in all five gray shades */
      for (x = 0 ; x < 5 ; x++) 
      {
        GpiSetColor( hpsMemory, (LONG)x );
        coords.x = x;
        coords.y = 0;
        GpiSetPel( hpsMemory, &coords ); 
      }  

      /* retrieve the 5 pixels as gray_palette */
      GpiQueryBitmapInfoHeader( hbm, (PBITMAPINFOHEADER2) pbmi );
      GpiQueryBitmapBits( hpsMemory, 0L, (LONG)VIO_HEIGHT - 2,
                          &Bitmap[0], pbmi );
      for ( x = 0; x < 5; x++ )
      {
        gray_palette[x] = Bitmap[x];
      } 

      /* initialization in case we paint before Driver_Display is called */
      memset( &Bitmap[0], gray_palette[0], vio_Height * vio_ScanLineWidth );
      DosReleaseMutexSem( hmtxPSMemoryLock );
      break;

    default:
      return 0;         /* Unknown mode */
    }

    return 1;     /* success even if windows were not setup right */
  }


  int  Driver_Display_Bitmap( char*  buffer, int  lines, int  cols )
  {
    int  y, target;


    /* copy the bitmap and blt to presentation space */
    if ( (lines == vio_Height) & (cols == vio_ScanLineWidth) )
      memcpy( &Bitmap[0], buffer, lines * cols );
    else
    {
      memset( &Bitmap[0], gray_palette[0], vio_Height * vio_ScanLineWidth );
      /* temporary hack to center any bitmap */
      target = ( vio_Height - lines ) / 2 * vio_ScanLineWidth +
               ( vio_ScanLineWidth - cols ) / 2;

      for ( y = 0 ; y < lines ; y++ )
      {
        memcpy( &Bitmap[target], buffer, cols );
        target += vio_ScanLineWidth;
        buffer += cols;
      }
    }

    /* Get permission and write to in-memory ps */
    DosRequestMutexSem( hmtxPSMemoryLock, SEM_INDEFINITE_WAIT );
    GpiSetBitmapBits( hpsMemory, 0L, (LONG)VIO_HEIGHT - 2, &Bitmap[0], pbmi );
    DosReleaseMutexSem( hmtxPSMemoryLock ); 
    ready = TRUE;

    /* Invalidate and ask for redraw now */
    WinInvalidateRect( hwndClient, NULL, FALSE );
    WinUpdateWindow( hwndFrame );

    return 1;       /* success */
  }


  void  Get_Event( TEvent*  event )
  {
    ULONG  ulRequestCount;


    /* the Get_Event function blocks until there is an event to process */
    DosWaitEventSem( hevKeyLock, SEM_INDEFINITE_WAIT );
    DosQueryEventSem( hevKeyLock, &ulRequestCount );
    DosResetEventSem( hevKeyLock, &ulRequestCount );
    event->what = ourevent.what;
    event->info = ourevent.info;
    return;
  }


  void  RunPMWindow( ULONG  dummy )
  {
    unsigned char   classname[] = "DisplayClass";
             ULONG  flClassFlags;
    static   HMQ    hmq;
             QMSG   qmsg;


    if ( (hab = WinInitialize( 0 )) == 0 )
    {
      printf( "Error doing WinInitialize()\n" );
      return;
    }

    if ( (hmq = WinCreateMsgQueue( hab, 0 )) == (HMQ)NULL )
    {
      printf( "Error doing WinCreateMsgQueue()\n" );
      return;
    }

    if ( !WinRegisterClass( hab, (PSZ)classname, (PFNWP)Message_Process,
                            CS_SIZEREDRAW, 0 ) )
    {
      printf( "Error doing WinRegisterClass()\n" );
      return;
    }

    flClassFlags = FCF_TITLEBAR | FCF_MINBUTTON | FCF_DLGBORDER | 
                   FCF_TASKLIST | FCF_SYSMENU; 
    if ( (hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                          WS_VISIBLE,
                                          &flClassFlags,
                                          (PSZ)classname,
                                          (PSZ)"FreeType PM Graphics",
                                          WS_VISIBLE,
                                          0, 0, &hwndClient )) == 0 )
    {
      printf( "Error doing WinCreateStdWindow()\n" );
      return;
    }

    /* find the title window handle */
    hwndTitle = WinWindowFromID( hwndFrame, FID_TITLEBAR );

    /* set Window size and position */
    WinSetWindowPos(
      hwndFrame, 0L,
      (SHORT)60,
      (SHORT)WinQuerySysValue( HWND_DESKTOP, SV_CYSCREEN ) - 
         ( VIO_HEIGHT + MAG_HEIGHT + 100 ),
      (SHORT)WinQuerySysValue( HWND_DESKTOP, SV_CYDLGFRAME ) * 2 +
         VIO_WIDTH,
      (SHORT)WinQuerySysValue( HWND_DESKTOP, SV_CYTITLEBAR ) +
         WinQuerySysValue( HWND_DESKTOP, SV_CYDLGFRAME ) * 2 + 
         VIO_HEIGHT + MAG_HEIGHT,
      SWP_SIZE | SWP_MOVE ) ;
                                
    /* run the message queue till the end */
    while ( WinGetMsg( hab, &qmsg, (HWND)NULL, 0, 0 ) )
      WinDispatchMsg( hab, &qmsg );

    /* clean-up */
    WinDestroyWindow( hwndFrame );
    hwndFrame = (HWND)NULL;
    WinDestroyMsgQueue( hmq );
    WinTerminate( hab );

    /* await death... */
    while ( 1 )
      DosSleep( 100 );
  }


  void Adjust_Mag_Rectangle( void )
  {
     SIZEL  source, 
            target; 


     /* Step 1, find optimal source size for this mag and window size */
     source.cx = mag_win_size.cx / magnification;
     if (source.cx > vio_Width)    source.cx = vio_Width; 
     source.cy = mag_win_size.cy / magnification;
     if (source.cy > vio_Height)   source.cy = vio_Height; 

     target.cx = source.cx * magnification;
     target.cy = source.cy * magnification;

     aptlMagd[0].x = (mag_win_size.cx - target.cx) / 2;
     aptlMagd[0].y = (mag_win_size.cy - target.cy) / 2;
     aptlMagd[1].x = aptlMagd[0].x + target.cx - 1;
     aptlMagd[1].y = aptlMagd[0].x + target.cy - 1;

     /* Step 2, try crosshairs point dependent coordinates */
     aptlMagd[2].x = view_target.x - source.cx / 2;
     aptlMagd[2].y = view_target.y - source.cy / 2;
     if (aptlMagd[2].x < 0 )   aptlMagd[2].x = 0;
     if (aptlMagd[2].y < 0 )   aptlMagd[2].y = 0;
     if (aptlMagd[2].x > vio_Width - source.cx)
         aptlMagd[2].x = vio_Width - source.cx;
     if (aptlMagd[2].y > vio_Height - source.cy)
         aptlMagd[2].y = vio_Height - source.cy;

     aptlMagd[3].x = aptlMagd[2].x + source.cx - 1;
     aptlMagd[3].y = aptlMagd[2].y + source.cy - 1;

  }    /*   End of Adjust_Mag_Rectangle; */


  /* Message processing for our PM Window class */
  MRESULT EXPENTRY  Message_Process( HWND handle, ULONG mess,
                                     MPARAM parm1, MPARAM parm2 )
  {
     static HDC     hdc;
     static HPS     hps;
     static BOOL    minimized;

            POINTL  top_corner, bottom_corner;
            SWP     swp;
            int     i;


    switch( mess )
    {
    case WM_DESTROY:
      /* warn the main thread to quit if it didn't know */
      ourevent.what = event_Quit;
      ourevent.info = 0;
      DosPostEventSem( hevKeyLock );
      break;

    case WM_CREATE:
      /* set original magnification */
      magnification = 4;
      minimized = FALSE;

      /* create Device Context and Presentation Space for screen. */
                                   /* could we use a cached one ? */
      hdc = WinOpenWindowDC( handle );
      mag_win_size.cx = 0;
      mag_win_size.cy = 0;
      hps = GpiCreatePS( hab, hdc, &mag_win_size,
                         PU_PELS | GPIT_MICRO | GPIA_ASSOC | GPIF_DEFAULT );

      /* Set to size of magnifier window */
      mag_win_size.cx = MAG_WIDTH;
      mag_win_size.cy = MAG_HEIGHT;
      Adjust_Mag_Rectangle();

      /* take the input focus */
      WinFocusChange( HWND_DESKTOP, handle, 0L );
      break;

    case WM_BUTTON1DOWN:
      if ( MOUSEMSG( &mess )->y >= MAG_HEIGHT )
      {
        view_target.x = MOUSEMSG( &mess )->x;
        view_target.y = MOUSEMSG( &mess )->y - MAG_HEIGHT;
        Adjust_Mag_Rectangle();
        WinInvalidateRect( hwndClient, NULL, FALSE );
      }

      return WinDefWindowProc( handle, mess, parm1, parm2 );
      break;

    case WM_MINMAXFRAME:
      /* to update minimized if changed */
      swp = *((PSWP) parm1);
      if ( swp.fl & SWP_MINIMIZE ) 
        minimized = TRUE;
      if ( swp.fl & SWP_RESTORE )
        minimized = FALSE;
      return WinDefWindowProc( handle, mess, parm1, parm2 );
      break;

    case WM_ERASEBACKGROUND:
    case WM_PAINT:  
      /* reset the window title only if not minimized */
      if ( !minimized ) 
        WinSetWindowText( hwndTitle, Header ); 

      /* copy the memory image of the screen out to the real screen */
      DosRequestMutexSem( hmtxPSMemoryLock, SEM_INDEFINITE_WAIT );
      WinBeginPaint( handle, hps, NULL );
      
      /* main image and magnified picture */
      GpiBitBlt( hps, hpsMemory, 4L, aptlFull, ROP_SRCCOPY, BBO_AND );
      GpiBitBlt( hps, hpsMemory, 4L, aptlMagd, ROP_SRCCOPY, BBO_AND );

      /* double-dash the magnifing bounding box.  Paint the mag liner? */
      if ( magnification != 1 )
      {
        GpiSetLineType( hps, LINETYPE_LONGDASH );

        bottom_corner.x = aptlMagd[2].x - 1;
        bottom_corner.y = aptlMagd[2].y + MAG_HEIGHT - 1;
        top_corner.x    = aptlMagd[3].x ;
        top_corner.y    = aptlMagd[3].y + MAG_HEIGHT;

        GpiMove( hps, &bottom_corner );
        GpiBox( hps, DRO_OUTLINE, &top_corner, 0L, 0L );

#if 0
        GpiSetClipRegion();
        GpiErase();
#endif
      }

      WinEndPaint( hps );
      DosReleaseMutexSem( hmtxPSMemoryLock );   
      break;

    case WM_CHAR:
      if ( CHARMSG( &mess )->fs & KC_KEYUP )
        break;

      switch ( CHARMSG( &mess )->vkey )
      {
      case VK_ESC:
        ourevent.what = event_Quit;
        ourevent.info = 0;
        DosPostEventSem( hevKeyLock );
        break;

      case VK_PAGEDOWN:
        if ( magnification < MAX_MAG )
        {
          magnification += 1;
          Adjust_Mag_Rectangle();
          WinInvalidateRect( handle, NULL, FALSE );
        }
        break;

      case VK_PAGEUP:
        if ( magnification > 1 )
        {
          magnification -= 1;
          Adjust_Mag_Rectangle();
          WinInvalidateRect( handle, NULL, FALSE );
        }
        break;

      case VK_LEFT:
        if ( view_target.x > 0 )
        {
          view_target.x -= 1;
          Adjust_Mag_Rectangle();
          WinInvalidateRect( handle, NULL, FALSE );
        }
        break;

      case VK_RIGHT:
        if ( view_target.x < VIO_WIDTH - 1 )
        {
          view_target.x += 1;
          Adjust_Mag_Rectangle();
          WinInvalidateRect( handle, NULL, FALSE );
        }
        break;

      case VK_DOWN:
        if ( view_target.y > 0 )
        {
          view_target.y -= 1;
          Adjust_Mag_Rectangle();
          WinInvalidateRect( handle, NULL, FALSE );
        }
        break;

      case VK_UP:
        if ( view_target.y < VIO_HEIGHT - 1 )
        {
          view_target.y += 1;
          Adjust_Mag_Rectangle();
          WinInvalidateRect( handle, NULL, FALSE );
        }
        break;

      case VK_F1:           /*  bring up help and about dialog window */
        break;
      }

      if ( CHARMSG( &mess )->fs & KC_CHAR )
      {
        char c = (CHAR)CHARMSG( &mess )->chr ;


        for ( i = 0; i < NUM_Translators; i++ )
        {
          if ( c == trans[i].key )
          {
            ourevent.what = trans[i].event_class;
            ourevent.info = trans[i].event_info;
            DosPostEventSem( hevKeyLock );  
            return (MRESULT)TRUE;
          }
        }

        /* unrecognized keystroke */
        ourevent.what = event_Keyboard;
        ourevent.info = (int)c;
        DosPostEventSem( hevKeyLock );  
      }
      break;

    default:
      return WinDefWindowProc( handle, mess, parm1, parm2 );
    }

    return (MRESULT) FALSE;
  }


/* End */
