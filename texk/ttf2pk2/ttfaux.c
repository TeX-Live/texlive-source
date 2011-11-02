/*
 *   ttfaux.c
 *
 *   This file is part of the ttf2pk package.
 *
 *   Copyright 1997-1999, 2000, 2002 by
 *     Frederic Loyer <loyer@ensta.fr>
 *     Werner Lemberg <wl@gnu.org>
 */

#include <math.h>
#include <string.h>
#include <stdlib.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include FT_TYPE1_TABLES_H
#include FT_TRUETYPE_TABLES_H
#include FT_BBOX_H

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

#define SCRIPT_kana MAKE_FT_TAG('k', 'a', 'n', 'a')
#define SCRIPT_hani MAKE_FT_TAG('h', 'a', 'n', 'i')
#define SCRIPT_hang MAKE_FT_TAG('h', 'a', 'n', 'g')

#define LANGUAGE_JAN MAKE_FT_TAG('J', 'A', 'N', ' ')
#define LANGUAGE_CHN MAKE_FT_TAG('C', 'H', 'N', ' ')
#define LANGUAGE_KOR MAKE_FT_TAG('K', 'O', 'R', ' ')

#define FEATURE_vert MAKE_FT_TAG('v', 'e', 'r', 't')


char *real_ttfname;

FT_Library  engine;
FT_Face     face;
//FT_Instance instance;
FT_GlyphSlot    glyph;
FT_Outline  outline;
FT_CharMap  char_map;
FT_Matrix   matrix1, matrix2;

int flags = FT_LOAD_DEFAULT|FT_LOAD_NO_SCALE;

#if 0
FT_Big_Glyph_Metrics metrics;
FT_Face_Properties   properties;
#endif
FT_BBox              bbox;

TT_Postscript  *postscript;
#if 0
TTO_GSUBHeader  gsub_;
TTO_GSUBHeader  *gsub;
#endif

Boolean has_gsub;


static void
readttf_kern(Font *fnt)
{
  kern *nk;
  ttfinfo *ti, *tj;
  FT_Error error;
  FT_Vector akerning;

  for (ti = fnt->charlist; ti; ti = ti->next)
  {
    kern head;
    kern *tail = &head;
    head.next = NULL;
    for (tj = fnt->charlist; tj; tj = tj->next)
    {
      if ((error = FT_Get_Kerning(face, ti->glyphindex, tj->glyphindex,
                                  FT_KERNING_UNSCALED, &akerning)))
        oops("Cannot get kerning vector (error code = 0x%x).", error);

      if (akerning.x)
      {
        nk = newkern();
        nk->succ = tj->adobename;
        nk->delta = transform(akerning.x * 1000 / fnt->units_per_em, 0,
                              fnt->efactor, fnt->slant);
        /* We append the new kern to reproduce the old .vpl file */
        tail->next = nk;
        tail = nk;
#if 0
        nk->next = ti->kerns;
        ti->kerns = nk;
#endif
      }
    }
    ti->kerns = head.next;
  }
}


void
readttf(Font *fnt, Boolean quiet, Boolean only_range)
{
  FT_Error error;
  ttfinfo *ti, *Ti;
  long Num, index;
  unsigned int i, j;
  long k, max_k;
  unsigned short num_cmap;
  unsigned short cmap_plat=0, cmap_enc=0;
  int index_array[257];

  static Boolean initialized = False;

#if 0
  FT_UShort in_string[2];
#if 0
  TTO_GSUB_String in, out;
#endif

  FT_UShort script_index, language_index, feature_index;
  FT_UShort req_feature_index = 0xFFFF;
#endif


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
    if ((error = FT_Init_FreeType(&engine)))
      oops("Cannot initialize engine (error code = 0x%x).", error);
#if 0
    if ((error = FT_Init_Kerning_Extension(engine)))
      oops("Cannot initialize kerning (error code = 0x%x).", error);

    if (fnt->PSnames)
      if ((error = FT_Init_Post_Extension(engine)))
        oops("Cannot initialize PS name support (error code = 0x%x).", error);

    if (fnt->rotate)
      if ((error = FT_Init_GSUB_Extension(engine)))
        oops("Cannot initialize GSUB support (error code = 0x%x).", error);
#endif

    /*
     *   Load face.
     */

    real_ttfname = TeX_search_ttf_file(&(fnt->ttfname));
    if (!real_ttfname)
      oops("Cannot find `%s'.", fnt->ttfname);

    if ((error = FT_New_Face(engine, real_ttfname, 0,&face)))
      oops("Cannot open `%s'.", real_ttfname);

    /*
     *   Get face properties and allocate preload arrays.
     */

#if 0
    FT_Get_Face_Properties(face, &properties);
#endif

    /*
     *   Now we try to open the proper font in a collection.
     */

    if (fnt->fontindex != 0)
    {
      if (face->num_faces == 1)
      {
        warning("This isn't a TrueType collection.\n"
                "Parameter `-f' is ignored.");
        fnt->fontindex = 0;
        fnt->fontindexparam = NULL;
      }
      else
      {
#if 0
        FT_Close_Face(face);
        if ((error = FT_Open_Collection(engine, real_ttfname,
                                        fnt->fontindex, &face)))
          oops("Cannot open font %lu in TrueType Collection `%s'.",
               fnt->fontindex, real_ttfname);
#endif
      }
    }

    /*
     *   Create instance.
     */

#if 0
    if ((error = FT_New_Instance(face, &instance)))
      oops("Cannot create instance for `%s' (error code = 0x%x).",
           real_ttfname, error);
#endif

    /*
     *   Get the Postscript table.
     */

    if ((postscript = FT_Get_Sfnt_Table(face, ft_sfnt_post)) == NULL)
      oops("Cannot find Postscript table for `%s'.", real_ttfname);

    /*
     *   We use a dummy glyph size of 10pt.
     */

#if 1
    //if ((error = FT_Set_Instance_CharSize(instance, 10 * 64)))
    if ((error = FT_Set_Char_Size(face, 10 * 64,10 * 64, 92,92)))
      oops("Cannot set character size (error code = 0x%x).", error);
#endif

    matrix1.xx = (FT_Fixed)(floor(fnt->efactor * 1024) * (1L<<16)/1024);
    matrix1.xy = (FT_Fixed)(floor(fnt->slant * 1024) * (1L<<16)/1024);
    matrix1.yx = (FT_Fixed)0;
    matrix1.yy = (FT_Fixed)(1L<<16);

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
      oops("Cannot create glyph container (error code = 0x%x).", error);
#endif

    fnt->units_per_em = face->units_per_EM;
    fnt->fixedpitch = postscript->isFixedPitch;
    fnt->italicangle = postscript->italicAngle / 65536.0;

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
        oops("Cannot load PS names.");
    }
    else if (cmap_plat == Microsoft_platform &&
             cmap_enc == Microsoft_Unicode_encoding)
      set_encoding_scheme(encUnicode, fnt);
    else if (cmap_plat == Macintosh_platform &&
             cmap_enc == Macintosh_encoding)
      set_encoding_scheme(encMac, fnt);
    else
      set_encoding_scheme(encFontSpecific, fnt);

