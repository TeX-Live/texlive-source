/*
  Purpose: call LATEX after preprocessing of the .tex file by
           the cjk conversion tool. The old f_name.bat script
	   is not working anymore.
  Author : F. Popineau <Fabrice.Popineau@supelec.fr>
  Date   : <29/06/2001>
*/

#ifndef LATEX
#define LATEX "latex"
#endif

/*
  Copyright (C) 2001-2012  F. Popineau <Fabrice.Popineau@supelec.fr>
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program in doc/COPYING; if not, write to the Free
  Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
  MA 02110-1301 USA
*/

#include <stdio.h>
#include <stdlib.h>

#include <kpathsea/config.h>
#include <kpathsea/lib.h>
#include <kpathsea/getopt.h>

static const char *cjklatex_version_string = "1.0";

static const char *usage_str[] = {
  "Usage: %s OPTIONS FILE\n",
  "Calls `" LATEX "' on FILE after conversion by the filter\n",
  "specified by OPTIONS.\n",
  "--conv=bg5\tfor traditional Chinese, encoding Big 5,\n",
  "--conv=cef\tfor Chinese Encoding Framework, encoding CEF,\n",
  "--conv=cef5\tidem CEF, also converts Big5 characters,\n",
  "--conv=cefs\tidem CEF, also converts SJIS characters,\n",
  "--conv=gbk\tfor Chinese, encoding GBK,\n",
  "--conv=sjis\tfor Japanese, SJIS encoding.",
  "\nAlternatively, for compatibility with the previous DOS batch files,\n",
  "you can also copy this program to any of the following names:\n",
  "bg5" LATEX ".exe, cef5" LATEX ".exe, cef" LATEX ".exe, cefs" LATEX ".exe,\n",
  "gbk" LATEX ".exe and sjis" LATEX ".exe .\n",
  "Then running one of these programs will be identical to specify\n",
  "the corresponding option.\n",
  "\nAdditional options:\n",
  "--verbose\tbe a bit more verbose about what is happening,\n",
  "--nocleanup\tdo not remove intermediate files,\n",
  "--latex=engine\tuse `engine' instead of `" LATEX "' to process the file.\n",
  NULL
};

static char *progname = NULL;

static struct _conv_table {
  const char *progname;
  const char *processor;
} CJKtable[] = {
  { "cjk" LATEX, "" },
  { "bg5" LATEX, "bg5conv" },
  { "cef" LATEX, "cefconv" },
  { "cef5" LATEX, "cef5conv" },
  { "cefs" LATEX, "cefsconv" },
  { "gbk" LATEX, "extconv" },
  { "sjis" LATEX, "sjisconv" }
};

#define PROGRAM_IS(p) FILESTRCASEEQ (p, progname)
#define ARGUMENT_IS(a) STREQ (long_options[option_index].name, a)

static int program_number = -1;
static int opt_verbose = 0;
static int opt_nocleanup = 0;
static char *texname, *cjkname, *texengine;

static struct option long_options [] = {
    { "debug",               1, 0, 0},
    { "help",                0, 0, 0},
    { "version",             0, 0, 0},
    { "verbose",             0, 0, 0},
    { "nocleanup",           0, 0, 0},
    { "latex",               1, 0, 0},
    { "conv",                1, 0, 0},
    {0, 0, 0, 0}
};

static BOOL sigint_handler(DWORD dwCtrlType)
{
  /* Fix me : there is a problem if a system() command is running.
     We should wait for the son process to be interrupted.
     Only way I can think of to do that : rewrite system() based on
     spawn() with parsing of the command line and set a global pid
     Next cwait(pid) in the HandlerRoutine. 
     */

  /* This is not that good, but else we would need to wait for 
     the child processes to finish ! */
  Sleep(250);

  fprintf(stderr, "Sending Ctrl+Break!\n");
  GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, 0);

  if (! opt_nocleanup)
    unlink(cjkname);

  exit(1);

  return FALSE;			/* return value obligatory */
}
   
static void usage(void)
{
  int i;
  fprintf(stderr, "CJK" LATEX " version %s\n", cjklatex_version_string);
  fprintf(stderr,usage_str[0], progname );
  fputs("\n", stderr);
  for(i = 1; usage_str[i]; ++i)
    fputs(usage_str[i], stderr);
}

