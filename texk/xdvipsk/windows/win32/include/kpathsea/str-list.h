/* str-list.h: declarations for string lists.

   Copyright 1993, 1994, 2007, 2008, 2010, 2012, 2018 Karl Berry.
   Copyright 1999, 2005 Olaf Weber.

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

#ifndef KPATHSEA_STR_LIST_H
#define KPATHSEA_STR_LIST_H

#include <kpathsea/c-proto.h>
#include <kpathsea/types.h>


#ifdef __cplusplus
extern "C" {
#endif

/* Lists of strings; used for, e.g., directory lists.  */
typedef struct
{
  unsigned length;
  string *list;
} str_list_type;

/* Lists of const strings; used for, e.g., hash tables.  */
typedef struct
{
  unsigned length;
  const_string *list;
} cstr_list_type;

#define STR_LIST_LENGTH(l) ((l).length)
#define STR_LIST_EMPTY(l) (STR_LIST_LENGTH (l) == 0)

#define STR_LIST(l) ((l).list)
#define STR_LIST_ELT(l, n) (STR_LIST (l)[n])
#define STR_LIST_FIRST_ELT(l) STR_LIST_ELT ((l), 0)
#define STR_LIST_LAST_ELT(l) STR_LIST_ELT ((l), STR_LIST_LENGTH (l) - 1)

#ifdef MAKE_KPSE_DLL /* libkpathsea internal only */

/* Return a new, empty, list.  */
static inline str_list_type
str_list_init (void)
{
  str_list_type ret;

  STR_LIST_LENGTH (ret) = 0;
  STR_LIST (ret) = NULL;

  return ret;
}
static inline cstr_list_type
cstr_list_init (void)
{
  cstr_list_type ret;

  STR_LIST_LENGTH (ret) = 0;
  STR_LIST (ret) = NULL;

  return ret;
}

#endif /* MAKE_KPSE_DLL */

/* Append the string S to the list L.  It's up to the caller to not
   deallocate S; we don't copy it.  Also up to the caller to terminate
   the list with a null entry.  */
extern KPSEDLL void str_list_add (str_list_type *l, string s);
extern KPSEDLL void cstr_list_add (cstr_list_type *l, const_string s);

#ifdef MAKE_KPSE_DLL /* libkpathsea internal only */

/* Append all the elements from MORE to TARGET.  */
extern void str_list_concat (str_list_type * target, str_list_type more);

/* Free the space for the list elements (but not the list elements
   themselves).  */
extern void str_list_free (str_list_type *l);

/* Append each element of MORE to each element of TARGET.  */
extern void str_list_concat_elements
  (str_list_type *target, str_list_type more);

/* Remove duplicate elements from L, freeing their space.  */
extern void str_list_uniqify (str_list_type *l);

#endif /* MAKE_KPSE_DLL */

#ifdef __cplusplus
}
#endif

#endif /* not KPATHSEA_STR_LIST_H */
