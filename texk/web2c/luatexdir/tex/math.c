/* math.c

   Copyright 2008-2009 Taco Hoekwater <taco@luatex.org>

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

#include "luatex-api.h"
#include <ptexlib.h>

#include "nodes.h"
#include "commands.h"
#include "managed-sa.h"

#include "tokens.h"

static const char _svn_version[] =
    "$Id: math.c 2099 2009-03-24 10:35:01Z taco $ $URL: http://scm.foundry.supelec.fr/svn/luatex/trunk/src/texk/web2c/luatexdir/tex/math.c $";

#define mode          cur_list.mode_field
#define head          cur_list.head_field
#define tail          cur_list.tail_field
#define prev_graf     cur_list.pg_field
#define eTeX_aux      cur_list.eTeX_aux_field
#define delim_ptr     eTeX_aux
#define aux           cur_list.aux_field
#define prev_depth    aux.cint
#define space_factor  aux.hh.lhfield
#define incompleat_noad aux.cint

#define stretching 1
#define shrinking 2

#define append_char(A) str_pool[pool_ptr++]=(A)
#define flush_char pool_ptr--   /* forget the last character in the pool */
#define cur_length (pool_ptr - str_start_macro(str_ptr))
#define saved(A) save_stack[save_ptr+(A)].cint

#define vmode 1
#define hmode (vmode+max_command_cmd+1)
#define mmode (hmode+max_command_cmd+1)

#define cur_fam int_par(param_cur_fam_code)
#define text_direction zeqtb[param_text_direction_code].cint

#define scan_normal_dimen() scan_dimen(false,false,false)

#define var_code 7

extern void rawset_sa_item(sa_tree hed, integer n, integer v);

/* TODO: not sure if this is the right order */
#define back_error(A,B) do {                    \
    OK_to_interrupt=false;                      \
    back_input();                               \
    OK_to_interrupt=true;                       \
    tex_error(A,B);                             \
  } while (0)

int scan_math(pointer);
pointer fin_mlist(pointer);

#define pre_display_size dimen_par(param_pre_display_size_code)
#define hsize          dimen_par(param_hsize_code)
#define display_width  dimen_par(param_display_width_code)
#define display_indent dimen_par(param_display_indent_code)
#define math_surround  dimen_par(param_math_surround_code)
#define hang_indent    dimen_par(param_hang_indent_code)
#define hang_after     int_par(param_hang_after_code)
#define every_math     loc_par(param_every_math_code)
#define every_display  loc_par(param_every_display_code)
#define par_shape_ptr  loc_par(param_par_shape_code)

/*
When \TeX\ reads a formula that is enclosed between \.\$'s, it constructs an
{\sl mlist}, which is essentially a tree structure representing that
formula.  An mlist is a linear sequence of items, but we can regard it as
a tree structure because mlists can appear within mlists. For example, many
of the entries can be subscripted or superscripted, and such ``scripts''
are mlists in their own right.

An entire formula is parsed into such a tree before any of the actual
typesetting is done, because the current style of type is usually not
known until the formula has been fully scanned. For example, when the
formula `\.{\$a+b \\over c+d\$}' is being read, there is no way to tell
that `\.{a+b}' will be in script size until `\.{\\over}' has appeared.

During the scanning process, each element of the mlist being built is
classified as a relation, a binary operator, an open parenthesis, etc.,
or as a construct like `\.{\\sqrt}' that must be built up. This classification
appears in the mlist data structure.

After a formula has been fully scanned, the mlist is converted to an hlist
so that it can be incorporated into the surrounding text. This conversion is
controlled by a recursive procedure that decides all of the appropriate
styles by a ``top-down'' process starting at the outermost level and working
in towards the subformulas. The formula is ultimately pasted together using
combinations of horizontal and vertical boxes, with glue and penalty nodes
inserted as necessary.

An mlist is represented internally as a linked list consisting chiefly
of ``noads'' (pronounced ``no-adds''), to distinguish them from the somewhat
similar ``nodes'' in hlists and vlists. Certain kinds of ordinary nodes are
allowed to appear in mlists together with the noads; \TeX\ tells the difference
by means of the |type| field, since a noad's |type| is always greater than
that of a node. An mlist does not contain character nodes, hlist nodes, vlist
nodes, math nodes or unset nodes; in particular, each mlist item appears in the
variable-size part of |mem|, so the |type| field is always present.

@ Each noad is five or more words long. The first word contains the
|type| and |subtype| and |link| fields that are already so familiar to
us; the second contains the attribute list pointer, and the third,
fourth an fifth words are called the noad's |nucleus|, |subscr|, and
|supscr| fields. (This use of a combined attribute list is temporary. 
Eventually, each of fields need their own list)

Consider, for example, the simple formula `\.{\$x\^2\$}', which would be
parsed into an mlist containing a single element called an |ord_noad|.
The |nucleus| of this noad is a representation of `\.x', the |subscr| is
empty, and the |supscr| is a representation of `\.2'.

The |nucleus|, |subscr|, and |supscr| fields are further broken into
subfields. If |p| points to a noad, and if |q| is one of its principal
fields (e.g., |q=subscr(p)|), |q=null| indicates a field with no value (the
corresponding attribute of noad |p| is not present). Otherwise, there are 
several possibilities for the subfields, depending on the |type| of |q|.

\yskip\hang|type(q)=math_char_node| means that |math_fam(q)| refers to one of
the sixteen font families, and |character(q)| is the number of a character
within a font of that family, as in a character node.

\yskip\hang|type(q)=math_text_char_node| is similar, but the character is
unsubscripted and unsuperscripted and it is followed immediately by another
character from the same font. (This |type| setting appears only
briefly during the processing; it is used to suppress unwanted italic
corrections.)

\yskip\hang|type(q)=sub_box_node| means that |math_list(q)| points to a box
node (either an |hlist_node| or a |vlist_node|) that should be used as the
value of the field.  The |shift_amount| in the subsidiary box node is the
amount by which that box will be shifted downward.

\yskip\hang|type(q)=sub_mlist_node| means that |math_list(q)| points to
an mlist; the mlist must be converted to an hlist in order to obtain
the value of this field.

\yskip\noindent In the latter case, we might have |math_list(q)=null|. This
is not the same as |q=null|; for example, `\.{\$P\_\{\}\$}'
and `\.{\$P\$}' produce different results (the former will not have the
``italic correction'' added to the width of |P|, but the ``script skip''
will be added).

*/


void unsave_math(void)
{
    unsave();
    decr(save_ptr);
    flush_node_list(text_dir_ptr);
    text_dir_ptr = saved(0);
}


/*
Sometimes it is necessary to destroy an mlist. The following
subroutine empties the current list, assuming that |abs(mode)=mmode|.
*/

void flush_math(void)
{
    flush_node_list(vlink(head));
    flush_node_list(incompleat_noad);
    vlink(head) = null;
    tail = head;
    incompleat_noad = null;
}

/* Before we can do anything in math mode, we need fonts. */

#define MATHFONTSTACK  8
#define MATHFONTDEFAULT 0       /* == nullfont */

static sa_tree math_fam_head = NULL;

integer fam_fnt(integer fam_id, integer size_id)
{
    integer n = fam_id + (256 * size_id);
    return (integer) get_sa_item(math_fam_head, n);
}

void def_fam_fnt(integer fam_id, integer size_id, integer f, integer lvl)
{
    integer n = fam_id + (256 * size_id);
    set_sa_item(math_fam_head, n, f, lvl);
    fixup_math_parameters(fam_id, size_id, f, lvl);
    if (int_par(param_tracing_assigns_code) > 0) {
        begin_diagnostic();
        tprint("{assigning");
        print_char(' ');
        print_size(size_id);
        print_int(fam_id);
        print_char('=');
        print_font_identifier(fam_fnt(fam_id, size_id));
        print_char('}');
        end_diagnostic(false);
    }
}

void unsave_math_fam_data(integer gl)
{
    sa_stack_item st;
    if (math_fam_head->stack == NULL)
        return;
    while (math_fam_head->stack_ptr > 0 &&
           abs(math_fam_head->stack[math_fam_head->stack_ptr].level)
           >= (integer) gl) {
        st = math_fam_head->stack[math_fam_head->stack_ptr];
        if (st.level > 0) {
            rawset_sa_item(math_fam_head, st.code, st.value);
            /* now do a trace message, if requested */
            if (int_par(param_tracing_restores_code) > 0) {
                int size_id = st.code / 256;
                int fam_id = st.code % 256;
                begin_diagnostic();
                tprint("{restoring");
                print_char(' ');
                print_size(size_id);
                print_int(fam_id);
                print_char('=');
                print_font_identifier(fam_fnt(fam_id, size_id));
                print_char('}');
                end_diagnostic(false);
            }
        }
        (math_fam_head->stack_ptr)--;
    }
}



/* and parameters */

