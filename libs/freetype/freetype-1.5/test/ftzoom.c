/****************************************************************************/
/*                                                                          */
/*  The FreeType project - a free and portable quality TrueType renderer.   */
/*                                                                          */
/*  Copyright 1996-2001 by                                                  */
/*  D. Turner, R.Wilhelm, and W. Lemberg                                    */
/*                                                                          */
/*  ftzoom : A simple glyph viewer.  Now supports graylevel rendering       */
/*           with the '-g' option.                                          */
/*                                                                          */
/*           Use `-p <platformID>' together with `-e <encodingID>' to       */
/*           select a cmap.                                                 */
/*                                                                          */
/*  Keys:                                                                   */
/*                                                                          */
/*  x :   fine counter_clockwise rotation                                   */
/*  c :   fine clockwise rotation                                           */
/*                                                                          */
/*  v :   fast counter_clockwise rotation                                   */
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
/*    q :                                                                   */
/*  ESC :   exit                                                            */
/*                                                                          */
/*                                                                          */
/*  NOTE 1: This is just a test program that is used to show off and        */
/*          debug the current engine.  In no way does it show the final     */
/*          high-level interface that client applications will use.         */
/*                                                                          */
/*  NOTE 2: The `post' engine is used to display the PS glyph names.        */
/*          Use the `-n' switch if you don't want that.                     */
/*                                                                          */
/****************************************************************************/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "freetype.h"
#include "ftxpost.h"

#include "gdriver.h"
#include "gevents.h"
#include "gmain.h"

#ifdef DEBUG_LEVEL_TRACE
#include "ttdebug.h"
#endif


#define  Pi         3.1415926535

