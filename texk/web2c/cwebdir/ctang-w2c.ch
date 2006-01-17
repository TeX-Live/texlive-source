% Kpathsea changes for CWEB by Wlodek Bzyl and Olaf Weber
% Copyright 2002 Wlodek Bzyl and Olaf Weber
% This file is in the Public Domain.

@x l.27
\def\title{CTANGLE (Version 3.64)}
\def\topofcontents{\null\vfill
  \centerline{\titlefont The {\ttitlefont CTANGLE} processor}
  \vskip 15pt
  \centerline{(Version 3.64)}
  \vfill}
@y
\def\title{CTANGLE (Version 3.64k)}
\def\topofcontents{\null\vfill
  \centerline{\titlefont The {\ttitlefont CTANGLE} processor}
  \vskip 15pt
  \centerline{(Version 3.64k)}
  \vfill}
@z

This change can not be applied when `tie' is  used
(TOC file can not be typeset).

%@x l.48
%\let\maybe=\iftrue
%@y
%\let\maybe=\iffalse % print only sections that change
%@z


Section 1.

@x l.49
The ``banner line'' defined here should be changed whenever \.{CTANGLE}
is modified.

@d banner "This is CTANGLE (Version 3.64)\n"
@y
The ``banner line'' defined here should be changed whenever \.{CTANGLE}
is modified.

@d banner "This is CTANGLE, Version 3.64"
@z


Section 2.

@x l.72
@ We predeclare several standard system functions here instead of including
their system header files, because the names of the header files are not as
standard as the names of the functions. (For example, some \CEE/ environments
have \.{<string.h>} where others have \.{<strings.h>}.)

@<Predecl...@>=
extern int strlen(); /* length of string */
extern int strcmp(); /* compare strings lexicographically */
extern char* strcpy(); /* copy one string to another */
extern int strncmp(); /* compare up to $n$ string characters */
extern char* strncpy(); /* copy up to $n$ string characters */
@y
@z

Section 3.

@x l.92
int main (ac, av)
int ac;
char **av;
@y
int main P2C(int,ac, char**,av)
@z

@x l.100 - Add Web2C version to banner.
  if (show_banner) printf(banner); /* print a ``banner line'' */
@y
  if (show_banner) {
    printf("%s%s\n", banner, versionstring); /* print a ``banner line'' */
  }
@z

Section 5.

@x common.h l.30 - boolean comes from kpathsea.
typedef short boolean;
@y
@z

Section 6.

@x l.36
#include <stdio.h>
@y
#include <stdio.h>
/* #include "cpascal.h" ??? */
extern char *versionstring;
#include <kpathsea/kpathsea.h>
@z

Section 9.

@x common.h l.109
extern name_pointer id_lookup(); /* looks up a string in the identifier table */
extern name_pointer section_lookup(); /* finds section name */
extern void print_section_name(), sprint_section_name();
@y
/* looks up a string in the identifier table */
extern name_pointer id_lookup P3H(char*,char*,char);
/* finds section name */
extern name_pointer section_lookup P3H(char*,char*,char);
extern void print_section_name P1H(name_pointer);
extern void sprint_section_name P2H(char*,name_pointer);
@z

Section 10.

@x common.h l.123 - explicit types.
extern history; /* indicates how bad this run was */
extern err_print(); /* print error message and context */
extern wrap_up(); /* indicate |history| and exit */
extern void fatal(); /* issue error message and die */
extern void overflow(); /* succumb because a table has overflowed */
@y
extern int history; /* indicates how bad this run was */
extern void err_print P1H(char*); /* print error message and context */
extern int wrap_up P1H(void); /* indicate |history| and exit */
extern void fatal P2H(char*,char*); /* issue error message and die */
extern void overflow P1H(char*); /* succumb because a table has overflowed */
@z

Section 11.

@x common.h l.131 - max_file_name_length is way too small.
@d max_file_name_length 60
@y
@d max_file_name_length 1024
@z

@x common.h l.138 - explicit types.
extern include_depth; /* current level of nesting */
@y
extern int include_depth; /* current level of nesting */
@z

@x common.h l.148 - explicit types.
extern line[]; /* number of current line in the stacked files */
extern change_line; /* number of current line in change file */
@y
extern int line[]; /* number of current line in the stacked files */
extern int change_line; /* number of current line in change file */
@z

@x common.h l.153 - explicit types.
extern reset_input(); /* initialize to read the web file and change file */
extern get_line(); /* inputs the next line */
extern check_complete(); /* checks that all changes were picked up */
@y
extern void reset_input P1H(void); /* initialize to read the web file and change file */
extern int get_line P1H(void); /* inputs the next line */
extern void check_complete P1H(void); /* checks that all changes were picked up */
@z

Section 15.

@x common.h l.192
extern void common_init();
@y
extern void common_init P1H(void);
@z

Section 21.

@x l.184
int names_match(p,first,l)
name_pointer p; /* points to the proposed match */
char *first; /* position of first character of string */
int l; /* length of identifier */
@y
int names_match P4C(name_pointer,p, char*,first, int,l, char,t)
@z

Section 22.

@x l.200
init_node(node)
name_pointer node;
@y
init_node P1C(name_pointer,node)
@z

@x l.207
init_p() {}
@y
init_p P2C(name_pointer,p, char,t) {}
@z

Section 26.

@x l.262
store_two_bytes(x)
sixteen_bits x;
@y
store_two_bytes P1C(sixteen_bits,x)
@z

Section 30.

@x l.338
push_level(p) /* suspends the current level */
name_pointer p;
@y
push_level P1C(name_pointer,p) /* suspends the current level */
@z

Section 31.

@x l.357
pop_level(flag) /* do this when |cur_byte| reaches |cur_end| */
int flag; /* |flag==0| means we are in |output_defs| */
@y
pop_level P1C(int,flag) /* do this when |cur_byte| reaches |cur_end| */
@z

Section 33.

@x l.393
get_output() /* sends next token to |out_char| */
@y
get_output P1H(void) /* sends next token to |out_char| */
@z

Section 37.

@x l.483
flush_buffer() /* writes one line to output file */
@y
flush_buffer P1H(void) /* writes one line to output file */
@z

Section 41.

@x l.534
void phase_two();
@y
void phase_two P1H(void);
@z

Section 42.

@x l.538
phase_two () {
@y
phase_two P1H(void) {
@z

Section 46.

@x l.603
void output_defs();
@y
void output_defs P1H(void);
@z

Section 47.

@x l.607
output_defs()
@y
output_defs P1H(void)
@z

Section 48.

@x l.649
static void out_char();
@y
static void out_char P1H(eight_bits);
@z

Section 49.

@x l.653
out_char(cur_char)
eight_bits cur_char;
@y
out_char P1C(eight_bits,cur_char)
@z

Section 58.

@x l.815
skip_ahead() /* skip to next control code */
@y
skip_ahead P1H(void) /* skip to next control code */
@z

Section 60.

@x l.849
int skip_comment(is_long_comment) /* skips over comments */
boolean is_long_comment;
@y
int skip_comment P1C(boolean,is_long_comment) /* skips over comments */
@z

Section 62

@x l.889 - add declaration of versionstring.
#include <ctype.h> /* definition of |isalpha|, |isdigit| and so on */
@y
@z

Section 63.

@x l.902
get_next() /* produces the next input token */
@y
get_next P1H(void) /* produces the next input token */
@z

Section 76.

@x l.1201
scan_repl(t) /* creates a replacement text */
eight_bits t;
@y
scan_repl P1C(eight_bits,t) /* creates a replacement text */
@z

Section 83.

@x l.1359
scan_section()
@y
scan_section P1H(void)
@z

Section 90.

@x l.1458
void phase_one();
@y
void phase_one P1H(void);
@z

Section 91.

@x l.1462
phase_one() {
@y
phase_one P1H(void) {
@z

Section 92.

@x l.1476
void skip_limbo();
@y
void skip_limbo P1H(void);
@z

Section 93.

@x l.1480
skip_limbo()
@y
skip_limbo P1H(void)
@z

Section 95.

@x l.1535
print_stats() {
@y
print_stats P1H(void) {
@z
