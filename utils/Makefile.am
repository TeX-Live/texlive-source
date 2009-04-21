## Makefile.am for the TeX Live subdirectory utils/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../m4

SUBDIRS = dummy . $(SUBUTILS)
DIST_SUBDIRS = dummy $(DIST_SUBUTILS)

EXTRA_DIST = README.orig

## just in case a subdirectory has added directories
dist-hook:
	rm -rf `find $(distdir) -name .svn`

SUBUTILS_AUX = subutilsdir-configure subutilsdir-conf.args
noinst_DATA = $(SUBUTILS_AUX)

DISTCLEANFILES = $(noinst_DATA)

all-local: subutils

.PHONY: subutils
## We must configure all utils subdirs since 'make dist' needs the Makefile.
## For those not required for the current set of configure options
## we append '--disable-build' so they can skip tests that would
## fail because, e.g., some libraries were not built.

## Sequentially configure and optionally build all utils subdirs;
## code inspired by automake's way to handle recursive targets.
subutils: $(noinst_DATA)
	@failcom='exit 1'; \
	for f in x $$MAKEFLAGS; do \
	  case $$f in \
	    *=* | --[!k]*);; \
	    *k*) failcom='fail=yes';; \
	  esac; \
	done; \
	list='$(DIST_SUBUTILS)'; for subutils in $$list; do \
	  if test ! -f $$subutils/Makefile; then \
	    test -d $$subutils || $(MKDIR_P) $$subutils; \
	    cmd=`cat $(SUBUTILS_AUX) | sed "s,SUBUTILSDIR,$$subutils,g"`; \
	    case " $(SUBUTILS) " in \
	      *" $$subutils "*) skip=;; \
	      *) skip=' --disable-build';; \
	    esac; \
	    (cd $$subutils && echo "=== configuring in $$subutils (`pwd`)" && \
	      echo "make: running $(SHELL) $$cmd$$skip" && \
	      eval $(SHELL) $$cmd$$skip) && \
	    if test "x$$skip" = x; then \
	      echo "Making all in $$subutils" && \
	      (cd $$subutils && $(MAKE) $(AM_MAKEFLAGS) all); \
	    fi || eval $$failcom; \
	  fi; \
	done; test -z "$$fail"

subutilsdir-configure:
	@echo "make in ./dummy failed to create the file $@"
	exit 1

subutilsdir-conf.args:
	@echo "configure in ./dummy failed to create the file $@"
	exit 1

