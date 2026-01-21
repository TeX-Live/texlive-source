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
 * A large part of codes are brought from ttfdump-0.5.5.
 */

#include <config.h>

#include "xdvips.h" /* The copyright notice in that file is included too! */
#include "protos.h"

#include "system.h"
#include "mem.h"
#include "error.h"

#include "sfnt.h"


/* Sorry for placing this here.
 * We need to rewrite TrueType font support code...
 */
#include "cmap.h"
#include "cmap_write.h"

#include "tt_aux.h"
#include "tt_post.h"

#include "unicode.h"

#include "dpxfile.h"

/* Hash */
#include "dpxutil.h"

#include "tt_cmap.h"

/* format 0: byte encoding table */
struct cmap0
{
  BYTE glyphIndexArray[256];
};

static struct cmap0 *
read_cmap0 (sfnt *sfont, ULONG len)
{
  struct cmap0 *map;
  int    i;

  if (len < 256)
    ERROR("invalid cmap subtable");

  map = NEW(1, struct cmap0);

  for (i = 0; i < 256; i++)
    map->glyphIndexArray[i] = sfnt_get_byte(sfont);

  return map;
}

static void
release_cmap0(struct cmap0 *map)
{
  if (map)
    RELEASE(map);
}

static USHORT
lookup_cmap0 (struct cmap0 *map, USHORT cc)
{
  return ((cc > 255) ? 0 : map->glyphIndexArray[cc]);
}

/* format 2: high-byte mapping through table */
struct SubHeader
{
  USHORT firstCode;
  USHORT entryCount;
  SHORT  idDelta;
  USHORT idRangeOffset;
};

struct cmap2
{
  USHORT  subHeaderKeys[256];
  struct SubHeader *subHeaders;
  USHORT *glyphIndexArray;
};

static struct cmap2 *
read_cmap2 (sfnt *sfont, ULONG len)
{
  struct cmap2 *map;
  USHORT i, n;

  if (len < 512)
    ERROR("invalid cmap subtable");
    
  map = NEW(1, struct cmap2);

  for (i = 0; i < 256; i++)
    map->subHeaderKeys[i] = sfnt_get_ushort(sfont);

  for (n = 0, i = 0; i < 256; i++) {
    map->subHeaderKeys[i] /= 8;
    if (n < map->subHeaderKeys[i])
      n = map->subHeaderKeys[i];
  }
  n += 1; /* the number of subHeaders is one plus the max of subHeaderKeys */

  map->subHeaders = NEW(n, struct SubHeader); 
  for (i = 0; i < n; i++) {
    map->subHeaders[i].firstCode     = sfnt_get_ushort(sfont);
    map->subHeaders[i].entryCount    = sfnt_get_ushort(sfont);
    map->subHeaders[i].idDelta       = sfnt_get_short(sfont);
    map->subHeaders[i].idRangeOffset = sfnt_get_ushort(sfont);

    /* It makes things easier to let the offset starts from
     * the beginning of glyphIndexArray.
     */
    if (map->subHeaders[i].idRangeOffset != 0)
      map->subHeaders[i].idRangeOffset -= (2 + (n - i - 1) * 8);
  }

  /* Caculate the length of glyphIndexArray, this is ugly,
   * there should be a better way to get this information.
   */
  n = (USHORT) (len - 518 - n * 8) / 2;

  map->glyphIndexArray = NEW(n, USHORT);
  for (i = 0; i < n; i++)
    map->glyphIndexArray[i] = sfnt_get_ushort(sfont);

  return map;
}

static void
release_cmap2 (struct cmap2 *map)
{
  if (map) {
    if (map->subHeaders)
      RELEASE(map->subHeaders);
    if (map->glyphIndexArray)
      RELEASE(map->glyphIndexArray);
    RELEASE(map);
  }
}

static USHORT
lookup_cmap2 (struct cmap2 *map, USHORT cc)
{
  USHORT  idx = 0;
  SHORT   idDelta;
  USHORT  firstCode, entryCount, idRangeOffset;
  int     hi, lo;
  USHORT  i;
   
  hi = (cc >> 8) & 0xff;
  lo = cc & 0xff;

  /* select which subHeader to use */
  i = map->subHeaderKeys[hi];

  firstCode     = map->subHeaders[i].firstCode;
  entryCount    = map->subHeaders[i].entryCount;
  idDelta       = map->subHeaders[i].idDelta;
  idRangeOffset = map->subHeaders[i].idRangeOffset / 2;

  if (lo >= firstCode &&
      lo < firstCode + entryCount) {
    idRangeOffset += lo - firstCode;
    idx = map->glyphIndexArray[idRangeOffset];
    if (idx != 0)
      idx = (idx + idDelta) & 0xffff;
  }

  return idx;
}

/*
 * format 4: segment mapping to delta values
 * - Microsoft standard character to glyph index mapping table
 */
struct cmap4
{
  USHORT  segCountX2;
  USHORT  searchRange;
  USHORT  entrySelector;
  USHORT  rangeShift;
  USHORT *endCount;
  USHORT  reservedPad;
  USHORT *startCount;
  USHORT *idDelta;
  USHORT *idRangeOffset;
  USHORT *glyphIndexArray;
};

