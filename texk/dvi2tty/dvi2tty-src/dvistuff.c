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

/*
 * Include files
 */

#include "dvi2tty.h"

#include <sys/types.h>
#include <sys/stat.h>

#include "commands.h"
#include "tex2ucs.h"


/*
 * Constant definitions
 */

#define mseek fseek

#define VERSIONID            2 /* dvi version number that pgm handles      */
#define VERTICALEPSILON 450000L /* crlf when increasing v more than this   */

#define rightmargin     MAXTERMWIDTH+20
                               /* nr of columns allowed to the right of h=0*/
#define leftmargin      -50    /* give some room for negative h-coordinate */
#define LINELEN         rightmargin - leftmargin + 1

#define MOVE            TRUE   /* if advancing h when outputing a rule     */
#define STAY            FALSE  /* if not advancing h when outputing a rule */

#define absolute        0      /* for seeking in files                     */
#define relative        1

#define FORM             12    /* formfeed                                 */
#define SPACE            32    /* space                                    */
#define DEL             127    /* delete                                   */

#define LASTCHAR        127    /* max dvi character, above are commands    */
#define LASTCHAR8B      255

#define IS_UNICODE  0x400000    /* flag for unicode                        */
#define MAX_UNICODE 0x10FFFF    /* max unicode                             */

#define IMIN(a, b)      (a<b ? a : b)
#define IMAX(a, b)      (a>b ? a : b)

#define get1()          num(1)
#define get2()          num(2)
#define get3()          num(3)
#define get4()          num(4)
#define sget1()         snum(1)
#define sget2()         snum(2)
#define sget3()         snum(3)
#define sget4()         snum(4)


/*
 * Structure and variable definitions
 */

/* const char *dvistuff = "@(#) dvistuff.c  " VERSION " 20101027 M.J.E. Mol (c) 1989-2010"; */

typedef struct {
    long hh;
    long vv;
    long ww;
    long xx;
    long yy;
    long zz;
} stackitem;

typedef struct lineptr {        /* the lines of text to be output to outfile */
    long            vv;                 /* vertical position of the line     */
    int             charactercount;     /* pos of last char on line          */
    struct lineptr *prev;               /* preceding line                    */
    struct lineptr *next;               /* succeeding line                   */
    long            text[LINELEN+1];    /* leftmargin...rightmargin          */
} linetype;

typedef struct _font {
    long    num;
    struct _font * next;
    char  * name;
    unsigned char  flags; /* to store font encoding types */
    int     fontnum; /* helper for japanese fonts */
    bool    is8bit;  /* 8bit fonts */
} font;

#define TTFONT    0x01
#define SYMFONT   0x02
#define MIFONT    0x03
#define T1FONT    0x04
#define TS1FONT   0x05
#define OT2FONT   0x10
#define T2AFONT   0x11
#define T2BFONT   0x12
#define T2CFONT   0x13
#define X2FONT    0x14
#define JPFONT    0x80



bool        pageswitchon;       /* true if user-set pages to print           */
bool        sequenceon;         /* false if pagesw-nrs refers to TeX-nrs     */
bool        scascii;            /* if true make Scand. nat. chars right      */
bool        latin1;             /* if true make latin1 chars right           */
bool        utf8;               /* if true print by utf8 encoding            */
bool        noligaturefi;       /* if true do not use ligature for ff,fi,fl,ffi,ffl  */
bool        accent;             /* if true output accents etc: \'{e} etc.    */
bool        ttfont = FALSE;     /* if true we assumed ttfonts, not cmr       */
bool        symbolfont = FALSE; /* true if font is a symbol font             */
bool        nttj = FALSE;       /* switch to NTT japanese fonts ...          */
bool        asciip = FALSE;      /* switch to ASCII japanese fonts ...       */
bool        uptex = FALSE;      /* switch to upTeX CJK fonts ...             */
bool        japan = FALSE;      /* switch to NTT/ASCII/.. japanese fonts ... */
bool        jautodetect = FALSE; /* switch if do auto detection of Japanese TeX */
bool        jdetect = FALSE;     /* switch if Japanese TeX detection is done */
bool        mifont = FALSE;      /* ASCII japanese font ??? */
bool        is8bit = FALSE;     /* true if 8bit encoding font                */
bool        noffd;              /* if true output ^L instead of formfeed     */
const char *delim;              /* -bdelim for font switch printing          */
bool        printfont;          /* true if user wants font switches printed  */
bool        compose;            /* if true try to compose a combining character sequence */
bool        allchar;            /* true if user sets all characters          */
                                /* overrides sscasci, accent                 */

int         opcode;             /* dvi-opcodes                               */

long        h, v;               /* coordinates, horizontal and vertical      */
long        w, x, y, z;         /* horizontal and vertical amounts           */

long        pagecounter;        /* sequence page number counter              */
long        backpointer;        /* pointer for offset to previous page       */
long        pagenr;             /* TeX page number                           */
int         stackmax;           /* stacksize required                        */

long        maxpagewidth;       /* width of widest page in file              */
long        charwidth;          /* aprox width of character                  */
long        lineheight = VERTICALEPSILON;
                                /* aprox height of a line                    */

linetype *  currentline;        /* pointer to current line on current page   */
linetype *  firstline;          /* pointer to first line on current page     */
linetype *  lastline;           /* pointer to last line on current page      */
int         firstcolumn;        /* 1st column with something to print        */

stackitem * stack;              /* stack for dvi-pushes                      */
int         sptr;               /* stack pointer                             */

font * fonts = NULL;            /* List of fontnames defined                 */
font * fnt = NULL;              /* Current font                              */

int    kanji1 = 0;     /* number of rest of trailer bytes in kanji character */



/*
 * Function definitions
 */

void            postamble       (void);
void            preamble        (void);
void            walkpages       (void);
void            initpage        (void);
void            dopage          (void);
void            skippage        (void);
void            printpage       (void);
bool            inlist          (long pagenr);
void            rule            (bool moving, long rulewt, long ruleht);
void            ruleaux         (long rulewt, long ruleht, char ch);
long            horizontalmove  (long amount);
int             skipnops        (void);
linetype    *   my_getline      (void);
linetype    *   findline        (void);
unsigned long   num             (int size);
long            snum            (int size);
void            dochar          (unsigned char ch);
void            symchar         (unsigned char ch);
void            michar          (unsigned char ch);
void            normchar        (char flag, unsigned char ch);
void            t1char          (unsigned char ch);
void            ts1char         (unsigned char ch);
void            ot2char         (unsigned char ch);
void            t2char          (char flag, unsigned char ch);
void            outchar         (long ch);
void            putcharacter    (long charnr);
void            setchar         (long charnr);
void            fontdef         (int x);
void            setfont         (long fntnum);
void            jischar         (unsigned long ch);
void            compute_jis     (int f, unsigned int c, unsigned int * ku, unsigned int * ten);
void            dounichar       (long ch);
void            dokanji         (long ch);
int             getjsubfont     (char * s);



/*
 * DVIMAIN -- The main function for processing the dvi file.
 *            Here we assume there are to file pointers: DVIfile and output.
 *            Also we have a list of pages pointed to by 'currentpage',
 *            which is only used (in 'inlist()') when a page list is given.
 */

void dvimain(void)
{

    postamble();                            /* seek and process the postamble */
    preamble();                             /* process preamble               */
    /* note that walkpages *must* immediately follow preamble */
    walkpages();                            /* time to do the actual work!    */

    return;

} /* dvimain */


 /*
  * POSTAMBLE -- Find and process postamble, use random access 
  */

void postamble(void)
{
    register long size;
    register int  count;

    fseek (DVIfile, 0L, SEEK_END);
    size = ftell (DVIfile);                     /* get size of file          */

    count = -1;
    do {              /* back file up past signature bytes (223), to id-byte */
        if (size-- == 0)
            errorexit(nopst);
        mseek(DVIfile, size, absolute);
        opcode = (int) get1();
        count++;
    } while (opcode == TRAILER);
    if (count < 4) {                            /* must have 4 trailer bytes */
         foo = count;
         errorexit(fwsgn);
    }
    if (opcode != VERSIONID)
        errorexit(badid);
    mseek(DVIfile, size-4, absolute);       /* back up to back-pointer       */
    mseek(DVIfile, sget4(), absolute);      /* and to start of postamble     */
    if (get1() != POST)
        errorexit(nopst);
    mseek(DVIfile, 20L, relative); /* lastpageoffset, numerator, denominator */
                                   /* magnification, maxpageheight           */
    maxpagewidth = sget4();
    charwidth = maxpagewidth / (ttywidth + espace);
    stackmax = (int) get2();
    if ((stack = (stackitem *) malloc(stackmax * sizeof(stackitem))) == NULL)
       errorexit(stkrq);

    /* get2() -- totalpages */
    /* fontdefs  do fontdefs in flight ... */

    return;

} /* postamble */



/*
 * PREAMBLE --process preamble, use random access
 */

void preamble(void)
{

    mseek(DVIfile, 0L, absolute);       /* read the dvifile from the start   */
    if ((opcode = (int) num(1)) != PRE)
        errorexit(nopre);
    opcode = (int) get1();        /* check id in preamble, ignore rest of it */
    if (opcode != VERSIONID)
        errorexit(badid);
    mseek(DVIfile, 12L, relative);  /* numerator, denominator, magnification */
    mseek(DVIfile, get1(), relative);         /* skip job identification     */

    return;

} /* preamble */



/*
 * WALKPAGES -- process the pages in the DVI-file
 */

void walkpages(void)
{
    register bool wantpage;

    pagecounter = 0L;
    while ((opcode = skipnops()) != POST) {

        if (opcode != BOP)              /* should be at start of page now    */
            errorexit(nobop);

        pagecounter++;
        pagenr = sget4();               /* get TeX page number               */
        mseek(DVIfile, 36L, relative);  /* skip page header */
        backpointer = sget4();          /* get previous page offset          */
        if (pageswitchon)
            wantpage = inlist(sequenceon ? pagecounter : pagenr);
        else
            wantpage = TRUE;

        if (wantpage) {
            initpage();
            dopage();
            printpage();
        }
        else
                skippage();
    }

    return;

} /* walkpages */



/*
 * INITPAGE -- Setup a new, empty page.
 */

void initpage(void)
{

    h = 0L;  v = 0L;                        /* initialize coordinates   */
    x = 0L;  w = 0L;  y = 0L;  z = 0L;      /* initialize amounts       */
    sptr = 0;                               /* initialize stack         */
    currentline = my_getline();                /* initialize list of lines */
    currentline->vv = 0L;
    firstline   = currentline;
    lastline    = currentline;
    firstcolumn = rightmargin;
    if (pageswitchon) {
        if ((sequenceon ? pagecounter : pagenr) != firstpage->pag) {
            if (noffd)
                fprintf(output, "^L\n");
            else
                putc(FORM, output);
        }
    }
    else
        if (backpointer != -1) {             /* not FORM at first page   */
            if (noffd)
                fprintf(output, "^L\n");
            else
                putc(FORM, output);
        }

    return;

} /* initpage */



