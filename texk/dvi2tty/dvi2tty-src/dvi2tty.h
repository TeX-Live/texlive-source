/*
 * dvi2tty
 * Copyright (C) 2003 Marcel J.E. Mol <marcel@mesa.nl>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#define Progname "dvi2tty"
#define Copyright "Copyright (C) 1984, 1985, 1986 Svante Lindahl.\n\
Copyright (C) 1989-2010 M.J.E. Mol, MESA Consulting B.V."


#if defined(KPATHSEA)
# define NO_DEBUG 1
# include <kpathsea/config.h>
# include <kpathsea/readable.h>
#if defined(WIN32)
# include <kpathsea/variable.h>
#endif
# include <sys/types.h>
# include <sys/stat.h>
#else
# define TRUE        1
# define FALSE       0
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if (defined(WIN32) && !defined(__MINGW32__))
# include <fcntl.h>
typedef void *HANDLE;
typedef unsigned long DWORD;
#define STD_OUTPUT_HANDLE  ((DWORD)-11)
#define STD_ERROR_HANDLE   ((DWORD)-12)
#endif

#define nil         NULL

/*
 * Define the maximum width of the terminal
 * (this is also used to define the with of the line structure in dvistuff.c)
 */
#define MAXTERMWIDTH    332

#include <ptexenc/ptexenc.h>
#include <ptexenc/unicode.h>

/* internal encoding for NTT JTeX : "euc" */
#define JTEX_INTERNAL_ENC  "euc"

/* internal encoding for ASCII pTeX : "euc" or "sjis" */
#define PTEX_INTERNAL_ENC  "euc"

/* internal encoding for upTeX : "uptex" */
#define UPTEX_INTERNAL_ENC  "uptex"

/*
 * ERROR CODES , don't start with 0
 */

#define illop    1              /* illegal op-code                   */
#define stkof    2              /* stack over-flow                   */
#define stkuf    3              /* stack under-flow                  */
#define stkrq    4              /* stack requirement                 */
#define lnerq    5              /* line allocation                   */
#define badid    6              /* id is not right                   */
#define bdsgn    7              /* signature is wrong                */
#define fwsgn    8              /* too few signatures                */
#define nopre    9              /* no pre-amble where expected       */
#define nobop   10              /* no bop-command where expected     */
#define nopp    11              /* no postpost where expected        */
#define bdpre   12              /* unexpected preamble occured       */
#define bdbop   13              /* unexpected bop-command occured    */
#define bdpst   14              /* unexpected post-command occured   */
#define bdpp    15              /* unexpected postpost               */
#define nopst   16              /* no post-amble where expected      */
#define illch   17              /* character code out of range       */
#define filop   18              /* cannot access file                */
#define filcr   19              /* cannot creat file                 */
#define pipcr   20              /* cannot creat pipe                 */
#define bdfnt   21              /* fail to get font info             */



/*
 * Type definitions
 */

typedef char bool;

typedef struct prlistptr {      /* list of pages selected for output         */
    int       pag;                      /* number of pages                   */
    bool      all;                      /* pages in interval selected        */
    struct prlistptr *prv;              /* previous item in list             */
    struct prlistptr *nxt;              /* next item in list                 */
} printlisttype;



/*
 * Variable definitions
 */

extern bool   outputtofile;            /* output to file or stdout(dvi2tty.c)*/
extern bool   pageswitchon;            /* user-set pages to print(dvistuff.c)*/
extern bool   sequenceon;              /* not TeX pagenrs (dvistuff.c)       */
extern bool   scascii;                 /* Scand. nat. chars (dvistuff.c)     */
extern bool   latin1;                  /* latin1 chars (dvistuff.c)          */
extern bool   utf8;                    /* print by utf8 encoding (dvistuff.c) */
extern bool   noligaturefi;            /* do not use ligature for ff,fi,fl,ffi,ffl (dvistuff.c) */
extern bool   accent;                  /* Output accent stuff(dvistuff.c)    */
extern bool   ttfont;                  /* tt font assumed   (dvistuff.c)     */
extern bool   jautodetect;             /* Autodetect NTT jTeX, ASCII pTeX and upTeX (dvistuff.c) */
extern bool   nttj;                    /* NTT jTeX font support (dvistuff.c) */
extern bool   asciip;                  /* ASCII pTeX font support (dvistuff.c) */
extern bool   uptex;                   /* upTeX font support (dvistuff.c)    */
extern bool   japan;                   /* japanized TeX font support (dvistuff.c) */
extern bool   noffd;                   /* output ^L or formfeed (dvistuff.c) */
extern bool   printfont;               /* include font switches (dvistuff.c) */
extern bool   compose;                 /* try to compose a combining character sequence (dvistuff.c) */
extern bool   allchar;                 /* output all characters (dvistuff.c) */

extern printlisttype *currentpage;     /* current page to print (dvi2tty.c)  */
extern printlisttype *firstpage;       /* first page selected (dvi2tty.c)    */
extern printlisttype *lastpage;        /* last page selected (dvi2tty.c)     */

extern int            ttywidth;        /* screen width (dvi2tty.c)           */
extern int            espace;          /* extra screen width (dvi2tty.c)     */
extern long           foo;             /* temporary 'register' (dvi2tty.c)   */
extern long           lineheight;      /* height of a line (dvistuff.c)      */
extern int            opcode;          /* dvi opcode (dvistuff.c)            */
extern const char *   delim;           /* printer font delim (dvistuff.c)    */

extern FILE *DVIfile;                  /* dvi file (dvi2tty.c)               */
extern FILE *output;                   /* output file (dvi2tty.c)            */



/*
 * Funtion declarations
 */

/* dvi2tty.c */
void errorexit(int errorcode);

/* dvistuff.c */
void dvimain(void);
