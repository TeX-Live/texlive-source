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
\def\title{CTANGLE (Version 3.64)}
@y
\def\title{CTANGLE (Version 3.64 [\TeX~Live])}
@z

@x
  \centerline{(Version 3.64)}
@y
  \centerline{(Version 3.64 [\TeX~Live])}
@z

@x
@s not_eq normal @q unreserve a C++ keyword @>
@y
@z

@x
@d banner "This is CTANGLE (Version 3.64)\n"
@y
@d banner "This is CTANGLE, Version 3.64"
  /* will be extended by the \TeX~Live |versionstring| */
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

@<Include files@>=
#include <string.h>
@z

@x
int main (ac, av)
int ac;
char **av;
@y
int main (
int ac,
char **av)
@z

@x
  if (show_banner) printf(banner); /* print a ``banner line'' */
@y
  if (show_banner) cb_show_banner(); /* print a ``banner line'' */
@z

@x
@d max_bytes 90000 /* the number of bytes in identifiers,
@y
@d max_bytes 1000000 /* the number of bytes in identifiers,
@z

@x
@d max_toks 270000 /* number of bytes in compressed \CEE/ code */
@d max_names 4000 /* number of identifiers, strings, section names;
@y
@d max_toks 1000000 /* number of bytes in compressed \CEE/ code */
@d max_names 10239 /* number of identifiers, strings, section names;
@z

@x
@d max_texts 2500 /* number of replacement texts, must be less than 10240 */
@d hash_size 353 /* should be prime; used in |"common.w"| */
@y
@d max_texts 10239 /* number of replacement texts, must be less than 10240 */
@d hash_size 8501 /* should be prime; used in |"common.w"| */
@z

@x
@d buf_size 100 /* for \.{CWEAVE} and \.{CTANGLE} */
@y
@d buf_size 1000 /* for \.{CWEAVE} and \.{CTANGLE} */
@z

@x
@i common.h
@y
@i comm-w2c.h
@z

@x
the array |text_info|, and we use a |text_pointer| variable to refer
@y
the array |text_info|, and we use a \&{text\_pointer} variable to refer
@z

@x
If your machine does not support |unsigned char| you should change
the definition of \&{eight\_bits} to |unsigned short|.
@^system dependencies@>
@y
@z

@x
name_dir->equiv=(char *)text_info; /* the undefined section has no replacement text */
@y
init_node(name_dir); /* the undefined section has no replacement text */
@z

