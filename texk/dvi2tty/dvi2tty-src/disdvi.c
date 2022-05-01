/*****************************************************************************/
/*                                                                           */
/*   disdvi  ---  disassembles TeX dvi files.                                */
/*                                                                           */
/*                                                                           */
/*   2.27 30apr2022  TexLive     support multibyte output for SET[23]        */
/*                               in pTeX dvi with PTEXENC library            */
/*   2.26 23oct2010  TexLive     support pTeX and XeTeX                      */
/*   2.25 23jan2003  M.J.E. Mol  fixed bug in copying font name (name[i])    */
/*   2.24 27may96 M.J.E. Mol     A few typecasts added                       */
/*   2.23 23jul95 M.J.E. Mol     Cleanups from duz@roxi.ez.fht-mannheim.de   */
/*   2.22 13dec90 M.J.E. Mol     Fixed bug in num(). Cleaned up code.        */
/*   2.21 03may90 M.J.E. Mol     Created usage().                            */
/*    2.2 02may90 M.J.E. Mol     Included port to VAX/VMS VAXC by            */
/*                               Robert Schneider.                           */
/*    2.1 19jan90 M.J.E. Mol     Maintain a list of fonts and                */
/*                               show fontnames in font changes.             */
/*                               Show character code when printing ligatures */
/*    2.0 23jan89 M.J.E. Mol (c) 1989                                        */
/*                                               marcel@mesa.nl              */
/*                                                                           */
/*****************************************************************************/

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

const char *disdvi = "@(#) disdvi.c  2.27 20220501 M.J.E. Mol (c) 1989-2010, marcel@mesa.nl";

/*
 * Include files
 */

#if defined(KPATHSEA)
# define NO_DEBUG 1
# include <kpathsea/config.h>
# include <kpathsea/lib.h>
#if defined(WIN32)
# include <kpathsea/variable.h>
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#if defined(WIN32)
# include <fcntl.h>
#else
# include <unistd.h>
#endif
#include <ctype.h>
#include <string.h>
#include "commands.h"
#if defined(WIN32) && defined(KPATHSEA)
#undef fopen
#define fopen fsyscp_fopen
#endif

#if defined(PTEXENC)
#include <kpathsea/config.h>
#include <kpathsea/readable.h>
#include <ptexenc/ptexenc.h>
#include <ptexenc/unicode.h>

/* internal encoding for ASCII pTeX : "euc" or "sjis" */
#define PTEX_INTERNAL_ENC  "euc"

/* internal encoding for upTeX : "uptex" */
#define UPTEX_INTERNAL_ENC  "uptex"
#endif

/*
 * Constant definitions
 */

#define LASTCHAR        127    /* max dvi character, above are commands    */

#define get1()           num(1)
#define get2()           num(2)
#define get3()           num(3)
#define get4()           num(4)
#define sget1()         snum(1)
#define sget2()         snum(2)
#define sget3()         snum(3)
#define sget4()         snum(4)



/*
 * Type definitions
 */

typedef struct _font {
    long    num;
    struct _font * next;
    char  * name;
} font;



/*
 * Variable declarations
 */

font * fonts = NULL;
FILE * dvifp;
char * dvi_name;
long   pc = 0;

char * progname;

int is_ptex = 0;
#if defined(PTEXENC)
int is_uptex = 0;
int with_ptexenc = 0;
#endif
int is_xetex = 0;
const char * dvi_ext = ".dvi";

/*
 * Function declarations
 */

void            bop             (void);
void            preamble        (void);
void            postamble       (void);
void            postpostamble   (void);
void            fontdef         (int x);
const char *    fontname        (unsigned long fntnum);
void            special         (int x);
void            printnonprint   (int ch);
unsigned long   num             (int size);
long            snum            (int size);
void            usage           (void);
void            picfile         (int opcode);
void            natfontdef      (int opcode);
void            glyphs          (int opcode);
void            dvidir          (int opcode);
void            invalid         (int opcode);

#if defined(PTEXENC)
int printch(int code, unsigned long ch0)
{
    int ch1;
    if (with_ptexenc) {
        printf("\"%lX", ch0);
        if (code>1) {
            ch1 = toBUFF(fromDVI(ch0));
            printf("  ");
            putc2('(', stdout);
            if (BYTE1(ch1) != 0) putc2(BYTE1(ch1), stdout);
            if (BYTE2(ch1) != 0) putc2(BYTE2(ch1), stdout);
            if (BYTE3(ch1) != 0) putc2(BYTE3(ch1), stdout);
            /* always */         putc2(BYTE4(ch1), stdout);
            putc2(')', stdout);
        }
    } else {
        printf("%ld", ch0);
    }
    return ch0;
} /* main */
#endif


