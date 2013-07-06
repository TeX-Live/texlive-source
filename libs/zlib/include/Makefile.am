## Proxy Makefile.am to install zlib headers for TeX Live.
##
##   Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
ZLIB_SRC = $(top_srcdir)/$(ZLIB_TREE)
ZLIB_BLD = $(top_builddir)

hdr_links = \
	$(ZLIB_SRC)/zlib.h \
	$(ZLIB_BLD)/zconf.h

include $(top_srcdir)/../../am/hdr_links.am

