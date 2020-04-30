% Changes to adapt tie to web2c.
% Copyright 2001 by Olaf Weber
% This file is in the Public Domain.

@x Already in cpascal.h.
@d incr(v) v+=1 /* increase a variable by unity */
@d decr(v) v-=1 /* decrease a variable by unity */
@y
@z

@x
@ Furthermore we include the additional types |boolean| and |string|.
@d false 0
@d true 1
@<Global types@>=
typedef int boolean;
typedef char* string;
@y
@ Furthermore we include the additional types |boolean| and |string|.
/* |boolean|, |false|, |true|; |string|; all from \.{<kpathsea/simpletypes.h>} */
@s boolean int
@s string int
@z

@x -- we need more input files.
#define max_file_index 9
/* we don't think that anyone needs more than 9 change files,
@y
#define max_file_index 32
/* we don't think that anyone needs more than 32 change files,
@z

@x
@d print(a)  fprintf(term_out,a) /* `|print|' means write on the terminal */
@y
@d print(a)  fprintf(term_out,"%s",a) /* `|print|' means write on the terminal */
@z

@x
@d print_ln(v)  {fprintf(term_out,v);term_new_line;}
@y
@d print_ln(v)  {fprintf(term_out,"%s",v);term_new_line;}
@z

@x -- add to global includes.
#include <stdio.h>
@y
#include "cpascal.h" /* |decr| and |incr| */
#include <kpathsea/kpathsea.h>
#define usage tieusage /* Also redefine |usage| to avoid clash with function from lib. */
@z

Section 16: Remove redundant #include directives.

@x l.483
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

@x l.617
void get_line(i)
	file_index i;
@y
static void
get_line (file_index i)
@z

@x l.650
replacement part of a change file, or in an incomplerte check if the
@y
replacement part of a change file, or in an incomplete check if the
@z

@x l.667
   if (c!=@' ' && c!=tab_mark)
@y
   if (c!=@' ' && c!=tab_mark && c!=@'\r')
@z

@x l.745
void err_loc(i) /* prints location of error */
        int i;
@y
static void
err_loc (int i) /* prints location of error */
@z

@x l.790 Use binary mode for output files
    out_file=fopen(out_name,"w");
@y
    out_file=fopen(out_name,"wb");
@z

@x l.811
	  fopen(input_organization[0]->name_of_file,"r");
@y
	  kpse_open_file(input_organization[0]->name_of_file, kpse_web_format);
@z

@x l.830
	fopen(input_organization[i]->name_of_file,"r");
@y
	kpse_open_file(input_organization[i]->name_of_file, kpse_web_format);
@z

@x
boolean lines_dont_match(i,j)
	file_index i,j;
@y
static boolean
lines_dont_match (file_index i, file_index j)
@z

@x
void init_change_file(i,b)
	file_index i; boolean b;
@y
static void
init_change_file (file_index i, boolean b)
@z

@x
void put_line(j)
	file_index j;
@y
static void
put_line (file_index j)
@z

@x
boolean e_of_ch_module(i)
	file_index i;
@y
static boolean
e_of_ch_module (file_index i)
@z

@x
boolean e_of_ch_preamble(i)
	file_index i;
@y
static boolean
e_of_ch_preamble (file_index i)
@z

@x l.1005
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

@x
void usage()
{
   print("Usage: tie -[mc] outfile master changefile(s)");
@y
static
void usage (void)
{
   print("Usage: tie -m|-c outfile master changefile(s)");
@z

@x l.1169
change files.  The names fo the file parameters will be inserted into
@y
change files.  The names of the file parameters will be inserted into
@z

@x
main(argc,argv)
        int argc; string *argv;
@y
int main (int argc, string *argv)
@z

@x
  print_ln(banner); /* print a ``banner line'' */
  print_ln(copyright); /* include the copyright notice */
@y
  kpse_set_program_name(argv[0], "tie");
  print(banner); /* print a ``banner line'' */
  print_ln(versionstring);  /* Web2C version */
  print_ln(copyright); /* include the copyright notice */
@z

@x l.1256
Additionaly we report the history to the user, although this may not
@y
Additionally we report the history to the user, although this may not
@z

@x
@<Print the job |history|@>=
{string msg;
@y
@s const_string int

@<Print the job |history|@>=
{const_string msg;
@z

@x -- silence unitialized warning
      case fatal: msg="That was a fatal error, my friend";  break;
@y
      default: /* Anything except spotless, troublesome, or fatal is a bug. */
      case fatal: msg="That was a fatal error, my friend";  break;
@z
