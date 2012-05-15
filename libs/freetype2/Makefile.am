## Proxy Makefile.am to build FreeType for TeX Live.
##
##   Copyright (C) 2009-2012 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
ACLOCAL_AMFLAGS = -I ../../m4

# Rebuild
.PHONY: rebuild
rebuild: all

## We want to re-distribute the whole original FreeType source tree.
##
## With current automake (1.10.2) 'make distcheck' fails when
## DISTFILES contains a directory and files in that directory.
EXTRA_DIST = $(FREETYPE_TREE)

## Changes applied to the original source tree
##
EXTRA_DIST += $(FREETYPE_TREE)-PATCHES

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn`

noinst_DATA = libfreetype.a

ft-build/Makefile:
	rm -rf ft-build
	$(MKDIR_P) ft-build
	cd ft-build && \
	  CC='$(CC)' CONFIG_SITE=/dev/null CONFIG_SHELL='$(SHELL)' \
	  $(SHELL) $(abs_srcdir)/$(FREETYPE_TREE)/configure \
	    $(config_args) --disable-shared \
	    --with-old-mac-fonts \
	    --without-bzip2 \
	    --without-zlib \
	    --prefix=$(abs_builddir)/ft-install \
	    --libdir=$(abs_builddir) \
	    --includedir=$(abs_builddir)

# On MacOSX, using our zlib from TL results in a .a that is unusable; ld
# warns "file is not of required architecture", and does not read it.
# There is no apparent reason for this.  Using --without-zlib evidently
# causes freetype2 to use its internal zlib, so no functionality should
# be lost, even though it is not the cleanest solution.
#	    --with-zlib $(ZLIB_FLAGS_FOR_FREETYPE) \
#
# Unfortunately the FreeType build system installs the headers after the library;
# to get our dependencies right we wait for a second and then touch the library.
libfreetype.a: ft-build/Makefile $(ZLIB_DEPEND)
	rm -rf ft-install
	cd ft-build \
	  && $(MAKE) $(AM_MAKEFLAGS) \
	  && $(MAKE) $(AM_MAKEFLAGS) install
	ft-install/bin/freetype-config --cflags >ft-includes
	ft-install/bin/freetype-config --libs \
	  | sed -e 's,-L\(.*\) -lfreetype,\1/libfreetype.la,' >ft-libs
	sleep 1
	@for f in libfreetype.*; do touch $$f; done

clean-local:
	rm -rf freetype2 ft2build.h ft-* libfreetype.* pkgconfig

