/* kpsecheck -- report about the files stored in ls-R databases.
   Copyright (C) 1997, 2000 Fabrice Popineau.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include <kpathsea/config.h>
#include <kpathsea/lib.h>
#include <kpathsea/hash.h>
#include <kpathsea/getopt.h>
#include <kpathsea/c-pathch.h>
#include <kpathsea/proginit.h>
#include <kpathsea/tex-file.h>
#include "gs32lib.h"

#include "fnmatch.h"


/* Reading the options.  */

string progname = NULL;

/* The device name, for $MAKETEX_MODE.  (-mode) */
string mode = NULL;
/* Base resolution. (-D, -dpi) */
unsigned dpi = 600;

#define USAGE "\
  Kpathsea checking about ls-R files.\n\
\n\
-debug=NUM             set debugging flags.\n\
\n\
-multiple-occurences   check for multiple occurences of the same file.\n\
-exclude=<pattern>     exclude any filename that matches the argument.\n\
-include=<pattern>     include only filenames that match the argument.\n\
-file=<pattern>        look for multiple occurences of the pattern.\n\
\n\
-shared-memory         report about shared memory usage.\n\
\n\
-ghostscript           report about the gs version installed.\n\
\n\
-help                  print this message and exit.\n\
-version               print version number and exit.\n\
"

/* Test whether getopt found an option ``A''.
   Assumes the option index is in the variable `option_index', and the
   option table in a variable `long_options'.  */
#define ARGUMENT_IS(a) STREQ (long_options[option_index].name, a)

/* SunOS cc can't initialize automatic structs.  */
static struct option long_options[]
  = { { "debug",		1, 0, 'd' },
      { "exclude",              1, 0, 'x' },
      { "include",              1, 0, 'i' },
      { "file",                 1, 0, 'f' },
      { "multiple-occurences",	0, 0, 'm' },
      { "shared-memory",	0, 0, 's' },
      { "ghostscript",	        0, 0, 'g' },
      { "help",                 0, 0, 'h' },
      { "version",              0, 0, 'v' },
      { 0, 0, 0, 0 } };

boolean opt_doublons = FALSE;
boolean opt_shared = FALSE;
boolean opt_ghostscript = FALSE;

const_string opt_file = NULL;

#define CHUNK 4096

string *file_list = NULL;
int file_num = 0;
int file_list_size = 0;

#define MAX_EXCLUDE 1024
#define MAX_INCLUDE 1024
const_string exclude_list[MAX_EXCLUDE];
const_string include_list[MAX_EXCLUDE];
int nb_exclude = 0;
int nb_include = 0;

#ifdef WIN32
#define FNM_FLAGS FNM_CASEFOLD
#else
#define FNM_FLAGS 0
#endif

void 
add_to_exclude_list(const_string exp)
{
  exclude_list[nb_exclude++] = exp;
}

void 
add_to_include_list(const_string exp)
{
  include_list[nb_include++] = exp;
}

void CDECL
build_file_list P2C(const_string, key, const_string, value)
{
  int i;
  string filename = concat(value, key);

  if (dir_p(filename))
    return;

  for (i = 0; i < nb_exclude; i++) {
    if (fnmatch(exclude_list[i], filename, FNM_FLAGS) == 0) {
      return;
    }
  }

  if (nb_include > 0) {
    for (i = 0; i < nb_include; i++) {
      if (fnmatch(include_list[i], filename, FNM_FLAGS) == FNM_NOMATCH)
	return;
    }
  }

  file_list[file_num] = filename;
  dostounix_filename(file_list[file_num]);
  file_num++;
  if (file_num >= file_list_size) {
    file_list_size += CHUNK;
    file_list = xrealloc(file_list, file_list_size * sizeof(string));
  }
}

int CDECL
file_cmp(const_string s1, const_string s2)
{
  return FILESTRCASEEQ(xbasename(s1), xbasename(s2));
}

void
check_for_doublons P1H(void)
{
  hash_table_type *db;
  string s1 = NULL, s2 = NULL;
  int i;

  file_list = (string *)xmalloc(CHUNK * sizeof(string));
  file_list_size = CHUNK;

  db = hash_exists_p(hashtable_db);

  hash_iter(db, &build_file_list);

  printf("%d files in your databases.\nSorting ...\n", file_num);

  qsort(file_list, file_num, sizeof(string), file_cmp);

  puts("Multiple occurences :\n");

  for (i = 0; i < file_num - 1; i++) {
    if (file_cmp(file_list[i], file_list[i+1])) {
      puts(file_list[i]);
      puts(file_list[i+1]);
      i++;
      while (i < file_num - 1 && file_cmp(file_list[i], file_list[i+1])) {
	puts(file_list[i+1]);
	i++;
      }
      putchar('\n');
    }
  }
  puts("End of list.\n");

  for (i = 0; i < file_num; i++) free(file_list[i]);
  free(file_list);
}