static char *math_param_names[] = {
    "Umathquad",
    "Umathaxis",
    "Umathoperatorsize",
    "Umathoverbarkern",
    "Umathoverbarrule",
    "Umathoverbarvgap",
    "Umathunderbarkern",
    "Umathunderbarrule",
    "Umathunderbarvgap",
    "Umathradicalkern",
    "Umathradicalrule",
    "Umathradicalvgap",
    "Umathradicaldegreebefore",
    "Umathradicaldegreeafter",
    "Umathradicaldegreeraise",
    "Umathstackvgap",
    "Umathstacknumup",
    "Umathstackdenomdown",
    "Umathfractionrule",
    "Umathfractionnumvgap",
    "Umathfractionnumup",
    "Umathfractiondenomvgap",
    "Umathfractiondenomdown",
    "Umathfractiondelsize",
    "Umathlimitabovevgap",
    "Umathlimitabovebgap",
    "Umathlimitabovekern",
    "Umathlimitdownvgap",
    "Umathlimitdownbgap",
    "Umathlimitdownkern",
    "Umathunderdelimitervgap",
    "Umathunderdelimiterbgap",
    "Umathoverdelimitervgap",
    "Umathoverdelimiterbgap",
    "Umathsubshiftdrop",
    "Umathsupshiftdrop",
    "Umathsubshiftdown",
    "Umathsubsupshiftdown",
    "Umathsubtopmax",
    "Umathsupshiftup",
    "Umathsupbottommin",
    "Umathsupsubbottommax",
    "Umathsubsupvgap",
    "Umathspaceafterscript",
    "Umathconnectoroverlapmin",
    "Umathordordspacing",
    "Umathordopspacing",
    "Umathordbinspacing",
    "Umathordrelspacing",
    "Umathordopenspacing",
    "Umathordclosespacing",
    "Umathordpunctspacing",
    "Umathordinnerspacing",
    "Umathopordspacing",
    "Umathopopspacing",
    "Umathopbinspacing",
    "Umathoprelspacing",
    "Umathopopenspacing",
    "Umathopclosespacing",
    "Umathoppunctspacing",
    "Umathopinnerspacing",
    "Umathbinordspacing",
    "Umathbinopspacing",
    "Umathbinbinspacing",
    "Umathbinrelspacing",
    "Umathbinopenspacing",
    "Umathbinclosespacing",
    "Umathbinpunctspacing",
    "Umathbininnerspacing",
    "Umathrelordspacing",
    "Umathrelopspacing",
    "Umathrelbinspacing",
    "Umathrelrelspacing",
    "Umathrelopenspacing",
    "Umathrelclosespacing",
    "Umathrelpunctspacing",
    "Umathrelinnerspacing",
    "Umathopenordspacing",
    "Umathopenopspacing",
    "Umathopenbinspacing",
    "Umathopenrelspacing",
    "Umathopenopenspacing",
    "Umathopenclosespacing",
    "Umathopenpunctspacing",
    "Umathopeninnerspacing",
    "Umathcloseordspacing",
    "Umathcloseopspacing",
    "Umathclosebinspacing",
    "Umathcloserelspacing",
    "Umathcloseopenspacing",
    "Umathcloseclosespacing",
    "Umathclosepunctspacing",
    "Umathcloseinnerspacing",
    "Umathpunctordspacing",
    "Umathpunctopspacing",
    "Umathpunctbinspacing",
    "Umathpunctrelspacing",
    "Umathpunctopenspacing",
    "Umathpunctclosespacing",
    "Umathpunctpunctspacing",
    "Umathpunctinnerspacing",
    "Umathinnerordspacing",
    "Umathinneropspacing",
    "Umathinnerbinspacing",
    "Umathinnerrelspacing",
    "Umathinneropenspacing",
    "Umathinnerclosespacing",
    "Umathinnerpunctspacing",
    "Umathinnerinnerspacing",
    NULL
};

#define MATHPARAMSTACK  8
#define MATHPARAMDEFAULT undefined_math_parameter

static sa_tree math_param_head = NULL;

void print_math_param(int param_code)
{
    if (param_code >= 0 && param_code < math_param_last)
        tprint_esc(math_param_names[param_code]);
    else
        tprint("Unknown math parameter code!");
}

void def_math_param(int param_id, int style_id, scaled value, int lvl)
{
    integer n = param_id + (256 * style_id);
    set_sa_item(math_param_head, n, value, lvl);
    if (int_par(param_tracing_assigns_code) > 0) {
        begin_diagnostic();
        tprint("{assigning");
        print_char(' ');
        print_math_param(param_id);
        print_style(style_id);
        print_char('=');
        print_int(value);
        print_char('}');
        end_diagnostic(false);
    }
}

scaled get_math_param(int param_id, int style_id)
{
    integer n = param_id + (256 * style_id);
    return (scaled) get_sa_item(math_param_head, n);
}


void unsave_math_param_data(integer gl)
{
    sa_stack_item st;
    if (math_param_head->stack == NULL)
        return;
    while (math_param_head->stack_ptr > 0 &&
           abs(math_param_head->stack[math_param_head->stack_ptr].level)
           >= (integer) gl) {
        st = math_param_head->stack[math_param_head->stack_ptr];
        if (st.level > 0) {
            rawset_sa_item(math_param_head, st.code, st.value);
            /* now do a trace message, if requested */
            if (int_par(param_tracing_restores_code) > 0) {
                int param_id = st.code % 256;
                int style_id = st.code / 256;
                begin_diagnostic();
                tprint("{restoring");
                print_char(' ');
                print_math_param(param_id);
                print_style(style_id);
                print_char('=');
                print_int(get_math_param(param_id, style_id));
                print_char('}');
                end_diagnostic(false);
            }
        }
        (math_param_head->stack_ptr)--;
    }
}


/* saving and unsaving of both */

void unsave_math_data(integer gl)
{
    unsave_math_fam_data(gl);
    unsave_math_param_data(gl);
}

void dump_math_data(void)
{
    if (math_fam_head == NULL)
        math_fam_head = new_sa_tree(MATHFONTSTACK, MATHFONTDEFAULT);
    dump_sa_tree(math_fam_head);
    if (math_param_head == NULL)
        math_param_head = new_sa_tree(MATHPARAMSTACK, MATHPARAMDEFAULT);
    dump_sa_tree(math_param_head);
}

void undump_math_data(void)
{
    math_fam_head = undump_sa_tree();
    math_param_head = undump_sa_tree();
}

/*  */

void initialize_math(void)
{
    if (math_fam_head == NULL)
        math_fam_head = new_sa_tree(MATHFONTSTACK, MATHFONTDEFAULT);
    if (math_param_head == NULL) {
        math_param_head = new_sa_tree(MATHPARAMSTACK, MATHPARAMDEFAULT);
        initialize_math_spacing();
    }
    return;
}


/*
@ Each portion of a formula is classified as Ord, Op, Bin, Rel, Ope,
Clo, Pun, or Inn, for purposes of spacing and line breaking. An
|ord_noad|, |op_noad|, |bin_noad|, |rel_noad|, |open_noad|, |close_noad|,
|punct_noad|, or |inner_noad| is used to represent portions of the various
types. For example, an `\.=' sign in a formula leads to the creation of a
|rel_noad| whose |nucleus| field is a representation of an equals sign
(usually |fam=0|, |character=@'75|).  A formula preceded by \.{\\mathrel}
also results in a |rel_noad|.  When a |rel_noad| is followed by an
|op_noad|, say, and possibly separated by one or more ordinary nodes (not
noads), \TeX\ will insert a penalty node (with the current |rel_penalty|)
just after the formula that corresponds to the |rel_noad|, unless there
already was a penalty immediately following; and a ``thick space'' will be
inserted just before the formula that corresponds to the |op_noad|.

A noad of type |ord_noad|, |op_noad|, \dots, |inner_noad| usually
has a |subtype=normal|. The only exception is that an |op_noad| might
have |subtype=limits| or |no_limits|, if the normal positioning of
limits has been overridden for this operator.

A |radical_noad| also has a |left_delimiter| field, which usually
represents a square root sign.

A |fraction_noad| has a |right_delimiter| field as well as a |left_delimiter|.

Delimiter fields have four subfields
called |small_fam|, |small_char|, |large_fam|, |large_char|. These subfields
represent variable-size delimiters by giving the ``small'' and ``large''
starting characters, as explained in Chapter~17 of {\sl The \TeX book}.
@:TeXbook}{\sl The \TeX book@>

A |fraction_noad| is actually quite different from all other noads. 
It has |thickness|, |denominator|, and |numerator| fields instead of 
|nucleus|, |subscr|, and |supscr|. The |thickness| is a scaled value 
that tells how thick to make a fraction rule; however, the special 
value |default_code| is used to stand for the
|default_rule_thickness| of the current size. The |numerator| and
|denominator| point to mlists that define a fraction; we always have
$$\hbox{|type(numerator)=type(denominator)=sub_mlist|}.$$ The
|left_delimiter| and |right_delimiter| fields specify delimiters that will
be placed at the left and right of the fraction. In this way, a
|fraction_noad| is able to represent all of \TeX's operators \.{\\over},
\.{\\atop}, \.{\\above}, \.{\\overwithdelims}, \.{\\atopwithdelims}, and
 \.{\\abovewithdelims}.
*/


/* The |new_noad| function creates an |ord_noad| that is completely null */

pointer new_noad(void)
{
    pointer p;
    p = new_node(ord_noad, normal);
    /* all noad fields are zero after this */
    return p;
}

