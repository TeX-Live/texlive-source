/* runht.c: routines to call tex4ht.

Copyright (C) 2000 Fabrice POPINEAU.

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

#include <kpathsea/config.h>
#include <kpathsea/lib.h>
#include <kpathsea/progname.h>
#include <kpathsea/c-dir.h>
#include <kpathsea/c-pathmx.h>
#include <kpathsea/getopt.h>
#include <kpathsea/tex-file.h>

typedef enum {
  MATH_BITMAP = 0,
  MATH_BITMAP_UNICODE,
  MATH_MATHML,
  MATH_MOZILLA,
  MATH_LAST
} math_type;

typedef enum {
  DTD_HTML = 0,
  DTD_XHTML,
  DTD_TEI,
  DTD_DOCBOOK,
  DTD_EBOOK,
  DTD_HTML4WORD,
  DTD_XHTML4WORD,
  DTD_LAST
} dtd_type;

char *stdcfg[MATH_LAST][DTD_LAST][4] = {
  /* MATH_BITMAP */
  {
    { "html", "", "alias,iso8859", "" },                 /* HTML */
    { "xhtml", "", "alias,iso8859", "" },                /* XHTML */
    { "", "", "", "" },	                 /* TEI, INVALID */
    { "", "", "", ""},	         /* DOCBOOK, INVALID */
    { "", "", "", ""},           /* EBOOK, INVALID */
    { "html,word", "", "symbol,alias,iso8859", "" },	 /* HTML4WORD */
    { "xhtml,word", "", "symbol,alias,iso8859", "" },	 /* XHTML4WORD */      
  },
  /* MATH_BITMAP_UNICODE */
  {
    { "html,uni-html4", "", "unicode", "" },             /* HTML */            
    { "xhtml,uni-html4", "", "unicode", "" },		 /* XHTML */           
    { "xhtml,tei", "", "unicode", "" },			 /* TEI */    
    { "xhtml,docbook", "", "unicode", ""},		 /* DOCBOOK */
    { "xhtml,ebmath", "-g.png", "unicode", ""},		 /* EBOOK */  
    { "", "", "", "" },		 /* HTML4WORD, INVALID */       
    { "", "", "", "" },		 /* XHTML4WORD,INVALID */      
  },
  /* MATH_MATHML */
  {
    { "", "", "", "" },                 /* HTML, INVALID */            
    { "xhtml,mathml", "", "unicode", "" },		 /* XHTML */           
    { "xhtml,tei-mml", "", "unicode", "" },		 /* TEI */    
    { "xhtml,docbook-mml", "", "unicode", ""},		 /* DOCBOOK */
    { "xhtml,ebook-mml", "-g.png", "unicode", ""},	 /* EBOOK */  
    { "", "", "", "" },		 /* HTML4WORD, INVALID */
    { "", "", "", "" },		 /* XHTML4WORD, INVALID */
  },
  /* MATH_MOZILLA */
  {
    { "", "", "", "" },                 /* HTML, INVALID */            
    { "xhtml,mozilla", "", "mozilla,unicode", "" },	 /* XHTML */           
    { "", "", "", "" },			 /* TEI, INVALID */    
    { "", "", "", ""},		 /* DOCBOOK, INVALID */
    { "", "", "", ""},		 /* EBOOK, INVALID */  
    { "", "", "", "" },		 /* HTML4WORD, INVALID */       
    { "", "", "", "" },		 /* XHTML4WORD, INVALID */      
  }
};

char *long_usage_string =  "Usage: %s [options] file.tex [params1] [params2] [params3]\n\
\toptions = --math=(bitmap|mathml)\n\
            --dtd=(html|xhtml|tei|docbook|ebook)\n\
            --mozilla\n\
            --unicode\n\
            --dbcs\n\
            --symbol\n\
            --msword\n\
            --output-dir=<dir>\n\
            --output-name=<name>\n\
            --index-file\n\
            --force\n\
            --dry-run\n\
            --cleanup\n\
            --help\n\
\tparams1 = (html|xhtml|3.2|mathml.4ht|unicode.4ht|...)*\n\
\tparams2 = symbol,unicode,...\n\
\tparams3 = (-p)\n";

