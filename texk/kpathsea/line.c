/* line.c: return the next line from a file, or NULL.

   Copyright 1992, 1993, 1995, 1996, 2008, 2013, 2014 Karl Berry.
   Copyright 1998, 1999, 2001, 2005 Olaf Weber.

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

#include <kpathsea/config.h>
#include <kpathsea/line.h>

#ifdef WIN32
#undef getc
#undef ungetc
#define getc   win32_getc
#define ungetc win32_ungetc
#define FLOCKFILE(x)
#define FUNLOCKFILE(x)

#else /* not WIN32 */
/* By POSIX, getc() has to be thread-safe, which means (un)locking on
   every character read.  It is much faster to lock the stream (once),
   use getc_unlocked to read, and then unlock the stream.  We need to be
   thread-safe especially for the sake of MPlib.
   
   Perhaps we will be lucky enough to be able to do this
   unconditionally, without checking in configure.  We'll see.  */
#undef getc
#define getc           getc_unlocked
#define FLOCKFILE(x)   flockfile(x)
#define FUNLOCKFILE(x) funlockfile(x)
#endif /* not WIN32 */

/* Allocate in increments of this size.  */
#define BLOCK_SIZE 75

char *
read_line (FILE *f)
{
  int c;
  unsigned limit = BLOCK_SIZE;
  unsigned loc = 0;
  char *line = xmalloc (limit);
  
  FLOCKFILE (f);

  while ((c = getc (f)) != EOF && c != '\n' && c != '\r') {
    line[loc] = c;
    loc++;

    /* By testing after the assignment, we guarantee that we'll always
       have space for the null we append below.  We know we always
       have room for the first char, since we start with BLOCK_SIZE.  */
    if (loc == limit) {
      limit += BLOCK_SIZE;
      line = xrealloc (line, limit);
    }
  }

  /* If we read anything, return it, even a partial last-line-if-file
     which is not properly terminated.  */
  if (loc == 0 && c == EOF) {
    /* At end of file.  */
    free (line);
    line = NULL;
  } else { 
    /* Terminate the string.  We can't represent nulls in the file,
       but this doesn't matter.  */
    line[loc] = 0;
    /* Absorb LF of a CRLF pair. */
    if (c == '\r') {
      c = getc (f);
      if (c != '\n') {
        ungetc (c, f);
      }
    }
  }
  
  FUNLOCKFILE (f);

  return line;
}
