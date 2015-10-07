% extensions.w
%
% Copyright 2009-2010 Taco Hoekwater <taco@@luatex.org>
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

\def\eTeX{e-\TeX}
\def\pdfTeX{pdf\TeX}

@ @c


#include "ptexlib.h"

@ @c
#define mode          cur_list.mode_field
#define tail          cur_list.tail_field
#define head          cur_list.head_field
#define prev_graf     cur_list.pg_field
#define dir_save      cur_list.dirs_field

#define tracing_nesting int_par(tracing_nesting_code)
#define box(A) eqtb[box_base+(A)].hh.rh
#define global_defs int_par(global_defs_code)
#define cat_code_table int_par(cat_code_table_code)
#define par_direction int_par(par_direction_code)
#define toks(A) equiv(toks_base+(A))

#define local_inter_line_penalty int_par(local_inter_line_penalty_code)
#define local_broken_penalty int_par(local_broken_penalty_code)
#define local_left_box equiv(local_left_box_base)
#define local_right_box equiv(local_right_box_base)


@ The program above includes a bunch of ``hooks'' that allow further
capabilities to be added without upsetting \TeX's basic structure.
Most of these hooks are concerned with ``whatsit'' nodes, which are
intended to be used for special purposes; whenever a new extension to
\TeX\ involves a new kind of whatsit node, a corresponding change needs
to be made to the routines below that deal with such nodes,
but it will usually be unnecessary to make many changes to the
other parts of this program.

In order to demonstrate how extensions can be made, we shall treat
`\.{\\write}', `\.{\\openout}', `\.{\\closeout}', `\.{\\immediate}',
and `\.{\\special}' as if they were extensions.
These commands are actually primitives of \TeX, and they should
appear in all implementations of the system; but let's try to imagine
that they aren't. Then the program below illustrates how a person
could add them.

