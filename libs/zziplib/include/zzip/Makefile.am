## Proxy Makefile.am to install zziplib headers for TeX Live.
##
##   Copyright (C) 2010-2013 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
ZZIPLIB_SRC = $(top_srcdir)/$(ZZIPLIB_TREE)/zzip

hdr_links = \
	$(ZZIPLIB_SRC)/autoconf.h \
	$(ZZIPLIB_SRC)/conf.h \
	$(ZZIPLIB_SRC)/fetch.h \
	$(ZZIPLIB_SRC)/file.h \
	$(ZZIPLIB_SRC)/format.h \
	$(ZZIPLIB_SRC)/info.h \
	$(ZZIPLIB_SRC)/lib.h \
	$(ZZIPLIB_SRC)/plugin.h \
	$(ZZIPLIB_SRC)/stdint.h \
	$(ZZIPLIB_SRC)/types.h \
	$(ZZIPLIB_SRC)/write.h \
	$(ZZIPLIB_SRC)/zzip.h

include $(top_srcdir)/../../am/hdr_links.am

