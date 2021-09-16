Section 1.

@x
@** Introduction.
@y
\ifacro \ifx\undefined\pdfpagewidth\else
  \pdfpagewidth=\pagewd \advance\pdfpagewidth by 2cm
  \pdfpageheight=\pageht \advance\pdfpageheight by 5cm
  \ifpdftex \pdfhorigin=1cm \pdfvorigin=1cm
  \else \global\hoffset=-1.54cm \global\voffset=-1.54cm \fi
\fi\fi

@** Introduction.
@z

@x
A kind of ``user manual'' for \.{CTWILL} can be found in the appendix
{\bf 271.~Mogrify \.{CWEAVE} into \.{CTWILL}} and beyond, together
with additional material specific to \.{CTWILL}. % FIXME
Until then, \.{CWEAVE}'s sequence of sections will be preserved.

The ``banner line'' defined here should be changed whenever \.{CTWILL} is
@y
A kind of ``user manual'' for \.{CTWILL} can be found in section~\&{289}
and beyond, together with additional material specific to \.{CTWILL}. % FIXME
\bigskip
{\font\itt=cmitt10 \font\bit=cmbxti10
\noindent \bit Editor's Note: \it This heavily redacted version of
{\itt ctwill.pdf} had to meddle with the section numbering of
{\itt cweave.w}, spreading tabular material over several sections
and splitting long sections into smaller chunks in order to
fix overful pages---both horizontally and vertically---, to make
the overall appearance of the {\itt CTWILL} documentation most
pleasing to the readers'~eyes.
\smallskip
\noindent Please do not try to compare this {\itt ctwill.pdf} to the one
created by {\itt CWEAVE} instead of {\itt CTWILL}; the section numbering will
be quite ``off'' from {\itt cweave.w}.  Care has been taken to give a
faithful overall rendering of {\itt CTWILL}'s code, though. \hfill
---Enjoy!\par}
\bigskip
The ``banner line'' defined here should be changed whenever \.{CTWILL} is
@-banner@>
@$banner {CTWILL}1 =\.{"This\ is\ CTWILL"}@>
@z

Section 2.

@x
@ \.{CWEAVE} has a fairly straightforward outline.  It operates in
@y
@r @ \.{CWEAVE} has a fairly straightforward outline.  It operates in
@%
@$show_banner {CTWILL}14 =\\{flags}[\.{'b'}]@>
@$show_progress {CTWILL}14 =\\{flags}[\.{'p'}]@>
@z

Section 4.

@x
@d _(s) gettext(s)
@y
@d _(s) gettext(s)
@-s@>
@-a@>
@-HAVE_GETTEXT@>
@z

Section 5.

@x
@d compress(c) if (loc++<=limit) return c
@y
@d compress(c) if (loc++<=limit) return c
@-c@>
@z

Section 6.

@x
@ Code related to input routines:
@y
@r @ Code related to input routines:
@-c@>
@z

Section 7.

@x
@d cur_line line[include_depth] /* number of current line in current file */
@y
@d cur_line line[include_depth] /* number of current line in current file */
@-cur_file@>
@-cur_file_name@>
@-cur_line@>
@$cur_file {CTWILL}7 =\\{file}[\\{include\_depth}]@>
@$cur_file_name {CTWILL}7 =\hfil\break\\{file\_name}[\\{include\_depth}]@>
@$cur_line {CTWILL}7 =\\{line}[\\{include\_depth}]@>
@z

Section 10.

@x
@ Code related to identifier and section name storage:
@y
@ Code related to identifier and section name storage:
@-c@>
@-llink@>
@-rlink@>
@-root@>
@$llink {CTWILL}10 =\\{link}@>
@$rlink {CTWILL}10 =\\{dummy}.\\{Rlink}@>
@$root {CTWILL}10 =\\{name\_dir}$\MG$\\{rlink}@>
@z

Section 12.

@x
@ Code related to error handling:
@y
@ Code related to error handling:
@-s@>
@z

Section 14.

@x
extern const char *use_language; /* prefix to \.{cwebmac.tex} in \TEX/ output */
@y
extern const char *use_language; /* prefix to \.{ctwimac.tex} in \TEX/ output */
@-show_banner@>
@-show_progress@>
@-show_happiness@>
@-show_stats@>
@-make_xrefs@>
@-check_for_change@>
@$show_banner {CTWILL}14 =\\{flags}[\.{'b'}]@>
@$show_progress {CTWILL}14 =\\{flags}[\.{'p'}]@>
@$show_happiness {CTWILL}14 =\\{flags}[\.{'h'}]@>
@$show_stats {CTWILL}14 =\\{flags}[\.{'s'}]@>
@$make_xrefs {CTWILL}14 =\\{flags}[\.{'x'}]@>
@$check_for_change {CTWILL}14 =\\{flags}[\.{'c'}]@>
@z

Section 15.

@x
@ Code related to output:
@y
@ Code related to output:
@-a@>
@-b@>
@-c@>
@-update_terminal@>
@-new_line@>
@$update_terminal {CTWILL}15 =\\{fflush}(\\{stdout})@>
@$new_line {CTWILL}15 =\\{putchar}(\.{'\\n'})@>
@z

Section 16.

@x
@ The procedure that gets everything rolling:
@y
@r @ The procedure that gets everything rolling:
@z

Section 17.

@x
@d long_buf_size (buf_size+longest_name) /* for \.{CWEAVE} */
@y
@d long_buf_size (buf_size+longest_name) /* for \.{CWEAVE} */
@-long_buf_size@>
@$long_buf_size {CTWILL}17 =$\\{buf\_size}+\\{longest\_name}$@>
@z

Section 20.

@x
@* Data structures exclusive to {\tt CWEAVE}.
@y
@* Data structures exclusive to {\tt CWEAVE}.
@-a@>
@-ilk@>
@$ilk {CTWILL}20 =\\{dummy}.\\{Ilk}@>
@z

Section 21.

@x
@ We keep track of the current section number in |section_count|, which
@y
@r @ We keep track of the current section number in |section_count|, which
@z

Section 22.

@x
@ The other large memory area in \.{CWEAVE} keeps the cross-reference data.
@y
@ The other large memory area in \.{CWEAVE} keeps the cross-reference data.
@-p@>
@-x@>
@z

Section 24.

@x
@d file_flag (3*cite_flag)
@y
@-file_flag@>
@-def_flag@>
@-xref@>
@$file_flag {CTWILL}24 =$\T{3}*\\{cite\_flag}{}$@>
@$def_flag {CTWILL}24 =$\T{2}*\\{cite\_flag}{}$@>
@$xref {CTWILL}24 =\\{equiv\_or\_xref}@>
@d file_flag (3*cite_flag)
@z

Section 25.

@x
@ A new cross-reference for an identifier is formed by calling |new_xref|,
@y
@ A new cross-reference for an identifier is formed by calling |new_xref|,
@-a@>
@-c@>
@-p@>
@-no_xref@>
@$no_xref {CTWILL}25 =$\R\\{make\_xrefs}$@>
@z

Section 36.

@x
@d underline '\n' /* this code will be intercepted without confusion */
@y
@d underline '\n' /* this code will be intercepted without confusion */
@-begin_comment@>
@-underline@>
@$begin_comment {CTWILL}36 =\.{'\\t'}@>
@$underline {CTWILL}36 =\.{'\\n'}@>
@z

Section 37.

@x
@ Control codes are converted to \.{CWEAVE}'s internal
@y
@r @ Control codes are converted to \.{CWEAVE}'s internal
@z

Section 43.

@x
\yskip\hang |identifier|: In this case the global variables |id_first| and
|id_loc| will have been set to the beginning and ending-plus-one locations
in the buffer, as required by the |id_lookup| routine.

\yskip\hang |string|: The string will have been copied into the array
|section_text|; |id_first| and |id_loc| are set as above (now they are
pointers into |section_text|).

\yskip\hang |constant|: The constant is copied into |section_text|, with
slight modifications; |id_first| and |id_loc| are set.
@y
{\raggedright
\yskip\hang |identifier|: In this case the global variables |id_first| and
|id_loc| will have been set to the beginning and ending-plus-one locations
in the buffer, as required by the |id_lookup| routine.

\yskip\hang |string|: The string will have been copied into the array
|section_text|; |id_first| and |id_loc| are set as above (now they are
pointers into |section_text|).

\yskip\hang |constant|: The constant is copied into |section_text|, with
slight modifications; |id_first| and |id_loc| are set.\par}
@z

