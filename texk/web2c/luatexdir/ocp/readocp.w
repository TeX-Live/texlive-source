% readocp.w
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
    "$Id: readocp.w 3584 2010-04-02 17:45:55Z hhenkel $ "
"$URL: http://foundry.supelec.fr/svn/luatex/branches/0.60.x/source/texk/web2c/luatexdir/ocp/readocp.w $";

@ @c
static char *b_test_in(char *nam)
{
    if (program_name_set) {
        return (char *) kpse_find_file(nam, kpse_program_binary_format, true);
    }
    return NULL;
}


static unsigned char *ocp_buffer = NULL;        /* byte buffer for ocp files */
static int ocp_size = 0;        /* total size of the ocp file */
static int ocp_cur = 0;         /* index into |ocp_buffer| */

@ @c
void init_null_ocp(str_number a, str_number n)
{
    ocp_ptr = null_ocp;
    allocate_ocp_table(null_ocp, 17);
    ocp_file_size(null_ocp) = 17;
    ocp_name(null_ocp) = n;
    ocp_area(null_ocp) = a;
    ocp_external(null_ocp) = 0;
    ocp_external_arg(null_ocp) = 0;
    ocp_input(null_ocp) = 1;
    ocp_output(null_ocp) = 1;
    ocp_no_tables(null_ocp) = 0;
    ocp_no_states(null_ocp) = 1;
    ocp_table_base(f) = offset_ocp_info;
    ocp_state_base(f) = offset_ocp_info;
    ocp_info(null_ocp, offset_ocp_info) = offset_ocp_info + 2;  /* number of entries */
    ocp_info(null_ocp, offset_ocp_info + 1) = offset_ocp_info + 5;      /* number of entries */
    ocp_info(null_ocp, offset_ocp_info + 2) = 23;       /* |OTP_LEFT_START| */
    ocp_info(null_ocp, offset_ocp_info + 3) = 3;        /* |OTP_RIGHT_CHAR| */
    ocp_info(null_ocp, offset_ocp_info + 4) = 36;       /* |OTP_STOP| */
}



@ $\Omega$ checks the information of a \.{OCP} file for validity as the
file is being read in, so that no further checks will be needed when
typesetting is going on. The somewhat tedious subroutine that does this
is called |read_ocp_info|. It has three parameters: the user ocp
identifier~|u|, and the file name and area strings |nom| and |aire|.

The subroutine opens and closes a global file variable called |ocp_file|.
It returns the value of the internal ocp number that was just loaded.
If an error is detected, an error message is issued and no ocp
information is stored; |null_ocp| is returned in this case.

@c
/* do this when the \.{OCP} data is wrong */
#define ocp_abort(A) do {				\
    tprint("OCP file error (");				\
    tprint(A); tprint(")"); print_ln();			\
    goto BAD_OCP;					\
} while (0)

/* macros for reading and storing ocp data */

#define add_to_ocp_info(A) ocp_tables[f][ocpmem_run_ptr++]=(A)
#define ocp_read(A) do {						\
    ocpword=ocp_buffer[ocp_cur++];					\
    if (ocpword>127) ocp_abort("checking first octet");			\
    ocpword=ocpword*0400+ocp_buffer[ocp_cur++];				\
    ocpword=ocpword*0400+ocp_buffer[ocp_cur++];				\
    ocpword=ocpword*0400+ocp_buffer[ocp_cur++];				\
    (A)=ocpword;							\
  } while (0)

#define ocp_read_all(A) ocp_read(A)
#define ocp_read_info do { ocp_read_all(ocpword); add_to_ocp_info(ocpword); } while (0)



@ Input a \.{OCP} file

