% mathnodes.w
%
% Copyright 2006-2012 Taco Hoekwater <taco@@luatex.org>
% Copyright 2012 Khaled Hosny <khaledhosny@@eglug.org>
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

@ math codes 
@c
static sa_tree mathcode_head = NULL;

#define MATHCODEHEAP 8

static mathcodeval *mathcode_heap = NULL;
static int mathcode_heapsize = MATHCODEHEAP;
static int mathcode_heapptr = 0;

/* the 0xFFFFFFFF is a flag value */
#define MATHCODESTACK 8
#define MATHCODEDEFAULT 0xFFFFFFFF

@ delcodes
@c
static sa_tree delcode_head = NULL;

#define DELCODEHEAP 8

static delcodeval *delcode_heap = NULL;
static int delcode_heapsize = DELCODEHEAP;
static int delcode_heapptr = 0;

#define DELCODESTACK 4
#define DELCODEDEFAULT 0xFFFFFFFF

@ some helpers for mathcode printing

@c
#define print_hex_digit(A) do {                 \
    if ((A)>=10) print_char('A'+(A)-10);        \
    else print_char('0'+(A));                   \
  } while (0)

#define two_hex(A) do {       \
    print_hex_digit((A)/16);  \
    print_hex_digit((A)%16);  \
  } while (0)

#define four_hex(A) do {      \
    two_hex((A)/256);         \
    two_hex((A)%256);         \
  } while (0)

#define six_hex(A) do {       \
    two_hex((A)/65536);       \
    two_hex(((A)%65536)/256); \
    two_hex((A)%256);         \
  } while (0)

@ @c
void show_mathcode_value(mathcodeval c)
{
    if (c.origin_value == xetex_mathcode) {
        print_char('"');
        print_hex_digit(c.class_value);
        print_char('"');
        two_hex(c.family_value);
        print_char('"');
        six_hex(c.character_value);
    } else if (c.origin_value == xetexnum_mathcode) {
        int m;
        m = (c.class_value + (c.family_value * 8)) * 2097152 +
            c.character_value;
        print_int(m);
    } else {
        print_char('"');
	if (c.class_value) {
	    print_hex_digit(c.class_value);
            print_hex_digit(c.family_value);
            two_hex(c.character_value);
        } else if (c.family_value) {
            print_hex_digit(c.family_value);
            two_hex(c.character_value);
        } else if (c.character_value >= 16) {
            two_hex(c.character_value);
        } else {
            print_hex_digit(c.character_value);
        }
    }
}

@ @c
static void show_mathcode(int n)
{
    mathcodeval c = get_math_code(n);
    if (c.origin_value == xetex_mathcode) {
        tprint_esc("Umathcode");
    } else if (c.origin_value == xetexnum_mathcode) {
        tprint_esc("Umathcodenum");
    } else {
        tprint_esc("mathcode");
    }
    print_int(n);
    print_char('=');
    show_mathcode_value(c);
}

@ @c
static void unsavemathcode(quarterword gl)
{
    sa_stack_item st;
    if (mathcode_head->stack == NULL)
        return;
    while (mathcode_head->stack_ptr > 0 &&
           abs(mathcode_head->stack[mathcode_head->stack_ptr].level)
           >= (int) gl) {
        st = mathcode_head->stack[mathcode_head->stack_ptr];
        if (st.level > 0) {
            rawset_sa_item(mathcode_head, st.code, st.value);
            /* now do a trace message, if requested */
            if (int_par(tracing_restores_code) > 0) {
                begin_diagnostic();
                print_char('{');
                tprint("restoring");
                print_char(' ');
                show_mathcode(st.code);
                print_char('}');
                end_diagnostic(false);
            }
        }
        (mathcode_head->stack_ptr)--;
    }
}

@ @c
void set_math_code(int n,
                   int commandorigin,
                   int mathclass,
                   int mathfamily, int mathcharacter, quarterword level)
{
    mathcodeval d;
    d.origin_value = commandorigin;
    d.class_value = mathclass;
    d.family_value = mathfamily;
    d.character_value = mathcharacter;
    if (mathcode_heapptr == mathcode_heapsize) {
        mathcode_heapsize += MATHCODEHEAP;
        mathcode_heap =
            Mxrealloc_array(mathcode_heap, mathcodeval, mathcode_heapsize);
    }
    mathcode_heap[mathcode_heapptr] = d;
    set_sa_item(mathcode_head, n, (sa_tree_item) mathcode_heapptr, level);
    mathcode_heapptr++;
    if (int_par(tracing_assigns_code) > 0) {
        begin_diagnostic();
        print_char('{');
        tprint("assigning");
        print_char(' ');
        show_mathcode(n);
        print_char('}');
        end_diagnostic(false);
    }
}

