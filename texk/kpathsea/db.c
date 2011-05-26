/* db.c: an external database to avoid filesystem lookups.

   Copyright 1994, 1995, 1996, 1997, 2008, 2009, 2011 Karl Berry.
   Copyright 1997-2005 Olaf Weber.

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
#include <kpathsea/absolute.h>
#include <kpathsea/c-stat.h>
#include <kpathsea/c-fopen.h>
#include <kpathsea/c-pathch.h>
#include <kpathsea/db.h>
#include <kpathsea/hash.h>
#include <kpathsea/line.h>
#include <kpathsea/pathsearch.h>
#include <kpathsea/readable.h>
#include <kpathsea/str-list.h>
#include <kpathsea/tex-file.h>
#include <kpathsea/variable.h>

#ifndef DB_HASH_SIZE
/* Based on the size of 2008 texmf-dist/ls-R, about 62000 entries.  But
   we don't want to make it too big, since texmf/ls-R only has about
   1300 entries.  We should dynamically adapt the size.  */
#define DB_HASH_SIZE 32003
#endif
#ifndef DB_NAME
#define DB_NAME "ls-R"
#endif
#ifndef DB_NAME_LC
#define DB_NAME_LC "ls-r"
#endif

/* read ls-R only on WIN32 */
#ifdef WIN32
static const_string db_names[] = {
    DB_NAME,
    NULL
};
#else
static const_string db_names[] = {
    DB_NAME,
    DB_NAME_LC,
    NULL
};
#endif

#ifndef ALIAS_NAME
#define ALIAS_NAME "aliases"
#endif
#ifndef ALIAS_HASH_SIZE
#define ALIAS_HASH_SIZE 1009
#endif


/* If DIRNAME contains any element beginning with a `.' (that is more
   than just `./'), return true.  This is to allow ``hidden''
   directories -- ones that don't get searched.  */

static boolean
ignore_dir_p (const_string dirname)
{
  const_string dot_pos = dirname;

  while ((dot_pos = strchr (dot_pos + 1, '.'))) {
    /* If / before and no / after, skip it. */
    if (IS_DIR_SEP (dot_pos[-1]) && dot_pos[1] && !IS_DIR_SEP (dot_pos[1]))
      return true;
  }

  return false;
}

/* If no DB_FILENAME, return false (maybe they aren't using this feature).
   Otherwise, add entries from DB_FILENAME to TABLE, and return true.  */