pointer new_sub_box(pointer cur_box)
{
    pointer p, q;
    p = new_noad();
    q = new_node(sub_box_node, 0);
    nucleus(p) = q;
    math_list(nucleus(p)) = cur_box;
    return p;
}

/*
@ A few more kinds of noads will complete the set: An |under_noad| has its
nucleus underlined; an |over_noad| has it overlined. An |accent_noad| places
an accent over its nucleus; the accent character appears as
|math_fam(accent_chr(p))| and |math_character(accent_chr(p))|. A |vcenter_noad|
centers its nucleus vertically with respect to the axis of the formula;
in such noads we always have |type(nucleus(p))=sub_box|.

And finally, we have the |fence_noad| type, to implement
\TeX's \.{\\left} and \.{\\right} as well as \eTeX's \.{\\middle}.
The |nucleus| of such noads is
replaced by a |delimiter| field; thus, for example, `\.{\\left(}' produces
a |fence_noad| such that |delimiter(p)| holds the family and character
codes for all left parentheses. A |fence_noad| of subtype |left_noad_side| 
never appears in an mlist except as the first element, and a |fence_noad| 
with subtype |right_noad_side| never appears in an mlist
except as the last element; furthermore, we either have both a |left_noad_side|
and a |right_noad_side|, or neither one is present.
*/

/*
@ Math formulas can also contain instructions like \.{\\textstyle} that
override \TeX's normal style rules. A |style_node| is inserted into the
data structure to record such instructions; it is three words long, so it
is considered a node instead of a noad. The |subtype| is either |display_style|
or |text_style| or |script_style| or |script_script_style|. The
second and third words of a |style_node| are not used, but they are
present because a |choice_node| is converted to a |style_node|.

\TeX\ uses even numbers 0, 2, 4, 6 to encode the basic styles
|display_style|, \dots, |script_script_style|, and adds~1 to get the
``cramped'' versions of these styles. This gives a numerical order that
is backwards from the convention of Appendix~G in {\sl The \TeX book\/};
i.e., a smaller style has a larger numerical value.
@:TeXbook}{\sl The \TeX book@>
*/

const char *math_style_names[] = { "display", "crampeddisplay",
    "text", "crampedtext",
    "script", "crampedscript",
    "scriptscript", "crampedscriptscript",
    NULL
};

pointer new_style(small_number s)
{                               /* create a style node */
    return new_node(style_node, s);
}

/* Finally, the \.{\\mathchoice} primitive creates a |choice_node|, which
has special subfields |display_mlist|, |text_mlist|, |script_mlist|,
and |script_script_mlist| pointing to the mlists for each style.
*/


pointer new_choice(void)
{                               /* create a choice node */
    return new_node(choice_node, 0);    /* the |subtype| is not used */
}

/*
@ Let's consider now the previously unwritten part of |show_node_list|
that displays the things that can only be present in mlists; this
program illustrates how to access the data structures just defined.

In the context of the following program, |p| points to a node or noad that
should be displayed, and the current string contains the ``recursion history''
that leads to this point. The recursion history consists of a dot for each
outer level in which |p| is subsidiary to some node, or in which |p| is
subsidiary to the |nucleus| field of some noad; the dot is replaced by
`\.\_' or `\.\^' or `\./' or `\.\\' if |p| is descended from the |subscr|
or |supscr| or |denominator| or |numerator| fields of noads. For example,
the current string would be `\.{.\^.\_/}' if |p| points to the |ord_noad| for
|x| in the (ridiculous) formula
`\.{\$\\sqrt\{a\^\{\\mathinner\{b\_\{c\\over x+y\}\}\}\}\$}'.
*/


void display_normal_noad(pointer p);    /* forward */
void display_fence_noad(pointer p);     /* forward */
void display_fraction_noad(pointer p);  /* forward */

void show_math_node(pointer p)
{
    switch (type(p)) {
    case style_node:
        print_style(subtype(p));
        break;
    case choice_node:
        tprint_esc("mathchoice");
        append_char('D');
        show_node_list(display_mlist(p));
        flush_char;
        append_char('T');
        show_node_list(text_mlist(p));
        flush_char;
        append_char('S');
        show_node_list(script_mlist(p));
        flush_char;
        append_char('s');
        show_node_list(script_script_mlist(p));
        flush_char;
        break;
    case ord_noad:
    case op_noad:
    case bin_noad:
    case rel_noad:
    case open_noad:
    case close_noad:
    case punct_noad:
    case inner_noad:
    case radical_noad:
    case over_noad:
    case under_noad:
    case vcenter_noad:
    case accent_noad:
        display_normal_noad(p);
        break;
    case fence_noad:
        display_fence_noad(p);
        break;
    case fraction_noad:
        display_fraction_noad(p);
        break;
    default:
        tprint("Unknown node type!");
        break;
    }
}


/* Here are some simple routines used in the display of noads. */

void print_fam_and_char(pointer p)
{                               /* prints family and character */
    tprint_esc("fam");
    print_int(math_fam(p));
    print_char(' ');
    print(math_character(p));
}

void print_delimiter(pointer p)
{
    integer a;
    if (small_fam(p) < 0) {
        print_int(-1);          /* this should never happen */
    } else if (small_fam(p) < 16 && large_fam(p) < 16 &&
               small_char(p) < 256 && large_char(p) < 256) {
        /* traditional tex style */
        a = small_fam(p) * 256 + small_char(p);
        a = a * 0x1000 + large_fam(p) * 256 + large_char(p);
        print_hex(a);
    } else if ((large_fam(p) == 0 && large_char(p) == 0) ||
               small_char(p) > 65535 || large_char(p) > 65535) {
        /* modern xetex/luatex style */
        print_hex(small_fam(p));
        print_hex(small_char(p));
    } else {
        /* assume this is omega-style */
        a = small_fam(p) * 65536 + small_char(p);
        print_hex(a);
        a = large_fam(p) * 65536 + large_char(p);
        print_hex(a);
    }
}

/*
@ The next subroutine will descend to another level of recursion when a
subsidiary mlist needs to be displayed. The parameter |c| indicates what
character is to become part of the recursion history. An empty mlist is
distinguished from a missing field, because these are not equivalent 
(as explained above).
@^recursion@>
*/


void print_subsidiary_data(pointer p, ASCII_code c)
{                               /* display a noad field */
    if (cur_length >= depth_threshold) {
        if (p != null)
            tprint(" []");
    } else {
        append_char(c);         /* include |c| in the recursion history */
        if (p != null) {
            switch (type(p)) {
            case math_char_node:
                print_ln();
                print_current_string();
                print_fam_and_char(p);
                break;
            case sub_box_node:
                show_node_list(math_list(p));
                break;
            case sub_mlist_node:
                if (math_list(p) == null) {
                    print_ln();
                    print_current_string();
                    tprint("{}");
                } else {
                    show_node_list(math_list(p));
                }
                break;
            }
        }
        flush_char;             /* remove |c| from the recursion history */
    }
}

void print_style(integer c)
{
    if (c <= cramped_script_script_style) {
        tprint_esc((char *) math_style_names[c]);
        tprint("style");
    } else {
        tprint("Unknown style!");
    }
}


void display_normal_noad(pointer p)
{
    switch (type(p)) {
    case ord_noad:
        tprint_esc("mathord");
        break;
    case op_noad:
        tprint_esc("mathop");
        if (subtype(p) == limits)
            tprint_esc("limits");
        else if (subtype(p) == no_limits)
            tprint_esc("nolimits");
        break;
    case bin_noad:
        tprint_esc("mathbin");
        break;
    case rel_noad:
        tprint_esc("mathrel");
        break;
    case open_noad:
        tprint_esc("mathopen");
        break;
    case close_noad:
        tprint_esc("mathclose");
        break;
    case punct_noad:
        tprint_esc("mathpunct");
        break;
    case inner_noad:
        tprint_esc("mathinner");
        break;
    case over_noad:
        tprint_esc("overline");
        break;
    case under_noad:
        tprint_esc("underline");
        break;
    case vcenter_noad:
        tprint_esc("vcenter");
        break;
    case radical_noad:
        if (subtype(p) == 7)
            tprint_esc("Udelimiterover");
        else if (subtype(p) == 6)
            tprint_esc("Udelimiterunder");
        else if (subtype(p) == 5)
            tprint_esc("Uoverdelimiter");
        else if (subtype(p) == 4)
            tprint_esc("Uunderdelimiter");
        else if (subtype(p) == 3)
            tprint_esc("Uroot");
        else
            tprint_esc("radical");
        print_delimiter(left_delimiter(p));
        if (degree(p) != null) {
            print_subsidiary_data(degree(p), '/');
        }
        break;
    case accent_noad:
        if (accent_chr(p) != null) {
            if (bot_accent_chr(p) != null) {
                tprint_esc("Umathaccents");
                print_fam_and_char(accent_chr(p));
                print_fam_and_char(bot_accent_chr(p));
            } else {
                tprint_esc("accent");
                print_fam_and_char(accent_chr(p));
            }
        } else {
            tprint_esc("Umathbotaccent");
            print_fam_and_char(bot_accent_chr(p));
        }
        break;
    }
    print_subsidiary_data(nucleus(p), '.');
    print_subsidiary_data(supscr(p), '^');
    print_subsidiary_data(subscr(p), '_');
}

