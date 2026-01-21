/* This is xdvipsk, an eXtended version of dvips(k) by Tomas Rokicki.

	Copyright (C) 2016 by VTeX Ltd (www.vtex.lt),
	the xdvipsk project team - Sigitas Tolusis and Arunas Povilaitis.

    Program original code copyright (C) 2007-2014 by Jin-Hwan Cho and 
	Shunsaku Hirata, the dvipdfmx project team.

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

/*
 * TrueType glyf table is sorted by CID and no CIDToGIDMap is used here.
 * GhostScript can't handle CIDToGIDMap correctly.
 */

#include <config.h>

#include "system.h"
#include "numbers.h"
#include "mem.h"
#include "error.h"

#include "xdvips.h"
#include "protos.h"

/* TrueType */
#include "sfnt.h"
#include "tt_aux.h"
#include "tt_glyf.h"
#include "tt_table.h"

/* CID font */
#include "cidtype2.h"
#include "cmap.h"
#include "tt_cmap.h"

static int verbose   = 0;

/*
 * PDF viewer applications use following tables (CIDFontType 2)
 *
 *  head, hhea, loca, maxp, glyf, hmtx, fpgm, cvt_, prep
 *
 *                                         - from PDF Ref. v.1.3, 2nd ed.
 *
 * The fpgm, cvt_, and prep tables appears only when TrueType instructions
 * requires them. Those tables must be preserved if they exist.
 * We use must_exist flag to indicate `preserve it if present'
 * and to make sure not to cause an error when it does not exist.
 *
 * post and name table must exist in ordinary TrueType font file,
 * but when a TrueType font is converted to CIDFontType 2 font, those tables
 * are no longer required.
 *
 * The OS/2 table (required for TrueType font for Windows and OS/2) contains
 * liscencing information, but PDF viewers seems not using them.
 *
 * The 'name' table added. See comments in ttf.c.
 */

static struct
{
  const char *name;
  int         must_exist;
} required_table[] = {
  {"OS/2", 0}, {"head", 1}, {"hhea", 1}, {"loca", 1}, {"maxp", 1},
  {"name", 1}, {"glyf", 1}, {"hmtx", 1}, {"fpgm", 0}, {"cvt ", 0},
  {"prep", 0}, {"vhea", 0}, {"vmtx", 0}, {NULL, 0}
};



/*
 * The following routine fixes few problems caused by vendor specific
 * Unicode mappings.
 */

#define FIX_CJK_UNIOCDE_SYMBOLS 1

static unsigned short
fix_CJK_symbols (unsigned short code)
{
  unsigned short alt_code;
  static struct
  {
    unsigned short alt1;
    unsigned short alt2;
  } CJK_Uni_symbols[] = {
    /*
     * Microsoft/Apple Unicode mapping difference:
     * They are taken from SJIS-Unicode mapping difference but nearly
     * same thing might be applied to Chinese (e.g., Big5) too.
     */
    {0x2014, 0x2015}, /* EM DASH <-> HORIZONTAL BAR */
    {0x2016, 0x2225}, /* DOUBLE VERTICAL LINE <-> PARALLEL TO */
    {0x203E, 0xFFE3}, /* OVERLINE <-> FULLWIDTH MACRON */
    {0x2026, 0x22EF}, /* HORIZONTAL ELLIPSIS <-> MIDLINE HORIZONTAL ELLIPSIS */
    {0x2212, 0xFF0D}, /* MINUS SIGN <-> FULLWIDTH HYPHEN-MINUS */
    {0x301C, 0xFF5E}, /* WAVE DASH <-> FULLWIDTH TILDE */
    {0xFFE0, 0x00A2}, /* FULLWIDTH CENT SIGN <-> CENT SIGN */
    {0xFFE1, 0x00A3}, /* FULLWIDTH POUND SIGN <-> POUND SIGN */
    {0xFFE2, 0x00AC}, /* FULLWIDTH NOT SIGN <-> NOT SIGN */
    {0xFFFF, 0xFFFF}, /* EOD */
  };
#define NUM_CJK_SYMBOLS (sizeof(CJK_Uni_symbols)/sizeof(CJK_Uni_symbols[0]))
  int i;

  alt_code = code;
  for (i = 0; i < NUM_CJK_SYMBOLS; i++) {
    if (CJK_Uni_symbols[i].alt1 == code) {
      alt_code = CJK_Uni_symbols[i].alt2;
      break;
    } else if (CJK_Uni_symbols[i].alt2 == code) {
      alt_code = CJK_Uni_symbols[i].alt1;
      break;
    }
  }

  return alt_code;
}

