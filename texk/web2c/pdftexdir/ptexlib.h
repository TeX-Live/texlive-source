/*
Copyright (c) 1996-2006 Han The Thanh, <thanh@pdftex.org>

This file is part of pdfTeX.

pdfTeX is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

pdfTeX is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with pdfTeX; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

$Id: //depot/Build/source.development/TeX/texk/web2c/pdftexdir/ptexlib.h#26 $
*/

#ifndef PDFTEXLIB
#  define PDFTEXLIB

/* WEB2C macros and prototypes */
#  if !defined(PDFTEXCOERCE) && !defined(PDFETEXCOERCE)
#    ifdef pdfTeX
#      undef pdfTeX             /* to avoid warning about redefining pdfTeX in pdftexd.h */
#    endif                      /* pdfTeX */
#    define EXTERN extern
#    include "pdftexd.h"
#  endif

/* pdftexlib macros */
#  include "ptexmac.h"

/* avl */
#  include "avlstuff.h"

#  include "openbsd-compat.h"

/* pdftexlib type declarations */
typedef struct {
    const char *pdfname;
    const char *t1name;
    float value;
    boolean valid;
} key_entry;

struct _subfont_entry;
typedef struct _subfont_entry subfont_entry;

struct _subfont_entry {
    char *infix;                /* infix for this subfont, eg "01" */
    long charcodes[256];        /* the mapping for this subfont as read from sfd */
    subfont_entry *next;
};

typedef struct {
    char *name;                 /* sfd name, eg "Unicode" */
    subfont_entry *subfont;     /* linked list of subfonts */
} sfd_entry;

typedef struct {
    integer fe_objnum;          /* object number */
    char *name;                 /* encoding file name */
    char **glyph_names;         /* array of glyph names */
    struct avl_table *tx_tree;  /* tree of encoding positions marked as used by TeX */
} fe_entry;

typedef struct {
    char *name;                 /* glyph name */
    long code;                  /* -1 = undefined; -2 = multiple codes, stored
                                   as string in unicode_seq; otherwise unicode value */
    char *unicode_seq;          /* multiple unicode sequence */
} glyph_unicode_entry;

typedef struct {
    /* parameters scanned from the map file: */
    char *tfm_name;             /* TFM file name (1st field in map line) */
    char *sfd_name;             /* subfont directory name, like @sfd_name@ */
    char *ps_name;              /* PostScript name (optional 2nd field in map line) */
    integer fd_flags;           /* font descriptor /Flags (PDF Ref. section 5.7.1) */
    integer slant;              /* SlantFont */
    integer extend;             /* ExtendFont */
    char *encname;              /* encoding file name */
    char *ff_name;              /* font file name */
    unsigned short type;        /* various flags */
    short pid;                  /* Pid for truetype fonts */
    short eid;                  /* Eid for truetype fonts */
    /* parameters NOT scanned from the map file: */
    subfont_entry *subfont;     /* subfont mapping */
    unsigned short links;       /* link flags from tfm_tree and ps_tree */
    boolean in_use;             /* true if this structure has been referenced already */
} fm_entry;

/**********************************************************************/

typedef struct {
    int val;                    /* value */
    boolean set;                /* true if parameter has been set */
} intparm;

typedef struct fd_entry_ {
    integer fd_objnum;          /* object number of the font descriptor object */
    char *fontname;             /* /FontName (without subset tag) */
    char *subset_tag;           /* 6-character subset tag */
    boolean ff_found;
    integer ff_objnum;          /* object number of the font program stream */
    integer fn_objnum;          /* font name object number (embedded PDF) */
    boolean all_glyphs;         /* embed all glyphs? */
    boolean write_ttf_glyph_names;
    intparm font_dim[FONT_KEYS_NUM];
    fe_entry *fe;               /* pointer to encoding structure */
    char **builtin_glyph_names; /* builtin encoding as read from the Type1 font file */
    fm_entry *fm;               /* pointer to font map structure */
    struct avl_table *tx_tree;  /* tree of non-reencoded TeX characters marked as used */
    struct avl_table *gl_tree;  /* tree of all marked glyphs */
} fd_entry;

