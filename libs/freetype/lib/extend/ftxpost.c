/*******************************************************************
 *
 *  ftxpost.c
 *
 *    post table support API extension body
 *
 *  Copyright 1996-2002 by
 *  David Turner, Robert Wilhelm, and Werner Lemberg.
 *
 *  This file is part of the FreeType project, and may only be used
 *  modified and distributed under the terms of the FreeType project
 *  license, LICENSE.TXT.  By continuing to use, modify, or distribute
 *  this file you indicate that you have read the license and
 *  understand and accept it fully.
 *
 *
 *  The post table is not completely loaded by the core engine.  This
 *  file loads the missing PS glyph names and implements an API to
 *  access them.
 *
 ******************************************************************/

#include "ftxpost.h"

#include "tttypes.h"
#include "ttobjs.h"
#include "tttables.h"
#include "ttload.h"     /* for the macros */
#include "ttfile.h"
#include "tttags.h"
#include "ttmemory.h"
#include "ttextend.h"


#define POST_ID  Build_Extension_ID( 'p', 'o', 's', 't' )


  /* the 258 default Mac PS glyph names */

  String*  TT_Post_Default_Names[258] =
  {
    /*   0 */
    ".notdef", ".null", "CR", "space", "exclam",
    "quotedbl", "numbersign", "dollar", "percent", "ampersand",
    /*  10 */
    "quotesingle", "parenleft", "parenright", "asterisk", "plus",
    "comma", "hyphen", "period", "slash", "zero",
    /*  20 */
    "one", "two", "three", "four", "five",
    "six", "seven", "eight", "nine", "colon",
    /*  30 */
    "semicolon", "less", "equal", "greater", "question",
    "at", "A", "B", "C", "D",
    /*  40 */
    "E", "F", "G", "H", "I",
    "J", "K", "L", "M", "N",
    /*  50 */
    "O", "P", "Q", "R", "S",
    "T", "U", "V", "W", "X",
    /*  60 */
    "Y", "Z", "bracketleft", "backslash", "bracketright",
    "asciicircum", "underscore", "grave", "a", "b",
    /*  70 */
    "c", "d", "e", "f", "g",
    "h", "i", "j", "k", "l",
    /*  80 */
    "m", "n", "o", "p", "q",
    "r", "s", "t", "u", "v",
    /*  90 */
    "w", "x", "y", "z", "braceleft",
    "bar", "braceright", "asciitilde", "Adieresis", "Aring",
    /* 100 */
    "Ccedilla", "Eacute", "Ntilde", "Odieresis", "Udieresis",
    "aacute", "agrave", "acircumflex", "adieresis", "atilde",
    /* 110 */
    "aring", "ccedilla", "eacute", "egrave", "ecircumflex",
    "edieresis", "iacute", "igrave", "icircumflex", "idieresis",
    /* 120 */
    "ntilde", "oacute", "ograve", "ocircumflex", "odieresis",
    "otilde", "uacute", "ugrave", "ucircumflex", "udieresis",
    /* 130 */
    "dagger", "degree", "cent", "sterling", "section",
    "bullet", "paragraph", "germandbls", "registered", "copyright",
    /* 140 */
    "trademark", "acute", "dieresis", "notequal", "AE",
    "Oslash", "infinity", "plusminus", "lessequal", "greaterequal",
    /* 150 */
    "yen", "mu", "partialdiff", "summation", "product",
    "pi", "integral", "ordfeminine", "ordmasculine", "Omega",
    /* 160 */
    "ae", "oslash", "questiondown", "exclamdown", "logicalnot",
    "radical", "florin", "approxequal", "Delta", "guillemotleft",
    /* 170 */
    "guillemotright", "ellipsis", "nbspace", "Agrave", "Atilde",
    "Otilde", "OE", "oe", "endash", "emdash",
    /* 180 */
    "quotedblleft", "quotedblright", "quoteleft", "quoteright", "divide",
    "lozenge", "ydieresis", "Ydieresis", "fraction", "currency",
    /* 190 */
    "guilsinglleft", "guilsinglright", "fi", "fl", "daggerdbl",
    "periodcentered", "quotesinglbase", "quotedblbase", "perthousand", "Acircumflex",
    /* 200 */
    "Ecircumflex", "Aacute", "Edieresis", "Egrave", "Iacute",
    "Icircumflex", "Idieresis", "Igrave", "Oacute", "Ocircumflex",
    /* 210 */
    "apple", "Ograve", "Uacute", "Ucircumflex", "Ugrave",
    "dotlessi", "circumflex", "tilde", "macron", "breve",
    /* 220 */
    "dotaccent", "ring", "cedilla", "hungarumlaut", "ogonek",
    "caron", "Lslash", "lslash", "Scaron", "scaron",
    /* 230 */
    "Zcaron", "zcaron", "brokenbar", "Eth", "eth",
    "Yacute", "yacute", "Thorn", "thorn", "minus",
    /* 240 */
    "multiply", "onesuperior", "twosuperior", "threesuperior", "onehalf",
    "onequarter", "threequarters", "franc", "Gbreve", "gbreve",
    /* 250 */
    "Idot", "Scedilla", "scedilla", "Cacute", "cacute",
    "Ccaron", "ccaron", "dmacron",
  };



  static TT_Error  Load_Format_20( TT_Post_20*  post20,
                                   PFace        input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort  n, num_glyphs, num_names;
    Byte    len;


    if ( ACCESS_Frame( 2L ) )
      return error;

    num_glyphs = GET_UShort();

    FORGET_Frame();

    /* UNDOCUMENTED!  The number of glyphs in this table can be smaller */
    /* than the value in the maxp table (cf. cyberbit.ttf).             */

    /* There already exist fonts which have more than 32768 glyph names */
    /* in this table, so the test for this threshold has been dropped.  */

    if ( num_glyphs > input->numGlyphs )
      return TT_Err_Invalid_Post_Table;

    post20->numGlyphs = num_glyphs;

    /* load the indices */

    if ( ALLOC_ARRAY( post20->glyphNameIndex, num_glyphs, TT_UShort ) )
      return error;

    if ( ACCESS_Frame( num_glyphs * 2L ) )
      goto Fail;

    for ( n = 0; n < num_glyphs; n++ )
    {
      post20->glyphNameIndex[n] = GET_UShort();
    }

    FORGET_Frame();

    /* compute number of names stored in table */
    num_names = 0;

    for ( n = 0; n < num_glyphs; n++ )
    {
      UShort  idx;


      idx = post20->glyphNameIndex[n];
      if ( idx >= 258 )
      {
        idx -= 257;
#if 0 /* FIXME This test has been dropped in Freetype 2. Why? */
        if ( idx > num_glyphs )
        {
          error = TT_Err_Invalid_Post_Table;
          goto Fail;
        }
#endif
        if ( idx > num_names )
          num_names = idx;
      }
    }

    post20->numNames = num_names;

    if ( num_names == 0 )                       /* nothing more to do */
      return TT_Err_Ok;

    /* now load the name strings */
    if ( ALLOC_ARRAY( post20->glyphNames, num_names, Char* ) )
      goto Fail;

    /* We must initialize the glyphNames array for proper */
    /* deallocation.                                      */
    /* FIXME  is it still really needed?                  */
    for ( n = 0; n < num_names; n++ )
      post20->glyphNames[n] = NULL;

    /* Now we can read the glyph names which are stored in */
    /* Pascal string format.                               */
    for ( n = 0; n < num_names; n++ )
    {
      if ( ACCESS_Frame( 1L ) )
        goto Fail1;

      len = GET_Byte();

      FORGET_Frame();

      if ( ALLOC_ARRAY( post20->glyphNames[n], len + 1, Char ) ||
           FILE_Read( post20->glyphNames[n], len ) )
        goto Fail1;

      /* we make a C string */
      post20->glyphNames[n][len] = '\0';
    }

    return TT_Err_Ok;


  Fail1:
    for ( n = 0; n < num_names; n++ )
      if ( post20->glyphNames[n] )
        FREE( post20->glyphNames[n] );

    FREE( post20->glyphNames );

  Fail:
    FREE( post20->glyphNameIndex );
    return error;
  }


  static TT_Error  Load_Format_25( TT_Post_25*  post25,
                                   PFace        input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort  n, num;


    if ( ACCESS_Frame( 2L ) )
      return error;

    /* UNDOCUMENTED!  This value appears only in the Apple TT specs. */
    num = GET_UShort();

    FORGET_Frame();

    if ( num > input->numGlyphs || num > 258 )
      return TT_Err_Invalid_Post_Table;

    post25->numGlyphs = num;

    if ( ALLOC_ARRAY( post25->offset, num, Char ) )
      return error;

    if ( ACCESS_Frame( num ) )
      goto Fail;

    for ( n = 0; n < num; n++ )
    {
      post25->offset[n] = GET_Char();

      /* We add 128 to the tests to avoid problems with negative */
      /* values for comparison.                                  */
      if ( n + ( post25->offset[n] + 128 ) > num + 128 ||
           n + ( post25->offset[n] + 128 ) < 128 )
      {
        FORGET_Frame();
        error = TT_Err_Invalid_Post_Table;
        goto Fail;
      }
    }

    FORGET_Frame();

    return TT_Err_Ok;


  Fail:
    FREE( post25->offset );
    return error;
  }


  static TT_Error  Post_Create( void*  ext,
                                PFace  face )
  {
    TT_Post*  post = (TT_Post*)ext;
    Long      table;


    /* by convention */
    if ( !post )
      return TT_Err_Ok;

    /* we store the start offset and the size of the subtable */
    table = TT_LookUp_Table( face, TTAG_post );
    post->offset = face->dirTables[table].Offset + 32L;
    post->length = face->dirTables[table].Length - 32L;
    post->loaded = FALSE;

    return TT_Err_Ok;
  }


  static TT_Error  Post_Destroy( void*  ext,
                                 PFace  face )
  {
    TT_Post*  post = (TT_Post*)ext;
    UShort    n;


    /* by convention */
    if ( !post )
      return TT_Err_Ok;

    if ( post->loaded )
    {
      switch ( face->postscript.FormatType )
      {
      case 0x00010000:          /* nothing to do */
        break;

      case 0x00020000:
        if ( post->p.post20.numNames )
        {
          for ( n = 0; n < post->p.post20.numNames; n++ )
            if ( post->p.post20.glyphNames[n] )
              FREE( post->p.post20.glyphNames[n] );
          FREE( post->p.post20.glyphNames );
        }
        FREE( post->p.post20.glyphNameIndex );
        break;

      case 0x00028000:
        FREE( post->p.post25.offset );
        break;

      case 0x00030000:          /* nothing to do */
        break;

#if 0
      case 0x00040000:
        break;
#endif

      default:
        ;                       /* invalid format, do nothing */
      }
    }

    return TT_Err_Ok;
  }


  FT_EXPORT_FUNC( TT_Error )
  TT_Init_Post_Extension( TT_Engine  engine )
  {
    PEngine_Instance  _engine = HANDLE_Engine( engine );

    TT_Error  error;


    if ( !_engine )
      return TT_Err_Invalid_Engine;

    error = TT_Register_Extension( _engine,
                                   POST_ID,
                                   sizeof ( TT_Post ),
                                   Post_Create,
                                   Post_Destroy );
    return error;
  }


