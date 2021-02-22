## Makefile.am for the TeX Live subdirectory texk/dvipdfm-x/
##
## Copyright (C) 2009-2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
bin_PROGRAMS = xdvipdfmx
if !WIN32
dist_bin_SCRIPTS = dvipdft
endif !WIN32

AM_CPPFLAGS = $(KPATHSEA_INCLUDES) $(LIBPNG_INCLUDES) $(ZLIB_INCLUDES) $(LIBPAPER_INCLUDES)
AM_CFLAGS = $(WARNING_CFLAGS)

xdvipdfmx_SOURCES = \
	agl.c \
	agl.h \
	bmpimage.c \
	bmpimage.h \
	cff.c \
	cff.h \
	cff_dict.c \
	cff_dict.h \
	cff_limits.h \
	cff_stdstr.h \
	cff_types.h \
	cid_basefont.h \
	cid.c \
	cid.h \
	cidtype0.c \
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
	dvi.c \
	dvi.h \
	dvicodes.h \
	dvipdfmx.c \
	dvipdfmx.h \
	epdf.c \
	epdf.h \
	error.c \
	error.h \
	fontmap.c \
	fontmap.h \
	jp2image.c \
	jp2image.h \
	jpegimage.c \
	jpegimage.h \
	mem.c \
	mem.h \
	mfileio.c \
	mfileio.h \
	mpost.c \
	mpost.h \
	mt19937ar.c \
	numbers.c \
	numbers.h \
	otl_opt.c \
	otl_opt.h \
	pdfcolor.c \
	pdfcolor.h \
	pdfdev.c \
	pdfdev.h \
	pdfdoc.c \
	pdfdoc.h \
	pdfdraw.c \
	pdfdraw.h \
	pdfencrypt.c \
	pdfencrypt.h \
	pdfencoding.c \
	pdfencoding.h \
	pdffont.c \
	pdffont.h \
	pdflimits.h \
	pdfnames.c \
	pdfnames.h \
	pdfobj.c \
	pdfobj.h \
	pdfparse.c \
	pdfparse.h \
	pdfresource.c \
	pdfresource.h \
	pdfximage.c \
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
	spc_dvipdfmx.c \
	spc_dvipdfmx.h \
	spc_dvips.c \
	spc_dvips.h \
	spc_html.c \
	spc_html.h \
	spc_misc.c \
	spc_misc.h \
	spc_pdfm.c \
	spc_pdfm.h \
	spc_tpic.c \
	spc_tpic.h \
	spc_util.h \
	spc_util.c \
	spc_xtx.c \
	spc_xtx.h \
	specials.c \
	specials.h \
	subfont.c \
	subfont.h \
	system.h \
	t1_char.c \
	t1_char.h \
	t1_load.c \
	t1_load.h \
	tfm.c \
	tfm.h \
	truetype.c \
	truetype.h \
	tt_aux.c \
	tt_aux.h \
	tt_cmap.c \
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
	type1.c \
	type1.h \
	type1c.c \
	type1c.h \
	unicode.c \
	unicode.h \
	vf.c \
	vf.h \
	xbb.c

LDADD = $(KPATHSEA_LIBS) $(LIBPNG_LIBS) $(ZLIB_LIBS) $(LIBPAPER_LIBS)

$(xdvipdfmx_OBJECTS): config.force

## Links
##
bin_links = \
	xdvipdfmx$(EXEEXT):dvipdfm \
	xdvipdfmx$(EXEEXT):dvipdfmx \
	xdvipdfmx$(EXEEXT):ebb \
	xdvipdfmx$(EXEEXT):extractbb

if WIN32
noinst_PROGRAMS = call_xdvipdfmx
endif WIN32

call_xdvipdfmx_CPPFLAGS = -DEXEPROG=\"xdvipdfmx.exe\"
nodist_call_xdvipdfmx_SOURCES = callexe.c

include $(top_srcdir)/../../am/bin_links.am

## Manpages

dist_man1_MANS = man/dvipdfmx.1 man/dvipdft.1 man/extractbb.1
man1_links = \
	dvipdfmx:dvipdfm \
	dvipdfmx:xdvipdfmx \
	extractbb:ebb
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
if WIN32
	$(INSTALL_PROGRAM) call_xdvipdfmx$(EXEEXT) $(DESTDIR)$(bindir)/dvipdfm$(EXEEXT)
	$(INSTALL_PROGRAM) call_xdvipdfmx$(EXEEXT) $(DESTDIR)$(bindir)/dvipdfmx$(EXEEXT)
	$(INSTALL_PROGRAM) call_xdvipdfmx$(EXEEXT) $(DESTDIR)$(bindir)/ebb$(EXEEXT)
	$(INSTALL_PROGRAM) call_xdvipdfmx$(EXEEXT) $(DESTDIR)$(bindir)/extractbb$(EXEEXT)
endif WIN32

install-data-hook: install-man1-links

