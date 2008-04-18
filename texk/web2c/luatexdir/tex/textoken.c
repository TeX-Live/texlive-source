/* $Id: textoken.c 1155 2008-04-14 07:53:21Z oneiros $ */

#include "luatex-api.h"
#include <ptexlib.h>

#include "tokens.h"

/* Integer parameters and other command-related defines. This needs it's own header file! */

#define end_line_char_code 48   /* character placed at the right end of the buffer */
#define cat_code_table_code 63
#define tex_int_pars 66         /* total number of \.{\\TeX} + Aleph integer parameters */
/* this is not what happens in the pascal code! there the values shift from bare numbers to offsets ! */
#define dir_base tex_int_pars
#define dir_pars 5
#define pdftex_first_integer_code dir_base+dir_pars     /* base for \pdfTeX's integer parameters */
#define pdf_int_pars pdftex_first_integer_code+27       /*total number of \pdfTeX's integer parameters */
#define etex_int_base pdf_int_pars      /*base for \eTeX's integer parameters */
#define tracing_nesting_code etex_int_base+4    /*show incomplete groups and ifs within files */

#define int_par(a) zeqtb[static_int_base+a].cint        /* an integer parameter */
#define cat_code_table int_par(cat_code_table_code)
#define tracing_nesting int_par(tracing_nesting_code)
#define end_line_char int_par(end_line_char_code)

#define every_eof get_every_eof()

#define number_active_chars 65536+65536
#define active_base 1           /* beginning of region 1, for active character equivalents */
#define null_cs active_base+number_active_chars /* equivalent of \.{\\csname\\endcsname} */


#define eq_level(a) zeqtb[a].hh.u.B1
#define eq_type(a)  zeqtb[a].hh.u.B0
#define equiv(a)    zeqtb[a].hh.v.RH

/* leave an input level, re-enter the old */
#define pop_input() cur_input=input_stack[--input_ptr]

#define nonstop_mode 1

/* command codes */
#define relax 0
#define out_param 5
#define max_command 117         /* fetched from C output */
#define dont_expand 133         /* fetched from C output */

#define terminal_input (name==0)        /* are we reading from the terminal? */
#define special_char 1114113    /* |biggest_char+2| */
#define cur_file input_file[index]      /* the current |alpha_file| variable */

#define no_expand_flag special_char     /*this characterizes a special variant of |relax| */

extern void insert_vj_template(void);

#define do_get_cat_code(a) do {						\
    if (local_catcode_table)						\
      a=get_cat_code(line_catcode_table,cur_chr);			\
    else								\
      a=get_cat_code(cat_code_table,cur_chr);				\
  } while (0)


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

/* no longer done */

static boolean process_sup_mark(void);  /* below */

static int scan_control_sequence(void); /* below */

typedef enum { next_line_ok, next_line_return,
        next_line_restart } next_line_retval;

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

static boolean get_next_file(void)
{
  SWITCH:
    if (loc <= limit) {         /* current line not yet finished */
        cur_chr = qbuffer_to_unichar(&loc);
      RESWITCH:
        if (detokenized_line) {
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
        case mid_line + ignore:
        case skip_blanks + ignore:
        case new_line + ignore:
        case skip_blanks + spacer:
        case new_line + spacer:        /* @<Cases where character is ignored@> */
            goto SWITCH;
            break;
        case mid_line + escape:
        case new_line + escape:
        case skip_blanks + escape:     /* @<Scan a control sequence ...@>; */
            state = scan_control_sequence();
            break;
        case mid_line + active_char:
        case new_line + active_char:
        case skip_blanks + active_char:        /* @<Process an active-character  */
            cur_cs = cur_chr + active_base;
            cur_cmd = eq_type(cur_cs);
            cur_chr = equiv(cur_cs);
            state = mid_line;
            break;
        case mid_line + sup_mark:
        case new_line + sup_mark:
        case skip_blanks + sup_mark:   /* @<If this |sup_mark| starts */
            if (process_sup_mark())
                goto RESWITCH;
            else
                state = mid_line;
            break;
        case mid_line + invalid_char:
        case new_line + invalid_char:
        case skip_blanks + invalid_char:       /* @<Decry the invalid character and |goto restart|@>; */
            invalid_character_error();
            return false;       /* because state may be token_list now */
            break;
        case mid_line + spacer:        /* @<Enter |skip_blanks| state, emit a space@>; */
            state = skip_blanks;
            cur_chr = ' ';
            break;
        case mid_line + car_ret:       /* @<Finish line, emit a space@>; */
            /* When a character of type |spacer| gets through, its character code is
               changed to $\.{"\ "}=@'40$. This means that the ASCII codes for tab and space,
               and for the space inserted at the end of a line, will
               be treated alike when macro parameters are being matched. We do this
               since such characters are indistinguishable on most computer terminal displays.
             */
            loc = limit + 1;
            cur_cmd = spacer;
            cur_chr = ' ';
            break;
        case skip_blanks + car_ret:
        case mid_line + comment:
        case new_line + comment:
        case skip_blanks + comment:    /* @<Finish line, |goto switch|@>; */
            loc = limit + 1;
            goto SWITCH;
            break;
        case new_line + car_ret:       /* @<Finish line, emit a \.{\\par}@>; */
            loc = limit + 1;
            cur_cs = par_loc;
            cur_cmd = eq_type(cur_cs);
            cur_chr = equiv(cur_cs);
            break;
        case skip_blanks + left_brace:
        case new_line + left_brace:
            state = mid_line;   /* fall through */
        case mid_line + left_brace:
            align_state++;
            break;
        case skip_blanks + right_brace:
        case new_line + right_brace:
            state = mid_line;   /* fall through */
        case mid_line + right_brace:
            align_state--;
            break;
        case mid_line + math_shift:
        case mid_line + tab_mark:
        case mid_line + mac_param:
        case mid_line + sub_mark:
        case mid_line + letter:
        case mid_line + other_char:
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

#define add_nybble(a)	do {						\
    if (a<='9') cur_chr=(cur_chr<<4)+a-'0';				\
    else        cur_chr=(cur_chr<<4)+a-'a'+10;				\
  } while (0)

#define hex_to_cur_chr do {						\
    if (c<='9')  cur_chr=c-'0';						\
    else         cur_chr=c-'a'+10;					\
    add_nybble(cc);							\
  } while (0)

#define four_hex_to_cur_chr do {					\
    hex_to_cur_chr;							\
    add_nybble(ccc); add_nybble(cccc);					\
  } while (0)

