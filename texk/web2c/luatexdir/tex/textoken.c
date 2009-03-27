/* textoken.c
   
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

#include "luatex-api.h"
#include <ptexlib.h>

#include "tokens.h"
#include "commands.h"

static const char _svn_version[] =
    "$Id: textoken.c 2086 2009-03-22 15:32:08Z oneiros $ $URL: http://scm.foundry.supelec.fr/svn/luatex/trunk/src/texk/web2c/luatexdir/tex/textoken.c $";

#define skipping 1              /* |scanner_status| when passing conditional text */
#define defining 2              /* |scanner_status| when reading a macro definition */
#define matching 3              /* |scanner_status| when reading macro arguments */
#define aligning 4              /* |scanner_status| when reading an alignment preamble */
#define absorbing 5             /* |scanner_status| when reading a balanced text */

#define right_brace_token 0x400000      /* $2^{21}\cdot|right_brace|$ */

#define cat_code_table int_par(param_cat_code_table_code)
#define tracing_nesting int_par(param_tracing_nesting_code)
#define end_line_char int_par(param_end_line_char_code)
#define suppress_outer_error int_par(param_suppress_outer_error_code)

#define every_eof get_every_eof()

#define null_cs 1               /* equivalent of \.{\\csname\\endcsname} */

#define eq_level(a) zeqtb[a].hh.u.B1
#define eq_type(a)  zeqtb[a].hh.u.B0
#define equiv(a)    zeqtb[a].hh.v.RH

/* leave an input level, re-enter the old */
#define pop_input() cur_input=input_stack[--input_ptr]

#define nonstop_mode 1

#define terminal_input (name==0)        /* are we reading from the terminal? */
#define special_char 1114113    /* |biggest_char+2| */
#define cur_file input_file[index]      /* the current |alpha_file| variable */

#define no_expand_flag special_char     /*this characterizes a special variant of |relax| */

#define detokenized_line() (line_catcode_table==NO_CAT_TABLE)

extern void insert_vj_template(void);

#define do_get_cat_code(a) do {                                         \
    if (line_catcode_table!=DEFAULT_CAT_TABLE)                          \
      a=get_cat_code(line_catcode_table,cur_chr);                       \
    else                                                                \
      a=get_cat_code(cat_code_table,cur_chr);                           \
  } while (0)



/* string compare */

boolean str_eq_cstr(str_number r, char *s, size_t l)
{
    if (l != (size_t) length(r))
        return false;
    return (strncmp((const char *) (str_pool + str_start_macro(r)), s, l) == 0);
}



int get_char_cat_code(int cur_chr)
{
    int a;
    do_get_cat_code(a);
    return a;
}

static void invalid_character_error(void)
{
    char *hlp[] = { "A funny symbol that I can't read has just been input.",
        "Continue, and I'll forget that it ever happened.",
        NULL
    };
    deletions_allowed = false;
    tex_error("Text line contains an invalid character", hlp);
    deletions_allowed = true;
}

static boolean process_sup_mark(void);  /* below */

static int scan_control_sequence(void); /* below */

typedef enum { next_line_ok, next_line_return,
    next_line_restart
} next_line_retval;

static next_line_retval next_line(void);        /* below */

/* @^inner loop@>*/

static void utf_error(void)
{
    char *hlp[] = { "A funny symbol that I can't read has just been input.",
        "Just continue, I'll change it to 0xFFFD.",
        NULL
    };
    deletions_allowed = false;
    tex_error("Text line contains an invalid utf-8 sequence", hlp);
    deletions_allowed = true;
}

#define do_buffer_to_unichar(a,b)  a = buffer[b] < 0x80 ? buffer[b++] : qbuffer_to_unichar(&b)

static integer qbuffer_to_unichar(integer * k)
{
    register int ch;
    int val = 0xFFFD;
    unsigned char *text = buffer + *k;
    if ((ch = *text++) < 0x80) {
        val = ch;
        *k += 1;
    } else if (ch <= 0xbf) {    /* error */
        *k += 1;
    } else if (ch <= 0xdf) {
        if (*text >= 0x80 && *text < 0xc0)
            val = ((ch & 0x1f) << 6) | (*text++ & 0x3f);
        *k += 2;
    } else if (ch <= 0xef) {
        if (*text >= 0x80 && *text < 0xc0 && text[1] >= 0x80 && text[1] < 0xc0) {
            val =
                ((ch & 0xf) << 12) | ((text[0] & 0x3f) << 6) | (text[1] & 0x3f);
            *k += 3;
        }
    } else {
        int w = (((ch & 0x7) << 2) | ((text[0] & 0x30) >> 4)) - 1, w2;
        w = (w << 6) | ((text[0] & 0xf) << 2) | ((text[1] & 0x30) >> 4);
        w2 = ((text[1] & 0xf) << 6) | (text[2] & 0x3f);
        val = w * 0x400 + w2 + 0x10000;
        if (*text < 0x80 || text[1] < 0x80 || text[2] < 0x80 ||
            *text >= 0xc0 || text[1] >= 0xc0 || text[2] >= 0xc0)
            val = 0xFFFD;
        *k += 4;
    }
    if (val == 0xFFFD)
        utf_error();
    return (val);
}

