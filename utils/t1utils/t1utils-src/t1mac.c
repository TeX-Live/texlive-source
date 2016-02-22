/* t1mac
 *
 * This program converts Type 1 fonts in PFA or PFB format into Macintosh Type
 * 1 fonts stored in MacBinary (I or II), AppleSingle, AppleDouble, BinHex, or
 * raw resource fork format.
 *
 * Copyright (c) 2000-2013 Eddie Kohler
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, subject to the
 * conditions listed in the Click LICENSE file, which is available in full at
 * http://github.com/kohler/click/blob/master/LICENSE. The conditions
 * include: you must preserve this copyright notice, and you cannot mention
 * the copyright holders in advertising related to the Software without
 * their permission. The Software is provided WITHOUT ANY WARRANTY, EXPRESS
 * OR IMPLIED. This notice is a summary of the Click LICENSE file; the
 * license in that file is binding.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#if defined(_MSDOS) || defined(_WIN32)
# include <fcntl.h>
# include <io.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>
#include <lcdf/clp.h>
#include "t1lib.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char byte;

/* resource fork layout */
#define RFORK_HEADERLEN 256
#define RFORK_MAP_RESERVEDLEN 22
#define RFORK_MAP_HEADERLEN 28
#define RFORK_RTYPE_LEN 8
#define RFORK_RSRC_LEN 12

/* Macintosh times are # seconds since 1/1/1904, not 1/1/1970 */
#define MAC_TIME_DELTA 2082844800

/* POST resource information */
#define POST_ASCII 1
#define POST_BINARY 2
#define POST_END 5

/* Adobe font file information */
#define T1_FILETYPE 0x4C57464E	/* LWFN */
#define T1_FILECREATOR 0x54315554 /* T1UT */
#define T1_FINDERFLAGS 33	/* Bundle + Inited */

#define MAX_RSRC_LEN 2048
static byte rbuf[MAX_RSRC_LEN];
static int rbufpos;
static int blocktyp;

static char *font_name;

/* information about the resources being built */
typedef struct Rsrc {
  int32_t type;
  int id;
  int attrs;
  int data_offset;
  uint32_t data_len;
  int next_in_type;
  int next_type;
} Rsrc;
static Rsrc *rsrc = 0;
static int nrsrc = 0;
static int rsrc_cap = 0;
static int cur_post_id = 0;

/* output resource fork */
static FILE *rfork_f = 0;

/* ICN# data */
static const unsigned char icon_bw_data[] = {
0,0,0,0,255,255,255,255,128,0,0,1,128,0,0,1,128,0,0,1,
128,0,0,1,128,0,0,1,128,0,0,1,128,0,0,1,128,0,0,33,
128,0,0,97,128,0,0,225,128,0,1,225,128,0,3,225,128,0,7,225,
128,0,15,225,128,0,31,225,128,0,55,225,159,128,103,249,144,128,199,9,
240,129,135,15,0,131,7,0,0,134,7,0,15,140,7,240,8,31,255,240,
8,63,255,240,8,96,7,240,8,192,7,240,9,192,15,240,11,224,31,240,
15,240,63,240,15,255,255,240,0,0,0,0,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,240,255,255,15,240,255,255,15,0,255,255,0,0,255,255,0,
15,255,255,240,15,255,255,240,15,255,255,240,15,255,255,240,15,255,255,240,
15,255,255,240,15,255,255,240,15,255,255,240,15,255,255,240,};

static const unsigned char small_icon_bw_data[] = {
255,255,128,1,128,1,128,1,128,5,128,13,128,29,128,61,128,125,184,253,
201,179,59,60,39,252,44,60,60,124,63,252,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,207,243,63,252,63,252,63,252,
63,252,63,252,};

