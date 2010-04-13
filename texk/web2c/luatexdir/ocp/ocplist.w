% ocplist.w
% 
% Copyright 2009-2010 Taco Hoekwater <taco@@luatex.org>

% This file is part of LuaTeX.

% LuaTeX is free software; you can redistribute it and/or modify it under
% the terms of the GNU General Public License as published by the Free
% Software Foundation; either version 2 of the License, or (at your
% option) any later version.

% LuaTeX is distributed in the hope that it will be useful, but WITHOUT
% ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
% FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
% License for more details.

% You should have received a copy of the GNU General Public License along
% with LuaTeX; if not, see <http://www.gnu.org/licenses/>. 

@ @c
#include "ptexlib.h"

static const char _svn_version[] =
    "$Id: ocplist.w 3584 2010-04-02 17:45:55Z hhenkel $ "
"$URL: http://foundry.supelec.fr/svn/luatex/branches/0.60.x/source/texk/web2c/luatexdir/ocp/ocplist.w $";

@ @c
memory_word *ocp_list_info;     /* the big collection of ocp list data */
ocp_list_index ocp_listmem_ptr; /* first unused word of |ocp_list_info| */
ocp_list_index ocp_listmem_run_ptr;     /* temp unused word of |ocp_list_info| */
memory_word *ocp_lstack_info;   /* the big collection of ocp lstack data */
ocp_lstack_index ocp_lstackmem_ptr;     /* first unused word of |ocp_lstack_info| */
ocp_lstack_index ocp_lstackmem_run_ptr; /* temp unused word of |ocp_lstack_info| */
internal_ocp_list_number ocp_list_ptr;  /* largest internal ocp list number in use */
ocp_list_index *ocp_list_list;

#define ocp_list_id_text(A) cs_text(ocp_list_id_base+(A))

void initialize_init_ocplists(void)
{
    ocp_listmem_ptr = 2;
    ocp_list_lstack(0) = 0;
    ocp_list_lstack_no(0) = ocp_maxint;
    ocp_list_lnext(0) = 0;
    ocp_list_ptr = null_ocp_list;
    ocp_list_list[null_ocp_list] = 0;
    ocp_lstackmem_ptr = 1;
}


void initialize_ocplist_arrays(int ocp_list_size)
{
    ocp_list_info = xmallocarray(memory_word, (unsigned) ocp_list_size);
    memset(ocp_list_info, 0, sizeof(memory_word) * (unsigned) ocp_list_size);
    ocp_lstack_info = xmallocarray(memory_word, (unsigned) ocp_list_size);
    memset(ocp_lstack_info, 0, sizeof(memory_word) * (unsigned) ocp_list_size);
    ocp_list_list = xmallocarray(ocp_list_index, (unsigned) ocp_list_size);
}


@ @c
ocp_list_index make_ocp_list_node(ocp_lstack_index llstack,
                                  scaled llstack_no, ocp_list_index llnext)
{
    ocp_list_index p;
    p = ocp_listmem_run_ptr;
    ocp_list_lstack(p) = (quarterword) llstack;
    ocp_list_lstack_no(p) = llstack_no;
    ocp_list_lnext(p) = (quarterword) llnext;
    ocp_listmem_run_ptr = ocp_listmem_run_ptr + 2;
    if (ocp_listmem_run_ptr >= ocp_list_size)
        overflow("ocp_list_size", (unsigned) ocp_list_size);
    return p;
}

ocp_lstack_index make_ocp_lstack_node(internal_ocp_number locp,
                                      ocp_lstack_index llnext)
{
    ocp_lstack_index p;
    p = ocp_lstackmem_run_ptr;
    ocp_lstack_ocp(p) = (quarterword) locp;
    ocp_lstack_lnext(p) = (quarterword) llnext;
    incr(ocp_lstackmem_run_ptr);
    if (ocp_lstackmem_run_ptr >= ocp_stack_size)
        overflow("ocp_stack_size", (unsigned) ocp_stack_size);
    return p;
}

ocp_lstack_index copy_ocp_lstack(ocp_lstack_index llstack)
{
    if (is_null_ocp_lstack(llstack))
        return make_null_ocp_lstack();
    else
        return make_ocp_lstack_node(ocp_lstack_ocp(llstack),
                                    copy_ocp_lstack(ocp_lstack_lnext(llstack)));
}

