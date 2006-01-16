/****************************************************************************/
/*                                                                          */
/*  The FreeType project -- a free and portable quality TrueType renderer.  */
/*                                                                          */
/*  Copyright 1996-2001 by                                                  */
/*  D. Turner, R.Wilhelm, and W. Lemberg                                    */
/*                                                                          */
/*  ftview:  A simple font viewer.  Supports hinting and grayscaling.       */
/*                                                                          */
/*                                                                          */
/*  Keys:                                                                   */
/*                                                                          */
/*  x :   fine counter-clockwise rotation                                   */
/*  c :   fine clockwise rotation                                           */
/*                                                                          */
/*  v :   fast counter-clockwise rotation                                   */
/*  b :   fast clockwise rotation                                           */
/*                                                                          */
/*  + :   fast scale up                                                     */
/*  - :   fast scale down                                                   */
/*  u :   fine scale down                                                   */
/*  j :   fine scale up                                                     */
/*                                                                          */
/*  l :   go to next glyph                                                  */
/*  k :   go to previous glyph                                              */
/*                                                                          */
/*  o :   go to tenth next glyph                                            */
/*  i :   go to tenth previous glyph                                        */
/*                                                                          */
/*  0 :   go to hundredth next glyph                                        */
/*  9 :   go to hundredth previous glyph                                    */
/*                                                                          */
/*  ) :   go to 1000th next glyph                                           */
/*  ( :   go to 1000th previous glyph                                       */
/*                                                                          */
/*  } :   go to 10000th next glyph                                          */
/*  { :   go to 10000th previous glyph                                      */
/*                                                                          */
/*  n :   go to next (or last) .ttf file                                    */
/*  p :   go to previous (or first) .ttf file                               */
/*                                                                          */
/*  h :   toggle hinting                                                    */
/*                                                                          */
/*  B :   toggle sbit                                                       */
/*                                                                          */
/*  ESC :   exit                                                            */
/*                                                                          */
/*                                                                          */
/*  NOTE: This is just a test program that is used to show off and          */
/*        debug the current engine.                                         */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "blitter.h"
#include "common.h"
#include "display.h"
#include "freetype.h"
#include "ftxsbit.h"

#include "gdriver.h"
#include "gevents.h"
#include "gmain.h"

#define  Pi         3.1415926535

#define  MAXPTSIZE  500                 /* dtp */
#define  Center_X   ( Bit.width / 2 )   /* dtp */
#define  Center_Y   ( Bit.rows  / 2 )   /* dtp */

/*
 * If Ignore_Err_Not_SBit is not defined,
 * "Fail" increase when the glyph does not have sbit and
 * sbit_flag is 1.
 */
