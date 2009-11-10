## Makefile.am for the TeX Live subdirectory texk/dviljk/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

INCLUDES = $(KPATHSEA_INCLUDES)
AM_CPPFLAGS = -DKPATHSEA
AM_CFLAGS = $(WARNING_CFLAGS)

noinst_LIBRARIES = libdvilj.a

libdvilj_a_SOURCES = \
	tfm.c \
	tfm.h

$(libdvilj_a_OBJECTS): $(KPATHSEA_DEPEND)

dist_bin_SCRIPTS = dvihp

bin_PROGRAMS = dvilj dvilj2p dvilj4 dvilj4l

dvilj_CPPFLAGS = -DKPATHSEA -DLJ

dvilj_SOURCES = \
	commands.h \
	config.h \
	dvi2xx.c \
	dvi2xx.h

$(dvilj_OBJECTS): libdvilj.a

dvilj2p_CPPFLAGS = -DKPATHSEA -DLJ2P

dvilj2p_SOURCES = \
	commands.h \
	config.h \
	dvi2xx.c \
	dvi2xx.h

$(dvilj2p_OBJECTS): libdvilj.a

dvilj4_CPPFLAGS = -DKPATHSEA -DLJ4

dvilj4_SOURCES = \
	commands.h \
	config.h \
	dvi2xx.c \
	dvi2xx.h

$(dvilj4_OBJECTS): libdvilj.a

dvilj4l_CPPFLAGS = -DKPATHSEA -DLJ4 -DLJ4L

dvilj4l_SOURCES = \
	commands.h \
	config.h \
	dvi2xx.c \
	dvi2xx.h

$(dvilj4l_OBJECTS): libdvilj.a

LDADD = libdvilj.a $(KPATHSEA_LIBS)	

## Rebuild libkpathsea
@KPATHSEA_RULE@

dist_man1_MANS = dvihp.man dvilj.man

man1_links = dvilj2p.1 dvilj4.1 dvilj4l.1 dvilj6.1

install-exec-hook:
	cd $(DESTDIR)$(bindir) && \
	  rm -f dvilj6$(EXEEXT) && \
	  $(LN_S) dvilj4$(EXEEXT) dvilj6$(EXEEXT)

install-data-hook:
	cd $(DESTDIR)$(man1dir) && \
	  for f in $(man1_links); do rm -f $$f; $(LN_S) dvilj.1 $$f; done

uninstall-hook:
	rm -f $(DESTDIR)$(bindir)/dvilj6$(EXEEXT)
	for f in $(man1_links); do rm -f $(DESTDIR)$(man1dir)/$$f; done

VERSION = `grep 'define VERSION' $(srcdir)/dvi2xx.c | sed -e 's/^.*version //' -e 's/).*//'`
if MAINTAINER_MODE
dvilj.man: dvilj.pod dvi2xx.c
	pod2man --center="TeX-Live" --release="Version $(VERSION)" $< >$@
endif MAINTAINER_MODE
dvilj.ps: dvilj.man
	man -t -l $< >$@

EXTRA_DIST = dvilj.pod

DISTCLEANFILES = dvilj.ps

