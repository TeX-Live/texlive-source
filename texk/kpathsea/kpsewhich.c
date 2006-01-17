/* kpsewhich -- standalone path lookup and variable expansion for Kpathsea.
   Ideas from Thomas Esser and Pierre MacKay.

   Copyright (C) 1995 - 2005 Karl Berry & Olaf Weber.

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

#include <kpathsea/config.h>
#include <kpathsea/c-ctype.h>
#include <kpathsea/c-pathch.h>
#include <kpathsea/expand.h>
#include <kpathsea/getopt.h>
#include <kpathsea/line.h>
#include <kpathsea/pathsearch.h>
#include <kpathsea/proginit.h>
#include <kpathsea/tex-file.h>
#include <kpathsea/tex-glyph.h>
#include <kpathsea/variable.h>
#include <kpathsea/progname.h>


/* Base resolution. (-D, -dpi) */
unsigned dpi = 600;

/* For variable and path expansion.  (-expand-var, -expand-path,
   -show-path, -separator) */
string var_to_expand = NULL;
string braces_to_expand = NULL;
string path_to_expand = NULL;
string path_to_show = NULL;
string var_to_value = NULL;

/* The file type and path for lookups.  (-format, -path) */
kpse_file_format_type user_format = kpse_last_format;
string user_format_string;
string user_path;

/* Interactively ask for names to look up?  (-interactive) */
boolean interactive = false;

/* Search the disk as well as ls-R?  (-must-exist) */
boolean must_exist = false;

/* Return all matches, not just the first one?  (-all) */
boolean show_all = false;

/* The device name, for $MAKETEX_MODE.  (-mode) */
string mode = NULL;

/* The program name, for `.PROG' construct in texmf.cnf.  (-program) */
string progname = NULL;

/* The engine name, for '$engine' construct in texmf.cnf.  (-engine) */
string engine = NULL;

/* Return the <number> substring in `<name>.<number><stuff>', if S has
   that form.  If it doesn't, return 0.  */

static unsigned
find_dpi P1C(string, s)
{
  unsigned dpi_number = 0;
  string extension = find_suffix (s);
  
  if (extension != NULL)
    sscanf (extension, "%u", &dpi_number);

  return dpi_number;
}

/* Use the file type from -format if that was specified, else guess
   dynamically from NAME.  Return kpse_last_format if undeterminable.
   This function is also used to parse the -format string, a case which
   we distinguish by setting is_filename to false.

   Note that a few filenames have been hard-coded for format types that
   differ from what would be inferred from their extensions. */

static kpse_file_format_type
find_format P2C(string, name, boolean, is_filename)
{
  kpse_file_format_type ret;
  
  if (is_filename && user_format != kpse_last_format) {
    ret = user_format;
  } else if (FILESTRCASEEQ (name, "pdftex.cfg")) {
    ret = kpse_pdftex_config_format;
  } else {
    int f;  /* kpse_file_format_type */
    unsigned name_len = strlen (name);

/* Have to rely on `try_len' being declared here, since we can't assume
   GNU C and statement expressions.  */
#define TRY_SUFFIX(ftry) (\
  try_len = (ftry) ? strlen (ftry) : 0, \
  (ftry) && try_len <= name_len \
     && FILESTRCASEEQ (ftry, name + name_len - try_len))

    f = 0;
    while (f != kpse_last_format) {
      unsigned try_len;
      const_string *ext;
      const_string ftry;
      boolean found = false;
      
      if (!kpse_format_info[f].type)
        kpse_init_format ((kpse_file_format_type)f);

      if (!is_filename) {
        /* Allow the long name, but only in the -format option.  We don't
           want a filename confused with a format name.  */
        ftry = kpse_format_info[f].type;
        found = TRY_SUFFIX (ftry);
      }
      for (ext = kpse_format_info[f].suffix; !found && ext && *ext; ext++){
        found = TRY_SUFFIX (*ext);
      }      
      for (ext = kpse_format_info[f].alt_suffix; !found && ext && *ext; ext++){
        found = TRY_SUFFIX (*ext);
      }

      if (found)
        break;

      /* Some trickery here: the extensions for kpse_fmt_format can
       * clash with other extensions in use, and we prefer for those
       * others to be preferred.  And we don't want to change the
       * integer value of kpse_fmt_format.  So skip it when first
       * enountered, then use it when we've done everything else,
       * and use it as the end-guard.
       */
      if (f == kpse_fmt_format) {
        f = kpse_last_format;
      } else if (++f == kpse_fmt_format) {
        f++;
      } else if (f == kpse_last_format) {
        f = kpse_fmt_format;
      }
    }

    /* If there was a match, f will be one past the correct value.  */
    ret = f;
  }
  
  return ret;
}

/* Look up a single filename NAME.  Return 0 if success, 1 if failure.  */

