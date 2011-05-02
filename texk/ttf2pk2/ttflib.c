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
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include FT_GLYPH_H
#include FT_TYPE1_TABLES_H
#include FT_BBOX_H
#include "ttfenc.h"
#include "ttflib.h"
#include "errormsg.h"
#include "newobj.h"
#include "ttf2tfm.h"


#define Macintosh_platform 1
#define Macintosh_encoding 0

#define Microsoft_platform 3
#define Microsoft_Symbol_encoding 0
#define Microsoft_Unicode_encoding 1

#define SCRIPT_kana MAKE_FT_TAG('k', 'a', 'n', 'a')
#define SCRIPT_hani MAKE_FT_TAG('h', 'a', 'n', 'i')
#define SCRIPT_hang MAKE_FT_TAG('h', 'a', 'n', 'g')

#define LANGUAGE_JAN MAKE_FT_TAG('J', 'A', 'N', ' ')
#define LANGUAGE_CHN MAKE_FT_TAG('C', 'H', 'N', ' ')
#define LANGUAGE_KOR MAKE_FT_TAG('K', 'O', 'R', ' ')

#define FEATURE_vert MAKE_FT_TAG('v', 'e', 'r', 't')


FT_Library	engine;
FT_Face		face;
//FT_Instance instance;
FT_GlyphSlot	glyph;
FT_CharMap	char_map;

//FT_Outline	outline;
//FT_Glyph	outline;
//FT_Face_Properties	properties;
FT_BBox		bbox;

//FT_Post post;

FT_Bitmap Bit, Bit2;
void          *Bitp, *Bit2p;

int dpi;
int ptsize;

int x_offset, y_offset;
int ppem;

//FT_Big_Glyph_Metrics face;

FT_Matrix matrix1, matrix2;

//OTL_GSUBHeader  gsub_;
//OTL_GSUBHeader  *gsub;

FT_UShort in_string[2];
//OTL_GSUB_String in, out;

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

  Bit.pitch = (Bit.width + 7) / 8;       /* convert to # of bytes */
//  Bit.flow = FT_Flow_Up;
//  Bit.size = Bit.rows * Bit.pitch;       /* number of bytes in buffer */

  /*
   *   We allocate one more row to have valid pointers for comparison
   *   purposes in pklib.c, making `gcc -fbounds-checking' happy.
   */

  if (Bitp)
    free(Bitp);
  Bitp = mymalloc(Bit.rows*Bit.pitch + Bit.pitch);
  Bit.buffer = Bitp;

  Bit2 = Bit;

  if (Bit2p)
    free(Bit2p);
  Bit2p = mymalloc(Bit.rows*Bit.pitch + Bit.pitch);
  Bit2.buffer = Bit2p;
}

#if 0
static void
FlipBit(void)
{
  int y;
  char *p1, *p2;

  p1 = (char *)Bit.buffer;
  p2 = (char *)Bit2.buffer + Bit2.pitch * (Bit2.rows - 1);

  for (y = 0; y < Bit.rows; y++)
  {
    memcpy(p2, p1, Bit.pitch);
    p1 += Bit.pitch;
    p2 -= Bit.pitch;
  }
}

#endif

static void
CopyBit(FT_Bitmap *b2,FT_Bitmap *b1)
{
  int y;
  char *p1, *p2;

  p1 = (char *)b1->buffer; /* source */
  p2 = (char *)b2->buffer; /* dest */

  for (y = 0; y < b1->rows; y++)
  {
    memcpy(p2, p1, b1->pitch);
    p1 += b1->pitch;
    p2 += b2->pitch;
  }
}

#if 0

