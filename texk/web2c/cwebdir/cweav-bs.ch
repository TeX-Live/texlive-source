Changes for CWEAVE for MSDOS and Borland C++ 3.1 using the following
options (and perhaps others):

    -mc -w-pro -Ff=5000 -Z- -O-p

The options -Z- and -O-p explicitly turn off optimizations that break
the code.  (See makefile.bs.)

The main purpose of these changes is to support MSDOS with full-size arrays
by using "huge" pointers.

This file contributed by Barry Schwartz, trashman@crud.mn.org, 28 Jun 94.
(Includes workaround for compiler bug [pointers wrapping around at
 segment boundaries], November 1993, contributed by Jorge Fernandez Arnaiz
 -- arnaiz@redvax1.dgsca.unam.mx)
(Last revised 5 Dec 94 with help of bob@microprograms.com.)


@x Section 1.
The ``banner line'' defined here should be changed whenever \.{CWEAVE}
is modified.

@d banner "This is CWEAVE (Version 4.9)"
@y
The ``banner line'' defined here should be changed whenever \.{CWEAVE}
is modified.

@d banner "This is CWEAVE (Version 4.9pc/big)"
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


@x Section 22.
typedef struct xref_info {
  sixteen_bits num; /* section number plus zero or |def_flag| */
  struct xref_info *xlink; /* pointer to the previous cross-reference */
} xref_info;
typedef xref_info *xref_pointer;
@y
typedef struct xref_info {
  sixteen_bits num; /* section number plus zero or |def_flag| */
  struct xref_info huge* xlink; /* pointer to the previous cross-reference */
} xref_info;
typedef xref_info huge* xref_pointer;
@z


@x Section 23.
static xref_info xmem[max_refs]; /* contains cross-reference information */
static xref_pointer xmem_end = xmem+max_refs-1;
@y
static xref_info huge xmem[max_refs]; /* contains cross-reference information */
static xref_pointer xmem_end;
@z


@x Section 24.
@d xref equiv_or_xref
@y
@d xref ptr_union.xref_member
@z


@x Section 24.
xref_ptr=xmem; init_node(name_dir); xref_switch=section_xref_switch=0;
xmem->num=0; /* sentinel value */
@y
xmem_end = xmem + max_refs - 1;
xref_ptr=xmem; init_node(name_dir); xref_switch=section_xref_switch=0;
xmem->num=0; /* sentinel value */
@z


@x Section 30. (to please Borland's C++, version 4.02)
static token tok_mem[max_toks]; /* tokens */
static token_pointer tok_mem_end = tok_mem+max_toks-1; /* end of |tok_mem| */
static token_pointer tok_ptr; /* first unused position in |tok_mem| */
static token_pointer max_tok_ptr; /* largest value of |tok_ptr| */
static token_pointer tok_start[max_texts]; /* directory into |tok_mem| */
static text_pointer tok_start_end = tok_start+max_texts-1; /* end of |tok_start| */
static text_pointer text_ptr; /* first unused position in |tok_start| */
@y
static token tok_mem[max_toks]; /* tokens */
static token_pointer tok_mem_end; /* end of |tok_mem| */
static token_pointer tok_ptr; /* first unused position in |tok_mem| */
static token_pointer max_tok_ptr; /* largest value of |tok_ptr| */
static token_pointer tok_start[max_texts]; /* directory into |tok_mem| */
static text_pointer tok_start_end; /* end of |tok_start| */
static text_pointer text_ptr; /* first unused position in |tok_start| */
@z

@x Section 31. (goes with the previous change)
tok_ptr=max_tok_ptr=tok_mem+1;@/
tok_start[0]=tok_start[1]=tok_mem+1;@/
text_ptr=max_text_ptr=tok_start+1;
@y
tok_start[1]=tok_mem+1;
tok_mem_end=tok_mem+max_toks-1;
tok_start_end=tok_start+max_texts-1;
max_tok_ptr=tok_mem+1; max_text_ptr=tok_start+1;
@z


@x Section 32.
  p->xref=(void *)xmem;
@y
  p->xref=(void huge*)xmem;
@z


@x Section 78.
      if (unindexed(lhs)) { /* retain only underlined entries */
        xref_pointer q,r=NULL;
        for (q=(xref_pointer)lhs->xref;q>xmem;q=q->xlink)
          if (q->num<def_flag)
            if (r) r->xlink=q->xlink;
            else lhs->xref=(void *)q->xlink;
          else r=q;
      }
@y
      if (unindexed(lhs)) { /* retain only underlined entries */
        xref_pointer q,r=NULL;
        for (q=(xref_pointer)lhs->xref;q>xmem;q=q->xlink)
          if (q->num<def_flag)
            if (r) r->xlink=q->xlink;
            else lhs->xref=(void huge*)q->xlink;
          else r=q;
      }
@z


@x Section 97.
  char *k, *k_end=(p+1)->byte_start; /* pointers into |byte_mem| */
  out('{');
  for (k=p->byte_start; k<k_end; k++) {
@y
  char huge* k, huge* k_end=(p+1)->byte_start; /* pointers into |byte_mem| */
  out('{');
  for (k=p->byte_start; k<k_end; k++) {
@z


@x Section 218.
  char *p; /* index into |byte_mem| */
@y
  char huge *p; /* index into |byte_mem| */
@z


@x Section 253.
    if (cur_name->xref!=(void *)xmem) {
@y
    if (cur_name->xref!=(void huge*)xmem) {
@z


@x Section 255.
static char *cur_byte; /* index into |byte_mem| */
@y
static char huge* cur_byte; /* index into |byte_mem| */
@z


@x Section 264.
switch (cur_name->ilk) {@+char *j;@+@t}\6{\4@>
@y
switch (cur_name->ilk) {@+char huge* j;@+@t}\6{\4@>
@z