Sometimes, of course, an extension will require changes to \TeX\ itself;
no system of hooks could be complete enough for all conceivable extensions.
The features associated with `\.{\\write}' are almost all confined to the
following paragraphs, but there are small parts of the |print_ln| and
|print_char| procedures that were introduced specifically to \.{\\write}
characters. Furthermore one of the token lists recognized by the scanner
is a |write_text|; and there are a few other miscellaneous places where we
have already provided for some aspect of \.{\\write}.  The goal of a \TeX\
extender should be to minimize alterations to the standard parts of the
program, and to avoid them completely if possible. He or she should also
be quite sure that there's no easy way to accomplish the desired goals
with the standard features that \TeX\ already has. ``Think thrice before
extending,'' because that may save a lot of work, and it will also keep
incompatible extensions of \TeX\ from proliferating.
@^system dependencies@>
@^extensions to \TeX@>

First let's consider the format of whatsit nodes that are used to represent
the data associated with \.{\\write} and its relatives. Recall that a whatsit
has |type=whatsit_node|, and the |subtype| is supposed to distinguish
different kinds of whatsits. Each node occupies two or more words; the
exact number is immaterial, as long as it is readily determined from the
|subtype| or other data.

We shall introduce five |subtype| values here, corresponding to the
control sequences \.{\\openout}, \.{\\write}, \.{\\closeout}, and \.{\\special}.
The second word of I/O whatsits has a |write_stream| field
that identifies the write-stream number (0 to 15, or 16 for out-of-range and
positive, or 17 for out-of-range and negative).
In the case of \.{\\write} and \.{\\special}, there is also a field that
points to the reference count of a token list that should be sent. In the
case of \.{\\openout}, we need three words and three auxiliary subfields
to hold the string numbers for name, area, and extension.

@ Extensions might introduce new command codes; but it's best to use
|extension| with a modifier, whenever possible, so that |main_control|
stays the same.

@ The sixteen possible \.{\\write} streams are represented by the |write_file|
array. The |j|th file is open if and only if |write_open[j]=true|. The last
two streams are special; |write_open[16]| represents a stream number
greater than 15, while |write_open[17]| represents a negative stream number,
and both of these variables are always |false|.

@c
alpha_file write_file[16];
halfword write_file_mode[16];
halfword write_file_translation[16];
boolean write_open[18];
scaled neg_wd;
scaled pos_wd;
scaled neg_ht;


@ The variable |write_loc| just introduced is used to provide an
appropriate error message in case of ``runaway'' write texts.

@c
halfword write_loc;             /* |eqtb| address of \.{\\write} */


@ When an |extension| command occurs in |main_control|, in any mode,
the |do_extension| routine is called.

@c
void do_extension(PDF pdf)
{
    int i, k;                   /* all-purpose integers */
    halfword p;                 /* all-purpose pointer */
    switch (cur_chr) {
    case open_node:
        /* Implement \.{\\openout} */
        new_write_whatsit(open_node_size);
        scan_optional_equals();
        scan_file_name();
        open_name(tail) = cur_name;
        open_area(tail) = cur_area;
        open_ext(tail) = cur_ext;
        break;
    case write_node:
        /* Implement \.{\\write} */
        /* When `\.{\\write 12\{...\}}' appears, we scan the token list `\.{\{...\}}'
           without expanding its macros; the macros will be expanded later when this
           token list is rescanned. */
        k = cur_cs;
        new_write_whatsit(write_node_size);
        cur_cs = k;
        p = scan_toks(false, false);
        write_tokens(tail) = def_ref;
        break;
    case close_node:
        /* Implement \.{\\closeout} */
        new_write_whatsit(write_node_size);
        write_tokens(tail) = null;
        break;
    case special_node:
        /* Implement \.{\\special} */
        /* When `\.{\\special\{...\}}' appears, we expand the macros in the token
           list as in \.{\\xdef} and \.{\\mark}. */
        new_whatsit(special_node);
        write_stream(tail) = null;
        p = scan_toks(false, true);
        write_tokens(tail) = def_ref;
        break;
    case immediate_code:
        /* Implement \.{\\immediate} */
        /* To write a token list, we must run it through \TeX's scanner, expanding
           macros and \.{\\the} and \.{\\number}, etc. This might cause runaways,
           if a delimited macro parameter isn't matched, and runaways would be
           extremely confusing since we are calling on \TeX's scanner in the middle
           of a \.{\\shipout} command. Therefore we will put a dummy control sequence as
           a ``stopper,'' right after the token list. This control sequence is
           artificially defined to be \.{\\outer}.

           The presence of `\.{\\immediate}' causes the |do_extension| procedure
           to descend to one level of recursion. Nothing happens unless \.{\\immediate}
           is followed by `\.{\\openout}', `\.{\\write}', or `\.{\\closeout}'.
         */
        get_x_token();
        if (cur_cmd == extension_cmd) {
            if (cur_chr <= close_node) {
                p = tail;
                /* |do_extension()| and |out_what()| here can only be open, write, or close */
                do_extension(pdf);      /* append a whatsit node */
                out_what(pdf, tail);    /* do the action immediately */
                flush_node_list(tail);
                tail = p;
                vlink(p) = null;
            } else {
                switch (cur_chr) {
                case pdf_obj_code:
                    check_o_mode(pdf, "\\immediate\\pdfobj", 1 << OMODE_PDF,
                                 true);
                    do_extension(pdf);  /* scan object and set |pdf_last_obj| */
                    if (obj_data_ptr(pdf, pdf_last_obj) == 0)   /* this object has not been initialized yet */
                        pdf_error("ext1",
                                  "`\\pdfobj reserveobjnum' cannot be used with \\immediate");
                    pdf_write_obj(pdf, pdf_last_obj);
                    break;
                case pdf_xform_code:
                    check_o_mode(pdf, "\\immediate\\pdfxform", 1 << OMODE_PDF,
                                 true);
                    do_extension(pdf);  /* scan form and set |pdf_last_xform| */
                    pdf_cur_form = pdf_last_xform;
                    ship_out(pdf, obj_xform_box(pdf, pdf_last_xform), SHIPPING_FORM);
                    break;
                case pdf_ximage_code:
                    check_o_mode(pdf, "\\immediate\\pdfximage", 1 << OMODE_PDF,
                                 true);
                    do_extension(pdf);  /* scan image and set |pdf_last_ximage| */
                    pdf_write_image(pdf, pdf_last_ximage);
                    break;
                default:
                    back_input();
                    break;
                }
            }
        } else {
            back_input();
        }
        break;
    case pdf_annot_node:
        /* Implement \.{\\pdfannot} */
        check_o_mode(pdf, "\\pdfannot", 1 << OMODE_PDF, false);
        scan_annot(pdf);
        break;
    case pdf_catalog_code:
        /* Implement \.{\\pdfcatalog} */
        check_o_mode(pdf, "\\pdfcatalog", 1 << OMODE_PDF, true);        /* writes an object */
        scan_pdfcatalog(pdf);
        break;
    case pdf_dest_node:
        /* Implement \.{\\pdfdest} */
        check_o_mode(pdf, "\\pdfdest", 1 << OMODE_PDF, false);
        scan_pdfdest(pdf);
        break;
    case pdf_end_link_node:
        /* Implement \.{\\pdfendlink} */
        check_o_mode(pdf, "\\pdfendlink", 1 << OMODE_PDF, false);
        if (abs(mode) == vmode)
            pdf_error("ext1", "\\pdfendlink cannot be used in vertical mode");
        new_whatsit(pdf_end_link_node);
        break;
    case pdf_end_thread_node:
        /* Implement \.{\\pdfendthread} */
        check_o_mode(pdf, "\\pdfendthread", 1 << OMODE_PDF, false);
        new_whatsit(pdf_end_thread_node);
        break;
    case pdf_font_attr_code:
        /* Implement \.{\\pdffontattr} */
        /* A change from Thanh's original code: the font attributes are simply
           initialized to zero now, this is easier to deal with from C than an
           empty \TeX{} string, and surely nobody will want to set
           \.{\\pdffontattr} to a string containing a single zero, as that
           would be nonsensical in the PDF output. */
        check_o_mode(pdf, "\\pdffontattr", 1 << OMODE_PDF, false);
        scan_font_ident();
        k = cur_val;
        if (k == null_font)
            pdf_error("font", "invalid font identifier");
        scan_pdf_ext_toks();
        set_pdf_font_attr(k, tokens_to_string(def_ref));
        if (str_length(pdf_font_attr(k)) == 0) {
            flush_str((str_ptr - 1));   /* from |tokens_to_string| */
            set_pdf_font_attr(k, 0);
        }
        break;
    case font_expand_code:
        /* Implement \.{\\fontexpand} */
        read_expand_font();
        break;
    case pdf_include_chars_code:
        /* Implement \.{\\pdfincludechars} */
        check_o_mode(pdf, "\\pdfincludechars", 1 << OMODE_PDF, false);
        pdf_include_chars(pdf);
        break;
    case pdf_info_code:
        /* Implement \.{\\pdfinfo} */
        check_o_mode(pdf, "\\pdfinfo", 1 << OMODE_PDF, false);
        scan_pdf_ext_toks();
        pdf_info_toks = concat_tokens(pdf_info_toks, def_ref);
        break;
    case pdf_literal_node:
        /* Implement \.{\\pdfliteral} */
        check_o_mode(pdf, "\\pdfliteral", 1 << OMODE_PDF, false);
        new_whatsit(pdf_literal_node);
        if (scan_keyword("direct"))
            set_pdf_literal_mode(tail, direct_always);
        else if (scan_keyword("page"))
            set_pdf_literal_mode(tail, direct_page);
        else
            set_pdf_literal_mode(tail, set_origin);
        scan_pdf_ext_toks();
        set_pdf_literal_type(tail, normal);
        set_pdf_literal_data(tail, def_ref);
        break;
    case pdf_colorstack_node:
        /* Implement \.{\\pdfcolorstack} */
        check_o_mode(pdf, "\\pdfcolorstack", 1 << OMODE_PDF, false);
        /* Scan and check the stack number and store in |cur_val| */
        scan_int();
        if (cur_val >= colorstackused()) {
            print_err("Unknown color stack number ");
            print_int(cur_val);
            help3
                ("Allocate and initialize a color stack with \\pdfcolorstackinit.",
                 "I'll use default color stack 0 here.",
                 "Proceed, with fingers crossed.");
            error();
            cur_val = 0;
        }
        if (cur_val < 0) {
            print_err("Invalid negative color stack number");
            help2("I'll use default color stack 0 here.",
                  "Proceed, with fingers crossed.");
            error();
            cur_val = 0;
        }
        if (scan_keyword("set"))
            i = colorstack_set;
        else if (scan_keyword("push"))
            i = colorstack_push;
        else if (scan_keyword("pop"))
            i = colorstack_pop;
        else if (scan_keyword("current"))
            i = colorstack_current;
        else
            i = -1;             /* error */

        if (i >= 0) {
            new_whatsit(pdf_colorstack_node);
            set_pdf_colorstack_stack(tail, cur_val);
            set_pdf_colorstack_cmd(tail, i);
            set_pdf_colorstack_data(tail, null);
            if (i <= colorstack_data) {
                scan_pdf_ext_toks();
                set_pdf_colorstack_data(tail, def_ref);
            }
        } else {
            print_err("Color stack action is missing");
            help3("The expected actions for \\pdfcolorstack:",
                  "    set, push, pop, current",
                  "I'll ignore the color stack command.");
            error();
        }
        break;
    case pdf_setmatrix_node:
        /* Implement \.{\\pdfsetmatrix} */
        check_o_mode(pdf, "\\pdfsetmatrix", 1 << OMODE_PDF, false);
        new_whatsit(pdf_setmatrix_node);
        scan_pdf_ext_toks();
        set_pdf_setmatrix_data(tail, def_ref);
        break;
    case pdf_save_node:
        /* Implement \.{\\pdfsave} */
        check_o_mode(pdf, "\\pdfsave", 1 << OMODE_PDF, false);
        new_whatsit(pdf_save_node);
        break;
    case pdf_restore_node:
        /* Implement \.{\\pdfrestore} */
        check_o_mode(pdf, "\\pdfrestore", 1 << OMODE_PDF, false);
        new_whatsit(pdf_restore_node);
        break;
    case pdf_map_file_code:
        /* Implement \.{\\pdfmapfile} */
        check_o_mode(pdf, "\\pdfmapfile", 1 << OMODE_PDF, false);
        scan_pdf_ext_toks();
        pdfmapfile(def_ref);
        delete_token_ref(def_ref);
        break;
    case pdf_map_line_code:
        /* Implement \.{\\pdfmapline} */
        check_o_mode(pdf, "\\pdfmapline", 1 << OMODE_PDF, false);
        scan_pdf_ext_toks();
        pdfmapline(def_ref);
        delete_token_ref(def_ref);
        break;
    case pdf_names_code:
        /* Implement \.{\\pdfnames} */
        check_o_mode(pdf, "\\pdfnames", 1 << OMODE_PDF, false);
        scan_pdf_ext_toks();
        pdf_names_toks = concat_tokens(pdf_names_toks, def_ref);
        break;
    case pdf_obj_code:
        /* Implement \.{\\pdfobj} */
        check_o_mode(pdf, "\\pdfobj", 1 << OMODE_PDF, false);
        scan_obj(pdf);
        break;
    case pdf_outline_code:
        /* Implement \.{\\pdfoutline} */
        check_o_mode(pdf, "\\pdfoutline", 1 << OMODE_PDF, true);
        scan_pdfoutline(pdf);
        break;
    case pdf_refobj_node:
        /* Implement \.{\\pdfrefobj} */
        check_o_mode(pdf, "\\pdfrefobj", 1 << OMODE_PDF, false);
        scan_refobj(pdf);
        break;
    case pdf_refxform_node:
        /* Implement \.{\\pdfrefxform} */
        check_o_mode(pdf, "\\pdfrefxform", 1 << OMODE_PDF, false);
        scan_pdfrefxform(pdf);
        break;
    case pdf_refximage_node:
        /* Implement \.{\\pdfrefximage} */
        check_o_mode(pdf, "\\pdfrefximage", 1 << OMODE_PDF, false);
        scan_pdfrefximage(pdf);
        break;
    case save_pos_node:
        /* Implement \.{\\savepos} */
        new_whatsit(save_pos_node);
        break;
    case pdf_start_link_node:
        /* Implement \.{\\pdfstartlink} */
        check_o_mode(pdf, "\\pdfstartlink", 1 << OMODE_PDF, false);
        scan_startlink(pdf);
        break;
    case pdf_start_thread_node:
        /* Implement \.{\\pdfstartthread} */
        check_o_mode(pdf, "\\pdfstartthread", 1 << OMODE_PDF, false);
        new_annot_whatsit(pdf_start_thread_node);
        scan_thread_id();
        break;
    case pdf_thread_node:
        /* Implement \.{\\pdfthread} */
        check_o_mode(pdf, "\\pdfthread", 1 << OMODE_PDF, false);
        new_annot_whatsit(pdf_thread_node);
        scan_thread_id();
        break;
    case pdf_trailer_code:
        /* Implement \.{\\pdftrailer} */
        check_o_mode(pdf, "\\pdftrailer", 1 << OMODE_PDF, false);
        scan_pdf_ext_toks();
        pdf_trailer_toks = concat_tokens(pdf_trailer_toks, def_ref);
        break;
    case pdf_xform_code:
        /* Implement \.{\\pdfxform} */
        check_o_mode(pdf, "\\pdfxform", 1 << OMODE_PDF, false);
        scan_pdfxform(pdf);
        break;
    case pdf_ximage_code:
        /* Implement \.{\\pdfximage} */
        check_o_mode(pdf, "\\pdfximage", 1 << OMODE_PDF, false);
        /* png, jpeg, and pdf image handling depends on this done so early: */
        fix_pdf_minorversion(pdf);
        scan_pdfximage(pdf);
        break;
    case save_cat_code_table_code:
        /* Implement \.{\\savecatcodetable} */
        scan_int();
        if ((cur_val < 0) || (cur_val > 0x7FFF)) {
            print_err("Invalid \\catcode table");
            help1("All \\catcode table ids must be between 0 and 0x7FFF");
            error();
        } else {
            if (cur_val == cat_code_table) {
                print_err("Invalid \\catcode table");
                help1("You cannot overwrite the current \\catcode table");
                error();
            } else {
                copy_cat_codes(cat_code_table, cur_val);
            }
        }
        break;
    case init_cat_code_table_code:
        /* Implement \.{\\initcatcodetable} */
        scan_int();
        if ((cur_val < 0) || (cur_val > 0x7FFF)) {
            print_err("Invalid \\catcode table");
            help1("All \\catcode table ids must be between 0 and 0x7FFF");
            error();
        } else {
            if (cur_val == cat_code_table) {
                print_err("Invalid \\catcode table");
                help1("You cannot overwrite the current \\catcode table");
                error();
            } else {
                initex_cat_codes(cur_val);
            }
        }
        break;
    case set_random_seed_code:
        /* Implement \.{\\setrandomseed} */
        /*  Negative random seed values are silently converted to positive ones */
        scan_int();
        if (cur_val < 0)
            negate(cur_val);
        random_seed = cur_val;
        init_randoms(random_seed);
        break;
    case pdf_glyph_to_unicode_code:
        /* Implement \.{\\pdfglyphtounicode} */
        glyph_to_unicode();
        break;
    case late_lua_node:
        /* Implement \.{\\latelua} */
        new_whatsit(late_lua_node); /* type == normal */
        late_lua_name(tail) = scan_lua_state();
        (void) scan_toks(false, false);
        late_lua_data(tail) = def_ref;
        break;
    default:
        confusion("ext1");
        break;
    }
}


