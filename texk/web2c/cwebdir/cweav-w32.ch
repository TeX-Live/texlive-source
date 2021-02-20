This is the change file for CWEB's CWEAVE under Win32
(Contributed by Fabrice Popineau, February 2002)

@x section 1
@d banner "This is CWEAVE (Version 4.1)"
@y
@d banner "This is CWEAVE (Version 4.1win32)"
@z

@x
boolean names_match(@t\1\1@>
name_pointer p, /* points to the proposed match */
const char *first, /* position of first character of string */
size_t l, /* length of identifier */
eight_bits t@t\2\2@>) /* desired |ilk| */
@y
boolean __cdecl names_match(@t\1\1@>
name_pointer p, /* points to the proposed match */
const char *first, /* position of first character of string */
size_t l, /* length of identifier */
eight_bits t@t\2\2@>) /* desired |ilk| */
@z

@x
void
init_p(
name_pointer p,
eight_bits t)
@y
void __cdecl
init_p(
name_pointer p,
eight_bits t)
@z
