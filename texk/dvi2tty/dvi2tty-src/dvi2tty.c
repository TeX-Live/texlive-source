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

#include "c-auto.h"
#include "dvi2tty.h"

#if defined(WIN32) && defined(KPATHSEA)
#undef fopen
#define fopen fsyscp_fopen
#endif


/* 
 * Constant definitons
 */
    /*-----------------------------------------------------------------------*/
    /* The following constants may be toggled before compilation to          */
    /* customize the default behaviour of the program for your site.         */
    /* Whichever their settings are, the defaults can be overridden at       */
    /* runtime.                                                              */
    /*-----------------------------------------------------------------------*/

#define DEFSCAND    FALSE     /* default is not Scandinavian, toggle this if */
                              /* you have terminals with Scand. nat. chars */
#define DEFLATIN1   FALSE     /* default is no latin1, toggle this if you */
                              /* have terminals with latin1 chars */
#define WANTPAGER   TRUE      /* default: try to pipe through a pager (like  */
                              /* more) if stdout is tty and no -o switch     */
#define DEFPAGER    "more"   /* CHANGE TO YOUR LOCAL PAGER            */

    /*------------------ end of customization constants ---------------------*/

#define OPTSET      "haJweEpPousqlfFtvbcANUC" /* legal options               */
#define OPTWARG     "weEpPoFvb"     /* options with argument                 */

/*
 * USAGE CODES
 */

#define wrnge  1                /* width switch arg out of range     */
#define ign    2                /* ignore cause, print 'Usage:..'    */
#define nan    3                /* not a number where one expected   */
#define gae    4                /* garbage at end                    */
#define bdlst  5                /* bad page-numberlist               */
#define onef   6                /* only one dvifile allowed          */
#define bdopt  7                /* bad option                        */
#define onepp  8                /* only one page list allowed        */
#define noarg  9                /* argument expected                 */
#define confl  10               /* -J, -N, -A, and -U conflict       */
#define incone 11               /* inconsistent output encoding      */


/*
 * Variable definitions
 */

const char *dvi2tty = "@(#) dvi2tty.c " VERSION " 20220501 M.J.E. Mol (c) 1989-2010, and contributors (c) -2022";


printlisttype * currentpage;    /* current page to print                     */
printlisttype * firstpage;      /* first page selected                       */
printlisttype * lastpage;       /* last page selected                        */

FILE *          DVIfile;
FILE *          output;
bool            outputtofile;   /* tells if output goes to file or stdout    */
int             ttywidth;       /* max nr of chars per printed line          */
int             espace;         /* to fake calcs with ttywidth               */

long            foo;            /* utility variable, "register"              */
bool            pager;          /* tells if output is piped to a pager       */
const char  *   path;           /* name of the pager to run                  */
char  *         progname;       /* our name                                  */
int             Argc;
char **         Argv;
char  *         DVIfilename;
const char *    OUTfilename;
char            optch;          /* for option handling                       */



/*
 * Function declarations
 */

void    setoption (const char * optarg);
void    getargs   (void);
void    getpages  (int j, const char * str);
void    plcnxt    (int pagnr);
void    getfname  (const char * str);
int     getinteger(int * dest, int * j, const char * str);
int     getlong   (long * dest, int * j, const char * str);
void    usage     (int uerr);


/****************************************************************************/
/*                                                                          */
/*                                 M A I N                                  */
/*                                                                          */
/****************************************************************************/

