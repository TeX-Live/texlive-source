/* $Id: texnodes.c 1168 2008-04-15 13:43:34Z taco $ */

#include "luatex-api.h"
#include <ptexlib.h>
#include "nodes.h"

#define MAX_CHAIN_SIZE 12

volatile memory_word *varmem = NULL;

#ifndef NDEBUG
char *varmem_sizes = NULL;
#endif

halfword var_mem_max = 0;
halfword rover = 0;

halfword free_chain[MAX_CHAIN_SIZE] = { null };

static int my_prealloc = 0;

int fix_node_lists = 1;

int free_error_seen = 0;
int copy_error_seen = 0;

halfword slow_get_node(integer s);      /* defined below */
int copy_error(halfword p);     /* define below */

#undef link                     /* defined by cpascal.h */
#define info(a)    fixmem[(a)].hhlh
#define link(a)    fixmem[(a)].hhrh

#define fake_node 100
#define fake_node_size 2
#define fake_node_name "fake"

#define variable_node_size 2


char *node_fields_list[] = { "attr", "width", "depth", "height", "dir", "shift",
    "glue_order", "glue_sign", "glue_set", "list", NULL
};
char *node_fields_rule[] = { "attr", "width", "depth", "height", "dir", NULL };
char *node_fields_insert[] =
    { "attr", "cost", "depth", "height", "spec", "list", NULL };
char *node_fields_mark[] = { "attr", "class", "mark", NULL };
char *node_fields_adjust[] = { "attr", "list", NULL };
char *node_fields_disc[] = { "attr", "pre", "post", "replace", NULL };
char *node_fields_math[] = { "attr", "surround", NULL };
char *node_fields_glue[] = { "attr", "spec", "leader", NULL };
char *node_fields_kern[] = { "attr", "kern", NULL };
char *node_fields_penalty[] = { "attr", "penalty", NULL };
char *node_fields_unset[] =
    { "attr", "width", "depth", "height", "dir", "shrink",
    "glue_order", "glue_sign", "stretch", "span", "list", NULL
};
char *node_fields_margin_kern[] = { "attr", "width", "glyph", NULL };
char *node_fields_glyph[] =
    { "attr", "char", "font", "lang", "left", "right", "uchyph",
    "components", "xoffset", "yoffset", NULL
};
char *node_fields_style[] = { NULL };
char *node_fields_choice[] = { NULL };
char *node_fields_ord[] = { NULL };
char *node_fields_op[] = { NULL };
char *node_fields_bin[] = { NULL };
char *node_fields_rel[] = { NULL };
char *node_fields_open[] = { NULL };
char *node_fields_close[] = { NULL };
char *node_fields_punct[] = { NULL };
char *node_fields_inner[] = { NULL };
char *node_fields_radical[] = { NULL };
char *node_fields_fraction[] = { NULL };
char *node_fields_under[] = { NULL };
char *node_fields_over[] = { NULL };
char *node_fields_accent[] = { NULL };
char *node_fields_vcenter[] = { NULL };
char *node_fields_left[] = { NULL };
char *node_fields_right[] = { NULL };

char *node_fields_inserting[] =
    { "height", "last_ins_ptr", "best_ins_ptr", NULL };

char *node_fields_splitup[] = { "height", "last_ins_ptr", "best_ins_ptr",
    "broken_ptr", "broken_ins", NULL
};

char *node_fields_action[] = { "action_type", "named_id", "action_id",
    "file", "new_window", "data", "ref_count", NULL
};
char *node_fields_attribute[] = { "number", "value", NULL };
char *node_fields_glue_spec[] = { "width", "stretch", "shrink",
    "stretch_order", "shrink_order", "ref_count", NULL
};
char *node_fields_attribute_list[] = { NULL };

char *node_fields_whatsit_open[] =
    { "attr", "stream", "name", "area", "ext", NULL };
char *node_fields_whatsit_write[] = { "attr", "stream", "data", NULL };
char *node_fields_whatsit_close[] = { "attr", "stream", NULL };
char *node_fields_whatsit_special[] = { "attr", "data", NULL };

char *node_fields_whatsit_local_par[] =
    { "attr", "pen_inter", "pen_broken", "dir",
    "box_left", "box_left_width", "box_right", "box_right_width", NULL
};
char *node_fields_whatsit_dir[] =
    { "attr", "dir", "level", "dvi_ptr", "dvi_h", NULL };

char *node_fields_whatsit_pdf_literal[] = { "attr", "mode", "data", NULL };
char *node_fields_whatsit_pdf_refobj[] = { "attr", "objnum", NULL };
char *node_fields_whatsit_pdf_refxform[] =
    { "attr", "width", "height", "depth", "objnum", NULL };
char *node_fields_whatsit_pdf_refximage[] =
    { "attr", "width", "height", "depth", "objnum", NULL };
char *node_fields_whatsit_pdf_annot[] =
    { "attr", "width", "height", "depth", "objnum", "data", NULL };
char *node_fields_whatsit_pdf_start_link[] =
    { "attr", "width", "height", "depth",
    "objnum", "link_attr", "action", NULL
};
char *node_fields_whatsit_pdf_end_link[] = { "attr", NULL };
char *node_fields_whatsit_pdf_dest[] = { "attr", "width", "height", "depth",
    "named_id", "dest_id", "dest_type", "xyz_zoom", "objnum", NULL
};
char *node_fields_whatsit_pdf_thread[] = { "attr", "width", "height", "depth",
    "named_id", "thread_id", "thread_attr", NULL
};
char *node_fields_whatsit_pdf_start_thread[] =
    { "attr", "width", "height", "depth",
    "named_id", "thread_id", "thread_attr", NULL
};
char *node_fields_whatsit_pdf_end_thread[] = { "attr", NULL };
char *node_fields_whatsit_pdf_save_pos[] = { "attr", NULL };
char *node_fields_whatsit_late_lua[] = { "attr", "reg", "data", NULL };
char *node_fields_whatsit_close_lua[] = { "attr", "reg", NULL };
char *node_fields_whatsit_pdf_colorstack[] =
    { "attr", "stack", "cmd", "data", NULL };
char *node_fields_whatsit_pdf_setmatrix[] = { "attr", "data", NULL };
char *node_fields_whatsit_pdf_save[] = { "attr", NULL };
char *node_fields_whatsit_pdf_restore[] = { "attr", NULL };
char *node_fields_whatsit_cancel_boundary[] = { "attr", NULL };
char *node_fields_whatsit_user_defined[] =
    { "attr", "user_id", "type", "value", NULL };

