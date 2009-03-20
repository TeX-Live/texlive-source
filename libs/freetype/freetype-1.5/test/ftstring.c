/****************************************************************************/
/*                                                                          */
/*  The FreeType project -- a free and portable quality TrueType renderer.  */
/*                                                                          */
/*  Copyright 1996-2001 by                                                  */
/*  D. Turner, R.Wilhelm, and W. Lemberg                                    */
/*                                                                          */
/*  ftstring: Making string text from individual glyph information.         */
/*                                                                          */
/*  Keys:                                                                   */
/*                                                                          */
/*  + :   fast scale up                                                     */
/*  - :   fast scale down                                                   */
/*  u :   fine scale down                                                   */
/*  j :   fine scale up                                                     */
/*                                                                          */
/*  h :   toggle hinting                                                    */
/*                                                                          */
/*  ESC :   exit                                                            */
/*                                                                          */
/*                                                                          */
/*  NOTE:  This is just a test program that is used to show off and         */
/*         debug the current engine.                                        */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "display.h"
#include "freetype.h"

#include "gevents.h"
#include "gdriver.h"
#include "gmain.h"

#define  Pi         3.1415926535

#define  MAXPTSIZE  500                 /* dtp */
#define  Center_X   ( Bit.width / 2 )   /* dtp */
#define  Center_Y   ( Bit.rows  / 2 )   /* dtp */

  TT_Engine            engine;

  TT_Face              face;
  TT_Instance          instance;
  TT_Glyph             glyph;
  TT_CharMap           char_map;

  TT_Glyph_Metrics     metrics;
  TT_Face_Properties   properties;
  TT_Instance_Metrics  imetrics;

  int            num_glyphs;

  int            ptsize;
  int            hinted;

  char           Header[128];
  int            Fail;
  int            Num;

  int            gray_render;

  short          glyph_code[128];
  int            num_codes;


