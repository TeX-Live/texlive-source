## $Id$
### Makefile.am for the TeX Live subdirectory texk/xdvipsk/
##
## Copyright 2015-2026 Karl Berry <tex-live@tug.org>
## Copyright 2009-2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
GRAPHLIB_INCLUDES = -I$(srcdir)/graphlib
OTFLIB_INCLUDES = -I$(srcdir)/otflib

## liblua53
##
LUA_INCLUDES = $(LUA53_INCLUDES) $(LUA53_DEFINES)
LUA_LIBS = $(LUA53_LIBS)
LUA_DEPEND = $(LUA53_DEPEND)

AM_CPPFLAGS = $(KPATHSEA_INCLUDES) -DUNIX -DKPATHSEA -DSHIFTLOWCHARS
AM_CFLAGS = $(WARNING_CFLAGS)

if cross
  MINGW_INCLUDES = -I$(abs_top_srcdir)/mingw/include
endif
bin_PROGRAMS = xdvipsk

xdvipsk_CPPFLAGS = $(OTFLIB_INCLUDES) $(GRAPHLIB_INCLUDES) $(LUA_INCLUDES) $(ZLIB_INCLUDES) $(LIBPNG_INCLUDES) $(LIBJPEG_INCLUDES) $(LIBTIFF_INCLUDES) $(KPATHSEA_INCLUDES) $(FREETYPE2_INCLUDES) -DUNIX -DKPATHSEA -DSHIFTLOWCHARS -DXDVIPSK

xdvipsk_SOURCES = \
	bbox.c \
	bitmapenc.c \
	charcode.c \
	color.c \
	config.h \
	debug.h \
	dopage.c \
	dosection.c \
	dospecial.c \
	download.c \
	dpicheck.c \
	drawPS.c \
	dviinput.c \
	emspecialex.c \
	finclude.c \
	fontdef.c \
	glyphlist_table.h \
	glyphmap.c \
	glyphmap.h \
	header.c \
	hps.c \
	loadfont.c \
	luamap.c \
	makefont.c \
	output.c \
	papersiz.c \
	paths.h \
	pprescan.c \
	prescan.c \
	protos.h \
	protos_add.h \
	ptexmac.h \
	repack.c \
	resident.c \
	scalewidth.c \
	scanpage.c \
	search.c \
	sfntload.c \
	skippage.c \
	t1part.c \
	t1part.h \
	tfmload.c \
	unpack.c \
	utarray.h \
	uthash.h \
	virtualfont.c \
	writecid.c \
	writet1.c \
	xdvips.c \
	xdvips.h \
	xdvipsk_tounicode.h \
	otflib/cff.c \
	otflib/cff.h \
	otflib/cff_dict.c \
	otflib/cff_dict.h \
	otflib/cff_limits.h \
	otflib/cff_stdstr.h \
	otflib/cff_types.h \
	otflib/cidsysinfo.c \
	otflib/cidsysinfo.h \
	otflib/cidtype0.c \
	otflib/cidtype0.h \
	otflib/cidtype2.c \
	otflib/cidtype2.h \
	otflib/cmap.c \
	otflib/cmap.h \
	otflib/cmap_p.h \
	otflib/cmap_write.c \
	otflib/cmap_write.h \
	otflib/cs_type2.c \
	otflib/cs_type2.h \
	otflib/dpxfile.c \
	otflib/dpxfile.h \
	otflib/dpxutil.c \
	otflib/dpxutil.h \
	otflib/error.c \
	otflib/error.h \
	otflib/mem.c \
	otflib/mem.h \
	otflib/mfileio.c \
	otflib/mfileio.h \
	otflib/numbers.c \
	otflib/numbers.h \
	otflib/sfnt.c \
	otflib/sfnt.h \
	otflib/system.h \
	otflib/tt_aux.c \
	otflib/tt_aux.h \
	otflib/tt_cmap.c \
	otflib/tt_cmap.h \
	otflib/tt_glyf.c \
	otflib/tt_glyf.h \
	otflib/tt_post.c \
	otflib/tt_post.h \
	otflib/tt_table.c \
	otflib/tt_table.h \
	otflib/unicode.c \
	otflib/unicode.h

LIBJPEG_TREE = thirdparty/libjpeg/libjpeg-src
LIBJPEG_INCLUDES = -I$(abs_top_srcdir)/$(LIBJPEG_TREE)
libjpeg_a_CPPFLAGS = $(LIBJPEG_INCLUDES)
MEMORYMGR = jmemnobs

