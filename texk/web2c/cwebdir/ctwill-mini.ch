Section 1.

@x
@** Introduction.
@y
\ifacro \ifx\undefined\pdfpagewidth\else
  \pdfpagewidth=\pagewd  \advance\pdfpagewidth by 2cm
  \pdfpageheight=\pageht \advance\pdfpageheight by 5cm
  \ifpdftex \pdfhorigin=1cm \pdfvorigin=1cm
  \else \global\hoffset=-1.54cm \global\voffset=-1.54cm \fi
\fi\fi

@** Introduction.
@z

@x
@d banner "This is CTWILL, Version 3.64"
@y
\bigskip
{\font\itt=cmitt10 \font\bit=cmbxti10
\noindent \bit Editor's Note: \it Although {\itt CTWILL} is based on
{\itt cweave.w}, new and modified material is incorporated all over the place,
without taking special care to keep the original section numbering intact.
\smallskip
\noindent Moreover, this heavily redacted version of {\itt ctwill.pdf} had to
meddle with the section numbering even more, spreading tabular material over
several sections and splitting long sections into smaller chunks in order to
fix overful pages---both horizontally and vertically---, to make the overall
appearance of the {\itt CTWILL} documentation most pleasing to the
readers'~eyes.
\smallskip
\noindent Please do not try to compare this {\itt ctwill.pdf} to the one
created by {\itt CWEAVE} instead of {\itt CTWILL}; the section numbering will
be even more ``off'' from {\itt cweave.w}.  Care has been taken to give a
faithful overall rendering of {\itt CTWILL}'s code, though. \hfill
---Enjoy!\bigskip}

@d banner "This is CTWILL, Version 3.64"
@z

Section 3.

@x
@ Here is a sort of user manual for \.{CTWILL}---which is exactly like
@y
@* \.{CTWILL} user manual.
Here is a sort of user manual for \.{CTWILL}---which is exactly like
@z

@x
give it the necessary hints in other places via your change file.
@y
give it the necessary hints in other places via your change file.
@-f@>
@-x@>
@z

Section 4.

@x
The current meaning of every identifier is initially `\.{\\uninitialized}'.
@y
@ The current meaning of every identifier is initially `\.{\\uninitialized}'.
@z

@x
must have fewer than 50 characters. If the \TeX\ part starts
@y
must have fewer than 50 characters.

@d max_tex_chars 50 /* limit on the \TeX\ part of a meaning */

@ If the \TeX\ part starts
@z

Section 5.

@x
@d max_tex_chars 50 /* limit on the \TeX\ part of a meaning */
@y
@z

Section 7.

@x
you have to change them also in the file |"common.w"|.
@y
you have to change them also in file |"common.w"|.
@z

Section 8.

@x
@ The next few sections contain stuff from the file |"common.w"| that must
@y
@r @ The next few sections contain stuff from the file |"common.w"| that must
@z

Section 9.

@x
internationalization.
@y
internationalization.
@-A@>
@-HAVE_GETTEXT@>
@-S@>
@z

Section 11.

@x
@ Code related to input routines:
@y
@ Code related to input routines:
@-c@>
@z

Section 12.

@x
@ Code related to identifier and section name storage:
@y
@ Code related to identifier and section name storage:
@-c@>
@z

Section 13.

@x
@ Code related to error handling:
@y
@ Code related to error handling:
@-s@>
@z

Section 15.

@x
@ Code related to section numbers:
@y
@r @ Code related to section numbers:
@z

Section 17.

@x
@ Code relating to output:
@y
@ Code relating to output:
@-a@>
@-b@>
@-c@>
@z

Section 19.

@x
@* Data structures exclusive to {\tt CWEAVE}.
@y
@* Data structures exclusive to {\tt CWEAVE}.
@-a@>
@z

Section 21.

@x
@ The other large memory area in \.{CWEAVE} keeps the cross-reference data.
@y
@ The other large memory area in \.{CWEAVE} keeps the cross-reference data.
@-p@>
@-x@>
@z

Section 23.