@x
int names_match(p,first,l)
name_pointer p; /* points to the proposed match */
char *first; /* position of first character of string */
int l; /* length of identifier */
{
@y
boolean names_match(@t\1\1@>
name_pointer p, /* points to the proposed match */
const char *first, /* position of first character of string */
size_t l, /* length of identifier */
eight_bits t@t\2\2@>) /* not used by \.{TANGLE} */
{@+(void)t;
@z

@x
void
init_node(node)
name_pointer node;
@y
void
init_node(
name_pointer node)
@z

@x
    node->equiv=(char *)text_info;
@y
    node->equiv=(void *)text_info;
@z

@x
void
init_p() {}
@y
void
init_p(name_pointer p,eight_bits t) {@+(void)p;@+(void)t;@+}
@z

@x
void
store_two_bytes(x)
sixteen_bits x;
@y
static void
store_two_bytes(
sixteen_bits x)
@z

@x
  if (tok_ptr+2>tok_mem_end) overflow("token");
@y
  if (tok_ptr+2>tok_mem_end) overflow(_("token"));
@z

@x
void
push_level(p) /* suspends the current level */
name_pointer p;
@y
static void
push_level(@t\1\1@> /* suspends the current level */
name_pointer p@t\2\2@>)
@z

@x
  if (stack_ptr==stack_end) overflow("stack");
@y
  if (stack_ptr==stack_end) overflow(_("stack"));
@z

@x
void
pop_level(flag) /* do this when |cur_byte| reaches |cur_end| */
int flag; /* |flag==0| means we are in |output_defs| */
@y
static void
pop_level(@t\1\1@> /* do this when |cur_byte| reaches |cur_end| */
int flag@t\2\2@>) /* |flag==0| means we are in |output_defs| */
@z

@x
void
get_output() /* sends next token to |out_char| */
@y
static void
get_output(void) /* sends next token to |out_char| */
@z

@x
  if ((a+name_dir)->equiv!=(char *)text_info) push_level(a+name_dir);
@y
  if ((a+name_dir)->equiv!=(void *)text_info) push_level(a+name_dir);
@z

@x
    printf("\n! Not present: <");
@y
    fputs(_("\n! Not present: <"),stdout);
@z

@x
void
flush_buffer() /* writes one line to output file */
@y
static void
flush_buffer(void) /* writes one line to output file */
@z

@x
      overflow("output files");
@y
      overflow(_("output files"));
@z

@x
void phase_two();
@y
static void phase_two(void);@/
@z

@x
void
phase_two () {
@y
static void
phase_two (void) {
@z

@x
    printf("\n! No program text was specified."); mark_harmless;
@y
    fputs(_("\n! No program text was specified."),stdout); mark_harmless;
@z

@x
        printf("\nWriting the output file (%s):",C_file_name);
@y
        printf(_("\nWriting the output file (%s):"),C_file_name);
@z

@x
        printf("\nWriting the output files:");
@y
        fputs(_("\nWriting the output files:"),stdout);
@z

@x
    if(show_happiness) printf("\nDone.");
@y
    if (show_happiness) {
      if (show_progress) new_line;
      fputs(_("Done."),stdout);
    }
@z

@x
@<Write all the named output files@>=
for (an_output_file=end_output_files; an_output_file>cur_out_file;) {
    an_output_file--;
    sprint_section_name(output_file_name,*an_output_file);
    fclose(C_file);
    C_file=fopen(output_file_name,"w");
    if (C_file ==0) fatal("! Cannot open output file:",output_file_name);
@.Cannot open output file@>
    printf("\n(%s)",output_file_name); update_terminal;
    cur_line=1;
    stack_ptr=stack+1;
    cur_name= (*an_output_file);
    cur_repl= (text_pointer)cur_name->equiv;
    cur_byte=cur_repl->tok_start;
    cur_end=(cur_repl+1)->tok_start;
    while (stack_ptr > stack) get_output();
    flush_buffer();
}
@y
@<Write all the named output files@>=
fclose(C_file); C_file=NULL;
@<Update the primary result when it has changed@>@;
for (an_output_file=end_output_files; an_output_file>cur_out_file;) {
    an_output_file--;
    sprint_section_name(output_file_name,*an_output_file);
    if((C_file=fopen(check_file_name,"wb"))==NULL)
      fatal(_("! Cannot open output file "),check_file_name);
@.Cannot open output file@>
    if (show_progress) { printf("\n(%s)",output_file_name); update_terminal; }
    cur_line=1;
    stack_ptr=stack+1;
    cur_name= (*an_output_file);
    cur_repl= (text_pointer)cur_name->equiv;
    cur_byte=cur_repl->tok_start;
    cur_end=(cur_repl+1)->tok_start;
    while (stack_ptr > stack) get_output();
    flush_buffer(); fclose(C_file); C_file=NULL;
    @<Update the secondary results when they have changed@>@;
}
strcpy(check_file_name,""); /* We want to get rid of the temporary file */
@z

@x
void output_defs();
@y
static void output_defs(void);@/
@z

@x
void
output_defs()
@y
static void
output_defs(void)
@z

@x
          else if (a<050000) { confusion("macro defs have strange char");}
@y
          else if (a<050000) { confusion(_("macro defs have strange char"));}
@z

@x
static void out_char();
@y
static void out_char(eight_bits);@/
@z

@x
static void
out_char(cur_char)
eight_bits cur_char;
@y
static void
out_char(
eight_bits cur_char)
@z

@x
@ @<Cases like \.{!=}@>=
case plus_plus: C_putc('+'); C_putc('+'); out_state=normal; break;
case minus_minus: C_putc('-'); C_putc('-'); out_state=normal; break;
case minus_gt: C_putc('-'); C_putc('>'); out_state=normal; break;
case gt_gt: C_putc('>'); C_putc('>'); out_state=normal; break;
case eq_eq: C_putc('='); C_putc('='); out_state=normal; break;
case lt_lt: C_putc('<'); C_putc('<'); out_state=normal; break;
case gt_eq: C_putc('>'); C_putc('='); out_state=normal; break;
case lt_eq: C_putc('<'); C_putc('='); out_state=normal; break;
case not_eq: C_putc('!'); C_putc('='); out_state=normal; break;
case and_and: C_putc('&'); C_putc('&'); out_state=normal; break;
case or_or: C_putc('|'); C_putc('|'); out_state=normal; break;
case dot_dot_dot: C_putc('.'); C_putc('.'); C_putc('.'); out_state=normal;
    break;
case colon_colon: C_putc(':'); C_putc(':'); out_state=normal; break;
case period_ast: C_putc('.'); C_putc('*'); out_state=normal; break;
case minus_gt_ast: C_putc('-'); C_putc('>'); C_putc('*'); out_state=normal;
    break;
@y
@ @<Cases like \.{!=}@>=
case plus_plus: C_putc('+');@+C_putc('+'); out_state=normal; break;
case minus_minus: C_putc('-');@+C_putc('-'); out_state=normal; break;
case minus_gt: C_putc('-');@+C_putc('>'); out_state=normal; break;
case gt_gt: C_putc('>');@+C_putc('>'); out_state=normal; break;
case eq_eq: C_putc('=');@+C_putc('='); out_state=normal; break;
case lt_lt: C_putc('<');@+C_putc('<'); out_state=normal; break;
case gt_eq: C_putc('>');@+C_putc('='); out_state=normal; break;
case lt_eq: C_putc('<');@+C_putc('='); out_state=normal; break;
case non_eq: C_putc('!');@+C_putc('='); out_state=normal; break;
case and_and: C_putc('&');@+C_putc('&'); out_state=normal; break;
case or_or: C_putc('|');@+C_putc('|'); out_state=normal; break;
case dot_dot_dot: C_putc('.');@+C_putc('.');@+C_putc('.'); out_state=normal;
    break;
case colon_colon: C_putc(':');@+C_putc(':'); out_state=normal; break;
case period_ast: C_putc('.');@+C_putc('*'); out_state=normal; break;
case minus_gt_ast: C_putc('-');@+C_putc('>');@+C_putc('*'); out_state=normal;
    break;
@z

@x
eight_bits
skip_ahead() /* skip to next control code */
@y
static eight_bits
skip_ahead(void) /* skip to next control code */
@z

@x
int skip_comment(is_long_comment) /* skips over comments */
boolean is_long_comment;
@y
static boolean skip_comment(@t\1\1@> /* skips over comments */
boolean is_long_comment@t\2\2@>)
@z

@x
          err_print("! Input ended in mid-comment");
@y
          err_print(_("! Input ended in mid-comment"));
@z

@x
        err_print("! Section name ended in mid-comment"); loc--;
@y
        err_print(_("! Section name ended in mid-comment")); loc--;
@z

@x
eight_bits
get_next() /* produces the next input token */
@y
static eight_bits
get_next(void) /* produces the next input token */
@z

@x
    else if (c=='\'' || c=='"' || (c=='L'&&(*loc=='\'' || *loc=='"')))
@y
    else if (c=='\'' || c=='"'@|
           || ((c=='L' || c=='u' || c=='U')&&(*loc=='\'' || *loc=='"'))@|
           || ((c=='u' && *loc=='8')&&(*(loc+1)=='\'' || *(loc+1)=='"')))
@z

@x
    else if (*loc=='>') if (*(loc+1)=='*') {loc++; compress(minus_gt_ast);}
                        else compress(minus_gt); break;
@y
    else { if (*loc=='>') { if (*(loc+1)=='*') {loc++; compress(minus_gt_ast);}
                        else compress(minus_gt); } } break;
@z

@x
  case '!': if (*loc=='=') compress(not_eq); break;
@y
  case '!': if (*loc=='=') compress(non_eq); break;
@z

@x
  while (isalpha(*++loc) || isdigit(*loc) || isxalpha(*loc) || ishigh(*loc));
@y
  while (isalpha((eight_bits)*++loc) || isdigit((eight_bits)*loc) @|
      || isxalpha((eight_bits)*loc) || ishigh((eight_bits)*loc));
@z

@x
  if (delim=='L') { /* wide character constant */
    delim=*loc++; *++id_loc=delim;
  }
@y
  if (delim=='L' || delim=='u' || delim=='U') { /* wide character constant */
    if (delim=='u' && *loc=='8') { *++id_loc=*loc++; }
    delim=*loc++; *++id_loc=delim;
  }
@z

@x
        err_print("! String didn't end"); loc=limit; break;
@y
        err_print(_("! String didn't end")); loc=limit; break;
@z

@x
        err_print("! Input ended in middle of string"); loc=buffer; break;
@y
        err_print(_("! Input ended in middle of string")); loc=buffer; break;
@z

@x
    printf("\n! String too long: ");
@y
    fputs(_("\n! String too long: "),stdout);
@z

@x
    case translit_code: err_print("! Use @@l in limbo only"); continue;
@y
    case translit_code: err_print(_("! Use @@l in limbo only")); continue;
@z

@x
        err_print("! Double @@ should be used in control text");
@y
        err_print(_("! Double @@ should be used in control text"));
@z

@x
        err_print("! Double @@ should be used in ASCII constant");
@y
        err_print(_("! Double @@ should be used in ASCII constant"));
@z

@x
        err_print("! String didn't end"); loc=limit-1; break;
@y
        err_print(_("! String didn't end")); loc=limit-1; break;
@z

@x
    err_print("! Input ended in section name");
@y
    err_print(_("! Input ended in section name"));
@z

@x
  printf("\n! Section name too long: ");
@y
  fputs(_("\n! Section name too long: "),stdout);
@z

@x
    err_print("! Section name didn't end"); break;
@y
    err_print(_("! Section name didn't end")); break;
@z

@x
    err_print("! Nesting of section names not allowed"); break;
@y
    err_print(_("! Nesting of section names not allowed")); break;
@z

@x
  if (loc>=limit) err_print("! Verbatim string didn't end");
@y
  if (loc>=limit) err_print(_("! Verbatim string didn't end"));
@z

@x
@d app_repl(c)  {if (tok_ptr==tok_mem_end) overflow("token"); *tok_ptr++=c;}
@y
@d app_repl(c) {if (tok_ptr==tok_mem_end) overflow(_("token")); *tok_ptr++=c;}
@z

@x
void
scan_repl(t) /* creates a replacement text */
eight_bits t;
@y
static void
scan_repl(@t\1\1@> /* creates a replacement text */
eight_bits t@t\2\2@>)
@z

@x
  if (text_ptr>text_info_end) overflow("text");
@y
  if (text_ptr>text_info_end) overflow(_("text"));
@z

@x
{int a=id_lookup(id_first,id_loc,0)-name_dir; app_repl((a / 0400)+0200);
  app_repl(a % 0400);}
@y
{int a_l=id_lookup(id_first,id_loc,0)-name_dir; app_repl((a_l / 0400)+0200);
  app_repl(a_l % 0400);}
@z

@x
case output_defs_code: if (t!=section_name) err_print("! Misplaced @@h");
@y
case output_defs_code: if (t!=section_name) err_print(_("! Misplaced @@h"));
@z

@x
    err_print("! @@d, @@f and @@c are ignored in C text"); continue;
@y
    err_print(_("! @@d, @@f and @@c are ignored in C text")); continue;
@z

@x
  if (*try_loc=='=') err_print ("! Missing `@@ ' before a named section");
@y
  if (*try_loc=='=') err_print (_("! Missing `@@ ' before a named section"));
@z

@x
      else err_print("! Double @@ should be used in string");
@y
      else err_print(_("! Double @@ should be used in string"));
@z

@x
        c=toupper(*id_first)-'A'+10;
@y
        c=toupper((eight_bits)*id_first)-'A'+10;
@z

@x
        c=16*c+toupper(*id_first)-'A'+10;
@y
        c=16*c+toupper((eight_bits)*id_first)-'A'+10;
@z

@x
    default: err_print("! Unrecognized escape sequence");
@y
    default: err_print(_("! Unrecognized escape sequence"));
@z

@x
void
scan_section()
@y
static void
scan_section(void)
@z

@x
    err_print("! Definition flushed, must start with identifier");
@y
    err_print(_("! Definition flushed, must start with identifier"));
@z

@x
else if (p->equiv==(char *)text_info) p->equiv=(char *)cur_text;
@y
else if (p->equiv==(void *)text_info) p->equiv=(void *)cur_text;
@z

@x
void phase_one();
@y
static void phase_one(void);@/
@z

@x
void
phase_one() {
@y
static void
phase_one(void) {
@z

@x
void skip_limbo();
@y
static void skip_limbo(void);@/
@z

@x
void
skip_limbo()
@y
static void
skip_limbo(void)
@z

@x
            err_print("! Double @@ should be used in control text");
@y
            err_print(_("! Double @@ should be used in control text"));
@z

@x
        default: err_print("! Double @@ should be used in limbo");
@y
        default: err_print(_("! Double @@ should be used in limbo"));
@z

@x
    err_print("! Improper hex number following @@l");
@y
    err_print(_("! Improper hex number following @@l"));
@z

@x
      err_print("! Replacement string in @@l too long");
@y
      err_print(_("! Replacement string in @@l too long"));
@z

@x
      strncpy(translit[i-0200],beg,loc-beg);
@y
      strncpy(translit[i-0200],beg,(size_t)(loc-beg));
@z

@x
void
print_stats() {
@y
void
print_stats(void) {
@z

@x
  printf("\nMemory usage statistics:\n");
  printf("%ld names (out of %ld)\n",
          (long)(name_ptr-name_dir),(long)max_names);
  printf("%ld replacement texts (out of %ld)\n",
          (long)(text_ptr-text_info),(long)max_texts);
  printf("%ld bytes (out of %ld)\n",
          (long)(byte_ptr-byte_mem),(long)max_bytes);
  printf("%ld tokens (out of %ld)\n",
@y
  puts(_("\nMemory usage statistics:"));
  printf(_("%ld names (out of %ld)\n"),
          (long)(name_ptr-name_dir),(long)max_names);
  printf(_("%ld replacement texts (out of %ld)\n"),
          (long)(text_ptr-text_info),(long)max_texts);
  printf(_("%ld bytes (out of %ld)\n"),
          (long)(byte_ptr-byte_mem),(long)max_bytes);
  printf(_("%ld tokens (out of %ld)\n"),
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
section should have the same number as the original ``\&{96.~Index},'' and
additional material follows below.

@* Function declarations.  Here are declarations---conforming to
{\mc ANSI~C}---of all functions in this code, as far as they are
not already in |"common.h"|.  These are private to \.{CTANGLE}.

@<Predecl...@>=
static eight_bits get_next(void);@/
static eight_bits skip_ahead(void);@/
static boolean skip_comment(boolean);@/
static void flush_buffer(void);@/
static void get_output(void);@/
static void pop_level(int);@/
static void push_level(name_pointer);@/
static void scan_repl(eight_bits);@/
static void scan_section(void);@/
static void store_two_bytes(sixteen_bits);@/

@* Output file update.  Most \CEE/ projects are controlled by a
\.{Makefile} that automatically takes care of the temporal dependecies
between the different source modules.  It is suitable that \.{CWEB} doesn't
create new output for all existing files, when there are only changes to
some of them. Thus the \.{make} process will only recompile those modules
where necessary. The idea and basic implementation of this mechanism can
be found in the program \.{NUWEB} by Preston Briggs, to whom credit is due.

@<Update the primary result...@>=
if((C_file=fopen(C_file_name,"r"))!=NULL) {
  @<Set up the comparison of temporary output@>@;
  @<Create the primary output depending on the comparison@>@;
} else
  rename(check_file_name,C_file_name); /* This was the first run */

@ @<Set up the comparison of temporary output@>=
  char x[BUFSIZ],y[BUFSIZ];
  int x_size,y_size,comparison;

  if((check_file=fopen(check_file_name,"r"))==NULL)
    fatal(_("! Cannot open output file "),check_file_name);
@.Cannot open output file@>

  @<Compare the temporary output to the previous output@>@;

  fclose(C_file); C_file=NULL;
  fclose(check_file); check_file=NULL;

@ We hope that this runs fast on most systems.

@<Compare the temp...@>=
do {
  x_size = fread(x,1,BUFSIZ,C_file);
  y_size = fread(y,1,BUFSIZ,check_file);
  comparison = (x_size == y_size); /* Do not merge these statements! */
  if(comparison) comparison = !memcmp(x,y,x_size);
} while(comparison && !feof(C_file) && !feof(check_file));

@ Note the superfluous call to |remove| before |rename|.  We're using it to
get around a bug in some implementations of |rename|.

@<Create the primary output...@>=
if(comparison)
  remove(check_file_name); /* The output remains untouched */
else {
  remove(C_file_name);
  rename(check_file_name,C_file_name);
}

@ @<Update the secondary results...@>=
if((C_file=fopen(output_file_name,"r"))!=NULL) {
  @<Set up the comparison of temporary output@>@;
  @<Create the secondary output depending on the comparison@>@;
} else
  rename(check_file_name,output_file_name); /* This was the first run */

@ Again, we use a call to |remove| before |rename|.

@<Create the secondary output...@>=
if(comparison)
  remove(check_file_name); /* The output remains untouched */
else {
  remove(output_file_name);
  rename(check_file_name,output_file_name);
}

@* Put ``version'' information in a single spot.
Don't do this at home, kids! Push our local macro to the variable in \.{COMMON}
for printing the |banner| and the |versionstring| from there.

@d max_banner 50

@<Common code...@>=
extern char cb_banner[];

@ @<Set init...@>=
  strncpy(cb_banner,banner,max_banner-1);

@** Index.
@z

