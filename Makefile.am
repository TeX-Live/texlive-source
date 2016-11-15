## $Id$
## Makefile.am for the TeX Live top-level
##
## Copyright 2016      Karl Berry <tex-live@tug.org>
## Copyright 2009-2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
## Ensure aclocal.m4 is up to date
ACLOCAL = @ACLOCAL@ --force

## speed up distcheck
AM_DISTCHECK_CONFIGURE_FLAGS = -C
## check as well for symlinks
distuninstallcheck_listfiles = find . -type f -o -type l -print
distcleancheck_listfiles = find . -type f -o -type l -print

EXTRA_DIST = \
	Build \
	README.0overview \
	README.1prerequisites \
	README.2building \
	README.3installing \
	README.4layout \
	README.5configure \
	README.6cross \
	README.7coding \
	build-aux/README.TL \
	m4/ChangeLog \
	m4/README \
	reautoconf

# Files not to be distributed
include $(srcdir)/am/dist_hook.am

SUBDIRS = . doc $(MAKE_SUBDIRS) libs utils texk
DIST_SUBDIRS = auxdir/auxsub doc $(CONF_SUBDIRS) libs utils texk

## Configure and build subdirs.
##
recurse_this =
recurse_top =

include $(srcdir)/am/recurse.am

# This is the default target for the Build script in TeX Live.
.PHONY: world
# Run texlinks here since the binaries won't be there
# at install-data, and fmtutil.cnf won't be there at install-exec,
# and there is no install-hook or install-local.
texlinks_dir = texk/texlive/tl_scripts
world: all
## Serialize 'make all' and 'make install-strip'.
	@echo "top-level make $@: running install-strip..."
	$(MAKE) $(AM_MAKEFLAGS) install-strip
##
	@echo "top-level make $@: running texlinks in $(texlinks_dir) ..."
	cd $(texlinks_dir) && $(MAKE) $(AM_MAKEFLAGS) run-texlinks
##
	@echo "top-level make $@: running $(check_target)..."
	$(MAKE) $(AM_MAKEFLAGS) $(check_target)
##
	@echo "top-level make $@ done."

install-exec-hook:
	@echo "WARNING: make install does not result in a usable TeX!"
	@echo "  ! The binaries require the myriad support files."
	@echo "  ! See http://tug.org/texlive/distro.html"
	@echo "  ! and http://tug.org/texlive/build.html#deploy, for example."

# Allow for skipping make check if needed.
if cross
check_target = skip
else !cross
check_target = check
endif !cross
skip:
.PHONY: skip

# Directly run texlinks.
.PHONY: texlinks
texlinks:
	cd $(texlinks_dir) && $(MAKE) $(AM_MAKEFLAGS) run-texlinks

# Special target to run TRIP and TRAP tests and create diffs.
.PHONY: triptrap
triptrap:
	cd texk && $(MAKE) $(AM_MAKEFLAGS) $@
