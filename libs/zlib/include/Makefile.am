## Proxy Makefile.am to install zlib headers for TeX Live.
##
##   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
ZLIB_SRC = $(top_srcdir)/$(ZLIB_TREE)
ZLIB_BLD = $(top_builddir)

ZLIB_HDRS = \
	$(ZLIB_SRC)/zlib.h \
	$(ZLIB_BLD)/zconf.h

all-local:
	@for file in $(ZLIB_HDRS); do \
	  test -f $$file || continue; \
	  inst=`echo $$file | sed -e 's/^.*\///'`; \
	  test -f $$inst || { \
	    echo "$(LN_S) $$file $$inst"; \
	    $(LN_S) $$file $$inst; } || exit 1; \
	done

distclean-local:
	rm -f *.h