static const unsigned char icon_8_data[] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,92,92,92,92,92,92,92,92,
92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,
92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,
92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,105,92,92,92,92,
92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,
92,92,92,92,92,92,92,105,92,92,92,92,92,92,92,92,92,92,92,92,
92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,105,
92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,
92,92,92,92,92,92,92,92,92,92,92,105,92,92,92,92,92,92,92,92,
92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,
92,92,92,105,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,
92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,105,92,92,92,92,
92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,
92,92,92,92,92,92,92,105,92,92,92,92,92,92,92,92,92,92,92,92,
92,92,92,92,92,92,92,92,92,92,92,92,92,92,5,92,92,92,92,105,
92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,
92,92,92,92,92,5,5,92,92,92,92,105,92,92,92,92,92,92,92,92,
92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,5,5,5,92,
92,92,92,105,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,
92,92,92,92,92,92,92,5,5,5,5,92,92,92,92,105,92,92,92,92,
92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,5,5,
5,5,5,92,92,92,92,105,92,92,92,92,92,92,92,92,92,92,92,92,
92,92,92,92,92,92,92,92,92,5,5,5,5,5,5,92,92,92,92,105,
92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,
5,5,5,5,5,5,5,92,92,92,92,105,92,92,92,92,92,92,92,92,
92,92,92,92,92,92,92,92,92,92,92,5,5,5,5,5,5,5,5,92,
92,92,92,105,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,
92,92,5,5,92,5,5,5,5,5,5,92,92,92,92,105,92,92,92,105,
105,105,105,105,92,92,92,92,92,92,92,92,92,5,5,92,92,5,5,105,
105,105,105,105,92,92,92,105,92,92,92,105,0,0,0,0,92,92,92,92,
92,92,92,92,5,5,92,92,92,5,5,105,0,0,0,0,92,92,92,105,
105,105,105,105,0,0,0,0,92,92,92,92,92,92,92,5,5,92,92,92,
92,5,5,105,0,0,0,0,105,105,105,105,0,0,0,0,0,0,0,0,
92,92,92,92,92,92,5,5,92,92,92,92,92,5,5,105,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,92,92,92,92,92,5,5,92,
92,92,92,92,92,5,5,105,0,0,0,0,0,0,0,0,0,0,0,0,
92,92,92,92,92,92,92,92,5,5,92,92,92,92,92,92,92,5,5,5,
5,5,5,92,0,0,0,0,0,0,0,0,92,92,92,92,92,92,92,5,
5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,105,0,0,0,0,
0,0,0,0,92,92,92,92,92,92,5,5,5,5,5,5,5,5,5,5,
5,5,5,5,5,5,5,105,0,0,0,0,0,0,0,0,92,92,92,92,
92,5,5,92,92,92,92,92,92,92,92,92,92,5,5,5,5,5,5,105,
0,0,0,0,0,0,0,0,92,92,92,92,5,5,92,92,92,92,92,92,
92,92,92,92,92,5,5,5,5,5,5,105,0,0,0,0,0,0,0,0,
92,92,92,5,5,5,92,92,92,92,92,92,92,92,92,92,5,5,5,5,
5,5,5,105,0,0,0,0,0,0,0,0,92,92,5,5,5,5,5,92,
92,92,92,92,92,92,92,5,5,5,5,5,5,5,5,105,0,0,0,0,
0,0,0,0,92,5,5,5,5,5,5,5,92,92,92,92,92,92,5,5,
5,5,5,5,5,5,5,105,0,0,0,0,0,0,0,0,92,105,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,
0,0,0,0,};

static const unsigned char small_icon_8_data[] = {
92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,
92,92,92,92,92,92,92,92,92,92,92,105,92,92,92,92,92,92,92,92,
92,92,92,92,92,92,92,105,92,92,92,92,92,92,92,92,92,92,92,92,
92,92,92,105,92,92,92,92,92,92,92,92,92,92,92,92,92,5,92,105,
92,92,92,92,92,92,92,92,92,92,92,92,5,5,92,105,92,92,92,92,
92,92,92,92,92,92,92,5,5,5,92,105,92,92,92,92,92,92,92,92,
92,92,5,5,5,5,92,105,92,92,92,92,92,92,92,92,92,5,92,5,
5,5,92,105,92,105,105,105,92,92,92,92,5,92,92,5,105,105,92,105,
92,105,0,0,92,92,92,5,92,92,92,5,0,0,105,105,0,0,92,92,
92,92,5,92,92,92,92,5,5,5,0,0,0,0,92,92,92,5,5,5,
5,5,5,5,5,5,0,0,0,0,92,92,5,92,92,92,92,92,92,5,
5,5,0,0,0,0,92,5,5,92,92,92,92,92,5,5,5,5,0,0,
0,0,5,5,5,5,105,105,105,5,5,5,5,5,0,0,};

