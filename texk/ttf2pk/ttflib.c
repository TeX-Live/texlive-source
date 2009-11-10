/*
 *   ttflib.c
 *
 *   This file is part of the ttf2pk package.
 *
 *   Copyright 1997-1999, 2000 by
 *     Loyer Frederic <loyer@ensta.fr>
 *     Werner Lemberg <wl@gnu.org>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>       /* libc ANSI */
#include <ctype.h>

#include "pklib.h"      /* for the `byte' type */
#include "freetype.h"
#include "ttfenc.h"
#include "ttflib.h"
#include "errormsg.h"
#include "newobj.h"
#include "ttf2tfm.h"

#include "ftxpost.h"
#include "ftxopen.h"


#define Macintosh_platform 1
#define Macintosh_encoding 0

#define Microsoft_platform 3
#define Microsoft_Symbol_encoding 0
#define Microsoft_Unicode_encoding 1

#define SCRIPT_kana MAKE_TT_TAG('k', 'a', 'n', 'a')
#define SCRIPT_hani MAKE_TT_TAG('h', 'a', 'n', 'i')
#define SCRIPT_hang MAKE_TT_TAG('h', 'a', 'n', 'g')

#define LANGUAGE_JAN MAKE_TT_TAG('J', 'A', 'N', ' ')
#define LANGUAGE_CHN MAKE_TT_TAG('C', 'H', 'N', ' ')
#define LANGUAGE_KOR MAKE_TT_TAG('K', 'O', 'R', ' ')

#define FEATURE_vert MAKE_TT_TAG('v', 'e', 'r', 't')


TT_Engine   engine;
TT_Face     face;
TT_Instance instance;
TT_Glyph    glyph;
TT_CharMap  char_map;

TT_Outline         outline;
TT_Face_Properties properties;
TT_BBox            bbox;

TT_Post post;

TT_Raster_Map Bit, Bit2;
void          *Bitp, *Bit2p;

int dpi;
int ptsize;

int x_offset, y_offset;
int ppem;

TT_Big_Glyph_Metrics metrics;

TT_Matrix matrix1, matrix2;

TTO_GSUBHeader  gsub_;
TTO_GSUBHeader  *gsub;

TT_UShort in_string[2];
TTO_GSUB_String in, out;

Boolean has_gsub;


static void  
SetRasterArea(int quiet)
{
  int temp1_x, temp1_y, temp2_x, temp2_y;


  temp1_x = bbox.xMin / 64;         /* scaling F16.6 -> int */
  temp1_y = bbox.yMin / 64;
  temp2_x = (bbox.xMax + 63) / 64;
  temp2_y = (bbox.yMax + 63) / 64;

  x_offset = 5 - temp1_x;
  y_offset = 5 - temp1_y;

  if (!quiet)
    printf("  off = (%d, %d)", x_offset, y_offset);

#if 0
  x_offset = y_offset = 0;
#endif

  if (!quiet)
    printf("  bbox = (%d, %d) <->  (%d, %d)\n",
           temp1_x, temp1_y, temp2_x, temp2_y);

  Bit.rows = temp2_y - temp1_y + 10;
  Bit.width = temp2_x - temp1_x + 10;

  Bit.cols = (Bit.width + 7) / 8;       /* convert to # of bytes */
  Bit.flow = TT_Flow_Up;
  Bit.size = Bit.rows * Bit.cols;       /* number of bytes in buffer */

  /*
   *   We allocate one more row to have valid pointers for comparison
   *   purposes in pklib.c, making `gcc -fbounds-checking' happy.
   */

  if (Bitp)
    free(Bitp);
  Bitp = mymalloc(Bit.size + Bit.cols);
  Bit.bitmap = Bitp;

  Bit2 = Bit;

  if (Bit2p)
    free(Bit2p);
  Bit2p = mymalloc(Bit.size + Bit.cols);
  Bit2.bitmap = Bit2p;
}


static void
FlipBit(void)
{
  int y;
  char *p1, *p2;

  p1 = (char *)Bit.bitmap;
  p2 = (char *)Bit2.bitmap + Bit2.cols * (Bit2.rows - 1);

  for (y = 0; y < Bit.rows; y++)
  {
    memcpy(p2, p1, Bit.cols);
    p1 += Bit.cols;
    p2 -= Bit.cols;
  }
}


#if 0

