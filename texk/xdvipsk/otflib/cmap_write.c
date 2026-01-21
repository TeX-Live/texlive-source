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
 * References:
 *
 *  PostScript Language Reference Manual, 3rd. ed. (Adobe Systems Inc.)
 *    5.11.4 CMap Dictionaries
 *    5.11.5 FMapType 9 Composite Fonts
 *  Building CMap Files for CID-Keyed Fonts, Adobe Technical Note #5099
 *  CID-Keyed Font Technology Overview, Adobe Technical Note #5092
 *  Adobe CMap and CIDFont Files Specification, Adobe Technical Specification #5014
 *
 *  Undefined Character Handling:
 *    PLRM 3rd. ed., sec. 5.11.5., "Handling Undefined Characters"
 *
 */

#include <config.h>
#
#include <string.h>

#include "system.h"
#include "mem.h"
#include "error.h"
#include "dpxutil.h"

#include "cmap_p.h"
#include "cmap.h"

#include "cmap_write.h"

struct sbuf {
  char *buf;
  char *curptr;
  char *limptr;
};

static void cmap_add_stream(FILE *fn, const void *buf, size_t buflen)
{
  fwrite(buf, 1, buflen, fn);
}

static int write_map (mapDef *mtab, int count,
                      unsigned char *codestr, int depth,
                      int detectranges, struct sbuf *wbuf, FILE *stream);

static int
block_count (mapDef *mtab, int c)
{
  int count = 0, n;

  n  = mtab[c].len - 1;
  c += 1;
  for (; c < 256; c++) {
    if (LOOKUP_CONTINUE(mtab[c].flag) ||
        !MAP_DEFINED(mtab[c].flag)     ||
        (MAP_TYPE(mtab[c].flag) != MAP_IS_CID &&
        MAP_TYPE(mtab[c].flag) != MAP_IS_CODE) ||
        mtab[c-1].len != mtab[c].len)
      break;
    else if (!memcmp(mtab[c-1].code, mtab[c].code, n) &&
      mtab[c-1].code[n] < 255 &&
      mtab[c-1].code[n] + 1 == mtab[c].code[n])
      count++;
    else {
      break;
    }
  }

  return count;
}

static int
write_map (mapDef *mtab, int count,
           unsigned char *codestr, int depth, int detectranges,
           struct sbuf *wbuf, FILE *stream)
{
  int     c, i, block_length;
  mapDef *mtab1;
  /* Must be greater than 1 */
#define BLOCK_LEN_MIN 2
  struct {
    int start, count;
  } blocks[256/BLOCK_LEN_MIN+1];
  int num_blocks = 0;

  for (c = 0; c < 256; c++) {
    codestr[depth] = (unsigned char) (c & 0xff);
    if (LOOKUP_CONTINUE(mtab[c].flag)) {
      mtab1 = mtab[c].next;
      count = write_map(mtab1, count,
                        codestr, depth + 1, detectranges, wbuf, stream);
    } else {
      if (MAP_DEFINED(mtab[c].flag)) {
        switch (MAP_TYPE(mtab[c].flag)) {
        case MAP_IS_CID: case MAP_IS_CODE:
          if (detectranges) {
            block_length = block_count(mtab, c);
            if (block_length >= BLOCK_LEN_MIN) {
              blocks[num_blocks].start = c;
              blocks[num_blocks].count = block_length;
              num_blocks++;
              c += block_length;
            } else {
              *(wbuf->curptr)++ = '<';
              for (i = 0; i <= depth; i++)
                sputx(codestr[i], &(wbuf->curptr), wbuf->limptr);
              *(wbuf->curptr)++ = '>';
              *(wbuf->curptr)++ = ' ';
              *(wbuf->curptr)++ = '<';
              for (i = 0; i < mtab[c].len; i++)
                sputx(mtab[c].code[i], &(wbuf->curptr), wbuf->limptr);
              *(wbuf->curptr)++ = '>';
              *(wbuf->curptr)++ = '\n';
              count++;
            }
          } else {
            *(wbuf->curptr)++ = '<';
            for (i = 0; i <= depth; i++)
              sputx(codestr[i], &(wbuf->curptr), wbuf->limptr);
            *(wbuf->curptr)++ = '>';
            *(wbuf->curptr)++ = ' ';
            *(wbuf->curptr)++ = '<';
            for (i = 0; i < mtab[c].len; i++)
              sputx(mtab[c].code[i], &(wbuf->curptr), wbuf->limptr);
            *(wbuf->curptr)++ = '>';
            *(wbuf->curptr)++ = '\n';
            count++;
          }
          break;
        case MAP_IS_NAME:
          ERROR("%s: Unexpected error...", CMAP_DEBUG_STR);
          break;
        case MAP_IS_NOTDEF:
          break;
        default:
          ERROR("%s: Unknown mapping type: %d",
                CMAP_DEBUG_STR, MAP_TYPE(mtab[c].flag));
        }
      }
    }

    /* Flush if necessary */
    if (count >= 100 || wbuf->curptr >= wbuf->limptr ) {
      char fmt_buf[32];
      if (count > 100)
        ERROR("Unexpected error....: %d", count);
      sprintf(fmt_buf, "%d beginbfchar\n", count);
      cmap_add_stream(stream, fmt_buf,  strlen(fmt_buf));
      cmap_add_stream(stream, wbuf->buf, (long) (wbuf->curptr - wbuf->buf));
      wbuf->curptr = wbuf->buf;
      cmap_add_stream(stream, "endbfchar\n", strlen("endbfchar\n"));
      count = 0;
    }
  }