#if 0
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
#endif

    initialized = True;
  }

  if (!quiet)
  {
    if (only_range)
      printf("\n\n%s:\n", fnt->fullname);
    printf("\n");
    printf("Glyph  Code   Glyph Name                ");
    printf("Width  llx    lly      urx    ury\n");
    printf("---------------------------------------");
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
    max_k = face->num_glyphs - 1;
  else
    max_k = only_range ? 0xFF : 0x16FFFF;

  for (k = 0; k <= max_k; k++)
  {
    char buff[128];
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

      Num = FT_Get_Char_Index(face, index);

      /* now we try to get a vertical glyph form */

#if 0
      if (has_gsub)
      {
        in_string[0] = Num;
        error = FT_GSUB_Apply_String(gsub, &in, &out);
        if (error && error != TTO_Err_Not_Covered)
          warning("Cannot get the vertical glyph form for glyph index %d.",
                  Num);
        else
          Num = out.string[0];
      }
#endif 

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

    error = FT_Load_Glyph(face, Num, flags);
#if 0
    if (!error)
      error = FT_Get_Glyph_Big_Metrics(glyph, &metrics);
    if (!error)
      error = FT_Get_Glyph_Outline(glyph, &outline);
#endif
    if (!error)
    {
      if (fnt->efactor != 1.0 || fnt->slant != 0.0 )
        FT_Outline_Transform(&face->glyph->outline, &matrix1);
      if (fnt->rotate)
        FT_Outline_Transform(&face->glyph->outline, &matrix2);
    }
    if (!error)
      error = FT_Outline_Get_BBox(&face->glyph->outline, &bbox); /* we need the non-
                                                       grid-fitted bbox */
    if (!error)
    {
      if (fnt->PSnames)
      {
        (void)FT_Get_Glyph_Name(face, Num, buff, 128);
        an = newstring(buff);
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

      /*
       *   We must now shift the rotated character both horizontally
       *   and vertically.  The vertical amount is 25% by default.
       */

      if (fnt->rotate)
      {
        ti->llx += (face->glyph->metrics.vertBearingY - bbox.xMin) *
                     1000 / fnt->units_per_em;
        ti->lly -= 1000 * fnt->y_offset;
        ti->urx += (face->glyph->metrics.vertBearingY - bbox.xMin) *
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
        ti->width = face->glyph->metrics.vertAdvance * 1000 / fnt->units_per_em;
      else
        ti->width = transform(face->glyph->metrics.horiAdvance * 1000 / fnt->units_per_em,
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
    for (i = 1; i <= face->num_glyphs; i++)
    {
      const char *an;


      if (index_array[i] == 0)
      {
        error = FT_Load_Glyph(face, i, flags);
#if 0
        if (!error)
          error = FT_Get_Glyph_Big_Metrics(glyph, &metrics);
        if (!error)
          error = FT_Get_Glyph_Outline(glyph, &outline);
        if (!error)
          error = FT_Get_Outline_BBox(&outline, &bbox);
#endif
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
          ti->width = transform(face->glyph->metrics.horiAdvance*1000 / fnt->units_per_em,
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
      ti->charcode = face->num_glyphs | 0x1000000;
      ti->glyphindex = face->num_glyphs;
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