void display_fence_noad(pointer p)
{
    if (subtype(p) == right_noad_side)
        tprint_esc("right");
    else if (subtype(p) == left_noad_side)
        tprint_esc("left");
    else
        tprint_esc("middle");
    print_delimiter(delimiter(p));
}

void display_fraction_noad(pointer p)
{
    tprint_esc("fraction, thickness ");
    if (thickness(p) == default_code)
        tprint("= default");
    else
        print_scaled(thickness(p));
    if ((left_delimiter(p) != null) &&
        ((small_fam(left_delimiter(p)) != 0) ||
         (small_char(left_delimiter(p)) != 0) ||
         (large_fam(left_delimiter(p)) != 0) ||
         (large_char(left_delimiter(p)) != 0))) {
        tprint(", left-delimiter ");
        print_delimiter(left_delimiter(p));
    }
    if ((right_delimiter(p) != null) &&
        ((small_fam(right_delimiter(p)) != 0) ||
         (small_char(right_delimiter(p)) != 0) ||
         (large_fam(right_delimiter(p)) != 0) ||
         (large_char(right_delimiter(p)) != 0))) {
        tprint(", right-delimiter ");
        print_delimiter(right_delimiter(p));
    }
    print_subsidiary_data(numerator(p), '\\');
    print_subsidiary_data(denominator(p), '/');
}

/* This function is for |print_cmd_chr| only */

void print_math_comp(halfword chr_code)
{
    switch (chr_code) {
    case ord_noad:
        tprint_esc("mathord");
        break;
    case op_noad:
        tprint_esc("mathop");
        break;
    case bin_noad:
        tprint_esc("mathbin");
        break;
    case rel_noad:
        tprint_esc("mathrel");
        break;
    case open_noad:
        tprint_esc("mathopen");
        break;
    case close_noad:
        tprint_esc("mathclose");
        break;
    case punct_noad:
        tprint_esc("mathpunct");
        break;
    case inner_noad:
        tprint_esc("mathinner");
        break;
    case under_noad:
        tprint_esc("underline");
        break;
    default:
        tprint_esc("overline");
        break;
    }
}

void print_limit_switch(halfword chr_code)
{
    if (chr_code == limits)
        tprint_esc("limits");
    else if (chr_code == no_limits)
        tprint_esc("nolimits");
    else
        tprint_esc("displaylimits");
}


/*
The routines that \TeX\ uses to create mlists are similar to those we have
just seen for the generation of hlists and vlists. But it is necessary to
make ``noads'' as well as nodes, so the reader should review the
discussion of math mode data structures before trying to make sense out of
the following program.

Here is a little routine that needs to be done whenever a subformula
is about to be processed. The parameter is a code like |math_group|.
*/

void new_save_level_math(group_code c)
{
    saved(0) = text_dir_ptr;
    text_dir_ptr = new_dir(math_direction);
    incr(save_ptr);
    new_save_level(c);
    eq_word_define(static_int_base + param_body_direction_code, math_direction);
    eq_word_define(static_int_base + param_par_direction_code, math_direction);
    eq_word_define(static_int_base + param_text_direction_code, math_direction);
    eq_word_define(static_int_base + param_level_local_dir_code, cur_level);
}

void push_math(group_code c)
{
    if (math_direction != text_direction)
        dir_math_save = true;
    push_nest();
    mode = -mmode;
    incompleat_noad = null;
    new_save_level_math(c);
}

void enter_ordinary_math(void)
{
    push_math(math_shift_group);
    eq_word_define(static_int_base + param_cur_fam_code, -1);
    if (every_math != null)
        begin_token_list(every_math, every_math_text);
}

void enter_display_math(void);

/* We get into math mode from horizontal mode when a `\.\$' (i.e., a
|math_shift| character) is scanned. We must check to see whether this
`\.\$' is immediately followed by another, in case display math mode is
called for.
*/

void init_math(void)
{
    get_token();                /* |get_x_token| would fail on \.{\\ifmmode}\thinspace! */
    if ((cur_cmd == math_shift_cmd) && (mode > 0)) {
        enter_display_math();
    } else {
        back_input();
        enter_ordinary_math();
    }
}

/*
We get into ordinary math mode from display math mode when `\.{\\eqno}' or
`\.{\\leqno}' appears. In such cases |cur_chr| will be 0 or~1, respectively;
the value of |cur_chr| is placed onto |save_stack| for safe keeping.
*/


/*
When \TeX\ is in display math mode, |cur_group=math_shift_group|,
so it is not necessary for the |start_eq_no| procedure to test for
this condition.
*/

void start_eq_no(void)
{
    saved(0) = cur_chr;
    incr(save_ptr);
    enter_ordinary_math();
}

/*
Subformulas of math formulas cause a new level of math mode to be entered,
on the semantic nest as well as the save stack. These subformulas arise in
several ways: (1)~A left brace by itself indicates the beginning of a
subformula that will be put into a box, thereby freezing its glue and
preventing line breaks. (2)~A subscript or superscript is treated as a
subformula if it is not a single character; the same applies to
the nucleus of things like \.{\\underline}. (3)~The \.{\\left} primitive
initiates a subformula that will be terminated by a matching \.{\\right}.
The group codes placed on |save_stack| in these three cases are
|math_group|, |math_group|, and |math_left_group|, respectively.

Here is the code that handles case (1); the other cases are not quite as
trivial, so we shall consider them later.
*/

void math_left_brace(void)
{
    pointer q;
    tail_append(new_noad());
    q = new_node(math_char_node, 0);
    nucleus(tail) = q;
    back_input();
    (void) scan_math(nucleus(tail));
}

/*
When we enter display math mode, we need to call |line_break| to
process the partial paragraph that has just been interrupted by the
display. Then we can set the proper values of |display_width| and
|display_indent| and |pre_display_size|.
*/


void enter_display_math(void)
{
    scaled w;                   /* new or partial |pre_display_size| */
    scaled l;                   /* new |display_width| */
    scaled s;                   /* new |display_indent| */
    pointer p;
    integer n;                  /* scope of paragraph shape specification */
    if (head == tail) {         /* `\.{\\noindent\$\$}' or `\.{\$\${ }\$\$}' */
        pop_nest();
        w = -max_dimen;
    } else {
        line_break(true);
        w = actual_box_width(just_box, (2 * quad(get_cur_font())));
    }
    /* now we are in vertical mode, working on the list that will contain the display */
    /* A displayed equation is considered to be three lines long, so we
       calculate the length and offset of line number |prev_graf+2|. */
    if (par_shape_ptr == null) {
        if ((hang_indent != 0) &&
            (((hang_after >= 0) && (prev_graf + 2 > hang_after)) ||
             (prev_graf + 1 < -hang_after))) {
            l = hsize - abs(hang_indent);
            if (hang_indent > 0)
                s = hang_indent;
            else
                s = 0;
        } else {
            l = hsize;
            s = 0;
        }
    } else {
        n = vinfo(par_shape_ptr + 1);
        if (prev_graf + 2 >= n)
            p = par_shape_ptr + 2 * n + 1;
        else
            p = par_shape_ptr + 2 * (prev_graf + 2) + 1;
        s = varmem[(p - 1)].cint;
        l = varmem[p].cint;
    }

    push_math(math_shift_group);
    mode = mmode;
    eq_word_define(static_int_base + param_cur_fam_code, -1);
    eq_word_define(static_dimen_base + param_pre_display_size_code, w);
    eq_word_define(static_dimen_base + param_display_width_code, l);
    eq_word_define(static_dimen_base + param_display_indent_code, s);
    if (every_display != null)
        begin_token_list(every_display, every_display_text);
    if (nest_ptr == 1) {
        if (!output_active)
            lua_node_filter_s(buildpage_filter_callback, "before_display");
        build_page();
    }
}

/* The next routine parses all variations of a delimiter code. The |extcode|
 * tells what syntax form to use (\TeX, \Aleph, \XeTeX, \XeTeXnum, ...) , the
 * |doclass| tells whether or not read a math class also (for \delimiter c.s.).
 * (the class is passed on for conversion to |\mathchar|).
 */

#define fam_in_range ((cur_fam>=0)&&(cur_fam<256))

