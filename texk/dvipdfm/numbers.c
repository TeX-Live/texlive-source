/*  $Header$

    This is dvipdfm, a DVI to PDF translator.
    Copyright (C) 1998, 1999 by Mark A. Wicks

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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    
    The author may be contacted via the e-mail address

	mwicks@kettering.edu
*/

#include "system.h"	
#include "error.h"
#include "mfileio.h"
#include "numbers.h"

UNSIGNED_BYTE get_unsigned_byte (FILE *file)
{
  int ch;
  if ((ch = fgetc (file)) < 0)
    ERROR ("File ended prematurely\n");
  return (UNSIGNED_BYTE) ch;
}

UNSIGNED_BYTE sget_unsigned_byte (char *s)
{
  return *((unsigned char *) s);
}

SIGNED_BYTE get_signed_byte (FILE *file)
{
  int byte;
  byte = get_unsigned_byte(file);
  if (byte >= 0x80) 
    byte -= 0x100;
  return (SIGNED_BYTE) byte;
}

UNSIGNED_PAIR get_unsigned_pair (FILE *file)
{
  int i;
  UNSIGNED_BYTE byte;
  UNSIGNED_PAIR pair = 0;
  for (i=0; i<2; i++) {
    byte = get_unsigned_byte(file);
    pair = pair*0x100u + byte;
  }
  return pair;
}

UNSIGNED_PAIR sget_unsigned_pair (unsigned char *s)
{
  int i;
  UNSIGNED_BYTE byte;
  UNSIGNED_PAIR pair = 0;
  for (i=0; i<2; i++) {
    byte = *(s++);
    pair = pair*0x100u + byte;
  }
  return pair;
}

SIGNED_PAIR get_signed_pair (FILE *file)
{
  int i;
  long pair = 0;
  for (i=0; i<2; i++) {
    pair = pair*0x100 + get_unsigned_byte(file);
  }
  if (pair >= 0x8000) {
    pair -= 0x10000l;
  }
  return (SIGNED_PAIR) pair;
}


UNSIGNED_TRIPLE get_unsigned_triple(FILE *file)
{
  int i;
  long triple = 0;
  for (i=0; i<3; i++) {
    triple = triple*0x100u + get_unsigned_byte(file);
  }
  return (UNSIGNED_TRIPLE) triple;
}

SIGNED_TRIPLE get_signed_triple(FILE *file)
{
  int i;
  long triple = 0;
  for (i=0; i<3; i++) {
    triple = triple*0x100 + get_unsigned_byte(file);
  }
  if (triple >= 0x800000l) 
    triple -= 0x1000000l;
  return (SIGNED_TRIPLE) triple;
}

SIGNED_QUAD get_signed_quad(FILE *file)
{
  int byte, i;
  long quad = 0;

  /* Check sign on first byte before reading others */
  byte = get_unsigned_byte(file);
  quad = byte;
  if (quad >= 0x80) 
    quad = byte - 0x100;
  for (i=0; i<3; i++) {
    quad = quad*0x100 + get_unsigned_byte(file);
  }
  return (SIGNED_QUAD) quad;
}

UNSIGNED_QUAD get_unsigned_quad(FILE *file)
{
  int i;
  unsigned long quad = 0;
  for (i=0; i<4; i++) {
    quad = quad*0x100u + get_unsigned_byte(file);
  }
  return (UNSIGNED_QUAD) quad;
}

SIGNED_QUAD sqxfw (SIGNED_QUAD sq, fixword fw)
{
  int sign = 1;
  unsigned long a, b, c, d, ad, bd, bc, ac;
  unsigned long e, f, g, h, i, j, k;
  unsigned long result;
  /* Make positive. */
  if (sq < 0) {
    sign = -sign;
    sq = -sq;
  }
  if (fw < 0) {
    sign = -sign;
    fw = -fw;
  }
  a = ((unsigned long) sq) >> 16u;
  b = ((unsigned long) sq) & 0xffffu;
  c = ((unsigned long) fw) >> 16u;
  d = ((unsigned long) fw) & 0xffffu;
  ad = a*d; bd = b*d; bc = b*c; ac = a*c;
  e = bd >> 16u;
  f = ad >> 16u;
  g = ad & 0xffffu;
  h = bc >> 16u;
  i = bc & 0xffffu;
  j = ac >> 16u;
  k = ac & 0xffffu;
  result = (e+g+i + (1<<3)) >> 4u;  /* 1<<3 is for rounding */
  result += (f+h+k) << 12u;
  result += j << 28u;
  return (sign>0)?result:-result;
}

