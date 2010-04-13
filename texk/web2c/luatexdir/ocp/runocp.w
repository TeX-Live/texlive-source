% runocp.w
% 
% Copyright 2006-2009 Taco Hoekwater <taco@@luatex.org>

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
    "$Id: runocp.w 3612 2010-04-13 09:29:42Z taco $ "
"$URL: http://foundry.supelec.fr/svn/luatex/branches/0.60.x/source/texk/web2c/luatexdir/ocp/runocp.w $";

memory_word active_info[(active_mem_size + 1)];
active_index active_min_ptr = 0;        /* first unused word of |active_info| */
active_index active_max_ptr = 0;        /* last unused word of |active_info| */
active_index active_real = 0;

static ocp_list_index holding[(active_mem_size + 1)] = { 0 };

@ Here we do the main work required for reading and interpreting
   $\Omega$ Compiled Translation Processes.

@c
#define ocp_list_id_text(A) cs_text(ocp_list_id_base+(A))

#define ocp_active_min_ptr_base (ocp_active_number_base+1)
#define ocp_active_max_ptr_base (ocp_active_min_ptr_base+1)
#define ocp_active_base (ocp_active_max_ptr_base+1)


@  Here are all the instructions in our mini-assembler. 

@c
typedef enum {
    otp_right_output = 1,
    otp_right_num = 2,
    otp_right_char = 3,
    otp_right_lchar = 4,
    otp_right_some = 5,
    otp_pback_output = 6,
    otp_pback_num = 7,
    otp_pback_char = 8,
    otp_pback_lchar = 9,
    otp_pback_some = 10,
    otp_add = 11,
    otp_sub = 12,
    otp_mult = 13,
    otp_div = 14,
    otp_mod = 15,
    otp_lookup = 16,
    otp_push_num = 17,
    otp_push_char = 18,
    otp_push_lchar = 19,
    otp_state_change = 20,
    otp_state_push = 21,
    otp_state_pop = 22,
    otp_left_start = 23,
    otp_left_return = 24,
    otp_left_backup = 25,
    otp_goto = 26,
    otp_goto_ne = 27,
    otp_goto_eq = 28,
    otp_goto_lt = 29,
    otp_goto_le = 30,
    otp_goto_gt = 31,
    otp_goto_ge = 32,
    otp_goto_no_advance = 33,
    otp_goto_beg = 34,
    otp_goto_end = 35,
    otp_stop = 36
} otp_command_codes;

#define check_buffer() do {						\
    if (last==buf_size-2) {						\
      check_buffer_overflow(buf_size+4);				\
    }									\
  } while (0)

@ Of course we want to define macros that suppress the detail of how ocp
information is actually packed, so that we don't have to write things like
$$\hbox{|ocp_info[k+ocp_info[j+ocp_state_base[i]]]|}$$
too often. The \.{WEB} definitions here make |ocp_state_entry(i)(j)(k)|
(|ocp_table_entry(i)(j)(k)|) the |k|-th word in the |j|-th state (table)
of the |i|-th ocp.
@^inner loop@>

@c
#define ocp_state_entry(A,B,C) ocp_tables[(A)][ocp_tables[(A)][ocp_state_base((A))+((B)*2)]+(C)]

#define ocp_state_no(A,B) ocp_tables[(A)][ocp_state_base(A)+((B)*2)+1]

#define ocp_table_entry(A,B,C) ocp_tables[(A)][ocp_tables[(A)][ocp_table_base((A))+(B)*2]+(C)]

#define ocp_table_no(A,B)  ocp_tables[(A)][ocp_table_base((A))+((B)*2)+1]

#define otp_cur_state otp_states[otp_state_ptr]

#define tsuccumb pdftex_fail

halfword otp_init_input_start;
halfword otp_init_input_last;
halfword otp_init_input_end;
quarterword *otp_init_input_buf;

halfword otp_input_start;
halfword otp_input_last;
halfword otp_input_end;
quarterword *otp_input_buf;

halfword otp_output_end;
quarterword *otp_output_buf;

halfword otp_stack_used;
halfword otp_stack_last;
halfword otp_stack_new;
quarterword *otp_stack_buf;

halfword otp_pc;

halfword otp_calc_ptr;
halfword *otp_calcs;
halfword otp_state_ptr;
halfword *otp_states;

halfword otp_input_char;
halfword otp_calculated_char;
halfword otp_no_input_chars;

halfword otp_instruction;
halfword otp_instr;
halfword otp_arg;
halfword otp_first_arg;
halfword otp_second_arg;

halfword otp_input_ocp;

boolean otp_finished;
int otp_ext_str;
int otp_ext_str_arg;
int otp_ext_i;

