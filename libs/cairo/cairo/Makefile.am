## Proxy Makefile.am to install cairo headers for TeX Live.
##
##   Copyright (C) 2013 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
CAIRO_SRC = $(top_srcdir)/$(CAIRO_TREE)
CAIRO_BLD = $(top_builddir)

hdr_links = \
	$(CAIRO_SRC)/src/cairo.h \
	$(CAIRO_SRC)/src/cairo-deprecated.h \
	$(CAIRO_BLD)/cairo-features.h \
	$(CAIRO_SRC)/cairo-version.h

include $(top_srcdir)/../../am/hdr_links.am