static const unsigned char icon_4_data[] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,69,69,69,69,
69,69,69,69,69,69,69,69,69,69,69,69,84,84,84,84,84,84,84,84,
84,84,84,84,84,84,84,85,69,69,69,69,69,69,69,69,69,69,69,69,
69,69,69,69,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,85,
69,69,69,69,69,69,69,69,69,69,69,69,69,69,69,69,84,84,84,84,
84,84,84,84,84,84,84,84,84,84,84,85,69,69,69,69,69,69,69,69,
69,69,69,69,69,69,69,69,84,84,84,84,84,84,84,84,84,84,84,84,
84,84,84,85,69,69,69,69,69,69,69,69,69,69,69,69,69,21,69,69,
84,84,84,84,84,84,84,84,84,84,84,84,81,20,84,85,69,69,69,69,
69,69,69,69,69,69,69,69,17,21,69,69,84,84,84,84,84,84,84,84,
84,84,84,81,17,20,84,85,69,69,69,69,69,69,69,69,69,69,69,17,
17,21,69,69,84,84,84,84,84,84,84,84,84,84,81,17,17,20,84,85,
69,69,69,69,69,69,69,69,69,69,17,17,17,21,69,69,84,84,84,84,
84,84,84,84,84,81,17,17,17,20,84,85,69,69,69,69,69,69,69,69,
69,17,65,17,17,21,69,69,84,85,85,85,84,84,84,84,81,20,81,21,
85,85,84,85,69,69,0,0,69,69,69,69,17,69,65,21,0,0,69,69,
85,85,0,0,84,84,84,81,20,84,81,21,0,0,85,85,0,0,0,0,
69,69,69,17,69,69,65,21,0,0,0,0,0,0,0,0,84,84,81,20,
84,84,81,21,0,0,0,0,0,0,69,69,69,69,17,69,69,69,65,17,
17,30,0,0,0,0,84,84,84,81,17,17,17,17,17,17,17,21,0,0,
0,0,69,69,69,17,17,17,17,17,17,17,17,21,0,0,0,0,84,84,
81,20,84,84,84,84,81,17,17,21,0,0,0,0,69,69,17,69,69,69,
69,69,65,17,17,21,0,0,0,0,84,81,17,84,84,84,84,84,17,17,
17,21,0,0,0,0,69,17,17,21,69,69,69,65,17,17,17,21,0,0,
0,0,81,17,17,17,84,84,84,17,17,17,17,21,0,0,0,0,229,85,
85,85,85,85,85,85,85,85,85,85,0,0,};

static const unsigned char small_icon_4_data[] = {
84,84,84,84,84,84,84,85,69,69,69,69,69,69,69,69,84,84,84,84,
84,84,84,85,69,69,69,69,69,69,69,69,84,84,84,84,84,84,81,85,
69,69,69,69,69,69,17,69,84,84,84,84,84,81,17,85,69,69,69,69,
69,17,17,69,84,84,84,84,81,81,17,85,69,85,69,69,21,65,85,69,
85,0,84,81,84,81,0,85,0,69,69,21,69,65,17,0,0,84,81,17,
17,17,17,0,0,69,21,69,69,65,17,0,0,81,20,84,84,17,17,0,
0,17,17,85,81,17,17,0,};


/* fseek with fatal_error */

static void
reposition(FILE *fi, int32_t absolute)
{
  if (fseek(fi, absolute, 0) == -1)
    fatal_error("can't seek to position %d", absolute);
}

/* Some functions to write one, two, three, and four byte integers in 68000
   byte order (most significant byte first). */

static void
write_one(int c, FILE *f)
{
  putc(c, f);
}

static void
write_two(int c, FILE *f)
{
  putc((c >> 8) & 255, f);
  putc(c & 255, f);
}

static void
write_three(int32_t c, FILE *f)
{
  putc((c >> 16) & 255, f);
  putc((c >> 8) & 255, f);
  putc(c & 255, f);
}

static void
write_four(int32_t c, FILE *f)
{
  putc((c >> 24) & 255, f);
  putc((c >> 16) & 255, f);
  putc((c >> 8) & 255, f);
  putc(c & 255, f);
}

/* Some functions to store one, two, three, and four byte integers in 68000
   byte order (most significant byte first). */

static void
store_one(int c, char *s)
{
  s[0] = (char)(c & 255);
}

static void
store_two(int c, char *s)
{
  s[0] = (char)((c >> 8) & 255);
  s[1] = (char)(c & 255);
}

static void
store_four(int32_t c, char *s)
{
  s[0] = (char)((c >> 24) & 255);
  s[1] = (char)((c >> 16) & 255);
  s[2] = (char)((c >> 8) & 255);
  s[3] = (char)(c & 255);
}

static void
output_new_rsrc(const char *rtype, int rid, int attrs,
		const char *data, uint32_t len)
{
  Rsrc *r;
  if (nrsrc >= rsrc_cap) {
    rsrc_cap = (rsrc_cap ? rsrc_cap * 2 : 256);
    r = (Rsrc *)malloc(sizeof(Rsrc) * rsrc_cap);
    if (!r)
      fatal_error("out of memory");
    memcpy(r, rsrc, sizeof(Rsrc) * nrsrc);
    free(rsrc);
    rsrc = r;
  }
  r = &rsrc[nrsrc];
  nrsrc++;

  /* prepare resource record */
  {
    const unsigned char *b = (const unsigned char *)rtype;
    r->type = (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3];
  }
  r->id = rid;
  r->attrs = attrs;
  if (nrsrc == 1)
    r->data_offset = 0;
  else
    r->data_offset = rsrc[nrsrc-2].data_offset + rsrc[nrsrc-2].data_len + 4;
  r->data_len = len;
  r->next_in_type = r->next_type = -2;

  /* resource consists of length, then data */
  write_four(r->data_len, rfork_f);
  fwrite(data, 1, len, rfork_f);
}

static void
init_current_post(void)
{
  rbufpos = 2;
  cur_post_id = 501;
  blocktyp = POST_ASCII;
}