@ Here is a subroutine that creates a whatsit node having a given |subtype|
and a given number of words. It initializes only the first word of the whatsit,
and appends it to the current list.

@c
void new_whatsit(int s)
{
    halfword p;                 /* the new node */
    p = new_node(whatsit_node, s);
    couple_nodes(tail, p);
    tail = p;
}


@ The next subroutine uses |cur_chr| to decide what sort of whatsit is
involved, and also inserts a |write_stream| number.

@c
void new_write_whatsit(int w)
{
    new_whatsit(cur_chr);
    if (w != write_node_size) {
        scan_four_bit_int();
    } else {
        scan_int();
        if (cur_val < 0)
            cur_val = 17;
        else if ((cur_val > 15) && (cur_val != 18))
            cur_val = 16;
    }
    write_stream(tail) = cur_val;
}


@ We have to check whether \.{\\pdfoutput} is set for using \pdfTeX{}
  extensions.

@c
void scan_pdf_ext_toks(void)
{
    (void) scan_toks(false, true);      /* like \.{\\special} */
}

@  We need to check whether the referenced object exists.

finds the node preceding the rightmost node |e|; |s| is some node before |e| 
@c
halfword prev_rightmost(halfword s, halfword e)
{
    halfword p = s;
    if (p == null)
        return null;
    while (vlink(p) != e) {
        p = vlink(p);
        if (p == null)
            return null;
    }
    return p;
}