int main(int argc, char **argv)
{
#if defined(WIN32) && defined(KPATHSEA)
    char *enc;
#endif

    progname = *argv;
    Argc = argc;
    Argv = argv;
#if defined(WIN32) && defined(KPATHSEA)
    kpse_set_program_name(argv[0], "dvi2tty");
    enc = kpse_var_value("command_line_encoding");
    get_command_line_args_utf8(enc, &Argc, &Argv);
#endif

#ifdef WIN32
    set_enc_string ("sjis", "default");
    _setmode (fileno(stdout), _O_BINARY);
#else
    set_enc_string (NULL, "default");
#endif
    getargs();                              /* read command line arguments   */
#if defined(WIN32)
    if ((DVIfile = fopen(DVIfilename, "rb")) == NULL)
#else
    if ((DVIfile = fopen(DVIfilename, "r")) == NULL)
#endif
        errorexit(filop);                   /* can't open dvifile            */

    if (outputtofile) {                     /* open the outfile, if needed   */
#if defined(WIN32)
        if ((output = fopen(OUTfilename, "wb")) == NULL)
#else
        if ((output = fopen(OUTfilename, "w")) == NULL)
#endif
            errorexit(filcr);

        pager = FALSE;
    }
    else {
        output = stdout;
        if (pager && isatty(fileno(output))) {   /* try to pipe to a pager   */
            if ((output = popen(path, "w")) == NULL) /* get pipe to pager    */
                errorexit(pipcr);                /* make output to output    */
        }
        else
            pager = FALSE;
    }

    dvimain();

    if (pager)
        pclose(output);                     /* close pipe to pager            */

    exit(0);

} /* main */



/*
 * GETARGS -- Process options from command line and from environment.
 */

void getargs(void)
{
    const char *str;
    char *envp;
    bool DVIfound;                      /* if a dvi filename found           */

    if (Argc <= 1)
        usage(ign);

    pageswitchon = FALSE;       /* show all pages                            */
    sequenceon   = FALSE;       /* selected pages are TeX-numbered           */
    outputtofile = FALSE;       /* write to stdout                           */
    pager        = WANTPAGER;   /* want paging, compile time option          */
    accent       = TRUE;        /* show all accent etc. as extra char        */
    ttfont       = FALSE;       /* assume tt font (verbatim mode)            */
    noffd        = FALSE;       /* print formfeed between pages              */
    scascii      = DEFSCAND;    /* scandinavian, compile time option         */
    latin1       = DEFLATIN1;   /* latin1 support, compile time option       */
    utf8         = FALSE;       /* print by utf encoding                     */
    noligaturefi = FALSE;       /* do not use ligature for ff,fi,fl,ffi,ffl  */
    ttywidth     = 80;          /* default terminal width                    */
    espace       = 0;           /* to fake ttywith calcs                     */
    DVIfound     = FALSE;
    printfont    = FALSE;       /* do not print font switches                */
    compose      = TRUE;        /* try to compose a combining character sequence */
    allchar      = FALSE;       /* do not put out all characters             */

    if ((path = getenv("PAGER")) == NULL)   /* find default pathname of page */
            path = DEFPAGER;             /* program in case paging is wanted */

    /*
     * First process environment variable.
     */

    if ((envp = getenv("DVI2TTY")) != NULL) { /* } keep vi happy */
        while (*envp == ' ')
             envp++;
        while (*envp) {                     /* environment var args          */
            if (strchr(OPTSET, optch = *envp++) != NULL) {
                /*
                 * we always pass one option, and arrange for optarg ourselfes,
                 * so setoption does not mesh up Argv
                 */
                if (strchr(OPTWARG, optch) != NULL) {
                    while (*envp == ' ')
                        envp++;
                    if (*envp == '\0')
                        usage(noarg);
                    str = envp;             /* str points to optarg          */
                    while ((*envp != ' ') && (*envp != '\0'))
                        envp++;             /* set envp just after optarg    */
                    if (*envp != '\0')
                        *envp++ = '\0';     /* end optarg string             */
                }
                else
                    str = "";
                setoption(str);
            }
            else
                usage(bdopt);
            while (*envp == ' ')
                 envp++;
        }
    }

    /*
     * Now process command line options.
     */

    while (--Argc > 0) {                    /* command line args             */
        str = *++Argv;
        if (*str != '-') {                  /* argument is not an option     */
            if (DVIfound)                   /* only one dvi file allowed     */
                usage(onef);
            getfname(str);
            DVIfound = TRUE;
        }
        else if (strchr(OPTSET, optch = *++str) != NULL) {
            str++;                      /* point to rest of argument if any  */
            if ((strchr(OPTWARG, optch) != NULL) && (*str == '\0')) {
                if (--Argc <= 0)
                    usage(noarg);
                str = *++Argv;
            }
            setoption(str);
        }
        else
            usage(bdopt);
    }

    if (!DVIfound)
        usage(ign);

    return;

} /* getargs */