static boolean
db_build (kpathsea kpse, hash_table_type *table,  const_string db_filename)
{
  string line;
  unsigned dir_count = 0, file_count = 0, ignore_dir_count = 0;
  unsigned len = strlen (db_filename) - sizeof (DB_NAME) + 1; /* Keep the /. */
  string top_dir = (string)xmalloc (len + 1);
  string cur_dir = NULL; /* First thing in ls-R might be a filename.  */
  FILE *db_file = fopen (db_filename, FOPEN_R_MODE);

  strncpy (top_dir, db_filename, len);
  top_dir[len] = 0;

  if (db_file) {
    while ((line = read_line (db_file)) != NULL) {
      len = strlen (line);

      /* A line like `/foo:' = new dir foo.  Allow both absolute (/...)
         and explicitly relative (./...) names here.  It's a kludge to
         pass in the directory name with the trailing : still attached,
         but it doesn't actually hurt.  */
      if (len > 0 && line[len - 1] == ':'
          && kpathsea_absolute_p (kpse, line, true)) {
        /* New directory line.  */
        if (!ignore_dir_p (line)) {
          /* If they gave a relative name, prepend full directory name now.  */
          line[len - 1] = DIR_SEP;
          /* Skip over leading `./', it confuses `match' and is just a
             waste of space, anyway.  This will lose on `../', but `match'
             won't work there, either, so it doesn't matter.  */
          cur_dir = *line == '.' ? concat (top_dir, line + 2) : xstrdup (line);
          dir_count++;
        } else {
          cur_dir = NULL;
          ignore_dir_count++;
        }

      /* Ignore blank, `.' and `..' lines.  */
      } else if (*line != 0 && cur_dir   /* a file line? */
                 && !(*line == '.'
                      && (line[1] == 0 || (line[1] == '.' && line[2] == 0))))
      {
        /* Make a new hash table entry with a key of `line' and a data
           of `cur_dir'.  An already-existing identical key is ok, since
           a file named `foo' can be in more than one directory.  Share
           `cur_dir' among all its files (and hence never free it).

           Note that we assume that all names in the ls-R file have already
           been case-smashed to lowercase where appropriate.
        */
        hash_insert_normalized (table, xstrdup (line), cur_dir);
        file_count++;

      } /* else ignore blank lines or top-level files
           or files in ignored directories*/

      free (line);
    }

    xfclose (db_file, db_filename);

    if (file_count == 0) {
      WARNING1 ("kpathsea: No usable entries in %s", db_filename);
      WARNING ("kpathsea: See the manual for how to generate ls-R");
      db_file = NULL;
    } else {
      str_list_add (&(kpse->db_dir_list), xstrdup (top_dir));
    }

#ifdef KPSE_DEBUG
    if (KPATHSEA_DEBUG_P (KPSE_DEBUG_HASH)) {
      /* Don't make this a debugging bit, since the output is so
         voluminous, and being able to specify -1 is too useful.
         Instead, let people who want it run the program under
         a debugger and change the variable that way.  */
      boolean hash_summary_only = true;

      DEBUGF4 ("%s: %u entries in %d directories (%d hidden).\n",
               db_filename, file_count, dir_count, ignore_dir_count);
      DEBUGF ("ls-R hash table:");
      hash_print (*table, hash_summary_only);
      fflush (stderr);
    }
#endif /* KPSE_DEBUG */
  }

  free (top_dir);

  return db_file != NULL;
}


/* Insert FNAME into the hash table.  This is for files that get built
   during a run.  We wouldn't want to reread all of ls-R, even if it got
   rebuilt.  */

void
kpathsea_db_insert (kpathsea kpse, const_string passed_fname)
{
  /* We might not have found ls-R, or even had occasion to look for it
     yet, so do nothing if we have no hash table.  */
  if (kpse->db.buckets) {
    const_string dir_part;
    string fname = xstrdup (passed_fname);
    string baseptr = fname + (xbasename (fname) - fname);
    const_string file_part = xstrdup (baseptr);

    *baseptr = '\0';  /* Chop off the filename.  */
    dir_part = fname; /* That leaves the dir, with the trailing /.  */

    /* Note that we do not assuse that these names have been normalized. */
    hash_insert (&(kpse->db), file_part, dir_part);
  }
}

#if defined(KPSE_COMPAT_API)
void
kpse_db_insert (const_string passed_fname)
{
  kpathsea_db_insert(kpse_def, passed_fname);
}
#endif

/* Return true if FILENAME could be in PATH_ELT, i.e., if the directory
   part of FILENAME matches PATH_ELT.  Have to consider // wildcards, but
   $ and ~ expansion have already been done.  */

static boolean
match (const_string filename,  const_string path_elt)
{
  const_string original_filename = filename;
  boolean matched = false;

  for (; *filename && *path_elt; filename++, path_elt++) {
    if (FILECHARCASEEQ (*filename, *path_elt)) /* normal character match */
      ;

    else if (IS_DIR_SEP (*path_elt)  /* at // */
             && original_filename < filename && IS_DIR_SEP (path_elt[-1])) {
      while (IS_DIR_SEP (*path_elt))
        path_elt++; /* get past second and any subsequent /'s */
      if (*path_elt == 0) {
        /* Trailing //, matches anything. We could make this part of the
           other case, but it seems pointless to do the extra work.  */
        matched = true;
        break;
      } else {
        /* Intermediate //, have to match rest of PATH_ELT.  */
        for (; !matched && *filename; filename++) {
          /* Try matching at each possible character.  */
          if (IS_DIR_SEP (filename[-1])
              && FILECHARCASEEQ (*filename, *path_elt))
            matched = match (filename, path_elt);
        }
        /* Prevent filename++ when *filename='\0'. */
        break;
      }
    }

    else /* normal character nonmatch, quit */
      break;
  }

  /* If we've reached the end of PATH_ELT, check that we're at the last
     component of FILENAME, we've matched.  */
  if (!matched && *path_elt == 0) {
    /* Probably PATH_ELT ended with `vf' or some such, and FILENAME ends
       with `vf/ptmr.vf'.  In that case, we'll be at a directory
       separator.  On the other hand, if PATH_ELT ended with a / (as in
       `vf/'), FILENAME being the same `vf/ptmr.vf', we'll be at the
       `p'.  Upshot: if we're at a dir separator in FILENAME, skip it.
       But if not, that's ok, as long as there are no more dir separators.  */
    if (IS_DIR_SEP (*filename))
      filename++;

    while (*filename && !IS_DIR_SEP (*filename))
      filename++;
    matched = *filename == 0;
  }

  return matched;
}