/*
 * DOPAGE -- Process the dvi file until an end-off-page.
 *           Build up a page image.
 */

void dopage(void)
{

    while ((opcode = (int) get1()) != EOP) {    /* process page until eop */
        if (opcode <= LASTCHAR)
            dochar((unsigned char) opcode);
        else if ((opcode >= FONT_00) && (opcode <= FONT_63)) 
            setfont((long) opcode - FONT_00);
        else if (opcode > POST_POST)
            errorexit(illop);
        else
            switch (opcode) {
                case SET1     : nttj ? jischar(get1()) : setchar(get1());break;
                case SET2     : (asciip || uptex) ? dokanji(get2()) : setchar(get2()); break;
                case SET3     : uptex ? dokanji(get3()) : setchar(get3()); break;
                case SET4     : setchar(get4()); break;
                case SET_RULE : { long height = sget4();
                                  rule(MOVE, sget4(), height); break;
                                }
                case PUT1     : putcharacter(get1()); break;
                case PUT2     : putcharacter(get2()); break;
                case PUT3     : putcharacter(get3()); break;
                case PUT4     : putcharacter(get4()); break;
                case PUT_RULE : { long height = sget4();
                                  rule(STAY, sget4(), height); break;
                                }
                case NOP      : break;  /* no-op */
                case BOP      : errorexit(bdbop); break;
/*              case EOP      : break;  strange place to have EOP */
                case PUSH     : if (sptr >= stackmax)            /* push */
                                     errorexit(stkof);
                                stack[sptr].hh = h;
                                stack[sptr].vv = v;
                                stack[sptr].ww = w;
                                stack[sptr].xx = x;
                                stack[sptr].yy = y;
                                stack[sptr].zz = z;
                                sptr++;
                                break;
                case POP      : if (sptr-- == 0)                 /* pop */
                                    errorexit(stkuf);
                                h = stack[sptr].hh;
                                v = stack[sptr].vv;
                                w = stack[sptr].ww;
                                x = stack[sptr].xx;
                                y = stack[sptr].yy;
                                z = stack[sptr].zz;
                                break;
                case RIGHT1   : (void) horizontalmove(sget1()); break;
                case RIGHT2   : (void) horizontalmove(sget2()); break;
                case RIGHT3   : (void) horizontalmove(sget3()); break;
                case RIGHT4   : (void) horizontalmove(sget4()); break;
                case W0       : h += w; break;
                case W1       : w = horizontalmove(sget1()); break;
                case W2       : w = horizontalmove(sget2()); break;
                case W3       : w = horizontalmove(sget3()); break;
                case W4       : w = horizontalmove(sget4()); break;
                case X0       : h += x; break;
                case X1       : x = horizontalmove(sget1()); break;
                case X2       : x = horizontalmove(sget2()); break;
                case X3       : x = horizontalmove(sget3()); break;
                case X4       : x = horizontalmove(sget4()); break;
                case DOWN1    : v += sget1(); break;
                case DOWN2    : v += sget2(); break;
                case DOWN3    : v += sget3(); break;
                case DOWN4    : v += sget4(); break;
                case Y0       : v += y; break;
                case Y1       : y = sget1(); v += y; break;
                case Y2       : y = sget2(); v += y; break;
                case Y3       : y = sget3(); v += y; break;
                case Y4       : y = sget4(); v += y; break;
                case Z0       : v += z; break;
                case Z1       : z = sget1(); v += z; break;
                case Z2       : z = sget2(); v += z; break;
                case Z3       : z = sget3(); v += z; break;
                case Z4       : z = sget4(); v += z; break;
                case FNT1     :
                case FNT2     :
                case FNT3     :
                case FNT4     : setfont(num(opcode - FNT1 + 1));
                                break;
                case XXX1     : mseek(DVIfile, get1(), relative); break;
                case XXX2     : mseek(DVIfile, get2(), relative); break;
                case XXX3     : mseek(DVIfile, get3(), relative); break;
                case XXX4     : mseek(DVIfile, get4(), relative); break;
                case FNT_DEF1 :
                case FNT_DEF2 :
                case FNT_DEF3 :
                case FNT_DEF4 : fontdef(opcode - FNT_DEF1 + 1);
                                break;
                case PRE      : errorexit(bdpre); break;
                case POST     : errorexit(bdpst); break;
                case POST_POST: errorexit(bdpp); break;
            }
    }

    return;

} /* dopage */



/*
 * SKIPPAGE -- Scan the dvi file until an end-off-page.
 *             Skip this page.
 */

void skippage(void)
{
    register int opcode;

    while ((opcode = (int) get1()) != EOP) {
        if (opcode > POST_POST)
            errorexit(illop);
        else
            switch (opcode) {
                case SET1     :
                case PUT1     :
                case RIGHT1   :
                case W1       :
                case X1       :
                case DOWN1    :
                case Y1       :
                case Z1       : /* assume FNT change can also be skipped */
                case FNT1     : mseek(DVIfile, 1L, relative); break;
                case SET2     :
                case PUT2     :
                case RIGHT2   :
                case W2       :
                case X2       :
                case DOWN2    :
                case Y2       :
                case Z2       :
                case FNT2     : mseek(DVIfile, 2L, relative); break;
                case SET3     :
                case PUT3     :
                case RIGHT3   :
                case W3       :
                case X3       :
                case DOWN3    :
                case Y3       :
                case Z3       :
                case FNT3     : mseek(DVIfile, 3L, relative); break;
                case SET4     :
                case PUT4     :
                case RIGHT4   :
                case W4       :
                case X4       :
                case DOWN4    :
                case Y4       :
                case Z4       :
                case FNT4     : mseek(DVIfile, 4L, relative); break;
                case SET_RULE :
                case PUT_RULE : mseek(DVIfile, 8L, relative); break;
                case BOP      : errorexit(bdbop); break;
                case XXX1     : mseek(DVIfile, get1(), relative); break;
                case XXX2     : mseek(DVIfile, get2(), relative); break;
                case XXX3     : mseek(DVIfile, get3(), relative); break;
                case XXX4     : mseek(DVIfile, get4(), relative); break;
                case FNT_DEF1 :
                case FNT_DEF2 :
                case FNT_DEF3 :
                case FNT_DEF4 : fontdef(opcode - FNT_DEF1 + 1); break;
                case PRE      : errorexit(bdpre); break;
                case POST     : errorexit(bdpst); break;
                case POST_POST: errorexit(bdpp); break;
        }
    }

    return;

} /* skippage */



/*
 * PRINTPAGE -- 'end of page', writes lines of page to output file
 */

void printpage(void)
{
    register int  i, j, k;
    register long ch, mbch;
    unsigned char buff[4];

    if (sptr != 0)
        fprintf(stderr, "dvi2tty: warning - stack not empty at eop.\n");
    for (currentline = firstline; currentline != nil;
          currentline = currentline->next) {
        if (currentline != firstline) {
            foo = ((currentline->vv - currentline->prev->vv)/lineheight)-1;
            if (foo > 3)
                foo = 3;            /* linespacings not too large */
            for (i = 1; i <= (int) foo; i++)
                putc('\n', output);
        }
        if (currentline->charactercount >= leftmargin) {
            foo = ttywidth - 2;
            for (i = firstcolumn, j = 1; i <= currentline->charactercount;
                   i++, j++) {
                ch = currentline->text[i - leftmargin];

		if (japan && !(ch & IS_UNICODE)) {
		  if (ch > 127) {
		    for (k = 0; k < 4; k++) {
		      if (i - leftmargin + k < LINELEN+1)
			buff[k] = currentline->text[i - leftmargin + k];
		      else buff[k] = '\0';
		    }
		    kanji1 = multistrlen(buff, 4, 0) - 1;
		  }
		  else kanji1 = 0;
		  if (kanji1 && (j + kanji1 > (int) foo) &&
		      (currentline->charactercount > i+1)) {
		    putc2('*', output);
		    putc2('\n', output);    /* if line to large */
		    putc2(' ', output);
		    putc2('*', output);     /* mark output      */
		    j = 2;
		  }
		}

                if (ch >= SPACE || allchar) {
		  if (utf8 && (ch & IS_UNICODE)) {
#ifdef WIN32
		    wchar_t wch;
		    HANDLE hStdout;
		    DWORD ret;
		    const int fd = fileno(output);

		    if ((fd == fileno(stdout) || fd == fileno(stderr)) && _isatty(fd)) {
		      if (fd == fileno(stdout))
			hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
		      else
			hStdout = GetStdHandle(STD_ERROR_HANDLE);

		      wch=(wchar_t)(ch & MAX_UNICODE); /* do not expect over BMP */
		      WriteConsoleW(hStdout, &wch, 1, &ret, NULL);

		    } else {
#endif
		      mbch = UCStoUTF8(ch & MAX_UNICODE);
		      if (BYTE1(mbch) != 0) putc((unsigned char)BYTE1(mbch), output);
		      if (BYTE2(mbch) != 0) putc((unsigned char)BYTE2(mbch), output);
		      if (BYTE3(mbch) != 0) putc((unsigned char)BYTE3(mbch), output);
		      /* always */          putc((unsigned char)BYTE4(mbch), output);
#ifdef WIN32
		    }
#endif
		  }
		  else if (japan) {
		    for (k = 0; k < kanji1; k++) {
		      putc2(ch, output);
		      i++; j++;
		      ch = currentline->text[i - leftmargin];
		    }
		    putc2(ch, output);
		  }
		  else
		    putc(ch, output);
		}
                if ((j > (int) foo) && (currentline->charactercount > i+1)) {
		  if (japan) {
		    putc2('*', output);
		    putc2('\n', output);    /* if line to large */
		    putc2(' ', output);
		    putc2('*', output);     /* mark output      */
		  }
		  else {
		    fprintf(output, "*\n");         /* if line to large */
		    fprintf(output, " *");          /* mark output      */
		  }
		  j = 2;
                }
            }
        }
        if (japan)
          putc2('\n', output);
        else
          putc('\n', output);
    }

    currentline = firstline;
    while (currentline->next != nil) {
        currentline = currentline->next;
        free(currentline->prev);
    }
    free(currentline);              /* free last line */
    currentline = nil;

    return;

} /* printpage */



/*
 * INLIST -- return true if pagenr is in the list of pages to be printed.
 */

bool inlist(long pagenr)
{

    while ((currentpage->pag < 0) && (currentpage->pag != pagenr) &&
           !currentpage->all && (currentpage->nxt != nil))
        currentpage = currentpage->nxt;
    if ((currentpage->all && (pagenr < currentpage->pag)) ||
         (currentpage->pag == pagenr))
            return TRUE;
    else if (pagenr > 0) {
        while ((currentpage->pag < pagenr) && (currentpage->nxt != nil))
            currentpage = currentpage->nxt;
        if (currentpage->pag == pagenr)
            return TRUE;
    }

    return FALSE;

} /* inlist */



/*
 * RULE -- Output a rule (vertical or horizontal).
 *         Increment h if moving is true.
 */

