/* texfont.c Main font API implementation for the pascal parts
   
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

/* Main font API implementation for the pascal parts */

/* stuff to watch out for: 
 *
 * - Knuth had a 'null_character' that was used when a character could
 * not be found by the fetch() routine, to signal an error. This has
 * been deleted, but it may mean that the output of luatex is
 * incompatible with TeX after fetch() has detected an error condition.
 *
 * - Knuth also had a font_glue() optimization. I've removed that
 * because it was a bit of dirty programming and it also was
 * problematic if 0 != null.
 */

#include "ptexlib.h"
#include "luatex-api.h"

static const char _svn_version[] =
    "$Id: texfont.c 2064 2009-03-20 13:13:14Z taco $ $URL: http://scm.foundry.supelec.fr/svn/luatex/trunk/src/texk/web2c/luatexdir/font/texfont.c $";

#define proper_char_index(c) (c<=font_ec(f) && c>=font_bc(f))
#define dxfree(a,b) { xfree(a); a = b ; }
#define do_realloc(a,b,d)    a = xrealloc(a,(b)*sizeof(d))

texfont **font_tables = NULL;

static integer font_arr_max = 0;
static integer font_id_maxval = 0;

extern extinfo *get_charinfo_vert_variants(charinfo * ci);
extern extinfo *get_charinfo_hor_variants(charinfo * ci);
extern void set_charinfo_hor_variants(charinfo * ci, extinfo * ext);
extern void set_charinfo_vert_variants(charinfo * ci, extinfo * ext);

extern extinfo *copy_variants(extinfo * o);


static void grow_font_table(integer id)
{
    int j;
    if (id >= font_arr_max) {
        font_bytes += (font_arr_max - id + 8) * sizeof(texfont *);
        font_tables = xrealloc(font_tables, (id + 8) * sizeof(texfont *));
        j = 8;
        while (j--) {
            font_tables[id + j] = NULL;
        }
        font_arr_max = id + 8;
    }
}

integer new_font_id(void)
{
    int i;
    for (i = 0; i < font_arr_max; i++) {
        if (font_tables[i] == NULL) {
            break;
        }
    }
    if (i >= font_arr_max)
        grow_font_table(i);
    if (i > font_id_maxval)
        font_id_maxval = i;
    return i;
}

integer max_font_id(void)
{
    return font_id_maxval;
}

void set_max_font_id(integer i)
{
    font_id_maxval = i;
}

integer new_font(void)
{
    int k;
    int id;
    charinfo *ci;
    id = new_font_id();
    font_bytes += sizeof(texfont);
    /* most stuff is zero */
    font_tables[id] = xcalloc(1, sizeof(texfont));
    font_tables[id]->_font_name = NULL;
    font_tables[id]->_font_area = NULL;
    font_tables[id]->_font_filename = NULL;
    font_tables[id]->_font_fullname = NULL;
    font_tables[id]->_font_encodingname = NULL;
    font_tables[id]->_font_cidregistry = NULL;
    font_tables[id]->_font_cidordering = NULL;
    font_tables[id]->_left_boundary = NULL;
    font_tables[id]->_right_boundary = NULL;
    font_tables[id]->_param_base = NULL;
    font_tables[id]->_math_param_base = NULL;

    set_font_bc(id, 1);         /* ec = 0 */
    set_hyphen_char(id, '-');
    set_skew_char(id, -1);

    /* allocate eight values including 0 */
    set_font_params(id, 7);
    for (k = 0; k <= 7; k++) {
        set_font_param(id, k, 0);
    }
    /* character info zero is reserved for notdef */
    font_tables[id]->characters = new_sa_tree(1, 0);    /* stack size 1, default item value 0 */

    ci = xcalloc(1, sizeof(charinfo));
    set_charinfo_name(ci, xstrdup(".notdef"));
    font_tables[id]->charinfo = ci;
    font_tables[id]->charinfo_cache = NULL;

    return id;
}

#define Charinfo_count(a) font_tables[a]->charinfo_count
#define Charinfo_size(a) font_tables[a]->charinfo_size
#define Characters(a) font_tables[a]->characters

#define find_charinfo_id(f,c) get_sa_item(font_tables[f]->characters,c)

charinfo *get_charinfo(internal_font_number f, integer c)
{
    sa_tree_item glyph;
    charinfo *ci;
    if (proper_char_index(c)) {
        glyph = get_sa_item(Characters(f), c);
        if (!glyph) {
            /* this could be optimized using controlled growth */
            font_bytes += sizeof(charinfo);
            glyph = ++font_tables[f]->charinfo_count;
            do_realloc(font_tables[f]->charinfo, (glyph + 1), charinfo);
            memset(&(font_tables[f]->charinfo[glyph]), 0, sizeof(charinfo));
            font_tables[f]->charinfo[glyph].ef = 1000;  /* init */
            font_tables[f]->charinfo_size = glyph;
            set_sa_item(font_tables[f]->characters, c, glyph, 1);       /* 1= global */
        }
        return &(font_tables[f]->charinfo[glyph]);
    } else if (c == left_boundarychar) {
        if (left_boundary(f) == NULL) {
            ci = xcalloc(1, sizeof(charinfo));
            font_bytes += sizeof(charinfo);
            set_left_boundary(f, ci);
        }
        return left_boundary(f);
    } else if (c == right_boundarychar) {
        if (right_boundary(f) == NULL) {
            ci = xcalloc(1, sizeof(charinfo));
            font_bytes += sizeof(charinfo);
            set_right_boundary(f, ci);
        }
        return right_boundary(f);
    }
    return &(font_tables[f]->charinfo[0]);
}