static long
cid_to_code (CMap *cmap, CID cid)
{
  unsigned char  inbuf[2], outbuf[32];
  long           inbytesleft = 2, outbytesleft = 32;
  const unsigned char *p;
  unsigned char *q;

  if (!cmap)
    return cid;

  inbuf[0] = (cid >> 8) & 0xff;
  inbuf[1] = cid & 0xff;
  p = inbuf; q = outbuf;

  CMap_decode_char(cmap, &p, &inbytesleft, &q, &outbytesleft);

  if (inbytesleft != 0)
    return 0;
  else if (outbytesleft == 31)
    return (long) outbuf[0];
  else if (outbytesleft == 30)
    return (long) (outbuf[0] << 8|outbuf[1]);
  else if (outbytesleft == 28) { /* We assume the output encoding is UTF-16. */
    CID hi, lo;
    hi = outbuf[0] << 8|outbuf[1];
    lo = outbuf[2] << 8|outbuf[3];
    if (hi >= 0xd800 && hi <= 0xdbff && lo >= 0xdc00 && lo <= 0xdfff)
      return (long) ((hi - 0xd800) * 0x400 + 0x10000 + lo - 0xdc00);
    else
      return (long) (hi << 16|lo);
  }

  return 0;
}

/* #define NO_GHOSTSCRIPT_BUG 1 */

