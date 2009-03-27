/* texfont.h Main font API implementation for the pascal parts
   
   Copyright 2006-2008 Taco Hoekwater <taco@luatex.org>

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

/* $Id: texfont.h 2057 2009-03-19 15:45:47Z taco $ */

/* Here we have the interface to LuaTeX's font system, as seen from the
   main pascal program. There is a companion list in luatex.defines to
   keep web2c happy */

/* this file is read at the end of ptexlib.h, which is called for at
   the end of luatexcoerce.h, as well as from the C sources 
*/

#ifndef TEXFONT_H
#  define TEXFONT_H 1

#  include "luatexdir/managed-sa.h"

#  define pointer halfword

typedef struct liginfo {
    integer adj;
    integer lig;
    char type;
} liginfo;

typedef struct kerninfo {
    integer adj;
    scaled sc;
} kerninfo;

typedef struct extinfo {
    struct extinfo *next;
    int glyph;
    int start_overlap;
    int end_overlap;
    int advance;
    int extender;
} extinfo;

typedef struct charinfo {
    char *name;                 /* postscript character name */
    liginfo *ligatures;         /* ligature items */
    kerninfo *kerns;            /* kern items */
    real_eight_bits *packets;   /* virtual commands.  */
    unsigned short index;       /* CID index */
    integer remainder;          /* spare value for odd items, could be union-ed with extensible */
    scaled width;               /* width */
    scaled height;              /* height */
    scaled depth;               /* depth */
    scaled italic;              /* italic correction */
    scaled top_accent;          /* top accent alignment */
    scaled bot_accent;          /* bot accent alignment */
    integer ef;                 /* font expansion factor */
    integer lp;                 /* left protruding factor */
    integer rp;                 /* right protruding factor */
    char tag;                   /* list / ext taginfo */
    char used;                  /* char is typeset ? */
    char *tounicode;            /* unicode equivalent */
    extinfo *hor_variants;      /* horizontal variants */
    extinfo *vert_variants;     /* vertical variants */
} charinfo;


/* this is for speed reasons, it is called from the web source
   when more than two of the traditional four char values are needed 
   at the same time. the structure |charinfo_short| is defined in 
   web */

#  ifndef luaTeX
typedef struct charinfo_short {
    scaled ci_wd;               /* width */
    scaled ci_ht;               /* height */
    scaled ci_dp;               /* depth */
} charinfo_short;
#  endif

#  define EXT_NORMAL 0
#  define EXT_REPEAT 1

extern extinfo *get_charinfo_vert_variants(charinfo * ci);
extern extinfo *get_charinfo_hor_variants(charinfo * ci);
extern void set_charinfo_hor_variants(charinfo * ci, extinfo * ext);
extern void set_charinfo_vert_variants(charinfo * ci, extinfo * ext);
extern void add_charinfo_vert_variant(charinfo * ci, extinfo * ext);
extern void add_charinfo_hor_variant(charinfo * ci, extinfo * ext);

extern extinfo *copy_variants(extinfo * o);

extern extinfo *new_variant(int glyph, int startconnect, int endconnect,
                            int advance, int repeater);

extern charinfo_short char_info_short(internal_font_number f, integer c);



