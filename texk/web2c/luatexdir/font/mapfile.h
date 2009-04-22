/* mapfile.h

   Copyright 2009 Taco Hoekwater <taco@luatex.org>

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

/* $Id: mapfile.h 2331 2009-04-18 16:39:50Z hhenkel $ */

#ifndef MAPFILE_H
#  define MAPFILE_H

#  define F_INCLUDED          0x01
#  define F_SUBSETTED         0x02
#  define F_STDT1FONT         0x04
#  define F_SUBFONT           0x08
#  define F_TYPE1             0x10
#  define F_TRUETYPE          0x20
#  define F_OTF               0x40
#  define F_CIDKEYED          0x80

#  define set_included(fm)    ((fm)->type |= F_INCLUDED)
#  define set_subsetted(fm)   ((fm)->type |= F_SUBSETTED)
#  define set_std_t1font(fm)  ((fm)->type |= F_STDT1FONT)
#  define set_subfont(fm)     ((fm)->type |= F_SUBFONT)
#  define set_type1(fm)       ((fm)->type |= F_TYPE1)
#  define set_truetype(fm)    ((fm)->type |= F_TRUETYPE)
#  define set_opentype(fm)    ((fm)->type |= F_OTF)
#  define set_subfont(fm)     ((fm)->type |= F_SUBFONT)
#  define set_cidkeyed(fm)    ((fm)->type |= F_CIDKEYED)

#  define unset_included(fm)  ((fm)->type &= ~F_INCLUDED)
#  define unset_subsetted(fm) ((fm)->type &= ~F_SUBSETTED)
#  define unset_std_t1font(fm)((fm)->type &= ~F_STDT1FONT)
#  define unset_subfont(fm)   ((fm)->type &= ~F_SUBFONT)
#  define unset_type1(fm)     ((fm)->type &= ~F_TYPE1)
#  define unset_truetype(fm)  ((fm)->type &= ~F_TRUETYPE)
#  define unset_opentype(fm)  ((fm)->type &= ~F_OTF)
#  define unset_subfont(fm)   ((fm)->type &= ~F_SUBFONT)
#  define unset_cidkeyed(fm)  ((fm)->type &= ~F_CIDKEYED)

#  define is_included(fm)     (((fm)->type & F_INCLUDED) != 0)
#  define is_subsetted(fm)    (((fm)->type & F_SUBSETTED) != 0)
#  define is_std_t1font(fm)   (((fm)->type & F_STDT1FONT) != 0)
#  define is_subfont(fm)      (((fm)->type & F_SUBFONT) != 0)
#  define is_type1(fm)        (((fm)->type & F_TYPE1) != 0)
#  define is_truetype(fm)     (((fm)->type & F_TRUETYPE) != 0)
#  define is_opentype(fm)     (((fm)->type & F_OTF) != 0)
#  define is_subfont(fm)      (((fm)->type & F_SUBFONT) != 0)
#  define is_cidkeyed(fm)     (((fm)->type & F_CIDKEYED) != 0)

#  define fm_slant(fm)        (fm)->slant
#  define fm_extend(fm)       (fm)->extend
#  define fm_fontfile(fm)     (fm)->ff_name

#  define is_reencoded(fm)    ((fm)->encname != NULL)
#  define is_fontfile(fm)     (fm_fontfile(fm) != NULL)
#  define is_t1fontfile(fm)   (is_fontfile(fm) && is_type1(fm))
#  define is_builtin(fm)      (!is_fontfile(fm))

#  define LINK_TFM            0x01
#  define LINK_PS             0x02
#  define set_tfmlink(fm)     ((fm)->links |= LINK_TFM)
#  define set_pslink(fm)      ((fm)->links |= LINK_PS)
#  define unset_tfmlink(fm)   ((fm)->links &= ~LINK_TFM)
#  define unset_pslink(fm)    ((fm)->links &= ~LINK_PS)
#  define has_tfmlink(fm)     ((fm)->links & LINK_TFM)
#  define has_pslink(fm)      ((fm)->links & LINK_PS)

/**********************************************************************/

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

typedef struct {
    char *ff_name;              /* base name of font file */
    char *ff_path;              /* full path to font file */
} ff_entry;

/**********************************************************************/

fm_entry *lookup_fontmap(char *);
boolean hasfmentry(internalfontnumber);
void fm_free(void);
void fm_read_info(void);
ff_entry *check_ff_exist(char *, boolean);
void pdfmapfile(integer);
void pdfmapline(integer);
void pdf_init_map_file(string map_name);
fm_entry *new_fm_entry(void);
void delete_fm_entry(fm_entry *);
int avl_do_entry(fm_entry *, int);
int check_std_t1font(char *s);
int is_subsetable(fm_entry * fm);

#endif                          /* MAPFILE_H */