/*
 * MAIN --
 */

int main(int argc, char **argv)
{
    register int opcode;                /* dvi opcode */
    register int i;
    unsigned long fontnum;
#if defined(PTEXENC)
    int code;
#endif

#if defined(WIN32) && defined(KPATHSEA)
    char **av, *enc;
    int ac;
    kpse_set_program_name(argv[0], "disdvi");
    enc = kpse_var_value("command_line_encoding");
    if (get_command_line_args_utf8(enc, &ac, &av)) {
        argv = av;
        argc = ac;
    }
    _setmode (fileno(stdout), _O_BINARY);
#endif

    progname = *argv++;

#if defined(PTEXENC)
    while ((argc > 1) && (*argv[0] == '-')) {
#else
    if ((argc > 1) && (*argv[0] == '-')) {
#endif
        if (!strcmp(*argv, "-h")) {
            usage();
            exit(0);
        }
        if (!strcmp(*argv, "-p")) {
            is_ptex = 1;
#if defined(PTEXENC)
            set_enc_string (NULL, PTEX_INTERNAL_ENC);
#endif
        }
#if defined(PTEXENC)
        else if (!strcmp(*argv, "-u")) {
            is_uptex = 1;
            enable_UPTEX(true);
            set_enc_string (NULL, UPTEX_INTERNAL_ENC);
        }
        else if (!strncmp(*argv, "-E", 2)) {
            with_ptexenc = 1;
            set_prior_file_enc();
            if (!strcmp(*argv, "-Ee")) set_enc_string ("euc", NULL);
            if (!strcmp(*argv, "-Es")) set_enc_string ("sjis", NULL);
            if (!strcmp(*argv, "-Ej")) set_enc_string ("jis", NULL);
            if (!strcmp(*argv, "-Eu")) set_enc_string ("utf8", NULL);
        }
#endif
        else if (!strcmp(*argv, "-x")) {
            is_xetex = 1;
            dvi_ext = ".xdv";
        } else {
            fprintf(stderr, "Invalid option `%s'\n", *argv);
            usage();
            exit(3);
        }
        argv++;
        argc--;
    }

    if (argc > 2) {
        fprintf(stderr, "Too many arguments\n");
        usage();
        exit(1);
    }

    if (argc == 2) {
        if ((i = strlen(*argv)) == 0) {
            fprintf(stderr, "Illegal empty filename\n");
            usage();
            exit(2);
        }
        if ((i >= 5) && (!strcmp(*argv+i-4, dvi_ext)))
            dvi_name = *argv;
        else {
            if ((dvi_name = malloc((i+5) * sizeof(char))) == NULL) {
                perror("dvi_name");
                exit(1);
            }
            strcpy(dvi_name, *argv);
            strcat(dvi_name, dvi_ext);
        }
        if ((dvifp = fopen(dvi_name, "r")) == NULL) {
            perror(dvi_name);
            exit(3);
        }
    }
    else
        dvifp = stdin;

#if defined(WIN32)
    setmode(fileno(dvifp), _O_BINARY);
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
                                    fontname((unsigned long) opcode - FONT_00));
        else
            switch (opcode) {
                case SET1     :
                case SET2     :
                case SET3     :
                case SET4     :
#if defined(PTEXENC)
                                code = opcode - SET1 + 1;
                                printf("SET%d:     ", code);
                                printch(code, num(code));
                                printf("\n");
#else
                                printf("SET%d:     %ld\n", opcode - SET1 + 1,
                                                       num(opcode - SET1 + 1));
#endif
                                break;
                case SET_RULE : printf("SET_RULE: height: %ld\n", sget4());
                                printf("%06ld: ", pc);
                                printf("          length: %ld\n", sget4());
                                break;
                case PUT1     :
                case PUT2     :
                case PUT3     :
                case PUT4     :
#if defined(PTEXENC)
                                code = opcode - PUT1 + 1;
                                printf("PUT%d:     ", code);
                                printch(code, num(code));
                                printf("\n");
#else
                                printf("PUT%d:     %ld\n", opcode - PUT1 + 1,
                                                       num(opcode - PUT1 + 1));
#endif
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
                                printf("FNT%d:     %ld    /* %s */\n",
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
                case PIC_FILE : picfile(opcode);                break;
                case NAT_FNT  : natfontdef(opcode);             break;
                case SET_GL_AR:
                case SET_GL_ST: glyphs(opcode);                 break;
                case DVI_DIR  : dvidir(opcode);                 break;
                default       : invalid(opcode);
            }
    }

    exit(0);

} /* main */



/*
 * BOP -- Process beginning of page.
 */

