/* $Id: dvi2xx.h,v 2.3 1996/05/20 11:05:57 neumann Exp $ */
#ifndef DVI2XX_H
#define DVI2XX_H
/**********************************************************************
 ************************  Global Definitions  ************************
 **********************************************************************/
/* #define IBM3812 */
/* #define LJ */
/* #define DRAWGLYPH */

#ifdef KPATHSEA
#include <kpathsea/config.h>
#include <kpathsea/c-std.h>
#include <kpathsea/c-limits.h>
#include <kpathsea/magstep.h>
#include <kpathsea/proginit.h>
#include <kpathsea/progname.h>
#include <kpathsea/tex-file.h>
#include <kpathsea/tex-glyph.h>
#include <kpathsea/tex-hush.h>
#include <kpathsea/tex-make.h>
#include <kpathsea/version.h>
#include <stdarg.h>
#include <c-auto.h>
#else
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#ifdef  unix
#include <limits.h>
#endif
#endif

#include <signal.h>
#include <ctype.h>
#ifdef vms
#include <file.h>
#else
# ifndef __riscos
# include <fcntl.h>
# endif
#endif
#ifdef MSC5
#include <dos.h>     /* only for binaryopen on device  */
#endif


#include "config.h"
#include "commands.h"

#define  DVIFORMAT     2
#ifndef UNKNOWN
#define  UNKNOWN      -1
#endif
#define  FIRSTFNTCHAR  0

#ifdef __riscos
# ifdef RISC_USE_OSL
#  define MAXOPEN_OS    16
# else
#  define MAXOPEN_OS    8      /* don't know if this IS the maximum */
# endif
#else
# ifdef   OPEN_MAX                    /* ... in a friendly unix system  */
#  ifndef vms
#   define MAXOPEN_OS (OPEN_MAX - 8)
#  else
#   define  MAXOPEN_OS 12     /* OPEN_MAX seems to be 8 on VMS systems */
#  endif
# else
#  ifdef __DJGPP__
#   if __DJGPP_MINOR__ <= 1
    /* DJGPP needs few handles free in the first 20, or else child programs
       (called by MakeTeX... scripts) won't run, since the stub loader
       cannot open the .exe program.  This is because DOS only copies the
       first 20 handles to the child program.  */
#    define MAXOPEN_OS   5
#   else
    /* DJGPP v2.02 and later works around this.  Assume they have at least
       FILES=30 in their CONFIG.SYS (everybody should).  */
#    define MAXOPEN_OS  20
#   endif
#  else
#   define  MAXOPEN_OS  12     /* limit on number of open font files */
#  endif
# endif
#endif

#ifdef LJ_RESIDENT_FONTS
/* we have to read tfm files as well */
#define  MAXOPEN       (MAXOPEN_OS - 1)
#else
#define  MAXOPEN       MAXOPEN_OS
#endif

#define  NFNTCHARS       LASTFNTCHAR+1
#define  STACK_SIZE      100     /* DVI-stack size                     */
#define  NONEXISTANT     -1      /* offset for PXL files not found     */
#ifdef RISC_USE_OSL
# define  NO_FILE        (FPNULL-1)
#else
# define  NO_FILE        ((FILE *)-1)
#endif
#define  NEW(A) ((A *)  malloc(sizeof(A)))
#define  EQ(a,b)        (strcmp(a,b)==0)
#define  MM_TO_PXL(x)   (int)(((x)*RESOLUTION*10)/254)
#define  PT_TO_PXL(x)   (int)((long4)((x)*RESOLUTION*100l)/7224)
#define  PT_TO_DVI(x)   (long4)((x)*65536l)
#define  BINOPEN(f) BOPENCMD(f,READ_BINARY)
/* SMALL_SIZE characters are loaded into font storage of the printer   */
/* LARGE_SIZE characters are rastered                                  */
/* HUGE_SIZE characters are not loaded into the memory of the host     */
#define  SMALL_SIZE (unsigned char) 0
#define  LARGE_SIZE (unsigned char) 1
#define  HUGE_SIZE  (unsigned char) 2
#define  HUGE_CHAR_PATTERN 32767l
#define  BYTES_PER_PIXEL_LINE 500    /* max number of bytes per pixel line */
#define  MAX_SPECIAL_DEFPOINTS 80    /* max number of defpoint specials */


