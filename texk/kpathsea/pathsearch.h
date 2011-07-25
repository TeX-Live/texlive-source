/* pathsearch.h: mostly-generic path searching.

   Copyright 1993, 1994, 1996, 1997, 2007, 2008, 2009 Karl Berry.
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

/* If PATH is non-null, return its first element (as defined by
   IS_ENV_SEP).  If it's NULL, return the next element in the previous
   path, a la strtok.  Leading, trailing, or doubled colons result in
   the empty string.  When at the end of PATH, return NULL.  In any
   case, return a pointer to an area that may be overwritten on
   subsequent calls.  */
extern KPSEDLL string kpathsea_path_element (kpathsea kpse, const_string path);

/* Like `kpathsea_path_element', but for filename components (using
   IS_DIR_SEP).  Uses same area as `kpathsea_path_element'.  */
extern string kpathsea_filename_component (kpathsea kpse, const_string path);

/*
  This function may rewrite its argument to avoid bugs when calling
  stat() or equivalent under Win32.  Also, it returns the index after
  which the program should start to look for expandable constructs. */
extern unsigned kpathsea_normalize_path (kpathsea kpse, string elt);

/* Given a path element ELT, return a pointer to a NULL-terminated list
   of the corresponding (existing) directory or directories, with
   trailing slashes, or NULL.  If ELT is the empty string, check the
   current working directory.

   It's up to the caller to expand ELT.  This is because this routine is
   most likely only useful to be called from `kpathsea_path_search', which
   has already assumed expansion has been done.  */
extern KPSEDLL str_llist_type *kpathsea_element_dirs (kpathsea kpse,
                                                      string elt);


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

/* Like `kpathsea_path_search', except look for a list of NAMES.  */
extern KPSEDLL string kpathsea_path_search_list
  (kpathsea kpse, const_string path, const_string* names, boolean must_exist);

/* Like `kpathsea_path_search' with MUST_EXIST true, but always return all
   matches in a NULL-terminated list.  */
extern KPSEDLL string *kpathsea_all_path_search
  (kpathsea kpse, const_string path, const_string name);

/* Search for any of the NAMES in PATH, and allow specifying both
   MUST_EXIST and ALL.  */
extern KPSEDLL string *kpathsea_path_search_list_generic (kpathsea kpse,
   const_string path, const_string* names, boolean must_exist, boolean all);

/* Search for any of NAMES, with MUST_EXIST and ALL true.  */
extern KPSEDLL string *kpathsea_all_path_search_list
  (kpathsea kpse, const_string path, const_string* names);

/* The naming of all these functions is rather scattered and
   inconsistent, but they grew over time, and we don't want to change
   the meaning of existing names.  */

#if defined(KPSE_COMPAT_API)

extern KPSEDLL string kpse_path_element (const_string path);
extern string kpse_filename_component (const_string path);
extern unsigned kpse_normalize_path (string elt);
extern KPSEDLL str_llist_type *kpse_element_dirs (string elt);
extern KPSEDLL string kpse_path_search
  (const_string path, const_string name, boolean must_exist);
extern KPSEDLL string kpse_path_search_list
  (const_string path, const_string* names, boolean must_exist);
extern KPSEDLL string *kpse_all_path_search
  (const_string path, const_string name);
extern KPSEDLL string *kpse_path_search_list_generic
  (const_string path, const_string* names, boolean must_exist, boolean all);
extern KPSEDLL string *kpse_all_path_search_list
  (const_string path, const_string* names);

#endif

#endif /* not KPATHSEA_PATHSEARCH_H */
