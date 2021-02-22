% Changes to adapt tie to web2c.
% Created 2001 by Olaf Weber
% Updated 2020 by Andreas Scherer
% This file is in the Public Domain.

Limbo

@x l.42
  \centerline{(CWEB Version 2.4)}
@y
  \centerline{(CWEB Version 2.4 [\TeX~Live])}
@z

@x l.44 -- reformat 'covernote' on table-of-contents page
\def\botofcontents{
\null\vfill
\item{$\copyright$}1989, 1992
   by Technische Hochschule Darmstadt,\hfill\break
Fachbereich Informatik, Institut f\"ur Theoretische Informatik\hfill\break
All rights reserved.\hfill\break
This program is distributed WITHOUT ANY WARRANTY, express or implied.
\hfill\break
Permission is granted to make and distribute verbatim copies of this
program provided that the copyright notice and this permission notice
are preserved on all copies.
\hfill\break
Permission is granted to copy and distribute modified versions of this
program under the conditions for verbatim copying, provided that the
entire resulting derived work is distributed under the terms of a
permission notice identical to this one.
}
@y
\let\maybe=\iftrue
\def\covernote{\vbox{\noindent
Copyright \copyright~1989, 1992
   by Technische Hochschule Darmstadt,\hfill\break
Fachbereich Informatik, Institut f\"ur Theoretische Informatik
\smallskip\noindent
All rights reserved.
\bigskip\noindent
This program is distributed WITHOUT ANY WARRANTY, express or implied.
\smallskip\noindent
Permission is granted to make and distribute verbatim copies of this
program provided that the copyright notice and this permission notice
are preserved on all copies.
\smallskip\noindent
Permission is granted to copy and distribute modified versions of this
program under the conditions for verbatim copying, provided that the
entire resulting derived work is distributed under the terms of a
permission notice identical to this one.
}}
\datecontentspage
@z

Section 1.

@x l.96
E.g.\ it will not use the |enum| type declarations.
@y
@z

@x l.105
@d banner  "This is TIE, CWEB Version 2.4."
@y
@d banner  "This is TIE, CWEB Version 2.4."
  /* will be extended by the \TeX~Live |versionstring| */
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
@ The types |@!boolean| (with values |@!false| and |@!true|) and
|@!string| come from \.{<kpathsea/simpletypes.h>}.
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
    @!spotless,
    @!troublesome,
    @!fatal } return_code;
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
#include "cpascal.h" /* |@!decr| and |@!incr| */
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
The \.{kpathsea} include files handle the definition of |@!malloc|, too.
@^system dependencies@>
@z

Sections 18 and 19: use enum as requested in tie.w

@x l.522
\leavevmode |file_types| is used to describe whether a file
@y
\leavevmode \&{file\_types} is used to describe whether a file
@z

@x l.526 Fix case glitch.
the kind of output. (this would even be necessary if we
@y
the kind of output. (This would even be necessary if we
@z

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
    @!search,
    @!test,
    @!reading,
    @!ignore } in_file_modes;
typedef enum {
    @!unknown,
    @!master,
    @!chf } file_types;
@z

@x l.541
@ A variable of type |out_md_type| will tell us in what state the output
@y
@ A variable of type \&{out\_md\_type} will tell us in what state the output
@z

@x l.548
#define normal 0
#define pre 1
#define post 2
typedef int out_md_type; /* should be |enum(normal,pre,post)| */
@y
typedef enum {
    @!normal,
    @!pre,
    @!post } out_md_type;
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

@x l.1233
\noindent Here is where \.{TIE} starts, and where it ends.
@y
\noindent Here is where \.{TIE} starts, and where it ends.
\def\Kpathsea/{{\mc KPATHSEA\spacefactor1000}}

This version of the \.{TIE} program uses the \Kpathsea/ library for searching
files.  Firstly, we use the |kpse_web_format| when opening input files, which
triggers the inspection of the \.{WEBINPUTS} environment variable.  Secondly,
we set |kpse_program_name| to `\.{tie}'.  This means if the variable
\.{WEBINPUTS.tie} is present in \.{texmf.cnf} (or \.{WEBINPUTS\_tie} in the
environment) its value will be used as the search path for filenames.  This
allows different flavors of \.{TIE} (or other \.{WEB} programs) to have
different search paths.@.WEBINPUTS@> In all, the directories to be searched for
come from at least two sources:
\smallskip
{\parindent1.5em
\item{(a)} a user-set environment variable \.{WEBINPUTS}
    (overridden by \.{WEBINPUTS\_tie});
\item{(b)} a line in \Kpathsea/ configuration file \.{texmf.cnf},\hfil\break
    e.g., \.{WEBINPUTS=\$TEXMFDOTDIR:\$TEXMF/texmf/web//}\hfil\break
    or \.{WEBINPUTS.tie=\$TEXMFDOTDIR:\$TEXMF/texmf/web//}.\par}
\smallskip
Note that, although \.{WEBINPUTS} might suggest otherwise, \.{TIE} is more or
less language-agnostic and that it is perfectly capable of handling \.{CWEB}
files as input as well, as long as the ``change files'' adhere to the general
\.{@@x}, \.{@@y}, \.{@@z} convention.
@z

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
