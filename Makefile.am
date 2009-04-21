## Makefile.am for the TeX Live top-level
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I m4

## speed up distcheck
DISTCHECK_CONFIGURE_FLAGS = -C

EXTRA_DIST = \
	Build \
	README.cross \
	README.layout \
	build-aux/README.TL \
	m4/README

## just in case a subdirectory has added directories
dist-hook:
	rm -rf `find $(distdir) -name .svn`

if build_kpathsea
SUBDIRS = texk/kpathsea
else !build_kpathsea
SUBDIRS =
endif !build_kpathsea
SUBDIRS += libs utils texk

## This is the default target for the Build script in TeX Live.
world: install-strip
##
## run texlinks here since the binaries won't be there 
## at install-data, and fmtutil.cnf won't be there at install-exec,
## and there is no hook or -local for install.
	echo "top-level make $@: running texlinks ..."
	cd texk/tetex && $(MAKE) $(AM_MAKEFLAGS) run-texlinks
##
	@echo "make $@ done."