delcodeval do_scan_extdef_del_code(int extcode, boolean doclass)
{
    char *hlp[] = {
        "I'm going to use 0 instead of that illegal code value.",
        NULL
    };
    delcodeval d;
    integer cur_val1;           /* and the global |cur_val| */
    integer mcls, msfam = 0, mschr = 0, mlfam = 0, mlchr = 0;
    mcls = 0;
    if (extcode == tex_mathcode) {      /* \delcode, this is the easiest */
        scan_int();
        /*  "MFCCFCC or "FCCFCC */
        if (doclass) {
            mcls = (cur_val / 0x1000000);
            cur_val = (cur_val & 0xFFFFFF);
        }
        if (cur_val > 0xFFFFFF) {
            tex_error("Invalid delimiter code", hlp);
            cur_val = 0;
        }
        msfam = (cur_val / 0x100000);
        mschr = (cur_val % 0x100000) / 0x1000;
        mlfam = (cur_val & 0xFFF) / 0x100;
        mlchr = (cur_val % 0x100);
    } else if (extcode == aleph_mathcode) {     /* \odelcode */
        /*  "MFFCCCC"FFCCCC or "FFCCCC"FFCCCC */
        scan_int();
        if (doclass) {
            mcls = (cur_val / 0x1000000);
            cur_val = (cur_val & 0xFFFFFF);
        }
        cur_val1 = cur_val;
        scan_int();
        if ((cur_val1 > 0xFFFFFF) || (cur_val > 0xFFFFFF)) {
            tex_error("Invalid delimiter code", hlp);
            cur_val1 = 0;
            cur_val = 0;
        }
        msfam = (cur_val1 / 0x10000);
        mschr = (cur_val1 % 0x10000);
        mlfam = (cur_val / 0x10000);
        mlchr = (cur_val % 0x10000);
    } else if (extcode == xetex_mathcode) {     /* \Udelcode */
        /* <0-7>,<0-0xFF>,<0-0x10FFFF>  or <0-0xFF>,<0-0x10FFFF> */
        if (doclass) {
            scan_int();
            mcls = cur_val;
        }
        scan_int();
        msfam = cur_val;
        scan_char_num();
        mschr = cur_val;
        if (msfam < 0 || msfam > 255) {
            tex_error("Invalid delimiter code", hlp);
            msfam = 0;
            mschr = 0;
        }
        mlfam = 0;
        mlchr = 0;
    } else if (extcode == xetexnum_mathcode) {  /* \Udelcodenum */
        /* "FF<21bits> */
        /* the largest numeric value is $2^29-1$, but 
           the top of bit 21 can't be used as it contains invalid USV's
         */
        if (doclass) {          /* such a primitive doesn't exist */
            tconfusion("xetexnum_mathcode");
        }
        scan_int();
        msfam = (cur_val / 0x200000);
        mschr = cur_val & 0x1FFFFF;
        if (msfam < 0 || msfam > 255 || mschr > 0x10FFFF) {
            tex_error("Invalid delimiter code", hlp);
            msfam = 0;
            mschr = 0;
        }
        mlfam = 0;
        mlchr = 0;
    } else {
        /* something's gone wrong */
        tconfusion("unknown_extcode");
    }
    d.origin_value = extcode;
    d.class_value = mcls;
    d.small_family_value = msfam;
    d.small_character_value = mschr;
    d.large_family_value = mlfam;
    d.large_character_value = mlchr;
    return d;
}

void scan_extdef_del_code(int level, int extcode)
{
    delcodeval d;
    integer p;
    scan_char_num();
    p = cur_val;
    scan_optional_equals();
    d = do_scan_extdef_del_code(extcode, false);
    set_del_code(p, extcode, d.small_family_value, d.small_character_value,
                 d.large_family_value, d.large_character_value, level);
}

mathcodeval scan_mathchar(int extcode)
{
    char *hlp[] = {
        "I'm going to use 0 instead of that illegal code value.",
        NULL
    };
    mathcodeval d;
    integer mcls = 0, mfam = 0, mchr = 0;
    if (extcode == tex_mathcode) {      /* \mathcode */
        /* "TFCC */
        scan_int();
        if (cur_val > 0x8000) {
            tex_error("Invalid math code", hlp);
            cur_val = 0;
        }
        mcls = (cur_val / 0x1000);
        mfam = ((cur_val % 0x1000) / 0x100);
        mchr = (cur_val % 0x100);
    } else if (extcode == aleph_mathcode) {     /* \omathcode */
        /* "TFFCCCC */
        scan_int();
        if (cur_val > 0x8000000) {
            tex_error("Invalid math code", hlp);
            cur_val = 0;
        }
        mcls = (cur_val / 0x1000000);
        mfam = ((cur_val % 0x1000000) / 0x10000);
        mchr = (cur_val % 0x10000);
    } else if (extcode == xetex_mathcode) {
        /* <0-0x7> <0-0xFF> <0-0x10FFFF> */
        scan_int();
        mcls = cur_val;
        scan_int();
        mfam = cur_val;
        scan_char_num();
        mchr = cur_val;
        if (mcls < 0 || mcls > 7 || mfam > 255) {
            tex_error("Invalid math code", hlp);
            mchr = 0;
            mfam = 0;
            mcls = 0;
        }
    } else if (extcode == xetexnum_mathcode) {
        /* "FFT<21bits> */
        /* the largest numeric value is $2^32-1$, but 
           the top of bit 21 can't be used as it contains invalid USV's
         */
        /* Note: |scan_int| won't accept families 128-255 because these use bit 32 */
        scan_int();
        mfam = (cur_val / 0x200000) & 0x7FF;
        mcls = mfam % 0x08;
        mfam = mfam / 0x08;
        mchr = cur_val & 0x1FFFFF;
        if (mchr > 0x10FFFF) {
            tex_error("Invalid math code", hlp);
            mcls = 0;
            mfam = 0;
            mchr = 0;
        }
    } else {
        /* something's gone wrong */
        tconfusion("unknown_extcode");
    }
    d.class_value = mcls;
    d.family_value = mfam;
    d.origin_value = extcode;
    d.character_value = mchr;
    return d;
}


void scan_extdef_math_code(int level, int extcode)
{
    mathcodeval d;
    integer p;
    scan_char_num();
    p = cur_val;
    scan_optional_equals();
    d = scan_mathchar(extcode);
    set_math_code(p, extcode, d.class_value,
                  d.family_value, d.character_value, level);
}


/* this reads in a delcode when actually a mathcode is needed */

mathcodeval scan_delimiter_as_mathchar(int extcode)
{
    delcodeval dval;
    mathcodeval mval;
    dval = do_scan_extdef_del_code(extcode, true);
    mval.origin_value = 0;
    mval.class_value = dval.class_value;
    mval.family_value = dval.small_family_value;
    mval.character_value = dval.small_character_value;
    return mval;
}

/* this has to match the inverse routine in the pascal code
 * where the |\Umathchardef| is executed 
 */

mathcodeval mathchar_from_integer(integer value, int extcode)
{
    mathcodeval mval;
    mval.origin_value = extcode;
    if (extcode == tex_mathcode) {
        mval.class_value = (value / 0x1000);
        mval.family_value = ((value % 0x1000) / 0x100);
        mval.character_value = (value % 0x100);
    } else if (extcode == aleph_mathcode) {
        mval.class_value = (value / 0x1000000);
        mval.family_value = ((value % 0x1000000) / 0x10000);
        mval.character_value = (value % 0x10000);
    } else {                    /* some xetexended xetex thing */
        int mfam = (value / 0x200000) & 0x7FF;
        mval.class_value = mfam % 0x08;
        mval.family_value = mfam / 0x08;
        mval.character_value = value & 0x1FFFFF;
    }
    return mval;
}

/* Recall that the |nucleus|, |subscr|, and |supscr| fields in a noad
are broken down into subfields called |type| and either |math_list| or
|(math_fam,math_character)|. The job of |scan_math| is to figure out
what to place in one of these principal fields; it looks at the
subformula that comes next in the input, and places an encoding of
that subformula into a given word of |mem|.
*/


#define get_next_nb_nr() do { get_x_token(); } while (cur_cmd==spacer_cmd||cur_cmd==relax_cmd)


int scan_math(pointer p)
{
    /* label restart,reswitch,exit; */
    mathcodeval mval;
    assert(p != null);
  RESTART:
    get_next_nb_nr();
  RESWITCH:
    switch (cur_cmd) {
    case letter_cmd:
    case other_char_cmd:
    case char_given_cmd:
        mval = get_math_code(cur_chr);
        if (mval.class_value == 8) {
            /* An active character that is an |outer_call| is allowed here */
            cur_cs = active_to_cs(cur_chr, true);
            cur_cmd = eq_type(cur_cs);
            cur_chr = equiv(cur_cs);
            x_token();
            back_input();
            goto RESTART;
        }
        break;
    case char_num_cmd:
        scan_char_num();
        cur_chr = cur_val;
        cur_cmd = char_given_cmd;
        goto RESWITCH;
        break;
    case math_char_num_cmd:
        if (cur_chr == 0)
            mval = scan_mathchar(tex_mathcode);
        else if (cur_chr == 1)
            mval = scan_mathchar(aleph_mathcode);
        else if (cur_chr == 2)
            mval = scan_mathchar(xetex_mathcode);
        else if (cur_chr == 3)
            mval = scan_mathchar(xetexnum_mathcode);
        else
            tconfusion("scan_math");
        break;
    case math_given_cmd:
        mval = mathchar_from_integer(cur_chr, tex_mathcode);
        break;
    case omath_given_cmd:
        mval = mathchar_from_integer(cur_chr, aleph_mathcode);
        break;
    case xmath_given_cmd:
        mval = mathchar_from_integer(cur_chr, xetex_mathcode);
        break;
    case delim_num_cmd:
        if (cur_chr == 0)
            mval = scan_delimiter_as_mathchar(tex_mathcode);
        else if (cur_chr == 1)
            mval = scan_delimiter_as_mathchar(aleph_mathcode);
        else if (cur_chr == 2)
            mval = scan_delimiter_as_mathchar(xetex_mathcode);
        else
            tconfusion("scan_math");
        break;
    default:
        /* The pointer |p| is placed on |save_stack| while a complex subformula
           is being scanned. */
        back_input();
        scan_left_brace();
        saved(0) = p;
        incr(save_ptr);
        push_math(math_group);
        return 1;
    }
    type(p) = math_char_node;
    math_character(p) = mval.character_value;
    if ((mval.class_value == var_code) && fam_in_range)
        math_fam(p) = cur_fam;
    else
        math_fam(p) = mval.family_value;
    return 0;
}