#define  Ignore_Err_Not_SBit 1

  TT_Engine             engine;

  TT_Face               face;
  TT_Instance           instance;
  TT_Glyph              glyph;

  TT_Big_Glyph_Metrics  metrics;
  TT_Outline            outline;
  TT_Face_Properties    properties;
  TT_Instance_Metrics   imetrics;

  TT_SBit_Image*        sbit;

  int            num_glyphs;

  int            ptsize;
  int            hinted;

  char           Header[128];

  int            Rotation;
  int            Fail;
  int            Num;

  int            gray_render;
  unsigned int   sbit_flag;     /* 0 if to display outlines
				 * 1 if to use embedded bitmaps
				 * 2 if to show sbit and outline glyphs
				 */

  int            glyph_has_sbit;



  static TT_Error  Reset_Scale( int  pointSize )
  {
    TT_Error  error;
    TT_SBit_Strike strike;


    if ( (error = TT_Set_Instance_PointSize( instance, pointSize )) )
    {
      RestoreScreen();
      fprintf( stderr, "Error = 0x%x.\n", (unsigned)error );
      Panic( "Could not reset instance.\n" );
    }
    TT_Get_Instance_Metrics( instance, &imetrics );

    /* now re-allocates the small bitmap */
    if ( gray_render )
    {
      Init_Small( imetrics.x_ppem, imetrics.y_ppem );
      Clear_Small();
    }

    if ( TT_Get_SBit_Strike( face, instance, &strike ) )
      glyph_has_sbit = 0;
    else
      glyph_has_sbit = 1;

    return TT_Err_Ok;
  }


  static TT_Error  LoadTrueTypeChar( int  idx, int  hint )
  {
    int  flags;


    flags = TTLOAD_SCALE_GLYPH;
    if ( hint )
      flags |= TTLOAD_HINT_GLYPH;

    return TT_Load_Glyph( instance, glyph, idx, flags );
  }


  static TT_Error  Render_All( int  first_glyph, int  ptsize )
  {
    TT_F26Dot6  start_x, start_y, step_y, x, y;
    int         i;

    TT_Error    error = 0;


    start_x = 4;
    start_y = vio_Height - ( ( ptsize * 96 + 36 ) / 72 + 10 );

    step_y = imetrics.y_ppem + 10;

    x = start_x;
    y = start_y;

    i = first_glyph;

    while ( i < num_glyphs )
    {
      if ( ( glyph_has_sbit ) &&
           ( ( sbit_flag + 1 ) & 2 )                                  &&
           !(error = TT_Load_Glyph_Bitmap( face, instance, i, sbit )) )
      {
        Blit_Bitmap( &Bit,
                     &sbit->map,
                     gray_render ? 8 : 1,
		     (int)x + (int)(sbit->metrics.horiBearingX/64),
		     Bit.rows - (int)y - (int)(sbit->metrics.horiBearingY/64),
		     gray_palette[4] );
        metrics = sbit->metrics;
        goto Step;
      }

      if ( ( ( !glyph_has_sbit ) ||
             ( ( sbit_flag + 1 ) & 1 ) )                &&
           !(error = LoadTrueTypeChar( i, hinted )) )
      {
        TT_Get_Glyph_Outline( glyph, &outline );
        TT_Get_Glyph_Big_Metrics( glyph, &metrics );

	Render_Single_Glyph( gray_render, glyph, (int)x, (int)y );
      }
      else
      {
#ifdef  Ignore_Err_Not_SBit
        if (!(( glyph_has_sbit ) &&
             ( sbit_flag == 1 ) &&
             ( error == TT_Err_Invalid_Glyph_Index )))
#endif
        Fail++;
      }
    Step:
      x += ( metrics.horiAdvance / 64 ) + 1;

      if ( x + imetrics.x_ppem > vio_Width )
      {
        x  = start_x;
        y -= step_y;

        if ( y < 10 )
            return TT_Err_Ok;
      }
      i++;
    }
    return TT_Err_Ok;
  }


  static int  Process_Event( TEvent*  event )
  {
    switch ( event->what )
    {
    case event_Quit:                                      /* ESC or q */
      return 0;

    case event_Keyboard:
      if ( event->info == 'n' )                           /* Next file */
        return 'n';
      if ( event->info == 'p' )                           /* Previous file */
        return 'p';
      if ( event->info == 'h' )                           /* Toggle hinting */
        hinted = !hinted;
      if ( ( event->info == 'B' ) && ( glyph_has_sbit ) ) /* Toggle sbit */
        sbit_flag = ( sbit_flag + 1 ) % 3;
      break;

    case event_Rotate_Glyph:
      Rotation = ( Rotation + event->info ) & 1023;
      break;

    case event_Scale_Glyph:
      ptsize += event->info;
      if ( ptsize < 1 )         ptsize = 1;
      if ( ptsize > MAXPTSIZE ) ptsize = MAXPTSIZE;
      break;

    case event_Change_Glyph:
      Num += event->info;
      if ( Num < 0 )           Num = 0;
      if ( Num >= num_glyphs ) Num = num_glyphs - 1;
      break;
    }

    return 1;
  }


  static void  usage( char*  execname )
  {
    fprintf( stderr,
          "\n"
          "ftview: simple TrueType interpreter tester -- part of the FreeType project\n"
          "--------------------------------------------------------------------------\n"
          "\n"
          "Usage: %s [options below] ppem fontname[.ttf|.ttc] ...\n",
          execname );
    fprintf( stderr,
          "\n"
          "  -g     gray-level rendering (default: none)\n"
          "  -r R   use resolution R dpi (default: 96)\n"
          "  -B     use embedded bitmaps (default: none)\n"
          "\n" );

    exit( EXIT_FAILURE );
  }


  int  main( int  argc, char**  argv )
  {
    int    i, old_ptsize, orig_ptsize, file;
    int    XisSetup = 0;
    char   filename[128 + 4];
    char   alt_filename[128 + 4];
    char*  execname;
    int    option;
    int    res = 96;

    TT_Error  error;
    TEvent    event;

    TT_EBLC   eblc_table;


    execname = ft_basename( argv[0] );

    while ( 1 )
    {
      option = ft_getopt( argc, argv, "gr:B" );

      if ( option == -1 )
        break;

      switch ( option )
      {
      case 'g':
        gray_render = 1;
        sbit_flag   = 0;
        break;

      case 'r':
        res = atoi( ft_optarg );
        if ( res < 1 )
          usage( execname );
        break;

      case 'B':
        if (!gray_render)
#if 0
          sbit_flag  = 1;
#else
          sbit_flag  = 2;
#endif
        break;

      default:
        usage( execname );
        break;
      }
    }

    argc -= ft_optind;
    argv += ft_optind;

    if ( argc <= 1 )
      usage( execname );

    if ( sscanf( argv[0], "%d", &orig_ptsize ) != 1 )
      orig_ptsize = 64;

    file = 1;

    /* Initialize engine */

    if ( (error = TT_Init_FreeType( &engine )) )
      Panic( "Error while initializing engine, code = 0x%x.\n", error );

    if ( (error = TT_Init_SBit_Extension( engine )) )
      Panic( "Error while initializing sbit extention, code = 0x%x.\n", error );

  NewFile:
    ptsize = orig_ptsize;
    hinted = 1;

    i = strlen( argv[file] );
    while ( i > 0 && argv[file][i] != '\\' && argv[file][i] != '/' )
    {
      if ( argv[file][i] == '.' )
        i = 0;
      i--;
    }

    filename[128]     = '\0';
    alt_filename[128] = '\0';

    strncpy( filename, argv[file], 128 );
    strncpy( alt_filename, argv[file], 128 );

    if ( i >= 0 )
    {
      strncpy( filename + strlen( filename ), ".ttf", 4 );
      strncpy( alt_filename + strlen( alt_filename ), ".ttc", 4 );
    }

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

    glyph_has_sbit = 0;

    error = TT_Get_Face_Bitmaps( face, &eblc_table );
    if ( error == TT_Err_Ok )
      glyph_has_sbit = 1;
    if ( sbit_flag && !glyph_has_sbit )
      Panic( "%s does not have embedded bitmap glyphs.\n", filename );

    /* create glyph */

    error = TT_New_Glyph( face, &glyph );
    if ( error )
      Panic( "Could not create glyph container.\n" );

    error = TT_New_SBit_Image( &sbit );
    if ( error )
      Panic( "Could not create sbit slot.\n" );

    /* create instance */

    error = TT_New_Instance( face, &instance );
    if ( error )
      Panic( "Could not create instance for %s.\n", filename );

    error = TT_Set_Instance_Resolutions( instance, res, res );
    if ( error )
      Panic( "Could not set device resolutions." );

    if ( !XisSetup )
    {
      XisSetup = 1;

      if ( gray_render )
      {
        if ( !SetGraphScreen( Graphics_Mode_Gray ) )
          Panic( "Could not set up grayscale graphics mode.\n" );

        TT_Set_Raster_Gray_Palette( engine, virtual_palette );
      }
      else
      {
        if ( !SetGraphScreen( Graphics_Mode_Mono ) )
          Panic( "Could not set up mono graphics mode.\n" );
      }
    }

    Init_Display( gray_render );

    Reset_Scale( ptsize );

    old_ptsize = ptsize;

    Fail = 0;
    Num  = 0;

    for ( ;; )
    {
      int   key;
      char* sbit_string;
      static char* sbit_messages[4] =
        { "off", "on", " if available", "NONE" };

      sbit_string = sbit_messages[3];
      if ( glyph_has_sbit )
      {
        if (sbit_flag >= 0 && sbit_flag < 3)
          sbit_string = sbit_messages[sbit_flag];
      }

      Clear_Display();
      Render_All( Num, ptsize );
      if ( gray_render )
        Convert_To_Display_Palette();

      sprintf( Header, "%s:   Glyph: %4d  ptsize: %4d  hinting: %s  sbit: %s",
                       ft_basename( filename ), Num, ptsize,
                       hinted ? "on" : "off",
                       sbit_string );
      Display_Bitmap_On_Screen( Bit.bitmap, Bit.rows, Bit.cols );

#ifndef X11
#ifndef OS2
      Print_XY( 0, 0, Header );
#endif
#endif

      Get_Event( &event );
      if ( !( key = Process_Event( &event ) ) )
        goto End;

      if ( key == 'n' )
      {
        TT_Close_Face( face );

        if ( file < argc - 1 )
          file++;

        goto NewFile;
      }

      if ( key == 'p' )
      {
        TT_Close_Face( face );

        if ( file > 1 )
          file--;

        goto NewFile;
      }

      if ( ptsize != old_ptsize )
      {
        if ( Reset_Scale( ptsize ) )
          Panic( "Could not resize font.\n" );

        old_ptsize = ptsize;
      }
    }

  End:
    RestoreScreen();

    TT_Done_SBit_Image( sbit );
    TT_Close_Face( face );
    TT_Done_FreeType( engine );

    printf( "Execution completed successfully.\n" );
    printf( "Fails = %d\n", Fail );

    exit( EXIT_SUCCESS );      /* for safety reasons */

    return 0;       /* never reached */
}


/* End */
