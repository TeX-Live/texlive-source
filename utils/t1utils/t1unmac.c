/* t1unmac/unpost
 *
 * This program converts Macintosh Type 1 fonts stored in MacBinary (I or II),
 * AppleSingle, AppleDouble, BinHex, or raw resource fork format to PFA and
 * PFB formats.
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
 * Revision 1.2  92/06/23  10:57:33  ilh
 * MSDOS porting by Kai-Uwe Herbing (herbing@netmbx.netmbx.de)
 * incoporated.
 * 
 * Revision 1.1  92/05/22  12:07:49  ilh
 * initial version
 *
 * Ported to Microsoft C/C++ Compiler and MS-DOS operating system by
 * Kai-Uwe Herbing (herbing@netmbx.netmbx.de) on June 12, 1992. Code
 * specific to the MS-DOS version is encapsulated with #ifdef _MSDOS
 * ... #endif, where _MSDOS is an identifier, which is automatically
 * defined, if you compile with the Microsoft C/C++ Compiler.
 * */

/* Note: this is ANSI C. */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#if defined(_MSDOS) || defined(_WIN32)
# include <fcntl.h>
# include <getopt.h>
# include <io.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <lcdf/clp.h>
#include "t1lib.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Some functions to read one, two, three, and four byte integers in 68000
   byte order (most significant byte first). */

static int
read_one(FILE *fi)
{
  return getc(fi);
}

static int
read_two(FILE *fi)
{
  int val;

  val = getc(fi);
  val = (val << 8) | getc(fi);

  return val;
}

static int32_t
read_three(FILE *fi)
{
  int32_t val;

  val = getc(fi);
  val = (val << 8) | getc(fi);
  val = (val << 8) | getc(fi);

  return val;
}

static int32_t
read_four(FILE *fi)
{
  int32_t val;

  val = getc(fi);
  val = (val << 8) | getc(fi);
  val = (val << 8) | getc(fi);
  val = (val << 8) | getc(fi);

  return val;
}

/* reposition a file with error messages */

static void
reposition(FILE *fi, int32_t absolute)
{
  if (fseek(fi, absolute, 0) == -1)
    fatal_error("can't seek to position %d\n\
   (The Mac file may be corrupted, or you may need the `-r' option.)",
		absolute);
}

static int blocklen = -1;

static int hex_column = 0;	/* current column of hex ASCII output */

static void
output_hex_byte(FILE *fo, int b)
{
  static const char *hex = "0123456789abcdef";
  
  if (hex_column >= blocklen) {
    putc('\n', fo);
    hex_column = 0;
  }
  putc(hex[b >> 4], fo);
  putc(hex[b & 0xf], fo);
  hex_column += 2;
}

/* Function to extract a particular POST resource.  Offset points to the four
   byte length which is followed by the data.  The first byte of the POST data
   specifies resource type: 1 for ASCII, 2 for binary, and 5 for end.  The
   second byte is always zero. */


/* Function to write four byte length to PFB file: least significant byte
   first. */

static int
extract_data(FILE *fi, FILE *fo, struct pfb_writer *w, int32_t offset, int pfb)
{
  enum PS_type { PS_ascii = 1, PS_binary = 2, PS_end = 5 };
  static int last_type = -1;
  static int skip_newline = 0;
  int32_t len;
  int more = 1;
  int i, c;

  reposition(fi, offset);
  len = read_four(fi) - 2;	/* subtract type field */

  switch ((enum PS_type)read_one(fi)) {
    
   case PS_ascii: {
     (void) read_one(fi);
     if (last_type != PFB_ASCII && pfb) {
       pfb_writer_output_block(w);
       w->blocktyp = PFB_ASCII;
     }
     for (i = 0; i < len; i++) {
       c = read_one(fi);
       if (c == '\n' && skip_newline) {
	 skip_newline = 0;
	 continue;
       }
       if (c == '\r') {
	 c = '\n';
	 skip_newline = 1;
       } else
	 skip_newline = 0;
       if (pfb)
	 PFB_OUTPUT_BYTE(w, c);
       else
	 putc(c, fo);
     }
     last_type = PFB_ASCII;
     break;
   }
   
   case PS_binary: {
     (void) read_one(fi);
     if (last_type != PFB_BINARY && pfb) {
       pfb_writer_output_block(w);
       w->blocktyp = PFB_BINARY;
     } else if (last_type != PFB_BINARY)
       hex_column = 0;
     if (pfb) {
       while (len--)
	 PFB_OUTPUT_BYTE(w, read_one(fi));
     } else {
       while (len--)
	 output_hex_byte(fo, read_one(fi));
     }
     last_type = PFB_BINARY;
     break;
   }

   case PS_end:
    more = 0;
    break;
    
  }
  
  return more;
}


