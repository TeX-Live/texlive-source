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

#if defined(VMS) 
# include types.h
# include stat
#else
# if defined(THINK_C)
#  include <unix.h>
# else
#  include <sys/types.h>
#  include <sys/stat.h>
# endif
#endif

#if defined(MSDOS) || defined(THINK_C)
# include <math.h>
#endif

#include "commands.h"



/*
 * Constant definitions
 */

#if defined(VMS) 
#define mseek vmsseek
#define ROUND(a)        (a>=0.0 ?  (int) (a + 0.5) : (int) (a - 0.5) )
#else
#define mseek fseek
#endif

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

const char *dvistuff = "@(#) dvistuff.c  " VERSION " 20101027 M.J.E. Mol (c) 1989-2010";

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
    unsigned char   text[LINELEN+1];    /* leftmargin...rightmargin          */
} linetype;

typedef struct _font {
    long    num;
    struct _font * next;
    char  * name;
    char    flags; /* to store font types, to get rid of nttj/asciip/uptex/ttfont/symbolfon/mifont vars */
    int     fontnum; /* helper for japanese fonts */
} font;

#define SYMFONT   0x01
#define TTFONT    0x02
#define MIFONT    0x04
#define JAPFONT   0x08
#define JASCFONT  0x10


bool        pageswitchon;       /* true if user-set pages to print           */
bool        sequenceon;         /* false if pagesw-nrs refers to TeX-nrs     */
bool        scascii;            /* if true make Scand. nat. chars right      */
bool        latin1;             /* if true make latin1 chars right           */
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
bool        noffd;              /* if true output ^L instead of formfeed     */
const char *delim;              /* -bdelim for font switch printing          */
bool        printfont;          /* true if user wants font switches printed  */
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

#if defined(MSDOS)
void            postamble       (void);
void            preamble        (void);
void            walkpages       (void);
void            initpage        (void);
void            dopage          (void);
void            skippage        (void);
void            printpage       (void);
bool            inlist          (long);
void            rule            (bool, long, long);
void            ruleaux         (long, long, char);
long            horizontalmove  (long);
int             skipnops        (void);
linetype    *   my_getline      (void);
linetype    *   findline        (void);
unsigned long   num             (int);
long            snum            (int);
void            dochar          (unsigned char);
void            symchar         (unsigned char);
void            normchar        (unsigned char);
void            outchar         (unsigned char);
void            putcharacter    (long);
void            setchar         (long);
void            fontdef         (int);
void            setfont         (long);
void            jischar         (unsigned long);
int             compute_jis     (int, unsigned int, unsigned int *, unsigned int *);
void            dokanji         (long);
int             getjsubfont     (char *);

#else
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
void            normchar        (unsigned char ch);
void            outchar         (unsigned char ch);
void            putcharacter    (long charnr);
void            setchar         (long charnr);
void            fontdef         (int x);
void            setfont         (long fntnum);
void            jischar         (unsigned long ch);
void            compute_jis     (int f, unsigned int c, unsigned int * ku, unsigned int * ten);
void            dokanji         (long ch);
int             getjsubfont     (char * s);
#if defined(VMS)
long		vmsseek		();
long		vms_ftell	();
long		vms_ungetc	();
#endif
#endif



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
#if !defined (THINK_C) && defined(VMS)
    struct stat st;
#endif

#if defined (THINK_C)
    size = DVIfile->len;
#elif defined(VMS)
    fstat (fileno(DVIfile), &st);
    size = (long) st.st_size;                   /* get size of file          */
#else
    fseek (DVIfile, 0L, SEEK_END);
    size = ftell (DVIfile);                     /* get size of file          */
