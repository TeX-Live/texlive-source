/****************************************************************************/
/*                                                                          */
/*  The FreeType project -- a free and portable quality TrueType renderer.  */
/*                                                                          */
/*  Copyright 1996-2001 by                                                  */
/*  D. Turner, R.Wilhelm, and W. Lemberg                                    */
/*                                                                          */
/*  ftsbit: a _very_ simple embedded bitmap dumper for FreeType 1.x.        */
/*                                                                          */
/*  NOTE:  This is just a test program that is used to show off and         */
/*         debug the current engine.                                        */
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

  static char*  TT_ErrToString18( TT_Error  error )
  {
    static char  temp[32];


    sprintf( temp, "0x%04lx", error );
    return temp;
  }

#endif /* !HAVE_LIBINTL_H */


  TT_Error     error;

  TT_Engine    engine;

  TT_Face      face;
  TT_Instance  instance;
  TT_Glyph     glyph;

  TT_Outline        outline;
  TT_Glyph_Metrics  metrics;

  TT_Face_Properties  properties;
  TT_EBLC             eblc;
  TT_SBit_Image*      bitmap;

  unsigned int  num_glyphs;
  int           ptsize;

  int  Fail;
  int  Num;


  static void  Usage( char*  name )
  {
    char*  gt;


    gt = gettext( "ftsbit: Simple TrueType `sbit' dumper -- part of the FreeType project" );
    fprintf( stderr, "%s\n", gt );
    separator_line( stderr, strlen( gt ) );

    fprintf( stderr, gettext(
             "Usage: %s ppem fontname[.ttf|.ttc] glyph_index [glyph_index2..]\n\n" ), name );

    exit( EXIT_FAILURE );
  }


  int  main( int  argc, char**  argv )
  {
    int    i;
    char   filename[128 + 4];
    char   alt_filename[128 + 4];
    char*  execname;
    char*  fname;


#ifdef HAVE_LIBINTL_H
    setlocale( LC_ALL, "" );
    bindtextdomain( "freetype", LOCALEDIR );
    textdomain( "freetype" );
#endif

    execname = argv[0];

    if ( argc < 3 )
      Usage( execname );

    if ( sscanf( argv[1], "%d", &ptsize ) != 1 )
      Usage( execname );

    /* Initialize engine */
    if ( (error = TT_Init_FreeType( &engine )) )
    {
      fprintf( stderr, gettext( "Error while initializing engine.\n" ) );
      goto Failure;
    }

    if ( (error = TT_Init_SBit_Extension( engine )) )
    {
      fprintf( stderr, gettext(
               "Error while initializing embedded bitmap extension.\n" ) );
      goto Failure;
    }

    /* Now check all files */
    fname = argv[2];
    i     = strlen( fname );
    while ( i > 0 && fname[i] != '\\' && fname[i] != '/' )
    {
      if ( fname[i] == '.' )
        i = 0;
      i--;
    }

    filename[128] = '\0';
    alt_filename[128] = '\0';

    strncpy( filename, fname, 128 );
    strncpy( alt_filename, fname, 128 );

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

    i     = strlen( filename );
    fname = filename;

    while ( i >= 0 )
      if ( filename[i] == '/' || filename[i] == '\\' )
      {
        fname = filename + i + 1;
        i = -1;
      }
      else
        i--;

    if ( error )
    {
      fprintf( stderr, gettext( "Could not find or open %s.\n" ),
               filename );
      goto Failure;
    }
    if ( error )
    {
      fprintf( stderr, gettext( "Error while opening %s.\n" ), filename );
      goto Failure;
    }

    /* get face properties */
    TT_Get_Face_Properties( face, &properties );
    num_glyphs = properties.num_Glyphs;

    error = TT_Get_Face_Bitmaps( face, &eblc );
    if ( error == TT_Err_Table_Missing )
    {
      fprintf( stderr, gettext(
               "Could not find embedded bitmaps in this font.\n" ) );
      goto Failure;
    }
    if ( error )
    {
      fprintf( stderr, gettext(
               "Error while loading embedded bitmaps.\n" ) );
      goto Failure;
    }

    /* create instance */
    error = TT_New_Instance( face, &instance );
    if ( error )
    {
      fprintf( stderr, gettext( "Could not create instance.\n" ) );
      goto Failure;
    }

    error = TT_Set_Instance_PixelSizes( instance,
                                        ptsize,
                                        ptsize,
                                        ptsize*3/4 );
    if ( error )
    {
      fprintf( stderr,
               gettext( "Could not set point size to %d.\n" ),
               ptsize );
      goto Failure;
    }

    error = TT_New_SBit_Image( &bitmap );
    if ( error )
    {
      fprintf( stderr, gettext(
               "Could not allocate glyph bitmap container.\n" ) );
      goto Failure;
    }

    for ( i = 3; i < argc; i++ )
    {
      unsigned short  glyph_index;


      /* we use %i to allow the prefixes `0x' and `0' */
      if ( sscanf( argv[i], "%hi", &glyph_index ) != 1 )
        Usage( execname );

      error = TT_Load_Glyph_Bitmap( face, instance, glyph_index, bitmap );

      if ( error == TT_Err_Invalid_Glyph_Index )
      {
        fprintf( stderr, gettext(
                 "  no bitmap for glyph %d.\n" ), glyph_index );
        continue;
      }
      if ( error )
      {
        fprintf( stderr, gettext(
                 "Can't load bitmap for glyph %d.\n" ), glyph_index );
        goto Failure;
      }

      /* Dump the resulting bitmap */
      {
        printf( gettext( "glyph index %d = %dx%d pixels, " ),
                glyph_index, bitmap->map.rows, bitmap->map.width );

        printf( gettext( "advance = %ld, minBearing = [%ld,%ld]\n" ),
                (long)(bitmap->metrics.horiAdvance / 64),
                (long)(bitmap->metrics.horiBearingX / 64),
                (long)(bitmap->metrics.horiBearingY / 64));

        Show_Single_Glyph( &bitmap->map );
      }
    }

    TT_Done_SBit_Image( bitmap );
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
