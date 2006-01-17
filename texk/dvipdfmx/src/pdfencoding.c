/*  $Header: /home/cvsroot/dvipdfmx/src/pdfencoding.c,v 1.5 2005/07/21 08:23:46 hirata Exp $
    
    This is dvipdfmx, an eXtended version of dvipdfm by Mark A. Wicks.

    Copyright (C) 2002 by Jin-Hwan Cho and Shunsaku Hirata,
    the dvipdfmx project team <dvipdfmx@project.ktug.or.kr>
    
    Copyright (C) 1998, 1999 by Mark A. Wicks <mwicks@kettering.edu>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
*/

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include "mem.h"
#include "error.h"
#include "dpxutil.h"

#include "pdfparse.h"
#include "pdfobj.h"

#include "dpxfile.h"

#include "pdfencoding.h"

#define _ENCODINGS_C_
#include "asl_charset.h"
#undef  _ENCODINGS_C_

static int      is_ASL_charset (char **encoding);
static pdf_obj *make_encoding_differences (char **encoding, char **baseenc);

static unsigned char verbose = 0;

static const char *MacRomanEncoding[256];
static const char *MacExpertEncoding[256];
static const char *WinAnsiEncoding[256];
static const char *StandardEncoding[256];
static const char *ISOLatin1Encoding[256];

void
pdf_encoding_set_verbose (void)
{
  verbose++;
}

/*
 * ident:  File name, e.g., 8a.enc.
 * name:   Name of encoding, StandardEncoding, TeXBase1Encoding, ...
 * glyphs: List of encoded glyphs (name).
 * flags:
 *   IS_PREDEFINED:
 *     Encoding is one of the MacRomanEncoding, MacExpertEncoding, and
 *     WinAnsiEncoding.
 *   IS_ASL_CHARSET:
 *     Encoded glyphs are only form Adobe Standard Latin Character Set.
 *     Fonts only uses encodings of this flag set can be "nonsymbolic".
 */
#define FLAG_IS_PREDEFINED  (1 << 0)
#define FLAG_IS_ASL_CHARSET (1 << 1)

typedef struct pdf_encoding
{
  char     *ident;

  char     *enc_name;
  int       flags;
  char     *glyphs[256];
  char      is_used[256];

  pdf_obj  *tounicode;
  char      accessible[256]; /* Flag indicating if glyphs
                              * are accessible in Unicode.
                              */

  pdf_obj  *resource;
  pdf_obj  *reference;
} pdf_encoding;

static void
pdf_init_encoding_struct (pdf_encoding *encoding)
{
  ASSERT(encoding);

  encoding->ident    = NULL;

  encoding->enc_name = NULL;

  memset(encoding->glyphs,  0, 256*sizeof(char *));
  memset(encoding->is_used, 0, 256);

  memset(encoding->accessible, 0, 256);
  encoding->tounicode = NULL;

  encoding->resource  = NULL;
  encoding->reference = NULL;

  encoding->flags     = 0;

  return;
}

static void
pdf_flush_encoding (pdf_encoding *encoding)
{
  ASSERT(encoding);

  if (encoding->resource)
    pdf_release_obj(encoding->resource);
  if (encoding->reference)
    pdf_release_obj(encoding->reference);
  if (encoding->tounicode) {
    pdf_obj *tounicode;

    tounicode = pdf_create_ToUnicode_CMap(encoding->enc_name,
                                          encoding->glyphs,
                                          encoding->is_used);
    if (tounicode) {
      pdf_add_stream(encoding->tounicode,
                     pdf_stream_dataptr(tounicode),
                     pdf_stream_length(tounicode));
      pdf_release_obj(tounicode);
    }
    pdf_release_obj(encoding->tounicode);
    encoding->tounicode = NULL;
  }

  encoding->resource  = NULL;
  encoding->reference = NULL;

  return;
}

static void
pdf_clean_encoding_struct (pdf_encoding *encoding)
{
  int   code;

  ASSERT(encoding);

  if (encoding->reference)
    ERROR("Object not flushed.");
  if (encoding->resource)
    ERROR("Object not flushed.");

  if (encoding->tounicode)
    pdf_release_obj(encoding->tounicode);
  if (encoding->ident)
    RELEASE(encoding->ident);
  if (encoding->enc_name)
    RELEASE(encoding->enc_name);

  encoding->ident    = NULL;
  encoding->enc_name = NULL;

  for (code = 0; code < 256; code++) {
    if (encoding->glyphs[code])
      RELEASE(encoding->glyphs[code]);
    encoding->glyphs[code] = NULL;
  }
  encoding->ident    = NULL;
  encoding->enc_name = NULL;

  return;
}

/*
 * The original dvipdfm describes as:
 *
 *  Some software doesn't like BaseEncoding key (e.g., FastLane) 
 *  so this code is commented out for the moment.  It may reemerge in the
 *  future
 *
 * and the line for BaseEncoding is commented out.
 *
 * I'm not sure why this happens. But maybe BaseEncoding key causes problems
 * when the font is Symbol font or TrueType font.
 */

