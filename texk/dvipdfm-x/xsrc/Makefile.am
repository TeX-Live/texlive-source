## Makefile.am for the TeX Live subdirectory texk/dvipdfm-x/xsrc/
##
## Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
AM_CPPFLAGS = -DXETEX $(KPATHSEA_INCLUDES) $(FREETYPE2_INCLUDES) $(LIBPNG_INCLUDES) $(ZLIB_INCLUDES)
AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = xdvipdfmx

xdvipdfmx_SOURCES = \
	agl.c bmpimage.c cff.c cff_dict.c cid.c cidtype0.c cidtype2.c \
	cmap.c cmap_read.c cmap_write.c cs_type2.c \
	dpxconf.c dpxcrypt.c dpxfile.c dpxutil.c \
	dvi.c dvipdfmx.c epdf.c error.c fontmap.c jpegimage.c mem.c mfileio.c mpost.c \
	numbers.c otl_conf.c otl_opt.c pdfcolor.c pdfdev.c pdfdoc.c pdfdraw.c pdfencoding.c \
	pdfencrypt.c pdffont.c pdfnames.c pdfobj.c pdfparse.c pdfresource.c pdfximage.c \
	pkfont.c pngimage.c pst.c pst_obj.c sfnt.c \
	spc_color.c spc_dvips.c spc_html.c spc_misc.c spc_pdfm.c spc_tpic.c spc_util.c spc_xtx.c specials.c \
	subfont.c tfm.c \
	tt_aux.c tt_cmap.c tt_glyf.c tt_gsub.c tt_post.c tt_table.c truetype.c \
	type0.c t1_char.c t1_load.c type1.c type1c.c \
	unicode.c vf.c xbb.c \
	agl.h bmpimage.h cff.h cff_dict.h cff_limits.h cff_stdstr.h cff_types.h \
	cid.h cid_basefont.h cid_p.h cidtype0.h cidtype2.h cmap.h cmap_p.h cmap_read.h cmap_write.h \
	cs_type2.h dpxconf.h dpxcrypt.h dpxutil.h dpxfile.h dvi.h dvicodes.h epdf.h error.h fontmap.h \
	jpegimage.h mem.h mfileio.h mpost.h numbers.h otl_conf.h otl_opt.h pdfcolor.h pdfdev.h \
	pdfdoc.h pdfdraw.h pdfencoding.h pdfencrypt.h pdffont.h pdflimits.h pdfnames.h pdfobj.h \
	pdfparse.h pdfresource.h pdfximage.h pkfont.h pngimage.h pst.h pst_obj.h sfnt.h \
	spc_color.h spc_dvips.h spc_html.h spc_misc.h spc_pdfm.h spc_tpic.h spc_util.h spc_xtx.h specials.h \
	subfont.h system.h tfm.h \
	tt_aux.h tt_cmap.h tt_glyf.h tt_gsub.h tt_post.h tt_table.h truetype.h \
	type0.h t1_char.h t1_load.h type1.h type1c.h \
	unicode.h vf.h xbb.h

xdvipdfmx_DEPENDENCIES = $(ZLIB_DEPEND) $(LIBPNG_DEPEND) $(FREETYPE2_DEPEND) $(KPATHSEA_DEPEND)

LDADD = $(KPATHSEA_LIBS) $(FREETYPE2_LIBS) $(LIBPNG_LIBS) $(ZLIB_LIBS)

## Rebuild libkpathsea
@KPATHSEA_RULE@
## Rebuild libfreetype
@FREETYPE2_RULE@
## Rebuild libpng
@LIBPNG_RULE@
## Rebuild libz
@ZLIB_RULE@

## Tests
TESTS = xdvipdfmx.test
EXTRA_DIST = $(TESTS)

