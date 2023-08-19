This is the change file for CWEB's CTANGLE under DOS
(Contributed by Lee Wittenberg, March 1993)

Changes necessary for compiling with Borland C/C++
Use compilation switches -mc -w-pro -Ff=5000

Note: The changes to section 4 are not necessary if using a compiler
that allows >64K arrays. If you need lots of bytes and toks, try the
alternate change files with -bs suffix instead of -pc.

@x section 1
@d banner "This is CTANGLE (Version 4.10)"
@y
@d banner "This is CTANGLE (Version 4.10pc)"
@z
@x section 17
@d max_bytes 100000 /* the number of bytes in identifiers,
  index entries, and section names; must be less than $2^{24}$ */
@y (note that CWEAVE itself needs only about 42K toks)
@d max_bytes (unsigned)60000 /* the number of bytes in identifiers,
  index entries, and section names; must be less than $2^{24}$ */
@z
@x section 20
@d max_toks 270000 /* number of bytes in compressed \CEE/ code */
@y
@d max_toks (unsigned)60000 /* number of bytes in compressed \CEE/ code */
@z
