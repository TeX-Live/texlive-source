## Makefile.am for the TeX Live subdirectory texk/dvipdfm-x/
##
## Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

bin_PROGRAMS =
if DVIPDFMX
bin_PROGRAMS += dvipdfmx
if !WIN32
dist_bin_SCRIPTS = dvipdft
endif !WIN32
endif DVIPDFMX
if XDVIPDFMX
bin_PROGRAMS += xdvipdfmx
endif XDVIPDFMX
EXTRA_PROGRAMS = dvipdfmx xdvipdfmx

AM_CPPFLAGS = $(KPATHSEA_INCLUDES) $(LIBPNG_INCLUDES) $(ZLIB_INCLUDES)
dvipdfmx_CPPFLAGS = -I$(srcdir)/src $(AM_CPPFLAGS)
xdvipdfmx_CPPFLAGS = -DXETEX -I$(srcdir)/xsrc $(FREETYPE2_INCLUDES) $(AM_CPPFLAGS)
AM_CFLAGS = $(WARNING_CFLAGS)

nodist_dvipdfmx_SOURCES = $(common_sources) $(dvipdfmx_sources)
nodist_xdvipdfmx_SOURCES = $(common_sources) $(xdvipdfmx_sources)

dvipdfmx_LDADD = $(KPATHSEA_LIBS) $(LIBPNG_LIBS) $(ZLIB_LIBS)
xdvipdfmx_LDADD = $(FREETYPE2_LIBS) $(dvipdfmx_LDADD)

$(dvipdfmx_OBJECTS) $(xdvipdfmx_OBJECTS): config.force

common_sources = \
	agl.c \
	agl.h \
	bmpimage.c \
	bmpimage.h \
	cff.c \
	cff.h \
	cff_dict.h \
	cff_limits.h \
	cff_stdstr.h \
	cff_types.h \
	cid.c \
	cid.h \
	cid_basefont.h \
	cid_p.h \
	cidtype0.h \
	cidtype2.c \
	cidtype2.h \
	cmap.c \
	cmap.h \
	cmap_p.h \
	cmap_read.c \
	cmap_read.h \
	cmap_write.c \
	cmap_write.h \
	cs_type2.c \
	cs_type2.h \
	dpxconf.c \
	dpxconf.h \
	dpxcrypt.c \
	dpxcrypt.h \
	dpxfile.c \
	dpxfile.h \
	dpxutil.c \
	dpxutil.h \
	dvi.h \
	dvicodes.h \
	epdf.h \
	error.c \
	error.h \
	fontmap.c \
	fontmap.h \
	jpegimage.h \
	mem.c \
	mem.h \
	mfileio.c \
	mfileio.h \
	mpost.c \
	mpost.h \
	numbers.c \
	numbers.h \
	otl_conf.c \
	otl_conf.h \
	otl_opt.c \
	otl_opt.h \
	pdfcolor.h \
	pdfdev.h \
	pdfdoc.h \
	pdfdraw.h \
	pdfencoding.c \
	pdfencoding.h \
	pdfencrypt.h \
	pdffont.c \
	pdffont.h \
	pdflimits.h \
	pdfobj.h \
	pdfparse.c \
	pdfparse.h \
	pdfresource.c \
	pdfresource.h \
	pdfximage.h \
	pkfont.c \
	pkfont.h \
	pngimage.c \
	pngimage.h \
	pst.c \
	pst.h \
	pst_obj.c \
	pst_obj.h \
	sfnt.c \
	sfnt.h \
	spc_color.c \
	spc_color.h \
	spc_dvips.h \
	spc_html.c \
	spc_html.h \
	spc_misc.c \
	spc_misc.h \
	spc_pdfm.h \
	spc_tpic.h \
	spc_util.c \
	spc_util.h \
	specials.h \
	subfont.c \
	subfont.h \
	system.h \
	t1_char.c \
	t1_char.h \
	t1_load.c \
	t1_load.h \
	tfm.h \
	truetype.h \
	tt_aux.c \
	tt_aux.h \
	tt_cmap.h \
	tt_glyf.c \
	tt_glyf.h \
	tt_gsub.c \
	tt_gsub.h \
	tt_post.c \
	tt_post.h \
	tt_table.c \
	tt_table.h \
	type0.c \
	type0.h \
	type1.h \
	type1c.h \
	unicode.c \
	unicode.h \
	vf.c \
	vf.h

dvipdfmx_sources = \
	src/cff_dict.c \
	src/cidtype0.c \
	src/dvi.c \
	src/dvipdfmx.c \
	dvipdfmx.h \
	src/epdf.c \
	src/jpegimage.c \
	src/pdfcolor.c \
	src/pdfdev.c \
	src/pdfdoc.c \
	src/pdfdraw.c \
	src/pdfencrypt.c \
	src/pdfnames.c \
	src/pdfnames.h \
	src/pdfobj.c \
	src/pdfximage.c \
	src/spc_dvips.c \
	src/spc_pdfm.c \
	src/spc_tpic.c \
	src/specials.c \
	src/tfm.c \
	src/truetype.c \
	src/tt_cmap.c \
	src/type1.c \
	src/type1c.c \
	src/xbb.c