void bop(void)
{
    int i;

    printf("BOP       page number      : %ld", sget4());
    for (i=9; i > 0; i--) {
        if (i % 3 == 0)
            printf("\n%06ld:         ", pc);
        printf("  %6ld", sget4());
    }
    printf("\n%06ld: ", pc);
    printf("          prev page offset : %06ld\n", sget4());

    return;

} /* bop */



/*
 * POSTAMBLE -- Process post amble.
 */

void postamble(void)
{

    printf("POST      last page offset : %06ld\n", sget4());
    printf("%06ld: ", pc);
    printf("          numerator        : %lu\n", get4());
    printf("%06ld: ", pc);
    printf("          denominator      : %lu\n", get4());
    printf("%06ld: ", pc);
    printf("          magnification    : %lu\n", get4());
    printf("%06ld: ", pc);
    printf("          max page height  : %lu\n", get4());
    printf("%06ld: ", pc);
    printf("          max page width   : %lu\n", get4());
    printf("%06ld: ", pc);
    printf("          stack size needed: %d\n", (int) get2());
    printf("%06ld: ", pc);
    printf("          number of pages  : %d\n", (int) get2());

    return;

} /* postamble */



/*
 * PREAMBLE -- Process pre amble.
 */

void preamble(void)
{
    register int i;

    printf("PRE       version          : %d\n", (int) get1());
    printf("%06ld: ", pc);
    printf("          numerator        : %lu\n", get4());
    printf("%06ld: ", pc);
    printf("          denominator      : %lu\n", get4());
    printf("%06ld: ", pc);
    printf("          magnification    : %lu\n", get4());
    printf("%06ld: ", pc);
    i = (int) get1();
    printf("          job name (%3d)   :", i);
    while (i-- > 0)
        putchar((int) get1());
    putchar('\n');

    return;

} /* preamble */



/*
 * POSTPOSTAMBLE -- Process post post amble.
 */

void postpostamble(void)
{
    register int i;
 
    printf("POSTPOST  postamble offset : %06lu\n", get4());
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

    return;

} /* postpostamble */



/*
 * SPECIAL -- Process special opcode.
 */

void special(int x)
{
    register long len;

    len = num(x);
    printf("XXX%d:     %ld bytes\n", x, len);
    printf("%06ld: ", pc);
    for (; len>0; len--)           /* a bit dangerous ... */
        putchar((int) get1());     /*   can be non-printables */
    putchar('\n');

    return;

} /* special */



/*
 * FONTDEF -- Process a font definition.
 */

void fontdef(int x)
{
    register int i;
    char * name;
    font * fnt;
    int namelen;
    unsigned long fntnum;
    int new = 0;

    fntnum = num(x);
    printf("FNT_DEF%d: %ld\n", x, fntnum);
    printf("%06ld: ", pc);           /* avoid side-effect on pc in get4() */
    printf("          checksum         : %lu\n", get4());
    printf("%06ld: ", pc);
    printf("          scale            : %lu\n", get4());
    printf("%06ld: ", pc);
    printf("          design           : %lu\n", get4());
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
    name[i] = '\0';
    fnt->name = name;
    if (new) {
        fnt->next = fonts;
        fonts = fnt;
    }

    printf("%s\n", name);

    return;

} /* fontdef */



/*
 * FONTNAME -- Return font name.
 */

const char * fontname(unsigned long fntnum)
{
    font * fnt;

    fnt = fonts;
    while (fnt != NULL && fnt->num != fntnum)
        fnt = fnt->next;
    if (fnt != NULL)
        return fnt->name;

    return "unknown fontname";

} /* fontname */



/*
 * PRINTNONPRINT -- Translate non-printable characters.
 */

void printnonprint(int ch)
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
        case 26  :  printf("ae         /* scandinavian (non-printing) 0x%02X */",
                           ch);
                    break;
        case 27  :  printf("oe         /* scandinavian (non-printing) 0x%02X */",
                           ch);
                    break;
        case 28  :  printf("o          /* scandinavian (non-printing) 0x%02X */",
                           ch);
                    break;
        case 29  :  printf("AE         /* scandinavian (non-printing) 0x%02X */",
                           ch);
                    break;
        case 30  :  printf("OE         /* scandinavian (non-printing) 0x%02X */",
                           ch);
                    break;
        case 31  :  printf("O          /* scandinavian (non-printing) 0x%02X */",
                           ch);
                    break;
        default  :  printf("0x%02X", ch); break;
    }
    putchar('\n');

    return;

} /* printnonprint */



/*
 * NUM --
 */

unsigned long num(int size)
{
    register int i;
    register unsigned long x = 0;

    pc += size;
    for (i = size; i > 0; i--)
        x = (x << 8) + (unsigned) getc(dvifp);

    return x;

} /* num */



