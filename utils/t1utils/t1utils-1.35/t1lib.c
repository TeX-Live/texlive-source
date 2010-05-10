/* t1lib
 *
 * This file contains functions for reading PFA and PFB files.
 *
 * (C) 1999-2008 Eddie Kohler <ekohler@gmail.com>, after code by
 * I. Lee Hetherington <ilh@lcs.mit.edu>. All rights reserved.
 *
 * Permission is hereby granted to use, modify, and distribute this program
 * for any purpose provided this copyright notice remains intact.
 *
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "t1lib.h"
#ifdef __cplusplus
extern "C" {
#endif

#define PFA_ASCII	1
#define PFA_EEXEC_TEST	2
#define PFA_HEX		3
#define PFA_BINARY	4

/* This function returns the value (0-15) of a single hex digit.  It returns
   0 for an invalid hex digit. */

static int
hexval(char c)
{
  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;
  else if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  else if (c >= '0' && c <= '9')
    return c - '0';
  else
    return 0;
}

/* This function translates a string of hexadecimal digits into binary data.
   We allow an odd number of digits. Returns length of binary data. */

static int
translate_hex_string(char *s, char *saved_orphan)
{
  int c1 = *saved_orphan;
  char *start = s;
  char *t = s;
  for (; *s; s++) {
    if (isspace(*s))
      continue;
    if (c1) {
      *t++ = (hexval(c1) << 4) + hexval(*s);
      c1 = 0;
    } else
      c1 = *s;
  }
  *saved_orphan = c1;
  return t - start;
}

/* This function returns 1 if the string contains all '0's. */

static int
all_zeroes(char *s)
{
  if (*s == '\0' || *s == '\n')
    return 0;
  while (*s == '0')
    s++;
  return *s == '\0' || *s == '\n';
}

/* This function handles the entire file. */

#define LINESIZE 1024

void
process_pfa(FILE *ifp, const char *ifp_filename, struct font_reader *fr)
{
    /* Loop until no more input. We need to look for `currentfile eexec' to
       start eexec section (hex to binary conversion) and line of all zeros to
       switch back to ASCII. */

    /* Don't use fgets() in case line-endings are indicated by bare \r's, as
       occurs in Macintosh fonts. */

    /* 2.Aug.1999 - At the behest of Tom Kacvinsky <tjk@ams.org>, support
       binary PFA fonts. */

    char buffer[LINESIZE];
    int c = 0;
    int blocktyp = PFA_ASCII;
    char saved_orphan = 0;
    (void)ifp_filename;

    while (c != EOF) {
	char *line = buffer, *last = buffer;
	int crlf = 0;
	c = getc(ifp);
	while (c != EOF && c != '\r' && c != '\n' && last < buffer + LINESIZE - 1) {
	    *last++ = c;
	    c = getc(ifp);
	}

	/* handle the end of the line */
	if (last == buffer + LINESIZE - 1)
	    /* buffer overrun: don't append newline even if we have it */
	    ungetc(c, ifp);
	else if (c == '\r' && blocktyp != PFA_BINARY) {
	    /* change CR or CR/LF into LF, unless reading binary data! (This
	       condition was wrong before, caused Thanh problems -
	       6.Mar.2001) */
	    c = getc(ifp);
	    if (c != '\n')
		ungetc(c, ifp), crlf = 1;
	    else
		crlf = 2;
	    *last++ = '\n';
	} else if (c != EOF)
	    *last++ = c;

	*last = 0;

	/* now that we have the line, handle it */
	if (blocktyp == PFA_ASCII) {
	    if (strncmp(line, "currentfile eexec", 17) == 0 && isspace(line[17])) {
		char saved_p;
		/* assert(line == buffer); */
		for (line += 18; isspace(*line); line++)
		    /* nada */;
		saved_p = *line;
		*line = 0;
		fr->output_ascii(buffer, line - buffer);
		*line = saved_p;
		blocktyp = PFA_EEXEC_TEST;
		if (!*line)
		    continue;
	    } else {
		fr->output_ascii(line, last - line);
		continue;
	    }
	}

	/* check immediately after "currentfile eexec" for ASCII or binary */
	if (blocktyp == PFA_EEXEC_TEST) {
	    /* 8.Feb.2004: fix bug if first character in a binary eexec block
	       is 0, reported by Werner Lemberg */
	    for (; line < last && isspace(*line); line++)
		/* nada */;
	    if (line == last)
		continue;
	    else if (last >= line + 4 && isxdigit(line[0]) && isxdigit(line[1])
		     && isxdigit(line[2]) && isxdigit(line[3]))
		blocktyp = PFA_HEX;
	    else
		blocktyp = PFA_BINARY;
	    memmove(buffer, line, last - line + 1);
	    last = buffer + (last - line);
	    line = buffer;
	    /* patch up crlf fix */
	    if (blocktyp == PFA_BINARY && crlf) {
		last[-1] = '\r';
		if (crlf == 2)
		    *last++ = '\n';
	    }
	}

	/* blocktyp == PFA_HEX || blocktyp == PFA_BINARY */
	if (all_zeroes(line)) {	/* XXX not safe */
	    fr->output_ascii(line, last - line);
	    blocktyp = PFA_ASCII;
	} else if (blocktyp == PFA_HEX) {
	    int len = translate_hex_string(line, &saved_orphan);
	    if (len)
		fr->output_binary((unsigned char *)line, len);
	} else
	    fr->output_binary((unsigned char *)line, last - line);
    }

    fr->output_end();
}

