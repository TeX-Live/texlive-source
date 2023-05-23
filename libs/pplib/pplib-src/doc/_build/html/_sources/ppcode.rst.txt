Examples
========

Example 1
---------

.. literalinclude:: ../src/pptest1.c

Example 2
---------

.. literalinclude:: ../src/pptest2.c

Example 3
---------

.. literalinclude:: ../src/pptest3.c

ppapi.h
-------

.. literalinclude:: ../src/ppapi.h


Changes
=======

v0.97
-----
First release integrated with luatex sources, plus portability changes from Luigi.

v0.98
-----
Changed references resolving in case of incremental updates; tech notes ppxref_find() in ppxref.c.

v0.99
-----
Fixed streams handling; null characters should NOT be gobbled after "stream" keyword

v1.00
-----
Fixed streams handling (Luigi); object streams updated before other streams
Revised streams handling, ppstream API extended

v1.01
-----
Fixed names handling (thanks Hans); digits after '#' weren't skipped

v1.02
-----
Fixed page finder (thanks Luigi)

v1.03
-----
Fixed TIFF predictor (thanks folks)

v1.04
-----
Fixed TIFF predictor for ARM

v1.05
-----
Attempt to fix parsing inline images crap

v2.00
-----
Deep rework on allocators.
Deep rework on strings and names.

v2.01
-----
Fixed invalid stream buffer handling; ``iof_discard()`` no longer reclaims the source (``filter->next``) filter.
Sanity alignment adjustments in ``iof_heap``.

v2.02
-----
Fixed incorrect encoding of strings alterego with octal escaping, thanks Luigi.
On Hans request added ``ppdoc_filehandle()`` function and ``ppdoc_file()`` macro for loading ppdoc from ``FILE *``.

v2.03
-----
Fixed alloc/free of ``ppdoc`` heap; ``ppdoc`` is now mallocated, no longer taken from its own heap. Again, thanks Luigi.
Fixed warnings about dereferencing type-puned pointers, and some others.

v2.04
-----
Byte lookups for names/strings loaders are now int8_t, as char might be signed.
ppdoc_first_page() / ppdoc_next_page() iterator now handles a case when /Kids array is empty.
Fixed generating keys for encrypted streams; ppstring_internal() returns the string of the proper size.
More stream tests in pptest3.c.
Some rework on md5 and sha2.

v2.05
-----
uint8_t instead of ppbyte in internals; ppbyte intent is "the most natural 8-bit integer", so it is 'char',
but internally we almost always need uint8_t (char may be signed or not..)

v2.10
-----
Rework on encryption; algorithms /V5 /R6.
Passwords passed to ppdoc_crypt_pass() should be UTF-8 (backward incompatible).