typedef struct texfont {
    integer _font_size;
    integer _font_dsize;
    char *_font_name;
    char *_font_area;
    char *_font_filename;
    char *_font_fullname;
    char *_font_encodingname;
    char *_font_cidregistry;
    char *_font_cidordering;
    integer _font_cidversion;
    integer _font_cidsupplement;

    integer _font_ec;
    unsigned _font_checksum;    /* internal information */
    char _font_used;            /* internal information */
    char _font_touched;         /* internal information */
    integer _font_cache_id;     /* internal information */
    char _font_encodingbytes;   /* 1 or 2 bytes */
    integer _font_slant;        /* a slant in ppt */
    integer _font_extend;       /* an extension in ppt, or 0 */
    char _font_tounicode;       /* 1 if info is present */
    fm_entry_ptr _font_map;
    integer _font_type;
    integer _font_format;
    integer _font_embedding;
    integer _font_bc;
    integer _hyphen_char;
    integer _skew_char;
    integer _font_natural_dir;

    charinfo *_left_boundary;
    charinfo *_right_boundary;

    integer _font_params;
    scaled *_param_base;

    integer _font_math_params;
    scaled *_math_param_base;

    sa_tree characters;
    integer charinfo_count;
    integer charinfo_size;
    charinfo *charinfo;
    int *charinfo_cache;
    integer ligatures_disabled;

    integer _pdf_font_num;      /* maps to a PDF resource ID */
    scaled _pdf_font_size;      /* maps to a PDF font size */
    internal_font_number _pdf_font_blink;       /* link to  base font for expanded fonts */
    internal_font_number _pdf_font_elink;       /* link to expanded fonts for base font */
    integer _pdf_font_expand_ratio;     /* expansion ratio of a particular font */
    internal_font_number _pdf_font_shrink;      /* font at limit of shrinking */
    internal_font_number _pdf_font_stretch;     /* font at limit of stretching */
    integer _pdf_font_step;     /* amount of one step of expansion */
    boolean _pdf_font_auto_expand;      /* this font is auto-expanded? */
    str_number _pdf_font_attr;  /* pointer to additional attributes */
} texfont;

typedef enum {
    unknown_font_type = 0,      /* new font (has not been used yet) */
    virtual_font_type,          /* virtual font */
    real_font_type,             /* real font */
} font_types;

typedef enum {
    unknown_format = 0,
    type1_format,
    type3_format,
    truetype_format,
    opentype_format,
} font_formats;

typedef enum {
    unknown_embedding = 0,
    no_embedding,
    subset_embedding,
    full_embedding,
} font_embedding_option;

extern char *font_type_strings[];
extern char *font_format_strings[];
extern char *font_embedding_strings[];


#  define font_checksum(a)          font_tables[a]->_font_checksum
#  define set_font_checksum(a,b)    font_checksum(a) = b

#  define font_check_0(a)           ((font_tables[a]->_font_checksum&0xFF000000)>>24)
#  define font_check_1(a)           ((font_tables[a]->_font_checksum&0x00FF0000)>>16)
#  define font_check_2(a)           ((font_tables[a]->_font_checksum&0x0000FF00)>>8)
#  define font_check_3(a)            (font_tables[a]->_font_checksum&0x000000FF)

#  define font_size(a)              font_tables[a]->_font_size
#  define set_font_size(a,b)        font_size(a) = b
#  define font_dsize(a)             font_tables[a]->_font_dsize
#  define set_font_dsize(a,b)       font_dsize(a) = b

#  define font_name(a)              font_tables[a]->_font_name
#  define get_font_name(a)          (unsigned char *)font_name(a)
#  define set_font_name(f,b)        font_name(f) = b
#  define tex_font_name(a)          maketexstring(font_name(a))

boolean cmp_font_name(integer, strnumber);

#  define font_area(a)              font_tables[a]->_font_area
#  define get_font_area(a)          (unsigned char *)font_area(a)
#  define set_font_area(f,b)        font_area(f) = b
#  define tex_font_area(a)          maketexstring(font_area(a))

boolean cmp_font_area(integer, strnumber);

#  define font_reassign(a,b)            { if (a!=NULL) free(a); a = b; }

#  define font_filename(a)            font_tables[a]->_font_filename
#  define set_font_filename(f,b)      font_reassign(font_filename(f),b)

#  define font_fullname(a)            font_tables[a]->_font_fullname
#  define set_font_fullname(f,b)      font_reassign(font_fullname(f),b)

#  define font_encodingname(a)        font_tables[a]->_font_encodingname
#  define set_font_encodingname(f,b)  font_reassign(font_encodingname(f),b)

boolean font_shareable(internal_font_number, internal_font_number);

