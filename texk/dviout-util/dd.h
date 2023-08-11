/*
 *  TEX Device Driver  ver 2.02-
 *  copyright(c) 1988,89 by TSG, 1990- by SHIMA
 *  1991 changed by hideki
 *
 *  dd.h : common header file
 *
 */
#ifndef INC_DD_H
#define INC_DD_H

#ifndef	__STDIO_H
#include <stdio.h>
#endif

/* reduce harmless warning messsages */
#if	defined(MSVC) && !defined(_WARNING_)
#define	_WARNING_
#pragma warning( disable : 4244 4018 4028 4102 4761 4146 4047 4305 4113 4245)
#endif

#ifdef	WIN32
#define	WIN32G
#define	STR_SEARCH
#define USE_COLOR
#define	HYPERTEX
#define	PLUG	1
#define	DOUBLE_PAGE
#define	DVI_ADD
#define	USE_ETF
#endif

#if	defined(WIN32G) || defined(WIN32C)
#define	USE_WINAPI
#define	MAX_KPS_DEPTH	16
#elif defined(UNIX)
#define	MAX_KPS_DEPTH	16
#endif

#ifdef	USE_WINAPI
#define	USE_WINFONT
#endif

#ifdef	NOPS
#define	NOPSFRAG
#endif

int		strlcmp(const char *, const char *);

#ifndef	MSVC
long	filelength(int);
int 	strcmpi(const char *, const char *);
#ifndef GCC
char	*stpcpy(char *, const char *);
#endif
#endif

#if !defined(GCC1)  || defined(UNIX)
#define	GetCh()				getch()
#define	KbHit()				kbhit()
#endif

#ifdef  UNIX
#define MAXPATH 256
#define MAXDRIVE 1
#define MAXDIR  256
#define MAXFILE 128
#define MAXEXT  128
#define O_BINARY 0
#endif

#ifndef	max
#define max(a,b) ( (a) > (b) ? (a) : (b) )
#define min(a,b) ( (a) < (b) ? (a) : (b) )
#endif

/* standard definitons */

#define uchar   unsigned char
#define uint    unsigned int
#define ulong   unsigned long
typedef void (*void_func_ptr) ();
typedef int (*int_func_ptr) ();
typedef int (* COMP)(const void *, const void *);

#ifdef FALSE
#undef FALSE
#endif
#ifdef TRUE
#undef TRUE
#endif
#ifdef BOOL
#undef BOOL
#endif
typedef enum{
	FALSE = 0,
	TRUE  = 1
} BOOL;

#define NOTHING 0
#define FAILURE (-1)
#define NOMORE  (-2)

#ifndef SEEK_SET
#define SEEK_SET    0
#define SEEK_CUR    1
#define SEEK_END    2
#endif

#if	defined(LIPS3)||defined(ESCPAGE)
#define LBP
#endif

#ifdef	LBP
#define	FDOWN
#else
#undef	JDWN
#endif

#if !defined(JDWN) && defined(JGAIJI)
#undef JGAIJI
#endif

void Free0(void *);

#ifdef	WIN32
void Free(void *);
#define get_near_work(x) marea(x)
#else
#define	Free(x)	free(x)
extern uchar *get_near_work(uint);
#endif

/* definitons for device driver */


//#ifdef	GCC
#define far
#define	huge
//#endif

#define BUFFER    uchar far
#define HUGE_BUF  uchar huge
 /*   Both BUFFER pointer and HUGE_BUF pointer are 32bit-pointer.
	 *   HUGE_BUF will be used for larger buffers ( correctly speaking,
	 * ones which keep more data than 64Kbytes ).
	 *   And it is noteworthy that the pointers of these types are impossible
	 * to express continually ( ex. BUFFER *a, *b ), because they are not
	 * "typedef" but "#define."  This foolish error is because of 8086CPU,
	 * which has only 16bit-pointer, so we must add "far" or "huge" for use of
	 * 32bit-pointer. However, "far" and "huge" are permitted to "typedef."
	 * How wonderful SEGMENT !!
	 */
#ifdef	NOFLOAT
#define SIZE_PARA   long
#else
#define SIZE_PARA   double
#endif
 /*   This device driver calculate units with double.
	 */
#define F_SIZE_PARA   long
 /*   This device driver calculate units with double.
	 */
#define PIXEL       int
#define SCALED_PT   long
 /*   the units of pixel and scaled-point
	 */

#define HEADER_DEPTH    0xe0000L
#define FOOTER_DEPTH    0xe0000L
 /*   the header depth and the footer depth ( scaled point )
	 */