/*
 The |set_math_char| procedure creates a new noad appropriate to a given
math code, and appends it to the current mlist. However, if the math code
is sufficiently large, the |cur_chr| is treated as an active character and
nothing is appended.
*/

void set_math_char(mathcodeval mval)
{
    pointer p;                  /* the new noad */
    if (mval.class_value == 8) {
        /* An active character that is an |outer_call| is allowed here */
        cur_cs = active_to_cs(cur_chr, true);
        cur_cmd = eq_type(cur_cs);
        cur_chr = equiv(cur_cs);
        x_token();
        back_input();
    } else {
        pointer q;
        p = new_noad();
        q = new_node(math_char_node, 0);
        nucleus(p) = q;
        math_character(nucleus(p)) = mval.character_value;
        math_fam(nucleus(p)) = mval.family_value;
        if (mval.class_value == var_code) {
            if (fam_in_range)
                math_fam(nucleus(p)) = cur_fam;
            type(p) = ord_noad;
        } else {
            type(p) = ord_noad + mval.class_value;
        }
        vlink(tail) = p;
        tail = p;
    }
}


/*
 The |math_char_in_text| procedure creates a new node representing a math char
in text code, and appends it to the current list. However, if the math code
is sufficiently large, the |cur_chr| is treated as an active character and
nothing is appended.
*/

void math_char_in_text(mathcodeval mval)
{
    pointer p;                  /* the new node */
    if (mval.class_value == 8) {
        /* An active character that is an |outer_call| is allowed here */
        cur_cs = active_to_cs(cur_chr, true);
        cur_cmd = eq_type(cur_cs);
        cur_chr = equiv(cur_cs);
        x_token();
        back_input();
    } else {
        p = new_char(fam_fnt(mval.family_value, text_size),
                     mval.character_value);
        vlink(tail) = p;
        tail = p;
    }
}


void math_math_comp(void)
{
    pointer q;
    tail_append(new_noad());
    type(tail) = cur_chr;
    q = new_node(math_char_node, 0);
    nucleus(tail) = q;
    (void) scan_math(nucleus(tail));
}


void math_limit_switch(void)
{
    char *hlp[] = {
        "I'm ignoring this misplaced \\limits or \\nolimits command.",
        NULL
    };
    if (head != tail) {
        if (type(tail) == op_noad) {
            subtype(tail) = cur_chr;
            return;
        }
    }
    tex_error("Limit controls must follow a math operator", hlp);
}

/*
 Delimiter fields of noads are filled in by the |scan_delimiter| routine.
The first parameter of this procedure is the |mem| address where the
delimiter is to be placed; the second tells if this delimiter follows
\.{\\radical} or not.
*/


void scan_delimiter(pointer p, integer r)
{
    delcodeval dval;
    if (r == tex_mathcode) {    /* \radical */
        dval = do_scan_extdef_del_code(tex_mathcode, true);
    } else if (r == aleph_mathcode) {   /* \oradical */
        dval = do_scan_extdef_del_code(aleph_mathcode, true);
    } else if (r == xetex_mathcode) {   /* \Uradical */
        dval = do_scan_extdef_del_code(xetex_mathcode, false);
    } else if (r == no_mathcode) {
        get_next_nb_nr();
        switch (cur_cmd) {
        case letter_cmd:
        case other_char_cmd:
            dval = get_del_code(cur_chr);
            break;
        case delim_num_cmd:
            if (cur_chr == 0)   /* \delimiter */
                dval = do_scan_extdef_del_code(tex_mathcode, true);
            else if (cur_chr == 1)      /* \odelimiter */
                dval = do_scan_extdef_del_code(aleph_mathcode, true);
            else if (cur_chr == 2)      /* \Udelimiter */
                dval = do_scan_extdef_del_code(xetex_mathcode, true);
            else
                tconfusion("scan_delimiter1");
            break;
        default:
            dval.small_family_value = -1;
            break;
        }
    } else {
        tconfusion("scan_delimiter2");
    }
    if (p == null)
        return;
    if (dval.small_family_value < 0) {
        char *hlp[] = {
            "I was expecting to see something like `(' or `\\{' or",
            "`\\}' here. If you typed, e.g., `{' instead of `\\{', you",
            "should probably delete the `{' by typing `1' now, so that",
            "braces don't get unbalanced. Otherwise just proceed",
            "Acceptable delimiters are characters whose \\delcode is",
            "nonnegative, or you can use `\\delimiter <delimiter code>'.",
            NULL
        };
        back_error("Missing delimiter (. inserted)", hlp);
        small_fam(p) = 0;
        small_char(p) = 0;
        large_fam(p) = 0;
        large_char(p) = 0;
    } else {
        small_fam(p) = dval.small_family_value;
        small_char(p) = dval.small_character_value;
        large_fam(p) = dval.large_family_value;
        large_char(p) = dval.large_character_value;
    }
    return;
}


void math_radical(void)
{
    halfword q;
    int chr_code = cur_chr;
    tail_append(new_node(radical_noad, chr_code));
    q = new_node(delim_node, 0);
    left_delimiter(tail) = q;
    if (chr_code == 0)          /* \radical */
        scan_delimiter(left_delimiter(tail), tex_mathcode);
    else if (chr_code == 1)     /* \oradical */
        scan_delimiter(left_delimiter(tail), aleph_mathcode);
    else if (chr_code == 2)     /* \Uradical */
        scan_delimiter(left_delimiter(tail), xetex_mathcode);
    else if (chr_code == 3)     /* \Uroot */
        scan_delimiter(left_delimiter(tail), xetex_mathcode);
    else if (chr_code == 4)     /* \Uunderdelimiter */
        scan_delimiter(left_delimiter(tail), xetex_mathcode);
    else if (chr_code == 5)     /* \Uoverdelimiter */
        scan_delimiter(left_delimiter(tail), xetex_mathcode);
    else if (chr_code == 6)     /* \Udelimiterunder */
        scan_delimiter(left_delimiter(tail), xetex_mathcode);
    else if (chr_code == 7)     /* \Udelimiterover */
        scan_delimiter(left_delimiter(tail), xetex_mathcode);
    else
        tconfusion("math_radical");
    if (chr_code == 3) {
        q = new_node(math_char_node, 0);
        vlink(q) = tail;
        degree(tail) = q;
        if (!scan_math(degree(tail))) {
            q = new_node(math_char_node, 0);
            nucleus(tail) = q;
            (void) scan_math(nucleus(tail));
        }
    } else {
        q = new_node(math_char_node, 0);
        nucleus(tail) = q;
        (void) scan_math(nucleus(tail));
    }
}

void math_ac(void)
{
    halfword q;
    mathcodeval t, b;
    t.character_value = 0;
    t.family_value = 0;
    b.character_value = 0;
    b.family_value = 0;
    if (cur_cmd == accent_cmd) {
        char *hlp[] = {
            "I'm changing \\accent to \\mathaccent here; wish me luck.",
            "(Accents are not the same in formulas as they are in text.)",
            NULL
        };
        tex_error("Please use \\mathaccent for accents in math mode", hlp);
    }
    tail_append(new_node(accent_noad, normal));
    if (cur_chr == 0) {         /* \mathaccent */
        t = scan_mathchar(tex_mathcode);
    } else if (cur_chr == 1) {  /* \omathaccent */
        t = scan_mathchar(aleph_mathcode);
    } else if (cur_chr == 2) {  /* \Umathaccent */
        t = scan_mathchar(xetex_mathcode);
    } else if (cur_chr == 3) {  /* \Umathbotaccent */
        b = scan_mathchar(xetex_mathcode);
    } else if (cur_chr == 4) {  /* \Umathaccents */
        t = scan_mathchar(xetex_mathcode);
        b = scan_mathchar(xetex_mathcode);
    } else {
        tconfusion("math_ac");
    }
    if (!(t.character_value == 0 && t.family_value == 0)) {
        q = new_node(math_char_node, 0);
        accent_chr(tail) = q;
        math_character(accent_chr(tail)) = t.character_value;
        if ((t.class_value == var_code) && fam_in_range)
            math_fam(accent_chr(tail)) = cur_fam;
        else
            math_fam(accent_chr(tail)) = t.family_value;
    }
    if (!(b.character_value == 0 && b.family_value == 0)) {
        q = new_node(math_char_node, 0);
        bot_accent_chr(tail) = q;
        math_character(bot_accent_chr(tail)) = b.character_value;
        if ((b.class_value == var_code) && fam_in_range)
            math_fam(bot_accent_chr(tail)) = cur_fam;
        else
            math_fam(bot_accent_chr(tail)) = b.family_value;
    }
    q = new_node(math_char_node, 0);
    nucleus(tail) = q;
    (void) scan_math(nucleus(tail));
}

