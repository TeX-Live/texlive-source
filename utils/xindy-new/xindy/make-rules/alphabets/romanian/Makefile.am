## Makefile fragment for romanian.

## Copyright (C) 2009 by Peter Breitenlohner.
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

subdirs += romanian

nobase_nodist_lang_DATA += \
	romanian/latin2-lang.xdy romanian/latin2-test.xdy romanian/latin2.xdy \
	romanian/utf8-lang.xdy romanian/utf8-test.xdy romanian/utf8.xdy

DOCS_LATIN2 += \
	romanian/latin2-doc.tex

DOCS_UTF8_LAT += \
	romanian/utf8-doc.tex

romanian/cp1250.pl : romanian/utf8.pl
	sed -e 's/@u{\([^}]*\)}//g;' -f $(srcdir)/rom-comma-kludge.sed <$< | \
	$(CONVERT_FROM_UTF8)CP1250 >$@

romanian/latin2.pl : romanian/utf8.pl
	sed -e 's/@u{\([^}]*\)}//g;' -f $(srcdir)/rom-comma-kludge.sed <$< | \
	$(CONVERT_FROM_UTF8)ISO8859-2 >$@

EXTRA_DIST += \
	rom-comma-kludge.sed \
	romanian/utf8.pl.in

CLEANFILES += \
	romanian/cp1250.raw romanian/cp1250.pl \
	romanian/latin2.pl romanian/latin2.raw \
	romanian/utf8.pl romanian/utf8.raw