char *long_simple_usage_string =  "Usage: %s [options] file.tex [\"t4ht params\"]\n\
\toptions = --output-dir=<dir>\n\
            --output-name=<name>\n\
            --index-file\n\
            --force\n\
            --dry-run\n\
            --cleanup\n\
            --help\n\
\tt4ht params = (-p|-i|...)\n";

char *runstr_simple = "%s %s %s";

char *runstr_tex = "tex %s \"\\def\\Link#1.a.b.c.{\\expandafter\\def\\csname tex4ht\\endcsname{\\expandafter\\def\\csname tex4ht\\endcsname{#1,html}\\input tex4ht.sty }}\\def\\HCode{\\futurelet\\HCode\\HChar}\\def\\HChar{\\ifx\\\"\\HCode\\def\\HCode\\\"##1\\\"{\\Link##1}\\expandafter\\HCode\\else\\expandafter\\Link\\fi}\\HCode %s.a.b.c.\\input %s\"";
    
char *runstr_latex = "latex %s \"\\makeatletter\\def\\HCode{\\futurelet\\HCode\\HChar}\\def\\HChar{\\ifx\\\"\\HCode\\def\\HCode\\\"##1\\\"{\\Link##1}\\expandafter\\HCode\\else\\expandafter\\Link\\fi}\\def\\Link#1.a.b.c.{\\g@addto@macro\\@documentclasshook{\\RequirePackage[#1,html]{tex4ht}}\\let\\HCode\\documentstyle\\def\\documentstyle{\\let\\documentstyle\\HCode\\expandafter\\def\\csname tex4ht\\endcsname{#1,html}\\def\\HCode####1{\\documentstyle[tex4ht,}\\@ifnextchar[{\\HCode}{\\documentstyle[tex4ht]}}}\\makeatother\\HCode %s.a.b.c.\\input %s\"";

char *runstr_texi = "tex %s \"\\def\\Link#1.a.b.c.{\\expandafter\\def\\csname tex4ht\\endcsname{\\expandafter\\def\\csname tex4ht\\endcsname{#1,html}\\input tex4ht.sty }}\\def\\HCode{\\futurelet\\HCode\\HChar}\\def\\HChar{\\ifx\\\"\\HCode\\def\\HCode\\\"##1\\\"{\\Link##1}\\expandafter\\HCode\\else\\expandafter\\Link\\fi}\\let\\svrm=\\rm\\def\\rm{\\svrm\\ifx\\c\\comment\\def\\rm{\\let\\rm=\\svrm\\catcode`\\@=12\\catcode`\\\\=0 \\csname tex4ht\\endcsname\\catcode`\\@=0\\catcode`\\\\=13 }\\expandafter\\rm\\fi}\\HCode %s.a.b.c.\\input %s\"";

boolean force = false;		/* Should we force LaTeX runs ? */
boolean dry_run = false;	/* Verbose trace */
boolean cleanup = false;	/* remove unused files */
dtd_type dtd = DTD_HTML;
math_type math = MATH_BITMAP;
char *texargs = NULL;
char *output_dir = NULL;
char *output_name = NULL;
char *texfile = NULL;
char *texsrc = NULL;
char *texengine = NULL;
boolean opt_index = false;
boolean newdest = false;
boolean mozilla = false;
boolean dbcs = false;
boolean symbol = false;
boolean msword = false;
boolean unicode = false;
boolean is_ht = false;

#define RUN_SYSTEM(x)                                               \
    { int retcode = 0;                                              \
      if (dry_run)                                                  \
        fprintf(stderr, "%s: running command:\n%s\n", progname, x); \
      else {                                                        \
        if ((retcode = system(x)) != 0) {                           \
          if (retcode == -1)                                        \
	    perror("system() failed");                              \
          if (! force)                                              \
            return retcode;                                         \
        }                                                           \
      }                                                             \
    }

#define TESTNZ(x) ((x != NULL) && (*(x) != 0))
#define TESTZ(x)  ((x == NULL) || (*(x) == 0))

