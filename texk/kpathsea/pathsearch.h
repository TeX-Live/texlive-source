/* pathsearch.h: mostly-generic path searching.

   Copyright 1999-2005 Olaf Weber.
   Copyright 1993, 94, 96, 97 Karl Berry.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/

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
extern KPSEDLL string kpse_path_element P1H(const_string path);

/* Like `kpse_path_element', but for filename components (using
   IS_DIR_SEP).  Uses same area as `kpse_path_element'.  */
extern string kpse_filename_component P1H(const_string path);

/*
  This function may rewrite its argument to avoid bugs when calling
  stat() or equivalent under Win32.  Also, it returns the index after
  which the program should start to look for expandable constructs. */
extern unsigned kpse_normalize_path P1H(string elt);

/* Given a path element ELT, return a pointer to a NULL-terminated list
   of the corresponding (existing) directory or directories, with
   trailing slashes, or NULL.  If ELT is the empty string, check the
   current working directory.
   
   It's up to the caller to expand ELT.  This is because this routine is
   most likely only useful to be called from `kpse_path_search', which
   has already assumed expansion has been done.  */
extern KPSEDLL str_llist_type *kpse_element_dirs P1H(string elt);


/* Call `kpse_expand' on NAME.  If the result is an absolute or
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
   do this once, in advance, instead of for every search using it.
   
   In any case, return the complete filename if found, otherwise NULL.  */
extern KPSEDLL string kpse_path_search P3H(const_string path,
                                           const_string name,
                                           boolean must_exist);

/* Like kpse_path_search, except we're given a list of names. */
extern KPSEDLL string kpse_path_search_list P3H(const_string path,
                                                const_string* names,
                                                boolean must_exist);

/* Like `kpse_path_search' with MUST_EXIST true, but return a list of
   all the filenames (or NULL if none), instead of taking the first.  */
extern KPSEDLL string *kpse_all_path_search P2H(const_string path,
                                                const_string name);

/* Like `kpse_path_search_list' with MUST_EXIST true, but return a list of
   all the filenames (or NULL if none), instead of taking the first.  */
extern KPSEDLL string *kpse_all_path_search_list P2H(const_string path,
                                                     const_string* names);

#endif /* not KPATHSEA_PATHSEARCH_H */

