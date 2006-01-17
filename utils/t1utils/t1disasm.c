/* t1disasm
 *
 * This program `disassembles' Adobe Type-1 font programs in either PFB or PFA
 * format.  It produces a human readable/editable pseudo-PostScript file by
 * performing eexec and charstring decryption as specified in the `Adobe Type 1
 * Font Format' version 1.1 (the `black book').  There is a companion program,
 * t1asm, which `assembles' such a pseudo-PostScript file into either PFB or
 * PFA format.
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
 * Revision 1.4  92/07/10  10:55:08  ilh
 * Added support for additional PostScript after the closefile command
 * (ie., some fonts have {restore}if' after the cleartomark).  Also,
 * removed hardwired charstring start command (-| or RD) in favor of
 * automatically determining it.
 * 
 * Revision 1.3  92/06/23  10:57:53  ilh
 * MSDOS porting by Kai-Uwe Herbing (herbing@netmbx.netmbx.de)
 * incoporated.
 * 
 * Revision 1.2  92/05/22  12:05:33  ilh
 * Fixed bug where we were counting on sprintf to return its first
 * argument---not true in ANSI C.  This bug was detected by Piet
 * Tutelaers (rcpt@urc.tue.nl).  Also, fixed (signed) integer overflow
 * error when testing high-order bit of integer for possible
 * sign-extension by making comparison between unsigned integers.
 *
 * Revision 1.1  92/05/22  12:04:07  ilh
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

static FILE *ofp;
static int lenIV = 4;
static char cs_start[10];
static int unknown = 0;

/* decryption stuff */
static uint16_t c1 = 52845, c2 = 22719;
static uint16_t cr_default = 4330;
static uint16_t er_default = 55665;

static int error_count = 0;
void fatal_error(const char *message, ...);
void error(const char *message, ...);

/* If the line contains an entry of the form `/lenIV <num>' then set the global
   lenIV to <num>.  This indicates the number of random bytes at the beginning
   of each charstring. */

static void
set_lenIV(char *line)
{
  char *p = strstr(line, "/lenIV ");

  /* Allow lenIV to be negative. Thanks to Tom Kacvinsky <tjk@ams.org> */
  if (p && (isdigit(p[7]) || p[7] == '+' || p[7] == '-')) {
    lenIV = atoi(p + 7);
  }
}

static void
set_cs_start(char *line)
{
  char *p, *q, *r;

  if ((p = strstr(line, "string currentfile"))) {
    /* enforce presence of `readstring' -- 5/29/99 */
    if (!strstr(line, "readstring"))
      return;
    /* locate the name of the charstring start command */
    *p = '\0';					  /* damage line[] */
    q = strrchr(line, '/');
    if (q) {
      r = cs_start;
      ++q;
      while (!isspace(*q) && *q != '{')
	*r++ = *q++;
      *r = '\0';
    }
    *p = 's';					  /* repair line[] */
  }
}

/* Subroutine to output strings. */

static void
output(const char *string)
{
    fprintf(ofp, "%s", string);
}

/* Subroutine to neatly format output of charstring tokens.  If token = "\n",
   then a newline is output.  If at start of line (start == 1), prefix token
   with tab, otherwise a space. */

static void
output_token(const char *token)
{
    static int start = 1;
    if (strcmp(token, "\n") == 0) {
	fprintf(ofp, "\n");
	start = 1;
    } else {
	fprintf(ofp, "%s%s", start ? "\t" : " ", token);
	start = 0;
    }
}

/* Subroutine to decrypt and ASCII-ify tokens in charstring data. The
   charstring decryption machinery is fired up, skipping the first lenIV
   bytes, and the decrypted tokens are expanded into human-readable form. */