#define otp_set_instruction() do {					\
    if (otp_pc>=ocp_state_no(otp_input_ocp,otp_cur_state)) {		\
      tsuccumb("bad OCP program -- PC not valid");			\
    }									\
    otp_instruction=ocp_state_entry(otp_input_ocp,otp_cur_state,otp_pc); \
    otp_instr=otp_instruction / 0x1000000;				\
    otp_arg=otp_instruction % 0x1000000;				\
  } while (0)


#define otp_check_char(A)  do {					\
    if ((1>(A)) || ((A)>otp_no_input_chars)) {			\
      tsuccumb("right hand side of OCP expression is bad");	\
    }								\
  } while (0)


#define otp_get_char(A) do {						\
    otp_check_char((A));						\
    if ((A)>otp_stack_last) {						\
      otp_calculated_char=otp_input_buf[otp_input_start+(A)-otp_stack_last]; \
    } else {								\
      otp_calculated_char=otp_stack_buf[(A)];				\
    }									\
  } while (0)


@ @c
static void overflow_ocp_buf_size(void)
{
    overflow("ocp_buf_size", (unsigned) ocp_buf_size);
}

static void overflow_ocp_stack_size(void)
{
    overflow("ocp_stack_size", (unsigned) ocp_stack_size);
}

void run_otp(void)
{
    halfword otp_counter;
    otp_set_instruction();
    switch (otp_instr) {
        /* Run the |otp_right| instructions */
    case otp_right_output:
        incr(otp_output_end);
        if (otp_output_end > ocp_buf_size)
            overflow_ocp_buf_size();
        otp_output_buf[otp_output_end] = (quarterword) otp_calcs[otp_calc_ptr];
        decr(otp_calc_ptr);
        incr(otp_pc);
        break;
    case otp_right_num:
        incr(otp_output_end);
        if (otp_output_end > ocp_buf_size)
            overflow_ocp_buf_size();
        otp_output_buf[otp_output_end] = (quarterword) otp_arg;
        incr(otp_pc);
        break;
    case otp_right_char:
        otp_get_char(otp_arg);
        incr(otp_output_end);
        if (otp_output_end > ocp_buf_size)
            overflow_ocp_buf_size();
        otp_output_buf[otp_output_end] = (quarterword) otp_calculated_char;
        incr(otp_pc);
        break;
    case otp_right_lchar:
        otp_get_char(otp_no_input_chars - otp_arg);
        incr(otp_output_end);
        if (otp_output_end > ocp_buf_size)
            overflow_ocp_buf_size();
        otp_output_buf[otp_output_end] = (quarterword) otp_calculated_char;
        incr(otp_pc);
        break;
    case otp_right_some:
        otp_first_arg = otp_arg + 1;
        incr(otp_pc);
        otp_set_instruction();
        otp_second_arg = otp_no_input_chars - otp_arg;
        for (otp_counter = otp_first_arg; otp_counter <= otp_second_arg;
             otp_counter++) {
            otp_get_char(otp_counter);
            incr(otp_output_end);
            if (otp_output_end > ocp_buf_size)
                overflow_ocp_buf_size();
            otp_output_buf[otp_output_end] = (quarterword) otp_calculated_char;
        }
        incr(otp_pc);
        break;

        /* Run the |otp_pback| instructions */
    case otp_pback_output:
        incr(otp_stack_new);
        if (otp_stack_new >= ocp_stack_size)
            overflow_ocp_stack_size();
        otp_stack_buf[otp_stack_new] = (quarterword) otp_calcs[otp_calc_ptr];
        decr(otp_calc_ptr);
        incr(otp_pc);
        break;
    case otp_pback_num:
        incr(otp_stack_new);
        if (otp_stack_new >= ocp_stack_size)
            overflow_ocp_stack_size();
        otp_stack_buf[otp_stack_new] = (quarterword) otp_arg;
        incr(otp_pc);
        break;
    case otp_pback_char:
        otp_get_char(otp_arg);
        incr(otp_stack_new);
        if (otp_stack_new >= ocp_stack_size)
            overflow_ocp_stack_size();
        otp_stack_buf[otp_stack_new] = (quarterword) otp_calculated_char;
        incr(otp_pc);
        break;
    case otp_pback_lchar:
        otp_get_char(otp_no_input_chars - otp_arg);
        incr(otp_stack_new);
        if (otp_stack_new >= ocp_stack_size)
            overflow_ocp_stack_size();
        otp_stack_buf[otp_stack_new] = (quarterword) otp_calculated_char;
        incr(otp_pc);
        break;
    case otp_pback_some:
        otp_first_arg = otp_arg + 1;
        incr(otp_pc);
        otp_set_instruction();
        otp_second_arg = otp_no_input_chars - otp_arg;
        for (otp_counter = otp_first_arg; otp_counter <= otp_second_arg;
             otp_counter++) {
            otp_get_char(otp_counter);
            incr(otp_stack_new);
            if (otp_stack_new >= ocp_stack_size)
                overflow_ocp_stack_size();
            otp_stack_buf[otp_stack_new] = (quarterword) otp_calculated_char;
        }
        incr(otp_pc);
        break;

        /* Run the arithmetic instructions */
    case otp_add:
        otp_calcs[otp_calc_ptr - 1] =
            otp_calcs[otp_calc_ptr - 1] + otp_calcs[otp_calc_ptr];
        incr(otp_pc);
        decr(otp_calc_ptr);
        break;
    case otp_sub:
        otp_calcs[otp_calc_ptr - 1] =
            otp_calcs[otp_calc_ptr - 1] - otp_calcs[otp_calc_ptr];
        incr(otp_pc);
        decr(otp_calc_ptr);
        break;
    case otp_mult:
        otp_calcs[otp_calc_ptr - 1] =
            otp_calcs[otp_calc_ptr - 1] * otp_calcs[otp_calc_ptr];
        incr(otp_pc);
        decr(otp_calc_ptr);
        break;
    case otp_div:
        otp_calcs[otp_calc_ptr - 1] =
            otp_calcs[otp_calc_ptr - 1] / otp_calcs[otp_calc_ptr];
        incr(otp_pc);
        decr(otp_calc_ptr);
        break;
    case otp_mod:
        otp_calcs[otp_calc_ptr - 1] =
            otp_calcs[otp_calc_ptr - 1] % otp_calcs[otp_calc_ptr];
        incr(otp_pc);
        decr(otp_calc_ptr);
        break;
    case otp_lookup:
        if (otp_calcs[otp_calc_ptr] >=
            ocp_table_no(otp_input_ocp, otp_calcs[otp_calc_ptr - 1])) {
            tsuccumb("bad OCP program -- table index not valid");
        }
        otp_calcs[otp_calc_ptr - 1] =
            ocp_table_entry(otp_input_ocp, otp_calcs[otp_calc_ptr - 1],
                            otp_calcs[otp_calc_ptr]);
        incr(otp_pc);
        decr(otp_calc_ptr);
        break;
    case otp_push_num:
        incr(otp_calc_ptr);
        if (otp_calc_ptr >= ocp_stack_size)
            overflow_ocp_stack_size();
        otp_calcs[otp_calc_ptr] = otp_arg;
        incr(otp_pc);
        break;
    case otp_push_char:
        otp_get_char(otp_arg);
        incr(otp_calc_ptr);
        if (otp_calc_ptr >= ocp_stack_size)
            overflow_ocp_stack_size();
        otp_calcs[otp_calc_ptr] = otp_calculated_char;
        incr(otp_pc);
        break;
    case otp_push_lchar:
        otp_get_char(otp_no_input_chars - otp_arg);
        incr(otp_calc_ptr);
        if (otp_calc_ptr >= ocp_stack_size)
            overflow_ocp_stack_size();
        otp_calcs[otp_calc_ptr] = otp_calculated_char;
        incr(otp_pc);
        break;

        /* Run the |otp_state| instructions */
    case otp_state_change:
        otp_input_start = otp_input_last;
        for (otp_counter = 1; otp_counter <= (otp_stack_new - otp_stack_used);
             otp_counter++) {
            otp_stack_buf[otp_counter] =
                otp_stack_buf[otp_counter + otp_stack_used];
        }
        otp_stack_new = otp_stack_new - otp_stack_used;
        otp_stack_last = otp_stack_new;
        otp_stack_used = 0;
        otp_states[otp_state_ptr] = otp_arg;
        otp_pc = 0;
        break;
    case otp_state_push:
        otp_input_start = otp_input_last;
        for (otp_counter = 1; otp_counter <= (otp_stack_new - otp_stack_used);
             otp_counter++) {
            otp_stack_buf[otp_counter] =
                otp_stack_buf[otp_counter + otp_stack_used];
        };
        otp_stack_new = otp_stack_new - otp_stack_used;
        otp_stack_last = otp_stack_new;
        otp_stack_used = 0;
        incr(otp_state_ptr);
        if (otp_state_ptr >= ocp_stack_size)
            overflow_ocp_stack_size();
        otp_states[otp_state_ptr] = otp_arg;
        otp_pc = 0;
        break;
    case otp_state_pop:
        otp_input_start = otp_input_last;
        for (otp_counter = 1; otp_counter <= (otp_stack_new - otp_stack_used);
             otp_counter++) {
            otp_stack_buf[otp_counter] =
                otp_stack_buf[otp_counter + otp_stack_used];
        }
        otp_stack_new = otp_stack_new - otp_stack_used;
        otp_stack_last = otp_stack_new;
        otp_stack_used = 0;
        if (otp_state_ptr > 0)
            decr(otp_state_ptr);
        otp_pc = 0;
        break;

        /* Run the |otp_left| instructions */
    case otp_left_start:
        otp_input_start = otp_input_last;
        otp_input_last = otp_input_start;
        otp_stack_used = 0;
        if ((otp_stack_last == 0) && (otp_input_last >= otp_input_end)) {
            otp_finished = true;
        } else if ((otp_stack_used < otp_stack_last)) {
            incr(otp_stack_used);       /* no overflow problem */
            otp_input_char = otp_stack_buf[otp_stack_used];
            otp_no_input_chars = 1;
            incr(otp_pc);
        } else {
            incr(otp_input_last);       /* no overflow problem */
            otp_input_char = otp_input_buf[otp_input_last];
            otp_no_input_chars = 1;
            incr(otp_pc);
        }
        break;
    case otp_left_return:
        otp_input_last = otp_input_start;
        otp_stack_used = 0;
        if ((otp_stack_used < otp_stack_last)) {
            incr(otp_stack_used);       /* no overflow problem */
            otp_input_char = otp_stack_buf[otp_stack_used];
            otp_no_input_chars = 1;
            incr(otp_pc);
        } else {
            incr(otp_input_last);       /* no overflow problem */
            otp_input_char = otp_input_buf[otp_input_last];
            otp_no_input_chars = 1;
            incr(otp_pc);
        }
        break;
    case otp_left_backup:
        if (otp_input_start < otp_input_last) {
            decr(otp_input_last);
            otp_input_char = otp_input_buf[otp_input_last];
        } else {
            decr(otp_stack_used);
            otp_input_char = otp_stack_buf[otp_stack_used];
        }
        decr(otp_no_input_chars);
        incr(otp_pc);
        break;

        /* Run the |otp_goto| instructions */
    case otp_goto:
        otp_pc = otp_arg;
        break;
    case otp_goto_ne:
        otp_first_arg = otp_arg;
        incr(otp_pc);
        otp_set_instruction();
        if (otp_input_char != otp_first_arg) {
            otp_pc = otp_arg;
        } else {
            incr(otp_pc);
        }
        break;
    case otp_goto_eq:
        otp_first_arg = otp_arg;
        incr(otp_pc);
        otp_set_instruction();
        if (otp_input_char == otp_first_arg) {
            otp_pc = otp_arg;
        } else {
            incr(otp_pc);
        }
        break;
    case otp_goto_lt:
        otp_first_arg = otp_arg;
        incr(otp_pc);
        otp_set_instruction();
        if (otp_input_char < otp_first_arg) {
            otp_pc = otp_arg;
        } else {
            incr(otp_pc);
        }
        break;
    case otp_goto_le:
        otp_first_arg = otp_arg;
        incr(otp_pc);
        otp_set_instruction();
        if (otp_input_char <= otp_first_arg) {
            otp_pc = otp_arg;
        } else {
            incr(otp_pc);
        }
        break;
    case otp_goto_gt:
        otp_first_arg = otp_arg;
        incr(otp_pc);
        otp_set_instruction();
        if (otp_input_char > otp_first_arg) {
            otp_pc = otp_arg;
        } else {
            incr(otp_pc);
        }
        break;
    case otp_goto_ge:
        otp_first_arg = otp_arg;
        incr(otp_pc);
        otp_set_instruction();
        if (otp_input_char >= otp_first_arg) {
            otp_pc = otp_arg;
        } else {
            incr(otp_pc);
        }
        break;
    case otp_goto_no_advance:
        if (otp_stack_used < otp_stack_last) {
            incr(otp_stack_used);       /* no overflow problem */
            otp_input_char = otp_stack_buf[otp_stack_used];
            incr(otp_no_input_chars);   /* no overflow problem */
            incr(otp_pc);
        } else if (otp_input_last >= otp_input_end) {
            otp_pc = otp_arg;
        } else {
            incr(otp_input_last);       /* no overflow problem */
            otp_input_char = otp_input_buf[otp_input_last];
            incr(otp_no_input_chars);   /* no overflow problem */
            incr(otp_pc);
        }
        break;
    case otp_goto_beg:
        if (otp_input_last == 0) {
            otp_pc = otp_arg;
        } else {
            incr(otp_pc);
        }
        break;
    case otp_goto_end:
        if (otp_input_last >= otp_input_end) {
            otp_pc = otp_arg;
        } else {
            incr(otp_pc);
        }
        break;

        /* Run the |otp_stop| instruction */
    case otp_stop:
        otp_input_start = otp_input_last;
        for (otp_counter = 1; otp_counter <= (otp_stack_new - otp_stack_used);
             otp_counter++) {
            otp_stack_buf[otp_counter] =
                otp_stack_buf[otp_counter + otp_stack_used];
        }
        otp_stack_new = otp_stack_new - otp_stack_used;
        otp_stack_last = otp_stack_new;
        otp_stack_used = 0;
        otp_pc = 0;
        break;

    default:
        tsuccumb("bad OCP program -- unknown instruction");
        break;
    }
}