void rule(bool moving, long rulewt, long ruleht)
{

    register char ch;               /* character to set rule with            */
    register long saveh = 0, savev;
                              /* rule   --   starts up the recursive routine */
    if (!moving)
        saveh = h;
    if ((ruleht <= 0) || (rulewt <= 0))
        h += rulewt;
    else {
        savev = v;
        if ((ruleht / rulewt) > 0)         /* value < 1 truncates to 0 */
            ch = '|';
        else if (ruleht > (lineheight / 2))
            ch = '=';
        else
            ch = '_';
        ruleaux(rulewt, ruleht, ch);
        v = savev;
    }
    if (!moving)
        h = saveh;

    return;

} /* rule */



/*
 * RULEAUX -- do the actual output for the rule recursively.
 */

void ruleaux(long rulewt, long ruleht, char ch)
{
    register long wt, lmh, rmh;

    wt = rulewt;
    lmh = h;                        /* save left margin                      */
    if (h < 0) {                    /* let rules that start at negative h    */
        wt -= h;                    /* start at coordinate 0, but let it     */
        h = 0;                      /*   have the right length               */
    }
    while (wt > 0) {                /* output the part of the rule that      */
        rmh = h;                    /*   goes on this line                   */
        outchar(ch);
        wt -= (h-rmh);              /* decrease the width left on line       */
    }
    ruleht -= lineheight;      /* decrease the height                   */
    if (ruleht > lineheight) { /* still more vertical?                  */
        rmh = h;                    /* save current h (right margin)         */
        h = lmh;                    /* restore left margin                   */
        v -= (lineheight + lineheight / 10);
        ruleaux(rulewt, ruleht, ch);
        h = rmh;                    /* restore right margin                  */
    }

    return;

} /* ruleaux */



/*
 * HORIZONTALMOVE -- Move the h pointer by amount.
 */

long horizontalmove(long amount)
{

    if (labs(amount) > charwidth / 4L) {
        foo = 3*charwidth / 4;
        if (amount > 0)
            amount = ((amount+foo) / charwidth) * charwidth;
        else
            amount = ((amount-foo) / charwidth) * charwidth;
        h += amount;
        return amount;
    }
    else
        return 0;

}   /* horizontalmove */



/*
 * SKIPNOPS -- Return first non NOP opcode.
 */

int skipnops(void)
{
    register int opcode;

    while (opcode = (int) num(1)) {
        if (opcode >= FNT_DEF1 && opcode <= FNT_DEF4) {
            fontdef(opcode - FNT_DEF1 + 1);
            continue;
        }
        else if (opcode == NOP)
            continue;
        break;
    }
    return opcode;

} /* skipnops */



/*
 * GETLINE -- Returns an initialized line-object 
 */

linetype *my_getline(void)
{
    register int  i;
    register linetype *temp;

    if ((temp = (linetype *) malloc(sizeof(linetype))) == NULL) 
        errorexit(lnerq);
    temp->charactercount = leftmargin - 1;
    temp->prev = nil;
    temp->next = nil;
    for (i = 0; i < LINELEN; i++)
        temp->text[i] = ' ';
    temp->text[i] = '\0';

    return temp;

} /* my_getline */



/*
 * FINDLINE -- Find best fit line were text should go
 *             and generate new line if needed.
 */

linetype *findline(void)
{
    register linetype *temp;
    register long topd, botd;

    if (v <= firstline->vv) {                      /* above first line */
        if (firstline->vv - v > lineheight) {
            temp = my_getline();
            temp->next = firstline;
            firstline->prev = temp;
            temp->vv = v;
            firstline = temp;
        }
        return firstline;
    }

    if (v >= lastline->vv) {                       /* below last line */
        if (v - lastline->vv > lineheight) {
            temp = my_getline();
            temp->prev = lastline;
            lastline->next = temp;
            temp->vv = v;
            lastline = temp;
        }
        return lastline;
    }

    temp = lastline;                               /* in between two lines */
    while ((temp->vv > v) && (temp != firstline))
        temp = temp->prev;

    /* temp->vv < v < temp->next->vv --- temp is above, temp->next is below */
    topd = v - temp->vv;
    botd = temp->next->vv - v;
    if ((topd < lineheight) || (botd < lineheight)) {
        if (topd < botd)                           /* take best fit */
            return temp;
        else
            return temp->next;
    }

    /* no line fits suitable, generate a new one */
    currentline = my_getline();
    currentline->next = temp->next;
    currentline->prev = temp;
    temp->next->prev = currentline;
    temp->next = currentline;
    currentline->vv = v;

    return currentline;

} /* findline */



/*
 * NUM --
 */

unsigned long num(int size)
{
    register int i;
    register unsigned long x = 0;

    for (i = size; i > 0; i--)
        x = (x << 8) + (unsigned) getc(DVIfile);

    return x;

} /* num */


/*
 * SNUM --
 */

long snum(int size)
{
    register int i;
    register long x;

    x = getc(DVIfile);
    if (x & 0x80)
        x -= 0x100;
    for (i = size - 1; i > 0; i--)
        x = (x << 8) + (unsigned) getc(DVIfile);

    return x;

} /* snum */



/*
 * DOUNICHAR -- Process a Unicode character
 */

void dounichar(long ch)
{
    unsigned char c[4] = {0}, *cc;

    if (noligaturefi && 0xFB00<=ch && ch<=0xFB04) {
        switch (ch) {
            case 0xFB00: strcpy((char*)c,"ff");  break;
            case 0xFB01: strcpy((char*)c,"fi");  break;
            case 0xFB02: strcpy((char*)c,"fl");  break;
            case 0xFB03: strcpy((char*)c,"ffi"); break;
            case 0xFB04: strcpy((char*)c,"ffl"); break;
        }
        cc=c;
        while (*cc) { outchar((long)*cc); cc++; }
        return;
    }
    if (ch>0x7F)
        outchar((long)(ch | IS_UNICODE));
    else {
        outchar((long)ch);
    }

    return;

} /* dounichar */


/*
 * DOKANJI -- Process a kanji character opcode.
 */

void dokanji(long ch)
{
    long i;
    i = toBUFF(fromDVI(ch));

    kanji1 = 3;
    if (BYTE1(i) != 0) outchar((long)BYTE1(i));
    kanji1 = 2;
    if (BYTE2(i) != 0) outchar((long)BYTE2(i));
    kanji1 = 1;
    if (BYTE3(i) != 0) outchar((long)BYTE3(i));
    kanji1 = 0;
    /* always */       outchar((long)BYTE4(i));

    return;

} /* dokanji */



/*
 * DOCHAR -- Process a character opcode.
 */

void dochar(unsigned char ch)
{
    char flag;
    if (!fnt) errorexit(bdfnt);
    flag = fnt->flags;

    if (nttj && fnt->fontnum)
        jischar((long) ch);
    else if (symbolfont)
        symchar(ch);
    else if (mifont)
        michar(ch);
    else if (flag == T1FONT)
        t1char(ch);
    else if (flag == TS1FONT)
        ts1char(ch);
    else if (flag == OT2FONT)
        ot2char(ch);
    else if (flag == T2AFONT || flag == T2BFONT ||
             flag == T2CFONT || flag == X2FONT)
        t2char(flag, ch);
    else
        normchar(flag, ch);

    return;

} /* dochar */



/*
 * SYMCHAR -- Process a character opcode for a symbol font.
 */

void symchar(unsigned char ch)
{
    unsigned char c[4] = {0}, *cc;
    long ucs;

    ucs = oms_to_ucs[ch];
    if (utf8) {
        dounichar(ucs);
        return;
    }
    else if ((latin1 && ucs<0x100) || ucs<0x80) {
        outchar(ucs);
        return;
    }

    switch (ch) {       /* can do a lot more on MSDOS/latin1/unicode machines ... */
       case   0: c[0] = '-'; break;
       case   1: c[0] = '.'; break;
       case   2: c[0] = 'x'; break;
       case   3: c[0] = '*'; break;
       case   4: c[0] = '/'; break;
       case   6: c[0] = '+'; c[1] = '-'; break;
       case   7: c[0] = '-'; c[1] = '+'; break;
       case  13: c[0] = 'O'; break;
       case  14: c[0] = 'O'; break;
       case  15: c[0] = 'o'; break;
       case  24: c[0] = '~'; break;
       case  28: c[0] = '<'; c[1] = '<'; break;
       case  29: c[0] = '>'; c[1] = '>'; break;
       case  32: c[0] = '<'; c[1] = '-'; break;
       case  33: c[0] = '-'; c[1] = '>'; break;
       case  34: c[0] = '^'; break;
       case  35: c[0] = 'v'; break;
       case  36: c[0] = '<'; c[1] = '-'; c[2] = '>'; break;
       case  40: c[0] = '<'; c[1] = '='; break;
       case  41: c[0] = '='; c[1] = '>'; break;
       case  42: c[0] = '^'; break;
       case  43: c[0] = 'v'; break;
       case  44: c[0] = '<'; c[1] = '='; c[2] = '>'; break;
       case  60: c[0] = 'R'; c[1] = 'e'; break;
       case  61: c[0] = 'I'; c[1] = 'm'; break;
       case 102: c[0] = '{'; break;
       case 103: c[0] = '}'; break;
       case 104: c[0] = '<'; break;
       case 105: c[0] = '>'; break;
       case 106: c[0] = '|'; break;
       case 107: c[0] = '|'; c[1] = '|'; break;
       case 110: c[0] = '\\'; break;
       case 120: c[0] = 'S'; break;
       case 121: c[0] = '*'; break;
       case 122: c[0] = '*'; c[1] = '*'; break;
       case 123: c[0] = 'P'; break;

       default: c[0] = '#';
    }

    cc=c;
    while (*cc) { outchar(*cc); cc++; }

    return;

} /* symchar */



/*
 * MICHAR -- Process a character opcode for OML font.
 */

void michar(unsigned char ch)
{
    unsigned char c[4] = {0}, *cc;
    long ucs;

    if (allchar) {
        outchar(ch);
        return;
    }
    ucs = oml_to_ucs[ch];
    if (utf8) {
        dounichar(ucs);
        return;
    }
    else if ((latin1 && ucs<0x100) || ucs<0x80) {
        outchar(ucs);
        return;
    }

    switch (ch) {
        case 0x3a:  c[0] = '.'; break;              /* .               */
        case 0x3b:  c[0] = ','; break;              /* ,               */
        case 0x3d:  c[0] = '/'; break;              /* /               */
        case 0x3e:  c[0] = '*'; break;              /* \star           */
        case 0x40:  c[0] = 'd'; break;              /* \partial        */
        case 0x60:  c[0] = 'l'; break;              /* \ell            */
        case 0x7b:  c[0] = 'i'; break;              /* dotless i       */
        case 0x7c:  c[0] = 'j'; break;              /* dotless j       */
        case 0x7d:  c[0] = 'P'; break;              /* \wp             */

        default  :  c[0] = '#';
    }

    cc=c;
    while (*cc) { outchar(*cc); cc++; }

    return;

} /* michar */


/*
 * NORMCHAR -- Process a character opcode for a normal font.
 */

