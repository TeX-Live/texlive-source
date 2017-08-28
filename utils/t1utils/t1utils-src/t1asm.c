/* t1asm        -*- c-basic-offset: 2 -*-
 *
 * This program `assembles' Adobe Type-1 font programs in pseudo-PostScript
 * form into either PFB or PFA format.  The human readable/editable input is
 * charstring- and eexec-encrypted as specified in the `Adobe Type 1 Font
 * Format' version 1.1 (the `black book').  There is a companion program,
 * t1disasm, which `disassembles' PFB and PFA files into a pseudo-PostScript
 * file.
 *
 * Copyright (c) 1992 by I. Lee Hetherington, all rights reserved.
 * Copyright (c) 1998-2017 Eddie Kohler
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, subject to the
 * conditions listed in the Click LICENSE file, which is available in full at
 * http://github.com/kohler/click/blob/master/LICENSE. The conditions
 * include: you must preserve this copyright notice, and you cannot mention
 * the copyright holders in advertising related to the Software without
 * their permission. The Software is provided WITHOUT ANY WARRANTY, EXPRESS
 * OR IMPLIED. This notice is a summary of the Click LICENSE file; the
 * license in that file is binding.
 *
 * New change log in `NEWS'. Old change log:
 *
 * Revision 1.4  92/07/10  10:53:09  ilh
 * Added support for additional PostScript after the closefile command
 * (ie., some fonts have {restore}if after the cleartomark).
 *
 * Revision 1.3  92/06/23  10:58:25  ilh
 * MSDOS porting by Kai-Uwe Herbing (herbing@netmbx.netmbx.de)
 * incoporated.
 *
 * Revision 1.2  92/05/22  11:54:45  ilh
 * Fixed bug where integers larger than 32000 could not be encoded in
 * charstrings.  Now integer range is correct for four-byte
 * twos-complement integers: -(1<<31) <= i <= (1<<31)-1.  Bug detected by
 * Piet Tutelaers (rcpt@urc.tue.nl).
 *
 * Revision 1.1  92/05/22  11:48:46  ilh
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
#include "t1asmhelp.h"

#define LINESIZE 512

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char byte;

static FILE *ifp;
static FILE *ofp;
static struct pfb_writer w;
static int blocklen = -1;

/* flags */
static int pfb = 1;
static int active = 0;
static int ever_active = 0;
static int start_charstring = 0;
static int in_eexec = 0;

/* need to add 1 as space for \0 */
static char line[LINESIZE + 1];

/* for charstring buffering */
static byte *charstring_buf, *charstring_bp;
static int charstring_bufsiz;

/* decryption stuff */
static uint16_t er, cr;
static const uint32_t c1 = 52845;
static const uint32_t c2 = 22719;

