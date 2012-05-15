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

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn`

noinst_DATA = libttf.a

ft-build/Makefile:
	rm -rf ft-build
	$(MKDIR_P) ft-build
	cd ft-build && \
	  CC='$(CC)' CONFIG_SITE=/dev/null CONFIG_SHELL='$(SHELL)' \
	  $(SHELL) $(abs_srcdir)/$(FREETYPE_TREE)/configure \
	    $(config_args) --disable-shared --disable-nls \
	    --prefix=$(abs_builddir)/ft-install \
	    --libdir=$(abs_builddir) \
	    --includedir=$(abs_builddir)

# Unfortunately the FreeType build system installs the headers after the library;
# to get our dependencies right we wait for a second and then touch the library.
libttf.a: ft-build/Makefile
	rm -rf ft-install
	cd ft-build/lib \
	  && $(MAKE) $(AM_MAKEFLAGS) -f arch/unix/Makefile all \
	  && $(MAKE) $(AM_MAKEFLAGS) -f arch/unix/Makefile install
	sleep 1
	touch libttf.a

clean-local:
	rm -rf libttf.a freetype ft-build ft-install

