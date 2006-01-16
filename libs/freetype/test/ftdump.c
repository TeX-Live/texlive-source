/****************************************************************************/
/*                                                                          */
/*  The FreeType project -- a free and portable quality TrueType renderer.  */
/*                                                                          */
/*  Copyright 1996-2001 by                                                  */
/*  D. Turner, R.Wilhelm, and W. Lemberg                                    */
/*                                                                          */
/*  ftdump: Simple TrueType font file resource profiler.                    */
/*                                                                          */
/*  This program dumps various properties of a given font file.             */
/*                                                                          */
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
#include "freetype.h"
#include "ftxcmap.h"
#include "ftxopen.h"   /* TrueType Open support */
#include "ftxsbit.h"   /* embedded bitmap support */

/*
 *  The following comment should be ignored. The "ttobjs.h" file does
 *  already include ft_conf.h.
 *
 * ------------------------------------------------------------------
 *
 * IGNORE> Basically, an external program using FreeType shouldn't depend on an
 * IGNORE> internal file of the FreeType library, especially not on ft_conf.h -- but
 * IGNORE> to avoid another configure script which tests for the existence of the
 * IGNORE> i18n stuff we include ft_conf.h here since we can be sure that our test
 * IGNORE> programs use the same configuration options as the library itself.
 */

#include "ttobjs.h"    /* We're going to access internal tables directly */
#include "ttmemory.h"  /* We use FREE() to satisfy -DDEBUG_MEMORY        */

#ifdef HAVE_LIBINTL_H

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#include "ftxerr18.h"
#include <libintl.h>
#else
#define gettext( x )  ( x )
#endif


  TT_Error     error;

  TT_Engine    engine;
  TT_Face      face;
  TT_Instance  instance;
  TT_Glyph     glyph;

  TT_Instance_Metrics  imetrics;
  TT_Outline           outline;
  TT_Glyph_Metrics     metrics;

  TT_Face_Properties   properties;

  int  num_glyphs;
  int  ptsize;

  int  Fail;
  int  Num;

  int  flag_memory    = 1;
  int  flag_names     = 1;
  int  flag_encodings = 1;
  int  flag_cmap      = 1;
  int  flag_sbits     = 1;
  int  flag_ttopen    = 1;

#if defined( FREETYPE_DLL ) || defined( TT_CONFIG_OPTION_THREAD_SAFE )

  /* If the library is linked as a DLL, TTMemory_Allocated()    */
  /* (which is not exported) cannot be accessed.                */
  /* In this case, some compilers report an error because       */
  /* they try to link against a non-existing symbol.            */
  /*                                                            */
  /* We thus avoid the external reference on these compilers.   */

  #define TTMemory_Allocated  0L

#else
  extern long  TTMemory_Allocated;
#endif

  long  org_memory, old_memory, cur_memory;

  const char*  Apple_Encodings[33] =
  {
    "Roman", "Japanese", "Chinese", "Korean", "Arabic", "Hebrew",
    "Greek", "Russian", "RSymbol", "Devanagari", "Gurmukhi",
    "Gujarati", "Oriya", "Bengali", "Tamil", "Telugu", "Kannada",
    "Malayalam", "Sinhalese", "Burmese", "Khmer", "Tai", "Laotian",
    "Georgian", "Armenian", "Maldivian/Simplif. Chinese", "Tibetan",
    "Mongolian", "Geez", "Slavic", "Vietnamese", "Sindhi", "Uninterpreted"
  };

  struct
  {
    long  initial_overhead;
    long  face_object;
    long  glyph_object;
    long  first_instance;
    long  second_instance;

  } memory_footprint;


  /* We ignore error message strings with this function */

#ifndef HAVE_LIBINTL_H
  static char*
  TT_ErrToString18( TT_Error  error )
  {
    static char  temp[32];


    sprintf( temp, "0x%04lx", error );
    return temp;
  }
#endif


  void
  Save_Memory( long*  var )
  {
    *var = TTMemory_Allocated - old_memory;
    old_memory += *var;
  }

#define FOOTPRINT( field )  Save_Memory( &memory_footprint.field )


  static void
  Print_Mem( long  val, char*  string )
  {
    printf( "%6ld Bytes (%4ld kByte): %s\n",
             val,
             ( val + 1023L ) / 1024,
             string );
  }

