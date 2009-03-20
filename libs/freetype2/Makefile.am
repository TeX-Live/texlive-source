## Proxy Makefile.am to build FreeType for TeX Live.
##
##   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
ACLOCAL_AMFLAGS = -I ../../m4

## We want to re-distribute the whole original FreeType source tree.
##
## With current automake (1.10.2) 'make distcheck' fails when
## DISTFILES contains a directory and files in that directory.
EXTRA_DIST = $(FREETYPE_TREE)

if build

noinst_DATA = libfreetype.a

endif build

ft-build/Makefile:
	rm -rf ft-build
	$(mkdir_p) ft-build
	cd ft-build && \
	  $(abs_srcdir)/$(FREETYPE_TREE)/configure \
	    $(config_args) --disable-shared \
	    --with-old-mac-fonts \
	    --with-zlib $(ZLIB_FLAGS_FOR_FREETYPE) \
	    --prefix=$(abs_builddir)/ft-install \
	    --includedir=$(abs_builddir)

## Unfortunately the FreeType build system installs the headers after the library;
## get our dependencies right we wait for a second and then touch the library.
libfreetype.a: ft-build/Makefile $(ZLIB_DEPEND)
	rm -rf ft-install
	cd ft-build \
	  && $(MAKE) $(AM_MAKEFLAGS) \
	  && $(MAKE) $(AM_MAKEFLAGS) install
	mv ft-install/lib/libfreetype.a .
	sleep 1
	touch libfreetype.a
	rm -rf ft-install

## Rebuild zlib
@ZLIB_RULE@

clean-local:
	rm -rf libfreetype.a ft2build.h freetype2 ft-build ft-install