node_info node_data[] = {
    {hlist_node, box_node_size, node_fields_list, "hlist"},
    {vlist_node, box_node_size, node_fields_list, "vlist"},
    {rule_node, rule_node_size, node_fields_rule, "rule"},
    {ins_node, ins_node_size, node_fields_insert, "ins"},
    {mark_node, mark_node_size, node_fields_mark, "mark"},
    {adjust_node, adjust_node_size, node_fields_adjust, "adjust"},
    {fake_node, fake_node_size, NULL, fake_node_name},  /* don't touch this! */
    {disc_node, disc_node_size, node_fields_disc, "disc"},
    {whatsit_node, -1, NULL, "whatsit"},
    {math_node, math_node_size, node_fields_math, "math"},
    {glue_node, glue_node_size, node_fields_glue, "glue"},
    {kern_node, kern_node_size, node_fields_kern, "kern"},
    {penalty_node, penalty_node_size, node_fields_penalty, "penalty"},
    {unset_node, box_node_size, node_fields_unset, "unset"},
    {style_node, style_node_size, node_fields_style, "style"},
    {choice_node, style_node_size, node_fields_choice, "choice"},
    {ord_noad, noad_size, node_fields_ord, "ord"},
    {op_noad, noad_size, node_fields_op, "op"},
    {bin_noad, noad_size, node_fields_bin, "bin"},
    {rel_noad, noad_size, node_fields_rel, "rel"},
    {open_noad, noad_size, node_fields_open, "open"},
    {close_noad, noad_size, node_fields_close, "close"},
    {punct_noad, noad_size, node_fields_punct, "punct"},
    {inner_noad, noad_size, node_fields_inner, "inner"},
    {radical_noad, radical_noad_size, node_fields_radical, "radical"},
    {fraction_noad, fraction_noad_size, node_fields_fraction, "fraction"},
    {under_noad, noad_size, node_fields_under, "under"},
    {over_noad, noad_size, node_fields_over, "over"},
    {accent_noad, accent_noad_size, node_fields_accent, "accent"},
    {vcenter_noad, noad_size, node_fields_vcenter, "vcenter"},
    {left_noad, noad_size, node_fields_left, "left"},
    {right_noad, noad_size, node_fields_right, "right"},
    {margin_kern_node, margin_kern_node_size, node_fields_margin_kern,
     "margin_kern"},
    {glyph_node, glyph_node_size, node_fields_glyph, "glyph"},  /* 33 */
    {align_record_node, box_node_size, NULL, "align_record"},
    {pseudo_file_node, pseudo_file_node_size, NULL, "pseudo_file"},
    {pseudo_line_node, variable_node_size, NULL, "pseudo_line"},
    {inserting_node, page_ins_node_size, node_fields_inserting, "page_insert"},
    {split_up_node, page_ins_node_size, node_fields_splitup, "split_insert"},
    {expr_node, expr_node_size, NULL, "expr_stack"},
    {nesting_node, nesting_node_size, NULL, "nested_list"},     /* 40 */
    {span_node, span_node_size, NULL, "span"},
    {attribute_node, attribute_node_size, node_fields_attribute, "attribute"},
    {glue_spec_node, glue_spec_size, node_fields_glue_spec, "glue_spec"},
    {attribute_list_node, attribute_node_size, node_fields_attribute_list,
     "attribute_list"},
    {action_node, pdf_action_size, node_fields_action, "action"},
    {temp_node, temp_node_size, NULL, "temp"},
    {align_stack_node, align_stack_node_size, NULL, "align_stack"},
    {movement_node, movement_node_size, NULL, "movement_stack"},
    {if_node, if_node_size, NULL, "if_stack"},
    {unhyphenated_node, active_node_size, NULL, "unhyphenated"},        /* 50 */
    {hyphenated_node, active_node_size, NULL, "hyphenated"},
    {delta_node, delta_node_size, NULL, "delta"},
    {passive_node, passive_node_size, NULL, "passive"},
    {shape_node, variable_node_size, NULL, "shape"},
    {-1, -1, NULL, NULL}
};

#define last_normal_node shape_node

node_info whatsit_node_data[] = {
    {open_node, open_node_size, node_fields_whatsit_open, "open"},
    {write_node, write_node_size, node_fields_whatsit_write, "write"},
    {close_node, close_node_size, node_fields_whatsit_close, "close"},
    {special_node, special_node_size, node_fields_whatsit_special, "special"},
    {fake_node, fake_node_size, NULL, fake_node_name},
    {fake_node, fake_node_size, NULL, fake_node_name},
    {local_par_node, local_par_size, node_fields_whatsit_local_par,
     "local_par"},
    {dir_node, dir_node_size, node_fields_whatsit_dir, "dir"},
    {pdf_literal_node, write_node_size, node_fields_whatsit_pdf_literal,
     "pdf_literal"},
    {fake_node, fake_node_size, NULL, fake_node_name},
    {pdf_refobj_node, pdf_refobj_node_size, node_fields_whatsit_pdf_refobj,
     "pdf_refobj"},
    {fake_node, fake_node_size, NULL, fake_node_name},
    {pdf_refxform_node, pdf_refxform_node_size,
     node_fields_whatsit_pdf_refxform, "pdf_refxform"},
    {fake_node, fake_node_size, NULL, fake_node_name},
    {pdf_refximage_node, pdf_refximage_node_size,
     node_fields_whatsit_pdf_refximage, "pdf_refximage"},
    {pdf_annot_node, pdf_annot_node_size, node_fields_whatsit_pdf_annot,
     "pdf_annot"},
    {pdf_start_link_node, pdf_annot_node_size,
     node_fields_whatsit_pdf_start_link, "pdf_start_link"},
    {pdf_end_link_node, pdf_end_link_node_size,
     node_fields_whatsit_pdf_end_link, "pdf_end_link"},
    {fake_node, fake_node_size, NULL, fake_node_name},
    {pdf_dest_node, pdf_dest_node_size, node_fields_whatsit_pdf_dest,
     "pdf_dest"},
    {pdf_thread_node, pdf_thread_node_size, node_fields_whatsit_pdf_thread,
     "pdf_thread"},
    {pdf_start_thread_node, pdf_thread_node_size,
     node_fields_whatsit_pdf_start_thread, "pdf_start_thread"},
    {pdf_end_thread_node, pdf_end_thread_node_size,
     node_fields_whatsit_pdf_end_thread, "pdf_end_thread"},
    {pdf_save_pos_node, pdf_save_pos_node_size,
     node_fields_whatsit_pdf_save_pos, "pdf_save_pos"},
    {pdf_thread_data_node, pdf_thread_node_size, NULL, "pdf_thread_data"},
    {pdf_link_data_node, pdf_annot_node_size, NULL, "pdf_link_data"},
    {fake_node, fake_node_size, NULL, fake_node_name},
    {fake_node, fake_node_size, NULL, fake_node_name},
    {fake_node, fake_node_size, NULL, fake_node_name},
    {fake_node, fake_node_size, NULL, fake_node_name},
    {fake_node, fake_node_size, NULL, fake_node_name},
    {fake_node, fake_node_size, NULL, fake_node_name},
    {fake_node, fake_node_size, NULL, fake_node_name},
    {fake_node, fake_node_size, NULL, fake_node_name},
    {fake_node, fake_node_size, NULL, fake_node_name},
    {late_lua_node, write_node_size, node_fields_whatsit_late_lua, "late_lua"},
    {close_lua_node, write_node_size, node_fields_whatsit_close_lua,
     "close_lua"},
    {fake_node, fake_node_size, NULL, fake_node_name},
    {fake_node, fake_node_size, NULL, fake_node_name},
    {pdf_colorstack_node, pdf_colorstack_node_size,
     node_fields_whatsit_pdf_colorstack, "pdf_colorstack"},
    {pdf_setmatrix_node, pdf_setmatrix_node_size,
     node_fields_whatsit_pdf_setmatrix, "pdf_setmatrix"},
    {pdf_save_node, pdf_save_node_size, node_fields_whatsit_pdf_save,
     "pdf_save"},
    {pdf_restore_node, pdf_restore_node_size, node_fields_whatsit_pdf_restore,
     "pdf_restore"},
    {cancel_boundary_node, cancel_boundary_size,
     node_fields_whatsit_cancel_boundary, "cancel_boundary"},
    {user_defined_node, user_defined_node_size,
     node_fields_whatsit_user_defined, "user_defined"},
    {-1, -1, NULL, NULL}
};

#define last_whatsit_node user_defined_node