#define PK_POST 245
#define PK_PRE 247
#define PK_ID 89

/* to speedup the program a little: redefinition of PixRound and PutWord */
/*#define PIXROUND(x,c) ((((double)x+(double)(c>>1))/(double)c)+0.5)*/
#define PIXROUND(x,c) (((x)+c)/c)
#define PUTWORD(w)  EMITC( ((unsigned char)(w>>8) & 0xff)), EMITC((unsigned char)(w & 0xff))
/*************************************************************************/
#ifdef RISC_BUFFER
# define   EMIT           emsize = sprintf
# define   EMFLUSH        b_wrtmult(outfp,embuf,emsize)
# define   EMTO           embuf
# define   EMIT1(a)       emsize = sprintf(embuf,a), EMFLUSH
# define   EMIT2(a,b)     emsize = sprintf(embuf,a,b), EMFLUSH
# define   EMIT3(a,b,c)   emsize = sprintf(embuf,a,b,c), EMFLUSH
# define   EMIT4(a,b,c,d) emsize = sprintf(embuf,a,b,c,d), EMFLUSH
#else
# ifdef RISC_USE_OSL
#  define  EMIT           emsize = sprintf
#  define  EMFLUSH        write_multi(embuf,1,emsize,outfp)
#  define  EMTO           embuf
#  define  EMIT1(a)       emsize = sprintf(embuf,a), EMFLUSH
#  define  EMIT2(a,b)     emsize = sprintf(embuf,a,b), EMFLUSH
# define   EMIT3(a,b,c)   emsize = sprintf(embuf,a,b,c), EMFLUSH
# define   EMIT4(a,b,c,d) emsize = sprintf(embuf,a,b,c,d), EMFLUSH
# else
#  define  EMIT           fprintf            /* output a formatted string   */
#  define  EMTO           outfp
#  define  EMIT1(a)       fprintf(outfp,a)
#  define  EMIT2(a,b)     fprintf(outfp,a,b)
#  define  EMIT3(a,b,c)   fprintf(outfp,a,b,c)
#  define  EMIT4(a,b,c,d) fprintf(outfp,a,b,c,d)
# endif
#endif
#ifndef vms
# ifdef RISC_BUFFER
#  define  EMITB(len,b)   b_wrtmult(outfp,b,len) /* output binary data of len*/
# else
#  define  EMITB(len,b)   write_multi(b,1,len,outfp)
# endif
#else
    /* VMS doesn't like to use fwrite on a file with fixed record sizes,
       so use number of putc calls */
# define  EMITB(len,b)   for (kk = 0;kk < len; kk++) fputc(*(b+kk),outfp);
#endif
#define  EMITWORD(w)     PUTWORD((w))        /* output a 2 byte word of data */

#define  MoveOver(b)  h += (long4) b
#define  MoveDown(a)  v += (long4) a
#define  qfprintf if (!G_quiet) fprintf
#define  qprintf  if (!G_quiet) printf
#define  LARGER(a,b) (((a)>(b)) ? (a) : (b))

#ifdef IBM3812
#define  PRINTER      "IBM 3812 pageprinter"
#define  EMITC(c)      PMPoutC(c)               /* output a single character */
#define  PMPcont(l)    PMPout(-1,(char *)l)       /* next l bytes continuous */
#define  PMPflush      PMPout(0l,"")                     /* flush PMP-buffer */
#define  EMITL(l,d)    PMPout((int)l,d)      /* EMIT-logical: via PMP-buffer */
#define  hconvRESOLUTION   240
#define  vconvRESOLUTION   240
#define  CHAR_WIDTH_LARGE  100       /*  limit for loading into printer font */
#define  CHAR_HEIGTH_LARGE 127       /*  limit for loading into printer font */
#define  OUTBUFSIZE     20000        /*   size of output buffer for PMP cmds */
                      /*   has to be less max(signed int)     */