/* table of charstring commands */
static struct command {
  const char *name;
  int one, two;
} command_table[] = {
  { "abs", 12, 9 },             /* Type 2 */
  { "add", 12, 10 },            /* Type 2 */
  { "and", 12, 3 },             /* Type 2 */
  { "blend", 16, -1 },          /* Type 2 */
  { "callgsubr", 29, -1 },      /* Type 2 */
  { "callother", 12, 16 },      /* Type 1 ONLY */
  { "callothersubr", 12, 16 },  /* Type 1 ONLY */
  { "callsubr", 10, -1 },
  { "closepath", 9, -1 },       /* Type 1 ONLY */
  { "cntrmask", 20, -1 },       /* Type 2 */
  { "div", 12, 12 },
  { "dotsection", 12, 0 },      /* Type 1 ONLY */
  { "drop", 12, 18 },           /* Type 2 */
  { "dup", 12, 27 },            /* Type 2 */
  { "endchar", 14, -1 },
  { "eq", 12, 15 },             /* Type 2 */
  { "error", 0, -1 },           /* special */
  { "escape", 12, -1 },         /* special */
  { "exch", 12, 28 },           /* Type 2 */
  { "flex", 12, 35 },           /* Type 2 */
  { "flex1", 12, 37 },          /* Type 2 */
  { "get", 12, 21 },            /* Type 2 */
  { "hflex", 12, 34 },          /* Type 2 */
  { "hflex1", 12, 36 },         /* Type 2 */
  { "hhcurveto", 27, -1 },      /* Type 2 */
  { "hintmask", 19, -1 },       /* Type 2 */
  { "hlineto", 6, -1 },
  { "hmoveto", 22, -1 },
  { "hsbw", 13, -1 },           /* Type 1 ONLY */
  { "hstem", 1, -1 },
  { "hstem3", 12, 2 },          /* Type 1 ONLY */
  { "hstemhm", 18, -1 },        /* Type 2 */
  { "hvcurveto", 31, -1 },
  { "ifelse", 12, 22 },         /* Type 2 */
  { "index", 12, 29 },          /* Type 2 */
  { "load", 12, 13 },           /* Type 2 */
  { "mul", 12, 24 },            /* Type 2 */
  { "neg", 12, 14 },            /* Type 2 */
  { "not", 12, 5 },             /* Type 2 */
  { "or", 12, 4 },              /* Type 2 */
  { "pop", 12, 17 },            /* Type 1 ONLY */
  { "put", 12, 20 },            /* Type 2 */
  { "random", 12, 23 },         /* Type 2 */
  { "rcurveline", 24, -1 },     /* Type 2 */
  { "return", 11, -1 },
  { "rlinecurve", 25, -1 },     /* Type 2 */
  { "rlineto", 5, -1 },
  { "rmoveto", 21, -1 },
  { "roll", 12, 30 },           /* Type 2 */
  { "rrcurveto", 8, -1 },
  { "sbw", 12, 7 },             /* Type 1 ONLY */
  { "seac", 12, 6 },            /* Type 1 ONLY */
  { "setcurrentpoint", 12, 33 }, /* Type 1 ONLY */
  { "sqrt", 12, 26 },           /* Type 2 */
  { "store", 12, 8 },           /* Type 2 */
  { "sub", 12, 11 },            /* Type 2 */
  { "vhcurveto", 30, -1 },
  { "vlineto", 7, -1 },
  { "vmoveto", 4, -1 },
  { "vstem", 3, -1 },
  { "vstem3", 12, 1 },          /* Type 1 ONLY */
  { "vstemhm", 23, -1 },        /* Type 2 */
  { "vvcurveto", 26, -1 },      /* Type 2 */
};                                                /* alphabetical */

/* Two separate encryption functions because eexec and charstring encryption
   must proceed in parallel. */

static byte eencrypt(byte plain)
{
  byte cipher;

  cipher = (byte)(plain ^ (er >> 8));
  er = (uint16_t)((cipher + er) * c1 + c2);
  return cipher;
}

static byte cencrypt(byte plain)
{
  byte cipher;

  /* Thanks to Tom Kacvinsky <tjk@ams.org> who reported that lenIV == -1 means
     unencrypted charstrings. */
  if (lenIV < 0) return plain;

  cipher = (byte)(plain ^ (cr >> 8));
  cr = (uint16_t)((cipher + cr) * c1 + c2);
  return cipher;
}

/* This function outputs a single byte.  If output is in PFB format then output
   is buffered through blockbuf[].  If output is in PFA format, then output
   will be hexadecimal if in_eexec is set, ASCII otherwise. */

static void output_byte(byte b)
{
  static const char *hexchar = "0123456789abcdef";
  static int hexcol = 0;

  if (pfb) {
    /* PFB */
    PFB_OUTPUT_BYTE(&w, b);
  } else {
    /* PFA */
    if (in_eexec) {
      /* trim hexadecimal lines to `blocklen' columns */
      if (hexcol >= blocklen) {
        putc('\n', ofp);
        hexcol = 0;
      }
      putc(hexchar[(b >> 4) & 0xf], ofp);
      putc(hexchar[b & 0xf], ofp);
      hexcol += 2;
    } else {
      putc(b, ofp);
    }
  }
}

