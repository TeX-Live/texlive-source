/* dv2dt - convert DVI file to human-readable "DTL" format.
   
   This file is public domain.
   Originally written 1995, Geoffrey Tobin.
   The author has expressed the hope that any modification will retain enough content to remain useful. He would also appreciate being acknowledged as the original author in the documentation.
   This declaration added 2008/11/14 by Clea F. Rees with the permission of Geoffrey Tobin.

   - (ANSI C) version 0.6.0 - 17:54 GMT +11  Wed 8 March 1995
   - author:  Geoffrey Tobin    ecsgrt@luxor.latrobe.edu.au
   - patch:  Michal Tomczak-Jaegermann   ntomczak@vm.ucs.ualberta.ca
   - Reference:  "The DVI Driver Standard, Level 0",
                 by  The TUG DVI Driver Standards Committee.
                 Appendix A, "Device-Independent File Format".
*/

/* unix version; read from stdin, write to stdout, by default. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef KPATHSEA
#include <kpathsea/c-fopen.h>
#endif

#include "dtl.h"

#define PRINT_BCOM   if (group) fprintf (dtl, "%s", BCOM)
#define PRINT_ECOM   if (group) fprintf (dtl, "%s", ECOM)

/*
  operation's:
     opcode,
     name,
     number of args,
     string of arguments.
*/
struct op_info_st {int code; const char * name; int nargs; const char * args; };

typedef  struct op_info_st  op_info;

/*
  table's:
     name,
     first opcode,
     last opcode,
     pointer to opcode info.
*/
struct op_table_st {const char * name; int first; int last; op_info * list; };

typedef  struct op_table_st  op_table;

/* Table for opcodes 128 to 170 inclusive. */

op_info  op_info_128_170 [] =
{
  {128, "s1", 1, "1"},
  {129, "s2", 1, "2"},
  {130, "s3", 1, "3"},
  {131, "s4", 1, "-4"},
  {132, "sr", 2, "-4 -4"},
  {133, "p1", 1, "1"},
  {134, "p2", 1, "2"},
  {135, "p3", 1, "3"},
  {136, "p4", 1, "-4"},
  {137, "pr", 2, "-4 -4"},
  {138, "nop", 0, ""},
  {139, "bop", 11, "-4 -4 -4 -4 -4 -4 -4 -4 -4 -4 -4"},
  {140, "eop", 0, ""},
  {141, "[", 0, ""},
  {142, "]", 0, ""},
  {143, "r1", 1, "-1"},
  {144, "r2", 1, "-2"},
  {145, "r3", 1, "-3"},
  {146, "r4", 1, "-4"},
  {147, "w0", 0, ""},
  {148, "w1", 1, "-1"},
  {149, "w2", 1, "-2"},
  {150, "w3", 1, "-3"},
  {151, "w4", 1, "-4"},
  {152, "x0", 0, ""},
  {153, "x1", 1, "-1"},
  {154, "x2", 1, "-2"},
  {155, "x3", 1, "-3"},
  {156, "x4", 1, "-4"},
  {157, "d1", 1, "-1"},
  {158, "d2", 1, "-2"},
  {159, "d3", 1, "-3"},
  {160, "d4", 1, "-4"},
  {161, "y0", 0, ""},
  {162, "y1", 1, "-1"},
  {163, "y2", 1, "-2"},
  {164, "y3", 1, "-3"},
  {165, "y4", 1, "-4"},
  {166, "z0", 0, ""},
  {167, "z1", 1, "-1"},
  {168, "z2", 1, "-2"},
  {169, "z3", 1, "-3"},
  {170, "z4", 1, "-4"}
};  /* op_info  op_info_128_170 [] */

op_table  op_128_170  =  {"op_128_170", 128, 170, op_info_128_170};

/* Table for font with 1 to 4 bytes (opcodes 235 to 238) inclusive. */

op_info  fnt_n [] =
{
  {235, "f1", 1, "1"},
  {236, "f2", 1, "2"},
  {237, "f3", 1, "3"},
  {238, "f4", 1, "-4"}
};  /* op_info  fnt_n [] */

op_table  fnt  =  {"f", 235, 238, fnt_n};


/* function prototypes */

int open_dvi ARGS((char * dvi_file, FILE ** dvi));
int open_dtl ARGS((char * dtl_file, FILE ** dtl));
int dv2dt ARGS((FILE * dvi, FILE * dtl));