@ @c
#define FPUTC(a,b) fputc((int)(a),b)

static void run_external_ocp(char *external_ocp_name)
{
    char *in_file_name;
    char *out_file_name;
    FILE *in_file;
    FILE *out_file;
    char command_line[400];
    int i;
    unsigned c;
    int c_in;
#ifdef WIN32
    char *tempenv;

#  define null_string(s) ((s == NULL) || (*s == '\0'))

    tempenv = getenv("TMPDIR");
    if (null_string(tempenv))
        tempenv = getenv("TEMP");
    if (null_string(tempenv))
        tempenv = getenv("TMP");
    if (null_string(tempenv))
        tempenv = "c:/tmp";     /* "/tmp" is not good if we are on a CD-ROM */
    in_file_name = concat(tempenv, "/__aleph__in__XXXXXX");
    mktemp(in_file_name);
    in_file = fopen(in_file_name, FOPEN_WBIN_MODE);
#else
    int in_file_fd;
    int out_file_fd;
    in_file_name = strdup("/tmp/__aleph__in__XXXXXX");
    in_file_fd = mkstemp(in_file_name);
    in_file = fdopen(in_file_fd, FOPEN_WBIN_MODE);
#endif                          /* WIN32 */


    for (i = 1; i <= otp_input_end; i++) {
        c = otp_input_buf[i];
        if (c > 0xffff) {
            fprintf(stderr, "Aleph does not currently support 31-bit chars\n");
            exit(1);
        }
        if (c > 0x4000000) {
            FPUTC(0xfc | ((c >> 30) & 0x1), in_file);
            FPUTC(0x80 | ((c >> 24) & 0x3f), in_file);
            FPUTC(0x80 | ((c >> 18) & 0x3f), in_file);
            FPUTC(0x80 | ((c >> 12) & 0x3f), in_file);
            FPUTC(0x80 | ((c >> 6) & 0x3f), in_file);
            FPUTC(0x80 | (c & 0x3f), in_file);
        } else if (c > 0x200000) {
            FPUTC(0xf8 | ((c >> 24) & 0x3), in_file);
            FPUTC(0x80 | ((c >> 18) & 0x3f), in_file);
            FPUTC(0x80 | ((c >> 12) & 0x3f), in_file);
            FPUTC(0x80 | ((c >> 6) & 0x3f), in_file);
            FPUTC(0x80 | (c & 0x3f), in_file);
        } else if (c > 0x10000) {
            FPUTC(0xf0 | ((c >> 18) & 0x7), in_file);
            FPUTC(0x80 | ((c >> 12) & 0x3f), in_file);
            FPUTC(0x80 | ((c >> 6) & 0x3f), in_file);
            FPUTC(0x80 | (c & 0x3f), in_file);
        } else if (c > 0x800) {
            FPUTC(0xe0 | ((c >> 12) & 0xf), in_file);
            FPUTC(0x80 | ((c >> 6) & 0x3f), in_file);
            FPUTC(0x80 | (c & 0x3f), in_file);
        } else if (c > 0x80) {
            FPUTC(0xc0 | ((c >> 6) & 0x1f), in_file);
            FPUTC(0x80 | (c & 0x3f), in_file);
        } else {
            FPUTC(c & 0x7f, in_file);
        }
    }
    fclose(in_file);

#define advance_cin if ((c_in = fgetc(out_file)) == -1) { \
                         fprintf(stderr, "File contains bad char\n"); \
                         goto end_of_while; \
                    }