#endif

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
    if ((opcode = skipnops()) != PRE)
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
    register unsigned char ch;
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

		if (japan) {
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
		  if (japan) {
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

#if defined(MSDOS) || defined(THINK_C)
    if (labs(amount) > charwidth / 4L) {    /* } to make vi happy */
#else
    if (abs(amount) > charwidth / 4L) {
#endif
        foo = 3*charwidth / 4;
        if (amount > 0)
            amount = ((amount+foo) / charwidth) * charwidth;
        else
#if defined(VMS)
            amount = (ROUND( (float) (amount-foo) / charwidth) + 1)* charwidth;
#else
            amount = ((amount-foo) / charwidth) * charwidth;
#endif
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

    while ((opcode = (int) num(1)) == NOP);

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
 * DOKANJI -- Process a kanji character opcode.
 */
 
void dokanji(long ch)
{
    long i;

    i = toBUFF(fromDVI(ch));
    kanji1 = 3;
    if (BYTE1(i) != 0) outchar(BYTE1(i));
    kanji1 = 2;
    if (BYTE2(i) != 0) outchar(BYTE2(i));
    kanji1 = 1;
    /* always */       outchar(BYTE3(i));
    kanji1 = 0;
    /* always */       outchar(BYTE4(i));

    return;

} /* dokanji */



/*
 * DOCHAR -- Process a character opcode.
 */

void dochar(unsigned char ch)
{

    if (nttj && fnt->fontnum)
        jischar((long) ch);
    else if (symbolfont == TRUE)
        symchar(ch);
    else
        normchar(ch);

    return;

} /* dochar */



/*
 * SYMCHAR -- Process a character opcode for a symbol font.
 */

void symchar(unsigned char ch)
{

    switch (ch) {       /* can do a lot more on MSDOS/latin1/unicode machines ... */
       case   0: ch = '-'; break;
       case   1: ch = '.'; break;
       case   2: ch = 'x'; break;
       case   3: ch = '*'; break;
       case  13: ch = 'O'; break;
       case  14: ch = 'O'; break;
       case  15: ch = 'o'; break;
       case  24: ch = '~'; break;
       case  32: ch = nttj ? '<' : 32; break; /* really only for japan? */
       case  33: ch = nttj ? '>' : 33; break; /* really only for japan? */
       case 102: ch = '{'; break;
       case 103: ch = '}'; break;
       case 104: ch = '<'; break;
       case 105: ch = '>'; break;
       case 106: ch = '|'; break;
       case 110: ch = '\\'; break;
    }

    outchar(ch);

    return;

} /* symchar */



/*
 * NORMCHAR -- Process a character opcode for a normal font.
 */

void normchar(unsigned char ch)
{

    switch (ch) {
        case 11  :  if (ttfont)
                        ch = '^';                   /* up symbol       */
                    else if (!allchar) {
                        outchar('f'); ch = 'f';     /* ligature        */
                    }
                    break;
        case 12  :  if (ttfont)
                        ch = 'v';                   /* low symbol       */
                    else if (!allchar) {
                        outchar('f'); ch = 'i';     /* ligature        */
                    }
                    break;
        case 13  :  if (ttfont)
                        ch = '`';
                    else if (!allchar) {
                        outchar('f'); ch = 'l';     /* ligature        */
                    }
                    break;
        case 14  :  if (ttfont)
                        ch = 'i';                   /* spanish !        */
                    else if (!allchar) {
                        outchar('f'); outchar('f');
                                  ch = 'i';         /* ligature        */
                    }
                    break;
        case 15  :  if (ttfont)
                        ch = '.';                   /* spanish ?        */
                    else if (!allchar) {
                        outchar('f'); outchar('f');
                                  ch = 'l';         /* ligature        */
                    }
                    break;
        case 16  :  if (!allchar) ch = 'i'; break;
        case 17  :  if (!allchar) ch = 'j'; break;
        case 25  :  if (!allchar) {
                        if (latin1) {
                            ch = 0xdf;
                        }
                        else {
                            outchar('s');
                            ch = 's';
                        }
                    }
                    break;  /* German double s */
        case 26  :  if (!allchar) {
                        if (latin1) {
                            ch = 0xe6;
                        }
                        else {
                            outchar('a');
                            ch = 'e';
                        }
                    }
                    break;  /* Dane/Norw ae    */
        case 27  :  if (!allchar) {
                        outchar('o');
                        ch = 'e';
                    }
                    break;  /* Dane/Norw oe    */
        case 28  :  if (!allchar) {
                        if (scascii)
                            ch = '|';
                        else if (latin1)
                            ch = 0xf8;
                        else
                            ch = 'o';
                    }
                    break; /* Dane/Norw /o    */
        case 29  :  if (!allchar) {
                        if (latin1) {
                            ch = 0xc6;
                        }
                        else {
                            outchar('A');
                            ch = 'E';
                        }
                    }
                    break;  /* Dane/Norw AE    */
        case 30  :  if (!allchar) {
                        outchar('O');
                        ch = 'E';
                    }
                    break;  /* Dane/Norw OE    */
        case 31  :  if (!allchar) {
                        if (scascii)
                            ch = '\\';
                        else if (latin1)
                            ch = 0xd8;
                        else
                            ch = 'O';
                    }
                    break; /* Dane/Norw /O    */
        case 32  :  ch = allchar || ttfont ? ch : '_'; break;
                                                        /* underlined blank */
        case 58  :  ch = allchar || !mifont ? ch : '.'; break; /* if japan */
        case 59  :  ch = allchar || !mifont ? ch : ','; break; /* if japan */
        case 92  :  ch = allchar || ttfont ? ch : '"'; break;  /* \ from `` */
        case 123 :  ch = allchar || ttfont ? ch : '-'; break;  /* { from -- */
        case 124 :  ch = allchar || ttfont ? ch : '_'; break;  /* | from --- */
        case 125 :  ch = allchar || ttfont ? ch : '"'; break;  /* } from \H */
        case 126 :  ch = allchar || ttfont ? ch : '"'; break;  /* ~ from \~ */
        case 127 :  if (!allchar) ch = '"'; break;             /* DEL from \" */
       
        /*
         * Should SPACE be useed for non-accents ???
         * This seems to work ...
         */
        case 18  :  ch = !allchar && accent ? '`' : ch; break;  /* from \` */
        case 19  :  ch = !allchar && accent ? 0x27 : ch; break; /* from \' */
        case 20  :  ch = !allchar && accent ? '~' : ch; break;  /* from \v */
        case 21  :  ch = !allchar && accent ? '~' : ch; break;  /* from \u */
        case 22  :  ch = !allchar && accent ? '~' : ch; break;  /* from \= */
        case 24  :  ch = !allchar && accent ? ',' : ch; break;  /* from \c */
        case 94  :  ch = (!allchar && accent && !ttfont) ? '^' : ch; break;
                                                                /* ^ from \^ */
        case 95  :  ch = (!allchar && accent && !ttfont) ? '`' : ch; break;
                                                                /* _ from \. */
    }
    outchar(ch); 

    return;

} /* normchar */



/*
 * OUTCHAR -- Here we put the character into the current page.
 *
 *            This function includes some code to handle Latin1/Scandinavian
 *            characters. I think that code doesn't belong here. IT
 *            SHOULD BE MOVED OUT.
 */

void outchar(unsigned char ch)
{
    register int i, j;

/*     fprintf(stderr, "hor: %ld, ver: %ld\n", h, v); */

#if defined(MSDOS) || defined(THINK_C)
    if (labs(v - currentline->vv) > lineheight / 2L)
#else
    if (abs(v - currentline->vv) > lineheight / 2L)
#endif
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
    if (!allchar && (scascii || latin1)) {
        if ((ch == 'a') || (ch == 'A') || (ch == 'o') || (ch == 'O') || (ch == 'u') || (ch == 'U')) {
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
                                   ch = latin1 ? 0xe4 : '{';            /* } vi */
                               else if (ch == 'A')      /* dots ... */
                                   ch = latin1 ? 0xc4 : '[';
                               else if (ch == 'o')
                                   ch = latin1 ? 0xf6 : '|';
                               else if (ch == 'O')
                                   ch = latin1 ? 0xd6 : '\\';
                               else if (ch == 'u')
                                   ch = latin1 ? 0xfc : 'u';
                               else if (ch == 'U')
                                   ch = latin1 ? 0xdc : 'U';
                               break;
                    case 23  : if (ch == 'a')
                                   ch = latin1 ? 0xe5 : '}';  /* { vi */
                               else if (ch == 'A')      /* circle */
                                   ch = latin1 ? 0xc5 : ']';
                               break;
                }
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
               ( (currentline->text[j - leftmargin] != SPACE) ||
                 (kanji1 && (currentline->text[j+1 - leftmargin] != SPACE))
               ) ) {
            j++;
            h += charwidth;
        }
      }
    }
    if ( allchar || ((ch >= SPACE) && (ch != DEL)) ||
         ((latin1 || scascii) && (ch == 23)) ) {
          /*  ((latin1 || scascii) && (ch == DEL)) )     if VMS ??? */
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
    if (nttj)
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

    outchar(allchar ? (unsigned char) charnr : '#');

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
            exit(1);
        }
        fnt->num = fntnum;
        new = 1;
    }
    else
        free(fnt->name);    /* free old name */
    if ((name = (char *) malloc((namelen+1) * sizeof(char))) == NULL) {
        perror("fontdef");
        exit(1);
    }
    
    for (i = 0; i < namelen; i++)
        name[i] = get1();
    name[i] = '\0';	/* properly end string */
    fnt->name = name;
    if (new) {
        fnt->next = fonts;
        fonts = fnt;
    }

    /*
     * some magic to learn about font types...
     */
    fonts->flags = 0;

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
            fonts->flags |= JAPFONT;
            set_enc_string (NULL, JTEX_INTERNAL_ENC);
        }
    }
    if (nttj)
        fonts->fontnum = getjsubfont(name);
    else
        fonts->fontnum = 0;

    if ((strstr(name, "sy")) != NULL)
            fonts->flags |= SYMFONT;
    if ((strstr(name, "tt")) != NULL)
            fonts->flags |= TTFONT;
    if ((strstr(name, "mi")) != NULL)
            fonts->flags |= MIFONT;

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
        symbolfont = fnt->flags & SYMFONT;
        ttfont = fnt->flags & TTFONT;
        mifont = fnt->flags & MIFONT;
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
    outchar(Ku+128);
    kanji1 = 0;
    outchar(Ten+128);

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

   

