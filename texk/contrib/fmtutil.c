/* fmtutil.c: this program emulates the fmtutil shell script from tetex.

Copyright (C) 1998 Fabrice POPINEAU.

Time-stamp: <04/03/20 16:03:00 popineau>

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
#define MAX_LINES 32

#define SEPARATORS " \t\n\""

#include <signal.h>

#include <kpathsea/kpathsea.h>
extern KPSEDLL char* kpathsea_version_string;

#include "mktex.h"
#include "fileutils.h"
#include "stackenv.h"
#include "variables.h"
#include "fmtutil.h"

#ifdef _WIN32
#define isatty _isatty
#else
#include <glob.h>
#endif

extern string fmtutil_version_string;

/* 
   Global Variables found in the fmtutil script
*/

static string texmfmain;
static string tempenv;
static string cwd;

static string mode, bdpi;

/* fmtutil.cnf */

static boolean downcase_names;
char cmd_buf[1024];		/* Hope it is enough for handling command */
char cmd_sys[1024];		/* system() calls */
char msg_buf[1024];

static int errstatus;
static string log_failure_msg = NULL;

static void usage(void);

/* Test whether getopt found an option ``A''.
   Assumes the option index is in the variable `option_index', and the
   option table in a variable `long_options'.  */
#define ARGUMENT_IS(a) STREQ (long_options[option_index].name, a)

static struct option long_options [] = {
    { "all",               0, 0, 0},
    { "quiet",             0, 0, 0},
    { "test",              0, 0, 0},
    { "force",             0, 0, 0},
    { "dolinks",           0, 0, 0},
    { "missing",           0, 0, 0},
    { "byfmt",             1, 0, 0},
    { "byhyphen",          1, 0, 0},
    { "enablefmt",         1, 0, 0},
    { "disablefmt",        1, 0, 0},
    { "listcfg",           1, 0, 0},
    { "showhyphen",        1, 0, 0},
    { "edit",              0, 0, 0},
    { "help",              0, 0, 0},
    { "cnffile",           1, 0, 0},
    { "fmtdir",            1, 0, 0},
    { "debug",             1, 0, 0},
    {0, 0, 0, 0}
};

enum {
  NONE, ALL, MISSING, BYFMT, BYHYPHEN, 
  ENABLEFMT, DISABLEFMT, LISTCFG, SHOWHYPHEN, EDIT
};

#define MAX_FMTS 128

typedef struct fmtdesc {
  string format;		/* e.g.: latex.fmt */
  string shortformat;		/* e.g.: latex */
  string engine;		/* e.g.: tex */
  string progname;		/* e.g.: latex */
  string hyphenation;		/* e.g.: - */
  string texargs;
  string inifile;
  string inifile_short;		/* remember it */
  string pool;
  string poolbase;
  string tcx;
  string tcxoption;
} fmtdesc;

static fmtdesc fmts[MAX_FMTS];
static int nb_formats = 0;

string unrmable_engines[] = {
    "tex", "pdftex", "etex", "pdfetex", "omega", "eomega", "aleph", "mf", "mpost", NULL
};

/*
  First  argument takes progname;
  Other arguments take no arguments.
  */
static string usage_msg[][MAX_LINES] = { 
  {
	"Usage: fmtutil [option] ... command\n\n",
	"Valid options:\n",
	"  --cnffile file\n",
	"  --fmtdir directory\n",
	"  --debug=<n>                set debug level to n\n",
	"  --quiet                    no output except error messages\n",
	"  --test                     only print what would be done\n",
	"  --dolinks                  link engine to format\n\n",
	"  --force                    force links even if target exists\n\n",
	"Valid commands:\n",
	"  --all                      recreate all format files\n",
	"  --missing                  create all missing format files\n",
	"  --byfmt formatname         (re)create format for `formatname'\n",
	"  --byhyphen hyphenfile      (re)create formats that depend on `hyphenfile'\n",
	"  --enablefmt formatname     enable `formatname' in config file\n",
	"  --disablefmt formatname    disable formatname in config file\n",
	"  --listcfg                  list (enabled and disabled) configurations\n",
	"  --showhyphen formatname    print name of hyphenfile for format `formatname'\n",
	"  --edit                     edit fmtutil.cnf file\n",
	"  --version                  display version information\n",
	"  --help                     show this message\n",
	0
  },
  {
	"Usage: mktexfmt [[option] format.ext | command]\n\n",
	"Valid options:\n",
	"  --debug=<n>                set debug level to n\n",
	"  --quiet                    no output except error messages\n",
	"  --test                     only print what would be done\n",
	"  --dolinks                  link engine to format\n\n",
	"  --force                    force links even if target exists\n\n",
	"Valid commands:\n",
	"  --version                  display version information\n",
	"  --help                     show this message\n",
	0
  }
};

static string fmtutil_version_string = "Revision: 0.40";

static string cnf_default = "fmtutil.cnf";
static string cnf_file = NULL;
static string arg = NULL;
static string destdir = NULL;
static int cmd= NONE;
static boolean quiet = false;
static boolean norebuild = false;
static boolean dolinks = false;
static boolean force = false;
static enum { FMTUTIL, MKTEXFMT, LAST_PROGRAM } incarnation = FMTUTIL;

