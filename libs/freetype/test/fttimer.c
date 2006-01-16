/****************************************************************************/
/*                                                                          */
/*  The FreeType project - a Free and Portable Quality TrueType Renderer.   */
/*                                                                          */
/*  Copyright 1996-2001 by                                                  */
/*  D. Turner, R.Wilhelm, and W. Lemberg                                    */
/*                                                                          */
/*  fttimer: A simple performance benchmark.  Now with graylevel rendering  */
/*           with the '-g' option.                                          */
/*                                                                          */
/*           Be aware that the timer program benchmarks different things    */
/*           in each release of the FreeType library.  Thus, performance    */
/*           should only be compared between similar release numbers.       */
/*                                                                          */
/*                                                                          */
/*  NOTE: This is just a test program that is used to show off and          */
/*        debug the current engine.  In no way does it shows the final      */
/*        high-level interface that client applications will use.           */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "freetype.h"

#include "gdriver.h"
#include "gevents.h"
#include "gmain.h"

#ifndef __USE_MISC
#define __USE_MISC   /* MkLinux needs this to get a definition of
                        CLOCKS_PER_SEC */
#endif

#include <time.h>    /* for clock() */

/* SunOS 4.1.* does not define CLOCKS_PER_SEC, so include <sys/param.h> */
/* to get the HZ macro which is the equivalent.                         */
#if defined(__sun__) && !defined(SVR4) && !defined(__SVR4)
#include <sys/param.h>
#define CLOCKS_PER_SEC HZ
#endif

#define MAX_GLYPHS  512   /* Maximum number of glyphs rendered at one time */

  char  Header[128];

  TT_Error     error;

  TT_Engine    engine;
  TT_Face      face;
  TT_Instance  instance;
  TT_Glyph     glyph;

  TT_Outline        outline;
  TT_Glyph_Metrics  metrics;

  TT_Face_Properties  properties;

  TT_F26Dot6*   cur_x;
  TT_F26Dot6*   cur_y;

  unsigned short*  cur_endContour;
  unsigned char*   cur_touch;

  TT_Outline  outlines[MAX_GLYPHS];

  int             num_glyphs;
  int             tab_glyphs;
  int             cur_glyph;
  int             cur_point;
  unsigned short  cur_contour;

  extern TT_Raster_Map  Bit;

  int  Fail;
  int  Num;

  short  visual;      /* display glyphs while rendering */
  short  gray_render; /* smooth fonts with gray levels  */


  static void Clear_Buffer( void );


/*******************************************************************/
/*                                                                 */
/*  Get_Time:                                                      */
/*                                                                 */
/*    Returns the current time in milliseconds.                    */
/*                                                                 */
/*******************************************************************/

  long  Get_Time( void )
  {
    return clock() * 1000 / CLOCKS_PER_SEC;
  }


/*******************************************************************/
/*                                                                 */
/*  Init_Engine:                                                   */
/*                                                                 */
/*    Allocates bitmap, render pool and other structs...           */
/*                                                                 */
/*******************************************************************/

  void  Init_Engine( void )
  {
    Bit.rows  = vio_Height;   /* The whole window */
    Bit.width = vio_Width;

    if ( gray_render )
    {
      Bit.cols = Bit.width;
      Bit.flow = TT_Flow_Up;
      Bit.size = Bit.rows * Bit.width;
    }
    else
    {
      Bit.cols = (Bit.width + 7) / 8;       /* convert to # of bytes */
      Bit.flow = TT_Flow_Up;
      Bit.size = Bit.rows * Bit.cols;       /* number of bytes in buffer */
    }

    Bit.bitmap = (void*)malloc( Bit.size );
    if ( !Bit.bitmap )
      Panic( "ERROR: not enough memory to allocate bitmap!\n" );

    Clear_Buffer();
  }


/*******************************************************************/
/*                                                                 */
/*  Clear_Buffer:                                                  */
/*                                                                 */
/*    Clears current bitmap.                                       */
/*                                                                 */
/*******************************************************************/

  static void  Clear_Buffer( void )
  {
    if ( gray_render )
      memset( Bit.bitmap, gray_palette[0], Bit.size );
    else
      memset( Bit.bitmap, 0, Bit.size );
  }


