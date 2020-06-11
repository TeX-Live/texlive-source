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
\def\page{\box255 } \normalbottom
@y
\let\page=\pagebody \raggedbottom
@z

@x
\def\runninghead{{\tentt CWEB} USER MANUAL (VERSION 3.64)}
@y
\def\Kpathsea/{{\mc KPATHSEA\spacefactor1000}}
\def\runninghead{{\tentt CWEB} USER MANUAL (3.64 [\TeX~Live])}
@z

@x
\vskip 18pt\centerline{(Version 3.64 --- February 2002)}
@y
\vskip 18pt\centerline{(Version 3.64 --- February 2002)%
\footnote*{This document describes the extended \.{CWEB} (3.64 [\TeX~Live]).}}
@z

@x
should be sent to Levy (\.{levy@math.berkeley.edu}).
@y
should be sent to Levy (\.{levy@math.berkeley.edu}).%
\footnote{$^1$}{Or rather to the ``\TeX~Live''-related mailing list
\.{tex-k@tug.org}.}
@z

@x
email address in a \.{CWEB} file (e.g., \.{levy@@math.berkeley.edu}).
@y
email address in a \.{CWEB} file (e.g., \.{levy@@math.berkeley.edu}%
\footnote{$^2$}{Or rather \.{tex-k@@tug.org}.}).
@z

@x
it cannot find them in the current directory.
@y
it cannot find them in the current directory.%
\footnote{$^3$}{In this extended implementation, if an \.{@i}nclude file can
not be found in the current directory, \.{CWEB} will use the conventions of
the \Kpathsea/ library as outlined in section~\X90:File lookup with
\Kpathsea/\X~of appendix~D.}% FIXME
@z

@x
output by \.{CWEAVE}. (On by default.) (Has no effect on \.{CTANGLE}.)
@y
output by \.{CWEAVE}. (On by default.) (Has no effect on \.{CTANGLE}.)%
\footnote{$^4$}{This extended version of \.{CWEB} adds the following options
to the list:

\option i Indent parameters in function declarations.  This causes the
formal parameter declarations in function heads to be indented.  (On by
default; \.{-i} typesets declarations flush left; some people think this
to be more logical than indenting them) (Has no effect on \.{CTANGLE}.)

\option l This option takes the string of characters to its right as its
argument to switch between different user languages and macro packages.
For example, if you call \.{CWEAVE} with the `\.{+ld}' (or `\.{-ld}') option,
the German \.{CWEB} macros \.{dcwebmac.tex} will be loaded in the first line
of output instead of the English ones. To avoid conflicts, the Danish
macros could be called \.{dkcwebmac.tex} and would be included by `\.{+ldk}'.
(Off by default) (Has no effect on \.{CTANGLE}.)

\option o Separate declarations and the first statement in a function block.
\.{CWEAVE} automatically inserts a bit of extra space.  (On by default.)
(Has no effect on \.{CTANGLE}.)

\option t Overwrite output files with temporary output only if changes occur.
(On by default.)}
@z

@x
appendices D, E, and~F, which exhibit the complete source code for
\.{CTANGLE} and \.{CWEAVE}.

\vfil\eject\titletrue
@y
appendices D, E, and~F, which exhibit the complete source code for
\.{CTANGLE} and \.{CWEAVE}.\footnote{$^5$}{Actually, appendix~D contains the
source code for \.{COMMON}, and the additional appendix~G exhibits the
source code for \.{CTWILL}, which is based on \.{CWEAVE}.}

\eject\titletrue
@z

@x
  \.{ { }\\vskip 15pt \\centerline\{(Version 3.64)\}{ }\\vfill\}}\cr}$$
@y
  \.{ { }\\vskip 15pt \\centerline\{(Version 3.64)\}{ }\\vfill\}}\cr}$$
@z

@x
if you have a duplex printer. Appendices D, E, and F of the complete
@y
if you have a duplex printer. Appendices D, E, and F\footnote{$^6$}{And
Appendix~G.} of the complete
@z

@x
or the \CEE/ comments of a \.{CWEB} file:
@y
or the \CEE/ comments of a \.{CWEB} file:%
\footnote{$^7$}{Be careful to make that $$\vbox{\halign{\.{#}\hfil\cr
You can send email to
 \\pdfURL\{the author\}\{mailto:andreas\\UNDER/github@@freenet.de\}\cr
or visit
 \\pdfURL\{his home page\}\{https://github.com/ascherer\}\cr}}$$ in
the $21^{\rm st}$ century.}
@z