/*****
 * Command line
 **/

#define OUTPUT_OPT	301
#define VERSION_OPT	302
#define HELP_OPT	303
#define PFB_OPT		304
#define PFA_OPT		305
#define MACBINARY_OPT	306
#define RAW_OPT		307
#define LINE_LEN_OPT	308
#define APPLEDOUBLE_OPT	309
#define BINHEX_OPT	310

static Clp_Option options[] = {
  { "applesingle", 0, APPLEDOUBLE_OPT, 0, 0 },
  { "appledouble", 0, APPLEDOUBLE_OPT, 0, 0 },
  { "binhex", 0, BINHEX_OPT, 0, 0 },
  { "block-length", 0, LINE_LEN_OPT, Clp_ArgUnsigned, 0 },
  { "help", 0, HELP_OPT, 0, 0 },
  { "line-length", 'l', LINE_LEN_OPT, Clp_ArgUnsigned, 0 },
  { "macbinary", 0, MACBINARY_OPT, 0, 0 },
  { "output", 'o', OUTPUT_OPT, Clp_ArgString, 0 },
  { "pfa", 'a', PFA_OPT, 0, 0 },
  { "pfb", 'b', PFB_OPT, 0, 0 },
  { "raw", 'r', RAW_OPT, 0, 0 },
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
  fprintf(stderr, "Usage: %s [OPTION]... INPUT [OUTPUT]\n\
Try `%s --help' for more information.\n",
	  program_name, program_name);
}

void
usage(void)
{
  printf("\
`T1unmac' extracts a PostScript Type 1 font from a Macintosh font file. It can\n\
read MacBinary, AppleSingle, AppleDouble, or BinHex files, or raw Macintosh\n\
resource forks. The result is written to the standard output unless an OUTPUT\n\
file is given.\n\
\n\
Usage: %s [OPTION]... INPUT [OUTPUT]\n\
\n\
Options:\n\
  -r, --raw                   Input is a raw Macintosh resource fork.\n\
      --macbinary             Input is in MacBinary format.\n\
      --applesingle           Input is in AppleSingle format.\n\
      --appledouble           Input is in AppleDouble format.\n\
      --binhex                Input is in BinHex format.\n\
  -a, --pfa                   Output font in ASCII (PFA) format.\n\
  -b, --pfb                   Output font in binary (PFB) format. This is\n\
                              the default.\n\
  -l, --block-length NUM      Set max block length for PFB output.\n\
  -l, --line-length NUM       Set max encrypted line length for PFA output.\n\
  -o, --output FILE           Write output to FILE.\n\
  -h, --help                  Print this message and exit.\n\
      --version               Print version number and warranty and exit.\n\
\n\
Report bugs to <kohler@icir.org>.\n", program_name);
}