/*
 * SNUM --
 */

long snum(int size)
{
    register int i;
    register long x;

    pc += size;
    x = getc(dvifp);
    if (x & 0x80)
        x -= 0x100;
    for (i = size - 1; i > 0; i--)
        x = (x << 8) + (unsigned) getc(dvifp);

    return x;

} /* snum */



/*
 * USAGE --
 */

void usage(void)
{

    fprintf(stderr, "\n%s\n\n", disdvi);
    fprintf(stderr, "    disassembles (p)TeX dvi and XeTeX xdv files\n");
#if defined(PTEXENC)
    fprintf(stderr, "Usage: %s [-h | [-p] [-u] [-Eenc] [dvi_file[.dvi]]\n", progname);
#else
    fprintf(stderr, "Usage: %s [-h | [-p] [dvi_file[.dvi]]\n", progname);
#endif
    fprintf(stderr, "              | -x [xdv_file[.xdv]]]\n");
    fprintf(stderr, "Options:\n");
#if defined(PTEXENC)
    fprintf(stderr, "   -p     Support ASCII pTeX dvi\n");
    fprintf(stderr, "   -u     Support upTeX dvi\n");
    fprintf(stderr, "   -x     Support XeTeX xdv\n");
    fprintf(stderr, "   -Eenc  Output multibyte encoding. The enc argument denotes u:UTF8 e:EUC-JP s:Shift_JIS j:JIS\n");
#else
    fprintf(stderr, "   -p     Support ASCII pTeX dvi\n");
    fprintf(stderr, "   -x     Support XeTeX xdv\n");
#endif
    fprintf(stderr, "   -h     This help message.\n");
    fprintf(stderr, "\n If you like this code and want to support is feel free\n to donate at Paypal marcel@mesa.nl. Thanks.\n\n");


    return;

} /* usage */



void picfile(int opcode)
{
    int i;

    if (!is_xetex) {
        invalid(opcode);
        return;
    }

    printf("PIC_FILE  flags            : %ld\n", get1());
    printf("%06ld:           trans :", pc);
    for (i=0; i<6; i++)
        printf(" %ld", sget4());
    printf("\n%06ld: ", pc);
    printf("          page              : %ld\n", get2());
    printf("%06ld: ", pc);
    i = (int) get1();
    printf("          path name (%3d)   :", i);
    while (i-- > 0)
        putchar((int) get1());
    putchar('\n');
} /* picfile */



void natfontdef(int opcode)
{
    register int i;
    char * name;
    font * fnt;
    int flags, namelen, famlen, stylen;
    long fntnum;
    int new = 0;

    if (!is_xetex) {
        invalid(opcode);
        return;
    }

    fntnum = num(4);
    printf("NAT_FNT:  %ld\n", fntnum);
    printf("%06ld: ", pc);
    printf("          scale            : %lu\n", get4());
    printf("%06ld: ", pc);
    flags = get2();
    printf("          flags            : %d\n", flags);
    printf("%06ld: ", pc);
    printf("          name             : ");
    namelen = (int) get1();
    famlen = (int) get1();
    stylen = (int) get1();
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

    if (famlen) {
        printf("                  family           : ");
        while (famlen-- > 0)
            putchar((int) get1());
        putchar('\n');
    }

    if (stylen) {
        printf("                  style            : ");
        while (stylen-- > 0)
            putchar((int) get1());
        putchar('\n');
    }
} /* fontdef */



void glyphs(int opcode)
{
    long width;
    int x, n, i, j;
    long * xy;

    if (!is_xetex) {
        invalid(opcode);
        return;
    }

    x = SET_GL_ST - opcode + 1;
    width = sget4();
    n = get2();
    printf("GLYPH_%s width            : %ld\n", x == 2 ? "ARR" : "STR", width);
    if ((xy = (long *) malloc(n * x * sizeof(long))) == NULL) {
        perror("glyphs");
        exit(1);
    }
    for (j=0; j < n * x; j++)
        xy[j] = sget4();
    for (i=0, j=0; i < n; i++) {
        printf("           x: %ld", xy[j++]);
        if (x == 2)
            printf("    y: %ld", xy[j++]);
        printf("    g: %ld\n", get2());
    }
} /* glyphs */



void dvidir(int opcode)
{
#if defined(PTEXENC)
    if (!is_ptex && !is_uptex) {
#else
    if (!is_ptex) {
#endif
        invalid(opcode);
        return;
    }

    printf("DVI_DIR:  %ld\n", get1());
} /* dvidir */



void invalid(int opcode)
{
    printf("INVALID   %d\n", opcode);
} /* invalid */