#ifdef WIN32
    out_file_name = concat(tempenv, "/__aleph__out__XXXXXX");
    mktemp(out_file_name);
    out_file = fopen(out_file_name, FOPEN_RBIN_MODE);
#else
    out_file_name = strdup("/tmp/__aleph__out__XXXXXX");
    out_file_fd = mkstemp(out_file_name);
    out_file = fdopen(out_file_fd, FOPEN_RBIN_MODE);
#endif

    sprintf(command_line, "%s <%s >%s\n",
            external_ocp_name + 1, in_file_name, out_file_name);
    system(command_line);
    otp_output_end = 0;
    otp_output_buf[otp_output_end] = 0;
    while ((c_in = (int) fgetc(out_file)) != -1) {
        if (c_in >= 0xfc) {
            c = (unsigned) (c_in & 0x1) << 30;
            advance_cin;
            c |= (c_in & 0x3f) << 24;
            advance_cin;
            c |= (c_in & 0x3f) << 18;
            advance_cin;
            c |= (c_in & 0x3f) << 12;
            advance_cin;
            c |= (c_in & 0x3f) << 6;
            advance_cin;
            c |= (c_in & 0x3f);
        } else if (c_in >= 0xf8) {
            c = (unsigned) (c_in & 0x3) << 24;
            advance_cin;
            c |= (c_in & 0x3f) << 18;
            advance_cin;
            c |= (c_in & 0x3f) << 12;
            advance_cin;
            c |= (c_in & 0x3f) << 6;
            advance_cin;
            c |= (c_in & 0x3f);
        } else if (c_in >= 0xf0) {
            c = (unsigned) (c_in & 0x7) << 18;
            advance_cin;
            c |= (c_in & 0x3f) << 12;
            advance_cin;
            c |= (c_in & 0x3f) << 6;
            advance_cin;
            c |= (c_in & 0x3f);
        } else if (c_in >= 0xe0) {
            c = (unsigned) (c_in & 0xf) << 12;
            advance_cin;
            c |= (c_in & 0x3f) << 6;
            advance_cin;
            c |= (c_in & 0x3f);
        } else if (c_in >= 0x80) {
            c = (unsigned) (c_in & 0x1f) << 6;
            advance_cin;
            c |= (c_in & 0x3f);
        } else {
            c = (unsigned) (c_in & 0x7f);
        }
        otp_output_buf[++otp_output_end] = (quarterword) c;
    }
    fclose(out_file);

  end_of_while:
    remove(in_file_name);
    remove(out_file_name);
}