#define  get_node_size(i,j) (i!=whatsit_node ? node_data[i].size : whatsit_node_data[j].size)
#define  get_node_name(i,j) (i!=whatsit_node ? node_data[i].name : whatsit_node_data[j].name)

halfword new_node(int i, int j)
{
    register int s;
    register halfword n;
    s = get_node_size(i, j);
    n = get_node(s);
    /* it should be possible to do this memset at free_node()  */
    /* type() and subtype() will be set below, and vlink() is 
       set to null by get_node(), so we can do we clearing one 
       word less than |s| */
    (void) memset((void *) (varmem + n + 1), 0,
                  (sizeof(memory_word) * (s - 1)));
    switch (i) {
    case glyph_node:
        init_lang_data(n);
        break;
    case glue_node:
    case kern_node:
    case glue_spec_node:
        break;
    case hlist_node:
    case vlist_node:
        box_dir(n) = -1;
        break;
    case whatsit_node:
        if (j == open_node) {
            open_name(n) = get_nullstr();
            open_area(n) = open_name(n);
            open_ext(n) = open_name(n);
        }
        break;
    case disc_node:
        pre_break(n) = pre_break_head(n);
        type(pre_break(n)) = nesting_node;
        subtype(pre_break(n)) = pre_break_head(0);
        post_break(n) = post_break_head(n);
        type(post_break(n)) = nesting_node;
        subtype(post_break(n)) = post_break_head(0);
        no_break(n) = no_break_head(n);
        type(no_break(n)) = nesting_node;
        subtype(no_break(n)) = no_break_head(0);
        break;
    case rule_node:
        depth(n) = null_flag;
        height(n) = null_flag;
        rule_dir(n) = -1;
        /* fall through */
    case unset_node:
        width(n) = null_flag;
        break;
    case pseudo_line_node:
    case shape_node:
        /* this is a trick that makes pseudo_files slightly slower,
         * but the overall allocation faster then an explicit test
         * at the top of new_node().
         */
        free_node(n, variable_node_size);
        n = slow_get_node(j);
        (void) memset((void *) (varmem + n + 1), 0,
                      (sizeof(memory_word) * (j - 1)));
        break;
    default:
        break;
    }
    if (nodetype_has_attributes(i)) {
        build_attribute_list(n);
    }
    type(n) = i;
    subtype(n) = j;
    return n;
}

halfword raw_glyph_node(void)
{
    register halfword n;
    n = get_node(glyph_node_size);
    (void) memset((void *) (varmem + n + 1), 0,
                  (sizeof(memory_word) * (glyph_node_size - 1)));
    type(n) = glyph_node;
    subtype(n) = 0;
    return n;
}

halfword new_glyph_node(void)
{
    register halfword n;
    n = get_node(glyph_node_size);
    (void) memset((void *) (varmem + n + 1), 0,
                  (sizeof(memory_word) * (glyph_node_size - 1)));
    type(n) = glyph_node;
    subtype(n) = 0;
    build_attribute_list(n);
    return n;
}


/* makes a duplicate of the node list that starts at |p| and returns a
   pointer to the new list */

halfword copy_node_list(halfword p)
{
    halfword q;                 /* previous position in new list */
    halfword h = null;          /* head of the list */
    copy_error_seen = 0;
    while (p != null) {
        register halfword s = copy_node(p);
        if (h == null) {
            h = s;
        } else {
            couple_nodes(q, s);
        }
        q = s;
        p = vlink(p);
    }
    return h;
}

 /* make a dupe of a single node */
halfword copy_node(const halfword p)
{
    halfword r;                 /* current node being fabricated for new list */
    register halfword s;        /* a helper variable for copying into variable mem  */
    register int i;
    if (copy_error(p)) {
        r = new_node(temp_node, 0);
        return r;
    }
    i = get_node_size(type(p), subtype(p));
    r = get_node(i);
    (void) memcpy((void *) (varmem + r), (void *) (varmem + p),
                  (sizeof(memory_word) * i));

    if (nodetype_has_attributes(type(p))) {
        add_node_attr_ref(node_attr(p));
        alink(r) = null;        /* needs checking */
    }
    vlink(r) = null;
    switch (type(p)) {
    case glyph_node:
        s = copy_node_list(lig_ptr(p));
        lig_ptr(r) = s;
        break;
    case glue_node:
        add_glue_ref(glue_ptr(p));
        s = copy_node_list(leader_ptr(p));
        leader_ptr(r) = s;
        break;
    case hlist_node:
    case vlist_node:
    case unset_node:
        s = copy_node_list(list_ptr(p));
        list_ptr(r) = s;
        break;
    case ins_node:
        add_glue_ref(split_top_ptr(p));
        s = copy_node_list(ins_ptr(p));
        ins_ptr(r) = s;
        break;
    case margin_kern_node:
        s = copy_node(margin_char(p));
        margin_char(r) = s;
        break;
    case disc_node:
        pre_break(r) = pre_break_head(r);
        if (vlink_pre_break(p) != null) {
            s = copy_node_list(vlink_pre_break(p));
            alink(s) = pre_break(r);
            tlink_pre_break(r) = tail_of_list(s);
            vlink_pre_break(r) = s;
        } else {
            assert(tlink(pre_break(r)) == null);
        }
        post_break(r) = post_break_head(r);
        if (vlink_post_break(p) != null) {
            s = copy_node_list(vlink_post_break(p));
            alink(s) = post_break(r);
            tlink_post_break(r) = tail_of_list(s);
            vlink_post_break(r) = s;
        } else {
            assert(tlink_post_break(r) == null);
        }
        no_break(r) = no_break_head(r);
        if (vlink(no_break(p)) != null) {
            s = copy_node_list(vlink_no_break(p));
            alink(s) = no_break(r);
            tlink_no_break(r) = tail_of_list(s);
            vlink_no_break(r) = s;
        } else {
            assert(tlink_no_break(r) == null);
        }
        break;
    case mark_node:
        add_token_ref(mark_ptr(p));
        break;
    case adjust_node:
        s = copy_node_list(adjust_ptr(p));
        adjust_ptr(r) = s;
        break;
    case glue_spec_node:
        glue_ref_count(r) = null;
        break;
    case whatsit_node:
        switch (subtype(p)) {
        case dir_node:
        case local_par_node:
            break;
        case write_node:
        case special_node:
            add_token_ref(write_tokens(p));
            break;
        case pdf_literal_node:
            add_token_ref(pdf_literal_data(p));
            break;
        case pdf_colorstack_node:
            if (pdf_colorstack_cmd(p) <= colorstack_data)
                add_token_ref(pdf_colorstack_data(p));
            break;
        case pdf_setmatrix_node:
            add_token_ref(pdf_setmatrix_data(p));
            break;
        case late_lua_node:
            add_token_ref(late_lua_data(p));
            break;
        case pdf_annot_node:
            add_token_ref(pdf_annot_data(p));
            break;
        case pdf_start_link_node:
            if (pdf_link_attr(r) != null)
                add_token_ref(pdf_link_attr(r));
            add_action_ref(pdf_link_action(r));
            break;
        case pdf_dest_node:
            if (pdf_dest_named_id(p) > 0)
                add_token_ref(pdf_dest_id(p));
            break;
        case pdf_thread_node:
        case pdf_start_thread_node:
            if (pdf_thread_named_id(p) > 0)
                add_token_ref(pdf_thread_id(p));
            if (pdf_thread_attr(p) != null)
                add_token_ref(pdf_thread_attr(p));
            break;
        case user_defined_node:
            switch (user_node_type(p)) {
            case 'a':
                add_node_attr_ref(user_node_value(p));
                break;
            case 't':
                add_token_ref(user_node_value(p));
                break;
            case 'n':
                s = copy_node_list(user_node_value(p));
                user_node_value(r) = s;
                break;
            }
            break;
        default:
            break;
        }
        break;
    }
    return r;
}

