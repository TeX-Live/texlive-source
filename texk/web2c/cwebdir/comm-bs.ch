Changes for code common to CTANGLE and CWEAVE, for MSDOS
and Borland C++ 3.1 using the following options (and perhaps others):

    -mc -w-pro -Ff=5000 -Z- -O-p

The options -Z- and -O-p explicitly turn off optimizations that seem to be
dangerous for the style of code in the CWEB sources.  (See makefile.bs.)

The main purpose of these changes is to support MSDOS with full-size arrays
by using "huge" pointers.

(This file contributed by Barry Schwartz, trashman@crud.mn.org, 28 Jun 94;
 revised 24 Jul 94.)

(Update attempt by Andreas Scherer, 31 Jan 2021.  Good luck!)


@x Section 10.
@d ilk dummy.Ilk /* used by \.{CWEAVE} only */

@<Common code...@>=
typedef struct name_info {
  char *byte_start; /* beginning of the name in |byte_mem| */
  struct name_info *link;
  union {
    struct name_info *Rlink; /* right link in binary search tree for section
      names */
    eight_bits Ilk; /* used by identifiers in \.{CWEAVE} only */
  } dummy;
  void *equiv_or_xref; /* info corresponding to names */
} name_info; /* contains information about an identifier or section name */
typedef name_info *name_pointer; /* pointer into array of \&{name\_info}s */
typedef name_pointer *hash_pointer;
extern char byte_mem[]; /* characters of names */
extern char *byte_mem_end; /* end of |byte_mem| */
@y
@d ilk dummy.Ilk /* used by \.{CWEAVE} only */

@f huge extern

@<Common code...@>=
typedef struct name_info {
  char huge* byte_start; /* beginning of the name in |byte_mem| */
  struct name_info *link;
  union {
    struct name_info *Rlink; /* right link in binary search tree for section
      names */
    eight_bits Ilk; /* used by identifiers in \.{CWEAVE} only */
  } dummy;
  union {
    void huge* equiv_member;
    void huge* xref_member;
  } ptr_union; /* info corresponding to names */
} name_info; /* contains information about an identifier or section name */
typedef name_info *name_pointer; /* pointer into array of \&{name\_info}s */
typedef name_pointer *hash_pointer;
extern char huge byte_mem[]; /* characters of names */
extern char huge* byte_mem_end; /* end of |byte_mem| */
@z


@x Section 36.
    cur_file_name[l]='/'; /* \UNIX/ pathname separator */
@y
    cur_file_name[l]='/'; /* A valid {\mc MSDOS} pathname separator */
@z


@x Section 44.
char *byte_ptr; /* first unused position in |byte_mem| */
@y
char huge* byte_ptr; /* first unused position in |byte_mem| */
@z


@x Section 45.
@ @<Init...@>=
name_dir->byte_start=byte_ptr=byte_mem; /* position zero in both arrays */
name_ptr=name_dir+1; /* |name_dir[0]| will be used only for error recovery */
name_ptr->byte_start=byte_mem; /* this makes name 0 of length zero */
@y
@ @<Init...@>=
name_dir->byte_start=byte_ptr=byte_mem; /* position zero in both arrays */
name_ptr=name_dir+1; /* |name_dir[0]| will be used only for error recovery */
name_ptr->byte_start=byte_mem; /* this makes name 0 of length zero */
byte_mem_end = byte_mem+max_bytes-1;
@z


@x Section 52.
void
print_section_name(
name_pointer p)
{
  char *ss, *s = first_chunk(p);
@y
void
print_section_name(
name_pointer p)
{
  char huge* ss, huge* s = first_chunk(p);
@z


@x Section 53.
void
sprint_section_name(
  char *dest,
  name_pointer p)
{
  char *ss, *s = first_chunk(p);
@y
void
sprint_section_name(
  char huge* dest,
  name_pointer p)
{
  char huge* ss, huge* s = first_chunk(p);
@z


@x Section 54.
void
print_prefix_name(
name_pointer p)
{
  char *s = first_chunk(p);
@y
void
print_prefix_name(
name_pointer p)
{
  char huge* s = first_chunk(p);
@z


@x Section 57.
static name_pointer
add_section_name( /* install a new node in the tree */
name_pointer par, /* parent of new node */
int c, /* right or left? */
char *first, /* first character of section name */
char *last, /* last character of section name, plus one */
boolean ispref) /* are we adding a prefix or a full name? */
{
  name_pointer p=name_ptr; /* new node */
  char *s=first_chunk(p);
@y
static name_pointer
add_section_name( /* install a new node in the tree */
name_pointer par, /* parent of new node */
int c, /* right or left? */
char huge* first, /* first character of section name */
char huge* last, /* last character of section name, plus one */
boolean ispref) /* are we adding a prefix or a full name? */
{
  name_pointer p=name_ptr; /* new node */
  char huge* s=first_chunk(p);
@z


@x Section 58.
static void
extend_section_name(
name_pointer p, /* name to be extended */
char *first, /* beginning of extension text */
char *last, /* one beyond end of extension text */
boolean ispref) /* are we adding a prefix or a full name? */
{
  char *s;
@y
static void
extend_section_name(
name_pointer p, /* name to be extended */
char huge* first, /* beginning of extension text */
char huge* last, /* one beyond end of extension text */
boolean ispref) /* are we adding a prefix or a full name? */
{
  char huge* s;
@z


@x Section 63.
static int section_name_cmp(
char **pfirst, /* pointer to beginning of comparison string */
size_t len, /* length of string */
name_pointer r) /* section name being compared */
{
  char *first=*pfirst; /* beginning of comparison string */
  name_pointer q=r+1; /* access to subsequent chunks */
  char *ss, *s=first_chunk(r);
@y
static int section_name_cmp(
char huge** pfirst, /* pointer to beginning of comparison string */
size_t len, /* length of string */
name_pointer r) /* section name being compared */
{
  char huge* first=*pfirst; /* beginning of comparison string */
  name_pointer q=r+1; /* access to subsequent chunks */
  char huge* ss, huge* s=first_chunk(r);
@z


@x Section 75.
An omitted change file argument means that |"/dev/null"| should be used,
@y
An omitted change file argument means that |"NUL"| should be used,
@z


@x Section 75.
  strcpy(change_file_name,"/dev/null");
@y
  strcpy(change_file_name,"NUL");
@z


@x Section 75.
        else if (*s=='/') dot_pos=NULL,name_pos=++s;
@y
        else if (*s == ':' || *s == '\\' || *s == '/')
	  dot_pos=NULL,name_pos=++s;
@z