libjpeg_a_SOURCES = \
	$(LIBJPEG_TREE)/jaricom.c \
	$(LIBJPEG_TREE)/jcapimin.c \
	$(LIBJPEG_TREE)/jcapistd.c \
	$(LIBJPEG_TREE)/jcarith.c \
	$(LIBJPEG_TREE)/jccoefct.c \
	$(LIBJPEG_TREE)/jccolor.c \
	$(LIBJPEG_TREE)/jcdctmgr.c \
	$(LIBJPEG_TREE)/jchuff.c \
	$(LIBJPEG_TREE)/jcinit.c \
	$(LIBJPEG_TREE)/jcmainct.c \
	$(LIBJPEG_TREE)/jcmarker.c \
	$(LIBJPEG_TREE)/jcmaster.c \
	$(LIBJPEG_TREE)/jcomapi.c \
	$(LIBJPEG_TREE)/jcparam.c \
	$(LIBJPEG_TREE)/jcprepct.c \
	$(LIBJPEG_TREE)/jcsample.c \
	$(LIBJPEG_TREE)/jctrans.c \
	$(LIBJPEG_TREE)/jdapimin.c \
	$(LIBJPEG_TREE)/jdapistd.c \
	$(LIBJPEG_TREE)/jdarith.c \
	$(LIBJPEG_TREE)/jdatadst.c \
	$(LIBJPEG_TREE)/jdatasrc.c \
	$(LIBJPEG_TREE)/jdcoefct.c \
	$(LIBJPEG_TREE)/jdcolor.c \
	$(LIBJPEG_TREE)/jddctmgr.c \
	$(LIBJPEG_TREE)/jdhuff.c \
	$(LIBJPEG_TREE)/jdinput.c \
	$(LIBJPEG_TREE)/jdmainct.c \
	$(LIBJPEG_TREE)/jdmarker.c \
	$(LIBJPEG_TREE)/jdmaster.c \
	$(LIBJPEG_TREE)/jdmerge.c \
	$(LIBJPEG_TREE)/jdpostct.c \
	$(LIBJPEG_TREE)/jdsample.c \
	$(LIBJPEG_TREE)/jdtrans.c \
	$(LIBJPEG_TREE)/jerror.c \
	$(LIBJPEG_TREE)/jfdctflt.c \
	$(LIBJPEG_TREE)/jfdctfst.c \
	$(LIBJPEG_TREE)/jfdctint.c \
	$(LIBJPEG_TREE)/jidctflt.c \
	$(LIBJPEG_TREE)/jidctfst.c \
	$(LIBJPEG_TREE)/jidctint.c \
	$(LIBJPEG_TREE)/jquant1.c \
	$(LIBJPEG_TREE)/jquant2.c \
	$(LIBJPEG_TREE)/jutils.c \
	$(LIBJPEG_TREE)/jmemmgr.c \
	$(LIBJPEG_TREE)/$(MEMORYMGR).c

LIBJPEG_LIBS = libjpeg.a

$(libjpeg_a_OBJECTS): config.force

LIBTIFF_TREE = thirdparty/libtiff/libtiff-src
LIBTIFF_INCLUDES = -I$(abs_top_srcdir)/$(LIBTIFF_TREE)
libtiff_a_CPPFLAGS = $(LIBJPEG_INCLUDES) $(LIBTIFF_INCLUDES) $(ZLIB_INCLUDES)