static void
pdf_encoding_set_encoding (pdf_encoding *encoding,
                           char **encoding_vec, const char *baseenc_name)
{
  char   **baseenc_vec = NULL;
  pdf_obj *differences;
  int      code;

  ASSERT(encoding);
  ASSERT(encoding_vec);

  if (encoding->reference) {
    WARN("Object already have a label... flushing");
    pdf_flush_encoding(encoding);
  }

  encoding->flags = 0;
  if (!encoding->resource) {
    encoding->resource = pdf_new_dict();
    pdf_add_dict(encoding->resource,
                 pdf_new_name("Type"), pdf_new_name("Encoding"));
  }

  for (code = 0; code < 256; code++) {
    if (encoding->glyphs[code])
      RELEASE(encoding->glyphs[code]);
    if (!encoding_vec[code] ||
        !strcmp(encoding_vec[code], ".notdef"))
      encoding->glyphs[code] = NULL;
    else {
      encoding->glyphs[code] = NEW(strlen(encoding_vec[code])+1, char);
      strcpy(encoding->glyphs[code], encoding_vec[code]);
    }
  }

  if (baseenc_name) {
    if (!strcmp(baseenc_name, "WinAnsiEncoding"))
      baseenc_vec = (char **) WinAnsiEncoding;
    else if (!strcmp(baseenc_name, "MacRomanEncoding"))
      baseenc_vec = (char **) MacRomanEncoding;
    else if (!strcmp(baseenc_name, "MacExpertEncoding"))
      baseenc_vec = (char **) MacExpertEncoding;
    else
      ERROR("Unknown encoding \"%s\".", baseenc_name);
  } else {
    baseenc_vec = NULL;
  }

  if (is_ASL_charset(encoding->glyphs)) {
    encoding->flags |= FLAG_IS_ASL_CHARSET;
    /* Dvipdfmx default setting. */
    if (!baseenc_name) {
      baseenc_vec  = (char **) WinAnsiEncoding; 
      baseenc_name = (char *) "WinAnsiEncoding";
    }
  }

  if (baseenc_name) {
    pdf_add_dict(encoding->resource,
                 pdf_new_name("BaseEncoding"), pdf_new_name(baseenc_name));
  }
  differences = make_encoding_differences(encoding->glyphs, baseenc_vec);
  if (differences) {
    pdf_add_dict(encoding->resource, pdf_new_name("Differences"),  differences);
  }

  return;
}

static int CDECL
glycmp (const void *pv1, const void *pv2)
{
  char *v1, *v2;

  v1 = (char *) pv1;
  v2 = *((char **) pv2);

  return strcmp(v1, v2);
}

static int
is_ASL_charset (char **enc_vec)
{
  int   code;

  for (code = 0; code < 256; code++) {
    if (enc_vec[code] && strcmp(enc_vec[code], ".notdef")) {
      if (!bsearch(enc_vec[code],
                   ASL_Charset, ASL_CHARSET_MAX, sizeof(char *), glycmp))
        return 0;
    }
  }

  return 1;
}

static pdf_obj *
make_encoding_differences (char **enc_vec, char **baseenc)
{
  pdf_obj *differences = NULL;
  int      code, count = 0;
  int      skipping = 1;

  ASSERT(enc_vec);

  /*
   *  Write all entries (except .notdef) if baseenc is unknown.
   */
  differences = pdf_new_array();
  for (code = 0; code < 256; code++) {
    /* We skip ".notdef". Any character code mapped to ".notdef"
     * glyph should not be used in the document.
     */
    if (!enc_vec[code] || !strcmp(".notdef", enc_vec[code]))
      skipping = 1;
    else if (!baseenc || !baseenc[code] ||
             strcmp(baseenc[code], enc_vec[code]) != 0) {
      /*
       * Difference found.
       */
      if (skipping)
        pdf_add_array(differences, pdf_new_number(code));
      pdf_add_array(differences,   pdf_new_name(enc_vec[code]));
      skipping = 0;
      count++;
    } else
      skipping = 1;
  }

  /*
   * No difference found. Some PDF viewers can't handle differences without
   * any differences. We return NULL.
   */
  if (count == 0) {
    pdf_release_obj(differences);
    differences = NULL;
  }

  return differences;
}

