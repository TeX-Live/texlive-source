/* libc replacement functions for win32.

Copyright (C) 1998, 99 Free Software Foundation, Inc.

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

#include <string.h>
#include <win32lib.h>

char *xstrdup(const char *s)
{
  char *p = _strdup(s);
  if (!p) {
    fprintf(stderr, "strdup(%x) failed in gnuw32.c at line %d\n", s, __LINE__);
    exit(1);
  }
  return p;
}

void *xmalloc(unsigned size)
{
  void *p = malloc(size);
  if (!p) {
    fprintf(stderr, "malloc(%d) failed in gnuw32.c at line %d\n", size, __LINE__);
    exit(1);
  }
  return p;
}

void *xrealloc(void *p, unsigned size)
{
  p = realloc(p, size);
  if (!p) {
    fprintf(stderr, "realloc(%d) failed in gnuw32.c at line %d\n", size, __LINE__);
    exit(1);
  }
  return p;
}

char *concat(const char *s1,  const char *s2)
{
  char *answer = (char *) xmalloc (strlen (s1) + strlen (s2) + 1);
  strcpy (answer, s1);
  strcat (answer, s2);

  return answer;
}

char *concat3(const char *s1,  const char *s2,  const char *s3)
{
  char *answer
    = (char *) xmalloc (strlen (s1) + strlen (s2) + strlen (s3) + 1);
  strcpy (answer, s1);
  strcat (answer, s2);
  strcat (answer, s3);

  return answer;
}

char *concatn(const char *str1,  ...)
{
  char *arg, *ret;
  int size = 1;
  va_list marker;

  if (!str1)
    return NULL;

  size = strlen(str1);

  va_start(marker, str1);
  while ((arg = va_arg (marker, char*)) != NULL) {
    size += strlen(arg);
  }
  va_end (marker);
  
  
  ret = xmalloc(size + 1);
  strcpy(ret, str1);
  
  va_start(marker, str1);
  while ((arg = va_arg (marker, char*)) != NULL) {
    strcat(ret, arg);
  }
  va_end (marker);
  
  return ret;
}