typedef enum {
	UNKNOWN, ONDEMAND, ONDEMAND2, UNRESOLVE, 
	PK_FONT, PXL1001, PXL1002, PXL1003, PKD_FONT, JXL, 
	SUBTFM, 
#ifdef VFD		/* Fonts before JXL should be unscalable */
	VFD_FONT,
#endif
#ifdef TTFONT	/* European TrueType fonts (expand by dviout) */
	CMTTF,
#endif
#ifdef	USE_WINFONT	/* Europian TrueType fonts (WinAPI) */
	WINTT_FONT,
#endif
#ifdef	VIRTUALFONT
	VIRTUAL_FONT,
#endif
	SUBJFM, ROM_FONT, ZS_FONT,
#ifdef BEZIERFONT
	JG_FONT,
#endif
#ifdef TTFONT	/* Japanese TrueType fonts (expand by dviout) */
	TT_FONT,
#endif
#ifdef	USE_WINFONT	/* Japanese TrueType fonts (WinAPI) */
	WINJTT_FONT,
#endif
#ifdef	USE_ETF
	ETF_FONT,
#endif
	JLBP, JIS_FONT, JIS_PK, JIS_PKH, GAIJI
} FONT_TYPE;

 /*   FONT_TYPE shows the type of fonts */

typedef enum {
	HORIZONTAL, VERTICAL
} PRINT_DIR;

 /*   Direction at printing, HORIZONTAL normally.
	 */

typedef enum {
	PreviousPart, NextPart,
	PreviousPage, NextPage,
	NextPageDirection,
	EscapeQuit
} NEXT_ACTION;

 /*   These values are returned by device-output module, which says
	 * what to do next.
	 */

#define PREAMBLE struct PREAMBLE_REC far
 /*   Information about Preamble of a font-file
	 */
struct PREAMBLE_REC {
	PIXEL width, height, pitch_offset, depth_offset;
	/*   the size of the character, and the offset for typesetting.
		 */
	BUFFER *raster;
	/*   the pointer of Raster-data buffer
		 */
	long tfm_width;
	int byte_width;
	/*   equal width/8, raised decimal
		 */
	PIXEL shift_up_ptex, shift_right_ptex;
	/*  the offset when lateral-Kanji used (when in pTeX tate mode)
		 */
	int rotate_ptex;
	/* the flag of rotation (when in pTeX tate mode)
		 */
};

#define CHAR_INFO struct CHAR_INFO_REC far
 /*   This buffer is a linked-list one, which keeps pointers of unpacked
	 * fonts.
	 */
struct CHAR_INFO_REC {
	uint code;
	/*   character code
		 */
	CHAR_INFO *next_char;
	CHAR_INFO *former_char;
	/*   needless to say? */
	struct PREAMBLE_REC pre;
	/*   preamble of an unpacked font
		 */
};

#define	f_HUGE	0x8000
#define	f_EMS	0x4000

#define BUF_INFO  struct BUF_INFO_REC
 /*   Information about buffers
	 */
struct BUF_INFO_REC {
	BUFFER *start;
	BUFFER *current;
	BUFFER *end;
	/*   Each means starting, current, and ending pointer of the buffer.
		 * Especially, 'end' means 'start+size', so the pointer above ( or
		 * which equals )'end' will be out of the buffer.
		 */
	long size;
	int flush;
};

typedef struct {
	long offset;
	int  number;
#ifdef	HYPERTEX
	int	 hyper0;
#endif
}
PAGE_INDEX;
/* _REC;

#define PAGE_INDEX PAGE_INDEX_REC far
*/

 /* page index buffer */

#define DVIFILE_INFO struct DVIFILE_INFO_REC
 /*   Information about Preamble of DVI-file. ( to say the truth, this is
	 * the same thing as Preamble itself. So, you'd better see
	 * "TeX : The Program." )
	 */
struct DVIFILE_INFO_REC {
	FILE *file_ptr;
	char *file_name;
	long post, last_bop;
	long num, den, mag;
	SCALED_PT l, u;
	int stack_depth;
	int total_page;
};

#define DIMENSION struct DIMENSION_REC
 /*  Information about device-output
	 */

