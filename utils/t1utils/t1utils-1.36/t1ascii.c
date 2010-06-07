/* t1ascii
 *
 * This program takes an Adobe Type-1 font program in binary (PFB) format and
 * converts it to ASCII (PFA) format.
 *
 * Copyright (c) 1992 by I. Lee Hetherington, all rights reserved.
 *
 * Permission is hereby granted to use, modify, and distribute this program
 * for any purpose provided this copyright notice and the one below remain
 * intact.
 *
 * I. Lee Hetherington (ilh@lcs.mit.edu)
 *
 * 1.5 and later versions contain changes by, and are maintained by,
 * Eddie Kohler <ekohler@gmail.com>.
 *
 * New change log in `NEWS'. Old change log:
 *
 * Revision 1.2  92/06/23  10:58:43  ilh
 * MSDOS porting by Kai-Uwe Herbing (herbing@netmbx.netmbx.de)
 * incoporated.
 *
 * Revision 1.1  92/05/22  11:47:24  ilh
 * initial version
 *
 * Ported to Microsoft C/C++ Compiler and MS-DOS operating system by
 * Kai-Uwe Herbing (herbing@netmbx.netmbx.de) on June 12, 1992. Code
 * specific to the MS-DOS version is encapsulated with #ifdef _MSDOS
 * ... #endif, where _MSDOS is an identifier, which is automatically
 * defined, if you compile with the Microsoft C/C++ Compiler.
 *
 */

/* Note: this is ANSI C. */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#if defined(_MSDOS) || defined(_WIN32)
# include <fcntl.h>
# include <io.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <errno.h>
#include <lcdf/clp.h>
#include "t1lib.h"

#ifdef __cplusplus
extern "C" {
#endif

static FILE *ofp;
static int line_length = 64;


/*****
 * Command line
 **/

#define OUTPUT_OPT	301
#define VERSION_OPT	302
#define HELP_OPT	303
#define LINE_LEN_OPT	304
#define WARNINGS_OPT	305

static Clp_Option options[] = {
  { "help", 0, HELP_OPT, 0, 0 },
  { "line-length", 'l', LINE_LEN_OPT, Clp_ValInt, 0 },
  { "output", 'o', OUTPUT_OPT, Clp_ValString, 0 },
  { "version", 0, VERSION_OPT, 0, 0 },
  { "warnings", 'w', WARNINGS_OPT, 0, Clp_Negate }
};
static const char *program_name;
static const char *ifp_filename = "<stdin>";
static int line_length_warning = -1;

void
fatal_error(const char *message, ...)
{
  va_list val;
  va_start(val, message);
  fprintf(stderr, "%s: ", program_name);
  vfprintf(stderr, message, val);
  putc('\n', stderr);
  exit(1);
}

void
error(const char *message, ...)
{
  va_list val;
  va_start(val, message);
  fprintf(stderr, "%s: ", program_name);
  vfprintf(stderr, message, val);
  putc('\n', stderr);
}

static void
short_usage(void)
{
  fprintf(stderr, "Usage: %s [OPTION]... [INPUT [OUTPUT]]\n\
Try `%s --help' for more information.\n",
	  program_name, program_name);
}

static void
usage(void)
{
  printf("\
`T1ascii' translates a PostScript Type 1 font from compact binary (PFB) to\n\
ASCII (PFA) format. The result is written to the standard output unless an\n\
OUTPUT file is given.\n\
\n\
Usage: %s [OPTION]... [INPUT [OUTPUT]]\n\
\n\
Options:\n\
  -l, --line-length=NUM         Set max encrypted line length (default 64).\n\
  -o, --output=FILE             Write output to FILE.\n\
  -w, --warnings                Warn on too-long lines.\n\
  -h, --help                    Print this message and exit.\n\
      --version                 Print version number and warranty and exit.\n\
\n\
Report bugs to <ekohler@gmail.com>.\n", program_name);
}


/*****
 * PFA font_reader functions
 **/

static int hexcol = 0;

static void
pfa_output_ascii(char *data, int len)
{
    if (hexcol) {
	putc('\n', ofp);
	hexcol = 0;
    }
    if (line_length_warning == 0 && len > 256) {
	line_length_warning = 1;
	fprintf(stderr, "%s: warning: %s has lines longer than 255 characters\n%s: (This may cause problems with older printers.)\n", program_name, ifp_filename, program_name);
    }
    fputs(data, ofp);
    if (len && data[len - 1] != '\n') {
	int p = len - 2;
	while (p > 0 && data[p] != '\n')
	    p--;
	hexcol = (p ? len - p - 1 : hexcol + len);
    }
}

static void
pfa_output_binary(unsigned char *data, int len)
{
  static const char *hexchar = "0123456789abcdef";
  for (; len > 0; len--, data++) {
    /* trim hexadecimal lines to line_length columns */
    if (hexcol >= line_length) {
      putc('\n', ofp);
      hexcol = 0;
    }
    putc(hexchar[(*data >> 4) & 0xf], ofp);
    putc(hexchar[*data & 0xf], ofp);
    hexcol += 2;
  }
}

static void
pfa_output_end(void)
{
}

#ifdef __cplusplus
}
#endif


