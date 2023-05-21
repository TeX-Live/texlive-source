% Changes to adapt CTIE to web2c.
% Copyright 2002,2003 Julian Gilbey
% All rights reserved.
%
% This file is distributed WITHOUT ANY WARRANTY, express or implied.
%
% Permission is granted to make and distribute verbatim copies of this
% file provided that the copyright notice and this permission notice
% are preserved on all copies.
%
% Permission is granted to copy and distribute modified versions of this
% file under the conditions for verbatim copying, provided that the
% entire resulting derived work is distributed under the terms of a
% permission notice identical to this one.
%
% This file is based heavily on tie.ch by Olaf Weber to adapt tie.w to
% the web2c system and on comm-w2c.ch from the Web2C 7.4.5 distribution
% by Wlodek Bzyl and Olaf Weber.

@x l.19 Add macro definitions
\def\title{The CTIE processor}
\def\botofcontents{%
\vfill
$\copyright$ 2002,2003 Julian Gilbey

All rights reserved.

This program is distributed WITHOUT ANY WARRANTY, express or implied.

Permission is granted to make and distribute verbatim copies of this
program provided that the copyright notice and this permission notice
are preserved on all copies.

Permission is granted to copy and distribute modified versions of this
program under the conditions for verbatim copying, provided that the
entire resulting derived work is distributed under the terms of a
permission notice identical to this one.
}
@y
\def\Kpathsea/{{\mc KPATHSEA\spacefactor1000}}
\let\maybe=\iftrue
\def\title{The CTIE processor}
\def\topofcontents{\null\vfill
  \centerline{\titlefont The {\ttitlefont CTIE} processor}
  \vskip 15pt
  \centerline{(Version 1.1 [\TeX~Live])}
  \vfill}