@x
@ \.{CTWILL} also has special data structures to keep track of current
@y
@r @ \.{CTWILL} also has special data structures to keep track of current
@z

Section 28.

@x
@ The |new_meaning| routine changes the current ``permanent meaning''
@y
@r @ The |new_meaning| routine changes the current ``permanent meaning''
@z

Section 31.

@x
@ A new cross-reference for an identifier is formed by calling |new_xref|,
@y
@ A new cross-reference for an identifier is formed by calling |new_xref|,
@-a@>
@-c@>
@z

Section 39.

@x
id_lookup("do",NULL,do_like);
@y
@ @<Store all...@>=
id_lookup("do",NULL,do_like);
@z

Section 40.

@x
id_lookup("undef",NULL,if_like);
@y
@ @<Store all...@>=
id_lookup("undef",NULL,if_like);
@z

Section 44.

@x
@ Control codes are converted to \.{CWEAVE}'s internal
@y
@r @ Control codes are converted to \.{CWEAVE}'s internal
@z

Section 50.

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

Section 52.

@x
@ As one might expect, |get_next| consists mostly of a big switch
@y
@ As one might expect, |get_next| consists mostly of a big switch
@-c@>
@z

Section 59.

@x
@ The following code assigns values to the combinations \.{++},
@y
@ The following code assigns values to the combinations \.{++},
@-c@>
@z

Section 62.