/*
 * SETOPTION -- Process an option.
 */

void setoption(const char *optarg)
{
    int j = 0;

    while (strchr(OPTSET, optch) != NULL) {
        switch (optch) {
            case 'h' : usage(ign); break;
            case 'q' : pager = FALSE; break;
            case 'f' : pager = TRUE; break;
            case 'F' : pager = TRUE;
                       path = optarg;
                       j = strlen(optarg);
                       break;
            case 'J' : jautodetect  = TRUE; break;
            case 'U' : uptex   = TRUE; japan = TRUE;    /* upTeX */
                       jautodetect = FALSE;
                       enable_UPTEX(true);
                       set_enc_string (NULL, UPTEX_INTERNAL_ENC);
                       break;
            case 'A' : asciip  = TRUE; japan = TRUE;    /* ASCII pTeX */
                       jautodetect = FALSE;
                       set_enc_string (NULL, PTEX_INTERNAL_ENC);
                       break;
            case 'N' : nttj    = TRUE; japan = TRUE;    /* NTT jTeX */
                       jautodetect = FALSE;
                       set_enc_string (NULL, JTEX_INTERNAL_ENC);
                       break;
            case 'E' :
                switch (optarg[0]) {
                       case 'e' :
                           set_enc_string ("euc", NULL);  break;
                       case 's' :
                           set_enc_string ("sjis", NULL); break;
                       case 'j' :
                           set_enc_string ("jis", NULL);  break;
                       case 'u' :
                           utf8 = TRUE;
                           set_enc_string ("utf8", NULL);
                           if (optarg[1]=='1') {
                               noligaturefi = TRUE; j++;
                           }
                           break;
                       default :
                           usage(noarg);
                }
                       j++;
                       break;
            case 't' : ttfont  = TRUE; break;
            case 'l' : noffd   = TRUE; break;
            case 's' : scascii ^= 1; break;
            case 'u' : latin1  ^= 1; break;
            case 'a' : accent  = FALSE; break;
            case 'C' : compose = FALSE; break;
            case 'c' : allchar = TRUE; break;
            case 'P' : sequenceon = TRUE;     /* fall through */
            case 'p' : if (pageswitchon)
                           usage(onepp);
                       getpages(j, optarg);
                       break;
            case 'w' : if (getinteger(&ttywidth, &j, optarg))
                           usage(nan);
                       if (optarg[j] != '\0')
                           usage(gae);
                       if ((ttywidth < 16) || (ttywidth > MAXTERMWIDTH))
                           usage(wrnge);
                       break;
            case 'e' : if (getinteger(&espace, &j, optarg))
                           usage(nan);
                       if (optarg[j] != '\0')
                           usage(gae);
                       break;
            case 'v' : if (getlong(&lineheight, &j, optarg))
                           usage(nan);
                       if (optarg[j] != '\0')
                           usage(gae);
                       /* lineheight *= 65536L; */
                          /* want to specify in pt, but have no getfloat */
                       break;
            case 'o' : OUTfilename = optarg;
                       outputtofile = TRUE;
                       j = strlen(optarg);
                       break;
            case 'b' : printfont = TRUE;
                       delim = optarg;
                       if (!strlen(optarg))
                           printfont = FALSE;
                       break;
            default  : usage(bdopt);
        }
        if ((optch = optarg[j++]) == '\0')
            break;
        if ( (optarg[j] == '\0') && (strchr(OPTWARG, optch) != NULL) ) {
                if (--Argc <= 0)
                    usage(noarg);
                optarg = *++Argv;
                j = 0;
            }
    }

    /* Option conflict */
    if ((asciip && uptex) ||
        (nttj && (asciip || uptex)) ||
        (jautodetect && (nttj || asciip || uptex))) {
        usage(confl);
    }
    if (((jautodetect || asciip || uptex || nttj || utf8)
          && (scascii || latin1)) ||
        (scascii && latin1)) {
        usage(incone);
    }

    return;

} /* setoption */



