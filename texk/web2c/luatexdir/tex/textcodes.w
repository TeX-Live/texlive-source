% textcodes.w
%
% Copyright 2006-2010 Taco Hoekwater <taco@@luatex.org>
%
% This file is part of LuaTeX.
%
% LuaTeX is free software; you can redistribute it and/or modify it under
% the terms of the GNU General Public License as published by the Free
% Software Foundation; either version 2 of the License, or (at your
% option) any later version.
%
% LuaTeX is distributed in the hope that it will be useful, but WITHOUT
% ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
% FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
% License for more details.
%
% You should have received a copy of the GNU General Public License along
% with LuaTeX; if not, see <http://www.gnu.org/licenses/>.

@ @c


#include "ptexlib.h"

@ @c
#define LCCODESTACK  8
#define LCCODEDEFAULT 0

static sa_tree lccode_head = NULL;

#define UCCODESTACK  8
#define UCCODEDEFAULT 0

static sa_tree uccode_head = NULL;

#define SFCODESTACK 8
#define SFCODEDEFAULT 1000

static sa_tree sfcode_head = NULL;

#define CATCODESTACK 8
#define CATCODEDEFAULT 12


void set_lc_code(int n, halfword v, quarterword gl)
{
    set_sa_item(lccode_head, n, (sa_tree_item) v, gl);
}

halfword get_lc_code(int n)
{
    return (halfword) get_sa_item(lccode_head, n);
}

static void unsavelccodes(quarterword gl)
{
    restore_sa_stack(lccode_head, gl);
}

static void initializelccodes(void)
{
    lccode_head = new_sa_tree(LCCODESTACK, LCCODEDEFAULT);
}

static void dumplccodes(void)
{
    dump_sa_tree(lccode_head);
}

static void undumplccodes(void)
{
    lccode_head = undump_sa_tree();
}

void set_uc_code(int n, halfword v, quarterword gl)
{
    set_sa_item(uccode_head, n, (sa_tree_item) v, gl);
}

halfword get_uc_code(int n)
{
    return (halfword) get_sa_item(uccode_head, n);
}

static void unsaveuccodes(quarterword gl)
{
    restore_sa_stack(uccode_head, gl);
}

static void initializeuccodes(void)
{
    uccode_head = new_sa_tree(UCCODESTACK, UCCODEDEFAULT);
}

static void dumpuccodes(void)
{
    dump_sa_tree(uccode_head);
}

static void undumpuccodes(void)
{
    uccode_head = undump_sa_tree();
}

void set_sf_code(int n, halfword v, quarterword gl)
{
    set_sa_item(sfcode_head, n, (sa_tree_item) v, gl);
}

halfword get_sf_code(int n)
{
    return (halfword) get_sa_item(sfcode_head, n);
}

static void unsavesfcodes(quarterword gl)
{
    restore_sa_stack(sfcode_head, gl);
}

static void initializesfcodes(void)
{
    sfcode_head = new_sa_tree(SFCODESTACK, SFCODEDEFAULT);
}

static void dumpsfcodes(void)
{
    dump_sa_tree(sfcode_head);
}

static void undumpsfcodes(void)
{
    sfcode_head = undump_sa_tree();
}


static sa_tree *catcode_heads = NULL;
static int catcode_max = 0;
static unsigned char *catcode_valid = NULL;

#define CATCODE_MAX 32767

#define update_catcode_max(h)  if (h > catcode_max)  catcode_max = h

void set_cat_code(int h, int n, halfword v, quarterword gl)
{
    sa_tree s = catcode_heads[h];
    update_catcode_max(h);
    if (s == NULL) {
        s = new_sa_tree(CATCODESTACK, CATCODEDEFAULT);
        catcode_heads[h] = s;
    }
    set_sa_item(s, n, (sa_tree_item) v, gl);
}

halfword get_cat_code(int h, int n)
{
    sa_tree s = catcode_heads[h];
    update_catcode_max(h);
    if (s == NULL) {
        s = new_sa_tree(CATCODESTACK, CATCODEDEFAULT);
        catcode_heads[h] = s;
    }
    return (halfword) get_sa_item(s, n);
}

void unsave_cat_codes(int h, quarterword gl)
{
    int k;
    update_catcode_max(h);
    for (k = 0; k <= catcode_max; k++) {
        if (catcode_heads[k] != NULL)
            restore_sa_stack(catcode_heads[k], gl);
    }
}

