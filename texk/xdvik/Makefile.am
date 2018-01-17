## Makefile.am for the TeX Live subdirectory texk/xdvik/
##
## Copyright (C) 2017 Karl Berry <tex-live@tug.org>
## Copyright (C) 2009-2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
AM_CFLAGS = $(WARNING_CFLAGS)

SUBDIRS = squeeze . tests

include common.am

AM_CPPFLAGS = -I$(srcdir)/gui $(common_includes)

## The object files created from ./*.c and gui/*.c are used for both
## ./xdvi-bin and tests/run_tests.
## In order not to compile them twice, we put them into a library.
noinst_LIBRARIES = libxdvi.a

nodist_libxdvi_a_SOURCES = psheader.c

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
	ft.c \
	gf.c \
	hypertex.c \
	hypertex.h \
	image-magick.c \
	image-magick.h \
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
	search-internal.c \
	search-internal.h \
	special.c \
	special.h \
	string-utils.c \
	string-utils.h \
	string_list.c \
	string_list.h \
	translations.h \
	util.c \
	util.h \
	version.h \
	vf.c \
	x_util.c \
	x_util.h \
	xdvi.c \
	xdvi-config.h \
	xdvi-debug.h \
	xserver-info.c \
	xdvi.h \
	xserver-info.h

## We cannot easily use two different libraries due to the mutual
## dependencies between routines in ./*.c and gui/*.c.
libxdvi_a_SOURCES += \
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
	gui/xm_toolbar.h

$(libxdvi_a_OBJECTS): config.force

config.force: $(common_dependencies)
	echo timestamp >config.force
	$(SHELL) ./config.status --recheck

DISTCLEANFILES = config.force

bin_SCRIPTS = xdvi

bin_PROGRAMS = xdvi-bin

xdvi_bin_SOURCES = main.c

$(xdvi_bin_OBJECTS): libxdvi.a $(common_dependencies)

LDADD = libxdvi.a $(common_ldadd)

# When cross-compiling, EXEEXT for the build and host systems may differ.
# Thus we depend on the auxiliary file squeeze/stamp-squeeze.
psheader.c: psheader.txt squeeze/stamp-squeeze
	$(AM_V_GEN)squeeze/squeeze $(srcdir)/psheader.txt $@

squeeze/stamp-squeeze:
	cd squeeze && $(MAKE) $(AM_MAKEFLAGS) stamp-squeeze

EXTRA_DIST = psheader.txt
CLEANFILES = psheader.c

nodist_man1_MANS = xdvi.1

xdvi.1: xdvi.1.in sedscript
	$(AM_V_GEN)sed -f sedscript <$(srcdir)/xdvi.1.in >$@

sedscript: mksedscript CHANGES Makefile c-auto.h
	$(AM_V_GEN)$(SHELL) $(srcdir)/mksedscript $(srcdir) $(DEFS) $(PS_DEF) >$@

EXTRA_DIST += mksedscript xdvi.1.in
CLEANFILES += sedscript xdvi.1

xdvidir = $(datarootdir)/texmf-dist/xdvi
dist_xdvi_DATA = texmf/XDvi

pixmapdir = $(datarootdir)/texmf-dist/xdvi/pixmap
dist_pixmap_DATA = pixmaps/toolbar.xpm pixmaps/toolbar2.xpm

dvipsconfdir = $(datarootdir)/texmf-dist/dvips/xdvi
dist_noinst_DATA = texmf/config.xdvi

desktopdir = $(datarootdir)/applications
desktop_DATA = xdvi.desktop

install-data-hook:
	@echo " $(MKDIR_P) '$(DESTDIR)$(dvipsconfdir)'"; \
	$(MKDIR_P) "$(DESTDIR)$(dvipsconfdir)" || exit 1; \
	if grep "original config.xdvi --" "$(DESTDIR)$(dvipsconfdir)/config.xdvi" >/dev/null 2>&1 \
	    || test ! -r "$(DESTDIR)$(dvipsconfdir)/config.xdvi"; then \
	  echo " $(INSTALL_DATA) '$(srcdir)/texmf/config.xdvi' '$(DESTDIR)$(dvipsconfdir)/config.xdvi'"; \
	  $(INSTALL_DATA) "$(srcdir)/texmf/config.xdvi" "$(DESTDIR)$(dvipsconfdir)/config.xdvi"; \
	else :; fi

uninstall-hook:
	@if grep "original config.xdvi --" "$(DESTDIR)$(dvipsconfdir)/config.xdvi" >/dev/null 2>&1 \
	    || test ! -r "$(DESTDIR)$(dvipsconfdir)/config.xdvi"; then \
	  echo " rm -f '$(DESTDIR)$(dvipsconfdir)/config.xdvi'"; \
	  rm -f "$(DESTDIR)$(dvipsconfdir)/config.xdvi"; \
	else :; fi

## Not used
##
EXTRA_DIST += \
	BUGS \
	CHANGES \
	FAQ \
	LESSTIF-BUGS \
	README.t1fonts \
	README.t1mapper \
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
	texmf-alt/README \
	texmf-alt/charter2.map \
	texmf-alt/lucida.map \
	texmf-alt/marvosym.map \
	texmf-alt/xdvifont.map \
	texmf-alt/xypic.map \
	xdvi-search.el \
	xdvi.icon \
	xdvizilla \
	xdvizilla.1