#  define cmp_font_filename(a,b)      (!(font_filename(a)!=NULL || font_filename(b)!=NULL || \
                                       strcmp(font_filename(a),font_filename(b))))
#  define cmp_font_fullname(a,b)      (!(font_fullname(a)!=NULL || font_fullname(b)!=NULL || \
                                       strcmp(font_fullname(a),font_fullname(b))))
#  define cmp_font_encodingname(a,b)  (!(font_encoding(a)!=NULL || font_encodingname(b)!=NULL || \
                                       strcmp(font_encodingname(a),font_encodingname(b))))

#  define font_bc(a)                  font_tables[a]->_font_bc
#  define set_font_bc(f,b)            font_bc(f) = b

#  define font_ec(a)                  font_tables[a]->_font_ec
#  define set_font_ec(f,b)            font_ec(f) = b

#  define font_used(a)                (font_tables[a]!=NULL && font_tables[a]->_font_used)
#  define set_font_used(a,b)          font_tables[a]->_font_used = b

#  define font_touched(a)             font_tables[a]->_font_touched
#  define set_font_touched(a,b)       font_touched(a) = b

#  define font_type(a)                font_tables[a]->_font_type
#  define set_font_type(a,b)          {  /* fprintf(stdout,"set font type of %s to %i: %s\n",font_name(a),b,__FILE__); */   \
                                       font_type(a) = b; }

#  define font_format(a)              font_tables[a]->_font_format
#  define font_format_name(a)         font_format_strings[font_tables[a]->_font_format]
#  define set_font_format(a,b)        font_format(a) = b

#  define font_embedding(a)           font_tables[a]->_font_embedding
#  define set_font_embedding(a,b)     font_embedding(a) = b

#  define font_cidversion(a)          font_tables[a]->_font_cidversion
#  define set_font_cidversion(a,b)    font_cidversion(a) = b

#  define font_cidsupplement(a)       font_tables[a]->_font_cidsupplement
#  define set_font_cidsupplement(a,b) font_cidsupplement(a) = b

#  define font_cidordering(a)         font_tables[a]->_font_cidordering
#  define set_font_cidordering(f,b)   font_reassign(font_cidordering(f),b)

#  define font_cidregistry(a)         font_tables[a]->_font_cidregistry
#  define set_font_cidregistry(f,b)   font_reassign(font_cidregistry(f),b)

#  define font_map(a)                 font_tables[a]->_font_map
#  define set_font_map(a,b)           font_map(a) = b

#  define font_cache_id(a)            font_tables[a]->_font_cache_id
#  define set_font_cache_id(a,b)      font_cache_id(a) = b

#  define font_encodingbytes(a)       font_tables[a]->_font_encodingbytes
#  define set_font_encodingbytes(a,b) font_encodingbytes(a) = b

#  define font_slant(a)               font_tables[a]->_font_slant
#  define set_font_slant(a,b)         font_slant(a) = b

#  define font_extend(a)              font_tables[a]->_font_extend
#  define set_font_extend(a,b)        font_extend(a) = b

#  define font_tounicode(a)           font_tables[a]->_font_tounicode
#  define set_font_tounicode(a,b)     font_tounicode(a) = b

#  define hyphen_char(a)              font_tables[a]->_hyphen_char
#  define set_hyphen_char(a,b)        hyphen_char(a) = b

#  define skew_char(a)                font_tables[a]->_skew_char
#  define set_skew_char(a,b)          skew_char(a) = b

#  define font_natural_dir(a)         font_tables[a]->_font_natural_dir
#  define set_font_natural_dir(a,b)   font_natural_dir(a) = b

#  define pdf_font_size(a)            font_tables[a]->_pdf_font_size
#  define set_pdf_font_size(a,b)      pdf_font_size(a) = b

#  define pdf_font_num(a)             font_tables[a]->_pdf_font_num
#  define set_pdf_font_num(a,b)       pdf_font_num(a) = b

