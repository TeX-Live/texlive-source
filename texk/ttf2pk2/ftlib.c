/*
 *   ftlib.c
 *
 *   This file is part of the ttf2pk package.
 *
 *   Copyright 1997-1999, 2000, 2002, 2003 by
 *     Frederic Loyer <loyer@ensta.fr>
 *     Werner Lemberg <wl@gnu.org>
 *
 *   Copyright (C) 2012 by
 *     Peter Breitenlohner <tex-live@tug.org>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TRUETYPE_TABLES_H
#include FT_OPENTYPE_VALIDATE_H

#include "ttf2tfm.h"
#include "ttfenc.h"
#include "errormsg.h"
#include "newobj.h"
#include "ftlib.h"

#define Macintosh_platform 1
#define Macintosh_encoding 0

#define Microsoft_platform 3
#define Microsoft_Symbol_encoding 0
#define Microsoft_Unicode_encoding 1

#define TTAG_GSUB FT_MAKE_TAG('G', 'S', 'U', 'B')

#define SCRIPT_kana FT_MAKE_TAG('k', 'a', 'n', 'a')
#define SCRIPT_hani FT_MAKE_TAG('h', 'a', 'n', 'i')
#define SCRIPT_hang FT_MAKE_TAG('h', 'a', 'n', 'g')

#define LANGUAGE_JAN FT_MAKE_TAG('J', 'A', 'N', ' ')
#define LANGUAGE_CHN FT_MAKE_TAG('C', 'H', 'N', ' ')
#define LANGUAGE_KOR FT_MAKE_TAG('K', 'O', 'R', ' ')

#define FEATURE_vert FT_MAKE_TAG('v', 'e', 'r', 't')

FT_Library  engine;
FT_Face     face;

FT_Matrix   matrix1, matrix2;

int dpi, ptsize;

Boolean has_gsub;

TT_OS2 *os2;
TT_Postscript  *postscript;

FT_Byte *GSUB_table;
FT_ULong GSUB_length;
FT_ULong GSUB_ptr;
FT_ULong Script_List_ptr;
FT_ULong Feature_List_ptr;
FT_ULong Lookup_List_ptr;

static inline void
need (FT_ULong needed)
{
  if (GSUB_ptr + needed > GSUB_length)
    oops("GSUB: Unexpected end of table.");
}

static inline FT_Byte
get_Byte(void)
{
  return GSUB_table[GSUB_ptr++];
}

static inline signed char
get_SByte(void)
{
  return (signed char) GSUB_table[GSUB_ptr++];
}

static FT_UShort
get_UShort(void)
{
  unsigned int cc;
  cc = get_Byte() << 8;
  cc |= get_Byte();
  return (FT_UShort) cc;
}

static FT_Short
get_Short(void)
{
  int cc;
  cc = get_SByte() << 8;
  cc |= get_Byte();
  return (FT_Short) cc;
}

static FT_ULong
get_ULong(void)
{
  unsigned int cc;
  cc = get_Byte() << 24;
  cc |= get_Byte() << 16;
  cc |= get_Byte() << 8;
  cc |= get_Byte();
  return (FT_ULong) cc;
}

static FT_Long
get_Long(void)
{
  int cc;
  cc = get_SByte() << 24;
  cc |= get_Byte() << 16;
  cc |= get_Byte() << 8;
  cc |= get_Byte();
  return (FT_Long) cc;
}

#if 0
FT_UShort in_string[2];

TTO_GSUBHeader  gsub_;
TTO_GSUBHeader  *gsub;
TTO_GSUB_String in, out;
#endif

void
FTopen (char *filename, Font *fnt, Boolean do_tfm, Boolean quiet)
{
  FT_Error error;
  int i;
  unsigned short num_cmap, cmap_plat=0, cmap_enc=0;

#if 0
  FT_UShort script_index, language_index, feature_index;
  FT_UShort req_feature_index = 0xFFFF;
#endif

  if ((error = FT_Init_FreeType(&engine)))
    oops("Cannot initialize FreeType engine (error code = 0x%x).", error);

#if 0
  if (fnt->rotate)
    if ((error = FT_Init_GSUB_Extension(engine)))
      oops("Cannot initialize GSUB support (error code = 0x%x).", error);
#endif

  /*
   *   Load face.
   */
  if ((error = FT_New_Face(engine, filename, 0, &face)))
    oops("Cannot open `%s'.", filename);

  if (face->num_faces == 1)
  {
    if (fnt->fontindex != 0)
    {
      warning("This isn't a TrueType collection.\n"
              "Parameter `%s' is ignored.", do_tfm ? "-f" : "Fontindex");
      fnt->fontindex = 0;
    }
    fnt->fontindexparam = NULL;
  }
  else
  {
    if (fnt->fontindex != 0)
    {
      /*
       *   Now we try to open the proper font in a collection.
       */
      FT_Done_Face(face);
      if ((error = FT_New_Face(engine, filename,  (FT_Long)fnt->fontindex, &face)))
        oops("Cannot open font %lu in TrueType Collection `%s'.",
             fnt->fontindex, filename);
    }
  }

  if (do_tfm)
  {
    /*
     *   Get the OS/2 table.
     */
    if ((os2 = FT_Get_Sfnt_Table(face, ft_sfnt_os2)) == NULL)
      oops("Cannot find OS/2 table for `%s'.", filename);

    /*
     *   Get the Postscript table.
     */
    if ((postscript = FT_Get_Sfnt_Table(face, ft_sfnt_post)) == NULL)
      oops("Cannot find Postscript table for `%s'.", filename);
  }

  if ((error = FT_Set_Char_Size(face, ptsize * 64, ptsize * 64, dpi, dpi)))
    oops("Cannot set character size (error code = 0x%x).", error);

  matrix1.xx = (FT_Fixed)(floor(fnt->efactor * 1024) * (1<<16)/1024);
  matrix1.xy = (FT_Fixed)(floor(fnt->slant * 1024) * (1<<16)/1024);
  matrix1.yx = (FT_Fixed)0;
  matrix1.yy = (FT_Fixed)(1<<16);

  if (fnt->rotate)
  {
    matrix2.xx = 0;
    matrix2.yx = 1L << 16;
    matrix2.xy = -matrix2.yx;
    matrix2.yy = matrix2.xx;
  }

  if (do_tfm)
  {
    fnt->units_per_em = face->units_per_EM;
    fnt->fixedpitch = postscript->isFixedPitch;
    fnt->italicangle = postscript->italicAngle / 65536.0;
    fnt->xheight = os2->sxHeight * 1000 / fnt->units_per_em;
  }

  if (fnt->PSnames != Only)
  {
    num_cmap = face->num_charmaps;
    for (i = 0; i < num_cmap; i++)
    {
      cmap_plat=face->charmaps[i]->platform_id;
      cmap_enc=face->charmaps[i]->encoding_id;
      if (cmap_plat == fnt->pid && cmap_enc == fnt->eid)
        break;
    }
    if (i == num_cmap)
    {
      fprintf(stderr, "%s: ERROR: Invalid platform and/or encoding ID.\n",
              progname);
      if (num_cmap == 1)
        fprintf(stderr, "  The only valid PID/EID pair is");
      else
        fprintf(stderr, "  Valid PID/EID pairs are:\n");
      for (i = 0; i < num_cmap; i++)
      {
        cmap_plat=face->charmaps[i]->platform_id;
        cmap_enc=face->charmaps[i]->encoding_id;
        fprintf(stderr, "    (%i,%i)\n", cmap_plat, cmap_enc);
      }
      fprintf(stderr, "\n");
      exit(1);
    }
  
    if ((error = FT_Set_Charmap(face, face->charmaps[i])))
      oops("Cannot load cmap (error code = 0x%x).", error);
  }

  if (fnt->PSnames)
  {
    if (!FT_HAS_GLYPH_NAMES(face))
      oops("This font does not support PS names.");
  }
  else if (cmap_plat == Microsoft_platform &&
           cmap_enc == Microsoft_Unicode_encoding)
    set_encoding_scheme(encUnicode, fnt);
  else if (cmap_plat == Macintosh_platform &&
           cmap_enc == Macintosh_encoding)
    set_encoding_scheme(encMac, fnt);
  else
    set_encoding_scheme(encFontSpecific, fnt);

  if (fnt->rotate)
  {
    if ((error = FT_Load_Sfnt_Table(face, TTAG_GSUB, 0, NULL, &GSUB_length)))
    {
      warning("No GSUB data available "
              "for vertical glyph presentation forms.");
      return;
    }
    GSUB_table = mymalloc(GSUB_length);
    if ((error = FT_Load_Sfnt_Table(face, TTAG_GSUB, 0, GSUB_table, &GSUB_length)))
    {
      warning("Cannot load GSUB table (error code = 0x%x).", error);
      return;
    }
    need(10);
    if (get_ULong() != 0x00010000)
      oops("GSUB: Bad version.");
    Script_List_ptr = get_UShort();
    warning("GSUB: Script List at 0x%04x.", Script_List_ptr);
    Feature_List_ptr = get_UShort();
    warning("GSUB: Feature List at 0x%04x.", Feature_List_ptr);
    Lookup_List_ptr = get_UShort();
    warning("GSUB: Lookup List at 0x%04x.", Lookup_List_ptr);
    GSUB_ptr = Script_List_ptr;
    need(2);
    {
      FT_UShort Script_Count = get_UShort();
      need (6 * Script_Count);
      for (i = 0; i < Script_Count; i++)
      {
        FT_Tag tag = get_ULong();
        FT_ULong Script_ptr = Script_List_ptr + get_UShort();
        warning("Script[%d] '%c%c%c%c' at 0x%04x.", i,
                tag >> 24, (tag >> 16) & 0xff, (tag >> 8) & 0xff, tag & 0xff,
                Script_ptr); 
      }
    }
    has_gsub = True;
//    exit(1);
#if 0
    gsub = &gsub_;

    error = FT_Load_GSUB_Table(face, gsub, NULL);
    if (!error)
      has_gsub = True;
    else if (error != FT_Err_Table_Missing)
      warning("Cannot load GSUB table (error code = 0x%x).", error);
    else
      warning("No GSUB data available "
              "for vertical glyph presentation forms.");

    /* we check for the `vert' feature in Chinese, Japanese, and Korean */

    error = FT_GSUB_Select_Script(gsub,
                                  SCRIPT_kana,
                                  &script_index);
    if (error)
      goto check_hani;
    error = FT_GSUB_Select_Feature(gsub,
                                   FEATURE_vert,
                                   script_index,
                                   0xFFFF,
                                   &feature_index);
    if (error)
    {
      error = FT_GSUB_Select_Language(gsub,
                                      LANGUAGE_JAN,
                                      script_index,
                                      &language_index,
                                      &req_feature_index);
      if (error)
        goto check_hani;
      error = FT_GSUB_Select_Feature(gsub,
                                     FEATURE_vert,
                                     script_index,
                                     language_index,
                                     &feature_index);
      if (error)
        goto check_hani;
      else
        goto Done;
    }
    else
      goto Done;

  check_hani:
    error = FT_GSUB_Select_Script(gsub,
                                  SCRIPT_hani,
                                  &script_index);
    if (error)
      goto check_hang;
    error = FT_GSUB_Select_Feature(gsub,
                                   FEATURE_vert,
                                   script_index,
                                   0xFFFF,
                                   &feature_index);
    if (error)
    {
      error = FT_GSUB_Select_Language(gsub,
                                      LANGUAGE_CHN,
                                      script_index,
                                      &language_index,
                                      &req_feature_index);
      if (error)
        goto check_hang;
      error = FT_GSUB_Select_Feature(gsub,
                                     FEATURE_vert,
                                     script_index,
                                     language_index,
                                     &feature_index);
      if (error)
        goto check_hang;
      else
        goto Done;
    }
    else
      goto Done;

  check_hang:
    error = FT_GSUB_Select_Script(gsub,
                                  SCRIPT_hang,
                                  &script_index);
    if (error)
      goto Done;
    error = FT_GSUB_Select_Feature(gsub,
                                   FEATURE_vert,
                                   script_index,
                                   0xFFFF,
                                   &feature_index);
    if (error)
    {
      error = FT_GSUB_Select_Language(gsub,
                                      LANGUAGE_KOR,
                                      script_index,
                                      &language_index,
                                      &req_feature_index);
      if (error)
        goto Done;
      error = FT_GSUB_Select_Feature(gsub,
                                     FEATURE_vert,
                                     script_index,
                                     language_index,
                                     &feature_index);
    }

  Done:
    if (error)
    {
      warning("There is no data for vertical typesetting in GSUB table.");
      has_gsub = False;
    }

    if (req_feature_index != 0xFFFF)
      FT_GSUB_Add_Feature(gsub, req_feature_index, ALL_GLYPHS);
    FT_GSUB_Add_Feature(gsub, feature_index, ALL_GLYPHS);

    in.length = 1;
    in.pos = 0;
    in.string = in_string;
    in.properties = NULL;

    out.pos = 0;
    out.allocated = 0;
    out.string = NULL;
    out.properties = NULL;
#endif
  }
}

int
Get_Vert (int Num)
{
  warning("GSUB: Looking for vertical form of glyph %d.", Num);
#if 0
      in_string[0] = Num;
      error = FT_GSUB_Apply_String(gsub, &in, &out);
      if (error && error != OTL_Err_Not_Covered)
        warning("Cannot get the vertical glyph form for glyph index %d.",
                Num);
      else
        Num = out.string[0];
#endif
  return Num;
}