/*****
 * main()
 **/

int
main(int argc, char *argv[])
{
  struct font_reader fr;
  int c;
  FILE *ifp = 0;

  Clp_Parser *clp =
    Clp_NewParser(argc, (const char * const *)argv, sizeof(options) / sizeof(options[0]), options);
  program_name = Clp_ProgramName(clp);

  /* interpret command line arguments using CLP */
  while (1) {
    int opt = Clp_Next(clp);
    switch (opt) {

     case LINE_LEN_OPT:
      line_length = clp->val.i;
      if (line_length < 8) {
	line_length = 8;
	error("warning: line length raised to %d", line_length);
      } else if (line_length > 1024) {
	line_length = 1024;
	error("warning: line length lowered to %d", line_length);
      }
      break;

     output_file:
     case OUTPUT_OPT:
      if (ofp)
	fatal_error("output file already specified");
      if (strcmp(clp->vstr, "-") == 0)
	ofp = stdout;
      else {
	ofp = fopen(clp->vstr, "w");
	if (!ofp) fatal_error("%s: %s", clp->vstr, strerror(errno));
      }
      break;

     case WARNINGS_OPT:
       line_length_warning = (clp->negated ? -1 : 0);
       break;

     case HELP_OPT:
      usage();
      exit(0);
      break;

     case VERSION_OPT:
      printf("t1ascii (LCDF t1utils) %s\n", VERSION);
      printf("Copyright (C) 1992-2010 I. Lee Hetherington, Eddie Kohler et al.\n\
This is free software; see the source for copying conditions.\n\
There is NO warranty, not even for merchantability or fitness for a\n\
particular purpose.\n");
      exit(0);
      break;

     case Clp_NotOption:
      if (ifp && ofp)
	fatal_error("too many arguments");
      else if (ifp)
	goto output_file;
      if (strcmp(clp->vstr, "-") == 0) {
	ifp_filename = "<stdin>";
	ifp = stdin;
      } else {
	ifp_filename = clp->vstr;
	ifp = fopen(clp->vstr, "rb");
	if (!ifp)
	    fatal_error("%s: %s", clp->vstr, strerror(errno));
      }
      break;

     case Clp_Done:
      goto done;

     case Clp_BadOption:
      short_usage();
      exit(1);
      break;

    }
  }

 done:
  if (!ifp)
      ifp = stdin;
  if (!ofp)
      ofp = stdout;
  if (line_length > 255 && line_length_warning == 0)
      fprintf(stderr, "%s: warning: selected --line-length is greater than 255\n", program_name);

#if defined(_MSDOS) || defined(_WIN32)
  /* As we are processing a PFB (binary) input */
  /* file, we must set its file mode to binary. */
  _setmode(_fileno(ifp), _O_BINARY);
#endif

  /* prepare font reader */
  fr.output_ascii = pfa_output_ascii;
  fr.output_binary = pfa_output_binary;
  fr.output_end = pfa_output_end;

  /* peek at first byte to see if it is the PFB marker 0x80 */
  c = getc(ifp);
  ungetc(c, ifp);

  /* do the file */
  if (c == PFB_MARKER)
    process_pfb(ifp, ifp_filename, &fr);
  else if (c == '%')
    process_pfa(ifp, ifp_filename, &fr);
  else
    fatal_error("%s does not start with font marker (`%%' or 0x80)", ifp_filename);

  fclose(ifp);
  fclose(ofp);
  return 0;
}