static int
load_encoding_file (pdf_encoding *encoding, const char *filename)
{
  FILE    *fp;
  pdf_obj *encoding_array = NULL;
  char    *wbuf, *p, *endptr;
  char    *enc_vec[256];
  int      code, fsize;

  if (!encoding || !filename)
    return -1;

  fp = DPXFOPEN(filename, DPX_RES_TYPE_ENC);
  if (!fp)
    return -1;
  /*
   * file_size do seek_end witout saving current position and
   * do rewind.
   */
  fsize = file_size(fp);

  wbuf = NEW(fsize + 1, char); 
  fread(wbuf, sizeof(char), fsize, fp);
  DPXFCLOSE(fp);

  p        = wbuf;
  endptr   = wbuf + fsize;
  p[fsize] = '\0';

  skip_white(&p, endptr);

  /*
   * Skip comment lines.
   */
  while (p < endptr && p[0] == '%') {
    skip_line (&p, endptr);
    skip_white(&p, endptr);
  }
  if (p[0] == '/') {
    pdf_obj *tmp;

    tmp = parse_pdf_name(&p, endptr);

    encoding->enc_name = NEW(strlen(pdf_name_value(tmp))+1, char);
    strcpy(encoding->enc_name, pdf_name_value(tmp));
    pdf_release_obj(tmp);
  }

  skip_white(&p, endptr);
  encoding_array = parse_pdf_array(&p, endptr);
  RELEASE(wbuf);
  if (!encoding_array) {
    return -1;
  }

  for (code = 0; code < 256; code++) {
    enc_vec[code] = pdf_name_value(pdf_get_array(encoding_array, code));
  }
  pdf_encoding_set_encoding(encoding, enc_vec, NULL);
  pdf_release_obj(encoding_array);

  return 0;
}

#define CHECK_ID(n) do { \
  if ((n) < 0 || (n) >= enc_cache.count) { \
     ERROR("Invalid encoding id: %d", (n)); \
  } \
} while (0)

#define CACHE_ALLOC_SIZE 16u

struct {
  int           count;
  int           capacity;
  pdf_encoding *encodings;
} enc_cache = {
  0, 0, NULL
};

void
pdf_init_encodings (void)
{
  pdf_encoding *encoding;
  int           code;

  enc_cache.count     = 0;
  enc_cache.capacity  = 3;
  enc_cache.encodings = NEW(enc_cache.capacity, pdf_encoding);

  /*
   * PDF Predefined Encodings
   */
  encoding = &enc_cache.encodings[0];
  pdf_init_encoding_struct(encoding);
  encoding->ident = NEW(strlen("WinAnsiEncoding")+1, char);
  strcpy(encoding->ident, "WinAnsiEncoding");
  encoding->enc_name  = NEW(strlen("WinAnsiEncoding")+1, char);
  strcpy(encoding->enc_name, "WinAnsiEncoding");
  encoding->flags    = (FLAG_IS_PREDEFINED|FLAG_IS_ASL_CHARSET);
  encoding->resource = pdf_new_name("WinAnsiEncoding");
  for (code = 0; code < 256; code++) {
    if (!WinAnsiEncoding[code] ||
        !strcmp(WinAnsiEncoding[code], ".notdef"))
      encoding->glyphs[code] = NULL;
    else {
      encoding->glyphs[code] = NEW(strlen(WinAnsiEncoding[code])+1, char);
      strcpy(encoding->glyphs[code], WinAnsiEncoding[code]);
    }
  }

  encoding = &enc_cache.encodings[1];
  pdf_init_encoding_struct(encoding);
  encoding->ident = NEW(strlen("MacRomanEncoding")+1, char);
  strcpy(encoding->ident, "MacRomanEncoding");
  encoding->enc_name  = NEW(strlen("MacRomanEncoding")+1, char);
  strcpy(encoding->enc_name, "MacRomanEncoding");
  encoding->flags    = (FLAG_IS_PREDEFINED|FLAG_IS_ASL_CHARSET);
  encoding->resource = pdf_new_name("MacRomanEncoding");
  for (code = 0; code < 256; code++) {
    if (!MacRomanEncoding[code] ||
        !strcmp(MacRomanEncoding[code], ".notdef"))
      encoding->glyphs[code] = NULL;
    else {
      encoding->glyphs[code] = NEW(strlen(MacRomanEncoding[code])+1, char);
      strcpy(encoding->glyphs[code], MacRomanEncoding[code]);
    }
  }

  encoding = &enc_cache.encodings[2];
  pdf_init_encoding_struct(encoding);
  encoding->ident = NEW(strlen("MacExpertEncoding")+1, char);
  strcpy(encoding->ident, "MacExpertEncoding");
  encoding->enc_name  = NEW(strlen("MacExpertEncoding")+1, char);
  strcpy(encoding->enc_name, "MacExpertEncoding");
  encoding->flags    = FLAG_IS_PREDEFINED;
  encoding->resource = pdf_new_name("MacExpertEncoding");
  for (code = 0; code < 256; code++) {
    if (!MacExpertEncoding[code] ||
        !strcmp(MacExpertEncoding[code], ".notdef"))
      encoding->glyphs[code] = NULL;
    else {
      encoding->glyphs[code] = NEW(strlen(MacExpertEncoding[code])+1, char);
      strcpy(encoding->glyphs[code], MacExpertEncoding[code]);
    }
  }

  enc_cache.count = 3;

  return;
}

