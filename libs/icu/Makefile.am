## Proxy Makefile.am to build ICU for TeX Live.
##
##   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
ACLOCAL_AMFLAGS = -I ../../m4

# Rebuild
.PHONY: rebuild
rebuild: icubuild
	cd icu-incl && $(MAKE) $(AM_MAKEFLAGS) all

SUBDIRS = dummy .

## We want to re-distribute the whole ICU source tree.
EXTRA_DIST = $(ICU_TREE)

EXTRA_DIST += license.html

## Patches applied to the original source tree
##
EXTRA_DIST +=  $(ICU_TREE)-PATCHES

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn`

ICUSUB_AUX = icusubdir-configure icusubdir-conf.args
noinst_DATA = $(ICUSUB_AUX)

DISTCLEANFILES = $(noinst_DATA) icu-build.args icu-native.args

if build
all-local: icubuild
SUBDIRS += icu-incl
else !build
all-local: icu-build/Makefile
endif !build

if cross
ICU_NATIVE = icu-native/config/icucross.mk
endif cross

icu-build/Makefile: $(ICUSUB_AUX) icu-build.args $(ICU_NATIVE)
	$(mkdir_p) icu-build
	@cmd=`cat $(ICUSUB_AUX) icu-build.args | sed 's,ICUSUBDIR,$(ICU_TREE),g'`; \
	(cd icu-build && echo "=== configuring in icu-build (`pwd`)" && \
	  echo "make: running $(SHELL) $$cmd" && \
	  CONFIG_SHELL=$(SHELL) && export CONFIG_SHELL && \
	  eval $(SHELL) $$cmd)

icu-native/config/icucross.mk: icu-native/Makefile
	cd icu-native && $(MAKE) $(AM_MAKEFLAGS) config/icucross.mk

icu-native/Makefile: $(ICUSUB_AUX) icu-native.args
	$(mkdir_p) icu-native
	@cmd=`cat $(ICUSUB_AUX) icu-native.args | sed 's,ICUSUBDIR,$(ICU_TREE),g'`; \
	(cd icu-native && echo "=== configuring in icu-native (`pwd`)" && \
	  echo "make: running $(SHELL) $$cmd" && \
	  CONFIG_SHELL=$(SHELL) && export CONFIG_SHELL && \
	  eval $(SHELL) $$cmd)

.PHONY: icubuild check-makeflags

icubuild: icu-build/Makefile check-makeflags
if cross
	cd icu-native && $(MAKE) $(AM_MAKEFLAGS) all
endif cross
	cd icu-build && $(MAKE) $(AM_MAKEFLAGS) all

check-makeflags:
	@for f in x $$MAKEFLAGS; do \
	  case $$f in \
	    CFLAGS=* | CPPFLAGS=* | CXXFLAGS=* | LDFLAGS=*) \
	      echo "Sorry, the icu-xetex build systems disallows \`make $$f'."; \
	      exit 1;; \
	  esac; \
	done

distclean-local:
	rm -rf icu-build icu-native

icusubdir-configure:
	cd dummy && $(MAKE) $(AM_MAKEFLAGS) ../$@

icusubdir-conf.args icu-build.args icu-native.args:
	@echo "configure in ./dummy failed to create the file $@"
	exit 1

