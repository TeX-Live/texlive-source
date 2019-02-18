/* dt2dv - convert human-readable "DTL" file to DVI format
         - this is intended to invert dv2dt version 0.6.0
   
   This file is public domain.
   Originally written 1995, Geoffrey Tobin.
   The author has expressed the hope that any modification will retain
   enough content to remain useful. He would also appreciate being
   acknowledged as the original author in the documentation.
   This declaration added 2008/11/14 by Clea F. Rees with the permission
   of Geoffrey Tobin.

   - version 0.6.1 - 14:38 GMT +11  Thu 9 March 1995
   - Geoffrey Tobin    G.Tobin@ee.latrobe.edu.au
   - fixes:  Michal Tomczak-Jaegermann    ntomczak@vm.ucs.ualberta.ca
             Nelson H. F. Beebe    beebe@math.utah.edu
   - Reference:  "The DVI Driver Standard, Level 0",
                 by  The TUG DVI Driver Standards Committee.
                 Appendix A, "Device-Independent File Format".
*/

/* unix version; read from stdin, write to stdout, by default. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef KPATHSEA
#include <kpathsea/config.h>
#include <kpathsea/c-fopen.h>
#include <kpathsea/progname.h>
#endif

#include "dtl.h"

/* by default, read and write regular files */
int rd_stdin = 0;
int wr_stdout = 0;

/* maximum number of characters in a DTL input line */
#define MAXLINE  8192

/* input line */
typedef struct
{
  COUNT num;    /* current line number */
  size_t max;   /* capacity of buf */
  S4 wrote;     /* number of characters written into buf */
  size_t read;  /* position in buf of next character to read from buf */
  char * buf;   /* line buffer */
} Line;

char linebuf[MAXLINE+1];

Line dtl_line = {0, 0, 0, MAXLINE, linebuf};

/* a DTL token either is:
     a quoted string (admitting an escape character),
     or BCOM (if that is a nonempty string),
     or ECOM (if that is a nonempty string),
     or a string _not_ including ECOM_CHAR or space.
*/

/* maximum expected length of a DTL token */
#define MAXTOKLEN 255
typedef char Token[MAXTOKLEN+2];

typedef unsigned char Byte;
typedef char Boolean;

#ifndef true
#define true  1
#endif
#ifndef false
#define false 0
#endif

/* command prefixes */
typedef struct
{
    Byte first_code;
    const char * name;
    Boolean has_suffix;
    Byte first_suffix, last_suffix;
} CmdPrefix;

CmdPrefix  cmd_prefixes [] =
{
  {0,   SETCHAR, true, 0, 127},
  {128, SET, true, 1, 4},
  {132, SETRULE, false, 0, 0},
  {133, PUT, true, 1, 4},
  {137, PUTRULE, false, 0, 0},
  {138, NOP, false, 0, 0},
  {139, BOP, false, 0, 0},
  {140, EOP, false, 0, 0},
  {141, PUSH, false, 0, 0},
  {142, POP, false, 0, 0},
  {143, RIGHT, true, 1, 4},
  {147, W, true, 0, 4},
  {152, X, true, 0, 4},
  {157, DOWN, true, 1, 4},
  {161, Y, true, 0, 4},
  {166, Z, true, 0, 4},
  {171, FONTNUM, true, 0, 63},
  {235, FONT, true, 1, 4},
  {239, SPECIAL, true, 1, 4},
  {243, FONTDEF, true, 1, 4},
  {247, PRE, false, 0, 0},
  {248, POST, false, 0, 0},
  {249, POSTPOST, false, 0, 0},
  {250, OPCODE, true, 250, 255}
};
/* cmd_prefixes[] */

/* Number of DVI commands, including those officially undefined */
#define NCMDS 256

/* table of command name (string) pointers */
typedef char * CmdTable [NCMDS];

/* initially all command name pointers are NULL */
CmdTable cmd_table;

/* operation's opcode, name, number of args, string of arguments. */
typedef struct
{
  int code;
  const char * name;
  int nargs;
  const char * args;
} op_info;

/* name of table, first opcode, last opcode, pointer to opcode info. */
typedef struct
{
  const char * name;
  int first;
  int last;
  op_info * list;
} op_table;

/* Table for opcodes 128 to 170 inclusive. */

op_info  op_info_128_170 [] =
{
  {128, SET1, 1, "1"},
  {129, SET2, 1, "2"},
  {130, SET3, 1, "3"},
  {131, SET4, 1, "-4"},
  {132, SETRULE, 2, "-4 -4"},
  {133, PUT1, 1, "1"},
  {134, PUT2, 1, "2"},
  {135, PUT3, 1, "3"},
  {136, PUT4, 1, "-4"},
  {137, PUTRULE, 2, "-4 -4"},
  {138, NOP, 0, ""},
  /* bop:  not counting last argument, a signed address: */
  {139, BOP, 10, "-4 -4 -4 -4 -4 -4 -4 -4 -4 -4"},
  {140, EOP, 0, ""},
  {141, PUSH, 0, ""},
  {142, POP, 0, ""},
  {143, RIGHT1, 1, "-1"},
  {144, RIGHT2, 1, "-2"},
  {145, RIGHT3, 1, "-3"},
  {146, RIGHT4, 1, "-4"},
  {147, W0, 0, ""},
  {148, W1, 1, "-1"},
  {149, W2, 1, "-2"},
  {150, W3, 1, "-3"},
  {151, W4, 1, "-4"},
  {152, X0, 0, ""},
  {153, X1, 1, "-1"},
  {154, X2, 1, "-2"},
  {155, X3, 1, "-3"},
  {156, X4, 1, "-4"},
  {157, DOWN1, 1, "-1"},
  {158, DOWN2, 1, "-2"},
  {159, DOWN3, 1, "-3"},
  {160, DOWN4, 1, "-4"},
  {161, Y0, 0, ""},
  {162, Y1, 1, "-1"},
  {163, Y2, 1, "-2"},
  {164, Y3, 1, "-3"},
  {165, Y4, 1, "-4"},
  {166, Z0, 0, ""},
  {167, Z1, 1, "-1"},
  {168, Z2, 1, "-2"},
  {169, Z3, 1, "-3"},
  {170, Z4, 1, "-4"}
};
/* op_info  op_info_128_170 [] */

op_table  op_128_170  =  {"op_128_170", 128, 170, op_info_128_170};

/* Table for fnt1 to fnt4 (opcodes 235 to 238) inclusive. */

op_info  fnt_n [] =
{
  {235, FONT1, 1, "1"},
  {236, FONT2, 1, "2"},
  {237, FONT3, 1, "3"},
  {238, FONT4, 1, "-4"}
};
/* op_info  fnt_n [] */

op_table  fnt  =  {FONT, 235, 238, fnt_n};


/* Function prototypes */

Void mem_viol ARGS((int sig));
Void give_help (VOID);
int parse ARGS((const char * s));
Void process ARGS((const char * s));

Void no_op (VOID);
Void dtl_stdin (VOID);
Void dvi_stdout (VOID);

int open_dtl ARGS((const char * dtl_file, FILE ** pdtl));
int open_dvi ARGS((const char * dvi_file, FILE ** pdvi));

int dt2dv ARGS((FILE * dtl, FILE * dvi));

Void * gmalloc ARGS((long int size));

Void dinfo (VOID);
Void dexit ARGS((int n));

int cons_cmds ARGS((int nprefixes, CmdPrefix * prefix, CmdTable cmds));
Void free_cmds ARGS((CmdTable cmds));

int get_line ARGS((FILE * fp, Line * line, int max));
int read_line_char ARGS((FILE * fp, int * ch));
int read_char ARGS((FILE * fp, int * ch));
int unread_char (VOID);
int read_string_char ARGS((FILE * fp, int * ch));

COUNT read_variety ARGS((FILE * dtl));
COUNT read_token ARGS((FILE * dtl, char * token));
COUNT skip_space ARGS((FILE * fp, int * ch));
COUNT read_misc ARGS((FILE * fp, Token token));
COUNT read_mes ARGS((FILE * fp, char * token));

int find_command ARGS((char * command, int * opcode));
int xfer_args ARGS((FILE * dtl, FILE * dvi, int opcode));

int set_seq ARGS((FILE * dtl, FILE * dvi));

int check_byte ARGS((int byte));
int put_byte ARGS((int onebyte, FILE * dvi));

U4 xfer_hex ARGS((int n,  FILE * dtl,  FILE * dvi));
U4 xfer_oct ARGS((int n,  FILE * dtl,  FILE * dvi));
U4 xfer_unsigned ARGS((int n,  FILE * dtl,  FILE * dvi));
S4 xfer_signed   ARGS((int n,  FILE * dtl,  FILE * dvi));

int check_bmes ARGS((FILE * dtl));
int check_emes ARGS((FILE * dtl));

Void init_Lstring ARGS((Lstring * lsp, long int n));
Void de_init_Lstring ARGS((Lstring * lsp));
Lstring * alloc_Lstring ARGS((long int n));
Void free_Lstring ARGS((Lstring * lstr));
Void ls_putb ARGS((int ch, Lstring * lstr));

S4 get_Lstring ARGS((FILE * dtl, Lstring * lstr));
Void put_Lstring ARGS((const Lstring * lstr, FILE * dvi));
U4 xfer_len_string ARGS((int n, FILE * dtl, FILE * dvi));

