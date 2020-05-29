% Changes to adapt tie to web2c.
% Copyright 2001 by Olaf Weber
% Updated 2020 by Andreas Scherer
% This file is in the Public Domain.

Limbo

@x l.44 -- reformat 'covernote' on table-of-contents page
\def\botofcontents{
\null\vfill
\item{$\copyright$}1989, 1992
@y
\def\covernote{\hsize 5in \vbox{\ninerm\noindent
\copyright~1989, 1992
@z

@x l.61
}
@y
}}
@z

Section 2

@x l.113 -- improve typography
@<Global |#include|s@>@;
@y
@<Global \&{\#include}s@>@;
@z

Section 3

@x l.123 -- Already in cpascal.h.
@d incr(v) v+=1 /* increase a variable by unity */
@d decr(v) v-=1 /* decrease a variable by unity */
@y
@z

Section 4

@x l.130
@ Furthermore we include the additional types |boolean| and |string|.
@d false 0
@d true 1
@<Global types@>=
typedef int boolean;
typedef char* string;
@y
@ The types |boolean| (with values |false| and |true|) and
|string| come from \.{<kpathsea/simpletypes.h>}.
@s boolean int
@s string int
@z

Section 5

@x l.144 -- we need more input files.
#define max_file_index 9
/* we don't think that anyone needs more than 9 change files,
@y
#define max_file_index 32
/* we don't think that anyone needs more than 32 change files,
@z

Section 6

l.155
@x -- replace preprocessor macros
@d spotless 0
@d troublesome 1
@d fatal 2

@<Global variables@>=
static int history=spotless;
@y
@<Global variables@>=
typedef enum {
    spotless,
    troublesome,
    fatal } return_code;
static return_code history=spotless;
@z

Section 15

@x l.461
@d print(a)  fprintf(term_out,a) /* `|print|' means write on the terminal */
@y
@d print(a)  fprintf(term_out,"%s",a) /* `|print|' means write on the terminal */
@z

@x l.463 -- function used only for error messages
@d print3(a,b,c)  fprintf(term_out,a,b,c) /* same with three arguments */
@y
@d print3(a,b,c)  fprintf(stderr,a,b,c) /* same with three arguments */
@z

@x l.468
@d print_ln(v)  {fprintf(term_out,v);term_new_line;}
@y
@d print_ln(v)  {fprintf(term_out,"%s",v);term_new_line;}
@z

@x l.471 -- used only for error reporting
@d print3_ln(a,b,c)  {print3(a,b,c);term_new_line;}
@y
@d print3_ln(a,b,c)  {print3(a,b,c);new_line(stderr);}
@z

@x l.478 -- add to global includes.
@<Global |#include|s@>=
#include <stdio.h>
@y
@<Global \&{\#include}s@>=
#include "cpascal.h" /* |decr| and |incr| */
#include <kpathsea/kpathsea.h>
#define usage tieusage /* Also redefine |usage| to avoid clash with function from lib. */
@z

Section 16

@x l.483 -- Remove redundant #include directives.
This should cause no trouble in any \Cl\ program.
@^system dependencies@>

@<Global |#include|s@>=
#ifdef __STDC__
#include <stdlib.h>
#else
#include <malloc.h>
#endif
@y
This should cause no trouble in any \Cl\ program.
The \.{kpathsea} include files handle the definition of |malloc()|,
too.
@^system dependencies@>
@z

Sections 18 and 19: use enum as requested in tie.w

@x l.530
#define search 0
#define test 1
#define reading 2
#define ignore 3
typedef int in_file_modes; /* should be |enum(search,test,reading,ignore)| */
#define unknown 0
#define master 1
#define chf 2
typedef int file_types; /* should be |enum(unknown,master,chf)| */
@y
typedef enum {
    search,
    test,
    reading,
    ignore } in_file_modes;
typedef enum {
    unknown,
    master,
    chf } file_types;
@z

@x l.548
#define normal 0
#define pre 1
#define post 2
typedef int out_md_type; /* should be |enum(normal,pre,post)| */
@y
typedef enum {
    normal,
    pre,
    post } out_md_type;
@z

Section 24

@x l.617
void get_line(i)
	file_index i;
@y
static void
get_line (file_index i)
@z

Section 27

@x l.650 -- fix typo
replacement part of a change file, or in an incomplerte check if the
@y
replacement part of a change file, or in an incomplete check if the
@z

@x l.667
   if (c!=@' ' && c!=tab_mark)
@y
   if (c!=@' ' && c!=tab_mark && c!=@'\r')
@z

Section 31

@x l.742 -- print errors on 'stderr'
@d err_print(m)  { @+ print_nl(m); error_loc
@y
@d err_print(m)  { @+ new_line(stderr); fprintf(stderr,"%s",m); error_loc
@z

@x l.745
void err_loc(i) /* prints location of error */
        int i;
@y
static void
err_loc (int i) /* prints location of error */
@z

Section 32

@x l.761 -- print errors on 'stderr'
         print(m); print_c('.'); history=fatal;
	 term_new_line; jump_out();
@y
         fprintf(stderr,"%s",m);
         fputc('.',stderr); history=fatal;
	 new_line(stderr); jump_out();
@z

Section 33

@x l.774
@d jump_out() exit(1)
@y
@d jump_out() exit(EXIT_FAILURE)
@z

Section 34

@x l.790 Use binary mode for output files
    out_file=fopen(out_name,"w");
@y
    out_file=fopen(out_name,"wb");
@z

Section 36

@x l.811
	  fopen(input_organization[0]->name_of_file,"r");
@y
	  kpse_open_file(input_organization[0]->name_of_file, kpse_web_format);
@z

Section 37

@x l.830
	fopen(input_organization[i]->name_of_file,"r");
@y
	kpse_open_file(input_organization[i]->name_of_file, kpse_web_format);
@z

Section 38

@x l.851
boolean lines_dont_match(i,j)
	file_index i,j;
@y
static boolean
lines_dont_match (file_index i, file_index j)
@z

Section 39

@x l.872
void init_change_file(i,b)
	file_index i; boolean b;
@y
static void
init_change_file (file_index i, boolean b)
@z

Section 42

@x l.919
void put_line(j)
	file_index j;
@y
static void
put_line (file_index j)
@z

Section 43

@x l.935
boolean e_of_ch_module(i)
	file_index i;
@y
static boolean
e_of_ch_module (file_index i)
@z

Section 44

@x l.955
boolean e_of_ch_preamble(i)
	file_index i;
@y
static boolean
e_of_ch_preamble (file_index i)
@z

Section 47

@x l.1005 -- fix typo
a line to write and |test_input| ist set to |none|.
@y
a line to write and |test_input| is set to |none|.
@z

Section 48: fix indentation of nested loop.

@x l.1044
if (prod_chf==chf) {
  loop @+ {
    @<Test for normal, |break| when done@>@;
    @<Test for pre, |break| when done@>@;
    @<Test for post, |break| when done@>@;
  }
} else
@y
if (prod_chf==chf)
  loop @+ {
    @<Test for normal, |break| when done@>@;
    @<Test for pre, |break| when done@>@;
    @<Test for post, |break| when done@>@;
  }
else
@z

Section 55

@x l.1158
void usage()
{
   print("Usage: tie -[mc] outfile master changefile(s)");
@y
static
void usage (void)
{
   print("Usage: tie -m|-c outfile master changefile(s)");
@z

Section 56

@x l.1169 -- fix typo
change files.  The names fo the file parameters will be inserted into
@y
change files.  The names of the file parameters will be inserted into
@z

Section 59

@x l.1236
main(argc,argv)
        int argc; string *argv;
@y
int main (int argc, string *argv)
@z

@x l.1241
  print_ln(banner); /* print a ``banner line'' */
  print_ln(copyright); /* include the copyright notice */
@y
  kpse_set_program_name(argv[0], "tie");
  print(banner); /* print a ``banner line'' */
  print_ln(versionstring);  /* Web2C version */
  print_ln(copyright); /* include the copyright notice */
@z

Section 60

@x l.1256 -- fix typo
Additionaly we report the history to the user, although this may not
@y
Additionally we report the history to the user, although this may not
@z

@x l.1261 -- rewrite error reporting
@<Print the job |history|@>=
{string msg;
   switch (history) {
      case spotless: msg="No errors were found"; break;
      case troublesome: msg="Pardon me, but I think I spotted something wrong.";
	        break;
      case fatal: msg="That was a fatal error, my friend";  break;
      } /* there are no other cases */
   print2_nl("(%s.)",msg);  term_new_line;
   exit ( history == spotless  ?  0 : 1 );
}
@y
@<Print the job |history|@>=
{
   switch (history) {
      case spotless: print2_nl("(%s.)", "No errors were found");
        term_new_line; break;
      case troublesome: new_line(stderr); fprintf(stderr,
        "(Pardon me, but I think I spotted something wrong.)");
        new_line(stderr); break;
      case fatal:
      default: /* Anything except spotless, troublesome, or fatal is a bug. */
        new_line(stderr);
        fprintf(stderr, "(That was a fatal error, my friend.)");
        new_line(stderr); break;
      }
   exit ( history == spotless ?  EXIT_SUCCESS : EXIT_FAILURE );
}
@z
