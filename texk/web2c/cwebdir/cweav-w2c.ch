% Kpathsea changes for CWEB by Wlodek Bzyl and Olaf Weber
% Copyright 2002 Wlodek Bzyl and Olaf Weber
% This file is in the Public Domain.


@x l.32
\def\title{CWEAVE (Version 3.64)}
\def\topofcontents{\null\vfill
  \centerline{\titlefont The {\ttitlefont CWEAVE} processor}
  \vskip 15pt
  \centerline{(Version 3.64)}
  \vfill}
@y
\def\Kpathsea/{{\mc KPATHSEA\spacefactor1000}}
\def\SQUARE{\vrule width 2pt depth -1pt height 3pt}
\def\sqitem{\item{\SQUARE}}

\def\title{CWEAVE (Version 3.64k)}
\def\topofcontents{\null\vfill
  \centerline{\titlefont The {\ttitlefont CWEAVE} processor}
  \vskip 15pt
  \centerline{(Version 3.64k)}
  \vfill}
@z

This change can not be applied when `tie' is  used
(TOC file can not be typeset).

%@x l.51
%\let\maybe=\iftrue
%@y
%\let\maybe=\iffalse % print only changed modules
%@z


Section 1.

@x l.65
The ``banner line'' defined here should be changed whenever \.{CWEAVE}
is modified.

@d banner "This is CWEAVE (Version 3.64)\n"
@y 
The ``banner line'' defined here should be changed whenever \.{CWEAVE}
is modified.

@d banner "This is CWEAVE, Version 3.64"
@z


Section 2.

@x l.77
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

@x l.98
int main (ac, av)
int ac; /* argument count */
char **av; /* argument values */
@y
int main P2C(int,ac, char**,av)
@z

@x l.107 - Add Web2C version to banner.
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

@x l.276
new_xref(p)
name_pointer p;
@y
new_xref P1C(name_pointer,p)
@z

Section 22.

@x l.307
new_section_xref(p)
name_pointer p;
@y
new_section_xref P1C(name_pointer,p)
@z

Section 23.

@x l.327
set_file_flag(p)
name_pointer p;
@y
set_file_flag P1C(name_pointer,p)
@z

Section 27.

@x l.371
int names_match(p,first,l,t)
name_pointer p; /* points to the proposed match */
char *first; /* position of first character of string */
int l; /* length of identifier */
eight_bits t; /* desired ilk */
@y
int names_match P4C(name_pointer,p, char*,first, int,l, char,t)
@z

@x l.383
init_p(p,t)
name_pointer p;
eight_bits t;
@y
init_p P2C(name_pointer,p, char,t)
@z

@x l.391
init_node(p)
name_pointer p;
@y
init_node P1C(name_pointer,p)
@z

Section 34.

@x l.600
void   skip_limbo();
@y
void   skip_limbo P1H(void);
@z

Section 35.

@x l.604
skip_limbo() {
@y
skip_limbo P1H(void) {
@z

Section 36.

@x l.626
skip_TeX() /* skip past pure \TEX/ code */
@y
skip_TeX P1H(void) /* skip past pure \TEX/ code */
@z

Section 38.

@x l.682 - Add declaration for versionstring.
#include <ctype.h> /* definition of |isalpha|, |isdigit| and so on */
@y
@z

Section 39.

@x l.696
eight_bits get_next();
@y
eight_bits get_next P1H(void);
@z

Section 40.

@x l.700
get_next() /* produces the next input token */
@y
get_next P1H(void) /* produces the next input token */
@z

Section 55.

@x l.971
void skip_restricted();
@y
void skip_restricted P1H(void);
@z

Section 56.

@x l.975
skip_restricted()
@y
skip_restricted P1H(void)
@z

Section 59.

@x l.1024
void phase_one();
@y
void phase_one P1H(void);
@z

Section 60.

@x l.1028
phase_one() {
@y
phase_one P1H(void) {
@z

Section 62.

@x l.1076
void C_xref();
@y
void C_xref P1H(eight_bits);
@z

Section 63.

@x l.1080
C_xref( spec_ctrl ) /* makes cross-references for \CEE/ identifiers */
  eight_bits spec_ctrl;
@y
C_xref P1C(eight_bits,spec_ctrl) /* makes cross-references for \CEE/ identifiers */
@z

Section 64.

@x l.1104
void outer_xref();
@y
void outer_xref P1H(void);
@z

Section 65.

@x l.1108
outer_xref() /* extension of |C_xref| */
@y
outer_xref P1H(void) /* extension of |C_xref| */
@z

Section 74.

@x l.1266
void section_check();
@y
void section_check P1H(name_pointer);
@z

Section 75.

@x l.1270
section_check(p)
name_pointer p; /* print anomalies in subtree |p| */
@y
section_check P1C(name_pointer,p) /* print anomalies in subtree |p| */
@z

Section 78.

@x l.1323
flush_buffer(b,per_cent,carryover)
char *b;  /* outputs from |out_buf+1| to |b|,where |b<=out_ptr| */
boolean per_cent,carryover;
@y
flush_buffer P3C(char*,b, boolean,per_cent, boolean,carryover)
@z

Section 79.

@x l.1352
finish_line() /* do this at the end of a line */
@y
finish_line P1H(void) /* do this at the end of a line */
@z

Section 81.

@x l.1384
out_str(s) /* output characters from |s| to end of string */
char *s;
@y
out_str P1C(char*,s) /* output characters from |s| to end of string */
@z

Section 83.

@x l.1402
void break_out();
@y
void break_out P1H(void);
@z

Section 84.

@x l.1406
break_out() /* finds a way to break the output line */
@y
break_out P1H(void) /* finds a way to break the output line */
@z

Section 86.

@x l.1441
out_section(n)
sixteen_bits n;
@y
out_section P1C(sixteen_bits,n)
@z

Section 87.

@x l.1455
out_name(p,quote_xalpha)
name_pointer p;
boolean quote_xalpha;
@y
out_name P2C(name_pointer,p, boolean,quote_xalpha)
@z

Section 88.

@x l.1485
copy_limbo()
@y
copy_limbo P1H(void)
@z

Section 90.

@x l.1520
copy_TeX()
@y
copy_TeX P1H(void)
@z

Section 91.

@x l.1449
int copy_comment();
@y
int copy_comment P2H(boolean,int);
@z

Section 92.

@x l.1551
int copy_comment(is_long_comment,bal) /* copies \TEX/ code in comments */
boolean is_long_comment; /* is this a traditional \CEE/ comment? */
int bal; /* brace balance */
@y
int copy_comment P2C(boolean,is_long_comment, int,bal)
@z

Section 99.

@x l.1784
print_cat(c) /* symbolic printout of a category */
eight_bits c;
@y
print_cat P1C(eight_bits,c)
@z

Section 106.

@x l.2139
print_text(p) /* prints a token list for debugging; not used in |main| */
text_pointer p;
@y
print_text P1C(text_pointer,p)
@z

Section 109.

@x l.2264
app_str(s)
char *s;
@y
app_str P1C(char*,s)
@z

@x l.2271
big_app(a)
token a;
@y
big_app P1C(token,a)
@z

@x l.2288
big_app1(a)
scrap_pointer a;
@y
big_app1 P1C(scrap_pointer,a)
@z

Section 111.

@x l.2415
find_first_ident(p)
text_pointer p;
@y
find_first_ident P1C(text_pointer,p)
@z

Section 112.

@x l.2447
make_reserved(p) /* make the first identifier in |p->trans| like |int| */
scrap_pointer p;
@y
make_reserved P1C(scrap_pointer,p)
@z

Section 113.

@x l.2478
make_underlined(p)
/* underline the entry for the first identifier in |p->trans| */
scrap_pointer p;
@y
make_underlined P1C(scrap_pointer,p)
@z

Section 114.

@x l.2495
void  underline_xref();
@y
void  underline_xref P1H(name_pointer);
@z

Section 115.

@x l.2499
underline_xref(p)
name_pointer p;
@y
underline_xref P1C(name_pointer,p)
@z

Section 164.

@x l.3004
reduce(j,k,c,d,n)
scrap_pointer j;
eight_bits c;
short k, d, n;
@y
reduce P5C(scrap_pointer,j, short,k, eight_bits,c, short,d, short,n)
@z

Section 165.

@x l.3030
squash(j,k,c,d,n)
scrap_pointer j;
eight_bits c;
short k, d, n;
@y
squash P5C(scrap_pointer,j, short,k, eight_bits,c, short,d, short,n)
@z

Section 170.

@x l.3126
translate() /* converts a sequence of scraps */
@y
translate P1H(void) /* converts a sequence of scraps */
@z

Section 174.

@x l.3191
C_parse(spec_ctrl) /* creates scraps from \CEE/ tokens */
  eight_bits spec_ctrl;
@y
C_parse P1C(eight_bits,spec_ctrl) /* creates scraps from \CEE/ tokens */
@z

Section 181.

@x l.3421
void app_cur_id();
@y
void app_cur_id P1H(boolean);
@z

Section 182.

@x l.3425
app_cur_id(scrapping)
boolean scrapping; /* are we making this into a scrap? */
@y
app_cur_id P1C(boolean,scrapping)
@z

Section 183.

@x l.3451
C_translate()
@y
C_translate P1H(void)
@z

Section 184.

@x l.3480
outer_parse() /* makes scraps from \CEE/ tokens and comments */
@y
outer_parse P1H(void) /* makes scraps from \CEE/ tokens and comments */
@z

Section 189.

@x l.3586
push_level(p) /* suspends the current level */
text_pointer p;
@y
push_level P1C(text_pointer,p) /* suspends the current level */
@z

Section 190.

@x l.3606
pop_level()
@y
pop_level P1H(void)
@z

Section 192.

@x l.3628
get_output() /* returns the next token of output */
@y
get_output P1H(void) /* returns the next token of output */
@z

Section 193.

@x l.3665
output_C() /* outputs the current token list */
@y
output_C P1H(void) /* outputs the current token list */
@z

Section 194.

@x l.3687
void make_output();
@y
void make_output P1H(void);
@z

Section 195.

@x l.3691
make_output() /* outputs the equivalents of tokens */
@y
make_output P1H(void) /* outputs the equivalents of tokens */
@z

Section 205.

@x l.3951
void phase_two();
@y
void phase_two P1H(void);
@z

Section 206.

@x l.3955
phase_two() {
@y
phase_two P1H(void) {
@z

Section 212.

@x l.4074
void finish_C();
@y
void finish_C P1H(boolean);
@z

Section 213.

@x l.4078
finish_C(visible) /* finishes a definition or a \CEE/ part */
  boolean visible; /* nonzero if we should produce \TEX/ output */
@y
finish_C P1C(boolean,visible) /* finishes a definition or a \CEE/ part */
@z

Section 221.

@x l.4245
void footnote();
@y
void footnote P1H(sixteen_bits);
@z

Section 222.

@x l.4249
footnote(flag) /* outputs section cross-references */
sixteen_bits flag;
@y
footnote P1C(sixteen_bits,flag) /* outputs section cross-references */
@z

Section 225.

@x l.4294
void phase_three();
@y
void phase_three P1H(void);
@z

Section 226.

@x l.4298
phase_three() {
@y
phase_three P1H(void) {
@z

Section 237.

@x l.4474
void  unbucket();
@y
void  unbucket P1H(eight_bits);
@z

Section 238.

@x l.4478
unbucket(d) /* empties buckets having depth |d| */
eight_bits d;
@y
unbucket P1C(eight_bits,d) /* empties buckets having depth |d| */
@z

Section 246.

@x l.4594
void section_print();
@y
void section_print P1H(name_pointer);
@z

Section 247.

@x l.4598
section_print(p) /* print all section names in subtree |p| */
name_pointer p;
@y
section_print P1C(name_pointer,p) /* print all section names in subtree |p| */
@z

Section 249.

@x l.4620
print_stats() {
@y
print_stats P1H(void) {
@z
