/*
 *       <<< dvispc >>>
 *
 *  A program to modify DVI file to be page-independent
 *  Support the following specials
 *   color specials:  push, pop, background
 *   pdf   specials:  pdf:bcolor, pdf:ecolor, pdf:bgcolor
 *   tpic  specials:  pn
 *
 *     Originally written by SHIMA
 *       January 2003
 */

#ifdef __GNUC__
/* Validate in case of UNIX */
#define UNIX 1
#else
/* Validate if SHIFT JIS is used for a filename */
/* Win32 MSVC is assumed */
#define SHIFT_JIS 1
#endif

/*     %%% Contents of an extended DVI file  %%%
 *  PRE(247)
 *  ...      % PREAMBRE %
 *  ...
 *
 *  BOP(139)
 *  c[10][4]
 *  p[4]       -1
 *  ...      % Contents of the first pages
 *  EOP(140)
 *
 *  BOP(139)
 *  ...      % Contents of pages
 *  ...
 *
 *  BOP(139) % top of the last page
 *  c[10][4]
 *  p[4]       address of BOP at the top of the former page
 *  ...      % Contents of the last page
 *  EOP(140) % end of the last page
 */
 
 /*        % Added part %
 *    POST(248)
 *    Data[num_add][?]                  series of data
 *    FileNames[num_add][?]             series of strings NULL terminated
 *    pt_Data[num_add][BOD[4], LOD[4]]  pointer and length of data
 *
 *                % 20 byte
 *    BOF[4]      top of series of strings
 *    EOF[4]      end of series of strings
 *    num_add[4]  number of data
 *    top_add[4]  address of POST at the top of added part
 *    AdID[3]     'A', 'd', 'I'
 *    EOP(140)    % End of Added part %
 */
 
 /*
 *  POST(248)  % Top of POSTAMBRE %
 *  p[4]                      address of BOP at the top of the last page
 *  ....
 *  POST_POST(249)
 *  top_post[4]               address of POST at the top of POSTAMBRE
 *  2 (or 3), 223,...,223     4 - 7 times 223
 */

/*
 *  The extended DVI file should be the following
 *
 *  1. The 8 byte preceeding to the top of the POSTAMBRE:
 *      top_add[4], 'A', 'd', 'O', 140(=EOP)
 *
 *  2.  DVI[top_add[4]]    140(=EOP)
 *      DVI[top_add[4]-1]  248(=POS)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef UNIX /* for mkstemp(), umask() etc. */
#include <unistd.h> /* for mkstemp(), umask() etc. */
#include <sys/types.h>
#include <sys/stat.h>
#else
#include <io.h>
#include <fcntl.h>
#endif

#include <config.h>
#ifdef PTEXENC
#include <ptexenc/ptexenc.h>
#include <ptexenc/unicode.h>
#endif

#include "common.h"

#define uchar unsigned char
#define uint  unsigned int
#define Long  int

#ifdef UNIX
#define PATH_SEP     '/'
#define READ_TEXT    "r"
#define READ_BINARY  "r"
#define WRITE_BINARY "w"
#define WRITE_TEXT   "w"
#define StrCmp strcmp
#else
#define PATH_SEP     '\\'
#define PATH_SEPU    '/'
#define READ_TEXT    "rt"
#define READ_BINARY  "rb"
#define WRITE_BINARY "wb"
#define WRITE_TEXT   "wt"
#define StrCmp stricmp
#endif

#define PIXEL      int
#define SIZE_PARA  int
#define PAGE_INDEX int
#define SCALED_PT  int

#define DIMENSION struct DIMENSION_REC
/* Information about device-output
 */

struct DIMENSION_REC {
    PIXEL text_width, text_height;
    /* These are the maxmum size directed in DVI-file.
     */
    int mag;
    /* is the magnification for output. This is the same value as 'mag' in
     * DVI-file ( so equal the one in DVI_INFO ), when no '-mag' & '-half'
     * options directed.
     */
    SIZE_PARA size_para;
    /* is size-parameter for conversion pixel<-->scaled pt.
     * ( equal "num * den / mag" )
     */
    PAGE_INDEX *page_index;
    /* the index of file-offset of pages in dvifile
     */
    int total_page;
    /* same as 'total_page' in DVI_INFO.
     */
#if 0
    int start_page, end_page;
    /* the starting and the ending page to print.
     */
#endif
    int max_nombre_page;
    /* the maximal nombre page
     */
} dvi_dim;


#define MAX_INCL 2048  /* maximal number of embedded files */
#ifndef MAX_PATH
#define MAX_PATH 0x200  /* maximal length of path name */
#endif

#define ID       2
#define IDP      3
#define END_DVI  223

#define SET_CHAR_0      0
#define SET1            128
#define SET_RULE        132
#define PUT1            133
#define PUT_RULE        137
#define NOP             138
#define BOP             139
#define EOP             140
#define PUSH            141
#define POP             142
#define RIGHT1          143
#define W0              147
#define W1              148
#define X0              152
#define X1              153
#define DOWN1           157
#define Y0              161
#define Y1              162
#define Z0              166
#define Z1              167
#define FNT_NUM_0       171
#define FNT1            235
#define XXX1            239
#define FNT_DEF_1       243
#define PRE             247
#define POST            248
#define POST_POST       249
#define OPCODE          250
#define EOFNC           255  /* end of func */

#define DVIFILE_INFO struct DVIFILE_INFO_REC

#define MAX_LEN         128
#define MAX_COLOR       512
#define COLOR_BUF_SIZE  MAX_COLOR*32
#define MAX_ANNOT       8
#define ANNOT_BUF_SIZE  MAX_ANNOT*512
#define COMMON_SIZE     0x4000
#define MAX_FONT        256

#define DTL_STRING  1        /* 0 \' \\                      */
#define DTL_CHAR    2        /* 1 (...)                      */
#define DTL_CHAR2   4        /* 2 character code by dig/hex  */
#define DTL_CMD     8        /* 3 compact command name       */
#define DTL_FNTDEF  0x10     /* 4 checksum is in octal       */
#define DTL_FNTNAME 0x20     /* 5 dir/name                   */
#define DTL_ILL     0x40     /* 6 opcode/illegal             */
#define DTL_PAGE    0x80     /* 7                            */
#define DTL_OCT     0x100    /* 8                            */
#define DTL_EXACT   0x200    /* 9 -> DVI according to string */
#define DTL_FNTDFN  0x400    /* 1 check used font definition */
#define DTL_VARIETY 0x800    /* 2 ignore variety             */
#define DTL_FORM    0x8000   /* the first line: variety      */
int f_dtl = 0;

enum {
    EXE2MODIFY, EXE2CHECK, EXE2SPECIAL, EXE2TEXT, EXE2DVI
};

int f_mode = EXE2MODIFY; /*  0: -c modify
                             1: -d report only
                             2: -s specials
                             3: -a to_Text
                             4: -x to_DVI   */

int f_debug = 0;        /* -v */
int f_overwrite = 0;
int f_backup = 0;       /* -b */

#ifdef PTEXENC
int  f_jstr = 0;  /* -J */
#else
#ifdef UNIX
int  f_sjis = 0;  /* -j */
#else
int  f_sjis = 1;
#endif
#endif
int  f_pos = 0;   /* position */
int  f_book = 0;  /* multiple of four pages */

int  f_ptex = 0;
int  f_prescan = 0;
int  f_last = 0;
int  max_stack;
char *out_pages ="T-L";
int  total_book_page;

/* non-stack specials */
char background[MAX_LEN];
char background_prev[MAX_LEN];
int  f_background = 0;  /* in each page, 0: not found; 1: found */
char pdf_bgcolor[MAX_LEN];
char pdf_bgcolor_prev[MAX_LEN];
int  f_pdf_bgcolor = 0; /* in each page, 0: not found; 1: found */
char tpic_pn[MAX_LEN];
char tpic_pn_prev[MAX_LEN];
int  f_pn = 0;  /* in each page, 0: not found and not actually used;
                                 1: found before actual use
                                -1: not found before actual use */

/* stack specials */
int  color_depth;
int  color_depth_max;
int  color_under;
char *color_pt[MAX_COLOR];
int  pdf_color_depth;
int  pdf_color_depth_max;
int  pdf_color_under;
char *pdf_color_pt[MAX_COLOR];
int  pdf_annot_depth;
int  pdf_annot_depth_max;
int  pdf_annot_under;
char *pdf_annot_pt[MAX_ANNOT];

int  f_needs_corr; /* flag to determine if correction is needed */
char color_buf[COLOR_BUF_SIZE]; /* common buffer for color/pdf_color */
char annot_buf[ANNOT_BUF_SIZE];

char tmp_buf[COMMON_SIZE];
FILE *fp_in;
FILE *fp_out;

#define DVIFILE_INFO struct DVIFILE_INFO_REC
 /*   Information about Preamble of DVI-file. ( to say the truth, this is
     * the same thing as Preamble itself. So, you'd better see
     * "TeX : The Program." )
     */
struct DVIFILE_INFO_REC {
    FILE *file_ptr;
    char *file_name;
    Long post, pt_post, last_bop;
    Long num, den, mag;
    SCALED_PT l, u;
    int stack_depth;
    int total_page;
} dvi_info;

char infile[MAX_PATH];
char outfile[MAX_PATH];
char font_use[MAX_FONT];

const int AdID = (('A'<<24)+('d'<<16)+('O'<<8)+EOP);

#ifndef PTEXENC
// #define issjis1(c) ((c)>=0x81&&(c)<=0xfc&&((c)<=0x9f||(c)>=0xe0))
// #define issjis2(c) ((c)>=0x40 && (c)<=0xfc && (c)!=0x7f)
#endif
#define isjis(c) (((c)>=0x21 && (c)<=0x7e))
#define is_hex(c)   ((c>='0'&&c<='9')||(c>='a'&&c<='f')||(c>='A'&&c<='F'))
#define is_oct(c)   (c>='0'&&c<='7')
// #define is_dig(c)   (c>='0'&&c<='9')
#define hex_to_dig(c)   ((c>='0'&&c<='9')?(c-'0'):(toupper(c)-('A'-10)))
#define MSG(x) (f_debug?(x):"")
#define read_byte(x) getc(x)
#define write_byte(x,y) putc(x,y)
#define read_short(x) signed_read_n(x,2)

uint work(FILE *);
uint s_work(FILE *);
int strsubcmp(char *s, char *t);
int strsubcmp_n(char *s, char *t);
void sp_color(char *sp);
void sp_pdf_bcolor(char *sp);
void sp_pdf_ecolor(char *sp);
void sp_pdf_bann(char *sp);
void sp_pdf_eann(char *sp);
void read_post(DVIFILE_INFO *dvi);
uint interpret(FILE *);
void make_page_index(DVIFILE_INFO *dvi, DIMENSION *dim);
void translate(DVIFILE_INFO *dvi, DIMENSION *dim);
void transpre(FILE *);
void transpost(FILE *);
void trans2dvi(void);
void replace(char *, char *);

