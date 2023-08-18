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

@x section 11
extern boolean names_match(name_pointer,const char *,size_t,eight_bits);
@y
extern boolean __cdecl names_match(name_pointer,const char *,size_t,eight_bits);
@z

@x section 75
An omitted change file argument means that |"/dev/null"| should be used,
@y
An omitted change file argument means that |"NUL"| should be used,
@z

@x section 75
  strcpy(change_file_name,"/dev/null");
@y
  strcpy(change_file_name,"NUL");
@z

@x section 75 (this change copied from comm-bs.ch, July 94)
        else if (*s=='/') dot_pos=NULL,name_pos=++s;
@y
        else if (*s == ':' || *s == '\\' || *s == '/')
	  dot_pos=NULL,name_pos=++s;
@z