int valid_node(halfword p)
{
    if (p > my_prealloc) {
        if (p < var_mem_max) {
            if (varmem_sizes[p] > 0)
                return 1;
        }
    } else {
        return 1;
    }
    return 0;
}

static void do_free_error(halfword p)
{
    char errstr[255] = { 0 };
    char *errhlp[] =
        {
"When I tried to free the node mentioned in the error message, it turned",
        "out it was not (or no longer) actually in use.",
        "Errors such as these are often caused by Lua node list alteration,",
        "but could also point to a bug in the executable. It should be safe to continue.",
        NULL
    };
    halfword r;

    check_node_mem();
    if (free_error_seen)
        return;

    r = null;
    free_error_seen = 1;
    if (type(p) == glyph_node) {
        snprintf(errstr, 255,
                 "Attempt to double-free glyph (%c) node %d, ignored",
                 (int) character(p), (int) p);
    } else {
        snprintf(errstr, 255, "Attempt to double-free %s node %d, ignored",
                 get_node_name(type(p), subtype(p)), (int) p);
    }
    tex_error(errstr, errhlp);
    for (r = my_prealloc + 1; r < var_mem_max; r++) {
        if (vlink(r) == p) {
            halfword s = r;
            while (s > my_prealloc && varmem_sizes[s] == 0)
                s--;
            if (s != null
                && s != my_prealloc
                && s != var_mem_max
                && (r - s) < get_node_size(type(s), subtype(s))
                && alink(s) != p) {

                if (type(s) == disc_node) {
                    fprintf(stdout,
                            "  pointed to from %s node %d (vlink %d, alink %d): ",
                            get_node_name(type(s), subtype(s)), (int) s,
                            (int) vlink(s), (int) alink(s));
                    fprintf(stdout, "pre_break(%d,%d,%d), ",
                            (int) vlink_pre_break(s), (int) tlink(pre_break(s)),
                            (int) alink(pre_break(s)));
                    fprintf(stdout, "post_break(%d,%d,%d), ",
                            (int) vlink_post_break(s),
                            (int) tlink(post_break(s)),
                            (int) alink(post_break(s)));
                    fprintf(stdout, "no_break(%d,%d,%d)",
                            (int) vlink_no_break(s), (int) tlink(no_break(s)),
                            (int) alink(no_break(s)));
                    fprintf(stdout, "\n");
                } else {
                    if (vlink(s) == p
                        || (type(s) == glyph_node && lig_ptr(s) == p)
                        || (type(s) == vlist_node && list_ptr(s) == p)
                        || (type(s) == hlist_node && list_ptr(s) == p)
                        || (type(s) == unset_node && list_ptr(s) == p)
                        || (type(s) == ins_node && ins_ptr(s) == p)
                        ) {
                        fprintf(stdout,
                                "  pointed to from %s node %d (vlink %d, alink %d): ",
                                get_node_name(type(s), subtype(s)), (int) s,
                                (int) vlink(s), (int) alink(s));
                        if (type(s) == glyph_node) {
                            fprintf(stdout, "lig_ptr(%d)", (int) lig_ptr(s));
                        } else if (type(s) == vlist_node
                                   || type(s) == hlist_node) {
                            fprintf(stdout, "list_ptr(%d)", (int) list_ptr(s));
                        }
                        fprintf(stdout, "\n");
                    } else {
                        if ((type(s) != penalty_node)
                            && (type(s) != math_node)
                            && (type(s) != kern_node)
                            ) {
                            fprintf(stdout, "  pointed to from %s node %d\n",
                                    get_node_name(type(s), subtype(s)),
                                    (int) s);
                        }
                    }
                }
            }
        }
    }
}

int free_error(halfword p)
{
    assert(p > my_prealloc);
    assert(p < var_mem_max);
    if (varmem_sizes[p] == 0) {
        do_free_error(p);
        return 1;               /* double free */
    }
    return 0;
}


static void do_copy_error(halfword p)
{
    char errstr[255] = { 0 };
    char *errhlp[] =
        {
"When I tried to copy the node mentioned in the error message, it turned",
        "out it was not (or no longer) actually in use.",
        "Errors such as these are often caused by Lua node list alteration,",
        "but could also point to a bug in the executable. It should be safe to continue.",
        NULL
    };

    if (copy_error_seen)
        return;

    copy_error_seen = 1;
    if (type(p) == glyph_node) {
        snprintf(errstr, 255,
                 "Attempt to copy free glyph (%c) node %d, ignored",
                 (int) character(p), (int) p);
    } else {
        snprintf(errstr, 255, "Attempt to copy free %s node %d, ignored",
                 get_node_name(type(p), subtype(p)), (int) p);
    }
    tex_error(errstr, errhlp);
}


int copy_error(halfword p)
{
    assert(p >= 0);
    assert(p < var_mem_max);
    if (p > my_prealloc && varmem_sizes[p] == 0) {
        do_copy_error(p);
        return 1;               /* copy free node */
    }
    return 0;
}