@ \.{\\pdfxform} and \.{\\pdfrefxform} are similiar to \.{\\pdfobj} and
  \.{\\pdfrefobj}

@c
int pdf_last_xform;

@ \.{\\pdfximage} and \.{\\pdfrefximage} are similiar to \.{\\pdfxform} and
  \.{\\pdfrefxform}. As we have to scan |<rule spec>| quite often, it is better
  have a |rule_node| that holds the most recently scanned |<rule spec>|.

@c
int pdf_last_ximage;
int pdf_last_ximage_pages;
int pdf_last_ximage_colordepth;
int pdf_last_annot;

@ pdflastlink needs an extra global variable 

@c
int pdf_last_link;
scaledpos pdf_last_pos = { 0, 0 };


@ To implement primitives as \.{\\pdfinfo}, \.{\\pdfcatalog} or
\.{\\pdfnames} we need to concatenate tokens lists.

@c
halfword concat_tokens(halfword q, halfword r)
{                               /* concat |q| and |r| and returns the result tokens list */
    halfword p;
    if (q == null)
        return r;
    p = q;
    while (token_link(p) != null)
        p = token_link(p);
    set_token_link(p, token_link(r));
    free_avail(r);
    return q;
}

@ @c
int pdf_retval;                 /* global multi-purpose return value */