U4 get_unsigned ARGS((FILE * dtl));
S4 get_signed   ARGS((FILE * dtl));

int put_unsigned ARGS((int n, U4 unum, FILE * dvi));
int put_signed   ARGS((int n, S4 snum, FILE * dvi));

S4 xfer_bop_address ARGS((FILE * dtl,  FILE * dvi));
S4 xfer_postamble_address ARGS((FILE * dtl,  FILE * dvi));

int put_table ARGS((op_table table, int opcode, FILE * dtl, FILE * dvi));

U4 special ARGS((FILE * dtl,  FILE * dvi,  int n));
int fontdef ARGS((FILE * dtl,  FILE * dvi,  int n));

U4 preamble ARGS((FILE * dtl,  FILE * dvi));
int postamble ARGS((FILE * dtl,  FILE * dvi));
int post_post ARGS((FILE * dtl,  FILE * dvi));


typedef struct
{
  const char * keyword;  /* command line option keyword */
  int * p_var;           /* pointer to option variable */
  const char * desc;     /* description of keyword and value */
  Void (* p_fn) (VOID);  /* pointer to function called when option is set */
} Options;

Options opts[] =
{
  {"-debug", &debug, "detailed debugging", no_op},
  {"-group", &group, "each DTL command is in parentheses", no_op},
  {"-si", &rd_stdin, "read all DTL commands from standard input", dtl_stdin},
  {"-so", &wr_stdout, "write all DVI commands to standard output", dvi_stdout},
  {NULL, NULL, NULL, NULL}
};
/* opts[] */

const char * progname;  /* intended for name of this program */
int nfile;  /* number of filename arguments on the command line */

#define PRINT_PROGNAME  fprintf (stderr, "%s ", progname)

/* Harbison & Steele (1991) warn that some C implementations */
/* of free() do not treat NULL pointers correctly. */
#define gfree(p) {if (p) free (p);}


FILE * dtl_fp;
FILE * dvi_fp;

const char * dtl_filename;
const char * dvi_filename;


int
main
#ifdef STDC
  (int argc,  char * argv[])
#else
  (argc,  argv)
  int argc;
  char * argv[];
#endif
{
  int i;

#ifdef KPATHSEA
  kpse_set_program_name(argv[0], "dt2dv");
  progname = kpse_program_name;
#else
  progname = argv[0];  /* name of this program */
#endif

  /* memory violation signal handler */
  /* Not supported under Win32 */
#ifndef WIN32
  signal (SIGSEGV, mem_viol);
#endif

#ifndef __DATE__
#define __DATE__ ""
#endif

#ifndef __TIME__
#define __TIME__ ""
#endif

#if STDC
#define C_LEVEL ""
#else /* NOT STDC */
#define C_LEVEL "non-"
#endif /* NOT STDC */

  /* message about program and compiler */
  /* NB:  LTU EE's Sun/OS library is BSD, even though gcc 2.2.2 is ANSI */

  fprintf (stderr, "\n");
  fprintf (stderr,
    "Program \"%s\" version %s compiled in %sstandard C.\n",
    progname, VERSION, C_LEVEL);

  /* interpret command line arguments */

  nfile = 0;
  dtl_fp = dvi_fp = NULL;
  dtl_filename = dvi_filename = "";

  for (i=1; i < argc; i++)
  {
    /* parse options, followed by any explicit filenames */
    parse (argv[i]);
  }

  if (nfile != 2)  /* not exactly two files specified, so give help */
    give_help();
  else
    /* the real works */
    dt2dv (dtl_fp, dvi_fp);

  return 0;  /* OK */
}
/* end main */

#ifndef WIN32
Void
mem_viol
#ifdef STDC
  (int sig)
#else
  (sig)
  int sig;
#endif
{
  signal (SIGSEGV, mem_viol);
  if (sig != SIGSEGV)
  {
    PRINT_PROGNAME;
    fprintf (stderr,
      "(mem_viol) : called with wrong signal!\n");
  }
  PRINT_PROGNAME;
  fprintf (stderr, "(mem_viol) : RUNTIME MEMORY ERROR : memory violation, ");
  fprintf (stderr, "dtl line >= ");
  fprintf (stderr, WF, dtl_line.num);
  fprintf (stderr, "\n");
  dexit (1);
}
/* mem_viol */
#endif

Void
give_help (VOID)
{
  int i;
  const char * keyword;
  fprintf (stderr, "usage:   ");
  PRINT_PROGNAME;
  fprintf (stderr, "[options]  dtl_file  dvi_file");
  fprintf (stderr, "\n");
  for (i=0; (keyword = opts[i].keyword) != NULL; i++)
  {
    fprintf (stderr, "    ");
    fprintf (stderr, "[%s]", keyword);
    fprintf (stderr, "    ");
    fprintf (stderr, "%s", opts[i].desc);
    fprintf (stderr, "\n");
  }
  fprintf (stderr, "Messages, like this one, go to stderr.\n");
}
/* give_help */


Void no_op (VOID)
/* do nothing */
{
}

Void dtl_stdin (VOID)
{
  dtl_fp = stdin;
  dtl_filename = "Standard Input";
  ++ nfile;
}

Void dvi_stdout (VOID)
{
  /* ! Perilous to monitors! */
  dvi_fp = stdout;
  dvi_filename = "Standard Output";
  ++ nfile;
  if (!isatty(fileno(dvi_fp)))
    SET_BINARY(fileno(dvi_fp));
}


int
parse
#ifdef STDC
  (const char * s)
#else
  (s)
  const char * s;
#endif
/* parse one command-line argument, `s' */
{
  int i;
  const char * keyword;
  for (i=0; (keyword = opts[i].keyword) != NULL; i++)
  {
    if (strncmp (s, keyword, strlen (keyword)) == 0)
    {
      Void (*pfn) (VOID);
      (*(opts[i].p_var)) = 1;  /* turn option on */
      if ((pfn = opts[i].p_fn) != NULL)
        (*pfn) ();    /* call option function */
      return i;
    }
  }
  /* reached here, so not an option: assume it's a filename */
  process (s);
  return i;
}
/* parse */


int
open_dtl
#ifdef STDC
  (const char * dtl_file, FILE ** pdtl)
#else
  (dtl_file, pdtl)
  const char * dtl_file;
  FILE ** pdtl;
#endif
/* I:  dtl_file;  I:  pdtl;  O:  *pdtl. */
{
  dtl_filename = dtl_file;

  if (dtl_filename == NULL)
  {
    PRINT_PROGNAME;
    fprintf (stderr,
      "(open_dtl) : INTERNAL ERROR : dtl file's name is NULL.\n");
    dexit (1);
  }

  if (pdtl == NULL)
  {
    PRINT_PROGNAME;
    fprintf (stderr,
      "(open_dtl) : INTERNAL ERROR : address of dtl variable is NULL.\n");
    dexit (1);
  }

  *pdtl = fopen (dtl_file, "r");

  if (*pdtl == NULL)
  {
    PRINT_PROGNAME;
    fprintf (stderr,
      "(open_dtl) : DTL FILE ERROR : Cannot open \"%s\" for text reading.\n",
      dtl_file);
    dexit (1);
  }

  return 1;  /* OK */
}
/* open_dtl */


int
open_dvi
#ifdef STDC
  (const char * dvi_file, FILE ** pdvi)
#else
  (dvi_file, pdvi)
  const char * dvi_file;
  FILE ** pdvi;
#endif
/* I:  dvi_file;  I:  pdvi;  O:  *pdvi. */
{
  dvi_filename = dvi_file;

  if (dvi_filename == NULL)
  {
    PRINT_PROGNAME;
    fprintf (stderr,
    "(open_dvi) : INTERNAL ERROR : dvi file's name is NULL.\n");
    dexit (1);
  }

  if (pdvi == NULL)
  {
    PRINT_PROGNAME;
    fprintf (stderr,
    "(open_dvi) : INTERNAL ERROR : address of dvi variable is NULL.\n");
    dexit (1);
  }

  *pdvi = fopen (dvi_file, "wb");

  if (*pdvi == NULL)
  {
    PRINT_PROGNAME;
    fprintf (stderr,
      "(open_dvi) : DVI FILE ERROR : Cannot open \"%s\" for binary writing.\n",
      dvi_file);
    dexit (1);
  }

  return 1;  /* OK */
}
/* open_dvi */


Void
process
#ifdef STDC
  (const char * s)
#else
  (s)
  const char * s;
#endif
{
  if (dtl_fp == NULL)  /* first filename assumed to be DTL input */
  {
    open_dtl (s, &dtl_fp);
  }
  else if (dvi_fp == NULL)  /* second filename assumed to be DVI output */
  {
    open_dvi (s, &dvi_fp);
  }
  else
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(process) : at most two filenames allowed.\n");
    exit (1);
  }
  ++ nfile;
}
/* process */


COUNT dtl_read = 0;  /* bytes read from dtl file */
COUNT dvi_written = 0;  /* bytes written to dvi file */
word_t last_bop_address = -1;  /* byte address of last bop; first bop uses -1 */
word_t postamble_address = -1;  /* byte address of postamble */
COUNT ncom = 0;  /* commands successfully read and interpreted from dtl file */
COUNT com_read = 0;  /* bytes read in current (command and arguments), */
                      /* since and including the opening BCOM_CHAR, if any */


