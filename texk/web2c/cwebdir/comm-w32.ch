This is the change file for CWEB's COMMON under Win32
(Contributed by Fabrice Popineau, February 2002 <Fabrice.Popineau@supelec.fr>)

The Microsoft C compiler included in Visual Studio allows for different
parameter passing conventions than the standard one. This is actually
specified with the Intel IA32 architecture. There exist three
calling conventions under the IA32 architecture as defined by Intel.

One of them is the standard C calling convention: Parameters are passed on
the stack, calling function is responsible to unstack arguments.  Names are
decorated with a prefixed underscore.  It is called the __cdecl convention.

Another calling convention is the __fastcall convention. The first two 32bits
arguments are passed on the stack.  The called function is responsible for
stack maintenance.  Names are decorated with an @ sign as prefix, and another
@ sign followed by the size of the arguments as suffix.

The third one is the __stdcall convention and is basically the Pascal calling
convention.

Using the __fastcall convention is usually faster on Intel architecture. Names
are decorated because obviously a __cdecl function can't behave the same way
as a __fastcall one of the same prototype.  So the new full prototype includes
the keyword __cdecl or __fastcall. A compiler option allows to compile all
functions using one or the other calling convention. If you compile using
__fastcall, then you are bound to explicitly declare a few functions as
__cdecl : the ones that are passed to the C library (like function pointers
for qsort() or signal()) or functions that replace functions from the C
library, because the standard headers have a __cdecl prototype for these
functions. Also, you cannot declare a library function without giving it the
exact prototype used in the library headers files. So you'd better use the
header files to be safe.

Admittedly, this is not vital for cweb, except that I build the whole texlive
set of programs using the __fastcall convention.

@x
@<Include files@>=
#include <ctype.h>
@y
@<Include files@>=
#include <ctype.h>
#include <string.h>
@z

@x
@ @<Predec...@>=
extern int names_match();
@y
@ @<Predec...@>=
extern int __cdecl names_match();
@z

@x
@<Pred...@>=
void init_p();
@y
@<Pred...@>=
void __cdecl init_p();
@z

@x section 69
An omitted change file argument means that |"/dev/null"| should be used,
@y
An omitted change file argument means that |"NUL"| should be used,
@z

@x section 70 (this change copied from comm-bs.ch, July 94)
        else if (*s=='/') dot_pos=NULL,name_pos=++s;
@y
        else if (*s == ':' || *s == '\\' || *s == '/')
	  dot_pos=NULL,name_pos=++s;
@z

@x section 70
  if (found_change<=0) strcpy(change_file_name,"/dev/null");
@y
  if (found_change<=0) strcpy(change_file_name,"NUL");
@z

@x
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