  if (num_blocks > 0) {
    char fmt_buf[32];

    if (count > 0) {
      sprintf(fmt_buf, "%d beginbfchar\n", count);
      cmap_add_stream(stream, fmt_buf,  strlen(fmt_buf));
      cmap_add_stream(stream, wbuf->buf, (long) (wbuf->curptr - wbuf->buf));
      wbuf->curptr = wbuf->buf;
      cmap_add_stream(stream, "endbfchar\n", strlen("endbfchar\n"));
      count = 0;
    }
    sprintf(fmt_buf, "%d beginbfrange\n", num_blocks);
    cmap_add_stream(stream, fmt_buf, strlen(fmt_buf));
    for (i = 0; i < num_blocks; i++) {
      int j;

      c = blocks[i].start;
      *(wbuf->curptr)++ = '<';
      for (j = 0; j < depth; j++)
        sputx(codestr[j], &(wbuf->curptr), wbuf->limptr);
      sputx((unsigned char)c, &(wbuf->curptr), wbuf->limptr);
      *(wbuf->curptr)++ = '>';
      *(wbuf->curptr)++ = ' ';
      *(wbuf->curptr)++ = '<';
      for (j = 0; j < depth; j++)
        sputx(codestr[j], &(wbuf->curptr), wbuf->limptr);
      sputx((unsigned char)(c + blocks[i].count), &(wbuf->curptr), wbuf->limptr);
      *(wbuf->curptr)++ = '>';
      *(wbuf->curptr)++ = ' ';
      *(wbuf->curptr)++ = '<';
      for (j = 0; j < mtab[c].len; j++)
        sputx(mtab[c].code[j], &(wbuf->curptr), wbuf->limptr);
      *(wbuf->curptr)++ = '>';
      *(wbuf->curptr)++ = '\n';
    }
    cmap_add_stream(stream, wbuf->buf, (long) (wbuf->curptr - wbuf->buf));
    wbuf->curptr = wbuf->buf;
    cmap_add_stream(stream, "endbfrange\n", strlen("endbfrange\n"));
  }

  return count;
}

#define CMAP_BEGIN "\
/CIDInit /ProcSet findresource begin\n\
12 dict begin\n\
begincmap\n\
"

#define CMAP_END "\
endcmap\n\
CMapName currentdict /CMap defineresource pop\n\
end\n\
end\n\
"