void set_charinfo(internal_font_number f, integer c, charinfo * ci)
{
    sa_tree_item glyph;
    if (proper_char_index(c)) {
        glyph = get_sa_item(Characters(f), c);
        if (glyph) {
            font_tables[f]->charinfo[glyph] = *ci;
        } else {
            pdftex_fail("font: %s", "character insertion failed");
        }
    } else if (c == left_boundarychar) {
        set_left_boundary(f, ci);
    } else if (c == right_boundarychar) {
        set_right_boundary(f, ci);
    }
}



charinfo *copy_charinfo(charinfo * ci)
{
    int x;
    kerninfo *kern;
    liginfo *lig;
    real_eight_bits *packet;
    charinfo *co = NULL;
    if (ci == NULL)
        return NULL;
    co = xmalloc(sizeof(charinfo));
    memcpy(co, ci, sizeof(charinfo));
    set_charinfo_used(co, false);
    co->name = NULL;
    co->tounicode = NULL;
    co->packets = NULL;
    co->ligatures = NULL;
    co->kerns = NULL;
    co->vert_variants = NULL;
    co->hor_variants = NULL;
    if (ci->name != NULL) {
        co->name = xstrdup(ci->name);
    }
    if (ci->tounicode != NULL) {
        co->tounicode = xstrdup(ci->tounicode);
    }
    /* kerns */
    if ((kern = get_charinfo_kerns(ci)) != NULL) {
        x = 0;
        while (!kern_end(kern[x])) {
            x++;
        }
        x++;
        co->kerns = xmalloc(x * sizeof(kerninfo));
        memcpy(co->kerns, ci->kerns, (x * sizeof(kerninfo)));
    }
    /* ligs */
    if ((lig = get_charinfo_ligatures(ci)) != NULL) {
        x = 0;
        while (!lig_end(lig[x])) {
            x++;
        }
        x++;
        co->ligatures = xmalloc(x * sizeof(liginfo));
        memcpy(co->ligatures, ci->ligatures, (x * sizeof(liginfo)));
    }
    /* packets */
    if ((packet = get_charinfo_packets(ci)) != NULL) {
        x = vf_packet_bytes(ci);
        co->packets = xmalloc(x);
        memcpy(co->packets, ci->packets, x);
    }

    /* horizontal and vertical extenders */
    if (get_charinfo_vert_variants(ci) != NULL) {
        set_charinfo_vert_variants(co,
                                   copy_variants(get_charinfo_vert_variants
                                                 (ci)));
    }
    if (get_charinfo_hor_variants(ci) != NULL) {
        set_charinfo_hor_variants(co,
                                  copy_variants(get_charinfo_hor_variants(ci)));
    }
    return co;
}

charinfo *char_info(internal_font_number f, integer c)
{
    if (f > font_id_maxval)
        return 0;
    if (proper_char_index(c)) {
        register int glyph = find_charinfo_id(f, c);
        return &(font_tables[f]->charinfo[glyph]);
    } else if (c == left_boundarychar && left_boundary(f) != NULL) {
        return left_boundary(f);
    } else if (c == right_boundarychar && right_boundary(f) != NULL) {
        return right_boundary(f);
    }
    return &(font_tables[f]->charinfo[0]);
}

charinfo_short char_info_short(internal_font_number f, integer c)
{
    charinfo_short s;
    charinfo *i;
    i = char_info(f, c);
    s.ci_wd = i->width;
    s.ci_dp = i->depth;
    s.ci_ht = i->height;
    return s;
}

integer char_exists(internal_font_number f, integer c)
{
    if (f > font_id_maxval)
        return 0;
    if (proper_char_index(c)) {
        return find_charinfo_id(f, c);
    } else if ((c == left_boundarychar) && has_left_boundary(f)) {
        return 1;
    } else if ((c == right_boundarychar) && has_right_boundary(f)) {
        return 1;
    }
    return 0;
}

int lua_char_exists_callback(internal_font_number f, integer c)
{
    integer callback_id;
    lua_State *L = Luas;
    int ret = 0;
    callback_id = callback_defined(char_exists_callback);
    if (callback_id != 0) {
        if (!get_callback(L, callback_id)) {
            lua_pop(L, 2);
            return 0;
        }
        lua_pushnumber(L, f);
        lua_pushnumber(L, c);
        if (lua_pcall(L, 2, 1, 0) != 0) {       /* two args, 1 result */
            fprintf(stdout, "error: %s\n", lua_tostring(L, -1));
            lua_pop(L, 2);
            error();
        } else {
            ret = lua_toboolean(L, -1);
        }
    }
    return ret;
}


extinfo *new_variant(int glyph, int startconnect, int endconnect,
                     int advance, int repeater)
{
    extinfo *ext;
    ext = xmalloc(sizeof(extinfo));
    ext->next = NULL;
    ext->glyph = glyph;
    ext->start_overlap = startconnect;
    ext->end_overlap = endconnect;
    ext->advance = advance;
    ext->extender = repeater;
    return ext;
}


