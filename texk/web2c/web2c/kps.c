/* Functions copied over from kpathsea library.

Original uppercasify is
  Copyright 1993 Karl Berry.

Original xfopen/xfclose are
  Copyright 1992, 93, 95 Free Software Foundation, Inc.

Further modifications
  Copyright 2004 Olaf Weber.
  
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "web2c.h"
#include <kpathsea/c-ctype.h>
/* Get rid of kpathsea debug definitions. */
#undef fopen
#undef fclose

string
uppercasify (const_string s)
{
  string target;
  string ret = malloc (strlen(s) + 1);

  target = ret;
  while (*s) {
      *target = TOUPPER(*s);
      target++;
      s++;
  }
  *target = '\0';
  
  return ret;
}

/* These routines just check the return status from standard library
   routines and abort if an error happens.  */

FILE *
xfopen (const_string filename,  const_string mode)
{
  FILE *f;

  assert (filename && mode);

  f = fopen (filename, mode);
  if (f == NULL) {
    perror(filename);
    exit(EXIT_FAILURE);
  }

  return f;
}


void
xfclose (FILE *f,  const_string filename)
{
  assert (f);

  if (fclose (f) == EOF) {
    perror(filename);
    exit(EXIT_FAILURE);
  }
}