void normchar(char flag, unsigned char ch)
{
    unsigned char c[4] = {0}, *cc;
    const unsigned short *tex_to_ucs;
    long ucs;

    if (allchar) {
        outchar(ch);
        return;
    }
    if (!accent) {
        switch (ch) {
            case 18  :  /* grave        from \` */
            case 19  :  /* acute        from \' */
            case 20  :  /* caron        from \v */
            case 21  :  /* breve        from \u */
            case 22  :  /* macron       from \= */
            case 23  :  /* ring above   from \r */
            case 24  :  /* cedilla      from \c */
            case 32  :  /* stroke    i.e. \L,\l */
            case 94  :  /* circumflex   from \^ */
            case 126 :  /* tilde        from \~ */
            case 127 :  /* diaeresis    from \" */
                return;
            case 125 :  /* double acute from \H */
            case 95  :  /* dot          from \. */
                if (!ttfont) return;
        }
    }
    switch (flag) {
        case TTFONT : tex_to_ucs=tt_to_ucs;  break;
        default :     tex_to_ucs=ot1_to_ucs;
    }
    ucs = tex_to_ucs[ch];
    if (utf8) {
        dounichar(ucs);
        return;
    }
    else if ((latin1 && ucs<0x100) || ucs<0x80) {
        outchar(ucs);
        return;
    }

    switch (ch) {
        case 11  :  if (ttfont)
                        c[0] = '^';                 /* up symbol       */
                    else {
                        c[0] = 'f'; c[1] = 'f';     /* ligature        */
                    }
                    break;
        case 12  :  if (ttfont)
                        c[0] = 'v';                 /* low symbol       */
                    else {
                        c[0] = 'f'; c[1] = 'i';     /* ligature        */
                    }
                    break;
        case 13  :  if (ttfont)
                        c[0] = '`';
                    else {
                        c[0] = 'f'; c[1] = 'l';     /* ligature        */
                    }
                    break;
        case 14  :  if (ttfont)
                        c[0] = 'i';                 /* spanish !        */
                    else {
                        c[0] = 'f'; c[1] = 'f';
                                  c[2] = 'i';       /* ligature        */
                    }
                    break;
        case 15  :  if (ttfont)
                        c[0] = '.';                 /* spanish ?        */
                    else {
                        c[0] = 'f'; c[1] = 'f';
                                  c[2] = 'l';       /* ligature        */
                    }
                    break;
        case 16  :  c[0] = 'i'; break;
        case 17  :  c[0] = 'j'; break;
        case 25  :  if (latin1)
                        c[0] = 0xdf;
                    else {
                        c[0] = 's'; c[1] = 's';
                    }
                    break;  /* German double s */
        case 26  :  if (latin1)
                        c[0] = 0xe6;
                    else {
                        c[0] = 'a'; c[1] = 'e';
                    }
                    break;  /* Dane/Norw ae    */
        case 27  :  c[0] = 'o'; c[1] = 'e';
                    break;  /* Dane/Norw oe    */
        case 28  :  if (scascii)
                        c[0] = '|';
                    else if (latin1)
                        c[0] = 0xf8;
                    else
                        c[0] = 'o';
                    break; /* Dane/Norw /o    */
        case 29  :  if (latin1)
                        c[0] = 0xc6;
                    else {
                        c[0] = 'A'; c[1] = 'E';
                    }
                    break;  /* Dane/Norw AE    */
        case 30  :  c[0] = 'O'; c[1] = 'E';
                    break;  /* Dane/Norw OE    */
        case 31  :  if (scascii)
                        c[0] = '\\';
                    else if (latin1)
                        c[0] = 0xd8;
                    else
                        c[0] = 'O';
                    break; /* Dane/Norw /O    */
        case 60  :  if (ttfont)
                        c[0] = ch;   /* '>' */
                    else if (latin1)
                        c[0] = 0xa1;
                    else
                        c[0] = '!';
                    break; /* inverted !    */
        case 62  :  if (ttfont)
                        c[0] = ch;   /* '<' */
                    else if (latin1)
                        c[0] = 0xbf;
                    else
                        c[0] = '?';
                    break; /* inverted ?    */
        case 32  :  c[0] = ttfont ? ch : '_'; break;  /* underlined blank */
        case 92  :  c[0] = ttfont ? ch : '"'; break;  /* \ from `` */
        case 123 :  if (ttfont)
                        c[0] = ch;                    /* {         */
                    else {
                        c[0] = '-'; c[1] = '-';       /* --        */
                    }
                    break;
        case 124 :  if (ttfont)
                        c[0] = ch;                    /* |         */
                    else {
                        c[0] = '-'; c[1] = '-';       /* ---       */
                        c[2] = '-';
                    }
                    break;
        case 125 :  if (ttfont)
                        c[0] = ch;                    /* }         */
                    else
                        c[0] = '"';        /* double acute from \H */
                    break;
        case 34  :                                    /* " */
        case 39  :                                    /* ' */
        case 96  :  c[0] = ch; break;                 /* ` */

        /* diacritical marks */
        case 18  :  c[0] = '`'  ; break;   /* grave        from \` */
        case 19  :  c[0] = latin1 ? 0xb4 : '\''; break;
                                           /* acute        from \' */
        case 20  :  c[0] = '~'  ; break;   /* caron        from \v */
        case 21  :  c[0] = '~'  ; break;   /* breve        from \u */
        case 22  :  c[0] = '~'  ; break;   /* macron       from \= */
        case 23  :  c[0] = latin1 ? 0xb0 : '~'; break;
                                           /* ring above   from \r */
        case 24  :  c[0] = latin1 ? 0xb8 : ','; break;
                                           /* cedilla      from \c */
        case 94  :  c[0] = '^'  ; break;   /* circumflex   from \^ */
        case 95  :  c[0] = !ttfont ? '.' : ch; break;
                                           /* dot          from \. */
        case 126 :  c[0] = '~'  ; break;   /* tilde        from \~ */
        case 127 :  c[0] = '"'  ; break;   /* diaeresis    from \" */

        default  :  c[0] = '#';
    }

    cc=c;
    while (*cc) { outchar(*cc); cc++; }

    return;

} /* normchar */


/*
 * T1CHAR -- Process a character opcode for a T1 encoding font.
 */