libtiff_a_SOURCES = \
	$(LIBTIFF_TREE)/tif_aux.c \
	$(LIBTIFF_TREE)/tif_close.c \
	$(LIBTIFF_TREE)/tif_codec.c \
	$(LIBTIFF_TREE)/tif_color.c \
	$(LIBTIFF_TREE)/tif_compress.c \
	$(LIBTIFF_TREE)/tif_dir.c \
	$(LIBTIFF_TREE)/tif_dirinfo.c \
	$(LIBTIFF_TREE)/tif_dirread.c \
	$(LIBTIFF_TREE)/tif_dirwrite.c \
	$(LIBTIFF_TREE)/tif_dumpmode.c \
	$(LIBTIFF_TREE)/tif_error.c \
	$(LIBTIFF_TREE)/tif_extension.c \
	$(LIBTIFF_TREE)/tif_fax3.c \
	$(LIBTIFF_TREE)/tif_fax3sm.c \
	$(LIBTIFF_TREE)/tif_flush.c \
	$(LIBTIFF_TREE)/tif_getimage.c \
	$(LIBTIFF_TREE)/tif_jbig.c \
	$(LIBTIFF_TREE)/tif_jpeg.c \
	$(LIBTIFF_TREE)/tif_jpeg_12.c \
	$(LIBTIFF_TREE)/tif_luv.c \
	$(LIBTIFF_TREE)/tif_lzma.c \
	$(LIBTIFF_TREE)/tif_lzw.c \
	$(LIBTIFF_TREE)/tif_next.c \
	$(LIBTIFF_TREE)/tif_ojpeg.c \
	$(LIBTIFF_TREE)/tif_open.c \
	$(LIBTIFF_TREE)/tif_packbits.c \
	$(LIBTIFF_TREE)/tif_pixarlog.c \
	$(LIBTIFF_TREE)/tif_predict.c \
	$(LIBTIFF_TREE)/tif_print.c \
	$(LIBTIFF_TREE)/tif_read.c \
	$(LIBTIFF_TREE)/tif_strip.c \
	$(LIBTIFF_TREE)/tif_swab.c \
	$(LIBTIFF_TREE)/tif_thunder.c \
	$(LIBTIFF_TREE)/tif_tile.c \
	$(LIBTIFF_TREE)/tif_version.c \
	$(LIBTIFF_TREE)/tif_warning.c \
	$(LIBTIFF_TREE)/tif_write.c \
	$(LIBTIFF_TREE)/tif_zip.c \
	$(LIBTIFF_TREE)/tif_hash_set.c

LIBTIFF_LIBS = libtiff.a

$(libtiff_a_OBJECTS): config.force

$(xdvipsk_OBJECTS): $(KPATHSEA_DEPEND) $(LUA_DEPEND) $(ZLIB_DEPEND) $(LIBPNG_DEPEND) $(LIBJPEG_DEPEND) $(LIBTIFF_DEPEND) $(FREETYPE2_DEPEND)
xdvipsk_LDADD = libdvipsgraph.a $(KPATHSEA_LIBS) $(LUA_LIBS) $(LIBTIFF_LIBS) $(LIBPNG_LIBS) $(ZLIB_LIBS) $(LIBJPEG_LIBS) $(FREETYPE2_LIBS) -lstdc++
if cross
   xdvipsk_SOURCES += mingw/include/wingdi.h
   xdvipsk_LDFLAGS = -fno-lto -fno-use-linker-plugin -static -static-libgcc -static-libstdc++ --disable-shared
   xdvipsk_LDADD += -lgdi32
endif

noinst_LIBRARIES = libdvipsgraph.a $(LIBTIFF_LIBS) $(LIBJPEG_LIBS)

libdvipsgraph_a_dependencies = $(ZLIB_DEPEND) $(LIBPNG_DEPEND) $(LIBJPEG_DEPEND) $(LIBTIFF_DEPEND)
libdvipsgraph_a_SOURCES = \
    graphlib/bitmap.cpp \
    graphlib/bitmap.h \
    graphlib/bitmapAccess.cpp \
    graphlib/bitmapIO.cpp \
    graphlib/bitmapIO.h \
    graphlib/colors.cpp \
    graphlib/conversion.cpp \
    graphlib/conversion24.cpp \
    graphlib/conversion32.cpp \
    graphlib/conversion8.cpp \
    graphlib/filters.h \
    graphlib/loadBMP.cpp \
    graphlib/loadJPEG.cpp \
    graphlib/loadPCX.cpp \
    graphlib/loadPNG.cpp \
    graphlib/loadTIFF.cpp \
    graphlib/loaders.h \
    graphlib/resample.cpp \
    graphlib/rescale.cpp \
    graphlib/resize.cpp \
    graphlib/resize.h \
    graphlib/tiffLogLuv.cpp \
    graphlib/toFloat.cpp \
    graphlib/toFloat.h \
    graphlib/utilities.h

$(libdvipsgraph_a_OBJECTS): config.force

AM_CXXFLAGS = @AM_CXXFLAGS@

libdvipsgraph_a_CPPFLAGS = $(ZLIB_INCLUDES) $(LIBPNG_INCLUDES) $(LIBJPEG_INCLUDES) $(LIBTIFF_INCLUDES)

###############################################################################
# THE LIBRARIES TO BUILD
###############################################################################

## Rebuild libkpathsea
@KPATHSEA_RULE@
## Rebuild liblua53
@LUA53_RULE@
## Rebuild libz
@ZLIB_RULE@
## Rebuild libpng
@LIBPNG_RULE@
## Rebuild freetype2
@FREETYPE2_RULE@

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
