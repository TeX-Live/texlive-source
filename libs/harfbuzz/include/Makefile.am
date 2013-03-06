## Proxy Makefile.am to install harfbuzz headers for TeX Live.
##
##   Copyright (C) 2012 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
HARFBUZZ_SRC = $(top_srcdir)/$(HARFBUZZ_TREE)/src
HARFBUZZ_BLD = $(top_builddir)

## common
hdr_links = \
	$(HARFBUZZ_SRC)/hb.h \
	$(HARFBUZZ_SRC)/hb-blob.h \
	$(HARFBUZZ_SRC)/hb-buffer.h \
	$(HARFBUZZ_SRC)/hb-common.h \
	$(HARFBUZZ_SRC)/hb-font.h \
	$(HARFBUZZ_SRC)/hb-set.h \
	$(HARFBUZZ_SRC)/hb-shape.h \
	$(HARFBUZZ_SRC)/hb-shape-plan.h \
	$(HARFBUZZ_SRC)/hb-unicode.h \
	$(HARFBUZZ_BLD)/hb-version.h

## native OpenType Layout backend
hdr_links += \
	$(HARFBUZZ_SRC)/hb-ot.h \
	$(HARFBUZZ_SRC)/hb-ot-layout.h \
	$(HARFBUZZ_SRC)/hb-ot-tag.h

## ICU library
hdr_links += $(HARFBUZZ_SRC)/hb-icu.h

## Graphite library
hdr_links += $(HARFBUZZ_SRC)/hb-graphite2.h

include $(top_srcdir)/../am/hdr_links.am

