
1.x vs 2.x
==========

Allocation
----------

Allocation mechanisms has been revised. All objects are still allocated from a private storage (called heap),
but now the heap serves properly aligned memory chunks. Implementation of the heap is (hopefully) platform
independent, so all ``ARM_COMPLIANT`` checks has been removed. Some details work differently on 32/64-bit
machines, this distinction seems inevitable. Allocation related code has been moved to ``util/utilmem*.c`` files.

[There are actually 3 different allocators - **heap**, **stock**, and **pool** - each of which has 4 variants
for 1, 2, 4 and 8 bytes alignment. So far ``pplib`` uses only the **heap**, but I want to have this part
in a single place for a while. More notes about allocators in ``utilmemallc.h``.]

``pplib`` uses two heaps:

* bytes heap - providing 2-bytes aligned memory chunks for byte data
* structures heap - providing 8-bytes aligned chunks for all structures

2-bytes alignment of byte data is caused by implementation details / limits, ``pplib`` doesn't make a use of it
(could be unaligned as well). All the structures are stored in 8-bytes aligned chunks on both 32 and 64-bit machines
(in some structures we use ``double`` and ``int64_t``, which generally needs 8-bytes).

Strings And Names
-----------------

So far, both ``ppname`` and ``ppstring`` were aliases to ``char *`` with an extra structure hidden just below
the data address. The idea was to make a string or name usable whenever chars array can, at the cost of some
pointers trickery. Both names and strings keep an information about pdf-encoded data, as well as plain bytes data.
I implemented this feature once ``pplib 1.0`` was almost done, and to avoid a revolution, I made that with yet
another trick. This all was dirty and caused lots of alignment issues.

Now ``ppstring`` and ``ppname`` are **structures**, keeping data and size members, pretty much like ``pparray``::

  struct ppname {
    ppbyte *data;
    size_t size;
    ppname *alterego;
    int flags;
  };

  struct ppstring {
    ppbyte *data;
    size_t size;
    ppstring *alterego;
    int flags;
  };

Data member is a pointer to (2-bytes aligned) bytes array -- ``ppbyte *``. ``ppbyte`` is an alias to ``char``.
``pplib`` makes no assumptions about ``ppbyte`` signedness. [I'd prefer to have ``uint8_t``, but better
keep that in sync with ``ppdict_get_*`` suite, which accepts ``char *``. Using ``"string literals"`` 
with ``ppdict_get`` function is the most common case, I guess.]

``ppname_size()`` and ``ppstring_size() `` now have
trivial ``ppname_data()`` and ``ppstring_data()`` counterparts::

  #define ppname_size(name) ((name)->size)
  #define ppname_data(name) ((name)->data)

  #define ppstring_size(string) ((string)->size)
  #define ppstring_data(string) ((string)->data)

Switching between encoded / decoded variants is made via explicit ``alterego`` member. If encoded and decoded forms
are identical, then ``self->alfterego == self`` (never NULL). Helper functions for that::

  PPAPI ppname * ppname_decoded (ppname *name);
  PPAPI ppname * ppname_encoded (ppname *name);

  PPAPI ppbyte * ppname_decoded_data (ppname *name);
  PPAPI ppbyte * ppname_encoded_data (ppname *name);

  PPAPI ppstring * ppstring_decoded (ppstring *string);
  PPAPI ppstring * ppstring_encoded (ppstring *string);

  PPAPI ppbyte * ppstring_decoded_data (ppstring *string);
  PPAPI ppbyte * ppstring_encoded_data (ppstring *string);

Since ``ppname`` and ``ppstring`` are now structures, all API functions returning name/string or taking name/string
as an argument, now take **a pointer to name/string**. Watch out for ``ppdict_get_*`` functions suite. They still accept
``const char *`` key as an argument, but ``ppname`` type can no longer be used there::

  ppname key;
  ppdict_get_something(dict, key);      // wrong
  ppdict_get_something(dict, key.data); // ok

  ppname *pkey;
  ppdict_get_something(dict, (const char *)pkey); // wrong
  ppdict_get_something(dict, pkey->data);         // ok

Consequently, functions / macros that used pointer to ``ppname`` (``ppdict_first()`` / ``ppdict_next()``),
now needs ``ppname **pname``.

Cleanups
--------

Some of unused utils were removed. There is still some code in utils part that is not used by the library,
but I need this tollbox in many other places and don't want to maintain two versions.

Some unification in integer types; ``size_t`` seems to be the best choice for and integer representing machine
word (``unsigned long`` is not long enough on windows), less utils dependencies, less compiler warnings.

