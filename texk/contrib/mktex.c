/* mktex.c: this program emulates the mktex* shell scripts from web2c.

Copyright (C) 1997 Fabrice POPINEAU.
Adapted to MS-DOS/DJGPP by Eli Zaretskii <eliz@is.elta.co.il>.

Time-stamp: <04/03/14 00:12:09 popineau>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/* Maximum number of lines per usage message. */
#define MAX_LINES 16

#define SEPARATORS " \t\n\""

#include <signal.h>

#include "stackenv.h"
#include "variables.h"
#include "fileutils.h"
#include "mktex.h"

#ifdef _WIN32
#define isatty _isatty
#endif

/* 
   Global Variables found in the mktex* scripts 
*/

/* mktex.opt */
int program_number = -1;
string progname = NULL;

/* mktex.cnf */

char tmpdir[PATH_MAX]; /* Working temporary directory */
string output = "astdout";
FILE *fout = NULL;
static boolean downcase_names;

char empty_str[] = "(empty)";

static int errstatus;
extern int mt_append_mask;
void delmulslash(string );
void read_mktexcnf(void);
void usage(void);

/* Description of functionality : long name, short name, min args,
   max args and pointer to a function to execute. */
program_description makedesc [] = {
  {"mktexpk",  2, 4, mktexpk },		/* 4,7 */
  {"mktextfm", 2, 2, mktextfm },	/* 1,2 */
  {"mktextex", 4, 6, mktextex },	/* ?? */
  {"mktexmf",  2, 2, mktexmf },		/* 1,1 */
  {"mktexlsr", 1, 255, mktexlsr },	/* 0,MANY */
  {"texhash", 1, 255, mktexlsr },	/* 0,MANY */
  {"mktexupd", 3, 3, mktexupdate },	/* 2,2 */
  {"mktexdir", 2, 255, mktexmkdir },	/* 0,MANY */
  {"mktexrmd", 2, 255, mktexrmdir },	/* ?? */
  {"mktexnam", 2, 5, mktexnames },	/* 1,4 */
#if 0
  {"mktexfmt", 2, 5, mktexfmt },	/* 1,4 */
#endif
  {0, 0, 0}
};

#define PROGRAM_IS(p) STREQ (p, makedesc[program_number].name)

/* Test whether getopt found an option ``A''.
   Assumes the option index is in the variable `option_index', and the
   option table in a variable `long_options'.  */
#define ARGUMENT_IS(a) STREQ (long_options[option_index].name, a)

static struct option long_options [] = {
    { "debug",               1, 0, 0},
    { "help",                0, 0, 0},
    { "version",             0, 0, 0},
    { "dpi",                 1, 0, 0},
    { "bdpi",                1, 0, 0},
    { "mag",                 1, 0, 0},
    { "mfmode",              1, 0, 0},
    { "destdir",             1, 0, 0},
    {0, 0, 0, 0}
};

/*
  First  argument takes progname;
  Other arguments take no arguments.
  */
static string usage_msg[][MAX_LINES] = {
  /* MktexPK */
  {
    "Usage: %s [OPTIONS] NAME [REDIRECT],\n",
    "Create a PK font.\n\n",

    "--dpi DPI           use resolution DPI.\n",
    "--bdpi BDPI         use base resolution BDPI.\n",
    "--mag MAG           use magnificiation MAG.\n",
    "--mfmode MODE       use MODE as the METAFONT mode.\n",
    "--destdir DESTDIR   write fonts in DESTDIR.\n\n",

    "Try to create a PK file for NAME at resolution DPI, with an assumed\n",
    "device base resolution of BDPI, and a Metafont `mag' of MAG. Use MODE\n",
    "for the METAFONT mode.  Use DESTDIR for the root of where to install\n",
    "into, either the absolute directory name to use (if it starts with a\n",
    "/) or relative to the default DESTDIR (if not). REDIRECT, if supplied,\n",
    "is a string of the form '>&n', where n is the number of the file\n",
    "descriptor which is to receive, instead of stdout, the name of the\n",
    "newly created pk file.\n",
    0
  },
  /* MktexTFM */
  {
    "Usage: %s [--destdir DESTDIR] FONT.\n\n",
    "Makes a TFM file for FONT, if possible. Use DESTDIR for the root of where\n",
    "to install into, either the absolute directory name to use (if it starts\n",
    "with a /) or relative to the default DESTDIR (if not).\n",
    0
  },
  /* MktexTeX */
  {
    0
  },
  /* MktexMF */
  {
    "Usage : %s FONT.\n\n",
    "Makes the Metafont source file for FONT, if possible. For example,\n",
    "`ecr12' or `cmr11'.\n",
    0
  },
  /* MktexlsR */
  {
    "Usage: %s [DIRS ...]\n\n",
    "Rebuild all necessary ls-R filename databases completely. If one or\n",
    "more arguments DIRS are given, these are used as texmf directories to\n",
    "build ls-R for. Else all directories in the search path for ls-R files\n",
    "($TEXMFDBS) are used.\n",
    0
  },
  /* TeXHash */
  {
    "Usage: %s [DIRS ...]\n\n",
    "Rebuild all necessary ls-R filename databases completely. If one or\n",
    "more arguments DIRS are given, these are used as texmf directories to\n",
    "build ls-R for. Else all directories in the search path for ls-R files\n",
    "($TEXMFDBS) are used.\n",
    0
  },
  /* Mktexupd */
  {
    "Usage : %s DIR FILE.\n",
    "  Update the ls-R file with an entry for FILE in DIR.\n",
    0
  },
  /* Mktexdir */
  {
    "Usage : %s DIRS...\n",
    "  Create each DIR, including any missing leading directories.\n",
    0
  },
  /* Mktexrmd */
  {
    "Usage : %s DIRS ...\n",
    "  Recursively remove each DIR.\n",
    0
  },
  /* Mktexnam */
  {
    "Usage : %s NAME [DPI MODE] [DESTDIR].\n",
    "  Output the PK, TFM and MF names for a font NAME.\n",
    0
  },
#if 0
  /* Mktexfmt */
  {
    "Usage : %s [-all] [FORMAT].\n",
    "  Rebuild all or specified format.\n",
    0
  }
#endif
};

/*
  First part: mktex.opt
  */
int mktex_opt(int argc, char *argv[],
	      program_description * program)
{
  int g; /* getopt return code */
  int i;
  int option_index;
  string tempenv, cwd, vartexmf;

  for(;;) {
    g = getopt_long_only (argc, argv, "", long_options, &option_index);

    if (g == EOF)
      break;

    if (g == '?') {
      usage();			/* Unknown option.  */
      exit(1);
    }

    /* assert (g == 0); */ /* We have no short option names.  */
    /*
      FIXME : try 'mktexpk --mfmode --bdpi 600 ...'
      */
    if (ARGUMENT_IS ("debug")) {
      kpathsea_debug |= atoi (optarg);
    }
    else if (ARGUMENT_IS ("help")) {
      usage();
      exit(0);
    }
    else if (ARGUMENT_IS ("version")) {
      fprintf(stderr, "%s of %s.\n", progname, kpathsea_version_string);
      exit(0);
    }
    else if (PROGRAM_IS("mktexpk")) {
      if (ARGUMENT_IS("dpi")) {
	setval("DPI", optarg);
      }
      else if (ARGUMENT_IS("bdpi")) {
	setval("BDPI", optarg);
      }
      else if (ARGUMENT_IS("mag")) {
	setval("MAG", optarg);
      }
      else if (ARGUMENT_IS("mfmode")) {
	if (!STREQ(optarg, DIR_SEP_STRING))
	  setval("MODE", optarg);
      }
      else if (ARGUMENT_IS("destdir")) {
	setval("DEST", optarg);
      }
    }
    else if (PROGRAM_IS("mkextfm")) {
      if (ARGUMENT_IS("destdir")) {
	setval("DEST", optarg);
      }
    }
  }

  /* shifting options from argv[] list */
  for (i = 1; optind < argc; i++, optind++)
    argv[i] = argv[optind];
  argv[i] = NULL;

  argc = i;
  if (argc < program->arg_min) {
    fprintf (stderr, "%s: Missing argument(s).\nTry `%s --help' for more information.\n", progname, kpse_program_name);
    exit(1);
  }
  if (argc > program->arg_max) {
    fprintf(stderr, "%s: Extra arguments", progname);
    for (i = program->arg_max - 1; i < argc; i++)
      fprintf (stderr, " \"%s\"", argv[i]);
    fprintf (stderr, "\nTry `%s --help' for more information.\n",
	     kpse_program_name);
    exit(1);
  }

  /* Catch signals, so we clean up if the child is interrupted.
     This emulates "trap 'whatever' 1 2 15".  */
#ifdef _WIN32
  SetConsoleCtrlHandler((PHANDLER_ROUTINE)sigint_handler, TRUE);
#else
# ifdef SIGHUP
  signal (SIGHUP, sigint_handler);
# endif
# ifdef SIGINT
  signal (SIGINT, sigint_handler);
# endif
# ifdef SIGQUIT
  signal (SIGQUIT, sigint_handler);
# endif
# ifdef SIGEMT
  signal (SIGEMT, sigint_handler);
# endif
# ifdef SIGPIPE
  signal (SIGPIPE, sigint_handler);
# endif
# ifdef SIGTERM
  signal (SIGTERM, sigint_handler);
# endif
#endif

  tempenv = getval("TMPDIR");
  if (test_file ('z', tempenv))
    tempenv = getval("TEMP");
  if (test_file ('z', tempenv))
    tempenv = getval("TMP");
#ifdef DOSISH
  if (test_file ('z', tempenv))
    tempenv = "c:/tmp";	/* "/tmp" is not good if we are on a CD-ROM */
#else
  if (test_file ('z', tempenv))
    tempenv = "/tmp";
#endif
  sprintf(tmpdir, "%s/mtXXXXXX", tempenv);
  mktemp(tmpdir);
  if (do_makedir(tmpdir)) {
    perror(tmpdir);
    exit(1);
  }
  setval("TEMPDIR", tmpdir);
  setval("STDOUT", concat_pathes(tmpdir, "astdout"));
  setval("KPSE_DOT", cwd = xgetcwd());
  /* export KPSE_DOT */
  xputenv("KPSE_DOT", cwd);

  /*: ${MT_VARTEXFONTS=`kpsewhich --expand-var='$VARTEXFONTS' | sed 's%^!!%%'`} */
  vartexmf = setval_default("MT_VARTEXFONTS", expand_var("$VARTEXFONTS"));

  /* A sanity check.  */
  if (test_file('z', vartexmf)) {
    fprintf(stderr, 
	    "%s:  VARTEXFONTS is not defined.  Defaulting to `%s'.\n",
	    progname, expand_var("$KPSE_DOT"));
    setval("MT_VARTEXFONTS", "$KPSE_DOT");
  }

  /* mktex.opt sets the $MT_MKTEX... variables here to point to the supporting
     scripts.  But we don't need this, since we just call ourselves.  */
  setval("ls_R_magic",
	 "% ls-R -- filename database for kpathsea; do not change this line.");
  setval("old_ls_R_magic", 
	 "% ls-R -- maintained by MakeTeXls-R; do not change this line.");

  pushd(tmpdir);
  
  if ((fout = fopen(getval("STDOUT"), "w")) == NULL) {
    perror(output);
    mt_exit(1);
  }

  read_mktexcnf();

  /* features by default */
#ifdef MSDOS
  setval_default("MT_FEATURES", "appendonlydir:dosnames");
#else
  setval_default("MT_FEATURES", "appendonlydir");
#endif
  setval_default("MODE", "ljfour");
  setval_default("BDPI", "600");
  setval_default("ps_to_pk", "gsftopk");

  /* umask = 0 */
  umask(0);	/* for those platforms who support it */

  /* Cache values that may be useful for recursive calls.  */
  xputenv("MT_MKTEX_CNF", getval("MT_MKTEX_CNF"));
  xputenv("MT_TEXMFMAIN", getval("MT_TEXMFMAIN"));
  xputenv("MT_VARTEXFONTS", getval("MT_VARTEXFONTS"));

  return argc;
}
  