ocp_list_index copy_ocp_list(ocp_list_index list)
{
    if (is_null_ocp_list(list))
        return make_null_ocp_list();
    else
        return make_ocp_list_node(copy_ocp_lstack(ocp_list_lstack(list)),
                                  ocp_list_lstack_no(list),
                                  copy_ocp_list(ocp_list_lnext(list)));
}

ocp_list_index ocp_ensure_lstack(ocp_list_index list, scaled llstack_no)
{
    ocp_list_index p;
    ocp_list_index q;
    p = list;
    if (is_null_ocp_list(p)) {
        ocp_list_lstack_no(p) = llstack_no;
        ocp_list_lnext(p) = (quarterword) make_null_ocp_list();
    } else if (ocp_list_lstack_no(p) > llstack_no) {
        ocp_list_lnext(p) = (quarterword)
            make_ocp_list_node(ocp_list_lstack(p),
                               ocp_list_lstack_no(p), ocp_list_lnext(p));
        ocp_list_lstack(p) = 0;
        ocp_list_lstack_no(p) = llstack_no;
    } else {
        q = ocp_list_lnext(p);
        while ((!(is_null_ocp_list(q))) && ocp_list_lstack_no(q) <= llstack_no) {
            p = q;
            q = ocp_list_lnext(q);
        }
        if (ocp_list_lstack_no(p) < llstack_no) {
            ocp_list_lnext(p) =
                (quarterword) make_ocp_list_node(0, llstack_no, q);
            p = ocp_list_lnext(p);
        }
    }
    return p;
}

void ocp_apply_add(ocp_list_index list_entry,
                   boolean lbefore, internal_ocp_number locp)
{
    ocp_lstack_index p;
    ocp_lstack_index q;
    p = ocp_list_lstack(list_entry);
    if (lbefore || (p == 0)) {
        ocp_list_lstack(list_entry) =
            (quarterword) make_ocp_lstack_node(locp, p);
    } else {
        q = ocp_lstack_lnext(p);
        while (q != 0) {
            p = q;
            q = ocp_lstack_lnext(q);
        }
        ocp_lstack_lnext(p) = (quarterword) make_ocp_lstack_node(locp, 0);
    }
}

void ocp_apply_remove(ocp_list_index list_entry, boolean lbefore)
{
    ocp_lstack_index p;
    ocp_lstack_index q;
    ocp_lstack_index r;
    p = ocp_list_lstack(list_entry);
    if (p == 0) {
        print_err("warning: stack entry already empty");
        print_ln();
    } else {
        q = ocp_lstack_lnext(p);
        if (lbefore || (q == 0)) {
            ocp_list_lstack(list_entry) = (quarterword) q;
        } else {
            r = ocp_lstack_lnext(q);
            while (r != 0) {
                p = q;
                q = r;
                r = ocp_lstack_lnext(r);
            }
            ocp_lstack_lnext(p) = 0;
        }
    }
}


void print_ocp_lstack(ocp_lstack_index lstack_entry)
{
    ocp_lstack_index p;
    p = lstack_entry;
    while (p != 0) {
        print_esc(cs_text(ocp_id_base + ocp_lstack_ocp(p)));
        p = ocp_lstack_lnext(p);
        if (p != 0)
            print_char(',');
    }
}

void print_ocp_list(ocp_list_index list_entry)
{
    ocp_list_index p;
    print_char('[');
    p = list_entry;
    while (!(is_null_ocp_list(p))) {
        print_char('(');
        print_scaled(ocp_list_lstack_no(p));
        tprint(" : ");
        print_ocp_lstack(ocp_list_lstack(p));
        print_char(')');
        p = ocp_list_lnext(p);
        if (!(is_null_ocp_list(p)))
            tprint(", ");
    }
    print_char(']');
}