int
CMap_create_file(const char *map_path, const char *map_name, const char *map_ext, CMap *cmap, int detectranges)
{
  FILE *stream;
  char *map_file;
  CIDSysInfo      *csi;
  struct sbuf      wbuf;
  struct rangeDef *ranges;
  unsigned char   *codestr;
  int              i, j, count = 0;

  if (!cmap || !CMap_is_valid(cmap)) {
    WARN("Invalid CMap");
    return 0;
  }

  if (cmap->type == CMAP_TYPE_IDENTITY)
    return 0;

  csi = CMap_get_CIDSysInfo(cmap);
  if (!csi) {
    csi = (cmap->type != CMAP_TYPE_TO_UNICODE) ?
      &CSI_IDENTITY : &CSI_UNICODE;
  }

  map_file = NEW(strlen(map_path) + strlen(map_name) + strlen(map_ext) + 5, char);
  strcpy(map_file,map_path);
  strcat(map_file,"/");
  strcat(map_file,map_name);
  if (strlen(map_ext) > 0) {
    strcat(map_file, ".");
    strcat(map_file, map_ext);
  }
  stream = fopen(map_file,"w");
  RELEASE(map_file);
  if ( ! stream )
    ERROR("Error creating ToUnicode cmap file %s", map_name);

#define WBUF_SIZE 4096
  wbuf.buf = NEW(WBUF_SIZE, char);
  codestr  = NEW(cmap->profile.maxBytesIn, unsigned char);
  memset(codestr, 0, cmap->profile.maxBytesIn);

  wbuf.curptr = wbuf.buf;
  wbuf.limptr = wbuf.buf + WBUF_SIZE -
    2 * (cmap->profile.maxBytesIn + cmap->profile.maxBytesOut) + 16;

  /* Start CMap */
  cmap_add_stream(stream, (const void *) CMAP_BEGIN, strlen(CMAP_BEGIN));

  wbuf.curptr += sprintf(wbuf.curptr, "/CMapName /%s def\n", cmap->name);
  wbuf.curptr += sprintf(wbuf.curptr, "/CMapType %d def\n" , cmap->type);
  if (cmap->wmode != 0 &&
      cmap->type != CMAP_TYPE_TO_UNICODE)
    wbuf.curptr += sprintf(wbuf.curptr, "/WMode %d def\n", cmap->wmode);

#define CMAP_CSI_FMT "/CIDSystemInfo <<\n\
  /Registry (%s)\n\
  /Ordering (%s)\n\
  /Supplement %d\n\
>> def\n"
  wbuf.curptr += sprintf(wbuf.curptr, CMAP_CSI_FMT,
                         csi->registry, csi->ordering, csi->supplement);
  cmap_add_stream(stream, wbuf.buf, (long)(wbuf.curptr - wbuf.buf));
  wbuf.curptr = wbuf.buf;

  /* codespacerange */
  ranges = cmap->codespace.ranges;
  wbuf.curptr += sprintf(wbuf.curptr,
                         "%d begincodespacerange\n", cmap->codespace.num);
  for (i = 0; i < cmap->codespace.num; i++) {
    *(wbuf.curptr)++ = '<';
    for (j = 0; j < ranges[i].dim; j++) {
      sputx(ranges[i].codeLo[j], &(wbuf.curptr), wbuf.limptr);
    }
    *(wbuf.curptr)++ = '>';
    *(wbuf.curptr)++ = ' ';
    *(wbuf.curptr)++ = '<';
    for (j = 0; j < ranges[i].dim; j++) {
      sputx(ranges[i].codeHi[j], &(wbuf.curptr), wbuf.limptr);
    }
    *(wbuf.curptr)++ = '>';
    *(wbuf.curptr)++ = '\n';
  }
  cmap_add_stream(stream, wbuf.buf, (long)(wbuf.curptr - wbuf.buf));
  wbuf.curptr = wbuf.buf;
  cmap_add_stream(stream,
                  "endcodespacerange\n", strlen("endcodespacerange\n"));

  /* CMap body */
  if (cmap->mapTbl) {
    count = write_map(cmap->mapTbl,
                      0, codestr, 0, detectranges, &wbuf, stream); /* Top node */
    if (count > 0) { /* Flush */
      char fmt_buf[32];
      if (count > 100)
        ERROR("Unexpected error....: %d", count);
      sprintf(fmt_buf, "%d beginbfchar\n", count);
      cmap_add_stream(stream, fmt_buf,  strlen(fmt_buf));
      cmap_add_stream(stream,
                      wbuf.buf, (long) (wbuf.curptr - wbuf.buf));
      cmap_add_stream(stream,
                      "endbfchar\n", strlen("endbfchar\n"));
      count = 0;
      wbuf.curptr = wbuf.buf;
    }
  }
  /* End CMap */
  cmap_add_stream(stream, CMAP_END, strlen(CMAP_END));

  RELEASE(codestr);
  RELEASE(wbuf.buf);

  return 1;
}