Long read_n(FILE * fp, int n)
{
    long d;

    if(n--)
        d = (unsigned char)getc(fp);
    else
        return 0;
    while (n-- > 0)
        d = (d<<8) +  (unsigned char)getc(fp);
    return d;
}


Long signed_read_n(FILE * fp, int n)
{
    long   d;

    if(n--)
        d = (signed char)getc(fp);
    else
        return 0;
    while (n-- > 0)
        d = (d<<8) +  (unsigned char)getc(fp);
    return d;
}


void Exit(int code)
{
    if(fp_out != stdout && fp_out != stderr && *outfile){
        if(fp_out){
            fclose(fp_out);
            unlink(outfile);
        }
    }
    exit(code);
}


void error(char *msg)
{
    fprintf(stderr, "%s\n", msg);
    Exit(1);
}


void write_long(int x, FILE *fp)
{
    write_byte(x >> 24, fp);
    write_byte((x >> 16) & 0xff, fp);
    write_byte((x >> 8) & 0xff, fp);
    write_byte(x & 0xff, fp);
}


Long read_long(FILE *fp)
{
    int i;

    i  = read_byte(fp) << 24;
    i += read_byte(fp) << 16;
    i += read_byte(fp) << 8;
    return i + read_byte(fp);
}


void write_word(int x, FILE *fp)
{
    write_byte((x >> 8) & 0xff, fp);
    write_byte(x & 0xff, fp);
}


Long read_word(FILE *fp)
{
    int i;

    i = read_byte(fp) << 8;
    return i + read_byte(fp);
}


void usage(int ex)
{
    fprintf(stderr,
    "\t  Modify a DVI file to be page-independent in specials\n"
    "\t  Translation between  DVI file  <->  Text file\n"
    "\t         Originally written by SHIMA, Jan. 2003\n"
    "\t         Ver.%s (%s)\n\n", VERSION, TL_VERSION);
    fprintf(stderr,
    "Usage: dvispc [-c] [-bvz] input_dvi_file [output_dvi_file]\n"
    "       dvispc -d input_dvi_file\n"
    "       dvispc -s [-p..] input_dvi_file [output_text_file]\n"
#ifdef PTEXENC
    "       dvispc -a [-ltv][-J..][-p..][-r..] input_dvi_file [output_text_file]\n"
#else
    "       dvispc -a [-jltv][-p..][-r..] input_dvi_file [output_text_file]\n"
#endif
    "       dvispc -x[..] [-ltv][-r..] [input_text_file] output_dvi_file\n\n"
    "Mode options:\n"
    "   -c: correct input DVI to be page-indepent in specials (default)\n"
    "   -d: dry run to check page-independence\n"
    "   -s: show specials\n"
    "   -a: translate DVI to Text\n"
    "   -x: translate Text to DVI (-x0:str0 1:chkfnt 2:variety)\n\n"
    "Other options:\n"
#ifdef PTEXENC
    "   -v: verbose       -l: location\n"
#else
    "   -v: verbose       -j: Japanese characters       -l: location\n"
#endif
    "   -b: backup original even if output_dvi_file is not given\n"
    "   -z: append empty pages if necessary to have multiple of 4 pages for book\n"
    "   -p: T:preamble  L:postamble  pages with - (eg: -pT-L  -pT2/4-8L  -p-4 etc.)\n"
    "   -r: replace  (-rorg_1=new_1/org_2=new_2...  eg. -rxxx=special/fnt=font)\n"
    "   -t: compatible to DTL (the followings are suboptions if necessary eg. -t02)\n"
    "       0:str 1:ch 2:ch2 3:cmd 4:c-sum 5:dir/name 6:err 7:page 8:oct 9:str0\n"
#ifdef PTEXENC
    "   -J: set Japanese characters output with a suboption:\n"
    "       e:EUC-JP s:Shift_JIS u:UTF-8 for pTeX  or  U:UTF-8 for upTeX.\n"
#endif
    "   output_dvi_file  : overwrite if it is not specified.\n"
    "   output_text_file : stdout if it is not specified.\n"
    "   input_text_file  : stdin  if it is not specified.\n\n"
    "Supported specials:\n"
    "   color specials:  push, pop, background\n"
    "   pdf   specials:  pdf:bcolor, pdf:ecolor, pdf:bgcolor\n"
    "   tpic  specials:  pn\n"
    );
    fprintf(stderr, "\nEmail bug reports to %s.\n", BUG_ADDRESS);
    exit(ex);
}


int main(int argc, char **argv)
{
    int i, len, ch, fnum;
    char *o_name, *n_name, *s;

    if(argc <= 1)
        usage(0);

    for(i = 1; i < argc && argv[i][0] == '-'; i++){
      for(len = 1; argv[i][len]; len++){
        switch(argv[i][len]){
            case 'c':
                f_mode = EXE2MODIFY;
                break;

            case 's':
                f_mode = EXE2SPECIAL;
                break;

            case 'a':
                f_mode = EXE2TEXT;
                break;

            case 'd':
                f_mode = EXE2CHECK;
            case 'v':
                f_debug = 1;
                break;

            case 'x':
                f_mode = EXE2DVI;
                if(isdigit(argv[i][len+1])){
                    while(isdigit(ch = argv[i][++len]))
                        f_dtl ^= (1 << (ch - ('0'-9)));
                    len--;
                }
                break;

            case 'b':
                f_backup = 1;
                break;

#ifdef PTEXENC
            case 'J':
                f_jstr = 1;
                switch(argv[i][len+1]){
                    case 'e':
                        set_enc_string ("euc",  "default"); break;
                    case 's':
                        set_enc_string ("sjis", "default"); break;
                    case 'u':
                        set_enc_string ("utf8", "default"); break;
                    case 'U':
                        enable_UPTEX(true);
                        set_enc_string ("utf8", "uptex"); break;
                    default:
                        fprintf(stderr, "Unknown option:%s\n", argv[i]);
                        exit(1);
                }
                len++;
                break;
#else
            case 'j':
                f_sjis = 1 - f_sjis;
                break;
#endif

            case 'p':
                s = out_pages = argv[i]+len+1;
                if(!*out_pages)
                    out_pages = "T-L";
                else{
                    while(*s){
                        if((*s >= '0' && *s <= '9') 
                          || *s == 'T' || *s == 'L' || *s == '-'){
                            s++;
                            continue;
                        }
error:                  fprintf(stderr, "Error in parameter %s\n", argv[i]);
                        exit(1);
                    }
                }
                goto skip;

            case 'r':
                if(strlen(argv[i]+len+1) > COMMON_SIZE-2){
                    fprintf(stderr, "Too long argument %s\n", argv[i]);
                    exit(1);
                }
                strcpy(tmp_buf, argv[i]+len+1);
                for(o_name = n_name = tmp_buf; *o_name; ){
                    while(*++n_name != 0 && *n_name != '=');
                    if(!*n_name){
                        fprintf(stderr, "Wrong argument %s\n", argv[i]);
                        exit(1);
                    }
                    *n_name++ = 0;
                    for(s = n_name; *++s && *s != '/';);
                    if(!*s)
                        s[1] = 0;
                    *s = 0;
                    if(*o_name < 'a' || *o_name > 'z' || !*n_name)
                        goto error;
                    replace(o_name, n_name);
                    o_name = n_name = s+1;
                }
                goto skip;

            case 't':
                if(isdigit(argv[i][len+1])){
                    while(isdigit(ch = argv[i][++len]))
                        f_dtl ^= (1 << (ch - '0'));
                    len--;
                }else{
#ifndef PTEXENC
                    f_sjis = 1-f_sjis;
#endif
                    f_dtl = 0x00ffffff;
                }
                break;

            case 'l':
                f_pos = 1;
                break;

            case 'z':
                f_book = 1;
                break;

            default:
                fprintf(stderr, "Unknown option:%s\n", argv[i]);
                exit(1);
        }
      }
skip: ;
    }
    /* now, i = (number of optional arguments) + 1
       cf.  argc = (number of all arguments) + 1
        {argv[0] is the program name itself} ^^^ */

    fnum = 0;
    if(!isatty(fileno(stdin))){  /* if stdin is redirected from a file */
        fp_in = stdin;
        fnum++;
    }
    if(!isatty(fileno(stdout))){ /* if stdout is redirected to a file */
        fp_out = stdout;
        fnum++;
    }
    if(fnum != 2){
        len = strlen(argv[argc-1]);
        if(len >= MAX_PATH-5 || strlen(argv[i-1]) >= MAX_PATH-5)
            error("Too long filename");
    }

    switch(argc - i){ /* number of non-optional arguments */
        case 0:
            if(fp_in == NULL)
                /* infile not given, empty stdin; nothing I can do */
                usage(1);
            if(fp_out == NULL){
                /* outfile not given, free stdout;
                   binary cannot be written, text is fine */
                if(f_mode == EXE2MODIFY || f_mode == EXE2DVI)
                    usage(1);
                fp_out = stdout;
            }
            break;

        case 1:
         /* if(fnum == 2)
                usage(1);   */
            if(!fnum){  /* empty stdin, free stdout */
                /* if EXE2DVI, the only argument might be outfile,
                   but no input available; nothing I can do */
                if(f_mode == EXE2DVI)
                    usage(1);
                /* otherwise, the only argument should be infile */
                strcpy(infile, argv[argc-1]);
                /* outfile not given;
                   nonetheless binary should be written to a file,
                   text is fine with free stdout */
                if(f_mode == EXE2MODIFY)
                    strcpy(outfile, argv[argc-1]);
                else
                    fp_out = stdout;
            }else
                /* [TODO] this may not work well? (especially for EXE2DVI) */
                /* if fp_out == NULL, non-empty stdin and free stdout
                    -> the only argument = outfile (input from stdin)
                   otherwise, redirected stdout
                    -> the only argument = infile
                       if fnum == 2, non-empty stdin will be discarded but don't care!
                       (output to overwrite for EXE2MODIFY, stdout for others) */
                strcpy((fp_out == NULL)?outfile:infile, argv[argc-1]);
            break;

        case 2:
            /* prioritize filename arguments;
               if fp_in != NULL, non-empty stdin will be discarded but don't care! */
            strcpy(infile, argv[argc-2]);
            strcpy(outfile, argv[argc-1]);
            break;
        default:
            usage(1);
    }
#ifndef UNIX
    if(fp_out && !*outfile && (f_mode == EXE2DVI || f_mode == EXE2MODIFY))
        setmode( fileno( stdout ), O_BINARY);
#endif
    if(fp_in && !*infile && f_mode != EXE2DVI){
        fprintf(stderr, "*** stdin is a DVI file. ***\n"
            "*** Random Access may not be supported! ***\n");
#ifndef UNIX
        setmode( fileno( stdin ), O_BINARY);
#endif
    }

    /* append .dvi suffix if input/output is DVI */
    if(f_mode == EXE2DVI || f_mode == EXE2MODIFY){
        len = strlen(outfile);
        if(len){
            if(len < 4 || StrCmp(outfile + len - 4, ".dvi"))
                strcat(outfile, ".dvi");
        }
    }
    if(f_mode != EXE2DVI){
        len = strlen(infile);
        if(len){
            if(len < 4 || StrCmp(infile + len - 4, ".dvi")){
                strcat(infile, ".dvi");
                len += 4;   /* will be reused later while preparing overwrite */
            }
            dvi_info.file_name = infile;
        }
    }

    /* -x : text -> DVI */
    if(f_mode == EXE2DVI){
        /* use infile if given, otherwise use existing fp_in (= non-empty stdin)
           note that fp_in and infile are exclusive (already checked above) */
        if(fp_in == NULL || *infile){
            fp_in = fopen(infile, READ_TEXT);
            if(fp_in == NULL){
                fprintf(stderr, "Cannot open %s for input\n", infile);
                exit(1);
            }
        }
        /* use outfile if given */
        if(fp_out == NULL || *outfile){
            fp_out = fopen(outfile, WRITE_BINARY);
            if(fp_out == NULL){
                fprintf(stderr, "Cannot open %s for output\n", outfile);
                exit(1);
            }
        }
        trans2dvi();  /* files will be closed */
        return 0;
    }

    /* dvi->dvi or -d or -s or -a */
    /* [TODO] comments not added yet */
    if(argc - i == 1){
        if(f_mode == EXE2MODIFY && !fnum){
#ifdef UNIX
            static char tmpfile[] = "/tmp/dvispcXXXXXX";
            int fd;
            mode_t mask;

same:       mask = umask(077);
            fd = mkstemp(tmpfile);
            /* better to use tmpfile() */
            umask(mask);
            if(fd == -1){
                fprintf(stderr, "Cannot open temporary file\n");
                exit(1);
            }
            strcpy(outfile, tmpfile);
            close(fd);  /* will be fopen-ed again afterward */
#else
same:       strcpy(outfile, infile);
            strcpy(outfile + len - 3, "$$$");
#endif
            f_overwrite = 1;
        }
    }else if(argc - i == 2){
#ifdef UNIX
        struct stat infstat, outfstat;
        if(stat(infile, &infstat) == 0 && stat(outfile, &outfstat) == 0 &&
           infstat.st_dev == outfstat.st_dev && infstat.st_ino == outfstat.st_ino)
#else
        if(!StrCmp(infile,outfile))
#endif
            goto same;
    }
    if(fp_in && !*infile){
        dvi_info.file_ptr = fp_in;
        dvi_info.file_name = "stdin";
    }else if ((dvi_info.file_ptr = fopen(dvi_info.file_name, READ_BINARY)) == NULL){
        fprintf(stderr, "Cannot open %s for input\n", infile);
        exit(1);
    }
    /* [TODO] I'd like to use outfile if given */
    if(fp_out == NULL || *outfile){
        if(!*outfile)
            fp_out = (f_mode == EXE2TEXT || f_mode == EXE2SPECIAL)?stdout:stderr;
        else if(f_mode == EXE2MODIFY)
            fp_out = stderr;
        else{
#ifndef UNIX
            fp_out = fopen(outfile, WRITE_BINARY);
#else
            fp_out = fopen(outfile, WRITE_TEXT);
#endif
            if(fp_out == NULL){
                fprintf(stderr, "Cannot open %s for output\n", outfile);
                exit(1);
            }
        }
    }
    read_post(&dvi_info);
    make_page_index(&dvi_info, &dvi_dim);
    translate(&dvi_info, &dvi_dim);
    return 0;
}