static unsigned
lookup P1C(string, name)
{
  string ret = NULL;
  string *ret_list = NULL;
  int i;
  unsigned local_dpi;
  kpse_glyph_file_type glyph_ret;
  
  if (user_path) {
    if (show_all) {
      ret_list = kpse_all_path_search (user_path, name);
    } else {
       ret = kpse_path_search (user_path, name, must_exist);
    }
    
  } else {
    /* No user-specified search path, check user format or guess from NAME.  */
    kpse_file_format_type fmt = find_format (name, true);

    switch (fmt) {
      case kpse_pk_format:
      case kpse_gf_format:
      case kpse_any_glyph_format:
        /* Try to extract the resolution from the name.  */
        local_dpi = find_dpi (name);
        if (!local_dpi)
          local_dpi = dpi;
        ret = kpse_find_glyph (remove_suffix (name), local_dpi, fmt, &glyph_ret);
        break;

      case kpse_last_format:
        /* If the suffix isn't recognized, assume it's a tex file. */
        fmt = kpse_tex_format;
        /* fall through */

      default:
        ret = kpse_find_file (name, fmt, must_exist);
    }
  }
  
  if (ret)
    puts (ret);
  if (ret_list) {
    for (i = 0; ret_list[i]; i++)
      puts (ret_list[i]);
    /* Save whether we found anything */
    ret = ret_list[0];
    free (ret_list);
  }
  
  return ret == NULL;
}

/* Reading the options.  */

#define USAGE "\
  Standalone path lookup and expansion for Kpathsea.\n\
\n\
-debug=NUM             set debugging flags.\n\
-D, -dpi=NUM           use a base resolution of NUM; default 600.\n\
-engine=STRING         set engine name to STRING.\n\
-expand-braces=STRING  output variable and brace expansion of STRING.\n\
-expand-path=STRING    output complete path expansion of STRING.\n\
-expand-var=STRING     output variable expansion of STRING.\n\
-format=NAME           use file type NAME (see list below).\n\
-help                  print this message and exit.\n\
-interactive           ask for additional filenames to look up.\n\
[-no]-mktex=FMT        disable/enable mktexFMT generation (FMT=pk/mf/tex/tfm).\n\
-mode=STRING           set device name for $MAKETEX_MODE to STRING;\n\
                       no default.\n\
-must-exist            search the disk as well as ls-R if necessary\n\
-path=STRING           search in the path STRING.\n\
-progname=STRING       set program name to STRING.\n\
-show-path=NAME        output search path for file type NAME (see list below).\n\
-var-value=STRING      output the value of variable $STRING.\n\
-version               print version number and exit.\n \
"

/* Test whether getopt found an option ``A''.
   Assumes the option index is in the variable `option_index', and the
   option table in a variable `long_options'.  */
#define ARGUMENT_IS(a) STREQ (long_options[option_index].name, a)

/* SunOS cc can't initialize automatic structs.  */
static struct option long_options[]
  = { { "D",			1, 0, 0 },
      { "all",			0, (int *) &show_all, 1 },
      { "debug",		1, 0, 0 },
      { "dpi",			1, 0, 0 },
      { "engine",		1, 0, 0 },
      { "expand-braces",	1, 0, 0 },
      { "expand-path",		1, 0, 0 },
      { "expand-var",		1, 0, 0 },
      { "format",		1, 0, 0 },
      { "help",                 0, 0, 0 },
      { "interactive",		0, (int *) &interactive, 1 },
      { "mktex",		1, 0, 0 },
      { "mode",			1, 0, 0 },
      { "must-exist",		0, (int *) &must_exist, 1 },
      { "path",			1, 0, 0 },
      { "no-mktex",		1, 0, 0 },
      { "progname",		1, 0, 0 },
      { "separator",		1, 0, 0 },
      { "show-path",		1, 0, 0 },
      { "var-value",		1, 0, 0 },
      { "version",              0, 0, 0 },
      { 0, 0, 0, 0 } };