static struct cmap4 *
read_cmap4(sfnt *sfont, ULONG len)
{
  struct cmap4 *map;
  USHORT i, n, segCount;

  if (len < 8)
    ERROR("invalid cmap subtable");

  map = NEW(1, struct cmap4);

  map->segCountX2    = segCount = sfnt_get_ushort(sfont);
  map->searchRange   = sfnt_get_ushort(sfont);
  map->entrySelector = sfnt_get_ushort(sfont);
  map->rangeShift    = sfnt_get_ushort(sfont);
  
  segCount /= 2;

  map->endCount = NEW(segCount, USHORT);
  for (i = 0; i < segCount; i++)
    map->endCount[i] = sfnt_get_ushort(sfont);

  map->reservedPad = sfnt_get_ushort(sfont);

  map->startCount  = NEW(segCount, USHORT);
  for (i = 0; i < segCount; i++)
    map->startCount[i] = sfnt_get_ushort(sfont);

  map->idDelta = NEW(segCount, USHORT);
  for (i = 0; i < segCount; i++)
    map->idDelta[i] = sfnt_get_ushort(sfont);

  map->idRangeOffset = NEW(segCount, USHORT);
  for (i = 0; i < segCount; i++)
    map->idRangeOffset[i] = sfnt_get_ushort(sfont);

  n = (len - 16 - 8 * segCount) / 2;
  if (n == 0)
    map->glyphIndexArray = NULL;
  else {
    map->glyphIndexArray = NEW(n, USHORT);
    for (i = 0; i < n; i++)
      map->glyphIndexArray[i] = sfnt_get_ushort(sfont);
  }

  return map;
}

static void
release_cmap4 (struct cmap4 *map)
{
  if (map) {
    if (map->endCount)   RELEASE(map->endCount);
    if (map->startCount) RELEASE(map->startCount);
    if (map->idDelta)    RELEASE(map->idDelta);
    if (map->idRangeOffset)   RELEASE(map->idRangeOffset);
    if (map->glyphIndexArray) RELEASE(map->glyphIndexArray);
    RELEASE(map);
  }
}

static USHORT
lookup_cmap4 (struct cmap4 *map, USHORT cc)
{
  USHORT gid = 0;
  USHORT i, j, segCount;

  /*
   * Segments are sorted in order of increasing endCode values.
   * Last segment maps 0xffff to gid 0 (?)
  */
  i = segCount = map->segCountX2 / 2;
  while (i-- > 0 &&  cc <= map->endCount[i]) {
    if (cc >= map->startCount[i]) {
      if (map->idRangeOffset[i] == 0) {
	gid = (cc + map->idDelta[i]) & 0xffff;
      } else if (cc == 0xffff && map->idRangeOffset[i] == 0xffff) {
	/* this is for protection against some old broken fonts... */
	gid = 0;
      } else {
	j  = map->idRangeOffset[i] - (segCount - i) * 2;
	j  = (cc - map->startCount[i]) + (j / 2);
	gid = map->glyphIndexArray[j];
	if (gid != 0)
	  gid = (gid + map->idDelta[i]) & 0xffff;
      }
      break;
    }
  }

  return gid;
}

/* format 6: trimmed table mapping */
struct cmap6
{
  USHORT  firstCode;
  USHORT  entryCount;
  USHORT *glyphIndexArray;
};

static struct cmap6 *
read_cmap6 (sfnt *sfont, ULONG len)
{
  struct cmap6 *map;
  USHORT i;
  
  if (len < 4)
    ERROR("invalid cmap subtable");

  map =  NEW(1, struct cmap6);
  map->firstCode       = sfnt_get_ushort(sfont);
  map->entryCount      = sfnt_get_ushort(sfont);
  map->glyphIndexArray = NEW(map->entryCount, USHORT);
  
  for (i = 0; i < map->entryCount; i++)
    map->glyphIndexArray[i] = sfnt_get_ushort(sfont);

  return map;
}

static void
release_cmap6 (struct cmap6 *map)
{
  if (map) {
    if (map->glyphIndexArray)
      RELEASE(map->glyphIndexArray);
    RELEASE(map);
  }
}

static USHORT
lookup_cmap6 (struct cmap6 *map, USHORT cc)
{
  USHORT idx;

  idx = cc - map->firstCode; 
  if (idx < map->entryCount)
    return map->glyphIndexArray[idx];
  return 0;
}

/* Format 8 and 10 not supported...
 *
 *  format  8: mixed 16-bit and 32-bit coverage
 *  format 10: trimmed array
 */

/*
 * format 12: segmented coverage
 *
 * startGlyphID is 32-bit long, however, GlyphID is still 16-bit long !
 */

struct charGroup
{
  ULONG startCharCode;
  ULONG endCharCode;
  ULONG startGlyphID;
};

struct cmap12
{
  ULONG  nGroups;
  struct charGroup *groups;
};

/* ULONG length */
static struct cmap12 *
read_cmap12 (sfnt *sfont, ULONG len)
{
  struct cmap12 *map;
  ULONG  i;
  
  if (len < 4)
    ERROR("invalid cmap subtable");

  map =  NEW(1, struct cmap12);
  map->nGroups = sfnt_get_ulong(sfont);
  map->groups  = NEW(map->nGroups, struct charGroup);

  for (i = 0; i < map->nGroups; i++) {
    map->groups[i].startCharCode = sfnt_get_ulong(sfont);
    map->groups[i].endCharCode   = sfnt_get_ulong(sfont);
    map->groups[i].startGlyphID  = sfnt_get_ulong(sfont);
  }

  return map;
}

static void
release_cmap12 (struct cmap12 *map)
{
  if (map) {
    if (map->groups)
      RELEASE(map->groups);
    RELEASE(map);
  }
}

