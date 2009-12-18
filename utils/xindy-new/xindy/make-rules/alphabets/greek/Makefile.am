## Makefile fragment for greek.

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

subdirs += greek

nobase_nodist_lang_DATA += \
	greek/iso88597-lang.xdy greek/iso88597-test.xdy greek/iso88597.xdy \
	greek/polytonic-utf8-lang.xdy greek/polytonic-utf8-test.xdy greek/polytonic-utf8.xdy \
	greek/translit-latin4-lang.xdy greek/translit-latin4-test.xdy greek/translit-latin4.xdy \
	greek/translit-utf8-lang.xdy greek/translit-utf8-test.xdy greek/translit-utf8.xdy \
	greek/utf8-lang.xdy greek/utf8-test.xdy greek/utf8.xdy

DOCS_LATIN4 += \
	greek/translit-latin4-doc.tex

DOCS_OTHER += \
	greek/iso88597-doc.tex

DOCS_UTF8_LAT += \
	greek/translit-utf8-doc.tex \
	greek/utf8-doc.tex

DOCS_UTF8_OTHER += \
	greek/utf8-doc.tex \
	greek/polytonic-utf8-doc.tex

EXTRA_DIST += \
	greek/polytonic-utf8.pl.in \
	greek/translit-utf8.pl.in \
	greek/utf8.pl.in


CLEANFILES += \
	greek/iso88597.pl greek/iso88597.raw \
	greek/polytonic-utf8.pl greek/polytonic-utf8.raw \
	greek/translit-latin4.pl greek/translit-latin4.raw \
	greek/translit-utf8.pl greek/translit-utf8.raw \
	greek/utf8.pl greek/utf8.raw

