## Proxy Makefile.am to install ICU headers for TeX Live.
##
##   Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
ICU_BLD = $(top_builddir)/icu-build
ICU_SRC = $(top_srcdir)/$(ICU_TREE)/source

# ICU headers to be installed in include/unicode/
hdr_links = \
	$(ICU_BLD)/common/unicode/*.h \
	$(ICU_SRC)/common/unicode/*.h \
	$(ICU_SRC)/i18n/unicode/*.h \
	$(ICU_SRC)/io/unicode/*.h

include $(top_srcdir)/../../am/hdr_links.am

