## Proxy Makefile.am to build libpng for TeX Live.
##
##   Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
ACLOCAL_AMFLAGS = -I ../../m4

# Rebuild
.PHONY: rebuild
rebuild: all

## We want to re-distribute the whole original libpng source tree.
##
EXTRA_DIST = $(GD_TREE)

## Changes applied to the original source tree
##
EXTRA_DIST += $(GD_TREE)-PATCHES

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn -o -name .deps -o -name .dirstamp -o -name '*.$(OBJEXT)'`
	rm -f $(distdir)/$(GD_TREE)/config/config.guess \
	  $(distdir)/$(GD_TREE)/config/config.rpath \
	  $(distdir)/$(GD_TREE)/config/config.sub \
	  $(distdir)/$(GD_TREE)/config/depcomp \
	  $(distdir)/$(GD_TREE)/config/install-sh \
	  $(distdir)/$(GD_TREE)/config/ltmain.sh \
	  $(distdir)/$(GD_TREE)/config/missing \
	  $(distdir)/$(GD_TREE)/config/mkinstalldirs \
	  $(distdir)/$(GD_TREE)/depcomp \
	  $(distdir)/$(GD_TREE)/install-sh \
	  $(distdir)/$(GD_TREE)/missing \
	  $(distdir)/$(GD_TREE)/mkinstalldirs

SUBDIRS = include

AM_CPPFLAGS = -I$(top_srcdir)/$(GD_TREE) $(ZLIB_INCLUDES) $(LIBPNG_INCLUDES) $(FREETYPE2_INCLUDES)
AM_CPPFLAGS += -DNONDLL

noinst_LIBRARIES=libgd.a

libgd_a_SOURCES = \
	@GD_TREE@/gd.c \
	@GD_TREE@/gd_gd.c \
	@GD_TREE@/gd_gd2.c \
	@GD_TREE@/gd_gif_in.c \
	@GD_TREE@/gd_gif_out.c \
	@GD_TREE@/gd_io.c \
	@GD_TREE@/gd_io_dp.c \
	@GD_TREE@/gd_io_file.c \
	@GD_TREE@/gd_io_ss.c \
	@GD_TREE@/gd_jpeg.c \
	@GD_TREE@/gd_png.c \
	@GD_TREE@/gd_security.c \
	@GD_TREE@/gd_ss.c \
	@GD_TREE@/gd_topal.c \
	@GD_TREE@/gd_wbmp.c \
	@GD_TREE@/gdcache.c \
	@GD_TREE@/gdfontg.c \
	@GD_TREE@/gdfontl.c \
	@GD_TREE@/gdfontmb.c \
	@GD_TREE@/gdfonts.c \
	@GD_TREE@/gdfontt.c \
	@GD_TREE@/gdft.c \
	@GD_TREE@/gdfx.c \
	@GD_TREE@/gdhelpers.c \
	@GD_TREE@/gdkanji.c \
	@GD_TREE@/gdtables.c \
	@GD_TREE@/gdxpm.c \
	@GD_TREE@/wbmp.c

$(libgd_a_OBJECTS): config.force

config.force: $(LIBPNG_DEPEND) $(FREETYPE2_DEPEND)
	echo timestamp >config.force
	$(SHELL) ./config.status --recheck

DISTCLEANFILES = config.force

## Rebuild libpng
@LIBPNG_RULE@
## Rebuild libfreetype
@FREETYPE2_RULE@