static void
output_current_post(void)
{
  if (blocktyp != POST_END && rbufpos <= 2)
    return;
  rbuf[0] = blocktyp;
  rbuf[1] = 0;
  output_new_rsrc("POST", cur_post_id, 0, (char *)rbuf, rbufpos);
  rbufpos = 2;
  cur_post_id++;
}

/* font_reader functions */

static void
t1mac_output_data(byte *s, int len)
{
  while (len > 0) {
    int n;
    /* In some Mac fonts, the ASCII sections terminate with a line-end */
    if (rbufpos >= MAX_RSRC_LEN
	|| (blocktyp == POST_ASCII && len + rbufpos > MAX_RSRC_LEN && rbufpos))
      output_current_post();
    n = (len + rbufpos <= MAX_RSRC_LEN ? len : MAX_RSRC_LEN - rbufpos);
    memcpy(rbuf + rbufpos, s, n);
    rbufpos += n;
    s += n;
    len -= n;
  }
}

static void
t1mac_output_ascii(char *s, int len)
{
  if (blocktyp == POST_BINARY) {
    output_current_post();
    blocktyp = POST_ASCII;
  }
  /* Mac line endings */
  if (len > 0 && s[len-1] == '\n')
    s[len-1] = '\r';
  t1mac_output_data((byte *)s, len);
  if (strncmp(s, "/FontName", 9) == 0) {
    for (s += 9; isspace((unsigned char) *s); s++)
        /* skip */;
    if (*s == '/') {
      const char *t = ++s;
      while (*t && !isspace((unsigned char) *t)) t++;
      free(font_name);
      font_name = (char *)malloc(t - s + 1);
      memcpy(font_name, s, t - s);
      font_name[t - s] = 0;
    }
  }
}

static void
t1mac_output_binary(unsigned char *s, int len)
{
  if (blocktyp == POST_ASCII) {
    output_current_post();
    blocktyp = POST_BINARY;
  }
  t1mac_output_data(s, len);
}

static void
t1mac_output_end(void)
{
  output_current_post();
  blocktyp = POST_END;
  output_current_post();
}


/* finish off the resource fork */

static uint32_t
complete_rfork(void)
{
  uint32_t reflist_offset, total_data_len;
  uint32_t typelist_len;
  int i, j, ntypes;

  /* analyze resources */
  {
    int last_type = -1;
    ntypes = 0;
    for (i = 0; i < nrsrc; i++)
      if (rsrc[i].next_in_type == -2) {
	int last = -1;
	if (last_type >= 0)
	  rsrc[last_type].next_type = i;
	for (j = i; j < nrsrc; j++)
	  if (rsrc[j].type == rsrc[i].type) {
	    if (last >= 0)
	      rsrc[last].next_in_type = j;
	    last = j;
	  }
	rsrc[last].next_in_type = -1;
	last_type = i;
	ntypes++;
      }
  }

  /* have just finished writing data */
  /* now write resource map */
  for (i = 0; i < RFORK_MAP_RESERVEDLEN; i++)
    putc(0, rfork_f);		/* reserved */
  write_two(0, rfork_f);	/* resource fork attributes */
  typelist_len = ntypes * RFORK_RTYPE_LEN + 2;
  write_two(RFORK_MAP_HEADERLEN, rfork_f); /* offset from start of map to typelist */
  write_two(RFORK_MAP_HEADERLEN + typelist_len + nrsrc * RFORK_RSRC_LEN, rfork_f); /* offset from start of map to namelist */

  /* output type map */
  write_two(ntypes - 1, rfork_f);/* number of types - 1 */
  reflist_offset = typelist_len;
  for (i = 0; i >= 0; i = rsrc[i].next_type) {
    int n_in_type = 0;
    for (j = i; j >= 0; j = rsrc[j].next_in_type)
      n_in_type++;
    write_four(rsrc[i].type, rfork_f);	/* resource type */
    write_two(n_in_type - 1, rfork_f);	/* number in type - 1 */
    write_two(reflist_offset, rfork_f);	/* offset to reflist from start of typelist */
    reflist_offset += n_in_type * RFORK_RSRC_LEN;
  }

  /* output reference list */
  for (i = 0; i >= 0; i = rsrc[i].next_type)
    for (j = i; j >= 0; j = rsrc[j].next_in_type) {
      write_two(rsrc[j].id, rfork_f);	/* ID */
      write_two(-1, rfork_f);		/* offset to name */
      write_one(rsrc[j].attrs, rfork_f); /* attributes */
      write_three(rsrc[j].data_offset, rfork_f); /* offset to data from start of data */
      write_four(0, rfork_f);		/* reserved */
    }

  /* finally, patch up resource fork header */
  {
    total_data_len = rsrc[nrsrc-1].data_offset + rsrc[nrsrc-1].data_len + 4;
    reposition(rfork_f, 0);
    write_four(RFORK_HEADERLEN, rfork_f); /* offset from rfork to data */
    write_four(RFORK_HEADERLEN + total_data_len, rfork_f); /* offset from rfork to map */
    write_four(total_data_len, rfork_f); /* length of data */
    write_four(RFORK_MAP_HEADERLEN + reflist_offset, rfork_f); /* length of map */
  }

  return RFORK_HEADERLEN + total_data_len + RFORK_MAP_HEADERLEN + reflist_offset;
}