@x
@<Get a string@>= {
@y
@<Get a string@>= {@+
@z

Section 66.

@x
@ @<Put section name...@>=
@y
@r @ @<Put section name...@>=
@z

Section 80.

@x
with |next_control!='|'| and ends with |next_control>=format_code|. Thus, it
@y
with |next_control| |!='|'| and ends with |next_control>=format_code|.
Thus, it
@z

Section 88.

@x
@ Finally, when the \TEX/ and definition parts have been treated, we have
|next_control>=begin_C|.
@y
@ Finally, when the \TEX/ and definition parts have been treated, we have
\hfil\break|next_control>=begin_C|.
@z

Section 94.

@x
@ The |flush_buffer| routine empties the buffer up to a given breakpoint,
@y
@ The |flush_buffer| routine empties the buffer up to a given breakpoint,
@-c@>
@z

Section 95.

@x
@ When we are copying \TEX/ source material, we retain line breaks
@y
@r @ When we are copying \TEX/ source material, we retain line breaks
@z

Section 97.

@x
@ When we wish to append one character |c| to the output buffer, we write
@y
@ When we wish to append one character |c| to the output buffer, we write
@-c@>
@z

Section 101.

@x
@ We get to this section only in the unusual case that the entire output line
@y
@r @ We get to this section only in the unusual case that the entire output line
@z

Section 105.

@x
static eight_bits
copy_TeX(void)
{
@y
static eight_bits
copy_TeX(void)
{@+
@z

Section 106.

@x
@ The |copy_comment| function issues a warning if more braces are opened than
@y
@ The |copy_comment| function issues a warning if more braces are opened than
@-t@>
@z

Section 109.

@x
@ @<Copy special things when |c=='@@'...@>=
@y
@r @ @<Copy special things when |c=='@@'...@>=
@z

Section 112.

@x
@ Here is a list of the category codes that scraps can have.
@y
@r @ Here is a list of the category codes that scraps can have.
@z

Section 115.

@x
@ The token lists for translated \TEX/ output contain some special control
@y
@r @ The token lists for translated \TEX/ output contain some special control
@-n@>
@z

Section 116.

@x
\yskip\noindent All of these tokens are removed from the \TEX/ output that
@y
@ All of these tokens are removed from the \TEX/ output that
@-n@>
@z

Section 117.

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

Section 118.

@x
\.>&|prerangle|: \.{\\rangle}&yes\cr
@y
\.>&|prerangle|: \.{\\rangle}&yes\cr}

@ Cont.

\yskip\halign{\quad#\hfil&\quad#\hfil&\quad\hfil#\hfil\cr
%\vskip\halign{\quad#\hfil&\quad\hbox to11cm{#\hfil}&\quad\hfil#\hfil\cr
@z

Section 119.

@x
\.{continue}&|case_like|: \stars&maybe\cr
@y
\.{continue}&|case_like|: \stars&maybe\cr}

@r @ Cont.

\yskip\halign{\quad#\hfil&\quad#\hfil&\quad\hfil#\hfil\cr
%\vskip\halign{\quad#\hfil&\quad\hbox to11cm{#\hfil}&\quad\hfil#\hfil\cr
@z

Section 120.

@x
\.{long}&|raw_int|: \stars&maybe\cr
@y
\.{long}&|raw_int|: \stars&maybe\cr}

@ Cont.

\yskip\halign{\quad#\hfil&\quad#\hfil&\quad\hfil#\hfil\cr
%\vskip\halign{\quad#\hfil&\quad\hbox to11cm{#\hfil}&\quad\hfil#\hfil\cr
@z

Section 121.

@x
\.{try}&|else_like|: \stars&maybe\cr
@y
\.{try}&|else_like|: \stars&maybe\cr}

@r @ Cont.

\yskip\halign{\quad#\hfil&\quad#\hfil&\quad\hfil#\hfil\cr
%\vskip\halign{\quad#\hfil&\quad\hbox to11cm{#\hfil}&\quad\hfil#\hfil\cr
@z

Sections 122--129.

@x l.7 line numbers refer to 'prod.w'
@ Here is a table of all the productions.  Each production that
@y
@* Table of all productions.  Each production that
@z

TeX reports 'extra \fi' when running on twilled 'ctwill.w'.

@x l.14
\fi \newcount\prodno \newdimen\midcol \let\+\relax \ifon
@y
\newcount\prodno \newdimen\midcol \let\+\relax
@z

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
{$\displaystyle\matrix{\strut\hbox{#1}\hfill\cr\strut\hbox{#2}\hfill\cr}$}
\def\maltt #1 #2 #3
{$\displaystyle\matrix{\strut\hbox{#1}\hfill\cr\hbox{#2}\hfill\cr
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
{$\displaystyle\matrix{\strut\hbox{#1}\hfill\cr\strut\hbox{#2}\hfill\cr}$}
\def\maltt #1 #2 #3
{$\displaystyle\matrix{\strut\hbox{#1}\hfill\cr\hbox{#2}\hfill\cr
   \strut\hbox{#3}\hfill\cr}$}
\yskip@-in@>
\prodno=27 \midcol=2.5in
\def\theprodno{\number\prodno \global\advance\prodno by1\enspace}
\def\dagit{\dag\theprodno}
\def\+#1&#2&#3&#4\cr{\def\next{#1}%
 \line{\hbox to 2em{\hss
  \ifx\next\empty\theprodno\else\next\fi}\strut
  \ignorespaces#2\hfil\hbox to\midcol{$\RA$
  \ignorespaces#3\hfil}\quad \hbox to1.45in{\ignorespaces#4\hfil}}}
@z

@x l.75
              \&{struct} \&{name\_info} $\{$\cr
@y
              \&{struct} \&{name\_info} $\{$\cr
\endgroup

@r @ Cont.@-z@>@-in@>
\begingroup \lineskip=4pt
\def\alt #1 #2
{$\displaystyle\Bigl\{\!\matrix{\strut\hbox{#1}\cr
   \strut\hbox{#2}\cr}\!\Bigr\}$ }
\def\altt #1 #2 #3
{$\displaystyle\Biggl\{\!\matrix{\strut\hbox{#1}\cr\hbox{#2}\cr
   \strut\hbox{#3}\cr}\!\Biggr\}$ }
\def\malt #1 #2
{$\displaystyle\matrix{\strut\hbox{#1}\hfill\cr\strut\hbox{#2}\hfill\cr}$}
\def\maltt #1 #2 #3
{$\displaystyle\matrix{\strut\hbox{#1}\hfill\cr\hbox{#2}\hfill\cr
   \strut\hbox{#3}\hfill\cr}$}
\yskip@-in@>
\prodno=47 \midcol=2.5in
\def\theprodno{\number\prodno \global\advance\prodno by1\enspace}
\def\dagit{\dag\theprodno}
\def\+#1&#2&#3&#4\cr{\def\next{#1}%
 \line{\hbox to 2em{\hss
  \ifx\next\empty\theprodno\else\next\fi}\strut
  \ignorespaces#2\hfil\hbox to\midcol{$\RA$
  \ignorespaces#3\hfil}\quad \hbox to1.45in{\ignorespaces#4\hfil}}}
@z

@x l.164
       $|force|\,E\,\\{in}\,\\{bsp}\,S\,\\{out}\,|force|$ & |else x=0;|\cr
@y
       $|force|\,E\,\\{in}\,\\{bsp}\,S\,\\{out}\,|force|$ & |else x=0;|\cr
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
{$\displaystyle\matrix{\strut\hbox{#1}\hfill\cr\strut\hbox{#2}\hfill\cr}$}
\def\maltt #1 #2 #3
{$\displaystyle\matrix{\strut\hbox{#1}\hfill\cr\hbox{#2}\hfill\cr
   \strut\hbox{#3}\hfill\cr}$}
\yskip@-any@>@-z@>@-g@>@-a@>@-x@>@-y@>@-f@>
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
{$\displaystyle\matrix{\strut\hbox{#1}\hfill\cr\strut\hbox{#2}\hfill\cr}$}
\def\maltt #1 #2 #3
{$\displaystyle\matrix{\strut\hbox{#1}\hfill\cr\hbox{#2}\hfill\cr
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

@x l.232
\+\dagit& |new_exp| & |exp| & |new int;|\cr
@y
\+\dagit& |new_exp| & |exp| & |new int;|\cr
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
{$\displaystyle\matrix{\strut\hbox{#1}\hfill\cr\strut\hbox{#2}\hfill\cr}$}
\def\maltt #1 #2 #3
{$\displaystyle\matrix{\strut\hbox{#1}\hfill\cr\hbox{#2}\hfill\cr
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

@x l.272
\+& |any_other| |end_arg| & |end_arg| &    \&{char}$*$\.{@@]}\cr
\yskip
\yskip
\yskip
\parindent=0pt
\dag{\bf Notes}
@y
\+& |any_other| |end_arg| & |end_arg| &    \&{char}$*$\.{@@]}\cr
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

@r @-any_other@>@-z@>@ \begingroup\dag{\bf Notes}
@z

Section 130.

@x
@* Implementing the productions.
@y
@* Implementing the productions.  \advance \hsize by -4cm
\ifx\undefined\pdfpagewidth \else \advance \pdfpagewidth by -4cm \fi
@z

@x
the category codes |pp->cat,@,@,(pp+1)->cat|$,\,\,\ldots\,$
@y
the category codes |pp->cat|, |(pp+1)->cat|, $\,\ldots\,$
@z

Section 132.

@x
@ @<Set init...@>=
@y
@r @ @<Set init...@>=
@z

Section 133.

@x
@ Token lists in |@!tok_mem| are composed of the following kinds of
@y
@ Token lists in |@!tok_mem| are composed of the following kinds of
@-p@>
@z

Section 134.

@x
#ifdef DEAD_CODE /* not used in |main| */
@y
@ Debugging routine, use at your own risk.
@-DEAD_CODE@>

@c
#ifdef DEAD_CODE /* not used in |main| */
@z

Section 135.

@x
@ @<Print token |r|...@>=
@y
@r @ @<Print token |r|...@>=
@z

Section 136.

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

Section 137.

@x
The code below is an exact translation of the production rules into
@y
@r @ The code below is an exact translation of the production rules into
@-a@>
@z

Section 139.

@x
@ Let us consider the big switch for productions now, before looking
@y
@r @ Let us consider the big switch for productions now, before looking
@z

Section 140.

@x
  switch (pp->cat) {
    case exp: @<Cases for |exp|@>; @+break;
    case lpar: @<Cases for |lpar|@>; @+break;
    case unop: @<Cases for |unop|@>; @+break;
    case ubinop: @<Cases for |ubinop|@>; @+break;
    case binop: @<Cases for |binop|@>; @+break;
    case cast: @<Cases for |cast|@>; @+break;
    case sizeof_like: @<Cases for |sizeof_like|@>; @+break;
    case int_like: @<Cases for |int_like|@>; @+break;
    case public_like: @<Cases for |public_like|@>; @+break;
    case colcol: @<Cases for |colcol|@>; @+break;
    case decl_head: @<Cases for |decl_head|@>; @+break;
    case decl: @<Cases for |decl|@>; @+break;
    case base: @<Cases for |base|@>; @+break;
    case struct_like: @<Cases for |struct_like|@>; @+break;
    case struct_head: @<Cases for |struct_head|@>; @+break;
    case fn_decl: @<Cases for |fn_decl|@>; @+break;
    case function: @<Cases for |function|@>; @+break;
    case lbrace: @<Cases for |lbrace|@>; @+break;
    case if_like: @<Cases for |if_like|@>; @+break;
    case else_like: @<Cases for |else_like|@>; @+break;
    case else_head: @<Cases for |else_head|@>; @+break;
    case if_clause: @<Cases for |if_clause|@>; @+break;
    case if_head: @<Cases for |if_head|@>; @+break;
    case do_like: @<Cases for |do_like|@>; @+break;
    case case_like: @<Cases for |case_like|@>; @+break;
    case catch_like: @<Cases for |catch_like|@>; @+break;
    case tag: @<Cases for |tag|@>; @+break;
    case stmt: @<Cases for |stmt|@>; @+break;
    case semi: @<Cases for |semi|@>; @+break;
    case lproc: @<Cases for |lproc|@>; @+break;
    case section_scrap: @<Cases for |section_scrap|@>; @+break;
    case insert: @<Cases for |insert|@>; @+break;
    case prelangle: @<Cases for |prelangle|@>; @+break;
    case prerangle: @<Cases for |prerangle|@>; @+break;
    case langle: @<Cases for |langle|@>; @+break;
    case template_like: @<Cases for |template_like|@>; @+break;
    case new_like: @<Cases for |new_like|@>; @+break;
    case new_exp: @<Cases for |new_exp|@>; @+break;
    case ftemplate: @<Cases for |ftemplate|@>; @+break;
    case for_like: @<Cases for |for_like|@>; @+break;
    case raw_ubin: @<Cases for |raw_ubin|@>; @+break;
    case const_like: @<Cases for |const_like|@>; @+break;
    case raw_int: @<Cases for |raw_int|@>; @+break;
    case operator_like: @<Cases for |operator_like|@>; @+break;
    case typedef_like: @<Cases for |typedef_like|@>; @+break;
    case delete_like: @<Cases for |delete_like|@>; @+break;
    case question: @<Cases for |question|@>; @+break;
  }
  pp++; /* if no match was found, we move to the right */
}
@y
  switch (pp->cat) {
    @<Cases for |pp->cat|@>@;
  }
  pp++; /* if no match was found, we move to the right */
}

@ @<Cases for |pp->cat|@>=
    case exp: @<Cases for |exp|@>; @+break;
    case lpar: @<Cases for |lpar|@>; @+break;
    case unop: @<Cases for |unop|@>; @+break;
    case ubinop: @<Cases for |ubinop|@>; @+break;
    case binop: @<Cases for |binop|@>; @+break;
    case cast: @<Cases for |cast|@>; @+break;
    case sizeof_like: @<Cases for |sizeof_like|@>; @+break;
    case int_like: @<Cases for |int_like|@>; @+break;
    case public_like: @<Cases for |public_like|@>; @+break;
    case colcol: @<Cases for |colcol|@>; @+break;
    case decl_head: @<Cases for |decl_head|@>; @+break;
    case decl: @<Cases for |decl|@>; @+break;
    case base: @<Cases for |base|@>; @+break;
    case struct_like: @<Cases for |struct_like|@>; @+break;
    case struct_head: @<Cases for |struct_head|@>; @+break;
    case fn_decl: @<Cases for |fn_decl|@>; @+break;
    case function: @<Cases for |function|@>; @+break;
    case lbrace: @<Cases for |lbrace|@>; @+break;
    case if_like: @<Cases for |if_like|@>; @+break;
    case else_like: @<Cases for |else_like|@>; @+break;
    case else_head: @<Cases for |else_head|@>; @+break;
    case if_clause: @<Cases for |if_clause|@>; @+break;
    case if_head: @<Cases for |if_head|@>; @+break;
    case do_like: @<Cases for |do_like|@>; @+break;
    case case_like: @<Cases for |case_like|@>; @+break;
    case catch_like: @<Cases for |catch_like|@>; @+break;
    case tag: @<Cases for |tag|@>; @+break;
    case stmt: @<Cases for |stmt|@>; @+break;
    case semi: @<Cases for |semi|@>; @+break;
    case lproc: @<Cases for |lproc|@>; @+break;
    case section_scrap: @<Cases for |section_scrap|@>; @+break;
    case insert: @<Cases for |insert|@>; @+break;
    case prelangle: @<Cases for |prelangle|@>; @+break;
    case prerangle: @<Cases for |prerangle|@>; @+break;
    case langle: @<Cases for |langle|@>; @+break;
    case template_like: @<Cases for |template_like|@>; @+break;
    case new_like: @<Cases for |new_like|@>; @+break;
    case new_exp: @<Cases for |new_exp|@>; @+break;
    case ftemplate: @<Cases for |ftemplate|@>; @+break;
    case for_like: @<Cases for |for_like|@>; @+break;
    case raw_ubin: @<Cases for |raw_ubin|@>; @+break;
    case const_like: @<Cases for |const_like|@>; @+break;
    case raw_int: @<Cases for |raw_int|@>; @+break;
    case operator_like: @<Cases for |operator_like|@>; @+break;
    case typedef_like: @<Cases for |typedef_like|@>; @+break;
    case delete_like: @<Cases for |delete_like|@>; @+break;
    case question: @<Cases for |question|@>; @+break;
@z

Section 141.

@x
of identifiers in case labels.

If the first identifier is the keyword `\&{operator}', we give up;
@y
of identifiers in case labels.
If the first identifier is the keyword `\&{operator}', we give up;
@z

Section 142.

@x
@ The scraps currently being parsed must be inspected for any
@y
@r @ The scraps currently being parsed must be inspected for any
@z

@x
|make_reserved|, hence |tok_loc| has been set.
@y
\hfil\break|make_reserved|, hence |tok_loc| has been set.
@z

Section 147.

@x
{ token_pointer j;
@y
{@+ token_pointer j;
@z

Section 148.

@x
@ The trickiest part of \.{CTWILL} is the procedure |make_ministring(l)|,
@y
@ The trickiest part of \.{CTWILL} is the procedure |make_ministring(l)|,
@-b@>
@z

@x
  int l) /* 0, 1, or 2 */
{
@y
  int l) /* 0, 1, or 2 */
{@+
@z

Section 152.

@x
@ @<Cases for |lpar|@>=
@y
@r @ @<Cases for |lpar|@>=
@z

Section 166.

@x
@ Outdent after parameter declarations with option \.{-i}.
@y
@r @ Outdent after parameter declarations with option \.{-i}.
@z

Section 199.

@x
@ Here's the |squash| procedure, which
@y
@r @ Here's the |squash| procedure, which
@z

Section 200.

@x
@ The following macro is used to append a scrap whose tokens have just
@y
@ The following macro is used to append a scrap whose tokens have just
@-b@>
@-c@>
@z

Section 221.

@x
@ @d cur_end cur_state.end_field /* current ending location in |tok_mem| */
@y
@r @ @d cur_end cur_state.end_field /* current ending location in |tok_mem| */
@z

Section 224.

@x
called when |stack_ptr==1|.
@y
called when |stack_ptr| |==1|.
@z

Section 227.

@x
  text_pointer save_text_ptr;
  sixteen_bits save_next_control; /* values to be restored */
  text_pointer p; /* translation of the \CEE/ text */
@y
  text_pointer save_text_ptr, p; /* translation of the \CEE/ text */
  sixteen_bits save_next_control; /* values to be restored */
@z

Section 230.

@x
@ An identifier of length one does not have to be enclosed in braces, and it
@y
@r @ An identifier of length one does not have to be enclosed in braces, and it
@z

Section 231.

@x
else @<Look ahead for strongest line break, |goto reswitch|@>
@y
else {@<Look ahead for strongest line break, |goto reswitch|@>}
@z

Section 232.

@x
@<Look ahead for st...@>= {
@y
@<Look ahead for st...@>=
@z

@x
}
@y
@z

Section 233.

@x
@ @<Output saved...@>=
@y
@r @ @<Output saved...@>=
@z

Section 243.

@x
@ The output file will contain the control sequence \.{\\Y} between non-null
@y
@r @ The output file will contain the control sequence \.{\\Y} between non-null
@z

Section 246.

@x
@<Translate the \T...@>= do {
@y
@<Translate the \T...@>= do {@+
@z

Section 254.

@x
|next_control>=begin_C|. We will make the global variable |this_section|
@y
\hfil\break|next_control>=begin_C|. We will make the global variable |this_section|
@z

Section 255.

@x
@ @<Translate the \CEE/...@>=
@y
@r @ @<Translate the \CEE/...@>=
@z

Section 259.

@x
@ The |footnote| procedure gives cross-reference information about
@y
@r @ The |footnote| procedure gives cross-reference information about
@z

Section 263.

@x
@ @<Flag the usage of this identifier, for the mini-index@>=
@y
@<Flag the usage of this identifier, for the mini-index@>=
@z

Section 264.

@x
@ @<Output information about usage of id's defined in other sections@>=
@y
@r @ @<Output information about usage of id's defined in other sections@>=
@z

Section 270.

@x
the index section itself---NOT!

@<Global...@>=
sixteen_bits k_section; /* runs through the sections */
@y
the index section itself---NOT!
@z

Section 271.

@x
@ A left-to-right radix sorting method is used, since this makes it easy to
@y
@ A left-to-right radix sorting method is used, since this makes it easy to
@-c@>
@z

@x
name_pointer blink[max_names]; /* links in the buckets */
@y
name_pointer blink[max_names]; /* links in the buckets */
sixteen_bits k_section; /* runs through the sections */
@z

Section 273.

@x
@ During the sorting phase we shall use the |cat| and |trans| arrays from
@y
@ During the sorting phase we shall use the |cat| and |trans| arrays from
@-j@>
@-k@>
@z

Section 276.

@x
$|collate|[0]<|collate|[1]<\cdots<|collate|[100]$.
@y
|collate[0]| $<$|collate[1]|$<\cdots<$|collate[100]|.
@z

Section 283.

@x
    else {char *j;
@y
    else {@+char *j;
@z

Section 290.

@x
@ Because on some systems the difference between two pointers is a |long|
@y
@r @ Because on some systems the difference between two pointers is a |long|
@z

Section 291.

@x
\bigskip
\font\itt=cmitt10
{\noindent \it Although \.{\itt CTWILL} is based on \.{\itt cweave.w}, new and
modified material is incorporated all over the place, without taking special
care for keeping the original section numbering intact.}
@y
@z

Section 293.

@x
@* Function declarations.  Here are declarations---conforming to
@y
@* Function declarations.  Here are declarations---conforming to
@-DEAD_CODE@>
@z

Section 294.

@x
extern const char *use_language; /* prefix to \.{cwebmac.tex} in \TEX/ output */
@y
extern const char *use_language; /* prefix to \.{ctwimac.tex} in \TEX/ output */
@z

Section 300.

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
