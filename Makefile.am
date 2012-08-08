## Makefile.am for the TeX Live top-level
##
## Copyright (C) 2009-2011 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
## Use 'aclocal --force' (for "computed m4_sinclude")
##
ACLOCAL_AMFLAGS = -I m4 --force

## speed up distcheck
AM_DISTCHECK_CONFIGURE_FLAGS = -C
## check as well for symlinks
distuninstallcheck_listfiles = find . -type f -o -type l -print
distcleancheck_listfiles = find . -type f -o -type l -print

EXTRA_DIST = \
	Build \
	README.coding \
	README.config \
	README.cross \
	README.distro \
	README.hacking \
	README.layout \
	build-aux/README.TL \
	m4/ChangeLog \
	m4/README \
	reautoconf

# just in case a subdirectory has added directories
dist-hook:
	rm -rf `find $(distdir) -name .svn`

CONFIG_AUX = subsubdir-conf.cmd

SUBDIRS = . $(TEXLIBS) libs utils texk
DIST_SUBDIRS = auxdir/auxsub $(DIST_TEXLIBS) libs utils texk

all-local: texlibs

.PHONY: texlibs
# We must configure all texk subdirs since 'make dist' needs the Makefile.
# For those not required for the current set of configure options
# we append '--disable-build' so they can skip tests that would
# fail because other libraries were not built.
#
# Sequentially configure and optionally build all texk subdirs;
# code inspired by automake's way to handle recursive targets.
texlibs: $(CONFIG_AUX)
	@fail= failcom='exit 1'; \
	for f in x $$MAKEFLAGS; do \
	  case $$f in \
	    *=* | --[!k]*);; \
	    *k*) failcom='fail=yes';; \
	  esac; \
	done; \
	list='$(DIST_TEXLIBS)'; for texlibs in $$list; do \
	  if test ! -f $$texlibs/Makefile; then \
	    test -d $$texlibs || $(MKDIR_P) $$texlibs; \
	    cmd=`cat $(CONFIG_AUX) | sed "s,auxdir/auxsub,$$texlibs,g"`; \
	    case " $(TEXLIBS) " in \
	      *" $$texlibs "*) skip=;; \
	      *) skip=' --disable-build';; \
	    esac; \
	    (cd $$texlibs && echo "=== configuring in $$texlibs (`pwd`)" && \
	      echo "make: running $(SHELL) $$cmd$$skip" && \
	      CONFIG_SHELL=$(SHELL) && export CONFIG_SHELL && \
	      eval $(SHELL) $$cmd$$skip || \
	        { echo "=== configuring in $$texlibs failed"; exit 1; }) && \
	    if test "x$$skip" = x; then \
	      echo "Making all in $$texlibs" && \
	      (cd $$texlibs && $(MAKE) $(AM_MAKEFLAGS) all); \
	    fi || eval $$failcom; \
	  fi; \
	done; test -z "$$fail"

subsubdir-conf.cmd:
	@echo "configure in auxdir/auxsub failed to create the file $@"
	exit 1


# This is the default target for the Build script in TeX Live.
.PHONY: world
# Run texlinks here since the binaries won't be there
# at install-data, and fmtutil.cnf won't be there at install-exec,
# and there is no install-hook or install-local.
world: all
## Serialize 'make all' and 'make install-strip'.
	@echo "top-level make $@: running install-strip..."
	$(MAKE) $(AM_MAKEFLAGS) install-strip
##
	@echo "top-level make $@: running texlinks in texk/texlive/linked_scripts..."
	cd texk/texlive/linked_scripts && $(MAKE) $(AM_MAKEFLAGS) run-texlinks
##
	@echo "top-level make $@: running $(check_target)..."
	$(MAKE) $(AM_MAKEFLAGS) $(check_target)
##
	@echo "make $@ done."

# Allow for skipping make check if needed.
if cross
check_target = skip
else !cross
check_target = check
endif !cross
skip:
.PHONY: skip

# Special target to run TRIP and TRAP tests and create diffs.
.PHONY: triptrap
triptrap:
	cd texk && $(MAKE) $(AM_MAKEFLAGS) $@
