Changes for CTANGLE for MSDOS and Borland C++ 3.1 using the following
options (and perhaps others):

    -mc -w-pro -Ff=5000 -Z- -O-p

The options -Z- and -O-p explicitly turn off optimizations that break
the code.  (See makefile.bs.)

The main purpose of these changes is to support MSDOS with full-size arrays
by using "huge" pointers.

(This file contributed by Barry Schwartz, trashman@crud.mn.org, 28 Jun 94;
 revised 24 Jul 94.)


@x Section 1.
The ``banner line'' defined here should be changed whenever \.{CTANGLE}
is modified.

@d banner "This is CTANGLE (Version 4.9)"
@y
The ``banner line'' defined here should be changed whenever \.{CTANGLE}
is modified.

@d banner "This is CTANGLE (Version 4.9pc/big)"
@z


@x Section 10.
  for section names */

@<Common code...@>=
typedef struct name_info {
  char *byte_start; /* beginning of the name in |byte_mem| */
  struct name_info *link;
  union {
    struct name_info *Rlink; /* right link in binary search tree for section
      names */
    char Ilk; /* used by identifiers in \.{CWEAVE} only */
  } dummy;
  void *equiv_or_xref; /* info corresponding to names */
} name_info; /* contains information about an identifier or section name */
typedef name_info *name_pointer; /* pointer into array of \&{name\_info}s */
typedef name_pointer *hash_pointer;
extern char byte_mem[]; /* characters of names */
extern char *byte_mem_end; /* end of |byte_mem| */
extern char *byte_ptr; /* first unused position in |byte_mem| */
extern name_info name_dir[]; /* information about names */
extern name_pointer name_dir_end; /* end of |name_dir| */
extern name_pointer name_ptr; /* first unused position in |name_dir| */
extern name_pointer hash[]; /* heads of hash lists */
extern hash_pointer hash_end; /* end of |hash| */
extern hash_pointer h; /* index into hash-head array */
@y
  for section names */

@f huge extern

@<Common code...@>=
typedef struct name_info {
  char huge* byte_start; /* beginning of the name in |byte_mem| */
  struct name_info *link;
  union {
    struct name_info *Rlink; /* right link in binary search tree for section
      names */
    char Ilk; /* used by identifiers in \.{CWEAVE} only */
  } dummy;
  union {
    char *equiv_member;
    char huge* xref_member;
  } ptr_union; /* info corresponding to names */
} name_info; /* contains information about an identifier or section name */
typedef name_info *name_pointer; /* pointer into array of \&{name\_info}s */
typedef name_pointer *hash_pointer;
extern char huge byte_mem[]; /* characters of names */
extern char huge* byte_mem_end; /* end of |byte_mem| */
extern name_info name_dir[]; /* information about names */
extern name_pointer name_dir_end; /* end of |name_dir| */
extern name_pointer name_ptr; /* first unused position in |name_dir| */
extern char huge* byte_ptr; /* first unused position in |byte_mem| */
extern name_pointer hash[]; /* heads of hash lists */
extern hash_pointer hash_end; /* end of |hash| */
extern hash_pointer h; /* index into hash-head array */
@z


@x Section 19.
  eight_bits *tok_start; /* pointer into |tok_mem| */
  sixteen_bits text_link; /* relates replacement texts */
} text;
typedef text *text_pointer;
@y
  eight_bits huge* tok_start; /* pointer into |tok_mem| */
  sixteen_bits text_link; /* relates replacement texts */
} text;
typedef text *text_pointer;
@z


@x Section 20.
@ @d max_texts 4000 /* number of replacement texts, must be less than 10240 */
@d max_toks 270000 /* number of bytes in compressed \CEE/ code */
@y
@ (This is a modified version of \.{CTANGLE}, and in fact one of the parameters
has been reduced in value.  The parameter |max_toks|
has been reduced from 270000 [which
was sufficient to handle \TEX/] to
170000, so that \.{CTANGLE}
may be run on {\mc MSDOS}
systems that are tight on memory.  Consider, for
instance, an 80286-based machine with several TSRs and drivers, trying
to run \.{CTANGLE} from a makefile.)
@d max_texts 2500 /* number of replacement texts, must be less than 10240 */
@d max_toks 170000 /* number of bytes in compressed \CEE/ code */
@z
@x
static eight_bits tok_mem[max_toks];
static eight_bits *tok_mem_end=tok_mem+max_toks-1;
static eight_bits *tok_ptr; /* first unused position in |tok_mem| */
@y
static eight_bits huge tok_mem[max_toks];
static eight_bits huge* tok_mem_end;
static eight_bits huge* tok_ptr; /* first unused position in |tok_mem| */
@z


@x Section 21.
text_info->tok_start=tok_ptr=tok_mem;
text_ptr=text_info+1; text_ptr->tok_start=tok_mem;
  /* this makes replacement text 0 of length zero */
@y
tok_mem_end=tok_mem+max_toks-1;
text_info->tok_start=tok_ptr=tok_mem;
text_ptr=text_info+1; text_ptr->tok_start=tok_mem;
  /* this makes replacement text 0 of length zero */
@z


@x Section 22.
@d equiv equiv_or_xref /* info corresponding to names */
@y
@d equiv ptr_union.equiv_member /* info corresponding to names */
@z


@x Section 31.
  eight_bits *end_field; /* ending location of replacement text */
  eight_bits *byte_field; /* present location within replacement text */
@y
  eight_bits huge* end_field; /* ending location of replacement text */
  eight_bits huge* byte_field; /* present location within replacement text */
@z


@x Section 55.
out_char(
eight_bits cur_char)
{
  char *j, *k; /* pointer into |byte_mem| */
@y
out_char(cur_char)
eight_bits cur_char;
{
  char huge* j, huge* k; /* pointer into |byte_mem| */
@z