static const char *
check_macbinary(FILE *ifp)
{
  int i, j;
  char buf[124];

  /* check "version" bytes at offsets 0 and 74 */
  reposition(ifp, 0);
  if (read_one(ifp) != 0)
    return "bad version byte";
  reposition(ifp, 74);
  if (read_one(ifp) != 0)
    return "bad version byte";
  
#if 0
  /* write out bullshit */
  {int t;
    reposition(ifp, 65);
    t = read_four(ifp);
    fprintf(stderr, "Type %c%c%c%c\n", (t>>24)&255, (t>>16)&255, (t>>8)&255, t&255);
    t = read_four(ifp);
    fprintf(stderr, "Creator %c%c%c%c\n", (t>>24)&255, (t>>16)&255, (t>>8)&255, t&255);
    t = read_one(ifp);
    fprintf(stderr, "Finder flags 1 %02x\n", t);
    read_one(ifp);
    t = read_two(ifp);
    fprintf(stderr, "horizpos %04x\n", t);
    t = read_two(ifp);
    fprintf(stderr, "vertpos %04x\n", t);
    t = read_two(ifp);
    fprintf(stderr, "folder id %d\n", t);
    t = read_one(ifp);
    fprintf(stderr, "protected? %x\n", t);
    t = read_one(ifp);
    t = read_four(ifp);
    fprintf(stderr, "data len %d\n", t);
    t = read_four(ifp);
    fprintf(stderr, "rsrc len %d\n", t);
    t = read_four(ifp);
    {
      struct tm *crap; 
      fprintf(stderr, "creation date %x\n", t);
      t -= 2082844800;
      fprintf(stderr, "   %s\n", ctime(&t));
      t = read_four(ifp);
      fprintf(stderr, "modification date %x\n", t);
      t -= 2082844800;
      fprintf(stderr, "   %s\n", ctime(&t));
      t = read_two(ifp);
    }
    fprintf(stderr, "getinfo len %d\n", t);
    t = read_one(ifp);
    fprintf(stderr, "finderflags 2 %02x\n", t);
    reposition(ifp, 116);
    t = read_four(ifp);
    fprintf(stderr, "total len %d\n", t);
    t = read_two(ifp);
    fprintf(stderr, "secondary header len %d\n", t);
    t = read_one(ifp);
    fprintf(stderr, "version %d\n", t);
    t = read_one(ifp);
    fprintf(stderr, "version %d\n", t);
  }
#endif

  /* check file length */
  reposition(ifp, 1);
  i = read_one(ifp);
  if (i > 63)
    return "bad length";
  reposition(ifp, 83);
  i = read_four(ifp);
  j = read_four(ifp);
  if (i < 0 || j < 0 || i >= 0x800000 || j >= 0x800000)
    return "bad length";

  /* check CRC */
  reposition(ifp, 0);
  fread(buf, 1, 124, ifp);
  if (crcbuf(0, 124, buf) != read_two(ifp)) {
    reposition(ifp, 82);
    if (read_one(ifp) != 0)
      return "bad checksum";
  }

  return 0;
}

#define APPLESINGLE_MAGIC 0x00051600
#define APPLEDOUBLE_MAGIC 0x00051607

const char *
check_appledouble(FILE *ifp)
{
  int i;
  reposition(ifp, 0);
  i = read_four(ifp);
  if (i != APPLEDOUBLE_MAGIC && i != APPLESINGLE_MAGIC)
    return "bad magic number";

  return 0;
}

const char *
translate_binhex(FILE *f, FILE *tmpf)
{
  int i, c = 0, last_char, after_x90, bits, bitpos;
  unsigned char value_table[256];

  /* prepare value table */
  {
    const char *table = "!\"#$%&'()*+,-012345689@ABCDEFGHIJKLMNPQRSTUVXYZ[`abcdefhijklmpqr";
    for (i = 0; i < 256; i++)
      value_table[i] = 255;
    for (i = 0; *table; i++, table++)
      value_table[(unsigned char)*table] = i;
  }
  
  /* skip to comment */
  {
    const char *comment = "(This file must be converted with BinHex";
    while (!feof(f)) {
      const char *s;
      for (s = comment; *s; s++) {
	c = getc(f);
	if (c != *s)
	  break;
      }
      /* skip to end of line */
      while (c >= 0 && c != '\n' && c != '\r')
	c = getc(f);
      /* stop if read comment */
      if (!*s)
	goto found_comment;
    }
    /* failed */
    return "no comment";
  }

 found_comment:
  /* skip spaces, look for ':' */
  for (c = ' '; isspace(c); c = getc(f)) ;
  if (c != ':')
    return "no file-start character";

  /* found ':', process until you find another ':' */
  last_char = -1;
  after_x90 = bits = 0;
  bitpos = 10;
  for (c = getc(f); c >= 0; c = getc(f))
    if (!isspace(c)) {
      /* add 6 bits to bits */
      if (value_table[c] == 255)
	break;
      bits |= (value_table[c] << bitpos);
      bitpos -= 6;

      /* output character(s) */
      if (bitpos <= 2) {
	int d = bits >> 8;
	bits = (bits << 8) & 0xFF00;
	bitpos += 8;
	if (after_x90) {
	  /* handle compression */
	  if (d == 0) {
	    last_char = 0x90;
	    putc(0x90, tmpf);
	  } else
	    for (i = 1; i < d; i++)
	      putc(last_char, tmpf);
	  after_x90 = 0;
	} else if (d == 0x90)
	  after_x90 = 1;
	else {
	  last_char = d;
	  putc(d, tmpf);
	}
      }
    }
  
  if (c < 0)
    return "unexpected EOF";
  else if (c != ':')
    return "bad character";

  fflush(tmpf);
  return 0;
}