/* This is a very basic helper */

char *u2s(unsigned unic)
{
    char *buf = xmalloc(5);
    char *pt = buf;
    if (unic < 0x80)
        *pt++ = unic;
    else if (unic < 0x800) {
        *pt++ = 0xc0 | (unic >> 6);
        *pt++ = 0x80 | (unic & 0x3f);
    } else if (unic >= 0x110000) {
        *pt++ = unic - 0x110000;
    } else if (unic < 0x10000) {
        *pt++ = 0xe0 | (unic >> 12);
        *pt++ = 0x80 | ((unic >> 6) & 0x3f);
        *pt++ = 0x80 | (unic & 0x3f);
    } else {
        int u, z, y, x;
        unsigned val = unic - 0x10000;
        u = ((val & 0xf0000) >> 16) + 1;
        z = (val & 0x0f000) >> 12;
        y = (val & 0x00fc0) >> 6;
        x = val & 0x0003f;
        *pt++ = 0xf0 | (u >> 2);
        *pt++ = 0x80 | ((u & 3) << 4) | z;
        *pt++ = 0x80 | y;
        *pt++ = 0x80 | x;
    }
    *pt = '\0';
    return buf;
}


/* 
   In case you are getting bored, here is a slightly less trivial routine:
   Given a string of lowercase letters, like `\.{pt}' or `\.{plus}' or
   `\.{width}', the |scan_keyword| routine checks to see whether the next
   tokens of input match this string. The match must be exact, except that
   uppercase letters will match their lowercase counterparts; uppercase
   equivalents are determined by subtracting |"a"-"A"|, rather than using the
   |uc_code| table, since \TeX\ uses this routine only for its own limited
   set of keywords.

   If a match is found, the characters are effectively removed from the input
   and |true| is returned. Otherwise |false| is returned, and the input
   is left essentially unchanged (except for the fact that some macros
   may have been expanded, etc.).
   @^inner loop@>
*/

boolean scan_keyword(char *s)
{                               /* look for a given string */
    pointer p;                  /* tail of the backup list */
    pointer q;                  /* new node being added to the token list via |store_new_token| */
    char *k;                    /* index into |str_pool| */
    if (strlen(s) == 1) {
        /* @<Get the next non-blank non-call token@>; */
        do {
            get_x_token();
        } while ((cur_cmd == spacer_cmd) || (cur_cmd == relax_cmd));
        if ((cur_cs == 0) && ((cur_chr == *s) || (cur_chr == *s - 'a' + 'A'))) {
            return true;
        } else {
            back_input();
            return false;
        }
    } else {
        p = backup_head;
        link(p) = null;
        k = s;
        while (*k) {
            get_x_token();      /* recursion is possible here */
            if ((cur_cs == 0) &&
                ((cur_chr == *k) || (cur_chr == *k - 'a' + 'A'))) {
                store_new_token(cur_tok);
                k++;
            } else if ((cur_cmd != spacer_cmd) || (p != backup_head)) {
                if (p != backup_head) {
                    q = get_avail();
                    info(q) = cur_tok;
                    link(q) = null;
                    link(p) = q;
                    begin_token_list(link(backup_head), backed_up);
                } else {
                    back_input();
                }
                return false;
            }
        }
        flush_list(link(backup_head));
    }
    return true;
}

/* |scan_direction| has to be defined here because luatangle will output 
   a character constant when it sees a string literal of length 1 */

#define dir_T 0
#define dir_L 1
#define dir_B 2
#define dir_R 3

#define scan_single_dir(A) do {                     \
        if (scan_keyword("T")) A=dir_T;             \
        else if (scan_keyword("L")) A=dir_L;        \
        else if (scan_keyword("B")) A=dir_B;        \
        else if (scan_keyword("R")) A=dir_R;        \
        else {                                      \
            tex_error("Bad direction", NULL);       \
            cur_val=0;                              \
            return;                                 \
        }                                           \
    } while (0)

void scan_direction(void)
{
    integer d1, d2, d3;
    get_x_token();
    if (cur_cmd == assign_dir_cmd) {
        cur_val = zeqtb[cur_chr].cint;
        return;
    } else {
        back_input();
    }
    scan_single_dir(d1);
    scan_single_dir(d2);
    if (dir_parallel(d1, d2)) {
        tex_error("Bad direction", NULL);
        cur_val = 0;
        return;
    }
    scan_single_dir(d3);
    get_x_token();
    if (cur_cmd != spacer_cmd)
        back_input();
    cur_val = d1 * 8 + dir_rearrange[d2] * 4 + d3;
}


/* We can not return |undefined_control_sequence| under some conditions
 * (inside |shift_case|, for example). This needs thinking.
 */