/* Convert an ASCII string to a string of glyph indexes.              */
/*                                                                    */
/* IMPORTANT NOTE:                                                    */
/*                                                                    */
/* There is no portable way to convert from any system's char. code   */
/* to Unicode.  This function simply takes a char. string as argument */
/* and "interprets" each character as a Unicode char. index with no   */
/* further check.                                                     */
/*                                                                    */
/* This mapping is only valid for the ASCII character set (i.e.,      */
/* codes 32 to 127); all other codes (like accentuated characters)    */
/* will produce more or less random results, depending on the system  */
/* being run.                                                         */

  static void  CharToUnicode( char*  source )
  {
    unsigned short  i, n;
    unsigned short  platform, encoding;

    /* First, look for a Unicode charmap */

    n = properties.num_CharMaps;

    for ( i = 0; i < n; i++ )
    {
      TT_Get_CharMap_ID( face, i, &platform, &encoding );
      if ( (platform == 3 && encoding == 1 )  ||
           (platform == 0 && encoding == 0 ) )
      {
        TT_Get_CharMap( face, i, &char_map );
        i = n + 1;
      }
    }

    if ( i == n )
      Panic( "Sorry, but this font doesn't contain any Unicode mapping table\n" );

    for ( n = 0; n < 128 && source[n]; n++ )
#if 1
      glyph_code[n] = TT_Char_Index( char_map, (short)source[n] );
#else
    /* Note, if you have a function, say ToUnicode(), to convert from     */
    /* char codes to Unicode, use the following line instead:             */

      glyph_code[n] = TT_Char_Index( char_map, ToUnicode( source[n] ) );
#endif

    num_codes = n;
  }


  static TT_Error  Reset_Scale( int  pointSize )
  {
    TT_Error  error;


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


  static TT_Error  Render_All( void )
  {
    TT_F26Dot6  x, y, z, minx, miny, maxx, maxy;
    int         i;

    TT_Error    error;


    /* On the first pass, we compute the compound bounding box */

    x = y = 0;

    minx = miny = maxx = maxy = 0;

    for ( i = 0; i < num_codes; i++ )
    {
      if ( !(error = LoadTrueTypeChar( glyph_code[i], hinted )) )
      {
        TT_Get_Glyph_Metrics( glyph, &metrics );

        z = x + metrics.bbox.xMin;
        if ( minx > z )
          minx = z;

        z = x + metrics.bbox.xMax;
        if ( maxx < z )
          maxx = z;

        z = y + metrics.bbox.yMin;
        if ( miny > z )
          miny = z;

        z = y + metrics.bbox.yMax;
        if ( maxy < z )
          maxy = z;

        x += metrics.advance & -64;
      }
      else
        Fail++;
    }

    /* We now center the bbox inside the target bitmap */

    minx = ( minx & -64 ) >> 6;
    miny = ( miny & -64 ) >> 6;

    maxx = ( (maxx+63) & -64 ) >> 6;
    maxy = ( (maxy+63) & -64 ) >> 6;

    maxx -= minx;
    maxy -= miny;

    minx = (Bit.width - maxx)/2;
    miny = (Bit.rows  + miny)/2;

    maxx += minx;
    maxy += maxy;

    /* On the second pass, we render each glyph to its centered position. */
    /* This is slow, because we reload each glyph to render it!           */

    x = minx;
    y = miny;

    for ( i = 0; i < num_codes; i++ )
    {
      if ( !(error = LoadTrueTypeChar( glyph_code[i], hinted )) )
      {
        TT_Get_Glyph_Metrics( glyph, &metrics );

	Render_Single_Glyph( gray_render, glyph, (int)x, (int)y );

        x += metrics.advance/64;
      }
    }

    return TT_Err_Ok;
  }


  static int  Process_Event( TEvent*  event )
  {
    switch ( event->what )
    {
    case event_Quit:            /* ESC or q */
      return 0;

    case event_Keyboard:
      if ( event->info == 'h' ) /* Toggle hinting */
        hinted = !hinted;
      break;

    case event_Rotate_Glyph:
      break;

    case event_Scale_Glyph:
      ptsize += event->info;
      if ( ptsize < 1 )         ptsize = 1;
      if ( ptsize > MAXPTSIZE ) ptsize = MAXPTSIZE;
      break;

    case event_Change_Glyph:
      break;
    }

    return 1;
  }


  static void  usage( char*  execname )
  {
    fprintf( stderr,
          "\n"
          "ftstring: simple String Test Display -- part of the FreeType project\n"
          "--------------------------------------------------------------------\n"
          "\n"
          "Usage: %s [options below] ppem fontname[.ttf|.ttc] [string]\n",
          execname );
    fprintf( stderr,
          "\n"
          "  -g     gray-level rendering (default: none)\n"
          "  -r R   use resolution R dpi (default: 96)\n"
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


    execname = ft_basename( argv[0] );

    while ( 1 )
    {
      option = ft_getopt( argc, argv, "gr:" );

      if ( option == -1 )
        break;

      switch ( option )
      {
      case 'g':
        gray_render = 1;
        break;

      case 'r':
        res = atoi( ft_optarg );
        if ( res < 1 )
          usage( execname );
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

    ptsize = orig_ptsize;
    hinted = 1;

    i = strlen( argv[file] );
    while ( i > 0 && argv[file][i] != '\\' && argv[file][i] != '/' )
    {
      if ( argv[file][i] == '.' )
        i = 0;
        i--;
    }

    filename[128] = '\0';
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
    else if (error)
      Panic( "Error while opening %s, error code = 0x%x.\n",
             filename, error );

    /* get face properties and allocate preload arrays */

    TT_Get_Face_Properties( face, &properties );

    num_glyphs = properties.num_Glyphs;

    /* create glyph */

    error = TT_New_Glyph( face, &glyph );
    if ( error )
      Panic( "Could not create glyph container.\n" );

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

    CharToUnicode( ( argv[2] ? argv[2] :
                     "The quick brown fox jumps over the lazy dog" ) );

    for ( ;; )
    {
      Clear_Display();
      Render_All();
      if ( gray_render )
        Convert_To_Display_Palette();

      sprintf( Header, "%s:   ptsize: %4d  hinting: %s",
                       ft_basename( filename ), ptsize,
                       hinted ? "on" : "off" );

      Display_Bitmap_On_Screen( Bit.bitmap, Bit.rows, Bit.cols );

#ifndef X11
#ifndef OS2
      Print_XY( 0, 0, Header );
#endif
#endif

      Get_Event( &event );
      if ( !( Process_Event( &event ) ) )
        goto End;

      if ( ptsize != old_ptsize )
      {
        if ( Reset_Scale( ptsize ) )
          Panic( "Could not resize font.\n" );

        old_ptsize = ptsize;
      }
    }

  End:
    RestoreScreen();

    TT_Close_Face( face );
    TT_Done_FreeType( engine );

    printf( "Execution completed successfully.\n" );
    printf( "Fails = %d.\n", Fail );

    exit( EXIT_SUCCESS );      /* for safety reasons */

    return 0;       /* never reached */
}


/* End */
