## $Id$
## Proxy Makefile.am to build libgd for TeX Live.
##
##   Copyright 2017 Karl Berry <tex-live@tug.org>
##   Copyright 2009-2015 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
## We want to re-distribute the whole original libgd source tree.
##
EXTRA_DIST = $(GD_TREE)

## Changes applied to the original source tree
##
EXTRA_DIST += TLpatches

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB)

SUBDIRS = . include

AM_CPPFLAGS = -I$(top_srcdir)/$(GD_TREE)/src
if WIN32
AM_CPPFLAGS += -DNONDLL -DBGDWIN32
endif WIN32
AM_CPPFLAGS += $(ZLIB_INCLUDES) $(LIBPNG_INCLUDES) $(FREETYPE2_INCLUDES)
AM_CFLAGS = # $(WARNING_CFLAGS)

noinst_LIBRARIES=libgd.a

libgd_a_SOURCES = \
	@GD_TREE@/src/gd.c \
	@GD_TREE@/src/gd_avif.c \
	@GD_TREE@/src/gd_bmp.c \
	@GD_TREE@/src/gd_color.c \
	@GD_TREE@/src/gd_color_map.c \
	@GD_TREE@/src/gd_color_match.c \
	@GD_TREE@/src/gd_crop.c \
	@GD_TREE@/src/gd_filename.c \
	@GD_TREE@/src/gd_filter.c \
	@GD_TREE@/src/gd_gd.c \
	@GD_TREE@/src/gd_gd2.c \
	@GD_TREE@/src/gd_gif_in.c \
	@GD_TREE@/src/gd_gif_out.c \
	@GD_TREE@/src/gd_heif.c \
	@GD_TREE@/src/gd_interpolation.c \
	@GD_TREE@/src/gd_io.c \
	@GD_TREE@/src/gd_io_dp.c \
	@GD_TREE@/src/gd_io_file.c \
	@GD_TREE@/src/gd_io_ss.c \
	@GD_TREE@/src/gd_jpeg.c \
	@GD_TREE@/src/gd_matrix.c \
	@GD_TREE@/src/gd_nnquant.c \
	@GD_TREE@/src/gd_png.c \
	@GD_TREE@/src/gd_rotate.c \
	@GD_TREE@/src/gd_security.c \
	@GD_TREE@/src/gd_ss.c \
	@GD_TREE@/src/gd_tga.c \
	@GD_TREE@/src/gd_tiff.c \
	@GD_TREE@/src/gd_topal.c \
	@GD_TREE@/src/gd_transform.c \
	@GD_TREE@/src/gd_version.c \
	@GD_TREE@/src/gd_wbmp.c \
	@GD_TREE@/src/gd_webp.c \
	@GD_TREE@/src/gd_xbm.c \
	@GD_TREE@/src/gdcache.c \
	@GD_TREE@/src/gdfontg.c \
	@GD_TREE@/src/gdfontl.c \
	@GD_TREE@/src/gdfontmb.c \
	@GD_TREE@/src/gdfonts.c \
	@GD_TREE@/src/gdfontt.c \
	@GD_TREE@/src/gdft.c \
	@GD_TREE@/src/gdfx.c \
	@GD_TREE@/src/gdhelpers.c \
	@GD_TREE@/src/gdkanji.c \
	@GD_TREE@/src/gdtables.c \
	@GD_TREE@/src/gdxpm.c \
	@GD_TREE@/src/wbmp.c

$(libgd_a_OBJECTS): config.force

if build
check_PROGRAMS = gdtest
dist_check_SCRIPTS = libgd.test
TESTS = libgd.test
endif build
libgd.log: gdtest$(EXEEXT)

gdtest_SOURCES = gdtest.c

gdtest_CPPFLAGS = -Iinclude -DBGDWIN32 -DNONDLL

LDADD = libgd.a

## Rebuild libpng
@LIBPNG_RULE@
## Rebuild libfreetype
@FREETYPE2_RULE@

# Reconfig
reconfig_prereq = $(LIBPNG_DEPEND) $(FREETYPE2_DEPEND)
DISTCLEANFILES =

include $(srcdir)/../../am/reconfig.am

# Rebuild
rebuild_prereq =
rebuild_target = all
CLEANFILES =

include $(srcdir)/../../am/rebuild.am

