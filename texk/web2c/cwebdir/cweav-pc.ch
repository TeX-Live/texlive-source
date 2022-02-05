This is the change file for CWEB's CWEAVE under DOS
(Contributed by Lee Wittenberg, March 1993)
(Also includes workaround for compiler bug, November 1993,
 contributed by Jorge Fernandez Arnaiz -- arnaiz@redvax1.dgsca.unam.mx)

Changes necessary for compiling with Borland C/C++
Use compilation switches -mc -w-pro -Ff=5000

Note: The changes to section 4 are not necessary if using a compiler
that allows >64K arrays. (If you need lots more bytes, try the alternate
change files that have -bs in their name instead of -pc.)

@x section 1
@d banner "This is CWEAVE (Version 4.7)"
@y
@d banner "This is CWEAVE (Version 4.7pc)"
@z

@x section 17
@d max_bytes 100000 /* the number of bytes in identifiers,
@y
@d max_bytes (unsigned)60000 /* the number of bytes in identifiers,
@z
@x
@d max_refs 30000 /* number of cross-references; must be less than 65536 */
@y
@d max_refs 10000 /* number of cross-references; must be less than 65536 */
@z