extinfo *copy_variant(extinfo * old)
{
    extinfo *ext;
    ext = xmalloc(sizeof(extinfo));
    ext->next = NULL;
    ext->glyph = old->glyph;
    ext->start_overlap = old->start_overlap;
    ext->end_overlap = old->end_overlap;
    ext->advance = old->advance;
    ext->extender = old->extender;
    return ext;
}

void dump_variant(extinfo * ext)
{
    dump_int(ext->glyph);
    dump_int(ext->start_overlap);
    dump_int(ext->end_overlap);
    dump_int(ext->advance);
    dump_int(ext->extender);
    return;
}


extinfo *undump_variant(void)
{
    int x;
    extinfo *ext;
    undump_int(x);
    if (x == 0)
        return NULL;
    ext = xmalloc(sizeof(extinfo));
    ext->next = NULL;
    ext->glyph = x;
    undump_int(x);
    ext->start_overlap = x;
    undump_int(x);
    ext->end_overlap = x;
    undump_int(x);
    ext->advance = x;
    undump_int(x);
    ext->extender = x;
    return ext;
}

void add_charinfo_vert_variant(charinfo * ci, extinfo * ext)
{
    if (ci->vert_variants == NULL) {
        ci->vert_variants = ext;
    } else {
        extinfo *lst = ci->vert_variants;
        while (lst->next != NULL)
            lst = lst->next;
        lst->next = ext;
    }

}

void add_charinfo_hor_variant(charinfo * ci, extinfo * ext)
{
    if (ci->hor_variants == NULL) {
        ci->hor_variants = ext;
    } else {
        extinfo *lst = ci->hor_variants;
        while (lst->next != NULL)
            lst = lst->next;
        lst->next = ext;
    }

}

extinfo *copy_variants(extinfo * o)
{
    extinfo *c, *t = NULL, *h = NULL;
    while (o != NULL) {
        c = copy_variant(o);
        if (h == null)
            h = c;
        else
            t->next = c;
        t = c;
        o = o->next;
    }

    return h;
}


void dump_charinfo_variants(extinfo * o)
{
    while (o != NULL) {
        dump_variant(o);
        o = o->next;
    }
    dump_int(0);
    return;
}

extinfo *undump_charinfo_variants(void)
{
    extinfo *c, *t, *h = NULL;
    c = undump_variant();
    while (c != NULL) {
        if (h == null)
            h = c;
        else
            t->next = c;
        t = c;
        c = undump_variant();
    }
    return h;
}


void set_charinfo_width(charinfo * ci, scaled val)
{
    ci->width = val;
}

void set_charinfo_height(charinfo * ci, scaled val)
{
    ci->height = val;
}

void set_charinfo_depth(charinfo * ci, scaled val)
{
    ci->depth = val;
}

void set_charinfo_italic(charinfo * ci, scaled val)
{
    ci->italic = val;
}

void set_charinfo_top_accent(charinfo * ci, scaled val)
{
    ci->top_accent = val;
}

void set_charinfo_bot_accent(charinfo * ci, scaled val)
{
    ci->bot_accent = val;
}

void set_charinfo_tag(charinfo * ci, scaled val)
{
    ci->tag = val;
}

void set_charinfo_remainder(charinfo * ci, scaled val)
{
    ci->remainder = val;
}

void set_charinfo_used(charinfo * ci, scaled val)
{
    ci->used = val;
}

void set_charinfo_index(charinfo * ci, scaled val)
{
    ci->index = val;
}
void set_charinfo_name(charinfo * ci, char *val)
{
    dxfree(ci->name, val);
}
void set_charinfo_tounicode(charinfo * ci, char *val)
{
    dxfree(ci->tounicode, val);
}

void set_charinfo_ligatures(charinfo * ci, liginfo * val)
{
    dxfree(ci->ligatures, val);
}

void set_charinfo_kerns(charinfo * ci, kerninfo * val)
{
    dxfree(ci->kerns, val);
}

void set_charinfo_packets(charinfo * ci, real_eight_bits * val)
{
    dxfree(ci->packets, val);
}

void set_charinfo_ef(charinfo * ci, scaled val)
{
    ci->ef = val;
}

void set_charinfo_lp(charinfo * ci, scaled val)
{
    ci->lp = val;
}

void set_charinfo_rp(charinfo * ci, scaled val)
{
    ci->rp = val;
}

void set_charinfo_vert_variants(charinfo * ci, extinfo * ext)
{
    extinfo *c, *lst;
    if (ci->vert_variants != NULL) {
        lst = ci->vert_variants;
        while (lst != NULL) {
            c = lst->next;
            free(lst);
            lst = c;
        }
    }
    ci->vert_variants = ext;
}

void set_charinfo_hor_variants(charinfo * ci, extinfo * ext)
{
    extinfo *c, *lst;
    if (ci->hor_variants != NULL) {
        lst = ci->hor_variants;
        while (lst != NULL) {
            c = lst->next;
            free(lst);
            lst = c;
        }
    }
    ci->hor_variants = ext;

}

/* In TeX, extensibles were fairly simple things. 
   This function squeezes a TFM extensible into the vertical extender structures.
   |advance==0| is a special case for TFM fonts, because finding the proper 
   advance width during tfm reading can be tricky 
*/

