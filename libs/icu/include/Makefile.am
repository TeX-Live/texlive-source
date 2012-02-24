## Proxy Makefile.am to install ICU headers for TeX Live.
##
##   Copyright (C) 2009-2012 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
SUBDIRS = layout unicode

ICU_SRC = $(top_srcdir)/$(ICU_TREE)

# ICU headers to be installed in include/
hdr_links = \
	$(ICU_SRC)/common/cmemory.h

include $(top_srcdir)/../am/hdr_links.am

