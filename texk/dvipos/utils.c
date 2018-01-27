/*  dvipos-20070107

    Copyright (C) 2003 by Jin-Hwan <chofchof@ktug.or.kr>
    
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

#include "utils.h"

#include <stdarg.h>

UNSIGNED_BYTE get_unsigned_byte (FILE *fp)
{
  return (UNSIGNED_BYTE)(fgetc(fp) & 0xFF);
}

SIGNED_BYTE get_signed_byte (FILE *fp)
{
  UNSIGNED_BYTE byte = get_unsigned_byte(fp);
  return (SIGNED_BYTE)(byte & 0x80 ? byte - 0x100 : byte);
}

UNSIGNED_PAIR get_unsigned_pair (FILE *fp)
{
  UNSIGNED_PAIR pair = get_unsigned_byte(fp);
  /* Read the second byte */
  pair = pair << 8; pair += get_unsigned_byte(fp);
  return pair;
}

SIGNED_PAIR get_signed_pair (FILE *fp)
{
  UNSIGNED_PAIR pair;
  UNSIGNED_BYTE byte = get_unsigned_byte(fp);
  pair = (byte & 0x80 ? byte - 0x100 : byte);
  /* Read the second byte */
  pair = pair << 8; pair += get_unsigned_byte(fp);
  return (SIGNED_PAIR)pair;
}

UNSIGNED_TRIPLE get_unsigned_triple (FILE *fp)
{
  UNSIGNED_TRIPLE triple = get_unsigned_byte(fp);
  /* Read the second, the third byte */
  triple = triple << 8; triple += get_unsigned_byte(fp);
  triple = triple << 8; triple += get_unsigned_byte(fp);
  return triple;
}

UNSIGNED_TRIPLE get_unsigned_triple_kanji (FILE *fp)
{
  /* yy zz XX -> XXyyzz */
  UNSIGNED_TRIPLE triple = get_unsigned_byte(fp);
  triple = triple << 8; triple += get_unsigned_byte(fp);
  triple += get_unsigned_byte(fp) << 16;
  return triple;
}

SIGNED_TRIPLE get_signed_triple (FILE *fp)
{
  UNSIGNED_TRIPLE triple;
  /* Read the first byte and check the sign */
  UNSIGNED_BYTE byte = get_unsigned_byte(fp);
  triple = (byte & 0x80 ? byte - 0x100 : byte);
  /* Read the second, the third byte */
  triple = triple << 8; triple += get_unsigned_byte(fp);
  triple = triple << 8; triple += get_unsigned_byte(fp);
  return (SIGNED_TRIPLE)triple;
}

UNSIGNED_QUAD get_unsigned_quad (FILE *fp)
{
  UNSIGNED_QUAD quad = get_unsigned_byte(fp);
  /* Read the second, the third, and the fourth byte */
  quad = quad << 8; quad += get_unsigned_byte(fp);
  quad = quad << 8; quad += get_unsigned_byte(fp);
  quad = quad << 8; quad += get_unsigned_byte(fp);
  return quad;
}

SIGNED_QUAD get_signed_quad (FILE *fp)
{
  UNSIGNED_QUAD quad;
  /* Read the first byte and check the sign */
  UNSIGNED_BYTE byte = get_unsigned_byte(fp);
  quad = (byte & 0x80 ? byte - 0x100 : byte);
  /* Read the second, the third, and the fourth byte */
  quad = quad << 8; quad += get_unsigned_byte(fp);
  quad = quad << 8; quad += get_unsigned_byte(fp);
  quad = quad << 8; quad += get_unsigned_byte(fp);
  return (SIGNED_QUAD)quad;
}

void put_unsigned_byte (SIGNED_QUAD quad, FILE *fp)
{
  fputc(quad & 0xff, fp);
  dbg_location++;
}

void put_signed_byte (SIGNED_QUAD quad, FILE *fp)
{
  if (quad < 0) fputc((quad + 0x100) & 0xff, fp);
  else fputc(quad & 0xff, fp);
  dbg_location++;
}

void put_unsigned_pair (SIGNED_QUAD quad, FILE *fp)
{
  put_unsigned_byte(quad >> 8, fp);
  put_unsigned_byte(quad, fp);
}

void put_signed_pair (SIGNED_QUAD quad, FILE *fp)
{
  put_signed_byte(quad >> 8, fp);
  put_unsigned_byte(quad, fp);
}

void put_unsigned_triple (SIGNED_QUAD quad, FILE *fp)
{
  put_unsigned_byte(quad >> 16, fp);
  put_unsigned_pair(quad & 0xffff, fp);
}

void put_signed_triple (SIGNED_QUAD quad, FILE *fp)
{
  put_signed_byte(quad >> 16, fp);
  put_unsigned_pair(quad & 0xffff, fp);
}

void put_signed_quad (SIGNED_QUAD quad, FILE *fp)
{
  put_signed_byte(quad >> 24, fp);
  put_unsigned_triple(quad & 0xffffff, fp);
}

void sput_signed_pair (char *buf, SIGNED_QUAD quad)
{
  if (quad < 0) *buf++ = (quad >> 8) + 0x100;
  else *buf++ = quad >> 8;
  *buf = quad & 0xff;
}

void sput_signed_quad (char *buf, SIGNED_QUAD quad)
{
  if (quad < 0) *buf++ = (quad >> 24) + 0x100;
  else *buf++ = quad >> 24;
  *buf++ = quad >> 16;
  *buf++ = quad >> 8;
  *buf = quad & 0xff;
}

#ifndef KPATHSEA
/* Borrowed from the kpathsea library, concat3.c */
char *concat (const char *s1, const char *s2)
{
  unsigned s1len = strlen(s1);
  unsigned s2len = strlen(s2);
  char *answer = (char *)xmalloc(s1len + s2len + 1);
  strcpy(answer, s1);
  strcat(answer + s1len, s2);
  return answer;
}

/* Borrowed from the kpathsea library, concat3.c */
char *concat3 (const char *s1, const char *s2, const char *s3)
{
  char *answer = (char *)xmalloc(strlen(s1) + strlen(s2) + strlen(s3) + 1);
  strcpy(answer, s1);
  strcat(answer, s2);
  strcat(answer, s3);
  return answer;
}

/* Borrowed from the kpathsea library, make-suffix.c */
/* Return a new string: S suffixed with SUFFIX, regardless of what it
 * was before. This returns a newly allocated string.  */ 
char *make_suffix (const char *s, const char *suffix)
{
  char *new_s;
  const char *dot_pos = strrchr(s, '.');
  const char *slash_pos;
  
  for (slash_pos = s + strlen(s) - 1; slash_pos > dot_pos && slash_pos > s; slash_pos--)
    if (IS_DIR_SEP(*slash_pos)) break;

  if (dot_pos == NULL || slash_pos > dot_pos)
    new_s = concat3(s, ".", suffix);
  else {
    unsigned past_dot_index = dot_pos + 1 - s;
    new_s = (char *)xmalloc (past_dot_index + strlen (suffix) + 1);
    strncpy(new_s, s, dot_pos + 1 - s);
    strcpy(new_s + past_dot_index, suffix);
  }

  return new_s;
}
#endif

void msg_out (int level, const char *fmt, ...)
{
  va_list args;
  if (verbose & level) {
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
  }
  if (level == M_FAIL)
    exit(1);
}