int
put_byte
#ifdef STDC
  (int byte, FILE * dvi)
#else
  (byte, dvi)
  int byte;
  FILE * dvi;
#endif
/* write byte into dvi file */
{
  check_byte (byte);
/*  if (fprintf (dvi, "%c", byte) != 1) */
  if (fprintf (dvi, "%c", byte) < 0)
  {
    PRINT_PROGNAME;
    fprintf (stderr,
      "(put_byte) : DVI FILE ERROR (%s) : cannot write to dvi file.\n",
      dtl_filename);
    dexit (1);
  }
  ++ dvi_written;
  return 1;  /* OK */
}
/* put_byte */


int
dt2dv
#ifdef STDC
  (FILE * dtl, FILE * dvi)
#else
  (dtl, dvi)
  FILE * dtl;
  FILE * dvi;
#endif
{
  int nprefixes = 0;  /* number of prefixes in cmd_prefixes[] list. */
  static Token dtl_cmd = "";  /* DTL command name */

  nprefixes = sizeof (cmd_prefixes) / sizeof (CmdPrefix);

  /* Construct array of all NCMDS == 256 DTL commands */

  (Void) cons_cmds (nprefixes, cmd_prefixes, cmd_table);

  /* DTL commands have the form "[ ]*command arg ... arg[ ]*", */
  /* possibly enclosed in a BCOM, ECOM pair, */
  /* and are separated by optional whitespace, typically newlines. */
  /* That is, each command and its arguments are parenthesised, */
  /* with optional spaces after the BCOM and before the ECOM, if any. */

  /* dt2dv is now at the very start of the DTL file */

  dtl_line.num = 0;
  dtl_read = 0;

  /* The very first thing should be the "variety" signature */

  read_variety (dtl);

  /* while not end of dtl file or reading error, */
  /*   read, interpret, and write commands */

  while (!feof (dtl))
  {
    int opcode;

    com_read = 0;

    if (group)
    {
      /* BCOM check */
      static Token token = "";  /* DTL token */
      read_token (dtl, token);
      /* test for end of input, or reading error */
      if (strlen (token) == 0)
      {
	if (debug)
	{
          PRINT_PROGNAME;
	  fprintf (stderr, "(dt2dv) : end of input, or reading error.\n");
	}
	break;
      }
      /* test whether this command begins correctly */
      else if (strcmp (token, BCOM) != 0)
      {
        PRINT_PROGNAME;
	fprintf (stderr, "(dt2dv) : DTL FILE ERROR (%s) : ", dtl_filename);
        fprintf (stderr, "command must begin with \"%s\", ", BCOM);
        fprintf (stderr, "not `%c' (char %d).\n", token[0], token[0]);
	dexit (1);
      }
      /* end BCOM check */
    }

    /* read the command name */
    read_token (dtl, dtl_cmd);
    /* test for end of input, or reading error */
    if (strlen (dtl_cmd) == 0)
    {
      if (debug)
      {
        PRINT_PROGNAME;
	fprintf (stderr,
	  "(dt2dv) : end of input, or reading error.\n");
      }
      break;
    }
    else
    {
      if (debug)
      {
        PRINT_PROGNAME;
	fprintf (stderr, "(dt2dv) : command ");
        fprintf (stderr, WF, ncom);
        fprintf (stderr, " = \"%s\".\n", dtl_cmd);
      }

      /* find opcode for this command */
      if (find_command (dtl_cmd, &opcode) == 1)
      {
	/* write the opcode, if we can */
	put_byte (opcode, dvi);

	/* treat the arguments, if any */
	xfer_args (dtl, dvi, opcode);
      }
      else if (dtl_cmd[0] == BSEQ_CHAR)
      {
	/* sequence of font characters for SETCHAR */
	set_seq (dtl, dvi);
      }
      else
      {
        PRINT_PROGNAME;
	fprintf (stderr,
	  "(dt2dv) : DTL FILE ERROR (%s) : unknown command \"%s\".\n",
	  dtl_filename, dtl_cmd);
	dexit (1);
      }
    }

    if (group)
    {
      /* seek ECOM after command's last argument and optional whitespace */
      static Token token = "";  /* DTL token */
      read_token (dtl, token);
      /* test for end of input, or reading error */
      if (strlen (token) == 0)
      {
	if (debug)
	{
          PRINT_PROGNAME;
	  fprintf (stderr,
	    "(dt2dv) : end of input, or reading error.\n");
	}
	break;
      }
      if (strcmp (token, ECOM) != 0)
      {
        PRINT_PROGNAME;
	fprintf (stderr, "(dt2dv) : DTL FILE ERROR (%s) : ", dtl_filename);
        fprintf (stderr, "ECOM (\"%s\") expected, not `%c' (char %d).\n",
	  ECOM, token[0], token[0]);
	dexit (1);
      }
      /* end ECOM check */
    }

    ++ ncom;  /* one more command successfully read and interpreted */
  }
  /* end while */

  PRINT_PROGNAME;
  fprintf (stderr, "(dt2dv) :\n");
  fprintf (stderr, "Read (from file \"%s\") ", dtl_filename);
  fprintf (stderr, WF, dtl_read);
  fprintf (stderr, " DTL bytes (");
  fprintf (stderr, UF4, dtl_line.num);
  fprintf (stderr, " lines);\n");
  fprintf (stderr, "wrote (to file \"%s\") ", dvi_filename);
  fprintf (stderr, WF, dvi_written);
  fprintf (stderr, " DVI bytes;\n");
  fprintf (stderr, "completely interpreted ");
  fprintf (stderr, WF, ncom);
  fprintf (stderr, " DVI command%s.\n", (ncom == 1 ? "" : "s"));
  fprintf (stderr, "\n");

  (Void) free_cmds (cmd_table);

  return 1;  /* OK */
}
/* dt2dv */


Void *
gmalloc
#ifdef STDC
  (long int size)
#else
  (size)
  long int size;
#endif
{
  Void * p = NULL;
  if (size < 1)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(gmalloc) : INTERNAL ERROR : ");
    fprintf (stderr,
      "unreasonable request to malloc %ld bytes\n",
      size);
    dexit (1);
  }
  p = (Void *) malloc ((size_t) size);
  if (p == NULL)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(gmalloc) : MEMORY ALLOCATION ERROR : ");
    fprintf (stderr,
      "operating system failed to malloc %ld bytes\n",
      size);
    dexit (1);
  }
  return (p);
}
/* gmalloc */


Void
dinfo (VOID)
{
  PRINT_PROGNAME;
  fprintf (stderr, "(dinfo) : ");
  fprintf (stderr, "Current DTL input line ");
  fprintf (stderr, UF4, dtl_line.num);
  fprintf (stderr, " :\n");
  fprintf (stderr, "\"%s\"\n", dtl_line.buf);
  fprintf (stderr, "Read ");
  fprintf (stderr, WF, dtl_read);
  fprintf (stderr, " DTL bytes (");
  fprintf (stderr, WF, com_read);
  fprintf (stderr, " in current command), wrote ");
  fprintf (stderr, WF, dvi_written);
  fprintf (stderr, " DVI bytes.\n");
  fprintf (stderr, "Successfully interpreted ");
  fprintf (stderr, WF, ncom);
  fprintf (stderr, " DVI command%s.\n", (ncom == 1 ? "" : "s"));
}
/* dinfo */


Void
dexit
#ifdef STDC
  (int n)
#else
  (n)
  int n;
#endif
{
  dinfo();
  PRINT_PROGNAME;
  fprintf (stderr, "(dexit) : exiting with status %d.\n", n);
  exit (n);
}
/* dexit */


int
cons_cmds
#ifdef STDC
  (int nprefixes, CmdPrefix * prefix, CmdTable cmds)
#else
  (nprefixes, prefix, cmds)
  int nprefixes;
  CmdPrefix * prefix;
  CmdTable cmds;
#endif
{
  int code;  /* first opcode for a given command prefix */
  int opcode;  /* command's opcode */
  int nsuffixes;  /* number of commands with a given prefix */
  int isuffix;  /**** integer suffix, of at most three digits ****/
  String suffix;  /* suffix string generated from integer suffix */
  size_t plen = 0;  /* prefix length */
  size_t slen;  /* suffix length */
  size_t clen;  /* whole command name length */
  int i, j;  /* loop indices */

  for (i=0; i < nprefixes; prefix++, i++)
  {
    code = prefix->first_code;
    if (code < 0 || code > 255)
    {
      PRINT_PROGNAME;
      fprintf (stderr, "(cons_cmds) : INTERNAL ERROR : ");
      fprintf (stderr,
        "prefix listed internally with code = %d, must be 0 to 255\n",
        code);
      dexit (1);
    }
    if (prefix->has_suffix)
    {
      plen = strlen (prefix->name);
      /**** Suffixes in DTL are Integers, in Sequence */
      if (prefix->last_suffix < prefix->first_suffix)
      {
        PRINT_PROGNAME;
        fprintf (stderr, "(cons_cmds) : INTERNAL ERROR : ");
        fprintf (stderr,
          "prefix's last suffix %d < first suffix (%d)\n",
          prefix->last_suffix, prefix->first_suffix);
        dexit (1);
      }
      nsuffixes = prefix->last_suffix - prefix->first_suffix + 1;
      opcode = prefix->first_code;
      for (j=0; j < nsuffixes; j++, opcode++)
      {
        isuffix = prefix->first_suffix + j;
        if (0 <= code && code <= 127)  /* SETCHAR */
        {
          /* SETCHAR's suffix is written in uppercase hexadecimal */
          sprintf (suffix, "%02X", isuffix);
        }
        else  /* 128 <= code && code <= 255 */  /* other DTL commands */
        {
          /* other commands' suffices are written in decimal */
          sprintf (suffix, "%d", isuffix);
        }
        slen = strlen (suffix);
        clen = plen + slen;
        cmds[opcode] = (char *) gmalloc (clen+1);
        strcpy (cmds[opcode], prefix->name);
        strcat (cmds[opcode], suffix);
      }
    }
    else /* command name = prefix */
    {
      plen = strlen (prefix->name);
      clen = plen;
      opcode = prefix->first_code;
      cmds[opcode] = (char *) gmalloc (clen+1);
      strcpy (cmds[opcode], prefix->name);
    }
  }

  return 1;  /* OK */
}
/* cons_cmds */


