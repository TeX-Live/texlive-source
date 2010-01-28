## Proxy Makefile.am to install headers headers for TeX Live.
##
##   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
GD_SRC = $(top_srcdir)/$(GD_TREE)

GD_HDRS = \
	$(GD_SRC)/entities.h \
	$(GD_SRC)/gd.h \
	$(GD_SRC)/gd_io.h \
	$(GD_SRC)/gdcache.h \
	$(GD_SRC)/gdfontg.h \
	$(GD_SRC)/gdfontl.h \
	$(GD_SRC)/gdfontmb.h \
	$(GD_SRC)/gdfonts.h \
	$(GD_SRC)/gdfontt.h \
	$(GD_SRC)/gdfx.h \
	$(GD_SRC)/gdhelpers.h \
	$(GD_SRC)/jisx0208.h \
	$(GD_SRC)/wbmp.h

all-local:
	@for file in $(GD_HDRS); do \
	  test -f $$file || continue; \
	  inst=`echo $$file | sed -e 's/^.*\///'`; \
	  test -f $$inst || { \
	    echo "$(LN_S) $$file $$inst"; \
	    $(LN_S) $$file $$inst; } || exit 1; \
	done

distclean-local:
	rm -f *.h