@ @c
void run_ocp(void)
{
    int t = 0;
    halfword otp_i;
    halfword otp_counter;
    /* The OTP input buffer is an array of 16-bit values.
       The range |0xD800| \dots |0xDFFF| is used for surrogate pairs, 
       so it cannot be used for simple 16-bit values like it is in 
       Omega, and incompatibility with Aleph is unavoidable. 
     */

    otp_init_input_end = 0;
    while ((cur_cmd == letter_cmd) || (cur_cmd == other_char_cmd) ||
           (cur_cmd == char_given_cmd) || (cur_cmd == spacer_cmd)) {
        incr(otp_init_input_end);
        if (otp_init_input_end > ocp_buf_size)
            overflow_ocp_buf_size();
        /* |cur_chr| can cover the full range |0..0x10FFFF| */
        if (cur_chr < 0x10000) {
            otp_init_input_buf[otp_init_input_end] = (quarterword) cur_chr;
        } else {
            otp_init_input_buf[otp_init_input_end] = (quarterword)
                (((cur_chr - 0x10000) / 0x400) + 0xD800);
            incr(otp_init_input_end);
            if (otp_init_input_end > ocp_buf_size)
                overflow_ocp_buf_size();
            otp_init_input_buf[otp_init_input_end] = (quarterword)
                (((cur_chr - 0x10000) % 0x400) + 0xDC00);
        }
        get_token();
        if ((cur_cmd != letter_cmd) && (cur_cmd != other_char_cmd) &&
            (cur_cmd != char_given_cmd)) {
            x_token();
            if (cur_cmd == char_num_cmd) {
                scan_char_num();
                cur_cmd = other_char_cmd;
                cur_chr = cur_val;
            }
        }
    }
    back_input();
    otp_input_end = otp_init_input_end;
    for (otp_i = 0; otp_i <= otp_init_input_end; otp_i++) {
        otp_input_buf[otp_i] = otp_init_input_buf[otp_i];
    }

    otp_input_ocp = active_ocp(active_real);
    if (otp_input_ocp == 0) {
        tex_error("Null ocp being used: all input lost", NULL);
        otp_output_end = 0;
    } else if (ocp_external(otp_input_ocp) == 0) {
        otp_input_start = 0;
        otp_input_last = 0;
        otp_stack_used = 0;
        otp_stack_last = 0;
        otp_stack_new = 0;
        otp_output_end = 0;
        otp_pc = 0;
        otp_finished = false;
        otp_calc_ptr = 0;
        otp_calcs[otp_calc_ptr] = 0;
        otp_state_ptr = 0;
        otp_states[otp_state_ptr] = 0;
        while (!otp_finished)
            run_otp();
    } else {
        /* k=0;  ? */
        char *cmd = makecstring(ocp_external(otp_input_ocp));
        char *arg = makecstring(ocp_external_arg(otp_input_ocp));
        char *cmdline = xmalloc((unsigned) (strlen(cmd) + strlen(arg) + 3));
        strcpy(cmdline, cmd);
        strcat(cmdline, " ");
        strcat(cmdline, arg);
        xfree(cmd);
        xfree(arg);
        run_external_ocp(cmdline);
    }


    /* The OTP output buffer is an array of 16-bit values. To convert back
       to the |buffer| it has to be re-encoded into UTF-8. 
     */

    if ((first + otp_output_end) >= ocp_buf_size)
        overflow_ocp_buf_size();
    push_input();
    current_ocp_lstack = active_lstack_no(active_real);
    current_ocp_no = (short) active_counter(active_real);
    istate = mid_line;
    istart = first;
    last = istart;
    iloc = istart;
    line_catcode_table = DEFAULT_CAT_TABLE;
    line_partial = false;
    for (otp_counter = 1; otp_counter <= otp_output_end; otp_counter++) {
        /* convert back to utf-8  */
        t = otp_output_buf[otp_counter];
        if (otp_counter < otp_output_end) {
            if ((t >= 0xD800) && (t < 0xDC00)) {
                if ((otp_output_buf[otp_counter + 1] >= 0xDC00)
                    && (otp_output_buf[otp_counter + 1] < 0xE000)) {
                    incr(otp_counter);
                    t = (t - 0xD800) * 0x400 + (otp_output_buf[otp_counter] -
                                                0xDC00) + 0x10000;
                }
            } else if ((t >= 0xDC00) && (t < 0xE000)) {
                if ((otp_output_buf[otp_counter + 1] >= 0xD800)
                    && (otp_output_buf[otp_counter + 1] < 0xDC00)) {
                    incr(otp_counter);
                    t = (otp_output_buf[otp_counter] - 0xD800) * 0x400 + (t -
                                                                          0xDC00)
                        + 0x10000;
                }
            }
        }
        if (t <= 0x7F) {
            buffer[last] = (packed_ASCII_code) t;
        } else if (t <= 0x7FF) {
            buffer[last] = (packed_ASCII_code) (0xC0 + t / 0x40);
            incr(last);
            check_buffer();
            buffer[last] = (packed_ASCII_code) (0x80 + t % 0x40);
        } else if (t <= 0xFFFF) {
            buffer[last] = (packed_ASCII_code) (0xE0 + t / 0x1000);
            incr(last);
            check_buffer();
            buffer[last] = (packed_ASCII_code) (0x80 + (t % 0x1000) / 0x40);
            incr(last);
            check_buffer();
            buffer[last] = (packed_ASCII_code) (0x80 + (t % 0x1000) % 0x40);
        } else {
            buffer[last] = (packed_ASCII_code) (0xF0 + t / 0x400000);
            incr(last);
            check_buffer();
            buffer[last] = (packed_ASCII_code) (0x80 + (t % 0x40000) / 0x1000);
            incr(last);
            check_buffer();
            buffer[last] =
                (packed_ASCII_code) (0x80 + ((t % 0x40000) % 0x1000) / 0x40);
            incr(last);
            check_buffer();
            buffer[last] =
                (packed_ASCII_code) (0x80 + ((t % 0x40000) % 0x1000) % 0x40);
        }
        incr(last);
        check_buffer();
    }
    ilimit = last - 1;
    if (t > 0xFFFF)
        decr(ilimit);
    if (t > 0x7FF)
        decr(ilimit);
    if (t > 0x7F)
        decr(ilimit);
    first = last;

}

