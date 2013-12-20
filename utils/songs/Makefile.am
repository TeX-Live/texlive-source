## Makefile.am for the TeX Live subdirectory utils/songs/
##
## Copyright (C) 2013 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##

## We want to re-distribute the whole original songs source tree.
EXTRA_DIST = $(SONGS_TREE)
## Patches applied to the original source tree
EXTRA_DIST += $(SONGS_TREE)-PATCHES

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn -o -name .deps -o -name .dirstamp -o -name '*.$(OBJEXT)'`

AM_CPPFLAGS = -I$(SONGS_TREE)/src/songidx
AM_CPPFLAGS += -D_ISOC99_SOURCE
AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = songidx

songidx_SOURCES = \
	@SONGS_TREE@/src/songidx/authidx.c \
	@SONGS_TREE@/src/songidx/fileio.c \
	@SONGS_TREE@/src/songidx/scripidx.c \
	@SONGS_TREE@/src/songidx/songidx.c \
	@SONGS_TREE@/src/songidx/songsort.c \
	@SONGS_TREE@/src/songidx/titleidx.c

## Data
##
songsdatadir = $(datarootdir)/texmf-dist/tex/latex/songs
dist_songsdata_DATA = songs.sty

all-local: songs.sty

songs.sty: @MAINTAINER_MODE_TRUE@ $(SONGS_TREE)/src/songs/songs.ins $(SONGS_TREE)/src/songs/songs.dtx
	TEXINPUTS=$(srcdir)/$(SONGS_TREE)/src/songs:$$TEXINPUTS $(LATEX) songs.ins

DISTCLEANFILES = songs.log

## Tests
##
TESTS = songidx.test
EXTRA_DIST += $(TESTS)

TESTS_ENVIRONMENT = SONGS_TREE=$(SONGS_TREE)

## songidx.test
EXTRA_DIST += tests/mybook.tex \
	tests/myauth.sbx tests/myauth.sxd \
	tests/myscrip.sbx tests/myscrip.sxd \
	tests/mytitle.sbx tests/mytitle.sxd
DISTCLEANFILES += tests/xmy*.sbx

