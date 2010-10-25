
#include "dvi2tty.h"
#include <sys/types.h>
#include <sys/stat.h>
#if defined(VMS) 
#include types.h
#include stat
#endif
#if defined(MSDOS)
#include <math.h>
#endif
#include "commands.h"

#if defined(VMS) 
#define mseek vmsseek
#define ROUND(a)        (a>=0.0 ?  (int) (a + 0.5) : (int) (a - 0.5) )
#else
#define mseek fseek
#endif

#define VERSIONID            2 /* dvi version number that pgm handles      */
#define VERTICALEPSILON 450000L /* crlf when increasing v more than this   */

#define rightmargin     152    /* nr of columns allowed to the right of h=0*/
#define leftmargin      -50    /* give some room for negative h-coordinate */
#define LINELEN         203    /* rightmargin - leftmargin + 1 */

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

#if 0
char *dvistuff = "@(#) dvistuff.c  4.1 27/03/90 M.J.E. Mol (c) 1989, 1990";
#endif

/*---------------------------------------------------------------------------*/

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
    char            text[LINELEN+1];    /* leftmargin...rightmargin          */
} linetype;

typedef struct _font {
    long    num;
    struct _font * next;
    char  * name;
} font;


/*---------------------------------------------------------------------------*/

bool        pageswitchon;       /* true if user-set pages to print           */
bool        sequenceon;         /* false if pagesw-nrs refers to TeX-nrs     */
bool        scascii;            /* if true make Scand. nat. chars right      */
bool        noffd;              /* if true output ^L instead of formfeed     */

int         opcode;             /* dvi-opcodes                               */

long        h, v;               /* coordinates, horizontal and vertical      */
long        w, x, y, z;         /* horizontal and vertical amounts           */

long        pagecounter;        /* sequence page number counter              */
long        backpointer;        /* pointer for offset to previous page       */
long        pagenr;             /* TeX page number                           */
int         stackmax;           /* stacksize required                        */

long        maxpagewidth;       /* width of widest page in file              */
long        charwidth;          /* aprox width of character                  */

linetype *  currentline;        /* pointer to current line on current page   */
linetype *  firstline;          /* pointer to first line on current page     */
linetype *  lastline;           /* pointer to last line on current page      */
int         firstcolumn;        /* 1st column with something to print        */

stackitem * stack;              /* stack for dvi-pushes                      */
int         sptr;               /* stack pointer                             */

font * fonts = NULL;            /* List of fontnames defined                 */
int  symbolfont = FALSE;        /* true if font is a symbol font             */

/*---------------------------------------------------------------------------*/

#if defined(KPATHSEA) || defined(MSDOS)
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
linetype    *   texlive_getline (void);
linetype    *   findline        (void);
unsigned long   num             (int);
long            snum            (int);
void            dochar          (char);
void            symchar         (char);
void            normchar        (char);
void            outchar         (unsigned char);
void            putcharacter    (long);
void            setchar         (long);
void            fontdef         (int);
void            setfont         (long);
#else
void            postamble       ();
void            preamble        ();
void            walkpages       ();
void            initpage        ();
void            dopage          ();
void            skippage        ();
void            printpage       ();
bool            inlist          ();
void            rule            ();
void            ruleaux         ();
long            horizontalmove  ();
int             skipnops        ();
linetype    *   texlive_getline ();
linetype    *   findline        ();
unsigned long   num             ();
long            snum            ();
void            dochar          ();
void            symchar         ();
void            normchar        ();
void            outchar         ();
void            putcharacter    ();
void            setchar         ();
void            fontdef         ();
void            setfont         ();
#if defined(VMS)
long		vmsseek		();
long		vms_ftell	();
long		vms_ungetc	();
#endif
#endif

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*
 * The main function for processing the dvi file.
 * Here we assume there are to file pointers: DVIfile and output.
 * Also we have a list of pages pointed to by 'currentpage',
 * which is only used (in 'inlist()') when a page list is given.
 */

void dvimain(void)
{

    postamble();                            /* seek and process the postamble */
    /* note that walkpages *must* immediately follow preamble */
    preamble();                             /* process preamble               */
    walkpages();                            /* time to do the actual work!    */

} /* dvimain */