/* Process a PFB file. */

/* XXX Doesn't handle "currentfile eexec" as intelligently as process_pfa
   does. */

static int
handle_pfb_ascii(struct font_reader *fr, char *line, int len)
{
  /* Divide PFB_ASCII blocks into lines */
  int start = 0;

  while (1) {
    int pos = start;

    while (pos < len && line[pos] != '\n' && line[pos] != '\r')
      pos++;

    if (pos >= len) {
      if (pos == start)
	return 0;
      else if (start == 0 && pos == LINESIZE - 1) {
	line[pos] = 0;
	fr->output_ascii(line, pos);
	return 0;
      } else {
	memmove(line, line + start, pos - start);
	return pos - start;
      }

    } else if (pos < len - 1 && line[pos] == '\r' && line[pos+1] == '\n') {
      line[pos] = '\n';
      line[pos+1] = 0;
      fr->output_ascii(line + start, pos + 1 - start);
      start = pos + 2;

    } else {
      char save = line[pos+1];
      line[pos] = '\n';
      line[pos+1] = 0;
      fr->output_ascii(line + start, pos + 1 - start);
      line[pos+1] = save;
      start = pos + 1;
    }
  }
}


void
process_pfb(FILE *ifp, const char *ifp_filename, struct font_reader *fr)
{
  int blocktyp = 0;
  int block_len = 0;
  int c = 0;
  int filepos = 0;
  int linepos = 0;
  char line[LINESIZE];

  while (1) {
    while (block_len == 0) {
      c = getc(ifp);
      blocktyp = getc(ifp);
      if (c != PFB_MARKER
	  || (blocktyp != PFB_ASCII && blocktyp != PFB_BINARY
	      && blocktyp != PFB_DONE)) {
	if (c == EOF || blocktyp == EOF)
	  error("%s corrupted: no end-of-file marker", ifp_filename);
	else
	  error("%s corrupted: bad block marker at position %d",
		ifp_filename, filepos);
	blocktyp = PFB_DONE;
      }
      if (blocktyp == PFB_DONE)
	goto done;

      block_len = getc(ifp) & 0xFF;
      block_len |= (getc(ifp) & 0xFF) << 8;
      block_len |= (getc(ifp) & 0xFF) << 16;
      block_len |= (getc(ifp) & 0xFF) << 24;
      if (feof(ifp)) {
	error("%s corrupted: bad block length at position %d",
	      ifp_filename, filepos);
	blocktyp = PFB_DONE;
	goto done;
      }
      filepos += 6;
    }

    /* read the block in its entirety, in LINESIZE chunks */
    while (block_len > 0) {
      int rest = LINESIZE - 1 - linepos; /* leave space for '\0' */
      int n = (block_len > rest ? rest : block_len);
      int actual = fread(line + linepos, 1, n, ifp);
      if (actual != n) {
	error("%s corrupted: block short by %d bytes at position %d",
	      ifp_filename, block_len - actual, filepos);
	block_len = actual;
      }

      if (blocktyp == PFB_BINARY)
	fr->output_binary((unsigned char *)line, actual);
      else
	linepos = handle_pfb_ascii(fr, line, linepos + actual);

      block_len -= actual;
      filepos += actual;
    }

    /* handle any leftover line */
    if (linepos > 0) {
      line[linepos] = 0;
      fr->output_ascii(line, linepos);
      linepos = 0;
    }
  }

 done:
  c = getc(ifp);
  if (c != EOF)
    error("%s corrupted: data after PFB end marker at position %d",
	  ifp_filename, filepos - 2);
  fr->output_end();
}


