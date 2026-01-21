/* c-fopen.h: how to open files with fopen.

   Copyright 1992, 1994, 1995, 1996, 2008, 2011 Karl Berry.
   Copyright 1998, 2005 Olaf Weber.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library; if not, see <http://www.gnu.org/licenses/>.  */

#ifndef C_FOPEN_H
#define C_FOPEN_H

/* How to open a text file:  */
/* From Akira:
   I'm using Unix style line ending character to write text files.
   I find it is easiest to define FOPEN_W_MODE == FOPEN_WBIN_MODE etc. for
   my purpose.  */
#ifndef FOPEN_A_MODE
#define FOPEN_A_MODE "ab"
#endif

#ifndef FOPEN_R_MODE
#define FOPEN_R_MODE "r"
#endif

#ifndef FOPEN_W_MODE
#define FOPEN_W_MODE "wb"
#endif

/* How to open a binary file for reading:  */
#ifndef FOPEN_RBIN_MODE
#define FOPEN_RBIN_MODE "rb"
#endif /* not FOPEN_RBIN_MODE */

/* How to open a binary file for writing:  */
#ifndef FOPEN_WBIN_MODE
#define FOPEN_WBIN_MODE "wb"
#endif /* not FOPEN_WBIN_MODE */

/* How to open a binary file for appending:  */
#ifndef FOPEN_ABIN_MODE
#define FOPEN_ABIN_MODE "ab"
#endif /* not FOPEN_ABIN_MODE */

/* How to switch an already open file handle to binary mode.
   Used on DOSISH systems when we need to switch a standard
   stream, such as stdin or stdout, to binary mode.
   We never use the value return by setmode().  */
#include <fcntl.h>
#ifdef DOSISH
#include <io.h>
#ifndef O_BINARY
#ifdef _O_BINARY
#define O_BINARY _O_BINARY
#endif
#endif
#if defined (__i386_pc_gnu__) || \
    defined (WIN32) || defined (__WIN32__) || defined (_WIN32)
#define SET_BINARY(f) (void)setmode((f), O_BINARY)
#endif
#else  /* not DOSISH */
#ifndef O_BINARY
#define O_BINARY 0
#endif
#define SET_BINARY(f) (void)0
#endif /* not DOSISH */

#endif /* not C_FOPEN_H */
