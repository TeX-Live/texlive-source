/*****************************************************************************/
/*                                                                           */
/*   disdvi  ---  disassembles TeX dvi files.                                */
/*                                                                           */
/*                                                                           */
/*    2.0 23/01/89 M.J.E. Mol (c) 1989              marcel@duteca.tudelft.nl */
/*    2.1 19/01/90 M.J.E. Mol    Maintain a list of fonts and                */
/*                               show fontnames in font changes.             */
/*                               Show character code when printing ligatures */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/


#if 0
char *disdvi = "@(#) disdvi.c  2.1 19/01/90 M.J.E. Mol (c) 1989, 1990";
#endif

#include <stdio.h>
#include <ctype.h>
#include "commands.h"

#if defined(KPATHSEA)
#define NO_DEBUG 1
#include <kpathsea/config.h>
#include <kpathsea/lib.h>
#else
#if defined(MSDOS)
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#endif
#endif

#define LASTCHAR        127    /* max dvi character, above are commands    */

#define get1()           num(1)
#define get2()           num(2)
#define get3()           num(3)
#define get4()           num(4)
#define sget1()         snum(1)
#define sget2()         snum(2)
#define sget3()         snum(3)
#define sget4()         snum(4)

typedef struct _font {
    long    num;
    struct _font * next;
    char  * name;
} font;

font * fonts = NULL;
FILE * dvifp;
char * dvi_name;
long   pc = 0;

#ifdef KPATHSEA
extern int main(int argc,char **argv );
extern void bop(void);
extern void postamble(void);
extern void preamble(void);
extern void postpostamble(void);
extern void special(int x);
extern void fontdef(int x);
extern const char *fontname(long fntnum);
extern void printnonprint(int ch);
extern unsigned long num(int size);
extern long snum(int size);
#else
char *          malloc          ();

int             main            ();
void            bop             ();
void            preamble        ();
void            postamble       ();
void            postpostamble   ();
void            fontdef         ();
char *          fontname        ();
void            special         ();
void            printnonprint   ();
unsigned long   num             ();
long            snum            ();
#endif



/*---------------------------------------------------------------------------*/