/* a small complication arises if |rep| is the only non-zero: it needs to be 
  doubled as a non-repeatable to avoid mayhem */

void set_charinfo_extensible(charinfo * ci, int top, int bot, int mid, int rep)
{
    extinfo *ext;
    set_charinfo_vert_variants(ci, NULL);       /* clear old */
    if (bot == 0 && top == 0 && mid == 0 && rep != 0) {
        ext = new_variant(rep, 0, 0, 0, EXT_NORMAL);
        add_charinfo_vert_variant(ci, ext);
        ext = new_variant(rep, 0, 0, 0, EXT_REPEAT);
        add_charinfo_vert_variant(ci, ext);
        return;
    }
    if (bot != 0) {
        ext = new_variant(bot, 0, 0, 0, EXT_NORMAL);
        add_charinfo_vert_variant(ci, ext);
    }
    if (rep != 0) {
        ext = new_variant(rep, 0, 0, 0, EXT_REPEAT);
        add_charinfo_vert_variant(ci, ext);
    }
    if (mid != 0) {
        ext = new_variant(mid, 0, 0, 0, EXT_NORMAL);
        add_charinfo_vert_variant(ci, ext);
        if (rep != 0) {
            ext = new_variant(rep, 0, 0, 0, EXT_REPEAT);
            add_charinfo_vert_variant(ci, ext);
        }
    }
    if (top != 0) {
        ext = new_variant(top, 0, 0, 0, EXT_NORMAL);
        add_charinfo_vert_variant(ci, ext);
    }
}

scaled get_charinfo_width(charinfo * ci)
{
    return ci->width;
}

scaled get_charinfo_height(charinfo * ci)
{
    return ci->height;
}

scaled get_charinfo_depth(charinfo * ci)
{
    return ci->depth;
}

scaled get_charinfo_italic(charinfo * ci)
{
    return ci->italic;
}

scaled get_charinfo_top_accent(charinfo * ci)
{
    return ci->top_accent;
}

scaled get_charinfo_bot_accent(charinfo * ci)
{
    return ci->bot_accent;
}

char get_charinfo_tag(charinfo * ci)
{
    return ci->tag;
}

integer get_charinfo_remainder(charinfo * ci)
{
    return ci->remainder;
}

char get_charinfo_used(charinfo * ci)
{
    return ci->used;
}

integer get_charinfo_index(charinfo * ci)
{
    return ci->index;
}

char *get_charinfo_name(charinfo * ci)
{
    return ci->name;
}

char *get_charinfo_tounicode(charinfo * ci)
{
    return ci->tounicode;
}

liginfo *get_charinfo_ligatures(charinfo * ci)
{
    return ci->ligatures;
}

kerninfo *get_charinfo_kerns(charinfo * ci)
{
    return ci->kerns;
}

real_eight_bits *get_charinfo_packets(charinfo * ci)
{
    return ci->packets;
}

integer get_charinfo_ef(charinfo * ci)
{
    return ci->ef;
}

integer get_charinfo_rp(charinfo * ci)
{
    return ci->rp;
}

integer get_charinfo_lp(charinfo * ci)
{
    return ci->lp;
}

extinfo *get_charinfo_vert_variants(charinfo * ci)
{
    extinfo *w = NULL;
    if (ci->vert_variants != NULL)
        w = ci->vert_variants;
    return w;
}

extinfo *get_charinfo_hor_variants(charinfo * ci)
{
    extinfo *w = NULL;
    if (ci->hor_variants != NULL)
        w = ci->hor_variants;
    return w;
}


scaled char_width(internal_font_number f, integer c)
{
    charinfo *ci = char_info(f, c);
    scaled w = get_charinfo_width(ci);
    /*fprintf(stdout,"width of char 0x%x in font %s: %i\n",c,font_name(f),w); */
    return w;
}

scaled char_depth(internal_font_number f, integer c)
{
    charinfo *ci = char_info(f, c);
    scaled w = get_charinfo_depth(ci);
    /*fprintf(stdout,"depth of char 0x%x in font %s: %i\n",c,font_name(f),w); */
    return w;
}

scaled char_height(internal_font_number f, integer c)
{
    charinfo *ci = char_info(f, c);
    scaled w = get_charinfo_height(ci);
    /*fprintf(stdout,"height of char 0x%x in font %s: %i\n",c,font_name(f),w); */
    return w;
}

scaled char_italic(internal_font_number f, integer c)
{
    charinfo *ci = char_info(f, c);
    return get_charinfo_italic(ci);
}

scaled char_top_accent(internal_font_number f, integer c)
{
    charinfo *ci = char_info(f, c);
    return get_charinfo_top_accent(ci);
}

scaled char_bot_accent(internal_font_number f, integer c)
{
    charinfo *ci = char_info(f, c);
    return get_charinfo_bot_accent(ci);
}


integer char_remainder(internal_font_number f, integer c)
{
    charinfo *ci = char_info(f, c);
    return get_charinfo_remainder(ci);
}

char char_tag(internal_font_number f, integer c)
{
    charinfo *ci = char_info(f, c);
    return get_charinfo_tag(ci);
}

char char_used(internal_font_number f, integer c)
{
    charinfo *ci = char_info(f, c);
    return get_charinfo_used(ci);
}