static void
decrypt_charstring(unsigned char *line, int len)
{
  int i;
  int32_t val;
  char buf[20];

  /* decrypt charstring */
  if (lenIV >= 0) {
    /* only decrypt if lenIV >= 0 -- negative lenIV means unencrypted
       charstring. Thanks to Tom Kacvinsky <tjk@ams.org> */
    uint16_t cr = cr_default;
    byte plain;
    for (i = 0; i < len; i++) {
      byte cipher = line[i];
      plain = (byte)(cipher ^ (cr >> 8));
      cr = (uint16_t)((cipher + cr) * c1 + c2);
      line[i] = plain;
    }
    line += lenIV;
    len -= lenIV;
  }

  /* handle each charstring command */
  for (i = 0; i < len; i++) {
    byte b = line[i];
    
    if (b >= 32) {
      if (b >= 32 && b <= 246)
	val = b - 139;
      else if (b >= 247 && b <= 250) {
	i++;
	val = (b - 247)*256 + 108 + line[i];
      } else if (b >= 251 && b <= 254) {
	i++;
	val = -(b - 251)*256 - 108 - line[i];
      } else {
	val =  (line[i+1] & 0xff) << 24;
	val |= (line[i+2] & 0xff) << 16;
	val |= (line[i+3] & 0xff) <<  8;
	val |= (line[i+4] & 0xff) <<  0;
	/* in case an int32 is larger than four bytes---sign extend */
#if INT_MAX > 0x7FFFFFFFUL
	if (val & 0x80000000)
	  val |= ~0x7FFFFFFF;
#endif
	i += 4;
      }
      sprintf(buf, "%d", val);
      output_token(buf);
      
    } else {
      switch (b) {
      case 0: output_token("error"); break;		/* special */
      case 1: output_token("hstem"); break;
      case 3: output_token("vstem"); break;
      case 4: output_token("vmoveto"); break;
      case 5: output_token("rlineto"); break;
      case 6: output_token("hlineto"); break;
      case 7: output_token("vlineto"); break;
      case 8: output_token("rrcurveto"); break;
      case 9: output_token("closepath"); break;		/* Type 1 ONLY */
      case 10: output_token("callsubr"); break;
      case 11: output_token("return"); break;
      case 13: output_token("hsbw"); break;		/* Type 1 ONLY */
      case 14: output_token("endchar"); break;
      case 16: output_token("blend"); break;		/* Type 2 */
      case 18: output_token("hstemhm"); break;		/* Type 2 */
      case 19: output_token("hintmask"); break;		/* Type 2 */
      case 20: output_token("cntrmask"); break;		/* Type 2 */
      case 21: output_token("rmoveto"); break;
      case 22: output_token("hmoveto"); break;
      case 23: output_token("vstemhm"); break;		/* Type 2 */
      case 24: output_token("rcurveline"); break;	/* Type 2 */
      case 25: output_token("rlinecurve"); break;	/* Type 2 */
      case 26: output_token("vvcurveto"); break;	/* Type 2 */
      case 27: output_token("hhcurveto"); break;	/* Type 2 */
      case 28: {		/* Type 2 */
	/* short integer */
	val =  (line[i+1] & 0xff) << 8;
	val |= (line[i+2] & 0xff);
	i += 2;
	if (val & 0x8000)
	  val |= ~0x7FFF;
	sprintf(buf, "%d", val);
	output_token(buf);
      }
      case 29: output_token("callgsubr"); break;	/* Type 2 */
      case 30: output_token("vhcurveto"); break;
      case 31: output_token("hvcurveto"); break;
      case 12:
	i++;
	b = line[i];
	switch (b) {
	case 0: output_token("dotsection"); break;	/* Type 1 ONLY */
	case 1: output_token("vstem3"); break;		/* Type 1 ONLY */
	case 2: output_token("hstem3"); break;		/* Type 1 ONLY */
	case 3: output_token("and"); break;		/* Type 2 */
	case 4: output_token("or"); break;		/* Type 2 */
	case 5: output_token("not"); break;		/* Type 2 */
	case 6: output_token("seac"); break;		/* Type 1 ONLY */
	case 7: output_token("sbw"); break;		/* Type 1 ONLY */
	case 8: output_token("store"); break;		/* Type 2 */
	case 9: output_token("abs"); break;		/* Type 2 */
	case 10: output_token("add"); break;		/* Type 2 */
	case 11: output_token("sub"); break;		/* Type 2 */
	case 12: output_token("div"); break;
	case 13: output_token("load"); break;		/* Type 2 */
	case 14: output_token("neg"); break;		/* Type 2 */
	case 15: output_token("eq"); break;		/* Type 2 */
	case 16: output_token("callothersubr"); break;	/* Type 1 ONLY */
	case 17: output_token("pop"); break;		/* Type 1 ONLY */
	case 18: output_token("drop"); break;		/* Type 2 */
	case 20: output_token("put"); break;		/* Type 2 */
	case 21: output_token("get"); break;		/* Type 2 */
	case 22: output_token("ifelse"); break;		/* Type 2 */
	case 23: output_token("random"); break;		/* Type 2 */
	case 24: output_token("mul"); break;		/* Type 2 */
	case 26: output_token("sqrt"); break;		/* Type 2 */
	case 27: output_token("dup"); break;		/* Type 2 */
	case 28: output_token("exch"); break;		/* Type 2 */
	case 29: output_token("index"); break;		/* Type 2 */
	case 30: output_token("roll"); break;		/* Type 2 */
	case 33: output_token("setcurrentpoint"); break;/* Type 1 ONLY */
	case 34: output_token("hflex"); break;		/* Type 2 */
	case 35: output_token("flex"); break;		/* Type 2 */
	case 36: output_token("hflex1"); break;		/* Type 2 */
	case 37: output_token("flex1"); break;		/* Type 2 */
	default:
	  sprintf(buf, "escape_%d", b);
	  unknown++;
	  output_token(buf);
	  break;
	}
	break;
      default:
       sprintf(buf, "UNKNOWN_%d", b);
       unknown++;
       output_token(buf);
       break;
      }
      output_token("\n");
    }
  }
  if (i > len) {
    output("\terror\n");
    error("disassembly error: charstring too short");
  }
}