@ @c
void initialize_ocp_buffers(int ocp_buf_size, int ocp_stack_size)
{
    otp_init_input_buf = xmallocarray(quarterword, (unsigned) ocp_buf_size);
    otp_input_buf = xmallocarray(quarterword, (unsigned) ocp_buf_size);
    otp_output_buf = xmallocarray(quarterword, (unsigned) ocp_buf_size);
    otp_stack_buf = xmallocarray(quarterword, (unsigned) ocp_stack_size);
    otp_calcs = xmallocarray(halfword, (unsigned) ocp_stack_size);
    otp_states = xmallocarray(halfword, (unsigned) ocp_stack_size);
}

@ @c
boolean is_last_ocp(scaled llstack_no, int counter)
{
    active_min_ptr = equiv(ocp_active_min_ptr_base);
    active_max_ptr = equiv(ocp_active_max_ptr_base);
    active_real = active_min_ptr;
    while ((active_real < active_max_ptr) &&
           (active_lstack_no(active_real) < llstack_no)) {
        active_real = active_real + 2;
    }
    while ((active_real < active_max_ptr) &&
           (active_lstack_no(active_real) == llstack_no) &&
           (active_counter(active_real) <= counter)) {
        active_real = active_real + 2;
    }
    return (active_real == active_max_ptr);
}