static void
Output(TT_Raster_Map Bit)
{
  int x;
  int y;
  int i;

  char *p, b;


  p = Bit.bitmap;
  printf("====\n");

  for (y = 0; y < Bit.rows; y++)
  {
    printf("%3d:", y);
    for (x = 0; x < Bit.cols; x++)
    {
      b = *p++;
      for(i = 0x80; i; i >>= 1)
        printf((b & i) ? "x" : ".");
    }
    printf("\n");
  }
}

#endif /* 0 */


void
TTFopen(char *filename, Font *fnt, int new_dpi, int new_ptsize, Boolean quiet)
{
  unsigned short i, num_cmap;
  unsigned short cmap_plat;
  unsigned short cmap_enc;
  TT_Error error;

  TT_UShort script_index, language_index, feature_index;
  TT_UShort req_feature_index = 0xFFFF;


  dpi = new_dpi;
  ptsize = new_ptsize;

  if ((error = TT_Init_FreeType(&engine)))
    oops("Cannot initialize FreeType engine (error code = 0x%x).", error);

  if (fnt->PSnames)
    if ((error = TT_Init_Post_Extension(engine)))
      oops("Cannot initialize PS name support (error code = 0x%x).", error);

  if (fnt->rotate)
    if ((error = TT_Init_GSUB_Extension(engine)))
      oops("Cannot initialize GSUB support (error code = 0x%x).", error);

  /*
   *   Load face.
   */

  error = TT_Open_Face(engine, filename, &face);
  if (error)
    oops("Cannot open `%s'.", filename);

  /*
   *   Get face properties and allocate preloaded arrays.
   */

  TT_Get_Face_Properties(face, &properties);

  /*
   *   Now we try to open the proper font in a collection.
   */

  if (fnt->fontindex != 0)
  {
    if (properties.num_Faces == 1)
      warning("This isn't a TrueType collection.\n"
              "Parameter `Fontindex' is ignored.");
    else
    {
      TT_Close_Face(face);
      if ((error = TT_Open_Collection(engine, filename,
                                      fnt->fontindex, &face)))
        oops("Cannot open font %lu in TrueType Collection `%s'.",
             fnt->fontindex, filename);
    }
  }

  /*
   *   Create instance.
   */
  
  if ((error = TT_New_Instance(face, &instance)))
    oops("Cannot create instance for `%s' (error code = 0x%x).",
         filename, error);

  if ((error = TT_Set_Instance_Resolutions(instance, dpi, dpi)))
    oops("Cannot set device resolutions (error code = 0x%x).");

  if ((error = TT_Set_Instance_CharSize(instance, ptsize * 64)))
    oops("Cannot set character size (error code = 0x%x).", error);

  ppem = (dpi * ptsize + 36) / 72;

  if (!quiet)
    printf("dpi = %d, ptsize = %d, ppem = %d\n\n", dpi, ptsize, ppem);

  matrix1.xx = (TT_Fixed)(floor(fnt->efactor * 1024) * (1<<16)/1024);
  matrix1.xy = (TT_Fixed)(floor(fnt->slant * 1024) * (1<<16)/1024);
  matrix1.yx = (TT_Fixed)0;
  matrix1.yy = (TT_Fixed)(1<<16);

  if (fnt->rotate)
  {
    matrix2.xx = 0;
    matrix2.yx = 1L << 16;
    matrix2.xy = -matrix2.yx;
    matrix2.yy = matrix2.xx;
  }

  if ((error = TT_Set_Instance_Transform_Flags(
                 instance,
                 fnt->rotate ? 1 : 0,
                 fnt->efactor != 1.0 ? 1 : 0)))
    oops("Cannot set transform flags (error code = 0x%x).", error);

  /*
   *   Create glyph container.
   */

  if ((error = TT_New_Glyph(face, &glyph)))
    oops("Cannot create glyph container (error code = 0x%x).");

  if (fnt->PSnames != Only)
  {
    num_cmap = properties.num_CharMaps;
    for (i = 0; i < num_cmap; i++)
    {
      if ((error = TT_Get_CharMap_ID(face, i, &cmap_plat, &cmap_enc)))
        oops("Cannot query cmap (error code = 0x%x).", error);
      if (cmap_plat == fnt->pid && cmap_enc == fnt->eid)
        break;
    }
    if (i == num_cmap)
      oops("Invalid platform and/or encoding ID.");
  
    if ((error = TT_Get_CharMap(face, i, &char_map)))
      oops("Cannot load cmap (error code = 0x%x).", error);
  }

  if (fnt->PSnames)
  {
    if ((error = TT_Load_PS_Names(face, &post)))
      oops("Cannot load TrueType PS names (error code = 0x%x).", error);
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
    gsub = &gsub_;

    error = TT_Load_GSUB_Table(face, gsub, NULL);
    if (!error)
      has_gsub = True;
    else if (error != TT_Err_Table_Missing)
      warning("Cannot load GSUB table (error code = 0x%x).", error);
    else
      warning("No GSUB data available "
              "for vertical glyph presentation forms.");

    /* we check for the `vert' feature in Chinese, Japanese, and Korean */

    error = TT_GSUB_Select_Script(gsub,
                                  SCRIPT_kana,
                                  &script_index);
    if (error)
      goto check_hani;
    error = TT_GSUB_Select_Feature(gsub,
                                   FEATURE_vert,
                                   script_index,
                                   0xFFFF,
                                   &feature_index);
    if (error)
    {
      error = TT_GSUB_Select_Language(gsub,
                                      LANGUAGE_JAN,
                                      script_index,
                                      &language_index,
                                      &req_feature_index);
      if (error)
        goto check_hani;
      error = TT_GSUB_Select_Feature(gsub,
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
    error = TT_GSUB_Select_Script(gsub,
                                  SCRIPT_hani,
                                  &script_index);
    if (error)
      goto check_hang;
    error = TT_GSUB_Select_Feature(gsub,
                                   FEATURE_vert,
                                   script_index,
                                   0xFFFF,
                                   &feature_index);
    if (error)
    {
      error = TT_GSUB_Select_Language(gsub,
                                      LANGUAGE_CHN,
                                      script_index,
                                      &language_index,
                                      &req_feature_index);
      if (error)
        goto check_hang;
      error = TT_GSUB_Select_Feature(gsub,
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
    error = TT_GSUB_Select_Script(gsub,
                                  SCRIPT_hang,
                                  &script_index);
    if (error)
      goto Done;
    error = TT_GSUB_Select_Feature(gsub,
                                   FEATURE_vert,
                                   script_index,
                                   0xFFFF,
                                   &feature_index);
    if (error)
    {
      error = TT_GSUB_Select_Language(gsub,
                                      LANGUAGE_KOR,
                                      script_index,
                                      &language_index,
                                      &req_feature_index);
      if (error)
        goto Done;
      error = TT_GSUB_Select_Feature(gsub,
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
      TT_GSUB_Add_Feature(gsub, req_feature_index, ALL_GLYPHS);
    TT_GSUB_Add_Feature(gsub, feature_index, ALL_GLYPHS);

    in.length = 1;
    in.pos = 0;
    in.string = in_string;
    in.properties = NULL;

    out.pos = 0;
    out.allocated = 0;
    out.string = NULL;
    out.properties = NULL;
  }
}


static TT_Error
LoadTrueTypeChar(Font *fnt,
                 int idx,
                 Boolean hint,
                 Boolean quiet)
{
  TT_Error error;
  int flags;


  flags = TTLOAD_SCALE_GLYPH;
  if (hint)
    flags |= TTLOAD_HINT_GLYPH;

  error = TT_Load_Glyph(instance, glyph, idx, flags);
  if (!error)
    error = TT_Get_Glyph_Big_Metrics(glyph, &metrics);
  if (!error)
    error = TT_Get_Glyph_Outline(glyph, &outline);
  if (!error)
  {
    if (fnt->efactor != 1.0 || fnt->slant != 0.0 )
      TT_Transform_Outline(&outline, &matrix1);
    if (fnt->rotate)
      TT_Transform_Outline(&outline, &matrix2);
  }
  if (!error)
    error = TT_Get_Outline_BBox(&outline, &bbox); /* we need the non-
                                                     grid-fitted bbox */
  if (fnt->rotate)
    TT_Translate_Outline(&outline,
                         metrics.vertBearingY - bbox.xMin,
                         -fnt->y_offset * ppem * 64);
  if (!error)
    error = TT_Get_Outline_BBox(&outline, &bbox);
  if (!error)
    SetRasterArea(quiet);
  return error;
}


Boolean
TTFprocess(Font *fnt,
           long Code,
           byte **bitmap,
           int *width, int *height,
           int *hoff, int *voff,
           Boolean hinting,
           Boolean quiet)
{
  int Num;
  TT_Error error;


  if (!bitmap || !width || !height || !hoff || !voff)
    oops("Invalid parameter in call to TTFprocess()");

  if (Code >= 0x1000000)
    Num = Code & 0xFFFFFF;
  else
  {
    Num = TT_Char_Index(char_map, Code);
    if (has_gsub)
    {
      in_string[0] = Num;
      error = TT_GSUB_Apply_String(gsub, &in, &out);
      if (error && error != TTO_Err_Not_Covered)
        warning("Cannot get the vertical glyph form for glyph index %d.",
                Num);
      else
        Num = out.string[0];
    }
  }

  if ((error = LoadTrueTypeChar(fnt, Num, hinting, quiet)) == TT_Err_Ok)
  {
    memset(Bit.bitmap, 0, Bit.size);
    TT_Get_Glyph_Bitmap(glyph, &Bit, x_offset * 64, y_offset * 64);

    FlipBit();      
    *bitmap = Bit2.bitmap;
    *width = Bit2.width;
    *height = Bit2.rows;
    *hoff = x_offset;
    *voff = y_offset;
    /* *voff = Bit2.rows - y_offset;    */
    /* printf("%D %d\n", *hoff, *voff); */
    /* Output(Bit2);                    */
    return True;
  }
  else
    return False;
}


/*
 *   We collect first all glyphs addressed via the cmap.  Then we fill the
 *   array up with glyphs not in the cmap.
 *
 *   If PSnames is set to `Only', we get the first 256 glyphs which have
 *   names different from `.notdef', `.null', and `nonmarkingreturn'.
 *
 *   For nicer output, we return the glyph names in an encoding array.
 */

encoding *
TTFget_first_glyphs(Font *fnt, long *array)
{
  unsigned int i, j, Num;
  unsigned int index_array[257];     /* we ignore glyph index 0 */
  const char *n;
  encoding *e = (encoding *)mymalloc(sizeof (encoding));


  if (!array)
    oops("Invalid parameter in call to TTFget_first_glyphs()");

  for (i = 0; i < 257; i++)
    index_array[i] = 0;

  j = 0;
  if (fnt->PSnames != Only)
  {
    for (i = 0; i <= 0x16FFFF; i++)
    {
      Num = TT_Char_Index(char_map, i);
      if (Num < 0)
        oops("cmap mapping failure.");
      if (Num == 0)
        continue;
      if (Num <= 256)
        index_array[Num] = 1;

      if (fnt->PSnames) {
        char *tn;
        (void)TT_Get_PS_Name(face, Num, &tn);
        n = tn;
      }
      else
        n = code_to_adobename(i);
      if (strcmp(n, ".notdef") == 0)
        continue;
      if (strcmp(n, ".null") == 0)
        continue;
      if (strcmp(n, "nonmarkingreturn") == 0)
        continue;

      if (j < 256)
      {
        array[j] = i;
        e->vec[j] = n;
      }
      else
        return e;
      j++;
    }

    if (!fnt->PSnames)
    {
      for (i = 1; i < properties.num_Glyphs; i++)
      {
        if (index_array[i] == 0)
        {
          if (j < 256)
          {
            array[j] = i | 0x1000000;
            e->vec[j] = code_to_adobename(i | 0x1000000);
          }
          else
            return e;
          j++;
        }
      }
    }
  }
  else
  {
    for (i = 0; i < properties.num_Glyphs; i++)
    {
      char *n;


      (void)TT_Get_PS_Name(face, i, &n);

      if (strcmp(n, ".notdef") == 0)
        continue;
      if (strcmp(n, ".null") == 0)
        continue;
      if (strcmp(n, "nonmarkingreturn") == 0)
        continue;

      if (j < 256)
      {
        array[j] = i | 0x1000000;
        e->vec[j] = n;
      }
      else
        return e;
      j++;
    }
  }

  return NULL;              /* never reached */
}


/*
 *   This routine fills `array' with the subfont character codes;
 *   additionally, it tests for valid glyph indices.
 */

void
TTFget_subfont(Font *fnt, long *array)
{
  int i, j, Num;


  if (!fnt || !array)
    oops("Invalid parameter in call to TTFget_subfont()");

  for (i = 0; i <= 0xFF; i++)
  {
    j = fnt->sf_code[i];

    if (j < 0)
      array[i] = j;
    else
    {
      Num = TT_Char_Index(char_map, j);
      if (Num < 0)
        oops("cmap mapping failure.");
      else
        array[i] = j;
    }
  }
}


long
TTFsearch_PS_name(const char *name)
{
  unsigned int i;
  char *n;


  for (i = 0; i < properties.num_Glyphs; i++)
  {
    TT_Get_PS_Name(face, i, &n);
    if (strcmp(name, n) == 0)
      break;
  }

  if (i == properties.num_Glyphs)
    return -1L;
  else
    return (long)i;
}


/* end */
