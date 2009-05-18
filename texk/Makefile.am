## Makefile.am for the TeX Live subdirectory texk/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
## Use 'aclocal --force' (for "computed m4_sinclude")
##
ACLOCAL_AMFLAGS = -I ../m4 --force

SUBDIRS = dummy . $(SUBTEXK)
DIST_SUBDIRS = dummy $(DIST_SUBTEXK)

# just in case a subdirectory has added directories
dist-hook:
	rm -rf `find $(distdir) -name .svn`

SUBTEXK_AUX = subtexkdir-configure subtexkdir-conf.args
noinst_DATA = $(SUBTEXK_AUX)

DISTCLEANFILES = $(noinst_DATA)

all-local: subtexk

.PHONY: subtexk
# We must configure all texk subdirs since 'make dist' needs the Makefile.
# For those not required for the current set of configure options
# we append '--disable-build' so they can skip tests that would
# fail because, e.g., some required libraries were not built.
#
# Sequentially configure and optionally build all texk subdirs;
# code inspired by automake's way to handle recursive targets.
subtexk: $(noinst_DATA)
	@failcom='exit 1'; \
	for f in x $$MAKEFLAGS; do \
	  case $$f in \
	    *=* | --[!k]*);; \
	    *k*) failcom='fail=yes';; \
	  esac; \
	done; \
	list='$(DIST_SUBTEXK)'; for subtexk in $$list; do \
	  if test ! -f $$subtexk/Makefile; then \
	    test -d $$subtexk || $(MKDIR_P) $$subtexk; \
	    cmd=`cat $(SUBTEXK_AUX) | sed "s,SUBTEXKDIR,$$subtexk,g"`; \
	    case " $(SUBTEXK) " in \
	      *" $$subtexk "*) skip=;; \
	      *) skip=' --disable-build';; \
	    esac; \
	    (cd $$subtexk && echo "=== configuring in $$subtexk (`pwd`)" && \
	      echo "make: running $(SHELL) $$cmd$$skip" && \
	      eval $(SHELL) $$cmd$$skip) && \
	    if test "x$$skip" = x; then \
	      echo "Making all in $$subtexk" && \
	      (cd $$subtexk && $(MAKE) $(AM_MAKEFLAGS) all); \
	    fi || eval $$failcom; \
	  fi; \
	done; test -z "$$fail"

subtexkdir-configure:
	@echo "make in ./dummy failed to create the file $@"
	exit 1

subtexkdir-conf.args:
	@echo "configure in ./dummy failed to create the file $@"
	exit 1