Void
free_cmds
#ifdef STDC
  (CmdTable cmds)
#else
  (cmds)
  CmdTable cmds;
#endif
{
  int i;
  for (i=0; i < NCMDS; i++)
    gfree (cmds[i]);
}
/* free_cmds */


int
get_line
#ifdef STDC
  (FILE * fp, Line * line, int max)
#else
  (fp, line, max)
  FILE * fp;
  Line * line;
  int max;
#endif
/* read a (Line *) line from fp, return length */
/* adapted from K&R (second, alias ANSI C, edition, 1988), page 165 */
{
  if (fgets (line->buf, max, fp) == NULL)
    return 0;
  else
  {
    ++ line->num;
    line->wrote = strlen (line->buf);
    line->read = 0;
    return 1;
  }
}
/* get_line */


int
read_line_char
#ifdef STDC
  (FILE * fp, int * ch)
#else
  (fp, ch)
  FILE * fp;
  int * ch;
#endif
/* read one character from dtl_line if possible, */
/* otherwise read another dtl_line from fp */
/* return 1 if a character is read, 0 if at end of fp file */
{
  if (dtl_line.wrote == 0 || dtl_line.read >= dtl_line.wrote)
  {
    int line_status;
    /* refill line buffer */
    line_status = get_line (fp, &dtl_line, MAXLINE);
    if (line_status == 0)
    {
      /* at end of DTL file */
      if (debug)
      {
        PRINT_PROGNAME;
        fprintf (stderr, "(read_line_char) : end of DTL file\n");
        dinfo();
      }
      return 0;
    }
    else
    {
      /* new DTL line was read */
      if (debug)
      {
        PRINT_PROGNAME;
        fprintf (stderr, "(read_line_char) : new DTL input line:\n");
        fprintf (stderr, "\"%s\"\n", dtl_line.buf);
      }
    }
  }
  *ch = dtl_line.buf [dtl_line.read ++];
  ++ dtl_read;
  ++ com_read;  /* count another DTL command character */
  return 1;
}
/* read_line_char */


int
read_char
#ifdef STDC
  (FILE * fp, int * ch)
#else
  (fp, ch)
  FILE * fp;
  int * ch;
#endif
/* Read next character, if any, from file fp. */
/* Write it into *ch. */
/* If no character is read, then *ch value < 0. */
/* Return 1 if OK, 0 if EOF or error. */
{
  int status = 1;
  int c;  /* in case ch points awry, we still have something in c. */

  c = EOF;
  if (read_line_char (fp, &c) == 0)
  {
    /* end of fp file, or error reading it */
    status = 0;
  }
  else
  {
    if (c > 255)
    {
      PRINT_PROGNAME;
      fprintf (stderr,
        "(read_char) : character %d not in range 0 to 255\n",
        c);
      dinfo();
      status = 0;
    }
    else if ( ! isprint (c) && ! isspace (c)
#ifdef WIN32
            && ! isknj (c) && ! isknj2 (c) 
#endif
            )
    {
      PRINT_PROGNAME;
      fprintf (stderr,
        "(read_char) : character %d not printable and not white space.\n",
        c);
      dinfo();
      status = 0;
    }
  }
  *ch = c;

  return status;
}
/* read_char */


COUNT
read_variety
#ifdef STDC
  (FILE * dtl)
#else
  (dtl)
  FILE * dtl;
#endif
/* read and check DTL variety signature */
/* return number of DTL bytes written */
/* DTL variety is _NEVER_ grouped by BCOM and ECOM. */
/* Uniformity here enables the program easily to modify its behavior. */
{
  COUNT vread = 0;  /* number of DTL bytes read by read_variety */
  static Token token = "";

  /* read the DTL VARIETY keyword */
  vread += read_token (dtl, token);
  /* test whether signature begins correctly */
  if (strcmp (token, "variety") != 0)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(read_variety) : DTL FILE ERROR (%s) : ", dtl_filename);
    fprintf (stderr, "DTL signature must begin with \"variety\", not \"%s\".\n",
      token);
    dexit (1);
  }

  /* read the DTL variety */
  vread += read_token (dtl, token);
  /* test whether variety is correct */
  if (strcmp (token, VARIETY) != 0)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(read_variety) : DTL FILE ERROR (%s) : ", dtl_filename);
    fprintf (stderr, "DTL variety must be \"%s\", not \"%s\".\n",
      VARIETY, token);
    dexit (1);
  }

  PRINT_PROGNAME;
  fprintf (stderr, "(read_variety) : DTL variety %s is OK.\n", VARIETY);

  return vread;  /* OK */
}
/* read_variety */


COUNT
skip_space
#ifdef STDC
  (FILE * fp, int * ch)
#else
  (fp, ch)
  FILE * fp;
  int * ch;
#endif
/* Skip whitespace characters in file fp. */
/* Write in *ch the last character read from fp, */
/*   or < 0 if fp could not be read. */
/* Return number of characters read from fp. */
{
  int c;  /* character read (if any) */
  COUNT count;  /* number (0 or more) of whitespace characters read */
  int nchar;  /* number (0 or 1) of characters read by read_char */

  /* loop ends at:  end of fp file, or reading error, or not a white space */
  for (count=0;  ((nchar = read_char (fp, &c)) == 1 && isspace (c));  ++count)
  {
    /* otherwise, more white spaces to skip */
    if (debug)
    {
      /* report when each DTL end of line is reached */
      if (c == '\n')
      {
        PRINT_PROGNAME;
        fprintf (stderr, "(skip_space) : ");
        fprintf (stderr, "end of DTL line (at least) ");
        fprintf (stderr, WF, dtl_line.num);
        fprintf (stderr, "\n");
      }
    }
  }

  if (nchar == 0)
  {
    c = -1;
  }

  *ch = c;  /* c will be < 0 if read_char could not read fp */
  return (count + nchar);
}
/* skip_space */


COUNT
read_token
#ifdef STDC
  (FILE * dtl, char * token)
#else
  (dtl, token)
  FILE * dtl;
  char * token;
#endif
/* read next token from dtl file. */
/* return number of DTL bytes read. */
/* A token is one of:
     a string from BMES_CHAR to the next unescaped EMES_CHAR, inclusive;
     BCOM or ECOM, unless these are empty strings;
     BSEQ or ESEQ;
     any other sequence of non-whitespace characters.
*/
{
  COUNT nread;  /* number of DTL bytes read by read_token */
  int ch;  /* most recent character read */

  nread = 0;

  /* obtain first non-space character */
  /* add to nread the number of characters read from dtl by skip_space */
  nread += skip_space (dtl, &ch);

  if (ch < 0)
  {
    /* end of dtl file */
    /* write an empty token */
    strcpy (token, "");
    if (debug)
    {
      PRINT_PROGNAME;
      fprintf (stderr, "(read_token) : end of dtl file.\n");
    }
  }
  else if (group && ch == BCOM_CHAR)
  {
    strcpy (token, BCOM);
  }
  else if (group && ch == ECOM_CHAR)
  {
    strcpy (token, ECOM);
  }
  else
  {
    token[0] = ch;
    token[1] = '\0';
    if (ch == BMES_CHAR)  /* string token; read until unescaped EMES_CHAR */
    {
      nread += read_mes (dtl, token+1);
    }
    else if (ch == BSEQ_CHAR || ch == ESEQ_CHAR)
    {
      /* token is complete */
    }
    else  /* any other string not containing (ECOM_CHAR or) whitespace */
    {
      nread += read_misc (dtl, token+1);
    }
  }

  if (debug)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(read_token) : token = \"%s\"\n", token);
  }

  return (nread);  /* number of bytes read from dtl file */
}
/* read_token */


#define CHAR_OK  1
#define CHAR_FAIL  0
#define CHAR_EOS  (-1)

int
read_string_char
#ifdef STDC
  (FILE * fp, int * ch)
#else
  (fp, ch)
  FILE * fp;
  int * ch;
