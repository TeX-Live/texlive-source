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
\def\title{CWEAVE (Version 3.64)}
@y
\def\title{CWEAVE (Version 3.64 [\TeX~Live])}
@z

@x
  \centerline{(Version 3.64)}
@y
  \centerline{(Version 3.64 [\TeX~Live])}
@z

@x
\def\botofcontents{\vfill
@y
\def\covernote{\vbox{%
@z

@x
}
@y
}}
\datecontentspage
@z

@x
@s not_eq normal @q unreserve a C++ keyword @>
@y
@z

@x
@d banner "This is CWEAVE (Version 3.64)\n"
@y
@d banner "This is CWEAVE, Version 3.64"
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
of predeclaring the standard system functions |@!strlen|, |@!strcmp|,
|@!strcpy|, |@!strncmp|, and |@!strncpy|.
@^system dependencies@>

@<Include files@>=
#include <string.h>
@z

@x
int main (ac, av)
int ac; /* argument count */
char **av; /* argument values */
@y
int main (@t\1\1@>
int ac, /* argument count */
char **av@t\2\2@>) /* argument values */
@z

@x
  make_xrefs=force_lines=make_pb=1; /* controlled by command-line options */
  common_init();
  @<Set initial values@>;
@y
  @<Set initial values@>;
  common_init();
  @<Start \TEX/ output@>;
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
@d max_names 4000 /* number of identifiers, strings, section names;
@y
@d max_names 10239 /* number of identifiers, strings, section names;
@z

@x
@d max_sections 2000 /* greater than the total number of sections */
@d hash_size 353 /* should be prime */
@d buf_size 100 /* maximum length of input line, plus one */
@y
@d max_sections 10239 /* greater than the total number of sections */
@d hash_size 8501 /* should be prime */
@d buf_size 1000 /* maximum length of input line, plus one */
@z

@x
@d max_refs 20000 /* number of cross-references; must be less than 65536 */
@d max_toks 20000 /* number of symbols in \CEE/ texts being parsed;
@y
@d max_refs 65535 /* number of cross-references; must be less than 65536 */
@d max_toks 65535 /* number of symbols in \CEE/ texts being parsed;
@z

@x
@d max_texts 4000 /* number of phrases in \CEE/ texts being parsed;
@y
@d max_texts 10239 /* number of phrases in \CEE/ texts being parsed;
@z

@x
@d max_scraps 2000 /* number of tokens in \CEE/ texts being parsed */
@d stack_size 400 /* number of simultaneous output levels */
@y
@d max_scraps 10000 /* number of tokens in \CEE/ texts being parsed */
@d stack_size 2000 /* number of simultaneous output levels */
@z

@x
@i common.h
@y
@i comm-w2c.h
@z

@x
xref_ptr=xmem; name_dir->xref=(char*)xmem; xref_switch=0; section_xref_switch=0;
@y
xref_ptr=xmem; init_node(name_dir); xref_switch=0; section_xref_switch=0;
@z

@x
@d append_xref(c) if (xref_ptr==xmem_end) overflow("cross-reference");
@y
@d append_xref(c) if (xref_ptr==xmem_end) overflow(_("cross-reference"));
@z

@x
@d no_xref (flags['x']==0)
@d make_xrefs flags['x'] /* should cross references be output? */
@y
@d no_xref (!make_xrefs) /* should cross references be suppressed? */
@z

@x
void
new_xref(p)
name_pointer p;
@y
static void
new_xref(
name_pointer p)
@z

@x
  append_xref(m); xref_ptr->xlink=q; p->xref=(char*)xref_ptr;
@y
  append_xref(m); xref_ptr->xlink=q; update_node(p);
@z

@x
void
new_section_xref(p)
name_pointer p;
@y
static void
new_section_xref(
name_pointer p)
@z

@x
  if (r==xmem) p->xref=(char*)xref_ptr;
@y
  if (r==xmem) update_node(p);
@z

@x
void
set_file_flag(p)
name_pointer p;
@y
static void
set_file_flag(
name_pointer p)
@z

@x
  p->xref = (char *)xref_ptr;
@y
  update_node(p);
@z

@x
further details about them will be explained later. A |text_pointer| variable
is an index into |tok_start|.
@y
further details about them will be explained later. A \&{text\_pointer}
variable is an index into |tok_start|.
@z

@x
tok_ptr=tok_mem+1; text_ptr=tok_start+1; tok_start[0]=tok_mem+1;
tok_start[1]=tok_mem+1;
max_tok_ptr=tok_mem+1; max_text_ptr=tok_start+1;
@y
tok_ptr=max_tok_ptr=tok_mem+1;@/
tok_start[0]=tok_start[1]=tok_mem+1;@/
text_ptr=max_text_ptr=tok_start+1;
@z

@x
int names_match(p,first,l,t)
name_pointer p; /* points to the proposed match */
char *first; /* position of first character of string */
int l; /* length of identifier */
eight_bits t; /* desired ilk */
@y
boolean names_match(@t\1\1@>
name_pointer p, /* points to the proposed match */
const char *first, /* position of first character of string */
size_t l, /* length of identifier */
eight_bits t@t\2\2@>) /* desired |ilk| */
@z

@x
void
init_p(p,t)
name_pointer p;
eight_bits t;
@y
void
init_p(
name_pointer p,
eight_bits t)
@z

@x
  p->ilk=t; p->xref=(char*)xmem;
@y
  p->ilk=t; init_node(p);
@z

@x
void
init_node(p)
name_pointer p;
@y
void
init_node(
name_pointer p)
@z

@x
  p->xref=(char*)xmem;
}
@y
  p->xref=(void *)xmem;
}

static void
update_node(
name_pointer p)
{
  p->xref=(void *)xref_ptr;
}
@z

@x
void   skip_limbo();
@y
static void skip_limbo(void);@/
@z