void
pdf_close_encodings (void)
{
  int  enc_id;

  if (enc_cache.encodings) {
    for (enc_id = 0; enc_id < enc_cache.count; enc_id++) {
      pdf_encoding *encoding;

      encoding = &enc_cache.encodings[enc_id];
      if (encoding) {
        pdf_flush_encoding(encoding);
        pdf_clean_encoding_struct(encoding);
      }
    }
    RELEASE(enc_cache.encodings);
  }
  enc_cache.encodings = NULL;
  enc_cache.count     = 0;
  enc_cache.capacity  = 0;
}

int
pdf_encoding_findresource (const char *enc_name)
{
  int           enc_id;
  pdf_encoding *encoding;

  ASSERT(enc_name);
  for (enc_id = 0; enc_id < enc_cache.count; enc_id++) {
    encoding = &enc_cache.encodings[enc_id];
    if (encoding->ident &&
        !strcmp(enc_name, encoding->ident))
      return enc_id;
    else if (encoding->enc_name &&
             !strcmp(enc_name, encoding->enc_name))
      return enc_id;
  }

  if (verbose) {
    MESG("(Encoding:%s", enc_name);
  }

  enc_id   = enc_cache.count;
  if (enc_cache.count >= enc_cache.capacity) {
    enc_cache.capacity += 16;
    enc_cache.encodings = RENEW(enc_cache.encodings,
                                enc_cache.capacity,  pdf_encoding);
  }
  encoding = &enc_cache.encodings[enc_id];

  pdf_init_encoding_struct(encoding);
  encoding->ident = NEW(strlen(enc_name)+1, char);
  strcpy(encoding->ident, enc_name);
  if (load_encoding_file(encoding, enc_name) < 0) {
    pdf_clean_encoding_struct(encoding);
    return -1;
  }

  if (verbose > 1) {
    if (encoding->enc_name) {
      MESG("[%s]", encoding->enc_name);
    }
  }

  if (verbose) MESG(")");

  enc_cache.count++;

  return enc_id;
}


/*
 * Pointer will change if other encoding is loaded...
 */

char **
pdf_encoding_get_encoding (int enc_id)
{
  pdf_encoding *encoding;

  CHECK_ID(enc_id);

  encoding = &enc_cache.encodings[enc_id];

  return encoding->glyphs;
}

pdf_obj *
pdf_get_encoding_reference (int enc_id)
{
  pdf_encoding *encoding;

  CHECK_ID(enc_id);

  encoding = &enc_cache.encodings[enc_id];

  if (!encoding->reference)
    encoding->reference = pdf_ref_obj(encoding->resource);

  return pdf_link_obj(encoding->reference);
}

int
pdf_encoding_is_predefined (int enc_id)
{
  pdf_encoding *encoding;

  CHECK_ID(enc_id);

  encoding = &enc_cache.encodings[enc_id];

  return (encoding->flags & FLAG_IS_PREDEFINED) ? 1 : 0;
}

int
pdf_encoding_is_ASL_charset (int enc_id)
{
  pdf_encoding *encoding;

  CHECK_ID(enc_id);

  encoding = &enc_cache.encodings[enc_id];

  return (encoding->flags & FLAG_IS_ASL_CHARSET) ? 1 : 0;
}

char *
pdf_encoding_get_name (int enc_id)
{
  pdf_encoding *encoding;

  CHECK_ID(enc_id);

  encoding = &enc_cache.encodings[enc_id];

  return encoding->enc_name;
}

/* CSI_UNICODE */
#include "cid.h"

#include "cmap.h"
#include "cmap_read.h"
#include "cmap_write.h"

#include "agl.h"

#define WBUF_SIZE 1024
static unsigned char wbuf[WBUF_SIZE];
static unsigned char range_min[1] = {0x00u};
static unsigned char range_max[1] = {0xFFu};

#define UNICODE_ACCESSIBLE     1
#define UNICODE_INACCESSIBLE  -1
#define UNICODE_UNKNOWN        0

static int
check_unicode_mappable (pdf_encoding *encoding,	char *is_used)
{
  int   code;
  int   glyph_count, total_fail_count;

  glyph_count = total_fail_count = 0;
  for (code = 0; code <= 0xff; code++) {
    if (is_used && !is_used[code])
      continue;

    switch (encoding->accessible[code]) {
    case UNICODE_ACCESSIBLE:
      break;
    case UNICODE_INACCESSIBLE:
      total_fail_count++;
      break;
    default: /* Unknown */
      if (!encoding->glyphs[code] ||
          !strcmp(encoding->glyphs[code], ".notdef"))
         encoding->accessible[code] = UNICODE_INACCESSIBLE;
      else {
        long  len;
        int   fail_count = 0;
        unsigned char *p, *endptr;

        p      = wbuf;
        endptr = wbuf + WBUF_SIZE;
        len = agl_sput_UTF16BE(encoding->glyphs[code],
                               &p, endptr, &fail_count);
        if (len < 1 || fail_count > 0) {
          total_fail_count++;
          encoding->accessible[code] = UNICODE_INACCESSIBLE;
        } else {
          encoding->accessible[code] = UNICODE_ACCESSIBLE;
        }
      }
    }
    glyph_count++;
  }

  if (glyph_count < 1)
    return 0;

  return (int) (100.0 * total_fail_count / glyph_count + 0.5);
}

