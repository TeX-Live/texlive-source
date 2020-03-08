## Use `autoreconf' in top-level directory to recreate all
## autoconf/automake files.
## Or, call `automake Makefile' to create only Makefile.in.

## Copyright 2020 by Karl Berry <tex-live@tug.org>
## Copyright (C) 2004-2005 by Gour.
##
## This program is free software; you can redistribute it and/or
## modify it under the terms of the GNU General Public License as
## published by the Free Software Foundation; either version 2 of the
## License, or (at your option) any later version.
##
## This program is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
## General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program in the file COPYING; if not, write to the
## Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
## Boston, MA 02110-1301 USA.

langdir	= $(pkglibdir)/modules/lang

nobase_nodist_lang_DATA =
EXTRA_DIST =
DISTCLEANFILES =

DOCS_CP1250 =
DOCS_CP1252 =
DOCS_CP850 =
DOCS_LATIN1 =
DOCS_LATIN2 =
DOCS_LATIN3 =
DOCS_LATIN4 =
DOCS_LATIN5 =
DOCS_LATIN9 =
DOCS_CYR_1251 =
DOCS_CYR_ISO88595 =
DOCS_CYR_ISOIR111 =
DOCS_CYR_KOI8_R =
DOCS_CYR_KOI8_U =
DOCS_OTHER =
DOCS_UTF8_LAT =
DOCS_UTF8_CYR =
DOCS_UTF8_OTHER =

## Include all Makefile.am fragments.
##
include albanian/Makefile.am
include belarusian/Makefile.am
include bulgarian/Makefile.am
include croatian/Makefile.am
include czech/Makefile.am
include danish/Makefile.am
include dutch/Makefile.am
include english/Makefile.am
include esperanto/Makefile.am
include estonian/Makefile.am
include finnish/Makefile.am
include french/Makefile.am
include general/Makefile.am
include georgian/Makefile.am
include german/Makefile.am
include greek/Makefile.am
include gypsy/Makefile.am
include hausa/Makefile.am
include hebrew/Makefile.am
include hungarian/Makefile.am
include icelandic/Makefile.am
include italian/Makefile.am
include klingon/Makefile.am
include kurdish/Makefile.am
include latin/Makefile.am
include latvian/Makefile.am
include lithuanian/Makefile.am
include lower-sorbian/Makefile.am
include macedonian/Makefile.am
include mongolian/Makefile.am
include norwegian/Makefile.am
include persian/Makefile.am
include polish/Makefile.am
include portuguese/Makefile.am
include romanian/Makefile.am
include russian/Makefile.am
include serbian/Makefile.am
include slovak/Makefile.am
include slovenian/Makefile.am
include spanish/Makefile.am
include swedish/Makefile.am
include test1/Makefile.am
include turkish/Makefile.am
include ukrainian/Makefile.am
include upper-sorbian/Makefile.am
include vietnamese/Makefile.am

%utf8.pl : %utf8.pl.in
	@$(MKDIR_P) `echo $@ | sed -e 's,/[^/]*$$,,'`
	sed -e 's/@.{\([^}]*\)}/\1/g;' <$< >$@

%latin1.pl : %utf8.pl.in
	@$(MKDIR_P) `echo $@ | sed -e 's,/[^/]*$$,,'`
	sed -e 's/@u{\([^}]*\)}//g;' <$< | \
	iconv -f UTF-8 -t ISO8859-1 >$@

%cp850.pl : %utf8.pl.in
	@$(MKDIR_P) `echo $@ | sed -e 's,/[^/]*$$,,'`
	sed -e 's/@.{\([^}]*\)}//g;' <$< | \
	iconv -f UTF-8 -t CP850 >$@

romanian/latin2.pl : romanian/utf8.pl.in
	@$(MKDIR_P) romanian
	sed -e 's/@u{\([^}]*\)}//g;' -f $(srcdir)/rom-comma-kludge.sed <$< | \
	iconv -f UTF-8 -t ISO8859-2 >$@

%latin2.pl : %utf8.pl.in
	@$(MKDIR_P) `echo $@ | sed -e 's,/[^/]*$$,,'`
	sed -e 's/@u{\([^}]*\)}//g;' <$< | \
	iconv -f UTF-8 -t ISO8859-2 >$@

%latin3.pl : %utf8.pl.in
	@$(MKDIR_P) `echo $@ | sed -e 's,/[^/]*$$,,'`
	sed -e 's/@u{\([^}]*\)}//g;' <$< | \
	iconv -f UTF-8 -t ISO8859-3 >$@

%latin4.pl : %utf8.pl.in
	@$(MKDIR_P) `echo $@ | sed -e 's,/[^/]*$$,,'`
	sed -e 's/@u{\([^}]*\)}//g;' <$< | \
	iconv -f UTF-8 -t ISO8859-4 >$@