static USHORT
lookup_cmap12 (struct cmap12 *map, ULONG cccc)
{
  USHORT gid = 0;
  int i;

  i = map->nGroups;
  while (i-- >= 0 &&
	 cccc <= map->groups[i].endCharCode) {
    if ( i < 0 ) break;
    if (cccc >= map->groups[i].startCharCode) {
      gid = (USHORT) ((cccc -
		       map->groups[i].startCharCode +
		       map->groups[i].startGlyphID) & 0xffff);
      break;
    }
  }

  return gid;
}

/* read cmap */
tt_cmap *
tt_cmap_read (sfnt *sfont, USHORT platform, USHORT encoding)
{
  tt_cmap *cmap = NULL;
  ULONG    offset, length = 0;
  USHORT   p_id, e_id;
  USHORT   i, n_subtabs;

  ASSERT(sfont);

  offset    = sfnt_locate_table(sfont, "cmap");
  (void)      sfnt_get_ushort(sfont);
  n_subtabs = sfnt_get_ushort(sfont);

  for (i = 0; i < n_subtabs; i++) {
    p_id = sfnt_get_ushort(sfont);
    e_id = sfnt_get_ushort(sfont);
    if (p_id != platform || e_id != encoding)
      sfnt_get_ulong(sfont);
    else {
      offset += sfnt_get_ulong(sfont);
      break;
    }
  }

  if (i == n_subtabs)
    return NULL;

  cmap = NEW(1, tt_cmap);
  cmap->map      = NULL;
  cmap->platform = platform;
  cmap->encoding = encoding;

  sfnt_seek_set(sfont, offset);
  cmap->format = sfnt_get_ushort(sfont);
  /* Length and version (language) is ULONG for
   * format 8, 10, 12 !
   */
  if (cmap->format <= 6) {
    length         = sfnt_get_ushort(sfont);
    cmap->language = sfnt_get_ushort(sfont); /* language (Mac) */
  } else {
    if (sfnt_get_ushort(sfont) != 0) { /* reverved - 0 */
      WARN("Unrecognized cmap subtable format.");
      tt_cmap_release(cmap);
      return NULL;
    } else {
      length         = sfnt_get_ulong(sfont);
      cmap->language = sfnt_get_ulong(sfont);
    }
  }
  
  switch(cmap->format) {
  case 0:
    cmap->map = read_cmap0(sfont, length);
    break;
  case 2:
    cmap->map = read_cmap2(sfont, length);
    break;
  case 4:
    cmap->map = read_cmap4(sfont, length);
    break;
  case 6:
    cmap->map = read_cmap6(sfont, length);
    break;
  case 12:
    /* WARN("UCS-4 TrueType cmap table..."); */
    cmap->map = read_cmap12(sfont, length);
    break;
  default:
    WARN("Unrecognized OpenType/TrueType cmap format.");
    tt_cmap_release(cmap);
    return NULL;
  }

  if (!cmap->map) {
    tt_cmap_release(cmap);
    cmap = NULL;
  }

  return cmap;
}

void
tt_cmap_release (tt_cmap *cmap)
{

  if (cmap) {
    if (cmap->map) {
      switch(cmap->format) {
      case 0:
	release_cmap0(cmap->map);
	break;
      case 2:
	release_cmap2(cmap->map);
	break;
      case 4:
	release_cmap4(cmap->map);
	break;
      case 6:
	release_cmap6(cmap->map);
	break;
      case 12:
	release_cmap12(cmap->map);
	break;
      default:
	ERROR("Unrecognized OpenType/TrueType cmap format.");
      }
    }
    RELEASE(cmap);
  }

  return;
}


USHORT
tt_cmap_lookup (tt_cmap *cmap, long cc)
{
  USHORT gid = 0;

  ASSERT(cmap);

  if (cc > 0xffffL && cmap->format < 12) {
    WARN("Four bytes charcode not supported in OpenType/TrueType cmap format 0...6.");
    return 0;
  }

  switch (cmap->format) {
  case 0:
    gid = lookup_cmap0(cmap->map,  (USHORT) cc);
    break;
  case 2:
    gid = lookup_cmap2(cmap->map,  (USHORT) cc);
    break;
  case 4:
    gid = lookup_cmap4(cmap->map,  (USHORT) cc);
    break;
  case 6:
    gid = lookup_cmap6(cmap->map,  (USHORT) cc);
    break;
  case 12:
    gid = lookup_cmap12(cmap->map, (ULONG) cc);
    break;
  default:
    ERROR("Unrecognized OpenType/TrueType cmap subtable format");
    break;
  }

  return gid;
}

/* Sorry for placing this here.
 * We need to rewrite TrueType font support code...
 */

#define WBUF_SIZE 1024
static unsigned char wbuf[WBUF_SIZE];

static unsigned char srange_min[2] = {0x00, 0x00};
static unsigned char srange_max[2] = {0xff, 0xff};
static unsigned char lrange_min[4] = {0x00, 0x00, 0x00, 0x00};
static unsigned char lrange_max[4] = {0x7f, 0xff, 0xff, 0xff};

