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
% This file is part of CWEB.
% This program by Silvio Levy and Donald E. Knuth
% is based on a program by Knuth.
% It is distributed WITHOUT ANY WARRANTY, express or implied.
@y
% This program by Don Knuth is based on CWEAVE by Levy and Knuth.
% It's somewhat flaky, so you probably shouldn't try to use it.
@z

@x
% (essentially the same as version 3.6, which added
%  recently introduced features of standard C++ to version 3.4)
% (In November 2016 I made minor adjustments but changed no code -- DEK)
@y
@z

@x
% Here is TeX material that gets inserted after \input cwebmac
@y
% Here is TeX material that gets inserted after \input ctwimac
\def\contentspagenumber{0} % default page number for table of contents
@z

@x
\def\title{CWEAVE (Version 3.64)}
@y
\def\title{CTWILL (Version 3.64 [\TeX~Live])}
@z

@x
  \centerline{\titlefont The {\ttitlefont CWEAVE} processor}
@y
  \centerline{\titlefont The {\ttitlefont CTWILL} processor}
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
This is the \.{CWEAVE} program by Silvio Levy and Donald E. Knuth,
based on \.{WEAVE} by Knuth.
We are thankful to Steve Avery,
Nelson Beebe, Hans-Hermann Bode (to whom the original \CPLUSPLUS/ adaptation
is due), Klaus Guntermann, Norman Ramsey, Tomas Rokicki, Joachim Schnitter,
Joachim Schrod, Lee Wittenberg, Saroj Mahapatra, Cesar Augusto Rorato
Crusius, and others who have contributed improvements.

The ``banner line'' defined here should be changed whenever \.{CWEAVE}
is modified.