@ @c
mathcodeval get_math_code(int n)
{
    unsigned int ret;
    ret = get_sa_item(mathcode_head, n);
    if (ret == MATHCODEDEFAULT) {
        mathcodeval d;
        d.class_value = 0;
        d.family_value = 0;
        d.origin_value = (n < 256 ? tex_mathcode : xetex_mathcode);
        d.character_value = n;
        return d;
    } else {
        return mathcode_heap[ret];
    }
}


@ @c
int get_math_code_num(int n, boolean compat)
{
    mathcodeval mval;
    mval = get_math_code(n);
    if (compat) { /* \.{\\the\\mathcode} */
        if (mval.class_value > 8
            || mval.family_value > 15
            || mval.character_value > 255) {
            print_err("Extended mathchar used as mathchar");
            help2("A mathchar number must be between 0 and \"8000.",
                  "I changed this one to zero.");
            int_error(get_math_code_num(n, false));
            return 0;
        } else
            return mval.class_value * 4096 + mval.family_value * 256 + mval.character_value;
    } else { /* \.{\\the\\Umathcodenum} */
        return (mval.class_value + (mval.family_value * 8)) * (65536 * 32) + mval.character_value;
    }
    return 0;
}

@ @c
static void initializemathcode(void)
{
    mathcode_head = new_sa_tree(MATHCODESTACK, MATHCODEDEFAULT);
    mathcode_heap = Mxmalloc_array(mathcodeval, MATHCODEHEAP);
}

@ @c
static void dumpmathcode(void)
{
    int k;
    mathcodeval d;
    dump_sa_tree(mathcode_head);
    dump_int(mathcode_heapsize);
    dump_int(mathcode_heapptr);
    for (k = 0; k < mathcode_heapptr; k++) {
        d = mathcode_heap[k];
        dump_int(d.origin_value);
        dump_int(d.class_value);
        dump_int(d.family_value);
        dump_int(d.character_value);
    }
}

static void undumpmathcode(void)
{
    int k, x;
    mathcodeval d;
    mathcode_head = undump_sa_tree();
    undump_int(mathcode_heapsize);
    undump_int(mathcode_heapptr);
    mathcode_heap = Mxmalloc_array(mathcodeval, mathcode_heapsize);
    for (k = 0; k < mathcode_heapptr; k++) {
        undump_int(x);
        d.origin_value = x;
        undump_int(x);
        d.class_value = x;
        undump_int(x);
        d.family_value = x;
        undump_int(x);
        d.character_value = x;
        mathcode_heap[k] = d;
    }
    d.origin_value = 0;
    d.class_value = 0;
    d.family_value = 0;
    d.character_value = 0;
    for (k = mathcode_heapptr; k < mathcode_heapsize; k++) {
        mathcode_heap[k] = d;
    }
}

@ @c
static void show_delcode(int n)
{
    delcodeval c;
    c = get_del_code(n);
    if (c.origin_value == tex_mathcode) {
        tprint_esc("delcode");
    } else if (c.origin_value == xetex_mathcode) {
        tprint_esc("Udelcode");
    } else if (c.origin_value == xetexnum_mathcode) {
        tprint_esc("Udelcodenum");
    }
    print_int(n);
    print_char('=');
    if (c.small_family_value < 0) {
        print_char('-');
        print_char('1');
    } else {
        if (c.origin_value == tex_mathcode) {
            print_char('"');
            print_hex_digit(c.small_family_value);
            two_hex(c.small_character_value);
            print_hex_digit(c.large_family_value);
            two_hex(c.large_character_value);
        } else if (c.origin_value == xetex_mathcode) {
            print_char('"');
            two_hex(c.small_family_value);
            six_hex(c.small_character_value);
        } else if (c.origin_value == xetexnum_mathcode) {
            int m;
            m = c.small_family_value * 2097152 + c.small_character_value;
            print_int(m);
        }
    }
}



@ TODO: clean up the heap
@c
static void unsavedelcode(quarterword gl)
{
    sa_stack_item st;
    if (delcode_head->stack == NULL)
        return;
    while (delcode_head->stack_ptr > 0 &&
           abs(delcode_head->stack[delcode_head->stack_ptr].level)
           >= (int) gl) {
        st = delcode_head->stack[delcode_head->stack_ptr];
        if (st.level > 0) {
            rawset_sa_item(delcode_head, st.code, st.value);
            /* now do a trace message, if requested */
            if (int_par(tracing_restores_code) > 0) {
                begin_diagnostic();
                print_char('{');
                tprint("restoring");
                print_char(' ');
                show_delcode(st.code);
                print_char('}');
                end_diagnostic(false);
            }
        }
        (delcode_head->stack_ptr)--;
    }

}