/* write a MacBinary II file */

static void
output_raw(FILE *rf, int32_t len, FILE *f)
{
  char buf[2048];
  reposition(rf, 0);
  while (len > 0) {
    int n = (len < 2048 ? len : 2048);
    fread(buf, 1, n, rf);
    fwrite(buf, 1, n, f);
    len -= n;
  }
}

static void
output_macbinary(FILE *rf, int32_t rf_len, const char *filename, FILE *f)
{
  int i, len = strlen(filename);
  char buf[128];
  if (len < 1 || len > 63)
    fatal_error("filename length must be between 1 and 63");
  store_one(0, buf+0);		/* old version number */
  store_one(len, buf+1);	/* filename length */
  memset(buf+2, 0, 63);		/* filename padding */
  memcpy(buf+2, filename, len);	/* filename */
  store_four(T1_FILETYPE, buf+65); /* file type */
  store_four(T1_FILECREATOR, buf+69); /* file creator */
  store_one(T1_FINDERFLAGS, buf+73); /* finder flags */
  store_one(0, buf+74);		/* zero byte */
  store_two(0, buf+75);		/* vertical position in window */
  store_two(0, buf+77);		/* horizontal position in window */
  store_two(0, buf+79);		/* window or folder ID */
  store_one(0, buf+81);		/* protected flag */
  store_one(0, buf+82);		/* zero byte */
  store_four(0, buf+83);	/* data fork length */
  store_four(rf_len, buf+87);	/* resource fork length */
  {
    time_t t = time(0) + MAC_TIME_DELTA;
    store_four(t, buf+91);	/* creation date */
    store_four(t, buf+95);	/* modification date */
  }
  store_two(0, buf+99);		/* GetInfo comment length */
  store_one(0, buf+101);	/* finder flags part 2 */
  memset(buf+102, 0, 116 - 102); /* padding */
  store_four(0, buf+116);	/* total length when unpacked */
  store_two(0, buf+120);	/* length of secondary header */
  store_one(129, buf+122);	/* version number */
  store_one(129, buf+123);	/* minimum acceptable version number */
  store_two(crcbuf(0, 124, buf), buf+124); /* CRC */
  store_two(0, buf+126);	/* padding to 128 bytes */

  /* write out the header */
  fwrite(buf, 1, 128, f);

  /* now write resource fork */
  output_raw(rf, rf_len, f);
  for (i = rf_len % 128; i && i < 128; i++)
    putc(0, f);
}


/* write an AppleSingle file */

#define APPLESINGLE_MAGIC 0x00051600
#define APPLEDOUBLE_MAGIC 0x00051607
#define APPLESINGLE_VERSION 0x00020000
#define APPLESINGLE_TIME_DELTA 883612800
#define APPLESINGLE_HEADERLEN 26
#define APPLESINGLE_ENTRYLEN 12
#define APPLESINGLE_DFORK_ENTRY 1
#define APPLESINGLE_RFORK_ENTRY 2
#define APPLESINGLE_DATES_ENTRY 8
#define APPLESINGLE_DATES_LEN 16
#define APPLESINGLE_FINDERINFO_ENTRY 9
#define APPLESINGLE_FINDERINFO_LEN 32
#define APPLESINGLE_REALNAME_ENTRY 3

