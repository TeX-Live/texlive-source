/* pathsearch.c: look up a filename in a path.

   Copyright 1993, 1994, 1995, 1997, 2007, 2009-2012, 2018 Karl Berry.
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
#include <kpathsea/c-pathch.h>
#include <kpathsea/c-fopen.h>
#include <kpathsea/absolute.h>
#include <kpathsea/expand.h>
#include <kpathsea/db.h>
#include <kpathsea/pathsearch.h>
#include <kpathsea/readable.h>
#include <kpathsea/str-list.h>
#include <kpathsea/str-llist.h>
#include <kpathsea/variable.h>
#include <kpathsea/xopendir.h>

#include <time.h> /* for `time' */

#ifdef __DJGPP__
#include <sys/stat.h>   /* for stat bits */
#endif

#ifdef WIN32
#undef fputs
#undef puts
#define fputs win32_fputs
#define puts  win32_puts
#endif

/* The very first search is for texmf.cnf, called when someone tries to
   initialize the TFM path or whatever.  init_path calls kpse_cnf_get
   which calls kpse_all_path_search to find all the texmf.cnf's.  We
   need to do various special things in this case, since we obviously
   don't yet have the configuration files when we're searching for the
   configuration files.  Therefore we have a followup_search member in
   kpathsea_instance to distinguish the first search from all others.  */



#ifdef KPSE_DEBUG
/* Print on FH elements of L surrounded by brackets, separated by spaces.  */

static void
print_space_list (FILE *fh, string *l)
{
  fputs ("[", fh);
  while (l && *l) {
    fputs (*l, fh);
    l++;
    if (*l)
      fputs (" ", fh);
  }
  fputs ("]", fh);
}
#endif /* KPSE_DEBUG */

/* This function is called after every search (except the first, since
   we definitely want to allow enabling the logging in texmf.cnf) to
   record the filename(s) found in $TEXMFLOG.  */

static void
log_search (kpathsea kpse, str_list_type filenames)
{
  if (kpse->log_opened == false) {
    /* Get name from either envvar or config file.  Thus, the first time
       is called, for the first search, we will be opening texmf.cnf
       and ls-R just to resolve the variable.  */
    string log_name = kpathsea_var_value (kpse, "TEXMFLOG");
    kpse->log_opened = true;
    if (log_name) {
      kpse->log_file = fopen (log_name, FOPEN_A_MODE);
      if (!kpse->log_file)
        perror (log_name);
      free (log_name);
    }
  }

  if (
#ifdef KPSE_DEBUG
      KPATHSEA_DEBUG_P (KPSE_DEBUG_SEARCH) ||
#endif /* KPSE_DEBUG */
      kpse->log_file) {
    unsigned e;

    /* FILENAMES should never be null, but safety doesn't hurt.  */
    for (e = 0; e < STR_LIST_LENGTH (filenames) && STR_LIST_ELT (filenames, e);
         e++) {
      string filename = STR_LIST_ELT (filenames, e);

      /* Only record absolute filenames, for privacy.  */
      if (kpse->log_file && kpathsea_absolute_p (kpse, filename, false)) {
        fprintf (kpse->log_file, "%lu %s\n", (long unsigned) time (NULL),
                 filename);
      }

#ifdef KPSE_DEBUG
      /* And show them online, if debugging.  We've already started the
         debugging line in `search' and
         `kpathsea_path_search_list_generic', where this is called, so
         just print the filename here, don't use DEBUGF.  */
      if (KPATHSEA_DEBUG_P (KPSE_DEBUG_SEARCH)) {
        putc (' ', stderr);
        fputs (filename, stderr);
      }
#endif /* KPSE_DEBUG */
    }
  }
}

/* Either casefold_readable_file or kpathsea_readable_file.  We need the
   type even if we don't have casefolding enabled at compile-time.  */
typedef string (*readable_file_fn_type) (kpathsea, string);

#ifdef MONOCASE_FILENAMES
/* Don't do any of this fallback casefolding stuff on Windows.  */
#undef KPSE_CASEFOLDING_SEARCH
#else
#define KPSE_CASEFOLDING_SEARCH 1
#endif