static void
load_cmap4 (struct cmap4 *map,
	    unsigned char *GIDToCIDMap, CMap *cmap)
{
  USHORT  c0, c1, gid, cid;
  USHORT  j, d, segCount;
  USHORT  ch;
  long    i;

  segCount = map->segCountX2 / 2;
  for (i = segCount - 1; i >= 0 ; i--) {
    c0 = map->startCount[i];
    c1 = map->endCount[i];
    d  = map->idRangeOffset[i] / 2 - (segCount - i);
    for (j = 0; j <= c1 - c0; j++) {
      ch = c0 + j;
      if (map->idRangeOffset[i] == 0) {
	gid = (ch + map->idDelta[i]) & 0xffff;
      } else if (c0 == 0xffff && c1 == 0xffff && map->idRangeOffset[i] == 0xffff) {
	/* this is for protection against some old broken fonts... */
	gid = 0;
      } else {
	gid = (map->glyphIndexArray[j+d] +
	       map->idDelta[i]) & 0xffff;
      }
      if (gid != 0 && gid != 0xffff) {
	if (GIDToCIDMap) {
	  cid = ((GIDToCIDMap[2*gid] << 8)|GIDToCIDMap[2*gid+1]);
	  if (cid == 0)
	    WARN("GID %u does not have corresponding CID %u.",
		 gid, cid);
	} else {
	  cid = gid;
	}
	wbuf[0] = 0;
	wbuf[1] = 0;
	wbuf[2] = (ch >> 8) & 0xff;
	wbuf[3] =  ch & 0xff;
	CMap_add_cidchar(cmap, wbuf, 4, cid);
      }
    }
  }

  return;
}

static void
load_cmap12 (struct cmap12 *map,
	     unsigned char *GIDToCIDMap, CMap *cmap)
{
  ULONG   i, ch;  /* LONG ? */
  USHORT  gid, cid;

  for (i = 0; i < map->nGroups; i++) {
    for (ch  = map->groups[i].startCharCode;
	 ch <= map->groups[i].endCharCode;
	 ch++) {
      long  d = ch - map->groups[i].startCharCode;
      gid = (USHORT) ((map->groups[i].startGlyphID + d) & 0xffff);
      if (GIDToCIDMap) {
	cid = ((GIDToCIDMap[2*gid] << 8)|GIDToCIDMap[2*gid+1]);
	if (cid == 0)
	  WARN("GID %u does not have corresponding CID %u.", gid, cid);
      } else {
	cid = gid;
      }
      wbuf[0] = (ch >> 24) & 0xff;
      wbuf[1] = (ch >> 16) & 0xff;
      wbuf[2] = (ch >>  8) & 0xff;
      wbuf[3] = ch & 0xff;
      CMap_add_cidchar(cmap, wbuf, 4, cid);
    }
  }

  return;
}

/* OpenType CIDFont:
 *
 *  We don't use GID for them. OpenType cmap table is for
 *  charcode to GID mapping rather than to-CID mapping.
 */
#include "cidsysinfo.h"

#include "tt_table.h"
#include "cff_types.h"
#include "cff_dict.h"
#include "cff.h"

static int
handle_CIDFont (sfnt *sfont,
		unsigned char **GIDToCIDMap, CIDSysInfo *csi)
{
  cff_font *cffont;
  long      offset, i;
  card16    num_glyphs, gid;
  cff_charsets  *charset;
  unsigned char *map;
  struct tt_maxp_table *maxp;

  ASSERT(csi);

  offset = sfnt_find_table_pos(sfont, "CFF ");
  if (offset == 0) {
    csi->registry = NULL;
    csi->ordering = NULL;
    *GIDToCIDMap  = NULL;
    return 0;
  }

  maxp       = tt_read_maxp_table(sfont);
  num_glyphs = (card16) maxp->numGlyphs;
  RELEASE(maxp);
  if (num_glyphs < 1)
    ERROR("No glyph contained in this font...");

  cffont = cff_open(sfont->stream, offset, 0);
  if (!cffont)
    ERROR("Could not open CFF font...");

  
  if (!(cffont->flag & FONTTYPE_CIDFONT)) {
    cff_close(cffont);
    csi->registry = NULL;
    csi->ordering = NULL;
    *GIDToCIDMap  = NULL;
    return 0;
  }

  if (!cff_dict_known(cffont->topdict, "ROS")) {
    ERROR("No CIDSystemInfo???");
  } else {
    card16 reg, ord;

    reg = (card16) cff_dict_get(cffont->topdict, "ROS", 0);
    ord = (card16) cff_dict_get(cffont->topdict, "ROS", 1);

    csi->registry = cff_get_string(cffont, reg);
    csi->ordering = cff_get_string(cffont, ord);
    csi->supplement = (int) cff_dict_get(cffont->topdict, "ROS", 2);
  }

  cff_read_charsets(cffont);
  charset = cffont->charsets;
  if (!charset) {
    ERROR("No CFF charset data???");
  }

  map     = NEW(num_glyphs * 2, unsigned char);
  memset(map, 0, num_glyphs * 2);
  switch (charset->format) {
  case 0:
    {
      s_SID   *cids; /* CID... */

      cids = charset->data.glyphs;
      for (gid = 1, i = 0;
	   i < charset->num_entries; i++) {
	map[2*gid  ] = (cids[i] >> 8) & 0xff;
	map[2*gid+1] = cids[i] & 0xff;
	gid++;
      }
    }
    break;
  case 1:
    {
      cff_range1 *ranges;
      card16      cid, count;

      ranges = charset->data.range1;
      for (gid = 1, i = 0;
	   i < charset->num_entries; i++) {
	cid   = ranges[i].first;
	count = ranges[i].n_left + 1; /* card8 */
	while (count-- > 0 &&
	       gid <= num_glyphs) {
	  map[2*gid    ] = (cid >> 8) & 0xff;
	  map[2*gid + 1] = cid & 0xff;
	  gid++; cid++;
	}
      }
    }
    break;
  case 2:
    {
      cff_range2 *ranges;
      card16      cid, count;

      ranges = charset->data.range2;
      if (charset->num_entries == 1 &&
	  ranges[0].first == 1) {
	/* "Complete" CIDFont */
	RELEASE(map); map = NULL;
      } else {
	/* Not trivial mapping */
	for (gid = 1, i = 0;
	     i < charset->num_entries; i++) {
	  cid   = ranges[i].first;
	  count = ranges[i].n_left + 1;
	  while (count-- > 0 &&
		 gid <= num_glyphs) {
	    map[gid] = (cid >> 8) & 0xff;
	    map[gid] = cid & 0xff;
	    gid++; cid++;
	  }
	}
	
      }
    }
    break;
  default:
    RELEASE(map); map = NULL;
    ERROR("Unknown CFF charset format...: %d", charset->format);
    break;
  }
  cff_close(cffont);

  *GIDToCIDMap = map;
  return 1;
}

