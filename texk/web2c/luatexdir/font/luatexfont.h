/* luatexfont.h --- General font definitions

   Copyright 2008 Taco Hoekwater <taco@luatex.org>

   This file is part of LuaTeX.

   LuaTeX is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 2 of the License, or (at your
   option) any later version.

   LuaTeX is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
   License for more details.

   You should have received a copy of the GNU General Public License along
   with LuaTeX; if not, see <http://www.gnu.org/licenses/>. */

/* $Id: luatexfont.h 2332 2009-04-18 17:17:56Z hhenkel $ */

#ifndef LUATEXFONT_H
#  define LUATEXFONT_H

#  include "ptexlib.h"
#  ifndef pdfTeX
#    define pdfTeX
#    include "sfnt.h"           /* which wants that pdfTeX is defined */
#    undef pdfTeX
#  else
#    include "sfnt.h"
#  endif

/**********************************************************************/

#  define ASCENT_CODE       0
#  define CAPHEIGHT_CODE    1
#  define DESCENT_CODE      2
#  define ITALIC_ANGLE_CODE 3
#  define STEMV_CODE        4
#  define XHEIGHT_CODE      5
#  define FONTBBOX1_CODE    6
#  define FONTBBOX2_CODE    7
#  define FONTBBOX3_CODE    8
#  define FONTBBOX4_CODE    9
#  define FONTNAME_CODE     10
#  define GEN_KEY_NUM       (XHEIGHT_CODE + 1)
#  define MAX_KEY_CODE      (FONTBBOX1_CODE + 1)
#  define INT_KEYS_NUM      (FONTBBOX4_CODE + 1)
#  define FONT_KEYS_NUM     (FONTNAME_CODE + 1)

#  define FD_FLAGS_NOT_SET_IN_MAPLINE -1
#  define FD_FLAGS_DEFAULT_EMBED  4     /* a symbol font */
#  define FD_FLAGS_DEFAULT_NON_EMBED 0x22
                                        /* a nonsymbolic serif font */

/**********************************************************************/

typedef struct {
    const char *pdfname;
    const char *t1name;
    boolean valid;
} key_entry;

extern const key_entry font_key[FONT_KEYS_NUM];

typedef struct _subfont_entry {
    char *infix;                /* infix for this subfont, eg "01" */
    long charcodes[256];        /* the mapping for this subfont as read from sfd */
    struct _subfont_entry *next;
} subfont_entry;

#  include "mapfile.h"

typedef struct {
    char *name;                 /* sfd name, eg "Unicode" */
    subfont_entry *subfont;     /* linked list of subfonts */
} sfd_entry;

typedef struct {
    int val;                    /* value */
    boolean set;                /* true if parameter has been set */
} intparm;

typedef struct {
    integer fe_objnum;          /* object number */
    char *name;                 /* encoding file name */
    char **glyph_names;         /* array of glyph names */
    struct avl_table *tx_tree;  /* tree of encoding positions marked as used by TeX */
} fe_entry;

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

typedef struct fo_entry_ {
    integer fo_objnum;          /* object number of the font dictionary */
    internalfontnumber tex_font;        /* needed only for \pdffontattr{} */
    fm_entry *fm;               /* pointer to font map structure for this font dictionary */
    fd_entry *fd;               /* pointer to /FontDescriptor object structure */
    fe_entry *fe;               /* pointer to encoding structure */
    integer cw_objnum;          /* object number of the font program object */
    integer first_char;         /* first character used in this font */
    integer last_char;          /* last character used in this font */
    struct avl_table *tx_tree;  /* tree of non-reencoded TeX characters marked as used */
    integer tounicode_objnum;   /* object number of ToUnicode */
} fo_entry;

typedef struct {
    char *name;                 /* glyph name */
    long code;                  /* -1 = undefined; -2 = multiple codes, stored
                                   as string in unicode_seq; otherwise unicode value */
    char *unicode_seq;          /* multiple unicode sequence */
} glyph_unicode_entry;

typedef struct glw_entry_ {     /* subset glyphs for inclusion in CID-based fonts */
    unsigned int id;            /* glyph CID */
    signed int wd;              /* glyph width in 1/1000 em parts */
} glw_entry;

typedef struct {
    integer charcode, cwidth, cheight, xoff, yoff, xescape, rastersize;
    halfword *raster;
} chardesc;

/**********************************************************************/

#  include "texfont.h"

/* tounicode.c */
integer write_cid_tounicode(fo_entry * fo, internalfontnumber f);
void glyph_unicode_free(void);
void def_tounicode(str_number, str_number);
integer write_tounicode(char **, char *);

/* vfpacket.c */
void replace_packet_fonts(internal_font_number f, integer * old_fontid,
                          integer * new_fontid, int count);
integer *packet_local_fonts(internal_font_number f, integer * num);

/* writeccf.c */
void writetype1w(fd_entry * fd);

/* writetype0.c */
void writetype0(fd_entry * fd);

/* writefont.c */
void do_pdf_font(integer, internalfontnumber);
fd_entry *lookup_fd_entry(char *, integer, integer);
fd_entry *new_fd_entry(void);
void write_fontstuff(void);
integer get_fd_objnum(fd_entry * fd);
integer get_fn_objnum(fd_entry * fd);
void embed_whole_font(fd_entry * fd);
void register_fd_entry(fd_entry * fd);

/* writet1.c */
boolean t1_subset(char *, char *, unsigned char *);
char **load_enc_file(char *);
void writet1(fd_entry *);
void t1_free(void);
extern integer t1_length1, t1_length2, t1_length3;

/* writetype2.c */
void writetype2(fd_entry * fd);
extern unsigned long cidtogid_obj;
pdf_obj *pdf_new_stream(void);
void pdf_add_stream(pdf_obj * stream, unsigned char *buf, long len);
void pdf_release_obj(pdf_obj * stream);

/* subfont.c */
void sfd_free(void);
boolean handle_subfont_fm(fm_entry *, int);

/* writeenc.c */
fe_entry *get_fe_entry(char *);
void enc_free(void);
void write_fontencodings(void);

/* writettf.c */
void writettf(fd_entry *);
void writeotf(fd_entry *);
void ttf_free(void);
extern integer ttf_length;

/* pkin.c */
int readchar(boolean, chardesc *);

/* macnames.c */
extern char notdef[];

/* vfovf.c */
void vf_expand_local_fonts(internal_font_number f);
internal_font_number letter_space_font(halfword u, internal_font_number f,
                                       integer e);
internal_font_number auto_expand_font(internal_font_number f, integer e);
str_number expand_font_name(internal_font_number f, integer e);
void pdf_check_vf_cur_val(void);
internal_font_number copy_font_info(internal_font_number f);

/* writet3.c */
extern FILE *t3_file;
void writet3(int, internalfontnumber);
scaled get_pk_char_width(internalfontnumber, scaled);

extern unsigned char *t3_buffer;
extern integer t3_size;
extern integer t3_curbyte;

#  define t3_read_file() readbinfile(t3_file, &t3_buffer, &t3_size)
#  define t3_close()     xfclose(t3_file, cur_file_name)
#  define t3_getchar()   t3_buffer[t3_curbyte++]
#  define t3_eof()       (t3_curbyte>t3_size)

#  define t3_prefix(s)   (!strncmp(t3_line_array, s, strlen(s)))
#  define t3_putchar(c)  pdfout(c)

#endif                          /* LUATEXFONT_H */