#endif
{
  int status = CHAR_OK;  /* OK so far */
  int c;

  if (read_char (fp, &c) == 0)
    status = CHAR_FAIL;  /* fail */

  if (c == EMES_CHAR)  /* end-of-string char */
  {
    status = CHAR_EOS;  /* end of string */
  }
  else if (c == ESC_CHAR)  /* escape character */
  {
    /* accept the next character literally, even ESC_CHAR and EMES_CHAR */
    if (read_char (fp, &c) == 0)
      status = CHAR_FAIL;  /* fail */
  }

  *ch = c;
  return status;
}
/* read_string_char */


COUNT
read_misc
#ifdef STDC
  (FILE * fp, Token token)
#else
  (fp, token)
  FILE * fp;
  Token token;
#endif
{
  int c;
  int count;
 /* loop ends at:  end of fp file, or reading error, or a space */
  for (count=0;  count <= MAXTOKLEN;  ++count)
  {
    if (read_char (fp, &c) == 0  ||  isspace (c))
      break;
    if (group && c == ECOM_CHAR)
    {
      (Void) unread_char ();
      break;
    }

    token[count] = c;
  }
  token[count] = '\0';
  return count;
}
/* read_misc */


COUNT
read_mes
#ifdef STDC
  (FILE * fp, char * token)
#else
  (fp, token)
  FILE * fp;
  char * token;
#endif
/* called **AFTER** a BMES_CHAR has been read */
/* read file fp for characters up to next unescaped EMES_CHAR */
/* this is called a "string token" */
/* write string, including EMES_CHAR, into token[] */
/* return number of characters read from fp */
{
  COUNT dtl_count;  /* number of DTL characters read by read_mes from fp */
  int more;  /* flag more == 0 to terminate loop */
  int escape;  /* flag escape == 1 if previous character was ESC_CHAR */
  int ch;  /* current DTL character */

  escape = 0;
  more = 1;
  dtl_count = 0;
  while (more)
  {
    if (read_char (fp, &ch) == 0)
    {
      /* end of fp file, or reading error */
      more = 0;
    }
    else  /* error checking passed */
    {
      ++ dtl_count;
      if (ch == EMES_CHAR && escape == 0)  /* end of string */
      {
        /* include final EMES_CHAR */
        * token ++ = ch;
        more = 0;
      }
      else if (ch == ESC_CHAR && escape == 0)
      {
        /* next character is not end of string */
        escape = 1;
      }
      else
      {
        /* store any other character, */
        /* including escaped EMES_CHAR and ESC_CHAR*/
        * token ++ = ch;
        escape = 0;
      }
    }
  }
  * token = '\0';
  return dtl_count;
}
/* read_mes */


int
unread_char (VOID)
/* wind input back, to allow rereading of one character */
/* return 1 if this works, 0 on error */
{
  int status;
  if (dtl_line.read > 0)
  {
    -- dtl_line.read;  /* back up one character in dtl_line */
    -- dtl_read;  /* correct the count of DTL characters */
    -- com_read;  /* count another DTL command character */
    status = 1;  /* OK */
  }
  else /* current DTL line is empty */
  {
    status = 0;  /* error */
  }
  return status;
}
/* unread_char */


int
find_command
#ifdef STDC
  (char * command, int * opcode)
#else
  (command, opcode)
  char * command;
  int * opcode;
#endif
{
  int found;
  int i;

  found = 0;
  for (i=0; i < NCMDS; i++)
  {
    if ((cmd_table[i] != 0) && (strcmp (command, cmd_table[i]) == 0))
    {
      found = 1;
      break;
    }
  }

  *opcode = i;

  return found;
}
/* find_command */


int
check_byte
#ifdef STDC
  (int byte)
#else
  (byte)
  int byte;
#endif
{
  if (byte < 0 || byte > 255)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(check_byte) : INTERNAL ERROR : ");
    fprintf (stderr, "byte %d not in the range of 0 to 255.\n", byte);
    dexit (1);
  }
  return 1;  /* OK */
}
/* check_byte */


int
xfer_args
#ifdef STDC
  (FILE * dtl, FILE * dvi, int opcode)
#else
  (dtl, dvi, opcode)
  FILE * dtl;
  FILE * dvi;
  int opcode;
#endif
{
  int n;

  if (opcode >= 0 && opcode <= 127)
    ;  /* SETCHAR uses no data */
  else if (opcode >= 128 && opcode <= 170)
  {
    word_t this_bop_address = last_bop_address;

    if (opcode == 139)  /* BOP */
    {
      this_bop_address = dvi_written - 1;
    }
    put_table (op_128_170, opcode, dtl, dvi);
    if (opcode == 139)  /* BOP */
    {
      xfer_bop_address (dtl, dvi);
      last_bop_address = this_bop_address;
    }
  }
  else if (opcode >= 171 && opcode <= 234)
    ;  /* FONTNUM uses no data */
  else if (opcode >= 235 && opcode <= 238)
    put_table (fnt, opcode, dtl, dvi);
  else if (opcode >= 239 && opcode <= 242)
  {
    n = opcode - 238;
    special (dtl, dvi, n);
  }
  else if (opcode >= 243 && opcode <= 246)
  {
    n = opcode - 242;
    fontdef (dtl, dvi, n);
  }
  else if (opcode == 247)
    preamble (dtl, dvi);
  else if (opcode == 248)
    postamble (dtl, dvi);
  else if (opcode == 249)
    post_post (dtl, dvi);
  else if (opcode >= 250 && opcode <= 255)
    ;  /* these, undefined, opcodes use no data */
  else
  {
    PRINT_PROGNAME;
    fprintf (stderr,
      "(xfer_args) : opcode %d not handled.\n",
      opcode);
  }

  return 1;  /* OK */
}
/* xfer_args */


int
set_seq
#ifdef STDC
  (FILE * dtl, FILE * dvi)
#else
  (dtl, dvi)
  FILE * dtl;
  FILE * dvi;
#endif
/* Called _after_ a BSEQ_CHAR command */
/* Read bytes from dtl file, */
/* writing corresponding SETCHAR or SET1 commands to DVI file, */
/* _until_ unescaped ESEQ_CHAR is found */
/* Return 1 if OK, 0 on error */
/****  dt2dv assumes 8 bit characters,      ****/
/****  but some day one might change that.  ****/
{
  int status = 1;  /* status = 1 if OK, 0 if error */
  int more;  /* sequence of font characters continuing? */
  int escape = 0;  /* flag set if previous character was an escape */
  int ch;  /* character read from DTL file */
  more = 1;
  while (more)
  {
    /* ignore read_char status, to allow unprintable characters */
    (Void) read_char (dtl, &ch);
    /* but check for end of dtl file, or serious file reading error */
    if (ch < 0)
    {
      PRINT_PROGNAME;
      fprintf (stderr, "(set_seq) : ");
      fprintf (stderr, "end of dtl file, ");
      fprintf (stderr, "or serious dtl file reading error\n");
      dinfo();
      more = 0;
      status = 0;  /* bad news */
    }
    else  /* read dtl file, okay */
    {
      if (ch == ESC_CHAR && escape == 0)  /* escape next character */
      {
        escape = 1;
      }
      else
      {
        if (ch == ESEQ_CHAR && escape == 0)  /* end of sequence */
        {
          more = 0;
        }
        else if (ch <= 127)  /* can use SETCHAR */
        {
          put_byte (ch, dvi);
        }
        else if (ch <= 255)  /* can use SET1 */
        {
          put_byte (128, dvi);  /* SET1 opcode */
          put_unsigned (1, (U4) ch, dvi);
        }
        else
        {
          PRINT_PROGNAME;
          fprintf (stderr, "(set_seq) : ");
          fprintf (stderr,
            "ERROR : DTL character %d is not in range 0 to 255\n",
            ch);
          dexit (1);
          more = 0;
          status = 0;  /* Error, because dt2dv assumes 8 bit characters */
        }
        escape = 0;  /* current ch is not an escape character */
      }
    }
  }
  return status;
}
/* set_seq */


U4
xfer_hex
#ifdef STDC
  (int n, FILE * dtl, FILE * dvi)
#else
  (n, dtl, dvi)
  int n;
  FILE * dtl;
  FILE * dvi;
#endif
/* translate unsigned n-byte hexadecimal number from dtl to dvi file. */
/* return value of hexadecimal number */
{
  U4 unum = 0;  /* at most this space needed */
  int nconv = 0;  /* number of arguments converted by sscanf */
  static Token token = "";  /* DTL token */

  if (n < 1 || n > 4)
  {
    PRINT_PROGNAME;
    fprintf (stderr,
      "(xfer_hex) : INTERNAL ERROR : asked for %d bytes.  Must be 1 to 4.\n",
      n);
    dexit (1);
  }

  read_token (dtl, token);

  nconv = sscanf (token, XF4, &unum);

  if (nconv < 1)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(xfer_hex) : DTL FILE ERROR (%s) :  %s \"%s\".\n",
      dtl_filename, "hexadecimal number expected, not", token);
    dexit (1);
  }

  put_unsigned (n, unum, dvi);

  return unum;
}
/* xfer_hex */


U4
xfer_oct
#ifdef STDC
  (int n, FILE * dtl, FILE * dvi)
#else
  (n, dtl, dvi)
  int n;
  FILE * dtl;
  FILE * dvi;