static void
output_applesingle(FILE *rf, int32_t rf_len, const char *filename, FILE *f,
		   int appledouble)
{
  uint32_t offset;
  int i, nentries, len = strlen(filename);
  if (appledouble)		/* magic number */
    write_four(APPLEDOUBLE_MAGIC, f);
  else
    write_four(APPLESINGLE_MAGIC, f);
  write_four(APPLESINGLE_VERSION, f); /* version number */
  for (i = 0; i < 4; i++)
    write_four(0, f);		/* filler */
  nentries = (appledouble ? 4 : 5);
  write_two(nentries, f); /* number of entries */

  /* real name entry */
  offset = APPLESINGLE_HEADERLEN + nentries * APPLESINGLE_ENTRYLEN;
  write_four(APPLESINGLE_REALNAME_ENTRY, f);
  write_four(offset, f);
  write_four(len, f);
  offset += len;

  /* time entry */
  write_four(APPLESINGLE_DATES_ENTRY, f);
  write_four(offset, f);
  write_four(APPLESINGLE_DATES_LEN, f);
  offset += APPLESINGLE_DATES_LEN;

  /* finder info entry */
  write_four(APPLESINGLE_FINDERINFO_ENTRY, f);
  write_four(offset, f);
  write_four(APPLESINGLE_FINDERINFO_LEN, f);
  offset += APPLESINGLE_FINDERINFO_LEN;

  /* resource fork entry */
  write_four(APPLESINGLE_RFORK_ENTRY, f);
  write_four(offset, f);
  write_four(rf_len, f);
  offset += rf_len;

  /* data fork entry */
  if (!appledouble) {
    write_four(APPLESINGLE_DFORK_ENTRY, f);
    write_four(offset, f);
    write_four(0, f);
  }

  /* real name data */
  fwrite(filename, 1, len, f);

  /* time data */
  i = time(0) - APPLESINGLE_TIME_DELTA;
  write_four(i, f);		/* creation date */
  write_four(i, f);		/* modification date */
  write_four(0x80000000, f);	/* backup date */
  write_four(0, f);		/* access date */

  /* finder info data */
  write_four(T1_FILETYPE, f);	/* file type */
  write_four(T1_FILECREATOR, f); /* file creator */
  write_one(T1_FINDERFLAGS, f); /* finder flags */
  write_one(0, f);		/* extended finder flags */
  write_two(0, f);		/* vertical position in window */
  write_two(0, f);		/* horizontal position in window */
  write_two(0, f);		/* window or folder ID */
  write_four(0, f);		/* icon ID and reserved */
  write_four(0, f);		/* reserved */
  write_one(0, f);		/* script flag */
  write_one(0, f);		/* reserved */
  write_two(0, f);		/* comment ID */
  write_four(0, f);		/* put away */

  /* resource fork data */
  output_raw(rf, rf_len, f);
}


/* write a BinHex file */

static void
binhex_buffer(const byte *s, int len, FILE *f)
{
  static int col = 1;
  static int bits = 0;
  static int bitspos = 2;
  static const char *table = "!\"#$%&'()*+,-012345689@ABCDEFGHIJKLMNPQRSTUVXYZ[`abcdefhijklmpqr";
  byte buf[5];
  int c, i, left;

  if (!s && bitspos > 2) {	/* output the remaining bits */
    s = (const byte *)"\0";
    len = 1;
  }

  for (left = len; left > 0; left--, s++) {
    int pos;
    if (s[0] == 0x90) {
      buf[0] = 0x90;
      buf[1] = 0x00;
      pos = 2;
    } else {
      buf[0] = s[0];
      pos = 1;
    }

    /* find a run */
    if (left > 2 && s[0] == s[1] && s[0] == s[2]) {
      for (i = 3; i < left && i < 255; i++)
	if (s[i] != s[0])
	  break;
      buf[pos] = 0x90;
      buf[pos+1] = i;
      pos += 2;
      s += i - 1;
      left -= i - 1;
    }

    /* store those characters */
    for (i = 0; i < pos; i++) {
      bits |= buf[i];
      while (bitspos >= 0) {
	c = (bits >> bitspos) & 0x3F;
	putc(table[c], f);
	if (++col == 63) {
	  putc('\n', f);
	  col = 0;
	}
	bitspos -= 6;
      }
      bits <<= 8;
      bitspos += 8;
    }
  }
}

static void
output_binhex(FILE *rf, int32_t rf_len, const char *filename, FILE *f)
{
  int crc, len = strlen(filename);
  char buf[2048];

  if (len < 1 || len > 63)
    fatal_error("filename length must be between 1 and 63");
  store_one(len, buf+0);	/* filename length */
  memcpy(buf+1, filename, len);	/* filename */
  store_one(0, buf+1+len);	/* version */
  store_four(T1_FILETYPE, buf+2+len); /* file type */
  store_four(T1_FILECREATOR, buf+6+len); /* file creator */
  store_one(T1_FINDERFLAGS, buf+10+len); /* finder flags */
  store_one(0, buf+11+len);	/* extended finder flags */
  store_four(0, buf+12+len);	/* length of data fork */
  store_four(rf_len, buf+16+len); /* length of resource fork */
  store_two(crcbuf(0, 20+len, buf), buf+20+len); /* CRC */
  store_two(0, buf+22+len);	/* data fork CRC */

  /* output BinHex comment */
  fputs("(This file must be converted with BinHex 4.0)\n:", f);

  /* BinHex the header */
  binhex_buffer((const byte *)buf, 24+len, f);

  /* resource fork data */
  reposition(rf, 0);
  crc = 0;
  while (rf_len > 0) {
    int n = (rf_len < 2048 ? rf_len : 2048);
    fread(buf, 1, n, rf);
    crc = crcbuf(crc, n, buf);	/* update CRC */
    binhex_buffer((const byte *)buf, n, f);
    rf_len -= n;
  }
  store_two(crc, buf);		/* resource fork CRC */
  binhex_buffer((const byte *)buf, 2, f);
  binhex_buffer(0, 0, f);	/* get rid of any remaining bits */
  fputs(":\n", f);		/* trailer */
}