#ifdef KPSE_CASEFOLDING_SEARCH

/* Same as kpathsea_readable_file (readable.c), but check
   case-insensitively on the last file name component.  We return the
   first match (in new memory), or NULL.  In practice this is always
   called after checking the filename as-is, so there's no use in
   continuing to look for an exact match.  The KPSE arg is only passed
   to kpathsea_readable_file if we have a candidate match.
   
   Clearly we could do some caching here, but let's see if it's actually
   necessary, given all the other levels of caching (disk, memory, cpu)
   that are going on these days.  */

static string
casefold_readable_file (kpathsea kpse, string name)
{
  string ret = NULL;
  const_string this_base_name = xbasename (name);
  string this_dir_name = xdirname (name);
  DIR *thisdir = opendir (this_dir_name);
  
#ifdef KPSE_DEBUG
  if (KPATHSEA_DEBUG_P (KPSE_DEBUG_SEARCH)) {
    DEBUGF2 ("   casefold_readable_file(%s) in %s => ",
             this_base_name, this_dir_name);
  }
#endif

  /* We can be called with a name like `subdir/somefile', where subdir/
     does not exist.  So the opendir might fail, and that's ok.  */
  if (thisdir) {
    struct dirent *e;

    while ((e = readdir (thisdir)) != NULL) {
      /* The standard strcasecmp seems like enough for the comparison?  */
      if (strcasecmp (e->d_name, this_base_name) == 0) {
        ret = concat3 (this_dir_name, DIR_SEP_STRING, e->d_name);
        if (kpathsea_readable_file (kpse, ret)) {
          break; /* success */
        } else {
          /* This only happens the name matches, but the potential file is
             not actually readable, e.g., a broken symlink.  It seems
             sufficiently unusual to be worth logging.  */
  #ifdef KPSE_DEBUG
          if (KPATHSEA_DEBUG_P (KPSE_DEBUG_SEARCH)) {
            fprintf (stderr, "{casefolded candidate %s not readable, continuing}",ret);
          }
  #endif
          free (ret); /* not readable, keep looking */
          ret = NULL;
        }
      }
    } /* end of readdir loop */
    xclosedir (thisdir);
  }
  free (this_dir_name);
  
  if (KPATHSEA_DEBUG_P (KPSE_DEBUG_SEARCH)) {
    fputs (ret ? ret : "(nil)", stderr);
    fputc ('\n', stderr);    
  }
  return ret;
}
#endif /* KPSE_CASEFOLDING_SEARCH */

/* Return a str_list of matches in DIRS of NAME.

   Concatenate each element in DIRS with NAME, assuming each ends with /.
   If SEARCH_ALL is false, return a list containing just the first
   readable (according to the READABLE_FILE_P function) regular file.
   Else continue to search and return them all.  In any case, if none,
   return a list containing just NULL.
   
   DIRS is modified only in that the directory where a search matches is
   floated toward the top of the list.

   We keep a single buffer for the potential filenames and reallocate
   only when necessary.  I'm not sure it's noticeably faster, but it
   does seem cleaner.  (We do waste a bit of space in the return
   value, since we don't shrink it to the final size returned.)  */

#define INIT_ALLOC 75  /* Doesn't much matter what this number is.  */

