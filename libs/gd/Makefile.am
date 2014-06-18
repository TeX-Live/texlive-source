## Proxy Makefile.am to build libgd for TeX Live.
##
##   Copyright (C) 2009-2014 Peter Breitenlohner <tex-live@tug.org>
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
EXTRA_DIST += $(GD_TREE)-PATCHES

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn -o -name .deps -o -name .dirstamp -o -name '*.$(OBJEXT)'`
	rm -f $(distdir)/$(GD_TREE)/Makefile.in \
	  $(distdir)/$(GD_TREE)/aclocal.m4 \
	  $(distdir)/$(GD_TREE)/config/Makefile.in \
	  $(distdir)/$(GD_TREE)/config/ar-lib \
	  $(distdir)/$(GD_TREE)/config/compile \
	  $(distdir)/$(GD_TREE)/config/config.guess \
	  $(distdir)/$(GD_TREE)/config/config.rpath \
	  $(distdir)/$(GD_TREE)/config/config.sub \
	  $(distdir)/$(GD_TREE)/config/depcomp \
	  $(distdir)/$(GD_TREE)/config/install-sh \
	  $(distdir)/$(GD_TREE)/config/ltmain.sh \
	  $(distdir)/$(GD_TREE)/config/missing \
	  $(distdir)/$(GD_TREE)/configure \
	  $(distdir)/$(GD_TREE)/m4/libtool.m4 \
	  $(distdir)/$(GD_TREE)/m4/ltoptions.m4 \
	  $(distdir)/$(GD_TREE)/m4/ltsugar.m4 \
	  $(distdir)/$(GD_TREE)/m4/ltversion.m4 \
	  $(distdir)/$(GD_TREE)/m4/lt~obsolete.m4 \
	  $(distdir)/$(GD_TREE)/src/Makefile.in \
	  $(distdir)/$(GD_TREE)/src/depcomp \
	  $(distdir)/$(GD_TREE)/tests/Makefile.in

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
	@GD_TREE@/src/gd_bmp.c \
	@GD_TREE@/src/gd_color.c \
	@GD_TREE@/src/gd_color_map.c \
	@GD_TREE@/src/gd_color_match.c \
	@GD_TREE@/src/gd_crop.c \
	@GD_TREE@/src/gd_filter.c \
	@GD_TREE@/src/gd_gd.c \
	@GD_TREE@/src/gd_gd2.c \
	@GD_TREE@/src/gd_gif_in.c \
	@GD_TREE@/src/gd_gif_out.c \
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
	@GD_TREE@/src/wbmp.c \
	@GD_TREE@/src/webpimg.c

$(libgd_a_OBJECTS): config.force

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