#if 0
static void clearcatcodestack(int h)
{
    clear_sa_stack(catcode_heads[h]);
}
#endif

static void initializecatcodes(void)
{
    catcode_max = 0;
#if 0
    xfree(catcode_heads); /* not needed */
    xfree(catcode_valid); 
#endif
    catcode_heads = Mxmalloc_array(sa_tree, (CATCODE_MAX + 1));
    catcode_valid = Mxmalloc_array(unsigned char, (CATCODE_MAX + 1));
    memset(catcode_heads, 0, sizeof(sa_tree) * (CATCODE_MAX + 1));
    memset(catcode_valid, 0, sizeof(unsigned char) * (CATCODE_MAX + 1));
    catcode_valid[0] = 1;
    catcode_heads[0] = new_sa_tree(CATCODESTACK, CATCODEDEFAULT);
}

static void dumpcatcodes(void)
{
    int k, total;
    dump_int(catcode_max);
    total = 0;
    for (k = 0; k <= catcode_max; k++) {
        if (catcode_valid[k]) {
            total++;
        }
    }
    dump_int(total);
    for (k = 0; k <= catcode_max; k++) {
        if (catcode_valid[k]) {
            dump_int(k);
            dump_sa_tree(catcode_heads[k]);
        }
    }
}

static void undumpcatcodes(void)
{
    int total, k, x;
    xfree(catcode_heads);
    xfree(catcode_valid);
    catcode_heads = Mxmalloc_array(sa_tree, (CATCODE_MAX + 1));
    catcode_valid = Mxmalloc_array(unsigned char, (CATCODE_MAX + 1));
    memset(catcode_heads, 0, sizeof(sa_tree) * (CATCODE_MAX + 1));
    memset(catcode_valid, 0, sizeof(unsigned char) * (CATCODE_MAX + 1));
    undump_int(catcode_max);
    undump_int(total);
    for (k = 0; k < total; k++) {
        undump_int(x);
        catcode_heads[x] = undump_sa_tree();
        catcode_valid[x] = 1;
    }
}

int valid_catcode_table(int h)
{
    if (h <= CATCODE_MAX && h >= 0 && catcode_valid[h]) {
        return 1;
    }
    return 0;
}

void copy_cat_codes(int from, int to)
{
    if (from < 0 || from > CATCODE_MAX || catcode_valid[from] == 0) {
        uexit(1);
    }
    update_catcode_max(to);
    destroy_sa_tree(catcode_heads[to]);
    catcode_heads[to] = copy_sa_tree(catcode_heads[from]);
    catcode_valid[to] = 1;
}

void initex_cat_codes(int h)
{
    int k;
    update_catcode_max(h);
    destroy_sa_tree(catcode_heads[h]);
    catcode_heads[h] = NULL;
    set_cat_code(h, '\r', car_ret_cmd, 1);
    set_cat_code(h, ' ', spacer_cmd, 1);
    set_cat_code(h, '\\', escape_cmd, 1);
    set_cat_code(h, '%', comment_cmd, 1);
    set_cat_code(h, 127, invalid_char_cmd, 1);
    set_cat_code(h, 0, ignore_cmd, 1);
    set_cat_code(h, 0xFEFF, ignore_cmd, 1);
    for (k = 'A'; k <= 'Z'; k++) {
        set_cat_code(h, k, letter_cmd, 1);
        set_cat_code(h, k + 'a' - 'A', letter_cmd, 1);
    }
    catcode_valid[h] = 1;
}

void unsave_text_codes(quarterword grouplevel)
{
    unsavesfcodes(grouplevel);
    unsaveuccodes(grouplevel);
    unsavelccodes(grouplevel);
}

void initialize_text_codes(void)
{
    initializesfcodes();
    initializeuccodes();
    initializecatcodes();
    initializelccodes();
}

void free_text_codes(void)
{
    int k;
    destroy_sa_tree(lccode_head);
    destroy_sa_tree(uccode_head);
    destroy_sa_tree(sfcode_head);
    for (k = 0; k <= catcode_max; k++) {
        if (catcode_valid[k]) {
            destroy_sa_tree(catcode_heads[k]);
        }
    }
    xfree(catcode_heads);
    xfree(catcode_valid);
}


void dump_text_codes(void)
{
    dumpsfcodes();
    dumpuccodes();
    dumplccodes();
    dumpcatcodes();
}

void undump_text_codes(void)
{
    undumpsfcodes();
    undumpuccodes();
    undumplccodes();
    undumpcatcodes();
}
