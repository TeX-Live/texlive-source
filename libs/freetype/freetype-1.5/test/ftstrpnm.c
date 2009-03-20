/****************************************************************************/
/*                                                                          */
/*  The FreeType project -- a free and portable quality TrueType renderer.  */
/*                                                                          */
/*  Copyright 1996-2001 by                                                  */
/*  D. Turner, R.Wilhelm, and W. Lemberg                                    */
/*                                                                          */
/*  ftstrpnm: convert text to image (in PGM or PBM format)                  */
/*                                                                          */
/*  NOTE:  This is just a test program that is used to show off and         */
/*         debug the current engine.                                        */
/*                                                                          */
/****************************************************************************/

#define PROGNAME "ftstrpnm"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "freetype.h"

#define TT_VALID( handle )  ( ( handle ).z != NULL )


  /* Global variables */

  TT_Engine    engine;
  TT_Face      face;
  TT_Instance  instance;

  TT_Face_Properties  properties;

  TT_Raster_Map  bit;
  TT_Raster_Map  small_bit;   /* used when font-smoothing is enabled */

  int  pnm_width,   pnm_height;
  int  pnm_x_shift, pnm_y_shift;


  /* Loaded glyphs for all characters */

  TT_Glyph  *glyphs = NULL;


  /* Options */

  int  dpi    = 96;
  int  ptsize = 12;
  int  hinted = 1;
  int  smooth = 0;
  int  border = 0;

  /* color for text in RGB format */
  unsigned short int textclr[3] = { 0, 0, 0 }; 

  /* raster map management */

  static void  Init_Raster_Map( TT_Raster_Map*  bit, int  width, int  height )
  {
    bit->rows  = height;
    bit->width = ( width + 3 ) & -4;
    bit->flow  = TT_Flow_Down;

    if ( smooth )
    {
      bit->cols  = bit->width;
      bit->size  = bit->rows * bit->width;
    }
    else
    {
      bit->cols  = ( bit->width + 7 ) / 8;    /* convert to # of bytes     */
      bit->size  = bit->rows * bit->cols;     /* number of bytes in buffer */
    }

    bit->bitmap = (void *) malloc( bit->size );
    if ( !bit->bitmap )
      Panic( "Not enough memory to allocate bitmap!\n" );
  }


  static void  Done_Raster_Map( TT_Raster_Map *bit )
  {
    free( bit->bitmap );
    bit->bitmap = NULL;
  }


  static void  Clear_Raster_Map( TT_Raster_Map*  bit )
  {
    memset( bit->bitmap, 0, bit->size );
  }


  static void  Blit_Or( TT_Raster_Map*  dst, TT_Raster_Map*  src,
                        int  x_off, int  y_off )
  {
    int   x, y;
    int   x1, x2, y1, y2;
    char  *s, *d;


    /* clipping */

    x1 = x_off < 0 ? -x_off : 0;
    y1 = y_off < 0 ? -y_off : 0;

    x2 = (int)dst->cols - x_off;
    if ( x2 > src->cols )
      x2 = src->cols;

    y2 = (int)dst->rows - y_off;
    if ( y2 > src->rows )
      y2 = src->rows;

    if ( x1 >= x2 )
      return;

    /* do the real work now */

    for ( y = y1; y < y2; ++y )
    {
      s = ( (char*)src->bitmap ) + y * src->cols + x1;
      d = ( (char*)dst->bitmap ) + ( y + y_off ) * dst->cols + x1 + x_off;

      for ( x = x1; x < x2; ++x )
        *d++ |= *s++;
    }
  }


  static void  Dump_Raster_Map( TT_Raster_Map*  bit, FILE*  file )
  {
    /* kudos for this code snippet go to Norman Walsh */

    char*  bmap;
    int    i, j;
    unsigned short int colors[3][5];

    bmap = (char *)bit->bitmap;

    if ( smooth ) /* anti-aliasing */
    {
      fprintf( file, "P6\n%d %d\n255\n", pnm_width, pnm_height );
      
      /* calculate colors for anti-aliasing */
      for (i = 0; i < 3; i++ ) 
        for (j = 0; j < 5; j++) 
          colors[i][j] = (255 - textclr[i]) * j / 4 + textclr[i];

       for ( i = 0; i < bit->size; i++ )  /* iterate pixels */
        bmap[i] = bmap[i] > 4 ? 0 : 4 - bmap[i];
  
        for ( i = pnm_height; i > 0; i--, bmap += bit->cols )
           for ( j = 0; j < pnm_width; j++ )
             fprintf( file, "%c%c%c", colors[0][(unsigned int) bmap[j]],\
                                      colors[1][(unsigned int) bmap[j]],\
                                      colors[2][(unsigned int) bmap[j]]);
    } 
    else  /* no anti-aliasing */
    {
      fprintf( file, "P6\n%d %d\n255\n", pnm_width, pnm_height );

      for ( i = 0; i < pnm_height; i++, bmap += bit->cols ) /* iterate rows */
        for ( j = 0; j < pnm_width; j++ ) /* iterate bits(=columns) in row */
          if ( bmap[j / 8] >> ( 7 - j % 8 ) & 1 ) /* bit set? */
            fprintf( file, "%c%c%c", textclr[0], textclr[1], textclr[2] );
    else
            fprintf( file, "%c%c%c", 255, 255, 255 );
    }
    fflush( file );
  }


  /* glyph management */

  static void  Load_Glyphs( char*  txt, int  txtlen )
  {
    unsigned short  i, n, code, load_flags;
    unsigned short  num_glyphs = 0, no_cmap = 0;
    unsigned short  platform, encoding;
    TT_Error        error;
    TT_CharMap      char_map;


    /* First, look for a Unicode charmap */

    n = properties.num_CharMaps;

    for ( i = 0; i < n; i++ )
    {
      TT_Get_CharMap_ID( face, i, &platform, &encoding );
      if ( (platform == 3 && encoding == 1 ) ||
           (platform == 0 && encoding == 0 ) )
      {
        TT_Get_CharMap( face, i, &char_map );
        break;
      }
    }

    if ( i == n )
    {
      TT_Face_Properties  properties;


      TT_Get_Face_Properties( face, &properties );

      no_cmap = 1;
      num_glyphs = properties.num_Glyphs;
    }


    /* Second, allocate the array */

    glyphs = (TT_Glyph*)malloc( 256 * sizeof ( TT_Glyph ) );
    memset( glyphs, 0, 256 * sizeof ( TT_Glyph ) );

    /* Finally, load the glyphs you need */

    load_flags = TTLOAD_SCALE_GLYPH;
    if ( hinted )
      load_flags |= TTLOAD_HINT_GLYPH;

    for ( i = 0; i < txtlen; ++i )
    {
      unsigned char  j = txt[i];


      if ( TT_VALID( glyphs[j] ) )
        continue;

      if ( no_cmap )
      {
        code = (j - ' ' + 1) < 0 ? 0 : (j - ' ' + 1);
        if ( code >= num_glyphs )
          code = 0;
      }
      else
        code = TT_Char_Index( char_map, j );

      (void)(
        ( error = TT_New_Glyph( face, &glyphs[j] ) ) ||
        ( error = TT_Load_Glyph( instance, glyphs[j], code, load_flags ) )
      );

      if ( error )
        Panic( "Cannot allocate and load glyph: error 0x%x.\n", error );
    }
  }


  static void  Done_Glyphs( void )
  {
    int  i;


    if ( !glyphs )
      return;

    for ( i = 0; i < 256; ++i )
      TT_Done_Glyph( glyphs[i] );

    free( glyphs );

    glyphs = NULL;
  }


  /* face & instance management */

  static void  Init_Face( const char*  filename )
  {
    TT_Error  error;


    /* load the typeface */

    error = TT_Open_Face( engine, filename, &face );
    if ( error )
    {
      if ( error == TT_Err_Could_Not_Open_File )
        Panic( "Could not find/open %s.\n", filename );
      else
        Panic( "Error while opening %s, error code = 0x%x.\n",
               filename, error );
    }

    TT_Get_Face_Properties( face, &properties );

    /* create and initialize instance */

    (void) (
      ( error = TT_New_Instance( face, &instance ) ) ||
      ( error = TT_Set_Instance_Resolutions( instance, dpi, dpi ) ) ||
      ( error = TT_Set_Instance_PointSize( instance, ptsize ) )
    );

    if ( error )
      Panic( "Could not create and initialize instance: error 0x%x.\n",
             error );
  }


  static void  Done_Face( void )
  {
    TT_Done_Instance( instance );
    TT_Close_Face( face );
  }


  /* rasterization stuff */

  static void  Init_Raster_Areas( const char*  txt, int  txtlen )
  {
    int                  i, upm, ascent, descent;
    TT_Face_Properties   properties;
    TT_Instance_Metrics  imetrics;
    TT_Glyph_Metrics     gmetrics;


    /* allocate the large bitmap */

    TT_Get_Face_Properties( face, &properties );
    TT_Get_Instance_Metrics( instance, &imetrics );

    upm     = properties.header->Units_Per_EM;
    ascent  = ( properties.horizontal->Ascender  * imetrics.y_ppem ) / upm;
    descent = ( properties.horizontal->Descender * imetrics.y_ppem ) / upm;

    pnm_width   = 2 * border;
    pnm_height  = 2 * border + ascent - descent;

    for ( i = 0; i < txtlen; ++i )
    {
      unsigned char  j = txt[i];


      if ( !TT_VALID( glyphs[j] ) )
        continue;

      TT_Get_Glyph_Metrics( glyphs[j], &gmetrics );
      pnm_width += gmetrics.advance / 64;
    }

    Init_Raster_Map( &bit, pnm_width, pnm_height );
    Clear_Raster_Map( &bit );

    pnm_x_shift = border;
    pnm_y_shift = border - descent;

    /* allocate the small bitmap if you need it */

    if ( smooth )
      Init_Raster_Map( &small_bit, imetrics.x_ppem + 32, pnm_height );
  }


  static void  Done_Raster_Areas( void )
  {
    Done_Raster_Map( &bit );
    if ( smooth )
      Done_Raster_Map( &small_bit );
  }


  static void  Render_Glyph( TT_Glyph  glyph,
                             int  x_off, int  y_off,
                             TT_Glyph_Metrics*  gmetrics )
  {
    if ( !smooth )
      TT_Get_Glyph_Bitmap( glyph, &bit, x_off * 64L, y_off * 64L);
    else
    {
      TT_F26Dot6  xmin, ymin, xmax, ymax;


      /* grid-fit the bounding box */

      xmin =  gmetrics->bbox.xMin & -64;
      ymin =  gmetrics->bbox.yMin & -64;
      xmax = (gmetrics->bbox.xMax + 63) & -64;
      ymax = (gmetrics->bbox.yMax + 63) & -64;

      /* now render the glyph in the small pixmap */
      /* and blit-or the resulting small pixmap into the biggest one */

      Clear_Raster_Map( &small_bit );
      TT_Get_Glyph_Pixmap( glyph, &small_bit, -xmin, -ymin );
      Blit_Or( &bit, &small_bit, xmin/64 + x_off, -ymin/64 - y_off );
    }
  }


  static void  Render_All_Glyphs( char*  txt, int  txtlen )
  {
    int               i;
    TT_F26Dot6        x, y, adjx;
    TT_Glyph_Metrics  gmetrics;


    x = pnm_x_shift;
    y = pnm_y_shift;

    for ( i = 0; i < txtlen; i++ )
    {
      unsigned char  j = txt[i];

      if ( !TT_VALID( glyphs[j] ) )
        continue;

      TT_Get_Glyph_Metrics( glyphs[j], &gmetrics );

      adjx = x;                                         /* ??? lsb */
      Render_Glyph( glyphs[j], adjx, y, &gmetrics );

      x += gmetrics.advance / 64;
    }
  }


  static void  usage( void )
  {
    printf( "\n" );
    printf( "%s: simple text to image converter -- part of the FreeType project\n", PROGNAME );
    printf( "\n" );
    printf( "Usage: %s [options below] filename [string]\n", PROGNAME );
    printf( "\n" );
    printf( "  -g     gray-level rendering (default: off)\n" );
    printf( "  -h     hinting off (default: on)\n" );
    printf( "  -r X   resolution X dpi (default: 96)\n" );
    printf( "  -p X   pointsize X pt (default: 12)\n" );
    printf( "  -b X   border X pixels wide (default: 0)\n" );
    printf( "  -c X   textcolor X in RRGGBB hexadecimal format (default: 00000 (=black))\n" );
    printf( "\n" );

    exit( EXIT_FAILURE );
  }


  int  main( int  argc, char**  argv )
  {
    int       option, txtlen;
    long int  hexvalue;
    char      *txt, *filename;
    TT_Error  error;


    /* Parse options */

    while ( 1 )
    {
      option = ft_getopt( argc, argv, "ghr:p:b:c:" );

      if ( option == -1 )
        break;

      switch ( option )
      {
      case 'g':
        smooth = 1;
        break;
      case 'h':
        hinted = 0;
        break;
      case 'r':
        dpi = atoi( ft_optarg );
        break;
      case 'p':
        ptsize = atoi( ft_optarg );
        break;
      case 'b':
        border = atoi( ft_optarg );
        break;
      case 'c':
        sscanf( ft_optarg, "%lx", &hexvalue );
        textclr[0] = hexvalue>>16 & 0xFF;
        textclr[1] = hexvalue>> 8 & 0xFF;
        textclr[2] = hexvalue>> 0 & 0xFF;
        break;

      default:
        usage();
        break;
      }
    }

    argc -= ft_optind;
    argv += ft_optind;

    if ( argc <= 0 || argc > 2 || dpi <= 0 || ptsize <= 0 || border < 0 )
      usage();

    filename = argv[0];

    if ( argc > 1 )
      txt = argv[1];
    else
      txt = "The quick brown fox jumps over the lazy dog";

    txtlen = strlen( txt );

    /* Initialize engine and other stuff */

    error = TT_Init_FreeType( &engine );
    if ( error )
      Panic( "Error while initializing engine, code = 0x%x.\n", error );

    Init_Face( filename );
    Load_Glyphs( txt, txtlen );
    Init_Raster_Areas( txt, txtlen );

    /* Do the real work now */

    Render_All_Glyphs( txt, txtlen );
    Dump_Raster_Map( &bit, stdout );

    /* Clean up */

    Done_Raster_Areas();
    Done_Glyphs();
    Done_Face();

    /* That's all, folks! */

    TT_Done_FreeType( engine );

    exit( EXIT_SUCCESS );      /* for safety reasons */

    return 0;       /* never reached */
}


/* End */