/*******************************************************************/
/*                                                                 */
/*  LoadTrueTypeChar:                                              */
/*                                                                 */
/*    Loads a glyph into memory.                                   */
/*                                                                 */
/*******************************************************************/

  TT_Error  LoadTrueTypeChar( int  idx )
  {
    error = TT_Load_Glyph( instance, glyph, idx, TTLOAD_DEFAULT );
    if ( error )
      return error;

    TT_Get_Glyph_Outline( glyph, &outline );

    outline.second_pass    = 0;
    outline.high_precision = 0;
    outline.dropout_mode   = 0;

    /* debugging */
#if 0
    if ( idx == 0 && !visual )
    {
      printf( "points = %d\n", outline.points );
      for ( j = 0; j < outline.points; j++ )
        printf( "%02x  (%01hx,%01hx)\n",
                 j, outline.xCoord[j], outline.yCoord[j] );
      printf( "\n" );
    }
#endif

    /* create a new outline */
    TT_New_Outline( outline.n_points,
                    outline.n_contours,
                    &outlines[cur_glyph] );

    /* copy the glyph outline into it */
    outline.high_precision = 0;
    outline.second_pass    = 0;
    TT_Copy_Outline( &outline, &outlines[cur_glyph] );

    /* translate it */
    TT_Translate_Outline( &outlines[cur_glyph],
                          vio_Width * 16,
                          vio_Height * 16 );
    cur_glyph++;

    return TT_Err_Ok;
  }


