% This is ctie.w
% Copyright (c) 2002,2003 by Julian Gilbey
% All rights reserved.
%
% This program is distributed WITHOUT ANY WARRANTY, express or implied.
%
% Permission is granted to make and distribute verbatim copies of this
% program provided that the copyright notice and this permission notice
% are preserved on all copies.
%
% Permission is granted to copy and distribute modified versions of this
% program under the conditions for verbatim copying, provided that the
% entire resulting derived work is distributed under the terms of a
% permission notice identical to this one.
%
% This program is based heavily on tie.w and common.w, part of the
% CWEB source.

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


@* Introduction.
Whenever a programmer wants to change a given \.{WEB} or \.{CWEB}
program (referred to as a \.{WEB} program throughout this program)
because of system dependencies, she or he will create a new change
file.  In addition there may be a second change file to modify system
independent modules of the program.  But the \.{WEB} file cannot be
tangled and weaved with more than one change file simultaneously.
The \.{TIE} program was designed to merge a \.{WEB} file
and several change files producing a new \.{WEB} file, and since the
input files are tied together, the program was called \.{TIE}.
Furthermore, the program could be used to merge several change files
giving a new single change file.  This method seems to be more
important because it doesn't modify the original source file.

However, the introduction of \.{CWEB} has meant that \.{TIE} is not
quite able to perform its task correctly any longer: \.{CWEB}
introduced the idea of include files, which are input into \.{CWEB}
files using the \.{@@i} command, and \.{TIE} is unable to handle such
constructs if the change files modify lines included in those files.
The present program, \.{CTIE}, is designed to overcome this lack.
Like \.{TIE}, upon which it is based, it can either output a single
master \.{WEB} file or a master change file.  However, in both cases,
any include commands will be totally expanded and the files included
in the output rather than the \.{@@i} commands being left; this makes
this code feasible, which it would not necessarily be otherwise.
Other than this difference, \.{CTIE} should function identically to
\.{TIE} on files which do not involve any \.{CWEB} include commands.

The algorithm used is essentially the same as that of \.{TIE}, with
modifications to check for and handle \.{@@i} commands.  Thus, as with
\.{TIE}, the method used only needs one buffer line for each input
file.  Thus the storage requirement of \.{CTIE} does not depend on the
sizes of the input files but only on their number.

The program is written in \CEE/ and has few system dependencies.

The ``banner line'' defined here should be changed whenever
\.{CTIE} is modified.  We also keep the version number here separately
for ease; it is used below.

@d version_number "1.1"
@d banner  "This is CTIE, Version 1.1"
@d copyright 
    "Copyright 2002,2003 Julian Gilbey.  All rights reserved.  There is no warranty.\n\
Run with the --version option for other important information."


@ The main outline of the program is now given.
This can be used more or less for any \CEE/ program.

