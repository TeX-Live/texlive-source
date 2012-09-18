/*
 *   ttfaux.c
 *
 *   This file is part of the ttf2pk package.
 *
 *   Copyright 1997-1999, 2000, 2002, 2003 by
 *     Frederic Loyer <loyer@ensta.fr>
 *     Werner Lemberg <wl@gnu.org>
 */

#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "freetype.h"
#include "ftxkern.h"     /* we use installed FreeType package headers */
#include "ftxpost.h"
#include "ftxopen.h"

#include "ttf2tfm.h"
#include "newobj.h"
#include "ligkern.h"
#include "ttfenc.h"
#include "tfmaux.h"
#include "errormsg.h"
#include "ttfaux.h"
#include "filesrch.h"


#define Macintosh_platform 1
#define Macintosh_encoding 0

#define Microsoft_platform 3
#define Microsoft_Unicode_encoding 1

#define SCRIPT_kana MAKE_TT_TAG('k', 'a', 'n', 'a')
#define SCRIPT_hani MAKE_TT_TAG('h', 'a', 'n', 'i')
#define SCRIPT_hang MAKE_TT_TAG('h', 'a', 'n', 'g')

#define LANGUAGE_JAN MAKE_TT_TAG('J', 'A', 'N', ' ')
#define LANGUAGE_CHN MAKE_TT_TAG('C', 'H', 'N', ' ')
#define LANGUAGE_KOR MAKE_TT_TAG('K', 'O', 'R', ' ')

#define FEATURE_vert MAKE_TT_TAG('v', 'e', 'r', 't')


char *real_ttfname;

TT_Engine   engine;
TT_Face     face;
TT_Instance instance;
TT_Glyph    glyph;
TT_Outline  outline;
TT_CharMap  char_map;
TT_Matrix   matrix1, matrix2;

TT_Big_Glyph_Metrics metrics;
TT_Face_Properties   properties;
TT_BBox              bbox;

TT_Kerning directory;
TT_Post    post;

TTO_GSUBHeader  gsub_;
TTO_GSUBHeader  *gsub;

Boolean has_gsub;



static void
readttf_kern(Font *fnt)
{
  register kern *nk;
  register ttfinfo *ti_l, *ti_r;
  TT_Kern_0_Pair* pairs0;
  TT_Error error;
  unsigned int i, j;

  
  if ((error = TT_Get_Kerning_Directory(face, &directory)))
    oops("Cannot get kerning directory (error code = 0x%x).", error);
  
  if (directory.nTables == 0)
    return;

  for (i = 0; i < directory.nTables; i++)
  {
    if ((error = TT_Load_Kerning_Table(face, i)))
      oops("Cannot load kerning table (error code = 0x%x).", error);

    switch (directory.tables[i].format)
    {
    case 0:
      pairs0 = directory.tables[i].t.kern0.pairs;
      for (j = 0; j < directory.tables[i].t.kern0.nPairs; j++, pairs0++)
      {
        ti_l = findglyph(pairs0->left, fnt->charlist);
        if (ti_l == NULL)
          continue;
        ti_r = findglyph(pairs0->right, fnt->charlist);
        if (ti_r == NULL)
          continue;
        nk = newkern();
        nk->succ = ti_r->adobename;
        nk->delta = transform(pairs0->value * 1000 / fnt->units_per_em, 0,
                              fnt->efactor, fnt->slant);
        nk->next = ti_l->kerns;
        ti_l->kerns = nk;
      }
      return;   /* we stop after the first format 0 kerning table */

    default:
      break;
    }
  }
  return;
}