int
pdf_attach_ToUnicode_CMap (pdf_obj *fontdict,
                           int encoding_id, char *is_used)
{
  pdf_encoding *encoding;
  int           inaccessibles, code;

  ASSERT(fontdict);

  CHECK_ID(encoding_id);

  if (pdf_encoding_is_predefined(encoding_id)) {
    return 0;
  }

  encoding = &enc_cache.encodings[encoding_id];
  
  inaccessibles = check_unicode_mappable(encoding, is_used);
  if (inaccessibles > 10) { /* 10 % */
    if (verbose) {
      WARN("%d%% of glyph in font (encoding: %s) could not converted to Unicode.",
           inaccessibles, encoding->enc_name);
      WARN("ToUnicode CMap not attached for this font.");
    }
    return -1;
  }

  for (code = 0; code <= 0xff; code++) {
    if (is_used && !is_used[code])
      continue;
    encoding->is_used[code] = 1;
  }

  if (!encoding->tounicode) {
    encoding->tounicode = pdf_new_stream(STREAM_COMPRESS);
  }

  pdf_add_dict(fontdict,
               pdf_new_name("ToUnicode"),
               pdf_ref_obj(encoding->tounicode));

  return 0;
}



pdf_obj *
pdf_create_ToUnicode_CMap (const char *enc_name,
                           char **enc_vec, char *is_used)
{
  pdf_obj  *stream;
  CMap     *cmap;
  int       code, glyph_count, total_fail_count;
  char     *cmap_name;
  unsigned char *p, *endptr;

  ASSERT(enc_name && enc_vec);

  cmap_name = NEW(strlen(enc_name)+strlen("-UTF16")+1, char);
  sprintf(cmap_name, "%s-UTF16", enc_name);

  cmap = CMap_new();
  CMap_set_name (cmap, cmap_name);
  CMap_set_type (cmap, CMAP_TYPE_TO_UNICODE);
  CMap_set_wmode(cmap, 0);

  CMap_set_CIDSysInfo(cmap, &CSI_UNICODE);

  CMap_add_codespacerange(cmap, range_min, range_max, 1);

  glyph_count = total_fail_count = 0;
  for (code = 0; code <= 0xff; code++) {
    if (is_used && !is_used[code])
      continue;

    if (enc_vec[code] && strcmp(enc_vec[code], ".notdef")) {
      long   len;
      int    fail_count = 0;

      wbuf[0] = (code & 0xff);
      p      = wbuf + 1;
      endptr = wbuf + WBUF_SIZE;
      len = agl_sput_UTF16BE(enc_vec[code], &p, endptr, &fail_count);
      if (len < 1 || fail_count > 0) {
        total_fail_count++;
      } else {
        CMap_add_bfchar(cmap, wbuf, 1, wbuf + 1, len);
      }
      glyph_count++;
    }
  }

  if (total_fail_count < (glyph_count + 9) / 10) {
    stream = CMap_create_stream(cmap, 0);
  } else {
    if (verbose) {
      WARN("%d (out of %d) glyphs in encoding \"%s\" missing Unicode mapping...",
           total_fail_count, glyph_count, enc_name);
      WARN("ToUnicode CMap \"%s\" removed.", cmap_name);
    }
    stream = NULL;
  }
  CMap_release(cmap);
  RELEASE(cmap_name);

  return stream;
}


pdf_obj *
pdf_load_ToUnicode_stream (const char *ident)
{
  pdf_obj *stream = NULL;
  CMap    *cmap;
  FILE    *fp;

  if (!ident)
    return NULL;

  fp = DPXFOPEN(ident, DPX_RES_TYPE_CMAP);
  if (!fp)
    return NULL;
  else if (CMap_parse_check_sig(fp) < 0) {
    DPXFCLOSE(fp);
    return NULL;
  }

  cmap = CMap_new();
  if (CMap_parse(cmap, fp) < 0) {
    WARN("Reading CMap file \"%s\" failed.", ident);
  } else {
    if (verbose) {
      MESG("(CMap:%s)", ident);
    }
    stream = CMap_create_stream(cmap, 0);
    if (!stream) {
      WARN("Failed to creat ToUnicode CMap stream for \"%s\".", ident);
    }
  }
  CMap_release(cmap);
  DPXFCLOSE(fp);

  return  stream;
}


