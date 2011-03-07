## Makefile.am for the TeX Live subdirectory texk/dvipdfmx/
##
## Copyright (C) 2009 - 2011 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4
AM_CFLAGS = $(WARNING_CFLAGS)

## We want to re-distribute the whole original dvipdfmx source tree.
##
EXTRA_DIST = $(DVIPDFMX_TREE)

## Patches applied to the original source tree
##
EXTRA_DIST += $(DVIPDFMX_TREE)-PATCHES

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn`

## Programs
##
bin_PROGRAMS = dvipdfmx
if WIN32
noinst_PROGRAMS = call_dvipdfmx
else !WIN32
dist_bin_SCRIPTS = src/dvipdft
endif !WIN32

dvipdfmx_SOURCES = \
	$(DVIPDFMX_TREE)/src/agl.c \
	$(DVIPDFMX_TREE)/src/agl.h \
	$(DVIPDFMX_TREE)/src/bmpimage.c \
	$(DVIPDFMX_TREE)/src/bmpimage.h \
	$(DVIPDFMX_TREE)/src/cff.c \
	$(DVIPDFMX_TREE)/src/cff.h \
	$(DVIPDFMX_TREE)/src/cff_dict.c \
	$(DVIPDFMX_TREE)/src/cff_dict.h \
	$(DVIPDFMX_TREE)/src/cff_limits.h \
	$(DVIPDFMX_TREE)/src/cff_stdstr.h \
	$(DVIPDFMX_TREE)/src/cff_types.h \
	$(DVIPDFMX_TREE)/src/cid.c \
	$(DVIPDFMX_TREE)/src/cid.h \
	$(DVIPDFMX_TREE)/src/cid_basefont.h \
	$(DVIPDFMX_TREE)/src/cid_p.h \
	$(DVIPDFMX_TREE)/src/cidtype0.c \
	$(DVIPDFMX_TREE)/src/cidtype0.h \
	$(DVIPDFMX_TREE)/src/cidtype2.c \
	$(DVIPDFMX_TREE)/src/cidtype2.h \
	$(DVIPDFMX_TREE)/src/cmap.c \
	$(DVIPDFMX_TREE)/src/cmap.h \
	$(DVIPDFMX_TREE)/src/cmap_p.h \
	$(DVIPDFMX_TREE)/src/cmap_read.c \
	$(DVIPDFMX_TREE)/src/cmap_read.h \
	$(DVIPDFMX_TREE)/src/cmap_write.c \
	$(DVIPDFMX_TREE)/src/cmap_write.h \
	$(DVIPDFMX_TREE)/src/cs_type2.c \
	$(DVIPDFMX_TREE)/src/cs_type2.h \
	$(DVIPDFMX_TREE)/src/dpxconf.c \
	$(DVIPDFMX_TREE)/src/dpxconf.h \
	$(DVIPDFMX_TREE)/src/dpxcrypt.c \
	$(DVIPDFMX_TREE)/src/dpxcrypt.h \
	$(DVIPDFMX_TREE)/src/dpxfile.c \
	$(DVIPDFMX_TREE)/src/dpxfile.h \
	$(DVIPDFMX_TREE)/src/dpxutil.c \
	$(DVIPDFMX_TREE)/src/dpxutil.h \
	$(DVIPDFMX_TREE)/src/dvi.c \
	$(DVIPDFMX_TREE)/src/dvi.h \
	$(DVIPDFMX_TREE)/src/dvicodes.h \
	$(DVIPDFMX_TREE)/src/dvipdfmx.c \
	$(DVIPDFMX_TREE)/src/dvipdfmx.h \
	$(DVIPDFMX_TREE)/src/epdf.c \
	$(DVIPDFMX_TREE)/src/epdf.h \
	$(DVIPDFMX_TREE)/src/error.c \
	$(DVIPDFMX_TREE)/src/error.h \
	$(DVIPDFMX_TREE)/src/fontmap.c \
	$(DVIPDFMX_TREE)/src/fontmap.h \
	$(DVIPDFMX_TREE)/src/jpegimage.c \
	$(DVIPDFMX_TREE)/src/jpegimage.h \
	$(DVIPDFMX_TREE)/src/mem.c \
	$(DVIPDFMX_TREE)/src/mem.h \
	$(DVIPDFMX_TREE)/src/mfileio.c \
	$(DVIPDFMX_TREE)/src/mfileio.h \
	$(DVIPDFMX_TREE)/src/mpost.c \
	$(DVIPDFMX_TREE)/src/mpost.h \
	$(DVIPDFMX_TREE)/src/numbers.c \
	$(DVIPDFMX_TREE)/src/numbers.h \
	$(DVIPDFMX_TREE)/src/otl_conf.c \
	$(DVIPDFMX_TREE)/src/otl_conf.h \
	$(DVIPDFMX_TREE)/src/otl_opt.c \
	$(DVIPDFMX_TREE)/src/otl_opt.h \
	$(DVIPDFMX_TREE)/src/pdfcolor.c \
	$(DVIPDFMX_TREE)/src/pdfcolor.h \
	$(DVIPDFMX_TREE)/src/pdfdev.c \
	$(DVIPDFMX_TREE)/src/pdfdev.h \
	$(DVIPDFMX_TREE)/src/pdfdoc.c \
	$(DVIPDFMX_TREE)/src/pdfdoc.h \
	$(DVIPDFMX_TREE)/src/pdfdraw.c \
	$(DVIPDFMX_TREE)/src/pdfdraw.h \
	$(DVIPDFMX_TREE)/src/pdfencoding.c \
	$(DVIPDFMX_TREE)/src/pdfencoding.h \
	$(DVIPDFMX_TREE)/src/pdfencrypt.c \
	$(DVIPDFMX_TREE)/src/pdfencrypt.h \
	$(DVIPDFMX_TREE)/src/pdffont.c \
	$(DVIPDFMX_TREE)/src/pdffont.h \
	$(DVIPDFMX_TREE)/src/pdflimits.h \
	$(DVIPDFMX_TREE)/src/pdfnames.c \
	$(DVIPDFMX_TREE)/src/pdfnames.h \
	$(DVIPDFMX_TREE)/src/pdfobj.c \
	$(DVIPDFMX_TREE)/src/pdfobj.h \
	$(DVIPDFMX_TREE)/src/pdfparse.c \
	$(DVIPDFMX_TREE)/src/pdfparse.h \
	$(DVIPDFMX_TREE)/src/pdfresource.c \
	$(DVIPDFMX_TREE)/src/pdfresource.h \
	$(DVIPDFMX_TREE)/src/pdfximage.c \
	$(DVIPDFMX_TREE)/src/pdfximage.h \
	$(DVIPDFMX_TREE)/src/pkfont.c \
	$(DVIPDFMX_TREE)/src/pkfont.h \
	$(DVIPDFMX_TREE)/src/pngimage.c \
	$(DVIPDFMX_TREE)/src/pngimage.h \
	$(DVIPDFMX_TREE)/src/pst.c \
	$(DVIPDFMX_TREE)/src/pst.h \
	$(DVIPDFMX_TREE)/src/pst_obj.c \
	$(DVIPDFMX_TREE)/src/pst_obj.h \
	$(DVIPDFMX_TREE)/src/sfnt.c \
	$(DVIPDFMX_TREE)/src/sfnt.h \
	$(DVIPDFMX_TREE)/src/spc_color.c \
	$(DVIPDFMX_TREE)/src/spc_color.h \
	$(DVIPDFMX_TREE)/src/spc_dvips.c \
	$(DVIPDFMX_TREE)/src/spc_dvips.h \
	$(DVIPDFMX_TREE)/src/spc_html.c \
	$(DVIPDFMX_TREE)/src/spc_html.h \
	$(DVIPDFMX_TREE)/src/spc_misc.c \
	$(DVIPDFMX_TREE)/src/spc_misc.h \
	$(DVIPDFMX_TREE)/src/spc_pdfm.c \
	$(DVIPDFMX_TREE)/src/spc_pdfm.h \
	$(DVIPDFMX_TREE)/src/spc_tpic.c \
	$(DVIPDFMX_TREE)/src/spc_tpic.h \
	$(DVIPDFMX_TREE)/src/spc_util.c \
	$(DVIPDFMX_TREE)/src/spc_util.h \
	$(DVIPDFMX_TREE)/src/specials.c \
	$(DVIPDFMX_TREE)/src/specials.h \
	$(DVIPDFMX_TREE)/src/subfont.c \
	$(DVIPDFMX_TREE)/src/subfont.h \
	$(DVIPDFMX_TREE)/src/system.h \
	$(DVIPDFMX_TREE)/src/t1_char.c \
	$(DVIPDFMX_TREE)/src/t1_char.h \
	$(DVIPDFMX_TREE)/src/t1_load.c \
	$(DVIPDFMX_TREE)/src/t1_load.h \
	$(DVIPDFMX_TREE)/src/tfm.c \
	$(DVIPDFMX_TREE)/src/tfm.h \
	$(DVIPDFMX_TREE)/src/truetype.c \
	$(DVIPDFMX_TREE)/src/truetype.h \
	$(DVIPDFMX_TREE)/src/tt_aux.c \
	$(DVIPDFMX_TREE)/src/tt_aux.h \
	$(DVIPDFMX_TREE)/src/tt_cmap.c \
	$(DVIPDFMX_TREE)/src/tt_cmap.h \
	$(DVIPDFMX_TREE)/src/tt_glyf.c \
	$(DVIPDFMX_TREE)/src/tt_glyf.h \
	$(DVIPDFMX_TREE)/src/tt_gsub.c \
	$(DVIPDFMX_TREE)/src/tt_gsub.h \
	$(DVIPDFMX_TREE)/src/tt_post.c \
	$(DVIPDFMX_TREE)/src/tt_post.h \
	$(DVIPDFMX_TREE)/src/tt_table.c \
	$(DVIPDFMX_TREE)/src/tt_table.h \
	$(DVIPDFMX_TREE)/src/type0.c \
	$(DVIPDFMX_TREE)/src/type0.h \
	$(DVIPDFMX_TREE)/src/type1.c \
	$(DVIPDFMX_TREE)/src/type1.h \
	$(DVIPDFMX_TREE)/src/type1c.c \
	$(DVIPDFMX_TREE)/src/type1c.h \
	$(DVIPDFMX_TREE)/src/unicode.c \
	$(DVIPDFMX_TREE)/src/unicode.h \
	$(DVIPDFMX_TREE)/src/vf.c \
	$(DVIPDFMX_TREE)/src/vf.h \
	$(DVIPDFMX_TREE)/src/xbb.c \
	$(DVIPDFMX_TREE)/src/xbb.h

dvipdfmx_CPPFLAGS = $(KPATHSEA_INCLUDES) $(LIBPNG_INCLUDES) $(ZLIB_INCLUDES)

dvipdfmx_DEPENDENCIES = $(KPATHSEA_DEPEND) $(LIBPNG_DEPEND) $(ZLIB_DEPEND)

dvipdfmx_LDADD = $(KPATHSEA_LIBS) $(LIBPNG_LIBS) $(ZLIB_LIBS)

call_dvipdfmx_SOURCES = $(srcdir)/../texlive/w32_wrapper/callexe.c

call_dvipdfmx_CPPFLAGS = -DEXEPROG=\"dvipdfmx.exe\"

## Rebuild libkpathsea
@KPATHSEA_RULE@
## Rebuild libpng
@LIBPNG_RULE@
## Rebuild libz
@ZLIB_RULE@

$(dvipdfmx_OBJECTS) $(call_dvipdfmx_OBJECTS): config.force

config.force: $(ZLIB_DEPEND) $(LIBPNG_DEPEND) $(KPATHSEA_DEPEND)
	echo timestamp >config.force
	$(SHELL) ./config.status --recheck

DISTCLEANFILES = config.force

## Data
##
configdatadir = ${prefix}/texmf/dvipdfmx
configdata_DATA = \
	$(DVIPDFMX_TREE)/data/dvipdfmx.cfg

glyphlistdatadir = ${prefix}/texmf-dist/fonts/map/glyphlist
glyphlistdata_DATA = \
	$(DVIPDFMX_TREE)/data/glyphlist.txt \
	$(DVIPDFMX_TREE)/data/pdfglyphlist.txt

mapdatadir = ${prefix}/texmf/fonts/map/dvipdfmx
mapdata_DATA = \
	$(DVIPDFMX_TREE)/data/cid-x.map

cmapdatadir = ${prefix}/texmf/fonts/cmap/dvipdfmx
cmapdata_DATA = \
	$(DVIPDFMX_TREE)/data/EUC-UCS2

## Manual pages
##
man1_MANS = \
	$(DVIPDFMX_TREE)/man/extractbb.1
# Note:  dvipdft.1 used here needs to be updated
# and dvipdfm.1 be replaced by dvipdfmx.1 with a link to dvipdfm.1
dist_man1_MANS = \
	man/dvipdfm.1 \
	man/dvipdft.1

# Symlinks within $(man1dir): FILE:LINK indicates LINK.1->FILE.1
man1_links = \
	extractbb:ebb

include $(srcdir)/../am/man1_links.am

install-data-hook: install-man1-links

install-exec-hook:
if WIN32
	$(INSTALL_PROGRAM) call_dvipdfmx$(EXEEXT) $(DESTDIR)$(bindir)/dvipdfm$(EXEEXT)
	$(INSTALL_PROGRAM) call_dvipdfmx$(EXEEXT) $(DESTDIR)$(bindir)/ebb$(EXEEXT)
	$(INSTALL_PROGRAM) call_dvipdfmx$(EXEEXT) $(DESTDIR)$(bindir)/extractbb$(EXEEXT)
else !WIN32
	cd $(DESTDIR)$(bindir) && \
	  rm -f ebb$(EXEEXT) extractbb$(EXEEXT) dvipdfm$(EXEEXT) && \
	  $(LN_S) dvipdfmx$(EXEEXT) dvipdfm$(EXEEXT) && \
	  $(LN_S) dvipdfmx$(EXEEXT) ebb$(EXEEXT) && \
	  $(LN_S) dvipdfmx$(EXEEXT) extractbb$(EXEEXT)
endif !WIN32

uninstall-hook: uninstall-man1-links
	rm -f $(DESTDIR)$(bindir)/dvipdfm$(EXEEXT)
	rm -f $(DESTDIR)$(bindir)/ebb$(EXEEXT)
	rm -f $(DESTDIR)$(bindir)/extractbb$(EXEEXT)