@ @c
halfword make_local_par_node(void)
/* This function creates a |local_paragraph| node */
{
    halfword p, q;
    p = new_node(whatsit_node, local_par_node);
    local_pen_inter(p) = local_inter_line_penalty;
    local_pen_broken(p) = local_broken_penalty;
    if (local_left_box != null) {
        q = copy_node_list(local_left_box);
        local_box_left(p) = q;
        local_box_left_width(p) = width(local_left_box);
    }
    if (local_right_box != null) {
        q = copy_node_list(local_right_box);
        local_box_right(p) = q;
        local_box_right_width(p) = width(local_right_box);
    }
    local_par_dir(p) = par_direction;
    return p;
}



@ The \eTeX\ features available in extended mode are grouped into two
categories:  (1)~Some of them are permanently enabled and have no
semantic effect as long as none of the additional primitives are
executed.  (2)~The remaining \eTeX\ features are optional and can be
individually enabled and disabled.  For each optional feature there is
an \eTeX\ state variable named \.{\\...state}; the feature is enabled,
resp.\ disabled by assigning a positive, resp.\ non-positive value to
that integer.


@ In order to handle \.{\\everyeof} we need an array |eof_seen| of
boolean variables.

@c
boolean *eof_seen;              /* has eof been seen? */


@ The |print_group| procedure prints the current level of grouping and
the name corresponding to |cur_group|.

@c
void print_group(boolean e)
{
    switch (cur_group) {
    case bottom_level:
        tprint("bottom level");
        return;
        break;
    case simple_group:
    case semi_simple_group:
        if (cur_group == semi_simple_group)
            tprint("semi ");
        tprint("simple");
        break;;
    case hbox_group:
    case adjusted_hbox_group:
        if (cur_group == adjusted_hbox_group)
            tprint("adjusted ");
        tprint("hbox");
        break;
    case vbox_group:
        tprint("vbox");
        break;
    case vtop_group:
        tprint("vtop");
        break;
    case align_group:
    case no_align_group:
        if (cur_group == no_align_group)
            tprint("no ");
        tprint("align");
        break;
    case output_group:
        tprint("output");
        break;
    case disc_group:
        tprint("disc");
        break;
    case insert_group:
        tprint("insert");
        break;
    case vcenter_group:
        tprint("vcenter");
        break;
    case math_group:
    case math_choice_group:
    case math_shift_group:
    case math_left_group:
        tprint("math");
        if (cur_group == math_choice_group)
            tprint(" choice");
        else if (cur_group == math_shift_group)
            tprint(" shift");
        else if (cur_group == math_left_group)
            tprint(" left");
        break;
    }                           /* there are no other cases */
    tprint(" group (level ");
    print_int(cur_level);
    print_char(')');
    if (saved_value(-1) != 0) { /* |saved_line| */
        if (e)
            tprint(" entered at line ");
        else
            tprint(" at line ");
        print_int(saved_value(-1));     /* |saved_line| */
    }
}


@ The |group_trace| procedure is called when a new level of grouping
begins (|e=false|) or ends (|e=true|) with |saved_value(-1)| containing the
line number.

