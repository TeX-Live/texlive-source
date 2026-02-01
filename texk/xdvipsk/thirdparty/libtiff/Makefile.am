## Wrapper Makefile.am to build libtiff for TeX Live.
##
##   Copyright (C) 2009-2015 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
## We want to re-distribute the whole original libtiff source tree.
##
EXTRA_DIST = $(LIBTIFF_TREE)

## Changes applied to the original source tree
##
EXTRA_DIST += $(LIBTIFF_TREE)-PATCHES

# Files not to be distributed
include $(srcdir)/../../../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB)

SUBDIRS = . include 

LIBJPEG_INCLUDES = -I$(top_srcdir)/../libjpeg/libjpeg-src

ZLIB_INCLUDES = -I$(top_srcdir)/../../../../libs/zlib/zlib-src -I../../../../libs/zlib

AM_CPPFLAGS = -I$(top_srcdir)/$(LIBTIFF_TREE) $(LIBJPEG_INCLUDES) $(ZLIB_INCLUDES)\
              $(LIBTIFF_DEFINES) $(LIBTIFF_USER_CPPFLAGS) -D_LARGEFILE64_SOURCE
##AM_CFLAGS = $(VISIBILITY_CFLAGS) $(WARNING_CFLAGS) \
AM_CFLAGS = $(WARNING_CFLAGS) \
    -DLZW_SUPPORT -DPACKBITS_SUPPORT -DTHUNDER_SUPPORT -DNEXT_SUPPORT \
    -DCCITT_SUPPORT -DZIP_SUPPORT -DPIXARLOG_SUPPORT -DLOGLUV_SUPPORT -DJPEG_SUPPORT -D_LARGEFILE64_SOURCE
# following codecs require additional third party sources
#   -DJBIG_SUPPORT -DLZMA_SUPPORT

noinst_LIBRARIES = libtiff.a

libtiff_a_SOURCES = \
	@LIBTIFF_TREE@/tif_aux.c \
	@LIBTIFF_TREE@/tif_close.c \
	@LIBTIFF_TREE@/tif_codec.c \
	@LIBTIFF_TREE@/tif_color.c \
	@LIBTIFF_TREE@/tif_compress.c \
	@LIBTIFF_TREE@/tif_dir.c \
	@LIBTIFF_TREE@/tif_dirinfo.c \
	@LIBTIFF_TREE@/tif_dirread.c \
	@LIBTIFF_TREE@/tif_dirwrite.c \
	@LIBTIFF_TREE@/tif_dumpmode.c \
	@LIBTIFF_TREE@/tif_error.c \
	@LIBTIFF_TREE@/tif_extension.c \
	@LIBTIFF_TREE@/tif_fax3.c \
	@LIBTIFF_TREE@/tif_fax3sm.c \
	@LIBTIFF_TREE@/tif_flush.c \
	@LIBTIFF_TREE@/tif_getimage.c \
	@LIBTIFF_TREE@/tif_jbig.c \
	@LIBTIFF_TREE@/tif_jpeg.c \
	@LIBTIFF_TREE@/tif_jpeg_12.c \
	@LIBTIFF_TREE@/tif_luv.c \
	@LIBTIFF_TREE@/tif_lzma.c \
	@LIBTIFF_TREE@/tif_lzw.c \
	@LIBTIFF_TREE@/tif_next.c \
	@LIBTIFF_TREE@/tif_ojpeg.c \
	@LIBTIFF_TREE@/tif_open.c \
	@LIBTIFF_TREE@/tif_packbits.c \
	@LIBTIFF_TREE@/tif_pixarlog.c \
	@LIBTIFF_TREE@/tif_predict.c \
	@LIBTIFF_TREE@/tif_print.c \
	@LIBTIFF_TREE@/tif_read.c \
	@LIBTIFF_TREE@/tif_strip.c \
	@LIBTIFF_TREE@/tif_swab.c \
	@LIBTIFF_TREE@/tif_thunder.c \
	@LIBTIFF_TREE@/tif_tile.c \
	@LIBTIFF_TREE@/tif_version.c \
	@LIBTIFF_TREE@/tif_warning.c \
	@LIBTIFF_TREE@/tif_write.c \
	@LIBTIFF_TREE@/tif_zip.c \
	@LIBTIFF_TREE@/tif_hash_set.c

$(libtiff_a_OBJECTS): config.force

CLEANFILES = 

LDADD = libtiff.a

clean-local:
	rm -rf libtiff libtiff.* 

# Rebuild libtiff
@LIBJPEG_RULE@ 
## Rebuild libz
@ZLIB_RULE@

#Reconfig
reconfig_prereq = $(LIBJPEG_DEPEND) $(ZLIB_DEPEND)
DISTCLEANFILES = 

#Rebuild
rebuild_prereq =
rebuild_target = all

include $(srcdir)/../../../../am/rebuild.am