%iso88595.pl : %utf8.pl.in
	@$(MKDIR_P) `echo $@ | sed -e 's,/[^/]*$$,,'`
	sed -e 's/@u{\([^}]*\)}//g;' <$< | \
	iconv -f UTF-8 -t ISO-8859-5 >$@

%iso88597.pl : %utf8.pl.in
	@$(MKDIR_P) `echo $@ | sed -e 's,/[^/]*$$,,'`
	sed -e 's/@u{\([^}]*\)}//g;' <$< | \
	iconv -f UTF-8 -t ISO-8859-7 >$@

%iso88598.pl : %utf8.pl.in
	@$(MKDIR_P) `echo $@ | sed -e 's,/[^/]*$$,,'`
	sed -e 's/@u{\([^}]*\)}//g;' <$< | \
	iconv -f UTF-8 -t ISO-8859-8 >$@

%latin5.pl : %utf8.pl.in
	@$(MKDIR_P) `echo $@ | sed -e 's,/[^/]*$$,,'`
	sed -e 's/@u{\([^}]*\)}//g;' <$< | \
	iconv -f UTF-8 -t ISO-8859-9 >$@

%latin9.pl : %utf8.pl.in
	@$(MKDIR_P) `echo $@ | sed -e 's,/[^/]*$$,,'`
	sed -e 's/@u{\([^}]*\)}//g;s/@9{\([^}]*\)}/\1/g;' <$< | \
	iconv -f UTF-8 -t ISO-8859-15 >$@

%cp1250.pl : %utf8.pl.in
	@$(MKDIR_P) `echo $@ | sed -e 's,/[^/]*$$,,'`
	sed -e 's/@u{\([^}]*\)}//g;' <$< | \
	iconv -f UTF-8 -t CP1250 >$@

%cp1251.pl : %utf8.pl.in
	@$(MKDIR_P) `echo $@ | sed -e 's,/[^/]*$$,,'`
	sed -e 's/@u{\([^}]*\)}//g;' <$< | \
	iconv -f UTF-8 -t CP1251 >$@

%cp1252.pl : %utf8.pl.in
	@$(MKDIR_P) `echo $@ | sed -e 's,/[^/]*$$,,'`
	sed -e 's/@u{\([^}]*\)}//g;s/@9{\([^}]*\)}/\1/g;' <$< | \
	iconv -f UTF-8 -t CP1252 >$@

%isoir111.pl : %utf8.pl.in
	@$(MKDIR_P) `echo $@ | sed -e 's,/[^/]*$$,,'`
	sed -e 's/@u{\([^}]*\)}//g;' <$< | \
	iconv -f UTF-8 -t ISO-IR-111 >$@

%koi8-r.pl : %utf8.pl.in
	@$(MKDIR_P) `echo $@ | sed -e 's,/[^/]*$$,,'`
	sed -e 's/@u{\([^}]*\)}//g;' <$< | \
	iconv -f UTF-8 -t KOI8-R >$@

%koi8-u.pl : %utf8.pl.in
	@$(MKDIR_P) `echo $@ | sed -e 's,/[^/]*$$,,'`
	sed -e 's/@u{\([^}]*\)}//g;' <$< | \
	iconv -f UTF-8 -t KOI8-U >$@

# %tex.pl : %utf8.pl utf8-to-tex.sed
#	sed -f $(srcdir)/utf8-to-tex.sed <$< >$@



%-lang.xdy : %-test.xdy
	sed -n '1,10 p' <$< >$@

# Running "$(PERL) -I$(srcdir) $< $*" creates %-test.xdy, %-doc.tex, %.raw,
# and %.xdy; %-test.xdy is finished first.  We must not run this command
# several times simultaneously (for details see Automake manual 27.9).
%-test.xdy : %.pl
	@$(MKDIR_P) `echo $@ | sed -e 's,/[^/]*$$,,'`
	$(PERL) -I$(srcdir) $< $*

%-doc.tex %.xdy : %-test.xdy
	@if test -f $@; then :; else \
	  trap 'rm -rf $*.lock $<' 1 2 13 15; \
	  if mkdir $*.lock 2>/dev/null; then \
	    rm -f $<; \
	    $(MAKE) $(AM_MAKEFLAGS) $<; \
	    result=$$?; rm -rf $*.lock; exit $$result; \
	  else \
	    while test -d $*.lock; do sleep 1; done; \
	    test -f $<; \
	  fi; \
	fi

SCRS = \
	alphabets-doc.tex \
	make-rules.pl \
	utf8-to-tex.sed