#define MAX_LINKS 128

struct _links {
  string src;
  string dst;
} to_link[MAX_LINKS];
int nb_links = 0;

void link_formats();

void log_failure(string msg)
{
  if (log_failure_msg == NULL) {
    log_failure_msg = xstrdup(msg);
  }
  else {
    string tmp = log_failure_msg;
    log_failure_msg = concat(log_failure_msg, msg);
    free(tmp);
  }
}

/*
  First part: fmtutil.opt
  */
int fmtutil_opt(int argc, char *argv[])
{
  int g; /* getopt return code */
  int i;
  int option_index;

  for(;;) {
    g = getopt_long_only (argc, argv, "", long_options, &option_index);

    if (g == EOF)
      break;

    if (g == '?')
      return 1;  /* Unknown option.  */

    /* assert (g == 0); */ /* We have no short option names.  */
    
    if (ARGUMENT_IS ("debug")) {
      kpathsea_debug |= atoi (optarg);
    }
    else if (ARGUMENT_IS ("quiet")) {
      quiet = true;
    }
    else if (ARGUMENT_IS ("test")) {
      norebuild = true;
    }
    else if (ARGUMENT_IS ("dolinks")) {
      dolinks = true;
    }
    else if (ARGUMENT_IS ("force")) {
      force = true;
    }
    else if (ARGUMENT_IS ("help")) {
      usage();
      exit(0);
    }
    else if (ARGUMENT_IS ("version")) {
	fprintf(stderr, "%s (version %s) of %s.\n", progname, 
		fmtutil_version_string,
		kpathsea_version_string);
	exit(0);
      }
    else if (incarnation == MKTEXFMT) {
      /* no othe options */
    }
    else if (incarnation == FMTUTIL) {
      if (ARGUMENT_IS("all")) {
	cmd = ALL;
      }
      else if (ARGUMENT_IS("missing")) {
	cmd = MISSING;
      }
      else if (ARGUMENT_IS("byfmt")) {
	cmd = BYFMT;
	arg = xstrdup(optarg);
      }
      else if (ARGUMENT_IS("byhyphen")) {
	cmd = BYHYPHEN;
	arg = xstrdup(optarg);
      }
      else if (ARGUMENT_IS("enablefmt")) {
	cmd = ENABLEFMT;
	arg = xstrdup(optarg);
      }
      else if (ARGUMENT_IS("disablefmt")) {
	cmd = DISABLEFMT;
	arg = xstrdup(optarg);
      }
      else if (ARGUMENT_IS("listcfg")) {
	cmd = LISTCFG;
	arg = xstrdup(optarg);
      }
      else if (ARGUMENT_IS("showhyphen")) {
	cmd = SHOWHYPHEN;
	arg = xstrdup(optarg);
      }
      else if (ARGUMENT_IS("edit")) {
	cmd = EDIT;
      }
      else if (ARGUMENT_IS("cnffile")) {
	cnf_file = xstrdup(optarg);
      }
      else if (ARGUMENT_IS("fmtdir")) {
	destdir = xstrdup(optarg);
      }
    }
  }

  if (incarnation == MKTEXFMT) {
    cmd = BYFMT;
    if (optind < argc) {
      string ext = NULL;
      /* Make `mktexfmt <format.ext>' equivalent to `fmtutil --byfmt=<format>' */
      cmd = BYFMT;
      arg = xstrdup(xbasename(argv[optind++]));
      /* remove any extension */
      ext = strrchr(arg, '.');
      if (ext) *ext = '\0';
    }
  }

  /* shifting options from argv[] list */
  for (i = 1; optind < argc; i++, optind++)
    argv[i] = argv[optind];
  argv[i] = NULL;

  argc = i;

  if (argc > 1) {
    fprintf(stderr, "%s: Extra arguments", progname);
    fprintf (stderr, "\nTry `%s --help' for more information.\n",
	     kpse_program_name);
    exit(1);
  }

  if (argc == 0 || cmd == NONE) {
    fprintf(stderr, "%s: Missing arguments", progname);
    fprintf (stderr, "\nTry `%s --help' for more information.\n",
	     kpse_program_name);
    exit(1);
  }

  return argc;
}
  