/* This function outputs a byte through possible eexec encryption. */

static void eexec_byte(byte b)
{
  if (in_eexec)
    output_byte(eencrypt(b));
  else
    output_byte(b);
}

/* This function outputs a null-terminated string through possible eexec
   encryption. */

static void eexec_string(const char *string)
{
  while (*string)
    eexec_byte(*string++);
}

/* This function gets ready for the eexec-encrypted data.  If output is in
   PFB format then flush current ASCII block and get ready for binary block.
   We start encryption with four random (zero) bytes. */

static void eexec_start(char *string)
{
  eexec_string("currentfile eexec\n");
  if (pfb && w.blocktyp != PFB_BINARY) {
    pfb_writer_output_block(&w);
    w.blocktyp = PFB_BINARY;
  }

  in_eexec = 1;
  er = 55665;
  eexec_byte(0);
  eexec_byte(0);
  eexec_byte(0);
  eexec_byte(0);
  eexec_string(string);
}

/* 25.Aug.1999 -- Return 1 if this line actually looks like the start of a
   charstring. We use the heuristic that it should start with `/' (a name) or
   `dup' (a subroutine). Previous heuristic caused killa bad output. */

static int check_line_charstring(void)
{
  char *p = line;
  while (isspace((unsigned char) *p))
    p++;
  return (*p == '/' || (p[0] == 'd' && p[1] == 'u' && p[2] == 'p'));
}

/* This function returns an input line of characters. A line is terminated by
   length (including terminating null) greater than LINESIZE, \r, \n, \r\n, or
   when active (looking for charstrings) by '{'. When terminated by a newline
   the newline is put into line[]. When terminated by '{', the '{' is not put
   into line[], and the flag start_charstring is set to 1. */

static void t1utils_getline(void)
{
  int c;
  char *p = line;
  int comment = 0;
  start_charstring = 0;

  while (p < line + LINESIZE) {
    c = getc(ifp);

    if (c == EOF)
      break;
    else if (c == '%')
      comment = 1;
    else if (active && !comment && c == '{') {
      /* 25.Aug.1999 -- new check for whether we should stop be active */
      if (check_line_charstring()) {
        start_charstring = 1;
        break;
      } else
        active = 0;
    }

    *p++ = (char) c;

    /* end of line processing: change CR or CRLF into LF, and exit */
    if (c == '\r') {
      c = getc(ifp);
      if (c != '\n')
        ungetc(c, ifp);
      p[-1] = '\n';
      break;
    } else if (c == '\n')
      break;
  }

  *p = '\0';
}

/* This function wraps-up the eexec-encrypted data and writes ASCII trailer.
   If output is in PFB format then this entails flushing binary block and
   starting an ASCII block. */

static void eexec_end(void)
{
  int i, j;

  if (!pfb)
    putc('\n', ofp);
  else if (w.blocktyp != PFB_ASCII) {
    pfb_writer_output_block(&w);
    w.blocktyp = PFB_ASCII;
  }

  in_eexec = active = 0;

  for (i = 0; i < 8; i++) {
    for (j = 0; j < 64; j++)
      eexec_byte('0');
    eexec_byte('\n');
  }
}

/* This function is used by the binary search, bsearch(), for command names in
   the command table. */

static int CDECL command_compare(const void *key, const void *item)
{
  return strcmp((const char *) key, ((const struct command *) item)->name);
}

/* This function returns 1 if the string is an integer and 0 otherwise. */

static int is_integer(char *string)
{
  if (isdigit((unsigned char) string[0]) || string[0] == '-' || string[0] == '+') {
    while (*++string && isdigit((unsigned char) *string))
      ;                                           /* deliberately empty */
    if (!*string)
      return 1;
  }
  return 0;
}