static const char *
MacRomanEncoding[256] = {
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  "space", "exclam",  "quotedbl", "numbersign",
  "dollar", "percent", "ampersand", "quotesingle",
  "parenleft", "parenright", "asterisk", "plus",
  "comma", "hyphen", "period", "slash",
  "zero", "one", "two", "three",
  "four", "five", "six", "seven",
  "eight", "nine", "colon", "semicolon",
  "less", "equal", "greater", "question",
  "at", "A", "B", "C",
  "D", "E", "F", "G", "H",
  "I", "J", "K", "L",
  "M", "N", "O", "P",
  "Q", "R", "S", "T",
  "U", "V", "W", "X",
  "Y", "Z", "bracketleft", "backslash",
  "bracketright", "asciicircum", "underscore",
  "grave", "a", "b", "c",
  "d", "e", "f", "g",
  "h", "i", "j", "k",
  "l", "m", "n", "o",
  "p", "q", "r", "s",
  "t", "u", "v", "w",
  "x", "y", "z", "braceleft",
  "bar", "braceright", "asciitilde", ".notdef",
  "Adieresis", "Aring", "Ccedilla", "Eacute",
  "Ntilde", "Odieresis", "Udieresis", "aacute",
  "agrave", "acircumflex", "adieresis", "atilde",
  "aring", "ccedilla", "eacute", "egrave",
  "ecircumflex", "edieresis", "iacute", "igrave",
  "icircumflex", "idieresis", "ntilde", "oacute",
  "ograve", "ocircumflex", "odieresis", "otilde",
  "uacute", "ugrave", "ucircumflex", "udieresis",
  "dagger", "degree", "cent", "sterling",
  "section", "bullet", "paragraph", "germandbls",
  "registered", "copyright", "trademark", "acute",
  "dieresis", "notequal", "AE", "Oslash",
  "infinity", "plusminus", "lessequal", "greaterequal",
  "yen", "mu", "partialdiff", "summation",
  "product", "pi", "integral", "ordfeminine",
  "ordmasculine", "Omega", "ae", "oslash",
  "questiondown", "exclamdown", "logicalnot", "radical",
  "florin", "approxequal", "Delta", "guillemotleft",
  "guillemotright", "ellipsis", "space", "Agrave",
  "Atilde", "Otilde", "OE", "oe",
  "endash", "emdash", "quotedblleft", "quotedblright",
  "quoteleft", "quoteright", "divide", "lozenge",
  "ydieresis", "Ydieresis", "fraction", "currency",
  "guilsinglleft", "guilsinglright", "fi", "fl",
  "daggerdbl", "periodcentered", "quotesinglbase", "quotedblbase",
  "perthousand", "Acircumflex", "Ecircumflex", "Aacute",
  "Edieresis", "Egrave", "Iacute", "Icircumflex",
  "Idieresis", "Igrave", "Oacute", "Ocircumflex",
  "apple", "Ograve", "Uacute", "Ucircumflex",
  "Ugrave", "dotlessi", "circumflex", "tilde",
  "macron", "breve", "dotaccent", "ring",
  "cedilla", "hungarumlaut", "ogonek", "caron"
};

static const char *
MacExpertEncoding[256] = {
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  "space", "exclamsmall", "Hungarumlautsmall", "centoldstyle",
  "dollaroldstyle", "dollarsuperior", "ampersandsmall", "Acutesmall",
  "parenleftsuperior", "parenrightsuperior", "twodotenleader", "onedotenleader",
  "comma", "hyphen", "period", "fraction",
  "zerooldstyle", "oneoldstyle", "twooldstyle", "threeoldstyle",
  "fouroldstyle", "fiveoldstyle", "sixoldstyle", "sevenoldstyle",
  "eightoldstyle", "nineoldstyle", "colon", "semicolon",
  ".notdef", "threequartersemdash", ".notdef", "questionsmall",
  ".notdef", ".notdef", ".notdef", ".notdef",
  "Ethsmall", ".notdef", ".notdef", "onequarter",
  "onehalf", "threequarters", "oneeighth", "threeeighths",
  "fiveeighths", "seveneighths", "onethird", "twothirds",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", "ff", "fi",
  "fl", "ffi", "ffl", "parenleftinferior",
  ".notdef", "parenrightinferior", "Circumflexsmall", "hypheninferior",
  "Gravesmall", "Asmall", "Bsmall", "Csmall",
  "Dsmall", "Esmall", "Fsmall", "Gsmall",
  "Hsmall", "Ismall", "Jsmall", "Ksmall",
  "Lsmall", "Msmall", "Nsmall", "Osmall",
  "Psmall", "Qsmall", "Rsmall", "Ssmall",
  "Tsmall", "Usmall", "Vsmall", "Wsmall",
  "Xsmall", "Ysmall", "Zsmall", "colonmonetary",
  "onefitted", "rupiah", "Tildesmall", ".notdef",
  ".notdef", "asuperior", "centsuperior", ".notdef",
  ".notdef", ".notdef", ".notdef", "Aacutesmall",
  "Agravesmall", "Acircumflexsmall", "Adieresissmall", "Atildesmall",
  "Aringsmall", "Ccedillasmall", "Eacutesmall", "Egravesmall",
  "Ecircumflexsmall", "Edieresissmall", "Iacutesmall", "Igravesmall",
  "Icircumflexsmall", "Idieresissmall", "Ntildesmall", "Oacutesmall",
  "Ogravesmall", "Ocircumflexsmall", "Odieresissmall", "Otildesmall",
  "Uacutesmall", "Ugravesmall", "Ucircumflexsmall", "Udieresissmall",
  ".notdef", "eightsuperior", "fourinferior", "threeinferior",
  "sixinferior", "eightinferior", "seveninferior", "Scaronsmall",
  ".notdef", "centinferior", "twoinferior", ".notdef",
  "Dieresissmall", ".notdef", "Caronsmall", "osuperior",
  "fiveinferior", ".notdef", "commainferior", "periodinferior",
  "Yacutesmall", ".notdef", "dollarinferior", ".notdef",
  ".notdef", "Thornsmall", ".notdef", "nineinferior",
  "zeroinferior", "Zcaronsmall", "AEsmall", "Oslashsmall",
  "questiondownsmall", "oneinferior", "Lslashsmall", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", "Cedillasmall", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", "OEsmall",
  "figuredash", "hyphensuperior", ".notdef", ".notdef",
  ".notdef", ".notdef", "exclamdownsmall", ".notdef",
  "Ydieresissmall", ".notdef", "onesuperior", "twosuperior",
  "threesuperior", "foursuperior", "fivesuperior", "sixsuperior",
  "sevensuperior", "ninesuperior", "zerosuperior", ".notdef",
  "esuperior", "rsuperior", "tsuperior", ".notdef",
  ".notdef", "isuperior", "ssuperior", "dsuperior",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", "lsuperior", "Ogoneksmall", "Brevesmall",
  "Macronsmall", "bsuperior", "nsuperior", "msuperior",
  "commasuperior", "periodsuperior", "Dotaccentsmall", "Ringsmall",
  ".notdef", ".notdef", ".notdef", ".notdef"
};