static int do_process(const char *processor, const char *filename)
{
  char *ext, *p;
  char cmd[_MAX_PATH*3];
  DWORD dwFA;
  int ret;

  if (strlen(filename) > 4 && _strnicmp(filename + strlen(filename) - 4, ".tex", 4) == 0) {
    texname = xstrdup(filename);
  }
  else {
    texname = concat(filename, ".tex");
  }
  for (p = texname; p && *p; p++)
    *p = (*p == '\\' ? '/' : *p);

  dwFA = GetFileAttributes(texname);
  if (dwFA == 0xFFFFFFFF || (dwFA & FILE_ATTRIBUTE_DIRECTORY)) {
    fprintf(stderr, "%s: %s is an invalid input file.\n",
	    progname, texname);
    ret = 1;
  }
  else {
    cjkname = xstrdup(texname);
    ext = strrchr(cjkname, '.');
    assert(ext != NULL);
    strcpy(cjkname + (ext - cjkname), ".cjk");
    
    sprintf(cmd, "%s < %s > %s", processor, texname, cjkname);
    if (opt_verbose)
      fprintf(stderr, "%s: running command `%s'.\n", progname, cmd);
    ret = system(cmd);
    if (ret == 0) {
      sprintf(cmd, "%s %s", texengine, cjkname);
      if (opt_verbose)
	fprintf(stderr, "%s: running command `%s'.\n", progname, cmd);
      ret = system(cmd);
      if (! opt_nocleanup)
	unlink(cjkname);
    }
  }

  free(texname);
  free(cjkname);

  return ret;
}

int main(int argc, char *argv[])
{
  int g; /* getopt return code */
  int i;
  int option_index;
  char *filename;

  if (!progname)
    progname = argv[0];

  kpse_set_program_name (progname, NULL);
  progname = kpse_program_name;

  for (i = 0; i < sizeof(CJKtable)/sizeof(CJKtable[0]) && program_number < 0; i++) {
    if (STREQ(progname, CJKtable[i].progname)
#if 0
	|| (STREQ(progname, CJKtable[i].progname)
	    && STREQ(progname+strlen(CJKtable[i].progname), ".exe"))
#endif
	) {
      program_number = i;
    }
  }

  if (program_number == -1) {
    fprintf(stderr, "%s: this program has been incorrecty copied to the name %s.\n", progname, progname);
    usage();
    exit(1);
  }

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
    else if (ARGUMENT_IS ("verbose")) {
      opt_verbose = 1;
    }
    else if (ARGUMENT_IS ("nocleanup")) {
      opt_nocleanup = 1;
    }
    else if (ARGUMENT_IS ("latex")) {
      texengine = xstrdup(optarg);
    }
    else if (ARGUMENT_IS ("version")) {
      fprintf(stderr, "%s of %s.\n", progname, cjklatex_version_string);
      exit(0);
    }
    else if (PROGRAM_IS("cjk" LATEX)) {
      if (ARGUMENT_IS("conv")) {
	for (i = 1; i < sizeof(CJKtable)/sizeof(CJKtable[0]) && program_number <= 0; i++) {
	  if (STRNEQ(optarg, CJKtable[i].progname, strlen(optarg))) {
	    program_number = i;
	  }
	}
      }
    }
  }  

  /* shifting options from argv[] list */
  for (i = 1; optind < argc; i++, optind++)
    argv[i] = argv[optind];
  argv[i] = NULL;

  argc = i;

  if (argc < 2) {
    fprintf (stderr, "%s: Missing argument(s).\nTry `%s --help' for more information.\n", progname, kpse_program_name);
    exit(1);
  }

  if (argc > 2) {
    fprintf(stderr, "%s: Extra arguments", progname);
    for (i = 2; i < argc; i++)
      fprintf (stderr, " \"%s\"", argv[i]);
    fprintf (stderr, "\nTry `%s --help' for more information.\n",
	     kpse_program_name);
    exit(1);
  }

  SetConsoleCtrlHandler((PHANDLER_ROUTINE)sigint_handler, TRUE);

  filename = xstrdup(argv[1]);

  assert(program_number > 0);

  if (! texengine) {
    texengine = xstrdup(LATEX);
  }

  do_process(CJKtable[program_number].processor, filename);

  free(filename);
  free(texengine);

  return 0;
}
