@q Changes for CWEB in TeX Live from numerous contributors. @>
@q This file is in the Public Domain. @>

@q Most of the original Kpathsea changes by Wlodek Bzyl and Olaf Weber @>
@q were merged with the set of change files of the CWEBbin project; @>
@q see https://github.com/ascherer/cwebbin for the original parts. @>

@q This stripped change file {comm,ctang,cweav,ctwill}-w2c.ch has been @>
@q created from the set of change files {comm,ctang,cweav}-patch.ch, @>
@q {comm,ctang,cweav}-ansi.ch, {comm,ctang,cweav}-extensions.ch, @>
@q {comm,ctang,cweav}-output.ch, {comm,ctang,cweav}-i18n.ch, and @>
@q cweav-twill.ch for CTWILL, and {comm,ctang,cweav,ctwill}-texlive.ch @>
@q with the 'tie' processor and is used as a monolithic changefile for @>
@q {common,ctangle,cweave}.w in TeX Live. @>

@q Please send comments, suggestions, etc. to tex-k@@tug.org. @>

@x
\def\title{Common code for CTANGLE and CWEAVE (Version 3.64)}
@y
\def\Kpathsea/{{\mc KPATHSEA\spacefactor1000}}
\def\title{Common code for CTANGLE and CWEAVE (\TeX~Live)}
@z

@x
  \centerline{(Version 3.64)}
@y
  \centerline{(Version 3.64 [\TeX~Live])}
@z

@x
@s not_eq normal @q unreserve a C++ keyword @>
@y
@s boolean int
@s uint8_t int
@s uint16_t int
@z

@x
|program|.

@d ctangle 0
@d cweave 1
@y
|program|. And \.{CTWILL} adds some extra twists.

@d ctangle 0
@d cweave 1
@d ctwill 2
@z

@x
typedef short boolean;
boolean program; /* \.{CWEAVE} or \.{CTANGLE}? */
@y
int program; /* \.{CWEAVE} or \.{CTANGLE} or \.{CTWILL}? */
@z

@x
void
common_init()
@y
void
common_init(void)
@z

@x
  @<Initialize pointers@>;
  @<Set the default options common to \.{CTANGLE} and \.{CWEAVE}@>;
  @<Scan arguments and open output files@>;
@y
  @<Initialize pointers@>@;
  @<Set up |PROGNAME| feature and initialize the search path mechanism@>@;
  @<Set locale and bind language catalogs@>@;
  @<Set the default options common to \.{CTANGLE} and \.{CWEAVE}@>@;
  @<Scan arguments and open output files@>@;
@z

@x
@d not_eq 032 /* `\.{!=}'\,;  corresponds to MIT's {\tentex\char'32} */
@y
@d non_eq 032 /* `\.{!=}'\,;  corresponds to MIT's {\tentex\char'32} */
@z

@x
@d buf_size 100 /* for \.{CWEAVE} and \.{CTANGLE} */
@y
@d buf_size 1000 /* for \.{CWEAVE} and \.{CTANGLE} */
@z

@x
@d xisspace(c) (isspace(c)&&((unsigned char)c<0200))
@d xisupper(c) (isupper(c)&&((unsigned char)c<0200))
@y
@d xisspace(c) (isspace((eight_bits)c)&&((eight_bits)c<0200))
@d xisupper(c) (isupper((eight_bits)c)&&((eight_bits)c<0200))
@z

@x
int input_ln(fp) /* copies a line into |buffer| or returns 0 */
FILE *fp; /* what file to read from */
@y
static boolean input_ln(@t\1\1@> /* copies a line into |buffer| or returns 0 */
FILE *fp@t\2\2@>) /* what file to read from */
@z

@x
    if ((*(k++) = c) != ' ') limit = k;
@y
    if ((*(k++) = c) != ' ' && c != '\r') limit = k;
@z

@x
      ungetc(c,fp); loc=buffer; err_print("! Input line too long");
@y
      ungetc(c,fp); loc=buffer; err_print(_("! Input line too long"));
@z

@x
@d max_file_name_length 60
@y
@d max_file_name_length 1024
@z

@x
char alt_web_file_name[max_file_name_length]; /* alternate name to try */
@y
@z

@x
@d lines_dont_match (change_limit-change_buffer != limit-buffer ||
  strncmp(buffer, change_buffer, limit-buffer))
@y
@d lines_dont_match (change_limit-change_buffer != limit-buffer || @|
  strncmp(buffer, change_buffer, (size_t)(limit-buffer)))
@z

@x
void
prime_the_change_buffer()
@y
static void
prime_the_change_buffer(void)
@z

@x
  if (xisupper(buffer[1])) buffer[1]=tolower(buffer[1]);
@y
  if (xisupper(buffer[1])) buffer[1]=tolower((eight_bits)buffer[1]);
@z

@x
    err_print("! Missing @@x in change file");
@y
    err_print(_("! Missing @@x in change file"));
@z

@x
    err_print("! Change file ended after @@x");
@y
    err_print(_("! Change file ended after @@x"));
@z

@x
  change_limit=change_buffer+(limit-buffer);
  strncpy(change_buffer,buffer,limit-buffer+1);
@y
  change_limit=change_buffer+(ptrdiff_t)(limit-buffer);
  strncpy(change_buffer,buffer,(size_t)(limit-buffer+1));
@z

@x
void
check_change() /* switches to |change_file| if the buffers match */
@y
static void
check_change(void) /* switches to |change_file| if the buffers match */
@z

@x
      err_print("! Change file ended before @@y");
