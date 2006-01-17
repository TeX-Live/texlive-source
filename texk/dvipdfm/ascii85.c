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

	
#include <stdio.h>
#include <ctype.h>

static int nextch(void)
{
  int ch;
  while ((ch = getchar()) >= 0) {
    if (ch >= '!' && ch <= 'u' || ch == 'z')
      return ch;
    if (ch == '~' && getchar() == '>') {
      return -1;
    }
  }
  if (ch < 0) {
    fprintf (stderr, "Premature end of file.\n");
    exit(1);
  }
}

unsigned long int ascii85;
unsigned binary[4];

static decode_block(void)
{
  int i;
  for (i=3; i>=0; i--){
    binary[i] = ascii85 % 256u;
    ascii85 /= 256u;
  }
}

static output_block (int n)
{
  int i;
  for (i=0; i<n; i++) {
    putchar (binary[i]);
  }
}

main (int argc, char *argv[])
{
  int i, ch, eof = 0, nread;
  while (!eof) {
    ascii85 = 0;
    nread = 0;
    /* Look ahead for special zero key */
    if ((ch = nextch()) == 'z') {
      nread = 4;  /* Lie to it */
    } else{
      ungetc (ch, stdin);
      for (i=0; i<5; i++) {
	if ((ch=nextch()) < 0) {
	  eof = 1;
	  break;
	}
	ascii85 = ascii85 * 85 + (ch-'!');
	nread += 1;
      }
    }
    if (nread > 1) {
      decode_block();
      output_block(nread-1);
    }
  }
}