static const char *
WinAnsiEncoding[256] = {
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  "space", "exclam", "quotedbl", "numbersign",
  "dollar", "percent", "ampersand", "quotesingle",
  "parenleft", "parenright", "asterisk", "plus",
  "comma", "hyphen", "period", "slash",
  "zero", "one", "two", "three",
  "four", "five", "six", "seven",
  "eight", "nine", "colon", "semicolon",
  "less", "equal", "greater", "question",
  "at", "A", "B", "C",
  "D", "E", "F", "G",
  "H", "I", "J", "K",
  "L", "M", "N", "O",
  "P", "Q", "R", "S",
  "T", "U", "V", "W",
  "X", "Y", "Z", "bracketleft",
  "backslash", "bracketright", "asciicircum", "underscore",
  "grave", "a", "b", "c",
  "d", "e", "f", "g",
  "h", "i", "j", "k",
  "l", "m", "n", "o",
  "p", "q", "r", "s",
  "t", "u", "v", "w",
  "x", "y", "z", "braceleft",
  "bar", "braceright", "asciitilde", "bullet",
  "Euro", "bullet", "quotesinglbase", "florin",
  "quotedblbase", "ellipsis", "dagger", "daggerdbl",
  "circumflex", "perthousand", "Scaron", "guilsinglleft",
  "OE", "bullet", "Zcaron", "bullet",
  "bullet", "quoteleft", "quoteright", "quotedblleft",
  "quotedblright", "bullet", "endash", "emdash",
  "tilde", "trademark", "scaron", "guilsinglright",
  "oe", "bullet", "zcaron", "Ydieresis",
  "space", "exclamdown", "cent", "sterling",
  "currency", "yen", "brokenbar", "section",
  "dieresis", "copyright", "ordfeminine", "guillemotleft",
  "logicalnot", "hyphen", "registered", "macron",
  "degree", "plusminus", "twosuperior", "threesuperior",
  "acute", "mu", "paragraph", "periodcentered",
  "cedilla", "onesuperior", "ordmasculine", "guillemotright",
  "onequarter", "onehalf", "threequarters", "questiondown",
  "Agrave", "Aacute", "Acircumflex", "Atilde",
  "Adieresis", "Aring", "AE", "Ccedilla",
  "Egrave", "Eacute", "Ecircumflex", "Edieresis",
  "Igrave", "Iacute", "Icircumflex", "Idieresis",
  "Eth", "Ntilde", "Ograve", "Oacute",
  "Ocircumflex", "Otilde", "Odieresis", "multiply",
  "Oslash", "Ugrave", "Uacute", "Ucircumflex",
  "Udieresis", "Yacute", "Thorn", "germandbls",
  "agrave", "aacute", "acircumflex", "atilde",
  "adieresis", "aring", "ae", "ccedilla",
  "egrave", "eacute", "ecircumflex", "edieresis",
  "igrave", "iacute", "icircumflex", "idieresis",
  "eth", "ntilde", "ograve", "oacute",
  "ocircumflex", "otilde", "odieresis", "divide",
  "oslash", "ugrave", "uacute", "ucircumflex",
  "udieresis", "yacute", "thorn", "ydieresis"
};

