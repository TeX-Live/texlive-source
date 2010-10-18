## Makefile.am for the TeX Live subdirectory texk/musixflx/
##
## Copyright (C) 2009, 2010 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

## We want to re-distribute the whole original musixflx source tree.
##
EXTRA_DIST = $(MUSIXFLX_TREE)

## Patches applied to the original source tree
##
EXTRA_DIST += $(MUSIXFLX_TREE)-PATCHES

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn`

AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = musixflx

nodist_musixflx_SOURCES = \
	$(MUSIXFLX_TREE)/musixflx.c

nodist_man1_MANS = \
	$(MUSIXFLX_TREE)/musixflx.1