pointer math_vcenter_group(pointer p)
{
    pointer q, r;
    q = new_noad();
    type(q) = vcenter_noad;
    r = new_node(sub_box_node, 0);
    nucleus(q) = r;
    math_list(nucleus(q)) = p;
    return q;
}

/*
The routine that scans the four mlists of a \.{\\mathchoice} is very
much like the routine that builds discretionary nodes.
*/

void append_choices(void)
{
    tail_append(new_choice());
    incr(save_ptr);
    saved(-1) = 0;
    push_math(math_choice_group);
    scan_left_brace();
}
void build_choices(void)
{
    pointer p;                  /* the current mlist */
    unsave_math();
    p = fin_mlist(null);
    switch (saved(-1)) {
    case 0:
        display_mlist(tail) = p;
        break;
    case 1:
        text_mlist(tail) = p;
        break;
    case 2:
        script_mlist(tail) = p;
        break;
    case 3:
        script_script_mlist(tail) = p;
        decr(save_ptr);
        return;
        break;
    }                           /* there are no other cases */
    incr(saved(-1));
    push_math(math_choice_group);
    scan_left_brace();
}

/*
 Subscripts and superscripts are attached to the previous nucleus by the
 action procedure called |sub_sup|. 
*/

void sub_sup(void)
{
    pointer q;
    if (tail == head || (!scripts_allowed(tail))) {
        tail_append(new_noad());
        q = new_node(sub_mlist_node, 0);
        nucleus(tail) = q;
    }
    if (cur_cmd == sup_mark_cmd) {
        if (supscr(tail) != null) {
            char *hlp[] = {
                "I treat `x^1^2' essentially like `x^1{}^2'.", NULL
            };
            tail_append(new_noad());
            q = new_node(sub_mlist_node, 0);
            nucleus(tail) = q;
            tex_error("Double superscript", hlp);
        }
        q = new_node(math_char_node, 0);
        supscr(tail) = q;
        (void) scan_math(supscr(tail));
    } else {
        if (subscr(tail) != null) {
            char *hlp[] = {
                "I treat `x_1_2' essentially like `x_1{}_2'.", NULL
            };
            tail_append(new_noad());
            q = new_node(sub_mlist_node, 0);
            nucleus(tail) = q;
            tex_error("Double subscript", hlp);
        }
        q = new_node(math_char_node, 0);
        subscr(tail) = q;
        (void) scan_math(subscr(tail));
    }
}

/*
An operation like `\.{\\over}' causes the current mlist to go into a
state of suspended animation: |incompleat_noad| points to a |fraction_noad|
that contains the mlist-so-far as its numerator, while the denominator
is yet to come. Finally when the mlist is finished, the denominator will
go into the incompleat fraction noad, and that noad will become the
whole formula, unless it is surrounded by `\.{\\left}' and `\.{\\right}'
delimiters. 
*/

void math_fraction(void)
{
    small_number c;             /* the type of generalized fraction we are scanning */
    pointer q;
    c = cur_chr;
    if (incompleat_noad != null) {
        char *hlp[] = {
            "I'm ignoring this fraction specification, since I don't",
            "know whether a construction like `x \\over y \\over z'",
            "means `{x \\over y} \\over z' or `x \\over {y \\over z}'.",
            NULL
        };
        if (c >= delimited_code) {
            scan_delimiter(null, no_mathcode);
            scan_delimiter(null, no_mathcode);
        }
        if ((c % delimited_code) == above_code)
            scan_normal_dimen();
        tex_error("Ambiguous; you need another { and }", hlp);
    } else {
        incompleat_noad = new_node(fraction_noad, normal);
        numerator(incompleat_noad) = new_node(sub_mlist_node, 0);
        math_list(numerator(incompleat_noad)) = vlink(head);
        vlink(head) = null;
        tail = head;

        if (c >= delimited_code) {
            q = new_node(delim_node, 0);
            left_delimiter(incompleat_noad) = q;
            q = new_node(delim_node, 0);
            right_delimiter(incompleat_noad) = q;
            scan_delimiter(left_delimiter(incompleat_noad), no_mathcode);
            scan_delimiter(right_delimiter(incompleat_noad), no_mathcode);
        }
        switch (c % delimited_code) {
        case above_code:
            scan_normal_dimen();
            thickness(incompleat_noad) = cur_val;
            break;
        case over_code:
            thickness(incompleat_noad) = default_code;
            break;
        case atop_code:
            thickness(incompleat_noad) = 0;
            break;
        }                       /* there are no other cases */
    }
}



/* At the end of a math formula or subformula, the |fin_mlist| routine is
called upon to return a pointer to the newly completed mlist, and to
pop the nest back to the enclosing semantic level. The parameter to
|fin_mlist|, if not null, points to a |fence_noad| that ends the
current mlist; this |fence_noad| has not yet been appended.
*/


pointer fin_mlist(pointer p)
{
    pointer q;                  /* the mlist to return */
    if (incompleat_noad != null) {
        if (denominator(incompleat_noad) != null) {
            type(denominator(incompleat_noad)) = sub_mlist_node;
        } else {
            q = new_node(sub_mlist_node, 0);
            denominator(incompleat_noad) = q;
        }
        math_list(denominator(incompleat_noad)) = vlink(head);
        if (p == null) {
            q = incompleat_noad;
        } else {
            q = math_list(numerator(incompleat_noad));
            if ((type(q) != fence_noad) || (subtype(q) != left_noad_side)
                || (delim_ptr == null))
                tconfusion("right");    /* this can't happen */
            math_list(numerator(incompleat_noad)) = vlink(delim_ptr);
            vlink(delim_ptr) = incompleat_noad;
            vlink(incompleat_noad) = p;
        }
    } else {
        vlink(tail) = p;
        q = vlink(head);
    }
    pop_nest();
    return q;
}


/* Now at last we're ready to see what happens when a right brace occurs
in a math formula. Two special cases are simplified here: Braces are effectively
removed when they surround a single Ord without sub/superscripts, or when they
surround an accent that is the nucleus of an Ord atom.
*/

void close_math_group(pointer p)
{
    pointer q;
    unsave_math();

    decr(save_ptr);
    type(saved(0)) = sub_mlist_node;
    p = fin_mlist(null);
    math_list(saved(0)) = p;
    if (p != null) {
        if (vlink(p) == null) {
            if (type(p) == ord_noad) {
                if (subscr(p) == null && supscr(p) == null) {
                    type(saved(0)) = type(nucleus(p));
                    if (type(nucleus(p)) == math_char_node) {
                        math_fam(saved(0)) = math_fam(nucleus(p));
                        math_character(saved(0)) = math_character(nucleus(p));
                    } else {
                        math_list(saved(0)) = math_list(nucleus(p));
                        math_list(nucleus(p)) = null;
                    }
                    node_attr(saved(0)) = node_attr(nucleus(p));
                    node_attr(nucleus(p)) = null;
                    flush_node(p);
                }
            }
        } else {
            if (type(p) == accent_noad) {
                if (saved(0) == nucleus(tail)) {
                    /* todo: check this branch */
                    if (type(tail) == ord_noad) {
                        q = head;
                        while (vlink(q) != tail)
                            q = vlink(q);
                        vlink(q) = p;
                        nucleus(tail) = null;
                        subscr(tail) = null;
                        supscr(tail) = null;
                        node_attr(p) = node_attr(tail);
                        node_attr(tail) = null;
                        flush_node(tail);
                        tail = p;
                    }
                }
            }
        }
    }
    if (vlink(saved(0)) > 0) {
        pointer q;
        q = new_node(math_char_node, 0);
        nucleus(vlink(saved(0))) = q;
        vlink(saved(0)) = null;
        saved(0) = q;
        (void) scan_math(saved(0));
        /* restart */
    }
}

/*
We have dealt with all constructions of math mode except `\.{\\left}' and
`\.{\\right}', so the picture is completed by the following sections of
the program. The |middle| feature of \eTeX\ allows one ore several \.{\\middle}
delimiters to appear between \.{\\left} and \.{\\right}.
*/

void math_left_right(void)
{
    small_number t;             /* |left_noad_side| .. |right_noad_side| */
    pointer p;                  /* new noad */
    pointer q;                  /* resulting mlist */
    pointer r;                  /* temporary */
    t = cur_chr;
    if ((t != left_noad_side) && (cur_group != math_left_group)) {
        if (cur_group == math_shift_group) {
            scan_delimiter(null, no_mathcode);
            if (t == middle_noad_side) {
                char *hlp[] = {
                    "I'm ignoring a \\middle that had no matching \\left.",
                    NULL
                };
                tex_error("Extra \\middle", hlp);
            } else {
                char *hlp[] = {
                    "I'm ignoring a \\right that had no matching \\left.",
                    NULL
                };
                tex_error("Extra \\right", hlp);
            }
        } else {
            off_save();
        }
    } else {
        p = new_noad();
        type(p) = fence_noad;
        subtype(p) = t;
        r = new_node(delim_node, 0);
        delimiter(p) = r;
        scan_delimiter(delimiter(p), no_mathcode);
        if (t == left_noad_side) {
            q = p;
        } else {
            q = fin_mlist(p);
            unsave_math();
        }
        if (t != right_noad_side) {
            push_math(math_left_group);
            vlink(head) = q;
            tail = p;
            delim_ptr = p;
        } else {
            tail_append(new_noad());
            type(tail) = inner_noad;
            r = new_node(sub_mlist_node, 0);
            nucleus(tail) = r;
            math_list(nucleus(tail)) = q;
        }
    }
}