#define OPT_FORCE 'f'
#define OPT_DRY_RUN 'r'
#define OPT_MOZILLA 'z'
#define OPT_MSWORD 'w'
#define OPT_UNICODE 'u'
#define OPT_DBCS 'd'
#define OPT_SYMBOL 's'
#define OPT_OUTPUT_DIR 'p'
#define OPT_OUTPUT_NAME 'o'
#define OPT_INDEX_FILE 'i'
#define OPT_DTD 't'
#define OPT_MATH 'm'
#define OPT_CLEANUP 'c'
#define OPT_HELP '?'

const struct option long_options[] =
{
    "force",            no_argument,            0,      OPT_FORCE,
    "dry-run",		no_argument,		0,	OPT_DRY_RUN,
    "mozilla",          no_argument,            0,      OPT_MOZILLA,
    "msword",           no_argument,            0,      OPT_MSWORD,
    "unicode",          no_argument,            0,      OPT_UNICODE,
    "dbcs",             no_argument,            0,      OPT_DBCS,
    "symbol",           no_argument,            0,      OPT_SYMBOL,
    "output-dir",       required_argument,      0,      OPT_OUTPUT_DIR,
    "output-name",      required_argument,      0,      OPT_OUTPUT_NAME,
    "index-file",       no_argument,            0,      OPT_INDEX_FILE,
    "dtd",              required_argument,      0,      OPT_DTD,
    "math",             required_argument,      0,      OPT_MATH,
    "cleanup",          no_argument,            0,      OPT_CLEANUP,
    "help",		no_argument,		0,	OPT_HELP,
    0,			no_argument,		0,	0,
};

char cmd[4096];
char *progname = NULL;
char cwd[MAXPATHLEN];
char *ext_to_cleanup[] = { "log", "aux", "toc", "lof", "lot", "dvi", 
			   "idv", "otc", "4ct", "4tc", "lg", "tmp",
                           "xref" };

void popd();

void do_cleanup()
{
  int i;
  char *ext;

#if defined(_WIN32)
  WIN32_FIND_DATA find_file_data;
  HANDLE hnd;
  char *root = xstrdup(output_name);
  int rootlen = strlen(root);

  if (rootlen > 4 && _stricmp(root + rootlen - 4, ".tex") == 0)
    root[rootlen - 4] = '\0';
  rootlen = strlen(root);

  hnd = FindFirstFile("*", &find_file_data);

  while (hnd != INVALID_HANDLE_VALUE && 
	   FindNextFile(hnd, &find_file_data) != FALSE) { 
    int len = strlen(find_file_data.cFileName);

    if (_strnicmp(find_file_data.cFileName, root, rootlen) != 0)
      continue;
    
    if (newdest) {
      if (dry_run) {
	  fprintf(stderr, "%s: cleaning up %s\n", progname, find_file_data.cFileName);
      }
      else {
	if (unlink(find_file_data.cFileName) == -1) {
	  fprintf(stderr, "%s: can't remove %s\n", progname, find_file_data.cFileName);
	  perror(find_file_data.cFileName);
	}
      }
      continue;
    }

    for (i = 0, ext = ext_to_cleanup[0]; 
	 i < sizeof(ext_to_cleanup)/sizeof(ext_to_cleanup[0]); 
	 ext = ext_to_cleanup[++i]) {
      if (stricmp(find_file_data.cFileName + len - strlen(ext), ext) != 0)
	continue;
      if (dry_run) {
	fprintf(stderr, "%s: cleaning up %s\n", progname, find_file_data.cFileName);
      }
      else {
	if (unlink(find_file_data.cFileName) == -1) {
	  fprintf(stderr, "%s: can't remove %s\n", progname, find_file_data.cFileName);
	  perror(find_file_data.cFileName);
	}
      }
    }
  }
  FindClose(hnd);

#else /* ! WIN32 */
  /* FIXME : adapt to what win32 is doing. */
  DIR *dp;
  struct dirent *ep;

  if ((dp = opendir("."))) {
    while ((ep = readdir(dp))) {
      int len = strlen(ep->d_name);
      if ((strlen(ep->d_name) < 4) ||
	  (ep->d_name[len - 4] != '.'))
	continue;
      for (i = 0, ext = ext_to_cleanup[0]; 
	   i < sizeof(ext_to_cleanup)/sizeof(ext_to_cleanup[0]); 
	   ext = ext_to_cleanup[++i]) {
	if (strcmp(ep->d_name[len - 3], ext) != 0)
	  continue;
	if (dry_run) {
	  fprintf(stderr, "%s: cleaning up %s\n", progname, find_file_data.cFileName);
	}
	else {
	  if (unlink(find_file_data.cFileName) == -1) {
	    fprintf(stderr, "%s: can't remove %s\n", progname, find_file_data.cFileName);
	    perror(find_file_data.cFileName);
	  }
	}
      }
    }
    closedir(dp);
  }
  else
    perror(path);
#endif /* WIN32 */
}