/* If DB_DIR is a prefix of PATH_ELT, return true; otherwise false.
   That is, the question is whether to try the db for a file looked up
   in PATH_ELT.  If PATH_ELT == ".", for example, the answer is no. If
   PATH_ELT == "/usr/local/lib/texmf/fonts//tfm", the answer is yes.

   In practice, ls-R is only needed for lengthy subdirectory
   comparisons, but there's no gain to checking PATH_ELT to see if it is
   a subdir match, since the only way to do that is to do a string
   search in it, which is all we do anyway.  */

static boolean
elt_in_db (const_string db_dir,  const_string path_elt)
{
  boolean found = false;

  while (!found && FILECHARCASEEQ (*db_dir++, *path_elt++)) {
    /* If we've matched the entire db directory, it's good.  */
    if (*db_dir == 0)
      found = true;

    /* If we've reached the end of PATH_ELT, but not the end of the db
       directory, it's no good.  */
    else if (*path_elt == 0)
      break;
  }

  return found;
}

/* If ALIAS_FILENAME exists, read it into TABLE.  */

static boolean
alias_build (kpathsea kpse, hash_table_type *table,
             const_string alias_filename)
{
  string line, real, alias;
  unsigned count = 0;
  FILE *alias_file = fopen (alias_filename, FOPEN_R_MODE);

  if (alias_file) {
    while ((line = read_line (alias_file)) != NULL) {
      /* comments or empty */
      if (*line == 0 || *line == '%' || *line == '#') {
        ;
      } else {
        /* Each line should have two fields: realname aliasname.  */
        real = line;
        while (*real && ISSPACE (*real))
          real++;
        alias = real;
        while (*alias && !ISSPACE (*alias))
          alias++;
        *alias++ = 0;
        while (*alias && ISSPACE (*alias))
          alias++;
        /* Is the check for errors strong enough?  Should we warn the user
           for potential errors?  */
        if (strlen (real) != 0 && strlen (alias) != 0) {
          /* Stuff in the alias file should be normalized. */
          hash_insert_normalized (table, xstrdup (alias), xstrdup (real));
          count++;
        }
      }
      free (line);
    }

#ifdef KPSE_DEBUG
    if (KPATHSEA_DEBUG_P (KPSE_DEBUG_HASH)) {
      /* As with ls-R above ... */
      boolean hash_summary_only = true;
      DEBUGF2 ("%s: %u aliases.\n", alias_filename, count);
      DEBUGF ("alias hash table:");
      hash_print (*table, hash_summary_only);
      fflush (stderr);
    }
#endif /* KPSE_DEBUG */

    xfclose (alias_file, alias_filename);
  }

  return alias_file != NULL;
}

/* Initialize the path for ls-R files, and read them all into the hash
   table `db'.  If no usable ls-R's found, set kpse->db.buckets to NULL.  */