#define  MAXFONTSTORAGE      130000l /* font storage in the 3812 pageprinter */
#define  EMITFILE_EXTENSION    ".pmp"      /* default extension of emit file */
#define  XDEFAULTOFF     ((int)(0.72*RESOLUTION))/* default x and y offset   */
#define  YDEFAULTOFF    RESOLUTION
#define  CHARSTRINGMAX  80                /* bufferlength for SetString      */
#define  MAX_PAGE_WIDTH  2040
#define  MAX_PAGE_HEIGHT 3360
/**********************************************************************/
/**************  Positioning for the 3812  ****************************/
/**********************************************************************/
#define VERT_HALF(n) ((short)((n+1)>>1)-1)
#define HOR_HALF(n)  ((short)(n>>1))
#define MoveHor(n)  if ((n)!=0) { PMPcont(3); PMPout(1,"\342"); EMITWORD((n)); }
#define MoveVert(n) if ((n)!=0) { PMPcont(3); PMPout(1,"\343"); EMITWORD((n)); }
#endif /* IBM 3812 */

#ifdef LJ
# ifdef LJ4
#  ifdef LJ4L
#  define  PRINTER       "HP Laserjet 4L"
#  else
#  define  PRINTER       "HP Laserjet 4"
#  endif
# else
#  ifdef LJ2P
#   define  PRINTER       "HP LaserJet IIP"
#  else
#   ifdef LJ2
#    define  PRINTER       "HP LaserJet 2"
#   else
#    define  PRINTER       "HP LaserJet"
#   endif
#  endif
# endif

# ifdef LJ4
#  ifdef LJ4L
int   RESOLUTION = 300;
const char *MFMODE     = MFMODE300;
#  else
int   RESOLUTION = 600;
const char *MFMODE     = MFMODE600;
#  endif
# else
#  define RESOLUTION 300
# endif
# define  hconvRESOLUTION   RESOLUTION
# define  vconvRESOLUTION   RESOLUTION
# ifdef LJ2
/* the printer limit of the LJ2P is actually 16384x16384,
  * but to exploit it, one would need lots of memory in the printer
 */
#  define  CHAR_WIDTH_LARGE  200     /* limit for loading into printer font */
#  define  CHAR_HEIGTH_LARGE 255         /* y_offset reaches the same size! */
# else   /* such as LaserJet+, Laserjet II */
#  define  CHAR_WIDTH_LARGE  100     /* limit for loading into printer font */
#  define  CHAR_HEIGTH_LARGE 127         /* y_offset reaches the same size! */
# endif
# define  EMITFILE_EXTENSION  ".lj"       /* default extension of emit file */
# ifndef MAX_FONTS_PER_PAGE
#  define  MAX_FONTS_PER_PAGE 16        /* maximum number of fonts per page */
# endif
# define  HANDLE_MAX_FONTS  255      /* max nr of fonts handled (rasterfont) */
# define  FONTS_DOWNLOADABLE 32    /* max nr of fonts that can be downloaded */
# ifdef SHARP_JX_9500
#  define  MAXFONTSTORAGE (200l*1024l)               /* standard user memory */
# else
#  define  MAXFONTSTORAGE (395l*1024l)               /* standard user memory */
# endif
# ifdef RISC_BUFFER
#  define EMITC(c)       b_write(outfp,c)       /* output a single character */
# else
#  define EMITC(c)       write_byte(outfp,c)    /* output a single character */
# endif
# define  EMITL(l,d)     EMITB(l,d)                  /* EMIT-logical = EMITB */

# define  XDEFAULTOFF   ((int)(0.72*RESOLUTION))
# define  YDEFAULTOFF   RESOLUTION
#ifdef NEVER
# define  XDEFAULTOFF   RESOLUTION   /*x default offset on page 1in (LJ2)*/
# define  YDEFAULTOFF   RESOLUTION    /* y default offset on page 1inch */
#endif
# ifndef vms
#  define  INT_ASCII(b,i) \
   if (i == 0) b[0] = '\0'; else sprintf((char *)b,"%hd",i)
# else
#  define  INT_ASCII(b,i) \
   if (i == 0) b[0] = '\0'; else sprintf((char *)b,"%d",i)
# endif
#endif


#ifndef SEVENBIT
#define VisChar(c) (unsigned char)(c)
#endif