@ @c
ocp_list_index scan_ocp_list(void)
{
    scaled llstack_no;
    quarterword lop;
    ocp_list_index lstack_entry;
    ocp_list_index other_list;
    internal_ocp_number ocp_ident = 0;
    get_r_token();
    if (cur_cmd == set_ocp_list_cmd) {
        return copy_ocp_list(ocp_list_list[cur_chr]);
    } else if (cur_cmd != ocp_list_op_cmd) {
        const char *hlp[] =
            { "I was looking for a ocp list specification.", NULL };
        tex_error("Bad ocp list specification", hlp);
        return make_null_ocp_list();
    } else {
        lop = (quarterword) cur_chr;
        scan_scaled();
        llstack_no = cur_val;
        if ((llstack_no <= 0) || (llstack_no >= ocp_maxint)) {
            tex_error("Stack numbers must be between 0 and 4096 (exclusive)",
                      NULL);
            return make_null_ocp_list();
        } else {
            if (lop <= add_after_op) {
                scan_ocp_ident();
                ocp_ident = cur_val;
            }
            other_list = scan_ocp_list();
            lstack_entry = ocp_ensure_lstack(other_list, llstack_no);
            if (lop <= add_after_op) {
                ocp_apply_add(lstack_entry, (lop = add_before_op), ocp_ident);
            } else {
                ocp_apply_remove(lstack_entry, (lop = remove_before_op));
            }
            return other_list;
        }
    }
}

@ @c
internal_ocp_list_number read_ocp_list(void)
{
    internal_ocp_list_number f;
    internal_ocp_list_number g;
    g = null_ocp_list;
    f = ocp_list_ptr + 1;
    ocp_listmem_run_ptr = ocp_listmem_ptr;
    ocp_lstackmem_run_ptr = ocp_lstackmem_ptr;
    ocp_list_list[f] = scan_ocp_list();
    ocp_list_ptr = f;
    ocp_listmem_ptr = ocp_listmem_run_ptr;
    ocp_lstackmem_ptr = ocp_lstackmem_run_ptr;
    g = f;
    return g;
}

@ @c
void scan_ocp_list_ident(void)
{
    internal_ocp_list_number f;
    do {
        get_x_token();
    } while (cur_cmd == spacer_cmd);

    if (cur_cmd == set_ocp_list_cmd) {
        f = cur_chr;
    } else {
        const char *hlp[] = { "I was looking for a control sequence whose",
            "current meaning has been defined by \\ocplist.",
            NULL
        };
        back_input();
        tex_error("Missing ocp list identifier", hlp);
        f = null_ocp_list;
    }
    cur_val = f;
}

@ @c
void new_ocp_list(small_number a)
{
    pointer u;                  /* user's ocp list identifier */
    internal_ocp_list_number f; /* runs through existing ocp lists */
    str_number t;               /* name for the frozen ocp list identifier */
    if (job_name == 0)
        open_log_file();
    /* avoid confusing \.{texput} with the ocp list name */
    get_r_token();
    u = cur_cs;
    if (u >= hash_base)
        t = cs_text(u);
    else
        t = maketexstring("OCPLIST");
    define(u, set_ocp_list_cmd, null_ocp_list);
    scan_optional_equals();
    f = read_ocp_list();
    equiv(u) = f;
    eqtb[ocp_list_id_base + f] = eqtb[u];
    cs_text(ocp_list_id_base + f) = t;
    if (ocp_trace_level == 1) {
        tprint_nl("");
        tprint_esc("ocplist");
        print_esc(t);
        tprint("=");
        print_ocp_list(ocp_list_list[f]);
    }
}

@ @c
void dump_ocplist_info(void)
{
    int k;
    dump_int(ocp_listmem_ptr);
    for (k = 0; k <= ocp_listmem_ptr - 1; k++)
        dump_wd(ocp_list_info[k]);
    dump_int(ocp_list_ptr);
    for (k = null_ocp_list; k <= ocp_list_ptr; k++) {
        dump_int(ocp_list_list[k]);
        if (null_ocp_list != ocp_list_ptr) {
            tprint_nl("\\ocplist");
            print_esc(ocp_list_id_text(k));
            print_char('=');
            print_ocp_list(ocp_list_list[k]);
        }
    }
    dump_int(ocp_lstackmem_ptr);
    for (k = 0; k <= ocp_lstackmem_ptr - 1; k++)
        dump_wd(ocp_lstack_info[k]);
}

void undump_ocplist_info(void)
{
    int k;
    initialize_ocplist_arrays(ocp_list_size);
    undump_int(ocp_listmem_ptr);
    for (k = 0; k <= ocp_listmem_ptr - 1; k++)
        undump_wd(ocp_list_info[k]);
    undump_int(ocp_list_ptr);
    for (k = null_ocp_list; k <= ocp_list_ptr; k++)
        undump_int(ocp_list_list[k]);
    undump_int(ocp_lstackmem_ptr);
    for (k = 0; k <= ocp_lstackmem_ptr - 1; k++)
        undump_wd(ocp_lstack_info[k]);
}
