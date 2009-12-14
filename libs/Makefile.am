## Makefile.am for the TeX Live subdirectory libs/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
## Use 'aclocal --force' (for "computed m4_sinclude")
##
ACLOCAL_AMFLAGS = -I ../m4 --force

SUBDIRS = . $(SUBLIBS)
DIST_SUBDIRS = $(DIST_SUBLIBS)

# just in case a subdirectory has added directories
dist-hook:
	rm -rf `find $(distdir) -name .svn`

CONFIG_AUX = ../subsubdir-conf.cmd

all-local: sublibs

.PHONY: sublibs
# We must configure all libs subdirs since 'make dist' needs the Makefile.
# For those not required for the current set of configure options
# we append '--disable-build' so they can skip tests that would
# fail because other libraries were not built.
#
# Sequentially configure and optionally build all libs subdirs;
# code inspired by automake's way to handle recursive targets.
sublibs: $(CONFIG_AUX)
	@fail= failcom='exit 1'; \
	for f in x $$MAKEFLAGS; do \
	  case $$f in \
	    *=* | --[!k]*);; \
	    *k*) failcom='fail=yes';; \
	  esac; \
	done; \
	list='$(DIST_SUBLIBS)'; for sublibs in $$list; do \
	  if test ! -f $$sublibs/Makefile; then \
	    test -d $$sublibs || $(MKDIR_P) $$sublibs; \
	    cmd=`cat $(CONFIG_AUX) | sed "s,auxdir/auxsub,libs/$$sublibs,g"`; \
	    case " $(SUBLIBS) " in \
	      *" $$sublibs "*) skip=;; \
	      *) skip=' --disable-build';; \
	    esac; \
	    (cd $$sublibs && echo "=== configuring in $$sublibs (`pwd`)" && \
	      echo "make: running $(SHELL) $$cmd$$skip" && \
	      CONFIG_SHELL=$(SHELL) && export CONFIG_SHELL && \
	      eval $(SHELL) $$cmd$$skip || \
	        { echo "=== configuring in $$sublibs failed"; exit 1; }) && \
	    if test "x$$skip" = x; then \
	      echo "Making all in $$sublibs" && \
	      (cd $$sublibs && $(MAKE) $(AM_MAKEFLAGS) all); \
	    fi || eval $$failcom; \
	  fi; \
	done; test -z "$$fail"

../subsubdir-conf.cmd:
	@echo "configure in ../auxdir/auxsub failed to create the file $@"
	exit 1

