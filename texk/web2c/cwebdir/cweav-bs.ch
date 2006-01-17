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

@d banner "This is CWEAVE (Version 3.64)\n"
@y
The ``banner line'' defined here should be changed whenever \.{CWEAVE}
is modified.

@d banner "This is CWEAVE (Version 3.64pc/big)\n"
@z


@x Section 9.
@d chunk_marker 0

@<Common code...@>=
typedef struct name_info {
  char *byte_start; /* beginning of the name in |byte_mem| */
  struct name_info *link;
  union {
    struct name_info *Rlink; /* right link in binary search tree for section
      names */
    char Ilk; /* used by identifiers in \.{CWEAVE} only */
  } dummy;
  char *equiv_or_xref; /* info corresponding to names */
} name_info; /* contains information about an identifier or section name */
typedef name_info *name_pointer; /* pointer into array of \&{name\_info}s */
typedef name_pointer *hash_pointer;
extern char byte_mem[]; /* characters of names */
extern char *byte_mem_end; /* end of |byte_mem| */
extern name_info name_dir[]; /* information about names */
extern name_pointer name_dir_end; /* end of |name_dir| */
extern name_pointer name_ptr; /* first unused position in |byte_start| */
extern char *byte_ptr; /* first unused position in |byte_mem| */
extern name_pointer hash[]; /* heads of hash lists */
extern hash_pointer hash_end; /* end of |hash| */
extern hash_pointer h; /* index into hash-head array */
extern name_pointer id_lookup(); /* looks up a string in the identifier table */
extern name_pointer section_lookup(); /* finds section name */
extern void print_section_name(), sprint_section_name();
@y
@d chunk_marker 0

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
extern name_pointer name_ptr; /* first unused position in |byte_start| */
extern char huge* byte_ptr; /* first unused position in |byte_mem| */
extern name_pointer hash[]; /* heads of hash lists */
extern hash_pointer hash_end; /* end of |hash| */
extern hash_pointer h; /* index into hash-head array */
extern name_pointer id_lookup(); /* looks up a string in the identifier table */
extern name_pointer section_lookup(); /* finds section name */
extern void print_section_name(), sprint_section_name();
@z


@x Section 18.
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


@x Section 19.
xref_info xmem[max_refs]; /* contains cross-reference information */
xref_pointer xmem_end = xmem+max_refs-1;
@y
xref_info huge xmem[max_refs]; /* contains cross-reference information */
xref_pointer xmem_end;
@z


@x Section 20.
@d xref equiv_or_xref
@y
@d xref ptr_union.xref_member
@z


@x Section 20.
xref_ptr=xmem; name_dir->xref=(char*)xmem; xref_switch=0; section_xref_switch=0;
xmem->num=0; /* sentinel value */
@y
xmem_end = xmem + max_refs - 1;
xref_ptr=xmem; name_dir->xref=(char*)xmem; xref_switch=0; section_xref_switch=0;
xmem->num=0; /* sentinel value */
@z


@x Section 21.
  append_xref(m); xref_ptr->xlink=q; p->xref=(char*)xref_ptr;
@y
  append_xref(m); xref_ptr->xlink=q; p->xref=(char huge*)xref_ptr;
@z


@x Section 22.
  if (r==xmem) p->xref=(char*)xref_ptr;
@y
  if (r==xmem) p->xref=(char huge*)xref_ptr;
@z


@x Section 23.
  q=(xref_pointer)p->xref;
  if (q->num==file_flag) return;
  append_xref(file_flag);
  xref_ptr->xlink = q;
  p->xref = (char *)xref_ptr;
@y
  q=(xref_pointer)p->xref;
  if (q->num==file_flag) return;
  append_xref(file_flag);
  xref_ptr->xlink = q;
  p->xref = (char huge*)xref_ptr;
@z

