## Proxy Makefile.am to install libjpeg headers for TeX Live.
##
##   Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
LIBJPEG_SRC = $(top_srcdir)/$(LIBJPEG_TREE)

hdr_links = \
        $(LIBJPEG_SRC)/jconfig.h \
	$(LIBJPEG_SRC)/jerror.h \
	$(LIBJPEG_SRC)/jinclude.h \
	$(LIBJPEG_SRC)/jmorecfg.h \
	$(LIBJPEG_SRC)/jpeglib.h

include $(top_srcdir)/../../../../am/hdr_links.am

