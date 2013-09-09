## Makefile.am for the TeX Live top-level
##
## Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
## Use 'aclocal --force' (for "computed m4_sinclude")
##
ACLOCAL_AMFLAGS = -I m4 --force

## The info files
info_TEXINFOS = doc/texlive.texi
doc_texlive_TEXINFOS = doc/build.texi doc/build-tools

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

SUBDIRS = . $(MAKE_SUBDIRS) libs utils texk
DIST_SUBDIRS = auxdir/auxsub $(CONF_SUBDIRS) libs utils texk

cf_silent = $(cf_silent_@AM_V@)
cf_silent_ = $(cf_silent_@AM_DEFAULT_V@)
cf_silent_0 = --silent
cf_silent_1 =

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