/* Used to be a function. buf is always an array, never a pointer.
   Without that invariant, we would have to introduce full dynamic
   memory management in this driver -- probably it would be easier to
   write a new one. [27 Jun 07 -js] */
#define GetBytes(fp,buf,n) \
    ( sizeof(buf) != sizeof(void *) && sizeof(buf) > n ? \
        (void)read_multi(buf, 1, n, fp)					\
      : Fatal("Try to read %d bytes in an array of size %d", n, sizeof(buf)) )


/**********************************************************************/
/***********************  external definitions  ***********************/
/**********************************************************************/

#ifndef WIN32
#ifndef _AMIGA
# ifndef unix
long    access(char *, int);      /* all the other ones known under RISC OS */
#  ifndef __riscos
FILEPTR BOPENCMD();
void    exit();
int     fclose();
int     fprintf();
int     fseek();
/*char   *index();*/
int     printf();
int     sscanf();
int     strcmp();
char   *strcpy();
char   *strncpy();
#   ifdef MSC5
unsigned int strlen();
#   endif
void    free();
void    setbuf();
#  endif

#  ifdef MSC5
int     intdos();
#  endif
# endif
#endif
#else /* WIN32 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#endif

#ifndef USEPXL
/* interface to gf.c */
extern FILEPTR gfin;
extern int checksum;
extern long4 tfm_wd[], char_pointer[];
extern char char_exists[];
extern int num_cols, num_rows, num_bytes, x_offset, y_offset;
extern unsigned char bits[];
extern int gf_font_max_m, gf_font_max_n, gf_font_min_n;
extern int gettochar();
extern void readbits();
extern void readpost();
extern void seekpost();
extern int seekchar();
#endif

/**********************************************************************/
/********************** Special Data Structures ***********************/
/**********************************************************************/

typedef enum { None, String, Integer /*, Number, Dimension*/ } ValTyp;
typedef struct {
  char    *Key;       /* the keyword string */
  char    *Val;       /* the value string */
  ValTyp  vt;         /* the value type */
  union {         /* the decoded value */
    int     i;
    float   n;
  } v;
} KeyWord;
typedef struct {
  int     KeyId;     /* the keyword ID */
  const char    *Entry;
  ValTyp  Typ;
} KeyDesc;

/**********************************************************************/
/***********************  Font Data Structures  ***********************/
/**********************************************************************/

struct char_entry {             /* character entry */
#ifdef USEPXL
    unsigned short  width, height;      /* width and height in pixels */
    short   xOffset, yOffset, yyOffset; /* x offset and y offset in pixels*/
#endif
    struct {
        bool isloaded;
        union {
            long4    fileOffset;
            long4    *pixptr;
        } address;
    } where;
    long4    tfmw;             /* TFM width                 */
    long4    cw;               /* character width in pixels */
    unsigned char   flag_byte;          /* for PK-files    */
    unsigned char   charsize;
};
struct font_entry {    /* font entry */
    long4    k, c, s, d;
    int     a, l;
    char n[STRSIZE];          /* FNT_DEF command parameters                */
    long4    font_mag;         /* computed from FNT_DEF s and d parameters  */
    /*char psname[STRSIZE];*/ /* PostScript name of the font               */
    char    *name;	       /* full name of PXL file                     */
    FILEPTR font_file_id;      /* file identifier (NO_FILE if none)         */
#ifdef USEPXL
    long4    magnification;    /* magnification read from PXL file          */
    long4    designsize;       /* design size read from PXL file            */
#endif
    struct char_entry ch[NFNTCHARS];   /* character information            */
    struct font_entry *next;
    unsigned short ncdl;      /* #of different chars actually downloaded   */
    unsigned short plusid;    /* Font id in Printer                        */
    bool used_on_this_page;
#ifdef LJ_RESIDENT_FONTS
    bool resident_p;          /* is font resident in printer?              */
    char symbol_set[40];      /* symbol set value (resident fonts)         */
    unsigned short resid;     /* typeface id (resident fonts)              */
    unsigned spacing;         /* 0=monospace, 1=variable (resident fonts)  */
    unsigned style;           /* upright/italic/... (resident fonts)       */
    int weight;               /* regular/bold/... (resident fonts)         */
    double pitch;             /* chars per inch (monospaced resident fonts)*/
#endif
    enum PxlId {
        id1001, id1002, pk89    } id;
#ifdef LJ
    unsigned short max_width, max_height, max_yoff;
#endif
};