void flush_node(halfword p)
{

    if (p == null)              /* legal, but no-op */
        return;

    if (free_error(p))
        return;

    switch (type(p)) {
    case glyph_node:
        flush_node_list(lig_ptr(p));
        break;
    case glue_node:
        delete_glue_ref(glue_ptr(p));
        flush_node_list(leader_ptr(p));
        break;

    case attribute_node:
    case attribute_list_node:
    case temp_node:
    case glue_spec_node:
    case rule_node:
    case kern_node:
    case math_node:
    case penalty_node:
        break;

    case hlist_node:
    case vlist_node:
    case unset_node:
        flush_node_list(list_ptr(p));
        break;
    case whatsit_node:
        switch (subtype(p)) {

        case dir_node:
            break;
        case open_node:
        case write_node:
        case close_node:
        case pdf_save_node:
        case pdf_restore_node:
        case cancel_boundary_node:
        case close_lua_node:
        case pdf_refobj_node:
        case pdf_refxform_node:
        case pdf_refximage_node:
        case pdf_end_link_node:
        case pdf_end_thread_node:
        case pdf_save_pos_node:
        case local_par_node:
            break;

        case special_node:
            delete_token_ref(write_tokens(p));
            break;
        case pdf_literal_node:
            delete_token_ref(pdf_literal_data(p));
            break;
        case pdf_colorstack_node:
            if (pdf_colorstack_cmd(p) <= colorstack_data)
                delete_token_ref(pdf_colorstack_data(p));
            break;
        case pdf_setmatrix_node:
            delete_token_ref(pdf_setmatrix_data(p));
            break;
        case late_lua_node:
            delete_token_ref(late_lua_data(p));
            break;
        case pdf_annot_node:
            delete_token_ref(pdf_annot_data(p));
            break;

        case pdf_link_data_node:
            break;

        case pdf_start_link_node:
            if (pdf_link_attr(p) != null)
                delete_token_ref(pdf_link_attr(p));
            delete_action_ref(pdf_link_action(p));
            break;
        case pdf_dest_node:
            if (pdf_dest_named_id(p) > 0)
                delete_token_ref(pdf_dest_id(p));
            break;

        case pdf_thread_data_node:
            break;

        case pdf_thread_node:
        case pdf_start_thread_node:
            if (pdf_thread_named_id(p) > 0)
                delete_token_ref(pdf_thread_id(p));
            if (pdf_thread_attr(p) != null)
                delete_token_ref(pdf_thread_attr(p));
            break;
        case user_defined_node:
            switch (user_node_type(p)) {
            case 'a':
                delete_attribute_ref(user_node_value(p));
                break;
            case 't':
                delete_token_ref(user_node_value(p));
                break;
            case 'n':
                flush_node_list(user_node_value(p));
                break;
            default:
                tconfusion("extuser");
                break;
            }
            break;

        default:
            tconfusion("ext3");
            return;

        }
        break;
    case ins_node:
        flush_node_list(ins_ptr(p));
        delete_glue_ref(split_top_ptr(p));
        break;
    case margin_kern_node:
        flush_node(margin_char(p));
        break;
    case mark_node:
        delete_token_ref(mark_ptr(p));
        break;
    case disc_node:
        flush_node_list(vlink(pre_break(p)));
        flush_node_list(vlink(post_break(p)));
        flush_node_list(vlink(no_break(p)));
        break;
    case adjust_node:
        flush_node_list(adjust_ptr(p));
        break;
    case style_node:
        break;
    case choice_node:
        flush_node_list(display_mlist(p));
        flush_node_list(text_mlist(p));
        flush_node_list(script_mlist(p));
        flush_node_list(script_script_mlist(p));
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

        /*
         * if (math_type(nucleus(p))>=sub_box)
         *  flush_node_list(vinfo(nucleus(p)));
         * if (math_type(supscr(p))>=sub_box)
         *  flush_node_list(vinfo(supscr(p)));
         * if (math_type(subscr(p))>=sub_box)
         *  flush_node_list(vinfo(subscr(p)));
         */

        break;
    case left_noad:
    case right_noad:
        break;
    case fraction_noad:
        flush_node_list(vinfo(numerator(p)));
        flush_node_list(vinfo(denominator(p)));
        break;
    case pseudo_file_node:
        flush_node_list(pseudo_lines(p));
        break;
    case pseudo_line_node:
    case shape_node:
        free_node(p, subtype(p));
        return;
        break;
    case align_stack_node:
    case span_node:
    case movement_node:
    case if_node:
    case nesting_node:
    case unhyphenated_node:
    case hyphenated_node:
    case delta_node:
    case passive_node:
    case action_node:
    case inserting_node:
    case split_up_node:
    case expr_node:
        break;
    default:
        fprintf(stdout, "flush_node: type is %d\n", type(p));
        return;

    }
    if (nodetype_has_attributes(type(p)))
        delete_attribute_ref(node_attr(p));
    free_node(p, get_node_size(type(p), subtype(p)));
    return;
}

void flush_node_list(halfword pp)
{                               /* erase list of nodes starting at |p| */
    register halfword p = pp;
    free_error_seen = 0;
    if (p == null)              /* legal, but no-op */
        return;
    if (free_error(p))
        return;

    while (p != null) {
        register halfword q = vlink(p);
        flush_node(p);
        p = q;
    }
}

static int test_count = 1;

#define dorangetest(a,b,c)  do {					\
    if (!(b>=0 && b<c)) {                                               \
      fprintf(stdout,"For node p:=%d, 0<=%d<%d (l.%d,r.%d)\n",          \
              (int)a, (int)b, (int)c, __LINE__,test_count);             \
      tconfusion("dorangetest");					\
    } } while (0)

#define dotest(a,b,c) do {						\
    if (b!=c) {                                                         \
      fprintf(stdout,"For node p:=%d, %d==%d (l.%d,r.%d)\n",            \
              (int)a, (int)b, (int)c, __LINE__,test_count);             \
      tconfusion("dotest");						\
    } } while (0)

#define check_action_ref(a)     { dorangetest(p,a,var_mem_max); }
#define check_glue_ref(a)       { dorangetest(p,a,var_mem_max); }
#define check_attribute_ref(a)  { dorangetest(p,a,var_mem_max); }
#define check_token_ref(a)      assert(1)

void check_node(halfword p)
{

    switch (type(p)) {
    case glyph_node:
        dorangetest(p, lig_ptr(p), var_mem_max);
        break;
    case glue_node:
        check_glue_ref(glue_ptr(p));
        dorangetest(p, leader_ptr(p), var_mem_max);
        break;
    case hlist_node:
    case vlist_node:
    case unset_node:
    case align_record_node:
        dorangetest(p, list_ptr(p), var_mem_max);
        break;
    case ins_node:
        dorangetest(p, ins_ptr(p), var_mem_max);
        check_glue_ref(split_top_ptr(p));
        break;
    case whatsit_node:
        switch (subtype(p)) {
        case special_node:
            check_token_ref(write_tokens(p));
            break;
        case pdf_literal_node:
            check_token_ref(pdf_literal_data(p));
            break;
        case pdf_colorstack_node:
            if (pdf_colorstack_cmd(p) <= colorstack_data)
                check_token_ref(pdf_colorstack_data(p));
            break;
        case pdf_setmatrix_node:
            check_token_ref(pdf_setmatrix_data(p));
            break;
        case late_lua_node:
            check_token_ref(late_lua_data(p));
            break;
        case pdf_annot_node:
            check_token_ref(pdf_annot_data(p));
            break;
        case pdf_start_link_node:
            if (pdf_link_attr(p) != null)
                check_token_ref(pdf_link_attr(p));
            check_action_ref(pdf_link_action(p));
            break;
        case pdf_dest_node:
            if (pdf_dest_named_id(p) > 0)
                check_token_ref(pdf_dest_id(p));
            break;
        case pdf_thread_node:
        case pdf_start_thread_node:
            if (pdf_thread_named_id(p) > 0)
                check_token_ref(pdf_thread_id(p));
            if (pdf_thread_attr(p) != null)
                check_token_ref(pdf_thread_attr(p));
            break;
        case user_defined_node:
            switch (user_node_type(p)) {
            case 'a':
                check_attribute_ref(user_node_value(p));
                break;
            case 't':
                check_token_ref(user_node_value(p));
                break;
            case 'n':
                dorangetest(p, user_node_value(p), var_mem_max);
                break;
            default:
                tconfusion("extuser");
                break;
            }
            break;
        case dir_node:
        case open_node:
        case write_node:
        case close_node:
        case pdf_save_node:
        case pdf_restore_node:
        case cancel_boundary_node:
        case close_lua_node:
        case pdf_refobj_node:
        case pdf_refxform_node:
        case pdf_refximage_node:
        case pdf_end_link_node:
        case pdf_end_thread_node:
        case pdf_save_pos_node:
        case local_par_node:
            break;
        default:
            tconfusion("ext3");
        }
        break;
    case margin_kern_node:
        check_node(margin_char(p));
        break;
    case disc_node:
        dorangetest(p, vlink(pre_break(p)), var_mem_max);
        dorangetest(p, vlink(post_break(p)), var_mem_max);
        dorangetest(p, vlink(no_break(p)), var_mem_max);
        break;
    case adjust_node:
        dorangetest(p, adjust_ptr(p), var_mem_max);
        break;
    case pseudo_file_node:
        dorangetest(p, pseudo_lines(p), var_mem_max);
        break;
    case pseudo_line_node:
    case shape_node:
        break;
    case choice_node:
        dorangetest(p, display_mlist(p), var_mem_max);
        dorangetest(p, text_mlist(p), var_mem_max);
        dorangetest(p, script_mlist(p), var_mem_max);
        dorangetest(p, script_script_mlist(p), var_mem_max);
        break;
    case fraction_noad:
        dorangetest(p, vinfo(numerator(p)), var_mem_max);
        dorangetest(p, vinfo(denominator(p)), var_mem_max);
        break;
    case rule_node:
    case kern_node:
    case math_node:
    case penalty_node:
    case mark_node:
    case style_node:
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
    case left_noad:
    case right_noad:
    case attribute_list_node:
    case attribute_node:
    case glue_spec_node:
    case temp_node:
    case align_stack_node:
    case movement_node:
    case if_node:
    case nesting_node:
    case span_node:
    case unhyphenated_node:
    case hyphenated_node:
    case delta_node:
    case passive_node:
    case expr_node:
        break;
    default:
        fprintf(stdout, "check_node: type is %d\n", type(p));
    }
}