static str_list_type
dir_list_search (kpathsea kpse, str_llist_type *dirs, const_string name,
                 boolean search_all, readable_file_fn_type readable_file_p)
{
  str_llist_elt_type *elt;
  str_llist_elt_type *next_elt;
  str_list_type ret;
  unsigned name_len = strlen (name);
  unsigned allocated = INIT_ALLOC;
  string potential = (string) xmalloc (allocated);

#ifdef KPSE_DEBUG
  if (KPATHSEA_DEBUG_P (KPSE_DEBUG_SEARCH)) {
    const_string casefold = 
#ifdef KPSE_CASEFOLDING_SEARCH
      (readable_file_p == casefold_readable_file) ? "yes" : "no";
#else
      "No";
#endif
    DEBUGF3 ("  dir_list_search(file=%s, find_all=%d, casefold=%s)\n",
             name, search_all, casefold);
  }
#endif

  ret = str_list_init ();

  for (elt = *dirs; elt; elt = next_elt) {
    string readable_name;
    const_string dir = STR_LLIST (*elt);
    unsigned dir_len = strlen (dir);

    next_elt = STR_LLIST_NEXT (*elt); /* in case elt floats */

    while (dir_len + name_len + 1 > allocated) {
      allocated += allocated;
      XRETALLOC (potential, allocated, char);
    }
    strcpy (potential, dir);
    strcat (potential, name);

    readable_name = readable_file_p (kpse, potential);
    if (readable_name) {
      str_list_add (&ret, readable_name);

      /* Move this element towards the top of the list.  */
      str_llist_float (dirs, elt);

      /* If caller only wanted one file returned, no need to
         terminate the list with NULL; the caller knows to only look
         at the first element.  */
      if (!search_all)
        return ret;

      /* Start new filename.  */
      allocated = INIT_ALLOC;
      potential = (string) xmalloc (allocated);
    }
  }

  /* If we get here, either we didn't find any files, or we were finding
     all the files.  But we're done with the last filename, anyway.  */
  free (potential);

  return ret;
}

/* This is analogous to dir_list_search above, except we search for
   multiple NAMES instead of one (unfortunately the code is duplicated).
   Absolute or explicitly relative items in NAMES are ignored; the
   caller (kpathsea_path_search_list_generic) deals with those
   separately.  NAMES[i] is not modified.  */
   

static str_list_type
dir_list_search_list (kpathsea kpse, str_llist_type *dirs, string* names,
                     boolean search_all, readable_file_fn_type readable_file_p)
{
  str_llist_elt_type *elt;
  str_llist_elt_type *next_elt;
  str_list_type ret;
  unsigned allocated = INIT_ALLOC;
  string potential = XTALLOC (allocated, char);

#ifdef KPSE_DEBUG
  if (KPATHSEA_DEBUG_P (KPSE_DEBUG_SEARCH)) {
    const_string casefold =
#ifdef KPSE_CASEFOLDING_SEARCH
      (readable_file_p == casefold_readable_file) ? "yes" : "no";
#else
      "No";
#endif
    DEBUGF ("  dir_list_search_list(files=");
    print_space_list (stderr, names);
    fprintf (stderr, ", find_all=%d, casefold=%s)\n", search_all, casefold);
  }
#endif

  ret = str_list_init ();

  for (elt = *dirs; elt; elt = next_elt) {
    int i;
    string readable_name;
    const_string dir = STR_LLIST (*elt);
    unsigned dir_len = strlen (dir);

    next_elt = STR_LLIST_NEXT (*elt); /* in case elt floats */

    for (i = 0; names[i]; i++) {
      const_string name = names[i];
      unsigned name_len;

      /* Don't bother with absolute & explicit relative. */
      if (kpathsea_absolute_p (kpse, name, true))
        continue;

      name_len = strlen (name);

      while (dir_len + name_len + 1 > allocated) {
        allocated += allocated;
        XRETALLOC (potential, allocated, char);
      }

      strcpy (potential, dir);
      strcat (potential + dir_len, name);
      readable_name = readable_file_p (kpse, potential);
      if (readable_name) {
        str_list_add (&ret, readable_name);

        /* Move this element towards the top of the list.  */
        str_llist_float (dirs, elt);

        /* If caller only wanted one file returned, no need to
           terminate the list with NULL; the caller knows to only look
           at the first element.  */
        if (!search_all)
          return ret;

        /* Start new filename. */
        allocated = INIT_ALLOC;
        potential = XTALLOC (allocated, char);
      }
    }
  }

  /* If we get here, either we didn't find any files, or we were finding
     all the files.  But we're done with the last filename, anyway.  */
  free (potential);

  return ret;
}

/* This is called when NAME is absolute or explicitly relative; if it's
   readable, return a one-element str_list containing it (in new
   memory); otherwise, return an empty list.  (We return a list so as to
   have the same return value as the path_search function.)  We also
   check case-insensitively if enabled and needed.  */

