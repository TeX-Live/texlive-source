/* db.h: lookups in an externally built db file.

   Copyright 1994, 1995, 2008, 2010, 2011 Karl Berry.
   Copyright 1999, 2003, 2005 Olaf Weber.

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

#ifndef KPATHSEA_DB_H
#define KPATHSEA_DB_H

#ifdef MAKE_KPSE_DLL /* libkpathsea internal only */

#include <kpathsea/c-proto.h>
#include <kpathsea/types.h>
#include <kpathsea/str-list.h>

/* Initialize the database.  Until this is called, no ls-R matches will
   be found.  */
extern void kpathsea_init_db (kpathsea kpse);

/* Return list of matches for NAME in the ls-R file matching PATH_ELT.  If
   ALL is set, return (null-terminated list) of all matches, else just
   the first.  If no matches, return a pointer to an empty list.  If no
   databases can be read, or PATH_ELT is not in any of the databases,
   return NULL.  */

extern str_list_type *kpathsea_db_search (kpathsea kpse, const_string name,
                                          const_string path_elt, boolean all);

/* Note: NAMES[i] is not modified.  */
extern str_list_type *kpathsea_db_search_list (kpathsea kpse,
                                               string* names,
                                               const_string  path_elt,
                                               boolean all);

/* Insert the filename FNAME into the database.
   Called by mktex() in tex-make.c.  */
extern void kpathsea_db_insert (kpathsea kpse, const_string fname);

#endif /* MAKE_KPSE_DLL */

#endif /* not KPATHSEA_DB_H */