halfword active_to_cs(int curchr, int force)
{
    halfword curcs;
    char *a, *b;
    char *utfbytes = xmalloc(10);
    int nncs = no_new_control_sequence;
    a = u2s(0xFFFF);
    utfbytes = strcpy(utfbytes, a);
    if (force)
        no_new_control_sequence = false;
    if (curchr > 0) {
        b = u2s(curchr);
        utfbytes = strcat(utfbytes, b);
        free(b);
        curcs = string_lookup(utfbytes, strlen(utfbytes));
    } else {
        utfbytes[3] = '\0';
        curcs = string_lookup(utfbytes, 4);
    }
    no_new_control_sequence = nncs;
    free(a);
    free(utfbytes);
    return curcs;
}

/* TODO this function should listen to \.{\\escapechar} */

#define is_active_cs(a) (length(a)>3 &&                               \
                         (str_pool[str_start_macro(a)]   == 0xEF) &&  \
                         (str_pool[str_start_macro(a)+1] == 0xBF) &&  \
                         (str_pool[str_start_macro(a)+2] == 0xBF))


char *cs_to_string(pointer p)
{                               /* prints a control sequence */
    char *s;
    int k = 0;
    static char ret[256] = { 0 };
    if (p == null_cs) {
        ret[k++] = '\\';
        s = "csname";
        while (*s) {
            ret[k++] = *s++;
        }
        ret[k++] = '\\';
        s = "endcsname";
        while (*s) {
            ret[k++] = *s++;
        }
        ret[k] = 0;

    } else {
        str_number txt = zget_cs_text(p);
        s = makecstring(txt);
        if (is_active_cs(txt)) {
            s = s + 3;
            while (*s) {
                ret[k++] = *s++;
            }
            ret[k] = 0;
        } else {
            ret[k++] = '\\';
            while (*s) {
                ret[k++] = *s++;
            }
            ret[k] = 0;
        }
    }
    return (char *) ret;
}

/* TODO this is a quick hack, will be solved differently soon */

char *cmd_chr_to_string(int cmd, int chr)
{
    char *s;
    strnumber str;
    int sel = selector;
    selector = new_string;
    print_cmd_chr(cmd, chr);
    str = make_string();
    s = makecstring(str);
    selector = sel;
    flush_str(str);
    return s;
}

/* Before getting into |get_next|, let's consider the subroutine that
   is called when an `\.{\\outer}' control sequence has been scanned or
   when the end of a file has been reached. These two cases are distinguished
   by |cur_cs|, which is zero at the end of a file.
*/

static int frozen_control_sequence = 0;

#define frozen_cr (frozen_control_sequence+1)   /* permanent `\.{\\cr}' */
#define frozen_fi (frozen_control_sequence+4)   /* permanent `\.{\\fi}' */

void check_outer_validity(void)
{
    pointer p;                  /* points to inserted token list */
    pointer q;                  /* auxiliary pointer */
    if (suppress_outer_error)
        return;
    if (frozen_control_sequence == 0) {
        frozen_control_sequence = get_nullcs() + 1 + get_hash_size();   /* hashbase=nullcs+1 */
    }
    if (scanner_status != normal) {
        deletions_allowed = false;
        /* @<Back up an outer control sequence so that it can be reread@>; */
        /* An outer control sequence that occurs in a \.{\\read} will not be reread,
           since the error recovery for \.{\\read} is not very powerful. */
        if (cur_cs != 0) {
            if ((state == token_list) || (name < 1) || (name > 17)) {
                p = get_avail();
                info(p) = cs_token_flag + cur_cs;
                begin_token_list(p, backed_up); /* prepare to read the control sequence again */
            }
            cur_cmd = spacer_cmd;
            cur_chr = ' ';      /* replace it by a space */
        }
        if (scanner_status > skipping) {
            char *errhlp[] = { "I suspect you have forgotten a `}', causing me",
                "to read past where you wanted me to stop.",
                "I'll try to recover; but if the error is serious,",
                "you'd better type `E' or `X' now and fix your file.",
                NULL
            };
            char errmsg[256];
            char *startmsg, *scannermsg;
            /* @<Tell the user what has run away and try to recover@> */
            runaway();          /* print a definition, argument, or preamble */
            if (cur_cs == 0) {
                startmsg = "File ended";
            } else {
                cur_cs = 0;
                startmsg = "Forbidden control sequence found";
            }
            /* @<Print either `\.{definition}' or `\.{use}' or `\.{preamble}' or `\.{text}',
               and insert tokens that should lead to recovery@>; */
            /* The recovery procedure can't be fully understood without knowing more
               about the \TeX\ routines that should be aborted, but we can sketch the
               ideas here:  For a runaway definition we will insert a right brace; for a
               runaway preamble, we will insert a special \.{\\cr} token and a right
               brace; and for a runaway argument, we will set |long_state| to
               |outer_call| and insert \.{\\par}. */
            p = get_avail();
            switch (scanner_status) {
            case defining:
                scannermsg = "definition";
                info(p) = right_brace_token + '}';
                break;
            case matching:
                scannermsg = "use";
                info(p) = par_token;
                long_state = outer_call_cmd;
                break;
            case aligning:
                scannermsg = "preamble";
                info(p) = right_brace_token + '}';
                q = p;
                p = get_avail();
                link(p) = q;
                info(p) = cs_token_flag + frozen_cr;
                align_state = -1000000;
                break;
            case absorbing:
                scannermsg = "text";
                info(p) = right_brace_token + '}';
                break;
            default:           /* can't happen */
                scannermsg = "unknown";
                break;
            }                   /*there are no other cases */
            begin_token_list(p, inserted);
            snprintf(errmsg, 255, "%s while scanning %s of %s",
                     startmsg, scannermsg, cs_to_string(warning_index));
            tex_error(errmsg, errhlp);
        } else {
            char errmsg[256];
            char *errhlp_no[] =
                { "The file ended while I was skipping conditional text.",
                "This kind of error happens when you say `\\if...' and forget",
                "the matching `\\fi'. I've inserted a `\\fi'; this might work.",
                NULL
            };
            char *errhlp_cs[] =
                { "A forbidden control sequence occurred in skipped text.",
                "This kind of error happens when you say `\\if...' and forget",
                "the matching `\\fi'. I've inserted a `\\fi'; this might work.",
                NULL
            };
            char **errhlp = (char **) errhlp_no;
            if (cur_cs != 0) {
                errhlp = errhlp_cs;
                cur_cs = 0;
            }
            snprintf(errmsg, 255,
                     "Incomplete %s; all text was ignored after line %d",
                     cmd_chr_to_string(if_test_cmd, cur_if), skip_line);
            /* @.Incomplete \\if...@> */
            cur_tok = cs_token_flag + frozen_fi;
            /* back up one inserted token and call |error| */
            {
                OK_to_interrupt = false;
                back_input();
                token_type = inserted;
                OK_to_interrupt = true;
                tex_error(errmsg, errhlp);
            }
        }
        deletions_allowed = true;
    }
}

