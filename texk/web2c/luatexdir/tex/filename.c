/* filename.c

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
#include "commands.h"

static const char _svn_version[] =
    "$Id: filename.c 2086 2009-03-22 15:32:08Z oneiros $ $URL: http://scm.foundry.supelec.fr/svn/luatex/trunk/src/texk/web2c/luatexdir/tex/filename.c $";

#define wake_up_terminal() ;
#define clear_terminal() ;

#define cur_length (pool_ptr - str_start_macro(str_ptr))

#define batch_mode 0            /* omits all stops and omits terminal output */
#define nonstop_mode 1          /* omits all stops */
#define scroll_mode 2           /* omits error stops */
#define error_stop_mode 3       /* stops at every opportunity to interact */
#define biggest_char 1114111

/* put |ASCII_code| \# at the end of |str_pool| */
#define append_char(A) str_pool[pool_ptr++]=(A)
#define str_room(A) check_pool_overflow((pool_ptr+(A)))

/*
  In order to isolate the system-dependent aspects of file names, the
  @^system dependencies@>
  system-independent parts of \TeX\ are expressed in terms
  of three system-dependent
  procedures called |begin_name|, |more_name|, and |end_name|. In
  essence, if the user-specified characters of the file name are $c_1\ldots c_n$,
  the system-independent driver program does the operations
  $$|begin_name|;\,|more_name|(c_1);\,\ldots\,;\,|more_name|(c_n);
  \,|end_name|.$$
  These three procedures communicate with each other via global variables.
  Afterwards the file name will appear in the string pool as three strings
  called |cur_name|\penalty10000\hskip-.05em,
  |cur_area|, and |cur_ext|; the latter two are null (i.e.,
  |""|), unless they were explicitly specified by the user.
  
  Actually the situation is slightly more complicated, because \TeX\ needs
  to know when the file name ends. The |more_name| routine is a function
  (with side effects) that returns |true| on the calls |more_name|$(c_1)$,
  \dots, |more_name|$(c_{n-1})$. The final call |more_name|$(c_n)$
  returns |false|; or, it returns |true| and the token following $c_n$ is
  something like `\.{\\hbox}' (i.e., not a character). In other words,
  |more_name| is supposed to return |true| unless it is sure that the
  file name has been completely scanned; and |end_name| is supposed to be able
  to finish the assembly of |cur_name|, |cur_area|, and |cur_ext| regardless of
  whether $|more_name|(c_n)$ returned |true| or |false|.
*/

/* Here now is the first of the system-dependent routines for file name scanning. 
   @^system dependencies@> */

static void begin_name(void)
{
    area_delimiter = 0;
    ext_delimiter = 0;
    quoted_filename = false;
}

/* And here's the second. The string pool might change as the file name is
   being scanned, since a new \.{\\csname} might be entered; therefore we keep
   |area_delimiter| and |ext_delimiter| relative to the beginning of the current
   string, instead of assigning an absolute address like |pool_ptr| to them.
   @^system dependencies@> */

static boolean more_name(ASCII_code c)
{
    if (c == ' ' && stop_at_space && (!quoted_filename)) {
        return false;
    } else if (c == '"') {
        quoted_filename = !quoted_filename;
        return true;
    } else {
        str_room(1);
        append_char(c);         /* contribute |c| to the current string */
        if (ISDIRSEP(c)) {
            area_delimiter = cur_length;
            ext_delimiter = 0;
        } else if (c == '.')
            ext_delimiter = cur_length;
        return true;
    }
}

/* The third.
   @^system dependencies@>

*/

static void end_name(void)
{
    if (str_ptr + 3 > (max_strings + string_offset))
        overflow_string("number of strings", max_strings - init_str_ptr);
    /* @:TeX capacity exceeded number of strings}{\quad number of strings@> */

    if (area_delimiter == 0) {
        cur_area = get_nullstr();
    } else {
        cur_area = str_ptr;
        str_start_macro(str_ptr + 1) =
            str_start_macro(str_ptr) + area_delimiter;
        incr(str_ptr);
    }
    if (ext_delimiter == 0) {
        cur_ext = get_nullstr();
        cur_name = make_string();
    } else {
        cur_name = str_ptr;
        str_start_macro(str_ptr + 1) =
            str_start_macro(str_ptr) + ext_delimiter - area_delimiter - 1;
        incr(str_ptr);
        cur_ext = make_string();
    }
}

/* Now let's consider the ``driver'' routines by which \TeX\ deals with file names
   in a system-independent manner.  First comes a procedure that looks for a
   file name in the input by calling |get_x_token| for the information.
*/

void scan_file_name(void)
{
    name_in_progress = true;
    begin_name();
    /* @<Get the next non-blank non-call token@>; */
    do {
        get_x_token();
    } while ((cur_cmd == spacer_cmd) || (cur_cmd == relax_cmd));

    while (true) {
        if ((cur_cmd > other_char_cmd) || (cur_chr > biggest_char)) {   /* not a character */
            back_input();
            break;
        }
        /* If |cur_chr| is a space and we're not scanning a token list, check
           whether we're at the end of the buffer. Otherwise we end up adding
           spurious spaces to file names in some cases. */
        if ((cur_chr == ' ') && (state != token_list) && (loc > limit)
            && !quoted_filename)
            break;
        if (!more_name(cur_chr))
            break;
        get_x_token();
    }
    end_name();
    name_in_progress = false;
}


/*
  Here is a routine that manufactures the output file names, assuming that
  |job_name<>0|. It ignores and changes the current settings of |cur_area|
  and |cur_ext|.
*/

#define pack_cur_name() pack_file_name(cur_name,cur_area,cur_ext)