/* \TeX\ gets to the following part of the program when 
the first `\.\$' ending a display has been scanned.
*/

static void check_second_math_shift(void)
{
    get_x_token();
    if (cur_cmd != math_shift_cmd) {
        char *hlp[] = {
            "The `$' that I just saw supposedly matches a previous `$$'.",
            "So I shall assume that you typed `$$' both times.",
            NULL
        };
        back_error("Display math should end with $$", hlp);
    }
}

void finish_displayed_math(boolean l, boolean danger, pointer a);

void after_math(void)
{
    boolean danger;             /* not enough symbol fonts are present */
    integer m;                  /* |mmode| or |-mmode| */
    pointer p;                  /* the formula */
    pointer a = null;           /* box containing equation number */
    boolean l = false;          /* `\.{\\leqno}' instead of `\.{\\eqno}' */
    danger = check_necessary_fonts();
    m = mode;
    p = fin_mlist(null);        /* this pops the nest */
    if (mode == -m) {           /* end of equation number */
        check_second_math_shift();
        run_mlist_to_hlist(p, text_style, false);
        a = hpack(vlink(temp_head), 0, additional);
        unsave_math();
        decr(save_ptr);         /* now |cur_group=math_shift_group| */
        if (saved(0) == 1)
            l = true;
        danger = check_necessary_fonts();
        m = mode;
        p = fin_mlist(null);
    }
    if (m < 0) {
        /* The |unsave| is done after everything else here; hence an appearance of
           `\.{\\mathsurround}' inside of `\.{\$...\$}' affects the spacing at these
           particular \.\$'s. This is consistent with the conventions of
           `\.{\$\$...\$\$}', since `\.{\\abovedisplayskip}' inside a display affects the
           space above that display.
         */
        tail_append(new_math(math_surround, before));
        if (dir_math_save) {
            tail_append(new_dir(math_direction));
        }
        run_mlist_to_hlist(p, text_style, (mode > 0));
        vlink(tail) = vlink(temp_head);
        while (vlink(tail) != null)
            tail = vlink(tail);
        if (dir_math_save) {
            tail_append(new_dir(math_direction - 64));
        }
        dir_math_save = false;
        tail_append(new_math(math_surround, after));
        space_factor = 1000;
        unsave_math();
    } else {
        if (a == null)
            check_second_math_shift();
        run_mlist_to_hlist(p, display_style, false);
        finish_displayed_math(l, danger, a);
    }
}


void resume_after_display(void)
{
    if (cur_group != math_shift_group)
        tconfusion("display");
    unsave_math();
    prev_graf = prev_graf + 3;
    push_nest();
    mode = hmode;
    space_factor = 1000;
    tail_append(make_local_par_node());
    get_x_token();
    if (cur_cmd != spacer_cmd)
        back_input();
    if (nest_ptr == 1) {
        lua_node_filter_s(buildpage_filter_callback, "after_display");
        build_page();
    }
}


/*
We have saved the worst for last: The fussiest part of math mode processing
occurs when a displayed formula is being centered and placed with an optional
equation number.
*/
/* At this time |p| points to the mlist for the formula; |a| is either
   |null| or it points to a box containing the equation number; and we are in
   vertical mode (or internal vertical mode). 
*/

void finish_displayed_math(boolean l, boolean danger, pointer a)
{
    pointer b;                  /* box containing the equation */
    scaled w;                   /* width of the equation */
    scaled z;                   /* width of the line */
    scaled e;                   /* width of equation number */
    scaled q;                   /* width of equation number plus space to separate from equation */
    scaled d;                   /* displacement of equation in the line */
    scaled s;                   /* move the line right this much */
    small_number g1, g2;        /* glue parameter codes for before and after */
    pointer r;                  /* kern node used to position the display */
    pointer t;                  /* tail of adjustment list */
    pointer pre_t;              /* tail of pre-adjustment list */
    pointer p;
    p = vlink(temp_head);
    adjust_tail = adjust_head;
    pre_adjust_tail = pre_adjust_head;
    b = hpack(p, 0, additional);
    p = list_ptr(b);
    t = adjust_tail;
    adjust_tail = null;
    pre_t = pre_adjust_tail;
    pre_adjust_tail = null;
    w = width(b);
    z = display_width;
    s = display_indent;
    if ((a == null) || danger) {
        e = 0;
        q = 0;
    } else {
        e = width(a);
        q = e + get_math_quad(text_size);
    }
    if (w + q > z) {
        /* The user can force the equation number to go on a separate line
           by causing its width to be zero. */
        if ((e != 0) && ((w - total_shrink[normal] + q <= z) ||
                         (total_shrink[sfi] != 0) || (total_shrink[fil] != 0) ||
                         (total_shrink[fill] != 0)
                         || (total_shrink[filll] != 0))) {
            list_ptr(b) = null;
            flush_node(b);
            b = hpack(p, z - q, exactly);
        } else {
            e = 0;
            if (w > z) {
                list_ptr(b) = null;
                flush_node(b);
                b = hpack(p, z, exactly);
            }
        }
        w = width(b);
    }
    /* We try first to center the display without regard to the existence of
       the equation number. If that would make it too close (where ``too close''
       means that the space between display and equation number is less than the
       width of the equation number), we either center it in the remaining space
       or move it as far from the equation number as possible. The latter alternative
       is taken only if the display begins with glue, since we assume that the
       user put glue there to control the spacing precisely.
     */
    d = half(z - w);
    if ((e > 0) && (d < 2 * e)) {       /* too close */
        d = half(z - w - e);
        if (p != null)
            if (!is_char_node(p))
                if (type(p) == glue_node)
                    d = 0;
    }

    /* If the equation number is set on a line by itself, either before or
       after the formula, we append an infinite penalty so that no page break will
       separate the display from its number; and we use the same size and
       displacement for all three potential lines of the display, even though
       `\.{\\parshape}' may specify them differently.
     */
    tail_append(new_penalty(int_par(param_pre_display_penalty_code)));
    if ((d + s <= pre_display_size) || l) {     /* not enough clearance */
        g1 = param_above_display_skip_code;
        g2 = param_below_display_skip_code;
    } else {
        g1 = param_above_display_short_skip_code;
        g2 = param_below_display_short_skip_code;
    }
    if (l && (e == 0)) {        /* it follows that |type(a)=hlist_node| */
        shift_amount(a) = s;
        append_to_vlist(a);
        tail_append(new_penalty(inf_penalty));
    } else {
        tail_append(new_param_glue(g1));
    }

    if (e != 0) {
        r = new_kern(z - w - e - d);
        if (l) {
            vlink(a) = r;
            vlink(r) = b;
            b = a;
            d = 0;
        } else {
            vlink(b) = r;
            vlink(r) = a;
        }
        b = hpack(b, 0, additional);
    }
    shift_amount(b) = s + d;
    append_to_vlist(b);

    if ((a != null) && (e == 0) && !l) {
        tail_append(new_penalty(inf_penalty));
        shift_amount(a) = s + z - width(a);
        append_to_vlist(a);
        g2 = 0;
    }
    if (t != adjust_head) {     /* migrating material comes after equation number */
        vlink(tail) = vlink(adjust_head);
        tail = t;
    }
    if (pre_t != pre_adjust_head) {
        vlink(tail) = vlink(pre_adjust_head);
        tail = pre_t;
    }
    tail_append(new_penalty(int_par(param_post_display_penalty_code)));
    if (g2 > 0)
        tail_append(new_param_glue(g2));

    resume_after_display();
}


/*
 When \.{\\halign} appears in a display, the alignment routines operate
essentially as they do in vertical mode. Then the following program is
activated, with |p| and |q| pointing to the beginning and end of the
resulting list, and with |aux_save| holding the |prev_depth| value.
*/


void finish_display_alignment(pointer p, pointer q, memory_word aux_save)
{
    do_assignments();
    if (cur_cmd != math_shift_cmd) {
        char *hlp[] = {
            "Displays can use special alignments (like \\eqalignno)",
            "only if nothing but the alignment itself is between $$'s.",
            NULL
        };
        back_error("Missing $$ inserted", hlp);
    } else {
        check_second_math_shift();
    }
    pop_nest();
    tail_append(new_penalty(int_par(param_pre_display_penalty_code)));
    tail_append(new_param_glue(param_above_display_skip_code));
    vlink(tail) = p;
    if (p != null)
        tail = q;
    tail_append(new_penalty(int_par(param_post_display_penalty_code)));
    tail_append(new_param_glue(param_below_display_skip_code));
    prev_depth = aux_save.cint;
    resume_after_display();
}