struct pixel_list {
    FILEPTR pixel_file_id;    /* file identifier  */
    int     use_count;        /* count of "opens" */
};

#ifdef __riscos
typedef struct {
  int scalex;
  int scaley;
  int cropl;
  int cropb;
  int cropr;
  int cropt;
} diagtrafo;                  /* to be passed to diagrams */
#endif


/**********************************************************************/
/*************************  Global Procedures  ************************/
/**********************************************************************/
/* Note: Global procedures are declared here in alphabetical order, with
   those which do not return values typed "void".  Their bodies occur in
   alphabetical order following the main() procedure.  The names are
   kept unique in the first 6 characters for portability. */

double  ActualFactor(long4);
void    AllDone(bool);
#ifdef  MSC5
void    AssureBinary(FILEPTR);  /* DOS and Microsoft C dependent !!! */
#endif
void    CloseFiles(void);
void    my_CopyFile(const char *);
void    CopyHPFile(char *);
void    DecodeArgs(int, char *[]);
#ifdef __riscos
void    diagram(char *, diagtrafo *);
void   *xosfile_set_type(char *, int);
void    MakeMetafontFile(char *, char *, int);
#endif
void    DoBop(void);
long4   DoConv(long4, long4, int);
void    DoSpecial(char *, int);
void    EmitChar(long4, struct char_entry *);
void    FindPostAmblePtr(long *);
void    FormFeed(void);
void    GetFontDef(void);
char    *GetKeyStr(char *, KeyWord *);
bool    GetKeyVal(KeyWord *, KeyDesc[], int, int *);
bool    IsSame(const char *, const char *);
void    LoadAChar(long4, register struct char_entry *);
void    OpenFontFile(void);
long4   PixRound(long4, long4);
void    PkRaster(struct char_entry *, int);
void    RasterLine(struct char_entry *, unsigned int,
                   unsigned int, unsigned char *);
void    RasterChar(struct char_entry *);
void    ReadFontDef(long4);
void    ReadPostAmble(bool);
void    SetChar(long4, short, int, bool, bool);
void    SetFntNum(long4, bool);
void    SetPosn(long4, long4);
void    SetRule(long4, long4, int);
void    SetString(short, int);
long4   SignExtend(FILEPTR, int);
void    SkipFontDef(void);
unsigned char   skip_specials(long4 *);
#ifdef LJ4
int     CompressLine2(unsigned char *, unsigned char *, int);
int     CompressLine3(unsigned char *, unsigned char *, int);
void    CompressedCharLine(struct char_entry *,int,unsigned char *);
#endif
#ifdef IBM3812
void    PMPout(int, char *);
void    PMPoutC(char);
#endif

/* buffer IO */
char   b_read(FILEPTR);
#ifdef RISC_BUFFER
void   b_write(FILEPTR, char);
void   b_wrtmult(FILEPTR, char *, int);
void   b_oflush(FILEPTR);
#endif


