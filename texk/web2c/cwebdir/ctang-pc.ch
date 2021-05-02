This is the change file for CWEB's CTANGLE under DOS
(Contributed by Lee Wittenberg, March 1993)

Changes necessary for compiling with Borland C/C++
Use compilation switches -mc -w-pro -Ff=5000

Note: The changes to section 4 are not necessary if using a compiler
that allows >64K arrays. If you need lots of bytes and toks, try the
alternate change files with -bs suffix instead of -pc.

@x section 1
@d banner "This is CTANGLE (Version 4.2)"
@y
@d banner "This is CTANGLE (Version 4.2pc)"
@z
@x section 4
@d max_bytes 1000000 /* the number of bytes in identifiers,
  index entries, and section names */
@d max_toks 1000000 /* number of bytes in compressed \CEE/ code */
@y (note that CWEAVE itself needs only about 42K toks)
@d max_bytes (unsigned)60000 /* the number of bytes in identifiers,
  index entries, and section names */
@d max_toks (unsigned)60000 /* number of bytes in compressed \CEE/ code */
@z