@c
@<Global \&{\#include}s@>@;
@<Global types@>@;
@<Predeclaration of functions@>@;
@<Global variables@>@;
@<Error handling functions@>@;
@<Internal functions@>@;
@<The main function@>@;

@ And this is the structure of the |main| function: this is where
\.{CTIE} starts, and where it ends.

@<The main function@>=
main(argc, argv)
        int argc; string *argv;
{
    @<Initialise parameters@>;
    @<Scan the parameters@>@;
    @<Print the banners@>;
    @<Get the master file started@>@;
    @<Prepare the change files@>@;
    @<Prepare the output file@>@;
    @<Process the input@>@;
    @<Check that all changes have been read@>@;
    exit(wrap_up());
}

@ We include the additional types |boolean| and |string|.  \.{CTIE}
replaces the complex \.{TIE} character set handling (based on that of
the original \.{WEB} system) with the standard \.{CWEB} behaviour, and
so uses the |char| type for input and output.

@d false 0
@d true 1

@<Global types@>=
typedef int boolean;
typedef char* string;


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


@ The following parameters should be sufficient for most
applications of \.{CTIE}.
@^system dependencies@>

@d buf_size 1024 /* maximum length of one input line */
@d max_file_index 32
/* we don't think that anyone needs more than 32 change files */
@d xisupper(c) (isupper(c)&&((unsigned char)c<0200))


@ We introduce a history variable that allows us to set a
return code if the operating system can use it.
First we introduce the coded values for the history.
This variable must be initialized.
(We do this even if the value given may be the default for
variables, just to document the need for the initial value.)
@d spotless 0
@d troublesome 1
@d fatal 2

@<Global variables@>=
int history=spotless;


@* Input and output.
Standard output for the user is done by writing on |stdout|.  Error
messages are written to |stderr|.  Terminal input is not needed in
this version of \.{CTIE}.  |stdin|, |stdout| and |stderr| are
predefined as we include the \.{stdio.h} definitions.

@<Global \&{\#include}s@>=
#include <stdio.h>


@ And we need dynamic memory allocation.
This should cause no trouble in any \CEE/ program.
@^system dependencies@>

@<Global \&{\#include}s@>=
#ifdef __STDC__
#include <stdlib.h>
#else
#include <malloc.h>
#endif


@* Data structures.
The multiple primary input files (master file and change
files) are treated the same way.  To organize the
simultaneous usage of several input files, we introduce the
data type \&{in\_file\_modes}.

The mode |search| indicates that \.{CTIE} searches for a
match of the input line with any line of an input file in
|reading| mode.  |test| is used whenever a match is found
and it has to be tested if the next input lines do match
also.  |reading| describes that the lines can be read without
any check for matching other lines.  |ignore| denotes
that the file cannot be used.  This may happen because an
error has been detected or because the end of the file has
been found.

\leavevmode |file_types| is used to describe whether a file
is a master file or a change file. The value |unknown| is added
to this type to set an initial mode for the output file.
This enables us to check whether any option was used to select
the kind of output. (this would even be necessary if we
would assume a default action for missing options.)

@<Global types@>=
#define search 0
#define test 1
#define reading 2
#define ignore 3
typedef int in_file_modes; /* should be |enum(search, test, reading, ignore)| */
#define unknown 0
#define master 1
#define chf 2
typedef int file_types; /* should be |enum(unknown, master, chf)| */


@ A variable of type |out_md_type| will tell us in what state the output
change file is during processing. |normal| will be the state, when we
did not yet start a change, |pre| will be set when we write the lines
to be changes and |post| will indicate that the replacement lines are
written.

@<Global types@>=
#define normal 0
#define pre 1
#define post 2
typedef int out_md_type; /* should be |enum(normal, pre, post)| */


@ The next type will indicate variables used as an index into the file
table.

@<Global types@>=
typedef int file_index;  /* |-1..max_file_index+1| */


@ This is the data structure in which we collect information about
each include file.

@<Global types@>=
typedef struct _indsc {
    char file_name[max_file_name_length];
    long line;
    FILE *the_file;
    struct _indsc *parent;
} include_description;


@ The following data structure contains all of the information needed
to use these input files.
%`line' is a normal identifier throughout this program 
@f line dummy
@<Global types@>=
typedef struct _idsc{
    string file_name;
    char buffer[buf_size];
    in_file_modes mode;
    long line;
    file_types type_of_file;
    include_description *current_include;
    char *buffer_end;
    char *limit;
    char *loc;
    FILE *the_file;
    int dont_match;
    } input_description;


@ Every one of the primary input files might include another file
using the \.{@@i} include mechanism.  In turn, each of these might
include other files, and so on.  We allow a limited number of these
files to be opened simultaneously, and we store information about the
currently open include files as a linked list attached to each primary
file.

@d max_include_files 20
        /* maximum number of include files open simultaneously */
@d max_file_name_length 60

@<Global variables@>=
int total_include_files = 0;  /* count 'em */

@ The following variables refer to the files in action, the number of
change files, the mode of operation and the current output state.

@<Global variables@>=
file_index actual_input, test_input, no_ch;
file_types prod_chf=unknown;
out_md_type out_mode;

@ And the |actual_input| and |out_mode| variables need to be
initialised sensibly.

@<Initialise parameters@>=
actual_input=0;
out_mode=normal;

@ All primary input files (including the master file) are recorded
in the following structure.
The components are usually accessed through a local pointer variable,
requiring only a one-time-computation of the index expression.

@<Global variables@>=
input_description *input_organisation[max_file_index+1];


@* File I/O.
The basic function |get_line| can be used to get a line from
an input file.  The line is stored in the |buffer| part of the
descriptor.  The components |limit| and |line| are updated.  If the
end of the file is reached |mode| is set to |ignore|.  On some systems
it might be useful to replace tab characters by a proper number of
spaces since several editors used to create change files insert tab
characters into a source file not under control of the user.  So it
might be a problem to create a matching change file. 
@^tab character expansion@>

We define |get_line| to read a line from a file specified by the
corresponding file descriptor.  This function returns |true| if it is
successful and |false| if the end of the file has been reached.

@<Internal functions@>=
boolean get_line(i, do_includes)
        file_index i; boolean do_includes;
{
    register input_description *inp_desc=input_organisation[i];
    register FILE *fp;

    if (inp_desc->mode==ignore) return false;

  restart:
    if (inp_desc->current_include != NULL) {
        register include_description *inc_desc=inp_desc->current_include;

        fp=inc_desc->the_file;
        @<Get include line into buffer or |goto restart| if end of file@>@;
    }
    else {
        fp=inp_desc->the_file;
        @<Get line into buffer, |return false| if end of file@>@;
    }

    if (do_includes)
        @<Check for \.{@@i} in newly read line, |goto restart| if
            include fails@>@;
    return true;
}


@ Lines must fit into the buffer completely.
We read all characters sequentially until an end of line is found
(but do not forget to check for |EOF|!).
Too long input lines will be truncated.
This will result in a damaged output if they occur in the
replacement part of a change file, or in an incomplerte check if the
matching part is concerned.
Tab character expansion might be done here.
@^tab character expansion@>

@<Get line into buffer,...@>=
{
    register int c; /* the actual character read */
    register char *k; /* where the next character goes */

    if (feof(fp))
        @<Handle end of file and return@>@;

    inp_desc->limit = k = inp_desc->buffer;  /* beginning of buffer */
    while (k<=inp_desc->buffer_end && (c=getc(fp)) != EOF && c!='\n')
        if ((*(k++) = c) != ' ') inp_desc->limit = k;
    if (k>inp_desc->buffer_end)
        if ((c=getc(fp))!=EOF && c!='\n') {
            ungetc(c, fp); inp_desc->loc=inp_desc->buffer;
            err_print(i, "! Input line too long");
@.Input line too long@>
        }
    if (c==EOF && inp_desc->limit==inp_desc->buffer)
        @<Handle end of file...@>@;

    @<Increment the line number and print a progess report at
        certain times@>@;
}


@ End of file is special if this file is the master file.
Then we set the global flag variable |input_has_ended|.

@<Handle end of file ...@>=
{
    inp_desc->mode=ignore;
    inp_desc->limit=NULL;  /* mark end-of-file */
    if (inp_desc->type_of_file==master) input_has_ended=true;
    fclose(fp);
    return false;
}


@ This variable must be declared for global access.

@<Global variables@>=
boolean input_has_ended=false;


@ This section does what its name says. Every 100 lines
in the master file we print a dot, every 500 lines the number
of lines is shown.

@<Increment the line number and print ...@>=
inp_desc->line++;
if (inp_desc->type_of_file==master && inp_desc->line % 100==0) {
   if (inp_desc->line % 500 == 0)  printf("%ld", inp_desc->line);
   else  putchar('.');
   fflush(stdout);
}


@ The following is very similar to the above, but for the case where
we are reading from an include file.

@<Get include line into buffer or |goto restart| if end of file@>=
{
    register int c; /* the actual character read */
    register char *k; /* where the next character goes */

    if (feof(fp))
        @<Handle end of include file and |goto restart|@>@;

    inp_desc->limit = k = inp_desc->buffer;  /* beginning of buffer */
    while (k<=inp_desc->buffer_end && (c=getc(fp)) != EOF && c!='\n')
        if ((*(k++) = c) != ' ') inp_desc->limit = k;
    if (k>inp_desc->buffer_end)
        if ((c=getc(fp))!=EOF && c!='\n') {
            ungetc(c, fp); inp_desc->loc=inp_desc->buffer;
            err_print(i, "! Input line too long");
@.Input line too long@>
        }
    if (c==EOF && inp_desc->limit==inp_desc->buffer)
        @<Handle end of include file...@>@;

    inc_desc->line++;
}

@ We don't bail out if we find the end of an include file, we just
return to the parent file.

@<Handle end of include file and |goto restart|@>=
{
    include_description *temp=inc_desc->parent;

    fclose(fp);
    free(inc_desc);
    total_include_files--;
    inp_desc->current_include=temp;
    goto restart;
}


@ Usually, we have to check the line we have just read to see whether
it begins with \.{@@i} and therefore needs expanding.

@<Check for \.{@@i} in newly read line...@>=
{
    inp_desc->loc=inp_desc->buffer;
    *inp_desc->limit=' ';
    if (*inp_desc->buffer=='@@' &&
        (inp_desc->buffer[1]=='i' || inp_desc->buffer[1]=='I')) {
        inp_desc->loc=inp_desc->buffer+2;
        *inp_desc->limit='"'; /* this will terminate the search in all cases */
        while (*inp_desc->loc==' '||*inp_desc->loc=='\t')
            inp_desc->loc++;
        if (inp_desc->loc>=inp_desc->limit) {
            err_print(i, "! Include file name not given");
@.Include file name ...@>
            goto restart;
        }
        if (total_include_files>=max_include_files) {
            err_print(i, "! Too many nested includes");
@.Too many nested includes@>
            goto restart;
        } 
        total_include_files++; /* push input stack */
        @<Try to open include file, abort push if unsuccessful, go to |restart|@>;
    }
}


@ When an \.{@@i} line is found in the file, we must temporarily
stop reading it and start reading from the named include file.  The
\.{@@i} line should give a complete file name with or without
double quotes.
If the environment variable \.{CWEBINPUTS} is set, or if the compiler flag 
of the same name was defined at compile time,
\.{CWEB} will look for include files in the directory thus named, if
it cannot find them in the current directory.
(Colon-separated paths are not supported.)
The remainder of the \.{@@i} line after the file name is ignored.

@d too_long() {total_include_files--; free(new_inc);
        err_print(i, "! Include file name too long"); goto restart;}

@<Try to open...@>=
{
    include_description *new_inc;
    char temp_file_name[max_file_name_length]; 
    char *file_name_end;
    char *k, *kk;
    int l; /* length of file name */
  
    new_inc=(include_description *) malloc(sizeof(include_description));
    if (new_inc==NULL)
        fatal_error(i, "! No memory for new include descriptor", "");
    new_inc->line=0;
    k=new_inc->file_name;
    file_name_end=k+max_file_name_length-1;

    if (*inp_desc->loc=='"') {
        inp_desc->loc++;
        while (*inp_desc->loc!='"' && k<=file_name_end)
            *k++=*inp_desc->loc++;
        if (inp_desc->loc==inp_desc->limit)
            k=file_name_end+1; /* unmatched quote is `too long' */
    } else
        while (*inp_desc->loc!=' '&&*inp_desc->loc!='\t'&&
               *inp_desc->loc!='"'&&k<=file_name_end) *k++=*inp_desc->loc++;
    if (k>file_name_end) too_long();
@.Include file name ...@>
    *k='\0';
    if ((new_inc->the_file=fopen(new_inc->file_name, "r"))!=NULL) {
        new_inc->parent=inp_desc->current_include; /* link it in */
        inp_desc->current_include=new_inc;
        goto restart; /* success */
    }
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
    total_include_files--;
    free(new_inc);
    err_print(i, "! Cannot open include file");
@.Cannot open include file@>
    goto restart;
}



@* Reporting errors to the user.
There may be errors if a line in a given change
file does not match a line in the master file or a
replacement in a previous change file.  Such errors are
reported to the user by saying
$$
   \hbox{|err_print(file_no, "! Error message")|;}
$$
where |file_no| is the number of the file which is concerned by the
error.  Please note that no trailing dot is supplied in the error
message because it is appended by |err_print|.

@<Predecl...@>=
void err_print();

@ Here is the outline of the |err_print| function.

@<Error handling...@>=
void err_print(i, s) /* prints `\..' and location of error message */
file_index i; char *s;
{
  char *k, *l; /* pointers into an appropriate |buffer| */
  fprintf(stderr, *s=='!'? "\n%s" : "%s", s);
  if(i>=0) @<Print error location based on input buffer@>@;
  else putc('\n', stderr);
  fflush(stderr);
  history=troublesome;
}

@ The error locations can be indicated by using the variables
|loc|, |line| and |file_name| within the appropriate file description
structures, which tell respectively the first
unlooked-at position in the |buffer|, the current line number and the
current file.  We can determine whether we are looking at an included
file or not by examining the |current_include| variable.
This routine should be modified on systems whose standard text editor
has special line-numbering conventions.
@^system dependencies@>

@<Print error location based on input buffer@>=
{
    register input_description *inp_desc=input_organisation[i];
    register include_description *inc_desc=inp_desc->current_include;

    if (inc_desc!=NULL) {
        fprintf(stderr, ". (l. %ld of include file %s", inc_desc->line,
            inc_desc->file_name);
        fprintf(stderr, " included from l. %ld of %s file %s)\n",
            inp_desc->line,
            inp_desc->type_of_file==master?"master":"change",
            inp_desc->file_name);
    }
    else 
        fprintf(stderr, ". (l. %ld of %s file %s)\n", inp_desc->line,
            inp_desc->type_of_file==master?"master":"change",
            inp_desc->file_name);
    l=(inp_desc->loc>=inp_desc->limit? inp_desc->limit: inp_desc->loc);
    if (l>inp_desc->buffer) {
        for (k=inp_desc->buffer; k<l; k++)
            if (*k=='\t') putc(' ', stderr);
            else putc(*k, stderr); /* print the characters already read */
        putc('\n', stderr);
        for (k=inp_desc->buffer; k<l; k++)
            putc(' ', stderr); /* space out the next line */
    }
    for (k=l; k<inp_desc->limit; k++)
        putc(*k, stderr); /* print the part not yet read */
    putc('\n', stderr);
}


@ Non recoverable errors are handled by calling |fatal_error| that
outputs a message and then calls `|wrap_up|' and exits.  |err_print|
will print the error message followed by an indication of where the
error was spotted in the source files.  |fatal_error| cannot state any
files because the problem is usually to access these.

@d fatal_error(i, s, t) {
    fprintf(stderr, "\n%s", s);
    err_print(i, t);
    history=fatal;
    exit(wrap_up());
    }


@ Some implementations may wish to pass the |history| value to the
operating system so that it can be used to govern whether or not other
programs are started. Here, for instance, we pass the operating system
a status of~0 if and only if only harmless messages were printed.
@^system dependencies@>

@<Internal func...@>=
int wrap_up()
{
    @<Print the job |history|@>;
    if (history > spotless) return 1;
    else return 0;
}

@ Always good to prototype.

@<Predecl...@>=
int wrap_up();

@ We report the history to the user, although this may not
be ``\UNIX/'' style---but we are in good company: \.{WEB} and
\TEX/ do the same.  We put this on |stdout| rather than |stderr|, so
that users can easily filter this away if they wish.
@^system dependencies@>

@<Print the job |history|@>=
switch (history) {
  case spotless:
    printf("\n(No errors were found.)\n"); break;
  case troublesome:
    printf("\n(Pardon me, but I think I spotted something wrong.)\n"); break;
  case fatal: printf("(That was a fatal error, my friend.)\n");
} /* there are no other cases */


@ If there's a system error, we may be able to give the user more
information with the |pfatal_error| function.  This prints out system
error information if it is available.

@<Predecl...@>=
void pfatal_error();

@ @<Error handling...@>=
void pfatal_error(s, t)
char *s, *t;
{
    char *strerr=strerror(errno);

    fprintf(stderr, "\n%s%s", s, t);
    if (strerr) fprintf(stderr, " (%s)\n", strerr);
    else fprintf(stderr, "\n");
    history=fatal;
    exit(wrap_up());
}


@ We need an include file for the above.

@<Global \&{\#include}s@>=
#include <errno.h>


@* Handling multiple change files.
In the standard version we take the name of the
files from the command line.
It is assumed that filenames can be used as given in the
command line without changes.

First there are some sections to open all files.
If a file is not accessible, the run will be aborted.
Otherwise the name of the open file will be displayed.

@<Prepare the output file@>=
{
    out_file=fopen(out_name, "w");
    if (out_file==NULL) {
         pfatal_error("! Cannot open/create output file", "");
@.Cannot open/create output file@>
    }
}


@ The name of the file and the file desciptor are stored in
global variables.

@<Global variables@>=
FILE *out_file;
string out_name;


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
    printf("(%s)\n", input_organisation[0]->file_name);
    input_organisation[0]->type_of_file=master;
    get_line(0, true);
}


@ For the change files we must skip any comment part and see whether
there are any changes in it.  This is done by |init_change_file|.

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
        printf("(%s)\n", input_organisation[i]->file_name);
        init_change_file(i);
        i++;
    }
}


@*Input/output organisation.
Here's a simple function that checks if two lines
are different.

@<Internal functions@>=
boolean lines_dont_match(i, j)
        file_index i, j;
{
    register input_description *iptr=input_organisation[i],
                               *jptr=input_organisation[j];

    if (iptr->limit-iptr->buffer != jptr->limit-jptr->buffer)
        return true;
    return strncmp(iptr->buffer, jptr->buffer, iptr->limit-iptr->buffer);
}


@ Function |init_change_file(i)| is used to ignore all
lines of the input file with index~|i| until the next change
module is found.

@<Internal functions@>=
void init_change_file(i)
        file_index i;
{
    register input_description *inp_desc=input_organisation[i];
    char ccode;

    inp_desc->limit=inp_desc->buffer;
    @<Skip over comment lines; |return| if end of file@>@;
    @<Skip to the next nonblank line; |return| if end of file@>@;
    inp_desc->dont_match=0;
}


@ While looking for a line that begins with \.{@@x} in the change
file, we allow lines that begin with \.{@@}, as long as they don't
begin with \.{@@y}, \.{@@z} or \.{@@i} (which would probably mean that
the change file is fouled up).

@<Skip over comment lines...@>=
while(1) {
    if (!get_line(i, false)) return; /* end of file reached */
    if (inp_desc->limit<inp_desc->buffer+2) continue;
    if (inp_desc->buffer[0]!='@@') continue;
    ccode=inp_desc->buffer[1];
    if (xisupper(ccode)) ccode=tolower(ccode);
    if (ccode=='x') break;
    if (ccode=='y' || ccode=='z' || ccode=='i') {
        inp_desc->loc=inp_desc->buffer+2;
        err_print(i, "! Missing @@x in change file");
@.Missing @@x...@>
    }
}

@ Here we are looking at lines following the \.{@@x}.

@<Skip to the next nonblank line...@>=
do {
    if (!get_line(i, true)) {
        err_print(i, "! Change file ended after @@x");
@.Change file ended...@>
        return;
    }
} while (inp_desc->limit==inp_desc->buffer);


@ The |put_line| function is used to write a line from
input buffer |j| to the output file.

@<Internal functions@>=
void put_line(j)
       file_index j;
{
    char *ptr=input_organisation[j]->buffer;
    char *lmt=input_organisation[j]->limit;

    while(ptr<lmt) putc(*ptr++, out_file);
    putc('\n', out_file);
}


@ The function |e_of_ch_module| returns true if the input
line from file |i| starts with \.{@@z}.

@<Internal functions@>=
boolean e_of_ch_module(i)
        file_index i;
{
    register input_description *inp_desc=input_organisation[i];

    if (inp_desc->limit==NULL) {
        err_print(i, "! Change file ended without @@z");
@.Change file ended without @@z@>
        return true;
    } else if (inp_desc->limit>=inp_desc->buffer+2)
        if (inp_desc->buffer[0]=='@@' &&
            (inp_desc->buffer[1]=='Z' || inp_desc->buffer[1]=='z'))
        return true;
    return false;
}


@ The function |e_of_ch_preamble| returns |true| if the input
line from file~|i| starts with \.{@@y}.

@<Internal functions@>=
boolean e_of_ch_preamble(i)
        file_index i;
{
    register input_description *inp_desc=input_organisation[i];

    if (inp_desc->limit>=inp_desc->buffer+2 && inp_desc->buffer[0]=='@@')
        if (inp_desc->buffer[1]=='Y'||inp_desc->buffer[1]=='y') {
            if (inp_desc->dont_match>0) {
                inp_desc->loc=inp_desc->buffer+2;
                fprintf(stderr, "\n! Hmm... %d ", inp_desc->dont_match);
                err_print(i, "of the preceding lines failed to match");
            }
            return true;
        }
    return false;
}



@ To process the input file the next section reads a line of the
current (actual) input file and updates the |input_organisation| for
all files with index greater than |actual_input|.

@<Process a line, |break| when end of source reached@>=
{
    file_index test_file;

    @<Check the current files for any ends of changes@>@;
    if (input_has_ended && actual_input==0) break; /* all done */
    @<Scan all other files for changes to be done@>@;
    @<Handle output@>@;
    @<Step to next line@>@;
}


@ Any of the current change files may have reached the end of the
current change.  In such a case, intermediate lines must be skipped
and the next start of change is to be found.  This may make a change
file become inactive if the end of the file is reached.

@<Check the...@>=
{
    register input_description *inp_desc;
    while (actual_input>0 && e_of_ch_module(actual_input)) {
        inp_desc=input_organisation[actual_input];
        if (inp_desc->type_of_file==master) {
          /* emergency exit, everything mixed up!*/
          fatal_error(-1, "! This can't happen: change file is master file", "");
@.This can't happen...@>
        }
        inp_desc->mode=search;
        init_change_file(actual_input);
        while ((input_organisation[actual_input]->mode!=reading
                 && actual_input>0))
            actual_input--;
    }
}


@ Now we will set |test_input| to the first change file that is being
tested against the current line.  If no other file is testing, then
|actual_input| refers to a line to write and |test_input| is set to
|none|.

@d none  (-1)

@<Scan all other files...@>=
test_input=none;
test_file=actual_input;
while (test_input==none && test_file<no_ch-1){
    test_file++;
    switch (input_organisation[test_file]->mode) {
      case search:
        if (lines_dont_match(actual_input, test_file)==false) {
            input_organisation[test_file]->mode=test;
            test_input=test_file;
        }
        break;
      case test:
        if (lines_dont_match(actual_input, test_file)) {
            /* error, sections do not match; just note at this point */
            input_organisation[test_file]->dont_match++;
        }
        test_input=test_file;
        break;
      case reading: /* this can't happen */
        break;
      case ignore: /* nothing to do */
        break;
    }
}


@ For the output we must distinguish between whether we are creating a
new change file or a new master file.  Change file creation requires
closer inspection because we may be before a change, in the pattern
(match) part or in the replacement part.  For master file creation, we
simply have to write the line from the current (actual) input.

@<Handle output@>=
if (prod_chf==chf) {
    while(1) {
        @<Test for |normal|, |break| when done@>@;
        @<Test for |pre|, |break| when done@>@;
        @<Test for |post|, |break| when done@>@;
    }
} else
    if (test_input==none) put_line(actual_input);


@ Check whether we have to start a change file entry.
Without a match nothing needs to be done.

@<Test for |normal|...@>=
if (out_mode==normal) {
    if (test_input!=none) {
        fprintf(out_file, "@@x\n");
        out_mode=pre;
    } else break;
}


@ Check whether we have to start the replacement text.  This is the
case when we are in |pre| mode but have no more matching lines.
Otherwise the master file source line must be copied to the change
file.

@<Test for |pre|...@>=
if (out_mode==pre) {
    if (test_input==none) {
        fprintf(out_file, "@@y\n");
        out_mode=post;
    } else {
        if (input_organisation[actual_input]->type_of_file==master)
            put_line(actual_input);
        break;
    }
}


@ Check whether an entry from a change file is complete.  If the
current input is from a change file which is not being tested against
a later change file, then this change file line must be written.  If
the actual input has been reset to the master file, we can finish this
change.

@<Test for |post|...@>=
if (out_mode==post) {
    if (input_organisation[actual_input]->type_of_file==chf) {
        if (test_input==none)  put_line(actual_input);
        break;
    } else {
        fprintf(out_file, "@@z\n\n");
        out_mode=normal;
    }
}


@ If we had a change, we must proceed in the actual file
to be changed and in the change file in effect.

@<Step to next line@>=
get_line(actual_input, true);
if (test_input!=none) {
    get_line(test_input, true);
    if (e_of_ch_preamble(test_input)==true) {
        get_line(test_input, true); /* update current changing file */
        input_organisation[test_input]->mode=reading;
        actual_input=test_input;
        test_input=none;
    }
}


@ To create the new output file we have to scan the whole
master file and all changes in effect when it ends.
At the very end it is wise to check for all changes
to have completed, in case the last line of the master file
was to be changed.

@<Process the input@>=
actual_input=0;
input_has_ended=false;
while (input_has_ended==false||actual_input!=0)
    @<Process a line...@>@;
if (out_mode==post) /* last line has been changed */
    fprintf(out_file, "@@z\n");


@ At the end of the program, we will tell the user if the
change file had a line that didn't match any relevant line
in the master file or any of the change files.

@<Check that all changes have been read@>=
{
    file_index i;

    for (i=1;i<no_ch;i++) { /* all change files */
        if (input_organisation[i]->mode!=ignore) {
            input_organisation[i]->loc=input_organisation[i]->buffer;
            err_print(i, "! Change file entry did not match");
@.Change file entry ...@>
        }
    }
}


@ We want to tell the user about our command line options if they made
a mistake.  This is done by the |usage_error()| function.  It contains
merely the necessary print statements and exits afterwards.

@<Intern...@>=
void usage_error()
{
    @<Print the banners@>;
    fprintf(stderr, "Usage: ctie -[mc] outfile master changefile(s)\n");
    fprintf(stderr, "Type ctie --help for more information\n");
    exit(1);
}


@ Printing our welcome banners; we only do this if we are not asked
for version or help information.

@<Print the banners@>=
printf("%s\n", banner); /* print a ``banner line'' */
printf("%s\n", copyright); /* include the copyright notice */


@ We must scan through the list of parameters, given in |argv|.  The
number is in |argc|.  We must pay attention to the flag parameter.  We
need at least 3~parameters (\.{-m} or \.{-c}, an output file and a
master file) and can handle up to |max_file_index| change files.  The
names of the file parameters will be inserted into the structure of
|input_organisation|.  The first file is special.  It indicates the
output file.  When we allow flags at any position, we must find out
which name is for what purpose.  The master file is already part of
the |input_organisation| structure (index~0).  As long as the number
of files found (counted in |no_ch|) is |-1| we have not yet found the
output file name.

@<Scan the parameters@>=
{
    if (argc>max_file_index+5-1)  usage_error();
    no_ch = -1; /* fill this part of |input_organisation| */
    while (--argc>0) {
        argv++;
        if (strcmp("-help", *argv)==0 || strcmp("--help", *argv)==0)
            @<Display help message and exit@>;
        if (strcmp("-version", *argv)==0 || strcmp("--version", *argv)==0)
            @<Display version information and exit@>;
        if (**argv=='-') @<Set a flag@>@;
        else @<Get a file name@>@;
    }
    if (no_ch<=0|| prod_chf==unknown)  usage_error();
}


@ The flag is about to determine the processing mode.
We must make sure that this flag has not been set before.
Further flags might be introduced to avoid/force overwriting of
output files.
Currently we just have to set the processing flag properly.

@<Set a flag@>=
if (prod_chf!=unknown)  usage_error();
else
    switch (*(*argv+1)) {
      case 'c': case 'C': prod_chf=chf;    break;
      case 'm': case 'M': prod_chf=master; break;
      default:  usage_error(); 
    }


@ We have to distinguish whether this is the very first file name
(which is the case if |no_ch==(-1)|) or if the next element of
|input_organisation| must be filled.

@<Get a file name@>=
{
    if (no_ch==(-1)) {
        out_name=*argv;
    } else {
        register input_description *inp_desc;

        inp_desc=(input_description *) malloc(sizeof(input_description));
        if (inp_desc==NULL)
            fatal_error(-1, "! No memory for input descriptor", "");
@.No memory for descriptor@>
        inp_desc->mode=search;
        inp_desc->line=0;
        inp_desc->type_of_file=chf;
        inp_desc->limit=inp_desc->buffer;
        inp_desc->buffer[0]=' ';
        inp_desc->loc=inp_desc->buffer+1;
        inp_desc->buffer_end=inp_desc->buffer+buf_size-2;
        inp_desc->file_name=*argv;
        inp_desc->current_include=NULL;
        input_organisation[no_ch]=inp_desc;
    }
    no_ch++;
}


@ Modules for dealing with help messages and version info.  We follow
the \.{kpathsea} standard code here, so that we can easily adapt this
to work with \.{kpathsea}.

@<Display help message and exit@>=
usage_help();
@.--help@>


@ 
@<Display version information and exit@>=
{
    print_version_and_exit("CTIE", version_number);
@.--version@>
}


@ Here is the usage information for \.{--help}.

@<Global variables@>=
string CTIEHELP[] = {
    "Usage: ctie -[mc] outfile master changefile(s)",
    "  Create a new master file or change file from the given",
    "  master (C)WEB file and changefiles.",
    "  All filenames are taken literally; no suffixes are added.",
    "",
    "-m  create a new master file from original (C)WEB and change file(s)",
    "-c  create a master change file for original (C)WEB file from changefile(s)",
    "--help      display this help and exit",
    "--version   display version information and exit",
    NULL
};


@ @<Predec...@>=
void usage_help();
void print_version_and_exit();


@ @c
void usage_help()
{
    string *message=CTIEHELP;

    while (*message) {
        fputs(*message, stdout);
        putchar('\n');
        ++message;
    }
    putchar('\n');
    exit(0);
}


@ @c
void print_version_and_exit(name, version)
        string name, version;
{
    printf ("%s %s\n", name, version);

    puts ("Copyright (C) 2002,2003 Julian Gilbey.");

    puts ("There is NO warranty.  This is free software.  See the source");
    puts ("code of CTIE for redistribution conditions.");

    exit (0);
}


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


@* Index.