/*******************************************************************
 *
 *  Function    :  TT_Load_PS_Names
 *
 *  Description :  Loads the PostScript Glyph Name subtable (if any).
 *
 *  Output :  error code
 *
 ******************************************************************/

  FT_EXPORT_FUNC( TT_Error )
  TT_Load_PS_Names( TT_Face   face,
                    TT_Post*  ppost )
  {
    PFace      faze = HANDLE_Face( face );
    TT_Error   error;
    TT_Stream  stream;
    TT_Post*   post;


    if ( !faze )
      return TT_Err_Invalid_Face_Handle;

    error = TT_Extension_Get( faze, POST_ID, (void**)&post );
    if ( error )
      return error;

    if ( USE_Stream( faze->stream, stream ) )
      return error;


    switch ( faze->postscript.FormatType )
    {
    case 0x00010000:
      error = TT_Err_Ok;            /* nothing to do */
      break;

    case 0x00020000:
      if ( FILE_Seek( post->offset ) )
        goto Fail;

      error = Load_Format_20( &post->p.post20, faze );
      break;

    case 0x00028000:                /* 2.5 in 16.16 format */
      if ( FILE_Seek( post->offset ) )
        goto Fail;

      error = Load_Format_25( &post->p.post25, faze );
      break;

    case 0x00030000:
      error = TT_Err_Ok;            /* nothing to do */
      break;

#if 0
    case 0x00040000:
      break;
#endif

    default:
      error = TT_Err_Invalid_Post_Table_Format;
      break;
    }

    if ( !error )
    {
      post->loaded = TRUE;
      *ppost = *post;
    }


  Fail:
    DONE_Stream( stream );

    return error;
  }