void
kpathsea_init_db (kpathsea kpse)
{
  boolean ok = false;
  const_string db_path;
  string *db_files;
  string *orig_db_files;

  assert (sizeof(DB_NAME) == sizeof(DB_NAME_LC));

  db_path = kpathsea_init_format (kpse, kpse_db_format);
  db_files = kpathsea_all_path_search_list (kpse, db_path, db_names);
  orig_db_files = db_files;

  /* Must do this after the path searching (which ends up calling
    kpse_db_search recursively), so kpse->db.buckets stays NULL.  */
  kpse->db = hash_create (DB_HASH_SIZE);

  while (db_files && *db_files) {
      if (db_build (kpse, &(kpse->db), *db_files))
      ok = true;
    free (*db_files);
    db_files++;
  }

  if (!ok) {
    /* If db can't be built, leave `size' nonzero (so we don't
       rebuild it), but clear `buckets' (so we don't look in it).  */
    free (kpse->db.buckets);
    kpse->db.buckets = NULL;
  }

  free (orig_db_files);

  /* Add the content of any alias databases.  There may exist more than
     one alias file along DB_NAME files.  This duplicates the above code
     -- should be a function.  */
  ok = false;
  db_files = kpathsea_all_path_search (kpse, db_path, ALIAS_NAME);
  orig_db_files = db_files;

  kpse->alias_db = hash_create (ALIAS_HASH_SIZE);

  while (db_files && *db_files) {
      if (alias_build (kpse, &(kpse->alias_db), *db_files))
      ok = true;
    free (*db_files);
    db_files++;
  }

  if (!ok) {
    free (kpse->alias_db.buckets);
    kpse->alias_db.buckets = NULL;
  }

  free (orig_db_files);
}

#if defined(KPSE_COMPAT_API)
void
kpse_init_db (void)
{
  kpathsea_init_db(kpse_def);
}
#endif


/* Avoid doing anything if this PATH_ELT is irrelevant to the databases. */
str_list_type *
kpathsea_db_search (kpathsea kpse, const_string name,
                    const_string orig_path_elt, boolean all)
{
  string *db_dirs, *orig_dirs;
  const_string last_slash, path_elt;
  string temp_str = NULL;
  boolean done;
  unsigned e;
  str_list_type *ret = NULL;
  const_string *aliases, *r;
  boolean relevant = false;

  /* If we failed to build the database (or if this is the recursive
     call to build the db path), quit.  */
  if (kpse->db.buckets == NULL)
    return NULL;

  /* When tex-glyph.c calls us looking for, e.g., dpi600/cmr10.pk, we
     won't find it unless we change NAME to just `cmr10.pk' and append
     `/dpi600' to PATH_ELT.  We are justified in using a literal `/'
     here, since that's what tex-glyph.c unconditionally uses in
     DPI_BITMAP_SPEC.  But don't do anything if the / begins NAME; that
     should never happen.  */
  last_slash = strrchr (name, '/');
  if (last_slash && last_slash != name) {
    unsigned len = last_slash - name + 1;
    string dir_part = (string)xmalloc (len);
    strncpy (dir_part, name, len - 1);
    dir_part[len - 1] = 0;
    path_elt = temp_str = concat3 (orig_path_elt, "/", dir_part);
    name = last_slash + 1;
    free (dir_part);
  } else
    path_elt = orig_path_elt;

  /* Don't bother doing any lookups if this `path_elt' isn't covered by
     any of database directories.  We do this not so much because the
     extra couple of hash lookups matter -- they don't -- but rather
     because we want to return NULL in this case, so path_search can
     know to do a disk search.  */
  for (e = 0; !relevant && e < STR_LIST_LENGTH (kpse->db_dir_list); e++) {
    relevant = elt_in_db (STR_LIST_ELT (kpse->db_dir_list, e), path_elt);
  }
  if (!relevant)
    return NULL;

  /* If we have aliases for this name, use them.  */
  if (kpse->alias_db.buckets)
    aliases = (const_string *) hash_lookup (kpse->alias_db, name);
  else
    aliases = NULL;

  if (!aliases) {
    aliases = XTALLOC1 (const_string);
    aliases[0] = NULL;
  }
  {  /* Push aliases up by one and insert the original name at the front.  */
    unsigned i;
    unsigned len = 1; /* Have NULL element already allocated.  */
    for (r = aliases; *r; r++)
      len++;
    XRETALLOC (aliases, len + 1, const_string);
    for (i = len; i > 0; i--) {
      aliases[i] = aliases[i - 1];
    }
    aliases[0] = name;
  }

  done = false;
  for (r = aliases; !done && *r; r++) {
    const_string ctry = *r;

    /* We have an ls-R db.  Look up `try'.  */
    orig_dirs = db_dirs = hash_lookup (kpse->db, ctry);

    ret = XTALLOC1 (str_list_type);
    *ret = str_list_init ();

    /* For each filename found, see if it matches the path element.  For
       example, if we have .../cx/cmr10.300pk and .../ricoh/cmr10.300pk,
       and the path looks like .../cx, we don't want the ricoh file.  */
    while (!done && db_dirs && *db_dirs) {
      string db_file = concat (*db_dirs, ctry);
      boolean matched = match (db_file, path_elt);

#ifdef KPSE_DEBUG
      if (KPATHSEA_DEBUG_P (KPSE_DEBUG_SEARCH))
        DEBUGF3 ("db:match(%s,%s) = %d\n", db_file, path_elt, matched);
#endif

      /* We got a hit in the database.  Now see if the file actually
         exists, possibly under an alias.  */
      if (matched) {
        string found = NULL;
        if (kpathsea_readable_file (kpse, db_file)) {
          found = db_file;

        } else {
          const_string *a;

          free (db_file); /* `db_file' wasn't on disk.  */

          /* The hit in the DB doesn't exist in disk.  Now try all its
             aliases.  For example, suppose we have a hierarchy on CD,
             thus `mf.bas', but ls-R contains `mf.base'.  Find it anyway.
             Could probably work around this with aliases, but
             this is pretty easy and shouldn't hurt.  The upshot is that
             if one of the aliases actually exists, we use that.  */
          for (a = aliases + 1; *a && !found; a++) {
            string atry = concat (*db_dirs, *a);
            if (kpathsea_readable_file (kpse, atry))
              found = atry;
            else
              free (atry);
          }
        }

        /* If we have a real file, add it to the list, maybe done.  */
        if (found) {
          str_list_add (ret, found);
          if (!all && found)
            done = true;
        }
      } else { /* no match in the db */
        free (db_file);
      }


      /* On to the next directory, if any.  */
      db_dirs++;
    }

    /* This is just the space for the pointers, not the strings.  */
    if (orig_dirs && *orig_dirs)
      free (orig_dirs);
  }

  free (aliases);

  /* If we had to break up NAME, free the TEMP_STR.  */
  if (temp_str)
    free (temp_str);

  return ret;
}

