@q Changes for CWEB in TeX Live from numerous contributors.              @>
@q This file is in the Public Domain.                                    @>

@q Most of the original Kpathsea changes by Wlodek Bzyl and Olaf Weber   @>
@q were merged with the set of change files of the CWEBbin project;      @>
@q see https://github.com/ascherer/cwebbin for the original parts.       @>

@q This stripped change file {comm,ctang,cweav,ctwill,cwebman}-w2c.ch    @>
@q has been created from the elaborate set of change files               @>
@q {comm,ctang,cweav,cwebman}-patch.ch,                                  @>
@q {comm,cweav,cwebman}-extensions.ch, {comm,ctang,cweav}-output.ch,     @>
@q {comm,ctang,cweav}-i18n.ch, and cweav-twill.ch for CTWILL, and        @>
@q {comm,ctang,cweav,ctwill,cwebman}-texlive.ch with the 'tie' processor @>
@q and is used as a monolithic changefile for {common,ctangle,cweave}.w  @>
@q and cwebman.tex in TeX Live.                                          @>

@q Please send comments, suggestions, etc. to tex-k@@tug.org.            @>

@x
\input cwebmac
\acrofalse\pdffalse\pdftexfalse\hintfalse\acrohintfalse
@y
\input cwebmac
@z

@x
\def\page{\box255 } \normalbottom
@y
\let\page=\pagebody \raggedbottom
\newcount\footnotecount \footnotecount 1\relax
\def\cwebfootnote#1{\footnote{${}^{\the\footnotecount}$}{#1}%
  \advance\footnotecount by 1\relax}
@z

@x
\outer\def\section #1.{\penalty-500\bigskip
        \centerline{\sectionfont\def\.##1{{\twelvett##1}} #1}\nobreak\vskip 6pt
        \everypar{\hskip-\parindent\everypar{}}}
@y
\ifacro
  \pdfpagewidth=\pagewidth \advance\pdfpagewidth by 2cm
  \pdfpageheight=\pageheight \advance\pdfpageheight by 3cm
  \ifpdftex \pdfhorigin=1cm \pdfvorigin=1cm
  \else \advance\pdfpageheight by 1cm \global\pageshift=-1.54cm
    \global\hoffset=-1.54cm \global\voffset=-1.54cm \fi
\fi

\newcount\destcount \destcount=1\relax