void t1char(unsigned char ch)
{
    unsigned char c[4] = {0}, *cc;
    long ucs;

    if (allchar) {
        outchar(ch);
        return;
    }
    if (!accent) {
        switch (ch) {
            case 0x00:  /* grave        from \` */
            case 0x01:  /* acute        from \' */
            case 0x02:  /* circumflex   from \^ */
            case 0x03:  /* tilde        from \~ */
            case 0x04:  /* diaeresis    from \" */
            case 0x05:  /* double acute from \H */
            case 0x06:  /* ring above   from \r */
            case 0x07:  /* caron        from \v */
            case 0x08:  /* breve        from \u */
            case 0x09:  /* macron       from \= */
            case 0x0a:  /* dot          from \. */
            case 0x0b:  /* cedilla      from \c */
            case 0x0c:  /* ogonek       from \k */
                return;
        }
    }
    if (ch==0xdf) {
        outchar('S'); outchar('S');                 /* SS              */
        return;
    }
    ucs = t1_to_ucs[ch];
    if (utf8) {
        dounichar(ucs);
        return;
    }
    else if ((latin1 && ucs<0x100) || ucs<0x80) {
        outchar(ucs);
        return;
    }

    switch (ch) {
        case 0x17:  return;                      /* \textcompwordmark  */
        case 0x0d:                               /* \quotesinglbase    */
        case 0x27:                               /* \textquoteright    */
        case 0x60:  c[0] = '\''; break;          /* \textquoteleft     */
        case 0x10:                               /* \textquotedblleft  */
        case 0x11:                               /* \textquotedblright */
        case 0x12:  c[0] = '"'; break;           /* \quotedblbase      */
        case 0x0e:  c[0] = '<'; break;           /* \guilsinglleft     */
        case 0x0f:  c[0] = '>'; break;           /* \guilsinglright    */
        case 0x13:  c[0] = '<'; c[1] = '<';      /* \guillemotleft     */
                    break;
        case 0x14:  c[0] = '>'; c[1] = '>';      /* \guillemotright    */
                    break;
        case 0x15:  c[0] = '-'; c[1] = '-';      /* \textendash        */
                    break;
        case 0x16:  c[0] = '-'; c[1] = '-';      /* \textemdash        */
                    c[2] = '-'; break;
        case 0x20:  c[0] = '_'; break;           /* \textvisiblespace  */
        case 0x7f:  c[0] = '-'; break;              /* -               */
        case 0x19:  c[0] = 'i'; break;              /* dotless i       */
        case 0x1a:  c[0] = 'j'; break;              /* dotless j       */
        case 0x1b:  c[0] = 'f'; c[1] = 'f';         /* ligature        */
                    break;
        case 0x1c:  c[0] = 'f'; c[1] = 'i';         /* ligature        */
                    break;
        case 0x1d:  c[0] = 'f'; c[1] = 'l';         /* ligature        */
                    break;
        case 0x1e:  c[0] = 'f'; c[1] = 'f';
                    c[2] = 'i';                     /* ligature        */
                    break;
        case 0x1f:  c[0] = 'f'; c[1] = 'f';
                    c[2] = 'l';                     /* ligature        */
                    break;
        case 0xff:  c[0] = 's'; c[1] = 's';
                    break;  /* German double s */
        case 0xe6:  c[0] = 'a'; c[1] = 'e';
                    break;  /* Dane/Norw ae    */
        case 0xf7:  c[0] = 'o'; c[1] = 'e';
                    break;  /* Dane/Norw oe    */
        case 0xf8:  c[0] = '/'; c[1] = 'o';
                    break;  /* Dane/Norw /o    */
        case 0xc6:  c[0] = 'A'; c[1] = 'E';
                    break;  /* Dane/Norw AE    */
        case 0xd7:  c[0] = 'O'; c[1] = 'E';
                    break;  /* Dane/Norw OE    */
        case 0xd8:  c[0] = '/'; c[1] = 'O';
                    break;  /* Dane/Norw /O    */
        case 0x9c:  c[0] = 'I'; c[1] = 'J';
                    break;  /* IJ              */
        case 0xbc:  c[0] = 'i'; c[1] = 'j';
                    break;  /* ij              */
        case 0x8d:  c[0] = 'N'; c[1] = 'G';
                    break;  /* ENG             */
        case 0xad:  c[0] = 'n'; c[1] = 'g';
                    break;  /* eng             */
        case 0xde:  c[0] = 'T'; c[1] = 'H';
                    break;  /* THORN           */
        case 0xfe:  c[0] = 't'; c[1] = 'h';
                    break;  /* thorn           */
        case 0x80:  c[0] = '~';  c[1] ='A'; break;   /* uA */
        case 0x81:  c[0] = ',';  c[1] ='A'; break;   /* ,A */
        case 0x82:  c[0] = '\''; c[1] ='C'; break;   /* 'C */
        case 0x83:  c[0] = '~';  c[1] ='C'; break;   /* vC */
        case 0x84:  c[0] = '~';  c[1] ='D'; break;   /* vD */
        case 0x85:  c[0] = '~';  c[1] ='E'; break;   /* vE */
        case 0x86:  c[0] = ',';  c[1] ='E'; break;   /* ,E */
        case 0x87:  c[0] = '~';  c[1] ='G'; break;   /* uG */
        case 0x88:  c[0] = '\''; c[1] ='L'; break;   /* 'L */
        case 0x89:  c[0] = '\''; c[1] ='L'; break;   /* 'L */
        case 0x8a:  c[0] = '-';  c[1] ='L'; break;   /* -L */
        case 0x8b:  c[0] = '\''; c[1] ='N'; break;   /* 'N */
        case 0x8c:  c[0] = '~';  c[1] ='N'; break;   /* vN */
        case 0x8e:  c[0] = '"';  c[1] ='O'; break;   /* "O */
        case 0x8f:  c[0] = '\''; c[1] ='R'; break;   /* 'R */
        case 0x90:  c[0] = '~';  c[1] ='R'; break;   /* vR */
        case 0x91:  c[0] = '\''; c[1] ='S'; break;   /* 'S */
        case 0x92:  c[0] = '~';  c[1] ='S'; break;   /* vS */
        case 0x93:  c[0] = ',';  c[1] ='S'; break;   /* ,S */
        case 0x94:  c[0] = '~';  c[1] ='T'; break;   /* vT */
        case 0x95:  c[0] = ',';  c[1] ='T'; break;   /* ,T */
        case 0x96:  c[0] = '"';  c[1] ='U'; break;   /* "U */
        case 0x97:  c[0] = '\''; c[1] ='U'; break;   /* oU */
        case 0x98:  c[0] = '"';  c[1] ='Y'; break;   /* "Y */
        case 0x99:  c[0] = '\''; c[1] ='Z'; break;   /* 'Z */
        case 0x9a:  c[0] = '~';  c[1] ='Z'; break;   /* vZ */
        case 0x9b:  c[0] = '\''; c[1] ='Z'; break;   /* .Z */
        case 0x9d:  c[0] = '\''; c[1] ='I'; break;   /* .I */
        case 0x9e:  c[0] = '-';  c[1] ='d'; break;   /* -d */
        case 0x9f:  c[0] = 'S';  break;   /* section sign */

        case 0xa0:  c[0] = '~';  c[1] ='a'; break;   /* ua */
        case 0xa1:  c[0] = ',';  c[1] ='a'; break;   /* ,a */
        case 0xa2:  c[0] = '\''; c[1] ='c'; break;   /* 'c */
        case 0xa3:  c[0] = '~';  c[1] ='c'; break;   /* vc */
        case 0xa4:  c[0] = '\''; c[1] ='d'; break;   /* 'd */
        case 0xa5:  c[0] = '~';  c[1] ='e'; break;   /* ve */
        case 0xa6:  c[0] = ',';  c[1] ='e'; break;   /* ,e */
        case 0xa7:  c[0] = '~';  c[1] ='g'; break;   /* ug */
        case 0xa8:  c[0] = '\''; c[1] ='l'; break;   /* 'l */
        case 0xa9:  c[0] = '\''; c[1] ='l'; break;   /* 'l */
        case 0xaa:  c[0] = '-';  c[1] ='l'; break;   /* -l */
        case 0xab:  c[0] = '\''; c[1] ='n'; break;   /* 'n */
        case 0xac:  c[0] = '~';  c[1] ='n'; break;   /* vn */
        case 0xae:  c[0] = '"';  c[1] ='o'; break;   /* "o */
        case 0xaf:  c[0] = '\''; c[1] ='r'; break;   /* 'r */

        case 0xb0:  c[0] = '~';  c[1] ='r'; break;   /* vr */
        case 0xb1:  c[0] = '\''; c[1] ='s'; break;   /* 's */
        case 0xb2:  c[0] = '~';  c[1] ='s'; break;   /* vs */
        case 0xb3:  c[0] = ',';  c[1] ='s'; break;   /* ,s */
        case 0xb4:  c[0] = '\''; c[1] ='t'; break;   /* 't */
        case 0xb5:  c[0] = ',';  c[1] ='t'; break;   /* ,t */
        case 0xb6:  c[0] = '"';  c[1] ='u'; break;   /* "u */
        case 0xb7:  c[0] = '\''; c[1] ='u'; break;   /* ou */
        case 0xb8:  c[0] = '"';  c[1] ='y'; break;   /* "y */
        case 0xb9:  c[0] = '\''; c[1] ='z'; break;   /* 'z */
        case 0xba:  c[0] = '~';  c[1] ='z'; break;   /* vz */
        case 0xbb:  c[0] = '\''; c[1] ='z'; break;   /* .z */

        case 0xbd:  c[0] = '!';  break;   /* inversed ! */
        case 0xbe:  c[0] = '?';  break;   /* inversed ? */
        case 0xbf:  c[0] = 'L';  break;   /* pound sign */

        case 0xc0:  c[0] = '`';  c[1] ='A'; break;   /* `A */
        case 0xc1:  c[0] = '\''; c[1] ='A'; break;   /* 'A */
        case 0xc2:  c[0] = '^';  c[1] ='A'; break;   /* ^A */
        case 0xc3:  c[0] = '~';  c[1] ='A'; break;   /* ~A */
        case 0xc4:  c[0] = '"';  c[1] ='A'; break;   /* "A */
        case 0xc5:  c[0] = 'A';  c[1] ='A'; break;   /* oA */
        case 0xc7:  c[0] = ',';  c[1] ='C'; break;   /* ,C */
        case 0xc8:  c[0] = '`';  c[1] ='E'; break;   /* `E */
        case 0xc9:  c[0] = '\''; c[1] ='E'; break;   /* 'E */
        case 0xca:  c[0] = '^';  c[1] ='E'; break;   /* ^E */
        case 0xcb:  c[0] = '^';  c[1] ='E'; break;   /* "E */
        case 0xcc:  c[0] = '`';  c[1] ='I'; break;   /* `I */
        case 0xcd:  c[0] = '\''; c[1] ='I'; break;   /* 'I */
        case 0xce:  c[0] = '^';  c[1] ='I'; break;   /* ^I */
        case 0xcf:  c[0] = '"';  c[1] ='I'; break;   /* "I */
        case 0xd0:  c[0] = '-';  c[1] ='D'; break;   /* -D */
        case 0xd1:  c[0] = '~';  c[1] ='n'; break;   /* ~n */
        case 0xd2:  c[0] = '`';  c[1] ='O'; break;   /* `O */
        case 0xd3:  c[0] = '\''; c[1] ='O'; break;   /* 'O */
        case 0xd4:  c[0] = '^';  c[1] ='O'; break;   /* ^O */
        case 0xd5:  c[0] = '~';  c[1] ='O'; break;   /* ~O */
        case 0xd6:  c[0] = '"';  c[1] ='O'; break;   /* "O */
        case 0xd9:  c[0] = '`';  c[1] ='U'; break;   /* `U */
        case 0xda:  c[0] = '\''; c[1] ='U'; break;   /* 'U */
        case 0xdb:  c[0] = '^';  c[1] ='U'; break;   /* ^U */
        case 0xdc:  c[0] = '"';  c[1] ='U'; break;   /* "U */
        case 0xdd:  c[0] = '\''; c[1] ='Y'; break;   /* 'Y */
        case 0xe0:  c[0] = '`';  c[1] ='a'; break;   /* `a */
        case 0xe1:  c[0] = '\''; c[1] ='a'; break;   /* 'a */
        case 0xe2:  c[0] = '^';  c[1] ='a'; break;   /* ^a */
        case 0xe3:  c[0] = '~';  c[1] ='a'; break;   /* ~a */
        case 0xe4:  c[0] = '"';  c[1] ='a'; break;   /* "a */
        case 0xe5:  c[0] = 'a';  c[1] ='a'; break;   /* oa */
        case 0xe7:  c[0] = ',';  c[1] ='c'; break;   /* ,c */
        case 0xe8:  c[0] = '`';  c[1] ='e'; break;   /* `e */
        case 0xe9:  c[0] = '\''; c[1] ='e'; break;   /* 'e */
        case 0xea:  c[0] = '^';  c[1] ='e'; break;   /* ^e */
        case 0xeb:  c[0] = '^';  c[1] ='e'; break;   /* "e */
        case 0xec:  c[0] = '`';  c[1] ='i'; break;   /* `i */
        case 0xed:  c[0] = '\''; c[1] ='i'; break;   /* 'i */
        case 0xee:  c[0] = '^';  c[1] ='i'; break;   /* ^i */
        case 0xef:  c[0] = '"';  c[1] ='i'; break;   /* "i */
        case 0xf0:  c[0] = '-';  c[1] ='d'; break;   /* -d */
        case 0xf1:  c[0] = '~';  c[1] ='n'; break;   /* ~n */
        case 0xf2:  c[0] = '`';  c[1] ='o'; break;   /* `o */
        case 0xf3:  c[0] = '\''; c[1] ='o'; break;   /* 'o */
        case 0xf4:  c[0] = '^';  c[1] ='o'; break;   /* ^o */
        case 0xf5:  c[0] = '~';  c[1] ='o'; break;   /* ~o */
        case 0xf6:  c[0] = '"';  c[1] ='o'; break;   /* "o */
        case 0xf9:  c[0] = '`';  c[1] ='u'; break;   /* `u */
        case 0xfa:  c[0] = '\''; c[1] ='u'; break;   /* 'u */
        case 0xfb:  c[0] = '^';  c[1] ='u'; break;   /* ^u */
        case 0xfc:  c[0] = '"';  c[1] ='u'; break;   /* "u */
        case 0xfd:  c[0] = '\''; c[1] ='y'; break;   /* 'y */

        /* diacritical marks */
        case 0x00:  c[0] = '`'  ; break;   /* grave        from \` */
        case 0x01:  c[0] = latin1 ? 0xb4 : '\''; break;
                                           /* acute        from \' */
        case 0x02:  c[0] = '^'  ; break;   /* circumflex   from \^ */
        case 0x03:  c[0] = '~'  ; break;   /* tilde        from \~ */
        case 0x04:  c[0] = '"'  ; break;   /* diaeresis    from \" */
        case 0x05:  c[0] = '"'  ; break;   /* double acute from \H */
        case 0x06:  c[0] = latin1 ? 0xb0 : '~'; break;
                                           /* ring above   from \r */
        case 0x07:  c[0] = '~'  ; break;   /* caron        from \v */
        case 0x08:  c[0] = '~'  ; break;   /* breve        from \u */
        case 0x09:  c[0] = '~'  ; break;   /* macron       from \= */
        case 0x0a:  c[0] = '.'  ; break;   /* dot          from \. */
        case 0x0b:  c[0] = latin1 ? 0xb8 : ','; break;
                                           /* cedilla      from \c */
        case 0x0c:  c[0] = ','  ; break;   /* ogonek       from \k */

        default  :  c[0] = '#';
    }

    cc=c;
    while (*cc) { outchar(*cc); cc++; }

    return;

} /* t1char */


/*
 * TS1CHAR -- Process a character opcode for a TS1 encoding font.
 */

