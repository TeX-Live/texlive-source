/* dir.c: directory operations.

   Copyright 1992, 1993, 1994, 1995, 2008-2012 Karl Berry.
   Copyright 2000, 2002, 2005 Olaf Weber.

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

#include <kpathsea/c-dir.h>
#include <kpathsea/c-stat.h>
#include <kpathsea/hash.h>
#include <kpathsea/pathsearch.h>

/* Return true if FN is a directory or a symlink to a directory,
   false if not. */

boolean
kpathsea_dir_p (kpathsea kpse, string fn)
{
  /* FIXME : using the stat() replacement in gnuw32,
         we could avoid this win32 specific code. However,
         I wonder if it would be as fast as this one is ?
  */
#ifdef WIN32
  int fa;

  kpathsea_normalize_path(kpse, fn);
  fa = GetFileAttributes(fn);

#ifdef KPSE_DEBUG
  if (KPATHSEA_DEBUG_P (KPSE_DEBUG_STAT)) {
    if (fa == 0xFFFFFFFF) {
      fprintf(stderr, "failed to get file attributes for %s (%d)\n",
              fn, (int)(GetLastError()));
    } else {
      fprintf(stderr, "path %s %s a directory\n",
              fn , (fa & FILE_ATTRIBUTE_DIRECTORY) ?
              "is"  : "is not");
    }
  }
#endif /* KPSE_DEBUG */
  return (fa != 0xFFFFFFFF && (fa & FILE_ATTRIBUTE_DIRECTORY));
#else /* !WIN32 */
  struct stat stats;
  return stat (fn, &stats) == 0 && S_ISDIR (stats.st_mode);
#endif /* !WIN32 */
}

#if defined(KPSE_COMPAT_API)
boolean
dir_p (string fn)
{
    return kpathsea_dir_p (kpse_def, fn);
}
#endif


/*
  Return -1 if FN isn't a directory, else its number of links.
  Duplicate the call to stat; no need to incur overhead of a function
  call for that little bit of cleanliness.

  The process is a bit different under Win32 : the first call
  memoizes the nlinks value, the following ones retrieve it.
*/
int
kpathsea_dir_links (kpathsea kpse, const_string fn, long nlinks)
{
  const_string *hash_ret;

  if (kpse->link_table.size == 0)
    kpse->link_table = hash_create (457);

#ifdef KPSE_DEBUG
  /* This is annoying, but since we're storing integers as pointers, we
     can't print them as strings.  */
  if (KPATHSEA_DEBUG_P (KPSE_DEBUG_HASH))
    kpse->debug_hash_lookup_int = true;
#endif

  hash_ret = hash_lookup (kpse->link_table, fn);

#ifdef KPSE_DEBUG
  if (KPATHSEA_DEBUG_P (KPSE_DEBUG_HASH))
    kpse->debug_hash_lookup_int = false;
#endif

  /* Have to cast the int we need to/from the const_string that the hash
     table stores for values. Let's hope an int fits in a pointer.  */
  if (hash_ret) {
#ifdef WIN32
      memcpy(&nlinks, hash_ret, sizeof(nlinks));
#else
      nlinks = (long) *hash_ret;
#endif
  } else {
#ifdef WIN32
      /* Insert it only if we have some informations about it. */
      if (nlinks) {
        char str_nlinks[sizeof(nlinks)+1];
        memcpy(str_nlinks, (char *)&nlinks, sizeof(nlinks));
        str_nlinks[sizeof(nlinks)] = '\0';
        /* It's up to us to copy the value.  */
        hash_insert(&(kpse->link_table), xstrdup (fn),
                    (const_string) str_nlinks);
      }
#else
      struct stat stats;
      if (stat (fn, &stats) == 0 && S_ISDIR (stats.st_mode))
        nlinks = stats.st_nlink;
      else
        nlinks = -1;
      /* It's up to us to copy the value.  */
      hash_insert(&(kpse->link_table), xstrdup(fn), (const_string)nlinks);
#endif

#ifdef KPSE_DEBUG
      if (KPATHSEA_DEBUG_P (KPSE_DEBUG_STAT))
        DEBUGF2 ("dir_links(%s) => %ld\n", fn, nlinks);
#endif
  }

  /* In any case, return nlinks
     (either 0, the value inserted or the value retrieved. */
  return nlinks;
}

#if defined (KPSE_COMPAT_API)
int
dir_links (const_string fn, long nlinks)
{
    return kpathsea_dir_links (kpse_def, fn, nlinks);
}
#endif