\def\subsections{0}
\outer\def\section #1.{\penalty-500\bigskip
        \centerline{\sectionfont\def\.##1{{\twelvett##1}}
  \ifacro\vbox to 0pt{\kern-2.5ex\relax
    \ifpdftex\pdfdest num \destcount fitbh\relax
    \else\special{pdf: dest (\the\destcount) [ @thispage /FitBH @ypos ]}\fi
    \def\.##1{##1}\def\TeX{TeX}%
    \ifpdftex\pdfoutline goto num \destcount
      \ifnum\subsections>0 count -\subsections\fi {#1}\relax
    \else\special{pdf: outline 0 << /Title (#1)
      /A << /S /GoTo /D (\the\destcount) >> >>}\fi
    \gdef\subsections{0}%
    \global\advance\destcount by 1\relax
    \kern2.5ex\relax
  }\fi #1}\nobreak\vskip 6pt
        \everypar{\hskip-\parindent\everypar{}}}

\def\appA{15}
\def\appB{16}
\def\appC{17}

\def\Appendix#1{\leavevmode
  \ifacro\ifpdftex
    \pdfstartlink attr{/Border[0 0 0]} goto num\csname app#1\endcsname\relax
    \Blue\hbox{Appendix}~#1\Black
    \pdfendlink
  \else
    \setbox0=\hbox{\special{pdf: bc [ \pdflinkcolor ]}{\hbox{Appendix}~#1}%
    \special{pdf: ec}}\special{pdf: ann width \thewidth height \theheight
      depth \thedepth << /Type /Annot /Subtype /Link /Border [0 0 0]
      /A << /S /GoTo /D (\csname app#1\endcsname) >> >>}\box0\relax
  \fi\else Appendix~#1\fi}

\newcount\subdestcount \subdestcount=151\relax

\outer\def\subsection #1.{\ifacro
    \ifpdftex\pdfdest num \subdestcount fitbh\relax
      \pdfoutline goto num \subdestcount {#1}\relax
    \else\special{pdf: dest (\the\subdestcount) [ @thispage /FitBH @ypos ]}%
      \special{pdf: outline 1 << /Title (#1)
        /A << /S /GoTo /D (\the\subdestcount) >> >>}\fi
    \global\advance\subdestcount by 1\relax
  \fi}
@z

@x
\def\runninghead{{\tentt CWEB} USER MANUAL (VERSION 4.7)}
@y
\def\Kpathsea/{{\mc KPATHSEA\spacefactor1000}}
\def\runninghead{{\tentt CWEB} USER MANUAL (Version 4.7 [\TeX~Live])}
@z

@x
\vskip 18pt\centerline{(Version 4.7 --- February 2022)}
@y
\vskip 18pt\centerline{(Version 4.7 --- February 2022)%
\footnote*{This document describes the extended \.{CWEB} (Version 4.7 [\TeX~Live]).}}
@z

@x
Internet page \.{http://www-cs-faculty.stanford.edu/\char`\~knuth/cweb.html}
@y
Internet page
\pdfURL{\.{http://www-cs-faculty.stanford.edu/\char`\~knuth/cweb.html}}%
          {http://www-cs-faculty.stanford.edu/\TILDE/knuth/cweb.html}
@z

@x
\.{https://github.com/ascherer/cweb} with the really current news.
@y
\pdfURL{\.{https://github.com/ascherer/cweb}}%
          {https://github.com/ascherer/cweb}
with the really current news.
@z

@x
should be sent to the \TeX-related mailing list \.{tex-k@tug.org}.
@y
should be sent to the \TeX-related mailing list
\pdfURL{\.{tex-k@tug.org}}%
   {mailto:tex-k@tug.org}.
@z

@x
email address in a \.{CWEB} file (e.g., \.{tex-k@@tug.org}).
@y
email address in a \.{CWEB} file (e.g.,
\pdfURL{\.{tex-k@@tug.org}}%
   {mailto:tex-k@tug.org}).
@z

@x
it cannot find them in the current directory.
@y
it cannot find them in the current directory.%
\cwebfootnote{\acrofalse In this extended implementation, if an \.{@i}nclude
file can not be found in the current directory, \.{CWEB} will use the
conventions of the \Kpathsea/ library as outlined in section~\X93:File lookup
with \Kpathsea/\X~of \pdfURL{appendix~D}{common-changes.pdf} to look for it.
% FIXME
(This is true for any other input or change file lookup.)}
@z

@x
except for error messages.
@y
except for error messages.%
\cwebfootnote{\.{CWEB} in \TeX~Live runs `\.{--quiet}ly', i.\,e.,
options `\.{b}', `\.{h}', and `\.{p}' are `off' by default.}
@z

@x
options are currently implemented:

\yskip
\def\option#1 {\textindent{\.#1}\hangindent2\parindent}
@y
\def\option#1 {\textindent{\.#1}\hangindent2\parindent}%
options are currently implemented:%
\cwebfootnote{This extended version of \.{CWEB} adds the following options
to the list:
\hfil\smallskip
\option c Check temporary output files for changes.  (Off by default.)
\hfil\vskip\normallineskip
\option d Set the debugging variable \\{kpathsea\_debug} to the numeric
value $N\in[0,127]$ of this option.  See section~95 % FIXME
of \pdfURL{appendix~D}{common-changes.pdf} for details.
\hfil\vskip\normallineskip
\option i Indent parameters in function declarations.  (On by default;
\.{-i} typesets declarations flush left; some people think this to be
more logical than indenting them.) (Has no effect on \.{CTANGLE}.)
\hfil\vskip\normallineskip
\option l This option takes the string of characters to its right as its
argument to switch between different user languages and macro packages.
For example, if you call \.{CWEAVE} with the `\.{+ld}' (or `\.{-ld}') option,
the German \.{CWEB} macros \.{dcwebmac.tex} will be loaded in the first line
of output instead of the English ones. (Off by default.) (Has no effect on
\.{CTANGLE}.)
\hfil\vskip\normallineskip
\option o Separate declarations and the first statement in a function block.
(On by default.) (Has no effect on\break\hbox{}\qquad\.{CTANGLE}.)}

\yskip
@z

@x
Sometimes things don't work as smoothly, and you get a bunch of
@y
\acrofalse\pdftexfalse\pdffalse
Sometimes things don't work as smoothly, and you get a bunch of
@z

@x
`$\\{main}(\\{argc},\39\\{argv}{}$)'.
@y
`$\\{main}(\\{argc},\39\\{argv}{}$)'.
\ifx\pdf+\pdftrue\fi
\ifx\pdfoutput\undefined \pdftexfalse \else\ifnum\pdfoutput=0 \pdftexfalse
\else \pdftextrue \pdfoutput=1\fi\fi
\ifpdf\acrotrue\fi \ifpdftex\acrotrue\fi
@z

@x
\section Hypertext and hyperdocumentation.
@y
\vfill\eject
\section Hypertext and hyperdocumentation.
@z

@x
the program sources at \.{ftp://ftp.cs.stanford.edu/pub/ctwill}.
@y
the program sources at
\pdfURL{\.{http://ftp.cs.stanford.edu/pub/ctwill}}%
          {http://ftp.cs.stanford.edu/pub/ctwill}.%
\cwebfootnote{\TeX~Live comes with an up-to-date
\.{ctwill} executable -- and its associated helpers -- out of the box.}
@z

@x
As an example of a real program written in \.{CWEB}, Appendix~A
@y
As an example of a real program written in \.{CWEB}, \Appendix A
@z

@x
Appendix B is the file that sets \TEX/ up to accept
the output of \.{CWEAVE}, and Appendix~C discusses how to use some of those
@y
\Appendix B is the file that sets \TEX/ up to accept
the output of \.{CWEAVE}, and \Appendix C discusses how to use some of those
@z

@x
appendices D, E, and~F, which exhibit the complete source code for
\.{CTANGLE} and \.{CWEAVE}.

\vfil\eject\titletrue
@y
appendices \pdfURL{D}{common.pdf}, \pdfURL{E}{ctangle.pdf},
and~\pdfURL{F}{cweave.pdf}, which exhibit the complete source code for
\.{CTANGLE} and \.{CWEAVE}.%
\cwebfootnote{Actually, \pdfURL{Appendix~D}{common.pdf} contains the source
code for \.{COMMON}, and the additional \pdfURL{Appendix~G}{ctwill.pdf}
exhibits the source code for \.{CTWILL}, which is based on \.{CWEAVE}.}

\eject\titletrue
@z

@x
\section Appendix A: Excerpts from a \.{CWEB} Program.
@y
\def\subsections{4}
\section Appendix A: Excerpts from a \.{CWEB} Program.
\subsection CWEB file format.
@z

@x
generated sections 27--31 of the file \.{common.w}, which contains
@y
generated sections 27--31 of the file
\pdfURL{\.{common.w}}{common.pdf}, which contains
@z

@x
\def\runninghead{APPENDIX A --- TRANSLATION BY {\tentt CTANGLE}}
@y
\def\runninghead{APPENDIX A --- TRANSLATION BY {\tentt CTANGLE}}
\subsection Translation by CTANGLE.
@z

@x
\def\runninghead{APPENDIX A --- TRANSLATION BY {\tentt CWEAVE}}
@y
\def\runninghead{APPENDIX A --- TRANSLATION BY {\tentt CWEAVE}}
\subsection Translation by CWEAVE.
@z

@x
\def\runninghead{APPENDIX A --- FINAL DOCUMENT}

And here's what the same excerpt looks like when typeset.
(Can you spot the typographical nicety used in the \.{CWEB} code?)
\let\K=\leftarrow
@y
\def\runninghead{APPENDIX A --- FINAL DOCUMENT}
\subsection Final document.

\acrofalse\pdftexfalse\pdffalse
And here's what the same excerpt looks like when typeset.
(Can you spot the typographical niceties used in the \.{CWEB} code?)
\let\K=\leftarrow
\def\C#1{\5\5\quad$\triangleright\,${\cmntfont#1}$\,\triangleleft$}
@z

@x
\vfil\eject\titletrue
@y
\vfil\eject\titletrue
\ifx\pdf+\pdftrue\fi
\ifx\pdfoutput\undefined \pdftexfalse \else\ifnum\pdfoutput=0 \pdftexfalse
\else \pdftextrue \pdfoutput=1\fi\fi
\ifpdf\acrotrue\fi \ifpdftex\acrotrue\fi
@z

@x
  \.{ { }\\vskip 15pt \\centerline\{(Version 4.7)\}{ }\\vfill\}}\cr}$$
@y
  \.{ { }\\vskip 15pt \\centerline\{(Version 4.7)\}{ }\\vfill\}}\cr}$$
@z

@x
if you have a duplex printer. Appendices D, E, and F of the complete
@y
if you have a duplex printer. Appendices \pdfURL{D}{common.pdf},
\pdfURL{E}{ctangle.pdf}, and \pdfURL{F}{cweave.pdf}\cwebfootnote{And
\pdfURL{Appendix~G}{ctwill.pdf}.} of the complete
@z

@x
\point 20. Furthermore, group titles can be converted to an arbitrary
@y
\vfill\eject
\point 20. Furthermore, group titles can be converted to an arbitrary
@z