int main(int argc, char **argv)
{
    register int opcode;                /* dvi opcode                        */
    register int i;
    int fontnum;

    if (argc > 2) {
        fprintf(stderr, "To many arguments\n");
        fprintf(stderr, "Usage: %s [dvi-file]\n", *argv);
        exit(1);
    }

    if (argc == 2) {
        if ((i = strlen(argv[1])) == 0) {
            fprintf(stderr, "Illegal empty filename\n");
            fprintf(stderr, "Usage: %s [dvi-file]\n", *argv);
            exit(2);
        }
        if ((i >= 5) && (argv[1][i-4] == '.') && (argv[1][i-3] == 'd') &&
              (argv[1][i-2] == 'v') && (argv[1][i-1] == 'i'))
            dvi_name = argv[1];
        else {
            dvi_name = malloc((i+5) * sizeof(char));
            strcpy(dvi_name, argv[1]);
            strcat(dvi_name, ".dvi");
        }
        if ((dvifp = fopen(dvi_name, "r")) == NULL) {
            perror(dvi_name);
            exit(3);
        }
    }
    else
        dvifp = stdin;

#if defined(MSDOS)
    setmode(fileno(dvifp), O_BINARY);
#endif

    while ((opcode = (int) get1()) != EOF) {    /* process until end of file */
        printf("%06ld: ", pc - 1);
        if ((opcode <= LASTCHAR) && isprint(opcode)) {
            printf("Char:     ");
            while ((opcode <= LASTCHAR) && isprint(opcode)) {
                putchar(opcode);
                opcode = (int) get1();
            }
            putchar('\n');
            printf("%06ld: ", pc - 1);
        }

        if (opcode <= LASTCHAR) 
            printnonprint(opcode);              /* it must be a non-printable */
        else if ((opcode >= FONT_00) && (opcode <= FONT_63)) 
            printf("FONT_%02d              /* %s */\n", opcode - FONT_00,
                                    fontname(opcode - FONT_00));
        else
            switch (opcode) {
                case SET1     :
                case SET2     : 
                case SET3     :
                case SET4     : printf("SET%d:    %ld\n", opcode - SET1 + 1,
                                                       num(opcode - SET1 + 1));
                                break;
                case SET_RULE : printf("SET_RULE: height: %ld\n", sget4());
                                printf("%06ld: ", pc);
                                printf("          length: %ld\n", sget4());
                                break;
                case PUT1     :
                case PUT2     :
                case PUT3     :
                case PUT4     : printf("PUT%d:     %ld\n", opcode - PUT1 + 1,
                                                       num(opcode - PUT1 + 1));
                                break;
                case PUT_RULE : printf("PUT_RULE: height: %ld\n", sget4());
                                printf("%06ld: ", pc);
                                printf("          length: %ld\n", sget4());
                                break;
                case NOP      : printf("NOP\n");  break;
                case BOP      : bop();            break;
                case EOP      : printf("EOP\n");  break;
                case PUSH     : printf("PUSH\n"); break;
                case POP      : printf("POP\n");  break;
                case RIGHT1   :
                case RIGHT2   : 
                case RIGHT3   : 
                case RIGHT4   : printf("RIGHT%d:   %ld\n", opcode - RIGHT1 + 1,
                                                     snum(opcode - RIGHT1 + 1));
                                break;
                case W0       : printf("W0\n");   break;
                case W1       : 
                case W2       :
                case W3       :
                case W4       : printf("W%d:       %ld\n", opcode - W0,
                                                      snum(opcode - W0));
                                break;
                case X0       : printf("X0\n");   break;
                case X1       :
                case X2       :
                case X3       :
                case X4       : printf("X%d:       %ld\n", opcode - X0,
                                                      snum(opcode - X0));
                                break;
                case DOWN1    : 
                case DOWN2    : 
                case DOWN3    :
                case DOWN4    : printf("DOWN%d:    %ld\n", opcode - DOWN1 + 1,
                                                      snum(opcode - DOWN1 + 1));
                                break;
                case Y0       : printf("Y0\n");   break;
                case Y1       :
                case Y2       :
                case Y3       :
                case Y4       : printf("Y%d:       %ld\n", opcode - Y0,
                                                      snum(opcode - Y0));
                                break;
                case Z0       : printf("Z0\n");   break;
                case Z1       :
                case Z2       :
                case Z3       : 
                case Z4       : printf("Z%d:       %ld\n", opcode - Z0,
                                                      snum(opcode - Z0));
                                break;
                case FNT1     :
                case FNT2     :
                case FNT3     :
                case FNT4     : fontnum = num(opcode -FNT1 + 1);
                                printf("FNT%d:     %d    /* %s */\n",
                                       opcode - FNT1 + 1, fontnum,
                                       fontname(fontnum));
                                break;
                case XXX1     : 
                case XXX2     : 
                case XXX3     :
                case XXX4     : special(opcode - XXX1 + 1);     break;
                case FNT_DEF1 :
                case FNT_DEF2 :
                case FNT_DEF3 :
                case FNT_DEF4 : fontdef(opcode - FNT_DEF1 + 1); break;
                case PRE      : preamble();                     break;
                case POST     : postamble();                    break;
                case POST_POST: postpostamble();                break;
            }
    }

    return 0;
} /* main */


/*----------------------------------------------------------------------------*/


void bop(void)
{
    int i;

    printf("BOP       page number      : %ld", sget4());
    for (i=0; i < 9; i++) {
        if (i % 3 == 0)
            printf("\n%06ld:         ", pc);
        printf("  %6ld", sget4()); 
    }
    printf("\n%06ld: ", pc);
    printf("          prev page offset : %06ld\n", sget4()); 

} /* bop */


/*---------------------------------------------------------------------------*/

void postamble(void) 
{

    printf("POST      last page offset : %06ld\n", sget4());
    printf("%06ld: ", pc);
    printf("          numerator        : %ld\n", get4());
    printf("%06ld: ", pc);
    printf("          denominator      : %ld\n", get4());
    printf("%06ld: ", pc);
    printf("          magnification    : %ld\n", get4());
    printf("%06ld: ", pc);
    printf("          max page height  : %ld\n", get4());
    printf("%06ld: ", pc);
    printf("          max page width   : %ld\n", get4());
    printf("%06ld: ", pc);
    printf("          stack size needed: %d\n", (int) get2());
    printf("%06ld: ", pc);
    printf("          number of pages  : %d\n", (int) get2());

} /* postamble */

void preamble(void)
{
    register int i;

    printf("PRE       version          : %d\n", (int) get1());
    printf("%06ld: ", pc);
    printf("          numerator        : %ld\n", get4());
    printf("%06ld: ", pc);
    printf("          denominator      : %ld\n", get4());
    printf("%06ld: ", pc);
    printf("          magnification    : %ld\n", get4());
    printf("%06ld: ", pc);
    i = (int) get1();
    printf("          job name (%3d)   :", i);
    while (i-- > 0)
        putchar((int) get1());
    putchar('\n');

} /* preamble */