@c
void group_trace(boolean e)
{
    begin_diagnostic();
    print_char('{');
    if (e)
        tprint("leaving ");
    else
        tprint("entering ");
    print_group(e);
    print_char('}');
    end_diagnostic(false);
}

@ A group entered (or a conditional started) in one file may end in a
different file.  Such slight anomalies, although perfectly legitimate,
may cause errors that are difficult to locate.  In order to be able to
give a warning message when such anomalies occur, \eTeX\ uses the
|grp_stack| and |if_stack| arrays to record the initial |cur_boundary|
and |cond_ptr| values for each input file.

@c
save_pointer *grp_stack;        /* initial |cur_boundary| */
halfword *if_stack;             /* initial |cond_ptr| */


@ When a group ends that was apparently entered in a different input
file, the |group_warning| procedure is invoked in order to update the
|grp_stack|.  If moreover \.{\\tracingnesting} is positive we want to
give a warning message.  The situation is, however, somewhat complicated
by two facts:  (1)~There may be |grp_stack| elements without a
corresponding \.{\\input} file or \.{\\scantokens} pseudo file (e.g.,
error insertions from the terminal); and (2)~the relevant information is
recorded in the |name_field| of the |input_stack| only loosely
synchronized with the |in_open| variable indexing |grp_stack|.

@c
void group_warning(void)
{
    int i;                      /* index into |grp_stack| */
    boolean w;                  /* do we need a warning? */
    base_ptr = input_ptr;
    input_stack[base_ptr] = cur_input;  /* store current state */
    i = in_open;
    w = false;
    while ((grp_stack[i] == cur_boundary) && (i > 0)) {
        /* Set variable |w| to indicate if this case should be reported */
        /* This code scans the input stack in order to determine the type of the
           current input file. */
        if (tracing_nesting > 0) {
            while ((input_stack[base_ptr].state_field == token_list) ||
                   (input_stack[base_ptr].index_field > i))
                decr(base_ptr);
            if (input_stack[base_ptr].name_field > 17)
                w = true;
        }

        grp_stack[i] = save_value(save_ptr);
        decr(i);
    }
    if (w) {
        tprint_nl("Warning: end of ");
        print_group(true);
        tprint(" of a different file");
        print_ln();
        if (tracing_nesting > 1)
            show_context();
        if (history == spotless)
            history = warning_issued;
    }
}


@ When a conditional ends that was apparently started in a different
input file, the |if_warning| procedure is invoked in order to update the
|if_stack|.  If moreover \.{\\tracingnesting} is positive we want to
give a warning message (with the same complications as above).

@c
void if_warning(void)
{
    int i;                      /* index into |if_stack| */
    boolean w;                  /* do we need a warning? */
    base_ptr = input_ptr;
    input_stack[base_ptr] = cur_input;  /* store current state */
    i = in_open;
    w = false;
    while (if_stack[i] == cond_ptr) {
        /* Set variable |w| to... */
        if (tracing_nesting > 0) {
            while ((input_stack[base_ptr].state_field == token_list) ||
                   (input_stack[base_ptr].index_field > i))
                decr(base_ptr);
            if (input_stack[base_ptr].name_field > 17)
                w = true;
        }

        if_stack[i] = vlink(cond_ptr);
        decr(i);
    }
    if (w) {
        tprint_nl("Warning: end of ");
        print_cmd_chr(if_test_cmd, cur_if);
        print_if_line(if_line);
        tprint(" of a different file");
        print_ln();
        if (tracing_nesting > 1)
            show_context();
        if (history == spotless)
            history = warning_issued;
    }
}


@ Conversely, the |file_warning| procedure is invoked when a file ends
and some groups entered or conditionals started while reading from that
file are still incomplete.

@c
void file_warning(void)
{
    halfword p;                 /* saved value of |save_ptr| or |cond_ptr| */
    int l;                      /* saved value of |cur_level| or |if_limit| */
    int c;                      /* saved value of |cur_group| or |cur_if| */
    int i;                      /* saved value of |if_line| */
    p = save_ptr;
    l = cur_level;
    c = cur_group;
    save_ptr = cur_boundary;
    while (grp_stack[in_open] != save_ptr) {
        decr(cur_level);
        tprint_nl("Warning: end of file when ");
        print_group(true);
        tprint(" is incomplete");
        cur_group = save_level(save_ptr);
        save_ptr = save_value(save_ptr);
    }
    save_ptr = p;
    cur_level = (quarterword) l;
    cur_group = (group_code) c; /* restore old values */
    p = cond_ptr;
    l = if_limit;
    c = cur_if;
    i = if_line;
    while (if_stack[in_open] != cond_ptr) {
        tprint_nl("Warning: end of file when ");
        print_cmd_chr(if_test_cmd, cur_if);
        if (if_limit == fi_code)
            tprint_esc("else");
        print_if_line(if_line);
        tprint(" is incomplete");
        if_line = if_line_field(cond_ptr);
        cur_if = if_limit_subtype(cond_ptr);
        if_limit = if_limit_type(cond_ptr);
        cond_ptr = vlink(cond_ptr);
    }
    cond_ptr = p;
    if_limit = l;
    cur_if = c;
    if_line = i;                /* restore old values */
    print_ln();
    if (tracing_nesting > 1)
        show_context();
    if (history == spotless)
        history = warning_issued;
}