static int is_PUA_or_presentation (unsigned int uni)
{
  return  ((uni >= 0xE000 && uni <= 0xF8FF) || (uni >= 0xFB00 && uni <= 0xFB4F) ||
           (uni >= 0xF0000 && uni <= 0xFFFFD) || (uni >= 0x100000 && uni <= 0x10FFFD));
}

static char*
sfnt_get_glyphname(struct tt_post_table *post, cff_font *cffont, USHORT gid)
{
  char* name = NULL;

  if (post)
    name = tt_get_glyphname(post, gid);

  if (!name && cffont)
    name = cff_get_glyphname(cffont, gid);

  return name;
}

/*
 * Substituted glyphs:
 *
 *  Mapping information stored in cmap_add.
 */

static cff_font *
prepare_CIDFont_from_sfnt(sfnt* sfont)
{
  cff_font *cffont;
  unsigned long offset = 0;

  if (sfont->type != SFNT_TYPE_POSTSCRIPT     ||
      sfnt_read_table_directory(sfont, 0) < 0 ||
      (offset = sfnt_find_table_pos(sfont, "CFF ")) == 0) {
    return NULL;
  }

  cffont = cff_open(sfont->stream, offset, 0);
  if (!cffont)
    return NULL;

  cff_read_charsets(cffont);
  return cffont;
}

static USHORT
add_to_cmap_if_used (CMap *cmap,
                     cff_font *cffont,
                     UsedMapElem *used_chars,
                     USHORT gid,
                     ULONG ch)
{
  USHORT count = 0;
  USHORT cid = cffont ? cff_charsets_lookup_inverse(cffont, gid) : gid;
  if (IS_USED_CHAR(used_chars, cid)) {
    int len;
    unsigned char *p = wbuf + 2;

    count++;

    wbuf[0] = (cid >> 8) & 0xff;
    wbuf[1] = (cid & 0xff);
    len = UC_sput_UTF16BE((long) ch, &p, wbuf + WBUF_SIZE);
    CMap_add_bfchar(cmap, wbuf, 2, wbuf + 2, len);

    /* Skip PUA characters and alphabetic presentation forms, allowing
     * handle_subst_glyphs() as it might find better mapping. Fixes the
     * mapping of ligatures encoded in PUA in fonts like Linux Libertine
     * and old Adobe fonts.
     */
    if (!is_PUA_or_presentation(ch)) {
      /* Avoid duplicate entry
       * There are problem when two Unicode code is mapped to
       * single glyph...
       */
      REMOVE_FROM_USED_CHARS(used_chars, cid);
    }
  }

  return count;
}

static USHORT
create_ToUnicode_cmap4 (CMap *cmap,
                        struct cmap4 *map,
                        UsedMapElem *used_chars,
                        cff_font *cffont)
{
  USHORT count = 0, segCount = map->segCountX2 / 2;
  USHORT i, j;

  for (i = 0; i < segCount; i++) {
    USHORT c0 = map->startCount[i];
    USHORT c1 = map->endCount[i];
    USHORT d  = map->idRangeOffset[i] / 2 - (segCount - i);
    for (j = 0; j <= c1 - c0; j++) {
      USHORT ch = c0 + j;
      USHORT gid;

      if (map->idRangeOffset[i] == 0) {
        gid = (ch + map->idDelta[i]) & 0xffff;
      } else if (c0 == 0xffff && c1 == 0xffff && map->idRangeOffset[i] == 0xffff) {
        /* this is for protection against some old broken fonts... */
        gid = 0;
      } else {
        gid = (map->glyphIndexArray[j + d] + map->idDelta[i]) & 0xffff;
      }

      count += add_to_cmap_if_used(cmap, cffont, used_chars, gid, ch);
    }
  }

  return count;
}

static USHORT
create_ToUnicode_cmap12 (CMap *cmap,
                         struct cmap12 *map,
                         UsedMapElem *used_chars,
                         cff_font *cffont)
{
  ULONG i, ch, count = 0;

  for (i = 0; i < map->nGroups; i++) {
    for (ch  = map->groups[i].startCharCode;
         ch <= map->groups[i].endCharCode; ch++) {
      long d = ch - map->groups[i].startCharCode;
      USHORT gid = (USHORT) ((map->groups[i].startGlyphID + d) & 0xffff);
      count += add_to_cmap_if_used(cmap, cffont, used_chars, gid, ch);
    }
  }

  return count;
}