void postpostamble(void)
{
    register int i;
 
    printf("POSTPOST  postamble offset : %06ld\n", get4());
    printf("%06ld: ", pc);
    printf("          version          : %d\n", (int) get1());
    while ((i = (int) get1()) == TRAILER) {
        printf("%06ld: ", pc - 1);
        printf("TRAILER\n");
    }
    while (i != EOF) {
        printf("%06ld: ", pc - 1);
        printf("BAD DVI FILE END: 0x%02X\n", i);
        i = (int) get1();
    }

} /* postpostamble */



void special(register int x)
{
    register long len;
    register long i;

    len = num(x);
    printf("XXX%d:     %ld bytes\n", x, len);
    printf("%06ld: ", pc);
    for (i = 0; i < len; i++)      /* a bit dangerous ... */
        putchar((int) get1());     /*   can be non-printables */
    putchar('\n');

} /* special */



void fontdef(register int x)
{
    register int i;
    char * name;
    font * fnt;
    int namelen;
    long fntnum;
    int new = 0;

    fntnum = num(x);
    printf("FNT_DEF%d: %ld\n", x, fntnum);
    printf("%06ld: ", pc);           /* avoid side-effect on pc in get4() */
    printf("          checksum         : %ld\n", get4());
    printf("%06ld: ", pc);
    printf("          scale            : %ld\n", get4());
    printf("%06ld: ", pc);
    printf("          design           : %ld\n", get4());
    printf("%06ld: ", pc);
    printf("          name             : ");
    namelen = (int) get1() + (int) get1();
    fnt = fonts;
    while (fnt != NULL && fnt->num != fntnum)
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

    printf("%s\n", name);

} /* fontdef */



const char *
fontname(long fntnum)
{
    font * fnt;

    fnt = fonts;
    while (fnt != NULL && fnt->num != fntnum)
        fnt = fnt->next;
    if (fnt != NULL)
        return fnt->name;
    else
        return "unknown fontname";
   
} /* fontname */



void printnonprint(register int ch)
{

    printf("Char:     ");
    switch (ch) {
        case 11  :  printf("ff         /* ligature (non-printing) 0x%02X */",
                           ch);
                    break;
        case 12  :  printf("fi         /* ligature (non-printing) 0x%02X */",
                           ch);
                    break;
        case 13  :  printf("fl         /* ligature (non-printing) 0x%02X */",
                           ch);
                    break;
        case 14  :  printf("ffi        /* ligature (non-printing) 0x%02X */",
                           ch);
                    break;
        case 15  :  printf("ffl        /* ligature (non-printing) 0x%02X */",
                           ch);
                    break;
        case 16  :  printf("i          /* (non-printing) 0x%02X */", ch);
                    break;
        case 17  :  printf("j          /* (non-printing) 0x%02X */", ch);
                    break;
        case 25  :  printf("ss         /* german (non-printing) 0x%02X */", ch);
                    break;
        case 26  :  printf("ae         /* scadinavian (non-printing) 0x%02X */",
                           ch);
                    break;
        case 27  :  printf("oe         /* scadinavian (non-printing) 0x%02X */",
                           ch);
                    break;
        case 28  :  printf("o          /* scadinavian (non-printing) 0x%02X */",
                           ch);
                    break;
        case 29  :  printf("AE         /* scadinavian (non-printing) 0x%02X */",
                           ch);
                    break;
        case 30  :  printf("OE         /* scadinavian (non-printing) 0x%02X */",
                           ch);
                    break;
        case 31  :  printf("O          /* scadinavian (non-printing) 0x%02X */",
                           ch);
                    break;
        default  :  printf("0x%02X", ch); break;
    }
    putchar('\n');

}



unsigned long num(register int size)
{
    register int i;
    register long x = 0;

    pc += size;
    for (i = 0; i < size; i++)
        x = (x << 8) + (unsigned) getc(dvifp);
    return x;

} /* num */



long snum(register int size)
{
    register int i;
    register long x = 0;

    pc += size;
    x = getc(dvifp);
    if (x & 0x80)
        x -= 0x100;
    for (i = 1; i < size; i++)
        x = (x << 8) + (unsigned) getc(dvifp);
    return x;

} /* snum */