void write_sp(FILE *fp, char *sp)
{
    int len;
    len = strlen(sp);

    if(f_debug)
        fprintf(fp_out, "%s", sp);
    if(f_mode != EXE2MODIFY || f_last)
        return; /* dry-run for EXE2CHECK */
    if(len <= 0xff)
        fprintf(fp, "%c%c%s", XXX1, len, sp);
    else if(len <= 0xffffffff){
        fprintf(fp, "%c", XXX1+3);
        write_long(len, fp);
        fprintf(fp, "%s", sp);
    }else{
        fprintf(stderr, "Too long special:\n%s\n", sp);
        Exit(1);
    }
}
void write_sp_nodebug(FILE *fp, char *sp)
{   /* omit debug message (for backward compatibility) */
    if(f_debug){
        f_debug = 0;    /* disable debug printing temporarily */
        write_sp(fp, sp);
        f_debug = 1;
    }else
        write_sp(fp, sp);
}


void translate(DVIFILE_INFO *dvi, DIMENSION *dim)
{
    int page, page2, pos, count, size, former, current, flag;
    FILE *fp;

    if(f_mode == EXE2MODIFY){
        fp = (*outfile)?fopen(outfile, WRITE_BINARY):fp_out;
        if(fp == NULL){
            fprintf(stderr, "Cannot open %s for output\n", outfile);
            exit(4);
        }else if(fp==stdout)
            f_debug = 0; /* ignore -v option, as debug output goes there, sigh */
    }else
        fp = NULL;

    f_needs_corr = flag = 0;
    f_last = 0;

    if(f_mode == EXE2TEXT || f_mode == EXE2SPECIAL){
        while(out_pages && *out_pages){
            if(*out_pages == 'T'){
                if(f_mode == EXE2TEXT){
                    fseek(dvi->file_ptr, dvi->post, SEEK_SET);
                    transpre(dvi->file_ptr);
                }
                out_pages++;
            }else if(*out_pages == 'L'){
                if(f_mode == EXE2TEXT){
                    fseek(dvi->file_ptr, dvi->post, SEEK_SET);
                    transpost(dvi->file_ptr);
                }
                out_pages++;
            }else if(*out_pages == '-'){
                page = 1;
                goto lastpage;
            }else if(isdigit(*out_pages)){
                page = atoi(out_pages);
                while(isdigit(*++out_pages));
                if(*out_pages == '-'){
lastpage:           if(isdigit(*++out_pages)){
                        page2 = atoi(out_pages);
                        while(isdigit(*++out_pages));
                    }else
                        page2 = dim->total_page;
                }else
                    page2 = page;
                if(page < 1 || page > page2 || page2 > dim->total_page){
                    fprintf(stderr, "Page instruction %d-%d by -p is out of range (1-%d)\n",
                        page, page2, dim->total_page);
                    exit(1);
                }
                for( ; page <= page2; page++){
                    if(!(f_dtl&DTL_PAGE)){
                        if(f_mode == EXE2TEXT)
                            putc(' ', fp_out);
                        fprintf(fp_out, "[%d]\n", page);
                    }
                    fseek(dvi->file_ptr, dim->page_index[page], SEEK_SET);
                    interpret(dvi->file_ptr);
                }
            }else
                out_pages++;
        }
        fclose(dvi->file_ptr);
        fclose(fp_out);
        dvi->file_ptr = fp_out = NULL;
        return;
    } /* done for if(f_mode == EXE2TEXT || f_mode == EXE2SPECIAL),
         the rest of translate() is meant for
         if((f_mode == EXE2MODIFY) and (f_mode == EXE2CHECK)) */

    /* Prior scanning. This ensures page independence in reverse order too,
       (e.g. Page 1 & 2 without background (= white) and Page 3 with background)
       by checking whether non-stack specials (except tpic_pn) appears somewhere in DVI.
       Specials with paired syntax (push/pop, bcolor/ecolor) are already safe
       without pre-scanning, so these are skipped due to f_prescan = 1.
       Other specials (background, pdf_bgcolor) are handled in this scanning. */
    f_prescan = 1;  /* change behavior of interpret(dvi) */
    for(page = 1; page <= dim->total_page; page++){
        fseek(dvi->file_ptr, dim->page_index[page], SEEK_SET);
        interpret(dvi->file_ptr);
    }
    f_prescan = 0;  /* restore interpret(dvi) */

    former = current = -1;
    if(fp){  /* f_mode == EXE2MODIFY and can be opened */
        fseek(dvi->file_ptr, 0, SEEK_SET);
        for(size =  dim->page_index[1]; size > 0; size--)
            write_byte(read_byte(dvi->file_ptr), fp);   /* Write preamble */
        /* [Process 1] Start writing the first page. */
        current  = ftell(fp);                           /* position of 1-st BOP */
        for(size = 41; size > 0; size--)                /* Write BOP and c[] */
            write_byte(read_byte(dvi->file_ptr), fp);
        write_long(former, fp);                         /* ptr to the former page = -1 */
    }

    for(page = 1; page <= dim->total_page; page++){ /* page loop start */
        fseek(dvi->file_ptr, dim->page_index[page], SEEK_SET);
        pos = interpret(dvi->file_ptr); /* scanned the whole page content; now
                                           pos = position of EOP + 1 */
        if(f_debug){    /* EXE2CHECK always falls into this */
            fprintf(fp_out, "[%d]", page);
            flag = f_needs_corr;    /* reserved for later check */
        }

        /* [Process 2] Before writing the current page, handle non-stack specials.
            * If not found but necessary, put one.
            * If found, save current status before going back to the loop head
              for the next page (= scanning the whole next page might overwrite it!).
           Also, if the page is suffering from stack underflow,
           open lacking stacks beforehand. */
        while(color_under > 0){     /* recover underflow of color stack */
            write_sp_nodebug(fp, "color push  Black");
            f_needs_corr++;
            color_under--;
        }
        while(pdf_color_under > 0){ /* recover underflow of pdf:bcolor ... pdf:ecolor stack */
            write_sp_nodebug(fp, "pdf:bcolor [0]");
            f_needs_corr++;
            pdf_color_under--;
        }
        if(background[0]){  /* background used somewhere */
            if(!f_background){  /* no background in this page */
                if(f_debug) fprintf(fp_out, "\n");
                if(!background_prev[0]) /* assume white */
                    write_sp(fp, "background gray 1");
                else
                    write_sp(fp, background_prev);
                f_needs_corr++;
            }else               /* this page had one! */
                strncpy(background_prev, background, MAX_LEN);      /* save current */
        }
        if(pdf_bgcolor[0]){ /* pdf:bgcolor used somewhere */
            if(!f_pdf_bgcolor){ /* no pdf:bgcolor in this page */
                if(f_debug) fprintf(fp_out, "\n");
                if(!pdf_bgcolor_prev[0]) /* assume white */
                    write_sp(fp, "pdf:bgcolor [1]");
                else
                    write_sp(fp, pdf_bgcolor_prev);
                f_needs_corr++;
            }else               /* this page had one! */
                strncpy(pdf_bgcolor_prev, pdf_bgcolor, MAX_LEN);    /* save current */
        }
//        while(pdf_annot_under > 0){ /* recover underflow of pdf:bann ... pdf:eann stack */
//            /* [TODO] what should we do here? */
//            f_needs_corr++;
//            pdf_annot_under--;
//        }
        if(f_pn < 0) {  /* tpic_pn from the former page should be effective ... */
            if(!tpic_pn_prev[0])    /* ... but nothing found before */
                fprintf(stderr, "\nCannot find valid tpic pn."
                                "\nPlease check your LaTeX source.");
            else{                   /* ... OK */
                if(f_debug) fprintf(fp_out, "\n");
                write_sp(fp, tpic_pn_prev);
                f_needs_corr++;
            }
        }
        if(tpic_pn[0] && f_pn)  /* tpic_pn used in this page */
            strncpy(tpic_pn_prev, tpic_pn, MAX_LEN);    /* save current */

        /* [Process 3] Write contents of the current page. */
        if(f_mode == EXE2MODIFY){
            fseek(dvi->file_ptr, dim->page_index[page]+45, SEEK_SET);
            for(size = pos - dim->page_index[page] - 46; size > 0; size--)
                write_byte(read_byte(dvi->file_ptr), fp);
        }

        /* [Process 4] After writing the current page,
           close not-yet-closed stacks. */
        for(count = 0; count < color_depth; count++){
            write_sp_nodebug(fp, "color pop");
            f_needs_corr++;
        }
        for(count = 0; count < pdf_color_depth; count++){
            write_sp_nodebug(fp, "pdf:ecolor");
            f_needs_corr++;
        }
//        for(count = 0; count < pdf_annot_depth; count++){
//            write_sp_nodebug(fp, "pdf:eann");
//            f_needs_corr++;
//        }
        if(f_mode == EXE2MODIFY){
            write_byte((uchar)EOP, fp); /* write EOP */
            former = current;
            current = ftell(fp);        /* get position of BOP/POST */
        }

        if(page == dim->total_page)
            f_last = 1; /* reached the last page, change behavior of write_sp(fp,sp) */

        /* [Process 5] Except for the last page,
           start the next page with passing not-yet-closed stacks. */
        if(f_mode == EXE2MODIFY && !f_last){
            fseek(dvi->file_ptr, dim->page_index[page+1], SEEK_SET);
            for(size = 41; size > 0; size--)  /* write BOP and c[] */
                write_byte(read_byte(dvi->file_ptr), fp);
            write_long(former, fp); /* position of BOP of the former page */
        }
        for(count = 0; count < color_depth; count++){
            if(f_debug) fprintf(fp_out, "\n%d:", count+1);
            write_sp(fp, color_pt[count]);
        }
        for(count = 0; count < pdf_color_depth; count++){
            if(f_debug) fprintf(fp_out, "\n%d:", count+1);
            write_sp(fp, pdf_color_pt[count]);
        }
//        for(count = 0; count < pdf_annot_depth; count++){
//            if(f_debug) fprintf(fp_out, "\n%d:", count+1);
//            write_sp(fp, pdf_annot_pt[count]);
//        }
        /* f_needs_corr already set properly in [Process 4] */

        if(f_debug){    /* EXE2CHECK always falls into this */
            if(flag != f_needs_corr)    /* at least one special printed for debug */
                fprintf(fp_out, "\n");
        }
        f_last = 0; /* restore write_sp(fp,sp) */
    } /* page loop end */

    if(f_debug) {   /* EXE2CHECK always falls into this */
        if(color_depth_max)
            fprintf(fp_out, "\nMaximal depth of color stack:%d", color_depth_max);
        if(pdf_color_depth_max)
            fprintf(fp_out, "\nMaximal depth of pdf:bcolor ... pdf:ecolor stack:%d", pdf_color_depth_max);
        if(pdf_annot_depth_max)
            fprintf(fp_out, "\nMaximal depth of pdf:bann ... pdf:eann stack:%d", pdf_annot_depth_max);
    }
    if(f_mode == EXE2CHECK){
        fclose(dvi->file_ptr);
        fprintf(fp_out, f_needs_corr?
            "\nSome corrections are necessary!\n":
            "\nNo modification is necessary\n");
        fclose(fp_out);
        dvi->file_ptr = fp_out = NULL;
        return;
    } /* done for EXE2CHECK; remainings are for EXE2MODIFY */

    /* if -z option is given, add empty pages to make multiple of 4 pages */
    if(f_book && dim->total_page%4 == 0)
        f_book = 0; /* modification unnecessary */
    if(f_book){
        total_book_page = dim->total_page + (4 - dim->total_page%4)%4;
        for(page = dim->total_page; page < total_book_page; page++){
            write_byte((uchar)BOP,fp);
            write_long(-1, fp);
            for (count = 1; count < 10; count++) /* set all sub counts to 0 */
                write_long(0, fp);
            write_long(former, fp);
            /* always white page */
            if(background[0]) /* background is used somewhere */
                write_sp(fp, "background gray 1");
            if(pdf_bgcolor[0]) /* pdf:bgcolor is used somewhere */
                write_sp(fp, "pdf:bgcolor [1]");
            write_byte((uchar)EOP,fp);
            former = current;
            current = ftell(fp);  /* get position of BOP/POST */
        }
    }

    write_byte((uchar)POST,fp); /* write POST */
    write_long(former, fp);     /* position of the last BOP */

    fseek(dvi->file_ptr, dvi->post + 5, SEEK_SET);
    if(f_book){
        write_long(read_long(dvi->file_ptr), fp);  /* numerator */
        write_long(read_long(dvi->file_ptr), fp);  /* denominator */
        write_long(read_long(dvi->file_ptr), fp);  /* magnification */
        write_long(read_long(dvi->file_ptr), fp);  /* tallest page height */
        write_long(read_long(dvi->file_ptr), fp);  /* widest page width */
        write_word(read_word(dvi->file_ptr), fp);  /* DVI stack size */
        read_word(dvi->file_ptr);   /* skip original number of pages */
        write_word(total_book_page, fp);    /* new number of pages */
        for(size = dvi->pt_post - dvi->post - 29; size-- > 0; )
            write_byte(read_byte(dvi->file_ptr), fp); /* write postamble upto post_post */
    }
    else{
        for(size = dvi->pt_post - dvi->post - 5; size-- > 0; )
            write_byte(read_byte(dvi->file_ptr), fp); /* write postamble upto post_post */
    }
    write_long(current, fp);    /* position of POST */
    read_long(dvi->file_ptr);   /* skip old position of POST */
    write_byte(read_byte(dvi->file_ptr), fp); /* write id = 2/3 */

    for(size = 4; size > 0; size--)
        write_byte((uchar)END_DVI, fp); /* write END_DVI */
    size = ftell(fp);
    while(size++ & 3)   /* fill END_DVI */
        write_byte((uchar)END_DVI, fp);
    fclose(fp);
    fclose(dvi->file_ptr);
    fp = dvi->file_ptr = NULL;
    if(!f_needs_corr && !f_book){
        unlink(outfile);
        fprintf(stderr, "\nNo correction is done.\n");
        return;
    }
    if(f_overwrite){
        if(f_backup){
            sprintf(tmp_buf, "%s.bak", infile);
            if(rename(infile, tmp_buf) == -1){
                fprintf(stderr, "Cannot backup %s to %s\n", infile, tmp_buf);
                Exit(1);
            }
        }else
            unlink(infile);
        if(rename(outfile, infile) == -1){
            fprintf(stderr, "Cannot overwrite %s\n", infile);
            Exit(1);
        }
        fprintf(stderr, "\nDVI file %s is corrected\n", infile);
    }else
        fprintf(stderr, "\nCreate a new DVI file %s\n", (fp_out==stdout)?"(stdout)":outfile);
}


