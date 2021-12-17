## $Id$
## Wrapper Makefile.am to build ICU for TeX Live.
##
##   Copyright 2017-2021 Karl Berry <tex-live@tug.org>
##   Copyright 2009-2015 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
## We want to re-distribute the whole ICU source tree.
EXTRA_DIST = $(ICU_TREE)

## Patches applied to the original source tree
##
EXTRA_DIST += TLpatches

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am

SUBDIRS = .

if build
all-local: icubuild
SUBDIRS += include/unicode
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
# these two cp commands are a kludge for mingw cross compilation.
	cd icu-build/lib && (test -r libicudata.a || cp libicudt.a libicudata.a || cp icudt.a libicudata.a)
	cd icu-build/lib && (test -r libicui18n.a || cp libicuin.a libicui18n.a)
#
	cd include/unicode && $(MAKE) $(AM_MAKEFLAGS) all

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

if build
check_PROGRAMS = icutest
dist_check_SCRIPTS = icu.test
TESTS = icu.test
endif build

icutest_SOURCES = icutest.c

# Force Automake to use CXXLD for linking
nodist_EXTRA_icutest_SOURCES = dummy.cxx

icutest_CPPFLAGS = -Iinclude

LDADD = icu-build/lib/libicuuc.a
if cross
LDADD += icu-build/lib/libicuuc.a
else
LDADD += icu-build/lib/libicudata.a 
endif
LDADD += $(ICU_LIBS_EXTRA)

# Rebuild
rebuild_prereq =
rebuild_target = icubuild
CLEANFILES =

include $(srcdir)/../../am/rebuild.am
