/* cnf.c: read config files.

   Copyright 1994, 1995, 1996, 1997, 2008, 2009, 2011, 2012, 2016,
   2017, 2018, 2019 Karl Berry.
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
#include <kpathsea/c-fopen.h>
#include <kpathsea/c-ctype.h>
#include <kpathsea/c-pathch.h>
#include <kpathsea/cnf.h>
#include <kpathsea/db.h>
#include <kpathsea/hash.h>
#include <kpathsea/line.h>
#include <kpathsea/paths.h>
#include <kpathsea/pathsearch.h>
#include <kpathsea/tex-file.h>
#include <kpathsea/variable.h>

/* By using our own hash table, instead of the environment, we
   complicate variable expansion (because we have to look in two
   places), but we don't bang so much on the system.  DOS and System V
   have very limited environment space.  Also, this way
   `kpse_init_format' can distinguish between values originating from
   the cnf file and ones from environment variables, which can be useful
   for users trying to figure out what's going on.  */

#define CNF_HASH_SIZE 751
#define CNF_NAME "texmf.cnf"


/* Do a single line LINE in a cnf file: if it's blank or a comment or
   erroneous, skip it.  Otherwise, parse
     <variable>[.<program>] [=] <value>
   and insert it either into the cnf_hash structure in KPSE (if
   ENV_PROGNAME is false), or in the environment (if ENV_PROGNAME is true).
   
   Furthermore, if ENV_PROGNAME is true, and there is no .<program> in
   LINE, then also insert <variable> "_" `kpse->program_name' with <value>
   into the environment; see kpathsea_cnf_line_env_progname in cnf.h.

   The name without _<progname> will usually never be looked up, but
   just in case the program name changes, or whatever.
   
   We return NULL if ok, an error string otherwise.  */

static string
do_line (kpathsea kpse, string line, boolean env_progname)
{
  unsigned len;
  string start;
  string value, var;
  string prog = NULL;

  /* Skip leading whitespace.  */
  while (*line && ISSPACE (*line))
    line++;

  /* More to do only if we have non-comment material left.  */
  if (*line == 0 || *line == '%' || *line == '#')
    return NULL;

  /* Remove trailing comment: a % or # preceded by whitespace.  Also
     remove any whitespace before that.  For example, the value for
       foo = a#b  %something
     is a#b.  */
  value = line + strlen (line) - 1; /* start at end of line */
  while (value > line) {            
    if (*value == '%' || *value == '#') {
      value--;                      /* move before comment char */
      while (ISSPACE (*value))
        *value-- = 0;               /* wipe out as much preceding whitespace
      continue;                        (and comment) as we find */
    }
    value--;                        /* move before the new null byte */
  }

  /* The variable name is everything up to the next space or = or `.'.  */
  start = line;
  while (*line && !ISSPACE (*line) && *line != '=' && *line != '.')
    line++;

  /* `line' is now one character past the end of the variable name.  */
  len = line - start;
  if (len == 0) {
    return ("No cnf variable name");
  }
  
  var = (string) xmalloc (len + 1);
  strncpy (var, start, len);
  var[len] = 0;

  /* If the variable is qualified with a program name, extract it.  */
  while (*line && ISSPACE (*line))
    line++;
  if (*line == '.') {
    /* Skip spaces, then everything up to the next space or =.  */
    line++;
    while (ISSPACE (*line))
      line++;
    start = line;
    while (*line && !ISSPACE (*line) && *line != '=')
      line++;

    /* The program name is what's in between.  */
    len = line - start;
    prog = (string) xmalloc (len + 1);
    strncpy (prog, start, len);
    prog[len] = 0;
    /* If the name is empty, or contains one of our usual special
       characters, it's probably a mistake.  For instance, a cnf line
         foo .;bar
       is interpreted as a program name ";bar", because the = between
       the variable name and value is optional.  We don't try to guess
       the user's intentions, but just give a warning.  */
    if (len == 0) {
      return ("Empty program name qualifier");
    } else {
      unsigned i;
      for (i = 0; i < len; i++) {
        if (prog[i] == '$' || prog[i] == '{' || prog[i] == '}'
            || IS_KPSE_SEP (prog[i])) {
          string msg = xmalloc (50);
          sprintf (msg, "Unlikely character %c in program name", prog[i]);
          return msg;
        }
      }
    }
  }

  /* Skip whitespace, an optional =, more whitespace.  */
  while (*line && ISSPACE (*line))
    line++;
  if (*line == '=') {
    line++;
    while (*line && ISSPACE (*line))
      line++;
  }

  /* The value is whatever remains.  Remove trailing whitespace.  */
  start = line;
  len = strlen (start);
  while (len > 0 && ISSPACE (start[len - 1]))
    len--;
  if (len == 0) {
    return ("No cnf value");
  }
  
  value = (string) xmalloc (len + 1);
  strncpy (value, start, len);
  value[len] = 0;

  /* Suppose we want to write a single texmf.cnf that can be used under
     both Windows and Unix. This is feasible except for the path
     separators: : on Unix, ; on Windows. We can't switch Windows to
     allowing :, since : is the drive separator. So we switch Unix to
     allowing ;. On the other hand, we don't want to change IS_ENV_SEP
     and all the rest.

     So, translate all ;'s in the path values to :'s if we'd normally
     use :. (Fortunately we don't use ; as a normal character in values.)  */

  if (IS_ENV_SEP(':')) {
      string loc;
      for (loc = value; *loc; loc++) {
          if (*loc == ';')
              *loc = ':';
      }
  }

  /* If we're supposed to make the setting in the environment ... */
  if (env_progname) {
    string this_prog = prog ? prog : kpse->program_name; /* maybe with .prog */
    /* last-ditch debug */
    /* fprintf (stderr, "kpse/cnf.c xputenv(%s,%s)\n", var, value); */
    xputenv (var, value);
    /* Use the .prog name on the input line if specified. Otherwise,
       although kpse->program_name should always be set at this point,
       check just in case.  */
    if (this_prog) {
      string var_prog = concat3 (var, "_", this_prog);
      /* fprintf (stderr, "kpse/cnf.c xputenv(%s,%s) [implicit]\n",
               var_prog, value); */
      xputenv (var_prog, value);
      free (var_prog); /* xputenv allocates its own */
    }
    free (var); /* again, xputenv allocated a copy */

  } else {
    /* Normal case of not ENV_PROGNAME, insert in cnf_hash, with .prog
       if specified, which will override non-.prog.  */
    string lhs = prog ? concat3 (var, ".", prog) : var;
    /* last-ditch debug */
    /* fprintf (stderr, "kpse/cnf.c hash_insert(%s,%s)\n", lhs, value); */
    hash_insert (&(kpse->cnf_hash), lhs, value);
    if (prog) {  /* the lhs string is new memory if we had .prog */
      free (var);
      /* If there was no .prog on the line, the original `var' memory gets
         inserted into the hash table, so do not free.  */
    }
  }

  /* We should check that anything remaining is preceded by a comment
     character, but we don't.  Sorry.  */
  return NULL;
}