void make_page_index(DVIFILE_INFO *dvi, DIMENSION *dim)
{
    int i, tmp;
    Long offset;

    dim->page_index =
        (PAGE_INDEX *)malloc(sizeof(PAGE_INDEX) * (
            (dim->total_page = dvi->total_page) + 2));

    dim->max_nombre_page = 0;
    for (offset = dvi->last_bop, i = dvi->total_page; i > 0; i--) {
        fseek(dvi->file_ptr, offset, SEEK_SET);
        if ((uchar)read_byte(dvi->file_ptr) != BOP){
            fprintf(stderr, "No BOP command in page %d\n", i);
            Exit(1);
        }
        dim->page_index[i] = offset;
        if((tmp = read_long(dvi->file_ptr)) 
          > dim->max_nombre_page)
            dim->max_nombre_page = tmp;

        /* Read count[0] */
        fseek(dvi->file_ptr, 36L, SEEK_CUR);
        /* Skip other 'count' values */
        offset = read_long(dvi->file_ptr);
    }
}

uint interpret(FILE *dvi)
{
    int i;

/*
    font = NULL;

    H0 = output->h_0;
    V0 = output->v_0;
*/
    if ((uchar)read_byte(dvi) != BOP)
        error("No BOP");
    if(f_mode == EXE2TEXT){
        if(f_pos)
            fprintf(fp_out, "%ld: ", ftell(dvi)-1);
        fputs("bop", fp_out);
        fprintf(fp_out, " %d%s", read_long(dvi), MSG("/page"));
        for(i = 1; i < 10; i++)
            fprintf(fp_out, " %d", read_long(dvi));
        fprintf(fp_out, " %d%s\n", read_long(dvi), MSG("/former_bop"));
    }else
        fseek(dvi, 44L, SEEK_CUR);
    return (f_mode == EXE2TEXT)?work(dvi):s_work(dvi);
}

    /* set_char_0,   0 SET_CHAR_0 */
static uchar cmd128_mode[] =
{
        0x31,   /* 128 SET1       */
        0x32,   /* 129            */
        0x33,   /* 130            */
        0x34,   /* 131            */
        8,      /* 132 SET_RULE   */
        0x31,   /* 133 PUT1       */
        0x32,   /* 134            */
        0x33,   /* 135            */
        0x34,   /* 136            */
        8,      /* 137 PUT_RULE   */
        0,      /* 138 NOP        */
        0x10,   /* 139 BOP        */
        0x10,   /* 140            */
        0,      /* 141 PUSH       */
        0,      /* 142 POP        */
        1,      /* 143 RIGHT1     */
        2,      /* 144            */
        3,      /* 145            */
        4,      /* 146            */
        0,      /* 147 W0         */
        1,      /* 148 W1         */
        2,      /* 149            */
        3,      /* 150            */
        4,      /* 151            */
        0,      /* 152 X0         */
        1,      /* 153 X1         */
        2,      /* 154            */
        3,      /* 155            */
        4,      /* 156            */
        1,      /* 157 DOWN1      */
        2,      /* 158            */
        3,      /* 159            */
        4,      /* 160            */
        0,      /* 161 Y0         */
        1,      /* 162 Y1         */
        2,      /* 163            */
        3,      /* 164            */
        4,      /* 165            */
        0,      /* 166 Z0         */
        1,      /* 167 Z1         */
        2,      /* 168            */
        3,      /* 169            */
        4       /* 170            */
};

    /* fnt_num_0,  171 FNT_NUM_0  */
static uchar cmd235_mode[] =
{
        0x41,   /* 235 FNT1       */
        0x42,   /* 236            */
        0x43,   /* 237            */
        0x44,   /* 238            */

        0x51,   /* 239 XXX1       */
        0x52,   /* 240            */
        0x53,   /* 241            */
        0x54,   /* 242            */

        0x21,   /* 243 FNT_DEF_1  */
        0x22,   /* 244            */
        0x23,   /* 245            */
        0x24,   /* 246            */

        0x10,   /* 247 PRE        */
        0x10,   /* 248            */
        0x10,   /* 249            */
        0x10,   /* 250            */
        0x10,   /* 251            */
        0x10,   /* 252            */
        0x10,   /* 253            */
        0x10,   /* 254            */
        0x10    /* 255 EOFNC      */
};


