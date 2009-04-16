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

/* $Id: mapfile.h 2271 2009-04-12 23:42:21Z oneiros $ */

#ifndef MAPFILE_H
#  define MAPFILE_H
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

/**********************************************************************/
#endif