@d banner "This is CWEAVE (Version 3.64)\n"
@y
This is the \.{CTWILL} program by D. E. Knuth, based
on \.{CWEAVE} by Silvio Levy and D.~E. Knuth. It is also based on
\.{TWILL}, a private \.{WEB} program that Knuth wrote to produce
Volumes B and~D of {\sl Computers {\char`\&} Typesetting\/} in 1985.
\.{CTWILL} was hacked together hastily in June, 1992, to generate pages for
Knuth's book about the Stanford GraphBase, and updated even more hastily
in March, 1993 to generate final copy for that book.  The main idea was to
extend \.{CWEAVE} so that ``mini-indexes'' could appear.
No time was available to make \.{CTWILL} into a refined or complete system,
nor even to fully update the program documentation below. Subsequent changes
were made only to maintain compatibility with \.{CWEAVE}. Further information
can be found in Knuth's article ``Mini-indexes for literate programs,''
reprinted in {\sl Digital Typography\/} (1999), 225--245.

The ``banner line'' defined here should be changed whenever \.{CTWILL} is
modified. The version number parallels the corresponding version of \.{CWEAVE}.

@d banner "This is CTWILL, Version 3.64"
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

@ Here is a sort of user manual for \.{CTWILL}---which is exactly like
\.{CWEAVE} except that it produces much better documentation, for which you
must work harder. As with \.{CWEAVE}, input comes from a source file
\.{foo.w} and from an optional (but now almost mandatory) change file
\.{foo.ch}; output goes to \.{foo.tex}, \.{foo.idx}, and \.{foo.scn}.
Unlike \.{CWEAVE}, there is an additional output file, \.{foo.aux},
which records all nonexternal definitions.  The \.{.aux} file also
serves as an input file on subsequent runs. You should run \.{CTWILL}
twice, once to prime the pump and once to get decent answers.

Moreover, you must run the output twice through \TeX. (This double duplicity
suggested the original name \.{TWILL}.) After `\.{tex} \.{foo}' you
will have output that looks like final pages except that the entries
of mini-indexes won't be alphabetized. \TeX\ will say `This is the first
pass', and it will produce a weird file called \.{foo.ref}. Say
$$\.{refsort < foo.ref > foo.sref}$$
and then another `\.{tex} \.{foo}' will produce alphabetized output.
While \TeX\ runs it emits messages filled with numeric data, indicating how
much space is consumed by each program section. If you can decipher these
numbers (see \.{ctwimac.tex}), you can use them to fine-tune the page
layout. You might be tempted to do fine tuning by editing \.{foo.tex}
directly, but it's better to incorporate all changes into \.{foo.ch}.

The mini-indexes list identifiers that are used but not defined on
each two-page spread. At the end of each section, \.{CTWILL} gives
\TeX\ a list of identifiers used in that section and information
about where they are defined. The macros in \.{ctwimac.tex} figure out
which identifiers should go in each mini-index, based on how the pages
break. (Yes, those macros are pretty hairy.)

The information that \.{CTWILL} concocts from \.{foo.w} is not always
correct. Sometimes you'll use an identifier that you don't want
indexed; for example, your exposition might talk about |f(x)| when you
don't mean to refer to program variables |f| or |x|. Sometimes you'll
use an identifier that's defined in a header file, unknown to
\.{CTWILL}. Sometimes you'll define a single identifier in several
different places, and \.{CTWILL} won't know which definition to choose.
But all is not lost. \.{CTWILL} guesses right most of the time, and you can
give it the necessary hints in other places via your change file.

If you think it's easy to write a completely automatic system that doesn't
make \.{CTWILL}'s mistakes and doesn't depend so much on change files,
please do so.

\.{CTWILL} uses a very simple method to generate mini-index info. By
understanding this method, you will understand how to fix it when things
go wrong. Every identifier has a current ``meaning,'' consisting of its
abstract type and the number of the section in which it was most recently
defined. For example, if your \Cee\ program says `|char *s|' in section~3,
the meaning of~|s| gets changed to `\&{char} $*$, \S3' while \.{CTWILL}
is processing that section. If you refer to~|s| in section~10, and if
|s|~hasn't been redefined in the meantime, and if section~10 doesn't
wind up on the same two-page spread as section~3, the mini-index generated
by section~10 will say ``|s|: \&{char}~$*$, \S3.''

The current meaning of every identifier is initially `\.{\\uninitialized}'.
Then \.{CTWILL} reads the \.{.aux} file for your job, if any; this
\.{.aux} file contains all definitions of new meanings in the previous
run, so it tells \.{CTWILL} about definitions that will be occurring
in the future. If all identifiers have a unique definition, they will
have a unique and appropriate meaning in the mini-indexes.

But some identifiers, like parameters to procedures, may be defined
several times. Others may not be defined at all, because they are
defined elsewhere and mentioned in header files included by the \Cee\
preprocessor. To solve this problem, \.{CTWILL} provides mechanisms by which
the current meaning of an identifier can be temporarily or permanently
changed.

For example, the operation
$$\.{@@\$s \{FOO\}3 \\\&\{char\} \$*\$@@>}$$
changes the current meaning of |s| to the \TeX\ output of `\.{\\\&\{char\}}
\.{\$*\$}' in section~3 of program {\sc FOO}. All entries in the \.{.aux}
file are expressed in the form of this \.{@@\$} operator; therefore you
can use a text editor to paste such entries into a \.{.ch} file, whenever
you want to tell \.{CTWILL} about definitions that are out of order
or from other programs.

Before reading the \.{.aux} file, \.{CTWILL} actually looks for a file
called \.{system.bux}, which will be read if present. And after
\.{foo.aux}, a third possibility is \.{foo.bux}. The general
convention is to put definitions of system procedures such as |printf|
into \.{system.bux}, and to put definitions found in specifically
foo-ish header files into \.{foo.bux}. Like the \.{.aux}
files, \.{.bux} files should contain only \.{@@\$} specifications;
this rule corresponds to the fact that `bux' is the plural of `\$'.
The \.{.bux} files may also contain \.{@@i} includes.

A companion operation \.{@@\%} signifies that all \.{@@\$}
specifications from the present point to the beginning of the next
section will define {\it temporary\/} meanings instead of permanent
ones. Temporary meanings are placed into the
mini-index of the current section; the permanent (current) meaning of
the identifier will not be changed, nor will it appear in the
mini-index of the section. If several temporary meanings are assigned
to the same identifier in a section, all will appear in the mini-index.
Each \.{@@\%} toggles the temporary/permanent convention; thus, after
an even number of \.{@@\%} operations in a section, meanings specified
by \.{@@\$} are permanent.

The operation \.{@@-} followed by an identifier followed by \.{@@>}
specifies that the identifier should not generate a mini-index entry
in the current section (unless, of course, a temporary meaning is assigned).

If \.{@@-foo@@>} appears in a section where a new permanent meaning is
later defined by the semantics of~\Cee, the current meaning of \\{foo}
will not be redefined; moreover, this current meaning, which may have
been changed by \.{@@\$foo ...@@>}, will also be written to the
\.{.aux} file. Therefore you can control what \.{CTWILL} outputs; you
can keep it from repeatedly contaminating the \.{.aux} file with
things you don't like.

The meaning specified by \.{@@\$...@@>} generally has four components:
an identifier (followed by space), a program name (enclosed in braces),
a section number (followed by space), and a \TeX\ part. The \TeX\ part
must have fewer than 50 characters. If the \TeX\ part starts
with `\.=', the mini-index entry will contain an equals sign instead
of a colon; for example,
$$\.{@@\$buf\_size \{PROG\}10 =\\T\{200\}@@>}$$
generates either `$\\{buf\_size}=200$, \S10' or
`$\\{buf\_size}=200$, {\sc PROG}\S10', depending on whether
`{\sc PROG}' is or isn't the title of the current program. If the
\TeX\ part is `\.{\\zip}', the mini-index entry will contain neither
colon nor equals, just a comma. The program name and section number
can also be replaced by a string. For example,
$$\.{@@\$printf "<stdio.h>" \\zip@@>}$$
will generate a mini-index entry like `\\{printf}, \.{<stdio.h>}.'.

\vfill\eject

A special ``proofmode'' is provided so that you can check \.{CTWILL}'s
conclusions about cross-references. Run \.{CTWILL} with the
flag \.{+P}, and \TeX\ will produce a specially formatted document
({\it without\/} mini-indexes)
in which you can check that your specifications are correct.
You should always do this before generating mini-indexes, because
mini-indexes can mask errors if page breaks are favorable but the
errors might reveal themselves later after your program has changed.
The proofmode output is much easier to check than the mini-indexes themselves.

The control code \.{@@r} or \.{@@R} causes \.{CTWILL} to emit
the \TeX\ macro `\.{\\shortpage}' just before starting the next
section of the program. This causes the section to appear at the top of
a right-hand page, if it would ordinarily have appeared near the bottom
of a left-hand page and split across the pages. (The \.{\\shortpage} macro
is fragile and should be used only in cases where it will not mess up
the output; insert it only when fine-tuning a set of pages.) If the
next section is a starred section, the behavior is slightly different
(but still fragile): The starred section will either be postponed to
a left-hand page, if it normally would begin on a right-hand page,
or vice versa. In other words, \.{@@r@@*} inverts the left/right logic.

\.{CTANGLE} does not recognize the operations \.{@@\$}, \.{@@\%}, \.{@@-},
and \.{@@r}, which are unique to \.{CTWILL}. But that is no problem,
since you use them only in change files set up for book publishing,
which are quite different from the change files you set up for tangling.

(End of user manual. We now resume the program for \.{CWEAVE}, with occasional
outbursts of new code.)

@d max_tex_chars 50 /* limit on the \TeX\ part of a meaning */
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
  argc=ac; argv=av;
@y
  extern const char *use_language; /* prefix to \.{cwebmac.tex} in \TEX/ output */
  argc=ac; argv=av;
@z

@x
  program=cweave;
@y
  program=ctwill;
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
@y
@d max_scraps 10000 /* number of tokens in \CEE/ texts being parsed */
@z

@x
@i common.h
@y
@i comm-w2c.h
@z

@x
turned on during the first phase.

@<Global...@>=
boolean change_exists; /* has any section changed? */
@y
turned on during the first phase---NOT!
@z

@x
sixteen_bits xref_switch,section_xref_switch; /* either zero or |def_flag| */
@y
sixteen_bits xref_switch,section_xref_switch; /* either zero or |def_flag| */

@ \.{CTWILL} also has special data structures to keep track of current
and temporary meanings. These structures were not designed for maximum
efficiency; they were designed to be easily grafted into \.{CWEAVE}'s
existing code without major surgery.

@d max_meanings 100 /* max temporary meanings per section */
@d max_titles 100 /* max distinct program or header names in meanings */

@<Type...@>=
typedef struct {
  name_pointer id; /* identifier whose meaning is being recorded */
  sixteen_bits prog_no; /* title of program or header in which defined */
  sixteen_bits sec_no; /* section number in which defined */
  char tex_part[max_tex_chars]; /* \TeX\ part of meaning */
} meaning_struct;

@ @<Glob...@>=
struct perm_meaning {
  meaning_struct perm; /* current meaning of an identifier */
  int stamp; /* last section number in which further output suppressed */
  struct perm_meaning *link; /* another meaning to output in this section */
} cur_meaning[max_names]; /* the current ``permanent'' meanings */
struct perm_meaning *top_usage; /* first meaning to output in this section */
meaning_struct temp_meaning_stack[max_meanings]; /* the current ``temporary'' meanings */
meaning_struct *temp_meaning_ptr; /* first available slot in |temp_meaning_stack| */
meaning_struct *max_temp_meaning_ptr; /* its maximum value so far */
name_pointer title_code[max_titles]; /* program names seen so far */
name_pointer *title_code_ptr; /* first available slot in |title_code| */
char ministring_buf[max_tex_chars]; /* \TeX\ code being generated */
char *ministring_ptr; /* first available slot in |ministring_buf| */
boolean ms_mode; /* are we outputting to |ministring_buf|? */

@ @<Set init...@>=
max_temp_meaning_ptr=temp_meaning_stack;
title_code_ptr=title_code;
ms_mode=0;

@ Here's a routine that converts a program title from the buffer
into an internal number for the |prog_no| field of a meaning.
It advances |loc| past the title found.

@c static sixteen_bits title_lookup(void)
{
  char *first,*last; /* boundaries */
  int balance; /* excess of left over right */
  register name_pointer *p;
  first=loc;
  if (*loc=='"') {
    while (++loc<=limit && *loc!='"') if (*loc=='\\') loc++;
  } else if (*loc=='{') {
    balance=1;
    while (++loc<=limit) {
      if (*loc=='}' && --balance==0) break;
      if (*loc=='{') balance++;
    }
  } else err_print(_("! Title should be enclosed in braces or doublequotes"));
  last=++loc;
  if (last>limit) err_print(_("! Title name didn't end"));
  if (title_code_ptr==&title_code[max_titles]) overflow(_("titles"));
  *title_code_ptr=id_lookup(first,last,title);
  for (p=title_code;;p++) if (*p==*title_code_ptr) break;
  if (p==title_code_ptr) title_code_ptr++;
  return p-title_code;
}

@ @<Give a default title to the program, if necessary@>=
if (title_code_ptr==title_code) { /* no \.{\\def\\title} found in limbo */
  char *saveloc=loc,*savelimit=limit;
  loc=limit+1; limit=loc;
  *limit++='{';
  strncpy(limit,tex_file_name,strlen(tex_file_name)-4);
  limit+=strlen(tex_file_name)-4;
  *limit++='}';
  title_lookup();
  loc=saveloc; limit=savelimit;
}

@ The |new_meaning| routine changes the current ``permanent meaning''
when an identifier is redeclared. It gets the |tex_part| from
|ministring_buf|.

@c
static void
new_meaning(
  name_pointer p)
{
  struct perm_meaning *q=p-name_dir+cur_meaning;
  ms_mode=0;
  if (q->stamp!=section_count) {
    if (*(ministring_ptr-1)==' ') ministring_ptr--;
    if (ministring_ptr>=&ministring_buf[max_tex_chars])
      strcpy(ministring_buf,"\\zip"); /* ignore |tex_part| if too long */
@.\\zip@>
    else *ministring_ptr='\0';
    q->perm.prog_no=0; /* |q->perm.id=p| */
    q->perm.sec_no=section_count;
    strcpy(q->perm.tex_part,ministring_buf);
  }
  @<Write the new meaning to the \.{.aux} file@>;
}

@ @<Write the new meaning to the \.{.aux} file@>=
{@+int n=q->perm.prog_no;
  fprintf(aux_file,"@@$%.*s %.*s",@|
     (int)((p+1)->byte_start-p->byte_start),p->byte_start,@|
      (int)((title_code[n]+1)->byte_start-title_code[n]->byte_start),
         title_code[n]->byte_start);
  if (*(title_code[n]->byte_start)=='{') fprintf(aux_file,"%d",q->perm.sec_no);
  fprintf(aux_file," %s@@>\n",q->perm.tex_part);
}
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
  struct perm_meaning *q=p-name_dir+cur_meaning;
  p->ilk=t; init_node(p);
  q->stamp=0;
  q->link=NULL;
  q->perm.id=p;
  q->perm.prog_no=q->perm.sec_no=0;
  strcpy(q->perm.tex_part,"\\uninitialized");
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
id_lookup("extern",NULL,int_like);
@y
ext_loc=id_lookup("extern",NULL,int_like)-name_dir;
@z

@x
id_lookup("int",NULL,raw_int);
@y
int_loc=id_lookup("int",NULL,raw_int)-name_dir;
@z

@x
id_lookup("make_pair",NULL,func_template);
@y
id_lookup("make_pair",NULL,func_template);

@ @<Glob...@>=
sixteen_bits int_loc, ext_loc; /* locations of special reserved words */
@z

@x
@d ord 0207 /* control code for `\.{@@'}' */
@d join 0210 /* control code for `\.{@@\&}' */
@d thin_space 0211 /* control code for `\.{@@,}' */
@d math_break 0212 /* control code for `\.{@@\v}' */
@d line_break 0213 /* control code for `\.{@@/}' */
@d big_line_break 0214 /* control code for `\.{@@\#}' */
@d no_line_break 0215 /* control code for `\.{@@+}' */
@d pseudo_semi 0216 /* control code for `\.{@@;}' */
@d macro_arg_open 0220 /* control code for `\.{@@[}' */
@d macro_arg_close 0221 /* control code for `\.{@@]}' */
@d trace 0222 /* control code for `\.{@@0}', `\.{@@1}' and `\.{@@2}' */
@d translit_code 0223 /* control code for `\.{@@l}' */
@d output_defs_code 0224 /* control code for `\.{@@h}' */
@d format_code 0225 /* control code for `\.{@@f}' and `\.{@@s}' */
@d definition 0226 /* control code for `\.{@@d}' */
@d begin_C 0227 /* control code for `\.{@@c}' */
@d section_name 0230 /* control code for `\.{@@<}' */
@d new_section 0231 /* control code for `\.{@@\ }' and `\.{@@*}' */
@y
@d meaning 0207 /* control code for `\.{@@\$}' */
@d suppress 0210 /* control code for `\.{@@-}' */
@d temp_meaning 0211 /* control code for `\.{@@\%}' */
@d right_start 0212 /* control code for `\.{@@r}' */
@d ord 0213 /* control code for `\.{@@'}' */
@d join 0214 /* control code for `\.{@@\&}' */
@d thin_space 0215 /* control code for `\.{@@,}' */
@d math_break 0216 /* control code for `\.{@@\v}' */
@d line_break 0217 /* control code for `\.{@@/}' */
@d big_line_break 0220 /* control code for `\.{@@\#}' */
@d no_line_break 0221 /* control code for `\.{@@+}' */
@d pseudo_semi 0222 /* control code for `\.{@@;}' */
@d macro_arg_open 0224 /* control code for `\.{@@[}' */
@d macro_arg_close 0225 /* control code for `\.{@@]}' */
@d trace 0226 /* control code for `\.{@@0}', `\.{@@1}' and `\.{@@2}' */
@d translit_code 0227 /* control code for `\.{@@l}' */
@d output_defs_code 0230 /* control code for `\.{@@h}' */
@d format_code 0231 /* control code for `\.{@@f}' and `\.{@@s}' */
@d definition 0232 /* control code for `\.{@@d}' */
@d begin_C 0233 /* control code for `\.{@@c}' */
@d section_name 0234 /* control code for `\.{@@<}' */
@d new_section 0235 /* control code for `\.{@@\ }' and `\.{@@*}' */
@z

@x
ccode['\'']=ord;
@y
ccode['\'']=ord;
ccode['$']=meaning; ccode['%']=temp_meaning; ccode['-']=suppress;
ccode['r']=ccode['R']=right_start;
@z

@x
void   skip_limbo();

@ @c
@y
static void skip_limbo(void);@/

@ We look for a clue about the program's title, because this will become
part of all meanings.

@c
@z

@x
void
skip_limbo() {
@y
static void
skip_limbo(void) {
@z

@x
    if (loc>limit && get_line()==0) return;
@y
    if (loc>limit && get_line()==0) return;
    if (loc==buffer && strncmp(buffer,"\\def\\title{",11)==0) {
      loc=buffer+10;
      title_lookup(); /* this program's title will be code zero */
    }
@z

@x
unsigned
skip_TeX() /* skip past pure \TEX/ code */
@y
static eight_bits
skip_TeX(void)
@z

@x
\yskip\hang |xref_roman|, |xref_wildcard|, |xref_typewriter|, |TeX_string|,
@y
\yskip\hang |xref_roman|, |xref_wildcard|, |xref_typewriter|, |TeX_string|,
|meaning|, |suppress|,
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
@d right_preproc 0217 /* ends a preprocessor command */
@y
@d right_preproc 0223 /* ends a preprocessor command */
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
    case underline: xref_switch=def_flag; continue;
@y
    case underline: xref_switch=def_flag; continue;
    case temp_meaning: temp_switch=1-temp_switch; continue;
    case right_start: right_start_switch=1; continue;
@z

@x
    case xref_roman: case xref_wildcard: case xref_typewriter:
    case noop: case TeX_string: c=ccode[c]; skip_restricted(); return(c);
@y
    case xref_roman: case xref_wildcard: case xref_typewriter: case noop:
    case meaning: case suppress:
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
{
@y
void
skip_restricted(void)
{ int c=ccode[(eight_bits)*(loc-1)];
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
@.Control codes are forbidden...@>
@y
@.Control codes are forbidden...@>
    if (c==meaning && phase==2) @<Process a user-generated meaning@>@;
    else if (c==suppress && phase==2) @<Suppress mini-index entry@>;
  }
}

@ @<Suppress mini-index entry@>=
{ char *first=id_first,*last=id_loc;
  while (xisspace(*first)) first++;
  while (xisspace(*(last-1))) last--;
  if (first<last) {
    struct perm_meaning *q=id_lookup(first,last,normal)-name_dir+cur_meaning;
    q->stamp=section_count; /* this is what actually suppresses output */
  }
}

@ @<Process a user-generated meaning@>=
{ char *first=id_first;
  while (xisspace(*first)) first++;
  loc=first;
  while (xisalpha(*loc)||xisdigit(*loc)||*loc=='_') loc++;
  if (*loc++!=' ')
    err_print(_("! Identifier in meaning should be followed by space"));
  else { name_pointer p=id_lookup(first,loc-1,normal);
    sixteen_bits t; int n=0;
    t=title_lookup();
    if (*(loc-1)=='}')
      while (xisdigit(*loc)) n=10*n+(*loc++)-'0';
    if (*loc++!=' ')
      err_print(_("! Location in meaning should be followed by space"));
    else @<Digest the meaning of |p|, |t|, |n|@>;
  }
  loc=id_loc+2;
}

@ @<Digest...@>=
{ meaning_struct *m;
  struct perm_meaning *q=p-name_dir+cur_meaning;
  if (temp_switch) {
    m=temp_meaning_ptr++;
    if (temp_meaning_ptr>max_temp_meaning_ptr) {
      if (temp_meaning_ptr>&temp_meaning_stack[max_meanings])
        overflow(_("temp meanings"));
      max_temp_meaning_ptr=temp_meaning_ptr;
    }
  } else m=&(q->perm);
  m->id=p;
  m->prog_no=t;
  m->sec_no=n;
  if (id_loc-loc>=max_tex_chars) strcpy(m->tex_part,"\\zip");
@.\\zip@>
  else { char *q=m->tex_part;
    while (loc<id_loc) *q++=*loc++;
    *q='\0';
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
  skip_limbo(); change_exists=0;
@y
  skip_limbo();
  @<Give a default title to the program, if necessary@>;
@z

@x
  changed_section[section_count]=change_exists;
    /* the index changes if anything does */
@y
@z

@x
  if (++section_count==max_sections) overflow("section number");
@y
  if (++section_count==max_sections) overflow(_("section number"));
@z

@x
  changed_section[section_count]=changing;
     /* it will become 1 if any line changes */
@y
@z

@x
  if (changed_section[section_count]) change_exists=1;
@y
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
    case xref_roman: case xref_wildcard: case xref_typewriter:
@y
    case xref_roman: case xref_wildcard: case xref_typewriter:
    case meaning: case suppress:
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
\.{CTWILL} with `\.{+lX}' (or `\.{-lX}' as well), where `\.X' is the
(possibly empty) string of characters to the right of~`\.l', `\.X'~will
be prepended to `\.{ctwimac.tex}', e.g., if you call \.{CTWILL} with
`\.{+ldeutsch}', you will receive the line `\.{\\input deutschctwimac}'.
Without this option the first line of the output file will be
`\.{\\input ctwimac}'. Or, if the user has specified proofing by
saying \.{+P} on the command line, it's `\.{\\input ctproofmac}' (resp.\
\.{\\input Xctproofmac} with option \.{+lX}), a set of macros used when
debugging mini-index entries.

@d proofing flags['P']

@<Start \TEX/...@>=
out_ptr=out_buf+1; out_line=1; active_file=tex_file; *out_ptr='c';
tex_puts("\\input ");
tex_printf(use_language);
tex_puts(proofing?"ctproofma":"ctwima");
@z

@x
@d out(c) {if (out_ptr>=out_buf_end) break_out(); *(++out_ptr)=c;}
@y
@d out(c)
 {if (ms_mode) { /* outputting to |ministring_buf| */
    if (ministring_ptr<&ministring_buf[max_tex_chars])
      *ministring_ptr++=c;
  } else {
     if (out_ptr>=out_buf_end) break_out();
     *(++out_ptr)=c;
   }
 }
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
|def_flag|, so it cannot have more than five decimal digits.  If
the section is changed, we output `\.{\\*}' just after the number.
@y
|def_flag|, so it cannot have more than five decimal digits.
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
  if(changed_section[n]) out_str ("\\*");
@.\\*@>
@y
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
        case right_start: right_start_switch=1; break;
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
@d end_arg 62 /* \.{@@]} */
@y
@d end_arg 62 /* \.{@@]} */
@d title 63 /* program name or header name in a ``meaning'' */
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
@i prod.w
@y
@i prod-twill.w
@z

@x
scrap scrap_info[max_scraps]; /* memory array for scraps */
@y
scrap scrap_info[max_scraps]; /* memory array for scraps */
scrap null_scrap; /* a scrap with empty translation */
@z

@x
@ @<Set init...@>=
@y
@ @<Set init...@>=
null_scrap.trans=&tok_start[0];
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
  printf("|\n"); update_terminal;
}
@#
static void pr_txt(
  int k)
{ print_text(&tok_start[k]); }
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
the |for| loop below.

@c
@y
the |for| loop below.

We use the fact that |make_underlined| has been called immediately preceding
|make_reserved|, hence |tok_loc| has been set.

@c
static token_pointer tok_loc; /* where the first identifier appears */
@#
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
  token_pointer tok_loc; /* pointer to |tok_value| */
  if ((tok_loc=find_first_ident(p->trans))<=operator_found)
    return; /* this should not happen */
@y
  if (tok_loc<=operator_found) return; /* this should not happen */
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
  token_pointer tok_loc; /* where the first identifier appears */
@y
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
  r->num=m; /* everything from |q| on is left undisturbed */
@y
  r->num=m; /* everything from |q| on is left undisturbed */

@ \.{CTWILL} needs the following procedure, which appends tokens of a
translated text until coming to |tok_loc|, then suppresses text that may
appear between parentheses or brackets. The calling routine should set
|ident_seen=0| first. (This is admittedly tricky.)

@c boolean ident_seen;
static boolean app_supp(
  text_pointer p)
{ token_pointer j;
  text_pointer q;
  if (ident_seen && **p>=tok_flag) {
    q=**p-tok_flag+tok_start;
    if (**q=='(') {
      app('(');@+app('\\');@+app(',');@+app(')'); goto catch14;
    }
    if (**q=='[') {
      app('[');@+app('\\');@+app(',');@+app(']'); goto catch14;
    }
  }
  for (j=*p;j<*(p+1);j++) {
    if (*j<tok_flag) {
      if (*j==inserted) return 0;
      if (j==tok_loc) ident_seen=1;
      else app(*j);
    } else if (*j>=inner_tok_flag) confusion(_("inner"));
    else if (app_supp(*j-tok_flag+tok_start)) goto catch14;;
  }
  return 0;
catch14: if (*(*(p+1)-1)=='9') return 1; /* production 14 was used */
  else return 0;
}

@ The trickiest part of \.{CTWILL} is the procedure |make_ministring(l)|,
which tries to figure out a symbolic form of definition after
|make_underlined(pp+l)| has been called. We rely heavily on the
existing productions, which force the translated texts to have a
structure that's decodable even though the underlying |cat| and |mathness|
codes have disappeared.

@c static void
make_ministring(
  int l) /* 0, 1, or 2 */
{
  text_pointer q,r;
  name_pointer cn;
  token t;
  int ast_count; /* asterisks preceding the expression */
  boolean non_ast_seen; /* have we seen a non-asterisk? */
  if (tok_loc<=operator_found) return;
  cn=((*tok_loc)%id_flag)+name_dir;
  @<Append the type of the declaree; |return| if it begins with \&{extern}@>;
  null_scrap.mathness=(((pp+l)->mathness)%4)*5; big_app1(&null_scrap);
    /* now we're ready for the mathness that follows (I think) */
    /* (without the mod 4 times 5, comments posed a problem) */
    /* (namely in cases like |int a(b,c)| followed by comment) */
  ident_seen=0;@+app_supp((pp+l)->trans);
  null_scrap.mathness=10; big_app1(&null_scrap);
   /* now |cur_mathness==no_math| */
  ms_mode=1; ministring_ptr=ministring_buf;
  if (l==2) *ministring_ptr++='=';
  make_output(); /* translate the current text into a ministring */
  tok_ptr=*(--text_ptr); /* delete that text */
  new_meaning(cn);
  cur_mathness=maybe_math; /* restore it */
}

@ Here we use the fact that a |decl_head| comes from |int_like| only in
production~27, whose translation is fairly easy to recognize. (Well,
production 28 has been added for \CPLUSPLUS/, but we hope that doesn't
mess us up.) And we also use other similar facts.

If an identifier is given an \&{extern} definition, we don't change
its current meaning, but we do suppress mini-index entries to its
current meaning in other sections.

@<Append the type of the declaree; |return| if it begins with \&{extern}@>=
if (l==0) { app(int_loc+res_flag); app(' '); cur_mathness=no_math; }
else {
  q=(pp+l-1)->trans;
  ast_count=0;
  non_ast_seen=0;
  while (1) {
    if (*(q+1)==*q+1) {
      r=q;@+break; /* e.g. \&{struct}; we're doing production 45 or 46 */
    }
    if (**q<tok_flag) confusion(_("find type"));
    r=**q-tok_flag+tok_start;
    if ((t=*(*(q+1)-2))>=tok_flag && **(t-tok_flag+tok_start)=='*') {
           /* production 34 */
      if (!non_ast_seen) ast_count++; /* count immediately preceding |*|'s */
    } else non_ast_seen=1;
    if (*(*q+1)==' ' && *(q+1)==*q+2) break; /* production 27 */
    if (*(*q+1)=='{' && *(*q+2)=='}' && *(*q+3)=='$' && *(*q+4)==' '@|
       && *(q+1)==*q+5) break; /* production 27 in disguise */
    q=r;
  }
  while (**r>=tok_flag) {
    if (*(r+1)>*r+9 && *(*r+1)=='{' && *(*r+2)=='}' && *(*r+3)=='$' @|
        && *(*r+4)==indent) q=**r-tok_flag+tok_start; /* production 49 */
    r=**r-tok_flag+tok_start;
  }
  if (**r==ext_loc+res_flag) return; /* \&{extern} gives no definition */
  @<Append tokens for type |q|@>;
}

@ @<Append tokens for type |q|@>=
cur_mathness=no_math; /* it was |maybe_math| */
if (*(q+1)==*q+8 && *(*q+1)==' ' && *(*q+3)==' ') {
  app(**q);@+app(' ');@+app(*(*q+2)); /* production 46 */
} else if ((t=*(*(q+1)-1))>=tok_flag && **(r=t-tok_flag+tok_start)=='\\'
   && *(*r+1)=='{') app(**q); /* |struct_like| identifier */
else app((q-tok_start)+tok_flag);
while (ast_count) {
  big_app('{');@+app('*');@+app('}');@+ast_count--;
}

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
  make_underlined(pp);
  make_ministring(0);
  big_app1(pp);
  if (indent_param_decl) {
    big_app(indent); app(indent);
  }
  reduce(pp,1,fn_decl,0,1);
}
@z

@x
  make_underlined (pp);  squash(pp,2,tag,-1,7);
@y
  make_underlined (pp);
  if (tok_loc>operator_found) {
    name_pointer cn=((*tok_loc)%id_flag)+name_dir;
    strcpy(ministring_buf,"label");
    new_meaning(cn);
  }
  squash(pp,2,tag,-1,7);
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
  make_underlined(pp+1);
  make_ministring(1);
  squash(pp,2,decl_head,-1,35);
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
    make_underlined(pp+1); make_reserved(pp+1);
@y
    make_underlined(pp+1); make_reserved(pp+1);
    make_ministring(1);
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
if (cat1==define_like) make_underlined(pp+2);
@y
if (cat1==define_like) { /* \.{\#define} is analogous to \&{extern} */
  make_underlined(pp+2);
  if (tok_loc>operator_found) {
    /* no time to work out this case; I'll handle defines by brute force
       in the \.{aux} file, since they usually don't go in mini-index */
  }
}
@z

@x
if (cat1==prelangle) squash(pp+1,1,langle,1,100);
else squash(pp,1,exp,-2,101);
@y
if (cat1==prelangle) squash(pp+1,1,langle,1,121);
else squash(pp,1,exp,-2,122);
@z

@x
  big_app1(pp); big_app(' '); big_app1(pp+1); reduce(pp,2,else_like,-2,102);
@y
  big_app1(pp); big_app(' '); big_app1(pp+1); reduce(pp,2,else_like,-2,123);
@z

@x
@ @<Cases for |typedef_like|@>=
if ((cat1==int_like || cat1==cast) && (cat2==comma || cat2==semi))
  squash(pp+1,1,exp,-1,115);
else if (cat1==int_like) {
  big_app1(pp); big_app(' '); big_app1(pp+1); reduce(pp,2,typedef_like,0,116);
}
else if (cat1==exp && cat2!=lpar && cat2!=exp && cat2!=cast) {
  make_underlined(pp+1); make_reserved(pp+1);
  big_app1(pp); big_app(' '); big_app1(pp+1); reduce(pp,2,typedef_like,0,117);
}
else if (cat1==comma) {
  big_app2(pp); big_app(' '); reduce(pp,2,typedef_like,0,118);
}
else if (cat1==semi) squash(pp,2,decl,-1,119);
else if (cat1==ubinop && (cat2==ubinop || cat2==cast)) {
  big_app('{'); big_app1(pp+1); big_app('}'); big_app1(pp+2);
  reduce(pp+1,2,cat2,0,120);
}
@y
@ Here \.{CTWILL} deviates from the normal productions introduced in
version 3.6, because those productions bypass |decl_head| (thereby
confusing |make_ministring|, which depends on the |decl_head| productions
to deduce the type). We revert to an older syntax that was
less friendly to \CPLUSPLUS/ but good enough for me.

@<Cases for |typedef_like|@>=
if (cat1==decl_head) {
  if ((cat2==exp&&cat3!=lpar&&cat3!=exp)||cat2==int_like) {
    make_underlined(pp+2); make_reserved(pp+2);
    make_ministring(2);
    big_app2(pp+1); reduce(pp+1,2,decl_head,0,200);
  }
  else if (cat2==semi) {
    big_app1(pp); big_app(' '); big_app2(pp+1); reduce(pp,3,decl,-1,201);
  }
} else if (cat1==int_like && cat2==raw_int &&
    (cat3==semi || cat3==comma)) squash(pp+2,1,exp,1,202);
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
  case meaning: case suppress:
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
      else app_scrap(p->ilk,maybe_math);
    }
  }
@y
      else app_scrap(p->ilk,maybe_math);
    }
  }
  @<Flag the usage of this identifier, for the mini-index@>;
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
to \.{\\PB}, if the user has invoked \.{CWEAVE} with the \.{+e} flag.
Although \.{cwebmac} ignores \.{\\PB}, other macro packages
@y
to \.{\\PB}, if the user has invoked \.{CTWILL} with the \.{+e} flag.
Although \.{ctwimac} ignores \.{\\PB}, other macro packages
@z

@x
void
outer_parse() /* makes scraps from \CEE/ tokens and comments */
@y
static void
outer_parse(void) /* makes scraps from \CEE/ tokens and comments */
@z

@x
        app(tok_flag+(int)(p-tok_start));
@y
        app(tok_flag+(int)(p-tok_start));
        app(inserted);
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
section_count=0; format_visible=1; copy_limbo();
@y
temp_switch=0; temp_meaning_ptr=temp_meaning_stack;
@<Read the \.{.aux} file, if present; then open it for output@>;
section_count=0; format_visible=1; right_start_switch=0; copy_limbo();
@z

@x
while (!input_has_ended) @<Translate the current section@>;
}

@y
while (!input_has_ended) @<Translate the current section@>;
}

@ @<Glob...@>=
FILE *aux_file;
char aux_file_name[max_file_name_length]; /* name of \.{.aux} file */

@ @<Read the \.{.aux} file, if present; then open it for output@>=
strncpy(aux_file_name,tex_file_name,strlen(tex_file_name)-4);
strcat(aux_file_name,".bux");
include_depth=1; /* we simulate \.{@@i} */
strcpy(cur_file_name,aux_file_name); /* first in, third out */
if ((cur_file=fopen(cur_file_name,"r"))) { cur_line=0; include_depth++; }
strcpy(aux_file_name+strlen(aux_file_name)-4,".aux");
strcpy(cur_file_name,aux_file_name); /* second in, second out */
if ((cur_file=fopen(cur_file_name,"r"))) { cur_line=0; include_depth++; }
strcpy(cur_file_name,"system.bux"); /* third in, first out */
if ((cur_file=fopen(cur_file_name,"r"))) cur_line=0;
else include_depth--;
if (include_depth) { /* at least one new file was opened */
  while (get_next()==meaning) ; /* new meaning is digested */
  if (include_depth) err_print(_("! Only @@$ is allowed in aux and bux files"));
  finish_line(); loc=buffer; /* now reading beginning of line 1 */
}
if ((aux_file=fopen(aux_file_name,"wb"))==NULL)
  fatal(_("! Cannot open aux output file "),aux_file_name);

@z

@x
boolean group_found=0; /* has a starred section occurred? */
@y
boolean group_found=0; /* has a starred section occurred? */
boolean right_start_switch; /* has `\.{@@r}' occurred recently? */
boolean temp_switch; /* has `\.{@@\%}' occurred recently? */
@z

@x
@ @<Translate the current section@>= {
  section_count++;
@y
@ @d usage_sentinel (struct perm_meaning *)1
@<Translate the current section@>= {
  section_count++;
  temp_switch=0; temp_meaning_ptr=temp_meaning_stack;
  top_usage=usage_sentinel;
@z

@x
if (*(loc-1)!='*') out_str("\\M");
@y
if (*(loc-1)!='*') {
  if (right_start_switch) {
    out_str("\\shortpage\n"); right_start_switch=0;
@.\\shortpage@>
  }
  out_str("\\M");
}
@z

@x
@.\\N@>
@y
@.\\N@>
  if (right_start_switch) {
    out_str("N"); right_start_switch=0;
@.\\NN@>
  }
@z

@x
out_str("{");out_section(section_count); out_str("}");
@y
out_str("{");out_section(section_count); out_str("}");
flush_buffer(out_ptr,0,0);
@z

@x
    case '@@': out('@@'); break;
@y
    case '@@': out('@@'); break;
    case temp_meaning: temp_switch=1-temp_switch; break;
    case right_start: right_start_switch=1; break;
@z

@x
    case section_name: loc-=2; next_control=get_next(); /* skip to \.{@@>} */
@y
    case meaning: case suppress:
    case section_name: loc-=2; next_control=get_next(); /* reprocess */
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
  outer_parse(); finish_C(format_visible); format_visible=1;
  doing_format=0;
}
@y
  outer_parse();
  if (is_macro) @<Make ministring for a new macro@>;
  finish_C(format_visible); format_visible=1;
  doing_format=0;
}

@ @<Glob...@>=
boolean is_macro; /* it's a macro def, not a format def */
int def_diff; /* 0 iff the current macro has parameters */
name_pointer id_being_defined; /* the definee */
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
@<Start a macro...@>= {
@y
@<Start a macro...@>= {
  is_macro=1;
@z

@x
    err_print("! Improper macro definition");
@y
    err_print(_("! Improper macro definition"));
@z

@x
@.Improper macro definition@>
  else {
    app('$'); app_cur_id(0);
@y
@.Improper macro definition@>
  else {
    id_being_defined=id_lookup(id_first,id_loc,normal);
    app('$'); app_cur_id(0);
    def_diff=*loc-'(';
@z

@x
      default: err_print("! Improper macro definition"); break;
@y
      default: err_print(_("! Improper macro definition")); break;
@z

@x
@ @<Start a format...@>= {
  doing_format=1;
@y
@ @<Make ministring for a new macro@>=
{
  ms_mode=1; ministring_ptr=ministring_buf;
  *ministring_ptr++='=';
  if (def_diff) { /* parameterless */
    scrap_pointer s=scrap_ptr;
    text_pointer t;
    token_pointer j;
    while (s->cat==insert) s--;
    if ((s-1)->cat==dead && s->cat==exp && **(t=s->trans)=='\\'
         && *(*t+1)=='T') /* it's just a constant */
      for (j=*t;j<*(t+1);j++) *ministring_ptr++=*j;
    else out_str("macro");
  } else out_str("macro (\\,)");
  new_meaning(id_being_defined);
}

@ @<Start a format...@>= {
  doing_format=1;
  is_macro=0;
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
out_str("\\fi"); finish_line();
@.\\fi@>
flush_buffer(out_buf,0,0); /* insert a blank line, it looks nice */
@y
finish_line(); out_str("\\mini"); finish_line();
@.\\mini@>
@<Output information about usage of id's defined in other sections@>;
out_str("}\\FI"); finish_line();
@.\\FI@>
flush_buffer(out_buf,0,0); /* insert a blank line, it looks nice */

@ The following code is performed for each identifier parsed during
a section. Variable |top_usage| is always nonzero; it has the sentinel
value~1 initially, then it points to each variable scheduled for
possible citation. A variable is on this list if and only if its
|link| field is nonzero. All variables mentioned in the section are
placed on the list, unless they are reserved and their current
\TeX\ meaning is uninitialized.

@ @<Flag the usage of this identifier, for the mini-index@>=
{ struct perm_meaning *q=p-name_dir+cur_meaning;
  if (!(abnormal(p)) || strcmp(q->perm.tex_part,"\\uninitialized")!=0)
    if (q->link==0) {
      q->link=top_usage;
      top_usage=q;
    }
}

@ @<Output information about usage of id's defined in other sections@>=
{@+struct perm_meaning *q;
  while (temp_meaning_ptr>temp_meaning_stack) {
    out_mini(--temp_meaning_ptr);
    q=temp_meaning_ptr->id-name_dir+cur_meaning;
    q->stamp=section_count; /* suppress output from ``permanent'' data */
  }
  while (top_usage!=usage_sentinel) {
    q=top_usage;
    top_usage=q->link;
    q->link=NULL;
    if (q->stamp!=section_count) out_mini(&(q->perm));
  }
}

@ @<Predec...@>=
static void out_mini(meaning_struct *);@/

@ @c static void
out_mini(
  meaning_struct *m)
{ char s[60];
  name_pointer cur_name=m->id;
  if (m->prog_no==0) { /* reference within current program */
    if (m->sec_no==section_count) return; /* defined in current section */
    sprintf(s,"\\[%d",m->sec_no);
  } else { name_pointer n=title_code[m->prog_no];
    if (*(n->byte_start)=='{')
      sprintf(s,"\\]%.*s%d",(int)((n+1)->byte_start-n->byte_start),n->byte_start,
             m->sec_no);
    else sprintf(s,"\\]%.*s",(int)((n+1)->byte_start-n->byte_start),n->byte_start);
  }
  out_str(s); out(' ');
  @<Mini-output the name at |cur_name|@>;
  out(' '); out_str(m->tex_part); finish_line();
}

@ @<Mini-output...@>=
switch (cur_name->ilk) {
  case normal: case func_template: if (length(cur_name)==1) out_str("\\|");
    else {char *j;
      for (j=cur_name->byte_start;j<(cur_name+1)->byte_start;j++)
        if (xislower(*j)) goto lowcase;
      out_str("\\."); break;
lowcase: out_str("\\\\");
    }
  break;
@.\\|@>
@.\\.@>
@.\\\\@>
  case roman: break;
  case wildcard: out_str("\\9"); break;
@.\\9@>
  case typewriter: out_str("\\."); break;
@.\\.@>
  case custom: {char *j; out_str("$\\");
    for (j=cur_name->byte_start;j<(cur_name+1)->byte_start;j++)
      out(*j=='_'? 'x': *j=='$'? 'X': *j);
    out('$');
    goto name_done;
    }
  default: out_str("\\&");
@.\\\&@>
}
out_name(cur_name,1);
name_done:

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
  if (change_exists) {
    @<Tell about changed sections@>; finish_line(); finish_line();
  }
@y
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
the index section itself.
@y
the index section itself---NOT!
@z

@x
@ @<Tell about changed sections@>= {
  /* remember that the index is already marked as changed */
  k_section=0;
  while (!changed_section[++k_section]);
  out_str("\\ch ");
@.\\ch@>
  out_section(k_section);
  while (k_section<section_count) {
    while (!changed_section[++k_section]);
    out_str(", "); out_section(k_section);
  }
  out('.');
}

@y
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
@ @<Output the name...@>=
@y
@ We don't format the index completely; the \.{twinx} program does the
rest of the job.

@<Output the name...@>=
@z

@x
  case normal: case func_template: if (is_tiny(cur_name)) out_str("\\|");
@y
  case normal: if (is_tiny(cur_name)) out_str("\\|");
@z

@x
  case wildcard: out_str("\\9");@+ goto not_an_identifier;
@y
  case roman: out_str("  "); goto not_an_identifier;
  case wildcard: out_str("\\9"); goto not_an_identifier;
@z

@x
  case roman: not_an_identifier: out_name(cur_name,0); goto name_done;
  case custom: {char *j; out_str("$\\");
    for (j=cur_name->byte_start;j<(cur_name+1)->byte_start;j++)
      out(*j=='_'? 'x': *j=='$'? 'X': *j);
    out('$');
    goto name_done;
    }
@y
not_an_identifier: out_name(cur_name,0); goto name_done;
  case custom: out_str("\\$"); break;
@.\\\$@>
@z

@x
out_name(cur_name,1);
@y
if (proofing) out_name(cur_name,1);
else {
  out('{');
  {char *j;
    for (j=cur_name->byte_start;j<(cur_name+1)->byte_start;j++) out(*j);
  }
  out('}');
}
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
  printf(_("%ld temp meanings (out of %ld)\n"),
            (long)(max_temp_meaning_ptr-temp_meaning_stack),
            (long)max_meanings);
  printf(_("%ld titles (out of %ld)\n"),
            (long)(title_code_ptr-title_code),(long)max_titles);
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
section should have the same number as the original ``\&{275.~Index},'' and
additional material follows below.

@* Set {\tt CWEAVE} flags.
At least one of these is already used in \.{COMMON}.

@<Set init...@>=
  make_xrefs=force_lines=make_pb=indent_param_decl=order_decl_stmt=1;
    /* controlled by command-line options */

@* Function declarations.  Here are declarations---conforming to
{\mc ANSI~C}---of all functions in this code, as far as they are
not already in |"common.h"|.  These are private to \.{CWEAVE} and \.{CTWILL}.

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
  int x_size,y_size,comparison;

  if((check_file=fopen(check_file_name,"r"))==NULL)
    fatal(_("! Cannot open output file "),check_file_name);
@.Cannot open output file@>

  @<Compare the temporary output to the previous output@>@;

  fclose(tex_file); tex_file=NULL;
  fclose(check_file); check_file=NULL;

  @<Take appropriate action depending on the comparison@>@;
} else
  rename(check_file_name,tex_file_name); /* This was the first run */

strcpy(check_file_name,""); /* We want to get rid of the temporary file */

@ We hope that this runs fast on most systems.

@<Compare the temp...@>=
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