int strsubcmp(char *s, char *t)
{
    while(*s == *t){
        if(!*t)
            return 0;
        s++;
        t++;
    }
    return(!*t && (uchar)(*s) <= ' ')?0:1;
}

/* without space separator (e.g. pdf:bann<<...>>, instead of pdf:bann <<...>>) */
int strsubcmp_n(char *s, char *t)
{
    while(*s == *t){
        if(!*t)
            return 0;
        s++;
        t++;
    }
    return(!*t)?0:1;
}

uint s_work(FILE *dvi)
{
    int code, mode, tmp = 0;
    f_background = 0;
    f_pdf_bgcolor = 0;
    f_pn = 0;

    while ((code = (uchar)read_byte(dvi)) != EOP){
        if(code >= 128){
            if (code < FNT_NUM_0){
                mode = cmd128_mode[code - 128];
                goto work;
            }else if (code >= FNT1){
                mode = cmd235_mode[code - FNT1];
work:           if (mode >= 0x30)
                    tmp = read_n(dvi, mode & 0xf);
                switch (mode >> 4) {
                  case (0):
                      while(mode-- > 0)
                          (void)read_byte(dvi);
                      break;

                  case (1):
                      if(!f_ptex || code != EOFNC){
                          fprintf(stderr, "illegal code(%d)\n", code);
                          Exit(1);
                      }
                      (void)read_byte(dvi);
                      break;

                  case (2):
                      fseek(dvi, (Long)((mode & 0xf) + 12), SEEK_CUR);
                      tmp = (uchar)read_byte(dvi) + (uchar)read_byte(dvi);
skip:                 while (tmp--)
                          (void)read_byte(dvi);
                      break;
/*
                  case (3):
                      break;

                  case (4):
                      break;
*/
                  case (5):
                      if(tmp < COMMON_SIZE-1){
                        char *special;

                        special = tmp_buf;
                        while (tmp-- > 0)
                            *special++ = (uchar)getc(dvi);
                        *special = '\0';
                        if(f_mode == EXE2SPECIAL){
                            fprintf(fp_out, "{%s}\n", tmp_buf);
                            break;
                        }
                        special = tmp_buf;
                        while(*special && ((uchar)*special) <= ' ')
                            special++;
                        if(!strsubcmp(special, "pn")){          /* pn: tpic pen size */
                            if(!f_prescan) strncpy(tpic_pn, special, MAX_LEN);
                            if(!f_pn)
                                f_pn = 1;
                        }else if(!f_pn && 
                          (!strsubcmp(special, "pa") ||         /* pa: tpic pen at */
                           !strsubcmp(special, "ar")) )         /* ar: draw circle */
                            f_pn = -1;
                        else if(!strsubcmp(special, "color"))   /* color push/pop */
                            sp_color(special);
                        else if(!strsubcmp(special, "pdf:bcolor"))  /* pdf:bcolor */
                            sp_pdf_bcolor(special);
                        else if(!strsubcmp(special, "pdf:ecolor"))  /* pdf:ecolor */
                            sp_pdf_ecolor(special);
                        else if(!strsubcmp(special, "background")){     /* background */
                            strncpy(background, special, MAX_LEN);
                            f_background = 1;
                        }
                        else if(!strsubcmp(special, "pdf:bgcolor")){    /* pdf:bgcolor */
                            strncpy(pdf_bgcolor, special, MAX_LEN);
                            f_pdf_bgcolor = 1;
                        }
//                        else if(!strsubcmp_n(special, "pdf:bann"))  /* pdf:bann */
//                            sp_pdf_bann(special);
//                        else if(!strsubcmp(special, "pdf:eann"))    /* pdf:eann */
//                            sp_pdf_eann(special);
                        break;
                      }
                    goto skip;
                }
            }
        }
    }
    return ftell(dvi);
}

/* color specials */
void sp_color(char *sp)
{
    char *s;
    if(f_prescan) return;

    if(strstr(sp, "pop")){
        if(--color_depth < 0){
            fprintf(stderr, "color stack underflow\n");
            color_under++;
            f_needs_corr++;
            color_depth = 0;
        }
        return;
    }
    if(strstr(sp, "push")){
        if(color_depth >= MAX_COLOR){
            fprintf(stderr, "Too many color push > %d\n", MAX_COLOR);
            Exit(1);
        }
        if(color_depth){
            s = color_pt[color_depth-1];
            s += strlen(s) + 1;
        }
        else
            s = color_buf;
        if(s - color_buf + strlen(sp) >= COLOR_BUF_SIZE - 2)
            error("Too much color definitions");
        else{
            strcpy(s, sp);
            color_pt[color_depth++] = s;
        }
        if(color_depth > color_depth_max)
            color_depth_max = color_depth;
    }
}

/* pdf:bcolor special */
void sp_pdf_bcolor(char *sp)
{
    char *s;
    if(f_prescan) return;

    /* copied from "color push" routine of sp_color */
    if(pdf_color_depth >= MAX_COLOR){
        fprintf(stderr, "Too many pdf:bcolor > %d\n", MAX_COLOR);
        Exit(1);
    }
    if(pdf_color_depth){
        s = pdf_color_pt[pdf_color_depth-1];
        s += strlen(s) + 1;
    }
    else
        s = color_buf;
    if(s - color_buf + strlen(sp) >= COLOR_BUF_SIZE - 2)
        error("Too much color definitions");
    else{
        strcpy(s, sp);
        pdf_color_pt[pdf_color_depth++] = s;
    }
    if(pdf_color_depth > pdf_color_depth_max)
        pdf_color_depth_max = pdf_color_depth;
}

/* pdf:ecolor special */
void sp_pdf_ecolor(char *sp)
{
    char *s;
    if(f_prescan) return;

    /* copied from "color pop" routine of sp_color */
    if(--pdf_color_depth < 0){
        fprintf(stderr, "pdf:bcolor ... pdf:ecolor stack underflow\n");
        pdf_color_under++;
        f_needs_corr++;
        pdf_color_depth = 0;
    }
    return;
}

/* pdf:bann special */
void sp_pdf_bann(char *sp)
{
    char *s;
    if(f_prescan) return;
    if(pdf_annot_depth >= MAX_ANNOT){
        fprintf(stderr, "Too many pdf:bann > %d\n", MAX_ANNOT);
        Exit(1);
    }
    if(pdf_annot_depth){
        s = pdf_annot_pt[pdf_annot_depth-1];
        s += strlen(s) + 1;
    }
    else
        s = annot_buf;
    if(s - annot_buf + strlen(sp) >= ANNOT_BUF_SIZE - 2)
        error("Too much annot definitions");
    else{
        strcpy(s, sp);
        pdf_annot_pt[pdf_annot_depth++] = s;
    }
    if(pdf_annot_depth > pdf_annot_depth_max)
        pdf_annot_depth_max = pdf_annot_depth;
}

/* pdf:eann special */
void sp_pdf_eann(char *sp)
{
    char *s;
    if(f_prescan) return;
    if(--pdf_annot_depth < 0){
        fprintf(stderr, "pdf:bann ... pdf:eann stack underflow\n");
        pdf_annot_under++;
//        f_needs_corr++;
        pdf_annot_depth = 0;
    }
    return;
}

int num_add;
int    top_add;

void read_post(DVIFILE_INFO *dvi)
    /* read POSTAMBLE */
{
    int code, endofs, top_add;

    if ( read_byte(dvi->file_ptr) != PRE ||
        ((code = read_byte(dvi->file_ptr)) != ID && code != IDP) ){
err:    fprintf(stderr, "%s is not correct DVI file\n", dvi->file_name);
        Exit(254);
    }
    for (endofs = -3L; fseek(dvi->file_ptr, endofs, SEEK_END),
         (code = read_byte(dvi->file_ptr)) != ID && code != IDP; endofs--){
        /* Search id number */
        if (code == EOF || code != END_DVI)
            goto err;
    }
    f_ptex = (code==IDP)?1:0;
    fseek(dvi->file_ptr, endofs - 4L, SEEK_END);
    dvi->pt_post = ftell(dvi->file_ptr);                        /* get POST_POST */
    if ((top_add = dvi->post = read_long(dvi->file_ptr)) <= 0)  /* get POST */
        goto err;
    /* Read the position of POSTAMBLE */

    fseek(dvi->file_ptr, dvi->post - 4, SEEK_SET);
    /* Set file-ptr at POSTAMBLE */

    if(AdID == read_long(dvi->file_ptr))
        num_add = 1;
    if((code = read_byte(dvi->file_ptr)) != POST)               /* at POST */
        goto err;
    if((dvi->last_bop = read_long(dvi->file_ptr)) <= 0)         /* get LAST_BOP */
        goto err;
#if 0
                 /* for extended DVI file of dviout */
    fseek(dvi->file_ptr, dvi->post - 20, SEEK_SET);
    if(num_add){
        int i, j, size;
        char *s;

        bofn = read_long(dvi->file_ptr);
        eofn = read_long(dvi->file_ptr);
        num_add = last_add = read_long(dvi->file_ptr);
        top_add = read_long(dvi->file_ptr);
        fseek(dvi->file_ptr, dvi->post - 20 - 8*num_add, SEEK_SET);
        for(i = 0; i < num_add; i++){
            pt_bod[i] = read_long(dvi->file_ptr);
            lod[i] = read_long(dvi->file_ptr);
        }
        fseek(dvi->file_ptr, bofn, SEEK_SET);
        size = eofn - bofn;
        s = malloc(size);
        for(i = 0; i < size; i++)
            s[i] = read_byte(dvi->file_ptr);
        pt_name[0] = s;
        for(i = j = 0; i < size; ){
            if(!s[i++])
                pt_name[++j] = s + i;
        }
    }
#endif
    fseek(dvi->file_ptr, dvi->post, SEEK_SET);

    /* Set file-ptr at POSTAMBLE */
    if ((uchar)read_byte(dvi->file_ptr) != POST)
        error("No Postamble");

    if ((dvi->last_bop = read_long(dvi->file_ptr)) <= 0)
        error("Negative Pointer(Last BOP)");

    dvi->num = read_long(dvi->file_ptr);
    dvi->den = read_long(dvi->file_ptr);
    dvi->mag = read_long(dvi->file_ptr);
    dvi->l   = read_long(dvi->file_ptr);
    dvi->u   = read_long(dvi->file_ptr);
    dvi->stack_depth = read_short(dvi->file_ptr);
    dvi->total_page  = read_short(dvi->file_ptr);

    if (dvi->num <= 0 || dvi->den <= 0 || dvi->mag <= 0)
        error("Reading Illegal Long");
    if (dvi->stack_depth < 0 || dvi->total_page <= 0)
        error("Reading Illegal Integer");
}

/**************     dvi -> text   ******************/

static char *SETCHAR = "setchar";

