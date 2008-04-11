/*
Copyright (c) 2006 Taco Hoekwater <taco@elvenkind.com>

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

$Id$
*/

#ifndef MPOSTLIB
#  define MPOSTLIB

/* WEB2C macros and prototypes */
#  if !defined(MPCOERCE)
#    define EXTERN extern
#    include "mpd.h"
#  endif

/* pdftexlib macros */
#  include "mpmac.h"

#  include "avlstuff.h"


/* pdftexlib type declarations */
typedef struct {
    const char *pdfname;
    const char *t1name;
    float value;
    boolean valid;
} key_entry;

typedef struct {
    boolean loaded;             /* the encoding has been loaded? */
    char *name;                 /* encoding file name */
    char *encname;              /* encoding true name */
    integer objnum;             /* object number */
    char **glyph_names;
    integer tounicode;          /* object number of associated ToUnicode entry */
} enc_entry;

typedef struct {
    char *tfm_name;             /* TFM file name */
    char *ps_name;              /* PostScript name */
    integer flags;              /* font flags */
    char *ff_name;              /* font file name */
    char *subset_tag;           /* pseudoUniqueTag for subsetted font */
    enc_entry *encoding;        /* pointer to corresponding encoding */
    fontnumber tfm_num; /* number of the TFM refering this entry */
    unsigned short type;        /* font type (T1/TTF/...) */
    short slant;                /* SlantFont */
    short extend;               /* ExtendFont */
    integer ff_objnum;          /* FontFile object number */
    integer fn_objnum;          /* FontName/BaseName object number */
    integer fd_objnum;          /* FontDescriptor object number */
    char *charset;              /* string containing used glyphs */
    boolean all_glyphs;         /* embed all glyphs? */
    unsigned short links;       /* link flags from tfm_tree and ps_tree */
    short tfm_avail;            /* flags whether a tfm is available */
    short pid;                  /* Pid for truetype fonts */
    short eid;                  /* Eid for truetype fonts */
    void *subfont;              /* subfont mapping, not used */
} fm_entry;

typedef struct {
    char *ff_name;              /* base name of font file */
    char *ff_path;              /* full path to font file */
} ff_entry;

/* variable declarations */

extern boolean fontfile_found;
extern char **t1_glyph_names, *t1_builtin_glyph_names[];
extern char *cur_file_name;
extern const char notdef[];
extern integer t1_length1, t1_length2, t1_length3;
extern key_entry font_keys[];
extern strnumber last_tex_string;
extern size_t last_ptr_index;
extern char fontname_buf[];

/* function prototypes */

/* mapfile.c */
extern int avl_do_entry (fm_entry *, int);
extern ff_entry *check_ff_exist (fm_entry *);
extern void delete_fm_entry (fm_entry *);
extern void fm_free (void);
extern void fm_read_info (void);
extern boolean hasfmentry (fontnumber);
extern fm_entry *lookup_fontmap (char *);
extern void mpmapfile (integer);
extern void mpmapline (integer);
extern void mpinitmapfile (int);
extern fm_entry *new_fm_entry (void);
extern fontnumber tfmoffm (fmentryptr);

/* utils.c */
extern char *convertStringToPDFString (const char *in, int len);
extern void escapehex (poolpointer in);
extern void escapename (poolpointer in);
extern void escapestring (poolpointer in);
extern scaled extxnoverd (scaled, scaled, scaled);
extern void initversionstring (char **versions);
extern void make_subset_tag (fm_entry *, char **, int);
extern char *makecfilename (strnumber s);
extern char *makecstring (integer);
extern strnumber maketexstring (const char *);
extern void mp_printf (const char *, ...);
extern void pdf_printf (const char *, ...);
extern void pdf_puts (const char *);
extern void pdftex_fail (const char *, ...);
extern void pdftex_warn (const char *, ...);
extern void setjobid (int, int, int, int);
extern void stripspaces (char *p);
extern char *stripzeros (char *a);
extern void unescapehex (poolpointer in);
extern int xfflush (FILE *);
extern size_t xfwrite (void *, size_t size, size_t nmemb, FILE *);
extern int xgetc (FILE *);
extern int xputc (int, FILE *);

/* writeenc.c */
extern enc_entry *add_enc (char *);
extern void enc_free (void);
extern void read_enc (enc_entry *);
extern void write_enc (char **, enc_entry *, integer);

/* writefont.c */
extern boolean dopsfont (fontnumber);
extern strnumber fmencodingname (fontnumber);
extern integer fmfontextend (fontnumber);
extern strnumber fmfontname (fontnumber);
extern integer fmfontslant (fontnumber);
extern strnumber fmfontsubsetname (fontnumber);
extern boolean fontisincluded (fontnumber);
extern boolean fontissubsetted (fontnumber);
extern boolean fontisreencoded (fontnumber);
extern void mpfontencodings (int, int);
extern void mploadencodings (int);

/* writet1.c */
extern void load_enc (char *, char **, char **);
extern void t1_free (void);
extern boolean t1_subset (char *, char *, unsigned char *);
extern int t1_updatefm (int, fm_entry *);
extern void writet1 (int, fm_entry *);

#define bitfile psfile

/* avlstuff.c */
extern void avlputobj (integer, integer);
extern integer avlfindobj (integer, integer, integer);

/* turningnumber.c */

extern angle bezierslope(integer,integer,integer,integer,integer,integer,integer,integer,int);

/* loadpool.c */

extern int loadpoolstrings (integer);

#endif                          /* MPOSTLIB */