int
check_binhex_crc(FILE *f, int offset, int length)
{
  int crc = 0;
  char buf[2048];
  reposition(f, offset);
  while (length > 0) {
    int n = (length < 2048 ? length : 2048);
    fread(buf, 1, n, f);
    crc = crcbuf(crc, n, buf);
    length -= n;
  }
  return crc == 0;
}

const char *
check_binhex(FILE *f)
{
  int fname_len, data_len, rsrc_len, off;
  
  /* check lengths */
  reposition(f, 0);
  fname_len = read_one(f);
  if (fname_len < 1 || fname_len > 63)
    return "bad length";
  reposition(f, 1 + fname_len + 11);
  data_len = read_four(f);
  rsrc_len = read_four(f);
  if (data_len < 0 || rsrc_len < 0 || data_len >= 0x800000 || rsrc_len >= 0x800000)
    return "bad length";

  /* check version */
  reposition(f, 1 + fname_len);
  if (read_one(f) != 0)
    return "bad version";

  /* check CRC */
  off = 1 + fname_len + 21;
  if (!check_binhex_crc(f, 0, off))
    return "bad header CRC";
  if (!check_binhex_crc(f, off, data_len + 2))
    return "bad data CRC";
  if (!check_binhex_crc(f, off + data_len + 2, rsrc_len + 2))
    return "bad resource fork CRC";

  return 0;
}

#ifdef __cplusplus
}
#endif


