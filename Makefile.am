## Makefile.am for the TeX Live top-level
##
## Copyright (C) 2009, 2010 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
## Use 'aclocal --force' (for "computed m4_sinclude")
##
ACLOCAL_AMFLAGS = -I m4 --force

## speed up distcheck
DISTCHECK_CONFIGURE_FLAGS = -C
## check as well for symlinks
distuninstallcheck_listfiles = find . -type f -o -type l -print
distcleancheck_listfiles = find . -type f -o -type l -print

EXTRA_DIST = \
	Build \
	README.coding \
	README.config \
	README.cross \
	README.hacking \
	README.layout \
	build-aux/README.TL \
	m4/README \
	reautoconf

# just in case a subdirectory has added directories
dist-hook:
	rm -rf `find $(distdir) -name .svn`

if build_kpathsea
SUBDIRS = texk/kpathsea libs utils texk
else !build_kpathsea
SUBDIRS = libs utils texk
endif !build_kpathsea
DIST_SUBDIRS = auxdir/auxsub texk/kpathsea libs utils texk

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
	@echo "top-level make $@: running texlinks in texk/tetex..."
	cd texk/tetex && $(MAKE) $(AM_MAKEFLAGS) run-texlinks
##
	@echo "top-level make $@: running $(check_target)..."
	$(MAKE) $(AM_MAKEFLAGS) $(check_target)
##
	@echo "make $@ done."

# Allow for skipping make check if needed.
if cross_compiling
check_target = skip
else !cross_compiling
check_target = check
endif !cross_compiling
skip:
.PHONY: skip

# Special target to run TRIP and TRAP tests and create diffs.
.PHONY: triptrap
triptrap:
	cd texk && $(MAKE) $(AM_MAKEFLAGS) $@