static boolean get_next_file(void)
{
  SWITCH:
    if (loc <= limit) {         /* current line not yet finished */
        do_buffer_to_unichar(cur_chr, loc);

      RESWITCH:
        if (detokenized_line()) {
            cur_cmd = (cur_chr == ' ' ? 10 : 12);
        } else {
            do_get_cat_code(cur_cmd);
        }
        /* 
           @<Change state if necessary, and |goto switch| if the current
           character should be ignored, or |goto reswitch| if the current
           character changes to another@>;
         */
        /* The following 48-way switch accomplishes the scanning quickly, assuming
           that a decent \PASCAL\ compiler has translated the code. Note that the numeric
           values for |mid_line|, |skip_blanks|, and |new_line| are spaced
           apart from each other by |max_char_code+1|, so we can add a character's
           command code to the state to get a single number that characterizes both.
         */
        switch (state + cur_cmd) {
        case mid_line + ignore_cmd:
        case skip_blanks + ignore_cmd:
        case new_line + ignore_cmd:
        case skip_blanks + spacer_cmd:
        case new_line + spacer_cmd:    /* @<Cases where character is ignored@> */
            goto SWITCH;
            break;
        case mid_line + escape_cmd:
        case new_line + escape_cmd:
        case skip_blanks + escape_cmd: /* @<Scan a control sequence ...@>; */
            state = scan_control_sequence();
            if (cur_cmd >= outer_call_cmd)
                check_outer_validity();
            break;
        case mid_line + active_char_cmd:
        case new_line + active_char_cmd:
        case skip_blanks + active_char_cmd:    /* @<Process an active-character  */
            cur_cs = active_to_cs(cur_chr, false);
            cur_cmd = eq_type(cur_cs);
            cur_chr = equiv(cur_cs);
            state = mid_line;
            if (cur_cmd >= outer_call_cmd)
                check_outer_validity();
            break;
        case mid_line + sup_mark_cmd:
        case new_line + sup_mark_cmd:
        case skip_blanks + sup_mark_cmd:       /* @<If this |sup_mark| starts */
            if (process_sup_mark())
                goto RESWITCH;
            else
                state = mid_line;
            break;
        case mid_line + invalid_char_cmd:
        case new_line + invalid_char_cmd:
        case skip_blanks + invalid_char_cmd:   /* @<Decry the invalid character and |goto restart|@>; */
            invalid_character_error();
            return false;       /* because state may be token_list now */
            break;
        case mid_line + spacer_cmd:    /* @<Enter |skip_blanks| state, emit a space@>; */
            state = skip_blanks;
            cur_chr = ' ';
            break;
        case mid_line + car_ret_cmd:   /* @<Finish line, emit a space@>; */
            /* When a character of type |spacer| gets through, its character code is
               changed to $\.{"\ "}=@'40$. This means that the ASCII codes for tab and space,
               and for the space inserted at the end of a line, will
               be treated alike when macro parameters are being matched. We do this
               since such characters are indistinguishable on most computer terminal displays.
             */
            loc = limit + 1;
            cur_cmd = spacer_cmd;
            cur_chr = ' ';
            break;
        case skip_blanks + car_ret_cmd:
        case mid_line + comment_cmd:
        case new_line + comment_cmd:
        case skip_blanks + comment_cmd:        /* @<Finish line, |goto switch|@>; */
            loc = limit + 1;
            goto SWITCH;
            break;
        case new_line + car_ret_cmd:   /* @<Finish line, emit a \.{\\par}@>; */
            loc = limit + 1;
            cur_cs = par_loc;
            cur_cmd = eq_type(cur_cs);
            cur_chr = equiv(cur_cs);
            if (cur_cmd >= outer_call_cmd)
                check_outer_validity();
            break;
        case skip_blanks + left_brace_cmd:
        case new_line + left_brace_cmd:
            state = mid_line;   /* fall through */
        case mid_line + left_brace_cmd:
            align_state++;
            break;
        case skip_blanks + right_brace_cmd:
        case new_line + right_brace_cmd:
            state = mid_line;   /* fall through */
        case mid_line + right_brace_cmd:
            align_state--;
            break;
        case mid_line + math_shift_cmd:
        case mid_line + tab_mark_cmd:
        case mid_line + mac_param_cmd:
        case mid_line + sub_mark_cmd:
        case mid_line + letter_cmd:
        case mid_line + other_char_cmd:
            break;
        default:
            /*
               case skip_blanks + math_shift:
               case skip_blanks + tab_mark:
               case skip_blanks + mac_param:
               case skip_blanks + sub_mark:
               case skip_blanks + letter:
               case skip_blanks + other_char:     
               case new_line    + math_shift:
               case new_line    + tab_mark:
               case new_line    + mac_param:
               case new_line    + sub_mark:
               case new_line    + letter:
               case new_line    + other_char:     
             */
            state = mid_line;
            break;
        }
    } else {
        if (current_ocp_lstack > 0) {
            pop_input();
            return false;
        }
        if (name != 21)
            state = new_line;

        /*
           @<Move to next line of file,
           or |goto restart| if there is no next line,
           or |return| if a \.{\\read} line has finished@>;
         */
        do {
            next_line_retval r = next_line();
            if (r == next_line_return) {
                return true;
            } else if (r == next_line_restart) {
                return false;
            }
        } while (0);
        check_interrupt();
        goto SWITCH;
    }
    return true;
}