/* Disassembly font_reader functions */

static int in_eexec = 0;
static unsigned char *save = 0;
static int save_len = 0;
static int save_cap = 0;

static void
append_save(unsigned char *line, int len)
{
  if (save_len + len >= save_cap) {
    unsigned char *new_save;
    if (!save_cap) save_cap = 1024;
    while (save_len + len >= save_cap) save_cap *= 2;
    new_save = (unsigned char *)malloc(save_cap);
    if (!new_save)
      fatal_error("out of memory");
    memcpy(new_save, save, save_len);
    free(save);
    save = new_save;
  }
  memcpy(save + save_len, line, len);
  save_len += len;
}

/* 23.Feb.2004 - use 'memstr', not strstr, because the strings input to
   eexec_line aren't null terminated! Reported by Werner Lemberg. */

static const char *
oog_memstr(const char *line, int line_len, const char *pattern, int pattern_len)
{
    const char *try;
    const char *last = line + line_len - pattern_len + 1;
    while (line < last
	   && (try = memchr(line, (unsigned char)*pattern, last - line))) {
	if (memcmp(try, pattern, pattern_len) == 0)
	    return try;
	else
	    line = try + 1;
    }
    return 0;
}

/* returns 1 if next \n should be deleted */

static int
eexec_line(unsigned char *line, int line_len)
{
    int cs_start_len = strlen(cs_start);
    int pos;
    int first_space;
    int digits;
    int cut_newline = 0;
  
    /* append this data to the end of `save' if necessary */
    if (save_len) {
	append_save(line, line_len);
	line = save;
	line_len = save_len;
    }
  
    if (!line_len)
	return 0;
  
    /* Look for charstring start */
  
    /* skip first word */
    for (pos = 0; pos < line_len && isspace(line[pos]); pos++)
	;
    while (pos < line_len && !isspace(line[pos]))
	pos++;
    if (pos >= line_len)
	goto not_charstring;
  
    /* skip spaces */
    first_space = pos;
    while (pos < line_len && isspace(line[pos]))
	pos++;
    if (pos >= line_len || !isdigit(line[pos]))
	goto not_charstring;

    /* skip number */
    digits = pos;
    while (pos < line_len && isdigit(line[pos]))
	pos++;
  
    /* check for subr (another number) */
    if (pos < line_len - 1 && isspace(line[pos]) && isdigit(line[pos+1])) {
	first_space = pos;
	digits = pos + 1;
	for (pos = digits; pos < line_len && isdigit(line[pos]); pos++)
	    ;
    }
  
    /* check for charstring start */
    if (pos + 2 + cs_start_len < line_len
	&& pos > digits
	&& line[pos] == ' '
	&& strncmp((const char *)(line + pos + 1), cs_start, cs_start_len) == 0
	&& line[pos + 1 + cs_start_len] == ' ') {
	/* check if charstring is long enough */
	int cs_len = atoi((const char *)(line + digits));
	if (pos + 2 + cs_start_len + cs_len < line_len) {
	    /* long enough! */
	    if (line[line_len - 1] == '\r') {
		line[line_len - 1] = '\n';
		cut_newline = 1;
	    }
	    fprintf(ofp, "%.*s {\n", first_space, line);
	    decrypt_charstring(line + pos + 2 + cs_start_len, cs_len);
	    pos += 2 + cs_start_len + cs_len;
	    fprintf(ofp, "\t}%.*s", line_len - pos, line + pos);
	    save_len = 0;
	    return cut_newline;
	} else {
	    /* not long enough! */
	    if (line != save)
		append_save(line, line_len);
	    return 0;
	}
    }
  
    /* otherwise, just output the line */
  not_charstring:
    /* 6.Oct.2003 - Werner Lemberg reports a stupid Omega font that behaves
       badly: a charstring definition follows "/Charstrings ... begin", ON THE
       SAME LINE. */
    {
	const char *CharStrings = oog_memstr(line, line_len, "/CharStrings ", 13);
	int crap, n;
	char should_be_slash = 0;
	if (CharStrings
	    && sscanf(CharStrings + 12, " %d dict dup begin %c%n", &crap, &should_be_slash, &n) >= 2
	    && should_be_slash == '/') {
	    int len = (CharStrings + 12 + n - 1) - (char *) line;
	    fprintf(ofp, "%.*s\n", len, line);
	    return eexec_line((unsigned char *) (CharStrings + 12 + n - 1), line_len - len);
	}
    }
  
    if (line[line_len - 1] == '\r') {
	line[line_len - 1] = '\n';
	cut_newline = 1;
    }
    set_lenIV((char *)line);
    set_cs_start((char *)line);
    fprintf(ofp, "%.*s", line_len, line);
    save_len = 0;

    /* look for `currentfile closefile' to see if we should stop decrypting */
    if (oog_memstr(line, line_len, "currentfile closefile", 21) != 0)
	in_eexec = -1;
  
    return cut_newline;
}

