## Proxy Makefile.am to build libpng for TeX Live.
##
##   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
ACLOCAL_AMFLAGS = -I ../../m4

# Rebuild
.PHONY: rebuild
rebuild: all

## We want to re-distribute the whole original libpng source tree.
##
## With current automake (1.10.2) 'make distcheck' fails when
## DISTFILES contains a directory and files in that directory.
## Thus nodist_* for all files in $(GD_TREE).
EXTRA_DIST = $(GD_TREE)

## Changes applied to the original source tree
##
EXTRA_DIST +=  $(GD_TREE)-PATCHES

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn`
	rm -f $(distdir)/$(GD_TREE)/config/config.guess \
	  $(distdir)/$(GD_TREE)/config/config.rpath \
	  $(distdir)/$(GD_TREE)/config/config.sub \
	  $(distdir)/$(GD_TREE)/config/depcomp \
	  $(distdir)/$(GD_TREE)/config/install-sh \
	  $(distdir)/$(GD_TREE)/config/ltmain.sh \
	  $(distdir)/$(GD_TREE)/config/missing \
	  $(distdir)/$(GD_TREE)/config/mkinstalldirs \
	  $(distdir)/$(GD_TREE)/depcomp \
	  $(distdir)/$(GD_TREE)/install-sh \
	  $(distdir)/$(GD_TREE)/missing \
	  $(distdir)/$(GD_TREE)/mkinstalldirs

INCLUDES = -I$(top_srcdir)/$(GD_TREE) $(ZLIB_INCLUDES) $(LIBPNG_INCLUDES)

noinst_LIBRARIES=libgd.a

nodist_libgd_a_SOURCES = \
	$(GD_TREE)/gd.c \
	$(GD_TREE)/gd_gd.c \
	$(GD_TREE)/gd_gd2.c \
	$(GD_TREE)/gd_gif_in.c \
	$(GD_TREE)/gd_gif_out.c \
	$(GD_TREE)/gd_io.c \
	$(GD_TREE)/gd_io_dp.c \
	$(GD_TREE)/gd_io_file.c \
	$(GD_TREE)/gd_io_ss.c \
	$(GD_TREE)/gd_jpeg.c \
	$(GD_TREE)/gd_png.c \
	$(GD_TREE)/gd_security.c \
	$(GD_TREE)/gd_ss.c \
	$(GD_TREE)/gd_topal.c \
	$(GD_TREE)/gd_wbmp.c \
	$(GD_TREE)/gdcache.c \
	$(GD_TREE)/gdfontg.c \
	$(GD_TREE)/gdfontl.c \
	$(GD_TREE)/gdfontmb.c \
	$(GD_TREE)/gdfonts.c \
	$(GD_TREE)/gdfontt.c \
	$(GD_TREE)/gdft.c \
	$(GD_TREE)/gdfx.c \
	$(GD_TREE)/gdhelpers.c \
	$(GD_TREE)/gdkanji.c \
	$(GD_TREE)/gdtables.c \
	$(GD_TREE)/gdxpm.c \
	$(GD_TREE)/wbmp.c

BUILT_SOURCES = \
	entities.h \
	gd.h \
	gd_io.h \
	gdcache.h \
	gdfontg.h \
	gdfontl.h \
	gdfontmb.h \
	gdfonts.h \
	gdfontt.h \
	gdfx.h \
	gdhelpers.h \
	jisx0208.h \
	wbmp.h

$(libgd_a_OBJECTS): $(BUILT_SOURCES) config.force

config.force: $(ZLIB_DEPEND) $(LIBPNG_DEPEND)
	echo timestamp >config.force
	$(SHELL) ./config.status --recheck

DISTCLEANFILES = config.force

## Rebuild zlib
@ZLIB_RULE@
## Rebuild libpng
@LIBPNG_RULE@

$(BUILT_SOURCES): stamp-gd

stamp-gd:
	@for file in $(BUILT_SOURCES); do \
	  test -f $$file || { \
	    echo "$(LN_S) $(srcdir)/$(GD_TREE)/$$file $$file"; \
	    $(LN_S) $(srcdir)/$(GD_TREE)/$$file $$file; } || exit 1; \
	done
	echo timestamp >$@

distclean-local:
	rm -f stamp-gd $(BUILT_SOURCES)