#define is_hex(a) ((a>='0'&&a<='9')||(a>='a'&&a<='f'))

#define add_nybble(a)   do {                                            \
    if (a<='9') cur_chr=(cur_chr<<4)+a-'0';                             \
    else        cur_chr=(cur_chr<<4)+a-'a'+10;                          \
  } while (0)

#define hex_to_cur_chr do {                                             \
    if (c<='9')  cur_chr=c-'0';                                         \
    else         cur_chr=c-'a'+10;                                      \
    add_nybble(cc);                                                     \
  } while (0)

#define four_hex_to_cur_chr do {                                        \
    hex_to_cur_chr;                                                     \
    add_nybble(ccc); add_nybble(cccc);                                  \
  } while (0)

#define five_hex_to_cur_chr  do {                                       \
    four_hex_to_cur_chr;                                                \
    add_nybble(ccccc);                                                  \
  } while (0)

#define six_hex_to_cur_chr do {                                         \
    five_hex_to_cur_chr;                                                \
    add_nybble(cccccc);                                                 \
  } while (0)


/* Notice that a code like \.{\^\^8} becomes \.x if not followed by a hex digit.*/

static boolean process_sup_mark(void)
{
    if (cur_chr == buffer[loc]) {
        int c, cc;
        if (loc < limit) {
            if ((cur_chr == buffer[loc + 1]) && (cur_chr == buffer[loc + 2])
                && (cur_chr == buffer[loc + 3]) && (cur_chr == buffer[loc + 4])
                && ((loc + 10) <= limit)) {
                int ccc, cccc, ccccc, cccccc;   /* constituents of a possible expanded code */
                c = buffer[loc + 5];
                cc = buffer[loc + 6];
                ccc = buffer[loc + 7];
                cccc = buffer[loc + 8];
                ccccc = buffer[loc + 9];
                cccccc = buffer[loc + 10];
                if ((is_hex(c)) && (is_hex(cc)) && (is_hex(ccc))
                    && (is_hex(cccc))
                    && (is_hex(ccccc)) && (is_hex(cccccc))) {
                    loc = loc + 11;
                    six_hex_to_cur_chr;
                    return true;
                }
            }
            if ((cur_chr == buffer[loc + 1]) && (cur_chr == buffer[loc + 2])
                && (cur_chr == buffer[loc + 3]) && ((loc + 8) <= limit)) {
                int ccc, cccc, ccccc;   /* constituents of a possible expanded code */
                c = buffer[loc + 4];
                cc = buffer[loc + 5];
                ccc = buffer[loc + 6];
                cccc = buffer[loc + 7];
                ccccc = buffer[loc + 8];
                if ((is_hex(c)) && (is_hex(cc)) && (is_hex(ccc))
                    && (is_hex(cccc)) && (is_hex(ccccc))) {
                    loc = loc + 9;
                    five_hex_to_cur_chr;
                    return true;
                }
            }
            if ((cur_chr == buffer[loc + 1]) && (cur_chr == buffer[loc + 2])
                && ((loc + 6) <= limit)) {
                int ccc, cccc;  /* constituents of a possible expanded code */
                c = buffer[loc + 3];
                cc = buffer[loc + 4];
                ccc = buffer[loc + 5];
                cccc = buffer[loc + 6];
                if ((is_hex(c)) && (is_hex(cc)) && (is_hex(ccc))
                    && (is_hex(cccc))) {
                    loc = loc + 7;
                    four_hex_to_cur_chr;
                    return true;
                }
            }
            c = buffer[loc + 1];
            if (c < 0200) {     /* yes we have an expanded char */
                loc = loc + 2;
                if (is_hex(c) && loc <= limit) {
                    cc = buffer[loc];
                    if (is_hex(cc)) {
                        incr(loc);
                        hex_to_cur_chr;
                        return true;
                    }
                }
                cur_chr = (c < 0100 ? c + 0100 : c - 0100);
                return true;
            }
        }
    }
    return false;
}

