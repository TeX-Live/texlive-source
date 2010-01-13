## Makefile fragment for hebrew.

## Copyright (C) 2009, 2010 by Peter Breitenlohner.
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

subdirs += hebrew

nobase_nodist_lang_DATA += \
	hebrew/utf8-lang.xdy \
	hebrew/utf8-test.xdy \
	hebrew/utf8.xdy \
	hebrew/iso88598-lang.xdy \
	hebrew/iso88598-test.xdy \
	hebrew/iso88598.xdy

DOCS_ISO88598 += \
	hebrew/iso88598-doc.tex

DOCS_UTF8_OTHER += \
	hebrew/utf8-doc.tex

EXTRA_DIST += \
	hebrew/utf8.pl.in

dist_noinst_DATA += \
	hebrew/iso88598.pl

CLEANFILES += \
	hebrew/utf8.pl \
	hebrew/utf8.raw \
	hebrew/iso88598.raw