#  define pdf_font_blink(a)            font_tables[a]->_pdf_font_blink
#  define set_pdf_font_blink(a,b)      pdf_font_blink(a) = b

#  define pdf_font_elink(a)            font_tables[a]->_pdf_font_elink
#  define set_pdf_font_elink(a,b)      pdf_font_elink(a) = b

#  define pdf_font_expand_ratio(a)            font_tables[a]->_pdf_font_expand_ratio
#  define set_pdf_font_expand_ratio(a,b)      pdf_font_expand_ratio(a) = b

#  define pdf_font_shrink(a)            font_tables[a]->_pdf_font_shrink
#  define set_pdf_font_shrink(a,b)      pdf_font_shrink(a) = b

#  define pdf_font_stretch(a)            font_tables[a]->_pdf_font_stretch
#  define set_pdf_font_stretch(a,b)      pdf_font_stretch(a) = b

#  define pdf_font_step(a)            font_tables[a]->_pdf_font_step
#  define set_pdf_font_step(a,b)      pdf_font_step(a) = b

#  define pdf_font_auto_expand(a)            font_tables[a]->_pdf_font_auto_expand
#  define set_pdf_font_auto_expand(a,b)      pdf_font_auto_expand(a) = b

#  define pdf_font_attr(a)            font_tables[a]->_pdf_font_attr
#  define set_pdf_font_attr(a,b)      pdf_font_attr(a) = b


#  define left_boundarychar  -1
#  define right_boundarychar -2
#  define non_boundarychar -3

#  define left_boundary(a)              font_tables[a]->_left_boundary
#  define has_left_boundary(a)          (left_boundary(a)!=NULL)
#  define set_left_boundary(a,b)        font_reassign(left_boundary(a),b)

#  define right_boundary(a)             font_tables[a]->_right_boundary
#  define has_right_boundary(a)         (right_boundary(a)!=NULL)
#  define set_right_boundary(a,b)       font_reassign(right_boundary(a),b)

#  define font_bchar(a)       (right_boundary(a)!=NULL ? right_boundarychar : non_boundarychar)

/* font parameters */

#  define font_params(a)       font_tables[a]->_font_params
#  define param_base(a)        font_tables[a]->_param_base
#  define font_param(a,b)      font_tables[a]->_param_base[b]

extern void set_font_params(internal_font_number f, int b);

#  define set_font_param(f,n,b)                                 \
  { if (font_params(f)<n) set_font_params(f,n);                 \
    font_param(f,n) = b; }


#  define font_math_params(a)       font_tables[a]->_font_math_params
#  define math_param_base(a)        font_tables[a]->_math_param_base
#  define font_math_param(a,b)      font_tables[a]->_math_param_base[b]

extern void set_font_math_params(internal_font_number f, int b);

#  define set_font_math_param(f,n,b)                                   \
  { if (font_math_params(f)<n) set_font_math_params(f,n);              \
    font_math_param(f,n) = b; }

/* Font parameters are sometimes referred to as |slant(f)|, |space(f)|, etc.*/

typedef enum {
    slant_code = 1,
    space_code = 2,
    space_stretch_code = 3,
    space_shrink_code = 4,
    x_height_code = 5,
    quad_code = 6,
    extra_space_code = 7
} font_parameter_codes;

#  define slant(f)         font_param(f,slant_code)
#  define space(f)         font_param(f,space_code)
#  define space_stretch(f) font_param(f,space_stretch_code)
#  define space_shrink(f)  font_param(f,space_shrink_code)
#  define x_height(f)      font_param(f,x_height_code)
#  define quad(f)          font_param(f,quad_code)
#  define extra_space(f)   font_param(f,extra_space_code)

/* now for characters  */

extern charinfo *get_charinfo(internal_font_number f, integer c);
extern integer char_exists(internal_font_number f, integer c);
extern charinfo *char_info(internal_font_number f, integer c);