static int
create_ToUnicode_cmap (tt_cmap *ttcmap,
                       const char *cmap_name,
					   const char *cmap_ext,
					   CIDSysInfo *csi,
                       CMap *cmap_add,
                       const UsedMapElem *used_chars,
                       sfnt *sfont,
                       CMap *code_to_cid_cmap,
					   const char *cmap_path,
					   luacharmap *luamap)
{
  int retval = 0;
  CMap     *cmap;
  USHORT    count = 0;
  cff_font *cffont = prepare_CIDFont_from_sfnt(sfont);
  char      is_cidfont = cffont && (cffont->flag & FONTTYPE_CIDFONT);

  cmap = CMap_new();
  CMap_set_name (cmap, cmap_name);
  CMap_set_wmode(cmap, 0);
  CMap_set_type (cmap, CMAP_TYPE_TO_UNICODE);
  CMap_set_CIDSysInfo(cmap, csi);
  CMap_add_codespacerange(cmap, srange_min, srange_max, 2);

  if (luamap) {
	  USHORT i;
	  luamaptype *map, *current;
	  luacharmap *p;
	  for (i = 0; i < USED_CHARS_BUF_SIZE; i++) {
		  int j;

		  if (used_chars[i] == 0)
			  continue;

		  for (j = 0; j < BITS_PER_USED_ELEM; j++) {
			  USHORT cid = BITS_PER_USED_ELEM * i + j;
			  int ch;

			  if (!IS_USED_CHAR(used_chars, cid))
				  continue;
			  ch = cid;
			  current = NULL;
			  p = luamap;
			  while ( p) {
				  map = LuaMap_cache_get(p->luamap_idx);
				  HASH_FIND_INT(map, &ch, current);  /* s: output pointer */
				  if (current)
					  break;
				  p = p->next;
			  }
			  if (current) {
				  if (current->tu_count > 0) {
					  unsigned char* p = wbuf + 2;
					  int  k;
					  long len = 0;
					  for (k = 0; k < current->tu_count; ++k) {
						  len += UC_sput_UTF16BE(current->tounicode[k], &p, wbuf + WBUF_SIZE);
					  }
					  wbuf[0] = (cid >> 8) & 0xff;
					  wbuf[1] = cid & 0xff;
					  CMap_add_bfchar(cmap, wbuf, 2, wbuf + 2, len);
				  }
				  count++;
			  }
		  }
	  }
  } else if (code_to_cid_cmap && cffont && is_cidfont) {
    USHORT i;
    for (i = 0; i < USED_CHARS_BUF_SIZE; i++) {
      int j;

      if (used_chars[i] == 0)
        continue;

      for (j = 0; j < BITS_PER_USED_ELEM; j++) {
        USHORT cid = BITS_PER_USED_ELEM * i + j;
        int ch;

        if (!IS_USED_CHAR(used_chars, cid))
          continue;

        ch = CMap_reverse_decode(code_to_cid_cmap, cid);
        if (ch >= 0) {
          long len;
          unsigned char *p = wbuf + 2;
          wbuf[0] = (cid >> 8) & 0xff;
          wbuf[1] =  cid & 0xff;
          len = UC_sput_UTF16BE((long)ch, &p, wbuf + WBUF_SIZE);
          CMap_add_bfchar(cmap, wbuf, 2, wbuf + 2, len);
          count++;
        }
      }
    }
  } else {
    static UsedMapElem used_chars_copy[USED_CHARS_BUF_SIZE];
    memcpy(used_chars_copy, used_chars, sizeof(used_chars_copy));

    /* For create_ToUnicode_cmap{4,12}(), cffont is for GID -> CID lookup,
     * so it is only needed for CID fonts. */
    switch (ttcmap->format) {
      case 4:
        count = create_ToUnicode_cmap4(cmap, ttcmap->map, used_chars_copy,
                                       is_cidfont ? cffont : NULL);
        break;
      case 12:
        count = create_ToUnicode_cmap12(cmap, ttcmap->map, used_chars_copy,
                                        is_cidfont ? cffont : NULL);
        break;
    }
  }

  if (count > 0) {
	  retval = CMap_create_file(cmap_path, cmap_name, cmap_ext,cmap, 0);
  }
  CMap_release(cmap);

  if (cffont)
    cff_close(cffont);

  return retval;
}

typedef struct {
  short platform;
  short encoding;
} cmap_plat_enc_rec;

static cmap_plat_enc_rec cmap_plat_encs[] = {
    { 3, 10 },
    { 0, 3 },
    { 0, 0 },
    { 3, 1 },
    { 0, 1 }
};

int
otf_create_ToUnicode_stream (sfnt *sfont,
							 const char *font_name,
							 CMap		*code_to_cid_cmap,
							 CIDSysInfo *csi,
                             const UsedMapElem *used_chars,
							 const char *path,
							 const char *cmap_name,
							 const char *cmap_ext,
							 void *luamap)
{
  CMap       *cmap_add;
  int         cmap_add_id;
  tt_cmap    *ttcmap;
  int         i, cmap_type, cmap_ret = 0;
  luacharmap *lmap;

  if (!font_name || !used_chars || !csi)
    return 0;


  cmap_add = NULL;

  CMap_set_silent(1); /* many warnings without this... */
  lmap = (luacharmap *)luamap;
  if (luamap && (lmap->luamap_idx >= 0)) {
	  ttcmap = NULL;
	  cmap_ret = create_ToUnicode_cmap(ttcmap, cmap_name, cmap_ext, csi, cmap_add, used_chars,
		  sfont, code_to_cid_cmap, path, lmap);
  }
  else {
	  for (i = 0; i < sizeof(cmap_plat_encs) / sizeof(cmap_plat_enc_rec); ++i) {
		  ttcmap = tt_cmap_read(sfont, cmap_plat_encs[i].platform, cmap_plat_encs[i].encoding);
		  if (!ttcmap)
			  continue;

		  if (ttcmap->format == 4 || ttcmap->format == 12) {
			  cmap_ret = create_ToUnicode_cmap(ttcmap, cmap_name, cmap_ext, csi, cmap_add, used_chars,
				  sfont, code_to_cid_cmap, path, NULL);
			  break;
		  }
	  }
	  tt_cmap_release(ttcmap);
  }
  if (cmap_ret == 0)
	  WARN("Unable to read OpenType/TrueType Unicode cmap table.");
  CMap_set_silent(0);

  return 1;
}