void
readttf(Font *fnt, Boolean quiet, Boolean only_range)
{
  TT_Error error;
  ttfinfo *ti, *Ti;
  long Num, index;
  unsigned int i, j;
  long k, max_k;
  unsigned short num_cmap;
  unsigned short cmap_plat, cmap_enc;
  int index_array[257];

  static Boolean initialized = False;

  TT_UShort in_string[2];
  TTO_GSUB_String in, out;

  TT_UShort script_index, language_index, feature_index;
  TT_UShort req_feature_index = 0xFFFF;


  /*
   *   We allocate a placeholder boundary and the `.notdef' character.
   */

  if (!only_range)
  {
    ti = newchar(fnt);
    ti->charcode = -1;
    ti->adobename = ".notdef";

    ti = newchar(fnt);
    ti->charcode = -1;
    ti->adobename = "||"; /* boundary character name */
  }

  /*
   *   Initialize FreeType engine.
   */

  if (!initialized)
  {
    if ((error = TT_Init_FreeType(&engine)))
      oops("Cannot initialize engine (error code = 0x%x).", error);

    if ((error = TT_Init_Kerning_Extension(engine)))
      oops("Cannot initialize kerning (error code = 0x%x).", error);

    if (fnt->PSnames)
      if ((error = TT_Init_Post_Extension(engine)))
        oops("Cannot initialize PS name support (error code = 0x%x).", error);

    if (fnt->rotate)
      if ((error = TT_Init_GSUB_Extension(engine)))
        oops("Cannot initialize GSUB support (error code = 0x%x).", error);

    /*
     *   Load face.
     */

    real_ttfname = TeX_search_ttf_file(&(fnt->ttfname));
    if (!real_ttfname)
      oops("Cannot find `%s'.", fnt->ttfname);

    if ((error = TT_Open_Face(engine, real_ttfname, &face)))
      oops("Cannot open `%s'.", real_ttfname);

    /*
     *   Get face properties and allocate preload arrays.
     */

    TT_Get_Face_Properties(face, &properties);

    /*
     *   Now we try to open the proper font in a collection.
     */

    if (fnt->fontindex != 0)
    {
      if (properties.num_Faces == 1)
      {
        warning("This isn't a TrueType collection.\n"
                "Parameter `-f' is ignored.");
        fnt->fontindex = 0;
        fnt->fontindexparam = NULL;
      }
      else
      {
        TT_Close_Face(face);
        if ((error = TT_Open_Collection(engine, real_ttfname,
                                        fnt->fontindex, &face)))
          oops("Cannot open font %lu in TrueType Collection `%s'.",
               fnt->fontindex, real_ttfname);
      }
    }

    /*
     *   Create instance.
     */

    if ((error = TT_New_Instance(face, &instance)))
      oops("Cannot create instance for `%s' (error code = 0x%x).",
           real_ttfname, error);

    /*
     *   We use a dummy glyph size of 10pt.
     */

    if ((error = TT_Set_Instance_CharSize(instance, 10 * 64)))
      oops("Cannot set character size (error code = 0x%x).", error);

    matrix1.xx = (TT_Fixed)(floor(fnt->efactor * 1024) * (1L<<16)/1024);
    matrix1.xy = (TT_Fixed)(floor(fnt->slant * 1024) * (1L<<16)/1024);
    matrix1.yx = (TT_Fixed)0;
    matrix1.yy = (TT_Fixed)(1L<<16);

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
      oops("Cannot create glyph container (error code = 0x%x).", error);

    fnt->units_per_em = properties.header->Units_Per_EM;
    fnt->fixedpitch = properties.postscript->isFixedPitch;
    fnt->italicangle = properties.postscript->italicAngle / 65536.0;
#if TT_FREETYPE_MINOR >= 5
    fnt->xheight = properties.os2->sxHeight * 1000 / fnt->units_per_em;
#endif

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
      {
        fprintf(stderr, "%s: ERROR: Invalid platform and/or encoding ID.\n",
                progname);
        if (num_cmap == 1)
          fprintf(stderr, "  The only valid PID/EID pair is");
        else
          fprintf(stderr, "  Valid PID/EID pairs are:\n");
        for (i = 0; i < num_cmap; i++)
        {
          TT_Get_CharMap_ID(face, i, &cmap_plat, &cmap_enc);
          fprintf(stderr, "    (%i,%i)\n", cmap_plat, cmap_enc);
        }
        fprintf(stderr, "\n");
        exit(1);
      }

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

    initialized = True;
  }

  if (!quiet)
  {
    if (only_range)
      printf("\n\n%s:\n", fnt->fullname);
    printf("\n");
    printf("Glyph  Code   Glyph Name                ");
    printf("Width    llx   lly      urx   ury\n");
    printf("----------------------------------------");
    printf("---------------------------------\n");
  }

  /*
   *   We load only glyphs with a valid cmap entry.  Nevertheless, for
   *   the default mapping, we use the first 256 glyphs addressed by
   *   ascending code points, followed by glyphs not in the cmap.
   *
   *   If we compute a range, we take the character codes given in
   *   the fnt->sf_code array.
   *
   *   If the -N flag is set, no cmap is used at all.  Instead, the
   *   first 256 glyphs (with a valid PS name) are used for the default
   *   mapping.
   */

  if (!only_range)
    for (i = 0; i < 257; i++)
      index_array[i] = 0;
  else
    for (i = 0; i < 256; i++)
      fnt->inencptrs[i] = 0;

  j = 0;
  if (fnt->PSnames == Only)
    max_k = properties.num_Glyphs - 1;
  else
    max_k = only_range ? 0xFF : 0x16FFFF;

  for (k = 0; k <= max_k; k++)
  {
    const char *an;


    if (fnt->PSnames != Only)
    {
      if (only_range)
      {
        index = fnt->sf_code[k];
        if (index < 0)
          continue;
        j = k;
      }
      else
        index = k;

      Num = TT_Char_Index(char_map, index);

      /* now we try to get a vertical glyph form */

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

      if (Num < 0)
        oops("Failure on cmap mapping from %s.", fnt->ttfname);
      if (Num == 0)
        continue;
      if (!only_range)
        if (Num <= 256)
          index_array[Num] = 1;
    }
    else
    {
      Num = k;
      index = 0;
    }

    error = TT_Load_Glyph(instance, glyph, Num, 0);
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
    if (!error)
    {
      if (fnt->PSnames) {
        char *tn;
        (void)TT_Get_PS_Name(face, Num, &tn);
        an = tn;
      }
      else
        an = code_to_adobename(index);

      /* ignore characters not usable for typesetting with TeX */

      if (strcmp(an, ".notdef") == 0)
        continue;
      if (strcmp(an, ".null") == 0)
        continue;
      if (strcmp(an, "nonmarkingreturn") == 0)
        continue;

      ti = newchar(fnt);
      ti->charcode = index;
      ti->glyphindex = Num;
      ti->adobename = an;
      ti->llx = bbox.xMin * 1000 / fnt->units_per_em;
      ti->lly = bbox.yMin * 1000 / fnt->units_per_em;
      ti->urx = bbox.xMax * 1000 / fnt->units_per_em;
      ti->ury = bbox.yMax * 1000 / fnt->units_per_em;

      ti->fntnum = fnt->subfont_num;

      /*
       *   We must now shift the rotated character both horizontally
       *   and vertically.  The vertical amount is 25% by default.
       */

      if (fnt->rotate)
      {
        ti->llx += (metrics.vertBearingY - bbox.xMin) *
                     1000 / fnt->units_per_em;
        ti->lly -= 1000 * fnt->y_offset;
        ti->urx += (metrics.vertBearingY - bbox.xMin) *
                     1000 / fnt->units_per_em;
        ti->ury -= 1000 * fnt->y_offset;
      }

      /*
       *   We need to avoid negative heights or depths.  They break accents
       *   in math mode, among other things.
       */

      if (ti->lly > 0)
        ti->lly = 0;
      if (ti->ury < 0)
        ti->ury = 0;
      if (fnt->rotate)
        ti->width = metrics.vertAdvance * 1000 / fnt->units_per_em;
      else
        ti->width = transform(metrics.horiAdvance * 1000 / fnt->units_per_em,
                              0, fnt->efactor, fnt->slant);

      if (!quiet)
        printf("%5ld  %05lx  %-25s %5d  % 5d,% 5d -- % 5d,% 5d\n",
               Num, index, ti->adobename,
               ti->width,
               ti->llx, ti->lly, ti->urx, ti->ury);

      if (j < 256)
      {
        fnt->inencptrs[j] = ti;
        ti->incode = j;
      }
      j++;
    }
  }

  /*
   *   Now we load glyphs without a cmap entry, provided some slots are
   *   still free -- we skip this if we have to compute a range or use
   *   PS names.
   */

  if (!only_range && !fnt->PSnames)
  {
    for (i = 1; i <= properties.num_Glyphs; i++)
    {
      const char *an;


      if (index_array[i] == 0)
      {
        error = TT_Load_Glyph(instance, glyph, i, 0);
        if (!error)
          error = TT_Get_Glyph_Big_Metrics(glyph, &metrics);
        if (!error)
          error = TT_Get_Glyph_Outline(glyph, &outline);
        if (!error)
          error = TT_Get_Outline_BBox(&outline, &bbox);
        if (!error)
        {
          an = code_to_adobename(i | 0x1000000);

          ti = newchar(fnt);
          ti->charcode = i | 0x1000000;
          ti->glyphindex = i;
          ti->adobename = an;
          ti->llx = bbox.xMin * 1000 / fnt->units_per_em;
          ti->lly = bbox.yMin * 1000 / fnt->units_per_em;
          ti->urx = bbox.xMax * 1000 / fnt->units_per_em;
          ti->ury = bbox.yMax * 1000 / fnt->units_per_em;

          if (ti->lly > 0)
            ti->lly = 0;
          if (ti->ury < 0)
            ti->ury = 0;
          ti->width = transform(metrics.horiAdvance*1000 / fnt->units_per_em,
                                0, fnt->efactor, fnt->slant);

          if (!quiet)
            printf("%5d         %-25s %5d  % 5d,% 5d -- % 5d,% 5d\n",
                   i, ti->adobename,
                   ti->width,
                   ti->llx, ti->lly, ti->urx, ti->ury);

          if (j < 256)
          {
            fnt->inencptrs[j] = ti;
            ti->incode = j;
          }
          else
            break;
          j++;
        }
      }
    }
  }

  /* Finally, we construct a `Germandbls' glyph if necessary */

  if (!only_range)
  {
    if (NULL == findadobe("Germandbls", fnt->charlist) &&
        NULL != (Ti = findadobe("S", fnt->charlist)))
    {
      pcc *np, *nq;


      ti = newchar(fnt);
      ti->charcode = properties.num_Glyphs | 0x1000000;
      ti->glyphindex = properties.num_Glyphs;
      ti->adobename = "Germandbls";
      ti->width = Ti->width << 1;
      ti->llx = Ti->llx;
      ti->lly = Ti->lly;
      ti->urx = Ti->width + Ti->urx;
      ti->ury = Ti->ury;
      ti->kerns = Ti->kerns;

      np = newpcc();
      np->partname = "S";
      nq = newpcc();
      nq->partname = "S";
      nq->xoffset = Ti->width;
      np->next = nq;
      ti->pccs = np;
      ti->constructed = True;

      if (!quiet)
        printf("*            %-25s %5d  % 5d,% 5d -- % 5d,% 5d\n",
               ti->adobename,
               ti->width,
               ti->llx, ti->lly, ti->urx, ti->ury);
    }
  }

  /* kerning between subfonts isn't available */
  if (!only_range)
    readttf_kern(fnt);
}


/* end */