COUNT wunsigned ARGS((int n,  FILE * dvi,  FILE * dtl));
COUNT wsigned   ARGS((int n,  FILE * dvi,  FILE * dtl));
S4 rsigned   ARGS((int n,  FILE * dvi));
U4 runsigned ARGS((int n,  FILE * dvi));

COUNT wtable ARGS((op_table table, int opcode, FILE * dvi, FILE * dtl));

COUNT setseq ARGS((int opcode, FILE * dvi, FILE * dtl));
Void setpchar ARGS((int charcode, FILE * dtl));
Void xferstring ARGS((int k, FILE * dvi, FILE * dtl));

COUNT special ARGS((FILE * dvi,  FILE * dtl,  int n));
COUNT fontdef ARGS((FILE * dvi,  FILE * dtl,  int n));
COUNT preamble  ARGS((FILE * dvi,  FILE * dtl));
COUNT postamble ARGS((FILE * dvi,  FILE * dtl));
COUNT postpost  ARGS((FILE * dvi,  FILE * dtl));


String program;  /* name of dv2dt program */

int
main
#ifdef STDC
  (int argc,  char * argv[])
#else
  (argc, argv)
  int argc;
  char * argv[];
#endif
{
  FILE * dvi = stdin;
  FILE * dtl = stdout;

  /* Watch out:  C's standard library's string functions are dicey */
  strncpy (program, argv[0], MAXSTRLEN);

  if (argc > 1)
    open_dvi (argv[1], &dvi);
  else if (!isatty(fileno(dvi)))
    SET_BINARY(fileno(dvi));

  if (argc > 2)
    open_dtl (argv[2], &dtl);

  dv2dt (dvi, dtl);

  return 0;  /* OK */
}
/* end main */

int
open_dvi
#ifdef STDC
  (char * dvi_file, FILE ** pdvi)
#else
  (dvi_file, pdvi)
  char * dvi_file;
  FILE ** pdvi;
#endif
/* I:  dvi_file;  I:  pdvi;  O:  *pdvi. */
{
  if (pdvi == NULL)
  {
    fprintf (stderr, "%s:  address of dvi variable is NULL.\n", program);
    exit (1);
  }

  *pdvi = fopen (dvi_file, "rb");

  if (*pdvi == NULL)
  {
    fprintf (stderr, "%s:  Cannot open \"%s\" for binary reading.\n",
      program, dvi_file);
    exit (1);
  }

  return 1;  /* OK */
}
/* open_dvi */

int
open_dtl
#ifdef STDC
  (char * dtl_file, FILE ** pdtl)
#else
  (dtl_file, pdtl)
  char * dtl_file;
  FILE ** pdtl;
#endif
/* I:  dtl_file;  I:  pdtl;  O:  *pdtl. */
{
  if (pdtl == NULL)
  {
    fprintf (stderr, "%s:  address of dtl variable is NULL.\n", program);
    exit (1);
  }

  *pdtl = fopen (dtl_file, "wb");

  if (*pdtl == NULL)
  {
    fprintf (stderr, "%s:  Cannot open \"%s\" for writing.\n",
      program, dtl_file);
    exit (1);
  }

  return 1;  /* OK */
}
/* open_dtl */

int
dv2dt
#ifdef STDC
  (FILE * dvi, FILE * dtl)
#else
  (dvi, dtl)
  FILE * dvi;
  FILE * dtl;