static str_list_type
absolute_search (kpathsea kpse, string name)
{
  str_list_type ret_list;
  string found;

  /* Some old compilers can't initialize structs.  */
  ret_list = str_list_init ();

  /* Do the first check.  */
  found = kpathsea_readable_file (kpse, name);
#ifdef KPSE_DEBUG
  if (KPATHSEA_DEBUG_P (KPSE_DEBUG_SEARCH)) {
    DEBUGF2 (" absolute_search(%s) => %s\n", name, found ? found : "(nil)");
  }
#endif
  if (found) {
    found = xstrdup (found); /* Return new memory.  */
  }

#ifdef KPSE_CASEFOLDING_SEARCH
  /* Do the casefolding search only if both needed and enabled. */
  if (!found) {
    if (KPSE_CNF_P (kpathsea_var_value (kpse, "texmf_casefold_search"))) {
      found = casefold_readable_file (kpse, name);
#ifdef KPSE_DEBUG
      if (KPATHSEA_DEBUG_P (KPSE_DEBUG_SEARCH)) {
        DEBUGF2 ("  casefold search(%s) => %s\n",name,found ? found : "(nil)");
      }
#endif /* KPSE_DEBUG */
    }
  }
#endif /* KPSE_CASEFOLDING_SEARCH */

  if (found) {
    /* If we didn't find anything, we'll return an empty list.  */
    str_list_add (&ret_list, found);
  }

  return ret_list;
}

/* This is the hard case -- look for NAME in PATH.  If ALL is false,
   return the first file found.  Otherwise, search all elements of PATH.
   We also check case-insensitively if needed and requested.  */

static str_list_type
path_search (kpathsea kpse, const_string path, string name,
             boolean must_exist, boolean all)
{
  string elt;
  str_list_type ret_list;
  boolean done = false;
  ret_list = str_list_init (); /* some compilers lack struct initialization */

#ifdef KPSE_DEBUG
  if (KPATHSEA_DEBUG_P (KPSE_DEBUG_SEARCH)) {
    DEBUGF4 (" path_search(file=%s, must_exist=%d, find_all=%d, path=%s)\n",
             name, all, must_exist, path);
  }
#endif

  for (elt = kpathsea_path_element (kpse, path); !done && elt;
       elt = kpathsea_path_element (kpse, NULL)) {
    str_list_type *found;
    boolean allow_disk_search = true;

    if (*elt == '!' && *(elt + 1) == '!') {
      /* Those magic leading chars in a path element means don't search the
         disk for this elt.  And move past the magic to get to the name.  */
      allow_disk_search = false;
      elt += 2;
    }

    /* See elt-dirs.c for side effects of this function.  */
    kpathsea_normalize_path (kpse, elt);

    /* Try ls-R, unless we're searching for texmf.cnf.  Our caller
       (search), also tests followup_search, and does the resetting.  */
    found = kpse->followup_search ? kpathsea_db_search (kpse, name, elt, all)
                                  : NULL;

    /* Search the filesystem if (1) the path spec allows it, and either
         (2a) we are searching for texmf.cnf; or
         (2b) no db exists; or
         (2c) no db's are relevant to this elt; or
         (3) MUST_EXIST && NAME was not in the db.
       In (2*), `found' will be NULL.
       In (3),  `found' will be an empty list. */
    if (allow_disk_search
        && (!found || (must_exist && !STR_LIST (*found)))) {
      /* Determine the directories in which to search: */
      str_llist_type *dirs = kpathsea_element_dirs (kpse, elt);
      if (dirs && *dirs) {
        if (!found) {
          found = XTALLOC1 (str_list_type);
        }
        /* Search in the directories: */
        *found = dir_list_search (kpse, dirs, name, all,
                                  kpathsea_readable_file);
#ifdef KPSE_CASEFOLDING_SEARCH
        if (!STR_LIST (*found)
            && KPSE_CNF_P (kpathsea_var_value (kpse,
                                               "texmf_casefold_search"))) {
          /* Nothing found; search again, case-insensitively: */
          *found = dir_list_search (kpse, dirs, name, all,
                                    casefold_readable_file);
        }
#endif /* KPSE_CASEFOLDING_SEARCH */
      }
    }

    /* Did we find anything?  */
    if (found && STR_LIST (*found)) {
      if (all) {
        str_list_concat (&ret_list, *found);
      } else {
        str_list_add (&ret_list, STR_LIST_FIRST_ELT (*found));
        done = true;
      }
    }

    /* Free the list space, if any (but not the elements).  */
    if (found) {
      str_list_free (found);
      free (found);
    }
  }

  return ret_list;
}