/* Reading fmtutil.cnf */
boolean parse_line(string line)
{
  string format = NULL, 
    engine = NULL,
    hyphenation = NULL,
    texargs = NULL, 
    inifile = NULL, 
    pool = NULL, 
    tcx = NULL;
  string p, end;

  end = line + strlen(line);

  format = strtok(line, "\t ");
  if (!format || !*format)
    return false;
  engine = strtok(NULL, "\t ");
  if (!engine || !*engine)
    return false;
  hyphenation = strtok(NULL, "\t ");
  if (!hyphenation || !*hyphenation)
    return false;
  texargs = strtok(NULL, " \t");
  if (!texargs || !*texargs) 
    return false;
  if (strncasecmp(texargs, "nls=", 4) == 0) {
    pool = texargs+4;
    if (tcx = strchr(pool, ',')) {
      *tcx = '\0';
    }
    texargs = strtok(NULL, " \t");
  }

  p = texargs + strlen(texargs);
  if (p < end && *p == '\0') 
    *p = ' ';

  /* Get last argument */
  for (inifile = texargs + strlen(texargs) - 1;
       isspace(*inifile) && inifile > texargs;
       inifile--)
    *inifile = '\0';
  for ( ; !isspace(*(inifile - 1)) && inifile > texargs; inifile--);

  inifile = (*inifile == '*' ? inifile+1 : inifile);

  fmts[nb_formats].inifile_short = (inifile ? xstrdup(inifile) : NULL);

#if 0
  fprintf(stderr, "parse_line : pool = %s, hyphen = %s, tcx = %s, inifile = %s\n", pool, hyphenation, tcx, inifile);
#endif
  kpse_reset_program_name(format);
  if (STREQ(engine, "mf") || STREQ(engine, "mf-nowin")) {
    if (!kpse_find_file(inifile, kpse_mf_format, false)) {
      /* Do not log it : we don't know if it has been requested or not ! */
      /* sprintf(msg_buf, "%s: ini file %s not found\n", cnf_file, inifile); */
      /* log_failure(msg_buf); */
      inifile = NULL;
    } 
    fmts[nb_formats].progname = xstrdup(engine);
    fmts[nb_formats].shortformat = xstrdup(format);
    fmts[nb_formats].format = concat(format, ".base");
  }
  else if (STREQ(engine, "mpost")) {
    if (!kpse_find_file(inifile, kpse_mp_format, false)) {
/*        sprintf(msg_buf, "%s: ini file %s not found\n", cnf_file, inifile); */
/*        log_failure(msg_buf); */
      inifile = NULL;
    } 
    fmts[nb_formats].progname = xstrdup(engine);
    fmts[nb_formats].shortformat = xstrdup(format);
    fmts[nb_formats].format = concat(format, ".mem");
  }
  else {
    if (!kpse_find_file(inifile, kpse_tex_format, false)) {
/*        sprintf(msg_buf, "%s: ini file %s not found\n", cnf_file, inifile); */
/*        log_failure(msg_buf); */
      inifile = NULL;
    } 
    fmts[nb_formats].progname = xstrdup(format);
    fmts[nb_formats].shortformat = xstrdup(format);
    
    if (STREQ(engine, "etex")
	|| STREQ(engine, "pdfetex")) {
      fmts[nb_formats].format = concat(format, ".efmt");
    }
    else if (STREQ(engine, "aleph")
	|| STREQ(engine, "pdfaleph")) {
      fmts[nb_formats].format = concat(format, ".afmt");
    }
    else if (STREQ(engine, "eomega")
	|| STREQ(engine, "pdfeomega")) {
      fmts[nb_formats].format = concat(format, ".eoft");
    }
    else if (STREQ(engine, "omega")) {
      fmts[nb_formats].format = concat(format, ".oft");
    }
    else {
      fmts[nb_formats].format = concat(format, ".fmt");
    }
  }
  kpse_reset_program_name(progname);
  fmts[nb_formats].engine = xstrdup(engine);
  fmts[nb_formats].hyphenation = xstrdup(hyphenation);
  fmts[nb_formats].texargs = xstrdup(texargs);
  fmts[nb_formats].inifile = (inifile ? xstrdup(inifile) : NULL);
  if (pool == NULL) {
    fmts[nb_formats].poolbase = NULL;
    fmts[nb_formats].pool = NULL;
  }
  else {
    fmts[nb_formats].poolbase = xstrdup(pool);
    fmts[nb_formats].pool = concat(pool, ".pool");
  }
  if (tcx == NULL) {
    fmts[nb_formats].tcx = xstrdup("");
    fmts[nb_formats].tcxoption = xstrdup("");
  }
  else {
    fmts[nb_formats].tcx = xstrdup(tcx);
    fmts[nb_formats].tcxoption = concat("-translate-file=",tcx);
  }
  nb_formats++;

  return true;
}

void read_fmtutilcnf()
{
  FILE *f;
  string line;

  if (test_file('n', cnf_file) && test_file('r', cnf_file)) {
    f = fopen(cnf_file, "r");
    if (KPSE_DEBUG_P(FMTUTIL_DEBUG)) {
      fprintf(stderr, "Reading fmtutil_cnf file: %s\n", cnf_file);
    }
    while ((line  = read_line(f)) != NULL) {
      /* skip comments */
      if (*line == '#' || *line == '%' || isspace(*line)
	  || *line == '\0' || *line == '\n')
	continue;
      
      if (!parse_line(line))
	fprintf(stderr, "%s: syntax error: %s\n", cnf_file, line);
      free(line);
    }
    fclose(f);
    if (KPSE_DEBUG_P(FMTUTIL_DEBUG)) {
      int i;
      for (i = 0; i < nb_formats; i++) {
	fprintf(stderr, "Line %d: fmt = %s engine = %s progname = %s hyph = %s tex = %s ini = %s\n",
		i, fmts[i].format, fmts[i].engine, fmts[i].progname,
		fmts[i].hyphenation, fmts[i].texargs, fmts[i].inifile);
      }
    }

  }
}