int
main(int argc, char *argv[])
{
  FILE *ifp = 0;
  FILE *ofp = 0;
  struct pfb_writer w;
  const char *ifp_name = "<stdin>";
  int32_t res_offset, res_data_offset, res_map_offset, type_list_offset;
  int32_t post_type;
  int num_types, num_extracted = 0, pfb = 1;
  int raw = 0, appledouble = 0, binhex = 0, macbinary = 0;
  
  Clp_Parser *clp =
    Clp_NewParser(argc, (const char * const *)argv, sizeof(options) / sizeof(options[0]), options);
  program_name = Clp_ProgramName(clp);
  
  /* interpret command line arguments using CLP */
  while (1) {
    int opt = Clp_Next(clp);
    switch (opt) {
      
     case RAW_OPT:
      raw = 1;
      appledouble = binhex = macbinary = 0;
      break;
      
     case MACBINARY_OPT:
      macbinary = 1;
      raw = appledouble = binhex = 0;
      break;

     case APPLEDOUBLE_OPT:
      appledouble = 1;
      raw = binhex = macbinary = 0;
      break;

     case BINHEX_OPT:
      binhex = 1;
      raw = appledouble = macbinary = 0;
      break;
      
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
      
     case PFB_OPT:
      pfb = 1;
      break;
      
     case PFA_OPT:
      pfb = 0;
      break;
      
     case LINE_LEN_OPT:
      blocklen = clp->val.i;
      break;
      
     case HELP_OPT:
      usage();
      exit(0);
      break;
      
     case VERSION_OPT:
      printf("t1unmac (LCDF t1utils) %s\n", VERSION);
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
	ifp_name = clp->arg;
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
  _setmode(_fileno(ifp), _O_BINARY);
  /* If we are processing a PFB (binary) output */
  /* file, we must set its file mode to binary. */
  if (pfb)
    _setmode(_fileno(ofp), _O_BINARY);
#endif
  
  if (pfb)
    init_pfb_writer(&w, blocklen, ofp);
  else {
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

  /* check for non-seekable input */
  if (fseek(ifp, 0, 0)) {
    char buf[2048];
    FILE *tmp = tmpfile();
    if (!tmp)
      fatal_error("cannot open temporary file: %s", strerror(errno));
    while (!feof(ifp)) {
      int i = fread(buf, 1, 2048, ifp);
      if (i > 0)
	fwrite(buf, 1, i, tmp);
    }
    if (ferror(ifp))
      fatal_error("%s: %s", ifp_name, strerror(errno));
    reposition(tmp, 0);
    fflush(tmp);
    if (ifp != stdin)
      fclose(ifp);
    ifp = tmp;
  }
  
  /* check for empty file */
  fseek(ifp, 0, 2);
  if (ftell(ifp) == 0)
    fatal_error("%s: empty file\n\
  (Try re-transferring the files using MacBinary format.)",
		ifp_name);
  
  reposition(ifp, 0);
  if (!raw && !appledouble && !binhex && !macbinary) {
    /* check magic number, try to figure out what it is */
    int i, magic;
    magic = read_four(ifp);
    reposition(ifp, 0);

    if (magic == APPLESINGLE_MAGIC || magic == APPLEDOUBLE_MAGIC)
      appledouble = 1;
    else if ((magic & 0xFF000000) == 0)
      macbinary = 1;
    else {
      binhex = 1;
      for (i = 0; i < 4; i++, magic >>= 8)
	if (!isprint(magic & 0xFF) && !isspace(magic & 0xFF))
	  /* not an ASCII character, assume not BinHex */
	  binhex = 0;
    }

    if (!appledouble && !macbinary && !binhex)
      fatal_error("%s: unknown file type", ifp_name);
  }
  
  if (raw) {
    /* raw resource file */
    res_offset = 0;
    
  } else if (macbinary) {	/* MacBinary (I or II) file */
    const char *check;
    int32_t data_fork_size;

    /* check integrity of file */
    check = check_macbinary(ifp);
    if (check)
      fatal_error("%s: not a MacBinary file (%s)", ifp_name, check);
    
    /* read data and resource fork sizes in MacBinary header */
    reposition(ifp, 83);
    data_fork_size = read_four(ifp);
    (void) read_four(ifp);
    
    /* round data_fork_size up to multiple of 128 */
    if (data_fork_size % 128)
      data_fork_size += 128 - data_fork_size % 128;
    
    res_offset = 128 + data_fork_size;

  } else if (appledouble) {	/* AppleDouble file */
    const char *check;
    const char *applewhat;
    int i, n;

    /* check integrity */
    check = check_appledouble(ifp);
    if (check)
      fatal_error("%s: not an AppleDouble file (%s)", ifp_name, check);
    reposition(ifp, 0);
    if (read_four(ifp) == APPLESINGLE_MAGIC)
      applewhat = "AppleSingle";
    else
      applewhat = "AppleDouble";
    
    /* find offset to resource and/or data fork */
    reposition(ifp, 24);
    n = read_two(ifp);
    res_offset = -1;
    for (i = 0; i < n; i++) {
      int type = read_four(ifp);
      if (type == 0)
	fatal_error("%s: bad %s file (bad entry descriptor)", ifp_name, applewhat);
      if (type == 2)		/* resource fork entry */
	res_offset = read_four(ifp);
      else
	(void) read_four(ifp);
      (void) read_four(ifp);
    }
    if (res_offset < 0)
      fatal_error("%s: bad %s file (no resource fork)", ifp_name, applewhat);

  } else if (binhex) {		/* BinHex file */
    const char *check;
    FILE *tmpf = tmpfile();
    if (!tmpf)
      fatal_error("cannot open temporary file: %s", strerror(errno));

    /* check integrity */
    check = translate_binhex(ifp, tmpf);
    if (check)
      fatal_error("%s: not a BinHex file (%s)", ifp_name, check);
    check = check_binhex(tmpf);
    if (check)
      fatal_error("%s: bad BinHex file (%s)", ifp_name, check);

    /* find resource offset */
    reposition(tmpf, 0);
    res_offset = read_one(tmpf);
    reposition(tmpf, 1 + res_offset + 11);
    res_offset += 22 + read_four(tmpf) + 2;
    if (ifp != stdin)
      fclose(ifp);
    ifp = tmpf;
    
  } else {
    fatal_error("%s: strange format", ifp_name);
    exit(1);
  }
  
  /* read offsets from resource fork header */
  reposition(ifp, res_offset);
  res_data_offset = res_offset + read_four(ifp);
  res_map_offset = res_offset + read_four(ifp);
  
  /* read type list offset from resource map header */
  reposition(ifp, res_map_offset + 24);
  type_list_offset = res_map_offset + read_two(ifp);
  
  /* read type list */
  reposition(ifp, type_list_offset);
  num_types = read_two(ifp) + 1;
  
  /* find POST type */
  post_type =  (int32_t)('P' & 0xff) << 24;
  post_type |= (int32_t)('O' & 0xff) << 16;
  post_type |= (int32_t)('S' & 0xff) << 8;
  post_type |= (int32_t)('T' & 0xff);
  
  while (num_types--) {
    if (read_four(ifp) == post_type) {
      int nrsrc = 1 + read_two(ifp);
      int list_offset = type_list_offset + read_two(ifp);
      int rsrc_pos = 0;
      int want_id = 501;
      int second_time = 1;
      reposition(ifp, list_offset);
      /* read resources sequentially, starting with ID 501, until we encounter
	 an "end" resource or we can't find the next resource */
      while (rsrc_pos < nrsrc) {
	int offset = ftell(ifp);
	int id = read_two(ifp);
	if (id == want_id) {
	  (void) read_two(ifp);
	  (void) read_one(ifp);
	  num_extracted++;
	  if (!extract_data(ifp, ofp, &w, res_data_offset + read_three(ifp), pfb))
	    break;
	  second_time = 0;
	  want_id++;
	}
	reposition(ifp, offset + 12);
	rsrc_pos++;
	if (rsrc_pos >= nrsrc && !second_time) {
	  reposition(ifp, list_offset);
	  rsrc_pos = 0;
	}
      }
      break;
    } else {
      (void) read_two(ifp);
      (void) read_two(ifp);
    }
  }

#if 0
  system("/bin/rm -f /tmp/x.*");
  {
    FILE *f;
    int i;
    reposition(ifp, res_offset + 16);
    if ((f = fopen("/tmp/x.systemarea", "wb"))) {
      for (i = res_offset + 16; i < res_data_offset; i++) {
	putc(getc(ifp), f);
      }
      fclose(f);
    }
  }
  reposition(ifp, type_list_offset);
  num_types = read_two(ifp) + 1;
  while (num_types--) {
    int t = read_four(ifp);
    int num_of_type = 1 + read_two(ifp);
    int32_t save_offset = ftell(ifp) + 2;
    reposition(ifp, type_list_offset + read_two(ifp));
    while (num_of_type--) {
      FILE *f;
      char buf[2048];
      int x, i, attrs;
      x = ftell(ifp);
      i = read_two(ifp);	/* ID */
      read_two(ifp);
      attrs = read_one(ifp);
      sprintf(buf, "/tmp/x.%c%c%c%c.%d", (t>>24)&255, (t>>16)&255, (t>>8)&255, t&255, i);
      fprintf(stderr, "%c%c%c%c.%d %d", (t>>24)&255, (t>>16)&255, (t>>8)&255, t&255, i, attrs);
      if ((f = fopen(buf, "wb"))) {
	int l;
	reposition(ifp, res_data_offset + read_three(ifp));
	l = read_four(ifp);
	fprintf(stderr, " %d\n", l);
	while (l > 0) {
	  int n = (l < 2048 ? l : 2048);
	  fread(buf, 1, n, ifp);
	  fwrite(buf, 1, n, f);
	  l -= n;
	}
	fclose(f);
      }
      reposition(ifp, x + 12);
    }
    reposition(ifp, save_offset);
  }
#endif

  if (pfb)
    pfb_writer_end(&w);
  if (num_extracted == 0)
    error("%s: not a Type 1 font (no POST resources)", ifp_name);
  
  fclose(ifp);
  fclose(ofp);
  return 0;
}