#endif
{
  int opcode;
  COUNT count;  /* intended to count bytes to DVI file; as yet unused. */

  PRINT_BCOM;
  fprintf (dtl, "variety ");
/*  fprintf (dtl, BMES); */
  fprintf (dtl, VARIETY);
/*  fprintf (dtl, EMES); */
  PRINT_ECOM;
  fprintf (dtl, "\n");

  /* start counting DVI bytes */
  count = 0;
  while ((opcode = fgetc (dvi)) != EOF)
  {
    PRINT_BCOM;  /* start of command and parameters */
    if (opcode < 0 || opcode > 255)
    {
      count += 1;
      fprintf (stderr, "%s:  Non-byte from \"fgetc()\"!\n", program);
      exit (1);
    }
    else if (opcode <= 127)
    {
      /* setchar commands */
      /* count += 1; */
      /* fprintf (dtl, "%s%d", SETCHAR, opcode); */
      count +=
      setseq (opcode, dvi, dtl);
    }
    else if (opcode >= 128 && opcode <= 170)
    {
      count +=
      wtable (op_128_170, opcode, dvi, dtl);
    }
    else if (opcode >= 171 && opcode <= 234)
    {
      count += 1;
      fprintf (dtl, "%s%d", FONTNUM, opcode - 171);
    }
    else if (opcode >= 235 && opcode <= 238)
    {
      count +=
      wtable (fnt, opcode, dvi, dtl);
    }
    else if (opcode >= 239 && opcode <= 242)
    {
      count +=
      special (dvi, dtl, opcode - 238);
    }
    else if (opcode >= 243 && opcode <= 246)
    {
      count +=
      fontdef (dvi, dtl, opcode - 242);
    }
    else if (opcode == 247)
    {
      count +=
      preamble (dvi, dtl);
    }
    else if (opcode == 248)
    {
      count +=
      postamble (dvi, dtl);
    }
    else if (opcode == 249)
    {
      count +=
      postpost (dvi, dtl);
    }
    else if (opcode >= 250 && opcode <= 255)
    {
      count += 1;
      fprintf (dtl, "opcode%d", opcode);
    }
    else
    {
      count += 1;
      fprintf (stderr, "%s:  unknown byte.\n", program);
      exit (1);
    }
    PRINT_ECOM;  /* end of command and parameters */
    fprintf (dtl, "\n");
    if (fflush (dtl) == EOF)
    {
      fprintf (stderr, "%s:  fflush on dtl file gave write error!\n", program);
      exit (1);
    }
  } /* end while */

  return 1;  /* OK */
}
/* dv2dt */


COUNT
wunsigned
#ifdef STDC
  (int n, FILE * dvi, FILE * dtl)
#else
  (n, dvi, dtl)
  int n;
  FILE * dvi;
  FILE * dtl;
#endif
{
  U4 unum;

  fprintf (dtl, " ");
  unum = runsigned (n, dvi);
  fprintf (dtl, UF4, unum);
  return n;
}
/* end wunsigned */

COUNT
wsigned
#ifdef STDC
  (int n, FILE * dvi, FILE * dtl)
#else
  (n, dvi, dtl)
  int n;
  FILE * dvi;
  FILE * dtl;
#endif
{
  S4 snum;

  fprintf (dtl, " ");
  snum = rsigned (n, dvi);
  fprintf (dtl, SF4, snum);
  return n;
}
/* end wsigned */

U4
runsigned
#ifdef STDC
  (int n,  FILE * dvi)
#else
  (n, dvi)
  int n;
  FILE * dvi;
#endif
/* read 1 <= n <= 4 bytes for an unsigned integer from dvi file */
/* DVI format uses Big-endian storage of numbers. */
{
  U4 integer;
  int ibyte = 0;
  int i;

  if (n < 1 || n > 4)
  {
    fprintf (stderr,
      "%s:  runsigned() asked for %d bytes.  Must be 1 to 4.\n", program, n);
    exit (1);
  }

  /* Following calculation works iff storage is big-endian. */
  integer = 0;
  for (i = 0; i < n; i++)
  {
    integer *= 256;
    ibyte = fgetc (dvi);
    integer += ibyte;
  }

  return integer;
}
/* end runsigned */

S4
rsigned
#ifdef STDC
  (int n,  FILE * dvi)
#else
  (n, dvi)
  int n;
  FILE * dvi;
#endif
/* read 1 <= n <= 4 bytes for a signed integer from dvi file */
/* DVI format uses Big-endian storage of numbers. */
{
  S4 integer;
  int ibyte = 0;
  int i;

  if (n < 1 || n > 4)
  {
    fprintf (stderr,
      "%s:  rsigned() asked for %d bytes.  Must be 1 to 4.\n", program, n);
    exit (1);
  }

  /* Following calculation works iff storage is big-endian. */
  integer = 0;
  for (i = 0; i < n; i++)
  {
    integer *= 256;
    ibyte = fgetc (dvi);
    /* Big-endian implies sign byte is first byte. */
    if (i == 0 && ibyte >= 128)
    {
      ibyte -= 256;
    }
    integer += ibyte;
  }

  return integer;
}
/* end rsigned */