/*
 * GETPAGES -- Getr a list of pages to print.
 */

void getpages(int j, const char *str)
{
    int i, c;
    int num;

    pageswitchon = TRUE;
    if ((firstpage = (printlisttype *) malloc(sizeof(printlisttype))) == NULL) {
        perror("firstpage");
        exit(1);
    }
    firstpage->all = FALSE;
    firstpage->nxt = nil;
    firstpage->pag = 0;
    lastpage = firstpage;
    currentpage = firstpage;
    if (getinteger(&num, &j, str))
        usage(nan);
    plcnxt((int) num);
    while (str[j]) {
        c = str[j];
        if (c == ',' || c == ':') {
            j++;
            if (getinteger(&num, &j, str))
                usage(nan);
        }
        else
            break;
        if (c == ',')
            plcnxt(num);
        else {
            if (currentpage->pag < 0) {
                if (num > 0) {
                    currentpage->all = TRUE;
                    plcnxt(num);
                }
                else if (num < currentpage->pag)
                    for (i = currentpage->pag - 1; i >= num; i--)
                        plcnxt(i);
                else
                    usage(bdlst);
            }
            else {
                if (num < currentpage->pag)
                    usage(bdlst);
                for (i = currentpage->pag + 1; i <= num; i++)
                    plcnxt(i);
            }
        }
    }
    if ((str[j] != ' ') && (str[j] != '\0')) {
        usage(gae);
    }
    currentpage = firstpage;

    return;

} /* getpages */



/*
 * PLCNXT -- Place page-nr next in list.
 */

void plcnxt(int pagnr)
{

    currentpage = lastpage;
    currentpage->pag = pagnr;
    if ((lastpage = (printlisttype *) malloc(sizeof(printlisttype))) == NULL) {
        perror("lastpage");
        exit(1);
    }
    lastpage->all = FALSE;
    lastpage->nxt = nil;
    lastpage->pag = 0;
    currentpage->nxt = lastpage;

    return;

} /* plcnxt */



/*
 * GETFNAME -- {Make sure we have a .dvi filename.
 */

void getfname(const char *str)
{
    int   i;

    i = strlen(str);
    if (i == 0)
        usage(ign);
    if ((DVIfilename = (char *) malloc(i+5)) == NULL) {
        perror("DVIfilename");
        exit(1);
    }
    strcpy(DVIfilename, str);
#ifdef KPATHSEA
    if (!kpse_readable_file(DVIfilename))
#else
    if ((i < 5) || strcmp(str+i-4, ".dvi"))
#endif
        strcat(DVIfilename, ".dvi");

    return;

} /* getfname */




/*
 * GETINTEGER -- Convert ascii to an integer. I'm sure there is a library
 *               call for it.
 */

int getinteger(int *dest, int *j, const char *str)
{
    int  cum;
    int  sgn;
    char ch;

    ch = str[*j];
    if (ch == '-') {
        sgn = -1;
        ch  = str[++(*j)];
    }
    else
        sgn = 1;

    if ((ch >= '0') && (ch <= '9')) {
        cum = 0;
        while ((ch >= '0') && (ch <= '9')) {
            cum = cum*10 + ch - '0';
            ch = str[++(*j)];
        }
        *dest = sgn * cum;

        return 0;                   /* return ok */
    }

    return 1;                       /* return error */

}   /* getinteger */