/* Just passing along env_progname = true to do_line.  */

void
kpathsea_cnf_line_env_progname (kpathsea kpse, string line)
{
  string msg = do_line (kpse, line, /* env_progname = */ true);
  if (msg) {
    WARNING2 ("command line (kpathsea): %s in argument: %s",
              msg, line);
  }
}


/* Read all the kpathsea configuration files in the path.  */

static void
read_all_cnf (kpathsea kpse)
{
  string *cnf_files;
  string *cnf;
  const_string cnf_path = kpathsea_init_format (kpse, kpse_cnf_format);

  cnf_files = kpathsea_all_path_search (kpse, cnf_path, CNF_NAME);
  if (cnf_files && *cnf_files) {
    for (cnf = cnf_files; *cnf; cnf++) {
      string line;
      string msg;
      unsigned lineno = 0;
      FILE *cnf_file = xfopen (*cnf, FOPEN_R_MODE);
      if (kpse->record_input)
        kpse->record_input (*cnf);

      while ((line = read_line (cnf_file)) != NULL) {
        unsigned len;
        lineno++;
        len = strlen (line);
        /* Strip trailing spaces. */
        while (len > 0 && ISSPACE(line[len-1])) {
          line[len - 1] = 0;
          --len;
        }
        /* Concatenate consecutive lines that end with \.  */
        while (len > 0 && line[len - 1] == '\\') {
          string next_line = read_line (cnf_file);
          lineno++;
          line[len - 1] = 0;
          if (!next_line) {
            WARNING2 ("%s:%d: (kpathsea) Last line of file ends with \\",
                       *cnf, lineno);
          } else {
            string new_line;
            new_line = concat (line, next_line);
            free (line);
            line = new_line;
            len = strlen (line);
          }
        }

        msg = do_line (kpse, line, /* env_progname= */ false);
        if (msg) {
          WARNING4 ("%s:%d: (kpathsea) %s on line: %s",
                    *cnf, lineno, msg, line);
        }
        free (line);
      }

      xfclose (cnf_file, *cnf);
      free (*cnf);
    }
    free (cnf_files);
  } else {
    string warn = getenv ("KPATHSEA_WARNING");
    if (!(warn && STREQ (warn, "0"))) {
      WARNING1
  ("kpathsea: configuration file texmf.cnf not found in these directories: %s",
        cnf_path);
    }
  }
}

/* Read the cnf files on the first call.  Return the first value in the
   returned list -- this will be from the last-read cnf file.  */

const_string
kpathsea_cnf_get (kpathsea kpse, const_string name)
{
  string ctry;
  const_string ret, *ret_list;

  /* When we expand the compile-time value for DEFAULT_TEXMFCNF,
     we end up needing the value for assorted variables,
     so kpse_var_expand ends up calling us again.  Just return.  */
  if (kpse->doing_cnf_init)
    return NULL;

  /* If no cnf hash yet, initialize.  */
  if (kpse->cnf_hash.size == 0) {
    kpse->cnf_hash = hash_create (CNF_HASH_SIZE);
    
    /* Read configuration files and initialize databases.  */
    kpse->doing_cnf_init = true;
    read_all_cnf (kpse);
    kpse->doing_cnf_init = false;

    /* Since `kpse_init_db' recursively calls us, we must call it from
       outside a `kpse_path_element' loop (namely, the one in
       `read_all_cnf' above): `kpse_path_element' is not reentrant.  */
    kpathsea_init_db (kpse);
  }

  /* First look up NAME.`kpse->program_name', then NAME.  */
  assert (kpse->program_name);
  ctry = concat3 (name, ".", kpse->program_name);
  ret_list = hash_lookup (kpse->cnf_hash, ctry);
  free (ctry);
  if (ret_list) {
    ret = *ret_list;
    free (ret_list);
  } else {
    ret_list = hash_lookup (kpse->cnf_hash, name);
    if (ret_list) {
      ret = *ret_list;
      free (ret_list);
    } else {
      ret = NULL;
    }
  }

  return ret;
}

#if defined(KPSE_COMPAT_API)
const_string
kpse_cnf_get (const_string name)
{
  return kpathsea_cnf_get(kpse_def, name);
}
#endif
