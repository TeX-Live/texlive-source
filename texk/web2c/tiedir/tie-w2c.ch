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
/* boolean, false, true; string; all from cpascal.h */
@z

@x -- we need more input files.
#define max_file_index 9
/* we don't think that anyone needs more than 9 change files,
@y
#define max_file_index 32
/* we don't think that anyone needs more than 32 change files,
@z

@x -- add to global includes.
#include <stdio.h>
@y
#include "cpascal.h"
#include <stdio.h>
#include <kpathsea/kpathsea.h>
/* Also redefine usage to avoid clash with function from lib. */
#define usage tieusage
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

@x
{string msg;
@y
{const_string msg;
@z

@x -- silence unitialized warning
      case fatal: msg="That was a fatal error, my friend";  break;
@y
      default: /* Anything except spotless, troublesome, or fatal is a bug. */
      case fatal: msg="That was a fatal error, my friend";  break;
@z