@x
\yskip\hang |xref_roman|, |xref_wildcard|, |xref_typewriter|, |TeX_string|,
|meaning|, |suppress|,
|verbatim|: The values of |id_first| and |id_loc| will have been set to
the beginning and ending-plus-one locations in the buffer.

\yskip\hang |section_name|: In this case the global variable |cur_section| will
point to the |byte_start| entry for the section name that has just been scanned.
The value of |cur_section_char| will be |'('| if the section name was
preceded by \.{@@(} instead of \.{@@<}.
@y
{\raggedright
\yskip\hang |xref_roman|, |xref_wildcard|, |xref_typewriter|, |TeX_string|,
|meaning|, |suppress|,
and |verbatim|: The values of |id_first| and |id_loc| will have been set to
the beginning and ending-plus-one locations in the buffer.

\yskip\hang |section_name|: In this case the global variable |cur_section| will
point to the\hfil\break |byte_start| entry for the section name that has just been scanned.
The value of |cur_section_char| will be |'('| if the section name was
preceded by \.{@@(} instead of \.{@@<}.\par}
@z

Section 44.

@x
@ As one might expect, |get_next| consists mostly of a big switch
@y
@ As one might expect, |get_next| consists mostly of a big switch
@-c@>
@$c {CTWILL}44 \&{eight\_bits}@>
@z

Section 45.

@x
@ @<Predecl...@>=@+static eight_bits get_next(void);
@y
@ @<Predecl...@>=@+static eight_bits get_next(void);
@-get_next@>
@z

Section 46.

@x
@d left_preproc ord /* begins a preprocessor command */
@y
@-left_preproc@>
@$left_preproc {CTWILL}46 =\\{ord}@>
@d left_preproc ord /* begins a preprocessor command */
@z

Section 51.

@x
              if (*(loc+1)=='*') {loc++;@+compress(minus_gt_ast);}
@y
              if (*(loc+1)=='*') {@+loc++;@+compress(minus_gt_ast);@+}
@z

@x
            else if (*loc=='.' && *(loc+1)=='.') {
              loc++;@+compress(dot_dot_dot);
            } break;
@y
            else if (*loc=='.' && *(loc+1)=='.') {@+
              loc++;@+compress(dot_dot_dot);@+
            } break;
@z

Section 52.

@x
  id_first=--loc;
  do
    ++loc;
  while (isalpha((int)*loc) || isdigit((int)*loc) @|
      || isxalpha(*loc) || ishigh(*loc));
  id_loc=loc; return identifier;
}
@y
  id_first=--loc;@/
  do
    ++loc;
  while (isalpha((int)*loc) || isdigit((int)*loc) @|
      || isxalpha(*loc) || ishigh(*loc));@/
  id_loc=loc;@/
  return identifier;
}
@z

Section 53.

@x
@d gather_digits_while(t) while ((t) || *loc=='\'')
@y
@d gather_digits_while(t) while ((t) || *loc=='\'')
@-t@>
@z

Section 55.