typedef struct cw_entry_ {
    integer cw_objnum;          /* object number of the font program object */
    integer *width;             /* array [0...255] of character width values */
} cw_entry;

typedef struct fo_entry_ {
    integer fo_objnum;          /* object number of the font dictionary */
    internalfontnumber tex_font;        /* needed only for \pdffontattr{} */
    fm_entry *fm;               /* pointer to font map structure for this font dictionary */
    fd_entry *fd;               /* pointer to /FontDescriptor object structure */
    fe_entry *fe;               /* pointer to encoding structure */
    cw_entry *cw;               /* pointer to character /Widths array object structure */
    integer first_char;         /* first character used in this font */
    integer last_char;          /* last character used in this font */
    integer tounicode_objnum;   /* object number of ToUnicode */
} fo_entry;

/**********************************************************************/

typedef struct {
    char *ff_name;              /* base name of font file */
    char *ff_path;              /* full path to font file */
} ff_entry;

typedef short shalfword;
typedef struct {
    integer charcode, cwidth, cheight, xoff, yoff, xescape, rastersize;
    halfword *raster;
} chardesc;

/* pdftexlib variable declarations */
extern boolean true_dimen;
extern char **t1_glyph_names, *t1_builtin_glyph_names[];
extern char *cur_file_name;
extern const char notdef[];
extern integer t1_length1, t1_length2, t1_length3;
extern integer ttf_length;
extern strnumber last_tex_string;
extern size_t last_ptr_index;

/* pdftexlib function prototypes */

/* epdf.c */
extern integer get_fontfile_num(int);
extern integer get_fontname_num(int);
extern void epdf_free(void);

/* mapfile.c */
extern fm_entry *lookup_fontmap(char *);
extern boolean hasfmentry(internalfontnumber);
extern void fm_free(void);
extern void fm_read_info(void);
extern ff_entry *check_ff_exist(char *, boolean);
extern void pdfmapfile(integer);
extern void pdfmapline(integer);
extern void pdfinitmapfile(string map_name);
extern fm_entry *new_fm_entry(void);
extern void delete_fm_entry(fm_entry *);
extern int avl_do_entry(fm_entry *, int);

/* papersiz.c */
extern integer myatodim(char **);
extern integer myatol(char **);

/* pkin.c */
extern int readchar(boolean, chardesc *);

/* subfont.c */
extern void sfd_free(void);
extern boolean handle_subfont_fm(fm_entry *, int);

/* tounicode.c */
extern void glyph_unicode_free(void);
extern void deftounicode(strnumber, strnumber);
extern integer write_tounicode(char **, char *);

/* utils.c */
extern boolean str_eq_cstr(strnumber, char *);
extern char *makecstring(integer);
extern int xfflush(FILE *);
extern int xgetc(FILE *);
extern int xputc(int, FILE *);
extern scaled extxnoverd(scaled, scaled, scaled);
extern size_t xfwrite(void *, size_t size, size_t nmemb, FILE *);
extern strnumber getresnameprefix(void);
extern strnumber maketexstring(const char *);
extern integer fb_offset(void);
extern void fb_flush(void);
extern void fb_putchar(eightbits b);
extern void fb_seek(integer);
extern void libpdffinish(void);
extern char *makecfilename(strnumber s);
extern void make_subset_tag(fd_entry *);
__attribute__ ((format(printf, 1, 2)))
extern void pdf_printf(const char *, ...);
extern void pdf_puts(const char *);
__attribute__ ((noreturn, format(printf, 1, 2)))
extern void pdftex_fail(const char *, ...);
__attribute__ ((format(printf, 1, 2)))
extern void pdftex_warn(const char *, ...);
extern void setjobid(int, int, int, int);
__attribute__ ((format(printf, 1, 2)))
extern void tex_printf(const char *, ...);
extern void writestreamlength(integer, integer);
extern char *convertStringToPDFString(const char *in, int len);
extern void printID(strnumber);
extern void printcreationdate();
extern void printmoddate();
extern void escapename(poolpointer in);
extern void escapestring(poolpointer in);
extern void escapehex(poolpointer in);
extern void unescapehex(poolpointer in);
extern void getcreationdate();
extern void getfilemoddate(strnumber s);
extern void getfilesize(strnumber s);
extern void getmd5sum(strnumber s, boolean file);
extern void getfiledump(strnumber s, int offset, int length);
extern void matchstrings(strnumber s, strnumber t, int subcount, boolean icase);
extern void getmatch(int i);
extern void makepdftexbanner(void);
extern void initstarttime();
extern void removepdffile(void);
extern void garbagewarning(void);
extern void initversionstring(char **versions);
extern int newcolorstack(integer s, integer literal_mode, boolean pagestart);
extern int colorstackused();
extern integer colorstackset(int colstack_no, integer s);
extern integer colorstackpush(int colstack_no, integer s);
extern integer colorstackpop(int colstack_no);
extern integer colorstackcurrent(int colstack_no);
extern integer colorstackskippagestart(int colstack_no);
extern void checkpdfsave(int cur_h, int cur_v);
extern void checkpdfrestore(int cur_h, int cur_v);
extern void pdfshipoutbegin(boolean shipping_page);
extern void pdfshipoutend(boolean shipping_page);
extern void pdfsetmatrix(poolpointer in, scaled cur_h, scaled cur_v);
extern void matrixtransformpoint(scaled x, scaled y);
extern void matrixtransformrect(scaled llx, scaled lly, scaled urx, scaled ury);
extern boolean matrixused();
extern void matrixrecalculate(scaled urx);
extern scaled getllx();
extern scaled getlly();
extern scaled geturx();
extern scaled getury();
extern void allocvffnts();