void check_static_node_mem(void)
{
    dotest(zero_glue, width(zero_glue), 0);
    dotest(zero_glue, type(zero_glue), glue_spec_node);
    dotest(zero_glue, vlink(zero_glue), null);
    dotest(zero_glue, stretch(zero_glue), 0);
    dotest(zero_glue, stretch_order(zero_glue), normal);
    dotest(zero_glue, shrink(zero_glue), 0);
    dotest(zero_glue, shrink_order(zero_glue), normal);

    dotest(sfi_glue, width(sfi_glue), 0);
    dotest(sfi_glue, type(sfi_glue), glue_spec_node);
    dotest(sfi_glue, vlink(sfi_glue), null);
    dotest(sfi_glue, stretch(sfi_glue), 0);
    dotest(sfi_glue, stretch_order(sfi_glue), sfi);
    dotest(sfi_glue, shrink(sfi_glue), 0);
    dotest(sfi_glue, shrink_order(sfi_glue), normal);

    dotest(fil_glue, width(fil_glue), 0);
    dotest(fil_glue, type(fil_glue), glue_spec_node);
    dotest(fil_glue, vlink(fil_glue), null);
    dotest(fil_glue, stretch(fil_glue), unity);
    dotest(fil_glue, stretch_order(fil_glue), fil);
    dotest(fil_glue, shrink(fil_glue), 0);
    dotest(fil_glue, shrink_order(fil_glue), normal);

    dotest(fill_glue, width(fill_glue), 0);
    dotest(fill_glue, type(fill_glue), glue_spec_node);
    dotest(fill_glue, vlink(fill_glue), null);
    dotest(fill_glue, stretch(fill_glue), unity);
    dotest(fill_glue, stretch_order(fill_glue), fill);
    dotest(fill_glue, shrink(fill_glue), 0);
    dotest(fill_glue, shrink_order(fill_glue), normal);

    dotest(ss_glue, width(ss_glue), 0);
    dotest(ss_glue, type(ss_glue), glue_spec_node);
    dotest(ss_glue, vlink(ss_glue), null);
    dotest(ss_glue, stretch(ss_glue), unity);
    dotest(ss_glue, stretch_order(ss_glue), fil);
    dotest(ss_glue, shrink(ss_glue), unity);
    dotest(ss_glue, shrink_order(ss_glue), fil);

    dotest(fil_neg_glue, width(fil_neg_glue), 0);
    dotest(fil_neg_glue, type(fil_neg_glue), glue_spec_node);
    dotest(fil_neg_glue, vlink(fil_neg_glue), null);
    dotest(fil_neg_glue, stretch(fil_neg_glue), -unity);
    dotest(fil_neg_glue, stretch_order(fil_neg_glue), fil);
    dotest(fil_neg_glue, shrink(fil_neg_glue), 0);
    dotest(fil_neg_glue, shrink_order(fil_neg_glue), normal);
}

void check_node_mem(void)
{
    int i;
    check_static_node_mem();

    for (i = (my_prealloc + 1); i < var_mem_max; i++) {
        if (varmem_sizes[i] > 0) {
            check_node(i);
        }
    }
    test_count++;
}

void fix_node_list(halfword head)
{
    halfword p, q;
    if (head == null)
        return;
    p = head;
    q = vlink(p);
    while (q != null) {
        alink(q) = p;
        p = q;
        q = vlink(p);
    }
}

halfword get_node(integer s)
{
    register halfword r;

    /*check_static_node_mem(); */
    assert(s < MAX_CHAIN_SIZE);

    r = free_chain[s];
    if (r != null) {
        free_chain[s] = vlink(r);
#ifndef NDEBUG
        varmem_sizes[r] = s;
#endif
        vlink(r) = null;
        var_used += s;          /* maintain usage statistics */
        return r;
    }
    /* this is the end of the 'inner loop' */
    return slow_get_node(s);
}

void print_free_chain(int c)
{
    halfword p = free_chain[c];
    fprintf(stdout, "\nfree chain[%d] =\n  ", c);
    while (p != null) {
	  fprintf(stdout, "%d,", (int)p);
        p = vlink(p);
    }
    fprintf(stdout, "null;\n");
}

void free_node(halfword p, integer s)
{

    if (p <= my_prealloc) {
        fprintf(stdout, "node %d (type %d) should not be freed!\n", (int) p,
                type(p));
        return;
    }
#ifndef NDEBUG
    varmem_sizes[p] = 0;
#endif
    if (s < MAX_CHAIN_SIZE) {
        vlink(p) = free_chain[s];
        free_chain[s] = p;
    } else {
        /* todo ? it is perhaps possible to merge this node with an existing rover */
        node_size(p) = s;
        vlink(p) = rover;
        while (vlink(rover) != vlink(p)) {
            rover = vlink(rover);
        }
        vlink(rover) = p;
    }
    var_used -= s;              /* maintain statistics */
}

void free_node_chain(halfword q, integer s)
{
    register halfword p = q;
    while (vlink(p) != null) {
#ifndef NDEBUG
        varmem_sizes[p] = 0;
#endif
        var_used -= s;
        p = vlink(p);
    }
    var_used -= s;
#ifndef NDEBUG
    varmem_sizes[p] = 0;
#endif
    vlink(p) = free_chain[s];
    free_chain[s] = q;
}


void init_node_mem(halfword prealloced, halfword t)
{
    my_prealloc = prealloced;
    assert(whatsit_node_data[user_defined_node].id == user_defined_node);
    assert(node_data[passive_node].id == passive_node);

    varmem = (memory_word *) realloc((void *) varmem, sizeof(memory_word) * t);
    if (varmem == NULL) {
        overflow_string("node memory size", var_mem_max);
    }
    memset((void *) (varmem), 0, t * sizeof(memory_word));

#ifndef NDEBUG
    varmem_sizes = (char *) realloc(varmem_sizes, sizeof(char) * t);
    if (varmem_sizes == NULL) {
        overflow_string("node memory size", var_mem_max);
    }
    memset((void *) varmem_sizes, 0, sizeof(char) * t);
#endif
    var_mem_max = t;
    rover = prealloced + 1;
    vlink(rover) = rover;
    node_size(rover) = (t - rover);
    var_used = 0;
}

void dump_node_mem(void)
{
    dump_int(var_mem_max);
    dump_int(rover);
    dump_things(varmem[0], var_mem_max);
#ifndef NDEBUG
    dump_things(varmem_sizes[0], var_mem_max);
#endif
    dump_things(free_chain[0], MAX_CHAIN_SIZE);
    dump_int(var_used);
    dump_int(my_prealloc);
}

/* it makes sense to enlarge the varmem array immediately */