COUNT
wtable
#ifdef STDC
  (op_table table, int opcode, FILE * dvi, FILE * dtl)
#else
  (table, opcode, dvi, dtl)
  op_table table;
  int opcode;
  FILE * dvi;
  FILE * dtl;
#endif
/* write command with given opcode in given table */
/* return number of DVI bytes in this command */
{
  op_info op;  /* pointer into table of operations and arguments */
  COUNT bcount = 0;  /* number of bytes in arguments of this opcode */
  String args;  /* arguments string */
  int i;  /* count of arguments read from args */
  int pos;  /* position in args */

  /* Defensive programming. */
  if (opcode < table.first || opcode > table.last)
  {
    fprintf (stderr,
      "%s: opcode %d is outside table %s [ %d to %d ] !\n",
      program, opcode, table.name, table.first, table.last);
    exit (1);
  }

  op = table.list [opcode - table.first];

  /* Further defensive programming. */
  if (op.code != opcode)
  {
    fprintf (stderr, "%s: internal table %s wrong!\n", program, table.name);
    exit (1);
  }

  bcount = 1;
  fprintf (dtl, "%s", op.name);

  /* NB:  sscanf does an ungetc, */
  /*      so args must be writable. */

  strncpy (args, op.args, MAXSTRLEN);

  pos = 0;
  for (i = 0; i < op.nargs; i++)
  {
    int argtype;  /* sign and number of bytes in current argument */
    int nconv;  /* number of successful conversions from args */
    int nread;  /* number of bytes read from args */

    nconv = sscanf (args + pos, "%d%n", &argtype, &nread);

    /* internal consistency checks */
    if (nconv != 1 || nread <= 0)
    {
      fprintf (stderr,
        "%s: internal read of table %s failed!\n", program, table.name);
      exit (1);
    }

    pos += nread;

    bcount += ( argtype < 0 ?
               wsigned  (-argtype, dvi, dtl) :
               wunsigned (argtype, dvi, dtl)  ) ;
  } /* end for */

  return bcount;

}
/* wtable */

COUNT
setseq
#ifdef STDC
  (int opcode, FILE * dvi, FILE * dtl)
#else
  (opcode, dvi, dtl)
  int opcode;
  FILE * dvi;
  FILE * dtl;
#endif
/* write a sequence of setchar commands */
/* return count of DVI bytes interpreted into DTL */
{
  int charcode = opcode;  /* fortuitous */
  int ccount = 0;

  if (!isprint (charcode))
  {
    ccount = 1;
    fprintf (dtl, "%s%02X", SETCHAR, opcode);
  }
  else
  {
    /* start of sequence of font characters */
    fprintf (dtl, BSEQ);

    /* first character */
    ccount = 1;
    setpchar (charcode, dtl);

    /* subsequent characters */
    while ((opcode = fgetc (dvi)) != EOF)
    {
      if (opcode < 0 || opcode > 127)
      {
        break;  /* not a setchar command, so sequence has ended */
      }
      charcode = opcode;  /* fortuitous */
      if (!isprint (charcode))  /* not printable ascii */
      {
        break;  /* end of font character sequence, as for other commands */
      }
      else  /* printable ASCII */
      {
        ccount += 1;
        setpchar (charcode, dtl);
      }
    }  /* end for loop */

    /* prepare to reread opcode of next DVI command */
    if (ungetc (opcode, dvi) == EOF)
    {
      fprintf (stderr, "setseq:  cannot push back a byte\n");
      exit (1);
    }

    /* end of sequence of font characters */
    fprintf (dtl, ESEQ);
  }
  return ccount;
}
/* setseq */

Void
setpchar
#ifdef STDC
  (int charcode, FILE * dtl)
#else
  (charcode, dtl)
  int charcode;
  FILE * dtl;
#endif
/* set printable character */
{
  switch (charcode)
  {
    case ESC_CHAR:
      fprintf (dtl, "%c", ESC_CHAR);
      fprintf (dtl, "%c", ESC_CHAR);
      break;
    case QUOTE_CHAR:
      fprintf (dtl, "%c", ESC_CHAR);
      fprintf (dtl, "%c", QUOTE_CHAR);
      break;
    case BSEQ_CHAR:
      fprintf (dtl, "%c", ESC_CHAR);
      fprintf (dtl, "%c", BSEQ_CHAR);
      break;
    case ESEQ_CHAR:
      fprintf (dtl, "%c", ESC_CHAR);
      fprintf (dtl, "%c", ESEQ_CHAR);
      break;
    default:
      fprintf (dtl, "%c", charcode);
      break;
  }
}
/* setpchar */

