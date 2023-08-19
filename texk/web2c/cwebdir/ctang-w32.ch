This is the change file for CWEB's CTANGLE under Win32
(Contributed by Fabrice Popineau, February 2002)

Changes necessary for compiling with Borland C/C++

@x section 1
@d banner "This is CTANGLE (Version 4.10)"
@y
@d banner "This is CTANGLE (Version 4.10win32)"
@z

@x section 24
boolean names_match(
name_pointer p, /* points to the proposed match */
const char *first, /* position of first character of string */
size_t l, /* length of identifier */
eight_bits t) /* not used by \.{CTANGLE} */
@y
boolean __cdecl names_match(
name_pointer p, /* points to the proposed match */
const char *first, /* position of first character of string */
size_t l, /* length of identifier */
eight_bits t) /* not used by \.{CTANGLE} */
@z

@x section 25
void
init_node(
name_pointer node)
{
    node->equiv=(void *)text_info;
}
@y
void
init_node(
name_pointer node)
{
    node->equiv=(void *)text_info;
}
@z