/* Reading mktex.cnf */

void read_mktexcnf()
{
  FILE *f;
  string line;
  string mtsite;

  mtsite = kpse_find_file ("mktex.cnf", kpse_web2c_format, false);
  mtsite = setval_default("MT_MKTEX_CNF", mtsite);
  if (test_file('n', mtsite) && test_file('r', mtsite)) {
    f = fopen(mtsite, "r");
    if (KPSE_DEBUG_P(MKTEX_DEBUG)) {
      fprintf(stderr, "Reading mktex_cnf file: %s\n", mtsite);
    }
    while ((line  = read_line(f)) != NULL) {
      /* skip comments */
      if (*line == '#' || *line == '%' || isspace(*line)
	  || *line == '\0' || *line == '\n') {
	free(line);
	continue;
      }

      if (!parse_variable(line))
	fprintf(stderr, "%s: unsupported keyword: %s\n", mtsite, line);
      free(line);
    }
    fclose(f);
  }

}

/* If non-negative, records the handle to redirect STDOUT
   in `output_and_cleanup'.  */
int redirect_stdout;

/* Record the handle where STDOUT is to be redirected
   for printing the names of the generated files before we exit.  */
void record_output_handle(int fd)
{
  redirect_stdout = fd;	/* FIXME: cannot be nested! */
}

void usage()
{
  int i;
  fprintf(stderr, "%s of %s\n", progname, kpathsea_version_string);
  fprintf(stderr, "Mktex version %s\n", mktex_version_string);
  fprintf(stderr,usage_msg[program_number][0], progname );
  fputs("\n", stderr);
  for(i = 1; usage_msg[program_number][i]; ++i)
    fputs(usage_msg[program_number][i], stderr);
}

void mktexinit(int argc, char **argv)
{
  string mode, bdpi;
  string texmfmain;

  program_number = -1;

  if (!progname)
    progname = argv[0];

  kpse_set_program_name (progname, NULL);

  /* initialize the symbol table */
  init_vars();
  /* `kpse_init_prog' will put MODE and DPI into environment as values of
     the two variables below.  We need to leave these variables as they are. */
  mode = getval("MAKETEX_MODE");
  bdpi = getval("MAKETEX_BASE_DPI");

  /* NULL for no fallback font. */
  kpse_init_prog (uppercasify (progname), 
		  bdpi && atoi(bdpi) ? atoi(bdpi) : 600, mode, NULL);

  /* Common code for all scripts.

    : ${MT_TEXMFMAIN=`kpsewhich -expand-path='$TEXMFMAIN'`}  */
  texmfmain = setval_default("MT_TEXMFMAIN", kpse_path_expand("$TEXMFMAIN"));

  /* FIXME: we don't support edited mktex.opt files, and don't look for
     that file along the TEXMF trees.  But at least we should fake the
     safeguard in case $TEXMFMAIN is not defined at all.  */
  if (test_file('z', texmfmain) || !test_file('d', texmfmain)) {
    fprintf(stderr, "%s: $TEXMFMAIN is undefined or points to a non-existent directory;\n%s: check your installation.\n$TEXMFMAIN = %s\n", progname, progname, kpse_var_expand("$TEXMFMAIN"));
    exit(1);	/* not mt_exit, since temporary files were not created yet */
  }

}

/* Output the string in lower case or without modification according
   to downcase_names. */
void output_name(string name, FILE *f)
{
  string p;
  if (downcase_names) {
    for(p = name; *p; p++)
      *p = TOLOWER(*p);
  }
  fputs(name, f);
}

/* lower casify the string */
string string_to_lower(string s)
{
  string p = NULL, q;
  if (s) {
    q = p = xstrdup(s);
    while (*q) {
      *q = TOLOWER(*q);
      q++;
    }
  }
  return p;
}

/* Info about the temporary file to which we redirect print_path's
   stdout.  */
static struct tem_file {
  const char *tem_dir;
  const char *tem_base;
  const char *lsr_base;
} tem_file_info;

/* Return 1 if DIR is the directory and BASE is the basename of the
   temporary file whose info is in tem_file structure.
   FIXME: this will break if mktexlsr invokes itself recirsively.  */
int is_db_file_tmp(string dir, int dir_len, char *base)
{
  if (!tem_file_info.tem_dir || !tem_file_info.tem_base)
    return 0;
  return ((dir_len >= 0 ? FILESTRNCASEEQ(dir, tem_file_info.tem_dir, dir_len)
		        : FILESTRCASEEQ(dir, tem_file_info.tem_dir))
	  && FILESTRCASEEQ(base, tem_file_info.tem_base));
}