#if defined(KPSE_COMPAT_API)
str_list_type *
kpse_db_search (const_string name,  const_string orig_path_elt,
                boolean all)
{
    return kpathsea_db_search (kpse_def, name, orig_path_elt, all);
}
#endif

str_list_type *
kpathsea_db_search_list (kpathsea kpse, const_string* names,
                         const_string path_elt, boolean all)
{
  string *db_dirs, *orig_dirs;
  const_string last_slash, name, path;
  string temp_str = NULL;
  boolean done;
  unsigned e;
  const_string *aliases, *r;
  int n;
  str_list_type *ret = NULL;
  boolean relevant = false;

  /* If we failed to build the database (or if this is the recursive
     call to build the db path), quit.  */
  if (kpse->db.buckets == NULL)
    return NULL;

  /* Don't bother doing any lookups if this `path_elt' isn't covered by
     any of database directories.  We do this not so much because the
     extra couple of hash lookups matter -- they don't -- but rather
     because we want to return NULL in this case, so path_search can
     know to do a disk search.  */
  for (e = 0; !relevant && e < STR_LIST_LENGTH (kpse->db_dir_list); e++) {
    relevant = elt_in_db (STR_LIST_ELT (kpse->db_dir_list, e), path_elt);
  }
  if (!relevant)
    return NULL;

  done = false;
  ret = XTALLOC1 (str_list_type);
  *ret = str_list_init ();

  /* Handle each name. */
  for (n = 0; !done && names[n]; n++) {
      name = names[n];

      /* Absolute names should have been caught in our caller. */
      if (kpathsea_absolute_p(kpse, name, true))
          continue;

      /* When tex-glyph.c calls us looking for, e.g., dpi600/cmr10.pk, we
         won't find it unless we change NAME to just `cmr10.pk' and append
         `/dpi600' to PATH_ELT.  We are justified in using a literal `/'
         here, since that's what tex-glyph.c unconditionally uses in
         DPI_BITMAP_SPEC.  But don't do anything if the / begins NAME; that
         should never happen.  */
      last_slash = strrchr (name, '/');
      if (last_slash && last_slash != name) {
          unsigned len = last_slash - name + 1;
          string dir_part = (string)xmalloc (len);
          strncpy (dir_part, name, len - 1);
          dir_part[len - 1] = 0;
          path = temp_str = concat3 (path_elt, "/", dir_part);
          name = last_slash + 1;
          free (dir_part);
      } else {
          path = path_elt;
      }

      /* If we have aliases for this name, use them.  */
      if (kpse->alias_db.buckets)
          aliases = (const_string *) hash_lookup (kpse->alias_db, name);
      else
          aliases = NULL;

      if (!aliases) {
          aliases = XTALLOC1 (const_string);
          aliases[0] = NULL;
      }
      {  /* Push aliases up by one and insert the original name at front.  */
          unsigned i;
          unsigned len = 1; /* Have NULL element already allocated.  */
          for (r = aliases; *r; r++)
              len++;
          XRETALLOC (aliases, len + 1, const_string);
          for (i = len; i > 0; i--) {
              aliases[i] = aliases[i - 1];
          }
          aliases[0] = name;
      }

      for (r = aliases; !done && *r; r++) {
          const_string ctry = *r;

          /* We have an ls-R db.  Look up `try'.  */
          orig_dirs = db_dirs = hash_lookup (kpse->db, ctry);

          /* For each filename found, see if it matches the path element.  For
             example, if we have .../cx/cmr10.300pk and .../ricoh/cmr10.300pk,
             and the path looks like .../cx, we don't want the ricoh file.  */
          while (!done && db_dirs && *db_dirs) {
            string db_file = concat (*db_dirs, ctry);
            boolean matched = match (db_file, path);

#ifdef KPSE_DEBUG
            if (KPATHSEA_DEBUG_P (KPSE_DEBUG_SEARCH))
              DEBUGF3 ("db:match(%s,%s) = %d\n", db_file, path, matched);
#endif

            /* We got a hit in the database.  Now see if the file actually
               exists, possibly under an alias.  */
            if (matched) {
              string found = NULL;
              if (kpathsea_readable_file (kpse, db_file)) {
                found = db_file;

              } else {
                const_string *a;

                free (db_file); /* `db_file' wasn't on disk.  */

                /* The hit in the DB doesn't exist in disk.  Now try all its
                   aliases.  For example, suppose we have a hierarchy on CD,
                   thus `mf.bas', but ls-R contains `mf.base'.  Find it anyway.
                   Could probably work around this with aliases, but
                   this is pretty easy and shouldn't hurt.  The upshot is that
                   if one of the aliases actually exists, we use that.  */
                for (a = aliases + 1; *a && !found; a++) {
                  string atry = concat (*db_dirs, *a);
                  if (kpathsea_readable_file (kpse, atry))
                    found = atry;
                  else
                    free (atry);
                }
              }

              /* If we have a real file, add it to the list, maybe done.  */
              if (found) {
                str_list_add (ret, found);
                if (!all && found)
                  done = true;
              }
            } else { /* no match in the db */
              free (db_file);
            }

            /* On to the next directory, if any.  */
            db_dirs++;
          }

          /* This is just the space for the pointers, not the strings.  */
          if (orig_dirs && *orig_dirs)
              free (orig_dirs);
      }

      free (aliases);
      if (temp_str)
          free (temp_str);
  }

  return ret;
}

#if defined(KPSE_COMPAT_API)
str_list_type *
kpse_db_search_list (const_string* names,  const_string path_elt,
                     boolean all)
{
    return kpathsea_db_search_list (kpse_def, names, path_elt, all);
}
#endif