/* Must be smaller than (WBUF_SIZE-2)/8 */
#define MAX_UNICODES 16

struct gent
{
  USHORT gid;
  long   ucv; /* assigned PUA unicode */

  int    num_unicodes;
  long   unicodes[MAX_UNICODES];
};

static void
create_cmaps (CMap *cmap, CMap *tounicode,
	      struct ht_table *unencoded, unsigned char *GIDToCIDMap)
{
  struct ht_iter iter;

  ASSERT(cmap && unencoded);

  if (ht_set_iter(unencoded, &iter) < 0)
    return;

  CMap_set_silent(1); /* many warnings without this... */

  do {
    struct gent   *glyph;
    unsigned char *ucv;
    int            i, len;
    unsigned char  *p, *endptr;
    CID            cid;

    glyph = (struct gent *)   ht_iter_getval(&iter);
    ucv   = (unsigned char *) ht_iter_getkey(&iter, &len);

    if (GIDToCIDMap) {
      cid = ((GIDToCIDMap[2 * glyph->gid] << 8)|GIDToCIDMap[2 * glyph->gid + 1]);
      if (cid == 0)
	WARN("Glyph gid=%u does not have corresponding CID.", glyph->gid);
    } else {
      cid = glyph->gid;
    }

    CMap_add_cidchar(cmap, ucv, 4, cid);

    if (tounicode) {
      wbuf[0] = (cid >> 8) & 0xff;
      wbuf[1] = cid & 0xff;
      p       = wbuf + 2;
      endptr  = wbuf + WBUF_SIZE;
      len     = 0;
      for (i = 0; i < glyph->num_unicodes; i++) {
	len += UC_sput_UTF16BE(glyph->unicodes[i], &p, endptr);
      }
      CMap_add_bfchar(tounicode, wbuf, 2, wbuf + 2, len);
    }
  } while (ht_iter_next(&iter) >= 0);

  CMap_set_silent(0);

  ht_clear_iter(&iter);
}

static void
add_glyph (struct ht_table *unencoded,
	   USHORT gid, long ucv, int num_unicodes, long *unicodes)
{
  struct gent *glyph;
  int i;

  ASSERT(unencoded);

  if (gid == 0 || num_unicodes < 1) {
    return;
  }

  wbuf[0] = (ucv >> 24) & 0xff;
  wbuf[1] = (ucv >> 16) & 0xff;
  wbuf[2] = (ucv >>  8) & 0xff;
  wbuf[3] =  ucv & 0xff;

  glyph = NEW(1, struct gent);
  glyph->gid = gid;
  glyph->num_unicodes = num_unicodes;
  for (i = 0;
       i < num_unicodes && i < MAX_UNICODES; i++) {
    glyph->unicodes[i] = unicodes[i];
  }

  ht_append_table(unencoded, wbuf, 4, glyph);
}

/* This seriously affects speed... */
static struct gent *
find_glyph (struct ht_table *unencoded, long ucv)
{
  ASSERT(unencoded);

  wbuf[0] = (ucv >> 24) & 0xff;
  wbuf[1] = (ucv >> 16) & 0xff;
  wbuf[2] = (ucv >>  8) & 0xff;
  wbuf[3] =  ucv & 0xff;

  return (struct gent *) ht_lookup_table(unencoded, wbuf, 4);
}

static CMap*
load_base_CMap (const char *cmap_name, int wmode,
		CIDSysInfo *csi, unsigned char *GIDToCIDMap,
		tt_cmap *ttcmap)
{
    CMap  *cmap;

    cmap = CMap_new();
    CMap_set_name (cmap, cmap_name);
    CMap_set_type (cmap, CMAP_TYPE_CODE_TO_CID);
    CMap_set_wmode(cmap, wmode);
    CMap_add_codespacerange(cmap, lrange_min, lrange_max, 4);

    if (csi) { /* CID */
      CMap_set_CIDSysInfo(cmap, csi);
    } else {
      CMap_set_CIDSysInfo(cmap, &CSI_IDENTITY);
    }

    if (ttcmap->format == 12) {
      load_cmap12(ttcmap->map, GIDToCIDMap, cmap);
    } else if (ttcmap->format == 4) {
      load_cmap4(ttcmap->map, GIDToCIDMap, cmap);
    }

  return cmap;
}

static void CDECL
hval_free (void *hval)
{
  RELEASE(hval);
}