/* Control sequence names are scanned only when they appear in some line of
   a file; once they have been scanned the first time, their |eqtb| location
   serves as a unique identification, so \TeX\ doesn't need to refer to the
   original name any more except when it prints the equivalent in symbolic form.
   
   The program that scans a control sequence has been written carefully
   in order to avoid the blowups that might otherwise occur if a malicious
   user tried something like `\.{\\catcode\'15=0}'. The algorithm might
   look at |buffer[limit+1]|, but it never looks at |buffer[limit+2]|.

   If expanded characters like `\.{\^\^A}' or `\.{\^\^df}'
   appear in or just following
   a control sequence name, they are converted to single characters in the
   buffer and the process is repeated, slowly but surely.
*/

static boolean check_expanded_code(integer * kk);       /* below */

static int scan_control_sequence(void)
{
    int retval = mid_line;
    if (loc > limit) {
        cur_cs = null_cs;       /* |state| is irrelevant in this case */
    } else {
        register int cat;       /* |cat_code(cur_chr)|, usually */
        while (1) {
            integer k = loc;
            do_buffer_to_unichar(cur_chr, k);
            do_get_cat_code(cat);
            if (cat != letter_cmd || k > limit) {
                retval = (cat == spacer_cmd ? skip_blanks : mid_line);
                if (cat == sup_mark_cmd && check_expanded_code(&k))     /* @<If an expanded...@>; */
                    continue;
            } else {
                retval = skip_blanks;
                do {
                    do_buffer_to_unichar(cur_chr, k);
                    do_get_cat_code(cat);
                } while (cat == letter_cmd && k <= limit);

                if (cat == sup_mark_cmd && check_expanded_code(&k))     /* @<If an expanded...@>; */
                    continue;
                if (cat != letter_cmd) {
                    decr(k);
                    if (cur_chr > 0xFFFF)
                        decr(k);
                    if (cur_chr > 0x7FF)
                        decr(k);
                    if (cur_chr > 0x7F)
                        decr(k);
                }               /* now |k| points to first nonletter */
            }
            cur_cs = id_lookup(loc, k - loc);
            loc = k;
            break;
        }
    }
    cur_cmd = eq_type(cur_cs);
    cur_chr = equiv(cur_cs);
    return retval;
}

/* Whenever we reach the following piece of code, we will have
   |cur_chr=buffer[k-1]| and |k<=limit+1| and |cat=get_cat_code(cat_code_table,cur_chr)|. If an
   expanded code like \.{\^\^A} or \.{\^\^df} appears in |buffer[(k-1)..(k+1)]|
   or |buffer[(k-1)..(k+2)]|, we
   will store the corresponding code in |buffer[k-1]| and shift the rest of
   the buffer left two or three places.
*/