@x
void
skip_limbo() {
@y
static void
skip_limbo(void) {
@z

@x
unsigned
skip_TeX() /* skip past pure \TEX/ code */
@y
static eight_bits
skip_TeX(void) /* skip past pure \TEX/ code */
@z

@x
eight_bits get_next();
@y
static eight_bits get_next(void);@/
@z

@x
eight_bits
get_next() /* produces the next input token */
{@+eight_bits c; /* the current character */
@y
static eight_bits
get_next(void) /* produces the next input token */
{
  eight_bits c; /* the current character */
@z

@x
    else if (c=='\'' || c=='"' || (c=='L'&&(*loc=='\'' || *loc=='"'))@|
@y
    else if (c=='\'' || c=='"'@|
           || ((c=='L' || c=='u' || c=='U')&&(*loc=='\'' || *loc=='"'))@|
           || ((c=='u' && *loc=='8')&&(*(loc+1)=='\'' || *(loc+1)=='"'))@|
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
    *id_loc++='$'; *id_loc++=toupper(*loc); loc++;
@y
    *id_loc++='$'; *id_loc++=toupper((eight_bits)*loc); loc++;
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
    if (c=='\\') if (loc>=limit) continue;
      else if (++id_loc<=section_text_end) {
        *id_loc = '\\'; c=*loc++;
      }
@y
    if (c=='\\') { if (loc>=limit) continue;
      else { if (++id_loc<=section_text_end) {
        *id_loc = '\\'; c=*loc++;
      } } }
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
    case xref_roman: case xref_wildcard: case xref_typewriter:
    case noop: case TeX_string: c=ccode[c]; skip_restricted(); return(c);
@y
    case xref_roman: case xref_wildcard: case xref_typewriter: case noop:
    case TeX_string: c=ccode[(eight_bits)c]; skip_restricted(); return(c);
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
    err_print("! Control codes are forbidden in section name"); break;
@y
    err_print(_("! Control codes are forbidden in section name")); break;
@z

@x
void skip_restricted();
@y
void skip_restricted(void);@/
@z

@x
void
skip_restricted()
@y
void
skip_restricted(void)
@z

@x
    err_print("! Control text didn't end"); loc=limit;
@y
    err_print(_("! Control text didn't end")); loc=limit;
@z

@x
      err_print("! Control codes are forbidden in control text");
@y
      err_print(_("! Control codes are forbidden in control text"));
@z

@x
  if (loc>=limit) err_print("! Verbatim string didn't end");
@y
  if (loc>=limit) err_print(_("! Verbatim string didn't end"));
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
  if (++section_count==max_sections) overflow("section number");
@y
  if (++section_count==max_sections) overflow(_("section number"));
@z

@x
void C_xref();
@y
static void C_xref(eight_bits);@/
@z

@x
void
C_xref( spec_ctrl ) /* makes cross-references for \CEE/ identifiers */
  eight_bits spec_ctrl;
@y
static void
C_xref(@t\1\1@> /* makes cross-references for \CEE/ identifiers */
  eight_bits spec_ctrl@t\2\2@>)
@z

@x
void outer_xref();
@y
static void outer_xref(void);@/
@z

@x
void
outer_xref() /* extension of |C_xref| */
@y
static void
outer_xref(void) /* extension of |C_xref| */
@z

@x
    case translit_code: err_print("! Use @@l in limbo only"); continue;
@y
    case translit_code: err_print(_("! Use @@l in limbo only")); continue;
@z

@x
            else lhs->xref=(char*)q->xlink;
@y
            else lhs->xref=(void *)q->xlink;
@z

@x
    err_print("! Missing left identifier of @@s");
@y
    err_print(_("! Missing left identifier of @@s"));
@z

@x
      err_print("! Missing right identifier of @@s");
@y
      err_print(_("! Missing right identifier of @@s"));
@z

@x
void section_check();
@y
static void section_check(name_pointer);@/
@z

@x
void
section_check(p)
name_pointer p; /* print anomalies in subtree |p| */
@y
static void
section_check(
name_pointer p) /* print anomalies in subtree |p| */
@z

@x
      printf("\n! Never defined: <"); print_section_name(p); putchar('>'); mark_harmless;
@y
      fputs(_("\n! Never defined: <"),stdout);
      print_section_name(p); putchar('>'); mark_harmless;
@z

@x
      printf("\n! Never used: <"); print_section_name(p); putchar('>'); mark_harmless;
@y
      fputs(_("\n! Never used: <"),stdout);
      print_section_name(p); putchar('>'); mark_harmless;
@z

@x
If the |per_cent| parameter is 1 a |'%'| is appended to the line
@y
If the |per_cent| parameter is 1, a |'%'| is appended to the line
@z

@x
@d tex_printf(c) fprintf(active_file,c)
@y
@d tex_printf(c) fprintf(active_file,"%s",c)
@d tex_puts(c) fputs(c,active_file)
@z

@x
void
flush_buffer(b,per_cent,carryover)
char *b; /* outputs from |out_buf+1| to |b|,where |b<=out_ptr| */
boolean per_cent,carryover;
@y
static void
flush_buffer(@t\1\1@>
char *b, /* outputs from |out_buf+1| to |b|, where |b<=out_ptr| */
boolean per_cent,boolean carryover@t\2\2@>)
@z

@x
  if (b<out_ptr) strncpy(out_buf+1,b+1,out_ptr-b);
@y
  if (b<out_ptr) strncpy(out_buf+1,b+1,(size_t)(out_ptr-b));
@z

@x
void
finish_line() /* do this at the end of a line */
@y
static void
finish_line(void) /* do this at the end of a line */
@z

@x
@ In particular, the |finish_line| procedure is called near the very
beginning of phase two. We initialize the output variables in a slightly
tricky way so that the first line of the output file will be
`\.{\\input cwebmac}'.

@<Set init...@>=
out_ptr=out_buf+1; out_line=1; active_file=tex_file;
*out_ptr='c'; tex_printf("\\input cwebma");
@y
@ In particular, the |finish_line| procedure is called near the very
beginning of phase two. We initialize the output variables in a slightly
tricky way so that the first line of the output file will be dependent of
the user language set by the `\.{+l}' option and its argument.  If you call
\.{CWEAVE} with `\.{+lX}' (or `\.{-lX}' as well), where `\.X' is the
(possibly empty) string of characters to the right of~`\.l', `\.X'~will be
prepended to `\.{cwebmac.tex}', e.g., if you call \.{CWEAVE} with
`\.{+ldeutsch}', you will receive the line `\.{\\input deutschcwebmac}'.
Without this option the first line of the output file will be
`\.{\\input cwebmac}'.

@<Start \TEX/...@>=
out_ptr=out_buf+1; out_line=1; active_file=tex_file; *out_ptr='c';
tex_puts("\\input ");
tex_printf(use_language);
tex_puts("cwebma");
@z

@x
void
out_str(s) /* output characters from |s| to end of string */
char *s;
@y
static void
out_str(@t\1\1@> /* output characters from |s| to end of string */
const char*s@t\2\2@>)
@z

@x
void break_out();
@y
static void break_out(void);@/
@z

@x
void
break_out() /* finds a way to break the output line */
@y
static void
break_out(void) /* finds a way to break the output line */
@z

@x
  printf("\n! Line had to be broken (output l. %d):\n",out_line);
@y
  printf(_("\n! Line had to be broken (output l. %d):\n"),out_line);
@z

@x
void
out_section(n)
sixteen_bits n;
@y
static void
out_section(
sixteen_bits n)
@z

@x
void
out_name(p,quote_xalpha)
name_pointer p;
boolean quote_xalpha;
@y
static void
out_name(
name_pointer p,
boolean quote_xalpha)
@z

@x
void
copy_limbo()
@y
static void
copy_limbo(void)
@z

@x
        default: err_print("! Double @@ should be used in limbo");
@y
        default: err_print(_("! Double @@ should be used in limbo"));
@z

@x
eight_bits
copy_TeX()
@y
static eight_bits
copy_TeX(void)
@z

@x
@d app_tok(c) {if (tok_ptr+2>tok_mem_end) overflow("token"); *(tok_ptr++)=c;}
@y
@d app_tok(c) {if (tok_ptr+2>tok_mem_end) overflow(_("token")); *(tok_ptr++)=c;}
@z

@x
int copy_comment();
@y
static int copy_comment(boolean,int);@/
@z

@x
int copy_comment(is_long_comment,bal) /* copies \TEX/ code in comments */
boolean is_long_comment; /* is this a traditional \CEE/ comment? */
int bal; /* brace balance */
@y
static int copy_comment(@t\1\1@> /* copies \TeX\ code in comments */
boolean is_long_comment, /* is this a traditional \CEE/ comment? */
int bal@t\2\2@>) /* brace balance */
@z

@x
          err_print("! Input ended in mid-comment");
@y
          err_print(_("! Input ended in mid-comment"));
@z

@x
        if (bal>1) err_print("! Missing } in comment");
@y
        if (bal>1) err_print(_("! Missing } in comment"));
@z

@x
      else {err_print("! Extra } in comment");
@y
      else {err_print(_("! Extra } in comment"));
@z

@x
  if (bal>1) err_print("! Missing } in comment");
@y
  if (bal>1) err_print(_("! Missing } in comment"));
@z

@x
    err_print("! Illegal use of @@ in comment");
@y
    err_print(_("! Illegal use of @@ in comment"));
@z

@x
else if (c=='\\' && *loc!='@@')
  if (phase==2) app_tok(*(loc++))@; else loc++;
@y
else { if (c=='\\' && *loc!='@@') {
  if (phase==2) app_tok(*(loc++))@; else loc++; } }
@z

@x
eight_bits cat_index;

@ @<Set in...@>=
    for (cat_index=0;cat_index<255;cat_index++)
      strcpy(cat_name[cat_index],"UNKNOWN");
@y

@ @<Set in...@>=
{int c; for (c=0;c<256;c++) strcpy(cat_name[c],"UNKNOWN");}
@z

@x
void
print_cat(c) /* symbolic printout of a category */
eight_bits c;
@y
static void
print_cat(@t\1\1@> /* symbolic printout of a category */
eight_bits c@t\2\2@>)
@z

@x
  printf(cat_name[c]);
@y
  fputs(cat_name[c],stdout);
@z

@x
void
print_text(p) /* prints a token list for debugging; not used in |main| */
text_pointer p;
@y
#ifdef DEAD_CODE /* not used in |main| */
static void
print_text(@t\1\1@> /* prints a token list for debugging; not used in |main| */
text_pointer p@t\2\2@>)
@z

@x
  fflush(stdout);
}
@y
  update_terminal;
}
#endif /* |DEAD_CODE| */
@z

@x
@d app(a) *(tok_ptr++)=a
@d app1(a) *(tok_ptr++)=tok_flag+(int)((a)->trans-tok_start)
@y
@d app(a) *(tok_ptr++)=(token)(a)
@d app1(a) *(tok_ptr++)=(token)(tok_flag+(int)((a)->trans-tok_start))
@z

@x
void
app_str(s)
char *s;
@y
static void
app_str(
const char *s)
@z

@x
void
big_app(a)
token a;
@y
static void
big_app(
token a)
@z

@x
void
big_app1(a)
scrap_pointer a;
@y
static void
big_app1(
scrap_pointer a)
@z

@x
        && pp->cat!=prerangle
@y
        && pp->cat!=prerangle @|
@z

@x
        && pp->cat!=ftemplate
@y
        && pp->cat!=ftemplate @|
@z

@x
token_pointer
find_first_ident(p)
text_pointer p;
@y
static token_pointer
find_first_ident(
text_pointer p)
@z

@x
void
make_reserved(p) /* make the first identifier in |p->trans| like |int| */
scrap_pointer p;
@y
static void
make_reserved(@t\1\1@> /* make the first identifier in |p->trans| like |int| */
scrap_pointer p@t\2\2@>)
@z

@x
void
make_underlined(p)
/* underline the entry for the first identifier in |p->trans| */
scrap_pointer p;
@y
static void
make_underlined(@t\1\1@>
/* underline the entry for the first identifier in |p->trans| */
scrap_pointer p@t\2\2@>)
@z

@x
void  underline_xref();
@y
static void underline_xref(name_pointer);@/
@z

@x
void
underline_xref(p)
name_pointer p;
@y
static void
underline_xref(
name_pointer p)
@z

@x
  p->xref=(char*)xref_ptr;
@y
  update_node(p);
@z

@x
@<Cases for |exp|@>=
if (cat1==lbrace || cat1==int_like || cat1==decl) {
  make_underlined(pp); big_app1(pp); big_app(indent); app(indent);
  reduce(pp,1,fn_decl,0,1);
}
@y
\.{CWEAVE} indents declarations after old-style function definitions.
With the \.{-i} option they will come out flush left.  You won't see
any difference if you use ANSI-style function definitions.

@d indent_param_decl flags['i'] /* should formal parameter declarations be indented? */

@<Cases for |exp|@>=
if(cat1==lbrace || cat1==int_like || cat1==decl) {
  make_underlined(pp); big_app1(pp);
  if (indent_param_decl) {
    big_app(indent); app(indent);
  }
  reduce(pp,1,fn_decl,0,1);
}
@z

@x
@ @<Cases for |decl_head|@>=
if (cat1==comma) {
  big_app2(pp); big_app(' '); reduce(pp,2,decl_head,-1,33);
}
else if (cat1==ubinop) {
  big_app1(pp); big_app('{'); big_app1(pp+1); big_app('}');
  reduce(pp,2,decl_head,-1,34);
}
else if (cat1==exp && cat2!=lpar && cat2!=exp && cat2!=cast) {
  make_underlined(pp+1); squash(pp,2,decl_head,-1,35);
}
else if ((cat1==binop||cat1==colon) && cat2==exp && (cat3==comma ||
    cat3==semi || cat3==rpar))
  squash(pp,3,decl_head,-1,36);
else if (cat1==cast) squash(pp,2,decl_head,-1,37);
else if (cat1==lbrace || cat1==int_like || cat1==decl) {
  big_app1(pp); big_app(indent); app(indent); reduce(pp,1,fn_decl,0,38);
}
else if (cat1==semi) squash(pp,2,decl,-1,39);
@y
@ @<Cases for |decl_head|@>=
if (cat1==comma) {
  big_app2(pp); big_app(' '); reduce(pp,2,decl_head,-1,33);
}
else if (cat1==ubinop) {
  big_app1(pp); big_app('{'); big_app1(pp+1); big_app('}');
  reduce(pp,2,decl_head,-1,34);
}
else if (cat1==exp && cat2!=lpar && cat2!=exp && cat2!=cast) {
  make_underlined(pp+1); squash(pp,2,decl_head,-1,35);
}
else if ((cat1==binop||cat1==colon) && cat2==exp && (cat3==comma ||
    cat3==semi || cat3==rpar))
  squash(pp,3,decl_head,-1,36);
else if (cat1==cast) squash(pp,2,decl_head,-1,37);
else if (cat1==lbrace || cat1==int_like || cat1==decl) {
  big_app1(pp);
  if (indent_param_decl) {
    big_app(indent); app(indent);
  }
  reduce(pp,1,fn_decl,0,38);
}
else if (cat1==semi) squash(pp,2,decl,-1,39);
@z

@x
@ @<Cases for |decl|@>=
if (cat1==decl) {
  big_app1(pp); big_app(force); big_app1(pp+1);
  reduce(pp,2,decl,-1,40);
}
else if (cat1==stmt || cat1==function) {
  big_app1(pp); big_app(big_force);
  big_app1(pp+1); reduce(pp,2,cat1,-1,41);
}
@y
@ The original manual described the \.{-o} option for \.{CWEAVE}, but this was
not yet present.  Here is a simple implementation.  The purpose is to suppress
the extra space between local variable declarations and the first statement in
a function block.

@d order_decl_stmt flags['o'] /* should declarations and statements be separated? */

@<Cases for |decl|@>=
if (cat1==decl) {
  big_app1(pp); big_app(force); big_app1(pp+1);
  reduce(pp,2,decl,-1,40);
}
else if (cat1==stmt || cat1==function) {
  big_app1(pp);
  if(order_decl_stmt) big_app(big_force);
  else big_app(force);
  big_app1(pp+1); reduce(pp,2,cat1,-1,41);
}
@z

@x
@ @<Cases for |fn_decl|@>=
if (cat1==decl) {
  big_app1(pp); big_app(force); big_app1(pp+1); reduce(pp,2,fn_decl,0,51);
}
else if (cat1==stmt) {
  big_app1(pp); app(outdent); app(outdent); big_app(force);
  big_app1(pp+1); reduce(pp,2,function,-1,52);
}
@y
@ Outdent after parameter declarations with option \.{-i}.

@<Cases for |fn_decl|@>=
if (cat1==decl) {
  big_app1(pp); big_app(force); big_app1(pp+1); reduce(pp,2,fn_decl,0,51);
}
else if (cat1==stmt) {
  big_app1(pp);
  if (indent_param_decl) {
    app(outdent); app(outdent);
  }
  big_app(force);
  big_app1(pp+1); reduce(pp,2,function,-1,52);
}
@z

@x
void
reduce(j,k,c,d,n)
scrap_pointer j;
eight_bits c;
short k, d, n;
@y
static void
reduce(
scrap_pointer j, short k,
eight_bits c,
short d, short n)
@z

@x
void
squash(j,k,c,d,n)
scrap_pointer j;
eight_bits c;
short k, d, n;
@y
static void
squash(
scrap_pointer j, short k,
eight_bits c,
short d, short n)
@z

@x
    overflow("token");
@y
    overflow(_("token"));
@z

@x
    overflow("text");
@y
    overflow(_("text"));
@z

@x
{ scrap_pointer k; /* pointer into |scrap_info| */
  if (tracing==2) {
    printf("\n%d:",n);
    for (k=scrap_base; k<=lo_ptr; k++) {
      if (k==pp) putxchar('*'); else putxchar(' ');
      if (k->mathness %4 ==  yes_math) putchar('+');
      else if (k->mathness %4 ==  no_math) putchar('-');
      print_cat(k->cat);
      if (k->mathness /4 ==  yes_math) putchar('+');
      else if (k->mathness /4 ==  no_math) putchar('-');
@y
{ scrap_pointer k_l; /* pointer into |scrap_info| */
  if (tracing==2) {
    printf("\n%d:",n);
    for (k_l=scrap_base; k_l<=lo_ptr; k_l++) {
      if (k_l==pp) putxchar('*'); else putxchar(' ');
      if (k_l->mathness %4 ==  yes_math) putchar('+');
      else if (k_l->mathness %4 ==  no_math) putchar('-');
      print_cat(k_l->cat);
      if (k_l->mathness /4 ==  yes_math) putchar('+');
      else if (k_l->mathness /4 ==  no_math) putchar('-');
@z

@x
text_pointer
translate() /* converts a sequence of scraps */
@y
static text_pointer
translate(void) /* converts a sequence of scraps */
@z

@x
    if (tok_ptr+6>tok_mem_end) overflow("token");
@y
    if (tok_ptr+6>tok_mem_end) overflow(_("token"));
@z

@x
  printf("\nIrreducible scrap sequence in section %d:",section_count);
@y
  printf(_("\nIrreducible scrap sequence in section %d:"),section_count);
@z

@x
  printf("\nTracing after l. %d:\n",cur_line); mark_harmless;
@y
  printf(_("\nTracing after l. %d:\n"),cur_line); mark_harmless;
@z

@x
void
C_parse(spec_ctrl) /* creates scraps from \CEE/ tokens */
  eight_bits spec_ctrl;
@y
static void
C_parse(@t\1\1@> /* creates scraps from \CEE/ tokens */
  eight_bits spec_ctrl@t\2\2@>)
@z

@x
switch (next_control) {
  case section_name:
    app(section_flag+(int)(cur_section-name_dir));
    app_scrap(section_scrap,maybe_math);
    app_scrap(exp,yes_math);@+break;
  case string: case constant: case verbatim: @<Append a string or constant@>;
   @+break;
  case identifier: app_cur_id(1);@+break;
  case TeX_string: @<Append a \TEX/ string, without forming a scrap@>;@+break;
  case '/': case '.':
    app(next_control); app_scrap(binop,yes_math);@+break;
  case '<': app_str("\\langle");@+app_scrap(prelangle,yes_math);@+break;
@.\\langle@>
  case '>': app_str("\\rangle");@+app_scrap(prerangle,yes_math);@+break;
@.\\rangle@>
  case '=': app_str("\\K"); app_scrap(binop,yes_math);@+break;
@.\\K@>
  case '|': app_str("\\OR"); app_scrap(binop,yes_math);@+break;
@.\\OR@>
  case '^': app_str("\\XOR"); app_scrap(binop,yes_math);@+break;
@.\\XOR@>
  case '%': app_str("\\MOD"); app_scrap(binop,yes_math);@+break;
@.\\MOD@>
  case '!': app_str("\\R"); app_scrap(unop,yes_math);@+break;
@.\\R@>
  case '~': app_str("\\CM"); app_scrap(unop,yes_math);@+break;
@.\\CM@>
  case '+': case '-': app(next_control); app_scrap(ubinop,yes_math);@+break;
  case '*': app(next_control); app_scrap(raw_ubin,yes_math);@+break;
  case '&': app_str("\\AND"); app_scrap(raw_ubin,yes_math);@+break;
@.\\AND@>
  case '?': app_str("\\?"); app_scrap(question,yes_math);@+break;
@.\\?@>
  case '#': app_str("\\#"); app_scrap(ubinop,yes_math);@+break;
@.\\\#@>
  case ignore: case xref_roman: case xref_wildcard:
  case xref_typewriter: case noop:@+break;
  case '(': case '[': app(next_control); app_scrap(lpar,maybe_math);@+break;
  case ')': case ']': app(next_control); app_scrap(rpar,maybe_math);@+break;
  case '{': app_str("\\{"@q}@>); app_scrap(lbrace,yes_math);@+break;
@.\\\{@>@q}@>
  case '}': app_str(@q{@>"\\}"); app_scrap(rbrace,yes_math);@+break;
@q{@>@.\\\}@>
  case ',': app(','); app_scrap(comma,yes_math);@+break;
  case ';': app(';'); app_scrap(semi,maybe_math);@+break;
  case ':': app(':'); app_scrap(colon,no_math);@+break;@/
  @t\4@>  @<Cases involving nonstandard characters@>@;
  case thin_space: app_str("\\,"); app_scrap(insert,maybe_math);@+break;
@.\\,@>
  case math_break: app(opt); app_str("0");
    app_scrap(insert,maybe_math);@+break;
  case line_break: app(force); app_scrap(insert,no_math);@+break;
  case left_preproc: app(force); app(preproc_line);
    app_str("\\#"); app_scrap(lproc,no_math);@+break;
@.\\\#@>
  case right_preproc: app(force); app_scrap(rproc,no_math);@+break;
  case big_line_break: app(big_force); app_scrap(insert,no_math);@+break;
  case no_line_break: app(big_cancel); app(noop); app(break_space);
    app(noop); app(big_cancel);
    app_scrap(insert,no_math);@+break;
  case pseudo_semi: app_scrap(semi,maybe_math);@+break;
  case macro_arg_open: app_scrap(begin_arg,maybe_math);@+break;
  case macro_arg_close: app_scrap(end_arg,maybe_math);@+break;
  case join: app_str("\\J"); app_scrap(insert,no_math);@+break;
@.\\J@>
  case output_defs_code: app(force); app_str("\\ATH"); app(force);
    app_scrap(insert,no_math);@+break;
@.\\ATH@>
  default: app(inserted); app(next_control);
    app_scrap(insert,maybe_math);@+break;
}
@y
switch (next_control) {
  case section_name:
    app(section_flag+(int)(cur_section-name_dir));
    app_scrap(section_scrap,maybe_math);
    app_scrap(exp,yes_math);@+break;
  case string: case constant: case verbatim:
    @<Append a string or constant@>;@+break;
  case identifier: app_cur_id(1);@+break;
  case TeX_string:
    @<Append a \TEX/ string, without forming a scrap@>;@+break;
  case '/': case '.':
    app(next_control);@+app_scrap(binop,yes_math);@+break;
  case '<': app_str("\\langle");@+app_scrap(prelangle,yes_math);@+break;
@.\\langle@>
  case '>': app_str("\\rangle");@+app_scrap(prerangle,yes_math);@+break;
@.\\rangle@>
  case '=': app_str("\\K");@+app_scrap(binop,yes_math);@+break;
@.\\K@>
  case '|': app_str("\\OR");@+app_scrap(binop,yes_math);@+break;
@.\\OR@>
  case '^': app_str("\\XOR");@+app_scrap(binop,yes_math);@+break;
@.\\XOR@>
  case '%': app_str("\\MOD");@+app_scrap(binop,yes_math);@+break;
@.\\MOD@>
  case '!': app_str("\\R");@+app_scrap(unop,yes_math);@+break;
@.\\R@>
  case '~': app_str("\\CM");@+app_scrap(unop,yes_math);@+break;
@.\\CM@>
  case '+': case '-': app(next_control);@+app_scrap(ubinop,yes_math);@+break;
  case '*': app(next_control);@+app_scrap(raw_ubin,yes_math);@+break;
  case '&': app_str("\\AND");@+app_scrap(raw_ubin,yes_math);@+break;
@.\\AND@>
  case '?': app_str("\\?");@+app_scrap(question,yes_math);@+break;
@.\\?@>
  case '#': app_str("\\#");@+app_scrap(ubinop,yes_math);@+break;
@.\\\#@>
  case ignore: case xref_roman: case xref_wildcard:
  case xref_typewriter: case noop:@+break;
  case '(': case '[': app(next_control);@+app_scrap(lpar,maybe_math);@+break;
  case ')': case ']': app(next_control);@+app_scrap(rpar,maybe_math);@+break;
  case '{': app_str("\\{"@q}@>);@+app_scrap(lbrace,yes_math);@+break;
@.\\\{@>@q}@>
  case '}': app_str(@q{@>"\\}");@+app_scrap(rbrace,yes_math);@+break;
@q{@>@.\\\}@>
  case ',': app(',');@+app_scrap(comma,yes_math);@+break;
  case ';': app(';');@+app_scrap(semi,maybe_math);@+break;
  case ':': app(':');@+app_scrap(colon,no_math);@+break;@/
  @t\4@>  @<Cases involving nonstandard characters@>@;
  case thin_space: app_str("\\,");@+app_scrap(insert,maybe_math);@+break;
@.\\,@>
  case math_break: app(opt);@+app_str("0");@+
    app_scrap(insert,maybe_math);@+break;
  case line_break: app(force);@+app_scrap(insert,no_math);@+break;
  case left_preproc: app(force);@+app(preproc_line);@+app_str("\\#");
    app_scrap(lproc,no_math);@+break;
@.\\\#@>
  case right_preproc: app(force);@+app_scrap(rproc,no_math);@+break;
  case big_line_break: app(big_force);@+app_scrap(insert,no_math);@+break;
  case no_line_break: app(big_cancel);@+app(noop);@+app(break_space);@+
    app(noop);@+app(big_cancel); app_scrap(insert,no_math);@+break;
  case pseudo_semi: app_scrap(semi,maybe_math);@+break;
  case macro_arg_open: app_scrap(begin_arg,maybe_math);@+break;
  case macro_arg_close: app_scrap(end_arg,maybe_math);@+break;
  case join: app_str("\\J");@+app_scrap(insert,no_math);@+break;
@.\\J@>
  case output_defs_code: app(force);@+app_str("\\ATH");@+app(force);
    app_scrap(insert,no_math);@+break;
@.\\ATH@>
  default: app(inserted);@+app(next_control);
    app_scrap(insert,maybe_math);@+break;
}
@z

@x
  overflow("scrap/token/text");
@y
  overflow(_("scrap/token/text"));
@z

@x
case not_eq: app_str("\\I");@+app_scrap(binop,yes_math);@+break;
@y
case non_eq: app_str("\\I");@+app_scrap(binop,yes_math);@+break;
@z

@x
        else err_print("! Double @@ should be used in strings");
@y
        else err_print(_("! Double @@ should be used in strings"));
@z

@x
void app_cur_id();
@y
void app_cur_id(boolean);@/
@z

@x
void
app_cur_id(scrapping)
boolean scrapping; /* are we making this into a scrap? */
@y
void
app_cur_id(@t\1\1@>
boolean scrapping@t\2\2@>) /* are we making this into a scrap? */
@z

@x
text_pointer
C_translate()
@y
static text_pointer
C_translate(void)
@z

@x
  if (next_control!='|') err_print("! Missing '|' after C text");
@y
  if (next_control!='|') err_print(_("! Missing '|' after C text"));
@z

@x
void
outer_parse() /* makes scraps from \CEE/ tokens and comments */
@y
static void
outer_parse(void) /* makes scraps from \CEE/ tokens and comments */
@z

@x
void
push_level(p) /* suspends the current level */
text_pointer p;
@y
static void
push_level(@t\1\1@> /* suspends the current level */
text_pointer p@t\2\2@>)
@z

@x
  if (stack_ptr==stack_end) overflow("stack");
@y
  if (stack_ptr==stack_end) overflow(_("stack"));
@z

@x
void
pop_level()
@y
static void
pop_level(void)
@z

@x
eight_bits
get_output() /* returns the next token of output */
@y
static eight_bits
get_output(void) /* returns the next token of output */
@z

@x
  return(a);
@y
  return((eight_bits)a);
@z

@x
void
output_C() /* outputs the current token list */
@y
static void
output_C(void) /* outputs the current token list */
@z

@x
void make_output();
@y
static void make_output(void);@/
@z

@x
void
make_output() /* outputs the equivalents of tokens */
{
  eight_bits a, /* current output byte */
@y
static void
make_output(void) /* outputs the equivalents of tokens */
{
  eight_bits a=0, /* current output byte */
@z

@x
  else if (b!='|') out(b)@;
@y
  else { if (b!='|') out(b)@;
@z

@x
  }
@y
  } }
@z

@x
  printf("\n! Illegal control code in section name: <");
@y
  fputs(_("\n! Illegal control code in section name: <"),stdout);
@z

@x
    printf("\n! C text in section name didn't end: <");
@y
    fputs(_("\n! C text in section name didn't end: <"),stdout);
@z

@x
    if (b=='\'' || b=='"')
      if (delim==0) delim=b;
      else if (delim==b) delim=0;
@y
    if (b=='\'' || b=='"') {
      if (delim==0) delim=b;
      else if (delim==b) delim=0; }
@z

@x
      if (j>buffer+long_buf_size-3) overflow("buffer");
@y
      if (j>buffer+long_buf_size-3) overflow(_("buffer"));
@z

@x
  if (j>buffer+long_buf_size-4) overflow("buffer");
@y
  if (j>buffer+long_buf_size-4) overflow(_("buffer"));
@z

@x
void phase_two();
@y
static void phase_two(void);@/
@z

@x
void
phase_two() {
@y
static void
phase_two(void) {
@z

@x
reset_input(); if (show_progress) printf("\nWriting the output file...");
@y
reset_input(); if (show_progress) fputs(_("\nWriting the output file..."),stdout);
@z

@x
        err_print("! TeX string should be in C text only"); break;
@y
        err_print(_("! TeX string should be in C text only")); break;
@z

@x
        err_print("! You can't do that in TeX text"); break;
@y
        err_print(_("! You can't do that in TeX text")); break;
@z

@x
void finish_C();
@y
static void finish_C(boolean);@/
@z

@x
void
finish_C(visible) /* finishes a definition or a \CEE/ part */
  boolean visible; /* nonzero if we should produce \TEX/ output */
@y
static void
finish_C(@t\1\1@> /* finishes a definition or a \Cee\ part */
  boolean visible@t\2\2@>) /* nonzero if we should produce \TeX\ output */
@z

@x
    if (out_ptr>out_buf+1)
      if (*(out_ptr-1)=='\\')
@.\\6@>
@.\\7@>
@.\\Y@>
        if (*out_ptr=='6') out_ptr-=2;
        else if (*out_ptr=='7') *out_ptr='Y';
@y
    if (out_ptr>out_buf+1) {
      if (*(out_ptr-1)=='\\') {
@.\\6@>
@.\\7@>
@.\\Y@>
        if (*out_ptr=='6') out_ptr-=2;
        else if (*out_ptr=='7') *out_ptr='Y';
      }
    }
@z

@x
    err_print("! Improper macro definition");
@y
    err_print(_("! Improper macro definition"));
@z

@x
      default: err_print("! Improper macro definition"); break;
@y
      default: err_print(_("! Improper macro definition")); break;
@z

@x
  if (scrap_ptr!=scrap_info+2) err_print("! Improper format definition");
@y
  if (scrap_ptr!=scrap_info+2) err_print(_("! Improper format definition"));
@z

@x
  err_print("! You need an = sign after the section name");
@y
  err_print(_("! You need an = sign after the section name"));
@z

@x
  err_print("! You can't do that in C text");
@y
  err_print(_("! You can't do that in C text"));
@z

@x
void footnote();
@y
static void footnote(sixteen_bits);@/
@z

@x
void
footnote(flag) /* outputs section cross-references */
sixteen_bits flag;
@y
static void
footnote(@t\1\1@> /* outputs section cross-references */
sixteen_bits flag@t\2\2@>)
@z

@x
void phase_three();
@y
static void phase_three(void);@/
@z

@x
void
phase_three() {
@y
static void
phase_three(void) {
@z

@x
if (no_xref) {
  finish_line();
  out_str("\\end");
@.\\end@>
  finish_line();
}
@y
if (no_xref) {
  finish_line();
  out_str("\\end");
@.\\end@>
  active_file=tex_file;
}
@z

@x
  phase=3; if (show_progress) printf("\nWriting the index...");
@y
  phase=3; if (show_progress) fputs(_("\nWriting the index..."),stdout);
@z

@x
  if ((idx_file=fopen(idx_file_name,"w"))==NULL)
@y
  if ((idx_file=fopen(idx_file_name,"wb"))==NULL)
@z

@x
    fatal("! Cannot open index file ",idx_file_name);
@y
    fatal(_("! Cannot open index file "),idx_file_name);
@z

@x
    @<Tell about changed sections@>; finish_line(); finish_line();
@y
    @<Tell about changed sections@>@; finish_line(); finish_line();
@z

@x
  @<Do the first pass of sorting@>;
  @<Sort and output the index@>;
@y
  @<Do the first pass of sorting@>@;
  @<Sort and output the index@>@;
@z

@x
  if ((scn_file=fopen(scn_file_name,"w"))==NULL)
@y
  if ((scn_file=fopen(scn_file_name,"wb"))==NULL)
@z

@x
    fatal("! Cannot open section file ",scn_file_name);
@y
    fatal(_("! Cannot open section file "),scn_file_name);
@z

@x
  @<Output all the section names@>;
@y
  @<Output all the section names@>@;
@z

@x
@.\\end@>
  finish_line();
  fclose(active_file);
}
@y
@.\\end@>
}
finish_line(); fclose(active_file); active_file=NULL;
@<Update the result when it has changed@>@;
@z

@x
if (show_happiness) printf("\nDone.");
@y
if (show_happiness) {
  if (show_progress) new_line;
  fputs(_("Done."),stdout);
}
@z

@x
for (c=0; c<=255; c++) bucket[c]=NULL;
@y
for (c=0; c<256; c++) bucket[c]=NULL;
@z

@x
    if (cur_name->xref!=(char*)xmem) {
@y
    if (cur_name->xref!=(void *)xmem) {
@z

@x
collate[0]=0;
strcpy(collate+1," \1\2\3\4\5\6\7\10\11\12\13\14\15\16\17");
/* 16 characters + 1 = 17 */
strcpy(collate+17,"\20\21\22\23\24\25\26\27\30\31\32\33\34\35\36\37");
/* 16 characters + 17 = 33 */
strcpy(collate+33,"!\42#$%&'()*+,-./:;<=>?@@[\\]^`{|}~_");
/* 32 characters + 33 = 65 */
strcpy(collate+65,"abcdefghijklmnopqrstuvwxyz0123456789");
/* (26 + 10) characters + 65 = 101 */
strcpy(collate+101,"\200\201\202\203\204\205\206\207\210\211\212\213\214\215\216\217");
/* 16 characters + 101 = 117 */
strcpy(collate+117,"\220\221\222\223\224\225\226\227\230\231\232\233\234\235\236\237");
/* 16 characters + 117 = 133 */
strcpy(collate+133,"\240\241\242\243\244\245\246\247\250\251\252\253\254\255\256\257");
/* 16 characters + 133 = 149 */
strcpy(collate+149,"\260\261\262\263\264\265\266\267\270\271\272\273\274\275\276\277");
/* 16 characters + 149 = 165 */
strcpy(collate+165,"\300\301\302\303\304\305\306\307\310\311\312\313\314\315\316\317");
/* 16 characters + 165 = 181 */
strcpy(collate+181,"\320\321\322\323\324\325\326\327\330\331\332\333\334\335\336\337");
/* 16 characters + 181 = 197 */
strcpy(collate+197,"\340\341\342\343\344\345\346\347\350\351\352\353\354\355\356\357");
/* 16 characters + 197 = 213 */
strcpy(collate+213,"\360\361\362\363\364\365\366\367\370\371\372\373\374\375\376\377");
/* 16 characters + 213 = 229 */
@y
collate[0]=0;
strcpy((char *)collate+1,
  " \1\2\3\4\5\6\7\10\11\12\13\14\15\16\17");
/* 16 characters + 1 = 17 */
strcpy((char *)collate+17,
  "\20\21\22\23\24\25\26\27\30\31\32\33\34\35\36\37");
/* 16 characters + 17 = 33 */
strcpy((char *)collate+33,
  "!\42#$%&'()*+,-./:;<=>?@@[\\]^`{|}~_");
/* 32 characters + 33 = 65 */
strcpy((char *)collate+65,
  "abcdefghijklmnopqrstuvwxyz0123456789");
/* (26 + 10) characters + 65 = 101 */
strcpy((char *)collate+101,
  "\200\201\202\203\204\205\206\207\210\211\212\213\214\215\216\217");
/* 16 characters + 101 = 117 */
strcpy((char *)collate+117,
  "\220\221\222\223\224\225\226\227\230\231\232\233\234\235\236\237");
/* 16 characters + 117 = 133 */
strcpy((char *)collate+133,
  "\240\241\242\243\244\245\246\247\250\251\252\253\254\255\256\257");
/* 16 characters + 133 = 149 */
strcpy((char *)collate+149,
  "\260\261\262\263\264\265\266\267\270\271\272\273\274\275\276\277");
/* 16 characters + 149 = 165 */
strcpy((char *)collate+165,
  "\300\301\302\303\304\305\306\307\310\311\312\313\314\315\316\317");
/* 16 characters + 165 = 181 */
strcpy((char *)collate+181,
  "\320\321\322\323\324\325\326\327\330\331\332\333\334\335\336\337");
/* 16 characters + 181 = 197 */
strcpy((char *)collate+197,
  "\340\341\342\343\344\345\346\347\350\351\352\353\354\355\356\357");
/* 16 characters + 197 = 213 */
strcpy((char *)collate+213,
  "\360\361\362\363\364\365\366\367\370\371\372\373\374\375\376\377");
/* 16 characters + 213 = 229 */
@z

@x
void  unbucket();
@y
static void unbucket(eight_bits);@/
@z

@x
void
unbucket(d) /* empties buckets having depth |d| */
eight_bits d;
@y
static void
unbucket(@t\1\1@> /* empties buckets having depth |d| */
eight_bits d@t\2\2@>)
@z

@x
    if (sort_ptr>=scrap_info_end) overflow("sorting");
@y
    if (sort_ptr>=scrap_info_end) overflow(_("sorting"));
@z

@x
void section_print();
@y
static void section_print(name_pointer);@/
@z

@x
void
section_print(p) /* print all section names in subtree |p| */
name_pointer p;
@y
static void
section_print(@t\1\1@> /* print all section names in subtree |p| */
name_pointer p@t\2\2@>)
@z

@x
@ @<Output all the section names@>=section_print(root)
@y
@ @<Output all the section names@>=section_print(root);
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
@.Memory usage statistics:@>
  printf("%ld names (out of %ld)\n",
            (long)(name_ptr-name_dir),(long)max_names);
  printf("%ld cross-references (out of %ld)\n",
            (long)(xref_ptr-xmem),(long)max_refs);
  printf("%ld bytes (out of %ld)\n",
            (long)(byte_ptr-byte_mem),(long)max_bytes);
  printf("Parsing:\n");
  printf("%ld scraps (out of %ld)\n",
            (long)(max_scr_ptr-scrap_info),(long)max_scraps);
  printf("%ld texts (out of %ld)\n",
            (long)(max_text_ptr-tok_start),(long)max_texts);
  printf("%ld tokens (out of %ld)\n",
            (long)(max_tok_ptr-tok_mem),(long)max_toks);
  printf("%ld levels (out of %ld)\n",
            (long)(max_stack_ptr-stack),(long)stack_size);
  printf("Sorting:\n");
  printf("%ld levels (out of %ld)\n",
            (long)(max_sort_ptr-scrap_info),(long)max_scraps);
}
@y
  puts(_("\nMemory usage statistics:"));
@.Memory usage statistics:@>
  printf(_("%ld names (out of %ld)\n"),
            (long)(name_ptr-name_dir),(long)max_names);
  printf(_("%ld cross-references (out of %ld)\n"),
            (long)(xref_ptr-xmem),(long)max_refs);
  printf(_("%ld bytes (out of %ld)\n"),
            (long)(byte_ptr-byte_mem),(long)max_bytes);
  puts(_("Parsing:"));
  printf(_("%ld scraps (out of %ld)\n"),
            (long)(max_scr_ptr-scrap_info),(long)max_scraps);
  printf(_("%ld texts (out of %ld)\n"),
            (long)(max_text_ptr-tok_start),(long)max_texts);
  printf(_("%ld tokens (out of %ld)\n"),
            (long)(max_tok_ptr-tok_mem),(long)max_toks);
  printf(_("%ld levels (out of %ld)\n"),
            (long)(max_stack_ptr-stack),(long)stack_size);
  puts(_("Sorting:"));
  printf(_("%ld levels (out of %ld)\n"),
            (long)(max_sort_ptr-scrap_info),(long)max_scraps);
}
@z

@x
@** Index.
@y
@** Extensions for modern {\tt CWEB}.

The following sections introduce code changes and extensions that have been
created by numerous contributors over the course of a quarter century. They
make \.{CWEB} adhere to modern coding standards and introduce new or improved
features.

Care has been taken to keep the original section numbering intact, so this new
section should have the same number as the original ``\&{250.~Index},'' and
additional material follows below.

@* Set {\tt CWEAVE} flags.
At least one of these is already used in \.{COMMON}.

@<Set init...@>=
  make_xrefs=force_lines=make_pb=indent_param_decl=order_decl_stmt=1;
    /* controlled by command-line options */

@* Function declarations.  Here are declarations---conforming to
{\mc ANSI~C}---of all functions in this code, as far as they are
not already in |"common.h"|.  These are private to \.{CWEAVE}.

@<Predecl...@>=
static eight_bits copy_TeX(void);@/
static eight_bits get_output(void);@/
static eight_bits skip_TeX(void);@/
static text_pointer C_translate(void);@/
static text_pointer translate(void);@/
static token_pointer find_first_ident(text_pointer);@/
static void app_str(const char *);@/
static void big_app(token);@/
static void big_app1(scrap_pointer);@/
static void copy_limbo(void);@/
static void C_parse(eight_bits);@/
static void finish_line(void);@/
static void flush_buffer(char *,boolean,boolean);@/
static void make_reserved(scrap_pointer);@/
static void make_underlined(scrap_pointer);@/
static void new_section_xref(name_pointer);@/
static void new_xref(name_pointer);@/
static void outer_parse(void);@/
static void output_C(void);@/
static void out_name(name_pointer,boolean);@/
static void out_section(sixteen_bits);@/
static void out_str(const char *);@/
static void pop_level(void);@/
static void print_cat(eight_bits);@/
#ifdef DEAD_CODE
static void print_text(text_pointer p);@/
#endif
static void push_level(text_pointer);@/
static void reduce(scrap_pointer,short,eight_bits,short,short);@/
static void set_file_flag(name_pointer);@/
static void skip_limbo(void);@/
static void squash(scrap_pointer,short,eight_bits,short,short);@/
static void update_node(name_pointer p);@/

@* Language setting.  This global variable is defined and set in \.{COMMON} by
the `\.{+l}' (or `\.{-l}') command-line option.

@<Global var...@>=
extern const char *use_language; /* prefix to \.{cwebmac.tex} in \TEX/ output */

@* Output file update.  Most \CEE/ projects are controlled by a
\.{Makefile} that automatically takes care of the temporal dependecies
between the different source modules.  It is suitable that \.{CWEB} doesn't
create new output for all existing files, when there are only changes to
some of them. Thus the \.{make} process will only recompile those modules
where necessary. The idea and basic implementation of this mechanism can
be found in the program \.{NUWEB} by Preston Briggs, to whom credit is due.

@<Update the result...@>=
if((tex_file=fopen(tex_file_name,"r"))!=NULL) {
  char x[BUFSIZ],y[BUFSIZ];
  int x_size,y_size,comparison=false;

  if((check_file=fopen(check_file_name,"r"))==NULL)
    fatal(_("! Cannot open output file "),check_file_name);
@.Cannot open output file@>

  if (temporary_output) @<Compare the temporary output...@>@;

  fclose(tex_file); tex_file=NULL;
  fclose(check_file); check_file=NULL;

  @<Take appropriate action depending on the comparison@>@;
} else
  rename(check_file_name,tex_file_name); /* This was the first run */

strcpy(check_file_name,""); /* We want to get rid of the temporary file */

@ We hope that this runs fast on most systems.

@<Compare the temporary output to the previous output@>=
do {
  x_size = fread(x,1,BUFSIZ,tex_file);
  y_size = fread(y,1,BUFSIZ,check_file);
  comparison = (x_size == y_size); /* Do not merge these statements! */
  if(comparison) comparison = !memcmp(x,y,x_size);
} while(comparison && !feof(tex_file) && !feof(check_file));

@ Note the superfluous call to |remove| before |rename|.  We're using it to
get around a bug in some implementations of |rename|.

@<Take appropriate action...@>=
if(comparison)
  remove(check_file_name); /* The output remains untouched */
else {
  remove(tex_file_name);
  rename(check_file_name,tex_file_name);
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

