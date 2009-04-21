## Makefile.am for the TeX Live subdirectory texk/xdvik/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4 -I m4

SUBDIRS = squeeze gui . tests

include common.am

INCLUDES = -I$(srcdir)/gui $(common_includes)

noinst_HEADERS = \
	alloc-debug.h \
	browser.h \
	c-openmx.h \
	dl_list.h \
	dvi-draw.h \
	dvi-init.h \
	dvi.h \
	dvisel.h \
	encodings.h \
	events.h \
	exit-handlers.h \
	filehist.h \
	font-open.h \
	hypertex.h \
	image-magick.h \
	mime.h \
	my-snprintf.h \
	my-vsnprintf.h \
	pagehist.h \
	print-internal.h \
	psdps.h \
	psgs.h \
	psnews.h \
	read-mapfile.h \
	search-internal.h \
	special.h \
	string-utils.h \
	string_list.h \
	tfmload.h \
	translations.h \
	util.h \
	version.h \
	x_util.h \
	xdvi-config.h \
	xdvi-debug.h \
	xdvi.h \
	xserver-info.h

noinst_LIBRARIES = libxdvi.a

nodist_libxdvi_a_SOURCES = psheader.c

libxdvi_a_SOURCES = \
	browser.c \
	dl_list.c \
	dvi-draw.c \
	dvi-init.c \
	dvisel.c \
	encodings.c \
	events.c \
	exit-handlers.c \
	filehist.c \
	font-open.c \
	gf.c \
	hypertex.c \
	image-magick.c \
	mime.c \
	my-snprintf.c \
	my-vsnprintf.c \
	pagehist.c \
	pk.c \
	psdps.c \
	psgs.c \
	psnews.c \
	read-mapfile.c \
	search-internal.c \
	special.c \
	string-utils.c \
	string_list.c \
	tfmload.c \
	util.c \
	vf.c \
	x_util.c \
	xdvi.c \
	xserver-info.c

bin_SCRIPTS = xdvi

bin_PROGRAMS = xdvi-bin

xdvi_bin_SOURCES = main.c

xdvi_bin_DEPENDENCIES = libxdvi.a gui/libgui.a libxdvi.a $(common_dependencies)

LDADD = libxdvi.a gui/libgui.a $(common_ldadd)

gui/libgui.a: $(libgui_dependencies)
	cd gui && $(MAKE) $(AM_MAKEFLAGS) libgui.a

psheader.c: psheader.txt squeeze/squeeze$(EXEEXT)
	squeeze/squeeze $(srcdir)/psheader.txt $@

squeeze/squeeze$(EXEEXT):
	cd squeeze && $(MAKE) $(AM_MAKEFLAGS)

EXTRA_DIST = psheader.txt
CLEANFILES = psheader.c

nodist_man1_MANS = xdvi.1

xdvi.1: xdvi.1.in sedscript
	sed -f sedscript <$(srcdir)/xdvi.1.in >$@

sedscript: mksedscript CHANGES Makefile c-auto.h
	$(SHELL) $(srcdir)/mksedscript $(srcdir) $(DEFS) $(PS_DEF) >$@

EXTRA_DIST += mksedscript xdvi.1.in
CLEANFILES += sedscript xdvi.1

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
	m4/acinclude.m4 \
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
	texmf-alt/xdvi.cfg \
	texmf-alt/xdvifont.map \
	texmf-alt/xypic.map \
	xdvi-search.el \
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
	Makefile.in.work \
	configure.in.work