static boolean check_expanded_code(integer * kk)
{
    int l;
    int k = *kk;
    int d = 1;                  /* number of excess characters in an expanded code */
    int c, cc, ccc, cccc, ccccc, cccccc;        /* constituents of a possible expanded code */
    if (buffer[k] == cur_chr && k < limit) {
        if ((cur_chr == buffer[k + 1]) && (cur_chr == buffer[k + 2])
            && ((k + 6) <= limit)) {
            d = 4;
            if ((cur_chr == buffer[k + 3]) && ((k + 8) <= limit))
                d = 5;
            if ((cur_chr == buffer[k + 4]) && ((k + 10) <= limit))
                d = 6;
            c = buffer[k + d - 1];
            cc = buffer[k + d];
            ccc = buffer[k + d + 1];
            cccc = buffer[k + d + 2];
            if (d == 6) {
                ccccc = buffer[k + d + 3];
                cccccc = buffer[k + d + 4];
                if (is_hex(c) && is_hex(cc) && is_hex(ccc) && is_hex(cccc)
                    && is_hex(ccccc) && is_hex(cccccc))
                    six_hex_to_cur_chr;
            } else if (d == 5) {
                ccccc = buffer[k + d + 3];
                if (is_hex(c) && is_hex(cc) && is_hex(ccc) && is_hex(cccc)
                    && is_hex(ccccc))
                    five_hex_to_cur_chr;
            } else {
                if (is_hex(c) && is_hex(cc) && is_hex(ccc) && is_hex(cccc))
                    four_hex_to_cur_chr;
            }
        } else {
            c = buffer[k + 1];
            if (c < 0200) {
                d = 1;
                if (is_hex(c) && (k + 2) <= limit) {
                    cc = buffer[k + 2];
                    if (is_hex(c) && is_hex(cc)) {
                        d = 2;
                        hex_to_cur_chr;
                    }
                } else if (c < 0100) {
                    cur_chr = c + 0100;
                } else {
                    cur_chr = c - 0100;
                }
            }
        }
        if (d > 2)
            d = 2 * d - 1;
        else
            d++;
        if (cur_chr <= 0x7F) {
            buffer[k - 1] = cur_chr;
        } else if (cur_chr <= 0x7FF) {
            buffer[k - 1] = 0xC0 + cur_chr / 0x40;
            k++;
            d--;
            buffer[k - 1] = 0x80 + cur_chr % 0x40;
        } else if (cur_chr <= 0xFFFF) {
            buffer[k - 1] = 0xE0 + cur_chr / 0x1000;
            k++;
            d--;
            buffer[k - 1] = 0x80 + (cur_chr % 0x1000) / 0x40;
            k++;
            d--;
            buffer[k - 1] = 0x80 + (cur_chr % 0x1000) % 0x40;
        } else {
            buffer[k - 1] = 0xF0 + cur_chr / 0x40000;
            k++;
            d--;
            buffer[k - 1] = 0x80 + (cur_chr % 0x40000) / 0x1000;
            k++;
            d--;
            buffer[k - 1] = 0x80 + ((cur_chr % 0x40000) % 0x1000) / 0x40;
            k++;
            d--;
            buffer[k - 1] = 0x80 + ((cur_chr % 0x40000) % 0x1000) % 0x40;
        }
        l = k;
        limit = limit - d;
        while (l <= limit) {
            buffer[l] = buffer[l + d];
            l++;
        }
        *kk = k;
        return true;
    }
    return false;
}

#define end_line_char_inactive ((end_line_char<0)||(end_line_char>127))

/* The global variable |force_eof| is normally |false|; it is set |true|
  by an \.{\\endinput} command.

 @<Glob...@>=
 @!force_eof:boolean; {should the next \.{\\input} be aborted early?}

*/


/* All of the easy branches of |get_next| have now been taken care of.
  There is one more branch.
*/

static next_line_retval next_line(void)
{
    boolean inhibit_eol = false;        /* a way to end a pseudo file without trailing space */
    if (name > 17) {
        /* @<Read next line of file into |buffer|, or |goto restart| if the file has ended@> */
        incr(line);
        first = start;
        if (!force_eof) {
            if (name <= 20) {
                if (pseudo_input()) {   /* not end of file */
                    firm_up_the_line(); /* this sets |limit| */
                    line_catcode_table = DEFAULT_CAT_TABLE;
                    if ((name == 19) && (pseudo_lines(pseudo_files) == null))
                        inhibit_eol = true;
                } else if ((every_eof != null) && !eof_seen[index]) {
                    limit = first - 1;
                    eof_seen[index] = true;     /* fake one empty line */
                    if (name != 19)
                        begin_token_list(every_eof, every_eof_text);
                    return next_line_restart;
                } else {
                    force_eof = true;
                }
            } else {
                if (name == 21) {
                    if (luacstring_input()) {   /* not end of strings  */
                        firm_up_the_line();
                        line_catcode_table = luacstring_cattable();
                        line_partial = luacstring_partial();
                        if (luacstring_final_line() || line_partial
                            || line_catcode_table == NO_CAT_TABLE)
                            inhibit_eol = true;
                        if (!line_partial)
                            state = new_line;
                    } else {
                        force_eof = true;
                    }
                } else {
                    if (lua_input_ln(cur_file, 0, true)) {      /* not end of file */
                        firm_up_the_line();     /* this sets |limit| */
                        line_catcode_table = DEFAULT_CAT_TABLE;
                    } else if ((every_eof != null) && (!eof_seen[index])) {
                        limit = first - 1;
                        eof_seen[index] = true; /* fake one empty line */
                        begin_token_list(every_eof, every_eof_text);
                        return next_line_restart;
                    } else {
                        force_eof = true;
                    }
                }
            }
        }
        if (force_eof) {
            if (tracing_nesting > 0)
                if ((grp_stack[in_open] != cur_boundary)
                    || (if_stack[in_open] != cond_ptr))
                    if (!((name == 19) || (name = 21)))
                        file_warning(); /* give warning for some unfinished groups and/or conditionals */
            if ((name > 21) || (name == 20)) {
                if (tracefilenames)
                    print_char(')');
                open_parens--;
                /* update_terminal(); *//* show user that file has been read */
            }
            force_eof = false;
            if (name == 21 ||   /* lua input */
                name == 19) {   /* \scantextokens */
                end_file_reading();
            } else {
                end_file_reading();
                check_outer_validity();
            }
            return next_line_restart;
        }
        if (inhibit_eol || end_line_char_inactive)
            limit--;
        else
            buffer[limit] = end_line_char;
        first = limit + 1;
        loc = start;            /* ready to read */
    } else {
        if (!terminal_input) {  /* \.{\\read} line has ended */
            cur_cmd = 0;
            cur_chr = 0;
            return next_line_return;    /* OUTER */
        }
        if (input_ptr > 0) {    /* text was inserted during error recovery */
            end_file_reading();
            return next_line_restart;   /* resume previous level */
        }
        if (selector < log_only)
            open_log_file();
        if (interaction > nonstop_mode) {
            if (end_line_char_inactive)
                limit++;
            if (limit == start) {       /* previous line was empty */
                tprint_nl("(Please type a command or say `\\end')");
            }
            print_ln();
            first = start;
            prompt_input((str_number) '*');     /* input on-line into |buffer| */
            limit = last;
            if (end_line_char_inactive)
                limit--;
            else
                buffer[limit] = end_line_char;
            first = limit + 1;
            loc = start;
        } else {
            fatal_error(maketexstring
                        ("*** (job aborted, no legal \\end found)"));
            /* nonstop mode, which is intended for overnight batch processing,
               never waits for on-line input */
        }
    }
    return next_line_ok;
}