/* This function initializes charstring encryption.  Note that this is called
   at the beginning of every charstring. */

static void charstring_start(void)
{
  int i;

  if (!charstring_buf) {
    charstring_bufsiz = 65536;
    if (!(charstring_buf = (byte *) malloc(charstring_bufsiz)))
      fatal_error("out of memory");
  }

  charstring_bp = charstring_buf;
  cr = 4330;
  for (i = 0; i < lenIV; i++)
    *charstring_bp++ = cencrypt((byte) 0);
}

/* This function encrypts and buffers a single byte of charstring data. */

static void charstring_byte(int v)
{
  byte b = (byte)(v & 0xff);
  if (charstring_bp - charstring_buf == charstring_bufsiz) {
    charstring_bufsiz *= 2;
    if (!(charstring_buf = (byte *) realloc(charstring_buf, charstring_bufsiz)))
      fatal_error("out of memory");
    charstring_bp = charstring_buf + charstring_bufsiz / 2;
  }
  *charstring_bp++ = cencrypt(b);
}

/* This function outputs buffered, encrypted charstring data through possible
   eexec encryption. */

static void charstring_end(void)
{
  byte *bp;

  sprintf(line, "%d ", (int) (charstring_bp - charstring_buf));
  eexec_string(line);
  sprintf(line, "%s ", cs_start);
  eexec_string(line);
  for (bp = charstring_buf; bp < charstring_bp; bp++)
    eexec_byte(*bp);
}

/* This function generates the charstring representation of an integer. */

static void charstring_int(int num)
{
  int x;

  if (num >= -107 && num <= 107) {
    charstring_byte(num + 139);
  } else if (num >= 108 && num <= 1131) {
    x = num - 108;
    charstring_byte(x / 256 + 247);
    charstring_byte(x % 256);
  } else if (num >= -1131 && num <= -108) {
    x = abs(num) - 108;
    charstring_byte(x / 256 + 251);
    charstring_byte(x % 256);
  } else if (num >= (-2147483647-1) && num <= 2147483647) {
    charstring_byte(255);
    charstring_byte(num >> 24);
    charstring_byte(num >> 16);
    charstring_byte(num >> 8);
    charstring_byte(num);
  } else {
    error("can't format huge number `%d'", num);
    /* output 0 instead */
    charstring_byte(139);
  }
}

/* This function returns one charstring token. It ignores comments. */

static void get_charstring_token(void)
{
  int c = getc(ifp);
  while (isspace(c))
    c = getc(ifp);

  if (c == '%') {
    while (c != EOF && c != '\r' && c != '\n')
      c = getc(ifp);
    get_charstring_token();

  } else if (c == '}') {
    line[0] = '}';
    line[1] = 0;

  } else {
    char *p = line;
    while (p < line + LINESIZE) {
      *p++ = c;
      c = getc(ifp);
      if (c == EOF || isspace(c) || c == '%' || c == '}') {
        ungetc(c, ifp);
        break;
      }
    }
    *p = 0;
  }
}


/* This function parses an entire charstring into integers and commands,
   outputting bytes through the charstring buffer. */

static void parse_charstring(void)
{
  struct command *cp;

  charstring_start();
  while (!feof(ifp)) {
    get_charstring_token();
    if (line[0] == '}')
      break;
    if (is_integer(line)) {
      charstring_int(atoi(line));
    } else {
      int one;
      int two;
      int ok = 0;

      cp = (struct command *)
        bsearch((void *) line, (void *) command_table,
                sizeof(command_table) / sizeof(struct command),
                sizeof(struct command),
                command_compare);

      if (cp) {
        one = cp->one;
        two = cp->two;
        ok = 1;

      } else if (strncmp(line, "escape_", 7) == 0) {
        /* Parse the `escape' keyword requested by Lee Chun-Yu and Werner
           Lemberg */
        one = 12;
        if (sscanf(line + 7, "%d", &two) == 1)
          ok = 1;

      } else if (strncmp(line, "UNKNOWN_", 8) == 0) {
        /* Allow unanticipated UNKNOWN commands. */
        one = 12;
        if (sscanf(line + 8, "12_%d", &two) == 1)
          ok = 1;
        else if (sscanf(line + 8, "%d", &one) == 1) {
          two = -1;
          ok = 1;
        }
      }

      if (!ok)
        error("unknown charstring command `%s'", line);
      else if (one < 0 || one > 255)
        error("bad charstring command number `%d'", one);
      else if (two > 255)
        error("bad charstring command number `%d'", two);
      else if (two < 0)
        charstring_byte(one);
      else {
        charstring_byte(one);
        charstring_byte(two);
      }
    }
  }
  charstring_end();
}