xdvipdfmx_sources = \
	xsrc/cff_dict.c \
	xsrc/cidtype0.c \
	xsrc/dvi.c \
	xsrc/dvipdfmx.c \
	xsrc/epdf.c \
	xsrc/jpegimage.c \
	xsrc/pdfcolor.c \
	xsrc/pdfdev.c \
	xsrc/pdfdoc.c \
	xsrc/pdfdraw.c \
	xsrc/pdfencrypt.c \
	xsrc/pdfnames.c \
	xsrc/pdfnames.h \
	xsrc/pdfobj.c \
	xsrc/pdfximage.c \
	xsrc/spc_dvips.c \
	xsrc/spc_pdfm.c \
	xsrc/spc_tpic.c \
	spc_xtx.c \
	spc_xtx.h \
	xsrc/specials.c \
	xsrc/tfm.c \
	xsrc/truetype.c \
	xsrc/tt_cmap.c \
	xsrc/type1.c \
	xsrc/type1c.c \
	xsrc/xbb.c

EXTRA_DIST = $(common_sources) $(dvipdfmx_sources) $(xdvipdfmx_sources)

## Links
##
bin_links =

if DVIPDFMX
if WIN32
noinst_PROGRAMS = call_dvipdfmx
endif WIN32
bin_links += \
	dvipdfmx$(EXEEXT):dvipdfm \
	dvipdfmx$(EXEEXT):ebb \
	dvipdfmx$(EXEEXT):extractbb
endif DVIPDFMX

call_dvipdfmx_CPPFLAGS = -DEXEPROG=\"dvipdfmx.exe\"
nodist_call_dvipdfmx_SOURCES = callexe.c

include $(top_srcdir)/../../am/bin_links.am

## Manpages
##
# Note:  dvipdft.1 used here needs to be updated
# and dvipdfm.1 be replaced by dvipdfmx.1 with a link to dvipdfm.1

man1_links =
if DVIPDFMX
dist_man1_MANS = man/dvipdfm.1 man/dvipdft.1 man/extractbb.1
man1_links += extractbb:ebb
endif DVIPDFMX

include $(top_srcdir)/../../am/man1_links.am

## Data
##
configdatadir = $(datarootdir)/texmf-dist/dvipdfmx
dist_configdata_DATA = data/dvipdfmx.cfg
 
glyphlistdatadir = $(datarootdir)/texmf-dist/fonts/map/glyphlist
dist_glyphlistdata_DATA = data/glyphlist.txt data/pdfglyphlist.txt

mapdatadir = $(datarootdir)/texmf-dist/fonts/map/dvipdfmx
dist_mapdata_DATA = data/cid-x.map

cmapdatadir = $(datarootdir)/texmf-dist/fonts/cmap/dvipdfmx
dist_cmapdata_DATA = data/EUC-UCS2

install-exec-hook: install-bin-links
if DVIPDFMX
if WIN32
	$(INSTALL_PROGRAM) call_dvipdfmx$(EXEEXT) $(DESTDIR)$(bindir)/dvipdfm$(EXEEXT)
	$(INSTALL_PROGRAM) call_dvipdfmx$(EXEEXT) $(DESTDIR)$(bindir)/ebb$(EXEEXT)
	$(INSTALL_PROGRAM) call_dvipdfmx$(EXEEXT) $(DESTDIR)$(bindir)/extractbb$(EXEEXT)
endif WIN32
endif DVIPDFMX

install-data-hook: install-man1-links

uninstall-hook: uninstall-bin-links uninstall-man1-links
if DVIPDFMX
if WIN32
	rm -f $(DESTDIR)$(bindir)/dvipdfm$(EXEEXT)
	rm -f $(DESTDIR)$(bindir)/ebb$(EXEEXT)
	rm -f $(DESTDIR)$(bindir)/extractbb$(EXEEXT)
endif WIN32
endif DVIPDFMX

all-local: config.force

config.force: $(ZLIB_DEPEND) $(LIBPNG_DEPEND) $(KPATHSEA_DEPEND) $(FREETYPE2_DEPEND) 
	echo timestamp >config.force
	$(SHELL) ./config.status --recheck

DISTCLEANFILES = config.force

## Rebuild libkpathsea
@KPATHSEA_RULE@
## Rebuild libfreetype
@FREETYPE2_RULE@
## Rebuild libpng
@LIBPNG_RULE@
## Rebuild libz
@ZLIB_RULE@

## Tests
##
TESTS =
dvipdfmx_tests = dvipdfmx.test dvipdfm-bb.test
if DVIPDFMX
TESTS += $(dvipdfmx_tests)
endif DVIPDFMX
xdvipdfmx_tests = xdvipdfmx.test xdvipdfm-bb.test
if XDVIPDFMX
TESTS += $(xdvipdfmx_tests)
endif XDVIPDFMX
EXTRA_DIST += $(dvipdfmx_tests) $(xdvipdfmx_tests)
## Test Data
##
EXTRA_DIST += tests/dvipdfmx.cfg
EXTRA_DIST += tests/image.jpeg tests/imgjpeg.bb tests/imgjpeg.xbb tests/picjpeg.bb tests/picjpeg.xbb
EXTRA_DIST += tests/image.pdf tests/imgpdf.bb tests/imgpdf.xbb tests/picpdf.bb tests/picpdf.xbb
EXTRA_DIST += tests/image.png tests/imgpng.bb tests/imgpng.xbb tests/picpng.bb tests/picpng.xbb
DISTCLEANFILES += img*.* pic*.*