@ @c
halfword last_line_fill;        /* the |par_fill_skip| glue node of the new paragraph */


@ The lua interface needs some extra functions. The functions
themselves are quite boring, but they are handy because otherwise this
internal stuff has to be accessed from C directly, where lots of the
defines are not available.

@c
#define get_tex_dimen_register(j) dimen(j)
#define get_tex_skip_register(j) skip(j)
#define get_tex_count_register(j) count(j)
#define get_tex_attribute_register(j) attribute(j)
#define get_tex_box_register(j) box(j)

int set_tex_dimen_register(int j, scaled v)
{
    int a;                      /* return non-nil for error */
    if (global_defs > 0)
        a = 4;
    else
        a = 0;
    word_define(j + scaled_base, v);
    return 0;
}

int set_tex_skip_register(int j, halfword v)
{
    int a;                      /* return non-nil for error */
    if (global_defs > 0)
        a = 4;
    else
        a = 0;
    if (type(v) != glue_spec_node)
        return 1;
    word_define(j + skip_base, v);
    return 0;
}

int set_tex_count_register(int j, scaled v)
{
    int a;                      /* return non-nil for error */
    if (global_defs > 0)
        a = 4;
    else
        a = 0;
    word_define(j + count_base, v);
    return 0;
}

int set_tex_box_register(int j, scaled v)
{
    int a;                      /* return non-nil for error */
    if (global_defs > 0)
        a = 4;
    else
        a = 0;
    define(j + box_base, box_ref_cmd, v);
    return 0;
}

int set_tex_attribute_register(int j, scaled v)
{
    int a;                      /* return non-nil for error */
    if (global_defs > 0)
        a = 4;
    else
        a = 0;
    if (j > max_used_attr)
        max_used_attr = j;
    attr_list_cache = cache_disabled;
    word_define(j + attribute_base, v);
    return 0;
}

int get_tex_toks_register(int j)
{
    str_number s;
    s = get_nullstr();
    if (toks(j) != null) {
        s = tokens_to_string(toks(j));
    }
    return s;
}

int set_tex_toks_register(int j, lstring s)
{
    halfword ref;
    int a;
    ref = get_avail();
    (void) str_toks(s);
    set_token_ref_count(ref, 0);
    set_token_link(ref, token_link(temp_token_head));
    if (global_defs > 0)
        a = 4;
    else
        a = 0;
    define(j + toks_base, call_cmd, ref);
    return 0;
}

scaled get_tex_box_width(int j)
{
    halfword q = box(j);
    if (q != null)
        return width(q);
    return 0;
}

int set_tex_box_width(int j, scaled v)
{
    halfword q = box(j);
    if (q == null)
        return 1;
    width(q) = v;
    return 0;
}

scaled get_tex_box_height(int j)
{
    halfword q = box(j);
    if (q != null)
        return height(q);
    return 0;
}

int set_tex_box_height(int j, scaled v)
{
    halfword q = box(j);
    if (q == null)
        return 1;
    height(q) = v;
    return 0;
}


scaled get_tex_box_depth(int j)
{
    halfword q = box(j);
    if (q != null)
        return depth(q);
    return 0;
}

int set_tex_box_depth(int j, scaled v)
{
    halfword q = box(j);
    if (q == null)
        return 1;
    depth(q) = v;
    return 0;
}


@ This section is devoted to the {\sl Synchronize \TeX nology}
- or simply {\sl Sync\TeX} - used to synchronize between input and output.
This section explains how synchronization basics are implemented.
Before we enter into more technical details,
let us recall in a few words what is synchronization.

\TeX\ typesetting system clearly separates the input and the output material,
and synchronization will provide a new link between both that can help
text editors and viewers to work together.
More precisely, forwards synchronization is the ability,
given a location in the input source file,
to find what is the corresponding place in the output.
Backwards synchronization just performs the opposite:
given a location in the output,
retrieve the corresponding material in the input source file.

For better code management and maintainance, we adopt a naming convention.
Throughout this program, code related to the {\sl Synchronize \TeX nology} is tagged
with the ``{\sl synctex}'' key word. Any code extract where {\sl Sync\TeX} plays
its part, either explicitly or implicitly, (should) contain the string ``{\sl synctex}''.
This naming convention also holds for external files.
Moreover, all the code related to {\sl Sync\TeX} is gathered in this section,
except the definitions.

Enabling synchronization should be performed from the command line,
|synctexoption| is used for that purpose.
This global integer variable is declared here but it is not used here.
This is just a placeholder where the command line controller will put
the {\sl Sync\TeX} related options, and the {\sl Sync\TeX} controller will read them.

@c
int synctexoption;


@ A convenient primitive is provided:
\.{\\synctex=1} in the input source file enables synchronization whereas
\.{\\synctex=0} disables it.
Its memory address is |synctex_code|.
It is initialized by the {\sl Sync\TeX} controller to the command-line option if given.
The controller may filter some reserved bits.