static int
all_zeroes(const char *string)
{
    if (*string != '0')
	return 0;
    while (*string == '0')
	string++;
    return *string == '\0' || *string == '\n';
}

static void
disasm_output_ascii(char *line, int len)
{
    int was_in_eexec = in_eexec;
    (void) len;			/* avoid warning */
    in_eexec = 0;

    /* if we came from a binary section, we need to process that too */
    if (was_in_eexec > 0) 
	eexec_line("", 0);

    /* if we just came from the "ASCII part" of an eexec section, we need to
       process the saved lines */
    if (was_in_eexec < 0) {
	int i = 0;
	int save_char = 0;	/* note: save[] is unsigned char * */
    
	while (i < save_len) {
	    /* grab a line */
	    int start = i;
	    while (i < save_len && save[i] != '\r' && save[i] != '\n')
		i++;
	    if (i < save_len) {
		if (i < save_len - 1 && save[i] == '\r' && save[i+1] == '\n')
		    save_char = -1;
		else
		    save_char = save[i+1];
		save[i] = '\n';
		save[i+1] = 0;
	    } else
		save[i] = 0;

	    /* output it */
	    disasm_output_ascii((char *)(save + start), -1);

	    /* repair damage */
	    if (i < save_len) {
		if (save_char >= 0) {
		    save[i+1] = save_char;
		    i++;
		} else
		    i += 2;
	    }
	}
	save_len = 0;
    }
  
    if (!all_zeroes(line))
	output(line);
}

/* collect until '\n' or end of binary section */