/* 
 * VMS CODE 
 */

#if defined(VMS)
long vmsseek(fp,n,dir)
FILE *fp;
long n;
long dir;
{
    long k,m,pos,val,oldpos;
    struct stat buffer;

    for (;;) {                     /* loops only once or twice */
        switch (dir) {
            case 0:            /* from BOF */
                    oldpos = vms_ftell(fp);
                    k = n & 511;
                    m = n >> 9;
                    if (((*fp)->_cnt) && ((oldpos >> 9) == m)) {
                        val = 0; /* still in */
                        (*fp)->_ptr = ((*fp)->_base) + k;
                        (*fp)->_cnt = 512 - k;
                    }
                    else {
                        val = fseek(fp, m << 9, 0);
                        if (val == 0) {
                            (*fp)->_cnt = 0;
                            (void) fgetc(fp);
                            (*fp)->_ptr = ((*fp)->_base) + k;
                            (*fp)->_cnt = 512 - k;
                        }
                    }
                    return(val);

            case 1: pos = vms_ftell(fp);
                    if (pos == EOF)
                        return (EOF);
                    n += pos;
                    dir = 0;
                    break;

            case 2: val = fstat(fileno(fp), &buffer);
                    if (val == EOF)
                        return (EOF);
                    n += buffer.st_size - 1;

                    dir = 0;
                    break;

            default : return (EOF);
        }
    }

    /* NOTREACHED */

} /* vmsseek */
        


long vms_ftell(fp)
FILE *fp;
{
    char c;
    long pos;
    long val;

    if ((*fp)->_cnt == 0) {
        c = fgetc(fp);
        val = vms_ungetc(c, fp);
        if (val != c)
            return (EOF);
    }
    pos = ftell(fp);
    if (pos >= 0)
        pos += ((*fp)->_ptr) - ((*fp)->_base);

    return (pos);

} /* vms_ftell */



long vms_ungetc(c,fp)
char c;
FILE *fp;
{

    if ((c == EOF) && feof(fp))
        return (EOF);

    if ((*fp)->_cnt >= 512)
        return (EOF);
    
    (*fp)->_cnt++;
    (*fp)->_ptr--;
    *((*fp)->_ptr) = c;

    return (c);

} /*vms_ungetc */
#endif