void __cdecl output_and_cleanup(int code)
{
  /* FIXME : what cleanup ? */
  int count = 0;

  if (test_file('d', tmpdir))
    rec_rmdir(tmpdir);

  if (code != 0 && log_failure_msg && *log_failure_msg) {
    fprintf(stderr, "\
###############################################################################\n\
%s: Error! Not all formats have been built successfully.\n\
Visit the log files in directory\n\
  %s\n\
for details.\n\
###############################################################################\n\
\n\
This is a summary of all `failed' messages and warnings:\n\
%s\n",
	    kpse_program_name, destdir, log_failure_msg);
  }
}

static void usage()
{
  int i;
  fprintf(stderr, "%s of %s\n", progname, kpathsea_version_string);
  fprintf(stderr, "Fmtutil version %s\n", fmtutil_version_string);
  fprintf(stderr,usage_msg[incarnation][0], progname );
  fputs("\n", stderr);
  for(i = 1; usage_msg[incarnation][i] != NULL; ++i)
    fputs(usage_msg[incarnation][i], stderr);
}


int main(int argc, char* argv[])
{
  string texinputs, texformats;

#if defined(WIN32)
  /* if _DEBUG is not defined, these macros will result in nothing. */
   SETUP_CRTDBG;
   /* Set the debug-heap flag so that freed blocks are kept on the
    linked list, to catch any inadvertent use of freed memory */
   SET_CRT_DEBUG_FIELD( _CRTDBG_DELAY_FREE_MEM_DF );
#endif
   output_and_cleanup_function = output_and_cleanup;
   
   if (!progname)
     progname = argv[0];
   kpse_set_program_name (progname, NULL);
   
   if (FILESTRCASEEQ(kpse_program_name, "fmtutil")) {
     incarnation = FMTUTIL;
     progname = "fmtutil";
   }
   else if (FILESTRCASEEQ(kpse_program_name, "mktexfmt")) {
     incarnation = MKTEXFMT;
     progname = "mktexfmt";
   }
   else {
     fprintf(stderr, "This program was incorrectly copied to the name %s\n", 
	     argv[0]);
     return 1;
   }

  /* initialize the symbol table */
  init_vars();

  /* `kpse_init_prog' will put MODE and DPI into environment as values of
     the two variables below.  We need to leave these variables as they are. */
  mode = getval("MAKETEX_MODE");
  bdpi = getval("MAKETEX_BASE_DPI");

  /* NULL for no fallback font. */
  kpse_init_prog (uppercasify (progname), 
		  bdpi && atoi(bdpi) ? atoi(bdpi) : 600, mode, NULL);

  /* fmtutil_opt may modify argc and shift argv */
  argc = fmtutil_opt(argc, argv);

  setval_default("DPI", getval("BDPI"));
  setval_default("MAG", "1.0");

  /* if no cnf_file from command-line, look it up with kpsewhich: */
  if (test_file('z', cnf_file)) {
    cnf_file = kpse_find_file(cnf_default, kpse_web2c_format, true);
    if (test_file('z', cnf_file)) {
      fprintf(stderr, "%s: fatal error, configuration file not found.\n",
	      progname);
      exit(1);
    }
  }
  if (!test_file('f', cnf_file)) {
    fprintf(stderr, "%s: config file `%s' not found.\n", progname, cnf_file);
    exit(1);
  }

  /* read the fmtutil.cnf file */
  read_fmtutilcnf();

  if (cmd == SHOWHYPHEN) {
    exit(show_hyphen_file(arg));
  }
  else if (cmd == EDIT) {
    exit(edit_fmtutil_cnf());
  }

  cache_vars();

  texmfmain = getval("MT_TEXMFMAIN");
  if (test_file('z', texmfmain) || !test_file('d', texmfmain)) {
    fprintf(stderr, "%s: $TEXMFMAIN is undefined or points to a non-existent directory;\n%s: check your installation.\n", progname, progname);
    exit(1);	/* not mt_exit, since temporary files were not created yet */
  }

  /* setup dest dir */
  if (test_file('z', destdir)) {
    setval_default("VARTEXMF", expand_var("$VARTEXMF"));
    if (test_file('z', getval("VARTEXMF")))
      setval("VARTEXMF", expand_var("$MT_TEXMFMAIN"));
    destdir = concat(getval("VARTEXMF"), "/web2c");
  }
  if (!test_file('d', destdir)) {
    do_makedir(destdir);
    if (!test_file('d', destdir)) {
      fprintf(stderr, "%s: format directory `%s' does not exist.\n",
	      progname, destdir);
      exit(1);
    }
  }

  cwd = xgetcwd();
  if (test_file('n', cwd)) {
    /* Fixme : is it enough ? */
    if (getenv("KPSE_DOT") == NULL) {
      xputenv("KPSE_DOT", cwd);
    }
    texinputs = getval("TEXINPUTS");
    if (test_file('z', texinputs)) {
      xputenv("TEXINPUTS", concat(cwd, ENV_SEP_STRING));
    }
    else {
      xputenv("TEXINPUTS", concatn(cwd, ENV_SEP_STRING, 
				   texinputs, ENV_SEP_STRING, NULL));
    }
  }

  /* Catch signals, so we clean up if the child is interrupted.
     This emulates "trap 'whatever' 1 2 15".  */
#ifdef _WIN32
  SetConsoleCtrlHandler((PHANDLER_ROUTINE)sigint_handler, TRUE);
#else
# ifdef SIGINT
  signal (SIGINT, sigint_handler);
# endif
# ifdef SIGHUP
  signal (SIGHUP, sigint_handler);
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
  sprintf(tmpdir, "%s/fmXXXXXX", tempenv);
  mktemp(tmpdir);
  if (do_makedir(tmpdir)) {
    perror(tmpdir);
    exit(1);
  }
  setval("TEMPDIR", tmpdir);
  setval("STDOUT", concat_pathes(tmpdir, "astdout"));
  pushd(tmpdir);

  /* Kludge to get dependency between formats working 
     in one pass : formats are build in tmpdir before
     being sent to their final destination. */
  texformats = concatn(tmpdir, 
		       ENV_SEP_STRING, 
		       kpse_var_expand("$TEXFORMATS"), 
		       ENV_SEP_STRING, 
		       NULL);
  xputenv("TEXFORMATS", texformats);

  
#if 0
  if ((fout = fopen(getval("STDOUT"), "w")) == NULL) {
    perror(output);
    mt_exit(1);
  }
#endif
  
  /* make local pathes absolute */
  if (! kpse_absolute_p(destdir, FALSE) ) {
    destdir = concat3(cwd, DIR_SEP_STRING, destdir);
  }
  if (! kpse_absolute_p(cnf_file, FALSE) ) {
    cnf_file = concat3(cwd, DIR_SEP_STRING, cnf_file);
  }


  /* umask = 0 */
  umask(0);	/* for those platforms who support it */

  errstatus = 0;

  switch (cmd) {
  case ALL:
    errstatus = recreate_all();
    break;
  case MISSING:
    errstatus = create_missing();
    break;
  case BYFMT:
    errstatus = create_one_format(arg);
    break;
  case BYHYPHEN:
    errstatus = recreate_by_hyphenfile(arg);
    break;
  }

  /* Install the log files and format files */
  process_multiple_files("*.log", move_log);
  process_multiple_files("*.base", move_fmt);
  process_multiple_files("*.mem", move_fmt);
  process_multiple_files("*.fmt", move_fmt);
  process_multiple_files("*.efmt", move_fmt);
  process_multiple_files("*.oft", move_fmt);
  process_multiple_files("*.eoft", move_fmt);
  process_multiple_files("*.afmt", move_fmt);

  if (dolinks)
    link_formats();

  mt_exit(errstatus != true);

}