struct DIMENSION_REC {
	int dpi;
	/* is horizontal dot-per-inch ratio of the device
		 */
	int DPI;
	/* is vertical dot-per-inch ratio of the device .
		 */
	PIXEL max_width, max_height;
	/*   These tell us the maximum size the device can output.
		 * They are represented by pixel-unit.
		 */
	PIXEL output_width, output_height;
	/* is the final size to be output to the device
		 */
	PIXEL buf_width, buf_height;
	/*   These tell us the maximum size Bitmap-buffer can hold.
		 * oriented by text direction
		 */
	PIXEL text_width, text_height;
	/*   These are the maxmum size directed in DVI-file.
		 */
	int mag;
	/*  is the magnification for output. This is the same value as 'mag' in
		 * DVI-file ( so equal the one in DVI_INFO ), when no '-mag' & '-half'
		 * options directed.
		 */
	int split;
	/*   Our Device-driver divides the text to several parts, if that can't
		 * be output at once. 'split' is the number of parts. Then, if 'split'
		 * holds '1', Device-driver will put the text at a time.
		 */
	PRINT_DIR print_direction;
	/*   This shows the direction for output, responsing '-V' option.
		 * So there is no meaning for PREVIEWER.
		 */
	SIZE_PARA size_para;
	/* is size-parameter for conversion pixel<-->scaled pt.
		 * ( equal "num * den / mag" )
		 */
	PIXEL x_offset, y_offset;
	/*   the offset of the head of text. These are directed by '-X' and
		 * '-Y' options.
		 */
	PAGE_INDEX *page_index;
	/* the index of file-offset of pages in dvifile
		 */
	int total_page;
	/* same as 'total_page' in DVI_INFO.
		 */
	int start_page, end_page;
	/* the starting and the ending page to print.
		 */
	int max_nombre_page;
	/* the maximal nombre page
		 */
	int prt_type;
	/* the type of the printer, ESC/P, PC-PR, PC-MN, LIPS3 or Previewer etc
		 */
};

#define	NO_PRN_OUT	0x1000

#define OUTPUT_INFO struct OUTPUT_INFO_REC
 /*   Information about output. Interpreting-module and Device-output-module
	 * receive the pointer of this.
	 */
struct OUTPUT_INFO_REC {
	FILE *dvifile_ptr;
	/* DVI-file pointer
		 */
	long page_start_offset;
	/* the file-offset at the position the page starts from.
		 */
	PRINT_DIR print_direction;
	/* equal the one in DIMENSION.
		 */
	PIXEL h_0, v_0;
	/* the original point for output.
		 */
	PIXEL width, height;
	/* the size for output.; it is assigned for all the size of the text
		 * to be held in the buffer.
		 */
	int byte_width, byte_height;
	/*  equal width/8 and height/8.
		 */
	int split, page;
	/* the current part and page.
		 */
	HUGE_BUF *bitmap_ptr;
	/* Bitmap pointer
		 */
};

typedef struct {	/* KANJI fonts */
	int width;	/* width by dots  (without counting K_DBL) */
	int height;	/* height by dots (without counting K_DBL) */
	int size;	/* size by bytes = (width+7)/8*height */
	int base;	/* number of dots under base line */
	int fh;		/* file handle number */
	char *name;	/* Kanji font file_name */
}
KFONT;

typedef struct {
	int char_type;
	int id;
	long width;		/* character width */
	long height;	/* character height(length above the baseline) */
	long depth;		/* character depth (length under the baseline) */
	long italic;
	long glue;
	long plus;
	long minus;
}
JFM_DATA;

#define	KFONT_SIZE  sizeof(KFONT)

#define FONT_INFO struct FONT_INFO_REC far
 /*   Information about fonts. This is linked-list.
	 */
union EXT {
	BUFFER *dir;	/* PXL1001, PXL1002, PXL1003
					   address of the directory for PXLFONT 	*/
	KFONT *kdir;	/* JLBP, JIS_FONT, JIS_PK, JIS_PKH, GAIJI	*/
	int fh;			/* JXL, PKD_FONT, VFD_FONT (file handle)	*/
					/* ZS_FONT, JG_FONT, TT_FONT (use as ID)	*/
	long count;
#ifdef	VIRTUALFONT
	FONT_INFO	*local_font;
#endif
};

#define	CHAR_ROOT		8		/* should be a power of 2 */
#define MAX_NAME_LEN	128

struct FONT_INFO_REC {		/* 4*CHAR_ROOT + 46 = 78 bytes
											   +  4 = 82 bytes (FDOWN)
											   +  6 = 88 bytes (NTTRPL)*/
	int font_code;
	/*   sent from dvi-file, when the font is defined.
		 */
	FONT_TYPE font_type;
	/*   type of font.
		 */
	FONT_TYPE etf_type;
	/*   type of ETF_FONT.
		 */
	int f_goth;
	/* flag for gothic etc.
		 */
	char *name;
	/*   the file-name of the font.
		 */
	BUFFER *pk;
	/*   the pointer of unpacked data of the font.
		 * it is NULL, when data is unloaded or flushed.
		 */
	CHAR_INFO *char_info[CHAR_ROOT];
	/*   the pointers of the first CHAR_INFO of the font.
		 * Also, it is NULL, when data is unloaded or flushed.
		 */
	FONT_INFO *k_top;
	FONT_INFO *k_next;

