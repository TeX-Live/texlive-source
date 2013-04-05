/* fontmap.c: read files for additional font names.

   Copyright 1993, 1994, 1995, 1996, 1997, 2008, 2011-2013 Karl Berry.
   Copyright 2001, 2002, 2005 Olaf Weber.

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

#include <kpathsea/c-ctype.h>
#include <kpathsea/c-fopen.h>
#include <kpathsea/fontmap.h>
#include <kpathsea/hash.h>
#include <kpathsea/line.h>
#include <kpathsea/pathsearch.h>
#include <kpathsea/str-list.h>
#include <kpathsea/tex-file.h>

/* We have one and only one fontmap, so may as well make it static
   instead of passing it around.  */

#ifndef MAP_NAME
#define MAP_NAME "texfonts.map"
#endif
#ifndef MAP_HASH_SIZE
#define MAP_HASH_SIZE 4001
#endif


/* Return next whitespace-delimited token in STR or NULL if none.  */

static string
token (const_string str)
{
  unsigned len;
  const_string start;
  string ret;

  while (*str && ISSPACE (*str))
    str++;

  start = str;
  while (*str && !ISSPACE (*str))
    str++;

  len = str - start;
  ret = (string)xmalloc (len + 1);
  strncpy (ret, start, len);
  ret[len] = 0;

  return ret;
}

/* Open and read the mapping file MAP_FILENAME, putting its entries into
   MAP. Comments begin with % and continue to the end of the line.  Each
   line of the file defines an entry: the first word is the real
   filename (e.g., `ptmr'), the second word is the alias (e.g.,
   `Times-Roman'), and any subsequent words are ignored.  .tfm is added
   if either the filename or the alias have no extension.  This is the
   same order as in Dvips' psfonts.map.  Perhaps someday the programs
   will both read the same file.  */

static void
map_file_parse (kpathsea kpse, const_string map_filename)
{
  char *orig_l;
  unsigned map_lineno = 0;
  FILE *f = xfopen (map_filename, FOPEN_R_MODE);

  if (kpse->record_input)
    kpse->record_input (map_filename);

  while ((orig_l = read_line (f)) != NULL) {
    string filename;
    string l = orig_l; /* save for free() */
    string comment_loc = strrchr (l, '%');
    if (!comment_loc) {
      comment_loc = strstr (l, "@c");
    }

    /* Ignore anything after a % or @c.  */
    if (comment_loc)
      *comment_loc = 0;

    map_lineno++;

    /* Skip leading whitespace so we can use strlen below.  Can't use
       strtok since this routine is recursive.  */
    while (*l && ISSPACE (*l))
      l++;

    /* If we don't have any filename, that's ok, the line is blank.  */
    filename = token (l);
    if (filename) {
      string alias = token (l + strlen (filename));

      if (STREQ (filename, "include")) {
        if (alias == NULL) {
  WARNING2 ("kpathsea: %s:%u: Filename argument for include directive missing",
                    map_filename, map_lineno);
        } else {
          string include_fname = kpathsea_path_search (kpse,
                                   kpse->map_path, alias, false);
          if (include_fname) {
            map_file_parse (kpse, include_fname);
            if (include_fname != alias)
              free (include_fname);
          } else {
            WARNING3 ("kpathsea: %s:%u: Can't find fontname include file `%s'",
                      map_filename, map_lineno, alias);
          }
          free (alias);
          free (filename);
        }

      /* But if we have a filename and no alias, something's wrong.  */
      } else if (alias == NULL) {
        WARNING3 ("kpathsea: %s:%u: Fontname alias missing for filename `%s'",
                  map_filename, map_lineno, filename);
        free (filename);

      } else {
        /* We've got everything.  Insert the new entry.  They were
           already dynamically allocated by token(), so don't bother
           with xstrdup.  */
          hash_insert_normalized (&(kpse->map), alias, filename);
      }
    }

    free (orig_l);
  }

  xfclose (f, map_filename);
}

/* Parse the file MAP_NAME in each of the directories in PATH and
   return the resulting structure.  Entries in earlier files override
   later files.  */

static void
read_all_maps (kpathsea kpse)
{
  string *filenames;

  kpse->map_path = kpathsea_init_format (kpse, kpse_fontmap_format);
  filenames = kpathsea_all_path_search (kpse, kpse->map_path, MAP_NAME);

  kpse->map = hash_create (MAP_HASH_SIZE);

  while (*filenames) {
    map_file_parse (kpse, *filenames);
    filenames++;
  }
}

/* Look up KEY in texfonts.map's; if it's not found, remove any suffix
   from KEY and try again.  Create the map if necessary.  */

const_string *
kpathsea_fontmap_lookup (kpathsea kpse, const_string key)
{
  const_string *ret;
  const_string suffix = find_suffix (key);

  if (kpse->map.size == 0) {
    read_all_maps (kpse);
  }

  ret = hash_lookup (kpse->map, key);
  if (!ret) {
    /* OK, the original KEY didn't work.  Let's check for the KEY without
       an extension -- perhaps they gave foobar.tfm, but the mapping only
       defines `foobar'.  */
    if (suffix) {
      string base_key = remove_suffix (key);
      ret = hash_lookup (kpse->map, base_key);
      free (base_key);
    }
  }

  /* Append any original suffix.  */
  if (ret && suffix) {
    const_string *elt;
    for (elt = ret; *elt; elt++) {
      *elt = extend_filename (*elt, suffix);
    }
  }

  return ret;
}
