## Makefile.am for the TeX Live subdirectory utils/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
## Use 'aclocal --force' (for "computed m4_sinclude")
##
ACLOCAL_AMFLAGS = -I ../m4 --force

SUBDIRS = . $(SUBUTILS)
DIST_SUBDIRS = $(DIST_SUBUTILS)

# just in case a subdirectory has added directories
dist-hook:
	rm -rf `find $(distdir) -name .svn`

CONFIG_AUX = ../subsubdir-conf.cmd

all-local: subutils

.PHONY: subutils
# We must configure all utils subdirs since 'make dist' needs the Makefile.
# For those not required for the current set of configure options
# we append '--disable-build' so they can skip tests that would
# fail because, e.g., some required libraries were not built.
#
# Sequentially configure and optionally build all utils subdirs;
# code inspired by automake's way to handle recursive targets.
subutils: $(CONFIG_AUX)
	@fail= failcom='exit 1'; \
	for f in x $$MAKEFLAGS; do \
	  case $$f in \
	    *=* | --[!k]*);; \
	    *k*) failcom='fail=yes';; \
	  esac; \
	done; \
	list='$(DIST_SUBUTILS)'; for subutils in $$list; do \
	  if test ! -f $$subutils/Makefile; then \
	    test -d $$subutils || $(MKDIR_P) $$subutils; \
	    cmd=`cat $(CONFIG_AUX) | sed "s,auxdir/auxsub,utils/$$subutils,g"`; \
	    case " $(SUBUTILS) " in \
	      *" $$subutils "*) skip=;; \
	      *) skip=' --disable-build';; \
	    esac; \
	    (cd $$subutils && echo "=== configuring in $$subutils (`pwd`)" && \
	      echo "make: running $(SHELL) $$cmd$$skip" && \
	      CONFIG_SHELL=$(SHELL) && export CONFIG_SHELL && \
	      eval $(SHELL) $$cmd$$skip || \
	        { echo "=== configuring in $$subutils failed"; exit 1; }) && \
	    if test "x$$skip" = x; then \
	      echo "Making all in $$subutils" && \
	      (cd $$subutils && $(MAKE) $(AM_MAKEFLAGS) all); \
	    fi || eval $$failcom; \
	  fi; \
	done; test -z "$$fail"

../subsubdir-conf.cmd:
	@echo "configure in ../auxdir/auxsub failed to create the file $@"
	exit 1