Void
xferstring
#ifdef STDC
  (int k, FILE * dvi, FILE * dtl)
#else
  (k, dvi, dtl)
  int k;
  FILE * dvi;
  FILE * dtl;
#endif
/* copy string of k characters from dvi file to dtl file */
{
  int i;
  int ch;

  fprintf (dtl, " ");
  fprintf (dtl, "'");
  for (i=0; i < k; i++)
  {
    ch = fgetc (dvi);
    if (ch == ESC_CHAR || ch == EMES_CHAR)
    {
      fprintf (dtl, "%c", ESC_CHAR);
    }
    fprintf (dtl, "%c", ch);
  }
  fprintf (dtl, "'");
}
/* xferstring */

COUNT
special
#ifdef STDC
  (FILE * dvi,  FILE * dtl,  int n)
#else
  (dvi, dtl, n)
  FILE * dvi;
  FILE * dtl;
  int n;
#endif
/* read special 1 .. 4 from dvi and write in dtl */
/* return number of DVI bytes interpreted into DTL */
{
  U4  k;

  if (n < 1 || n > 4)
  {
    fprintf (stderr, "%s:  special %d, range is 1 to 4.\n", program, n);
    exit (1);
  }

  fprintf (dtl, "%s%d", SPECIAL, n);

  /* k[n] = length of special string */
  fprintf (dtl, " ");
  k = runsigned (n, dvi);
  fprintf (dtl, UF4, k);

  /* x[k] = special string */
  xferstring (k, dvi, dtl);

  return (1 + n + k);
}
/* end special */

COUNT
fontdef
#ifdef STDC
  (FILE * dvi,  FILE * dtl,  int n)
#else
  (dvi,  dtl,  n)
  FILE * dvi;
  FILE * dtl;
  int n;
#endif
/* read fontdef 1 .. 4 from dvi and write in dtl */
/* return number of DVI bytes interpreted into DTL */
{
  U4 ku, c, s, d, a, l;
  S4 ks;

  if (n < 1 || n > 4)
  {
    fprintf (stderr, "%s:  font def %d, range is 1 to 4.\n", program, n);
    exit (1);
  }

  fprintf (dtl, "%s%d", FONTDEF, n);

  /* k[n] = font number */
  fprintf (dtl, " ");
  if (n == 4)
  {
    ks = rsigned (n, dvi);
    fprintf (dtl, SF4, ks);
  }
  else
  {
    ku = runsigned (n, dvi);
    fprintf (dtl, UF4, ku);
  }

  /* c[4] = checksum */
  fprintf (dtl, " ");
  c = runsigned (4, dvi);
#ifdef HEX_CHECKSUM
  fprintf (dtl, XF4, c);
#else /* NOT HEX_CHECKSUM */
  /* write in octal, to allow quick comparison with tftopl's output */
  fprintf (dtl, OF4, c);
#endif

  /* s[4] = scale factor */
  fprintf (dtl, " ");
  s = runsigned (4, dvi);
  fprintf (dtl, UF4, s);

  /* d[4] = design size */
  fprintf (dtl, " ");
  d = runsigned (4, dvi);
  fprintf (dtl, UF4, d);

  /* a[1] = length of area (directory) name */
  a = runsigned (1, dvi);
  fprintf (dtl, " ");
  fprintf (dtl, UF4, a);

  /* l[1] = length of font name */
  l = runsigned (1, dvi);
  fprintf (dtl, " ");
  fprintf (dtl, UF4, l);

  /* n[a+l] = font pathname string => area (directory) + font */
  xferstring (a, dvi, dtl);
  xferstring (l, dvi, dtl);

  return (1 + n + 4 + 4 + 4 + 1 + 1 + a + l);
}
/* end fontdef */

COUNT
preamble
#ifdef STDC
  (FILE * dvi,  FILE * dtl)
#else
  (dvi,  dtl)
  FILE * dvi;
  FILE * dtl;
