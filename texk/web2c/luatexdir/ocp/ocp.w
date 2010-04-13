% ocp.w
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
    "$Id: ocp.w 3584 2010-04-02 17:45:55Z hhenkel $ "
"$URL: http://foundry.supelec.fr/svn/luatex/branches/0.60.x/source/texk/web2c/luatexdir/ocp/ocp.w $";

@ @c
int **ocp_tables;

static int ocp_entries = 0;
int ocp_maxint = 10000000;

@ When the user defines \.{\\ocp\\f}, say, \TeX\ assigns an internal number
to the user's ocp~\.{\\f}. Adding this number to |ocp_id_base| gives the
|eqtb| location of a ``frozen'' control sequence that will always select
the ocp.

@c
internal_ocp_number ocp_ptr;    /* largest internal ocp number in use */

void new_ocp(small_number a)
{
    pointer u;                  /* user's ocp identifier */
    internal_ocp_number f;      /* runs through existing ocps */
    str_number t;               /* name for the frozen ocp identifier */
    boolean external_ocp = false;       /* external binary file */
    if (job_name == 0)
        open_log_file();
    /* avoid confusing \.{texput} with the ocp name */
    if (cur_chr == 1)
        external_ocp = true;
    get_r_token();
    u = cur_cs;
    if (u >= hash_base)
        t = cs_text(u);
    else
        t = maketexstring("OCP");
    define(u, set_ocp_cmd, null_ocp);
    scan_optional_equals();
    scan_file_name();
    /* If this ocp has already been loaded, set |f| to the internal
       ocp number and |goto common_ending| */
    /* When the user gives a new identifier to a ocp that was previously loaded,
       the new name becomes the ocp identifier of record. OCP names `\.{xyz}' and
       `\.{XYZ}' are considered to be different.
     */
    for (f = ocp_base + 1; f <= ocp_ptr; f++) {
        if (str_eq_str(ocp_name(f), cur_name)
            && str_eq_str(ocp_area(f), cur_area)) {
            flush_str(cur_name);
            flush_str(cur_area);
            cur_name = ocp_name(f);
            cur_area = ocp_area(f);
            goto COMMON_ENDING;
        }
    }
    {
        char *nam = makecstring(cur_name);
        char *are = makecstring(cur_area);
        char *ext = makecstring(cur_ext);
        f = read_ocp_info(u, nam, are, ext, external_ocp);
        free(nam);
        free(are);
        free(ext);
    }
  COMMON_ENDING:
    equiv(u) = f;
    eqtb[ocp_id_base + f] = eqtb[u];
    cs_text(ocp_id_base + f) = t;
    if (ocp_trace_level == 1) {
        tprint_nl("");
        tprint_esc("ocp");
        print_esc(t);
        tprint("=");
        print(cur_name);
    }
}

@ Before we forget about the format of these tables, let's deal with
$\Omega$'s basic scanning routine related to ocp information. 

@c
void scan_ocp_ident(void)
{
    internal_ocp_number f;
    do {
        get_x_token();
    } while (cur_cmd == spacer_cmd);

    if (cur_cmd == set_ocp_cmd) {
        f = cur_chr;
    } else {
        const char *hlp[] = { "I was looking for a control sequence whose",
            "current meaning has been defined by \\ocp.",
            NULL
        };
        back_input();
        tex_error("Missing ocp identifier", hlp);
        f = null_ocp;
    }
    cur_val = f;
}

void allocate_ocp_table(int ocp_number, int ocp_size)
{
    int i;
    if (ocp_entries == 0) {
        ocp_tables = (int **) xmalloc(256 * sizeof(int **));
        ocp_entries = 256;
    } else if ((ocp_number == 256) && (ocp_entries == 256)) {
        ocp_tables = xrealloc(ocp_tables, 65536);
        ocp_entries = 65536;
    }
    ocp_tables[ocp_number] =
        (int *) xmalloc((unsigned) ((1 + (unsigned) ocp_size) * sizeof(int)));
    ocp_tables[ocp_number][0] = ocp_size;
    for (i = 1; i <= ocp_size; i++) {
        ocp_tables[ocp_number][i] = 0;
    }
}

@ @c
static void dump_ocp_table(int ocp_number)
{
    dump_things(ocp_tables[ocp_number][0], ocp_tables[ocp_number][0] + 1);
}

void dump_ocp_info(void)
{
    int k;
    dump_int(123456);
    dump_int(ocp_ptr);
    for (k = null_ocp; k <= ocp_ptr; k++) {
        dump_ocp_table(k);
        if (ocp_ptr - ocp_base > 0) {
            tprint_nl("\\ocp");
            print_esc(cs_text(ocp_id_base + k));
            print_char('=');
            print_file_name(ocp_name(k), ocp_area(k), get_nullstr());
        }
    }
    dump_int(123456);
    if (ocp_ptr - ocp_base > 0) {
        print_ln();
        print_int(ocp_ptr - ocp_base);
        tprint(" preloaded ocp");
        if (ocp_ptr != ocp_base + 1)
            print_char('s');
    }
}

static void undump_ocp_table(int ocp_number)
{
    int sizeword;
    if (ocp_entries == 0) {
        ocp_tables = (int **) xmalloc(256 * sizeof(int **));
        ocp_entries = 256;
    } else if ((ocp_number == 256) && (ocp_entries == 256)) {
        ocp_tables = xrealloc(ocp_tables, 65536);
        ocp_entries = 65536;
    }
    undump_things(sizeword, 1);
    ocp_tables[ocp_number] =
        (int *) xmalloc((unsigned) ((1 + (unsigned) sizeword) * sizeof(int)));
    ocp_tables[ocp_number][0] = sizeword;
    undump_things(ocp_tables[ocp_number][1], sizeword);
}


void undump_ocp_info(void)
{
    int k;
    int x;
    undump_int(x);
    assert(x == 123456);
    undump_int(ocp_ptr);
    for (k = null_ocp; k <= ocp_ptr; k++)
        undump_ocp_table(k);
    undump_int(x);
    assert(x == 123456);
}