@y
      err_print(_("! Change file ended before @@y"));
@z

@x
      char xyz_code=xisupper(buffer[1])? tolower(buffer[1]): buffer[1];
@y
      char xyz_code=xisupper(buffer[1])? tolower((eight_bits)buffer[1]): buffer[1];
@z

@x
        err_print("! CWEB file ended during a change");
@y
        err_print(_("! CWEB file ended during a change"));
@z

@x
  loc=buffer+2; err_print("! Where is the matching @@y?");
@y
  loc=buffer+2; err_print(_("! Where is the matching @@y?"));
@z

@x
    err_print("of the preceding lines failed to match");
@y
    err_print(_("of the preceding lines failed to match"));
@z

@x
void
reset_input()
@y
void
reset_input(void)
@z

@x
if ((web_file=fopen(web_file_name,"r"))==NULL) {
  strcpy(web_file_name,alt_web_file_name);
  if ((web_file=fopen(web_file_name,"r"))==NULL)
       fatal("! Cannot open input file ", web_file_name);
}
@y
if ((found_filename=kpse_find_cweb(web_file_name))==NULL || @|
    (web_file=fopen(found_filename,"r"))==NULL) {
  fatal(_("! Cannot open input file "), web_file_name);
} else if (strlen(found_filename) < max_file_name_length) {
  /* Copy name for |#line| directives. */
  if (strcmp(web_file_name, found_filename))
    strcpy(web_file_name, found_filename +
      ((strncmp(found_filename,"./",2)==0) ? 2 : 0));
  free(found_filename);
} else fatal(_("! Filename too long\n"), found_filename);
@z

@x
if ((change_file=fopen(change_file_name,"r"))==NULL)
       fatal("! Cannot open change file ", change_file_name);
@y
if ((found_filename=kpse_find_cweb(change_file_name))==NULL || @|
    (change_file=fopen(found_filename,"r"))==NULL) {
  fatal(_("! Cannot open change file "), change_file_name);
} else if (strlen(found_filename) < max_file_name_length) {
  /* Copy name for |#line| directives. */
  if (strcmp(change_file_name, found_filename))
    strcpy(change_file_name, found_filename +
      ((strncmp(found_filename,"./",2)==0) ? 2 : 0));
  free(found_filename);
} else fatal(_("! Filename too long\n"), found_filename);
@z

@x
@d max_sections 2000 /* number of identifiers, strings, section names;
  must be less than 10240 */
@y
@d max_sections 10239 /* number of identifiers, strings, section names;
  must be less than 10240 */
@z

@x
typedef unsigned short sixteen_bits;
@y
typedef uint8_t eight_bits;
typedef uint16_t sixteen_bits;
@z

@x
int get_line() /* inputs the next line */
@y
boolean get_line(void) /* inputs the next line */
@z

@x
      err_print("! Include file name not given");
@y
      err_print(_("! Include file name not given"));
@z

@x
      err_print("! Too many nested includes");
@y
      err_print(_("! Too many nested includes"));
@z

@x
@ When an \.{@@i} line is found in the |cur_file|, we must temporarily
stop reading it and start reading from the named include file.  The
\.{@@i} line should give a complete file name with or without
double quotes.
If the environment variable \.{CWEBINPUTS} is set, or if the compiler flag
of the same name was defined at compile time,
\.{CWEB} will look for include files in the directory thus named, if
it cannot find them in the current directory.
(Colon-separated paths are not supported.)
The remainder of the \.{@@i} line after the file name is ignored.
@y
@ When an \.{@@i} line is found in the |cur_file|, we must temporarily
stop reading it and start reading from the named include file.  The
\.{@@i} line should give a complete file name with or without
double quotes.
The actual file lookup is done with the help of the \Kpathsea/ library;
see section~\X90:File lookup with \Kpathsea/\X~for details. % FIXME
The remainder of the \.{@@i} line after the file name is ignored.
@z

@x
        err_print("! Include file name too long"); goto restart;}
@y
        err_print(_("! Include file name too long")); goto restart;}
@z

@x
  char temp_file_name[max_file_name_length];
  char *cur_file_name_end=cur_file_name+max_file_name_length-1;
  char *k=cur_file_name, *kk;
  int l; /* length of file name */
@y
  char *cur_file_name_end=cur_file_name+max_file_name_length-1;
  char *k=cur_file_name;
@z