static void
disasm_output_binary(unsigned char *data, int len)
{
    static int ignore_newline;
    static uint16_t er;
  
    byte plain;
    int i;
  
    /* in the ASCII portion of a binary section, just save this data */
    if (in_eexec < 0) {
	append_save(data, len);
	return;
    }

    /* eexec initialization */
    if (in_eexec == 0) {
	er = er_default;
	ignore_newline = 0;
	in_eexec = 0;
    }
    if (in_eexec < 4) {
	for (i = 0; i < len && in_eexec < 4; i++, in_eexec++) {
	    byte cipher = data[i];
	    plain = (byte)(cipher ^ (er >> 8));
	    er = (uint16_t)((cipher + er) * c1 + c2);
	    data[i] = plain;
	}
	data += i;
	len -= i;
    }
  
    /* now make lines: collect until '\n' or '\r' and pass them off to
       eexec_line. */
    i = 0;
    while (in_eexec > 0) {
	int start = i;
    
	for (; i < len; i++) {
	    byte cipher = data[i];
	    plain = (byte)(cipher ^ (er >> 8));
	    er = (uint16_t)((cipher + er) * c1 + c2);
	    data[i] = plain;
	    if (plain == '\r' || plain == '\n')
		break;
	}
    
	if (ignore_newline && start < i && data[start] == '\n') {
	    ignore_newline = 0;
	    continue;
	}
    
	if (i >= len) {
	    if (start < len)
		append_save(data + start, i - start);
	    break;
	}

	i++;
	ignore_newline = eexec_line(data + start, i - start);
    }
  
    /* if in_eexec < 0, we have some plaintext lines sitting around in a binary
       section of the PFB. save them for later */
    if (in_eexec < 0 && i < len)
	append_save(data + i, len - i);
}

static void
disasm_output_end()
{
    /* take care of leftover saved data */
    static char crap[1] = "";
    disasm_output_ascii(crap, 0);
}


/*****
 * Command line
 **/

#define OUTPUT_OPT	301
#define VERSION_OPT	302
#define HELP_OPT	303

static Clp_Option options[] = {
  { "help", 0, HELP_OPT, 0, 0 },
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
  fputc('\n', stderr);
  exit(1);
}

void
error(const char *message, ...)
{
  va_list val;
  va_start(val, message);
  fprintf(stderr, "%s: ", program_name);
  vfprintf(stderr, message, val);
  fputc('\n', stderr);
  error_count++;
}

void
short_usage(void)
{
  fprintf(stderr, "Usage: %s [INPUT [OUTPUT]]\n\
Try `%s --help' for more information.\n",
	  program_name, program_name);
}

void
usage(void)
{
  printf("\
`T1disasm' translates a PostScript Type 1 font in PFB or PFA format into a\n\
human-readable, human-editable form. The result is written to the standard\n\
output unless an OUTPUT file is given.\n\
\n\
Usage: %s [OPTION]... [INPUT [OUTPUT]]\n\
\n\
Options:\n\
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
  struct font_reader fr;
  int c;
  FILE *ifp = 0;
  const char *ifp_filename = "<stdin>";
  
  Clp_Parser *clp =
    Clp_NewParser(argc, (const char * const *)argv, sizeof(options) / sizeof(options[0]), options);
  program_name = Clp_ProgramName(clp);
  
  /* interpret command line arguments using CLP */
  while (1) {
    int opt = Clp_Next(clp);
    switch (opt) {
      
     output_file:
     case OUTPUT_OPT:
      if (ofp)
	fatal_error("output file already specified");
      if (strcmp(clp->arg, "-") == 0)
	ofp = stdout;
      else {
	ofp = fopen(clp->arg, "w");
	if (!ofp) fatal_error("%s: %s", clp->arg, strerror(errno));
      }
      break;
      
     case HELP_OPT:
      usage();
      exit(0);
      break;
      
     case VERSION_OPT:
      printf("t1disasm (LCDF t1utils) %s\n", VERSION);
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
	ifp = fopen(clp->arg, "rb");
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
  /* As we might be processing a PFB (binary) input file, we must set its file
     mode to binary. */
  _setmode(_fileno(ifp), _O_BINARY);
#endif
  
  /* prepare font reader */
  fr.output_ascii = disasm_output_ascii;
  fr.output_binary = disasm_output_binary;
  fr.output_end = disasm_output_end;
  
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
  
  if (unknown)
    error((unknown > 1
	   ? "encountered %d unknown charstring commands"
	   : "encountered %d unknown charstring command"),
	  unknown);
  
  return (error_count ? 1 : 0);
}
