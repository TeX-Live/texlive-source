/****************************************************************************/
/*                                                                          */
/*  The FreeType project -- a free and portable quality TrueType renderer.  */
/*                                                                          */
/*  Copyright 1999-2001 by                                                  */
/*  Yamano'uchi H. and W. Lemberg                                           */
/*                                                                          */
/*  ftmetric: dump metrics and a glyph.                                     */
/*                                                                          */
/*  NOTE:  This is just a test program that is used to debug                */
/*         the current engine.                                              */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "freetype.h"
#include "ftxsbit.h"

#include "textdisp.h"

/*
 *  Basically, an external program using FreeType shouldn't depend on an
 *  internal file of the FreeType library, especially not on ft_conf.h -- but
 *  to avoid another configure script which tests for the existence of the
 *  i18n stuff we include ft_conf.h here since we can be sure that our test
 *  programs use the same configuration options as the library itself.
 */

#include "ft_conf.h"


#ifdef HAVE_LIBINTL_H

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#include <libintl.h>
#include "ftxerr18.h"

#else /* !HAVE_LIBINTL_H */

#define gettext( x )  ( x )

  /* We ignore error message strings with this function */

  static char*
  TT_ErrToString18( TT_Error  error )
  {
    static char  temp[32];


    sprintf( temp, "0x%04lx", error );
    return temp;
  }