SIGNED_QUAD axboverc (SIGNED_QUAD n1, SIGNED_QUAD n2, SIGNED_QUAD div)
{
  int sign = 1;
  unsigned long a, b, c, d, ad, bd, bc, ac, e, f, g, h, i, j, o;
  unsigned long high, low;
  SIGNED_QUAD result = 0;
  /*  Make positive. */
  if (n1 < 0) {
    sign = -sign;
    n1 = -n1;
  }
  if (n2 < 0) {
    sign = -sign;
    n2 = -n2;
  }
  if (div < 0) {
    sign = -sign;
    div = -div;
  }
  a = ((unsigned long) n1) >> 16u;
  b = ((unsigned long) n1) & 0xffffu;
  c = ((unsigned long) n2) >> 16u;
  d = ((unsigned long) n2) & 0xffffu;
  ad = a*d; bd = b*d; bc = b*c; ac = a*c;
  e = bd >> 16u; f = bd & 0xffffu;
  g = ad >> 16u; h = ad & 0xffffu;
  i = bc >> 16u; j = bc & 0xffffu;
  o = e+h+j;
  high = g+i+(o>>16u)+ac; o &= 0xffffu;
  low = (o << 16) + f;
  if (high >= div)
    ERROR ("Overflow in axboc");
  {
    int i;
    for (i=0; i<32; i++) {
      high *= 2;
      result *= 2;
      if (low >= 0x80000000) {
	low -= 0x80000000;
	high += 1;
      }
      low *= 2;
      if (high > div) {
	high -= div;
	result += 1;
      }
    }
  }
  high *= 2;
  if (high >= div)
    result += 1;
  return (sign>0)?result:-result;
}

/* Create a  private_itoa to be used internally in the
   hopes that a good optimizing compiler will use it inline */
static int private_itoa (char *s, long int n, int mindigits)
{
   int j, nwhole;
   char *p = s;
   if (n<0) {
      *(p++) = '-';
      n = -n;
   }
   /* Generate at least one digit in reverse order */
   nwhole = 0;
   do {
      p[nwhole++] = n%10 + '0';
      n /= 10;
   } while (n != 0 || nwhole < mindigits);
   /* Reverse the digits */
   for (j=0; j<nwhole/2; j++) {
      char tmp = p[j];
      p[j] = p[nwhole-j-1];
      p[nwhole-j-1]=tmp;
   }
   p += nwhole;
   *p = 0;
   return (p-s);
}

int inttoa (char *s, long int i)
{
  /* Call the private one */
  return private_itoa (s, i, 0);
}

int centi_u_to_a (char *s, long int n)
{
  char *p = s;
  unsigned long whole_part;
  int frac_part;
  if (n<0) {
    *(p++) = '-';
    n = -n;
  }
  whole_part = ((unsigned long) n) / 100;
  frac_part = ((unsigned long) n) % 100;
  /* Print the whole part */
  p += private_itoa (p, whole_part, 0);
  if (frac_part) {
    int mindigits = 2;
    *(p++) = '.';
    while (!(frac_part % 10)) {
      frac_part /= 10;
      mindigits -= 1;
    }
    p += private_itoa (p, frac_part, mindigits);
  }
  return (p-s);
}

int fixnumtoa (char *s, long int n)
{
   int j, thresh;
   char *p = s;
   unsigned long whole_part, frac_part;
   if (n<0) {
      *(p++) = '-';
      n = -n;
   }
   whole_part = ((unsigned long) n) / (65536l);
   frac_part = ((unsigned long) n) % 65536l;
   /* Print the whole part */
   p += private_itoa (p, whole_part, 0);

   #define BASE ((unsigned long)(256*65536l))
   frac_part *= 256;
   thresh = BASE / 10000;
   /* Round last digit */
   frac_part += thresh/2;
   if (frac_part > thresh) {
      *(p++) = '.';
   }
   for (j=0; j<4 && frac_part> thresh; j++) {
      char next_digit;
      next_digit = (10*frac_part)/BASE;
      frac_part = (10*frac_part)%BASE;
      *(p++) = next_digit + '0';
      thresh *= 10;
   }
   *p = 0;
   return (p-s);
}



