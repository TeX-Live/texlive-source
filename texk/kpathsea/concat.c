/* concat.c: dynamic string concatenation.

   Copyright 1992, 1993, 2008 Karl Berry.
   Copyright 2002, 2005 Olaf Weber.

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


/* Return the concatenation of S1 and S2.  See `concatn.c' for a
   `concatn', which takes a variable number of arguments.  */

string
concat (const_string s1,  const_string s2)
{
  unsigned s1len = strlen(s1);
  unsigned s2len = strlen(s2);
  string answer = (string) xmalloc (s1len + s2len + 1);
  strcpy (answer, s1);
  strcat (answer + s1len, s2);

  return answer;
}
