## Proxy Makefile.am to install libpng headers for TeX Live.
##
##   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
LIBPNG_SRC = $(top_srcdir)/$(LIBPNG_TREE)

LIBPNG_HDRS = \
	$(LIBPNG_SRC)/png.h \
	$(LIBPNG_SRC)/pngconf.h

all-local:
	@for file in $(LIBPNG_HDRS); do \
	  test -f $$file || continue; \
	  inst=`echo $$file | sed -e 's/^.*\///'`; \
	  test -f $$inst || { \
	    echo "$(LN_S) $$file $$inst"; \
	    $(LN_S) $$file $$inst; } || exit 1; \
	done

distclean-local:
	rm -f *.h