void
CIDFont_type2_dofont (const char *PSName, sfnt *sfont, int index, UsedMapElem *used_glyphs, long *maxcid)
{
  UsedMapElem *h_used_chars, *v_used_chars, *used_chars;
  struct tt_glyphs *glyphs;
  CMap    *cmap = NULL;
  tt_cmap *ttcmap = NULL;
  unsigned long offset = 0;
  CID      cid, last_cid;
  unsigned char *cidtogidmap;
  USHORT   num_glyphs;
  int      i, glyph_ordering = 0, unicode_cmap = 0;


  ASSERT(sfont);

  switch (sfont->type) {
  case SFNT_TYPE_TTC:
    offset = ttc_read_offset(sfont, index);
    if (offset == 0)
      ERROR("Invalid TTC index in %s.", PSName);
    break;
  case SFNT_TYPE_TRUETYPE:
    if (index > 0)
      ERROR("Found TrueType font file while expecting TTC file (%s).", PSName);
    offset = 0;
    break;
  case SFNT_TYPE_DFONT:
    offset = sfont->offset;
    break;
  default:
    ERROR("Not a TrueType/TTC font (%s)?", PSName);
    break;
  }

  if (sfnt_read_table_directory(sfont, offset) < 0)
    ERROR("Could not read TrueType table directory (%s).", PSName);

  glyph_ordering = 1;
  ttcmap = NULL;
  cmap   = NULL;

  glyphs = tt_build_init();

  last_cid   = 0;
  num_glyphs = 1; /* .notdef */
  used_chars = h_used_chars = v_used_chars = NULL;
  {
    int        c;

    h_used_chars = used_glyphs;

    /*
     * Quick check of max CID.
     */
    c = 0;
    for (i = USED_CHARS_BUF_SIZE - 1; i >= 0; i--) {
      if (h_used_chars && h_used_chars[i] != 0) {
	last_cid = i * BITS_PER_USED_ELEM + BITS_PER_USED_ELEM - 1;
	c = h_used_chars[i];
	break;
      }
      if (v_used_chars && v_used_chars[i] != 0) {
	last_cid = i * BITS_PER_USED_ELEM + BITS_PER_USED_ELEM - 1;
	c = v_used_chars[i];
	break;
      }
    }
    if (last_cid > 0) {
      for (i = 0; i < BITS_PER_USED_ELEM; i++) {
	if ((c >> i) & 1)
	  break;
	last_cid--;
      }
    }
    if (last_cid >= 0xFFFFu) {
      ERROR("CID count > 65535");
    }
  }

#ifndef NO_GHOSTSCRIPT_BUG
  cidtogidmap = NULL;
#else
  cidtogidmap = NEW((last_cid + 1) * 2, unsigned char);
  memset(cidtogidmap, 0, (last_cid + 1) * 2);
#endif /* !NO_GHOSTSCRIPT_BUG */

  /*
   * Map CIDs to GIDs.
   * Horizontal and vertical used_chars are merged.
   */

  /*
   * Horizontal
   */
  if (h_used_chars) {
    used_chars = h_used_chars;
    for (cid = 1; cid <= last_cid; cid++) {
      long           code;
      unsigned short gid;

      if (!IS_USED_CHAR(h_used_chars, cid))
	continue;

      if (glyph_ordering) {
	gid  = cid;
	code = cid;
      } else {
	code = cid_to_code(cmap, cid);
	gid  = tt_cmap_lookup(ttcmap, code);
#ifdef FIX_CJK_UNIOCDE_SYMBOLS
	if (gid == 0 && unicode_cmap) {
	  long alt_code;

	  alt_code = fix_CJK_symbols((unsigned short)code);
	  if (alt_code != code) {
	    gid = tt_cmap_lookup(ttcmap, alt_code);
	    if (gid != 0) {
	      WARN("Unicode char U+%04x replaced with U+%04x.",
		   code, alt_code);
	    }
	  }
	}
#endif /* FIX_CJK_UNIOCDE_SYMBOLS */
      }

      if (gid == 0) {
	WARN("Glyph missing in font. (CID=%u, code=0x%04x)", cid, code);
      }

      /* TODO: duplicated glyph */
#ifndef NO_GHOSTSCRIPT_BUG
      gid = tt_add_glyph(glyphs, gid, cid);
#else
      gid = tt_add_glyph(glyphs, gid, num_glyphs);
      cidtogidmap[2*cid  ] = gid >> 8;
      cidtogidmap[2*cid+1] = gid & 0xff;
#endif /* !NO_GHOSTSCRIPT_BUG */

      num_glyphs++;
    }
  }

  /*
   * Vertical
   */
  if (v_used_chars) {

    /*
     * Require `vrt2' or `vert'.
     */

    for (cid = 1; cid <= last_cid; cid++) {
      long           code;
      unsigned short gid;

      if (!IS_USED_CHAR(v_used_chars, cid))
	continue;

      /* There may be conflict of horizontal and vertical glyphs
       * when font is used with /UCS. However, we simply ignore
       * that...
       */
      if (h_used_chars && IS_USED_CHAR(h_used_chars, cid)) {
	continue;
      }

      if (glyph_ordering) {
	gid  = cid;
	code = cid;
      } else {
	code = cid_to_code(cmap, cid);
	gid  = tt_cmap_lookup(ttcmap, code);
#ifdef FIX_CJK_UNIOCDE_SYMBOLS
	if (gid == 0 && unicode_cmap) {
	  long alt_code;

	  alt_code = fix_CJK_symbols((unsigned short)code);
	  if (alt_code != code) {
	    gid = tt_cmap_lookup(ttcmap, alt_code);
	    if (gid != 0) {
	      WARN("Unicode char U+%04x replaced with U+%04x.",
		   code, alt_code);
	    }
	  }
	}
#endif /* FIX_CJK_UNIOCDE_SYMBOLS */
      }
      if (gid == 0) {
	WARN("Glyph missing in font. (CID=%u, code=0x%04x)", cid, code);
      }

#ifndef NO_GHOSTSCRIPT_BUG
      gid = tt_add_glyph(glyphs, gid, cid);
#else
      gid = tt_add_glyph(glyphs, gid, num_glyphs);
      cidtogidmap[2*cid  ] = gid >> 8;
      cidtogidmap[2*cid+1] = gid & 0xff;
#endif /* !NO_GHOSTSCRIPT_BUG */

      if (used_chars) /* merge vertical used_chars to horizontal */
          ADD_TO_USED_CHARS(used_chars, cid);

      num_glyphs++;
    }

    if (!used_chars) /* We have no horizontal. */
      used_chars = v_used_chars;
  }

  if (!used_chars)
    ERROR("Unexpected error.");

  tt_cmap_release(ttcmap);

  if (tt_build_tables(sfont, glyphs) < 0)
     ERROR("Could not created FontFile stream.");
  if (verbose > 1)
    MESG("[%u glyphs (Max CID: %u)]", glyphs->num_glyphs, last_cid);

  tt_build_finish(glyphs);

  *maxcid = last_cid;
  
  return;
}

void
CIDFont_type2_checktables(sfnt *sfont)
{
  int i;

  for (i = 0; required_table[i].name; i++) {
    if (sfnt_require_table(sfont,
			   required_table[i].name,
			   required_table[i].must_exist) < 0) {
      ERROR("Some required TrueType table (%s) does not exist.", required_table[i].name);
    }
  }
}