/*******************************************************************
 *
 *  Function    :  TT_Get_PS_Name
 *
 *  Description :  Gets the PostScript Glyph Name of a glyph.
 *
 *  Input  :  index     glyph index
 *            PSname    address of a string pointer.
 *                      Will be NULL in case of error; otherwise it
 *                      contains a pointer to the glyph name.
 *
 *                      You must not modify the returned string!
 *
 *  Output :  error code
 *
 ******************************************************************/

  FT_EXPORT_FUNC( TT_Error )
  TT_Get_PS_Name( TT_Face      face,
                  TT_UShort    index,
                  TT_String**  PSname )
  {
    PFace     faze = HANDLE_Face( face );
    TT_Error  error;
    TT_Post*  post;
    UShort    nameindex;


    if ( !faze )
      return TT_Err_Invalid_Face_Handle;

    if ( index >= faze->numGlyphs )
      return TT_Err_Invalid_Glyph_Index;

    error = TT_Extension_Get( faze, POST_ID, (void**)&post );
    if ( error )
      return error;


    *PSname = TT_Post_Default_Names[0];         /* default value */

    switch ( faze->postscript.FormatType )
    {
    case 0x00010000:
      if ( index < 258 )                        /* paranoid checking */
        *PSname = TT_Post_Default_Names[index];
      break;

    case 0x00020000:
      if ( index < post->p.post20.numGlyphs )
        nameindex = post->p.post20.glyphNameIndex[index];
      else
        break;

      if ( nameindex < 258 )
        *PSname = TT_Post_Default_Names[nameindex];
      else
        *PSname = (String*)post->p.post20.glyphNames[nameindex - 258];
      break;

    case 0x00028000:
      if ( index < post->p.post25.numGlyphs )   /* paranoid checking */
        *PSname = TT_Post_Default_Names[index + post->p.post25.offset[index]];
      break;

    case 0x00030000:
      break;                                    /* nothing to do */

#if 0
    case 0x00040000:
      break;
#endif

    default:
      ;                                         /* should never happen */
    }

    return TT_Err_Ok;
  }


/* END */