char *char_name(internal_font_number f, integer c)
{
    charinfo *ci = char_info(f, c);
    return get_charinfo_name(ci);
}

integer char_index(internal_font_number f, integer c)
{
    charinfo *ci = char_info(f, c);
    return get_charinfo_index(ci);
}

liginfo *char_ligatures(internal_font_number f, integer c)
{
    charinfo *ci = char_info(f, c);
    return get_charinfo_ligatures(ci);
}

kerninfo *char_kerns(internal_font_number f, integer c)
{
    charinfo *ci = char_info(f, c);
    return get_charinfo_kerns(ci);
}

real_eight_bits *char_packets(internal_font_number f, integer c)
{
    charinfo *ci = char_info(f, c);
    return get_charinfo_packets(ci);
}


void set_font_params(internal_font_number f, int b)
{
    int i;
    i = font_params(f);
    if (i != b) {
        font_bytes += (b - font_params(f) + 1) * sizeof(scaled);
        do_realloc(param_base(f), (b + 2), integer);
        font_params(f) = b;
        if (b > i) {
            while (i < b) {
                i++;
                set_font_param(f, i, 0);
            }
        }
    }
}

void set_font_math_params(internal_font_number f, int b)
{
    int i;
    i = font_math_params(f);
    if (i != b) {
        font_bytes += (b - font_math_params(f) + 1) * sizeof(scaled);
        do_realloc(math_param_base(f), (b + 2), integer);
        font_math_params(f) = b;
        if (b > i) {
            while (i < b) {
                i++;
                set_font_math_param(f, i, undefined_math_parameter);
            }
        }
    }
}



integer copy_font(integer f)
{
    int i;
    charinfo *ci;
    integer k = new_font();
    memcpy(font_tables[k], font_tables[f], sizeof(texfont));

    set_font_cache_id(k, 0);
    set_font_used(k, 0);
    set_font_touched(k, 0);

    font_tables[k]->_font_name = NULL;
    font_tables[k]->_font_filename = NULL;
    font_tables[k]->_font_fullname = NULL;
    font_tables[k]->_font_encodingname = NULL;
    font_tables[k]->_font_area = NULL;
    font_tables[k]->_font_cidregistry = NULL;
    font_tables[k]->_font_cidordering = NULL;
    font_tables[k]->_left_boundary = NULL;
    font_tables[k]->_right_boundary = NULL;

    set_font_name(k, xstrdup(font_name(f)));
    if (font_filename(f) != NULL)
        set_font_filename(k, xstrdup(font_filename(f)));
    if (font_fullname(f) != NULL)
        set_font_fullname(k, xstrdup(font_fullname(f)));
    if (font_encodingname(f) != NULL)
        set_font_encodingname(k, xstrdup(font_encodingname(f)));
    if (font_area(f) != NULL)
        set_font_area(k, xstrdup(font_area(f)));
    if (font_cidregistry(f) != NULL)
        set_font_cidregistry(k, xstrdup(font_cidregistry(f)));
    if (font_cidordering(f) != NULL)
        set_font_cidordering(k, xstrdup(font_cidordering(f)));

    i = sizeof(*param_base(f)) * font_params(f);
    font_bytes += i;
    param_base(k) = xmalloc(i);
    memcpy(param_base(k), param_base(f), i);

    if (font_math_params(f) > 0) {
        i = sizeof(*math_param_base(f)) * font_math_params(f);
        font_bytes += i;
        math_param_base(k) = xmalloc(i);
        memcpy(math_param_base(k), math_param_base(f), i);
    }

    i = sizeof(charinfo) * (Charinfo_size(f) + 1);
    font_bytes += i;
    font_tables[k]->charinfo = xmalloc(i);
    memset(font_tables[k]->charinfo, 0, i);
    for (i = 0; i <= Charinfo_size(k); i++) {
        ci = copy_charinfo(&font_tables[f]->charinfo[i]);
        font_tables[k]->charinfo[i] = *ci;
    }

    if (left_boundary(f) != NULL) {
        ci = copy_charinfo(left_boundary(f));
        set_charinfo(k, left_boundarychar, ci);
    }

    if (right_boundary(f) != NULL) {
        ci = copy_charinfo(right_boundary(f));
        set_charinfo(k, right_boundarychar, ci);
    }
    return k;
}

void delete_font(integer f)
{
    int i;
    charinfo *co;
    assert(f > 0);
    if (font_tables[f] != NULL) {
        set_font_name(f, NULL);
        set_font_filename(f, NULL);
        set_font_fullname(f, NULL);
        set_font_encodingname(f, NULL);
        set_font_area(f, NULL);
        set_font_cidregistry(f, NULL);
        set_font_cidordering(f, NULL);
        set_left_boundary(f, NULL);
        set_right_boundary(f, NULL);

        for (i = font_bc(f); i <= font_ec(f); i++) {
            if (char_exists(f, i)) {
                co = char_info(f, i);
                set_charinfo_name(co, NULL);
                set_charinfo_tounicode(co, NULL);
                set_charinfo_packets(co, NULL);
                set_charinfo_ligatures(co, NULL);
                set_charinfo_kerns(co, NULL);
                set_charinfo_vert_variants(co, NULL);
                set_charinfo_hor_variants(co, NULL);
            }
        }
        /* free .notdef */
        set_charinfo_name(font_tables[f]->charinfo + 0, NULL);
        free(font_tables[f]->charinfo);
        destroy_sa_tree(font_tables[f]->characters);

        free(param_base(f));
        if (math_param_base(f) != NULL)
            free(math_param_base(f));
        free(font_tables[f]);
        font_tables[f] = NULL;

        if (font_id_maxval == f) {
            font_id_maxval--;
        }
    }
}