#endif
/* translate unsigned n-byte octal number from dtl to dvi file. */
/* return value of octal number */
{
  U4 unum = 0;  /* at most this space needed */
  int nconv = 0;  /* number of arguments converted by sscanf */
  static Token token = "";  /* DTL token */

  if (n < 1 || n > 4)
  {
    PRINT_PROGNAME;
    fprintf (stderr,
      "(xfer_oct) : INTERNAL ERROR : asked for %d bytes.  Must be 1 to 4.\n",
      n);
    dexit (1);
  }

  read_token (dtl, token);

  nconv = sscanf (token, OF4, &unum);

  if (nconv < 1)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(xfer_oct) : DTL FILE ERROR (%s) :  %s \"%s\".\n",
      dtl_filename, "octal number expected, not", token);
    dexit (1);
  }

  put_unsigned (n, unum, dvi);

  return unum;
}
/* xfer_oct */


U4
xfer_unsigned
#ifdef STDC
  (int n, FILE * dtl, FILE * dvi)
#else
  (n, dtl, dvi)
  int n;
  FILE * dtl;
  FILE * dvi;
#endif
/* translate unsigned n-byte number from dtl to dvi file. */
/* return value of unsigned number */
{
  U4 unum = 0;  /* at most this space needed */

  unum = get_unsigned (dtl);
  put_unsigned (n, unum, dvi);

  return unum;
}
/* xfer_unsigned */


S4
xfer_signed
#ifdef STDC
  (int n, FILE * dtl, FILE * dvi)
#else
  (n, dtl, dvi)
  int n;
  FILE * dtl;
  FILE * dvi;
#endif
/* translate signed n-byte number from dtl to dvi file. */
/* return value of signed number */
{
  S4 snum = 0;

  snum = get_signed (dtl);
  put_signed (n, snum, dvi);

  return snum;
}
/* xfer_signed */


U4
get_unsigned
#ifdef STDC
  (FILE * dtl)
#else
  (dtl)
  FILE * dtl;
#endif
/* read unsigned number from dtl file. */
/* return value of unsigned number */
{
  U4 unum = 0;  /* at most this space needed */
  int nconv = 0;  /* number of arguments converted by sscanf */
  static Token token = "";  /* DTL token */

  read_token (dtl, token);

  nconv = sscanf (token, UF4, &unum);

  if (nconv < 1)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(get_unsigned) : DTL FILE ERROR (%s) :  %s \"%s\".\n",
      dtl_filename, "unsigned number expected, not", token);
    dexit (1);
  }

  return unum;
}
/* get_unsigned */


S4
get_signed
#ifdef STDC
  (FILE * dtl)
#else
  (dtl)
  FILE * dtl;
#endif
/* read signed number from dtl file. */
/* return value of signed number */
{
  S4 snum = 0;
  int nconv = 0;  /* number of sscanf arguments converted and assigned */
  static Token token = "";

  read_token (dtl, token);

  nconv = sscanf (token, SF4, &snum);

  if (nconv < 1)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(get_signed) : DTL FILE ERROR (%s) :  %s \"%s\".\n",
      dtl_filename, "signed number expected, not", token);
    dexit (1);
  }

  return snum;
}
/* get_signed */


int
put_unsigned
#ifdef STDC
  (int n, U4 unum, FILE * dvi)
#else
  (n, unum, dvi)
  int n;
  U4 unum;
  FILE * dvi;
#endif
/* put unsigned in-byte integer to dvi file */
/* DVI format uses Big-endian storage of numbers: */
/* most significant byte is first. */
/* return number of bytes written. */
{
  Byte ubyte[4];  /* at most 4 bytes needed in DVI format */
  int i;

  if (n < 1 || n > 4)
  {
    PRINT_PROGNAME;
    fprintf (stderr,
      "(put_unsigned) : INTERNAL ERROR : asked for %d bytes.  Must be 1 to 4.\n",
      n);
    dexit (1);
  }

  /* Big-endian storage. */
  for (i = 0; i < n; i++)
  {
    ubyte[i] = (Byte) (unum % 256);
    unum /= 256;
  }
  /* Reverse order for big-endian representation. */
  for (i = n-1;  i >= 0;  i--)
  {
    put_byte ((int) ubyte[i], dvi);
  }

  return n;
}
/* put_unsigned */


int
put_signed
#ifdef STDC
  (int n, S4 snum, FILE * dvi)
#else
  (n, snum, dvi)
  int n;
  S4 snum;
  FILE * dvi;
#endif
/* put signed in-byte integer to dvi file */
/* DVI format uses 2's complement Big-endian storage of signed numbers: */
/* most significant byte is first. */
/* return number of bytes written. */
{
  /* Will this deal properly with the sign? */

  if (n < 1 || n > 4)
  {
    PRINT_PROGNAME;
    fprintf (stderr,
      "(put_signed) : INTERNAL ERROR : asked for %d bytes.  Must be 1 to 4.\n",
      n);
    dexit (1);
  }

  /* How do we ensure 2's complement representation? */
  /* Here's a trick that I hope is portable, given ANSI C. */
  /* See K&R (2nd edition), Appendix A6.2 "Integral Conversions". */

  /* Convert snum to U4 data type */
  put_unsigned (n, (U4) snum, dvi);

  return n;
}
/* put_signed */


int
check_bmes
#ifdef STDC
  (FILE * dtl)
#else
  (dtl)
  FILE * dtl;
#endif
/* check that a BMES_CHAR is the next non-whitespace character in dtl */
{
  int ch;  /* next non-whitespace character in dtl */

  /* `(Void)' because we ignore the number of spaces skipped */
  (Void) skip_space (dtl, &ch);

  if (ch < 0)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(check_bmes) : DTL FILE ERROR (%s) : ",
      dtl_filename);
    fprintf (stderr, "end of dtl file, or reading error\n");
    dexit (1);
  }

  if (ch != BMES_CHAR)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(check_bmes) : DTL FILE ERROR (%s) : ",
      dtl_filename);
    fprintf (stderr, "BMES_CHAR (`%c') expected before string, not `%c' (char %d).\n",
      BMES_CHAR, ch, ch);
    dexit (1);
  }

  return 1;  /* OK */
}
/* check_bmes */


int
check_emes
#ifdef STDC
  (FILE * dtl)
#else
  (dtl)
  FILE * dtl;
#endif
/* check that an EMES_CHAR is the next character in dtl */
{
  int ch;  /* dtl character */

  if (read_char (dtl, &ch) == 0)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(check_emes) : DTL FILE ERROR (%s) : ",
      dtl_filename);
    fprintf (stderr, "end of dtl file, or reading error\n");
    dexit (1);
  }

  if (ch != EMES_CHAR)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(check_emes) : DTL FILE ERROR (%s) : ",
      dtl_filename);
    fprintf (stderr, "EMES_CHAR (`%c') expected to follow string, not `%c' (char %d).\n",
      EMES_CHAR, ch, ch);
    dexit (1);
  }

  return 1;  /* OK */
}
/* check_emes */


/* Size typically used in this program for Lstring variables */
#define LSIZE 16384


Void
init_Lstring
#ifdef STDC
  (Lstring * lsp, long int n)
#else
  (lsp, n)
  Lstring * lsp;
  long int n;
#endif
{
  lsp->l = 0;
  lsp->m = n;
  lsp->s = (char *) gmalloc (n);
}
/* init_Lstring */


Void
de_init_Lstring
#ifdef STDC
  (Lstring * lsp)
#else
  (lsp)
  Lstring * lsp;
#endif
{
  lsp->l = 0;
  lsp->m = 0;
  free (lsp->s);
  lsp->s = NULL;  /* to be sure */
}
/* de_init_Lstring */


Lstring *
alloc_Lstring
#ifdef STDC
  (long int n)
#else
  (n)
  long int n;
#endif
{
  Lstring * lsp;
  lsp = (Lstring *) gmalloc (sizeof (Lstring));
  init_Lstring (lsp, n);
  return (lsp);
}
/* alloc_Lstring */


Void
free_Lstring
#ifdef STDC
  (Lstring * lstr)
#else
  (lstr)
  Lstring * lstr;
#endif
{
  free (lstr->s);
  free (lstr);
}
/* free_Lstring */


Void
ls_putb
#ifdef STDC
  (int ch, Lstring * lstr)
#else
  (ch, lstr)
  int ch;
  Lstring * lstr;
#endif
/* write byte ch into Lstring *lstr */
{
  if (lstr->l >= lstr->m)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(ls_putb) : ERROR : No more room in Lstring.\n");
    dexit (1);
  }
  else
  {
    lstr->s [(lstr->l)++] = ch;
  }
}
/* ls_putb */


long int
get_Lstring
#ifdef STDC
  (FILE * dtl, Lstring * lstr)
#else
  (dtl, lstr)
  FILE * dtl;
  Lstring * lstr;