#endif
/* read preamble from dvi and write in dtl */
/* return number of DVI bytes interpreted into DTL */
{
  U4 id, num, den, mag, k;

  fprintf (dtl, "pre");

  /* i[1] = DVI format identification */
  fprintf (dtl, " ");
  id = runsigned (1, dvi);
  fprintf (dtl, UF4, id);

  /* num[4] = numerator of DVI unit */
  fprintf (dtl, " ");
  num = runsigned (4, dvi);
  fprintf (dtl, UF4, num);

  /* den[4] = denominator of DVI unit */
  fprintf (dtl, " ");
  den = runsigned (4, dvi);
  fprintf (dtl, UF4, den);

  /* mag[4] = 1000 x magnification */
  fprintf (dtl, " ");
  mag = runsigned (4, dvi);
  fprintf (dtl, UF4, mag);

  /* k[1] = length of comment */
  fprintf (dtl, " ");
  k = runsigned (1, dvi);
  fprintf (dtl, UF4, k);

  /* x[k] = comment string */
  xferstring (k, dvi, dtl);

  return (1 + 1 + 4 + 4 + 4 + 1 + k);
}
/* end preamble */

COUNT
postamble
#ifdef STDC
  (FILE * dvi,  FILE * dtl)
#else
  (dvi,  dtl)
  FILE * dvi;
  FILE * dtl;
#endif
/* read postamble from dvi and write in dtl */
/* return number of bytes */
{
  U4 p, num, den, mag, l, u, s, t;

  fprintf (dtl, "post");

  /* p[4] = pointer to final bop */
  fprintf (dtl, " ");
  p = runsigned (4, dvi);
  fprintf (dtl, UF4, p);

  /* num[4] = numerator of DVI unit */
  fprintf (dtl, " ");
  num = runsigned (4, dvi);
  fprintf (dtl, UF4, num);

  /* den[4] = denominator of DVI unit */
  fprintf (dtl, " ");
  den = runsigned (4, dvi);
  fprintf (dtl, UF4, den);

  /* mag[4] = 1000 x magnification */
  fprintf (dtl, " ");
  mag = runsigned (4, dvi);
  fprintf (dtl, UF4, mag);

  /* l[4] = height + depth of tallest page */
  fprintf (dtl, " ");
  l = runsigned (4, dvi);
  fprintf (dtl, UF4, l);

  /* u[4] = width of widest page */
  fprintf (dtl, " ");
  u = runsigned (4, dvi);
  fprintf (dtl, UF4, u);

  /* s[2] = maximum stack depth */
  fprintf (dtl, " ");
  s = runsigned (2, dvi);
  fprintf (dtl, UF4, s);

  /* t[2] = total number of pages (bop commands) */
  fprintf (dtl, " ");
  t = runsigned (2, dvi);
  fprintf (dtl, UF4, t);

/*  return (29);  */
  return (1 + 4 + 4 + 4 + 4 + 4 + 4 + 2 + 2);
}
/* end postamble */

COUNT
postpost
#ifdef STDC
  (FILE * dvi,  FILE * dtl)
#else
  (dvi,  dtl)
  FILE * dvi;
  FILE * dtl;
#endif
/* read post_post from dvi and write in dtl */
/* return number of bytes */
{
  U4 q, id;
  int b223;  /* hope this is 8-bit clean */
  int n223;  /* number of "223" bytes in final padding */

  fprintf (dtl, "post_post");

  /* q[4] = pointer to post command */
  fprintf (dtl, " ");
  q = runsigned (4, dvi);
  fprintf (dtl, UF4, q);

  /* i[1] = DVI identification byte */
  fprintf (dtl, " ");
  id = runsigned (1, dvi);
  fprintf (dtl, UF4, id);

  /* final padding by "223" bytes */
  /* hope this way of obtaining b223 is 8-bit clean */
  for (n223 = 0; (b223 = fgetc (dvi)) == 223; n223++)
  {
    fprintf (dtl, " ");
    fprintf (dtl, "%d", 223);
  }
  if (n223 < 4)
  {
    fprintf (stderr,
      "%s:  bad post_post:  fewer than four \"223\" bytes.\n", program);
    exit (1);
  }
  if (b223 != EOF)
  {
    fprintf (stderr,
      "%s:  bad post_post:  doesn't end with a \"223\".\n", program);
    exit (1);
  }

  return (1 + 4 + 1 + n223);
}
/* end postpost */

/* end of "dv2dt.c" */