#endif /* !HAVE_LIBINTL_H */

  static void
  usage( char*  execname )
  {
    char*  gt;


    fprintf( stderr, "\n" );
    gt = gettext( "ftmetric: Simple TTF metrics/glyph dumper -- part of the FreeType project" );
    fprintf( stderr, "%s\n", gt );
    separator_line( stderr, strlen( gt ) );
    fprintf( stderr, gettext(
             "Usage: %s [options below] point fontname[.ttf|.ttc]\n"
             "\n"
             "  -B        show sbit's metrics (default: none)\n"
             "  -c C      use C'th font index of TrueType collection  (default: 0)\n"
             "  -i index  glyph index (default: 0)\n"
             "  -r R      use resolution R dpi (default: 72)\n"
             "\n" ), execname );

    exit( EXIT_FAILURE );
  }


  static void
  Show_Metrics( TT_Big_Glyph_Metrics  metrics,
                char*                 title )
  {
    int  show_advance = 1;


    printf("%s: xMin %d, xMax %d, yMin %d, yMax %d",
           title,
           (int)(metrics.bbox.xMin / 64),
           (int)(metrics.bbox.xMax / 64),
           (int)(metrics.bbox.yMin / 64),
           (int)(metrics.bbox.yMax / 64));


    if ( show_advance )
      printf( ", advance width %d", (int)(metrics.horiAdvance / 64) );

    printf("\n");
    return;
  }


  int
  main( int  argc, char**  argv )
  {
    int    i, orig_ptsize, file;
    char   filename[128 + 4];
    char   alt_filename[128 + 4];
    char*  execname;
    int    option;

    int    ptsize;
    int    num_Faces = 0;
    int    glyph_index = 0;
    int    load_flags = TTLOAD_DEFAULT;

    int    force_sbit = 0;

    TT_Engine    engine;

    TT_Face      face;
    TT_Instance  instance;
    TT_Glyph     glyph;

    TT_Raster_Map  map;

    TT_Big_Glyph_Metrics  metrics;
    TT_Face_Properties    properties;
    TT_Instance_Metrics   imetrics;

    TT_EBLC         eblc;
    TT_SBit_Image*  bitmap = NULL;

    TT_Error  error;


    int res = 72;

#ifdef HAVE_LIBINTL_H
    setlocale( LC_ALL, "" );
    bindtextdomain( "freetype", LOCALEDIR );
    textdomain( "freetype" );
#endif

    execname = ft_basename( argv[0] );

    while ( 1 )
    {
      option = ft_getopt( argc, argv, "c:r:i:B" );

      if ( option == -1 )
        break;

      switch ( option )
      {
      case 'r':
        res = atoi( ft_optarg );
        if ( res < 1 )
          usage( execname );
        break;

      case 'c':
        num_Faces = atoi( ft_optarg );
        if ( num_Faces < 0 )
          usage( execname );
        break;

      case 'i':
        glyph_index = atoi( ft_optarg );
        if ( glyph_index < 0 )
          usage( execname );
        break;

      case 'B':
        force_sbit = 1;
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

    ptsize = orig_ptsize;

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

    /* Initialize engine */

    error = TT_Init_FreeType( &engine );
    if ( error )
    {
      fprintf( stderr, gettext( "Error while initializing engine.\n" ) );
      goto Failure;
    }

    error = TT_Init_SBit_Extension( engine );
    if ( error )
    {
      fprintf( stderr, gettext(
               "Error while initializing embedded bitmap extension.\n" ) );
      goto Failure;
    }

    /* Load face */

    error = TT_Open_Face( engine, filename, &face );

    if ( error == TT_Err_Could_Not_Open_File )
    {
      strcpy( filename, alt_filename );
      error = TT_Open_Face( engine, alt_filename, &face );
    }

    if ( error == TT_Err_Could_Not_Open_File )
      Panic( gettext( "Could not find or open %s.\n" ), filename );
    if ( error )
    {
      fprintf( stderr, gettext( "Error while opening %s.\n" ),
               filename );
      goto Failure;
    }

    TT_Get_Face_Properties( face, &properties );

    printf( gettext( "There are %d fonts in this collection.\n" ),
            (int)(properties.num_Faces) );

    if ( num_Faces >= properties.num_Faces )
      Panic( gettext(
             "There is no collection with index %d in this font file.\n" ),
             num_Faces );

    TT_Close_Face( face );

    error = TT_Open_Collection( engine, filename, num_Faces, &face );

    /* get face properties and eblc */

    TT_Get_Face_Properties( face, &properties );
    if ( force_sbit )
    {
      error = TT_Get_Face_Bitmaps( face, &eblc );
      if ( error == TT_Err_Table_Missing )
        Panic( gettext( "There is no embedded bitmap data in the font.\n" ) );
      if ( error )
      {
        fprintf( stderr, gettext(
                 "Error while retrieving embedded bitmaps table.\n" ) );
        goto Failure;
      }
    }

    /* create glyph */

    error = TT_New_Glyph( face, &glyph );
    if ( error )
    {
      fprintf( stderr, gettext( "Could not create glyph container.\n" ) );
      goto Failure;
    }

    /* create instance */

    error = TT_New_Instance( face, &instance );
    if ( error )
    {
      fprintf( stderr, gettext( "Could not create instance.\n" ) );
      goto Failure;
    }

    error = TT_Set_Instance_Resolutions( instance, res, res );
    if ( error )
    {
      fprintf( stderr, gettext( "Could not set device resolutions.\n" ) );
      goto Failure;
    }

    error = TT_Set_Instance_CharSize( instance, ptsize*64 );
    if ( error )
    {
      fprintf( stderr, gettext( "Could not reset instance.\n" ) );
      goto Failure;
    }

    TT_Get_Instance_Metrics( instance, &imetrics );

    printf( gettext( "Instance metrics: ppemX %d, ppemY %d\n" ),
            imetrics.x_ppem,
            imetrics.y_ppem );

    if ( force_sbit )
    {
      error = TT_New_SBit_Image( &bitmap );
      if ( error )
      {
        fprintf( stderr, gettext(
                 "Could not allocate glyph bitmap container.\n" ) );
        goto Failure;
      }

      error = TT_Load_Glyph_Bitmap( face, instance, glyph_index, bitmap );
      if ( error )
      {
        fprintf( stderr, gettext(
                 "Can't load bitmap for glyph %d.\n" ), glyph_index );
        goto Failure;
      }

      Show_Metrics( bitmap->metrics, "SBit's metrics" );

      printf( "SBit glyph:\n" );
      Show_Single_Glyph( &bitmap->map );
    }
    else
    {
      TT_Load_Glyph( instance, glyph, glyph_index, load_flags );
      TT_Get_Glyph_Big_Metrics( glyph, &metrics );

      map.width = ( metrics.bbox.xMax - metrics.bbox.xMin ) / 64;
      map.rows = ( metrics.bbox.yMax - metrics.bbox.yMin ) / 64;
      map.cols = ( map.width + 7 ) / 8;
      map.size = map.cols * map.rows;
      map.bitmap = malloc( map.size );
      map.flow = TT_Flow_Down;

      memset( map.bitmap, 0, map.size );

      error = TT_Get_Glyph_Bitmap( glyph, &map,
                                   -metrics.bbox.xMin,
                                   -metrics.bbox.yMin );

      Show_Metrics( metrics, gettext( "Outline's metrics" ) );

      printf( gettext( "Outline glyph\n" ) );
      Show_Single_Glyph( &map );
    }

    free( map.bitmap );

    if ( bitmap )
      TT_Done_SBit_Image( bitmap );

    TT_Done_Instance( instance );
    TT_Done_Glyph( glyph );
    TT_Close_Face( face );
    TT_Done_FreeType( engine );

    exit( EXIT_SUCCESS );      /* for safety reasons */

    return 0;       /* never reached */

  Failure:
    fprintf( stderr, "  " );
    fprintf( stderr, gettext( "FreeType error message: %s\n" ),
             TT_ErrToString18( error ) );

    exit( EXIT_FAILURE );

    return 0;       /* never reached */
  }


/* End */
