## Wrapper Makefile.am to build ICU for TeX Live.
##
##   Copyright (C) 2009-2012 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
ACLOCAL_AMFLAGS = -I ../../m4

# Rebuild
.PHONY: rebuild
rebuild: icubuild
	cd include && $(MAKE) $(AM_MAKEFLAGS) all

SUBDIRS = .

## We want to re-distribute the whole ICU source tree.
EXTRA_DIST = $(ICU_TREE) license.html

## Patches applied to the original source tree
##
EXTRA_DIST += $(ICU_TREE)-PATCHES

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn`

if build
all-local: icubuild
SUBDIRS += include
else !build
all-local: icu-build/Makefile
endif !build

if cross
ICU_NATIVE = icu-native/config/icucross.mk
endif cross

icu-build/Makefile: $(ICU_NATIVE)
	$(MKDIR_P) icu-build
	@cmd="$(icu_config) $(icu_build_args)"; \
	(cd icu-build && echo "=== configuring in icu-build (`pwd`)" && \
	  echo "make: running $(SHELL) $$cmd" && \
	  CONFIG_SHELL=$(SHELL) && export CONFIG_SHELL && \
	  eval $(SHELL) $$cmd)

icu-native/config/icucross.mk: icu-native/Makefile
	cd icu-native && $(MAKE) $(AM_MAKEFLAGS) config/icucross.mk

icu-native/Makefile:
	$(MKDIR_P) icu-native
	@cmd="$(icu_config) $(icu_native_args)"; \
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
	      echo "Sorry, the icu build systems disallows \`make $$f'."; \
	      exit 1;; \
	  esac; \
	done

distclean-local:
	rm -rf icu-build icu-native

