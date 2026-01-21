## Proxy Makefile.am to install libtiff headers for TeX Live.
##
##   Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
LIBTIFF_SRC = $(top_srcdir)/$(LIBTIFF_TREE)

hdr_links = \
        $(LIBTIFF_SRC)/tif_config.h \
        $(LIBTIFF_SRC)/tiffconf.h \
        $(LIBTIFF_SRC)/tiff.h \
        $(LIBTIFF_SRC)/tiffvers.h \
        $(LIBTIFF_SRC)/tiffiop.h \
        $(LIBTIFF_SRC)/tiffio.h \
        $(LIBTIFF_SRC)/tif_dir.h \
        $(LIBTIFF_SRC)/tif_hash_set.h

include $(top_srcdir)/../../../../am/hdr_links.am