/*
###############################################################################
# cache_vars()
#   locate files / kpathsea variables and export variables to environment
#    this speeds up future calls to e.g. mktexupd
###############################################################################
*/
void cache_vars()
{
  setval_default("MT_VARTEXFONTS", expand_var("$VARTEXFONTS"));
  setval_default("MT_TEXMFMAIN", kpse_path_expand("$TEXMFMAIN"));
  setval_default("MT_MKTEX_CNF", kpse_find_file("mktex.cnf", kpse_web2c_format, false));
  setval_default("MT_MKTEX_OPT", kpse_find_file("mktex.opt", kpse_web2c_format, false));
  /* This should give us the ls-R default path */
  if (!kpse_format_info[kpse_db_format].type) /* needed if arg was numeric */
    kpse_init_format (kpse_db_format);
  setval_default("MT_LSR_PATH", kpse_format_info[kpse_db_format].path);
}

int do_format(int f)
{
  string cmdfmt = "%s -ini %s %s=%s -progname=%s %s <nul";
  string fmtswitch;
  string prgswitch;
  int ret = 0;
  
  /* Do nothing if inifile has not been found */
  if (fmts[f].inifile == NULL) {
    sprintf(msg_buf, "%s: ini file %s not found\n", cnf_file, fmts[f].inifile_short);
    log_failure(msg_buf);
    return -1;
  }

  fmtswitch = "-jobname";

  prgswitch = fmts[f].progname;
  /* Kludge for context : avoid to set texmf.cnf parameters for
	 every cont-?? format. */
  if (strlen(prgswitch) == strlen("cont-??")
	  && FILESTRNCASEEQ(prgswitch, "cont-", 5)) {
	prgswitch = "context";
  }
  else if (strlen(prgswitch) == strlen("mptopdf")
	  && FILESTRCASEEQ(prgswitch, "mptopdf")) {
	prgswitch = "context";
  }
  else if (strlen(prgswitch) == strlen("metafun")
	  && FILESTRCASEEQ(prgswitch, "metafun")) {
	prgswitch = "mpost";
  }

  if (sprintf(cmd_sys, cmdfmt, fmts[f].engine, fmts[f].tcxoption, fmtswitch, fmts[f].shortformat,
	      prgswitch, fmts[f].texargs) > 1023) {
    fprintf(stderr, "Warning: do_format buffer overrun!\n");
    fprintf(stderr, "no command run.\n");
    return -1;
  }

  if (fmts[f].pool != NULL) {
    string poolfile;
    string local_poolfile = concat(fmts[f].engine, ".pool");

    kpse_reset_program_name(fmts[f].engine);

    if (STREQ(fmts[f].engine, "mf") || STREQ(fmts[f].engine, "mf-nowin")) {
      poolfile = kpse_find_file(fmts[f].pool, kpse_mfpool_format, true);
    }
    else if (STREQ(fmts[f].engine, "mpost")) {
      poolfile = kpse_find_file(fmts[f].pool, kpse_mppool_format, true);
    }
    else {
      poolfile = kpse_find_file(fmts[f].pool, kpse_texpool_format, true);
    }

    process_multiple_files("*.pool", remove_file);
    if (test_file('n', poolfile) && test_file('f', poolfile)) {
      fprintf(stderr, "%s: attempting to create localized format using pool=$pool and tcx=$tcx.\n",
	      kpse_program_name, fmts[f].poolbase, fmts[f].tcx);
      catfile(poolfile, local_poolfile, false);
      xputenv("TEXPOOL", ".");
    } 
    kpse_reset_program_name(progname);
  }

  /* run discarding stdout */
  start_redirection(quiet);

  if (!quiet)
    fprintf(stdout, "running `%s' ...\n", cmd_sys);

  if (!norebuild) {
    unlink(fmts[f].format);
    ret = system(cmd_sys);
    if (ret == -1 || !test_file('f', fmts[f].format)) {
      sprintf(msg_buf, "%s: command `%s' failed (ret = %d, %s).\n", progname, cmd_sys, ret, fmts[f].format);
      log_failure(msg_buf);
      unlink(fmts[f].format);
    }
  }

  pop_fd();

  if (dolinks) {
    to_link[nb_links].src = fmts[f].engine;
    to_link[nb_links].dst = fmts[f].progname;
    nb_links++;
  }
  return ret;
}