static void
read_command_line P2C(int, argc,  string *, argv)
{
  int g;   /* `getopt' return code.  */
  int option_index;

  for (;;) {
    g = getopt_long_only (argc, argv, "", long_options, &option_index);

    if (g == -1)
      break;

    if (g == '?')
      exit (1);  /* Unknown option.  */

    assert (g == 0); /* We have no short option names.  */

    if (ARGUMENT_IS ("debug")) {
      kpathsea_debug |= atoi (optarg);

    } else if (ARGUMENT_IS ("dpi") || ARGUMENT_IS ("D")) {
      dpi = atoi (optarg);

    } else if (ARGUMENT_IS ("engine")) {
      engine = optarg;
      
    } else if (ARGUMENT_IS ("expand-braces")) {
      braces_to_expand = optarg;
      
    } else if (ARGUMENT_IS ("expand-path")) {
      path_to_expand = optarg;

    } else if (ARGUMENT_IS ("expand-var")) {
      var_to_expand = optarg;

    } else if (ARGUMENT_IS ("format")) {
      user_format_string = optarg;

    } else if (ARGUMENT_IS ("help")) {
      int f; /* kpse_file_format_type */
      extern KPSEDLL char *kpse_bug_address; /* from version.c */
      
      printf ("Usage: %s [OPTION]... [FILENAME]...\n", argv[0]);
      fputs (USAGE, stdout);
      putchar ('\n');
      fputs (kpse_bug_address, stdout);

      /* Have to set this for init_format to work.  */
      kpse_set_program_name (argv[0], progname);

      puts ("\nRecognized format names and their suffixes:");
      for (f = 0; f < kpse_last_format; f++) {
        const_string *ext;
        kpse_init_format ((kpse_file_format_type)f);
        printf ("%s:", kpse_format_info[f].type);
        for (ext = kpse_format_info[f].suffix; ext && *ext; ext++) {
          putchar (' ');
          fputs (*ext, stdout);
        }
        for (ext = kpse_format_info[f].alt_suffix; ext && *ext; ext++) {
          putchar (' ');
          fputs (*ext, stdout);
        }
        putchar ('\n');
      }

      exit (0);

    } else if (ARGUMENT_IS ("mktex")) {
      kpse_maketex_option (optarg, true);

    } else if (ARGUMENT_IS ("mode")) {
      mode = optarg;

    } else if (ARGUMENT_IS ("no-mktex")) {
      kpse_maketex_option (optarg, false);

    } else if (ARGUMENT_IS ("path")) {
      user_path = optarg;

    } else if (ARGUMENT_IS ("progname")) {
      progname = optarg;

    } else if (ARGUMENT_IS ("show-path")) {
      path_to_show = optarg;
      user_format_string = optarg;

    } else if (ARGUMENT_IS ("var-value")) {
      var_to_value = optarg;

    } else if (ARGUMENT_IS ("version")) {
      extern KPSEDLL char *kpathsea_version_string; /* from version.c */
      puts (kpathsea_version_string);
      puts ("Copyright 2005 Karl Berry & Olaf Weber.\n\
There is NO warranty.  You may redistribute this software\n\
under the terms of the GNU General Public License.\n\
For more information about these matters, see the files named GPL and LGPL.");
      exit (0);
    }

    /* Else it was just a flag; getopt has already done the assignment.  */
  }
  
  if (user_path && user_format_string) {
    fprintf (stderr, "-path (%s) and -format (%s) are mutually exclusive.\n",
             user_path, user_format_string);
    fputs ("Try `kpsewhich --help' for more information.\n", stderr);
    exit (1);
  }

  if (optind == argc && !var_to_expand && !braces_to_expand && !path_to_expand
                     && !path_to_show && !var_to_value) {
    fputs ("Missing argument. Try `kpsewhich --help' for more information.\n",
           stderr);
    exit (1);
  }
}

int
main P2C(int, argc,  string *, argv)
{
  unsigned unfound = 0;

  read_command_line (argc, argv);

  kpse_set_program_name (argv[0], progname);

  if (engine)
    xputenv("engine", engine);
  
  /* NULL for no fallback font.  */
  kpse_init_prog (uppercasify (kpse_program_name), dpi, mode, NULL);
  
  /* Have to do this after setting the program name.  */
  if (user_format_string)
    user_format = find_format (user_format_string, false);
  
  /* Variable expansion.  */
  if (var_to_expand)
    puts (kpse_var_expand (var_to_expand));

  /* Brace expansion. */
  if (braces_to_expand)
    puts (kpse_brace_expand (braces_to_expand));
  
  /* Path expansion. */
  if (path_to_expand)
    puts (kpse_path_expand (path_to_expand));

  /* Show a search path. */
  if (path_to_show) {
    if (user_format != kpse_last_format) {
      if (!kpse_format_info[user_format].type) /* needed if arg was numeric */
        kpse_init_format (user_format);
      puts (kpse_format_info[user_format].path);
    } else {
      WARNING1 ("kpsewhich: Ignoring unknown file type `%s'", path_to_show);
    }
  }

  /* Var to value. */
  if (var_to_value) {
    string value = kpse_var_value (var_to_value);
    if (!value) {
      unfound++;
      value="";
    }
    puts (value);
  }
  
  for (; optind < argc; optind++) {
    unfound += lookup (argv[optind]);
  }

  if (interactive) {
    for (;;) {
      string name = read_line (stdin);
      if (!name || STREQ (name, "q") || STREQ (name, "quit")) break;
      unfound += lookup (name);
      free (name);
    }
  }
  
  return unfound > 255 ? 1 : unfound;
}