void mt_exit(int code)
{
#if 0
  if (cwd) {
    popd();
  }
#endif

  if (cleanup) {
    if (dry_run) 
      fprintf(stderr, "%s: doing cleanup ...\n", progname);
    else
      do_cleanup();
  }
    
  if (newdest) {
    DeleteFile(texfile);
  }

  exit(code);
}

#ifdef _WIN32
BOOL sigint_handler(DWORD dwCtrlType)
{
  /* Fix me : there is a problem if a system() command is running.
     We should wait for the son process to be interrupted.
     Only way I can think of to do that : rewrite system() based on
     spawn() with parsing of the command line and set a global pid
     Next cwait(pid) in the HandlerRoutine. 
     */
  mt_exit(2);
  return FALSE;			/* return value mandatory */
}
#else
void sigint_handler (int sig)
{
  mt_exit(2);
}
#endif

#if 0
/* pushd */
void pushd(char *p)
{
  if (getcwd(cwd, MAXPATHLEN) == NULL) {
    fprintf(stderr, "%s: pushd error, can't getcwd() !\n", progname);
    perror(p);
    exit(1);
  }

  if (chdir(p) == -1) {
    fprintf(stderr, "%s: pushd error, can't chdir() !\n", progname);
    perror(p);
    exit(1);
  }
}

/* popd */
void popd()
{
  if (cwd == NULL || *cwd == '\0') {
    fprintf(stderr, "%s: popd warning, cwd empty !\n", progname);
    return;
  }

  if (chdir(cwd) == -1) {
    fprintf(stderr, "%s: popd warning, can't chdir() !\n", progname);
    perror(cwd);
    return;
  }

  return;
}
#endif

void usage()
{
  if (is_ht) {
    fprintf(stderr, long_simple_usage_string, progname);
  }
  else {
    fprintf(stderr, long_usage_string, progname);
  }
  exit(1);
}

int ht(int argc, char *argv[])
{
  int i;

  if (argc != 2 && argc != 3) {
    usage();
  }

  sprintf(cmd, runstr_simple, texengine, texargs, texfile);
  for (i = 0; i < 3; i++)
    RUN_SYSTEM(cmd);

  sprintf(cmd, "tex4ht %s", output_name);
  RUN_SYSTEM(cmd);

  if (TESTNZ(argv[2])) {
    sprintf(cmd, "t4ht %s %s", output_name, argv[2]);
  }
  else {
    sprintf(cmd, "t4ht %s", output_name);
  }

  if (TESTNZ(output_dir)) {
    strcat(cmd, " -d");
    strcat(cmd, output_dir);
  }

  RUN_SYSTEM(cmd);
  
  return 0;
}

int ht_engine(char *runstr, int argc, char *argv[])
{
  int i;

  if (argc < 2 || argc > 5) {
    usage();
  }

  sprintf(cmd, runstr, texargs, argv[2], texfile);
  for (i = 0; i < 3; i++)
    RUN_SYSTEM(cmd);

  if (TESTNZ(argv[3])) {
    xputenv("TEX4HTFONTSET", argv[3]);
  }
  else {
    xputenv("TEX4HTFONTSET", "alias,iso8859");
  }
  
  if (dry_run) {
    fprintf(stderr, "TEX4HTFONTSET = %s\n", getenv("TEX4HTFONTSET"));
  }

  sprintf(cmd, "tex4ht %s %s", stdcfg[math][dtd][1], output_name);
  RUN_SYSTEM(cmd);

  if (TESTNZ(argv[4])) {
    sprintf(cmd, "t4ht %s %s", output_name, argv[4]);
  }
  else {
    sprintf(cmd, "t4ht %s", output_name);
  }

  if (TESTNZ(output_dir)) {
    strcat(cmd, " -d");
    strcat(cmd, output_dir);
  }

  RUN_SYSTEM(cmd);
  
  return 0;
}

