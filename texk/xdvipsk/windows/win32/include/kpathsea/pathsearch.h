/* pathsearch.h: mostly-generic path searching.

   Copyright 1993, 1994, 1996, 1997, 2007, 2008, 2009, 2011, 2012,
   2014, 2018 Karl Berry.
   Copyright 1999-2005 Olaf Weber.

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

#ifndef KPATHSEA_PATHSEARCH_H
#define KPATHSEA_PATHSEARCH_H

#include <kpathsea/c-proto.h>

#include <kpathsea/str-llist.h>
#include <kpathsea/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The naming of all these functions is rather scattered and
   inconsistent, but they grew over time, and we don't want to change
   the meaning of existing names.  */

#ifdef MAKE_KPSE_DLL /* libkpathsea internal only */

/* If PATH is non-null, return its first element (as defined by
   IS_KPSE_SEP).  If it's NULL, return the next element in the previous
   path, a la strtok.  Leading, trailing, or doubled colons result in
   the empty string.  When at the end of PATH, return NULL.  In any
   case, return a pointer to an area that may be overwritten on
   subsequent calls.  */
extern string kpathsea_path_element (kpathsea kpse, const_string path);

/* Like `kpathsea_path_element', but for filename components (using
   IS_DIR_SEP).  Uses same area as `kpathsea_path_element'.  */
extern string kpathsea_filename_component (kpathsea kpse, const_string path);

/* This function may rewrite its argument to avoid bugs when calling
   stat() or equivalent under Windows.  Also, it returns the index after
   which the program should start to look for expandable constructs. */
extern unsigned kpathsea_normalize_path (kpathsea kpse, string elt);

/* Given a path element ELT, return a pointer to a NULL-terminated list
   of the corresponding (existing) directory or directories, with
   trailing slashes, or NULL.  If ELT is the empty string, check the
   current working directory.

   It's up to the caller to expand ELT.  This is because this routine is
   most likely only useful to be called from `kpathsea_path_search', which
   has already assumed expansion has been done.  */
extern str_llist_type *kpathsea_element_dirs (kpathsea kpse, string elt);

#endif /* MAKE_KPSE_DLL */

/* Call `kpathsea_expand' on NAME.  If the result is an absolute or
   explicitly relative filename, check whether it is a readable
   (regular) file.

   Otherwise, look in each of the directories specified in PATH (also do
   tilde and variable expansion on elements in PATH), using a prebuilt
   db (see db.h) if it's relevant for a given path element.

   If the prebuilt db doesn't exist, or if MUST_EXIST is true and NAME
   isn't found in the prebuilt db, look on the filesystem.  (I.e., if
   MUST_EXIST is false, and NAME isn't found in the db, do *not* look on
   the filesystem.)

   The caller must expand PATH. This is because it makes more sense to
   do this once, in advance, instead of for every search.

   In any case, return a matching filename if found, otherwise NULL.
   If more than one file matches, which one gets returned is
   unspecified.  */
extern KPSEDLL string kpathsea_path_search
  (kpathsea kpse, const_string path, const_string name, boolean must_exist);

/* Like `kpathsea_path_search' with MUST_EXIST true, but always return all
   matches in a NULL-terminated list.  */
extern KPSEDLL string *kpathsea_all_path_search
  (kpathsea kpse, const_string path, const_string name);

#ifdef MAKE_KPSE_DLL /* libkpathsea internal only */

/* Search for any of the NAMES in PATH, and allow specifying both
   MUST_EXIST and ALL.  */
extern string *kpathsea_path_search_list_generic (kpathsea kpse,
   const_string path, string* names, boolean must_exist, boolean all);

#endif /* MAKE_KPSE_DLL */

#if defined(KPSE_COMPAT_API)

extern KPSEDLL string kpse_path_search
  (const_string path, const_string name, boolean must_exist);
extern KPSEDLL string *kpse_all_path_search
  (const_string path, const_string name);

#endif

#ifdef __cplusplus
}
#endif

#endif /* not KPATHSEA_PATHSEARCH_H */