#define five_hex_to_cur_chr  do {					\
    four_hex_to_cur_chr;						\
    add_nybble(ccccc);							\
  } while (0)

#define six_hex_to_cur_chr do {						\
    five_hex_to_cur_chr;						\
    add_nybble(cccccc);							\
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
            cur_chr = qbuffer_to_unichar(&k);
            do_get_cat_code(cat);
            if (cat != letter || k > limit) {
                retval = (cat == spacer ? skip_blanks : mid_line);
                if (cat == sup_mark && check_expanded_code(&k)) /* @<If an expanded...@>; */
                    continue;
            } else {
                retval = skip_blanks;
                do {
                    cur_chr = qbuffer_to_unichar(&k);
                    do_get_cat_code(cat);
                } while (cat == letter && k <= limit);

                if (cat == sup_mark && check_expanded_code(&k)) /* @<If an expanded...@>; */
                    continue;
                if (cat != letter) {
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
    integer tab;                /* a category table */
    boolean inhibit_eol = false;        /* a way to end a pseudo file without trailing space */
    detokenized_line = false;
    local_catcode_table = false;
    if (name > 17) {
        /* @<Read next line of file into |buffer|, or |goto restart| if the file has ended@> */
        incr(line);
        first = start;
        if (!force_eof) {
            if (name <= 20) {
                if (pseudo_input()) {   /* not end of file */
                    firm_up_the_line(); /* this sets |limit| */
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
                        if ((luacstring_penultimate()) || (luacstring_simple()))
                            inhibit_eol = true;
                        if (!luacstring_simple())
                            state = new_line;
                        if (luacstring_detokenized()) {
                            inhibit_eol = true;
                            detokenized_line = true;
                        } else {
                            if (!luacstring_defaultcattable()) {
                                tab = luacstring_cattable();
                                if (valid_catcode_table(tab)) {
                                    local_catcode_table = true;
                                    line_catcode_table = tab;
                                }
                            }
                        }
                    } else {
                        force_eof = true;
                    }
                } else {
                    if (lua_input_ln(cur_file, 0, true)) {      /* not end of file */
                        firm_up_the_line();     /* this sets |limit| */
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
            end_file_reading();
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
        if (cur_cmd == dont_expand) {   /* @<Get the next token, suppressing expansion@> */
            /* The present point in the program is reached only when the |expand|
               routine has inserted a special marker into the input. In this special
               case, |info(loc)| is known to be a control sequence token, and |link(loc)=null|.
             */
            cur_cs = info(loc) - cs_token_flag;
            loc = null;
            cur_cmd = eq_type(cur_cs);
            if (cur_cmd > max_command) {
                cur_cmd = relax;
                cur_chr = no_expand_flag;
                return true;
            }
        }
        cur_chr = equiv(cur_cs);
    } else {
        cur_cmd = t >> string_offset_bits;      /* cur_cmd=t / string_offset; */
        cur_chr = t & (string_offset - 1);      /* cur_chr=t % string_offset; */
        switch (cur_cmd) {
        case left_brace:
            align_state++;
            break;
        case right_brace:
            align_state--;
            break;
        case out_param:        /* @<Insert macro parameter and |goto restart|@>; */
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
    if ((cur_cmd == tab_mark || cur_cmd == car_ret) && align_state == 0) {
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