/* Updates ls-R db for directory pointed to by path. */
void print_path(string path, struct stat* st)
{
  if ((st->st_mode & S_IFMT) == S_IFDIR) {
#ifdef _WIN32
    WIN32_FIND_DATA ffd;
    HANDLE hnd;
    string fulldir;
    int offset = 0;
    int ind = strlen(path);
#else
    DIR *dp;
    struct dirent *ep;
#endif

    putchar('\n');	/* extra blank line before new directory */
    output_name(path, stdout);
    if (path[0] == '.' && path[1] == '\0')
      putchar('/');
    putchar(':'); putchar('\n');

    /* Note that we actually read each directory twice: once
       here, to print all its entries, and once in `recurse_dir',
       to recurse into its subdirectories.  This is so that
       `ls-R' file is written in breadth-first manner, like
       `ls -R' does in the original script.  Caching the entries
       from the first read would make this code run faster, but
       since it is faster than the script already, why bother?  */

#ifdef _WIN32
    if (path[0] == '.' && IS_DIR_SEP(path[1])) {
      offset = 2;
    }
    fulldir = concat_pathes(tem_file_info.lsr_base, path + offset);
    
    strcat(path, "/*");	  /* ok since the caller says path[PATH_MAX] */
    hnd = FindFirstFile(path, &ffd);
    while (hnd && FindNextFile(hnd, &ffd)) { 
      if (ffd.cFileName[0] == '.'
	  && ((ffd.cFileName[1] == '\0')
	      || (ffd.cFileName[1] == '.' && ffd.cFileName[2] == '\0')))
	continue;
      if (!is_db_file_tmp(path, ind, ffd.cFileName)
	  /* We don't want to report directories ! */
	  && !(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
	output_name(ffd.cFileName, stdout);
	putchar('\n');/* We do want a \n */
	{
	  string fullname;
	  fullname = concat_pathes(fulldir, ffd.cFileName);
	  kpse_db_insert(fullname);
	  free(fullname);
	}
      }
    }
    path[ind] = '\0';
    FindClose(hnd);
    free(fulldir);
#else  /* not _WIN32 */
    if ((dp = opendir(path))) {
      while ((ep = readdir(dp)))
	if (strcmp(ep->d_name, ".")
	    && strcmp(ep->d_name, "..")
	    && !is_db_file_tmp(path, -1, ep->d_name)) {
	  output_name(ep->d_name, stdout);
	  putchar('\n');
	}
      closedir(dp);
    }
    else
      perror(path);
#endif /* not _WIN32 */

  }
}

/* return true if the fs on which path lies is case sensitive.
   Insufficient. Some people may have copied the complete tree in
   upper case from a case insensitive fs.
   Second test : look at the texmf name. If GetShortName !=
   GetLongName it is ok. 
*/
boolean is_casesensitive(string path)
{
  boolean retval = TRUE;	/* default for non-DOSISH systems */

#ifdef MONOCASE_FILENAMES
  char name[PATH_MAX];
#ifdef _WIN32
  char volname[PATH_MAX];
  DWORD sernum;
  DWORD maxcomplen;
#endif
#ifdef __DJGPP__
  int   maxnamelen;
  DWORD maxpathlen;
#endif
  DWORD fsflags;
  char fsname[PATH_MAX];
  
  /* How should the db be output ? Preserve case or lower case ? 
     First, determine the root volume : if path has the drive, ok. If
     not, get current drive. */
#ifdef WIN32
  if (IS_UNC_NAME(path)) {
    /* UNC pathname : \\server\dir\...
       We must find the first 2 components.
       */
    char *p;
    strcpy(name, path);
    name[0] = name[1] = '\\';
    p = strchr(name+2, '/');
    if (p)
      *p = '\\';
    else {
      p = strchr(name+2, '\\');
      if (!p) {
	fprintf(stderr, "Error getting volume info on %s.\n", name);
	mt_exit(1);
      }
    }
    p = strchr(p+1, '/');
    if (p)
      *p = '\\';
    else {
      p = strchr(p+1, '\\');
      if (!p) {
	fprintf(stderr, "Error getting volume info on %s.\n", name);
	mt_exit(1);
      }
    }
    *(p+1) = '\0';
  }
  else {
    if (IS_DEVICE_SEP(path[1])) {	/* the shortest path is "." and has 2 chars */
      name[0] =  path[0];
    }
    else
      name[0] = _getdrive() + 'A' - 1;
    
    strcpy(name+1, ":\\");
  }

  if (GetVolumeInformation(name, volname, 256, &sernum, &maxcomplen,
			   &fsflags, fsname, 256) == FALSE) {
    fprintf(stderr, "Error getting volume info on %s (code %d).\n", name, GetLastError());
    mt_exit(1);
  }

#else /* ! WIN32 */
  if (IS_DEVICE_SEP(path[1])) {	/* the shortest path is "." and has 2 chars */
    name[0] =  path[0];
  }
  else
    name[0] = _getdrive() + 'A' - 1;

  strcpy(name+1, ":\\");

#ifdef __DJGPP__
  fsflags = _get_volume_info(name, &maxnamelen, &maxpathlen, fsname);
#endif

#endif /* ! WIN32 */

  /* FIXME: FS_CASE_IS_PRESERVED does not seem sufficient. People 
     manage to get share/texmf/FONTS/TFM/... under W95. Too bad.
     After all, this flag distinguishes NTFS from all FATs */
    retval = (fsflags & FS_CASE_SENSITIVE) != 0;
  
  if (KPSE_DEBUG_P(MKTEX_FINE_DEBUG)) {
    fprintf(stderr, "FS %s %s case-sensitive\n", name, 
	    (retval ? "is" : "is not"));
  }
#endif /* MONOCASE_FILENAMES */

  return retval;
}

/* Set FILE to have the same permissions as directory DIR, minus the bits
   masked off by MASK.
   This emulates "chmod `kpsestat -xst,go-w $DIR` FILE".  */

int chmod_as_dir(string file, const string dir, mode_t mask)
{
  struct stat st_buf;

  if (stat (dir, &st_buf) == 0)
    return chmod (file, st_buf.st_mode & mask);
  return -1;
}

void do_mktexlsr(string path)
{
  static char name[PATH_MAX];
#ifdef __DJGPP__
  /* `stat' can be very expensive on MS-DOS, and `recurse_dir'
     uses it a lot. Make it work as fast as we possibly can afford.  */
  unsigned short save_stat_bits = _djstat_flags;
  _djstat_flags = (_STAT_WRITEBIT   | _STAT_ROOT_TIME | _STAT_DIRSIZE |
		   _STAT_EXEC_MAGIC | _STAT_EXEC_EXT);
#endif

  if (KPSE_DEBUG_P(MKTEX_DEBUG)) {
    fprintf(stderr, "Building ls-R database in: %s\n", path);
  }

  downcase_names = ! is_casesensitive(path);

  strcpy(name,path);
  recurse_dir(name, print_path, NULL);
#ifdef __DJGPP__
  _djstat_flags = save_stat_bits;
#endif
}

#if !defined(S_IFLNK) && !defined(S_ISLNK)

/* Dummy version of `readlink' for systems which don't support links.  */
int readlink(const char *path, char *buf, int bufsize)
{
  return -1;
}

#endif

int mktexlsr(int argc, char **argv)
{
  /* We have to take multiple root trees into account. */

  string db_dir, db_file, lsrdir;
  char db_dir_tmp[PATH_MAX];
  char db_file_tmp[PATH_MAX];
  FILE* lsrfile;
  int newfd[3];
  char **newargv;
  int i;

  if (argc == 1) {
    newargv = (char **) malloc(2 * sizeof(char *));
    newargv[0] = argv[0];
    /* This should give us the ls-R default path */
    if (!kpse_format_info[kpse_db_format].type) /* needed if arg was numeric */
      kpse_init_format (kpse_db_format);
    newargv[1] = xstrdup(kpse_format_info[kpse_db_format].path);
    argc = 2;
  }
  else {
    newargv = (char **) malloc(argc * sizeof(char *));
    for (i = 0; i < argc; i++)
      newargv[i] = argv[i];
  }

  /* For each argument in the list */
  for (i = 1; i < argc; i++) {
    for (lsrdir = kpse_path_element(newargv[i]);
	 lsrdir;
	 lsrdir = kpse_path_element(NULL)) {
      char symlink[PATH_MAX];
      int symlink_len;

      /* We need to skip !! */
      if (lsrdir && *lsrdir == '!' && *(lsrdir+1) == '!')
	lsrdir +=2;
      /* Skip empty path components. */
      if (*lsrdir == 0)
        continue;
      /* FIXME : /local/tex is absolute under win32, but should it really be ? */
      if (!kpse_absolute_p(lsrdir, FALSE))
	lsrdir = concat_pathes(getval("KPSE_DOT"), lsrdir);

      /* Follow a possible symlink to get the right filesystem.  */
      db_file = concat_pathes(lsrdir, "ls-R");
      if ((symlink_len = readlink(db_file, symlink, PATH_MAX)) != -1) {
	string new_db_file = xmalloc(symlink_len + 1);

	strncpy(new_db_file, symlink, symlink_len);
	new_db_file[symlink_len] = '\0';
	if (!kpse_absolute_p(new_db_file, false)) {
	  string tem = concat_pathes(lsrdir, new_db_file);
	  free(new_db_file);
	  new_db_file = tem;
	}
      }
      db_dir = xdirname(db_file);
      
      if (!test_file('d', db_dir)) {
	fprintf(stderr, "%s: %s is not a directory. Skipping...\n", 
		progname, db_dir);
	continue;
      }

      if (!test_file('w', db_dir)) {
	fprintf(stderr, "%s: %s: directory not writable. Skipping...\n", 
		progname, db_dir);
	continue;
      }

      sprintf(db_dir_tmp, "%s/lsXXXXXX", db_dir);
      if (mktemp(db_dir_tmp) == NULL) {
	perror(db_dir_tmp);
	mt_exit(1);
      }
      if (mkdir(db_dir_tmp, 0777 & ~umask(0)) == -1) {
	fprintf(stderr, "%s: could not create directory '%s'. Skipping...\n");
	continue;
      }

      sprintf(db_file_tmp, "%s/lsXXXXXX", db_dir_tmp); 
      if (mktemp(db_file_tmp) == NULL) {
	perror(db_file_tmp);
	mt_exit(1);
      }
      
      if ((lsrfile = fopen(db_file_tmp, "wb")) == NULL) {
	fprintf(stderr, "%s: %s: no write permission. Skipping...\n",
		progname, db_file_tmp);
	continue;
      }
      if (!is_writable(db_file) && errno != ENOENT) {
	fprintf(stderr, "%s: %s: no write permission. Skipping...\n",
		progname, db_file);
	continue;
      }
      
      if (isatty(fileno(stdout)))
	fprintf(stderr, "%s: Updating %s...\n", progname, db_file);
      
      newfd[0] = 0;
      newfd[1] = fileno(lsrfile);
      newfd[2] = 2;
      push_fd(newfd);
      
      puts(getval("ls_R_magic"));
      fputs("/dev/null", stdout);	/* like `ls -R' does */
      putchar('\n');

      /* Record the info about the temporary file, so we don't enter
         it into the database.  */
      tem_file_info.tem_dir = ".";
      tem_file_info.tem_base = xbasename(db_file_tmp);
      tem_file_info.lsr_base = db_dir;

      pushd(db_dir);
      do_mktexlsr(".");
      
      popd();			/* back to tmpdir */
      /* Restore original stdout */
      pop_fd();
      
      fflush(stdout);
      fclose(lsrfile);

      /* Use same permissions as parent directory, minus x, s, or t bits,
	 but plus writeable bits for those who can read it.  */
      {
	struct stat stbuf;

	if (stat(db_dir, &stbuf) == 0) {
	  if ((stbuf.st_mode & S_IRUSR) != 0)
	    stbuf.st_mode |= S_IWUSR;
	  if ((stbuf.st_mode & S_IRGRP) != 0)
	    stbuf.st_mode |= S_IWGRP;
	  if ((stbuf.st_mode & S_IROTH) != 0)
	    stbuf.st_mode |= S_IWOTH;
	  chmod (db_file_tmp, stbuf.st_mode & 00666);
	}
      }

      /* Forget about this temporary file.  */
      tem_file_info.tem_dir = 0;
      tem_file_info.tem_base = 0;

      if (mvfile(db_file_tmp, db_file) == FALSE) {
	mt_exit(1);
      }

      do_rmdir(db_dir_tmp);

      free(db_file);
    }
  }
  free(newargv);

  hash_reinit();

  if (isatty(fileno(stdout)))
    fprintf(stderr, "%s: Done.\n", progname);
  return 0;
}

/* Find namepart for font name in the TDS structure. */
void findmap()
{
  FILE *f;
  string line;
  string token;
  string filename;
  string name = getval("NAME"), noraw_name;

  /* Raw font */
  if (name && *name == 'r')
    name = name+1;

  filename = getval("MT_SPECIALMAP");
  if (!test_file('r', filename))
    return;

  f = fopen(filename, FOPEN_R_MODE);
  if (f == NULL) {
    perror(filename);
    return;
  }

  /* FIXME : there is a seg fault if the font is not found here */
  /* Is it ok ? 03/02/97 ;.. */
  while((line = read_line(f)) != NULL) {
    token = strtok(line, SEPARATORS);
    if (!token || !*token)
      continue;

    if (FILESTRCASEEQ(token, name)
	|| (FILESTRNCASEEQ(token, name, strlen(token))
	    && isdigit(name[strlen(name)-1])
	    && !isdigit(token[strlen(token)-1]))) {
      setval("SUPPLIER", xstrdup(strtok(NULL, SEPARATORS)));
      setval("TYPEFACE", xstrdup(strtok(NULL, SEPARATORS)));
      free(line);
      break;
    }
    else 
      free(line);
  }

  if (test_file('z', getval("SUPPLIER"))) {
    string s_abbrev = xstrdup(" ");
    /* $NAME might be raw */
    noraw_name = (name[0] == 'r' ? name + 1 : name);
    s_abbrev[0] =  noraw_name[0];
    setval("s_abbrev", s_abbrev);
    filename = getval("MT_SUPPLIERMAP");
    if (test_file('z', filename)) {
      fprintf(stderr, "%s: %s: No such file or directory\n",
	      progname, filename ? filename : empty_str);
      return;
    }
    f  = freopen(filename, FOPEN_R_MODE, f);
    if (f == NULL) {
      perror(filename);
      return ;
    }

    while((line = read_line(f)) != NULL) {
      token = strtok(line, SEPARATORS);
      if (FILECHARCASEEQ(*token, *s_abbrev) && token[1] == '\0') {
	setval("SUPPLIER", xstrdup(strtok(NULL, SEPARATORS)));
	free(line);
	break;
      }
      else
	free(line);
    }

    if (test_file('n', getval("SUPPLIER"))) {
      string t_abbrev = xmalloc (3);

      t_abbrev[0] = '\0';
      strncat (t_abbrev, noraw_name + 1, 2);
      setval("t_abbrev", t_abbrev);
      filename = getval("MT_TYPEFACEMAP");
      if (test_file('z', filename)) {
	fprintf(stderr, "%s: %s: No such file or directory\n",
		progname, filename ? filename : empty_str);
	return;
      }
      f = freopen(filename, FOPEN_R_MODE, f);
      if (f == NULL) {
	perror(filename);
	return;
      }

      while((line = read_line(f)) != NULL) {
	token = strtok(line, SEPARATORS);
	if ((strlen(token) == 2)
	    && (t_abbrev[1] == token[0]) && (t_abbrev[2] == token[1])) {
	  setval("TYPEFACE", xstrdup(strtok(NULL, SEPARATORS)));
	  free(line);
	  break;
	}
	else
	  free(line);
      }
    }
  }

  free(filename);
  fclose(f);
  
  if (KPSE_DEBUG_P(MKTEX_DEBUG)) {
    fprintf(stderr, "Font %s has supplier %s and typeface %s.\n", 
	    getval("NAME"),
	    getval("SUPPLIER") ? getval("SUPPLIER") : empty_str,
	    getval("TYPEFACE") ? getval("TYPEFACE") : empty_str);
  }

}


/* Test if two pathes are equal. Up to the shortest one. */
boolean path_included(const_string p1, const_string p2)
{
  boolean ret = true;

  if (KPSE_DEBUG_P(MKTEX_FINE_DEBUG)) {
    fprintf(stderr, "Path %s included in %s: ", 
	    p1, p2);
  }

  for( ; *p1 && *p2 && ret; p1++, p2++)
    ret = ret && (IS_DIR_SEP(*p1) ? IS_DIR_SEP(*p2) :
		  FILECHARCASEEQ(*p1, *p2));

  if (KPSE_DEBUG_P(MKTEX_FINE_DEBUG)) {
    fprintf(stderr, "%s\n", (ret ? "true": "false"));
  }
  
  return ret;
}

/* return the path element in path which can match dir else NULL */
string path_in_pathes(const_string dir, const_string path)
{
  string ret = NULL;
  string elt;
  string pathcpy = xstrdup(path);
  /* FIXME : why is it necessary to dup it ? */
  string dircpy = xstrdup(dir);

  for (elt = kpse_path_element (pathcpy); 
       elt && !ret;
       elt = kpse_path_element (NULL)) {
    /* Not sufficient to have both equal, there should be a DIR_SEP
       on the next char of dir */
    if (path_included(elt, dircpy) &&
	(IS_DIR_SEP(*(dir+strlen(elt)))
	 || (strlen(dircpy) == strlen(elt))))
      ret = xstrdup(elt);
  }

  if (KPSE_DEBUG_P(MKTEX_FINE_DEBUG)) {
    fprintf(stderr, "Path %s searched for %s => %s.\n", 
	    path, dircpy, ret);
  }
  free(pathcpy);
  free(dircpy);
  return ret;
  
}
       

/*
  Split a .mf name into root part and point size part.
  Root and point size are optional (may be NULL).
  */
void split_mf_name(string name, string *base, string *root, string *ptsize)
{
  string p, q;
  /* name = basename $1 .mf */
  p = my_basename(name, ".mf");
  if (base)
    *base = xstrdup(p);
  /* rootname = `echo $name | sed 's/[0-9]*$//'` */
  for (q = p + strlen(p);  q > p && isdigit(q[-1]); q--);
  /* ptsize = `echo $name | sed 's/^$rootname//'` */
  if (ptsize)
    *ptsize = xstrdup(q);
  *q = '\0';
  if (root)
    *root = p;
  else
    free(p);
}

string find_sauter(string name, string *rootname)
{
  string root, sauter_name, res;
  split_mf_name(name, NULL, &root, NULL);
  sauter_name = concat("b-", root);
  res = kpse_find_file(sauter_name, kpse_mf_format, false);
  if (rootname)
    *rootname = root;
  else
    free(root);
  free(sauter_name);
  return res;
}

/* Return non-zero iff FEATURE is included in $MT_FEATURES.  */
int feature_p(const char *feature)
{
  string mt_features = getval("MT_FEATURES");
  char *p = strstr(mt_features, feature);
  int next_char = p ? p[strlen(feature)] : 0;

  if (p && (p == mt_features || p[-1] == ':')
        && (next_char == ':' || next_char == '\0'))
    return 1;
  return 0;
}

void mktexnam_opt()
{
  string mtn_opt;

  /* FIXME: we don't support edited mktexnam.opt files.  */
  if (feature_p("dosnames"))
    setval("MT_PKBASE", "dpi$DPI/${NAME}.pk");

  if (feature_p("nomode"))
    setval("MODE", "");

  /* This is to avoid running the code below if $TEXMF and/or
     $TEXMFMAIN are set incorrectly.  The code below cannot
     cope with that situation.  (The scripts do not call
     mktexnam.opt in such a case, either.)  */
  mtn_opt = kpse_find_file ("mktexnam.opt", kpse_web2c_format, false);
  if (!mtn_opt || !*mtn_opt)
    return;

  if (feature_p("fontmaps")) {
    setval("MT_NAMEPART", "$MT_SUPPLIER/$MT_TYPEFACE");
    setval("MT_PKDESTREL", "pk/$MT_MODE/$MT_NAMEPART");
    setval("MT_TFMDESTREL", "tfm/$MT_NAMEPART");
    setval("MT_MFDESTREL", "source/$MT_NAMEPART");
    setval_default("MT_SPECIALMAP",
	   kpse_find_file("special.map", kpse_fontmap_format, false));
    setval_default("MT_TYPEFACEMAP",
	   kpse_find_file("typeface.map", kpse_fontmap_format, false));
    setval_default("MT_SUPPLIERMAP",
	   kpse_find_file("supplier.map", kpse_fontmap_format, false));

    if (test_file('z', getval("MT_SUPPLIER"))
	|| test_file('z', getval("MT_TYPEFACE"))
	|| STREQ(getval("MT_SUPPLIER"), "unknown")
	|| STREQ(getval("MT_SUPPLIER"), "unknown")) {

      findmap();
      
      if (test_file('z', getval("SUPPLIER"))) {
	fprintf(stderr, "%s: Could not map source abbreviation %s for %s.\n",
		progname,
		getval("s_abbrev") ? getval("s_abbrev") : empty_str,
		getval("NAME"));
	if (test_file('n', getval("MT_SPECIALMAP")))
	  fprintf(stderr, "%s: Need to update %s?\n", 
		  progname, getval("MT_SPECIALMAP"));
	else
	  fprintf(stderr, "%s: $MT_SPECIALMAP not found!\n", progname);
	/* setval("MT_SUPPLIER", getval("MT_DEFAULT_SUPPLIER")); */
	/* setval("MT_TYPEFACE", getval("MT_DEFAULT_TYPEFACE")); */
      }
      else {
	setval("MT_SUPPLIER", getval("SUPPLIER"));
	if (test_file('z', getval("TYPEFACE"))) {
	  fprintf(stderr, "%s: Could not map typeface abbreviation %s for %s.\n",
		  progname,
		  getval("t_abbrev") ? getval("t_abbrev") : empty_str,
		  getval("NAME"));
	  if (test_file('n', getval("MT_SPECIALMAP")))
	    fprintf(stderr, "%s: Need to update %s?\n",
		    progname, getval("MT_SPECIALMAP"));
	  else
	    fprintf(stderr, "%s: $MT_SPECIALMAP not found!\n", progname);
	  /* setval("MT_TYPEFACE", getval("MT_DEFAULT_TYPEFACE")); */
	}
	else
	  setval("MT_TYPEFACE", getval("TYPEFACE"));
      }
    }
  }

  if (feature_p("stripsupplier"))
    setval("MT_SUPPLIER", "");

  if (feature_p("striptypeface"))
    setval("MT_TYPEFACE", "");

  if (feature_p("varfonts"))
    /* atoi is undefined for empty arguments, so test explicitly.  */
    if (test_file('z', (getval("USE_VARTEXFONTS")))
	|| atoi(getval("USE_VARTEXFONTS")) != 0)
      setval("USE_VARTEXFONTS", "1");
}

void do_mktexnames(string name)
{
  string fullname, rootname, i;
  string stdfontpath, relfmt, namepart;
  string *vars;
  string s_dest;
  int use_alternate = 0;

  setval("NAME", name);
  setval_default("MT_PKDESTDIR", "$MT_DESTROOT/$MT_PKDESTREL");
  setval_default("MT_TFMDESTDIR", "$MT_DESTROOT/$MT_TFMDESTREL");
  setval_default("MT_MFDESTDIR", "$MT_DESTROOT/$MT_MFDESTREL");
  setval_default("MT_PKBASE", "$NAME.${DPI}pk");
  setval_default("MT_TFMBASE", "$NAME.tfm");
  setval_default("MT_MFBASE", "$NAME.mf");
  setval_default("MT_DEFAULT_SUPPLIER", "unknown");
  setval_default("MT_DEFAULT_TYPEFACE", "unknown");
  setval_default("MT_DEFAULT_NAMEPART", "$MT_SUPPLIER/$MT_TYPEFACE");
  setval_default("MT_DEFAULT_PKDESTREL", "pk/modeless/$MT_NAMEPART");
  setval_default("MT_DEFAULT_TFMDESTREL", "tfm/$MT_NAMEPART");
  setval_default("MT_DEFAULT_MFDESTREL", "source/$MT_NAMEPART");
  setval_default("MT_DEFAULT_DESTROOT", getval("KPSE_DOT"));
  
  /* Looking for $name.tfm */
  fullname = kpse_find_file(name, kpse_tfm_format, false);
  if (test_file('z', fullname)) {
    /* If tfm not found, looking for $name.mf */
    fullname = kpse_find_file(name, kpse_mf_format, false);
    if (test_file('z', fullname)) {
      /* Try Sauter font */
      fullname = find_sauter(name, &rootname);
      if (test_file('z', fullname)) {
	if (FILESTRNCASEEQ(rootname, "cs", 2)
		 || FILESTRNCASEEQ(rootname, "lcsss", 5)
		 || FILESTRNCASEEQ(rootname, "icscsc", 6)
		 || FILESTRNCASEEQ(rootname, "icstt", 5)
		 || FILESTRNCASEEQ(rootname, "ilcsss", 6)
		 ) {
	  fullname = kpse_find_file("cscode.mf", kpse_mf_format, false);	  
	}
	else if (strlen(rootname) >= 3
		 && ((FILESTRNCASEEQ(rootname, "wn", 2)
		      && strchr("bBcCdDfFiIrRsStTuUvV", rootname[2]))
		     || (FILESTRNCASEEQ(rootname, "rx", 2)
			 && strchr("bBcCdDfFiIoOrRsStTuUvVxX", rootname[2])
			 && strlen(rootname) >= 4
			 && strchr("bBcCfFhHiIlLmMoOsStTxX", rootname[3]))
		     || ((rootname[0] == 'l' || rootname[0] == 'L')
			 && strchr("aAbBcCdDhHlL", rootname[1])
			 && strchr("bBcCdDfFiIoOrRsStTuUvVxX", rootname[2])))) {
	  char lhname[64];
	  strcpy(lhname, "xxcodes.mf");
	  strncpy(lhname, name, 2);
	  fullname = kpse_find_file(lhname, kpse_mf_format, false);
	}
	else {
	  fullname = kpse_find_file(rootname, kpse_mf_format, false);
	}
      }
    }
  }

  if (test_file('z', fullname)) {
    /* Moved on 99/04/29
       setval("MT_FEATURES", concat(getval("MT_FEATURES"), ":fontmaps")); */
    setval_default("MT_DESTROOT", getval("MT_VARTEXFONTS"));
  }
  else {
    /* Normalize fullname */
    normalize(fullname);
    stdfontpath = kpse_path_expand(concat3(("$TEXMF/fonts"),
					   ENV_SEP_STRING,
					   "$MT_VARTEXFONTS"));
    /* FIXME : what about if fullname is not in stdfontpath ?
       For example: fullname is absolute ... */
    if (test_file('n', i = path_in_pathes(fullname, stdfontpath))) {
      /* We now have a preliminary value for the destination root.
	 MT_DESTROOT="$i"
	 */
      setval_default("MT_DESTROOT", i);
      /*
	# When we're done, relfmt contains one of these:
	# "/source/$MT_NAMEPART/"
	# "/$MT_NAMEPART/source/"
	# while namepart contains the actual namepart.
	*/
      relfmt = xstrdup(fullname);
      /* should match in lower cases letters if fs is case insensitive */
      if (!is_casesensitive(relfmt)) {
	string p = string_to_lower(relfmt);
	string q = string_to_lower(i);
	/* remove the 'i' part and the name */
	vars = grep(concatn("^", q, "(/.*/)[^/]*$", NULL), p, 1);
      }
      else {
	/* remove the 'i' part and the name */
	vars = grep(concatn("^", i, "(/.*/)[^/]*$", NULL), relfmt, 1);
      }
      if (vars) {
	free(relfmt);
	relfmt = xstrdup(vars[1]);
	free(vars);
      }
      else {
	/* grep failed. Shouldn't happen. */
	relfmt = xstrdup("");
      }
      
      {
	string res = "";
	size_t l_relfmt = strlen(relfmt);
	
	namepart = NULL;
	
	/* sed -e 's%^/tfm/%/source/%'  */
	if (FILESTRNCASEEQ(relfmt, "/tfm/", 5)) {
	  res = concat("/source/", relfmt + 5);
	  namepart = concat ("/", relfmt+5);
	  free(relfmt);
	  relfmt = res;
	}
	/* sed -e 's%/tfm/$%/source/%'  */
	else if (l_relfmt >= 5
		 && FILESTRCASEEQ(relfmt + l_relfmt - 5, "/tfm/")) {
	  relfmt[l_relfmt - 4] = '\0';
	  res = concat(relfmt, "/source/");
	  namepart = concat(relfmt, "/");
	  free(relfmt);
	  relfmt = res;
	}
	/* namepart=`echo "$relfmt" | sed -e 's%/source/%/%'` */
      /*
	This should be almost done by now. I assume the scripts not
	to be strict enough. IE: /tfm/ can occur only once in the name.
	That's why the 'else if' instead of 'if'.
	*/
	else {
	  /* namepart is unset by now */
	  namepart = subst(relfmt, "/source/", "/");
	}
	
	/* See if namepart is empty.  */
	if (test_file('n', namepart) && strcmp(namepart, "/") != 0) {
	  res = subst(relfmt, namepart, "/$MT_NAMEPART/");
	  free(relfmt);
	  relfmt = res;
	}
	else {
	  /* Assume TDS */
	  free(relfmt);
	  relfmt = xstrdup("/source/$MT_NAMEPART/");
	  namepart = "";
	}
      }
      
      /* Determine supplier and typeface from namepart. If there is
	 only one part in the name part, we take it to be the typeface. 
	 */
      vars = grep("^/([^/]*)/([^/]*)[/]?$", namepart, 2);
      if (vars) {
	if (vars[2] == NULL || *vars[2] == '\0') {
	  setval("MT_TYPEFACE", xstrdup(vars[1]));
	  setval("MT_SUPPLIER", "");
	}
	else {
	  setval("MT_TYPEFACE", xstrdup(vars[2]));
	  setval("MT_SUPPLIER", xstrdup(vars[1]));
	}
	free(vars[1]);
	if (vars[2]) free(vars[2]);
	free(vars);
      }
      else {
	setval("MT_TYPEFACE", namepart);
	setval("MT_SUPPLIER", namepart);
      }
      /*
	Phew. Now we create the relative paths for pk, tfm and source.
	*/
      setval_default("MT_NAMEPART", "$MT_SUPPLIER/$MT_TYPEFACE");
      setval_default("MT_PKDESTREL", subst(relfmt, "/source/", "/pk/$MT_MODE/"));
      setval_default("MT_TFMDESTREL", subst(relfmt, "/source/", "/tfm/"));
      setval_default("MT_MFDESTREL", relfmt);
    }
  } /* FIXME: if fullname is absolute, path_in_pathes will fail */
  /*
    In case some variables remain unset
    */
  /*  setval_default("MT_DESTROOT", getval("MT_DEFAULT_DESTROOT")); */
  setval_default("MT_SUPPLIER", getval("MT_DEFAULT_SUPPLIER"));
  setval_default("MT_TYPEFACE", getval("MT_DEFAULT_TYPEFACE"));
  setval_default("MT_NAMEPART", getval("MT_DEFAULT_NAMEPART"));
  setval_default("MT_PKDESTREL", getval("MT_DEFAULT_PKDESTREL"));
  setval_default("MT_TFMDESTREL", getval("MT_DEFAULT_TFMDESTREL"));
  setval_default("MT_MFDESTREL", getval("MT_DEFAULT_MFDESTREL"));
  
  /* We have found nothing, so force using the fontmaps as a last resort.
     This also means mktexnam can be queried for advice on where to
     place fonts. */
  if (STREQ(getval("MT_SUPPLIER"), "unknown")
      || STREQ(getval("MT_TYPEFACE"), "unknown")) {
    setval_default("MT_FEATURES", concat(getval("MT_FEATURES"), ":fontmaps"));
  }

  /* Handle the options */
  mktexnam_opt();

  /* # Adjust MT_DESTROOT, if necessary. */
  if (test_file('n', expand_var("$MT_DESTROOT"))) {
    string systexmf;
    /* # Do we have write access and permission?  */
    if (!test_file('w', expand_var("$MT_DESTROOT")))
      use_alternate = 1;
    /* # We distinguish system trees from the rest. */
    systexmf = concat3(kpse_path_expand("{$SYSTEXMF}/fonts"),
		       ENV_SEP_STRING,
		       getval("MT_VARTEXFONTS"));
    i = path_in_pathes(expand_var("$MT_DESTROOT"), systexmf);
					
    if (test_file('n', i) && FILESTRCASEEQ(expand_var("$MT_DESTROOT"), i)) {
      if ((test_file('n', (getval("USE_VARTEXFONTS"))) 
	   && STREQ(getval("USE_VARTEXFONTS"),"1"))
	  || use_alternate) {
	setval("MT_DESTROOT", getval("MT_VARTEXFONTS"));
      }
    }
    /* else $MT_DESTROOT stays put.  */
    else if (use_alternate)
      setval("MT_DESTROOT", "");
  }

  if (test_file('z', expand_var("$MT_DESTROOT"))) {
    string default_dest_root = expand_var("$MT_DEFAULT_DESTROOT");
    setval("MT_DESTROOT", default_dest_root);
    setval("MT_PKDESTDIR", default_dest_root);
    setval("MT_TFMDESTDIR", default_dest_root);
    setval("MT_MFDESTDIR", default_dest_root);
  }

  s_dest = expand_var("$DEST");
  if (test_file('n', s_dest))
    if (kpse_absolute_p(s_dest, false)) {
      setval("MT_PKDESTDIR", s_dest);
      setval("MT_TFMDESTDIR", s_dest);
      setval("MT_MFDESTDIR", s_dest);
      setval("MT_NAMEPART", "");
    }
    else
      setval("MT_NAMEPART", s_dest);
  
  setval ("MT_MODE", expand_var("$MODE"));
  setval ("MT_NAMEPART", expand_var("$MT_NAMEPART"));
  setval ("MT_DESTROOT", expand_var("$MT_DESTROOT"));
  setval ("MT_PKDESTREL", expand_var("$MT_PKDESTREL"));
  setval ("MT_TFMDESTREL", expand_var("$MT_TFMDESTREL"));
  setval ("MT_MFDESTREL", expand_var("$MT_MFDESTREL"));
  setval ("MT_PKDESTDIR", expand_var("$MT_PKDESTDIR"));
  setval ("MT_TFMDESTDIR", expand_var("$MT_TFMDESTDIR"));
  setval ("MT_MFDESTDIR", expand_var("$MT_MFDESTDIR"));
  s_dest = setval ("MT_PKNAME", expand_var(concat_pathes(getval("MT_PKDESTDIR"), 
					      getval("MT_PKBASE"))));
  /* When "dosnames" is in effect, MT_PKDESTDIR should include "/dpiNNN".  */
  if (feature_p("dosnames")) {
    string p = xdirname(s_dest);

    setval ("MT_PKDESTDIR", p);
  }
  setval ("MT_TFMNAME", expand_var(concat_pathes(getval("MT_TFMDESTDIR"),
				      getval("MT_TFMBASE"))));
  setval ("MT_MFNAME", expand_var(concat_pathes(getval("MT_MFDESTDIR"), 
				     getval("MT_MFBASE"))));
}

int mktexnames(int argc, char **argv)
{
  string name = argv[1];

  argc--; argv++; /* We are eliminating argv[0] */
  if (argc <= 2) {
    setval("DPI", getval("BDPI"));
    if (argc == 2)
      setval("DEST", argv[1]);
  }
  else if (argc < 5) {
    setval("DPI", argv[1]);
    setval("MODE", argv[2]);
    if (argc == 4)
      setval("DEST", argv[3]);
  }

  do_mktexnames(name);

  printf("%s%s%s%s%s\n", normalize(getval("MT_PKNAME")), ENV_SEP_STRING,
	 normalize(getval("MT_TFMNAME")), ENV_SEP_STRING,
	 normalize(getval("MT_MFNAME")));
  return 0;
}

int mktexmkdir(int argc, char **argv)
{
  int i;
  string path;

  mt_append_mask = 0;

  /* FIXME: we don't support edited mktexdir.opt files!  */
  if (feature_p("appendonlydir"))
    /* Set append-only bit on created directories. */
    mt_append_mask = 01000;	/*  MT_APPEND_MASK=+t;; */

  for(i = 1; i < argc; i++) {
    /* We have to concat the original wd if argv[i] is not absolute */
    if (kpse_absolute_p(argv[i], false))
      errstatus = do_makedir(argv[i]) != 0;
    else {
      /* original dir is the zeroth directory in the env stack */
      path = concat_pathes(peek_dir(0), argv[i]);
      errstatus = do_makedir(path) != 0;
      free(path);
    }
  }
  return errstatus;
}
int mktexrmdir(int argc, char **argv)
{
  int i;

  for(i = 1; i < argc; i++) {
      do_rmdir(argv[i]);
  }
  return 0;
}

void do_mktexupdate(string dir, char *name)
{
  string lsrdir, relative_dir, dirname;
  string elt, db_file, line;
  FILE *f;

  if (KPSE_DEBUG_P(MKTEX_DEBUG)) {
    fprintf(stderr, "Mktexupd adds %s in %s\n", name, dir);
  }

  normalize(dir);

  if (!test_file('d', dir)) {
    fprintf(stderr, "%s: %s not a directory.\n", progname,
	    test_file('n', dir) ? dir : empty_str);
    mt_exit(1);
  }
  if (!test_file('f', lsrdir = concat_pathes(dir, name))) {
    fprintf(stderr, "%s: %s not a file.\n", progname,
	    test_file('n', lsrdir) ? lsrdir : empty_str);
    mt_exit(1);
  }
  
  /* update the current db too ! */
  dirname = concat_pathes(dir, name);
  if (KPSE_DEBUG_P(MKTEX_FINE_DEBUG)) {
    fprintf(stderr, "Inserting `%s' into current db\n", dirname);
  }
  kpse_db_insert(dirname);
  free(dirname);

  /* This should give us the ls-R default path */
  if (!kpse_format_info[kpse_db_format].type) /* needed if arg was numeric */
    kpse_init_format (kpse_db_format);

  lsrdir = normalize(xstrdup(kpse_format_info[kpse_db_format].path));

  setval("TEXMFLS_R", "");
  /* For each argument in the list */
  for (elt = kpse_path_element(lsrdir);
       elt;
       elt = kpse_path_element(NULL)) {
    if (*elt 
	&& FILESTRNCASEEQ(dir, elt, strlen(elt))) {
      free(lsrdir);
      setval("TEXMFLS_R", lsrdir = xstrdup(elt));
      break;
    }
  }
  if (!test_file('n', getval("TEXMFLS_R")))
    /* just return, it is call by other functions than mktexupd() */
    return;
  db_file = concat_pathes(lsrdir, "ls-R");

  if (!test_file('f', db_file)) {
    char *mtlsr_argv[3];

    mtlsr_argv[0] = "mktexlsr";
    mtlsr_argv[1] = lsrdir;
    mtlsr_argv[2] = NULL;
    mktexlsr(2, mtlsr_argv);
  }
  if(!test_file('w', db_file)) {
    fprintf(stderr, "%s: %s unwritable.\n", progname, db_file);
    mt_exit(1);
  }
  if ((f = fopen(db_file, "r+b")) == NULL) {
    fprintf(stderr, "%s: can't open %s.\n", progname, db_file);
    mt_exit(1);
  }
  if ((line = read_line(f)) == NULL 
      || (strcmp(line, getval("ls_R_magic"))
	  && strcmp(line, getval("old_ls_R_magic")))) {
    fprintf(stderr, "%s: %s lacks magic string `%s'.\n", progname, db_file,
	    getval("ls_R_magic"));
    mt_exit(1);
  }
  free(line);

  /* does the fs preserve case ? */
  if (!is_casesensitive(dir)) {
    string p;
    for(p = dir; *p; p++)
      *p = TOLOWER(*p);
    for(p = name; *p; p++)
      *p = TOLOWER(*p);
  }
  /* Change `$TEXMFLS_R/' to `./'.  */
  if (FILESTRNCASEEQ(dir, lsrdir, strlen(lsrdir)))
    relative_dir = concat_pathes("./", dir + strlen(lsrdir));
  else
    relative_dir = xstrdup(dir);
  fseek(f, 0L, SEEK_END);
  fprintf(f, "%s:\n%s\n", relative_dir, name); 
  /*    fputs(dir, f); fputs(":\n", f);
	fputs(name, f); fputc('\n', f); */
  fclose(f);
  free(relative_dir);
}

int mktexupdate(int argc, char **argv)
{
  do_mktexupdate(argv[1], argv[2]);
  return 0;
}

/* if dirname is a path that can expand into multiple directories,
   return the first directory that exists. */
string expand(const_string dirname)
{
  string path = kpse_path_expand(dirname);
  string elt;

  if (KPSE_DEBUG_P(MKTEX_FINE_DEBUG)) {
    fprintf(stderr, "MktexMF (expand) <= %s\n\t <= %s\n", dirname, path);
  }

  for(elt = kpse_path_element(path); 
      elt && !test_file('d', elt); 
      elt = kpse_path_element(NULL)) {

    if (KPSE_DEBUG_P(MKTEX_FINE_DEBUG)) {
      fprintf(stderr, "MktexMF (expand) => %s\n", elt);
    }
  }
  return (elt != NULL ? xstrdup(elt) : NULL);
}

void do_mktexmf(string font)
{
  string rootname, pointsize, realsize;
  string name;
  string mfname;
  string gen, tmpmf, fullname;
  string sauterroot, rootfile, destdir;
  FILE *f;
  size_t ptsz_len;

  if (KPSE_DEBUG_P(MKTEX_DEBUG)) {
    fprintf(stderr, "MktexMF:\n\tFONT: %s\n", font);
  }

  split_mf_name(font, &name, &rootname, &pointsize);

  if (KPSE_DEBUG_P(MKTEX_FINE_DEBUG)) {
    fprintf(stderr, "name = %s, rootname = %s, pointsize = %s\n",
	    name, rootname, pointsize);
  }      

  sauterroot = kpse_find_file(concat3("b-", rootname, ".mf"),
			      kpse_mf_format, false);

  if (test_file('n', sauterroot)) {
    rootfile = sauterroot;
    rootname = concat("b-", rootname);
  }
  else if (strlen(rootname) == 8
	   && FILESTRNCASEEQ(rootname, "csso12", 6)
	   && isdigit(rootname[6]) /* FIXME: Should be [0-5] actually */
	   && isdigit(rootname[7])) {
    rootfile = xstrdup("");
  }
  else if (FILESTRNCASEEQ(rootname, "cs", 2)
	   || FILESTRNCASEEQ(rootname, "lcsss", 5)
	   || FILESTRNCASEEQ(rootname, "icscsc", 6)
	   || FILESTRNCASEEQ(rootname, "icstt", 5)
	   || FILESTRNCASEEQ(rootname, "ilcsss", 6)
	   ) {
    rootfile = kpse_find_file("cscode.mf",
			      kpse_mf_format, false);
  }
  else if (strlen(rootname) >= 3
	   && ((FILESTRNCASEEQ(rootname, "wn", 2)
		&& strchr("bBcCdDfFiIrRsStTuUvV", rootname[2]))
	       || (FILESTRNCASEEQ(rootname, "rx", 2)
		   && strchr("bBcCdDfFiIoOrRsStTuUvVxX", rootname[2])
		   && strlen(rootname) >= 4
		   && strchr("bBcCfFhHiIlLmMoOsStTxX", rootname[3]))
	       || ((rootname[0] == 'l' || rootname[0] == 'L')
		   && strchr("aAbBcCdDhHlL", rootname[1])
		   && strchr("bBcCdDfFiIoOrRsStTuUvVxX", rootname[2])))) {
    char lhprefix[64];
    strcpy(lhprefix, "xxcodes.mf");
    strncpy(lhprefix, name, 2);
    rootfile = kpse_find_file(lhprefix,
			      kpse_mf_format, false);
  }
  else {
    string tem;
    rootfile = kpse_find_file(tem = concat(rootname, ".mf"),
			      kpse_mf_format, false);
    free(tem);
  }

  if (test_file('z', rootfile)) {
    fprintf (stderr, "%s: empty or non-existent rootfile!\n", progname);
    mt_exit(1);
  }
  if (!test_file('f', rootfile)) {
    fprintf (stderr, "%s: rootfile %s does not exist!\n", progname, rootfile);
    mt_exit(1);
  }

  do_mktexnames(name);
  destdir = xdirname(getval("MT_MFNAME"));
  if (!test_file('d', destdir))
    if (do_makedir(destdir))
      mt_exit(1);

  /* Changing directory */
  pushd(destdir);

  ptsz_len = strlen(pointsize);
  if (ptsz_len == 0) {
    fprintf(stderr, "%s: no pointsize.\n", progname);
    mt_exit(1);
  } else if (ptsz_len == 2) {
    if (pointsize[0] == '1' && pointsize[1] == '1')
      realsize = "10.95";		/* \magstephalf */
    else if (pointsize[0] == '1' && pointsize[1] == '4')
      realsize = "14.4";		/* \magstep2 */
    else if (pointsize[0] == '1' && pointsize[1] == '7')
      realsize = "17.28";		/* \magstep3 */
    else if (pointsize[0] == '2' && pointsize[1] == '0')
      realsize = "20.74";		/* \magstep4 */
    else if (pointsize[0] == '2' && pointsize[1] == '5')
      realsize = "24.88";		/* \magstep5 */
    else if (pointsize[0] == '3' && pointsize[1] == '0')
      realsize = "29.86";		/* \magstep6 */
    else if (pointsize[0] == '3' && pointsize[1] == '6')
      realsize = "35.83";		/* \magstep7 */
    else
      realsize = pointsize;
  }
  /* The new convention is to have three or four letters for the
     font name and four digits for the pointsize. The number is
     pointsize * 100. We effectively divide by 100 by inserting a
     dot before the last two digits.  */
  else if (ptsz_len == 4 || ptsz_len == 5) {
    /* realsize=`echo "$pointsize" | sed 's/\(..\)$/.\1/'` */
    realsize = (string)xmalloc(ptsz_len + 2);
    strcpy(realsize, pointsize);
    /* The script doesn't check for last chars being digits, but we do!  */
    if (isdigit(realsize[ptsz_len-1])
	&& isdigit(realsize[ptsz_len-2])) {
      realsize[ptsz_len+1] = '\0';
      realsize[ptsz_len]   = realsize[ptsz_len-1];
      realsize[ptsz_len-1] = realsize[ptsz_len-2];
      realsize[ptsz_len-2] = '.';
      free(pointsize);
    }
  } else realsize = pointsize;

  mfname = concat(name, ".mf");
  if (test_file('r', mfname)) {
    gen = normalize(concat_pathes(destdir, mfname));
    fprintf(stderr, "%s: %s already exists.\n", progname, gen);
    fprintf(fout, "%s\n", gen);
    free(gen);
    do_mktexupdate(destdir, mfname);
    return;	/* in case called by others */
  }

  /* FIXME: We have to expand fontdir and restrain it to the first 
     valid directory. */
  tmpmf = xstrdup("mfXXXXXX");
  if( mktemp(tmpmf) == NULL ) {
    fprintf(stderr, "%s: can't create temp filename in %s.\n", progname,
	    destdir);
    mt_exit(1);
  }
  if ((f = fopen(tmpmf, "w")) == NULL) {
    fprintf(stderr, "%s: can't write into temp filename %s/%s.\n",
	    progname, destdir, tmpmf);
    mt_exit(1);
  }

  if (FILESTRNCASEEQ(name, "ec", 2)
      || FILESTRNCASEEQ(name, "tc", 2)) {
    fprintf(f, "if unknown exbase: input exbase fi;\n");
    fprintf(f, "gensize:=%s;\ngenerate %s;\n", realsize, rootname);
  }
  else if (FILESTRNCASEEQ(name, "dc", 2)) {
    fprintf(f, "if unknown dxbase: input dxbase fi;\n");
    fprintf(f, "gensize:=%s;\ngenerate %s;\n", realsize, rootname);

  }
  else if (FILESTRNCASEEQ(name, "cs", 2)
	   || FILESTRNCASEEQ(name, "lcsss", 5)
	   || FILESTRNCASEEQ(name, "icscsc", 6)
	   || FILESTRNCASEEQ(name, "icstt", 5)
	   || FILESTRNCASEEQ(name, "ilcsss", 6)
	   ) {
    fprintf(f, "input cscode\nuse_driver;\n");
  }
  else if (strlen(name) >= 3
	   && ((FILESTRNCASEEQ(name, "wn", 2)
		&& strchr("bBcCdDfFiIrRsStTuUvV", name[2]))
	       || (FILESTRNCASEEQ(name, "rx", 2)
		   && strchr("bBcCdDfFiIoOrRsStTuUvVxX", name[2])
		   && strlen(name) >= 4
		   && strchr("bBcCfFhHiIlLmMoOsStTxX", name[3]))
	       || ((name[0] == 'l' || name[0] == 'L')
		   && strchr("aAbBcCdDhHlL", name[1])
		   && strchr("bBcCdDfFiIoOrRsStTuUvVxX", name[2])))) {
    fprintf(f, "input fikparm;\n");
  }
  else if (strlen(name) >= 4 && strchr("gG", name[0])
	   && strchr("oOrRsStTlL", name[1]) 
	   && strchr("iIjJmMtTwWxX", name[2]) 
	   && strchr("cCiInNoOuU", name[3])) {
    /* The CB font family; the test matches (case insensitively) the
       pattern `g[lorst][ijmtwx][cinou]*'. 
       This is only slightly more general than the exact set of patterns.  */
    fprintf(f, "gensize:=%s;\ninput %s;\n", realsize, rootname);
  }
  else {
    /* FIXME: this was in the previous versions */
    /*      fprintf(tmpfile, "if unknown %s: input %s fi;\n", base, base); */
      fprintf(f, "design_size := %s;\ninput %s;\n",
	      realsize, rootname);
  }
  fclose(f);
  chmod_as_dir(tmpmf, ".", 00644);
  unlink(mfname);
  if (mvfile(tmpmf, mfname) == FALSE) {
    mt_exit(1);
  }
  setval("destdir", destdir);
  setval("mfname", mfname);
  fullname = normalize(concat_pathes(destdir, mfname));
  fprintf(fout, "%s\n", fullname);
  fprintf(stderr, "%s: %s: successfully generated.\n", progname, fullname);
  free(fullname);
  do_mktexupdate(destdir, mfname);
}

int mktexmf(int argc, char **argv)
{
  start_redirection(false);
  do_mktexmf(argv[1]);
  return 0;
}

void do_mktextex(const_string texname)
{
  string fullname, destdir;

  /*
    We may try to get this file directly from a remote repository.
  */
#if 0
  fprintf(fout, "%s\n", 
	  fullname = normalize(concat_pathes(destdir, texname)));
  fprintf(stderr, "%s: %s: successfully generated.\n", progname, fullname);
#endif
}

int mktextex(int argc, char **argv)
{
  start_redirection(false);
  do_mktextex(argv[1]);
  return 0;
}

boolean check_for_strange_path(string name)
{
  FILE *f;
  string line;
  string strange1 = "! Strange path";
  string strange2 = "! bad pos";
  string strange3 = "! angle(0,0) is taken as zero.";
  boolean res = false;	/* in case there are no lines which begin with `!' */

  if ((f = fopen(name, "r")) == NULL) {
    fprintf(stderr, "%s: warning: can't open log file %s.\n", progname, name);
    return false;
  }
  while ((line  = read_line(f)) != NULL) {
    if (*line == '!') {
      if (strncmp(line, strange1, strlen(strange1))
	  && strncmp(line, strange2, strlen(strange2))
	  && strncmp(line, strange3, strlen(strange3))
	  ) {
	res = false;
	break;
      }
      else if (res == false)
	res = true;	/* seen at least one ``Strange path'' message */
    }
  }
  fclose(f);
  return res;
}


void do_mktextfm(string font, string destdir)
{
  string fullname, name;
  char tfmtempname[PATH_MAX];
  char pktempname[PATH_MAX];
  char cmd[256];
  int retcode;
  
  setval("NAME", name = my_basename(font, ".tfm"));
  setval("MAG", "1");
  setval("DEST", destdir);
  setval("DPI", getval("BDPI"));
  
  do_mktexnames(name);
  
  setval("PKDEST", getval("MT_PKNAME"));
  setval("TFMDEST", getval("MT_TFMNAME"));
  setval("PKDESTDIR", getval("MT_PKDESTDIR"));
  setval("TFMDESTDIR", getval("MT_TFMDESTDIR"));
  setval("PKNAME", (string)xbasename(getval("MT_PKNAME")));
  setval("TFMNAME", concat(name, ".tfm"));
  /* FIXME: can e.g. cmr10.600gf clash with e.g. cmr10.600whatever on DOS?  */
  setval("GFNAME", concatn(name, ".", getval("DPI"), "gf", NULL));
  
  fullname = expand_var("$TFMDESTDIR/$TFMNAME");
  if (test_file('r', fullname)) {
    fprintf(stderr, "%s: %s already exists.\n", progname, fullname);
    fprintf(fout, "%s\n", fullname);
    free(fullname);
    do_mktexupdate(getval("TFMDESTDIR"), getval("TFMNAME"));
    return;
  }
  free(fullname);
      
  do_makedir(getval("TFMDESTDIR"));
  if (!test_file('d', getval("TFMDESTDIR"))) {
    fprintf(stderr, "%s: mktexdir %s failed.\n",
	    progname, getval("TFMDESTDIR"));
    mt_exit(1);
  }

  /* Protect the semi-colons from being interpreted by the shell.
     (Use double quotes since they are supported on non-Unix platforms.)  */
  sprintf(cmd, "mf \"\\mode:=%s; mag:=%s; nonstopmode; input %s\"",
	  getval("MODE"), getval("MAG"), getval("NAME"));

  fprintf(stderr, "%s: Running %s\n", progname, cmd);

  /* Now run metafont.
     Note that stdin has been redirected to "/dev/null" by `main'. */
  retcode = system(cmd);

  if (retcode != 0) {
    if (retcode == -1) {
      perror(cmd);
      mt_exit(1);
    }
    else if (check_for_strange_path(expand_var("$NAME.log")) == true) {
      fprintf(stderr, "%s: warning: `%s' caused strange path errors.\n",
	      progname, cmd);
    }
    else {
      struct stat statbuf;
      fprintf(stderr, "%s: `%s' failed.\n", progname, cmd);
      if (stat(expand_var("$NAME.log"), &statbuf) == 0
	  && statbuf.st_size > 0)
	mvfile(expand_var("$NAME.log"),
	       concat_pathes(getval("KPSE_DOT"), expand_var("$NAME.log")));
      mt_exit(1);
    }
  }
  
  if (!test_file('r', getval("TFMNAME"))) {
    fprintf(stderr, "%s: `%s' failed to make %s.\n",
	    progname, cmd, getval("TFMNAME"));
    mt_exit(1);
  }

  /* Install the TFM file carefully, since others
     may be working simultaneously.  */
  sprintf(tfmtempname, "%s/tfXXXXXX", getval("TFMDESTDIR"));
  mktemp(tfmtempname);
  if (mvfile(getval("TFMNAME"), tfmtempname) == FALSE) {
    mt_exit(1);
  }

  pushd(getval("TFMDESTDIR"));
  chmod_as_dir(tfmtempname, ".", 00644);
  if (!test_file('r', getval("TFMNAME"))) {
    char *tfmtmp_base = tfmtempname + strlen(getval("TFMDESTDIR")) + 1;

    if (mvfile(tfmtmp_base, getval("TFMNAME")) == FALSE) {
      mt_exit(1);
    }
  }

  /* OK, success with the TFM.  */
  do_mktexupdate(getval("TFMDESTDIR"), getval("TFMNAME"));
  fullname = normalize(concat_pathes(getval("TFMDESTDIR"), getval("TFMNAME")));
  fprintf(fout, "%s\n", fullname);

  fprintf(stderr, "%s: %s: successfully generated.\n", progname,
	  fullname);

  /* Since we probably made a GF(->PK) file, too, may as well install
     it if it's needed.  */
  popd();			/* Back to $TEMPDIR */
  if (test_file('r', getval("GFNAME"))
      && !test_file('f', concat_pathes(getval("PKDESTDIR"),
				       getval("PKNAME")))) {
    char *pktmp_base;

    sprintf(cmd, "gftopk ./%s %s", getval("GFNAME"), getval("PKNAME"));
    if (KPSE_DEBUG_P(MKTEX_DEBUG)) {
      fprintf(stderr, "Running: %s\n", cmd);
    }
    retcode = system(cmd);
    if (retcode != 0) {
      if (retcode == -1)
	perror("gftopk");
      else
	fprintf(stderr, "%s: gftopk failed to make %s.\n",
		progname, getval("PKNAME"));
      mt_exit(1);
    }
    
    do_makedir(getval("PKDESTDIR"));
    sprintf(pktempname, "%s/pkXXXXXX", getval("PKDESTDIR"));
    mktemp(pktempname);
    if (mvfile(getval("PKNAME"), pktempname) == FALSE) {
      mt_exit(1);
    }
    pushd(getval("PKDESTDIR"));
    pktmp_base = pktempname + strlen(getval("PKDESTDIR")) + 1;
    if (test_file('f', getval("PKNAME")))
      remove(pktmp_base);
    else {
      chmod_as_dir(pktmp_base, ".", 00644);
      if (mvfile(pktmp_base, getval("PKNAME")) == FALSE) {
	mt_exit(1);
      }
      do_mktexupdate(getval("PKDESTDIR"), getval("PKNAME"));
    }
    popd();
  }
}

/* this is similar to mktexpk ... */
int mktextfm(int argc, char **argv)
{
  string dest = getval("DEST");
  string font = argv[1];

  /* Where do potential driver files belong?
     case "$MT_FEATURES" in
     *nomfdrivers*)
     : ${MT_MFDESTDIR=`pwd`}
     export MT_MFDESTDIR;;
     esac
  */
  if (feature_p("nomfdrivers")) {
    setval_default("MT_MFDESTDIR", xgetcwd());
  }

  start_redirection(false);
  do_mktextfm(font, dest);
  return 0;
}

int get_mode_from_mf(string mode)
{
  FILE *fpipe;
  char buf[1024];
  int result = 0;

  sprintf(buf, "mf \"\\mode:=%s;mode_setup;message\\\"BDPI= \\\"&decimal round pixels_per_inch;end.\"", mode);

  /* METAFONT's stdin was redirected to /dev/null by `main'.  */
  fpipe = popen(buf, "r");
  if (fpipe == NULL) {
    perror("pipe to mf");
    return 0;
  }

  /* awk '/DPI=/ {print $2}' */
  while (fgets(buf, sizeof buf, fpipe)) {
    char *p = strstr(buf, "DPI= ");
    if (p)
      result = atoi(p + 5);	/* could break here, but AWK script doesn't */
  }
  if (pclose(fpipe))
    perror("running mf");
  return result;
}

/* A common lossage with stock DOS/Windows shells is that `system("foo")'
   returns zero even though `foo' doesn't exist as an executable.  A better
   way is to say `system("foo.exe")', but that is non-portable to Unix.

   Thus the need for this function.  It is a no-op (always returning TRUE)
   on Unix, but on DOS/Windows actually looks for its argument along the
   PATH.  It is assumed that on Unix, the call to `system' will fail as God
   intended if the program is not accessible via PATH.  */
boolean find_exe(char *progname)
{
  char path[PATH_MAX];
#ifdef _WIN32
  char *fp;
  return (SearchPath(NULL, progname, ".exe", PATH_MAX, path, &fp) != 0);
#else
# ifdef __DJGPP__
  extern char * __dosexec_find_on_path (const char *, char **, char *);
  extern char **environ;
  return __dosexec_find_on_path (progname, environ, path) != NULL;
# else
  return true;
# endif
#endif
}

/*
  We will read the `modes.mf' and gather the various modes.
*/
#define MAX_MODES 64

struct mf_mode
{
  string mnemonic;
  string description;
  int hor_res;
  int vert_res;
};

static struct mf_mode *modes;
static int nb_modes;

#define SKIP_NON_DIGIT(p) while(!isdigit(*p)) p++
#define SKIP_ALPHA(p) while(isalpha(*p)) p++;
#define SKIP_SPACE(p) while(isspace(*p)) p++;
#define SKIP_LINE(p) while(*p && *p != '\n') p++;

static boolean __cdecl
cmp_mf_mode(const struct mf_mode *m1, const struct mf_mode *m2) {
  return (strcmp(m1->description, m2->description) < 0);
}

static boolean
read_modes()
{
  string modes_file_name = kpse_find_file("modes.mf", kpse_mf_format, true);
  FILE *f;
  string line;
  boolean in_mode_def = false;
  int size_modes = MAX_MODES;
  
  /* modes will keep this value if modes.mf can't be found or read. */
  modes = (struct mf_mode *)-1;
  if (modes_file_name  == NULL)
    return false;
  if((f = fopen(modes_file_name, "r")) == NULL) {
    free(modes_file_name);
    return false;
  }

  modes = xmalloc(size_modes * sizeof(struct mf_mode));
    
  while((line = read_line(f)) != NULL) {
    if (in_mode_def) {
      char * cp;
      if (strncmp(line, "enddef;", 7) == 0) {
	nb_modes++;
	if (nb_modes == MAX_MODES) {
	  size_modes *= 2;
	  modes = xrealloc(modes, size_modes * sizeof(struct mf_mode));
	}
	in_mode_def = false;
      }
      else if (modes[nb_modes].hor_res == 0 
	       && (cp = strstr(line, "pixels_per_inch"))) {
	SKIP_NON_DIGIT (cp);
	modes[nb_modes].hor_res = atoi(cp);
	modes[nb_modes].vert_res = atoi(cp);
      }
      else if (cp = strstr(line, "aspect_ratio")) {
	SKIP_NON_DIGIT (cp);
	modes[nb_modes].vert_res = atoi(cp);
      }
    }
    else if (strncmp(line, "mode_def", 8) == 0) {
      char *mode_name, *printer_name;
      char *cp = line + 8;
      while (! isalpha(*cp) && *cp != '\n')
	cp++;
      if (! isalpha (*cp))
	goto line_done;
      mode_name = cp;
      SKIP_ALPHA (cp);
      if (*cp == 0)
	goto line_done;
      *cp++ = 0;
      if (strcmp(mode_name, "help") == 0)
	goto line_done;
      cp = strstr(cp, "%\\[");
      if (cp == 0)
	goto line_done;
      cp += 3;
      SKIP_SPACE (cp);
      if (cp == 0)
	goto line_done;
      printer_name = cp;
      SKIP_LINE (cp);
      *cp = 0;
      in_mode_def = true;
      
      modes[nb_modes].mnemonic = xstrdup(mode_name);
      modes[nb_modes].description = xstrdup(printer_name);
      modes[nb_modes].hor_res = 0;
    }
  line_done:
    free(line);
  }
  qsort(modes, nb_modes, sizeof(struct mf_mode), cmp_mf_mode);
  return true;
}

string
get_mf_mode(int dpi)
{
  int i;

  if ((modes == NULL && !read_modes())
      || (modes == (struct mf_mode *)-1))
    return NULL;

  for (i = 0; i < nb_modes; i++) {
    if (modes[i].hor_res == dpi)
      return modes[i].mnemonic;
  }
  return NULL;
}

int mktexpk(int argc, char **argv)
{
  FILE *psmapfile;
  string psmapfilename, fullname;
  string line = NULL, pktempname;
  string cmd = "", gfcmd;
  string aname, mfname;
  string *vars;
  int mf_bdpi, ibdpi, retcode;
  boolean has_ttf2pk = false, has_hbf2gf = false;

  /* Where do potential driver files belong?
     case "$MT_FEATURES" in
     *nomfdrivers*)
     : ${MT_MFDESTDIR=`pwd`}
     export MT_MFDESTDIR;;
     esac
  */
  if (feature_p("nomfdrivers")) {
    setval_default("MT_FEATURES", xgetcwd());
  }

  if (argc == 3) {
    char *ep = NULL;
    int fd = - 1;

    if (argv[1][0] ==  '>' && argv[1][1] == '&' && isdigit(argv[1][2]))
      fd = strtoul(argv[1] + 2, &ep, 0);
    if (!ep || *ep || fd > 255 || fd < 0)
      fprintf(stderr, "%s: argument '%s' ignored - bad file number\n",
	      progname, argv[2]);
    else if (fd != 1)
      /* The actual redirection happens before we print the names
	 of generated file(s).  */
      record_output_handle(fd);
  }

  start_redirection(false);

  setval("NAME", argv[1]);

  has_ttf2pk = find_exe("ttf2pk");
  has_hbf2gf = find_exe("hbf2gf");

  if (KPSE_DEBUG_P(MKTEX_DEBUG)) {
    fprintf(stderr, "MktexPK:\n\tName: %s\n\tDpi: %s\n\tBdpi: %s\n\tMag: %s\n\tMode: %s\n\tDest: %s\n", getval("NAME"), getval("DPI"), getval("BDPI"), 
	    getval("MAG"), getval("MODE"), getval("DEST"));
  }
  
  mfname = concat(getval("NAME"), ".mf");
  cmd = expand_var("mktexmf $NAME");

  if ((kpse_find_file(mfname, kpse_mf_format, false) != NULL) 
      || (system(cmd) == 0)) {
    
    cmd = NULL; /* reset cmd */
    if (test_file('n', getval("MODE"))) {
      if ((mf_bdpi = get_mode_from_mf(getval("MODE"))) 
	  != atoi(getval("BDPI"))) {
	fprintf(stderr, "%s: Mismatched mode %s and resolution %s; ignoring mode.\n", progname, getval("MODE"), getval("BDPI"));
	setval("MODE", "");
      }
    }

    if (test_file('z', getval("MODE")) 
	|| strcmp(getval("MODE"), "default") == 0) {
      ibdpi = atoi(getval("BDPI"));
      switch (ibdpi) {
      case 85:
	setval("MODE", "sun");
	break;
      case 100:
	setval("MODE", "nextscrn");
	break;
      case 180:
	setval("MODE", "toshiba");
	break;
      case 300:
	setval("MODE", "cx");
	break;
      case 400:
	setval("MODE", "nexthi");
	break;
      case 600:
	setval("MODE", "ljfour");
	break;
      case 720:
	setval("MODE", "epshi");
	break;
      case 1270:
	setval("MODE", "linoone");
	break;
      default: {
	string mode = get_mf_mode(ibdpi);
	if (mode) {
	  setval("MODE", mode);
	}
	else {
	  fprintf (stderr, "%s: Can't guess mode for %s dpi devices.\n",
		   progname, getval("BDPI"));
	  fprintf (stderr, "%s: Use a config file, or update me.\n",
		   progname);
	  mt_exit(1);
	}
      }
      }
    }
    /* Run Metafont. Always use plain Metafont, since reading cmbase.mf */
    /* does not noticeably slow things down. */
    cmd=expand_var("mf \"\\mode:=$MODE; mag:=$MAG; nonstopmode; input $NAME\"");
  }
  else {
    cmd = NULL; /* reset cmd */
    setval("MODE", "modeless");
    if (FILESTRCASEEQ(getval("ps_to_pk"), "gsftopk")) {
#if 0
      if (KPSE_DEBUG_P(MKTEX_DEBUG)) {
	printf("Test: system -> %d win32_system -> %d\n",
	       system(expand_var("$ps_to_pk -t $NAME")),
	       win32_system(expand_var("$ps_to_pk -t $NAME")));
      }
#endif
      if (system(expand_var("$ps_to_pk -t $NAME"))==0) {
	cmd = expand_var("$ps_to_pk $NAME $DPI");
      }
    }
    else if (FILESTRCASEEQ(getval("ps_to_pk"), "ps2pk")) {
      string pstmp;
      /* look for name in psfont.map.
	 Can't use `kpse_open_file' because it exits if the file doesn't
	 exits, and we have a special exit procedure which cleans up.  */
      pstmp = kpse_find_file("ps2pk.map", kpse_fontmap_format, false);
      if (test_file('z', pstmp))
	pstmp = kpse_find_file("psfonts.map", kpse_fontmap_format, false);
      psmapfilename = setval_default("PSMAPFILE", pstmp);
      if (test_file('n', pstmp)) free(pstmp);
      
      if (test_file('n', psmapfilename)) {
	string pattern = concat3("^", getval("NAME"), "([ \t]|$)");
	
	psmapfile = fopen(psmapfilename, "r");
	if (psmapfile == NULL) oops("Cannot open file %s.", psmapfilename);
	while ((line = read_line(psmapfile)) != NULL) {
	  if ((vars = grep(pattern, line, 1)) != NULL)
	    break;
	  free(line);
	  line = NULL;
	}
	free(pattern);
	fclose(psmapfile);
      }
      else
	fprintf(stderr, "%s: no `psfonts.map' file.\n", progname);
      
      if (line != NULL) {
	/* ps_to_pk is set in mktex.opt */
	string encoding, psname, slant, extend;
	encoding = psname = slant = extend = "";
	/* FIXME: mktexpk script has a loop here which effectively
	   looks for the LAST matching string.  Is it important?  */
	if ((vars = grep("([^ \t<[\"]*.enc)", line, 1)))
	  encoding = concat ("-e", vars[1]);
	if ((vars = grep("([^ \t<[\"]*.pf[ab])", line, 1)))
	  psname = vars[1];
	if ((vars = grep("\" ([^ \t]+)SlantFont\"", line, 1)))
	  slant = concat ("-S ", vars[1]);
	if ((vars = grep("\" ([^ \t]+)ExtendFont\"", line, 1)))
	  extend = concat ("-E", vars[1]);
	/* This is looking for .pfa *and* .pfb with the 7.2 kpathsea. */
	if (test_file('z', psname)) {
	  psname = kpse_find_file(getval("NAME"), kpse_type1_format, false);
	}
	if (test_file('z', psname)) {
	  aname = xstrdup(getval("NAME"));
	  if (strlen(aname) >= 2
	      && aname[strlen(aname)-2] == '8' 
	      && aname[strlen(aname)-1] == 'r')
	    /* Guessing the name of the type1 font file as fallback: */
	    aname[strlen(aname)-1] = 'a';
	  psname = kpse_find_file(aname, kpse_type1_format, false);
	  free(aname);
	}
	if (test_file('n', psname)) {
	  /* In fact, ps2pk can't cope with the full pathname, so : */
	  psname = xbasename(psname);
	  cmd = concatn( expand_var("ps2pk -v -X$DPI -R$BDPI"), 
			 slant, " ", extend, " ", encoding,
			 " ", psname, " ", expand_var("$NAME.${DPI}pk"), NULL);
	}
	else {
	  cmd = expand_var("gsftopk -t $NAME");
	  if (system(cmd) == 0) {
	    cmd = NULL; /* reset cmd */
	    fprintf(stderr, 
		    "%s: cannot find %s.pfa or %s.pfb. Trying gsftopk.\n",
		    progname, getval("NAME"), getval("NAME"));
	    cmd = expand_var("gsftopk $NAME $DPI");
#if 0
	    /* make sure that gsftopk uses the psline from our mapfile: */
	    {
	      FILE *f;
	      if ((f = fopen("./psfonts.map", "w")) != NULL) {
		fprintf(f, "%s\n", line);
		fclose(f);
		setval("TEXFONTMAP", expand_var(".;$TEXFONTMAP"));
	      }
	      else {
		/* Can't write in the current directory ? Strange ...
		   Anyway, let's go on and home we will find the right
		   psfonts.map ... */
	      }
	    }
#endif
	  }
	  else {
	    cmd = NULL; /* reset cmd */
	  }
	}
      }
    }
    
    /* unsupported by $ps_to_pk, try other conversions: */
    if (test_file('z', cmd)) {
      if (has_ttf2pk && system(expand_var("ttf2pk -t -q $NAME")) == 0) {
	cmd = expand_var("ttf2pk -q $NAME $DPI");
      }
      else if (has_hbf2gf && system(expand_var("hbf2gf -q -t $NAME")) == 0) {
	cmd = expand_var("hbf2gf -g $NAME $DPI");
      }
      else {
	fprintf(stderr, "%s: don't know how to create bitmap font for %s.",
		progname, getval("NAME"));
	mt_exit(1);
      }
    }
  }
  
  /* Put files into proper place */
  do_mktexnames(getval("NAME"));

  setval("PKDEST", getval("MT_PKNAME"));
  setval("PKDESTDIR", getval("MT_PKDESTDIR"));
  setval("PKNAME", (string)xbasename(getval("PKDEST")));
  setval("GFNAME", expand_var("${NAME}.${DPI}gf"));

  if (test_file('r', fullname = expand_var("$PKDESTDIR/$PKNAME"))) {
    fprintf(stderr, "%s: %s already exists.\n",
	    progname, fullname);
    fprintf(fout, "%s\n", fullname);
    free(fullname);
    do_mktexupdate(getval("PKDESTDIR"), getval("PKNAME"));
    return 0;
  }

  do_makedir(getval("PKDESTDIR"));
  if (!test_file('d', getval("PKDESTDIR"))) {
    fprintf(stderr, "%s: mktexdir %s failed.\n",
	    progname, getval("PKDESTDIR"));
    mt_exit(1);
  }

  printf("%s: Running %s\n", progname, cmd);

  /* Now run metafont.
     Note that stdin has been redirected to "/dev/null" by `main'. */
  retcode = system(cmd);
  if (retcode != 0) {
      fprintf(stderr, "command %s return %d\n", cmd, retcode);
    if (retcode == -1) {
      perror("mf");
      mt_exit(1);
    }
    else if (check_for_strange_path(expand_var("$NAME.log")) == true) {
      fprintf(stderr, "%s: warning: `%s' caused strange path errors.\n",
	      progname, cmd);
    }
    else {
      struct stat statbuf;
      fprintf(stderr, "%s: `%s' failed.\n", progname, cmd);
      if (stat(expand_var("$NAME.log"), &statbuf) == 0
	  && statbuf.st_size > 0)
	mvfile(expand_var("$NAME.log"),
	       concat_pathes(getval("KPSE_DOT"), expand_var("$NAME.log")));
      mt_exit(1);
    }
  }
  
  if (test_file('r', getval("GFNAME"))) {
    gfcmd = expand_var("gftopk ./$GFNAME $PKNAME");
    retcode = system(gfcmd);
    if (retcode == -1) {
      perror("gftopk");
      mt_exit(1);
    }
    else if (retcode != 0) {
      fprintf(stderr, "%s: `%s' failed.\n", progname, gfcmd);
      mt_exit(1);
    }
  }

  /* FIXME : What about the file is created in the current directory ? */
  if (!test_file('f', getval("PKNAME"))
      && test_file('f', expand_var("$NAME.${DPI}pk")))
    mvfile(expand_var("$NAME.${DPI}pk"), getval("PKNAME"));

  if (!test_file('s', getval("PKNAME"))) {
    fprintf(stderr, "%s: `%s' failed to make %s.\n",
	    progname, cmd, getval("PKNAME"));
    mt_exit(1);
  }
  
  pktempname = concat_pathes(getval("PKDESTDIR"), "pkXXXXXX");
  mktemp(pktempname);
  if (mvfile(getval("PKNAME"), pktempname) == FALSE) {
    mt_exit(1);
  }

  pushd(getval("PKDESTDIR"));
  chmod_as_dir(pktempname, ".", 00644);
  if (!test_file('r', getval("PKNAME")))
    if (mvfile(pktempname, getval("PKNAME")) == FALSE) {
      mt_exit(1);
    }

  /* OK, success with the PK.  */
  do_mktexupdate(getval("PKDESTDIR"), getval("PKNAME"));
  fullname = normalize(concat_pathes(getval("PKDESTDIR"),
				     getval("PKNAME")));
  fprintf(fout, "%s\n", fullname);
  fprintf(stderr, "%s: %s: successfully generated.\n", progname,
	  fullname);
  free(fullname);

  popd();
  return 0;
}