/*******************************************************************/
/*                                                                 */
/*  ConvertRaster:                                                 */
/*                                                                 */
/*    Performs scan conversion.                                    */
/*                                                                 */
/*******************************************************************/

  TT_Error  ConvertRaster( int  index )
  {
    outlines[index].second_pass    = 0;
    outlines[index].high_precision = 0;

    if ( gray_render )
      return TT_Get_Outline_Pixmap( engine, &outlines[index], &Bit );
    else
      return TT_Get_Outline_Bitmap( engine, &outlines[index], &Bit );
  }


  int  main( int  argc, char**  argv )
  {
    int    i, total, mode, base, rendered_glyphs;
    char   filename[128 + 4];
    char   alt_filename[128 + 4];
    char*  execname;

    long   t, t0, tz0;


    execname    = argv[0];

    gray_render = 0;
    visual      = 0;

    while ( argc > 1 && argv[1][0] == '-' )
    {
      switch ( argv[1][1] )
      {
      case 'g':
        gray_render = 1;
        break;

      case 'v':
        visual = 1;
        break;

      default:
        Panic( "Unknown argument '%s'!\n", argv[1] );
      }
      argc--;
      argv++;
    }

    if ( argc != 2 )
    {
      fprintf( stderr, "fttimer: simple performance timer -- part of the FreeType project\n" );
      fprintf( stderr,"-----------------------------------------------------------------\n\n" );
      fprintf( stderr,"Usage: %s [-g] [-v] fontname[.ttf|.ttc]\n\n", execname );
      fprintf( stderr,"  where '-g' asks for gray-levels rendering\n" );
      fprintf( stderr,"        '-v' displays while rendering (slower)\n" );

      exit( EXIT_FAILURE );
    }

    i = strlen( argv[1] );
    while ( i > 0 && argv[1][i] != '\\' )
    {
      if ( argv[1][i] == '.' )
        i = 0;
      i--;
    }

    filename[128] = '\0';
    alt_filename[128] = '\0';

    strncpy( filename, argv[1], 128 );
    strncpy( alt_filename, argv[1], 128 );

    if ( i >= 0 )
    {
      strncpy( filename + strlen( filename ), ".ttf", 4 );
      strncpy( alt_filename + strlen( alt_filename ), ".ttc", 4 );
    }

    /* Initialize engine */

    if ( (error = TT_Init_FreeType( &engine )) )
      Panic( "Error while initializing engine, code = 0x%x.\n", error );

    /* Load face */

    error = TT_Open_Face( engine, filename, &face );

    if ( error == TT_Err_Could_Not_Open_File )
    {
      strcpy( filename, alt_filename );
      error = TT_Open_Face( engine, alt_filename, &face );
    }

    if ( error == TT_Err_Could_Not_Open_File )
      Panic( "Could not find/open %s.\n", filename );
    else if ( error )
      Panic( "Error while opening %s, error code = 0x%x.\n",
             filename, error );

    /* get face properties and allocate preload arrays */

    TT_Get_Face_Properties( face, &properties );

    num_glyphs = properties.num_Glyphs;

    tab_glyphs = MAX_GLYPHS;
    if ( tab_glyphs > num_glyphs )
      tab_glyphs = num_glyphs;

    /* create glyph */

    error = TT_New_Glyph( face, &glyph );
    if ( error )
      Panic( "Could not create glyph container.\n" );

    /* create instance */

    error = TT_New_Instance( face, &instance );
    if ( error )
      Panic( "Could not create instance for %s.\n", filename );

    error = TT_Set_Instance_CharSize( instance, 400*64 );
    if ( error )
      Panic( "Could not reset instance for %s.\n", filename );

    if ( gray_render )
      mode = Graphics_Mode_Gray;
    else
      mode = Graphics_Mode_Mono;

    if ( visual )
    {
      if ( !SetGraphScreen( mode ) )
        Panic( "Could not set graphics mode.\n" );
      TT_Set_Raster_Gray_Palette( engine, gray_palette );
    }
    else
    {
      /* This is the default bitmap size used */
      vio_Width  = 640;
      vio_Height = 450;
    }

    Init_Engine();

    Num  = 0;
    Fail = 0;

    total = num_glyphs;
    base  = 0;

    rendered_glyphs = 0;

    t0 = 0;  /* Initial time */

    tz0 = Get_Time();

    while ( total > 0 )
    {
      /* First, preload 'tab_glyphs' in memory */

      cur_glyph   = 0;
      cur_point   = 0;
      cur_contour = 0;

      printf( "loading %d glyphs", tab_glyphs );

      for ( Num = 0; Num < tab_glyphs; Num++ )
      {
        error = LoadTrueTypeChar( base + Num );
        if ( error )
          Fail++;

        total--;
      }

      base += tab_glyphs;

      if ( tab_glyphs > total )
        tab_glyphs = total;

      printf( ", rendering... " );

      /* Now, render the loaded glyphs */

      t = Get_Time();

      for ( Num = 0; Num < cur_glyph; Num++ )
      {
        if ( (error = ConvertRaster( Num )) )
          Fail++;
        else
        {
          rendered_glyphs ++;

          if ( visual )
          {
            sprintf( Header, "Glyph: %5d", Num );
            Display_Bitmap_On_Screen( Bit.bitmap, Bit.rows, Bit.cols );

            Clear_Buffer();
          }
        }
      }

      t = Get_Time() - t;
      if ( t < 0 )
        t += 100L * 60 * 60;

      printf( " = %f s\n", (double)t / 1000 );
      t0 += t;

      /* Now free all loaded outlines */
      for ( Num = 0; Num < cur_glyph; Num++ )
        TT_Done_Outline( &outlines[Num] );
    }

    tz0 = Get_Time() - tz0;

    if ( visual )
      RestoreScreen();

    TT_Close_Face( face );

    printf( "\n" );
    printf( "rendered glyphs  = %d\n", rendered_glyphs );
    printf( "render time      = %f s\n", (double)t0 / 1000 );
    printf( "fails            = %d\n", Fail );
    printf( "average glyphs/s = %f\n",
             (double)rendered_glyphs / t0 * 1000 );

    printf( "total timing     = %f s\n", (double)tz0 / 1000 );
    printf( "Fails = %d\n", Fail );

    TT_Done_FreeType( engine );

    exit( EXIT_SUCCESS );      /* for safety reasons */

    return 0;       /* never reached */
  }


/* End */