@x
@ @<Get a bin...@>={
@y
@r @ @<Get a bin...@>={
@z

Section 58.

@x
@ @<Get a wide...@>={
@y
@r @ @<Get a wide...@>={
@z

Section 63.

@x
@ @<If end of name...@>=
@y
@r @ @<If end of name...@>=
@z

Section 65.

@x
@ @<Predecl...@>=@+static void skip_restricted(void);
@y
@ @<Predecl...@>=@+static void skip_restricted(void);
@-skip_restricted@>
@z

Section 69.

@x
@ @<Predecl...@>=@+static void phase_one(void);
@y
@ @<Predecl...@>=@+static void phase_one(void);
@-phase_one@>
@z

Section 72.

@x
C_xref( /* makes cross-references for \CEE/ identifiers */
  eight_bits spec_ctrl)
@y
C_xref( /* makes cross-references for \CEE/ identifiers */
  eight_bits spec_ctrl)
@-C_xref@>
@$C_xref {CTWILL}72 \&{static} \&{void} (\,)@>
@z

@x
    if (next_control=='|' || next_control==begin_comment ||
        next_control==begin_short_comment) return;
@y
    if (next_control=='|' || next_control==begin_comment @| ||
        next_control==begin_short_comment) return;
@z

Section 73.

@x
@ The |outer_xref| subroutine is like |C_xref| except that it begins
with |next_control!='|'| and ends with |next_control>=format_code|. Thus, it
handles \CEE/ text with embedded comments.
@y
@ The |outer_xref| subroutine is like |C_xref| except that it begins
with |next_control| |!='|'| and ends with |next_control>=format_code|.
Thus, it handles \CEE/ text with embedded comments.
@z

Section 80.

@x
@ Finally, when the \TEX/ and definition parts have been treated, we have
|next_control>=begin_C|.
@y
@ Finally, when the \TEX/ and definition parts have been treated, we have
\hfil\break|next_control>=begin_C|.
@z

Section 86.

@x
@ The |flush_buffer| routine empties the buffer up to a given breakpoint,
@y
@ The |flush_buffer| routine empties the buffer up to a given breakpoint,
@-b@>
@-c@>
@-tex_new_line@>
@$tex_new_line {CTWILL}86 =$\\{putc}(\.{'\\n'},\39\\{active\_file})$@>
@z

Section 89.

@x
@d proofing flags['P']
@y
@d proofing flags['P']
@-proofing@>
@$proofing {CTWILL}89 =\\{flags}[\.{'P'}]@>
@z

Section 90.

@x
@ When we wish to append one character |c| to the output buffer, we write
@y
@ When we wish to append one character |c| to the output buffer, we write
@-c@>
@-s@>
@z

Section 91.

@x
out_str( /* output characters from |s| to end of string */
const char*s)
@y
out_str( /* output characters from |s| to end of string */
const char*s)
@-out_str@>
@$out_str {CTWILL}91 \&{static} \&{void} (\,)@>
@z

Section 97.

@x
@ The |out_name| procedure is used to output an identifier or index
@y
@r @ The |out_name| procedure is used to output an identifier or index
@z

Section 100.

@x
@ The |copy_TeX| routine processes the \TEX/ code at the beginning of a
@y
@r @ The |copy_TeX| routine processes the \TEX/ code at the beginning of a
@z

Section 101.

@x
@ The |copy_comment| function issues a warning if more braces are opened than
@y
@ The |copy_comment| function issues a warning if more braces are opened than
@-c@>
@-t@>
@-copy_comment@>
@$copy_comment {CTWILL}101 \&{static} \&{int} (\,)@>
@z

@x
      } else {
@y
      } @+ else {
@z

Section 106.

@x
@ Here is a list of the category codes that scraps can have.
@y
@r @ Here is a list of the category codes that scraps can have.
@z

Section 108.

@x
@d print_cat(c) fputs(cat_name[c],stdout)
@y
@d print_cat(c) fputs(cat_name[c],stdout)
@-c@>
@z

Section 109--110.

@x
@ The token lists for translated \TEX/ output contain some special control
@y
@r @ The token lists for translated \TEX/ output contain some special control
@-n@>
@z

@x
\yskip\noindent All of these tokens are removed from the \TEX/ output that
@y
@ All of these tokens are removed from the \TEX/ output that
@-n@>
@z

Section 111--116.

@x
@ The raw input is converted into scraps according to the following table,
@y
@* From raw input to scraps.
@-c@>
\advance \hsize by 4cm
\ifx\undefined\pdfpagewidth \else \advance \pdfpagewidth by 4cm \fi
The raw input is converted into scraps according to the following table,
@z

@x
\yskip\halign{\quad#\hfil&\quad#\hfil&\quad\hfil#\hfil\cr
@y
\yskip\halign{\quad#\hfil&\quad\hbox to11cm{#\hfil}&\quad\hfil#\hfil\cr
@z

@x
\./&|binop|: \./&yes\cr
@y
\./&|binop|: \./&yes\cr}

@ Cont.

\yskip\halign{\quad#\hfil&\quad#\hfil&\quad\hfil#\hfil\cr
@z

@x
\.{complex}&|int_like|: \stars&yes\cr
@y
\.{complex}&|int_like|: \stars&yes\cr}

@ Cont.

\yskip\halign{\quad#\hfil&\quad#\hfil&\quad\hfil#\hfil\cr
@z

@x
\.{friend}&|int_like|: \stars&maybe\cr
@y
\.{friend}&|int_like|: \stars&maybe\cr}

@ Cont.

\yskip\halign{\quad#\hfil&\quad#\hfil&\quad\hfil#\hfil\cr
@z

@x
\.{static\_cast}&|raw_int|: \stars&maybe\cr
@y
\.{static\_cast}&|raw_int|: \stars&maybe\cr}

@ Cont.

\yskip\halign{\quad#\hfil&\quad#\hfil&\quad\hfil#\hfil\cr
@z

@x
\.{xor\_eq}&|alfop|: \stars&yes\cr
@y
\.{xor\_eq}&|alfop|: \stars&yes\cr}

@ Cont.

\yskip\halign{\quad#\hfil&\quad#\hfil&\quad\hfil#\hfil\cr
@z

Sections 117--125.

@x l.7 line numbers refer to 'prod.w'
@ Here is a table of all the productions.  Each production that
@y
@* Table of all productions.  Each production that
@-time@>
@z

TeX reports 'extra \fi' when running on twilled 'ctwill.w'.

@x l.14
\fi \newcount\prodno \newdimen\midcol \let\+\relax \ifon
@y
\newcount\prodno \newdimen\midcol \let\+\relax
@z

Section 118.

@x l.78
\+& |lpar| |rpar| & |exp| \hfill $L\.{\\,}R$ & functions, declarations\cr
@y
\+& |lpar| |rpar| & |exp| \hfill $L\.{\\,}R$ & functions, declarations\cr
\endgroup
@-in@>@-f@>@-x@>@-y@>

@r @ Cont.
\begingroup \lineskip=4pt
\def\alt #1 #2
{$\displaystyle\Bigl\{\!\matrix{\strut\hbox{#1}\cr
   \strut\hbox{#2}\cr}\!\Bigr\}$ }
\def\altt #1 #2 #3
{$\displaystyle\Biggl\{\!\matrix{\strut\hbox{#1}\cr\hbox{#2}\cr
   \strut\hbox{#3}\cr}\!\Biggr\}$ }
\def\malt #1 #2
{$\displaystyle\!\matrix{\strut\hbox{#1}\hfill\cr\strut\hbox{#2}\hfill\cr}$}
\def\maltt #1 #2 #3
{$\displaystyle\!\matrix{\strut\hbox{#1}\hfill\cr\hbox{#2}\hfill\cr
   \strut\hbox{#3}\hfill\cr}$}
\yskip@-in@>@-x@>@-y@>
\prodno=13 \midcol=2.5in
\def\theprodno{\number\prodno \global\advance\prodno by1\enspace}
\def\dagit{\dag\theprodno}
\def\+#1&#2&#3&#4\cr{\def\next{#1}%
 \line{\hbox to 2em{\hss
  \ifx\next\empty\theprodno\else\next\fi}\strut
  \ignorespaces#2\hfil\hbox to\midcol{$\RA$
  \ignorespaces#3\hfil}\quad \hbox to1.45in{\ignorespaces#4\hfil}}}
@z

Section 119.

@x l.45
         |int_like| \alt|raw_int| |struct_like| & |extern "Ada" int|\cr
@y
         |int_like| \alt|raw_int| |struct_like| & |extern "Ada" int|\cr
\endgroup

@ Cont.
\begingroup \lineskip=4pt
\def\alt #1 #2
{$\displaystyle\Bigl\{\!\matrix{\strut\hbox{#1}\cr
   \strut\hbox{#2}\cr}\!\Bigr\}$ }
\def\altt #1 #2 #3
{$\displaystyle\Biggl\{\!\matrix{\strut\hbox{#1}\cr\hbox{#2}\cr
   \strut\hbox{#3}\cr}\!\Biggr\}$ }
\def\malt #1 #2
{$\displaystyle\!\matrix{\strut\hbox{#1}\hfill\cr\strut\hbox{#2}\hfill\cr}$}
\def\maltt #1 #2 #3
{$\displaystyle\!\matrix{\strut\hbox{#1}\hfill\cr\hbox{#2}\hfill\cr
   \strut\hbox{#3}\hfill\cr}$}
\yskip@-in@>
@%@$dindent {CTWILL}110 =\T{\~226/}@>
\prodno=27 \midcol=2.5in
\def\theprodno{\number\prodno \global\advance\prodno by1\enspace}
\def\dagit{\dag\theprodno}
\def\+#1&#2&#3&#4\cr{\def\next{#1}%
 \line{\hbox to 2em{\hss
  \ifx\next\empty\theprodno\else\next\fi}\strut
  \ignorespaces#2\hfil\hbox to\midcol{$\RA$
  \ignorespaces#3\hfil}\quad \hbox to1.45in{\ignorespaces#4\hfil}}}
@z

Section 120.

@x l.75
    & \&{struct} \&{name\_info} $\{$\cr
@y
    & \&{struct} \&{name\_info} $\{$\cr
\endgroup

@r @ Cont.@-z@>@-in@>@-x@>
\begingroup \lineskip=4pt
\def\alt #1 #2
{$\displaystyle\Bigl\{\!\matrix{\strut\hbox{#1}\cr
   \strut\hbox{#2}\cr}\!\Bigr\}$ }
\def\altt #1 #2 #3
{$\displaystyle\Biggl\{\!\matrix{\strut\hbox{#1}\cr\hbox{#2}\cr
   \strut\hbox{#3}\cr}\!\Biggr\}$ }
\def\malt #1 #2
{$\displaystyle\!\matrix{\strut\hbox{#1}\hfill\cr\strut\hbox{#2}\hfill\cr}$}
\def\maltt #1 #2 #3
{$\displaystyle\!\matrix{\strut\hbox{#1}\hfill\cr\hbox{#2}\hfill\cr
   \strut\hbox{#3}\hfill\cr}$}
\yskip@-in@>@-x@>
\prodno=47 \midcol=2.5in
\def\theprodno{\number\prodno \global\advance\prodno by1\enspace}
\def\dagit{\dag\theprodno}
\def\+#1&#2&#3&#4\cr{\def\next{#1}%
 \line{\hbox to 2em{\hss
  \ifx\next\empty\theprodno\else\next\fi}\strut
  \ignorespaces#2\hfil\hbox to\midcol{$\RA$
  \ignorespaces#3\hfil}\quad \hbox to1.45in{\ignorespaces#4\hfil}}}
@z

Section 121.

@x l.164
    & \&{else} $x=0;$\cr
@y
    & \&{else} $x=0;$\cr
\endgroup

@ Cont.
\begingroup \lineskip=4pt
\def\alt #1 #2
{$\displaystyle\Bigl\{\!\matrix{\strut\hbox{#1}\cr
   \strut\hbox{#2}\cr}\!\Bigr\}$ }
\def\altt #1 #2 #3
{$\displaystyle\Biggl\{\!\matrix{\strut\hbox{#1}\cr\hbox{#2}\cr
   \strut\hbox{#3}\cr}\!\Biggr\}$ }
\def\malt #1 #2
{$\displaystyle\!\matrix{\strut\hbox{#1}\hfill\cr\strut\hbox{#2}\hfill\cr}$}
\def\maltt #1 #2 #3
{$\displaystyle\!\matrix{\strut\hbox{#1}\hfill\cr\hbox{#2}\hfill\cr
   \strut\hbox{#3}\hfill\cr}$}
\yskip@-any@>@-z@>@-g@>@-a@>@-x@>@-y@>@-f@>
@%@$dindent {CTWILL}110 =\T{\~226/}@>
\prodno=61 \midcol=2.5in
\def\theprodno{\number\prodno \global\advance\prodno by1\enspace}
\def\dagit{\dag\theprodno}
\def\+#1&#2&#3&#4\cr{\def\next{#1}%
 \line{\hbox to 2em{\hss
  \ifx\next\empty\theprodno\else\next\fi}\strut
  \ignorespaces#2\hfil\hbox to\midcol{$\RA$
  \ignorespaces#3\hfil}\quad \hbox to1.45in{\ignorespaces#4\hfil}}}
\advance\midcol20pt
@z

Section 122.

@x l.211
\+& |prerangle| & |binop| \hfill \.> & $>$ not in template\cr
@y
\+& |prerangle| & |binop| \hfill \.> & $>$ not in template\cr
\endgroup

@r @ Cont.
\begingroup \lineskip=4pt
\def\alt #1 #2
{$\displaystyle\Bigl\{\!\matrix{\strut\hbox{#1}\cr
   \strut\hbox{#2}\cr}\!\Bigr\}$ }
\def\altt #1 #2 #3
{$\displaystyle\Biggl\{\!\matrix{\strut\hbox{#1}\cr\hbox{#2}\cr
   \strut\hbox{#3}\cr}\!\Biggr\}$ }
\def\malt #1 #2
{$\displaystyle\!\matrix{\strut\hbox{#1}\hfill\cr\strut\hbox{#2}\hfill\cr}$}
\def\maltt #1 #2 #3
{$\displaystyle\!\matrix{\strut\hbox{#1}\hfill\cr\hbox{#2}\hfill\cr
   \strut\hbox{#3}\hfill\cr}$}
\yskip@-any@>@-z@>@-g@>@-a@>
\prodno=86 \midcol=2.5in
\def\theprodno{\number\prodno \global\advance\prodno by1\enspace}
\def\dagit{\dag\theprodno}
\def\+#1&#2&#3&#4\cr{\def\next{#1}%
 \line{\hbox to 2em{\hss
  \ifx\next\empty\theprodno\else\next\fi}\strut
  \ignorespaces#2\hfil\hbox to\midcol{$\RA$
  \ignorespaces#3\hfil}\quad \hbox to1.45in{\ignorespaces#4\hfil}}}
@z

Section 123.

@x l.232
\+\dagit& |new_exp| & |exp| & |new int;|\cr
@y
\+\dagit& |new_exp| & |exp| & |new int;|\cr
\endgroup

@ Cont.
@-deprecated@>
@-fallthrough@>
@-likely@>
@-nodiscard@>
@-unlikely@>
@-s@>
\begingroup \lineskip=4pt
\def\alt #1 #2
{$\displaystyle\Bigl\{\!\matrix{\strut\hbox{#1}\cr
   \strut\hbox{#2}\cr}\!\Bigr\}$ }
\def\altt #1 #2 #3
{$\displaystyle\Biggl\{\!\matrix{\strut\hbox{#1}\cr\hbox{#2}\cr
   \strut\hbox{#3}\cr}\!\Biggr\}$ }
\def\malt #1 #2
{$\displaystyle\!\matrix{\strut\hbox{#1}\hfill\cr\strut\hbox{#2}\hfill\cr}$}
\def\maltt #1 #2 #3
{$\displaystyle\!\matrix{\strut\hbox{#1}\hfill\cr\hbox{#2}\hfill\cr
   \strut\hbox{#3}\hfill\cr}$}
\yskip@-any_other@>@-z@>@-f@>@-x@>@-p@>
\prodno=100 \midcol=2.5in
\def\theprodno{\number\prodno \global\advance\prodno by1\enspace}
\def\dagit{\dag\theprodno}
\def\+#1&#2&#3&#4\cr{\def\next{#1}%
 \line{\hbox to 2em{\hss
  \ifx\next\empty\theprodno\else\next\fi}\strut
  \ignorespaces#2\hfil\hbox to\midcol{$\RA$
  \ignorespaces#3\hfil}\quad \hbox to1.45in{\ignorespaces#4\hfil}}}
\advance\midcol20pt
@z

@x
\advance\midcol-3pt
\+\dag200\enspace& |typedef_like| |decl_head| \alt|exp| |int_like| &
      |typedef_like| |decl_head| \hfill $D=D$\alt $E^{**}$ $I^{**}$ \unskip &
          \&{typedef} \&{char} \&{ch};\cr
\advance\midcol+3pt
\+201\enspace& |typedef_like| |decl_head| |semi| & |decl| \hfill $T\.\ D$ &
                                             \&{typedef} \&{int} $\&x,\&y$;\cr
\+\dag202\enspace& |typedef_like| |int_like| |raw_int| & |typedef_like| |int_like| |exp| &
  \&{typedef} \&{int} \&{foo}\cr
@y
@z

Section 124.

@x l.291
\+& |exp| |attr| & |attr| \hfill $E\.\ A$ & \&{enum} $\{x\ [[\ldots]]\}$ \cr
@y
\+& |exp| |attr| & |attr| \hfill $E\.\ A$ & \&{enum} $\{x\ [[\ldots]]\}$ \cr
\endgroup

@ Cont.
@-deprecated@>
@-fallthrough@>
@-likely@>
@-nodiscard@>
@-unlikely@>
@-y@>
\begingroup \lineskip=4pt
\def\alt #1 #2
{$\displaystyle\Bigl\{\!\matrix{\strut\hbox{#1}\cr
   \strut\hbox{#2}\cr}\!\Bigr\}$ }
\def\altt #1 #2 #3
{$\displaystyle\Biggl\{\!\matrix{\strut\hbox{#1}\cr\hbox{#2}\cr
   \strut\hbox{#3}\cr}\!\Biggr\}$ }
\def\malt #1 #2
{$\displaystyle\!\matrix{\strut\hbox{#1}\hfill\cr\strut\hbox{#2}\hfill\cr}$}
\def\maltt #1 #2 #3
{$\displaystyle\!\matrix{\strut\hbox{#1}\hfill\cr\hbox{#2}\hfill\cr
   \strut\hbox{#3}\hfill\cr}$}
\yskip@-any_other@>@-z@>@-f@>@-x@>@-p@>
\prodno=143 \midcol=2.5in
\def\theprodno{\number\prodno \global\advance\prodno by1\enspace}
\def\dagit{\dag\theprodno}
\def\+#1&#2&#3&#4\cr{\def\next{#1}%
 \line{\hbox to 2em{\hss
  \ifx\next\empty\theprodno\else\next\fi}\strut
  \ignorespaces#2\hfil\hbox to\midcol{$\RA$
  \ignorespaces#3\hfil}\quad \hbox to1.45in{\ignorespaces#4\hfil}}}
\advance\midcol20pt
@z

Section 125.

@x l.326
\+& |alignas_like| |cast| & |attr| & |alignas(int)| \cr
\vfill\break
\parindent=0pt
\everypar={\hangindent=2em}
\dag{\bf Notes}
@y
\+& |alignas_like| |cast| & |attr| & |alignas(int)| \cr
\advance\midcol-3pt
\+\dag200\enspace& |typedef_like| |decl_head| \alt|exp| |int_like| &
      |typedef_like| |decl_head| \hfill $D=D$\alt $E^{**}$ $I^{**}$ \unskip &
          \&{typedef} \&{char} \&{ch};\cr
\advance\midcol+3pt
\+201\enspace& |typedef_like| |decl_head| |semi| & |decl| \hfill $T\.\ D$ &
                                             \&{typedef} \&{int} $\&x,\&y$;\cr
\+\dag202\enspace& |typedef_like| |int_like| |raw_int| & |typedef_like| |int_like| |exp| &
  \&{typedef} \&{int} \&{foo}\cr
\endgroup

@-any_other@>@-z@>@ \begingroup\dag{\bf Notes}
\advance \hsize by -4cm
\parindent=0pt
\everypar={\hangindent=2em}
@z

Section 126.

@x
@* Implementing the productions.
@y
@* Implementing the productions.\advance \hsize by -4cm
\ifx\undefined\pdfpagewidth \else \advance \pdfpagewidth by -4cm \fi
@z

@x
the category codes |pp->cat,@,@,(pp+1)->cat|$,\,\,\ldots\,$
@y
the category codes |pp->cat|, |(pp+1)->cat|, $\,\ldots\,$
@z

Section 127.

@x
@ @d trans trans_plus.Trans /* translation texts of scraps */
@y
@ @d trans trans_plus.Trans /* translation texts of scraps */
@-trans@>
@$trans {CTWILL}127 =\\{trans\_plus}.\\{Trans}@>
@z

Section 128.

@x
null_scrap.trans=&tok_start[0];
scrap_base=scrap_info+1;
@y
null_scrap.trans=&tok_start[0];@/
scrap_base=scrap_info+1;@/
@z

Section 129.

@x
@ Token lists in |@!tok_mem| are composed of the following kinds of
@y
@ Token lists in |@!tok_mem| are composed of the following kinds of
@-p@>
@-res_flag@>
@-section_flag@>
@-tok_flag@>
@-inner_tok_flag@>
@$res_flag {CTWILL}129 =$\T{2}*\\{id\_flag}$@>
@$section_flag {CTWILL}129 =$\T{3}*\\{id\_flag}$@>
@$tok_flag {CTWILL}129 =$\T{3}*\\{id\_flag}$@>
@$inner_tok_flag {CTWILL}129 =$\T{4}*\\{id\_flag}$@>
@z

Section 132.

@x
@ The production rules listed above are embedded directly into \.{CWEAVE},
@y
@ The production rules listed above are embedded directly into \.{CWEAVE},
@-a@>
@-c@>
@-d@>
@-j@>
@-k@>
@-n@>
@-pp@>
@z

@x
example, `|squash(pp,3,exp,-2,3)|' is an abbreviation for `|big_app3(pp);
reduce(pp,3,exp,-2,3)|'.
@y
example, `|squash(pp,3,exp,-2,3)|' is an abbreviation for `|big_app3(pp)|'
followed by `|reduce(pp,3,exp,-2,3)|'.
@z

Section 133.

@x
The code below is an exact translation of the production rules into
@y
@ The code below is an exact translation of the production rules into
@-a@>
@-c@>
@-p@>
@z

Section 136--137.

@x
@ Let us consider the big switch for productions now, before looking
@y
@r @ Let us consider the big switch for productions now, before looking
@-cat1@>
@-cat2@>
@-cat3@>
@$cat1 {CTWILL}136 =$(\\{pp}+\T{1})\MG\\{cat}$@>
@$cat2 {CTWILL}136 =$(\\{pp}+\T{2})\MG\\{cat}$@>
@$cat3 {CTWILL}136 =$(\\{pp}+\T{3})\MG\\{cat}$@>
@z

@x
        && pp->cat!=new_exp
        && pp->cat!=ftemplate @|
@y
        && pp->cat!=new_exp @|
        && pp->cat!=ftemplate
@z

@x
        && pp->cat!=const_like
@y
        && pp->cat!=const_like @|
@z

@x
  switch (pp->cat) {
    case exp: @<Cases for |exp|@>@; @+break;
    case lpar: @<Cases for |lpar|@>@; @+break;
    case unop: @<Cases for |unop|@>@; @+break;
    case ubinop: @<Cases for |ubinop|@>@; @+break;
    case binop: @<Cases for |binop|@>@; @+break;
    case cast: @<Cases for |cast|@>@; @+break;
    case sizeof_like: @<Cases for |sizeof_like|@>@; @+break;
    case int_like: @<Cases for |int_like|@>@; @+break;
    case public_like: @<Cases for |public_like|@>@; @+break;
    case colcol: @<Cases for |colcol|@>@; @+break;
    case decl_head: @<Cases for |decl_head|@>@; @+break;
    case decl: @<Cases for |decl|@>@; @+break;
    case base: @<Cases for |base|@>@; @+break;
    case struct_like: @<Cases for |struct_like|@>@; @+break;
    case struct_head: @<Cases for |struct_head|@>@; @+break;
    case fn_decl: @<Cases for |fn_decl|@>@; @+break;
    case function: @<Cases for |function|@>@; @+break;
    case lbrace: @<Cases for |lbrace|@>@; @+break;
    case if_like: @<Cases for |if_like|@>@; @+break;
    case else_like: @<Cases for |else_like|@>@; @+break;
    case else_head: @<Cases for |else_head|@>@; @+break;
    case if_clause: @<Cases for |if_clause|@>@; @+break;
    case if_head: @<Cases for |if_head|@>@; @+break;
    case do_like: @<Cases for |do_like|@>@; @+break;
    case case_like: @<Cases for |case_like|@>@; @+break;
    case catch_like: @<Cases for |catch_like|@>@; @+break;
    case tag: @<Cases for |tag|@>@; @+break;
    case stmt: @<Cases for |stmt|@>@; @+break;
    case semi: @<Cases for |semi|@>@; @+break;
    case lproc: @<Cases for |lproc|@>@; @+break;
    case section_scrap: @<Cases for |section_scrap|@>@; @+break;
    case insert: @<Cases for |insert|@>@; @+break;
    case prelangle: @<Cases for |prelangle|@>@; @+break;
    case prerangle: @<Cases for |prerangle|@>@; @+break;
    case langle: @<Cases for |langle|@>@; @+break;
    case template_like: @<Cases for |template_like|@>@; @+break;
    case new_like: @<Cases for |new_like|@>@; @+break;
    case new_exp: @<Cases for |new_exp|@>@; @+break;
    case ftemplate: @<Cases for |ftemplate|@>@; @+break;
    case for_like: @<Cases for |for_like|@>@; @+break;
    case raw_ubin: @<Cases for |raw_ubin|@>@; @+break;
    case const_like: @<Cases for |const_like|@>@; @+break;
    case raw_int: @<Cases for |raw_int|@>@; @+break;
    case operator_like: @<Cases for |operator_like|@>@; @+break;
    case typedef_like: @<Cases for |typedef_like|@>@; @+break;
    case delete_like: @<Cases for |delete_like|@>@; @+break;
    case question: @<Cases for |question|@>@; @+break;
    case alignas_like: @<Cases for |alignas_like|@>@; @+break;
    case lbrack: @<Cases for |lbrack|@>@; @+break;
    case attr_head: @<Cases for |attr_head|@>@; @+break;
    case attr: @<Cases for |attr|@>@; @+break;
    case default_like: @<Cases for |default_like|@>@; @+break;
  }
pp++; /* if no match was found, we move to the right */
@y
  switch (pp->cat) {
    @<Cases for |pp->cat|@>@;
  }
pp++; /* if no match was found, we move to the right */

@ @<Cases for |pp->cat|@>=@t\1\5\5@>
    case exp: @<Cases for |exp|@>@; @+break;
    case lpar: @<Cases for |lpar|@>@; @+break;
    case unop: @<Cases for |unop|@>@; @+break;
    case ubinop: @<Cases for |ubinop|@>@; @+break;
    case binop: @<Cases for |binop|@>@; @+break;
    case cast: @<Cases for |cast|@>@; @+break;
    case sizeof_like: @<Cases for |sizeof_like|@>@; @+break;
    case int_like: @<Cases for |int_like|@>@; @+break;
    case public_like: @<Cases for |public_like|@>@; @+break;
    case colcol: @<Cases for |colcol|@>@; @+break;
    case decl_head: @<Cases for |decl_head|@>@; @+break;
    case decl: @<Cases for |decl|@>@; @+break;
    case base: @<Cases for |base|@>@; @+break;
    case struct_like: @<Cases for |struct_like|@>@; @+break;
    case struct_head: @<Cases for |struct_head|@>@; @+break;
    case fn_decl: @<Cases for |fn_decl|@>@; @+break;
    case function: @<Cases for |function|@>@; @+break;
    case lbrace: @<Cases for |lbrace|@>@; @+break;
    case if_like: @<Cases for |if_like|@>@; @+break;
    case else_like: @<Cases for |else_like|@>@; @+break;
    case else_head: @<Cases for |else_head|@>@; @+break;
    case if_clause: @<Cases for |if_clause|@>@; @+break;
    case if_head: @<Cases for |if_head|@>@; @+break;
    case do_like: @<Cases for |do_like|@>@; @+break;
    case case_like: @<Cases for |case_like|@>@; @+break;
    case catch_like: @<Cases for |catch_like|@>@; @+break;
    case tag: @<Cases for |tag|@>@; @+break;
    case stmt: @<Cases for |stmt|@>@; @+break;
    case semi: @<Cases for |semi|@>@; @+break;
    case lproc: @<Cases for |lproc|@>@; @+break;
    case section_scrap: @<Cases for |section_scrap|@>@; @+break;
    case insert: @<Cases for |insert|@>@; @+break;
    case prelangle: @<Cases for |prelangle|@>@; @+break;
    case prerangle: @<Cases for |prerangle|@>@; @+break;
    case langle: @<Cases for |langle|@>@; @+break;
    case template_like: @<Cases for |template_like|@>@; @+break;
    case new_like: @<Cases for |new_like|@>@; @+break;
    case new_exp: @<Cases for |new_exp|@>@; @+break;
    case ftemplate: @<Cases for |ftemplate|@>@; @+break;
    case for_like: @<Cases for |for_like|@>@; @+break;
    case raw_ubin: @<Cases for |raw_ubin|@>@; @+break;
    case const_like: @<Cases for |const_like|@>@; @+break;
    case raw_int: @<Cases for |raw_int|@>@; @+break;
    case operator_like: @<Cases for |operator_like|@>@; @+break;
    case typedef_like: @<Cases for |typedef_like|@>@; @+break;
    case delete_like: @<Cases for |delete_like|@>@; @+break;
    case question: @<Cases for |question|@>@; @+break;
    case alignas_like: @<Cases for |alignas_like|@>@; @+break;
    case lbrack: @<Cases for |lbrack|@>@; @+break;
    case attr_head: @<Cases for |attr_head|@>@; @+break;
    case attr: @<Cases for |attr|@>@; @+break;
    case default_like: @<Cases for |default_like|@>@; @+break;
@z

Section 138.

@x
should say, for example, `\.{@@!@@\^\\\&\{operator\} \$+\{=\}\$@@>}' (or,
more properly alpha\-betized,
`\.{@@!@@:operator+=\}\{\\\&\{operator\} \$+\{=\}\$@@>}').
@y
should say, for example, `\.{@@!@@\^\\\&\{operator\}} \.{\$+\{=\}\$@@>}'
(or, properly alpha\-betized,
`\.{@@!@@:operator+=\}\{\\\&\{operator\}} \.{\$+\{=\}\$@@>}').
@-find_first_ident@>
@-make_reserved@>
@-make_underlined@>
@-underline_xref@>
@-no_ident_found@>
@-case_found@>
@-operator_found@>
@$no_ident_found {CTWILL}138 =\hfil\break(\&{token\_pointer}) 0@>
@$case_found {CTWILL}138 =\hfil\break(\&{token\_pointer}) 1@>
@$operator_found {CTWILL}138 =\hfil\break(\&{token\_pointer}) 2@>
@z

Section 140.

@x
the |for| loop below.

We use the fact that |make_underlined| has been called immediately preceding
|make_reserved|, hence |tok_loc| has been set.
@y
the |for| loop below.
We use the fact that |make_underlined| has been called immediately preceding
|make_reserved|, hence |tok_loc| has been set.
@z

@x
make_reserved( /* make the first identifier in |p->trans| like |int| */
scrap_pointer p)
@y
make_reserved( /* make the first identifier in |p->trans| like |int| */
scrap_pointer p)
@-make_reserved@>
@$make_reserved {CTWILL}140 \&{static} \&{void} (\,)@>
@z

Section 141.

@x
@ In the following situations we want to mark the occurrence of
@y
@r @ In the following situations we want to mark the occurrence of
@z

@x
make_underlined(
/* underline the entry for the first identifier in |p->trans| */
scrap_pointer p)
@y
make_underlined(
/* underline the entry for the first identifier in |p->trans| */
scrap_pointer p)
@-make_underlined@>
@$make_underlined {CTWILL}141 \&{static} \&{void} (\,)@>
@z

@x
    return; /* this happens, for example, in |case found:| */
@y
    return; /* this happens, for example, in \&{case} \\{found}: */
@z

Section 144.

@x
    strcpy(ministring_buf,"label");
@y
    strcpy(ministring_buf,"label");@/
@z

@x
else if (cat1==attr) {
@y
else @/ if (cat1==attr) {
@z

Section 158.

@x
@ @<Cases for |struct_head|@>=
@y
@r @ @<Cases for |struct_head|@>=
@z

Section 165.

@x
  else reduce(pp,0,else_like,0,65);
}
else if (cat1==attr) {
@y
  else reduce(pp,0,else_like,0,65);
}
@+ else @/ if (cat1==attr) {
@z

Section 171.

@x
@d force_lines flags['f'] /* should each statement be on its own line? */
@y
@d force_lines flags['f'] /* should each statement be on its own line? */
@-force_lines@>
@$force_lines {CTWILL}171 =\\{flags}[\.{'f'}]@>
@z

Section 174.

@x
if (cat1==else_like || cat1==if_like || cat1==define_like)
@y
if (cat1==else_like || cat1==if_like || cat1==define_like) @/
@z

@x
} else if (cat1==exp || cat1==function) {
@y
} else @/ if (cat1==exp || cat1==function) {
@z

Section 182.

@x
  big_app1(pp+2); reduce(pp,3,new_exp,0,96);
}
else if (cat1==raw_ubin) {
@y
  big_app1(pp+2); reduce(pp,3,new_exp,0,96);
}
@+ else @/ if (cat1==raw_ubin) {
@z

Section 198.

@x
@ Now here's the |reduce| procedure used in our code for productions,
@y
@r @ Now here's the |reduce| procedure used in our code for productions,
@z

Section 203.

@x
@ @<Print a snapsh...@>=
@y
@ @<Print a snapsh...@>=
@-n@>
@%
@$n {CTWILL}198 \&{short}@>
@$n {CTWILL}199 \&{short}@>
@z

Section 205.

@x
@ @<Predecl...@>=@+static text_pointer translate(void);
@y
@ @<Predecl...@>=@+static text_pointer translate(void);
@-translate@>
@z

Section 209.

@x
    if (next_control=='|' || next_control==begin_comment ||
        next_control==begin_short_comment) return;
@y
@-C_parse@>
@$C_parse {CTWILL}209 \&{static} \&{void} (\,)@>
    if (next_control=='|' || next_control==begin_comment @| ||
        next_control==begin_short_comment) return;
@z

Section 210.

@x
@ @<Predecl...@>=@+static void C_parse(eight_bits);
@y
@ @<Predecl...@>=@+static void C_parse(eight_bits);
@-C_parse@>
@z

Section 211.

@x
@ The following macro is used to append a scrap whose tokens have just
@y
@ The following macro is used to append a scrap whose tokens have just
@-b@>
@-c@>
@z

Section 212.

@x
@ @<Append the scr...@>=
@<Make sure that there is room for the new scraps, tokens, and texts@>@;
@y
@ @<Append the scr...@>=
@#
@<Make sure that there is room for the new scraps, tokens, and texts@>@;
@#
@z

@x
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
  case '(': app(next_control);@+app_scrap(lpar,maybe_math);@+break;
  case ')': app(next_control);@+app_scrap(rpar,maybe_math);@+break;
  case '[': app(next_control);@+app_scrap(lbrack,maybe_math);@+break;
  case ']': app(next_control);@+app_scrap(rbrack,maybe_math);@+break;
  case '{': app_str("\\{"@q}@>);@+app_scrap(lbrace,yes_math);@+break;
@.\\\{@>@q}@>
  case '}': app_str(@q{@>"\\}");@+app_scrap(rbrace,yes_math);@+break;
@q{@>@.\\\}@>
  case ',': app(',');@+app_scrap(comma,yes_math);@+break;
  case ';': app(';');@+app_scrap(semi,maybe_math);@+break;
  case ':': app(':');@+app_scrap(colon,no_math);@+break;@/
  @t\4@> @<Cases involving nonstandard characters@>@;
@y
  case ignore: case xref_roman: case xref_wildcard: case meaning: @/
  case suppress: case xref_typewriter: case noop:@+break;
@#
  @t\4@> @<Cases for operators and syntax markers@>@;
  @t\4@> @<Cases involving nonstandard characters@>@;
@#
@z

Section 213--214.

@x
  overflow(_("scrap/token/text"));
}
@y
  overflow(_("scrap/token/text"));
}

@ @<Cases for operators...@>=@t\1\5\5@>
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
  case '(': app(next_control);@+app_scrap(lpar,maybe_math);@+break;
  case ')': app(next_control);@+app_scrap(rpar,maybe_math);@+break;
  case '[': app(next_control);@+app_scrap(lbrack,maybe_math);@+break;
  case ']': app(next_control);@+app_scrap(rbrack,maybe_math);@+break;
  case '{': app_str("\\{"@q}@>);@+app_scrap(lbrace,yes_math);@+break;
@.\\\{@>@q}@>
  case '}': app_str(@q{@>"\\}");@+app_scrap(rbrace,yes_math);@+break;
@q{@>@.\\\}@>
  case ',': app(',');@+app_scrap(comma,yes_math);@+break;
  case ';': app(';');@+app_scrap(semi,maybe_math);@+break;
  case ':': app(':');@+app_scrap(colon,no_math);@+break;@/
@z

Section 221.

@x
@d make_pb flags['e']
@y
@d make_pb flags['e']
@-make_pb@>
@$make_pb {CTWILL}221 =\\{flags}[\.{'e'}]@>
@z

Section 222.

@x
@ @c
@y
@r @ @c
@z

Section 225.

@x
@d cur_mode cur_state.mode_field /* current mode of interpretation */
@y
@d cur_mode cur_state.mode_field /* current mode of interpretation */
@-cur_end@>
@-cur_tok@>
@-cur_mode@>
@$cur_end {CTWILL}225 =\\{cur\_state}.\\{end\_field}]@>
@$cur_tok {CTWILL}225 =\\{cur\_state}.\\{tok\_field}]@>
@$cur_mode {CTWILL}225 =\\{cur\_state}.\\{mode\_field}]@>
@z

Section 228.

@x
push_level( /* suspends the current level */
text_pointer p)
@y
push_level( /* suspends the current level */
text_pointer p)
@-push_level@>
@$push_level {CTWILL}228 \&{static} \&{void} (\,)@>
@z

Section 229.

@x
force when the current level was begun. This subroutine will never be
called when |stack_ptr==1|.
@y
force when the current level was begun.

This subroutine will never be called when |stack_ptr==1|.
@z

Section 234--235.

@x
    reswitch: switch(a) {
      case end_translation: return;
      case identifier: case res_word: @<Output an identifier@>@; break;
      case section_code: @<Output a section name@>@; break;
      case math_rel: out_str("\\MRL{"@q}@>); /* fall through */
@.\\MRL@>
      case noop: case inserted: break;
      case cancel: case big_cancel: c=0; b=a;
        while (true) {
          a=get_output();
          if (a==inserted) continue;
          if ((a<indent && !(b==big_cancel&&a==' ')) @|
            || (a>big_force && a!=dindent)) break;
          switch (a) {
          case indent: c++; break;
          case outdent: c--; break;
          case dindent: c+=2; break;
          case opt: a=get_output();
          }
        }
        @<Output saved |indent| or |outdent| tokens@>@;
        goto reswitch;
      case dindent: a=get_output();
        if (a!=big_force) {
          out_str("\\1\\1"); goto reswitch;
        }
        else dindent_pending=true; /* fall through */
      case indent: case outdent: case opt: case backup: case break_space:
      case force: case big_force: case preproc_line: @<Output a control,
        look ahead in case of line breaks, possibly |goto reswitch|@>@; break;
      case quoted_char: out(*(cur_tok++)); /* fall through */
      case qualifier: break;
      default: out(a); /* otherwise |a| is an ordinary character */
    }
  }
}
@y
    @/@<The output |switch|@>@;
  }
}

@ @<The output |switch|@>=
    reswitch: switch(a) {
      case end_translation: return;
      case identifier: case res_word: @<Output an identifier@>@; break;
      case section_code: @<Output a section name@>@; break;
      case math_rel: out_str("\\MRL{"@q}@>); /* fall through */
@.\\MRL@>
      case noop: case inserted: break;
      case cancel: case big_cancel: c=0; b=a;
        while (true) {
          a=get_output();
          if (a==inserted) continue;
          if ((a<indent && !(b==big_cancel&&a==' ')) @|
            || (a>big_force && a!=dindent)) break;
          switch (a) {
          case indent: c++; break;
          case outdent: c--; break;
          case dindent: c+=2; break;
          case opt: a=get_output();
          }
        }
        @<Output saved |indent| or |outdent| tokens@>@;
        goto reswitch;
      case dindent: a=get_output();
        if (a!=big_force) {
          out_str("\\1\\1"); goto reswitch;
        }
        else dindent_pending=true; /* fall through */
      case indent: case outdent: case opt: case backup: case break_space:
      case force: case big_force: case preproc_line: @<Output a control,
        look ahead in case of line breaks, possibly |goto reswitch|@>@; break;
      case quoted_char: out(*(cur_tok++)); /* fall through */
      case qualifier: break;
      default: out(a); /* otherwise |a| is an ordinary character */
    }
@z

Section 236.

@x
@ An identifier of length one does not have to be enclosed in braces, and it
@y
@r @ An identifier of length one does not have to be enclosed in braces, and it
@z

Section 237.

@x
  } else if (a==opt) b=get_output(); /* ignore digit following |opt| */
@y
  }@+ else if (a==opt) b=get_output(); /* ignore digit following |opt| */
@z

Section 238.

@x
@<Look ahead for st...@>= {
  b=a; save_mode=cur_mode;
  if (dindent_pending) {
    c=2; dindent_pending=false;
  } else c=0;
@y
@<Look ahead for st...@>= {@+
  b=a; save_mode=cur_mode;
  if (dindent_pending) {@+
    c=2; dindent_pending=false;@+
  } else c=0;
@z

@x
  while (true) {
@y
  while (true) {@+
@z

Section 241.

@x
    case ' ': case '\\': case '#': case '%': case '$': case '^':
    case '{': case '}': case '~': case '&': case '_':
      out('\\'); /* falls through */
@y
    case ' ': case '\\': case '#': case '%': @/
    case '$': case '^': case '{': case '}': @/
    case '~': case '&': case '_': @/
      out('\\'); /* falls through */
@z

Section 243.

@x
  if (b=='@@' || (b=='\\' && delim!=0))
@y
  if (b=='@@' || (b=='\\' && delim!=0)) @/
@z

Section 245.

@x
phase_two(void) {
@y
phase_two(void) {@+
@z

Section 246.

@x
@ @<Predecl...@>=@+static void phase_two(void);
@y
@ @<Predecl...@>=@+static void phase_two(void);
@-phase_two@>
@z

Section 248.

@x
@ @d usage_sentinel (struct perm_meaning *)1
@<Translate the current section@>= {
@y
@ @d usage_sentinel (struct perm_meaning *)1
@-usage_sentinel@>
@$usage_sentinel {CTWILL}248 =(\&{struct} \&{perm\_meaning} ${}{*}{}$) \T{1}@>
@<Translate the current section@>=@+ {
@z

Section 252.

@x
finish_C( /* finishes a definition or a \CEE/ part */
  boolean visible) /* |true| if we should produce \TeX\ output */
@y
finish_C( /* finishes a definition or a \CEE/ part */
  boolean visible) /* |true| if we should produce \TeX\ output */
@-finish_C@>
@$finish_C {CTWILL}252 \&{static} \&{void} (\,)@>
@z

Section 253.

@x
@ @<Predecl...@>=@+static void finish_C(boolean);
@y
@ @<Predecl...@>=@+static void finish_C(boolean);
@-finish_C@>
@z

Section 255.

@x
@ @<Start a format...@>= {
@y
@r @ @<Start a format...@>= {
@z

Section 256.

@x
|next_control>=begin_C|. We will make the global variable |this_section|
point to the current section name, if it has a name.
@y
\hfil\break|next_control>=begin_C|.
We will make the global variable |this_section| point to
\hfil\break the current section name, if it has a name.
@z

Section 261.

@x
footnote( /* outputs section cross-references */
sixteen_bits flag)
@y
footnote( /* outputs section cross-references */
sixteen_bits flag)
@-footnote@>
@$footnote {CTWILL}261 \&{static} \&{void} (\,)@>
@z

Section 262.

@x
@ @<Predecl...@>=@+static void footnote(sixteen_bits);
@y
@ @<Predecl...@>=@+static void footnote(sixteen_bits);
@-footnote@>
@z

Section 266.

@x
@ @<Predecl...@>=@+static void phase_three(void);
@y
@ @<Predecl...@>=@+static void phase_three(void);
@-phase_three@>
@z

Section 269.

@x
@ A left-to-right radix sorting method is used, since this makes it easy to
@y
@ A left-to-right radix sorting method is used, since this makes it easy to
@-c@>
@z

Section 271.

@x
@ During the sorting phase we shall use the |cat| and |trans| arrays from
@y
@r @ During the sorting phase we shall use the |cat| and |trans| arrays from
@z

@x
@<Rest of |trans_plus| union@>=
@y
@<Rest of |trans_plus| union@>=
@-j@>
@-k@>
@z

Section 272.

@x
@ @d depth cat /* reclaims memory that is no longer needed for parsing */
@y
@ @d depth cat /* reclaims memory that is no longer needed for parsing */
@-depth@>
@-head@>
@-sort_ptr@>
@-max_sorts@>
@$depth {CTWILL}272 =\\{cat}@>
@$head {CTWILL}272 =\\{trans\_plus}.\\{Head}@>
@$sort_ptr {CTWILL}272 =\\{scrap\_ptr}@>
@$max_sorts {CTWILL}272 =\\{max_scraps}@>
@z

Section 274.

@x
$|collate|[0]<|collate|[1]<\cdots<|collate|[100]$.
@y
|collate[0]| ${}<{}$|collate[1]|${}<\cdots<{}$|collate[100]|.
@z

Section 276.

@x
@ Procedure |unbucket| goes through the buckets and adds nonempty lists
@y
@r @ Procedure |unbucket| goes through the buckets and adds nonempty lists
@-unbucket@>
@$unbucket {CTWILL}276 \&{static} \&{void} (\,)@>
@z

Section 277.

@x
@ @<Predecl...@>=@+static void unbucket(eight_bits);
@y
@ @<Predecl...@>=@+static void unbucket(eight_bits);
@-unbucket@>
@z

Section 279.

@x
@ @<Split the list...@>= {
  int c;
  next_name=sort_ptr->head;
@y
@ @<Split the list...@>= {@+
  int c;
  next_name=sort_ptr->head;@/
@z

Section 280.

@x
  cur_name=sort_ptr->head;
@y
  @+cur_name=sort_ptr->head;@/
@z

Section 286.

@x
@ @<Predecl...@>=@+static void section_print(name_pointer);
@y
@ @<Predecl...@>=@+static void section_print(name_pointer);
@-section_print@>
@z

Section 288.

@x
  puts(_("\nMemory usage statistics:"));
@.Memory usage statistics:@>
  printf(_("%td names (out of %ld)\n"),
            (ptrdiff_t)(name_ptr-name_dir),(long)max_names);
  printf(_("%td cross-references (out of %ld)\n"),
            (ptrdiff_t)(xref_ptr-xmem),(long)max_refs);
  printf(_("%td bytes (out of %ld)\n"),
            (ptrdiff_t)(byte_ptr-byte_mem),(long)max_bytes);
  printf(_("%td temp meanings (out of %ld)\n"),
            (ptrdiff_t)(max_temp_meaning_ptr-temp_meaning_stack),
            (long)max_meanings);
  printf(_("%td titles (out of %ld)\n"),
            (ptrdiff_t)(title_code_ptr-title_code),(long)max_titles);
  puts(_("Parsing:"));
  printf(_("%td scraps (out of %ld)\n"),
            (ptrdiff_t)(max_scr_ptr-scrap_info),(long)max_scraps);
  printf(_("%td texts (out of %ld)\n"),
            (ptrdiff_t)(max_text_ptr-tok_start),(long)max_texts);
  printf(_("%td tokens (out of %ld)\n"),
            (ptrdiff_t)(max_tok_ptr-tok_mem),(long)max_toks);
  printf(_("%td levels (out of %ld)\n"),
            (ptrdiff_t)(max_stack_ptr-stack),(long)stack_size);
  puts(_("Sorting:"));
  printf(_("%td levels (out of %ld)\n"),
            (ptrdiff_t)(max_sort_ptr-scrap_info),(long)max_scraps);
@y
  puts(_("\nMemory usage statistics:"));@/
@.Memory usage statistics:@>
  printf(_("%td names (out of %ld)\n"),
            (ptrdiff_t)(name_ptr-name_dir),@/
            @t\5\5\5\5@>(long)max_names);@/
  printf(_("%td cross-references (out of %ld)\n"),
            (ptrdiff_t)(xref_ptr-xmem),(long)max_refs);@/
  printf(_("%td bytes (out of %ld)\n"),
            (ptrdiff_t)(byte_ptr-byte_mem),@/
            @t\5\5\5\5@>(long)max_bytes);@/
  printf(_("%td temp meanings (out of %ld)\n"),
            (ptrdiff_t)(max_temp_meaning_ptr-temp_meaning_stack),@/
            @t\5\5\5\5@>(long)max_meanings);@/
  printf(_("%td titles (out of %ld)\n"),
            (ptrdiff_t)(title_code_ptr-title_code),@/
            @t\5\5\5\5@>(long)max_titles);@/
  puts(_("Parsing:"));@/
  printf(_("%td scraps (out of %ld)\n"),
            (ptrdiff_t)(max_scr_ptr-scrap_info),@/
            @t\5\5\5\5@>(long)max_scraps);@/
  printf(_("%td texts (out of %ld)\n"),
            (ptrdiff_t)(max_text_ptr-tok_start),@/
            @t\5\5\5\5@>(long)max_texts);@/
  printf(_("%td tokens (out of %ld)\n"),
            (ptrdiff_t)(max_tok_ptr-tok_mem),@/
            @t\5\5\5\5@>(long)max_toks);@/
  printf(_("%td levels (out of %ld)\n"),
            (ptrdiff_t)(max_stack_ptr-stack),@/
            @t\5\5\5\5@>(long)stack_size);@/
  puts(_("Sorting:"));@/
  printf(_("%td levels (out of %ld)\n"),
            (ptrdiff_t)(max_sort_ptr-scrap_info),@/
            @t\5\5\5\5@>(long)max_scraps);
@z

Section 289.

@x
@** Mogrify {\tentex CWEAVE} into {\tentex CTWILL}.  The following sections
introduce material that is specific to \.{CTWILL}.

Care has been taken to keep the original section numbering of \.{CWEAVE}
up to this point intact, so this new material should nicely integrate
with the original ``\&{271.~Index}.''

@q Section 2->272. @>
@* {\tentex CTWILL} user manual.
@y
@** Mogrify {\tentex CWEAVE} into {\tentex CTWILL}.
@-s@>
@z

@x
give it the necessary hints in other places via your change file.
@y
give it the necessary hints in other places via your change file.
@-f@>
@-x@>
@z

Section 290--291.

@x
The current meaning of every identifier is initially `\.{\\uninitialized}'.
@y
@r @ The current meaning of every identifier is initially `\.{\\uninitialized}'.
@-printf@>
@z

@x
must have fewer than 50 characters. If the \TeX\ part starts
@y
must have fewer than 50 characters.

@d max_tex_chars 50 /* limit on the \TeX\ part of a meaning */
@-s@>

@ If the \TeX\ part starts
@z

@x
@d max_tex_chars 50 /* limit on the \TeX\ part of a meaning */
@y
@z

Section 293.

@x
static struct perm_meaning {
@y
static struct perm_meaning {
@-perm_meaning@>
@$perm_meaning {CTWILL}293 \&{static} \&{struct}@>
@z

Section 302.

@x
@ @<Predec...@>=@+static boolean app_supp(text_pointer);
@y
@ @<Predec...@>=@+static boolean app_supp(text_pointer);
@-app_supp@>
@z

Section 303.

@x
@ The trickiest part of \.{CTWILL} is the procedure |make_ministring(pp+l)|,
@y
@r @ The trickiest part of \.{CTWILL} is the procedure |make_ministring(pp+l)|,
@-a@>
@-b@>
@-c@>
@-l@>
@-make_ministring@>
@$make_ministring {CTWILL}303 \&{static} \&{void} (\,)@>
@z

Section 304.

@x
@ @<Predec...@>=@+static void make_ministring(scrap_pointer);
@y
@ @<Predec...@>=@+static void make_ministring(scrap_pointer);
@-make_ministring@>
@z

Section 307.

@x
@ @<Append tokens for type |q|@>=
@y
@r @ @<Append tokens for type |q|@>=
@z

@x
   && *(*r+1)=='{') app(**q); /* |struct_like| identifier */
@y
   @|&& *(*r+1)=='{') app(**q); /* |struct_like| identifier */
@z

Section 312.

@x
@ @<Write the new meaning to the \.{.aux} file@>=
@y
@ @<Write the new meaning to the \.{.aux} file@>=
@%
@$p {CTWILL}296 \&{name\_pointer}@>
@$q {CTWILL}296 \&{struct perm\_meaning} ${}{*}{}$@>
@z

Section 313.

@x
@<Flag the usage of this identifier, for the mini-index@>=
@y
@<Flag the usage of this identifier, for the mini-index@>=
@%
@$p {CTWILL}219 \&{name\_pointer}@>
@z

Section 316.

@x
@ @<Predec...@>=@+static void out_mini(meaning_struct *);
@y
@ @<Predec...@>=@+static void out_mini(meaning_struct *);
@-out_mini@>
@z

Section 317.

@x
@ Compare this code with section |@<Output the name...@>|.

@<Mini-output...@>=
@y
@ @<Mini-output...@>=
@z

Section 319.

@x
@ @<Predec...@>=@+static sixteen_bits title_lookup(void);
@y
@ @<Predec...@>=@+static sixteen_bits title_lookup(void);
@-title_lookup@>
@z

Section 322.

@x
@d indent_param_decl flags['i'] /* should formal parameter declarations be indented? */
@y
@d indent_param_decl flags['i'] /* should formal parameter declarations be indented? */
@-indent_param_decl@>
@$indent_param_decl {CTWILL}322 =\\{flags}[\.{'i'}]@>
@z

Section 323.

@x
@d order_decl_stmt flags['o'] /* should declarations and statements be separated? */
@y
@d order_decl_stmt flags['o'] /* should declarations and statements be separated? */
@-order_decl_stmt@>
@$order_decl_stmt {CTWILL}323 =\\{flags}[\.{'o'}]@>
@z

Section 329.

@x
@** Index.
If you have read and understood the code for Phase III above, you know what
is in this index and how it got here. All sections in which an identifier is
used are listed with that identifier, except that reserved words are
indexed only when they appear in format definitions, and the appearances
of identifiers in section names are not indexed. Underlined entries
correspond to where the identifier was declared. Error messages, control
sequences put into the output, and a few
other things like ``recursion'' are indexed here too.
@y
@z