/* Search PATH for ORIGINAL_NAME.  If ALL is false, or ORIGINAL_NAME is
   absolute_p, check ORIGINAL_NAME itself.  Otherwise, look at each
   element of PATH for the first readable ORIGINAL_NAME.

   Always return a list; if no files are found, the list will
   contain just NULL.  If ALL is true, the list will be
   terminated with NULL.
   
   This function is a special case of kpathsea_path_search_list_generic
   below (which takes a list of names, instead of a single name, to
   search for), and so should be rewritten to call it.  But this
   function came first, and life is short, so the code duplication
   is here.  Sorry.  Please fix.  */

static string *
search (kpathsea kpse, const_string path,  const_string original_name,
        boolean must_exist,  boolean all)
{
  str_list_type ret_list;
  string name;
  boolean absolute_p;
#ifdef __DJGPP__
  /* We will use `stat' heavily, so let's request for
     the fastest possible version of `stat', by telling
     it what members of struct stat do we really need.

     We need to set this on each call because this is a
     library function; the caller might need other options
     from `stat'.  Thus save the flags and restore them
     before exit.

     This call tells `stat' that we do NOT need to recognize
     executable files (neither by an extension nor by a magic
     signature); that we do NOT need time stamp of root directories;
     and that we do NOT need the write access bit in st_mode.

     Note that `kpse_set_program_name' needs the EXEC bits,
     but it was already called by the time we get here.  */
  unsigned short save_djgpp_flags  = _djstat_flags;

  _djstat_flags = _STAT_EXEC_MAGIC | _STAT_EXEC_EXT
                  | _STAT_ROOT_TIME | _STAT_WRITEBIT;
#endif

  /* Make a leading ~ count as an absolute filename, and expand $FOO's.  */
  name = kpathsea_expand (kpse, original_name);

#ifdef KPSE_DEBUG
  if (KPATHSEA_DEBUG_P (KPSE_DEBUG_SEARCH))
    DEBUGF4 ("start search(xname=%s, must_exist=%d, find_all=%d, path=%s).\n",
             name, must_exist, all, path);
#endif /* KPSE_DEBUG */

  /* If the first name is absolute or explicitly relative, no need to
     consider PATH at all.  */
  absolute_p = kpathsea_absolute_p (kpse, name, true);

  /* Find the file(s). */
  ret_list = absolute_p ? absolute_search (kpse, name)
                        : path_search (kpse, path, name, must_exist, all);

  /* Append NULL terminator if we didn't find anything at all, or we're
     supposed to find ALL and the list doesn't end in NULL now.  */
  if (STR_LIST_EMPTY (ret_list)
      || (all && STR_LIST_LAST_ELT (ret_list) != NULL))
    str_list_add (&ret_list, NULL);

  /* The very first search is for texmf.cnf.  We can't log that, since
     we want to allow setting TEXMFLOG in texmf.cnf.  */
  if (kpse->followup_search == false) {
    kpse->followup_search = true;
  } else {
    /* Record the filenames we found, if desired.  And wrap them in a
       debugging line if we're doing that.  */
#ifdef KPSE_DEBUG
    if (KPATHSEA_DEBUG_P (KPSE_DEBUG_SEARCH))
      DEBUGF1 ("returning from search(%s) =>", original_name);
#endif /* KPSE_DEBUG */
    log_search (kpse, ret_list);
#ifdef KPSE_DEBUG
    if (KPATHSEA_DEBUG_P (KPSE_DEBUG_SEARCH))
      putc ('\n', stderr);
#endif /* KPSE_DEBUG */
  }

#ifdef __DJGPP__
  /* Undo any side effects.  */
  _djstat_flags = save_djgpp_flags;
#endif

  /* Free the expanded name we were passed.  It can't be in the return
     list, since the path directories got unconditionally prepended.  */
  free (name);

  return STR_LIST (ret_list);
}

