## Use `autoreconf' in top-level directory to recreate all
## autoconf/automake files.
## Or, call `automake Makefile' to create only Makefile.in.

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

modulesdir = $(pkglibdir)/modules

inputencdir = $(modulesdir)/tex/inputenc

INPUTENCS = \
	cp1250.xdy \
	cp1251.xdy \
	cp1252.xdy \
	cp437.xdy \
	cp850.xdy \
	cp855.xdy \
	cp866.xdy \
	iso88595.xdy \
	isoir111.xdy \
	koi8-r.xdy \
	koi8-u.xdy \
	latin.xdy \
	latin1.xdy \
	latin2.xdy \
	latin3.xdy \
	utf8.xdy

latin.xdy : latin1.xdy latin2.xdy latin3.xdy
	echo ';; Generated from latin?.xdy' >latin.xdy
	echo >>latin.xdy
	sort -u latin?.xdy | grep -v '^;' >>latin.xdy

utf8.xdy : latin1.xdy latin2.xdy latin3.xdy
	iconv -t UTF-8 -f ISO8859-1 latin1.xdy >latin1-utf8.xdy
	iconv -t UTF-8 -f ISO8859-2 latin2.xdy >latin2-utf8.xdy
	iconv -t UTF-8 -f ISO8859-3 latin3.xdy >latin3-utf8.xdy
	echo ';; Generated from recoded latin?.xdy' >utf8.xdy
	echo >>utf8.xdy
	sort -u latin?-utf8.xdy | grep -v '^;' >>utf8.xdy

%.xdy %.idx :
	$(PERL) $(srcdir)/make-testidx.pl TS1,T2A,T1 $* >$*.tex
	$(LATEX) -interaction nonstopmode $*.tex
	$(PERL) $(srcdir)/make-inp-rules.pl <$*.idx >$*.xdy

PERLSCRIPTS = \
	make-inp-rules.pl \
	make-testidx.pl


AUXS = \
	cp1250.aux \
	cp1251.aux \
	cp1252.aux \
	cp437.aux \
	cp850.aux \
	cp855.aux \
	cp866.aux \
	iso88595.aux \
	isoir111.aux \
	koi8-r.aux \
	koi8-u.aux \
	latin1.aux \
	latin2.aux \
	latin3.aux

DVIS = \
	cp1250.dvi \
	cp1251.dvi \
	cp1252.dvi \
	cp437.dvi \
	cp850.dvi \
	cp855.dvi \
	cp866.dvi \
	iso88595.dvi \
	isoir111.dvi \
	koi8-r.dvi \
	koi8-u.dvi \
	latin1.dvi \
	latin2.dvi \
	latin3.dvi

IDXS = \
	cp1250.idx \
	cp1251.idx \
	cp1252.idx \
	cp437.idx \
	cp850.idx \
	cp855.idx \
	cp866.idx \
	iso88595.idx \
	isoir111.idx \
	koi8-r.idx \
	koi8-u.idx \
	latin1.idx \
	latin2.idx \
	latin3.idx

LOGS = \
	cp1250.log \
	cp1251.log \
	cp1252.log \
	cp437.log \
	cp850.log \
	cp855.log \
	cp866.log \
	iso88595.log \
	isoir111.log \
	koi8-r.log \
	koi8-u.log \
	latin1.log \
	latin2.log \
	latin3.log

TEXS = \
	cp1250.tex \
	cp1251.tex \
	cp1252.tex \
	cp437.tex \
	cp850.tex \
	cp855.tex \
	cp866.tex \
	iso88595.tex \
	isoir111.tex \
	koi8-r.tex \
	koi8-u.tex \
	latin1.tex \
	latin2.tex \
	latin3.tex

UTF8 = \
	latin1-utf8.xdy \
	latin2-utf8.xdy \
	latin3-utf8.xdy

EXTRA_DIST = $(PERLSCRIPTS)

CLEANFILES = $(AUXS) $(DVIS) $(IDXS) $(INPUTENCS) $(LOGS) $(TEXS) $(UTF8)

nodist_inputenc_DATA = $(INPUTENCS)