@ @c
void print_active_ocps(void)
{
    int i;
    tprint_nl("Active ocps: [");
    i = active_min_ptr;
    while (i < active_max_ptr) {
        tprint("(");
        print_scaled(active_lstack_no(i));
        tprint(",");
        print_int(active_counter(i));
        tprint(",");
        print_esc(cs_text(ocp_id_base + active_ocp(i)));
        tprint(")");
        if (i != (active_max_ptr - 2))
            tprint(",");
        i = i + 2;
    }
    tprint("]");
}

@ @c
void add_ocp_stack(int min_index, scaled min_value)
{
    ocp_lstack_index p;
    scaled llstack_no;
    int counter;
    scaled m;
    m = min_value;              /* TH: whatever this is .. */
    p = ocp_list_lstack(holding[min_index]);
    llstack_no = ocp_list_lstack_no(holding[min_index]);
    counter = 0;
    while (!(is_null_ocp_lstack(p))) {
        active_ocp(active_max_ptr) = ocp_lstack_ocp(p);
        active_counter(active_max_ptr) = (quarterword) counter;
        active_lstack_no(active_max_ptr) = llstack_no;
        p = ocp_lstack_lnext(p);
        active_max_ptr = active_max_ptr + 2;
        incr(counter);          /* no overflow problem */
    }
}

