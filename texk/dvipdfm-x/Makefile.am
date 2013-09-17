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

EXTRA_LIBRARIES = libutil.a

AM_CPPFLAGS = $(KPATHSEA_INCLUDES) $(LIBPNG_INCLUDES) $(ZLIB_INCLUDES) $(PAPER_INCLUDES)
xdvipdfmx_CPPFLAGS = -DXETEX $(FREETYPE2_INCLUDES) $(AM_CPPFLAGS)
AM_CFLAGS = $(WARNING_CFLAGS)

libutil_a_SOURCES = \
	agl.c \
	agl.h \
	bmpimage.h \
	cff_dict.h \
	cff_limits.h \
	cff_stdstr.h \
	cff_types.h \
	cid_basefont.h \
	cidtype0.c \
	cidtype0.h \
	cidtype2.h \
	cmap.h \
	cmap_p.h \
	cmap_read.h \
	cmap_write.c \
	cmap_write.h \
	cs_type2.c \
	cs_type2.h \
	dpxconf.c \
	dpxconf.h \
	dpxcrypt.c \
	dpxcrypt.h \
	dpxfile.h \
	dpxutil.c \
	dpxutil.h \
	dvi.h \
	dvicodes.h \
	dvipdfmx.h \
	epdf.h \
	error.c \
	error.h \
	jpegimage.c \
	jpegimage.h \
	mem.c \
	mem.h \
	mfileio.c \
	mfileio.h \
	mpost.h \
	numbers.c \
	numbers.h \
	otl_conf.c \
	otl_conf.h \
	otl_opt.c \
	otl_opt.h \
	pdfcolor.c \
	pdfcolor.h \
	pdfdev.h \
	pdfdoc.h \
	pdfdraw.h \
	pdfencrypt.c \
	pdfencrypt.h \
	pdfencoding.c \
	pdfencoding.h \
	pdflimits.h \
	pdfnames.h \
	pdfobj.h \
	pdfparse.h \
	pdfresource.c \
	pdfresource.h \
	pdfximage.h \
	pkfont.h \
	pngimage.c \
	pngimage.h \
	pst.c \
	pst.h \
	pst_obj.c \
	pst_obj.h \
	spc_color.h \
	spc_dvips.h \
	spc_html.h \
	spc_misc.c \
	spc_misc.h \
	spc_pdfm.h \
	spc_tpic.h \
	spc_util.h \
	spc_util.c \
	specials.h \
	subfont.c \
	subfont.h \
	system.h \
	t1_char.h \
	t1_load.h \
	tfm.c \
	tfm.h \
	truetype.h \
	tt_aux.h \
	tt_glyf.h \
	tt_gsub.h \
	tt_post.h \
	tt_table.h \
	type0.h \
	type1.h \
	type1c.c \
	type1c.h \
	unicode.c \
	unicode.h \
	vf.c \
	vf.h \
	xbb.c

dvipdfmx_SOURCES = \
	bmpimage.c \
	cff.c \
	cff.h \
	cff_dict.c \
	cid.c \
	cid.h \
	cid_p.h \
	cidtype2.c \
	cmap.c \
	cmap_read.c \
	dpxfile.c \
	dvi.c \
	dvipdfmx.c \
	epdf.c \
	fontmap.c \
	fontmap.h \
	mpost.c \
	pdfdev.c \
	pdfdoc.c \
	pdfdraw.c \
	pdffont.c \
	pdffont.h \
	pdfnames.c \
	pdfobj.c \
	pdfparse.c \
	pdfximage.c \
	pkfont.c \
	sfnt.c \
	sfnt.h \
	spc_color.c \
	spc_dvips.c \
	spc_html.c \
	spc_pdfm.c \
	spc_tpic.c \
	spc_xtx.c \
	spc_xtx.h \
	specials.c \
	t1_char.c \
	t1_load.c \
	truetype.c \
	tt_aux.c \
	tt_cmap.c \
	tt_cmap.h \
	tt_glyf.c \
	tt_gsub.c \
	tt_post.c \
	tt_table.c \
	type0.c \
	type1.c

nodist_xdvipdfmx_SOURCES = $(dvipdfmx_SOURCES)

dvipdfmx_LDADD = libutil.a $(KPATHSEA_LIBS) $(LIBPNG_LIBS) $(ZLIB_LIBS) $(PAPER_LIBS)
xdvipdfmx_LDADD = $(FREETYPE2_LIBS) $(dvipdfmx_LDADD)

$(dvipdfmx_OBJECTS) $(xdvipdfmx_OBJECTS): config.force

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
dvipdfmx_tests = dvipdfmx.test dvipdfm-ann.test dvipdfm-bb.test dvipdfm-bkm.test dvipdfm-psz.test
if DVIPDFMX
TESTS += $(dvipdfmx_tests)
endif DVIPDFMX
xdvipdfmx_tests = xdvipdfmx.test xdvipdfm-ann.test xdvipdfm-bb.test xdvipdfm-bkm.test xdvipdfm-psz.test
if XDVIPDFMX
TESTS += $(xdvipdfmx_tests)
endif XDVIPDFMX
EXTRA_DIST = $(dvipdfmx_tests) $(xdvipdfmx_tests)
## Test Data
EXTRA_DIST += tests/dvipdfmx.cfg tests/psfonts.map
EXTRA_DIST += tests/image.jpeg tests/imgjpeg.bb tests/imgjpeg.xbb tests/picjpeg.bb tests/picjpeg.xbb
EXTRA_DIST += tests/image.pdf tests/imgpdf.bb tests/imgpdf.xbb tests/picpdf.bb tests/picpdf.xbb
EXTRA_DIST += tests/image.png tests/imgpng.bb tests/imgpng.xbb tests/picpng.bb tests/picpng.xbb
EXTRA_DIST += tests/image.dvi tests/image.tex
EXTRA_DIST += tests/cmr10.pfb tests/cmr10.tfm tests/annot.dvi tests/annot.tex
EXTRA_DIST += tests/bookm.dvi tests/bookm.tex tests/paper.dvi tests/paper.tex
DISTCLEANFILES += img*.* pic*.* image*.pdf annot*.pdf bookm*.pdf paper*.pdf

