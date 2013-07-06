## Proxy Makefile.am to install pixman headers for TeX Live.
##
##   Copyright (C) 2012-2013 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
PIXMAN_SRC = $(top_srcdir)/$(PIXMAN_TREE)/pixman
PIXMAN_BLD = $(top_builddir)

hdr_links = \
	$(PIXMAN_SRC)/pixman.h \
	$(PIXMAN_BLD)/pixman-version.h

include $(top_srcdir)/../../am/hdr_links.am

