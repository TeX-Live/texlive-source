/* texdeffont.c

   Copyright 2009 Taco Hoekwater <taco@luatex.org>

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
#include "nodes.h"
#include "commands.h"

static const char _svn_version[] =
    "$Id: texdeffont.c 2086 2009-03-22 15:32:08Z oneiros $ $URL: http://scm.foundry.supelec.fr/svn/luatex/trunk/src/texk/web2c/luatexdir/tex/texdeffont.c $";

#define text(a) hash[(a)].rh    /* string number for control sequence name */
#define null_cs 1               /* equivalent of \.{\\csname\\endcsname} */
#define scan_normal_dimen() scan_dimen(false,false,false)

char *scaled_to_string(scaled s)
{                               /* prints scaled real, rounded to five digits */
    static char result[16];
    int n, k;
    scaled delta;               /* amount of allowable inaccuracy */
    k = 0;
    if (s < 0) {
        result[k++] = '-';
        s = -s;                 /* print the sign, if negative */
    }
    {
        int l = 0;
        char dig[8] = { 0 };
        n = s / unity;
        /* process the integer part */
        do {
            dig[l++] = n % 10;
            n = n / 10;;
        } while (n > 0);
        while (l > 0) {
            result[k++] = (dig[--l] + '0');
        }
    }
    result[k++] = '.';
    s = 10 * (s % unity) + 5;
    delta = 10;
    do {
        if (delta > unity)
            s = s + 0100000 - 050000;   /* round the last digit */
        result[k++] = '0' + (s / unity);
        s = 10 * (s % unity);
        delta = delta * 10;
    } while (s > delta);

    result[k] = 0;
    return (char *) result;
}

void tex_def_font(small_number a)
{
    pointer u;                  /* user's font identifier */
    internal_font_number f;     /* runs through existing fonts */
    str_number t;               /* name for the frozen font identifier */
    int old_setting;            /* holds |selector| setting */
    integer offset = 0;
    scaled s = -1000;           /* stated ``at'' size, or negative of scaled magnification */
    integer natural_dir = -1;   /* the natural direction of the font */
    if (job_name == 0)
        open_log_file();        /* avoid confusing \.{texput} with the font name */
    get_r_token();
    u = cur_cs;
    if (u >= null_cs)
        t = text(u);
    else
        t = maketexstring("FONT");
    if (a >= 4) {
        geq_define(u, set_font_cmd, null_font);
    } else {
        eq_define(u, set_font_cmd, null_font);
    }
    scan_optional_equals();
    /* @<Get the next non-blank non-call token@>; */
    do {
        get_x_token();
    } while ((cur_cmd == spacer_cmd) || (cur_cmd == relax_cmd));

    if (cur_cmd != left_brace_cmd) {
        back_input();
        scan_file_name();
        if (cur_area != get_nullstr() || cur_ext != get_nullstr()) {
            /* Have to do some rescue-ing here, fonts only have a name,
               no area nor extension */
            old_setting = selector;
            selector = new_string;
            if (cur_area != get_nullstr()) {
                print(cur_area);
            }
            if (cur_name != get_nullstr()) {
                print(cur_name);
            }
            if (cur_ext != get_nullstr()) {
                print(cur_ext);
            }
            selector = old_setting;
            cur_name = make_string();
            cur_ext = get_nullstr();
            cur_area = get_nullstr();
        }
    } else {
        back_input();
        (void) scan_toks(false, true);
        old_setting = selector;
        selector = new_string;
        token_show(def_ref);
        selector = old_setting;
        flush_list(def_ref);
        /* str_room(1); *//* what did that do ? */
        cur_name = make_string();
        cur_ext = get_nullstr();
        cur_area = get_nullstr();
    }
    /* @<Scan the font size specification@>; */
    name_in_progress = true;    /* this keeps |cur_name| from being changed */
    if (scan_keyword("at")) {
        /* @<Put the \(p)(positive) `at' size into |s|@> */
        scan_normal_dimen();
        s = cur_val;
        if ((s <= 0) || (s >= 01000000000)) {
            char err[256];
            char *errhelp[] =
                { "I can only handle fonts at positive sizes that are",
                "less than 2048pt, so I've changed what you said to 10pt.",
                NULL
            };
            snprintf(err, 255, "Improper `at' size (%spt), replaced by 10pt",
                     scaled_to_string(s));
            tex_error(err, errhelp);
            s = 10 * unity;
        }
    } else if (scan_keyword("scaled")) {
        scan_int();
        s = -cur_val;
        if ((cur_val <= 0) || (cur_val > 32768)) {
            char err[256];
            char *errhelp[] =
                { "The magnification ratio must be between 1 and 32768.",
      NULL };
            snprintf(err, 255,
                     "Illegal magnification has been changed to 1000 (%d)",
                     (int) cur_val);
            tex_error(err, errhelp);
            s = -1000;
        }
    }
    if (scan_keyword("offset")) {
        scan_int();
        offset = cur_val;
        if (cur_val < 0) {
            char err[256];
            char *errhelp[] = { "The offset must be bigger than 0.", NULL };
            snprintf(err, 255, "Illegal offset has been changed to 0 (%d)",
                     (int) cur_val);
            tex_error(err, errhelp);
            offset = 0;
        }
    }
    if (scan_keyword("naturaldir")) {
        scan_direction();
        natural_dir = cur_val;
    }
    name_in_progress = false;
    f = read_font_info(u, cur_name, s, natural_dir);
    equiv(u) = f;
    zeqtb[get_font_id_base() + f] = zeqtb[u];
    text(get_font_id_base() + f) = t;
}
