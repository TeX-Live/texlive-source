## Proxy Makefile.am to install TECkit headers for TeX Live.
##
##   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
TECKIT_SRC = $(top_srcdir)/$(TECKIT_TREE)/source/Public-headers

TECKIT_HDRS = \
	$(TECKIT_SRC)/TECkit_Common.h \
	$(TECKIT_SRC)/TECkit_Compiler.h \
	$(TECKIT_SRC)/TECkit_Engine.h

all-local:
	@for file in $(TECKIT_HDRS); do \
	  test -f $$file || continue; \
	  inst=`echo $$file | sed -e 's/^.*\///'`; \
	  test -f $$inst || { \
	    echo "$(LN_S) $$file $$inst"; \
	    $(LN_S) $$file $$inst; } || exit 1; \
	done

distclean-local:
	rm -f *.h