void create_null_font(void)
{
    int i = new_font();
    assert(i == 0);
    set_font_name(i, xstrdup("nullfont"));
    set_font_area(i, xstrdup(""));
    set_font_touched(i, 1);
}

boolean is_valid_font(integer id)
{
    int ret = 0;
    if (id >= 0 && id <= font_id_maxval && font_tables[id] != NULL)
        ret = 1;
    return ret;
}

/* return 1 == identical */
boolean cmp_font_name(integer id, strnumber t)
{
    char *tid, *tt;
    if (!is_valid_font(id))
        return 0;
    tt = makecstring(t);
    tid = font_name(id);
    if (tt == NULL && tid == NULL)
        return 1;
    if (tt == NULL || tid == NULL || strcmp(tid, tt) != 0)
        return 0;
    return 1;
}

boolean cmp_font_area(integer id, strnumber t)
{
    char *tt = NULL;
    char *tid = font_area(id);
    if (t == 0) {
        if (tid == NULL || strlen(tid) == 0)
            return 1;
        else
            return 0;
    }
    tt = makecstring(t);
    if ((tt == NULL || strlen(tt) == 0) && (tid == NULL || strlen(tid) == 0))
        return 1;
    if (tt == NULL || strcmp(tid, tt) != 0)
        return 0;
    return 1;
}


static boolean same_font_name(integer id, integer t)
{
    int ret = 0;
    if (font_name(t) == NULL ||
        font_name(id) == NULL || strcmp(font_name(t), font_name(id)) != 0) {
        ;
    } else {
        ret = 1;
    }
    return ret;
}

boolean font_shareable(internal_font_number f, internal_font_number k)
{
    int ret = 0;
    /* For some lua-loaded (for instance AFM) fonts, it is normal to have 
       a zero cidregistry,  and such fonts do not have a fontmap entry yet
       at this point, so the test shoulh use the other branch  */
    if (font_cidregistry(f) == NULL && font_cidregistry(k) == NULL &&
        font_encodingbytes(f) != 2 && font_encodingbytes(k) != 2) {
        if (hasfmentry(k)
            && (font_map(k) == font_map(f))
            && (same_font_name(k, f)
                || (pdf_font_auto_expand(f)
                    && (pdf_font_blink(f) != 0) /* 0 = nullfont */
                    &&same_font_name(k, pdf_font_blink(f))))) {
            ret = 1;
        }
    } else {
        if ((font_filename(k) != NULL && font_filename(f) != NULL &&
             strcmp(font_filename(k), font_filename(f)) == 0)
            || (pdf_font_auto_expand(f)
                && (pdf_font_blink(f) != 0)     /* 0 = nullfont */
                &&same_font_name(k, pdf_font_blink(f)))) {
            ret = 1;
        }
    }
    return ret;
}

integer test_no_ligatures(internal_font_number f)
{
    integer c;
    for (c = font_bc(f); c <= font_ec(f); c++) {
        if (has_lig(f, c))      /* char_exists(f,c) */
            return 0;
    }
    return 1;
}

integer get_tag_code(internal_font_number f, integer c)
{
    small_number i;
    if (char_exists(f, c)) {
        i = char_tag(f, c);
        if (i == lig_tag)
            return 1;
        else if (i == list_tag)
            return 2;
        else if (i == ext_tag)
            return 4;
        else
            return 0;
    }
    return -1;
}

integer get_lp_code(internal_font_number f, integer c)
{
    charinfo *ci = char_info(f, c);
    return get_charinfo_lp(ci);
}

integer get_rp_code(internal_font_number f, integer c)
{
    charinfo *ci = char_info(f, c);
    return get_charinfo_rp(ci);
}

integer get_ef_code(internal_font_number f, integer c)
{
    charinfo *ci = char_info(f, c);
    return get_charinfo_ef(ci);
}

void set_tag_code(internal_font_number f, integer c, integer i)
{
    integer fixedi;
    charinfo *co;
    if (char_exists(f, c)) {
        /* abs(fix_int(i-7,0)) */
        fixedi = -(i < -7 ? -7 : (i > 0 ? 0 : i));
        co = char_info(f, c);
        if (fixedi >= 4) {
            if (char_tag(f, c) == ext_tag)
                set_charinfo_tag(co, (char_tag(f, c) - ext_tag));
            fixedi = fixedi - 4;
        }
        if (fixedi >= 2) {
            if (char_tag(f, c) == list_tag)
                set_charinfo_tag(co, (char_tag(f, c) - list_tag));
            fixedi = fixedi - 2;
        };
        if (fixedi >= 1) {
            if (has_lig(f, c))
                set_charinfo_ligatures(co, NULL);
            if (has_kern(f, c))
                set_charinfo_kerns(co, NULL);
        }
    }
}


void set_lp_code(internal_font_number f, integer c, integer i)
{
    charinfo *co;
    if (char_exists(f, c)) {
        co = char_info(f, c);
        set_charinfo_lp(co, i);
    }
}

