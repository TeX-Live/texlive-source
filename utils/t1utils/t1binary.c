/* t1binary
 *
 * This program takes an Adobe Type-1 font program in ASCII (PFA) format and
 * converts it to binary (PFB) format.
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
 * Eddie Kohler <kohler@icir.org>.
 *
 * New change log in `NEWS'. Old change log:
 *
 * Revision 1.2  92/06/23  10:58:08  ilh
 * MSDOS porting by Kai-Uwe Herbing (herbing@netmbx.netmbx.de)
 * incoporated.
 * 
 * Revision 1.1  92/05/22  11:58:17  ilh
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

typedef unsigned char byte;

/* for PFB block buffering */
static struct pfb_writer w;


/* PFB font_reader functions */

static void
pfb_output_ascii(char *s, int len)
{
    if (w.blocktyp == PFB_BINARY) {
	pfb_writer_output_block(&w);
	w.blocktyp = PFB_ASCII;
    }
    for (; len > 0; len--, s++)
	PFB_OUTPUT_BYTE(&w, (byte)*s);
}

static void
pfb_output_binary(unsigned char *s, int len)
{
  if (w.blocktyp == PFB_ASCII) {
    pfb_writer_output_block(&w);
    w.blocktyp = PFB_BINARY;
  }
  for (; len > 0; len--, s++)
    PFB_OUTPUT_BYTE(&w, *s);
}

static void
pfb_output_end()
{
  pfb_writer_end(&w);
}


/*****
 * Command line
 **/

#define BLOCK_LEN_OPT	300
#define OUTPUT_OPT	301
#define VERSION_OPT	302
#define HELP_OPT	303

static Clp_Option options[] = {
  { "block-length", 'l', BLOCK_LEN_OPT, Clp_ArgInt, 0 },
  { "help", 0, HELP_OPT, 0, 0 },
  { "length", 0, BLOCK_LEN_OPT, Clp_ArgInt, 0 },
  { "output", 'o', OUTPUT_OPT, Clp_ArgString, 0 },
  { "version", 0, VERSION_OPT, 0, 0 },
};
static const char *program_name;


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


void
short_usage(void)
{
  fprintf(stderr, "Usage: %s [OPTION]... [INPUT [OUTPUT]]\n\
Try `%s --help' for more information.\n",
	  program_name, program_name);
}


void
usage(void)
{
  printf("\
`T1binary' translates a PostScript Type 1 font from ASCII (PFA) to compact\n\
binary (PFB) format. The result is written to the standard output unless an\n\
OUTPUT file is given.\n\
\n\
Usage: %s [OPTION]... [INPUT [OUTPUT]]\n\
\n\
Options:\n\
  -l, --block-length=NUM        Set max output block length.\n\
  -o, --output=FILE             Write output to FILE.\n\
  -h, --help                    Print this message and exit.\n\
      --version                 Print version number and warranty and exit.\n\
\n\
Report bugs to <kohler@icir.org>.\n", program_name);
}

#ifdef __cplusplus
}
#endif


int
main(int argc, char *argv[])
{
  int c;
  FILE *ifp = 0, *ofp = 0;
  const char *ifp_filename = "<stdin>";
  struct font_reader fr;
  int max_blocklen = -1;
  
  Clp_Parser *clp =
    Clp_NewParser(argc, (const char * const *)argv, sizeof(options) / sizeof(options[0]), options);
  program_name = Clp_ProgramName(clp);
  
  /* interpret command line arguments using CLP */
  while (1) {
    int opt = Clp_Next(clp);
    switch (opt) {
      
     case BLOCK_LEN_OPT:
      max_blocklen = clp->val.i;
      if (max_blocklen <= 0) {
	max_blocklen = 1;
	error("warning: block length raised to %d", max_blocklen);
      }
      break;
      
     output_file:
     case OUTPUT_OPT:
      if (ofp)
	fatal_error("output file already specified");
      if (strcmp(clp->arg, "-") == 0)
	ofp = stdout;
      else {
	ofp = fopen(clp->arg, "wb");
	if (!ofp) fatal_error("%s: %s", clp->arg, strerror(errno));
      }
      break;
      
     case HELP_OPT:
      usage();
      exit(0);
      break;
      
     case VERSION_OPT:
      printf("t1binary (LCDF t1utils) %s\n", VERSION);
      printf("Copyright (C) 1992-2003 I. Lee Hetherington, Eddie Kohler et al.\n\
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
      if (strcmp(clp->arg, "-") == 0)
	ifp = stdin;
      else {
	ifp_filename = clp->arg;
	ifp = fopen(clp->arg, "r");
	if (!ifp) fatal_error("%s: %s", clp->arg, strerror(errno));
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
  if (!ifp) ifp = stdin;
  if (!ofp) ofp = stdout;
  
#if defined(_MSDOS) || defined(_WIN32)
  /* As we are processing a PFB (binary) output */
  /* file, we must set its file mode to binary. */
  _setmode(_fileno(ofp), _O_BINARY);
#endif
  
  /* prepare font reader and pfb writer */
  fr.output_ascii = pfb_output_ascii;
  fr.output_binary = pfb_output_binary;
  fr.output_end = pfb_output_end;
  init_pfb_writer(&w, max_blocklen, ofp);
  
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
  
  if (!w.binary_blocks_written)
    fatal_error("no binary blocks written! Are you sure this was a font?");
  
  return 0;
}
