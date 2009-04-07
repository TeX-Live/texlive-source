##
ACLOCAL_AMFLAGS = -I ../../m4 -I m4

SUBDIRS = squeeze

INCLUDES = $(KPATHSEA_INCLUDES) $(T1LIB_INCLUDES) -I$(srcdir)/gui $(PS_DEF)

## building without t1lib not supported
INCLUDES += -DT1LIB

## building without GREY or COLOR not supported
INCLUDES += -DGREY -DCOLOR

## For testing: either noinst_LIBRARIES or EXTRA_LIBRARIES!
noinst_LIBRARIES = libxdvi.a
## EXTRA_LIBRARIES = libxdvi.a

libxdvi_a_SOURCES = \
	alloc-debug.h \
	browser.c \
	browser.h \
	c-openmx.h \
	dl_list.c \
	dl_list.h \
	dvi-draw.c \
	dvi-draw.h \
	dvi-init.c \
	dvi-init.h \
	dvi.h \
	dvisel.c \
	dvisel.h \
	encodings.c \
	encodings.h \
	events.c \
	events.h \
	exit-handlers.c \
	exit-handlers.h \
	filehist.c \
	filehist.h \
	font-open.c \
	font-open.h \
	gf.c \
	hypertex.c \
	hypertex.h \
	image-magick.c \
	image-magick.h \
	main.c \
	mime.c \
	mime.h \
	my-snprintf.c \
	my-snprintf.h \
	my-vsnprintf.c \
	my-vsnprintf.h \
	pagehist.c \
	pagehist.h \
	pk.c \
	print-internal.c \
	print-internal.h \
	psdps.c \
	psdps.h \
	psgs.c \
	psgs.h \
	psnews.c \
	psnews.h \
	read-mapfile.c \
	read-mapfile.h \
	search-internal.c \
	search-internal.h \
	special.c \
	special.h \
	string-utils.c \
	string-utils.h \
	string_list.c \
	string_list.h \
	tfmload.c \
	tfmload.h \
	translations.h \
	util.c \
	util.h \
	version.h \
	vf.c \
	x_util.c \
	x_util.h \
	xdvi.c \
	xdvi.h \
	xserver-info.c \
	xserver-info.h

## dvips_DEPENDENCIES = $(KPATHSEA_DEPEND)

## LDADD = $(KPATHSEA_LIBS)

## Rebuild libkpathsea
@KPATHSEA_RULE@
## Rebuild libt1
@T1LIB_RULE@

nodist_libxdvi_a_SOURCES = psheader.c

psheader.c: psheader.txt squeeze/squeeze$(EXEEXT)
	squeeze/squeeze $(srcdir)/psheader.txt $@

squeeze/squeeze$(EXEEXT):
	cd squeeze && $(MAKE) $(AM_MAKEFLAGS)

EXTRA_DIST = psheader.txt
CLEANFILES = psheader.c

xdvidir = ${prefix}/texmf/xdvi
dist_xdvi_DATA = texmf/XDvi

pixmapdir = ${prefix}/texmf/xdvi/pixmap
dist_pixmap_DATA = pixmaps/toolbar.xpm pixmaps/toolbar2.xpm

dist_noinst_DATA = texmf/xdvi.cfg

install-data-hook:
	@if grep "original xdvi.cfg --" "$(DESTDIR)$(xdvidir)/xdvi.cfg" >/dev/null 2>&1 \
	    || test ! -r "$(DESTDIR)$(xdvidir)/xdvi.cfg"; then \
	  echo " $(INSTALL_DATA) '$(srcdir)/texmf/xdvi.cfg' '$(DESTDIR)$(xdvidir)/xdvi.cfg'"; \
	  $(INSTALL_DATA) "$(srcdir)/texmf/xdvi.cfg" "$(DESTDIR)$(xdvidir)/xdvi.cfg"; \
	else :; fi

uninstall-hook:
	@if grep "original xdvi.cfg --" "$(DESTDIR)$(xdvidir)/xdvi.cfg" >/dev/null 2>&1 \
	    || test ! -r "$(DESTDIR)$(xdvidir)/xdvi.cfg"; then \
	  echo " rm -f '$(DESTDIR)$(xdvidir)/xdvi.cfg'"; \
	  rm -f "$(DESTDIR)$(xdvidir)/xdvi.cfg"; \
	else :; fi