void set_rp_code(internal_font_number f, integer c, integer i)
{
    charinfo *co;
    if (char_exists(f, c)) {
        co = char_info(f, c);
        set_charinfo_rp(co, i);
    }
}

void set_ef_code(internal_font_number f, integer c, integer i)
{
    charinfo *co;
    if (char_exists(f, c)) {
        co = char_info(f, c);
        set_charinfo_ef(co, i);
    }
}

void set_no_ligatures(internal_font_number f)
{
    integer c;
    charinfo *co;

    if (font_tables[f]->ligatures_disabled)
        return;

    co = char_info(f, left_boundarychar);
    set_charinfo_ligatures(co, NULL);
    co = char_info(f, right_boundarychar);      /* this is weird */
    set_charinfo_ligatures(co, NULL);
    for (c = 0; c < font_tables[f]->charinfo_count; c++) {
        co = font_tables[f]->charinfo + c;
        set_charinfo_ligatures(co, NULL);
    }
    font_tables[f]->ligatures_disabled = 1;
}

liginfo get_ligature(internal_font_number f, integer lc, integer rc)
{
    liginfo t, u;
    charinfo *co;
    t.lig = 0;
    t.type = 0;
    t.adj = 0;
    if (lc == non_boundarychar || rc == non_boundarychar || (!has_lig(f, lc)))
        return t;
    k = 0;
    co = char_info(f, lc);
    while (1) {
        u = charinfo_ligature(co, k);
        if (lig_end(u))
            break;
        if (lig_char(u) == rc) {
            if (lig_disabled(u)) {
                return t;
            } else {
                return u;
            }
        }
        k++;
    }
    return t;
}


scaled raw_get_kern(internal_font_number f, integer lc, integer rc)
{
    integer k;
    kerninfo u;
    charinfo *co;
    if (lc == non_boundarychar || rc == non_boundarychar)
        return 0;
    k = 0;
    co = char_info(f, lc);
    while (1) {
        u = charinfo_kern(co, k);
        if (kern_end(u))
            break;
        if (kern_char(u) == rc) {
            if (kern_disabled(u))
                return 0;
            else
                return kern_kern(u);
        }
        k++;
    }
    return 0;
}


scaled get_kern(internal_font_number f, integer lc, integer rc)
{
    if (lc == non_boundarychar || rc == non_boundarychar || (!has_kern(f, lc)))
        return 0;
    return raw_get_kern(f, lc, rc);
}


/* dumping and undumping fonts */

#define dump_string(a)        \
  if (a!=NULL) {        \
    x = strlen(a)+1;        \
    dump_int(x);  dump_things(*a, x);   \
  } else {          \
    x = 0; dump_int(x);       \
  }

void dump_charinfo(int f, int c)
{
    charinfo *co;
    int x;
    liginfo *lig;
    kerninfo *kern;
    dump_int(c);
    co = char_info(f, c);
    set_charinfo_used(co, 0);
    dump_int(get_charinfo_width(co));
    dump_int(get_charinfo_height(co));
    dump_int(get_charinfo_depth(co));
    dump_int(get_charinfo_italic(co));
    dump_int(get_charinfo_top_accent(co));
    dump_int(get_charinfo_bot_accent(co));
    dump_int(get_charinfo_tag(co));
    dump_int(get_charinfo_ef(co));
    dump_int(get_charinfo_rp(co));
    dump_int(get_charinfo_lp(co));
    dump_int(get_charinfo_remainder(co));
    dump_int(get_charinfo_used(co));
    dump_int(get_charinfo_index(co));
    dump_string(get_charinfo_name(co));
    dump_string(get_charinfo_tounicode(co));

    /* ligatures */
    x = 0;
    if ((lig = get_charinfo_ligatures(co)) != NULL) {
        while (!lig_end(lig[x])) {
            x++;
        }
        x++;
        dump_int(x);
        dump_things(*lig, x);
    } else {
        dump_int(x);
    }
    /* kerns */
    x = 0;
    if ((kern = get_charinfo_kerns(co)) != NULL) {
        while (!kern_end(kern[x])) {
            x++;
        }
        x++;
        dump_int(x);
        dump_things(*kern, x);
    } else {
        dump_int(x);
    }
    /* packets */
    x = vf_packet_bytes(co);
    dump_int(x);
    if (x > 0) {
        dump_things(*get_charinfo_packets(co), x);
    }

    if (get_charinfo_tag(co) == ext_tag) {
        dump_charinfo_variants(get_charinfo_vert_variants(co));
        dump_charinfo_variants(get_charinfo_hor_variants(co));
    }
}

void dump_font(int f)
{
    int i, x;

    set_font_used(f, 0);
    font_tables[f]->charinfo_cache = NULL;
    dump_things(*(font_tables[f]), 1);
    dump_string(font_name(f));
    dump_string(font_area(f));
    dump_string(font_filename(f));
    dump_string(font_fullname(f));
    dump_string(font_encodingname(f));
    dump_string(font_cidregistry(f));
    dump_string(font_cidordering(f));

    dump_things(*param_base(f), (font_params(f) + 1));

    if (font_math_params(f) > 0) {
        dump_things(*math_param_base(f), (font_math_params(f)));
    }
    if (has_left_boundary(f)) {
        dump_int(1);
        dump_charinfo(f, left_boundarychar);
    } else {
        dump_int(0);
    }
    if (has_right_boundary(f)) {
        dump_int(1);
        dump_charinfo(f, right_boundarychar);
    } else {
        dump_int(0);
    }

    for (i = font_bc(f); i <= font_ec(f); i++) {
        if (char_exists(f, i)) {
            dump_charinfo(f, i);
        }
    }
}

