## Makefile.am for the TeX Live subdirectory texk/dviljk/
##
## Copyright (C) 2009 - 2011 Peter Breitenlohner <tex-live@tug.org>
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

bin_PROGRAMS = dvilj dvilj2p dvilj4 dvilj4l

if WIN32
bin_PROGRAMS += dvilj6
else !WIN32
dist_bin_SCRIPTS = dvihp
endif !WIN32

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

dvilj6_CPPFLAGS = -DEXEPROG=\"dvilj4.exe\"

dvilj6_SOURCES = $(srcdir)/../texlive/w32_wrapper/callexe.c

dvilj6_LDADD =

## Rebuild libkpathsea
@KPATHSEA_RULE@

dist_man1_MANS = dvihp.man dvilj.man

# Symlinks within $(man1dir): FILE:LINK indicates LINK.1->FILE.1
man1_links = \
	dvilj:dvilj2p \
	dvilj:dvilj4 \
	dvilj:dvilj4l \
	dvilj:dvilj6

include $(top_srcdir)/../am/man1_links.am

install-exec-hook:
if !WIN32
	cd $(DESTDIR)$(bindir) && \
	  rm -f dvilj6$(EXEEXT) && \
	  $(LN_S) dvilj4$(EXEEXT) dvilj6$(EXEEXT)
endif !WIN32

install-data-hook: install-man1-links

uninstall-hook: uninstall-man1-links
if !WIN32
	rm -f $(DESTDIR)$(bindir)/dvilj6$(EXEEXT)
endif !WIN32

VERSION = `grep 'define VERSION' $(srcdir)/dvi2xx.c | sed -e 's/^.*version //' -e 's/).*//'`
if MAINTAINER_MODE
dvilj.man: dvilj.pod dvi2xx.c
	pod2man --center="TeX-Live" --release="Version $(VERSION)" $< >$@
endif MAINTAINER_MODE
dvilj.ps: dvilj.man
	man -t -l $< >$@

EXTRA_DIST = dvilj.pod

DISTCLEANFILES = dvilj.ps