/*****
 * Command line
 **/

#define BLOCK_LEN_OPT   300
#define OUTPUT_OPT      301
#define VERSION_OPT     302
#define HELP_OPT        303
#define PFB_OPT         304
#define PFA_OPT         305

static Clp_Option options[] = {
  { "block-length", 'l', BLOCK_LEN_OPT, Clp_ValInt, 0 },
  { "help", 0, HELP_OPT, 0, 0 },
  { "line-length", 0, BLOCK_LEN_OPT, Clp_ValInt, 0 },
  { "output", 'o', OUTPUT_OPT, Clp_ValString, 0 },
  { "pfa", 'a', PFA_OPT, 0, 0 },
  { "pfb", 'b', PFB_OPT, 0, 0 },
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
  va_end(val);
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
  va_end(val);
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
`T1asm' translates a human-readable version of a PostScript Type 1 font into\n\
standard PFB or PFA format. The result is written to the standard output\n\
unless an OUTPUT file is given.\n\
\n\
Usage: %s [OPTION]... [INPUT [OUTPUT]]\n\
\n\
Options:\n\
  -a, --pfa                   Output font in ASCII (PFA) format.\n\
  -b, --pfb                   Output font in binary (PFB) format. This is\n\
                              the default.\n\
  -l, --block-length NUM      Set max block length for PFB output.\n\
  -l, --line-length NUM       Set max encrypted line length for PFA output.\n\
  -o, --output=FILE           Write output to FILE.\n\
  -h, --help                  Print this message and exit.\n\
      --version               Print version number and warranty and exit.\n\
\n\
Report bugs to <ekohler@gmail.com>.\n", program_name);
}

#ifdef __cplusplus
}
#endif


int main(int argc, char *argv[])
{
  char *p, *q;

  Clp_Parser *clp =
    Clp_NewParser(argc, (const char * const *)argv, sizeof(options) / sizeof(options[0]), options);
  program_name = Clp_ProgramName(clp);

  /* interpret command line arguments using CLP */
  while (1) {
    int opt = Clp_Next(clp);
    switch (opt) {

     case BLOCK_LEN_OPT:
      blocklen = clp->val.i;
      break;

     output_file:
     case OUTPUT_OPT:
      if (ofp)
        fatal_error("output file already specified");
      if (strcmp(clp->vstr, "-") == 0)
        ofp = stdout;
      else if (!(ofp = fopen(clp->vstr, "w")))
        fatal_error("%s: %s", clp->vstr, strerror(errno));
      break;

     case PFB_OPT:
      pfb = 1;
      break;

     case PFA_OPT:
      pfb = 0;
      break;

     case HELP_OPT:
      usage();
      exit(0);
      break;

     case VERSION_OPT:
      printf("t1asm (LCDF t1utils) %s\n", VERSION);
      printf("Copyright (C) 1992-2017 I. Lee Hetherington, Eddie Kohler et al.\n\
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
      if (strcmp(clp->vstr, "-") == 0)
        ifp = stdin;
      else if (!(ifp = fopen(clp->vstr, "r")))
        fatal_error("%s: %s", clp->vstr, strerror(errno));
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
  if (!pfb) {
    if (blocklen == -1)
      blocklen = 64;
    else if (blocklen < 8) {
      blocklen = 8;
      error("warning: line length raised to %d", blocklen);
    } else if (blocklen > 1024) {
      blocklen = 1024;
      error("warning: line length lowered to %d", blocklen);
    }
  }

  if (!ifp) ifp = stdin;
  if (!ofp) ofp = stdout;

  if (pfb)
    init_pfb_writer(&w, blocklen, ofp);

#if defined(_MSDOS) || defined(_WIN32)
  /* If we are processing a PFB (binary) output */
  /* file, we must set its file mode to binary. */
  if (pfb)
    _setmode(_fileno(ofp), _O_BINARY);
#endif

  /* Finally, we loop until no more input. Some special things to look for are
     the `currentfile eexec' line, the beginning of the `/Subrs' or
     `/CharStrings' definition, the definition of `/lenIV', and the definition
     of the charstring start command which has `...string currentfile...' in
     it.

     Being careful: Check with `/Subrs' and `/CharStrings' to see that a
     number follows the token -- otherwise, the token is probably nested in a
     subroutine a la Adobe Jenson, and we shouldn't pay attention to it.

     Bugs: Occurrence of `/Subrs 9' in a comment will fool t1asm.

     Thanks to Tom Kacvinsky <tjk@ams.org> who reported that some fonts come
     without /Subrs sections and provided a patch. */

  while (!feof(ifp) && !ferror(ifp)) {
    t1utils_getline();

    if (!ever_active) {
      if (strncmp(line, "currentfile eexec", 17) == 0 && isspace((unsigned char) line[17])) {
        /* Allow arbitrary whitespace after "currentfile eexec".
           Thanks to Tom Kacvinsky <tjk@ams.org> for reporting this.
           Note: strlen("currentfile eexec") == 17. */
        for (p = line + 18; isspace((unsigned char) *p); p++)
          ;
        eexec_start(p);
        continue;
      } else if (strncmp(line, "/lenIV", 6) == 0) {
        set_lenIV(line, strlen(line));
      } else if ((p = strstr(line, "string currentfile"))) {
        set_cs_start(line, strlen(line));
      }
    }

    if (!active) {
      if ((p = strstr(line, "/Subrs")) && isdigit((unsigned char) p[7]))
        ever_active = active = 1;
      else if ((p = strstr(line, "/CharStrings")) && isdigit((unsigned char) p[13]))
        ever_active = active = 1;
    }
    if ((p = strstr(line, "currentfile closefile"))) {
      /* 2/14/99 -- happy Valentine's day! -- don't look for `mark
         currentfile closefile'; the `mark' might be on a different line */
      /* 1/3/2002 -- happy new year! -- Luc Devroye reports a failure with
         some printers when `currentfile closefile' is followed by space */
      p += sizeof("currentfile closefile") - 1;
      for (q = p; isspace((unsigned char) *q) && *q != '\n'; q++)
        /* nada */;
      if (q == p && !*q)
        error("warning: `currentfile closefile' line too long");
      else if (q != p) {
        if (*q != '\n')
          error("text after `currentfile closefile' ignored");
        *p++ = '\n';
        *p++ = '\0';
      }
      eexec_string(line);
      break;
    }

    eexec_string(line);

    /* output line data */
    if (start_charstring) {
      if (!cs_start[0])
        fatal_error("couldn't find charstring start command");
      parse_charstring();
    }
  }

  /* Handle remaining PostScript after the eexec section */
  if (in_eexec)
    eexec_end();

  /* There may be additional code. */
  while (!feof(ifp) && !ferror(ifp)) {
    t1utils_getline();
    eexec_string(line);
  }

  if (pfb)
    pfb_writer_end(&w);

  /* the end! */
  if (!ever_active)
    error("warning: no charstrings found in input file");
  fclose(ifp);
  fclose(ofp);
  return 0;
}