extern void set_charinfo_width(charinfo * ci, scaled val);
extern void set_charinfo_height(charinfo * ci, scaled val);
extern void set_charinfo_depth(charinfo * ci, scaled val);
extern void set_charinfo_italic(charinfo * ci, scaled val);
extern void set_charinfo_top_accent(charinfo * ci, scaled val);
extern void set_charinfo_bot_accent(charinfo * ci, scaled val);
extern void set_charinfo_tag(charinfo * ci, scaled val);
extern void set_charinfo_remainder(charinfo * ci, scaled val);
extern void set_charinfo_used(charinfo * ci, scaled val);
extern void set_charinfo_index(charinfo * ci, scaled val);
extern void set_charinfo_name(charinfo * ci, char *val);
extern void set_charinfo_tounicode(charinfo * ci, char *val);
extern void set_charinfo_ligatures(charinfo * ci, liginfo * val);
extern void set_charinfo_kerns(charinfo * ci, kerninfo * val);
extern void set_charinfo_packets(charinfo * ci, real_eight_bits * val);
extern void set_charinfo_extensible(charinfo * ci, int a, int b, int c, int d);
extern void set_charinfo_ef(charinfo * ci, scaled val);
extern void set_charinfo_lp(charinfo * ci, scaled val);
extern void set_charinfo_rp(charinfo * ci, scaled val);

#  define set_char_used(f,a,b)  do {                            \
        if (char_exists(f,a))                                   \
            set_charinfo_used(char_info(f,a),b);                \
    } while (0)

extern scaled get_charinfo_width(charinfo * ci);
extern scaled get_charinfo_height(charinfo * ci);
extern scaled get_charinfo_depth(charinfo * ci);
extern scaled get_charinfo_italic(charinfo * ci);
extern scaled get_charinfo_top_accent(charinfo * ci);
extern scaled get_charinfo_bot_accent(charinfo * ci);
extern char get_charinfo_tag(charinfo * ci);
extern integer get_charinfo_remainder(charinfo * ci);
extern char get_charinfo_used(charinfo * ci);
extern integer get_charinfo_index(charinfo * ci);
extern char *get_charinfo_name(charinfo * ci);
extern char *get_charinfo_tounicode(charinfo * ci);
extern liginfo *get_charinfo_ligatures(charinfo * ci);
extern kerninfo *get_charinfo_kerns(charinfo * ci);
extern real_eight_bits *get_charinfo_packets(charinfo * ci);
extern integer get_charinfo_ef(charinfo * ci);
extern integer get_charinfo_rp(charinfo * ci);
extern integer get_charinfo_lp(charinfo * ci);
extern integer get_charinfo_extensible(charinfo * ci, int which);

extern integer ext_top(internal_font_number f, integer c);
extern integer ext_bot(internal_font_number f, integer c);
extern integer ext_rep(internal_font_number f, integer c);
extern integer ext_mid(internal_font_number f, integer c);

#  define set_ligature_item(f,b,c,d)  { f.type = b; f.adj = c;  f.lig = d; }

#  define set_kern_item(f,b,c)      { f.adj = b;  f.sc = c; }


/* character information */

#  define non_char 65536        /* a code that can't match a real character */
#  define non_address 0         /* a spurious |bchar_label| */


/* character kerns and ligatures */

#  define end_kern               0x7FFFFF
                                        /* otherchar value meaning "stop" */
#  define ignored_kern           0x800000
                                        /* otherchar value meaning "disabled" */

#  define charinfo_kern(b,c)        b->kerns[c]

#  define kern_char(b)          (b).adj
#  define kern_kern(b)          (b).sc
#  define kern_end(b)          ((b).adj == end_kern)
#  define kern_disabled(b)     ((b).adj > end_kern)

/* character ligatures */

#  define end_ligature          0x7FFFFF        /* otherchar value meaning "stop" */
#  define ignored_ligature      0x800000        /* otherchar value meaning "disabled" */

#  define charinfo_ligature(b,c)     b->ligatures[c]