static void
Output(FT_Bitmap Bit)
{
  int x;
  int y;
  int i;

  char *p, b;


  p = Bit.buffer;
  printf("====\n");

  for (y = 0; y < Bit.rows; y++)
  {
    printf("%3d:", y);
    for (x = 0; x < Bit.pitch; x++)
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
  unsigned short cmap_plat=0;
  unsigned short cmap_enc=0;
  FT_Error error;

#if 0
  FT_UShort script_index, language_index, feature_index;
  FT_UShort req_feature_index = 0xFFFF;
#endif

  dpi = new_dpi;
  ptsize = new_ptsize;

  if ((error = FT_Init_FreeType(&engine)))
    oops("Cannot initialize FreeType engine (error code = 0x%x).", error);

#if 0
  if (fnt->PSnames)
    if ((error = FT_Init_Post_Extension(engine)))
      oops("Cannot initialize PS name support (error code = 0x%x).", error);

  if (fnt->rotate)
    if ((error = FT_Init_GSUB_Extension(engine)))
      oops("Cannot initialize GSUB support (error code = 0x%x).", error);
#endif

  error = FT_New_Face(engine, filename, 0,&face);
    if (error)
      oops("Cannot open `%s'.", filename);
  if (fnt->fontindex != 0)
  {
    if (face->num_faces == 1)
      warning("This isn't a TrueType collection.\n"
              "Parameter `Fontindex' is ignored.");
    else
    {
  /*
   *   Load face.
   */
#if 0
      FT_Close_Face(face);
      if ((error = FT_Open_Collection(engine, filename,
                                      fnt->fontindex, &face)))
        oops("Cannot open font %lu in TrueType Collection `%s'.",
             fnt->fontindex, filename);
#endif
    }
  }
//  error= FT_Has_PS_Glyph_Names(face);
//  if (error)
  if (!FT_HAS_GLYPH_NAMES(face)) 
     oops("This font does not support PS name (error code = 0x%x).", error);

  /*
   *   Create instance.
   */
  
//  if ((error = FT_New_Instance(face, &instance)))
//    oops("Cannot create instance for `%s' (error code = 0x%x).",
//         filename, error);

//  if ((error = FT_Set_Instance_Resolutions(instance, dpi, dpi)))
//    oops("Cannot set device resolutions (error code = 0x%x).");

//  if ((error = FT_Set_Instance_CharSize(instance, ptsize * 64)))
//    oops("Cannot set character size (error code = 0x%x).", error);
  if ((error = FT_Set_Char_Size(face, ptsize * 64, ptsize * 64, dpi,dpi)))
    oops("Cannot set character size (error code = 0x%x).", error);

  ppem = (dpi * ptsize + 36) / 72;

  if (!quiet)
    printf("dpi = %d, ptsize = %d, ppem = %d\n\n", dpi, ptsize, ppem);

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

#if 0
  if ((error = FT_Set_Instance_Transform_Flags(
                 instance,
                 fnt->rotate ? 1 : 0,
                 fnt->efactor != 1.0 ? 1 : 0)))
    oops("Cannot set transform flags (error code = 0x%x).", error);
#endif

  /*
   *   Create glyph container.
   */

#if 0
  if ((error = FT_New_Glyph(face, &glyph)))
    oops("Cannot create glyph container (error code = 0x%x).");
#endif

  if (fnt->PSnames != Only)
  {
    num_cmap = face->num_charmaps;
    for (i = 0; i < num_cmap; i++)
    {
      cmap_plat=face->charmaps[i]->platform_id;
      cmap_enc=face->charmaps[i]->encoding_id;
#if 0
      if ((error = FT_Get_CharMap_ID(face, i, &cmap_plat, &cmap_enc)))
        oops("Cannot query cmap (error code = 0x%x).", error);
#endif
      if (cmap_plat == fnt->pid && cmap_enc == fnt->eid)
        break;
    }
    if (i == num_cmap)
      oops("Invalid platform and/or encoding ID.");
  
    if ((error = FT_Set_Charmap(face, face->charmaps[i])))
      oops("Cannot load cmap (error code = 0x%x).", error);
  }

#if 0
  if (fnt->PSnames)
  {
    if ((error = FT_Load_PS_Names(face, &post)))
      oops("Cannot load TrueType PS names (error code = 0x%x).", error);
  }
#endif
  if (cmap_plat == Microsoft_platform &&
           cmap_enc == Microsoft_Unicode_encoding)
    set_encoding_scheme(encUnicode, fnt);
  else if (cmap_plat == Macintosh_platform &&
           cmap_enc == Macintosh_encoding)
    set_encoding_scheme(encMac, fnt);
  else
    set_encoding_scheme(encFontSpecific, fnt);

#ifdef FREETYPE1
  if (fnt->rotate)
  {
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
  }
#endif // FREETYPE1
}


static FT_Error
LoadTrueTypeChar(Font *fnt,
                 int idx,
                 Boolean hint,
                 Boolean quiet)
{
  FT_Error error;
  int flags;


//  flags = FT_LOAD_SCALE_GLYPH;
  flags = FT_LOAD_DEFAULT;
  if (hint)
    flags |= FT_LOAD_FORCE_AUTOHINT;

  //error = FT_Load_Glyph(instance, glyph, idx, flags);
  error = FT_Load_Glyph(face, idx, flags);
//  if (!error)
//    error = FT_Get_Glyph_Big_Metrics(glyph, &metrics);
//  if (!error)
//    error = FT_Get_Glyph_Outline(glyph, &outline);
//    error = FT_Get_Glyph(glyph, &outline);
//    error = FT_Get_Glyph(face->glyph, &glyph);
  if (!error)
  {
    if (fnt->efactor != 1.0 || fnt->slant != 0.0 )
//      FT_Transform_Outline(&outline, &matrix1);
      FT_Outline_Transform(&face->glyph->outline, &matrix1);
    if (fnt->rotate)
      FT_Outline_Transform(&face->glyph->outline, &matrix2);
  }
  if (!error)
    error = FT_Outline_Get_BBox(&face->glyph->outline, &bbox); /* we need the non-
                                                     grid-fitted bbox */
  if (fnt->rotate)
    FT_Outline_Translate(&face->glyph->outline,
                         face->glyph->metrics.vertBearingY - bbox.xMin,
                         -fnt->y_offset * ppem * 64);
  if (!error)
    error = FT_Outline_Get_BBox(&face->glyph->outline, &bbox);
  if (!error) {
    FT_Outline_Get_CBox(&face->glyph->outline, &bbox);
    /* for the case of BBox != CBox */
    SetRasterArea(quiet);
  }
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
  FT_Error error;


  if (!bitmap || !width || !height || !hoff || !voff)
    oops("Invalid parameter in call to TTFprocess()");

  if (Code >= 0x1000000)
    Num = Code & 0xFFFFFF;
  else
  {
//    Num = FT_Char_Index(char_map, Code);
    Num = FT_Get_Char_Index(face, Code);
#if 0
    if (has_gsub)
    {
      in_string[0] = Num;
      error = FT_GSUB_Apply_String(gsub, &in, &out);
      if (error && error != OTL_Err_Not_Covered)
        warning("Cannot get the vertical glyph form for glyph index %d.",
                Num);
      else
        Num = out.string[0];
    }
#endif
  }

  if ((error = LoadTrueTypeChar(fnt, Num, hinting, quiet)) == FT_Err_Ok)
  {
    memset(Bit.buffer, 0, Bit.rows*Bit.pitch);
    //FT_Get_Glyph_Bitmap(glyph, &Bit, x_offset * 64, y_offset * 64);
    //FT_Glyph_To_Bitmap(glyph, &Bit, x_offset * 64, y_offset * 64);
    //error = FT_Outline_Get_Bitmap(engine,&face->glyph->outline, &Bit);
    error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_MONO);
    if (error) {
        warning("Cannot Render to Bitmap");
	return False;
    }

    //fprintf(stderr,"(%d,%d) ->",Bit.width,Bit.rows);
    //fprintf(stderr," (%d,%d)\n",face->glyph->bitmap.width,face->glyph->bitmap.rows);
    CopyBit(&Bit, &face->glyph->bitmap);      

#if 0
    FlipBit();      
    *bitmap = Bit2.buffer;
    *width = Bit2.width;
    *height = Bit2.rows;
#else
    *bitmap = Bit.buffer;
    *width = Bit.width;
    *height = Bit.rows;
#endif
#if 1
    *hoff = x_offset;
    *voff = y_offset;
#else
    *hoff = face->glyph->bitmap_left;
    *voff = face->glyph->bitmap_top;
#endif
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
  unsigned int i, j, Num=0;
  unsigned int index_array[257];     /* we ignore glyph index 0 */
  char buff[128];
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
      Num = FT_Get_Char_Index(face, i);
     // Num = FT_Char_Index(char_map, i);
      if (Num < 0)
        oops("cmap mapping failure.");
      if (Num == 0)
        continue;
      if (Num <= 256)
        index_array[Num] = 1;

      if (fnt->PSnames)
      {
	  (void)FT_Get_Glyph_Name(face,Num,buff,128);
	  n = newstring(buff);
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
      for (i = 1; i < face->num_glyphs; i++)
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
    for (i = 0; i < face->num_glyphs; i++)
    {
      (void)FT_Get_Glyph_Name(face,Num,buff,128);
      n = newstring(buff);

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
      //Num = FT_Char_Index(char_map, j);
      Num = FT_Get_Char_Index(face, j);
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
  char buff[128];


  for (i = 0; i < face->num_glyphs; i++)
  {
    FT_Get_Glyph_Name(face, i, buff, 128);
    if (strcmp(name, buff) == 0)
      break;
  }

  if (i == face->num_glyphs)
    return -1L;
  else
    return (long)i;
}


/* end */