int recreate_all()
{
  int i, ret = true;

  for (i = 0; i < nb_formats; i++) {
    if (do_format(i) != 0)
      ret = false;
  }

  return ret;
}

int create_missing()
{
  int i, ret = true;
  string destfmt;

  for (i = 0; i < nb_formats; i++) {
    destfmt = concat_pathes(destdir, fmts[i].format);
    if (! test_file('f', destfmt)) {
      if (do_format(i) != 0)
	ret = false;
    }
    free(destfmt);
  }
  return ret;
}

int create_one_format(string fmtname)
{
  int i, ret = true;
  boolean created = false;

  for (i = 0; i < nb_formats; i++) {
    if (FILESTRCASEEQ(fmtname, fmts[i].shortformat)) {
      if (do_format(i) != 0)
	ret = false;
      created = true;
      break;
    }
  }

  if (! created) {
    fprintf(stderr, "%s: format file %s not found in fmtutil.cnf.\n",
	    progname, fmtname);
    ret = false;
  }

  return ret;
}

int recreate_by_hyphenfile(string hyphenfile)
{
  int i, ret = true;
  boolean created = false;

  for (i = 0; i < nb_formats; i++) {
    if (FILESTRCASEEQ(hyphenfile, fmts[i].hyphenation)) {
      if (do_format(i) != 0)
	ret = false;
      created = true;
      break;
    }
  }

  if (! created) {
    fprintf(stderr, "%s: hyphen file %s not found in fmtutil.cnf.\n",
	    progname, hyphenfile);
    ret = false;
  }

  return ret;
}

int enable_format_file(string format)
{
  FILE *fin, *fout;
  char line[256];
  string bak_file = concat(cnf_file, ".bak");

  fin = fopen(cnf_file, "r");
  if (! fin) {
    perror(cnf_file);
    return false;
  }
  fout = fopen (bak_file, "w");
  if (! fout) {
    perror(bak_file);
    return false;
  }
  while (fgets(line, sizeof(line), fin)) {
    if (strncmp(line, "#! ", 3) == 0
	&& strncmp(line+3, format, strlen(format)) == 0
	&& isspace(line+3+strlen(format))) {
      fputs(line + 3, fout);
    }
    else {
      fputs(line, fout);
    }
  }
  fclose(fin);
  fclose(fout);
  mvfile(bak_file, cnf_file);
  free(bak_file);
  return true;
}

int disable_format_file(string format)
{
  FILE *fin, *fout;
  char line[256];
  string bak_file = concat(cnf_file, ".bak");

  fin = fopen(cnf_file, "r");
  if (! fin) {
    perror(cnf_file);
    return false;
  }
  fout = fopen (bak_file, "w");
  if (! fout) {
    perror(bak_file);
    return false;
  }
  while (fgets(line, sizeof(line), fin)) {
    if (strncmp(line, format, strlen(format)) == 0
	&& isspace(line + strlen(format))) {
      fputs("#! ", fout);
    }
    fputs(line, fout);
  }
  fclose(fin);
  fclose(fout);
  mvfile(bak_file, cnf_file);
  free(bak_file);
  return true;
}