int undump_charinfo(int f)
{
    charinfo *co;
    int x, i;
    char *s = NULL;
    liginfo *lig = NULL;
    kerninfo *kern = NULL;
    real_eight_bits *packet = NULL;

    undump_int(i);
    co = get_charinfo(f, i);
    undump_int(x);
    set_charinfo_width(co, x);
    undump_int(x);
    set_charinfo_height(co, x);
    undump_int(x);
    set_charinfo_depth(co, x);
    undump_int(x);
    set_charinfo_italic(co, x);
    undump_int(x);
    set_charinfo_top_accent(co, x);
    undump_int(x);
    set_charinfo_bot_accent(co, x);
    undump_int(x);
    set_charinfo_tag(co, x);
    undump_int(x);
    set_charinfo_ef(co, x);
    undump_int(x);
    set_charinfo_rp(co, x);
    undump_int(x);
    set_charinfo_lp(co, x);
    undump_int(x);
    set_charinfo_remainder(co, x);
    undump_int(x);
    set_charinfo_used(co, x);
    undump_int(x);
    set_charinfo_index(co, x);

    /* name */
    undump_int(x);
    if (x > 0) {
        font_bytes += x;
        s = xmalloc(x);
        undump_things(*s, x);
    }
    set_charinfo_name(co, s);
    /* tounicode */
    undump_int(x);
    if (x > 0) {
        font_bytes += x;
        s = xmalloc(x);
        undump_things(*s, x);
    }
    set_charinfo_tounicode(co, s);
    /* ligatures */
    undump_int(x);
    if (x > 0) {
        font_bytes += x * sizeof(liginfo);
        lig = xmalloc(x * sizeof(liginfo));
        undump_things(*lig, x);
    }
    set_charinfo_ligatures(co, lig);
    /* kerns */
    undump_int(x);
    if (x > 0) {
        font_bytes += x * sizeof(kerninfo);
        kern = xmalloc(x * sizeof(kerninfo));
        undump_things(*kern, x);
    }
    set_charinfo_kerns(co, kern);

    /* packets */
    undump_int(x);
    if (x > 0) {
        font_bytes += x;
        packet = xmalloc(x);
        undump_things(*packet, x);
    }
    set_charinfo_packets(co, packet);

    if (get_charinfo_tag(co) == ext_tag) {
        set_charinfo_vert_variants(co, undump_charinfo_variants());
        set_charinfo_hor_variants(co, undump_charinfo_variants());
    }
    return i;
}

#define undump_font_string(a)   undump_int (x); \
  if (x>0) {          \
    font_bytes += x;        \
    s = xmalloc(x); undump_things(*s,x);  \
    a(f,s); }


void undump_font(int f)
{
    int x, i;
    texfont *tt;
    charinfo *ci;
    char *s;
    grow_font_table(f);
    font_tables[f] = NULL;
    font_bytes += sizeof(texfont);
    tt = xmalloc(sizeof(texfont));
    undump_things(*tt, 1);
    /* these |char *| need resetting */
    tt->_font_name = NULL;
    tt->_font_area = NULL;
    tt->_font_filename = NULL;
    tt->_font_fullname = NULL;
    tt->_font_encodingname = NULL;
    tt->_font_cidregistry = NULL;
    tt->_font_cidordering = NULL;
    font_tables[f] = tt;

    undump_font_string(set_font_name);
    undump_font_string(set_font_area);
    undump_font_string(set_font_filename);
    undump_font_string(set_font_fullname);
    undump_font_string(set_font_encodingname);
    undump_font_string(set_font_cidregistry);
    undump_font_string(set_font_cidordering);

    i = sizeof(*param_base(f)) * (font_params(f) + 1);
    font_bytes += i;
    param_base(f) = xmalloc(i);
    undump_things(*param_base(f), (font_params(f) + 1));

    if (font_math_params(f) > 0) {
        i = sizeof(*math_param_base(f)) * (font_math_params(f) + 1);
        font_bytes += i;
        math_param_base(f) = xmalloc(i);
        undump_things(*math_param_base(f), (font_math_params(f) + 1));
    } else {
        math_param_base(f) = NULL;
    }

    font_tables[f]->_left_boundary = NULL;
    undump_int(x);
    if (x) {
        i = undump_charinfo(f);
    }
    /* left boundary */
    font_tables[f]->_right_boundary = NULL;
    undump_int(x);
    if (x) {
        i = undump_charinfo(f);
    }

    /* right boundary */
    font_tables[f]->characters = new_sa_tree(1, 0);     /* stack size 1, default item value 0 */
    ci = xcalloc(1, sizeof(charinfo));
    set_charinfo_name(ci, xstrdup(".notdef"));
    font_tables[f]->charinfo = ci;

    i = font_bc(f);
    while (i < font_ec(f)) {
        i = undump_charinfo(f);
    }
}