#define PRINT_MEM( field, string ) \
          Print_Mem( memory_footprint.field, string )


  /* Print the memory footprint */

  void
  Print_Memory( void )
  {
    /* create glyph */
    error = TT_New_Glyph( face, &glyph );
    if ( error )
    {
      fprintf( stderr, gettext( "Could not create glyph container.\n" ) );
      goto Failure;
    }

    FOOTPRINT( glyph_object );

    /* create instance */
    error = TT_New_Instance( face, &instance );
    if ( error )
    {
      fprintf( stderr, gettext( "Could not create instance.\n" ) );
      goto Failure;
    }

    FOOTPRINT( first_instance );

    error = TT_New_Instance( face, &instance );
    if ( error )
    {
      fprintf( stderr, gettext( "Could not create second instance.\n" ) );
      goto Failure;
    }

    FOOTPRINT( second_instance );

    printf( gettext( "Memory footprint statistics:\n" ) );
    separator_line( stdout, 78 );

    /* NOTE: In our current implementation, the face's execution */
    /*       context object is created lazily with the first     */
    /*       instance.  However, all later instances share the   */
    /*       the same context.                                   */

    PRINT_MEM( face_object,     gettext( "face object" )     );
    PRINT_MEM( glyph_object,    gettext( "glyph object" )    );
    PRINT_MEM( second_instance, gettext( "instance object" ) );

    Print_Mem( memory_footprint.first_instance -
               memory_footprint.second_instance,
               gettext( "exec. context object" ) );

    separator_line( stdout, 78 );

    Print_Mem( memory_footprint.face_object  +
               memory_footprint.glyph_object +
               memory_footprint.first_instance,
               gettext( "total memory usage" ) );

    printf( "\n" );

    return;

  Failure:
    fprintf( stderr, "  " );
    Panic( gettext( "FreeType error message: %s\n" ),
           TT_ErrToString18( error ) );
  }


  static char  name_buffer[257];
  static int   name_len = 0;


  static char*
  LookUp_Name( int  index )
  {
    unsigned short  i, n;

    unsigned short  platform, encoding, language, id;
    char*           string;
    unsigned short  string_len;

    int             j, found;


    n = properties.num_Names;

    for ( i = 0; i < n; i++ )
    {
      TT_Get_Name_ID( face, i, &platform, &encoding, &language, &id );
      TT_Get_Name_String( face, i, &string, &string_len );

      if ( id == index )
      {

        /* The following code was inspired from Mark Leisher's */
        /* ttf2bdf package                                     */

        found = 0;

        /* Try to find a Microsoft English name */

        if ( platform == 3 )
          for ( j = 1; j >= 0; j-- )
            if ( encoding == j )  /* Microsoft ? */
              if ( (language & 0x3FF) == 0x009 )    /* English language */
              {
                found = 1;
                break;
              }

        if ( !found && platform == 0 && language == 0 )
          found = 1;

        /* Found a Unicode Name. */

        if ( found )
        {
          if ( string_len > 512 )
            string_len = 512;

          name_len = 0;

          for ( i = 1; i < string_len; i += 2 )
            name_buffer[name_len++] = string[i];

          name_buffer[name_len] = '\0';

          return name_buffer;
        }
      }
    }

    /* Not found */
    return NULL;
  }


  static void
  Print_Names( void )
  {
    printf( gettext( "font name table entries\n" ) );
    separator_line( stdout, 78 );

    if ( LookUp_Name( 4 ) )
      printf( "%s - ", name_buffer );

    if ( LookUp_Name( 5 ) )
      printf( "%s\n\n", name_buffer );

    if ( LookUp_Name( 6 ) )
      printf( gettext( "PostScript name: %s\n\n" ), name_buffer );

    if ( LookUp_Name( 0 ) )
      printf( "%s\n\n", name_buffer );

    if ( LookUp_Name( 7 ) )
      printf( name_buffer );

    printf( "\n" );
    separator_line( stdout, 78 );
  }


  static void
  Print_Encodings( void )
  {
    unsigned short  n, i;
    unsigned short  platform, encoding;
    char*           platStr, *encoStr;

    char  tempStr[128];


    printf( gettext( "character map encodings\n" ) );
    separator_line( stdout, 78 );

    n = properties.num_CharMaps;
    if ( n == 0 )
    {
      printf( gettext(
              "The file doesn't seem to have any encoding table.\n" ) );
      return;
    }

    printf( gettext( "There are %hu encodings:\n\n" ), n );

    for ( i = 0; i < n; i++ )
    {
      TT_Get_CharMap_ID( face, i, &platform, &encoding );
      printf( gettext( "encoding %2u: " ), i );

      platStr = encoStr = NULL;

      switch ( platform )
      {
      case TT_PLATFORM_APPLE_UNICODE:
        platStr = "Apple Unicode";
        switch ( encoding )
        {
        case TT_APPLE_ID_DEFAULT:
          encoStr = "";
          break;

        case TT_APPLE_ID_UNICODE_1_1:
          encoStr = "(v.1.1)";
          break;

        case TT_APPLE_ID_ISO_10646:
          encoStr = "(ISO 10646-1:1993)";
          break;

        case TT_APPLE_ID_UNICODE_2_0:
          encoStr = "(v.2.0)";
          break;

        default:
          sprintf( tempStr, gettext( "Unknown value %hu" ), encoding );
          encoStr = tempStr;
        }
        break;

      case TT_PLATFORM_MACINTOSH:
        platStr = "Apple";
        if ( encoding > 32 )
        {
          sprintf( tempStr, gettext( "Unknown value %hu" ), encoding );
          encoStr = tempStr;
        }
        else
          encoStr = (char*)Apple_Encodings[encoding];
        break;

      case TT_PLATFORM_ISO:
        platStr = "Iso";
        switch ( encoding )
        {
        case TT_ISO_ID_7BIT_ASCII:
          platStr = "Ascii";
          encoStr = "7-bit";
          break;

        case TT_ISO_ID_10646:
          encoStr = "10646";
          break;

        case TT_ISO_ID_8859_1:
          encoStr = "8859-1";
          break;

        default:
          sprintf( tempStr, "%hu", encoding );
          encoStr = tempStr;
        }
        break;

      case TT_PLATFORM_MICROSOFT:
        platStr = "Windows";
        switch ( encoding )
        {
        case TT_MS_ID_SYMBOL_CS:
          encoStr = "Symbol";
          break;

        case TT_MS_ID_UNICODE_CS:
          encoStr = "Unicode";
          break;

        case TT_MS_ID_SJIS:
          encoStr = "Shift-JIS";
          break;

        case TT_MS_ID_GB2312:
          encoStr = "GB2312";
          break;

        case TT_MS_ID_BIG_5:
          encoStr = "Big 5";
          break;

        case TT_MS_ID_WANSUNG:
          encoStr = "WanSung";
          break;

        case TT_MS_ID_JOHAB:
          encoStr = "Johab";
          break;

        case TT_MS_ID_UCS_4:
          encoStr = "UCS-4";
          break;

        default:
          sprintf( tempStr, gettext( "Unknown value %hu" ), encoding );
          encoStr = tempStr;
        }
        break;

      default:
        sprintf( tempStr, "%hu - %hu", platform, encoding );
        platStr = gettext( "Unknown" );
        encoStr = tempStr;
      }

      printf( "%s %s\n", platStr, encoStr );
    }

    printf( "\n" );
    separator_line( stdout, 78 );
  }


  static void
  Print_Cmap( void )
  {
     TT_CharMap      charmap;
     TT_UShort       glyph_index;
     TT_Long         char_index;
     unsigned short  n, i;
     unsigned short  platform, encoding;


     printf( gettext( "ftxcmap test\n" ) );
     separator_line( stdout, 78 );

     n = properties.num_CharMaps;
     if ( n == 0 )
     {
       printf( gettext(
               "The file doesn't seem to have any encoding table.\n" ) );
       return;
     }

     printf( gettext( "There are %hu encodings:\n\n" ), n );

     for ( i = 0; i < n; i++ )
     {

       TT_Get_CharMap_ID( face, i, &platform, &encoding );
       printf( gettext( "encoding %2u:\n" ), i );

       TT_Get_CharMap( face, i, &charmap);

       char_index = TT_CharMap_First( charmap, &glyph_index );
       printf( gettext( "first: glyph index %hu, character code 0x%lx\n" ),
               glyph_index, char_index );

       char_index  = TT_CharMap_Next( charmap, char_index, &glyph_index );
       printf( gettext( "next:  glyph index %hu, character code 0x%lx\n" ),
               glyph_index, char_index );

       char_index  = TT_CharMap_Last( charmap, &glyph_index );
       printf( gettext( "last:  glyph index %hu, character code 0x%lx\n" ),
               glyph_index, char_index );
     }

     printf( "\n" );
     separator_line( stdout, 78 );
  }


  static void
  Print_SBits( void )
  {
    TT_EBLC   eblc;
    TT_Error  error;


    error = TT_Get_Face_Bitmaps( face, &eblc );
    if ( error == TT_Err_Table_Missing )
      return;
    if ( error )
    {
      fprintf( stderr, gettext(
               "Error while retrieving embedded bitmaps table.\n" ) );
      goto Failure;
    }

    printf( gettext( "embedded bitmap table\n" ) );
    separator_line( stdout, 78 );

    printf( gettext( " version of embedded bitmap table:  0x%lx\n" ),
            eblc.version );
    printf( gettext( " number of embedded bitmap strikes: %lu\n" ),
            eblc.num_strikes );

    {
      TT_SBit_Strike*  strike = eblc.strikes;
      int              count  = 0;


      for ( ; count < eblc.num_strikes; count++, strike++ )
      {
        printf( gettext( "  bitmap strike %hu/%lu: " ),
                count + 1, eblc.num_strikes );

        printf( gettext( "%hux%hu pixels, %hu-bit depth, glyphs [%hu..%hu]\n" ),
                strike->x_ppem, strike->y_ppem, strike->bit_depth,
                strike->start_glyph, strike->end_glyph );
        {
          TT_SBit_Range*  range = strike->sbit_ranges;
          TT_SBit_Range*  limit = range + strike->num_ranges;


          for ( ; range < limit; range++ )
            printf( gettext( "      range format (%hu:%hu) glyphs %hu..%hu\n" ),
                    range->index_format,
                    range->image_format,
                    range->first_glyph,
                    range->last_glyph );
        }
      }
    }
    printf( "\n" );
    separator_line( stdout, 78 );

    return;

  Failure:
    fprintf( stderr, "  " );
    Panic( gettext( "FreeType error message: %s\n" ),
           TT_ErrToString18( error ) );
  }