/* Search PATH for null-terminated array of NAMES. Always return a list;
   if no files are found, the list will contain just NULL.  If ALL is
   true, the list will be terminated with NULL (but no NULL terminator
   if ALL is false).  This is a generalization of the `search' fn above.  */

string *
kpathsea_path_search_list_generic (kpathsea kpse,
                                   const_string path, string* names,
                                   boolean must_exist, boolean all)
{
  str_list_type ret_list;
  string* namep;
  string elt;
  boolean done = false;
  boolean all_absolute = true;
#ifdef __DJGPP__
  /* See DJGPP comments above.  */
  unsigned short save_djgpp_flags  = _djstat_flags;

  _djstat_flags = _STAT_EXEC_MAGIC | _STAT_EXEC_EXT
                  | _STAT_ROOT_TIME | _STAT_WRITEBIT;
#endif

  ret_list = str_list_init ();

#ifdef KPSE_DEBUG
  if (KPATHSEA_DEBUG_P (KPSE_DEBUG_SEARCH)) {
    DEBUGF ("start generic search(files=");
    print_space_list (stderr, names);
    fprintf (stderr, ", must_exist=%d, find_all=%d, path=%s)\n",
             must_exist, all, path);
  }
#endif /* KPSE_DEBUG */

  /* kpathsea_find_file_generic in tex-file.c does the variable and
     tilde expansion, so don't redo that here. Maybe we should have
     done it differently, but we certainly don't want to create an
     incompatibility now.  */

  /* First catch any absolute or explicit relative names. */
  for (namep = names; *namep; namep++) {
    if (kpathsea_absolute_p (kpse, *namep, true)) {
      str_list_type abs_ret_list = absolute_search (kpse, *namep);
      /* That search can only return a zero- or one-element list, so: */
      if (!STR_LIST_EMPTY (abs_ret_list)) {
        str_list_add (&ret_list, STR_LIST_FIRST_ELT (abs_ret_list));
        if (!all) { /* if they only wanted one, we're done */
          goto out;
        }
      }
    } else {
      all_absolute = false;
    }
  }
  /* Shortcut: if we were only given absolute/explicit relative names,
     we can skip the rest.  Typically, if one name is absolute, they
     all are, because our caller derived them from each other. */
  if (all_absolute) {
#ifdef KPSE_DEBUG
  if (KPATHSEA_DEBUG_P (KPSE_DEBUG_SEARCH)) {
    unsigned i;
    DEBUGF (" generic search: all absolute, candidates are:");
    /* List might not be NULL-terminated, so can't use print_space_list.  */
    for (i = 0; i < STR_LIST_LENGTH (ret_list); i++) {
      fprintf (stderr, " %s", STR_LIST_ELT (ret_list, i));
    }
    fputs (".\n", stderr);
  }
#endif
    goto out;
  }

  /* Look at each path element in turn.  This is essentially the same
     code as `path_search' above, unfortunately.  */
  for (elt = kpathsea_path_element (kpse, path); !done && elt;
       elt = kpathsea_path_element (kpse, NULL)) {
    str_list_type *found;
    boolean allow_disk_search = true;

    if (elt[0] == '!' && elt[1] == '!') {
      /* !! magic string -> disallow disk searches.  */
      allow_disk_search = false;
      elt += 2;
    }

    /* See elt-dirs.c for side effects of this function.  */
    kpathsea_normalize_path (kpse, elt);

    /* Try ls-R, unless we're searching for texmf.cnf.  */
    found = kpse->followup_search
            ? kpathsea_db_search_list (kpse, names, elt, all) : NULL;

    /* Search the filesystem in the same cases as `path_search' above.  */
    if (allow_disk_search
        && (!found || (must_exist && !STR_LIST (*found)))) {
      str_llist_type *dirs = kpathsea_element_dirs (kpse, elt);
      if (dirs && *dirs) {
        if (!found) {
          found = XTALLOC1 (str_list_type);
        }
        /* Search in the directories: */
        *found = dir_list_search_list (kpse, dirs, names, all,
                                      kpathsea_readable_file);
#ifdef KPSE_CASEFOLDING_SEARCH
        if (!STR_LIST (*found) && KPSE_CNF_P (kpathsea_var_value (kpse,
                                                   "texmf_casefold_search"))) {
          /* Still nothing; search again, case-insensitively: */
          *found = dir_list_search_list (kpse, dirs, names, all,
                                         casefold_readable_file);
        }
#endif
      }
    }

    /* Did we find anything? */
    if (found && STR_LIST (*found)) {
      if (all) {
        str_list_concat (&ret_list, *found);
      } else {
        str_list_add (&ret_list, STR_LIST_FIRST_ELT (*found));
        done = true;
      }
    }
  }

 out:
  /* Uniqify, since our paths can often end up finding the same file
     more than once.  */
  str_list_uniqify (&ret_list);

  /* Add NULL element to terminate return list if empty or multiple.  */
  if (STR_LIST_EMPTY (ret_list)
      || (all && STR_LIST_LAST_ELT (ret_list) != NULL))
    str_list_add (&ret_list, NULL);

  if (kpse->followup_search == false) {
    kpse->followup_search = true;
  } else {
    /* Record the filenames we found, if desired.  And wrap them in a
       debugging line if we're doing that.  */
#ifdef KPSE_DEBUG
    if (KPATHSEA_DEBUG_P (KPSE_DEBUG_SEARCH)) {
      DEBUGF ("returning from generic search(");
      print_space_list (stderr, names);
      fputs (") =>", stderr);
    }
#endif /* KPSE_DEBUG */
    log_search (kpse, ret_list);
#ifdef KPSE_DEBUG
    if (KPATHSEA_DEBUG_P (KPSE_DEBUG_SEARCH))
      putc ('\n', stderr);
#endif /* KPSE_DEBUG */
  }

#ifdef __DJGPP__
  /* Undo any side effects.  */
  _djstat_flags = save_djgpp_flags;
#endif

  return STR_LIST (ret_list);
}

