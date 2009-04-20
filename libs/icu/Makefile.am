## Proxy Makefile.am to build icu-xetex for TeX Live.
##
##   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
ACLOCAL_AMFLAGS = -I ../../m4

SUBDIRS = dummy

## We want to re-distribute the whole icu-xetex source tree.
EXTRA_DIST = icu-xetex

## in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir)/icu-xetex -name .svn`

ICUSUB_AUX = icusubdir-configure icusubdir-conf.args
noinst_DATA = $(ICUSUB_AUX)

DISTCLEANFILES = $(noinst_DATA)

if build
all-local: icubuild
else !build
all-local: icu-build/Makefile
endif !build

icu-build/Makefile: $(noinst_DATA)
	$(mkdir_p) icu-build
	@cmd=`cat $(ICUSUB_AUX)`; \
	cmd="$$cmd --enable-static --disable-shared --disable-extras --disable-samples --disable-tests"; \
	(cd icu-build && echo "=== configuring in icu-build (`pwd`)" && \
	  echo "make: running $(SHELL) $$cmd" && \
	  eval $(SHELL) $$cmd)

.PHONY: icubuild
icubuild: icu-build/Makefile
	cd icu-build && $(MAKE) $(AM_MAKEFLAGS) all

distclean-local:
	rm -rf icu-build

icusubdir-configure:
	@echo "make in ./dummy failed to create the file $@"
	exit 1

icusubdir-conf.args:
	@echo "configure in ./dummy failed to create the file $@"
	exit 1