/*---------------------------------------------------------------------------*/

void postamble(void)        /* find and process postamble, use random access */
{
    register long size;
    register int  count;
    struct stat st;

    fstat (fileno(DVIfile), &st);
    size = (long) st.st_size;                   /* get size of file          */
    count = -1;
    do {              /* back file up past signature bytes (223), to id-byte */
        if (size == 0)
            errorexit(nopst);
        size--;
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
    mseek(DVIfile, sget4(), absolute);      /* and to start of postamble   */
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

} /* postamble */

/*---------------------------------------------------------------------------*/

void preamble(void)             /* process preamble, use random access       */
{

    mseek(DVIfile, 0L, absolute);       /* read the dvifile from the start   */
    if ((opcode = skipnops()) != PRE)
        errorexit(nopre);
    opcode = (int) get1();        /* check id in preamble, ignore rest of it */
    if (opcode != VERSIONID)
        errorexit(badid);
    mseek(DVIfile, 12L, relative);  /* numerator, denominator, magnification */
    mseek(DVIfile, get1(), relative);         /* skip job identification     */

} /* preamble */

/*----------------------------------------------------------------------------*/

void walkpages(void)              /* process the pages in the DVI-file */
{
    register bool wantpage;

    pagecounter = 0L;
    while ((opcode = skipnops()) != POST) {
        if (opcode != BOP)              /* should be at start of page now    */
            errorexit(nobop);
        else {
            pagecounter++;
            pagenr = sget4();           /* get TeX page number               */
            mseek(DVIfile, 36L, relative); /* skip page header */
            backpointer = sget4();      /* get previous page offset          */
            if (pageswitchon)
                if (sequenceon)
                    wantpage = inlist(pagecounter);
                else
                    wantpage = inlist(pagenr);
            else
                wantpage = TRUE;

            if (wantpage) {
                initpage();
                dopage();
                printpage();
            }
            else {
                skippage();
            }
        }
    }

} /* walkpages */

/*---------------------------------------------------------------------------*/

void initpage(void)
{

    h = 0L;  v = 0L;                        /* initialize coordinates   */
    x = 0L;  w = 0L;  y = 0L;  z = 0L;      /* initialize amounts       */
    sptr = 0;                               /* initialize stack         */
    currentline = texlive_getline();                /* initialize list of lines */
    currentline->vv = 0L;
    firstline   = currentline;
    lastline    = currentline;
    firstcolumn = rightmargin;
    if (pageswitchon) {
        if ((sequenceon && (pagecounter != firstpage->pag)) ||
            (!sequenceon && (pagenr != firstpage->pag))) {
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

} /* initpage */

/*----------------------------------------------------------------------------*/

void dopage(void)
{

    while ((opcode = (int) get1()) != EOP) {    /* process page until eop */
        if (opcode <= LASTCHAR)
            dochar((char) opcode);
        else if ((opcode >= FONT_00) && (opcode <= FONT_63)) 
            setfont(opcode - FONT_00);
        else if (opcode > POST_POST)
            errorexit(illop);
        else
            switch (opcode) {
                case SET1     : setchar(get1()); break;
                case SET2     : setchar(get2()); break;
                case SET3     : setchar(get3()); break;
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
                case POP      : if (sptr == 0)                   /* pop */
                                    errorexit(stkuf);
                                sptr--;
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

} /* dopage */

/*----------------------------------------------------------------------------*/

void skippage(void)            /* skip past one page */
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

} /* skippage */

/*---------------------------------------------------------------------------*/

void printpage(void)   /* 'end of page', writes lines of page to output file */
{
    register int  i, j;
    register unsigned char ch;

    if (sptr != 0)
        fprintf(stderr, "dvi2tty: warning - stack not empty at eop.\n");
    for (currentline = firstline; currentline != nil;
          currentline = currentline->next) {
        if (currentline != firstline) {
            foo = ((currentline->vv - currentline->prev->vv)/VERTICALEPSILON)-1;
            if (foo > 3)
                foo = 3;        /* linespacings not too large */
            for (i = 1; i <= (int) foo; i++)
                putc('\n', output);
        }
        if (currentline->charactercount >= leftmargin) {
            foo = ttywidth - 2;
            for (i = firstcolumn, j = 1; i <= currentline->charactercount;
                   i++, j++) {
                ch = currentline->text[i - leftmargin];
                if (ch >= SPACE)
                    putc(ch, output);
                if ((j > (int) foo) && (currentline->charactercount > i+1)) {
                        fprintf(output, "*\n");         /* if line to large */
                        fprintf(output, " *");          /* mark output      */
                        j = 2;
                }
            } 
        }
        putc('\n', output);
    } 

    currentline = firstline;
    while (currentline->next != nil) {
        currentline = currentline->next;
        free(currentline->prev);
    }
    free(currentline);              /* free last line */
    currentline = nil;

} /* printpage */

/*----------------------------------------------------------------------------*/

bool inlist(register long pagenr)                         /* ret true if in list of pages */
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

/*----------------------------------------------------------------------------*/

void rule(bool moving, long rulewt, long ruleht)
{   /* output a rule (vertical or horizontal), increment h if moving is true */

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
        else if (ruleht > (VERTICALEPSILON / 2))
            ch = '=';
        else
            ch = '_';
        ruleaux(rulewt, ruleht, ch);
        v = savev;
    }
    if (!moving)
        h = saveh;

} /* rule */



void ruleaux(long rulewt, long ruleht, char ch)     /* recursive  that does the job */
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
    ruleht -= VERTICALEPSILON;      /* decrease the height                   */
    if (ruleht > VERTICALEPSILON) { /* still more vertical?                  */
        rmh = h;                    /* save current h (right margin)         */
        h = lmh;                    /* restore left margin                   */
        v -= (VERTICALEPSILON + VERTICALEPSILON / 10);
        ruleaux(rulewt, ruleht, ch);
        h = rmh;                    /* restore right margin                  */
    }

} /* ruleaux */

/*----------------------------------------------------------------------------*/

long horizontalmove(register long amount)
{

#if defined(MSDOS)
    if (labs(amount) > charwidth / 4L) {
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

/*----------------------------------------------------------------------------*/

int skipnops(void)                  /* skips by no-op commands  */
{
    register int opcode;

    while ((opcode = (int) num(1)) == NOP);
    return opcode;

} /* skipnops */

/*----------------------------------------------------------------------------*/

linetype *texlive_getline(void)         /* returns an initialized line-object */
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

} /* texlive_getline */

/*----------------------------------------------------------------------------*/

linetype *findline(void)        /* find best fit line were text should go */
{                               /* and generate new line if needed        */
    register linetype *temp;
    register long topd, botd;

    if (v <= firstline->vv) {                      /* above first line */
        if (firstline->vv - v > VERTICALEPSILON) {
            temp = texlive_getline();
            temp->next = firstline;
            firstline->prev = temp;
            temp->vv = v;
            firstline = temp;
        }
        return firstline;
    }

    if (v >= lastline->vv) {                       /* below last line */
        if (v - lastline->vv > VERTICALEPSILON) {
            temp = texlive_getline();
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
    if ((topd < VERTICALEPSILON) || (botd < VERTICALEPSILON)) {
        if (topd < botd)                           /* take best fit */
            return temp;
        else
            return temp->next;
    }

    /* no line fits suitable, generate a new one */
    currentline = texlive_getline();
    currentline->next = temp->next;
    currentline->prev = temp;
    temp->next->prev = currentline;
    temp->next = currentline;
    currentline->vv = v;
    return currentline;

} /* findline */

/*----------------------------------------------------------------------------*/

unsigned long num(register int size)
{
    register int i;
    register long x = 0;

    for (i = 0; i < size; i++)
        x = (x << 8) + (unsigned) getc(DVIfile);
    return x;

} /* num */


long snum(register int size)
{
    register int i;
    register long x = 0;

    x = getc(DVIfile);
    if (x & 0x80)
        x -= 0x100;
    for (i = 1; i < size; i++)
        x = (x << 8) + (unsigned) getc(DVIfile);
    return x;

} /* snum */

/*----------------------------------------------------------------------------*/

void dochar(char ch)
{

    if (symbolfont == TRUE)
        symchar(ch);
    else
        normchar(ch);

    return;

} /* dochar */



void symchar(char ch)                     /* output ch to appropriate line */
{

    switch (ch) {       /* can do a lot more on MSDOS machines ... */
       case   0: ch = '-'; break;
       case   1: ch = '.'; break;
       case   2: ch = 'x'; break;
       case   3: ch = '*'; break;
       case  13: ch = 'O'; break;
       case  14: ch = 'O'; break;
       case  15: ch = 'o'; break;
       case  24: ch = '~'; break;
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



void normchar(char ch)
{

    switch (ch) {
        case 11  :  outchar('f'); ch = 'f'; break;  /* ligature        */
        case 12  :  outchar('f'); ch = 'i'; break;  /* ligature        */
        case 13  :  outchar('f'); ch = 'l'; break;  /* ligature        */
        case 14  :  outchar('f'); outchar('f');
                                  ch = 'i'; break;  /* ligature        */
        case 15  :  outchar('f'); outchar('f');
                                  ch = 'l'; break;  /* ligature        */
        case 16  :  ch = 'i'; break;
        case 17  :  ch = 'j'; break;
        case 25  :  if (scascii)                    /* German sz ligature */
                        ch = 0xdf;
                    else
                        {outchar('s'); ch = 's';}
                    break;
        case 26  :  if (scascii)                    /* Dane/Norw ae    */
                        ch = 0xe6;
                    else
                        {outchar('a'); ch = 'e';}
                    break;
        case 27  :  outchar('o'); ch = 'e'; break;  /* Dane/Norw oe    */
        case 28  :  if (scascii)                    /* Dane/Norw /o    */
                        ch = 0xf8;
                    else
                        ch = 'o';
                    break;
        case 29  :  if (scascii)                    /* Dane/Norw AE    */
                        ch = 0xc6;
                    else
                        {outchar('A'); ch = 'E';}
                    break;
        case 30  :  outchar('O'); ch = 'E'; break;  /* Dane/Norw OE    */
        case 31  :  if (scascii)
                        ch = 0xd8;                  /* Dane/Norw /O    */
                    else
                        ch = 'O';
                    break;
        case 92  :  ch = '"'; break;  /* \ from `` */
        case 123 :  ch = '-'; break;  /* { from -- */
        case 124 :  ch = '_'; break;  /* | from --- */
        case 125 :  ch = '"'; break;  /* } from \H */
        case 126 :  ch = '"'; break;  /* ~ from \~ */
        case 127 :  ch = '"'; break;  /* DEL from \" */
#if 0
        case 18  :  ch = '`'; break   /* from \` */
        case 19  :  ch = ''''; break  /* from \' */
        case 20  :  ch = '~'; break   /* from \v */
        case 21  :  ch = '~'; break   /* from \u */
        case 22  :  ch = '~'; break   /* from \= */
        case 24  :  ch = ','; break   /* from \c */
        case 94  :  ch = '^'; break   /* ^ from \^ */
        case 95  :  ch = '`'; break   /* _ from \. */
#endif
    }
    outchar(ch); 

    return;

} /*normchar */



void outchar(unsigned char ch)                     /* output ch to appropriate line */
{
    register int i, j;

#if defined(MSDOS)
    if (labs(v - currentline->vv) > VERTICALEPSILON / 2L)
#else
    if (abs(v - currentline->vv) > VERTICALEPSILON / 2L)
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
     *-------------------------------------------------------------*
     * The following is very specialized code, it handles some eu- *
     * ropean characters.  These are: a, o, u with two dots ("a &  *
     * "o & "u), and a with a circle (Oa).  TeX outputs these by   *
     * first issuing the dots or circle and then backspace and set *
     * the a, o, or u.  When dvitty finds an a, o, or u it sear-   *
     * ches in the near vicinity for the character codes that re-  *
     * present circle or dots and if one is found the correspon-   *
     * ding Latin-1 character replaces the special character code. *
     *-------------------------------------------------------------*/
    if (scascii) {
        if ((ch == 'a') || (ch == 'A') || (ch == 'o') || (ch == 'O') ||
		      (ch == 'u') || (ch == 'U')) {
            for (i = IMAX(leftmargin, j-2);
                 i <= IMIN(rightmargin, j+2);
                 i++)
                if ((currentline->text[i - leftmargin] == 127) ||
                    (currentline->text[i - leftmargin] == 34) ||
                    (currentline->text[i - leftmargin] == 23))
                    foo = i;
            if (foo >= leftmargin) {
                j = (int) foo;
                switch (currentline->text[j - leftmargin]) {
                    case 127 : case 34:
                               if (ch == 'a')
                                   ch = 0xe4;
                               else if (ch == 'A')      /* dots ... */
                                   ch = 0xc4;
                               else if (ch == 'o')
                                   ch = 0xf6;
                               else if (ch == 'O')
                                   ch = 0xd6;
                               else if (ch == 'u')
                                   ch = 0xfc;
                               else if (ch == 'U')
                                   ch = 0xdc;
                               break;
                    case 23  : if (ch == 'a')
                                   ch = 0xe5;
                               else if (ch == 'A')      /* circle */
                                   ch = 0xc5;
                               break;
                }
            }
        }
    }
    /*----------------- end of 'Scandinavian code' ----------------*/
    if (foo == leftmargin-1)
        while ((currentline->text[j - leftmargin] != SPACE)
               && (j < rightmargin)) {
            j++;
            h += charwidth;
        }
    if ( ((ch >= SPACE) && (ch != DEL)) ||
         (scascii && (ch == 23)) ) {
          /*  (scascii && (ch == DEL)) ) {    if VMS ??? */
        if (j < rightmargin)
            currentline->text[j - leftmargin] = ch;
        else
            currentline->text[rightmargin - leftmargin] = '@';
        if (j > currentline->charactercount)
            currentline->charactercount = j;
        if (j < firstcolumn)
            firstcolumn = j;
        h += charwidth;
    }

} /* outchar */

/*----------------------------------------------------------------------------*/

void putcharacter(register long charnr)            /* output character, don't change h */
{
    register long saveh;

    saveh = h;
    if ((charnr >= 0) && (charnr <= LASTCHAR))
        outchar((char) charnr);
    else
        setchar(charnr);
    h = saveh;

} /* putcharacter */

/*----------------------------------------------------------------------------*/

void setchar(long charnr)
{    /* should print characters with character code>127 from current font */
     /* note that the parameter is a dummy, since ascii-chars are<=127    */

    outchar((unsigned char)(charnr));

} /* setchar */


/*----------------------------------------------------------------------------*/


void fontdef(register int x)
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
    name[namelen] = '\0';
    fnt->name = name;
    if (new) {
        fnt->next = fonts;
        fonts = fnt;
    }

    return;

} /* fontdef */



void setfont(long fntnum)
{
    font * fnt;
    char * s;

    fnt = fonts;
    while (fnt != NULL && fnt->num != fntnum)
        fnt = fnt->next;
    if (fnt == NULL) {
        /* error : font not found */
        symbolfont = FALSE;
        return;
    }

    s = fnt->name;
    while ((s = strchr(s, 's')) != NULL) {
        if (strncmp("sy", s, 2) == 0) {
            symbolfont = TRUE;
            return;
        }
	s++;	/* New line to fix bug; font names with 's' would hang */
    }
   
    symbolfont = FALSE;
    return;

} /* setfont */
   


/*----------------------------------------------------------------------------*/


#if defined(VMS)
long vmsseek(FILE *fp, long n, long dir)
{
    long k,m,pos,val,oldpos;
    struct stat buffer;

    for (;;) {                     /*loops only once or twice*/
        switch (dir) {
            case 0:            /*from BOF*/
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

} /* vmsseek */
        


long vms_ftell(FILE *fp)
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



long vms_ungetc(char c, FILE *fp)
{
    if ((c == EOF) && feof(fp))
        return (EOF);
    else if ((*fp)->_cnt >= 512)
        return (EOF);
    else {
        (*fp)->_cnt++;
        (*fp)->_ptr--;
        *((*fp)->_ptr) = c;
        return (c);
    }

} /*vms_ungetc */
#endif