void undump_node_mem(void)
{
    int x;
    undump_int(x);
    undump_int(rover);
    var_mem_max = (x < 100000 ? 100000 : x);
    varmem = xmallocarray(memory_word, var_mem_max);
    /*memset ((void *)varmem,0,x*sizeof(memory_word)); */
    undump_things(varmem[0], x);
#ifndef NDEBUG
    varmem_sizes = xmallocarray(char, var_mem_max);
    memset((void *) varmem_sizes, 0, var_mem_max * sizeof(char));
    undump_things(varmem_sizes[0], x);
#endif
    undump_things(free_chain[0], MAX_CHAIN_SIZE);
    undump_int(var_used);
    undump_int(my_prealloc);
    if (var_mem_max > x) {
        /* todo ? it is perhaps possible to merge the new node with an existing rover */
        vlink(x) = rover;
        node_size(x) = (var_mem_max - x);
        while (vlink(rover) != vlink(x)) {
            rover = vlink(rover);
        }
        vlink(rover) = x;
    }
}

#if 0
void test_rovers(char *s)
{
    int q = rover;
    int r = q;
    fprintf(stdout, "%s: {rover=%d,size=%d,link=%d}", s, r, node_size(r),
            vlink(r));
    while (vlink(r) != q) {
        r = vlink(r);
        fprintf(stdout, ",{rover=%d,size=%d,link=%d}", r, node_size(r),
                vlink(r));
    }
    fprintf(stdout, "\n");
}
#else
#  define test_rovers(a)
#endif

halfword slow_get_node(integer s)
{
    register int t;

  RETRY:
    t = node_size(rover);
    assert(vlink(rover) < var_mem_max);
    assert(vlink(rover) != 0);
    test_rovers("entry");
    if (t > s) {
        register halfword r;
        /* allocating from the bottom helps decrease page faults */
        r = rover;
        rover += s;
        vlink(rover) = vlink(r);
        node_size(rover) = node_size(r) - s;
        if (vlink(rover) != r) {        /* list is longer than one */
            halfword q = r;
            while (vlink(q) != r) {
                q = vlink(q);
            }
            vlink(q) += s;
        } else {
            vlink(rover) += s;
        }
        test_rovers("taken");
        assert(vlink(rover) < var_mem_max);
#ifndef NDEBUG
        varmem_sizes[r] = (s > 127 ? 127 : s);
#endif
        vlink(r) = null;
        var_used += s;          /* maintain usage statistics */
        return r;               /* this is the only exit */
    } else {
        int x;
        /* attempt to keep the free list small */
        if (vlink(rover) != rover) {
            if (t < MAX_CHAIN_SIZE) {
                halfword l = vlink(rover);
                vlink(rover) = free_chain[t];
                free_chain[t] = rover;
                rover = l;
                while (vlink(l) != free_chain[t]) {
                    l = vlink(l);
                }
                vlink(l) = rover;
                test_rovers("outtake");
                goto RETRY;
            } else {
                halfword l = rover;
                while (vlink(rover) != l) {
                    if (node_size(rover) > s) {
                        goto RETRY;
                    }
                    rover = vlink(rover);
                }
            }
        }
        /* if we are still here, it was apparently impossible to get a match */
        x = (var_mem_max >> 2) + s;
        varmem =
            (memory_word *) realloc((void *) varmem,
                                    sizeof(memory_word) * (var_mem_max + x));
        if (varmem == NULL) {
            overflow_string("node memory size", var_mem_max);
        }
        memset((void *) (varmem + var_mem_max), 0, x * sizeof(memory_word));

#ifndef NDEBUG
        varmem_sizes =
            (char *) realloc(varmem_sizes, sizeof(char) * (var_mem_max + x));
        if (varmem_sizes == NULL) {
            overflow_string("node memory size", var_mem_max);
        }
        memset((void *) (varmem_sizes + var_mem_max), 0, x * sizeof(char));
#endif

        /* todo ? it is perhaps possible to merge the new memory with an existing rover */
        vlink(var_mem_max) = rover;
        node_size(var_mem_max) = x;
        while (vlink(rover) != vlink(var_mem_max)) {
            rover = vlink(rover);
        }
        vlink(rover) = var_mem_max;
        rover = var_mem_max;
        test_rovers("realloc");
        var_mem_max += x;
        goto RETRY;
    }
}

char *sprint_node_mem_usage(void)
{
    int i, b;

    char *s, *ss;
    char msg[256];
    int node_counts[last_normal_node + last_whatsit_node + 2] = { 0 };

    for (i = (var_mem_max - 1); i > my_prealloc; i--) {
        if (varmem_sizes[i] > 0) {
            if (type(i) > last_normal_node + last_whatsit_node) {
                node_counts[last_normal_node + last_whatsit_node + 1]++;
            } else if (type(i) == whatsit_node) {
                node_counts[(subtype(i) + last_normal_node + 1)]++;
            } else {
                node_counts[type(i)]++;
            }
        }
    }
    s = strdup("");
    b = 0;
    for (i = 0; i < last_normal_node + last_whatsit_node + 2; i++) {
        if (node_counts[i] > 0) {
            int j =
                (i > (last_normal_node + 1) ? (i - last_normal_node - 1) : 0);
            snprintf(msg, 255, "%s%d %s", (b ? ", " : ""), (int) node_counts[i],
                     get_node_name((i > last_normal_node ? whatsit_node : i),
                                   j));
            ss = concat(s, msg);
            free(s);
            s = ss;
            b = 1;
        }
    }
    return s;
}

halfword list_node_mem_usage(void)
{
    halfword i, j;
    halfword p = null, q = null;
    char *saved_varmem_sizes = xmallocarray(char, var_mem_max);
    memcpy(saved_varmem_sizes, varmem_sizes, var_mem_max);
    for (i = my_prealloc + 1; i < (var_mem_max - 1); i++) {
        if (saved_varmem_sizes[i] > 0) {
            j = copy_node(i);
            if (p == null) {
                q = j;
            } else {
                vlink(p) = j;
            }
            p = j;
        }
    }
    free(saved_varmem_sizes);
    return q;
}

void print_node_mem_stats(int num, int online)
{
    int i, b;
    halfword j;
    char msg[256];
    char *s;
    integer free_chain_counts[MAX_CHAIN_SIZE] = { 0 };

    snprintf(msg, 255, "node memory in use: %d words out of %d",
             (int) (var_used + my_prealloc), (int) var_mem_max);
    tprint_nl(msg);
    tprint_nl("rapidly available: ");
    b = 0;
    for (i = 1; i < MAX_CHAIN_SIZE; i++) {
        for (j = free_chain[i]; j != null; j = vlink(j))
            free_chain_counts[i]++;
        if (free_chain_counts[i] > 0) {
            snprintf(msg, 255, "%s%d:%d", (b ? ", " : ""), i,
                     (int) free_chain_counts[i]);
            tprint(msg);
            b = 1;
        }
    }
    tprint(" nodes");
    s = sprint_node_mem_usage();
    tprint_nl("current usage: ");
    tprint(s);
    free(s);
    tprint(" nodes");
    print_nlp();                /* newline, if needed */
}

/* this belongs in the web but i couldn't find the correct syntactic place */

halfword new_span_node(halfword n, int s, scaled w)
{
    halfword p = new_node(span_node, 0);
    span_link(p) = n;
    span_span(p) = s;
    width(p) = w;
    return p;
}