@ @c
void set_del_code(int n,
                  int commandorigin,
                  int smathfamily,
                  int smathcharacter,
                  int lmathfamily, int lmathcharacter, quarterword gl)
{
    delcodeval d;
    d.class_value = 0;
    d.origin_value = commandorigin;
    d.small_family_value = smathfamily;
    d.small_character_value = smathcharacter;
    d.large_family_value = lmathfamily;
    d.large_character_value = lmathcharacter;
    if (delcode_heapptr == delcode_heapsize) {
        delcode_heapsize += DELCODEHEAP;
        delcode_heap =
            Mxrealloc_array(delcode_heap, delcodeval, delcode_heapsize);
    }
    delcode_heap[delcode_heapptr] = d;
    set_sa_item(delcode_head, n, (sa_tree_item) delcode_heapptr, gl);
    if (int_par(tracing_assigns_code) > 0) {
        begin_diagnostic();
        print_char('{');
        tprint("assigning");
        print_char(' ');
        show_delcode(n);
        print_char('}');
        end_diagnostic(false);
    }
    delcode_heapptr++;
}

@ @c
delcodeval get_del_code(int n)
{
    unsigned ret;
    ret = get_sa_item(delcode_head, n);
    if (ret == DELCODEDEFAULT) {
        delcodeval d;
        d.class_value = 0;
        d.origin_value = tex_mathcode;
        d.small_family_value = -1;
        d.small_character_value = 0;
        d.large_family_value = 0;
        d.large_character_value = 0;
        return d;
    } else {
        return delcode_heap[ret];
    }
}

@ this really only works for old-style delcodes!

@c
int get_del_code_num(int n)
{
    unsigned ret;
    ret = get_sa_item(delcode_head, n);
    if (ret == DELCODEDEFAULT) {
        return -1;
    } else {
        delcodeval d = delcode_heap[ret];
        if (d.origin_value == tex_mathcode) {
            return ((d.small_family_value * 256 +
                     d.small_character_value) * 4096 +
                    (d.large_family_value * 256) + d.large_character_value);
        } else {
            return -1;
        }
    }
}

@ @c
static void initializedelcode(void)
{
    delcode_head = new_sa_tree(DELCODESTACK, DELCODEDEFAULT);
    delcode_heap = Mxmalloc_array(delcodeval, DELCODEHEAP);
}

@ @c
static void dumpdelcode(void)
{
    int k;
    delcodeval d;
    dump_sa_tree(delcode_head);
    dump_int(delcode_heapsize);
    dump_int(delcode_heapptr);
    for (k = 0; k < delcode_heapptr; k++) {
        d = delcode_heap[k];
        dump_int(d.origin_value);
        dump_int(d.class_value);
        dump_int(d.small_family_value);
        dump_int(d.small_character_value);
        dump_int(d.large_family_value);
        dump_int(d.large_character_value);
    }
}

static void undumpdelcode(void)
{
    int k;
    delcodeval d;
    delcode_head = undump_sa_tree();
    undump_int(delcode_heapsize);
    undump_int(delcode_heapptr);
    delcode_heap = Mxmalloc_array(delcodeval, delcode_heapsize);
    for (k = 0; k < delcode_heapptr; k++) {
        undump_int(d.origin_value);
        undump_int(d.class_value);
        undump_int(d.small_family_value);
        undump_int(d.small_character_value);
        undump_int(d.large_family_value);
        undump_int(d.large_character_value);
        delcode_heap[k] = d;
    }
    d.origin_value = tex_mathcode;
    d.class_value = 0;
    d.small_family_value = -1;
    d.small_character_value = 0;
    d.large_family_value = 0;
    d.large_character_value = 0;
    for (k = delcode_heapptr; k < delcode_heapsize; k++) {
        delcode_heap[k] = d;
    }
}

@ @c
void unsave_math_codes(quarterword grouplevel)
{
    unsavemathcode(grouplevel);
    unsavedelcode(grouplevel);
}

@ @c
void initialize_math_codes(void)
{
    initializemathcode();
    initializedelcode();
}

@ @c
void free_math_codes(void)
{
    destroy_sa_tree(mathcode_head);
    xfree(mathcode_heap);
    destroy_sa_tree(delcode_head);
    xfree(delcode_heap);
}

@ @c
void dump_math_codes(void)
{
    dumpmathcode();
    dumpdelcode();
}

void undump_math_codes(void)
{
    undumpmathcode();
    undumpdelcode();
}