/* Let's consider now what happens when |get_next| is looking at a token list. */

static boolean get_next_tokenlist(void)
{
    register halfword t;        /* a token */
    t = info(loc);
    loc = link(loc);            /* move to next */
    if (t >= cs_token_flag) {   /* a control sequence token */
        cur_cs = t - cs_token_flag;
        cur_cmd = eq_type(cur_cs);
        if (cur_cmd >= outer_call_cmd) {
            if (cur_cmd == dont_expand_cmd) {   /* @<Get the next token, suppressing expansion@> */
                /* The present point in the program is reached only when the |expand|
                   routine has inserted a special marker into the input. In this special
                   case, |info(loc)| is known to be a control sequence token, and |link(loc)=null|.
                 */
                cur_cs = info(loc) - cs_token_flag;
                loc = null;
                cur_cmd = eq_type(cur_cs);
                if (cur_cmd > max_command_cmd) {
                    cur_cmd = relax_cmd;
                    cur_chr = no_expand_flag;
                    return true;
                }
            } else {
                check_outer_validity();
            }
        }
        cur_chr = equiv(cur_cs);
    } else {
        cur_cmd = t >> string_offset_bits;      /* cur_cmd=t / string_offset; */
        cur_chr = t & (string_offset - 1);      /* cur_chr=t % string_offset; */
        switch (cur_cmd) {
        case left_brace_cmd:
            align_state++;
            break;
        case right_brace_cmd:
            align_state--;
            break;
        case out_param_cmd:    /* @<Insert macro parameter and |goto restart|@>; */
            begin_token_list(param_stack[param_start + cur_chr - 1], parameter);
            return false;
            break;
        }
    }
    return true;
}

/* Now we're ready to take the plunge into |get_next| itself. Parts of
   this routine are executed more often than any other instructions of \TeX.
   @^mastication@>@^inner loop@>
*/

/* sets |cur_cmd|, |cur_chr|, |cur_cs| to next token */

void get_next(void)
{
  RESTART:
    cur_cs = 0;
    if (state != token_list) {
        /* Input from external file, |goto restart| if no input found */
        if (!get_next_file())
            goto RESTART;
    } else {
        if (loc == null) {
            end_token_list();
            goto RESTART;       /* list exhausted, resume previous level */
        } else if (!get_next_tokenlist()) {
            goto RESTART;       /* parameter needs to be expanded */
        }
    }
    /* @<If an alignment entry has just ended, take appropriate action@> */
    if ((cur_cmd == tab_mark_cmd || cur_cmd == car_ret_cmd) && align_state == 0) {
        insert_vj_template();
        goto RESTART;
    }
}

void get_token_lua(void)
{
    register int callback_id;
    callback_id = callback_defined(token_filter_callback);
    if (callback_id != 0) {
        while (state == token_list && loc == null && index != v_template)
            end_token_list();
        /* there is some stuff we don't want to see inside the callback */
        if (!(state == token_list &&
              ((nofilter == true) || (index == backed_up && loc != null)))) {
            do_get_token_lua(callback_id);
            return;
        }
    }
    get_next();
}