/* vfpacket.c */
extern eightbits packetbyte(void);
extern integer newvfpacket(internalfontnumber);
extern void poppacketstate(void);
extern void pushpacketstate(void);
extern void startpacket(internalfontnumber, integer);
extern void storepacket(integer, integer, integer);
extern void vf_free(void);

/* writeenc.c */
extern fe_entry *get_fe_entry(char *);
extern void enc_free(void);
extern void write_fontencodings(void);

/* writefont.c */
extern void dopdffont(integer, internalfontnumber);
extern fd_entry *lookup_fd_entry(char *, integer, integer);
extern fd_entry *new_fd_entry(void);
extern void writefontstuff();

/* writeimg.c */
extern boolean checkimageb(integer);
extern boolean checkimagec(integer);
extern boolean checkimagei(integer);
extern boolean ispdfimage(integer);
extern integer epdforigx(integer);
extern integer epdforigy(integer);
extern integer imageheight(integer);
extern integer imagepages(integer);
extern integer imagewidth(integer);
extern integer imagexres(integer);
extern integer imageyres(integer);
extern integer readimage(strnumber, integer, strnumber, integer, integer,
                         integer, integer);
extern void deleteimage(integer);
extern void img_free(void);
extern void updateimageprocset(integer);
extern void writeimage(integer);
extern integer imagecolordepth(integer img);

/* writejbig2.c */
extern void flushjbig2page0objects();

/* writet1.c */
extern boolean t1_subset(char *, char *, unsigned char *);
extern char **load_enc_file(char *);
extern void writet1(fd_entry *);
extern void t1_free(void);

/* writet3.c */
extern void writet3(int, internalfontnumber);
extern scaled getpkcharwidth(internalfontnumber, scaled);

/* writettf.c */
extern void writettf(fd_entry *);
extern void writeotf(fd_entry *);
extern void ttf_free(void);

/* writezip.c */
extern void writezip(boolean);

/* avlstuff.c */
extern int comp_int_entry(const void *, const void *, void *);
extern int comp_string_entry(const void *, const void *, void *);
extern void avlputobj(integer, integer);
extern integer avlfindobj(integer, integer, integer);

/**********************************************************************/
static const key_entry font_key[FONT_KEYS_NUM] = {
    {"Ascent", "Ascender"}
    , {"CapHeight", "CapHeight"}
    , {"Descent", "Descender"}
    , {"ItalicAngle", "ItalicAngle"}
    , {"StemV", "StdVW"}
    , {"XHeight", "XHeight"}
    , {"FontBBox", "FontBBox"}
    , {"", "", 0}
    , {"", "", 0}
    , {"", "", 0}
    , {"FontName", "FontName"}
};

/**********************************************************************/
#endif                          /* PDFTEXLIB */