halfword string_to_pseudo(integer l, integer pool_ptr, integer nl)
{
    halfword i, r, q = null;
    four_quarters w;
    int sz;
    halfword h = new_node(pseudo_file_node, 0);
    while (l < pool_ptr) {
        int m = l;
        while ((l < pool_ptr) && (str_pool[l] != nl))
            l++;
        sz = (l - m + 7) / 4;
        if (sz == 1)
            sz = 2;
        r = new_node(pseudo_line_node, sz);
        i = r;
        while (--sz > 1) {
            w.b0 = str_pool[m++];
            w.b1 = str_pool[m++];
            w.b2 = str_pool[m++];
            w.b3 = str_pool[m++];
            varmem[++i].qqqq = w;
        }
        w.b0 = (l > m ? str_pool[m++] : ' ');
        w.b1 = (l > m ? str_pool[m++] : ' ');
        w.b2 = (l > m ? str_pool[m++] : ' ');
        w.b3 = (l > m ? str_pool[m] : ' ');
        varmem[++i].qqqq = w;
        if (pseudo_lines(h) == null) {
            pseudo_lines(h) = r;
            q = r;
        } else {
            couple_nodes(q, r);
        }
        q = vlink(q);
        if (str_pool[l] == nl)
            l++;
    }
    return h;
}

/* attribute stuff */

static halfword new_attribute_node(unsigned int i, int v)
{
    register halfword r = get_node(attribute_node_size);
    type(r) = attribute_node;
    attribute_id(r) = i;
    attribute_value(r) = v;
    return r;
}

halfword copy_attribute_list(halfword n)
{
    halfword q = get_node(attribute_node_size);
    register halfword p = q;
    type(p) = attribute_list_node;
    attr_list_ref(p) = 0;
    n = vlink(n);
    while (n != null) {
        register halfword r = get_node(attribute_node_size);
        /* the link will be fixed automatically in the next loop */
        (void) memcpy((void *) (varmem + r), (void *) (varmem + n),
                      (sizeof(memory_word) * attribute_node_size));
        vlink(p) = r;
        p = r;
        n = vlink(n);
    }
    return q;
}

void update_attribute_cache(void)
{
    halfword p;
    register int i;
    attr_list_cache = get_node(attribute_node_size);
    type(attr_list_cache) = attribute_list_node;
    attr_list_ref(attr_list_cache) = 0;
    p = attr_list_cache;
    for (i = 0; i <= max_used_attr; i++) {
        register int v = get_attribute(i);
        if (v >= 0) {
            register halfword r = new_attribute_node(i, v);
            vlink(p) = r;
            p = r;
        }
    }
    if (vlink(attr_list_cache) == null) {
        free_node(attr_list_cache, attribute_node_size);
        attr_list_cache = null;
    }
    return;
}

void build_attribute_list(halfword b)
{
    if (max_used_attr >= 0) {
        if (attr_list_cache == cache_disabled) {
            update_attribute_cache();
            if (attr_list_cache == null)
                return;
        }
        attr_list_ref(attr_list_cache)++;
        node_attr(b) = attr_list_cache;
    }
}

void delete_attribute_ref(halfword b)
{
    if (b != null) {
        assert(type(b) == attribute_list_node);
        attr_list_ref(b)--;
        if (attr_list_ref(b) == 0) {
            if (b == attr_list_cache)
                attr_list_cache = cache_disabled;
            free_node_chain(b, attribute_node_size);
        }
        /* maintain sanity */
        if (attr_list_ref(b) < 0)
            attr_list_ref(b) = 0;
    }
}

/* |p| is an attr list head, or zero */

halfword do_set_attribute(halfword p, int i, int val)
{
    register halfword q;
    register int j = 0;
    if (p == null) {            /* add a new head & node */
        q = get_node(attribute_node_size);
        type(q) = attribute_list_node;
        attr_list_ref(q) = 1;
        p = new_attribute_node(i, val);
        vlink(q) = p;
        return q;
    }

    assert(vlink(p) != null);
    while (vlink(p) != null) {
        int t = attribute_id(vlink(p));
        if (t == i && attribute_value(vlink(p)) == val)
            return q;           /* no need to do anything */
        if (t >= i)
            break;
        j++;
        p = vlink(p);
    }

    p = q;
    while (j-- > 0)
        p = vlink(p);
    if (attribute_id(vlink(p)) == i) {
        attribute_value(vlink(p)) = val;
    } else {                    /* add a new node */
        halfword r = new_attribute_node(i, val);
        vlink(r) = vlink(p);
        vlink(p) = r;
    }
    return q;
}

void set_attribute(halfword n, int i, int val)
{
    register halfword p;
    register int j = 0;
    if (!nodetype_has_attributes(type(n)))
        return;
    p = node_attr(n);
    if (p == null) {            /* add a new head & node */
        p = get_node(attribute_node_size);
        type(p) = attribute_list_node;
        attr_list_ref(p) = 1;
        node_attr(n) = p;
        p = new_attribute_node(i, val);
        vlink(node_attr(n)) = p;
        return;
    }
    assert(vlink(p) != null);
    while (vlink(p) != null) {
        int t = attribute_id(vlink(p));
        if (t == i && attribute_value(vlink(p)) == val)
            return;
        if (t >= i)
            break;
        j++;
        p = vlink(p);
    }
    p = node_attr(n);
    if (attr_list_ref(p) != 1) {
        if (attr_list_ref(p) > 1) {
            p = copy_attribute_list(p);
            delete_attribute_ref(node_attr(n));
            node_attr(n) = p;
        } else {
            fprintf(stdout,
                    "Node %d has an attribute list that is free already\n",
                    (int) n);
        }
        attr_list_ref(p) = 1;
    }
    while (j-- > 0)
        p = vlink(p);

    if (attribute_id(vlink(p)) == i) {
        attribute_value(vlink(p)) = val;
    } else {                    /* add a new node */
        halfword r = new_attribute_node(i, val);
        vlink(r) = vlink(p);
        vlink(p) = r;
    }
    return;
}


int unset_attribute(halfword n, int i, int val)
{
    register halfword p;
    register int t;
    register int j = 0;

    if (!nodetype_has_attributes(type(n)))
        return null;
    p = node_attr(n);
    if (p == null)
        return -1;
    assert(vlink(p) != null);
    while (vlink(p) != null) {
        t = attribute_id(vlink(p));
        if (t > i)
            return -1;
        if (t == i)
            break;
        j++;
        p = vlink(p);
    }
    /* if we are still here, the attribute exists */
    p = node_attr(n);
    if (attr_list_ref(p) != 1) {
        if (attr_list_ref(p) > 1) {
            p = copy_attribute_list(p);
            delete_attribute_ref(node_attr(n));
            node_attr(n) = p;
        } else {
            fprintf(stdout,
                    "Node %d has an attribute list that is free already\n",
                    (int) n);
        }
        attr_list_ref(p) = 1;
    }
    p = vlink(p);
    while (j-- > 0)
        p = vlink(p);
    t = attribute_value(p);
    if (val == -1 || t == val) {
        attribute_value(p) = -1;
    }
    return t;
}

int has_attribute(halfword n, int i, int val)
{
    register halfword p;
    if (!nodetype_has_attributes(type(n)))
        return -1;
    p = node_attr(n);
    if (p == null || vlink(p) == null)
        return -1;
    p = vlink(p);
    while (p != null) {
        if (attribute_id(p) == i) {
            int ret = attribute_value(p);
            if (val == -1 || val == ret)
                return ret;
            return -1;
        } else if (attribute_id(p) > i) {
            return -1;
        }
        p = vlink(p);
    }
    return -1;
}