void ts1char(unsigned char ch)
{
    unsigned char c[4] = {0}, *cc;
    long ucs;

    if (allchar) {
        outchar(ch);
        return;
    }
    ucs = ts1_to_ucs[ch];
    if (utf8) {
        dounichar(ucs);
        return;
    }
    else if ((latin1 && ucs<0x100) || ucs<0x80) {
        outchar(ucs);
        return;
    }

    switch (ch) {
        case 0x17:                          /* \capitalcompwordmark      */
        case 0x1F:  return;                 /* \textascendercompwordmark */
        case 0x0D:                          /* \textquotestraightbase    */
        case 0x27:  c[0] = '\''; break;     /* \textquotesingle          */
        case 0x12:  c[0] = '"'; break;      /* \textquotestraghtdblbase  */
        case 0x15:  c[0] = '-'; break;      /* \texttwelveudash          */
        case 0x16:  c[0] = '-'; c[1] = '-'; /* \textthreequartersemdash  */
                    break;
        case 0x18:  c[0] = '<'; c[1] = '-'; /* \textleftarrow            */
                    break;
        case 0x19:  c[0] = '-'; c[1] = '>'; /* \textrightarrow           */
                    break;
        case 0x2A:  c[0] = '*'; break;      /* \textasteriskcentered     */
        case 0x2D:  c[0] = '='; break;      /* \textdblhyphen            */
        case 0x2F:  c[0] = '/'; break;      /* \textfractionsolidus      */
        case 0x3C:  c[0] = '<'; break;      /* \textlangle               */
        case 0x3D:  c[0] = '-'; break;      /* \textminus                */
        case 0x3E:  c[0] = '>'; break;      /* \textrangle               */
        case 0x5B:  c[0] = '['; break;      /* \textlbrackdbl            */
        case 0x5D:  c[0] = ']'; break;      /* \textrbrackdbl            */
        case 0x5E:  c[0] = '^'; break;      /* \textuparrow              */
        case 0x5F:  c[0] = 'v'; break;      /* \textdownarrow            */
        case 0x7E:  c[0] = '~'; break;      /* \texttildelow             */
        case 0x7F:  c[0] = '='; break;      /* \textdblhyphenchar        */
        case 0x84:  c[0] = '*'; break;      /* \textdagger               */
        case 0x85:  c[0] = '*'; c[1] = '*'; /* \textdaggerdbl            */
                    break;
        case 0x86:  c[0] = '|'; c[1] = '|'; /* \textbardbl               */
                    break;
        case 0x89:  if (latin1) {
                        c[0] = 0xb0; c[1] = 'C';
                    }
                    else
                        c[0] = 'C';
                    break;                  /* \textcelsius              */
        case 0x8B:  c[0] = 'c'; break;      /* \textcent                 */
        case 0x8C:  c[0] = 'f'; break;      /* \textflorin               */
        case 0x8D:  c[0] = 'C'; break;      /* \textcentoldstyle         */
        case 0x8E:  c[0] = 'W'; break;      /* \textwon                  */
        case 0x8F:  c[0] = 'N'; break;      /* \textnaira                */
        case 0x90:  c[0] = 'G'; break;      /* \textguarani              */
        case 0x91:  c[0] = 'P'; break;      /* \textpeso                 */
        case 0x92:  c[0] = 'L'; break;      /* \textlira                 */
        case 0x93:  c[0] = 'R'; break;      /* \textrecipe               */
        case 0x94:                          /* \textinterrobang          */
        case 0x95:  c[0] = '!'; c[1] = '?'; /* \textinterrobangdown      */
                    break;
        case 0x97:  c[0] = 'T'; c[1] = 'M'; /* \texttrademark            */
                    break;
        case 0x99:  c[0] = 'P'; break;      /* \textpilcrow              */
        case 0x9B:  c[0] = 'N'; c[1] = 'o'; /* \textnumero               */
                    break;
        case 0x9F:  c[0] = 'S'; c[1] = 'M'; /* \textservicemark          */
                    break;
        case 0xA0:  c[0] = '{'; break;      /* \textlquill               */
        case 0xA1:  c[0] = '}'; break;      /* \textrquill               */
        case 0xA2:  c[0] = 'c'; break;      /* \textcent                 */
        case 0xA3:  c[0] = 'L'; break;      /* \textsterling             */
        case 0xA5:  c[0] = 'Y'; break;      /* \textyen                  */
        case 0xA6:  c[0] = '|'; break;      /* \textbrokenbar            */
        case 0xA7:  c[0] = 'S'; break;      /* \textsection              */
        case 0xA9:  c[0] = 'C'; break;      /* \textcopyright            */
        case 0xAD:  c[0] = 'P'; break;      /* \textcircledP             */
        case 0xAE:  c[0] = 'R'; break;      /* \textregistered           */
        case 0xB6:  c[0] = 'P'; break;      /* \textparagraph            */
        case 0xB1:  c[0] = '+'; c[1] = '-'; /* \textpm                   */
                    break;
        case 0xBC:  c[0] = '1'; c[1] = '/'; /* \textonequarter           */
                    c[2] = '4'; break;
        case 0xBD:  c[0] = '1'; c[1] = '/'; /* \textonehalf              */
                    c[2] = '2'; break;
        case 0xBE:  c[0] = '3'; c[1] = '/'; /* \textthreequarters        */
                    c[2] = '4'; break;
        case 0xBF:  c[0] = 'E'; break;      /* \texteuro                 */
        case 0xD6:  c[0] = 'x'; break;      /* \texttimes                */
        case 0xF6:  c[0] = '/'; break;      /* \textdiv                  */

        case 0x30:  case 0x31:  case 0x32:  case 0x33:
        case 0x34:  case 0x35:  case 0x36:  case 0x37:
        case 0x38:  case 0x39:  case 0x3A:  case 0x3B:
                    c[0] = ch; break;

        /* diacritical marks */
        case 0x00:  c[0] = '`'  ; break;   /* grave        from \` */
        case 0x01:  c[0] = latin1 ? 0xb4 : '\''; break;
                                           /* acute        from \' */
        case 0x02:  c[0] = '^'  ; break;   /* circumflex   from \^ */
        case 0x03:  c[0] = '~'  ; break;   /* tilde        from \~ */
        case 0x04:  c[0] = '"'  ; break;   /* diaeresis    from \" */
        case 0x05:  c[0] = '"'  ; break;   /* double acute from \H */
        case 0x06:  c[0] = latin1 ? 0xb0 : '~'; break;
                                           /* ring above   from \r */
        case 0x07:  c[0] = '~'  ; break;   /* caron        from \v */
        case 0x08:  c[0] = '~'  ; break;   /* breve        from \u */
        case 0x09:  c[0] = '~'  ; break;   /* macron       from \= */
        case 0x0a:  c[0] = '.'  ; break;   /* dot          from \. */
        case 0x0b:  c[0] = latin1 ? 0xb8 : ','; break;
                                           /* cedilla      from \c */
        case 0x0c:  c[0] = ','  ; break;   /* ogonek       from \k */

        default  :  c[0] = '#';
    }

    cc=c;
    while (*cc) { outchar(*cc); cc++; }

    return;

} /* ts1char */


/*
 * T2CHAR -- Process a character opcode for a T2A/T2B/T2C/X2 encoding font.
 */

void t2char(char flag, unsigned char ch)
{
    unsigned char c[4] = {0}, *cc;
    const unsigned short *tex_to_ucs;
    long ucs;

    if (allchar) {
        outchar(ch);
        return;
    }
    if (!accent) {
        switch (ch) {
            case 0x00:  /* grave        from \` */
            case 0x01:  /* acute        from \' */
            case 0x02:  /* circumflex   from \^ */
            case 0x03:  /* tilde        from \~ */
            case 0x04:  /* diaeresis    from \" */
            case 0x05:  /* double acute from \H */
            case 0x06:  /* ring above   from \r */
            case 0x07:  /* caron        from \v */
            case 0x08:  /* breve        from \u */
            case 0x09:  /* macron       from \= */
            case 0x0a:  /* dot          from \. */
            case 0x0b:  /* cedilla      from \c */
            case 0x0c:  /* ogonek       from \k */
            case 0x12:  /*              from \f */
            case 0x13:  /*              from \C */
            case 0x14:  /* breve        from \U */
                return;
        }
    }
    switch (flag) {
        case T2AFONT: tex_to_ucs=t2a_to_ucs; break;
        case T2BFONT: tex_to_ucs=t2b_to_ucs; break;
        case T2CFONT: tex_to_ucs=t2c_to_ucs; break;
        case X2FONT : tex_to_ucs=x2_to_ucs;  break;
        default : exit(41); /* not supported */
    }
    ucs = tex_to_ucs[ch];
    if (utf8) {
        dounichar(ucs);
        return;
    }
    else if ((latin1 && ucs<0x100) || ucs<0x80) {
        outchar(ucs);
        return;
    }

    switch (ch) {
        case 0x49:  c[0] = 'I'; break;           /* \CYRII             */
        case 0x69:  c[0] = 'i'; break;           /* \cyrii             */
        case 0x4A:  c[0] = 'J'; break;           /* \CYRJE             */
        case 0x6A:  c[0] = 'j'; break;           /* \cyrje             */
        case 0x51:  c[0] = 'Q'; break;           /* \CYRQ              */
        case 0x53:  c[0] = 'S'; break;           /* \CYRDZE            */
        case 0x57:  c[0] = 'W'; break;           /* \CYRW              */
        case 0x71:  c[0] = 'q'; break;           /* \cyrq              */
        case 0x73:  c[0] = 's'; break;           /* \cyrdze            */
        case 0x77:  c[0] = 'w'; break;           /* \cyrw              */
        case 0x0E:  c[0] = '<'; break;           /* \cyrlangle         */
        case 0x0F:  c[0] = '>'; break;           /* \cyrrangle         */
        case 0x15:  c[0] = '-'; c[1] = '-';      /* \textendash        */
                    break;
        case 0x16:  c[0] = '-'; c[1] = '-';      /* \textemdash        */
                    c[2] = '-'; break;
        case 0x27:                               /* \textquoteright    */
        case 0x60:  c[0] = '\''; break;          /* \textquoteleft     */
        case 0x10:                               /* \textquotedblleft  */
        case 0x11:                               /* \textquotedblright */
        case 0xBD:  c[0] = '"'; break;           /* \quotedblbase      */
        case 0x20:  c[0] = '_'; break;           /* \textvisiblespace  */
        case 0x17:  return;                      /* \textcompwordmark  */
        case 0x7E:  c[0] = '~'; break;           /* \textasciitilde    */
        case 0x9D:  c[0] = 'N'; c[1] = 'o';      /* \textnumero        */
                    break;
        case 0x9F:  c[0] = 'S'; break;           /* \textsection       */
        case 0xBE:  c[0] = '<'; c[1] = '<';      /* \guillemotleft     */
                    break;
        case 0xBF:  c[0] = '>'; c[1] = '>';      /* \guillemotright    */
                    break;

        /* diacritical marks */
        case 0x00:  c[0] = '`'  ; break;   /* grave        from \` */
        case 0x01:  c[0] = latin1 ? 0xb4 : '\''; break;
                                           /* acute        from \' */
        case 0x02:  c[0] = '^'  ; break;   /* circumflex   from \^ */
        case 0x03:  c[0] = '~'  ; break;   /* tilde        from \~ */
        case 0x04:  c[0] = '"'  ; break;   /* diaeresis    from \" */
        case 0x05:  c[0] = '"'  ; break;   /* double acute from \H */
        case 0x06:  c[0] = latin1 ? 0xb0 : '~'; break;
                                           /* ring above   from \r */
        case 0x07:  c[0] = '~'  ; break;   /* caron        from \v */
        case 0x08:  c[0] = '~'  ; break;   /* breve        from \u */
        case 0x09:  c[0] = '~'  ; break;   /* macron       from \= */
        case 0x0a:  c[0] = '.'  ; break;   /* dot          from \. */
        case 0x0b:  c[0] = latin1 ? 0xb8 : ','; break;
                                           /* cedilla      from \c */
        case 0x0c:  c[0] = ','  ; break;   /* ogonek       from \k */
        case 0x14:  c[0] = '~'  ; break;   /* breve        from \U */

        default  :  c[0] = '#';
    }
    if (flag != X2FONT) {
    switch (ch) {
        case 0x19:  c[0] = 'i'; break;           /* dotless i          */
        case 0x1A:  c[0] = 'j'; break;           /* dotless j          */
        case 0x1B:  c[0] = 'f'; c[1] = 'f';      /* ligature           */
                    break;
        case 0x1C:  c[0] = 'f'; c[1] = 'i';      /* ligature           */
                    break;
        case 0x1D:  c[0] = 'f'; c[1] = 'l';      /* ligature           */
                    break;
        case 0x1E:  c[0] = 'f'; c[1] = 'f';      /* ligature           */
                    c[2] = 'i'; break;
        case 0x1F:  c[0] = 'f'; c[1] = 'f';      /* ligature           */
                    c[2] = 'l'; break;
    }
    }

    cc=c;
    while (*cc) { outchar(*cc); cc++; }

    return;

} /* t2char */


