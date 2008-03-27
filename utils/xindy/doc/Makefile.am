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

SUBDIRS = style-tutorial

FILES = \
	manual.html \
	manual-1.html \
	manual-2.html \
	manual-3.html \
	manual-4.html \
	manual-5.html \
	manual-6.html \
	manual-7.html \
	faq.html \
	faq-1.html \
	faq-2.html \
	faq-3.html \
	faq-4.html \
	style-tutorial.html \
	style-tutorial-1.html \
	style-tutorial-2.html \
	style-tutorial-3.html \
	style-tutorial-4.html \
	dataflow.gif \
	index.gif \
	mappings.gif \
	README

PDFS = xindy.pdf texindy.pdf tex2xindy.pdf

EXTRA_DIST = $(FILES) $(PDFS)

dist_noinst_DATA = xindy.v2.pdf makeindex4.pdf

doc_DATA = $(FILES) $(PDFS)

if MAINTAINER_MODE
GROFF = groff

$(srcdir)/xindy.pdf: $(top_srcdir)/user-commands/xindy.1
	$(GROFF) -man -P-g $< | ps2pdf - $@

$(srcdir)/texindy.pdf: $(top_srcdir)/user-commands/texindy.1
	$(GROFF) -man -P-g $< | ps2pdf - $@

$(srcdir)/makeindex4.pdf: $(top_srcdir)/user-commands/makeindex4.1
	$(GROFF) -man -P-g $< | ps2pdf - $@

$(srcdir)/xindy.v2.pdf: $(top_srcdir)/user-commands/xindy.v2.1
	$(GROFF) -man -P-g $< | ps2pdf - $@

$(srcdir)/tex2xindy.pdf: $(top_srcdir)/tex2xindy/tex2xindy.1
	$(GROFF) -man -P-g $< | ps2pdf - $@
endif