/*****
 * command line
 **/

#define OUTPUT_OPT	301
#define VERSION_OPT	302
#define HELP_OPT	303
#define MACBINARY_OPT	304
#define RAW_OPT		305
#define APPLESINGLE_OPT	306
#define APPLEDOUBLE_OPT	307
#define BINHEX_OPT	308
#define FILENAME_OPT	309

static Clp_Option options[] = {
  { "appledouble", 0, APPLEDOUBLE_OPT, 0, 0 },
  { "applesingle", 0, APPLESINGLE_OPT, 0, 0 },
  { "binhex", 0, BINHEX_OPT, 0, 0 },
  { "help", 0, HELP_OPT, 0, 0 },
  { "macbinary", 0, MACBINARY_OPT, 0, 0 },
  { "filename", 'n', FILENAME_OPT, Clp_ValString, 0 },
  { "output", 'o', OUTPUT_OPT, Clp_ValString, 0 },
  { "raw", 'r', RAW_OPT, 0, 0 },
  { "version", 0, VERSION_OPT, 0, 0 },
};
static const char *program_name;


void
fatal_error(const char *message, ...)
{
  va_list val;
  va_start(val, message);
  fprintf(stderr, "%s: ", program_name);
  vfprintf(stderr, message, val);
  putc('\n', stderr);
  va_end(val);
  exit(1);
}

void
error(const char *message, ...)
{
  va_list val;
  va_start(val, message);
  fprintf(stderr, "%s: ", program_name);
  vfprintf(stderr, message, val);
  putc('\n', stderr);
  va_end(val);
}


static void
short_usage(void)
{
  fprintf(stderr, "Usage: %s [OPTION]... [INPUT [OUTPUT]]\n\
Try `%s --help' for more information.\n",
	  program_name, program_name);
}


static void
usage(void)
{
  printf("\
`T1mac' translates a PostScript Type 1 font from PFA or PFB format into\n\
Macintosh Type 1 format. The result can be written in MacBinary II format (the\n\
default), AppleSingle format, AppleDouble format, or BinHex format, or as a\n\
raw resource fork. It is sent to the standard output unless an OUTPUT file is\n\
given.\n\
\n\
Usage: %s [OPTION]... [INPUT [OUTPUT]]\n\
\n\
Options:\n\
  -r, --raw                   Output is a raw Macintosh resource fork.\n\
      --macbinary             Output is in MacBinary format (default).\n\
      --applesingle           Output is in AppleSingle format.\n\
      --appledouble           Output is in AppleDouble format.\n\
      --binhex                Output is in BinHex format.\n\
  -n, --filename NAME         Macintosh font filename will be NAME.\n\
  -o, --output FILE           Write output to FILE.\n\
  -h, --help                  Print this message and exit.\n\
      --version               Print version number and warranty and exit.\n\
\n\
Report bugs to <ekohler@gmail.com>.\n", program_name);
}

#ifdef __cplusplus
}
#endif


