/* Copyright (C) 2013 Kevin W. Hamlen
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301, USA.
 *
 * The latest version of this program can be obtained from
 * http://songs.sourceforge.net.
 */

#ifndef FILEIO_H
#define FILEIO_H

#if HAVE_CONFIG_H
#  include "config.h"
#else
#  include "vsconfig.h"
#endif

#include "chars.h"

#if HAVE_STDIO_H
#  include <stdio.h>
#endif

/* Define the maximum length of lines found in the input file */
#define MAXLINELEN 1024

/* FSTATE structures model the state of a FILE */
typedef struct
{
  FILE *f;
  char *filename;
  int lineno;
}
FSTATE;

/* The following functions are in fileio.c */
extern int fileopen(FSTATE *fs, const char *fnam);
extern void fileclose(FSTATE *fs);
extern int filereadln(FSTATE *fs, WCHAR *buf, int *eof);

#endif