void pack_job_name(char *s)
{                               /* |s = ".log"|, |".dvi"|, or |format_extension| */
    cur_area = get_nullstr();
    cur_ext = maketexstring(s);
    cur_name = job_name;
    pack_cur_name();
}

/* If some trouble arises when \TeX\ tries to open a file, the following
   routine calls upon the user to supply another file name. Parameter~|s|
   is used in the error message to identify the type of file; parameter~|e|
   is the default extension if none is given. Upon exit from the routine,
   variables |cur_name|, |cur_area|, |cur_ext|, and |nameoffile| are
   ready for another attempt at file opening.
*/

void prompt_file_name(char *s, char *e)
{
    int k;                      /* index into |buffer| */
    str_number saved_cur_name;  /* to catch empty terminal input */
    char prompt[256];
    str_number texprompt;
    char *ar, *na, *ex;
    saved_cur_name = cur_name;
    if (interaction == scroll_mode) {
        wake_up_terminal();
    }
    ar = xstrdup(makecstring(cur_area));
    na = xstrdup(makecstring(cur_name));
    ex = xstrdup(makecstring(cur_ext));
    if (strcmp(s, "input file name") == 0) {    /* @.I can't find file x@> */
        snprintf(prompt, 255, "I can't find file `%s%s%s'.", ar, na, ex);
    } else {                    /*@.I can't write on file x@> */
        snprintf(prompt, 255, "I can't write on file `%s%s%s'.", ar, na, ex);
    }
    free(ar);
    free(na);
    free(ex);
    texprompt = maketexstring((char *) prompt);
    do_print_err(texprompt);
    flush_str(texprompt);
    if ((strcmp(e, ".tex") == 0) || (strcmp(e, "") == 0))
        show_context();
    tprint_nl("Please type another ");  /*@.Please type...@> */
    tprint(s);
    if (interaction < scroll_mode)
        fatal_error(maketexstring
                    ("*** (job aborted, file error in nonstop mode)"));
    clear_terminal();
    texprompt = maketexstring(": ");
    prompt_input(texprompt);
    flush_str(texprompt);
    begin_name();
    k = first;
    while ((buffer[k] == ' ') && (k < last))
        k++;
    while (true) {
        if (k == last)
            break;
        if (!more_name(buffer[k]))
            break;
        k++;
    }
    end_name();
    if (cur_ext == get_nullstr())
        cur_ext = maketexstring(e);
    if (length(cur_name) == 0)
        cur_name = saved_cur_name;
    pack_cur_name();
}


str_number make_name_string(void)
{
    int k;                      /* index into |nameoffile| */
    pool_pointer save_area_delimiter, save_ext_delimiter;
    boolean save_name_in_progress, save_stop_at_space;
    str_number ret;
    if ((pool_ptr + namelength > pool_size) ||
        (str_ptr == max_strings) || (cur_length > 0)) {
        ret = maketexstring("?");
    } else {
        for (k = 1; k <= namelength; k++)
            append_char(nameoffile[k]);
        ret = make_string();
    }
    /* At this point we also reset |cur_name|, |cur_ext|, and |cur_area| to
       match the contents of |nameoffile|. */
    save_area_delimiter = area_delimiter;
    save_ext_delimiter = ext_delimiter;
    save_name_in_progress = name_in_progress;
    save_stop_at_space = stop_at_space;
    name_in_progress = true;
    begin_name();
    stop_at_space = false;
    k = 1;
    while ((k <= namelength) && (more_name(nameoffile[k])))
        k++;
    stop_at_space = save_stop_at_space;
    end_name();
    name_in_progress = save_name_in_progress;
    area_delimiter = save_area_delimiter;
    ext_delimiter = save_ext_delimiter;
    return ret;
}



void print_file_name(str_number n, str_number a, str_number e)
{
    boolean must_quote;         /* whether to quote the filename */
    pool_pointer j;             /* index into |str_pool| */
    must_quote = false;
    if (a != 0) {
        j = str_start_macro(a);
        while ((!must_quote) && (j < str_start_macro(a + 1))) {
            must_quote = (str_pool[j] == ' ');
            incr(j);
        }
    }
    if (n != 0) {
        j = str_start_macro(n);
        while ((!must_quote) && (j < str_start_macro(n + 1))) {
            must_quote = (str_pool[j] == ' ');
            incr(j);
        }
    }
    if (e != 0) {
        j = str_start_macro(e);
        while ((!must_quote) && (j < str_start_macro(e + 1))) {
            must_quote = (str_pool[j] == ' ');
            incr(j);
        }
    }
    /* FIXME: Alternative is to assume that any filename that has to be quoted has
       at least one quoted component...if we pick this, a number of insertions
       of |print_file_name| should go away.
       |must_quote|:=((|a|<>0)and(|str_pool|[|str_start|[|a|]]=""""))or
       ((|n|<>0)and(|str_pool|[|str_start|[|n|]]=""""))or
       ((|e|<>0)and(|str_pool|[|str_start|[|e|]]="""")); */

    if (must_quote)
        print_char('"');
    if (a != 0) {
        for (j = str_start_macro(a); j <= str_start_macro(a + 1) - 1; j++)
            if (str_pool[j] != '"')
                print_char(str_pool[j]);
    }
    if (n != 0) {
        for (j = str_start_macro(n); j <= str_start_macro(n + 1) - 1; j++)
            if (str_pool[j] != '"')
                print_char(str_pool[j]);
    }
    if (e != 0) {
        for (j = str_start_macro(e); j <= str_start_macro(e + 1) - 1; j++)
            if (str_pool[j] != '"')
                print_char(str_pool[j]);
    }
    if (must_quote)
        print_char('"');
}