CMap*
otf_load_Unicode_CMap (const char *map_name, sfnt *sfont,
		       const char *otl_tags, int wmode)
{
  int    tounicode_id = -1, is_cidfont = 0;
  char  *base_name = NULL, *cmap_name = NULL;
  char  *tounicode_name = NULL;
  FILE  *fp = NULL;
  tt_cmap       *ttcmap;
  CMap          *cmap, *base, *tounicode = NULL;
  CIDSysInfo     csi = {NULL, NULL, 0};
  unsigned char *GIDToCIDMap = NULL;

  if (!map_name)
    return NULL;

  cmap = NULL;

  base_name = NEW(strlen(map_name)+strlen("-UCS4-H")+5, char);
  if (wmode)
    sprintf(base_name, "%s,%03d-UCS4-V", map_name, 0);
  else {
    sprintf(base_name, "%s,%03d-UCS4-H", map_name, 0);
  }

  if (otl_tags) {
    cmap_name = NEW(strlen(map_name)+strlen(otl_tags)+strlen("-UCS4-H")+6, char);
    if (wmode)
      sprintf(cmap_name, "%s,%03d,%s-UCS4-V", map_name, 0, otl_tags);
    else
      sprintf(cmap_name, "%s,%03d,%s-UCS4-H", map_name, 0, otl_tags);
  }
  else {
    cmap_name = NEW(strlen(base_name)+1, char);
    strcpy(cmap_name, base_name);
  }

  if (sfont->type == SFNT_TYPE_POSTSCRIPT) {
    is_cidfont = handle_CIDFont(sfont, &GIDToCIDMap, &csi);
  } else {
    is_cidfont = 0;
  }

  if (is_cidfont) {
    tounicode_name = NULL;
  } else {
    tounicode_name = NEW(strlen(map_name)+strlen("-UTF16")+5, char);
    sprintf(tounicode_name, "%s,%03d-UTF16", map_name, 0);
  }

  ttcmap = tt_cmap_read(sfont, 3, 10); /* Microsoft UCS4 */
  if (!ttcmap) {
    ttcmap = tt_cmap_read(sfont, 3, 1); /* Microsoft UCS2 */
    if (!ttcmap) {
      ttcmap = tt_cmap_read(sfont, 0, 3); /* Unicode 2.0 or later */
      if (!ttcmap) {
		  ttcmap = tt_cmap_read(sfont, 3, 0); /* Windows Symbol encoding */
		  if (!ttcmap) {
			  ttcmap = tt_cmap_read(sfont, 1, 0); /*  Mac encoding */
			  if (!ttcmap) {
				  RELEASE(base_name);
				  return NULL;
			  }
		  }
      }
    }
  }
  cmap = load_base_CMap(base_name, wmode,
			   (is_cidfont ? &csi : NULL),
			   GIDToCIDMap, ttcmap);
  if (cmap == NULL)
    ERROR("Failed to read OpenType/TrueType cmap table.");

  if (!otl_tags) {
    RELEASE(cmap_name);
    RELEASE(base_name);
    if (GIDToCIDMap)
      RELEASE(GIDToCIDMap);
    if (tounicode_name)
      RELEASE(tounicode_name);
    if (is_cidfont) {
      if (csi.registry)
	RELEASE(csi.registry);
      if (csi.ordering)
	RELEASE(csi.ordering);
    }
    tt_cmap_release(ttcmap);

    return cmap;
  }

  base = cmap;

  cmap = CMap_new();
  CMap_set_name (cmap, cmap_name);
  CMap_set_type (cmap, CMAP_TYPE_CODE_TO_CID);
  CMap_set_wmode(cmap, wmode);
  /* CMap_add_codespacerange(cmap, lrange_min, lrange_max, 4); */
  CMap_set_usecmap(cmap, base);
  CMap_add_cidchar(cmap, lrange_max, 4, 0); /* FIXME */

  if (is_cidfont) {
    CMap_set_CIDSysInfo(cmap, &csi);
    if (csi.registry)
      RELEASE(csi.registry);
    if (csi.ordering)
      RELEASE(csi.ordering);
  } else {
    CMap_set_CIDSysInfo(cmap, &CSI_IDENTITY);
  }

  if (!is_cidfont && tounicode_id < 0) /* New */
    CMap_release(tounicode);

  tt_cmap_release(ttcmap);

  if (GIDToCIDMap)
    RELEASE(GIDToCIDMap);
  if (base_name)
    RELEASE(base_name);
  if (cmap_name)
    RELEASE(cmap_name);
  if (tounicode_name)
    RELEASE(tounicode_name);

  return cmap;
}

extern CMap*      ttf_load_Native_CMap      (const char *map_name, sfnt *sfont)
{
  char  *base_name = NULL, *cmap_name = NULL;
  FILE  *fp = NULL;
  tt_cmap       *ttcmap;
  CMap          *cmap;

  if (!map_name)
    return NULL;

  cmap = NULL;

  base_name = NEW(strlen(map_name)+strlen("-UCS4-H")+5, char);
  sprintf(base_name, "%s,%03d-UCS4-V", map_name, 0);

  ttcmap = tt_cmap_read(sfont, 3, 0); /* Windows Symbol encoding */
  if (!ttcmap) {
    ttcmap = tt_cmap_read(sfont, 1, 0); /* Mac encoding */
    if (!ttcmap) {
      ERROR("Unable to read TrueType Native cmap table.");
    }
  }
  cmap = load_base_CMap(base_name, 0,NULL, NULL, ttcmap);
  if (cmap == NULL)
    ERROR("Failed to read TrueType cmap table.");

  RELEASE(cmap_name);
  RELEASE(base_name);
  tt_cmap_release(ttcmap);

  return cmap;
}