/* Search PATH for the first NAME according to MUST_EXIST.  */

string
kpathsea_path_search (kpathsea kpse, const_string path, const_string name,
                      boolean must_exist)
{
  string *ret_list = search (kpse, path, name, must_exist, false);
  string ret = *ret_list;
  free (ret_list);
  return ret;
}

/* Search PATH for all files named NAME.  Might have been better not
   to assert `must_exist' here, but it's too late to change.  */

string *
kpathsea_all_path_search (kpathsea kpse, const_string path, const_string name)
{
  string *ret = search (kpse, path, name, true, true);
  return ret;
}

#if defined (KPSE_COMPAT_API)
string
kpse_path_search (const_string path, const_string name, boolean must_exist)
{
    return kpathsea_path_search (kpse_def, path, name, must_exist);
}

string *
kpse_all_path_search (const_string path, const_string name)
{
    return kpathsea_all_path_search (kpse_def, path, name);
}
#endif /* KPSE_COMPAT_API */


#ifdef TEST

/* Each element of L on its own line, prefixed by a tab.  */

static void
print_tab_list (string *l)
{
  while (l && *l) {
    if (*l)
      putchar ('\t');
    printf ("%s\n", *l);
    l++;
  }
}

static void
test_path_search (const_string path, const_string file)
{
  string answer;
  string *answer_list;

  printf ("\nSearch %s for %s:\t", path, file);
  answer = kpse_path_search (path, file, 0);
  puts (answer ? answer : "(nil)");

  printf ("Search %s for all %s:\t", path, file);
  answer_list = kpse_all_path_search (path, file);
  putchar ('\n');
  print_tab_list (answer_list);
}