/*
 * OT2CHAR -- Process a character opcode for a OT2 encoding font.
 */

void ot2char(unsigned char ch)
{
    unsigned char c[4] = {0}, *cc;
    long ucs;

    if (allchar) {
        outchar(ch);
        return;
    }
    if (!accent) {
        switch (ch) {
            case 0x20:  /* diaeresis    from \" */
            case 0x24:  /* breve        from \U */
            case 0x26:  /* acute        from \' */
            case 0x40:  /* breve        from \u */
                return;
        }
    }
    ucs = ot2_to_ucs[ch];
    if (utf8) {
        dounichar(ucs);
        return;
    }
    else if ((latin1 && ucs<0x100) || ucs<0x80) {
        outchar(ucs);
        return;
    }

    switch (ch) {
        case 0x04:  c[0] = 'I'; break;           /* \CYRII             */
        case 0x0C:  c[0] = 'i'; break;           /* \cyrii             */
        case 0x4A:  c[0] = 'J'; break;           /* \CYRJE             */
        case 0x6A:  c[0] = 'j'; break;           /* \cyrje             */
        case 0x16:  c[0] = 'S'; break;           /* \CYRDZE            */
        case 0x1E:  c[0] = 's'; break;           /* \cyrdze            */
        case 0x7B:  c[0] = '-'; c[1] = '-';      /* \textendash        */
                    break;
        case 0x7C:  c[0] = '-'; c[1] = '-';      /* \textemdash        */
                    c[2] = '-'; break;
        case 0x7D:  c[0] = 'N'; c[1] = 'o';      /* \textnumero        */
                    break;
        case 0x3C:  c[0] = '<'; c[1] = '<';      /* \guillemotleft     */
                    break;
        case 0x3D:  c[0] = 'i'; break;           /* dotless i          */
        case 0x3E:  c[0] = '>'; c[1] = '>';      /* \guillemotright    */
                    break;
        case 0x27:                               /* \textquoteright    */
        case 0x60:  c[0] = '\''; break;          /* \textquoteleft     */
        case 0x22:                               /* \textquotedblright */
        case 0x5C:  c[0] = '"'; break;           /* \textquotedblleft  */

        /* diacritical marks */
        case 0x20:  c[0] = '"'  ; break;   /* diaeresis    from \" */
        case 0x24:  c[0] = '~'  ; break;   /* breve        from \u */
        case 0x26:  c[0] = latin1 ? 0xb4 : '\''; break;
                                           /* acute        from \' */
        case 0x40:  c[0] = '~'  ; break;   /* breve        from \U */

        default  :  c[0] = '#';
    }

    cc=c;
    while (*cc) { outchar(*cc); cc++; }

    return;

} /* ot2char */



/*
 * OUTCHAR -- Here we put the character into the current page.
 *
 *            This function includes some code to handle Latin1/Scandinavian
 *            characters. I think that code doesn't belong here. IT
 *            SHOULD BE MOVED OUT.
 */

void outchar(long ch)
{
    register int i, j;
    register long dia;

/*     fprintf(stderr, "hor: %ld, ver: %ld\n", h, v); */

    if (labs(v - currentline->vv) > lineheight / 2L)
        currentline = findline();

#if 0
    j = (int) (((double) h / (double) maxpagewidth) * (ttywidth-1)) + 1;
#else
    j = (int) (h / charwidth);
#endif
    if (j > rightmargin)     /* leftmargin <= j <= rightmargin */
        j = rightmargin;
    else if (j < leftmargin)
        j = leftmargin;
    foo = leftmargin - 1;

    /*
     * This code does not really belong here ...
     *
     * The following is very specialized code, it handles national *
     * Swe/Fin characters. They are respectively: a and o with two *
     * dots ("a & "o) and a with a circle (Oa). In Swe/Fin "ASCII" *
     * these characters replace {}|[] and \.  TeX outputs these by *
     * first issuing the dots or circle and then backspace and set *
     * the a or o. When dvi2tty finds an a or o it searches in the *
     * near vicinity for the character codes that represent circle *
     * or dots and if one is found the corresponding national char *
     * replaces the special character codes.                       *
     */
    if (!allchar && compose && scascii) {
        if (strchr("aAoO", ch) != NULL) {
            for (i = IMAX(leftmargin, j-2);
                 i <= IMIN(rightmargin, j+2);
                 i++)
                if ((currentline->text[i - leftmargin] == 127) || /* DEL */
                    (currentline->text[i - leftmargin] == 34)  || /* "   */
                    (currentline->text[i - leftmargin] == 23))
                    foo = i;
            if (foo >= leftmargin) {
                j = (int) foo;
                switch (currentline->text[j - leftmargin]) {
                    case 127 :
                    case 34  :                         /* DEL or " */
                               if (ch == 'a')
                                   ch = '{';            /* } vi */
                               else if (ch == 'A')      /* dots ... */
                                   ch = '[';
                               else if (ch == 'o')
                                   ch = '|';
                               else if (ch == 'O')
                                   ch = '\\';
                               break;
                    case 23  : if (ch == 'a')
                                   ch = '}';  /* { vi */
                               else if (ch == 'A')      /* circle */
                                   ch = ']';
                               break;
                }
            }
        }
    }
    if (!allchar && compose && (latin1 || utf8)) {
          if (strchr("aAeEiIoOuUnCcNYy", ch) != NULL || (ch & MAX_UNICODE) == 0x131) {
            for (i = IMAX(leftmargin, j-2);
                 i <= IMIN(rightmargin, j+2);
                 i++) {
                dia = currentline->text[i - leftmargin] & MAX_UNICODE;
                if ((dia == 0x60)  || /* grave      */
                    (dia == 0xB0)  || /* ring above */
                    (dia == 0x2DA) || /* ring above */
                    (dia == 0xB4)  || /* acute      */
                    (dia == 0x5E)  || /* circumflex */
                    (dia == 0xA8)  || /* diaeresis  */
                    (dia == 0xB8)  || /* cedilla    */
                    (dia == 0x7E)  || /* tilde      */
                    (dia == 0x2DC))   /* tilde      */
                    foo = i;
            }
            if (foo >= leftmargin) {
                j = (int) foo;
                dia = currentline->text[j - leftmargin] & MAX_UNICODE;
                switch (dia) {
                    case 0x60: /* grave */
                               if      (ch == 'a') ch = 0xe0;
                               else if (ch == 'A') ch = 0xc0;
                               else if (ch == 'e') ch = 0xe8;
                               else if (ch == 'E') ch = 0xc8;
                               else if (ch == 'i') ch = 0xec;
                               else if ((ch & MAX_UNICODE) == 0x131) ch = 0xec;
                               else if (ch == 'I') ch = 0xcc;
                               else if (ch == 'o') ch = 0xf2;
                               else if (ch == 'O') ch = 0xd2;
                               else if (ch == 'u') ch = 0xf9;
                               else if (ch == 'U') ch = 0xd9;
                               break;
                    case 0xB0: /* ring above */
                    case 0x2DA:
                               if      (ch == 'a') ch = 0xe5;
                               else if (ch == 'A') ch = 0xc5;
                               break;
                    case 0xB4: /* acute */
                               if      (ch == 'a') ch = 0xe1;
                               else if (ch == 'A') ch = 0xc1;
                               else if (ch == 'e') ch = 0xe9;
                               else if (ch == 'E') ch = 0xc9;
                               else if (ch == 'i') ch = 0xed;
                               else if ((ch & MAX_UNICODE) == 0x131) ch = 0xed;
                               else if (ch == 'I') ch = 0xcd;
                               else if (ch == 'o') ch = 0xf3;
                               else if (ch == 'O') ch = 0xd3;
                               else if (ch == 'u') ch = 0xfa;
                               else if (ch == 'U') ch = 0xda;
                               else if (ch == 'y') ch = 0xfd;
                               else if (ch == 'Y') ch = 0xdd;
                               break;
                    case 0x5E: /* circumflex */
                               if      (ch == 'a') ch = 0xe2;
                               else if (ch == 'A') ch = 0xc2;
                               else if (ch == 'e') ch = 0xea;
                               else if (ch == 'E') ch = 0xca;
                               else if (ch == 'i') ch = 0xee;
                               else if ((ch & MAX_UNICODE) == 0x131) ch = 0xee;
                               else if (ch == 'I') ch = 0xce;
                               else if (ch == 'o') ch = 0xf4;
                               else if (ch == 'O') ch = 0xd4;
                               else if (ch == 'u') ch = 0xfb;
                               else if (ch == 'U') ch = 0xdb;
                               break;
                    case 0xA8: /* diaeresis */
                               if      (ch == 'a') ch = 0xe4;
                               else if (ch == 'A') ch = 0xc4;
                               else if (ch == 'e') ch = 0xeb;
                               else if (ch == 'E') ch = 0xcb;
                               else if (ch == 'i') ch = 0xef;
                               else if ((ch & MAX_UNICODE) == 0x131) ch = 0xef;
                               else if (ch == 'I') ch = 0xcf;
                               else if (ch == 'o') ch = 0xf6;
                               else if (ch == 'O') ch = 0xd6;
                               else if (ch == 'u') ch = 0xfc;
                               else if (ch == 'U') ch = 0xdc;
                               else if (ch == 'y') ch = 0xff;
                               else if (ch == 'Y' && utf8) ch = 0x178;
                               break;
                    case 0xB8: /* cedilla */
                               if      (ch == 'c') ch = 0xe7;
                               else if (ch == 'C') ch = 0xc7; /* It does not seem to work */
                               break;
                    case 0x7E: /* tilde */
                    case 0x2DC:
                               if      (ch == 'a') ch = 0xe3;
                               else if (ch == 'A') ch = 0xc3;
                               else if (ch == 'o') ch = 0xf5;
                               else if (ch == 'O') ch = 0xd5;
                               else if (ch == 'n') ch = 0xf1;
                               else if (ch == 'N') ch = 0xd1;
                               break;
                }
                if (utf8 && ch>0x7f) ch |= IS_UNICODE;
            }
        }
    }
    /*----------------- end of 'latin1 / Scandinavian code' ----------------*/

    if (foo == leftmargin-1) {
      if (japan) {
        while (((currentline->text[j - leftmargin] != SPACE) ||
                (kanji1 && (currentline->text[j+kanji1 - leftmargin] != SPACE)))
                && (j < rightmargin)) {
          j++;
          h += charwidth;
        }
      } else {
        while (j < rightmargin &&
               (currentline->text[j - leftmargin] != SPACE)) {
            j++;
            h += charwidth;
        }
      }
    }
    if ( allchar || ((ch >= SPACE) && (ch != DEL)) ||
         ((latin1 || scascii) && (ch == 23)) ) {
        if (j < rightmargin)
            currentline->text[j - leftmargin] = ch;
        else
            currentline->text[rightmargin - leftmargin] = '@';
        if (j > currentline->charactercount)
            currentline->charactercount = j;
        if (j < firstcolumn)
            firstcolumn = j;
    }
    h += charwidth;

    return;

} /* outchar */