@c
internal_ocp_number
read_ocp_info(pointer u, char *nom, char *aire, char *ext, boolean external_ocp)
{
    boolean file_opened;        /* was |ocp_file| successfully opened? */
    boolean res;
    int callback_id;
    char *cnam;                 /* C version of file name */
    internal_ocp_number f;      /* the new ocp's number */
    internal_ocp_number g;      /* the number to return */
    int ocpword;
    ocp_index ocpmem_run_ptr;
    int ocp_length, real_ocp_length;    /* length of ocp file */
    ocp_index previous_address;
    int temp_ocp_input;
    int temp_ocp_output;
    int temp_ocp_no_tables;
    int temp_ocp_no_states;
    int i, new_offset, room_for_tables, room_for_states;
    g = null_ocp;
    f = null_ocp;
    cnam = NULL;
    file_opened = false;
    if (external_ocp) {
        /*  Check |ocp_file| exists */
        cnam =
            xmalloc((unsigned) (strlen(nom) + strlen(aire) + strlen(ext) + 1));
        sprintf(cnam, "%s%s%s", aire, nom, ext);
        cnam = b_test_in(cnam);
        if (!cnam)
            ocp_abort("opening file");
        f = ocp_ptr + 1;
        allocate_ocp_table(f, 13);
        ocp_file_size(f) = 13;
        ocp_external(f) = maketexstring(cnam);
        scan_string_argument();
        ocp_external_arg(f) = cur_val;
        ocp_name(f) = get_nullstr();
        ocp_area(f) = get_nullstr();
        ocp_state_base(f) = 0;
        ocp_table_base(f) = 0;
        ocp_input(f) = 1;
        ocp_output(f) = 1;
        ocp_info(f, offset_ocp_info) = 0;
        ocp_ptr = f;
        g = f;
        goto DONE;

    } else {
        /* Open |ocp_file| for input */
        char *cname = xmalloc((unsigned)
                              (strlen(nom) + strlen(aire) + strlen(".ocp") +
                               1));
        sprintf(cname, "%s%s.ocp", aire, nom);
        xfree(ocp_buffer);
        ocp_cur = 0;
        ocp_size = 0;
        cnam = luatex_find_file(cname, find_ocp_file_callback);
        callback_id = callback_defined(read_ocp_file_callback);
        if (cnam && callback_id > 0) {
            res = run_callback(callback_id, "S->bSd", cnam,
                               &file_opened, &ocp_buffer, &ocp_size);
            if (!res)
                ocp_abort("callback error");
            if (!file_opened)
                ocp_abort("opening file");
        } else {
            FILE *ocp_file = NULL;
            if (!cnam)
                cnam = cname;
            if (!luatex_open_input
                (&ocp_file, cnam, kpse_ocp_format, FOPEN_RBIN_MODE, true))
                ocp_abort("opening file");
            file_opened = true;
            res = read_ocp_file(ocp_file, &ocp_buffer, &ocp_size);
            close_file(ocp_file);
            if (!res)
                ocp_abort("reading file");
        }
        if (ocp_size == 0)
            ocp_abort("checking size");

        /* Read the {\.{OCP}} file */
        f = ocp_ptr + 1;
        ocpmem_run_ptr = offset_ocp_info;
        ocp_read(ocp_length);
        real_ocp_length = ocp_length - 7;
        ocp_read_all(temp_ocp_input);
        ocp_read_all(temp_ocp_output);
        ocp_read_all(temp_ocp_no_tables);
        ocp_read_all(room_for_tables);
        ocp_read_all(temp_ocp_no_states);
        ocp_read_all(room_for_states);
        if (real_ocp_length !=
            (temp_ocp_no_tables + room_for_tables +
             temp_ocp_no_states + room_for_states))
            ocp_abort("checking size");
        real_ocp_length = real_ocp_length + 12 +
            temp_ocp_no_states + temp_ocp_no_tables;
        allocate_ocp_table(f, real_ocp_length);
        ocp_external(f) = 0;
        ocp_external_arg(f) = 0;
        ocp_file_size(f) = real_ocp_length;
        ocp_input(f) = temp_ocp_input;
        ocp_output(f) = temp_ocp_output;
        ocp_no_tables(f) = temp_ocp_no_tables;
        ocp_no_states(f) = temp_ocp_no_states;
        ocp_table_base(f) = ocpmem_run_ptr;
        if (ocp_no_tables(f) != 0) {
            previous_address = ocpmem_run_ptr + 2 * (ocp_no_tables(f));
            for (i = 1; i <= ocp_no_tables(f); i++) {
                add_to_ocp_info(previous_address);
                ocp_read_all(new_offset);
                add_to_ocp_info(new_offset);
                previous_address = previous_address + new_offset;
            }
        }
        if (room_for_tables != 0) {
            for (i = 1; i <= room_for_tables; i++) {
                ocp_read_info;
            }
        }
        ocp_state_base(f) = ocpmem_run_ptr;
        if (ocp_no_states(f) != 0) {
            previous_address = ocpmem_run_ptr + 2 * (ocp_no_states(f));
            for (i = 1; i <= ocp_no_states(f); i++) {
                add_to_ocp_info(previous_address);
                ocp_read_all(new_offset);
                add_to_ocp_info(new_offset);
                previous_address = previous_address + new_offset;
            }
        }
        if (room_for_states != 0) {
            for (i = 1; i <= room_for_states; i++) {
                ocp_read_info;
            }
        }
        ocp_ptr = f;
        g = f;
        goto DONE;
    }
  BAD_OCP:
    {
        /* Report that the ocp won't be loaded */
        /* $\Omega$ does not give precise details about why it
           rejects a particular \.{OCP} file. */
        const char *hlp[] = {
            "I wasn't able to read the data for this ocp,",
            "so I will ignore the ocp specification.",
            NULL
        };
        char errmsg[256];
        char *c = makecstring(cs_text(u));
        if (file_opened) {
            snprintf(errmsg, 255,
                     "Translation process \\%s=%s not loadable: Bad ocp file",
                     c, cnam);
        } else {
            snprintf(errmsg, 255,
                     "Translation process \\%s=%s not loadable: ocp file not found",
                     c, cnam);
        }
        free(c);
        tex_error(errmsg, hlp);
    }
  DONE:
    ocp_name(f) = maketexstring(nom);
    ocp_area(f) = maketexstring(aire);
    return g;
}