/*
 * GETLONG -- Convert ascii to a long. I'm sure there is a library
 *            call for it.
 */

int getlong(long *dest, int *j, const char *str)
{
    long  cum;
    int  sgn;
    char ch;

    ch = str[*j];
    if (ch == '-') {
        sgn = -1;
        ch  = str[++(*j)];
    }
    else
        sgn = 1;

    if ((ch >= '0') && (ch <= '9')) {
        cum = 0;
        while ((ch >= '0') && (ch <= '9')) {
            cum = cum*10L + ch - '0';
            ch = str[++(*j)];
        }
        *dest = (long) sgn * cum;

        return 0;                   /* return ok */
    }

    return 1;                       /* return error */

}   /* getinteger */



/*
 * ERROREXIT -- Exit program with an erro message.
 */

void errorexit(int errorcode)
{

    fprintf(stderr, "%s: ", progname);
    switch (errorcode) {
        case  illop : fprintf(stderr, "Illegal op-code found: %d\n", opcode);
                      break;
        case  stkof : fprintf(stderr, "Stack overflow\n");
                      break;
        case  stkuf : fprintf(stderr, "Stack underflow\n");
                      break;
        case  stkrq : fprintf(stderr, "Cannot create dvi stack\n");
                      break;
        case  lnerq : fprintf(stderr, "Cannot allocate memory\n");
                      break;
        case  badid : fprintf(stderr, "Id-byte is not correct: %d\n ", opcode);
                      break;
        case  bdsgn : fprintf(stderr, "Bad signature: %d (not 223)\n",
                                      (int) foo);
                      break;
        case  fwsgn : fprintf(stderr, "%d signature bytes (min. 4)\n",
                                      (int) foo);
                      break;
        case  nopre : fprintf(stderr, "Missing preamble\n");
                      break;
        case  nobop : fprintf(stderr, "Missing beginning-of-page command\n");
                      break;
        case  nopp  : fprintf(stderr, "Missing post-post command\n");
                      break;
        case  bdpre : fprintf(stderr, "Preamble occured inside a page\n");
                      break;
        case  bdbop : fprintf(stderr, "BOP-command occured inside a page\n");
                      break;
        case  bdpst : fprintf(stderr, "Postamble occured before end-of-page\n");
                      break;
        case  bdpp  : fprintf(stderr, "Postpost occured before post-command\n");
                      break;
        case  nopst : fprintf(stderr, "Missing postamble\n");
                      break;
        case  illch : fprintf(stderr, "Character code out of range, 0..127\n");
                      break;
        case  filop : fprintf(stderr, "Cannot open dvifile\n");
                      break;
        case  filcr : fprintf(stderr, "Cannot create outfile\n");
                      break;
        case  pipcr : fprintf(stderr, "Cannot create pipe to pager\n");
                      break;
        case  bdfnt : fprintf(stderr, "Fail to get font information\n");
                      break;
        default     : fprintf(stderr, "Unkown error code\n");
                      break;
    };
    if (outputtofile)
        unlink(OUTfilename);

    exit(errorcode);

}  /* errorexit */



/*
 * USAGE -- Print the usage info. Also print a warning/error message
 *          if needed.
 */