#endif
/* get a string from dtl file, store as an Lstring in *lstr. */
/* lstr must already be allocated and initialised. */
/* return length of Lstring *lstr */
{
  U4 nch;
  int char_status = CHAR_OK;  /* OK so far */

  if (debug)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(get_Lstring) : entering get_Lstring.\n");
  }

  check_bmes (dtl);

  if (debug)
  {
    PRINT_PROGNAME;
    fprintf (stderr,
      "(get_Lstring) : string is: \"");
  }

  for (nch=0; ; nch++)
  {
    int ch;

    char_status = read_string_char (dtl, &ch);

    if (char_status == CHAR_FAIL)
    {
      /* end of dtl file, or reading error */
      fprintf (stderr, "\n");
      PRINT_PROGNAME;
      fprintf (stderr, "(get_Lstring) : DTL FILE ERROR (%s) : ",
        dtl_filename);
      fprintf (stderr, "cannot read string[");
      fprintf (stderr, UF4, nch);
      fprintf (stderr, "] from dtl file.\n");
      dexit (1);
    }

    if (debug)
    {
      fprintf (stderr, "%c", ch);
    }

    if (char_status == CHAR_EOS)
    {
      if (ch != EMES_CHAR)
      {
        PRINT_PROGNAME;
        fprintf (stderr, "(get_Lstring) : INTERNAL ERROR : ");
        fprintf (stderr, "char_status = CHAR_FAIL,\n");
        fprintf (stderr,
          "but ch = %c (char %d) is not EMES_CHAR = %c (char %d)\n",
          ch, ch, EMES_CHAR, EMES_CHAR);
        dexit (1);
      }
      (Void) unread_char ();
      break;  /* end of string */
    }
    else if (char_status == CHAR_OK)
    {
      ls_putb (ch, lstr);
    }
    else
    {
        PRINT_PROGNAME;
        fprintf (stderr, "(get_Lstring) : INTERNAL ERROR : ");
        fprintf (stderr, "char_status = %d is unfamiliar!\n", char_status);
        dexit (1);
    }
  }
  /* end for */

  if (debug)
  {
    fprintf (stderr, "\".\n");
  }

  check_emes (dtl);

  if (debug)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(get_Lstring) : leaving get_Lstring.\n");
  }

  return (lstr->l);
}
/* get_Lstring */


Void
put_Lstring
#ifdef STDC
  (const Lstring * lstr, FILE * dvi)
#else
  (str, dvi)
  Lstring lstr;
  FILE * dvi;
#endif
{
  size_t fwret;
  fwret = fwrite ((lstr->s), sizeof (char), (size_t) (lstr->l), dvi);

  dvi_written += fwret;

  if (fwret < lstr->l)
  {
    PRINT_PROGNAME;
    fprintf (stderr,
      "(put_Lstring) : DVI File ERROR : not all bytes written ");
    fprintf (stderr, "(%ld of %ld).\n",
      (long int) fwret, (long int) (lstr->l));
    dexit (1);
  }
}
/* put_Lstring */


U4
xfer_len_string
#ifdef STDC
  (int n, FILE * dtl, FILE * dvi)
#else
  (n, dtl, dvi)
  int n;
  FILE * dtl;
  FILE * dvi;
#endif
/* transfer (length and) quoted string from dtl to dvi file, */
/* return number of bytes written to dvi file. */
{
  U4 k, k2;
  Lstring lstr;

  if (debug)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(xfer_len_string) : entering xfer_len_string.\n");
  }

  init_Lstring (&lstr, LSIZE);

  /* k[n] : length of special string */

  k = get_unsigned (dtl);

  if (debug)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(xfer_len_string) : string's nominal length k = ");
    fprintf (stderr, UF4, k);
    fprintf (stderr, " characters.\n");
  }

  k2 = get_Lstring (dtl, &lstr);

  if (k2 != k)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(xfer_len_string) : WARNING : string length (");
    fprintf (stderr, UF4, k);
    fprintf (stderr, ") in DTL file is wrong\n");
    fprintf (stderr, "Writing correct value (");
    fprintf (stderr, UF4, k2);
    fprintf (stderr, ") to DVI file\n");
  }

  put_unsigned (n, k2, dvi);

  put_Lstring (&lstr, dvi);

  if (debug)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(xfer_len_string) : leaving xfer_len_string.\n");
  }

  de_init_Lstring (&lstr);

  return (n + k2);
}
/* xfer_len_string */


S4
xfer_bop_address
#ifdef STDC
  (FILE * dtl, FILE * dvi)
#else
  (dtl, dvi)
  FILE * dtl;
  FILE * dvi;
#endif
/* translate signed 4-byte bop address from dtl to dvi file. */
/* return value of bop address written to DVI file */
{
  S4 snum = 0;  /* at most this space needed for byte address */
  int nconv = 0;  /* number of arguments converted by sscanf */
  static Token token = "";  /* DTL token */

  read_token (dtl, token);

  nconv = sscanf (token, SF4, &snum);

  if (nconv != 1)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(xfer_bop_address) : DTL FILE ERROR (%s) : ",
      dtl_filename);
    fprintf (stderr, "signed number expected, not \"%s\".\n", token);
    dexit (1);
  }

  if (snum != last_bop_address)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(xfer_bop_address) : WARNING : byte address (");
    fprintf (stderr, WF, snum);
    fprintf (stderr, ")\n");
    fprintf (stderr, "for previous bop in DTL file is wrong\n");
    fprintf (stderr, "Writing correct value (");
    fprintf (stderr, WF, last_bop_address);
    fprintf (stderr, ") to DVI file\n");
  }

  put_signed (4, last_bop_address, dvi);

  return last_bop_address;
}
/* xfer_bop_address */


S4
xfer_postamble_address
#ifdef STDC
  (FILE * dtl, FILE * dvi)
#else
  (dtl, dvi)
  FILE * dtl;
  FILE * dvi;
#endif
/* translate signed 4-byte postamble address from dtl to dvi file. */
/* return value of postamble address written to DVI file */
{
  S4 snum = 0;  /* at most this space needed for byte address */
  int nconv = 0;  /* number of arguments converted by sscanf */
  static Token token = "";  /* DTL token */

  read_token (dtl, token);

  nconv = sscanf (token, SF4, &snum);

  if (nconv != 1)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(xfer_postamble_address) : DTL FILE ERROR (%s) : ",
      dtl_filename);
    fprintf (stderr, "signed number expected, not \"%s\".\n", token);
    dexit (1);
  }

  if (snum != postamble_address)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(xfer_postamble_address) : WARNING : byte address (");
    fprintf (stderr, WF, snum);
    fprintf (stderr, ")\n");
    fprintf (stderr, "for postamble in DTL file is wrong\n");
    fprintf (stderr, "Writing correct value (");
    fprintf (stderr, WF, postamble_address);
    fprintf (stderr, ") to DVI file\n");
  }

  put_signed (4, postamble_address, dvi);

  return postamble_address;
}
/* xfer_postamble_address */


int
put_table
#ifdef STDC
  (op_table table, int opcode, FILE * dtl, FILE * dvi)
#else
  (table, opcode, dtl, dvi)
  op_table table;
  int opcode;
  FILE * dtl;
  FILE * dvi;
#endif
{
  /* table:  {char * name; int first, last; op_info * list; }; */
  /* op_info:   {int code; char * name; int nargs; char * args; }; */

  op_info op;  /* entry in table */
  int i;
  int pos;  /* current position in string being scanned */
  static String args = "";

  /* Defensive programming. */
  if (opcode < table.first || opcode > table.last)
  {
    PRINT_PROGNAME;
    fprintf (stderr,
      "(put_table) : DTL FILE (OR INTERNAL) ERROR : opcode %d ", opcode);
    fprintf (stderr, "is outside table %s [ %d to %d ] !\n",
      table.name, table.first, table.last);
    dexit (1);
  }

  op = table.list [ opcode - table.first ];

  /* More defensive programming. */
  if (opcode != op.code)
  {
    PRINT_PROGNAME;
    fprintf (stderr,
      "(put_table) : INTERNAL ERROR : opcode %d for command \"%s\"",
      opcode, op.name);
    fprintf (stderr, " faulty in table \"%s\".\n", table.name);
    dexit (1);
  }

  /* process all the arguments, according to size and sign */

  strncpy (args, op.args, MAXSTRLEN);

  pos = 0;
  for (i=0; i < op.nargs; i++)
  {
    int argtype = 0;
    int nscan = 0;  /* number of bytes read by sscanf */
    int nconv = 0;  /* number of sscanf arguments converted & assigned */

    /* sscanf() does NOT advance over its input: */
    /* C strings lack internal state information, which C files have. */
    /* On Sun/OS, sscanf calls ungetc on the string it reads, */
    /* which therefore has to be writable. */

    nconv = sscanf (args + pos, "%d%n", &argtype, &nscan);

    if (nconv < 1 || nscan < 1)
    {
      PRINT_PROGNAME;
      fprintf (stderr,
        "(put_table) : INTERNAL ERROR : internal read of table %s failed!\n",
        table.name);
      dexit (1);
    }

    pos += nscan;

    if (argtype < 0)
      xfer_signed (-argtype, dtl, dvi);
    else
      xfer_unsigned (argtype, dtl, dvi);
  }
  /* end for */

  return 1;  /* OK */
}
/* put_table */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */


U4
special
#ifdef STDC
  (FILE * dtl,  FILE * dvi,  int n)
#else
  (dtl,  dvi,  n)
  FILE * dtl;
  FILE * dvi;
  int n;