static char *cmd128_name[] =
{
        "set1",         /* 128 SET1         */
        "set2",         /* 129              */
        "set3",         /* 130              */
        "set4",         /* 131              */
        "setrule",      /* 132 SET_RULE     */
        "put1",         /* 133 PUT1         */
        "put2",         /* 134              */
        "put3",         /* 135              */
        "put4",         /* 136              */
        "putrule",      /* 137 PUT_RULE     */
        "nop",          /* 138 NOP          */
        "bop",          /* 139 BOP          */
        "eop",          /* 140 EOP          */
        "push",         /* 141 PUSH         */
        "pop",          /* 142 POP          */
        "right1",       /* 143 RIGHT1       */
        "right2",       /* 144              */
        "right3",       /* 145              */
        "right4",       /* 146              */
        "w0",           /* 147 W0           */
        "w1",           /* 148 W1           */
        "w2",           /* 149              */
        "w3",           /* 150              */
        "w4",           /* 151              */
        "x0",           /* 152 X0           */
        "x1",           /* 153 X1           */
        "x2",           /* 154              */
        "x3",           /* 155              */
        "x4",           /* 156              */
        "down1",        /* 157 DOWN1        */
        "down2",        /* 158              */
        "down3",        /* 159              */
        "down4",        /* 160              */
        "y0",           /* 161 Y0           */
        "y1",           /* 162 Y1           */
        "y2",           /* 163              */
        "y3",           /* 164              */
        "y4",           /* 165              */
        "z0",           /* 166 Z0           */
        "z1",           /* 167 Z1           */
        "z2",           /* 168              */
        "z3",           /* 169              */
        "z4"            /* 170              */
};

static char *cmd235_name[] =
{
        "fnt1",         /* 235 FNT1         */
        "fnt2",         /* 236              */
        "fnt3",         /* 237              */
        "fnt4",         /* 238              */

        "xxx1",         /* 239 XXX1         */
        "xxx2",         /* 240              */
        "xxx3",         /* 241              */
        "xxx4",         /* 242              */

        "fntdef1",      /* 243 FNT_DEF_1    */
        "fntdef2",      /* 244              */
        "fntdef3",      /* 245              */
        "fntdef4",      /* 246              */

        "pre",          /* 247 PRE          */
        "illegal",      /* 248              */
        "illegal",      /* 249              */
        "illegal",      /* 250              */
        "illegal",      /* 251              */
        "illegal",      /* 252              */
        "illegal",      /* 253              */
        "illegal",      /* 254              */
        "dir"           /* 255 EOFNC        */
};

static char *c128_name[] =
{
        "s1",           /* 128 SET1         */
        "s2",           /* 129              */
        "s3",           /* 130              */
        "s4",           /* 131              */
        "sr",           /* 132 SET_RULE     */
        "p1",           /* 133 PUT1         */
        "p2",           /* 134              */
        "p3",           /* 135              */
        "p4",           /* 136              */
        "pr",           /* 137 PUT_RULE     */
        "nop",          /* 138 NOP          */
        "bop",          /* 139 BOP          */
        "eop",          /* 140 EOP          */
        "[",            /* 141 PUSH         */
        "]",            /* 142 POP          */
        "r1",           /* 143 RIGHT1       */
        "r2",           /* 144              */
        "r3",           /* 145              */
        "r4",           /* 146              */
        "w0",           /* 147 W0           */
        "w1",           /* 148 W1           */
        "w2",           /* 149              */
        "w3",           /* 150              */
        "w4",           /* 151              */
        "x0",           /* 152 X0           */
        "x1",           /* 153 X1           */
        "x2",           /* 154              */
        "x3",           /* 155              */
        "x4",           /* 156              */
        "d1",           /* 157 DOWN1        */
        "d2",           /* 158              */
        "d3",           /* 159              */
        "d4",           /* 160              */
        "y0",           /* 161 Y0           */
        "y1",           /* 162 Y1           */
        "y2",           /* 163              */
        "y3",           /* 164              */
        "y4",           /* 165              */
        "z0",           /* 166 Z0           */
        "z1",           /* 167 Z1           */
        "z2",           /* 168              */
        "z3",           /* 169              */
        "z4"            /* 170              */
};

static char *c235_name[] =
{
        "f1",           /* 235 FNT1         */
        "f2",           /* 236              */
        "f3",           /* 237              */
        "f4",           /* 238              */

        "special1",     /* 239 XXX1         */
        "special2",     /* 240              */
        "special3",     /* 241              */
        "special4",     /* 242              */

        "fd1",          /* 243 FNT_DEF_1    */
        "fd2",          /* 244              */
        "fd3",          /* 245              */
        "fd4",          /* 246              */

        "pre",          /* 247 PRE          */
        "opcode",       /* 248              */
        "opcode",       /* 249              */
        "opcode",       /* 250              */
        "opcode",       /* 251              */
        "opcode",       /* 252              */
        "opcode",       /* 253              */
        "opcode",       /* 254              */
        "dir"           /* 255 EOFNC        */
};


#define MAX_STR 0x60

char str_buf[MAX_STR];
int len = 0;

void flush_str(void)
{
    if(!len)
        return;
    str_buf[len] = 0;
    if(f_dtl&DTL_CHAR)
        fprintf(fp_out, "(%s)\n", str_buf);
    else
        fprintf(fp_out, " \"%s\"\n", str_buf);
    len = 0;
}

#ifndef PTEXENC
#ifndef UNIX
void jis2sjis(int *h, int *l)
{
    if (*h & 1) {
        if (*l < 0x60)  *l += 0x1f;
            else        *l += 0x20;
    }else               *l += 0x7e;
    if (*h < 0x5f)      *h = (*h + 0xe1) >> 1;
        else            *h = (*h + 0x161) >> 1;
}
#endif
#endif

void out_string(FILE *in, FILE *out, int len)
{
    int ch;

    while(len-- > 0){
        ch = getc(in);
        if(f_dtl & DTL_STRING){
            if(ch < ' ' || ch > 0x7e){
                fprintf(out, "\\%02X", ch & 0xff);
                continue;
            }
            if(ch == '\'' || ch == '\\')
                putc('\\', out);
        }
        putc(ch, out);
    }
}

/* preamble */
void transpre(FILE *dvi)
{
    int len;
    fseek(dvi, 1, SEEK_SET);
    if(f_dtl & DTL_FORM)
        fprintf(fp_out, "variety sequences-6\n");
    if(f_pos)
        fputs("0: ", fp_out);
    fprintf(fp_out, "pre %u%s ", getc(dvi), MSG("/id"));    /* id */
    fprintf(fp_out, "%u%s ", read_long(dvi), MSG("/num"));  /* num */
    fprintf(fp_out, "%u%s ", read_long(dvi), MSG("/den"));  /* den */
    fprintf(fp_out, "%u%s ", read_long(dvi), MSG("/mag"));  /* mag */
    fprintf(fp_out, "%u%s ", len = getc(dvi),MSG("/len"));  /* len */
    putc('\'', fp_out);
    out_string(dvi, fp_out, len);
    fputs("\'\n", fp_out);
}


/* postamble */
void transpost(FILE *dvi)
{
    int code;

    if(f_pos)
        fprintf(fp_out, "%ld: ", ftell(dvi));
    getc(dvi);
    fprintf(fp_out, "post %u%s ", read_long(dvi), MSG("/final_bop")); /* pointer to the last bop*/
    fprintf(fp_out, "%u%s ", read_long(dvi), MSG("/num"));      /* num */
    fprintf(fp_out, "%u%s ", read_long(dvi), MSG("/den"));      /* den */
    fprintf(fp_out, "%u%s ", read_long(dvi), MSG("/mag"));      /* mag */
    fprintf(fp_out, "%u%s ", read_long(dvi), MSG("/h+d"));      /* h+d */
    fprintf(fp_out, "%u%s ", read_long(dvi), MSG("/w"));        /* w */
    fprintf(fp_out, "%u%s ", read_n(dvi, 2), MSG("/stack"));    /* stack */
    fprintf(fp_out, "%u%s\n", read_n(dvi, 2), MSG("/pages"));   /* pages */
    work(dvi);                                                  /* definiton of fonts */
    fprintf(fp_out, " %u%s", read_long(dvi), MSG("/post"));     /* pointer to post */
    fprintf(fp_out, " %u%s", getc(dvi), MSG("/id"));            /* id */
    while( (code = getc(dvi)) != EOF)
        fprintf(fp_out, " %u", code);
    putc('\n', fp_out);
}