	F_SIZE_PARA size_para;

	F_SIZE_PARA des_para;
	/*  design size.
		 */
	int	 dpi;
	/*  requested dpi
		 */
	int	 sdpi;
	/*  substituted dpi
		 */
	char n[MAX_NAME_LEN];
	/* font name
		*/
	FONT_INFO *next_font;
	/*   equal NULL, when no next font.
		 */
	FONT_INFO *before_font;
	/*	 equal NULL, when no former font
		 */
	FONT_INFO *after_font;
	/*	 equal NULL, when no after font
		 */
	int k_width;
	/*   normal allowable maximal width of Kanji character by bits
		 *   counting k_scale
		 *   in PTeX TATE-KUMI fonts, it shows height of fonts.
		 */
	int k_height;
	/*   normal allowable maximal height of Kanji character by bits
		 *   counting k_scale
		 *   in PTeX TATE-KUMI fonts, it shows width of fonts.
		 */
	int last;
	/* last JIS CODE of Kanji font,
	   offset of character packet of Virtual Font
		 */
	long c;
	/* checksum (id)
		 */
	union EXT ext;

	int texpk_pos;

#ifdef	LBP
	int	lbp_font_inf;	/*0-3bit:フォント番号-80H, 
						  4,5:bold flag (0:normal, 1:1dot, 2:2dot)
						  6:ouline flag 8-15:フォント縦横比(0-200)*/
#endif
#ifdef	FDOWN
	uchar *d_flag;
#endif
	void *vjfm;		/* WINTT_FONT or VIRTUAL_FONT */

#ifndef NO_NTTRPL
	int code_offset;
	int pt_size;
	int family_code;
#endif
#ifdef	USE_SUBFONT
	int sfd_id;
#endif
};

#ifdef LBP
#ifdef FDOWN
typedef struct {	/*ダウンロード情報*/
	int assign_num;		/*割当番号*/
	int maxwidth;		/*フォント最大幅*/
	int maxheight;		/*フォント最大高*/
	int mincharcode;	/*最小コード*/
	int maxcharcode;	/*最大コード*/
	int numoffonts;		/*ダウンロードしたフォントの数*/
	long total_send;	/*ダウンロードしたトータルバイト数*/
	long total_off;		/*実サイズを使う場合のバイト数の差*/
	long total_mem;		/*セルサイズで計ったトータルバイト数*/
}
DL_INFO;

struct DJFONT_COUNT {	/*和文フォントの文字の出現頻度情報
						  : 8 byte -> should be even size*/
	unsigned short int code;		/*文字コード*/
	unsigned char font_code;		/*フォント番号*/
	unsigned char count;			/*出現回数-1*/
	unsigned short int after;		/*次のノード*/
	unsigned short int  before;		/*前のノード*/
};

struct DJFONT_TABLE {	/*和文フォントのダウンロードした文字情報: 5 byte*/
	unsigned short int code;		/*元の文字コード*/
	unsigned char font_code;		/*元のフォント番号*/
	unsigned char new_code;			/*ダウンロードで埋め込んだ文字コード*/
	unsigned char new_font_code;	/*ダウンロードで埋め込んだフォント番号*/
};

#define	DJFONT	struct DJFONT_COUNT
#define	DJTABLE	struct DJFONT_TABLE

struct REAL_SIZE {	/*文字の左右の空白を削った実ボックスサイズ、左上が原点*/
	int left;			/*最左端のx-座標*/
	int	top;			/*最上端のy-座標*/
	int	right;			/*最右端のx-座標 + 1*/
	int	bottom;			/*最下端のy-座標 + 1*/
};

# endif/*FDOWN*/

typedef struct {
	void (*init) (void);
	void (*init2) (int, int);
	void (*end) (void);
	void (*set_font) (FONT_INFO *, KFONT *, int, int, int);
	void (*move_cap) (PIXEL, PIXEL);
	void (*move_homepoint) (PIXEL, PIXEL);
	void (*pre_put_char) (FONT_INFO *, PREAMBLE*, uint *, PIXEL *, PIXEL *);
	void (*put_jis_font) (uint);
	void (*put_dl_font) (uint);
	long (*get_dlhsize) (int num);
	void (*pre_download) (FONT_INFO *, DL_INFO *, int, int);
	void (*download_rastor) (FONT_INFO *, DL_INFO *, uint, int);
	void (*post_download) (FONT_INFO *, DL_INFO *);
	void (*drawbox) (PIXEL, PIXEL, PIXEL, PIXEL, int);
	void (*rotate) (int);
	void (*tate_yoko) (int);
	struct LIPS_LFONT *data;
} LBP_FUNC;