char **addto_argv(char *arg, int *argc, char **argv, int *max_args)
{
    if (*argc == *max_args) {
	max_args += 10;
	argv = (char**)(realloc(argv, *max_args * sizeof(argv[0])));
    }
    argv[ *argc++ ] = strdup(arg);
    return argv;
}

char **parse_arguments(int *argc, char *argv[])
{
  int option_index = 0;
  char c;
  int maxargs = 0;
  int i;

  char **newargv;

  optind = 0;
  while ((c = getopt_long_only(*argc, argv, "", long_options, &option_index)) != EOF) {
    switch (c) {
      
    case OPT_HELP:
      usage();
      break;
	    
    case OPT_DRY_RUN:
      dry_run = true;
      break;
	    
    case OPT_FORCE:
      force = true;
      break;
		  
    case OPT_DTD:
      if (is_ht) {
	fprintf(stderr, "%s: wrong option, use `--help'.\n", progname);
	exit(1);
      }
      if (_strcmpi(optarg, "html") == 0) {
	dtd = DTD_HTML;
      }
      else if (_strcmpi(optarg, "xhtml") == 0) {
	dtd = DTD_XHTML;
      }
      else if (_strcmpi(optarg, "tei") == 0) {
	dtd = DTD_TEI;
      }
      else if (_strcmpi(optarg, "docbook") == 0) {
	dtd = DTD_DOCBOOK;
      }
      else if (_strcmpi(optarg, "ebook") == 0) {
	dtd = DTD_EBOOK;
      }
      else {
	fprintf(stderr, "%s: invalid --dtd=%s option.\n", progname, optarg);
	exit(1);
      }
      break;
      
    case OPT_MATH:
      if (is_ht) {
	fprintf(stderr, "%s: wrong option, use `--help'.\n", progname);
	exit(1);
      }
      if (_strcmpi(optarg, "bitmap") == 0) {
	math = MATH_BITMAP;
      }
      else if (_strcmpi(optarg, "mathml") == 0) {
	math = MATH_MATHML;
      }
      else {
	fprintf(stderr, "%s: invalid --math=%s option.\n", progname, optarg);
	exit(1);
      }
      break;

    case OPT_MSWORD:
      if (is_ht) {
	fprintf(stderr, "%s: wrong option, use `--help'.\n", progname);
	exit(1);
      }
      msword = true;
      break;

    case OPT_MOZILLA:
      if (is_ht) {
	fprintf(stderr, "%s: wrong option, use `--help'.\n", progname);
	exit(1);
      }
      mozilla = true;
      break;

    case OPT_UNICODE:
      if (is_ht) {
	fprintf(stderr, "%s: wrong option, use `--help'.\n", progname);
	exit(1);
      }
      unicode = true;
      break;

    case OPT_DBCS:
      if (is_ht) {
	fprintf(stderr, "%s: wrong option, use `--help'.\n", progname);
	exit(1);
      }
      dbcs = true;
      break;

    case OPT_SYMBOL:
      if (is_ht) {
	fprintf(stderr, "%s: wrong option, use `--help'.\n", progname);
	exit(1);
      }
      symbol = true;
      break;

    case OPT_OUTPUT_DIR:
      output_dir = xstrdup(optarg);
      if (!dir_p(optarg)) {
	fprintf(stderr, "%s: can't output to directory %s\n", progname, optarg);
	exit(1);
      }
      output_dir = xstrdup(optarg);
      break;

    case OPT_OUTPUT_NAME:
      output_name = xstrdup(optarg);
      break;

    case OPT_INDEX_FILE:
      opt_index = true;
      break;

    case OPT_CLEANUP:
      cleanup = true;
      break;

    default:
      usage();
      break;
    }
  }

  if (is_ht) {
    texengine = argv[optind++];
  }
  else {
    texengine = argv[0]+2;
  }

  newargv = (char **)malloc(5 * sizeof(char *));
  
  newargv[0] = argv[0];
  /* shifting options from argv[] list */
  for (i = 1; optind < *argc; i++, optind++)
    newargv[i] = argv[optind];
  newargv[i] = NULL;
  *argc = i;
  for (i = *argc; i < 5; i++)
    newargv[i] = NULL;
  
  if (is_ht) {
    /* Nothing else to do */
  }
  else {

    if (dtd == DTD_TEI || dtd == DTD_EBOOK || dtd == DTD_DOCBOOK) {
      if (math == MATH_BITMAP && ! unicode) {
	fprintf(stderr, "%s: warning, TEI, eBook and DocBook dtds valid only with `--unicode' option.\nTurning on option `--unicode'.\n", progname);
	unicode = true;
	math = MATH_BITMAP_UNICODE;
      }
      else if (mozilla) {
	fprintf(stderr, "%s: warning, `--mozilla' valid only with XHTML dtd. Ignored.\n", progname);
	mozilla = false;
      }
    }
    
    if (dtd = DTD_HTML) {
      if (math == MATH_MATHML) {
	fprintf(stderr, "%s: warning, MathML incompatible with HTML dtd, reverting to bitmap math.\n", progname);
	math = MATH_BITMAP;
      }
      if (mozilla) {
	fprintf(stderr, "%s: warning, `--mozilla' valid only with XHTML dtd. Ignored.\n", progname);
	mozilla = false;
      }
    }
    
    if (msword) {
      if (dtd == DTD_HTML) {
	dtd = DTD_HTML4WORD;
      }
      else if (dtd == DTD_XHTML) {
	dtd = DTD_XHTML4WORD;
      }
      else {
	fprintf(stderr, "%s: error, `--msword' incompatible with dtds other than HTML or XHTML. Exiting...\n", progname);
	mt_exit(1);
      }
      if (unicode) {
      fprintf(stderr, "%s: warning, `--unicode' incompatible with `--msword'. Exiting...\n", progname);
      mt_exit(1);
    }
      if (math == MATH_MATHML) {
	fprintf(stderr, "%s: warning, MathML incompatible with `--msword'. Exiting...\n", progname);
	mt_exit(1);
      }
      if (mozilla) {
      fprintf(stderr, "%s: warning, `--mozilla' incompatible with `--msword'. Exiting...\n", progname);
      mt_exit(1);
      }
    }
    
    if (math == MATH_BITMAP && unicode)
      math = MATH_BITMAP_UNICODE;
        
    if (TESTNZ(newargv[2]))
      newargv[2] = concat3(newargv[2], ",", stdcfg[math][dtd][0]);
    else
      newargv[2] = stdcfg[math][dtd][0];
    
    if (TESTNZ(newargv[3]))
      newargv[3] = concat3(newargv[3], ",", stdcfg[math][dtd][2]);
    else
      newargv[3] = stdcfg[math][dtd][2];
    
    if (dbcs)
      newargv[3] = concat("dbcs,", newargv[3]);
    if (symbol)
      newargv[3] = concat("symbol,", newargv[3]);
    
    if (*argc < 4)
      *argc = 4;
  }  
  return newargv;
}
  