/* content of a page */
uint work(FILE *dvi)
{
    int code, mode, tmp = 0;
    long pos = 0;
    uint csum;
#ifdef PTEXENC
    int imb;
    long wch;
    char mbstr[4];
#else
    int h_code, l_code;
#endif

    while( (code = (uchar)read_byte(dvi)) != EOP && code != POST_POST){
        if(f_pos)
            pos = ftell(dvi)-1;
        if(code < 128){
            if(f_pos && (!len || !(f_dtl&DTL_CHAR)))
                fprintf(fp_out, "%ld: ", pos);
            if(code >= 0x20 && code < 0x7f){
                if(len > MAX_STR-2){
                    flush_str();
                    len = 0;
                    if(f_pos)
                        fprintf(fp_out, "%ld: ", pos);
                }
                if((f_dtl & DTL_CHAR) && 
                  (code == '(' || code == ')' || code == '\\' || code == '"'))
                    str_buf[len++] = '\\';
                str_buf[len++] = (uchar)code;
            }else{
                if(len && f_pos)
                    fprintf(fp_out, "%ld: ", pos);
                len = 0;
                flush_str();
                if(f_dtl & DTL_CHAR)
                    fprintf(fp_out, "\\%02X\n", code);
            }
            if(!(f_dtl & DTL_CHAR))
                fprintf(fp_out, "%s%d\n", SETCHAR, code);
        }else{
            if(len)
                flush_str();
            if(f_pos)
                fprintf(fp_out, "%ld: ", pos);
            if (code < FNT_NUM_0){
                mode = cmd128_mode[code - 128];
                fprintf(fp_out, "%s", (f_dtl&DTL_CMD)?
                    c128_name[code-128]:cmd128_name[code-128]);
                if(!(mode & 0xf)){
                    putc('\n', fp_out);
                    continue;
                }
                if((mode & 0xf) > 4){
                    fprintf(fp_out, " %d", read_long(dvi));
                    mode -= 4;
                }
                if(mode > 0x30){
                    code = read_n(dvi, mode & 0xf);
#ifdef PTEXENC
                    if(f_jstr){
                      // internal-euc/sjis: fromDVI cannot convert ASCII range
                      if (is_internalUPTEX() || (isjis(code>>8) && isjis(code&0xff))) {
                        wch = fromDVI(code);
                        if (is_internalUPTEX()) wch = UCStoUTF8(wch);
                        imb = 0;  memset(mbstr, '\0', 4);
                        if (BYTE1(wch) != 0) mbstr[imb++]=BYTE1(wch);
                        if (BYTE2(wch) != 0) mbstr[imb++]=BYTE2(wch);
                        if (BYTE3(wch) != 0) mbstr[imb++]=BYTE3(wch);
                        /* always */         mbstr[imb++]=BYTE4(wch);
                        fprintf(fp_out,
                            (f_dtl&DTL_CHAR2)?" %u \"":" 0x%x \"", code);
                        fputs2(mbstr, fp_out);
                        fprintf(fp_out, "\"\n");
                        continue;
                      }
                    }
#else
                    if(f_sjis){
                        l_code = code & 0xff;
                        h_code = code >> 8;
                        if(isjis(h_code) && isjis(l_code)){
#ifdef UNIX
                            h_code |= 0x80;
                            l_code |= 0x80;
#else
                            jis2sjis(&h_code, &l_code);
#endif
                            fprintf(fp_out, 
                                (f_dtl&DTL_CHAR2)?" %u \"%c%c\"\n":" 0x%x \"%c%c\"\n", 
                                code, h_code, l_code);
                            continue;
                        }
                    }
#endif
                    fprintf(fp_out, (f_dtl&DTL_CHAR2)?" %u\n":" 0x%x\n", code);
                }else
                    fprintf(fp_out, " %d\n", signed_read_n(dvi, mode & 0xf));
                continue;
            }else if (code< FNT1){
                fprintf(fp_out, (f_dtl&DTL_CMD)?"fn%d\n":"fntnum%d\n", code - FNT_NUM_0);
                continue;
            }else{
                mode = cmd235_mode[code - FNT1];
                fprintf(fp_out, "%s", (f_dtl&DTL_CMD)?c235_name[code-FNT1]:cmd235_name[code-FNT1]);
                if (mode >= 0x30)
                    tmp = (long)read_n(dvi, mode & 0xf);
                switch (mode >> 4) {
/*
                  case (0):
skip_m:             while(mode-- > 0)
                        (void)read_byte(dvi);
                        break;
*/
                    case (1):
                        if(!f_ptex || code != EOFNC){
                            fprintf(stderr, "illegal code(%d)", code);
                            if((f_dtl&DTL_ILL))
                                fprintf(stderr, " %d\n", code);
                            else
                                Exit(1);
                        }else
                            fprintf(fp_out, " %d\n", read_byte(dvi));
                        continue;

                    case (2):   /* fntdef */
                        fprintf(fp_out, " %d", read_n(dvi, mode & 0xf));    /* code */
                        csum = read_n(dvi, 4);
                        if(csum)
                            fprintf(fp_out, 
                                (f_dtl&DTL_FNTDEF)?((f_dtl&DTL_OCT)?" %o%s":" 0%o%s"):
                                " 0x%X%s", csum, MSG("/c-sum"));            /* chksum */
                        else
                            fprintf(fp_out, " 0%s", MSG("/c-sum")); 
                        fprintf(fp_out, " %u%s", read_long(dvi), MSG("/s-size"));   /* scaled size */
                        fprintf(fp_out, " %u%s", read_long(dvi), MSG("/d-size"));   /* design size */
                        tmp = (uchar)read_byte(dvi);
                        fprintf(fp_out, " %d%s", tmp, MSG("/dir"));                 /* len:directry */
                        code = (uchar)read_byte(dvi);
                        fprintf(fp_out, " %d%s '", code, MSG("/name"));             /* len:name */
                        while (tmp-- > 0)
                            putc(read_byte(dvi), fp_out);
                        if((f_dtl&DTL_FNTNAME))
                            fputs("' '", fp_out);
                        while(code-- > 0)
                            putc(read_byte(dvi), fp_out);
                        fputs("'\n", fp_out);
                        continue;
/*
                  case (3):
                      break;
*/

                    case (4):   /* fntnum */
                        fprintf(fp_out, " %u\n", tmp);
                        continue;

                    case (5):   /* specials */
                        fprintf(fp_out, " %d%s '", tmp, MSG("/len"));
                        out_string(dvi, fp_out, tmp);
                        putc('\'', fp_out);
                        putc('\n', fp_out);
                        continue;
                }
                while(tmp-- > 0)
                    getc(dvi);
            }
        }
    }
    if(f_pos)
        fprintf(fp_out, "%ld: ", ftell(dvi)-1);
    fputs((code==EOP)?"eop\n":"post_post", fp_out);
    return ftell(dvi);
}


/*************************   text -> dvi *************************************/
struct KEY_LIST  {
    char *name;
    short int code;
    short int len;
};

struct KEY_LIST key[] = {
    {"bop",     BOP,        1},
    {"d",       DOWN1,      4},
    {"dir",     EOFNC,      1},
    {"down",    DOWN1,      4},
    {"eop",     EOP,        1},
    {"f",       FNT1,       4},
    {"fd",      FNT_DEF_1,  4},
    {"fn",      FNT_NUM_0, 64},
    {"fnt",     FNT1,       4},
    {"fntdef",  FNT_DEF_1,  4},
    {"fntnum",  FNT_NUM_0, 64},
    {"nop",     NOP,        1},
    {"opcode",  OPCODE,     1},
    {"p",       PUT1,       4},
    {"pop",     POP,        1},
    {"post",    POST,       1},
    {"post_post",POST_POST, 1},
    {"pr",      PUT_RULE,   1},
    {"pre",     PRE,        1},
    {"push",    PUSH,       1},
    {"put",     PUT1,       4},
    {"putrule", PUT_RULE,   1},
    {"r",       RIGHT1,     4},
    {"right",   RIGHT1,     4},
    {"s",       SET1,       4},
    {"set",     SET1,       4},
    {"setchar", SET_CHAR_0,128},
    {"setrule", SET_RULE,   1},
    {"special", XXX1,       4},
    {"sr",      SET_RULE,   1},
    {"w",       W0,         5},
    {"x",       X0,         5},
    {"xxx",     XXX1,       4},
    {"y",       Y0,         5},
    {"z",       Z0,         5}
};

void key_swap(int i, int j)
{
    char tmp[sizeof(struct KEY_LIST)];

    memcpy(tmp, &key[i], sizeof(struct KEY_LIST));
    memcpy(&key[i], &key[j], sizeof(struct KEY_LIST));
    memcpy(&key[j], tmp, sizeof(struct KEY_LIST));
}

void sort_key(void)
{
    int i, j;

    for(i = 1; i < sizeof(key)/sizeof(struct KEY_LIST); i++){
        if(strcmp(key[i-1].name, key[i].name) > 0){
            j = i;
            do{
                key_swap(j-1, j);
                j--;
            }while(j >= 1 && strcmp(key[j-1].name, key[j].name) > 0);
        }
    }
}

void replace(char *o_name, char *n_name)
{
    int i, j, count;
    char *s, **cmd;

    if(!strcmp(o_name, SETCHAR)){
        SETCHAR = n_name;
        count = 1;
    }else{
        count = 0;
        cmd = cmd128_name;
        i = sizeof(cmd128_name)/sizeof(char *);
again:  while(--i >= 0){
            for(j = 0; cmd[i][j] == o_name[j] && o_name[j]; j++);
            if(!o_name[j]){
                if(!cmd[i][j] || (cmd[i][j] >= '0' && cmd[i][j] <= '9')){
                    s = malloc(strlen(cmd[i]) + j + 1);
                    strcpy(s, n_name);
                    strcat(s, cmd[i]+j);
                    cmd[i] = s;
                    count++;
                }
            }
        }
        if(cmd == cmd128_name){
            cmd = cmd235_name;
            i = sizeof(cmd235_name)/sizeof(char *) - 1;
            goto again;
        }
    }
    if(!count){
        fprintf(stderr, "%s is not a keyword\n", o_name);
        exit(1);
    }
    for(i = sizeof(key)/sizeof(struct KEY_LIST)-1; i >= 0 && strcmp(o_name, key[i].name); i--);
    if(i >= 0){
        key[i].name = malloc(strlen(n_name)+1);
        strcpy(key[i].name, n_name);
        sort_key();
    }
}

int sub_number;

int find_key(char *s0)
{
    int top, end, pt, num;
    char *s, *t;

    top = 0;
    end = sizeof(key)/sizeof(struct KEY_LIST)-1;

    while(top <= end){
        s = s0;
        t = key[pt = (top+end)/2].name;
        while(*s == *t && *t){
            s++;
            t++;
        }
        if(!*t){
            if((num = key[pt].len) == 1){
                if(*s <= ' '){
                    sub_number = 0;
                    return pt;
                }
            }else if(*s >= '0' && *s <= '9'){
                sub_number = atoi(s);
                if(num == 4){
                    if(sub_number > 0 && sub_number <= 4)
                        return pt;
                }else{
                    if(sub_number < num)
                        return pt;
                }
            }
        }
        if(*s > *t)
            top = pt+1;
        else
            end = pt-1;
    }
    return -1;
}

void write_n(int num, int byte)
{
    switch(byte){
        case 4:
            putc((num>>24) & 0xff, fp_out);
        case 3:
            putc((num>>16)& 0xff, fp_out);
        case 2:
            putc((num>>8) & 0xff, fp_out);
        case 1:
            putc(num & 0xff, fp_out);
    }
}

uchar *get_next(unsigned char *pt)
{
    while(*pt > ' ')
        pt++;
    while(*pt <= ' ' && *pt)
        pt++;
    return pt;
}


uint a2i(unsigned char *s)
{
    uint num;
/* strtol(s, &pt, 0)  will overflow for unsigned */

    if(!*s)
        return 0;
    if(*s != '0'){
        if(*s == '-' && isdigit(s[1]))
            return((uint)0xffffffff - ((uint)atoi((char *)s+1) - 1));
        return((uint)atoi((char *)s));
    }
    if(s[1] == 'x' || s[1] == 'X'){
        num = 0;
        for(s += 2; is_hex(*s); s++)
            num = (num << 4) + hex_to_dig(*s);
        return num;
    }else{
        num = 0;
        for(s += 1; is_oct(*s); s++)
            num = (num << 3) + *s - '0';
        return num; 
    }
}


int StrLen(uchar *pt)
{
    uchar *s;

    for(s = pt; *s && *s != '\''; s++){
        if(*s == '\\' && (f_dtl&DTL_STRING)){
            if(is_hex(s[1]))
                s++;
            if(*s)
                s++;
        }
    }
    return s - pt;
}


