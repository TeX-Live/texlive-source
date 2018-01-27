/*  dvipos-20070107

    Copyright (C) 2003 by Jin-Hwan <chofchof@ktug.or.kr>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
*/

#ifndef _UTILS_H_
#define _UTILS_H_

#ifdef MIKTEX
#include "miktex.h"
#include "gnu-miktex.h"
#include "web2c-miktex.h"
#else
#include <kpathsea/kpathsea.h>
#endif

#include <stdio.h>

typedef int           UNSIGNED_BYTE, SIGNED_BYTE, SIGNED_PAIR;
typedef unsigned      UNSIGNED_PAIR;
typedef long          UNSIGNED_TRIPLE, SIGNED_TRIPLE, SIGNED_QUAD;
typedef unsigned long UNSIGNED_QUAD;

extern UNSIGNED_BYTE get_unsigned_byte (FILE *fp);
extern SIGNED_BYTE get_signed_byte (FILE *fp);
extern UNSIGNED_PAIR get_unsigned_pair (FILE *fp);
extern SIGNED_PAIR get_signed_pair (FILE *fp);
extern UNSIGNED_TRIPLE get_unsigned_triple (FILE *fp);
extern UNSIGNED_TRIPLE get_unsigned_triple_kanji (FILE *fp);
extern SIGNED_TRIPLE get_signed_triple (FILE *fp);
extern UNSIGNED_QUAD get_unsigned_quad (FILE *fp);
extern SIGNED_QUAD get_signed_quad (FILE *fp);

extern void put_unsigned_byte (SIGNED_QUAD quad, FILE *fp);
extern void put_signed_byte (SIGNED_QUAD quad, FILE *fp);
extern void put_unsigned_pair (SIGNED_QUAD quad, FILE *fp);
extern void put_signed_pair (SIGNED_QUAD quad, FILE *fp);
extern void put_unsigned_triple (SIGNED_QUAD quad, FILE *fp);
extern void put_signed_triple (SIGNED_QUAD quad, FILE *fp);
extern void put_signed_quad (SIGNED_QUAD quad, FILE *fp);

extern void sput_signed_pair (char *buf, SIGNED_QUAD quad);
extern void sput_signed_quad (char *buf, SIGNED_QUAD quad);

#ifndef KPATHSEA
#  ifdef concat
#  undef concat
extern char *concat (const char *s1, const char *s2);
#  endif
extern char *concat3 (const char *s1, const char *s2, const char *s3);
extern char *make_suffix (const char *s, const char *suffix);
#endif

#define M_FAIL    0x01
#define M_VERBOSE 0x02
#define M_WARNING 0x04
#define M_DEBUG   0x08
#define M_USAGE   0x10

extern void msg_out (int level, const char *fmt, ...);

/* Global Variables */
extern int verbose, do_smashchars;
extern FILE *infp, *outfp, *bbxfp, *frmfp;
extern char *infname, *bbxfname, *frmfname;
extern SIGNED_QUAD dbg_location, rule_width;

#define WEB_INFINITY 0x7FFFFFFFL //017777777777

#define VERSION "20070107"

#endif /* _UTILS_H_ */