/*
 * PUTCHARACTER -- Output character, don't change h 
 */

void putcharacter(long charnr)
{
    register long saveh;

    saveh = h;
    if (nttj || is8bit)
        dochar((unsigned char) charnr);
    else if (allchar || ((charnr >= 0) && (charnr <= LASTCHAR)))
        outchar((unsigned char) charnr);
    else
        setchar(charnr);
    h = saveh;

    return;

} /* putcharacter */



/*
 * SETCHAR -- Should print characters with character code>127 from
 *            current font. Note that the parameter is a dummy, since
 *            ascii-chars are<=127.
 */

void setchar(long charnr)
{

    if (is8bit)
        dochar((unsigned char) charnr);
    else
        outchar((unsigned char)(allchar ? charnr : '#'));

    return;

} /* setchar */


static const char *ptex_fontchk[] = {
    "min", "goth", "jis",
    "hmin", "hgoth", "hmgoth",               /* japanese-otf package */
    "nmlmin", "nmlgoth", "nmlmgoth",
    "hiramin", "hirakaku", "hiramaru",
    NULL /* end */
};

static const char *uptex_fontchk[] = {
    "umin", "ugoth", "ujis",
    "upjis", "upjpn", "upsch", "uptch", "upkor",
    "uphmin", "uphgoth", "uphmgoth",         /* japanese-otf package */
    "upnmlmin", "upnmlgoth", "upnmlmgoth",
    "uphiramin", "uphirakaku", "uphiramaru",
    NULL /* end */
};

static const char *jtex_fontchk[] = {
    "dmj", "dgj",
    NULL /* end */
};

static int checkjfont(const char **jfontlist, const char *name)
{
    int i, len;
    const char *tmpfont;

    i=0;
    while ( (tmpfont=jfontlist[i]) != NULL ) {
        len=strlen(tmpfont);
        if ( !strncmp(tmpfont, name, len) ) return 1;
        i++;
    }
    return 0;
} /* checkjfont */



/*
 * FONTDEF -- Process a font definition.
 */

void fontdef(int x)
{
    register int i;
    char * name;
    font * fnt;
    int namelen;
    long fntnum;
    int new = 0;

    fntnum = num(x);
    (void) get4();                      /* checksum */
    (void) get4();                      /* scale */
    (void) get4();                      /* design */
    namelen = (int) get1() + (int) get1();
    fnt = fonts;
    while (fnt != NULL && fnt->num != fntnum)       /* does fontnum exist */
        fnt = fnt->next;
    if (fnt == NULL) {
        if ((fnt = (font *) malloc(sizeof(font))) == NULL) {
            perror("fontdef");
            exit(40);
        }
        fnt->num = fntnum;
        new = 1;
    }
    else
        free(fnt->name);    /* free old name */
    if ((name = (char *) malloc((namelen+1) * sizeof(char))) == NULL) {
        perror("fontdef");
        exit(40);
    }

    for (i = 0; i < namelen; i++)
        name[i] = get1();
    name[i] = '\0';     /* properly end string */
    fnt->name = name;
    if (new) {
        fnt->next = fonts;
        fonts = fnt;
    }

    /*
     * some magic to learn about font types...
     */
    fonts->flags = 0;
    fonts->is8bit = FALSE;

    if ((asciip == FALSE && nttj == FALSE && uptex == FALSE)
        && (!jdetect) && jautodetect) {
        if ( checkjfont(ptex_fontchk, name) ) {
            /* Detect as ASCII TeX */
            asciip = TRUE;
            nttj = uptex = FALSE;
            japan = jdetect = TRUE;
            fonts->flags |= MIFONT;
            set_enc_string (NULL, PTEX_INTERNAL_ENC);
        } else if ( checkjfont(uptex_fontchk, name) ) {
            /* Detect as upTeX */
            uptex = TRUE;
            nttj = asciip = FALSE;
            japan = jdetect = TRUE;
            fonts->flags |= MIFONT;
            enable_UPTEX(true);
            set_enc_string (NULL, UPTEX_INTERNAL_ENC);
        } else if ( checkjfont(jtex_fontchk, name) ) {
            /* Detect as NTT JTeX */
            nttj = TRUE;
            asciip = uptex = FALSE;
            japan = jdetect = TRUE;
            fonts->flags |= JPFONT;
            set_enc_string (NULL, JTEX_INTERNAL_ENC);
        }
    }
    if (nttj)
        fonts->fontnum = getjsubfont(name);
    else
        fonts->fontnum = 0;

    if ((strncmp(name, "ec", 2)) == 0) {
            fonts->flags = T1FONT;
            fonts->is8bit = TRUE;
            return;
    }
    else if ((strncmp(name, "tc", 2)) == 0 ||
             (strncmp(name, "ts1", 3)) == 0) {
            fonts->flags = TS1FONT;
            fonts->is8bit = TRUE;
            return;
    }
    else if ((strncmp(name, "wn", 2)) == 0) {
            fonts->flags = OT2FONT;
            return;
    }
    else if ((strncmp(name, "la", 2)) == 0) {
            fonts->flags = T2AFONT;
            fonts->is8bit = TRUE;
            return;
    }
    else if ((strncmp(name, "lb", 2)) == 0) {
            fonts->flags = T2BFONT;
            fonts->is8bit = TRUE;
            return;
    }
    else if ((strncmp(name, "lc", 2)) == 0) {
            fonts->flags = T2CFONT;
            fonts->is8bit = TRUE;
            return;
    }
    else if ((strncmp(name, "rx", 2)) == 0) {
            fonts->flags = X2FONT;
            fonts->is8bit = TRUE;
            return;
    }
    if ((strstr(name, "sy")) != NULL)
            fonts->flags = SYMFONT;
    if ((strstr(name, "tt")) != NULL)
            fonts->flags = TTFONT;
    if ((strstr(name, "mi")) != NULL)
            fonts->flags = MIFONT;

    return;

} /* fontdef */



#define    NJSUBS        33
const char *jsf_names[]={
    "sy", "roma", "hira", "kata", "greek", "russian", "keisen",
    "ka", "kb", "kc", "kd", "ke", "kf", "kg", "kh", "ki", "kj",
    "kk", "kl", "km", "kn", "ko", "kp", "kq", "kr", "ks", "kt",
    "ku", "kv", "kw", "kx", "ky", "kz"
};


int getjsubfont(char *s)
{
    int jf;

    if (strlen(s) > 3 && s[0] == 'd' && (s[1] == 'm' || s[1] == 'g') && s[2] == 'j') {
        for (jf = 0; jf < NJSUBS; jf++) {
            if (strncmp(&s[3], jsf_names[jf], strlen(jsf_names[jf])) == 0) 
                return jf+1;
        }
    }

    return 0;

} /* getjsubfont */



/*
 * SETFONT -- Switch to specific font. Try to find out if it is a symbol
 *            font.
 *            Option -c allchar does not pertain to this portion, so symbols
 *            are still translated.
 */

void setfont(long fntnum)
{
    char * s;
    const char * d;

    symbolfont = FALSE;
    ttfont = FALSE;
    mifont = FALSE;
    fnt = fonts;

    while (fnt != NULL && fnt->num != fntnum)
        fnt = fnt->next;

    if (fnt == NULL) {
        /* error : font not found */
        return;
    }

    if (fnt->fontnum == 0) {
        symbolfont = fnt->flags == SYMFONT;
        ttfont = fnt->flags == TTFONT;
        mifont = fnt->flags == MIFONT;
        is8bit = fnt->is8bit;
    }

    s = fnt->name;
    if (printfont) {
         d = delim;      /* print delim and font name if -b was chosen */
         while (*d) {putcharacter(*d); d++;}
         while (*s) {putcharacter(*s); s++;}
         while (d-- > delim) {putcharacter(*d);}
    }

    return;

} /* setfont */



void jischar(unsigned long ch)
{
    unsigned int Ku, Ten;

    compute_jis(fnt->fontnum, (unsigned int) ch, &Ku, &Ten);
    kanji1 = 1;
    outchar((unsigned char)(Ku+128));
    kanji1 = 0;
    outchar((unsigned char)(Ten+128));

    return;

} /* jischar */

#define	kushift(c)	c+0x20
#define	tenshift(c)	c+0x20

void compute_jis(int f, unsigned int c, unsigned int *ku, unsigned int *ten)
{
    int n;

    if (f <= 7) {
        if (f == 1) {
            if (c >= 100) {
                *ku = kushift(2);
                *ten = tenshift(c-100);
            }
            else {
                *ku = kushift(1);
                *ten = tenshift(c);
            }
        }
        else if (f == 2) {
            *ku = kushift(3);
            *ten = tenshift(c-32);
        }
        else {
            *ku = kushift(f+1);
            *ten = tenshift(c);
        }
    }
    else if (f <= 19) {    /* Daiichi Suijun */
        n = (f-8)*256+c;
        *ku = kushift((n/94)+16);
        *ten = tenshift((n%94)+1);
    }
    else {            /* Daini Suijun */
        n = (f-20)*256+c;
        *ku = kushift((n/94)+48);
        *ten = tenshift((n%94)+1);
    }

    return;

} /* compute_jis */