void usage(int uerr)
{

    if (jautodetect || nttj || asciip || uptex)
        fprintf(stderr, "%s (%s) %s", Progname, get_enc_string(), Copyright);
    else
        fprintf(stderr, "%s  %s", Progname, Copyright);

    if (uerr != ign) {
        fprintf(stderr,"\n%s: ", progname);
        switch (uerr) {
            case   wrnge  : fprintf(stderr, "width arg out of range:16-%d",
                                            MAXTERMWIDTH);
                            break;
            case   nan    : fprintf(stderr,
                                    "numeric argument expected for option %c",
                                            optch);
                            break;
            case   gae    : fprintf(stderr, "garbage in argument for option %c",
                                            optch);
                            break;
            case   bdlst  : fprintf(stderr, "mal-formed list of pagenumbers");
                            break;
            case   onef   : fprintf(stderr, "only one infile argument allowed");
                            break;
            case   noarg  : fprintf(stderr,
                                    "option argument expected for option %c",
                                            optch);
                            break;
            case   bdopt  : fprintf(stderr, "bad option %c", optch);
                            break;
            case   onepp  : fprintf(stderr, "only one pagelist allowed");
                            break;
            case   confl  : fprintf(stderr, "-J, -N, -A, and -U are mutually exclusive");
                            break;
            case   incone : fprintf(stderr, "output encoding is not consistent");
                            break;
            default       : fprintf(stderr, "unknown usage error");
                            break;
        }
        fprintf(stderr, "\n");
    }

    fprintf(stderr, "\n%s\n\n", dvi2tty);
    fprintf(stderr, "Usage: %s [ options ] <dvi-file>[.dvi]\n", progname);
    fprintf(stderr, "Options are:\n");
    fprintf(stderr,
            " -ofile   Write output to file, else write to stdout.\n");
    fprintf(stderr,
            " -plist   Print pages whose TeX-page-number are in list.\n");
    fprintf(stderr,
            " -Plist   Print pages whose sequential number are in list.\n");
    fprintf(stderr,
            " -wn      Print the lines with width n characters, default 80.\n");
    fprintf(stderr,
            " -vn      Use n for vertical line height, default 450000.\n");
    fprintf(stderr,
            " -evalue  Add/Substract this value for spacing (-20..20)\n");
    fprintf(stderr, " -f       Try to pipe to a pager if output is a tty");
    if (WANTPAGER)
        fprintf(stderr, " (default).\n");
    else
        fprintf(stderr, ".\n");
    fprintf(stderr, " -q       Don't try to pipe to a pager");
    if (WANTPAGER)
        fprintf(stderr, ".\n");
    else
        fprintf(stderr, " (default).\n");
    fprintf(stderr, " -Fprog   Pipe output to pager prog.\n");
    fprintf(stderr,
            " -a       Remove accents grave etc. from output: \\'{e} -> e.\n");
    fprintf(stderr,
            " -t       Assuming that document was made with tt fonts\n");
    fprintf(stderr,
            " -l       Write ''^L'' instead of formfeed between pages.\n");
    fprintf(stderr,
            " -s       Toggle National Swedish/Finnish characters printed as aaoAAO (default %s).\n", DEFSCAND ? "off" : "on");
    fprintf(stderr,
            " -u       Toggle latin1 support (default %s).\n", DEFLATIN1 ? "on" : "off");
    fprintf(stderr,
            " -J       Enable auto detect for NTT JTeX, ASCII pTeX, and upTeX (japanese fonts).\n");
    fprintf(stderr,
            " -N       Support NTT JTeX dvi.\n");
    fprintf(stderr,
            " -A       Support ASCII pTeX dvi.\n");
    fprintf(stderr,
            " -U       Support upTeX dvi.\n");
    fprintf(stderr,
            " -Eenc    Output multibyte encoding. u:UTF8, e:EUC-JP s:Shift_JIS j:JIS\n"
            "                             u1:UTF8 (do not use ligature for ff,fi,fl,ffi,ffl).\n");
    fprintf(stderr,
            " -C       Don't try to compose a combining character sequence.\n");
    fprintf(stderr,
            " -c       Override -a -u -s and print all characters 0-255.\n");
    fprintf(stderr,
            " -bdelim  Print font switch as text: delimcmr10miled\n");
    fprintf(stderr, " -h       This help message.\n");
    fprintf(stderr, "\n If you like this code and want to support is feel free\n to donate at Paypal marcel@mesa.nl. Thanks.\n\n");

    if (uerr != ign)
       exit(uerr);
    exit(0);

} /* usage */

