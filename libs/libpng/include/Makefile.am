## Proxy Makefile.am to install libpng headers for TeX Live.
##
##   Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
LIBPNG_SRC = $(top_srcdir)/$(LIBPNG_TREE)

hdr_links = \
	$(LIBPNG_SRC)/png.h \
	$(LIBPNG_SRC)/pngconf.h \
	$(LIBPNG_SRC)/pnglibconf.h

include $(top_srcdir)/../../am/hdr_links.am