\def\covernote{\vbox{\ninerm\noindent
Copyright \copyright\ 2002, 2003 Julian Gilbey
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

@x l.81
@d copyright 
    "Copyright 2002,2003 Julian Gilbey.  All rights reserved.  There is no warranty.\n\
Run with the --version option for other important information."
@y
@d copyright
    "Copyright 2002,2003 Julian Gilbey."@|
    "  All rights reserved.  There is no warranty.\n"@|
    "Run with the --version option for other important information."
@z

@x l.102
main(argc, argv)
        int argc; string *argv;
@y
int main (int argc, string *argv)
@z

@x l.105 Set up kpathsea stuff
    @<Initialise parameters@>;
@y
    @<Set up |PROGNAME| feature and initialise the search path mechanism@>@;
    @<Initialise parameters@>@;
@z

@x l.107
    @<Print the banners@>;
@y
    @<Print the banners@>@;
@z

boolean and string are defined by kpathsea.
@x l.116
@ We include the additional types |boolean| and |string|.  \.{CTIE}
replaces the complex \.{TIE} character set handling (based on that of
the original \.{WEB} system) with the standard \.{CWEB} behaviour, and
so uses the |char| type for input and output.

@d false 0
@d true 1

@<Global types@>=
typedef int boolean;
typedef char* string;
@y
@ We include the additional types |boolean| and |string|.  \.{CTIE}
replaces the complex \.{TIE} character set handling (based on that of
the original \.{WEB} system) with the standard \.{CWEB} behaviour, and
so uses the |char| type for input and output.

The \.{kpathsea} library (version 3.4.5 and higher) defines the |@!boolean|
(with the values |@!true| and |@!false|) and |@!string| (and |@!const_string|)
types in \.{<kpathsea/simpletypes.h>}, so we do not actually need to define
them here.

@s boolean int
@s string int
@s const_string int
@z

@x l.129 The kpathsea include files find the right header file for these.
@ We predeclare some standard string-handling functions here instead of
including their system header files, because the names of the header files
are not as standard as the names of the functions.  (There's confusion
between \.{<string.h>} and \.{<strings.h>}.)

@<Predecl...@>=
extern int strlen(); /* length of string */
extern char* strcpy(); /* copy one string to another */
extern int strncmp(); /* compare up to $n$ string characters */
extern char* strncpy(); /* copy up to $n$ string characters */
extern char *strerror();
@y
@ We don't need to predeclare any string handling functions here, as
the \.{kpathsea} headers do the right thing by including \.{<string.h>}
behind the scenes.
@z

@x l.149
@d xisupper(c) (isupper(c)&&((unsigned char)c<0200))
@y
@d xisupper(c) (isupper((unsigned char)c)&&((unsigned char)c<0200))
@z

@x l.155
This variable must be initialized.
@y
This variable must be initialised.
@z

@x l.158
@d spotless 0
@d troublesome 1
@d fatal 2

@<Global variables@>=
int history=spotless;
@y
@<Global variables@>=
typedef enum {
    @!spotless,
    @!troublesome,
    @!fatal } return_code;
static return_code history=spotless;
@z

@x l.170 The kpathsea include files must be first.
predefined as we include the \.{stdio.h} definitions.

@<Global \&{\#include}s@>=
#include <stdio.h>
@y
predefined as we include the \.{<stdio.h>} definitions
through the \.{kpathsea} interface.

@<Global \&{\#include}s@>=
#include <kpathsea/kpathsea.h>
#include <w2c/config.h>
#include <lib/lib.h>
@z

@x l.176 And this.
@ And we need dynamic memory allocation.
This should cause no trouble in any \CEE/ program.
@^system dependencies@>

@<Global \&{\#include}s@>=
#ifdef __STDC__
#include <stdlib.h>
#else
#include <malloc.h>
#endif
@y
@ And we need dynamic memory allocation.
This should cause no trouble in any \CEE/ program.
The \.{kpathsea} include files handle the definition of |@!malloc|, too.
@^system dependencies@>
@z

@x l.190
files) are treated the same way.  To organize the
@y
files) are treated the same way.  To organise the
@z

Sections 10 and 11: use enum as requested in ctie.w

@x l.204
\leavevmode |file_types| is used to describe whether a file
@y
\leavevmode \&{file\_types} is used to describe whether a file
@z

@x l.208
the kind of output. (this would even be necessary if we
@y
the kind of output. (This would even be necessary if we
@z

@x l.212
#define search 0
#define test 1
#define reading 2
#define ignore 3
typedef int in_file_modes; /* should be |enum(search, test, reading, ignore)| */
#define unknown 0
#define master 1
#define chf 2
typedef int file_types; /* should be |enum(unknown, master, chf)| */
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

@x l.223
@ A variable of type |out_md_type| will tell us in what state the output
@y
@ A variable of type \&{out\_md\_type} will tell us in what state the output
@z

@x l.230 dito
#define normal 0
#define pre 1
#define post 2
typedef int out_md_type; /* should be |enum(normal, pre, post)| */
@y
typedef enum {
    @!normal,
    @!pre,
    @!post } out_md_type;
@z

@x l.284 way too short!
@d max_file_name_length 60
@y
@d max_file_name_length 1024
@z

@x l.329
boolean get_line(i, do_includes)
        file_index i; boolean do_includes;
@y
static boolean
get_line (file_index i, boolean do_includes)
@z

@x l.361
replacement part of a change file, or in an incomplerte check if the
@y
replacement part of a change file, or in an incomplete check if the
@z

Handle input lines with CRLF

@x l.376
        if ((*(k++) = c) != ' ') inp_desc->limit = k;
@y
        if ((*(k++) = c) != ' ' && c != '\r') inp_desc->limit = k;
@z

@x l.386
    @<Increment the line number and print a progess report at
@y
    @<Increment the line number and print a progress report at
@z

@x l.436
        if ((*(k++) = c) != ' ') inp_desc->limit = k;
@y
        if ((*(k++) = c) != ' ' && c != '\r') inp_desc->limit = k;
@z

The next piece is simplified using the kpathsea kpse_find_file
function.

@x l.497
If the environment variable \.{CWEBINPUTS} is set, or if the compiler flag 
of the same name was defined at compile time,
\.{CWEB} will look for include files in the directory thus named, if
it cannot find them in the current directory.
(Colon-separated paths are not supported.)
@y
We use the \Kpathsea/ library (in particular, the \.{CWEBINPUTS}
variable) to search for this file.@.CWEBINPUTS@>
@z

@x l.510 Don't need the same variables any longer
    char temp_file_name[max_file_name_length]; 
    char *file_name_end;
    char *k, *kk;
    int l; /* length of file name */
@y
    char *file_name_end;
    string fullname;
    char *k;
@z

@x l.534 Replace with kpse_find_file
    if ((new_inc->the_file=fopen(new_inc->file_name, "r"))!=NULL) {
@y
    if ((fullname=kpse_find_cweb(new_inc->file_name))!=NULL @|
      && (new_inc->the_file=fopen(fullname, "r"))!=NULL) {
        free(fullname);
@z

@x l.539 And this part is replaced by kpse_find_file
    kk=getenv("CWEBINPUTS");
    if (kk!=NULL) {
        if ((l=strlen(kk))>max_file_name_length-2) too_long();
        strcpy(temp_file_name, kk);
    }
    else {
#ifdef CWEBINPUTS
        if ((l=strlen(CWEBINPUTS))>max_file_name_length-2) too_long();
        strcpy(temp_file_name, CWEBINPUTS);
#else
        l=0; 
#endif /* |CWEBINPUTS| */
    }
    if (l>0) {
        if (k+l+2>=file_name_end)  too_long();
        for (; k>= new_inc->file_name; k--) *(k+l+1)=*k;
        strcpy(new_inc->file_name, temp_file_name);
        new_inc->file_name[l]='/'; /* \UNIX/ pathname separator */
        if ((new_inc->the_file=fopen(new_inc->file_name, "r"))!=NULL) {
            new_inc->parent=inp_desc->current_include; /* link it in */
            inp_desc->current_include=new_inc;
            goto restart; /* success */
        }
    }
@y
@z

@x l.565 slightly more useful error message
    err_print(i, "! Cannot open include file");
@y
    if (fullname) {
        free(fullname);
        err_print(i, "! Cannot open include file");
    } else
        err_print(i, "! Cannot find include file");
@z


@x l.585
void err_print();
@y
void err_print (file_index, const char *);
@z

@x l.590
void err_print(i, s) /* prints `\..' and location of error message */
file_index i; char *s;
@y
void err_print (file_index i, const char *s)
/* prints `\..' and location of error message */
@z

@x l.664
int wrap_up()
@y
int wrap_up (void)
@z

@x l.667
    if (history > spotless) return 1;
    else return 0;
@y
    if (history > spotless) return EXIT_FAILURE;
    else return EXIT_SUCCESS;
@z

@x l.674
int wrap_up();
@y
int wrap_up (void);
@z

@x l.697
void pfatal_error();
@y
void pfatal_error (const char *, const char *);
@z

@x l.700
void pfatal_error(s, t)
char *s, *t;
@y
void pfatal_error (const char *s, const char *t)
@z

@x l.707
    else fprintf(stderr, "\n");
@y
    else putc('\n', stderr);
@z

@x l.713
@ We need an include file for the above.

@<Global \&{\#include}s@>=
#include <errno.h>
@y
@ The \.{<errno.h>} include file for the above comes via the \.{kpathsea}
interface.
@z

@x l.731 Use binary mode for output files
    out_file=fopen(out_name, "w");
    if (out_file==NULL) {
@y
    if ((out_file=fopen(out_name, "wb"))==NULL) {
@z

@x l.739
@ The name of the file and the file desciptor are stored in
@y
@ The name of the file and the file descriptor are stored in
@z

@x l.747 Use the kpathsea library to do this
@ For the master file we start by reading its first line into the
buffer, if we could open it.

@<Get the master file started@>=
{
    input_organisation[0]->the_file=
        fopen(input_organisation[0]->file_name, "r");

    if (input_organisation[0]->the_file==NULL)
        pfatal_error("! Cannot open master file ",
            input_organisation[0]->file_name);
@.Cannot open master file@>
@y
@ For the master file we start by reading its first line into the
buffer, if we could open it.  We use the \.{kpathsea} library to find
the file.

@<Get the master file started@>=
{
    string fullname;

    if ((fullname = kpse_find_cweb(input_organisation[0]->file_name))!=NULL) {
        if ((input_organisation[0]->the_file = fopen(fullname, "r"))==NULL)
            pfatal_error("! Cannot open master file ",
                input_organisation[0]->file_name);
        free(fullname);
    } @+ else {
        fatal_error(-1, "! Cannot find master file ",
            input_organisation[0]->file_name);
    }
@.Cannot open master file@>
@.Cannot find master file@>
@z

@x l.768 And this
@<Prepare the change files@>=
{
    file_index i;

    i=1;
    while (i<no_ch) {
        input_organisation[i]->the_file=
            fopen(input_organisation[i]->file_name, "r");
        if (input_organisation[i]->the_file==NULL)
            pfatal_error("! Cannot open change file ",
                input_organisation[i]->file_name);
@.Cannot open change file@>
@y
@<Prepare the change files@>=
{
    file_index i;
    string fullname;

    i=1;
    while (i<no_ch) {
        if ((fullname = kpse_find_cweb(input_organisation[i]->file_name))
                !=NULL) {
            if ((input_organisation[i]->the_file = fopen(fullname, "r"))==NULL)
                pfatal_error("! Cannot open change file ",
                    input_organisation[i]->file_name);
            free(fullname);
        } @+ else {
            fatal_error(-1, "! Cannot find change file ",
                input_organisation[i]->file_name);
        }
@.Cannot open change file@>
@.Cannot find change file@>
@z

@x l.792
boolean lines_dont_match(i, j)
        file_index i, j;
@y
static boolean
lines_dont_match (file_index i, file_index j)
@z

@x l.809
void init_change_file(i)
        file_index i;
@y
static void
init_change_file (file_index i)
@z

@x l.833
    if (xisupper(ccode)) ccode=tolower(ccode);
@y
    if (xisupper(ccode)) ccode=tolower((unsigned char)ccode);
@z

@x l.858
void put_line(j)
       file_index j;
@y
static void
put_line (file_index j)
@z

@x l.873
boolean e_of_ch_module(i)
        file_index i;
@y
static boolean
e_of_ch_module (file_index i)
@z

@x l.894
boolean e_of_ch_preamble(i)
        file_index i;
@y
static boolean
e_of_ch_preamble (file_index i)
@z

@x l.1079
if (out_mode==post) /* last line has been changed */
    fprintf(out_file, "@@z\n");
@y
if (out_mode==pre) /* last line has been deleted */
    fprintf(out_file, "@@y\n"), out_mode=post;
if (out_mode==post) /* last line has been changed */
    fprintf(out_file, "@@z\n");
@z

@x l.1106
void usage_error()
{
    @<Print the banners@>;
    fprintf(stderr, "Usage: ctie -[mc] outfile master changefile(s)\n");
@y
static void
usage_error (void)
{
    @<Print the banners@>;
    fprintf(stderr, "Usage: ctie -m|-c outfile master changefile(s)\n");
@z

@x l.1111
    exit(1);
@y
    exit(EXIT_FAILURE);
@z

@x l.1119 Add Web2C version to banner string
printf("%s\n", banner); /* print a ``banner line'' */
@y
printf("%s%s\n", banner, versionstring); /* print a ``banner line'' */
@z

Section 63: use 'none' more than once.

@x l.1169
(which is the case if |no_ch==(-1)|) or if the next element of
@y
(which is the case if |no_ch==none|) or if the next element of
@z

@x l.1174
    if (no_ch==(-1)) {
@y
    if (no_ch==none) {
@z

@x l.1218
string CTIEHELP[] = {
    "Usage: ctie -[mc] outfile master changefile(s)",
@y
const_string CTIEHELP[] = {
    "Usage: ctie -m|-c outfile master changefile(s)",
@z

@x l.1233
void usage_help();
void print_version_and_exit();
@y
static void usage_help (void);
static void print_version_and_exit (const_string, const_string);
@z

@x l.1238
void usage_help()
{
    string *message=CTIEHELP;
@y
static void
usage_help (void)
{
    const_string *message=CTIEHELP;
@z

@x l.1248
    exit(0);
@y
    exit(EXIT_SUCCESS);
@z

@x l.1253
void print_version_and_exit(name, version)
        string name, version;
{
    printf ("%s %s\n", name, version);

    puts ("Copyright (C) 2002,2003 Julian Gilbey.");

    puts ("There is NO warranty.  This is free software.  See the source");
    puts ("code of CTIE for redistribution conditions.");

    exit (0);
}
@y
static void
print_version_and_exit (const_string name, const_string version)
{
    printf ("%s %s%s\n", name, version, versionstring);
    puts (kpathsea_version_string);

    puts ("Copyright (C) 2002,2003 Julian Gilbey.");
    puts ("Kpathsea is copyright (C) 1999 Free Software Foundation, Inc.");

    puts ("There is NO warranty.  This is free software.");
    puts ("Redistribution of this software is covered by the terms of");
    puts ("both the CTIE copyright and the GNU General Public Licence.");
    puts ("For more information about these matters, see the files");
    puts ("named COPYING and the CTIE source.");
    puts ("Primary author of CTIE: Julian Gilbey.");
    puts ("Kpathsea written by Karl Berry and others.");

    exit (EXIT_SUCCESS);
}
@z

We use the path-searching initialisation code
taken almost verbatim from comm-w2c.ch.

@x l.1267
@* System-dependent changes.
This section should be replaced, if necessary, by
changes to the program that are necessary to make \.{CTIE}
work at a particular installation.  It is usually best to
design your change file so that all changes to previous
modules preserve the module numbering; then everybody's
version will be consistent with the printed program.  More
extensive changes, which introduce new modules, can be
inserted here; then only the index itself will get a new
module number.
@^system dependencies@>
@y
@* System-dependent changes.@^system dependencies@>@.CWEBINPUTS@>
The \.{ctie} program from the original \.{CTIE} package uses the
compile-time default directory or the value of the environment
variable \.{CWEBINPUTS} as an alternative place to be searched for
files, if they could not be found in the current directory.

This version uses the \Kpathsea/ mechanism for searching files.
The directories to be searched for come from three sources:
\smallskip
{\parindent5em
\item{(a)} a user-set environment variable \.{CWEBINPUTS}
    (overridden by \.{CWEBINPUTS\_ctie});
\item{(b)} a line in \Kpathsea/ configuration file \.{texmf.cnf},\hfil\break
    e.g., \.{CWEBINPUTS=\$TEXMFDOTDIR:\$TEXMF/texmf/cweb//}\hfil\break
    or \.{CWEBINPUTS.ctie=\$TEXMFDOTDIR:\$TEXMF/texmf/cweb//};
\item{(c)} compile-time default directories (specified in
    \.{texmf.in}),\hfil\break
    i.e., \.{\$TEXMFDOTDIR:\$TEXMF/texmf/cweb//}.\par}

@d kpse_find_cweb(name) kpse_find_file(name, kpse_cweb_format, true)

@ The simple file searching is replaced by the `path searching'
mechanism that the \Kpathsea/ library provides.

We set |kpse_program_name| to `\.{ctie}'.  This means if the variable
\.{CWEBINPUTS.ctie} is present in \.{texmf.cnf} (or \.{CWEBINPUTS\_ctie}
in the environment) its value will be used as the search path for
filenames.  This allows different flavors of \.{CTIE} to have
different search paths.@.CWEBINPUTS@>

@<Set up |PROGNAME| feature...@>=
kpse_set_program_name(argv[0], "ctie");
@z