/**********************************************************************/
/*************************  Global Variables  *************************/
/**********************************************************************/
bool    ManualFeed = _FALSE;
long4    FirstPage  = -1000000;  /* first page to print (uses count0)   */
long4    LastPage   = 1000000;   /* last page to print                  */
long4    PrintPages = 1000000;   /* nr of pages to print                */
bool    FirstPageSpecified = _FALSE;
bool    LastPageSpecified = _FALSE;
#ifndef KPATHSEA
char   *PXLpath = FONTAREA;
#endif
char   *G_progname;		 /* program name                        */
char   *filename;	         /* DVI file name                       */
char   *rootname;		 /* DVI filename without extension      */
const char   *HeaderFileName = "";     /* file name & path of Headerfile      */
const char   *EmitFileName = "";       /* file name & path for output         */
char    tmp_dir[STRSIZE] = "";	 /* temporary directory for auxiliary files */
enum   { Ignore, PSFile_dvilj /*, PSFile_dvips */ } PSFileSyntaxTyp = PSFile_dvilj;
#ifdef IBM3812
bool    FirstAlternate = _FALSE; /* first page from alternate casette ?   */
#endif
bool    Reverse = _FALSE;        /* process DVI pages in reverse order?   */
bool    Landscape = _FALSE;      /* print document in ladscape mode       */
bool    my_ResetPrinter = _TRUE; /* reset printer at the begin of the job */
bool    DoublePage = _FALSE;     /* print on both sides of a paper        */
bool    PrintSecondPart = _TRUE; /* print First Part when DoublePage      */
bool    PrintFirstPart  = _TRUE; /* print Second Part when DoublePage     */
bool    PrintEmptyPages = _TRUE; /* print Empty pages in DoublePage mode  */
short   PageParity = 1;
#ifdef MAKETEXPK
#ifdef KPATHSEA
bool    makeTexPK = MAKE_TEX_PK_BY_DEFAULT;
#else
bool    makeTexPK = _TRUE;
#endif
#endif

#ifdef LJ
bool    kyocera_mode = _FALSE;  /* bug fixes for Kyocera F-1200 LJ-Emulation */
bool    brother_mode = _FALSE;  /* bug fixes for Brother HL-8D LJ-Emulation */
#ifdef LJ2P
int     DuplexMode = 0;
#endif
#ifdef LJ4
bool    econoMode = _FALSE;
bool    LJ6 = _FALSE;
#endif
bool    PrintTestPage = _FALSE; /* print testpage with pagecounter after job */
unsigned short pagesize = 0;    /* page size value                      */
unsigned short pgsiz_dots = 0;  /* page size in dots (for rule-clipping)*/
#endif


#ifndef vms
short   G_errenc = 0;           /* has an error been encountered?      */
#else
long4    G_errenc = SS$_NORMAL;  /* has an error been encountered?      */
#endif
bool    G_header = _FALSE;      /* copy header file to output?         */
bool    G_quiet = _FALSE;       /* for quiet operation                 */
bool    G_verbose = _FALSE;     /* inform user about pxl-files used    */
bool    G_nowarn = _FALSE;      /* don't print out warnings            */
short   x_origin;               /* x-origin in dots                    */
short   y_origin;               /* y-origin in dots                    */
short   x_goffset;              /* global x-offset in dots             */
short   y_goffset;              /* global y-offset in dots             */
unsigned short ncopies = 1;     /* number of copies to print           */
long4    hconv, vconv;           /* converts DVI units to pixels        */
long4    h;                      /* current horizontal position         */
long4    hh = 0;                 /* current h on device                 */
long4    v;                      /* current vertical position           */
long4    vv = 0;                 /* current v on device                 */
long4    mag;                    /* magnification specified in preamble */
long     usermag = 0;            /* user specified magnification        */
int      ndone = 0;              /* number of pages converted           */
int      nopen = 0;              /* number of open PXL files            */
#ifdef vms
int	kk;			 /* loop variable for EMITB	       */
#endif
FILEPTR outfp = FPNULL;          /* output file                         */
FILEPTR pxlfp;                   /* PXL file pointer                    */
FILEPTR dvifp  = FPNULL;         /* DVI file pointer                    */
struct font_entry *prevfont = NULL; /* font_entry pointer previous font*/
struct font_entry *fontptr;      /* font_entry pointer                  */
struct font_entry *hfontptr = NULL; /* font_entry pointer              */
struct font_entry *pfontptr = NULL; /* previous font_entry pointer     */
struct pixel_list pixel_files[MAXOPEN+1]; /* list of open PXL files    */
long   postambleptr;            /* Pointer to the postamble            */
long   ppagep;                  /* previous page pointer               */
static int      last_ry = UNKNOWN;      /* last y-position on page     */
static int      last_rx = UNKNOWN;      /* last x-position on page     */
long4  StartPrintPages;         /* notpad for double paged output      */
int    WouldPrint    = 0;
bool   ZeroPage = _FALSE;       /* Document starts with a Zero Page    */
bool   EvenPage = _FALSE;       /* Document starts with an even Page   */
long4  LastPtobePrinted = 0;
int    G_ncdl = 0;

