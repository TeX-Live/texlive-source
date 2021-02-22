## $Id$
## Wrapper Makefile.am to build FreeType for TeX Live.
##
##   Copyright 2016-2017 Karl Berry <tex-live@tug.org>
##   Copyright 2009-2015 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
## We want to re-distribute the whole original FreeType source tree.
##
EXTRA_DIST = $(FREETYPE_TREE)

## Changes applied to the original source tree
##
EXTRA_DIST += TLpatches

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am

noinst_DATA = libfreetype.a

# On MacOSX, using our zlib from TL results in a .a that is unusable; ld
# warns "file is not of required architecture", and does not read it.
# There is no apparent reason for this.  Using --without-zlib evidently
# causes freetype2 to use its internal zlib, so no functionality should
# be lost, even though it is not the cleanest solution. Similarly, don't
# incorporate other libraries which are not needed or are elsewhere in TL.
# They just complicate things.
ft-config:
	rm -rf ft-build
	$(MKDIR_P) ft-build
	cd ft-build && \
	  CC='$(CC)' CONFIG_SITE=/dev/null CONFIG_SHELL='$(SHELL)' \
	  $(SHELL) $(abs_srcdir)/$(FREETYPE_TREE)/configure \
	    $(config_args) --disable-shared \
	    --without-bzip2 \
	    --without-brotli \
	    --without-harfbuzz \
	    --without-png \
	    --without-zlib \
	    --prefix=$(abs_builddir)/ft-install \
	    --libdir=$(abs_builddir) \
	    --includedir=$(abs_builddir)
	echo timestamp >ft-config

# Unfortunately the FreeType build system installs the headers after the library;
# to get our dependencies right we wait for a second and then touch the library.
libfreetype.a: ft-config
	rm -rf ft-install
	cd ft-build \
	  && $(MAKE) $(AM_MAKEFLAGS) \
	  && $(MAKE) $(AM_MAKEFLAGS) install
	sleep 1
	@for f in libfreetype.*; do touch $$f; done

clean-local:
	rm -rf freetype2 ft2build.h ft-* libfreetype.* pkgconfig

# Rebuild
rebuild_prereq =
rebuild_target = all
CLEANFILES =

include $(srcdir)/../../am/rebuild.am