void
check_for_shared_memory P1H(void)
{
  HANDLE hash_handle;

  hash_handle = OpenFileMapping(FILE_MAP_ALL_ACCESS, TRUE, HASH_MAPPING_NAME);

  if (hash_handle == 0) {
    printf("No kpathsea file mapping set up.\n");
  }
  else {
    printf("Kpathsea file mapping is set up.\n");
    CloseHandle(hash_handle);
  }
}

void
check_for_ghostscript P1H(void)
{
  char *gs_path = gs_locate();

  if (gs_path == NULL) {
    printf("%s: no version of Ghostscript has been found.\n", progname);
    return;
  }

  printf("%s: Ghostscript version %s has been found:\n",
	 progname, gs_revision);
  printf("dll     : %s\n", gs_dll);
  printf("libpath : %s\n", gs_lib);
}

void
usage P1H(void)
{
      extern KPSEDLL char *kpse_bug_address; /* from version.c */
      
      printf ("Usage: %s [OPTION]... [FILENAME]...\n", progname);
      fputs (USAGE, stdout);
      putchar ('\n');
      fputs (kpse_bug_address, stdout);
}

static void
read_command_line P2C(int, argc,  string *, argv)
{
  int g;   /* `getopt' return code.  */
  int option_index;

  for (;;) {
    g = getopt_long_only (argc, argv, "d:mx:i:f:shv", long_options, &option_index);

    if (g == -1)
      break;

    if (g == '?')
      exit (1);  /* Unknown option.  */
#if 0
    if (ARGUMENT_IS ("debug")) {
      kpathsea_debug |= atoi (optarg);

    } else if (ARGUMENT_IS ("multiple-occurences")) {
      opt_doublons = TRUE;

    } else if (ARGUMENT_IS ("exclude")) {
      add_to_exclude_list(optarg);

    } else if (ARGUMENT_IS ("include")) {
      add_to_include_list(optarg);

    } else if (ARGUMENT_IS ("file")) {
      opt_file = optarg;

    } else if (ARGUMENT_IS ("shared-memory")) {
      opt_shared = TRUE;

    } else if (ARGUMENT_IS ("help")) {
      usage();
      exit (0);

    } else if (ARGUMENT_IS ("version")) {
      extern KPSEDLL char *kpathsea_version_string; /* from version.c */
      puts (kpathsea_version_string);
      puts ("Copyright (C) 1997 K. Berry.\n\
There is NO warranty.  You may redistribute this software\n\
under the terms of the GNU General Public License.\n\
For more information about these matters, see the files named COPYING.");
      exit (0);
    }
#else
    switch (g) {
    case 'd':
      kpathsea_debug |= atoi (optarg);
      break;
    case 'm':
      opt_doublons = TRUE;
      break;
    case 'x':
      add_to_exclude_list(optarg);
      break;
    case 'i':
      add_to_include_list(optarg);
      break;
    case 'f':
      opt_file = optarg;
      break;
    case 's':
      opt_shared = TRUE;
      break;
    case 'g':
      opt_ghostscript = TRUE;
      break;
    case 'h':
      usage();
      exit (0);
    case 'v':
      { 
	extern KPSEDLL char *kpathsea_version_string; /* from version.c */
	puts (kpathsea_version_string);
	puts ("Copyright (C) 1997 K. Berry.\n\
There is NO warranty.  You may redistribute this software\n\
under the terms of the GNU General Public License.\n\
For more information about these matters, see the files named COPYING.");
	exit (0);
      }
    default:
      fprintf(stderr, "%: unknown option.\n", progname);
      usage();
      exit(1);
    }
#endif

    /* Else it was just a flag; getopt has already done the assignment.  */
  }

  if (opt_doublons) {
    for ( ; optind < argc; optind++) {
      add_to_include_list(argv[optind]);
    }
  }
  else {
    if (optind < argc - 1) {
      fprintf(stderr, "%s: warning, extra arguments.\n", progname);
    }
  }
}

int
main P2C(int, argc, char **, argv)
{
  progname = argv[0];

  read_command_line (argc, argv);

  if (opt_shared) {
    check_for_shared_memory();
  }
  else if (opt_ghostscript) {
    check_for_ghostscript();
  }
  else if (opt_doublons) {

    kpse_set_program_name (argv[0], progname);
    /* NULL for no fallback font.  */
    kpse_init_prog (uppercasify (kpse_program_name), dpi, mode, NULL);

    check_for_doublons();
  }
  else {
    usage();
  }

  return 0;
}