#define TAG2STRING( t, s )  s[0] = (char)(t >> 24), \
                            s[1] = (char)(t >> 16), \
                            s[2] = (char)(t >>  8), \
                            s[3] = (char)(t      )


  static void
  Print_GSUB( void )
  {
    TTO_GSUBHeader  gsub;
    TT_Error        error;

    TT_UShort       i;
    TTO_Feature     f;
    TTO_Lookup*     lo;

    TT_ULong        *script_tag_list, *stl;
    TT_ULong        *language_tag_list, *ltl;
    TT_ULong        *feature_tag_list, *ftl;

    TT_UShort       script_index, language_index,
                    feature_index, req_feature_index;

    char            script_tag[4], language_tag[4], feature_tag[4];


    error = TT_Load_GSUB_Table( face, &gsub, NULL );
    if ( error == TT_Err_Table_Missing )
      return;
    if ( error )
    {
      fprintf( stderr, gettext( "Error while loading GSUB table.\n" ) );
      goto Failure;
    }

    printf( gettext( "GSUB table\n" ) );
    separator_line( stdout, 78 );

    error = TT_GSUB_Query_Scripts( &gsub, &script_tag_list );
    if ( error )
    {
      fprintf( stderr, gettext(
               "Error while querying GSUB script list.\n" ) );
      goto Failure;
    }

    stl = script_tag_list;
    for ( ; *stl; stl++ )
    {
      TAG2STRING( *stl, script_tag );

      error = TT_GSUB_Select_Script( &gsub, *stl, &script_index );
      if ( error )
      {
        fprintf( stderr, gettext(
                 "Error while selecting GSUB script `%4.4s'.\n" ),
                 script_tag );
        goto Failure;
      }

      printf( gettext( "  script `%4.4s' (index %hu):\n" ),
              script_tag, script_index );

      error = TT_GSUB_Query_Features( &gsub, script_index, 0xFFFF,
                                      &feature_tag_list );
      if ( error )
      {
        fprintf( stderr, gettext(
                 "Error while querying GSUB default language system for script `%4.4s'.\n" ),
                 script_tag );
        goto Failure;
      }

      printf( gettext( "    default language system:\n" ) );

      ftl = feature_tag_list;
      for ( ; *ftl; ftl++ )
      {
        TAG2STRING( *ftl, feature_tag );

        error = TT_GSUB_Select_Feature( &gsub, *ftl,
                                        script_index, 0xFFFF,
                                        &feature_index );
        if ( error )
        {
          fprintf( stderr, gettext(
                   "Error while selecting GSUB feature `%4.4s'\n"
                   "for default language system of script `%4.4s'.\n" ),
                   feature_tag, script_tag );
          goto Failure;
        }

        printf( gettext( "      feature `%4.4s' (index %hu; lookup " ),
                feature_tag, feature_index );

        f = gsub.FeatureList.FeatureRecord[feature_index].Feature;

        for ( i = 0; i < f.LookupListCount - 1; i++ )
          printf( "%hu, ", f.LookupListIndex[i] );
        printf( "%hu)\n", f.LookupListIndex[i] );
      }
      FREE( feature_tag_list );

      error = TT_GSUB_Query_Languages( &gsub, script_index,
                                       &language_tag_list );
      if ( error )
      {
        fprintf( stderr, gettext(
                 "Error while querying GSUB language list for script `%4.4s'.\n" ),
                 script_tag );
        goto Failure;
      }

      ltl = language_tag_list;
      for ( ; *ltl; ltl++ )
      {
        TAG2STRING( *ltl, language_tag );

        error = TT_GSUB_Select_Language( &gsub, *ltl, 
                                         script_index,
                                         &language_index,
                                         &req_feature_index );
        if ( error )
        {
          fprintf( stderr, gettext(
                   "Error while selecting GSUB language `%4.4s' for script `%4.4s'.\n" ),
                   language_tag, script_tag );
          goto Failure;
        }

        printf( gettext( "    language `%4.4s' (index %hu):\n" ),
                language_tag, language_index );

        if ( req_feature_index != 0xFFFF )
        {
          printf( gettext( "      required feature index %hu (lookup " ),
                  req_feature_index );

          f = gsub.FeatureList.FeatureRecord[req_feature_index].Feature;

          for ( i = 0; i < f.LookupListCount - 1; i++ )
            printf( "%hu, ", f.LookupListIndex[i] );
          printf( "%hu)\n", f.LookupListIndex[i] );
        }

        error = TT_GSUB_Query_Features( &gsub, script_index, language_index,
                                        &feature_tag_list );
        if ( error )
        {
          fprintf( stderr, gettext(
                   "Error while querying GSUB feature list\n"
                   "for script `%4.4s', language `%4.4s'.\n" ),
                   script_tag, language_tag );
          goto Failure;
        }

        ftl = feature_tag_list;
        for ( ; *ftl; ftl++ )
        {
          TAG2STRING( *ftl, feature_tag );

          error = TT_GSUB_Select_Feature( &gsub, *ftl,
                                          script_index, language_index,
                                          &feature_index );
          if ( error )
          {
            fprintf( stderr, gettext(
                     "Error while selecting GSUB feature `%4.4s'\n"
                     "for script `%4.4s', language `%4.4s'.\n" ),
                     feature_tag, script_tag, language_tag );
            goto Failure;
          }

          printf( gettext( "      feature `%4.4s' (index %hu; lookup " ),
                  feature_tag, feature_index );

          f = gsub.FeatureList.FeatureRecord[feature_index].Feature;

          for ( i = 0; i < f.LookupListCount - 1; i++ )
            printf( "%hu, ", f.LookupListIndex[i] );
          printf( "%hu)\n", f.LookupListIndex[i] );
        }
        FREE( feature_tag_list );
      }
      FREE( language_tag_list );
    }
    FREE( script_tag_list );

    printf( "\n" );

    lo = gsub.LookupList.Lookup;

    printf( gettext( "Lookups:\n\n" ) );

    for ( i = 0; i < gsub.LookupList.LookupCount; i++ )
      printf( gettext( "  %hu: type %hu, flag 0x%x\n" ),
              i, lo[i].LookupType, lo[i].LookupFlag );

    printf( "\n" );
    separator_line( stdout, 78 );

    return;

  Failure:
    fprintf( stderr, "  " );
    Panic( gettext( "FreeType error message: %s\n" ),
           TT_ErrToString18( error ) );
  }


  static void
  Print_GPOS( void )
  {
    TTO_GPOSHeader  gpos;
    TT_Error        error;

    TT_UShort       i;
    TTO_Feature     f;
    TTO_Lookup*     lo;

    TT_ULong        *script_tag_list, *stl;
    TT_ULong        *language_tag_list, *ltl;
    TT_ULong        *feature_tag_list, *ftl;

    TT_UShort       script_index, language_index,
                    feature_index, req_feature_index;

    char            script_tag[4], language_tag[4], feature_tag[4];


    error = TT_Load_GPOS_Table( face, &gpos, NULL );
    if ( error == TT_Err_Table_Missing )
      return;
    if ( error )
    {
      fprintf( stderr, gettext( "Error while loading GPOS table.\n" ) );
      goto Failure;
    }

    printf( gettext( "GPOS table\n" ) );
    separator_line( stdout, 78 );

    error = TT_GPOS_Query_Scripts( &gpos, &script_tag_list );
    if ( error )
    {
      fprintf( stderr, gettext(
               "Error while querying GPOS script list.\n" ) );
      goto Failure;
    }

    stl = script_tag_list;
    for ( ; *stl; stl++ )
    {
      TAG2STRING( *stl, script_tag );

      error = TT_GPOS_Select_Script( &gpos, *stl, &script_index );
      if ( error )
      {
        fprintf( stderr, gettext(
                 "Error while selecting GPOS script `%4.4s'.\n" ),
                 script_tag );
        goto Failure;
      }

      printf( gettext( "  script `%4.4s' (index %hu):\n" ),
              script_tag, script_index );

      error = TT_GPOS_Query_Features( &gpos, script_index, 0xFFFF,
                                      &feature_tag_list );
      if ( error )
      {
        fprintf( stderr, gettext(
                 "Error while querying GPOS default language system for script `%4.4s'.\n" ),
                 script_tag );
        goto Failure;
      }

      printf( gettext( "    default language system:\n" ) );

      ftl = feature_tag_list;
      for ( ; *ftl; ftl++ )
      {
        TAG2STRING( *ftl, feature_tag );

        error = TT_GPOS_Select_Feature( &gpos, *ftl,
                                        script_index, 0xFFFF,
                                        &feature_index );
        if ( error )
        {
          fprintf( stderr, gettext(
                   "Error while selecting GPOS feature `%4.4s'\n"
                   "for default language system of script `%4.4s'.\n" ),
                   feature_tag, script_tag );
          goto Failure;
        }

        printf( gettext( "      feature `%4.4s' (index %hu; lookup " ),
                feature_tag, feature_index );

        f = gpos.FeatureList.FeatureRecord[feature_index].Feature;

        for ( i = 0; i < f.LookupListCount - 1; i++ )
          printf( "%hu, ", f.LookupListIndex[i] );
        printf( "%hu)\n", f.LookupListIndex[i] );
      }
      FREE( feature_tag_list );

      error = TT_GPOS_Query_Languages( &gpos, script_index,
                                       &language_tag_list );
      if ( error )
      {
        fprintf( stderr, gettext(
                 "Error while querying GPOS language list for script `%4.4s'.\n" ),
                 script_tag );
        goto Failure;
      }

      ltl = language_tag_list;
      for ( ; *ltl; ltl++ )
      {
        TAG2STRING( *ltl, language_tag );

        error = TT_GPOS_Select_Language( &gpos, *ltl, 
                                         script_index,
                                         &language_index,
                                         &req_feature_index );
        if ( error )
        {
          fprintf( stderr, gettext(
                   "Error while selecting GPOS language `%4.4s' for script `%4.4s'.\n" ),
                   language_tag, script_tag );
          goto Failure;
        }

        printf( gettext( "    language `%4.4s' (index %hu):\n" ),
                language_tag, language_index );

        if ( req_feature_index != 0xFFFF )
        {
          printf( gettext( "      required feature index %hu (lookup " ),
                  req_feature_index );

          f = gpos.FeatureList.FeatureRecord[req_feature_index].Feature;

          for ( i = 0; i < f.LookupListCount - 1; i++ )
            printf( "%hu, ", f.LookupListIndex[i] );
          printf( "%hu)\n", f.LookupListIndex[i] );
        }

        error = TT_GPOS_Query_Features( &gpos, script_index, language_index,
                                        &feature_tag_list );
        if ( error )
        {
          fprintf( stderr, gettext(
                   "Error while querying GPOS feature list\n"
                   "for script `%4.4s', language `%4.4s'.\n" ),
                   script_tag, language_tag );
          goto Failure;
        }

        ftl = feature_tag_list;
        for ( ; *ftl; ftl++ )
        {
          TAG2STRING( *ftl, feature_tag );

          error = TT_GPOS_Select_Feature( &gpos, *ftl,
                                          script_index, language_index,
                                          &feature_index );
          if ( error )
          {
            fprintf( stderr, gettext(
                     "Error while selecting GPOS feature `%4.4s'\n"
                     "for script `%4.4s', language `%4.4s'.\n" ),
                     feature_tag, script_tag, language_tag );
            goto Failure;
          }

          printf( gettext( "      feature `%4.4s' (index %hu; lookup " ),
                  feature_tag, feature_index );

          f = gpos.FeatureList.FeatureRecord[feature_index].Feature;

          for ( i = 0; i < f.LookupListCount - 1; i++ )
            printf( "%hu, ", f.LookupListIndex[i] );
          printf( "%hu)\n", f.LookupListIndex[i] );
        }
        FREE( feature_tag_list );
      }
      FREE( language_tag_list );
    }
    FREE( script_tag_list );

    printf( "\n" );

    lo = gpos.LookupList.Lookup;

    printf( gettext( "Lookups:\n\n" ) );

    for ( i = 0; i < gpos.LookupList.LookupCount; i++ )
      printf( gettext( "  %hu: type %hu, flag 0x%x\n" ),
              i, lo[i].LookupType, lo[i].LookupFlag );

    printf( "\n" );
    separator_line( stdout, 78 );

    return;

  Failure:
    fprintf( stderr, "  " );
    Panic( gettext( "FreeType error message: %s\n" ),
           TT_ErrToString18( error ) );
  }


  int
  main( int  argc, char**  argv )
  {
    int    i;
    char   filename[128 + 4];
    char   alt_filename[128 + 4];
    char*  execname;
    char*  gt;


#ifdef HAVE_LIBINTL_H
    setlocale( LC_ALL, "" );
    bindtextdomain( "freetype", LOCALEDIR );
    textdomain( "freetype" );
#endif

    execname = argv[0];

    if ( argc != 2 )
    {
      gt = gettext( "ftdump: Simple TrueType Dumper -- part of the FreeType project" );
      fprintf( stderr, "%s\n", gt );
      separator_line( stderr, strlen( gt ) );

      fprintf( stderr, gettext( "Usage: %s fontname[.ttf|.ttc]\n\n" ),
               execname );

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

    old_memory = 0;

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

    if ( (error = TT_Init_GSUB_Extension( engine )) )
    {
      fprintf( stderr, gettext(
               "Error while initializing GSUB extension.\n" ) );
      goto Failure;
    }

    if ( (error = TT_Init_GPOS_Extension( engine )) )
    {
      fprintf( stderr, gettext(
               "Error while initializing GPOS extension.\n" ) );
      goto Failure;
    }

    FOOTPRINT( initial_overhead );

    /* Open and Load face */

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
      fprintf( stderr, gettext( "Error while opening %s.\n" ), filename );
      goto Failure;
    }

    FOOTPRINT( face_object );

    /* get face properties and allocate preload arrays */

    TT_Get_Face_Properties( face, &properties );
    num_glyphs = properties.num_Glyphs;

    /* Now do various dumps */

    if ( flag_names )
      Print_Names();

    if ( flag_encodings )
      Print_Encodings();

    if ( flag_cmap )
      Print_Cmap();

    if ( flag_sbits )
      Print_SBits();

    if ( flag_ttopen )
    {
      Print_GSUB();
      Print_GPOS();
    }

#ifndef FREETYPE_DLL  /* the statistics are meaningless if we use a DLL. */
    if ( flag_memory )
      Print_Memory();
#endif

    TT_Close_Face( face );
    TT_Done_FreeType( engine );

    exit( EXIT_SUCCESS );       /* for safety reasons */

    return 0;                   /* never reached */

  Failure:
    fprintf( stderr, "  " );
    Panic( gettext( "FreeType error message: %s\n" ),
           TT_ErrToString18( error ) );

    return 0;                   /* never reached */
}


/* End */
