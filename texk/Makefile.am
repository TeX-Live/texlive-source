## Makefile.am for the TeX Live subdirectory texk/
##
## Copyright (C) 2009-2012 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
## Use 'aclocal --force' (for "computed m4_sinclude")
##
ACLOCAL_AMFLAGS = -I ../m4 --force

SUBDIRS = . $(SUBTEXK)
DIST_SUBDIRS = $(DIST_SUBTEXK)

cf_silent = $(cf_silent_@AM_V@)
cf_silent_ = $(cf_silent_@AM_DEFAULT_V@)
cf_silent_0 = --silent
cf_silent_1 =

# just in case a subdirectory has added directories
dist-hook:
	rm -rf `find $(distdir) -name .svn`

CONFIG_AUX = ../subsubdir-conf.cmd

all-local: subtexk

.PHONY: subtexk
# We must configure all texk subdirs since 'make dist' needs the Makefile.
# For those not required for the current set of configure options
# we append '--disable-build' so they can skip tests that would
# fail because, e.g., some required libraries were not built.
#
# Sequentially configure and optionally build all texk programs;
# code inspired by automake's way to handle recursive targets.
subtexk: $(CONFIG_AUX)
	@fail= failcom='exit 1'; \
	for f in x $$MAKEFLAGS; do \
	  case $$f in \
	    *=* | --[!k]*);; \
	    *k*) failcom='fail=yes';; \
	  esac; \
	done; \
	list='$(DIST_SUBTEXK)'; for subtexk in $$list; do \
	  if test ! -f $$subtexk/Makefile; then \
	    test -d $$subtexk || $(MKDIR_P) $$subtexk; \
	    cmd=`cat $(CONFIG_AUX) | sed "s,auxdir/auxsub,texk/$$subtexk,g"`; \
	    case " $(SUBTEXK) " in \
	      *" $$subtexk "*) skip=;; \
	      *) skip=' --disable-build';; \
	    esac; \
	    (cd $$subtexk && echo "=== configuring in $$subtexk (`pwd`)" && \
	      if $(AM_V_P); then echo "make: running $(SHELL) $$cmd$$skip"; fi && \
	      CONFIG_SHELL=$(SHELL) && export CONFIG_SHELL && \
	      eval $(SHELL) $$cmd$$skip $(cf_silent) || \
	        { echo "=== configuring in $$subtexk failed"; exit 1; }) && \
	    if test "x$$skip" = x; then \
	      echo "Making all in $$subtexk" && \
	      (cd $$subtexk && $(MAKE) $(AM_MAKEFLAGS) all); \
	    fi || eval $$failcom; \
	  fi; \
	done; test -z "$$fail"

../subsubdir-conf.cmd:
	@echo "configure in ../auxdir/auxsub failed to create the file $@"
	exit 1

# Common infrastructure for tests
EXTRA_DIST = tests

# Special target to run TRIP and TRAP tests and create diffs.
.PHONY: triptrap
triptrap: web2c/Makefile
	cd web2c && $(MAKE) $(AM_MAKEFLAGS) $@
web2c/Makefile:
	$(MAKE) $(AM_MAKEFLAGS) DIST_SUBTEXK=web2c subtexk
