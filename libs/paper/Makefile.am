## Proxy Makefile.am to build paper for TeX Live.
##
##   Copyright (C) 2013 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
## We want to re-distribute the whole original harfbuzz source tree.
##
EXTRA_DIST = $(PAPER_TREE)

## Changes applied to the original source tree
##
EXTRA_DIST += $(PAPER_TREE)-PATCHES

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn -o -name .deps -o -name .dirstamp -o -name '*.$(OBJEXT)'`

AM_CPPFLAGS = -I$(top_srcdir)/$(PAPER_TREE)/lib $(WARNING_CFLAGS)

SUBDIRS = . include

noinst_LIBRARIES = libpaper.a

libpaper_a_SOURCES = \
	@PAPER_TREE@/lib/dimen.c \
	@PAPER_TREE@/lib/dimen.h \
	@PAPER_TREE@/lib/paper.c \
	@PAPER_TREE@/lib/paper.h \
	@PAPER_TREE@/lib/paperspecs.h

LDADD = libpaper.a

bin_PROGRAMS = paperconf

paperconf_SOURCES = @PAPER_TREE@/src/paperconf.c

man_MANS = paperconf.1

## Tests
##
if build
dist_check_SCRIPTS = paper.test
TESTS = paper.test
endif build

# Rebuild
rebuild_prereq =
rebuild_target = all
CLEANFILES =

include $(srcdir)/../../am/rebuild.am

if MAINTAINER_MODE
BUILT_SOURCES = $(PAPER_TREE)/lib/paperspecs.h

paperspecs.h: paperspecs Makefile
	echo '    /* This file has been automaticaly generated.' >$@
	echo '       Edit paperspecs to make changes to the papers specifications. */' >>$@
	echo >>$@
	env LANG=C LC_ALL=C $(AWK) '{ factor = 1.0; \
	       if ($$4 == "mm") factor = 72.0 / 25.4; \
	       if ($$4 == "in") factor = 72.0; \
	       printf("    { \"%s\", %5f, %5f },\n", \
		      $$1, $$2 * factor, $$3 * factor); }' < $< >> $@
endif MAINTAINER_MODE

