
lib_LTLIBRARIES = libpaper.la
libpaper_la_SOURCES = dimen.c paper.c dimen.h paperspecs.h
libpaper_la_LDFLAGS = -version-info 2:2:1
libpaper_la_CPPFLAGS = -D_REENTRANT

BUILT_SOURCES = paperspecs.h

include_HEADERS = paper.h

EXTRA_DIST = paperspecs

paperspecs.h: paperspecs Makefile
	echo '    /* This file has been automaticaly generated.' >$@
	echo '       Edit paperspecs to make changes to the papers specifications. */' >>$@
	echo >>$@
	env LANG=C LC_ALL=C awk '{ factor = 1.0; \
	       if ($$4 == "mm") factor = 72.0 / 25.4; \
	       if ($$4 == "in") factor = 72.0; \
	       printf("    { \"%s\", %5f, %5f },\n", \
		      $$1, $$2 * factor, $$3 * factor); }' < $< >> $@