#endif
/* read special (1 <= n <= 4 byte) data from dtl, and write in dvi */
/* return number of bytes written */
{
  U4  nk;

  if (debug)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(special) : entering special.\n");
  }

  if (n < 1 || n > 4)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(special) : DTL FILE ERROR (%s) : special %d, ",
      dtl_filename, n);
    fprintf (stderr, "range is 1 to 4.\n");
    dexit (1);
  }

  /* k[n] : length of special string */
  /* x[k] : special string */
  /* nk = n + k */
  nk = xfer_len_string (n, dtl, dvi);

  if (debug)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(special) : leaving special.\n");
  }

  return (nk);
}
/* special */


int
fontdef
#ifdef STDC
  (FILE * dtl,  FILE * dvi,  int suffix)
#else
  (dtl,  dvi,  suffix)
  FILE * dtl;
  FILE * dvi;
  int suffix;
#endif
/* read fontdef fnt_def1 .. fnt_def4 from dtl, and write in dvi */
/* suffix is the fontdef suffix : 1 to 4 */
/* return number of bytes written */
{
  U4  a, l, a2, l2;
  U4 k;
  Lstring lstr1, lstr2;

  if (debug)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(fontdef) : entering fontdef.\n");
  }

  if (suffix < 1 || suffix > 4)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(fontdef) : DTL FILE ERROR (%s) : ",
      dtl_filename);
    fprintf (stderr, "font def %d, but range is 1 to 4.\n", suffix);
    dexit (1);
  }

  init_Lstring (&lstr1, LSIZE);
  init_Lstring (&lstr2, LSIZE);

  if (debug)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(fontdef) : about to read font number.\n");
  }

  /* k[suffix] : font number */
  if (suffix == 4)
    k = xfer_signed (suffix, dtl, dvi);
  else
    k = xfer_unsigned (suffix, dtl, dvi);

  if (debug)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(fontdef) : font ");
    fprintf (stderr, UF4, k);
    fprintf (stderr, ".\n");
  }

#ifdef HEX_CHECKSUM
  /* c[4] : (hexadecimal) checksum : I (gt) would prefer this */
  xfer_hex (4, dtl, dvi);
#else /* NOT HEX_CHECKSUM */
  /* c[4] : checksum (octal, for comparison with tftopl's .pl file) */
  xfer_oct (4, dtl, dvi);
#endif

  /* s[4] */
  xfer_unsigned (4, dtl, dvi);

  /* d[4] */
  xfer_unsigned (4, dtl, dvi);

  /* If DTL file's edited, a and l may be wrong. */

  /* a[1] : length of font `area' (directory) portion of pathname string */
  a = get_unsigned (dtl);

  /* l[1] : length of font portion of pathname string */
  l = get_unsigned (dtl);

  /* n[a+l] : font pathname string <= area + font */

  a2 = get_Lstring (dtl, &lstr1);

  if (a2 != a)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(fontdef) : WARNING : font area string's length (");
    fprintf (stderr, UF4, a);
    fprintf (stderr, ") in DTL file is wrong\n");
    fprintf (stderr, "Writing correct value (");
    fprintf (stderr, UF4, a2);
    fprintf (stderr, ") to DVI file\n");
  }

  put_unsigned (1, a2, dvi);

  l2 = get_Lstring (dtl, &lstr2);

  if (l2 != l)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(fontdef) : WARNING : font string's length (");
    fprintf (stderr, UF4, l);
    fprintf (stderr, ") in DTL file is wrong\n");
    fprintf (stderr, "Writing correct value (");
    fprintf (stderr, UF4, l2);
    fprintf (stderr, ") to DVI file\n");
  }

  put_unsigned (1, l2, dvi);

  put_Lstring (&lstr1, dvi);
  put_Lstring (&lstr2, dvi);

  de_init_Lstring (&lstr2);
  de_init_Lstring (&lstr1);

  if (debug)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(fontdef) : leaving fontdef.\n");
  }

  return (suffix + 4*4 + 2*1 + a2 + l2);
}
/* fontdef */


U4
preamble
#ifdef STDC
  (FILE * dtl,  FILE * dvi)
#else
  (dtl,  dvi)
  FILE * dtl;
  FILE * dvi;
#endif
/* read preamble from dtl, and write in dvi */
/* return number of bytes written */
{
  U4  k1;

  if (debug)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(preamble) : entering preamble.\n");
  }

  /* i[1] */
  xfer_unsigned (1, dtl, dvi);

  /* num[4] */
  xfer_unsigned (4, dtl, dvi);

  /* den[4] */
  xfer_unsigned (4, dtl, dvi);

  /* mag[4] */
  xfer_unsigned (4, dtl, dvi);

  /* k[1] : length of comment */
  /* x[k] : comment string */
  /* k1 = 1 + k */
  k1 = xfer_len_string (1, dtl, dvi);

  if (debug)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(preamble) : leaving preamble.\n");
  }

  return (1 + 3*4 + k1);
}
/* preamble */


int
postamble
#ifdef STDC
  (FILE * dtl,  FILE * dvi)
#else
  (dtl,  dvi)
  FILE * dtl;
  FILE * dvi;
#endif
/* read postamble from dtl, and write in dvi */
/* return number of bytes written */
{
  postamble_address = dvi_written - 1;

  /* p[4] : DVI address of previous bop command */
  /*        --- unsigned? --- or signed, as I assume? */
  /* For, surely  p  should be  -1  if the DVI file has NO bop? */
  xfer_bop_address (dtl, dvi);

  /* num[4] */
  xfer_unsigned (4, dtl, dvi);

  /* den[4] */
  xfer_unsigned (4, dtl, dvi);

  /* mag[4] */
  xfer_unsigned (4, dtl, dvi);

  /* l[4] */
  xfer_unsigned (4, dtl, dvi);

  /* u[4] */
  xfer_unsigned (4, dtl, dvi);

  /* s[2] */
  xfer_unsigned (2, dtl, dvi);

  /* t[2] */
  xfer_unsigned (2, dtl, dvi);

  return (6*4 + 2*2);
}
/* postamble */


int
post_post
#ifdef STDC
  (FILE * dtl,  FILE * dvi)
#else
  (dtl,  dvi)
  FILE * dtl;
  FILE * dvi;
#endif
/* read post_post from dtl, and write in dvi */
/* return number of bytes written */
{
  /* hope I'm writing the "223" bytes in an 8-bit clean way */
  int n223 = 0;  /* number of "223" bytes in final padding */

  /* q[4] : DVI address of post command */
  /*        --- unsigned? --- or signed, as I assume? */
  /* what happens if there is NO postamble command? */
  /* shouldn't  q  be  -1  then? */

  xfer_postamble_address (dtl, dvi);

  /* i[1] : DVI identification byte = 2 */
  xfer_unsigned (1, dtl, dvi);

  for (n223 = 0;  true;  n223++)
  {
    COUNT nread = 0;  /* number of DTL bytes read by read_token */
    static Token token;

    strcpy (token, "");

    nread = read_token (dtl, token);

    /* check whether end of dtl file */
    if (nread == 0)
    {
      if (group)
      {
	/* dtl file shouldn't end before an ECOM */
        PRINT_PROGNAME;
	fprintf (stderr, "(post_post) : DTL FILE ERROR (%s) : ",
          dtl_filename);
        fprintf (stderr, "premature end of DTL file!\n");
	fprintf (stderr,
	  "%d complete iterations of \"padding byte\" loop;\n", n223);
	fprintf (stderr, "troublesome token = \"%s\"\n", token);
	dexit (1);
      }
      /* leave the "223" loop */
      break;
    }
    else if (strcmp (token, "223") == 0)
    {
      /* token is a "223" padding byte */
      /* loop again */
    }
    else
    {
      /* read a non-empty token that wasn't "223" */
      (Void) unread_char ();
      if (group)
      {
	if (strcmp (token, ECOM) == 0)
	{
	  /* end of DTL's post_post command */
	}
	else
	{
	  /* error : expected end of post_post */
          PRINT_PROGNAME;
	  fprintf (stderr, "(post_post) : DTL FILE ERROR (%s) : ",
            dtl_filename);
	  fprintf (stderr, "token \"%s\" should be ECOM (\"%s\")\n",
            token, ECOM);
	  dexit (1);
	}
      }
      /* leave the "223" loop */
      break;
    }
  }
  /* end for */

  if (n223 < 4)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(post_post) : DTL FILE ERROR (%s) : \n",
      dtl_filename);
    fprintf (stderr, "fewer than four `223' padding bytes.\n");
    fprintf (stderr, "Will write at least four `223' padding bytes.\n");
  }

  /* check whether the DVI file size is a multiple of 4 bytes */
  if ((dvi_written + n223) % 4 != 0)
  {
    PRINT_PROGNAME;
    fprintf (stderr, "(post_post) : WARNING : \n");
    fprintf (stderr, "DVI size ");
    fprintf (stderr, WF, dvi_written);
    fprintf (stderr, " (bytes) wouldn't be a multiple of 4 !\n");
    fprintf (stderr,
      "Will write (at least four) `223' padding bytes until it is.\n");
  }

  /* final padding of DVI file by "223" bytes to a multiple of 4 bytes, */
  /* with at least 4 bytes */

  for (n223 = 0;  (n223 < 4) || (dvi_written % 4 != 0);  n223++)
  {
    /* add a "223" padding byte */
    put_byte (223, dvi);
  }

  return (4 + 1 + n223);
}
/* post_post */


/* end of dt2dv.c */