DOCS_CPS = \
	$(DOCS_CP1250) \
	$(DOCS_CP1252) \
	$(DOCS_CP850)

DOCS_LAT = \
	$(DOCS_LATIN1) \
	$(DOCS_LATIN2) \
	$(DOCS_LATIN3) \
	$(DOCS_LATIN4) \
	$(DOCS_LATIN5) \
	$(DOCS_LATIN9)

DOCS_CYR = \
	$(DOCS_CYR_1251) \
	$(DOCS_CYR_ISO88595) \
	$(DOCS_CYR_ISOIR111) \
	$(DOCS_CYR_KOI8_R) \
	$(DOCS_CYR_KOI8_U)

DOCS_UTF8 = \
	$(DOCS_UTF8_LAT) \
	$(DOCS_UTF8_CYR) \
	$(DOCS_UTF8_OTHER)

DOCS = $(DOCS_CPS) $(DOCS_LAT) $(DOCS_CYR) $(DOCS_OTHER) $(DOCS_UTF8)


alphabets-inc.tex : $(DOCS)
	$(RM) $@
# zsh, dash, others make echo '\f' output a form feed. Likewise they
# interpret \c (no further output). Sigh.
	printf '\%s{Latin scripts}' "chapter" >>$@
	echo '\ienc{cp1252}' >>$@
	echo '\section{Codepage 1252}' >>$@
	for i in $(DOCS_CP1252); do echo "\\input{$$i}" >>$@; done
	echo '\ienc{latin1}' >>$@
	echo '\section{Codepage latin1}' >>$@
	for i in $(DOCS_LATIN1); do echo "\\input{$$i}" >>$@; done
	echo '\ienc{cp850}' >>$@
	echo '\section{Codepage 850}' >>$@
	for i in $(DOCS_CP850); do echo "\\input{$$i}" >>$@; done
	echo '\ienc{latin2}' >>$@
	echo '\section{Codepage latin2}' >>$@
	for i in $(DOCS_LATIN2); do echo "\\input{$$i}" >>$@; done
	echo '\ienc{cp1250}' >>$@
	echo '\section{Codepage 1250}' >>$@
	for i in $(DOCS_CP1250); do echo "\\input{$$i}" >>$@; done
	echo '\ienc{latin3}' >>$@
	echo '\section{Codepage latin3}' >>$@
	for i in $(DOCS_LATIN3); do echo "\\input{$$i}" >>$@; done
	echo '\ienc{latin4}' >>$@
	echo '\section{Codepage latin4}' >>$@
	for i in $(DOCS_LATIN4); do echo "\\input{$$i}" >>$@; done
	echo '\ienc{latin5}' >>$@
	echo '\section{Codepage latin5}' >>$@
	for i in $(DOCS_LATIN5); do echo "\\input{$$i}" >>$@; done
	echo '\ienc{latin9}' >>$@
	echo '\section{Codepage latin9}' >>$@
	for i in $(DOCS_LATIN9); do echo "\\input{$$i}" >>$@; done
	echo '\ienc{cp1251}' >>$@
	printf '\%s{T2A}\n' "fenc" >>$@
	printf '\%s{Cyrillic scripts}' "chapter" >>$@
	echo '\section{Codepage 1251}' >>$@
	for i in $(DOCS_CYR_1251); do echo "\\input{$$i}" >>$@; done
#	echo '\ienc{iso-8859-7}' >>$@
#	echo '\fenc{LGR}' >>$@
#	echo '\chapter{Other scripts}' >>$@
#	for i in $(DOCS_OTHER); do echo "\\input{$$i}" >>$@; done
#	FIXME: UTF8 encoding
#	echo '\section{UTF-8 scripts}' >>$@
#	for i in $(DOCS_UTF8); do echo "\\input{$$i}" >>$@; done
#	echo '\ienc{utf8}' >>$@

# whether to build docs
if BUILDRULES

if BUILDDOCS
alphabets-doc.pdf : alphabets-doc.tex alphabets-inc.tex
	$(PDFLATEX) -interaction nonstopmode $(srcdir)/alphabets-doc.tex
	$(PDFLATEX) -interaction nonstopmode $(srcdir)/alphabets-doc.tex

PDFS = alphabets-doc.pdf

doc_DATA = $(PDFS)

endif BUILDDOCS

endif BUILDRULES

DOC_CLEAN = \
	alphabets-doc.aux  \
	alphabets-doc.log \
	alphabets-doc.out  \
	alphabets-doc.pdf \
	alphabets-inc.tex \
	alphabets-doc.toc

EXTRA_DIST += $(SCRS) test.xdy rom-comma-kludge.sed

CLEANFILES = */*.raw */*-doc.tex $(nobase_nodist_lang_DATA) $(DOC_CLEAN)