#  define is_valid_ligature(a)   ((a).type!=0)
#  define lig_type(a)            ((a).type>>1)
#  define lig_char(a)            (a).adj
#  define lig_replacement(a)     (a).lig
#  define lig_end(a)             (lig_char(a) == end_ligature)
#  define lig_disabled(a)        (lig_char(a) > end_ligature)

#  define no_tag 0              /* vanilla character */
#  define lig_tag 1             /* character has a ligature/kerning program */
#  define list_tag 2            /* character has a successor in a charlist */
#  define ext_tag 3             /* character is extensible */

extern scaled char_height(internal_font_number f, integer c);
extern scaled char_width(internal_font_number f, integer c);
extern scaled char_depth(internal_font_number f, integer c);
extern scaled char_italic(internal_font_number f, integer c);
extern scaled char_top_accent(internal_font_number f, integer c);
extern scaled char_bot_accent(internal_font_number f, integer c);

extern liginfo *char_ligatures(internal_font_number f, integer c);
extern kerninfo *char_kerns(internal_font_number f, integer c);
extern real_eight_bits *char_packets(internal_font_number f, integer c);

#  define has_lig(f,b)          (char_exists(f,b) &&( char_ligatures(f,b) != NULL))
#  define has_kern(f,b)         (char_exists(f,b) && (char_kerns(f,b) != NULL))
#  define has_packet(f,b)       (char_exists(f,b) && (char_packets(f,b) != NULL))

extern integer char_remainder(internal_font_number f, integer c);
extern char char_tag(internal_font_number f, integer c);
extern char char_used(internal_font_number f, integer c);
extern char *char_name(internal_font_number f, integer c);
extern integer char_index(internal_font_number f, integer c);

scaled raw_get_kern(internalfontnumber f, integer lc, integer rc);
scaled get_kern(internalfontnumber f, integer lc, integer rc);
liginfo get_ligature(internalfontnumber f, integer lc, integer rc);

#  define EXT_TOP 0
#  define EXT_BOT 1
#  define EXT_MID 2
#  define EXT_REP 3

extern texfont **font_tables;

integer new_font(void);
integer copy_font(integer id);
integer scale_font(integer id, integer atsize);
integer max_font_id(void);
void set_max_font_id(integer id);
integer new_font_id(void);
void create_null_font(void);
void delete_font(integer id);
boolean is_valid_font(integer id);

void dump_font(int font_number);
void undump_font(int font_number);

integer test_no_ligatures(internal_font_number f);
void set_no_ligatures(internal_font_number f);

extern integer get_tag_code(internal_font_number f, integer c);
extern integer get_lp_code(internal_font_number f, integer c);
extern integer get_rp_code(internal_font_number f, integer c);
extern integer get_ef_code(internal_font_number f, integer c);

extern void set_tag_code(internal_font_number f, integer c, integer i);
extern void set_lp_code(internal_font_number f, integer c, integer i);
extern void set_rp_code(internal_font_number f, integer c, integer i);
extern void set_ef_code(internal_font_number f, integer c, integer i);

int read_tfm_info(internal_font_number f, char *nom, char *aire, scaled s);


/* from dofont.c */

extern int read_font_info(pointer u, strnumber nom, scaled s, integer ndir);
extern int find_font_id(char *nom, char *aire, scaled s);

/* for and from vfpacket.c */

#  define scan_special 3 /* look into special text */   /* this is a hack */

typedef enum { packet_char_code,
    packet_font_code,
    packet_pop_code,
    packet_push_code,
    packet_special_code,
    packet_image_code,
    packet_right_code,
    packet_down_code,
    packet_rule_code,
    packet_node_code,
    packet_nop_code,
    packet_end_code
} packet_command_codes;

extern scaled sqxfw(scaled sq, integer fw);

extern void do_vf_packet(internal_font_number vf_f, integer c);
extern int vf_packet_bytes(charinfo * co);

#endif