#define  MAXPTSIZE  5000                /* dtp */

  char  Header[128];

  TT_Engine    engine;
  TT_Face      face;
  TT_Instance  instance;
  TT_Glyph     glyph;
  TT_CharMap   char_map;

  TT_Glyph_Metrics     metrics;
  TT_Outline           outline;
  TT_Face_Properties   properties;
  TT_Instance_Metrics  imetrics;

  int  num_glyphs;
  int  xcenter_upem;
  int  ycenter_upem;
  int  units_per_em;

  int  ptsize;
  int  old_ptsize;
  int  rotation;
  int  old_rotation;

  TT_Matrix  matrix;
  TT_Matrix  zoom_matrix;
  int        apply_matrix;
  int        xcenter;
  int        ycenter;
  int        xoffset;
  int        yoffset;

  extern TT_Raster_Map  Bit;

  int  Fail;
  int  Num;
  int  Code;

  int  gray_render = 0;
  int  hinted      = 1;
  int  use_cmap    = 0;
  int  zoom_factor = 1;
  int  grid        = 0;
  int  use_post    = 1;

  char  palette[5] = { 0, 1, 2, 3, 4 };


  static void  ClearData( void )
  {
    if ( gray_render )
      memset( Bit.bitmap, gray_palette[0], Bit.size );
    else
      memset( Bit.bitmap, 0, Bit.size );
  }


  void  Init_Raster_Area( void )
  {
    Bit.rows  = vio_Height;   /* The whole window */
    Bit.width = vio_Width;
    Bit.flow  = TT_Flow_Up;

    if ( gray_render )
    {
      Bit.cols  = Bit.width;
      Bit.size  = Bit.rows * Bit.width;
    }
    else
    {
      Bit.cols  = ( Bit.width + 7 ) / 8;    /* convert to # of bytes     */
      Bit.size  = Bit.rows * Bit.cols;      /* number of bytes in buffer */
    }

    Bit.bitmap = (void*)malloc( (int)Bit.size );
    if ( !Bit.bitmap )
      Panic( "Not enough memory to allocate bitmap!\n" );

    ClearData();
  }


  static TT_Error  Reset_PtSize( int  pointSize )
  {
    TT_Error  error;


    if ( (error = TT_Set_Instance_PointSize( instance, pointSize )) )
    {
      RestoreScreen();
      printf( "Error = 0x%x.\n", (int)error );
      Panic( "Could not reset instance.\n" );
    }

    TT_Get_Instance_Metrics( instance, &imetrics );

    xcenter = imetrics.x_ppem * xcenter_upem / units_per_em / 4;
    ycenter = imetrics.y_ppem * ycenter_upem / units_per_em / 4;

    xoffset = vio_Width/2  - xcenter;
    yoffset = vio_Height/2 - ycenter;

    return TT_Err_Ok;
  }


  static TT_Error  Reset_Rotation( int  rotation )
  {
    if ( rotation )
    {
      float  angle;


      TT_Set_Instance_Transform_Flags( instance, 1, 0 );

      angle = rotation * Pi / 512;

      matrix.xx = (TT_Fixed)(cos( angle ) * (1L<<16));
      matrix.xy = (TT_Fixed)(sin( angle ) * (1L<<16));
      matrix.yx = -matrix.xy;
      matrix.yy = matrix.xx;

      apply_matrix = 1;
    }
    else
      apply_matrix = 0;

    if ( zoom_factor != 1 )
    {
      zoom_matrix.xx = zoom_matrix.yy = zoom_factor * (1L<<16);
      zoom_matrix.xy = zoom_matrix.yx = 0;
    }

    return TT_Err_Ok;
  }


  static TT_Error  LoadTrueTypeChar( int  idx, int  hint )
  {
    TT_Error  error;
    int       flags;


    flags = TTLOAD_SCALE_GLYPH;
    if ( hint )
      flags |= TTLOAD_HINT_GLYPH;

    error = TT_Load_Glyph( instance, glyph, idx, flags );
    if ( error )
      return error;

    if ( apply_matrix || zoom_factor != 1 )
    {
      TT_Get_Glyph_Outline( glyph, &outline );
      TT_Translate_Outline( &outline, -xcenter*64L, -ycenter*64L );
      if ( apply_matrix )
        TT_Transform_Outline( &outline, &matrix );
      if ( zoom_factor != 1 )
        TT_Transform_Outline( &outline, &zoom_matrix );
      TT_Translate_Outline( &outline, xcenter*64L, ycenter*64L );
    }

    return error;
  }


  static TT_Error  ConvertRaster( void )
  {
    if ( gray_render )
      return TT_Get_Glyph_Pixmap( glyph, &Bit, xoffset*64L, yoffset*64L );
    else
      return TT_Get_Glyph_Bitmap( glyph, &Bit, xoffset*64L, yoffset*64L );
  }


  static void  DrawGrid( void )
  {
    char *bmap, mask;
    int   x, y, shift, toggle;


    bmap = (char *)Bit.bitmap;

    for ( y = 0; y < Bit.rows; ++y )
    {
      if ( ( y - xoffset ) % zoom_factor == 0 )
      {
        if ( gray_render )
          for ( x = y & 1; x < Bit.cols; x += 2 )
            bmap[x] = 4 - bmap[x];
        else {
          mask = y & 1 ? 0x55 : 0xAA;
          for ( x = 0; x < Bit.cols; ++x )
            bmap[x] ^= mask;
        }
      }
      else
      {
        toggle = y & 1;

        if ( gray_render )
        {
          for ( x = xoffset % zoom_factor; x < Bit.cols; x += zoom_factor )
            if ( ( x & 1 ) == toggle )
              bmap[x] = 4 - bmap[x];
        }
        else
        {
          /* tricky business */
          shift = xoffset % zoom_factor;
          for ( x = 0; x < Bit.cols; ++x )
          {
            for ( mask = 0; shift < 8; shift += zoom_factor )
              if ( ( shift & 1 ) == toggle )
                mask |= 0x80 >> shift;
            bmap[x] ^= mask;
            shift -= 8;
          }
        }
      }
      bmap += Bit.cols;
    }
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
      rotation = ( rotation + event->info ) & 1023;
      break;

    case event_Scale_Glyph:
      ptsize += event->info;
      if ( ptsize < 1 )
        ptsize = 1;
      if ( ptsize > MAXPTSIZE / zoom_factor )
        ptsize = MAXPTSIZE / zoom_factor;
      break;

    case event_Change_Glyph:
      if ( use_cmap )
      {
        if ( event->info < 0 )
        {
          if ( Code > -event->info )
            Code += event->info;
          else
            Code = 0;

          for ( ; Code >= 0; Code-- )
          {
            Num = TT_Char_Index( char_map, Code );
            if ( Num > 0 )
              break;
          }
        }
        else
        {
          if ( Code < 65536 - event->info - 1 )
            Code += event->info;
          else
            Code = 65536 - 1;

          for ( ; Code < 65536; Code++ )
          {
            Num = TT_Char_Index( char_map, Code );
            if ( Num > 0 )
              break;
          }
        }
      }
      else
      {
        if ( event->info < 0 )
        {
          if ( Num > -event->info )
            Num += event->info;
          else
            Num = 0;
        }
        else
        {
          if ( Num < num_glyphs - event->info - 1 )
            Num += event->info;
          else
            Num = num_glyphs - 1;
        }
      }
      break;
    }

    return 1;
  }


  void usage( char*  execname )
  {
    printf( "\n" );
    printf( "ftzoom: simple TrueType glyph viewer -- part of the FreeType project\n" );
    printf( "--------------------------------------------------------------------\n" );
    printf( "\n" );
    printf( "Usage: %s [options below] fontname[.ttf|.ttc]\n", execname );
    printf( "\n" );
    printf( "  -g       gray-level rendering (default: none)\n" );
    printf( "  -r R     use resolution R dpi (default: 96)\n" );
    printf( "  -z Z     Z:1 magnification (default: 1:1)\n" );
    printf( "  -p id    platform id (default: none)\n" );
    printf( "  -e id    encoding id (default: none)\n" );
    printf( "  -n       don't use the `post' table\n" );
    printf( "    If either -p or -e is not set, no cmap will be used.\n" );
    printf( "\n" );

    exit( EXIT_FAILURE );
  }


  /* stack check dtp */

  int  main( int  argc, char**  argv )
  {
    int       i;
    int       platform = -1, encoding = -1;
    char      filename[128 + 4];
    char      alt_filename[128 + 4];
    char*     execname;
    int       option;
    int       res = 96;
    TT_Error  error;
    TT_Post   post;

    TEvent    event;


    execname    = argv[0];
    gray_render = 0;


#ifdef DEBUG_LEVEL_TRACE

    set_tt_trace_levels( trace_raster, 7 );
    set_tt_trace_levels( trace_gload, 7 );

#endif


    while ( 1 )
    {
      option = ft_getopt( argc, argv, "e:gnp:r:z:" );

      if ( option == -1 )
        break;

      switch ( option )
      {
      case 'e':
        encoding = atoi( ft_optarg );
        break;

      case 'g':
        gray_render = 1;
        break;

      case 'n':
        use_post = 0;
        break;

      case 'p':
        platform = atoi( ft_optarg );
        break;

      case 'r':
        res = atoi( ft_optarg );
        if ( res < 1 )
          usage( execname );
        break;

      case 'z':
        zoom_factor = atoi( ft_optarg );
        if ( zoom_factor < 1 || zoom_factor > 16 )
          usage( execname );
        if ( zoom_factor > 4 )
          grid = 1;
        break;

      default:
        usage( execname );
        break;
      }
    }

    if ( ft_optind == argc )
      usage( execname );

    i = strlen( argv[ft_optind] );
    while ( i > 0 && argv[ft_optind][i] != '\\' && argv[ft_optind][i] != '/' )
    {
      if ( argv[ft_optind][i] == '.' )
        i = 0;
      i--;
    }

    filename[128] = '\0';
    alt_filename[128] = '\0';

    strncpy( filename, argv[ft_optind], 128 );
    strncpy( alt_filename, argv[ft_optind], 128 );

    if ( i >= 0 )
    {
      strncpy( filename + strlen( filename ), ".ttf", 4 );
      strncpy( alt_filename + strlen( alt_filename ), ".ttc", 4 );
    }

    if ( platform >= 0 || encoding >= 0 )
      use_cmap = 1;

    /* Initialization */
    TT_Init_FreeType( &engine );

    /* Initialization of the post extension */
    if ( use_post )
      TT_Init_Post_Extension( engine );

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

    num_glyphs   = properties.num_Glyphs;
    xcenter_upem = (properties.header->xMax - properties.header->xMin) / 2;
    ycenter_upem = (properties.header->yMax - properties.header->yMin) / 2;
    units_per_em = properties.header->Units_Per_EM;

    /* load full post table */
    if ( use_post )
    {
      error = TT_Load_PS_Names( face, &post );
      if ( error )
        Panic( "Could not load PS names.\n" );
    }

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

    if ( gray_render )
    {
      if ( !SetGraphScreen( Graphics_Mode_Gray ) )
        Panic( "Could not set up grayscale graphics mode.\n" );

      TT_Set_Raster_Gray_Palette( engine, gray_palette );
    }
    else
    {
      if ( !SetGraphScreen( Graphics_Mode_Mono ) )
        Panic( "Could not set up mono graphics mode.\n" );
    }

    Init_Raster_Area();

    old_ptsize   = ptsize   = 150 / zoom_factor;
    old_rotation = rotation = 0;

    Reset_PtSize  ( ptsize );
    Reset_Rotation( rotation );

    if ( use_cmap )
    {
      unsigned short  num_cmap;
      unsigned short  cmap_plat;
      unsigned short  cmap_enc;


      num_cmap = properties.num_CharMaps;
      for ( i = 0; i < num_cmap; i++ )
      {
        error = TT_Get_CharMap_ID( face, i, &cmap_plat, &cmap_enc );
        if ( error )
          Panic( "Cannot query cmap, error = 0x%x.\n", error );
        if ( cmap_plat == platform && cmap_enc == encoding )
          break;
      }

      if ( i == num_cmap )
        Panic( "Invalid platform and/or encoding ID.\n" );

      error = TT_Get_CharMap( face, i, &char_map );
      if ( error )
        Panic( "Cannot load cmap, error = 0x%x.\n", error );

      num_glyphs = (1L << 16) - 1;
    }

    Code = 0;
    Num  = 0;
    Fail = 0;

    for ( ;; )
    {
      char *glyphname;


      glyphname = NULL;

      ClearData();

      if ( ptsize != old_ptsize )
      {
        Reset_PtSize( ptsize );
        old_ptsize = ptsize;
      }

      if ( rotation != old_rotation )
      {
        Reset_Rotation( rotation );
        old_rotation = rotation;
      }

      if ( (error = LoadTrueTypeChar( Num, hinted )) == TT_Err_Ok )
      {
        ConvertRaster();
        if ( grid )
           DrawGrid();

        if ( use_post )
          (void)TT_Get_PS_Name(face, Num, &glyphname);

        if ( use_cmap )
          sprintf( Header, "\"%s\": index = %3d, code = 0x%x, hinting = %s",
                   use_post ? glyphname : "", Num, Code,
                   hinted ? "ON" : "OFF" );
        else
          sprintf( Header, "\"%s\": index = %3d, hinting = %s",
                   use_post ? glyphname : "", Num, hinted ? "ON" : "OFF" );
      }
      else {
        Fail++;
        sprintf( Header, "\"%s\": index = %3d, hinting = %s (ERROR 0x%lx)",
                 glyphname ? glyphname : "", Num,
                 hinted ? "ON" : "OFF", error );
      }

      Display_Bitmap_On_Screen( Bit.bitmap, Bit.rows, Bit.cols );

#ifndef X11
#ifndef OS2
      Print_XY( 0, 0, Header );
#endif
#endif

      Get_Event( &event );
      if ( !Process_Event( &event ) ) goto End;
    }

  End:

    RestoreScreen();

    TT_Close_Face( face );
    TT_Done_FreeType( engine );

    printf( "Fails = %d.\n", Fail );

    exit( EXIT_SUCCESS );      /* for safety reasons */

    return 0;       /* never reached */
  }


/* End */