void trans2dvi(void)
{
    int num, pt, num0, pos0, len;
    int code = -1, pos = -1, page = 0, line = 0, f_in = 0, f_proc = 0, stack = 0;
    uchar *s, *base;
    static char hex[6] = "0x00";

    s = (uchar *)tmp_buf;   /* for Warning of compiler */
    while(fgets(tmp_buf, COMMON_SIZE, fp_in) != NULL){
        if(!line++ && !strncmp(tmp_buf, "variety ", 8)){
            if(!(f_dtl&DTL_VARIETY))
                f_dtl = 0x0ffff;
            continue;
        }
        base = (uchar *)tmp_buf;
        if(f_pos){  /* position: */
            if(*base <= ' ')
                continue;
skp:        while(*base++ > ' ');
        }
        if(*base < 'a' || *base > 'z'){
            if(!f_in){
                if(!f_proc && !f_pos && !strncmp(tmp_buf, "0: pre 2 ", 9)){
                    f_pos = 1;
                    goto skp;
                }
                continue;
            }else if(*base == '['){ /* PUSH by DTL */
                code = PUSH;
                goto cmd;
            }else if(*base == ']'){ /* POP by DTL */
                code = POP;
                goto cmd;
            }else if(*base == '\\' && (f_dtl&DTL_CHAR)){
                if(is_hex(base[1]) && is_hex(base[2])){ /* char \XY by DTL */
                    if(f_proc != 1){
                        if(!f_proc)
                            goto er0;
                        if(f_proc == 2)
                            goto er3;
                        goto er1;
                    }
                    if(!f_in)
                        goto er2;
                    hex[2] = base[1];
                    hex[3] = base[2];
                    putc(a2i((uchar *)hex), fp_out);
                }
            }else if(*base == '(' && (f_dtl&DTL_CHAR)){ /* char (...) by DTL */
                if(f_proc != 1){
                    if(!f_proc)
                        goto er0;
                    if(f_proc == 2)
                        goto er3;
                    goto er1;
                }
                if(!f_in)
                    goto er2;
                for(s = base; *++s != ')' && *s; ){ /* for DTL */
                    if(*s != '\\')
                        putc(*s, fp_out);
                    else{
                        s++;
                        if(is_hex(*s) && is_hex(s[1])){
                            hex[2] = *s;
                            hex[3] = *s++;
                            putc(a2i((uchar *)hex), fp_out);
                        }else
                            putc(*s, fp_out);
                    }
                }
            }
            continue;
        }
        pt = find_key((char *)base);
        if(pt < 0){
            fprintf(stderr, "No key!\n");
            goto err;
        }
        code = key[pt].code;
cmd:    if(!page && code != PRE){
er0:        fprintf(stderr, "Need PRE comamnd at the top\n");
            goto err;
        }
        if(f_proc > 2){
er1:        fprintf(stderr, "Command after POST_POST\n");
            goto err;
        }
        if(!f_in){
er2:        if(code < FNT_DEF_1 && code != BOP){
                fprintf(stderr, "This command shoud be after BOP\n");
                goto err;
            }
        }
        if(f_proc == 2 && (code < FNT_DEF_1 || code  > POST_POST)){
er3:        /* POST and PRE will be checked later */
            fprintf(stderr, "This command shoud be not be after POST\n");
            goto err;
        }
        switch(code){
            case PRE:
                if(f_proc){
                    fprintf(stderr, "Multiple PRE command\n");
                    goto err;
                }
                f_proc = 1;
                putc(code, fp_out);     /* pre */
                page++;
                s = get_next(base);
                code = a2i(s);
                putc(code, fp_out);     /* id */
                for(num=0; num <3; num++){
                    s = get_next(s);
                    code = a2i(s);
                    write_n(code, 4);
                }
                s = get_next(s);
                code = a2i(s);
                s = get_next(s) + 1;
                if(f_dtl&DTL_EXACT){
                    len = StrLen(s);
                    if(len > 255)
                        fprintf(stderr, "string length is too wrong (%d) in line %d\n%s\n", 
                            len, line, tmp_buf); 
                    else if(code != len)
                        fprintf(stderr, "string length in [%d:%s]: %d -> %d:\n", 
                            line, tmp_buf, code, len);
                }else
                    len = code;
                putc(len, fp_out);      /* length of string */
strout:         if(len >= COMMON_SIZE/2){
                    fprintf(stderr, "Too long data: %d\n", len);
                    Exit(1);
                }
                while(len-- > 0){
                    if((f_dtl&DTL_STRING) && *s == '\\'){
                        s++;
                        if(is_hex(*s)){
                            code = hex_to_dig(*s) << 4;
                            s++;
                            code += hex_to_dig(*s);
                            putc(code, fp_out);
                            continue;
                        }
                    }
                    putc(*s++, fp_out);
                }
                if(f_debug){
                    if(*s != '\'' || s[1] > ' '){
                        fprintf(stderr, "String length is not correct!\n");
                        goto err;
                    }
                }
                break;

            case BOP:
                f_in = 1;
                pos0 = ftell(fp_out);
                putc(code, fp_out);
                s = base;
                for(num = 0; num < 10; num++){  /* c[] */
                    s = get_next(s);
                    write_n(a2i(s), 4);
                }
                if(f_debug){
                    num0 = a2i(get_next(s));
                    if(pos != num0)
                        fprintf(stderr, 
                            "Address of BOP of page %d: %d -> %d\n",
                            page-1, num0, pos);
                }
                write_n(pos, 4);    /* position of the former bop */
                pos = pos0;
                break;

            case FNT_NUM_0:
                font_use[sub_number] = 1;
            case SET_CHAR_0:
                putc(code+sub_number, fp_out);
                break;

            case EOP:
                if(!f_in){
                    fprintf(stderr, "EOP without BOP\n");
                    goto err;
                }
                f_in = 0;
                if(stack > 0){
                    fprintf(stderr, "Stack remains at page %d. It is corrected\n", stack);
                    while(stack--)
                        putc(code, fp_out);
                }
                page++;
                putc(code, fp_out);
                break;

            case POP:
                if(--stack < 0){
                    fprintf(stderr, "Stack underflow\n");
                    goto err;
                }
                putc(code, fp_out);
                break;

            case PUSH:
                if(++stack > max_stack)
                    max_stack = stack;
                putc(code, fp_out);
                break;

            case FNT1:
                putc(code+sub_number-1, fp_out);
                num = a2i(get_next(base));
                if(num >= 0 && num < MAX_FONT)
                    font_use[num] = 1;
                write_n(num, sub_number);
                break;

            case PUT1:
            case SET1:
            case DOWN1:
            case RIGHT1:
                putc(code+sub_number-1, fp_out);
put_num:        write_n(a2i(get_next(base)), sub_number);
                break;

            case XXX1:
                s = get_next(base);
                num = a2i(s);
                s = get_next(s) + 1;
                if(f_dtl&DTL_EXACT){
                    len = StrLen(s); 
                    if(num != len)
                        fprintf(stderr, "string length in %d:[%s]: %d -> %d\n", 
                            line, tmp_buf, num, len);
                    num0 = (len < 0x100)?1:2;
                    if(sub_number != num0)
                        fprintf(stderr, "Code is changed at line %d: xxx%d -> xxx%d\n", 
                            line, sub_number, num0);
                    sub_number = num0;
                }else
                    len = num;
                putc(code+sub_number-1, fp_out);
                write_n(len, sub_number);
                goto strout;

            case EOFNC:
                putc(code, fp_out);
                sub_number = 1;
                goto put_num;

            case W0:
            case X0:
            case Y0:
            case Z0:
                putc(code+sub_number, fp_out);
                if(sub_number)
                    goto put_num;
                break;

            case SET_RULE:
            case PUT_RULE:
                putc(code, fp_out);
                s = get_next(base);
                write_n(a2i(s), 4);
                s = get_next(s);
                write_n(a2i(s), 4);
                break;

            case POST:
                if(f_in){
                    fprintf(stderr, "Need EOP before POST\n");
                    goto err;
                }
                if(f_proc > 1){
                    fprintf(stderr, "Multiple POST\n");
                    goto err;
                }
                f_proc = 2;
                pos0 = ftell(fp_out);
                putc(code, fp_out);
                s = get_next(base);
                if(f_debug){
                    num0 = a2i(s);
                    if(pos != num0)
                        fprintf(stderr, 
                            "Address of BOP of the last page: %d -> %d\n",
                             num0, pos);
                }
                write_n(pos, 4);        /* position of the last bop */
                pos = pos0;
                s = get_next(s);
                for(num = 5; num-->0; ){
                    write_n(a2i(s), 4);
                    s = get_next(s);
                }
                num0 = a2i(s);
                if(max_stack != num0)
                    fprintf(stderr, "Max stack depth: %d -> %d\n",
                        num0, max_stack);
                write_n(max_stack, 2);
                s = get_next(s);
                num0 = a2i(s);
                if(num0 != --page)
                    fprintf(stderr, "The number of total pages: %d -> %d\n", 
                        num0, page);
                write_n(page, 2);
                break;

            case FNT_DEF_1:
                s = get_next(base);
                num0 = a2i(s);
                if(f_dtl & DTL_FNTDFN){
                    if(num0 >= 0 && num0 < MAX_FONT){
                        if(f_proc == 2){
                             if(!font_use[num0]){
                                fprintf(stderr, "The definition of font %d is deleted\n", 
                                    num0);
                                break;
                            }else
                                font_use[num0] = 1;
                        }
                    }
                }
                putc(code+sub_number-1, fp_out);
                write_n(num0, sub_number);
                s = get_next(s);
                for(num = 0; num < 3; num++){   /* checksum/s-size/d-size */
                    if((f_dtl&DTL_FNTDEF) && !num && (s[1] != 'x' && s[1] != 'X')){
                        num0 = *(s-1);
                        *(s-1) = '0';
                        write_n(a2i(s-1), 4);
                        *(s-1) = (uchar)num0;
                    }else
                        write_n(a2i(s), 4);
                    s = get_next(s);
                }
                num = a2i(s);
                s = get_next(s);        /* directory */
                putc(num, fp_out);
                code = a2i(s);
                s = get_next(s) + 1;    /* directory/name */
                putc(code, fp_out);
                while(num-- > 0)
                    putc(*s++, fp_out); /* directory */
                if((*s == '\'' && s[1] <= ' ') || (f_dtl&DTL_FNTNAME))
                    s = get_next(s) + 1;
                while(code-- > 0)
                    putc(*s++, fp_out); /* name */
                break;

            case POST_POST:
                if(f_proc++ != 2){
                    fprintf(stderr, "Need POST before POST_POST!\n");
                    goto err;
                }
                putc(code, fp_out);
                s = get_next(base);
                write_n(pos, 4);        /* position of post */
                if(f_debug){
                    num0 = a2i(s);
                    if(pos != num0)
                        fprintf(stderr, "Address of POST: %d -> %d\n",
                            num0, pos);
                }
                for(num = 5; num-- > 0; ){
                    s = get_next(s);
                    code = a2i(s);
                    if(num == 4){
                        if(code != 2 && code != 3){
                            fprintf(stderr, "Last id (%d) should be 2 or 3\n", code);
                            goto err;
                        }
                    }else{
                        if(code != END_DVI)
                            fprintf(stderr, "Last code: %d -> %d\n", code, END_DVI);
                        code = END_DVI;
                    }
                    putc(code, fp_out);
                }
                num = ftell(fp_out);
                while(num++ & 3)
                    putc((uchar)END_DVI, fp_out);
                break;

            case OPCODE:
                putc(a2i(get_next(s)), fp_out);
                break;

            default:
err:            fprintf(stderr, "Error(line %d, code %d): %s\n", line, code, tmp_buf);
                Exit(1);
        }
    }
    num = ftell(fp_out);
    fprintf(stderr, "Write %d byte (%d page): %s\n", num, page,
        (fp_out==stdout)?"(stdout)":outfile);
    fclose(fp_in);
    fclose(fp_out);
    fp_in = fp_out = NULL;
}