int list_config_file()
{
  FILE *fin;
  char line[256];

  fin = fopen(cnf_file, "r");
  if (! fin) {
    perror(cnf_file);
    return false;
  }
  while (fgets(line, sizeof(line), fin)) {
    if (isalpha(line[0])
	|| strncmp(line, "#! ", 3) == 0)
      fputs(line, stdout);
  }
  fclose(fin);
  return true;
}

int show_hyphen_file(string format)
{
  int i, ret;
  string hyphenfile;

  for (i = 0; i < nb_formats; i++) {
    if (FILESTRCASEEQ(format, fmts[i].shortformat)) {
      if (STREQ(fmts[i].hyphenation, "-")) {
	printf("%s\n", fmts[i].hyphenation);
	return 0;
      }
      kpse_reset_program_name(format);
      kpse_format_info[kpse_tex_format].cnf_path = kpse_cnf_get("TEXINPUTS");
      if (!(hyphenfile 
	    = kpse_find_file(fmts[i].hyphenation, kpse_tex_format, false))) {
	fprintf(stderr, "%s: hyphen file %s not found\n", progname, 
		fmts[i].hyphenation);
	ret = -1;
      }
      else {
	printf("%s\n", hyphenfile);
	free(hyphenfile);
	ret = 0;
      }
      kpse_reset_program_name(progname);
      return ret;
    }
  }
  fprintf(stderr, "no info for format %s\n", format);
  return 0;
}

int edit_fmtutil_cnf()
{
  string editor = kpse_var_expand("$EDITOR");
  string command, p;
  int ret;

  if (test_file('z', editor)) {
    editor = kpse_var_expand("$VISUAL");
    if (test_file('z', editor)) {
      editor = kpse_var_expand("$TEXEDIT");
      if (test_file('z', editor)) {
	editor = "wordpad";
      }
    }
  }
   
  p = editor;

  while (isspace(*p++));
  if (*p == '\"') {
    do {
      p++;
    } while (*p != '\"' && *p != '\0');
    if (*p == '\"') {
      *(p+1) = '\0';
    }
    else {
      fprintf(stderr, "%s: unterminated quoted string for editor specification.\n",
	      progname);
      exit(1);
    }
  }
  else {
    do {
      p++;
    } while (!isspace(*p) && *p != '\0');
    *p = '\0';
  }
  
  command = concat3(editor, " ", cnf_file);
#ifdef WIN32
  ret = win32_system(command, true);
#else
  ret = system (command);
#endif
  if (ret != 0)
    fprintf (stderr, "! Trouble executing `%s'.\n", command);

  return ret;
}

/*
  This function expects a globbing file specification.
  There is no recursive call here.
*/
void process_multiple_files(string filespec, process_fn fn)
{

  char path[PATH_MAX];
  int path_len;
#if defined(_WIN32)
  WIN32_FIND_DATA find_file_data;
  HANDLE hnd;
#else
  glob_t gl;
#endif

  strcpy(path, filespec);

  /* If it is a directory, then get all the files there */
  if (!(strrchr(path, '*') || strrchr(path, '?'))
      && test_file('d', path)) {
    path_len = strlen(path);
    strcat(path, "/*");
  }
  else {
    path_len = strlen(path) - strlen(xbasename(path)) -1;
  }

#if defined(_WIN32)
  hnd = FindFirstFile(path, &find_file_data);
  while (hnd != INVALID_HANDLE_VALUE) {
    if(!strcmp(find_file_data.cFileName, ".")
       || !strcmp(find_file_data.cFileName, "..")) 
	continue;
    path[path_len+1] = '\0';
    strcat(path, find_file_data.cFileName);
    if (KPSE_DEBUG_P(FMTUTIL_DEBUG)) {
      fprintf(stderr, "Processing %s\n", path);
    }
    (*fn)(path);
      if (FindNextFile(hnd, &find_file_data) == FALSE)
	break;
  }
  path[path_len+1] = '\0';
  FindClose(hnd);

#else /* ! WIN32 */

  switch  (glob(path, GLOB_NOCHECK, NULL, &gl)) {
  case 0:			/* success */
      for (i = 0; i < gl.gl_pathc; i++) {
	(*fn)(gl.gl_pathv[i]);
      }
      globfree(&gl);
      break;

  case GLOB_NOSPACE:
    fprintf(stderr, "%s: Out of memory while globbing %s.\n",
	    kpse_program_name, filespec);
    mt_exit(1);
    break;

  default:
    break;
  }
#endif

}

void remove_file(string filename)
{
  unlink(filename);
}

void move_log(string logname)
{
  string logdest;
  logdest = concat3(destdir, DIR_SEP_STRING, logname);
  unlink(logdest);
  mvfile(logname, logdest);
  free(logdest);
}