In order to give the {\sl Sync\TeX} controller read and write access to
the contents of the \.{\\synctex} primitive, we declare |synctexoffset|,
such that |mem[synctexoffset]| and \.{\\synctex} correspond to
the same memory storage. |synctexoffset| is initialized to
the correct value when quite everything is initialized.

@c
int synctexoffset;              /* holds the true value of |synctex_code| */


@ Synchronization is achieved with the help of an auxiliary file named
`\.{{\sl jobname}.synctex}' ({\sl jobname} is the contents of the
\.{\\jobname} macro), where a {\sl Sync\TeX} controller implemented
in the external |synctex.c| file will store geometrical information.
This {\sl Sync\TeX} controller will take care of every technical details
concerning the {\sl Sync\TeX} file, we will only focus on the messages
the controller will receive from the \TeX\ program.

The most accurate synchronization information should allow to map
any character of the input source file to the corresponding location
in the output, if relevant.
Ideally, the synchronization information of the input material consists of
the file name, the line and column numbers of every character.
The synchronization information in the output is simply the page number and
either point coordinates, or box dimensions and position.
The problem is that the mapping between these informations is only known at
ship out time, which means that we must keep track of the input
synchronization information until the pages ship out.

As \TeX\ only knows about file names and line numbers,
but forgets the column numbers, we only consider a
restricted input synchronization information called {\sl Sync\TeX\ information}.
It consists of a unique file name identifier, the {\sl Sync\TeX\ file tag},
and the line number.

Keeping track of such information,
should be different whether characters or nodes are involved.
Actually, only certain nodes are involved in {\sl Sync\TeX},
we call them {\sl synchronized nodes}.
Synchronized nodes store the {\sl Sync\TeX} information in their last two words:
the first one contains a {\sl Sync\TeX\ file tag} uniquely identifying
the input file, and the second one contains the current line number,
as returned by the \.{\\inputlineno} primitive.
The |synctex_field_size| macro contains the necessary size to store
the {\sl Sync\TeX} information in a node.

When declaring the size of a new node, it is recommanded to use the following
convention: if the node is synchronized, use a definition similar to
|my_synchronized_node_size|={\sl xxx}+|synctex_field_size|.
Moreover, one should expect that the {\sl Sync\TeX} information is always stored
in the last two words of a synchronized node.

By default, every node with a sufficiently big size is initialized
at creation time in the |get_node| routine with the current
{\sl Sync\TeX} information, whether or not the node is synchronized.
One purpose is to set this information very early in order to minimize code
dependencies, including forthcoming extensions.
Another purpose is to avoid the assumption that every node type has a dedicated getter,
where initialization should take place. Actually, it appears that some nodes are created
using directly the |get_node| routine and not the dedicated constructor.
And finally, initializing the node at only one place is less error prone.

Instead of storing the input file name, it is better to store just an identifier.
Each time \TeX\ opens a new file, it notifies the {\sl Sync\TeX} controller with
a |synctex_start_input| message.
This controller will create a new {\sl Sync\TeX} file tag and
will update the current input state record accordingly.
If the input comes from the terminal or a pseudo file, the |synctex_tag| is set to 0.
It results in automatically disabling synchronization for material
input from the terminal or pseudo files.


Synchronized nodes are boxes, math, kern and glue nodes.
Other nodes should be synchronized too, in particular math noads.
\TeX\ assumes that math, kern and glue nodes have the same size,
this is why both are synchronized.
{\sl In fine}, only horizontal lists are really used in {\sl Sync\TeX},
but all box nodes are considered the same with respect to synchronization,
because a box node type is allowed to change at execution time.

{\sl Nota Bene:}
The {\sl Sync\TeX} code is very close to the memory model.
It is not connected to any other part of the code,
except for memory management. It is possible to neutralize the {\sl Sync\TeX} code
rather simply. The first step is to define a null |synctex_field_size|.
The second step is to comment out the code in ``Initialize bigger nodes...'' and every
``Copy ... {\sl Sync\TeX} information''.
The last step will be to comment out the |synctex_tag_field| related code in the
definition of |synctex_tag| and the various ``Prepare ... {\sl Sync\TeX} information''.
Then all the remaining code should be just harmless.
The resulting program would behave exactly the same as if absolutely no {\sl Sync\TeX}
related code was there, including memory management.
Of course, all this assumes that {\sl Sync\TeX} is turned off from the command line.
@^synctex@>
@^synchronization@>


@ Here are extra variables for Web2c.  (This numbering of the
system-dependent section allows easy integration of Web2c and e-\TeX, etc.)
@^<system dependencies@>

@c
pool_pointer edit_name_start;   /* where the filename to switch to starts */
int edit_name_length, edit_line;        /* what line to start editing at */
int ipcon;                      /* level of IPC action, 0 for none [default] */
boolean stop_at_space;          /* whether |more_name| returns false for space */

@ The |edit_name_start| will be set to point into |str_pool| somewhere after
its beginning if \TeX\ is supposed to switch to an editor on exit.

@c
int shellenabledp;
int restrictedshell;
char *output_comment;

@ Are we printing extra info as we read the format file? 

@c
boolean debug_format_file;