@x Section 25. (to please Borland's C++, version 4.02)
token tok_mem[max_toks]; /* tokens */
token_pointer tok_mem_end = tok_mem+max_toks-1; /* end of |tok_mem| */
token_pointer tok_start[max_texts]; /* directory into |tok_mem| */
token_pointer tok_ptr; /* first unused position in |tok_mem| */
text_pointer text_ptr; /* first unused position in |tok_start| */
text_pointer tok_start_end = tok_start+max_texts-1; /* end of |tok_start| */
token_pointer max_tok_ptr; /* largest value of |tok_ptr| */
@y
token tok_mem[max_toks]; /* tokens */
token_pointer tok_mem_end; /* end of |tok_mem| */
token_pointer tok_start[max_texts]; /* directory into |tok_mem| */
token_pointer tok_ptr; /* first unused position in |tok_mem| */
text_pointer text_ptr; /* first unused position in |tok_start| */
text_pointer tok_start_end; /* end of |tok_start| */
token_pointer max_tok_ptr; /* largest value of |tok_ptr| */
@z

@x Section 26. (goes with the previous change)
tok_start[1]=tok_mem+1;
max_tok_ptr=tok_mem+1; max_text_ptr=tok_start+1;
@y
tok_start[1]=tok_mem+1;
tok_mem_end=tok_mem+max_toks-1;
tok_start_end=tok_start+max_texts-1;
max_tok_ptr=tok_mem+1; max_text_ptr=tok_start+1;
@z


@x Section 27.
  p->ilk=t; p->xref=(char*)xmem;
@y
  p->ilk=t; p->xref=(char huge*)xmem;
@z


@x Section 27.
  p->xref=(char*)xmem;
@y
  p->xref=(char huge*)xmem;
@z


@x Section 70.
      if (unindexed(lhs)) { /* retain only underlined entries */
        xref_pointer q,r=NULL;
        for (q=(xref_pointer)lhs->xref;q>xmem;q=q->xlink)
          if (q->num<def_flag)
            if (r) r->xlink=q->xlink;
            else lhs->xref=(char*)q->xlink;
          else r=q;
      }
@y
      if (unindexed(lhs)) { /* retain only underlined entries */
        xref_pointer q,r=NULL;
        for (q=(xref_pointer)lhs->xref;q>xmem;q=q->xlink)
          if (q->num<def_flag)
            if (r) r->xlink=q->xlink;
            else lhs->xref=(char huge*)q->xlink;
          else r=q;
      }
@z


@x Section 87.
  char *k, *k_end=(p+1)->byte_start; /* pointers into |byte_mem| */
  out('{');
  for (k=p->byte_start; k<k_end; k++) {
@y
  char huge* k, huge* k_end=(p+1)->byte_start; /* pointers into |byte_mem| */
  out('{');
  for (k=p->byte_start; k<k_end; k++) {
@z


@x Section 116.
  append_xref(0); /* this number doesn't matter */
  xref_ptr->xlink=(xref_pointer)p->xref; r=xref_ptr;
  p->xref=(char*)xref_ptr;
  while (r->xlink!=q) {r->num=r->xlink->num; r=r->xlink;}
  r->num=m; /* everything from |q| on is left undisturbed */
@y
  append_xref(0); /* this number doesn't matter */
  xref_ptr->xlink=(xref_pointer)p->xref; r=xref_ptr;
  p->xref=(char huge*)xref_ptr;
  while (r->xlink!=q) {r->num=r->xlink->num; r=r->xlink;}
  r->num=m; /* everything from |q| on is left undisturbed */
@z


@x Section 163.
@ @<Change |pp| to $\max...@>=
@y
@ @<Change |pp| to $\max...@>=
#ifdef __MSDOS__
if (d<0 && pp+d>pp) pp=scrap_base; /* segmented architecture caused wrap */
else
#endif
@z


@x Section 194.
  char *p; /* index into |byte_mem| */
@y
  char huge *p; /* index into |byte_mem| */
@z


@x Section 229.
    if (cur_name->xref!=(char*)xmem) {
@y
    if (cur_name->xref!=(char huge*)xmem) {
@z


@x Section 232.
char *cur_byte; /* index into |byte_mem| */
@y
char huge* cur_byte; /* index into |byte_mem| */
@z


@x Section 241.
switch (cur_name->ilk) {
  case normal: if (is_tiny(cur_name)) out_str("\\|");
    else {char *j;
@y
switch (cur_name->ilk) {
  case normal: if (is_tiny(cur_name)) out_str("\\|");
    else {char huge* j;
@z


@x Section 241.
  case custom: case quoted: {char *j; out_str("$\\");
@y
  case custom: case quoted: {char huge* j; out_str("$\\");
@z