void move_fmt(string fmtname)
{
  string fmtdest;
  string texfmt, plainfmt;
  char cmd[4096];
  string cmdfmt = "mktexupd \"%s\" %s";

  fmtdest = concat3(destdir, DIR_SEP_STRING, fmtname);
  unlink(fmtdest);
  if (KPSE_DEBUG_P(FMTUTIL_DEBUG)) {
    fprintf(stderr, "Moving %s to %s\n", fmtname, fmtdest);
  }
  mvfile(fmtname, fmtdest);
  fprintf(stderr, "%s: %s successfully generated.\n", progname, fmtdest);
  sprintf(cmd, cmdfmt, destdir, fmtname);
#if 1
  if (KPSE_DEBUG_P(FMTUTIL_DEBUG)) {
    fprintf(stderr, "Calling system(%s)\n", cmd);
  }
#endif
  system(cmd);
  /* if we have been called to build a format at runtime */
  if (incarnation == MKTEXFMT) {
    fputs(fmtdest, stdout);
  }
  free(fmtdest);

  texfmt = concat3(destdir, DIR_SEP_STRING, "tex.fmt");
  plainfmt = concat3(destdir, DIR_SEP_STRING, "plain.fmt");
  if (test_file('f', texfmt) && !test_file('f', plainfmt)) {
    catfile(texfmt, plainfmt, FALSE);
    sprintf(cmd, cmdfmt, destdir, "plain.fmt");
    system(cmd);
  }
  free(texfmt);
  free(plainfmt);

  texfmt = concat3(destdir, DIR_SEP_STRING, "tex.efmt");
  plainfmt = concat3(destdir, DIR_SEP_STRING, "plain.efmt");
  if (test_file('f', texfmt) && !test_file('f', plainfmt)) {
    catfile(texfmt, plainfmt, FALSE);
    sprintf(cmd, cmdfmt, destdir, "plain.efmt");
    system(cmd);
  }
  free(texfmt);
  free(plainfmt);

  texfmt = concat3(destdir, DIR_SEP_STRING, "mf.base");
  plainfmt = concat3(destdir, DIR_SEP_STRING, "plain.base");
  if (test_file('f', texfmt) && !test_file('f', plainfmt)) {
    catfile(texfmt, plainfmt, FALSE);
    sprintf(cmd, cmdfmt, destdir, "plain.base");
    system(cmd);
  }
  free(texfmt);
  free(plainfmt);

  texfmt = concat3(destdir, DIR_SEP_STRING, "mpost.mem");
  plainfmt = concat3(destdir, DIR_SEP_STRING, "plain.mem");
  if (test_file('f', texfmt) && !test_file('f', plainfmt)) {
    catfile(texfmt, plainfmt, FALSE);
    sprintf(cmd, cmdfmt, destdir, "plain.mem");
    system(cmd);
  }
  free(texfmt);
  free(plainfmt);
}

void link_formats()
{
  int i, j;
  boolean skip;
  string src, srcdll, srcexe, dstexe;

  for (i = 0; i < nb_links; i++) {
    src = expand_var(concat("$SELFAUTOLOC/", to_link[i].src));
    /* Check that the destination is not among the ones that should
     *never* be removed */
    skip = false;
    for (j = 0; unrmable_engines[j] != NULL; j++) {
      if (FILESTRCASEEQ(to_link[i].dst, unrmable_engines[j])) {
	fprintf(stderr, 
		"%s: %s is a basic engine, won't remove it... skipping.\n",
		kpse_program_name, unrmable_engines[j]);
	skip = true;
	break;
      }
    }
    /* Any smarter way to do that ? */
    if (skip)
      continue;

    dstexe = expand_var(concatn("$SELFAUTOLOC/", to_link[i].dst, ".exe", NULL));
    if (test_file('f', dstexe) && force == false) {
      fprintf(stderr, "%s: destination %s already exists, skipping.\n", kpse_program_name, dstexe);
      free(dstexe);
      continue;
    }
#if 0
    srcexe = concat(src, ".exe");
    srcdll = concat(src, ".dll");
    if (FILESTRCASEEQ(srcexe, dstexe)) {
      printf("same files: %s and %s, doing nothing\n", srcexe, dstexe);
    }
    else if (test_file('f', srcdll)) {
      if (norebuild) {
	printf("copy %s to %s\n", srcexe, dstexe);
      }
      else if (catfile(srcexe, dstexe, false) == false) {
	fprintf(stderr, "%s: failed to copy %s to %s\n", 
		kpse_program_name, srcexe, dstexe);
      }
    }
    else {
      sprintf(cmd_sys,"lnexe %s %s", srcexe, dstexe);
      if (norebuild) {
	printf("lnexe %s to %s\n", srcexe, dstexe);
      }
      else if (system(cmd_sys) == -1) {
	fprintf(stderr, "%s: failed to lnexe %s to %s\n", 
		kpse_program_name, srcexe, dstexe);
      }
    }
    free(srcdll);
#else
    srcexe = concat(src, ".exe");
    if (FILESTRCASEEQ(srcexe, dstexe)) {
      printf("same files: %s and %s, doing nothing\n", srcexe, dstexe);
    }
    else if (norebuild) {
      printf("copy %s to %s\n", srcexe, dstexe);
    }
    else if (catfile(srcexe, dstexe, false) == false) {
      fprintf(stderr, "%s: failed to copy %s to %s\n", 
	      kpse_program_name, srcexe, dstexe);
    }
#endif
    free(srcexe);
    free(dstexe);
  }
}