int main(int argc, char *argv[])
{
  int i;
  int retval = 0;

  kpse_set_progname(argv[0]);
  progname = xstrdup(program_invocation_short_name);

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

  if (strlen(progname) > 4 && FILESTRNCASEEQ(progname + strlen(progname) - 4, ".exe", 4)) {
    *(progname + strlen(progname) - 4) = '\0';
  }
  is_ht = FILESTRCASEEQ(progname, "ht");

  argv = parse_arguments(&argc, argv);

#if 0
  fprintf(stderr, "%s:\n", progname);
  for (i = 1; i < argc; i++) 
    fprintf(stderr, "\t argv[%d] = %s\n", i, argv[i]);
  fprintf(stderr, "\nconfig(%d,%d) = ", math, dtd);
  for (i = 0; i < 3; i++)
    fprintf(stderr, "%s ", stdcfg[math][dtd][i]);
  fprintf(stderr, "\n");
#endif
  
  texargs = (force ? "--interaction=nonstopmode" : "");

  if (TESTZ(argv[1])) {
    fprintf(stderr, "%s: error, no file specified.\n", progname);
    exit(1);
  }

  texsrc = xstrdup(argv[1]);

#if 0
  /* Rely on latex / kpathsea to find the right source file. */
  if ((strlen(texsrc) < 4)
      || _strnicmp(texsrc + strlen(texsrc) - 4, ".tex", 4) != 0) {
    texsrc = concat(texsrc, ".tex");
  }
#endif

  texfile = xstrdup(texsrc);

  if ((strlen(texfile) >= 4) 
      && _strnicmp(texfile + strlen(texfile) - 4, ".tex", 4) == 0) {
    *(texfile + strlen(texfile) - 4) = '\0';
  }

  if (TESTZ(output_name)) {
    output_name = xstrdup(texfile);
  }
  else {
    if ((strlen(output_name) >= 4) 
	&& _strnicmp(output_name + strlen(output_name) - 4, ".tex", 4) == 0) {
      *(output_name + strlen(output_name) - 4) = '\0';
    }
    texargs = concat3(texargs, " --jobname=", output_name);
  }

#if 0
  // copy the original name to output name if needed
  if (TESTNZ(output_dir)) {
    texfile = concat3(output_dir, "\\", output_name);
  }
  else if (TESTNZ(output_name)) {
    texfile = xstrdup(output_name);
  }
  else 
    texfile = NULL;

  if (texfile) {
    unixtodos_filename(texfile);
    
    if (dry_run) {
      fprintf(stderr, "%s: copying %s to %s\n", progname, texsrc, texfile);
    }
    else {
      if (CopyFile(texsrc, texfile, false) == 0) {
	fprintf(stderr, "%s: failed to copy %s to %s (Error %d)\n",
		progname, texsrc, texfile, GetLastError());
      }
    }
    free(texfile);
  }

  texfile = xstrdup(output_name);

  if (TESTNZ(output_dir)) {
    pushd(output_dir);
    xputenv("KPSE_DOT", cwd);
    xputenv("TEXINPUTS", concatn(cwd, "/", output_dir, ";", NULL));
    if (dry_run) {
      fprintf(stderr, "%s: changing directory to %s\n", progname, output_dir);
      fprintf(stderr, "%s: setting KPSE_DOT to %s\n", progname, cwd);
    }
  }
#endif
  if (is_ht) {
    retval = ht(argc, argv);
  }
  else if (FILESTRCASEEQ(progname, "httex")) {
    retval = ht_engine(runstr_tex, argc, argv);
  }
  else if (FILESTRCASEEQ(progname, "htlatex")) {
    retval = ht_engine(runstr_latex, argc, argv);
  }
  else if (FILESTRCASEEQ(progname, "httexi")) {
    retval = ht_engine(runstr_texi, argc, argv);
  }
  else {
    fprintf(stderr, "%s: %s is unknown, aborting.\n", argv[0], progname);
    retval = 1;
  }

  if (opt_index) {
    /* copy dest_dir/output_name.html to dest_dir/index.html */
    char *destfile = output_name;
    char *indexfile = "index.html";

    if (TESTNZ(output_dir)) {
      destfile = concat3(output_dir, "\\", output_name);
      indexfile = concat(output_dir, "\\index.html");
    }

    if ((strlen(destfile) < 5) 
	|| _strnicmp(destfile + strlen(destfile) - 5, ".html", 5) != 0) {
      destfile = concat(destfile, ".html");
    }
    
    if (CopyFile(destfile, indexfile, false) == 0) {
      fprintf(stderr, "%s: failed to copy %s to %s (Error %d)\n",
	      progname, destfile, indexfile, GetLastError());
    }
  }

  mt_exit(retval);

  /* Not Reached */
  return retval;
}
