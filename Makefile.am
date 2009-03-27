##
ACLOCAL_AMFLAGS = -I m4

## speed up distcheck
DISTCHECK_CONFIGURE_FLAGS = -C

EXTRA_DIST = \
	Build \
	README.cross \
	README.layout \
	README.orig \
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
	@echo "make $@ done."