## Not yet used
##
EXTRA_DIST += \
	BUGS \
	CHANGES \
	ChangeLog.TL \
	FAQ \
	LESSTIF-BUGS \
	README.t1fonts \
	README.t1mapper \
	gui/Makefile.in \
	gui/Panner.c \
	gui/Panner.h \
	gui/PannerP.h \
	gui/Tip.c \
	gui/Tip.h \
	gui/TipP.h \
	gui/help-window.c \
	gui/help-window.h \
	gui/mag.c \
	gui/mag.h \
	gui/menu.c \
	gui/menu.h \
	gui/message-window.c \
	gui/message-window.h \
	gui/pagesel.c \
	gui/pagesel.h \
	gui/print-dialog.c \
	gui/print-dialog.h \
	gui/print-log.c \
	gui/print-log.h \
	gui/search-dialog.c \
	gui/search-dialog.h \
	gui/selection.c \
	gui/selection.h \
	gui/sfDir.c \
	gui/sfDir.h \
	gui/sfDraw.c \
	gui/sfDraw.h \
	gui/sfPath.c \
	gui/sfPath.h \
	gui/sfSelFile.c \
	gui/sfSelFile.h \
	gui/sfinternal.h \
	gui/statusline.c \
	gui/statusline.h \
	gui/topic-window.c \
	gui/topic-window.h \
	gui/xaw_bitmaps.c \
	gui/xaw_bitmaps.h \
	gui/xaw_menu.c \
	gui/xaw_menu.h \
	gui/xicon.c \
	gui/xicon.h \
	gui/xlwradio.c \
	gui/xlwradio.h \
	gui/xlwradioP.h \
	gui/xm_colorsel.c \
	gui/xm_colorsel.h \
	gui/xm_filesel.c \
	gui/xm_menu.c \
	gui/xm_menu.h \
	gui/xm_prefs.c \
	gui/xm_prefs.h \
	gui/xm_prefsP.h \
	gui/xm_prefs_appearance.c \
	gui/xm_prefs_appearance.h \
	gui/xm_prefs_fonts.c \
	gui/xm_prefs_fonts.h \
	gui/xm_prefs_helpers.c \
	gui/xm_prefs_helpers.h \
	gui/xm_prefs_page.c \
	gui/xm_prefs_page.h \
	gui/xm_prefs_scroll.c \
	gui/xm_prefs_scroll.h \
	gui/xm_toolbar.c \
	gui/xm_toolbar.h \
	m4/acinclude.m4 \
	mksedscript \
	pixmaps/drag_horiz.xbm \
	pixmaps/drag_horiz_mask.xbm \
	pixmaps/drag_omni.xbm \
	pixmaps/drag_omni_mask.xbm \
	pixmaps/drag_vert.xbm \
	pixmaps/drag_vert_mask.xbm \
	pixmaps/fist.xbm \
	pixmaps/fist_mask.xbm \
	pixmaps/hand.xbm \
	pixmaps/hand_mask.xbm \
	pixmaps/magglass.xbm \
	pixmaps/magglass_mask.xbm \
	pixmaps/time16.xbm \
	pixmaps/time16_mask.xbm \
	pixmaps/xdvi16x16.xpm \
	pixmaps/xdvi32x32.xpm \
	pixmaps/xdvi48x48.xpm \
	t1mapper \
	t1mapper.1 \
	tests/Makefile.in \
	tests/README \
	tests/run_tests.c \
	tests/run_tests.h \
	tests/test1.c \
	tests/test_dl_list.c \
	tests/test_string_list.c \
	tests/test_string_utils.c \
	tests/test_util.c \
	texmf-alt/README \
	texmf-alt/charter2.map \
	texmf-alt/lucida.map \
	texmf-alt/marvosym.map \
	texmf-alt/xdvi.cfg \
	texmf-alt/xdvifont.map \
	texmf-alt/xypic.map \
	xdvi-config.h \
	xdvi-debug.h \
	xdvi-search.el \
	xdvi-sh.in \
	xdvi.1.in \
	xdvi.icon \
	xdvizilla \
	xdvizilla.1

## Original files from xdvik-22.84.14
##
EXTRA_DIST += \
	Makefile.in-22.84.14 \
	acconfig.h-22.84.14 \
	acinclude.m4-22.84.14 \
	aclocal.m4-22.84.14 \
	configure.in-22.84.14 \
	kpathsea.ac-22.84.14 \
	withenable.ac-22.84.14

## Original files from xdvik-22.84.15
##
EXTRA_DIST += \
	Makefile.in-22.84.15 \
	acconfig.h-22.84.15 \
	aclocal.m4-22.84.15 \
	configure.in-22.84.15 \
	gui/Makefile.in-22.84.15 \
	kpathsea.ac-22.84.15 \
	tests/Makefile.in-22.84.15 \
	withenable.ac-22.84.15

## Eventually delete these files
##
EXTRA_DIST += \
	Makefile.in.orig \
	Makefile.in.work \
	configure.in.orig \
	configure.in.work \
	gui/Makefile.in.orig \
	kpathsea.ac.orig \
	tests/Makefile.in.orig \
	withenable.ac.orig

