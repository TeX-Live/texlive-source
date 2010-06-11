% $Id: mp.w 1242 2010-05-31 08:18:21Z taco $
%
% Copyright 2008-2009 Taco Hoekwater.
%
% This program is free software: you can redistribute it and/or modify
% it under the terms of the GNU Lesser General Public License as published by
% the Free Software Foundation, either version 3 of the License, or
% (at your option) any later version.
%
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU Lesser General Public License for more details.
%
% You should have received a copy of the GNU Lesser General Public License
% along with this program.  If not, see <http://www.gnu.org/licenses/>.
%
% TeX is a trademark of the American Mathematical Society.
% METAFONT is a trademark of Addison-Wesley Publishing Company.
% PostScript is a trademark of Adobe Systems Incorporated.

% Here is TeX material that gets inserted after \input webmac
\def\hang{\hangindent 3em\noindent\ignorespaces}
\def\textindent#1{\hangindent2.5em\noindent\hbox to2.5em{\hss#1 }\ignorespaces}
\def\ps{PostScript}
\def\psqrt#1{\sqrt{\mathstrut#1}}
\def\k{_{k+1}}
\def\pct!{{\char`\%}} % percent sign in ordinary text
\font\tenlogo=logo10 % font used for the METAFONT logo
\font\logos=logosl10
\def\MF{{\tenlogo META}\-{\tenlogo FONT}}
\def\MP{{\tenlogo META}\-{\tenlogo POST}}
\def\[#1]{\ignorespaces} % left over from pascal web
\def\<#1>{$\langle#1\rangle$}
\def\section{\mathhexbox278}
\let\swap=\leftrightarrow
\def\round{\mathop{\rm round}\nolimits}
\mathchardef\vbv="026A % synonym for `\|'
\def\vb{\relax\ifmmode\vbv\else$\vbv$\fi}

\def\(#1){} % this is used to make section names sort themselves better
\def\9#1{} % this is used for sort keys in the index via @@:sort key}{entry@@>
\def\title{MetaPost}
\pdfoutput=1
\pageno=3

@* \[1] Introduction.

This is \MP\ by John Hobby, a graphics-language processor based on D. E. Knuth's \MF.

Much of the original Pascal version of this program was copied with
permission from MF.web Version 1.9. It interprets a language very
similar to D.E. Knuth's METAFONT, but with changes designed to make it
more suitable for PostScript output.

The main purpose of the following program is to explain the algorithms of \MP\
as clearly as possible. However, the program has been written so that it
can be tuned to run efficiently in a wide variety of operating environments
by making comparatively few changes. Such flexibility is possible because
the documentation that follows is written in the \.{WEB} language, which is
at a higher level than C.

A large piece of software like \MP\ has inherent complexity that cannot
be reduced below a certain level of difficulty, although each individual
part is fairly simple by itself. The \.{WEB} language is intended to make
the algorithms as readable as possible, by reflecting the way the
individual program pieces fit together and by providing the
cross-references that connect different parts. Detailed comments about
what is going on, and about why things were done in certain ways, have
been liberally sprinkled throughout the program.  These comments explain
features of the implementation, but they rarely attempt to explain the
\MP\ language itself, since the reader is supposed to be familiar with
{\sl The {\logos METAFONT\/}book} as well as the manual
@.WEB@>
@:METAFONTbook}{\sl The {\logos METAFONT\/}book@>
{\sl A User's Manual for MetaPost}, Computing Science Technical Report 162,
AT\AM T Bell Laboratories.

@ The present implementation is a preliminary version, but the possibilities
for new features are limited by the desire to remain as nearly compatible
with \MF\ as possible.

On the other hand, the \.{WEB} description can be extended without changing
the core of the program, and it has been designed so that such
extensions are not extremely difficult to make.
The |banner| string defined here should be changed whenever \MP\
undergoes any modifications, so that it will be clear which version of
\MP\ might be the guilty party when a problem arises.
@^extensions to \MP@>
@^system dependencies@>

@d default_banner "This is MetaPost, Version 1.211" /* printed when \MP\ starts */
@d true 1
@d false 0

@(mpmp.h@>=
#define metapost_version "1.211"
#define metapost_magic (('M'*256) + 'P')*65536 + 1211
#define metapost_old_magic (('M'*256) + 'P')*65536 + 1080

@ The external library header for \MP\ is |mplib.h|. It contains a
few typedefs and the header defintions for the externally used
fuctions.

The most important of the typedefs is the definition of the structure 
|MP_options|, that acts as a small, configurable front-end to the fairly 
large |MP_instance| structure.
 
@(mplib.h@>=
typedef struct MP_instance * MP;
@<Exported types@>
typedef struct MP_options {
  @<Option variables@>
} MP_options;
@<Exported function headers@>

@ The internal header file is much longer: it not only lists the complete
|MP_instance|, but also a lot of functions that have to be available to
the \ps\ backend, that is defined in a separate \.{WEB} file. 

The variables from |MP_options| are included inside the |MP_instance| 
wholesale.

@(mpmp.h@>=
#include <setjmp.h>
typedef struct psout_data_struct * psout_data;
typedef struct svgout_data_struct * svgout_data;
#ifndef HAVE_BOOLEAN
typedef int boolean;
#endif
#ifndef INTEGER_TYPE
typedef int integer;
#endif
@<Declare helpers@>
@<Types in the outer block@>
@<Constants in the outer block@>
typedef struct MP_instance {
  @<Option variables@>
  @<Global variables@>
} MP_instance;
@<Internal library declarations@>

@ @c 
#include <w2c/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h> /* for access() */
#endif
#include <time.h> /* for struct tm \& co */
#include "mplib.h"
#include "mplibps.h" /* external header */
#include "mplibsvg.h" /* external header */
#include "mpmp.h" /* internal header */
#include "mppsout.h" /* internal header */
#include "mpsvgout.h" /* internal header */
#include "mptfmin.h" /* internal header */
#include "mpmemio.h" /* internal header */
@h
@<Declarations@>
@<Basic printing procedures@>
@<Error handling procedures@>

@ Here are the functions that set up the \MP\ instance.

@<Declarations@> =
MP_options *mp_options (void);
MP mp_initialize (MP_options *opt);

@ @c
MP_options *mp_options (void) {
  MP_options *opt;
  size_t l = sizeof(MP_options);
  opt = malloc(l);
  if (opt!=NULL) {
    memset (opt,0,l);
    opt->ini_version = true;
  }
  return opt;
} 

@ @<Internal library declarations@>=
@<Declare subroutines for parsing file names@>

@ The whole instance structure is initialized with zeroes,
this greatly reduces the number of statements needed in 
the |Allocate or initialize variables| block.

@d set_callback_option(A) do { mp->A = mp_##A;
  if (opt->A!=NULL) mp->A = opt->A;
} while (0)

@c
static MP mp_do_new (jmp_buf *buf) {
  MP mp = malloc(sizeof(MP_instance));
  if (mp==NULL) {
    xfree(buf);
	return NULL;
  }
  memset(mp,0,sizeof(MP_instance));
  mp->jump_buf = buf;
  return mp;
}

@ @c
static void mp_free (MP mp) {
  int k; /* loop variable */
  @<Dealloc variables@>
  if (mp->noninteractive) {
    @<Finish non-interactive use@>;
  }
  xfree(mp->jump_buf);
  xfree(mp);
}

@ @c
static void mp_do_initialize ( MP mp) {
  @<Local variables for initialization@>
  @<Set initial values of key variables@>
}

@ This procedure gets things started properly.
@c
MP mp_initialize (MP_options *opt) { 
  MP mp;
  jmp_buf *buf = malloc(sizeof(jmp_buf));
  if (buf == NULL || setjmp(*buf) != 0) 
    return NULL;
  mp = mp_do_new(buf);
  if (mp == NULL)
    return NULL;
  mp->userdata=opt->userdata;
  @<Set |ini_version|@>;
  mp->noninteractive=opt->noninteractive;
  set_callback_option(find_file);
  set_callback_option(open_file);
  set_callback_option(read_ascii_file);
  set_callback_option(read_binary_file);
  set_callback_option(close_file);
  set_callback_option(eof_file);
  set_callback_option(flush_file);
  set_callback_option(write_ascii_file);
  set_callback_option(write_binary_file);
  set_callback_option(shipout_backend);
  if (opt->banner && *(opt->banner)) {
    mp->banner = xstrdup(opt->banner);
  } else {
    mp->banner = xstrdup(default_banner);
  }
  if (opt->command_line && *(opt->command_line))
    mp->command_line = xstrdup(opt->command_line);
  if (mp->noninteractive) {
    @<Prepare function pointers for non-interactive use@>;
  } 
  /* open the terminal for output */
  t_open_out; 
  @<Find constant sizes@>;
  @<Allocate or initialize variables@>
  mp_reallocate_memory(mp,mp->mem_max);
  mp_reallocate_paths(mp,1000);
  mp_reallocate_fonts(mp,8);
  mp->history=mp_fatal_error_stop; /* in case we quit during initialization */
  @<Check the ``constant'' values...@>;
  if ( mp->bad>0 ) {
	char ss[256];
    mp_snprintf(ss,256,"Ouch---my internal constants have been clobbered!\n"
                   "---case %i",(int)mp->bad);
    do_fprintf(mp->err_out,(char *)ss);
@.Ouch...clobbered@>
    return mp;
  }
  mp_do_initialize(mp); /* erase preloaded mem */
  if (mp->ini_version) {
    @<Run inimpost commands@>;
  }
  if (!mp->noninteractive) {
    @<Initialize the output routines@>;
    @<Get the first line of input and prepare to start@>;
    @<Initializations after first line is read@>;
    @<Fix up |mp->internal[mp_job_name]|@>;
  } else {
    mp->history=mp_spotless;
  }
  return mp;
}

@ @<Initializations after first line is read@>=
mp_open_log_file(mp);
mp_set_job_id(mp);
mp_init_map_file(mp, mp->troff_mode);
mp->history=mp_spotless; /* ready to go! */
if (mp->troff_mode) {
  mp->internal[mp_gtroffmode]=unity; 
  mp->internal[mp_prologues]=unity; 
}
if ( mp->start_sym>0 ) { /* insert the `\&{everyjob}' symbol */
  mp->cur_sym=mp->start_sym; mp_back_input(mp);
}

@ @<Exported function headers@>=
extern MP_options *mp_options (void);
extern MP mp_initialize (MP_options *opt) ;
extern int mp_status(MP mp);
extern void *mp_userdata(MP mp);

@ @c
int mp_status(MP mp) { return mp->history; }

@ @c
void *mp_userdata(MP mp) { return mp->userdata; }

@ The overall \MP\ program begins with the heading just shown, after which
comes a bunch of procedure declarations and function declarations.
Finally we will get to the main program, which begins with the
comment `|start_here|'. If you want to skip down to the
main program now, you can look up `|start_here|' in the index.
But the author suggests that the best way to understand this program
is to follow pretty much the order of \MP's components as they appear in the
\.{WEB} description you are now reading, since the present ordering is
intended to combine the advantages of the ``bottom up'' and ``top down''
approaches to the problem of understanding a somewhat complicated system.

@ Some of the code below is intended to be used only when diagnosing the
strange behavior that sometimes occurs when \MP\ is being installed or
when system wizards are fooling around with \MP\ without quite knowing
what they are doing. Such code will not normally be compiled; it is
delimited by the preprocessor test `|#ifdef DEBUG .. #endif|'.

@ This program has two important variations: (1) There is a long and slow
version called \.{INIMP}, which does the extra calculations needed to
@.INIMP@>
initialize \MP's internal tables; and (2)~there is a shorter and faster
production version, which cuts the initialization to a bare minimum.

Which is which is decided at runtime.

@ The following parameters can be changed at compile time to extend or
reduce \MP's capacity. They may have different values in \.{INIMP} and
in production versions of \MP.
@.INIMP@>
@^system dependencies@>

@<Constants...@>=
#define file_name_size 255 /* file names shouldn't be longer than this */
#define bistack_size 1500 /* size of stack for bisection algorithms;
  should probably be left at this value */

@ Like the preceding parameters, the following quantities can be changed
to extend or reduce \MP's capacity. But if they are changed,
it is necessary to rerun the initialization program \.{INIMP}
@.INIMP@>
to generate new tables for the production \MP\ program.
One can't simply make helter-skelter changes to the following constants,
since certain rather complex initialization
numbers are computed from them. 

@ @<Glob...@>=
int max_strings; /* maximum number of strings; must not exceed |max_halfword| */
int pool_size; /* maximum number of characters in strings, including all
  error messages and help texts, and the names of all identifiers */
int old_pool_size; /* a helper used by |mp_cat| */
int mem_max; /* greatest index in \MP's internal |mem| array;
  must be strictly less than |max_halfword|;
  must be equal to |mem_top| in \.{INIMP}, otherwise |>=mem_top| */
int mem_top; /* largest index in the |mem| array dumped by \.{INIMP};
  must not be greater than |mem_max| */
int hash_prime; /* a prime number equal to about 85\pct! of |hash_size| */

@ @<Option variables@>=
int error_line; /* width of context lines on terminal error messages */
int half_error_line; /* width of first lines of contexts in terminal
  error messages; should be between 30 and |error_line-15| */
int halt_on_error; /* do we quit at the first error? */
int max_print_line; /* width of longest text lines output; should be at least 60 */
unsigned hash_size; /* maximum number of symbolic tokens,
  must be less than |max_halfword-3*param_size| */
int param_size; /* maximum number of simultaneous macro parameters */
int max_in_open; /* maximum number of input files and error insertions that
  can be going on simultaneously */
int main_memory; /* only for options, to set up |mem_max| and |mem_top| */
void *userdata; /* this allows the calling application to setup local */
char *banner; /* the banner that is printed to the screen and log */

@ @<Dealloc variables@>=
xfree(mp->banner);

@ 
@d set_value(a,b,c) do { a=c; if (b>c) a=b; } while (0)

@<Allocate or ...@>=
mp->max_strings=500;
mp->pool_size=10000;
mp->old_pool_size=10000;
set_value(mp->error_line,opt->error_line,79);
set_value(mp->half_error_line,opt->half_error_line,50);
if (mp->half_error_line>mp->error_line-15 ) 
  mp->half_error_line = mp->error_line-15;
mp->max_print_line=100;
set_value(mp->max_print_line,opt->max_print_line,79);
mp->halt_on_error = (opt->halt_on_error ? true : false);

@ In case somebody has inadvertently made bad settings of the ``constants,''
\MP\ checks them using a global variable called |bad|.

This is the second of many sections of \MP\ where global variables are
defined.

@<Glob...@>=
integer bad; /* is some ``constant'' wrong? */

@ Later on we will say `\ignorespaces|if (mem_max>=max_halfword) bad=10;|',
or something similar. (We can't do that until |max_halfword| has been defined.)

In case you are wondering about the non-consequtive values of |bad|: some
of the things that used to be WEB constants are now runtime variables
with checking at assignment time.

@<Check the ``constant'' values for consistency@>=
mp->bad=0;
if ( mp->mem_top<=1100 ) mp->bad=4;

@ Some |goto| labels are used by the following definitions. The label
`|restart|' is occasionally used at the very beginning of a procedure; and
the label `|reswitch|' is occasionally used just prior to a |case|
statement in which some cases change the conditions and we wish to branch
to the newly applicable case.  Loops that are set up with the |loop|
construction defined below are commonly exited by going to `|done|' or to
`|found|' or to `|not_found|', and they are sometimes repeated by going to
`|continue|'.  If two or more parts of a subroutine start differently but
end up the same, the shared code may be gathered together at
`|common_ending|'.

@ Here are some macros for common programming idioms.

@d incr(A)   (A)=(A)+1 /* increase a variable by unity */
@d decr(A)   (A)=(A)-1 /* decrease a variable by unity */
@d negate(A) (A)=-(A) /* change the sign of a variable */
@d double(A) (A)=(A)+(A)
@d odd(A)   ((A)%2==1)
@d do_nothing   /* empty statement */

@* \[2] The character set.
In order to make \MP\ readily portable to a wide variety of
computers, all of its input text is converted to an internal eight-bit
code that includes standard ASCII, the ``American Standard Code for
Information Interchange.''  This conversion is done immediately when each
character is read in. Conversely, characters are converted from ASCII to
the user's external representation just before they are output to a
text file.
@^ASCII code@>

Such an internal code is relevant to users of \MP\ only with respect to
the \&{char} and \&{ASCII} operations, and the comparison of strings.

@ Characters of text that have been converted to \MP's internal form
are said to be of type |ASCII_code|, which is a subrange of the integers.

@<Types...@>=
typedef unsigned char ASCII_code; /* eight-bit numbers */

@ The present specification of \MP\ has been written under the assumption
that the character set contains at least the letters and symbols associated
with ASCII codes 040 through 0176; all of these characters are now
available on most computer terminals.

@<Types...@>=
typedef unsigned char text_char; /* the data type of characters in text files */

@ @<Local variables for init...@>=
integer i;

@ The \MP\ processor converts between ASCII code and
the user's external character set by means of arrays |xord| and |xchr|
that are analogous to Pascal's |ord| and |chr| functions.

@(mpmp.h@>=
#define xchr(A) mp->xchr[(A)]
#define xord(A) mp->xord[(A)]

@ @<Glob...@>=
ASCII_code xord[256];  /* specifies conversion of input characters */
text_char xchr[256];  /* specifies conversion of output characters */

@ The core system assumes all 8-bit is acceptable.  If it is not,
a change file has to alter the below section.
@^system dependencies@>

Additionally, people with extended character sets can
assign codes arbitrarily, giving an |xchr| equivalent to whatever
characters the users of \MP\ are allowed to have in their input files.
Appropriate changes to \MP's |char_class| table should then be made.
(Unlike \TeX, each installation of \MP\ has a fixed assignment of category
codes, called the |char_class|.) Such changes make portability of programs
more difficult, so they should be introduced cautiously if at all.
@^character set dependencies@>
@^system dependencies@>

@<Set initial ...@>=
for (i=0;i<=0377;i++) { xchr(i)=(text_char)i; }

@ The following system-independent code makes the |xord| array contain a
suitable inverse to the information in |xchr|. Note that if |xchr[i]=xchr[j]|
where |i<j<0177|, the value of |xord[xchr[i]]| will turn out to be
|j| or more; hence, standard ASCII code numbers will be used instead of
codes below 040 in case there is a coincidence.

@<Set initial ...@>=
for (i=0;i<=255;i++) { 
   xord(xchr(i))=0177;
}
for (i=0200;i<=0377;i++) { xord(xchr(i))=(ASCII_code)i;}
for (i=0;i<=0176;i++) { xord(xchr(i))=(ASCII_code)i;}

@* \[3] Input and output.
The bane of portability is the fact that different operating systems treat
input and output quite differently, perhaps because computer scientists
have not given sufficient attention to this problem. People have felt somehow
that input and output are not part of ``real'' programming. Well, it is true
that some kinds of programming are more fun than others. With existing
input/output conventions being so diverse and so messy, the only sources of
joy in such parts of the code are the rare occasions when one can find a
way to make the program a little less bad than it might have been. We have
two choices, either to attack I/O now and get it over with, or to postpone
I/O until near the end. Neither prospect is very attractive, so let's
get it over with.

The basic operations we need to do are (1)~inputting and outputting of
text, to or from a file or the user's terminal; (2)~inputting and
outputting of eight-bit bytes, to or from a file; (3)~instructing the
operating system to initiate (``open'') or to terminate (``close'') input or
output from a specified file; (4)~testing whether the end of an input
file has been reached; (5)~display of bits on the user's screen.
The bit-display operation will be discussed in a later section; we shall
deal here only with more traditional kinds of I/O.

@ Finding files happens in a slightly roundabout fashion: the \MP\
instance object contains a field that holds a function pointer that finds a
file, and returns its name, or NULL. For this, it receives three
parameters: the non-qualified name |fname|, the intended |fopen|
operation type |fmode|, and the type of the file |ftype|.

The file types that are passed on in |ftype| can be  used to 
differentiate file searches if a library like kpathsea is used,
the fopen mode is passed along for the same reason.

@<Types...@>=
typedef unsigned char eight_bits ; /* unsigned one-byte quantity */

@ @<Exported types@>=
enum mp_filetype {
  mp_filetype_terminal = 0, /* the terminal */
  mp_filetype_error, /* the terminal */
  mp_filetype_program , /* \MP\ language input */
  mp_filetype_log,  /* the log file */
  mp_filetype_postscript, /* the postscript output */
  mp_filetype_memfile, /* memory dumps */
  mp_filetype_metrics, /* TeX font metric files */
  mp_filetype_fontmap, /* PostScript font mapping files */
  mp_filetype_font, /*  PostScript type1 font programs */
  mp_filetype_encoding, /*  PostScript font encoding files */
  mp_filetype_text  /* first text file for readfrom and writeto primitives */
};
typedef char *(*mp_file_finder)(MP, const char *, const char *, int);
typedef void *(*mp_file_opener)(MP, const char *, const char *, int);
typedef char *(*mp_file_reader)(MP, void *, size_t *);
typedef void (*mp_binfile_reader)(MP, void *, void **, size_t *);
typedef void (*mp_file_closer)(MP, void *);
typedef int (*mp_file_eoftest)(MP, void *);
typedef void (*mp_file_flush)(MP, void *);
typedef void (*mp_file_writer)(MP, void *, const char *);
typedef void (*mp_binfile_writer)(MP, void *, void *, size_t);

@ @<Option variables@>=
mp_file_finder find_file;
mp_file_opener open_file;
mp_file_reader read_ascii_file;
mp_binfile_reader read_binary_file;
mp_file_closer close_file;
mp_file_eoftest eof_file;
mp_file_flush flush_file;
mp_file_writer write_ascii_file;
mp_binfile_writer write_binary_file;

@ The default function for finding files is |mp_find_file|. It is 
pretty stupid: it will only find files in the current directory.

This function may disappear altogether, it is currently only
used for the default font map file.

@c
static char *mp_find_file (MP mp, const char *fname, const char *fmode, int ftype)  {
  (void) mp;
  if (fmode[0] != 'r' || (! access (fname,R_OK)) || ftype) {  
     return mp_strdup(fname);
  }
  return NULL;
}

@ Because |mp_find_file| is used so early, it has to be in the helpers
section.

@<Declarations@>=
static char *mp_find_file (MP mp, const char *fname, const char *fmode, int ftype) ;
static void *mp_open_file (MP mp , const char *fname, const char *fmode, int ftype) ;
static char *mp_read_ascii_file (MP mp, void *f, size_t *size) ;
static void mp_read_binary_file (MP mp, void *f, void **d, size_t *size) ;
static void mp_close_file (MP mp, void *f) ;
static int mp_eof_file (MP mp, void *f) ;
static void mp_flush_file (MP mp, void *f) ;
static void mp_write_ascii_file (MP mp, void *f, const char *s) ;
static void mp_write_binary_file (MP mp, void *f, void *s, size_t t) ;

@ The function to open files can now be very short.

@c
void *mp_open_file(MP mp, const char *fname, const char *fmode, int ftype)  {
  char realmode[3];
  (void) mp;
  realmode[0] = *fmode;
  realmode[1] = 'b';
  realmode[2] = 0;
  if (ftype==mp_filetype_terminal) {
    return (fmode[0] == 'r' ? stdin : stdout);
  } else if (ftype==mp_filetype_error) {
    return stderr;
  } else if (fname != NULL && (fmode[0] != 'r' || (! access (fname,R_OK)))) {
    return (void *)fopen(fname, realmode);
  }
  return NULL;
}

@ This is a legacy interface: (almost) all file names pass through |name_of_file|.

@<Glob...@>=
char name_of_file[file_name_size+1]; /* the name of a system file */
int name_length;/* this many characters are actually
  relevant in |name_of_file| (the rest are blank) */

@ If this parameter is true, the terminal and log will report the found
file names for input files instead of the requested ones. 
It is off by default because it creates an extra filename lookup.

@<Option variables@>=
int print_found_names; /* configuration parameter */

@ @<Allocate or initialize ...@>=
mp->print_found_names = (opt->print_found_names>0 ? true : false);

@ The |file_line_error_style| parameter makes \MP\ use a more
standard compiler error message format instead of the Knuthian 
exclamation mark. It needs the actual version of the current input 
file name, that will be saved by |a_open_in| in the |long_name|.

TODO: currently these strings cause memory leaks, because they cannot
be safely freed as they may appear in the |input_stack| multiple times.
In fact, the current implementation is just a quick hack in response 
to a bug report for metapost 1.205.

@d long_name mp->cur_input.long_name_field /* long name of the current file */

@<Option variables@>=
int file_line_error_style; /* configuration parameter */

@ @<Allocate or initialize ...@>=
mp->file_line_error_style = (opt->file_line_error_style>0 ? true : false);

@ \MP's file-opening procedures return |false| if no file identified by
|name_of_file| could be opened.

The |OPEN_FILE| macro takes care of the |print_found_names| parameter.
It is not used for opening a mem file for read, because that file name 
is never printed.

@d OPEN_FILE(A) do {
  if (mp->print_found_names || mp->file_line_error_style) {
    char *s = (mp->find_file)(mp,mp->name_of_file,A,ftype);
    if (s!=NULL) {
      *f = (mp->open_file)(mp,mp->name_of_file,A, ftype); 
      if (mp->print_found_names) {
        strncpy(mp->name_of_file,s,file_name_size);
      }
      if ((*(A) == 'r') && (ftype == mp_filetype_program)) {
        long_name = xstrdup(s);
      }
      xfree(s);
    } else {
      *f = NULL;
    }
  } else {
    *f = (mp->open_file)(mp,mp->name_of_file,A, ftype); 
  }
} while (0);
return (*f ? true : false)

@c 
static boolean mp_a_open_in (MP mp, void **f, int ftype) {
  /* open a text file for input */
  OPEN_FILE("r");
}
@#
#if 0
boolean mp_w_open_in (MP mp, void **f) {
  /* open a word file for input */
  *f = (mp->open_file)(mp,mp->name_of_file,"r",mp_filetype_memfile); 
  return (*f ? true : false);
}
#endif
@#
static boolean mp_a_open_out (MP mp, void **f, int ftype) {
  /* open a text file for output */
  OPEN_FILE("w");
}
@#
static boolean mp_b_open_out (MP mp, void **f, int ftype) {
  /* open a binary file for output */
  OPEN_FILE("w");
}
@#
boolean mp_w_open_out (MP mp, void **f) {
  /* open a word file for output */
  int ftype = mp_filetype_memfile;
  OPEN_FILE("w");
}

@ @<Internal library ...@>=
boolean mp_w_open_out (MP mp, void **f);

@ @c
static char *mp_read_ascii_file (MP mp, void *ff, size_t *size) {
  int c;
  size_t len = 0, lim = 128;
  char *s = NULL;
  FILE *f = (FILE *)ff;
  *size = 0;
  (void) mp; /* for -Wunused */
  if (f==NULL)
    return NULL;
  c = fgetc(f);
  if (c==EOF)
    return NULL;
  s = malloc(lim); 
  if (s==NULL) return NULL;
  while (c!=EOF && c!='\n' && c!='\r') { 
    if ((len+1)==lim) {
      s =realloc(s, (lim+(lim>>2)));
      if (s==NULL) return NULL;
      lim+=(lim>>2);
    }
	s[len++] = (char)c;
    c =fgetc(f);
  }
  if (c=='\r') {
    c = fgetc(f);
    if (c!=EOF && c!='\n')
       ungetc(c,f);
  }
  s[len] = 0;
  *size = len;
  return s;
}

@ @c
void mp_write_ascii_file (MP mp, void *f, const char *s) {
  (void) mp;
  if (f!=NULL) {
    fputs(s,(FILE *)f);
  }
}

@ @c
void mp_read_binary_file (MP mp, void *f, void **data, size_t *size) {
  size_t len = 0;
  (void) mp;
  if (f!=NULL)
    len = fread(*data,1,*size,(FILE *)f);
  *size = len;
}

@ @c
void mp_write_binary_file (MP mp, void *f, void *s, size_t size) {
  (void) mp;
  if (f!=NULL)
    (void)fwrite(s,size,1,(FILE *)f);
}


@ @c
void mp_close_file (MP mp, void *f) {
  (void) mp;
  if (f!=NULL)
    fclose((FILE *)f);
}

@ @c
int mp_eof_file (MP mp, void *f) {
  (void) mp;
  if (f!=NULL)
    return feof((FILE *)f);
   else 
    return 1;
}

@ @c
void mp_flush_file (MP mp, void *f) {
  (void) mp;
  if (f!=NULL)
    fflush((FILE *)f);
}

@ Input from text files is read one line at a time, using a routine called
|input_ln|. This function is defined in terms of global variables called
|buffer|, |first|, and |last| that will be described in detail later; for
now, it suffices for us to know that |buffer| is an array of |ASCII_code|
values, and that |first| and |last| are indices into this array
representing the beginning and ending of a line of text.

@<Glob...@>=
size_t buf_size; /* maximum number of characters simultaneously present in
                    current lines of open files */
ASCII_code *buffer; /* lines of characters being read */
size_t first; /* the first unused position in |buffer| */
size_t last; /* end of the line just input to |buffer| */
size_t max_buf_stack; /* largest index used in |buffer| */

@ @<Allocate or initialize ...@>=
mp->buf_size = 200;
mp->buffer = xmalloc((mp->buf_size+1),sizeof(ASCII_code));

@ @<Dealloc variables@>=
xfree(mp->buffer);

@ @c
static void mp_reallocate_buffer(MP mp, size_t l) {
  ASCII_code *buffer;
  if (l>max_halfword) {
    mp_confusion(mp,"buffer size"); /* can't happen (I hope) */
  }
  buffer = xmalloc((l+1),sizeof(ASCII_code));
  (void)memcpy(buffer,mp->buffer,(mp->buf_size+1));
  xfree(mp->buffer);
  mp->buffer = buffer ;
  mp->buf_size = l;
}

@ The |input_ln| function brings the next line of input from the specified
field into available positions of the buffer array and returns the value
|true|, unless the file has already been entirely read, in which case it
returns |false| and sets |last:=first|.  In general, the |ASCII_code|
numbers that represent the next line of the file are input into
|buffer[first]|, |buffer[first+1]|, \dots, |buffer[last-1]|; and the
global variable |last| is set equal to |first| plus the length of the
line. Trailing blanks are removed from the line; thus, either |last=first|
(in which case the line was entirely blank) or |buffer[last-1]<>" "|.
@^inner loop@>

The variable |max_buf_stack|, which is used to keep track of how large
the |buf_size| parameter must be to accommodate the present job, is
also kept up to date by |input_ln|.

@c 
static boolean mp_input_ln (MP mp, void *f ) {
  /* inputs the next line or returns |false| */
  char *s;
  size_t size = 0; 
  mp->last=mp->first; /* cf.\ Matthew 19\thinspace:\thinspace30 */
  s = (mp->read_ascii_file)(mp,f, &size);
  if (s==NULL)
	return false;
  if (size>0) {
    mp->last = mp->first+size;
    if ( mp->last>=mp->max_buf_stack ) { 
      mp->max_buf_stack=mp->last+1;
      while ( mp->max_buf_stack>mp->buf_size ) {
        mp_reallocate_buffer(mp,(mp->buf_size+(mp->buf_size>>2)));
      }
    }
    (void)memcpy((mp->buffer+mp->first),s,size);
  } 
  free(s);
  return true;
}

@ The user's terminal acts essentially like other files of text, except
that it is used both for input and for output. When the terminal is
considered an input file, the file variable is called |term_in|, and when it
is considered an output file the file variable is |term_out|.
@^system dependencies@>

@<Glob...@>=
void * term_in; /* the terminal as an input file */
void * term_out; /* the terminal as an output file */
void * err_out; /* the terminal as an output file */

@ Here is how to open the terminal files. In the default configuration,
nothing happens except that the command line (if there is one) is copied
to the input buffer.  The variable |command_line| will be filled by the 
|main| procedure. The copying can not be done earlier in the program 
logic because in the |INI| version, the |buffer| is also used for primitive 
initialization.

@d t_open_out  do {/* open the terminal for text output */
    mp->term_out = (mp->open_file)(mp,"terminal", "w", mp_filetype_terminal);
    mp->err_out = (mp->open_file)(mp,"error", "w", mp_filetype_error);
} while (0)
@d t_open_in  do { /* open the terminal for text input */
    mp->term_in = (mp->open_file)(mp,"terminal", "r", mp_filetype_terminal);
    if (mp->command_line!=NULL) {
      mp->last = strlen(mp->command_line);
      (void)memcpy((void *)mp->buffer,(void *)mp->command_line,mp->last);
      xfree(mp->command_line);
    } else {
	  mp->last = 0;
    }
} while (0)

@<Option variables@>=
char *command_line;

@ Sometimes it is necessary to synchronize the input/output mixture that
happens on the user's terminal, and three system-dependent
procedures are used for this
purpose. The first of these, |update_terminal|, is called when we want
to make sure that everything we have output to the terminal so far has
actually left the computer's internal buffers and been sent.
The second, |clear_terminal|, is called when we wish to cancel any
input that the user may have typed ahead (since we are about to
issue an unexpected error message). The third, |wake_up_terminal|,
is supposed to revive the terminal if the user has disabled it by
some instruction to the operating system.  The following macros show how
these operations can be specified:
@^system dependencies@>

@(mpmp.h@>=
#define update_terminal  (mp->flush_file)(mp,mp->term_out) /* empty the terminal output buffer */
#define clear_terminal   do_nothing /* clear the terminal input buffer */
#define wake_up_terminal (mp->flush_file)(mp,mp->term_out) 
                    /* cancel the user's cancellation of output */

@ We need a special routine to read the first line of \MP\ input from
the user's terminal. This line is different because it is read before we
have opened the transcript file; there is sort of a ``chicken and
egg'' problem here. If the user types `\.{input cmr10}' on the first
line, or if some macro invoked by that line does such an \.{input},
the transcript file will be named `\.{cmr10.log}'; but if no \.{input}
commands are performed during the first line of terminal input, the transcript
file will acquire its default name `\.{mpout.log}'. (The transcript file
will not contain error messages generated by the first line before the
first \.{input} command.)

The first line is even more special. It's nice to let the user start
running a \MP\ job by typing a command line like `\.{MP cmr10}'; in
such a case, \MP\ will operate as if the first line of input were
`\.{cmr10}', i.e., the first line will consist of the remainder of the
command line, after the part that invoked \MP.

@ Different systems have different ways to get started. But regardless of
what conventions are adopted, the routine that initializes the terminal
should satisfy the following specifications:

\yskip\textindent{1)}It should open file |term_in| for input from the
  terminal. (The file |term_out| will already be open for output to the
  terminal.)

\textindent{2)}If the user has given a command line, this line should be
  considered the first line of terminal input. Otherwise the
  user should be prompted with `\.{**}', and the first line of input
  should be whatever is typed in response.

\textindent{3)}The first line of input, which might or might not be a
  command line, should appear in locations |first| to |last-1| of the
  |buffer| array.

\textindent{4)}The global variable |loc| should be set so that the
  character to be read next by \MP\ is in |buffer[loc]|. This
  character should not be blank, and we should have |loc<last|.

\yskip\noindent(It may be necessary to prompt the user several times
before a non-blank line comes in. The prompt is `\.{**}' instead of the
later `\.*' because the meaning is slightly different: `\.{input}' need
not be typed immediately after~`\.{**}'.)

@d loc mp->cur_input.loc_field /* location of first unread character in |buffer| */

@c 
boolean mp_init_terminal (MP mp) { /* gets the terminal input started */
  t_open_in; 
  if (mp->last!=0) {
    loc = 0; mp->first = 0;
	return true;
  }
  while (1) { 
    if (!mp->noninteractive) {
	  wake_up_terminal; do_fprintf(mp->term_out,"**"); update_terminal;
@.**@>
    }
    if ( ! mp_input_ln(mp, mp->term_in ) ) { /* this shouldn't happen */
      do_fprintf(mp->term_out,"\n! End of file on the terminal... why?");
@.End of file on the terminal@>
      return false;
    }
    loc=(halfword)mp->first;
    while ( (loc<(int)mp->last)&&(mp->buffer[loc]==' ') ) 
      incr(loc);
    if ( loc<(int)mp->last ) { 
      return true; /* return unless the line was all blank */
    }
    if (!mp->noninteractive) {
	  do_fprintf(mp->term_out,"Please type the name of your input file.\n");
    }
  }
}

@ @<Declarations@>=
static boolean mp_init_terminal (MP mp) ;


@* \[4] String handling.
Symbolic token names and diagnostic messages are variable-length strings
of eight-bit characters. Many strings \MP\ uses are simply literals
in the compiled source, like the error messages and the names of the
internal parameters. Other strings are used or defined from the \MP\ input 
language, and these have to be interned.

\MP\ uses strings more extensively than \MF\ does, but the necessary
operations can still be handled with a fairly simple data structure.
The array |str_pool| contains all of the (eight-bit) ASCII codes in all
of the strings, and the array |str_start| contains indices of the starting
points of each string. Strings are referred to by integer numbers, so that
string number |s| comprises the characters |str_pool[j]| for
|str_start[s]<=j<str_start[ss]| where |ss=next_str[s]|.  The string pool
is allocated sequentially and |str_pool[pool_ptr]| is the next unused
location.  The first string number not currently in use is |str_ptr|
and |next_str[str_ptr]| begins a list of free string numbers.  String
pool entries |str_start[str_ptr]| up to |pool_ptr| are reserved for a
string currently being constructed.

String numbers 0 to 255 are reserved for strings that correspond to single
ASCII characters. This is in accordance with the conventions of \.{WEB},
@.WEB@>
which converts single-character strings into the ASCII code number of the
single character involved, while it converts other strings into integers
and builds a string pool file. Thus, when the string constant \.{"."} appears
in the program below, \.{WEB} converts it into the integer 46, which is the
ASCII code for a period, while \.{WEB} will convert a string like \.{"hello"}
into some integer greater than~255. String number 46 will presumably be the
single character `\..'\thinspace; but some ASCII codes have no standard visible
representation, and \MP\ may need to be able to print an arbitrary
ASCII character, so the first 256 strings are used to specify exactly what
should be printed for each of the 256 possibilities.

@<Types...@>=
typedef int pool_pointer; /* for variables that point into |str_pool| */
typedef int str_number; /* for variables that point into |str_start| */

@ @<Glob...@>=
ASCII_code *str_pool; /* the characters */
pool_pointer *str_start; /* the starting pointers */
str_number *next_str; /* for linking strings in order */
pool_pointer pool_ptr; /* first unused position in |str_pool| */
str_number str_ptr; /* number of the current string being created */
pool_pointer init_pool_ptr; /* the starting value of |pool_ptr| */
str_number init_str_use; /* the initial number of strings in use */
pool_pointer max_pool_ptr; /* the maximum so far of |pool_ptr| */
str_number max_str_ptr; /* the maximum so far of |str_ptr| */

@ @<Allocate or initialize ...@>=
mp->str_pool  = xmalloc ((mp->pool_size +1),sizeof(ASCII_code));
mp->str_start = xmalloc ((mp->max_strings+1),sizeof(pool_pointer));
mp->next_str  = xmalloc ((mp->max_strings+1),sizeof(str_number));

@ @<Dealloc variables@>=
xfree(mp->str_pool);
xfree(mp->str_start);
xfree(mp->next_str);

@ Most printing is done from |char *|s, but sometimes not. Here are
functions that convert an internal string into a |char *| for use
by the printing routines, and vice versa.

@d str(A) mp_str(mp,A)
@d rts(A) mp_rts(mp,A)
@d null_str rts("")

@<Internal ...@>=
int mp_xstrcmp (const char *a, const char *b);
char * mp_str (MP mp, str_number s);

@ @<Declarations@>=
static str_number mp_rts (MP mp, const char *s);
static str_number mp_make_string (MP mp);

@ @c 
int mp_xstrcmp (const char *a, const char *b) {
	if (a==NULL && b==NULL) 
	  return 0;
    if (a==NULL)
      return -1;
    if (b==NULL)
      return 1;
    return strcmp(a,b);
}

@ The attempt to catch interrupted strings that is in |mp_rts|, is not 
very good: it does not handle nesting over more than one level.

@c
char * mp_str (MP mp, str_number ss) {
  char *s;
  size_t len;
  if (ss==mp->str_ptr) {
    return NULL;
  } else {
    len = (size_t)length(ss);
    s = xmalloc(len+1,sizeof(char));
    (void)memcpy(s,(char *)(mp->str_pool+(mp->str_start[ss])),len);
    s[len] = 0;
    return (char *)s;
  }
}
str_number mp_rts (MP mp, const char *s) {
  int r; /* the new string */ 
  int old; /* a possible string in progress */
  int i=0;
  if (strlen(s)==0) {
    return 256;
  } else if (strlen(s)==1) {
    return s[0];
  } else {
   old=0;
   str_room((integer)strlen(s));
   if (mp->str_start[mp->str_ptr]<mp->pool_ptr)
     old = mp_make_string(mp);
   while (*s) {
     append_char(*s);
     s++;
   }
   r = mp_make_string(mp);
   if (old!=0) {
      str_room(length(old));
      while (i<length(old)) {
        append_char((mp->str_start[old]+i));
      } 
      mp_flush_string(mp,old);
    }
    return r;
  }
}

@ Except for |strs_used_up|, the following string statistics are only
maintained when code between |stat| $\ldots$ |tats| delimiters is not
commented out:

@<Glob...@>=
integer strs_used_up; /* strings in use or unused but not reclaimed */
integer pool_in_use; /* total number of cells of |str_pool| actually in use */
integer strs_in_use; /* total number of strings actually in use */
integer max_pl_used; /* maximum |pool_in_use| so far */
integer max_strs_used; /* maximum |strs_in_use| so far */

@ Several of the elementary string operations are performed using \.{WEB}
macros instead of functions, because many of the
operations are done quite frequently and we want to avoid the
overhead of procedure calls. For example, here is
a simple macro that computes the length of a string.
@.WEB@>

@d str_stop(A) mp->str_start[mp->next_str[(A)]] /* one cell past the end of string \# */
@d length(A) (str_stop((A))-mp->str_start[(A)]) /* the number of characters in string \# */

@ The length of the current string is called |cur_length|.  If we decide that
the current string is not needed, |flush_cur_string| resets |pool_ptr| so that
|cur_length| becomes zero.

@d cur_length   (mp->pool_ptr - mp->str_start[mp->str_ptr])
@d flush_cur_string   mp->pool_ptr=mp->str_start[mp->str_ptr]

@ Strings are created by appending character codes to |str_pool|.
The |append_char| macro, defined here, does not check to see if the
value of |pool_ptr| has gotten too high; this test is supposed to be
made before |append_char| is used.

To test if there is room to append |l| more characters to |str_pool|,
we shall write |str_room(l)|, which tries to make sure there is enough room
by compacting the string pool if necessary.  If this does not work,
|do_compaction| aborts \MP\ and gives an apologetic error message.

@d append_char(A)   /* put |ASCII_code| \# at the end of |str_pool| */
{ mp->str_pool[mp->pool_ptr]=(ASCII_code)(A); incr(mp->pool_ptr);
}
@d str_room(A)   /* make sure that the pool hasn't overflowed */
  { if ( mp->pool_ptr+(A) > mp->max_pool_ptr ) {
    if ( mp->pool_ptr+(A) > mp->pool_size ) mp_do_compaction(mp, (A));
    else mp->max_pool_ptr=mp->pool_ptr+(A); }
  }

@ The following routine is similar to |str_room(1)| but it uses the
argument |mp->pool_size| to prevent |do_compaction| from aborting when
string space is exhausted.

@<Declarations@>=
static void mp_unit_str_room (MP mp);

@ @c
void mp_unit_str_room (MP mp) { 
  if ( mp->pool_ptr>=mp->pool_size ) mp_do_compaction(mp, mp->pool_size);
  if ( mp->pool_ptr>=mp->max_pool_ptr ) mp->max_pool_ptr=mp->pool_ptr+1;
}

@ \MP's string expressions are implemented in a brute-force way: Every
new string or substring that is needed is simply copied into the string pool.
Space is eventually reclaimed by a procedure called |do_compaction| with
the aid of a simple system system of reference counts.
@^reference counts@>

The number of references to string number |s| will be |str_ref[s]|. The
special value |str_ref[s]=max_str_ref=127| is used to denote an unknown
positive number of references; such strings will never be recycled. If
a string is ever referred to more than 126 times, simultaneously, we
put it in this category. Hence a single byte suffices to store each |str_ref|.

@d max_str_ref 127 /* ``infinite'' number of references */
@d add_str_ref(A) { if ( mp->str_ref[(A)]<max_str_ref ) incr(mp->str_ref[(A)]); }

@<Glob...@>=
int *str_ref;

@ @<Allocate or initialize ...@>=
mp->str_ref = xmalloc ((mp->max_strings+1),sizeof(int));

@ @<Dealloc variables@>=
xfree(mp->str_ref);

@ Here's what we do when a string reference disappears:

@d delete_str_ref(A)  { 
    if ( mp->str_ref[(A)]<max_str_ref ) {
       if ( mp->str_ref[(A)]>1 ) decr(mp->str_ref[(A)]); 
       else mp_flush_string(mp, (A));
    }
  }

@<Declarations@>=
static void mp_flush_string (MP mp,str_number s) ;

@ We can't flush the first set of static strings at all, so there 
is no point in trying

@c
void mp_flush_string (MP mp,str_number s) { 
  if (length(s)>1) {
    mp->pool_in_use=mp->pool_in_use-length(s);
    decr(mp->strs_in_use);
    if ( mp->next_str[s]!=mp->str_ptr ) {
      mp->str_ref[s]=0;
    } else { 
      mp->str_ptr=s;
      decr(mp->strs_used_up);
    }
    mp->pool_ptr=mp->str_start[mp->str_ptr];
  }
}

@ C literals cannot be simply added, they need to be set so they can't
be flushed.

@d intern(A) mp_intern(mp,(A))

@c
str_number mp_intern (MP mp, const char *s) {
  str_number r ;
  r = rts(s);
  mp->str_ref[r] = max_str_ref;
  return r;
}

@ @<Declarations@>=
static str_number mp_intern (MP mp, const char *s);


@ Once a sequence of characters has been appended to |str_pool|, it
officially becomes a string when the function |make_string| is called.
This function returns the identification number of the new string as its
value.

When getting the next unused string number from the linked list, we pretend
that
$$ \hbox{|max_str_ptr+1|, |max_str_ptr+2|, $\ldots$, |mp->max_strings|} $$
are linked sequentially even though the |next_str| entries have not been
initialized yet.  We never allow |str_ptr| to reach |mp->max_strings|;
|do_compaction| is responsible for making sure of this.

@<Declarations@>=
static str_number mp_make_string (MP mp);

@ @c 
str_number mp_make_string (MP mp) { /* current string enters the pool */
  str_number s; /* the new string */
RESTART: 
  s=mp->str_ptr;
  mp->str_ptr=mp->next_str[s];
  if ( mp->str_ptr>mp->max_str_ptr ) {
    if ( mp->str_ptr==mp->max_strings ) { 
      mp->str_ptr=s;
      mp_do_compaction(mp, 0);
      goto RESTART;
    } else {
      mp->max_str_ptr=mp->str_ptr;
      mp->next_str[mp->str_ptr]=mp->max_str_ptr+1;
    }
  }
  mp->str_ref[s]=1;
  mp->str_start[mp->str_ptr]=mp->pool_ptr;
  incr(mp->strs_used_up);
  incr(mp->strs_in_use);
  mp->pool_in_use=mp->pool_in_use+length(s);
  if ( mp->pool_in_use>mp->max_pl_used ) 
    mp->max_pl_used=mp->pool_in_use;
  if ( mp->strs_in_use>mp->max_strs_used ) 
    mp->max_strs_used=mp->strs_in_use;
  return s;
}

@ The most interesting string operation is string pool compaction.  The idea
is to recover unused space in the |str_pool| array by recopying the strings
to close the gaps created when some strings become unused.  All string
numbers~$k$ where |str_ref[k]=0| are to be linked into the list of free string
numbers after |str_ptr|.  If this fails to free enough pool space we issue an
|overflow| error unless |needed=mp->pool_size|.  Calling |do_compaction|
with |needed=mp->pool_size| supresses all overflow tests.

The compaction process starts with |last_fixed_str| because all lower numbered
strings are permanently allocated with |max_str_ref| in their |str_ref| entries.

@<Glob...@>=
str_number last_fixed_str; /* last permanently allocated string */
str_number fixed_str_use; /* number of permanently allocated strings */

@ @<Internal library ...@>=
void mp_do_compaction (MP mp, pool_pointer needed) ;

@ @c
void mp_do_compaction (MP mp, pool_pointer needed) {
  str_number str_use; /* a count of strings in use */
  str_number r,s,t; /* strings being manipulated */
  pool_pointer p,q; /* destination and source for copying string characters */
  @<Advance |last_fixed_str| as far as possible and set |str_use|@>;
  r=mp->last_fixed_str;
  s=mp->next_str[r];
  p=mp->str_start[s];
  while ( s!=mp->str_ptr ) { 
    while ( mp->str_ref[s]==0 ) {
      @<Advance |s| and add the old |s| to the list of free string numbers;
        then |break| if |s=str_ptr|@>;
    }
    r=s; s=mp->next_str[s];
    incr(str_use);
    @<Move string |r| back so that |str_start[r]=p|; make |p| the location
     after the end of the string@>;
  }
DONE:   
  @<Move the current string back so that it starts at |p|@>;
  if ( needed<mp->pool_size ) {
    @<Make sure that there is room for another string with |needed| characters@>;
  }
  @<Account for the compaction and make sure the statistics agree with the
     global versions@>;
  mp->strs_used_up=str_use;
}

@ @<Advance |last_fixed_str| as far as possible and set |str_use|@>=
t=mp->next_str[mp->last_fixed_str];
while (t!=mp->str_ptr && mp->str_ref[t]==max_str_ref) {
  incr(mp->fixed_str_use);
  mp->last_fixed_str=t;
  t=mp->next_str[t];
}
str_use=mp->fixed_str_use

@ Because of the way |flush_string| has been written, it should never be
necessary to |break| here.  The extra line of code seems worthwhile to
preserve the generality of |do_compaction|.

@<Advance |s| and add the old |s| to the list of free string numbers;...@>=
{
t=s;
s=mp->next_str[s];
mp->next_str[r]=s;
mp->next_str[t]=mp->next_str[mp->str_ptr];
mp->next_str[mp->str_ptr]=t;
if ( s==mp->str_ptr ) goto DONE;
}

@ The string currently starts at |str_start[r]| and ends just before
|str_start[s]|.  We don't change |str_start[s]| because it might be needed
to locate the next string.

@<Move string |r| back so that |str_start[r]=p|; make |p| the location...@>=
q=mp->str_start[r];
mp->str_start[r]=p;
while ( q<mp->str_start[s] ) { 
  mp->str_pool[p]=mp->str_pool[q];
  incr(p); incr(q);
}

@ Pointers |str_start[str_ptr]| and |pool_ptr| have not been updated.  When
we do this, anything between them should be moved.

@ @<Move the current string back so that it starts at |p|@>=
q=mp->str_start[mp->str_ptr];
mp->str_start[mp->str_ptr]=p;
while ( q<mp->pool_ptr ) { 
  mp->str_pool[p]=mp->str_pool[q];
  incr(p); incr(q);
}
mp->pool_ptr=p

@ We must remember that |str_ptr| is not allowed to reach |mp->max_strings|.

@<Make sure that there is room for another string with |needed| char...@>=
if ( str_use>=mp->max_strings-1 )
  mp_reallocate_strings (mp,str_use);
if ( mp->pool_ptr+needed>mp->max_pool_ptr ) {
  mp_reallocate_pool(mp, mp->pool_ptr+needed);
  mp->max_pool_ptr=mp->pool_ptr+needed;
}

@ @<Internal library ...@>=
void mp_reallocate_strings (MP mp, str_number str_use) ;
void mp_reallocate_pool(MP mp, pool_pointer needed) ;

@ @c 
void mp_reallocate_strings (MP mp, str_number str_use) { 
  while ( str_use>=mp->max_strings-1 ) {
    int l = mp->max_strings + (mp->max_strings/4);
    XREALLOC (mp->str_ref,   l, int);
    XREALLOC (mp->str_start, l, pool_pointer);
    XREALLOC (mp->next_str,  l, str_number);
    mp->max_strings = l;
  }
}
void mp_reallocate_pool(MP mp, pool_pointer needed) {
  while ( needed>mp->pool_size ) {
    int l = mp->pool_size + (mp->pool_size/4);
 	XREALLOC (mp->str_pool, l, ASCII_code);
    mp->pool_size = l;
  }
}

@ @<Account for the compaction and make sure the statistics agree with...@>=
if ( (mp->str_start[mp->str_ptr]!=mp->pool_in_use)||(str_use!=mp->strs_in_use) )
  mp_confusion(mp, "string");
@:this can't happen string}{\quad string@>
incr(mp->pact_count);
mp->pact_chars=mp->pact_chars+mp->pool_ptr-str_stop(mp->last_fixed_str);
mp->pact_strs=mp->pact_strs+str_use-mp->fixed_str_use;

@ A few more global variables are needed to keep track of statistics when
|stat| $\ldots$ |tats| blocks are not commented out.

@<Glob...@>=
integer pact_count; /* number of string pool compactions so far */
integer pact_chars; /* total number of characters moved during compactions */
integer pact_strs; /* total number of strings moved during compactions */

@ @<Initialize compaction statistics@>=
mp->pact_count=0;
mp->pact_chars=0;
mp->pact_strs=0;

@ The following subroutine compares string |s| with another string of the
same length that appears in |buffer| starting at position |k|;
the result is |true| if and only if the strings are equal.

@c 
static boolean mp_str_eq_buf (MP mp,str_number s, integer k) {
  /* test equality of strings */
  pool_pointer j; /* running index */
  j=mp->str_start[s];
  while ( j<str_stop(s) ) { 
    if ( mp->str_pool[j++]!=mp->buffer[k++] ) 
      return false;
  }
  return true;
}

@ This routine compares a pool string with a sequence of characters
of equal length.

@c 
static boolean mp_str_eq_cstr (MP mp,str_number s, char *k) {
  /* test equality of strings */
  pool_pointer j; /* running index */
  j=mp->str_start[s];
  while ( j<str_stop(s) ) { 
    if ( mp->str_pool[j++]!=*k++ ) 
      return false;
  }
  return true;
}

@ Here is a similar routine, but it compares two strings in the string pool,
and it does not assume that they have the same length. If the first string
is lexicographically greater than, less than, or equal to the second,
the result is respectively positive, negative, or zero.

@c 
static integer mp_str_vs_str (MP mp, str_number s, str_number t) {
  /* test equality of strings */
  pool_pointer j,k; /* running indices */
  integer ls,lt; /* lengths */
  integer l; /* length remaining to test */
  ls=length(s); lt=length(t);
  if ( ls<=lt ) l=ls; else l=lt;
  j=mp->str_start[s]; k=mp->str_start[t];
  while ( l-->0 ) { 
    if ( mp->str_pool[j]!=mp->str_pool[k] ) {
       return (mp->str_pool[j]-mp->str_pool[k]); 
    }
    j++; k++;
  }
  return (ls-lt);
}

@ The initial values of |str_pool|, |str_start|, |pool_ptr|,
and |str_ptr| are computed by the \.{INIMP} program, based in part
on the information that \.{WEB} has output while processing \MP.
@.INIMP@>
@^string pool@>

@c 
void mp_get_strings_started (MP mp) { 
  /* initializes the string pool,
    but returns |false| if something goes wrong */
  int k; /* small indices or counters */
  str_number g; /* a new string */
  mp->pool_ptr=0; mp->str_ptr=0; mp->max_pool_ptr=0; mp->max_str_ptr=0;
  mp->str_start[0]=0;
  mp->next_str[0]=1;
  mp->pool_in_use=0; mp->strs_in_use=0;
  mp->max_pl_used=0; mp->max_strs_used=0;
  @<Initialize compaction statistics@>;
  mp->strs_used_up=0;
  @<Make the first 256 strings@>;
  g=mp_make_string(mp); /* string 256 == "" */
  mp->str_ref[g]=max_str_ref;
  mp->last_fixed_str=mp->str_ptr-1;
  mp->fixed_str_use=mp->str_ptr;
  return;
}

@ @<Declarations@>=
static void mp_get_strings_started (MP mp);

@ The first 256 strings will consist of a single character only.

@<Make the first 256...@>=
for (k=0;k<=255;k++) { 
  append_char(k);
  g=mp_make_string(mp); 
  mp->str_ref[g]=max_str_ref;
}

@ The first 128 strings will contain 95 standard ASCII characters, and the
other 33 characters will be printed in three-symbol form like `\.{\^\^A}'
unless a system-dependent change is made here. Installations that have
an extended character set, where for example |xchr[032]=@t\.{'^^Z'}@>|,
would like string 032 to be printed as the single character 032 instead
of the three characters 0136, 0136, 0132 (\.{\^\^Z}). On the other hand,
even people with an extended character set will want to represent string
015 by \.{\^\^M}, since 015 is ASCII's ``carriage return'' code; the idea is
to produce visible strings instead of tabs or line-feeds or carriage-returns
or bell-rings or characters that are treated anomalously in text files.

The boolean expression defined here should be |true| unless \MP\ internal
code number~|k| corresponds to a non-troublesome visible symbol in the
local character set.
If character |k| cannot be printed, and |k<0200|, then character |k+0100| or
|k-0100| must be printable; moreover, ASCII codes |[060..071, 0141..0146]|
must be printable.
@^character set dependencies@>
@^system dependencies@>

@<Character |k| cannot be printed@>=
  (k<' ')||(k==127)

@* \[5] On-line and off-line printing.
Messages that are sent to a user's terminal and to the transcript-log file
are produced by several `|print|' procedures. These procedures will
direct their output to a variety of places, based on the setting of
the global variable |selector|, which has the following possible
values:

\yskip
\hang |term_and_log|, the normal setting, prints on the terminal and on the
  transcript file.

\hang |log_only|, prints only on the transcript file.

\hang |term_only|, prints only on the terminal.

\hang |no_print|, doesn't print at all. This is used only in rare cases
  before the transcript file is open.

\hang |pseudo|, puts output into a cyclic buffer that is used
  by the |show_context| routine; when we get to that routine we shall discuss
  the reasoning behind this curious mode.

\hang |new_string|, appends the output to the current string in the
  string pool.

\hang |>=write_file| prints on one of the files used for the \&{write}
@:write_}{\&{write} primitive@>
  command.

\yskip
\noindent The symbolic names `|term_and_log|', etc., have been assigned
numeric codes that satisfy the convenient relations |no_print+1=term_only|,
|no_print+2=log_only|, |term_only+2=log_only+1=term_and_log|.  These
relations are not used when |selector| could be |pseudo|, or |new_string|.
We need not check for unprintable characters when |selector<pseudo|.

Three additional global variables, |tally|, |term_offset| and |file_offset|
record the number of characters that have been printed
since they were most recently cleared to zero. We use |tally| to record
the length of (possibly very long) stretches of printing; |term_offset|,
and |file_offset|, on the other hand, keep track of how many
characters have appeared so far on the current line that has been output
to the terminal, the transcript file, or the \ps\ output file, respectively.

@d new_string 0 /* printing is deflected to the string pool */
@d pseudo 2 /* special |selector| setting for |show_context| */
@d no_print 3 /* |selector| setting that makes data disappear */
@d term_only 4 /* printing is destined for the terminal only */
@d log_only 5 /* printing is destined for the transcript file only */
@d term_and_log 6 /* normal |selector| setting */
@d write_file 7 /* first write file selector */

@<Glob...@>=
void * log_file; /* transcript of \MP\ session */
void * output_file; /* the generic font output goes here */
unsigned int selector; /* where to print a message */
unsigned char dig[23]; /* digits in a number, for rounding */
integer tally; /* the number of characters recently printed */
unsigned int term_offset;
  /* the number of characters on the current terminal line */
unsigned int file_offset;
  /* the number of characters on the current file line */
ASCII_code *trick_buf; /* circular buffer for pseudoprinting */
integer trick_count; /* threshold for pseudoprinting, explained later */
integer first_count; /* another variable for pseudoprinting */

@ @<Allocate or initialize ...@>=
mp->trick_buf = xmalloc((mp->error_line+1),sizeof(ASCII_code));

@ @<Dealloc variables@>=
xfree(mp->trick_buf);

@ @<Initialize the output routines@>=
mp->selector=term_only; mp->tally=0; mp->term_offset=0; mp->file_offset=0; 

@ Macro abbreviations for output to the terminal and to the log file are
defined here for convenience. Some systems need special conventions
for terminal output, and it is possible to adhere to those conventions
by changing |wterm|, |wterm_ln|, and |wterm_cr| here.
@^system dependencies@>

@(mpmp.h@>=
#define do_fprintf(f,b) (mp->write_ascii_file)(mp,f,b)
#define wterm(A)     do_fprintf(mp->term_out,(A))
#define wterm_chr(A) { unsigned char ss[2]; ss[0]=(A); ss[1]='\0'; wterm((char *)ss);}
#define wterm_cr     do_fprintf(mp->term_out,"\n")
#define wterm_ln(A)  { wterm_cr; do_fprintf(mp->term_out,(A)); }
#define wlog(A)        do_fprintf(mp->log_file,(A))
#define wlog_chr(A)  { unsigned char ss[2]; ss[0]=(A); ss[1]='\0'; wlog((char *)ss);}
#define wlog_cr      do_fprintf(mp->log_file, "\n")
#define wlog_ln(A)   { wlog_cr; do_fprintf(mp->log_file,(A)); }


@ To end a line of text output, we call |print_ln|.  Cases |0..max_write_files|
use an array |wr_file| that will be declared later.

@d mp_print_text(A) mp_print_str(mp,text((A)))

@<Internal library ...@>=
void mp_print (MP mp, const char *s);
void mp_print_ln (MP mp);
void mp_print_visible_char (MP mp, ASCII_code s); 
void mp_print_char (MP mp, ASCII_code k);
void mp_print_str (MP mp, str_number s);
void mp_print_nl (MP mp, const char *s);
void mp_print_two (MP mp,scaled x, scaled y) ;
void mp_print_scaled (MP mp,scaled s);

@ @<Basic print...@>=
void mp_print_ln (MP mp) { /* prints an end-of-line */
 switch (mp->selector) {
  case term_and_log: 
    wterm_cr; wlog_cr;
    mp->term_offset=0;  mp->file_offset=0;
    break;
  case log_only: 
    wlog_cr; mp->file_offset=0;
    break;
  case term_only: 
    wterm_cr; mp->term_offset=0;
    break;
  case no_print:
  case pseudo: 
  case new_string: 
    break;
  default: 
    do_fprintf(mp->wr_file[(mp->selector-write_file)],"\n");
  }
} /* note that |tally| is not affected */

@ The |print_visible_char| procedure sends one character to the desired
destination, using the |xchr| array to map it into an external character
compatible with |input_ln|.  (It assumes that it is always called with
a visible ASCII character.)  All printing comes through |print_ln| or
|print_char|, which ultimately calls |print_visible_char|, hence these
routines are the ones that limit lines to at most |max_print_line| characters.
But we must make an exception for the \ps\ output file since it is not safe
to cut up lines arbitrarily in \ps.

Procedure |unit_str_room| needs to be declared |forward| here because it calls
|do_compaction| and |do_compaction| can call the error routines.  Actually,
|unit_str_room| avoids |overflow| errors but it can call |confusion|.

@<Basic printing...@>=
void mp_print_visible_char (MP mp, ASCII_code s) { /* prints a single character */
  switch (mp->selector) {
  case term_and_log: 
    wterm_chr(xchr(s)); wlog_chr(xchr(s));
    incr(mp->term_offset); incr(mp->file_offset);
    if ( mp->term_offset==(unsigned)mp->max_print_line ) { 
       wterm_cr; mp->term_offset=0;
    };
    if ( mp->file_offset==(unsigned)mp->max_print_line ) { 
       wlog_cr; mp->file_offset=0;
    };
    break;
  case log_only: 
    wlog_chr(xchr(s)); incr(mp->file_offset);
    if ( mp->file_offset==(unsigned)mp->max_print_line ) mp_print_ln(mp);
    break;
  case term_only: 
    wterm_chr(xchr(s)); incr(mp->term_offset);
    if ( mp->term_offset==(unsigned)mp->max_print_line ) mp_print_ln(mp);
    break;
  case no_print: 
    break;
  case pseudo: 
    if ( mp->tally<mp->trick_count ) 
      mp->trick_buf[mp->tally % mp->error_line]=s;
    break;
  case new_string: 
    if (mp->pool_ptr>=mp->pool_size || mp->pool_ptr>=mp->max_pool_ptr ) { 
      mp_unit_str_room(mp);
      if ( mp->pool_ptr>=mp->pool_size ) 
        goto DONE; /* drop characters if string space is full */
    };
    append_char(s);
    break;
  default:
    { text_char ss[2]; ss[0] = xchr(s); ss[1]=0;
      do_fprintf(mp->wr_file[(mp->selector-write_file)],(char *)ss);
    }
  }
DONE:
  incr(mp->tally);
}

@ The |print_char| procedure sends one character to the desired destination.
File names and string expressions might contain |ASCII_code| values that
can't be printed using |print_visible_char|.  These characters will be
printed in three- or four-symbol form like `\.{\^\^A}' or `\.{\^\^e4}'.
(This procedure assumes that it is safe to bypass all checks for unprintable
characters when |selector| is in the range |0..max_write_files-1|.
The user might want to write unprintable characters.

@<Basic printing...@>=
void mp_print_char (MP mp, ASCII_code k) { /* prints a single character */
  if ( mp->selector<pseudo || mp->selector>=write_file) {
    mp_print_visible_char(mp, k);
  } else if ( @<Character |k| cannot be printed@> ) { 
    mp_print(mp, "^^"); 
    if ( k<0100 ) { 
      mp_print_visible_char(mp, (ASCII_code)(k+0100)); 
    } else if ( k<0200 ) { 
      mp_print_visible_char(mp, (ASCII_code)(k-0100)); 
    } else {
      int l; /* small index or counter */
      l = (k / 16);
      mp_print_visible_char(mp, xord(l<10 ? l+'0' : l-10+'a'));
      l = (k % 16);
      mp_print_visible_char(mp, xord(l<10 ? l+'0' : l-10+'a'));
    }
  } else {
    mp_print_visible_char(mp, k);
  }
}

@ An entire string is output by calling |print|. Note that if we are outputting
the single standard ASCII character \.c, we could call |print("c")|, since
|"c"=99| is the number of a single-character string, as explained above. But
|print_char("c")| is quicker, so \MP\ goes directly to the |print_char|
routine when it knows that this is safe. (The present implementation
assumes that it is always safe to print a visible ASCII character.)
@^system dependencies@>

@<Basic print...@>=
static void mp_do_print (MP mp, const char *ss, size_t len) { /* prints string |s| */
  size_t j = 0;
  if (mp->selector == new_string) {
    str_room((integer)(len*4));
  }
  while ( j<len ){ 
    /* this was |xord((int)ss[j])| but that doesnt work */
    mp_print_char(mp, (ASCII_code)ss[j]); j++;
  }
}

@ 
@<Basic print...@>=
void mp_print (MP mp, const char *ss) {
  if (ss==NULL) return;
  mp_do_print(mp, ss,strlen(ss));
}

@ This function is somewhat less trivial than expected
because it is not safe to directly print data in the
string pool since |mp_do_print()| can potentially reallocate 
the whole lot.

@<Basic print...@>=
void mp_print_str (MP mp, str_number s) {
  pool_pointer j; /* current character code position */
  char *ss; /* a temporary C string */
  size_t len; /* its length */
  if ( (s<0)||(s>mp->max_str_ptr) ) {
     mp_do_print(mp,"???",3); /* this can't happen */
@.???@>
  }
  j=mp->str_start[s];
  len = (size_t)(str_stop(s)-j);
  ss = xmalloc(len+1, sizeof(char));
  if (len > 0) {
    /* the man page doesnt say whether 0 is allowed */
    memcpy(ss,(char *)(mp->str_pool+j),len);
  }
  ss[len] = '\0';
  mp_do_print(mp, ss, len);
  mp_xfree(ss);
}


@ Here is the very first thing that \MP\ prints: a headline that identifies
the version number and base name. The |term_offset| variable is temporarily
incorrect, but the discrepancy is not serious since we assume that the banner
and mem identifier together will occupy at most |max_print_line|
character positions.

@<Initialize the output...@>=
wterm (mp->banner);
if (mp->mem_ident!=NULL) 
  mp_print(mp,mp->mem_ident); 
mp_print_ln(mp);
update_terminal;

@ The procedure |print_nl| is like |print|, but it makes sure that the
string appears at the beginning of a new line.

@<Basic print...@>=
void mp_print_nl (MP mp, const char *s) { /* prints string |s| at beginning of line */
  switch(mp->selector) {
  case term_and_log: 
    if ( (mp->term_offset>0)||(mp->file_offset>0) ) mp_print_ln(mp);
    break;
  case log_only: 
    if ( mp->file_offset>0 ) mp_print_ln(mp);
    break;
  case term_only: 
    if ( mp->term_offset>0 ) mp_print_ln(mp);
    break;
  case no_print:
  case pseudo:
  case new_string: 
	break;
  } /* there are no other cases */
  mp_print(mp, s);
}

@ The following procedure, which prints out the decimal representation of a
given integer |n|, assumes that all integers fit nicely into a |int|.
@^system dependencies@>

@<Basic print...@>=
void mp_print_int (MP mp,integer n) { /* prints an integer in decimal form */
  char s[12];
  mp_snprintf(s,12,"%d", (int)n);
  mp_print(mp,s);
}

@ @<Internal library ...@>=
void mp_print_int (MP mp,integer n);

@ \MP\ also makes use of a trivial procedure to print two digits. The
following subroutine is usually called with a parameter in the range |0<=n<=99|.

@c 
static void mp_print_dd (MP mp,integer n) { /* prints two least significant digits */
  n=abs(n) % 100; 
  mp_print_char(mp, xord('0'+(n / 10)));
  mp_print_char(mp, xord('0'+(n % 10)));
}


@ @<Declarations@>=
static void mp_print_dd (MP mp,integer n);

@ Here is a procedure that asks the user to type a line of input,
assuming that the |selector| setting is either |term_only| or |term_and_log|.
The input is placed into locations |first| through |last-1| of the
|buffer| array, and echoed on the transcript file if appropriate.

This procedure is never called when |interaction<mp_scroll_mode|.

@d prompt_input(A) do { 
    if (!mp->noninteractive) {
      wake_up_terminal; mp_print(mp, (A)); 
    }
    mp_term_input(mp);
  } while (0) /* prints a string and gets a line of input */

@c 
void mp_term_input (MP mp) { /* gets a line from the terminal */
  size_t k; /* index into |buffer| */
  if (mp->noninteractive) {
    if (!mp_input_ln(mp, mp->term_in ))
	  longjmp(*(mp->jump_buf),1);  /* chunk finished */
    mp->buffer[mp->last]=xord('%'); 
  } else {
    update_terminal; /* Now the user sees the prompt for sure */
    if (!mp_input_ln(mp, mp->term_in )) {
	  mp_fatal_error(mp, "End of file on the terminal!");
@.End of file on the terminal@>
    }
    mp->term_offset=0; /* the user's line ended with \<\rm return> */
    decr(mp->selector); /* prepare to echo the input */
    if ( mp->last!=mp->first ) {
      for (k=mp->first;k<mp->last;k++) {
        mp_print_char(mp, mp->buffer[k]);
      }
    }
    mp_print_ln(mp); 
    mp->buffer[mp->last]=xord('%'); 
    incr(mp->selector); /* restore previous status */
  }
}

@* \[6] Reporting errors.
When something anomalous is detected, \MP\ typically does something like this:
$$\vbox{\halign{#\hfil\cr
|print_err("Something anomalous has been detected");|\cr
|help3("This is the first line of my offer to help.")|\cr
|("This is the second line. I'm trying to")|\cr
|("explain the best way for you to proceed.");|\cr
|error;|\cr}}$$
A two-line help message would be given using |help2|, etc.; these informal
helps should use simple vocabulary that complements the words used in the
official error message that was printed. (Outside the U.S.A., the help
messages should preferably be translated into the local vernacular. Each
line of help is at most 60 characters long, in the present implementation,
so that |max_print_line| will not be exceeded.)

The |print_err| procedure supplies a `\.!' before the official message,
and makes sure that the terminal is awake if a stop is going to occur.
The |error| procedure supplies a `\..' after the official message, then it
shows the location of the error; and if |interaction=error_stop_mode|,
it also enters into a dialog with the user, during which time the help
message may be printed.
@^system dependencies@>

@ The global variable |interaction| has four settings, representing increasing
amounts of user interaction:

@<Exported types@>=
enum mp_interaction_mode { 
 mp_unspecified_mode=0, /* extra value for command-line switch */
 mp_batch_mode, /* omits all stops and omits terminal output */
 mp_nonstop_mode, /* omits all stops */
 mp_scroll_mode, /* omits error stops */
 mp_error_stop_mode /* stops at every opportunity to interact */
};

@ @<Option variables@>=
int interaction; /* current level of interaction */
int noninteractive; /* do we have a terminal? */

@ Set it here so it can be overwritten by the commandline

@<Allocate or initialize ...@>=
mp->interaction=opt->interaction;
if (mp->interaction==mp_unspecified_mode || mp->interaction>mp_error_stop_mode) 
  mp->interaction=mp_error_stop_mode;
if (mp->interaction<mp_unspecified_mode) 
  mp->interaction=mp_batch_mode;

@ 

@d print_err(A) mp_print_err(mp,(A))

@<Internal ...@>=
void mp_print_err(MP mp, const char * A);

@ @c
void mp_print_err(MP mp, const char * A) { 
  if ( mp->interaction==mp_error_stop_mode ) 
    wake_up_terminal;
  if (mp->file_line_error_style && file_state && !terminal_input) {
    mp_print_nl(mp, ""); 
    if (long_name != NULL) {
      mp_print(mp, long_name);      
    } else {
      mp_print(mp, mp_str(mp,name));
    }
    mp_print(mp, ":");
    mp_print_int(mp, line);
    mp_print(mp, ": ");
  } else{
    mp_print_nl(mp, "! "); 
  }
  mp_print(mp, A);
@.!\relax@>
}


@ \MP\ is careful not to call |error| when the print |selector| setting
might be unusual. The only possible values of |selector| at the time of
error messages are

\yskip\hang|no_print| (when |interaction=mp_batch_mode|
  and |log_file| not yet open);

\hang|term_only| (when |interaction>mp_batch_mode| and |log_file| not yet open);

\hang|log_only| (when |interaction=mp_batch_mode| and |log_file| is open);

\hang|term_and_log| (when |interaction>mp_batch_mode| and |log_file| is open).

@<Initialize the print |selector| based on |interaction|@>=
if ( mp->interaction==mp_batch_mode ) mp->selector=no_print; else mp->selector=term_only

@ A global variable |deletions_allowed| is set |false| if the |get_next|
routine is active when |error| is called; this ensures that |get_next|
will never be called recursively.
@^recursion@>

The global variable |history| records the worst level of error that
has been detected. It has four possible values: |spotless|, |warning_issued|,
|error_message_issued|, and |fatal_error_stop|.

Another global variable, |error_count|, is increased by one when an
|error| occurs without an interactive dialog, and it is reset to zero at
the end of every statement.  If |error_count| reaches 100, \MP\ decides
that there is no point in continuing further.

@<Exported types@>=
enum mp_history_state {
  mp_spotless=0, /* |history| value when nothing has been amiss yet */
  mp_warning_issued, /* |history| value when |begin_diagnostic| has been called */
  mp_error_message_issued, /* |history| value when |error| has been called */
  mp_fatal_error_stop, /* |history| value when termination was premature */
  mp_system_error_stop /* |history| value when termination was due to disaster */
};

@ @<Glob...@>=
boolean deletions_allowed; /* is it safe for |error| to call |get_next|? */
int history; /* has the source input been clean so far? */
int error_count; /* the number of scrolled errors since the last statement ended */

@ The value of |history| is initially |fatal_error_stop|, but it will
be changed to |spotless| if \MP\ survives the initialization process.

@<Allocate or ...@>=
mp->deletions_allowed=true; /* |history| is initialized elsewhere */

@ Since errors can be detected almost anywhere in \MP, we want to declare the
error procedures near the beginning of the program. But the error procedures
in turn use some other procedures, which need to be declared |forward|
before we get to |error| itself.

It is possible for |error| to be called recursively if some error arises
when |get_next| is being used to delete a token, and/or if some fatal error
occurs while \MP\ is trying to fix a non-fatal one. But such recursion
@^recursion@>
is never more than two levels deep.

@<Declarations@>=
static void mp_get_next (MP mp);
static void mp_term_input (MP mp);
static void mp_show_context (MP mp);
static void mp_begin_file_reading (MP mp);
static void mp_open_log_file (MP mp);
static void mp_clear_for_error_prompt (MP mp);

@ @<Internal ...@>=
void mp_normalize_selector (MP mp);

@ Individual lines of help are recorded in the array |help_line|, which
contains entries in positions |0..(help_ptr-1)|. They should be printed
in reverse order, i.e., with |help_line[0]| appearing last.

@d hlp1(A) mp->help_line[0]=A; }
@d hlp2(A,B) mp->help_line[1]=A; hlp1(B)
@d hlp3(A,B,C) mp->help_line[2]=A; hlp2(B,C)
@d hlp4(A,B,C,D) mp->help_line[3]=A; hlp3(B,C,D)
@d hlp5(A,B,C,D,E) mp->help_line[4]=A; hlp4(B,C,D,E)
@d hlp6(A,B,C,D,E,F) mp->help_line[5]=A; hlp5(B,C,D,E,F)
@d help0 mp->help_ptr=0 /* sometimes there might be no help */
@d help1  { mp->help_ptr=1; hlp1 /* use this with one help line */
@d help2  { mp->help_ptr=2; hlp2 /* use this with two help lines */
@d help3  { mp->help_ptr=3; hlp3 /* use this with three help lines */
@d help4  { mp->help_ptr=4; hlp4 /* use this with four help lines */
@d help5  { mp->help_ptr=5; hlp5 /* use this with five help lines */
@d help6  { mp->help_ptr=6; hlp6 /* use this with six help lines */

@<Glob...@>=
const char * help_line[6]; /* helps for the next |error| */
unsigned int help_ptr; /* the number of help lines present */
boolean use_err_help; /* should the |err_help| string be shown? */
str_number err_help; /* a string set up by \&{errhelp} */

@ @<Allocate or ...@>=
mp->use_err_help=false;

@ The |jump_out| procedure just cuts across all active procedure levels and
goes to |end_of_MP|. This is the only nonlocal |goto| statement in the
whole program. It is used when there is no recovery from a particular error.

The program uses a |jump_buf| to handle this, this is initialized at three
spots: the start of |mp_new|, the start of |mp_initialize|, and the start 
of |mp_run|. Those are the only library enty points.

@^system dependencies@>

@<Glob...@>=
jmp_buf *jump_buf;

@ If the array of internals is still |NULL| when |jump_out| is called, a
crash occured during initialization, and it is not safe to run the normal
cleanup routine.

@<Error hand...@>=
static void mp_jump_out (MP mp) { 
  if (mp->internal!=NULL && mp->history < mp_system_error_stop) 
    mp_close_files_and_terminate(mp);
  longjmp(*(mp->jump_buf),1);
}

@ Here now is the general |error| routine.

@<Error hand...@>=
void mp_error (MP mp) { /* completes the job of error reporting */
  ASCII_code c; /* what the user types */
  integer s1,s2,s3; /* used to save global variables when deleting tokens */
  pool_pointer j; /* character position being printed */
  if ( mp->history<mp_error_message_issued ) 
	mp->history=mp_error_message_issued;
  mp_print_char(mp, xord('.')); mp_show_context(mp);
  if (mp->halt_on_error) {
    mp->history=mp_fatal_error_stop; mp_jump_out(mp);
  }
  if ((!mp->noninteractive) && (mp->interaction==mp_error_stop_mode )) {
    @<Get user's advice and |return|@>;
  }
  incr(mp->error_count);
  if ( mp->error_count==100 ) { 
    mp_print_nl(mp,"(That makes 100 errors; please try again.)");
@.That makes 100 errors...@>
    mp->history=mp_fatal_error_stop; mp_jump_out(mp);
  }
  @<Put help message on the transcript file@>;
}
void mp_warn (MP mp, const char *msg) {
  unsigned saved_selector = mp->selector;
  mp_normalize_selector(mp);
  mp_print_nl(mp,"Warning: ");
  mp_print(mp,msg);
  mp_print_ln(mp);
  mp->selector = saved_selector;
}

@ @<Exported function ...@>=
extern void mp_error (MP mp);
extern void mp_warn (MP mp, const char *msg);


@ @<Get user's advice...@>=
while (true) { 
CONTINUE:
  mp_clear_for_error_prompt(mp); prompt_input("? ");
@.?\relax@>
  if ( mp->last==mp->first ) return;
  c=mp->buffer[mp->first];
  if ( c>='a' ) c=(ASCII_code)(c+'A'-'a'); /* convert to uppercase */
  @<Interpret code |c| and |return| if done@>;
}

@ It is desirable to provide an `\.E' option here that gives the user
an easy way to return from \MP\ to the system editor, with the offending
line ready to be edited. But such an extension requires some system
wizardry, so the present implementation simply types out the name of the
file that should be
edited and the relevant line number.
@^system dependencies@>

@<Exported types@>=
typedef void (*mp_editor_cmd)(MP, char *, int);

@ @<Option variables@>=
mp_editor_cmd run_editor;

@ @<Allocate or initialize ...@>=
set_callback_option(run_editor);

@ @<Declarations@>=
static void mp_run_editor (MP mp, char *fname, int fline);

@ @c 
void mp_run_editor (MP mp, char *fname, int fline) {
    char *s = xmalloc(256,1);
    mp_snprintf(s, 256,"You want to edit file %s at line %d\n", fname, fline);
    wterm_ln(s);
@.You want to edit file x@>
}

@ 
There is a secret `\.D' option available when the debugging routines haven't
been commented~out.
@^debugging@>

@<Interpret code |c| and |return| if done@>=
switch (c) {
case '0': case '1': case '2': case '3': case '4':
case '5': case '6': case '7': case '8': case '9': 
  if ( mp->deletions_allowed ) {
    @<Delete |c-"0"| tokens and |continue|@>;
  }
  break;
case 'E': 
  if ( mp->file_ptr>0 ){ 
    mp->interaction=mp_scroll_mode; 
    mp_close_files_and_terminate(mp);
    (mp->run_editor)(mp, 
                     str(mp->input_stack[mp->file_ptr].name_field), 
                     mp_true_line(mp));
    mp_jump_out(mp);
  }
  break;
case 'H': 
  @<Print the help information and |continue|@>;
  /* |break;| */
case 'I':
  @<Introduce new material from the terminal and |return|@>;
  /* |break;| */
case 'Q': case 'R': case 'S':
  @<Change the interaction level and |return|@>;
  /* |break;| */
case 'X':
  mp->interaction=mp_scroll_mode; mp_jump_out(mp);
  break;
default:
  break;
}
@<Print the menu of available options@>

@ @<Print the menu...@>=
{ 
  mp_print(mp, "Type <return> to proceed, S to scroll future error messages,");
@.Type <return> to proceed...@>
  mp_print_nl(mp, "R to run without stopping, Q to run quietly,");
  mp_print_nl(mp, "I to insert something, ");
  if ( mp->file_ptr>0 ) 
    mp_print(mp, "E to edit your file,");
  if ( mp->deletions_allowed )
    mp_print_nl(mp, "1 or ... or 9 to ignore the next 1 to 9 tokens of input,");
  mp_print_nl(mp, "H for help, X to quit.");
}

@ Here the author of \MP\ apologizes for making use of the numerical
relation between |"Q"|, |"R"|, |"S"|, and the desired interaction settings
|mp_batch_mode|, |mp_nonstop_mode|, |mp_scroll_mode|.
@^Knuth, Donald Ervin@>

@<Change the interaction...@>=
{ 
  mp->error_count=0; mp->interaction=mp_batch_mode+c-'Q';
  mp_print(mp, "OK, entering ");
  switch (c) {
  case 'Q': mp_print(mp, "batchmode"); decr(mp->selector); break;
  case 'R': mp_print(mp, "nonstopmode"); break;
  case 'S': mp_print(mp, "scrollmode"); break;
  } /* there are no other cases */
  mp_print(mp, "..."); mp_print_ln(mp); update_terminal; return;
}

@ When the following code is executed, |buffer[(first+1)..(last-1)]| may
contain the material inserted by the user; otherwise another prompt will
be given. In order to understand this part of the program fully, you need
to be familiar with \MP's input stacks.

@<Introduce new material...@>=
{ 
  mp_begin_file_reading(mp); /* enter a new syntactic level for terminal input */
  if ( mp->last>mp->first+1 ) { 
    loc=(halfword)(mp->first+1); mp->buffer[mp->first]=xord(' ');
  } else { 
   prompt_input("insert>"); loc=(halfword)mp->first;
@.insert>@>
  };
  mp->first=mp->last+1; mp->cur_input.limit_field=(halfword)mp->last; return;
}

@ We allow deletion of up to 99 tokens at a time.

@<Delete |c-"0"| tokens...@>=
{ 
  s1=mp->cur_cmd; s2=mp->cur_mod; s3=mp->cur_sym; mp->OK_to_interrupt=false;
  if ( (mp->last>mp->first+1) && (mp->buffer[mp->first+1]>='0')&&(mp->buffer[mp->first+1]<='9') )
    c=xord(c*10+mp->buffer[mp->first+1]-'0'*11);
  else 
    c = (ASCII_code) (c - '0');
  while ( c>0 ) { 
    mp_get_next(mp); /* one-level recursive call of |error| is possible */
    @<Decrease the string reference count, if the current token is a string@>;
    c--;
  };
  mp->cur_cmd=s1; mp->cur_mod=s2; mp->cur_sym=s3; mp->OK_to_interrupt=true;
  help2("I have just deleted some text, as you asked.",
       "You can now delete more, or insert, or whatever.");
  mp_show_context(mp); 
  goto CONTINUE;
}

@ @<Print the help info...@>=
{ 
  if ( mp->use_err_help ) { 
    @<Print the string |err_help|, possibly on several lines@>;
    mp->use_err_help=false;
  } else { 
    if ( mp->help_ptr==0 ) {
      help2("Sorry, I don't know how to help in this situation.",
            "Maybe you should try asking a human?");
     }
    do { 
      decr(mp->help_ptr); mp_print(mp, mp->help_line[mp->help_ptr]); mp_print_ln(mp);
    } while (mp->help_ptr!=0);
  };
  help4("Sorry, I already gave what help I could...",
       "Maybe you should try asking a human?",
       "An error might have occurred before I noticed any problems.",
       "``If all else fails, read the instructions.''");
  goto CONTINUE;
}

@ @<Print the string |err_help|, possibly on several lines@>=
j=mp->str_start[mp->err_help];
while ( j<str_stop(mp->err_help) ) { 
  if ( mp->str_pool[j]!='%' ) mp_print_str(mp, mp->str_pool[j]);
  else if ( j+1==str_stop(mp->err_help) ) mp_print_ln(mp);
  else if ( mp->str_pool[j+1]!='%' ) mp_print_ln(mp);
  else  { j++; mp_print_char(mp, xord('%')); };
  j++;
}

@ @<Put help message on the transcript file@>=
if ( mp->interaction>mp_batch_mode ) decr(mp->selector); /* avoid terminal output */
if ( mp->use_err_help ) { 
  mp_print_nl(mp, "");
  @<Print the string |err_help|, possibly on several lines@>;
} else { 
  while ( mp->help_ptr>0 ){ 
    decr(mp->help_ptr); mp_print_nl(mp, mp->help_line[mp->help_ptr]);
  };
}
mp_print_ln(mp);
if ( mp->interaction>mp_batch_mode ) incr(mp->selector); /* re-enable terminal output */
mp_print_ln(mp)

@ In anomalous cases, the print selector might be in an unknown state;
the following subroutine is called to fix things just enough to keep
running a bit longer.

@c 
void mp_normalize_selector (MP mp) { 
  if ( mp->log_opened ) mp->selector=term_and_log;
  else mp->selector=term_only;
  if ( mp->job_name==NULL) mp_open_log_file(mp);
  if ( mp->interaction==mp_batch_mode ) decr(mp->selector);
}

@ The following procedure prints \MP's last words before dying.

@d succumb { if ( mp->interaction==mp_error_stop_mode )
    mp->interaction=mp_scroll_mode; /* no more interaction */
  if ( mp->log_opened ) mp_error(mp);
  mp->history=mp_fatal_error_stop; mp_jump_out(mp); /* irrecoverable error */
  }

@<Error hand...@>=
void mp_fatal_error (MP mp, const char *s) { /* prints |s|, and that's it */
  mp_normalize_selector(mp);
  print_err("Emergency stop"); help1(s); succumb;
@.Emergency stop@>
}

@ @<Exported function ...@>=
extern void mp_fatal_error (MP mp, const char *s);


@ Here is the most dreaded error message.

@<Error hand...@>=
void mp_overflow (MP mp, const char *s, integer n) { /* stop due to finiteness */
  char msg[256];
  mp_normalize_selector(mp);
  mp_snprintf(msg, 256, "MetaPost capacity exceeded, sorry [%s=%d]",s,(int)n);
@.MetaPost capacity exceeded ...@>
  print_err(msg);
  help2("If you really absolutely need more capacity,",
        "you can ask a wizard to enlarge me.");
  succumb;
}

@ @<Internal library declarations@>=
void mp_overflow (MP mp, const char *s, integer n);

@ The program might sometime run completely amok, at which point there is
no choice but to stop. If no previous error has been detected, that's bad
news; a message is printed that is really intended for the \MP\
maintenance person instead of the user (unless the user has been
particularly diabolical).  The index entries for `this can't happen' may
help to pinpoint the problem.
@^dry rot@>

@<Internal library ...@>=
void mp_confusion (MP mp, const char *s);

@ Consistency check violated; |s| tells where.
@<Error hand...@>=
void mp_confusion (MP mp, const char *s) {
  char msg[256];
  mp_normalize_selector(mp);
  if ( mp->history<mp_error_message_issued ) { 
    mp_snprintf(msg, 256, "This can't happen (%s)",s);
@.This can't happen@>
    print_err(msg);
    help1("I'm broken. Please show this to someone who can fix can fix");
  } else { 
    print_err("I can\'t go on meeting you like this");
@.I can't go on...@>
    help2("One of your faux pas seems to have wounded me deeply...",
          "in fact, I'm barely conscious. Please fix it and try again.");
  }
  succumb;
}

@ Users occasionally want to interrupt \MP\ while it's running.
If the runtime system allows this, one can implement
a routine that sets the global variable |interrupt| to some nonzero value
when such an interrupt is signaled. Otherwise there is probably at least
a way to make |interrupt| nonzero using the C debugger.
@^system dependencies@>
@^debugging@>

@d check_interrupt { if ( mp->interrupt!=0 )
   mp_pause_for_instructions(mp); }

@<Global...@>=
integer interrupt; /* should \MP\ pause for instructions? */
boolean OK_to_interrupt; /* should interrupts be observed? */
integer run_state; /* are we processing input ?*/
boolean finished; /* set true by |close_files_and_terminate| */

@ @<Allocate or ...@>=
mp->OK_to_interrupt=true;
mp->finished=false;

@ When an interrupt has been detected, the program goes into its
highest interaction level and lets the user have the full flexibility of
the |error| routine.  \MP\ checks for interrupts only at times when it is
safe to do this.

@c 
static void mp_pause_for_instructions (MP mp) { 
  if ( mp->OK_to_interrupt ) { 
    mp->interaction=mp_error_stop_mode;
    if ( (mp->selector==log_only)||(mp->selector==no_print) )
      incr(mp->selector);
    print_err("Interruption");
@.Interruption@>
    help3("You rang?",
         "Try to insert some instructions for me (e.g.,`I show x'),",
         "unless you just want to quit by typing `X'.");
    mp->deletions_allowed=false; mp_error(mp); mp->deletions_allowed=true;
    mp->interrupt=0;
  }
}

@ Many of \MP's error messages state that a missing token has been
inserted behind the scenes. We can save string space and program space
by putting this common code into a subroutine.

@c 
static void mp_missing_err (MP mp, const char *s) { 
  char msg[256];
  mp_snprintf(msg, 256, "Missing `%s' has been inserted", s);
@.Missing...inserted@>
  print_err(msg);
}

@* \[7] Arithmetic with scaled numbers.
The principal computations performed by \MP\ are done entirely in terms of
integers less than $2^{31}$ in magnitude; thus, the arithmetic specified in this
program can be carried out in exactly the same way on a wide variety of
computers, including some small ones.
@^small computers@>

But C does not rigidly define the |/| operation in the case of negative
dividends; for example, the result of |(-2*n-1) / 2| is |-(n+1)| on some
computers and |-n| on others (is this true ?).  There are two principal
types of arithmetic: ``translation-preserving,'' in which the identity
|(a+q*b)/b=(a/b)+q| is valid; and ``negation-preserving,'' in which
|(-a)/b=-(a/b)|. This leads to two \MP s, which can produce
different results, although the differences should be negligible when the
language is being used properly.  The \TeX\ processor has been defined
carefully so that both varieties of arithmetic will produce identical
output, but it would be too inefficient to constrain \MP\ in a similar way.

@d el_gordo   0x7fffffff /* $2^{31}-1$, the largest value that \MP\ likes */


@ One of \MP's most common operations is the calculation of
$\lfloor{a+b\over2}\rfloor$,
the midpoint of two given integers |a| and~|b|. The most decent way to do
this is to write `|(a+b)/2|'; but on many machines it is more efficient 
to calculate `|(a+b)>>1|'.

Therefore the midpoint operation will always be denoted by `|half(a+b)|'
in this program. If \MP\ is being implemented with languages that permit
binary shifting, the |half| macro should be changed to make this operation
as efficient as possible.  Since some systems have shift operators that can
only be trusted to work on positive numbers, there is also a macro |halfp|
that is used only when the quantity being halved is known to be positive
or zero.

@d half(A) ((A) / 2)
@d halfp(A) (integer)((unsigned)(A) >> 1)

@ A single computation might use several subroutine calls, and it is
desirable to avoid producing multiple error messages in case of arithmetic
overflow. So the routines below set the global variable |arith_error| to |true|
instead of reporting errors directly to the user.
@^overflow in arithmetic@>

@<Glob...@>=
boolean arith_error; /* has arithmetic overflow occurred recently? */

@ @<Allocate or ...@>=
mp->arith_error=false;

@ At crucial points the program will say |check_arith|, to test if
an arithmetic error has been detected.

@d check_arith { if ( mp->arith_error ) mp_clear_arith(mp); }

@c 
static void mp_clear_arith (MP mp) { 
  print_err("Arithmetic overflow");
@.Arithmetic overflow@>
  help4("Uh, oh. A little while ago one of the quantities that I was",
       "computing got too large, so I'm afraid your answers will be",
       "somewhat askew. You'll probably have to adopt different",
       "tactics next time. But I shall try to carry on anyway.");
  mp_error(mp); 
  mp->arith_error=false;
}

@ Addition is not always checked to make sure that it doesn't overflow,
but in places where overflow isn't too unlikely the |slow_add| routine
is used.

@c static integer mp_slow_add (MP mp,integer x, integer y) { 
  if ( x>=0 )  {
    if ( y<=el_gordo-x ) { 
      return x+y;
    } else  { 
      mp->arith_error=true; 
	  return el_gordo;
    }
  } else  if ( -y<=el_gordo+x ) {
    return x+y;
  } else { 
    mp->arith_error=true; 
	return -el_gordo;
  }
}

@ Fixed-point arithmetic is done on {\sl scaled integers\/} that are multiples
of $2^{-16}$. In other words, a binary point is assumed to be sixteen bit
positions from the right end of a binary computer word.

@d quarter_unit   040000 /* $2^{14}$, represents 0.250000 */
@d half_unit   0100000 /* $2^{15}$, represents 0.50000 */
@d three_quarter_unit   0140000 /* $3\cdot2^{14}$, represents 0.75000 */
@d unity   0200000 /* $2^{16}$, represents 1.00000 */
@d two   0400000 /* $2^{17}$, represents 2.00000 */
@d three   0600000 /* $2^{17}+2^{16}$, represents 3.00000 */

@<Types...@>=
typedef integer scaled; /* this type is used for scaled integers */

@ The following function is used to create a scaled integer from a given decimal
fraction $(.d_0d_1\ldots d_{k-1})$, where |0<=k<=17|. The digit $d_i$ is
given in |dig[i]|, and the calculation produces a correctly rounded result.

@c 
static scaled mp_round_decimals (MP mp,quarterword k) {
  /* converts a decimal fraction */
 unsigned a = 0; /* the accumulator */
 while ( k-->0 ) { 
    a=(a+(unsigned)mp->dig[k]*two) / 10;
  }
  return (scaled)halfp(a+1);
}

@ Conversely, here is a procedure analogous to |print_int|. If the output
of this procedure is subsequently read by \MP\ and converted by the
|round_decimals| routine above, it turns out that the original value will
be reproduced exactly. A decimal point is printed only if the value is
not an integer. If there is more than one way to print the result with
the optimum number of digits following the decimal point, the closest
possible value is given.

The invariant relation in the \&{repeat} loop is that a sequence of
decimal digits yet to be printed will yield the original number if and only if
they form a fraction~$f$ in the range $s-\delta\L10\cdot2^{16}f<s$.
We can stop if and only if $f=0$ satisfies this condition; the loop will
terminate before $s$ can possibly become zero.

@<Basic printing...@>=
void mp_print_scaled (MP mp,scaled s) { /* prints scaled real, rounded to five  digits */
  scaled delta; /* amount of allowable inaccuracy */
  if ( s<0 ) { 
	mp_print_char(mp, xord('-')); 
    negate(s); /* print the sign, if negative */
  }
  mp_print_int(mp, s / unity); /* print the integer part */
  s=10*(s % unity)+5;
  if ( s!=5 ) { 
    delta=10; 
    mp_print_char(mp, xord('.'));
    do {  
      if ( delta>unity )
        s=s+0100000-(delta / 2); /* round the final digit */
      mp_print_char(mp, xord('0'+(s / unity))); 
      s=10*(s % unity); 
      delta=delta*10;
    } while (s>delta);
  }
}

@ We often want to print two scaled quantities in parentheses,
separated by a comma.

@<Basic printing...@>=
void mp_print_two (MP mp,scaled x, scaled y) { /* prints `|(x,y)|' */
  mp_print_char(mp, xord('(')); 
  mp_print_scaled(mp, x); 
  mp_print_char(mp, xord(',')); 
  mp_print_scaled(mp, y);
  mp_print_char(mp, xord(')'));
}

@ The |scaled| quantities in \MP\ programs are generally supposed to be
less than $2^{12}$ in absolute value, so \MP\ does much of its internal
arithmetic with 28~significant bits of precision. A |fraction| denotes
a scaled integer whose binary point is assumed to be 28 bit positions
from the right.

@d fraction_half 01000000000 /* $2^{27}$, represents 0.50000000 */
@d fraction_one 02000000000 /* $2^{28}$, represents 1.00000000 */
@d fraction_two 04000000000 /* $2^{29}$, represents 2.00000000 */
@d fraction_three 06000000000 /* $3\cdot2^{28}$, represents 3.00000000 */
@d fraction_four 010000000000 /* $2^{30}$, represents 4.00000000 */

@<Types...@>=
typedef integer fraction; /* this type is used for scaled fractions */

@ In fact, the two sorts of scaling discussed above aren't quite
sufficient; \MP\ has yet another, used internally to keep track of angles
in units of $2^{-20}$ degrees.

@d forty_five_deg 0264000000 /* $45\cdot2^{20}$, represents $45^\circ$ */
@d ninety_deg 0550000000 /* $90\cdot2^{20}$, represents $90^\circ$ */
@d one_eighty_deg 01320000000 /* $180\cdot2^{20}$, represents $180^\circ$ */
@d three_sixty_deg 02640000000 /* $360\cdot2^{20}$, represents $360^\circ$ */

@<Types...@>=
typedef integer angle; /* this type is used for scaled angles */

@ The |make_fraction| routine produces the |fraction| equivalent of
|p/q|, given integers |p| and~|q|; it computes the integer
$f=\lfloor2^{28}p/q+{1\over2}\rfloor$, when $p$ and $q$ are
positive. If |p| and |q| are both of the same scaled type |t|,
the ``type relation'' |make_fraction(t,t)=fraction| is valid;
and it's also possible to use the subroutine ``backwards,'' using
the relation |make_fraction(t,fraction)=t| between scaled types.

If the result would have magnitude $2^{31}$ or more, |make_fraction|
sets |arith_error:=true|. Most of \MP's internal computations have
been designed to avoid this sort of error.

If this subroutine were programmed in assembly language on a typical
machine, we could simply compute |(@t$2^{28}$@>*p)div q|, since a
double-precision product can often be input to a fixed-point division
instruction. But when we are restricted to int-eger arithmetic it
is necessary either to resort to multiple-precision maneuvering
or to use a simple but slow iteration. The multiple-precision technique
would be about three times faster than the code adopted here, but it
would be comparatively long and tricky, involving about sixteen
additional multiplications and divisions.

This operation is part of \MP's ``inner loop''; indeed, it will
consume nearly 10\pct! of the running time (exclusive of input and output)
if the code below is left unchanged. A machine-dependent recoding
will therefore make \MP\ run faster. The present implementation
is highly portable, but slow; it avoids multiplication and division
except in the initial stage. System wizards should be careful to
replace it with a routine that is guaranteed to produce identical
results in all cases.
@^system dependencies@>

As noted below, a few more routines should also be replaced by machine-dependent
code, for efficiency. But when a procedure is not part of the ``inner loop,''
such changes aren't advisable; simplicity and robustness are
preferable to trickery, unless the cost is too high.
@^inner loop@>

@<Internal library declarations@>=
integer mp_take_scaled (MP mp,integer q, scaled f) ;

@ @<Declarations@>=
static fraction mp_make_fraction (MP mp,integer p, integer q);

@ If FIXPT is not defined, we need these preprocessor values

@d TWEXP31  2147483648.0
@d TWEXP28  268435456.0
@d TWEXP16 65536.0
@d TWEXP_16 (1.0/65536.0)
@d TWEXP_28 (1.0/268435456.0)


@c 
fraction mp_make_fraction (MP mp,integer p, integer q) {
  fraction i;
  if ( q==0 ) mp_confusion(mp, "/");
@:this can't happen /}{\quad \./@>
#ifdef FIXPT
{
  integer f; /* the fraction bits, with a leading 1 bit */
  integer n; /* the integer part of $\vert p/q\vert$ */
  boolean negative = false; /* should the result be negated? */
  if ( p<0 ) {
    negate(p); negative=true;
  }
  if ( q<0 ) { 
    negate(q); negative = ! negative;
  }
  n=p / q; p=p % q;
  if ( n>=8 ){ 
    mp->arith_error=true;
    i= ( negative ? -el_gordo : el_gordo);
  } else { 
    n=(n-1)*fraction_one;
    @<Compute $f=\lfloor 2^{28}(1+p/q)+{1\over2}\rfloor$@>;
    i = (negative ? (-(f+n)) : (f+n));
  }
}
#else /* FIXPT */
  {
    register double d;
	d = TWEXP28 * (double)p /(double)q;
	if ((p^q) >= 0) {
		d += 0.5;
		if (d>=TWEXP31) {mp->arith_error=true; return el_gordo;}
		i = (integer) d;
		if (d==(double)i && ( ((q>0 ? -q : q)&077777)
				* (((i&037777)<<1)-1) & 04000)!=0) --i;
	} else {
		d -= 0.5;
		if (d<= -TWEXP31) {mp->arith_error=true; return -el_gordo;}
		i = (integer) d;
		if (d==(double)i && ( ((q>0 ? q : -q)&077777)
				* (((i&037777)<<1)+1) & 04000)!=0) ++i;
	}
  }
#endif /* FIXPT */
  return i;
}

@ The |repeat| loop here preserves the following invariant relations
between |f|, |p|, and~|q|:
(i)~|0<=p<q|; (ii)~$fq+p=2^k(q+p_0)$, where $k$ is an integer and
$p_0$ is the original value of~$p$.

Notice that the computation specifies
|(p-q)+p| instead of |(p+p)-q|, because the latter could overflow.
Let us hope that optimizing compilers do not miss this point; a
special variable |be_careful| is used to emphasize the necessary
order of computation. Optimizing compilers should keep |be_careful|
in a register, not store it in memory.
@^inner loop@>

@<Compute $f=\lfloor 2^{28}(1+p/q)+{1\over2}\rfloor$@>=
{
  integer be_careful; /* disables certain compiler optimizations */
  f=1;
  do {  
    be_careful=p-q; p=be_careful+p;
    if ( p>=0 ) { 
      f=f+f+1;
    } else  { 
      f+=f; p=p+q;
    }
  } while (f<fraction_one);
  be_careful=p-q;
  if ( be_careful+p>=0 ) incr(f);
}

@ The dual of |make_fraction| is |take_fraction|, which multiplies a
given integer~|q| by a fraction~|f|. When the operands are positive, it
computes $p=\lfloor qf/2^{28}+{1\over2}\rfloor$, a symmetric function
of |q| and~|f|.

This routine is even more ``inner loopy'' than |make_fraction|;
the present implementation consumes almost 20\pct! of \MP's computation
time during typical jobs, so a machine-language substitute is advisable.
@^inner loop@> @^system dependencies@>

@<Internal library declarations@>=
integer mp_take_fraction (MP mp,integer q, fraction f) ;

@ @c 
#ifdef FIXPT
integer mp_take_fraction (MP mp,integer q, fraction f) {
  integer p; /* the fraction so far */
  boolean negative; /* should the result be negated? */
  integer n; /* additional multiple of $q$ */
  integer be_careful; /* disables certain compiler optimizations */
  @<Reduce to the case that |f>=0| and |q>=0|@>;
  if ( f<fraction_one ) { 
    n=0;
  } else { 
    n=f / fraction_one; f=f % fraction_one;
    if ( q<=el_gordo / n ) { 
      n=n*q ; 
    } else { 
      mp->arith_error=true; n=el_gordo;
    }
  }
  f=f+fraction_one;
  @<Compute $p=\lfloor qf/2^{28}+{1\over2}\rfloor-q$@>;
  be_careful=n-el_gordo;
  if ( be_careful+p>0 ){ 
    mp->arith_error=true; n=el_gordo-p;
  }
  if ( negative ) 
	return (-(n+p));
  else 
    return (n+p);
#else /* FIXPT */
integer mp_take_fraction (MP mp,integer p, fraction q) {
    register double d;
	register integer i;
	d = (double)p * (double)q * TWEXP_28;
	if ((p^q) >= 0) {
		d += 0.5;
		if (d>=TWEXP31) {
			if (d!=TWEXP31 || (((p&077777)*(q&077777))&040000)==0)
				mp->arith_error = true;
			return el_gordo;
		}
		i = (integer) d;
		if (d==(double)i && (((p&077777)*(q&077777))&040000)!=0) --i;
	} else {
		d -= 0.5;
		if (d<= -TWEXP31) {
			if (d!= -TWEXP31 || ((-(p&077777)*(q&077777))&040000)==0)
				mp->arith_error = true;
			return -el_gordo;
		}
		i = (integer) d;
		if (d==(double)i && ((-(p&077777)*(q&077777))&040000)!=0) ++i;
	}
	return i;
#endif /* FIXPT */
}

@ @<Reduce to the case that |f>=0| and |q>=0|@>=
if ( f>=0 ) {
  negative=false;
} else { 
  negate( f); negative=true;
}
if ( q<0 ) { 
  negate(q); negative=! negative;
}

@ The invariant relations in this case are (i)~$\lfloor(qf+p)/2^k\rfloor
=\lfloor qf_0/2^{28}+{1\over2}\rfloor$, where $k$ is an integer and
$f_0$ is the original value of~$f$; (ii)~$2^k\L f<2^{k+1}$.
@^inner loop@>

@<Compute $p=\lfloor qf/2^{28}+{1\over2}\rfloor-q$@>=
p=fraction_half; /* that's $2^{27}$; the invariants hold now with $k=28$ */
if ( q<fraction_four ) {
  do {  
    if ( odd(f) ) p=halfp(p+q); else p=halfp(p);
    f=halfp(f);
  } while (f!=1);
} else  {
  do {  
    if ( odd(f) ) p=p+halfp(q-p); else p=halfp(p);
    f=halfp(f);
  } while (f!=1);
}


@ When we want to multiply something by a |scaled| quantity, we use a scheme
analogous to |take_fraction| but with a different scaling.
Given positive operands, |take_scaled|
computes the quantity $p=\lfloor qf/2^{16}+{1\over2}\rfloor$.

Once again it is a good idea to use a machine-language replacement if
possible; otherwise |take_scaled| will use more than 2\pct! of the running time
when the Computer Modern fonts are being generated.
@^inner loop@>

@c 
#ifdef FIXPT
integer mp_take_scaled (MP mp,integer q, scaled f) {
  integer p; /* the fraction so far */
  boolean negative; /* should the result be negated? */
  integer n; /* additional multiple of $q$ */
  integer be_careful; /* disables certain compiler optimizations */
  @<Reduce to the case that |f>=0| and |q>=0|@>;
  if ( f<unity ) { 
    n=0;
  } else  { 
    n=f / unity; f=f % unity;
    if ( q<=el_gordo / n ) {
      n=n*q;
    } else  { 
      mp->arith_error=true; n=el_gordo;
    }
  }
  f=f+unity;
  @<Compute $p=\lfloor qf/2^{16}+{1\over2}\rfloor-q$@>;
  be_careful=n-el_gordo;
  if ( be_careful+p>0 ) { 
    mp->arith_error=true; n=el_gordo-p;
  }
  return ( negative ?(-(n+p)) :(n+p));
#else /* FIXPT */
integer mp_take_scaled (MP mp,integer p, scaled q) {
    register double d;
	register integer i;
	d = (double)p * (double)q * TWEXP_16;
	if ((p^q) >= 0) {
		d += 0.5;
		if (d>=TWEXP31) {
			if (d!=TWEXP31 || (((p&077777)*(q&077777))&040000)==0)
				mp->arith_error = true;
			return el_gordo;
		}
		i = (integer) d;
		if (d==(double)i && (((p&077777)*(q&077777))&040000)!=0) --i;
	} else {
		d -= 0.5;
		if (d<= -TWEXP31) {
			if (d!= -TWEXP31 || ((-(p&077777)*(q&077777))&040000)==0)
				mp->arith_error = true;
			return -el_gordo;
		}
		i = (integer) d;
		if (d==(double)i && ((-(p&077777)*(q&077777))&040000)!=0) ++i;
	}
	return i;
#endif /* FIXPT */
}

@ @<Compute $p=\lfloor qf/2^{16}+{1\over2}\rfloor-q$@>=
p=half_unit; /* that's $2^{15}$; the invariants hold now with $k=16$ */
@^inner loop@>
if ( q<fraction_four ) {
  do {  
    p = (odd(f) ? halfp(p+q) : halfp(p));
    f=halfp(f);
  } while (f!=1);
} else {
  do {  
    p = (odd(f) ? p+halfp(q-p) : halfp(p));
    f=halfp(f);
  } while (f!=1);
}

@ For completeness, there's also |make_scaled|, which computes a
quotient as a |scaled| number instead of as a |fraction|.
In other words, the result is $\lfloor2^{16}p/q+{1\over2}\rfloor$, if the
operands are positive. \ (This procedure is not used especially often,
so it is not part of \MP's inner loop.)

@<Internal library ...@>=
scaled mp_make_scaled (MP mp,integer p, integer q) ;

@ @c 
scaled mp_make_scaled (MP mp,integer p, integer q) {
  register integer i;
  if ( q==0 ) mp_confusion(mp, "/");
@:this can't happen /}{\quad \./@>
  {
#ifdef FIXPT 
    integer f; /* the fraction bits, with a leading 1 bit */
    integer n; /* the integer part of $\vert p/q\vert$ */
    boolean negative; /* should the result be negated? */
    integer be_careful; /* disables certain compiler optimizations */
    if ( p>=0 ) negative=false;
    else  { negate(p); negative=true; };
    if ( q<0 ) { 
      negate(q); negative=! negative;
    }
    n=p / q; p=p % q;
    if ( n>=0100000 ) { 
      mp->arith_error=true;
      return (negative ? (-el_gordo) : el_gordo);
    } else  { 
      n=(n-1)*unity;
      @<Compute $f=\lfloor 2^{16}(1+p/q)+{1\over2}\rfloor$@>;
      i = (negative ? (-(f+n)) :(f+n));
    }
#else /* FIXPT */
    register double d;
	d = TWEXP16 * (double)p /(double)q;
	if ((p^q) >= 0) {
		d += 0.5;
		if (d>=TWEXP31) {mp->arith_error=true; return el_gordo;}
		i = (integer) d;
		if (d==(double)i && ( ((q>0 ? -q : q)&077777)
				* (((i&037777)<<1)-1) & 04000)!=0) --i;
	} else {
		d -= 0.5;
		if (d<= -TWEXP31) {mp->arith_error=true; return -el_gordo;}
		i = (integer) d;
		if (d==(double)i && ( ((q>0 ? q : -q)&077777)
				* (((i&037777)<<1)+1) & 04000)!=0) ++i;
	}
#endif /* FIXPT */
  }
  return i;
}

@ @<Compute $f=\lfloor 2^{16}(1+p/q)+{1\over2}\rfloor$@>=
f=1;
do {  
  be_careful=p-q; p=be_careful+p;
  if ( p>=0 ) f=f+f+1;
  else  { f+=f; p=p+q; };
} while (f<unity);
be_careful=p-q;
if ( be_careful+p>=0 ) incr(f)

@ Here is a typical example of how the routines above can be used.
It computes the function
$${1\over3\tau}f(\theta,\phi)=
{\tau^{-1}\bigl(2+\sqrt2\,(\sin\theta-{1\over16}\sin\phi)
 (\sin\phi-{1\over16}\sin\theta)(\cos\theta-\cos\phi)\bigr)\over
3\,\bigl(1+{1\over2}(\sqrt5-1)\cos\theta+{1\over2}(3-\sqrt5\,)\cos\phi\bigr)},$$
where $\tau$ is a |scaled| ``tension'' parameter. This is \MP's magic
fudge factor for placing the first control point of a curve that starts
at an angle $\theta$ and ends at an angle $\phi$ from the straight path.
(Actually, if the stated quantity exceeds 4, \MP\ reduces it to~4.)

The trigonometric quantity to be multiplied by $\sqrt2$ is less than $\sqrt2$.
(It's a sum of eight terms whose absolute values can be bounded using
relations such as $\sin\theta\cos\theta\L{1\over2}$.) Thus the numerator
is positive; and since the tension $\tau$ is constrained to be at least
$3\over4$, the numerator is less than $16\over3$. The denominator is
nonnegative and at most~6.  Hence the fixed-point calculations below
are guaranteed to stay within the bounds of a 32-bit computer word.

The angles $\theta$ and $\phi$ are given implicitly in terms of |fraction|
arguments |st|, |ct|, |sf|, and |cf|, representing $\sin\theta$, $\cos\theta$,
$\sin\phi$, and $\cos\phi$, respectively.

@c 
static fraction mp_velocity (MP mp,fraction st, fraction ct, fraction sf,
                      fraction cf, scaled t) {
  integer acc,num,denom; /* registers for intermediate calculations */
  acc=mp_take_fraction(mp, st-(sf / 16), sf-(st / 16));
  acc=mp_take_fraction(mp, acc,ct-cf);
  num=fraction_two+mp_take_fraction(mp, acc,379625062);
                   /* $2^{28}\sqrt2\approx379625062.497$ */
  denom=fraction_three+mp_take_fraction(mp, ct,497706707)+mp_take_fraction(mp, cf,307599661);
                      /* $3\cdot2^{27}\cdot(\sqrt5-1)\approx497706706.78$ and
                         $3\cdot2^{27}\cdot(3-\sqrt5\,)\approx307599661.22$ */
  if ( t!=unity ) num=mp_make_scaled(mp, num,t);
  /* |make_scaled(fraction,scaled)=fraction| */
  if ( num / 4>=denom ) 
    return fraction_four;
  else 
    return mp_make_fraction(mp, num, denom);
}

@ The following somewhat different subroutine tests rigorously if $ab$ is
greater than, equal to, or less than~$cd$,
given integers $(a,b,c,d)$. In most cases a quick decision is reached.
The result is $+1$, 0, or~$-1$ in the three respective cases.

@d mp_ab_vs_cd(M,A,B,C,D) mp_do_ab_vs_cd(A,B,C,D)

@c 
static integer mp_do_ab_vs_cd (integer a,integer b, integer c, integer d) {
  integer q,r; /* temporary registers */
  @<Reduce to the case that |a,c>=0|, |b,d>0|@>;
  while (1) { 
    q = a / d; r = c / b;
    if ( q!=r )
      return ( q>r ? 1 : -1);
    q = a % d; r = c % b;
    if ( r==0 )
      return (q ? 1 : 0);
    if ( q==0 ) return -1;
    a=b; b=q; c=d; d=r;
  } /* now |a>d>0| and |c>b>0| */
}

@ @<Reduce to the case that |a...@>=
if ( a<0 ) { negate(a); negate(b);  };
if ( c<0 ) { negate(c); negate(d);  };
if ( d<=0 ) { 
  if ( b>=0 ) {
    if ( (a==0||b==0)&&(c==0||d==0) ) return 0;
    else return 1;
  }
  if ( d==0 )
    return ( a==0 ? 0 : -1);
  q=a; a=c; c=q; q=-b; b=-d; d=q;
} else if ( b<=0 ) { 
  if ( b<0 ) if ( a>0 ) return -1;
  return (c==0 ? 0 : -1);
}

@ We conclude this set of elementary routines with some simple rounding
and truncation operations.

@<Internal library declarations@>=
#define mp_floor_scaled(M,i) ((i)&(-65536))

#define mp_round_unscaled(M,x) (x>=0100000 ? 1+((x-0100000) / 0200000) \
  : ( x>=-0100000 ? 0 : -(1+((-(x+1)-0100000) / 0200000))))

#define mp_round_fraction(M,x) (x>=2048 ? 1+((x-2048) / 4096) \
  : ( x>=-2048 ? 0 : -(1+((-(x+1)-2048) / 4096))))


@* \[8] Algebraic and transcendental functions.
\MP\ computes all of the necessary special functions from scratch, without
relying on |real| arithmetic or system subroutines for sines, cosines, etc.

@ To get the square root of a |scaled| number |x|, we want to calculate
$s=\lfloor 2^8\!\sqrt x +{1\over2}\rfloor$. If $x>0$, this is the unique
integer such that $2^{16}x-s\L s^2<2^{16}x+s$. The following subroutine
determines $s$ by an iterative method that maintains the invariant
relations $x=2^{46-2k}x_0\bmod 2^{30}$, $0<y=\lfloor 2^{16-2k}x_0\rfloor
-s^2+s\L q=2s$, where $x_0$ is the initial value of $x$. The value of~$y$
might, however, be zero at the start of the first iteration.

@<Declarations@>=
static scaled mp_square_rt (MP mp,scaled x) ;

@ @c 
scaled mp_square_rt (MP mp,scaled x) {
  quarterword k; /* iteration control counter */
  integer y; /* register for intermediate calculations */
  integer q; /* register for intermediate calculations */
  if ( x<=0 ) { 
    @<Handle square root of zero or negative argument@>;
  } else { 
    k=23; q=2;
    while ( x<fraction_two ) { /* i.e., |while x<@t$2^{29}$@>|\unskip */
      k--; x=x+x+x+x;
    }
    if ( x<fraction_four ) y=0;
    else  { x=x-fraction_four; y=1; };
    do {  
      @<Decrease |k| by 1, maintaining the invariant
      relations between |x|, |y|, and~|q|@>;
    } while (k!=0);
    return (scaled)(halfp(q));
  }
}

@ @<Handle square root of zero...@>=
{ 
  if ( x<0 ) { 
    print_err("Square root of ");
@.Square root...replaced by 0@>
    mp_print_scaled(mp, x); mp_print(mp, " has been replaced by 0");
    help2("Since I don't take square roots of negative numbers,",
          "I'm zeroing this one. Proceed, with fingers crossed.");
    mp_error(mp);
  };
  return 0;
}

@ @<Decrease |k| by 1, maintaining...@>=
x+=x; y+=y;
if ( x>=fraction_four ) { /* note that |fraction_four=@t$2^{30}$@>| */
  x=x-fraction_four; y++;
};
x+=x; y=y+y-q; q+=q;
if ( x>=fraction_four ) { x=x-fraction_four; y++; };
if ( y>(int)q ){ y-=q; q+=2; }
else if ( y<=0 )  { q-=2; y+=q;  };
k--

@ Pythagorean addition $\psqrt{a^2+b^2}$ is implemented by an elegant
iterative scheme due to Cleve Moler and Donald Morrison [{\sl IBM Journal
@^Moler, Cleve Barry@>
@^Morrison, Donald Ross@>
of Research and Development\/ \bf27} (1983), 577--581]. It modifies |a| and~|b|
in such a way that their Pythagorean sum remains invariant, while the
smaller argument decreases.

@<Internal library ...@>=
integer mp_pyth_add (MP mp,integer a, integer b);


@ @c 
integer mp_pyth_add (MP mp,integer a, integer b) {
  fraction r; /* register used to transform |a| and |b| */
  boolean big; /* is the result dangerously near $2^{31}$? */
  a=abs(a); b=abs(b);
  if ( a<b ) { r=b; b=a; a=r; }; /* now |0<=b<=a| */
  if ( b>0 ) {
    if ( a<fraction_two ) {
      big=false;
    } else { 
      a=a / 4; b=b / 4; big=true;
    }; /* we reduced the precision to avoid arithmetic overflow */
    @<Replace |a| by an approximation to $\psqrt{a^2+b^2}$@>;
    if ( big ) {
      if ( a<fraction_two ) {
        a=a+a+a+a;
      } else  { 
        mp->arith_error=true; a=el_gordo;
      };
    }
  }
  return a;
}

@ The key idea here is to reflect the vector $(a,b)$ about the
line through $(a,b/2)$.

@<Replace |a| by an approximation to $\psqrt{a^2+b^2}$@>=
while (1) {  
  r=mp_make_fraction(mp, b,a);
  r=mp_take_fraction(mp, r,r); /* now $r\approx b^2/a^2$ */
  if ( r==0 ) break;
  r=mp_make_fraction(mp, r,fraction_four+r);
  a=a+mp_take_fraction(mp, a+a,r); b=mp_take_fraction(mp, b,r);
}


@ Here is a similar algorithm for $\psqrt{a^2-b^2}$.
It converges slowly when $b$ is near $a$, but otherwise it works fine.

@c 
static integer mp_pyth_sub (MP mp,integer a, integer b) {
  fraction r; /* register used to transform |a| and |b| */
  boolean big; /* is the input dangerously near $2^{31}$? */
  a=abs(a); b=abs(b);
  if ( a<=b ) {
    @<Handle erroneous |pyth_sub| and set |a:=0|@>;
  } else { 
    if ( a<fraction_four ) {
      big=false;
    } else  { 
      a=(integer)halfp(a); b=(integer)halfp(b); big=true;
    }
    @<Replace |a| by an approximation to $\psqrt{a^2-b^2}$@>;
    if ( big ) double(a);
  }
  return a;
}

@ @<Replace |a| by an approximation to $\psqrt{a^2-b^2}$@>=
while (1) { 
  r=mp_make_fraction(mp, b,a);
  r=mp_take_fraction(mp, r,r); /* now $r\approx b^2/a^2$ */
  if ( r==0 ) break;
  r=mp_make_fraction(mp, r,fraction_four-r);
  a=a-mp_take_fraction(mp, a+a,r); b=mp_take_fraction(mp, b,r);
}

@ @<Handle erroneous |pyth_sub| and set |a:=0|@>=
{ 
  if ( a<b ){ 
    print_err("Pythagorean subtraction "); mp_print_scaled(mp, a);
    mp_print(mp, "+-+"); mp_print_scaled(mp, b); 
    mp_print(mp, " has been replaced by 0");
@.Pythagorean...@>
    help2("Since I don't take square roots of negative numbers,",
          "I'm zeroing this one. Proceed, with fingers crossed.");
    mp_error(mp);
  }
  a=0;
}

@ The subroutines for logarithm and exponential involve two tables.
The first is simple: |two_to_the[k]| equals $2^k$. The second involves
a bit more calculation, which the author claims to have done correctly:
|spec_log[k]| is $2^{27}$ times $\ln\bigl(1/(1-2^{-k})\bigr)=
2^{-k}+{1\over2}2^{-2k}+{1\over3}2^{-3k}+\cdots\,$, rounded to the
nearest integer.

@d two_to_the(A) (1<<(unsigned)(A))

@<Declarations@>=
static const integer spec_log[29] = { 0, /* special logarithms */
93032640, 38612034, 17922280, 8662214, 4261238, 2113709,
1052693, 525315, 262400, 131136, 65552, 32772, 16385,
8192, 4096, 2048, 1024, 512, 256, 128, 64, 32, 16, 8, 4, 2, 1, 1 };

@ @<Local variables for initialization@>=
integer k; /* all-purpose loop index */


@ Here is the routine that calculates $2^8$ times the natural logarithm
of a |scaled| quantity; it is an integer approximation to $2^{24}\ln(x/2^{16})$,
when |x| is a given positive integer.

The method is based on exercise 1.2.2--25 in {\sl The Art of Computer
Programming\/}: During the main iteration we have $1\L 2^{-30}x<1/(1-2^{1-k})$,
and the logarithm of $2^{30}x$ remains to be added to an accumulator
register called~$y$. Three auxiliary bits of accuracy are retained in~$y$
during the calculation, and sixteen auxiliary bits to extend |y| are
kept in~|z| during the initial argument reduction. (We add
$100\cdot2^{16}=6553600$ to~|z| and subtract 100 from~|y| so that |z| will
not become negative; also, the actual amount subtracted from~|y| is~96,
not~100, because we want to add~4 for rounding before the final division by~8.)

@c 
static scaled mp_m_log (MP mp,scaled x) {
  integer y,z; /* auxiliary registers */
  integer k; /* iteration counter */
  if ( x<=0 ) {
     @<Handle non-positive logarithm@>;
  } else  { 
    y=1302456956+4-100; /* $14\times2^{27}\ln2\approx1302456956.421063$ */
    z=27595+6553600; /* and $2^{16}\times .421063\approx 27595$ */
    while ( x<fraction_four ) {
       double(x); y-=93032639; z-=48782;
    } /* $2^{27}\ln2\approx 93032639.74436163$ and $2^{16}\times.74436163\approx 48782$ */
    y=y+(z / unity); k=2;
    while ( x>fraction_four+4 ) {
      @<Increase |k| until |x| can be multiplied by a
        factor of $2^{-k}$, and adjust $y$ accordingly@>;
    }
    return (y / 8);
  }
}

@ @<Increase |k| until |x| can...@>=
{ 
  z=((x-1) / two_to_the(k))+1; /* $z=\lceil x/2^k\rceil$ */
  while ( x<fraction_four+z ) { z=halfp(z+1); k++; };
  y+=spec_log[k]; x-=z;
}

@ @<Handle non-positive logarithm@>=
{ 
  print_err("Logarithm of ");
@.Logarithm...replaced by 0@>
  mp_print_scaled(mp, x); mp_print(mp, " has been replaced by 0");
  help2("Since I don't take logs of non-positive numbers,",
        "I'm zeroing this one. Proceed, with fingers crossed.");
  mp_error(mp); 
  return 0;
}

@ Conversely, the exponential routine calculates $\exp(x/2^8)$,
when |x| is |scaled|. The result is an integer approximation to
$2^{16}\exp(x/2^{24})$, when |x| is regarded as an integer.

@c 
static scaled mp_m_exp (MP mp,scaled x) {
  quarterword k; /* loop control index */
  integer y,z; /* auxiliary registers */
  if ( x>174436200 ) {
    /* $2^{24}\ln((2^{31}-1)/2^{16})\approx 174436199.51$ */
    mp->arith_error=true; 
    return el_gordo;
  } else if ( x<-197694359 ) {
	/* $2^{24}\ln(2^{-1}/2^{16})\approx-197694359.45$ */
    return 0;
  } else { 
    if ( x<=0 ) { 
       z=-8*x; y=04000000; /* $y=2^{20}$ */
    } else { 
      if ( x<=127919879 ) { 
        z=1023359037-8*x;
        /* $2^{27}\ln((2^{31}-1)/2^{20})\approx 1023359037.125$ */
      } else {
       z=8*(174436200-x); /* |z| is always nonnegative */
      }
      y=el_gordo;
    };
    @<Multiply |y| by $\exp(-z/2^{27})$@>;
    if ( x<=127919879 ) 
       return ((y+8) / 16);
     else 
       return y;
  }
}

@ The idea here is that subtracting |spec_log[k]| from |z| corresponds
to multiplying |y| by $1-2^{-k}$.

A subtle point (which had to be checked) was that if $x=127919879$, the
value of~|y| will decrease so that |y+8| doesn't overflow. In fact,
$z$ will be 5 in this case, and |y| will decrease by~64 when |k=25|
and by~16 when |k=27|.

@<Multiply |y| by...@>=
k=1;
while ( z>0 ) { 
  while ( z>=spec_log[k] ) { 
    z-=spec_log[k];
    y=y-1-((y-two_to_the(k-1)) / two_to_the(k));
  }
  k++;
}

@ The trigonometric subroutines use an auxiliary table such that
|spec_atan[k]| contains an approximation to the |angle| whose tangent
is~$1/2^k$. $\arctan2^{-k}$ times $2^{20}\cdot180/\pi$ 

@<Declarations@>=
static const angle spec_atan[27] = { 0, 27855475, 14718068, 7471121, 3750058, 
1876857, 938658, 469357, 234682, 117342, 58671, 29335, 14668, 7334, 3667, 
1833, 917, 458, 229, 115, 57, 29, 14, 7, 4, 2, 1 };

@ Given integers |x| and |y|, not both zero, the |n_arg| function
returns the |angle| whose tangent points in the direction $(x,y)$.
This subroutine first determines the correct octant, then solves the
problem for |0<=y<=x|, then converts the result appropriately to
return an answer in the range |-one_eighty_deg<=@t$\theta$@><=one_eighty_deg|.
(The answer is |+one_eighty_deg| if |y=0| and |x<0|, but an answer of
|-one_eighty_deg| is possible if, for example, |y=-1| and $x=-2^{30}$.)

The octants are represented in a ``Gray code,'' since that turns out
to be computationally simplest.

@d negate_x 1
@d negate_y 2
@d switch_x_and_y 4
@d first_octant 1
@d second_octant (first_octant+switch_x_and_y)
@d third_octant (first_octant+switch_x_and_y+negate_x)
@d fourth_octant (first_octant+negate_x)
@d fifth_octant (first_octant+negate_x+negate_y)
@d sixth_octant (first_octant+switch_x_and_y+negate_x+negate_y)
@d seventh_octant (first_octant+switch_x_and_y+negate_y)
@d eighth_octant (first_octant+negate_y)

@c 
static angle mp_n_arg (MP mp,integer x, integer y) {
  angle z; /* auxiliary register */
  integer t; /* temporary storage */
  quarterword k; /* loop counter */
  int octant; /* octant code */
  if ( x>=0 ) {
    octant=first_octant;
  } else { 
    negate(x); octant=first_octant+negate_x;
  }
  if ( y<0 ) { 
    negate(y); octant=octant+negate_y;
  }
  if ( x<y ) { 
    t=y; y=x; x=t; octant=octant+switch_x_and_y;
  }
  if ( x==0 ) { 
    @<Handle undefined arg@>; 
  } else { 
    @<Set variable |z| to the arg of $(x,y)$@>;
    @<Return an appropriate answer based on |z| and |octant|@>;
  }
}

@ @<Handle undefined arg@>=
{ 
  print_err("angle(0,0) is taken as zero");
@.angle(0,0)...zero@>
  help2("The `angle' between two identical points is undefined.",
        "I'm zeroing this one. Proceed, with fingers crossed.");
  mp_error(mp); 
  return 0;
}

@ @<Return an appropriate answer...@>=
switch (octant) {
case first_octant: return z;
case second_octant: return (ninety_deg-z);
case third_octant: return (ninety_deg+z);
case fourth_octant: return (one_eighty_deg-z);
case fifth_octant: return (z-one_eighty_deg);
case sixth_octant: return (-z-ninety_deg);
case seventh_octant: return (z-ninety_deg);
case eighth_octant: return (-z);
}; /* there are no other cases */
return 0

@ At this point we have |x>=y>=0|, and |x>0|. The numbers are scaled up
or down until $2^{28}\L x<2^{29}$, so that accurate fixed-point calculations
will be made.

@<Set variable |z| to the arg...@>=
while ( x>=fraction_two ) { 
  x=halfp(x); y=halfp(y);
}
z=0;
if ( y>0 ) { 
 while ( x<fraction_one ) { 
    x+=x; y+=y; 
 };
 @<Increase |z| to the arg of $(x,y)$@>;
}

@ During the calculations of this section, variables |x| and~|y|
represent actual coordinates $(x,2^{-k}y)$. We will maintain the
condition |x>=y|, so that the tangent will be at most $2^{-k}$.
If $x<2y$, the tangent is greater than $2^{-k-1}$. The transformation
$(a,b)\mapsto(a+b\tan\phi,b-a\tan\phi)$ replaces $(a,b)$ by
coordinates whose angle has decreased by~$\phi$; in the special case
$a=x$, $b=2^{-k}y$, and $\tan\phi=2^{-k-1}$, this operation reduces
to the particularly simple iteration shown here. [Cf.~John E. Meggitt,
@^Meggitt, John E.@>
{\sl IBM Journal of Research and Development\/ \bf6} (1962), 210--226.]

The initial value of |x| will be multiplied by at most
$(1+{1\over2})(1+{1\over8})(1+{1\over32})\cdots\approx 1.7584$; hence
there is no chance of integer overflow.

@<Increase |z|...@>=
k=0;
do {  
  y+=y; k++;
  if ( y>x ){ 
    z=z+spec_atan[k]; t=x; x=x+(y / two_to_the(k+k)); y=y-t;
  };
} while (k!=15);
do {  
  y+=y; k++;
  if ( y>x ) { z=z+spec_atan[k]; y=y-x; };
} while (k!=26)

@ Conversely, the |n_sin_cos| routine takes an |angle| and produces the sine
and cosine of that angle. The results of this routine are
stored in global integer variables |n_sin| and |n_cos|.

@<Glob...@>=
fraction n_sin;fraction n_cos; /* results computed by |n_sin_cos| */

@ Given an integer |z| that is $2^{20}$ times an angle $\theta$ in degrees,
the purpose of |n_sin_cos(z)| is to set
|x=@t$r\cos\theta$@>| and |y=@t$r\sin\theta$@>| (approximately),
for some rather large number~|r|. The maximum of |x| and |y|
will be between $2^{28}$ and $2^{30}$, so that there will be hardly
any loss of accuracy. Then |x| and~|y| are divided by~|r|.

@c 
static void mp_n_sin_cos (MP mp,angle z) { /* computes a multiple of the sine
                                       and cosine */ 
  quarterword k; /* loop control variable */
  int q; /* specifies the quadrant */
  fraction r; /* magnitude of |(x,y)| */
  integer x,y,t; /* temporary registers */
  while ( z<0 ) z=z+three_sixty_deg;
  z=z % three_sixty_deg; /* now |0<=z<three_sixty_deg| */
  q=z / forty_five_deg; z=z % forty_five_deg;
  x=fraction_one; y=x;
  if ( ! odd(q) ) z=forty_five_deg-z;
  @<Subtract angle |z| from |(x,y)|@>;
  @<Convert |(x,y)| to the octant determined by~|q|@>;
  r=mp_pyth_add(mp, x,y); 
  mp->n_cos=mp_make_fraction(mp, x,r); 
  mp->n_sin=mp_make_fraction(mp, y,r);
}

@ In this case the octants are numbered sequentially.

@<Convert |(x,...@>=
switch (q) {
case 0: break;
case 1: t=x; x=y; y=t; break;
case 2: t=x; x=-y; y=t; break;
case 3: negate(x); break;
case 4: negate(x); negate(y); break;
case 5: t=x; x=-y; y=-t; break;
case 6: t=x; x=y; y=-t; break;
case 7: negate(y); break;
} /* there are no other cases */

@ The main iteration of |n_sin_cos| is similar to that of |n_arg| but
applied in reverse. The values of |spec_atan[k]| decrease slowly enough
that this loop is guaranteed to terminate before the (nonexistent) value
|spec_atan[27]| would be required.

@<Subtract angle |z|...@>=
k=1;
while ( z>0 ){ 
  if ( z>=spec_atan[k] ) { 
    z=z-spec_atan[k]; t=x;
    x=t+y / two_to_the(k);
    y=y-t / two_to_the(k);
  }
  k++;
}
if ( y<0 ) y=0 /* this precaution may never be needed */

@ And now let's complete our collection of numeric utility routines
by considering random number generation.
\MP\ generates pseudo-random numbers with the additive scheme recommended
in Section 3.6 of {\sl The Art of Computer Programming}; however, the
results are random fractions between 0 and |fraction_one-1|, inclusive.

There's an auxiliary array |randoms| that contains 55 pseudo-random
fractions. Using the recurrence $x_n=(x_{n-55}-x_{n-31})\bmod 2^{28}$,
we generate batches of 55 new $x_n$'s at a time by calling |new_randoms|.
The global variable |j_random| tells which element has most recently
been consumed.
The global variable |random_seed| was introduced in version 0.9,
for the sole reason of stressing the fact that the initial value of the
random seed is system-dependant. The initialization code below will initialize
this variable to |(internal[mp_time] div unity)+internal[mp_day]|, but this 
is not good enough on modern fast machines that are capable of running
multiple MetaPost processes within the same second.
@^system dependencies@>

@<Glob...@>=
fraction randoms[55]; /* the last 55 random values generated */
int j_random; /* the number of unused |randoms| */

@ @<Option variables@>=
int random_seed; /* the default random seed */

@ @<Allocate or initialize ...@>=
mp->random_seed = (scaled)opt->random_seed;

@ To consume a random fraction, the program below will say `|next_random|'
and then it will fetch |randoms[j_random]|.

@d next_random { if ( mp->j_random==0 ) mp_new_randoms(mp);
  else decr(mp->j_random); }

@c 
static void mp_new_randoms (MP mp) {
  int k; /* index into |randoms| */
  fraction x; /* accumulator */
  for (k=0;k<=23;k++) { 
   x=mp->randoms[k]-mp->randoms[k+31];
    if ( x<0 ) x=x+fraction_one;
    mp->randoms[k]=x;
  }
  for (k=24;k<= 54;k++){ 
    x=mp->randoms[k]-mp->randoms[k-24];
    if ( x<0 ) x=x+fraction_one;
    mp->randoms[k]=x;
  }
  mp->j_random=54;
}

@ @<Declarations@>=
static void mp_init_randoms (MP mp,scaled seed);

@ To initialize the |randoms| table, we call the following routine.

@c 
void mp_init_randoms (MP mp,scaled seed) {
  fraction j,jj,k; /* more or less random integers */
  int i; /* index into |randoms| */
  j=abs(seed);
  while ( j>=fraction_one ) j=halfp(j);
  k=1;
  for (i=0;i<=54;i++ ){ 
    jj=k; k=j-k; j=jj;
    if ( k<0 ) k=k+fraction_one;
    mp->randoms[(i*21)% 55]=j;
  }
  mp_new_randoms(mp); 
  mp_new_randoms(mp); 
  mp_new_randoms(mp); /* ``warm up'' the array */
}

@ To produce a uniform random number in the range |0<=u<x| or |0>=u>x|
or |0=u=x|, given a |scaled| value~|x|, we proceed as shown here.

Note that the call of |take_fraction| will produce the values 0 and~|x|
with about half the probability that it will produce any other particular
values between 0 and~|x|, because it rounds its answers.

@c 
static scaled mp_unif_rand (MP mp,scaled x) {
  scaled y; /* trial value */
  next_random; y=mp_take_fraction(mp, abs(x),mp->randoms[mp->j_random]);
  if ( y==abs(x) ) return 0;
  else if ( x>0 ) return y;
  else return (-y);
}

@ Finally, a normal deviate with mean zero and unit standard deviation
can readily be obtained with the ratio method (Algorithm 3.4.1R in
{\sl The Art of Computer Programming\/}).

@c 
static scaled mp_norm_rand (MP mp) {
  integer x,u,l; /* what the book would call $2^{16}X$, $2^{28}U$, and $-2^{24}\ln U$ */
  do { 
    do {  
      next_random;
      x=mp_take_fraction(mp, 112429,mp->randoms[mp->j_random]-fraction_half);
      /* $2^{16}\sqrt{8/e}\approx 112428.82793$ */
      next_random; u=mp->randoms[mp->j_random];
    } while (abs(x)>=u);
    x=mp_make_fraction(mp, x,u);
    l=139548960-mp_m_log(mp, u); /* $2^{24}\cdot12\ln2\approx139548959.6165$ */
  } while (mp_ab_vs_cd(mp, 1024,l,x,x)<0);
  return x;
}

@* \[9] Packed data.
In order to make efficient use of storage space, \MP\ bases its major data
structures on a |memory_word|, which contains either a (signed) integer,
possibly scaled, or a small number of fields that are one half or one
quarter of the size used for storing integers.

If |x| is a variable of type |memory_word|, it contains up to four
fields that can be referred to as follows:
$$\vbox{\halign{\hfil#&#\hfil&#\hfil\cr
|x|&.|int|&(an |integer|)\cr
|x|&.|sc|\qquad&(a |scaled| integer)\cr
|x.hh.lh|, |x.hh|&.|rh|&(two halfword fields)\cr
|x.hh.b0|, |x.hh.b1|, |x.hh|&.|rh|&(two quarterword fields, one halfword
  field)\cr
|x.qqqq.b0|, |x.qqqq.b1|, |x.qqqq|&.|b2|, |x.qqqq.b3|\hskip-100pt
  &\qquad\qquad\qquad(four quarterword fields)\cr}}$$
This is somewhat cumbersome to write, and not very readable either, but
macros will be used to make the notation shorter and more transparent.
The code below gives a formal definition of |memory_word| and
its subsidiary types, using packed variant records. \MP\ makes no
assumptions about the relative positions of the fields within a word.

@d max_quarterword 0x3FFF /* largest allowable value in a |quarterword| */
@d max_halfword 0xFFFFFFF /* largest allowable value in a |halfword| */

@ Here are the inequalities that the quarterword and halfword values
must satisfy (or rather, the inequalities that they mustn't satisfy):

@<Check the ``constant''...@>=
if (mp->ini_version) {
  if ( mp->mem_max!=mp->mem_top ) mp->bad=8;
} else {
  if ( mp->mem_max<mp->mem_top ) mp->bad=8;
}
if ( mp->mem_max>=max_halfword ) mp->bad=12;
if ( mp->max_strings>max_halfword ) mp->bad=13;

@ The macros |qi| and |qo| are used for input to and output 
from quarterwords. These are legacy macros.
@^system dependencies@>

@d qo(A) (A) /* to read eight bits from a quarterword */
@d qi(A) (quarterword)(A) /* to store eight bits in a quarterword */

@ The reader should study the following definitions closely:
@^system dependencies@>

@d sc cint /* |scaled| data is equivalent to |integer| */

@<Types...@>=
typedef short quarterword; /* 1/4 of a word */
typedef int halfword; /* 1/2 of a word */
typedef union {
  struct {
    halfword RH, LH;
  } v;
  struct { /* Make B0,B1 overlap the most significant bytes of LH.  */
    halfword junk;
    quarterword B0, B1;
  } u;
} two_halves;
typedef struct {
  struct {
    quarterword B2, B3, B0, B1;
  } u;
} four_quarters;
typedef union {
  two_halves hh;
  integer cint;
  four_quarters qqqq;
} memory_word;
#define	b0 u.B0
#define	b1 u.B1
#define	b2 u.B2
#define	b3 u.B3
#define rh v.RH
#define lh v.LH

@ When debugging, we may want to print a |memory_word| without knowing
what type it is; so we print it in all modes.
@^debugging@>

@c 
#if 0
void mp_print_word (MP mp,memory_word w) {
  /* prints |w| in all ways */
  mp_print_int(mp, w.cint); mp_print_char(mp, xord(' '));
  mp_print_scaled(mp, w.sc); mp_print_char(mp, xord(' ')); 
  mp_print_scaled(mp, w.sc / 010000); mp_print_ln(mp);
  mp_print_int(mp, w.hh.lh); mp_print_char(mp, xord('=')); 
  mp_print_int(mp, w.hh.b0); mp_print_char(mp, xord(':'));
  mp_print_int(mp, w.hh.b1); mp_print_char(mp, xord(';')); 
  mp_print_int(mp, w.hh.rh); mp_print_char(mp, xord(' '));
  mp_print_int(mp, w.qqqq.b0); mp_print_char(mp, xord(':')); 
  mp_print_int(mp, w.qqqq.b1); mp_print_char(mp, xord(':'));
  mp_print_int(mp, w.qqqq.b2); mp_print_char(mp, xord(':')); 
  mp_print_int(mp, w.qqqq.b3);
}
#endif

@* \[10] Dynamic memory allocation.

The \MP\ system does nearly all of its own memory allocation, so that it
can readily be transported into environments that do not have automatic
facilities for strings, garbage collection, etc., and so that it can be in
control of what error messages the user receives. The dynamic storage
requirements of \MP\ are handled by providing a large array |mem| in
which consecutive blocks of words are used as nodes by the \MP\ routines.

Pointer variables are indices into this array, or into another array
called |eqtb| that will be explained later. A pointer variable might
also be a special flag that lies outside the bounds of |mem|, so we
allow pointers to assume any |halfword| value. The minimum memory
index represents a null pointer.

@d null 0 /* the null pointer */
@d mp_void (null+1) /* a null pointer different from |null| */


@<Types...@>=
typedef halfword pointer; /* a flag or a location in |mem| or |eqtb| */

@ The |mem| array is divided into two regions that are allocated separately,
but the dividing line between these two regions is not fixed; they grow
together until finding their ``natural'' size in a particular job.
Locations less than or equal to |lo_mem_max| are used for storing
variable-length records consisting of two or more words each. This region
is maintained using an algorithm similar to the one described in exercise
2.5--19 of {\sl The Art of Computer Programming}. However, no size field
appears in the allocated nodes; the program is responsible for knowing the
relevant size when a node is freed. Locations greater than or equal to
|hi_mem_min| are used for storing one-word records; a conventional
\.{AVAIL} stack is used for allocation in this region.

Locations of |mem| between |0| and |mem_top| may be dumped as part
of preloaded mem files, by the \.{INIMP} preprocessor.
@.INIMP@>
Production versions of \MP\ may extend the memory at the top end in order to
provide more space; these locations, between |mem_top| and |mem_max|,
are always used for single-word nodes.

The key pointers that govern |mem| allocation have a prescribed order:
$$\hbox{|null=0<lo_mem_max<hi_mem_min<mem_top<=mem_end<=mem_max|.}$$

@<Glob...@>=
memory_word *mem; /* the big dynamic storage area */
pointer lo_mem_max; /* the largest location of variable-size memory in use */
pointer hi_mem_min; /* the smallest location of one-word memory in use */


@ 
@d xfree(A) do { mp_xfree(A); A=NULL; } while (0)
@d xrealloc(P,A,B) mp_xrealloc(mp,P,(size_t)A,B)
@d xmalloc(A,B)  mp_xmalloc(mp,(size_t)A,B)
@d xstrdup(A)  mp_xstrdup(mp,A)
@d XREALLOC(a,b,c) a = xrealloc(a,(b+1),sizeof(c));

@<Declare helpers@>=
extern char *mp_strdup(const char *p) ;
extern char *mp_strldup(const char *p, size_t l) ;
extern void mp_xfree ( @= /*@@only@@*/ /*@@out@@*/ /*@@null@@*/ @> void *x);
extern @= /*@@only@@*/ @> void *mp_xrealloc (MP mp, void *p, size_t nmem, size_t size) ;
extern @= /*@@only@@*/ @> void *mp_xmalloc (MP mp, size_t nmem, size_t size) ;
extern @= /*@@only@@*/ @> char *mp_xstrdup(MP mp, const char *s);
extern @= /*@@only@@*/ @> char *mp_xstrldup(MP mp, const char *s, size_t l);
#ifndef HAVE_SNPRINTF
extern void mp_do_snprintf(char *str, int size, const char *fmt, ...);
#endif

@ The |max_size_test| guards against overflow, on the assumption that
|size_t| is at least 31bits wide.

@d max_size_test 0x7FFFFFFF

@c
char *mp_strldup(const char *p, size_t l) {
  char *r;
  if (p==NULL) return NULL;
  r = malloc ((size_t)(l*sizeof(char)+1));
  if (r==NULL)
    return NULL;
  return memcpy (r,p,(size_t)(l+1));
}
char *mp_strdup(const char *p) {
  if (p==NULL) return NULL;
  return mp_strldup(p, strlen(p));
}
void mp_xfree (void *x) {
  if (x!=NULL) free(x);
}
void  *mp_xrealloc (MP mp, void *p, size_t nmem, size_t size) {
  void *w ; 
  if ((max_size_test/size)<nmem) {
    do_fprintf(mp->err_out,"Memory size overflow!\n");
    mp->history =mp_fatal_error_stop;    mp_jump_out(mp);
  }
  w = realloc (p,(nmem*size));
  if (w==NULL) {
    do_fprintf(mp->err_out,"Out of memory!\n");
    mp->history =mp_system_error_stop;    mp_jump_out(mp);
  }
  return w;
}
void  *mp_xmalloc (MP mp, size_t nmem, size_t size) {
  void *w;
  if ((max_size_test/size)<nmem) {
    do_fprintf(mp->err_out,"Memory size overflow!\n");
    mp->history =mp_fatal_error_stop;    mp_jump_out(mp);
  }
  w = malloc (nmem*size);
  if (w==NULL) {
    do_fprintf(mp->err_out,"Out of memory!\n");
    mp->history =mp_system_error_stop;    mp_jump_out(mp);
  }
  return w;
}
char *mp_xstrldup(MP mp, const char *s, size_t l) {
  char *w; 
  if (s==NULL)
    return NULL;
  w = mp_strldup(s, l);
  if (w==NULL) {
    do_fprintf(mp->err_out,"Out of memory!\n");
    mp->history =mp_system_error_stop;    mp_jump_out(mp);
  }
  return w;
}
char *mp_xstrdup(MP mp, const char *s) {
  if (s==NULL)  return NULL;
  return mp_xstrldup(mp,s,strlen(s));
}


@ @<Internal library declarations@>=
#ifdef HAVE_SNPRINTF
#define mp_snprintf (void)snprintf
#else
#define mp_snprintf mp_do_snprintf
#endif

@ This internal version is rather stupid, but good enough for its purpose.

@c
#ifndef HAVE_SNPRINTF
static char *mp_itoa (int i) {
  char res[32] ;
  unsigned idx = 30;
  unsigned v = (unsigned)abs(i);
  memset(res,0,32*sizeof(char));
  while (v>=10) {
    char d = (char)(v % 10);
    v = v / 10;
    res[idx--] = (char)(d + '0');
  }
  res[idx--] = (char)(v + '0');
  if (i<0) {
      res[idx--] = '-';
  }
  return mp_strdup((res+idx+1));
}
static char *mp_utoa (unsigned v) {
  char res[32] ;
  unsigned idx = 30;
  memset(res,0,32*sizeof(char));
  while (v>=10) {
    char d = (char)(v % 10);
    v = v / 10;
    res[idx--] = (char)(d + '0');
  }
  res[idx--] = (char)(v + '0');
  return mp_strdup((res+idx+1));
}
void mp_do_snprintf (char *str, int size, const char *format, ...) {
  const char *fmt;
  char *res;
  int fw, pad;
  va_list ap;
  va_start(ap, format);
  res = str;
  for (fmt=format;*fmt!='\0';fmt++) {
     if (*fmt=='%') {
       fw=0;
       pad=0;
     RESTART:
       fmt++;
       switch(*fmt) {
       case '0':
         pad=1;
         goto RESTART;
         break;
       case '1':
       case '2':
       case '3':
       case '4':
       case '5':
       case '6':
       case '7':
       case '8':
       case '9':
         assert(fw==0);
         fw = *fmt-'0';
         goto RESTART;
         break;
       case 's':
         {
           char *s = va_arg(ap, char *);
           while (*s) {
             *res = *s++;
             if (size-->0) res++;
           }
         }
         break;
       case 'c':
         {
           int s = va_arg(ap, int);
           *res = (char)s;
           if (size-->0) res++;
         }
         break;
       case 'i':
       case 'd':
         {
           char *sstart, *s = mp_itoa(va_arg(ap, int));
           sstart = s;
           if (fw) {
              int ffw = fw-(int)strlen(s);
              while (ffw-->0) {
                 *res = (char)(pad ? '0' : ' ');
                 if (size-->0) res++;
              }
           }
           if (s != NULL) {
             while (*s) {
               *res = *s++;
               if (size-->0) res++;
             }
             mp_xfree(sstart);
           }
         }
         break;
       case 'u':
         {
           char *sstart, *s = mp_utoa(va_arg(ap, unsigned));
           sstart = s;
           if (fw) {
              int ffw = fw-(int)strlen(s);
              while (ffw-->0) {
                 *res = (char)(pad ? '0' : ' ');
                 if (size-->0) res++;
              }
           }
           if (s != NULL) {
             while (*s) {
               *res = *s++;
               if (size-->0) res++;
             }
             mp_xfree(sstart);
           }
         }
         break;
       case '%':
         *res = '%';
         if (size-->0) res++;
         break;
       default:
         *res = '%';
         if (size-->0) res++;
         *res = *fmt;
         if (size-->0) res++;
         break;
       }
     } else {
       *res = *fmt;
       if (size-->0) res++;
     }
  }
  *res = '\0';
  va_end(ap);
}
#endif

@ 
@<Allocate or initialize ...@>=
mp->mem = xmalloc ((mp->mem_max+1),sizeof (memory_word));
memset(mp->mem,0,(size_t)(mp->mem_max+1)*sizeof (memory_word));

@ @<Dealloc variables@>=
xfree(mp->mem);

@ Users who wish to study the memory requirements of particular applications can
can use optional special features that keep track of current and
maximum memory usage. When code between the delimiters |stat| $\ldots$
|tats| is not ``commented out,'' \MP\ will run a bit slower but it will
report these statistics when |mp_tracing_stats| is positive.

@<Glob...@>=
integer var_used; integer dyn_used; /* how much memory is in use */

@ Let's consider the one-word memory region first, since it's the
simplest. The pointer variable |mem_end| holds the highest-numbered location
of |mem| that has ever been used. The free locations of |mem| that
occur between |hi_mem_min| and |mem_end|, inclusive, are of type
|two_halves|, and we write |info(p)| and |mp_link(p)| for the |lh|
and |rh| fields of |mem[p]| when it is of this type. The single-word
free locations form a linked list
$$|avail|,\;\hbox{|mp_link(avail)|},\;\hbox{|mp_link(mp_link(avail))|},\;\ldots$$
terminated by |null|.

@(mpmp.h@>=
#define mp_link(A)   mp->mem[(A)].hh.rh /* the |link| field of a memory word */
#define mp_info(A)   mp->mem[(A)].hh.lh /* the |info| field of a memory word */

@ @<Glob...@>=
pointer avail; /* head of the list of available one-word nodes */
pointer mem_end; /* the last one-word node used in |mem| */

@ If one-word memory is exhausted, it might mean that the user has forgotten
a token like `\&{enddef}' or `\&{endfor}'. We will define some procedures
later that try to help pinpoint the trouble.

@ The function |get_avail| returns a pointer to a new one-word node whose
|link| field is null. However, \MP\ will halt if there is no more room left.
@^inner loop@>

@c 
static pointer mp_get_avail (MP mp) { /* single-word node allocation */
  pointer p; /* the new node being got */
  p=mp->avail; /* get top location in the |avail| stack */
  if ( p!=null ) {
    mp->avail=mp_link(mp->avail); /* and pop it off */
  } else if ( mp->mem_end<mp->mem_max ) { /* or go into virgin territory */
    incr(mp->mem_end); p=mp->mem_end;
  } else { 
    decr(mp->hi_mem_min); p=mp->hi_mem_min;
    if ( mp->hi_mem_min<=mp->lo_mem_max ) { 
      mp_runaway(mp); /* if memory is exhausted, display possible runaway text */
      mp_overflow(mp, "main memory size",mp->mem_max);
      /* quit; all one-word nodes are busy */
@:MetaPost capacity exceeded main memory size}{\quad main memory size@>
    }
  }
  mp_link(p)=null; /* provide an oft-desired initialization of the new node */
  incr(mp->dyn_used);/* maintain statistics */
  return p;
}

@ Conversely, a one-word node is recycled by calling |free_avail|.

@d free_avail(A)  /* single-word node liberation */
  { mp_link((A))=mp->avail; mp->avail=(A); decr(mp->dyn_used);  }

@ There's also a |fast_get_avail| routine, which saves the procedure-call
overhead at the expense of extra programming. This macro is used in
the places that would otherwise account for the most calls of |get_avail|.
@^inner loop@>

@d fast_get_avail(A) { 
  (A)=mp->avail; /* avoid |get_avail| if possible, to save time */
  if ( (A)==null ) { (A)=mp_get_avail(mp); } 
  else { mp->avail=mp_link((A)); mp_link((A))=null;  incr(mp->dyn_used); }
  }

@ The available-space list that keeps track of the variable-size portion
of |mem| is a nonempty, doubly-linked circular list of empty nodes,
pointed to by the roving pointer |rover|.

Each empty node has size 2 or more; the first word contains the special
value |max_halfword| in its |link| field and the size in its |info| field;
the second word contains the two pointers for double linking.

Each nonempty node also has size 2 or more. Its first word is of type
|two_halves|\kern-1pt, and its |link| field is never equal to |max_halfword|.
Otherwise there is complete flexibility with respect to the contents
of its other fields and its other words.

(We require |mem_max<max_halfword| because terrible things can happen
when |max_halfword| appears in the |link| field of a nonempty node.)

@d empty_flag   max_halfword /* the |link| of an empty variable-size node */
@d is_empty(A)   (mp_link((A))==empty_flag) /* tests for empty node */

@(mpmp.h@>=
#define node_size   mp_info /* the size field in empty variable-size nodes */
#define lmp_link(A)   mp_info((A)+1) /* left link in doubly-linked list of empty nodes */
#define rmp_link(A)   mp_link((A)+1) /* right link in doubly-linked list of empty nodes */

@ @<Glob...@>=
pointer rover; /* points to some node in the list of empties */

@ A call to |get_node| with argument |s| returns a pointer to a new node
of size~|s|, which must be 2~or more. The |link| field of the first word
of this new node is set to null. An overflow stop occurs if no suitable
space exists.

If |get_node| is called with $s=2^{30}$, it simply merges adjacent free
areas and returns the value |max_halfword|.

@<Internal library declarations@>=
pointer mp_get_node (MP mp,integer s) ;

@ @c 
pointer mp_get_node (MP mp,integer s) { /* variable-size node allocation */
  pointer p; /* the node currently under inspection */
  pointer q;  /* the node physically after node |p| */
  integer r; /* the newly allocated node, or a candidate for this honor */
  integer t,tt; /* temporary registers */
@^inner loop@>
 RESTART: 
  p=mp->rover; /* start at some free node in the ring */
  do {  
    @<Try to allocate within node |p| and its physical successors,
     and |goto found| if allocation was possible@>;
    if (rmp_link(p)==null || (rmp_link(p)==p && p!=mp->rover)) {
      print_err("Free list garbled");
      help3("I found an entry in the list of free nodes that links",
       "badly. I will try to ignore the broken link, but something",
       "is seriously amiss. It is wise to warn the maintainers.")
	  mp_error(mp);
      rmp_link(p)=mp->rover;
    }
	p=rmp_link(p); /* move to the next node in the ring */
  } while (p!=mp->rover); /* repeat until the whole list has been traversed */
  if ( s==010000000000 ) { 
    return max_halfword;
  };
  if ( mp->lo_mem_max+2<mp->hi_mem_min ) {
    if ( mp->lo_mem_max+2<=max_halfword ) {
      @<Grow more variable-size memory and |goto restart|@>;
    }
  }
  mp_overflow(mp, "main memory size",mp->mem_max);
  /* sorry, nothing satisfactory is left */
@:MetaPost capacity exceeded main memory size}{\quad main memory size@>
FOUND: 
  mp_link(r)=null; /* this node is now nonempty */
  mp->var_used+=s; /* maintain usage statistics */
  return r;
}

@ The lower part of |mem| grows by 1000 words at a time, unless
we are very close to going under. When it grows, we simply link
a new node into the available-space list. This method of controlled
growth helps to keep the |mem| usage consecutive when \MP\ is
implemented on ``virtual memory'' systems.
@^virtual memory@>

@<Grow more variable-size memory and |goto restart|@>=
{ 
  if ( mp->hi_mem_min-mp->lo_mem_max>=1998 ) {
    t=mp->lo_mem_max+1000;
  } else {
    t=mp->lo_mem_max+1+(mp->hi_mem_min-mp->lo_mem_max) / 2; 
    /* |lo_mem_max+2<=t<hi_mem_min| */
  }
  if ( t>max_halfword ) t=max_halfword;
  p=lmp_link(mp->rover); q=mp->lo_mem_max; rmp_link(p)=q; lmp_link(mp->rover)=q;
  rmp_link(q)=mp->rover; lmp_link(q)=p; mp_link(q)=empty_flag; 
  node_size(q)=t-mp->lo_mem_max;
  mp->lo_mem_max=t; mp_link(mp->lo_mem_max)=null; mp_info(mp->lo_mem_max)=null;
  mp->rover=q; 
  goto RESTART;
}

@ @<Try to allocate...@>=
q=p+node_size(p); /* find the physical successor */
while ( is_empty(q) ) { /* merge node |p| with node |q| */
  t=rmp_link(q); tt=lmp_link(q);
@^inner loop@>
  if ( q==mp->rover ) mp->rover=t;
  lmp_link(t)=tt; rmp_link(tt)=t;
  q=q+node_size(q);
}
r=q-s;
if ( r>p+1 ) {
  @<Allocate from the top of node |p| and |goto found|@>;
}
if ( r==p ) { 
  if ( rmp_link(p)!=p ) {
    @<Allocate entire node |p| and |goto found|@>;
  }
}
node_size(p)=q-p /* reset the size in case it grew */

@ @<Allocate from the top...@>=
{ 
  node_size(p)=r-p; /* store the remaining size */
  mp->rover=p; /* start searching here next time */
  goto FOUND;
}

@ Here we delete node |p| from the ring, and let |rover| rove around.

@<Allocate entire...@>=
{ 
  mp->rover=rmp_link(p); t=lmp_link(p);
  lmp_link(mp->rover)=t; rmp_link(t)=mp->rover;
  goto FOUND;
}

@ Conversely, when some variable-size node |p| of size |s| is no longer needed,
the operation |free_node(p,s)| will make its words available, by inserting
|p| as a new empty node just before where |rover| now points.

@<Internal library declarations@>=
void mp_free_node (MP mp, pointer p, halfword s) ;

@ @c 
void mp_free_node (MP mp, pointer p, halfword s) { /* variable-size node
  liberation */
  pointer q; /* |lmp_link(rover)| */
  node_size(p)=s; mp_link(p)=empty_flag;
@^inner loop@>
  q=lmp_link(mp->rover); lmp_link(p)=q; rmp_link(p)=mp->rover; /* set both links */
  lmp_link(mp->rover)=p; rmp_link(q)=p; /* insert |p| into the ring */
  mp->var_used-=s; /* maintain statistics */
}

@* \[11] Memory layout.
Some areas of |mem| are dedicated to fixed usage, since static allocation is
more efficient than dynamic allocation when we can get away with it. For
example, locations |0| to |1| are always used to store a
two-word dummy token whose second word is zero.
The following macro definitions accomplish the static allocation by giving
symbolic names to the fixed positions. Static variable-size nodes appear
in locations |0| through |lo_mem_stat_max|, and static single-word nodes
appear in locations |hi_mem_stat_min| through |mem_top|, inclusive.

@d sentinel mp->mem_top /* end of sorted lists */
@d temp_head (mp->mem_top-1) /* head of a temporary list of some kind */
@d hold_head (mp->mem_top-2) /* head of a temporary list of another kind */

@(mpmp.h@>=
#define spec_head (mp->mem_top-3) /* head of a list of unprocessed \&{special} items */
#define null_dash (2) /* the first two words are reserved for a null value */
#define dep_head (null_dash+3) /* we will define |dash_node_size=3| */
#define zero_val (dep_head+2) /* two words for a permanently zero value */
#define temp_val (zero_val+2) /* two words for a temporary value node */
#define end_attr temp_val /* we use |end_attr+2| only */
#define inf_val (end_attr+2) /* and |inf_val+1| only */
#define bad_vardef (inf_val+2) /* two words for \&{vardef} error recovery */
#define lo_mem_stat_max (bad_vardef+1)  /* largest statically
  allocated word in the variable-size |mem| */
#define hi_mem_stat_min (mp->mem_top-3) /* smallest statically allocated word in
  the one-word |mem| */

@ The following code gets the dynamic part of |mem| off to a good start,
when \MP\ is initializing itself the slow way.

@<Initialize table entries (done by \.{INIMP} only)@>=
mp->rover=lo_mem_stat_max+1; /* initialize the dynamic memory */
mp_link(mp->rover)=empty_flag;
node_size(mp->rover)=1000; /* which is a 1000-word available node */
lmp_link(mp->rover)=mp->rover; rmp_link(mp->rover)=mp->rover;
mp->lo_mem_max=mp->rover+1000; 
mp_link(mp->lo_mem_max)=null; mp_info(mp->lo_mem_max)=null;
for (k=hi_mem_stat_min;k<=(int)mp->mem_top;k++) {
  mp->mem[k]=mp->mem[mp->lo_mem_max]; /* clear list heads */
}
mp->avail=null; mp->mem_end=mp->mem_top;
mp->hi_mem_min=hi_mem_stat_min; /* initialize the one-word memory */
mp->var_used=lo_mem_stat_max+1; 
mp->dyn_used=mp->mem_top+1-(hi_mem_stat_min);  /* initialize statistics */

@ The procedure |flush_list(p)| frees an entire linked list of one-word
nodes that starts at a given position, until coming to |sentinel| or a
pointer that is not in the one-word region. Another procedure,
|flush_node_list|, frees an entire linked list of one-word and two-word
nodes, until coming to a |null| pointer.
@^inner loop@>

@c 
static void mp_flush_list (MP mp,pointer p) { /* makes list of single-word nodes  available */
  pointer q,r; /* list traversers */
  if ( p>=mp->hi_mem_min ) if ( p!=sentinel ) { 
    r=p;
    do {  
      q=r; r=mp_link(r); 
      decr(mp->dyn_used);
      if ( r<mp->hi_mem_min ) break;
    } while (r!=sentinel);
  /* now |q| is the last node on the list */
    mp_link(q)=mp->avail; mp->avail=p;
  }
}
@#
static void mp_flush_node_list (MP mp,pointer p) {
  pointer q; /* the node being recycled */
  while ( p!=null ){ 
    q=p; p=mp_link(p);
    if ( q<mp->hi_mem_min ) 
      mp_free_node(mp, q,2);
    else 
      free_avail(q);
  }
}

@ If \MP\ is extended improperly, the |mem| array might get screwed up.
For example, some pointers might be wrong, or some ``dead'' nodes might not
have been freed when the last reference to them disappeared. Procedures
|check_mem| and |search_mem| are available to help diagnose such
problems. These procedures make use of two arrays called |free| and
|was_free| that are present only if \MP's debugging routines have
been included. (You may want to decrease the size of |mem| while you
@^debugging@>
are debugging.)

Because |boolean|s are typedef-d as ints, it is better to use
unsigned chars here.

@<Glob...@>=
unsigned char *free; /* free cells */
unsigned char *was_free; /* previously free cells */
pointer was_mem_end; pointer was_lo_max; pointer was_hi_min;
  /* previous |mem_end|, |lo_mem_max|,and |hi_mem_min| */
boolean panicking; /* do we want to check memory constantly? */

@ @<Allocate or initialize ...@>=
mp->free = xmalloc ((mp->mem_max+1),sizeof (unsigned char));
mp->was_free = xmalloc ((mp->mem_max+1), sizeof (unsigned char));

@ @<Dealloc variables@>=
xfree(mp->free);
xfree(mp->was_free);

@ @<Allocate or ...@>=
mp->was_hi_min=mp->mem_max;
mp->panicking=false;

@ @<Declarations@>=
static void mp_reallocate_memory(MP mp, int l) ;

@ @c
static void mp_reallocate_memory(MP mp, int l) {
   XREALLOC(mp->free,     l, unsigned char);
   XREALLOC(mp->was_free, l, unsigned char);
   if (mp->mem) {
	 int newarea = l-mp->mem_max;
     XREALLOC(mp->mem,      l, memory_word);
     memset (mp->mem+(mp->mem_max+1),0,(size_t)(sizeof(memory_word)*((unsigned)newarea)));
   } else {
     XREALLOC(mp->mem,      l, memory_word);
     memset(mp->mem,0,(size_t)(sizeof(memory_word)*(unsigned)(l+1)));
   }
   mp->mem_max = l;
   if (mp->ini_version) 
     mp->mem_top = l;
}



@ Procedure |check_mem| makes sure that the available space lists of
|mem| are well formed, and it optionally prints out all locations
that are reserved now but were free the last time this procedure was called.

@c 
#if 0
static void mp_check_mem (MP mp,boolean print_locs ) {
  pointer p,q,r; /* current locations of interest in |mem| */
  boolean clobbered; /* is something amiss? */
  for (p=0;p<=mp->lo_mem_max;p++) {
    mp->free[p]=false; /* you can probably do this faster */
  }
  for (p=mp->hi_mem_min;p<= mp->mem_end;p++) {
    mp->free[p]=false; /* ditto */
  }
  @<Check single-word |avail| list@>;
  @<Check variable-size |avail| list@>;
  @<Check flags of unavailable nodes@>;
  @<Check the list of linear dependencies@>;
  if ( print_locs ) {
    @<Print newly busy locations@>;
  }
  (void)memcpy(mp->was_free, mp->free, (size_t)(sizeof(char)*(unsigned)(mp->mem_end+1)));
  mp->was_mem_end=mp->mem_end; 
  mp->was_lo_max=mp->lo_mem_max; 
  mp->was_hi_min=mp->hi_mem_min;
}
#endif

@ @<Check single-word...@>=
p=mp->avail; q=null; clobbered=false;
while ( p!=null ) { 
  if ( (p>mp->mem_end)||(p<mp->hi_mem_min) ) clobbered=true;
  else if ( mp->free[p] ) clobbered=true;
  if ( clobbered ) { 
    mp_print_nl(mp, "AVAIL list clobbered at ");
@.AVAIL list clobbered...@>
    mp_print_int(mp, q); break;
  }
  mp->free[p]=true; q=p; p=mp_link(q);
}

@ @<Check variable-size...@>=
p=mp->rover; q=null; clobbered=false;
do {  
  if ( (p>=mp->lo_mem_max)||(p<0) ) clobbered=true;
  else if ( (rmp_link(p)>=mp->lo_mem_max)||(rmp_link(p)<0) ) clobbered=true;
  else if (  !(is_empty(p))||(node_size(p)<2)||
   (p+node_size(p)>mp->lo_mem_max)|| (lmp_link(rmp_link(p))!=p) ) clobbered=true;
  if ( clobbered ) { 
    mp_print_nl(mp, "Double-AVAIL list clobbered at ");
@.Double-AVAIL list clobbered...@>
    mp_print_int(mp, q); break;
  }
  for (q=p;q<=p+node_size(p)-1;q++) { /* mark all locations free */
    if ( mp->free[q] ) { 
      mp_print_nl(mp, "Doubly free location at ");
@.Doubly free location...@>
      mp_print_int(mp, q); break;
    }
    mp->free[q]=true;
  }
  q=p; p=rmp_link(p);
} while (p!=mp->rover)


@ @<Check flags...@>=
p=0;
while ( p<=mp->lo_mem_max ) { /* node |p| should not be empty */
  if ( is_empty(p) ) {
    mp_print_nl(mp, "Bad flag at "); mp_print_int(mp, p);
@.Bad flag...@>
  }
  while ( (p<=mp->lo_mem_max) && ! mp->free[p] ) p++;
  while ( (p<=mp->lo_mem_max) && mp->free[p] ) p++;
}

@ @<Print newly busy...@>=
{ 
  @<Do intialization required before printing new busy locations@>;
  mp_print_nl(mp, "New busy locs:");
@.New busy locs@>
  for (p=0;p<= mp->lo_mem_max;p++ ) {
    if ( ! mp->free[p] && ((p>mp->was_lo_max) || mp->was_free[p]) ) {
      @<Indicate that |p| is a new busy location@>;
    }
  }
  for (p=mp->hi_mem_min;p<=mp->mem_end;p++ ) {
    if ( ! mp->free[p] &&
        ((p<mp->was_hi_min) || (p>mp->was_mem_end) || mp->was_free[p]) ) {
      @<Indicate that |p| is a new busy location@>;
    }
  }
  @<Finish printing new busy locations@>;
}

@ There might be many new busy locations so we are careful to print contiguous
blocks compactly.  During this operation |q| is the last new busy location and
|r| is the start of the block containing |q|.

@<Indicate that |p| is a new busy location@>=
{ 
  if ( p>q+1 ) { 
    if ( q>r ) { 
      mp_print(mp, ".."); mp_print_int(mp, q);
    }
    mp_print_char(mp, xord(' ')); mp_print_int(mp, p);
    r=p;
  }
  q=p;
}

@ @<Do intialization required before printing new busy locations@>=
q=mp->mem_max; r=mp->mem_max

@ @<Finish printing new busy locations@>=
if ( q>r ) { 
  mp_print(mp, ".."); mp_print_int(mp, q);
}

@ The |search_mem| procedure attempts to answer the question ``Who points
to node~|p|?'' In doing so, it fetches |link| and |info| fields of |mem|
that might not be of type |two_halves|. Strictly speaking, this is
undefined, and it can lead to ``false drops'' (words that seem to
point to |p| purely by coincidence). But for debugging purposes, we want
to rule out the places that do {\sl not\/} point to |p|, so a few false
drops are tolerable.

@c
#if 0
void mp_search_mem (MP mp, pointer p) { /* look for pointers to |p| */
  integer q; /* current position being searched */
  for (q=0;q<=mp->lo_mem_max;q++) { 
    if ( mp_link(q)==p ){ 
      mp_print_nl(mp, "LINK("); mp_print_int(mp, q); mp_print_char(mp, xord(')'));
    }
    if ( mp_info(q)==p ) { 
      mp_print_nl(mp, "INFO("); mp_print_int(mp, q); mp_print_char(mp, xord(')'));
    }
  }
  for (q=mp->hi_mem_min;q<=mp->mem_end;q++) {
    if ( mp_link(q)==p ) {
      mp_print_nl(mp, "LINK("); mp_print_int(mp, q); mp_print_char(mp, xord(')'));
    }
    if ( mp_info(q)==p ) {
      mp_print_nl(mp, "INFO("); mp_print_int(mp, q); mp_print_char(mp, xord(')'));
    }
  }
  @<Search |eqtb| for equivalents equal to |p|@>;
}
#endif

@ Just before \.{INIMP} writes out the memory, it sorts the doubly linked
available space list. The list is probably very short at such times, so a
simple insertion sort is used. The smallest available location will be
pointed to by |rover|, the next-smallest by |rmp_link(rover)|, etc.

@<Internal library ...@>=
void mp_sort_avail (MP mp);

@ @c 
void mp_sort_avail (MP mp) { /* sorts the available variable-size nodes
  by location */
  pointer p,q,r; /* indices into |mem| */
  pointer old_rover; /* initial |rover| setting */
  p=mp_get_node(mp, 010000000000); /* merge adjacent free areas */
  p=rmp_link(mp->rover); rmp_link(mp->rover)=max_halfword; old_rover=mp->rover;
  while ( p!=old_rover ) {
    @<Sort |p| into the list starting at |rover|
     and advance |p| to |rmp_link(p)|@>;
  }
  p=mp->rover;
  while ( rmp_link(p)!=max_halfword ) { 
    lmp_link(rmp_link(p))=p; p=rmp_link(p);
  };
  rmp_link(p)=mp->rover; lmp_link(mp->rover)=p;
}

@ The following |while| loop is guaranteed to
terminate, since the list that starts at
|rover| ends with |max_halfword| during the sorting procedure.

@<Sort |p|...@>=
if ( p<mp->rover ) { 
  q=p; p=rmp_link(q); rmp_link(q)=mp->rover; mp->rover=q;
} else  { 
  q=mp->rover;
  while ( rmp_link(q)<p ) q=rmp_link(q);
  r=rmp_link(p); rmp_link(p)=rmp_link(q); rmp_link(q)=p; p=r;
}


@* \[12] The command codes.
Before we can go much further, we need to define symbolic names for the internal
code numbers that represent the various commands obeyed by \MP. These codes
are somewhat arbitrary, but not completely so. For example,
some codes have been made adjacent so that |case| statements in the
program need not consider cases that are widely spaced, or so that |case|
statements can be replaced by |if| statements. A command can begin an
expression if and only if its code lies between |min_primary_command| and
|max_primary_command|, inclusive. The first token of a statement that doesn't
begin with an expression has a command code between |min_command| and
|max_statement_command|, inclusive. Anything less than |min_command| is
eliminated during macro expansions, and anything no more than |max_pre_command|
is eliminated when expanding \TeX\ material.  Ranges such as
|min_secondary_command..max_secondary_command| are used when parsing
expressions, but the relative ordering within such a range is generally not
critical.

The ordering of the highest-numbered commands
(|comma<semicolon<end_group<stop|) is crucial for the parsing and
error-recovery methods of this program as is the ordering |if_test<fi_or_else|
for the smallest two commands.  The ordering is also important in the ranges
|numeric_token..plus_or_minus| and |left_brace..ampersand|.

At any rate, here is the list, for future reference.

@d start_tex 1 /* begin \TeX\ material (\&{btex}, \&{verbatimtex}) */
@d etex_marker 2 /* end \TeX\ material (\&{etex}) */
@d mpx_break 3 /* stop reading an \.{MPX} file (\&{mpxbreak}) */
@d max_pre_command mpx_break
@d if_test 4 /* conditional text (\&{if}) */
@d fi_or_else 5 /* delimiters for conditionals (\&{elseif}, \&{else}, \&{fi}) */
@d input 6 /* input a source file (\&{input}, \&{endinput}) */
@d iteration 7 /* iterate (\&{for}, \&{forsuffixes}, \&{forever}, \&{endfor}) */
@d repeat_loop 8 /* special command substituted for \&{endfor} */
@d exit_test 9 /* premature exit from a loop (\&{exitif}) */
@d relax 10 /* do nothing (\.{\char`\\}) */
@d scan_tokens 11 /* put a string into the input buffer */
@d expand_after 12 /* look ahead one token */
@d defined_macro 13 /* a macro defined by the user */
@d min_command (defined_macro+1)
@d save_command 14 /* save a list of tokens (\&{save}) */
@d interim_command 15 /* save an internal quantity (\&{interim}) */
@d let_command 16 /* redefine a symbolic token (\&{let}) */
@d new_internal 17 /* define a new internal quantity (\&{newinternal}) */
@d macro_def 18 /* define a macro (\&{def}, \&{vardef}, etc.) */
@d ship_out_command 19 /* output a character (\&{shipout}) */
@d add_to_command 20 /* add to edges (\&{addto}) */
@d bounds_command 21  /* add bounding path to edges (\&{setbounds}, \&{clip}) */
@d tfm_command 22 /* command for font metric info (\&{ligtable}, etc.) */
@d protection_command 23 /* set protection flag (\&{outer}, \&{inner}) */
@d show_command 24 /* diagnostic output (\&{show}, \&{showvariable}, etc.) */
@d mode_command 25 /* set interaction level (\&{batchmode}, etc.) */
@d mp_random_seed 26 /* initialize random number generator (\&{randomseed}) */
@d message_command 27 /* communicate to user (\&{message}, \&{errmessage}) */
@d every_job_command 28 /* designate a starting token (\&{everyjob}) */
@d delimiters 29 /* define a pair of delimiters (\&{delimiters}) */
@d special_command 30 /* output special info (\&{special})
                       or font map info (\&{fontmapfile}, \&{fontmapline}) */
@d write_command 31 /* write text to a file (\&{write}) */
@d type_name 32 /* declare a type (\&{numeric}, \&{pair}, etc.) */
@d max_statement_command type_name
@d min_primary_command type_name
@d left_delimiter 33 /* the left delimiter of a matching pair */
@d begin_group 34 /* beginning of a group (\&{begingroup}) */
@d nullary 35 /* an operator without arguments (e.g., \&{normaldeviate}) */
@d unary 36 /* an operator with one argument (e.g., \&{sqrt}) */
@d str_op 37 /* convert a suffix to a string (\&{str}) */
@d cycle 38 /* close a cyclic path (\&{cycle}) */
@d primary_binary 39 /* binary operation taking `\&{of}' (e.g., \&{point}) */
@d capsule_token 40 /* a value that has been put into a token list */
@d string_token 41 /* a string constant (e.g., |"hello"|) */
@d internal_quantity 42 /* internal numeric parameter (e.g., \&{pausing}) */
@d min_suffix_token internal_quantity
@d tag_token 43 /* a symbolic token without a primitive meaning */
@d numeric_token 44 /* a numeric constant (e.g., \.{3.14159}) */
@d max_suffix_token numeric_token
@d plus_or_minus 45 /* either `\.+' or `\.-' */
@d max_primary_command plus_or_minus /* should also be |numeric_token+1| */
@d min_tertiary_command plus_or_minus
@d tertiary_secondary_macro 46 /* a macro defined by \&{secondarydef} */
@d tertiary_binary 47 /* an operator at the tertiary level (e.g., `\.{++}') */
@d max_tertiary_command tertiary_binary
@d left_brace 48 /* the operator `\.{\char`\{}' */
@d min_expression_command left_brace
@d path_join 49 /* the operator `\.{..}' */
@d ampersand 50 /* the operator `\.\&' */
@d expression_tertiary_macro 51 /* a macro defined by \&{tertiarydef} */
@d expression_binary 52 /* an operator at the expression level (e.g., `\.<') */
@d equals 53 /* the operator `\.=' */
@d max_expression_command equals
@d and_command 54 /* the operator `\&{and}' */
@d min_secondary_command and_command
@d secondary_primary_macro 55 /* a macro defined by \&{primarydef} */
@d slash 56 /* the operator `\./' */
@d secondary_binary 57 /* an operator at the binary level (e.g., \&{shifted}) */
@d max_secondary_command secondary_binary
@d param_type 58 /* type of parameter (\&{primary}, \&{expr}, \&{suffix}, etc.) */
@d controls 59 /* specify control points explicitly (\&{controls}) */
@d tension 60 /* specify tension between knots (\&{tension}) */
@d at_least 61 /* bounded tension value (\&{atleast}) */
@d curl_command 62 /* specify curl at an end knot (\&{curl}) */
@d macro_special 63 /* special macro operators (\&{quote}, \.{\#\AT!}, etc.) */
@d right_delimiter 64 /* the right delimiter of a matching pair */
@d left_bracket 65 /* the operator `\.[' */
@d right_bracket 66 /* the operator `\.]' */
@d right_brace 67 /* the operator `\.{\char`\}}' */
@d with_option 68 /* option for filling (\&{withpen}, \&{withweight}, etc.) */
@d thing_to_add 69
  /* variant of \&{addto} (\&{contour}, \&{doublepath}, \&{also}) */
@d of_token 70 /* the operator `\&{of}' */
@d to_token 71 /* the operator `\&{to}' */
@d step_token 72 /* the operator `\&{step}' */
@d until_token 73 /* the operator `\&{until}' */
@d within_token 74 /* the operator `\&{within}' */
@d lig_kern_token 75
  /* the operators `\&{kern}' and `\.{=:}' and `\.{=:\char'174}', etc. */
@d assignment 76 /* the operator `\.{:=}' */
@d skip_to 77 /* the operation `\&{skipto}' */
@d bchar_label 78 /* the operator `\.{\char'174\char'174:}' */
@d double_colon 79 /* the operator `\.{::}' */
@d colon 80 /* the operator `\.:' */
@#
@d comma 81 /* the operator `\.,', must be |colon+1| */
@d end_of_statement (mp->cur_cmd>comma)
@d semicolon 82 /* the operator `\.;', must be |comma+1| */
@d end_group 83 /* end a group (\&{endgroup}), must be |semicolon+1| */
@d stop 84 /* end a job (\&{end}, \&{dump}), must be |end_group+1| */
@d max_command_code stop
@d outer_tag (max_command_code+1) /* protection code added to command code */

@<Types...@>=
typedef int command_code;

@ Variables and capsules in \MP\ have a variety of ``types,''
distinguished by the code numbers defined here. These numbers are also
not completely arbitrary.  Things that get expanded must have types
|>mp_independent|; a type remaining after expansion is numeric if and only if
its code number is at least |numeric_type|; objects containing numeric
parts must have types between |transform_type| and |pair_type|;
all other types must be smaller than |transform_type|; and among the types
that are not unknown or vacuous, the smallest two must be |boolean_type|
and |string_type| in that order.
 
@d undefined 0 /* no type has been declared */
@d unknown_tag 1 /* this constant is added to certain type codes below */
@d unknown_types mp_unknown_boolean: case mp_unknown_string:
  case mp_unknown_pen: case mp_unknown_picture: case mp_unknown_path

@<Types...@>=
enum mp_variable_type {
mp_vacuous = 1, /* no expression was present */
mp_boolean_type, /* \&{boolean} with a known value */
mp_unknown_boolean,
mp_string_type, /* \&{string} with a known value */
mp_unknown_string,
mp_pen_type, /* \&{pen} with a known value */
mp_unknown_pen,
mp_path_type, /* \&{path} with a known value */
mp_unknown_path,
mp_picture_type, /* \&{picture} with a known value */
mp_unknown_picture,
mp_transform_type, /* \&{transform} variable or capsule */
mp_color_type, /* \&{color} variable or capsule */
mp_cmykcolor_type, /* \&{cmykcolor} variable or capsule */
mp_pair_type, /* \&{pair} variable or capsule */
mp_numeric_type, /* variable that has been declared \&{numeric} but not used */
mp_known, /* \&{numeric} with a known value */
mp_dependent, /* a linear combination with |fraction| coefficients */
mp_proto_dependent, /* a linear combination with |scaled| coefficients */
mp_independent, /* \&{numeric} with unknown value */
mp_token_list, /* variable name or suffix argument or text argument */
mp_structured, /* variable with subscripts and attributes */
mp_unsuffixed_macro, /* variable defined with \&{vardef} but no \.{\AT!\#} */
mp_suffixed_macro /* variable defined with \&{vardef} and \.{\AT!\#} */
} ;

@ @<Declarations@>=
static void mp_print_type (MP mp,quarterword t) ;

@ @<Basic printing procedures@>=
void mp_print_type (MP mp,quarterword t) { 
  switch (t) {
  case mp_vacuous:mp_print(mp, "vacuous"); break;
  case mp_boolean_type:mp_print(mp, "boolean"); break;
  case mp_unknown_boolean:mp_print(mp, "unknown boolean"); break;
  case mp_string_type:mp_print(mp, "string"); break;
  case mp_unknown_string:mp_print(mp, "unknown string"); break;
  case mp_pen_type:mp_print(mp, "pen"); break;
  case mp_unknown_pen:mp_print(mp, "unknown pen"); break;
  case mp_path_type:mp_print(mp, "path"); break;
  case mp_unknown_path:mp_print(mp, "unknown path"); break;
  case mp_picture_type:mp_print(mp, "picture"); break;
  case mp_unknown_picture:mp_print(mp, "unknown picture"); break;
  case mp_transform_type:mp_print(mp, "transform"); break;
  case mp_color_type:mp_print(mp, "color"); break;
  case mp_cmykcolor_type:mp_print(mp, "cmykcolor"); break;
  case mp_pair_type:mp_print(mp, "pair"); break;
  case mp_known:mp_print(mp, "known numeric"); break;
  case mp_dependent:mp_print(mp, "dependent"); break;
  case mp_proto_dependent:mp_print(mp, "proto-dependent"); break;
  case mp_numeric_type:mp_print(mp, "numeric"); break;
  case mp_independent:mp_print(mp, "independent"); break;
  case mp_token_list:mp_print(mp, "token list"); break;
  case mp_structured:mp_print(mp, "mp_structured"); break;
  case mp_unsuffixed_macro:mp_print(mp, "unsuffixed macro"); break;
  case mp_suffixed_macro:mp_print(mp, "suffixed macro"); break;
  default: mp_print(mp, "undefined"); break;
  }
}

@ Values inside \MP\ are stored in two-word nodes that have a |name_type|
as well as a |type|. The possibilities for |name_type| are defined
here; they will be explained in more detail later.

@<Types...@>=
enum mp_name_types {
 mp_root=0, /* |name_type| at the top level of a variable */
 mp_saved_root, /* same, when the variable has been saved */
 mp_structured_root, /* |name_type| where a |mp_structured| branch occurs */
 mp_subscr, /* |name_type| in a subscript node */
 mp_attr, /* |name_type| in an attribute node */
 mp_x_part_sector, /* |name_type| in the \&{xpart} of a node */
 mp_y_part_sector, /* |name_type| in the \&{ypart} of a node */
 mp_xx_part_sector, /* |name_type| in the \&{xxpart} of a node */
 mp_xy_part_sector, /* |name_type| in the \&{xypart} of a node */
 mp_yx_part_sector, /* |name_type| in the \&{yxpart} of a node */
 mp_yy_part_sector, /* |name_type| in the \&{yypart} of a node */
 mp_red_part_sector, /* |name_type| in the \&{redpart} of a node */
 mp_green_part_sector, /* |name_type| in the \&{greenpart} of a node */
 mp_blue_part_sector, /* |name_type| in the \&{bluepart} of a node */
 mp_cyan_part_sector, /* |name_type| in the \&{redpart} of a node */
 mp_magenta_part_sector, /* |name_type| in the \&{greenpart} of a node */
 mp_yellow_part_sector, /* |name_type| in the \&{bluepart} of a node */
 mp_black_part_sector, /* |name_type| in the \&{greenpart} of a node */
 mp_grey_part_sector, /* |name_type| in the \&{bluepart} of a node */
 mp_capsule, /* |name_type| in stashed-away subexpressions */
 mp_token  /* |name_type| in a numeric token or string token */
};

@ Primitive operations that produce values have a secondary identification
code in addition to their command code; it's something like genera and species.
For example, `\.*' has the command code |primary_binary|, and its
secondary identification is |times|. The secondary codes start at 30 so that
they don't overlap with the type codes; some type codes (e.g., |mp_string_type|)
are used as operators as well as type identifications.  The relative values
are not critical, except for |true_code..false_code|, |or_op..and_op|,
and |filled_op..bounded_op|.  The restrictions are that
|and_op-false_code=or_op-true_code|, that the ordering of
|x_part...blue_part| must match that of |x_part_sector..mp_blue_part_sector|,
and the ordering of |filled_op..bounded_op| must match that of the code
values they test for.

@d true_code 30 /* operation code for \.{true} */
@d false_code 31 /* operation code for \.{false} */
@d null_picture_code 32 /* operation code for \.{nullpicture} */
@d null_pen_code 33 /* operation code for \.{nullpen} */
@d read_string_op 35 /* operation code for \.{readstring} */
@d pen_circle 36 /* operation code for \.{pencircle} */
@d normal_deviate 37 /* operation code for \.{normaldeviate} */
@d read_from_op 38 /* operation code for \.{readfrom} */
@d close_from_op 39 /* operation code for \.{closefrom} */
@d odd_op 40 /* operation code for \.{odd} */
@d known_op 41 /* operation code for \.{known} */
@d unknown_op 42 /* operation code for \.{unknown} */
@d not_op 43 /* operation code for \.{not} */
@d decimal 44 /* operation code for \.{decimal} */
@d reverse 45 /* operation code for \.{reverse} */
@d make_path_op 46 /* operation code for \.{makepath} */
@d make_pen_op 47 /* operation code for \.{makepen} */
@d oct_op 48 /* operation code for \.{oct} */
@d hex_op 49 /* operation code for \.{hex} */
@d ASCII_op 50 /* operation code for \.{ASCII} */
@d char_op 51 /* operation code for \.{char} */
@d length_op 52 /* operation code for \.{length} */
@d turning_op 53 /* operation code for \.{turningnumber} */
@d color_model_part 54 /* operation code for \.{colormodel} */
@d x_part 55 /* operation code for \.{xpart} */
@d y_part 56 /* operation code for \.{ypart} */
@d xx_part 57 /* operation code for \.{xxpart} */
@d xy_part 58 /* operation code for \.{xypart} */
@d yx_part 59 /* operation code for \.{yxpart} */
@d yy_part 60 /* operation code for \.{yypart} */
@d red_part 61 /* operation code for \.{redpart} */
@d green_part 62 /* operation code for \.{greenpart} */
@d blue_part 63 /* operation code for \.{bluepart} */
@d cyan_part 64 /* operation code for \.{cyanpart} */
@d magenta_part 65 /* operation code for \.{magentapart} */
@d yellow_part 66 /* operation code for \.{yellowpart} */
@d black_part 67 /* operation code for \.{blackpart} */
@d grey_part 68 /* operation code for \.{greypart} */
@d font_part 69 /* operation code for \.{fontpart} */
@d text_part 70 /* operation code for \.{textpart} */
@d path_part 71 /* operation code for \.{pathpart} */
@d pen_part 72 /* operation code for \.{penpart} */
@d dash_part 73 /* operation code for \.{dashpart} */
@d sqrt_op 74 /* operation code for \.{sqrt} */
@d mp_m_exp_op 75 /* operation code for \.{mexp} */
@d mp_m_log_op 76 /* operation code for \.{mlog} */
@d sin_d_op 77 /* operation code for \.{sind} */
@d cos_d_op 78 /* operation code for \.{cosd} */
@d floor_op 79 /* operation code for \.{floor} */
@d uniform_deviate 80 /* operation code for \.{uniformdeviate} */
@d char_exists_op 81 /* operation code for \.{charexists} */
@d font_size 82 /* operation code for \.{fontsize} */
@d ll_corner_op 83 /* operation code for \.{llcorner} */
@d lr_corner_op 84 /* operation code for \.{lrcorner} */
@d ul_corner_op 85 /* operation code for \.{ulcorner} */
@d ur_corner_op 86 /* operation code for \.{urcorner} */
@d arc_length 87 /* operation code for \.{arclength} */
@d angle_op 88 /* operation code for \.{angle} */
@d cycle_op 89 /* operation code for \.{cycle} */
@d filled_op 90 /* operation code for \.{filled} */
@d stroked_op 91 /* operation code for \.{stroked} */
@d textual_op 92 /* operation code for \.{textual} */
@d clipped_op 93 /* operation code for \.{clipped} */
@d bounded_op 94 /* operation code for \.{bounded} */
@d plus 95 /* operation code for \.+ */
@d minus 96 /* operation code for \.- */
@d times 97 /* operation code for \.* */
@d over 98 /* operation code for \./ */
@d pythag_add 99 /* operation code for \.{++} */
@d pythag_sub 100 /* operation code for \.{+-+} */
@d or_op 101 /* operation code for \.{or} */
@d and_op 102 /* operation code for \.{and} */
@d less_than 103 /* operation code for \.< */
@d less_or_equal 104 /* operation code for \.{<=} */
@d greater_than 105 /* operation code for \.> */
@d greater_or_equal 106 /* operation code for \.{>=} */
@d equal_to 107 /* operation code for \.= */
@d unequal_to 108 /* operation code for \.{<>} */
@d concatenate 109 /* operation code for \.\& */
@d rotated_by 110 /* operation code for \.{rotated} */
@d slanted_by 111 /* operation code for \.{slanted} */
@d scaled_by 112 /* operation code for \.{scaled} */
@d shifted_by 113 /* operation code for \.{shifted} */
@d transformed_by 114 /* operation code for \.{transformed} */
@d x_scaled 115 /* operation code for \.{xscaled} */
@d y_scaled 116 /* operation code for \.{yscaled} */
@d z_scaled 117 /* operation code for \.{zscaled} */
@d in_font 118 /* operation code for \.{infont} */
@d intersect 119 /* operation code for \.{intersectiontimes} */
@d double_dot 120 /* operation code for improper \.{..} */
@d substring_of 121 /* operation code for \.{substring} */
@d min_of substring_of
@d subpath_of 122 /* operation code for \.{subpath} */
@d direction_time_of 123 /* operation code for \.{directiontime} */
@d point_of 124 /* operation code for \.{point} */
@d precontrol_of 125 /* operation code for \.{precontrol} */
@d postcontrol_of 126 /* operation code for \.{postcontrol} */
@d pen_offset_of 127 /* operation code for \.{penoffset} */
@d arc_time_of 128 /* operation code for \.{arctime} */
@d mp_version 129 /* operation code for \.{mpversion} */
@d envelope_of 130 /* operation code for \.{envelope} */
@d glyph_infont 131 /* operation code for \.{glyph} */

@c static void mp_print_op (MP mp,quarterword c) { 
  if (c<=mp_numeric_type ) {
    mp_print_type(mp, c);
  } else {
    switch (c) {
    case true_code:mp_print(mp, "true"); break;
    case false_code:mp_print(mp, "false"); break;
    case null_picture_code:mp_print(mp, "nullpicture"); break;
    case null_pen_code:mp_print(mp, "nullpen"); break;
    case read_string_op:mp_print(mp, "readstring"); break;
    case pen_circle:mp_print(mp, "pencircle"); break;
    case normal_deviate:mp_print(mp, "normaldeviate"); break;
    case read_from_op:mp_print(mp, "readfrom"); break;
    case close_from_op:mp_print(mp, "closefrom"); break;
    case odd_op:mp_print(mp, "odd"); break;
    case known_op:mp_print(mp, "known"); break;
    case unknown_op:mp_print(mp, "unknown"); break;
    case not_op:mp_print(mp, "not"); break;
    case decimal:mp_print(mp, "decimal"); break;
    case reverse:mp_print(mp, "reverse"); break;
    case make_path_op:mp_print(mp, "makepath"); break;
    case make_pen_op:mp_print(mp, "makepen"); break;
    case oct_op:mp_print(mp, "oct"); break;
    case hex_op:mp_print(mp, "hex"); break;
    case ASCII_op:mp_print(mp, "ASCII"); break;
    case char_op:mp_print(mp, "char"); break;
    case length_op:mp_print(mp, "length"); break;
    case turning_op:mp_print(mp, "turningnumber"); break;
    case x_part:mp_print(mp, "xpart"); break;
    case y_part:mp_print(mp, "ypart"); break;
    case xx_part:mp_print(mp, "xxpart"); break;
    case xy_part:mp_print(mp, "xypart"); break;
    case yx_part:mp_print(mp, "yxpart"); break;
    case yy_part:mp_print(mp, "yypart"); break;
    case red_part:mp_print(mp, "redpart"); break;
    case green_part:mp_print(mp, "greenpart"); break;
    case blue_part:mp_print(mp, "bluepart"); break;
    case cyan_part:mp_print(mp, "cyanpart"); break;
    case magenta_part:mp_print(mp, "magentapart"); break;
    case yellow_part:mp_print(mp, "yellowpart"); break;
    case black_part:mp_print(mp, "blackpart"); break;
    case grey_part:mp_print(mp, "greypart"); break;
    case color_model_part:mp_print(mp, "colormodel"); break;
    case font_part:mp_print(mp, "fontpart"); break;
    case text_part:mp_print(mp, "textpart"); break;
    case path_part:mp_print(mp, "pathpart"); break;
    case pen_part:mp_print(mp, "penpart"); break;
    case dash_part:mp_print(mp, "dashpart"); break;
    case sqrt_op:mp_print(mp, "sqrt"); break;
    case mp_m_exp_op:mp_print(mp, "mexp"); break;
    case mp_m_log_op:mp_print(mp, "mlog"); break;
    case sin_d_op:mp_print(mp, "sind"); break;
    case cos_d_op:mp_print(mp, "cosd"); break;
    case floor_op:mp_print(mp, "floor"); break;
    case uniform_deviate:mp_print(mp, "uniformdeviate"); break;
    case char_exists_op:mp_print(mp, "charexists"); break;
    case font_size:mp_print(mp, "fontsize"); break;
    case ll_corner_op:mp_print(mp, "llcorner"); break;
    case lr_corner_op:mp_print(mp, "lrcorner"); break;
    case ul_corner_op:mp_print(mp, "ulcorner"); break;
    case ur_corner_op:mp_print(mp, "urcorner"); break;
    case arc_length:mp_print(mp, "arclength"); break;
    case angle_op:mp_print(mp, "angle"); break;
    case cycle_op:mp_print(mp, "cycle"); break;
    case filled_op:mp_print(mp, "filled"); break;
    case stroked_op:mp_print(mp, "stroked"); break;
    case textual_op:mp_print(mp, "textual"); break;
    case clipped_op:mp_print(mp, "clipped"); break;
    case bounded_op:mp_print(mp, "bounded"); break;
    case plus:mp_print_char(mp, xord('+')); break;
    case minus:mp_print_char(mp, xord('-')); break;
    case times:mp_print_char(mp, xord('*')); break;
    case over:mp_print_char(mp, xord('/')); break;
    case pythag_add:mp_print(mp, "++"); break;
    case pythag_sub:mp_print(mp, "+-+"); break;
    case or_op:mp_print(mp, "or"); break;
    case and_op:mp_print(mp, "and"); break;
    case less_than:mp_print_char(mp, xord('<')); break;
    case less_or_equal:mp_print(mp, "<="); break;
    case greater_than:mp_print_char(mp, xord('>')); break;
    case greater_or_equal:mp_print(mp, ">="); break;
    case equal_to:mp_print_char(mp, xord('=')); break;
    case unequal_to:mp_print(mp, "<>"); break;
    case concatenate:mp_print(mp, "&"); break;
    case rotated_by:mp_print(mp, "rotated"); break;
    case slanted_by:mp_print(mp, "slanted"); break;
    case scaled_by:mp_print(mp, "scaled"); break;
    case shifted_by:mp_print(mp, "shifted"); break;
    case transformed_by:mp_print(mp, "transformed"); break;
    case x_scaled:mp_print(mp, "xscaled"); break;
    case y_scaled:mp_print(mp, "yscaled"); break;
    case z_scaled:mp_print(mp, "zscaled"); break;
    case in_font:mp_print(mp, "infont"); break;
    case intersect:mp_print(mp, "intersectiontimes"); break;
    case substring_of:mp_print(mp, "substring"); break;
    case subpath_of:mp_print(mp, "subpath"); break;
    case direction_time_of:mp_print(mp, "directiontime"); break;
    case point_of:mp_print(mp, "point"); break;
    case precontrol_of:mp_print(mp, "precontrol"); break;
    case postcontrol_of:mp_print(mp, "postcontrol"); break;
    case pen_offset_of:mp_print(mp, "penoffset"); break;
    case arc_time_of:mp_print(mp, "arctime"); break;
    case mp_version:mp_print(mp, "mpversion"); break;
    case envelope_of:mp_print(mp, "envelope"); break;
    case glyph_infont:mp_print(mp, "glyph"); break;
    default: mp_print(mp, ".."); break;
    }
  }
}

@ \MP\ also has a bunch of internal parameters that a user might want to
fuss with. Every such parameter has an identifying code number, defined here.

@<Types...@>=
enum mp_given_internal {
  mp_output_template=1, /* a string set up by \&{outputtemplate} */
  mp_output_format, /* the output format set up by \&{outputformat} */
  mp_job_name, /* the perceived jobname, as set up from the options stucture, 
                  the name of the input file, or by \&{jobname}  */
  mp_tracing_titles, /* show titles online when they appear */
  mp_tracing_equations, /* show each variable when it becomes known */
  mp_tracing_capsules, /* show capsules too */
  mp_tracing_choices, /* show the control points chosen for paths */
  mp_tracing_specs, /* show path subdivision prior to filling with polygonal a pen */
  mp_tracing_commands, /* show commands and operations before they are performed */
  mp_tracing_restores, /* show when a variable or internal is restored */
  mp_tracing_macros, /* show macros before they are expanded */
  mp_tracing_output, /* show digitized edges as they are output */
  mp_tracing_stats, /* show memory usage at end of job */
  mp_tracing_lost_chars, /* show characters that aren't \&{infont} */
  mp_tracing_online, /* show long diagnostics on terminal and in the log file */
  mp_year, /* the current year (e.g., 1984) */
  mp_month, /* the current month (e.g., 3 $\equiv$ March) */
  mp_day, /* the current day of the month */
  mp_time, /* the number of minutes past midnight when this job started */
  mp_hour, /* the number of hours past midnight when this job started */
  mp_minute, /* the number of minutes in that hour when this job started */
  mp_char_code, /* the number of the next character to be output */
  mp_char_ext, /* the extension code of the next character to be output */
  mp_char_wd, /* the width of the next character to be output */
  mp_char_ht, /* the height of the next character to be output */
  mp_char_dp, /* the depth of the next character to be output */
  mp_char_ic, /* the italic correction of the next character to be output */
  mp_design_size, /* the unit of measure used for |mp_char_wd..mp_char_ic|, in points */
  mp_pausing, /* positive to display lines on the terminal before they are read */
  mp_showstopping, /* positive to stop after each \&{show} command */
  mp_fontmaking, /* positive if font metric output is to be produced */
  mp_linejoin, /* as in \ps: 0 for mitered, 1 for round, 2 for beveled */
  mp_linecap, /* as in \ps: 0 for butt, 1 for round, 2 for square */
  mp_miterlimit, /* controls miter length as in \ps */
  mp_warning_check, /* controls error message when variable value is large */
  mp_boundary_char, /* the right boundary character for ligatures */
  mp_prologues, /* positive to output conforming PostScript using built-in fonts */
  mp_true_corners, /* positive to make \&{llcorner} etc. ignore \&{setbounds} */
  mp_default_color_model, /* the default color model for unspecified items */
  mp_restore_clip_color,
  mp_procset, /* wether or not create PostScript command shortcuts */
  mp_gtroffmode  /* whether the user specified |-troff| on the command line */
};

@

@d max_given_internal mp_gtroffmode

@<Glob...@>=
scaled *internal;  /* the values of internal quantities */
int *int_type;    /* their types */
char **int_name;  /* their names */
int int_ptr;  /* the maximum internal quantity defined so far */
int max_internal; /* current maximum number of internal quantities */

@ @<Option variables@>=
int troff_mode; 

@ @<Allocate or initialize ...@>=
mp->max_internal=2*max_given_internal;
mp->internal = xmalloc ((mp->max_internal+1), sizeof(scaled));
memset(mp->internal,0,(size_t)(mp->max_internal+1)* sizeof(scaled));
mp->int_name = xmalloc ((mp->max_internal+1), sizeof(char *));
memset(mp->int_name,0,(size_t)(mp->max_internal+1) * sizeof(char *));
mp->int_type = xmalloc ((mp->max_internal+1), sizeof(int));
memset(mp->int_type,0,(size_t)(mp->max_internal+1) * sizeof(int));
{
  int i;
  for (i=1;i<=max_given_internal;i++)
    mp->int_type[i]=mp_known;
}
mp->int_type[mp_output_format]=mp_string_type;
mp->int_type[mp_output_template]=mp_string_type;
mp->int_type[mp_job_name]=mp_string_type;
mp->troff_mode=(opt->troff_mode>0 ? true : false);

@ @<Exported function ...@>=
int mp_troff_mode(MP mp);

@ @c
int mp_troff_mode(MP mp) { return mp->troff_mode; }

@ @<Set initial ...@>=
mp->int_ptr=max_given_internal;

@ The symbolic names for internal quantities are put into \MP's hash table
by using a routine called |primitive|, which will be defined later. Let us
enter them now, so that we don't have to list all those names again
anywhere else.

@<Put each of \MP's primitives into the hash table@>=
mp_primitive(mp, "tracingtitles",internal_quantity,mp_tracing_titles);
@:tracingtitles_}{\&{tracingtitles} primitive@>
mp_primitive(mp, "tracingequations",internal_quantity,mp_tracing_equations);
@:mp_tracing_equations_}{\&{tracingequations} primitive@>
mp_primitive(mp, "tracingcapsules",internal_quantity,mp_tracing_capsules);
@:mp_tracing_capsules_}{\&{tracingcapsules} primitive@>
mp_primitive(mp, "tracingchoices",internal_quantity,mp_tracing_choices);
@:mp_tracing_choices_}{\&{tracingchoices} primitive@>
mp_primitive(mp, "tracingspecs",internal_quantity,mp_tracing_specs);
@:mp_tracing_specs_}{\&{tracingspecs} primitive@>
mp_primitive(mp, "tracingcommands",internal_quantity,mp_tracing_commands);
@:mp_tracing_commands_}{\&{tracingcommands} primitive@>
mp_primitive(mp, "tracingrestores",internal_quantity,mp_tracing_restores);
@:mp_tracing_restores_}{\&{tracingrestores} primitive@>
mp_primitive(mp, "tracingmacros",internal_quantity,mp_tracing_macros);
@:mp_tracing_macros_}{\&{tracingmacros} primitive@>
mp_primitive(mp, "tracingoutput",internal_quantity,mp_tracing_output);
@:mp_tracing_output_}{\&{tracingoutput} primitive@>
mp_primitive(mp, "tracingstats",internal_quantity,mp_tracing_stats);
@:mp_tracing_stats_}{\&{tracingstats} primitive@>
mp_primitive(mp, "tracinglostchars",internal_quantity,mp_tracing_lost_chars);
@:mp_tracing_lost_chars_}{\&{tracinglostchars} primitive@>
mp_primitive(mp, "tracingonline",internal_quantity,mp_tracing_online);
@:mp_tracing_online_}{\&{tracingonline} primitive@>
mp_primitive(mp, "year",internal_quantity,mp_year);
@:mp_year_}{\&{year} primitive@>
mp_primitive(mp, "month",internal_quantity,mp_month);
@:mp_month_}{\&{month} primitive@>
mp_primitive(mp, "day",internal_quantity,mp_day);
@:mp_day_}{\&{day} primitive@>
mp_primitive(mp, "time",internal_quantity,mp_time);
@:time_}{\&{time} primitive@>
mp_primitive(mp, "hour",internal_quantity,mp_hour);
@:hour_}{\&{hour} primitive@>
mp_primitive(mp, "minute",internal_quantity,mp_minute);
@:minute_}{\&{minute} primitive@>
mp_primitive(mp, "charcode",internal_quantity,mp_char_code);
@:mp_char_code_}{\&{charcode} primitive@>
mp_primitive(mp, "charext",internal_quantity,mp_char_ext);
@:mp_char_ext_}{\&{charext} primitive@>
mp_primitive(mp, "charwd",internal_quantity,mp_char_wd);
@:mp_char_wd_}{\&{charwd} primitive@>
mp_primitive(mp, "charht",internal_quantity,mp_char_ht);
@:mp_char_ht_}{\&{charht} primitive@>
mp_primitive(mp, "chardp",internal_quantity,mp_char_dp);
@:mp_char_dp_}{\&{chardp} primitive@>
mp_primitive(mp, "charic",internal_quantity,mp_char_ic);
@:mp_char_ic_}{\&{charic} primitive@>
mp_primitive(mp, "designsize",internal_quantity,mp_design_size);
@:mp_design_size_}{\&{designsize} primitive@>
mp_primitive(mp, "pausing",internal_quantity,mp_pausing);
@:mp_pausing_}{\&{pausing} primitive@>
mp_primitive(mp, "showstopping",internal_quantity,mp_showstopping);
@:mp_showstopping_}{\&{showstopping} primitive@>
mp_primitive(mp, "fontmaking",internal_quantity,mp_fontmaking);
@:mp_fontmaking_}{\&{fontmaking} primitive@>
mp_primitive(mp, "linejoin",internal_quantity,mp_linejoin);
@:mp_linejoin_}{\&{linejoin} primitive@>
mp_primitive(mp, "linecap",internal_quantity,mp_linecap);
@:mp_linecap_}{\&{linecap} primitive@>
mp_primitive(mp, "miterlimit",internal_quantity,mp_miterlimit);
@:mp_miterlimit_}{\&{miterlimit} primitive@>
mp_primitive(mp, "warningcheck",internal_quantity,mp_warning_check);
@:mp_warning_check_}{\&{warningcheck} primitive@>
mp_primitive(mp, "boundarychar",internal_quantity,mp_boundary_char);
@:mp_boundary_char_}{\&{boundarychar} primitive@>
mp_primitive(mp, "prologues",internal_quantity,mp_prologues);
@:mp_prologues_}{\&{prologues} primitive@>
mp_primitive(mp, "truecorners",internal_quantity,mp_true_corners);
@:mp_true_corners_}{\&{truecorners} primitive@>
mp_primitive(mp, "mpprocset",internal_quantity,mp_procset);
@:mp_procset_}{\&{mpprocset} primitive@>
mp_primitive(mp, "troffmode",internal_quantity,mp_gtroffmode);
@:troffmode_}{\&{troffmode} primitive@>
mp_primitive(mp, "defaultcolormodel",internal_quantity,mp_default_color_model);
@:mp_default_color_model_}{\&{defaultcolormodel} primitive@>
mp_primitive(mp, "restoreclipcolor",internal_quantity,mp_restore_clip_color);
@:mp_restore_clip_color_}{\&{restoreclipcolor} primitive@>
mp_primitive(mp, "outputtemplate",internal_quantity,mp_output_template);
@:mp_output_template_}{\&{outputtemplate} primitive@>
mp_primitive(mp, "outputformat",internal_quantity,mp_output_format);
@:mp_output_format_}{\&{outputformat} primitive@>
mp_primitive(mp, "jobname",internal_quantity,mp_job_name);
@:mp_job_name_}{\&{jobname} primitive@>

@ Colors can be specified in four color models. In the special
case of |no_model|, MetaPost does not output any color operator to
the postscript output.

Note: these values are passed directly on to |with_option|. This only
works because the other possible values passed to |with_option| are
8 and 10 respectively (from |with_pen| and |with_picture|).

There is a first state, that is only used for |gs_colormodel|. It flags
the fact that there has not been any kind of color specification by
the user so far in the game.

@(mplib.h@>=
enum mp_color_model {
  mp_no_model=1,
  mp_grey_model=3,
  mp_rgb_model=5,
  mp_cmyk_model=7,
  mp_uninitialized_model=9
};


@ @<Initialize table entries (done by \.{INIMP} only)@>=
mp->internal[mp_default_color_model]=(mp_rgb_model*unity);
mp->internal[mp_restore_clip_color]=unity;
mp->internal[mp_output_template]=intern("%j.%c");
mp->internal[mp_output_format]=intern("eps");

@ Well, we do have to list the names one more time, for use in symbolic
printouts.

@<Initialize table...@>=
mp->int_name[mp_tracing_titles]=xstrdup("tracingtitles");
mp->int_name[mp_tracing_equations]=xstrdup("tracingequations");
mp->int_name[mp_tracing_capsules]=xstrdup("tracingcapsules");
mp->int_name[mp_tracing_choices]=xstrdup("tracingchoices");
mp->int_name[mp_tracing_specs]=xstrdup("tracingspecs");
mp->int_name[mp_tracing_commands]=xstrdup("tracingcommands");
mp->int_name[mp_tracing_restores]=xstrdup("tracingrestores");
mp->int_name[mp_tracing_macros]=xstrdup("tracingmacros");
mp->int_name[mp_tracing_output]=xstrdup("tracingoutput");
mp->int_name[mp_tracing_stats]=xstrdup("tracingstats");
mp->int_name[mp_tracing_lost_chars]=xstrdup("tracinglostchars");
mp->int_name[mp_tracing_online]=xstrdup("tracingonline");
mp->int_name[mp_year]=xstrdup("year");
mp->int_name[mp_month]=xstrdup("month");
mp->int_name[mp_day]=xstrdup("day");
mp->int_name[mp_time]=xstrdup("time");
mp->int_name[mp_hour]=xstrdup("hour");
mp->int_name[mp_minute]=xstrdup("minute");
mp->int_name[mp_char_code]=xstrdup("charcode");
mp->int_name[mp_char_ext]=xstrdup("charext");
mp->int_name[mp_char_wd]=xstrdup("charwd");
mp->int_name[mp_char_ht]=xstrdup("charht");
mp->int_name[mp_char_dp]=xstrdup("chardp");
mp->int_name[mp_char_ic]=xstrdup("charic");
mp->int_name[mp_design_size]=xstrdup("designsize");
mp->int_name[mp_pausing]=xstrdup("pausing");
mp->int_name[mp_showstopping]=xstrdup("showstopping");
mp->int_name[mp_fontmaking]=xstrdup("fontmaking");
mp->int_name[mp_linejoin]=xstrdup("linejoin");
mp->int_name[mp_linecap]=xstrdup("linecap");
mp->int_name[mp_miterlimit]=xstrdup("miterlimit");
mp->int_name[mp_warning_check]=xstrdup("warningcheck");
mp->int_name[mp_boundary_char]=xstrdup("boundarychar");
mp->int_name[mp_prologues]=xstrdup("prologues");
mp->int_name[mp_true_corners]=xstrdup("truecorners");
mp->int_name[mp_default_color_model]=xstrdup("defaultcolormodel");
mp->int_name[mp_procset]=xstrdup("mpprocset");
mp->int_name[mp_gtroffmode]=xstrdup("troffmode");
mp->int_name[mp_restore_clip_color]=xstrdup("restoreclipcolor");
mp->int_name[mp_output_template]=xstrdup("outputtemplate");
mp->int_name[mp_output_format]=xstrdup("outputformat");
mp->int_name[mp_job_name]=xstrdup("jobname");

@ The following procedure, which is called just before \MP\ initializes its
input and output, establishes the initial values of the date and time.
@^system dependencies@>

Note that the values are |scaled| integers. Hence \MP\ can no longer
be used after the year 32767.

@c 
static void mp_fix_date_and_time (MP mp) { 
  time_t aclock = time ((time_t *) 0);
  struct tm *tmptr = localtime (&aclock);
  mp->internal[mp_time]=
      (tmptr->tm_hour*60+tmptr->tm_min)*unity; /* minutes since midnight */
  mp->internal[mp_hour]= (tmptr->tm_hour)*unity; /* hours since midnight */
  mp->internal[mp_minute]= (tmptr->tm_min)*unity; /* minutes since the hour */
  mp->internal[mp_day]=(tmptr->tm_mday)*unity; /* fourth day of the month */
  mp->internal[mp_month]=(tmptr->tm_mon+1)*unity; /* seventh month of the year */
  mp->internal[mp_year]=(tmptr->tm_year+1900)*unity; /* Anno Domini */
}

@ @<Declarations@>=
static void mp_fix_date_and_time (MP mp) ;

@ \MP\ is occasionally supposed to print diagnostic information that
goes only into the transcript file, unless |mp_tracing_online| is positive.
Now that we have defined |mp_tracing_online| we can define
two routines that adjust the destination of print commands:

@<Declarations@>=
static void mp_begin_diagnostic (MP mp) ;
static void mp_end_diagnostic (MP mp,boolean blank_line);
static void mp_print_diagnostic (MP mp, const char *s, const char *t, boolean nuline) ;

@ @<Basic printing...@>=
void mp_begin_diagnostic (MP mp) { /* prepare to do some tracing */
  mp->old_setting=mp->selector;
  if ((mp->internal[mp_tracing_online]<=0)&&(mp->selector==term_and_log)){ 
    decr(mp->selector);
    if ( mp->history==mp_spotless ) mp->history=mp_warning_issued;
  }
}
@#
void mp_end_diagnostic (MP mp,boolean blank_line) {
  /* restore proper conditions after tracing */
  mp_print_nl(mp, "");
  if ( blank_line ) mp_print_ln(mp);
  mp->selector=mp->old_setting;
}

@ 

@<Glob...@>=
unsigned int old_setting;

@ We will occasionally use |begin_diagnostic| in connection with line-number
printing, as follows. (The parameter |s| is typically |"Path"| or
|"Cycle spec"|, etc.)

@<Basic printing...@>=
void mp_print_diagnostic (MP mp, const char *s, const char *t, boolean nuline) { 
  mp_begin_diagnostic(mp);
  if ( nuline ) mp_print_nl(mp, s); else mp_print(mp, s);
  mp_print(mp, " at line "); 
  mp_print_int(mp, mp_true_line(mp));
  mp_print(mp, t); mp_print_char(mp, xord(':'));
}

@ The 256 |ASCII_code| characters are grouped into classes by means of
the |char_class| table. Individual class numbers have no semantic
or syntactic significance, except in a few instances defined here.
There's also |max_class|, which can be used as a basis for additional
class numbers in nonstandard extensions of \MP.

@d digit_class 0 /* the class number of \.{0123456789} */
@d period_class 1 /* the class number of `\..' */
@d space_class 2 /* the class number of spaces and nonstandard characters */
@d percent_class 3 /* the class number of `\.\%' */
@d string_class 4 /* the class number of `\."' */
@d right_paren_class 8 /* the class number of `\.)' */
@d isolated_classes 5: case 6: case 7: case 8 /* characters that make length-one tokens only */
@d letter_class 9 /* letters and the underline character */
@d left_bracket_class 17 /* `\.[' */
@d right_bracket_class 18 /* `\.]' */
@d invalid_class 20 /* bad character in the input */
@d max_class 20 /* the largest class number */

@<Glob...@>=
int char_class[256]; /* the class numbers */

@ If changes are made to accommodate non-ASCII character sets, they should
follow the guidelines in Appendix~C of {\sl The {\logos METAFONT\/}book}.
@:METAFONTbook}{\sl The {\logos METAFONT\/}book@>
@^system dependencies@>

@<Set initial ...@>=
for (k='0';k<='9';k++) 
  mp->char_class[k]=digit_class;
mp->char_class['.']=period_class;
mp->char_class[' ']=space_class;
mp->char_class['%']=percent_class;
mp->char_class['"']=string_class;
mp->char_class[',']=5;
mp->char_class[';']=6;
mp->char_class['(']=7;
mp->char_class[')']=right_paren_class;
for (k='A';k<= 'Z';k++ )
  mp->char_class[k]=letter_class;
for (k='a';k<='z';k++) 
  mp->char_class[k]=letter_class;
mp->char_class['_']=letter_class;
mp->char_class['<']=10;
mp->char_class['=']=10;
mp->char_class['>']=10;
mp->char_class[':']=10;
mp->char_class['|']=10;
mp->char_class['`']=11;
mp->char_class['\'']=11;
mp->char_class['+']=12;
mp->char_class['-']=12;
mp->char_class['/']=13;
mp->char_class['*']=13;
mp->char_class['\\']=13;
mp->char_class['!']=14;
mp->char_class['?']=14;
mp->char_class['#']=15;
mp->char_class['&']=15;
mp->char_class['@@']=15;
mp->char_class['$']=15;
mp->char_class['^']=16;
mp->char_class['~']=16;
mp->char_class['[']=left_bracket_class;
mp->char_class[']']=right_bracket_class;
mp->char_class['{']=19;
mp->char_class['}']=19;
for (k=0;k<' ';k++)
  mp->char_class[k]=invalid_class;
mp->char_class['\t']=space_class;
mp->char_class['\f']=space_class;
for (k=127;k<=255;k++)
  mp->char_class[k]=invalid_class;

@* \[13] The hash table.
Symbolic tokens are stored and retrieved by means of a fairly standard hash
table algorithm called the method of ``coalescing lists'' (cf.\ Algorithm 6.4C
in {\sl The Art of Computer Programming\/}). Once a symbolic token enters the
table, it is never removed.

The actual sequence of characters forming a symbolic token is
stored in the |str_pool| array together with all the other strings. An
auxiliary array |hash| consists of items with two halfword fields per
word. The first of these, called |mp_next(p)|, points to the next identifier
belonging to the same coalesced list as the identifier corresponding to~|p|;
and the other, called |text(p)|, points to the |str_start| entry for
|p|'s identifier. If position~|p| of the hash table is empty, we have
|text(p)=0|; if position |p| is either empty or the end of a coalesced
hash list, we have |mp_next(p)=0|.

An auxiliary pointer variable called |hash_used| is maintained in such a
way that all locations |p>=hash_used| are nonempty. The global variable
|st_count| tells how many symbolic tokens have been defined, if statistics
are being kept.

The first 256 locations of |hash| are reserved for symbols of length one.

There's a parallel array called |eqtb| that contains the current equivalent
values of each symbolic token. The entries of this array consist of
two halfwords called |eq_type| (a command code) and |equiv| (a secondary
piece of information that qualifies the |eq_type|).

@d eq_type(A)   mp->eqtb[(A)].lh /* the current ``meaning'' of a symbolic token */
@d equiv(A)   mp->eqtb[(A)].rh /* parametric part of a token's meaning */
@d hash_is_full   (mp->hash_used==hash_base) /* are all positions occupied? */

@(mpmp.h@>=
#define mp_next(A)   mp->hash[(A)].lh /* link for coalesced lists */
#define text(A)   mp->hash[(A)].rh /* string number for symbolic token name */
#define hash_base 257 /* hashing actually starts here */

@ @<Glob...@>=
pointer hash_used; /* allocation pointer for |hash| */
integer st_count; /* total number of known identifiers */

@ Certain entries in the hash table are ``frozen'' and not redefinable,
since they are used in error recovery.

@(mpmp.h@>=
#define hash_top (integer)(hash_base+mp->hash_size) /* the first location of the frozen area */
#define frozen_inaccessible hash_top /* |hash| location to protect the frozen area */
#define frozen_repeat_loop (hash_top+1) /* |hash| location of a loop-repeat token */
#define frozen_right_delimiter (hash_top+2) /* |hash| location of a permanent `\.)' */
#define frozen_left_bracket (hash_top+3) /* |hash| location of a permanent `\.[' */
#define frozen_slash (hash_top+4) /* |hash| location of a permanent `\./' */
#define frozen_colon (hash_top+5) /* |hash| location of a permanent `\.:' */
#define frozen_semicolon (hash_top+6) /* |hash| location of a permanent `\.;' */
#define frozen_end_for (hash_top+7) /* |hash| location of a permanent \&{endfor} */
#define frozen_end_def (hash_top+8) /* |hash| location of a permanent \&{enddef} */
#define frozen_fi (hash_top+9) /* |hash| location of a permanent \&{fi} */
#define frozen_end_group (hash_top+10) /* |hash| location of a permanent `\.{endgroup}' */
#define frozen_etex (hash_top+11) /* |hash| location of a permanent \&{etex} */
#define frozen_mpx_break (hash_top+12) /* |hash| location of a permanent \&{mpxbreak} */
#define frozen_bad_vardef (hash_top+13) /* |hash| location of `\.{a bad variable}' */
#define frozen_undefined (hash_top+14) /* |hash| location that never gets defined */
#define hash_end (integer)(hash_top+14) /* the actual size of the |hash| and |eqtb| arrays */


@ @<Glob...@>=
two_halves *hash; /* the hash table */
two_halves *eqtb; /* the equivalents */

@ @<Allocate or initialize ...@>=
mp->hash = xmalloc((hash_end+1),sizeof(two_halves));
mp->eqtb = xmalloc((hash_end+1),sizeof(two_halves));

@ @<Dealloc variables@>=
xfree(mp->hash);
xfree(mp->eqtb);

@ @<Set init...@>=
mp_next(1)=0; text(1)=0; eq_type(1)=tag_token; equiv(1)=null;
for (k=2;k<=hash_end;k++)  { 
  mp->hash[k]=mp->hash[1]; mp->eqtb[k]=mp->eqtb[1];
}

@ @<Initialize table entries...@>=
mp->hash_used=frozen_inaccessible; /* nothing is used */
mp->st_count=0;
text(frozen_bad_vardef)=intern("a bad variable");
text(frozen_etex)=intern("etex");
text(frozen_mpx_break)=intern("mpxbreak");
text(frozen_fi)=intern("fi");
text(frozen_end_group)=intern("endgroup");
text(frozen_end_def)=intern("enddef");
text(frozen_end_for)=intern("endfor");
text(frozen_semicolon)=intern(";");
text(frozen_colon)=intern(":");
text(frozen_slash)=intern("/");
text(frozen_left_bracket)=intern("[");
text(frozen_right_delimiter)=intern(")");
text(frozen_inaccessible)=intern(" INACCESSIBLE");
eq_type(frozen_right_delimiter)=right_delimiter;

@ @<Check the ``constant'' values...@>=
if ( hash_end+mp->max_internal>max_halfword ) mp->bad=17;

@ The value of |hash_prime| should be roughly 85\pct! of |hash_size|, and it
should be a prime number.  The theory of hashing tells us to expect fewer
than two table probes, on the average, when the search is successful.
[See J.~S. Vitter, {\sl Journal of the ACM\/ \bf30} (1983), 231--258.]
@^Vitter, Jeffrey Scott@>

@c
static integer mp_compute_hash (MP mp, const char *s, int l) {
  integer k;
  integer h = *s;
  for (k=1;k<l;k++){ 
    h=h+h+(*(s+k));
    while ( h>=mp->hash_prime ) h=h-mp->hash_prime;
  }
  return h;
}


@ Here is the subroutine that searches the hash table for an identifier
that matches a given string of length~|l| appearing in |buffer[j..
(j+l-1)]|. If the identifier is not found, it is inserted; hence it
will always be found, and the corresponding hash table address
will be returned.

@c 
static pointer mp_id_lookup (MP mp,integer j, integer l) { /* search the hash table */
  integer h; /* hash code */
  pointer p; /* index in |hash| array */
  pointer k; /* index in |buffer| array */
  if (l==1) {
    @<Treat special case of length 1 and |break|@>;
  }
  @<Compute the hash code |h|@>;
  p=h+hash_base; /* we start searching here; note that |0<=h<hash_prime| */
  while (true)  { 
	if (text(p)>0 && length(text(p))==l && mp_str_eq_buf(mp, text(p),j)) 
      break;
    if ( mp_next(p)==0 ) {
      @<Insert a new symbolic token after |p|, then
        make |p| point to it and |break|@>;
    }
    p=mp_next(p);
  }
  return p;
}

@ @<Treat special case of length 1...@>=
 p=mp->buffer[j]+1; text(p)=p-1; return p;


@ @<Insert a new symbolic...@>=
{
if ( text(p)>0 ) { 
  do {  
    if ( hash_is_full )
      mp_overflow(mp, "hash size",(integer)mp->hash_size);
@:MetaPost capacity exceeded hash size}{\quad hash size@>
    decr(mp->hash_used);
  } while (text(mp->hash_used)!=0); /* search for an empty location in |hash| */
  mp_next(p)=mp->hash_used; 
  p=mp->hash_used;
}
str_room(l);
for (k=j;k<j+l;k++) {
  append_char(mp->buffer[k]);
}
text(p)=mp_make_string(mp); 
mp->str_ref[text(p)]=max_str_ref;
incr(mp->st_count);
break;
}

@  @<Compute the hash code |h|@>=
h=mp_compute_hash(mp, (char *)mp->buffer+j, l)


@ @<Search |eqtb| for equivalents equal to |p|@>=
for (q=1;q<=hash_end;q++) { 
  if ( equiv(q)==p ) { 
    mp_print_nl(mp, "EQUIV("); 
    mp_print_int(mp, q); 
    mp_print_char(mp, xord(')'));
  }
}

@ We need to put \MP's ``primitive'' symbolic tokens into the hash
table, together with their command code (which will be the |eq_type|)
and an operand (which will be the |equiv|). The |primitive| procedure
does this, in a way that no \MP\ user can. The global value |cur_sym|
contains the new |eqtb| pointer after |primitive| has acted.

@c 
static void mp_primitive (MP mp, const char *ss, halfword c, halfword o) {
  pool_pointer k; /* index into |str_pool| */
  quarterword j; /* index into |buffer| */
  quarterword l; /* length of the string */
  str_number s;
  s = intern(ss);
  k=mp->str_start[s]; l=(quarterword)(str_stop(s)-k);
  /* we will move |s| into the (empty) |buffer| */
  for (j=0;j<=l-1;j++) {
    mp->buffer[j]=mp->str_pool[k+j];
  }
  mp->cur_sym=mp_id_lookup(mp, 0,l);
  if ( s>=256 ) { /* we don't want to have the string twice */
    mp_flush_string(mp, text(mp->cur_sym)); text(mp->cur_sym)=s;
  };
  eq_type(mp->cur_sym)=c; 
  equiv(mp->cur_sym)=o;
}


@ Many of \MP's primitives need no |equiv|, since they are identifiable
by their |eq_type| alone. These primitives are loaded into the hash table
as follows:

@<Put each of \MP's primitives into the hash table@>=
mp_primitive(mp, "..",path_join,0);
@:.._}{\.{..} primitive@>
mp_primitive(mp, "[",left_bracket,0); mp->eqtb[frozen_left_bracket]=mp->eqtb[mp->cur_sym];
@:[ }{\.{[} primitive@>
mp_primitive(mp, "]",right_bracket,0);
@:] }{\.{]} primitive@>
mp_primitive(mp, "}",right_brace,0);
@:]]}{\.{\char`\}} primitive@>
mp_primitive(mp, "{",left_brace,0);
@:][}{\.{\char`\{} primitive@>
mp_primitive(mp, ":",colon,0); mp->eqtb[frozen_colon]=mp->eqtb[mp->cur_sym];
@:: }{\.{:} primitive@>
mp_primitive(mp, "::",double_colon,0);
@::: }{\.{::} primitive@>
mp_primitive(mp, "||:",bchar_label,0);
@:::: }{\.{\char'174\char'174:} primitive@>
mp_primitive(mp, ":=",assignment,0);
@::=_}{\.{:=} primitive@>
mp_primitive(mp, ",",comma,0);
@:, }{\., primitive@>
mp_primitive(mp, ";",semicolon,0); mp->eqtb[frozen_semicolon]=mp->eqtb[mp->cur_sym];
@:; }{\.; primitive@>
mp_primitive(mp, "\\",relax,0);
@:]]\\}{\.{\char`\\} primitive@>
@#
mp_primitive(mp, "addto",add_to_command,0);
@:add_to_}{\&{addto} primitive@>
mp_primitive(mp, "atleast",at_least,0);
@:at_least_}{\&{atleast} primitive@>
mp_primitive(mp, "begingroup",begin_group,0); mp->bg_loc=mp->cur_sym;
@:begin_group_}{\&{begingroup} primitive@>
mp_primitive(mp, "controls",controls,0);
@:controls_}{\&{controls} primitive@>
mp_primitive(mp, "curl",curl_command,0);
@:curl_}{\&{curl} primitive@>
mp_primitive(mp, "delimiters",delimiters,0);
@:delimiters_}{\&{delimiters} primitive@>
mp_primitive(mp, "endgroup",end_group,0);
 mp->eqtb[frozen_end_group]=mp->eqtb[mp->cur_sym]; mp->eg_loc=mp->cur_sym;
@:endgroup_}{\&{endgroup} primitive@>
mp_primitive(mp, "everyjob",every_job_command,0);
@:every_job_}{\&{everyjob} primitive@>
mp_primitive(mp, "exitif",exit_test,0);
@:exit_if_}{\&{exitif} primitive@>
mp_primitive(mp, "expandafter",expand_after,0);
@:expand_after_}{\&{expandafter} primitive@>
mp_primitive(mp, "interim",interim_command,0);
@:interim_}{\&{interim} primitive@>
mp_primitive(mp, "let",let_command,0);
@:let_}{\&{let} primitive@>
mp_primitive(mp, "newinternal",new_internal,0);
@:new_internal_}{\&{newinternal} primitive@>
mp_primitive(mp, "of",of_token,0);
@:of_}{\&{of} primitive@>
mp_primitive(mp, "randomseed",mp_random_seed,0);
@:mp_random_seed_}{\&{randomseed} primitive@>
mp_primitive(mp, "save",save_command,0);
@:save_}{\&{save} primitive@>
mp_primitive(mp, "scantokens",scan_tokens,0);
@:scan_tokens_}{\&{scantokens} primitive@>
mp_primitive(mp, "shipout",ship_out_command,0);
@:ship_out_}{\&{shipout} primitive@>
mp_primitive(mp, "skipto",skip_to,0);
@:skip_to_}{\&{skipto} primitive@>
mp_primitive(mp, "special",special_command,0);
@:special}{\&{special} primitive@>
mp_primitive(mp, "fontmapfile",special_command,1);
@:fontmapfile}{\&{fontmapfile} primitive@>
mp_primitive(mp, "fontmapline",special_command,2);
@:fontmapline}{\&{fontmapline} primitive@>
mp_primitive(mp, "step",step_token,0);
@:step_}{\&{step} primitive@>
mp_primitive(mp, "str",str_op,0);
@:str_}{\&{str} primitive@>
mp_primitive(mp, "tension",tension,0);
@:tension_}{\&{tension} primitive@>
mp_primitive(mp, "to",to_token,0);
@:to_}{\&{to} primitive@>
mp_primitive(mp, "until",until_token,0);
@:until_}{\&{until} primitive@>
mp_primitive(mp, "within",within_token,0);
@:within_}{\&{within} primitive@>
mp_primitive(mp, "write",write_command,0);
@:write_}{\&{write} primitive@>

@ Each primitive has a corresponding inverse, so that it is possible to
display the cryptic numeric contents of |eqtb| in symbolic form.
Every call of |primitive| in this program is therefore accompanied by some
straightforward code that forms part of the |print_cmd_mod| routine
explained below.

@<Cases of |print_cmd_mod| for symbolic printing of primitives@>=
case add_to_command:mp_print(mp, "addto"); break;
case assignment:mp_print(mp, ":="); break;
case at_least:mp_print(mp, "atleast"); break;
case bchar_label:mp_print(mp, "||:"); break;
case begin_group:mp_print(mp, "begingroup"); break;
case colon:mp_print(mp, ":"); break;
case comma:mp_print(mp, ","); break;
case controls:mp_print(mp, "controls"); break;
case curl_command:mp_print(mp, "curl"); break;
case delimiters:mp_print(mp, "delimiters"); break;
case double_colon:mp_print(mp, "::"); break;
case end_group:mp_print(mp, "endgroup"); break;
case every_job_command:mp_print(mp, "everyjob"); break;
case exit_test:mp_print(mp, "exitif"); break;
case expand_after:mp_print(mp, "expandafter"); break;
case interim_command:mp_print(mp, "interim"); break;
case left_brace:mp_print(mp, "{"); break;
case left_bracket:mp_print(mp, "["); break;
case let_command:mp_print(mp, "let"); break;
case new_internal:mp_print(mp, "newinternal"); break;
case of_token:mp_print(mp, "of"); break;
case path_join:mp_print(mp, ".."); break;
case mp_random_seed:mp_print(mp, "randomseed"); break;
case relax:mp_print_char(mp, xord('\\')); break;
case right_brace:mp_print_char(mp, xord('}')); break;
case right_bracket:mp_print_char(mp, xord(']')); break;
case save_command:mp_print(mp, "save"); break;
case scan_tokens:mp_print(mp, "scantokens"); break;
case semicolon:mp_print_char(mp, xord(';')); break;
case ship_out_command:mp_print(mp, "shipout"); break;
case skip_to:mp_print(mp, "skipto"); break;
case special_command: if ( m==2 ) mp_print(mp, "fontmapline"); else
                 if ( m==1 ) mp_print(mp, "fontmapfile"); else
                 mp_print(mp, "special"); break;
case step_token:mp_print(mp, "step"); break;
case str_op:mp_print(mp, "str"); break;
case tension:mp_print(mp, "tension"); break;
case to_token:mp_print(mp, "to"); break;
case until_token:mp_print(mp, "until"); break;
case within_token:mp_print(mp, "within"); break;
case write_command:mp_print(mp, "write"); break;

@ We will deal with the other primitives later, at some point in the program
where their |eq_type| and |equiv| values are more meaningful.  For example,
the primitives for macro definitions will be loaded when we consider the
routines that define macros.
It is easy to find where each particular
primitive was treated by looking in the index at the end; for example, the
section where |"def"| entered |eqtb| is listed under `\&{def} primitive'.

@* \[14] Token lists.
A \MP\ token is either symbolic or numeric or a string, or it denotes
a macro parameter or capsule; so there are five corresponding ways to encode it
@^token@>
internally: (1)~A symbolic token whose hash code is~|p|
is represented by the number |p|, in the |info| field of a single-word
node in~|mem|. (2)~A numeric token whose |scaled| value is~|v| is
represented in a two-word node of~|mem|; the |type| field is |known|,
the |name_type| field is |token|, and the |value| field holds~|v|.
The fact that this token appears in a two-word node rather than a
one-word node is, of course, clear from the node address.
(3)~A string token is also represented in a two-word node; the |type|
field is |mp_string_type|, the |name_type| field is |token|, and the
|value| field holds the corresponding |str_number|.  (4)~Capsules have
|name_type=capsule|, and their |type| and |value| fields represent
arbitrary values (in ways to be explained later).  (5)~Macro parameters
are like symbolic tokens in that they appear in |info| fields of
one-word nodes. The $k$th parameter is represented by |expr_base+k| if it
is of type \&{expr}, or by |suffix_base+k| if it is of type \&{suffix}, or
by |text_base+k| if it is of type \&{text}.  (Here |0<=k<param_size|.)
Actual values of these parameters are kept in a separate stack, as we will
see later.  The constants |expr_base|, |suffix_base|, and |text_base| are,
of course, chosen so that there will be no confusion between symbolic
tokens and parameters of various types.

Note that
the `\\{type}' field of a node has nothing to do with ``type'' in a
printer's sense. It's curious that the same word is used in such different ways.

@d mp_type(A)     mp->mem[(A)].hh.b0 /* identifies what kind of value this is */
@d mp_name_type(A)   mp->mem[(A)].hh.b1 /* a clue to the name of this value */
@d token_node_size 2 /* the number of words in a large token node */
@d value_loc(A) ((A)+1) /* the word that contains the |value| field */
@d value(A) mp->mem[value_loc((A))].cint /* the value stored in a large token node */
@d expr_base (hash_end+1) /* code for the zeroth \&{expr} parameter */
@d suffix_base (expr_base+mp->param_size) /* code for the zeroth \&{suffix} parameter */
@d text_base (suffix_base+mp->param_size) /* code for the zeroth \&{text} parameter */

@<Check the ``constant''...@>=
if ( text_base+mp->param_size>max_halfword ) mp->bad=18;

@ We have set aside a two word node beginning at |null| so that we can have
|value(null)=0|.  We will make use of this coincidence later.

@<Initialize table entries...@>=
mp_link(null)=null; value(null)=0;

@ A numeric token is created by the following trivial routine.

@c 
static pointer mp_new_num_tok (MP mp,scaled v) {
  pointer p; /* the new node */
  p=mp_get_node(mp, token_node_size); value(p)=v;
  mp_type(p)=mp_known; mp_name_type(p)=mp_token; 
  return p;
}

@ A token list is a singly linked list of nodes in |mem|, where
each node contains a token and a link.  Here's a subroutine that gets rid
of a token list when it is no longer needed.

@c static void mp_flush_token_list (MP mp,pointer p) {
  pointer q; /* the node being recycled */
  while ( p!=null ) { 
    q=p; p=mp_link(p);
    if ( q>=mp->hi_mem_min ) {
     free_avail(q);
    } else { 
      switch (mp_type(q)) {
      case mp_vacuous: case mp_boolean_type: case mp_known:
        break;
      case mp_string_type:
        delete_str_ref(value(q));
        break;
      case unknown_types: case mp_pen_type: case mp_path_type: 
      case mp_picture_type: case mp_pair_type: case mp_color_type:
      case mp_cmykcolor_type: case mp_transform_type: case mp_dependent:
      case mp_proto_dependent: case mp_independent:
        mp_recycle_value(mp,q);
        break;
      default: mp_confusion(mp, "token");
@:this can't happen token}{\quad token@>
      }
      mp_free_node(mp, q,token_node_size);
    }
  }
}

@ The procedure |show_token_list|, which prints a symbolic form of
the token list that starts at a given node |p|, illustrates these
conventions. The token list being displayed should not begin with a reference
count. However, the procedure is intended to be fairly robust, so that if the
memory links are awry or if |p| is not really a pointer to a token list,
almost nothing catastrophic can happen.

An additional parameter |q| is also given; this parameter is either null
or it points to a node in the token list where a certain magic computation
takes place that will be explained later. (Basically, |q| is non-null when
we are printing the two-line context information at the time of an error
message; |q| marks the place corresponding to where the second line
should begin.)

The generation will stop, and `\.{\char`\ ETC.}' will be printed, if the length
of printing exceeds a given limit~|l|; the length of printing upon entry is
assumed to be a given amount called |null_tally|. (Note that
|show_token_list| sometimes uses itself recursively to print
variable names within a capsule.)
@^recursion@>

Unusual entries are printed in the form of all-caps tokens
preceded by a space, e.g., `\.{\char`\ BAD}'.

@<Declarations@>=
static void mp_show_token_list (MP mp, integer p, integer q, integer l,
                         integer null_tally) ;

@ @c
void mp_show_token_list (MP mp, integer p, integer q, integer l,
                         integer null_tally) {
  quarterword class,c; /* the |char_class| of previous and new tokens */
  integer r,v; /* temporary registers */
  class=percent_class;
  mp->tally=null_tally;
  while ( (p!=null) && (mp->tally<l) ) { 
    if ( p==q ) 
      @<Do magic computation@>;
    @<Display token |p| and set |c| to its class;
      but |return| if there are problems@>;
    class=c; p=mp_link(p);
  }
  if ( p!=null ) 
     mp_print(mp, " ETC.");
@.ETC@>
  return;
}

@ @<Display token |p| and set |c| to its class...@>=
c=letter_class; /* the default */
if ( (p<0)||(p>mp->mem_end) ) { 
  mp_print(mp, " CLOBBERED"); return;
@.CLOBBERED@>
}
if ( p<mp->hi_mem_min ) { 
  @<Display two-word token@>;
} else { 
  r=mp_info(p);
  if ( r>=expr_base ) {
     @<Display a parameter token@>;
  } else {
    if ( r<1 ) {
      if ( r==0 ) { 
        @<Display a collective subscript@>
      } else {
        mp_print(mp, " IMPOSSIBLE");
@.IMPOSSIBLE@>
      }
    } else { 
      r=text(r);
      if ( (r<0)||(r>mp->max_str_ptr) ) {
        mp_print(mp, " NONEXISTENT");
@.NONEXISTENT@>
      } else {
       @<Print string |r| as a symbolic token
        and set |c| to its class@>;
      }
    }
  }
}

@ @<Display two-word token@>=
if ( mp_name_type(p)==mp_token ) {
  if ( mp_type(p)==mp_known ) {
    @<Display a numeric token@>;
  } else if ( mp_type(p)!=mp_string_type ) {
    mp_print(mp, " BAD");
@.BAD@>
  } else { 
    mp_print_char(mp, xord('"')); mp_print_str(mp, value(p)); mp_print_char(mp, xord('"'));
    c=string_class;
  }
} else if ((mp_name_type(p)!=mp_capsule)||(mp_type(p)<mp_vacuous)||(mp_type(p)>mp_independent) ) {
  mp_print(mp, " BAD");
} else { 
  mp_print_capsule(mp,p); c=right_paren_class;
}

@ @<Display a numeric token@>=
if ( class==digit_class ) 
  mp_print_char(mp, xord(' '));
v=value(p);
if ( v<0 ){ 
  if ( class==left_bracket_class ) 
    mp_print_char(mp, xord(' '));
  mp_print_char(mp, xord('[')); mp_print_scaled(mp, v); mp_print_char(mp, xord(']'));
  c=right_bracket_class;
} else { 
  mp_print_scaled(mp, v); c=digit_class;
}


@ Strictly speaking, a genuine token will never have |mp_info(p)=0|.
But we will see later (in the |print_variable_name| routine) that
it is convenient to let |mp_info(p)=0| stand for `\.{[]}'.

@<Display a collective subscript@>=
{
if ( class==left_bracket_class ) 
  mp_print_char(mp, xord(' '));
mp_print(mp, "[]"); c=right_bracket_class;
}

@ @<Display a parameter token@>=
{
if ( r<suffix_base ) { 
  mp_print(mp, "(EXPR"); r=r-(expr_base);
@.EXPR@>
} else if ( r<text_base ) { 
  mp_print(mp, "(SUFFIX"); r=r-(suffix_base);
@.SUFFIX@>
} else { 
  mp_print(mp, "(TEXT"); r=r-(text_base);
@.TEXT@>
}
mp_print_int(mp, r); mp_print_char(mp, xord(')')); c=right_paren_class;
}


@ @<Print string |r| as a symbolic token...@>=
{ 
c=(quarterword)mp->char_class[mp->str_pool[mp->str_start[r]]];
if ( c==class ) {
  switch (c) {
  case letter_class:mp_print_char(mp, xord('.')); break;
  case isolated_classes: break;
  default: mp_print_char(mp, xord(' ')); break;
  }
}
mp_print_str(mp, r);
}

@ @<Declarations@>=
static void mp_print_capsule (MP mp, pointer p);

@ @<Declare miscellaneous procedures that were declared |forward|@>=
void mp_print_capsule (MP mp, pointer p) { 
  mp_print_char(mp, xord('(')); mp_print_exp(mp,p,0); mp_print_char(mp, xord(')'));
}

@ Macro definitions are kept in \MP's memory in the form of token lists
that have a few extra one-word nodes at the beginning.

The first node contains a reference count that is used to tell when the
list is no longer needed. To emphasize the fact that a reference count is
present, we shall refer to the |info| field of this special node as the
|ref_count| field.
@^reference counts@>

The next node or nodes after the reference count serve to describe the
formal parameters. They consist of zero or more parameter tokens followed
by a code for the type of macro.

@d ref_count mp_info
  /* reference count preceding a macro definition or picture header */
@d add_mac_ref(A) incr(ref_count((A))) /* make a new reference to a macro list */
@d general_macro 0 /* preface to a macro defined with a parameter list */
@d primary_macro 1 /* preface to a macro with a \&{primary} parameter */
@d secondary_macro 2 /* preface to a macro with a \&{secondary} parameter */
@d tertiary_macro 3 /* preface to a macro with a \&{tertiary} parameter */
@d expr_macro 4 /* preface to a macro with an undelimited \&{expr} parameter */
@d of_macro 5 /* preface to a macro with
  undelimited `\&{expr} |x| \&{of}~|y|' parameters */
@d suffix_macro 6 /* preface to a macro with an undelimited \&{suffix} parameter */
@d text_macro 7 /* preface to a macro with an undelimited \&{text} parameter */

@c 
static void mp_delete_mac_ref (MP mp,pointer p) {
  /* |p| points to the reference count of a macro list that is
    losing one reference */
  if ( ref_count(p)==null ) mp_flush_token_list(mp, p);
  else decr(ref_count(p));
}

@ The following subroutine displays a macro, given a pointer to its
reference count.

@c 
static void mp_show_macro (MP mp, pointer p, integer q, integer l) {
  pointer r; /* temporary storage */
  p=mp_link(p); /* bypass the reference count */
  while ( mp_info(p)>text_macro ){ 
    r=mp_link(p); mp_link(p)=null;
    mp_show_token_list(mp, p,null,l,0); mp_link(p)=r; p=r;
    if ( l>0 ) l=l-mp->tally; else return;
  } /* control printing of `\.{ETC.}' */
@.ETC@>
  mp->tally=0;
  switch(mp_info(p)) {
  case general_macro:mp_print(mp, "->"); break;
@.->@>
  case primary_macro: case secondary_macro: case tertiary_macro:
    mp_print_char(mp, xord('<'));
    mp_print_cmd_mod(mp, param_type,mp_info(p)); 
    mp_print(mp, ">->");
    break;
  case expr_macro:mp_print(mp, "<expr>->"); break;
  case of_macro:mp_print(mp, "<expr>of<primary>->"); break;
  case suffix_macro:mp_print(mp, "<suffix>->"); break;
  case text_macro:mp_print(mp, "<text>->"); break;
  } /* there are no other cases */
  mp_show_token_list(mp, mp_link(p),q,l-mp->tally,0);
}

@* \[15] Data structures for variables.
The variables of \MP\ programs can be simple, like `\.x', or they can
combine the structural properties of arrays and records, like `\.{x20a.b}'.
A \MP\ user assigns a type to a variable like \.{x20a.b} by saying, for
example, `\.{boolean} \.{x[]a.b}'. It's time for us to study how such
things are represented inside of the computer.

Each variable value occupies two consecutive words, either in a two-word
node called a value node, or as a two-word subfield of a larger node.  One
of those two words is called the |value| field; it is an integer,
containing either a |scaled| numeric value or the representation of some
other type of quantity. (It might also be subdivided into halfwords, in
which case it is referred to by other names instead of |value|.) The other
word is broken into subfields called |type|, |name_type|, and |link|.  The
|type| field is a quarterword that specifies the variable's type, and
|name_type| is a quarterword from which \MP\ can reconstruct the
variable's name (sometimes by using the |link| field as well).  Thus, only
1.25 words are actually devoted to the value itself; the other
three-quarters of a word are overhead, but they aren't wasted because they
allow \MP\ to deal with sparse arrays and to provide meaningful diagnostics.

In this section we shall be concerned only with the structural aspects of
variables, not their values. Later parts of the program will change the
|type| and |value| fields, but we shall treat those fields as black boxes
whose contents should not be touched.

However, if the |type| field is |mp_structured|, there is no |value| field,
and the second word is broken into two pointer fields called |attr_head|
and |subscr_head|. Those fields point to additional nodes that
contain structural information, as we shall see.

@d subscr_head_loc(A)   (A)+1 /* where |value|, |subscr_head| and |attr_head| are */
@d attr_head(A)   mp_info(subscr_head_loc((A))) /* pointer to attribute info */
@d subscr_head(A)   mp_link(subscr_head_loc((A))) /* pointer to subscript info */
@d value_node_size 2 /* the number of words in a value node */

@ An attribute node is three words long. Two of these words contain |type|
and |value| fields as described above, and the third word contains
additional information:  There is an |attr_loc| field, which contains the
hash address of the token that names this attribute; and there's also a
|parent| field, which points to the value node of |mp_structured| type at the
next higher level (i.e., at the level to which this attribute is
subsidiary).  The |name_type| in an attribute node is `|attr|'.  The
|link| field points to the next attribute with the same parent; these are
arranged in increasing order, so that |attr_loc(mp_link(p))>attr_loc(p)|. The
final attribute node links to the constant |end_attr|, whose |attr_loc|
field is greater than any legal hash address. The |attr_head| in the
parent points to a node whose |name_type| is |mp_structured_root|; this
node represents the null attribute, i.e., the variable that is relevant
when no attributes are attached to the parent. The |attr_head| node
has the fields of either
a value node, a subscript node, or an attribute node, depending on what
the parent would be if it were not structured; but the subscript and
attribute fields are ignored, so it effectively contains only the data of
a value node. The |link| field in this special node points to an attribute
node whose |attr_loc| field is zero; the latter node represents a collective
subscript `\.{[]}' attached to the parent, and its |link| field points to
the first non-special attribute node (or to |end_attr| if there are none).

A subscript node likewise occupies three words, with |type| and |value| fields
plus extra information; its |name_type| is |subscr|. In this case the
third word is called the |subscript| field, which is a |scaled| integer.
The |link| field points to the subscript node with the next larger
subscript, if any; otherwise the |link| points to the attribute node
for collective subscripts at this level. We have seen that the latter node
contains an upward pointer, so that the parent can be deduced.

The |name_type| in a parent-less value node is |root|, and the |link|
is the hash address of the token that names this value.

In other words, variables have a hierarchical structure that includes
enough threads running around so that the program is able to move easily
between siblings, parents, and children. An example should be helpful:
(The reader is advised to draw a picture while reading the following
description, since that will help to firm up the ideas.)
Suppose that `\.x' and `\.{x.a}' and `\.{x[]b}' and `\.{x5}'
and `\.{x20b}' have been mentioned in a user's program, where
\.{x[]b} has been declared to be of \&{boolean} type. Let |h(x)|, |h(a)|,
and |h(b)| be the hash addresses of \.x, \.a, and~\.b. Then
|eq_type(h(x))=name| and |equiv(h(x))=p|, where |p|~is a two-word value
node with |mp_name_type(p)=root| and |mp_link(p)=h(x)|. We have |type(p)=mp_structured|,
|attr_head(p)=q|, and |subscr_head(p)=r|, where |q| points to a value
node and |r| to a subscript node. (Are you still following this? Use
a pencil to draw a diagram.) The lone variable `\.x' is represented by
|type(q)| and |value(q)|; furthermore
|mp_name_type(q)=mp_structured_root| and |mp_link(q)=q1|, where |q1| points
to an attribute node representing `\.{x[]}'. Thus |mp_name_type(q1)=attr|,
|attr_loc(q1)=collective_subscript=0|, |parent(q1)=p|,
|type(q1)=mp_structured|, |attr_head(q1)=qq|, and |subscr_head(q1)=qq1|;
|qq| is a  three-word ``attribute-as-value'' node with |type(qq)=numeric_type|
(assuming that \.{x5} is numeric, because |qq| represents `\.{x[]}' 
with no further attributes), |mp_name_type(qq)=structured_root|, 
|attr_loc(qq)=0|, |parent(qq)=p|, and
|mp_link(qq)=qq1|. (Now pay attention to the next part.) Node |qq1| is
an attribute node representing `\.{x[][]}', which has never yet
occurred; its |type| field is |undefined|, and its |value| field is
undefined. We have |mp_name_type(qq1)=attr|, |attr_loc(qq1)=collective_subscript|,
|parent(qq1)=q1|, and |mp_link(qq1)=qq2|. Since |qq2| represents
`\.{x[]b}', |type(qq2)=mp_unknown_boolean|; also |attr_loc(qq2)=h(b)|,
|parent(qq2)=q1|, |mp_name_type(qq2)=attr|, |mp_link(qq2)=end_attr|.
(Maybe colored lines will help untangle your picture.)
 Node |r| is a subscript node with |type| and |value|
representing `\.{x5}'; |mp_name_type(r)=subscr|, |subscript(r)=5.0|,
and |mp_link(r)=r1| is another subscript node. To complete the picture,
see if you can guess what |mp_link(r1)| is; give up? It's~|q1|.
Furthermore |subscript(r1)=20.0|, |mp_name_type(r1)=subscr|,
|type(r1)=mp_structured|, |attr_head(r1)=qqq|, |subscr_head(r1)=qqq1|,
and we finish things off with three more nodes
|qqq|, |qqq1|, and |qqq2| hung onto~|r1|. (Perhaps you should start again
with a larger sheet of paper.) The value of variable \.{x20b}
appears in node~|qqq2|, as you can well imagine.

If the example in the previous paragraph doesn't make things crystal
clear, a glance at some of the simpler subroutines below will reveal how
things work out in practice.

The only really unusual thing about these conventions is the use of
collective subscript attributes. The idea is to avoid repeating a lot of
type information when many elements of an array are identical macros
(for which distinct values need not be stored) or when they don't have
all of the possible attributes. Branches of the structure below collective
subscript attributes do not carry actual values except for macro identifiers;
branches of the structure below subscript nodes do not carry significant
information in their collective subscript attributes.

@d attr_loc_loc(A) ((A)+2) /* where the |attr_loc| and |parent| fields are */
@d attr_loc(A) mp_info(attr_loc_loc((A))) /* hash address of this attribute */
@d parent(A) mp_link(attr_loc_loc((A))) /* pointer to |mp_structured| variable */
@d subscript_loc(A) ((A)+2) /* where the |subscript| field lives */
@d subscript(A) mp->mem[subscript_loc((A))].sc /* subscript of this variable */
@d attr_node_size 3 /* the number of words in an attribute node */
@d subscr_node_size 3 /* the number of words in a subscript node */
@d collective_subscript 0 /* code for the attribute `\.{[]}' */

@<Initialize table...@>=
attr_loc(end_attr)=hash_end+1; parent(end_attr)=null;

@ Variables of type \&{pair} will have values that point to four-word
nodes containing two numeric values. The first of these values has
|name_type=mp_x_part_sector| and the second has |name_type=mp_y_part_sector|;
the |link| in the first points back to the node whose |value| points
to this four-word node.

Variables of type \&{transform} are similar, but in this case their
|value| points to a 12-word node containing six values, identified by
|x_part_sector|, |y_part_sector|, |mp_xx_part_sector|, |mp_xy_part_sector|,
|mp_yx_part_sector|, and |mp_yy_part_sector|.
Finally, variables of type \&{color} have 3~values in 6~words
identified by |mp_red_part_sector|, |mp_green_part_sector|, and |mp_blue_part_sector|.

When an entire structured variable is saved, the |root| indication
is temporarily replaced by |saved_root|.

Some variables have no name; they just are used for temporary storage
while expressions are being evaluated. We call them {\sl capsules}.

@d x_part_loc(A) (A) /* where the \&{xpart} is found in a pair or transform node */
@d y_part_loc(A) ((A)+2) /* where the \&{ypart} is found in a pair or transform node */
@d xx_part_loc(A) ((A)+4) /* where the \&{xxpart} is found in a transform node */
@d xy_part_loc(A) ((A)+6) /* where the \&{xypart} is found in a transform node */
@d yx_part_loc(A) ((A)+8) /* where the \&{yxpart} is found in a transform node */
@d yy_part_loc(A) ((A)+10) /* where the \&{yypart} is found in a transform node */
@d red_part_loc(A) (A) /* where the \&{redpart} is found in a color node */
@d green_part_loc(A) ((A)+2) /* where the \&{greenpart} is found in a color node */
@d blue_part_loc(A) ((A)+4) /* where the \&{bluepart} is found in a color node */
@d cyan_part_loc(A) (A) /* where the \&{cyanpart} is found in a color node */
@d magenta_part_loc(A) ((A)+2) /* where the \&{magentapart} is found in a color node */
@d yellow_part_loc(A) ((A)+4) /* where the \&{yellowpart} is found in a color node */
@d black_part_loc(A) ((A)+6) /* where the \&{blackpart} is found in a color node */
@d grey_part_loc(A) (A) /* where the \&{greypart} is found in a color node */
@#
@d pair_node_size 4 /* the number of words in a pair node */
@d transform_node_size 12 /* the number of words in a transform node */
@d color_node_size 6 /* the number of words in a color node */
@d cmykcolor_node_size 8 /* the number of words in a color node */

@<Glob...@>=
quarterword big_node_size[mp_pair_type+1];
quarterword sector0[mp_pair_type+1];
quarterword sector_offset[mp_black_part_sector+1];

@ The |sector0| array gives for each big node type, |name_type| values
for its first subfield; the |sector_offset| array gives for each
|name_type| value, the offset from the first subfield in words;
and the |big_node_size| array gives the size in words for each type of
big node.

@<Set init...@>=
mp->big_node_size[mp_transform_type]=transform_node_size;
mp->big_node_size[mp_pair_type]=pair_node_size;
mp->big_node_size[mp_color_type]=color_node_size;
mp->big_node_size[mp_cmykcolor_type]=cmykcolor_node_size;
mp->sector0[mp_transform_type]=mp_x_part_sector;
mp->sector0[mp_pair_type]=mp_x_part_sector;
mp->sector0[mp_color_type]=mp_red_part_sector;
mp->sector0[mp_cmykcolor_type]=mp_cyan_part_sector;
for (k=mp_x_part_sector;k<= mp_yy_part_sector;k++ ) {
  mp->sector_offset[k]=(quarterword)(2*(k-mp_x_part_sector));
}
for (k=mp_red_part_sector;k<= mp_blue_part_sector ; k++) {
  mp->sector_offset[k]=(quarterword)(2*(k-mp_red_part_sector));
}
for (k=mp_cyan_part_sector;k<= mp_black_part_sector;k++ ) {
  mp->sector_offset[k]=(quarterword)(2*(k-mp_cyan_part_sector));
}

@ If |type(p)=mp_pair_type| or |mp_transform_type| and if |value(p)=null|, the
procedure call |init_big_node(p)| will allocate a pair or transform node
for~|p|.  The individual parts of such nodes are initially of type
|mp_independent|.

@c 
static void mp_init_big_node (MP mp,pointer p) {
  pointer q; /* the new node */
  quarterword s; /* its size */
  s=mp->big_node_size[mp_type(p)]; q=mp_get_node(mp, s);
  do {  
    s=(quarterword)(s-2); 
    @<Make variable |q+s| newly independent@>;
    mp_name_type(q+s)=(quarterword)(halfp(s)+mp->sector0[mp_type(p)]); 
    mp_link(q+s)=null;
  } while (s!=0);
  mp_link(q)=p; value(p)=q;
}

@ The |id_transform| function creates a capsule for the
identity transformation.

@c 
static pointer mp_id_transform (MP mp) {
  pointer p,q,r; /* list manipulation registers */
  p=mp_get_node(mp, value_node_size); mp_type(p)=mp_transform_type;
  mp_name_type(p)=mp_capsule; value(p)=null; mp_init_big_node(mp, p); q=value(p);
  r=q+transform_node_size;
  do {  
    r=r-2;
    mp_type(r)=mp_known; value(r)=0;
  } while (r!=q);
  value(xx_part_loc(q))=unity; 
  value(yy_part_loc(q))=unity;
  return p;
}

@ Tokens are of type |tag_token| when they first appear, but they point
to |null| until they are first used as the root of a variable.
The following subroutine establishes the root node on such grand occasions.

@c 
static void mp_new_root (MP mp,pointer x) {
  pointer p; /* the new node */
  p=mp_get_node(mp, value_node_size); mp_type(p)=undefined; mp_name_type(p)=mp_root;
  mp_link(p)=x; equiv(x)=p;
}

@ These conventions for variable representation are illustrated by the
|print_variable_name| routine, which displays the full name of a
variable given only a pointer to its two-word value packet.

@<Declarations@>=
static void mp_print_variable_name (MP mp, pointer p);

@ @c 
void mp_print_variable_name (MP mp, pointer p) {
  pointer q; /* a token list that will name the variable's suffix */
  pointer r; /* temporary for token list creation */
  while ( mp_name_type(p)>=mp_x_part_sector ) {
    @<Preface the output with a part specifier; |return| in the
      case of a capsule@>;
  }
  q=null;
  while ( mp_name_type(p)>mp_saved_root ) {
    @<Ascend one level, pushing a token onto list |q|
     and replacing |p| by its parent@>;
  }
  r=mp_get_avail(mp); mp_info(r)=mp_link(p); mp_link(r)=q;
  if ( mp_name_type(p)==mp_saved_root ) mp_print(mp, "(SAVED)");
@.SAVED@>
  mp_show_token_list(mp, r,null,el_gordo,mp->tally); 
  mp_flush_token_list(mp, r);
}

@ @<Ascend one level, pushing a token onto list |q|...@>=
{ 
  if ( mp_name_type(p)==mp_subscr ) { 
    r=mp_new_num_tok(mp, subscript(p));
    do {  
      p=mp_link(p);
    } while (mp_name_type(p)!=mp_attr);
  } else if ( mp_name_type(p)==mp_structured_root ) {
    p=mp_link(p); goto FOUND;
  } else { 
    if ( mp_name_type(p)!=mp_attr ) mp_confusion(mp, "var");
@:this can't happen var}{\quad var@>
    r=mp_get_avail(mp); mp_info(r)=attr_loc(p);
  }
  mp_link(r)=q; q=r;
FOUND:  
  p=parent(p);
}

@ @<Preface the output with a part specifier...@>=
{ switch (mp_name_type(p)) {
  case mp_x_part_sector: mp_print_char(mp, xord('x')); break;
  case mp_y_part_sector: mp_print_char(mp, xord('y')); break;
  case mp_xx_part_sector: mp_print(mp, "xx"); break;
  case mp_xy_part_sector: mp_print(mp, "xy"); break;
  case mp_yx_part_sector: mp_print(mp, "yx"); break;
  case mp_yy_part_sector: mp_print(mp, "yy"); break;
  case mp_red_part_sector: mp_print(mp, "red"); break;
  case mp_green_part_sector: mp_print(mp, "green"); break;
  case mp_blue_part_sector: mp_print(mp, "blue"); break;
  case mp_cyan_part_sector: mp_print(mp, "cyan"); break;
  case mp_magenta_part_sector: mp_print(mp, "magenta"); break;
  case mp_yellow_part_sector: mp_print(mp, "yellow"); break;
  case mp_black_part_sector: mp_print(mp, "black"); break;
  case mp_grey_part_sector: mp_print(mp, "grey"); break;
  case mp_capsule: 
    mp_print(mp, "%CAPSULE"); mp_print_int(mp, p-null); return;
    break;
@.CAPSULE@>
  } /* there are no other cases */
  mp_print(mp, "part "); 
  p=mp_link(p-mp->sector_offset[mp_name_type(p)]);
}

@ The |interesting| function returns |true| if a given variable is not
in a capsule, or if the user wants to trace capsules.

@c 
static boolean mp_interesting (MP mp,pointer p) {
  quarterword t; /* a |name_type| */
  if ( mp->internal[mp_tracing_capsules]>0 ) {
    return true;
  } else { 
    t=mp_name_type(p);
    if ( t>=mp_x_part_sector ) if ( t!=mp_capsule )
      t=mp_name_type(mp_link(p-mp->sector_offset[t]));
    return (t!=mp_capsule);
  }
}

@ Now here is a subroutine that converts an unstructured type into an
equivalent structured type, by inserting a |mp_structured| node that is
capable of growing. This operation is done only when |mp_name_type(p)=root|,
|subscr|, or |attr|.

The procedure returns a pointer to the new node that has taken node~|p|'s
place in the structure. Node~|p| itself does not move, nor are its
|value| or |type| fields changed in any way.

@c 
static pointer mp_new_structure (MP mp,pointer p) {
  pointer q,r=0; /* list manipulation registers */
  switch (mp_name_type(p)) {
  case mp_root: 
    q=mp_link(p); r=mp_get_node(mp, value_node_size); equiv(q)=r;
    break;
  case mp_subscr: 
    @<Link a new subscript node |r| in place of node |p|@>;
    break;
  case mp_attr: 
    @<Link a new attribute node |r| in place of node |p|@>;
    break;
  default: 
    mp_confusion(mp, "struct");
@:this can't happen struct}{\quad struct@>
    break;
  }
  mp_link(r)=mp_link(p); mp_type(r)=mp_structured; mp_name_type(r)=mp_name_type(p);
  attr_head(r)=p; mp_name_type(p)=mp_structured_root;
  q=mp_get_node(mp, attr_node_size); mp_link(p)=q; subscr_head(r)=q;
  parent(q)=r; mp_type(q)=undefined; mp_name_type(q)=mp_attr; mp_link(q)=end_attr;
  attr_loc(q)=collective_subscript; 
  return r;
}

@ @<Link a new subscript node |r| in place of node |p|@>=
{ 
  q=p;
  do {  
    q=mp_link(q);
  } while (mp_name_type(q)!=mp_attr);
  q=parent(q); r=subscr_head_loc(q); /* |mp_link(r)=subscr_head(q)| */
  do {  
    q=r; r=mp_link(r);
  } while (r!=p);
  r=mp_get_node(mp, subscr_node_size);
  mp_link(q)=r; subscript(r)=subscript(p);
}

@ If the attribute is |collective_subscript|, there are two pointers to
node~|p|, so we must change both of them.

@<Link a new attribute node |r| in place of node |p|@>=
{ 
  q=parent(p); r=attr_head(q);
  do {  
    q=r; r=mp_link(r);
  } while (r!=p);
  r=mp_get_node(mp, attr_node_size); mp_link(q)=r;
  mp->mem[attr_loc_loc(r)]=mp->mem[attr_loc_loc(p)]; /* copy |attr_loc| and |parent| */
  if ( attr_loc(p)==collective_subscript ) { 
    q=subscr_head_loc(parent(p));
    while ( mp_link(q)!=p ) q=mp_link(q);
    mp_link(q)=r;
  }
}

@ The |find_variable| routine is given a pointer~|t| to a nonempty token
list of suffixes; it returns a pointer to the corresponding two-word
value. For example, if |t| points to token \.x followed by a numeric
token containing the value~7, |find_variable| finds where the value of
\.{x7} is stored in memory. This may seem a simple task, and it
usually is, except when \.{x7} has never been referenced before.
Indeed, \.x may never have even been subscripted before; complexities
arise with respect to updating the collective subscript information.

If a macro type is detected anywhere along path~|t|, or if the first
item on |t| isn't a |tag_token|, the value |null| is returned.
Otherwise |p| will be a non-null pointer to a node such that
|undefined<type(p)<mp_structured|.

@d abort_find { return null; }

@c 
static pointer mp_find_variable (MP mp,pointer t) {
  pointer p,q,r,s; /* nodes in the ``value'' line */
  pointer pp,qq,rr,ss; /* nodes in the ``collective'' line */
  integer n; /* subscript or attribute */
  memory_word save_word; /* temporary storage for a word of |mem| */
@^inner loop@>
  p=mp_info(t); t=mp_link(t);
  if ( (eq_type(p) % outer_tag) != tag_token ) abort_find;
  if ( equiv(p)==null ) mp_new_root(mp, p);
  p=equiv(p); pp=p;
  while ( t!=null ) { 
    @<Make sure that both nodes |p| and |pp| are of |mp_structured| type@>;
    if ( t<mp->hi_mem_min ) {
      @<Descend one level for the subscript |value(t)|@>
    } else {
      @<Descend one level for the attribute |mp_info(t)|@>;
    }
    t=mp_link(t);
  }
  if ( mp_type(pp)>=mp_structured ) {
    if ( mp_type(pp)==mp_structured ) pp=attr_head(pp); else abort_find;
  }
  if ( mp_type(p)==mp_structured ) p=attr_head(p);
  if ( mp_type(p)==undefined ) { 
    if ( mp_type(pp)==undefined ) { mp_type(pp)=mp_numeric_type; value(pp)=null; };
    mp_type(p)=mp_type(pp); value(p)=null;
  };
  return p;
}

@ Although |pp| and |p| begin together, they diverge when a subscript occurs;
|pp|~stays in the collective line while |p|~goes through actual subscript
values.

@<Make sure that both nodes |p| and |pp|...@>=
if ( mp_type(pp)!=mp_structured ) { 
  if ( mp_type(pp)>mp_structured ) abort_find;
  ss=mp_new_structure(mp, pp);
  if ( p==pp ) p=ss;
  pp=ss;
}; /* now |type(pp)=mp_structured| */
if ( mp_type(p)!=mp_structured ) /* it cannot be |>mp_structured| */
  p=mp_new_structure(mp, p) /* now |type(p)=mp_structured| */

@ We want this part of the program to be reasonably fast, in case there are
@^inner loop@>
lots of subscripts at the same level of the data structure. Therefore
we store an ``infinite'' value in the word that appears at the end of the
subscript list, even though that word isn't part of a subscript node.

@<Descend one level for the subscript |value(t)|@>=
{ 
  n=value(t);
  pp=mp_link(attr_head(pp)); /* now |attr_loc(pp)=collective_subscript| */
  q=mp_link(attr_head(p)); save_word=mp->mem[subscript_loc(q)];
  subscript(q)=el_gordo; s=subscr_head_loc(p); /* |mp_link(s)=subscr_head(p)| */
  do {  
    r=s; s=mp_link(s);
  } while (n>subscript(s));
  if ( n==subscript(s) ) {
    p=s;
  } else { 
    p=mp_get_node(mp, subscr_node_size); mp_link(r)=p; mp_link(p)=s;
    subscript(p)=n; mp_name_type(p)=mp_subscr; mp_type(p)=undefined;
  }
  mp->mem[subscript_loc(q)]=save_word;
}

@ @<Descend one level for the attribute |mp_info(t)|@>=
{ 
  n=mp_info(t);
  ss=attr_head(pp);
  do {  
    rr=ss; ss=mp_link(ss);
  } while (n>attr_loc(ss));
  if ( n<attr_loc(ss) ) { 
    qq=mp_get_node(mp, attr_node_size); mp_link(rr)=qq; mp_link(qq)=ss;
    attr_loc(qq)=n; mp_name_type(qq)=mp_attr; mp_type(qq)=undefined;
    parent(qq)=pp; ss=qq;
  }
  if ( p==pp ) { 
    p=ss; pp=ss;
  } else { 
    pp=ss; s=attr_head(p);
    do {  
      r=s; s=mp_link(s);
    } while (n>attr_loc(s));
    if ( n==attr_loc(s) ) {
      p=s;
    } else { 
      q=mp_get_node(mp, attr_node_size); mp_link(r)=q; mp_link(q)=s;
      attr_loc(q)=n; mp_name_type(q)=mp_attr; mp_type(q)=undefined;
      parent(q)=p; p=q;
    }
  }
}

@ Variables lose their former values when they appear in a type declaration,
or when they are defined to be macros or \&{let} equal to something else.
A subroutine will be defined later that recycles the storage associated
with any particular |type| or |value|; our goal now is to study a higher
level process called |flush_variable|, which selectively frees parts of a
variable structure.

This routine has some complexity because of examples such as
`\hbox{\tt numeric x[]a[]b}'
which recycles all variables of the form \.{x[i]a[j]b} (and no others), while
`\hbox{\tt vardef x[]a[]=...}'
discards all variables of the form \.{x[i]a[j]} followed by an arbitrary
suffix, except for the collective node \.{x[]a[]} itself. The obvious way
to handle such examples is to use recursion; so that's what we~do.
@^recursion@>

Parameter |p| points to the root information of the variable;
parameter |t| points to a list of one-word nodes that represent
suffixes, with |info=collective_subscript| for subscripts.

@<Declarations@>=
static void mp_flush_cur_exp (MP mp,scaled v) ;

@ @c 
static void mp_flush_variable (MP mp,pointer p, pointer t, boolean discard_suffixes) {
  pointer q,r; /* list manipulation */
  halfword n; /* attribute to match */
  while ( t!=null ) { 
    if ( mp_type(p)!=mp_structured ) return;
    n=mp_info(t); t=mp_link(t);
    if ( n==collective_subscript ) { 
      r=subscr_head_loc(p); q=mp_link(r); /* |q=subscr_head(p)| */
      while ( mp_name_type(q)==mp_subscr ){ 
        mp_flush_variable(mp, q,t,discard_suffixes);
        if ( t==null ) {
          if ( mp_type(q)==mp_structured ) r=q;
          else  { mp_link(r)=mp_link(q); mp_free_node(mp, q,subscr_node_size);   }
        } else {
          r=q;
        }
        q=mp_link(r);
      }
    }
    p=attr_head(p);
    do {  
      r=p; p=mp_link(p);
    } while (attr_loc(p)<n);
    if ( attr_loc(p)!=n ) return;
  }
  if ( discard_suffixes ) {
    mp_flush_below_variable(mp, p);
  } else { 
    if ( mp_type(p)==mp_structured ) p=attr_head(p);
    mp_recycle_value(mp, p);
  }
}

@ The next procedure is simpler; it wipes out everything but |p| itself,
which becomes undefined.

@<Declarations@>=
static void mp_flush_below_variable (MP mp, pointer p);

@ @c
void mp_flush_below_variable (MP mp,pointer p) {
   pointer q,r; /* list manipulation registers */
  if ( mp_type(p)!=mp_structured ) {
    mp_recycle_value(mp, p); /* this sets |type(p)=undefined| */
  } else { 
    q=subscr_head(p);
    while ( mp_name_type(q)==mp_subscr ) { 
      mp_flush_below_variable(mp, q); r=q; q=mp_link(q);
      mp_free_node(mp, r,subscr_node_size);
    }
    r=attr_head(p); q=mp_link(r); mp_recycle_value(mp, r);
    if ( mp_name_type(p)<=mp_saved_root ) mp_free_node(mp, r,value_node_size);
    else mp_free_node(mp, r,subscr_node_size);
    /* we assume that |subscr_node_size=attr_node_size| */
    do {  
      mp_flush_below_variable(mp, q); r=q; q=mp_link(q); mp_free_node(mp, r,attr_node_size);
    } while (q!=end_attr);
    mp_type(p)=undefined;
  }
}

@ Just before assigning a new value to a variable, we will recycle the
old value and make the old value undefined. The |und_type| routine
determines what type of undefined value should be given, based on
the current type before recycling.

@c 
static quarterword mp_und_type (MP mp,pointer p) { 
  switch (mp_type(p)) {
  case undefined: case mp_vacuous:
    return undefined;
  case mp_boolean_type: case mp_unknown_boolean:
    return mp_unknown_boolean;
  case mp_string_type: case mp_unknown_string:
    return mp_unknown_string;
  case mp_pen_type: case mp_unknown_pen:
    return mp_unknown_pen;
  case mp_path_type: case mp_unknown_path:
    return mp_unknown_path;
  case mp_picture_type: case mp_unknown_picture:
    return mp_unknown_picture;
  case mp_transform_type: case mp_color_type: case mp_cmykcolor_type:
  case mp_pair_type: case mp_numeric_type: 
    return mp_type(p);
  case mp_known: case mp_dependent: case mp_proto_dependent: case mp_independent:
    return mp_numeric_type;
  } /* there are no other cases */
  return 0;
}

@ The |clear_symbol| routine is used when we want to redefine the equivalent
of a symbolic token. It must remove any variable structure or macro
definition that is currently attached to that symbol. If the |saving|
parameter is true, a subsidiary structure is saved instead of destroyed.

@c 
static void mp_clear_symbol (MP mp,pointer p, boolean saving) {
  pointer q; /* |equiv(p)| */
  q=equiv(p);
  switch (eq_type(p) % outer_tag)  {
  case defined_macro:
  case secondary_primary_macro:
  case tertiary_secondary_macro:
  case expression_tertiary_macro: 
    if ( ! saving ) mp_delete_mac_ref(mp, q);
    break;
  case tag_token:
    if ( q!=null ) {
      if ( saving ) {
        mp_name_type(q)=mp_saved_root;
      } else { 
        mp_flush_below_variable(mp, q); 
	    mp_free_node(mp,q,value_node_size); 
      }
    }
    break;
  default:
    break;
  }
  mp->eqtb[p]=mp->eqtb[frozen_undefined];
}

@* \[16] Saving and restoring equivalents.
The nested structure given by \&{begingroup} and \&{endgroup}
allows |eqtb| entries to be saved and restored, so that temporary changes
can be made without difficulty.  When the user requests a current value to
be saved, \MP\ puts that value into its ``save stack.'' An appearance of
\&{endgroup} ultimately causes the old values to be removed from the save
stack and put back in their former places.

The save stack is a linked list containing three kinds of entries,
distinguished by their |info| fields. If |p| points to a saved item,
then

\smallskip\hang
|mp_info(p)=0| stands for a group boundary; each \&{begingroup} contributes
such an item to the save stack and each \&{endgroup} cuts back the stack
until the most recent such entry has been removed.

\smallskip\hang
|mp_info(p)=q|, where |1<=q<=hash_end|, means that |mem[p+1]| holds the former
contents of |eqtb[q]|. Such save stack entries are generated by \&{save}
commands.

\smallskip\hang
|mp_info(p)=hash_end+q|, where |q>0|, means that |value(p)| is a |scaled|
integer to be restored to internal parameter number~|q|. Such entries
are generated by \&{interim} commands.

\smallskip\noindent
The global variable |save_ptr| points to the top item on the save stack.

@d save_node_size 2 /* number of words per non-boundary save-stack node */
@d saved_equiv(A) mp->mem[(A)+1].hh /* where an |eqtb| entry gets saved */
@d save_boundary_item(A) { (A)=mp_get_avail(mp); mp_info((A))=0;
  mp_link((A))=mp->save_ptr; mp->save_ptr=(A);
  }

@<Glob...@>=
pointer save_ptr; /* the most recently saved item */

@ @<Set init...@>=mp->save_ptr=null;

@ The |save_variable| routine is given a hash address |q|; it salts this
address in the save stack, together with its current equivalent,
then makes token~|q| behave as though it were brand new.

Nothing is stacked when |save_ptr=null|, however; there's no way to remove
things from the stack when the program is not inside a group, so there's
no point in wasting the space.

@c 
static void mp_save_variable (MP mp,pointer q) {
  pointer p; /* temporary register */
  if ( mp->save_ptr!=null ){ 
    p=mp_get_node(mp, save_node_size); mp_info(p)=q; mp_link(p)=mp->save_ptr;
    saved_equiv(p)=mp->eqtb[q]; mp->save_ptr=p;
  }
  mp_clear_symbol(mp, q,(mp->save_ptr!=null));
}

@ Similarly, |save_internal| is given the location |q| of an internal
quantity like |mp_tracing_pens|. It creates a save stack entry of the
third kind.

@c 
static void mp_save_internal (MP mp,halfword q) {
  pointer p; /* new item for the save stack */
  if ( mp->save_ptr!=null ){ 
     p=mp_get_node(mp, save_node_size);
     mp_info(p)=hash_end+q;
     mp_link(p)=mp->save_ptr; 
     value(p)=mp->internal[q];
     mp->save_ptr=p;
  }
}

@ At the end of a group, the |unsave| routine restores all of the saved
equivalents in reverse order. This routine will be called only when there
is at least one boundary item on the save stack.

@c 
static void mp_unsave (MP mp) {
  pointer q; /* index to saved item */
  pointer p; /* temporary register */
  while ( mp_info(mp->save_ptr)!=0 ) {
    q=mp_info(mp->save_ptr);
    if ( q>hash_end ) {
      if ( mp->internal[mp_tracing_restores]>0 ) {
        mp_begin_diagnostic(mp);
        mp_print_nl(mp, "{restoring ");
        mp_print(mp, mp->int_name[q-(hash_end)]);
        mp_print_char(mp, xord('='));
        if (mp->int_type[q-(hash_end)]==mp_known) {
           mp_print_scaled(mp, value(mp->save_ptr));
        } else if (mp->int_type[q-(hash_end)]==mp_string_type) {
           char *s = mp_str(mp, value(mp->save_ptr));
           mp_print(mp, s);
           free(s);
        } else {
           mp_confusion(mp,"internal_restore");
        }
        mp_print_char(mp, xord('}'));
        mp_end_diagnostic(mp, false);
      }
      mp->internal[q-(hash_end)]=value(mp->save_ptr);
    } else { 
      if ( mp->internal[mp_tracing_restores]>0 ) {
        mp_begin_diagnostic(mp); 
        mp_print_nl(mp, "{restoring ");
        mp_print_text(q); 
        mp_print_char(mp, xord('}'));
        mp_end_diagnostic(mp, false);
      }
      mp_clear_symbol(mp, q,false);
      mp->eqtb[q]=saved_equiv(mp->save_ptr);
      if ( eq_type(q) % outer_tag==tag_token ) {
        p=equiv(q);
        if ( p!=null ) mp_name_type(p)=mp_root;
      }
    }
    p=mp_link(mp->save_ptr); 
    mp_free_node(mp, mp->save_ptr,save_node_size); mp->save_ptr=p;
  }
  p=mp_link(mp->save_ptr); free_avail(mp->save_ptr); mp->save_ptr=p;
}

@* \[17] Data structures for paths.
When a \MP\ user specifies a path, \MP\ will create a list of knots
and control points for the associated cubic spline curves. If the
knots are $z_0$, $z_1$, \dots, $z_n$, there are control points
$z_k^+$ and $z_{k+1}^-$ such that the cubic splines between knots
$z_k$ and $z_{k+1}$ are defined by B\'ezier's formula
@:Bezier}{B\'ezier, Pierre Etienne@>
$$\eqalign{z(t)&=B(z_k,z_k^+,z_{k+1}^-,z_{k+1};t)\cr
&=(1-t)^3z_k+3(1-t)^2tz_k^++3(1-t)t^2z_{k+1}^-+t^3z_{k+1}\cr}$$
for |0<=t<=1|.

There is an 8-word node for each knot $z_k$, containing one word of
control information and six words for the |x| and |y| coordinates of
$z_k^-$ and $z_k$ and~$z_k^+$. The control information appears in the
|mp_left_type| and |mp_right_type| fields, which each occupy a quarter of
the first word in the node; they specify properties of the curve as it
enters and leaves the knot. There's also a halfword |link| field,
which points to the following knot, and a final supplementary word (of
which only a quarter is used).

If the path is a closed contour, knots 0 and |n| are identical;
i.e., the |link| in knot |n-1| points to knot~0. But if the path
is not closed, the |mp_left_type| of knot~0 and the |mp_right_type| of knot~|n|
are equal to |endpoint|. In the latter case the |link| in knot~|n| points
to knot~0, and the control points $z_0^-$ and $z_n^+$ are not used.

@d mp_left_type(A)   mp->mem[(A)].hh.b0 /* characterizes the path entering this knot */
@d mp_right_type(A)   mp->mem[(A)].hh.b1 /* characterizes the path leaving this knot */
@d mp_x_coord(A)   mp->mem[(A)+1].sc /* the |x| coordinate of this knot */ 
@d mp_y_coord(A)   mp->mem[(A)+2].sc /* the |y| coordinate of this knot */
@d mp_left_x(A)   mp->mem[(A)+3].sc /* the |x| coordinate of previous control point */
@d mp_left_y(A)   mp->mem[(A)+4].sc /* the |y| coordinate of previous control point */
@d mp_right_x(A)   mp->mem[(A)+5].sc /* the |x| coordinate of next control point */
@d mp_right_y(A)   mp->mem[(A)+6].sc /* the |y| coordinate of next control point */
@d x_loc(A)   ((A)+1) /* where the |x| coordinate is stored in a knot */
@d y_loc(A)   ((A)+2) /* where the |y| coordinate is stored in a knot */
@d knot_coord(A)   mp->mem[(A)].sc /* |x| or |y| coordinate given |x_loc| or |y_loc| */
@d left_coord(A)   mp->mem[(A)+2].sc
  /* coordinate of previous control point given |x_loc| or |y_loc| */
@d right_coord(A)   mp->mem[(A)+4].sc
  /* coordinate of next control point given |x_loc| or |y_loc| */
@d knot_node_size 8 /* number of words in a knot node */

@(mplib.h@>=
enum mp_knot_type {
 mp_endpoint=0, /* |mp_left_type| at path beginning and |mp_right_type| at path end */
 mp_explicit, /* |mp_left_type| or |mp_right_type| when control points are known */
 mp_given, /* |mp_left_type| or |mp_right_type| when a direction is given */
 mp_curl, /* |mp_left_type| or |mp_right_type| when a curl is desired */
 mp_open, /* |mp_left_type| or |mp_right_type| when \MP\ should choose the direction */
 mp_end_cycle
};

@ Before the B\'ezier control points have been calculated, the memory
space they will ultimately occupy is taken up by information that can be
used to compute them. There are four cases:

\yskip
\textindent{$\bullet$} If |mp_right_type=mp_open|, the curve should leave
the knot in the same direction it entered; \MP\ will figure out a
suitable direction.

\yskip
\textindent{$\bullet$} If |mp_right_type=mp_curl|, the curve should leave the
knot in a direction depending on the angle at which it enters the next
knot and on the curl parameter stored in |right_curl|.

\yskip
\textindent{$\bullet$} If |mp_right_type=mp_given|, the curve should leave the
knot in a nonzero direction stored as an |angle| in |right_given|.

\yskip
\textindent{$\bullet$} If |mp_right_type=mp_explicit|, the B\'ezier control
point for leaving this knot has already been computed; it is in the
|mp_right_x| and |mp_right_y| fields.

\yskip\noindent
The rules for |mp_left_type| are similar, but they refer to the curve entering
the knot, and to \\{left} fields instead of \\{right} fields.

Non-|explicit| control points will be chosen based on ``tension'' parameters
in the |left_tension| and |right_tension| fields. The
`\&{atleast}' option is represented by negative tension values.
@:at_least_}{\&{atleast} primitive@>

For example, the \MP\ path specification
$$\.{z0..z1..tension atleast 1..\{curl 2\}z2..z3\{-1,-2\}..tension
  3 and 4..p},$$
where \.p is the path `\.{z4..controls z45 and z54..z5}', will be represented
by the six knots
\def\lodash{\hbox to 1.1em{\thinspace\hrulefill\thinspace}}
$$\vbox{\halign{#\hfil&&\qquad#\hfil\cr
|mp_left_type|&\\{left} info&|mp_x_coord,mp_y_coord|&|mp_right_type|&\\{right} info\cr
\noalign{\yskip}
|endpoint|&\lodash$,\,$\lodash&$x_0,y_0$&|curl|&$1.0,1.0$\cr
|open|&\lodash$,1.0$&$x_1,y_1$&|open|&\lodash$,-1.0$\cr
|curl|&$2.0,-1.0$&$x_2,y_2$&|curl|&$2.0,1.0$\cr
|given|&$d,1.0$&$x_3,y_3$&|given|&$d,3.0$\cr
|open|&\lodash$,4.0$&$x_4,y_4$&|explicit|&$x_{45},y_{45}$\cr
|explicit|&$x_{54},y_{54}$&$x_5,y_5$&|endpoint|&\lodash$,\,$\lodash\cr}}$$
Here |d| is the |angle| obtained by calling |n_arg(-unity,-two)|.
Of course, this example is more complicated than anything a normal user
would ever write.

These types must satisfy certain restrictions because of the form of \MP's
path syntax:
(i)~|open| type never appears in the same node together with |endpoint|,
|given|, or |curl|.
(ii)~The |mp_right_type| of a node is |explicit| if and only if the
|mp_left_type| of the following node is |explicit|.
(iii)~|endpoint| types occur only at the ends, as mentioned above.

@d left_curl mp_left_x /* curl information when entering this knot */
@d left_given mp_left_x /* given direction when entering this knot */
@d left_tension mp_left_y /* tension information when entering this knot */
@d right_curl mp_right_x /* curl information when leaving this knot */
@d right_given mp_right_x /* given direction when leaving this knot */
@d right_tension mp_right_y /* tension information when leaving this knot */

@ Knots can be user-supplied, or they can be created by program code,
like the |split_cubic| function, or |copy_path|. The distinction is
needed for the cleanup routine that runs after |split_cubic|, because
it should only delete knots it has previously inserted, and never
anything that was user-supplied. In order to be able to differentiate
one knot from another, we will set |originator(p):=mp_metapost_user| when
it appeared in the actual metapost program, and
|originator(p):=mp_program_code| in all other cases.

@d mp_originator(A)   mp->mem[(A)+7].hh.b0 /* the creator of this knot */

@<Exported types@>=
enum mp_knot_originator {
  mp_program_code=0, /* not created by a user */
  mp_metapost_user /* created by a user */
};

@ Here is a routine that prints a given knot list
in symbolic form. It illustrates the conventions discussed above,
and checks for anomalies that might arise while \MP\ is being debugged.

@<Declarations@>=
static void mp_pr_path (MP mp,pointer h);

@ @c
void mp_pr_path (MP mp,pointer h) {
  pointer p,q; /* for list traversal */
  p=h;
  do {  
    q=mp_link(p);
    if ( (p==null)||(q==null) ) { 
      mp_print_nl(mp, "???"); return; /* this won't happen */
@.???@>
    }
    @<Print information for adjacent knots |p| and |q|@>;
  DONE1:
    p=q;
    if ( (p!=h)||(mp_left_type(h)!=mp_endpoint) ) {
      @<Print two dots, followed by |given| or |curl| if present@>;
    }
  } while (p!=h);
  if ( mp_left_type(h)!=mp_endpoint ) 
    mp_print(mp, "cycle");
}

@ @<Print information for adjacent knots...@>=
mp_print_two(mp, mp_x_coord(p),mp_y_coord(p));
switch (mp_right_type(p)) {
case mp_endpoint: 
  if ( mp_left_type(p)==mp_open ) mp_print(mp, "{open?}"); /* can't happen */
@.open?@>
  if ( (mp_left_type(q)!=mp_endpoint)||(q!=h) ) q=null; /* force an error */
  goto DONE1;
  break;
case mp_explicit: 
  @<Print control points between |p| and |q|, then |goto done1|@>;
  break;
case mp_open: 
  @<Print information for a curve that begins |open|@>;
  break;
case mp_curl:
case mp_given: 
  @<Print information for a curve that begins |curl| or |given|@>;
  break;
default:
  mp_print(mp, "???"); /* can't happen */
@.???@>
  break;
}
if ( mp_left_type(q)<=mp_explicit ) {
  mp_print(mp, "..control?"); /* can't happen */
@.control?@>
} else if ( (right_tension(p)!=unity)||(left_tension(q)!=unity) ) {
  @<Print tension between |p| and |q|@>;
}

@ Since |n_sin_cos| produces |fraction| results, which we will print as if they
were |scaled|, the magnitude of a |given| direction vector will be~4096.

@<Print two dots...@>=
{ 
  mp_print_nl(mp, " ..");
  if ( mp_left_type(p)==mp_given ) { 
    mp_n_sin_cos(mp, left_given(p)); mp_print_char(mp, xord('{'));
    mp_print_scaled(mp, mp->n_cos); mp_print_char(mp, xord(','));
    mp_print_scaled(mp, mp->n_sin); mp_print_char(mp, xord('}'));
  } else if ( mp_left_type(p)==mp_curl ){ 
    mp_print(mp, "{curl "); 
    mp_print_scaled(mp, left_curl(p)); mp_print_char(mp, xord('}'));
  }
}

@ @<Print tension between |p| and |q|@>=
{ 
  mp_print(mp, "..tension ");
  if ( right_tension(p)<0 ) mp_print(mp, "atleast");
  mp_print_scaled(mp, abs(right_tension(p)));
  if ( right_tension(p)!=left_tension(q) ){ 
    mp_print(mp, " and ");
    if ( left_tension(q)<0 ) mp_print(mp, "atleast");
    mp_print_scaled(mp, abs(left_tension(q)));
  }
}

@ @<Print control points between |p| and |q|, then |goto done1|@>=
{ 
  mp_print(mp, "..controls "); 
  mp_print_two(mp, mp_right_x(p),mp_right_y(p)); 
  mp_print(mp, " and ");
  if ( mp_left_type(q)!=mp_explicit ) { 
    mp_print(mp, "??"); /* can't happen */
@.??@>
  } else {
    mp_print_two(mp, mp_left_x(q),mp_left_y(q));
  }
  goto DONE1;
}

@ @<Print information for a curve that begins |open|@>=
if ( (mp_left_type(p)!=mp_explicit)&&(mp_left_type(p)!=mp_open) ) {
  mp_print(mp, "{open?}"); /* can't happen */
@.open?@>
}

@ A curl of 1 is shown explicitly, so that the user sees clearly that
\MP's default curl is present.

@<Print information for a curve that begins |curl|...@>=
{ 
  if ( mp_left_type(p)==mp_open )  
    mp_print(mp, "??"); /* can't happen */
@.??@>
  if ( mp_right_type(p)==mp_curl ) { 
    mp_print(mp, "{curl "); mp_print_scaled(mp, right_curl(p));
  } else { 
    mp_n_sin_cos(mp, right_given(p)); mp_print_char(mp, xord('{'));
    mp_print_scaled(mp, mp->n_cos); mp_print_char(mp, xord(',')); 
    mp_print_scaled(mp, mp->n_sin);
  }
  mp_print_char(mp, xord('}'));
}

@ It is convenient to have another version of |pr_path| that prints the path
as a diagnostic message.

@<Declarations@>=
static void mp_print_path (MP mp,pointer h, const char *s, boolean nuline) ;

@ @c
void mp_print_path (MP mp,pointer h, const char *s, boolean nuline) { 
  mp_print_diagnostic(mp, "Path", s, nuline); mp_print_ln(mp);
@.Path at line...@>
  mp_pr_path(mp, h);
  mp_end_diagnostic(mp, true);
}

@ If we want to duplicate a knot node, we can say |copy_knot|:

@c 
static pointer mp_copy_knot (MP mp,pointer p) {
  pointer q; /* the copy */
  int k; /* runs through the words of a knot node */
  q=mp_get_node(mp, knot_node_size);
  for (k=0;k<knot_node_size;k++) {
    mp->mem[q+k]=mp->mem[p+k];
  }
  mp_originator(q)=mp_originator(p);
  return q;
}

@ The |copy_path| routine makes a clone of a given path.

@c 
static pointer mp_copy_path (MP mp, pointer p) {
  pointer q,pp,qq; /* for list manipulation */
  q=mp_copy_knot(mp, p);
  qq=q; pp=mp_link(p);
  while ( pp!=p ) { 
    mp_link(qq)=mp_copy_knot(mp, pp);
    qq=mp_link(qq);
    pp=mp_link(pp);
  }
  mp_link(qq)=q;
  return q;
}


@ Just before |ship_out|, knot lists are exported for printing.

The |gr_XXXX| macros are defined in |mppsout.h|.

@c 
static mp_knot *mp_export_knot (MP mp,pointer p) {
  mp_knot *q; /* the copy */
  if (p==null)
     return NULL;
  q = xmalloc(1, sizeof (mp_knot));
  memset(q,0,sizeof (mp_knot));
  gr_left_type(q)  = (unsigned short)mp_left_type(p);
  gr_right_type(q) = (unsigned short)mp_right_type(p);
  gr_x_coord(q)    = mp_x_coord(p);
  gr_y_coord(q)    = mp_y_coord(p);
  gr_left_x(q)     = mp_left_x(p);
  gr_left_y(q)     = mp_left_y(p);
  gr_right_x(q)    = mp_right_x(p);
  gr_right_y(q)    = mp_right_y(p);
  gr_originator(q) = (unsigned char)mp_originator(p);
  return q;
}
static pointer mp_import_knot (MP mp, mp_knot *q) {
  pointer p; /* the copy */
  if (q==NULL)
     return null;
  p=mp_get_node(mp, knot_node_size);
  mp_left_type(p) =  (quarterword)gr_left_type(q);
  mp_right_type(p) = (quarterword)gr_right_type(q);
  mp_x_coord(p) =    gr_x_coord(q);
  mp_y_coord(p) =    gr_y_coord(q);
  mp_left_x(p) =     gr_left_x(q);
  mp_left_y(p) =     gr_left_y(q);
  mp_right_x(p) =    gr_right_x(q);
  mp_right_y(p) =    gr_right_y(q);
  mp_originator(p) = gr_originator(q);
  return p;
}

@ The |export_knot_list| routine therefore also makes a clone 
of a given path.

@c 
static mp_knot *mp_export_knot_list (MP mp, pointer p) {
  mp_knot *q, *qq; /* for list manipulation */
  pointer pp; /* for list manipulation */
  if (p==null)
     return NULL;
  q=mp_export_knot(mp, p);
  qq=q; pp=mp_link(p);
  while ( pp!=p ) { 
    gr_next_knot(qq)=mp_export_knot(mp, pp);
    qq=gr_next_knot(qq);
    pp=mp_link(pp);
  }
  gr_next_knot(qq)=q;
  return q;
}
static pointer mp_import_knot_list (MP mp, mp_knot *q) {
  mp_knot *qq; /* for list manipulation */
  pointer p, pp; /* for list manipulation */
  if (q==NULL)
     return null;
  p=mp_import_knot(mp, q);
  pp=p; qq=gr_link(q);
  while ( qq!=q ) { 
    mp_link(pp)=mp_import_knot(mp, qq);
    qq=gr_next_knot(qq);
    pp=mp_link(pp);
  }
  mp_link(pp)=p;
  return p;
}


@ Similarly, there's a way to copy the {\sl reverse\/} of a path. This procedure
returns a pointer to the first node of the copy, if the path is a cycle,
but to the final node of a non-cyclic copy. The global
variable |path_tail| will point to the final node of the original path;
this trick makes it easier to implement `\&{doublepath}'.

All node types are assumed to be |endpoint| or |explicit| only.

@c 
static pointer mp_htap_ypoc (MP mp,pointer p) {
  pointer q,pp,qq,rr; /* for list manipulation */
  q=mp_get_node(mp, knot_node_size); /* this will correspond to |p| */
  qq=q; pp=p;
  while (1) { 
    mp_right_type(qq)=mp_left_type(pp); mp_left_type(qq)=mp_right_type(pp);
    mp_x_coord(qq)=mp_x_coord(pp); mp_y_coord(qq)=mp_y_coord(pp);
    mp_right_x(qq)=mp_left_x(pp); mp_right_y(qq)=mp_left_y(pp);
    mp_left_x(qq)=mp_right_x(pp); mp_left_y(qq)=mp_right_y(pp);
    mp_originator(qq)=mp_originator(pp);
    if ( mp_link(pp)==p ) { 
      mp_link(q)=qq; mp->path_tail=pp; return q;
    }
    rr=mp_get_node(mp, knot_node_size); mp_link(rr)=qq; qq=rr; pp=mp_link(pp);
  }
}

@ @<Glob...@>=
pointer path_tail; /* the node that links to the beginning of a path */

@ When a cyclic list of knot nodes is no longer needed, it can be recycled by
calling the following subroutine.

@<Declarations@>=
static void mp_toss_knot_list (MP mp,pointer p) ;

@ @c
void mp_toss_knot_list (MP mp,pointer p) {
  pointer q; /* the node being freed */
  pointer r; /* the next node */
  q=p;
  do {  
    r=mp_link(q); 
    mp_free_node(mp, q,knot_node_size); q=r;
  } while (q!=p);
}

@* \[18] Choosing control points.
Now we must actually delve into one of \MP's more difficult routines,
the |make_choices| procedure that chooses angles and control points for
the splines of a curve when the user has not specified them explicitly.
The parameter to |make_choices| points to a list of knots and
path information, as described above.

A path decomposes into independent segments at ``breakpoint'' knots,
which are knots whose left and right angles are both prespecified in
some way (i.e., their |mp_left_type| and |mp_right_type| aren't both open).

@c 
static void mp_make_choices (MP mp,pointer knots) {
  pointer h; /* the first breakpoint */
  pointer p,q; /* consecutive breakpoints being processed */
  @<Other local variables for |make_choices|@>;
  check_arith; /* make sure that |arith_error=false| */
  if ( mp->internal[mp_tracing_choices]>0 )
    mp_print_path(mp, knots,", before choices",true);
  @<If consecutive knots are equal, join them explicitly@>;
  @<Find the first breakpoint, |h|, on the path;
    insert an artificial breakpoint if the path is an unbroken cycle@>;
  p=h;
  do {  
    @<Fill in the control points between |p| and the next breakpoint,
      then advance |p| to that breakpoint@>;
  } while (p!=h);
  if ( mp->internal[mp_tracing_choices]>0 )
    mp_print_path(mp, knots,", after choices",true);
  if ( mp->arith_error ) {
    @<Report an unexpected problem during the choice-making@>;
  }
}

@ @<Report an unexpected problem during the choice...@>=
{ 
  print_err("Some number got too big");
@.Some number got too big@>
  help2("The path that I just computed is out of range.",
        "So it will probably look funny. Proceed, for a laugh.");
  mp_put_get_error(mp); mp->arith_error=false;
}

@ Two knots in a row with the same coordinates will always be joined
by an explicit ``curve'' whose control points are identical with the
knots.

@<If consecutive knots are equal, join them explicitly@>=
p=knots;
do {  
  q=mp_link(p);
  if ( mp_x_coord(p)==mp_x_coord(q) && 
       mp_y_coord(p)==mp_y_coord(q) && mp_right_type(p)>mp_explicit ) { 
    mp_right_type(p)=mp_explicit;
    if ( mp_left_type(p)==mp_open ) { 
      mp_left_type(p)=mp_curl; left_curl(p)=unity;
    }
    mp_left_type(q)=mp_explicit;
    if ( mp_right_type(q)==mp_open ) { 
      mp_right_type(q)=mp_curl; right_curl(q)=unity;
    }
    mp_right_x(p)=mp_x_coord(p); mp_left_x(q)=mp_x_coord(p);
    mp_right_y(p)=mp_y_coord(p); mp_left_y(q)=mp_y_coord(p);
  }
  p=q;
} while (p!=knots)

@ If there are no breakpoints, it is necessary to compute the direction
angles around an entire cycle. In this case the |mp_left_type| of the first
node is temporarily changed to |end_cycle|.

@<Find the first breakpoint, |h|, on the path...@>=
h=knots;
while (1) { 
  if ( mp_left_type(h)!=mp_open ) break;
  if ( mp_right_type(h)!=mp_open ) break;
  h=mp_link(h);
  if ( h==knots ) { 
    mp_left_type(h)=mp_end_cycle; break;
  }
}

@ If |mp_right_type(p)<given| and |q=mp_link(p)|, we must have
|mp_right_type(p)=mp_left_type(q)=mp_explicit| or |endpoint|.

@<Fill in the control points between |p| and the next breakpoint...@>=
q=mp_link(p);
if ( mp_right_type(p)>=mp_given ) { 
  while ( (mp_left_type(q)==mp_open)&&(mp_right_type(q)==mp_open) ) q=mp_link(q);
  @<Fill in the control information between
    consecutive breakpoints |p| and |q|@>;
} else if ( mp_right_type(p)==mp_endpoint ) {
  @<Give reasonable values for the unused control points between |p| and~|q|@>;
}
p=q

@ This step makes it possible to transform an explicitly computed path without
checking the |mp_left_type| and |mp_right_type| fields.

@<Give reasonable values for the unused control points between |p| and~|q|@>=
{ 
  mp_right_x(p)=mp_x_coord(p); mp_right_y(p)=mp_y_coord(p);
  mp_left_x(q)=mp_x_coord(q); mp_left_y(q)=mp_y_coord(q);
}

@ Before we can go further into the way choices are made, we need to
consider the underlying theory. The basic ideas implemented in |make_choices|
are due to John Hobby, who introduced the notion of ``mock curvature''
@^Hobby, John Douglas@>
at a knot. Angles are chosen so that they preserve mock curvature when
a knot is passed, and this has been found to produce excellent results.

It is convenient to introduce some notations that simplify the necessary
formulas. Let $d_{k,k+1}=\vert z\k-z_k\vert$ be the (nonzero) distance
between knots |k| and |k+1|; and let
$${z\k-z_k\over z_k-z_{k-1}}={d_{k,k+1}\over d_{k-1,k}}e^{i\psi_k}$$
so that a polygonal line from $z_{k-1}$ to $z_k$ to $z\k$ turns left
through an angle of~$\psi_k$. We assume that $\vert\psi_k\vert\L180^\circ$.
The control points for the spline from $z_k$ to $z\k$ will be denoted by
$$\eqalign{z_k^+&=z_k+
  \textstyle{1\over3}\rho_k e^{i\theta_k}(z\k-z_k),\cr
 z\k^-&=z\k-
  \textstyle{1\over3}\sigma\k e^{-i\phi\k}(z\k-z_k),\cr}$$
where $\rho_k$ and $\sigma\k$ are nonnegative ``velocity ratios'' at the
beginning and end of the curve, while $\theta_k$ and $\phi\k$ are the
corresponding ``offset angles.'' These angles satisfy the condition
$$\theta_k+\phi_k+\psi_k=0,\eqno(*)$$
whenever the curve leaves an intermediate knot~|k| in the direction that
it enters.

@ Let $\alpha_k$ and $\beta\k$ be the reciprocals of the ``tension'' of
the curve at its beginning and ending points. This means that
$\rho_k=\alpha_k f(\theta_k,\phi\k)$ and $\sigma\k=\beta\k f(\phi\k,\theta_k)$,
where $f(\theta,\phi)$ is \MP's standard velocity function defined in
the |velocity| subroutine. The cubic spline $B(z_k^{\phantom+},z_k^+,
z\k^-,z\k^{\phantom+};t)$
has curvature
@^curvature@>
$${2\sigma\k\sin(\theta_k+\phi\k)-6\sin\theta_k\over\rho_k^2d_{k,k+1}}
\qquad{\rm and}\qquad
{2\rho_k\sin(\theta_k+\phi\k)-6\sin\phi\k\over\sigma\k^2d_{k,k+1}}$$
at |t=0| and |t=1|, respectively. The mock curvature is the linear
@^mock curvature@>
approximation to this true curvature that arises in the limit for
small $\theta_k$ and~$\phi\k$, if second-order terms are discarded.
The standard velocity function satisfies
$$f(\theta,\phi)=1+O(\theta^2+\theta\phi+\phi^2);$$
hence the mock curvatures are respectively
$${2\beta\k(\theta_k+\phi\k)-6\theta_k\over\alpha_k^2d_{k,k+1}}
\qquad{\rm and}\qquad
{2\alpha_k(\theta_k+\phi\k)-6\phi\k\over\beta\k^2d_{k,k+1}}.\eqno(**)$$

@ The turning angles $\psi_k$ are given, and equation $(*)$ above
determines $\phi_k$ when $\theta_k$ is known, so the task of
angle selection is essentially to choose appropriate values for each
$\theta_k$. When equation~$(*)$ is used to eliminate $\phi$~variables
from $(**)$, we obtain a system of linear equations of the form
$$A_k\theta_{k-1}+(B_k+C_k)\theta_k+D_k\theta\k=-B_k\psi_k-D_k\psi\k,$$
where
$$A_k={\alpha_{k-1}\over\beta_k^2d_{k-1,k}},
\qquad B_k={3-\alpha_{k-1}\over\beta_k^2d_{k-1,k}},
\qquad C_k={3-\beta\k\over\alpha_k^2d_{k,k+1}},
\qquad D_k={\beta\k\over\alpha_k^2d_{k,k+1}}.$$
The tensions are always $3\over4$ or more, hence each $\alpha$ and~$\beta$
will be at most $4\over3$. It follows that $B_k\G{5\over4}A_k$ and
$C_k\G{5\over4}D_k$; hence the equations are diagonally dominant;
hence they have a unique solution. Moreover, in most cases the tensions
are equal to~1, so that $B_k=2A_k$ and $C_k=2D_k$. This makes the
solution numerically stable, and there is an exponential damping
effect: The data at knot $k\pm j$ affects the angle at knot~$k$ by
a factor of~$O(2^{-j})$.

@ However, we still must consider the angles at the starting and ending
knots of a non-cyclic path. These angles might be given explicitly, or
they might be specified implicitly in terms of an amount of ``curl.''

Let's assume that angles need to be determined for a non-cyclic path
starting at $z_0$ and ending at~$z_n$. Then equations of the form
$$A_k\theta_{k-1}+(B_k+C_k)\theta_k+D_k\theta_{k+1}=R_k$$
have been given for $0<k<n$, and it will be convenient to introduce
equations of the same form for $k=0$ and $k=n$, where
$$A_0=B_0=C_n=D_n=0.$$
If $\theta_0$ is supposed to have a given value $E_0$, we simply
define $C_0=1$, $D_0=0$, and $R_0=E_0$. Otherwise a curl
parameter, $\gamma_0$, has been specified at~$z_0$; this means
that the mock curvature at $z_0$ should be $\gamma_0$ times the
mock curvature at $z_1$; i.e.,
$${2\beta_1(\theta_0+\phi_1)-6\theta_0\over\alpha_0^2d_{01}}
=\gamma_0{2\alpha_0(\theta_0+\phi_1)-6\phi_1\over\beta_1^2d_{01}}.$$
This equation simplifies to
$$(\alpha_0\chi_0+3-\beta_1)\theta_0+
 \bigl((3-\alpha_0)\chi_0+\beta_1\bigr)\theta_1=
 -\bigl((3-\alpha_0)\chi_0+\beta_1\bigr)\psi_1,$$
where $\chi_0=\alpha_0^2\gamma_0/\beta_1^2$; so we can set $C_0=
\chi_0\alpha_0+3-\beta_1$, $D_0=(3-\alpha_0)\chi_0+\beta_1$, $R_0=-D_0\psi_1$.
It can be shown that $C_0>0$ and $C_0B_1-A_1D_0>0$ when $\gamma_0\G0$,
hence the linear equations remain nonsingular.

Similar considerations apply at the right end, when the final angle $\phi_n$
may or may not need to be determined. It is convenient to let $\psi_n=0$,
hence $\theta_n=-\phi_n$. We either have an explicit equation $\theta_n=E_n$,
or we have
$$\bigl((3-\beta_n)\chi_n+\alpha_{n-1}\bigr)\theta_{n-1}+
(\beta_n\chi_n+3-\alpha_{n-1})\theta_n=0,\qquad
  \chi_n={\beta_n^2\gamma_n\over\alpha_{n-1}^2}.$$

When |make_choices| chooses angles, it must compute the coefficients of
these linear equations, then solve the equations. To compute the coefficients,
it is necessary to compute arctangents of the given turning angles~$\psi_k$.
When the equations are solved, the chosen directions $\theta_k$ are put
back into the form of control points by essentially computing sines and
cosines.

@ OK, we are ready to make the hard choices of |make_choices|.
Most of the work is relegated to an auxiliary procedure
called |solve_choices|, which has been introduced to keep
|make_choices| from being extremely long.

@<Fill in the control information between...@>=
@<Calculate the turning angles $\psi_k$ and the distances $d_{k,k+1}$;
  set $n$ to the length of the path@>;
@<Remove |open| types at the breakpoints@>;
mp_solve_choices(mp, p,q,n)

@ It's convenient to precompute quantities that will be needed several
times later. The values of |delta_x[k]| and |delta_y[k]| will be the
coordinates of $z\k-z_k$, and the magnitude of this vector will be
|delta[k]=@t$d_{k,k+1}$@>|. The path angle $\psi_k$ between $z_k-z_{k-1}$
and $z\k-z_k$ will be stored in |psi[k]|.

@<Glob...@>=
int path_size; /* maximum number of knots between breakpoints of a path */
scaled *delta_x;
scaled *delta_y;
scaled *delta; /* knot differences */
angle  *psi; /* turning angles */

@ @<Dealloc variables@>=
xfree(mp->delta_x);
xfree(mp->delta_y);
xfree(mp->delta);
xfree(mp->psi);

@ @<Other local variables for |make_choices|@>=
  int k,n; /* current and final knot numbers */
  pointer s,t; /* registers for list traversal */
  scaled delx,dely; /* directions where |open| meets |explicit| */
  fraction sine,cosine; /* trig functions of various angles */

@ @<Calculate the turning angles...@>=
{
RESTART:
  k=0; s=p; n=mp->path_size;
  do {  
    t=mp_link(s);
    mp->delta_x[k]=mp_x_coord(t)-mp_x_coord(s);
    mp->delta_y[k]=mp_y_coord(t)-mp_y_coord(s);
    mp->delta[k]=mp_pyth_add(mp, mp->delta_x[k],mp->delta_y[k]);
    if ( k>0 ) { 
      sine=mp_make_fraction(mp, mp->delta_y[k-1],mp->delta[k-1]);
      cosine=mp_make_fraction(mp, mp->delta_x[k-1],mp->delta[k-1]);
      mp->psi[k]=mp_n_arg(mp, mp_take_fraction(mp, mp->delta_x[k],cosine)+
        mp_take_fraction(mp, mp->delta_y[k],sine),
        mp_take_fraction(mp, mp->delta_y[k],cosine)-
          mp_take_fraction(mp, mp->delta_x[k],sine));
    }
    incr(k); s=t;
    if ( k==mp->path_size ) {
      mp_reallocate_paths(mp, mp->path_size+(mp->path_size/4));
      goto RESTART; /* retry, loop size has changed */
    }
    if ( s==q ) n=k;
  } while (!((k>=n)&&(mp_left_type(s)!=mp_end_cycle)));
  if ( k==n ) mp->psi[n]=0; else mp->psi[k]=mp->psi[1];
}

@ When we get to this point of the code, |mp_right_type(p)| is either
|given| or |curl| or |open|. If it is |open|, we must have
|mp_left_type(p)=mp_end_cycle| or |mp_left_type(p)=mp_explicit|. In the latter
case, the |open| type is converted to |given|; however, if the
velocity coming into this knot is zero, the |open| type is
converted to a |curl|, since we don't know the incoming direction.

Similarly, |mp_left_type(q)| is either |given| or |curl| or |open| or
|mp_end_cycle|. The |open| possibility is reduced either to |given| or to |curl|.

@<Remove |open| types at the breakpoints@>=
if ( mp_left_type(q)==mp_open ) { 
  delx=mp_right_x(q)-mp_x_coord(q); dely=mp_right_y(q)-mp_y_coord(q);
  if ( (delx==0)&&(dely==0) ) { 
    mp_left_type(q)=mp_curl; left_curl(q)=unity;
  } else { 
    mp_left_type(q)=mp_given; left_given(q)=mp_n_arg(mp, delx,dely);
  }
}
if ( (mp_right_type(p)==mp_open)&&(mp_left_type(p)==mp_explicit) ) { 
  delx=mp_x_coord(p)-mp_left_x(p); dely=mp_y_coord(p)-mp_left_y(p);
  if ( (delx==0)&&(dely==0) ) { 
    mp_right_type(p)=mp_curl; right_curl(p)=unity;
  } else { 
    mp_right_type(p)=mp_given; right_given(p)=mp_n_arg(mp, delx,dely);
  }
}

@ Linear equations need to be solved whenever |n>1|; and also when |n=1|
and exactly one of the breakpoints involves a curl. The simplest case occurs
when |n=1| and there is a curl at both breakpoints; then we simply draw
a straight line.

But before coding up the simple cases, we might as well face the general case,
since we must deal with it sooner or later, and since the general case
is likely to give some insight into the way simple cases can be handled best.

When there is no cycle, the linear equations to be solved form a tridiagonal
system, and we can apply the standard technique of Gaussian elimination
to convert that system to a sequence of equations of the form
$$\theta_0+u_0\theta_1=v_0,\quad
\theta_1+u_1\theta_2=v_1,\quad\ldots,\quad
\theta_{n-1}+u_{n-1}\theta_n=v_{n-1},\quad
\theta_n=v_n.$$
It is possible to do this diagonalization while generating the equations.
Once $\theta_n$ is known, it is easy to determine $\theta_{n-1}$, \dots,
$\theta_1$, $\theta_0$; thus, the equations will be solved.

The procedure is slightly more complex when there is a cycle, but the
basic idea will be nearly the same. In the cyclic case the right-hand
sides will be $v_k+w_k\theta_0$ instead of simply $v_k$, and we will start
the process off with $u_0=v_0=0$, $w_0=1$. The final equation will be not
$\theta_n=v_n$ but $\theta_n+u_n\theta_1=v_n+w_n\theta_0$; an appropriate
ending routine will take account of the fact that $\theta_n=\theta_0$ and
eliminate the $w$'s from the system, after which the solution can be
obtained as before.

When $u_k$, $v_k$, and $w_k$ are being computed, the three pointer
variables |r|, |s|,~|t| will point respectively to knots |k-1|, |k|,
and~|k+1|. The $u$'s and $w$'s are scaled by $2^{28}$, i.e., they are
of type |fraction|; the $\theta$'s and $v$'s are of type |angle|.

@<Glob...@>=
angle *theta; /* values of $\theta_k$ */
fraction *uu; /* values of $u_k$ */
angle *vv; /* values of $v_k$ */
fraction *ww; /* values of $w_k$ */

@ @<Dealloc variables@>=
xfree(mp->theta);
xfree(mp->uu);
xfree(mp->vv);
xfree(mp->ww);

@ @<Declarations@>=
static void mp_reallocate_paths (MP mp, int l);

@ @c
void mp_reallocate_paths (MP mp, int l) {
  XREALLOC (mp->delta_x, l, scaled);
  XREALLOC (mp->delta_y, l, scaled);
  XREALLOC (mp->delta,   l, scaled);
  XREALLOC (mp->psi,     l, angle);
  XREALLOC (mp->theta,   l, angle);
  XREALLOC (mp->uu,      l, fraction);
  XREALLOC (mp->vv,      l, angle);
  XREALLOC (mp->ww,      l, fraction);
  mp->path_size = l;
}

@ Our immediate problem is to get the ball rolling by setting up the
first equation or by realizing that no equations are needed, and to fit
this initialization into a framework suitable for the overall computation.

@<Declarations@>=
static void mp_solve_choices (MP mp,pointer p, pointer q, halfword n) ;

@ @c
void mp_solve_choices (MP mp,pointer p, pointer q, halfword n) {
  int k; /* current knot number */
  pointer r,s,t; /* registers for list traversal */
  @<Other local variables for |solve_choices|@>;
  k=0; s=p; r=0;
  while (1) { 
    t=mp_link(s);
    if ( k==0 ) {
      @<Get the linear equations started; or |return|
        with the control points in place, if linear equations
        needn't be solved@>
    } else  { 
      switch (mp_left_type(s)) {
      case mp_end_cycle: case mp_open:
        @<Set up equation to match mock curvatures
          at $z_k$; then |goto found| with $\theta_n$
          adjusted to equal $\theta_0$, if a cycle has ended@>;
        break;
      case mp_curl:
        @<Set up equation for a curl at $\theta_n$
          and |goto found|@>;
        break;
      case mp_given:
        @<Calculate the given value of $\theta_n$
          and |goto found|@>;
        break;
      } /* there are no other cases */
    }
    r=s; s=t; incr(k);
  }
FOUND:
  @<Finish choosing angles and assigning control points@>;
}

@ On the first time through the loop, we have |k=0| and |r| is not yet
defined. The first linear equation, if any, will have $A_0=B_0=0$.

@<Get the linear equations started...@>=
switch (mp_right_type(s)) {
case mp_given: 
  if ( mp_left_type(t)==mp_given ) {
    @<Reduce to simple case of two givens  and |return|@>
  } else {
    @<Set up the equation for a given value of $\theta_0$@>;
  }
  break;
case mp_curl: 
  if ( mp_left_type(t)==mp_curl ) {
    @<Reduce to simple case of straight line and |return|@>
  } else {
    @<Set up the equation for a curl at $\theta_0$@>;
  }
  break;
case mp_open: 
  mp->uu[0]=0; mp->vv[0]=0; mp->ww[0]=fraction_one;
  /* this begins a cycle */
  break;
} /* there are no other cases */

@ The general equation that specifies equality of mock curvature at $z_k$ is
$$A_k\theta_{k-1}+(B_k+C_k)\theta_k+D_k\theta\k=-B_k\psi_k-D_k\psi\k,$$
as derived above. We want to combine this with the already-derived equation
$\theta_{k-1}+u_{k-1}\theta_k=v_{k-1}+w_{k-1}\theta_0$ in order to obtain
a new equation
$\theta_k+u_k\theta\k=v_k+w_k\theta_0$. This can be done by dividing the
equation
$$(B_k-u_{k-1}A_k+C_k)\theta_k+D_k\theta\k=-B_k\psi_k-D_k\psi\k-A_kv_{k-1}
    -A_kw_{k-1}\theta_0$$
by $B_k-u_{k-1}A_k+C_k$. The trick is to do this carefully with
fixed-point arithmetic, avoiding the chance of overflow while retaining
suitable precision.

The calculations will be performed in several registers that
provide temporary storage for intermediate quantities.

@<Other local variables for |solve_choices|@>=
fraction aa,bb,cc,ff,acc; /* temporary registers */
scaled dd,ee; /* likewise, but |scaled| */
scaled lt,rt; /* tension values */

@ @<Set up equation to match mock curvatures...@>=
{ @<Calculate the values $\\{aa}=A_k/B_k$, $\\{bb}=D_k/C_k$,
    $\\{dd}=(3-\alpha_{k-1})d_{k,k+1}$, $\\{ee}=(3-\beta\k)d_{k-1,k}$,
    and $\\{cc}=(B_k-u_{k-1}A_k)/B_k$@>;
  @<Calculate the ratio $\\{ff}=C_k/(C_k+B_k-u_{k-1}A_k)$@>;
  mp->uu[k]=mp_take_fraction(mp, ff,bb);
  @<Calculate the values of $v_k$ and $w_k$@>;
  if ( mp_left_type(s)==mp_end_cycle ) {
    @<Adjust $\theta_n$ to equal $\theta_0$ and |goto found|@>;
  }
}

@ Since tension values are never less than 3/4, the values |aa| and
|bb| computed here are never more than 4/5.

@<Calculate the values $\\{aa}=...@>=
if ( abs(right_tension(r))==unity) { 
  aa=fraction_half; dd=2*mp->delta[k];
} else { 
  aa=mp_make_fraction(mp, unity,3*abs(right_tension(r))-unity);
  dd=mp_take_fraction(mp, mp->delta[k],
    fraction_three-mp_make_fraction(mp, unity,abs(right_tension(r))));
}
if ( abs(left_tension(t))==unity ){ 
  bb=fraction_half; ee=2*mp->delta[k-1];
} else { 
  bb=mp_make_fraction(mp, unity,3*abs(left_tension(t))-unity);
  ee=mp_take_fraction(mp, mp->delta[k-1],
    fraction_three-mp_make_fraction(mp, unity,abs(left_tension(t))));
}
cc=fraction_one-mp_take_fraction(mp, mp->uu[k-1],aa)

@ The ratio to be calculated in this step can be written in the form
$$\beta_k^2\cdot\\{ee}\over\beta_k^2\cdot\\{ee}+\alpha_k^2\cdot
  \\{cc}\cdot\\{dd},$$
because of the quantities just calculated. The values of |dd| and |ee|
will not be needed after this step has been performed.

@<Calculate the ratio $\\{ff}=C_k/(C_k+B_k-u_{k-1}A_k)$@>=
dd=mp_take_fraction(mp, dd,cc); lt=abs(left_tension(s)); rt=abs(right_tension(s));
if ( lt!=rt ) { /* $\beta_k^{-1}\ne\alpha_k^{-1}$ */
  if ( lt<rt ) { 
    ff=mp_make_fraction(mp, lt,rt);
    ff=mp_take_fraction(mp, ff,ff); /* $\alpha_k^2/\beta_k^2$ */
    dd=mp_take_fraction(mp, dd,ff);
  } else { 
    ff=mp_make_fraction(mp, rt,lt);
    ff=mp_take_fraction(mp, ff,ff); /* $\beta_k^2/\alpha_k^2$ */
    ee=mp_take_fraction(mp, ee,ff);
  }
}
ff=mp_make_fraction(mp, ee,ee+dd)

@ The value of $u_{k-1}$ will be |<=1| except when $k=1$ and the previous
equation was specified by a curl. In that case we must use a special
method of computation to prevent overflow.

Fortunately, the calculations turn out to be even simpler in this ``hard''
case. The curl equation makes $w_0=0$ and $v_0=-u_0\psi_1$, hence
$-B_1\psi_1-A_1v_0=-(B_1-u_0A_1)\psi_1=-\\{cc}\cdot B_1\psi_1$.

@<Calculate the values of $v_k$ and $w_k$@>=
acc=-mp_take_fraction(mp, mp->psi[k+1],mp->uu[k]);
if ( mp_right_type(r)==mp_curl ) { 
  mp->ww[k]=0;
  mp->vv[k]=acc-mp_take_fraction(mp, mp->psi[1],fraction_one-ff);
} else { 
  ff=mp_make_fraction(mp, fraction_one-ff,cc); /* this is
    $B_k/(C_k+B_k-u_{k-1}A_k)<5$ */
  acc=acc-mp_take_fraction(mp, mp->psi[k],ff);
  ff=mp_take_fraction(mp, ff,aa); /* this is $A_k/(C_k+B_k-u_{k-1}A_k)$ */
  mp->vv[k]=acc-mp_take_fraction(mp, mp->vv[k-1],ff);
  if ( mp->ww[k-1]==0 ) mp->ww[k]=0;
  else mp->ww[k]=-mp_take_fraction(mp, mp->ww[k-1],ff);
}

@ When a complete cycle has been traversed, we have $\theta_k+u_k\theta\k=
v_k+w_k\theta_0$, for |1<=k<=n|. We would like to determine the value of
$\theta_n$ and reduce the system to the form $\theta_k+u_k\theta\k=v_k$
for |0<=k<n|, so that the cyclic case can be finished up just as if there
were no cycle.

The idea in the following code is to observe that
$$\eqalign{\theta_n&=v_n+w_n\theta_0-u_n\theta_1=\cdots\cr
&=v_n+w_n\theta_0-u_n\bigl(v_1+w_1\theta_0-u_1(v_2+\cdots
  -u_{n-2}(v_{n-1}+w_{n-1}\theta_0-u_{n-1}\theta_0))\bigr),\cr}$$
so we can solve for $\theta_n=\theta_0$.

@<Adjust $\theta_n$ to equal $\theta_0$ and |goto found|@>=
{ 
aa=0; bb=fraction_one; /* we have |k=n| */
do {  decr(k);
if ( k==0 ) k=n;
  aa=mp->vv[k]-mp_take_fraction(mp, aa,mp->uu[k]);
  bb=mp->ww[k]-mp_take_fraction(mp, bb,mp->uu[k]);
} while (k!=n); /* now $\theta_n=\\{aa}+\\{bb}\cdot\theta_n$ */
aa=mp_make_fraction(mp, aa,fraction_one-bb);
mp->theta[n]=aa; mp->vv[0]=aa;
for (k=1;k<n;k++) {
  mp->vv[k]=mp->vv[k]+mp_take_fraction(mp, aa,mp->ww[k]);
}
goto FOUND;
}

@ @d reduce_angle(A) if ( abs((A))>one_eighty_deg ) {
  if ( (A)>0 ) (A)=(A)-three_sixty_deg; else (A)=(A)+three_sixty_deg; }

@<Calculate the given value of $\theta_n$...@>=
{ 
  mp->theta[n]=left_given(s)-mp_n_arg(mp, mp->delta_x[n-1],mp->delta_y[n-1]);
  reduce_angle(mp->theta[n]);
  goto FOUND;
}

@ @<Set up the equation for a given value of $\theta_0$@>=
{ 
  mp->vv[0]=right_given(s)-mp_n_arg(mp, mp->delta_x[0],mp->delta_y[0]);
  reduce_angle(mp->vv[0]);
  mp->uu[0]=0; mp->ww[0]=0;
}

@ @<Set up the equation for a curl at $\theta_0$@>=
{ cc=right_curl(s); lt=abs(left_tension(t)); rt=abs(right_tension(s));
  if ( (rt==unity)&&(lt==unity) )
    mp->uu[0]=mp_make_fraction(mp, cc+cc+unity,cc+two);
  else 
    mp->uu[0]=mp_curl_ratio(mp, cc,rt,lt);
  mp->vv[0]=-mp_take_fraction(mp, mp->psi[1],mp->uu[0]); mp->ww[0]=0;
}

@ @<Set up equation for a curl at $\theta_n$...@>=
{ cc=left_curl(s); lt=abs(left_tension(s)); rt=abs(right_tension(r));
  if ( (rt==unity)&&(lt==unity) )
    ff=mp_make_fraction(mp, cc+cc+unity,cc+two);
  else 
    ff=mp_curl_ratio(mp, cc,lt,rt);
  mp->theta[n]=-mp_make_fraction(mp, mp_take_fraction(mp, mp->vv[n-1],ff),
    fraction_one-mp_take_fraction(mp, ff,mp->uu[n-1]));
  goto FOUND;
}

@ The |curl_ratio| subroutine has three arguments, which our previous notation
encourages us to call $\gamma$, $\alpha^{-1}$, and $\beta^{-1}$. It is
a somewhat tedious program to calculate
$${(3-\alpha)\alpha^2\gamma+\beta^3\over
  \alpha^3\gamma+(3-\beta)\beta^2},$$
with the result reduced to 4 if it exceeds 4. (This reduction of curl
is necessary only if the curl and tension are both large.)
The values of $\alpha$ and $\beta$ will be at most~4/3.

@<Declarations@>=
static fraction mp_curl_ratio (MP mp,scaled gamma, scaled a_tension, 
                        scaled b_tension) ;

@ @c
fraction mp_curl_ratio (MP mp,scaled gamma, scaled a_tension, 
                        scaled b_tension) {
  fraction alpha,beta,num,denom,ff; /* registers */
  alpha=mp_make_fraction(mp, unity,a_tension);
  beta=mp_make_fraction(mp, unity,b_tension);
  if ( alpha<=beta ) {
    ff=mp_make_fraction(mp, alpha,beta); ff=mp_take_fraction(mp, ff,ff);
    gamma=mp_take_fraction(mp, gamma,ff);
    beta=beta / 010000; /* convert |fraction| to |scaled| */
    denom=mp_take_fraction(mp, gamma,alpha)+three-beta;
    num=mp_take_fraction(mp, gamma,fraction_three-alpha)+beta;
  } else { 
    ff=mp_make_fraction(mp, beta,alpha); ff=mp_take_fraction(mp, ff,ff);
    beta=mp_take_fraction(mp, beta,ff) / 010000; /* convert |fraction| to |scaled| */
    denom=mp_take_fraction(mp, gamma,alpha)+(ff / 1365)-beta;
      /* $1365\approx 2^{12}/3$ */
    num=mp_take_fraction(mp, gamma,fraction_three-alpha)+beta;
  }
  if ( num>=denom+denom+denom+denom ) return fraction_four;
  else return mp_make_fraction(mp, num,denom);
}

@ We're in the home stretch now.

@<Finish choosing angles and assigning control points@>=
for (k=n-1;k>=0;k--) {
  mp->theta[k]=mp->vv[k]-mp_take_fraction(mp,mp->theta[k+1],mp->uu[k]);
}
s=p; k=0;
do {  
  t=mp_link(s);
  mp_n_sin_cos(mp, mp->theta[k]); mp->st=mp->n_sin; mp->ct=mp->n_cos;
  mp_n_sin_cos(mp, -mp->psi[k+1]-mp->theta[k+1]); mp->sf=mp->n_sin; mp->cf=mp->n_cos;
  mp_set_controls(mp, s,t,k);
  incr(k); s=t;
} while (k!=n)

@ The |set_controls| routine actually puts the control points into
a pair of consecutive nodes |p| and~|q|. Global variables are used to
record the values of $\sin\theta$, $\cos\theta$, $\sin\phi$, and
$\cos\phi$ needed in this calculation.

@<Glob...@>=
fraction st;
fraction ct;
fraction sf;
fraction cf; /* sines and cosines */

@ @<Declarations@>=
static void mp_set_controls (MP mp,pointer p, pointer q, integer k);

@ @c
void mp_set_controls (MP mp,pointer p, pointer q, integer k) {
  fraction rr,ss; /* velocities, divided by thrice the tension */
  scaled lt,rt; /* tensions */
  fraction sine; /* $\sin(\theta+\phi)$ */
  lt=abs(left_tension(q)); rt=abs(right_tension(p));
  rr=mp_velocity(mp, mp->st,mp->ct,mp->sf,mp->cf,rt);
  ss=mp_velocity(mp, mp->sf,mp->cf,mp->st,mp->ct,lt);
  if ( (right_tension(p)<0)||(left_tension(q)<0) ) {
    @<Decrease the velocities,
      if necessary, to stay inside the bounding triangle@>;
  }
  mp_right_x(p)=mp_x_coord(p)+mp_take_fraction(mp, 
                          mp_take_fraction(mp, mp->delta_x[k],mp->ct)-
                          mp_take_fraction(mp, mp->delta_y[k],mp->st),rr);
  mp_right_y(p)=mp_y_coord(p)+mp_take_fraction(mp, 
                          mp_take_fraction(mp, mp->delta_y[k],mp->ct)+
                          mp_take_fraction(mp, mp->delta_x[k],mp->st),rr);
  mp_left_x(q)=mp_x_coord(q)-mp_take_fraction(mp, 
                         mp_take_fraction(mp, mp->delta_x[k],mp->cf)+
                         mp_take_fraction(mp, mp->delta_y[k],mp->sf),ss);
  mp_left_y(q)=mp_y_coord(q)-mp_take_fraction(mp, 
                         mp_take_fraction(mp, mp->delta_y[k],mp->cf)-
                         mp_take_fraction(mp, mp->delta_x[k],mp->sf),ss);
  mp_right_type(p)=mp_explicit; mp_left_type(q)=mp_explicit;
}

@ The boundedness conditions $\\{rr}\L\sin\phi\,/\sin(\theta+\phi)$ and
$\\{ss}\L\sin\theta\,/\sin(\theta+\phi)$ are to be enforced if $\sin\theta$,
$\sin\phi$, and $\sin(\theta+\phi)$ all have the same sign. Otherwise
there is no ``bounding triangle.''

@<Decrease the velocities, if necessary...@>=
if (((mp->st>=0)&&(mp->sf>=0))||((mp->st<=0)&&(mp->sf<=0)) ) {
  sine=mp_take_fraction(mp, abs(mp->st),mp->cf)+
                            mp_take_fraction(mp, abs(mp->sf),mp->ct);
  if ( sine>0 ) {
    sine=mp_take_fraction(mp, sine,fraction_one+unity); /* safety factor */
    if ( right_tension(p)<0 )
     if ( mp_ab_vs_cd(mp, abs(mp->sf),fraction_one,rr,sine)<0 )
      rr=mp_make_fraction(mp, abs(mp->sf),sine);
    if ( left_tension(q)<0 )
     if ( mp_ab_vs_cd(mp, abs(mp->st),fraction_one,ss,sine)<0 )
      ss=mp_make_fraction(mp, abs(mp->st),sine);
  }
}

@ Only the simple cases remain to be handled.

@<Reduce to simple case of two givens and |return|@>=
{ 
  aa=mp_n_arg(mp, mp->delta_x[0],mp->delta_y[0]);
  mp_n_sin_cos(mp, right_given(p)-aa); mp->ct=mp->n_cos; mp->st=mp->n_sin;
  mp_n_sin_cos(mp, left_given(q)-aa); mp->cf=mp->n_cos; mp->sf=-mp->n_sin;
  mp_set_controls(mp, p,q,0); return;
}

@ @<Reduce to simple case of straight line and |return|@>=
{ 
  mp_right_type(p)=mp_explicit; mp_left_type(q)=mp_explicit;
  lt=abs(left_tension(q)); rt=abs(right_tension(p));
  if ( rt==unity ) {
    if ( mp->delta_x[0]>=0 ) mp_right_x(p)=mp_x_coord(p)+((mp->delta_x[0]+1) / 3);
    else mp_right_x(p)=mp_x_coord(p)+((mp->delta_x[0]-1) / 3);
    if ( mp->delta_y[0]>=0 ) mp_right_y(p)=mp_y_coord(p)+((mp->delta_y[0]+1) / 3);
    else mp_right_y(p)=mp_y_coord(p)+((mp->delta_y[0]-1) / 3);
  } else { 
    ff=mp_make_fraction(mp, unity,3*rt); /* $\alpha/3$ */
    mp_right_x(p)=mp_x_coord(p)+mp_take_fraction(mp, mp->delta_x[0],ff);
    mp_right_y(p)=mp_y_coord(p)+mp_take_fraction(mp, mp->delta_y[0],ff);
  }
  if ( lt==unity ) {
    if ( mp->delta_x[0]>=0 ) mp_left_x(q)=mp_x_coord(q)-((mp->delta_x[0]+1) / 3);
    else mp_left_x(q)=mp_x_coord(q)-((mp->delta_x[0]-1) / 3);
    if ( mp->delta_y[0]>=0 ) mp_left_y(q)=mp_y_coord(q)-((mp->delta_y[0]+1) / 3);
    else mp_left_y(q)=mp_y_coord(q)-((mp->delta_y[0]-1) / 3);
  } else  { 
    ff=mp_make_fraction(mp, unity,3*lt); /* $\beta/3$ */
    mp_left_x(q)=mp_x_coord(q)-mp_take_fraction(mp, mp->delta_x[0],ff);
    mp_left_y(q)=mp_y_coord(q)-mp_take_fraction(mp, mp->delta_y[0],ff);
  }
  return;
}

@* \[19] Measuring paths.
\MP's \&{llcorner}, \&{lrcorner}, \&{ulcorner}, and \&{urcorner} operators
allow the user to measure the bounding box of anything that can go into a
picture.  It's easy to get rough bounds on the $x$ and $y$ extent of a path
by just finding the bounding box of the knots and the control points. We
need a more accurate version of the bounding box, but we can still use the
easy estimate to save time by focusing on the interesting parts of the path.

@ Computing an accurate bounding box involves a theme that will come up again
and again. Given a Bernshte{\u\i}n polynomial
@^Bernshte{\u\i}n, Serge{\u\i} Natanovich@>
$$B(z_0,z_1,\ldots,z_n;t)=\sum_k{n\choose k}t^k(1-t)^{n-k}z_k,$$
we can conveniently bisect its range as follows:

\smallskip
\textindent{1)} Let $z_k^{(0)}=z_k$, for |0<=k<=n|.

\smallskip
\textindent{2)} Let $z_k^{(j+1)}={1\over2}(z_k^{(j)}+z\k^{(j)})$, for
|0<=k<n-j|, for |0<=j<n|.

\smallskip\noindent
Then
$$B(z_0,z_1,\ldots,z_n;t)=B(z_0^{(0)},z_0^{(1)},\ldots,z_0^{(n)};2t)
 =B(z_0^{(n)},z_1^{(n-1)},\ldots,z_n^{(0)};2t-1).$$
This formula gives us the coefficients of polynomials to use over the ranges
$0\L t\L{1\over2}$ and ${1\over2}\L t\L1$.

@ Now here's a subroutine that's handy for all sorts of path computations:
Given a quadratic polynomial $B(a,b,c;t)$, the |crossing_point| function
returns the unique |fraction| value |t| between 0 and~1 at which
$B(a,b,c;t)$ changes from positive to negative, or returns
|t=fraction_one+1| if no such value exists. If |a<0| (so that $B(a,b,c;t)$
is already negative at |t=0|), |crossing_point| returns the value zero.

@d no_crossing {  return (fraction_one+1); }
@d one_crossing { return fraction_one; }
@d zero_crossing { return 0; }
@d mp_crossing_point(M,A,B,C) mp_do_crossing_point(A,B,C)

@c static fraction mp_do_crossing_point (integer a, integer b, integer c) {
  integer d; /* recursive counter */
  integer x,xx,x0,x1,x2; /* temporary registers for bisection */
  if ( a<0 ) zero_crossing;
  if ( c>=0 ) { 
    if ( b>=0 ) {
      if ( c>0 ) { no_crossing; }
      else if ( (a==0)&&(b==0) ) { no_crossing;} 
      else { one_crossing; } 
    }
    if ( a==0 ) zero_crossing;
  } else if ( a==0 ) {
    if ( b<=0 ) zero_crossing;
  }
  @<Use bisection to find the crossing point, if one exists@>;
}

@ The general bisection method is quite simple when $n=2$, hence
|crossing_point| does not take much time. At each stage in the
recursion we have a subinterval defined by |l| and~|j| such that
$B(a,b,c;2^{-l}(j+t))=B(x_0,x_1,x_2;t)$, and we want to ``zero in'' on
the subinterval where $x_0\G0$ and $\min(x_1,x_2)<0$.

It is convenient for purposes of calculation to combine the values
of |l| and~|j| in a single variable $d=2^l+j$, because the operation
of bisection then corresponds simply to doubling $d$ and possibly
adding~1. Furthermore it proves to be convenient to modify
our previous conventions for bisection slightly, maintaining the
variables $X_0=2^lx_0$, $X_1=2^l(x_0-x_1)$, and $X_2=2^l(x_1-x_2)$.
With these variables the conditions $x_0\ge0$ and $\min(x_1,x_2)<0$ are
equivalent to $\max(X_1,X_1+X_2)>X_0\ge0$.

The following code maintains the invariant relations
$0\L|x0|<\max(|x1|,|x1|+|x2|)$,
$\vert|x1|\vert<2^{30}$, $\vert|x2|\vert<2^{30}$;
it has been constructed in such a way that no arithmetic overflow
will occur if the inputs satisfy
$a<2^{30}$, $\vert a-b\vert<2^{30}$, and $\vert b-c\vert<2^{30}$.

@<Use bisection to find the crossing point...@>=
d=1; x0=a; x1=a-b; x2=b-c;
do {  
  x=half(x1+x2);
  if ( x1-x0>x0 ) { 
    x2=x; x0+=x0; d+=d;  
  } else { 
    xx=x1+x-x0;
    if ( xx>x0 ) { 
      x2=x; x0+=x0; d+=d;
    }  else { 
      x0=x0-xx;
      if ( x<=x0 ) { if ( x+x2<=x0 ) no_crossing; }
      x1=x; d=d+d+1;
    }
  }
} while (d<fraction_one);
return (d-fraction_one)

@ Here is a routine that computes the $x$ or $y$ coordinate of the point on
a cubic corresponding to the |fraction| value~|t|.

It is convenient to define a \.{WEB} macro |t_of_the_way| such that
|t_of_the_way(a,b)| expands to |a-(a-b)*t|, i.e., to |t[a,b]|.

@d t_of_the_way(A,B) ((A)-mp_take_fraction(mp,((A)-(B)),t))

@c static scaled mp_eval_cubic (MP mp,pointer p, pointer q, fraction t) {
  scaled x1,x2,x3; /* intermediate values */
  x1=t_of_the_way(knot_coord(p),right_coord(p));
  x2=t_of_the_way(right_coord(p),left_coord(q));
  x3=t_of_the_way(left_coord(q),knot_coord(q));
  x1=t_of_the_way(x1,x2);
  x2=t_of_the_way(x2,x3);
  return t_of_the_way(x1,x2);
}

@ The actual bounding box information is stored in global variables.
Since it is convenient to address the $x$ and $y$ information
separately, we define arrays indexed by |x_code..y_code| and use
macros to give them more convenient names.

@<Types...@>=
enum mp_bb_code  {
  mp_x_code=0, /* index for |minx| and |maxx| */
  mp_y_code /* index for |miny| and |maxy| */
} ;

@ 
@d mp_minx mp->bbmin[mp_x_code]
@d mp_maxx mp->bbmax[mp_x_code]
@d mp_miny mp->bbmin[mp_y_code]
@d mp_maxy mp->bbmax[mp_y_code]

@<Glob...@>=
scaled bbmin[mp_y_code+1];
scaled bbmax[mp_y_code+1]; 
/* the result of procedures that compute bounding box information */

@ Now we're ready for the key part of the bounding box computation.
The |bound_cubic| procedure updates |bbmin[c]| and |bbmax[c]| based on
$$B(\hbox{|knot_coord(p)|}, \hbox{|right_coord(p)|},
    \hbox{|left_coord(q)|}, \hbox{|knot_coord(q)|};t)
$$
for $0<t\le1$.  In other words, the procedure adjusts the bounds to
accommodate |knot_coord(q)| and any extremes over the range $0<t<1$.
The |c| parameter is |x_code| or |y_code|.

@c static void mp_bound_cubic (MP mp,pointer p, pointer q, quarterword c) {
  boolean wavy; /* whether we need to look for extremes */
  scaled del1,del2,del3,del,dmax; /* proportional to the control
     points of a quadratic derived from a cubic */
  fraction t,tt; /* where a quadratic crosses zero */
  scaled x; /* a value that |bbmin[c]| and |bbmax[c]| must accommodate */
  x=knot_coord(q);
  @<Adjust |bbmin[c]| and |bbmax[c]| to accommodate |x|@>;
  @<Check the control points against the bounding box and set |wavy:=true|
    if any of them lie outside@>;
  if ( wavy ) {
    del1=right_coord(p)-knot_coord(p);
    del2=left_coord(q)-right_coord(p);
    del3=knot_coord(q)-left_coord(q);
    @<Scale up |del1|, |del2|, and |del3| for greater accuracy;
      also set |del| to the first nonzero element of |(del1,del2,del3)|@>;
    if ( del<0 ) {
      negate(del1); negate(del2); negate(del3);
    };
    t=mp_crossing_point(mp, del1,del2,del3);
    if ( t<fraction_one ) {
      @<Test the extremes of the cubic against the bounding box@>;
    }
  }
}

@ @<Adjust |bbmin[c]| and |bbmax[c]| to accommodate |x|@>=
if ( x<mp->bbmin[c] ) mp->bbmin[c]=x;
if ( x>mp->bbmax[c] ) mp->bbmax[c]=x

@ @<Check the control points against the bounding box and set...@>=
wavy=true;
if ( mp->bbmin[c]<=right_coord(p) )
  if ( right_coord(p)<=mp->bbmax[c] )
    if ( mp->bbmin[c]<=left_coord(q) )
      if ( left_coord(q)<=mp->bbmax[c] )
        wavy=false

@ If |del1=del2=del3=0|, it's impossible to obey the title of this
section. We just set |del=0| in that case.

@<Scale up |del1|, |del2|, and |del3| for greater accuracy...@>=
if ( del1!=0 ) del=del1;
else if ( del2!=0 ) del=del2;
else del=del3;
if ( del!=0 ) {
  dmax=abs(del1);
  if ( abs(del2)>dmax ) dmax=abs(del2);
  if ( abs(del3)>dmax ) dmax=abs(del3);
  while ( dmax<fraction_half ) {
    dmax+=dmax; del1+=del1; del2+=del2; del3+=del3;
  }
}

@ Since |crossing_point| has tried to choose |t| so that
$B(|del1|,|del2|,|del3|;\tau)$ crosses zero at $\tau=|t|$ with negative
slope, the value of |del2| computed below should not be positive.
But rounding error could make it slightly positive in which case we
must cut it to zero to avoid confusion.

@<Test the extremes of the cubic against the bounding box@>=
{ 
  x=mp_eval_cubic(mp, p,q,t);
  @<Adjust |bbmin[c]| and |bbmax[c]| to accommodate |x|@>;
  del2=t_of_the_way(del2,del3);
    /* now |0,del2,del3| represent the derivative on the remaining interval */
  if ( del2>0 ) del2=0;
  tt=mp_crossing_point(mp, 0,-del2,-del3);
  if ( tt<fraction_one ) {
    @<Test the second extreme against the bounding box@>;
  }
}

@ @<Test the second extreme against the bounding box@>=
{
   x=mp_eval_cubic(mp, p,q,t_of_the_way(tt,fraction_one));
  @<Adjust |bbmin[c]| and |bbmax[c]| to accommodate |x|@>;
}

@ Finding the bounding box of a path is basically a matter of applying
|bound_cubic| twice for each pair of adjacent knots.

@c static void mp_path_bbox (MP mp,pointer h) {
  pointer p,q; /* a pair of adjacent knots */
  mp_minx=mp_x_coord(h); mp_miny=mp_y_coord(h);
  mp_maxx=mp_minx; mp_maxy=mp_miny;
  p=h;
  do {  
    if ( mp_right_type(p)==mp_endpoint ) return;
    q=mp_link(p);
    mp_bound_cubic(mp, x_loc(p),x_loc(q),mp_x_code);
    mp_bound_cubic(mp, y_loc(p),y_loc(q),mp_y_code);
    p=q;
  } while (p!=h);
}

@ Another important way to measure a path is to find its arc length.  This
is best done by using the general bisection algorithm to subdivide the path
until obtaining ``well behaved'' subpaths whose arc lengths can be approximated
by simple means.

Since the arc length is the integral with respect to time of the magnitude of
the velocity, it is natural to use Simpson's rule for the approximation.
@^Simpson's rule@>
If $\dot B(t)$ is the spline velocity, Simpson's rule gives
$$ \vb\dot B(0)\vb + 4\vb\dot B({1\over2})\vb + \vb\dot B(1)\vb \over 6 $$
for the arc length of a path of length~1.  For a cubic spline
$B(z_0,z_1,z_2,z_3;t)$, the time derivative $\dot B(t)$ is
$3B(dz_0,dz_1,dz_2;t)$, where $dz_i=z_{i+1}-z_i$.  Hence the arc length
approximation is
$$ {\vb dz_0\vb \over 2} + 2\vb dz_{02}\vb + {\vb dz_2\vb \over 2}, $$
where
$$ dz_{02}={1\over2}\left({dz_0+dz_1\over 2}+{dz_1+dz_2\over 2}\right)$$
is the result of the bisection algorithm.

@ The remaining problem is how to decide when a subpath is ``well behaved.''
This could be done via the theoretical error bound for Simpson's rule,
@^Simpson's rule@>
but this is impractical because it requires an estimate of the fourth
derivative of the quantity being integrated.  It is much easier to just perform
a bisection step and see how much the arc length estimate changes.  Since the
error for Simpson's rule is proportional to the fourth power of the sample
spacing, the remaining error is typically about $1\over16$ of the amount of
the change.  We say ``typically'' because the error has a pseudo-random behavior
that could cause the two estimates to agree when each contain large errors.

To protect against disasters such as undetected cusps, the bisection process
should always continue until all the $dz_i$ vectors belong to a single
$90^\circ$ sector.  This ensures that no point on the spline can have velocity
less than 70\% of the minimum of $\vb dz_0\vb$, $\vb dz_1\vb$ and $\vb dz_2\vb$.
If such a spline happens to produce an erroneous arc length estimate that
is little changed by bisection, the amount of the error is likely to be fairly
small.  We will try to arrange things so that freak accidents of this type do
not destroy the inverse relationship between the \&{arclength} and
\&{arctime} operations.
@:arclength_}{\&{arclength} primitive@>
@:arctime_}{\&{arctime} primitive@>

@ The \&{arclength} and \&{arctime} operations are both based on a recursive
@^recursion@>
function that finds the arc length of a cubic spline given $dz_0$, $dz_1$,
$dz_2$. This |arc_test| routine also takes an arc length goal |a_goal| and
returns the time when the arc length reaches |a_goal| if there is such a time.
Thus the return value is either an arc length less than |a_goal| or, if the
arc length would be at least |a_goal|, it returns a time value decreased by
|two|.  This allows the caller to use the sign of the result to distinguish
between arc lengths and time values.  On certain types of overflow, it is
possible for |a_goal| and the result of |arc_test| both to be |el_gordo|.
Otherwise, the result is always less than |a_goal|.

Rather than halving the control point coordinates on each recursive call to
|arc_test|, it is better to keep them proportional to velocity on the original
curve and halve the results instead.  This means that recursive calls can
potentially use larger error tolerances in their arc length estimates.  How
much larger depends on to what extent the errors behave as though they are
independent of each other.  To save computing time, we use optimistic assumptions
and increase the tolerance by a factor of about $\sqrt2$ for each recursive
call.

In addition to the tolerance parameter, |arc_test| should also have parameters
for ${1\over3}\vb\dot B(0)\vb$, ${2\over3}\vb\dot B({1\over2})\vb$, and
${1\over3}\vb\dot B(1)\vb$.  These quantities are relatively expensive to compute
and they are needed in different instances of |arc_test|.

@c 
static scaled mp_arc_test (MP mp, scaled dx0, scaled dy0, scaled dx1, scaled dy1, 
                    scaled dx2, scaled dy2, scaled  v0, scaled v02, 
                    scaled v2, scaled a_goal, scaled tol) {
  boolean simple; /* are the control points confined to a $90^\circ$ sector? */
  scaled dx01, dy01, dx12, dy12, dx02, dy02;  /* bisection results */
  scaled v002, v022;
    /* twice the velocity magnitudes at $t={1\over4}$ and $t={3\over4}$ */
  scaled arc; /* best arc length estimate before recursion */
  @<Other local variables in |arc_test|@>;
  @<Bisect the B\'ezier quadratic given by |dx0|, |dy0|, |dx1|, |dy1|,
    |dx2|, |dy2|@>;
  @<Initialize |v002|, |v022|, and the arc length estimate |arc|; if it overflows
    set |arc_test| and |return|@>;
  @<Test if the control points are confined to one quadrant or rotating them
    $45^\circ$ would put them in one quadrant.  Then set |simple| appropriately@>;
  if ( simple && (abs(arc-v02-halfp(v0+v2)) <= tol) ) {
    if ( arc < a_goal ) {
      return arc;
    } else {
       @<Estimate when the arc length reaches |a_goal| and set |arc_test| to
         that time minus |two|@>;
    }
  } else {
    @<Use one or two recursive calls to compute the |arc_test| function@>;
  }
}

@ The |tol| value should by multiplied by $\sqrt 2$ before making recursive
calls, but $1.5$ is an adequate approximation.  It is best to avoid using
|make_fraction| in this inner loop.
@^inner loop@>

@<Use one or two recursive calls to compute the |arc_test| function@>=
{ 
  @<Set |a_new| and |a_aux| so their sum is |2*a_goal| and |a_new| is as
    large as possible@>;
  tol = tol + halfp(tol);
  a = mp_arc_test(mp, dx0,dy0, dx01,dy01, dx02,dy02, v0, v002, 
                  halfp(v02), a_new, tol);
  if ( a<0 )  {
     return (-halfp(two-a));
  } else { 
    @<Update |a_new| to reduce |a_new+a_aux| by |a|@>;
    b = mp_arc_test(mp, dx02,dy02, dx12,dy12, dx2,dy2,
                    halfp(v02), v022, v2, a_new, tol);
    if ( b<0 )  
      return (-halfp(-b) - half_unit);
    else  
      return (a + half(b-a));
  }
}

@ @<Other local variables in |arc_test|@>=
scaled a,b; /* results of recursive calls */
scaled a_new,a_aux; /* the sum of these gives the |a_goal| */

@ @<Set |a_new| and |a_aux| so their sum is |2*a_goal| and |a_new| is...@>=
a_aux = el_gordo - a_goal;
if ( a_goal > a_aux ) {
  a_aux = a_goal - a_aux;
  a_new = el_gordo;
} else { 
  a_new = a_goal + a_goal;
  a_aux = 0;
}

@ There is no need to maintain |a_aux| at this point so we use it as a temporary
to force the additions and subtractions to be done in an order that avoids
overflow.

@<Update |a_new| to reduce |a_new+a_aux| by |a|@>=
if ( a > a_aux ) {
  a_aux = a_aux - a;
  a_new = a_new + a_aux;
}

@ This code assumes all {\it dx} and {\it dy} variables have magnitude less than
|fraction_four|.  To simplify the rest of the |arc_test| routine, we strengthen
this assumption by requiring the norm of each $({\it dx},{\it dy})$ pair to obey
this bound.  Note that recursive calls will maintain this invariant.

@<Bisect the B\'ezier quadratic given by |dx0|, |dy0|, |dx1|, |dy1|,...@>=
dx01 = half(dx0 + dx1);
dx12 = half(dx1 + dx2);
dx02 = half(dx01 + dx12);
dy01 = half(dy0 + dy1);
dy12 = half(dy1 + dy2);
dy02 = half(dy01 + dy12)

@ We should be careful to keep |arc<el_gordo| so that calling |arc_test| with
|a_goal=el_gordo| is guaranteed to yield the arc length.

@<Initialize |v002|, |v022|, and the arc length estimate |arc|;...@>=
v002 = mp_pyth_add(mp, dx01+half(dx0+dx02), dy01+half(dy0+dy02));
v022 = mp_pyth_add(mp, dx12+half(dx02+dx2), dy12+half(dy02+dy2));
tmp = halfp(v02+2);
arc1 = v002 + half(halfp(v0+tmp) - v002);
arc = v022 + half(halfp(v2+tmp) - v022);
if ( (arc < el_gordo-arc1) )  {
  arc = arc+arc1;
} else { 
  mp->arith_error = true;
  if ( a_goal==el_gordo )  return (el_gordo);
  else return (-two);
}

@ @<Other local variables in |arc_test|@>=
scaled tmp, tmp2; /* all purpose temporary registers */
scaled arc1; /* arc length estimate for the first half */

@ @<Test if the control points are confined to one quadrant or rotating...@>=
simple = ((dx0>=0) && (dx1>=0) && (dx2>=0)) ||
         ((dx0<=0) && (dx1<=0) && (dx2<=0));
if ( simple )
  simple = ((dy0>=0) && (dy1>=0) && (dy2>=0)) ||
           ((dy0<=0) && (dy1<=0) && (dy2<=0));
if ( ! simple ) {
  simple = ((dx0>=dy0) && (dx1>=dy1) && (dx2>=dy2)) ||
           ((dx0<=dy0) && (dx1<=dy1) && (dx2<=dy2));
  if ( simple ) 
    simple = ((-dx0>=dy0) && (-dx1>=dy1) && (-dx2>=dy2)) ||
             ((-dx0<=dy0) && (-dx1<=dy1) && (-dx2<=dy2));
}

@ Since Simpson's rule is based on approximating the integrand by a parabola,
@^Simpson's rule@>
it is appropriate to use the same approximation to decide when the integral
reaches the intermediate value |a_goal|.  At this point
$$\eqalign{
    {\vb\dot B(0)\vb\over 3} &= \hbox{|v0|}, \qquad
    {\vb\dot B({1\over4})\vb\over 3} = {\hbox{|v002|}\over 2}, \qquad
    {\vb\dot B({1\over2})\vb\over 3} = {\hbox{|v02|}\over 2}, \cr
    {\vb\dot B({3\over4})\vb\over 3} &= {\hbox{|v022|}\over 2}, \qquad
    {\vb\dot B(1)\vb\over 3} = \hbox{|v2|} \cr
}
$$
and
$$ {\vb\dot B(t)\vb\over 3} \approx
  \cases{B\left(\hbox{|v0|},
      \hbox{|v002|}-{1\over 2}\hbox{|v0|}-{1\over 4}\hbox{|v02|},
      {1\over 2}\hbox{|v02|}; 2t \right)&
    if $t\le{1\over 2}$\cr
  B\left({1\over 2}\hbox{|v02|},
      \hbox{|v022|}-{1\over 4}\hbox{|v02|}-{1\over 2}\hbox{|v2|},
      \hbox{|v2|}; 2t-1 \right)&
    if $t\ge{1\over 2}$.\cr}
 \eqno (*)
$$
We can integrate $\vb\dot B(t)\vb$ by using
$$\int 3B(a,b,c;\tau)\,dt =
  {B(0,a,a+b,a+b+c;\tau) + {\rm constant} \over {d\tau\over dt}}.
$$

This construction allows us to find the time when the arc length reaches
|a_goal| by solving a cubic equation of the form
$$ B(0,a,a+b,a+b+c;\tau) = x, $$
where $\tau$ is $2t$ or $2t+1$, $x$ is |a_goal| or |a_goal-arc1|, and $a$, $b$,
and $c$ are the Bernshte{\u\i}n coefficients from $(*)$ divided by
@^Bernshte{\u\i}n, Serge{\u\i} Natanovich@>
$d\tau\over dt$.  We shall define a function |solve_rising_cubic| that finds
$\tau$ given $a$, $b$, $c$, and $x$.

@<Estimate when the arc length reaches |a_goal| and set |arc_test| to...@>=
{ 
  tmp = (v02 + 2) / 4;
  if ( a_goal<=arc1 ) {
    tmp2 = halfp(v0);
    return 
      (halfp(mp_solve_rising_cubic(mp, tmp2, arc1-tmp2-tmp, tmp, a_goal))- two);
  } else { 
    tmp2 = halfp(v2);
    return ((half_unit - two) +
      halfp(mp_solve_rising_cubic(mp, tmp, arc-arc1-tmp-tmp2, tmp2, a_goal-arc1)));
  }
}

@ Here is the |solve_rising_cubic| routine that finds the time~$t$ when
$$ B(0, a, a+b, a+b+c; t) = x. $$
This routine is based on |crossing_point| but is simplified by the
assumptions that $B(a,b,c;t)\ge0$ for $0\le t\le1$ and that |0<=x<=a+b+c|.
If rounding error causes this condition to be violated slightly, we just ignore
it and proceed with binary search.  This finds a time when the function value
reaches |x| and the slope is positive.

@<Declarations@>=
static scaled mp_solve_rising_cubic (MP mp,scaled a, scaled b,  scaled c, scaled x) ;

@ @c
scaled mp_solve_rising_cubic (MP mp,scaled a, scaled b,  scaled c, scaled x) {
  scaled ab, bc, ac; /* bisection results */
  integer t; /* $2^k+q$ where unscaled answer is in $[q2^{-k},(q+1)2^{-k})$ */
  integer xx; /* temporary for updating |x| */
  if ( (a<0) || (c<0) ) mp_confusion(mp, "rising?");
@:this can't happen rising?}{\quad rising?@>
  if ( x<=0 ) {
	return 0;
  } else if ( x >= a+b+c ) {
    return unity;
  } else { 
    t = 1;
    @<Rescale if necessary to make sure |a|, |b|, and |c| are all less than
      |el_gordo div 3|@>;
    do {  
      t+=t;
      @<Subdivide the B\'ezier quadratic defined by |a|, |b|, |c|@>;
      xx = x - a - ab - ac;
      if ( xx < -x ) { x+=x; b=ab; c=ac;  }
      else { x = x + xx;  a=ac; b=bc; t = t+1; };
    } while (t < unity);
    return (t - unity);
  }
}

@ @<Subdivide the B\'ezier quadratic defined by |a|, |b|, |c|@>=
ab = half(a+b);
bc = half(b+c);
ac = half(ab+bc)

@ @d one_third_el_gordo 05252525252 /* upper bound on |a|, |b|, and |c| */

@<Rescale if necessary to make sure |a|, |b|, and |c| are all less than...@>=
while ((a>one_third_el_gordo)||(b>one_third_el_gordo)||(c>one_third_el_gordo)) { 
  a = halfp(a);
  b = half(b);
  c = halfp(c);
  x = halfp(x);
}

@ It is convenient to have a simpler interface to |arc_test| that requires no
unnecessary arguments and ensures that each $({\it dx},{\it dy})$ pair has
length less than |fraction_four|.

@d arc_tol   16  /* quit when change in arc length estimate reaches this */

@c static scaled mp_do_arc_test (MP mp,scaled dx0, scaled dy0, scaled dx1, 
                          scaled dy1, scaled dx2, scaled dy2, scaled a_goal) {
  scaled v0,v1,v2; /* length of each $({\it dx},{\it dy})$ pair */
  scaled v02; /* twice the norm of the quadratic at $t={1\over2}$ */
  v0 = mp_pyth_add(mp, dx0,dy0);
  v1 = mp_pyth_add(mp, dx1,dy1);
  v2 = mp_pyth_add(mp, dx2,dy2);
  if ( (v0>=fraction_four) || (v1>=fraction_four) || (v2>=fraction_four) ) { 
    mp->arith_error = true;
    if ( a_goal==el_gordo )  return el_gordo;
    else return (-two);
  } else { 
    v02 = mp_pyth_add(mp, dx1+half(dx0+dx2), dy1+half(dy0+dy2));
    return (mp_arc_test(mp, dx0,dy0, dx1,dy1, dx2,dy2,
                                 v0, v02, v2, a_goal, arc_tol));
  }
}

@ Now it is easy to find the arc length of an entire path.

@c static scaled mp_get_arc_length (MP mp,pointer h) {
  pointer p,q; /* for traversing the path */
  scaled a,a_tot; /* current and total arc lengths */
  a_tot = 0;
  p = h;
  while ( mp_right_type(p)!=mp_endpoint ){ 
    q = mp_link(p);
    a = mp_do_arc_test(mp, mp_right_x(p)-mp_x_coord(p), mp_right_y(p)-mp_y_coord(p),
      mp_left_x(q)-mp_right_x(p), mp_left_y(q)-mp_right_y(p),
      mp_x_coord(q)-mp_left_x(q), mp_y_coord(q)-mp_left_y(q), el_gordo);
    a_tot = mp_slow_add(mp, a, a_tot);
    if ( q==h ) break;  else p=q;
  }
  check_arith;
  return a_tot;
}

@ The inverse operation of finding the time on a path~|h| when the arc length
reaches some value |arc0| can also be accomplished via |do_arc_test|.  Some care
is required to handle very large times or negative times on cyclic paths.  For
non-cyclic paths, |arc0| values that are negative or too large cause
|get_arc_time| to return 0 or the length of path~|h|.

If |arc0| is greater than the arc length of a cyclic path~|h|, the result is a
time value greater than the length of the path.  Since it could be much greater,
we must be prepared to compute the arc length of path~|h| and divide this into
|arc0| to find how many multiples of the length of path~|h| to add.

@c static scaled mp_get_arc_time (MP mp,pointer h, scaled  arc0) {
  pointer p,q; /* for traversing the path */
  scaled t_tot; /* accumulator for the result */
  scaled t; /* the result of |do_arc_test| */
  scaled arc; /* portion of |arc0| not used up so far */
  integer n; /* number of extra times to go around the cycle */
  if ( arc0<0 ) {
    @<Deal with a negative |arc0| value and |return|@>;
  }
  if ( arc0==el_gordo ) decr(arc0);
  t_tot = 0;
  arc = arc0;
  p = h;
  while ( (mp_right_type(p)!=mp_endpoint) && (arc>0) ) {
    q = mp_link(p);
    t = mp_do_arc_test(mp, mp_right_x(p)-mp_x_coord(p), mp_right_y(p)-mp_y_coord(p),
      mp_left_x(q)-mp_right_x(p), mp_left_y(q)-mp_right_y(p),
      mp_x_coord(q)-mp_left_x(q), mp_y_coord(q)-mp_left_y(q), arc);
    @<Update |arc| and |t_tot| after |do_arc_test| has just returned |t|@>;
    if ( q==h ) {
      @<Update |t_tot| and |arc| to avoid going around the cyclic
        path too many times but set |arith_error:=true| and |goto done| on
        overflow@>;
    }
    p = q;
  }
  check_arith;
  return t_tot;
}

@ @<Update |arc| and |t_tot| after |do_arc_test| has just returned |t|@>=
if ( t<0 ) { t_tot = t_tot + t + two;  arc = 0;  }
else { t_tot = t_tot + unity;  arc = arc - t;  }

@ @<Deal with a negative |arc0| value and |return|@>=
{ 
  if ( mp_left_type(h)==mp_endpoint ) {
    t_tot=0;
  } else { 
    p = mp_htap_ypoc(mp, h);
    t_tot = -mp_get_arc_time(mp, p, -arc0);
    mp_toss_knot_list(mp, p);
  }
  check_arith;
  return t_tot;
}

@ @<Update |t_tot| and |arc| to avoid going around the cyclic...@>=
if ( arc>0 ) { 
  n = arc / (arc0 - arc);
  arc = arc - n*(arc0 - arc);
  if ( t_tot > (el_gordo / (n+1)) ) { 
        mp->arith_error = true;
        check_arith;
	return el_gordo;
  }
  t_tot = (n + 1)*t_tot;
}

@* \[20] Data structures for pens.
A Pen in \MP\ can be either elliptical or polygonal.  Elliptical pens result
in \ps\ \&{stroke} commands, while anything drawn with a polygonal pen is
@:stroke}{\&{stroke} command@>
converted into an area fill as described in the next part of this program.
The mathematics behind this process is based on simple aspects of the theory
of tracings developed by Leo Guibas, Lyle Ramshaw, and Jorge Stolfi
[``A kinematic framework for computational geometry,'' Proc.\ IEEE Symp.\
Foundations of Computer Science {\bf 24} (1983), 100--111].

Polygonal pens are created from paths via \MP's \&{makepen} primitive.
@:makepen_}{\&{makepen} primitive@>
This path representation is almost sufficient for our purposes except that
a pen path should always be a convex polygon with the vertices in
counter-clockwise order.
Since we will need to scan pen polygons both forward and backward, a pen
should be represented as a doubly linked ring of knot nodes.  There is
room for the extra back pointer because we do not need the
|mp_left_type| or |mp_right_type| fields.  In fact, we don't need the |mp_left_x|,
|mp_left_y|, |mp_right_x|, or |mp_right_y| fields either but we leave these alone
so that certain procedures can operate on both pens and paths.  In particular,
pens can be copied using |copy_path| and recycled using |toss_knot_list|.

@d knil mp_info
  /* this replaces the |mp_left_type| and |mp_right_type| fields in a pen knot */

@ The |make_pen| procedure turns a path into a pen by initializing
the |knil| pointers and making sure the knots form a convex polygon.
Thus each cubic in the given path becomes a straight line and the control
points are ignored.  If the path is not cyclic, the ends are connected by a
straight line.

@d copy_pen(A) mp_make_pen(mp, mp_copy_path(mp, (A)),false)

@c 
static pointer mp_make_pen (MP mp,pointer h, boolean need_hull) {
  pointer p,q; /* two consecutive knots */
  q=h;
  do {  
    p=q; q=mp_link(q);
    knil(q)=p;
  } while (q!=h);
  if ( need_hull ){ 
    h=mp_convex_hull(mp, h);
    @<Make sure |h| isn't confused with an elliptical pen@>;
  }
  return h;
}

@ The only information required about an elliptical pen is the overall
transformation that has been applied to the original \&{pencircle}.
@:pencircle_}{\&{pencircle} primitive@>
Since it suffices to keep track of how the three points $(0,0)$, $(1,0)$,
and $(0,1)$ are transformed, an elliptical pen can be stored in a single
knot node and transformed as if it were a path.

@d pen_is_elliptical(A) ((A)==mp_link((A)))

@c 
static pointer mp_get_pen_circle (MP mp,scaled diam) {
  pointer h; /* the knot node to return */
  h=mp_get_node(mp, knot_node_size);
  mp_link(h)=h; knil(h)=h;
  mp_originator(h)=mp_program_code;
  mp_x_coord(h)=0; mp_y_coord(h)=0;
  mp_left_x(h)=diam; mp_left_y(h)=0;
  mp_right_x(h)=0; mp_right_y(h)=diam;
  return h;
}

@ If the polygon being returned by |make_pen| has only one vertex, it will
be interpreted as an elliptical pen.  This is no problem since a degenerate
polygon can equally well be thought of as a degenerate ellipse.  We need only
initialize the |mp_left_x|, |mp_left_y|, |mp_right_x|, and |mp_right_y| fields.

@<Make sure |h| isn't confused with an elliptical pen@>=
if ( pen_is_elliptical( h) ){ 
  mp_left_x(h)=mp_x_coord(h); mp_left_y(h)=mp_y_coord(h);
  mp_right_x(h)=mp_x_coord(h); mp_right_y(h)=mp_y_coord(h);
}

@ Printing a polygonal pen is very much like printing a path

@<Declarations@>=
static void mp_pr_pen (MP mp,pointer h) ;

@ @c
void mp_pr_pen (MP mp,pointer h) {
  pointer p,q; /* for list traversal */
  if ( pen_is_elliptical(h) ) {
    @<Print the elliptical pen |h|@>;
  } else { 
    p=h;
    do {  
      mp_print_two(mp, mp_x_coord(p),mp_y_coord(p));
      mp_print_nl(mp, " .. ");
      @<Advance |p| making sure the links are OK and |return| if there is
        a problem@>;
     } while (p!=h);
     mp_print(mp, "cycle");
  }
}

@ @<Advance |p| making sure the links are OK and |return| if there is...@>=
q=mp_link(p);
if ( (q==null) || (knil(q)!=p) ) { 
  mp_print_nl(mp, "???"); return; /* this won't happen */
@.???@>
}
p=q

@ @<Print the elliptical pen |h|@>=
{ 
mp_print(mp, "pencircle transformed (");
mp_print_scaled(mp, mp_x_coord(h));
mp_print_char(mp, xord(','));
mp_print_scaled(mp, mp_y_coord(h));
mp_print_char(mp, xord(','));
mp_print_scaled(mp, mp_left_x(h)-mp_x_coord(h));
mp_print_char(mp, xord(','));
mp_print_scaled(mp, mp_right_x(h)-mp_x_coord(h));
mp_print_char(mp, xord(','));
mp_print_scaled(mp, mp_left_y(h)-mp_y_coord(h));
mp_print_char(mp, xord(','));
mp_print_scaled(mp, mp_right_y(h)-mp_y_coord(h));
mp_print_char(mp, xord(')'));
}

@ Here us another version of |pr_pen| that prints the pen as a diagnostic
message.

@<Declarations@>=
static void mp_print_pen (MP mp,pointer h, const char *s, boolean nuline) ;

@ @c
void mp_print_pen (MP mp,pointer h, const char *s, boolean nuline) { 
  mp_print_diagnostic(mp, "Pen",s,nuline); mp_print_ln(mp);
@.Pen at line...@>
  mp_pr_pen(mp, h);
  mp_end_diagnostic(mp, true);
}

@ Making a polygonal pen into a path involves restoring the |mp_left_type| and
|mp_right_type| fields and setting the control points so as to make a polygonal
path.

@c 
static void mp_make_path (MP mp,pointer h) {
  pointer p; /* for traversing the knot list */
  quarterword k; /* a loop counter */
  @<Other local variables in |make_path|@>;
  if ( pen_is_elliptical(h) ) {
    @<Make the elliptical pen |h| into a path@>;
  } else { 
    p=h;
    do {  
      mp_left_type(p)=mp_explicit;
      mp_right_type(p)=mp_explicit;
      @<copy the coordinates of knot |p| into its control points@>;
       p=mp_link(p);
    } while (p!=h);
  }
}

@ @<copy the coordinates of knot |p| into its control points@>=
mp_left_x(p)=mp_x_coord(p);
mp_left_y(p)=mp_y_coord(p);
mp_right_x(p)=mp_x_coord(p);
mp_right_y(p)=mp_y_coord(p)

@ We need an eight knot path to get a good approximation to an ellipse.

@<Make the elliptical pen |h| into a path@>=
{ 
  @<Extract the transformation parameters from the elliptical pen~|h|@>;
  p=h;
  for (k=0;k<=7;k++ ) { 
    @<Initialize |p| as the |k|th knot of a circle of unit diameter,
      transforming it appropriately@>;
    if ( k==7 ) mp_link(p)=h;  else mp_link(p)=mp_get_node(mp, knot_node_size);
    p=mp_link(p);
  }
}

@ @<Extract the transformation parameters from the elliptical pen~|h|@>=
center_x=mp_x_coord(h);
center_y=mp_y_coord(h);
width_x=mp_left_x(h)-center_x;
width_y=mp_left_y(h)-center_y;
height_x=mp_right_x(h)-center_x;
height_y=mp_right_y(h)-center_y

@ @<Other local variables in |make_path|@>=
scaled center_x,center_y; /* translation parameters for an elliptical pen */
scaled width_x,width_y; /* the effect of a unit change in $x$ */
scaled height_x,height_y; /* the effect of a unit change in $y$ */
scaled dx,dy; /* the vector from knot |p| to its right control point */
integer kk;
  /* |k| advanced $270^\circ$ around the ring (cf. $\sin\theta=\cos(\theta+270)$) */

@ The only tricky thing here are the tables |half_cos| and |d_cos| used to
find the point $k/8$ of the way around the circle and the direction vector
to use there.

@<Initialize |p| as the |k|th knot of a circle of unit diameter,...@>=
kk=(k+6)% 8;
mp_x_coord(p)=center_x+mp_take_fraction(mp, mp->half_cos[k],width_x)
           +mp_take_fraction(mp, mp->half_cos[kk],height_x);
mp_y_coord(p)=center_y+mp_take_fraction(mp, mp->half_cos[k],width_y)
           +mp_take_fraction(mp, mp->half_cos[kk],height_y);
dx=-mp_take_fraction(mp, mp->d_cos[kk],width_x)
   +mp_take_fraction(mp, mp->d_cos[k],height_x);
dy=-mp_take_fraction(mp, mp->d_cos[kk],width_y)
   +mp_take_fraction(mp, mp->d_cos[k],height_y);
mp_right_x(p)=mp_x_coord(p)+dx;
mp_right_y(p)=mp_y_coord(p)+dy;
mp_left_x(p)=mp_x_coord(p)-dx;
mp_left_y(p)=mp_y_coord(p)-dy;
mp_left_type(p)=mp_explicit;
mp_right_type(p)=mp_explicit;
mp_originator(p)=mp_program_code

@ @<Glob...@>=
fraction half_cos[8]; /* ${1\over2}\cos(45k)$ */
fraction d_cos[8]; /* a magic constant times $\cos(45k)$ */

@ The magic constant for |d_cos| is the distance between $({1\over2},0)$ and
$({1\over4}\sqrt2,{1\over4}\sqrt2)$ times the result of the |velocity|
function for $\theta=\phi=22.5^\circ$.  This comes out to be
$$ d = {\sqrt{2-\sqrt2}\over 3+3\cos22.5^\circ}
  \approx 0.132608244919772.
$$

@<Set init...@>=
mp->half_cos[0]=fraction_half;
mp->half_cos[1]=94906266; /* $2^{26}\sqrt2\approx94906265.62$ */
mp->half_cos[2]=0;
mp->d_cos[0]=35596755; /* $2^{28}d\approx35596754.69$ */
mp->d_cos[1]=25170707; /* $2^{27}\sqrt2\,d\approx25170706.63$ */
mp->d_cos[2]=0;
for (k=3;k<= 4;k++ ) { 
  mp->half_cos[k]=-mp->half_cos[4-k];
  mp->d_cos[k]=-mp->d_cos[4-k];
}
for (k=5;k<= 7;k++ ) { 
  mp->half_cos[k]=mp->half_cos[8-k];
  mp->d_cos[k]=mp->d_cos[8-k];
}

@ The |convex_hull| function forces a pen polygon to be convex when it is
returned by |make_pen| and after any subsequent transformation where rounding
error might allow the convexity to be lost.
The convex hull algorithm used here is described by F.~P. Preparata and
M.~I. Shamos [{\sl Computational Geometry}, Springer-Verlag, 1985].

@<Declarations@>=
static pointer mp_convex_hull (MP mp,pointer h);

@ @c
pointer mp_convex_hull (MP mp,pointer h) { /* Make a polygonal pen convex */
  pointer l,r; /* the leftmost and rightmost knots */
  pointer p,q; /* knots being scanned */
  pointer s; /* the starting point for an upcoming scan */
  scaled dx,dy; /* a temporary pointer */
  if ( pen_is_elliptical(h) ) {
     return h;
  } else { 
    @<Set |l| to the leftmost knot in polygon~|h|@>;
    @<Set |r| to the rightmost knot in polygon~|h|@>;
    if ( l!=r ) { 
      s=mp_link(r);
      @<Find any knots on the path from |l| to |r| above the |l|-|r| line and
        move them past~|r|@>;
      @<Find any knots on the path from |s| to |l| below the |l|-|r| line and
        move them past~|l|@>;
      @<Sort the path from |l| to |r| by increasing $x$@>;
      @<Sort the path from |r| to |l| by decreasing $x$@>;
    }
    if ( l!=mp_link(l) ) {
      @<Do a Gramm scan and remove vertices where there is no left turn@>;
    }
    return l;
  }
}

@ All comparisons are done primarily on $x$ and secondarily on $y$.

@<Set |l| to the leftmost knot in polygon~|h|@>=
l=h;
p=mp_link(h);
while ( p!=h ) { 
  if ( mp_x_coord(p)<=mp_x_coord(l) )
    if ( (mp_x_coord(p)<mp_x_coord(l)) || (mp_y_coord(p)<mp_y_coord(l)) )
      l=p;
  p=mp_link(p);
}

@ @<Set |r| to the rightmost knot in polygon~|h|@>=
r=h;
p=mp_link(h);
while ( p!=h ) { 
  if ( mp_x_coord(p)>=mp_x_coord(r) )
    if ( (mp_x_coord(p)>mp_x_coord(r)) || (mp_y_coord(p)>mp_y_coord(r)) )
      r=p;
  p=mp_link(p);
}

@ @<Find any knots on the path from |l| to |r| above the |l|-|r| line...@>=
dx=mp_x_coord(r)-mp_x_coord(l);
dy=mp_y_coord(r)-mp_y_coord(l);
p=mp_link(l);
while ( p!=r ) { 
  q=mp_link(p);
  if ( mp_ab_vs_cd(mp, dx,mp_y_coord(p)-mp_y_coord(l),dy,mp_x_coord(p)-mp_x_coord(l))>0 )
    mp_move_knot(mp, p, r);
  p=q;
}

@ The |move_knot| procedure removes |p| from a doubly linked list and inserts
it after |q|.

@ @<Declarations@>=
static void mp_move_knot (MP mp,pointer p, pointer q) ;

@ @c
void mp_move_knot (MP mp,pointer p, pointer q) { 
  mp_link(knil(p))=mp_link(p);
  knil(mp_link(p))=knil(p);
  knil(p)=q;
  mp_link(p)=mp_link(q);
  mp_link(q)=p;
  knil(mp_link(p))=p;
}

@ @<Find any knots on the path from |s| to |l| below the |l|-|r| line...@>=
p=s;
while ( p!=l ) { 
  q=mp_link(p);
  if ( mp_ab_vs_cd(mp, dx,mp_y_coord(p)-mp_y_coord(l),dy,mp_x_coord(p)-mp_x_coord(l))<0 )
    mp_move_knot(mp, p,l);
  p=q;
}

@ The list is likely to be in order already so we just do linear insertions.
Secondary comparisons on $y$ ensure that the sort is consistent with the
choice of |l| and |r|.

@<Sort the path from |l| to |r| by increasing $x$@>=
p=mp_link(l);
while ( p!=r ) { 
  q=knil(p);
  while ( mp_x_coord(q)>mp_x_coord(p) ) q=knil(q);
  while ( mp_x_coord(q)==mp_x_coord(p) ) {
    if ( mp_y_coord(q)>mp_y_coord(p) ) q=knil(q); else break;
  }
  if ( q==knil(p) ) p=mp_link(p);
  else { p=mp_link(p); mp_move_knot(mp, knil(p),q); };
}

@ @<Sort the path from |r| to |l| by decreasing $x$@>=
p=mp_link(r);
while ( p!=l ){ 
  q=knil(p);
  while ( mp_x_coord(q)<mp_x_coord(p) ) q=knil(q);
  while ( mp_x_coord(q)==mp_x_coord(p) ) {
    if ( mp_y_coord(q)<mp_y_coord(p) ) q=knil(q); else break;
  }
  if ( q==knil(p) ) p=mp_link(p);
  else { p=mp_link(p); mp_move_knot(mp, knil(p),q); };
}

@ The condition involving |ab_vs_cd| tests if there is not a left turn
at knot |q|.  There usually will be a left turn so we streamline the case
where the |then| clause is not executed.

@<Do a Gramm scan and remove vertices where there...@>=
{ 
p=l; q=mp_link(l);
while (1) { 
  dx=mp_x_coord(q)-mp_x_coord(p);
  dy=mp_y_coord(q)-mp_y_coord(p);
  p=q; q=mp_link(q);
  if ( p==l ) break;
  if ( p!=r )
    if ( mp_ab_vs_cd(mp, dx,mp_y_coord(q)-mp_y_coord(p),dy,mp_x_coord(q)-mp_x_coord(p))<=0 ) {
      @<Remove knot |p| and back up |p| and |q| but don't go past |l|@>;
    }
  }
}

@ @<Remove knot |p| and back up |p| and |q| but don't go past |l|@>=
{ 
s=knil(p);
mp_free_node(mp, p,knot_node_size);
mp_link(s)=q; knil(q)=s;
if ( s==l ) p=s;
else { p=knil(s); q=s; };
}

@ The |find_offset| procedure sets global variables |(cur_x,cur_y)| to the
offset associated with the given direction |(x,y)|.  If two different offsets
apply, it chooses one of them.

@c 
static void mp_find_offset (MP mp,scaled x, scaled y, pointer h) {
  pointer p,q; /* consecutive knots */
  scaled wx,wy,hx,hy;
  /* the transformation matrix for an elliptical pen */
  fraction xx,yy; /* untransformed offset for an elliptical pen */
  fraction d; /* a temporary register */
  if ( pen_is_elliptical(h) ) {
    @<Find the offset for |(x,y)| on the elliptical pen~|h|@>
  } else { 
    q=h;
    do {  
      p=q; q=mp_link(q);
    } while (!(mp_ab_vs_cd(mp, mp_x_coord(q)-mp_x_coord(p),y, mp_y_coord(q)-mp_y_coord(p),x)>=0));
    do {  
      p=q; q=mp_link(q);
    } while (!(mp_ab_vs_cd(mp, mp_x_coord(q)-mp_x_coord(p),y, mp_y_coord(q)-mp_y_coord(p),x)<=0));
    mp->cur_x=mp_x_coord(p);
    mp->cur_y=mp_y_coord(p);
  }
}

@ @<Glob...@>=
scaled cur_x;
scaled cur_y; /* all-purpose return value registers */

@ @<Find the offset for |(x,y)| on the elliptical pen~|h|@>=
if ( (x==0) && (y==0) ) {
  mp->cur_x=mp_x_coord(h); mp->cur_y=mp_y_coord(h);  
} else { 
  @<Find the non-constant part of the transformation for |h|@>;
  while ( (abs(x)<fraction_half) && (abs(y)<fraction_half) ){ 
    x+=x; y+=y;  
  };
  @<Make |(xx,yy)| the offset on the untransformed \&{pencircle} for the
    untransformed version of |(x,y)|@>;
  mp->cur_x=mp_x_coord(h)+mp_take_fraction(mp, xx,wx)+mp_take_fraction(mp, yy,hx);
  mp->cur_y=mp_y_coord(h)+mp_take_fraction(mp, xx,wy)+mp_take_fraction(mp, yy,hy);
}

@ @<Find the non-constant part of the transformation for |h|@>=
wx=mp_left_x(h)-mp_x_coord(h);
wy=mp_left_y(h)-mp_y_coord(h);
hx=mp_right_x(h)-mp_x_coord(h);
hy=mp_right_y(h)-mp_y_coord(h)

@ @<Make |(xx,yy)| the offset on the untransformed \&{pencircle} for the...@>=
yy=-(mp_take_fraction(mp, x,hy)+mp_take_fraction(mp, y,-hx));
xx=mp_take_fraction(mp, x,-wy)+mp_take_fraction(mp, y,wx);
d=mp_pyth_add(mp, xx,yy);
if ( d>0 ) { 
  xx=half(mp_make_fraction(mp, xx,d));
  yy=half(mp_make_fraction(mp, yy,d));
}

@ Finding the bounding box of a pen is easy except if the pen is elliptical.
But we can handle that case by just calling |find_offset| twice.  The answer
is stored in the global variables |minx|, |maxx|, |miny|, and |maxy|.

@c 
static void mp_pen_bbox (MP mp,pointer h) {
  pointer p; /* for scanning the knot list */
  if ( pen_is_elliptical(h) ) {
    @<Find the bounding box of an elliptical pen@>;
  } else { 
    mp_minx=mp_x_coord(h); mp_maxx=mp_minx;
    mp_miny=mp_y_coord(h); mp_maxy=mp_miny;
    p=mp_link(h);
    while ( p!=h ) {
      if ( mp_x_coord(p)<mp_minx ) mp_minx=mp_x_coord(p);
      if ( mp_y_coord(p)<mp_miny ) mp_miny=mp_y_coord(p);
      if ( mp_x_coord(p)>mp_maxx ) mp_maxx=mp_x_coord(p);
      if ( mp_y_coord(p)>mp_maxy ) mp_maxy=mp_y_coord(p);
      p=mp_link(p);
    }
  }
}

@ @<Find the bounding box of an elliptical pen@>=
{ 
mp_find_offset(mp, 0,fraction_one,h);
mp_maxx=mp->cur_x;
mp_minx=2*mp_x_coord(h)-mp->cur_x;
mp_find_offset(mp, -fraction_one,0,h);
mp_maxy=mp->cur_y;
mp_miny=2*mp_y_coord(h)-mp->cur_y;
}

@* \[21] Edge structures.
Now we come to \MP's internal scheme for representing pictures.
The representation is very different from \MF's edge structures
because \MP\ pictures contain \ps\ graphics objects instead of pixel
images.  However, the basic idea is somewhat similar in that shapes
are represented via their boundaries.

The main purpose of edge structures is to keep track of graphical objects
until it is time to translate them into \ps.  Since \MP\ does not need to
know anything about an edge structure other than how to translate it into
\ps\ and how to find its bounding box, edge structures can be just linked
lists of graphical objects.  \MP\ has no easy way to determine whether
two such objects overlap, but it suffices to draw the first one first and
let the second one overwrite it if necessary.

@(mplib.h@>=
enum mp_graphical_object_code {
  @<Graphical object codes@>
  mp_final_graphic
};

@ Let's consider the types of graphical objects one at a time.
First of all, a filled contour is represented by a eight-word node.  The first
word contains |type| and |link| fields, and the next six words contain a
pointer to a cyclic path and the value to use for \ps' \&{currentrgbcolor}
parameter.  If a pen is used for filling |pen_p|, |ljoin_val| and |miterlim_val|
give the relevant information.

@d mp_path_p(A) mp_link((A)+1)
  /* a pointer to the path that needs filling */
@d mp_pen_p(A) mp_info((A)+1)
  /* a pointer to the pen to fill or stroke with */
@d mp_color_model(A) mp_type((A)+2) /*  the color model  */
@d obj_red_loc(A) ((A)+3)  /* the first of three locations for the color */
@d obj_cyan_loc obj_red_loc  /* the first of four locations for the color */
@d obj_grey_loc obj_red_loc  /* the location for the color */
@d red_val(A) mp->mem[(A)+3].sc
  /* the red component of the color in the range $0\ldots1$ */
@d cyan_val red_val
@d grey_val red_val
@d green_val(A) mp->mem[(A)+4].sc
  /* the green component of the color in the range $0\ldots1$ */
@d magenta_val green_val
@d blue_val(A) mp->mem[(A)+5].sc
  /* the blue component of the color in the range $0\ldots1$ */
@d yellow_val blue_val
@d black_val(A) mp->mem[(A)+6].sc
  /* the blue component of the color in the range $0\ldots1$ */
@d ljoin_val(A) mp_name_type((A))  /* the value of \&{linejoin} */
@:mp_linejoin_}{\&{linejoin} primitive@>
@d miterlim_val(A) mp->mem[(A)+7].sc  /* the value of \&{miterlimit} */
@:mp_miterlimit_}{\&{miterlimit} primitive@>
@d obj_color_part(A) mp->mem[(A)+3-red_part].sc
  /* interpret an object pointer that has been offset by |red_part..blue_part| */
@d mp_pre_script(A) mp->mem[(A)+8].hh.lh
@d mp_post_script(A) mp->mem[(A)+8].hh.rh
@d fill_node_size 9

@ @<Graphical object codes@>=
mp_fill_code=1,

@ @c 
static pointer mp_new_fill_node (MP mp,pointer p) {
  /* make a fill node for cyclic path |p| and color black */
  pointer t; /* the new node */
  t=mp_get_node(mp, fill_node_size);
  mp_type(t)=mp_fill_code;
  mp_path_p(t)=p;
  mp_pen_p(t)=null; /* |null| means don't use a pen */
  red_val(t)=0;
  green_val(t)=0;
  blue_val(t)=0;
  black_val(t)=0;
  mp_color_model(t)=mp_uninitialized_model;
  mp_pre_script(t)=null;
  mp_post_script(t)=null;
  @<Set the |ljoin_val| and |miterlim_val| fields in object |t|@>;
  return t;
}

@ @<Set the |ljoin_val| and |miterlim_val| fields in object |t|@>=
if ( mp->internal[mp_linejoin]>unity ) ljoin_val(t)=2;
else if ( mp->internal[mp_linejoin]>0 ) ljoin_val(t)=1;
else ljoin_val(t)=0;
if ( mp->internal[mp_miterlimit]<unity )
  miterlim_val(t)=unity;
else
  miterlim_val(t)=mp->internal[mp_miterlimit]

@ A stroked path is represented by an eight-word node that is like a filled
contour node except that it contains the current \&{linecap} value, a scale
factor for the dash pattern, and a pointer that is non-null if the stroke
is to be dashed.  The purpose of the scale factor is to allow a picture to
be transformed without touching the picture that |dash_p| points to.

@d mp_dash_p(A) mp_link((A)+9)
  /* a pointer to the edge structure that gives the dash pattern */
@d lcap_val(A) mp_type((A)+9)
  /* the value of \&{linecap} */
@:mp_linecap_}{\&{linecap} primitive@>
@d dash_scale(A) mp->mem[(A)+10].sc /* dash lengths are scaled by this factor */
@d stroked_node_size 11

@ @<Graphical object codes@>=
mp_stroked_code=2,

@ @c 
static pointer mp_new_stroked_node (MP mp,pointer p) {
  /* make a stroked node for path |p| with |mp_pen_p(p)| temporarily |null| */
  pointer t; /* the new node */
  t=mp_get_node(mp, stroked_node_size);
  mp_type(t)=mp_stroked_code;
  mp_path_p(t)=p; mp_pen_p(t)=null;
  mp_dash_p(t)=null;
  dash_scale(t)=unity;
  red_val(t)=0;
  green_val(t)=0;
  blue_val(t)=0;
  black_val(t)=0;
  mp_color_model(t)=mp_uninitialized_model;
  mp_pre_script(t)=null;
  mp_post_script(t)=null;
  @<Set the |ljoin_val| and |miterlim_val| fields in object |t|@>;
  if ( mp->internal[mp_linecap]>unity ) lcap_val(t)=2;
  else if ( mp->internal[mp_linecap]>0 ) lcap_val(t)=1;
  else lcap_val(t)=0;
  return t;
}

@ When a dashed line is computed in a transformed coordinate system, the dash
lengths get scaled like the pen shape and we need to compensate for this.  Since
there is no unique scale factor for an arbitrary transformation, we use the
the square root of the determinant.  The properties of the determinant make it
easier to maintain the |dash_scale|.  The computation is fairly straight-forward
except for the initialization of the scale factor |s|.  The factor of 64 is
needed because |square_rt| scales its result by $2^8$ while we need $2^{14}$
to counteract the effect of |take_fraction|.

@ @c
scaled mp_sqrt_det (MP mp,scaled a, scaled b, scaled c, scaled d) {
  scaled maxabs; /* $max(|a|,|b|,|c|,|d|)$ */
  unsigned s; /* amount by which the result of |square_rt| needs to be scaled */
  @<Initialize |maxabs|@>;
  s=64;
  while ( (maxabs<fraction_one) && (s>1) ){ 
    a+=a; b+=b; c+=c; d+=d;
    maxabs+=maxabs; s=(unsigned)(halfp(s));
  }
  return (scaled)(s*(unsigned)mp_square_rt(mp, abs(mp_take_fraction(mp, a,d)-mp_take_fraction(mp, b,c))));
}
@#
static scaled mp_get_pen_scale (MP mp,pointer p) { 
  return mp_sqrt_det(mp, 
    mp_left_x(p)-mp_x_coord(p), mp_right_x(p)-mp_x_coord(p),
    mp_left_y(p)-mp_y_coord(p), mp_right_y(p)-mp_y_coord(p));
}

@ @<Declarations@>=
static scaled mp_sqrt_det (MP mp,scaled a, scaled b, scaled c, scaled d) ;


@ @<Initialize |maxabs|@>=
maxabs=abs(a);
if ( abs(b)>maxabs ) maxabs=abs(b);
if ( abs(c)>maxabs ) maxabs=abs(c);
if ( abs(d)>maxabs ) maxabs=abs(d)

@ When a picture contains text, this is represented by a fourteen-word node
where the color information and |type| and |link| fields are augmented by
additional fields that describe the text and  how it is transformed.
The |path_p| and |mp_pen_p| pointers are replaced by a number that identifies
the font and a string number that gives the text to be displayed.
The |width|, |height|, and |depth| fields
give the dimensions of the text at its design size, and the remaining six
words give a transformation to be applied to the text.  The |new_text_node|
function initializes everything to default values so that the text comes out
black with its reference point at the origin.

@d mp_text_p(A) mp_link((A)+1)  /* a string pointer for the text to display */
@d mp_font_n(A) mp_info((A)+1)  /* the font number */
@d width_val(A) mp->mem[(A)+7].sc  /* unscaled width of the text */
@d height_val(A) mp->mem[(A)+9].sc  /* unscaled height of the text */
@d depth_val(A) mp->mem[(A)+10].sc  /* unscaled depth of the text */
@d text_tx_loc(A) ((A)+11)
  /* the first of six locations for transformation parameters */
@d tx_val(A) mp->mem[(A)+11].sc  /* $x$ shift amount */
@d ty_val(A) mp->mem[(A)+12].sc  /* $y$ shift amount */
@d txx_val(A) mp->mem[(A)+13].sc  /* |txx| transformation parameter */
@d txy_val(A) mp->mem[(A)+14].sc  /* |txy| transformation parameter */
@d tyx_val(A) mp->mem[(A)+15].sc  /* |tyx| transformation parameter */
@d tyy_val(A) mp->mem[(A)+16].sc  /* |tyy| transformation parameter */
@d text_trans_part(A) mp->mem[(A)+11-x_part].sc
    /* interpret a text node pointer that has been offset by |x_part..yy_part| */
@d text_node_size 17

@ @<Graphical object codes@>=
mp_text_code=3,

@ @c
static pointer mp_new_text_node (MP mp,char *f,str_number s) {
  /* make a text node for font |f| and text string |s| */
  pointer t; /* the new node */
  t=mp_get_node(mp, text_node_size);
  mp_type(t)=mp_text_code;
  mp_text_p(t)=s;
  mp_font_n(t)=(halfword)mp_find_font(mp, f); /* this identifies the font */
  red_val(t)=0;
  green_val(t)=0;
  blue_val(t)=0;
  black_val(t)=0;
  mp_color_model(t)=mp_uninitialized_model;
  mp_pre_script(t)=null;
  mp_post_script(t)=null;
  tx_val(t)=0; ty_val(t)=0;
  txx_val(t)=unity; txy_val(t)=0;
  tyx_val(t)=0; tyy_val(t)=unity;
  mp_set_text_box(mp, t); /* this finds the bounding box */
  return t;
}

@ The last two types of graphical objects that can occur in an edge structure
are clipping paths and \&{setbounds} paths.  These are slightly more difficult
@:set_bounds_}{\&{setbounds} primitive@>
to implement because we must keep track of exactly what is being clipped or
bounded when pictures get merged together.  For this reason, each clipping or
\&{setbounds} operation is represented by a pair of nodes:  first comes a
two-word node whose |path_p| gives the relevant path, then there is the list
of objects to clip or bound followed by a two-word node whose second word is
unused.

Using at least two words for each graphical object node allows them all to be
allocated and deallocated similarly with a global array |gr_object_size| to
give the size in words for each object type.

@d start_clip_size 2
@d start_bounds_size 2
@d stop_clip_size 2 /* the second word is not used here */
@d stop_bounds_size 2 /* the second word is not used here */
@#
@d stop_type(A) ((A)+2)
  /* matching |type| for |start_clip_code| or |start_bounds_code| */
@d has_color(A) (mp_type((A))<mp_start_clip_code)
  /* does a graphical object have color fields? */
@d has_pen(A) (mp_type((A))<mp_text_code)
  /* does a graphical object have a |mp_pen_p| field? */
@d is_start_or_stop(A) (mp_type((A))>=mp_start_clip_code)
@d is_stop(A) (mp_type((A))>=mp_stop_clip_code)

@ @<Graphical object codes@>=
mp_start_clip_code=4, /* |type| of a node that starts clipping */
mp_start_bounds_code=5, /* |type| of a node that gives a \&{setbounds} path */
mp_stop_clip_code=6, /* |type| of a node that stops clipping */
mp_stop_bounds_code=7, /* |type| of a node that stops \&{setbounds} */

@ @c 
static pointer mp_new_bounds_node (MP mp,pointer p, quarterword  c) {
  /* make a node of type |c| where |p| is the clipping or \&{setbounds} path */
  pointer t; /* the new node */
  t=mp_get_node(mp, mp->gr_object_size[c]);
  mp_type(t)=c;
  mp_path_p(t)=p;
  return t;
}

@ We need an array to keep track of the sizes of graphical objects.

@<Glob...@>=
quarterword gr_object_size[mp_stop_bounds_code+1];

@ @<Set init...@>=
mp->gr_object_size[mp_fill_code]=fill_node_size;
mp->gr_object_size[mp_stroked_code]=stroked_node_size;
mp->gr_object_size[mp_text_code]=text_node_size;
mp->gr_object_size[mp_start_clip_code]=start_clip_size;
mp->gr_object_size[mp_stop_clip_code]=stop_clip_size;
mp->gr_object_size[mp_start_bounds_code]=start_bounds_size;
mp->gr_object_size[mp_stop_bounds_code]=stop_bounds_size;

@ All the essential information in an edge structure is encoded as a linked list
of graphical objects as we have just seen, but it is helpful to add some
redundant information.  A single edge structure might be used as a dash pattern
many times, and it would be nice to avoid scanning the same structure
repeatedly.  Thus, an edge structure known to be a suitable dash pattern
has a header that gives a list of dashes in a sorted order designed for rapid
translation into \ps.

Each dash is represented by a three-word node containing the initial and final
$x$~coordinates as well as the usual |link| field.  The |link| fields points to
the dash node with the next higher $x$-coordinates and the final link points
to a special location called |null_dash|.  (There should be no overlap between
dashes).  Since the $y$~coordinate of the dash pattern is needed to determine
the period of repetition, this needs to be stored in the edge header along
with a pointer to the list of dash nodes.

@d start_x(A) mp->mem[(A)+1].sc  /* the starting $x$~coordinate in a dash node */
@d stop_x(A) mp->mem[(A)+2].sc  /* the ending $x$~coordinate in a dash node */
@d dash_node_size 3
@d dash_list mp_link
  /* in an edge header this points to the first dash node */
@d dash_y(A) mp->mem[(A)+1].sc  /* $y$ value for the dash list in an edge header */

@ It is also convenient for an edge header to contain the bounding
box information needed by the \&{llcorner} and \&{urcorner} operators
so that this does not have to be recomputed unnecessarily.  This is done by
adding fields for the $x$~and $y$ extremes as well as a pointer that indicates
how far the bounding box computation has gotten.  Thus if the user asks for
the bounding box and then adds some more text to the picture before asking
for more bounding box information, the second computation need only look at
the additional text.

When the bounding box has not been computed, the |bblast| pointer points
to a dummy link at the head of the graphical object list while the |minx_val|
and |miny_val| fields contain |el_gordo| and the |maxx_val| and |maxy_val|
fields contain |-el_gordo|.

Since the bounding box of pictures containing objects of type
|mp_start_bounds_code| depends on the value of \&{truecorners}, the bounding box
@:mp_true_corners_}{\&{truecorners} primitive@>
data might not be valid for all values of this parameter.  Hence, the |bbtype|
field is needed to keep track of this.

@d minx_val(A) mp->mem[(A)+2].sc
@d miny_val(A) mp->mem[(A)+3].sc
@d maxx_val(A) mp->mem[(A)+4].sc
@d maxy_val(A) mp->mem[(A)+5].sc
@d bblast(A) mp_link((A)+6)  /* last item considered in bounding box computation */
@d bbtype(A) mp_info((A)+6)  /* tells how bounding box data depends on \&{truecorners} */
@d dummy_loc(A) ((A)+7)  /* where the object list begins in an edge header */
@d no_bounds 0
  /* |bbtype| value when bounding box data is valid for all \&{truecorners} values */
@d bounds_set 1
  /* |bbtype| value when bounding box data is for \&{truecorners}${}\le 0$ */
@d bounds_unset 2
  /* |bbtype| value when bounding box data is for \&{truecorners}${}>0$ */

@c 
static void mp_init_bbox (MP mp,pointer h) {
  /* Initialize the bounding box information in edge structure |h| */
  bblast(h)=dummy_loc(h);
  bbtype(h)=no_bounds;
  minx_val(h)=el_gordo;
  miny_val(h)=el_gordo;
  maxx_val(h)=-el_gordo;
  maxy_val(h)=-el_gordo;
}

@ The only other entries in an edge header are a reference count in the first
word and a pointer to the tail of the object list in the last word.

@d obj_tail(A) mp_info((A)+7)  /* points to the last entry in the object list */
@d edge_header_size 8

@c 
static void mp_init_edges (MP mp,pointer h) {
  /* initialize an edge header to null values */
  dash_list(h)=null_dash;
  obj_tail(h)=dummy_loc(h);
  mp_link(dummy_loc(h))=null;
  ref_count(h)=null;
  mp_init_bbox(mp, h);
}

@ Here is how edge structures are deleted.  The process can be recursive because
of the need to dereference edge structures that are used as dash patterns.
@^recursion@>

@d add_edge_ref(A) incr(ref_count(A))
@d delete_edge_ref(A) { 
   if ( ref_count((A))==null ) 
     mp_toss_edges(mp, A);
   else 
     decr(ref_count(A)); 
   }

@<Declarations@>=
static void mp_flush_dash_list (MP mp,pointer h);
static pointer mp_toss_gr_object (MP mp,pointer p) ;
static void mp_toss_edges (MP mp,pointer h) ;

@ @c void mp_toss_edges (MP mp,pointer h) {
  pointer p,q;  /* pointers that scan the list being recycled */
  pointer r; /* an edge structure that object |p| refers to */
  mp_flush_dash_list(mp, h);
  q=mp_link(dummy_loc(h));
  while ( (q!=null) ) { 
    p=q; q=mp_link(q);
    r=mp_toss_gr_object(mp, p);
    if ( r!=null ) delete_edge_ref(r);
  }
  mp_free_node(mp, h,edge_header_size);
}
void mp_flush_dash_list (MP mp,pointer h) {
  pointer p,q;  /* pointers that scan the list being recycled */
  q=dash_list(h);
  while ( q!=null_dash ) { 
    p=q; q=mp_link(q);
    mp_free_node(mp, p,dash_node_size);
  }
  dash_list(h)=null_dash;
}
pointer mp_toss_gr_object (MP mp,pointer p) {
  /* returns an edge structure that needs to be dereferenced */
  pointer e; /* the edge structure to return */
  e=null;
  @<Prepare to recycle graphical object |p|@>;
  mp_free_node(mp, p,mp->gr_object_size[mp_type(p)]);
  return e;
}

@ @<Prepare to recycle graphical object |p|@>=
switch (mp_type(p)) {
case mp_fill_code: 
  mp_toss_knot_list(mp, mp_path_p(p));
  if ( mp_pen_p(p)!=null ) mp_toss_knot_list(mp, mp_pen_p(p));
  if ( mp_pre_script(p)!=null ) delete_str_ref(mp_pre_script(p));
  if ( mp_post_script(p)!=null ) delete_str_ref(mp_post_script(p));
  break;
case mp_stroked_code: 
  mp_toss_knot_list(mp, mp_path_p(p));
  if ( mp_pen_p(p)!=null ) mp_toss_knot_list(mp, mp_pen_p(p));
  if ( mp_pre_script(p)!=null ) delete_str_ref(mp_pre_script(p));
  if ( mp_post_script(p)!=null ) delete_str_ref(mp_post_script(p));
  e=mp_dash_p(p);
  break;
case mp_text_code: 
  delete_str_ref(mp_text_p(p));
  if ( mp_pre_script(p)!=null ) delete_str_ref(mp_pre_script(p));
  if ( mp_post_script(p)!=null ) delete_str_ref(mp_post_script(p));
  break;
case mp_start_clip_code:
case mp_start_bounds_code: 
  mp_toss_knot_list(mp, mp_path_p(p));
  break;
case mp_stop_clip_code:
case mp_stop_bounds_code: 
  break;
} /* there are no other cases */

@ If we use |add_edge_ref| to ``copy'' edge structures, the real copying needs
to be done before making a significant change to an edge structure.  Much of
the work is done in a separate routine |copy_objects| that copies a list of
graphical objects into a new edge header.

@c
static pointer mp_private_edges (MP mp,pointer h) {
  /* make a private copy of the edge structure headed by |h| */
  pointer hh;  /* the edge header for the new copy */
  pointer p,pp;  /* pointers for copying the dash list */
  if ( ref_count(h)==null ) {
    return h;
  } else { 
    decr(ref_count(h));
    hh=mp_copy_objects(mp, mp_link(dummy_loc(h)),null);
    @<Copy the dash list from |h| to |hh|@>;
    @<Copy the bounding box information from |h| to |hh| and make |bblast(hh)|
      point into the new object list@>;
    return hh;
  }
}

@ Here we use the fact that |dash_list(hh)=mp_link(hh)|.
@^data structure assumptions@>

@<Copy the dash list from |h| to |hh|@>=
pp=hh; p=dash_list(h);
while ( (p!=null_dash) ) { 
  mp_link(pp)=mp_get_node(mp, dash_node_size);
  pp=mp_link(pp);
  start_x(pp)=start_x(p);
  stop_x(pp)=stop_x(p);
  p=mp_link(p);
}
mp_link(pp)=null_dash;
dash_y(hh)=dash_y(h)


@ |h| is an edge structure

@c
static mp_dash_object *mp_export_dashes (MP mp, pointer q, scaled *w) {
  mp_dash_object *d;
  pointer p, h;
  scaled scf; /* scale factor */
  int *dashes = NULL;
  int num_dashes = 1;
  h = mp_dash_p(q);
  if (h==null ||  dash_list(h)==null_dash) 
	return NULL;
  p = dash_list(h);
  scf=mp_get_pen_scale(mp, mp_pen_p(q));
  if (scf==0) {
    if (*w==0) scf = dash_scale(q); else return NULL;
  } else {
    scf=mp_make_scaled(mp, *w,scf);
    scf=mp_take_scaled(mp, scf,dash_scale(q));
  }
  *w = scf;
  d = xmalloc(1,sizeof(mp_dash_object));
  start_x(null_dash)=start_x(p)+dash_y(h);
  while (p != null_dash) { 
	dashes = xrealloc(dashes, (num_dashes+2), sizeof(scaled));
	dashes[(num_dashes-1)] = 
      mp_take_scaled(mp,(stop_x(p)-start_x(p)),scf);
	dashes[(num_dashes)]   = 
      mp_take_scaled(mp,(start_x(mp_link(p))-stop_x(p)),scf);
	dashes[(num_dashes+1)] = -1; /* terminus */
	num_dashes+=2;
    p=mp_link(p);
  }
  d->array  = dashes;
  d->offset = mp_take_scaled(mp,mp_dash_offset(mp, h),scf);
  return d;
}



@ @<Copy the bounding box information from |h| to |hh|...@>=
minx_val(hh)=minx_val(h);
miny_val(hh)=miny_val(h);
maxx_val(hh)=maxx_val(h);
maxy_val(hh)=maxy_val(h);
bbtype(hh)=bbtype(h);
p=dummy_loc(h); pp=dummy_loc(hh);
while ((p!=bblast(h)) ) { 
  if ( p==null ) mp_confusion(mp, "bblast");
@:this can't happen bblast}{\quad bblast@>
  p=mp_link(p); pp=mp_link(pp);
}
bblast(hh)=pp

@ Here is the promised routine for copying graphical objects into a new edge
structure.  It starts copying at object~|p| and stops just before object~|q|.
If |q| is null, it copies the entire sublist headed at |p|.  The resulting edge
structure requires further initialization by |init_bbox|.

@<Declarations@>=
static pointer mp_copy_objects (MP mp, pointer p, pointer q);

@ @c
pointer mp_copy_objects (MP mp, pointer p, pointer q) {
  pointer hh;  /* the new edge header */
  pointer pp;  /* the last newly copied object */
  quarterword k;  /* temporary register */
  hh=mp_get_node(mp, edge_header_size);
  dash_list(hh)=null_dash;
  ref_count(hh)=null;
  pp=dummy_loc(hh);
  while ( (p!=q) ) {
    @<Make |mp_link(pp)| point to a copy of object |p|, and update |p| and |pp|@>;
  }
  obj_tail(hh)=pp;
  mp_link(pp)=null;
  return hh;
}

@ @<Make |mp_link(pp)| point to a copy of object |p|, and update |p| and |pp|@>=
{ k=mp->gr_object_size[mp_type(p)];
  mp_link(pp)=mp_get_node(mp, k);
  pp=mp_link(pp);
  while ( (k>0) ) { k--; mp->mem[pp+k]=mp->mem[p+k];  };
  @<Fix anything in graphical object |pp| that should differ from the
    corresponding field in |p|@>;
  p=mp_link(p);
}

@ @<Fix anything in graphical object |pp| that should differ from the...@>=
switch (mp_type(p)) {
case mp_start_clip_code:
case mp_start_bounds_code: 
  mp_path_p(pp)=mp_copy_path(mp, mp_path_p(p));
  break;
case mp_fill_code: 
  mp_path_p(pp)=mp_copy_path(mp, mp_path_p(p));
  if ( mp_pre_script(p)!=null )  add_str_ref(mp_pre_script(p));
  if ( mp_post_script(p)!=null ) add_str_ref(mp_post_script(p));
  if ( mp_pen_p(p)!=null ) mp_pen_p(pp)=copy_pen(mp_pen_p(p));
  break;
case mp_stroked_code: 
  if ( mp_pre_script(p)!=null )  add_str_ref(mp_pre_script(p));
  if ( mp_post_script(p)!=null ) add_str_ref(mp_post_script(p));
  mp_path_p(pp)=mp_copy_path(mp, mp_path_p(p));
  mp_pen_p(pp)=copy_pen(mp_pen_p(p));
  if ( mp_dash_p(p)!=null ) add_edge_ref(mp_dash_p(pp));
  break;
case mp_text_code: 
  if ( mp_pre_script(p)!=null )  add_str_ref(mp_pre_script(p));
  if ( mp_post_script(p)!=null ) add_str_ref(mp_post_script(p));
  add_str_ref(mp_text_p(pp));
  break;
case mp_stop_clip_code:
case mp_stop_bounds_code: 
  break;
}  /* there are no other cases */

@ Here is one way to find an acceptable value for the second argument to
|copy_objects|.  Given a non-null graphical object list, |skip_1component|
skips past one picture component, where a ``picture component'' is a single
graphical object, or a start bounds or start clip object and everything up
through the matching stop bounds or stop clip object.  The macro version avoids
procedure call overhead and error handling: |skip_component(p)(e)| advances |p|
unless |p| points to a stop bounds or stop clip node, in which case it executes
|e| instead.

@d skip_component(A)
    if ( ! is_start_or_stop((A)) ) (A)=mp_link((A));
    else if ( ! is_stop((A)) ) (A)=mp_skip_1component(mp, (A));
    else 

@c 
static pointer mp_skip_1component (MP mp,pointer p) {
  integer lev; /* current nesting level */
  lev=0;
  do {  
   if ( is_start_or_stop(p) ) {
     if ( is_stop(p) ) decr(lev);  else incr(lev);
   }
   p=mp_link(p);
  } while (lev!=0);
  return p;
}

@ Here is a diagnostic routine for printing an edge structure in symbolic form.

@<Declarations@>=
static void mp_print_edges (MP mp,pointer h, const char *s, boolean nuline) ;

@ @c
void mp_print_edges (MP mp,pointer h, const char *s, boolean nuline) {
  pointer p;  /* a graphical object to be printed */
  pointer hh,pp;  /* temporary pointers */
  scaled scf;  /* a scale factor for the dash pattern */
  boolean ok_to_dash;  /* |false| for polygonal pen strokes */
  mp_print_diagnostic(mp, "Edge structure",s,nuline);
  p=dummy_loc(h);
  while ( mp_link(p)!=null ) { 
    p=mp_link(p);
    mp_print_ln(mp);
    switch (mp_type(p)) {
      @<Cases for printing graphical object node |p|@>;
    default: 
	  mp_print(mp, "[unknown object type!]");
	  break;
    }
  }
  mp_print_nl(mp, "End edges");
  if ( p!=obj_tail(h) ) mp_print(mp, "?");
@.End edges?@>
  mp_end_diagnostic(mp, true);
}

@ @<Cases for printing graphical object node |p|@>=
case mp_fill_code: 
  mp_print(mp, "Filled contour ");
  mp_print_obj_color(mp, p);
  mp_print_char(mp, xord(':')); mp_print_ln(mp);
  mp_pr_path(mp, mp_path_p(p)); mp_print_ln(mp);
  if ( (mp_pen_p(p)!=null) ) {
    @<Print join type for graphical object |p|@>;
    mp_print(mp, " with pen"); mp_print_ln(mp);
    mp_pr_pen(mp, mp_pen_p(p));
  }
  break;

@ @<Print join type for graphical object |p|@>=
switch (ljoin_val(p)) {
case 0:
  mp_print(mp, "mitered joins limited ");
  mp_print_scaled(mp, miterlim_val(p));
  break;
case 1:
  mp_print(mp, "round joins");
  break;
case 2:
  mp_print(mp, "beveled joins");
  break;
default: 
  mp_print(mp, "?? joins");
@.??@>
  break;
}

@ For stroked nodes, we need to print |lcap_val(p)| as well.

@<Print join and cap types for stroked node |p|@>=
switch (lcap_val(p)) {
case 0:mp_print(mp, "butt"); break;
case 1:mp_print(mp, "round"); break;
case 2:mp_print(mp, "square"); break;
default: mp_print(mp, "??"); break;
@.??@>
}
mp_print(mp, " ends, ");
@<Print join type for graphical object |p|@>

@ Here is a routine that prints the color of a graphical object if it isn't
black (the default color).

@<Declarations@>=
static void mp_print_obj_color (MP mp,pointer p) ;

@ @c
void mp_print_obj_color (MP mp,pointer p) { 
  if ( mp_color_model(p)==mp_grey_model ) {
    if ( grey_val(p)>0 ) { 
      mp_print(mp, "greyed ");
      mp_print_compact_node(mp, obj_grey_loc(p),1);
    };
  } else if ( mp_color_model(p)==mp_cmyk_model ) {
    if ( (cyan_val(p)>0) || (magenta_val(p)>0) || 
         (yellow_val(p)>0) || (black_val(p)>0) ) { 
      mp_print(mp, "processcolored ");
      mp_print_compact_node(mp, obj_cyan_loc(p),4);
    };
  } else if ( mp_color_model(p)==mp_rgb_model ) {
    if ( (red_val(p)>0) || (green_val(p)>0) || (blue_val(p)>0) ) { 
      mp_print(mp, "colored "); 
      mp_print_compact_node(mp, obj_red_loc(p),3);
    };
  }
}

@ We also need a procedure for printing consecutive scaled values as if they
were a known big node.

@<Declarations@>=
static void mp_print_compact_node (MP mp,pointer p, quarterword k) ;

@ @c
void mp_print_compact_node (MP mp,pointer p, quarterword k) {
  pointer q;  /* last location to print */
  q=p+k-1;
  mp_print_char(mp, xord('('));
  while ( p<=q ){ 
    mp_print_scaled(mp, mp->mem[p].sc);
    if ( p<q ) mp_print_char(mp, xord(','));
    incr(p);
  }
  mp_print_char(mp, xord(')'));
}

@ @<Cases for printing graphical object node |p|@>=
case mp_stroked_code: 
  mp_print(mp, "Filled pen stroke ");
  mp_print_obj_color(mp, p);
  mp_print_char(mp, xord(':')); mp_print_ln(mp);
  mp_pr_path(mp, mp_path_p(p));
  if ( mp_dash_p(p)!=null ) { 
    mp_print_nl(mp, "dashed (");
    @<Finish printing the dash pattern that |p| refers to@>;
  }
  mp_print_ln(mp);
  @<Print join and cap types for stroked node |p|@>;
  mp_print(mp, " with pen"); mp_print_ln(mp);
  if ( mp_pen_p(p)==null ) mp_print(mp, "???"); /* shouldn't happen */
@.???@>
  else mp_pr_pen(mp, mp_pen_p(p));
  break;

@ Normally, the  |dash_list| field in an edge header is set to |null_dash|
when it is not known to define a suitable dash pattern.  This is disallowed
here because the |mp_dash_p| field should never point to such an edge header.
Note that memory is allocated for |start_x(null_dash)| and we are free to
give it any convenient value.

@<Finish printing the dash pattern that |p| refers to@>=
ok_to_dash=pen_is_elliptical(mp_pen_p(p));
if ( ! ok_to_dash ) scf=unity; else scf=dash_scale(p);
hh=mp_dash_p(p);
pp=dash_list(hh);
if ( (pp==null_dash) || (dash_y(hh)<0) ) {
  mp_print(mp, " ??");
} else { start_x(null_dash)=start_x(pp)+dash_y(hh);
  while ( pp!=null_dash ) { 
    mp_print(mp, "on ");
    mp_print_scaled(mp, mp_take_scaled(mp, stop_x(pp)-start_x(pp),scf));
    mp_print(mp, " off ");
    mp_print_scaled(mp, mp_take_scaled(mp, start_x(mp_link(pp))-stop_x(pp),scf));
    pp = mp_link(pp);
    if ( pp!=null_dash ) mp_print_char(mp, xord(' '));
  }
  mp_print(mp, ") shifted ");
  mp_print_scaled(mp, -mp_take_scaled(mp, mp_dash_offset(mp, hh),scf));
  if ( ! ok_to_dash || (dash_y(hh)==0) ) mp_print(mp, " (this will be ignored)");
}

@ @<Declarations@>=
static scaled mp_dash_offset (MP mp,pointer h) ;

@ @c
scaled mp_dash_offset (MP mp,pointer h) {
  scaled x;  /* the answer */
  if (dash_list(h)==null_dash || dash_y(h)<0) mp_confusion(mp, "dash0");
@:this can't happen dash0}{\quad dash0@>
  if ( dash_y(h)==0 ) {
    x=0; 
  } else { 
    x=-(start_x(dash_list(h)) % dash_y(h));
    if ( x<0 ) x=x+dash_y(h);
  }
  return x;
}

@ @<Cases for printing graphical object node |p|@>=
case mp_text_code: 
  mp_print_char(mp, xord('"')); mp_print_str(mp,mp_text_p(p));
  mp_print(mp, "\" infont \""); mp_print(mp, mp->font_name[mp_font_n(p)]);
  mp_print_char(mp, xord('"')); mp_print_ln(mp);
  mp_print_obj_color(mp, p);
  mp_print(mp, "transformed ");
  mp_print_compact_node(mp, text_tx_loc(p),6);
  break;

@ @<Cases for printing graphical object node |p|@>=
case mp_start_clip_code: 
  mp_print(mp, "clipping path:");
  mp_print_ln(mp);
  mp_pr_path(mp, mp_path_p(p));
  break;
case mp_stop_clip_code: 
  mp_print(mp, "stop clipping");
  break;

@ @<Cases for printing graphical object node |p|@>=
case mp_start_bounds_code: 
  mp_print(mp, "setbounds path:");
  mp_print_ln(mp);
  mp_pr_path(mp, mp_path_p(p));
  break;
case mp_stop_bounds_code: 
  mp_print(mp, "end of setbounds");
  break;

@ To initialize the |dash_list| field in an edge header~|h|, we need a
subroutine that scans an edge structure and tries to interpret it as a dash
pattern.  This can only be done when there are no filled regions or clipping
paths and all the pen strokes have the same color.  The first step is to let
$y_0$ be the initial $y$~coordinate of the first pen stroke.  Then we implicitly
project all the pen stroke paths onto the line $y=y_0$ and require that there
be no retracing.  If the resulting paths cover a range of $x$~coordinates of
length $\Delta x$, we set |dash_y(h)| to the length of the dash pattern by
finding the maximum of $\Delta x$ and the absolute value of~$y_0$.

@c 
static pointer mp_make_dashes (MP mp,pointer h) { /* returns |h| or |null| */
  pointer p;  /* this scans the stroked nodes in the object list */
  pointer p0;  /* if not |null| this points to the first stroked node */
  pointer pp,qq,rr;  /* pointers into |mp_path_p(p)| */
  pointer d,dd;  /* pointers used to create the dash list */
  scaled y0;
  @<Other local variables in |make_dashes|@>;
  y0=0;  /* the initial $y$ coordinate */
  if ( dash_list(h)!=null_dash ) 
	return h;
  p0=null;
  p=mp_link(dummy_loc(h));
  while ( p!=null ) { 
    if ( mp_type(p)!=mp_stroked_code ) {
      @<Compain that the edge structure contains a node of the wrong type
        and |goto not_found|@>;
    }
    pp=mp_path_p(p);
    if ( p0==null ){ p0=p; y0=mp_y_coord(pp);  };
    @<Make |d| point to a new dash node created from stroke |p| and path |pp|
      or |goto not_found| if there is an error@>;
    @<Insert |d| into the dash list and |goto not_found| if there is an error@>;
    p=mp_link(p);
  }
  if ( dash_list(h)==null_dash ) 
    goto NOT_FOUND; /* No error message */
  @<Scan |dash_list(h)| and deal with any dashes that are themselves dashed@>;
  @<Set |dash_y(h)| and merge the first and last dashes if necessary@>;
  return h;
NOT_FOUND: 
  @<Flush the dash list, recycle |h| and return |null|@>;
}

@ @<Compain that the edge structure contains a node of the wrong type...@>=
{ 
print_err("Picture is too complicated to use as a dash pattern");
help3("When you say `dashed p', picture p should not contain any",
  "text, filled regions, or clipping paths.  This time it did",
  "so I'll just make it a solid line instead.");
mp_put_get_error(mp);
goto NOT_FOUND;
}

@ A similar error occurs when monotonicity fails.

@<Declarations@>=
static void mp_x_retrace_error (MP mp) ;

@ @c
void mp_x_retrace_error (MP mp) { 
print_err("Picture is too complicated to use as a dash pattern");
help3("When you say `dashed p', every path in p should be monotone",
  "in x and there must be no overlapping.  This failed",
  "so I'll just make it a solid line instead.");
mp_put_get_error(mp);
}

@ We stash |p| in |mp_info(d)| if |mp_dash_p(p)<>0| so that subsequent processing can
handle the case where the pen stroke |p| is itself dashed.

@<Make |d| point to a new dash node created from stroke |p| and path...@>=
@<Make sure |p| and |p0| are the same color and |goto not_found| if there is
  an error@>;
rr=pp;
if ( mp_link(pp)!=pp ) {
  do {  
    qq=rr; rr=mp_link(rr);
    @<Check for retracing between knots |qq| and |rr| and |goto not_found|
      if there is a problem@>;
  } while (mp_right_type(rr)!=mp_endpoint);
}
d=mp_get_node(mp, dash_node_size);
if ( mp_dash_p(p)==0 ) mp_info(d)=0;  else mp_info(d)=p;
if ( mp_x_coord(pp)<mp_x_coord(rr) ) { 
  start_x(d)=mp_x_coord(pp);
  stop_x(d)=mp_x_coord(rr);
} else { 
  start_x(d)=mp_x_coord(rr);
  stop_x(d)=mp_x_coord(pp);
}

@ We also need to check for the case where the segment from |qq| to |rr| is
monotone in $x$ but is reversed relative to the path from |pp| to |qq|.

@<Check for retracing between knots |qq| and |rr| and |goto not_found|...@>=
x0=mp_x_coord(qq);
x1=mp_right_x(qq);
x2=mp_left_x(rr);
x3=mp_x_coord(rr);
if ( (x0>x1) || (x1>x2) || (x2>x3) ) {
  if ( (x0<x1) || (x1<x2) || (x2<x3) ) {
    if ( mp_ab_vs_cd(mp, x2-x1,x2-x1,x1-x0,x3-x2)>0 ) {
      mp_x_retrace_error(mp); goto NOT_FOUND;
    }
  }
}
if ( (mp_x_coord(pp)>x0) || (x0>x3) ) {
  if ( (mp_x_coord(pp)<x0) || (x0<x3) ) {
    mp_x_retrace_error(mp); goto NOT_FOUND;
  }
}

@ @<Other local variables in |make_dashes|@>=
  scaled x0,x1,x2,x3;  /* $x$ coordinates of the segment from |qq| to |rr| */

@ @<Make sure |p| and |p0| are the same color and |goto not_found|...@>=
if ( (red_val(p)!=red_val(p0)) || (black_val(p)!=black_val(p0)) ||
  (green_val(p)!=green_val(p0)) || (blue_val(p)!=blue_val(p0)) ) {
  print_err("Picture is too complicated to use as a dash pattern");
  help3("When you say `dashed p', everything in picture p should",
    "be the same color.  I can\'t handle your color changes",
    "so I'll just make it a solid line instead.");
  mp_put_get_error(mp);
  goto NOT_FOUND;
}

@ @<Insert |d| into the dash list and |goto not_found| if there is an error@>=
start_x(null_dash)=stop_x(d);
dd=h; /* this makes |mp_link(dd)=dash_list(h)| */
while ( start_x(mp_link(dd))<stop_x(d) )
  dd=mp_link(dd);
if ( dd!=h ) {
  if ( (stop_x(dd)>start_x(d)) )
    { mp_x_retrace_error(mp); goto NOT_FOUND;  };
}
mp_link(d)=mp_link(dd);
mp_link(dd)=d

@ @<Set |dash_y(h)| and merge the first and last dashes if necessary@>=
d=dash_list(h);
while ( (mp_link(d)!=null_dash) )
  d=mp_link(d);
dd=dash_list(h);
dash_y(h)=stop_x(d)-start_x(dd);
if ( abs(y0)>dash_y(h) ) {
  dash_y(h)=abs(y0);
} else if ( d!=dd ) { 
  dash_list(h)=mp_link(dd);
  stop_x(d)=stop_x(dd)+dash_y(h);
  mp_free_node(mp, dd,dash_node_size);
}

@ We get here when the argument is a null picture or when there is an error.
Recovering from an error involves making |dash_list(h)| empty to indicate
that |h| is not known to be a valid dash pattern.  We also dereference |h|
since it is not being used for the return value.

@<Flush the dash list, recycle |h| and return |null|@>=
mp_flush_dash_list(mp, h);
delete_edge_ref(h);
return null

@ Having carefully saved the dashed stroked nodes in the
corresponding dash nodes, we must be prepared to break up these dashes into
smaller dashes.

@<Scan |dash_list(h)| and deal with any dashes that are themselves dashed@>=
d=h;  /* now |mp_link(d)=dash_list(h)| */
while ( mp_link(d)!=null_dash ) {
  ds=mp_info(mp_link(d));
  if ( ds==null ) { 
    d=mp_link(d);
  } else {
    hh=mp_dash_p(ds);
    hsf=dash_scale(ds);
    if ( (hh==null) ) mp_confusion(mp, "dash1");
@:this can't happen dash0}{\quad dash1@>
    if ( dash_y(hh)==0 ) {
      d=mp_link(d);
    } else { 
      if ( dash_list(hh)==null ) mp_confusion(mp, "dash1");
@:this can't happen dash0}{\quad dash1@>
      @<Replace |mp_link(d)| by a dashed version as determined by edge header
          |hh| and scale factor |ds|@>;
    }
  }
}

@ @<Other local variables in |make_dashes|@>=
pointer dln;  /* |mp_link(d)| */
pointer hh;  /* an edge header that tells how to break up |dln| */
scaled hsf;  /* the dash pattern from |hh| gets scaled by this */
pointer ds;  /* the stroked node from which |hh| and |hsf| are derived */
scaled xoff;  /* added to $x$ values in |dash_list(hh)| to match |dln| */

@ @<Replace |mp_link(d)| by a dashed version as determined by edge header...@>=
dln=mp_link(d);
dd=dash_list(hh);
xoff=start_x(dln)-mp_take_scaled(mp, hsf,start_x(dd))-
        mp_take_scaled(mp, hsf,mp_dash_offset(mp, hh));
start_x(null_dash)=mp_take_scaled(mp, hsf,start_x(dd))
                   +mp_take_scaled(mp, hsf,dash_y(hh));
stop_x(null_dash)=start_x(null_dash);
@<Advance |dd| until finding the first dash that overlaps |dln| when
  offset by |xoff|@>;
while ( start_x(dln)<=stop_x(dln) ) {
  @<If |dd| has `fallen off the end', back up to the beginning and fix |xoff|@>;
  @<Insert a dash between |d| and |dln| for the overlap with the offset version
    of |dd|@>;
  dd=mp_link(dd);
  start_x(dln)=xoff+mp_take_scaled(mp, hsf,start_x(dd));
}
mp_link(d)=mp_link(dln);
mp_free_node(mp, dln,dash_node_size)

@ The name of this module is a bit of a lie because we just find the
first |dd| where |take_scaled (hsf, stop_x(dd))| is large enough to make an
overlap possible.  It could be that the unoffset version of dash |dln| falls
in the gap between |dd| and its predecessor.

@<Advance |dd| until finding the first dash that overlaps |dln| when...@>=
while ( xoff+mp_take_scaled(mp, hsf,stop_x(dd))<start_x(dln) ) {
  dd=mp_link(dd);
}

@ @<If |dd| has `fallen off the end', back up to the beginning and fix...@>=
if ( dd==null_dash ) { 
  dd=dash_list(hh);
  xoff=xoff+mp_take_scaled(mp, hsf,dash_y(hh));
}

@ At this point we already know that
|start_x(dln)<=xoff+take_scaled(hsf,stop_x(dd))|.

@<Insert a dash between |d| and |dln| for the overlap with the offset...@>=
if ( (xoff+mp_take_scaled(mp, hsf,start_x(dd)))<=stop_x(dln) ) {
  mp_link(d)=mp_get_node(mp, dash_node_size);
  d=mp_link(d);
  mp_link(d)=dln;
  if ( start_x(dln)>(xoff+mp_take_scaled(mp, hsf,start_x(dd))))
    start_x(d)=start_x(dln);
  else 
    start_x(d)=xoff+mp_take_scaled(mp, hsf,start_x(dd));
  if ( stop_x(dln)<(xoff+mp_take_scaled(mp, hsf,stop_x(dd)))) 
    stop_x(d)=stop_x(dln);
  else 
    stop_x(d)=xoff+mp_take_scaled(mp, hsf,stop_x(dd));
}

@ The next major task is to update the bounding box information in an edge
header~|h|. This is done via a procedure |adjust_bbox| that enlarges an edge
header's bounding box to accommodate the box computed by |path_bbox| or
|pen_bbox|. (This is stored in global variables |minx|, |miny|, |maxx|, and
|maxy|.)

@c static void mp_adjust_bbox (MP mp,pointer h) { 
  if ( mp_minx<minx_val(h) ) minx_val(h)=mp_minx;
  if ( mp_miny<miny_val(h) ) miny_val(h)=mp_miny;
  if ( mp_maxx>maxx_val(h) ) maxx_val(h)=mp_maxx;
  if ( mp_maxy>maxy_val(h) ) maxy_val(h)=mp_maxy;
}

@ Here is a special routine for updating the bounding box information in
edge header~|h| to account for the squared-off ends of a non-cyclic path~|p|
that is to be stroked with the pen~|pp|.

@c static void mp_box_ends (MP mp, pointer p, pointer pp, pointer h) {
  pointer q;  /* a knot node adjacent to knot |p| */
  fraction dx,dy;  /* a unit vector in the direction out of the path at~|p| */
  scaled d;  /* a factor for adjusting the length of |(dx,dy)| */
  scaled z;  /* a coordinate being tested against the bounding box */
  scaled xx,yy;  /* the extreme pen vertex in the |(dx,dy)| direction */
  integer i; /* a loop counter */
  if ( mp_right_type(p)!=mp_endpoint ) { 
    q=mp_link(p);
    while (1) { 
      @<Make |(dx,dy)| the final direction for the path segment from
        |q| to~|p|; set~|d|@>;
      d=mp_pyth_add(mp, dx,dy);
      if ( d>0 ) { 
         @<Normalize the direction |(dx,dy)| and find the pen offset |(xx,yy)|@>;
         for (i=1;i<= 2;i++) { 
           @<Use |(dx,dy)| to generate a vertex of the square end cap and
             update the bounding box to accommodate it@>;
           dx=-dx; dy=-dy; 
        }
      }
      if ( mp_right_type(p)==mp_endpoint ) {
         return;
      } else {
        @<Advance |p| to the end of the path and make |q| the previous knot@>;
      } 
    }
  }
}

@ @<Make |(dx,dy)| the final direction for the path segment from...@>=
if ( q==mp_link(p) ) { 
  dx=mp_x_coord(p)-mp_right_x(p);
  dy=mp_y_coord(p)-mp_right_y(p);
  if ( (dx==0)&&(dy==0) ) {
    dx=mp_x_coord(p)-mp_left_x(q);
    dy=mp_y_coord(p)-mp_left_y(q);
  }
} else { 
  dx=mp_x_coord(p)-mp_left_x(p);
  dy=mp_y_coord(p)-mp_left_y(p);
  if ( (dx==0)&&(dy==0) ) {
    dx=mp_x_coord(p)-mp_right_x(q);
    dy=mp_y_coord(p)-mp_right_y(q);
  }
}
dx=mp_x_coord(p)-mp_x_coord(q);
dy=mp_y_coord(p)-mp_y_coord(q)

@ @<Normalize the direction |(dx,dy)| and find the pen offset |(xx,yy)|@>=
dx=mp_make_fraction(mp, dx,d);
dy=mp_make_fraction(mp, dy,d);
mp_find_offset(mp, -dy,dx,pp);
xx=mp->cur_x; yy=mp->cur_y

@ @<Use |(dx,dy)| to generate a vertex of the square end cap and...@>=
mp_find_offset(mp, dx,dy,pp);
d=mp_take_fraction(mp, xx-mp->cur_x,dx)+mp_take_fraction(mp, yy-mp->cur_y,dy);
if ( ((d<0)&&(i==1)) || ((d>0)&&(i==2))) 
  mp_confusion(mp, "box_ends");
@:this can't happen box ends}{\quad\\{box\_ends}@>
z=mp_x_coord(p)+mp->cur_x+mp_take_fraction(mp, d,dx);
if ( z<minx_val(h) ) minx_val(h)=z;
if ( z>maxx_val(h) ) maxx_val(h)=z;
z=mp_y_coord(p)+mp->cur_y+mp_take_fraction(mp, d,dy);
if ( z<miny_val(h) ) miny_val(h)=z;
if ( z>maxy_val(h) ) maxy_val(h)=z

@ @<Advance |p| to the end of the path and make |q| the previous knot@>=
do {  
  q=p;
  p=mp_link(p);
} while (mp_right_type(p)!=mp_endpoint)

@ The major difficulty in finding the bounding box of an edge structure is the
effect of clipping paths.  We treat them conservatively by only clipping to the
clipping path's bounding box, but this still
requires recursive calls to |set_bbox| in order to find the bounding box of
@^recursion@>
the objects to be clipped.  Such calls are distinguished by the fact that the
boolean parameter |top_level| is false.

@c 
void mp_set_bbox (MP mp,pointer h, boolean top_level) {
  pointer p;  /* a graphical object being considered */
  scaled sminx,sminy,smaxx,smaxy;
  /* for saving the bounding box during recursive calls */
  scaled x0,x1,y0,y1;  /* temporary registers */
  integer lev;  /* nesting level for |mp_start_bounds_code| nodes */
  @<Wipe out any existing bounding box information if |bbtype(h)| is
  incompatible with |internal[mp_true_corners]|@>;
  while ( mp_link(bblast(h))!=null ) { 
    p=mp_link(bblast(h));
    bblast(h)=p;
    switch (mp_type(p)) {
    case mp_stop_clip_code: 
      if ( top_level ) mp_confusion(mp, "bbox");  else return;
@:this can't happen bbox}{\quad bbox@>
      break;
    @<Other cases for updating the bounding box based on the type of object |p|@>;
    } /* all cases are enumerated above */
  }
  if ( ! top_level ) mp_confusion(mp, "bbox");
}

@ @<Declarations@>=
static void mp_set_bbox (MP mp,pointer h, boolean top_level);

@ @<Wipe out any existing bounding box information if |bbtype(h)| is...@>=
switch (bbtype(h)) {
case no_bounds: 
  break;
case bounds_set: 
  if ( mp->internal[mp_true_corners]>0 ) mp_init_bbox(mp, h);
  break;
case bounds_unset: 
  if ( mp->internal[mp_true_corners]<=0 ) mp_init_bbox(mp, h);
  break;
} /* there are no other cases */

@ @<Other cases for updating the bounding box...@>=
case mp_fill_code: 
  mp_path_bbox(mp, mp_path_p(p));
  if ( mp_pen_p(p)!=null ) { 
    x0=mp_minx; y0=mp_miny;
    x1=mp_maxx; y1=mp_maxy;
    mp_pen_bbox(mp, mp_pen_p(p));
    mp_minx=mp_minx+x0;
    mp_miny=mp_miny+y0;
    mp_maxx=mp_maxx+x1;
    mp_maxy=mp_maxy+y1;
  }
  mp_adjust_bbox(mp, h);
  break;

@ @<Other cases for updating the bounding box...@>=
case mp_start_bounds_code: 
  if ( mp->internal[mp_true_corners]>0 ) {
    bbtype(h)=bounds_unset;
  } else { 
    bbtype(h)=bounds_set;
    mp_path_bbox(mp, mp_path_p(p));
    mp_adjust_bbox(mp, h);
    @<Scan to the matching |mp_stop_bounds_code| node and update |p| and
      |bblast(h)|@>;
  }
  break;
case mp_stop_bounds_code: 
  if ( mp->internal[mp_true_corners]<=0 ) mp_confusion(mp, "bbox2");
@:this can't happen bbox2}{\quad bbox2@>
  break;

@ @<Scan to the matching |mp_stop_bounds_code| node and update |p| and...@>=
lev=1;
while ( lev!=0 ) { 
  if ( mp_link(p)==null ) mp_confusion(mp, "bbox2");
@:this can't happen bbox2}{\quad bbox2@>
  p=mp_link(p);
  if ( mp_type(p)==mp_start_bounds_code ) incr(lev);
  else if ( mp_type(p)==mp_stop_bounds_code ) decr(lev);
}
bblast(h)=p

@ It saves a lot of grief here to be slightly conservative and not account for
omitted parts of dashed lines.  We also don't worry about the material omitted
when using butt end caps.  The basic computation is for round end caps and
|box_ends| augments it for square end caps.

@<Other cases for updating the bounding box...@>=
case mp_stroked_code: 
  mp_path_bbox(mp, mp_path_p(p));
  x0=mp_minx; y0=mp_miny;
  x1=mp_maxx; y1=mp_maxy;
  mp_pen_bbox(mp, mp_pen_p(p));
  mp_minx=mp_minx+x0;
  mp_miny=mp_miny+y0;
  mp_maxx=mp_maxx+x1;
  mp_maxy=mp_maxy+y1;
  mp_adjust_bbox(mp, h);
  if ( (mp_left_type(mp_path_p(p))==mp_endpoint)&&(lcap_val(p)==2) )
    mp_box_ends(mp, mp_path_p(p), mp_pen_p(p), h);
  break;

@ The height width and depth information stored in a text node determines a
rectangle that needs to be transformed according to the transformation
parameters stored in the text node.

@<Other cases for updating the bounding box...@>=
case mp_text_code: 
  x1=mp_take_scaled(mp, txx_val(p),width_val(p));
  y0=mp_take_scaled(mp, txy_val(p),-depth_val(p));
  y1=mp_take_scaled(mp, txy_val(p),height_val(p));
  mp_minx=tx_val(p);
  mp_maxx=mp_minx;
  if ( y0<y1 ) { mp_minx=mp_minx+y0; mp_maxx=mp_maxx+y1;  }
  else         { mp_minx=mp_minx+y1; mp_maxx=mp_maxx+y0;  }
  if ( x1<0 ) mp_minx=mp_minx+x1;  else mp_maxx=mp_maxx+x1;
  x1=mp_take_scaled(mp, tyx_val(p),width_val(p));
  y0=mp_take_scaled(mp, tyy_val(p),-depth_val(p));
  y1=mp_take_scaled(mp, tyy_val(p),height_val(p));
  mp_miny=ty_val(p);
  mp_maxy=mp_miny;
  if ( y0<y1 ) { mp_miny=mp_miny+y0; mp_maxy=mp_maxy+y1;  }
  else         { mp_miny=mp_miny+y1; mp_maxy=mp_maxy+y0;  }
  if ( x1<0 ) mp_miny=mp_miny+x1;  else mp_maxy=mp_maxy+x1;
  mp_adjust_bbox(mp, h);
  break;

@ This case involves a recursive call that advances |bblast(h)| to the node of
type |mp_stop_clip_code| that matches |p|.

@<Other cases for updating the bounding box...@>=
case mp_start_clip_code: 
  mp_path_bbox(mp, mp_path_p(p));
  x0=mp_minx; y0=mp_miny;
  x1=mp_maxx; y1=mp_maxy;
  sminx=minx_val(h); sminy=miny_val(h);
  smaxx=maxx_val(h); smaxy=maxy_val(h);
  @<Reinitialize the bounding box in header |h| and call |set_bbox| recursively
    starting at |mp_link(p)|@>;
  @<Clip the bounding box in |h| to the rectangle given by |x0|, |x1|,
    |y0|, |y1|@>;
  mp_minx=sminx; mp_miny=sminy;
  mp_maxx=smaxx; mp_maxy=smaxy;
  mp_adjust_bbox(mp, h);
  break;

@ @<Reinitialize the bounding box in header |h| and call |set_bbox|...@>=
minx_val(h)=el_gordo;
miny_val(h)=el_gordo;
maxx_val(h)=-el_gordo;
maxy_val(h)=-el_gordo;
mp_set_bbox(mp, h,false)

@ @<Clip the bounding box in |h| to the rectangle given by |x0|, |x1|,...@>=
if ( minx_val(h)<x0 ) minx_val(h)=x0;
if ( miny_val(h)<y0 ) miny_val(h)=y0;
if ( maxx_val(h)>x1 ) maxx_val(h)=x1;
if ( maxy_val(h)>y1 ) maxy_val(h)=y1

@* \[22] Finding an envelope.
When \MP\ has a path and a polygonal pen, it needs to express the desired
shape in terms of things \ps\ can understand.  The present task is to compute
a new path that describes the region to be filled.  It is convenient to
define this as a two step process where the first step is determining what
offset to use for each segment of the path.

@ Given a pointer |c| to a cyclic path,
and a pointer~|h| to the first knot of a pen polygon,
the |offset_prep| routine changes the path into cubics that are
associated with particular pen offsets. Thus if the cubic between |p|
and~|q| is associated with the |k|th offset and the cubic between |q| and~|r|
has offset |l| then |mp_info(q)=zero_off+l-k|. (The constant |zero_off| is added
to because |l-k| could be negative.)

After overwriting the type information with offset differences, we no longer
have a true path so we refer to the knot list returned by |offset_prep| as an
``envelope spec.''
@^envelope spec@>
Since an envelope spec only determines relative changes in pen offsets,
|offset_prep| sets a global variable |spec_offset| to the relative change from
|h| to the first offset.

@d zero_off 16384 /* added to offset changes to make them positive */

@<Glob...@>=
integer spec_offset; /* number of pen edges between |h| and the initial offset */

@ @c
static pointer mp_offset_prep (MP mp,pointer c, pointer h) {
  halfword n; /* the number of vertices in the pen polygon */
  pointer c0,p,q,q0,r,w, ww; /* for list manipulation */
  integer k_needed; /* amount to be added to |mp_info(p)| when it is computed */
  pointer w0; /* a pointer to pen offset to use just before |p| */
  scaled dxin,dyin; /* the direction into knot |p| */
  integer turn_amt; /* change in pen offsets for the current cubic */
  @<Other local variables for |offset_prep|@>;
  dx0=0; dy0=0;
  @<Initialize the pen size~|n|@>;
  @<Initialize the incoming direction and pen offset at |c|@>;
  p=c; c0=c; k_needed=0;
  do {  
    q=mp_link(p);
    @<Split the cubic between |p| and |q|, if necessary, into cubics
      associated with single offsets, after which |q| should
      point to the end of the final such cubic@>;
  NOT_FOUND:
    @<Advance |p| to node |q|, removing any ``dead'' cubics that
      might have been introduced by the splitting process@>;
  } while (q!=c);
  @<Fix the offset change in |mp_info(c)| and set |c| to the return value of
    |offset_prep|@>;
  return c;
}

@ We shall want to keep track of where certain knots on the cyclic path
wind up in the envelope spec.  It doesn't suffice just to keep pointers to
knot nodes because some nodes are deleted while removing dead cubics.  Thus
|offset_prep| updates the following pointers

@<Glob...@>=
pointer spec_p1;
pointer spec_p2; /* pointers to distinguished knots */

@ @<Set init...@>=
mp->spec_p1=null; mp->spec_p2=null;

@ @<Initialize the pen size~|n|@>=
n=0; p=h;
do {  
  incr(n);
  p=mp_link(p);
} while (p!=h)

@ Since the true incoming direction isn't known yet, we just pick a direction
consistent with the pen offset~|h|.  If this is wrong, it can be corrected
later.

@<Initialize the incoming direction and pen offset at |c|@>=
dxin=mp_x_coord(mp_link(h))-mp_x_coord(knil(h));
dyin=mp_y_coord(mp_link(h))-mp_y_coord(knil(h));
if ( (dxin==0)&&(dyin==0) ) {
  dxin=mp_y_coord(knil(h))-mp_y_coord(h);
  dyin=mp_x_coord(h)-mp_x_coord(knil(h));
}
w0=h

@ We must be careful not to remove the only cubic in a cycle.

But we must also be careful for another reason. If the user-supplied
path starts with a set of degenerate cubics, the target node |q| can
be collapsed to the initial node |p| which might be the same as the
initial node |c| of the curve. This would cause the |offset_prep| routine
to bail out too early, causing distress later on. (See for example
the testcase reported by Bogus\l{}aw Jackowski in tracker id 267, case 52c
on Sarovar.)

@<Advance |p| to node |q|, removing any ``dead'' cubics...@>=
q0=q;
do { 
  r=mp_link(p);
  if ( mp_x_coord(p)==mp_right_x(p) && mp_y_coord(p)==mp_right_y(p) &&
       mp_x_coord(p)==mp_left_x(r)  && mp_y_coord(p)==mp_left_y(r) &&
       mp_x_coord(p)==mp_x_coord(r) && mp_y_coord(p)==mp_y_coord(r) &&
       r!=p ) {
      @<Remove the cubic following |p| and update the data structures
        to merge |r| into |p|@>;
  }
  p=r;
} while (p!=q);
/* Check if we removed too much */
if ((q!=q0)&&(q!=c||c==c0))
  q = mp_link(q)

@ @<Remove the cubic following |p| and update the data structures...@>=
{ k_needed=mp_info(p)-zero_off;
  if ( r==q ) { 
    q=p;
  } else { 
    mp_info(p)=k_needed+mp_info(r);
    k_needed=0;
  };
  if ( r==c ) { mp_info(p)=mp_info(c); c=p; };
  if ( r==mp->spec_p1 ) mp->spec_p1=p;
  if ( r==mp->spec_p2 ) mp->spec_p2=p;
  r=p; mp_remove_cubic(mp, p);
}

@ Not setting the |info| field of the newly created knot allows the splitting
routine to work for paths.

@<Declarations@>=
static void mp_split_cubic (MP mp,pointer p, fraction t) ;

@ @c
void mp_split_cubic (MP mp,pointer p, fraction t) { /* splits the cubic after |p| */
  scaled v; /* an intermediate value */
  pointer q,r; /* for list manipulation */
  q=mp_link(p); r=mp_get_node(mp, knot_node_size); mp_link(p)=r; mp_link(r)=q;
  mp_originator(r)=mp_program_code;
  mp_left_type(r)=mp_explicit; mp_right_type(r)=mp_explicit;
  v=t_of_the_way(mp_right_x(p),mp_left_x(q));
  mp_right_x(p)=t_of_the_way(mp_x_coord(p),mp_right_x(p));
  mp_left_x(q)=t_of_the_way(mp_left_x(q),mp_x_coord(q));
  mp_left_x(r)=t_of_the_way(mp_right_x(p),v);
  mp_right_x(r)=t_of_the_way(v,mp_left_x(q));
  mp_x_coord(r)=t_of_the_way(mp_left_x(r),mp_right_x(r));
  v=t_of_the_way(mp_right_y(p),mp_left_y(q));
  mp_right_y(p)=t_of_the_way(mp_y_coord(p),mp_right_y(p));
  mp_left_y(q)=t_of_the_way(mp_left_y(q),mp_y_coord(q));
  mp_left_y(r)=t_of_the_way(mp_right_y(p),v);
  mp_right_y(r)=t_of_the_way(v,mp_left_y(q));
  mp_y_coord(r)=t_of_the_way(mp_left_y(r),mp_right_y(r));
}

@ This does not set |mp_info(p)| or |mp_right_type(p)|.

@<Declarations@>=
static void mp_remove_cubic (MP mp,pointer p) ; 

@ @c
void mp_remove_cubic (MP mp,pointer p) { /* removes the dead cubic following~|p| */
  pointer q; /* the node that disappears */
  q=mp_link(p); mp_link(p)=mp_link(q);
  mp_right_x(p)=mp_right_x(q); mp_right_y(p)=mp_right_y(q);
  mp_free_node(mp, q,knot_node_size);
}

@ Let $d\prec d'$ mean that the counter-clockwise angle from $d$ to~$d'$ is
strictly between zero and $180^\circ$.  Then we can define $d\preceq d'$ to
mean that the angle could be zero or $180^\circ$. If $w_k=(u_k,v_k)$ is the
$k$th pen offset, the $k$th pen edge direction is defined by the formula
$$d_k=(u\k-u_k,\,v\k-v_k).$$
When listed by increasing $k$, these directions occur in counter-clockwise
order so that $d_k\preceq d\k$ for all~$k$.
The goal of |offset_prep| is to find an offset index~|k| to associate with
each cubic, such that the direction $d(t)$ of the cubic satisfies
$$d_{k-1}\preceq d(t)\preceq d_k\qquad\hbox{for $0\le t\le 1$.}\eqno(*)$$
We may have to split a cubic into many pieces before each
piece corresponds to a unique offset.

@<Split the cubic between |p| and |q|, if necessary, into cubics...@>=
mp_info(p)=zero_off+k_needed;
k_needed=0;
@<Prepare for derivative computations;
  |goto not_found| if the current cubic is dead@>;
@<Find the initial direction |(dx,dy)|@>;
@<Update |mp_info(p)| and find the offset $w_k$ such that
  $d_{k-1}\preceq(\\{dx},\\{dy})\prec d_k$; also advance |w0| for
  the direction change at |p|@>;
@<Find the final direction |(dxin,dyin)|@>;
@<Decide on the net change in pen offsets and set |turn_amt|@>;
@<Complete the offset splitting process@>;
w0=mp_pen_walk(mp, w0,turn_amt)

@ @<Declarations@>=
static pointer mp_pen_walk (MP mp,pointer w, integer k) ;

@ @c
pointer mp_pen_walk (MP mp,pointer w, integer k) {
  /* walk |k| steps around a pen from |w| */
  while ( k>0 ) { w=mp_link(w); decr(k);  };
  while ( k<0 ) { w=knil(w); incr(k);  };
  return w;
}

@ The direction of a cubic $B(z_0,z_1,z_2,z_3;t)=\bigl(x(t),y(t)\bigr)$ can be
calculated from the quadratic polynomials
${1\over3}x'(t)=B(x_1-x_0,x_2-x_1,x_3-x_2;t)$ and
${1\over3}y'(t)=B(y_1-y_0,y_2-y_1,y_3-y_2;t)$.
Since we may be calculating directions from several cubics
split from the current one, it is desirable to do these calculations
without losing too much precision. ``Scaled up'' values of the
derivatives, which will be less tainted by accumulated errors than
derivatives found from the cubics themselves, are maintained in
local variables |x0|, |x1|, and |x2|, representing $X_0=2^l(x_1-x_0)$,
$X_1=2^l(x_2-x_1)$, and $X_2=2^l(x_3-x_2)$; similarly |y0|, |y1|, and~|y2|
represent $Y_0=2^l(y_1-y_0)$, $Y_1=2^l(y_2-y_1)$, and $Y_2=2^l(y_3-y_2)$.

@<Other local variables for |offset_prep|@>=
integer x0,x1,x2,y0,y1,y2; /* representatives of derivatives */
integer t0,t1,t2; /* coefficients of polynomial for slope testing */
integer du,dv,dx,dy; /* for directions of the pen and the curve */
integer dx0,dy0; /* initial direction for the first cubic in the curve */
integer max_coef; /* used while scaling */
integer x0a,x1a,x2a,y0a,y1a,y2a; /* intermediate values */
fraction t; /* where the derivative passes through zero */
fraction s; /* a temporary value */

@ @<Prepare for derivative computations...@>=
x0=mp_right_x(p)-mp_x_coord(p);
x2=mp_x_coord(q)-mp_left_x(q);
x1=mp_left_x(q)-mp_right_x(p);
y0=mp_right_y(p)-mp_y_coord(p); y2=mp_y_coord(q)-mp_left_y(q);
y1=mp_left_y(q)-mp_right_y(p);
max_coef=abs(x0);
if ( abs(x1)>max_coef ) max_coef=abs(x1);
if ( abs(x2)>max_coef ) max_coef=abs(x2);
if ( abs(y0)>max_coef ) max_coef=abs(y0);
if ( abs(y1)>max_coef ) max_coef=abs(y1);
if ( abs(y2)>max_coef ) max_coef=abs(y2);
if ( max_coef==0 ) goto NOT_FOUND;
while ( max_coef<fraction_half ) {
  double(max_coef);
  double(x0); double(x1); double(x2);
  double(y0); double(y1); double(y2);
}

@ Let us first solve a special case of the problem: Suppose we
know an index~$k$ such that either (i)~$d(t)\succeq d_{k-1}$ for all~$t$
and $d(0)\prec d_k$, or (ii)~$d(t)\preceq d_k$ for all~$t$ and
$d(0)\succ d_{k-1}$.
Then, in a sense, we're halfway done, since one of the two relations
in $(*)$ is satisfied, and the other couldn't be satisfied for
any other value of~|k|.

Actually, the conditions can be relaxed somewhat since a relation such as
$d(t)\succeq d_{k-1}$ restricts $d(t)$ to a half plane when all that really
matters is whether $d(t)$ crosses the ray in the $d_{k-1}$ direction from
the origin.  The condition for case~(i) becomes $d_{k-1}\preceq d(0)\prec d_k$
and $d(t)$ never crosses the $d_{k-1}$ ray in the clockwise direction.
Case~(ii) is similar except $d(t)$ cannot cross the $d_k$ ray in the
counterclockwise direction.

The |fin_offset_prep| subroutine solves the stated subproblem.
It has a parameter called |rise| that is |1| in
case~(i), |-1| in case~(ii). Parameters |x0| through |y2| represent
the derivative of the cubic following |p|.
The |w| parameter should point to offset~$w_k$ and |mp_info(p)| should already
be set properly.  The |turn_amt| parameter gives the absolute value of the
overall net change in pen offsets.

@<Declarations@>=
static void mp_fin_offset_prep (MP mp,pointer p, pointer w, integer 
  x0,integer x1, integer x2, integer y0, integer y1, integer y2, 
  integer rise, integer turn_amt) ;

@ @c
void mp_fin_offset_prep (MP mp,pointer p, pointer w, integer 
  x0,integer x1, integer x2, integer y0, integer y1, integer y2, 
  integer rise, integer turn_amt)  {
  pointer ww; /* for list manipulation */
  scaled du,dv; /* for slope calculation */
  integer t0,t1,t2; /* test coefficients */
  fraction t; /* place where the derivative passes a critical slope */
  fraction s; /* slope or reciprocal slope */
  integer v; /* intermediate value for updating |x0..y2| */
  pointer q; /* original |mp_link(p)| */
  q=mp_link(p);
  while (1)  { 
    if ( rise>0 ) ww=mp_link(w); /* a pointer to $w\k$ */
    else  ww=knil(w); /* a pointer to $w_{k-1}$ */
    @<Compute test coefficients |(t0,t1,t2)|
      for $d(t)$ versus $d_k$ or $d_{k-1}$@>;
    t=mp_crossing_point(mp, t0,t1,t2);
    if ( t>=fraction_one ) {
      if ( turn_amt>0 ) t=fraction_one;  else return;
    }
    @<Split the cubic at $t$,
      and split off another cubic if the derivative crosses back@>;
    w=ww;
  }
}

@ We want $B(\\{t0},\\{t1},\\{t2};t)$ to be the dot product of $d(t)$ with a
$-90^\circ$ rotation of the vector from |w| to |ww|.  This makes the resulting
function cross from positive to negative when $d_{k-1}\preceq d(t)\preceq d_k$
begins to fail.

@<Compute test coefficients |(t0,t1,t2)| for $d(t)$ versus...@>=
du=mp_x_coord(ww)-mp_x_coord(w); dv=mp_y_coord(ww)-mp_y_coord(w);
if ( abs(du)>=abs(dv) ) {
  s=mp_make_fraction(mp, dv,du);
  t0=mp_take_fraction(mp, x0,s)-y0;
  t1=mp_take_fraction(mp, x1,s)-y1;
  t2=mp_take_fraction(mp, x2,s)-y2;
  if ( du<0 ) { negate(t0); negate(t1); negate(t2);  }
} else { 
  s=mp_make_fraction(mp, du,dv);
  t0=x0-mp_take_fraction(mp, y0,s);
  t1=x1-mp_take_fraction(mp, y1,s);
  t2=x2-mp_take_fraction(mp, y2,s);
  if ( dv<0 ) { negate(t0); negate(t1); negate(t2);  }
}
if ( t0<0 ) t0=0 /* should be positive without rounding error */

@ The curve has crossed $d_k$ or $d_{k-1}$; its initial segment satisfies
$(*)$, and it might cross again and return towards $s_{k-1}$ or $s_k$,
respectively, yielding another solution of $(*)$.

@<Split the cubic at $t$, and split off another...@>=
{ 
mp_split_cubic(mp, p,t); p=mp_link(p); mp_info(p)=zero_off+rise;
decr(turn_amt);
v=t_of_the_way(x0,x1); x1=t_of_the_way(x1,x2);
x0=t_of_the_way(v,x1);
v=t_of_the_way(y0,y1); y1=t_of_the_way(y1,y2);
y0=t_of_the_way(v,y1);
if ( turn_amt<0 ) {
  t1=t_of_the_way(t1,t2);
  if ( t1>0 ) t1=0; /* without rounding error, |t1| would be |<=0| */
  t=mp_crossing_point(mp, 0,-t1,-t2);
  if ( t>fraction_one ) t=fraction_one;
  incr(turn_amt);
  if ( (t==fraction_one)&&(mp_link(p)!=q) ) {
    mp_info(mp_link(p))=mp_info(mp_link(p))-rise;
  } else { 
    mp_split_cubic(mp, p,t); mp_info(mp_link(p))=zero_off-rise;
    v=t_of_the_way(x1,x2); x1=t_of_the_way(x0,x1);
    x2=t_of_the_way(x1,v);
    v=t_of_the_way(y1,y2); y1=t_of_the_way(y0,y1);
    y2=t_of_the_way(y1,v);
  }
}
}

@ Now we must consider the general problem of |offset_prep|, when
nothing is known about a given cubic. We start by finding its
direction in the vicinity of |t=0|.

If $z'(t)=0$, the given cubic is numerically unstable but |offset_prep|
has not yet introduced any more numerical errors.  Thus we can compute
the true initial direction for the given cubic, even if it is almost
degenerate.

@<Find the initial direction |(dx,dy)|@>=
dx=x0; dy=y0;
if ( dx==0 && dy==0 ) { 
  dx=x1; dy=y1;
  if ( dx==0 && dy==0 ) { 
    dx=x2; dy=y2;
  }
}
if ( p==c ) { dx0=dx; dy0=dy;  }

@ @<Find the final direction |(dxin,dyin)|@>=
dxin=x2; dyin=y2;
if ( dxin==0 && dyin==0 ) {
  dxin=x1; dyin=y1;
  if ( dxin==0 && dyin==0 ) {
    dxin=x0; dyin=y0;
  }
}

@ The next step is to bracket the initial direction between consecutive
edges of the pen polygon.  We must be careful to turn clockwise only if
this makes the turn less than $180^\circ$. (A $180^\circ$ turn must be
counter-clockwise in order to make \&{doublepath} envelopes come out
@:double_path_}{\&{doublepath} primitive@>
right.) This code depends on |w0| being the offset for |(dxin,dyin)|.

@<Update |mp_info(p)| and find the offset $w_k$ such that...@>=
turn_amt=mp_get_turn_amt(mp,w0,dx,dy,(mp_ab_vs_cd(mp, dy,dxin,dx,dyin)>=0));
w=mp_pen_walk(mp, w0, turn_amt);
w0=w;
mp_info(p)=mp_info(p)+turn_amt

@ Decide how many pen offsets to go away from |w| in order to find the offset
for |(dx,dy)|, going counterclockwise if |ccw| is |true|.  This assumes that
|w| is the offset for some direction $(x',y')$ from which the angle to |(dx,dy)|
in the sense determined by |ccw| is less than or equal to $180^\circ$.

If the pen polygon has only two edges, they could both be parallel
to |(dx,dy)|.  In this case, we must be careful to stop after crossing the first
such edge in order to avoid an infinite loop.

@<Declarations@>=
static integer mp_get_turn_amt (MP mp,pointer w, scaled  dx,
                         scaled dy, boolean  ccw);

@ @c
integer mp_get_turn_amt (MP mp,pointer w, scaled  dx,
                         scaled dy, boolean  ccw) {
  pointer ww; /* a neighbor of knot~|w| */
  integer s; /* turn amount so far */
  integer t; /* |ab_vs_cd| result */
  s=0;
  if ( ccw ) { 
    ww=mp_link(w);
    do {  
      t=mp_ab_vs_cd(mp, dy,(mp_x_coord(ww)-mp_x_coord(w)),
                        dx,(mp_y_coord(ww)-mp_y_coord(w)));
      if ( t<0 ) break;
      incr(s);
      w=ww; ww=mp_link(ww);
    } while (t>0);
  } else { 
    ww=knil(w);
    while ( mp_ab_vs_cd(mp, dy,(mp_x_coord(w)-mp_x_coord(ww)),
                            dx,(mp_y_coord(w)-mp_y_coord(ww))) < 0) { 
      decr(s);
      w=ww; ww=knil(ww);
    }
  }
  return s;
}

@ When we're all done, the final offset is |w0| and the final curve direction
is |(dxin,dyin)|.  With this knowledge of the incoming direction at |c|, we
can correct |mp_info(c)| which was erroneously based on an incoming offset
of~|h|.

@d fix_by(A) mp_info(c)=mp_info(c)+(A)

@<Fix the offset change in |mp_info(c)| and set |c| to the return value of...@>=
mp->spec_offset=mp_info(c)-zero_off;
if ( mp_link(c)==c ) {
  mp_info(c)=zero_off+n;
} else { 
  fix_by(k_needed);
  while ( w0!=h ) { fix_by(1); w0=mp_link(w0);  };
  while ( mp_info(c)<=zero_off-n ) fix_by(n);
  while ( mp_info(c)>zero_off ) fix_by(-n);
  if ( (mp_info(c)!=zero_off)&&(mp_ab_vs_cd(mp, dy0,dxin,dx0,dyin)>=0) ) fix_by(n);
}

@ Finally we want to reduce the general problem to situations that
|fin_offset_prep| can handle. We split the cubic into at most three parts
with respect to $d_{k-1}$, and apply |fin_offset_prep| to each part.

@<Complete the offset splitting process@>=
ww=knil(w);
@<Compute test coeff...@>;
@<Find the first |t| where $d(t)$ crosses $d_{k-1}$ or set
  |t:=fraction_one+1|@>;
if ( t>fraction_one ) {
  mp_fin_offset_prep(mp, p,w,x0,x1,x2,y0,y1,y2,1,turn_amt);
} else {
  mp_split_cubic(mp, p,t); r=mp_link(p);
  x1a=t_of_the_way(x0,x1); x1=t_of_the_way(x1,x2);
  x2a=t_of_the_way(x1a,x1);
  y1a=t_of_the_way(y0,y1); y1=t_of_the_way(y1,y2);
  y2a=t_of_the_way(y1a,y1);
  mp_fin_offset_prep(mp, p,w,x0,x1a,x2a,y0,y1a,y2a,1,0); x0=x2a; y0=y2a;
  mp_info(r)=zero_off-1;
  if ( turn_amt>=0 ) {
    t1=t_of_the_way(t1,t2);
    if ( t1>0 ) t1=0;
    t=mp_crossing_point(mp, 0,-t1,-t2);
    if ( t>fraction_one ) t=fraction_one;
    @<Split off another rising cubic for |fin_offset_prep|@>;
    mp_fin_offset_prep(mp, r,ww,x0,x1,x2,y0,y1,y2,-1,0);
  } else {
    mp_fin_offset_prep(mp, r,ww,x0,x1,x2,y0,y1,y2,-1,(-1-turn_amt));
  }
}

@ @<Split off another rising cubic for |fin_offset_prep|@>=
mp_split_cubic(mp, r,t); mp_info(mp_link(r))=zero_off+1;
x1a=t_of_the_way(x1,x2); x1=t_of_the_way(x0,x1);
x0a=t_of_the_way(x1,x1a);
y1a=t_of_the_way(y1,y2); y1=t_of_the_way(y0,y1);
y0a=t_of_the_way(y1,y1a);
mp_fin_offset_prep(mp, mp_link(r),w,x0a,x1a,x2,y0a,y1a,y2,1,turn_amt);
x2=x0a; y2=y0a

@ At this point, the direction of the incoming pen edge is |(-du,-dv)|.
When the component of $d(t)$ perpendicular to |(-du,-dv)| crosses zero, we
need to decide whether the directions are parallel or antiparallel.  We
can test this by finding the dot product of $d(t)$ and |(-du,-dv)|, but this
should be avoided when the value of |turn_amt| already determines the
answer.  If |t2<0|, there is one crossing and it is antiparallel only if
|turn_amt>=0|.  If |turn_amt<0|, there should always be at least one
crossing and the first crossing cannot be antiparallel.

@<Find the first |t| where $d(t)$ crosses $d_{k-1}$ or set...@>=
t=mp_crossing_point(mp, t0,t1,t2);
if ( turn_amt>=0 ) {
  if ( t2<0 ) {
    t=fraction_one+1;
  } else { 
    u0=t_of_the_way(x0,x1);
    u1=t_of_the_way(x1,x2);
    ss=mp_take_fraction(mp, -du,t_of_the_way(u0,u1));
    v0=t_of_the_way(y0,y1);
    v1=t_of_the_way(y1,y2);
    ss=ss+mp_take_fraction(mp, -dv,t_of_the_way(v0,v1));
    if ( ss<0 ) t=fraction_one+1;
  }
} else if ( t>fraction_one ) {
  t=fraction_one;
}

@ @<Other local variables for |offset_prep|@>=
integer u0,u1,v0,v1; /* intermediate values for $d(t)$ calculation */
integer ss = 0; /* the part of the dot product computed so far */
int d_sign; /* sign of overall change in direction for this cubic */

@ If the cubic almost has a cusp, it is a numerically ill-conditioned
problem to decide which way it loops around but that's OK as long we're
consistent.  To make \&{doublepath} envelopes work properly, reversing
the path should always change the sign of |turn_amt|.

@<Decide on the net change in pen offsets and set |turn_amt|@>=
d_sign=mp_ab_vs_cd(mp, dx,dyin, dxin,dy);
if ( d_sign==0 ) {
  @<Check rotation direction based on node position@>
}
if ( d_sign==0 ) {
  if ( dx==0 ) {
    if ( dy>0 ) d_sign=1;  else d_sign=-1;
  } else {
    if ( dx>0 ) d_sign=1;  else d_sign=-1; 
  }
}
@<Make |ss| negative if and only if the total change in direction is
  more than $180^\circ$@>;
turn_amt=mp_get_turn_amt(mp, w, dxin, dyin, (d_sign>0));
if ( ss<0 ) turn_amt=turn_amt-d_sign*n

@ We check rotation direction by looking at the vector connecting the current
node with the next. If its angle with incoming and outgoing tangents has the
same sign, we pick this as |d_sign|, since it means we have a flex, not a cusp.
Otherwise we proceed to the cusp code.

@<Check rotation direction based on node position@>=
u0=mp_x_coord(q)-mp_x_coord(p);
u1=mp_y_coord(q)-mp_y_coord(p);
d_sign = half(mp_ab_vs_cd(mp, dx, u1, u0, dy)+
  mp_ab_vs_cd(mp, u0, dyin, dxin, u1));

@ In order to be invariant under path reversal, the result of this computation
should not change when |x0|, |y0|, $\ldots$ are all negated and |(x0,y0)| is
then swapped with |(x2,y2)|.  We make use of the identities
|take_fraction(-a,-b)=take_fraction(a,b)| and
|t_of_the_way(-a,-b)=-(t_of_the_way(a,b))|.

@<Make |ss| negative if and only if the total change in direction is...@>=
t0=half(mp_take_fraction(mp, x0,y2))-half(mp_take_fraction(mp, x2,y0));
t1=half(mp_take_fraction(mp, x1,(y0+y2)))-half(mp_take_fraction(mp, y1,(x0+x2)));
if ( t0==0 ) t0=d_sign; /* path reversal always negates |d_sign| */
if ( t0>0 ) {
  t=mp_crossing_point(mp, t0,t1,-t0);
  u0=t_of_the_way(x0,x1);
  u1=t_of_the_way(x1,x2);
  v0=t_of_the_way(y0,y1);
  v1=t_of_the_way(y1,y2);
} else { 
  t=mp_crossing_point(mp, -t0,t1,t0);
  u0=t_of_the_way(x2,x1);
  u1=t_of_the_way(x1,x0);
  v0=t_of_the_way(y2,y1);
  v1=t_of_the_way(y1,y0);
}
ss=mp_take_fraction(mp, (x0+x2),t_of_the_way(u0,u1))+
   mp_take_fraction(mp, (y0+y2),t_of_the_way(v0,v1))

@ Here's a routine that prints an envelope spec in symbolic form.  It assumes
that the |cur_pen| has not been walked around to the first offset.

@c 
static void mp_print_spec (MP mp,pointer cur_spec, pointer cur_pen, const char *s) {
  pointer p,q; /* list traversal */
  pointer w; /* the current pen offset */
  mp_print_diagnostic(mp, "Envelope spec",s,true);
  p=cur_spec; w=mp_pen_walk(mp, cur_pen,mp->spec_offset);
  mp_print_ln(mp);
  mp_print_two(mp, mp_x_coord(cur_spec),mp_y_coord(cur_spec));
  mp_print(mp, " % beginning with offset ");
  mp_print_two(mp, mp_x_coord(w),mp_y_coord(w));
  do { 
    while (1) {  
      q=mp_link(p);
      @<Print the cubic between |p| and |q|@>;
      p=q;
	  if ((p==cur_spec) || (mp_info(p)!=zero_off)) 
    	break;
    }
    if ( mp_info(p)!=zero_off ) {
      @<Update |w| as indicated by |mp_info(p)| and print an explanation@>;
    }
  } while (p!=cur_spec);
  mp_print_nl(mp, " & cycle");
  mp_end_diagnostic(mp, true);
}

@ @<Update |w| as indicated by |mp_info(p)| and print an explanation@>=
{ 
  w=mp_pen_walk(mp, w, (mp_info(p)-zero_off));
  mp_print(mp, " % ");
  if ( mp_info(p)>zero_off ) mp_print(mp, "counter");
  mp_print(mp, "clockwise to offset ");
  mp_print_two(mp, mp_x_coord(w),mp_y_coord(w));
}

@ @<Print the cubic between |p| and |q|@>=
{ 
  mp_print_nl(mp, "   ..controls ");
  mp_print_two(mp, mp_right_x(p),mp_right_y(p));
  mp_print(mp, " and ");
  mp_print_two(mp, mp_left_x(q),mp_left_y(q));
  mp_print_nl(mp, " ..");
  mp_print_two(mp, mp_x_coord(q),mp_y_coord(q));
}

@ Once we have an envelope spec, the remaining task to construct the actual
envelope by offsetting each cubic as determined by the |info| fields in
the knots.  First we use |offset_prep| to convert the |c| into an envelope
spec. Then we add the offsets so that |c| becomes a cyclic path that represents
the envelope.

The |ljoin| and |miterlim| parameters control the treatment of points where the
pen offset changes, and |lcap| controls the endpoints of a \&{doublepath}.
The endpoints are easily located because |c| is given in undoubled form
and then doubled in this procedure.  We use |spec_p1| and |spec_p2| to keep
track of the endpoints and treat them like very sharp corners.
Butt end caps are treated like beveled joins; round end caps are treated like
round joins; and square end caps are achieved by setting |join_type:=3|.

None of these parameters apply to inside joins where the convolution tracing
has retrograde lines.  In such cases we use a simple connect-the-endpoints
approach that is achieved by setting |join_type:=2|.

@c
static pointer mp_make_envelope (MP mp,pointer c, pointer h, quarterword ljoin,
  quarterword lcap, scaled miterlim) {
  pointer p,q,r,q0; /* for manipulating the path */
  int join_type=0; /* codes |0..3| for mitered, round, beveled, or square */
  pointer w,w0; /* the pen knot for the current offset */
  scaled qx,qy; /* unshifted coordinates of |q| */
  halfword k,k0; /* controls pen edge insertion */
  @<Other local variables for |make_envelope|@>;
  dxin=0; dyin=0; dxout=0; dyout=0;
  mp->spec_p1=null; mp->spec_p2=null;
  @<If endpoint, double the path |c|, and set |spec_p1| and |spec_p2|@>;
  @<Use |offset_prep| to compute the envelope spec then walk |h| around to
    the initial offset@>;
  w=h;
  p=c;
  do {  
    q=mp_link(p); q0=q;
    qx=mp_x_coord(q); qy=mp_y_coord(q);
    k=mp_info(q);
    k0=k; w0=w;
    if ( k!=zero_off ) {
      @<Set |join_type| to indicate how to handle offset changes at~|q|@>;
    }
    @<Add offset |w| to the cubic from |p| to |q|@>;
    while ( k!=zero_off ) { 
      @<Step |w| and move |k| one step closer to |zero_off|@>;
      if ( (join_type==1)||(k==zero_off) )
         q=mp_insert_knot(mp, q,qx+mp_x_coord(w),qy+mp_y_coord(w));
    };
    if ( q!=mp_link(p) ) {
      @<Set |p=mp_link(p)| and add knots between |p| and |q| as
        required by |join_type|@>;
    }
    p=q;
  } while (q0!=c);
  return c;
}

@ @<Use |offset_prep| to compute the envelope spec then walk |h| around to...@>=
c=mp_offset_prep(mp, c,h);
if ( mp->internal[mp_tracing_specs]>0 ) 
  mp_print_spec(mp, c,h,"");
h=mp_pen_walk(mp, h,mp->spec_offset)

@ Mitered and squared-off joins depend on path directions that are difficult to
compute for degenerate cubics.  The envelope spec computed by |offset_prep| can
have degenerate cubics only if the entire cycle collapses to a single
degenerate cubic.  Setting |join_type:=2| in this case makes the computed
envelope degenerate as well.

@<Set |join_type| to indicate how to handle offset changes at~|q|@>=
if ( k<zero_off ) {
  join_type=2;
} else {
  if ( (q!=mp->spec_p1)&&(q!=mp->spec_p2) ) join_type=ljoin;
  else if ( lcap==2 ) join_type=3;
  else join_type=2-lcap;
  if ( (join_type==0)||(join_type==3) ) {
    @<Set the incoming and outgoing directions at |q|; in case of
      degeneracy set |join_type:=2|@>;
    if ( join_type==0 ) {
      @<If |miterlim| is less than the secant of half the angle at |q|
        then set |join_type:=2|@>;
    }
  }
}

@ @<If |miterlim| is less than the secant of half the angle at |q|...@>=
{ 
  tmp=mp_take_fraction(mp, miterlim,fraction_half+
      half(mp_take_fraction(mp, dxin,dxout)+mp_take_fraction(mp, dyin,dyout)));
  if ( tmp<unity )
    if ( mp_take_scaled(mp, miterlim,tmp)<unity ) join_type=2;
}

@ @<Other local variables for |make_envelope|@>=
fraction dxin,dyin,dxout,dyout; /* directions at |q| when square or mitered */
scaled tmp; /* a temporary value */

@ The coordinates of |p| have already been shifted unless |p| is the first
knot in which case they get shifted at the very end.

@<Add offset |w| to the cubic from |p| to |q|@>=
mp_right_x(p)=mp_right_x(p)+mp_x_coord(w);
mp_right_y(p)=mp_right_y(p)+mp_y_coord(w);
mp_left_x(q)=mp_left_x(q)+mp_x_coord(w);
mp_left_y(q)=mp_left_y(q)+mp_y_coord(w);
mp_x_coord(q)=mp_x_coord(q)+mp_x_coord(w);
mp_y_coord(q)=mp_y_coord(q)+mp_y_coord(w);
mp_left_type(q)=mp_explicit;
mp_right_type(q)=mp_explicit

@ @<Step |w| and move |k| one step closer to |zero_off|@>=
if ( k>zero_off ){ w=mp_link(w); decr(k);  }
else { w=knil(w); incr(k);  }

@ The cubic from |q| to the new knot at |(x,y)| becomes a line segment and
the |mp_right_x| and |mp_right_y| fields of |r| are set from |q|.  This is done in
case the cubic containing these control points is ``yet to be examined.''

@<Declarations@>=
static pointer mp_insert_knot (MP mp,pointer q, scaled x, scaled y);

@ @c
pointer mp_insert_knot (MP mp,pointer q, scaled x, scaled y) {
  /* returns the inserted knot */
  pointer r; /* the new knot */
  r=mp_get_node(mp, knot_node_size);
  mp_link(r)=mp_link(q); mp_link(q)=r;
  mp_right_x(r)=mp_right_x(q);
  mp_right_y(r)=mp_right_y(q);
  mp_x_coord(r)=x;
  mp_y_coord(r)=y;
  mp_right_x(q)=mp_x_coord(q);
  mp_right_y(q)=mp_y_coord(q);
  mp_left_x(r)=mp_x_coord(r);
  mp_left_y(r)=mp_y_coord(r);
  mp_left_type(r)=mp_explicit;
  mp_right_type(r)=mp_explicit;
  mp_originator(r)=mp_program_code;
  return r;
}

@ After setting |p:=mp_link(p)|, either |join_type=1| or |q=mp_link(p)|.

@<Set |p=mp_link(p)| and add knots between |p| and |q| as...@>=
{ 
  p=mp_link(p);
  if ( (join_type==0)||(join_type==3) ) {
    if ( join_type==0 ) {
      @<Insert a new knot |r| between |p| and |q| as required for a mitered join@>
    } else {
      @<Make |r| the last of two knots inserted between |p| and |q| to form a
        squared join@>;
    }
    if ( r!=null ) { 
      mp_right_x(r)=mp_x_coord(r);
      mp_right_y(r)=mp_y_coord(r);
    }
  }
}

@ For very small angles, adding a knot is unnecessary and would cause numerical
problems, so we just set |r:=null| in that case.

@<Insert a new knot |r| between |p| and |q| as required for a mitered join@>=
{ 
  det=mp_take_fraction(mp, dyout,dxin)-mp_take_fraction(mp, dxout,dyin);
  if ( abs(det)<26844 ) { 
     r=null; /* sine $<10^{-4}$ */
  } else { 
    tmp=mp_take_fraction(mp, mp_x_coord(q)-mp_x_coord(p),dyout)-
        mp_take_fraction(mp, mp_y_coord(q)-mp_y_coord(p),dxout);
    tmp=mp_make_fraction(mp, tmp,det);
    r=mp_insert_knot(mp, p,mp_x_coord(p)+mp_take_fraction(mp, tmp,dxin),
      mp_y_coord(p)+mp_take_fraction(mp, tmp,dyin));
  }
}

@ @<Other local variables for |make_envelope|@>=
fraction det; /* a determinant used for mitered join calculations */

@ @<Make |r| the last of two knots inserted between |p| and |q| to form a...@>=
{ 
  ht_x=mp_y_coord(w)-mp_y_coord(w0);
  ht_y=mp_x_coord(w0)-mp_x_coord(w);
  while ( (abs(ht_x)<fraction_half)&&(abs(ht_y)<fraction_half) ) { 
    ht_x+=ht_x; ht_y+=ht_y;
  }
  @<Scan the pen polygon between |w0| and |w| and make |max_ht| the range dot
    product with |(ht_x,ht_y)|@>;
  tmp=mp_make_fraction(mp, max_ht,mp_take_fraction(mp, dxin,ht_x)+
                                  mp_take_fraction(mp, dyin,ht_y));
  r=mp_insert_knot(mp, p,mp_x_coord(p)+mp_take_fraction(mp, tmp,dxin),
                         mp_y_coord(p)+mp_take_fraction(mp, tmp,dyin));
  tmp=mp_make_fraction(mp, max_ht,mp_take_fraction(mp, dxout,ht_x)+
                                  mp_take_fraction(mp, dyout,ht_y));
  r=mp_insert_knot(mp, r,mp_x_coord(q)+mp_take_fraction(mp, tmp,dxout),
                         mp_y_coord(q)+mp_take_fraction(mp, tmp,dyout));
}

@ @<Other local variables for |make_envelope|@>=
fraction ht_x,ht_y; /* perpendicular to the segment from |p| to |q| */
scaled max_ht; /* maximum height of the pen polygon above the |w0|-|w| line */
halfword kk; /* keeps track of the pen vertices being scanned */
pointer ww; /* the pen vertex being tested */

@ The dot product of the vector from |w0| to |ww| with |(ht_x,ht_y)| ranges
from zero to |max_ht|.

@<Scan the pen polygon between |w0| and |w| and make |max_ht| the range...@>=
max_ht=0;
kk=zero_off;
ww=w;
while (1)  { 
  @<Step |ww| and move |kk| one step closer to |k0|@>;
  if ( kk==k0 ) break;
  tmp=mp_take_fraction(mp, (mp_x_coord(ww)-mp_x_coord(w0)),ht_x)+
      mp_take_fraction(mp, (mp_y_coord(ww)-mp_y_coord(w0)),ht_y);
  if ( tmp>max_ht ) max_ht=tmp;
}


@ @<Step |ww| and move |kk| one step closer to |k0|@>=
if ( kk>k0 ) { ww=mp_link(ww); decr(kk);  }
else { ww=knil(ww); incr(kk);  }

@ @<If endpoint, double the path |c|, and set |spec_p1| and |spec_p2|@>=
if ( mp_left_type(c)==mp_endpoint ) { 
  mp->spec_p1=mp_htap_ypoc(mp, c);
  mp->spec_p2=mp->path_tail;
  mp_originator(mp->spec_p1)=mp_program_code;
  mp_link(mp->spec_p2)=mp_link(mp->spec_p1);
  mp_link(mp->spec_p1)=c;
  mp_remove_cubic(mp, mp->spec_p1);
  c=mp->spec_p1;
  if ( c!=mp_link(c) ) {
    mp_originator(mp->spec_p2)=mp_program_code;
    mp_remove_cubic(mp, mp->spec_p2);
  } else {
    @<Make |c| look like a cycle of length one@>;
  }
}

@ @<Make |c| look like a cycle of length one@>=
{ 
  mp_left_type(c)=mp_explicit; mp_right_type(c)=mp_explicit;
  mp_left_x(c)=mp_x_coord(c); mp_left_y(c)=mp_y_coord(c);
  mp_right_x(c)=mp_x_coord(c); mp_right_y(c)=mp_y_coord(c);
}

@ In degenerate situations we might have to look at the knot preceding~|q|.
That knot is |p| but if |p<>c|, its coordinates have already been offset by |w|.

@<Set the incoming and outgoing directions at |q|; in case of...@>=
dxin=mp_x_coord(q)-mp_left_x(q);
dyin=mp_y_coord(q)-mp_left_y(q);
if ( (dxin==0)&&(dyin==0) ) {
  dxin=mp_x_coord(q)-mp_right_x(p);
  dyin=mp_y_coord(q)-mp_right_y(p);
  if ( (dxin==0)&&(dyin==0) ) {
    dxin=mp_x_coord(q)-mp_x_coord(p);
    dyin=mp_y_coord(q)-mp_y_coord(p);
    if ( p!=c ) { /* the coordinates of |p| have been offset by |w| */
      dxin=dxin+mp_x_coord(w);
      dyin=dyin+mp_y_coord(w);
    }
  }
}
tmp=mp_pyth_add(mp, dxin,dyin);
if ( tmp==0 ) {
  join_type=2;
} else { 
  dxin=mp_make_fraction(mp, dxin,tmp);
  dyin=mp_make_fraction(mp, dyin,tmp);
  @<Set the outgoing direction at |q|@>;
}

@ If |q=c| then the coordinates of |r| and the control points between |q|
and~|r| have already been offset by |h|.

@<Set the outgoing direction at |q|@>=
dxout=mp_right_x(q)-mp_x_coord(q);
dyout=mp_right_y(q)-mp_y_coord(q);
if ( (dxout==0)&&(dyout==0) ) {
  r=mp_link(q);
  dxout=mp_left_x(r)-mp_x_coord(q);
  dyout=mp_left_y(r)-mp_y_coord(q);
  if ( (dxout==0)&&(dyout==0) ) {
    dxout=mp_x_coord(r)-mp_x_coord(q);
    dyout=mp_y_coord(r)-mp_y_coord(q);
  }
}
if ( q==c ) {
  dxout=dxout-mp_x_coord(h);
  dyout=dyout-mp_y_coord(h);
}
tmp=mp_pyth_add(mp, dxout,dyout);
if ( tmp==0 ) mp_confusion(mp, "degenerate spec");
@:this can't happen degerate spec}{\quad degenerate spec@>
dxout=mp_make_fraction(mp, dxout,tmp);
dyout=mp_make_fraction(mp, dyout,tmp)

@* \[23] Direction and intersection times.
A path of length $n$ is defined parametrically by functions $x(t)$ and
$y(t)$, for |0<=t<=n|; we can regard $t$ as the ``time'' at which the path
reaches the point $\bigl(x(t),y(t)\bigr)$.  In this section of the program
we shall consider operations that determine special times associated with
given paths: the first time that a path travels in a given direction, and
a pair of times at which two paths cross each other.

@ Let's start with the easier task. The function |find_direction_time| is
given a direction |(x,y)| and a path starting at~|h|. If the path never
travels in direction |(x,y)|, the direction time will be~|-1|; otherwise
it will be nonnegative.

Certain anomalous cases can arise: If |(x,y)=(0,0)|, so that the given
direction is undefined, the direction time will be~0. If $\bigl(x'(t),
y'(t)\bigr)=(0,0)$, so that the path direction is undefined, it will be
assumed to match any given direction at time~|t|.

The routine solves this problem in nondegenerate cases by rotating the path
and the given direction so that |(x,y)=(1,0)|; i.e., the main task will be
to find when a given path first travels ``due east.''

@c 
static scaled mp_find_direction_time (MP mp,scaled x, scaled y, pointer h) {
  scaled max; /* $\max\bigl(\vert x\vert,\vert y\vert\bigr)$ */
  pointer p,q; /* for list traversal */
  scaled n; /* the direction time at knot |p| */
  scaled tt; /* the direction time within a cubic */
  @<Other local variables for |find_direction_time|@>;
  @<Normalize the given direction for better accuracy;
    but |return| with zero result if it's zero@>;
  n=0; p=h; phi=0;
  while (1) { 
    if ( mp_right_type(p)==mp_endpoint ) break;
    q=mp_link(p);
    @<Rotate the cubic between |p| and |q|; then
      |goto found| if the rotated cubic travels due east at some time |tt|;
      but |break| if an entire cyclic path has been traversed@>;
    p=q; n=n+unity;
  }
  return (-unity);
FOUND: 
  return (n+tt);
}

@ @<Normalize the given direction for better accuracy...@>=
if ( abs(x)<abs(y) ) { 
  x=mp_make_fraction(mp, x,abs(y));
  if ( y>0 ) y=fraction_one; else y=-fraction_one;
} else if ( x==0 ) { 
  return 0;
} else  { 
  y=mp_make_fraction(mp, y,abs(x));
  if ( x>0 ) x=fraction_one; else x=-fraction_one;
}

@ Since we're interested in the tangent directions, we work with the
derivative $${1\over3}B'(x_0,x_1,x_2,x_3;t)=
B(x_1-x_0,x_2-x_1,x_3-x_2;t)$$ instead of
$B(x_0,x_1,x_2,x_3;t)$ itself. The derived coefficients are also scaled up
in order to achieve better accuracy.

The given path may turn abruptly at a knot, and it might pass the critical
tangent direction at such a time. Therefore we remember the direction |phi|
in which the previous rotated cubic was traveling. (The value of |phi| will be
undefined on the first cubic, i.e., when |n=0|.)

@<Rotate the cubic between |p| and |q|; then...@>=
tt=0;
@<Set local variables |x1,x2,x3| and |y1,y2,y3| to multiples of the control
  points of the rotated derivatives@>;
if ( y1==0 ) if ( x1>=0 ) goto FOUND;
if ( n>0 ) { 
  @<Exit to |found| if an eastward direction occurs at knot |p|@>;
  if ( p==h ) break;
  };
if ( (x3!=0)||(y3!=0) ) phi=mp_n_arg(mp, x3,y3);
@<Exit to |found| if the curve whose derivatives are specified by
  |x1,x2,x3,y1,y2,y3| travels eastward at some time~|tt|@>

@ @<Other local variables for |find_direction_time|@>=
scaled x1,x2,x3,y1,y2,y3;  /* multiples of rotated derivatives */
angle theta,phi; /* angles of exit and entry at a knot */
fraction t; /* temp storage */

@ @<Set local variables |x1,x2,x3| and |y1,y2,y3| to multiples...@>=
x1=mp_right_x(p)-mp_x_coord(p); x2=mp_left_x(q)-mp_right_x(p);
x3=mp_x_coord(q)-mp_left_x(q);
y1=mp_right_y(p)-mp_y_coord(p); y2=mp_left_y(q)-mp_right_y(p);
y3=mp_y_coord(q)-mp_left_y(q);
max=abs(x1);
if ( abs(x2)>max ) max=abs(x2);
if ( abs(x3)>max ) max=abs(x3);
if ( abs(y1)>max ) max=abs(y1);
if ( abs(y2)>max ) max=abs(y2);
if ( abs(y3)>max ) max=abs(y3);
if ( max==0 ) goto FOUND;
while ( max<fraction_half ){ 
  max+=max; x1+=x1; x2+=x2; x3+=x3;
  y1+=y1; y2+=y2; y3+=y3;
}
t=x1; x1=mp_take_fraction(mp, x1,x)+mp_take_fraction(mp, y1,y);
y1=mp_take_fraction(mp, y1,x)-mp_take_fraction(mp, t,y);
t=x2; x2=mp_take_fraction(mp, x2,x)+mp_take_fraction(mp, y2,y);
y2=mp_take_fraction(mp, y2,x)-mp_take_fraction(mp, t,y);
t=x3; x3=mp_take_fraction(mp, x3,x)+mp_take_fraction(mp, y3,y);
y3=mp_take_fraction(mp, y3,x)-mp_take_fraction(mp, t,y)

@ @<Exit to |found| if an eastward direction occurs at knot |p|@>=
theta=mp_n_arg(mp, x1,y1);
if ( theta>=0 ) if ( phi<=0 ) if ( phi>=theta-one_eighty_deg ) goto FOUND;
if ( theta<=0 ) if ( phi>=0 ) if ( phi<=theta+one_eighty_deg ) goto FOUND

@ In this step we want to use the |crossing_point| routine to find the
roots of the quadratic equation $B(y_1,y_2,y_3;t)=0$.
Several complications arise: If the quadratic equation has a double root,
the curve never crosses zero, and |crossing_point| will find nothing;
this case occurs iff $y_1y_3=y_2^2$ and $y_1y_2<0$. If the quadratic
equation has simple roots, or only one root, we may have to negate it
so that $B(y_1,y_2,y_3;t)$ crosses from positive to negative at its first root.
And finally, we need to do special things if $B(y_1,y_2,y_3;t)$ is
identically zero.

@ @<Exit to |found| if the curve whose derivatives are specified by...@>=
if ( x1<0 ) if ( x2<0 ) if ( x3<0 ) goto DONE;
if ( mp_ab_vs_cd(mp, y1,y3,y2,y2)==0 ) {
  @<Handle the test for eastward directions when $y_1y_3=y_2^2$;
    either |goto found| or |goto done|@>;
}
if ( y1<=0 ) {
  if ( y1<0 ) { y1=-y1; y2=-y2; y3=-y3; }
  else if ( y2>0 ){ y2=-y2; y3=-y3; };
}
@<Check the places where $B(y_1,y_2,y_3;t)=0$ to see if
  $B(x_1,x_2,x_3;t)\ge0$@>;
DONE:

@ The quadratic polynomial $B(y_1,y_2,y_3;t)$ begins |>=0| and has at most
two roots, because we know that it isn't identically zero.

It must be admitted that the |crossing_point| routine is not perfectly accurate;
rounding errors might cause it to find a root when $y_1y_3>y_2^2$, or to
miss the roots when $y_1y_3<y_2^2$. The rotation process is itself
subject to rounding errors. Yet this code optimistically tries to
do the right thing.

@d we_found_it { tt=(t+04000) / 010000; goto FOUND; }

@<Check the places where $B(y_1,y_2,y_3;t)=0$...@>=
t=mp_crossing_point(mp, y1,y2,y3);
if ( t>fraction_one ) goto DONE;
y2=t_of_the_way(y2,y3);
x1=t_of_the_way(x1,x2);
x2=t_of_the_way(x2,x3);
x1=t_of_the_way(x1,x2);
if ( x1>=0 ) we_found_it;
if ( y2>0 ) y2=0;
tt=t; t=mp_crossing_point(mp, 0,-y2,-y3);
if ( t>fraction_one ) goto DONE;
x1=t_of_the_way(x1,x2);
x2=t_of_the_way(x2,x3);
if ( t_of_the_way(x1,x2)>=0 ) { 
  t=t_of_the_way(tt,fraction_one); we_found_it;
}

@ @<Handle the test for eastward directions when $y_1y_3=y_2^2$;
    either |goto found| or |goto done|@>=
{ 
  if ( mp_ab_vs_cd(mp, y1,y2,0,0)<0 ) {
    t=mp_make_fraction(mp, y1,y1-y2);
    x1=t_of_the_way(x1,x2);
    x2=t_of_the_way(x2,x3);
    if ( t_of_the_way(x1,x2)>=0 ) we_found_it;
  } else if ( y3==0 ) {
    if ( y1==0 ) {
      @<Exit to |found| if the derivative $B(x_1,x_2,x_3;t)$ becomes |>=0|@>;
    } else if ( x3>=0 ) {
      tt=unity; goto FOUND;
    }
  }
  goto DONE;
}

@ At this point we know that the derivative of |y(t)| is identically zero,
and that |x1<0|; but either |x2>=0| or |x3>=0|, so there's some hope of
traveling east.

@<Exit to |found| if the derivative $B(x_1,x_2,x_3;t)$ becomes |>=0|...@>=
{ 
  t=mp_crossing_point(mp, -x1,-x2,-x3);
  if ( t<=fraction_one ) we_found_it;
  if ( mp_ab_vs_cd(mp, x1,x3,x2,x2)<=0 ) { 
    t=mp_make_fraction(mp, x1,x1-x2); we_found_it;
  }
}

@ The intersection of two cubics can be found by an interesting variant
of the general bisection scheme described in the introduction to
|crossing_point|.\
Given $w(t)=B(w_0,w_1,w_2,w_3;t)$ and $z(t)=B(z_0,z_1,z_2,z_3;t)$,
we wish to find a pair of times $(t_1,t_2)$ such that $w(t_1)=z(t_2)$,
if an intersection exists. First we find the smallest rectangle that
encloses the points $\{w_0,w_1,w_2,w_3\}$ and check that it overlaps
the smallest rectangle that encloses
$\{z_0,z_1,z_2,z_3\}$; if not, the cubics certainly don't intersect.
But if the rectangles do overlap, we bisect the intervals, getting
new cubics $w'$ and~$w''$, $z'$~and~$z''$; the intersection routine first
tries for an intersection between $w'$ and~$z'$, then (if unsuccessful)
between $w'$ and~$z''$, then (if still unsuccessful) between $w''$ and~$z'$,
finally (if thrice unsuccessful) between $w''$ and~$z''$. After $l$~successful
levels of bisection we will have determined the intersection times $t_1$
and~$t_2$ to $l$~bits of accuracy.

\def\submin{_{\rm min}} \def\submax{_{\rm max}}
As before, it is better to work with the numbers $W_k=2^l(w_k-w_{k-1})$
and $Z_k=2^l(z_k-z_{k-1})$ rather than the coefficients $w_k$ and $z_k$
themselves. We also need one other quantity, $\Delta=2^l(w_0-z_0)$,
to determine when the enclosing rectangles overlap. Here's why:
The $x$~coordinates of~$w(t)$ are between $u\submin$ and $u\submax$,
and the $x$~coordinates of~$z(t)$ are between $x\submin$ and $x\submax$,
if we write $w_k=(u_k,v_k)$ and $z_k=(x_k,y_k)$ and $u\submin=
\min(u_0,u_1,u_2,u_3)$, etc. These intervals of $x$~coordinates
overlap if and only if $u\submin\L x\submax$ and
$x\submin\L u\submax$. Letting
$$U\submin=\min(0,U_1,U_1+U_2,U_1+U_2+U_3),\;
  U\submax=\max(0,U_1,U_1+U_2,U_1+U_2+U_3),$$
we have $2^lu\submin=2^lu_0+U\submin$, etc.; the condition for overlap
reduces to
$$X\submin-U\submax\L 2^l(u_0-x_0)\L X\submax-U\submin.$$
Thus we want to maintain the quantity $2^l(u_0-x_0)$; similarly,
the quantity $2^l(v_0-y_0)$ accounts for the $y$~coordinates. The
coordinates of $\Delta=2^l(w_0-z_0)$ must stay bounded as $l$ increases,
because of the overlap condition; i.e., we know that $X\submin$,
$X\submax$, and their relatives are bounded, hence $X\submax-
U\submin$ and $X\submin-U\submax$ are bounded.

@ Incidentally, if the given cubics intersect more than once, the process
just sketched will not necessarily find the lexicographically smallest pair
$(t_1,t_2)$. The solution actually obtained will be smallest in ``shuffled
order''; i.e., if $t_1=(.a_1a_2\ldots a_{16})_2$ and
$t_2=(.b_1b_2\ldots b_{16})_2$, then we will minimize
$a_1b_1a_2b_2\ldots a_{16}b_{16}$, not
$a_1a_2\ldots a_{16}b_1b_2\ldots b_{16}$.
Shuffled order agrees with lexicographic order if all pairs of solutions
$(t_1,t_2)$ and $(t_1',t_2')$ have the property that $t_1<t_1'$ iff
$t_2<t_2'$; but in general, lexicographic order can be quite different,
and the bisection algorithm would be substantially less efficient if it were
constrained by lexicographic order.

For example, suppose that an overlap has been found for $l=3$ and
$(t_1,t_2)= (.101,.011)$ in binary, but that no overlap is produced by
either of the alternatives $(.1010,.0110)$, $(.1010,.0111)$ at level~4.
Then there is probably an intersection in one of the subintervals
$(.1011,.011x)$; but lexicographic order would require us to explore
$(.1010,.1xxx)$ and $(.1011,.00xx)$ and $(.1011,.010x)$ first. We wouldn't
want to store all of the subdivision data for the second path, so the
subdivisions would have to be regenerated many times. Such inefficiencies
would be associated with every `1' in the binary representation of~$t_1$.

@ The subdivision process introduces rounding errors, hence we need to
make a more liberal test for overlap. It is not hard to show that the
computed values of $U_i$ differ from the truth by at most~$l$, on
level~$l$, hence $U\submin$ and $U\submax$ will be at most $3l$ in error.
If $\beta$ is an upper bound on the absolute error in the computed
components of $\Delta=(|delx|,|dely|)$ on level~$l$, we will replace
the test `$X\submin-U\submax\L|delx|$' by the more liberal test
`$X\submin-U\submax\L|delx|+|tol|$', where $|tol|=6l+\beta$.

More accuracy is obtained if we try the algorithm first with |tol=0|;
the more liberal tolerance is used only if an exact approach fails.
It is convenient to do this double-take by letting `3' in the preceding
paragraph be a parameter, which is first 0, then 3.

@<Glob...@>=
unsigned int tol_step; /* either 0 or 3, usually */

@ We shall use an explicit stack to implement the recursive bisection
method described above. The |bisect_stack| array will contain numerous 5-word
packets like $(U_1,U_2,U_3,U\submin,U\submax)$, as well as 20-word packets
comprising the 5-word packets for $U$, $V$, $X$, and~$Y$.

The following macros define the allocation of stack positions to
the quantities needed for bisection-intersection.

@d stack_1(A) mp->bisect_stack[(A)] /* $U_1$, $V_1$, $X_1$, or $Y_1$ */
@d stack_2(A) mp->bisect_stack[(A)+1] /* $U_2$, $V_2$, $X_2$, or $Y_2$ */
@d stack_3(A) mp->bisect_stack[(A)+2] /* $U_3$, $V_3$, $X_3$, or $Y_3$ */
@d stack_min(A) mp->bisect_stack[(A)+3]
  /* $U\submin$, $V\submin$, $X\submin$, or $Y\submin$ */
@d stack_max(A) mp->bisect_stack[(A)+4]
  /* $U\submax$, $V\submax$, $X\submax$, or $Y\submax$ */
@d int_packets 20 /* number of words to represent $U_k$, $V_k$, $X_k$, and $Y_k$ */
@#
@d u_packet(A) ((A)-5)
@d v_packet(A) ((A)-10)
@d x_packet(A) ((A)-15)
@d y_packet(A) ((A)-20)
@d l_packets (mp->bisect_ptr-int_packets)
@d r_packets mp->bisect_ptr
@d ul_packet u_packet(l_packets) /* base of $U'_k$ variables */
@d vl_packet v_packet(l_packets) /* base of $V'_k$ variables */
@d xl_packet x_packet(l_packets) /* base of $X'_k$ variables */
@d yl_packet y_packet(l_packets) /* base of $Y'_k$ variables */
@d ur_packet u_packet(r_packets) /* base of $U''_k$ variables */
@d vr_packet v_packet(r_packets) /* base of $V''_k$ variables */
@d xr_packet x_packet(r_packets) /* base of $X''_k$ variables */
@d yr_packet y_packet(r_packets) /* base of $Y''_k$ variables */
@#
@d u1l stack_1(ul_packet) /* $U'_1$ */
@d u2l stack_2(ul_packet) /* $U'_2$ */
@d u3l stack_3(ul_packet) /* $U'_3$ */
@d v1l stack_1(vl_packet) /* $V'_1$ */
@d v2l stack_2(vl_packet) /* $V'_2$ */
@d v3l stack_3(vl_packet) /* $V'_3$ */
@d x1l stack_1(xl_packet) /* $X'_1$ */
@d x2l stack_2(xl_packet) /* $X'_2$ */
@d x3l stack_3(xl_packet) /* $X'_3$ */
@d y1l stack_1(yl_packet) /* $Y'_1$ */
@d y2l stack_2(yl_packet) /* $Y'_2$ */
@d y3l stack_3(yl_packet) /* $Y'_3$ */
@d u1r stack_1(ur_packet) /* $U''_1$ */
@d u2r stack_2(ur_packet) /* $U''_2$ */
@d u3r stack_3(ur_packet) /* $U''_3$ */
@d v1r stack_1(vr_packet) /* $V''_1$ */
@d v2r stack_2(vr_packet) /* $V''_2$ */
@d v3r stack_3(vr_packet) /* $V''_3$ */
@d x1r stack_1(xr_packet) /* $X''_1$ */
@d x2r stack_2(xr_packet) /* $X''_2$ */
@d x3r stack_3(xr_packet) /* $X''_3$ */
@d y1r stack_1(yr_packet) /* $Y''_1$ */
@d y2r stack_2(yr_packet) /* $Y''_2$ */
@d y3r stack_3(yr_packet) /* $Y''_3$ */
@#
@d stack_dx mp->bisect_stack[mp->bisect_ptr] /* stacked value of |delx| */
@d stack_dy mp->bisect_stack[mp->bisect_ptr+1] /* stacked value of |dely| */
@d stack_tol mp->bisect_stack[mp->bisect_ptr+2] /* stacked value of |tol| */
@d stack_uv mp->bisect_stack[mp->bisect_ptr+3] /* stacked value of |uv| */
@d stack_xy mp->bisect_stack[mp->bisect_ptr+4] /* stacked value of |xy| */
@d int_increment (int_packets+int_packets+5) /* number of stack words per level */

@<Glob...@>=
integer *bisect_stack;
integer bisect_ptr;

@ @<Allocate or initialize ...@>=
mp->bisect_stack = xmalloc((bistack_size+1),sizeof(integer));

@ @<Dealloc variables@>=
xfree(mp->bisect_stack);

@ @<Check the ``constant''...@>=
if ( int_packets+17*int_increment>bistack_size ) mp->bad=19;

@ Computation of the min and max is a tedious but fairly fast sequence of
instructions; exactly four comparisons are made in each branch.

@d set_min_max(A) 
  if ( stack_1((A))<0 ) {
    if ( stack_3((A))>=0 ) {
      if ( stack_2((A))<0 ) stack_min((A))=stack_1((A))+stack_2((A));
      else stack_min((A))=stack_1((A));
      stack_max((A))=stack_1((A))+stack_2((A))+stack_3((A));
      if ( stack_max((A))<0 ) stack_max((A))=0;
    } else { 
      stack_min((A))=stack_1((A))+stack_2((A))+stack_3((A));
      if ( stack_min((A))>stack_1((A)) ) stack_min((A))=stack_1((A));
      stack_max((A))=stack_1((A))+stack_2((A));
      if ( stack_max((A))<0 ) stack_max((A))=0;
    }
  } else if ( stack_3((A))<=0 ) {
    if ( stack_2((A))>0 ) stack_max((A))=stack_1((A))+stack_2((A));
    else stack_max((A))=stack_1((A));
    stack_min((A))=stack_1((A))+stack_2((A))+stack_3((A));
    if ( stack_min((A))>0 ) stack_min((A))=0;
  } else  { 
    stack_max((A))=stack_1((A))+stack_2((A))+stack_3((A));
    if ( stack_max((A))<stack_1((A)) ) stack_max((A))=stack_1((A));
    stack_min((A))=stack_1((A))+stack_2((A));
    if ( stack_min((A))>0 ) stack_min((A))=0;
  }

@ It's convenient to keep the current values of $l$, $t_1$, and $t_2$ in
the integer form $2^l+2^lt_1$ and $2^l+2^lt_2$. The |cubic_intersection|
routine uses global variables |cur_t| and |cur_tt| for this purpose;
after successful completion, |cur_t| and |cur_tt| will contain |unity|
plus the |scaled| values of $t_1$ and~$t_2$.

The values of |cur_t| and |cur_tt| will be set to zero if |cubic_intersection|
finds no intersection. The routine gives up and gives an approximate answer
if it has backtracked
more than 5000 times (otherwise there are cases where several minutes
of fruitless computation would be possible).

@d max_patience 5000

@<Glob...@>=
integer cur_t;integer cur_tt; /* controls and results of |cubic_intersection| */
integer time_to_go; /* this many backtracks before giving up */
integer max_t; /* maximum of $2^{l+1}$ so far achieved */

@ The given cubics $B(w_0,w_1,w_2,w_3;t)$ and
$B(z_0,z_1,z_2,z_3;t)$ are specified in adjacent knot nodes |(p,mp_link(p))|
and |(pp,mp_link(pp))|, respectively.

@c 
static void mp_cubic_intersection (MP mp,pointer p, pointer pp) {
  pointer q,qq; /* |mp_link(p)|, |mp_link(pp)| */
  mp->time_to_go=max_patience; mp->max_t=2;
  @<Initialize for intersections at level zero@>;
CONTINUE:
  while (1) { 
    if ( mp->delx-mp->tol<=stack_max(x_packet(mp->xy))-stack_min(u_packet(mp->uv)))
    if ( mp->delx+mp->tol>=stack_min(x_packet(mp->xy))-stack_max(u_packet(mp->uv)))
    if ( mp->dely-mp->tol<=stack_max(y_packet(mp->xy))-stack_min(v_packet(mp->uv)))
    if ( mp->dely+mp->tol>=stack_min(y_packet(mp->xy))-stack_max(v_packet(mp->uv))) 
    { 
      if ( mp->cur_t>=mp->max_t ){ 
        if ( mp->max_t==two ) { /* we've done 17 bisections */ 
           mp->cur_t=halfp(mp->cur_t+1); 
	       mp->cur_tt=halfp(mp->cur_tt+1); 
           return;
        }
        mp->max_t+=mp->max_t; mp->appr_t=mp->cur_t; mp->appr_tt=mp->cur_tt;
      }
      @<Subdivide for a new level of intersection@>;
      goto CONTINUE;
    }
    if ( mp->time_to_go>0 ) {
      decr(mp->time_to_go);
    } else { 
      while ( mp->appr_t<unity ) { 
        mp->appr_t+=mp->appr_t; mp->appr_tt+=mp->appr_tt;
      }
      mp->cur_t=mp->appr_t; mp->cur_tt=mp->appr_tt; return;
    }
    @<Advance to the next pair |(cur_t,cur_tt)|@>;
  }
}

@ The following variables are global, although they are used only by
|cubic_intersection|, because it is necessary on some machines to
split |cubic_intersection| up into two procedures.

@<Glob...@>=
integer delx;integer dely; /* the components of $\Delta=2^l(w_0-z_0)$ */
integer tol; /* bound on the uncertainty in the overlap test */
integer uv;
integer xy; /* pointers to the current packets of interest */
integer three_l; /* |tol_step| times the bisection level */
integer appr_t;integer appr_tt; /* best approximations known to the answers */

@ We shall assume that the coordinates are sufficiently non-extreme that
integer overflow will not occur.
@^overflow in arithmetic@>

@<Initialize for intersections at level zero@>=
q=mp_link(p); qq=mp_link(pp); mp->bisect_ptr=int_packets;
u1r=mp_right_x(p)-mp_x_coord(p); u2r=mp_left_x(q)-mp_right_x(p);
u3r=mp_x_coord(q)-mp_left_x(q); set_min_max(ur_packet);
v1r=mp_right_y(p)-mp_y_coord(p); v2r=mp_left_y(q)-mp_right_y(p);
v3r=mp_y_coord(q)-mp_left_y(q); set_min_max(vr_packet);
x1r=mp_right_x(pp)-mp_x_coord(pp); x2r=mp_left_x(qq)-mp_right_x(pp);
x3r=mp_x_coord(qq)-mp_left_x(qq); set_min_max(xr_packet);
y1r=mp_right_y(pp)-mp_y_coord(pp); y2r=mp_left_y(qq)-mp_right_y(pp);
y3r=mp_y_coord(qq)-mp_left_y(qq); set_min_max(yr_packet);
mp->delx=mp_x_coord(p)-mp_x_coord(pp); mp->dely=mp_y_coord(p)-mp_y_coord(pp);
mp->tol=0; mp->uv=r_packets; mp->xy=r_packets; 
mp->three_l=0; mp->cur_t=1; mp->cur_tt=1

@ @<Subdivide for a new level of intersection@>=
stack_dx=mp->delx; stack_dy=mp->dely; stack_tol=mp->tol; 
stack_uv=mp->uv; stack_xy=mp->xy;
mp->bisect_ptr=mp->bisect_ptr+int_increment;
mp->cur_t+=mp->cur_t; mp->cur_tt+=mp->cur_tt;
u1l=stack_1(u_packet(mp->uv)); u3r=stack_3(u_packet(mp->uv));
u2l=half(u1l+stack_2(u_packet(mp->uv)));
u2r=half(u3r+stack_2(u_packet(mp->uv)));
u3l=half(u2l+u2r); u1r=u3l;
set_min_max(ul_packet); set_min_max(ur_packet);
v1l=stack_1(v_packet(mp->uv)); v3r=stack_3(v_packet(mp->uv));
v2l=half(v1l+stack_2(v_packet(mp->uv)));
v2r=half(v3r+stack_2(v_packet(mp->uv)));
v3l=half(v2l+v2r); v1r=v3l;
set_min_max(vl_packet); set_min_max(vr_packet);
x1l=stack_1(x_packet(mp->xy)); x3r=stack_3(x_packet(mp->xy));
x2l=half(x1l+stack_2(x_packet(mp->xy)));
x2r=half(x3r+stack_2(x_packet(mp->xy)));
x3l=half(x2l+x2r); x1r=x3l;
set_min_max(xl_packet); set_min_max(xr_packet);
y1l=stack_1(y_packet(mp->xy)); y3r=stack_3(y_packet(mp->xy));
y2l=half(y1l+stack_2(y_packet(mp->xy)));
y2r=half(y3r+stack_2(y_packet(mp->xy)));
y3l=half(y2l+y2r); y1r=y3l;
set_min_max(yl_packet); set_min_max(yr_packet);
mp->uv=l_packets; mp->xy=l_packets;
mp->delx += mp->delx; mp->dely+=mp->dely;
mp->tol = mp->tol- mp->three_l+(integer)mp->tol_step; 
mp->tol += mp->tol; mp->three_l=mp->three_l+(integer)mp->tol_step

@ @<Advance to the next pair |(cur_t,cur_tt)|@>=
NOT_FOUND: 
if ( odd(mp->cur_tt) ) {
  if ( odd(mp->cur_t) ) {
     @<Descend to the previous level and |goto not_found|@>;
  } else { 
    incr(mp->cur_t);
    mp->delx=mp->delx+stack_1(u_packet(mp->uv))+stack_2(u_packet(mp->uv))
      +stack_3(u_packet(mp->uv));
    mp->dely=mp->dely+stack_1(v_packet(mp->uv))+stack_2(v_packet(mp->uv))
      +stack_3(v_packet(mp->uv));
    mp->uv=mp->uv+int_packets; /* switch from |l_packets| to |r_packets| */
    decr(mp->cur_tt); mp->xy=mp->xy-int_packets; 
         /* switch from |r_packets| to |l_packets| */
    mp->delx=mp->delx+stack_1(x_packet(mp->xy))+stack_2(x_packet(mp->xy))
      +stack_3(x_packet(mp->xy));
    mp->dely=mp->dely+stack_1(y_packet(mp->xy))+stack_2(y_packet(mp->xy))
      +stack_3(y_packet(mp->xy));
  }
} else { 
  incr(mp->cur_tt); mp->tol=mp->tol+mp->three_l;
  mp->delx=mp->delx-stack_1(x_packet(mp->xy))-stack_2(x_packet(mp->xy))
    -stack_3(x_packet(mp->xy));
  mp->dely=mp->dely-stack_1(y_packet(mp->xy))-stack_2(y_packet(mp->xy))
    -stack_3(y_packet(mp->xy));
  mp->xy=mp->xy+int_packets; /* switch from |l_packets| to |r_packets| */
}

@ @<Descend to the previous level...@>=
{ 
  mp->cur_t=halfp(mp->cur_t); mp->cur_tt=halfp(mp->cur_tt);
  if ( mp->cur_t==0 ) return;
  mp->bisect_ptr -= int_increment; 
  mp->three_l -= (integer)mp->tol_step;
  mp->delx=stack_dx; mp->dely=stack_dy; mp->tol=stack_tol; 
  mp->uv=stack_uv; mp->xy=stack_xy;
  goto NOT_FOUND;
}

@ The |path_intersection| procedure is much simpler.
It invokes |cubic_intersection| in lexicographic order until finding a
pair of cubics that intersect. The final intersection times are placed in
|cur_t| and~|cur_tt|.

@c 
static void mp_path_intersection (MP mp,pointer h, pointer hh) {
  pointer p,pp; /* link registers that traverse the given paths */
  integer n,nn; /* integer parts of intersection times, minus |unity| */
  @<Change one-point paths into dead cycles@>;
  mp->tol_step=0;
  do {  
    n=-unity; p=h;
    do {  
      if ( mp_right_type(p)!=mp_endpoint ) { 
        nn=-unity; pp=hh;
        do {  
          if ( mp_right_type(pp)!=mp_endpoint )  { 
            mp_cubic_intersection(mp, p,pp);
            if ( mp->cur_t>0 ) { 
              mp->cur_t=mp->cur_t+n; mp->cur_tt=mp->cur_tt+nn; 
              return;
            }
          }
          nn=nn+unity; pp=mp_link(pp);
        } while (pp!=hh);
      }
      n=n+unity; p=mp_link(p);
    } while (p!=h);
    mp->tol_step=mp->tol_step+3;
  } while (mp->tol_step<=3);
  mp->cur_t=-unity; mp->cur_tt=-unity;
}

@ @<Change one-point paths...@>=
if ( mp_right_type(h)==mp_endpoint ) {
  mp_right_x(h)=mp_x_coord(h); mp_left_x(h)=mp_x_coord(h);
  mp_right_y(h)=mp_y_coord(h); mp_left_y(h)=mp_y_coord(h); mp_right_type(h)=mp_explicit;
}
if ( mp_right_type(hh)==mp_endpoint ) {
  mp_right_x(hh)=mp_x_coord(hh); mp_left_x(hh)=mp_x_coord(hh);
  mp_right_y(hh)=mp_y_coord(hh); mp_left_y(hh)=mp_y_coord(hh); mp_right_type(hh)=mp_explicit;
}

@* \[24] Dynamic linear equations.
\MP\ users define variables implicitly by stating equations that should be
satisfied; the computer is supposed to be smart enough to solve those equations.
And indeed, the computer tries valiantly to do so, by distinguishing five
different types of numeric values:

\smallskip\hang
|type(p)=mp_known| is the nice case, when |value(p)| is the |scaled| value
of the variable whose address is~|p|.

\smallskip\hang
|type(p)=mp_dependent| means that |value(p)| is not present, but |dep_list(p)|
points to a {\sl dependency list\/} that expresses the value of variable~|p|
as a |scaled| number plus a sum of independent variables with |fraction|
coefficients.

\smallskip\hang
|type(p)=mp_independent| means that |value(p)=64s+m|, where |s>0| is a ``serial
number'' reflecting the time this variable was first used in an equation;
also |0<=m<64|, and each dependent variable
that refers to this one is actually referring to the future value of
this variable times~$2^m$. (Usually |m=0|, but higher degrees of
scaling are sometimes needed to keep the coefficients in dependency lists
from getting too large. The value of~|m| will always be even.)

\smallskip\hang
|type(p)=mp_numeric_type| means that variable |p| hasn't appeared in an
equation before, but it has been explicitly declared to be numeric.

\smallskip\hang
|type(p)=undefined| means that variable |p| hasn't appeared before.

\smallskip\noindent
We have actually discussed these five types in the reverse order of their
history during a computation: Once |known|, a variable never again
becomes |dependent|; once |dependent|, it almost never again becomes
|mp_independent|; once |mp_independent|, it never again becomes |mp_numeric_type|;
and once |mp_numeric_type|, it never again becomes |undefined| (except
of course when the user specifically decides to scrap the old value
and start again). A backward step may, however, take place: Sometimes
a |dependent| variable becomes |mp_independent| again, when one of the
independent variables it depends on is reverting to |undefined|.


The next patch detects overflow of independent-variable serial
numbers. Diagnosed and patched by Thorsten Dahlheimer.

@d s_scale 64 /* the serial numbers are multiplied by this factor */
@d new_indep(A)  /* create a new independent variable */
  { if ( mp->serial_no>el_gordo-s_scale )
    mp_fatal_error(mp, "variable instance identifiers exhausted");
  mp_type((A))=mp_independent; mp->serial_no=mp->serial_no+s_scale;
  value((A))=mp->serial_no;
  }

@<Glob...@>=
integer serial_no; /* the most recent serial number, times |s_scale| */

@ @<Make variable |q+s| newly independent@>=new_indep(q+s)

@ But how are dependency lists represented? It's simple: The linear combination
$\alpha_1v_1+\cdots+\alpha_kv_k+\beta$ appears in |k+1| value nodes. If
|q=dep_list(p)| points to this list, and if |k>0|, then |value(q)=
@t$\alpha_1$@>| (which is a |fraction|); |mp_info(q)| points to the location
of $\alpha_1$; and |mp_link(p)| points to the dependency list
$\alpha_2v_2+\cdots+\alpha_kv_k+\beta$. On the other hand if |k=0|,
then |value(q)=@t$\beta$@>| (which is |scaled|) and |mp_info(q)=null|.
The independent variables $v_1$, \dots,~$v_k$ have been sorted so that
they appear in decreasing order of their |value| fields (i.e., of
their serial numbers). \ (It is convenient to use decreasing order,
since |value(null)=0|. If the independent variables were not sorted by
serial number but by some other criterion, such as their location in |mem|,
the equation-solving mechanism would be too system-dependent, because
the ordering can affect the computed results.)

The |link| field in the node that contains the constant term $\beta$ is
called the {\sl final link\/} of the dependency list. \MP\ maintains
a doubly-linked master list of all dependency lists, in terms of a permanently
allocated node
in |mem| called |dep_head|. If there are no dependencies, we have
|mp_link(dep_head)=dep_head| and |prev_dep(dep_head)=dep_head|;
otherwise |mp_link(dep_head)| points to the first dependent variable, say~|p|,
and |prev_dep(p)=dep_head|. We have |type(p)=mp_dependent|, and |dep_list(p)|
points to its dependency list. If the final link of that dependency list
occurs in location~|q|, then |mp_link(q)| points to the next dependent
variable (say~|r|); and we have |prev_dep(r)=q|, etc.

@d dep_list(A) mp_link(value_loc((A)))
  /* half of the |value| field in a |dependent| variable */
@d prev_dep(A) mp_info(value_loc((A)))
  /* the other half; makes a doubly linked list */
@d dep_node_size 2 /* the number of words per dependency node */

@<Initialize table entries...@>= mp->serial_no=0;
mp_link(dep_head)=dep_head; prev_dep(dep_head)=dep_head;
mp_info(dep_head)=null; dep_list(dep_head)=null;

@ Actually the description above contains a little white lie. There's
another kind of variable called |mp_proto_dependent|, which is
just like a |dependent| one except that the $\alpha$ coefficients
in its dependency list are |scaled| instead of being fractions.
Proto-dependency lists are mixed with dependency lists in the
nodes reachable from |dep_head|.

@ Here is a procedure that prints a dependency list in symbolic form.
The second parameter should be either |dependent| or |mp_proto_dependent|,
to indicate the scaling of the coefficients.

@<Declarations@>=
static void mp_print_dependency (MP mp,pointer p, quarterword t);

@ @c
void mp_print_dependency (MP mp,pointer p, quarterword t) {
  integer v; /* a coefficient */
  pointer pp,q; /* for list manipulation */
  pp=p;
  while (true) { 
    v=abs(value(p)); q=mp_info(p);
    if ( q==null ) { /* the constant term */
      if ( (v!=0)||(p==pp) ) {
         if ( value(p)>0 ) if ( p!=pp ) mp_print_char(mp, xord('+'));
         mp_print_scaled(mp, value(p));
      }
      return;
    }
    @<Print the coefficient, unless it's $\pm1.0$@>;
    if ( mp_type(q)!=mp_independent ) mp_confusion(mp, "dep");
@:this can't happen dep}{\quad dep@>
    mp_print_variable_name(mp, q); v=value(q) % s_scale;
    while ( v>0 ) { mp_print(mp, "*4"); v=v-2; }
    p=mp_link(p);
  }
}

@ @<Print the coefficient, unless it's $\pm1.0$@>=
if ( value(p)<0 ) mp_print_char(mp, xord('-'));
else if ( p!=pp ) mp_print_char(mp, xord('+'));
if ( t==mp_dependent ) v=mp_round_fraction(mp, v);
if ( v!=unity ) mp_print_scaled(mp, v)

@ The maximum absolute value of a coefficient in a given dependency list
is returned by the following simple function.

@c 
static fraction mp_max_coef (MP mp,pointer p) {
  fraction x; /* the maximum so far */
  x=0;
  while ( mp_info(p)!=null ) {
    if ( abs(value(p))>x ) x=abs(value(p));
    p=mp_link(p);
  }
  return x;
}

@ One of the main operations needed on dependency lists is to add a multiple
of one list to the other; we call this |p_plus_fq|, where |p| and~|q| point
to dependency lists and |f| is a fraction.

If the coefficient of any independent variable becomes |coef_bound| or
more, in absolute value, this procedure changes the type of that variable
to `|independent_needing_fix|', and sets the global variable |fix_needed|
to~|true|. The value of $|coef_bound|=\mu$ is chosen so that
$\mu^2+\mu<8$; this means that the numbers we deal with won't
get too large. (Instead of the ``optimum'' $\mu=(\sqrt{33}-1)/2\approx
2.3723$, the safer value 7/3 is taken as the threshold.)

The changes mentioned in the preceding paragraph are actually done only if
the global variable |watch_coefs| is |true|. But it usually is; in fact,
it is |false| only when \MP\ is making a dependency list that will soon
be equated to zero.

Several procedures that act on dependency lists, including |p_plus_fq|,
set the global variable |dep_final| to the final (constant term) node of
the dependency list that they produce.

@d coef_bound 04525252525 /* |fraction| approximation to 7/3 */
@d independent_needing_fix 0

@<Glob...@>=
boolean fix_needed; /* does at least one |independent| variable need scaling? */
boolean watch_coefs; /* should we scale coefficients that exceed |coef_bound|? */
pointer dep_final; /* location of the constant term and final link */

@ @<Set init...@>=
mp->fix_needed=false; mp->watch_coefs=true;

@ The |p_plus_fq| procedure has a fourth parameter, |t|, that should be
set to |mp_proto_dependent| if |p| is a proto-dependency list. In this
case |f| will be |scaled|, not a |fraction|. Similarly, the fifth parameter~|tt|
should be |mp_proto_dependent| if |q| is a proto-dependency list.

List |q| is unchanged by the operation; but list |p| is totally destroyed.

The final link of the dependency list or proto-dependency list returned
by |p_plus_fq| is the same as the original final link of~|p|. Indeed, the
constant term of the result will be located in the same |mem| location
as the original constant term of~|p|.

Coefficients of the result are assumed to be zero if they are less than
a certain threshold. This compensates for inevitable rounding errors,
and tends to make more variables `|known|'. The threshold is approximately
$10^{-5}$ in the case of normal dependency lists, $10^{-4}$ for
proto-dependencies.

@d fraction_threshold 2685 /* a |fraction| coefficient less than this is zeroed */
@d half_fraction_threshold 1342 /* half of |fraction_threshold| */
@d scaled_threshold 8 /* a |scaled| coefficient less than this is zeroed */
@d half_scaled_threshold 4 /* half of |scaled_threshold| */

@<Declarations@>=
static pointer mp_p_plus_fq ( MP mp, pointer p, integer f, 
                      pointer q, quarterword t, quarterword tt) ;

@ @c
pointer mp_p_plus_fq ( MP mp, pointer p, integer f, 
                      pointer q, quarterword t, quarterword tt) {
  pointer pp,qq; /* |mp_info(p)| and |mp_info(q)|, respectively */
  pointer r,s; /* for list manipulation */
  integer threshold; /* defines a neighborhood of zero */
  integer v; /* temporary register */
  if ( t==mp_dependent ) threshold=fraction_threshold;
  else threshold=scaled_threshold;
  r=temp_head; pp=mp_info(p); qq=mp_info(q);
  while (1) {
    if ( pp==qq ) {
      if ( pp==null ) {
       break;
      } else {
        @<Contribute a term from |p|, plus |f| times the
          corresponding term from |q|@>
      }
    } else if ( value(pp)<value(qq) ) {
      @<Contribute a term from |q|, multiplied by~|f|@>
    } else { 
     mp_link(r)=p; r=p; p=mp_link(p); pp=mp_info(p);
    }
  }
  if ( t==mp_dependent )
    value(p)=mp_slow_add(mp, value(p),mp_take_fraction(mp, value(q),f));
  else  
    value(p)=mp_slow_add(mp, value(p),mp_take_scaled(mp, value(q),f));
  mp_link(r)=p; mp->dep_final=p; 
  return mp_link(temp_head);
}

@ @<Contribute a term from |p|, plus |f|...@>=
{ 
  if ( tt==mp_dependent ) v=value(p)+mp_take_fraction(mp, f,value(q));
  else v=value(p)+mp_take_scaled(mp, f,value(q));
  value(p)=v; s=p; p=mp_link(p);
  if ( abs(v)<threshold ) {
    mp_free_node(mp, s,dep_node_size);
  } else {
    if ( (abs(v)>=coef_bound)  && mp->watch_coefs ) { 
      mp_type(qq)=independent_needing_fix; mp->fix_needed=true;
    }
    mp_link(r)=s; r=s;
  };
  pp=mp_info(p); q=mp_link(q); qq=mp_info(q);
}

@ @<Contribute a term from |q|, multiplied by~|f|@>=
{ 
  if ( tt==mp_dependent ) v=mp_take_fraction(mp, f,value(q));
  else v=mp_take_scaled(mp, f,value(q));
  if ( abs(v)>halfp(threshold) ) { 
    s=mp_get_node(mp, dep_node_size); mp_info(s)=qq; value(s)=v;
    if ( (abs(v)>=coef_bound) && mp->watch_coefs ) { 
      mp_type(qq)=independent_needing_fix; mp->fix_needed=true;
    }
    mp_link(r)=s; r=s;
  }
  q=mp_link(q); qq=mp_info(q);
}

@ It is convenient to have another subroutine for the special case
of |p_plus_fq| when |f=1.0|. In this routine lists |p| and |q| are
both of the same type~|t| (either |dependent| or |mp_proto_dependent|).

@c 
static pointer mp_p_plus_q (MP mp,pointer p, pointer q, quarterword t) {
  pointer pp,qq; /* |mp_info(p)| and |mp_info(q)|, respectively */
  pointer r,s; /* for list manipulation */
  integer threshold; /* defines a neighborhood of zero */
  integer v; /* temporary register */
  if ( t==mp_dependent ) threshold=fraction_threshold;
  else threshold=scaled_threshold;
  r=temp_head; pp=mp_info(p); qq=mp_info(q);
  while (1) {
    if ( pp==qq ) {
      if ( pp==null ) {
        break;
      } else {
        @<Contribute a term from |p|, plus the
          corresponding term from |q|@>
      }
    } else { 
	  if ( value(pp)<value(qq) ) {
        s=mp_get_node(mp, dep_node_size); mp_info(s)=qq; value(s)=value(q);
        q=mp_link(q); qq=mp_info(q); mp_link(r)=s; r=s;
      } else { 
        mp_link(r)=p; r=p; p=mp_link(p); pp=mp_info(p);
      }
    }
  }
  value(p)=mp_slow_add(mp, value(p),value(q));
  mp_link(r)=p; mp->dep_final=p; 
  return mp_link(temp_head);
}

@ @<Contribute a term from |p|, plus the...@>=
{ 
  v=value(p)+value(q);
  value(p)=v; s=p; p=mp_link(p); pp=mp_info(p);
  if ( abs(v)<threshold ) {
    mp_free_node(mp, s,dep_node_size);
  } else { 
    if ( (abs(v)>=coef_bound ) && mp->watch_coefs ) {
      mp_type(qq)=independent_needing_fix; mp->fix_needed=true;
    }
    mp_link(r)=s; r=s;
  }
  q=mp_link(q); qq=mp_info(q);
}

@ A somewhat simpler routine will multiply a dependency list
by a given constant~|v|. The constant is either a |fraction| less than
|fraction_one|, or it is |scaled|. In the latter case we might be forced to
convert a dependency list to a proto-dependency list.
Parameters |t0| and |t1| are the list types before and after;
they should agree unless |t0=mp_dependent| and |t1=mp_proto_dependent|
and |v_is_scaled=true|.

@c 
static pointer mp_p_times_v (MP mp,pointer p, integer v, quarterword t0,
                         quarterword t1, boolean v_is_scaled) {
  pointer r,s; /* for list manipulation */
  integer w; /* tentative coefficient */
  integer threshold;
  boolean scaling_down;
  if ( t0!=t1 ) scaling_down=true; else scaling_down=(!v_is_scaled);
  if ( t1==mp_dependent ) threshold=half_fraction_threshold;
  else threshold=half_scaled_threshold;
  r=temp_head;
  while ( mp_info(p)!=null ) {    
    if ( scaling_down ) w=mp_take_fraction(mp, v,value(p));
    else w=mp_take_scaled(mp, v,value(p));
    if ( abs(w)<=threshold ) { 
      s=mp_link(p); mp_free_node(mp, p,dep_node_size); p=s;
    } else {
      if ( abs(w)>=coef_bound ) { 
        mp->fix_needed=true; mp_type(mp_info(p))=independent_needing_fix;
      }
      mp_link(r)=p; r=p; value(p)=w; p=mp_link(p);
    }
  }
  mp_link(r)=p;
  if ( v_is_scaled ) value(p)=mp_take_scaled(mp, value(p),v);
  else value(p)=mp_take_fraction(mp, value(p),v);
  return mp_link(temp_head);
}

@ Similarly, we sometimes need to divide a dependency list
by a given |scaled| constant.

@<Declarations@>=
static pointer mp_p_over_v (MP mp,pointer p, scaled v, quarterword 
  t0, quarterword t1) ;

@ @c
pointer mp_p_over_v (MP mp,pointer p, scaled v, quarterword 
  t0, quarterword t1) {
  pointer r,s; /* for list manipulation */
  integer w; /* tentative coefficient */
  integer threshold;
  boolean scaling_down;
  if ( t0!=t1 ) scaling_down=true; else scaling_down=false;
  if ( t1==mp_dependent ) threshold=half_fraction_threshold;
  else threshold=half_scaled_threshold;
  r=temp_head;
  while ( mp_info( p)!=null ) {
    if ( scaling_down ) {
      if ( abs(v)<02000000 ) w=mp_make_scaled(mp, value(p),v*010000);
      else w=mp_make_scaled(mp, mp_round_fraction(mp, value(p)),v);
    } else {
      w=mp_make_scaled(mp, value(p),v);
    }
    if ( abs(w)<=threshold ) {
      s=mp_link(p); mp_free_node(mp, p,dep_node_size); p=s;
    } else { 
      if ( abs(w)>=coef_bound ) {
         mp->fix_needed=true; mp_type(mp_info(p))=independent_needing_fix;
      }
      mp_link(r)=p; r=p; value(p)=w; p=mp_link(p);
    }
  }
  mp_link(r)=p; value(p)=mp_make_scaled(mp, value(p),v);
  return mp_link(temp_head);
}

@ Here's another utility routine for dependency lists. When an independent
variable becomes dependent, we want to remove it from all existing
dependencies. The |p_with_x_becoming_q| function computes the
dependency list of~|p| after variable~|x| has been replaced by~|q|.

This procedure has basically the same calling conventions as |p_plus_fq|:
List~|q| is unchanged; list~|p| is destroyed; the constant node and the
final link are inherited from~|p|; and the fourth parameter tells whether
or not |p| is |mp_proto_dependent|. However, the global variable |dep_final|
is not altered if |x| does not occur in list~|p|.

@c 
static pointer mp_p_with_x_becoming_q (MP mp,pointer p,
           pointer x, pointer q, quarterword t) {
  pointer r,s; /* for list manipulation */
  integer v; /* coefficient of |x| */
  integer sx; /* serial number of |x| */
  s=p; r=temp_head; sx=value(x);
  while ( value(mp_info(s))>sx ) { r=s; s=mp_link(s); };
  if ( mp_info(s)!=x ) { 
    return p;
  } else { 
    mp_link(temp_head)=p; mp_link(r)=mp_link(s); v=value(s);
    mp_free_node(mp, s,dep_node_size);
    return mp_p_plus_fq(mp, mp_link(temp_head),v,q,t,mp_dependent);
  }
}

@ Here's a simple procedure that reports an error when a variable
has just received a known value that's out of the required range.

@<Declarations@>=
static void mp_val_too_big (MP mp,scaled x) ;

@ @c void mp_val_too_big (MP mp,scaled x) { 
  if ( mp->internal[mp_warning_check]>0 ) { 
    print_err("Value is too large ("); mp_print_scaled(mp, x); mp_print_char(mp, xord(')'));
@.Value is too large@>
    help4("The equation I just processed has given some variable",
      "a value of 4096 or more. Continue and I'll try to cope",
      "with that big value; but it might be dangerous.",
      "(Set warningcheck:=0 to suppress this message.)");
    mp_error(mp);
  }
}

@ When a dependent variable becomes known, the following routine
removes its dependency list. Here |p| points to the variable, and
|q| points to the dependency list (which is one node long).

@<Declarations@>=
static void mp_make_known (MP mp,pointer p, pointer q) ;

@ @c void mp_make_known (MP mp,pointer p, pointer q) {
  int t; /* the previous type */
  prev_dep(mp_link(q))=prev_dep(p);
  mp_link(prev_dep(p))=mp_link(q); t=mp_type(p);
  mp_type(p)=mp_known; value(p)=value(q); mp_free_node(mp, q,dep_node_size);
  if ( abs(value(p))>=fraction_one ) mp_val_too_big(mp, value(p));
  if (( mp->internal[mp_tracing_equations]>0) && mp_interesting(mp, p) ) {
    mp_begin_diagnostic(mp); mp_print_nl(mp, "#### ");
@:]]]\#\#\#\#_}{\.{\#\#\#\#}@>
    mp_print_variable_name(mp, p); 
    mp_print_char(mp, xord('=')); mp_print_scaled(mp, value(p));
    mp_end_diagnostic(mp, false);
  }
  if (( mp->cur_exp==p ) && mp->cur_type==t ) {
    mp->cur_type=mp_known; mp->cur_exp=value(p);
    mp_free_node(mp, p,value_node_size);
  }
}

@ The |fix_dependencies| routine is called into action when |fix_needed|
has been triggered. The program keeps a list~|s| of independent variables
whose coefficients must be divided by~4.

In unusual cases, this fixup process might reduce one or more coefficients
to zero, so that a variable will become known more or less by default.

@<Declarations@>=
static void mp_fix_dependencies (MP mp);

@ @c 
static void mp_fix_dependencies (MP mp) {
  pointer p,q,r,s,t; /* list manipulation registers */
  pointer x; /* an independent variable */
  r=mp_link(dep_head); s=null;
  while ( r!=dep_head ){ 
    t=r;
    @<Run through the dependency list for variable |t|, fixing
      all nodes, and ending with final link~|q|@>;
    r=mp_link(q);
    if ( q==dep_list(t) ) mp_make_known(mp, t,q);
  }
  while ( s!=null ) { 
    p=mp_link(s); x=mp_info(s); free_avail(s); s=p;
    mp_type(x)=mp_independent; value(x)=value(x)+2;
  }
  mp->fix_needed=false;
}

@ @d independent_being_fixed 1 /* this variable already appears in |s| */

@<Run through the dependency list for variable |t|...@>=
r=value_loc(t); /* |mp_link(r)=dep_list(t)| */
while (1) { 
  q=mp_link(r); x=mp_info(q);
  if ( x==null ) break;
  if ( mp_type(x)<=independent_being_fixed ) {
    if ( mp_type(x)<independent_being_fixed ) {
      p=mp_get_avail(mp); mp_link(p)=s; s=p;
      mp_info(s)=x; mp_type(x)=independent_being_fixed;
    }
    value(q)=value(q) / 4;
    if ( value(q)==0 ) {
      mp_link(r)=mp_link(q); mp_free_node(mp, q,dep_node_size); q=r;
    }
  }
  r=q;
}


@ The |new_dep| routine installs a dependency list~|p| into the value node~|q|,
linking it into the list of all known dependencies. We assume that
|dep_final| points to the final node of list~|p|.

@c 
static void mp_new_dep (MP mp,pointer q, pointer p) {
  pointer r; /* what used to be the first dependency */
  dep_list(q)=p; prev_dep(q)=dep_head;
  r=mp_link(dep_head); mp_link(mp->dep_final)=r; prev_dep(r)=mp->dep_final;
  mp_link(dep_head)=q;
}

@ Here is one of the ways a dependency list gets started.
The |const_dependency| routine produces a list that has nothing but
a constant term.

@c static pointer mp_const_dependency (MP mp, scaled v) {
  mp->dep_final=mp_get_node(mp, dep_node_size);
  value(mp->dep_final)=v; mp_info(mp->dep_final)=null;
  return mp->dep_final;
}

@ And here's a more interesting way to start a dependency list from scratch:
The parameter to |single_dependency| is the location of an
independent variable~|x|, and the result is the simple dependency list
`|x+0|'.

In the unlikely event that the given independent variable has been doubled so
often that we can't refer to it with a nonzero coefficient,
|single_dependency| returns the simple list `0'.  This case can be
recognized by testing that the returned list pointer is equal to
|dep_final|.

@c 
static pointer mp_single_dependency (MP mp,pointer p) {
  pointer q; /* the new dependency list */
  integer m; /* the number of doublings */
  m=value(p) % s_scale;
  if ( m>28 ) {
    return mp_const_dependency(mp, 0);
  } else { 
    q=mp_get_node(mp, dep_node_size);
    value(q)=(integer)two_to_the(28-m); mp_info(q)=p;
    mp_link(q)=mp_const_dependency(mp, 0);
    return q;
  }
}

@ We sometimes need to make an exact copy of a dependency list.

@c 
static pointer mp_copy_dep_list (MP mp,pointer p) {
  pointer q; /* the new dependency list */
  q=mp_get_node(mp, dep_node_size); mp->dep_final=q;
  while (1) { 
    mp_info(mp->dep_final)=mp_info(p); value(mp->dep_final)=value(p);
    if ( mp_info(mp->dep_final)==null ) break;
    mp_link(mp->dep_final)=mp_get_node(mp, dep_node_size);
    mp->dep_final=mp_link(mp->dep_final); p=mp_link(p);
  }
  return q;
}

@ But how do variables normally become known? Ah, now we get to the heart of the
equation-solving mechanism. The |linear_eq| procedure is given a |dependent|
or |mp_proto_dependent| list,~|p|, in which at least one independent variable
appears. It equates this list to zero, by choosing an independent variable
with the largest coefficient and making it dependent on the others. The
newly dependent variable is eliminated from all current dependencies,
thereby possibly making other dependent variables known.

The given list |p| is, of course, totally destroyed by all this processing.

@c 
static void mp_linear_eq (MP mp, pointer p, quarterword t) {
  pointer q,r,s; /* for link manipulation */
  pointer x; /* the variable that loses its independence */
  integer n; /* the number of times |x| had been halved */
  integer v; /* the coefficient of |x| in list |p| */
  pointer prev_r; /* lags one step behind |r| */
  pointer final_node; /* the constant term of the new dependency list */
  integer w; /* a tentative coefficient */
   @<Find a node |q| in list |p| whose coefficient |v| is largest@>;
  x=mp_info(q); n=value(x) % s_scale;
  @<Divide list |p| by |-v|, removing node |q|@>;
  if ( mp->internal[mp_tracing_equations]>0 ) {
    @<Display the new dependency@>;
  }
  @<Simplify all existing dependencies by substituting for |x|@>;
  @<Change variable |x| from |independent| to |dependent| or |known|@>;
  if ( mp->fix_needed ) mp_fix_dependencies(mp);
}

@ @<Find a node |q| in list |p| whose coefficient |v| is largest@>=
q=p; r=mp_link(p); v=value(q);
while ( mp_info(r)!=null ) { 
  if ( abs(value(r))>abs(v) ) { q=r; v=value(r); };
  r=mp_link(r);
}

@ Here we want to change the coefficients from |scaled| to |fraction|,
except in the constant term. In the common case of a trivial equation
like `\.{x=3.14}', we will have |v=-fraction_one|, |q=p|, and |t=mp_dependent|.

@<Divide list |p| by |-v|, removing node |q|@>=
s=temp_head; mp_link(s)=p; r=p;
do { 
  if ( r==q ) {
    mp_link(s)=mp_link(r); mp_free_node(mp, r,dep_node_size);
  } else  { 
    w=mp_make_fraction(mp, value(r),v);
    if ( abs(w)<=half_fraction_threshold ) {
      mp_link(s)=mp_link(r); mp_free_node(mp, r,dep_node_size);
    } else { 
      value(r)=-w; s=r;
    }
  }
  r=mp_link(s);
} while (mp_info(r)!=null);
if ( t==mp_proto_dependent ) {
  value(r)=-mp_make_scaled(mp, value(r),v);
} else if ( v!=-fraction_one ) {
  value(r)=-mp_make_fraction(mp, value(r),v);
}
final_node=r; p=mp_link(temp_head)

@ @<Display the new dependency@>=
if ( mp_interesting(mp, x) ) {
  mp_begin_diagnostic(mp); mp_print_nl(mp, "## "); 
  mp_print_variable_name(mp, x);
@:]]]\#\#_}{\.{\#\#}@>
  w=n;
  while ( w>0 ) { mp_print(mp, "*4"); w=w-2;  };
  mp_print_char(mp, xord('=')); mp_print_dependency(mp, p,mp_dependent); 
  mp_end_diagnostic(mp, false);
}

@ @<Simplify all existing dependencies by substituting for |x|@>=
prev_r=dep_head; r=mp_link(dep_head);
while ( r!=dep_head ) {
  s=dep_list(r); q=mp_p_with_x_becoming_q(mp, s,x,p,mp_type(r));
  if ( mp_info(q)==null ) {
    mp_make_known(mp, r,q);
  } else { 
    dep_list(r)=q;
    do {  q=mp_link(q); } while (mp_info(q)!=null);
    prev_r=q;
  }
  r=mp_link(prev_r);
}

@ @<Change variable |x| from |independent| to |dependent| or |known|@>=
if ( n>0 ) @<Divide list |p| by $2^n$@>;
if ( mp_info(p)==null ) {
  mp_type(x)=mp_known;
  value(x)=value(p);
  if ( abs(value(x))>=fraction_one ) mp_val_too_big(mp, value(x));
  mp_free_node(mp, p,dep_node_size);
  if ( mp->cur_exp==x ) if ( mp->cur_type==mp_independent ) {
    mp->cur_exp=value(x); mp->cur_type=mp_known;
    mp_free_node(mp, x,value_node_size);
  }
} else { 
  mp_type(x)=mp_dependent; mp->dep_final=final_node; mp_new_dep(mp, x,p);
  if ( mp->cur_exp==x ) if ( mp->cur_type==mp_independent ) mp->cur_type=mp_dependent;
}

@ @<Divide list |p| by $2^n$@>=
{ 
  s=temp_head; mp_link(temp_head)=p; r=p;
  do {  
    if ( n>30 ) w=0;
    else w=value(r) / two_to_the(n);
    if ( (abs(w)<=half_fraction_threshold)&&(mp_info(r)!=null) ) {
      mp_link(s)=mp_link(r);
      mp_free_node(mp, r,dep_node_size);
    } else { 
      value(r)=w; s=r;
    }
    r=mp_link(s);
  } while (mp_info(s)!=null);
  p=mp_link(temp_head);
}

@ The |check_mem| procedure, which is used only when \MP\ is being
debugged, makes sure that the current dependency lists are well formed.

@<Check the list of linear dependencies@>=
q=dep_head; p=mp_link(q);
while ( p!=dep_head ) {
  if ( prev_dep(p)!=q ) {
    mp_print_nl(mp, "Bad PREVDEP at "); mp_print_int(mp, p);
@.Bad PREVDEP...@>
  }
  p=dep_list(p);
  while (1) {
    r=mp_info(p); q=p; p=mp_link(q);
    if ( r==null ) break;
    if ( value(mp_info(p))>=value(r) ) {
      mp_print_nl(mp, "Out of order at "); mp_print_int(mp, p);
@.Out of order...@>
    }
  }
}

@* \[25] Dynamic nonlinear equations.
Variables of numeric type are maintained by the general scheme of
independent, dependent, and known values that we have just studied;
and the components of pair and transform variables are handled in the
same way. But \MP\ also has five other types of values: \&{boolean},
\&{string}, \&{pen}, \&{path}, and \&{picture}; what about them?

Equations are allowed between nonlinear quantities, but only in a
simple form. Two variables that haven't yet been assigned values are
either equal to each other, or they're not.

Before a boolean variable has received a value, its type is |mp_unknown_boolean|;
similarly, there are variables whose type is |mp_unknown_string|, |mp_unknown_pen|,
|mp_unknown_path|, and |mp_unknown_picture|. In such cases the value is either
|null| (which means that no other variables are equivalent to this one), or
it points to another variable of the same undefined type. The pointers in the
latter case form a cycle of nodes, which we shall call a ``ring.''
Rings of undefined variables may include capsules, which arise as
intermediate results within expressions or as \&{expr} parameters to macros.

When one member of a ring receives a value, the same value is given to
all the other members. In the case of paths and pictures, this implies
making separate copies of a potentially large data structure; users should
restrain their enthusiasm for such generality, unless they have lots and
lots of memory space.

@ The following procedure is called when a capsule node is being
added to a ring (e.g., when an unknown variable is mentioned in an expression).

@c 
static pointer mp_new_ring_entry (MP mp,pointer p) {
  pointer q; /* the new capsule node */
  q=mp_get_node(mp, value_node_size); mp_name_type(q)=mp_capsule;
  mp_type(q)=mp_type(p);
  if ( value(p)==null ) value(q)=p; else value(q)=value(p);
  value(p)=q;
  return q;
}

@ Conversely, we might delete a capsule or a variable before it becomes known.
The following procedure simply detaches a quantity from its ring,
without recycling the storage.

@<Declarations@>=
static void mp_ring_delete (MP mp,pointer p);

@ @c
void mp_ring_delete (MP mp,pointer p) {
  pointer q; 
  q=value(p);
  if ( q!=null ) if ( q!=p ){ 
    while ( value(q)!=p ) q=value(q);
    value(q)=value(p);
  }
}

@ Eventually there might be an equation that assigns values to all of the
variables in a ring. The |nonlinear_eq| subroutine does the necessary
propagation of values.

If the parameter |flush_p| is |true|, node |p| itself needn't receive a
value, it will soon be recycled.

@c 
static void mp_nonlinear_eq (MP mp,integer v, pointer p, boolean flush_p) {
  quarterword t; /* the type of ring |p| */
  pointer q,r; /* link manipulation registers */
  t=(quarterword)(mp_type(p)-unknown_tag); q=value(p);
  if ( flush_p ) mp_type(p)=mp_vacuous; else p=q;
  do {  
    r=value(q); mp_type(q)=t;
    switch (t) {
    case mp_boolean_type: value(q)=v; break;
    case mp_string_type: value(q)=v; add_str_ref(v); break;
    case mp_pen_type: value(q)=copy_pen(v); break;
    case mp_path_type: value(q)=mp_copy_path(mp, v); break;
    case mp_picture_type: value(q)=v; add_edge_ref(v); break;
    } /* there ain't no more cases */
    q=r;
  } while (q!=p);
}

@ If two members of rings are equated, and if they have the same type,
the |ring_merge| procedure is called on to make them equivalent.

@c 
static void mp_ring_merge (MP mp,pointer p, pointer q) {
  pointer r; /* traverses one list */
  r=value(p);
  while ( r!=p ) {
    if ( r==q ) {
      @<Exclaim about a redundant equation@>;
      return;
    };
    r=value(r);
  }
  r=value(p); value(p)=value(q); value(q)=r;
}

@ @<Exclaim about a redundant equation@>=
{ 
  print_err("Redundant equation");
@.Redundant equation@>
  help2("I already knew that this equation was true.",
        "But perhaps no harm has been done; let's continue.");
  mp_put_get_error(mp);
}

@* \[26] Introduction to the syntactic routines.
Let's pause a moment now and try to look at the Big Picture.
The \MP\ program consists of three main parts: syntactic routines,
semantic routines, and output routines. The chief purpose of the
syntactic routines is to deliver the user's input to the semantic routines,
while parsing expressions and locating operators and operands. The
semantic routines act as an interpreter responding to these operators,
which may be regarded as commands. And the output routines are
periodically called on to produce compact font descriptions that can be
used for typesetting or for making interim proof drawings. We have
discussed the basic data structures and many of the details of semantic
operations, so we are good and ready to plunge into the part of \MP\ that
actually controls the activities.

Our current goal is to come to grips with the |get_next| procedure,
which is the keystone of \MP's input mechanism. Each call of |get_next|
sets the value of three variables |cur_cmd|, |cur_mod|, and |cur_sym|,
representing the next input token.
$$\vbox{\halign{#\hfil\cr
  \hbox{|cur_cmd| denotes a command code from the long list of codes
   given earlier;}\cr
  \hbox{|cur_mod| denotes a modifier of the command code;}\cr
  \hbox{|cur_sym| is the hash address of the symbolic token that was
   just scanned,}\cr
  \hbox{\qquad or zero in the case of a numeric or string
   or capsule token.}\cr}}$$
Underlying this external behavior of |get_next| is all the machinery
necessary to convert from character files to tokens. At a given time we
may be only partially finished with the reading of several files (for
which \&{input} was specified), and partially finished with the expansion
of some user-defined macros and/or some macro parameters, and partially
finished reading some text that the user has inserted online,
and so on. When reading a character file, the characters must be
converted to tokens; comments and blank spaces must
be removed, numeric and string tokens must be evaluated.

To handle these situations, which might all be present simultaneously,
\MP\ uses various stacks that hold information about the incomplete
activities, and there is a finite state control for each level of the
input mechanism. These stacks record the current state of an implicitly
recursive process, but the |get_next| procedure is not recursive.

@<Glob...@>=
integer cur_cmd; /* current command set by |get_next| */
integer cur_mod; /* operand of current command */
halfword cur_sym; /* hash address of current symbol */

@ The |print_cmd_mod| routine prints a symbolic interpretation of a
command code and its modifier.
It consists of a rather tedious sequence of print
commands, and most of it is essentially an inverse to the |primitive|
routine that enters a \MP\ primitive into |hash| and |eqtb|. Therefore almost
all of this procedure appears elsewhere in the program, together with the
corresponding |primitive| calls.

@<Declarations@>=
static void mp_print_cmd_mod (MP mp,integer c, integer m) ;

@ @c
void mp_print_cmd_mod (MP mp,integer c, integer m) { 
 switch (c) {
  @<Cases of |print_cmd_mod| for symbolic printing of primitives@>
  default: mp_print(mp, "[unknown command code!]"); break;
  }
}

@ Here is a procedure that displays a given command in braces, in the
user's transcript file.

@d show_cur_cmd_mod mp_show_cmd_mod(mp, mp->cur_cmd,mp->cur_mod)

@c 
static void mp_show_cmd_mod (MP mp,integer c, integer m) { 
  mp_begin_diagnostic(mp); mp_print_nl(mp, "{");
  mp_print_cmd_mod(mp, c,m); mp_print_char(mp, xord('}'));
  mp_end_diagnostic(mp, false);
}

@* \[27] Input stacks and states.
The state of \MP's input mechanism appears in the input stack, whose
entries are records with five fields, called |index|, |start|, |loc|,
|limit|, and |name|. The top element of this stack is maintained in a
global variable for which no subscripting needs to be done; the other
elements of the stack appear in an array. Hence the stack is declared thus:

@<Types...@>=
typedef struct {
  char *long_name_field;
  halfword start_field, loc_field, limit_field, name_field;
  quarterword index_field;
} in_state_record;

@ @<Glob...@>=
in_state_record *input_stack;
integer input_ptr; /* first unused location of |input_stack| */
integer max_in_stack; /* largest value of |input_ptr| when pushing */
in_state_record cur_input; /* the ``top'' input state */
int stack_size; /* maximum number of simultaneous input sources */

@ @<Allocate or initialize ...@>=
mp->stack_size = 300;
mp->input_stack = xmalloc((mp->stack_size+1),sizeof(in_state_record));

@ @<Dealloc variables@>=
xfree(mp->input_stack);

@ We've already defined the special variable |loc==cur_input.loc_field|
in our discussion of basic input-output routines. The other components of
|cur_input| are defined in the same way:

@d iindex mp->cur_input.index_field /* reference for buffer information */
@d start mp->cur_input.start_field /* starting position in |buffer| */
@d limit mp->cur_input.limit_field /* end of current line in |buffer| */
@d name mp->cur_input.name_field /* name of the current file */

@ Let's look more closely now at the five control variables
(|index|,~|start|,~|loc|,~|limit|,~|name|),
assuming that \MP\ is reading a line of characters that have been input
from some file or from the user's terminal. There is an array called
|buffer| that acts as a stack of all lines of characters that are
currently being read from files, including all lines on subsidiary
levels of the input stack that are not yet completed. \MP\ will return to
the other lines when it is finished with the present input file.

(Incidentally, on a machine with byte-oriented addressing, it would be
appropriate to combine |buffer| with the |str_pool| array,
letting the buffer entries grow downward from the top of the string pool
and checking that these two tables don't bump into each other.)

The line we are currently working on begins in position |start| of the
buffer; the next character we are about to read is |buffer[loc]|; and
|limit| is the location of the last character present. We always have
|loc<=limit|. For convenience, |buffer[limit]| has been set to |"%"|, so
that the end of a line is easily sensed.

The |name| variable is a string number that designates the name of
the current file, if we are reading an ordinary text file.  Special codes
|is_term..max_spec_src| indicate other sources of input text.

@d is_term 0 /* |name| value when reading from the terminal for normal input */
@d is_read 1 /* |name| value when executing a \&{readstring} or \&{readfrom} */
@d is_scantok 2 /* |name| value when reading text generated by \&{scantokens} */
@d max_spec_src is_scantok

@ Additional information about the current line is available via the
|index| variable, which counts how many lines of characters are present
in the buffer below the current level. We have |index=0| when reading
from the terminal and prompting the user for each line; then if the user types,
e.g., `\.{input figs}', we will have |index=1| while reading
the file \.{figs.mp}. However, it does not follow that |index| is the
same as the input stack pointer, since many of the levels on the input
stack may come from token lists and some |index| values may correspond
to \.{MPX} files that are not currently on the stack.

The global variable |in_open| is equal to the highest |index| value counting
\.{MPX} files but excluding token-list input levels.  Thus, the number of
partially read lines in the buffer is |in_open+1| and we have |in_open>=index|
when we are not reading a token list.

If we are not currently reading from the terminal,
we are reading from the file variable |input_file[index]|. We use
the notation |terminal_input| as a convenient abbreviation for |name=is_term|,
and |cur_file| as an abbreviation for |input_file[index]|.

When \MP\ is not reading from the terminal, the global variable |line| contains
the line number in the current file, for use in error messages. More precisely,
|line| is a macro for |line_stack[index]| and the |line_stack| array gives
the line number for each file in the |input_file| array.

When an \.{MPX} file is opened the file name is stored in the |mpx_name|
array so that the name doesn't get lost when the file is temporarily removed
from the input stack.
Thus when |input_file[k]| is an \.{MPX} file, its name is |mpx_name[k]|
and it contains translated \TeX\ pictures for |input_file[k-1]|.
Since this is not an \.{MPX} file, we have
$$ \hbox{|mpx_name[k-1]<=absent|}. $$
This |name| field is set to |finished| when |input_file[k]| is completely
read.

If more information about the input state is needed, it can be
included in small arrays like those shown here. For example,
the current page or segment number in the input file might be put
into a variable |page|, that is really a macro for the current entry
in `\ignorespaces|page_stack:array[0..max_in_open] of integer|\unskip'
by analogy with |line_stack|.
@^system dependencies@>

@d terminal_input (name==is_term) /* are we reading from the terminal? */
@d cur_file mp->input_file[iindex] /* the current |void *| variable */
@d line mp->line_stack[iindex] /* current line number in the current source file */
@d in_ext mp->inext_stack[iindex] /* a string used to construct \.{MPX} file names */
@d in_name mp->iname_stack[iindex] /* a string used to construct \.{MPX} file names */
@d in_area mp->iarea_stack[iindex] /* another string for naming \.{MPX} files */
@d absent 1 /* |name_field| value for unused |mpx_in_stack| entries */
@d mpx_reading (mp->mpx_name[iindex]>absent)
  /* when reading a file, is it an \.{MPX} file? */
@d mpx_finished 0
  /* |name_field| value when the corresponding \.{MPX} file is finished */

@<Glob...@>=
integer in_open; /* the number of lines in the buffer, less one */
integer in_open_max; /* highest value of |in_open| ever seen */
unsigned int open_parens; /* the number of open text files */
void  * *input_file ;
integer *line_stack ; /* the line number for each file */
char *  *inext_stack; /* used for naming \.{MPX} files */
char *  *iname_stack; /* used for naming \.{MPX} files */
char *  *iarea_stack; /* used for naming \.{MPX} files */
halfword*mpx_name  ;

@ @<Allocate or ...@>=
mp->input_file  = xmalloc((mp->max_in_open+1),sizeof(void *));
mp->line_stack  = xmalloc((mp->max_in_open+1),sizeof(integer));
mp->inext_stack = xmalloc((mp->max_in_open+1),sizeof(char *));
mp->iname_stack = xmalloc((mp->max_in_open+1),sizeof(char *));
mp->iarea_stack = xmalloc((mp->max_in_open+1),sizeof(char *));
mp->mpx_name    = xmalloc((mp->max_in_open+1),sizeof(halfword));
{
  int k;
  for (k=0;k<=mp->max_in_open;k++) {
    mp->inext_stack[k] =NULL;
    mp->iname_stack[k] =NULL;
    mp->iarea_stack[k] =NULL;
  }
}

@ @<Dealloc variables@>=
{
  int l;
  for (l=0;l<=mp->max_in_open;l++) {
    xfree(mp->inext_stack[l]);
    xfree(mp->iname_stack[l]);
    xfree(mp->iarea_stack[l]);
  }
}
xfree(mp->input_file);
xfree(mp->line_stack);
xfree(mp->inext_stack);
xfree(mp->iname_stack);
xfree(mp->iarea_stack);
xfree(mp->mpx_name);


@ However, all this discussion about input state really applies only to the
case that we are inputting from a file. There is another important case,
namely when we are currently getting input from a token list. In this case
|iindex>max_in_open|, and the conventions about the other state variables
are different:

\yskip\hang|loc| is a pointer to the current node in the token list, i.e.,
the node that will be read next. If |loc=null|, the token list has been
fully read.

\yskip\hang|start| points to the first node of the token list; this node
may or may not contain a reference count, depending on the type of token
list involved.

\yskip\hang|token_type|, which takes the place of |iindex| in the
discussion above, is a code number that explains what kind of token list
is being scanned.

\yskip\hang|name| points to the |eqtb| address of the control sequence
being expanded, if the current token list is a macro not defined by
\&{vardef}. Macros defined by \&{vardef} have |name=null|; their name
can be deduced by looking at their first two parameters.

\yskip\hang|param_start|, which takes the place of |limit|, tells where
the parameters of the current macro or loop text begin in the |param_stack|.

\yskip\noindent The |token_type| can take several values, depending on
where the current token list came from:

\yskip
\indent|forever_text|, if the token list being scanned is the body of
a \&{forever} loop;

\indent|loop_text|, if the token list being scanned is the body of
a \&{for} or \&{forsuffixes} loop;

\indent|parameter|, if a \&{text} or \&{suffix} parameter is being scanned;

\indent|backed_up|, if the token list being scanned has been inserted as
`to be read again'.

\indent|inserted|, if the token list being scanned has been inserted as
part of error recovery;

\indent|macro|, if the expansion of a user-defined symbolic token is being
scanned.

\yskip\noindent
The token list begins with a reference count if and only if |token_type=
macro|.
@^reference counts@>

@d token_type iindex /* type of current token list */
@d token_state (iindex>(int)mp->max_in_open) /* are we scanning a token list? */
@d file_state (iindex<=(int)mp->max_in_open) /* are we scanning a file line? */
@d param_start limit /* base of macro parameters in |param_stack| */
@d forever_text (mp->max_in_open+1) /* |token_type| code for loop texts */
@d loop_text (mp->max_in_open+2) /* |token_type| code for loop texts */
@d parameter (mp->max_in_open+3) /* |token_type| code for parameter texts */
@d backed_up (mp->max_in_open+4) /* |token_type| code for texts to be reread */
@d inserted (mp->max_in_open+5) /* |token_type| code for inserted texts */
@d macro (mp->max_in_open+6) /* |token_type| code for macro replacement texts */

@ The |param_stack| is an auxiliary array used to hold pointers to the token
lists for parameters at the current level and subsidiary levels of input.
This stack grows at a different rate from the others.

@<Glob...@>=
pointer *param_stack;  /* token list pointers for parameters */
integer param_ptr; /* first unused entry in |param_stack| */
integer max_param_stack;  /* largest value of |param_ptr| */

@ @<Allocate or initialize ...@>=
mp->param_stack = xmalloc((mp->param_size+1),sizeof(pointer));

@ @<Dealloc variables@>=
xfree(mp->param_stack);

@ Notice that the |line| isn't valid when |token_state| is true because it
depends on |iindex|.  If we really need to know the line number for the
topmost file in the iindex stack we use the following function.  If a page
number or other information is needed, this routine should be modified to
compute it as well.
@^system dependencies@>

@<Declarations@>=
static integer mp_true_line (MP mp) ;

@ @c
integer mp_true_line (MP mp) {
  int k; /* an index into the input stack */
  if ( file_state && (name>max_spec_src) ) {
    return line;
  } else { 
    k=mp->input_ptr;
    while ((k>0) &&
           ((mp->input_stack[(k-1)].index_field>mp->max_in_open)||
            (mp->input_stack[(k-1)].name_field<=max_spec_src))) {
      decr(k);
    }
    return (k>0 ? mp->line_stack[(k-1)] : 0 );
  }
}

@ Thus, the ``current input state'' can be very complicated indeed; there
can be many levels and each level can arise in a variety of ways. The
|show_context| procedure, which is used by \MP's error-reporting routine to
print out the current input state on all levels down to the most recent
line of characters from an input file, illustrates most of these conventions.
The global variable |file_ptr| contains the lowest level that was
displayed by this procedure.

@<Glob...@>=
integer file_ptr; /* shallowest level shown by |show_context| */

@ The status at each level is indicated by printing two lines, where the first
line indicates what was read so far and the second line shows what remains
to be read. The context is cropped, if necessary, so that the first line
contains at most |half_error_line| characters, and the second contains
at most |error_line|. Non-current input levels whose |token_type| is
`|backed_up|' are shown only if they have not been fully read.

@c void mp_show_context (MP mp) { /* prints where the scanner is */
  unsigned old_setting; /* saved |selector| setting */
  @<Local variables for formatting calculations@>
  mp->file_ptr=mp->input_ptr; mp->input_stack[mp->file_ptr]=mp->cur_input;
  /* store current state */
  while (1) { 
    mp->cur_input=mp->input_stack[mp->file_ptr]; /* enter into the context */
    @<Display the current context@>;
    if ( file_state )
      if ( (name>max_spec_src) || (mp->file_ptr==0) ) break;
    decr(mp->file_ptr);
  }
  mp->cur_input=mp->input_stack[mp->input_ptr]; /* restore original state */
}

@ @<Display the current context@>=
if ( (mp->file_ptr==mp->input_ptr) || file_state ||
   (token_type!=backed_up) || (loc!=null) ) {
    /* we omit backed-up token lists that have already been read */
  mp->tally=0; /* get ready to count characters */
  old_setting=mp->selector;
  if ( file_state ) {
    @<Print location of current line@>;
    @<Pseudoprint the line@>;
  } else { 
    @<Print type of token list@>;
    @<Pseudoprint the token list@>;
  }
  mp->selector=old_setting; /* stop pseudoprinting */
  @<Print two lines using the tricky pseudoprinted information@>;
}

@ This routine should be changed, if necessary, to give the best possible
indication of where the current line resides in the input file.
For example, on some systems it is best to print both a page and line number.
@^system dependencies@>

@<Print location of current line@>=
if ( name>max_spec_src ) {
  mp_print_nl(mp, "l."); mp_print_int(mp, mp_true_line(mp));
} else if ( terminal_input ) {
  if ( mp->file_ptr==0 ) mp_print_nl(mp, "<*>");
  else mp_print_nl(mp, "<insert>");
} else if ( name==is_scantok ) {
  mp_print_nl(mp, "<scantokens>");
} else {
  mp_print_nl(mp, "<read>");
}
mp_print_char(mp, xord(' '))

@ Can't use case statement here because the |token_type| is not
a constant expression.

@<Print type of token list@>=
{
  if(token_type==forever_text) {
    mp_print_nl(mp, "<forever> ");
  } else if (token_type==loop_text) {
    @<Print the current loop value@>;
  } else if (token_type==parameter) {
    mp_print_nl(mp, "<argument> "); 
  } else if (token_type==backed_up) { 
    if ( loc==null ) mp_print_nl(mp, "<recently read> ");
    else mp_print_nl(mp, "<to be read again> ");
  } else if (token_type==inserted) {
    mp_print_nl(mp, "<inserted text> ");
  } else if (token_type==macro) {
    mp_print_ln(mp);
    if ( name!=null ) mp_print_text(name);
    else @<Print the name of a \&{vardef}'d macro@>;
    mp_print(mp, "->");
  } else {
    mp_print_nl(mp, "?");/* this should never happen */
@.?\relax@>
  }
}

@ The parameter that corresponds to a loop text is either a token list
(in the case of \&{forsuffixes}) or a ``capsule'' (in the case of \&{for}).
We'll discuss capsules later; for now, all we need to know is that
the |link| field in a capsule parameter is |void| and that
|print_exp(p,0)| displays the value of capsule~|p| in abbreviated form.

@<Print the current loop value@>=
{ mp_print_nl(mp, "<for("); p=mp->param_stack[param_start];
  if ( p!=null ) {
    if ( mp_link(p)==mp_void ) mp_print_exp(mp, p,0); /* we're in a \&{for} loop */
    else mp_show_token_list(mp, p,null,20,mp->tally);
  }
  mp_print(mp, ")> ");
}

@ The first two parameters of a macro defined by \&{vardef} will be token
lists representing the macro's prefix and ``at point.'' By putting these
together, we get the macro's full name.

@<Print the name of a \&{vardef}'d macro@>=
{ p=mp->param_stack[param_start];
  if ( p==null ) {
    mp_show_token_list(mp, mp->param_stack[param_start+1],null,20,mp->tally);
  } else { 
    q=p;
    while ( mp_link(q)!=null ) q=mp_link(q);
    mp_link(q)=mp->param_stack[param_start+1];
    mp_show_token_list(mp, p,null,20,mp->tally);
    mp_link(q)=null;
  }
}

@ Now it is necessary to explain a little trick. We don't want to store a long
string that corresponds to a token list, because that string might take up
lots of memory; and we are printing during a time when an error message is
being given, so we dare not do anything that might overflow one of \MP's
tables. So `pseudoprinting' is the answer: We enter a mode of printing
that stores characters into a buffer of length |error_line|, where character
$k+1$ is placed into \hbox{|trick_buf[k mod error_line]|} if
|k<trick_count|, otherwise character |k| is dropped. Initially we set
|tally:=0| and |trick_count:=1000000|; then when we reach the
point where transition from line 1 to line 2 should occur, we
set |first_count:=tally| and |trick_count:=@tmax@>(error_line,
tally+1+error_line-half_error_line)|. At the end of the
pseudoprinting, the values of |first_count|, |tally|, and
|trick_count| give us all the information we need to print the two lines,
and all of the necessary text is in |trick_buf|.

Namely, let |l| be the length of the descriptive information that appears
on the first line. The length of the context information gathered for that
line is |k=first_count|, and the length of the context information
gathered for line~2 is $m=\min(|tally|, |trick_count|)-k$. If |l+k<=h|,
where |h=half_error_line|, we print |trick_buf[0..k-1]| after the
descriptive information on line~1, and set |n:=l+k|; here |n| is the
length of line~1. If $l+k>h$, some cropping is necessary, so we set |n:=h|
and print `\.{...}' followed by
$$\hbox{|trick_buf[(l+k-h+3)..k-1]|,}$$
where subscripts of |trick_buf| are circular modulo |error_line|. The
second line consists of |n|~spaces followed by |trick_buf[k..(k+m-1)]|,
unless |n+m>error_line|; in the latter case, further cropping is done.
This is easier to program than to explain.

@<Local variables for formatting...@>=
int i; /* index into |buffer| */
integer l; /* length of descriptive information on line 1 */
integer m; /* context information gathered for line 2 */
int n; /* length of line 1 */
integer p; /* starting or ending place in |trick_buf| */
integer q; /* temporary index */

@ The following code tells the print routines to gather
the desired information.

@d begin_pseudoprint { 
  l=mp->tally; mp->tally=0; mp->selector=pseudo;
  mp->trick_count=1000000;
}
@d set_trick_count {
  mp->first_count=mp->tally;
  mp->trick_count=mp->tally+1+mp->error_line-mp->half_error_line;
  if ( mp->trick_count<mp->error_line ) mp->trick_count=mp->error_line;
}

@ And the following code uses the information after it has been gathered.

@<Print two lines using the tricky pseudoprinted information@>=
if ( mp->trick_count==1000000 ) set_trick_count;
  /* |set_trick_count| must be performed */
if ( mp->tally<mp->trick_count ) m=mp->tally-mp->first_count;
else m=mp->trick_count-mp->first_count; /* context on line 2 */
if ( l+mp->first_count<=mp->half_error_line ) {
  p=0; n=l+mp->first_count;
} else  { 
  mp_print(mp, "..."); p=l+mp->first_count-mp->half_error_line+3;
  n=mp->half_error_line;
}
for (q=p;q<=mp->first_count-1;q++) {
  mp_print_char(mp, mp->trick_buf[q % mp->error_line]);
}
mp_print_ln(mp);
for (q=1;q<=n;q++) {
  mp_print_char(mp, xord(' ')); /* print |n| spaces to begin line~2 */
}
if ( m+n<=mp->error_line ) p=mp->first_count+m; 
else p=mp->first_count+(mp->error_line-n-3);
for (q=mp->first_count;q<=p-1;q++) {
  mp_print_char(mp, mp->trick_buf[q % mp->error_line]);
}
if ( m+n>mp->error_line ) mp_print(mp, "...")

@ But the trick is distracting us from our current goal, which is to
understand the input state. So let's concentrate on the data structures that
are being pseudoprinted as we finish up the |show_context| procedure.

@<Pseudoprint the line@>=
begin_pseudoprint;
if ( limit>0 ) {
  for (i=start;i<=limit-1;i++) {
    if ( i==loc ) set_trick_count;
    mp_print_str(mp, mp->buffer[i]);
  }
}

@ @<Pseudoprint the token list@>=
begin_pseudoprint;
if ( token_type!=macro ) mp_show_token_list(mp, start,loc,100000,0);
else mp_show_macro(mp, start,loc,100000)

@ Here is the missing piece of |show_token_list| that is activated when the
token beginning line~2 is about to be shown:

@<Do magic computation@>=set_trick_count

@* \[28] Maintaining the input stacks.
The following subroutines change the input status in commonly needed ways.

First comes |push_input|, which stores the current state and creates a
new level (having, initially, the same properties as the old).

@d push_input  { /* enter a new input level, save the old */
  if ( mp->input_ptr>mp->max_in_stack ) {
    mp->max_in_stack=mp->input_ptr;
    if ( mp->input_ptr==mp->stack_size ) {
      int l = (mp->stack_size+(mp->stack_size/4));
      XREALLOC(mp->input_stack, l, in_state_record);
      mp->stack_size = l;
    }         
  }
  mp->input_stack[mp->input_ptr]=mp->cur_input; /* stack the record */
  incr(mp->input_ptr);
}

@ And of course what goes up must come down.

@d pop_input { /* leave an input level, re-enter the old */
    decr(mp->input_ptr); mp->cur_input=mp->input_stack[mp->input_ptr];
  }

@ Here is a procedure that starts a new level of token-list input, given
a token list |p| and its type |t|. If |t=macro|, the calling routine should
set |name|, reset~|loc|, and increase the macro's reference count.

@d back_list(A) mp_begin_token_list(mp, (A), (quarterword)backed_up) /* backs up a simple token list */

@c 
static void mp_begin_token_list (MP mp,pointer p, quarterword t)  { 
  push_input; start=p; token_type=t;
  param_start=mp->param_ptr; loc=p;
}

@ When a token list has been fully scanned, the following computations
should be done as we leave that level of input.
@^inner loop@>

@c 
static void mp_end_token_list (MP mp) { /* leave a token-list input level */
  pointer p; /* temporary register */
  if ( token_type>=backed_up ) { /* token list to be deleted */
    if ( token_type<=inserted ) { 
      mp_flush_token_list(mp, start); goto DONE;
    } else {
      mp_delete_mac_ref(mp, start); /* update reference count */
    }
  }
  while ( mp->param_ptr>param_start ) { /* parameters must be flushed */
    decr(mp->param_ptr);
    p=mp->param_stack[mp->param_ptr];
    if ( p!=null ) {
      if ( mp_link(p)==mp_void ) { /* it's an \&{expr} parameter */
        mp_recycle_value(mp, p); mp_free_node(mp, p,value_node_size);
      } else {
        mp_flush_token_list(mp, p); /* it's a \&{suffix} or \&{text} parameter */
      }
    }
  }
DONE: 
  pop_input; check_interrupt;
}

@ The contents of |cur_cmd,cur_mod,cur_sym| are placed into an equivalent
token by the |cur_tok| routine.
@^inner loop@>

@c @<Declare the procedure called |make_exp_copy|@>
static pointer mp_cur_tok (MP mp) {
  pointer p; /* a new token node */
  quarterword save_type; /* |cur_type| to be restored */
  integer save_exp; /* |cur_exp| to be restored */
  if ( mp->cur_sym==0 ) {
    if ( mp->cur_cmd==capsule_token ) {
      save_type=mp->cur_type; save_exp=mp->cur_exp;
      mp_make_exp_copy(mp, mp->cur_mod); p=mp_stash_cur_exp(mp); mp_link(p)=null;
      mp->cur_type=save_type; mp->cur_exp=save_exp;
    } else { 
      p=mp_get_node(mp, token_node_size);
      value(p)=mp->cur_mod; mp_name_type(p)=mp_token;
      if ( mp->cur_cmd==numeric_token ) mp_type(p)=mp_known;
      else mp_type(p)=mp_string_type;
    }
  } else { 
    fast_get_avail(p); mp_info(p)=mp->cur_sym;
  }
  return p;
}

@ Sometimes \MP\ has read too far and wants to ``unscan'' what it has
seen. The |back_input| procedure takes care of this by putting the token
just scanned back into the input stream, ready to be read again.
If |cur_sym<>0|, the values of |cur_cmd| and |cur_mod| are irrelevant.

@<Declarations@>= 
static void mp_back_input (MP mp);

@ @c void mp_back_input (MP mp) {/* undoes one token of input */
  pointer p; /* a token list of length one */
  p=mp_cur_tok(mp);
  while ( token_state &&(loc==null) ) 
    mp_end_token_list(mp); /* conserve stack space */
  back_list(p);
}

@ The |back_error| routine is used when we want to restore or replace an
offending token just before issuing an error message.  We disable interrupts
during the call of |back_input| so that the help message won't be lost.

@ @c static void mp_back_error (MP mp) { /* back up one token and call |error| */
  mp->OK_to_interrupt=false; 
  mp_back_input(mp); 
  mp->OK_to_interrupt=true; mp_error(mp);
}
static void mp_ins_error (MP mp) { /* back up one inserted token and call |error| */
  mp->OK_to_interrupt=false; 
  mp_back_input(mp); token_type=(quarterword)inserted;
  mp->OK_to_interrupt=true; mp_error(mp);
}

@ The |begin_file_reading| procedure starts a new level of input for lines
of characters to be read from a file, or as an insertion from the
terminal. It does not take care of opening the file, nor does it set |loc|
or |limit| or |line|.
@^system dependencies@>

@c void mp_begin_file_reading (MP mp) { 
  if ( mp->in_open==mp->max_in_open ) 
    mp_overflow(mp, "text input levels",mp->max_in_open);
@:MetaPost capacity exceeded text input levels}{\quad text input levels@>
  if ( mp->first==mp->buf_size ) 
    mp_reallocate_buffer(mp,(mp->buf_size+(mp->buf_size/4)));
  mp->in_open++; push_input; iindex=(quarterword)mp->in_open;
  if (mp->in_open_max<mp->in_open)
    mp->in_open_max=mp->in_open;
  mp->mpx_name[iindex]=absent;
  start=(halfword)mp->first;
  name=is_term; /* |terminal_input| is now |true| */
}

@ Conversely, the variables must be downdated when such a level of input
is finished.  Any associated \.{MPX} file must also be closed and popped
off the file stack.

@c static void mp_end_file_reading (MP mp) { 
  if ( mp->in_open>iindex ) {
    if ( (mp->mpx_name[mp->in_open]==absent)||(name<=max_spec_src) ) {
      mp_confusion(mp, "endinput");
@:this can't happen endinput}{\quad endinput@>
    } else { 
      (mp->close_file)(mp,mp->input_file[mp->in_open]); /* close an \.{MPX} file */
      delete_str_ref(mp->mpx_name[mp->in_open]);
      decr(mp->in_open);
    }
  }
  mp->first=(size_t)start;
  if ( iindex!=mp->in_open ) mp_confusion(mp, "endinput");
  if ( name>max_spec_src ) {
    (mp->close_file)(mp,cur_file);
    delete_str_ref(name);
    xfree(in_ext); 
    xfree(in_name); 
    xfree(in_area);
  }
  pop_input; decr(mp->in_open);
}

@ Here is a function that tries to resume input from an \.{MPX} file already
associated with the current input file.  It returns |false| if this doesn't
work.

@c static boolean mp_begin_mpx_reading (MP mp) { 
  if ( mp->in_open!=iindex+1 ) {
     return false;
  } else { 
    if ( mp->mpx_name[mp->in_open]<=absent ) mp_confusion(mp, "mpx");
@:this can't happen mpx}{\quad mpx@>
    if ( mp->first==mp->buf_size ) 
      mp_reallocate_buffer(mp,(mp->buf_size+(mp->buf_size/4)));
    push_input; iindex=(quarterword)mp->in_open;
    start=(halfword)mp->first;
    name=mp->mpx_name[mp->in_open]; add_str_ref(name);
    @<Put an empty line in the input buffer@>;
    return true;
  }
}

@ This procedure temporarily stops reading an \.{MPX} file.

@c static void mp_end_mpx_reading (MP mp) { 
  if ( mp->in_open!=iindex ) mp_confusion(mp, "mpx");
@:this can't happen mpx}{\quad mpx@>
  if ( loc<limit ) {
    @<Complain that we are not at the end of a line in the \.{MPX} file@>;
  }
  mp->first=(size_t)start;
  pop_input;
}

@ Here we enforce a restriction that simplifies the input stacks considerably.
This should not inconvenience the user because \.{MPX} files are generated
by an auxiliary program called \.{DVItoMP}.

@ @<Complain that we are not at the end of a line in the \.{MPX} file@>=
{ 
print_err("`mpxbreak' must be at the end of a line");
help4("This file contains picture expressions for btex...etex",
  "blocks.  Such files are normally generated automatically",
  "but this one seems to be messed up.  I'm going to ignore",
  "the rest of this line.");
mp_error(mp);
}

@ In order to keep the stack from overflowing during a long sequence of
inserted `\.{show}' commands, the following routine removes completed
error-inserted lines from memory.

@c void mp_clear_for_error_prompt (MP mp) { 
  while ( file_state && terminal_input &&
    (mp->input_ptr>0)&&(loc==limit) ) mp_end_file_reading(mp);
  mp_print_ln(mp); clear_terminal;
}

@ To get \MP's whole input mechanism going, we perform the following
actions.

@<Initialize the input routines@>=
{ mp->input_ptr=0; mp->max_in_stack=0;
  mp->in_open=0; mp->open_parens=0; mp->max_buf_stack=0;
  mp->param_ptr=0; mp->max_param_stack=0;
  mp->first=0;
  start=0; iindex=0; line=0; name=is_term;
  mp->mpx_name[0]=absent;
  mp->force_eof=false;
  if ( ! mp_init_terminal(mp) ) mp_jump_out(mp);
  limit=(halfword)mp->last; mp->first=mp->last+1; 
  /* |init_terminal| has set |loc| and |last| */
}

@* \[29] Getting the next token.
The heart of \MP's input mechanism is the |get_next| procedure, which
we shall develop in the next few sections of the program. Perhaps we
shouldn't actually call it the ``heart,'' however; it really acts as \MP's
eyes and mouth, reading the source files and gobbling them up. And it also
helps \MP\ to regurgitate stored token lists that are to be processed again.

The main duty of |get_next| is to input one token and to set |cur_cmd|
and |cur_mod| to that token's command code and modifier. Furthermore, if
the input token is a symbolic token, that token's |hash| address
is stored in |cur_sym|; otherwise |cur_sym| is set to zero.

Underlying this simple description is a certain amount of complexity
because of all the cases that need to be handled.
However, the inner loop of |get_next| is reasonably short and fast.

@ Before getting into |get_next|, we need to consider a mechanism by which
\MP\ helps keep errors from propagating too far. Whenever the program goes
into a mode where it keeps calling |get_next| repeatedly until a certain
condition is met, it sets |scanner_status| to some value other than |normal|.
Then if an input file ends, or if an `\&{outer}' symbol appears,
an appropriate error recovery will be possible.

The global variable |warning_info| helps in this error recovery by providing
additional information. For example, |warning_info| might indicate the
name of a macro whose replacement text is being scanned.

@d normal 0 /* |scanner_status| at ``quiet times'' */
@d skipping 1 /* |scanner_status| when false conditional text is being skipped */
@d flushing 2 /* |scanner_status| when junk after a statement is being ignored */
@d absorbing 3 /* |scanner_status| when a \&{text} parameter is being scanned */
@d var_defining 4 /* |scanner_status| when a \&{vardef} is being scanned */
@d op_defining 5 /* |scanner_status| when a macro \&{def} is being scanned */
@d loop_defining 6 /* |scanner_status| when a \&{for} loop is being scanned */
@d tex_flushing 7 /* |scanner_status| when skipping \TeX\ material */

@<Glob...@>=
integer scanner_status; /* are we scanning at high speed? */
integer warning_info; /* if so, what else do we need to know,
    in case an error occurs? */

@ @<Initialize the input routines@>=
mp->scanner_status=normal;

@ The following subroutine
is called when an `\&{outer}' symbolic token has been scanned or
when the end of a file has been reached. These two cases are distinguished
by |cur_sym|, which is zero at the end of a file.

@c
static boolean mp_check_outer_validity (MP mp) {
  pointer p; /* points to inserted token list */
  if ( mp->scanner_status==normal ) {
    return true;
  } else if ( mp->scanner_status==tex_flushing ) {
    @<Check if the file has ended while flushing \TeX\ material and set the
      result value for |check_outer_validity|@>;
  } else { 
    mp->deletions_allowed=false;
    @<Back up an outer symbolic token so that it can be reread@>;
    if ( mp->scanner_status>skipping ) {
      @<Tell the user what has run away and try to recover@>;
    } else { 
      print_err("Incomplete if; all text was ignored after line ");
@.Incomplete if...@>
      mp_print_int(mp, mp->warning_info);
      help3("A forbidden `outer' token occurred in skipped text.",
        "This kind of error happens when you say `if...' and forget",
        "the matching `fi'. I've inserted a `fi'; this might work.");
      if ( mp->cur_sym==0 ) 
        mp->help_line[2]="The file ended while I was skipping conditional text.";
      mp->cur_sym=frozen_fi; mp_ins_error(mp);
    }
    mp->deletions_allowed=true; 
	return false;
  }
}

@ @<Check if the file has ended while flushing \TeX\ material and set...@>=
if ( mp->cur_sym!=0 ) { 
   return true;
} else { 
  mp->deletions_allowed=false;
  print_err("TeX mode didn't end; all text was ignored after line ");
  mp_print_int(mp, mp->warning_info);
  help2("The file ended while I was looking for the `etex' to",
        "finish this TeX material.  I've inserted `etex' now.");
  mp->cur_sym = frozen_etex;
  mp_ins_error(mp);
  mp->deletions_allowed=true;
  return false;
}

@ @<Back up an outer symbolic token so that it can be reread@>=
if ( mp->cur_sym!=0 ) {
  p=mp_get_avail(mp); mp_info(p)=mp->cur_sym;
  back_list(p); /* prepare to read the symbolic token again */
}

@ @<Tell the user what has run away...@>=
{ 
  mp_runaway(mp); /* print the definition-so-far */
  if ( mp->cur_sym==0 ) {
    print_err("File ended");
@.File ended while scanning...@>
  } else { 
    print_err("Forbidden token found");
@.Forbidden token found...@>
  }
  mp_print(mp, " while scanning ");
  help4("I suspect you have forgotten an `enddef',",
    "causing me to read past where you wanted me to stop.",
    "I'll try to recover; but if the error is serious,",
    "you'd better type `E' or `X' now and fix your file.");
  switch (mp->scanner_status) {
    @<Complete the error message,
      and set |cur_sym| to a token that might help recover from the error@>
  } /* there are no other cases */
  mp_ins_error(mp);
}

@ As we consider various kinds of errors, it is also appropriate to
change the first line of the help message just given; |help_line[3]|
points to the string that might be changed.

@<Complete the error message,...@>=
case flushing: 
  mp_print(mp, "to the end of the statement");
  mp->help_line[3]="A previous error seems to have propagated,";
  mp->cur_sym=frozen_semicolon;
  break;
case absorbing: 
  mp_print(mp, "a text argument");
  mp->help_line[3]="It seems that a right delimiter was left out,";
  if ( mp->warning_info==0 ) {
    mp->cur_sym=frozen_end_group;
  } else { 
    mp->cur_sym=frozen_right_delimiter;
    equiv(frozen_right_delimiter)=mp->warning_info;
  }
  break;
case var_defining:
case op_defining: 
  mp_print(mp, "the definition of ");
  if ( mp->scanner_status==op_defining ) 
     mp_print_text(mp->warning_info);
  else 
     mp_print_variable_name(mp, mp->warning_info);
  mp->cur_sym=frozen_end_def;
  break;
case loop_defining: 
  mp_print(mp, "the text of a "); 
  mp_print_text(mp->warning_info);
  mp_print(mp, " loop");
  mp->help_line[3]="I suspect you have forgotten an `endfor',";
  mp->cur_sym=frozen_end_for;
  break;

@ The |runaway| procedure displays the first part of the text that occurred
when \MP\ began its special |scanner_status|, if that text has been saved.

@<Declarations@>=
static void mp_runaway (MP mp) ;

@ @c
void mp_runaway (MP mp) { 
  if ( mp->scanner_status>flushing ) { 
     mp_print_nl(mp, "Runaway ");
 	 switch (mp->scanner_status) { 
	 case absorbing: mp_print(mp, "text?"); break;
	 case var_defining: 
     case op_defining: mp_print(mp,"definition?"); break;
     case loop_defining: mp_print(mp, "loop?"); break;
     } /* there are no other cases */
     mp_print_ln(mp); 
     mp_show_token_list(mp, mp_link(hold_head),null,mp->error_line-10,0);
  }
}

@ We need to mention a procedure that may be called by |get_next|.

@<Declarations@>= 
static void mp_firm_up_the_line (MP mp);

@ And now we're ready to take the plunge into |get_next| itself.
Note that the behavior depends on the |scanner_status| because percent signs
and double quotes need to be passed over when skipping TeX material.

@c 
void mp_get_next (MP mp) {
  /* sets |cur_cmd|, |cur_mod|, |cur_sym| to next token */
@^inner loop@>
  /*restart*/ /* go here to get the next input token */
  /*exit*/ /* go here when the next input token has been got */
  /*|common_ending|*/ /* go here to finish getting a symbolic token */
  /*found*/ /* go here when the end of a symbolic token has been found */
  /*switch*/ /* go here to branch on the class of an input character */
  /*|start_numeric_token|,|start_decimal_token|,|fin_numeric_token|,|done|*/
    /* go here at crucial stages when scanning a number */
  int k; /* an index into |buffer| */
  ASCII_code c; /* the current character in the buffer */
  int class; /* its class number */
  integer n,f; /* registers for decimal-to-binary conversion */
RESTART: 
  mp->cur_sym=0;
  if ( file_state ) {
    @<Input from external file; |goto restart| if no input found,
    or |return| if a non-symbolic token is found@>;
  } else {
    @<Input from token list; |goto restart| if end of list or
      if a parameter needs to be expanded,
      or |return| if a non-symbolic token is found@>;
  }
COMMON_ENDING: 
  @<Finish getting the symbolic token in |cur_sym|;
   |goto restart| if it is illegal@>;
}

@ When a symbolic token is declared to be `\&{outer}', its command code
is increased by |outer_tag|.
@^inner loop@>

@<Finish getting the symbolic token in |cur_sym|...@>=
mp->cur_cmd=eq_type(mp->cur_sym); mp->cur_mod=equiv(mp->cur_sym);
if ( mp->cur_cmd>=outer_tag ) {
  if ( mp_check_outer_validity(mp) ) 
    mp->cur_cmd=mp->cur_cmd-outer_tag;
  else 
    goto RESTART;
}

@ A percent sign appears in |buffer[limit]|; this makes it unnecessary
to have a special test for end-of-line.
@^inner loop@>

@<Input from external file;...@>=
{ 
SWITCH: 
  c=mp->buffer[loc]; incr(loc); class=mp->char_class[c];
  switch (class) {
  case digit_class: goto START_NUMERIC_TOKEN; break;
  case period_class: 
    class=mp->char_class[mp->buffer[loc]];
    if ( class>period_class ) {
      goto SWITCH;
    } else if ( class<period_class ) { /* |class=digit_class| */
      n=0; goto START_DECIMAL_TOKEN;
    }
@:. }{\..\ token@>
    break;
  case space_class: goto SWITCH; break;
  case percent_class: 
    if ( mp->scanner_status==tex_flushing ) {
      if ( loc<limit ) goto SWITCH;
    }
    @<Move to next line of file, or |goto restart| if there is no next line@>;
    check_interrupt;
    goto SWITCH;
    break;
  case string_class: 
    if ( mp->scanner_status==tex_flushing ) goto SWITCH;
    else @<Get a string token and |return|@>;
    break;
  case isolated_classes: 
    k=loc-1; goto FOUND; break;
  case invalid_class: 
    if ( mp->scanner_status==tex_flushing ) goto SWITCH;
    else @<Decry the invalid character and |goto restart|@>;
    break;
  default: break; /* letters, etc. */
  }
  k=loc-1;
  while ( mp->char_class[mp->buffer[loc]]==class ) incr(loc);
  goto FOUND;
START_NUMERIC_TOKEN:
  @<Get the integer part |n| of a numeric token;
    set |f:=0| and |goto fin_numeric_token| if there is no decimal point@>;
START_DECIMAL_TOKEN:
  @<Get the fraction part |f| of a numeric token@>;
FIN_NUMERIC_TOKEN:
  @<Pack the numeric and fraction parts of a numeric token
    and |return|@>;
FOUND: 
  mp->cur_sym=mp_id_lookup(mp, k,loc-k);
}

@ We go to |restart| instead of to |SWITCH|, because we might enter
|token_state| after the error has been dealt with
(cf.\ |clear_for_error_prompt|).

@<Decry the invalid...@>=
{ 
  print_err("Text line contains an invalid character");
@.Text line contains...@>
  help2("A funny symbol that I can\'t read has just been input.",
        "Continue, and I'll forget that it ever happened.");
  mp->deletions_allowed=false; mp_error(mp); mp->deletions_allowed=true;
  goto RESTART;
}

@ @<Get a string token and |return|@>=
{ 
  if ( mp->buffer[loc]=='"' ) {
    mp->cur_mod=null_str;
  } else { 
    k=loc; mp->buffer[limit+1]=xord('"');
    do {  
     incr(loc);
    } while (mp->buffer[loc]!='"');
    if ( loc>limit ) {
      @<Decry the missing string delimiter and |goto restart|@>;
    }
    if ( loc==k+1 ) {
      mp->cur_mod=mp->buffer[k];
    } else { 
      str_room(loc-k);
      do {  
        append_char(mp->buffer[k]); incr(k);
      } while (k!=loc);
      mp->cur_mod=mp_make_string(mp);
    }
  }
  incr(loc); mp->cur_cmd=string_token; 
  return;
}

@ We go to |restart| after this error message, not to |SWITCH|,
because the |clear_for_error_prompt| routine might have reinstated
|token_state| after |error| has finished.

@<Decry the missing string delimiter and |goto restart|@>=
{ 
  loc=limit; /* the next character to be read on this line will be |"%"| */
  print_err("Incomplete string token has been flushed");
@.Incomplete string token...@>
  help3("Strings should finish on the same line as they began.",
    "I've deleted the partial string; you might want to",
    "insert another by typing, e.g., `I\"new string\"'.");
  mp->deletions_allowed=false; mp_error(mp);
  mp->deletions_allowed=true; 
  goto RESTART;
}

@ @<Get the integer part |n| of a numeric token...@>=
n=c-'0';
while ( mp->char_class[mp->buffer[loc]]==digit_class ) {
  if ( n<32768 ) n=10*n+mp->buffer[loc]-'0';
  incr(loc);
}
if ( mp->buffer[loc]=='.' ) 
  if ( mp->char_class[mp->buffer[loc+1]]==digit_class ) 
    goto DONE;
f=0; 
goto FIN_NUMERIC_TOKEN;
DONE: incr(loc)

@ @<Get the fraction part |f| of a numeric token@>=
k=0;
do { 
  if ( k<17 ) { /* digits for |k>=17| cannot affect the result */
    mp->dig[k]=(unsigned char)(mp->buffer[loc]-'0'); incr(k);
  }
  incr(loc);
} while (mp->char_class[mp->buffer[loc]]==digit_class);
f=mp_round_decimals(mp, (quarterword)k);
if ( f==unity ) {
  incr(n); f=0;
}

@ @<Pack the numeric and fraction parts of a numeric token and |return|@>=
if ( n<32768 ) {
  @<Set |cur_mod:=n*unity+f| and check if it is uncomfortably large@>;
} else if ( mp->scanner_status!=tex_flushing ) {
  print_err("Enormous number has been reduced");
@.Enormous number...@>
  help2("I can\'t handle numbers bigger than 32767.99998;",
        "so I've changed your constant to that maximum amount.");
  mp->deletions_allowed=false; mp_error(mp); mp->deletions_allowed=true;
  mp->cur_mod=el_gordo;
}
mp->cur_cmd=numeric_token; return

@ @<Set |cur_mod:=n*unity+f| and check if it is uncomfortably large@>=
{ 
  mp->cur_mod=n*unity+f;
  if ( mp->cur_mod>=fraction_one ) {
    if ( (mp->internal[mp_warning_check]>0) &&
         (mp->scanner_status!=tex_flushing) ) {
      print_err("Number is too large (");
      mp_print_scaled(mp, mp->cur_mod);
      mp_print_char(mp, xord(')'));
      help3("It is at least 4096. Continue and I'll try to cope",
      "with that big value; but it might be dangerous.",
      "(Set warningcheck:=0 to suppress this message.)");
      mp_error(mp);
    }
  }
}

@ Let's consider now what happens when |get_next| is looking at a token list.
@^inner loop@>

@<Input from token list;...@>=
if ( loc>=mp->hi_mem_min ) { /* one-word token */
  mp->cur_sym=mp_info(loc); loc=mp_link(loc); /* move to next */
  if ( mp->cur_sym>=expr_base ) {
    if ( mp->cur_sym>=suffix_base ) {
      @<Insert a suffix or text parameter and |goto restart|@>;
    } else { 
      mp->cur_cmd=capsule_token;
      mp->cur_mod=mp->param_stack[param_start+mp->cur_sym-(expr_base)];
      mp->cur_sym=0; return;
    }
  }
} else if ( loc>null ) {
  @<Get a stored numeric or string or capsule token and |return|@>
} else { /* we are done with this token list */
  mp_end_token_list(mp); goto RESTART; /* resume previous level */
}

@ @<Insert a suffix or text parameter...@>=
{ 
  if ( mp->cur_sym>=text_base ) mp->cur_sym=mp->cur_sym-mp->param_size;
  /* |param_size=text_base-suffix_base| */
  mp_begin_token_list(mp,
                      mp->param_stack[param_start+mp->cur_sym-(suffix_base)],
                      (quarterword)parameter);
  goto RESTART;
}

@ @<Get a stored numeric or string or capsule token...@>=
{ 
  if ( mp_name_type(loc)==mp_token ) {
    mp->cur_mod=value(loc);
    if ( mp_type(loc)==mp_known ) {
      mp->cur_cmd=numeric_token;
    } else { 
      mp->cur_cmd=string_token; add_str_ref(mp->cur_mod);
    }
  } else { 
    mp->cur_mod=loc; mp->cur_cmd=capsule_token;
  };
  loc=mp_link(loc); return;
}

@ All of the easy branches of |get_next| have now been taken care of.
There is one more branch.

@<Move to next line of file, or |goto restart|...@>=
if ( name>max_spec_src) {
  @<Read next line of file into |buffer|, or
    |goto restart| if the file has ended@>;
} else { 
  if ( mp->input_ptr>0 ) {
     /* text was inserted during error recovery or by \&{scantokens} */
    mp_end_file_reading(mp); goto RESTART; /* resume previous level */
  }
  if (mp->job_name == NULL && ( mp->selector<log_only || mp->selector>=write_file))  
    mp_open_log_file(mp);
  if ( mp->interaction>mp_nonstop_mode ) {
    if ( limit==start ) /* previous line was empty */
      mp_print_nl(mp, "(Please type a command or say `end')");
@.Please type...@>
    mp_print_ln(mp); mp->first=(size_t)start;
    prompt_input("*"); /* input on-line into |buffer| */
@.*\relax@>
    limit=(halfword)mp->last; mp->buffer[limit]=xord('%');
    mp->first=(size_t)(limit+1); loc=start;
  } else {
    mp_fatal_error(mp, "*** (job aborted, no legal end found)");
@.job aborted@>
    /* nonstop mode, which is intended for overnight batch processing,
       never waits for on-line input */
  }
}

@ The global variable |force_eof| is normally |false|; it is set |true|
by an \&{endinput} command.

@<Glob...@>=
boolean force_eof; /* should the next \&{input} be aborted early? */

@ We must decrement |loc| in order to leave the buffer in a valid state
when an error condition causes us to |goto restart| without calling
|end_file_reading|.

@<Read next line of file into |buffer|, or
  |goto restart| if the file has ended@>=
{ 
  incr(line); mp->first=(size_t)start;
  if ( ! mp->force_eof ) {
    if ( mp_input_ln(mp, cur_file ) ) /* not end of file */
      mp_firm_up_the_line(mp); /* this sets |limit| */
    else 
      mp->force_eof=true;
  };
  if ( mp->force_eof ) {
    mp->force_eof=false;
    decr(loc);
    if ( mpx_reading ) {
      @<Complain that the \.{MPX} file ended unexpectly; then set
        |cur_sym:=frozen_mpx_break| and |goto comon_ending|@>;
    } else { 
      mp_print_char(mp, xord(')')); decr(mp->open_parens);
      update_terminal; /* show user that file has been read */
      mp_end_file_reading(mp); /* resume previous level */
      if ( mp_check_outer_validity(mp) ) goto  RESTART;  
      else goto RESTART;
    }
  }
  mp->buffer[limit]=xord('%'); mp->first=(size_t)(limit+1); loc=start; /* ready to read */
}

@ We should never actually come to the end of an \.{MPX} file because such
files should have an \&{mpxbreak} after the translation of the last
\&{btex}$\,\ldots\,$\&{etex} block.

@<Complain that the \.{MPX} file ended unexpectly; then set...@>=
{ 
  mp->mpx_name[iindex]=mpx_finished;
  print_err("mpx file ended unexpectedly");
  help4("The file had too few picture expressions for btex...etex",
    "blocks.  Such files are normally generated automatically",
    "but this one got messed up.  You might want to insert a",
    "picture expression now.");
  mp->deletions_allowed=false; mp_error(mp); mp->deletions_allowed=true;
  mp->cur_sym=frozen_mpx_break; goto COMMON_ENDING;
}

@ Sometimes we want to make it look as though we have just read a blank line
without really doing so.

@<Put an empty line in the input buffer@>=
mp->last=mp->first; limit=(halfword)mp->last; 
  /* simulate |input_ln| and |firm_up_the_line| */
mp->buffer[limit]=xord('%'); mp->first=(size_t)(limit+1); loc=start

@ If the user has set the |mp_pausing| parameter to some positive value,
and if nonstop mode has not been selected, each line of input is displayed
on the terminal and the transcript file, followed by `\.{=>}'.
\MP\ waits for a response. If the response is null (i.e., if nothing is
typed except perhaps a few blank spaces), the original
line is accepted as it stands; otherwise the line typed is
used instead of the line in the file.

@c void mp_firm_up_the_line (MP mp) {
  size_t k; /* an index into |buffer| */
  limit=(halfword)mp->last;
  if ((!mp->noninteractive)   
      && (mp->internal[mp_pausing]>0 )
      && (mp->interaction>mp_nonstop_mode )) {
    wake_up_terminal; mp_print_ln(mp);
    if ( start<limit ) {
      for (k=(size_t)start;k<(size_t)limit;k++) {
        mp_print_str(mp, mp->buffer[k]);
      } 
    }
    mp->first=(size_t)limit; prompt_input("=>"); /* wait for user response */
@.=>@>
    if ( mp->last>mp->first ) {
      for (k=mp->first;k<mp->last;k++) { /* move line down in buffer */
        mp->buffer[k+(size_t)start-mp->first]=mp->buffer[k];
      }
      limit=(halfword)((size_t)start+mp->last-mp->first);
    }
  }
}

@* \[30] Dealing with \TeX\ material.
The \&{btex}$\,\ldots\,$\&{etex} and \&{verbatimtex}$\,\ldots\,$\&{etex}
features need to be implemented at a low level in the scanning process
so that \MP\ can stay in synch with the a preprocessor that treats
blocks of \TeX\ material as they occur in the input file without trying
to expand \MP\ macros.  Thus we need a special version of |get_next|
that does not expand macros and such but does handle \&{btex},
\&{verbatimtex}, etc.

The special version of |get_next| is called |get_t_next|.  It works by flushing
\&{btex}$\,\ldots\,$\&{etex} and \&{verbatimtex}\allowbreak
$\,\ldots\,$\&{etex} blocks, switching to the \.{MPX} file when it sees
\&{btex}, and switching back when it sees \&{mpxbreak}.

@d btex_code 0
@d verbatim_code 1

@ @<Put each...@>=
mp_primitive(mp, "btex",start_tex,btex_code);
@:btex_}{\&{btex} primitive@>
mp_primitive(mp, "verbatimtex",start_tex,verbatim_code);
@:verbatimtex_}{\&{verbatimtex} primitive@>
mp_primitive(mp, "etex",etex_marker,0); mp->eqtb[frozen_etex]=mp->eqtb[mp->cur_sym];
@:etex_}{\&{etex} primitive@>
mp_primitive(mp, "mpxbreak",mpx_break,0); mp->eqtb[frozen_mpx_break]=mp->eqtb[mp->cur_sym];
@:mpx_break_}{\&{mpxbreak} primitive@>

@ @<Cases of |print_cmd...@>=
case start_tex: if ( m==btex_code ) mp_print(mp, "btex");
  else mp_print(mp, "verbatimtex"); break;
case etex_marker: mp_print(mp, "etex"); break;
case mpx_break: mp_print(mp, "mpxbreak"); break;

@ Actually, |get_t_next| is a macro that avoids procedure overhead except
in the unusual case where \&{btex}, \&{verbatimtex}, \&{etex}, or \&{mpxbreak}
is encountered.

@d get_t_next {mp_get_next(mp); if ( mp->cur_cmd<=max_pre_command ) mp_t_next(mp); }

@<Declarations@>=
static void mp_start_mpx_input (MP mp);

@ @c 
static void mp_t_next (MP mp) {
  int old_status; /* saves the |scanner_status| */
  integer old_info; /* saves the |warning_info| */
  while ( mp->cur_cmd<=max_pre_command ) {
    if ( mp->cur_cmd==mpx_break ) {
      if ( ! file_state || (mp->mpx_name[iindex]==absent) ) {
        @<Complain about a misplaced \&{mpxbreak}@>;
      } else { 
        mp_end_mpx_reading(mp); 
      	goto TEX_FLUSH;
      }
    } else if ( mp->cur_cmd==start_tex ) {
      if ( token_state || (name<=max_spec_src) ) {
        @<Complain that we are not reading a file@>;
      } else if ( mpx_reading ) {
        @<Complain that \.{MPX} files cannot contain \TeX\ material@>;
      } else if ( (mp->cur_mod!=verbatim_code)&&
                  (mp->mpx_name[iindex]!=mpx_finished) ) {
        if ( ! mp_begin_mpx_reading(mp) ) mp_start_mpx_input(mp);
      } else {
        goto TEX_FLUSH;
      }
    } else {
       @<Complain about a misplaced \&{etex}@>;
    }
    goto COMMON_ENDING;
  TEX_FLUSH: 
    @<Flush the \TeX\ material@>;
  COMMON_ENDING: 
    mp_get_next(mp);
  }
}

@ We could be in the middle of an operation such as skipping false conditional
text when \TeX\ material is encountered, so we must be careful to save the
|scanner_status|.

@<Flush the \TeX\ material@>=
old_status=mp->scanner_status;
old_info=mp->warning_info;
mp->scanner_status=tex_flushing;
mp->warning_info=line;
do {  mp_get_next(mp); } while (mp->cur_cmd!=etex_marker);
mp->scanner_status=old_status;
mp->warning_info=old_info

@ @<Complain that \.{MPX} files cannot contain \TeX\ material@>=
{ print_err("An mpx file cannot contain btex or verbatimtex blocks");
help4("This file contains picture expressions for btex...etex",
  "blocks.  Such files are normally generated automatically",
  "but this one seems to be messed up.  I'll just keep going",
  "and hope for the best.");
mp_error(mp);
}

@ @<Complain that we are not reading a file@>=
{ print_err("You can only use `btex' or `verbatimtex' in a file");
help3("I'll have to ignore this preprocessor command because it",
  "only works when there is a file to preprocess.  You might",
  "want to delete everything up to the next `etex`.");
mp_error(mp);
}

@ @<Complain about a misplaced \&{mpxbreak}@>=
{ print_err("Misplaced mpxbreak");
help2("I'll ignore this preprocessor command because it",
      "doesn't belong here");
mp_error(mp);
}

@ @<Complain about a misplaced \&{etex}@>=
{ print_err("Extra etex will be ignored");
help1("There is no btex or verbatimtex for this to match");
mp_error(mp);
}

@* \[31] Scanning macro definitions.
\MP\ has a variety of ways to tuck tokens away into token lists for later
use: Macros can be defined with \&{def}, \&{vardef}, \&{primarydef}, etc.;
repeatable code can be defined with \&{for}, \&{forever}, \&{forsuffixes}.
All such operations are handled by the routines in this part of the program.

The modifier part of each command code is zero for the ``ending delimiters''
like \&{enddef} and \&{endfor}.

@d start_def 1 /* command modifier for \&{def} */
@d var_def 2 /* command modifier for \&{vardef} */
@d end_def 0 /* command modifier for \&{enddef} */
@d start_forever 1 /* command modifier for \&{forever} */
@d end_for 0 /* command modifier for \&{endfor} */

@<Put each...@>=
mp_primitive(mp, "def",macro_def,start_def);
@:def_}{\&{def} primitive@>
mp_primitive(mp, "vardef",macro_def,var_def);
@:var_def_}{\&{vardef} primitive@>
mp_primitive(mp, "primarydef",macro_def,secondary_primary_macro);
@:primary_def_}{\&{primarydef} primitive@>
mp_primitive(mp, "secondarydef",macro_def,tertiary_secondary_macro);
@:secondary_def_}{\&{secondarydef} primitive@>
mp_primitive(mp, "tertiarydef",macro_def,expression_tertiary_macro);
@:tertiary_def_}{\&{tertiarydef} primitive@>
mp_primitive(mp, "enddef",macro_def,end_def); mp->eqtb[frozen_end_def]=mp->eqtb[mp->cur_sym];
@:end_def_}{\&{enddef} primitive@>
@#
mp_primitive(mp, "for",iteration,expr_base);
@:for_}{\&{for} primitive@>
mp_primitive(mp, "forsuffixes",iteration,suffix_base);
@:for_suffixes_}{\&{forsuffixes} primitive@>
mp_primitive(mp, "forever",iteration,start_forever);
@:forever_}{\&{forever} primitive@>
mp_primitive(mp, "endfor",iteration,end_for); mp->eqtb[frozen_end_for]=mp->eqtb[mp->cur_sym];
@:end_for_}{\&{endfor} primitive@>

@ @<Cases of |print_cmd...@>=
case macro_def:
  if ( m<=var_def ) {
    if ( m==start_def ) mp_print(mp, "def");
    else if ( m<start_def ) mp_print(mp, "enddef");
    else mp_print(mp, "vardef");
  } else if ( m==secondary_primary_macro ) { 
    mp_print(mp, "primarydef");
  } else if ( m==tertiary_secondary_macro ) { 
    mp_print(mp, "secondarydef");
  } else { 
    mp_print(mp, "tertiarydef");
  }
  break;
case iteration: 
  if ( m<=start_forever ) {
    if ( m==start_forever ) mp_print(mp, "forever"); 
    else mp_print(mp, "endfor");
  } else if ( m==expr_base ) {
    mp_print(mp, "for"); 
  } else { 
    mp_print(mp, "forsuffixes");
  }
  break;

@ Different macro-absorbing operations have different syntaxes, but they
also have a lot in common. There is a list of special symbols that are to
be replaced by parameter tokens; there is a special command code that
ends the definition; the quotation conventions are identical.  Therefore
it makes sense to have most of the work done by a single subroutine. That
subroutine is called |scan_toks|.

The first parameter to |scan_toks| is the command code that will
terminate scanning (either |macro_def| or |iteration|).

The second parameter, |subst_list|, points to a (possibly empty) list
of two-word nodes whose |info| and |value| fields specify symbol tokens
before and after replacement. The list will be returned to free storage
by |scan_toks|.

The third parameter is simply appended to the token list that is built.
And the final parameter tells how many of the special operations
\.{\#\AT!}, \.{\AT!}, and \.{\AT!\#} are to be replaced by suffix parameters.
When such parameters are present, they are called \.{(SUFFIX0)},
\.{(SUFFIX1)}, and \.{(SUFFIX2)}.

@c static pointer mp_scan_toks (MP mp,command_code terminator, pointer 
  subst_list, pointer tail_end, quarterword suffix_count) {
  pointer p; /* tail of the token list being built */
  pointer q; /* temporary for link management */
  integer balance; /* left delimiters minus right delimiters */
  p=hold_head; balance=1; mp_link(hold_head)=null;
  while (1) { 
    get_t_next;
    if ( mp->cur_sym>0 ) {
      @<Substitute for |cur_sym|, if it's on the |subst_list|@>;
      if ( mp->cur_cmd==terminator ) {
        @<Adjust the balance; |break| if it's zero@>;
      } else if ( mp->cur_cmd==macro_special ) {
        @<Handle quoted symbols, \.{\#\AT!}, \.{\AT!}, or \.{\AT!\#}@>;
      }
    }
    mp_link(p)=mp_cur_tok(mp); p=mp_link(p);
  }
  mp_link(p)=tail_end; mp_flush_node_list(mp, subst_list);
  return mp_link(hold_head);
}

@ @<Substitute for |cur_sym|...@>=
{ 
  q=subst_list;
  while ( q!=null ) {
    if ( mp_info(q)==mp->cur_sym ) {
      mp->cur_sym=value(q); mp->cur_cmd=relax; break;
    }
    q=mp_link(q);
  }
}

@ @<Adjust the balance; |break| if it's zero@>=
if ( mp->cur_mod>0 ) {
  incr(balance);
} else { 
  decr(balance);
  if ( balance==0 )
    break;
}

@ Four commands are intended to be used only within macro texts: \&{quote},
\.{\#\AT!}, \.{\AT!}, and \.{\AT!\#}. They are variants of a single command
code called |macro_special|.

@d quote 0 /* |macro_special| modifier for \&{quote} */
@d macro_prefix 1 /* |macro_special| modifier for \.{\#\AT!} */
@d macro_at 2 /* |macro_special| modifier for \.{\AT!} */
@d macro_suffix 3 /* |macro_special| modifier for \.{\AT!\#} */

@<Put each...@>=
mp_primitive(mp, "quote",macro_special,quote);
@:quote_}{\&{quote} primitive@>
mp_primitive(mp, "#@@",macro_special,macro_prefix);
@:]]]\#\AT!_}{\.{\#\AT!} primitive@>
mp_primitive(mp, "@@",macro_special,macro_at);
@:]]]\AT!_}{\.{\AT!} primitive@>
mp_primitive(mp, "@@#",macro_special,macro_suffix);
@:]]]\AT!\#_}{\.{\AT!\#} primitive@>

@ @<Cases of |print_cmd...@>=
case macro_special: 
  switch (m) {
  case macro_prefix: mp_print(mp, "#@@"); break;
  case macro_at: mp_print_char(mp, xord('@@')); break;
  case macro_suffix: mp_print(mp, "@@#"); break;
  default: mp_print(mp, "quote"); break;
  }
  break;

@ @<Handle quoted...@>=
{ 
  if ( mp->cur_mod==quote ) { get_t_next; } 
  else if ( mp->cur_mod<=suffix_count ) 
    mp->cur_sym=suffix_base-1+mp->cur_mod;
}

@ Here is a routine that's used whenever a token will be redefined. If
the user's token is unredefinable, the `|frozen_inaccessible|' token is
substituted; the latter is redefinable but essentially impossible to use,
hence \MP's tables won't get fouled up.

@c static void mp_get_symbol (MP mp) { /* sets |cur_sym| to a safe symbol */
RESTART: 
  get_t_next;
  if ( (mp->cur_sym==0)||(mp->cur_sym>(integer)frozen_inaccessible) ) {
    print_err("Missing symbolic token inserted");
@.Missing symbolic token...@>
    help3("Sorry: You can\'t redefine a number, string, or expr.",
      "I've inserted an inaccessible symbol so that your",
      "definition will be completed without mixing me up too badly.");
    if ( mp->cur_sym>0 )
      mp->help_line[2]="Sorry: You can\'t redefine my error-recovery tokens.";
    else if ( mp->cur_cmd==string_token ) 
      delete_str_ref(mp->cur_mod);
    mp->cur_sym=frozen_inaccessible; mp_ins_error(mp); goto RESTART;
  }
}

@ Before we actually redefine a symbolic token, we need to clear away its
former value, if it was a variable. The following stronger version of
|get_symbol| does that.

@c static void mp_get_clear_symbol (MP mp) { 
  mp_get_symbol(mp); mp_clear_symbol(mp, mp->cur_sym,false);
}

@ Here's another little subroutine; it checks that an equals sign
or assignment sign comes along at the proper place in a macro definition.

@c static void mp_check_equals (MP mp) { 
  if ( mp->cur_cmd!=equals ) if ( mp->cur_cmd!=assignment ) {
     mp_missing_err(mp, "=");
@.Missing `='@>
    help5("The next thing in this `def' should have been `=',",
          "because I've already looked at the definition heading.",
          "But don't worry; I'll pretend that an equals sign",
          "was present. Everything from here to `enddef'",
          "will be the replacement text of this macro.");
    mp_back_error(mp);
  }
}

@ A \&{primarydef}, \&{secondarydef}, or \&{tertiarydef} is rather easily
handled now that we have |scan_toks|.  In this case there are
two parameters, which will be \.{EXPR0} and \.{EXPR1} (i.e.,
|expr_base| and |expr_base+1|).

@c static void mp_make_op_def (MP mp) {
  command_code m; /* the type of definition */
  pointer p,q,r; /* for list manipulation */
  m=mp->cur_mod;
  mp_get_symbol(mp); q=mp_get_node(mp, token_node_size);
  mp_info(q)=mp->cur_sym; value(q)=expr_base;
  mp_get_clear_symbol(mp); mp->warning_info=mp->cur_sym;
  mp_get_symbol(mp); p=mp_get_node(mp, token_node_size);
  mp_info(p)=mp->cur_sym; value(p)=expr_base+1; mp_link(p)=q;
  get_t_next; mp_check_equals(mp);
  mp->scanner_status=op_defining; q=mp_get_avail(mp); ref_count(q)=null;
  r=mp_get_avail(mp); mp_link(q)=r; mp_info(r)=general_macro;
  mp_link(r)=mp_scan_toks(mp, macro_def,p,null,0);
  mp->scanner_status=normal; eq_type(mp->warning_info)=m;
  equiv(mp->warning_info)=q; mp_get_x_next(mp);
}

@ Parameters to macros are introduced by the keywords \&{expr},
\&{suffix}, \&{text}, \&{primary}, \&{secondary}, and \&{tertiary}.

@<Put each...@>=
mp_primitive(mp, "expr",param_type,expr_base);
@:expr_}{\&{expr} primitive@>
mp_primitive(mp, "suffix",param_type,suffix_base);
@:suffix_}{\&{suffix} primitive@>
mp_primitive(mp, "text",param_type,text_base);
@:text_}{\&{text} primitive@>
mp_primitive(mp, "primary",param_type,primary_macro);
@:primary_}{\&{primary} primitive@>
mp_primitive(mp, "secondary",param_type,secondary_macro);
@:secondary_}{\&{secondary} primitive@>
mp_primitive(mp, "tertiary",param_type,tertiary_macro);
@:tertiary_}{\&{tertiary} primitive@>

@ @<Cases of |print_cmd...@>=
case param_type:
  if ( m>=expr_base ) {
    if ( m==expr_base ) mp_print(mp, "expr");
    else if ( m==suffix_base ) mp_print(mp, "suffix");
    else mp_print(mp, "text");
  } else if ( m<secondary_macro ) {
    mp_print(mp, "primary");
  } else if ( m==secondary_macro ) {
    mp_print(mp, "secondary");
  } else {
    mp_print(mp, "tertiary");
  }
  break;

@ Let's turn next to the more complex processing associated with \&{def}
and \&{vardef}. When the following procedure is called, |cur_mod|
should be either |start_def| or |var_def|.

Note that although the macro scanner allows |def = := enddef| and
|def := = enddef|; |def = = enddef| and |def := := enddef| will generate
an error because by the time the second of the two identical tokens is 
seen, its meaning has already become undefined.

@c 
static void mp_scan_def (MP mp) {
  int m; /* the type of definition */
  int n; /* the number of special suffix parameters */
  int k; /* the total number of parameters */
  int c; /* the kind of macro we're defining */
  pointer r; /* parameter-substitution list */
  pointer q; /* tail of the macro token list */
  pointer p; /* temporary storage */
  halfword base; /* |expr_base|, |suffix_base|, or |text_base| */
  pointer l_delim,r_delim; /* matching delimiters */
  m=mp->cur_mod; c=general_macro; mp_link(hold_head)=null;
  q=mp_get_avail(mp); ref_count(q)=null; r=null;
  @<Scan the token or variable to be defined;
    set |n|, |scanner_status|, and |warning_info|@>;
  k=n;
  if ( mp->cur_cmd==left_delimiter ) {
    @<Absorb delimited parameters, putting them into lists |q| and |r|@>;
  }
  if ( mp->cur_cmd==param_type ) {
    @<Absorb undelimited parameters, putting them into list |r|@>;
  }
  mp_check_equals(mp);
  p=mp_get_avail(mp); mp_info(p)=c; mp_link(q)=p;
  @<Attach the replacement text to the tail of node |p|@>;
  mp->scanner_status=normal; mp_get_x_next(mp);
}

@ We don't put `|frozen_end_group|' into the replacement text of
a \&{vardef}, because the user may want to redefine `\.{endgroup}'.

@<Attach the replacement text to the tail of node |p|@>=
if ( m==start_def ) {
  mp_link(p)=mp_scan_toks(mp, macro_def,r,null, (quarterword)n);
} else { 
  q=mp_get_avail(mp); mp_info(q)=mp->bg_loc; mp_link(p)=q;
  p=mp_get_avail(mp); mp_info(p)=mp->eg_loc;
  mp_link(q)=mp_scan_toks(mp, macro_def,r,p, (quarterword)n);
}
if ( mp->warning_info==bad_vardef ) 
  mp_flush_token_list(mp, value(bad_vardef))

@ @<Glob...@>=
int bg_loc;
int eg_loc; /* hash addresses of `\.{begingroup}' and `\.{endgroup}' */

@ @<Scan the token or variable to be defined;...@>=
if ( m==start_def ) {
  mp_get_clear_symbol(mp); mp->warning_info=mp->cur_sym; get_t_next;
  mp->scanner_status=op_defining; n=0;
  eq_type(mp->warning_info)=defined_macro; equiv(mp->warning_info)=q;
} else { 
  p=mp_scan_declared_variable(mp);
  mp_flush_variable(mp, equiv(mp_info(p)),mp_link(p),true);
  mp->warning_info=mp_find_variable(mp, p); mp_flush_list(mp, p);
  if ( mp->warning_info==null ) @<Change to `\.{a bad variable}'@>;
  mp->scanner_status=var_defining; n=2;
  if ( mp->cur_cmd==macro_special ) if ( mp->cur_mod==macro_suffix ) {/* \.{\AT!\#} */
    n=3; get_t_next;
  }
  mp_type(mp->warning_info)=(quarterword)(mp_unsuffixed_macro-2+n); 
  value(mp->warning_info)=q;
} /* |mp_suffixed_macro=mp_unsuffixed_macro+1| */

@ @<Change to `\.{a bad variable}'@>=
{ 
  print_err("This variable already starts with a macro");
@.This variable already...@>
  help2("After `vardef a' you can\'t say `vardef a.b'.",
        "So I'll have to discard this definition.");
  mp_error(mp); mp->warning_info=bad_vardef;
}

@ @<Initialize table entries...@>=
mp_name_type(bad_vardef)=mp_root; mp_link(bad_vardef)=frozen_bad_vardef;
equiv(frozen_bad_vardef)=bad_vardef; eq_type(frozen_bad_vardef)=tag_token;

@ @<Absorb delimited parameters, putting them into lists |q| and |r|@>=
do {  
  l_delim=mp->cur_sym; r_delim=mp->cur_mod; get_t_next;
  if ( (mp->cur_cmd==param_type)&&(mp->cur_mod>=expr_base) ) {
   base=mp->cur_mod;
  } else { 
    print_err("Missing parameter type; `expr' will be assumed");
@.Missing parameter type@>
    help1("You should've had `expr' or `suffix' or `text' here.");
    mp_back_error(mp); base=expr_base;
  }
  @<Absorb parameter tokens for type |base|@>;
  mp_check_delimiter(mp, l_delim,r_delim);
  get_t_next;
} while (mp->cur_cmd==left_delimiter)

@ @<Absorb parameter tokens for type |base|@>=
do { 
  mp_link(q)=mp_get_avail(mp); q=mp_link(q); mp_info(q)=base+k;
  mp_get_symbol(mp); p=mp_get_node(mp, token_node_size); 
  value(p)=base+k; mp_info(p)=mp->cur_sym;
  if ( k==mp->param_size ) mp_overflow(mp, "parameter stack size",mp->param_size);
@:MetaPost capacity exceeded parameter stack size}{\quad parameter stack size@>
  incr(k); mp_link(p)=r; r=p; get_t_next;
} while (mp->cur_cmd==comma)

@ @<Absorb undelimited parameters, putting them into list |r|@>=
{ 
  p=mp_get_node(mp, token_node_size);
  if ( mp->cur_mod<expr_base ) {
    c=mp->cur_mod; value(p)=expr_base+k;
  } else { 
    value(p)=mp->cur_mod+k;
    if ( mp->cur_mod==expr_base ) c=expr_macro;
    else if ( mp->cur_mod==suffix_base ) c=suffix_macro;
    else c=text_macro;
  }
  if ( k==mp->param_size ) mp_overflow(mp, "parameter stack size",mp->param_size);
  incr(k); mp_get_symbol(mp); mp_info(p)=mp->cur_sym; mp_link(p)=r; r=p; get_t_next;
  if ( c==expr_macro ) if ( mp->cur_cmd==of_token ) {
    c=of_macro; p=mp_get_node(mp, token_node_size);
    if ( k==mp->param_size ) mp_overflow(mp, "parameter stack size",mp->param_size);
    value(p)=expr_base+k; mp_get_symbol(mp); mp_info(p)=mp->cur_sym;
    mp_link(p)=r; r=p; get_t_next;
  }
}

@* \[32] Expanding the next token.
Only a few command codes |<min_command| can possibly be returned by
|get_t_next|; in increasing order, they are
|if_test|, |fi_or_else|, |input|, |iteration|, |repeat_loop|,
|exit_test|, |relax|, |scan_tokens|, |expand_after|, and |defined_macro|.

\MP\ usually gets the next token of input by saying |get_x_next|. This is
like |get_t_next| except that it keeps getting more tokens until
finding |cur_cmd>=min_command|. In other words, |get_x_next| expands
macros and removes conditionals or iterations or input instructions that
might be present.

It follows that |get_x_next| might invoke itself recursively. In fact,
there is massive recursion, since macro expansion can involve the
scanning of arbitrarily complex expressions, which in turn involve
macro expansion and conditionals, etc.
@^recursion@>

Therefore it's necessary to declare a whole bunch of |forward|
procedures at this point, and to insert some other procedures
that will be invoked by |get_x_next|.

@<Declarations@>= 
static void mp_scan_primary (MP mp);
static void mp_scan_secondary (MP mp);
static void mp_scan_tertiary (MP mp);
static void mp_scan_expression (MP mp);
static void mp_scan_suffix (MP mp);
static void mp_get_boolean (MP mp);
static void mp_pass_text (MP mp);
static void mp_conditional (MP mp);
static void mp_start_input (MP mp);
static void mp_begin_iteration (MP mp);
static void mp_resume_iteration (MP mp);
static void mp_stop_iteration (MP mp);

@ A recursion depth counter is used to discover infinite recursions.
(Near) infinite recursion is a problem because it translates into 
C function calls that eat up the available call stack. A better solution
would be to depend on signal trapping, but that is problematic when
Metapost is used as a library. 

@<Global...@>=
int expand_depth_count; /* current expansion depth */
int expand_depth; /* current expansion depth */

@ The limit is set at |10000|, which should be enough to allow 
normal usages of metapost while preventing the most obvious 
crashes on most all operating systems, but the value can be
raised if the runtime system allows a larger C stack.
@^system dependencies@>

@<Set initial...@>=
mp->expand_depth=10000;

@ Even better would be if the system allows
discovery of the amount of space available on the call stack.
@^system dependencies@>

@c
static void mp_check_expansion_depth (MP mp ){
  if (mp->expand_depth_count>=mp->expand_depth) {
    mp_overflow(mp, "expansion depth", mp->expand_depth);
  }
}

@ An auxiliary subroutine called |expand| is used by |get_x_next|
when it has to do exotic expansion commands.

@c 
static void mp_expand (MP mp) {
  pointer p; /* for list manipulation */
  size_t k; /* something that we hope is |<=buf_size| */
  pool_pointer j; /* index into |str_pool| */
  mp->expand_depth_count++;
  mp_check_expansion_depth(mp);
  if ( mp->internal[mp_tracing_commands]>unity ) 
    if ( mp->cur_cmd!=defined_macro )
      show_cur_cmd_mod;
  switch (mp->cur_cmd)  {
  case if_test:
    mp_conditional(mp); /* this procedure is discussed in Part 36 below */
    break;
  case fi_or_else:
    @<Terminate the current conditional and skip to \&{fi}@>;
    break;
  case input:
    @<Initiate or terminate input from a file@>;
    break;
  case iteration:
    if ( mp->cur_mod==end_for ) {
      @<Scold the user for having an extra \&{endfor}@>;
    } else {
      mp_begin_iteration(mp); /* this procedure is discussed in Part 37 below */
    }
    break;
  case repeat_loop: 
    @<Repeat a loop@>;
    break;
  case exit_test: 
    @<Exit a loop if the proper time has come@>;
    break;
  case relax: 
    break;
  case expand_after: 
    @<Expand the token after the next token@>;
    break;
  case scan_tokens: 
    @<Put a string into the input buffer@>;
    break;
  case defined_macro:
   mp_macro_call(mp, mp->cur_mod,null,mp->cur_sym);
   break;
  }; /* there are no other cases */
  mp->expand_depth_count--;
}

@ @<Scold the user...@>=
{ 
  print_err("Extra `endfor'");
@.Extra `endfor'@>
  help2("I'm not currently working on a for loop,",
        "so I had better not try to end anything.");
  mp_error(mp);
}

@ The processing of \&{input} involves the |start_input| subroutine,
which will be declared later; the processing of \&{endinput} is trivial.

@<Put each...@>=
mp_primitive(mp, "input",input,0);
@:input_}{\&{input} primitive@>
mp_primitive(mp, "endinput",input,1);
@:end_input_}{\&{endinput} primitive@>

@ @<Cases of |print_cmd_mod|...@>=
case input: 
  if ( m==0 ) mp_print(mp, "input");
  else mp_print(mp, "endinput");
  break;

@ @<Initiate or terminate input...@>=
if ( mp->cur_mod>0 ) mp->force_eof=true;
else mp_start_input(mp)

@ We'll discuss the complicated parts of loop operations later. For now
it suffices to know that there's a global variable called |loop_ptr|
that will be |null| if no loop is in progress.

@<Repeat a loop@>=
{ while ( token_state &&(loc==null) ) 
    mp_end_token_list(mp); /* conserve stack space */
  if ( mp->loop_ptr==null ) {
    print_err("Lost loop");
@.Lost loop@>
    help2("I'm confused; after exiting from a loop, I still seem",
          "to want to repeat it. I'll try to forget the problem.");
    mp_error(mp);
  } else {
    mp_resume_iteration(mp); /* this procedure is in Part 37 below */
  }
}

@ @<Exit a loop if the proper time has come@>=
{ mp_get_boolean(mp);
  if ( mp->internal[mp_tracing_commands]>unity ) 
    mp_show_cmd_mod(mp, nullary,mp->cur_exp);
  if ( mp->cur_exp==true_code ) {
    if ( mp->loop_ptr==null ) {
      print_err("No loop is in progress");
@.No loop is in progress@>
      help1("Why say `exitif' when there's nothing to exit from?");
      if ( mp->cur_cmd==semicolon ) mp_error(mp); else mp_back_error(mp);
    } else {
     @<Exit prematurely from an iteration@>;
    }
  } else if ( mp->cur_cmd!=semicolon ) {
    mp_missing_err(mp, ";");
@.Missing `;'@>
    help2("After `exitif <boolean exp>' I expect to see a semicolon.",
          "I shall pretend that one was there."); mp_back_error(mp);
  }
}

@ Here we use the fact that |forever_text| is the only |token_type| that
is less than |loop_text|.

@<Exit prematurely...@>=
{ p=null;
  do {  
    if ( file_state ) {
      mp_end_file_reading(mp);
    } else { 
      if ( token_type<=loop_text ) p=start;
      mp_end_token_list(mp);
    }
  } while (p==null);
  if ( p!=mp_info(mp->loop_ptr) ) mp_fatal_error(mp, "*** (loop confusion)");
@.loop confusion@>
  mp_stop_iteration(mp); /* this procedure is in Part 34 below */
}

@ @<Expand the token after the next token@>=
{ get_t_next;
  p=mp_cur_tok(mp); get_t_next;
  if ( mp->cur_cmd<min_command ) mp_expand(mp); 
  else mp_back_input(mp);
  back_list(p);
}

@ @<Put a string into the input buffer@>=
{ mp_get_x_next(mp); mp_scan_primary(mp);
  if ( mp->cur_type!=mp_string_type ) {
    mp_disp_err(mp, null,"Not a string");
@.Not a string@>
    help2("I'm going to flush this expression, since",
          "scantokens should be followed by a known string.");
    mp_put_get_flush_error(mp, 0);
  } else { 
    mp_back_input(mp);
    if ( length(mp->cur_exp)>0 )
       @<Pretend we're reading a new one-line file@>;
  }
}

@ @<Pretend we're reading a new one-line file@>=
{ mp_begin_file_reading(mp); name=is_scantok;
  k=mp->first+(size_t)length(mp->cur_exp);
  if ( k>=mp->max_buf_stack ) {
    while ( k>=mp->buf_size ) {
      mp_reallocate_buffer(mp,(mp->buf_size+(mp->buf_size/4)));
    }
    mp->max_buf_stack=k+1;
  }
  j=mp->str_start[mp->cur_exp]; limit=(halfword)k;
  while ( mp->first<(size_t)limit ) {
    mp->buffer[mp->first]=mp->str_pool[j]; incr(j); incr(mp->first);
  }
  mp->buffer[limit]=xord('%'); mp->first=(size_t)(limit+1); loc=start; 
  mp_flush_cur_exp(mp, 0);
}

@ Here finally is |get_x_next|.

The expression scanning routines to be considered later
communicate via the global quantities |cur_type| and |cur_exp|;
we must be very careful to save and restore these quantities while
macros are being expanded.
@^inner loop@>

@<Declarations@>=
static void mp_get_x_next (MP mp);

@ @c void mp_get_x_next (MP mp) {
  pointer save_exp; /* a capsule to save |cur_type| and |cur_exp| */
  get_t_next;
  if ( mp->cur_cmd<min_command ) {
    save_exp=mp_stash_cur_exp(mp);
    do {  
      if ( mp->cur_cmd==defined_macro ) 
        mp_macro_call(mp, mp->cur_mod,null,mp->cur_sym);
      else 
        mp_expand(mp);
      get_t_next;
     } while (mp->cur_cmd<min_command);
     mp_unstash_cur_exp(mp, save_exp); /* that restores |cur_type| and |cur_exp| */
  }
}

@ Now let's consider the |macro_call| procedure, which is used to start up
all user-defined macros. Since the arguments to a macro might be expressions,
|macro_call| is recursive.
@^recursion@>

The first parameter to |macro_call| points to the reference count of the
token list that defines the macro. The second parameter contains any
arguments that have already been parsed (see below).  The third parameter
points to the symbolic token that names the macro. If the third parameter
is |null|, the macro was defined by \&{vardef}, so its name can be
reconstructed from the prefix and ``at'' arguments found within the
second parameter.

What is this second parameter? It's simply a linked list of one-word items,
whose |info| fields point to the arguments. In other words, if |arg_list=null|,
no arguments have been scanned yet; otherwise |mp_info(arg_list)| points to
the first scanned argument, and |mp_link(arg_list)| points to the list of
further arguments (if any).

Arguments of type \&{expr} are so-called capsules, which we will
discuss later when we concentrate on expressions; they can be
recognized easily because their |link| field is |void|. Arguments of type
\&{suffix} and \&{text} are token lists without reference counts.

@ After argument scanning is complete, the arguments are moved to the
|param_stack|. (They can't be put on that stack any sooner, because
the stack is growing and shrinking in unpredictable ways as more arguments
are being acquired.)  Then the macro body is fed to the scanner; i.e.,
the replacement text of the macro is placed at the top of the \MP's
input stack, so that |get_t_next| will proceed to read it next.

@<Declarations@>=
static void mp_macro_call (MP mp,pointer def_ref, pointer arg_list, 
                    pointer macro_name) ;

@ @c
void mp_macro_call (MP mp,pointer def_ref, pointer arg_list, 
                    pointer macro_name) {
  /* invokes a user-defined control sequence */
  pointer r; /* current node in the macro's token list */
  pointer p,q; /* for list manipulation */
  integer n; /* the number of arguments */
  pointer tail = 0; /* tail of the argument list */
  pointer l_delim=0,r_delim=0; /* a delimiter pair */
  r=mp_link(def_ref); add_mac_ref(def_ref);
  if ( arg_list==null ) {
    n=0;
  } else {
   @<Determine the number |n| of arguments already supplied,
    and set |tail| to the tail of |arg_list|@>;
  }
  if ( mp->internal[mp_tracing_macros]>0 ) {
    @<Show the text of the macro being expanded, and the existing arguments@>;
  }
  @<Scan the remaining arguments, if any; set |r| to the first token
    of the replacement text@>;
  @<Feed the arguments and replacement text to the scanner@>;
}

@ @<Show the text of the macro...@>=
mp_begin_diagnostic(mp); mp_print_ln(mp); 
mp_print_macro_name(mp, arg_list,macro_name);
if ( n==3 ) mp_print(mp, "@@#"); /* indicate a suffixed macro */
mp_show_macro(mp, def_ref,null,100000);
if ( arg_list!=null ) {
  n=0; p=arg_list;
  do {  
    q=mp_info(p);
    mp_print_arg(mp, q,n,0);
    incr(n); p=mp_link(p);
  } while (p!=null);
}
mp_end_diagnostic(mp, false)


@ @<Declarations@>=
static void mp_print_macro_name (MP mp,pointer a, pointer n);

@ @c
void mp_print_macro_name (MP mp,pointer a, pointer n) {
  pointer p,q; /* they traverse the first part of |a| */
  if ( n!=null ) {
    mp_print_text(n);
  } else  { 
    p=mp_info(a);
    if ( p==null ) {
      mp_print_text(mp_info(mp_info(mp_link(a))));
    } else { 
      q=p;
      while ( mp_link(q)!=null ) q=mp_link(q);
      mp_link(q)=mp_info(mp_link(a));
      mp_show_token_list(mp, p,null,1000,0);
      mp_link(q)=null;
    }
  }
}

@ @<Declarations@>=
static void mp_print_arg (MP mp,pointer q, integer n, pointer b) ;

@ @c
void mp_print_arg (MP mp,pointer q, integer n, pointer b) {
  if ( mp_link(q)==mp_void ) mp_print_nl(mp, "(EXPR");
  else if ( (b<text_base)&&(b!=text_macro) ) mp_print_nl(mp, "(SUFFIX");
  else mp_print_nl(mp, "(TEXT");
  mp_print_int(mp, n); mp_print(mp, ")<-");
  if ( mp_link(q)==mp_void ) mp_print_exp(mp, q,1);
  else mp_show_token_list(mp, q,null,1000,0);
}

@ @<Determine the number |n| of arguments already supplied...@>=
{  
  n=1; tail=arg_list;
  while ( mp_link(tail)!=null ) { 
    incr(n); tail=mp_link(tail);
  }
}

@ @<Scan the remaining arguments, if any; set |r|...@>=
mp->cur_cmd=comma+1; /* anything |<>comma| will do */
while ( mp_info(r)>=expr_base ) { 
  @<Scan the delimited argument represented by |mp_info(r)|@>;
  r=mp_link(r);
}
if ( mp->cur_cmd==comma ) {
  print_err("Too many arguments to ");
@.Too many arguments...@>
  mp_print_macro_name(mp, arg_list,macro_name); mp_print_char(mp, xord(';'));
  mp_print_nl(mp, "  Missing `"); mp_print_text(r_delim);
@.Missing `)'...@>
  mp_print(mp, "' has been inserted");
  help3("I'm going to assume that the comma I just read was a",
   "right delimiter, and then I'll begin expanding the macro.",
   "You might want to delete some tokens before continuing.");
  mp_error(mp);
}
if ( mp_info(r)!=general_macro ) {
  @<Scan undelimited argument(s)@>;
}
r=mp_link(r)

@ At this point, the reader will find it advisable to review the explanation
of token list format that was presented earlier, paying special attention to
the conventions that apply only at the beginning of a macro's token list.

On the other hand, the reader will have to take the expression-parsing
aspects of the following program on faith; we will explain |cur_type|
and |cur_exp| later. (Several things in this program depend on each other,
and it's necessary to jump into the circle somewhere.)

@<Scan the delimited argument represented by |mp_info(r)|@>=
if ( mp->cur_cmd!=comma ) {
  mp_get_x_next(mp);
  if ( mp->cur_cmd!=left_delimiter ) {
    print_err("Missing argument to ");
@.Missing argument...@>
    mp_print_macro_name(mp, arg_list,macro_name);
    help3("That macro has more parameters than you thought.",
     "I'll continue by pretending that each missing argument",
     "is either zero or null.");
    if ( mp_info(r)>=suffix_base ) {
      mp->cur_exp=null; mp->cur_type=mp_token_list;
    } else { 
      mp->cur_exp=0; mp->cur_type=mp_known;
    }
    mp_back_error(mp); mp->cur_cmd=right_delimiter; 
    goto FOUND;
  }
  l_delim=mp->cur_sym; r_delim=mp->cur_mod;
}
@<Scan the argument represented by |mp_info(r)|@>;
if ( mp->cur_cmd!=comma ) 
  @<Check that the proper right delimiter was present@>;
FOUND:  
@<Append the current expression to |arg_list|@>

@ @<Check that the proper right delim...@>=
if ( (mp->cur_cmd!=right_delimiter)||(mp->cur_mod!=l_delim) ) {
  if ( mp_info(mp_link(r))>=expr_base ) {
    mp_missing_err(mp, ",");
@.Missing `,'@>
    help3("I've finished reading a macro argument and am about to",
      "read another; the arguments weren't delimited correctly.",
      "You might want to delete some tokens before continuing.");
    mp_back_error(mp); mp->cur_cmd=comma;
  } else { 
    mp_missing_err(mp, str(text(r_delim)));
@.Missing `)'@>
    help2("I've gotten to the end of the macro parameter list.",
          "You might want to delete some tokens before continuing.");
    mp_back_error(mp);
  }
}

@ A \&{suffix} or \&{text} parameter will have been scanned as
a token list pointed to by |cur_exp|, in which case we will have
|cur_type=token_list|.

@<Append the current expression to |arg_list|@>=
{ 
  p=mp_get_avail(mp);
  if ( mp->cur_type==mp_token_list ) mp_info(p)=mp->cur_exp;
  else mp_info(p)=mp_stash_cur_exp(mp);
  if ( mp->internal[mp_tracing_macros]>0 ) {
    mp_begin_diagnostic(mp); mp_print_arg(mp, mp_info(p),n,mp_info(r)); 
    mp_end_diagnostic(mp, false);
  }
  if ( arg_list==null ) arg_list=p;
  else mp_link(tail)=p;
  tail=p; incr(n);
}

@ @<Scan the argument represented by |mp_info(r)|@>=
if ( mp_info(r)>=text_base ) {
  mp_scan_text_arg(mp, l_delim,r_delim);
} else { 
  mp_get_x_next(mp);
  if ( mp_info(r)>=suffix_base ) mp_scan_suffix(mp);
  else mp_scan_expression(mp);
}

@ The parameters to |scan_text_arg| are either a pair of delimiters
or zero; the latter case is for undelimited text arguments, which
end with the first semicolon or \&{endgroup} or \&{end} that is not
contained in a group.

@<Declarations@>=
static void mp_scan_text_arg (MP mp,pointer l_delim, pointer r_delim) ;

@ @c
void mp_scan_text_arg (MP mp,pointer l_delim, pointer r_delim) {
  integer balance; /* excess of |l_delim| over |r_delim| */
  pointer p; /* list tail */
  mp->warning_info=l_delim; mp->scanner_status=absorbing;
  p=hold_head; balance=1; mp_link(hold_head)=null;
  while (1)  { 
    get_t_next;
    if ( l_delim==0 ) {
      @<Adjust the balance for an undelimited argument; |break| if done@>;
    } else {
 	  @<Adjust the balance for a delimited argument; |break| if done@>;
    }
    mp_link(p)=mp_cur_tok(mp); p=mp_link(p);
  }
  mp->cur_exp=mp_link(hold_head); mp->cur_type=mp_token_list;
  mp->scanner_status=normal;
}

@ @<Adjust the balance for a delimited argument...@>=
if ( mp->cur_cmd==right_delimiter ) { 
  if ( mp->cur_mod==l_delim ) { 
    decr(balance);
    if ( balance==0 ) break;
  }
} else if ( mp->cur_cmd==left_delimiter ) {
  if ( mp->cur_mod==r_delim ) incr(balance);
}

@ @<Adjust the balance for an undelimited...@>=
if ( end_of_statement ) { /* |cur_cmd=semicolon|, |end_group|, or |stop| */
  if ( balance==1 ) { break; }
  else  { if ( mp->cur_cmd==end_group ) decr(balance); }
} else if ( mp->cur_cmd==begin_group ) { 
  incr(balance); 
}

@ @<Scan undelimited argument(s)@>=
{ 
  if ( mp_info(r)<text_macro ) {
    mp_get_x_next(mp);
    if ( mp_info(r)!=suffix_macro ) {
      if ( (mp->cur_cmd==equals)||(mp->cur_cmd==assignment) ) mp_get_x_next(mp);
    }
  }
  switch (mp_info(r)) {
  case primary_macro:mp_scan_primary(mp); break;
  case secondary_macro:mp_scan_secondary(mp); break;
  case tertiary_macro:mp_scan_tertiary(mp); break;
  case expr_macro:mp_scan_expression(mp); break;
  case of_macro:
    @<Scan an expression followed by `\&{of} $\langle$primary$\rangle$'@>;
    break;
  case suffix_macro:
    @<Scan a suffix with optional delimiters@>;
    break;
  case text_macro:mp_scan_text_arg(mp, 0,0); break;
  } /* there are no other cases */
  mp_back_input(mp); 
  @<Append the current expression to |arg_list|@>;
}

@ @<Scan an expression followed by `\&{of} $\langle$primary$\rangle$'@>=
{ 
  mp_scan_expression(mp); p=mp_get_avail(mp); mp_info(p)=mp_stash_cur_exp(mp);
  if ( mp->internal[mp_tracing_macros]>0 ) { 
    mp_begin_diagnostic(mp); mp_print_arg(mp, mp_info(p),n,0); 
    mp_end_diagnostic(mp, false);
  }
  if ( arg_list==null ) arg_list=p; else mp_link(tail)=p;
  tail=p;incr(n);
  if ( mp->cur_cmd!=of_token ) {
    mp_missing_err(mp, "of"); mp_print(mp, " for ");
@.Missing `of'@>
    mp_print_macro_name(mp, arg_list,macro_name);
    help1("I've got the first argument; will look now for the other.");
    mp_back_error(mp);
  }
  mp_get_x_next(mp); mp_scan_primary(mp);
}

@ @<Scan a suffix with optional delimiters@>=
{ 
  if ( mp->cur_cmd!=left_delimiter ) {
    l_delim=null;
  } else { 
    l_delim=mp->cur_sym; r_delim=mp->cur_mod; mp_get_x_next(mp);
  };
  mp_scan_suffix(mp);
  if ( l_delim!=null ) {
    if ((mp->cur_cmd!=right_delimiter)||(mp->cur_mod!=l_delim) ) {
      mp_missing_err(mp, str(text(r_delim)));
@.Missing `)'@>
      help2("I've gotten to the end of the macro parameter list.",
            "You might want to delete some tokens before continuing.");
      mp_back_error(mp);
    }
    mp_get_x_next(mp);
  }
}

@ Before we put a new token list on the input stack, it is wise to clean off
all token lists that have recently been depleted. Then a user macro that ends
with a call to itself will not require unbounded stack space.

@<Feed the arguments and replacement text to the scanner@>=
while ( token_state &&(loc==null) ) mp_end_token_list(mp); /* conserve stack space */
if ( mp->param_ptr+n>mp->max_param_stack ) {
  mp->max_param_stack=mp->param_ptr+n;
  if ( mp->max_param_stack>mp->param_size )
    mp_overflow(mp, "parameter stack size",mp->param_size);
@:MetaPost capacity exceeded parameter stack size}{\quad parameter stack size@>
}
mp_begin_token_list(mp, def_ref, (quarterword)macro); name=macro_name; loc=r;
if ( n>0 ) {
  p=arg_list;
  do {  
   mp->param_stack[mp->param_ptr]=mp_info(p); incr(mp->param_ptr); p=mp_link(p);
  } while (p!=null);
  mp_flush_list(mp, arg_list);
}

@ It's sometimes necessary to put a single argument onto |param_stack|.
The |stack_argument| subroutine does this.

@c 
static void mp_stack_argument (MP mp,pointer p) { 
  if ( mp->param_ptr==mp->max_param_stack ) {
    incr(mp->max_param_stack);
    if ( mp->max_param_stack>mp->param_size )
      mp_overflow(mp, "parameter stack size",mp->param_size);
@:MetaPost capacity exceeded parameter stack size}{\quad parameter stack size@>
  }
  mp->param_stack[mp->param_ptr]=p; incr(mp->param_ptr);
}

@* \[33] Conditional processing.
Let's consider now the way \&{if} commands are handled.

Conditions can be inside conditions, and this nesting has a stack
that is independent of other stacks.
Four global variables represent the top of the condition stack:
|cond_ptr| points to pushed-down entries, if~any; |cur_if| tells whether
we are processing \&{if} or \&{elseif}; |if_limit| specifies
the largest code of a |fi_or_else| command that is syntactically legal;
and |if_line| is the line number at which the current conditional began.

If no conditions are currently in progress, the condition stack has the
special state |cond_ptr=null|, |if_limit=normal|, |cur_if=0|, |if_line=0|.
Otherwise |cond_ptr| points to a two-word node; the |type|, |name_type|, and
|link| fields of the first word contain |if_limit|, |cur_if|, and
|cond_ptr| at the next level, and the second word contains the
corresponding |if_line|.

@d if_node_size 2 /* number of words in stack entry for conditionals */
@d if_line_field(A) mp->mem[(A)+1].cint
@d if_code 1 /* code for \&{if} being evaluated */
@d fi_code 2 /* code for \&{fi} */
@d else_code 3 /* code for \&{else} */
@d else_if_code 4 /* code for \&{elseif} */

@<Glob...@>=
pointer cond_ptr; /* top of the condition stack */
integer if_limit; /* upper bound on |fi_or_else| codes */
quarterword cur_if; /* type of conditional being worked on */
integer if_line; /* line where that conditional began */

@ @<Set init...@>=
mp->cond_ptr=null; mp->if_limit=normal; mp->cur_if=0; mp->if_line=0;

@ @<Put each...@>=
mp_primitive(mp, "if",if_test,if_code);
@:if_}{\&{if} primitive@>
mp_primitive(mp, "fi",fi_or_else,fi_code); mp->eqtb[frozen_fi]=mp->eqtb[mp->cur_sym];
@:fi_}{\&{fi} primitive@>
mp_primitive(mp, "else",fi_or_else,else_code);
@:else_}{\&{else} primitive@>
mp_primitive(mp, "elseif",fi_or_else,else_if_code);
@:else_if_}{\&{elseif} primitive@>

@ @<Cases of |print_cmd_mod|...@>=
case if_test:
case fi_or_else: 
  switch (m) {
  case if_code:mp_print(mp, "if"); break;
  case fi_code:mp_print(mp, "fi");  break;
  case else_code:mp_print(mp, "else"); break;
  default: mp_print(mp, "elseif"); break;
  }
  break;

@ Here is a procedure that ignores text until coming to an \&{elseif},
\&{else}, or \&{fi} at level zero of $\&{if}\ldots\&{fi}$
nesting. After it has acted, |cur_mod| will indicate the token that
was found.

\MP's smallest two command codes are |if_test| and |fi_or_else|; this
makes the skipping process a bit simpler.

@c 
void mp_pass_text (MP mp) {
  integer l = 0;
  mp->scanner_status=skipping;
  mp->warning_info=mp_true_line(mp);
  while (1)  { 
    get_t_next;
    if ( mp->cur_cmd<=fi_or_else ) {
      if ( mp->cur_cmd<fi_or_else ) {
        incr(l);
      } else { 
        if ( l==0 ) break;
        if ( mp->cur_mod==fi_code ) decr(l);
      }
    } else {
      @<Decrease the string reference count,
       if the current token is a string@>;
    }
  }
  mp->scanner_status=normal;
}

@ @<Decrease the string reference count...@>=
if ( mp->cur_cmd==string_token ) { delete_str_ref(mp->cur_mod); }

@ When we begin to process a new \&{if}, we set |if_limit:=if_code|; then
if \&{elseif} or \&{else} or \&{fi} occurs before the current \&{if}
condition has been evaluated, a colon will be inserted.
A construction like `\.{if fi}' would otherwise get \MP\ confused.

@<Push the condition stack@>=
{ p=mp_get_node(mp, if_node_size); mp_link(p)=mp->cond_ptr; mp_type(p)=(quarterword)mp->if_limit;
  mp_name_type(p)=mp->cur_if; if_line_field(p)=mp->if_line;
  mp->cond_ptr=p; mp->if_limit=if_code; mp->if_line=mp_true_line(mp); 
  mp->cur_if=if_code;
}

@ @<Pop the condition stack@>=
{ p=mp->cond_ptr; mp->if_line=if_line_field(p);
  mp->cur_if=mp_name_type(p); mp->if_limit=mp_type(p); mp->cond_ptr=mp_link(p);
  mp_free_node(mp, p,if_node_size);
}

@ Here's a procedure that changes the |if_limit| code corresponding to
a given value of |cond_ptr|.

@c 
static void mp_change_if_limit (MP mp,quarterword l, pointer p) {
  pointer q;
  if ( p==mp->cond_ptr ) {
    mp->if_limit=l; /* that's the easy case */
  } else  { 
    q=mp->cond_ptr;
    while (1) { 
      if ( q==null ) mp_confusion(mp, "if");
@:this can't happen if}{\quad if@>
      if ( mp_link(q)==p ) { 
        mp_type(q)=l; return;
      }
      q=mp_link(q);
    }
  }
}

@ The user is supposed to put colons into the proper parts of conditional
statements. Therefore, \MP\ has to check for their presence.

@c 
static void mp_check_colon (MP mp) { 
  if ( mp->cur_cmd!=colon ) { 
    mp_missing_err(mp, ":");
@.Missing `:'@>
    help2("There should've been a colon after the condition.",
          "I shall pretend that one was there.");
    mp_back_error(mp);
  }
}

@ A condition is started when the |get_x_next| procedure encounters
an |if_test| command; in that case |get_x_next| calls |conditional|,
which is a recursive procedure.
@^recursion@>

@c 
void mp_conditional (MP mp) {
  pointer save_cond_ptr; /* |cond_ptr| corresponding to this conditional */
  int new_if_limit; /* future value of |if_limit| */
  pointer p; /* temporary register */
  @<Push the condition stack@>; 
  save_cond_ptr=mp->cond_ptr;
RESWITCH: 
  mp_get_boolean(mp); new_if_limit=else_if_code;
  if ( mp->internal[mp_tracing_commands]>unity ) {
    @<Display the boolean value of |cur_exp|@>;
  }
FOUND: 
  mp_check_colon(mp);
  if ( mp->cur_exp==true_code ) {
    mp_change_if_limit(mp, (quarterword)new_if_limit, save_cond_ptr);
    return; /* wait for \&{elseif}, \&{else}, or \&{fi} */
  };
  @<Skip to \&{elseif} or \&{else} or \&{fi}, then |goto done|@>;
DONE: 
  mp->cur_if=(quarterword)mp->cur_mod; mp->if_line=mp_true_line(mp);
  if ( mp->cur_mod==fi_code ) {
    @<Pop the condition stack@>
  } else if ( mp->cur_mod==else_if_code ) {
    goto RESWITCH;
  } else  { 
    mp->cur_exp=true_code; new_if_limit=fi_code; mp_get_x_next(mp); 
    goto FOUND;
  }
}

@ In a construction like `\&{if} \&{if} \&{true}: $0=1$: \\{foo}
\&{else}: \\{bar} \&{fi}', the first \&{else}
that we come to after learning that the \&{if} is false is not the
\&{else} we're looking for. Hence the following curious logic is needed.

@<Skip to \&{elseif}...@>=
while (1) { 
  mp_pass_text(mp);
  if ( mp->cond_ptr==save_cond_ptr ) goto DONE;
  else if ( mp->cur_mod==fi_code ) @<Pop the condition stack@>;
}


@ @<Display the boolean value...@>=
{ mp_begin_diagnostic(mp);
  if ( mp->cur_exp==true_code ) mp_print(mp, "{true}");
  else mp_print(mp, "{false}");
  mp_end_diagnostic(mp, false);
}

@ The processing of conditionals is complete except for the following
code, which is actually part of |get_x_next|. It comes into play when
\&{elseif}, \&{else}, or \&{fi} is scanned.

@<Terminate the current conditional and skip to \&{fi}@>=
if ( mp->cur_mod>mp->if_limit ) {
  if ( mp->if_limit==if_code ) { /* condition not yet evaluated */
    mp_missing_err(mp, ":");
@.Missing `:'@>
    mp_back_input(mp); mp->cur_sym=frozen_colon; mp_ins_error(mp);
  } else  { 
    print_err("Extra "); mp_print_cmd_mod(mp, fi_or_else,mp->cur_mod);
@.Extra else@>
@.Extra elseif@>
@.Extra fi@>
    help1("I'm ignoring this; it doesn't match any if.");
    mp_error(mp);
  }
} else  { 
  while ( mp->cur_mod!=fi_code ) mp_pass_text(mp); /* skip to \&{fi} */
  @<Pop the condition stack@>;
}

@* \[34] Iterations.
To bring our treatment of |get_x_next| to a close, we need to consider what
\MP\ does when it sees \&{for}, \&{forsuffixes}, and \&{forever}.

There's a global variable |loop_ptr| that keeps track of the \&{for} loops
that are currently active. If |loop_ptr=null|, no loops are in progress;
otherwise |mp_info(loop_ptr)| points to the iterative text of the current
(innermost) loop, and |mp_link(loop_ptr)| points to the data for any other
loops that enclose the current one.

A loop-control node also has two other fields, called |loop_type| and
|loop_list|, whose contents depend on the type of loop:

\yskip\indent|loop_type(loop_ptr)=null| means that |loop_list(loop_ptr)|
points to a list of one-word nodes whose |info| fields point to the
remaining argument values of a suffix list and expression list.

\yskip\indent|loop_type(loop_ptr)=mp_void| means that the current loop is
`\&{forever}'.

\yskip\indent|loop_type(loop_ptr)=progression_flag| means that
|p=loop_list(loop_ptr)| points to a ``progression node'' and |value(p)|,
|step_size(p)|, and |final_value(p)| contain the data for an arithmetic
progression.

\yskip\indent|loop_type(loop_ptr)=p>mp_void| means that |p| points to an edge
header and |loop_list(loop_ptr)| points into the graphical object list for
that edge header.

\yskip\noindent In the case of a progression node, the first word is not used
because the link field of words in the dynamic memory area cannot be arbitrary.

@d loop_list_loc(A) ((A)+1) /* where the |loop_list| field resides */
@d loop_type(A) mp_info(loop_list_loc((A))) /* the type of \&{for} loop */
@d loop_list(A) mp_link(loop_list_loc((A))) /* the remaining list elements */
@d loop_node_size 2 /* the number of words in a loop control node */
@d progression_node_size 4 /* the number of words in a progression node */
@d step_size(A) mp->mem[(A)+2].sc /* the step size in an arithmetic progression */
@d final_value(A) mp->mem[(A)+3].sc /* the final value in an arithmetic progression */
@d progression_flag (null+2)
  /* |loop_type| value when |loop_list| points to a progression node */

@<Glob...@>=
pointer loop_ptr; /* top of the loop-control-node stack */

@ @<Set init...@>=
mp->loop_ptr=null;

@ If the expressions that define an arithmetic progression in
a \&{for} loop don't have known numeric values, the |bad_for|
subroutine screams at the user.

@c 
static void mp_bad_for (MP mp, const char * s) {
  mp_disp_err(mp, null,"Improper "); /* show the bad expression above the message */
@.Improper...replaced by 0@>
  mp_print(mp, s); mp_print(mp, " has been replaced by 0");
  help4("When you say `for x=a step b until c',",
    "the initial value `a' and the step size `b'",
    "and the final value `c' must have known numeric values.",
    "I'm zeroing this one. Proceed, with fingers crossed.");
  mp_put_get_flush_error(mp, 0);
}

@ Here's what \MP\ does when \&{for}, \&{forsuffixes}, or \&{forever}
has just been scanned. (This code requires slight familiarity with
expression-parsing routines that we have not yet discussed; but it seems
to belong in the present part of the program, even though the original author
didn't write it until later. The reader may wish to come back to it.)

@c void mp_begin_iteration (MP mp) {
  halfword m; /* |expr_base| (\&{for}) or |suffix_base| (\&{forsuffixes}) */
  halfword n; /* hash address of the current symbol */
  pointer s; /* the new loop-control node */
  pointer p; /* substitution list for |scan_toks| */
  pointer q;  /* link manipulation register */
  pointer pp; /* a new progression node */
  m=mp->cur_mod; n=mp->cur_sym; s=mp_get_node(mp, loop_node_size);
  if ( m==start_forever ){ 
    loop_type(s)=mp_void; p=null; mp_get_x_next(mp);
  } else { 
    mp_get_symbol(mp); p=mp_get_node(mp, token_node_size);
    mp_info(p)=mp->cur_sym; value(p)=m;
    mp_get_x_next(mp);
    if ( mp->cur_cmd==within_token ) {
      @<Set up a picture iteration@>;
    } else { 
      @<Check for the |"="| or |":="| in a loop header@>;
      @<Scan the values to be used in the loop@>;
    }
  }
  @<Check for the presence of a colon@>;
  @<Scan the loop text and put it on the loop control stack@>;
  mp_resume_iteration(mp);
}

@ @<Check for the |"="| or |":="| in a loop header@>=
if ( (mp->cur_cmd!=equals)&&(mp->cur_cmd!=assignment) ) { 
  mp_missing_err(mp, "=");
@.Missing `='@>
  help3("The next thing in this loop should have been `=' or `:='.",
    "But don't worry; I'll pretend that an equals sign",
    "was present, and I'll look for the values next.");
  mp_back_error(mp);
}

@ @<Check for the presence of a colon@>=
if ( mp->cur_cmd!=colon ) { 
  mp_missing_err(mp, ":");
@.Missing `:'@>
  help3("The next thing in this loop should have been a `:'.",
    "So I'll pretend that a colon was present;",
    "everything from here to `endfor' will be iterated.");
  mp_back_error(mp);
}

@ We append a special |frozen_repeat_loop| token in place of the
`\&{endfor}' at the end of the loop. This will come through \MP's scanner
at the proper time to cause the loop to be repeated.

(If the user tries some shenanigan like `\&{for} $\ldots$ \&{let} \&{endfor}',
he will be foiled by the |get_symbol| routine, which keeps frozen
tokens unchanged. Furthermore the |frozen_repeat_loop| is an \&{outer}
token, so it won't be lost accidentally.)

@ @<Scan the loop text...@>=
q=mp_get_avail(mp); mp_info(q)=frozen_repeat_loop;
mp->scanner_status=loop_defining; mp->warning_info=n;
mp_info(s)=mp_scan_toks(mp, iteration,p,q,0); mp->scanner_status=normal;
mp_link(s)=mp->loop_ptr; mp->loop_ptr=s

@ @<Initialize table...@>=
eq_type(frozen_repeat_loop)=repeat_loop+outer_tag;
text(frozen_repeat_loop)=intern(" ENDFOR");

@ The loop text is inserted into \MP's scanning apparatus by the
|resume_iteration| routine.

@c void mp_resume_iteration (MP mp) {
  pointer p,q; /* link registers */
  p=loop_type(mp->loop_ptr);
  if ( p==progression_flag ) { 
    p=loop_list(mp->loop_ptr); /* now |p| points to a progression node */
    mp->cur_exp=value(p);
    if ( @<The arithmetic progression has ended@> ) {
      mp_stop_iteration(mp);
      return;
    }
    mp->cur_type=mp_known; q=mp_stash_cur_exp(mp); /* make |q| an \&{expr} argument */
    value(p)=mp->cur_exp+step_size(p); /* set |value(p)| for the next iteration */
    /* detect numeric overflow */
    if ((step_size(p)>0) && (value(p)<mp->cur_exp)) {
       if (final_value(p)>0) {
         value(p)=final_value(p);
         final_value(p) = final_value(p) - 1;
       } else {
         value(p)=final_value(p)+1;
       }
    } else if ((step_size(p)<0) && (value(p)>mp->cur_exp)) {
       if (final_value(p)<0) {
         value(p)=final_value(p);
         final_value(p) = final_value(p)+1;
       } else {
         value(p)=final_value(p)-1;
       }
    }
    
  } else if ( p==null ) { 
    p=loop_list(mp->loop_ptr);
    if ( p==null ) {
      mp_stop_iteration(mp);
      return;
    }
    loop_list(mp->loop_ptr)=mp_link(p); q=mp_info(p); free_avail(p);
  } else if ( p==mp_void ) { 
    mp_begin_token_list(mp, mp_info(mp->loop_ptr), (quarterword)forever_text); return;
  } else {
    @<Make |q| a capsule containing the next picture component from
      |loop_list(loop_ptr)| or |goto not_found|@>;
  }
  mp_begin_token_list(mp, mp_info(mp->loop_ptr), (quarterword)loop_text);
  mp_stack_argument(mp, q);
  if ( mp->internal[mp_tracing_commands]>unity ) {
     @<Trace the start of a loop@>;
  }
  return;
NOT_FOUND:
  mp_stop_iteration(mp);
}

@ @<The arithmetic progression has ended@>=
((step_size(p)>0)&&(mp->cur_exp>final_value(p)))||
 ((step_size(p)<0)&&(mp->cur_exp<final_value(p)))

@ @<Trace the start of a loop@>=
{ 
  mp_begin_diagnostic(mp); mp_print_nl(mp, "{loop value=");
@.loop value=n@>
  if ( (q!=null)&&(mp_link(q)==mp_void) ) mp_print_exp(mp, q,1);
  else mp_show_token_list(mp, q,null,50,0);
  mp_print_char(mp, xord('}')); mp_end_diagnostic(mp, false);
}

@ @<Make |q| a capsule containing the next picture component from...@>=
{ q=loop_list(mp->loop_ptr);
  if ( q==null ) goto NOT_FOUND;
  skip_component(q) goto NOT_FOUND;
  mp->cur_exp=mp_copy_objects(mp, loop_list(mp->loop_ptr),q);
  mp_init_bbox(mp, mp->cur_exp);
  mp->cur_type=mp_picture_type;
  loop_list(mp->loop_ptr)=q;
  q=mp_stash_cur_exp(mp);
}

@ A level of loop control disappears when |resume_iteration| has decided
not to resume, or when an \&{exitif} construction has removed the loop text
from the input stack.

@c void mp_stop_iteration (MP mp) {
  pointer p,q; /* the usual */
  p=loop_type(mp->loop_ptr);
  if ( p==progression_flag )  {
    mp_free_node(mp, loop_list(mp->loop_ptr),progression_node_size);
  } else if ( p==null ){ 
    q=loop_list(mp->loop_ptr);
    while ( q!=null ) {
      p=mp_info(q);
      if ( p!=null ) {
        if ( mp_link(p)==mp_void ) { /* it's an \&{expr} parameter */
          mp_recycle_value(mp, p); mp_free_node(mp, p,value_node_size);
        } else {
          mp_flush_token_list(mp, p); /* it's a \&{suffix} or \&{text} parameter */
        }
      }
      p=q; q=mp_link(q); free_avail(p);
    }
  } else if ( p>progression_flag ) {
    delete_edge_ref(p);
  }
  p=mp->loop_ptr; mp->loop_ptr=mp_link(p); mp_flush_token_list(mp, mp_info(p));
  mp_free_node(mp, p,loop_node_size);
}

@ Now that we know all about loop control, we can finish up
the missing portion of |begin_iteration| and we'll be done.

The following code is performed after the `\.=' has been scanned in
a \&{for} construction (if |m=expr_base|) or a \&{forsuffixes} construction
(if |m=suffix_base|).

@<Scan the values to be used in the loop@>=
loop_type(s)=null; q=loop_list_loc(s); mp_link(q)=null; /* |mp_link(q)=loop_list(s)| */
do {  
  mp_get_x_next(mp);
  if ( m!=expr_base ) {
    mp_scan_suffix(mp);
  } else { 
    if ( mp->cur_cmd>=colon ) if ( mp->cur_cmd<=comma ) 
	  goto CONTINUE;
    mp_scan_expression(mp);
    if ( mp->cur_cmd==step_token ) if ( q==loop_list_loc(s) ) {
      @<Prepare for step-until construction and |break|@>;
    }
    mp->cur_exp=mp_stash_cur_exp(mp);
  }
  mp_link(q)=mp_get_avail(mp); q=mp_link(q); 
  mp_info(q)=mp->cur_exp; mp->cur_type=mp_vacuous;
CONTINUE:
  ;
} while (mp->cur_cmd==comma)

@ @<Prepare for step-until construction and |break|@>=
{ 
  if ( mp->cur_type!=mp_known ) mp_bad_for(mp, "initial value");
  pp=mp_get_node(mp, progression_node_size); value(pp)=mp->cur_exp;
  mp_get_x_next(mp); mp_scan_expression(mp);
  if ( mp->cur_type!=mp_known ) mp_bad_for(mp, "step size");
  step_size(pp)=mp->cur_exp;
  if ( mp->cur_cmd!=until_token ) { 
    mp_missing_err(mp, "until");
@.Missing `until'@>
    help2("I assume you meant to say `until' after `step'.",
          "So I'll look for the final value and colon next.");
    mp_back_error(mp);
  }
  mp_get_x_next(mp); mp_scan_expression(mp);
  if ( mp->cur_type!=mp_known ) mp_bad_for(mp, "final value");
  final_value(pp)=mp->cur_exp; loop_list(s)=pp;
  loop_type(s)=progression_flag; 
  break;
}

@ The last case is when we have just seen ``\&{within}'', and we need to
parse a picture expression and prepare to iterate over it.

@<Set up a picture iteration@>=
{ mp_get_x_next(mp);
  mp_scan_expression(mp);
  @<Make sure the current expression is a known picture@>;
  loop_type(s)=mp->cur_exp; mp->cur_type=mp_vacuous;
  q=mp_link(dummy_loc(mp->cur_exp));
  if ( q!= null ) 
    if ( is_start_or_stop(q) )
      if ( mp_skip_1component(mp, q)==null ) q=mp_link(q);
  loop_list(s)=q;
}

@ @<Make sure the current expression is a known picture@>=
if ( mp->cur_type!=mp_picture_type ) {
  mp_disp_err(mp, null,"Improper iteration spec has been replaced by nullpicture");
  help1("When you say `for x in p', p must be a known picture.");
  mp_put_get_flush_error(mp, mp_get_node(mp, edge_header_size));
  mp_init_edges(mp, mp->cur_exp); mp->cur_type=mp_picture_type;
}

@* \[35] File names.
It's time now to fret about file names.  Besides the fact that different
operating systems treat files in different ways, we must cope with the
fact that completely different naming conventions are used by different
groups of people. The following programs show what is required for one
particular operating system; similar routines for other systems are not
difficult to devise.
@^system dependencies@>

\MP\ assumes that a file name has three parts: the name proper; its
``extension''; and a ``file area'' where it is found in an external file
system.  The extension of an input file is assumed to be
`\.{.mp}' unless otherwise specified; it is `\.{.log}' on the
transcript file that records each run of \MP; it is `\.{.tfm}' on the font
metric files that describe characters in any fonts created by \MP; it is
`\.{.ps}' or `.{\it nnn}' for some number {\it nnn} on the \ps\ output files;
and it is `\.{.mem}' on the mem files written by \.{INIMP} to initialize \MP.
The file area can be arbitrary on input files, but files are usually
output to the user's current area.  If an input file cannot be
found on the specified area, \MP\ will look for it on a special system
area; this special area is intended for commonly used input files.

Simple uses of \MP\ refer only to file names that have no explicit
extension or area. For example, a person usually says `\.{input} \.{cmr10}'
instead of `\.{input} \.{cmr10.new}'. Simple file
names are best, because they make the \MP\ source files portable;
whenever a file name consists entirely of letters and digits, it should be
treated in the same way by all implementations of \MP. However, users
need the ability to refer to other files in their environment, especially
when responding to error messages concerning unopenable files; therefore
we want to let them use the syntax that appears in their favorite
operating system.

@ \MP\ uses the same conventions that have proved to be satisfactory for
\TeX\ and \MF. In order to isolate the system-dependent aspects of file names,
@^system dependencies@>
the system-independent parts of \MP\ are expressed in terms
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

Actually the situation is slightly more complicated, because \MP\ needs
to know when the file name ends. The |more_name| routine is a function
(with side effects) that returns |true| on the calls |more_name|$(c_1)$,
\dots, |more_name|$(c_{n-1})$. The final call |more_name|$(c_n)$
returns |false|; or, it returns |true| and $c_n$ is the last character
on the current input line. In other words,
|more_name| is supposed to return |true| unless it is sure that the
file name has been completely scanned; and |end_name| is supposed to be able
to finish the assembly of |cur_name|, |cur_area|, and |cur_ext| regardless of
whether $|more_name|(c_n)$ returned |true| or |false|.

@<Glob...@>=
char * cur_name; /* name of file just scanned */
char * cur_area; /* file area just scanned, or \.{""} */
char * cur_ext; /* file extension just scanned, or \.{""} */

@ It is easier to maintain reference counts if we assign initial values.

@<Set init...@>=
mp->cur_name=xstrdup(""); 
mp->cur_area=xstrdup(""); 
mp->cur_ext=xstrdup("");

@ @<Dealloc variables@>=
xfree(mp->cur_area);
xfree(mp->cur_name);
xfree(mp->cur_ext);

@ The file names we shall deal with for illustrative purposes have the
following structure:  If the name contains `\.>' or `\.:', the file area
consists of all characters up to and including the final such character;
otherwise the file area is null.  If the remaining file name contains
`\..', the file extension consists of all such characters from the first
remaining `\..' to the end, otherwise the file extension is null.
@^system dependencies@>

We can scan such file names easily by using two global variables that keep track
of the occurrences of area and extension delimiters.  Note that these variables
cannot be of type |pool_pointer| because a string pool compaction could occur
while scanning a file name.

@<Glob...@>=
integer area_delimiter;
  /* most recent `\.>' or `\.:' relative to |str_start[str_ptr]| */
integer ext_delimiter; /* the relevant `\..', if any */
boolean quoted_filename; /* whether the filename is wrapped in " markers */

@ Here now is the first of the system-dependent routines for file name scanning.
@^system dependencies@>

The file name length is limited to |file_name_size|. That is good, because
in the current configuration we cannot call |mp_do_compaction| while a name 
is being scanned, |mp->area_delimiter| and |mp->ext_delimiter| are direct
offsets into |mp->str_pool|. I am not in a great hurry to fix this, because 
calling |str_room()| just once is more efficient anyway. TODO.

@<Declarations@>=
static void mp_begin_name (MP mp);
static boolean mp_more_name (MP mp, ASCII_code c);
static void mp_end_name (MP mp);

@ @c
void mp_begin_name (MP mp) { 
  xfree(mp->cur_name); 
  xfree(mp->cur_area); 
  xfree(mp->cur_ext);
  mp->area_delimiter=-1; 
  mp->ext_delimiter=-1;
  mp->quoted_filename=false;
  str_room(file_name_size); 
}

@ And here's the second.
@^system dependencies@>

@d IS_DIR_SEP(c) (c=='/' || c=='\\')

@c 
boolean mp_more_name (MP mp, ASCII_code c) {
  if (c=='"') {
    mp->quoted_filename= ! mp->quoted_filename;
  } else if ((c==' '|| c=='\t') && (mp->quoted_filename==false)) {
    return false;
  } else {
    if (IS_DIR_SEP (c)) {
      mp->area_delimiter=mp->pool_ptr; 
      mp->ext_delimiter=-1;
    } else if ( c=='.' ) {
      mp->ext_delimiter=mp->pool_ptr;
    }
    append_char(c); /* contribute |c| to the current string */
  }
  return true;
}

@ The third.
@^system dependencies@>

@d copy_pool_segment(A,B,C) { 
      A = xmalloc(C+1,sizeof(char)); 
      (void)memcpy(A,(char *)(mp->str_pool+B),C);  
      A[C] = 0;}

@c
void mp_end_name (MP mp) {
  pool_pointer s; /* length of area, name, and extension */
  unsigned int len;
  /* "my/w.mp" */
  s = mp->str_start[mp->str_ptr];
  if ( mp->area_delimiter<0 ) {    
    mp->cur_area=xstrdup("");
  } else {
    len = (unsigned)(mp->area_delimiter-s+1); 
    copy_pool_segment(mp->cur_area,s,len);
    s += (pool_pointer)len;
  }
  if ( mp->ext_delimiter<0 ) {
    mp->cur_ext=xstrdup("");
    len = (unsigned)(mp->pool_ptr-s); 
  } else {
    copy_pool_segment(mp->cur_ext,mp->ext_delimiter,(size_t)(mp->pool_ptr-mp->ext_delimiter));
    len = (unsigned)(mp->ext_delimiter-s);
  }
  copy_pool_segment(mp->cur_name,s,len);
  mp->pool_ptr=s; /* don't need this partial string */
}

@ Conversely, here is a routine that takes three strings and prints a file
name that might have produced them. (The routine is system dependent, because
some operating systems put the file area last instead of first.)
@^system dependencies@>

@<Basic printing...@>=
static void mp_print_file_name (MP mp, char * n, char * a, char * e) { 
  boolean must_quote = false;
  if (((a != NULL) && (strchr(a,' ') != NULL)) || 
      ((n != NULL) && (strchr(n,' ') != NULL)) ||
      ((e != NULL) && (strchr(e,' ') != NULL)))
    must_quote = true;
  if (must_quote) mp_print_char(mp, (ASCII_code)'"');
  mp_print(mp, a); mp_print(mp, n); mp_print(mp, e);
  if (must_quote) mp_print_char(mp, (ASCII_code)'"');
}

@ Another system-dependent routine is needed to convert three internal
\MP\ strings
to the |name_of_file| value that is used to open files. The present code
allows both lowercase and uppercase letters in the file name.
@^system dependencies@>

@d append_to_name(A) { c=xord((ASCII_code)(A));
  if ( k<file_name_size ) {
    mp->name_of_file[k]=(char)xchr(c);
    incr(k);
  }
}

@ @c
void mp_pack_file_name (MP mp, const char *n, const char *a, const char *e) {
  integer k; /* number of positions filled in |name_of_file| */
  ASCII_code c; /* character being packed */
  const char *j; /* a character  index */
  k=0;
  assert(n!=NULL);
  if (a!=NULL) {
    for (j=a;*j!='\0';j++) { append_to_name(*j); }
  }
  for (j=n;*j!='\0';j++) { append_to_name(*j); }
  if (e!=NULL) {
    for (j=e;*j!='\0';j++) { append_to_name(*j); }
  }
  mp->name_of_file[k]=0;
  mp->name_length=k; 
}

@ @<Internal library declarations@>=
void mp_pack_file_name (MP mp, const char *n, const char *a, const char *e) ;

@ @<Option variables@>=
char *mem_name; /* for commandline */

@ @<Find constant sizes@>=
mp->mem_name = xstrdup(opt->mem_name);
if (mp->mem_name) {
  size_t l = strlen(mp->mem_name);
  if (l>4) {
    char *test = strstr(mp->mem_name,".mem");
    if (test == mp->mem_name+l-4) {
      *test = 0;
    }
  }
}


@ @<Dealloc variables@>=
xfree(mp->mem_name);

@ This part of the program becomes active when a ``virgin'' \MP\ is
trying to get going, just after the preliminary initialization, or
when the user is substituting another mem file by typing `\.\&' after
the initial `\.{**}' prompt.  The buffer contains the first line of
input in |buffer[loc..(last-1)]|, where |loc<last| and |buffer[loc]<>""|.

@<Declarations@>=
static boolean mp_open_mem_name (MP mp) ;
static boolean mp_open_mem_file (MP mp) ;

@ @c
boolean mp_open_mem_name (MP mp) {
  if (mp->mem_name!=NULL) {
    size_t l = strlen(mp->mem_name);
    char *s = xstrdup (mp->mem_name);
    if (l>4) {
      char *test = strstr(s,".mem");
      if (test == NULL || test != s+l-4) {
        s = xrealloc (s, l+5, 1);       
        strcat (s, ".mem");
      }
    } else {
      s = xrealloc (s, l+5, 1);
      strcat (s, ".mem");
    }
    mp->mem_file = (mp->open_file)(mp,s, "r", mp_filetype_memfile);
    xfree(s);
    if ( mp->mem_file ) return true;
  }
  return false;
}
boolean mp_open_mem_file (MP mp) {
  if (mp->mem_file != NULL)
    return true;
  if (mp_open_mem_name(mp)) 
    return true;
  if (mp_xstrcmp(mp->mem_name, "plain")) {
    wake_up_terminal;
    wterm_ln("Sorry, I can\'t find the '");
    wterm(mp->mem_name);
    wterm("' mem file; will try 'plain'.");
@.Sorry, I can't find...@>
    update_terminal;
    /* now pull out all the stops: try for the system \.{plain} file */
    xfree(mp->mem_name);
    mp->mem_name = xstrdup("plain");
    if (mp_open_mem_name(mp))
      return true;
  }
  wake_up_terminal;
  wterm_ln("I can't find the 'plain' mem file!\n");
@.I can't find PLAIN...@>
@.plain@>
  return false;
}

@ Operating systems often make it possible to determine the exact name (and
possible version number) of a file that has been opened. The following routine,
which simply makes a \MP\ string from the value of |name_of_file|, should
ideally be changed to deduce the full name of file~|f|, which is the file
most recently opened, if it is possible to do this.
@^system dependencies@>

@<Declarations@>=
#define mp_a_make_name_string(A,B)  mp_make_name_string(A)
#define mp_b_make_name_string(A,B)  mp_make_name_string(A)
#define mp_w_make_name_string(A,B)  mp_make_name_string(A)

@ @c 
static str_number mp_make_name_string (MP mp) {
  int k; /* index into |name_of_file| */
  str_room(mp->name_length);
  for (k=0;k<mp->name_length;k++) {
    append_char(xord((ASCII_code)mp->name_of_file[k]));
  }
  return mp_make_string(mp);
}

@ Now let's consider the ``driver''
routines by which \MP\ deals with file names
in a system-independent manner.  First comes a procedure that looks for a
file name in the input by taking the information from the input buffer.
(We can't use |get_next|, because the conversion to tokens would
destroy necessary information.)

This procedure doesn't allow semicolons or percent signs to be part of
file names, because of other conventions of \MP.
{\sl The {\logos METAFONT\/}book} doesn't
use semicolons or percents immediately after file names, but some users
no doubt will find it natural to do so; therefore system-dependent
changes to allow such characters in file names should probably
be made with reluctance, and only when an entire file name that
includes special characters is ``quoted'' somehow.
@^system dependencies@>

@c 
static void mp_scan_file_name (MP mp) { 
  mp_begin_name(mp);
  while ( mp->buffer[loc]==' ' ) incr(loc);
  while (1) { 
    if ( (mp->buffer[loc]==';')||(mp->buffer[loc]=='%') ) break;
    if ( ! mp_more_name(mp, mp->buffer[loc]) ) break;
    incr(loc);
  }
  mp_end_name(mp);
}

@ Here is another version that takes its input from a string.

@<Declare subroutines for parsing file names@>=
void mp_str_scan_file (MP mp,  str_number s) ;

@ @c
void mp_str_scan_file (MP mp,  str_number s) {
  pool_pointer p,q; /* current position and stopping point */
  mp_begin_name(mp);
  p=mp->str_start[s]; q=str_stop(s);
  while ( p<q ){ 
    if ( ! mp_more_name(mp, mp->str_pool[p]) ) break;
    incr(p);
  }
  mp_end_name(mp);
}

@ And one that reads from a |char*|.

@<Declare subroutines for parsing file names@>=
extern void mp_ptr_scan_file (MP mp,  char *s);

@ @c
void mp_ptr_scan_file (MP mp,  char *s) {
  char *p, *q; /* current position and stopping point */
  mp_begin_name(mp);
  p=s; q=p+strlen(s);
  while ( p<q ){ 
    if ( ! mp_more_name(mp, xord((ASCII_code)(*p)))) break;
    p++;
  }
  mp_end_name(mp);
}


@ The option variable |job_name| contains the file name that was first
\&{input} by the user. This name is used to initialize the |job_name| global
as well as the |mp_job_name| internal, and is extended by `\.{.log}' and 
`\.{ps}' and `\.{.mem}' and `\.{.tfm}' in order to make the names of \MP's 
output files.

@<Glob...@>=
boolean log_opened; /* has the transcript file been opened? */
char *log_name; /* full name of the log file */

@ @<Option variables@>=
char *job_name; /* principal file name */

@ Initially |job_name=NULL|; it becomes nonzero as soon as the true name is known.
We have |job_name=NULL| if and only if the `\.{log}' file has not been opened,
except of course for a short time just after |job_name| has become nonzero.

@<Allocate or ...@>=
mp->job_name=mp_xstrdup(mp, opt->job_name); 
if (mp->job_name != NULL) {
  char *s = mp->job_name+strlen(mp->job_name);
  while (s>mp->job_name) {
     if (*s == '.') {
         if(strcasecmp(s, ".mp") == 0 || strcasecmp(s, ".mem") == 0 ||
            strcasecmp(s, ".mf") == 0) {
 	         *s = '\0';
	         break;
	     }
     }
     s--;
  }
}
if (opt->noninteractive && opt->ini_version) {
  if (mp->job_name == NULL)
    mp->job_name=mp_xstrdup(mp,mp->mem_name); 
  if (mp->job_name != NULL) {
    size_t l = strlen(mp->job_name);
    if (l>4) {
      char *test = strstr(mp->job_name,".mem");
      if (test == mp->job_name+l-4)
        *test = 0;
    }
  }
}
mp->log_opened=false;

@ Cannot do this earlier because at the |@<Allocate or ...@>|, the string
pool is not yet initialized.

@<Fix up |mp->internal[mp_job_name]|@>=
if (mp->job_name != NULL) {
  if (mp->internal[mp_job_name]!=0)
    delete_str_ref(mp->internal[mp_job_name]);
  mp->internal[mp_job_name]=mp_rts(mp,mp->job_name);
}


@ @<Dealloc variables@>=
xfree(mp->job_name);

@ Here is a routine that manufactures the output file names, assuming that
|job_name<>0|. It ignores and changes the current settings of |cur_area|
and |cur_ext|.

@d pack_cur_name mp_pack_file_name(mp, mp->cur_name,mp->cur_area,mp->cur_ext)

@<Internal library ...@>=
void mp_pack_job_name (MP mp, const char *s) ;

@ @c 
void mp_pack_job_name (MP mp, const char  *s) { /* |s = ".log"|, |".mem"|, |".ps"|, or .\\{nnn} */
  xfree(mp->cur_name); mp->cur_name=xstrdup(mp->job_name);
  xfree(mp->cur_area); mp->cur_area=xstrdup(""); 
  xfree(mp->cur_ext);  mp->cur_ext=xstrdup(s);
  pack_cur_name;
}

@ If some trouble arises when \MP\ tries to open a file, the following
routine calls upon the user to supply another file name. Parameter~|s|
is used in the error message to identify the type of file; parameter~|e|
is the default extension if none is given. Upon exit from the routine,
variables |cur_name|, |cur_area|, |cur_ext|, and |name_of_file| are
ready for another attempt at file opening.

@<Internal library ...@>=
void mp_prompt_file_name (MP mp, const char * s, const char * e) ;

@ @c void mp_prompt_file_name (MP mp, const char * s, const char * e) {
  size_t k; /* index into |buffer| */
  char * saved_cur_name;
  if ( mp->interaction==mp_scroll_mode ) 
	wake_up_terminal;
  if (strcmp(s,"input file name")==0) {
	print_err("I can\'t find file `");
@.I can't find file x@>
  } else {
	print_err("I can\'t write on file `");
@.I can't write on file x@>
  }
  if (strcmp(s,"file name for output")==0) {
    mp_print(mp, mp->output_file); 
  } else {
    mp_print_file_name(mp, mp->cur_name,mp->cur_area,mp->cur_ext); 
  }
  mp_print(mp, "'.");
  if (strcmp(e,"")==0) 
	mp_show_context(mp);
  mp_print_nl(mp, "Please type another "); mp_print(mp, s);
@.Please type...@>
  if (mp->noninteractive || mp->interaction<mp_scroll_mode )
    mp_fatal_error(mp, "*** (job aborted, file error in nonstop mode)");
@.job aborted, file error...@>
  saved_cur_name = xstrdup(mp->cur_name);
  clear_terminal; prompt_input(": "); @<Scan file name in the buffer@>;
  if (strcmp(mp->cur_ext,"")==0) 
	mp->cur_ext=xstrdup(e);
  if (strlen(mp->cur_name)==0) {
    mp->cur_name=saved_cur_name;
  } else {
    xfree(saved_cur_name);
  }
  pack_cur_name;
}

@ @<Scan file name in the buffer@>=
{ 
  mp_begin_name(mp); k=mp->first;
  while ( (mp->buffer[k]==' ')&&(k<mp->last) ) incr(k);
  while (1) { 
    if ( k==mp->last ) break;
    if ( ! mp_more_name(mp, mp->buffer[k]) ) break;
    incr(k);
  }
  mp_end_name(mp);
}

@ The |open_log_file| routine is used to open the transcript file and to help
it catch up to what has previously been printed on the terminal.

@c void mp_open_log_file (MP mp) {
  unsigned old_setting; /* previous |selector| setting */
  int k; /* index into |months| and |buffer| */
  int l; /* end of first input line */
  integer m; /* the current month */
  const char *months="JANFEBMARAPRMAYJUNJULAUGSEPOCTNOVDEC"; 
    /* abbreviations of month names */
  if (mp->log_opened)
    return;
  old_setting=mp->selector;
  if ( mp->job_name==NULL ) {
     mp->job_name=xstrdup("mpout");
     @<Fix up |mp->internal[mp_job_name]|@>;
  }
  mp_pack_job_name(mp,".log");
  while ( ! mp_a_open_out(mp, &mp->log_file, mp_filetype_log) ) {
    @<Try to get a different log file name@>;
  }
  mp->log_name=xstrdup(mp->name_of_file);
  mp->selector=log_only; mp->log_opened=true;
  @<Print the banner line, including the date and time@>;
  mp->input_stack[mp->input_ptr]=mp->cur_input; 
    /* make sure bottom level is in memory */
  if (!mp->noninteractive) {
    mp_print_nl(mp, "**");
@.**@>
    l=mp->input_stack[0].limit_field-1; /* last position of first line */
    for (k=0;k<=l;k++) mp_print_str(mp, mp->buffer[k]);
    mp_print_ln(mp); /* now the transcript file contains the first line of input */
  }
  mp->selector=old_setting+2; /* |log_only| or |term_and_log| */
}

@ @<Dealloc variables@>=
xfree(mp->log_name);

@ Sometimes |open_log_file| is called at awkward moments when \MP\ is
unable to print error messages or even to |show_context|.
The |prompt_file_name| routine can result in a |fatal_error|, but the |error|
routine will not be invoked because |log_opened| will be false.

The normal idea of |mp_batch_mode| is that nothing at all should be written
on the terminal. However, in the unusual case that
no log file could be opened, we make an exception and allow
an explanatory message to be seen.

Incidentally, the program always refers to the log file as a `\.{transcript
file}', because some systems cannot use the extension `\.{.log}' for
this file.

@<Try to get a different log file name@>=
{  
  mp->selector=term_only;
  mp_prompt_file_name(mp, "transcript file name",".log");
}

@ @<Print the banner...@>=
{ 
  wlog(mp->banner);
  mp_print(mp, mp->mem_ident); mp_print(mp, "  ");
  mp_print_int(mp, mp_round_unscaled(mp, mp->internal[mp_day])); 
  mp_print_char(mp, xord(' '));
  m=mp_round_unscaled(mp, mp->internal[mp_month]);
  for (k=3*m-3;k<3*m;k++) { wlog_chr((unsigned char)months[k]); }
  mp_print_char(mp, xord(' ')); 
  mp_print_int(mp, mp_round_unscaled(mp, mp->internal[mp_year])); 
  mp_print_char(mp, xord(' '));
  mp_print_dd(mp, mp_round_unscaled(mp, mp->internal[mp_hour]));
  mp_print_char(mp, xord(':')); 
  mp_print_dd(mp, mp_round_unscaled(mp, mp->internal[mp_minute]));
}

@ The |try_extension| function tries to open an input file determined by
|cur_name|, |cur_area|, and the argument |ext|.  It returns |false| if it
can't find the file in |cur_area| or the appropriate system area.

@c
static boolean mp_try_extension (MP mp, const char *ext) { 
  mp_pack_file_name(mp, mp->cur_name,mp->cur_area, ext);
  in_name=xstrdup(mp->cur_name);
  in_area=xstrdup(mp->cur_area);
  in_ext=xstrdup(ext);
  if ( mp_a_open_in(mp, &cur_file, mp_filetype_program) ) {
    return true;
  } else { 
    mp_pack_file_name(mp, mp->cur_name,NULL,ext);
    return mp_a_open_in(mp, &cur_file, mp_filetype_program);
  }
}

@ Let's turn now to the procedure that is used to initiate file reading
when an `\.{input}' command is being processed.

@c void mp_start_input (MP mp) { /* \MP\ will \.{input} something */
  char *fname = NULL;
  @<Put the desired file name in |(cur_name,cur_ext,cur_area)|@>;
  while (1) { 
    mp_begin_file_reading(mp); /* set up |cur_file| and new level of input */
    if ( strlen(mp->cur_ext)==0 ) {
      if ( mp_try_extension(mp, ".mp") ) break;
      else if ( mp_try_extension(mp, "") ) break;
      else if ( mp_try_extension(mp, ".mf") ) break;
      /* |else do_nothing; | */
    } else if ( mp_try_extension(mp, mp->cur_ext) ) {
      break;
    }
    mp_end_file_reading(mp); /* remove the level that didn't work */
    mp_prompt_file_name(mp, "input file name","");
  }
  name=mp_a_make_name_string(mp, cur_file);
  fname = xstrdup(mp->name_of_file);
  if ( mp->job_name==NULL ) {
    mp->job_name=xstrdup(mp->cur_name); 
    @<Fix up |mp->internal[mp_job_name]|@>;
  }
  if (!mp->log_opened) {
    mp_open_log_file(mp);
  } /* |open_log_file| doesn't |show_context|, so |limit|
        and |loc| needn't be set to meaningful values yet */
  if ( ((int)mp->term_offset+(int)strlen(fname)) > (mp->max_print_line-2)) mp_print_ln(mp);
  else if ( (mp->term_offset>0)||(mp->file_offset>0) ) mp_print_char(mp, xord(' '));
  mp_print_char(mp, xord('(')); incr(mp->open_parens); mp_print(mp, fname); 
  xfree(fname);
  update_terminal;
  @<Flush |name| and replace it with |cur_name| if it won't be needed@>;
  @<Read the first line of the new file@>;
}

@ This code should be omitted if |a_make_name_string| returns something other
than just a copy of its argument and the full file name is needed for opening
\.{MPX} files or implementing the switch-to-editor option.
@^system dependencies@>

@<Flush |name| and replace it with |cur_name| if it won't be needed@>=
mp_flush_string(mp, name); name=rts(mp->cur_name); xfree(mp->cur_name)

@ If the file is empty, it is considered to contain a single blank line,
so there is no need to test the return value.

@<Read the first line...@>=
{ 
  line=1;
  (void)mp_input_ln(mp, cur_file ); 
  mp_firm_up_the_line(mp);
  mp->buffer[limit]=xord('%'); mp->first=(size_t)(limit+1); loc=start;
}

@ @<Put the desired file name in |(cur_name,cur_ext,cur_area)|@>=
while ( token_state &&(loc==null) ) mp_end_token_list(mp);
if ( token_state ) { 
  print_err("File names can't appear within macros");
@.File names can't...@>
  help3("Sorry...I've converted what follows to tokens,",
    "possibly garbaging the name you gave.",
    "Please delete the tokens and insert the name again.");
  mp_error(mp);
}
if ( file_state ) {
  mp_scan_file_name(mp);
} else { 
   xfree(mp->cur_name); mp->cur_name=xstrdup(""); 
   xfree(mp->cur_ext);  mp->cur_ext =xstrdup(""); 
   xfree(mp->cur_area); mp->cur_area=xstrdup(""); 
}

@ The following simple routine starts reading the \.{MPX} file associated
with the current input file.

@c void mp_start_mpx_input (MP mp) {
  char *origname = NULL; /* a copy of nameoffile */
  mp_pack_file_name(mp, in_name, in_area, in_ext);
  origname = xstrdup(mp->name_of_file);
  mp_pack_file_name(mp, in_name, in_area, ".mpx");
  if (!(mp->run_make_mpx)(mp, origname, mp->name_of_file))
    goto NOT_FOUND;
  mp_begin_file_reading(mp);
  if ( ! mp_a_open_in(mp, &cur_file, mp_filetype_program) ) {
    mp_end_file_reading(mp);
    goto NOT_FOUND;
  }
  name=mp_a_make_name_string(mp, cur_file);
  mp->mpx_name[iindex]=name; add_str_ref(name);
  @<Read the first line of the new file@>;
  xfree(origname);
  return;
NOT_FOUND: 
    @<Explain that the \.{MPX} file can't be read and |succumb|@>;
  xfree(origname);
}

@ This should ideally be changed to do whatever is necessary to create the
\.{MPX} file given by |name_of_file| if it does not exist or if it is out
of date.  This requires invoking \.{MPtoTeX} on the |origname| and passing
the results through \TeX\ and \.{DVItoMP}.  (It is possible to use a
completely different typesetting program if suitable postprocessor is
available to perform the function of \.{DVItoMP}.)
@^system dependencies@>

@ @<Exported types@>=
typedef int (*mp_makempx_cmd)(MP mp, char *origname, char *mtxname);

@ @<Option variables@>=
mp_makempx_cmd run_make_mpx;

@ @<Allocate or initialize ...@>=
set_callback_option(run_make_mpx);

@ @<Declarations@>=
static int mp_run_make_mpx (MP mp, char *origname, char *mtxname);

@ The default does nothing.
@c 
int mp_run_make_mpx (MP mp, char *origname, char *mtxname) {
  (void)mp;
  (void)origname;
  (void)mtxname;
  return false;
}

@ @<Explain that the \.{MPX} file can't be read and |succumb|@>=
if ( mp->interaction==mp_error_stop_mode ) wake_up_terminal;
mp_print_nl(mp, ">> ");
mp_print(mp, origname);
mp_print_nl(mp, ">> ");
mp_print(mp, mp->name_of_file);
mp_print_nl(mp, "! Unable to make mpx file");
help4("The two files given above are one of your source files",
  "and an auxiliary file I need to read to find out what your",
  "btex..etex blocks mean. If you don't know why I had trouble,",
  "try running it manually through MPtoTeX, TeX, and DVItoMP");
xfree(origname);
succumb;

@ The last file-opening commands are for files accessed via the \&{readfrom}
@:read_from_}{\&{readfrom} primitive@>
operator and the \&{write} command.  Such files are stored in separate arrays.
@:write_}{\&{write} primitive@>

@<Types in the outer block@>=
typedef unsigned int readf_index; /* |0..max_read_files| */
typedef unsigned int write_index;  /* |0..max_write_files| */

@ @<Glob...@>=
readf_index max_read_files; /* maximum number of simultaneously open \&{readfrom} files */
void ** rd_file; /* \&{readfrom} files */
char ** rd_fname; /* corresponding file name or 0 if file not open */
readf_index read_files; /* number of valid entries in the above arrays */
write_index max_write_files; /* maximum number of simultaneously open \&{write} */
void ** wr_file; /* \&{write} files */
char ** wr_fname; /* corresponding file name or 0 if file not open */
write_index write_files; /* number of valid entries in the above arrays */

@ @<Allocate or initialize ...@>=
mp->max_read_files=8;
mp->rd_file = xmalloc((mp->max_read_files+1),sizeof(void *));
mp->rd_fname = xmalloc((mp->max_read_files+1),sizeof(char *));
memset(mp->rd_fname, 0, sizeof(char *)*(mp->max_read_files+1));
mp->max_write_files=8;
mp->wr_file = xmalloc((mp->max_write_files+1),sizeof(void *));
mp->wr_fname = xmalloc((mp->max_write_files+1),sizeof(char *));
memset(mp->wr_fname, 0, sizeof(char *)*(mp->max_write_files+1));


@ This routine starts reading the file named by string~|s| without setting
|loc|, |limit|, or |name|.  It returns |false| if the file is empty or cannot
be opened.  Otherwise it updates |rd_file[n]| and |rd_fname[n]|.

@c 
static boolean mp_start_read_input (MP mp,char *s, readf_index  n) {
  mp_ptr_scan_file(mp, s);
  pack_cur_name;
  mp_begin_file_reading(mp);
  if ( ! mp_a_open_in(mp, &mp->rd_file[n], (int)(mp_filetype_text+n)) ) 
	goto NOT_FOUND;
  if ( ! mp_input_ln(mp, mp->rd_file[n] ) ) {
    (mp->close_file)(mp,mp->rd_file[n]); 
	goto NOT_FOUND; 
  }
  mp->rd_fname[n]=xstrdup(s);
  return true;
NOT_FOUND: 
  mp_end_file_reading(mp);
  return false;
}

@ Open |wr_file[n]| using file name~|s| and update |wr_fname[n]|.

@<Declarations@>=
static void mp_open_write_file (MP mp, char *s, readf_index  n) ;

@ @c void mp_open_write_file (MP mp,char *s, readf_index  n) {
  mp_ptr_scan_file(mp, s);
  pack_cur_name;
  while ( ! mp_a_open_out(mp, &mp->wr_file[n], (int)(mp_filetype_text+n)) )
    mp_prompt_file_name(mp, "file name for write output","");
  mp->wr_fname[n]=xstrdup(s);
}


@* \[36] Introduction to the parsing routines.
We come now to the central nervous system that sparks many of \MP's activities.
By evaluating expressions, from their primary constituents to ever larger
subexpressions, \MP\ builds the structures that ultimately define complete
pictures or fonts of type.

Four mutually recursive subroutines are involved in this process: We call them
$$\hbox{|scan_primary|, |scan_secondary|, |scan_tertiary|,
and |scan_expression|.}$$
@^recursion@>
Each of them is parameterless and begins with the first token to be scanned
already represented in |cur_cmd|, |cur_mod|, and |cur_sym|. After execution,
the value of the primary or secondary or tertiary or expression that was
found will appear in the global variables |cur_type| and |cur_exp|. The
token following the expression will be represented in |cur_cmd|, |cur_mod|,
and |cur_sym|.

Technically speaking, the parsing algorithms are ``LL(1),'' more or less;
backup mechanisms have been added in order to provide reasonable error
recovery.

@<Glob...@>=
quarterword cur_type; /* the type of the expression just found */
integer cur_exp; /* the value of the expression just found */

@ @<Set init...@>=
mp->cur_exp=0;

@ Many different kinds of expressions are possible, so it is wise to have
precise descriptions of what |cur_type| and |cur_exp| mean in all cases:

\smallskip\hang
|cur_type=mp_vacuous| means that this expression didn't turn out to have a
value at all, because it arose from a \&{begingroup}$\,\ldots\,$\&{endgroup}
construction in which there was no expression before the \&{endgroup}.
In this case |cur_exp| has some irrelevant value.

\smallskip\hang
|cur_type=mp_boolean_type| means that |cur_exp| is either |true_code|
or |false_code|.

\smallskip\hang
|cur_type=mp_unknown_boolean| means that |cur_exp| points to a capsule
node that is in 
a ring of equivalent booleans whose value has not yet been defined.

\smallskip\hang
|cur_type=mp_string_type| means that |cur_exp| is a string number (i.e., an
integer in the range |0<=cur_exp<str_ptr|). That string's reference count
includes this particular reference.

\smallskip\hang
|cur_type=mp_unknown_string| means that |cur_exp| points to a capsule
node that is in
a ring of equivalent strings whose value has not yet been defined.

\smallskip\hang
|cur_type=mp_pen_type| means that |cur_exp| points to a node in a pen.  Nobody
else points to any of the nodes in this pen.  The pen may be polygonal or
elliptical.

\smallskip\hang
|cur_type=mp_unknown_pen| means that |cur_exp| points to a capsule
node that is in
a ring of equivalent pens whose value has not yet been defined.

\smallskip\hang
|cur_type=mp_path_type| means that |cur_exp| points to a the first node of
a path; nobody else points to this particular path. The control points of
the path will have been chosen.

\smallskip\hang
|cur_type=mp_unknown_path| means that |cur_exp| points to a capsule
node that is in
a ring of equivalent paths whose value has not yet been defined.

\smallskip\hang
|cur_type=mp_picture_type| means that |cur_exp| points to an edge header node.
There may be other pointers to this particular set of edges.  The header node
contains a reference count that includes this particular reference.

\smallskip\hang
|cur_type=mp_unknown_picture| means that |cur_exp| points to a capsule
node that is in
a ring of equivalent pictures whose value has not yet been defined.

\smallskip\hang
|cur_type=mp_transform_type| means that |cur_exp| points to a |mp_transform_type|
capsule node. The |value| part of this capsule
points to a transform node that contains six numeric values,
each of which is |independent|, |dependent|, |mp_proto_dependent|, or |known|.

\smallskip\hang
|cur_type=mp_color_type| means that |cur_exp| points to a |color_type|
capsule node. The |value| part of this capsule
points to a color node that contains three numeric values,
each of which is |independent|, |dependent|, |mp_proto_dependent|, or |known|.

\smallskip\hang
|cur_type=mp_cmykcolor_type| means that |cur_exp| points to a |mp_cmykcolor_type|
capsule node. The |value| part of this capsule
points to a color node that contains four numeric values,
each of which is |independent|, |dependent|, |mp_proto_dependent|, or |known|.

\smallskip\hang
|cur_type=mp_pair_type| means that |cur_exp| points to a capsule
node whose type is |mp_pair_type|. The |value| part of this capsule
points to a pair node that contains two numeric values,
each of which is |independent|, |dependent|, |mp_proto_dependent|, or |known|.

\smallskip\hang
|cur_type=mp_known| means that |cur_exp| is a |scaled| value.

\smallskip\hang
|cur_type=mp_dependent| means that |cur_exp| points to a capsule node whose type
is |dependent|. The |dep_list| field in this capsule points to the associated
dependency list.

\smallskip\hang
|cur_type=mp_proto_dependent| means that |cur_exp| points to a |mp_proto_dependent|
capsule node. The |dep_list| field in this capsule
points to the associated dependency list.

\smallskip\hang
|cur_type=independent| means that |cur_exp| points to a capsule node
whose type is |independent|. This somewhat unusual case can arise, for
example, in the expression
`$x+\&{begingroup}\penalty0\,\&{string}\,x; 0\,\&{endgroup}$'.

\smallskip\hang
|cur_type=mp_token_list| means that |cur_exp| points to a linked list of
tokens. 

\smallskip\noindent
The possible settings of |cur_type| have been listed here in increasing
numerical order. Notice that |cur_type| will never be |mp_numeric_type| or
|suffixed_macro| or |mp_unsuffixed_macro|, although variables of those types
are allowed.  Conversely, \MP\ has no variables of type |mp_vacuous| or
|token_list|.

@ Capsules are two-word nodes that have a similar meaning
to |cur_type| and |cur_exp|. Such nodes have |name_type=capsule|,
and their |type| field is one of the possibilities for |cur_type| listed above.
Also |link<=void| in capsules that aren't part of a token list.

The |value| field of a capsule is, in most cases, the value that
corresponds to its |type|, as |cur_exp| corresponds to |cur_type|.
However, when |cur_exp| would point to a capsule,
no extra layer of indirection is present; the |value|
field is what would have been called |value(cur_exp)| if it had not been
encapsulated.  Furthermore, if the type is |dependent| or
|mp_proto_dependent|, the |value| field of a capsule is replaced by
|dep_list| and |prev_dep| fields, since dependency lists in capsules are
always part of the general |dep_list| structure.

The |get_x_next| routine is careful not to change the values of |cur_type|
and |cur_exp| when it gets an expanded token. However, |get_x_next| might
call a macro, which might parse an expression, which might execute lots of
commands in a group; hence it's possible that |cur_type| might change
from, say, |mp_unknown_boolean| to |mp_boolean_type|, or from |dependent| to
|known| or |independent|, during the time |get_x_next| is called. The
programs below are careful to stash sensitive intermediate results in
capsules, so that \MP's generality doesn't cause trouble.

Here's a procedure that illustrates these conventions. It takes
the contents of $(|cur_type|\kern-.3pt,|cur_exp|\kern-.3pt)$
and stashes them away in a
capsule. It is not used when |cur_type=mp_token_list|.
After the operation, |cur_type=mp_vacuous|; hence there is no need to
copy path lists or to update reference counts, etc.

The special link |mp_void| is put on the capsule returned by
|stash_cur_exp|, because this procedure is used to store macro parameters
that must be easily distinguishable from token lists.

@<Declare the stashing/unstashing routines@>=
static pointer mp_stash_cur_exp (MP mp) {
  pointer p; /* the capsule that will be returned */
  switch (mp->cur_type) {
  case unknown_types:
  case mp_transform_type:
  case mp_color_type:
  case mp_pair_type:
  case mp_dependent:
  case mp_proto_dependent:
  case mp_independent: 
  case mp_cmykcolor_type:
    p=mp->cur_exp;
    break;
  default: 
    p=mp_get_node(mp, value_node_size); mp_name_type(p)=mp_capsule;
    mp_type(p)=mp->cur_type; value(p)=mp->cur_exp;
    break;
  }
  mp->cur_type=mp_vacuous; mp_link(p)=mp_void; 
  return p;
}

@ The inverse of |stash_cur_exp| is the following procedure, which
deletes an unnecessary capsule and puts its contents into |cur_type|
and |cur_exp|.

The program steps of \MP\ can be divided into two categories: those in
which |cur_type| and |cur_exp| are ``alive'' and those in which they are
``dead,'' in the sense that |cur_type| and |cur_exp| contain relevant
information or not. It's important not to ignore them when they're alive,
and it's important not to pay attention to them when they're dead.

There's also an intermediate category: If |cur_type=mp_vacuous|, then
|cur_exp| is irrelevant, hence we can proceed without caring if |cur_type|
and |cur_exp| are alive or dead. In such cases we say that |cur_type|
and |cur_exp| are {\sl dormant}. It is permissible to call |get_x_next|
only when they are alive or dormant.

The \\{stash} procedure above assumes that |cur_type| and |cur_exp|
are alive or dormant. The \\{unstash} procedure assumes that they are
dead or dormant; it resuscitates them.

@<Declare the stashing/unstashing...@>=
static void mp_unstash_cur_exp (MP mp,pointer p) ;

@ @c
void mp_unstash_cur_exp (MP mp,pointer p) { 
  mp->cur_type=mp_type(p);
  switch (mp->cur_type) {
  case unknown_types:
  case mp_transform_type:
  case mp_color_type:
  case mp_pair_type:
  case mp_dependent: 
  case mp_proto_dependent:
  case mp_independent:
  case mp_cmykcolor_type: 
    mp->cur_exp=p;
    break;
  default:
    mp->cur_exp=value(p);
    mp_free_node(mp, p,value_node_size);
    break;
  }
}

@ The following procedure prints the values of expressions in an
abbreviated format. If its first parameter |p| is null, the value of
|(cur_type,cur_exp)| is displayed; otherwise |p| should be a capsule
containing the desired value. The second parameter controls the amount of
output. If it is~0, dependency lists will be abbreviated to
`\.{linearform}' unless they consist of a single term.  If it is greater
than~1, complicated structures (pens, pictures, and paths) will be displayed
in full.
@.linearform@>

@<Declarations@>=
@<Declare the procedure called |print_dp|@>
@<Declare the stashing/unstashing routines@>
static void mp_print_exp (MP mp,pointer p, quarterword verbosity) ;

@ @c
void mp_print_exp (MP mp,pointer p, quarterword verbosity) {
  boolean restore_cur_exp; /* should |cur_exp| be restored? */
  quarterword t; /* the type of the expression */
  pointer q; /* a big node being displayed */
  integer v=0; /* the value of the expression */
  if ( p!=null ) {
    restore_cur_exp=false;
  } else { 
    p=mp_stash_cur_exp(mp); restore_cur_exp=true;
  }
  t=mp_type(p);
  if ( t<mp_dependent ) v=value(p); else if ( t<mp_independent ) v=dep_list(p);
  @<Print an abbreviated value of |v| with format depending on |t|@>;
  if ( restore_cur_exp ) mp_unstash_cur_exp(mp, p);
}

@ @<Print an abbreviated value of |v| with format depending on |t|@>=
switch (t) {
case mp_vacuous:mp_print(mp, "vacuous"); break;
case mp_boolean_type:
  if ( v==true_code ) mp_print(mp, "true"); else mp_print(mp, "false");
  break;
case unknown_types: case mp_numeric_type:
  @<Display a variable that's been declared but not defined@>;
  break;
case mp_string_type:
  mp_print_char(mp, xord('"')); mp_print_str(mp, v); mp_print_char(mp, xord('"'));
  break;
case mp_pen_type: case mp_path_type: case mp_picture_type:
  @<Display a complex type@>;
  break;
case mp_transform_type: case mp_color_type: case mp_pair_type: case mp_cmykcolor_type:
  if ( v==null ) mp_print_type(mp, t);
  else @<Display a big node@>;
  break;
case mp_known:mp_print_scaled(mp, v); break;
case mp_dependent: case mp_proto_dependent:
  mp_print_dp(mp, t,v,verbosity);
  break;
case mp_independent:mp_print_variable_name(mp, p); break;
default: mp_confusion(mp, "exp"); break;
@:this can't happen exp}{\quad exp@>
}

@ @<Display a big node@>=
{ 
  mp_print_char(mp, xord('(')); q=v+mp->big_node_size[t];
  do {  
    if ( mp_type(v)==mp_known ) mp_print_scaled(mp, value(v));
    else if ( mp_type(v)==mp_independent ) mp_print_variable_name(mp, v);
    else mp_print_dp(mp, mp_type(v),dep_list(v),verbosity);
    v=v+2;
    if ( v!=q ) mp_print_char(mp, xord(','));
  } while (v!=q);
  mp_print_char(mp, xord(')'));
}

@ Values of type \&{picture}, \&{path}, and \&{pen} are displayed verbosely
in the log file only, unless the user has given a positive value to
\\{tracingonline}.

@<Display a complex type@>=
if ( verbosity<=1 ) {
  mp_print_type(mp, t);
} else { 
  if ( mp->selector==term_and_log )
   if ( mp->internal[mp_tracing_online]<=0 ) {
    mp->selector=term_only;
    mp_print_type(mp, t); mp_print(mp, " (see the transcript file)");
    mp->selector=term_and_log;
  };
  switch (t) {
  case mp_pen_type:mp_print_pen(mp, v,"",false); break;
  case mp_path_type:mp_print_path(mp, v,"",false); break;
  case mp_picture_type:mp_print_edges(mp, v,"",false); break;
  } /* there are no other cases */
}

@ @<Declare the procedure called |print_dp|@>=
static void mp_print_dp (MP mp, quarterword t, pointer p, 
                  quarterword verbosity)  {
  pointer q; /* the node following |p| */
  q=mp_link(p);
  if ( (mp_info(q)==null) || (verbosity>0) ) mp_print_dependency(mp, p,t);
  else mp_print(mp, "linearform");
}

@ The displayed name of a variable in a ring will not be a capsule unless
the ring consists entirely of capsules.

@<Display a variable that's been declared but not defined@>=
{ mp_print_type(mp, t);
if ( v!=null )
  { mp_print_char(mp, xord(' '));
    while ( (mp_name_type(v)==mp_capsule) && (v!=p) ) v=value(v);
    mp_print_variable_name(mp, v);
  };
}

@ When errors are detected during parsing, it is often helpful to
display an expression just above the error message, using |exp_err|
or |disp_err| instead of |print_err|.

@d exp_err(A) mp_disp_err(mp, null,(A)) /* displays the current expression */

@<Declarations@>=
static void mp_disp_err (MP mp,pointer p, const char *s) ;

@ @c
void mp_disp_err (MP mp,pointer p, const char *s) { 
  if ( mp->interaction==mp_error_stop_mode ) wake_up_terminal;
  mp_print_nl(mp, ">> ");
@.>>@>
  mp_print_exp(mp, p,1); /* ``medium verbose'' printing of the expression */
  if (strlen(s)>0) { 
    print_err(s);
  }
}

@ If |cur_type| and |cur_exp| contain relevant information that should
be recycled, we will use the following procedure, which changes |cur_type|
to |known| and stores a given value in |cur_exp|. We can think of |cur_type|
and |cur_exp| as either alive or dormant after this has been done,
because |cur_exp| will not contain a pointer value.

@ @c 
static void mp_flush_cur_exp (MP mp,scaled v) { 
  switch (mp->cur_type) {
  case unknown_types: case mp_transform_type: case mp_color_type: case mp_pair_type:
  case mp_dependent: case mp_proto_dependent: case mp_independent: case mp_cmykcolor_type:
    mp_recycle_value(mp, mp->cur_exp); 
    mp_free_node(mp, mp->cur_exp,value_node_size);
    break;
  case mp_string_type:
    delete_str_ref(mp->cur_exp); break;
  case mp_pen_type: case mp_path_type: 
    mp_toss_knot_list(mp, mp->cur_exp); break;
  case mp_picture_type:
    delete_edge_ref(mp->cur_exp); break;
  default: 
    break;
  }
  mp->cur_type=mp_known; mp->cur_exp=v;
}

@ There's a much more general procedure that is capable of releasing
the storage associated with any two-word value packet.

@<Declarations@>=
static void mp_recycle_value (MP mp,pointer p) ;

@ @c 
static void mp_recycle_value (MP mp,pointer p) {
  quarterword t; /* a type code */
  integer vv; /* another value */
  pointer q,r,s,pp; /* link manipulation registers */
  integer v=0; /* a value */
  t=mp_type(p);
  if ( t<mp_dependent ) v=value(p);
  switch (t) {
  case undefined: case mp_vacuous: case mp_boolean_type: case mp_known:
  case mp_numeric_type:
    break;
  case unknown_types:
    mp_ring_delete(mp, p); break;
  case mp_string_type:
    delete_str_ref(v); break;
  case mp_path_type: case mp_pen_type:
    mp_toss_knot_list(mp, v); break;
  case mp_picture_type:
    delete_edge_ref(v); break;
  case mp_cmykcolor_type: case mp_pair_type: case mp_color_type:
  case mp_transform_type:
    @<Recycle a big node@>; break; 
  case mp_dependent: case mp_proto_dependent:
    @<Recycle a dependency list@>; break;
  case mp_independent:
    @<Recycle an independent variable@>; break;
  case mp_token_list: case mp_structured:
    mp_confusion(mp, "recycle"); break;
@:this can't happen recycle}{\quad recycle@>
  case mp_unsuffixed_macro: case mp_suffixed_macro:
    mp_delete_mac_ref(mp, value(p)); break;
  } /* there are no other cases */
  mp_type(p)=undefined;
}

@ @<Recycle a big node@>=
if ( v!=null ){ 
  q=v+mp->big_node_size[t];
  do {  
    q=q-2; mp_recycle_value(mp, q);
  } while (q!=v);
  mp_free_node(mp, v,mp->big_node_size[t]);
}

@ @<Recycle a dependency list@>=
{ 
  q=dep_list(p);
  while ( mp_info(q)!=null ) q=mp_link(q);
  mp_link(prev_dep(p))=mp_link(q);
  prev_dep(mp_link(q))=prev_dep(p);
  mp_link(q)=null; mp_flush_node_list(mp, dep_list(p));
}

@ When an independent variable disappears, it simply fades away, unless
something depends on it. In the latter case, a dependent variable whose
coefficient of dependence is maximal will take its place.
The relevant algorithm is due to Ignacio~A. Zabala, who implemented it
as part of his Ph.D. thesis (Stanford University, December 1982).
@^Zabala Salelles, Ignacio Andr\'es@>

For example, suppose that variable $x$ is being recycled, and that the
only variables depending on~$x$ are $y=2x+a$ and $z=x+b$. In this case
we want to make $y$ independent and $z=.5y-.5a+b$; no other variables
will depend on~$y$. If $\\{tracingequations}>0$ in this situation,
we will print `\.{\#\#\# -2x=-y+a}'.

There's a slight complication, however: An independent variable $x$
can occur both in dependency lists and in proto-dependency lists.
This makes it necessary to be careful when deciding which coefficient
is maximal.

Furthermore, this complication is not so slight when
a proto-dependent variable is chosen to become independent. For example,
suppose that $y=2x+100a$ is proto-dependent while $z=x+b$ is dependent;
then we must change $z=.5y-50a+b$ to a proto-dependency, because of the
large coefficient `50'.

In order to deal with these complications without wasting too much time,
we shall link together the occurrences of~$x$ among all the linear
dependencies, maintaining separate lists for the dependent and
proto-dependent cases.

@<Recycle an independent variable@>=
{ 
  mp->max_c[mp_dependent]=0; mp->max_c[mp_proto_dependent]=0;
  mp->max_link[mp_dependent]=null; mp->max_link[mp_proto_dependent]=null;
  q=mp_link(dep_head);
  while ( q!=dep_head ) { 
    s=value_loc(q); /* now |mp_link(s)=dep_list(q)| */
    while (1) { 
      r=mp_link(s);
      if ( mp_info(r)==null ) break;
      if ( mp_info(r)!=p ) { 
        s=r;
      } else  { 
        t=mp_type(q); mp_link(s)=mp_link(r); mp_info(r)=q;
        if ( abs(value(r))>mp->max_c[t] ) {
          @<Record a new maximum coefficient of type |t|@>;
        } else { 
          mp_link(r)=mp->max_link[t]; mp->max_link[t]=r;
        }
      }
    } 
    q=mp_link(r);
  }
  if ( (mp->max_c[mp_dependent]>0)||(mp->max_c[mp_proto_dependent]>0) ) {
    @<Choose a dependent variable to take the place of the disappearing
    independent variable, and change all remaining dependencies
    accordingly@>;
  }
}

@ The code for independency removal makes use of three two-word arrays.

@<Glob...@>=
integer max_c[mp_proto_dependent+1];  /* max coefficient magnitude */
pointer max_ptr[mp_proto_dependent+1]; /* where |p| occurs with |max_c| */
pointer max_link[mp_proto_dependent+1]; /* other occurrences of |p| */

@ @<Record a new maximum coefficient...@>=
{ 
  if ( mp->max_c[t]>0 ) {
    mp_link(mp->max_ptr[t])=mp->max_link[t]; mp->max_link[t]=mp->max_ptr[t];
  }
  mp->max_c[t]=abs(value(r)); mp->max_ptr[t]=r;
}

@ @<Choose a dependent...@>=
{ 
  if ( (mp->max_c[mp_dependent] / 010000) >= mp->max_c[mp_proto_dependent] )
    t=mp_dependent;
  else 
    t=mp_proto_dependent;
  @<Determine the dependency list |s| to substitute for the independent
    variable~|p|@>;
  t=(quarterword)(mp_dependent+mp_proto_dependent-t); /* complement |t| */
  if ( mp->max_c[t]>0 ) { /* we need to pick up an unchosen dependency */ 
    mp_link(mp->max_ptr[t])=mp->max_link[t]; mp->max_link[t]=mp->max_ptr[t];
  }
  if ( t!=mp_dependent ) { @<Substitute new dependencies in place of |p|@>; }
  else { @<Substitute new proto-dependencies in place of |p|@>;}
  mp_flush_node_list(mp, s);
  if ( mp->fix_needed ) mp_fix_dependencies(mp);
  check_arith;
}

@ Let |s=max_ptr[t]|. At this point we have $|value|(s)=\pm|max_c|[t]$,
and |mp_info(s)| points to the dependent variable~|pp| of type~|t| from
whose dependency list we have removed node~|s|. We must reinsert
node~|s| into the dependency list, with coefficient $-1.0$, and with
|pp| as the new independent variable. Since |pp| will have a larger serial
number than any other variable, we can put node |s| at the head of the
list.

@<Determine the dep...@>=
s=mp->max_ptr[t]; pp=mp_info(s); v=value(s);
if ( t==mp_dependent ) value(s)=-fraction_one; else value(s)=-unity;
r=dep_list(pp); mp_link(s)=r;
while ( mp_info(r)!=null ) r=mp_link(r);
q=mp_link(r); mp_link(r)=null;
prev_dep(q)=prev_dep(pp); mp_link(prev_dep(pp))=q;
new_indep(pp);
if ( mp->cur_exp==pp ) if ( mp->cur_type==t ) mp->cur_type=mp_independent;
if ( mp->internal[mp_tracing_equations]>0 ) { 
  @<Show the transformed dependency@>; 
}

@ Now $(-v)$ times the formerly independent variable~|p| is being replaced
by the dependency list~|s|.

@<Show the transformed...@>=
if ( mp_interesting(mp, p) ) {
  mp_begin_diagnostic(mp); mp_print_nl(mp, "### ");
@:]]]\#\#\#_}{\.{\#\#\#}@>
  if ( v>0 ) mp_print_char(mp, xord('-'));
  if ( t==mp_dependent ) vv=mp_round_fraction(mp, mp->max_c[mp_dependent]);
  else vv=mp->max_c[mp_proto_dependent];
  if ( vv!=unity ) mp_print_scaled(mp, vv);
  mp_print_variable_name(mp, p);
  while ( value(p) % s_scale>0 ) {
    mp_print(mp, "*4"); value(p)=value(p)-2;
  }
  if ( t==mp_dependent ) mp_print_char(mp, xord('=')); else mp_print(mp, " = ");
  mp_print_dependency(mp, s,t);
  mp_end_diagnostic(mp, false);
}

@ Finally, there are dependent and proto-dependent variables whose
dependency lists must be brought up to date.

@<Substitute new dependencies...@>=
for (t=mp_dependent;t<=mp_proto_dependent;t++){ 
  r=mp->max_link[t];
  while ( r!=null ) {
    q=mp_info(r);
    dep_list(q)=mp_p_plus_fq(mp, dep_list(q),
     mp_make_fraction(mp, value(r),-v),s,t,mp_dependent);
    if ( dep_list(q)==mp->dep_final ) mp_make_known(mp, q,mp->dep_final);
    q=r; r=mp_link(r); mp_free_node(mp, q,dep_node_size);
  }
}

@ @<Substitute new proto...@>=
for (t=mp_dependent;t<=mp_proto_dependent;t++) {
  r=mp->max_link[t];
  while ( r!=null ) {
    q=mp_info(r);
    if ( t==mp_dependent ) { /* for safety's sake, we change |q| to |mp_proto_dependent| */
      if ( mp->cur_exp==q ) if ( mp->cur_type==mp_dependent )
        mp->cur_type=mp_proto_dependent;
      dep_list(q)=mp_p_over_v(mp, dep_list(q),unity,
         mp_dependent,mp_proto_dependent);
      mp_type(q)=mp_proto_dependent; 
      value(r)=mp_round_fraction(mp, value(r));
    }
    dep_list(q)=mp_p_plus_fq(mp, dep_list(q),
       mp_make_scaled(mp, value(r),-v),s,
       mp_proto_dependent,mp_proto_dependent);
    if ( dep_list(q)==mp->dep_final ) 
       mp_make_known(mp, q,mp->dep_final);
    q=r; r=mp_link(r); mp_free_node(mp, q,dep_node_size);
  }
}

@ Here are some routines that provide handy combinations of actions
that are often needed during error recovery. For example,
`|flush_error|' flushes the current expression, replaces it by
a given value, and calls |error|.

Errors often are detected after an extra token has already been scanned.
The `\\{put\_get}' routines put that token back before calling |error|;
then they get it back again. (Or perhaps they get another token, if
the user has changed things.)

@<Declarations@>=
static void mp_flush_error (MP mp,scaled v);
static void mp_put_get_error (MP mp);
static void mp_put_get_flush_error (MP mp,scaled v) ;

@ @c
void mp_flush_error (MP mp,scaled v) { 
  mp_error(mp); mp_flush_cur_exp(mp, v); 
}
void mp_put_get_error (MP mp) { 
  mp_back_error(mp); mp_get_x_next(mp); 
}
void mp_put_get_flush_error (MP mp,scaled v) { 
  mp_put_get_error(mp);
  mp_flush_cur_exp(mp, v); 
}

@ A global variable |var_flag| is set to a special command code
just before \MP\ calls |scan_expression|, if the expression should be
treated as a variable when this command code immediately follows. For
example, |var_flag| is set to |assignment| at the beginning of a
statement, because we want to know the {\sl location\/} of a variable at
the left of `\.{:=}', not the {\sl value\/} of that variable.

The |scan_expression| subroutine calls |scan_tertiary|,
which calls |scan_secondary|, which calls |scan_primary|, which sets
|var_flag:=0|. In this way each of the scanning routines ``knows''
when it has been called with a special |var_flag|, but |var_flag| is
usually zero.

A variable preceding a command that equals |var_flag| is converted to a
token list rather than a value. Furthermore, an `\.{=}' sign following an
expression with |var_flag=assignment| is not considered to be a relation
that produces boolean expressions.


@<Glob...@>=
int var_flag; /* command that wants a variable */

@ @<Set init...@>=
mp->var_flag=0;

@* \[37] Parsing primary expressions.
The first parsing routine, |scan_primary|, is also the most complicated one,
since it involves so many different cases. But each case---with one
exception---is fairly simple by itself.

When |scan_primary| begins, the first token of the primary to be scanned
should already appear in |cur_cmd|, |cur_mod|, and |cur_sym|. The values
of |cur_type| and |cur_exp| should be either dead or dormant, as explained
earlier. If |cur_cmd| is not between |min_primary_command| and
|max_primary_command|, inclusive, a syntax error will be signaled.

@<Declare the basic parsing subroutines@>=
void mp_scan_primary (MP mp) {
  pointer p,q,r; /* for list manipulation */
  quarterword c; /* a primitive operation code */
  int my_var_flag; /* initial value of |my_var_flag| */
  pointer l_delim,r_delim; /* hash addresses of a delimiter pair */
  @<Other local variables for |scan_primary|@>;
  my_var_flag=mp->var_flag; mp->var_flag=0;
RESTART:
  check_arith;
  @<Supply diagnostic information, if requested@>;
  switch (mp->cur_cmd) {
  case left_delimiter:
    @<Scan a delimited primary@>; break;
  case begin_group:
    @<Scan a grouped primary@>; break;
  case string_token:
    @<Scan a string constant@>; break;
  case numeric_token:
    @<Scan a primary that starts with a numeric token@>; break;
  case nullary:
    @<Scan a nullary operation@>; break;
  case unary: case type_name: case cycle: case plus_or_minus:
    @<Scan a unary operation@>; break;
  case primary_binary:
    @<Scan a binary operation with `\&{of}' between its operands@>; break;
  case str_op:
    @<Convert a suffix to a string@>; break;
  case internal_quantity:
    @<Scan an internal numeric quantity@>; break;
  case capsule_token:
    mp_make_exp_copy(mp, mp->cur_mod); break;
  case tag_token:
    @<Scan a variable primary; |goto restart| if it turns out to be a macro@>; break;
  default: 
    mp_bad_exp(mp, "A primary"); goto RESTART; break;
@.A primary expression...@>
  }
  mp_get_x_next(mp); /* the routines |goto done| if they don't want this */
DONE: 
  if ( mp->cur_cmd==left_bracket ) {
    if ( mp->cur_type>=mp_known ) {
      @<Scan a mediation construction@>;
    }
  }
}



@ Errors at the beginning of expressions are flagged by |bad_exp|.

@c 
static void mp_bad_exp (MP mp, const char * s) {
  int save_flag;
  print_err(s); mp_print(mp, " expression can't begin with `");
  mp_print_cmd_mod(mp, mp->cur_cmd,mp->cur_mod); 
  mp_print_char(mp, xord('\''));
  help4("I'm afraid I need some sort of value in order to continue,",
    "so I've tentatively inserted `0'. You may want to",
    "delete this zero and insert something else;",
    "see Chapter 27 of The METAFONTbook for an example.");
@:METAFONTbook}{\sl The {\logos METAFONT\/}book@>
  mp_back_input(mp); mp->cur_sym=0; mp->cur_cmd=numeric_token; 
  mp->cur_mod=0; mp_ins_error(mp);
  save_flag=mp->var_flag; mp->var_flag=0; mp_get_x_next(mp);
  mp->var_flag=save_flag;
}

@ @<Supply diagnostic information, if requested@>=
#ifdef DEBUG
if ( mp->panicking ) mp_check_mem(mp, false);
#endif
if ( mp->interrupt!=0 ) if ( mp->OK_to_interrupt ) {
  mp_back_input(mp); check_interrupt; mp_get_x_next(mp);
}

@ @<Scan a delimited primary@>=
{ 
  l_delim=mp->cur_sym; r_delim=mp->cur_mod; 
  mp_get_x_next(mp); mp_scan_expression(mp);
  if ( (mp->cur_cmd==comma) && (mp->cur_type>=mp_known) ) {
    @<Scan the rest of a delimited set of numerics@>;
  } else {
    mp_check_delimiter(mp, l_delim,r_delim);
  }
}

@ The |stash_in| subroutine puts the current (numeric) expression into a field
within a ``big node.''

@c 
static void mp_stash_in (MP mp,pointer p) {
  pointer q; /* temporary register */
  mp_type(p)=mp->cur_type;
  if ( mp->cur_type==mp_known ) {
    value(p)=mp->cur_exp;
  } else { 
    if ( mp->cur_type==mp_independent ) {
      @<Stash an independent |cur_exp| into a big node@>;
    } else { 
      mp->mem[value_loc(p)]=mp->mem[value_loc(mp->cur_exp)];
      /* |dep_list(p):=dep_list(cur_exp)| and |prev_dep(p):=prev_dep(cur_exp)| */
      mp_link(prev_dep(p))=p;
    }
    mp_free_node(mp, mp->cur_exp,value_node_size);
  }
  mp->cur_type=mp_vacuous;
}

@ In rare cases the current expression can become |independent|. There
may be many dependency lists pointing to such an independent capsule,
so we can't simply move it into place within a big node. Instead,
we copy it, then recycle it.

@ @<Stash an independent |cur_exp|...@>=
{ 
  q=mp_single_dependency(mp, mp->cur_exp);
  if ( q==mp->dep_final ){ 
    mp_type(p)=mp_known; value(p)=0; mp_free_node(mp, q,dep_node_size);
  } else { 
    mp_type(p)=mp_dependent; mp_new_dep(mp, p,q);
  }
  mp_recycle_value(mp, mp->cur_exp);
}

@ This code uses the fact that |red_part_loc| and |green_part_loc|
are synonymous with |x_part_loc| and |y_part_loc|.

@<Scan the rest of a delimited set of numerics@>=
{ 
p=mp_stash_cur_exp(mp);
mp_get_x_next(mp); mp_scan_expression(mp);
@<Make sure the second part of a pair or color has a numeric type@>;
q=mp_get_node(mp, value_node_size); mp_name_type(q)=mp_capsule;
if ( mp->cur_cmd==comma ) mp_type(q)=mp_color_type;
else mp_type(q)=mp_pair_type;
mp_init_big_node(mp, q); r=value(q);
mp_stash_in(mp, y_part_loc(r));
mp_unstash_cur_exp(mp, p);
mp_stash_in(mp, x_part_loc(r));
if ( mp->cur_cmd==comma ) {
  @<Scan the last of a triplet of numerics@>;
}
if ( mp->cur_cmd==comma ) {
  mp_type(q)=mp_cmykcolor_type;
  mp_init_big_node(mp, q); t=value(q);
  mp->mem[cyan_part_loc(t)]=mp->mem[red_part_loc(r)];
  value(cyan_part_loc(t))=value(red_part_loc(r));
  mp->mem[magenta_part_loc(t)]=mp->mem[green_part_loc(r)];
  value(magenta_part_loc(t))=value(green_part_loc(r));
  mp->mem[yellow_part_loc(t)]=mp->mem[blue_part_loc(r)];
  value(yellow_part_loc(t))=value(blue_part_loc(r));
  mp_recycle_value(mp, r);
  r=t;
  @<Scan the last of a quartet of numerics@>;
}
mp_check_delimiter(mp, l_delim,r_delim);
mp->cur_type=mp_type(q);
mp->cur_exp=q;
}

@ @<Make sure the second part of a pair or color has a numeric type@>=
if ( mp->cur_type<mp_known ) {
  exp_err("Nonnumeric ypart has been replaced by 0");
@.Nonnumeric...replaced by 0@>
  help4("I've started to scan a pair `(a,b)' or a color `(a,b,c)';",
    "but after finding a nice `a' I found a `b' that isn't",
    "of numeric type. So I've changed that part to zero.",
    "(The b that I didn't like appears above the error message.)");
  mp_put_get_flush_error(mp, 0);
}

@ @<Scan the last of a triplet of numerics@>=
{ 
  mp_get_x_next(mp); mp_scan_expression(mp);
  if ( mp->cur_type<mp_known ) {
    exp_err("Nonnumeric third part has been replaced by 0");
@.Nonnumeric...replaced by 0@>
    help3("I've just scanned a color `(a,b,c)' or cmykcolor(a,b,c,d); but the `c'",
      "isn't of numeric type. So I've changed that part to zero.",
      "(The c that I didn't like appears above the error message.)");
    mp_put_get_flush_error(mp, 0);
  }
  mp_stash_in(mp, blue_part_loc(r));
}

@ @<Scan the last of a quartet of numerics@>=
{ 
  mp_get_x_next(mp); mp_scan_expression(mp);
  if ( mp->cur_type<mp_known ) {
    exp_err("Nonnumeric blackpart has been replaced by 0");
@.Nonnumeric...replaced by 0@>
    help3("I've just scanned a cmykcolor `(c,m,y,k)'; but the `k' isn't",
      "of numeric type. So I've changed that part to zero.",
      "(The k that I didn't like appears above the error message.)");
    mp_put_get_flush_error(mp, 0);
  }
  mp_stash_in(mp, black_part_loc(r));
}

@ The local variable |group_line| keeps track of the line
where a \&{begingroup} command occurred; this will be useful
in an error message if the group doesn't actually end.

@<Other local variables for |scan_primary|@>=
integer group_line; /* where a group began */

@ @<Scan a grouped primary@>=
{ 
  group_line=mp_true_line(mp);
  if ( mp->internal[mp_tracing_commands]>0 ) show_cur_cmd_mod;
  save_boundary_item(p);
  do {  
    mp_do_statement(mp); /* ends with |cur_cmd>=semicolon| */
  } while (mp->cur_cmd==semicolon);
  if ( mp->cur_cmd!=end_group ) {
    print_err("A group begun on line ");
@.A group...never ended@>
    mp_print_int(mp, group_line);
    mp_print(mp, " never ended");
    help2("I saw a `begingroup' back there that hasn't been matched",
          "by `endgroup'. So I've inserted `endgroup' now.");
    mp_back_error(mp); mp->cur_cmd=end_group;
  }
  mp_unsave(mp); 
    /* this might change |cur_type|, if independent variables are recycled */
  if ( mp->internal[mp_tracing_commands]>0 ) show_cur_cmd_mod;
}

@ @<Scan a string constant@>=
{ 
  mp->cur_type=mp_string_type; mp->cur_exp=mp->cur_mod;
}

@ Later we'll come to procedures that perform actual operations like
addition, square root, and so on; our purpose now is to do the parsing.
But we might as well mention those future procedures now, so that the
suspense won't be too bad:

\smallskip
|do_nullary(c)| does primitive operations that have no operands (e.g.,
`\&{true}' or `\&{pencircle}');

\smallskip
|do_unary(c)| applies a primitive operation to the current expression;

\smallskip
|do_binary(p,c)| applies a primitive operation to the capsule~|p|
and the current expression.

@<Scan a nullary operation@>=mp_do_nullary(mp, (quarterword)mp->cur_mod)

@ @<Scan a unary operation@>=
{ 
  c=(quarterword)mp->cur_mod; mp_get_x_next(mp); mp_scan_primary(mp); 
  mp_do_unary(mp, c); goto DONE;
}

@ A numeric token might be a primary by itself, or it might be the
numerator of a fraction composed solely of numeric tokens, or it might
multiply the primary that follows (provided that the primary doesn't begin
with a plus sign or a minus sign). The code here uses the facts that
|max_primary_command=plus_or_minus| and
|max_primary_command-1=numeric_token|. If a fraction is found that is less
than unity, we try to retain higher precision when we use it in scalar
multiplication.

@<Other local variables for |scan_primary|@>=
scaled num,denom; /* for primaries that are fractions, like `1/2' */

@ @<Scan a primary that starts with a numeric token@>=
{ 
  mp->cur_exp=mp->cur_mod; mp->cur_type=mp_known; mp_get_x_next(mp);
  if ( mp->cur_cmd!=slash ) { 
    num=0; denom=0;
  } else { 
    mp_get_x_next(mp);
    if ( mp->cur_cmd!=numeric_token ) { 
      mp_back_input(mp);
      mp->cur_cmd=slash; mp->cur_mod=over; mp->cur_sym=frozen_slash;
      goto DONE;
    }
    num=mp->cur_exp; denom=mp->cur_mod;
    if ( denom==0 ) { @<Protest division by zero@>; }
    else { mp->cur_exp=mp_make_scaled(mp, num,denom); }
    check_arith; mp_get_x_next(mp);
  }
  if ( mp->cur_cmd>=min_primary_command ) {
   if ( mp->cur_cmd<numeric_token ) { /* in particular, |cur_cmd<>plus_or_minus| */
     p=mp_stash_cur_exp(mp); mp_scan_primary(mp);
     if ( (abs(num)>=abs(denom))||(mp->cur_type<mp_color_type) ) {
       mp_do_binary(mp, p,times);
     } else {
       mp_frac_mult(mp, num,denom);
       mp_free_node(mp, p,value_node_size);
     }
    }
  }
  goto DONE;
}

@ @<Protest division...@>=
{ 
  print_err("Division by zero");
@.Division by zero@>
  help1("I'll pretend that you meant to divide by 1."); mp_error(mp);
}

@ @<Scan a binary operation with `\&{of}' between its operands@>=
{ 
  c=(quarterword)mp->cur_mod; mp_get_x_next(mp); mp_scan_expression(mp);
  if ( mp->cur_cmd!=of_token ) {
    mp_missing_err(mp, "of"); mp_print(mp, " for "); 
    mp_print_cmd_mod(mp, primary_binary,c);
@.Missing `of'@>
    help1("I've got the first argument; will look now for the other.");
    mp_back_error(mp);
  }
  p=mp_stash_cur_exp(mp); mp_get_x_next(mp); mp_scan_primary(mp); 
  mp_do_binary(mp, p,c); goto DONE;
}

@ @<Convert a suffix to a string@>=
{ 
  mp_get_x_next(mp); mp_scan_suffix(mp); 
  mp->old_setting=mp->selector; mp->selector=new_string;
  mp_show_token_list(mp, mp->cur_exp,null,100000,0); 
  mp_flush_token_list(mp, mp->cur_exp);
  mp->cur_exp=mp_make_string(mp); mp->selector=mp->old_setting; 
  mp->cur_type=mp_string_type;
  goto DONE;
}

@ If an internal quantity appears all by itself on the left of an
assignment, we return a token list of length one, containing the address
of the internal quantity plus |hash_end|. (This accords with the conventions
of the save stack, as described earlier.)

@<Scan an internal...@>=
{ 
  q=mp->cur_mod;
  if ( my_var_flag==assignment ) {
    mp_get_x_next(mp);
    if ( mp->cur_cmd==assignment ) {
      mp->cur_exp=mp_get_avail(mp);
      mp_info(mp->cur_exp)=q+hash_end; mp->cur_type=mp_token_list; 
      goto DONE;
    }
    mp_back_input(mp);
  }
  mp->cur_exp=mp->internal[q];
  if (mp->int_type[q]==mp_string_type)
    add_str_ref(mp->cur_exp);
  mp->cur_type=(quarterword)mp->int_type[q];
}

@ The most difficult part of |scan_primary| has been saved for last, since
it was necessary to build up some confidence first. We can now face the task
of scanning a variable.

As we scan a variable, we build a token list containing the relevant
names and subscript values, simultaneously following along in the
``collective'' structure to see if we are actually dealing with a macro
instead of a value.

The local variables |pre_head| and |post_head| will point to the beginning
of the prefix and suffix lists; |tail| will point to the end of the list
that is currently growing.

Another local variable, |tt|, contains partial information about the
declared type of the variable-so-far. If |tt>=mp_unsuffixed_macro|, the
relation |tt=mp_type(q)| will always hold. If |tt=undefined|, the routine
doesn't bother to update its information about type. And if
|undefined<tt<mp_unsuffixed_macro|, the precise value of |tt| isn't critical.

@ @<Other local variables for |scan_primary|@>=
pointer pre_head,post_head,tail;
  /* prefix and suffix list variables */
quarterword tt; /* approximation to the type of the variable-so-far */
pointer t; /* a token */
pointer macro_ref = 0; /* reference count for a suffixed macro */

@ @<Scan a variable primary...@>=
{ 
  fast_get_avail(pre_head); tail=pre_head; post_head=null; tt=mp_vacuous;
  while (1) { 
    t=mp_cur_tok(mp); mp_link(tail)=t;
    if ( tt!=undefined ) {
       @<Find the approximate type |tt| and corresponding~|q|@>;
      if ( tt>=mp_unsuffixed_macro ) {
        @<Either begin an unsuffixed macro call or
          prepare for a suffixed one@>;
      }
    }
    mp_get_x_next(mp); tail=t;
    if ( mp->cur_cmd==left_bracket ) {
      @<Scan for a subscript; replace |cur_cmd| by |numeric_token| if found@>;
    }
    if ( mp->cur_cmd>max_suffix_token ) break;
    if ( mp->cur_cmd<min_suffix_token ) break;
  } /* now |cur_cmd| is |internal_quantity|, |tag_token|, or |numeric_token| */
  @<Handle unusual cases that masquerade as variables, and |goto restart|
    or |goto done| if appropriate;
    otherwise make a copy of the variable and |goto done|@>;
}

@ @<Either begin an unsuffixed macro call or...@>=
{ 
  mp_link(tail)=null;
  if ( tt>mp_unsuffixed_macro ) { /* |tt=mp_suffixed_macro| */
    post_head=mp_get_avail(mp); tail=post_head; mp_link(tail)=t;
    tt=undefined; macro_ref=value(q); add_mac_ref(macro_ref);
  } else {
    @<Set up unsuffixed macro call and |goto restart|@>;
  }
}

@ @<Scan for a subscript; replace |cur_cmd| by |numeric_token| if found@>=
{ 
  mp_get_x_next(mp); mp_scan_expression(mp);
  if ( mp->cur_cmd!=right_bracket ) {
    @<Put the left bracket and the expression back to be rescanned@>;
  } else { 
    if ( mp->cur_type!=mp_known ) mp_bad_subscript(mp);
    mp->cur_cmd=numeric_token; mp->cur_mod=mp->cur_exp; mp->cur_sym=0;
  }
}

@ The left bracket that we thought was introducing a subscript might have
actually been the left bracket in a mediation construction like `\.{x[a,b]}'.
So we don't issue an error message at this point; but we do want to back up
so as to avoid any embarrassment about our incorrect assumption.

@<Put the left bracket and the expression back to be rescanned@>=
{ 
  mp_back_input(mp); /* that was the token following the current expression */
  mp_back_expr(mp); mp->cur_cmd=left_bracket; 
  mp->cur_mod=0; mp->cur_sym=frozen_left_bracket;
}

@ Here's a routine that puts the current expression back to be read again.

@c 
static void mp_back_expr (MP mp) {
  pointer p; /* capsule token */
  p=mp_stash_cur_exp(mp); mp_link(p)=null; back_list(p);
}

@ Unknown subscripts lead to the following error message.

@c 
static void mp_bad_subscript (MP mp) { 
  exp_err("Improper subscript has been replaced by zero");
@.Improper subscript...@>
  help3("A bracketed subscript must have a known numeric value;",
    "unfortunately, what I found was the value that appears just",
    "above this error message. So I'll try a zero subscript.");
  mp_flush_error(mp, 0);
}

@ Every time we call |get_x_next|, there's a chance that the variable we've
been looking at will disappear. Thus, we cannot safely keep |q| pointing
into the variable structure; we need to start searching from the root each time.

@<Find the approximate type |tt| and corresponding~|q|@>=
@^inner loop@>
{ 
  p=mp_link(pre_head); q=mp_info(p); tt=undefined;
  if ( eq_type(q) % outer_tag==tag_token ) {
    q=equiv(q);
    if ( q==null ) goto DONE2;
    while (1) { 
      p=mp_link(p);
      if ( p==null ) {
        tt=mp_type(q); goto DONE2;
      };
      if ( mp_type(q)!=mp_structured ) goto DONE2;
      q=mp_link(attr_head(q)); /* the |collective_subscript| attribute */
      if ( p>=mp->hi_mem_min ) { /* it's not a subscript */
        do {  q=mp_link(q); } while (! (attr_loc(q)>=mp_info(p)));
        if ( attr_loc(q)>mp_info(p) ) goto DONE2;
      }
    }
  }
DONE2:
  ;
}

@ How do things stand now? Well, we have scanned an entire variable name,
including possible subscripts and/or attributes; |cur_cmd|, |cur_mod|, and
|cur_sym| represent the token that follows. If |post_head=null|, a
token list for this variable name starts at |mp_link(pre_head)|, with all
subscripts evaluated. But if |post_head<>null|, the variable turned out
to be a suffixed macro; |pre_head| is the head of the prefix list, while
|post_head| is the head of a token list containing both `\.{\AT!}' and
the suffix.

Our immediate problem is to see if this variable still exists. (Variable
structures can change drastically whenever we call |get_x_next|; users
aren't supposed to do this, but the fact that it is possible means that
we must be cautious.)

The following procedure prints an error message when a variable
unexpectedly disappears. Its help message isn't quite right for
our present purposes, but we'll be able to fix that up.

@c 
static void mp_obliterated (MP mp,pointer q) { 
  print_err("Variable "); mp_show_token_list(mp, q,null,1000,0);
  mp_print(mp, " has been obliterated");
@.Variable...obliterated@>
  help5("It seems you did a nasty thing---probably by accident,",
     "but nevertheless you nearly hornswoggled me...",
     "While I was evaluating the right-hand side of this",
     "command, something happened, and the left-hand side",
     "is no longer a variable! So I won't change anything.");
}

@ If the variable does exist, we also need to check
for a few other special cases before deciding that a plain old ordinary
variable has, indeed, been scanned.

@<Handle unusual cases that masquerade as variables...@>=
if ( post_head!=null ) {
  @<Set up suffixed macro call and |goto restart|@>;
}
q=mp_link(pre_head); free_avail(pre_head);
if ( mp->cur_cmd==my_var_flag ) { 
  mp->cur_type=mp_token_list; mp->cur_exp=q; goto DONE;
}
p=mp_find_variable(mp, q);
if ( p!=null ) {
  mp_make_exp_copy(mp, p);
} else { 
  mp_obliterated(mp, q);
  mp->help_line[2]="While I was evaluating the suffix of this variable,";
  mp->help_line[1]="something was redefined, and it's no longer a variable!";
  mp->help_line[0]="In order to get back on my feet, I've inserted `0' instead.";
  mp_put_get_flush_error(mp, 0);
}
mp_flush_node_list(mp, q); 
goto DONE

@ The only complication associated with macro calling is that the prefix
and ``at'' parameters must be packaged in an appropriate list of lists.

@<Set up unsuffixed macro call and |goto restart|@>=
{ 
  p=mp_get_avail(mp); mp_info(pre_head)=mp_link(pre_head); mp_link(pre_head)=p;
  mp_info(p)=t; mp_macro_call(mp, value(q),pre_head,null);
  mp_get_x_next(mp); 
  goto RESTART;
}

@ If the ``variable'' that turned out to be a suffixed macro no longer exists,
we don't care, because we have reserved a pointer (|macro_ref|) to its
token list.

@<Set up suffixed macro call and |goto restart|@>=
{ 
  mp_back_input(mp); p=mp_get_avail(mp); q=mp_link(post_head);
  mp_info(pre_head)=mp_link(pre_head); mp_link(pre_head)=post_head;
  mp_info(post_head)=q; mp_link(post_head)=p; mp_info(p)=mp_link(q); mp_link(q)=null;
  mp_macro_call(mp, macro_ref,pre_head,null); decr(ref_count(macro_ref));
  mp_get_x_next(mp); goto RESTART;
}

@ Our remaining job is simply to make a copy of the value that has been
found. Some cases are harder than others, but complexity arises solely
because of the multiplicity of possible cases.

@<Declare the procedure called |make_exp_copy|@>=
@<Declare subroutines needed by |make_exp_copy|@>
static void mp_make_exp_copy (MP mp,pointer p) {
  pointer q,r,t; /* registers for list manipulation */
RESTART: 
  mp->cur_type=mp_type(p);
  switch (mp->cur_type) {
  case mp_vacuous: case mp_boolean_type: case mp_known:
    mp->cur_exp=value(p); break;
  case unknown_types:
    mp->cur_exp=mp_new_ring_entry(mp, p);
    break;
  case mp_string_type: 
    mp->cur_exp=value(p); add_str_ref(mp->cur_exp);
    break;
  case mp_picture_type:
    mp->cur_exp=value(p);add_edge_ref(mp->cur_exp);
    break;
  case mp_pen_type:
    mp->cur_exp=copy_pen(value(p));
    break; 
  case mp_path_type:
    mp->cur_exp=mp_copy_path(mp, value(p));
    break;
  case mp_transform_type: case mp_color_type: 
  case mp_cmykcolor_type: case mp_pair_type:
    @<Copy the big node |p|@>;
    break;
  case mp_dependent: case mp_proto_dependent:
    mp_encapsulate(mp, mp_copy_dep_list(mp, dep_list(p)));
    break;
  case mp_numeric_type: 
    new_indep(p); goto RESTART;
    break;
  case mp_independent: 
    q=mp_single_dependency(mp, p);
    if ( q==mp->dep_final ){ 
      mp->cur_type=mp_known; mp->cur_exp=0; mp_free_node(mp, q,dep_node_size);
    } else { 
      mp->cur_type=mp_dependent; mp_encapsulate(mp, q);
    }
    break;
  default: 
    mp_confusion(mp, "copy");
@:this can't happen copy}{\quad copy@>
    break;
  }
}

@ The |encapsulate| subroutine assumes that |dep_final| is the
tail of dependency list~|p|.

@<Declare subroutines needed by |make_exp_copy|@>=
static void mp_encapsulate (MP mp,pointer p) { 
  mp->cur_exp=mp_get_node(mp, value_node_size); mp_type(mp->cur_exp)=mp->cur_type;
  mp_name_type(mp->cur_exp)=mp_capsule; mp_new_dep(mp, mp->cur_exp,p);
}

@ The most tedious case arises when the user refers to a
\&{pair}, \&{color}, or \&{transform} variable; we must copy several fields,
each of which can be |independent|, |dependent|, |mp_proto_dependent|,
or |known|.

@<Copy the big node |p|@>=
{ 
  if ( value(p)==null ) 
    mp_init_big_node(mp, p);
  t=mp_get_node(mp, value_node_size); mp_name_type(t)=mp_capsule; mp_type(t)=mp->cur_type;
  mp_init_big_node(mp, t);
  q=value(p)+mp->big_node_size[mp->cur_type]; 
  r=value(t)+mp->big_node_size[mp->cur_type];
  do {  
    q=q-2; r=r-2; mp_install(mp, r,q);
  } while (q!=value(p));
  mp->cur_exp=t;
}

@ The |install| procedure copies a numeric field~|q| into field~|r| of
a big node that will be part of a capsule.

@<Declare subroutines needed by |make_exp_copy|@>=
static void mp_install (MP mp,pointer r, pointer q) {
  pointer p; /* temporary register */
  if ( mp_type(q)==mp_known ){ 
    value(r)=value(q); mp_type(r)=mp_known;
  } else  if ( mp_type(q)==mp_independent ) {
    p=mp_single_dependency(mp, q);
    if ( p==mp->dep_final ) {
      mp_type(r)=mp_known; value(r)=0; mp_free_node(mp, p,dep_node_size);
    } else  { 
      mp_type(r)=mp_dependent; mp_new_dep(mp, r,p);
    }
  } else {
    mp_type(r)=mp_type(q); mp_new_dep(mp, r,mp_copy_dep_list(mp, dep_list(q)));
  }
}

@ Expressions of the form `\.{a[b,c]}' are converted into
`\.{b+a*(c-b)}', without checking the types of \.b~or~\.c,
provided that \.a is numeric.

@<Scan a mediation...@>=
{ 
  p=mp_stash_cur_exp(mp); mp_get_x_next(mp); mp_scan_expression(mp);
  if ( mp->cur_cmd!=comma ) {
    @<Put the left bracket and the expression back...@>;
    mp_unstash_cur_exp(mp, p);
  } else { 
    q=mp_stash_cur_exp(mp); mp_get_x_next(mp); mp_scan_expression(mp);
    if ( mp->cur_cmd!=right_bracket ) {
      mp_missing_err(mp, "]");
@.Missing `]'@>
      help3("I've scanned an expression of the form `a[b,c',",
      "so a right bracket should have come next.",
      "I shall pretend that one was there.");
      mp_back_error(mp);
    }
    r=mp_stash_cur_exp(mp); mp_make_exp_copy(mp, q);
    mp_do_binary(mp, r,minus); mp_do_binary(mp, p,times); 
    mp_do_binary(mp, q,plus); mp_get_x_next(mp);
  }
}

@ Here is a comparatively simple routine that is used to scan the
\&{suffix} parameters of a macro.

@<Declare the basic parsing subroutines@>=
static void mp_scan_suffix (MP mp) {
  pointer h,t; /* head and tail of the list being built */
  pointer p; /* temporary register */
  h=mp_get_avail(mp); t=h;
  while (1) { 
    if ( mp->cur_cmd==left_bracket ) {
      @<Scan a bracketed subscript and set |cur_cmd:=numeric_token|@>;
    }
    if ( mp->cur_cmd==numeric_token ) {
      p=mp_new_num_tok(mp, mp->cur_mod);
    } else if ((mp->cur_cmd==tag_token)||(mp->cur_cmd==internal_quantity) ) {
       p=mp_get_avail(mp); mp_info(p)=mp->cur_sym;
    } else {
      break;
    }
    mp_link(t)=p; t=p; mp_get_x_next(mp);
  }
  mp->cur_exp=mp_link(h); free_avail(h); mp->cur_type=mp_token_list;
}

@ @<Scan a bracketed subscript and set |cur_cmd:=numeric_token|@>=
{ 
  mp_get_x_next(mp); mp_scan_expression(mp);
  if ( mp->cur_type!=mp_known ) mp_bad_subscript(mp);
  if ( mp->cur_cmd!=right_bracket ) {
     mp_missing_err(mp, "]");
@.Missing `]'@>
    help3("I've seen a `[' and a subscript value, in a suffix,",
      "so a right bracket should have come next.",
      "I shall pretend that one was there.");
    mp_back_error(mp);
  }
  mp->cur_cmd=numeric_token; mp->cur_mod=mp->cur_exp;
}

@* \[38] Parsing secondary and higher expressions.

After the intricacies of |scan_primary|\kern-1pt,
the |scan_secondary| routine is
refreshingly simple. It's not trivial, but the operations are relatively
straightforward; the main difficulty is, again, that expressions and data
structures might change drastically every time we call |get_x_next|, so a
cautious approach is mandatory. For example, a macro defined by
\&{primarydef} might have disappeared by the time its second argument has
been scanned; we solve this by increasing the reference count of its token
list, so that the macro can be called even after it has been clobbered.

@<Declare the basic parsing subroutines@>=
static void mp_scan_secondary (MP mp) {
  pointer p; /* for list manipulation */
  halfword c,d; /* operation codes or modifiers */
  pointer mac_name; /* token defined with \&{primarydef} */
RESTART:
  if ((mp->cur_cmd<min_primary_command)||
      (mp->cur_cmd>max_primary_command) )
    mp_bad_exp(mp, "A secondary");
@.A secondary expression...@>
  mp_scan_primary(mp);
CONTINUE: 
  if ( mp->cur_cmd<=max_secondary_command &&
       mp->cur_cmd>=min_secondary_command ) {
    p=mp_stash_cur_exp(mp); 
    c=mp->cur_mod; d=mp->cur_cmd;
    if ( d==secondary_primary_macro ) { 
      mac_name=mp->cur_sym; 
      add_mac_ref(c);
    }
    mp_get_x_next(mp); 
    mp_scan_primary(mp);
    if ( d!=secondary_primary_macro ) {
      mp_do_binary(mp, p,c);
    } else { 
      mp_back_input(mp); 
      mp_binary_mac(mp, p,c,mac_name);
      decr(ref_count(c)); 
      mp_get_x_next(mp); 
      goto RESTART;
    }
    goto CONTINUE;
  }
}

@ The following procedure calls a macro that has two parameters,
|p| and |cur_exp|.

@c 
static void mp_binary_mac (MP mp,pointer p, pointer c, pointer n) {
  pointer q,r; /* nodes in the parameter list */
  q=mp_get_avail(mp); r=mp_get_avail(mp); mp_link(q)=r;
  mp_info(q)=p; mp_info(r)=mp_stash_cur_exp(mp);
  mp_macro_call(mp, c,q,n);
}

@ The next procedure, |scan_tertiary|, is pretty much the same deal.

@<Declare the basic parsing subroutines@>=
static void mp_scan_tertiary (MP mp) {
  pointer p; /* for list manipulation */
  halfword c,d; /* operation codes or modifiers */
  pointer mac_name; /* token defined with \&{secondarydef} */
RESTART:
  if ((mp->cur_cmd<min_primary_command)||
      (mp->cur_cmd>max_primary_command) )
    mp_bad_exp(mp, "A tertiary");
@.A tertiary expression...@>
  mp_scan_secondary(mp);
CONTINUE: 
  if ( mp->cur_cmd<=max_tertiary_command ) {
    if ( mp->cur_cmd>=min_tertiary_command ) {
      p=mp_stash_cur_exp(mp); c=mp->cur_mod; d=mp->cur_cmd;
      if ( d==tertiary_secondary_macro ) { 
        mac_name=mp->cur_sym; add_mac_ref(c);
      };
      mp_get_x_next(mp); mp_scan_secondary(mp);
      if ( d!=tertiary_secondary_macro ) {
        mp_do_binary(mp, p,c);
      } else { 
        mp_back_input(mp); mp_binary_mac(mp, p,c,mac_name);
        decr(ref_count(c)); mp_get_x_next(mp); 
        goto RESTART;
      }
      goto CONTINUE;
    }
  }
}

@ Finally we reach the deepest level in our quartet of parsing routines.
This one is much like the others; but it has an extra complication from
paths, which materialize here.

@d continue_path 25 /* a label inside of |scan_expression| */
@d finish_path 26 /* another */

@<Declare the basic parsing subroutines@>=
static void mp_scan_expression (MP mp) {
  pointer p,q,r,pp,qq; /* for list manipulation */
  halfword c,d; /* operation codes or modifiers */
  int my_var_flag; /* initial value of |var_flag| */
  pointer mac_name; /* token defined with \&{tertiarydef} */
  boolean cycle_hit; /* did a path expression just end with `\&{cycle}'? */
  scaled x,y; /* explicit coordinates or tension at a path join */
  int t; /* knot type following a path join */
  t=0; y=0; x=0;
  my_var_flag=mp->var_flag; mac_name=null;
  mp->expand_depth_count++;
  mp_check_expansion_depth(mp);
RESTART:
  if ((mp->cur_cmd<min_primary_command)||
      (mp->cur_cmd>max_primary_command) )
    mp_bad_exp(mp, "An");
@.An expression...@>
  mp_scan_tertiary(mp);
CONTINUE: 
  if ( mp->cur_cmd<=max_expression_command )
    if ( mp->cur_cmd>=min_expression_command ) {
      if ( (mp->cur_cmd!=equals)||(my_var_flag!=assignment) ) {
        p=mp_stash_cur_exp(mp); c=mp->cur_mod; d=mp->cur_cmd;
        if ( d==expression_tertiary_macro ) {
          mac_name=mp->cur_sym; add_mac_ref(c);
        }
        if ( (d<ampersand)||((d==ampersand)&&
             ((mp_type(p)==mp_pair_type)||(mp_type(p)==mp_path_type))) ) {
          @<Scan a path construction operation;
            but |return| if |p| has the wrong type@>;
        } else { 
          mp_get_x_next(mp); mp_scan_tertiary(mp);
          if ( d!=expression_tertiary_macro ) {
            mp_do_binary(mp, p,c);
          } else  { 
            mp_back_input(mp); mp_binary_mac(mp, p,c,mac_name);
            decr(ref_count(c)); mp_get_x_next(mp); 
            goto RESTART;
          }
        }
        goto CONTINUE;
     }
  }
  mp->expand_depth_count--;
}

@ The reader should review the data structure conventions for paths before
hoping to understand the next part of this code.

@<Scan a path construction operation...@>=
{ 
  cycle_hit=false;
  @<Convert the left operand, |p|, into a partial path ending at~|q|;
    but |return| if |p| doesn't have a suitable type@>;
CONTINUE_PATH: 
  @<Determine the path join parameters;
    but |goto finish_path| if there's only a direction specifier@>;
  if ( mp->cur_cmd==cycle ) {
    @<Get ready to close a cycle@>;
  } else { 
    mp_scan_tertiary(mp);
    @<Convert the right operand, |cur_exp|,
      into a partial path from |pp| to~|qq|@>;
  }
  @<Join the partial paths and reset |p| and |q| to the head and tail
    of the result@>;
  if ( mp->cur_cmd>=min_expression_command )
    if ( mp->cur_cmd<=ampersand ) if ( ! cycle_hit ) goto CONTINUE_PATH;
FINISH_PATH:
  @<Choose control points for the path and put the result into |cur_exp|@>;
}

@ @<Convert the left operand, |p|, into a partial path ending at~|q|...@>=
{ 
  mp_unstash_cur_exp(mp, p);
  if ( mp->cur_type==mp_pair_type ) p=mp_new_knot(mp);
  else if ( mp->cur_type==mp_path_type ) p=mp->cur_exp;
  else return;
  q=p;
  while ( mp_link(q)!=p ) q=mp_link(q);
  if ( mp_left_type(p)!=mp_endpoint ) { /* open up a cycle */
    r=mp_copy_knot(mp, p); mp_link(q)=r; q=r;
  }
  mp_left_type(p)=mp_open; mp_right_type(q)=mp_open;
}

@ A pair of numeric values is changed into a knot node for a one-point path
when \MP\ discovers that the pair is part of a path.

@c 
static pointer mp_new_knot (MP mp) { /* convert a pair to a knot with two endpoints */
  pointer q; /* the new node */
  q=mp_get_node(mp, knot_node_size); mp_left_type(q)=mp_endpoint;
  mp_right_type(q)=mp_endpoint; mp_originator(q)=mp_metapost_user; mp_link(q)=q;
  mp_known_pair(mp); mp_x_coord(q)=mp->cur_x; mp_y_coord(q)=mp->cur_y;
  return q;
}

@ The |known_pair| subroutine sets |cur_x| and |cur_y| to the components
of the current expression, assuming that the current expression is a
pair of known numerics. Unknown components are zeroed, and the
current expression is flushed.

@<Declarations@>=
static void mp_known_pair (MP mp);

@ @c
void mp_known_pair (MP mp) {
  pointer p; /* the pair node */
  if ( mp->cur_type!=mp_pair_type ) {
    exp_err("Undefined coordinates have been replaced by (0,0)");
@.Undefined coordinates...@>
    help5("I need x and y numbers for this part of the path.",
       "The value I found (see above) was no good;",
       "so I'll try to keep going by using zero instead.",
       "(Chapter 27 of The METAFONTbook explains that",
@:METAFONTbook}{\sl The {\logos METAFONT\/}book@>
       "you might want to type `I ??" "?' now.)");
    mp_put_get_flush_error(mp, 0); mp->cur_x=0; mp->cur_y=0;
  } else { 
    p=value(mp->cur_exp);
     @<Make sure that both |x| and |y| parts of |p| are known;
       copy them into |cur_x| and |cur_y|@>;
    mp_flush_cur_exp(mp, 0);
  }
}

@ @<Make sure that both |x| and |y| parts of |p| are known...@>=
if ( mp_type(x_part_loc(p))==mp_known ) {
  mp->cur_x=value(x_part_loc(p));
} else { 
  mp_disp_err(mp, x_part_loc(p),
    "Undefined x coordinate has been replaced by 0");
@.Undefined coordinates...@>
  help5("I need a `known' x value for this part of the path.",
    "The value I found (see above) was no good;",
    "so I'll try to keep going by using zero instead.",
    "(Chapter 27 of The METAFONTbook explains that",
@:METAFONTbook}{\sl The {\logos METAFONT\/}book@>
    "you might want to type `I ??" "?' now.)");
  mp_put_get_error(mp); mp_recycle_value(mp, x_part_loc(p)); mp->cur_x=0;
}
if ( mp_type(y_part_loc(p))==mp_known ) {
  mp->cur_y=value(y_part_loc(p));
} else { 
  mp_disp_err(mp, y_part_loc(p),
    "Undefined y coordinate has been replaced by 0");
  help5("I need a `known' y value for this part of the path.",
    "The value I found (see above) was no good;",
    "so I'll try to keep going by using zero instead.",
    "(Chapter 27 of The METAFONTbook explains that",
    "you might want to type `I ??" "?' now.)");
  mp_put_get_error(mp); mp_recycle_value(mp, y_part_loc(p)); mp->cur_y=0;
}

@ At this point |cur_cmd| is either |ampersand|, |left_brace|, or |path_join|.

@<Determine the path join parameters...@>=
if ( mp->cur_cmd==left_brace ) {
  @<Put the pre-join direction information into node |q|@>;
}
d=mp->cur_cmd;
if ( d==path_join ) {
  @<Determine the tension and/or control points@>;
} else if ( d!=ampersand ) {
  goto FINISH_PATH;
}
mp_get_x_next(mp);
if ( mp->cur_cmd==left_brace ) {
  @<Put the post-join direction information into |x| and |t|@>;
} else if ( mp_right_type(q)!=mp_explicit ) {
  t=mp_open; x=0;
}

@ The |scan_direction| subroutine looks at the directional information
that is enclosed in braces, and also scans ahead to the following character.
A type code is returned, either |open| (if the direction was $(0,0)$),
or |curl| (if the direction was a curl of known value |cur_exp|), or
|given| (if the direction is given by the |angle| value that now
appears in |cur_exp|).

There's nothing difficult about this subroutine, but the program is rather
lengthy because a variety of potential errors need to be nipped in the bud.

@c 
static quarterword mp_scan_direction (MP mp) {
  int t; /* the type of information found */
  scaled x; /* an |x| coordinate */
  mp_get_x_next(mp);
  if ( mp->cur_cmd==curl_command ) {
     @<Scan a curl specification@>;
  } else {
    @<Scan a given direction@>;
  }
  if ( mp->cur_cmd!=right_brace ) {
    mp_missing_err(mp, "}");
@.Missing `\char`\}'@>
    help3("I've scanned a direction spec for part of a path,",
      "so a right brace should have come next.",
      "I shall pretend that one was there.");
    mp_back_error(mp);
  }
  mp_get_x_next(mp); 
  return (quarterword)t;
}

@ @<Scan a curl specification@>=
{ mp_get_x_next(mp); mp_scan_expression(mp);
if ( (mp->cur_type!=mp_known)||(mp->cur_exp<0) ){ 
  exp_err("Improper curl has been replaced by 1");
@.Improper curl@>
  help1("A curl must be a known, nonnegative number.");
  mp_put_get_flush_error(mp, unity);
}
t=mp_curl;
}

@ @<Scan a given direction@>=
{ mp_scan_expression(mp);
  if ( mp->cur_type>mp_pair_type ) {
    @<Get given directions separated by commas@>;
  } else {
    mp_known_pair(mp);
  }
  if ( (mp->cur_x==0)&&(mp->cur_y==0) )  t=mp_open;
  else  { t=mp_given; mp->cur_exp=mp_n_arg(mp, mp->cur_x,mp->cur_y);}
}

@ @<Get given directions separated by commas@>=
{ 
  if ( mp->cur_type!=mp_known ) {
    exp_err("Undefined x coordinate has been replaced by 0");
@.Undefined coordinates...@>
    help5("I need a `known' x value for this part of the path.",
      "The value I found (see above) was no good;",
      "so I'll try to keep going by using zero instead.",
      "(Chapter 27 of The METAFONTbook explains that",
@:METAFONTbook}{\sl The {\logos METAFONT\/}book@>
      "you might want to type `I ??" "?' now.)");
    mp_put_get_flush_error(mp, 0);
  }
  x=mp->cur_exp;
  if ( mp->cur_cmd!=comma ) {
    mp_missing_err(mp, ",");
@.Missing `,'@>
    help2("I've got the x coordinate of a path direction;",
          "will look for the y coordinate next.");
    mp_back_error(mp);
  }
  mp_get_x_next(mp); mp_scan_expression(mp);
  if ( mp->cur_type!=mp_known ) {
     exp_err("Undefined y coordinate has been replaced by 0");
    help5("I need a `known' y value for this part of the path.",
      "The value I found (see above) was no good;",
      "so I'll try to keep going by using zero instead.",
      "(Chapter 27 of The METAFONTbook explains that",
      "you might want to type `I ??" "?' now.)");
    mp_put_get_flush_error(mp, 0);
  }
  mp->cur_y=mp->cur_exp; mp->cur_x=x;
}

@ At this point |mp_right_type(q)| is usually |open|, but it may have been
set to some other value by a previous operation. We must maintain
the value of |mp_right_type(q)| in cases such as
`\.{..\{curl2\}z\{0,0\}..}'.

@<Put the pre-join...@>=
{ 
  t=mp_scan_direction(mp);
  if ( t!=mp_open ) {
    mp_right_type(q)=(quarterword)t; right_given(q)=mp->cur_exp;
    if ( mp_left_type(q)==mp_open ) {
      mp_left_type(q)=(quarterword)t; left_given(q)=mp->cur_exp;
    } /* note that |left_given(q)=left_curl(q)| */
  }
}

@ Since |left_tension| and |mp_left_y| share the same position in knot nodes,
and since |left_given| is similarly equivalent to |mp_left_x|, we use
|x| and |y| to hold the given direction and tension information when
there are no explicit control points.

@<Put the post-join...@>=
{ 
  t=mp_scan_direction(mp);
  if ( mp_right_type(q)!=mp_explicit ) x=mp->cur_exp;
  else t=mp_explicit; /* the direction information is superfluous */
}

@ @<Determine the tension and/or...@>=
{ 
  mp_get_x_next(mp);
  if ( mp->cur_cmd==tension ) {
    @<Set explicit tensions@>;
  } else if ( mp->cur_cmd==controls ) {
    @<Set explicit control points@>;
  } else  { 
    right_tension(q)=unity; y=unity; mp_back_input(mp); /* default tension */
    goto DONE;
  };
  if ( mp->cur_cmd!=path_join ) {
     mp_missing_err(mp, "..");
@.Missing `..'@>
    help1("A path join command should end with two dots.");
    mp_back_error(mp);
  }
DONE:
  ;
}

@ @<Set explicit tensions@>=
{ 
  mp_get_x_next(mp); y=mp->cur_cmd;
  if ( mp->cur_cmd==at_least ) mp_get_x_next(mp);
  mp_scan_primary(mp);
  @<Make sure that the current expression is a valid tension setting@>;
  if ( y==at_least ) negate(mp->cur_exp);
  right_tension(q)=mp->cur_exp;
  if ( mp->cur_cmd==and_command ) {
    mp_get_x_next(mp); y=mp->cur_cmd;
    if ( mp->cur_cmd==at_least ) mp_get_x_next(mp);
    mp_scan_primary(mp);
    @<Make sure that the current expression is a valid tension setting@>;
    if ( y==at_least ) negate(mp->cur_exp);
  }
  y=mp->cur_exp;
}

@ @d min_tension three_quarter_unit

@<Make sure that the current expression is a valid tension setting@>=
if ( (mp->cur_type!=mp_known)||(mp->cur_exp<min_tension) ) {
  exp_err("Improper tension has been set to 1");
@.Improper tension@>
  help1("The expression above should have been a number >=3/4.");
  mp_put_get_flush_error(mp, unity);
}

@ @<Set explicit control points@>=
{ 
  mp_right_type(q)=mp_explicit; t=mp_explicit; mp_get_x_next(mp); mp_scan_primary(mp);
  mp_known_pair(mp); mp_right_x(q)=mp->cur_x; mp_right_y(q)=mp->cur_y;
  if ( mp->cur_cmd!=and_command ) {
    x=mp_right_x(q); y=mp_right_y(q);
  } else { 
    mp_get_x_next(mp); mp_scan_primary(mp);
    mp_known_pair(mp); x=mp->cur_x; y=mp->cur_y;
  }
}

@ @<Convert the right operand, |cur_exp|, into a partial path...@>=
{ 
  if ( mp->cur_type!=mp_path_type ) pp=mp_new_knot(mp);
  else pp=mp->cur_exp;
  qq=pp;
  while ( mp_link(qq)!=pp ) qq=mp_link(qq);
  if ( mp_left_type(pp)!=mp_endpoint ) { /* open up a cycle */
    r=mp_copy_knot(mp, pp); mp_link(qq)=r; qq=r;
  }
  mp_left_type(pp)=mp_open; mp_right_type(qq)=mp_open;
}

@ If a person tries to define an entire path by saying `\.{(x,y)\&cycle}',
we silently change the specification to `\.{(x,y)..cycle}', since a cycle
shouldn't have length zero.

@<Get ready to close a cycle@>=
{ 
  cycle_hit=true; mp_get_x_next(mp); pp=p; qq=p;
  if ( d==ampersand ) if ( p==q ) {
    d=path_join; right_tension(q)=unity; y=unity;
  }
}

@ @<Join the partial paths and reset |p| and |q|...@>=
{ 
if ( d==ampersand ) {
  if ( (mp_x_coord(q)!=mp_x_coord(pp))||(mp_y_coord(q)!=mp_y_coord(pp)) ) {
    print_err("Paths don't touch; `&' will be changed to `..'");
@.Paths don't touch@>
    help3("When you join paths `p&q', the ending point of p",
      "must be exactly equal to the starting point of q.",
      "So I'm going to pretend that you said `p..q' instead.");
    mp_put_get_error(mp); d=path_join; right_tension(q)=unity; y=unity;
  }
}
@<Plug an opening in |mp_right_type(pp)|, if possible@>;
if ( d==ampersand ) {
  @<Splice independent paths together@>;
} else  { 
  @<Plug an opening in |mp_right_type(q)|, if possible@>;
  mp_link(q)=pp; mp_left_y(pp)=y;
  if ( t!=mp_open ) { mp_left_x(pp)=x; mp_left_type(pp)=(quarterword)t;  };
}
q=qq;
}

@ @<Plug an opening in |mp_right_type(q)|...@>=
if ( mp_right_type(q)==mp_open ) {
  if ( (mp_left_type(q)==mp_curl)||(mp_left_type(q)==mp_given) ) {
    mp_right_type(q)=mp_left_type(q); right_given(q)=left_given(q);
  }
}

@ @<Plug an opening in |mp_right_type(pp)|...@>=
if ( mp_right_type(pp)==mp_open ) {
  if ( (t==mp_curl)||(t==mp_given) ) {
    mp_right_type(pp)=(quarterword)t; right_given(pp)=x;
  }
}

@ @<Splice independent paths together@>=
{ 
  if ( mp_left_type(q)==mp_open ) if ( mp_right_type(q)==mp_open ) {
    mp_left_type(q)=mp_curl; left_curl(q)=unity;
  }
  if ( mp_right_type(pp)==mp_open ) if ( t==mp_open ) {
    mp_right_type(pp)=mp_curl; right_curl(pp)=unity;
  }
  mp_right_type(q)=mp_right_type(pp); mp_link(q)=mp_link(pp);
  mp_right_x(q)=mp_right_x(pp); mp_right_y(q)=mp_right_y(pp);
  mp_free_node(mp, pp,knot_node_size);
  if ( qq==pp ) qq=q;
}

@ @<Choose control points for the path...@>=
if ( cycle_hit ) { 
  if ( d==ampersand ) p=q;
} else  { 
  mp_left_type(p)=mp_endpoint;
  if ( mp_right_type(p)==mp_open ) { 
    mp_right_type(p)=mp_curl; right_curl(p)=unity;
  }
  mp_right_type(q)=mp_endpoint;
  if ( mp_left_type(q)==mp_open ) { 
    mp_left_type(q)=mp_curl; left_curl(q)=unity;
  }
  mp_link(q)=p;
}
mp_make_choices(mp, p);
mp->cur_type=mp_path_type; mp->cur_exp=p

@ Finally, we sometimes need to scan an expression whose value is
supposed to be either |true_code| or |false_code|.

@<Declare the basic parsing subroutines@>=
static void mp_get_boolean (MP mp) { 
  mp_get_x_next(mp); mp_scan_expression(mp);
  if ( mp->cur_type!=mp_boolean_type ) {
    exp_err("Undefined condition will be treated as `false'");
@.Undefined condition...@>
    help2("The expression shown above should have had a definite",
          "true-or-false value. I'm changing it to `false'.");
    mp_put_get_flush_error(mp, false_code); mp->cur_type=mp_boolean_type;
  }
}

@* \[39] Doing the operations.
The purpose of parsing is primarily to permit people to avoid piles of
parentheses. But the real work is done after the structure of an expression
has been recognized; that's when new expressions are generated. We
turn now to the guts of \MP, which handles individual operators that
have come through the parsing mechanism.

We'll start with the easy ones that take no operands, then work our way
up to operators with one and ultimately two arguments. In other words,
we will write the three procedures |do_nullary|, |do_unary|, and |do_binary|
that are invoked periodically by the expression scanners.

First let's make sure that all of the primitive operators are in the
hash table. Although |scan_primary| and its relatives made use of the
\\{cmd} code for these operators, the \\{do} routines base everything
on the \\{mod} code. For example, |do_binary| doesn't care whether the
operation it performs is a |primary_binary| or |secondary_binary|, etc.

@<Put each...@>=
mp_primitive(mp, "true",nullary,true_code);
@:true_}{\&{true} primitive@>
mp_primitive(mp, "false",nullary,false_code);
@:false_}{\&{false} primitive@>
mp_primitive(mp, "nullpicture",nullary,null_picture_code);
@:null_picture_}{\&{nullpicture} primitive@>
mp_primitive(mp, "nullpen",nullary,null_pen_code);
@:null_pen_}{\&{nullpen} primitive@>
mp_primitive(mp, "readstring",nullary,read_string_op);
@:read_string_}{\&{readstring} primitive@>
mp_primitive(mp, "pencircle",nullary,pen_circle);
@:pen_circle_}{\&{pencircle} primitive@>
mp_primitive(mp, "normaldeviate",nullary,normal_deviate);
@:normal_deviate_}{\&{normaldeviate} primitive@>
mp_primitive(mp, "readfrom",unary,read_from_op);
@:read_from_}{\&{readfrom} primitive@>
mp_primitive(mp, "closefrom",unary,close_from_op);
@:close_from_}{\&{closefrom} primitive@>
mp_primitive(mp, "odd",unary,odd_op);
@:odd_}{\&{odd} primitive@>
mp_primitive(mp, "known",unary,known_op);
@:known_}{\&{known} primitive@>
mp_primitive(mp, "unknown",unary,unknown_op);
@:unknown_}{\&{unknown} primitive@>
mp_primitive(mp, "not",unary,not_op);
@:not_}{\&{not} primitive@>
mp_primitive(mp, "decimal",unary,decimal);
@:decimal_}{\&{decimal} primitive@>
mp_primitive(mp, "reverse",unary,reverse);
@:reverse_}{\&{reverse} primitive@>
mp_primitive(mp, "makepath",unary,make_path_op);
@:make_path_}{\&{makepath} primitive@>
mp_primitive(mp, "makepen",unary,make_pen_op);
@:make_pen_}{\&{makepen} primitive@>
mp_primitive(mp, "oct",unary,oct_op);
@:oct_}{\&{oct} primitive@>
mp_primitive(mp, "hex",unary,hex_op);
@:hex_}{\&{hex} primitive@>
mp_primitive(mp, "ASCII",unary,ASCII_op);
@:ASCII_}{\&{ASCII} primitive@>
mp_primitive(mp, "char",unary,char_op);
@:char_}{\&{char} primitive@>
mp_primitive(mp, "length",unary,length_op);
@:length_}{\&{length} primitive@>
mp_primitive(mp, "turningnumber",unary,turning_op);
@:turning_number_}{\&{turningnumber} primitive@>
mp_primitive(mp, "xpart",unary,x_part);
@:x_part_}{\&{xpart} primitive@>
mp_primitive(mp, "ypart",unary,y_part);
@:y_part_}{\&{ypart} primitive@>
mp_primitive(mp, "xxpart",unary,xx_part);
@:xx_part_}{\&{xxpart} primitive@>
mp_primitive(mp, "xypart",unary,xy_part);
@:xy_part_}{\&{xypart} primitive@>
mp_primitive(mp, "yxpart",unary,yx_part);
@:yx_part_}{\&{yxpart} primitive@>
mp_primitive(mp, "yypart",unary,yy_part);
@:yy_part_}{\&{yypart} primitive@>
mp_primitive(mp, "redpart",unary,red_part);
@:red_part_}{\&{redpart} primitive@>
mp_primitive(mp, "greenpart",unary,green_part);
@:green_part_}{\&{greenpart} primitive@>
mp_primitive(mp, "bluepart",unary,blue_part);
@:blue_part_}{\&{bluepart} primitive@>
mp_primitive(mp, "cyanpart",unary,cyan_part);
@:cyan_part_}{\&{cyanpart} primitive@>
mp_primitive(mp, "magentapart",unary,magenta_part);
@:magenta_part_}{\&{magentapart} primitive@>
mp_primitive(mp, "yellowpart",unary,yellow_part);
@:yellow_part_}{\&{yellowpart} primitive@>
mp_primitive(mp, "blackpart",unary,black_part);
@:black_part_}{\&{blackpart} primitive@>
mp_primitive(mp, "greypart",unary,grey_part);
@:grey_part_}{\&{greypart} primitive@>
mp_primitive(mp, "colormodel",unary,color_model_part);
@:color_model_part_}{\&{colormodel} primitive@>
mp_primitive(mp, "fontpart",unary,font_part);
@:font_part_}{\&{fontpart} primitive@>
mp_primitive(mp, "textpart",unary,text_part);
@:text_part_}{\&{textpart} primitive@>
mp_primitive(mp, "pathpart",unary,path_part);
@:path_part_}{\&{pathpart} primitive@>
mp_primitive(mp, "penpart",unary,pen_part);
@:pen_part_}{\&{penpart} primitive@>
mp_primitive(mp, "dashpart",unary,dash_part);
@:dash_part_}{\&{dashpart} primitive@>
mp_primitive(mp, "sqrt",unary,sqrt_op);
@:sqrt_}{\&{sqrt} primitive@>
mp_primitive(mp, "mexp",unary,mp_m_exp_op);
@:m_exp_}{\&{mexp} primitive@>
mp_primitive(mp, "mlog",unary,mp_m_log_op);
@:m_log_}{\&{mlog} primitive@>
mp_primitive(mp, "sind",unary,sin_d_op);
@:sin_d_}{\&{sind} primitive@>
mp_primitive(mp, "cosd",unary,cos_d_op);
@:cos_d_}{\&{cosd} primitive@>
mp_primitive(mp, "floor",unary,floor_op);
@:floor_}{\&{floor} primitive@>
mp_primitive(mp, "uniformdeviate",unary,uniform_deviate);
@:uniform_deviate_}{\&{uniformdeviate} primitive@>
mp_primitive(mp, "charexists",unary,char_exists_op);
@:char_exists_}{\&{charexists} primitive@>
mp_primitive(mp, "fontsize",unary,font_size);
@:font_size_}{\&{fontsize} primitive@>
mp_primitive(mp, "llcorner",unary,ll_corner_op);
@:ll_corner_}{\&{llcorner} primitive@>
mp_primitive(mp, "lrcorner",unary,lr_corner_op);
@:lr_corner_}{\&{lrcorner} primitive@>
mp_primitive(mp, "ulcorner",unary,ul_corner_op);
@:ul_corner_}{\&{ulcorner} primitive@>
mp_primitive(mp, "urcorner",unary,ur_corner_op);
@:ur_corner_}{\&{urcorner} primitive@>
mp_primitive(mp, "arclength",unary,arc_length);
@:arc_length_}{\&{arclength} primitive@>
mp_primitive(mp, "angle",unary,angle_op);
@:angle_}{\&{angle} primitive@>
mp_primitive(mp, "cycle",cycle,cycle_op);
@:cycle_}{\&{cycle} primitive@>
mp_primitive(mp, "stroked",unary,stroked_op);
@:stroked_}{\&{stroked} primitive@>
mp_primitive(mp, "filled",unary,filled_op);
@:filled_}{\&{filled} primitive@>
mp_primitive(mp, "textual",unary,textual_op);
@:textual_}{\&{textual} primitive@>
mp_primitive(mp, "clipped",unary,clipped_op);
@:clipped_}{\&{clipped} primitive@>
mp_primitive(mp, "bounded",unary,bounded_op);
@:bounded_}{\&{bounded} primitive@>
mp_primitive(mp, "+",plus_or_minus,plus);
@:+ }{\.{+} primitive@>
mp_primitive(mp, "-",plus_or_minus,minus);
@:- }{\.{-} primitive@>
mp_primitive(mp, "*",secondary_binary,times);
@:* }{\.{*} primitive@>
mp_primitive(mp, "/",slash,over); mp->eqtb[frozen_slash]=mp->eqtb[mp->cur_sym];
@:/ }{\.{/} primitive@>
mp_primitive(mp, "++",tertiary_binary,pythag_add);
@:++_}{\.{++} primitive@>
mp_primitive(mp, "+-+",tertiary_binary,pythag_sub);
@:+-+_}{\.{+-+} primitive@>
mp_primitive(mp, "or",tertiary_binary,or_op);
@:or_}{\&{or} primitive@>
mp_primitive(mp, "and",and_command,and_op);
@:and_}{\&{and} primitive@>
mp_primitive(mp, "<",expression_binary,less_than);
@:< }{\.{<} primitive@>
mp_primitive(mp, "<=",expression_binary,less_or_equal);
@:<=_}{\.{<=} primitive@>
mp_primitive(mp, ">",expression_binary,greater_than);
@:> }{\.{>} primitive@>
mp_primitive(mp, ">=",expression_binary,greater_or_equal);
@:>=_}{\.{>=} primitive@>
mp_primitive(mp, "=",equals,equal_to);
@:= }{\.{=} primitive@>
mp_primitive(mp, "<>",expression_binary,unequal_to);
@:<>_}{\.{<>} primitive@>
mp_primitive(mp, "substring",primary_binary,substring_of);
@:substring_}{\&{substring} primitive@>
mp_primitive(mp, "subpath",primary_binary,subpath_of);
@:subpath_}{\&{subpath} primitive@>
mp_primitive(mp, "directiontime",primary_binary,direction_time_of);
@:direction_time_}{\&{directiontime} primitive@>
mp_primitive(mp, "point",primary_binary,point_of);
@:point_}{\&{point} primitive@>
mp_primitive(mp, "precontrol",primary_binary,precontrol_of);
@:precontrol_}{\&{precontrol} primitive@>
mp_primitive(mp, "postcontrol",primary_binary,postcontrol_of);
@:postcontrol_}{\&{postcontrol} primitive@>
mp_primitive(mp, "penoffset",primary_binary,pen_offset_of);
@:pen_offset_}{\&{penoffset} primitive@>
mp_primitive(mp, "arctime",primary_binary,arc_time_of);
@:arc_time_of_}{\&{arctime} primitive@>
mp_primitive(mp, "mpversion",nullary,mp_version);
@:mp_verison_}{\&{mpversion} primitive@>
mp_primitive(mp, "&",ampersand,concatenate);
@:!!!}{\.{\&} primitive@>
mp_primitive(mp, "rotated",secondary_binary,rotated_by);
@:rotated_}{\&{rotated} primitive@>
mp_primitive(mp, "slanted",secondary_binary,slanted_by);
@:slanted_}{\&{slanted} primitive@>
mp_primitive(mp, "scaled",secondary_binary,scaled_by);
@:scaled_}{\&{scaled} primitive@>
mp_primitive(mp, "shifted",secondary_binary,shifted_by);
@:shifted_}{\&{shifted} primitive@>
mp_primitive(mp, "transformed",secondary_binary,transformed_by);
@:transformed_}{\&{transformed} primitive@>
mp_primitive(mp, "xscaled",secondary_binary,x_scaled);
@:x_scaled_}{\&{xscaled} primitive@>
mp_primitive(mp, "yscaled",secondary_binary,y_scaled);
@:y_scaled_}{\&{yscaled} primitive@>
mp_primitive(mp, "zscaled",secondary_binary,z_scaled);
@:z_scaled_}{\&{zscaled} primitive@>
mp_primitive(mp, "infont",secondary_binary,in_font);
@:in_font_}{\&{infont} primitive@>
mp_primitive(mp, "intersectiontimes",tertiary_binary,intersect);
@:intersection_times_}{\&{intersectiontimes} primitive@>
mp_primitive(mp, "envelope",primary_binary,envelope_of);
@:envelope_}{\&{envelope} primitive@>
mp_primitive(mp, "glyph",primary_binary,glyph_infont);
@:glyph_infont_}{\&{envelope} primitive@>

@ @<Cases of |print_cmd...@>=
case nullary:
case unary:
case primary_binary:
case secondary_binary:
case tertiary_binary:
case expression_binary:
case cycle:
case plus_or_minus:
case slash:
case ampersand:
case equals:
case and_command:
  mp_print_op(mp, (quarterword)m);
  break;

@ OK, let's look at the simplest \\{do} procedure first.

@c @<Declare nullary action procedure@>
static void mp_do_nullary (MP mp,quarterword c) { 
  check_arith;
  if ( mp->internal[mp_tracing_commands]>two )
    mp_show_cmd_mod(mp, nullary,c);
  switch (c) {
  case true_code: case false_code: 
    mp->cur_type=mp_boolean_type; mp->cur_exp=c;
    break;
  case null_picture_code: 
    mp->cur_type=mp_picture_type;
    mp->cur_exp=mp_get_node(mp, edge_header_size); 
    mp_init_edges(mp, mp->cur_exp);
    break;
  case null_pen_code: 
    mp->cur_type=mp_pen_type; mp->cur_exp=mp_get_pen_circle(mp, 0);
    break;
  case normal_deviate: 
    mp->cur_type=mp_known; mp->cur_exp=mp_norm_rand(mp);
    break;
  case pen_circle: 
    mp->cur_type=mp_pen_type; mp->cur_exp=mp_get_pen_circle(mp, unity);
    break;
  case mp_version: 
    mp->cur_type=mp_string_type; 
    mp->cur_exp=intern(metapost_version) ;
    break;
  case read_string_op:
    @<Read a string from the terminal@>;
    break;
  } /* there are no other cases */
  check_arith;
}

@ @<Read a string...@>=
{ 
  if (mp->noninteractive || mp->interaction<=mp_nonstop_mode )
    mp_fatal_error(mp, "*** (cannot readstring in nonstop modes)");
  mp_begin_file_reading(mp); name=is_read;
  limit=start; prompt_input("");
  mp_finish_read(mp);
}

@ @<Declare nullary action procedure@>=
static void mp_finish_read (MP mp) { /* copy |buffer| line to |cur_exp| */
  size_t k;
  str_room((int)mp->last-start);
  for (k=(size_t)start;k<mp->last;k++) {
   append_char(mp->buffer[k]);
  }
  mp_end_file_reading(mp); mp->cur_type=mp_string_type; 
  mp->cur_exp=mp_make_string(mp);
}

@ Things get a bit more interesting when there's an operand. The
operand to |do_unary| appears in |cur_type| and |cur_exp|.

@c @<Declare unary action procedures@>
static void mp_do_unary (MP mp,quarterword c) {
  pointer p,q,r; /* for list manipulation */
  integer x; /* a temporary register */
  check_arith;
  if ( mp->internal[mp_tracing_commands]>two )
    @<Trace the current unary operation@>;
  switch (c) {
  case plus:
    if ( mp->cur_type<mp_color_type ) mp_bad_unary(mp, plus);
    break;
  case minus:
    @<Negate the current expression@>;
    break;
  @<Additional cases of unary operators@>;
  } /* there are no other cases */
  check_arith;
}

@ The |nice_pair| function returns |true| if both components of a pair
are known.

@<Declare unary action procedures@>=
static boolean mp_nice_pair (MP mp,integer p, quarterword t) { 
  if ( t==mp_pair_type ) {
    p=value(p);
    if ( mp_type(x_part_loc(p))==mp_known )
      if ( mp_type(y_part_loc(p))==mp_known )
        return true;
  }
  return false;
}

@ The |nice_color_or_pair| function is analogous except that it also accepts
fully known colors.

@<Declare unary action procedures@>=
static boolean mp_nice_color_or_pair (MP mp,integer p, quarterword t) {
  pointer q,r; /* for scanning the big node */
  if ( (t!=mp_pair_type)&&(t!=mp_color_type)&&(t!=mp_cmykcolor_type) ) {
    return false;
  } else { 
    q=value(p);
    r=q+mp->big_node_size[mp_type(p)];
    do {  
      r=r-2;
      if ( mp_type(r)!=mp_known )
        return false;
    } while (r!=q);
    return true;
  }
}

@ @<Declare unary action...@>=
static void mp_print_known_or_unknown_type (MP mp,quarterword t, integer v) { 
  mp_print_char(mp, xord('('));
  if ( t>mp_known ) mp_print(mp, "unknown numeric");
  else { if ( (t==mp_pair_type)||(t==mp_color_type)||(t==mp_cmykcolor_type) )
    if ( ! mp_nice_color_or_pair(mp, v,t) ) mp_print(mp, "unknown ");
    mp_print_type(mp, t);
  }
  mp_print_char(mp, xord(')'));
}

@ @<Declare unary action...@>=
static void mp_bad_unary (MP mp,quarterword c) { 
  exp_err("Not implemented: "); mp_print_op(mp, c);
@.Not implemented...@>
  mp_print_known_or_unknown_type(mp, mp->cur_type,mp->cur_exp);
  help3("I'm afraid I don't know how to apply that operation to that",
    "particular type. Continue, and I'll simply return the",
    "argument (shown above) as the result of the operation.");
  mp_put_get_error(mp);
}

@ @<Trace the current unary operation@>=
{ 
  mp_begin_diagnostic(mp); mp_print_nl(mp, "{"); 
  mp_print_op(mp, c); mp_print_char(mp, xord('('));
  mp_print_exp(mp, null,0); /* show the operand, but not verbosely */
  mp_print(mp, ")}"); mp_end_diagnostic(mp, false);
}

@ Negation is easy except when the current expression
is of type |independent|, or when it is a pair with one or more
|independent| components.

It is tempting to argue that the negative of an independent variable
is an independent variable, hence we don't have to do anything when
negating it. The fallacy is that other dependent variables pointing
to the current expression must change the sign of their
coefficients if we make no change to the current expression.

Instead, we work around the problem by copying the current expression
and recycling it afterwards (cf.~the |stash_in| routine).

@<Negate the current expression@>=
switch (mp->cur_type) {
case mp_color_type:
case mp_cmykcolor_type:
case mp_pair_type:
case mp_independent: 
  q=mp->cur_exp; mp_make_exp_copy(mp, q);
  if ( mp->cur_type==mp_dependent ) {
    mp_negate_dep_list(mp, dep_list(mp->cur_exp));
  } else if ( mp->cur_type<=mp_pair_type ) { /* |mp_color_type| or |mp_pair_type| */
    p=value(mp->cur_exp);
    r=p+mp->big_node_size[mp->cur_type];
    do {  
      r=r-2;
      if ( mp_type(r)==mp_known ) negate(value(r));
      else mp_negate_dep_list(mp, dep_list(r));
    } while (r!=p);
  } /* if |cur_type=mp_known| then |cur_exp=0| */
  mp_recycle_value(mp, q); mp_free_node(mp, q,value_node_size);
  break;
case mp_dependent:
case mp_proto_dependent:
  mp_negate_dep_list(mp, dep_list(mp->cur_exp));
  break;
case mp_known:
  negate(mp->cur_exp);
  break;
default:
  mp_bad_unary(mp, minus);
  break;
}

@ @<Declare unary action...@>=
static void mp_negate_dep_list (MP mp,pointer p) { 
  while (1) { 
    negate(value(p));
    if ( mp_info(p)==null ) return;
    p=mp_link(p);
  }
}

@ @<Additional cases of unary operators@>=
case not_op: 
  if ( mp->cur_type!=mp_boolean_type ) mp_bad_unary(mp, not_op);
  else mp->cur_exp=true_code+false_code-mp->cur_exp;
  break;

@ @d three_sixty_units 23592960 /* that's |360*unity| */
@d boolean_reset(A) if ( (A) ) mp->cur_exp=true_code; else mp->cur_exp=false_code

@<Additional cases of unary operators@>=
case sqrt_op:
case mp_m_exp_op:
case mp_m_log_op:
case sin_d_op:
case cos_d_op:
case floor_op:
case  uniform_deviate:
case odd_op:
case char_exists_op:
  if ( mp->cur_type!=mp_known ) {
    mp_bad_unary(mp, c);
  } else {
    switch (c) {
    case sqrt_op:mp->cur_exp=mp_square_rt(mp, mp->cur_exp);break;
    case mp_m_exp_op:mp->cur_exp=mp_m_exp(mp, mp->cur_exp);break;
    case mp_m_log_op:mp->cur_exp=mp_m_log(mp, mp->cur_exp);break;
    case sin_d_op:
    case cos_d_op:
      mp_n_sin_cos(mp, (mp->cur_exp % three_sixty_units)*16);
      if ( c==sin_d_op ) mp->cur_exp=mp_round_fraction(mp, mp->n_sin);
      else mp->cur_exp=mp_round_fraction(mp, mp->n_cos);
      break;
    case floor_op:mp->cur_exp=mp_floor_scaled(mp, mp->cur_exp);break;
    case uniform_deviate:mp->cur_exp=mp_unif_rand(mp, mp->cur_exp);break;
    case odd_op: 
      boolean_reset(odd(mp_round_unscaled(mp, mp->cur_exp)));
      mp->cur_type=mp_boolean_type;
      break;
    case char_exists_op:
      @<Determine if a character has been shipped out@>;
      break;
    } /* there are no other cases */
  }
  break;

@ @<Additional cases of unary operators@>=
case angle_op:
  if ( mp_nice_pair(mp, mp->cur_exp,mp->cur_type) ) {
    p=value(mp->cur_exp);
    x=mp_n_arg(mp, value(x_part_loc(p)),value(y_part_loc(p)));
    if ( x>=0 ) mp_flush_cur_exp(mp, (x+8)/ 16);
    else mp_flush_cur_exp(mp, -((-x+8)/ 16));
  } else {
    mp_bad_unary(mp, angle_op);
  }
  break;

@ If the current expression is a pair, but the context wants it to
be a path, we call |pair_to_path|.

@<Declare unary action...@>=
static void mp_pair_to_path (MP mp) { 
  mp->cur_exp=mp_new_knot(mp); 
  mp->cur_type=mp_path_type;
}

@ This complicated if test makes sure that any |bounds| or |clip|
picture objects that get passed into \&{within} do not raise an 
error when queried using the color part primitives (this is needed
for backward compatibility) .

@d cur_pic_item mp_link(dummy_loc(mp->cur_exp))
@d pict_color_type(A) ((cur_pic_item!=null) &&
         ((!has_color(cur_pic_item)) 
          ||
         (((mp_color_model(cur_pic_item)==A)
          ||
          ((mp_color_model(cur_pic_item)==mp_uninitialized_model) &&
           (mp->internal[mp_default_color_model]/unity)==(A))))))

@<Additional cases of unary operators@>=
case x_part:
case y_part:
  if ( (mp->cur_type==mp_pair_type)||(mp->cur_type==mp_transform_type) )
    mp_take_part(mp, c);
  else if ( mp->cur_type==mp_picture_type ) mp_take_pict_part(mp, c);
  else mp_bad_unary(mp, c);
  break;
case xx_part:
case xy_part:
case yx_part:
case yy_part: 
  if ( mp->cur_type==mp_transform_type ) mp_take_part(mp, c);
  else if ( mp->cur_type==mp_picture_type ) mp_take_pict_part(mp, c);
  else mp_bad_unary(mp, c);
  break;
case red_part:
case green_part:
case blue_part: 
  if ( mp->cur_type==mp_color_type ) mp_take_part(mp, c);
  else if ( mp->cur_type==mp_picture_type ) {
    if pict_color_type(mp_rgb_model) mp_take_pict_part(mp, c);
    else mp_bad_color_part(mp, c);
  }
  else mp_bad_unary(mp, c);
  break;
case cyan_part:
case magenta_part:
case yellow_part:
case black_part: 
  if ( mp->cur_type==mp_cmykcolor_type) mp_take_part(mp, c); 
  else if ( mp->cur_type==mp_picture_type ) {
    if pict_color_type(mp_cmyk_model) mp_take_pict_part(mp, c);
    else mp_bad_color_part(mp, c);
  }
  else mp_bad_unary(mp, c);
  break;
case grey_part: 
  if ( mp->cur_type==mp_known ) ; /* mp->cur_exp=mp->cur_exp */
  else if ( mp->cur_type==mp_picture_type ) {
    if pict_color_type(mp_grey_model) mp_take_pict_part(mp, c);
    else mp_bad_color_part(mp, c);
  }
  else mp_bad_unary(mp, c);
  break;
case color_model_part: 
  if ( mp->cur_type==mp_picture_type ) mp_take_pict_part(mp, c);
  else mp_bad_unary(mp, c);
  break;

@ @<Declarations@>=
static void mp_bad_color_part(MP mp, quarterword c);

@ @c
static void mp_bad_color_part(MP mp, quarterword c) {
  pointer p; /* the big node */
  p=mp_link(dummy_loc(mp->cur_exp));
  exp_err("Wrong picture color model: "); mp_print_op(mp, c);
@.Wrong picture color model...@>
  if (mp_color_model(p)==mp_grey_model)
    mp_print(mp, " of grey object");
  else if (mp_color_model(p)==mp_cmyk_model)
    mp_print(mp, " of cmyk object");
  else if (mp_color_model(p)==mp_rgb_model)
    mp_print(mp, " of rgb object");
  else if (mp_color_model(p)==mp_no_model) 
    mp_print(mp, " of marking object");
  else 
    mp_print(mp," of defaulted object");
  help3("You can only ask for the redpart, greenpart, bluepart of a rgb object,",
    "the cyanpart, magentapart, yellowpart or blackpart of a cmyk object, ",
    "or the greypart of a grey object. No mixing and matching, please.");
  mp_error(mp);
  if (c==black_part)
    mp_flush_cur_exp(mp,unity);
  else
    mp_flush_cur_exp(mp,0);
}

@ In the following procedure, |cur_exp| points to a capsule, which points to
a big node. We want to delete all but one part of the big node.

@<Declare unary action...@>=
static void mp_take_part (MP mp,quarterword c) {
  pointer p; /* the big node */
  p=value(mp->cur_exp); value(temp_val)=p; mp_type(temp_val)=mp->cur_type;
  mp_link(p)=temp_val; mp_free_node(mp, mp->cur_exp,value_node_size);
  mp_make_exp_copy(mp, p+mp->sector_offset[c+mp_x_part_sector-x_part]);
  mp_recycle_value(mp, temp_val);
}

@ @<Initialize table entries...@>=
mp_name_type(temp_val)=mp_capsule;

@ @<Additional cases of unary operators@>=
case font_part:
case text_part:
case path_part:
case pen_part:
case dash_part:
  if ( mp->cur_type==mp_picture_type ) mp_take_pict_part(mp, c);
  else mp_bad_unary(mp, c);
  break;

@ @<Declarations@>=
static void mp_scale_edges (MP mp);

@ @<Declare unary action...@>=
static void mp_take_pict_part (MP mp,quarterword c) {
  pointer p; /* first graphical object in |cur_exp| */
  p=mp_link(dummy_loc(mp->cur_exp));
  if ( p!=null ) {
    switch (c) {
    case x_part: case y_part: case xx_part:
    case xy_part: case yx_part: case yy_part:
      if ( mp_type(p)==mp_text_code ) mp_flush_cur_exp(mp, text_trans_part(p+c));
      else goto NOT_FOUND;
      break;
    case red_part: case green_part: case blue_part:
      if ( has_color(p) ) mp_flush_cur_exp(mp, obj_color_part(p+c));
      else goto NOT_FOUND;
      break;
    case cyan_part: case magenta_part: case yellow_part:
    case black_part:
      if ( has_color(p) ) {
        if ( mp_color_model(p)==mp_uninitialized_model && c==black_part)
          mp_flush_cur_exp(mp, unity);
        else
          mp_flush_cur_exp(mp, obj_color_part(p+c+(red_part-cyan_part)));
      } else goto NOT_FOUND;
      break;
    case grey_part:
      if ( has_color(p) )
          mp_flush_cur_exp(mp, obj_color_part(p+c+(red_part-grey_part)));
      else goto NOT_FOUND;
      break;
    case color_model_part:
      if ( has_color(p) ) {
        if ( mp_color_model(p)==mp_uninitialized_model )
          mp_flush_cur_exp(mp, mp->internal[mp_default_color_model]);
        else
          mp_flush_cur_exp(mp, mp_color_model(p)*unity);
      } else goto NOT_FOUND;
      break;
    @<Handle other cases in |take_pict_part| or |goto not_found|@>;
    } /* all cases have been enumerated */
    return;
  };
NOT_FOUND:
  @<Convert the current expression to a null value appropriate
    for |c|@>;
}

@ @<Handle other cases in |take_pict_part| or |goto not_found|@>=
case text_part: 
  if ( mp_type(p)!=mp_text_code ) goto NOT_FOUND;
  else { 
    mp_flush_cur_exp(mp, mp_text_p(p));
    add_str_ref(mp->cur_exp);
    mp->cur_type=mp_string_type;
    };
  break;
case font_part: 
  if ( mp_type(p)!=mp_text_code ) goto NOT_FOUND;
  else { 
    mp_flush_cur_exp(mp, rts(mp->font_name[mp_font_n(p)])); 
    add_str_ref(mp->cur_exp);
    mp->cur_type=mp_string_type;
  };
  break;
case path_part:
  if ( mp_type(p)==mp_text_code ) goto NOT_FOUND;
  else if ( is_stop(p) ) mp_confusion(mp, "pict");
@:this can't happen pict}{\quad pict@>
  else { 
    mp_flush_cur_exp(mp, mp_copy_path(mp, mp_path_p(p)));
    mp->cur_type=mp_path_type;
  }
  break;
case pen_part: 
  if ( ! has_pen(p) ) goto NOT_FOUND;
  else {
    if ( mp_pen_p(p)==null ) goto NOT_FOUND;
    else { mp_flush_cur_exp(mp, copy_pen(mp_pen_p(p)));
      mp->cur_type=mp_pen_type;
    };
  }
  break;
case dash_part: 
  if ( mp_type(p)!=mp_stroked_code ) goto NOT_FOUND;
  else { if ( mp_dash_p(p)==null ) goto NOT_FOUND;
    else { add_edge_ref(mp_dash_p(p));
    mp->se_sf=dash_scale(p);
    mp->se_pic=mp_dash_p(p);
    mp_scale_edges(mp);
    mp_flush_cur_exp(mp, mp->se_pic);
    mp->cur_type=mp_picture_type;
    };
  }
  break;

@ Since |scale_edges| had to be declared |forward|, it had to be declared as a
parameterless procedure even though it really takes two arguments and updates
one of them.  Hence the following globals are needed.

@<Global...@>=
pointer se_pic;  /* edge header used and updated by |scale_edges| */
scaled se_sf;  /* the scale factor argument to |scale_edges| */

@ @<Convert the current expression to a null value appropriate...@>=
switch (c) {
case text_part: case font_part: 
  mp_flush_cur_exp(mp, null_str);
  mp->cur_type=mp_string_type;
  break;
case path_part: 
  mp_flush_cur_exp(mp, mp_get_node(mp, knot_node_size));
  mp_left_type(mp->cur_exp)=mp_endpoint;
  mp_right_type(mp->cur_exp)=mp_endpoint;
  mp_link(mp->cur_exp)=mp->cur_exp;
  mp_x_coord(mp->cur_exp)=0;
  mp_y_coord(mp->cur_exp)=0;
  mp_originator(mp->cur_exp)=mp_metapost_user;
  mp->cur_type=mp_path_type;
  break;
case pen_part: 
  mp_flush_cur_exp(mp, mp_get_pen_circle(mp, 0));
  mp->cur_type=mp_pen_type;
  break;
case dash_part: 
  mp_flush_cur_exp(mp, mp_get_node(mp, edge_header_size));
  mp_init_edges(mp, mp->cur_exp);
  mp->cur_type=mp_picture_type;
  break;
default: 
   mp_flush_cur_exp(mp, 0);
  break;
}

@ @<Additional cases of unary...@>=
case char_op: 
  if ( mp->cur_type!=mp_known ) { 
    mp_bad_unary(mp, char_op);
  } else { 
    mp->cur_exp=mp_round_unscaled(mp, mp->cur_exp) % 256; 
    mp->cur_type=mp_string_type;
    if ( mp->cur_exp<0 ) mp->cur_exp=mp->cur_exp+256;
  }
  break;
case decimal: 
  if ( mp->cur_type!=mp_known ) {
     mp_bad_unary(mp, decimal);
  } else { 
    mp->old_setting=mp->selector; mp->selector=new_string;
    mp_print_scaled(mp, mp->cur_exp); mp->cur_exp=mp_make_string(mp);
    mp->selector=mp->old_setting; mp->cur_type=mp_string_type;
  }
  break;
case oct_op:
case hex_op:
case ASCII_op: 
  if ( mp->cur_type!=mp_string_type ) mp_bad_unary(mp, c);
  else mp_str_to_num(mp, c);
  break;
case font_size: 
  if ( mp->cur_type!=mp_string_type ) mp_bad_unary(mp, font_size);
  else @<Find the design size of the font whose name is |cur_exp|@>;
  break;

@ @<Declare unary action...@>=
static void mp_str_to_num (MP mp,quarterword c) { /* converts a string to a number */
  integer n; /* accumulator */
  ASCII_code m; /* current character */
  pool_pointer k; /* index into |str_pool| */
  int b; /* radix of conversion */
  boolean bad_char; /* did the string contain an invalid digit? */
  if ( c==ASCII_op ) {
    if ( length(mp->cur_exp)==0 ) n=-1;
    else n=mp->str_pool[mp->str_start[mp->cur_exp]];
  } else { 
    if ( c==oct_op ) b=8; else b=16;
    n=0; bad_char=false;
    for (k=mp->str_start[mp->cur_exp];k<str_stop(mp->cur_exp);k++) {
      m=mp->str_pool[k];
      if ( (m>='0')&&(m<='9') ) m=(ASCII_code)(m-'0');
      else if ( (m>='A')&&(m<='F') ) m=(ASCII_code)(m-'A'+10);
      else if ( (m>='a')&&(m<='f') ) m=(ASCII_code)(m-'a'+10);
      else  { bad_char=true; m=0; };
      if ( (int)m>=b ) { bad_char=true; m=0; };
      if ( n<32768 / b ) n=n*b+m; else n=32767;
    }
    @<Give error messages if |bad_char| or |n>=4096|@>;
  }
  mp_flush_cur_exp(mp, n*unity);
}

@ @<Give error messages if |bad_char|...@>=
if ( bad_char ) { 
  exp_err("String contains illegal digits");
@.String contains illegal digits@>
  if ( c==oct_op ) {
    help1("I zeroed out characters that weren't in the range 0..7.");
  } else  {
    help1("I zeroed out characters that weren't hex digits.");
  }
  mp_put_get_error(mp);
}
if ( (n>4095) ) {
  if ( mp->internal[mp_warning_check]>0 ) {
    print_err("Number too large ("); 
    mp_print_int(mp, n); mp_print_char(mp, xord(')'));
@.Number too large@>
    help2("I have trouble with numbers greater than 4095; watch out.",
           "(Set warningcheck:=0 to suppress this message.)");
    mp_put_get_error(mp);
  }
}

@ The length operation is somewhat unusual in that it applies to a variety
of different types of operands.

@<Additional cases of unary...@>=
case length_op: 
  switch (mp->cur_type) {
  case mp_string_type: mp_flush_cur_exp(mp, length(mp->cur_exp)*unity); break;
  case mp_path_type: mp_flush_cur_exp(mp, mp_path_length(mp)); break;
  case mp_known: mp->cur_exp=abs(mp->cur_exp); break;
  case mp_picture_type: mp_flush_cur_exp(mp, mp_pict_length(mp)); break;
  default: 
    if ( mp_nice_pair(mp, mp->cur_exp,mp->cur_type) )
      mp_flush_cur_exp(mp, mp_pyth_add(mp, 
        value(x_part_loc(value(mp->cur_exp))),
        value(y_part_loc(value(mp->cur_exp)))));
    else mp_bad_unary(mp, c);
    break;
  }
  break;

@ @<Declare unary action...@>=
static scaled mp_path_length (MP mp) { /* computes the length of the current path */
  scaled n; /* the path length so far */
  pointer p; /* traverser */
  p=mp->cur_exp;
  if ( mp_left_type(p)==mp_endpoint ) n=-unity; else n=0;
  do {  p=mp_link(p); n=n+unity; } while (p!=mp->cur_exp);
  return n;
}

@ @<Declare unary action...@>=
static scaled mp_pict_length (MP mp) { 
  /* counts interior components in picture |cur_exp| */
  scaled n; /* the count so far */
  pointer p; /* traverser */
  n=0;
  p=mp_link(dummy_loc(mp->cur_exp));
  if ( p!=null ) {
    if ( is_start_or_stop(p) )
      if ( mp_skip_1component(mp, p)==null ) p=mp_link(p);
    while ( p!=null )  { 
      skip_component(p) return n; 
      n=n+unity;   
    }
  }
  return n;
}

@ Implement |turningnumber|

@<Additional cases of unary...@>=
case turning_op:
  if ( mp->cur_type==mp_pair_type ) mp_flush_cur_exp(mp, 0);
  else if ( mp->cur_type!=mp_path_type ) mp_bad_unary(mp, turning_op);
  else if ( mp_left_type(mp->cur_exp)==mp_endpoint )
     mp_flush_cur_exp(mp, 0); /* not a cyclic path */
  else
    mp_flush_cur_exp(mp, mp_turn_cycles_wrapper(mp, mp->cur_exp));
  break;

@ The function |an_angle| returns the value of the |angle| primitive, or $0$ if the
argument is |origin|.

@<Declare unary action...@>=
static angle mp_an_angle (MP mp,scaled xpar, scaled ypar) {
  if ( (! ((xpar==0) && (ypar==0))) )
    return mp_n_arg(mp, xpar,ypar);
  return 0;
}


@ The actual turning number is (for the moment) computed in a C function
that receives eight integers corresponding to the four controlling points,
and returns a single angle.  Besides those, we have to account for discrete
moves at the actual points.

@d mp_floor(a) ((a)>=0 ? (int)(a) : -(int)(-(a)))
@d bezier_error (720*(256*256*16))+1
@d mp_sign(v) ((v)>0 ? 1 : ((v)<0 ? -1 : 0 ))
@d mp_out(A) (double)((A)/(256*256*16))
@d divisor (256.0*256.0)
@d double2angle(a) (int)mp_floor(a*256.0*256.0*16.0)

@<Declare unary action...@>=
static angle mp_bezier_slope(MP mp, integer AX,integer AY,integer BX,integer BY,
            integer CX,integer CY,integer DX,integer DY);

@ @c 
static angle mp_bezier_slope(MP mp, integer AX,integer AY,integer BX,integer BY,
            integer CX,integer CY,integer DX,integer DY) {
  double a, b, c;
  integer deltax,deltay;
  double ax,ay,bx,by,cx,cy,dx,dy;
  angle xi = 0, xo = 0, xm = 0;
  double res = 0;
  ax=(double)(AX/divisor);  ay=(double)(AY/divisor);
  bx=(double)(BX/divisor);  by=(double)(BY/divisor);
  cx=(double)(CX/divisor);  cy=(double)(CY/divisor);
  dx=(double)(DX/divisor);  dy=(double)(DY/divisor);

  deltax = (BX-AX); deltay = (BY-AY);
  if (deltax==0 && deltay == 0) { deltax=(CX-AX); deltay=(CY-AY); }
  if (deltax==0 && deltay == 0) { deltax=(DX-AX); deltay=(DY-AY); }
  xi = mp_an_angle(mp,deltax,deltay);

  deltax = (CX-BX); deltay = (CY-BY);
  xm = mp_an_angle(mp,deltax,deltay);

  deltax = (DX-CX); deltay = (DY-CY);
  if (deltax==0 && deltay == 0) { deltax=(DX-BX); deltay=(DY-BY); }
  if (deltax==0 && deltay == 0) { deltax=(DX-AX); deltay=(DY-AY); }
  xo = mp_an_angle(mp,deltax,deltay);

  a = (bx-ax)*(cy-by) - (cx-bx)*(by-ay); /* a = (bp-ap)x(cp-bp); */
  b = (bx-ax)*(dy-cy) - (by-ay)*(dx-cx);; /* b = (bp-ap)x(dp-cp);*/
  c = (cx-bx)*(dy-cy) - (dx-cx)*(cy-by); /* c = (cp-bp)x(dp-cp);*/

  if ((a==0)&&(c==0)) {
    res = (b==0 ?  0 :  (mp_out(xo)-mp_out(xi))); 
  } else if ((a==0)||(c==0)) {
    if ((mp_sign(b) == mp_sign(a)) || (mp_sign(b) == mp_sign(c))) {
      res = mp_out(xo)-mp_out(xi); /* ? */
      if (res<-180.0) 
	res += 360.0;
      else if (res>180.0)
	res -= 360.0;
    } else {
      res = mp_out(xo)-mp_out(xi); /* ? */
    }
  } else if ((mp_sign(a)*mp_sign(c))<0) {
    res = mp_out(xo)-mp_out(xi); /* ? */
      if (res<-180.0) 
	res += 360.0;
      else if (res>180.0)
	res -= 360.0;
  } else {
    if (mp_sign(a) == mp_sign(b)) {
      res = mp_out(xo)-mp_out(xi); /* ? */
      if (res<-180.0) 
	res += 360.0;
      else if (res>180.0)
	res -= 360.0;
    } else {
      if ((b*b) == (4*a*c)) {
	res = (double)bezier_error;
      } else if ((b*b) < (4*a*c)) {
	res = mp_out(xo)-mp_out(xi); /* ? */
	if (res<=0.0 &&res>-180.0) 
	  res += 360.0;
        else if (res>=0.0 && res<180.0)
	  res -= 360.0;
      } else {
	res = mp_out(xo)-mp_out(xi);
	if (res<-180.0) 
	  res += 360.0;
        else if (res>180.0)
	  res -= 360.0;
      }
    }
  }
  return double2angle(res);
}

@
@d p_nextnext mp_link(mp_link(p))
@d p_next mp_link(p)
@d seven_twenty_deg 05500000000 /* $720\cdot2^{20}$, represents $720^\circ$ */

@<Declare unary action...@>=
static scaled mp_new_turn_cycles (MP mp,pointer c) {
  angle res,ang; /*  the angles of intermediate results  */
  scaled turns;  /*  the turn counter  */
  pointer p;     /*  for running around the path  */
  integer xp,yp;   /*  coordinates of next point  */
  integer x,y;   /*  helper coordinates  */
  angle in_angle,out_angle;     /*  helper angles */
  unsigned old_setting; /* saved |selector| setting */
  res=0;
  turns= 0;
  p=c;
  old_setting = mp->selector; mp->selector=term_only;
  if ( mp->internal[mp_tracing_commands]>unity ) {
    mp_begin_diagnostic(mp);
    mp_print_nl(mp, "");
    mp_end_diagnostic(mp, false);
  }
  do { 
    xp = mp_x_coord(p_next); yp = mp_y_coord(p_next);
    ang  = mp_bezier_slope(mp,mp_x_coord(p), mp_y_coord(p), mp_right_x(p), mp_right_y(p),
             mp_left_x(p_next), mp_left_y(p_next), xp, yp);
    if ( ang>seven_twenty_deg ) {
      print_err("Strange path");
      mp_error(mp);
      mp->selector=old_setting;
      return 0;
    }
    res  = res + ang;
    if ( res > one_eighty_deg ) {
      res = res - three_sixty_deg;
      turns = turns + unity;
    }
    if ( res <= -one_eighty_deg ) {
      res = res + three_sixty_deg;
      turns = turns - unity;
    }
    /*  incoming angle at next point  */
    x = mp_left_x(p_next);  y = mp_left_y(p_next);
    if ( (xp==x)&&(yp==y) ) { x = mp_right_x(p);  y = mp_right_y(p);  };
    if ( (xp==x)&&(yp==y) ) { x = mp_x_coord(p);  y = mp_y_coord(p);  };
    in_angle = mp_an_angle(mp, xp - x, yp - y);
    /*  outgoing angle at next point  */
    x = mp_right_x(p_next);  y = mp_right_y(p_next);
    if ( (xp==x)&&(yp==y) ) { x = mp_left_x(p_nextnext);  y = mp_left_y(p_nextnext);  };
    if ( (xp==x)&&(yp==y) ) { x = mp_x_coord(p_nextnext); y = mp_y_coord(p_nextnext); };
    out_angle = mp_an_angle(mp, x - xp, y- yp);
    ang  = (out_angle - in_angle);
    reduce_angle(ang);
    if ( ang!=0 ) {
      res  = res + ang;
      if ( res >= one_eighty_deg ) {
        res = res - three_sixty_deg;
        turns = turns + unity;
      };
      if ( res <= -one_eighty_deg ) {
        res = res + three_sixty_deg;
        turns = turns - unity;
      };
    };
    p = mp_link(p);
  } while (p!=c);
  mp->selector=old_setting;
  return turns;
}


@ This code is based on Bogus\l{}av Jackowski's
|emergency_turningnumber| macro, with some minor changes by Taco
Hoekwater. The macro code looked more like this:
{\obeylines
vardef turning\_number primary p =
~~save res, ang, turns;
~~res := 0;
~~if length p <= 2:
~~~~if Angle ((point 0 of p) - (postcontrol 0 of p)) >= 0:  1  else: -1 fi
~~else:
~~~~for t = 0 upto length p-1 :
~~~~~~angc := Angle ((point t+1 of p)  - (point t of p))
~~~~~~~~- Angle ((point t of p) - (point t-1 of p));
~~~~~~if angc > 180: angc := angc - 360; fi;
~~~~~~if angc < -180: angc := angc + 360; fi;
~~~~~~res  := res + angc;
~~~~endfor;
~~res/360
~~fi
enddef;}
The general idea is to calculate only the sum of the angles of
straight lines between the points, of a path, not worrying about cusps
or self-intersections in the segments at all. If the segment is not
well-behaved, the result is not necesarily correct. But the old code
was not always correct either, and worse, it sometimes failed for
well-behaved paths as well. All known bugs that were triggered by the
original code no longer occur with this code, and it runs roughly 3
times as fast because the algorithm is much simpler.

@ It is possible to overflow the return value of the |turn_cycles|
function when the path is sufficiently long and winding, but I am not
going to bother testing for that. In any case, it would only return
the looped result value, which is not a big problem.

The macro code for the repeat loop was a bit nicer to look
at than the pascal code, because it could use |point -1 of p|. In
pascal, the fastest way to loop around the path is not to look
backward once, but forward twice. These defines help hide the trick.

@d p_to mp_link(mp_link(p))
@d p_here mp_link(p)
@d p_from p

@<Declare unary action...@>=
static scaled mp_turn_cycles (MP mp,pointer c) {
  angle res,ang; /*  the angles of intermediate results  */
  scaled turns;  /*  the turn counter  */
  pointer p;     /*  for running around the path  */
  res=0;  turns= 0; p=c;
  do { 
    ang  = mp_an_angle (mp, mp_x_coord(p_to) - mp_x_coord(p_here), 
                            mp_y_coord(p_to) - mp_y_coord(p_here))
    	- mp_an_angle (mp, mp_x_coord(p_here) - mp_x_coord(p_from), 
                           mp_y_coord(p_here) - mp_y_coord(p_from));
    reduce_angle(ang);
    res  = res + ang;
    if ( res >= three_sixty_deg )  {
      res = res - three_sixty_deg;
      turns = turns + unity;
    };
    if ( res <= -three_sixty_deg ) {
      res = res + three_sixty_deg;
      turns = turns - unity;
    };
    p = mp_link(p);
  } while (p!=c);
  return turns;
}

@ @<Declare unary action...@>=
static scaled mp_turn_cycles_wrapper (MP mp,pointer c) {
  scaled nval,oval;
  scaled saved_t_o; /* tracing\_online saved  */
  if ( (mp_link(c)==c)||(mp_link(mp_link(c))==c) ) {
    if ( mp_an_angle (mp, mp_x_coord(c) - mp_right_x(c),  mp_y_coord(c) - mp_right_y(c)) > 0 )
      return unity;
    else
      return -unity;
  } else {
    nval = mp_new_turn_cycles(mp, c);
    oval = mp_turn_cycles(mp, c);
    if ( nval!=oval && mp->internal[mp_tracing_choices]>(2*unity)) {
      saved_t_o=mp->internal[mp_tracing_online];
      mp->internal[mp_tracing_online]=unity;
      mp_begin_diagnostic(mp);
      mp_print_nl (mp, "Warning: the turningnumber algorithms do not agree."
                       " The current computed value is ");
      mp_print_scaled(mp, nval);
      mp_print(mp, ", but the 'connect-the-dots' algorithm returned ");
      mp_print_scaled(mp, oval);
      mp_end_diagnostic(mp, false);
      mp->internal[mp_tracing_online]=saved_t_o;
    }
    return nval;
  }
}

@ @d type_range(A,B) { 
  if ( (mp->cur_type>=(A)) && (mp->cur_type<=(B)) ) 
    mp_flush_cur_exp(mp, true_code);
  else mp_flush_cur_exp(mp, false_code);
  mp->cur_type=mp_boolean_type;
  }
@d type_test(A) { 
  if ( mp->cur_type==(A) ) mp_flush_cur_exp(mp, true_code);
  else mp_flush_cur_exp(mp, false_code);
  mp->cur_type=mp_boolean_type;
  }

@<Additional cases of unary operators@>=
case mp_boolean_type: 
  type_range(mp_boolean_type,mp_unknown_boolean); break;
case mp_string_type: 
  type_range(mp_string_type,mp_unknown_string); break;
case mp_pen_type: 
  type_range(mp_pen_type,mp_unknown_pen); break;
case mp_path_type: 
  type_range(mp_path_type,mp_unknown_path); break;
case mp_picture_type: 
  type_range(mp_picture_type,mp_unknown_picture); break;
case mp_transform_type: case mp_color_type: case mp_cmykcolor_type:
case mp_pair_type: 
  type_test(c); break;
case mp_numeric_type: 
  type_range(mp_known,mp_independent); break;
case known_op: case unknown_op: 
  mp_test_known(mp, c); break;

@ @<Declare unary action procedures@>=
static void mp_test_known (MP mp,quarterword c) {
  int b; /* is the current expression known? */
  pointer p,q; /* locations in a big node */
  b=false_code;
  switch (mp->cur_type) {
  case mp_vacuous: case mp_boolean_type: case mp_string_type:
  case mp_pen_type: case mp_path_type: case mp_picture_type:
  case mp_known: 
    b=true_code;
    break;
  case mp_transform_type:
  case mp_color_type: case mp_cmykcolor_type: case mp_pair_type: 
    p=value(mp->cur_exp);
    q=p+mp->big_node_size[mp->cur_type];
    do {  
      q=q-2;
      if ( mp_type(q)!=mp_known ) 
       goto DONE;
    } while (q!=p);
    b=true_code;
  DONE:  
    break;
  default: 
    break;
  }
  if ( c==known_op ) mp_flush_cur_exp(mp, b);
  else mp_flush_cur_exp(mp, true_code+false_code-b);
  mp->cur_type=mp_boolean_type;
}

@ @<Additional cases of unary operators@>=
case cycle_op: 
  if ( mp->cur_type!=mp_path_type ) mp_flush_cur_exp(mp, false_code);
  else if ( mp_left_type(mp->cur_exp)!=mp_endpoint ) mp_flush_cur_exp(mp, true_code);
  else mp_flush_cur_exp(mp, false_code);
  mp->cur_type=mp_boolean_type;
  break;

@ @<Additional cases of unary operators@>=
case arc_length: 
  if ( mp->cur_type==mp_pair_type ) mp_pair_to_path(mp);
  if ( mp->cur_type!=mp_path_type ) mp_bad_unary(mp, arc_length);
  else mp_flush_cur_exp(mp, mp_get_arc_length(mp, mp->cur_exp));
  break;

@ Here we use the fact that |c-filled_op+fill_code| is the desired graphical
object |type|.
@^data structure assumptions@>

@<Additional cases of unary operators@>=
case filled_op:
case stroked_op:
case textual_op:
case clipped_op:
case bounded_op:
  if ( mp->cur_type!=mp_picture_type ) mp_flush_cur_exp(mp, false_code);
  else if ( mp_link(dummy_loc(mp->cur_exp))==null ) mp_flush_cur_exp(mp, false_code);
  else if ( mp_type(mp_link(dummy_loc(mp->cur_exp)))==c+mp_fill_code-filled_op )
    mp_flush_cur_exp(mp, true_code);
  else mp_flush_cur_exp(mp, false_code);
  mp->cur_type=mp_boolean_type;
  break;

@ @<Additional cases of unary operators@>=
case make_pen_op: 
  if ( mp->cur_type==mp_pair_type ) mp_pair_to_path(mp);
  if ( mp->cur_type!=mp_path_type ) mp_bad_unary(mp, make_pen_op);
  else { 
    mp->cur_type=mp_pen_type;
    mp->cur_exp=mp_make_pen(mp, mp->cur_exp,true);
  };
  break;
case make_path_op: 
  if ( mp->cur_type!=mp_pen_type ) mp_bad_unary(mp, make_path_op);
  else  { 
    mp->cur_type=mp_path_type;
    mp_make_path(mp, mp->cur_exp);
  };
  break;
case reverse: 
  if ( mp->cur_type==mp_path_type ) {
    p=mp_htap_ypoc(mp, mp->cur_exp);
    if ( mp_right_type(p)==mp_endpoint ) p=mp_link(p);
    mp_toss_knot_list(mp, mp->cur_exp); mp->cur_exp=p;
  } else if ( mp->cur_type==mp_pair_type ) mp_pair_to_path(mp);
  else mp_bad_unary(mp, reverse);
  break;

@ The |pair_value| routine changes the current expression to a
given ordered pair of values.

@<Declare unary action procedures@>=
static void mp_pair_value (MP mp,scaled x, scaled y) {
  pointer p; /* a pair node */
  p=mp_get_node(mp, value_node_size); 
  mp_flush_cur_exp(mp, p); mp->cur_type=mp_pair_type;
  mp_type(p)=mp_pair_type; mp_name_type(p)=mp_capsule; mp_init_big_node(mp, p);
  p=value(p);
  mp_type(x_part_loc(p))=mp_known; value(x_part_loc(p))=x;
  mp_type(y_part_loc(p))=mp_known; value(y_part_loc(p))=y;
}

@ @<Additional cases of unary operators@>=
case ll_corner_op: 
  if ( ! mp_get_cur_bbox(mp) ) mp_bad_unary(mp, ll_corner_op);
  else mp_pair_value(mp, mp_minx, mp_miny);
  break;
case lr_corner_op: 
  if ( ! mp_get_cur_bbox(mp) ) mp_bad_unary(mp, lr_corner_op);
  else mp_pair_value(mp, mp_maxx, mp_miny);
  break;
case ul_corner_op: 
  if ( ! mp_get_cur_bbox(mp) ) mp_bad_unary(mp, ul_corner_op);
  else mp_pair_value(mp, mp_minx, mp_maxy);
  break;
case ur_corner_op: 
  if ( ! mp_get_cur_bbox(mp) ) mp_bad_unary(mp, ur_corner_op);
  else mp_pair_value(mp, mp_maxx, mp_maxy);
  break;

@ Here is a function that sets |minx|, |maxx|, |miny|, |maxy| to the bounding
box of the current expression.  The boolean result is |false| if the expression
has the wrong type.

@<Declare unary action procedures@>=
static boolean mp_get_cur_bbox (MP mp) { 
  switch (mp->cur_type) {
  case mp_picture_type: 
    mp_set_bbox(mp, mp->cur_exp,true);
    if ( minx_val(mp->cur_exp)>maxx_val(mp->cur_exp) ) {
      mp_minx=0; mp_maxx=0; mp_miny=0; mp_maxy=0;
    } else { 
      mp_minx=minx_val(mp->cur_exp);
      mp_maxx=maxx_val(mp->cur_exp);
      mp_miny=miny_val(mp->cur_exp);
      mp_maxy=maxy_val(mp->cur_exp);
    }
    break;
  case mp_path_type: 
    mp_path_bbox(mp, mp->cur_exp);
    break;
  case mp_pen_type: 
    mp_pen_bbox(mp, mp->cur_exp);
    break;
  default: 
    return false;
  }
  return true;
}

@ @<Additional cases of unary operators@>=
case read_from_op:
case close_from_op: 
  if ( mp->cur_type!=mp_string_type ) mp_bad_unary(mp, c);
  else mp_do_read_or_close(mp,c);
  break;

@ Here is a routine that interprets |cur_exp| as a file name and tries to read
a line from the file or to close the file.

@<Declare unary action procedures@>=
static void mp_do_read_or_close (MP mp,quarterword c) {
  readf_index n,n0; /* indices for searching |rd_fname| */
  @<Find the |n| where |rd_fname[n]=cur_exp|; if |cur_exp| must be inserted,
    call |start_read_input| and |goto found| or |not_found|@>;
  mp_begin_file_reading(mp);
  name=is_read;
  if ( mp_input_ln(mp, mp->rd_file[n] ) ) 
    goto FOUND;
  mp_end_file_reading(mp);
NOT_FOUND:
  @<Record the end of file and set |cur_exp| to a dummy value@>;
  return;
CLOSE_FILE:
  mp_flush_cur_exp(mp, 0); mp->cur_type=mp_vacuous; 
  return;
FOUND:
  mp_flush_cur_exp(mp, 0);
  mp_finish_read(mp);
}

@ Free slots in the |rd_file| and |rd_fname| arrays are marked with NULL's in
|rd_fname|.

@<Find the |n| where |rd_fname[n]=cur_exp|...@>=
{   
  char *fn;
  n=mp->read_files;
  n0=mp->read_files;
  fn = str(mp->cur_exp);
  while (mp_xstrcmp(fn,mp->rd_fname[n])!=0) { 
    if ( n>0 ) {
      decr(n);
    } else if ( c==close_from_op ) {
      goto CLOSE_FILE;
    } else {
      if ( n0==mp->read_files ) {
        if ( mp->read_files<mp->max_read_files ) {
          incr(mp->read_files);
        } else {
          void **rd_file;
          char **rd_fname;
	      readf_index l,k;
          l = mp->max_read_files + (mp->max_read_files/4);
          rd_file = xmalloc((l+1), sizeof(void *));
          rd_fname = xmalloc((l+1), sizeof(char *));
	      for (k=0;k<=l;k++) {
            if (k<=mp->max_read_files) {
   	          rd_file[k]=mp->rd_file[k]; 
              rd_fname[k]=mp->rd_fname[k];
            } else {
 	      rd_file[k]=0; 
              rd_fname[k]=NULL;
            }
          }
	      xfree(mp->rd_file); xfree(mp->rd_fname);
          mp->max_read_files = l;
          mp->rd_file = rd_file;
          mp->rd_fname = rd_fname;
        }
      }
      n=n0;
      if ( mp_start_read_input(mp,fn,n) ) 
        goto FOUND;
      else 
        goto NOT_FOUND;
    }
    if ( mp->rd_fname[n]==NULL ) { n0=n; }
  } 
  if ( c==close_from_op ) { 
    (mp->close_file)(mp,mp->rd_file[n]); 
    goto NOT_FOUND; 
  }
}

@ @<Record the end of file and set |cur_exp| to a dummy value@>=
xfree(mp->rd_fname[n]);
mp->rd_fname[n]=NULL;
if ( n==mp->read_files-1 ) mp->read_files=n;
if ( c==close_from_op ) 
  goto CLOSE_FILE;
mp_flush_cur_exp(mp, mp->eof_line);
mp->cur_type=mp_string_type

@ The string denoting end-of-file is a one-byte string at position zero, by definition

@<Glob...@>=
str_number eof_line;

@ @<Set init...@>=
mp->eof_line=0;

@ Finally, we have the operations that combine a capsule~|p|
with the current expression.

@d binary_return  { mp_finish_binary(mp, old_p, old_exp); return; }

@c @<Declare binary action procedures@>
static void mp_finish_binary (MP mp, pointer old_p, pointer old_exp ){
  check_arith; 
  @<Recycle any sidestepped |independent| capsules@>;
}
static void mp_do_binary (MP mp,pointer p, integer c) {
  pointer q,r,rr; /* for list manipulation */
  pointer old_p,old_exp; /* capsules to recycle */
  integer v; /* for numeric manipulation */
  check_arith;
  if ( mp->internal[mp_tracing_commands]>two ) {
    @<Trace the current binary operation@>;
  }
  @<Sidestep |independent| cases in capsule |p|@>;
  @<Sidestep |independent| cases in the current expression@>;
  switch (c) {
  case plus: case minus:
    @<Add or subtract the current expression from |p|@>;
    break;
  @<Additional cases of binary operators@>;
  }; /* there are no other cases */
  mp_recycle_value(mp, p); 
  mp_free_node(mp, p,value_node_size); /* |return| to avoid this */
  mp_finish_binary(mp, old_p, old_exp);
}

@ @<Declare binary action...@>=
static void mp_bad_binary (MP mp,pointer p, quarterword c) { 
  mp_disp_err(mp, p,"");
  exp_err("Not implemented: ");
@.Not implemented...@>
  if ( c>=min_of ) mp_print_op(mp, c);
  mp_print_known_or_unknown_type(mp, mp_type(p),p);
  if ( c>=min_of ) mp_print(mp, "of"); else mp_print_op(mp, c);
  mp_print_known_or_unknown_type(mp, mp->cur_type,mp->cur_exp);
  help3("I'm afraid I don't know how to apply that operation to that",
       "combination of types. Continue, and I'll return the second",
       "argument (see above) as the result of the operation.");
  mp_put_get_error(mp);
}
static void mp_bad_envelope_pen (MP mp) {
  mp_disp_err(mp, null,"");
  exp_err("Not implemented: envelope(elliptical pen)of(path)");
@.Not implemented...@>
  help3("I'm afraid I don't know how to apply that operation to that",
       "combination of types. Continue, and I'll return the second",
       "argument (see above) as the result of the operation.");
  mp_put_get_error(mp);
}

@ @<Trace the current binary operation@>=
{ 
  mp_begin_diagnostic(mp); mp_print_nl(mp, "{(");
  mp_print_exp(mp,p,0); /* show the operand, but not verbosely */
  mp_print_char(mp,xord(')')); mp_print_op(mp, (quarterword)c); 
  mp_print_char(mp,xord('('));
  mp_print_exp(mp,null,0); mp_print(mp,")}"); 
  mp_end_diagnostic(mp, false);
}

@ Several of the binary operations are potentially complicated by the
fact that |independent| values can sneak into capsules. For example,
we've seen an instance of this difficulty in the unary operation
of negation. In order to reduce the number of cases that need to be
handled, we first change the two operands (if necessary)
to rid them of |independent| components. The original operands are
put into capsules called |old_p| and |old_exp|, which will be
recycled after the binary operation has been safely carried out.

@<Recycle any sidestepped |independent| capsules@>=
if ( old_p!=null ) { 
  mp_recycle_value(mp, old_p); mp_free_node(mp, old_p,value_node_size);
}
if ( old_exp!=null ) {
  mp_recycle_value(mp, old_exp); mp_free_node(mp, old_exp,value_node_size);
}

@ A big node is considered to be ``tarnished'' if it contains at least one
independent component. We will define a simple function called `|tarnished|'
that returns |null| if and only if its argument is not tarnished.

@<Sidestep |independent| cases in capsule |p|@>=
switch (mp_type(p)) {
case mp_transform_type:
case mp_color_type:
case mp_cmykcolor_type:
case mp_pair_type: 
  old_p=mp_tarnished(mp, p);
  break;
case mp_independent: old_p=mp_void; break;
default: old_p=null; break;
}
if ( old_p!=null ) {
  q=mp_stash_cur_exp(mp); old_p=p; mp_make_exp_copy(mp, old_p);
  p=mp_stash_cur_exp(mp); mp_unstash_cur_exp(mp, q);
}

@ @<Sidestep |independent| cases in the current expression@>=
switch (mp->cur_type) {
case mp_transform_type:
case mp_color_type:
case mp_cmykcolor_type:
case mp_pair_type: 
  old_exp=mp_tarnished(mp, mp->cur_exp);
  break;
case mp_independent:old_exp=mp_void; break;
default: old_exp=null; break;
}
if ( old_exp!=null ) {
  old_exp=mp->cur_exp; mp_make_exp_copy(mp, old_exp);
}

@ @<Declare binary action...@>=
static pointer mp_tarnished (MP mp,pointer p) {
  pointer q; /* beginning of the big node */
  pointer r; /* current position in the big node */
  q=value(p); r=q+mp->big_node_size[mp_type(p)];
  do {  
   r=r-2;
   if ( mp_type(r)==mp_independent ) return mp_void; 
  } while (r!=q);
  return null;
}

@ @<Add or subtract the current expression from |p|@>=
if ( (mp->cur_type<mp_color_type)||(mp_type(p)<mp_color_type) ) {
  mp_bad_binary(mp, p, (quarterword)c);
} else  {
  if ((mp->cur_type>mp_pair_type)&&(mp_type(p)>mp_pair_type) ) {
    mp_add_or_subtract(mp, p,null, (quarterword)c);
  } else {
    if ( mp->cur_type!=mp_type(p) )  {
      mp_bad_binary(mp, p, (quarterword)c);
    } else { 
      q=value(p); r=value(mp->cur_exp);
      rr=r+mp->big_node_size[mp->cur_type];
      while ( r<rr ) { 
        mp_add_or_subtract(mp, q, r, (quarterword)c);
        q=q+2; r=r+2;
      }
    }
  }
}

@ The first argument to |add_or_subtract| is the location of a value node
in a capsule or pair node that will soon be recycled. The second argument
is either a location within a pair or transform node of |cur_exp|,
or it is null (which means that |cur_exp| itself should be the second
argument).  The third argument is either |plus| or |minus|.

The sum or difference of the numeric quantities will replace the second
operand.  Arithmetic overflow may go undetected; users aren't supposed to
be monkeying around with really big values.
@^overflow in arithmetic@>

@<Declare binary action...@>=
@<Declare the procedure called |dep_finish|@>
static void mp_add_or_subtract (MP mp,pointer p, pointer q, quarterword c) {
  quarterword s,t; /* operand types */
  pointer r; /* list traverser */
  integer v; /* second operand value */
  if ( q==null ) { 
    t=mp->cur_type;
    if ( t<mp_dependent ) v=mp->cur_exp; else v=dep_list(mp->cur_exp);
  } else { 
    t=mp_type(q);
    if ( t<mp_dependent ) v=value(q); else v=dep_list(q);
  }
  if ( t==mp_known ) {
    if ( c==minus ) negate(v);
    if ( mp_type(p)==mp_known ) {
      v=mp_slow_add(mp, value(p),v);
      if ( q==null ) mp->cur_exp=v; else value(q)=v;
      return;
    }
    @<Add a known value to the constant term of |dep_list(p)|@>;
  } else  { 
    if ( c==minus ) mp_negate_dep_list(mp, v);
    @<Add operand |p| to the dependency list |v|@>;
  }
}

@ @<Add a known value to the constant term of |dep_list(p)|@>=
r=dep_list(p);
while ( mp_info(r)!=null ) r=mp_link(r);
value(r)=mp_slow_add(mp, value(r),v);
if ( q==null ) {
  q=mp_get_node(mp, value_node_size); mp->cur_exp=q; mp->cur_type=mp_type(p);
  mp_name_type(q)=mp_capsule;
}
dep_list(q)=dep_list(p); mp_type(q)=mp_type(p);
prev_dep(q)=prev_dep(p); mp_link(prev_dep(p))=q;
mp_type(p)=mp_known; /* this will keep the recycler from collecting non-garbage */

@ We prefer |dependent| lists to |mp_proto_dependent| ones, because it is
nice to retain the extra accuracy of |fraction| coefficients.
But we have to handle both kinds, and mixtures too.

@<Add operand |p| to the dependency list |v|@>=
if ( mp_type(p)==mp_known ) {
  @<Add the known |value(p)| to the constant term of |v|@>;
} else { 
  s=mp_type(p); r=dep_list(p);
  if ( t==mp_dependent ) {
    if ( s==mp_dependent ) {
      if ( mp_max_coef(mp, r)+mp_max_coef(mp, v)<coef_bound ) {
          v=mp_p_plus_q(mp, v,r,mp_dependent); goto DONE;
        } 
      } /* |fix_needed| will necessarily be false */
    t=mp_proto_dependent; 
    v=mp_p_over_v(mp, v,unity,mp_dependent,mp_proto_dependent);
  }
  if ( s==mp_proto_dependent ) v=mp_p_plus_q(mp, v,r,mp_proto_dependent);
  else v=mp_p_plus_fq(mp, v,unity,r,mp_proto_dependent,mp_dependent);
 DONE:  
  @<Output the answer, |v| (which might have become |known|)@>;
}

@ @<Add the known |value(p)| to the constant term of |v|@>=
{ 
  while ( mp_info(v)!=null ) v=mp_link(v);
  value(v)=mp_slow_add(mp, value(p),value(v));
}

@ @<Output the answer, |v| (which might have become |known|)@>=
if ( q!=null ) mp_dep_finish(mp, v,q,t);
else  { mp->cur_type=t; mp_dep_finish(mp, v,null,t); }

@ Here's the current situation: The dependency list |v| of type |t|
should either be put into the current expression (if |q=null|) or
into location |q| within a pair node (otherwise). The destination (|cur_exp|
or |q|) formerly held a dependency list with the same
final pointer as the list |v|.

@<Declare the procedure called |dep_finish|@>=
static void mp_dep_finish (MP mp, pointer v, pointer q, quarterword t) {
  pointer p; /* the destination */
  scaled vv; /* the value, if it is |known| */
  if ( q==null ) p=mp->cur_exp; else p=q;
  dep_list(p)=v; mp_type(p)=t;
  if ( mp_info(v)==null ) { 
    vv=value(v);
    if ( q==null ) { 
      mp_flush_cur_exp(mp, vv);
    } else  { 
      mp_recycle_value(mp, p); mp_type(q)=mp_known; value(q)=vv; 
    }
  } else if ( q==null ) {
    mp->cur_type=t;
  }
  if ( mp->fix_needed ) mp_fix_dependencies(mp);
}

@ Let's turn now to the six basic relations of comparison.

@<Additional cases of binary operators@>=
case less_than: case less_or_equal: case greater_than:
case greater_or_equal: case equal_to: case unequal_to:
  check_arith; /* at this point |arith_error| should be |false|? */
  if ( (mp->cur_type>mp_pair_type)&&(mp_type(p)>mp_pair_type) ) {
    mp_add_or_subtract(mp, p,null,minus); /* |cur_exp:=(p)-cur_exp| */
  } else if ( mp->cur_type!=mp_type(p) ) {
    mp_bad_binary(mp, p, (quarterword)c); goto DONE; 
  } else if ( mp->cur_type==mp_string_type ) {
    mp_flush_cur_exp(mp, mp_str_vs_str(mp, value(p),mp->cur_exp));
  } else if ((mp->cur_type==mp_unknown_string)||
           (mp->cur_type==mp_unknown_boolean) ) {
    @<Check if unknowns have been equated@>;
  } else if ( (mp->cur_type<=mp_pair_type)&&(mp->cur_type>=mp_transform_type)) {
    @<Reduce comparison of big nodes to comparison of scalars@>;
  } else if ( mp->cur_type==mp_boolean_type ) {
    mp_flush_cur_exp(mp, mp->cur_exp-value(p));
  } else { 
    mp_bad_binary(mp, p, (quarterword)c); goto DONE;
  }
  @<Compare the current expression with zero@>;
DONE:  
  mp->arith_error=false; /* ignore overflow in comparisons */
  break;

@ @<Compare the current expression with zero@>=
if ( mp->cur_type!=mp_known ) {
  if ( mp->cur_type<mp_known ) {
    mp_disp_err(mp, p,"");
    help1("The quantities shown above have not been equated.")
  } else  {
    help2("Oh dear. I can\'t decide if the expression above is positive,",
          "negative, or zero. So this comparison test won't be `true'.");
  }
  exp_err("Unknown relation will be considered false");
@.Unknown relation...@>
  mp_put_get_flush_error(mp, false_code);
} else {
  switch (c) {
  case less_than: boolean_reset(mp->cur_exp<0); break;
  case less_or_equal: boolean_reset(mp->cur_exp<=0); break;
  case greater_than: boolean_reset(mp->cur_exp>0); break;
  case greater_or_equal: boolean_reset(mp->cur_exp>=0); break;
  case equal_to: boolean_reset(mp->cur_exp==0); break;
  case unequal_to: boolean_reset(mp->cur_exp!=0); break;
  }; /* there are no other cases */
}
mp->cur_type=mp_boolean_type

@ When two unknown strings are in the same ring, we know that they are
equal. Otherwise, we don't know whether they are equal or not, so we
make no change.

@<Check if unknowns have been equated@>=
{ 
  q=value(mp->cur_exp);
  while ( (q!=mp->cur_exp)&&(q!=p) ) q=value(q);
  if ( q==p ) mp_flush_cur_exp(mp, 0);
}

@ @<Reduce comparison of big nodes to comparison of scalars@>=
{ 
  q=value(p); r=value(mp->cur_exp);
  rr=r+mp->big_node_size[mp->cur_type]-2;
  while (1) { mp_add_or_subtract(mp, q,r,minus);
    if ( mp_type(r)!=mp_known ) break;
    if ( value(r)!=0 ) break;
    if ( r==rr ) break;
    q=q+2; r=r+2;
  }
  mp_take_part(mp, (quarterword)(mp_name_type(r)+x_part-mp_x_part_sector));
}

@ Here we use the sneaky fact that |and_op-false_code=or_op-true_code|.

@<Additional cases of binary operators@>=
case and_op:
case or_op: 
  if ( (mp_type(p)!=mp_boolean_type)||(mp->cur_type!=mp_boolean_type) )
    mp_bad_binary(mp, p, (quarterword)c);
  else if ( value(p)==c+false_code-and_op ) mp->cur_exp=value(p);
  break;

@ @<Additional cases of binary operators@>=
case times: 
  if ( (mp->cur_type<mp_color_type)||(mp_type(p)<mp_color_type) ) {
   mp_bad_binary(mp, p, times);
  } else if ( (mp->cur_type==mp_known)||(mp_type(p)==mp_known) ) {
    @<Multiply when at least one operand is known@>;
  } else if ( (mp_nice_color_or_pair(mp, p,mp_type(p))&&(mp->cur_type>mp_pair_type))
      ||(mp_nice_color_or_pair(mp, mp->cur_exp,mp->cur_type)&&
          (mp_type(p)>mp_pair_type)) ) {
    mp_hard_times(mp, p); 
    binary_return;
  } else {
    mp_bad_binary(mp, p,times);
  }
  break;

@ @<Multiply when at least one operand is known@>=
{ 
  if ( mp_type(p)==mp_known ) {
    v=value(p); mp_free_node(mp, p,value_node_size); 
  } else {
    v=mp->cur_exp; mp_unstash_cur_exp(mp, p);
  }
  if ( mp->cur_type==mp_known ) {
    mp->cur_exp=mp_take_scaled(mp, mp->cur_exp,v);
  } else if ( (mp->cur_type==mp_pair_type)||
              (mp->cur_type==mp_color_type)||
              (mp->cur_type==mp_cmykcolor_type) ) {
    p=value(mp->cur_exp)+mp->big_node_size[mp->cur_type];
    do {  
       p=p-2; mp_dep_mult(mp, p,v,true);
    } while (p!=value(mp->cur_exp));
  } else {
    mp_dep_mult(mp, null,v,true);
  }
  binary_return;
}

@ @<Declare binary action...@>=
static void mp_dep_mult (MP mp,pointer p, integer v, boolean v_is_scaled) {
  pointer q; /* the dependency list being multiplied by |v| */
  quarterword s,t; /* its type, before and after */
  if ( p==null ) {
    q=mp->cur_exp;
  } else if ( mp_type(p)!=mp_known ) {
    q=p;
  } else { 
    if ( v_is_scaled ) value(p)=mp_take_scaled(mp, value(p),v);
    else value(p)=mp_take_fraction(mp, value(p),v);
    return;
  };
  t=mp_type(q); q=dep_list(q); s=t;
  if ( t==mp_dependent ) if ( v_is_scaled )
    if (mp_ab_vs_cd(mp, mp_max_coef(mp,q),abs(v),coef_bound-1,unity)>=0 ) 
      t=mp_proto_dependent;
  q=mp_p_times_v(mp, q,v,s,t,v_is_scaled); 
  mp_dep_finish(mp, q,p,t);
}

@ Here is a routine that is similar to |times|; but it is invoked only
internally, when |v| is a |fraction| whose magnitude is at most~1,
and when |cur_type>=mp_color_type|.

@c 
static void mp_frac_mult (MP mp,scaled n, scaled d) {
  /* multiplies |cur_exp| by |n/d| */
  pointer p; /* a pair node */
  pointer old_exp; /* a capsule to recycle */
  fraction v; /* |n/d| */
  if ( mp->internal[mp_tracing_commands]>two ) {
    @<Trace the fraction multiplication@>;
  }
  switch (mp->cur_type) {
  case mp_transform_type:
  case mp_color_type:
  case mp_cmykcolor_type:
  case mp_pair_type:
   old_exp=mp_tarnished(mp, mp->cur_exp);
   break;
  case mp_independent: old_exp=mp_void; break;
  default: old_exp=null; break;
  }
  if ( old_exp!=null ) { 
     old_exp=mp->cur_exp; mp_make_exp_copy(mp, old_exp);
  }
  v=mp_make_fraction(mp, n,d);
  if ( mp->cur_type==mp_known ) {
    mp->cur_exp=mp_take_fraction(mp, mp->cur_exp,v);
  } else if ( mp->cur_type<=mp_pair_type ) { 
    p=value(mp->cur_exp)+mp->big_node_size[mp->cur_type];
    do {  
      p=p-2;
      mp_dep_mult(mp, p,v,false);
    } while (p!=value(mp->cur_exp));
  } else {
    mp_dep_mult(mp, null,v,false);
  }
  if ( old_exp!=null ) {
    mp_recycle_value(mp, old_exp); 
    mp_free_node(mp, old_exp,value_node_size);
  }
}

@ @<Trace the fraction multiplication@>=
{ 
  mp_begin_diagnostic(mp); 
  mp_print_nl(mp, "{("); mp_print_scaled(mp,n); mp_print_char(mp,xord('/'));
  mp_print_scaled(mp,d); mp_print(mp,")*("); mp_print_exp(mp,null,0); 
  mp_print(mp,")}");
  mp_end_diagnostic(mp, false);
}

@ The |hard_times| routine multiplies a nice color or pair by a dependency list.

@<Declare binary action procedures@>=
static void mp_hard_times (MP mp,pointer p) {
  pointer q; /* a copy of the dependent variable |p| */
  pointer r; /* a component of the big node for the nice color or pair */
  scaled v; /* the known value for |r| */
  if ( mp_type(p)<=mp_pair_type ) { 
     q=mp_stash_cur_exp(mp); mp_unstash_cur_exp(mp, p); p=q;
  }; /* now |cur_type=mp_pair_type| or |cur_type=mp_color_type| */
  r=value(mp->cur_exp)+mp->big_node_size[mp->cur_type];
  while (1) { 
    r=r-2;
    v=value(r);
    mp_type(r)=mp_type(p);
    if ( r==value(mp->cur_exp) ) 
      break;
    mp_new_dep(mp, r,mp_copy_dep_list(mp, dep_list(p)));
    mp_dep_mult(mp, r,v,true);
  }
  mp->mem[value_loc(r)]=mp->mem[value_loc(p)];
  mp_link(prev_dep(p))=r;
  mp_free_node(mp, p,value_node_size);
  mp_dep_mult(mp, r,v,true);
}

@ @<Additional cases of binary operators@>=
case over: 
  if ( (mp->cur_type!=mp_known)||(mp_type(p)<mp_color_type) ) {
    mp_bad_binary(mp, p,over);
  } else { 
    v=mp->cur_exp; mp_unstash_cur_exp(mp, p);
    if ( v==0 ) {
      @<Squeal about division by zero@>;
    } else { 
      if ( mp->cur_type==mp_known ) {
        mp->cur_exp=mp_make_scaled(mp, mp->cur_exp,v);
      } else if ( mp->cur_type<=mp_pair_type ) { 
        p=value(mp->cur_exp)+mp->big_node_size[mp->cur_type];
        do {  
          p=p-2;  mp_dep_div(mp, p,v);
        } while (p!=value(mp->cur_exp));
      } else {
        mp_dep_div(mp, null,v);
      }
    }
    binary_return;
  }
  break;

@ @<Declare binary action...@>=
static void mp_dep_div (MP mp,pointer p, scaled v) {
  pointer q; /* the dependency list being divided by |v| */
  quarterword s,t; /* its type, before and after */
  if ( p==null ) q=mp->cur_exp;
  else if ( mp_type(p)!=mp_known ) q=p;
  else { value(p)=mp_make_scaled(mp, value(p),v); return; };
  t=mp_type(q); q=dep_list(q); s=t;
  if ( t==mp_dependent )
    if ( mp_ab_vs_cd(mp, mp_max_coef(mp,q),unity,coef_bound-1,abs(v))>=0 ) 
      t=mp_proto_dependent;
  q=mp_p_over_v(mp, q,v,s,t); 
  mp_dep_finish(mp, q,p,t);
}

@ @<Squeal about division by zero@>=
{ 
  exp_err("Division by zero");
@.Division by zero@>
  help2("You're trying to divide the quantity shown above the error",
        "message by zero. I'm going to divide it by one instead.");
  mp_put_get_error(mp);
}

@ @<Additional cases of binary operators@>=
case pythag_add:
case pythag_sub: 
   if ( (mp->cur_type==mp_known)&&(mp_type(p)==mp_known) ) {
     if ( c==pythag_add ) mp->cur_exp=mp_pyth_add(mp, value(p),mp->cur_exp);
     else mp->cur_exp=mp_pyth_sub(mp, value(p),mp->cur_exp);
   } else mp_bad_binary(mp, p, (quarterword)c);
   break;

@ The next few sections of the program deal with affine transformations
of coordinate data.

@<Additional cases of binary operators@>=
case rotated_by: case slanted_by:
case scaled_by: case shifted_by: case transformed_by:
case x_scaled: case y_scaled: case z_scaled:
  if ( mp_type(p)==mp_path_type ) { 
    path_trans((quarterword)c, p); binary_return;
  } else if ( mp_type(p)==mp_pen_type ) { 
    pen_trans((quarterword)c, p);
    mp->cur_exp=mp_convex_hull(mp, mp->cur_exp); 
      /* rounding error could destroy convexity */
    binary_return;
  } else if ( (mp_type(p)==mp_pair_type)||(mp_type(p)==mp_transform_type) ) {
    mp_big_trans(mp, p, (quarterword)c);
  } else if ( mp_type(p)==mp_picture_type ) {
    mp_do_edges_trans(mp, p, (quarterword)c); binary_return;
  } else {
    mp_bad_binary(mp, p, (quarterword)c);
  }
  break;

@ Let |c| be one of the eight transform operators. The procedure call
|set_up_trans(c)| first changes |cur_exp| to a transform that corresponds to
|c| and the original value of |cur_exp|. (In particular, |cur_exp| doesn't
change at all if |c=transformed_by|.)

Then, if all components of the resulting transform are |known|, they are
moved to the global variables |txx|, |txy|, |tyx|, |tyy|, |tx|, |ty|;
and |cur_exp| is changed to the known value zero.

@<Declare binary action...@>=
static void mp_set_up_trans (MP mp,quarterword c) {
  pointer p,q,r; /* list manipulation registers */
  if ( (c!=transformed_by)||(mp->cur_type!=mp_transform_type) ) {
    @<Put the current transform into |cur_exp|@>;
  }
  @<If the current transform is entirely known, stash it in global variables;
    otherwise |return|@>;
}

@ @<Glob...@>=
scaled txx;
scaled txy;
scaled tyx;
scaled tyy;
scaled tx;
scaled ty; /* current transform coefficients */

@ @<Put the current transform...@>=
{ 
  p=mp_stash_cur_exp(mp); 
  mp->cur_exp=mp_id_transform(mp); 
  mp->cur_type=mp_transform_type;
  q=value(mp->cur_exp);
  switch (c) {
  @<For each of the eight cases, change the relevant fields of |cur_exp|
    and |goto done|;
    but do nothing if capsule |p| doesn't have the appropriate type@>;
  }; /* there are no other cases */
  mp_disp_err(mp, p,"Improper transformation argument");
@.Improper transformation argument@>
  help3("The expression shown above has the wrong type,",
       "so I can\'t transform anything using it.",
       "Proceed, and I'll omit the transformation.");
  mp_put_get_error(mp);
DONE: 
  mp_recycle_value(mp, p); 
  mp_free_node(mp, p,value_node_size);
}

@ @<If the current transform is entirely known, ...@>=
q=value(mp->cur_exp); r=q+transform_node_size;
do {  
  r=r-2;
  if ( mp_type(r)!=mp_known ) return;
} while (r!=q);
mp->txx=value(xx_part_loc(q));
mp->txy=value(xy_part_loc(q));
mp->tyx=value(yx_part_loc(q));
mp->tyy=value(yy_part_loc(q));
mp->tx=value(x_part_loc(q));
mp->ty=value(y_part_loc(q));
mp_flush_cur_exp(mp, 0)

@ @<For each of the eight cases...@>=
case rotated_by:
  if ( mp_type(p)==mp_known )
    @<Install sines and cosines, then |goto done|@>;
  break;
case slanted_by:
  if ( mp_type(p)>mp_pair_type ) { 
   mp_install(mp, xy_part_loc(q),p); goto DONE;
  };
  break;
case scaled_by:
  if ( mp_type(p)>mp_pair_type ) { 
    mp_install(mp, xx_part_loc(q),p); mp_install(mp, yy_part_loc(q),p); 
    goto DONE;
  };
  break;
case shifted_by:
  if ( mp_type(p)==mp_pair_type ) {
    r=value(p); mp_install(mp, x_part_loc(q),x_part_loc(r));
    mp_install(mp, y_part_loc(q),y_part_loc(r)); goto DONE;
  };
  break;
case x_scaled:
  if ( mp_type(p)>mp_pair_type ) {
    mp_install(mp, xx_part_loc(q),p); goto DONE;
  };
  break;
case y_scaled:
  if ( mp_type(p)>mp_pair_type ) {
    mp_install(mp, yy_part_loc(q),p); goto DONE;
  };
  break;
case z_scaled:
  if ( mp_type(p)==mp_pair_type )
    @<Install a complex multiplier, then |goto done|@>;
  break;
case transformed_by:
  break;
  

@ @<Install sines and cosines, then |goto done|@>=
{ mp_n_sin_cos(mp, (value(p) % three_sixty_units)*16);
  value(xx_part_loc(q))=mp_round_fraction(mp, mp->n_cos);
  value(yx_part_loc(q))=mp_round_fraction(mp, mp->n_sin);
  value(xy_part_loc(q))=-value(yx_part_loc(q));
  value(yy_part_loc(q))=value(xx_part_loc(q));
  goto DONE;
}

@ @<Install a complex multiplier, then |goto done|@>=
{ 
  r=value(p);
  mp_install(mp, xx_part_loc(q),x_part_loc(r));
  mp_install(mp, yy_part_loc(q),x_part_loc(r));
  mp_install(mp, yx_part_loc(q),y_part_loc(r));
  if ( mp_type(y_part_loc(r))==mp_known ) negate(value(y_part_loc(r)));
  else mp_negate_dep_list(mp, dep_list(y_part_loc(r)));
  mp_install(mp, xy_part_loc(q),y_part_loc(r));
  goto DONE;
}

@ Procedure |set_up_known_trans| is like |set_up_trans|, but it
insists that the transformation be entirely known.

@<Declare binary action...@>=
static void mp_set_up_known_trans (MP mp,quarterword c) { 
  mp_set_up_trans(mp, c);
  if ( mp->cur_type!=mp_known ) {
    exp_err("Transform components aren't all known");
@.Transform components...@>
    help3("I'm unable to apply a partially specified transformation",
      "except to a fully known pair or transform.",
      "Proceed, and I'll omit the transformation.");
    mp_put_get_flush_error(mp, 0);
    mp->txx=unity; mp->txy=0; mp->tyx=0; mp->tyy=unity; 
    mp->tx=0; mp->ty=0;
  }
}

@ Here's a procedure that applies the transform |txx..ty| to a pair of
coordinates in locations |p| and~|q|.

@<Declare binary action...@>= 
static void mp_trans (MP mp,pointer p, pointer q) {
  scaled v; /* the new |x| value */
  v=mp_take_scaled(mp, mp->mem[p].sc,mp->txx)+
  mp_take_scaled(mp, mp->mem[q].sc,mp->txy)+mp->tx;
  mp->mem[q].sc=mp_take_scaled(mp, mp->mem[p].sc,mp->tyx)+
  mp_take_scaled(mp, mp->mem[q].sc,mp->tyy)+mp->ty;
  mp->mem[p].sc=v;
}

@ The simplest transformation procedure applies a transform to all
coordinates of a path.  The |path_trans(c)(p)| macro applies
a transformation defined by |cur_exp| and the transform operator |c|
to the path~|p|.

@d path_trans(A,B) { mp_set_up_known_trans(mp, (A)); 
                     mp_unstash_cur_exp(mp, (B)); 
                     mp_do_path_trans(mp, mp->cur_exp); }

@<Declare binary action...@>=
static void mp_do_path_trans (MP mp,pointer p) {
  pointer q; /* list traverser */
  q=p;
  do { 
    if ( mp_left_type(q)!=mp_endpoint ) 
      mp_trans(mp, q+3,q+4); /* that's |mp_left_x| and |mp_left_y| */
    mp_trans(mp, q+1,q+2); /* that's |mp_x_coord| and |mp_y_coord| */
    if ( mp_right_type(q)!=mp_endpoint ) 
      mp_trans(mp, q+5,q+6); /* that's |mp_right_x| and |mp_right_y| */
@^data structure assumptions@>
    q=mp_link(q);
  } while (q!=p);
}

@ Transforming a pen is very similar, except that there are no |mp_left_type|
and |mp_right_type| fields.

@d pen_trans(A,B) { mp_set_up_known_trans(mp, (A)); 
                    mp_unstash_cur_exp(mp, (B)); 
                    mp_do_pen_trans(mp, mp->cur_exp); }

@<Declare binary action...@>=
static void mp_do_pen_trans (MP mp,pointer p) {
  pointer q; /* list traverser */
  if ( pen_is_elliptical(p) ) {
    mp_trans(mp, p+3,p+4); /* that's |mp_left_x| and |mp_left_y| */
    mp_trans(mp, p+5,p+6); /* that's |mp_right_x| and |mp_right_y| */
  };
  q=p;
  do { 
    mp_trans(mp, q+1,q+2); /* that's |mp_x_coord| and |mp_y_coord| */
@^data structure assumptions@>
    q=mp_link(q);
  } while (q!=p);
}

@ The next transformation procedure applies to edge structures. It will do
any transformation, but the results may be substandard if the picture contains
text that uses downloaded bitmap fonts.  The binary action procedure is
|do_edges_trans|, but we also need a function that just scales a picture.
That routine is |scale_edges|.  Both it and the underlying routine |edges_trans|
should be thought of as procedures that update an edge structure |h|, except
that they have to return a (possibly new) structure because of the need to call
|private_edges|.

@<Declare binary action...@>=
static pointer mp_edges_trans (MP mp, pointer h) {
  pointer q; /* the object being transformed */
  pointer r,s; /* for list manipulation */
  scaled sx,sy; /* saved transformation parameters */
  scaled sqdet; /* square root of determinant for |dash_scale| */
  integer sgndet; /* sign of the determinant */
  scaled v; /* a temporary value */
  h=mp_private_edges(mp, h);
  sqdet=mp_sqrt_det(mp, mp->txx,mp->txy,mp->tyx,mp->tyy);
  sgndet=mp_ab_vs_cd(mp, mp->txx,mp->tyy,mp->txy,mp->tyx);
  if ( dash_list(h)!=null_dash ) {
    @<Try to transform the dash list of |h|@>;
  }
  @<Make the bounding box of |h| unknown if it can't be updated properly
    without scanning the whole structure@>;  
  q=mp_link(dummy_loc(h));
  while ( q!=null ) { 
    @<Transform graphical object |q|@>;
    q=mp_link(q);
  }
  return h;
}
static void mp_do_edges_trans (MP mp,pointer p, quarterword c) { 
  mp_set_up_known_trans(mp, c);
  value(p)=mp_edges_trans(mp, value(p));
  mp_unstash_cur_exp(mp, p);
}
static void mp_scale_edges (MP mp) { 
  mp->txx=mp->se_sf; mp->tyy=mp->se_sf;
  mp->txy=0; mp->tyx=0; mp->tx=0; mp->ty=0;
  mp->se_pic=mp_edges_trans(mp, mp->se_pic);
}

@ @<Try to transform the dash list of |h|@>=
if ( (mp->txy!=0)||(mp->tyx!=0)||
     (mp->ty!=0)||(abs(mp->txx)!=abs(mp->tyy))) {
  mp_flush_dash_list(mp, h);
} else { 
  if ( mp->txx<0 ) { @<Reverse the dash list of |h|@>; } 
  @<Scale the dash list by |txx| and shift it by |tx|@>;
  dash_y(h)=mp_take_scaled(mp, dash_y(h),abs(mp->tyy));
}

@ @<Reverse the dash list of |h|@>=
{ 
  r=dash_list(h);
  dash_list(h)=null_dash;
  while ( r!=null_dash ) {
    s=r; r=mp_link(r);
    v=start_x(s); start_x(s)=stop_x(s); stop_x(s)=v;
    mp_link(s)=dash_list(h);
    dash_list(h)=s;
  }
}

@ @<Scale the dash list by |txx| and shift it by |tx|@>=
r=dash_list(h);
while ( r!=null_dash ) {
  start_x(r)=mp_take_scaled(mp, start_x(r),mp->txx)+mp->tx;
  stop_x(r)=mp_take_scaled(mp, stop_x(r),mp->txx)+mp->tx;
  r=mp_link(r);
}

@ @<Make the bounding box of |h| unknown if it can't be updated properly...@>=
if ( (mp->txx==0)&&(mp->tyy==0) ) {
  @<Swap the $x$ and $y$ parameters in the bounding box of |h|@>;
} else if ( (mp->txy!=0)||(mp->tyx!=0) ) {
  mp_init_bbox(mp, h);
  goto DONE1;
}
if ( minx_val(h)<=maxx_val(h) ) {
  @<Scale the bounding box by |txx+txy| and |tyx+tyy|; then shift by
   |(tx,ty)|@>;
}
DONE1:



@ @<Swap the $x$ and $y$ parameters in the bounding box of |h|@>=
{ 
  v=minx_val(h); minx_val(h)=miny_val(h); miny_val(h)=v;
  v=maxx_val(h); maxx_val(h)=maxy_val(h); maxy_val(h)=v;
}

@ The sum ``|txx+txy|'' is whichever of |txx| or |txy| is nonzero.  The other
sum is similar.

@<Scale the bounding box by |txx+txy| and |tyx+tyy|; then shift...@>=
{ 
  minx_val(h)=mp_take_scaled(mp, minx_val(h),mp->txx+mp->txy)+mp->tx;
  maxx_val(h)=mp_take_scaled(mp, maxx_val(h),mp->txx+mp->txy)+mp->tx;
  miny_val(h)=mp_take_scaled(mp, miny_val(h),mp->tyx+mp->tyy)+mp->ty;
  maxy_val(h)=mp_take_scaled(mp, maxy_val(h),mp->tyx+mp->tyy)+mp->ty;
  if ( mp->txx+mp->txy<0 ) {
    v=minx_val(h); minx_val(h)=maxx_val(h); maxx_val(h)=v;
  }
  if ( mp->tyx+mp->tyy<0 ) {
    v=miny_val(h); miny_val(h)=maxy_val(h); maxy_val(h)=v;
  }
}

@ Now we ready for the main task of transforming the graphical objects in edge
structure~|h|.

@<Transform graphical object |q|@>=
switch (mp_type(q)) {
case mp_fill_code: case mp_stroked_code: 
  mp_do_path_trans(mp, mp_path_p(q));
  @<Transform |mp_pen_p(q)|, making sure polygonal pens stay counter-clockwise@>;
  break;
case mp_start_clip_code: case mp_start_bounds_code: 
  mp_do_path_trans(mp, mp_path_p(q));
  break;
case mp_text_code: 
  r=text_tx_loc(q);
  @<Transform the compact transformation starting at |r|@>;
  break;
case mp_stop_clip_code: case mp_stop_bounds_code: 
  break;
} /* there are no other cases */

@ Note that the shift parameters |(tx,ty)| apply only to the path being stroked.
The |dash_scale| has to be adjusted  to scale the dash lengths in |mp_dash_p(q)|
since the \ps\ output procedures will try to compensate for the transformation
we are applying to |mp_pen_p(q)|.  Since this compensation is based on the square
root of the determinant, |sqdet| is the appropriate factor.

We pass the mptrap test only if |dash_scale| is not adjusted, nowadays
(backend is changed?)

@<Transform |mp_pen_p(q)|, making sure...@>=
if ( mp_pen_p(q)!=null ) {
  sx=mp->tx; sy=mp->ty;
  mp->tx=0; mp->ty=0;
  mp_do_pen_trans(mp, mp_pen_p(q));
  if ( sqdet !=0 && ((mp_type(q)==mp_stroked_code)&&(mp_dash_p(q)!=null)) )
    dash_scale(q)=mp_take_scaled(mp, dash_scale(q),sqdet);
  if ( ! pen_is_elliptical(mp_pen_p(q)) )
    if ( sgndet<0 )
      mp_pen_p(q)=mp_make_pen(mp, mp_copy_path(mp, mp_pen_p(q)),true); 
         /* this unreverses the pen */
  mp->tx=sx; mp->ty=sy;
}

@ This uses the fact that transformations are stored in the order
|(tx,ty,txx,txy,tyx,tyy)|.
@^data structure assumptions@>

@<Transform the compact transformation starting at |r|@>=
mp_trans(mp, r,r+1);
sx=mp->tx; sy=mp->ty;
mp->tx=0; mp->ty=0;
mp_trans(mp, r+2,r+4);
mp_trans(mp, r+3,r+5);
mp->tx=sx; mp->ty=sy

@ The hard cases of transformation occur when big nodes are involved,
and when some of their components are unknown.

@<Declare binary action...@>=
@<Declare subroutines needed by |big_trans|@>
static void mp_big_trans (MP mp,pointer p, quarterword c) {
  pointer q,r,pp,qq; /* list manipulation registers */
  quarterword s; /* size of a big node */
  s=mp->big_node_size[mp_type(p)]; q=value(p); r=q+s;
  do {  
    r=r-2;
    if ( mp_type(r)!=mp_known ) {
      @<Transform an unknown big node and |return|@>;
    }
  } while (r!=q);
  @<Transform a known big node@>;
} /* node |p| will now be recycled by |do_binary| */

@ @<Transform an unknown big node and |return|@>=
{ 
  mp_set_up_known_trans(mp, c); mp_make_exp_copy(mp, p); 
  r=value(mp->cur_exp);
  if ( mp->cur_type==mp_transform_type ) {
    mp_bilin1(mp, yy_part_loc(r),mp->tyy,xy_part_loc(q),mp->tyx,0);
    mp_bilin1(mp, yx_part_loc(r),mp->tyy,xx_part_loc(q),mp->tyx,0);
    mp_bilin1(mp, xy_part_loc(r),mp->txx,yy_part_loc(q),mp->txy,0);
    mp_bilin1(mp, xx_part_loc(r),mp->txx,yx_part_loc(q),mp->txy,0);
  }
  mp_bilin1(mp, y_part_loc(r),mp->tyy,x_part_loc(q),mp->tyx,mp->ty);
  mp_bilin1(mp, x_part_loc(r),mp->txx,y_part_loc(q),mp->txy,mp->tx);
  return;
}

@ Let |p| point to a two-word value field inside a big node of |cur_exp|,
and let |q| point to a another value field. The |bilin1| procedure
replaces |p| by $p\cdot t+q\cdot u+\delta$.

@<Declare subroutines needed by |big_trans|@>=
static void mp_bilin1 (MP mp, pointer p, scaled t, pointer q, 
                scaled u, scaled delta) {
  pointer r; /* list traverser */
  if ( t!=unity ) mp_dep_mult(mp, p,t,true);
  if ( u!=0 ) {
    if ( mp_type(q)==mp_known ) {
      delta+=mp_take_scaled(mp, value(q),u);
    } else { 
      @<Ensure that |type(p)=mp_proto_dependent|@>;
      dep_list(p)=mp_p_plus_fq(mp, dep_list(p),u,dep_list(q),
                               mp_proto_dependent,mp_type(q));
    }
  }
  if ( mp_type(p)==mp_known ) {
    value(p)+=delta;
  } else {
    r=dep_list(p);
    while ( mp_info(r)!=null ) r=mp_link(r);
    delta+=value(r);
    if ( r!=dep_list(p) ) value(r)=delta;
    else { mp_recycle_value(mp, p); mp_type(p)=mp_known; value(p)=delta; };
  }
  if ( mp->fix_needed ) mp_fix_dependencies(mp);
}

@ @<Ensure that |type(p)=mp_proto_dependent|@>=
if ( mp_type(p)!=mp_proto_dependent ) {
  if ( mp_type(p)==mp_known ) 
    mp_new_dep(mp, p,mp_const_dependency(mp, value(p)));
  else 
    dep_list(p)=mp_p_times_v(mp, dep_list(p),unity,mp_dependent,
                             mp_proto_dependent,true);
  mp_type(p)=mp_proto_dependent;
}

@ @<Transform a known big node@>=
mp_set_up_trans(mp, c);
if ( mp->cur_type==mp_known ) {
  @<Transform known by known@>;
} else { 
  pp=mp_stash_cur_exp(mp); qq=value(pp);
  mp_make_exp_copy(mp, p); r=value(mp->cur_exp);
  if ( mp->cur_type==mp_transform_type ) {
    mp_bilin2(mp, yy_part_loc(r),yy_part_loc(qq),
      value(xy_part_loc(q)),yx_part_loc(qq),null);
    mp_bilin2(mp, yx_part_loc(r),yy_part_loc(qq),
      value(xx_part_loc(q)),yx_part_loc(qq),null);
    mp_bilin2(mp, xy_part_loc(r),xx_part_loc(qq),
      value(yy_part_loc(q)),xy_part_loc(qq),null);
    mp_bilin2(mp, xx_part_loc(r),xx_part_loc(qq),
      value(yx_part_loc(q)),xy_part_loc(qq),null);
  };
  mp_bilin2(mp, y_part_loc(r),yy_part_loc(qq),
    value(x_part_loc(q)),yx_part_loc(qq),y_part_loc(qq));
  mp_bilin2(mp, x_part_loc(r),xx_part_loc(qq),
    value(y_part_loc(q)),xy_part_loc(qq),x_part_loc(qq));
  mp_recycle_value(mp, pp); mp_free_node(mp, pp,value_node_size);
}

@ Let |p| be a |mp_proto_dependent| value whose dependency list ends
at |dep_final|. The following procedure adds |v| times another
numeric quantity to~|p|.

@<Declare subroutines needed by |big_trans|@>=
static void mp_add_mult_dep (MP mp,pointer p, scaled v, pointer r) { 
  if ( mp_type(r)==mp_known ) {
    value(mp->dep_final)+=mp_take_scaled(mp, value(r),v);
  } else  { 
    dep_list(p)=mp_p_plus_fq(mp, dep_list(p),v,dep_list(r),
							 mp_proto_dependent,mp_type(r));
    if ( mp->fix_needed ) mp_fix_dependencies(mp);
  }
}

@ The |bilin2| procedure is something like |bilin1|, but with known
and unknown quantities reversed. Parameter |p| points to a value field
within the big node for |cur_exp|; and |type(p)=mp_known|. Parameters
|t| and~|u| point to value fields elsewhere; so does parameter~|q|,
unless it is |null| (which stands for zero). Location~|p| will be
replaced by $p\cdot t+v\cdot u+q$.

@<Declare subroutines needed by |big_trans|@>=
static void mp_bilin2 (MP mp,pointer p, pointer t, scaled v, 
                pointer u, pointer q) {
  scaled vv; /* temporary storage for |value(p)| */
  vv=value(p); mp_type(p)=mp_proto_dependent;
  mp_new_dep(mp, p,mp_const_dependency(mp, 0)); /* this sets |dep_final| */
  if ( vv!=0 ) 
    mp_add_mult_dep(mp, p,vv,t); /* |dep_final| doesn't change */
  if ( v!=0 ) mp_add_mult_dep(mp, p,v,u);
  if ( q!=null ) mp_add_mult_dep(mp, p,unity,q);
  if ( dep_list(p)==mp->dep_final ) {
    vv=value(mp->dep_final); mp_recycle_value(mp, p);
    mp_type(p)=mp_known; value(p)=vv;
  }
}

@ @<Transform known by known@>=
{ 
  mp_make_exp_copy(mp, p); r=value(mp->cur_exp);
  if ( mp->cur_type==mp_transform_type ) {
    mp_bilin3(mp, yy_part_loc(r),mp->tyy,value(xy_part_loc(q)),mp->tyx,0);
    mp_bilin3(mp, yx_part_loc(r),mp->tyy,value(xx_part_loc(q)),mp->tyx,0);
    mp_bilin3(mp, xy_part_loc(r),mp->txx,value(yy_part_loc(q)),mp->txy,0);
    mp_bilin3(mp, xx_part_loc(r),mp->txx,value(yx_part_loc(q)),mp->txy,0);
  }
  mp_bilin3(mp, y_part_loc(r),mp->tyy,value(x_part_loc(q)),mp->tyx,mp->ty);
  mp_bilin3(mp, x_part_loc(r),mp->txx,value(y_part_loc(q)),mp->txy,mp->tx);
}

@ Finally, in |bilin3| everything is |known|.

@<Declare subroutines needed by |big_trans|@>=
static void mp_bilin3 (MP mp,pointer p, scaled t, 
               scaled v, scaled u, scaled delta) { 
  if ( t!=unity )
    delta+=mp_take_scaled(mp, value(p),t);
  else 
    delta+=value(p);
  if ( u!=0 ) value(p)=delta+mp_take_scaled(mp, v,u);
  else value(p)=delta;
}

@ @<Additional cases of binary operators@>=
case concatenate: 
  if ( (mp->cur_type==mp_string_type)&&(mp_type(p)==mp_string_type) ) mp_cat(mp, p);
  else mp_bad_binary(mp, p,concatenate);
  break;
case substring_of: 
  if ( mp_nice_pair(mp, p,mp_type(p))&&(mp->cur_type==mp_string_type) )
    mp_chop_string(mp, value(p));
  else mp_bad_binary(mp, p,substring_of);
  break;
case subpath_of: 
  if ( mp->cur_type==mp_pair_type ) mp_pair_to_path(mp);
  if ( mp_nice_pair(mp, p,mp_type(p))&&(mp->cur_type==mp_path_type) )
    mp_chop_path(mp, value(p));
  else mp_bad_binary(mp, p,subpath_of);
  break;

@ @<Declare binary action...@>=
static void mp_cat (MP mp,pointer p) {
  str_number a,b; /* the strings being concatenated */
  integer k; /* run length */
  integer needed;
  a=value(p); b=mp->cur_exp; k=length(a);
  needed=mp->pool_ptr+k+length(b);
  /* this will free some memory, hopefully */
  if (mp->pool_ptr>(11*mp->old_pool_size)/10) {
      mp->old_pool_size = mp->pool_ptr;
      mp_do_compaction(mp, mp->pool_size);
  }
  if ( needed > mp->max_pool_ptr ) {
    if ( needed > mp->pool_size ) {
      mp_reallocate_pool(mp,needed);
    }
    mp->max_pool_ptr=needed; 
  }
  (void)memcpy(mp->str_pool+mp->pool_ptr, mp->str_pool+mp->str_start[a],(size_t)k);
  mp->pool_ptr+=k; 
  k=length(b);
  (void)memcpy(mp->str_pool+mp->pool_ptr, mp->str_pool+mp->str_start[b],(size_t)k);
  mp->pool_ptr+=k;
  mp->cur_exp=mp_make_string(mp); delete_str_ref(b);
}

@ @<Declare binary action...@>=
static void mp_chop_string (MP mp,pointer p) {
  integer a, b; /* start and stop points */
  integer l; /* length of the original string */
  integer k; /* runs from |a| to |b| */
  str_number s; /* the original string */
  boolean reversed; /* was |a>b|? */
  a=mp_round_unscaled(mp, value(x_part_loc(p)));
  b=mp_round_unscaled(mp, value(y_part_loc(p)));
  if ( a<=b ) reversed=false;
  else  { reversed=true; k=a; a=b; b=k; };
  s=mp->cur_exp; l=length(s);
  if ( a<0 ) { 
    a=0;
    if ( b<0 ) b=0;
  }
  if ( b>l ) { 
    b=l;
    if ( a>l ) a=l;
  }
  str_room(b-a);
  if ( reversed ) {
    for (k=mp->str_start[s]+b-1;k>=mp->str_start[s]+a;k--)  {
      append_char(mp->str_pool[k]);
    }
  } else  {
    for (k=mp->str_start[s]+a;k<mp->str_start[s]+b;k++)  {
      append_char(mp->str_pool[k]);
    }
  }
  mp->cur_exp=mp_make_string(mp); delete_str_ref(s);
}

@ @<Declare binary action...@>=
static void mp_chop_path (MP mp,pointer p) {
  pointer q; /* a knot in the original path */
  pointer pp,qq,rr,ss; /* link variables for copies of path nodes */
  scaled a,b,k,l; /* indices for chopping */
  boolean reversed; /* was |a>b|? */
  l=mp_path_length(mp); a=value(x_part_loc(p)); b=value(y_part_loc(p));
  if ( a<=b ) reversed=false;
  else  { reversed=true; k=a; a=b; b=k; };
  @<Dispense with the cases |a<0| and/or |b>l|@>;
  q=mp->cur_exp;
  while ( a>=unity ) {
    q=mp_link(q); a=a-unity; b=b-unity;
  }
  if ( b==a ) {
    @<Construct a path from |pp| to |qq| of length zero@>; 
  } else { 
    @<Construct a path from |pp| to |qq| of length $\lceil b\rceil$@>; 
  }
  mp_left_type(pp)=mp_endpoint; mp_right_type(qq)=mp_endpoint; mp_link(qq)=pp;
  mp_toss_knot_list(mp, mp->cur_exp);
  if ( reversed ) {
    mp->cur_exp=mp_link(mp_htap_ypoc(mp, pp)); mp_toss_knot_list(mp, pp);
  } else {
    mp->cur_exp=pp;
  }
}

@ @<Dispense with the cases |a<0| and/or |b>l|@>=
if ( a<0 ) {
  if ( mp_left_type(mp->cur_exp)==mp_endpoint ) {
    a=0; if ( b<0 ) b=0;
  } else  {
    do {  a=a+l; b=b+l; } while (a<0); /* a cycle always has length |l>0| */
  }
}
if ( b>l ) {
  if ( mp_left_type(mp->cur_exp)==mp_endpoint ) {
    b=l; if ( a>l ) a=l;
  } else {
    while ( a>=l ) { 
      a=a-l; b=b-l;
    }
  }
}

@ @<Construct a path from |pp| to |qq| of length $\lceil b\rceil$@>=
{ 
  pp=mp_copy_knot(mp, q); qq=pp;
  do {  
    q=mp_link(q); rr=qq; qq=mp_copy_knot(mp, q); mp_link(rr)=qq; b=b-unity;
  } while (b>0);
  if ( a>0 ) {
    ss=pp; pp=mp_link(pp);
    mp_split_cubic(mp, ss,a*010000); pp=mp_link(ss);
    mp_free_node(mp, ss,knot_node_size);
    if ( rr==ss ) {
      b=mp_make_scaled(mp, b,unity-a); rr=pp;
    }
  }
  if ( b<0 ) {
    mp_split_cubic(mp, rr,(b+unity)*010000);
    mp_free_node(mp, qq,knot_node_size);
    qq=mp_link(rr);
  }
}

@ @<Construct a path from |pp| to |qq| of length zero@>=
{ 
  if ( a>0 ) { mp_split_cubic(mp, q,a*010000); q=mp_link(q); };
  pp=mp_copy_knot(mp, q); qq=pp;
}

@ @<Additional cases of binary operators@>=
case point_of: case precontrol_of: case postcontrol_of: 
  if ( mp->cur_type==mp_pair_type )
     mp_pair_to_path(mp);
  if ( (mp->cur_type==mp_path_type)&&(mp_type(p)==mp_known) )
    mp_find_point(mp, value(p), (quarterword)c);
  else 
    mp_bad_binary(mp, p, (quarterword)c);
  break;
case pen_offset_of: 
  if ( (mp->cur_type==mp_pen_type)&& mp_nice_pair(mp, p,mp_type(p)) )
    mp_set_up_offset(mp, value(p));
  else 
    mp_bad_binary(mp, p,pen_offset_of);
  break;
case direction_time_of: 
  if ( mp->cur_type==mp_pair_type ) mp_pair_to_path(mp);
  if ( (mp->cur_type==mp_path_type)&& mp_nice_pair(mp, p,mp_type(p)) )
    mp_set_up_direction_time(mp, value(p));
  else 
    mp_bad_binary(mp, p,direction_time_of);
  break;
case envelope_of:
  if ( (mp_type(p) != mp_pen_type) || (mp->cur_type != mp_path_type) )
    mp_bad_binary(mp, p,envelope_of);
  else
    mp_set_up_envelope(mp, p);
  break;
case glyph_infont:
  if ( (mp_type(p) != mp_string_type &&
        mp_type(p) != mp_known) || (mp->cur_type != mp_string_type) )
    mp_bad_binary(mp, p,glyph_infont);
  else
    mp_set_up_glyph_infont(mp, p);
  break;
  break;

@ @<Declare binary action...@>=
static void mp_set_up_offset (MP mp,pointer p) { 
  mp_find_offset(mp, value(x_part_loc(p)),value(y_part_loc(p)),mp->cur_exp);
  mp_pair_value(mp, mp->cur_x,mp->cur_y);
}
static void mp_set_up_direction_time (MP mp,pointer p) { 
  mp_flush_cur_exp(mp, mp_find_direction_time(mp, value(x_part_loc(p)),
  value(y_part_loc(p)),mp->cur_exp));
}
static void mp_set_up_envelope (MP mp,pointer p) {
  quarterword ljoin, lcap;
  scaled miterlim;
  pointer q = mp_copy_path(mp, mp->cur_exp); /* the original path */
  /* TODO: accept elliptical pens for straight paths */
  if (pen_is_elliptical(value(p))) {
    mp_bad_envelope_pen(mp);
    mp->cur_exp = q;
    mp->cur_type = mp_path_type;
    return;
  }
  if ( mp->internal[mp_linejoin]>unity ) ljoin=2;
  else if ( mp->internal[mp_linejoin]>0 ) ljoin=1;
  else ljoin=0;
  if ( mp->internal[mp_linecap]>unity ) lcap=2;
  else if ( mp->internal[mp_linecap]>0 ) lcap=1;
  else lcap=0;
  if ( mp->internal[mp_miterlimit]<unity )
    miterlim=unity;
  else
    miterlim=mp->internal[mp_miterlimit];
  mp->cur_exp = mp_make_envelope(mp, q, value(p), ljoin,lcap,miterlim);
  mp->cur_type = mp_path_type;
}


@ This is pretty straightfoward. The one silly thing is that
the output of |mp_ps_do_font_charstring| has to be un-exported.

@<Declare binary action...@>=
static void mp_set_up_glyph_infont (MP mp, pointer p) {
  mp_edge_object *h = NULL;
  mp_ps_font *f = NULL;
  char *n = mp_str(mp, mp->cur_exp);
  f = mp_ps_font_parse(mp, (int)mp_find_font(mp, n));
  if (f!=NULL) {
    if (mp_type(p) == mp_known) {
      int v = mp_round_unscaled(mp,value(p));
      if (v<0 || v>255) {
        print_err ("glyph index too high (");
        mp_print_int(mp, v);
        mp_print(mp,")");
        mp_error(mp);
      } else {
        h = mp_ps_font_charstring (mp,f,v);
      }
    } else {
      n = mp_str(mp, value(p));
      h = mp_ps_do_font_charstring (mp,f,n);
      free(n);
    }
    mp_ps_font_free(mp,f);
  }
  if (h!=NULL) {
    mp->cur_exp=mp_gr_unexport(mp, h); 
  } else {
    mp->cur_exp=mp_get_node(mp, edge_header_size); 
    mp_init_edges(mp, mp->cur_exp); 
  }
  mp->cur_type=mp_picture_type;
}

@ @<Declare binary action...@>=
static void mp_find_point (MP mp,scaled v, quarterword c) {
  pointer p; /* the path */
  scaled n; /* its length */
  p=mp->cur_exp;
  if ( mp_left_type(p)==mp_endpoint ) n=-unity; else n=0;
  do {  p=mp_link(p); n=n+unity; } while (p!=mp->cur_exp);
  if ( n==0 ) { 
    v=0; 
  } else if ( v<0 ) {
    if ( mp_left_type(p)==mp_endpoint ) v=0;
    else v=n-1-((-v-1) % n);
  } else if ( v>n ) {
    if ( mp_left_type(p)==mp_endpoint ) v=n;
    else v=v % n;
  }
  p=mp->cur_exp;
  while ( v>=unity ) { p=mp_link(p); v=v-unity;  };
  if ( v!=0 ) {
     @<Insert a fractional node by splitting the cubic@>;
  }
  @<Set the current expression to the desired path coordinates@>;
}

@ @<Insert a fractional node...@>=
{ mp_split_cubic(mp, p,v*010000); p=mp_link(p); }

@ @<Set the current expression to the desired path coordinates...@>=
switch (c) {
case point_of: 
  mp_pair_value(mp, mp_x_coord(p),mp_y_coord(p));
  break;
case precontrol_of: 
  if ( mp_left_type(p)==mp_endpoint ) mp_pair_value(mp, mp_x_coord(p),mp_y_coord(p));
  else mp_pair_value(mp, mp_left_x(p),mp_left_y(p));
  break;
case postcontrol_of: 
  if ( mp_right_type(p)==mp_endpoint ) mp_pair_value(mp, mp_x_coord(p),mp_y_coord(p));
  else mp_pair_value(mp, mp_right_x(p),mp_right_y(p));
  break;
} /* there are no other cases */

@ @<Additional cases of binary operators@>=
case arc_time_of: 
  if ( mp->cur_type==mp_pair_type )
     mp_pair_to_path(mp);
  if ( (mp->cur_type==mp_path_type)&&(mp_type(p)==mp_known) )
    mp_flush_cur_exp(mp, mp_get_arc_time(mp, mp->cur_exp,value(p)));
  else 
    mp_bad_binary(mp, p, (quarterword)c);
  break;

@ @<Additional cases of bin...@>=
case intersect: 
  if ( mp_type(p)==mp_pair_type ) {
    q=mp_stash_cur_exp(mp); mp_unstash_cur_exp(mp, p);
    mp_pair_to_path(mp); p=mp_stash_cur_exp(mp); mp_unstash_cur_exp(mp, q);
  };
  if ( mp->cur_type==mp_pair_type ) mp_pair_to_path(mp);
  if ( (mp->cur_type==mp_path_type)&&(mp_type(p)==mp_path_type) ) {
    mp_path_intersection(mp, value(p),mp->cur_exp);
    mp_pair_value(mp, mp->cur_t,mp->cur_tt);
  } else {
    mp_bad_binary(mp, p,intersect);
  }
  break;

@ @<Additional cases of bin...@>=
case in_font:
  if ( (mp->cur_type!=mp_string_type)||mp_type(p)!=mp_string_type) 
    mp_bad_binary(mp, p,in_font);
  else { mp_do_infont(mp, p); binary_return; }
  break;

@ Function |new_text_node| owns the reference count for its second argument
(the text string) but not its first (the font name).

@<Declare binary action...@>=
static void mp_do_infont (MP mp,pointer p) {
  pointer q;
  q=mp_get_node(mp, edge_header_size);
  mp_init_edges(mp, q);
  mp_link(obj_tail(q))=mp_new_text_node(mp,str(mp->cur_exp),value(p));
  obj_tail(q)=mp_link(obj_tail(q));
  mp_free_node(mp, p,value_node_size);
  mp_flush_cur_exp(mp, q);
  mp->cur_type=mp_picture_type;
}

@* \[40] Statements and commands.
The chief executive of \MP\ is the |do_statement| routine, which
contains the master switch that causes all the various pieces of \MP\
to do their things, in the right order.

In a sense, this is the grand climax of the program: It applies all the
tools that we have worked so hard to construct. In another sense, this is
the messiest part of the program: It necessarily refers to other pieces
of code all over the place, so that a person can't fully understand what is
going on without paging back and forth to be reminded of conventions that
are defined elsewhere. We are now at the hub of the web.

The structure of |do_statement| itself is quite simple.  The first token
of the statement is fetched using |get_x_next|.  If it can be the first
token of an expression, we look for an equation, an assignment, or a
title. Otherwise we use a \&{case} construction to branch at high speed to
the appropriate routine for various and sundry other types of commands,
each of which has an ``action procedure'' that does the necessary work.

The program uses the fact that
$$\hbox{|min_primary_command=max_statement_command=type_name|}$$
to interpret a statement that starts with, e.g., `\&{string}',
as a type declaration rather than a boolean expression.

@c void mp_do_statement (MP mp) { /* governs \MP's activities */
  mp->cur_type=mp_vacuous; mp_get_x_next(mp);
  if ( mp->cur_cmd>max_primary_command ) {
    @<Worry about bad statement@>;
  } else if ( mp->cur_cmd>max_statement_command ) {
    @<Do an equation, assignment, title, or
     `$\langle\,$expression$\,\rangle\,$\&{endgroup}'@>;
  } else {
    @<Do a statement that doesn't begin with an expression@>;
  }
  if ( mp->cur_cmd<semicolon )
    @<Flush unparsable junk that was found after the statement@>;
  mp->error_count=0;
}

@ @<Declarations@>=
@<Declare action procedures for use by |do_statement|@>

@ The only command codes |>max_primary_command| that can be present
at the beginning of a statement are |semicolon| and higher; these
occur when the statement is null.

@<Worry about bad statement@>=
{ 
  if ( mp->cur_cmd<semicolon ) {
    print_err("A statement can't begin with `");
@.A statement can't begin with x@>
    mp_print_cmd_mod(mp, mp->cur_cmd,mp->cur_mod); mp_print_char(mp, xord('\''));
    help5("I was looking for the beginning of a new statement.",
      "If you just proceed without changing anything, I'll ignore",
      "everything up to the next `;'. Please insert a semicolon",
      "now in front of anything that you don't want me to delete.",
      "(See Chapter 27 of The METAFONTbook for an example.)");
@:METAFONTbook}{\sl The {\logos METAFONT\/}book@>
    mp_back_error(mp); mp_get_x_next(mp);
  }
}

@ The help message printed here says that everything is flushed up to
a semicolon, but actually the commands |end_group| and |stop| will
also terminate a statement.

@<Flush unparsable junk that was found after the statement@>=
{ 
  print_err("Extra tokens will be flushed");
@.Extra tokens will be flushed@>
  help6("I've just read as much of that statement as I could fathom,",
        "so a semicolon should have been next. It's very puzzling...",
        "but I'll try to get myself back together, by ignoring",
        "everything up to the next `;'. Please insert a semicolon",
        "now in front of anything that you don't want me to delete.",
        "(See Chapter 27 of The METAFONTbook for an example.)");
@:METAFONTbook}{\sl The {\logos METAFONT\/}book@>
  mp_back_error(mp); mp->scanner_status=flushing;
  do {  
    get_t_next;
    @<Decrease the string reference count...@>;
  } while (! end_of_statement); /* |cur_cmd=semicolon|, |end_group|, or |stop| */
  mp->scanner_status=normal;
}

@ If |do_statement| ends with |cur_cmd=end_group|, we should have
|cur_type=mp_vacuous| unless the statement was simply an expression;
in the latter case, |cur_type| and |cur_exp| should represent that
expression.

@<Do a statement that doesn't...@>=
{ 
  if ( mp->internal[mp_tracing_commands]>0 ) 
    show_cur_cmd_mod;
  switch (mp->cur_cmd ) {
  case type_name:mp_do_type_declaration(mp); break;
  case macro_def:
    if ( mp->cur_mod>var_def ) mp_make_op_def(mp);
    else if ( mp->cur_mod>end_def ) mp_scan_def(mp);
     break;
  @<Cases of |do_statement| that invoke particular commands@>;
  } /* there are no other cases */
  mp->cur_type=mp_vacuous;
}

@ The most important statements begin with expressions.

@<Do an equation, assignment, title, or...@>=
{ 
  mp->var_flag=assignment; mp_scan_expression(mp);
  if ( mp->cur_cmd<end_group ) {
    if ( mp->cur_cmd==equals ) mp_do_equation(mp);
    else if ( mp->cur_cmd==assignment ) mp_do_assignment(mp);
    else if ( mp->cur_type==mp_string_type ) {@<Do a title@> ; }
    else if ( mp->cur_type!=mp_vacuous ){ 
      exp_err("Isolated expression");
@.Isolated expression@>
      help3("I couldn't find an `=' or `:=' after the",
        "expression that is shown above this error message,",
        "so I guess I'll just ignore it and carry on.");
      mp_put_get_error(mp);
    }
    mp_flush_cur_exp(mp, 0); mp->cur_type=mp_vacuous;
  }
}

@ @<Do a title@>=
{ 
  if ( mp->internal[mp_tracing_titles]>0 ) {
    mp_print_nl(mp, "");  mp_print_str(mp, mp->cur_exp); update_terminal;
  }
}

@ Equations and assignments are performed by the pair of mutually recursive
@^recursion@>
routines |do_equation| and |do_assignment|. These routines are called when
|cur_cmd=equals| and when |cur_cmd=assignment|, respectively; the left-hand
side is in |cur_type| and |cur_exp|, while the right-hand side is yet
to be scanned. After the routines are finished, |cur_type| and |cur_exp|
will be equal to the right-hand side (which will normally be equal
to the left-hand side).

@<Declarations@>=
@<Declare the procedure called |make_eq|@>
static void mp_do_equation (MP mp) ;

@ @c
void mp_do_equation (MP mp) {
  pointer lhs; /* capsule for the left-hand side */
  pointer p; /* temporary register */
  lhs=mp_stash_cur_exp(mp); mp_get_x_next(mp); 
  mp->var_flag=assignment; mp_scan_expression(mp);
  if ( mp->cur_cmd==equals ) mp_do_equation(mp);
  else if ( mp->cur_cmd==assignment ) mp_do_assignment(mp);
  if ( mp->internal[mp_tracing_commands]>two ) 
    @<Trace the current equation@>;
  if ( mp->cur_type==mp_unknown_path ) if ( mp_type(lhs)==mp_pair_type ) {
    p=mp_stash_cur_exp(mp); mp_unstash_cur_exp(mp, lhs); lhs=p;
  }; /* in this case |make_eq| will change the pair to a path */
  mp_make_eq(mp, lhs); /* equate |lhs| to |(cur_type,cur_exp)| */
}

@ And |do_assignment| is similar to |do_equation|:

@<Declarations@>=
static void mp_do_assignment (MP mp);

@ @c
void mp_do_assignment (MP mp) {
  pointer lhs; /* token list for the left-hand side */
  pointer p; /* where the left-hand value is stored */
  pointer q; /* temporary capsule for the right-hand value */
  if ( mp->cur_type!=mp_token_list ) { 
    exp_err("Improper `:=' will be changed to `='");
@.Improper `:='@>
    help2("I didn't find a variable name at the left of the `:=',",
          "so I'm going to pretend that you said `=' instead.");
    mp_error(mp); mp_do_equation(mp);
  } else { 
    lhs=mp->cur_exp; mp->cur_type=mp_vacuous;
    mp_get_x_next(mp); mp->var_flag=assignment; mp_scan_expression(mp);
    if ( mp->cur_cmd==equals ) mp_do_equation(mp);
    else if ( mp->cur_cmd==assignment ) mp_do_assignment(mp);
    if ( mp->internal[mp_tracing_commands]>two ) 
      @<Trace the current assignment@>;
    if ( mp_info(lhs)>hash_end ) {
      @<Assign the current expression to an internal variable@>;
    } else  {
      @<Assign the current expression to the variable |lhs|@>;
    }
    mp_flush_node_list(mp, lhs);
  }
}

@ @<Trace the current equation@>=
{ 
  mp_begin_diagnostic(mp); mp_print_nl(mp, "{("); mp_print_exp(mp,lhs,0);
  mp_print(mp,")=("); mp_print_exp(mp,null,0); 
  mp_print(mp,")}"); mp_end_diagnostic(mp, false);
}

@ @<Trace the current assignment@>=
{ 
  mp_begin_diagnostic(mp); mp_print_nl(mp, "{");
  if ( mp_info(lhs)>hash_end ) 
     mp_print(mp, mp->int_name[mp_info(lhs)-(hash_end)]);
  else 
     mp_show_token_list(mp, lhs,null,1000,0);
  mp_print(mp, ":="); mp_print_exp(mp, null,0); 
  mp_print_char(mp, xord('}')); mp_end_diagnostic(mp, false);
}

@ @<Assign the current expression to an internal variable@>=
if ( mp->cur_type==mp_known || mp->cur_type==mp_string_type )  {
  if (mp->cur_type==mp_string_type) {
    if (mp->int_type[mp_info(lhs)-(hash_end)]!=mp->cur_type) {
       exp_err("Internal quantity `");
@.Internal quantity...@>
       mp_print(mp, mp->int_name[mp_info(lhs)-(hash_end)]);
       mp_print(mp, "' must receive a known numeric value");
       help2("I can\'t set this internal quantity to anything but a known",
             "numeric value, so I'll have to ignore this assignment.");
      mp_put_get_error(mp);
    } else {
      add_str_ref(mp->cur_exp);
      mp->internal[mp_info(lhs)-(hash_end)]=mp->cur_exp;
    }
  } else { /* mp_known */
    if (mp->int_type[mp_info(lhs)-(hash_end)]!=mp->cur_type) {
       exp_err("Internal quantity `");
@.Internal quantity...@>
       mp_print(mp, mp->int_name[mp_info(lhs)-(hash_end)]);
       mp_print(mp, "' must receive a known string");
       help2("I can\'t set this internal quantity to anything but a known",
             "string, so I'll have to ignore this assignment.");
      mp_put_get_error(mp);
    } else {
      mp->internal[mp_info(lhs)-(hash_end)]=mp->cur_exp;
    }
  }
} else { 
  exp_err("Internal quantity `");
@.Internal quantity...@>
  mp_print(mp, mp->int_name[mp_info(lhs)-(hash_end)]);
  mp_print(mp, "' must receive a known numeric or string");
  help2("I can\'t set an internal quantity to anything but a known string",
        "or known numeric value, so I'll have to ignore this assignment.");
  mp_put_get_error(mp);
}

@ @<Assign the current expression to the variable |lhs|@>=
{ 
  p=mp_find_variable(mp, lhs);
  if ( p!=null ) {
    q=mp_stash_cur_exp(mp); mp->cur_type=mp_und_type(mp, p); 
    mp_recycle_value(mp, p);
    mp_type(p)=mp->cur_type; value(p)=null; mp_make_exp_copy(mp, p);
    p=mp_stash_cur_exp(mp); mp_unstash_cur_exp(mp, q); mp_make_eq(mp, p);
  } else  { 
    mp_obliterated(mp, lhs); mp_put_get_error(mp);
  }
}


@ And now we get to the nitty-gritty. The |make_eq| procedure is given
a pointer to a capsule that is to be equated to the current expression.

@<Declare the procedure called |make_eq|@>=
static void mp_make_eq (MP mp,pointer lhs) ;


@ 
@c void mp_make_eq (MP mp,pointer lhs) {
  quarterword t; /* type of the left-hand side */
  pointer p,q; /* pointers inside of big nodes */
  integer v=0; /* value of the left-hand side */
RESTART: 
  t=mp_type(lhs);
  if ( t<=mp_pair_type ) v=value(lhs);
  switch (t) {
  @<For each type |t|, make an equation and |goto done| unless |cur_type|
    is incompatible with~|t|@>;
  } /* all cases have been listed */
  @<Announce that the equation cannot be performed@>;
DONE:
  check_arith; mp_recycle_value(mp, lhs); 
  mp_free_node(mp, lhs,value_node_size);
}

@ @<Announce that the equation cannot be performed@>=
mp_disp_err(mp, lhs,""); 
exp_err("Equation cannot be performed (");
@.Equation cannot be performed@>
if ( mp_type(lhs)<=mp_pair_type ) mp_print_type(mp, mp_type(lhs));
else mp_print(mp, "numeric");
mp_print_char(mp, xord('='));
if ( mp->cur_type<=mp_pair_type ) mp_print_type(mp, mp->cur_type);
else mp_print(mp, "numeric");
mp_print_char(mp, xord(')'));
help2("I'm sorry, but I don't know how to make such things equal.",
      "(See the two expressions just above the error message.)");
mp_put_get_error(mp)

@ @<For each type |t|, make an equation and |goto done| unless...@>=
case mp_boolean_type: case mp_string_type: case mp_pen_type:
case mp_path_type: case mp_picture_type:
  if ( mp->cur_type==t+unknown_tag ) { 
    mp_nonlinear_eq(mp, v,mp->cur_exp,false); 
    mp_unstash_cur_exp(mp, mp->cur_exp); goto DONE;
  } else if ( mp->cur_type==t ) {
    @<Report redundant or inconsistent equation and |goto done|@>;
  }
  break;
case unknown_types:
  if ( mp->cur_type==t-unknown_tag ) { 
    mp_nonlinear_eq(mp, mp->cur_exp,lhs,true); goto DONE;
  } else if ( mp->cur_type==t ) { 
    mp_ring_merge(mp, lhs,mp->cur_exp); goto DONE;
  } else if ( mp->cur_type==mp_pair_type ) {
    if ( t==mp_unknown_path ) { 
     mp_pair_to_path(mp); goto RESTART;
    };
  }
  break;
case mp_transform_type: case mp_color_type:
case mp_cmykcolor_type: case mp_pair_type:
  if ( mp->cur_type==t ) {
    @<Do multiple equations and |goto done|@>;
  }
  break;
case mp_known: case mp_dependent:
case mp_proto_dependent: case mp_independent:
  if ( mp->cur_type>=mp_known ) { 
    mp_try_eq(mp, lhs,null); goto DONE;
  };
  break;
case mp_vacuous:
  break;

@ @<Report redundant or inconsistent equation and |goto done|@>=
{ 
  if ( mp->cur_type<=mp_string_type ) {
    if ( mp->cur_type==mp_string_type ) {
      if ( mp_str_vs_str(mp, v,mp->cur_exp)!=0 ) {
        goto NOT_FOUND;
      }
    } else if ( v!=mp->cur_exp ) {
      goto NOT_FOUND;
    }
    @<Exclaim about a redundant equation@>; goto DONE;
  }
  print_err("Redundant or inconsistent equation");
@.Redundant or inconsistent equation@>
  help2("An equation between already-known quantities can't help.",
        "But don't worry; continue and I'll just ignore it.");
  mp_put_get_error(mp); goto DONE;
NOT_FOUND: 
  print_err("Inconsistent equation");
@.Inconsistent equation@>
  help2("The equation I just read contradicts what was said before.",
        "But don't worry; continue and I'll just ignore it.");
  mp_put_get_error(mp); goto DONE;
}

@ @<Do multiple equations and |goto done|@>=
{ 
  p=v+mp->big_node_size[t]; 
  q=value(mp->cur_exp)+mp->big_node_size[t];
  do {  
    p=p-2; q=q-2; mp_try_eq(mp, p,q);
  } while (p!=v);
  goto DONE;
}

@ The first argument to |try_eq| is the location of a value node
in a capsule that will soon be recycled. The second argument is
either a location within a pair or transform node pointed to by
|cur_exp|, or it is |null| (which means that |cur_exp| itself
serves as the second argument). The idea is to leave |cur_exp| unchanged,
but to equate the two operands.

@<Declarations@>=
static void mp_try_eq (MP mp,pointer l, pointer r) ;

@ 
@c void mp_try_eq (MP mp,pointer l, pointer r) {
  pointer p; /* dependency list for right operand minus left operand */
  int t; /* the type of list |p| */
  pointer q; /* the constant term of |p| is here */
  pointer pp; /* dependency list for right operand */
  int tt; /* the type of list |pp| */
  boolean copied; /* have we copied a list that ought to be recycled? */
  @<Remove the left operand from its container, negate it, and
    put it into dependency list~|p| with constant term~|q|@>;
  @<Add the right operand to list |p|@>;
  if ( mp_info(p)==null ) {
    @<Deal with redundant or inconsistent equation@>;
  } else { 
    mp_linear_eq(mp, p, (quarterword)t);
    if ( r==null ) if ( mp->cur_type!=mp_known ) {
      if ( mp_type(mp->cur_exp)==mp_known ) {
        pp=mp->cur_exp; mp->cur_exp=value(mp->cur_exp); mp->cur_type=mp_known;
        mp_free_node(mp, pp,value_node_size);
      }
    }
  }
}

@ @<Remove the left operand from its container, negate it, and...@>=
t=mp_type(l);
if ( t==mp_known ) { 
  t=mp_dependent; p=mp_const_dependency(mp, -value(l)); q=p;
} else if ( t==mp_independent ) {
  t=mp_dependent; p=mp_single_dependency(mp, l); negate(value(p));
  q=mp->dep_final;
} else { 
  p=dep_list(l); q=p;
  while (1) { 
    negate(value(q));
    if ( mp_info(q)==null ) break;
    q=mp_link(q);
  }
  mp_link(prev_dep(l))=mp_link(q); prev_dep(mp_link(q))=prev_dep(l);
  mp_type(l)=mp_known;
}

@ @<Deal with redundant or inconsistent equation@>=
{ 
  if ( abs(value(p))>64 ) { /* off by .001 or more */
    print_err("Inconsistent equation");
@.Inconsistent equation@>
    mp_print(mp, " (off by "); mp_print_scaled(mp, value(p)); 
    mp_print_char(mp, xord(')'));
    help2("The equation I just read contradicts what was said before.",
          "But don't worry; continue and I'll just ignore it.");
    mp_put_get_error(mp);
  } else if ( r==null ) {
    @<Exclaim about a redundant equation@>;
  }
  mp_free_node(mp, p,dep_node_size);
}

@ @<Add the right operand to list |p|@>=
if ( r==null ) {
  if ( mp->cur_type==mp_known ) {
    value(q)=value(q)+mp->cur_exp; goto DONE1;
  } else { 
    tt=mp->cur_type;
    if ( tt==mp_independent ) pp=mp_single_dependency(mp, mp->cur_exp);
    else pp=dep_list(mp->cur_exp);
  } 
} else {
  if ( mp_type(r)==mp_known ) {
    value(q)=value(q)+value(r); goto DONE1;
  } else { 
    tt=mp_type(r);
    if ( tt==mp_independent ) pp=mp_single_dependency(mp, r);
    else pp=dep_list(r);
  }
}
if ( tt!=mp_independent ) copied=false;
else  { copied=true; tt=mp_dependent; };
@<Add dependency list |pp| of type |tt| to dependency list~|p| of type~|t|@>;
if ( copied ) mp_flush_node_list(mp, pp);
DONE1:

@ @<Add dependency list |pp| of type |tt| to dependency list~|p| of type~|t|@>=
mp->watch_coefs=false;
if ( t==tt ) {
  p=mp_p_plus_q(mp, p,pp, (quarterword)t);
} else if ( t==mp_proto_dependent ) {
  p=mp_p_plus_fq(mp, p,unity,pp,mp_proto_dependent,mp_dependent);
} else { 
  q=p;
  while ( mp_info(q)!=null ) {
    value(q)=mp_round_fraction(mp, value(q)); q=mp_link(q);
  }
  t=mp_proto_dependent; p=mp_p_plus_q(mp, p, pp, (quarterword)t);
}
mp->watch_coefs=true;

@ Our next goal is to process type declarations. For this purpose it's
convenient to have a procedure that scans a $\langle\,$declared
variable$\,\rangle$ and returns the corresponding token list. After the
following procedure has acted, the token after the declared variable
will have been scanned, so it will appear in |cur_cmd|, |cur_mod|,
and~|cur_sym|.

@<Declarations@>=
static pointer mp_scan_declared_variable (MP mp) ;

@ @c
pointer mp_scan_declared_variable (MP mp) {
  pointer x; /* hash address of the variable's root */
  pointer h,t; /* head and tail of the token list to be returned */
  pointer l; /* hash address of left bracket */
  mp_get_symbol(mp); x=mp->cur_sym;
  if ( mp->cur_cmd!=tag_token ) mp_clear_symbol(mp, x,false);
  h=mp_get_avail(mp); mp_info(h)=x; t=h;
  while (1) { 
    mp_get_x_next(mp);
    if ( mp->cur_sym==0 ) break;
    if ( mp->cur_cmd!=tag_token ) if ( mp->cur_cmd!=internal_quantity)  {
      if ( mp->cur_cmd==left_bracket ) {
        @<Descend past a collective subscript@>;
      } else {
        break;
      }
    }
    mp_link(t)=mp_get_avail(mp); t=mp_link(t); mp_info(t)=mp->cur_sym;
  }
  if ( (eq_type(x)%outer_tag)!=tag_token ) mp_clear_symbol(mp, x,false);
  if ( equiv(x)==null ) mp_new_root(mp, x);
  return h;
}

@ If the subscript isn't collective, we don't accept it as part of the
declared variable.

@<Descend past a collective subscript@>=
{ 
  l=mp->cur_sym; mp_get_x_next(mp);
  if ( mp->cur_cmd!=right_bracket ) {
    mp_back_input(mp); mp->cur_sym=l; mp->cur_cmd=left_bracket; break;
  } else {
    mp->cur_sym=collective_subscript;
  }
}

@ Type declarations are introduced by the following primitive operations.

@<Put each...@>=
mp_primitive(mp, "numeric",type_name,mp_numeric_type);
@:numeric_}{\&{numeric} primitive@>
mp_primitive(mp, "string",type_name,mp_string_type);
@:string_}{\&{string} primitive@>
mp_primitive(mp, "boolean",type_name,mp_boolean_type);
@:boolean_}{\&{boolean} primitive@>
mp_primitive(mp, "path",type_name,mp_path_type);
@:path_}{\&{path} primitive@>
mp_primitive(mp, "pen",type_name,mp_pen_type);
@:pen_}{\&{pen} primitive@>
mp_primitive(mp, "picture",type_name,mp_picture_type);
@:picture_}{\&{picture} primitive@>
mp_primitive(mp, "transform",type_name,mp_transform_type);
@:transform_}{\&{transform} primitive@>
mp_primitive(mp, "color",type_name,mp_color_type);
@:color_}{\&{color} primitive@>
mp_primitive(mp, "rgbcolor",type_name,mp_color_type);
@:color_}{\&{rgbcolor} primitive@>
mp_primitive(mp, "cmykcolor",type_name,mp_cmykcolor_type);
@:color_}{\&{cmykcolor} primitive@>
mp_primitive(mp, "pair",type_name,mp_pair_type);
@:pair_}{\&{pair} primitive@>

@ @<Cases of |print_cmd...@>=
case type_name: mp_print_type(mp, (quarterword)m); break;

@ Now we are ready to handle type declarations, assuming that a
|type_name| has just been scanned.

@<Declare action procedures for use by |do_statement|@>=
static void mp_do_type_declaration (MP mp) ;

@ @c
void mp_do_type_declaration (MP mp) {
  quarterword t; /* the type being declared */
  pointer p; /* token list for a declared variable */
  pointer q; /* value node for the variable */
  if ( mp->cur_mod>=mp_transform_type ) 
    t=(quarterword)mp->cur_mod;
  else 
    t=(quarterword)(mp->cur_mod+unknown_tag);
  do {  
    p=mp_scan_declared_variable(mp);
    mp_flush_variable(mp, equiv(mp_info(p)),mp_link(p),false);
    q=mp_find_variable(mp, p);
    if ( q!=null ) { 
      mp_type(q)=t; value(q)=null; 
    } else  { 
      print_err("Declared variable conflicts with previous vardef");
@.Declared variable conflicts...@>
      help2("You can't use, e.g., `numeric foo[]' after `vardef foo'.",
            "Proceed, and I'll ignore the illegal redeclaration.");
      mp_put_get_error(mp);
    }
    mp_flush_list(mp, p);
    if ( mp->cur_cmd<comma ) {
      @<Flush spurious symbols after the declared variable@>;
    }
  } while (! end_of_statement);
}

@ @<Flush spurious symbols after the declared variable@>=
{ 
  print_err("Illegal suffix of declared variable will be flushed");
@.Illegal suffix...flushed@>
  help5("Variables in declarations must consist entirely of",
    "names and collective subscripts, e.g., `x[]a'.",
    "Are you trying to use a reserved word in a variable name?",
    "I'm going to discard the junk I found here,",
    "up to the next comma or the end of the declaration.");
  if ( mp->cur_cmd==numeric_token )
    mp->help_line[2]="Explicit subscripts like `x15a' aren't permitted.";
  mp_put_get_error(mp); mp->scanner_status=flushing;
  do {  
    get_t_next;
    @<Decrease the string reference count...@>;
  } while (mp->cur_cmd<comma); /* either |end_of_statement| or |cur_cmd=comma| */
  mp->scanner_status=normal;
}

@ \MP's |main_control| procedure just calls |do_statement| repeatedly
until coming to the end of the user's program.
Each execution of |do_statement| concludes with
|cur_cmd=semicolon|, |end_group|, or |stop|.

@c 
static void mp_main_control (MP mp) { 
  do {  
    mp_do_statement(mp);
    if ( mp->cur_cmd==end_group ) {
      print_err("Extra `endgroup'");
@.Extra `endgroup'@>
      help2("I'm not currently working on a `begingroup',",
            "so I had better not try to end anything.");
      mp_flush_error(mp, 0);
    }
  } while (mp->cur_cmd!=stop);
}
int mp_run (MP mp) {
  if (mp->history < mp_fatal_error_stop ) {
    xfree(mp->jump_buf);
    mp->jump_buf = malloc(sizeof(jmp_buf));
    if (mp->jump_buf == NULL || setjmp(*(mp->jump_buf)) != 0) 
      return mp->history;
    mp_main_control(mp); /* come to life */
    mp_final_cleanup(mp); /* prepare for death */
    mp_close_files_and_terminate(mp);
  }
  return mp->history;
}

@ This function allows setting of internals from an external
source (like the command line or a controlling application).

It accepts two |char *|'s, even for numeric assignments when
it calls |atoi| to get an integer from the start of the string.

@c
void mp_set_internal (MP mp, char *n, char *v, int isstring) {
  size_t l = strlen(n);
  char err[256];
  const char *errid = NULL;
  if (l>0) {
    integer h = mp_compute_hash(mp, n, (int)l);
    pointer p = h+hash_base; /* we start searching here */
    while (true) { 
      if (text(p)>0 && length(text(p))==(int)l && 
	  mp_str_eq_cstr(mp, text(p),n)) {
        if (eq_type(p)==internal_quantity) {
     	  if ((mp->int_type[equiv(p)]==mp_string_type) && (isstring)) {
            mp->internal[equiv(p)] = mp_rts(mp,v);
          } else if ((mp->int_type[equiv(p)]==mp_known) && (!isstring)) {
            scaled test = (scaled)atoi(v);
            if (test>16383 ) {
               errid = "value is too large";
            } else if (test<-16383) {
               errid = "value is too small";
            } else {
               mp->internal[equiv(p)] =  test*unity;
            }
          } else {
            errid = "value has the wrong type";
          }
        } else {
          errid = "variable is not an internal";
        }
        break;
      }
      if ( mp_next(p)==0 ) {
        errid = "variable does not exist";
        break;
      }
      p=mp_next(p);
    }
  }
  if (errid != NULL) {
    if (isstring) {
      mp_snprintf(err,256,"%s=\"%s\": %s, assignment ignored.",n,v, errid);
    } else {
      mp_snprintf(err,256,"%s=%d: %s, assignment ignored.",n,atoi(v),errid);
    }
    mp_warn(mp,err);
  }
}

@ @<Exported function headers@>=
void mp_set_internal (MP mp, char *n, char *v, int isstring);

@ For |mp_execute|, we need to define a structure to store the
redirected input and output. This structure holds the five relevant
streams: the three informational output streams, the PostScript
generation stream, and the input stream. These streams have many
things in common, so it makes sense to give them their own structure
definition. 

\item{fptr} is a virtual file pointer
\item{data} is the data this stream holds
\item{cur}  is a cursor pointing into |data| 
\item{size} is the allocated length of the data stream
\item{used} is the actual length of the data stream

There are small differences between input and output: |term_in| never
uses |used|, whereas the other four never use |cur|.

@<Exported types@>= 
typedef struct {
   void * fptr;
   char * data;
   char * cur;
   size_t size;
   size_t used;
} mp_stream;

typedef struct {
    mp_stream term_out;
    mp_stream error_out;
    mp_stream log_out;
    mp_stream ps_out;
    mp_stream term_in;
    struct mp_edge_object *edges;
} mp_run_data;

@ We need a function to clear an output stream, this is called at the
beginning of |mp_execute|. We also need one for destroying an output
stream, this is called just before a stream is (re)opened.

@c
static void mp_reset_stream(mp_stream *str) {
   xfree(str->data); 
   str->cur = NULL;
   str->size = 0; 
   str->used = 0;
}
static void mp_free_stream(mp_stream *str) {
   xfree(str->fptr); 
   mp_reset_stream(str);
}

@ @<Declarations@>=
static void mp_reset_stream(mp_stream *str);
static void mp_free_stream(mp_stream *str);

@ The global instance contains a pointer instead of the actual structure
even though it is essentially static, because that makes it is easier to move 
the object around.

@<Global ...@>=
mp_run_data run_data;

@ Another type is needed: the indirection will overload some of the
file pointer objects in the instance (but not all). For clarity, an
indirect object is used that wraps a |FILE *|.

@<Types ... @>=
typedef struct File {
    FILE *f;
} File;

@ Here are all of the functions that need to be overloaded for |mp_execute|.

@<Declarations@>=
static void *mplib_open_file(MP mp, const char *fname, const char *fmode, int ftype);
static int mplib_get_char(void *f, mp_run_data * mplib_data);
static void mplib_unget_char(void *f, mp_run_data * mplib_data, int c);
static char *mplib_read_ascii_file(MP mp, void *ff, size_t * size);
static void mplib_write_ascii_file(MP mp, void *ff, const char *s);
static void mplib_read_binary_file(MP mp, void *ff, void **data, size_t * size);
static void mplib_write_binary_file(MP mp, void *ff, void *s, size_t size);
static void mplib_close_file(MP mp, void *ff);
static int mplib_eof_file(MP mp, void *ff);
static void mplib_flush_file(MP mp, void *ff);
static void mplib_shipout_backend(MP mp, int h);

@ The |xmalloc(1,1)| calls make sure the stored indirection values are unique.

@d reset_stream(a)  do { 
        mp_reset_stream(&(a));
        if (!ff->f) {
          ff->f = xmalloc(1,1);
          (a).fptr = ff->f;
        } } while (0)

@c

static void *mplib_open_file(MP mp, const char *fname, const char *fmode, int ftype)
{
    File *ff = xmalloc(1, sizeof(File));
    mp_run_data *run = mp_rundata(mp);
    ff->f = NULL;
    if (ftype == mp_filetype_terminal) {
        if (fmode[0] == 'r') {
            if (!ff->f) {
              ff->f = xmalloc(1,1);
              run->term_in.fptr = ff->f;
            }
        } else {
            reset_stream(run->term_out);
        }
    } else if (ftype == mp_filetype_error) {
        reset_stream(run->error_out);
    } else if (ftype == mp_filetype_log) {
        reset_stream(run->log_out);
    } else if (ftype == mp_filetype_postscript) {
        mp_free_stream(&(run->ps_out));
        ff->f = xmalloc(1,1);
        run->ps_out.fptr = ff->f;
    } else {
        char realmode[3];
        char *f = (mp->find_file)(mp, fname, fmode, ftype);
        if (f == NULL)
            return NULL;
        realmode[0] = *fmode;
        realmode[1] = 'b';
        realmode[2] = 0;
        ff->f = fopen(f, realmode);
        free(f);
        if ((fmode[0] == 'r') && (ff->f == NULL)) {
            free(ff);
            return NULL;
        }
    }
    return ff;
}

static int mplib_get_char(void *f, mp_run_data * run)
{
    int c;
    if (f == run->term_in.fptr && run->term_in.data != NULL) {
        if (run->term_in.size == 0) {
            if (run->term_in.cur  != NULL) {
                run->term_in.cur = NULL;
            } else {
                xfree(run->term_in.data);
            }
            c = EOF;
        } else {
            run->term_in.size--;
            c = *(run->term_in.cur)++;
        }
    } else {
        c = fgetc(f);
    }
    return c;
}

static void mplib_unget_char(void *f, mp_run_data * run, int c)
{
    if (f == run->term_in.fptr && run->term_in.cur != NULL) {
        run->term_in.size++;
        run->term_in.cur--;
    } else {
        ungetc(c, f);
    }
}


static char *mplib_read_ascii_file(MP mp, void *ff, size_t * size)
{
    char *s = NULL;
    if (ff != NULL) {
        int c;
        size_t len = 0, lim = 128;
        mp_run_data *run = mp_rundata(mp);
        FILE *f = ((File *) ff)->f;
        if (f == NULL)
            return NULL;
        *size = 0;
        c = mplib_get_char(f, run);
        if (c == EOF)
            return NULL;
        s = malloc(lim);
        if (s == NULL)
            return NULL;
        while (c != EOF && c != '\n' && c != '\r') {
            if (len >= (lim-1)) {
                s = xrealloc(s, (lim + (lim >> 2)),1);
                if (s == NULL)
                    return NULL;
                lim += (lim >> 2);
            }
            s[len++] = (char)c;
            c = mplib_get_char(f, run);
        }
        if (c == '\r') {
            c = mplib_get_char(f, run);
            if (c != EOF && c != '\n')
                mplib_unget_char(f, run, c);
        }
        s[len] = 0;
        *size = len;
    }
    return s;
}

static void mp_append_string (MP mp, mp_stream *a,const char *b) {
    size_t l = strlen(b);
    if ((a->used+l)>=a->size) {
        a->size += 256+(a->size)/5+l;
        a->data = xrealloc(a->data,a->size,1);
    }
    (void)strcpy(a->data+a->used,b);
    a->used += l;
}


static void mplib_write_ascii_file(MP mp, void *ff, const char *s)
{
    if (ff != NULL) {
        void *f = ((File *) ff)->f;
        mp_run_data *run = mp_rundata(mp);
        if (f != NULL) {
            if (f == run->term_out.fptr) {
                mp_append_string(mp,&(run->term_out), s);
            } else if (f == run->error_out.fptr) {
                mp_append_string(mp,&(run->error_out), s);
            } else if (f == run->log_out.fptr) {
                mp_append_string(mp,&(run->log_out), s);
            } else if (f == run->ps_out.fptr) {
                mp_append_string(mp,&(run->ps_out), s);
            } else {
                fprintf((FILE *) f, "%s", s);
            }
        }
    }
}

static void mplib_read_binary_file(MP mp, void *ff, void **data, size_t * size)
{
    (void) mp;
    if (ff != NULL) {
        size_t len = 0;
        FILE *f = ((File *) ff)->f;
        if (f != NULL)
            len = fread(*data, 1, *size, f);
        *size = len;
    }
}

static void mplib_write_binary_file(MP mp, void *ff, void *s, size_t size)
{
    (void) mp;
    if (ff != NULL) {
        FILE *f = ((File *) ff)->f;
        if (f != NULL)
            (void)fwrite(s, size, 1, f);
    }
}

static void mplib_close_file(MP mp, void *ff)
{
    if (ff != NULL) {
        mp_run_data *run = mp_rundata(mp);
        void *f = ((File *) ff)->f;
        if (f != NULL) {
          if (f != run->term_out.fptr
            && f != run->error_out.fptr
            && f != run->log_out.fptr
            && f != run->ps_out.fptr
            && f != run->term_in.fptr) {
            fclose(f);
          }
        }
        free(ff);
    }
}

static int mplib_eof_file(MP mp, void *ff)
{
    if (ff != NULL) {
        mp_run_data *run = mp_rundata(mp);
        FILE *f = ((File *) ff)->f;
        if (f == NULL)
            return 1;
        if (f == run->term_in.fptr && run->term_in.data != NULL) {
            return (run->term_in.size == 0);
        }
        return feof(f);
    }
    return 1;
}

static void mplib_flush_file(MP mp, void *ff)
{
    (void) mp;
    (void) ff;
    return;
}

static void mplib_shipout_backend(MP mp, int h)
{
    mp_edge_object *hh = mp_gr_export(mp, h);
    if (hh) {
        mp_run_data *run = mp_rundata(mp);
        if (run->edges==NULL) {
           run->edges = hh;
        } else {
           mp_edge_object *p = run->edges; 
           while (p->next!=NULL) { p = p->next; }
            p->next = hh;
        } 
    }
}


@ This is where we fill them all in.
@<Prepare function pointers for non-interactive use@>=
{
    mp->open_file         = mplib_open_file;
    mp->close_file        = mplib_close_file;
    mp->eof_file          = mplib_eof_file;
    mp->flush_file        = mplib_flush_file;
    mp->write_ascii_file  = mplib_write_ascii_file;
    mp->read_ascii_file   = mplib_read_ascii_file;
    mp->write_binary_file = mplib_write_binary_file;
    mp->read_binary_file  = mplib_read_binary_file;
    mp->shipout_backend   = mplib_shipout_backend;
}

@ Perhaps this is the most important API function in the library.

@<Exported function ...@>=
extern mp_run_data *mp_rundata (MP mp) ;

@ @c
mp_run_data *mp_rundata (MP mp)  {
  return &(mp->run_data);
}

@ @<Dealloc ...@>=
mp_free_stream(&(mp->run_data.term_in));
mp_free_stream(&(mp->run_data.term_out));
mp_free_stream(&(mp->run_data.log_out));
mp_free_stream(&(mp->run_data.error_out));
mp_free_stream(&(mp->run_data.ps_out));

@ @<Finish non-interactive use@>=
xfree(mp->term_out);
xfree(mp->term_in);
xfree(mp->err_out);

@ @<Start non-interactive work@>=
@<Initialize the output routines@>;
mp->input_ptr=0; mp->max_in_stack=0;
mp->in_open=0; mp->open_parens=0; mp->max_buf_stack=0;
mp->param_ptr=0; mp->max_param_stack=0;
start = loc = iindex = 0; mp->first = 0;
line=0; name=is_term;
mp->mpx_name[0]=absent;
mp->force_eof=false;
t_open_in; 
mp->scanner_status=normal;
if (mp->mem_ident==NULL) {
  if ( ! mp_load_mem_file(mp) ) {
    (mp->close_file)(mp, mp->mem_file); 
     mp->history  = mp_fatal_error_stop;
     return mp->history;
  }
  (mp->close_file)(mp, mp->mem_file);
}
mp_fix_date_and_time(mp);
if (mp->random_seed==0)
  mp->random_seed = (mp->internal[mp_time] / unity)+mp->internal[mp_day];
mp_init_randoms(mp, mp->random_seed);
@<Initialize the print |selector|...@>;
mp_open_log_file(mp);
mp_set_job_id(mp);
mp_init_map_file(mp, mp->troff_mode);
mp->history=mp_spotless; /* ready to go! */
if (mp->troff_mode) {
  mp->internal[mp_gtroffmode]=unity; 
  mp->internal[mp_prologues]=unity; 
}
@<Fix up |mp->internal[mp_job_name]|@>;
if ( mp->start_sym>0 ) { /* insert the `\&{everyjob}' symbol */
  mp->cur_sym=mp->start_sym; mp_back_input(mp);
}

@ @c
int mp_execute (MP mp, const char *s, size_t l) {
  mp_reset_stream(&(mp->run_data.term_out));
  mp_reset_stream(&(mp->run_data.log_out));
  mp_reset_stream(&(mp->run_data.error_out));
  mp_reset_stream(&(mp->run_data.ps_out));
  if (mp->finished) {
      return mp->history;
  } else if (!mp->noninteractive) {
      mp->history = mp_fatal_error_stop ;
      return mp->history;
  }
  if (mp->history < mp_fatal_error_stop ) {
    xfree(mp->jump_buf);
    mp->jump_buf = malloc(sizeof(jmp_buf));
    if (mp->jump_buf == NULL || setjmp(*(mp->jump_buf)) != 0) {   
       return mp->history; 
    }
    if (s==NULL) { /* this signals EOF */
      mp_final_cleanup(mp); /* prepare for death */
      mp_close_files_and_terminate(mp);
      return mp->history;
    } 
    mp->tally=0; 
    mp->term_offset=0; mp->file_offset=0; 
    /* Perhaps some sort of warning here when |data| is not 
     * yet exhausted would be nice ...  this happens after errors
     */
    if (mp->run_data.term_in.data)
      xfree(mp->run_data.term_in.data);
    mp->run_data.term_in.data = xstrdup(s);
    mp->run_data.term_in.cur = mp->run_data.term_in.data;
    mp->run_data.term_in.size = l;
    if (mp->run_state == 0) {
      mp->selector=term_only; 
      @<Start non-interactive work@>; 
    }
    mp->run_state =1;    
    (void)mp_input_ln(mp,mp->term_in);
    mp_firm_up_the_line(mp);	
    mp->buffer[limit]=xord('%');
    mp->first=(size_t)(limit+1); 
    loc=start;
	do {  
      mp_do_statement(mp);
    } while (mp->cur_cmd!=stop);
    mp_final_cleanup(mp); 
    mp_close_files_and_terminate(mp);
  }
  return mp->history;
}

@ This function cleans up
@c
int mp_finish (MP mp) {
  int history = 0;
  if (mp->finished || mp->history >= mp_fatal_error_stop) {
    history = mp->history;
    mp_free(mp);
    return history;
  }
  xfree(mp->jump_buf);
  mp->jump_buf = malloc(sizeof(jmp_buf));
  if (mp->jump_buf == NULL || setjmp(*(mp->jump_buf)) != 0) { 
    history = mp->history;
  } else {
    history = mp->history;
    mp_final_cleanup(mp); /* prepare for death */
  }
  mp_close_files_and_terminate(mp);
  mp_free(mp);
  return history;
}

@ People may want to know the library version
@c 
char * mp_metapost_version (void) {
  return mp_strdup(metapost_version);
}

@ @<Exported function headers@>=
int mp_run (MP mp);
int mp_execute (MP mp, const char *s, size_t l);
int mp_finish (MP mp);
char * mp_metapost_version (void);

@ @<Put each...@>=
mp_primitive(mp, "end",stop,0);
@:end_}{\&{end} primitive@>
mp_primitive(mp, "dump",stop,1);
@:dump_}{\&{dump} primitive@>

@ @<Cases of |print_cmd...@>=
case stop:
  if ( m==0 ) mp_print(mp, "end");
  else mp_print(mp, "dump");
  break;

@* \[41] Commands.
Let's turn now to statements that are classified as ``commands'' because
of their imperative nature. We'll begin with simple ones, so that it
will be clear how to hook command processing into the |do_statement| routine;
then we'll tackle the tougher commands.

Here's one of the simplest:

@<Cases of |do_statement|...@>=
case mp_random_seed: mp_do_random_seed(mp);  break;

@ @<Declare action procedures for use by |do_statement|@>=
static void mp_do_random_seed (MP mp) ;

@ @c void mp_do_random_seed (MP mp) { 
  mp_get_x_next(mp);
  if ( mp->cur_cmd!=assignment ) {
    mp_missing_err(mp, ":=");
@.Missing `:='@>
    help1("Always say `randomseed:=<numeric expression>'.");
    mp_back_error(mp);
  };
  mp_get_x_next(mp); mp_scan_expression(mp);
  if ( mp->cur_type!=mp_known ) {
    exp_err("Unknown value will be ignored");
@.Unknown value...ignored@>
    help2("Your expression was too random for me to handle,",
          "so I won't change the random seed just now.");
    mp_put_get_flush_error(mp, 0);
  } else {
   @<Initialize the random seed to |cur_exp|@>;
  }
}

@ @<Initialize the random seed to |cur_exp|@>=
{ 
  mp_init_randoms(mp, mp->cur_exp);
  if ( mp->selector>=log_only && mp->selector<write_file) {
    mp->old_setting=mp->selector; mp->selector=log_only;
    mp_print_nl(mp, "{randomseed:="); 
    mp_print_scaled(mp, mp->cur_exp); 
    mp_print_char(mp, xord('}'));
    mp_print_nl(mp, ""); mp->selector=mp->old_setting;
  }
}

@ And here's another simple one (somewhat different in flavor):

@<Cases of |do_statement|...@>=
case mode_command: 
  mp_print_ln(mp); mp->interaction=mp->cur_mod;
  @<Initialize the print |selector| based on |interaction|@>;
  if ( mp->log_opened ) mp->selector=mp->selector+2;
  mp_get_x_next(mp);
  break;

@ @<Put each...@>=
mp_primitive(mp, "batchmode",mode_command,mp_batch_mode);
@:mp_batch_mode_}{\&{batchmode} primitive@>
mp_primitive(mp, "nonstopmode",mode_command,mp_nonstop_mode);
@:mp_nonstop_mode_}{\&{nonstopmode} primitive@>
mp_primitive(mp, "scrollmode",mode_command,mp_scroll_mode);
@:mp_scroll_mode_}{\&{scrollmode} primitive@>
mp_primitive(mp, "errorstopmode",mode_command,mp_error_stop_mode);
@:mp_error_stop_mode_}{\&{errorstopmode} primitive@>

@ @<Cases of |print_cmd_mod|...@>=
case mode_command: 
  switch (m) {
  case mp_batch_mode: mp_print(mp, "batchmode"); break;
  case mp_nonstop_mode: mp_print(mp, "nonstopmode"); break;
  case mp_scroll_mode: mp_print(mp, "scrollmode"); break;
  default: mp_print(mp, "errorstopmode"); break;
  }
  break;

@ The `\&{inner}' and `\&{outer}' commands are only slightly harder.

@<Cases of |do_statement|...@>=
case protection_command: mp_do_protection(mp); break;

@ @<Put each...@>=
mp_primitive(mp, "inner",protection_command,0);
@:inner_}{\&{inner} primitive@>
mp_primitive(mp, "outer",protection_command,1);
@:outer_}{\&{outer} primitive@>

@ @<Cases of |print_cmd...@>=
case protection_command: 
  if ( m==0 ) mp_print(mp, "inner");
  else mp_print(mp, "outer");
  break;

@ @<Declare action procedures for use by |do_statement|@>=
static void mp_do_protection (MP mp) ;

@ @c void mp_do_protection (MP mp) {
  int m; /* 0 to unprotect, 1 to protect */
  halfword t; /* the |eq_type| before we change it */
  m=mp->cur_mod;
  do {  
    mp_get_symbol(mp); t=eq_type(mp->cur_sym);
    if ( m==0 ) { 
      if ( t>=outer_tag ) 
        eq_type(mp->cur_sym)=t-outer_tag;
    } else if ( t<outer_tag ) {
      eq_type(mp->cur_sym)=t+outer_tag;
    }
    mp_get_x_next(mp);
  } while (mp->cur_cmd==comma);
}

@ \MP\ never defines the tokens `\.(' and `\.)' to be primitives, but
plain \MP\ begins with the declaration `\&{delimiters} \.{()}'. Such a
declaration assigns the command code |left_delimiter| to `\.{(}' and
|right_delimiter| to `\.{)}'; the |equiv| of each delimiter is the
hash address of its mate.

@<Cases of |do_statement|...@>=
case delimiters: mp_def_delims(mp); break;

@ @<Declare action procedures for use by |do_statement|@>=
static void mp_def_delims (MP mp) ;

@ @c void mp_def_delims (MP mp) {
  pointer l_delim,r_delim; /* the new delimiter pair */
  mp_get_clear_symbol(mp); l_delim=mp->cur_sym;
  mp_get_clear_symbol(mp); r_delim=mp->cur_sym;
  eq_type(l_delim)=left_delimiter; equiv(l_delim)=r_delim;
  eq_type(r_delim)=right_delimiter; equiv(r_delim)=l_delim;
  mp_get_x_next(mp);
}

@ Here is a procedure that is called when \MP\ has reached a point
where some right delimiter is mandatory.

@<Declarations@>=
static void mp_check_delimiter (MP mp,pointer l_delim, pointer r_delim);

@ @c
void mp_check_delimiter (MP mp,pointer l_delim, pointer r_delim) {
  if ( mp->cur_cmd==right_delimiter ) 
    if ( mp->cur_mod==l_delim ) 
      return;
  if ( mp->cur_sym!=r_delim ) {
     mp_missing_err(mp, str(text(r_delim)));
@.Missing `)'@>
    help2("I found no right delimiter to match a left one. So I've",
          "put one in, behind the scenes; this may fix the problem.");
    mp_back_error(mp);
  } else { 
    print_err("The token `"); mp_print_text(r_delim);
@.The token...delimiter@>
    mp_print(mp, "' is no longer a right delimiter");
    help3("Strange: This token has lost its former meaning!",
      "I'll read it as a right delimiter this time;",
      "but watch out, I'll probably miss it later.");
    mp_error(mp);
  }
}

@ The next four commands save or change the values associated with tokens.

@<Cases of |do_statement|...@>=
case save_command: 
  do {  
    mp_get_symbol(mp); mp_save_variable(mp, mp->cur_sym); mp_get_x_next(mp);
  } while (mp->cur_cmd==comma);
  break;
case interim_command: mp_do_interim(mp); break;
case let_command: mp_do_let(mp); break;
case new_internal: mp_do_new_internal(mp); break;

@ @<Declare action procedures for use by |do_statement|@>=
static void mp_do_statement (MP mp);
static void mp_do_interim (MP mp);

@ @c void mp_do_interim (MP mp) { 
  mp_get_x_next(mp);
  if ( mp->cur_cmd!=internal_quantity ) {
     print_err("The token `");
@.The token...quantity@>
    if ( mp->cur_sym==0 ) mp_print(mp, "(%CAPSULE)");
    else mp_print_text(mp->cur_sym);
    mp_print(mp, "' isn't an internal quantity");
    help1("Something like `tracingonline' should follow `interim'.");
    mp_back_error(mp);
  } else { 
    mp_save_internal(mp, mp->cur_mod); mp_back_input(mp);
  }
  mp_do_statement(mp);
}

@ The following procedure is careful not to undefine the left-hand symbol
too soon, lest commands like `{\tt let x=x}' have a surprising effect.

@<Declare action procedures for use by |do_statement|@>=
static void mp_do_let (MP mp) ;

@ @c void mp_do_let (MP mp) {
  pointer l; /* hash location of the left-hand symbol */
  mp_get_symbol(mp); l=mp->cur_sym; mp_get_x_next(mp);
  if ( mp->cur_cmd!=equals ) if ( mp->cur_cmd!=assignment ) {
     mp_missing_err(mp, "=");
@.Missing `='@>
    help3("You should have said `let symbol = something'.",
      "But don't worry; I'll pretend that an equals sign",
      "was present. The next token I read will be `something'.");
    mp_back_error(mp);
  }
  mp_get_symbol(mp);
  switch (mp->cur_cmd) {
  case defined_macro: case secondary_primary_macro:
  case tertiary_secondary_macro: case expression_tertiary_macro: 
    add_mac_ref(mp->cur_mod);
    break;
  default: 
    break;
  }
  mp_clear_symbol(mp, l,false); eq_type(l)=mp->cur_cmd;
  if ( mp->cur_cmd==tag_token ) equiv(l)=null;
  else equiv(l)=mp->cur_mod;
  mp_get_x_next(mp);
}

@ @<Declarations@>=
static void mp_do_new_internal (MP mp) ;

@ @<Internal library ...@>=
void mp_grow_internals (MP mp, int l);

@ @c
void mp_grow_internals (MP mp, int l) {
  scaled *internal;
  char * *int_name; 
  int    *int_type; 
  int k;
  if ( hash_end+l>max_halfword ) {
    mp_confusion(mp, "out of memory space"); /* can't be reached */
  }
  int_name = xmalloc ((l+1),sizeof(char *));
  int_type = xmalloc ((l+1),sizeof(int));
  internal = xmalloc ((l+1),sizeof(scaled));
  for (k=0;k<=l; k++ ) { 
    if (k<=mp->max_internal) {
      internal[k]=mp->internal[k]; 
      int_name[k]=mp->int_name[k]; 
      int_type[k]=mp->int_type[k]; 
    } else {
      internal[k]=0; 
      int_name[k]=NULL; 
      int_type[k]=0; 
    }
  }
  xfree(mp->internal); xfree(mp->int_name); xfree(mp->int_type);
  mp->int_type = int_type;
  mp->int_name = int_name;
  mp->internal = internal;
  mp->max_internal = l;
}

void mp_do_new_internal (MP mp) { 
  int the_type = mp_known;
  mp_get_x_next(mp);
  if (mp->cur_cmd==type_name && mp->cur_mod==mp_string_type) {
     the_type = mp_string_type;
  } else {
     if (!(mp->cur_cmd==type_name && mp->cur_mod==mp_known)) {
        mp_back_input(mp);
     }
  }
  do {  
    if ( mp->int_ptr==mp->max_internal ) {
      mp_grow_internals(mp, (mp->max_internal + (mp->max_internal/4)));
    }
    mp_get_clear_symbol(mp); incr(mp->int_ptr);
    eq_type(mp->cur_sym)=internal_quantity; 
    equiv(mp->cur_sym)=mp->int_ptr;
    if(mp->int_name[mp->int_ptr]!=NULL)
      xfree(mp->int_name[mp->int_ptr]);
    mp->int_name[mp->int_ptr]=str(text(mp->cur_sym)); 
    if (the_type==mp_string_type) {
      mp->internal[mp->int_ptr]=null_str;
    } else {
      mp->internal[mp->int_ptr]=0;
    }
    mp->int_type[mp->int_ptr]=the_type;
    mp_get_x_next(mp);
  } while (mp->cur_cmd==comma);
}

@ @<Dealloc variables@>=
for (k=0;k<=mp->max_internal;k++) {
   xfree(mp->int_name[k]);
}
xfree(mp->internal); 
xfree(mp->int_name); 
xfree(mp->int_type); 


@ The various `\&{show}' commands are distinguished by modifier fields
in the usual way.

@d show_token_code 0 /* show the meaning of a single token */
@d show_stats_code 1 /* show current memory and string usage */
@d show_code 2 /* show a list of expressions */
@d show_var_code 3 /* show a variable and its descendents */
@d show_dependencies_code 4 /* show dependent variables in terms of independents */

@<Put each...@>=
mp_primitive(mp, "showtoken",show_command,show_token_code);
@:show_token_}{\&{showtoken} primitive@>
mp_primitive(mp, "showstats",show_command,show_stats_code);
@:show_stats_}{\&{showstats} primitive@>
mp_primitive(mp, "show",show_command,show_code);
@:show_}{\&{show} primitive@>
mp_primitive(mp, "showvariable",show_command,show_var_code);
@:show_var_}{\&{showvariable} primitive@>
mp_primitive(mp, "showdependencies",show_command,show_dependencies_code);
@:show_dependencies_}{\&{showdependencies} primitive@>

@ @<Cases of |print_cmd...@>=
case show_command: 
  switch (m) {
  case show_token_code:mp_print(mp, "showtoken"); break;
  case show_stats_code:mp_print(mp, "showstats"); break;
  case show_code:mp_print(mp, "show"); break;
  case show_var_code:mp_print(mp, "showvariable"); break;
  default: mp_print(mp, "showdependencies"); break;
  }
  break;

@ @<Cases of |do_statement|...@>=
case show_command:mp_do_show_whatever(mp); break;

@ The value of |cur_mod| controls the |verbosity| in the |print_exp| routine:
if it's |show_code|, complicated structures are abbreviated, otherwise
they aren't.

@<Declare action procedures for use by |do_statement|@>=
static void mp_do_show (MP mp) ;

@ @c void mp_do_show (MP mp) { 
  do {  
    mp_get_x_next(mp); mp_scan_expression(mp);
    mp_print_nl(mp, ">> ");
@.>>@>
    mp_print_exp(mp, null,2); mp_flush_cur_exp(mp, 0);
  } while (mp->cur_cmd==comma);
}

@ @<Declare action procedures for use by |do_statement|@>=
static void mp_disp_token (MP mp) ;

@ @c void mp_disp_token (MP mp) { 
  mp_print_nl(mp, "> ");
@.>\relax@>
  if ( mp->cur_sym==0 ) {
    @<Show a numeric or string or capsule token@>;
  } else { 
    mp_print_text(mp->cur_sym); mp_print_char(mp, xord('='));
    if ( eq_type(mp->cur_sym)>=outer_tag ) mp_print(mp, "(outer) ");
    mp_print_cmd_mod(mp, mp->cur_cmd,mp->cur_mod);
    if ( mp->cur_cmd==defined_macro ) {
      mp_print_ln(mp); mp_show_macro(mp, mp->cur_mod,null,100000);
    } /* this avoids recursion between |show_macro| and |print_cmd_mod| */
@^recursion@>
  }
}

@ @<Show a numeric or string or capsule token@>=
{ 
  if ( mp->cur_cmd==numeric_token ) {
    mp_print_scaled(mp, mp->cur_mod);
  } else if ( mp->cur_cmd==capsule_token ) {
    mp_print_capsule(mp,mp->cur_mod);
  } else  { 
    mp_print_char(mp, xord('"')); 
    mp_print_str(mp, mp->cur_mod); mp_print_char(mp, xord('"'));
    delete_str_ref(mp->cur_mod);
  }
}

@ The following cases of |print_cmd_mod| might arise in connection
with |disp_token|, although they don't necessarily correspond to
primitive tokens.

@<Cases of |print_cmd_...@>=
case left_delimiter:
case right_delimiter: 
  if ( c==left_delimiter ) mp_print(mp, "left");
  else mp_print(mp, "right");
  mp_print(mp, " delimiter that matches "); 
  mp_print_text(m);
  break;
case tag_token:
  if ( m==null ) mp_print(mp, "tag");
   else mp_print(mp, "variable");
   break;
case defined_macro: 
   mp_print(mp, "macro:");
   break;
case secondary_primary_macro:
case tertiary_secondary_macro:
case expression_tertiary_macro:
  mp_print_cmd_mod(mp, macro_def,c); 
  mp_print(mp, "'d macro:");
  mp_print_ln(mp); mp_show_token_list(mp, mp_link(mp_link(m)),null,1000,0);
  break;
case repeat_loop:
  mp_print(mp, "[repeat the loop]");
  break;
case internal_quantity:
  mp_print(mp, mp->int_name[m]);
  break;

@ @<Declare action procedures for use by |do_statement|@>=
static void mp_do_show_token (MP mp) ;

@ @c void mp_do_show_token (MP mp) { 
  do {  
    get_t_next; mp_disp_token(mp);
    mp_get_x_next(mp);
  } while (mp->cur_cmd==comma);
}

@ @<Declare action procedures for use by |do_statement|@>=
static void mp_do_show_stats (MP mp) ;

@ @c void mp_do_show_stats (MP mp) { 
  mp_print_nl(mp, "Memory usage ");
@.Memory usage...@>
  mp_print_int(mp, mp->var_used); mp_print_char(mp, xord('&')); mp_print_int(mp, mp->dyn_used);
  mp_print(mp, " ("); mp_print_int(mp, mp->hi_mem_min-mp->lo_mem_max-1);
  mp_print(mp, " still untouched)"); mp_print_ln(mp);
  mp_print_nl(mp, "String usage ");
  mp_print_int(mp, mp->strs_in_use-mp->init_str_use);
  mp_print_char(mp, xord('&')); mp_print_int(mp, mp->pool_in_use-mp->init_pool_ptr);
  mp_print(mp, " (");
  mp_print_int(mp, mp->max_strings-1-mp->strs_used_up); mp_print_char(mp, xord('&'));
  mp_print_int(mp, mp->pool_size-mp->pool_ptr); 
  mp_print(mp, " now untouched)"); mp_print_ln(mp);
  mp_get_x_next(mp);
}

@ Here's a recursive procedure that gives an abbreviated account
of a variable, for use by |do_show_var|.

@<Declare action procedures for use by |do_statement|@>=
static void mp_disp_var (MP mp,pointer p) ;

@ @c void mp_disp_var (MP mp,pointer p) {
  pointer q; /* traverses attributes and subscripts */
  int n; /* amount of macro text to show */
  if ( mp_type(p)==mp_structured )  {
    @<Descend the structure@>;
  } else if ( mp_type(p)>=mp_unsuffixed_macro ) {
    @<Display a variable macro@>;
  } else if ( mp_type(p)!=undefined ){ 
    mp_print_nl(mp, ""); mp_print_variable_name(mp, p); 
    mp_print_char(mp, xord('='));
    mp_print_exp(mp, p,0);
  }
}

@ @<Descend the structure@>=
{ 
  q=attr_head(p);
  do {  mp_disp_var(mp, q); q=mp_link(q); } while (q!=end_attr);
  q=subscr_head(p);
  while ( mp_name_type(q)==mp_subscr ) { 
    mp_disp_var(mp, q); q=mp_link(q);
  }
}

@ @<Display a variable macro@>=
{ 
  mp_print_nl(mp, ""); mp_print_variable_name(mp, p);
  if ( mp_type(p)>mp_unsuffixed_macro ) 
    mp_print(mp, "@@#"); /* |suffixed_macro| */
  mp_print(mp, "=macro:");
  if ( (int)mp->file_offset>=mp->max_print_line-20 ) n=5;
  else n=mp->max_print_line-(int)mp->file_offset-15;
  mp_show_macro(mp, value(p),null,n);
}

@ @<Declare action procedures for use by |do_statement|@>=
static void mp_do_show_var (MP mp) ;

@ @c void mp_do_show_var (MP mp) { 
  do {  
    get_t_next;
    if ( mp->cur_sym>0 ) if ( mp->cur_sym<=hash_end )
      if ( mp->cur_cmd==tag_token ) if ( mp->cur_mod!=null ) {
      mp_disp_var(mp, mp->cur_mod); goto DONE;
    }
   mp_disp_token(mp);
  DONE:
   mp_get_x_next(mp);
  } while (mp->cur_cmd==comma);
}

@ @<Declare action procedures for use by |do_statement|@>=
static void mp_do_show_dependencies (MP mp) ;

@ @c void mp_do_show_dependencies (MP mp) {
  pointer p; /* link that runs through all dependencies */
  p=mp_link(dep_head);
  while ( p!=dep_head ) {
    if ( mp_interesting(mp, p) ) {
      mp_print_nl(mp, ""); mp_print_variable_name(mp, p);
      if ( mp_type(p)==mp_dependent ) mp_print_char(mp, xord('='));
      else mp_print(mp, " = "); /* extra spaces imply proto-dependency */
      mp_print_dependency(mp, dep_list(p),mp_type(p));
    }
    p=dep_list(p);
    while ( mp_info(p)!=null ) p=mp_link(p);
    p=mp_link(p);
  }
  mp_get_x_next(mp);
}

@ Finally we are ready for the procedure that governs all of the
show commands.

@<Declare action procedures for use by |do_statement|@>=
static void mp_do_show_whatever (MP mp) ;

@ @c void mp_do_show_whatever (MP mp) { 
  if ( mp->interaction==mp_error_stop_mode ) wake_up_terminal;
  switch (mp->cur_mod) {
  case show_token_code:mp_do_show_token(mp); break;
  case show_stats_code:mp_do_show_stats(mp); break;
  case show_code:mp_do_show(mp); break;
  case show_var_code:mp_do_show_var(mp); break;
  case show_dependencies_code:mp_do_show_dependencies(mp); break;
  } /* there are no other cases */
  if ( mp->internal[mp_showstopping]>0 ){ 
    print_err("OK");
@.OK@>
    if ( mp->interaction<mp_error_stop_mode ) { 
      help0; decr(mp->error_count);
    } else {
      help1("This isn't an error message; I'm just showing something.");
    }
    if ( mp->cur_cmd==semicolon ) mp_error(mp);
     else mp_put_get_error(mp);
  }
}

@ The `\&{addto}' command needs the following additional primitives:

@d double_path_code 0 /* command modifier for `\&{doublepath}' */
@d contour_code 1 /* command modifier for `\&{contour}' */
@d also_code 2 /* command modifier for `\&{also}' */

@ Pre and postscripts need two new identifiers:

@d with_mp_pre_script 11
@d with_mp_post_script 13

@<Put each...@>=
mp_primitive(mp, "doublepath",thing_to_add,double_path_code);
@:double_path_}{\&{doublepath} primitive@>
mp_primitive(mp, "contour",thing_to_add,contour_code);
@:contour_}{\&{contour} primitive@>
mp_primitive(mp, "also",thing_to_add,also_code);
@:also_}{\&{also} primitive@>
mp_primitive(mp, "withpen",with_option,mp_pen_type);
@:with_pen_}{\&{withpen} primitive@>
mp_primitive(mp, "dashed",with_option,mp_picture_type);
@:dashed_}{\&{dashed} primitive@>
mp_primitive(mp, "withprescript",with_option,with_mp_pre_script);
@:with_mp_pre_script_}{\&{withprescript} primitive@>
mp_primitive(mp, "withpostscript",with_option,with_mp_post_script);
@:with_mp_post_script_}{\&{withpostscript} primitive@>
mp_primitive(mp, "withoutcolor",with_option,mp_no_model);
@:with_color_}{\&{withoutcolor} primitive@>
mp_primitive(mp, "withgreyscale",with_option,mp_grey_model);
@:with_color_}{\&{withgreyscale} primitive@>
mp_primitive(mp, "withcolor",with_option,mp_uninitialized_model);
@:with_color_}{\&{withcolor} primitive@>
/*  \&{withrgbcolor} is an alias for \&{withcolor} */
mp_primitive(mp, "withrgbcolor",with_option,mp_rgb_model);
@:with_color_}{\&{withrgbcolor} primitive@>
mp_primitive(mp, "withcmykcolor",with_option,mp_cmyk_model);
@:with_color_}{\&{withcmykcolor} primitive@>

@ @<Cases of |print_cmd...@>=
case thing_to_add:
  if ( m==contour_code ) mp_print(mp, "contour");
  else if ( m==double_path_code ) mp_print(mp, "doublepath");
  else mp_print(mp, "also");
  break;
case with_option:
  if ( m==mp_pen_type ) mp_print(mp, "withpen");
  else if ( m==with_mp_pre_script ) mp_print(mp, "withprescript");
  else if ( m==with_mp_post_script ) mp_print(mp, "withpostscript");
  else if ( m==mp_no_model ) mp_print(mp, "withoutcolor");
  else if ( m==mp_rgb_model ) mp_print(mp, "withrgbcolor");
  else if ( m==mp_uninitialized_model ) mp_print(mp, "withcolor");
  else if ( m==mp_cmyk_model ) mp_print(mp, "withcmykcolor");
  else if ( m==mp_grey_model ) mp_print(mp, "withgreyscale");
  else mp_print(mp, "dashed");
  break;

@ The |scan_with_list| procedure parses a $\langle$with list$\rangle$ and
updates the list of graphical objects starting at |p|.  Each $\langle$with
clause$\rangle$ updates all graphical objects whose |type| is compatible.
Other objects are ignored.

@<Declare action procedures for use by |do_statement|@>=
static void mp_scan_with_list (MP mp,pointer p) ;

@ @c void mp_scan_with_list (MP mp,pointer p) {
  quarterword t; /* |cur_mod| of the |with_option| (should match |cur_type|) */
  pointer q; /* for list manipulation */
  unsigned old_setting; /* saved |selector| setting */
  pointer k; /* for finding the near-last item in a list  */
  str_number s; /* for string cleanup after combining  */
  pointer cp,pp,dp,ap,bp;
    /* objects being updated; |void| initially; |null| to suppress update */
  cp=mp_void; pp=mp_void; dp=mp_void; ap=mp_void; bp=mp_void;
  k=0;
  while ( mp->cur_cmd==with_option ){ 
    t=(quarterword)mp->cur_mod;
    mp_get_x_next(mp);
    if ( t!=mp_no_model ) mp_scan_expression(mp);
    if (((t==with_mp_pre_script)&&(mp->cur_type!=mp_string_type))||
     ((t==with_mp_post_script)&&(mp->cur_type!=mp_string_type))||
     ((t==mp_uninitialized_model)&&
        ((mp->cur_type!=mp_cmykcolor_type)&&(mp->cur_type!=mp_color_type)
          &&(mp->cur_type!=mp_known)&&(mp->cur_type!=mp_boolean_type)))||
     ((t==mp_cmyk_model)&&(mp->cur_type!=mp_cmykcolor_type))||
     ((t==mp_rgb_model)&&(mp->cur_type!=mp_color_type))||
     ((t==mp_grey_model)&&(mp->cur_type!=mp_known))||
     ((t==mp_pen_type)&&(mp->cur_type!=t))||
     ((t==mp_picture_type)&&(mp->cur_type!=t)) ) {
      @<Complain about improper type@>;
    } else if ( t==mp_uninitialized_model ) {
      if ( cp==mp_void ) @<Make |cp| a colored object in object list~|p|@>;
      if ( cp!=null )
        @<Transfer a color from the current expression to object~|cp|@>;
      mp_flush_cur_exp(mp, 0);
    } else if ( t==mp_rgb_model ) {
      if ( cp==mp_void ) @<Make |cp| a colored object in object list~|p|@>;
      if ( cp!=null )
        @<Transfer a rgbcolor from the current expression to object~|cp|@>;
      mp_flush_cur_exp(mp, 0);
    } else if ( t==mp_cmyk_model ) {
      if ( cp==mp_void ) @<Make |cp| a colored object in object list~|p|@>;
      if ( cp!=null )
        @<Transfer a cmykcolor from the current expression to object~|cp|@>;
      mp_flush_cur_exp(mp, 0);
    } else if ( t==mp_grey_model ) {
      if ( cp==mp_void ) @<Make |cp| a colored object in object list~|p|@>;
      if ( cp!=null )
        @<Transfer a greyscale from the current expression to object~|cp|@>;
      mp_flush_cur_exp(mp, 0);
    } else if ( t==mp_no_model ) {
      if ( cp==mp_void ) @<Make |cp| a colored object in object list~|p|@>;
      if ( cp!=null )
        @<Transfer a noncolor from the current expression to object~|cp|@>;
    } else if ( t==mp_pen_type ) {
      if ( pp==mp_void ) @<Make |pp| an object in list~|p| that needs a pen@>;
      if ( pp!=null ) {
        if ( mp_pen_p(pp)!=null ) mp_toss_knot_list(mp, mp_pen_p(pp));
        mp_pen_p(pp)=mp->cur_exp; mp->cur_type=mp_vacuous;
      }
    } else if ( t==with_mp_pre_script ) {
      if ( ap==mp_void )
        ap=p;
      while ( (ap!=null)&&(! has_color(ap)) )
         ap=mp_link(ap);
      if ( ap!=null ) {
        if ( mp_pre_script(ap)!=null ) { /*  build a new,combined string  */
          s=mp_pre_script(ap);
          old_setting=mp->selector;
	      mp->selector=new_string;
          str_room(length(mp_pre_script(ap))+length(mp->cur_exp)+2);
	      mp_print_str(mp, mp->cur_exp);
          append_char(13);  /* a forced \ps\ newline  */
          mp_print_str(mp, mp_pre_script(ap));
          mp_pre_script(ap)=mp_make_string(mp);
          delete_str_ref(s);
          mp->selector=old_setting;
        } else {
          mp_pre_script(ap)=mp->cur_exp;
        }
        mp->cur_type=mp_vacuous;
      }
    } else if ( t==with_mp_post_script ) {
      if ( bp==mp_void )
        k=p; 
      bp=k;
      while ( mp_link(k)!=null ) {
        k=mp_link(k);
        if ( has_color(k) ) bp=k;
      }
      if ( bp!=null ) {
         if ( mp_post_script(bp)!=null ) {
           s=mp_post_script(bp);
           old_setting=mp->selector;
	       mp->selector=new_string;
           str_room(length(mp_post_script(bp))+length(mp->cur_exp)+2);
           mp_print_str(mp, mp_post_script(bp));
           append_char(13); /* a forced \ps\ newline  */
	   mp_print_str(mp, mp->cur_exp);
           mp_post_script(bp)=mp_make_string(mp);
           delete_str_ref(s);
           mp->selector=old_setting;
         } else {
           mp_post_script(bp)=mp->cur_exp;
         }
         mp->cur_type=mp_vacuous;
       }
    } else { 
      if ( dp==mp_void ) {
        @<Make |dp| a stroked node in list~|p|@>;
      }
      if ( dp!=null ) {
        if ( mp_dash_p(dp)!=null ) delete_edge_ref(mp_dash_p(dp));
        mp_dash_p(dp)=mp_make_dashes(mp, mp->cur_exp);
        dash_scale(dp)=unity;
        mp->cur_type=mp_vacuous;
      }
    }
  }
  @<Copy the information from objects |cp|, |pp|, and |dp| into the rest
    of the list@>;
}

@ @<Complain about improper type@>=
{ exp_err("Improper type");
@.Improper type@>
help2("Next time say `withpen <known pen expression>';",
      "I'll ignore the bad `with' clause and look for another.");
if ( t==with_mp_pre_script )
  mp->help_line[1]="Next time say `withprescript <known string expression>';";
else if ( t==with_mp_post_script )
  mp->help_line[1]="Next time say `withpostscript <known string expression>';";
else if ( t==mp_picture_type )
  mp->help_line[1]="Next time say `dashed <known picture expression>';";
else if ( t==mp_uninitialized_model )
  mp->help_line[1]="Next time say `withcolor <known color expression>';";
else if ( t==mp_rgb_model )
  mp->help_line[1]="Next time say `withrgbcolor <known color expression>';";
else if ( t==mp_cmyk_model )
  mp->help_line[1]="Next time say `withcmykcolor <known cmykcolor expression>';";
else if ( t==mp_grey_model )
  mp->help_line[1]="Next time say `withgreyscale <known numeric expression>';";;
mp_put_get_flush_error(mp, 0);
}

@ Forcing the color to be between |0| and |unity| here guarantees that no
picture will ever contain a color outside the legal range for \ps\ graphics.

@<Transfer a color from the current expression to object~|cp|@>=
{ if ( mp->cur_type==mp_color_type )
   @<Transfer a rgbcolor from the current expression to object~|cp|@>
else if ( mp->cur_type==mp_cmykcolor_type )
   @<Transfer a cmykcolor from the current expression to object~|cp|@>
else if ( mp->cur_type==mp_known )
   @<Transfer a greyscale from the current expression to object~|cp|@>
else if ( mp->cur_exp==false_code )
   @<Transfer a noncolor from the current expression to object~|cp|@>
else if ( mp->cur_exp==true_code )
   @<Transfer no color from the current expression to object~|cp|@>;
}

@ @<Transfer a rgbcolor from the current expression to object~|cp|@>=
{ q=value(mp->cur_exp);
cyan_val(cp)=0;
magenta_val(cp)=0;
yellow_val(cp)=0;
black_val(cp)=0;
red_val(cp)=value(red_part_loc(q));
green_val(cp)=value(green_part_loc(q));
blue_val(cp)=value(blue_part_loc(q));
mp_color_model(cp)=mp_rgb_model;
if ( red_val(cp)<0 ) red_val(cp)=0;
if ( green_val(cp)<0 ) green_val(cp)=0;
if ( blue_val(cp)<0 ) blue_val(cp)=0;
if ( red_val(cp)>unity ) red_val(cp)=unity;
if ( green_val(cp)>unity ) green_val(cp)=unity;
if ( blue_val(cp)>unity ) blue_val(cp)=unity;
}

@ @<Transfer a cmykcolor from the current expression to object~|cp|@>=
{ q=value(mp->cur_exp);
cyan_val(cp)=value(cyan_part_loc(q));
magenta_val(cp)=value(magenta_part_loc(q));
yellow_val(cp)=value(yellow_part_loc(q));
black_val(cp)=value(black_part_loc(q));
mp_color_model(cp)=mp_cmyk_model;
if ( cyan_val(cp)<0 ) cyan_val(cp)=0;
if ( magenta_val(cp)<0 ) magenta_val(cp)=0;
if ( yellow_val(cp)<0 ) yellow_val(cp)=0;
if ( black_val(cp)<0 ) black_val(cp)=0;
if ( cyan_val(cp)>unity ) cyan_val(cp)=unity;
if ( magenta_val(cp)>unity ) magenta_val(cp)=unity;
if ( yellow_val(cp)>unity ) yellow_val(cp)=unity;
if ( black_val(cp)>unity ) black_val(cp)=unity;
}

@ @<Transfer a greyscale from the current expression to object~|cp|@>=
{ q=mp->cur_exp;
cyan_val(cp)=0;
magenta_val(cp)=0;
yellow_val(cp)=0;
black_val(cp)=0;
grey_val(cp)=q;
mp_color_model(cp)=mp_grey_model;
if ( grey_val(cp)<0 ) grey_val(cp)=0;
if ( grey_val(cp)>unity ) grey_val(cp)=unity;
}

@ @<Transfer a noncolor from the current expression to object~|cp|@>=
{
cyan_val(cp)=0;
magenta_val(cp)=0;
yellow_val(cp)=0;
black_val(cp)=0;
grey_val(cp)=0;
mp_color_model(cp)=mp_no_model;
}

@ @<Transfer no color from the current expression to object~|cp|@>=
{
cyan_val(cp)=0;
magenta_val(cp)=0;
yellow_val(cp)=0;
black_val(cp)=0;
grey_val(cp)=0;
mp_color_model(cp)=mp_uninitialized_model;
}

@ @<Make |cp| a colored object in object list~|p|@>=
{ cp=p;
  while ( cp!=null ){ 
    if ( has_color(cp) ) break;
    cp=mp_link(cp);
  }
}

@ @<Make |pp| an object in list~|p| that needs a pen@>=
{ pp=p;
  while ( pp!=null ) {
    if ( has_pen(pp) ) break;
    pp=mp_link(pp);
  }
}

@ @<Make |dp| a stroked node in list~|p|@>=
{ dp=p;
  while ( dp!=null ) {
    if ( mp_type(dp)==mp_stroked_code ) break;
    dp=mp_link(dp);
  }
}

@ @<Copy the information from objects |cp|, |pp|, and |dp| into...@>=
@<Copy |cp|'s color into the colored objects linked to~|cp|@>;
if ( pp>mp_void ) {
  @<Copy |mp_pen_p(pp)| into stroked and filled nodes linked to |pp|@>;
}
if ( dp>mp_void ) {
  @<Make stroked nodes linked to |dp| refer to |mp_dash_p(dp)|@>;
}


@ @<Copy |cp|'s color into the colored objects linked to~|cp|@>=
{ q=mp_link(cp);
  while ( q!=null ) { 
    if ( has_color(q) ) {
      red_val(q)=red_val(cp);
      green_val(q)=green_val(cp);
      blue_val(q)=blue_val(cp);
      black_val(q)=black_val(cp);
      mp_color_model(q)=mp_color_model(cp);
    }
    q=mp_link(q);
  }
}

@ @<Copy |mp_pen_p(pp)| into stroked and filled nodes linked to |pp|@>=
{ q=mp_link(pp);
  while ( q!=null ) {
    if ( has_pen(q) ) {
      if ( mp_pen_p(q)!=null ) mp_toss_knot_list(mp, mp_pen_p(q));
      mp_pen_p(q)=copy_pen(mp_pen_p(pp));
    }
    q=mp_link(q);
  }
}

@ @<Make stroked nodes linked to |dp| refer to |mp_dash_p(dp)|@>=
{ q=mp_link(dp);
  while ( q!=null ) {
    if ( mp_type(q)==mp_stroked_code ) {
      if ( mp_dash_p(q)!=null ) delete_edge_ref(mp_dash_p(q));
      mp_dash_p(q)=mp_dash_p(dp);
      dash_scale(q)=unity;
      if ( mp_dash_p(q)!=null ) add_edge_ref(mp_dash_p(q));
    }
    q=mp_link(q);
  }
}

@ One of the things we need to do when we've parsed an \&{addto} or
similar command is find the header of a supposed \&{picture} variable, given
a token list for that variable.  Since the edge structure is about to be
updated, we use |private_edges| to make sure that this is possible.

@<Declare action procedures for use by |do_statement|@>=
static pointer mp_find_edges_var (MP mp, pointer t) ;

@ @c pointer mp_find_edges_var (MP mp, pointer t) {
  pointer p;
  pointer cur_edges; /* the return value */
  p=mp_find_variable(mp, t); cur_edges=null;
  if ( p==null ) { 
    mp_obliterated(mp, t); mp_put_get_error(mp);
  } else if ( mp_type(p)!=mp_picture_type )  { 
    print_err("Variable "); mp_show_token_list(mp, t,null,1000,0);
@.Variable x is the wrong type@>
    mp_print(mp, " is the wrong type ("); 
    mp_print_type(mp, mp_type(p)); mp_print_char(mp, xord(')'));
    help2("I was looking for a \"known\" picture variable.",
          "So I'll not change anything just now."); 
    mp_put_get_error(mp);
  } else { 
    value(p)=mp_private_edges(mp, value(p));
    cur_edges=value(p);
  }
  mp_flush_node_list(mp, t);
  return cur_edges;
}

@ @<Cases of |do_statement|...@>=
case add_to_command: mp_do_add_to(mp); break;
case bounds_command:mp_do_bounds(mp); break;

@ @<Put each...@>=
mp_primitive(mp, "clip",bounds_command,mp_start_clip_code);
@:clip_}{\&{clip} primitive@>
mp_primitive(mp, "setbounds",bounds_command,mp_start_bounds_code);
@:set_bounds_}{\&{setbounds} primitive@>

@ @<Cases of |print_cmd...@>=
case bounds_command: 
  if ( m==mp_start_clip_code ) mp_print(mp, "clip");
  else mp_print(mp, "setbounds");
  break;

@ The following function parses the beginning of an \&{addto} or \&{clip}
command: it expects a variable name followed by a token with |cur_cmd=sep|
and then an expression.  The function returns the token list for the variable
and stores the command modifier for the separator token in the global variable
|last_add_type|.  We must be careful because this variable might get overwritten
any time we call |get_x_next|.

@<Glob...@>=
quarterword last_add_type;
  /* command modifier that identifies the last \&{addto} command */

@ @<Declare action procedures for use by |do_statement|@>=
static pointer mp_start_draw_cmd (MP mp,quarterword sep) ;

@ @c pointer mp_start_draw_cmd (MP mp,quarterword sep) {
  pointer lhv; /* variable to add to left */
  quarterword add_type=0; /* value to be returned in |last_add_type| */
  lhv=null;
  mp_get_x_next(mp); mp->var_flag=sep; mp_scan_primary(mp);
  if ( mp->cur_type!=mp_token_list ) {
    @<Abandon edges command because there's no variable@>;
  } else  { 
    lhv=mp->cur_exp; add_type=(quarterword)mp->cur_mod;
    mp->cur_type=mp_vacuous; mp_get_x_next(mp); mp_scan_expression(mp);
  }
  mp->last_add_type=add_type;
  return lhv;
}

@ @<Abandon edges command because there's no variable@>=
{ exp_err("Not a suitable variable");
@.Not a suitable variable@>
  help4("At this point I needed to see the name of a picture variable.",
    "(Or perhaps you have indeed presented me with one; I might",
    "have missed it, if it wasn't followed by the proper token.)",
    "So I'll not change anything just now.");
  mp_put_get_flush_error(mp, 0);
}

@ Here is an example of how to use |start_draw_cmd|.

@<Declare action procedures for use by |do_statement|@>=
static void mp_do_bounds (MP mp) ;

@ @c void mp_do_bounds (MP mp) {
  pointer lhv,lhe; /* variable on left, the corresponding edge structure */
  pointer p; /* for list manipulation */
  integer m; /* initial value of |cur_mod| */
  m=mp->cur_mod;
  lhv=mp_start_draw_cmd(mp, to_token);
  if ( lhv!=null ) {
    lhe=mp_find_edges_var(mp, lhv);
    if ( lhe==null ) {
      mp_flush_cur_exp(mp, 0);
    } else if ( mp->cur_type!=mp_path_type ) {
      exp_err("Improper `clip'");
@.Improper `addto'@>
      help2("This expression should have specified a known path.",
            "So I'll not change anything just now."); 
      mp_put_get_flush_error(mp, 0);
    } else if ( mp_left_type(mp->cur_exp)==mp_endpoint ) {
      @<Complain about a non-cycle@>;
    } else {
      @<Make |cur_exp| into a \&{setbounds} or clipping path and add it to |lhe|@>;
    }
  }
}

@ @<Complain about a non-cycle@>=
{ print_err("Not a cycle");
@.Not a cycle@>
  help2("That contour should have ended with `..cycle' or `&cycle'.",
        "So I'll not change anything just now."); mp_put_get_error(mp);
}

@ @<Make |cur_exp| into a \&{setbounds} or clipping path and add...@>=
{ p=mp_new_bounds_node(mp, mp->cur_exp, (quarterword)m);
  mp_link(p)=mp_link(dummy_loc(lhe));
  mp_link(dummy_loc(lhe))=p;
  if ( obj_tail(lhe)==dummy_loc(lhe) ) obj_tail(lhe)=p;
  p=mp_get_node(mp, mp->gr_object_size[stop_type(m)]);
  mp_type(p)=(quarterword)stop_type(m);
  mp_link(obj_tail(lhe))=p;
  obj_tail(lhe)=p;
  mp_init_bbox(mp, lhe);
}

@ The |do_add_to| procedure is a little like |do_clip| but there are a lot more
cases to deal with.

@<Declare action procedures for use by |do_statement|@>=
static void mp_do_add_to (MP mp) ;

@ @c void mp_do_add_to (MP mp) {
  pointer lhv,lhe; /* variable on left, the corresponding edge structure */
  pointer p; /* the graphical object or list for |scan_with_list| to update */
  pointer e; /* an edge structure to be merged */
  quarterword add_type; /* |also_code|, |contour_code|, or |double_path_code| */
  lhv=mp_start_draw_cmd(mp, thing_to_add); add_type=mp->last_add_type;
  if ( lhv!=null ) {
    if ( add_type==also_code ) {
      @<Make sure the current expression is a suitable picture and set |e| and |p|
       appropriately@>;
    } else {
      @<Create a graphical object |p| based on |add_type| and the current
        expression@>;
    }
    mp_scan_with_list(mp, p);
    @<Use |p|, |e|, and |add_type| to augment |lhv| as requested@>;
  }
}

@ Setting |p:=null| causes the $\langle$with list$\rangle$ to be ignored;
setting |e:=null| prevents anything from being added to |lhe|.

@ @<Make sure the current expression is a suitable picture and set |e|...@>=
{ 
  p=null; e=null;
  if ( mp->cur_type!=mp_picture_type ) {
    exp_err("Improper `addto'");
@.Improper `addto'@>
    help2("This expression should have specified a known picture.",
          "So I'll not change anything just now."); 
    mp_put_get_flush_error(mp, 0);
  } else { 
    e=mp_private_edges(mp, mp->cur_exp); mp->cur_type=mp_vacuous;
    p=mp_link(dummy_loc(e));
  }
}

@ In this case |add_type<>also_code| so setting |p:=null| suppresses future
attempts to add to the edge structure.

@<Create a graphical object |p| based on |add_type| and the current...@>=
{ e=null; p=null;
  if ( mp->cur_type==mp_pair_type ) mp_pair_to_path(mp);
  if ( mp->cur_type!=mp_path_type ) {
    exp_err("Improper `addto'");
@.Improper `addto'@>
    help2("This expression should have specified a known path.",
          "So I'll not change anything just now."); 
    mp_put_get_flush_error(mp, 0);
  } else if ( add_type==contour_code ) {
    if ( mp_left_type(mp->cur_exp)==mp_endpoint ) {
      @<Complain about a non-cycle@>;
    } else { 
      p=mp_new_fill_node(mp, mp->cur_exp);
      mp->cur_type=mp_vacuous;
    }
  } else { 
    p=mp_new_stroked_node(mp, mp->cur_exp);
    mp->cur_type=mp_vacuous;
  }
}

@ @<Use |p|, |e|, and |add_type| to augment |lhv| as requested@>=
lhe=mp_find_edges_var(mp, lhv);
if ( lhe==null ) {
  if ( (e==null)&&(p!=null) ) e=mp_toss_gr_object(mp, p);
  if ( e!=null ) delete_edge_ref(e);
} else if ( add_type==also_code ) {
  if ( e!=null ) {
    @<Merge |e| into |lhe| and delete |e|@>;
  } else { 
    do_nothing;
  }
} else if ( p!=null ) {
  mp_link(obj_tail(lhe))=p;
  obj_tail(lhe)=p;
  if ( add_type==double_path_code )
    if ( mp_pen_p(p)==null ) 
      mp_pen_p(p)=mp_get_pen_circle(mp, 0);
}

@ @<Merge |e| into |lhe| and delete |e|@>=
{ if ( mp_link(dummy_loc(e))!=null ) {
    mp_link(obj_tail(lhe))=mp_link(dummy_loc(e));
    obj_tail(lhe)=obj_tail(e);
    obj_tail(e)=dummy_loc(e);
    mp_link(dummy_loc(e))=null;
    mp_flush_dash_list(mp, lhe);
  }
  mp_toss_edges(mp, e);
}

@ @<Cases of |do_statement|...@>=
case ship_out_command: mp_do_ship_out(mp); break;

@ @<Declare action procedures for use by |do_statement|@>=
@<Declare the \ps\ output procedures@>
static void mp_do_ship_out (MP mp) ;

@ @c void mp_do_ship_out (MP mp) {
  integer c; /* the character code */
  mp_get_x_next(mp); mp_scan_expression(mp);
  if ( mp->cur_type!=mp_picture_type ) {
    @<Complain that it's not a known picture@>;
  } else { 
    c=mp_round_unscaled(mp, mp->internal[mp_char_code]) % 256;
    if ( c<0 ) c=c+256;
    @<Store the width information for character code~|c|@>;
    mp_ship_out(mp, mp->cur_exp);
    mp_flush_cur_exp(mp, 0);
  }
}

@ @<Complain that it's not a known picture@>=
{ 
  exp_err("Not a known picture");
  help1("I can only output known pictures.");
  mp_put_get_flush_error(mp, 0);
}

@ The \&{everyjob} command simply assigns a nonzero value to the global variable
|start_sym|.

@<Cases of |do_statement|...@>=
case every_job_command: 
  mp_get_symbol(mp); mp->start_sym=mp->cur_sym; mp_get_x_next(mp);
  break;

@ @<Glob...@>=
halfword start_sym; /* a symbolic token to insert at beginning of job */

@ @<Set init...@>=
mp->start_sym=0;

@ Finally, we have only the ``message'' commands remaining.

@d message_code 0
@d err_message_code 1
@d err_help_code 2
@d filename_template_code 3
@d print_with_leading_zeroes(A,B)  do {
              integer g = mp->pool_ptr;
              integer f = (B);
              mp_print_int(mp, (A)); g = mp->pool_ptr-g;
              if ( f>g ) {
                mp->pool_ptr = mp->pool_ptr - g;
                while ( f>g ) {
                  mp_print_char(mp, xord('0'));
                  decr(f);
                  };
                mp_print_int(mp, (A));
              };
              f = 0;
          } while (0)

@<Put each...@>=
mp_primitive(mp, "message",message_command,message_code);
@:message_}{\&{message} primitive@>
mp_primitive(mp, "errmessage",message_command,err_message_code);
@:err_message_}{\&{errmessage} primitive@>
mp_primitive(mp, "errhelp",message_command,err_help_code);
@:err_help_}{\&{errhelp} primitive@>
mp_primitive(mp, "filenametemplate",message_command,filename_template_code);
@:filename_template_}{\&{filenametemplate} primitive@>

@ @<Cases of |print_cmd...@>=
case message_command: 
  if ( m<err_message_code ) mp_print(mp, "message");
  else if ( m==err_message_code ) mp_print(mp, "errmessage");
  else if ( m==filename_template_code ) mp_print(mp, "filenametemplate");
  else mp_print(mp, "errhelp");
  break;

@ @<Cases of |do_statement|...@>=
case message_command: mp_do_message(mp); break;

@ @<Declare action procedures for use by |do_statement|@>=
@<Declare a procedure called |no_string_err|@>
static void mp_do_message (MP mp) ;

@ 
@c void mp_do_message (MP mp) {
  int m; /* the type of message */
  m=mp->cur_mod; mp_get_x_next(mp); mp_scan_expression(mp);
  if ( mp->cur_type!=mp_string_type )
    mp_no_string_err(mp, "A message should be a known string expression.");
  else {
    switch (m) {
    case message_code: 
      mp_print_nl(mp, ""); mp_print_str(mp, mp->cur_exp);
      break;
    case err_message_code:
      @<Print string |cur_exp| as an error message@>;
      break;
    case err_help_code:
      @<Save string |cur_exp| as the |err_help|@>;
      break;
    case filename_template_code:
      @<Save the filename template@>;
      break;
    } /* there are no other cases */
  }
  mp_flush_cur_exp(mp, 0);
}

@ @<Declare a procedure called |no_string_err|@>=
static void mp_no_string_err (MP mp, const char *s) { 
   exp_err("Not a string");
@.Not a string@>
  help1(s);
  mp_put_get_error(mp);
}

@ The global variable |err_help| is zero when the user has most recently
given an empty help string, or if none has ever been given.

@<Save string |cur_exp| as the |err_help|@>=
{ 
  if ( mp->err_help!=0 ) delete_str_ref(mp->err_help);
  if ( length(mp->cur_exp)==0 ) mp->err_help=0;
  else  { mp->err_help=mp->cur_exp; add_str_ref(mp->err_help); }
}

@ If \&{errmessage} occurs often in |mp_scroll_mode|, without user-defined
\&{errhelp}, we don't want to give a long help message each time. So we
give a verbose explanation only once.

@<Glob...@>=
boolean long_help_seen; /* has the long \.{\\errmessage} help been used? */

@ @<Set init...@>=mp->long_help_seen=false;

@ @<Print string |cur_exp| as an error message@>=
{ 
  print_err(""); mp_print_str(mp, mp->cur_exp);
  if ( mp->err_help!=0 ) {
    mp->use_err_help=true;
  } else if ( mp->long_help_seen ) { 
    help1("(That was another `errmessage'.)") ; 
  } else  { 
   if ( mp->interaction<mp_error_stop_mode ) mp->long_help_seen=true;
    help4("This error message was generated by an `errmessage'",
     "command, so I can\'t give any explicit help.",
     "Pretend that you're Miss Marple: Examine all clues,",
@^Marple, Jane@>
     "and deduce the truth by inspired guesses.");
  }
  mp_put_get_error(mp); mp->use_err_help=false;
}

@ @<Cases of |do_statement|...@>=
case write_command: mp_do_write(mp); break;

@ @<Declare action procedures for use by |do_statement|@>=
static void mp_do_write (MP mp) ;

@ @c void mp_do_write (MP mp) {
  str_number t; /* the line of text to be written */
  write_index n,n0; /* for searching |wr_fname| and |wr_file| arrays */
  unsigned old_setting; /* for saving |selector| during output */
  mp_get_x_next(mp);
  mp_scan_expression(mp);
  if ( mp->cur_type!=mp_string_type ) {
    mp_no_string_err(mp, "The text to be written should be a known string expression");
  } else if ( mp->cur_cmd!=to_token ) { 
    print_err("Missing `to' clause");
    help1("A write command should end with `to <filename>'");
    mp_put_get_error(mp);
  } else { 
    t=mp->cur_exp; mp->cur_type=mp_vacuous;
    mp_get_x_next(mp);
    mp_scan_expression(mp);
    if ( mp->cur_type!=mp_string_type )
      mp_no_string_err(mp, "I can\'t write to that file name.  It isn't a known string");
    else {
      @<Write |t| to the file named by |cur_exp|@>;
    }
    delete_str_ref(t);
  }
  mp_flush_cur_exp(mp, 0);
}

@ @<Write |t| to the file named by |cur_exp|@>=
{ 
  @<Find |n| where |wr_fname[n]=cur_exp| and call |open_write_file| if
    |cur_exp| must be inserted@>;
  if ( mp_str_vs_str(mp, t,mp->eof_line)==0 ) {
    @<Record the end of file on |wr_file[n]|@>;
  } else { 
    old_setting=mp->selector;
    mp->selector=n+write_file;
    mp_print_str(mp, t); mp_print_ln(mp);
    mp->selector = old_setting;
  }
}

@ @<Find |n| where |wr_fname[n]=cur_exp| and call |open_write_file| if...@>=
{
  char *fn = str(mp->cur_exp);
  n=mp->write_files;
  n0=mp->write_files;
  while (mp_xstrcmp(fn,mp->wr_fname[n])!=0) { 
    if ( n==0 ) { /* bottom reached */
	  if ( n0==mp->write_files ) {
        if ( mp->write_files<mp->max_write_files ) {
          incr(mp->write_files);
        } else {
          void **wr_file;
          char **wr_fname;
	      write_index l,k;
          l = mp->max_write_files + (mp->max_write_files/4);
          wr_file = xmalloc((l+1),sizeof(void *));
          wr_fname = xmalloc((l+1),sizeof(char *));
	      for (k=0;k<=l;k++) {
            if (k<=mp->max_write_files) {
   	          wr_file[k]=mp->wr_file[k]; 
              wr_fname[k]=mp->wr_fname[k];
            } else {
   	          wr_file[k]=0; 
              wr_fname[k]=NULL;
            }
          }
	      xfree(mp->wr_file); xfree(mp->wr_fname);
          mp->max_write_files = l;
          mp->wr_file = wr_file;
          mp->wr_fname = wr_fname;
        }
      }
      n=n0;
      mp_open_write_file(mp, fn ,n);
    } else { 
      decr(n);
	  if ( mp->wr_fname[n]==NULL )  n0=n; 
    }
  }
}

@ @<Record the end of file on |wr_file[n]|@>=
{ (mp->close_file)(mp,mp->wr_file[n]);
  xfree(mp->wr_fname[n]);
  if ( n==mp->write_files-1 ) mp->write_files=n;
}


@* \[42] Writing font metric data.
\TeX\ gets its knowledge about fonts from font metric files, also called
\.{TFM} files; the `\.T' in `\.{TFM}' stands for \TeX,
but other programs know about them too. One of \MP's duties is to
write \.{TFM} files so that the user's fonts can readily be
applied to typesetting.
@:TFM files}{\.{TFM} files@>
@^font metric files@>

The information in a \.{TFM} file appears in a sequence of 8-bit bytes.
Since the number of bytes is always a multiple of~4, we could
also regard the file as a sequence of 32-bit words, but \MP\ uses the
byte interpretation. The format of \.{TFM} files was designed by
Lyle Ramshaw in 1980. The intent is to convey a lot of different kinds
@^Ramshaw, Lyle Harold@>
of information in a compact but useful form.

@<Glob...@>=
void * tfm_file; /* the font metric output goes here */
char * metric_file_name; /* full name of the font metric file */

@ The first 24 bytes (6 words) of a \.{TFM} file contain twelve 16-bit
integers that give the lengths of the various subsequent portions
of the file. These twelve integers are, in order:
$$\vbox{\halign{\hfil#&$\null=\null$#\hfil\cr
|lf|&length of the entire file, in words;\cr
|lh|&length of the header data, in words;\cr
|bc|&smallest character code in the font;\cr
|ec|&largest character code in the font;\cr
|nw|&number of words in the width table;\cr
|nh|&number of words in the height table;\cr
|nd|&number of words in the depth table;\cr
|ni|&number of words in the italic correction table;\cr
|nl|&number of words in the lig/kern table;\cr
|nk|&number of words in the kern table;\cr
|ne|&number of words in the extensible character table;\cr
|np|&number of font parameter words.\cr}}$$
They are all nonnegative and less than $2^{15}$. We must have |bc-1<=ec<=255|,
|ne<=256|, and
$$\hbox{|lf=6+lh+(ec-bc+1)+nw+nh+nd+ni+nl+nk+ne+np|.}$$
Note that a font may contain as many as 256 characters (if |bc=0| and |ec=255|),
and as few as 0 characters (if |bc=ec+1|).

Incidentally, when two or more 8-bit bytes are combined to form an integer of
16 or more bits, the most significant bytes appear first in the file.
This is called BigEndian order.
@^BigEndian order@>

@ The rest of the \.{TFM} file may be regarded as a sequence of ten data
arrays.

The most important data type used here is a |fix_word|, which is
a 32-bit representation of a binary fraction. A |fix_word| is a signed
quantity, with the two's complement of the entire word used to represent
negation. Of the 32 bits in a |fix_word|, exactly 12 are to the left of the
binary point; thus, the largest |fix_word| value is $2048-2^{-20}$, and
the smallest is $-2048$. We will see below, however, that all but two of
the |fix_word| values must lie between $-16$ and $+16$.

@ The first data array is a block of header information, which contains
general facts about the font. The header must contain at least two words,
|header[0]| and |header[1]|, whose meaning is explained below.  Additional
header information of use to other software routines might also be
included, and \MP\ will generate it if the \.{headerbyte} command occurs.
For example, 16 more words of header information are in use at the Xerox
Palo Alto Research Center; the first ten specify the character coding
scheme used (e.g., `\.{XEROX TEXT}' or `\.{TEX MATHSY}'), the next five
give the font family name (e.g., `\.{HELVETICA}' or `\.{CMSY}'), and the
last gives the ``face byte.''

\yskip\hang|header[0]| is a 32-bit check sum that \MP\ will copy into
the \.{GF} output file. This helps ensure consistency between files,
since \TeX\ records the check sums from the \.{TFM}'s it reads, and these
should match the check sums on actual fonts that are used.  The actual
relation between this check sum and the rest of the \.{TFM} file is not
important; the check sum is simply an identification number with the
property that incompatible fonts almost always have distinct check sums.
@^check sum@>

\yskip\hang|header[1]| is a |fix_word| containing the design size of the
font, in units of \TeX\ points. This number must be at least 1.0; it is
fairly arbitrary, but usually the design size is 10.0 for a ``10 point''
font, i.e., a font that was designed to look best at a 10-point size,
whatever that really means. When a \TeX\ user asks for a font `\.{at}
$\delta$ \.{pt}', the effect is to override the design size and replace it
by $\delta$, and to multiply the $x$ and~$y$ coordinates of the points in
the font image by a factor of $\delta$ divided by the design size.  {\sl
All other dimensions in the\/ \.{TFM} file are |fix_word|\kern-1pt\
numbers in design-size units.} Thus, for example, the value of |param[6]|,
which defines the \.{em} unit, is often the |fix_word| value $2^{20}=1.0$,
since many fonts have a design size equal to one em.  The other dimensions
must be less than 16 design-size units in absolute value; thus,
|header[1]| and |param[1]| are the only |fix_word| entries in the whole
\.{TFM} file whose first byte might be something besides 0 or 255.
@^design size@>

@ Next comes the |char_info| array, which contains one |char_info_word|
per character. Each word in this part of the file contains six fields
packed into four bytes as follows.

\yskip\hang first byte: |width_index| (8 bits)\par
\hang second byte: |height_index| (4 bits) times 16, plus |depth_index|
  (4~bits)\par
\hang third byte: |italic_index| (6 bits) times 4, plus |tag|
  (2~bits)\par
\hang fourth byte: |remainder| (8 bits)\par
\yskip\noindent
The actual width of a character is \\{width}|[width_index]|, in design-size
units; this is a device for compressing information, since many characters
have the same width. Since it is quite common for many characters
to have the same height, depth, or italic correction, the \.{TFM} format
imposes a limit of 16 different heights, 16 different depths, and
64 different italic corrections.

Incidentally, the relation $\\{width}[0]=\\{height}[0]=\\{depth}[0]=
\\{italic}[0]=0$ should always hold, so that an index of zero implies a
value of zero.  The |width_index| should never be zero unless the
character does not exist in the font, since a character is valid if and
only if it lies between |bc| and |ec| and has a nonzero |width_index|.

@ The |tag| field in a |char_info_word| has four values that explain how to
interpret the |remainder| field.

\yskip\hang|tag=0| (|no_tag|) means that |remainder| is unused.\par
\hang|tag=1| (|lig_tag|) means that this character has a ligature/kerning
program starting at location |remainder| in the |lig_kern| array.\par
\hang|tag=2| (|list_tag|) means that this character is part of a chain of
characters of ascending sizes, and not the largest in the chain.  The
|remainder| field gives the character code of the next larger character.\par
\hang|tag=3| (|ext_tag|) means that this character code represents an
extensible character, i.e., a character that is built up of smaller pieces
so that it can be made arbitrarily large. The pieces are specified in
|exten[remainder]|.\par
\yskip\noindent
Characters with |tag=2| and |tag=3| are treated as characters with |tag=0|
unless they are used in special circumstances in math formulas. For example,
\TeX's \.{\\sum} operation looks for a |list_tag|, and the \.{\\left}
operation looks for both |list_tag| and |ext_tag|.

@d no_tag 0 /* vanilla character */
@d lig_tag 1 /* character has a ligature/kerning program */
@d list_tag 2 /* character has a successor in a charlist */
@d ext_tag 3 /* character is extensible */

@ The |lig_kern| array contains instructions in a simple programming language
that explains what to do for special letter pairs. Each word in this array is a
|lig_kern_command| of four bytes.

\yskip\hang first byte: |skip_byte|, indicates that this is the final program
  step if the byte is 128 or more, otherwise the next step is obtained by
  skipping this number of intervening steps.\par
\hang second byte: |next_char|, ``if |next_char| follows the current character,
  then perform the operation and stop, otherwise continue.''\par
\hang third byte: |op_byte|, indicates a ligature step if less than~128,
  a kern step otherwise.\par
\hang fourth byte: |remainder|.\par
\yskip\noindent
In a kern step, an
additional space equal to |kern[256*(op_byte-128)+remainder]| is inserted
between the current character and |next_char|. This amount is
often negative, so that the characters are brought closer together
by kerning; but it might be positive.

There are eight kinds of ligature steps, having |op_byte| codes $4a+2b+c$ where
$0\le a\le b+c$ and $0\le b,c\le1$. The character whose code is
|remainder| is inserted between the current character and |next_char|;
then the current character is deleted if $b=0$, and |next_char| is
deleted if $c=0$; then we pass over $a$~characters to reach the next
current character (which may have a ligature/kerning program of its own).

If the very first instruction of the |lig_kern| array has |skip_byte=255|,
the |next_char| byte is the so-called right boundary character of this font;
the value of |next_char| need not lie between |bc| and~|ec|.
If the very last instruction of the |lig_kern| array has |skip_byte=255|,
there is a special ligature/kerning program for a left boundary character,
beginning at location |256*op_byte+remainder|.
The interpretation is that \TeX\ puts implicit boundary characters
before and after each consecutive string of characters from the same font.
These implicit characters do not appear in the output, but they can affect
ligatures and kerning.

If the very first instruction of a character's |lig_kern| program has
|skip_byte>128|, the program actually begins in location
|256*op_byte+remainder|. This feature allows access to large |lig_kern|
arrays, because the first instruction must otherwise
appear in a location |<=255|.

Any instruction with |skip_byte>128| in the |lig_kern| array must satisfy
the condition
$$\hbox{|256*op_byte+remainder<nl|.}$$
If such an instruction is encountered during
normal program execution, it denotes an unconditional halt; no ligature
command is performed.

@d stop_flag (128)
  /* value indicating `\.{STOP}' in a lig/kern program */
@d kern_flag (128) /* op code for a kern step */
@d skip_byte(A) mp->lig_kern[(A)].b0
@d next_char(A) mp->lig_kern[(A)].b1
@d op_byte(A) mp->lig_kern[(A)].b2
@d rem_byte(A) mp->lig_kern[(A)].b3

@ Extensible characters are specified by an |extensible_recipe|, which
consists of four bytes called |top|, |mid|, |bot|, and |rep| (in this
order). These bytes are the character codes of individual pieces used to
build up a large symbol.  If |top|, |mid|, or |bot| are zero, they are not
present in the built-up result. For example, an extensible vertical line is
like an extensible bracket, except that the top and bottom pieces are missing.

Let $T$, $M$, $B$, and $R$ denote the respective pieces, or an empty box
if the piece isn't present. Then the extensible characters have the form
$TR^kMR^kB$ from top to bottom, for some |k>=0|, unless $M$ is absent;
in the latter case we can have $TR^kB$ for both even and odd values of~|k|.
The width of the extensible character is the width of $R$; and the
height-plus-depth is the sum of the individual height-plus-depths of the
components used, since the pieces are butted together in a vertical list.

@d ext_top(A) mp->exten[(A)].b0 /* |top| piece in a recipe */
@d ext_mid(A) mp->exten[(A)].b1 /* |mid| piece in a recipe */
@d ext_bot(A) mp->exten[(A)].b2 /* |bot| piece in a recipe */
@d ext_rep(A) mp->exten[(A)].b3 /* |rep| piece in a recipe */

@ The final portion of a \.{TFM} file is the |param| array, which is another
sequence of |fix_word| values.

\yskip\hang|param[1]=slant| is the amount of italic slant, which is used
to help position accents. For example, |slant=.25| means that when you go
up one unit, you also go .25 units to the right. The |slant| is a pure
number; it is the only |fix_word| other than the design size itself that is
not scaled by the design size.
@^design size@>

\hang|param[2]=space| is the normal spacing between words in text.
Note that character 040 in the font need not have anything to do with
blank spaces.

\hang|param[3]=space_stretch| is the amount of glue stretching between words.

\hang|param[4]=space_shrink| is the amount of glue shrinking between words.

\hang|param[5]=x_height| is the size of one ex in the font; it is also
the height of letters for which accents don't have to be raised or lowered.

\hang|param[6]=quad| is the size of one em in the font.

\hang|param[7]=extra_space| is the amount added to |param[2]| at the
ends of sentences.

\yskip\noindent
If fewer than seven parameters are present, \TeX\ sets the missing parameters
to zero.

@d slant_code 1
@d space_code 2
@d space_stretch_code 3
@d space_shrink_code 4
@d x_height_code 5
@d quad_code 6
@d extra_space_code 7

@ So that is what \.{TFM} files hold. One of \MP's duties is to output such
information, and it does this all at once at the end of a job.
In order to prepare for such frenetic activity, it squirrels away the
necessary facts in various arrays as information becomes available.

Character dimensions (\&{charwd}, \&{charht}, \&{chardp}, and \&{charic})
are stored respectively in |tfm_width|, |tfm_height|, |tfm_depth|, and
|tfm_ital_corr|. Other information about a character (e.g., about
its ligatures or successors) is accessible via the |char_tag| and
|char_remainder| arrays. Other information about the font as a whole
is kept in additional arrays called |header_byte|, |lig_kern|,
|kern|, |exten|, and |param|.

@d max_tfm_int 32510
@d undefined_label max_tfm_int /* an undefined local label */

@<Glob...@>=
#define TFM_ITEMS 257
eight_bits bc;
eight_bits ec; /* smallest and largest character codes shipped out */
scaled tfm_width[TFM_ITEMS]; /* \&{charwd} values */
scaled tfm_height[TFM_ITEMS]; /* \&{charht} values */
scaled tfm_depth[TFM_ITEMS]; /* \&{chardp} values */
scaled tfm_ital_corr[TFM_ITEMS]; /* \&{charic} values */
boolean char_exists[TFM_ITEMS]; /* has this code been shipped out? */
int char_tag[TFM_ITEMS]; /* |remainder| category */
int char_remainder[TFM_ITEMS]; /* the |remainder| byte */
char *header_byte; /* bytes of the \.{TFM} header */
int header_last; /* last initialized \.{TFM} header byte */
int header_size; /* size of the \.{TFM} header */
four_quarters *lig_kern; /* the ligature/kern table */
short nl; /* the number of ligature/kern steps so far */
scaled *kern; /* distinct kerning amounts */
short nk; /* the number of distinct kerns so far */
four_quarters exten[TFM_ITEMS]; /* extensible character recipes */
short ne; /* the number of extensible characters so far */
scaled *param; /* \&{fontinfo} parameters */
short np; /* the largest \&{fontinfo} parameter specified so far */
short nw;short nh;short nd;short ni; /* sizes of \.{TFM} subtables */
short skip_table[TFM_ITEMS]; /* local label status */
boolean lk_started; /* has there been a lig/kern step in this command yet? */
integer bchar; /* right boundary character */
short bch_label; /* left boundary starting location */
short ll;short lll; /* registers used for lig/kern processing */
short label_loc[257]; /* lig/kern starting addresses */
eight_bits label_char[257]; /* characters for |label_loc| */
short label_ptr; /* highest position occupied in |label_loc| */

@ @<Allocate or initialize ...@>=
mp->header_last=7;
mp->header_size = 128; /* just for init */
mp->header_byte = xmalloc(mp->header_size, sizeof(char));

@ @<Dealloc variables@>=
xfree(mp->header_byte);
xfree(mp->lig_kern);
xfree(mp->kern);
xfree(mp->param);

@ @<Set init...@>=
for (k=0;k<= 255;k++ ) {
  mp->tfm_width[k]=0; mp->tfm_height[k]=0; mp->tfm_depth[k]=0; mp->tfm_ital_corr[k]=0;
  mp->char_exists[k]=false; mp->char_tag[k]=no_tag; mp->char_remainder[k]=0;
  mp->skip_table[k]=undefined_label;
}
memset(mp->header_byte,0,(size_t)mp->header_size);
mp->bc=255; mp->ec=0; mp->nl=0; mp->nk=0; mp->ne=0; mp->np=0;
mp->internal[mp_boundary_char]=-unity;
mp->bch_label=undefined_label;
mp->label_loc[0]=-1; mp->label_ptr=0;

@ @<Declarations@>=
static scaled mp_tfm_check (MP mp,quarterword m) ;

@ @c
static scaled mp_tfm_check (MP mp,quarterword m) {
  if ( abs(mp->internal[m])>=fraction_half ) {
    print_err("Enormous "); mp_print(mp, mp->int_name[m]);
@.Enormous charwd...@>
@.Enormous chardp...@>
@.Enormous charht...@>
@.Enormous charic...@>
@.Enormous designsize...@>
    mp_print(mp, " has been reduced");
    help1("Font metric dimensions must be less than 2048pt.");
    mp_put_get_error(mp);
    if ( mp->internal[m]>0 ) return (fraction_half-1);
    else return (1-fraction_half);
  } else {
    return mp->internal[m];
  }
}

@ @<Store the width information for character code~|c|@>=
if ( c<mp->bc ) mp->bc=(eight_bits)c;
if ( c>mp->ec ) mp->ec=(eight_bits)c;
mp->char_exists[c]=true;
mp->tfm_width[c]=mp_tfm_check(mp,mp_char_wd);
mp->tfm_height[c]=mp_tfm_check(mp, mp_char_ht);
mp->tfm_depth[c]=mp_tfm_check(mp, mp_char_dp);
mp->tfm_ital_corr[c]=mp_tfm_check(mp, mp_char_ic)

@ Now let's consider \MP's special \.{TFM}-oriented commands.

@<Cases of |do_statement|...@>=
case tfm_command: mp_do_tfm_command(mp); break;

@ @d char_list_code 0
@d lig_table_code 1
@d extensible_code 2
@d header_byte_code 3
@d font_dimen_code 4

@<Put each...@>=
mp_primitive(mp, "charlist",tfm_command,char_list_code);
@:char_list_}{\&{charlist} primitive@>
mp_primitive(mp, "ligtable",tfm_command,lig_table_code);
@:lig_table_}{\&{ligtable} primitive@>
mp_primitive(mp, "extensible",tfm_command,extensible_code);
@:extensible_}{\&{extensible} primitive@>
mp_primitive(mp, "headerbyte",tfm_command,header_byte_code);
@:header_byte_}{\&{headerbyte} primitive@>
mp_primitive(mp, "fontdimen",tfm_command,font_dimen_code);
@:font_dimen_}{\&{fontdimen} primitive@>

@ @<Cases of |print_cmd...@>=
case tfm_command: 
  switch (m) {
  case char_list_code:mp_print(mp, "charlist"); break;
  case lig_table_code:mp_print(mp, "ligtable"); break;
  case extensible_code:mp_print(mp, "extensible"); break;
  case header_byte_code:mp_print(mp, "headerbyte"); break;
  default: mp_print(mp, "fontdimen"); break;
  }
  break;

@ @<Declare action procedures for use by |do_statement|@>=
static eight_bits mp_get_code (MP mp) ;

@ @c eight_bits mp_get_code (MP mp) { /* scans a character code value */
  integer c; /* the code value found */
  mp_get_x_next(mp); mp_scan_expression(mp);
  if ( mp->cur_type==mp_known ) { 
    c=mp_round_unscaled(mp, mp->cur_exp);
    if ( c>=0 ) if ( c<256 ) return (eight_bits)c;
  } else if ( mp->cur_type==mp_string_type ) {
    if ( length(mp->cur_exp)==1 )  { 
      c=mp->str_pool[mp->str_start[mp->cur_exp]];
      return (eight_bits)c;
    }
  }
  exp_err("Invalid code has been replaced by 0");
@.Invalid code...@>
  help2("I was looking for a number between 0 and 255, or for a",
        "string of length 1. Didn't find it; will use 0 instead.");
  mp_put_get_flush_error(mp, 0); c=0;
  return (eight_bits)c;
}

@ @<Declare action procedures for use by |do_statement|@>=
static void mp_set_tag (MP mp,halfword c, quarterword t, halfword r) ;

@ @c void mp_set_tag (MP mp,halfword c, quarterword t, halfword r) { 
  if ( mp->char_tag[c]==no_tag ) {
    mp->char_tag[c]=t; mp->char_remainder[c]=r;
    if ( t==lig_tag ){ 
      mp->label_ptr++; mp->label_loc[mp->label_ptr]=(short)r; 
      mp->label_char[mp->label_ptr]=(eight_bits)c;
    }
  } else {
    @<Complain about a character tag conflict@>;
  }
}

@ @<Complain about a character tag conflict@>=
{ 
  print_err("Character ");
  if ( (c>' ')&&(c<127) ) mp_print_char(mp,xord(c));
  else if ( c==256 ) mp_print(mp, "||");
  else  { mp_print(mp, "code "); mp_print_int(mp, c); };
  mp_print(mp, " is already ");
@.Character c is already...@>
  switch (mp->char_tag[c]) {
  case lig_tag: mp_print(mp, "in a ligtable"); break;
  case list_tag: mp_print(mp, "in a charlist"); break;
  case ext_tag: mp_print(mp, "extensible"); break;
  } /* there are no other cases */
  help2("It's not legal to label a character more than once.",
        "So I'll not change anything just now.");
  mp_put_get_error(mp); 
}

@ @<Declare action procedures for use by |do_statement|@>=
static void mp_do_tfm_command (MP mp) ;

@ @c void mp_do_tfm_command (MP mp) {
  int c,cc; /* character codes */
  int k; /* index into the |kern| array */
  int j; /* index into |header_byte| or |param| */
  switch (mp->cur_mod) {
  case char_list_code: 
    c=mp_get_code(mp);
     /* we will store a list of character successors */
    while ( mp->cur_cmd==colon )   { 
      cc=mp_get_code(mp); mp_set_tag(mp, c,list_tag,cc); c=cc;
    };
    break;
  case lig_table_code: 
    if (mp->lig_kern==NULL) 
       mp->lig_kern = xmalloc((max_tfm_int+1),sizeof(four_quarters));
    if (mp->kern==NULL) 
       mp->kern = xmalloc((max_tfm_int+1),sizeof(scaled));
    @<Store a list of ligature/kern steps@>;
    break;
  case extensible_code: 
    @<Define an extensible recipe@>;
    break;
  case header_byte_code: 
  case font_dimen_code: 
    c=mp->cur_mod; mp_get_x_next(mp);
    mp_scan_expression(mp);
    if ( (mp->cur_type!=mp_known)||(mp->cur_exp<half_unit) ) {
      exp_err("Improper location");
@.Improper location@>
      help2("I was looking for a known, positive number.",
            "For safety's sake I'll ignore the present command.");
      mp_put_get_error(mp);
    } else  { 
      j=mp_round_unscaled(mp, mp->cur_exp);
      if ( mp->cur_cmd!=colon ) {
        mp_missing_err(mp, ":");
@.Missing `:'@>
        help1("A colon should follow a headerbyte or fontinfo location.");
        mp_back_error(mp);
      }
      if ( c==header_byte_code ) { 
	@<Store a list of header bytes@>;
      } else {     
        if (mp->param==NULL) 
          mp->param = xmalloc((max_tfm_int+1),sizeof(scaled));
        @<Store a list of font dimensions@>;
      }
    }
    break;
  } /* there are no other cases */
}

@ @<Store a list of ligature/kern steps@>=
{ 
  mp->lk_started=false;
CONTINUE: 
  mp_get_x_next(mp);
  if ((mp->cur_cmd==skip_to)&& mp->lk_started )
    @<Process a |skip_to| command and |goto done|@>;
  if ( mp->cur_cmd==bchar_label ) { c=256; mp->cur_cmd=colon; }
  else { mp_back_input(mp); c=mp_get_code(mp); };
  if ((mp->cur_cmd==colon)||(mp->cur_cmd==double_colon)) {
    @<Record a label in a lig/kern subprogram and |goto continue|@>;
  }
  if ( mp->cur_cmd==lig_kern_token ) { 
    @<Compile a ligature/kern command@>; 
  } else  { 
    print_err("Illegal ligtable step");
@.Illegal ligtable step@>
    help1("I was looking for `=:' or `kern' here.");
    mp_back_error(mp); next_char(mp->nl)=qi(0); 
    op_byte(mp->nl)=qi(0); rem_byte(mp->nl)=qi(0);
    skip_byte(mp->nl)=stop_flag+1; /* this specifies an unconditional stop */
  }
  if ( mp->nl==max_tfm_int) mp_fatal_error(mp, "ligtable too large");
  mp->nl++;
  if ( mp->cur_cmd==comma ) goto CONTINUE;
  if ( skip_byte(mp->nl-1)<stop_flag ) skip_byte(mp->nl-1)=stop_flag;
}
DONE:

@ @<Put each...@>=
mp_primitive(mp, "=:",lig_kern_token,0);
@:=:_}{\.{=:} primitive@>
mp_primitive(mp, "=:|",lig_kern_token,1);
@:=:/_}{\.{=:\char'174} primitive@>
mp_primitive(mp, "=:|>",lig_kern_token,5);
@:=:/>_}{\.{=:\char'174>} primitive@>
mp_primitive(mp, "|=:",lig_kern_token,2);
@:=:/_}{\.{\char'174=:} primitive@>
mp_primitive(mp, "|=:>",lig_kern_token,6);
@:=:/>_}{\.{\char'174=:>} primitive@>
mp_primitive(mp, "|=:|",lig_kern_token,3);
@:=:/_}{\.{\char'174=:\char'174} primitive@>
mp_primitive(mp, "|=:|>",lig_kern_token,7);
@:=:/>_}{\.{\char'174=:\char'174>} primitive@>
mp_primitive(mp, "|=:|>>",lig_kern_token,11);
@:=:/>_}{\.{\char'174=:\char'174>>} primitive@>
mp_primitive(mp, "kern",lig_kern_token,128);
@:kern_}{\&{kern} primitive@>

@ @<Cases of |print_cmd...@>=
case lig_kern_token: 
  switch (m) {
  case 0:mp_print(mp, "=:"); break;
  case 1:mp_print(mp, "=:|"); break;
  case 2:mp_print(mp, "|=:"); break;
  case 3:mp_print(mp, "|=:|"); break;
  case 5:mp_print(mp, "=:|>"); break;
  case 6:mp_print(mp, "|=:>"); break;
  case 7:mp_print(mp, "|=:|>"); break;
  case 11:mp_print(mp, "|=:|>>"); break;
  default: mp_print(mp, "kern"); break;
  }
  break;

@ Local labels are implemented by maintaining the |skip_table| array,
where |skip_table[c]| is either |undefined_label| or the address of the
most recent lig/kern instruction that skips to local label~|c|. In the
latter case, the |skip_byte| in that instruction will (temporarily)
be zero if there were no prior skips to this label, or it will be the
distance to the prior skip.

We may need to cancel skips that span more than 127 lig/kern steps.

@d cancel_skips(A) mp->ll=(A);
  do {  
    mp->lll=qo(skip_byte(mp->ll)); 
    skip_byte(mp->ll)=stop_flag; mp->ll=(short)(mp->ll-mp->lll);
  } while (mp->lll!=0)
@d skip_error(A) { print_err("Too far to skip");
@.Too far to skip@>
  help1("At most 127 lig/kern steps can separate skipto1 from 1::.");
  mp_error(mp); cancel_skips((A));
  }

@<Process a |skip_to| command and |goto done|@>=
{ 
  c=mp_get_code(mp);
  if ( mp->nl-mp->skip_table[c]>128 ) {
    skip_error(mp->skip_table[c]); mp->skip_table[c]=(short)undefined_label;
  }
  if ( mp->skip_table[c]==undefined_label ) skip_byte(mp->nl-1)=qi(0);
  else skip_byte(mp->nl-1)=qi(mp->nl-mp->skip_table[c]-1);
  mp->skip_table[c]=(short)(mp->nl-1); goto DONE;
}

@ @<Record a label in a lig/kern subprogram and |goto continue|@>=
{ 
  if ( mp->cur_cmd==colon ) {
    if ( c==256 ) mp->bch_label=mp->nl;
    else mp_set_tag(mp, c,lig_tag,mp->nl);
  } else if ( mp->skip_table[c]<undefined_label ) {
    mp->ll=mp->skip_table[c]; mp->skip_table[c]=undefined_label;
    do {  
      mp->lll=qo(skip_byte(mp->ll));
      if ( mp->nl-mp->ll>128 ) {
        skip_error(mp->ll); goto CONTINUE;
      }
      skip_byte(mp->ll)=qi(mp->nl-mp->ll-1); mp->ll=(short)(mp->ll-mp->lll);
    } while (mp->lll!=0);
  }
  goto CONTINUE;
}

@ @<Compile a ligature/kern...@>=
{ 
  next_char(mp->nl)=qi(c); skip_byte(mp->nl)=qi(0);
  if ( mp->cur_mod<128 ) { /* ligature op */
    op_byte(mp->nl)=qi(mp->cur_mod); rem_byte(mp->nl)=qi(mp_get_code(mp));
  } else { 
    mp_get_x_next(mp); mp_scan_expression(mp);
    if ( mp->cur_type!=mp_known ) {
      exp_err("Improper kern");
@.Improper kern@>
      help2("The amount of kern should be a known numeric value.",
            "I'm zeroing this one. Proceed, with fingers crossed.");
      mp_put_get_flush_error(mp, 0);
    }
    mp->kern[mp->nk]=mp->cur_exp;
    k=0; 
    while ( mp->kern[k]!=mp->cur_exp ) incr(k);
    if ( k==mp->nk ) {
      if ( mp->nk==max_tfm_int ) mp_fatal_error(mp, "too many TFM kerns");
      mp->nk++;
    }
    op_byte(mp->nl)=qi(kern_flag+(k / 256));
    rem_byte(mp->nl)=qi((k % 256));
  }
  mp->lk_started=true;
}

@ @d missing_extensible_punctuation(A) 
  { mp_missing_err(mp, (A));
@.Missing `\char`\#'@>
  help1("I'm processing `extensible c: t,m,b,r'."); mp_back_error(mp);
  }

@<Define an extensible recipe@>=
{ 
  if ( mp->ne==256 ) mp_fatal_error(mp, "too many extensible recipies");
  c=mp_get_code(mp); mp_set_tag(mp, c,ext_tag,mp->ne);
  if ( mp->cur_cmd!=colon ) missing_extensible_punctuation(":");
  ext_top(mp->ne)=qi(mp_get_code(mp));
  if ( mp->cur_cmd!=comma ) missing_extensible_punctuation(",");
  ext_mid(mp->ne)=qi(mp_get_code(mp));
  if ( mp->cur_cmd!=comma ) missing_extensible_punctuation(",");
  ext_bot(mp->ne)=qi(mp_get_code(mp));
  if ( mp->cur_cmd!=comma ) missing_extensible_punctuation(",");
  ext_rep(mp->ne)=qi(mp_get_code(mp));
  mp->ne++;
}

@ The header could contain ASCII zeroes, so can't use |strdup|.

@<Store a list of header bytes@>=
j--;
do {  
  if ( j>=mp->header_size ) {
    size_t l = (size_t)(mp->header_size + (mp->header_size/4));
    char *t = xmalloc(l,1);
    memset(t,0,l); 
    (void)memcpy(t,mp->header_byte,(size_t)mp->header_size);
    xfree (mp->header_byte);
    mp->header_byte = t;
    mp->header_size = (int)l;
  }
  mp->header_byte[j]=(char)mp_get_code(mp); 
  incr(j); incr(mp->header_last);
} while (mp->cur_cmd==comma)

@ @<Store a list of font dimensions@>=
do {  
  if ( j>max_tfm_int ) mp_fatal_error(mp, "too many fontdimens");
  while ( j>mp->np ) { mp->np++; mp->param[mp->np]=0; };
  mp_get_x_next(mp); mp_scan_expression(mp);
  if ( mp->cur_type!=mp_known ){ 
    exp_err("Improper font parameter");
@.Improper font parameter@>
    help1("I'm zeroing this one. Proceed, with fingers crossed.");
    mp_put_get_flush_error(mp, 0);
  }
  mp->param[j]=mp->cur_exp; incr(j);
} while (mp->cur_cmd==comma)

@ OK: We've stored all the data that is needed for the \.{TFM} file.
All that remains is to output it in the correct format.

An interesting problem needs to be solved in this connection, because
the \.{TFM} format allows at most 256~widths, 16~heights, 16~depths,
and 64~italic corrections. If the data has more distinct values than
this, we want to meet the necessary restrictions by perturbing the
given values as little as possible.

\MP\ solves this problem in two steps. First the values of a given
kind (widths, heights, depths, or italic corrections) are sorted;
then the list of sorted values is perturbed, if necessary.

The sorting operation is facilitated by having a special node of
essentially infinite |value| at the end of the current list.

@<Initialize table entries...@>=
value(inf_val)=fraction_four;

@ Straight linear insertion is good enough for sorting, since the lists
are usually not terribly long. As we work on the data, the current list
will start at |mp_link(temp_head)| and end at |inf_val|; the nodes in this
list will be in increasing order of their |value| fields.

Given such a list, the |sort_in| function takes a value and returns a pointer
to where that value can be found in the list. The value is inserted in
the proper place, if necessary.

At the time we need to do these operations, most of \MP's work has been
completed, so we will have plenty of memory to play with. The value nodes
that are allocated for sorting will never be returned to free storage.

@d clear_the_list mp_link(temp_head)=inf_val

@c 
static pointer mp_sort_in (MP mp,scaled v) {
  pointer p,q,r; /* list manipulation registers */
  p=temp_head;
  while (1) { 
    q=mp_link(p);
    if ( v<=value(q) ) break;
    p=q;
  }
  if ( v<value(q) ) {
    r=mp_get_node(mp, value_node_size); value(r)=v; mp_link(r)=q; mp_link(p)=r;
  }
  return mp_link(p);
}

@ Now we come to the interesting part, where we reduce the list if necessary
until it has the required size. The |min_cover| routine is basic to this
process; it computes the minimum number~|m| such that the values of the
current sorted list can be covered by |m|~intervals of width~|d|. It
also sets the global value |perturbation| to the smallest value $d'>d$
such that the covering found by this algorithm would be different.

In particular, |min_cover(0)| returns the number of distinct values in the
current list and sets |perturbation| to the minimum distance between
adjacent values.

@c 
static integer mp_min_cover (MP mp,scaled d) {
  pointer p; /* runs through the current list */
  scaled l; /* the least element covered by the current interval */
  integer m; /* lower bound on the size of the minimum cover */
  m=0; p=mp_link(temp_head); mp->perturbation=el_gordo;
  while ( p!=inf_val ){ 
    incr(m); l=value(p);
    do {  p=mp_link(p); } while (value(p)<=l+d);
    if ( value(p)-l<mp->perturbation ) 
      mp->perturbation=value(p)-l;
  }
  return m;
}

@ @<Glob...@>=
scaled perturbation; /* quantity related to \.{TFM} rounding */
integer excess; /* the list is this much too long */

@ The smallest |d| such that a given list can be covered with |m| intervals
is determined by the |threshold| routine, which is sort of an inverse
to |min_cover|. The idea is to increase the interval size rapidly until
finding the range, then to go sequentially until the exact borderline has
been discovered.

@c 
static scaled mp_threshold (MP mp,integer m) {
  scaled d; /* lower bound on the smallest interval size */
  mp->excess=mp_min_cover(mp, 0)-m;
  if ( mp->excess<=0 ) {
    return 0;
  } else  { 
    do {  
      d=mp->perturbation;
    } while (mp_min_cover(mp, d+d)>m);
    while ( mp_min_cover(mp, d)>m ) 
      d=mp->perturbation;
    return d;
  }
}

@ The |skimp| procedure reduces the current list to at most |m| entries,
by changing values if necessary. It also sets |mp_info(p):=k| if |value(p)|
is the |k|th distinct value on the resulting list, and it sets
|perturbation| to the maximum amount by which a |value| field has
been changed. The size of the resulting list is returned as the
value of |skimp|.

@c 
static integer mp_skimp (MP mp,integer m) {
  scaled d; /* the size of intervals being coalesced */
  pointer p,q,r; /* list manipulation registers */
  scaled l; /* the least value in the current interval */
  scaled v; /* a compromise value */
  d=mp_threshold(mp, m); mp->perturbation=0;
  q=temp_head; m=0; p=mp_link(temp_head);
  while ( p!=inf_val ) {
    incr(m); l=value(p); mp_info(p)=m;
    if ( value(mp_link(p))<=l+d ) {
      @<Replace an interval of values by its midpoint@>;
    }
    q=p; p=mp_link(p);
  }
  return m;
}

@ @<Replace an interval...@>=
{ 
  do {  
    p=mp_link(p); mp_info(p)=m;
    decr(mp->excess); if ( mp->excess==0 ) d=0;
  } while (value(mp_link(p))<=l+d);
  v=l+halfp(value(p)-l);
  if ( value(p)-v>mp->perturbation ) 
    mp->perturbation=value(p)-v;
  r=q;
  do {  
    r=mp_link(r); value(r)=v;
  } while (r!=p);
  mp_link(q)=p; /* remove duplicate values from the current list */
}

@ A warning message is issued whenever something is perturbed by
more than 1/16\thinspace pt.

@c 
static void mp_tfm_warning (MP mp,quarterword m) { 
  mp_print_nl(mp, "(some "); 
  mp_print(mp, mp->int_name[m]);
@.some charwds...@>
@.some chardps...@>
@.some charhts...@>
@.some charics...@>
  mp_print(mp, " values had to be adjusted by as much as ");
  mp_print_scaled(mp, mp->perturbation); mp_print(mp, "pt)");
}

@ Here's an example of how we use these routines.
The width data needs to be perturbed only if there are 256 distinct
widths, but \MP\ must check for this case even though it is
highly unusual.

An integer variable |k| will be defined when we use this code.
The |dimen_head| array will contain pointers to the sorted
lists of dimensions.

@<Massage the \.{TFM} widths@>=
clear_the_list;
for (k=mp->bc;k<=mp->ec;k++)  {
  if ( mp->char_exists[k] )
    mp->tfm_width[k]=mp_sort_in(mp, mp->tfm_width[k]);
}
mp->nw=(short)(mp_skimp(mp, 255)+1); mp->dimen_head[1]=mp_link(temp_head);
if ( mp->perturbation>=010000 ) mp_tfm_warning(mp, mp_char_wd)

@ @<Glob...@>=
pointer dimen_head[5]; /* lists of \.{TFM} dimensions */

@ Heights, depths, and italic corrections are different from widths
not only because their list length is more severely restricted, but
also because zero values do not need to be put into the lists.

@<Massage the \.{TFM} heights, depths, and italic corrections@>=
clear_the_list;
for (k=mp->bc;k<=mp->ec;k++) {
  if ( mp->char_exists[k] ) {
    if ( mp->tfm_height[k]==0 ) mp->tfm_height[k]=zero_val;
    else mp->tfm_height[k]=mp_sort_in(mp, mp->tfm_height[k]);
  }
}
mp->nh=(short)(mp_skimp(mp, 15)+1); mp->dimen_head[2]=mp_link(temp_head);
if ( mp->perturbation>=010000 ) mp_tfm_warning(mp, mp_char_ht);
clear_the_list;
for (k=mp->bc;k<=mp->ec;k++) {
  if ( mp->char_exists[k] ) {
    if ( mp->tfm_depth[k]==0 ) mp->tfm_depth[k]=zero_val;
    else mp->tfm_depth[k]=mp_sort_in(mp, mp->tfm_depth[k]);
  }
}
mp->nd=(short)(mp_skimp(mp, 15)+1); mp->dimen_head[3]=mp_link(temp_head);
if ( mp->perturbation>=010000 ) mp_tfm_warning(mp, mp_char_dp);
clear_the_list;
for (k=mp->bc;k<=mp->ec;k++) {
  if ( mp->char_exists[k] ) {
    if ( mp->tfm_ital_corr[k]==0 ) mp->tfm_ital_corr[k]=zero_val;
    else mp->tfm_ital_corr[k]=mp_sort_in(mp, mp->tfm_ital_corr[k]);
  }
}
mp->ni=(short)(mp_skimp(mp, 63)+1); mp->dimen_head[4]=mp_link(temp_head);
if ( mp->perturbation>=010000 ) mp_tfm_warning(mp, mp_char_ic)

@ @<Initialize table entries...@>=
value(zero_val)=0; mp_info(zero_val)=0;

@ Bytes 5--8 of the header are set to the design size, unless the user has
some crazy reason for specifying them differently.
@^design size@>

Error messages are not allowed at the time this procedure is called,
so a warning is printed instead.

The value of |max_tfm_dimen| is calculated so that
$$\hbox{|make_scaled(16*max_tfm_dimen,internal[mp_design_size])|}
 < \\{three\_bytes}.$$

@d three_bytes 0100000000 /* $2^{24}$ */

@c 
static void mp_fix_design_size (MP mp) {
  scaled d; /* the design size */
  d=mp->internal[mp_design_size];
  if ( (d<unity)||(d>=fraction_half) ) {
    if ( d!=0 )
      mp_print_nl(mp, "(illegal design size has been changed to 128pt)");
@.illegal design size...@>
    d=040000000; mp->internal[mp_design_size]=d;
  }
  if ( mp->header_byte[4]==0 && mp->header_byte[5]==0 &&
       mp->header_byte[6]==0 && mp->header_byte[7]==0 ) {
     mp->header_byte[4]=(char)(d / 04000000);
     mp->header_byte[5]=(char)((d / 4096) % 256);
     mp->header_byte[6]=(char)((d / 16) % 256);
     mp->header_byte[7]=(char)((d % 16)*16);
  }
  mp->max_tfm_dimen=16*mp->internal[mp_design_size]-1-mp->internal[mp_design_size] / 010000000;
  if ( mp->max_tfm_dimen>=fraction_half ) mp->max_tfm_dimen=fraction_half-1;
}

@ The |dimen_out| procedure computes a |fix_word| relative to the
design size. If the data was out of range, it is corrected and the
global variable |tfm_changed| is increased by~one.

@c 
static integer mp_dimen_out (MP mp,scaled x) { 
  if ( abs(x)>mp->max_tfm_dimen ) {
    incr(mp->tfm_changed);
    if ( x>0 ) x=mp->max_tfm_dimen; else x=-mp->max_tfm_dimen;
  }
  x=mp_make_scaled(mp, x*16,mp->internal[mp_design_size]);
  return x;
}

@ @<Glob...@>=
scaled max_tfm_dimen; /* bound on widths, heights, kerns, etc. */
integer tfm_changed; /* the number of data entries that were out of bounds */

@ If the user has not specified any of the first four header bytes,
the |fix_check_sum| procedure replaces them by a ``check sum'' computed
from the |tfm_width| data relative to the design size.
@^check sum@>

@c 
static void mp_fix_check_sum (MP mp) {
  eight_bits k; /* runs through character codes */
  eight_bits B1,B2,B3,B4; /* bytes of the check sum */
  integer x;  /* hash value used in check sum computation */
  if ( mp->header_byte[0]==0 && mp->header_byte[1]==0 &&
       mp->header_byte[2]==0 && mp->header_byte[3]==0 ) {
    @<Compute a check sum in |(b1,b2,b3,b4)|@>;
    mp->header_byte[0]=(char)B1; mp->header_byte[1]=(char)B2;
    mp->header_byte[2]=(char)B3; mp->header_byte[3]=(char)B4; 
    return;
  }
}

@ @<Compute a check sum in |(b1,b2,b3,b4)|@>=
B1=mp->bc; B2=mp->ec; B3=mp->bc; B4=mp->ec; mp->tfm_changed=0;
for (k=mp->bc;k<=mp->ec;k++) { 
  if ( mp->char_exists[k] ) {
    x=mp_dimen_out(mp, value(mp->tfm_width[k]))+(k+4)*020000000; /* this is positive */
    B1=(eight_bits)((B1+B1+x) % 255);
    B2=(eight_bits)((B2+B2+x) % 253);
    B3=(eight_bits)((B3+B3+x) % 251);
    B4=(eight_bits)((B4+B4+x) % 247);
  }
  if (k==mp->ec) break;
}

@ Finally we're ready to actually write the \.{TFM} information.
Here are some utility routines for this purpose.

@d tfm_out(A) do { /* output one byte to |tfm_file| */
  unsigned char s=(unsigned char)(A); 
  (mp->write_binary_file)(mp,mp->tfm_file,(void *)&s,1); 
  } while (0)

@c 
static void mp_tfm_two (MP mp,integer x) { /* output two bytes to |tfm_file| */
  tfm_out(x / 256); tfm_out(x % 256);
}
static void mp_tfm_four (MP mp,integer x) { /* output four bytes to |tfm_file| */
  if ( x>=0 ) tfm_out(x / three_bytes);
  else { 
    x=x+010000000000; /* use two's complement for negative values */
    x=x+010000000000;
    tfm_out((x / three_bytes) + 128);
  };
  x=x % three_bytes; tfm_out(x / unity);
  x=x % unity; tfm_out(x / 0400);
  tfm_out(x % 0400);
}
static void mp_tfm_qqqq (MP mp,four_quarters x) { /* output four quarterwords to |tfm_file| */
  tfm_out(qo(x.b0)); tfm_out(qo(x.b1)); 
  tfm_out(qo(x.b2)); tfm_out(qo(x.b3));
}

@ @<Finish the \.{TFM} file@>=
if ( mp->job_name==NULL ) mp_open_log_file(mp);
mp_pack_job_name(mp, ".tfm");
while ( ! mp_b_open_out(mp, &mp->tfm_file, mp_filetype_metrics) )
  mp_prompt_file_name(mp, "file name for font metrics",".tfm");
mp->metric_file_name=xstrdup(mp->name_of_file);
@<Output the subfile sizes and header bytes@>;
@<Output the character information bytes, then
  output the dimensions themselves@>;
@<Output the ligature/kern program@>;
@<Output the extensible character recipes and the font metric parameters@>;
  if ( mp->internal[mp_tracing_stats]>0 )
  @<Log the subfile sizes of the \.{TFM} file@>;
mp_print_nl(mp, "Font metrics written on "); 
mp_print(mp, mp->metric_file_name); mp_print_char(mp, xord('.'));
@.Font metrics written...@>
(mp->close_file)(mp,mp->tfm_file)

@ Integer variables |lh|, |k|, and |lk_offset| will be defined when we use
this code.

@<Output the subfile sizes and header bytes@>=
k=mp->header_last;
LH=(k+4) / 4; /* this is the number of header words */
if ( mp->bc>mp->ec ) mp->bc=1; /* if there are no characters, |ec=0| and |bc=1| */
@<Compute the ligature/kern program offset and implant the
  left boundary label@>;
mp_tfm_two(mp,6+LH+(mp->ec-mp->bc+1)+mp->nw+mp->nh+mp->nd+mp->ni+mp->nl
     +lk_offset+mp->nk+mp->ne+mp->np);
  /* this is the total number of file words that will be output */
mp_tfm_two(mp, LH); mp_tfm_two(mp, mp->bc); mp_tfm_two(mp, mp->ec); 
mp_tfm_two(mp, mp->nw); mp_tfm_two(mp, mp->nh);
mp_tfm_two(mp, mp->nd); mp_tfm_two(mp, mp->ni); mp_tfm_two(mp, mp->nl+lk_offset); 
mp_tfm_two(mp, mp->nk); mp_tfm_two(mp, mp->ne);
mp_tfm_two(mp, mp->np);
for (k=0;k< 4*LH;k++)   { 
  tfm_out(mp->header_byte[k]);
}

@ @<Output the character information bytes...@>=
for (k=mp->bc;k<=mp->ec;k++) {
  if ( ! mp->char_exists[k] ) {
    mp_tfm_four(mp, 0);
  } else { 
    tfm_out(mp_info(mp->tfm_width[k])); /* the width index */
    tfm_out((mp_info(mp->tfm_height[k]))*16+mp_info(mp->tfm_depth[k]));
    tfm_out((mp_info(mp->tfm_ital_corr[k]))*4+mp->char_tag[k]);
    tfm_out(mp->char_remainder[k]);
  };
}
mp->tfm_changed=0;
for (k=1;k<=4;k++) { 
  mp_tfm_four(mp, 0); p=mp->dimen_head[k];
  while ( p!=inf_val ) {
    mp_tfm_four(mp, mp_dimen_out(mp, value(p))); p=mp_link(p);
  }
}


@ We need to output special instructions at the beginning of the
|lig_kern| array in order to specify the right boundary character
and/or to handle starting addresses that exceed 255. The |label_loc|
and |label_char| arrays have been set up to record all the
starting addresses; we have $-1=|label_loc|[0]<|label_loc|[1]\le\cdots
\le|label_loc|[|label_ptr]|$.

@<Compute the ligature/kern program offset...@>=
mp->bchar=mp_round_unscaled(mp, mp->internal[mp_boundary_char]);
if ((mp->bchar<0)||(mp->bchar>255)) { 
  mp->bchar=-1; mp->lk_started=false; lk_offset=0; 
} else { 
  mp->lk_started=true; lk_offset=1; 
}
@<Find the minimum |lk_offset| and adjust all remainders@>;
if ( mp->bch_label<undefined_label )
  { skip_byte(mp->nl)=qi(255); next_char(mp->nl)=qi(0);
  op_byte(mp->nl)=qi(((mp->bch_label+lk_offset)/ 256));
  rem_byte(mp->nl)=qi(((mp->bch_label+lk_offset)% 256));
  mp->nl++; /* possibly |nl=lig_table_size+1| */
  }

@ @<Find the minimum |lk_offset|...@>=
k=mp->label_ptr; /* pointer to the largest unallocated label */
if ( mp->label_loc[k]+lk_offset>255 ) {
  lk_offset=0; mp->lk_started=false; /* location 0 can do double duty */
  do {  
    mp->char_remainder[mp->label_char[k]]=lk_offset;
    while ( mp->label_loc[k-1]==mp->label_loc[k] ) {
       decr(k); mp->char_remainder[mp->label_char[k]]=lk_offset;
    }
    incr(lk_offset); decr(k);
  } while (! (lk_offset+mp->label_loc[k]<256));
    /* N.B.: |lk_offset=256| satisfies this when |k=0| */
}
if ( lk_offset>0 ) {
  while ( k>0 ) {
    mp->char_remainder[mp->label_char[k]]
     =mp->char_remainder[mp->label_char[k]]+lk_offset;
    decr(k);
  }
}

@ @<Output the ligature/kern program@>=
for (k=0;k<= 255;k++ ) {
  if ( mp->skip_table[k]<undefined_label ) {
     mp_print_nl(mp, "(local label "); mp_print_int(mp, k); mp_print(mp, ":: was missing)");
@.local label l:: was missing@>
    cancel_skips(mp->skip_table[k]);
  }
}
if ( mp->lk_started ) { /* |lk_offset=1| for the special |bchar| */
  tfm_out(255); tfm_out(mp->bchar); mp_tfm_two(mp, 0);
} else {
  for (k=1;k<=lk_offset;k++) {/* output the redirection specs */
    mp->ll=mp->label_loc[mp->label_ptr];
    if ( mp->bchar<0 ) { tfm_out(254); tfm_out(0);   }
    else { tfm_out(255); tfm_out(mp->bchar);   };
    mp_tfm_two(mp, mp->ll+lk_offset);
    do {  
      mp->label_ptr--;
    } while (! (mp->label_loc[mp->label_ptr]<mp->ll));
  }
}
for (k=0;k<mp->nl;k++) mp_tfm_qqqq(mp, mp->lig_kern[k]);
for (k=0;k<mp->nk;k++) mp_tfm_four(mp, mp_dimen_out(mp, mp->kern[k]))

@ @<Output the extensible character recipes...@>=
for (k=0;k<mp->ne;k++) 
  mp_tfm_qqqq(mp, mp->exten[k]);
for (k=1;k<=mp->np;k++) {
  if ( k==1 ) {
    if ( abs(mp->param[1])<fraction_half ) {
      mp_tfm_four(mp, mp->param[1]*16);
    } else  { 
      incr(mp->tfm_changed);
      if ( mp->param[1]>0 ) mp_tfm_four(mp, el_gordo);
      else mp_tfm_four(mp, -el_gordo);
    }
  } else {
    mp_tfm_four(mp, mp_dimen_out(mp, mp->param[k]));
  }
}
if ( mp->tfm_changed>0 )  { 
  if ( mp->tfm_changed==1 ) mp_print_nl(mp, "(a font metric dimension");
@.a font metric dimension...@>
  else  { 
    mp_print_nl(mp, "("); mp_print_int(mp, mp->tfm_changed);
@.font metric dimensions...@>
    mp_print(mp, " font metric dimensions");
  }
  mp_print(mp, " had to be decreased)");
}

@ @<Log the subfile sizes of the \.{TFM} file@>=
{ 
  char s[200];
  wlog_ln(" ");
  if ( mp->bch_label<undefined_label ) mp->nl--;
  mp_snprintf(s,128,"(You used %iw,%ih,%id,%ii,%il,%ik,%ie,%ip metric file positions)",
                 mp->nw, mp->nh, mp->nd, mp->ni, mp->nl, mp->nk, mp->ne, mp->np);
  wlog_ln(s);
}

@* \[43] Reading font metric data.

\MP\ isn't a typesetting program but it does need to find the bounding box
of a sequence of typeset characters.  Thus it needs to read \.{TFM} files as
well as write them.

@<Glob...@>=
void * tfm_infile;

@ All the width, height, and depth information is stored in an array called
|font_info|.  This array is allocated sequentially and each font is stored
as a series of |char_info| words followed by the width, height, and depth
tables.  Since |font_name| entries are permanent, their |str_ref| values are
set to |max_str_ref|.

@<Types...@>=
typedef unsigned int font_number; /* |0..font_max| */

@ The |font_info| array is indexed via a group directory arrays.
For example, the |char_info| data for character~|c| in font~|f| will be
in |font_info[char_base[f]+c].qqqq|.

@<Glob...@>=
font_number font_max; /* maximum font number for included text fonts */
size_t      font_mem_size; /* number of words for \.{TFM} information for text fonts */
memory_word *font_info; /* height, width, and depth data */
char        **font_enc_name; /* encoding names, if any */
boolean     *font_ps_name_fixed; /* are the postscript names fixed already?  */
size_t      next_fmem; /* next unused entry in |font_info| */
font_number last_fnum; /* last font number used so far */
scaled      *font_dsize;  /* 16 times the ``design'' size in \ps\ points */
char        **font_name;  /* name as specified in the \&{infont} command */
char        **font_ps_name;  /* PostScript name for use when |internal[mp_prologues]>0| */
font_number last_ps_fnum; /* last valid |font_ps_name| index */
eight_bits  *font_bc;
eight_bits  *font_ec;  /* first and last character code */
int         *char_base;  /* base address for |char_info| */
int         *width_base; /* index for zeroth character width */
int         *height_base; /* index for zeroth character height */
int         *depth_base; /* index for zeroth character depth */
pointer     *font_sizes;

@ @<Allocate or initialize ...@>=
mp->font_mem_size = 10000; 
mp->font_info = xmalloc ((mp->font_mem_size+1),sizeof(memory_word));
memset (mp->font_info,0,sizeof(memory_word)*(mp->font_mem_size+1));
mp->last_fnum = null_font;

@ @<Dealloc variables@>=
for (k=1;k<=(int)mp->last_fnum;k++) {
  xfree(mp->font_enc_name[k]);
  xfree(mp->font_name[k]);
  xfree(mp->font_ps_name[k]);
}
xfree(mp->font_info);
xfree(mp->font_enc_name);
xfree(mp->font_ps_name_fixed);
xfree(mp->font_dsize);
xfree(mp->font_name);
xfree(mp->font_ps_name);
xfree(mp->font_bc);
xfree(mp->font_ec);
xfree(mp->char_base);
xfree(mp->width_base);
xfree(mp->height_base);
xfree(mp->depth_base);
xfree(mp->font_sizes);

@ 
@c 
void mp_reallocate_fonts (MP mp, font_number l) {
  font_number f;
  XREALLOC(mp->font_enc_name,      l, char *);
  XREALLOC(mp->font_ps_name_fixed, l, boolean);
  XREALLOC(mp->font_dsize,         l, scaled);
  XREALLOC(mp->font_name,          l, char *);
  XREALLOC(mp->font_ps_name,       l, char *);
  XREALLOC(mp->font_bc,            l, eight_bits);
  XREALLOC(mp->font_ec,            l, eight_bits);
  XREALLOC(mp->char_base,          l, int);
  XREALLOC(mp->width_base,         l, int);
  XREALLOC(mp->height_base,        l, int);
  XREALLOC(mp->depth_base,         l, int);
  XREALLOC(mp->font_sizes,         l, pointer);
  for (f=(mp->last_fnum+1);f<=l;f++) {
    mp->font_enc_name[f]=NULL;
    mp->font_ps_name_fixed[f] = false;
    mp->font_name[f]=NULL;
    mp->font_ps_name[f]=NULL;
    mp->font_sizes[f]=null;
  }
  mp->font_max = l;
}

@ @<Internal library declarations@>=
void mp_reallocate_fonts (MP mp, font_number l);


@ A |null_font| containing no characters is useful for error recovery.  Its
|font_name| entry starts out empty but is reset each time an erroneous font is
found.  This helps to cut down on the number of duplicate error messages without
wasting a lot of space.

@d null_font 0 /* the |font_number| for an empty font */

@<Set initial...@>=
mp->font_dsize[null_font]=0;
mp->font_bc[null_font]=1;
mp->font_ec[null_font]=0;
mp->char_base[null_font]=0;
mp->width_base[null_font]=0;
mp->height_base[null_font]=0;
mp->depth_base[null_font]=0;
mp->next_fmem=0;
mp->last_fnum=null_font;
mp->last_ps_fnum=null_font;
mp->font_name[null_font]=mp_xstrdup(mp,"nullfont");
mp->font_ps_name[null_font]=mp_xstrdup(mp,"");
mp->font_ps_name_fixed[null_font] = false;
mp->font_enc_name[null_font]=NULL;
mp->font_sizes[null_font]=null;

@ Each |char_info| word is of type |four_quarters|.  The |b0| field contains
the |width index|; the |b1| field contains the height
index; the |b2| fields contains the depth index, and the |b3| field used only
for temporary storage. (It is used to keep track of which characters occur in
an edge structure that is being shipped out.)
The corresponding words in the width, height, and depth tables are stored as
|scaled| values in units of \ps\ points.

With the macros below, the |char_info| word for character~|c| in font~|f| is
|char_mp_info(f,c)| and the width is
$$\hbox{|char_width(f,char_mp_info(f,c)).sc|.}$$

@d char_mp_info(A,B) mp->font_info[mp->char_base[(A)]+(B)].qqqq
@d char_width(A,B) mp->font_info[mp->width_base[(A)]+(B).b0].sc
@d char_height(A,B) mp->font_info[mp->height_base[(A)]+(B).b1].sc
@d char_depth(A,B) mp->font_info[mp->depth_base[(A)]+(B).b2].sc
@d ichar_exists(A) ((A).b0>0)

@ When we have a font name and we don't know whether it has been loaded yet,
we scan the |font_name| array before calling |read_font_info|.

@<Declarations@>=
static font_number mp_find_font (MP mp, char *f) ;

@ @c
font_number mp_find_font (MP mp, char *f) {
  font_number n;
  for (n=0;n<=mp->last_fnum;n++) {
    if (mp_xstrcmp(f,mp->font_name[n])==0 ) {
      mp_xfree(f);
      return n;
    }
  }
  n = mp_read_font_info(mp, f);
  mp_xfree(f);
  return n;
}

@ This is an interface function for getting the width of character,
as a double in ps units

@c double mp_get_char_dimension (MP mp, const char *fname, int c, int t) {
  unsigned n;
  four_quarters cc;
  font_number f = 0;
  double w = -1.0;
  for (n=0;n<=mp->last_fnum;n++) {
    if (mp_xstrcmp(fname,mp->font_name[n])==0 ) {
      f = n;
      break;
    }
  }
  if (f==0)
    return 0.0;
  cc = char_mp_info(f,c);
  if (! ichar_exists(cc) )
    return 0.0;
  if (t=='w')
    w = (double)char_width(f,cc);
  else if (t=='h')
    w = (double)char_height(f,cc);
  else if (t=='d')
    w = (double)char_depth(f,cc);
  return w/655.35*(72.27/72);
}

@ @<Exported function ...@>=
double mp_get_char_dimension (MP mp, const char *fname, int n, int t);


@ One simple application of |find_font| is the implementation of the |font_size|
operator that gets the design size for a given font name.

@<Find the design size of the font whose name is |cur_exp|@>=
mp_flush_cur_exp(mp, (mp->font_dsize[mp_find_font(mp, str(mp->cur_exp))]+8) / 16)

@ If we discover that the font doesn't have a requested character, we omit it
from the bounding box computation and expect the \ps\ interpreter to drop it.
This routine issues a warning message if the user has asked for it.

@<Declarations@>=
static void mp_lost_warning (MP mp,font_number f, pool_pointer k);

@ @c 
void mp_lost_warning (MP mp,font_number f, pool_pointer k) { 
  if ( mp->internal[mp_tracing_lost_chars]>0 ) { 
    mp_begin_diagnostic(mp);
    if ( mp->selector==log_only ) incr(mp->selector);
    mp_print_nl(mp, "Missing character: There is no ");
@.Missing character@>
    mp_print_str(mp, mp->str_pool[k]); 
    mp_print(mp, " in font ");
    mp_print(mp, mp->font_name[f]); mp_print_char(mp, xord('!')); 
    mp_end_diagnostic(mp, false);
  }
}

@ The whole purpose of saving the height, width, and depth information is to be
able to find the bounding box of an item of text in an edge structure.  The
|set_text_box| procedure takes a text node and adds this information.

@<Declarations@>=
static void mp_set_text_box (MP mp,pointer p); 

@ @c 
void mp_set_text_box (MP mp,pointer p) {
  font_number f; /* |mp_font_n(p)| */
  ASCII_code bc,ec; /* range of valid characters for font |f| */
  pool_pointer k,kk; /* current character and character to stop at */
  four_quarters cc; /* the |char_info| for the current character */
  scaled h,d; /* dimensions of the current character */
  width_val(p)=0;
  height_val(p)=-el_gordo;
  depth_val(p)=-el_gordo;
  f=(font_number)mp_font_n(p);
  bc=mp->font_bc[f];
  ec=mp->font_ec[f];
  kk=str_stop(mp_text_p(p));
  k=mp->str_start[mp_text_p(p)];
  while ( k<kk ) {
    @<Adjust |p|'s bounding box to contain |str_pool[k]|; advance |k|@>;
  }
  @<Set the height and depth to zero if the bounding box is empty@>;
}

@ @<Adjust |p|'s bounding box to contain |str_pool[k]|; advance |k|@>=
{ 
  if ( (mp->str_pool[k]<bc)||(mp->str_pool[k]>ec) ) {
    mp_lost_warning(mp, f,k);
  } else { 
    cc=char_mp_info(f,mp->str_pool[k]);
    if ( ! ichar_exists(cc) ) {
      mp_lost_warning(mp, f,k);
    } else { 
      width_val(p)=width_val(p)+char_width(f,cc);
      h=char_height(f,cc);
      d=char_depth(f,cc);
      if ( h>height_val(p) ) height_val(p)=h;
      if ( d>depth_val(p) ) depth_val(p)=d;
    }
  }
  incr(k);
}

@ Let's hope modern compilers do comparisons correctly when the difference would
overflow.

@<Set the height and depth to zero if the bounding box is empty@>=
if ( height_val(p)<-depth_val(p) ) { 
  height_val(p)=0;
  depth_val(p)=0;
}

@ The new primitives fontmapfile and fontmapline.

@<Declare action procedures for use by |do_statement|@>=
static void mp_do_mapfile (MP mp) ;
static void mp_do_mapline (MP mp) ;

@ @c 
static void mp_do_mapfile (MP mp) { 
  mp_get_x_next(mp); mp_scan_expression(mp);
  if ( mp->cur_type!=mp_string_type ) {
    @<Complain about improper map operation@>;
  } else {
    mp_map_file(mp,mp->cur_exp);
  }
}
static void mp_do_mapline (MP mp) { 
  mp_get_x_next(mp); mp_scan_expression(mp);
  if ( mp->cur_type!=mp_string_type ) {
     @<Complain about improper map operation@>;
  } else { 
     mp_map_line(mp,mp->cur_exp);
  }
}

@ @<Complain about improper map operation@>=
{ 
  exp_err("Unsuitable expression");
  help1("Only known strings can be map files or map lines.");
  mp_put_get_error(mp);
}

@ To print |scaled| value to PDF output we need some subroutines to ensure
accurary.

@d max_integer   0x7FFFFFFF /* $2^{31}-1$ */

@<Glob...@>=
scaled one_bp; /* scaled value corresponds to 1bp */
scaled one_hundred_bp; /* scaled value corresponds to 100bp */
scaled one_hundred_inch; /* scaled value corresponds to 100in */
integer ten_pow[10]; /* $10^0..10^9$ */
integer scaled_out; /* amount of |scaled| that was taken out in |divide_scaled| */

@ @<Set init...@>=
mp->one_bp = 65782; /* 65781.76 */
mp->one_hundred_bp = 6578176;
mp->one_hundred_inch = 473628672;
mp->ten_pow[0] = 1;
for (i = 1;i<= 9; i++ ) {
  mp->ten_pow[i] = 10*mp->ten_pow[i - 1];
}

@ The following function divides |s| by |m|. |dd| is number of decimal digits.

@c 
#if 0
scaled mp_divide_scaled (MP mp,scaled s, scaled m, integer  dd) {
  scaled q,r;
  integer sign,i;
  sign = 1;
  if ( s < 0 ) { sign = -sign; s = -s; }
  if ( m < 0 ) { sign = -sign; m = -m; }
  if ( m == 0 )
    mp_confusion(mp, "arithmetic: divided by zero");
  else if ( m >= (max_integer / 10) )
    mp_confusion(mp, "arithmetic: number too big");
  q = s / m;
  r = s % m;
  for (i = 1;i<=dd;i++) {
    q = 10*q + (10*r) / m;
    r = (10*r) % m;
  }
  if ( 2*r >= m ) { incr(q); r = r - m; }
  mp->scaled_out = sign*(s - (r / mp->ten_pow[dd]));
  return (sign*q);
}
#endif

@* \[44] Shipping pictures out.
The |ship_out| procedure, to be described below, is given a pointer to
an edge structure. Its mission is to output a file containing the \ps\
description of an edge structure.

@ Each time an edge structure is shipped out we write a new \ps\ output
file named according to the current \&{charcode}.
@:char_code_}{\&{charcode} primitive@>

This is the only backend function that remains in the main |mpost.w| file. 
There are just too many variable accesses needed for status reporting 
etcetera to make it worthwile to move the code to |psout.w|.

@<Internal library declarations@>=
void mp_open_output_file (MP mp) ;

@ @c 
static void mp_append_to_template (MP mp, integer ff, integer c) {
  if (mp->int_type[c]==mp_string_type) {
    char *ss = str(mp->internal[c]);
    mp_print(mp,ss);
    mp_xfree(ss);
  } else if (mp->int_type[c]==mp_known) {
    integer cc = mp_round_unscaled(mp, mp->internal[c]);
    print_with_leading_zeroes(cc, ff);
  }
}
static char *mp_set_output_file_name (MP mp, integer c) {
  char *ss = NULL; /* filename extension proposal */  
  char *nn = NULL; /* temp string  for str() */
  unsigned old_setting; /* previous |selector| setting */
  pool_pointer i; /*  indexes into |filename_template|  */
  integer f; /* field width */
  str_room(1024); /* have to prevent reallocation of the
               	     string pool during this template processing */
  if ( mp->job_name==NULL ) mp_open_log_file(mp);
  if ( mp->internal[mp_output_template]==0) { 
    char *s; /* a file extension derived from |c| */
    if ( c<0 ) 
      s=xstrdup(".ps");
    else 
      @<Use |c| to compute the file extension |s|@>;
    mp_pack_job_name(mp, s);
    free(s);
    ss = xstrdup(mp->name_of_file);
  } else { /* initializations */
    str_number s, n; /* a file extension derived from |c| */
    scaled saved_char_code = mp->internal[mp_char_code];
    mp->internal[mp_char_code] = (c*unity);
    if (mp->internal[mp_job_name]==0) {
      if ( mp->job_name==NULL ) {
         mp->job_name=xstrdup("mpout");
      }
      @<Fix up |mp->internal[mp_job_name]|@>;
    }
    old_setting=mp->selector; 
    mp->selector=new_string;
    i = mp->str_start[mp->internal[mp_output_template]];
    n = null_str; /* initialize */
    while ( i<str_stop(mp->internal[mp_output_template]) ) {
       f=0;
       if ( mp->str_pool[i]=='%' ) {
      CONTINUE:
        incr(i);
        if ( i<str_stop(mp->internal[mp_output_template]) ) {
          switch (mp->str_pool[i]) {
	  case 'j':
 	    mp_append_to_template(mp,f,mp_job_name); 
	    break;
	  case 'c': 
            if (mp->internal[mp_char_code]<0) {
                mp_print(mp,"ps");
            } else {
                mp_append_to_template(mp,f,mp_char_code); 
            }
            break;
	  case 'o': 
	    mp_append_to_template(mp,f,mp_output_format); 
            break;
	  case 'd': 
	    mp_append_to_template(mp,f,mp_day); 
            break;
	  case 'm': 
	    mp_append_to_template(mp,f,mp_month); 
            break;
	  case 'y': 
	    mp_append_to_template(mp,f,mp_year); 
            break;
	  case 'H': 
	    mp_append_to_template(mp,f,mp_hour); 
            break;
	  case 'M': 
	    mp_append_to_template(mp,f,mp_minute); 
            break;
          case '{':
	    {
	      /* look up a name */
              integer l=0;
              integer frst = i+1;
	      while ( i<str_stop(mp->internal[mp_output_template]) ) {
                i++;
	        if (mp->str_pool[i] == '}')
                  break;
                l++;
              }
              if (l>0) {
                integer h = mp_compute_hash(mp, (char *)(mp->str_pool+frst),l);
                pointer p=h+hash_base; /* we start searching here */
	        char *id = xmalloc(mp, (size_t)(l+1));
                (void)memcpy(id,(char *)(mp->str_pool+frst),(size_t)l);
	        *(id+l)=0;
	        while (true)  { 
	     	  if (text(p)>0 && length(text(p))==l && 
	              mp_str_eq_cstr(mp, text(p),id)) {
                    if (eq_type(p)==internal_quantity) {
		      if (equiv(p)==mp_output_template) {
    		        char err[256];
                        mp_snprintf(err,256,
                           "The appearance of outputtemplate inside outputtemplate is ignored.");
                        mp_warn(mp,err);
                      } else {
	         	mp_append_to_template(mp,f,equiv(p)); 
	              }
                    } else {
		      char err[256];
                      mp_snprintf(err,256,
                       "requested identifier (%s) in outputtemplate is not an internal.",id);
                      mp_warn(mp,err);
                    }
                    break;
                  }
                  if ( mp_next(p)==0 ) {
                    char err[256];
                    mp_snprintf(err,256,
                      "requested identifier (%s) in outputtemplate not found.",id);
                    mp_warn(mp,err);
                    break;
                  }
                  p=mp_next(p);
                }
                free(id);
              }
            }
	    break;
	  case '0': case '1': case '2': case '3': case '4': 
	  case '5': case '6': case '7': case '8': case '9':
            if ( (f<10)  )
              f = (f*10) + mp->str_pool[i]-'0';
            goto CONTINUE;
	    break;
          case '%':
            mp_print_str(mp, mp->str_pool[i]);
	    break;
          default:
            {
              char err[256];
              mp_snprintf(err,256,
                "requested format (%c) in outputtemplate is unknown.",mp->str_pool[i]);
              mp_warn(mp,err);
            }
            mp_print_str(mp, mp->str_pool[i]);
          }
        }
      } else {
        if ( mp->str_pool[i]=='.' )
          if (length(n)==0)
            n = mp_make_string(mp);
        mp_print_str(mp, mp->str_pool[i]);
      };
      incr(i);
    }
    s = mp_make_string(mp);
    mp->internal[mp_char_code] = saved_char_code;
    mp->selector= old_setting;
    if (length(n)==0) {
       n=s;
       s=null_str;
    }
    ss = str(s);
    nn = str(n);
    mp_pack_file_name(mp, nn,"",ss);
    mp_xfree(nn);
    delete_str_ref(n);
    delete_str_ref(s);
  }
  return ss;
}

static char * mp_get_output_file_name (MP mp) {
  char *f;
  char *saved_name;  /* saved |name_of_file| */
  saved_name = xstrdup(mp->name_of_file);
  (void)mp_set_output_file_name(mp, mp_round_unscaled(mp, mp->internal[mp_char_code]));
  f = xstrdup(mp->name_of_file);
  mp_pack_file_name(mp, saved_name,NULL,NULL);
  free(saved_name);
  return f;
}

void mp_open_output_file (MP mp) {
  char *ss; /* filename extension proposal */
  integer c; /* \&{charcode} rounded to the nearest integer */
  c=mp_round_unscaled(mp, mp->internal[mp_char_code]);
  ss = mp_set_output_file_name(mp, c);
  while ( ! mp_a_open_out(mp, (void *)&mp->output_file, mp_filetype_postscript) )
    mp_prompt_file_name(mp, "file name for output",ss);
  xfree(ss);
  @<Store the true output file name if appropriate@>;
}

@ The file extension created here could be up to five characters long in
extreme cases so it may have to be shortened on some systems.
@^system dependencies@>

@<Use |c| to compute the file extension |s|@>=
{ 
  s = xmalloc(7,1);
  mp_snprintf(s,7,".%i",(int)c);
}

@ The user won't want to see all the output file names so we only save the
first and last ones and a count of how many there were.  For this purpose
files are ordered primarily by \&{charcode} and secondarily by order of
creation.
@:char_code_}{\&{charcode} primitive@>

@<Store the true output file name if appropriate@>=
if ((c<mp->first_output_code)&&(mp->first_output_code>=0)) {
  mp->first_output_code=c;
  xfree(mp->first_file_name);
  mp->first_file_name=xstrdup(mp->name_of_file);
}
if ( c>=mp->last_output_code ) {
  mp->last_output_code=c;
  xfree(mp->last_file_name);
  mp->last_file_name=xstrdup(mp->name_of_file);
}

@ @<Glob...@>=
char * first_file_name;
char * last_file_name; /* full file names */
integer first_output_code;integer last_output_code; /* rounded \&{charcode} values */
@:char_code_}{\&{charcode} primitive@>
integer total_shipped; /* total number of |ship_out| operations completed */

@ @<Set init...@>=
mp->first_file_name=xstrdup("");
mp->last_file_name=xstrdup("");
mp->first_output_code=32768;
mp->last_output_code=-32768;
mp->total_shipped=0;

@ @<Dealloc variables@>=
xfree(mp->first_file_name);
xfree(mp->last_file_name);

@ @<Begin the progress report for the output of picture~|c|@>=
if ( (int)mp->term_offset>mp->max_print_line-6 ) mp_print_ln(mp);
else if ( (mp->term_offset>0)||(mp->file_offset>0) ) mp_print_char(mp, xord(' '));
mp_print_char(mp, xord('['));
if ( c>=0 ) mp_print_int(mp, c)

@ @<End progress report@>=
mp_print_char(mp, xord(']'));
update_terminal;
incr(mp->total_shipped)

@ @<Explain what output files were written@>=
if ( mp->total_shipped>0 ) { 
  mp_print_nl(mp, "");
  mp_print_int(mp, mp->total_shipped);
  if (mp->noninteractive) {
    mp_print(mp, " figure");
    if ( mp->total_shipped>1 ) mp_print_char(mp, xord('s'));
    mp_print(mp, " created.");
  } else {
    mp_print(mp, " output file");
    if ( mp->total_shipped>1 ) mp_print_char(mp, xord('s'));
    mp_print(mp, " written: ");
    mp_print(mp, mp->first_file_name);
    if ( mp->total_shipped>1 ) {
      if ( 31+strlen(mp->first_file_name)+
         strlen(mp->last_file_name)> (unsigned)mp->max_print_line) 
        mp_print_ln(mp);
      mp_print(mp, " .. ");
      mp_print(mp, mp->last_file_name);
    }
  }
}

@ @<Internal library declarations@>=
boolean mp_has_font_size(MP mp, font_number f );

@ @c 
boolean mp_has_font_size(MP mp, font_number f ) {
  return (mp->font_sizes[f]!=null);
}

@ The \&{special} command saves up lines of text to be printed during the next
|ship_out| operation.  The saved items are stored as a list of capsule tokens.

@<Glob...@>=
pointer last_pending; /* the last token in a list of pending specials */

@ @<Set init...@>=
mp->last_pending=spec_head;

@ @<Cases of |do_statement|...@>=
case special_command: 
  if ( mp->cur_mod==0 ) mp_do_special(mp); else 
  if ( mp->cur_mod==1 ) mp_do_mapfile(mp); else 
  mp_do_mapline(mp);
  break;

@ @<Declare action procedures for use by |do_statement|@>=
static void mp_do_special (MP mp) ;

@ @c void mp_do_special (MP mp) { 
  mp_get_x_next(mp); mp_scan_expression(mp);
  if ( mp->cur_type!=mp_string_type ) {
    @<Complain about improper special operation@>;
  } else { 
    mp_link(mp->last_pending)=mp_stash_cur_exp(mp);
    mp->last_pending=mp_link(mp->last_pending);
    mp_link(mp->last_pending)=null;
  }
}

@ @<Complain about improper special operation@>=
{ 
  exp_err("Unsuitable expression");
  help1("Only known strings are allowed for output as specials.");
  mp_put_get_error(mp);
}

@ On the export side, we need an extra object type for special strings.

@<Graphical object codes@>=
mp_special_code=8, 

@ @<Export pending specials@>=
p=mp_link(spec_head);
while ( p!=null ) {
  mp_special_object *tp;
  tp = (mp_special_object *)mp_new_graphic_object(mp,mp_special_code);  
  gr_pre_script(tp)  = str(value(p));
  if (hh->body==NULL) hh->body = (mp_graphic_object *)tp; 
  else gr_link(hp) = (mp_graphic_object *)tp;
  hp = (mp_graphic_object *)tp;
  p=mp_link(p);
}
mp_flush_token_list(mp, mp_link(spec_head));
mp_link(spec_head)=null;
mp->last_pending=spec_head

@ We are now ready for the main output procedure.  Note that the |selector|
setting is saved in a global variable so that |begin_diagnostic| can access it.

@<Declare the \ps\ output procedures@>=
static void mp_ship_out (MP mp, pointer h) ;

@ Once again, the |gr_XXXX| macros are defined in |mppsout.h|

@d export_color(q,p) 
  if ( mp_color_model(p)==mp_uninitialized_model ) {
    gr_color_model(q)  = (unsigned char)(mp->internal[mp_default_color_model]/65536);
    gr_cyan_val(q)     = 0;
	gr_magenta_val(q)  = 0;
	gr_yellow_val(q)   = 0;
	gr_black_val(q)    = (gr_color_model(q)==mp_cmyk_model ? unity : 0);
  } else {
    gr_color_model(q)  = (unsigned char)mp_color_model(p);
    gr_cyan_val(q)     = cyan_val(p);
    gr_magenta_val(q)  = magenta_val(p);
    gr_yellow_val(q)   = yellow_val(p);
    gr_black_val(q)    = black_val(p);
  }

@d export_scripts(q,p)
  if (mp_pre_script(p)!=null)  gr_pre_script(q)   = str(mp_pre_script(p));
  if (mp_post_script(p)!=null) gr_post_script(q)  = str(mp_post_script(p));

@c
struct mp_edge_object *mp_gr_export(MP mp, pointer h) {
  pointer p; /* the current graphical object */
  integer t; /* a temporary value */
  integer c; /* a rounded charcode */
  scaled d_width; /* the current pen width */
  mp_edge_object *hh; /* the first graphical object */
  mp_graphic_object *hq; /* something |hp| points to  */
  mp_text_object    *tt;
  mp_fill_object    *tf;
  mp_stroked_object *ts;
  mp_clip_object    *tc;
  mp_bounds_object  *tb;
  mp_graphic_object *hp = NULL; /* the current graphical object */
  mp_set_bbox(mp, h, true);
  hh = xmalloc(1,sizeof(mp_edge_object));
  hh->body = NULL;
  hh->next = NULL;
  hh->parent = mp;
  hh->minx = minx_val(h);
  hh->miny = miny_val(h);
  hh->maxx = maxx_val(h);
  hh->maxy = maxy_val(h);
  hh->filename = mp_get_output_file_name(mp);
  c = mp_round_unscaled(mp,mp->internal[mp_char_code]);
  hh->charcode = c;
  hh->width = mp->internal[mp_char_wd];
  hh->height = mp->internal[mp_char_ht];
  hh->depth = mp->internal[mp_char_dp];
  hh->ital_corr = mp->internal[mp_char_ic];
  @<Export pending specials@>;
  p=mp_link(dummy_loc(h));
  while ( p!=null ) { 
    hq = mp_new_graphic_object(mp,mp_type(p));
    switch (mp_type(p)) {
    case mp_fill_code:
      tf = (mp_fill_object *)hq;
      gr_pen_p(tf)        = mp_export_knot_list(mp,mp_pen_p(p));
      d_width = mp_get_pen_scale(mp, mp_pen_p(p));
      if ((mp_pen_p(p)==null) || pen_is_elliptical(mp_pen_p(p)))  {
  	    gr_path_p(tf)       = mp_export_knot_list(mp,mp_path_p(p));
      } else {
        pointer pc, pp;
        pc = mp_copy_path(mp, mp_path_p(p));
        pp = mp_make_envelope(mp, pc, mp_pen_p(p),ljoin_val(p),0,miterlim_val(p));
        gr_path_p(tf)       = mp_export_knot_list(mp,pp);
        mp_toss_knot_list(mp, pp);
        pc = mp_htap_ypoc(mp, mp_path_p(p));
        pp = mp_make_envelope(mp, pc, mp_pen_p(p),ljoin_val(p),0,miterlim_val(p));
        gr_htap_p(tf)       = mp_export_knot_list(mp,pp);
        mp_toss_knot_list(mp, pp);
      }
      export_color(tf,p) ;
      export_scripts(tf,p);
      gr_ljoin_val(tf)    = (unsigned char)ljoin_val(p);
      gr_miterlim_val(tf) = miterlim_val(p);
      break;
    case mp_stroked_code:
      ts = (mp_stroked_object *)hq;
      gr_pen_p(ts)        = mp_export_knot_list(mp,mp_pen_p(p));
      d_width = mp_get_pen_scale(mp, mp_pen_p(p));
      if (pen_is_elliptical(mp_pen_p(p)))  {
	      gr_path_p(ts)       = mp_export_knot_list(mp,mp_path_p(p));
      } else {
        pointer pc;
        pc=mp_copy_path(mp, mp_path_p(p));
        t=lcap_val(p);
        if ( mp_left_type(pc)!=mp_endpoint ) { 
          mp_left_type(mp_insert_knot(mp, pc,mp_x_coord(pc),mp_y_coord(pc)))=mp_endpoint;
          mp_right_type(pc)=mp_endpoint;
          pc=mp_link(pc);
          t=1;
        }
        pc=mp_make_envelope(mp,pc,mp_pen_p(p),ljoin_val(p),(quarterword)t,miterlim_val(p));
        gr_path_p(ts)       = mp_export_knot_list(mp,pc);
        mp_toss_knot_list(mp, pc);
      }
      export_color(ts,p) ;
      export_scripts(ts,p);
      gr_ljoin_val(ts)    = (unsigned char)ljoin_val(p);
      gr_miterlim_val(ts) = miterlim_val(p);
      gr_lcap_val(ts)     = (unsigned char)lcap_val(p);
      gr_dash_p(ts)       = mp_export_dashes(mp,p,&d_width);
      break;
    case mp_text_code:
      tt = (mp_text_object *)hq;
      gr_text_p(tt)       = str(mp_text_p(p));
      gr_text_l(tt)       = (size_t)length(mp_text_p(p));
      gr_font_n(tt)       = (unsigned int)mp_font_n(p);
      gr_font_name(tt)    = mp_xstrdup(mp,mp->font_name[mp_font_n(p)]);
      gr_font_dsize(tt)   = (unsigned int)mp->font_dsize[mp_font_n(p)];
      export_color(tt,p) ;
      export_scripts(tt,p);
      gr_width_val(tt)    = width_val(p);
      gr_height_val(tt)   = height_val(p);
      gr_depth_val(tt)    = depth_val(p);
      gr_tx_val(tt)       = tx_val(p);
      gr_ty_val(tt)       = ty_val(p);
      gr_txx_val(tt)      = txx_val(p);
      gr_txy_val(tt)      = txy_val(p);
      gr_tyx_val(tt)      = tyx_val(p);
      gr_tyy_val(tt)      = tyy_val(p);
      break;
    case mp_start_clip_code: 
      tc = (mp_clip_object *)hq;
      gr_path_p(tc) = mp_export_knot_list(mp,mp_path_p(p));
      break;
    case mp_start_bounds_code:
      tb = (mp_bounds_object *)hq;
      gr_path_p(tb) = mp_export_knot_list(mp,mp_path_p(p));
      break;
    case mp_stop_clip_code: 
    case mp_stop_bounds_code:
      /* nothing to do here */
      break;
    } 
    if (hh->body==NULL) hh->body=hq; else  gr_link(hp) = hq;
    hp = hq;
    p=mp_link(p);
  }
  return hh;
}

@ This function is only used for the |glyph| operator, so 
it takes quite a few shortcuts for cases that cannot appear
in the output of |mp_ps_font_charstring|.

@c
pointer mp_gr_unexport(MP mp, struct mp_edge_object *hh) {
  pointer h; /* the edge object */
  pointer ph, pn, pt; /* for adding items */
  mp_graphic_object *p; /* the current graphical object */
  h = mp_get_node(mp, edge_header_size);
  mp_init_edges(mp, h);
  ph = dummy_loc(h); 
  pt = ph;
  p = hh->body;
  minx_val(h) = hh->minx;
  miny_val(h) = hh->miny;
  maxx_val(h) = hh->maxx;
  maxy_val(h) = hh->maxy;
  while ( p!=NULL ) { 
    switch (gr_type(p)) {
    case mp_fill_code: 
      if ( gr_pen_p((mp_fill_object *)p)==NULL ) {
        pn = mp_new_fill_node (mp, null);
        mp_path_p(pn) = mp_import_knot_list(mp,gr_path_p((mp_fill_object *)p));
        mp_color_model(pn)=mp_grey_model;
        if (mp_new_turn_cycles(mp, mp_path_p(pn))<0) {
          grey_val(pn) = unity;
          mp_link(pt) = pn;
          pt = mp_link(pt);
        } else {
          mp_link(pn) = mp_link(ph);
          mp_link(ph) = pn;
          if (ph==pt)
            pt=pn;
        }
      }
      break;
    case mp_stroked_code:
    case mp_text_code: 
    case mp_start_clip_code: 
    case mp_stop_clip_code: 
    case mp_start_bounds_code:
    case mp_stop_bounds_code:
    case mp_special_code:  
      break;
    } /* all cases are enumerated */
    p=p->next;
  }
  mp_gr_toss_objects(hh);
  return h;
}


@ @<Declarations@>=
static struct mp_edge_object *mp_gr_export(MP mp, pointer h);
static pointer mp_gr_unexport(MP mp, struct mp_edge_object *h);

@ This function is now nearly trivial.

@c
void mp_ship_out (MP mp, pointer h) { /* output edge structure |h| */
  integer c; /* \&{charcode} rounded to the nearest integer */
  c=mp_round_unscaled(mp, mp->internal[mp_char_code]);
  @<Begin the progress report for the output of picture~|c|@>;
  (mp->shipout_backend) (mp, h);
  @<End progress report@>;
  if ( mp->internal[mp_tracing_output]>0 ) 
   mp_print_edges(mp, h," (just shipped out)",true);
}

@ @<Declarations@>=
static void mp_shipout_backend (MP mp, pointer h);

@ 
@c
void mp_shipout_backend (MP mp, pointer h) {
  char *s;
  mp_edge_object *hh; /* the first graphical object */
  hh = mp_gr_export(mp,h);
  s = NULL;
  if (mp->internal[mp_output_format]>0)
    s =  str(mp->internal[mp_output_format]);
  if (s && strcmp(s,"svg")==0) {
    (void)mp_svg_gr_ship_out (hh,
                 (mp->internal[mp_prologues]/65536),
                 false);
  } else {
    (void)mp_gr_ship_out (hh,
                 (mp->internal[mp_prologues]/65536),
                 (mp->internal[mp_procset]/65536), 
                 false);
  }
  mp_xfree(s);
  mp_gr_toss_objects(hh);
}

@ @<Exported types@>=
typedef void (*mp_backend_writer)(MP, int);

@ @<Option variables@>=
mp_backend_writer shipout_backend;

@ Now that we've finished |ship_out|, let's look at the other commands
by which a user can send things to the \.{GF} file.

@ @<Determine if a character has been shipped out@>=
{ 
  mp->cur_exp=mp_round_unscaled(mp, mp->cur_exp) % 256;
  if ( mp->cur_exp<0 ) mp->cur_exp=mp->cur_exp+256;
  boolean_reset(mp->char_exists[mp->cur_exp]);
  mp->cur_type=mp_boolean_type;
}

@ @<Glob...@>=
psout_data ps;
svgout_data svg;

@ @<Allocate or initialize ...@>=
mp_ps_backend_initialize(mp);
mp_svg_backend_initialize(mp);

@ @<Dealloc...@>=
mp_ps_backend_free(mp);
mp_svg_backend_free(mp);


@* \[45] Dumping and undumping the tables.
After \.{INIMP} has seen a collection of macros, it
can write all the necessary information on an auxiliary file so
that production versions of \MP\ are able to initialize their
memory at high speed. The present section of the program takes
care of such output and input. We shall consider simultaneously
the processes of storing and restoring,
so that the inverse relation between them is clear.
@.INIMP@>

The global variable |mem_ident| is a string that is printed right
after the |banner| line when \MP\ is ready to start. For \.{INIMP} this
string says simply `\.{(INIMP)}'; for other versions of \MP\ it says,
for example, `\.{(mem=plain 1990.4.14)}', showing the year,
month, and day that the mem file was created. We have |mem_ident=0|
before \MP's tables are loaded.

@<Glob...@>=
char * mem_ident;
void * mem_file; /* for input or output of mem information */

@ @<Set init...@>=
mp->mem_ident=NULL;

@ @<Initialize table entries...@>=
mp->mem_ident=xstrdup(" (INIMP)");

@ @<Dealloc variables@>=
xfree(mp->mem_ident);


@* \[46] The main program.
This is it: the part of \MP\ that executes all those procedures we have
written.

Well---almost. We haven't put the parsing subroutines into the
program yet; and we'd better leave space for a few more routines that may
have been forgotten.

@c @<Declare the basic parsing subroutines@>
@<Declare miscellaneous procedures that were declared |forward|@>

@ We've noted that there are two versions of \MP. One, called \.{INIMP},
@.INIMP@>
has to be run first; it initializes everything from scratch, without
reading a mem file, and it has the capability of dumping a mem file.
The other one is called `\.{VIRMP}'; it is a ``virgin'' program that needs
@.VIRMP@>
to input a mem file in order to get started. \.{VIRMP} typically has
a bit more memory capacity than \.{INIMP}, because it does not need the
space consumed by the dumping/undumping routines and the numerous calls on
|primitive|, etc.

The \.{VIRMP} program cannot read a mem file instantaneously, of course;
the best implementations therefore allow for production versions of \MP\ that
not only avoid the loading routine for object code, they also have
a mem file pre-loaded. 

@ @<Option variables@>=
int ini_version; /* are we iniMP? */

@ @<Set |ini_version|@>=
mp->ini_version = (opt->ini_version ? true : false);

@ The code below make the final chosen hash size the next larger
multiple of 2 from the requested size, and this array is a list of
suitable prime numbers to go with such values. 

The top limit is chosen such that it is definately lower than
|max_halfword-3*param_size|, because |param_size| cannot be larger
than |max_halfword/sizeof(pointer)|.

@<Declarations@>=
static int mp_prime_choices[] = 
  { 12289,        24593,    49157,    98317,
    196613,      393241,   786433,  1572869,
    3145739,    6291469, 12582917, 25165843,
    50331653, 100663319  };

@ @<Find constant sizes@>=
if (mp->ini_version) {
  unsigned i = 14;
  set_value(mp->mem_top,opt->main_memory,5000);
  mp->mem_max = mp->mem_top;
  set_value(mp->param_size,opt->param_size,150);
  set_value(mp->max_in_open,opt->max_in_open,10);
  if (opt->hash_size>0x8000000) 
    opt->hash_size=0x8000000;
  if (opt->hash_size>0) {
    set_value(mp->hash_size,(2*opt->hash_size-1),16384);
  } else {
    mp->hash_size = 16384;
  }
  mp->hash_size = mp->hash_size>>i;
  while (mp->hash_size>=2) {
    mp->hash_size /= 2;
    i++;
  }
  mp->hash_size = mp->hash_size << i;
  if (mp->hash_size>0x8000000) 
    mp->hash_size=0x8000000;
  mp->hash_prime=mp_prime_choices[(i-14)];
} else {
  int i = -1;
  if (mp->mem_name == NULL) {
    mp->mem_name = mp_xstrdup(mp,"plain");
  }
  if (mp_open_mem_file(mp)) {
    i = mp_undump_constants(mp);
    if (i != metapost_magic)
      goto OFF_BASE;    
    set_value(mp->mem_max,opt->main_memory,mp->mem_top);
    goto DONE;
OFF_BASE:
    wterm_ln("(Fatal mem file error; ");
    wterm((mp->find_file)(mp, mp->mem_name, "r", mp_filetype_memfile));
    if (i>metapost_old_magic && i<metapost_magic) {
      wterm(" was written by an older version)\n");
    } else {
      wterm(" appears not to be a mem file)\n");
    }
  } 
  mp->history = mp_fatal_error_stop;
  mp_jump_out(mp);
}
DONE:


@ Here we do whatever is needed to complete \MP's job gracefully on the
local operating system. The code here might come into play after a fatal
error; it must therefore consist entirely of ``safe'' operations that
cannot produce error messages. For example, it would be a mistake to call
|str_room| or |make_string| at this time, because a call on |overflow|
might lead to an infinite loop.
@^system dependencies@>

This program doesn't bother to close the input files that may still be open.

@ @c
void mp_close_files_and_terminate (MP mp) {
  integer k; /* all-purpose index */
  integer LH; /* the length of the \.{TFM} header, in words */
  int lk_offset; /* extra words inserted at beginning of |lig_kern| array */
  pointer p; /* runs through a list of \.{TFM} dimensions */
  if (mp->finished) 
    return;
  @<Close all open files in the |rd_file| and |wr_file| arrays@>;
  if ( mp->internal[mp_tracing_stats]>0 )
    @<Output statistics about this job@>;
  wake_up_terminal; 
  @<Do all the finishing work on the \.{TFM} file@>;
  @<Explain what output files were written@>;
  if ( mp->log_opened  && ! mp->noninteractive ){ 
    wlog_cr;
    (mp->close_file)(mp,mp->log_file); 
    mp->selector=mp->selector-2;
    if ( mp->selector==term_only ) {
      mp_print_nl(mp, "Transcript written on ");
@.Transcript written...@>
      mp_print(mp, mp->log_name); mp_print_char(mp, xord('.'));
    }
  }
  mp_print_ln(mp);
  mp->finished = true;
}

@ @<Declarations@>=
static void mp_close_files_and_terminate (MP mp) ;

@ @<Close all open files in the |rd_file| and |wr_file| arrays@>=
if (mp->rd_fname!=NULL) {
  for (k=0;k<(int)mp->read_files;k++ ) {
    if ( mp->rd_fname[k]!=NULL ) {
      (mp->close_file)(mp,mp->rd_file[k]);
      xfree(mp->rd_fname[k]);      
   }
 }
}
if (mp->wr_fname!=NULL) {
  for (k=0;k<(int)mp->write_files;k++) {
    if ( mp->wr_fname[k]!=NULL ) {
     (mp->close_file)(mp,mp->wr_file[k]);
      xfree(mp->wr_fname[k]); 
    }
  }
}

@ @<Dealloc ...@>=
for (k=0;k<(int)mp->max_read_files;k++ ) {
  if ( mp->rd_fname[k]!=NULL ) {
    (mp->close_file)(mp,mp->rd_file[k]);
    xfree(mp->rd_fname[k]); 
  }
}
xfree(mp->rd_file);
xfree(mp->rd_fname);
for (k=0;k<(int)mp->max_write_files;k++) {
  if ( mp->wr_fname[k]!=NULL ) {
    (mp->close_file)(mp,mp->wr_file[k]);
    xfree(mp->wr_fname[k]); 
  }
}
xfree(mp->wr_file);
xfree(mp->wr_fname);


@ We want to produce a \.{TFM} file if and only if |mp_fontmaking| is positive.

We reclaim all of the variable-size memory at this point, so that
there is no chance of another memory overflow after the memory capacity
has already been exceeded.

@<Do all the finishing work on the \.{TFM} file@>=
if ( mp->internal[mp_fontmaking]>0 ) {
  @<Make the dynamic memory into one big available node@>;
  @<Massage the \.{TFM} widths@>;
  mp_fix_design_size(mp); mp_fix_check_sum(mp);
  @<Massage the \.{TFM} heights, depths, and italic corrections@>;
  mp->internal[mp_fontmaking]=0; /* avoid loop in case of fatal error */
  @<Finish the \.{TFM} file@>;
}

@ @<Make the dynamic memory into one big available node@>=
mp->rover=lo_mem_stat_max+1; mp_link(mp->rover)=empty_flag; mp->lo_mem_max=mp->hi_mem_min-1;
if ( mp->lo_mem_max-mp->rover>max_halfword ) mp->lo_mem_max=max_halfword+mp->rover;
node_size(mp->rover)=mp->lo_mem_max-mp->rover; 
lmp_link(mp->rover)=mp->rover; rmp_link(mp->rover)=mp->rover;
mp_link(mp->lo_mem_max)=null; mp_info(mp->lo_mem_max)=null

@ The present section goes directly to the log file instead of using
|print| commands, because there's no need for these strings to take
up |str_pool| memory when a non-{\bf stat} version of \MP\ is being used.

@<Output statistics...@>=
if ( mp->log_opened ) { 
  char s[128];
  wlog_ln(" ");
  wlog_ln("Here is how much of MetaPost's memory you used:");
@.Here is how much...@>
  mp_snprintf(s,128," %i string%s out of %i",(int)mp->max_strs_used-mp->init_str_use,
          (mp->max_strs_used!=mp->init_str_use+1 ? "s" : ""),
          (int)(mp->max_strings-1-mp->init_str_use));
  wlog_ln(s);
  mp_snprintf(s,128," %i string characters out of %i",
           (int)mp->max_pl_used-mp->init_pool_ptr,
           (int)mp->pool_size-mp->init_pool_ptr);
  wlog_ln(s);
  mp_snprintf(s,128," %i words of memory out of %i",
           (int)mp->lo_mem_max+mp->mem_end-mp->hi_mem_min+2,
           (int)mp->mem_end);
  wlog_ln(s);
  mp_snprintf(s,128," %i symbolic tokens out of %i", (int)mp->st_count, (int)mp->hash_size);
  wlog_ln(s);
  mp_snprintf(s,128," %ii,%in,%ip,%ib,%if stack positions out of %ii,%in,%ip,%ib,%if",
           (int)mp->max_in_stack,(int)mp->int_ptr,
           (int)mp->max_param_stack,(int)mp->max_buf_stack+1,(int)mp->in_open_max,
           (int)mp->stack_size,(int)mp->max_internal,(int)mp->param_size,
	   (int)mp->buf_size,(int)mp->max_in_open);
  wlog_ln(s);
  mp_snprintf(s,128," %i string compactions (moved %i characters, %i strings)",
          (int)mp->pact_count,(int)mp->pact_chars,(int)mp->pact_strs);
  wlog_ln(s);
}

@ It is nice to have have some of the stats available from the API.

@<Exported function ...@>=
int mp_memory_usage (MP mp );
int mp_hash_usage (MP mp );
int mp_param_usage (MP mp );
int mp_open_usage (MP mp );

@ @c
int mp_memory_usage (MP mp ) {
	return (int)mp->lo_mem_max+mp->mem_end-mp->hi_mem_min+2;
}
int mp_hash_usage (MP mp ) {
  return (int)mp->st_count;
}
int mp_param_usage (MP mp ) {
	return (int)mp->max_param_stack;
}
int mp_open_usage (MP mp ) {
	return (int)mp->max_in_stack;
}

@ We get to the |final_cleanup| routine when \&{end} or \&{dump} has
been scanned.

@c
void mp_final_cleanup (MP mp) {
  integer c; /* 0 for \&{end}, 1 for \&{dump} */
  c=mp->cur_mod;
  if ( mp->job_name==NULL ) mp_open_log_file(mp);
  while ( mp->input_ptr>0 ) {
    if ( token_state ) mp_end_token_list(mp);
    else  mp_end_file_reading(mp);
  }
  while ( mp->loop_ptr!=null ) mp_stop_iteration(mp);
  while ( mp->open_parens>0 ) { 
    mp_print(mp, " )"); decr(mp->open_parens);
  };
  while ( mp->cond_ptr!=null ) {
    mp_print_nl(mp, "(end occurred when ");
@.end occurred...@>
    mp_print_cmd_mod(mp, fi_or_else,mp->cur_if);
    /* `\.{if}' or `\.{elseif}' or `\.{else}' */
    if ( mp->if_line!=0 ) {
      mp_print(mp, " on line "); mp_print_int(mp, mp->if_line);
    }
    mp_print(mp, " was incomplete)");
    mp->if_line=if_line_field(mp->cond_ptr);
    mp->cur_if=mp_name_type(mp->cond_ptr); mp->cond_ptr=mp_link(mp->cond_ptr);
  }
  if ( mp->history!=mp_spotless )
    if ( ((mp->history==mp_warning_issued)||(mp->interaction<mp_error_stop_mode)) )
      if ( mp->selector==term_and_log ) {
    mp->selector=term_only;
    mp_print_nl(mp, "(see the transcript file for additional information)");
@.see the transcript file...@>
    mp->selector=term_and_log;
  }
  if ( c==1 ) {
    if (mp->ini_version) {
      mp_store_mem_file(mp); return;
    }
    mp_print_nl(mp, "(dump is performed only by INIMP)"); return;
@.dump...only by INIMP@>
  }
}

@ @<Declarations@>=
static void mp_final_cleanup (MP mp) ;
static void mp_init_prim (MP mp) ;
static void mp_init_tab (MP mp) ;

@ @c
void mp_init_prim (MP mp) { /* initialize all the primitives */
  @<Put each...@>;
}
@#
void mp_init_tab (MP mp) { /* initialize other tables */
  integer k; /* all-purpose index */
  @<Initialize table entries (done by \.{INIMP} only)@>;
}


@ When we begin the following code, \MP's tables may still contain garbage;
thus we must proceed cautiously to get bootstrapped in.

But when we finish this part of the program, \MP\ is ready to call on the
|main_control| routine to do its work.

@<Get the first line...@>=
{ 
  @<Initialize the input routines@>;
  if (mp->mem_ident==NULL) {
    if ( ! mp_load_mem_file(mp) ) {
      (mp->close_file)(mp, mp->mem_file); 
       mp->history = mp_fatal_error_stop;
       return mp;
    }
    (mp->close_file)(mp, mp->mem_file);
  }
  @<Initializations following first line@>;
}

@ @<Initializations following first line@>=
  mp->buffer[limit]=(ASCII_code)'%';
  mp_fix_date_and_time(mp);
  if (mp->random_seed==0)
    mp->random_seed = (mp->internal[mp_time] / unity)+mp->internal[mp_day];
  mp_init_randoms(mp, mp->random_seed);
  @<Initialize the print |selector|...@>;
  if ( loc<limit ) if ( mp->buffer[loc]!='\\' ) 
    mp_start_input(mp); /* \&{input} assumed */

@ @<Run inimpost commands@>=
{
  mp_get_strings_started(mp);
  mp_init_tab(mp); /* initialize the tables */
  mp_init_prim(mp); /* call |primitive| for each primitive */
  mp->init_str_use=mp->max_str_ptr=mp->str_ptr;
  mp->init_pool_ptr=mp->max_pool_ptr=mp->pool_ptr;
  mp_fix_date_and_time(mp);
}

@ Saving the filename template

@<Save the filename template@>=
{ 
  delete_str_ref(mp->internal[mp_output_template]);
  if ( length(mp->cur_exp)==0 ) {
    mp->internal[mp_output_template] = rts("%j.%c");
  } else { 
    mp->internal[mp_output_template]=mp->cur_exp; 
    add_str_ref(mp->internal[mp_output_template]);
  }
}

@* \[47] Debugging.


@* \[48] System-dependent changes.
This section should be replaced, if necessary, by any special
modification of the program
that are necessary to make \MP\ work at a particular installation.
It is usually best to design your change file so that all changes to
previous sections preserve the section numbering; then everybody's version
will be consistent with the published program. More extensive changes,
which introduce new sections, can be inserted here; then only the index
itself will get a new section number.
@^system dependencies@>

@* \[49] Index.
Here is where you can find all uses of each identifier in the program,
with underlined entries pointing to where the identifier was defined.
If the identifier is only one letter long, however, you get to see only
the underlined entries. {\sl All references are to section numbers instead of
page numbers.}

This index also lists error messages and other aspects of the program
that you might want to look up some day. For example, the entry
for ``system dependencies'' lists all sections that should receive
special attention from people who are installing \MP\ in a new
operating environment. A list of various things that can't happen appears
under ``this can't happen''.
Approximately 25 sections are listed under ``inner loop''; these account
for more than 60\pct! of \MP's running time, exclusive of input and output.