int
main(int argc, char *argv[])
{
  int i, c;
  FILE *ifp = 0, *ofp = 0;
  const char *ifp_filename = "<stdin>";
  const char *ofp_filename = "<stdout>";
  const char *set_font_name = 0;
  struct font_reader fr;
  uint32_t rfork_len;
  int raw = 0, macbinary = 1, applesingle = 0, appledouble = 0, binhex = 0;

  Clp_Parser *clp =
    Clp_NewParser(argc, (const char * const *)argv, sizeof(options) / sizeof(options[0]), options);
  program_name = Clp_ProgramName(clp);

  /* interpret command line arguments using CLP */
  while (1) {
    int opt = Clp_Next(clp);
    switch (opt) {

     case RAW_OPT:
      raw = 1;
      macbinary = applesingle = appledouble = binhex = 0;
      break;

     case MACBINARY_OPT:
      macbinary = 1;
      raw = applesingle = appledouble = binhex = 0;
      break;

     case APPLESINGLE_OPT:
      applesingle = 1;
      raw = macbinary = appledouble = binhex = 0;
      break;

     case APPLEDOUBLE_OPT:
      appledouble = 1;
      raw = macbinary = applesingle = binhex = 0;
      break;

     case BINHEX_OPT:
      binhex = 1;
      raw = macbinary = applesingle = appledouble = 0;
      break;

     output_file:
     case OUTPUT_OPT:
      if (ofp)
	fatal_error("output file already specified");
      if (strcmp(clp->vstr, "-") == 0)
	ofp = stdout;
      else {
	ofp_filename = clp->vstr;
	ofp = fopen(ofp_filename, "wb");
	if (!ofp) fatal_error("%s: %s", ofp_filename, strerror(errno));
      }
      break;

     case FILENAME_OPT:
      if (set_font_name)
	fatal_error("Macintosh font filename already specified");
      set_font_name = clp->vstr;
      break;

     case HELP_OPT:
      usage();
      exit(0);
      break;

     case VERSION_OPT:
      printf("t1mac (LCDF t1utils) %s\n", VERSION);
      printf("Copyright (C) 2000-2010 Eddie Kohler et al.\n\
This is free software; see the source for copying conditions.\n\
There is NO warranty, not even for merchantability or fitness for a\n\
particular purpose.\n");
      exit(0);
      break;

     case Clp_NotOption:
      if (ifp && ofp)
	fatal_error("too many arguments");
      else if (ifp)
	goto output_file;
      if (strcmp(clp->vstr, "-") == 0)
	ifp = stdin;
      else {
	ifp_filename = clp->vstr;
	ifp = fopen(clp->vstr, "r");
	if (!ifp) fatal_error("%s: %s", clp->vstr, strerror(errno));
      }
      break;

     case Clp_Done:
      goto done;

     case Clp_BadOption:
      short_usage();
      exit(1);
      break;

    }
  }

 done:
  if (!ifp) ifp = stdin;
  if (!ofp) ofp = stdout;

#if defined(_MSDOS) || defined(_WIN32)
  /* As we are processing a PFB (binary) output */
  /* file, we must set its file mode to binary. */
  _setmode(_fileno(ofp), _O_BINARY);
#endif

  /* prepare font reader */
  fr.output_ascii = t1mac_output_ascii;
  fr.output_binary = t1mac_output_binary;
  fr.output_end = t1mac_output_end;

  /* prepare resource fork file */
  rfork_f = tmpfile();
  if (!rfork_f)
    fatal_error("cannot open temorary file: %s", strerror(errno));
  for (i = 0; i < RFORK_HEADERLEN; i++)
    putc(0, rfork_f);
  init_current_post();

  /* peek at first byte to see if it is the PFB marker 0x80 */
  c = getc(ifp);
  ungetc(c, ifp);

  /* do the file */
  if (c == PFB_MARKER)
    process_pfb(ifp, ifp_filename, &fr);
  else if (c == '%')
    process_pfa(ifp, ifp_filename, &fr);
  else
    fatal_error("%s does not start with font marker (`%%' or 0x80)", ifp_filename);
  if (ifp != stdin)
    fclose(ifp);

  /* check if anything was read */
  if (nrsrc == 0)
    error("no POST resources written -- are you sure this was a font?");

  /* output large B/W icon */
  output_new_rsrc("ICN#", 256, 32, (const char *)icon_bw_data, 256);
  /* output FREF */
  output_new_rsrc("FREF", 256, 32, "LWFN\0\0\0", 7);
  /* output BNDL */
  output_new_rsrc("BNDL", 256, 32, "T1UT\0\0\0\1FREF\0\0\0\0\1\0ICN#\0\0\0\0\1\0", 28);
  /* output other icons */
  output_new_rsrc("icl8", 256, 32, (const char *)icon_8_data, 1024);
  output_new_rsrc("icl4", 256, 32, (const char *)icon_4_data, 512);
  output_new_rsrc("ics#", 256, 32, (const char *)small_icon_bw_data, 64);
  output_new_rsrc("ics8", 256, 32, (const char *)small_icon_8_data, 256);
  output_new_rsrc("ics4", 256, 32, (const char *)small_icon_4_data, 128);
  /* output T1UT (signature) */
  output_new_rsrc("T1UT", 0, 0, "DConverted by t1mac (t1utils) \251Eddie Kohler http://www.lcdf.org/type/", 69);

  /* finish off resource file */
  rfork_len = complete_rfork();

  /* prepare font name */
  if (!set_font_name && font_name) {
    int part = 0, len = 0;
    char *x, *s;
    for (x = s = font_name; *s; s++)
      if (isupper((unsigned char) *s) || isdigit((unsigned char) *s)) {
	*x++ = *s;
	part++;
	len = 1;
      } else if (islower((unsigned char) *s)) {
	if (len < (part <= 1 ? 5 : 3))
	  *x++ = *s;
	len++;
      }
    *x++ = 0;
    set_font_name = font_name;
  } else if (!set_font_name)
    set_font_name = "Unknown Font";

  /* now, output the file */
  if (macbinary)
    output_macbinary(rfork_f, rfork_len, set_font_name, ofp);
  else if (raw)
    output_raw(rfork_f, rfork_len, ofp);
  else if (applesingle || appledouble)
    output_applesingle(rfork_f, rfork_len, set_font_name, ofp, appledouble);
  else if (binhex)
    output_binhex(rfork_f, rfork_len, set_font_name, ofp);
  else
    fatal_error("strange output format");
  fclose(rfork_f);

  if (ofp != stdout)
    fclose(ofp);
  return 0;
}