@x
  if ((cur_file=fopen(cur_file_name,"r"))!=NULL) {
@y
  if ((found_filename=kpse_find_cweb(cur_file_name))!=NULL && @|
      (cur_file=fopen(found_filename,"r"))!=NULL) {
    /* Copy name for |#line| directives. */
    if (strlen(found_filename) < max_file_name_length) {
      if (strcmp(cur_file_name, found_filename))
        strcpy(cur_file_name, found_filename +
          ((strncmp(found_filename,"./",2)==0) ? 2 : 0));
      free(found_filename);
    } else fatal(_("! Filename too long\n"), found_filename);
@z

@x
  kk=getenv("CWEBINPUTS");
  if (kk!=NULL) {
    if ((l=strlen(kk))>max_file_name_length-2) too_long();
    strcpy(temp_file_name,kk);
  }
  else {
#ifdef CWEBINPUTS
    if ((l=strlen(CWEBINPUTS))>max_file_name_length-2) too_long();
    strcpy(temp_file_name,CWEBINPUTS);
#else
    l=0;
#endif /* |CWEBINPUTS| */
  }
  if (l>0) {
    if (k+l+2>=cur_file_name_end)  too_long();
@.Include file name ...@>
    for (; k>= cur_file_name; k--) *(k+l+1)=*k;
    strcpy(cur_file_name,temp_file_name);
    cur_file_name[l]='/'; /* \UNIX/ pathname separator */
    if ((cur_file=fopen(cur_file_name,"r"))!=NULL) {
      cur_line=0; print_where=1;
      goto restart; /* success */
    }
  }
  include_depth--; err_print("! Cannot open include file"); goto restart;
@y
  include_depth--; err_print(_("! Cannot open include file")); goto restart;
@z

@x
    err_print("! Change file ended without @@z");
@y
    err_print(_("! Change file ended without @@z"));
@z

@x
      if (xisupper(buffer[1])) buffer[1]=tolower(buffer[1]);
@y
      if (xisupper(buffer[1])) buffer[1]=tolower((eight_bits)buffer[1]);
@z

@x
        err_print("! Where is the matching @@z?");
@y
        err_print(_("! Where is the matching @@z?"));
@z

@x
void
check_complete(){
  if (change_limit!=change_buffer) { /* |changing| is 0 */
    strncpy(buffer,change_buffer,change_limit-change_buffer+1);
    limit=buffer+(int)(change_limit-change_buffer);
@y
void
check_complete(void) {
  if (change_limit!=change_buffer) { /* |changing| is 0 */
    strncpy(buffer,change_buffer,(size_t)(change_limit-change_buffer+1));
    limit=buffer+(ptrdiff_t)(change_limit-change_buffer);
@z

@x
    err_print("! Change file entry did not match");
@y
    err_print(_("! Change file entry did not match"));
@z

@x
@d max_bytes 90000 /* the number of bytes in identifiers,
  index entries, and section names; must be less than $2^{24}$ */
@d max_names 4000 /* number of identifiers, strings, section names;
  must be less than 10240 */
@y
@d max_bytes 1000000 /* the number of bytes in identifiers,
  index entries, and section names; must be less than $2^{24}$ */
@d max_names 10239 /* number of identifiers, strings, section names;
  must be less than 10240 */
@z

@x
@d length(c) (c+1)->byte_start-(c)->byte_start /* the length of a name */
@y
@d length(c) (size_t)((c+1)->byte_start-(c)->byte_start) /* the length of a name */
@z

@x
@d hash_size 353 /* should be prime */
@y
@d hash_size 8501 /* should be prime */
@z

@x
extern int names_match();
@y
extern boolean names_match(name_pointer,const char *,size_t,eight_bits);@/
@z

@x
name_pointer
id_lookup(first,last,t) /* looks up a string in the identifier table */
char *first; /* first character of string */
char *last; /* last character of string plus one */
char t; /* the |ilk|; used by \.{CWEAVE} only */
{
  char *i=first; /* position in |buffer| */
@y
name_pointer
id_lookup(@t\1\1@> /* looks up a string in the identifier table */
const char *first, /* first character of string */
const char *last, /* last character of string plus one */
char t@t\2\2@>) /* the |ilk|; used by \.{CWEAVE} only */
{
  const char *i=first; /* position in |buffer| */
@z

@x
  l=last-first; /* compute the length */
@y
  l=(int)(last-first); /* compute the length */
@z

@x
void init_p();
@y
extern void init_p(name_pointer,eight_bits);@/
@z

@x
  if (byte_ptr+l>byte_mem_end) overflow("byte memory");
  if (name_ptr>=name_dir_end) overflow("name");
@y
  if (byte_ptr+l>byte_mem_end) overflow(_("byte memory"));
  if (name_ptr>=name_dir_end) overflow(_("name"));
@z

@x
  if (program==cweave) init_p(p,t);
@y
  init_p(p,t);
@z

@x
void
print_section_name(p)
name_pointer p;
@y
void
print_section_name(
name_pointer p)
@z

@x
      term_write(s,ss-s); p=q->link; q=p;
    } else {
      term_write(s,ss+1-s); p=name_dir; q=NULL;
@y
      term_write(s,(size_t)(ss-s)); p=q->link; q=p;
    } else {
      term_write(s,(size_t)(ss+1-s)); p=name_dir; q=NULL;
@z

@x
void
sprint_section_name(dest,p)
  char*dest;
  name_pointer p;
@y
void
sprint_section_name(
  char *dest,
  name_pointer p)
@z

@x
    strncpy(dest,s,ss-s), dest+=ss-s;
@y
    strncpy(dest,s,(size_t)(ss-s)), dest+=ss-s;
@z

@x
void
print_prefix_name(p)
name_pointer p;
@y
void
print_prefix_name(
name_pointer p)
@z

@x
int web_strcmp(j,j_len,k,k_len) /* fuller comparison than |strcmp| */
  char *j, *k; /* beginning of first and second strings */
  int j_len, k_len; /* length of strings */
@y
static int web_strcmp(@t\1\1@> /* fuller comparison than |strcmp| */
  char *j, /* beginning of first string */
  int j_len, /* length of first string */
  char *k, /* beginning of second string */
  int k_len@t\2\2@>) /* length of second string */
@z

@x
extern void init_node();
@y
extern void init_node(name_pointer);@/
@z

@x
name_pointer
add_section_name(par,c,first,last,ispref) /* install a new node in the tree */
name_pointer par; /* parent of new node */
int c; /* right or left? */
char *first; /* first character of section name */
char *last; /* last character of section name, plus one */
int ispref; /* are we adding a prefix or a full name? */
@y
static name_pointer
add_section_name(@t\1\1@> /* install a new node in the tree */
name_pointer par, /* parent of new node */
int c, /* right or left? */
char *first, /* first character of section name */
char *last, /* last character of section name, plus one */
int ispref@t\2\2@>) /* are we adding a prefix or a full name? */
@z

@x
  int name_len=last-first+ispref; /* length of section name */
@y
  int name_len=(int)(last-first)+ispref; /* length of section name */
@z

@x
  if (s+name_len>byte_mem_end) overflow("byte memory");
  if (name_ptr+1>=name_dir_end) overflow("name");
@y
  if (s+name_len>byte_mem_end) overflow(_("byte memory"));
  if (name_ptr+1>=name_dir_end) overflow(_("name"));
@z

@x
void
extend_section_name(p,first,last,ispref)
name_pointer p; /* name to be extended */
char *first; /* beginning of extension text */
char *last; /* one beyond end of extension text */
int ispref; /* are we adding a prefix or a full name? */
@y
static void
extend_section_name(@t\1\1@>
name_pointer p, /* name to be extended */
char *first, /* beginning of extension text */
char *last, /* one beyond end of extension text */
int ispref@t\2\2@>) /* are we adding a prefix or a full name? */
@z

@x
  int name_len=last-first+ispref;
@y
  int name_len=(int)(last-first)+ispref;
@z

@x
  if (name_ptr>=name_dir_end) overflow("name");
@y
  if (name_ptr>=name_dir_end) overflow(_("name"));
@z

@x
  if (s+name_len>byte_mem_end) overflow("byte memory");
@y
  if (s+name_len>byte_mem_end) overflow(_("byte memory"));
@z

@x
name_pointer
section_lookup(first,last,ispref) /* find or install section name in tree */
char *first, *last; /* first and last characters of new name */
int ispref; /* is the new name a prefix or a full name? */
@y
name_pointer
section_lookup(@t\1\1@> /* find or install section name in tree */
char *first,char *last, /* first and last characters of new name */
int ispref@t\2\2@>) /* is the new name a prefix or a full name? */
@z

@x
  int name_len=last-first+1;
@y
  int name_len=(int)(last-first)+1;
@z

@x
      printf("\n! Ambiguous prefix: matches <");
@y
      fputs(_("\n! Ambiguous prefix: matches <"),stdout);
@z

@x
      printf(">\n and <");
@y
      fputs(_(">\n and <"),stdout);
@z

@x
      printf("\n! New name is a prefix of <");
@y
      fputs(_("\n! New name is a prefix of <"),stdout);
@z

@x
      printf("\n! New name extends <");
@y
      fputs(_("\n! New name extends <"),stdout);
@z

@x
    printf("\n! Section name incompatible with <");
@y
    fputs(_("\n! Section name incompatible with <"),stdout);
@z

@x
    printf(">,\n which abbreviates <");
@y
    fputs(_(">,\n which abbreviates <"),stdout);
@z

@x
int section_name_cmp();
@y
static int section_name_cmp(char **,int,name_pointer);@/
@z

@x
int section_name_cmp(pfirst,len,r)
char **pfirst; /* pointer to beginning of comparison string */
int len; /* length of string */
name_pointer r; /* section name being compared */
@y
static int section_name_cmp(@t\1\1@>
char **pfirst, /* pointer to beginning of comparison string */
int len, /* length of string */
name_pointer r@t\2\2@>) /* section name being compared */
@z

@x
          *pfirst=first+(ss-s);
@y
          *pfirst=first+(ptrdiff_t)(ss-s);
@z

@x
      if (q!=name_dir) {len -= ss-s; s=q->byte_start; r=q; continue;}
@y
      if (q!=name_dir) {len -= (int)(ss-s); s=q->byte_start; r=q; continue;}
@z

@x
|equiv_or_xref| as a pointer to a |char|.

@<More elements of |name...@>=
char *equiv_or_xref; /* info corresponding to names */
@y
|equiv_or_xref| as a pointer to |void|.

@<More elements of |name...@>=
void *equiv_or_xref; /* info corresponding to names */
@z

@x
void  err_print();
@y
extern void err_print(const char *);@/
@z

@x
void
err_print(s) /* prints `\..' and location of error message */
char *s;
@y
void
err_print(@t\1\1@> /* prints `\..' and location of error message */
const char *s@t\2\2@>)
@z

@x
  printf(". (l. %d of change file)\n", change_line);
else if (include_depth==0) printf(". (l. %d)\n", cur_line);
  else printf(". (l. %d of include file %s)\n", cur_line, cur_file_name);
@y
  printf(_(". (l. %d of change file)\n"), change_line);
else if (include_depth==0) printf(_(". (l. %d)\n"), cur_line);
  else printf(_(". (l. %d of include file %s)\n"), cur_line, cur_file_name);
@z

@x
int wrap_up();
extern void print_stats();
@y
extern int wrap_up(void);@/
extern void print_stats(void);@/
@z

@x
@ Some implementations may wish to pass the |history| value to the
operating system so that it can be used to govern whether or not other
programs are started. Here, for instance, we pass the operating system
a status of 0 if and only if only harmless messages were printed.
@^system dependencies@>
@y
@ On multi-tasking systems like the {\mc AMIGA} it is very convenient to
know a little bit more about the reasons why a program failed.  The four
levels of return indicated by the |history| value are very suitable for
this purpose.  Here, for instance, we pass the operating system a status
of~0 if and only if the run was a complete success.  Any warning or error
message will result in a higher return value, so that {\mc AREXX} scripts
can be made sensitive to these conditions.
@^system dependencies@>

@d RETURN_OK     0 /* No problems, success */
@d RETURN_WARN   5 /* A warning only */
@d RETURN_ERROR 10 /* Something wrong */
@d RETURN_FAIL  20 /* Complete or severe failure */
@z

@x
int wrap_up() {
  putchar('\n');
@y
int wrap_up(void) {
  if (show_progress) new_line;
@z

@x
  @<Print the job |history|@>;
@y
  @<Print the job |history|@>@;
  @<Remove the temporary file if not already done@>@;
@z

@x
  if (history > harmless_message) return(1);
  else return(0);
@y
  switch(history) {
  case harmless_message: return RETURN_WARN;
  case error_message: return RETURN_ERROR;
  case fatal_message: return RETURN_FAIL;
  default: return RETURN_OK;
  }
@z

@x
case spotless: if (show_happiness) printf("(No errors were found.)\n"); break;
case harmless_message:
  printf("(Did you see the warning message above?)\n"); break;
case error_message:
  printf("(Pardon me, but I think I spotted something wrong.)\n"); break;
case fatal_message: printf("(That was a fatal error, my friend.)\n");
@y
case spotless:
  if (show_happiness) puts(_("(No errors were found.)")); break;
case harmless_message:
  puts(_("(Did you see the warning message above?)")); break;
case error_message:
  puts(_("(Pardon me, but I think I spotted something wrong.)")); break;
case fatal_message:
  puts(_("(That was a fatal error, my friend.)"));
@z

@x
void fatal(), overflow();
@y
extern void fatal(const char *,const char *);@/
extern void overflow(const char *);@/
@z

@x
@c void
fatal(s,t)
  char *s,*t;
@y
@c void
fatal(
  const char *s,const char *t)
@z

@x
  if (*s) printf(s);
@y
  if (*s) fputs(s,stdout);
@z

@x
@c void
overflow(t)
  char *t;
@y
@c void
overflow(
  const char *t)
@z

@x
  printf("\n! Sorry, %s capacity exceeded",t); fatal("","");
@y
  printf(_("\n! Sorry, %s capacity exceeded"),t); fatal("","");
@z

@x
@d confusion(s) fatal("! This can't happen: ",s)
@y
@d confusion(s) fatal(_("! This can't happen: "),s)
@z

@x
@d show_happiness flags['h'] /* should lack of errors be announced? */
@y
@d show_happiness flags['h'] /* should lack of errors be announced? */
@d make_xrefs flags['x'] /* should cross references be output? */
@z

@x
char scn_file_name[max_file_name_length]; /* name of |scn_file| */
@y
char scn_file_name[max_file_name_length]; /* name of |scn_file| */
char check_file_name[max_file_name_length]; /* name of |check_file| */
@z

@x
boolean flags[128]; /* an option for each 7-bit code */
@y
boolean flags[128]; /* an option for each 7-bit code */
const char *use_language=""; /* prefix of \.{cwebmac.tex} in \TEX/ output */
@z

@x
show_banner=show_happiness=show_progress=1;
@y
@z

@x
An omitted change file argument means that |"/dev/null"| should be used,
when no changes are desired.
@y
An omitted change file argument means that |"/dev/null"| or---on non-\UNIX/
systems the contents of the compile-time variable |DEV_NULL| (\TeX~Live) or
|_DEV_NULL| (Amiga)---should be used, when no changes are desired.
@z

@x
void scan_args();
@y
static void scan_args(void);@/
@z

@x
void
scan_args()
@y
static void
scan_args(void)
@z

@x
  boolean flag_change;
@y
@z

@x
  while (--argc > 0) {
@y
@#
#if defined DEV_NULL
  strncpy(change_file_name,DEV_NULL,max_file_name_length-2);
  change_file_name[max_file_name_length-2]='\0';
#elif defined _DEV_NULL
  strncpy(change_file_name,_DEV_NULL,max_file_name_length-2);
  change_file_name[max_file_name_length-2]='\0';
#else
  strcpy(change_file_name,"/dev/null");
#endif
@^system dependencies@>
  while (--argc > 0) {
@z

@x
      while (*s) {
        if (*s=='.') dot_pos=s++;
        else if (*s=='/') dot_pos=NULL,name_pos=++s;
        else s++;
      }
@y
      while (*s) {
        if (*s=='.') dot_pos=s++;
        else if (*s==DIR_SEPARATOR || *s==DEVICE_SEPARATOR || *s=='/')
          dot_pos=NULL,name_pos=++s;
        else s++;
      }
@^system dependencies@>
@z

@x
  if (found_change<=0) strcpy(change_file_name,"/dev/null");
@y
@z

@x
@ We use all of |*argv| for the |web_file_name| if there is a |'.'| in it,
otherwise we add |".w"|. If this file can't be opened, we prepare an
|alt_web_file_name| by adding |"web"| after the dot.
The other file names come from adding other things
after the dot.  We must check that there is enough room in
|web_file_name| and the other arrays for the argument.
@y
@ We use all of |*argv| for the |web_file_name| if there is a |'.'| in it,
otherwise we add |".w"|.  The other file names come from adding other things
after the dot.  We must check that there is enough room in |web_file_name| and
the other arrays for the argument.
@z

@x
  sprintf(alt_web_file_name,"%s.web",*argv);
@y
@z

@x
  if (strcmp(*argv,"-")==0) found_change=-1;
  else {
@y
  if (strcmp(*argv,"-")!=0) {
@z

@x
    found_change=1;
  }
@y
  }
  found_change=1;
@z

@x
    if (flags['x']) { /* indexes will be generated */
@y
    if (make_xrefs) { /* indexes will be generated */
@z

@x
@ @<Handle flag...@>=
{
  if (**argv=='-') flag_change=0;
  else flag_change=1;
  for(dot_pos=*argv+1;*dot_pos>'\0';dot_pos++)
@y
@ @d flag_change (**argv!='-')
@<Handle flag...@>=
{
  if (strcmp("-help",*argv)==0 || strcmp("--help",*argv)==0)
@.--help@>
    @<Display help message and |exit|@>@;
  if (strcmp("-version",*argv)==0 || strcmp("--version",*argv)==0)
@.--version@>
    @<Display version information and |exit|@>@;
  if (strcmp("-verbose",*argv)==0 || strcmp("--verbose",*argv)==0)
@.--verbose@>
  { show_banner=show_progress=show_happiness=1; continue; }
  if (strcmp("-quiet",*argv)==0 || strcmp("--quiet",*argv)==0)
@.--quiet@>
  { show_banner=show_progress=show_happiness=0; continue; }
  for(dot_pos=*argv+1;*dot_pos>'\0';dot_pos++)
    if (*dot_pos=='v') {
      show_banner=show_progress=show_happiness=1;
    } else
    if (*dot_pos=='q') {
      show_banner=show_progress=show_happiness=0;
    } else
    if (*dot_pos=='d') {
      if (sscanf(++dot_pos,"%u",&kpathsea_debug)!=1) @<Print usage error...@>@;
      while (isdigit(*dot_pos)) dot_pos++; /* skip numeric part */
      dot_pos--; /* reset to final digit */
    } else
    if(*dot_pos=='l') {
       use_language=++dot_pos;
       break;
    } else
@z

@x
    flags[*dot_pos]=flag_change;
@y
    flags[(eight_bits)*dot_pos]=flag_change;
@z

@x
{
if (program==ctangle)
  fatal(
"! Usage: ctangle [options] webfile[.w] [{changefile[.ch]|-} [outfile[.c]]]\n"
   ,"");
@.Usage:@>
else fatal(
"! Usage: cweave [options] webfile[.w] [{changefile[.ch]|-} [outfile[.tex]]]\n"
   ,"");
}
@y
cb_usage(program==ctangle ? "ctangle" : program==cweave ? "cweave" : "ctwill");
@.Usage:@>
@z

@x
@ @<Complain about arg...@>= fatal("! Filename too long\n", *argv);
@y
@ @<Complain about arg...@>= fatal(_("! Filename too long\n"), *argv);
@z

@x
FILE *scn_file; /* where list of sections from \.{CWEAVE} goes */
@y
FILE *scn_file; /* where list of sections from \.{CWEAVE} goes */
FILE *check_file; /* temporary output file */
@z

@x
FILE *active_file; /* currently active file for \.{CWEAVE} output */
@y
FILE *active_file; /* currently active file for \.{CWEAVE} output */
char *found_filename; /* filename found by |kpse_find_file| */
@z

@x
@ @<Scan arguments and open output files@>=
scan_args();
if (program==ctangle) {
  if ((C_file=fopen(C_file_name,"w"))==NULL)
    fatal("! Cannot open output file ", C_file_name);
@.Cannot open output file@>
}
else {
  if ((tex_file=fopen(tex_file_name,"w"))==NULL)
    fatal("! Cannot open output file ", tex_file_name);
}
@y
@ @<Scan arguments and open output files@>=
scan_args();
if (program==ctangle) {
  strcpy(check_file_name,C_file_name);
  if(check_file_name[0]!='\0') {
    char *dot_pos=strrchr(check_file_name,'.');
    if(dot_pos==NULL) strcat(check_file_name,".ttp");
    else strcpy(dot_pos,".ttp");
  }
  if ((C_file=fopen(check_file_name,"wb"))==NULL)
    fatal(_("! Cannot open output file "), check_file_name);
@.Cannot open output file@>
}
else {
  strcpy(check_file_name,tex_file_name);
  if(check_file_name[0]!='\0') {
    char *dot_pos=strrchr(check_file_name,'.');
    if(dot_pos==NULL) strcat(check_file_name,".wtp");
    else strcpy(dot_pos,".wtp");
  }
  if ((tex_file=fopen(check_file_name,"wb"))==NULL)
    fatal(_("! Cannot open output file "), check_file_name);
}
@z

@x
@ We predeclare several standard system functions here instead of including
their system header files, because the names of the header files are not as
standard as the names of the functions. (For example, some \CEE/ environments
have \.{<string.h>} where others have \.{<strings.h>}.)

@<Predecl...@>=
extern int strlen(); /* length of string */
extern int strcmp(); /* compare strings lexicographically */
extern char* strcpy(); /* copy one string to another */
extern int strncmp(); /* compare up to $n$ string characters */
extern char* strncpy(); /* copy up to $n$ string characters */
@y
@ For string handling we include the {\mc ANSI C} system header file instead
of predeclaring the standard system functions |strlen|, |strcmp|, |strcpy|,
|strncmp|, and |strncpy|.
@^system dependencies@>

@<Include...@>=
#include <string.h>
@z

@x
@** Index.
@y
@** Extensions for modern \.{CWEB}.

The following sections introduce code changes and extensions that have been
created by numerous contributors over the course of a quarter century. They
make \.{CWEB} adhere to modern coding standards and introduce new or improved
features.

Care has been taken to keep the original section numbering intact, so this new
section should have the same number as the original ``\&{82.~Index},'' and
additional material follows below.

@* Function declarations. Here are declarations---conforming to
{\mc ANSI~C}---of all functions in this code that appear in |"common.h"|
and thus should agree with \.{CTANGLE} and \.{CWEAVE}.

@<Predecl...@>=
boolean get_line(void);@/
name_pointer id_lookup(const char *,const char *,char);@/
name_pointer section_lookup(char *,char *,int);@/
void check_complete(void);@/
void common_init(void);@/
void print_prefix_name(name_pointer);@/
void print_section_name(name_pointer);@/
void reset_input(void);@/
void sprint_section_name(char *,name_pointer);@/

@ The following functions are private to |"common.w"|.

@<Predecl...@>=
static boolean input_ln(FILE *);@/
static int web_strcmp(char *,int,char *,int);@/
static name_pointer add_section_name(name_pointer,int,char *,char *,int);@/
static void extend_section_name(name_pointer,char *,char *,int);@/
static void check_change(void);@/
static void prime_the_change_buffer(void);@/

@* Standard C library interfaces.  This updated version of \.{CWEB} uses
standard C types for boolean values, pointers, and objects with fixed sizes.

@<Include files@>=
#include <stdbool.h> /* type definition of |bool| */
#include <stddef.h> /* type definition of |ptrdiff_t| */
#include <stdint.h> /* type definition of |uint8_t| et al. */

@ The |scan_args| and |cb_show_banner| routines and the |bindtextdomain|
argument string need a few extra variables.

@s string int

@d max_banner 50

@d PATH_SEPARATOR   separators[0]
@d DIR_SEPARATOR    separators[1]
@d DEVICE_SEPARATOR separators[2]

@<Other...@>=
char cb_banner[max_banner];@/
string texmf_locale;@/
#ifndef SEPARATORS
#define SEPARATORS "://"
#endif
char separators[]=SEPARATORS;

@* Temporary file output.  Before we leave the program we have to make
sure that the output files are correctly written.

@<Remove the temporary file...@>=
if(C_file) fclose(C_file);
if(tex_file) fclose(tex_file);
if(check_file) fclose(check_file);
if(strlen(check_file_name)) /* Delete the temporary file in case of a break */
   remove(check_file_name);

@* Internationalization.  You may have noticed that almost all \.{"strings"}
in the \.{CWEB} sources are placed in the context of the `|_|'~macro.
This is just a shortcut for the `|gettext|' function from the ``GNU~gettext
utilities.'' For systems that do not have this library installed, we wrap
things for neutral behavior without internationalization.

@d _(STRING) gettext(STRING)

@<Include files@>=
#ifndef HAVE_GETTEXT
#define HAVE_GETTEXT 0
#endif
@#
#if HAVE_GETTEXT
#include <locale.h>
#include <libintl.h>
#else
#define setlocale(A,B) ""
#define bindtextdomain(A,B) ""
#define textdomain(A) ""
#define gettext(A) A
#endif

@ If translation catalogs for your personal \.{LANGUAGE} are installed at the
appropriate place, \.{CTANGLE} and \.{CWEAVE} will talk to you in your favorite
language.  Catalog \.{cweb} contains all strings from ``plain \.{CWEB},''
catalog \.{cweb-tl} contains a few extra strings specific to the \TeX~Live
interface, and catalog \.{web2c-help} contains the ``\.{--help}'' texts for
\.{CTANGLE} and \.{CWEAVE}.
@.cweb.mo@>
@.cweb-tl.mo@>
@.web2c-help.mo@>
@.--help@>

If such translation files are not available, you may want to improve this
system by checking out the sources and translating the strings in files
\.{cweb.pot}, \.{cweb-tl.pot}, and \.{web2c-help.pot}, and submitting the
resulting \.{*.po} files to the maintainers at \.{tex-k@@tug.org}.

\medskip \noindent \&{Note to maintainers:} \.{CWEB} in \TeX~Live generally
does \\{not} set |HAVE_GETTEXT| at build-time, so \.{i18n} is ``off'' by
default.  If you want to create \.{CWEB} executables with NLS support, you
have to recompile the \TeX~Live sources with a positive value for
|HAVE_GETTEXT| both in \.{"comm-w2c.ch"} and \.{"comm-w2c.h"}.  Also you
have to ``compile'' the NLS catalogs provided for \.{CWEB} in the source
tree with \.{msgfmt} and store the resulting \.{.mo} files at an appropriate
place in the file system.

Plans for \TeX~Live are to store NLS catalogs inside the ``\TeX\ Directory
Structure'' (TDS) and look them up with the help of the configuration variable
``|TEXMFLOCALEDIR|,'' which should contain a single absolute path definition.
Below we use the \Kpathsea/ function |kpse_var_expand| to evaluate this
variable from various origins and redirect the ``GNU~gettext utilities''
to a possibly different location than the canonical \.{/usr/share/locale}.

There are several ways to set |TEXMFLOCALEDIR|:
\smallskip
{\parindent5em
\item{(a)} a user-set environment variable \.{TEXMFLOCALEDIR}\hfil\break
    (overridden by \.{TEXMFLOCALEDIR\_cweb});
\item{(b)} a line in \Kpathsea/ configuration file \.{texmf.cnf},\hfil\break
    e.g., \.{TEXMFLOCALEDIR=\$TEXMFMAIN/locale}\hfil\break
    or \.{TEXMFLOCALEDIR.cweb=\$TEXMFMAIN/locale}.\par}

@<Set locale...@>=
setlocale(LC_MESSAGES, setlocale(LC_CTYPE, ""));
texmf_locale = kpse_var_expand ("${TEXMFLOCALEDIR}");

bindtextdomain("cweb",
  bindtextdomain("cweb-tl",
    bindtextdomain("web2c-help", @|
      strcmp(texmf_locale, "") ?
        texmf_locale : "/usr/share/locale")));

free(texmf_locale);
textdomain("cweb"); /* the majority of |"strings"| come from ``plain \.{CWEB}'' */
@.cweb.mo@>

@* File lookup with \Kpathsea/.  The \.{CTANGLE} and \.{CWEAVE} programs from
the original \.{CWEB} package use the compile-time default directory or the
value of the environment variable \.{CWEBINPUTS} as an alternative place to be
searched for files, if they could not be found in the current directory.

This version uses the \Kpathsea/ mechanism for searching files.
The directories to be searched for come from three sources:
\smallskip
{\parindent5em
\item{(a)} a user-set environment variable \.{CWEBINPUTS}
    (overridden by \.{CWEBINPUTS\_cweb});
\item{(b)} a line in \Kpathsea/ configuration file \.{texmf.cnf},\hfil\break
    e.g., \.{CWEBINPUTS=\$TEXMFDOTDIR:\$TEXMF/texmf/cweb//}\hfil\break
    or \.{CWEBINPUTS.cweb=\$TEXMFDOTDIR:\$TEXMF/texmf/cweb//};
\item{(c)} compile-time default directories (specified in
    \.{texmf.in}),\hfil\break
    i.e., \.{\$TEXMFDOTDIR:\$TEXMF/texmf/cweb//}.\par}

@d kpse_find_cweb(name) kpse_find_file(name,kpse_cweb_format,true)

@<Include files@>=
typedef bool boolean;
#define HAVE_BOOLEAN
#include <kpathsea/kpathsea.h> /* include every \Kpathsea/ header */
#include <w2c/config.h> /* \&{integer} */
#include <lib/lib.h> /* |versionstring| */
@#
#define CWEB
#include "help.h"

@ We set |kpse_program_name| to `\.{cweb}'.  This means if the variable
\.{CWEBINPUTS.cweb} is present in \.{texmf.cnf} (or \.{CWEBINPUTS\_cweb}
in the environment) its value will be used as the search path for filenames.
This allows different flavors of \.{CWEB} to have different search paths.

@<Set up |PROGNAME| feature and initialize the search path mechanism@>=
kpse_set_program_name(argv[0], "cweb");

@ When the files you expect are not found, the thing to do is to enable
`kpathsea' runtime debugging by assigning to the |kpathsea_debug| variable a
small number via the `\.{-d}' option. The meaning of this number is shown
below. To set more than one debugging option, simply sum the corresponding
numbers.
\medskip
\halign{\hskip5em\tt\hfil#&&\qquad\rm#\hfil\cr
 1&report `\.{stat}' calls\cr
 2&report lookups in all hash tables\cr
 4&report file openings and closings\cr
 8&report path information\cr
16&report directory list\cr
32&report on each file search\cr
64&report values of variables being looked up\cr}
\medskip
Debugging output is always written to |stderr|, and begins with the string
`\.{kdebug:}'.

@* System dependent changes. The most volatile stuff comes at the very end.

@ Modules for dealing with help messages and version info.

@<Display help message and |exit|@>=
cb_usagehelp(program==ctangle ? CTANGLEHELP :
  program==cweave ? CWEAVEHELP : CTWILLHELP, NULL);
@.--help@>

@ Special variants from \Kpathsea/ for i18n/t10n.
We simply filter the strings through the catalogs (if available).

@c
static void cb_usage (const_string str)
{
  textdomain("cweb-tl");
@.cweb-tl.mo@>
  fprintf(stderr, _("%s: Need one to three file arguments.\n"), str);
  fprintf(stderr, _("Try `%s --help' for more information.\n"), str);
@.--help@>
  textdomain("cweb");
@.cweb.mo@>
  history=fatal_message; exit(wrap_up());
}

static void cb_usagehelp (const_string *message, const_string bug_email)
{
  if (!bug_email)
    bug_email = "tex-k@@tug.org";
  textdomain("web2c-help");
@.web2c-help.mo@>
  while (*message) {
    printf("%s\n", strcmp("", *message) ? _(*message) : *message);
    ++message;
  }
  textdomain("cweb-tl");
@.cweb-tl.mo@>
  printf(_("\nEmail bug reports to %s.\n"), bug_email);
  textdomain("cweb");
@.cweb.mo@>
  history=spotless; exit(wrap_up());
}

@ The version information will not be translated, it uses a generic text
template in English.

@<Display version information and |exit|@>=
printversionandexit(cb_banner,
  program == ctwill ? "Donald E. Knuth" : "Silvio Levy and Donald E. Knuth",
  NULL, NULL);
@.--version@>

@ But the ``banner'' is, at least the first part.

@c
void cb_show_banner (void)
{
  assert(cb_banner[0]!='\0');
  textdomain("cweb-tl");
@.cweb-tl.mo@>
  printf("%s%s\n", _(cb_banner), versionstring);
  textdomain("cweb");
@.cweb.mo@>
}

@ @<Predecl...@>=
static void cb_usage (const_string str);@/
static void cb_usagehelp (const_string *message, const_string bug_email);@/
void cb_show_banner (void); /* |extern| for option \.{+b} */

@** Index.
@z