#define DEFAULT_BLOCKLEN (1L<<12)

void
init_pfb_writer(struct pfb_writer *w, int blocklen, FILE *f)
{
    w->len = DEFAULT_BLOCKLEN;
    w->buf = (unsigned char *)malloc(w->len);
    if (!w->buf)
	fatal_error("out of memory");
    w->max_len = (blocklen <= 0 ? 0xFFFFFFFFU : (unsigned)blocklen);
    w->pos = 0;
    w->blocktyp = PFB_ASCII;
    w->binary_blocks_written = 0;
    w->f = f;
}

void
pfb_writer_output_block(struct pfb_writer *w)
{
  /* do nothing if nothing in block */
  if (w->pos == 0)
    return;

  /* output four-byte block length */
  putc(PFB_MARKER, w->f);
  putc(w->blocktyp, w->f);
  putc((int)(w->pos & 0xff), w->f);
  putc((int)((w->pos >> 8) & 0xff), w->f);
  putc((int)((w->pos >> 16) & 0xff), w->f);
  putc((int)((w->pos >> 24) & 0xff), w->f);

  /* output block data */
  fwrite(w->buf, 1, w->pos, w->f);

  /* mark block buffer empty and uninitialized */
  w->pos =  0;
  if (w->blocktyp == PFB_BINARY)
    w->binary_blocks_written++;
}

void
pfb_writer_grow_buf(struct pfb_writer *w)
{
  if (w->len < w->max_len) {
    /* grow w->buf */
    unsigned new_len = w->len * 2;
    unsigned char *new_buf;
    if (new_len > w->max_len)
      new_len = w->max_len;
    new_buf = (unsigned char *)malloc(new_len);
    if (!new_buf) {
      error("out of memory; continuing with a smaller block size");
      w->max_len = w->len;
      pfb_writer_output_block(w);
    } else {
      memcpy(new_buf, w->buf, w->len);
      free(w->buf);
      w->buf = new_buf;
      w->len = new_len;
    }

  } else
    /* buf already the right size, just output the block */
    pfb_writer_output_block(w);
}

void
pfb_writer_end(struct pfb_writer *w)
{
  if (w->pos)
    pfb_writer_output_block(w);
  putc(PFB_MARKER, w->f);
  putc(PFB_DONE, w->f);
}

/* This CRC table and routine were borrowed from macutils-2.0b3 */

static unsigned short crctab[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
    0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
    0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
    0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
    0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
    0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
    0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
    0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
    0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
    0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
    0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
    0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
    0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
    0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
    0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
    0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
    0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
    0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
    0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
    0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
    0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0,
};

/*
 * Update a CRC check on the given buffer.
 */

int
crcbuf(int crc, unsigned int len, const char *buf)
{
    const unsigned char *ubuf = (const unsigned char *)buf;
    while (len--)
	crc = ((crc << 8) & 0xFF00) ^ crctab[((crc >> 8) & 0xFF) ^ *ubuf++];
    return crc;
}

#ifdef __cplusplus
}
#endif