static const char *
StandardEncoding[256] = {
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  "space", "exclam", "quotedbl", "numbersign",
  "dollar", "percent", "ampersand", "quoteright",
  "parenleft", "parenright", "asterisk", "plus",
  "comma", "hyphen", "period", "slash",
  "zero", "one", "two", "three",
  "four", "five", "six", "seven",
  "eight", "nine", "colon", "semicolon",
  "less", "equal", "greater", "question",
  "at", "A", "B", "C",
  "D", "E", "F", "G",
  "H", "I", "J", "K",
  "L", "M", "N", "O",
  "P", "Q", "R", "S",
  "T", "U", "V", "W",
  "X", "Y", "Z", "bracketleft",
  "backslash", "bracketright", "asciicircum", "underscore",
  "quoteleft", "a", "b", "c",
  "d", "e", "f", "g",
  "h", "i", "j", "k",
  "l", "m", "n", "o",
  "p", "q", "r", "s",
  "t", "u", "v", "w",
  "x", "y", "z", "braceleft",
  "bar", "braceright", "asciitilde", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", "exclamdown", "cent", "sterling",
  "fraction", "yen", "florin", "section",
  "currency", "quotesingle", "quotedblleft", "guillemotleft",
  "guilsinglleft", "guilsinglright", "fi", "fl",
  ".notdef", "endash", "dagger", "daggerdbl",
  "periodcentered", ".notdef", "paragraph", "bullet",
  "quotesinglbase", "quotedblbase", "quotedblright", "guillemotright",
  "ellipsis", "perthousand", ".notdef", "questiondown",
  ".notdef", "grave", "acute", "circumflex",
  "tilde", "macron", "breve", "dotaccent",
  "dieresis", ".notdef", "ring", "cedilla",
  ".notdef", "hungarumlaut", "ogonek", "caron",
  "emdash", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", "AE", ".notdef", "ordfeminine",
  ".notdef", ".notdef", ".notdef", ".notdef",
  "Lslash", "Oslash", "OE", "ordmasculine",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", "ae", ".notdef", ".notdef",
  ".notdef", "dotlessi", ".notdef", ".notdef",
  "lslash", "oslash", "oe", "germandbls",
  ".notdef", ".notdef", ".notdef", ".notdef"
};

static const char *
ISOLatin1Encoding[256] = {
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  "space", "exclam", "quotedbl", "numbersign",
  "dollar", "percent", "ampersand", "quotesingle",
  "parenleft", "parenright", "asterisk", "plus",
  "comma", "hyphen", "period", "slash",
  "zero", "one", "two", "three",
  "four", "five", "six", "seven",
  "eight", "nine", "colon", "semicolon",
  "less", "equal", "greater", "question",
  "at", "A", "B", "C",
  "D", "E", "F", "G",
  "H", "I", "J", "K",
  "L", "M", "N", "O",
  "P", "Q", "R", "S",
  "T", "U", "V", "W",
  "X", "Y", "Z", "bracketleft",
  "backslash", "bracketright", "asciicircum", "underscore",
  "grave", "a", "b", "c",
  "d", "e", "f", "g",
  "h", "i", "j", "k",
  "l", "m", "n", "o",
  "p", "q", "r", "s",
  "t", "u", "v", "w",
  "x", "y", "z", "braceleft",
  "bar", "braceright", "asciitilde", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  ".notdef", ".notdef", ".notdef", ".notdef",
  "dotlessi", "quoteleft", "quoteright", "circumflex",
  "tilde", "macron", "breve", "dotaccent",
  "dieresis", ".notdef", "ring", "cedilla",
  ".notdef", "hungarumlaut", "ogonek", "caron",
  "space", "exclamdown", "cent", "sterling",
  "currency", "yen", "brokenbar", "section",
  "dieresis", "copyright", "ordfeminine", "guillemotleft",
  "logicalnot", "hyphen", "registered", "macron",
  "degree", "plusminus", "twosuperior", "threesuperior",
  "acute", "mu", "paragraph", "periodcentered",
  "cedilla", "onesuperior", "ordmasculine", "guillemotright",
  "onequarter", "onehalf", "threequarters", "questiondown",
  "Agrave", "Aacute", "Acircumflex", "Atilde",
  "Adieresis", "Aring", "AE", "Ccedilla",
  "Egrave", "Eacute", "Ecircumflex", "Edieresis",
  "Igrave", "Iacute", "Icircumflex", "Idieresis",
  "Eth", "Ntilde", "Ograve", "Oacute",
  "Ocircumflex", "Otilde", "Odieresis", "multiply",
  "Oslash", "Ugrave", "Uacute", "Ucircumflex",
  "Udieresis", "Yacute", "Thorn", "germandbls",
  "agrave", "aacute", "acircumflex", "atilde",
  "adieresis", "aring", "ae", "ccedilla",
  "egrave", "eacute", "ecircumflex", "edieresis",
  "igrave", "iacute", "icircumflex", "idieresis",
  "eth", "ntilde", "ograve", "oacute",
  "ocircumflex", "otilde", "odieresis", "divide",
  "oslash", "ugrave", "uacute", "ucircumflex",
  "udieresis", "yacute", "thorn", "ydieresis"
};