static void
test_path_search_list_generic (void)
{
  const_string path = "/u/karl/.fonts";
  /* absolute: should return just console/both, because no case
     sensitive match */
  // string names[] = { "/u/karl/.fonts/lucidaConsoleDK.otf",
  //                    "/u/karl/.fonts/lucidaGrandeMonoDK.otf", NULL }; 

  /* dirs: should return just grande, because case sensitive wins */
  // string names[] = { "lucidaConsoleDK.otf", "LucidaGrandeMonoDK.otf", NULL };
  
  /* dirs: should return just console/both, because no case sensitive match */
  string names[] = { "lucidaConsoleDK.otf", "lucidaGrandeMonoDK.otf", NULL };

  boolean all;
  string *answer_list;

  printf ("\nGeneric search %s for ", path);
  print_space_list (stdout, names);
  puts (":\t");
  
  all = false; answer_list
    = kpathsea_path_search_list_generic (kpse_def, path, names, false, all);
  puts (answer_list && *answer_list ? *answer_list : "(nil)");

  printf ("\nGeneric search %s for all ", path);
  print_space_list (stdout, names);
  puts (":");
  
  all = true; answer_list
    = kpathsea_path_search_list_generic (kpse_def, path, names, false, all);
  print_tab_list (answer_list);
}

#define TEXFONTS "/usr/local/texlive-rel/texmf-dist/fonts"

int
main (int argc, char **argv)
{
  xputenv ("KPATHSEA_DEBUG", "-1"); /* must be before setting progname */
  kpse_set_program_name (argv[0], NULL);

  xputenv ("MALLOC_CHECK_", "3");
  xputenv ("MALLOC_PERTURB_", "75");

  xputenv ("texmf_casefold_search", "1");
  test_path_search (".:/k", "readme");
  exit (0);
  test_path_search_list_generic ();
  exit (0);
  
  xputenv ("TEXMFCNF", "/nc");
  /* casefolding with absolute search: */
  test_path_search ("/k", "/u/karl/.fonts/lucidaConsoleDK.otf");
  /* casefolding with directory search: */
  test_path_search ("/u/karl/.fonts", "Lucidaconsoledk.otf");
  /* exit (0); */
  xputenv ("texmf_casefold_search", "0");
  /* should fail since no casefolding: */
  test_path_search ("/u/karl/.fonts", "lucidaconsoledk.otf");

  /* All lists end with NULL.  */
  test_path_search (".", "nonexistent");
  test_path_search (".", "/nonexistent");
  test_path_search ("/k" ENV_SEP_STRING ".", "README");
  test_path_search ("/k" ENV_SEP_STRING ".", "/etc/fstab");
  test_path_search ("." ENV_SEP_STRING TEXFONTS "//", "cmr10.tfm");
  test_path_search ("." ENV_SEP_STRING TEXFONTS "//", "logo10.tfm");
  test_path_search (TEXFONTS "//times" ENV_SEP_STRING "."
                    ENV_SEP_STRING ENV_SEP_STRING, "ptmr.vf");
  test_path_search (TEXFONTS ENV_SEP_STRING
                    "/u/karl/.fonts", "LucidaConsoleDK.otf");

  test_path_search ("~karl", ".profile"); // nil, no ~ expansion on path
  test_path_search ("/k", "~karl/.profile");

  xputenv ("NONEXIST", "nonexistent");
  test_path_search (".", "$NONEXIST");
  xputenv ("KPATHSEA", "kpathsea");
  test_path_search ("/k" ENV_SEP_STRING "./doc", "$KPATHSEA.texi");
  test_path_search ("/k" ENV_SEP_STRING "./doc", "${KPATHSEA}.texi");
  test_path_search ("$KPATHSEA" ENV_SEP_STRING ".", "README");
  test_path_search ("." ENV_SEP_STRING "$KPATHSEA", "README");

  return 0;
}

#endif /* TEST */


/* ${wc} is the corresponding build directory.
   -DMAKE_KPSE_DLL for inlined str_list_init etc.
   Memory checking: -fsanitize=address or -lefence.
Local variables:
standalone-compile-command: "make --no-print-dir -C ${wc} && gcc -g -I. -I.. -I${wc}/.. -DMAKE_KPSE_DLL -DTEST pathsearch.c ${wc}/.libs/libkpathsea.a && ./a.out"
End:
*/