uninstall-hook: uninstall-bin-links uninstall-man1-links
if WIN32
	rm -f $(DESTDIR)$(bindir)/dvipdfm$(EXEEXT)
	rm -f $(DESTDIR)$(bindir)/dvipdfmx$(EXEEXT)
	rm -f $(DESTDIR)$(bindir)/ebb$(EXEEXT)
	rm -f $(DESTDIR)$(bindir)/extractbb$(EXEEXT)
endif WIN32

all-local: config.force

config.force: $(ZLIB_DEPEND) $(LIBPNG_DEPEND) $(KPATHSEA_DEPEND) $(LIBPAPER_DEPEND)
	echo timestamp >config.force
	$(SHELL) ./config.status --recheck

DISTCLEANFILES = config.force

## Rebuild libkpathsea
@KPATHSEA_RULE@
## Rebuild libpng
@LIBPNG_RULE@
## Rebuild libz
@ZLIB_RULE@
## Rebuild libpaper
@LIBPAPER_RULE@

## Tests
##
TESTS = xdvipdfmx.test xdvipdfm-ann.test xdvipdfm-bad.test xdvipdfm-bb.test
TESTS += xdvipdfm-bkm.test xdvipdfm-psz.test xdvipdfm-ptx.test xdvipdfm-res.test
TESTS += xdvipdfm-rev.test xdvipdfm-ttc.test
TESTS += dvipdfmx-upjf.test
xdvipdfmx.log xdvipdfm-ann.log xdvipdfm-bad.log xdvipdfm-bb.log \
	xdvipdfm-bkm.log xdvipdfm-psz.log xdvipdfm-ptx.log xdvipdfm-res.log \
	xdvipdfm-rev.log xdvipdfm-ttc.log: xdvipdfmx$(EXEEXT)
EXTRA_DIST = $(TESTS)
## xdvipdfmx.test
EXTRA_DIST += tests/dvipdfmx.cfg tests/psfonts.map
EXTRA_DIST += tests/cmr10.pfb tests/cmr10.tfm
EXTRA_DIST += tests/image.dvi tests/image.tex
EXTRA_DIST += tests/xbmc.dvi tests/xbmc.tex tests/xbmc10.600pk tests/xbmc10.tfm
DISTCLEANFILES += image*.pdf xbmc*.pdf
## xdvipdfm-ann.test
EXTRA_DIST += tests/annot.dvi tests/annot.tex
DISTCLEANFILES += annot*.pdf
## xdvipdfm-bad.test
EXTRA_DIST += tests/ids_2_6.dvi tests/ids_3_2.dvi tests/ids_a_b.dvi tests/opc_fe.dvi
EXTRA_DIST += tests/ptx_2_2.dvi tests/ptx_6_6.dvi tests/rev_2_2.dvi tests/rev_2_3.dvi
EXTRA_DIST += tests/void.dvi
## xdvipdfm-bb.test
EXTRA_DIST += tests/image.bmp tests/picbmp.bb tests/picbmp.xbb
EXTRA_DIST += tests/image.jp2 tests/picjp2.bb tests/picjp2.xbb
EXTRA_DIST += tests/image.jpeg tests/picjpeg.bb tests/picjpeg.xbb
EXTRA_DIST += tests/image.png tests/picpng.bb tests/picpng.xbb
EXTRA_DIST += tests/image.pdf tests/picpdf.bb tests/picpdf.xbb
DISTCLEANFILES += pic*.*
## xdvipdfm-bkm.test
EXTRA_DIST += tests/bookm.dvi tests/bookm.tex
DISTCLEANFILES += bookm*.pdf
## xdvipdfm-psz.test
EXTRA_DIST += tests/paper.dvi tests/paper.tex
DISTCLEANFILES += paper*.pdf
## xdvipdfm-ptx.test
EXTRA_DIST += tests/ptex.dvi
DISTCLEANFILES += ptex*.pdf
## xdvipdfm-res.test
EXTRA_DIST += tests/resrc.dvi tests/resrc.tex
DISTCLEANFILES += resrc*.pdf
## xdvipdfm-rev.test
EXTRA_DIST += tests/reverse.dvi
DISTCLEANFILES += reverse.pdf
## xdvipdfm-ttc.test
EXTRA_DIST += tests/ttc.dvi tests/ttc.tex tests/test.ttc
DISTCLEANFILES += ttc*.pdf
## dvipdfmx-upjf.test
EXTRA_DIST += tests/upjf.dvi tests/upjf.tex tests/upjf.map tests/Makefile_upjf
EXTRA_DIST += tests/upjf_full.cnf tests/upjf_omit.cnf tests/upjf_full.vf tests/upjf_omit.vf
EXTRA_DIST += tests/upjf-r.tfm tests/upjf-g.tfm tests/upjf.tfm tests/UPJF-UTF16-H
DISTCLEANFILES += upjf.vf upjf*.pdf
##
EXTRA_DIST += tests/fullmap.dvi tests/fullmap.tex