long     allocated_storage = 0; /* size of mallocated storage (statistics) */
long4    power[32] ;
long4    gpower[33] ;

unsigned char buffin[BUFFSIZE]; /* Input buffer; always used for Copy[HP]File */
int binumber=0;            /* number of valid bytes in input buffer */
int biact=0;               /* number of next byte to read from input buffer */
#ifdef RISC_BUFFER
char buffout[BUFFSIZE];    /* Output buffer; used if RISC_BUFFER defined */
int boact=0;               /* number of next byte to write to output buffer */
#endif

#ifdef LJ4
# define DEFAULT_COMPRESS_MODE  3
# define DEFAULT_COMPRESS_WIDTH 0
# define COMPRESS_WIDTH0        0
# define COMPRESS_WIDTH2        72
# define COMPRESS_WIDTH3        0
/* Raster chars compression */
int CompressCharWidth = -1;  /* Value to be determined dependent at runtime */
int CompressCharMode;        /* Used only from within RasterChar/Line */
int UseCompression = DEFAULT_COMPRESS_MODE; /* This performs very well with Characters */
/* Shared by both compression types */
unsigned char *PrevLine = NULL; /* Holds last raster line; set at runtime */
int PrevSize = 0;               /* Size of PrevLine */
/* Downloaded chars compression */
int CChar_Off, CChar_Last;      /* For compressed character downloading */
bool CompressFontMode = _TRUE;  /* Download characters compressed */
#endif /* LJ4 */

#ifdef RISC_USE_OSL
char   embuf[STRSIZE];         /* Buffer for emitting stuff */
int    emsize;                 /* Number of bytes written in buffer */
#else
# ifdef RISC_BUFFER
char   embuf[STRSIZE];
int    emsize;
# endif
#endif

#ifdef __riscos
#define DIAGDIRSIZE 32
char diagdir[DIAGDIRSIZE] = "LJdiag"; /* Prefix name of directory for
					 cached printouts */
bool cachediag = _FALSE;       /* cache PDriver's output in document folder */
bool printdiag = _TRUE;        /* printf diagrams */
FILEPTR metafile = FPNULL;     /* Filepointer of file containing
				  metafont directives*/

char MFFileName[STRSIZE];
int RasterMultipass = 0;
#endif

#ifdef DEBUG
int Debug = 0;
#define DEBUG_START() do { if (Debug) {
#define DEBUG_END()        fflush (stdout); } } while (0)
#define DEBUG_PRINT(str)						\
  DEBUG_START (); fputs (str, stdout); DEBUG_END ()
#define DEBUG_PRINT1(str, e1)						\
  DEBUG_START (); printf (str, e1); DEBUG_END ()
#else
#define DEBUG_PRINT(str)
#define DEBUG_PRINT1(str, e1)
#endif

#ifdef LJ
int   fonts_used_on_this_page = MAX_FONTS_PER_PAGE+1;
char  rasterfont[HANDLE_MAX_FONTS];
    /* raster if fonts/page>MAX_FONTS_PER_PAGE*/
#ifdef LJ_RESIDENT_FONTS
unsigned resident_count = 0;
#ifndef KPATHSEA
char *TFMpath = DEFAULT_TFM_PATH;
#endif
#endif
#endif

long     used_fontstorage = 0;

#ifdef IBM3812
char    PMPformat[20];
char    CharString[CHARSTRINGMAX];
unsigned int CharStringPos = 0;
#define CharStringOut \
    if (CharStringPos>0) { \
        PMPcont(CharStringPos+1);\
        PMPoutC((unsigned char)CharStringPos);\
        PMPout(CharStringPos, CharString); \
        CharStringPos=0; }
#endif


/************************timing stuff*********************/
#ifdef TIMING
# ifdef BSD_TIME_CALLS
#  ifndef vms
#   include <sys/timeb.h>
#  else
#   include <timeb.h>
#  endif
struct timeb timebuffer;
double  start_time;
# else
#  ifdef __riscos
#   include <sys/times.h>
#  else
#   include <sys/time.h>
struct timeval Tp;
double  start_time;
#  endif
# endif
#endif /* TIMING */


#endif /* DVI2XX_H */