@ @c
void active_compile(void)
{
    int i;
    int min_index;
    scaled min_stack_ocp;
    scaled old_min;
    int max_active;
    scaled stack_value;
    active_min_ptr = active_max_ptr;
    min_stack_ocp = ocp_maxint;
    max_active = equiv(ocp_active_number_base) - 1;
    min_index = max_active;     /* TH I hope ...  */
    for (i = max_active; i >= 0; i--) {
        holding[i] = ocp_list_list[ocp_active_base + i];
        stack_value = ocp_list_lstack_no(holding[i]);
        if (stack_value < min_stack_ocp) {
            min_index = i;
            min_stack_ocp = stack_value;
        }
    }
    while (min_stack_ocp < ocp_maxint) {
        add_ocp_stack(min_index, min_stack_ocp);
        old_min = min_stack_ocp;
        min_stack_ocp = ocp_maxint;
        for (i = max_active; i >= 0; i--) {
            stack_value = ocp_list_lstack_no(holding[i]);
            while (old_min == stack_value) {
                holding[i] = ocp_list_lnext(holding[i]);
                stack_value = ocp_list_lstack_no(holding[i]);
            }
            if (stack_value < min_stack_ocp) {
                min_index = i;
                min_stack_ocp = stack_value;
            }
        }
    }
}

@ @c
void do_push_ocp_list(small_number a)
{
    halfword ocp_list_no;
    halfword old_number;
    int i;
    scan_ocp_list_ident();
    ocp_list_no = cur_val;
    old_number = equiv(ocp_active_number_base);
    define(ocp_active_base + old_number, data_cmd, ocp_list_no);
    define(ocp_active_number_base, data_cmd, (old_number + 1));
    if (ocp_trace_level == 1) {
        tprint_nl("New active ocp list: {");
        for (i = old_number; i >= 0; i--) {
            print_esc(ocp_list_id_text(equiv(ocp_active_base + i)));
            tprint("=");
            print_ocp_list(ocp_list_list[equiv(ocp_active_base + i)]);
            if (i != 0)
                tprint(",");
        }
        tprint("}");
    }
    active_compile();
    define(ocp_active_min_ptr_base, data_cmd, active_min_ptr);
    define(ocp_active_max_ptr_base, data_cmd, active_max_ptr);
}

@ @c
void do_pop_ocp_list(small_number a)
{
    halfword old_number;
    int i;
    old_number = equiv(ocp_active_number_base);
    if (old_number == 0) {
        print_err("No active ocp lists to be popped");
    } else {
        define(ocp_active_number_base, data_cmd, (old_number - 1));
        if (ocp_trace_level == 1) {
            tprint_nl("New active ocp list: {");
            for (i = (old_number - 2); i >= 0; i++) {
                print_esc(ocp_list_id_text(equiv(ocp_active_base + i)));
                tprint("=");
                print_ocp_list(ocp_list_list[equiv(ocp_active_base + i)]);
                if (i != 0)
                    tprint(",");
            }
            tprint("}");
        }
        active_compile();
        define(ocp_active_min_ptr_base, data_cmd, active_min_ptr);
        define(ocp_active_max_ptr_base, data_cmd, active_max_ptr);
    }
}

@ @c
void do_clear_ocp_lists(small_number a)
{
    define(ocp_active_number_base, data_cmd, 0);
    active_compile();
    define(ocp_active_min_ptr_base, data_cmd, active_min_ptr);
    define(ocp_active_max_ptr_base, data_cmd, active_max_ptr);
}


@ @c
void dump_active_ocp_info(void)
{
    int k;
    dump_int(active_min_ptr);
    dump_int(active_max_ptr);
    for (k = 0; k <= active_max_ptr - 1; k++)
        dump_wd(active_info[k]);
    if (active_max_ptr > 0) {
        print_ln();
        print_int(active_max_ptr);
        tprint(" words of active ocps");
    }
}

void undump_active_ocp_info(void)
{
    int k;
    undump_int(active_min_ptr);
    undump_int(active_max_ptr);
    for (k = 0; k <= active_max_ptr - 1; k++)
        undump_wd(active_info[k]);
}