struct LIPS_LFONT {
	char *font_name;
	int no;
};

#endif /*LBP*/

/* extern variables */

extern BUF_INFO buffers[];

extern	FONT_INFO *first_font_info;

 /*  the first position of the font-list.
	 */

#define	pk_buf_pointer	(&buffers[1])

 /*  the buffer for pk-font-data loaded from disk directly.
	 */

#define	raster_buf_pointer	(&buffers[0])

 /*  the buffer for the raster-data of unpacked fonts.
	 */

#define	bitmap_buf_pointer	(&buffers[2])

 /*  Virtual VRAM buffer. Interpret-module will draw the figure onto this.
	 * This is casted to HUGE_BUF when used.
	 */

 /*  Buffer for working in far heap: default size  = 0 
	 */

 /* Buffer for working in EMS
  */

#ifdef	GCC
#define	COMMON_SIZE	0x4000
#else
#define	COMMON_SIZE	2560
#endif
extern char common_work[COMMON_SIZE];
#define	tmp_buf	common_work

 /*  Buffer for working in near heap
	 COMMON_SIZE should not be smaller than 2048
	 */

extern BUFFER *get_work(uint);
extern FILE *fopenf(char*, char*);

#define	F_GOTH		1
#define	K_DBL		2
#define	F_LBP		4
#define	F_DWN		8
#define	F_FILL		0x10
#define	F_SHADOW	0x20
#define	F_OUTLINE	0x40
#define	F_GGOTH		0x80
#define	K_SCALE		0x100
#define	F_TATE		0x200
#define K_SCALE2    0x400
#define	F_TOPT	    0x800
#define	F_DTATE		0x1000
#define	F_DYOKO		0x2000

#define	F_ATTR	(F_GOTH|F_FILL|F_SHADOW|F_OUTLINE|F_GGOTH)

/* Definition for DVI commands */

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
#define FNT_DEF_4       246
#define PRE             247
#define POST            248
#define POST_POST       249
#define EOFNC           255	/* end of func */

/* Units for length : mm, inch(in), point(pt), scaled point(sp) and macros
 * for them.
 * by K.Yoshizawa Dec. 26, 1992
 */
#define ROUND(X) (int)((X)+0.5) /* 浮動小数点数→整数時の四捨五入 */

#define MM_PER_IN 25.4	/* 1in = 25.4mm */
#define PT_PER_IN 72.27	/* 1in = 72.27pt */
#define	BP_PER_IN 72.0	/* 1in = 72.0pt */

#define IN_PER_MM (1.0/MM_PER_IN)
#define PT_PER_MM (PT_PER_IN/MM_PER_IN)

#define	PT_PER_BP (PT_PER_IN/BP_PER_IN)

#define MM_PER_PT (MM_PER_IN/PT_PER_IN)
#define IN_PER_PT (1.0/PT_PER_IN)

#define MM_TO_SP(X) PT_TO_SP(MM_TO_PT(X))
#define IN_TO_SP(X) PT_TO_SP(IN_TO_PT(X))
#define	BP_TO_SP(X)	PT_TO_SP(BP_TO_PT(X))
#define PT_TO_SP(X) ((SCALED_PT)ROUND((double)(X)*65536.0))

#define IN_TO_MM(X) ((X)*MM_PER_IN)
#define PT_TO_MM(X) ((X)*MM_PER_PT)
#define SP_TO_MM(X) PT_TO_MM(SP_TO_PT(X))

#define MM_TO_IN(X) ((X)*IN_PER_MM)
#define PT_TO_IN(X) ((X)*IN_PER_PT)
#define SP_TO_IN(X) PT_TO_IN(SP_TO_PT(X))

#define MM_TO_PT(X) ((X)*PT_PER_MM)
#define IN_TO_PT(X) ((X)*PT_PER_IN)
#define	BP_TO_PT(X)	((X)*PT_PER_BP)
#define SP_TO_PT(X) ((double)(X)/65536.0)

/*
 * type of machine
 */
#define	M_PC98		0
#define	M_DOSV		1
#define	M_J31		2

#define	M_DBCS		1

#define	issjis1(x)	((x)>=0x81&&(x)<=0xfc&&((x)<=0x9f||(x)>=0xe0))

#endif // INC_DD_H
/* end of file : dd.h */
