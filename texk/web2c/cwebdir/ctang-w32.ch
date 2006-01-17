This is the change file for CWEB's CTANGLE under Win32
(Contributed by Fabrice Popineau, February 2002)

Changes necessary for compiling with Borland C/C++

@x section 1
@d banner "This is CTANGLE (Version 3.64)\n"
@y
@d banner "This is CTANGLE (Version 3.64win32)\n"
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

@x
int names_match(p,first,l)
name_pointer p; /* points to the proposed match */
char *first; /* position of first character of string */
@y
int __cdecl names_match(p,first,l)
name_pointer p; /* points to the proposed match */
char *first; /* position of first character of string */
@z

@x
void
init_node(node)
name_pointer node;
{
    node->equiv=(char *)text_info;
}
void
init_p() {}
@y
void
init_node(node)
name_pointer node;
{
    node->equiv=(char *)text_info;
}
void __cdecl
init_p() {}
@z

@x
@ @<Include...@>=
#include <ctype.h> /* definition of |isalpha|, |isdigit| and so on */
#include <stdlib.h> /* definition of |exit| */
@y
@ @<Include...@>=
#include <ctype.h> /* definition of |isalpha|, |isdigit| and so on */
#include <stdlib.h> /* definition of |exit| */
#include <string.h>
@z
