This is the change file for CWEB's CTANGLE under Win32
(Contributed by Fabrice Popineau, February 2002)

Changes necessary for compiling with Borland C/C++

@x section 1
@d banner "This is CTANGLE (Version 4.0)"
@y
@d banner "This is CTANGLE (Version 4.0win32)"
@z

@x
boolean names_match(@t\1\1@>
name_pointer p, /* points to the proposed match */
const char *first, /* position of first character of string */
size_t l, /* length of identifier */
eight_bits t@t\2\2@>) /* not used by \.{TANGLE} */
@y
boolean __cdecl names_match(@t\1\1@>
name_pointer p, /* points to the proposed match */
const char *first, /* position of first character of string */
size_t l, /* length of identifier */
eight_bits t@t\2\2@>) /* not used by \.{TANGLE} */
@z

@x
void
init_node(
name_pointer node)
{
    node->equiv=(void *)text_info;
}
void
init_p(name_pointer p,eight_bits t) {@+(void)p;@+(void)t;@+}
@y
void
init_node(
name_pointer node)
{
    node->equiv=(void *)text_info;
}
void __cdecl
init_p(name_pointer p,eight_bits t) {@+(void)p;@+(void)t;@+}
@z
