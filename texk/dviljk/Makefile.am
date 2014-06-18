## Makefile.am for the TeX Live subdirectory texk/dviljk/
##
## Copyright (C) 2009-2014 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
AM_CPPFLAGS = $(KPATHSEA_INCLUDES) -DKPATHSEA
AM_CFLAGS = $(WARNING_CFLAGS)

noinst_LIBRARIES = libdvilj.a

libdvilj_a_SOURCES = \
	tfm.c \
	tfm.h

$(libdvilj_a_OBJECTS): $(KPATHSEA_DEPEND)

bin_PROGRAMS = dvilj dvilj2p dvilj4 dvilj4l

if WIN32
SUBDIRS = win32
bin_PROGRAMS += dvilj6
else !WIN32
dist_bin_SCRIPTS = dvihp
endif !WIN32

dvi2xx_sources = \
	commands.h \
	config.h \
	dvi2xx.c \
	dvi2xx.h

dvilj_CPPFLAGS = $(AM_CPPFLAGS) -DLJ
dvilj_SOURCES = $(dvi2xx_sources)
$(dvilj_OBJECTS): libdvilj.a

dvilj2p_CPPFLAGS = $(AM_CPPFLAGS) -DLJ2P
dvilj2p_SOURCES = $(dvi2xx_sources)
$(dvilj2p_OBJECTS): libdvilj.a

dvilj4_CPPFLAGS = $(AM_CPPFLAGS) -DLJ4
dvilj4_SOURCES = $(dvi2xx_sources)
$(dvilj4_OBJECTS): libdvilj.a

dvilj4l_CPPFLAGS = $(AM_CPPFLAGS) -DLJ4 -DLJ4L
dvilj4l_SOURCES = $(dvi2xx_sources)
$(dvilj4l_OBJECTS): libdvilj.a

LDADD = libdvilj.a $(KPATHSEA_LIBS)	

dvilj6_CPPFLAGS = -DEXEPROG=\"dvilj4.exe\"
nodist_dvilj6_SOURCES = callexe.c
dvilj6_LDADD =

## Rebuild libkpathsea
@KPATHSEA_RULE@

bin_links = dvilj4$(EXEEXT):dvilj6

include $(top_srcdir)/../../am/bin_links.am

dist_man1_MANS = dvihp.man dvilj.man

man1_links = \
	dvilj:dvilj2p \
	dvilj:dvilj4 \
	dvilj:dvilj4l \
	dvilj:dvilj6

include $(top_srcdir)/../../am/man1_links.am

install-exec-hook: install-bin-links

install-data-hook: install-man1-links

uninstall-hook: uninstall-bin-links uninstall-man1-links

VERSION = `grep 'define VERSION' $(srcdir)/dvi2xx.c | sed -e 's/^.*version //' -e 's/).*//'`
if MAINTAINER_MODE
dvilj.man: dvilj.pod dvi2xx.c
	pod2man --center="TeX-Live" --release="Version $(VERSION)" $< >$@
endif MAINTAINER_MODE
dvilj.ps: dvilj.man
	man -t -l $< >$@

EXTRA_DIST = dvilj.pod

DISTCLEANFILES = dvilj.ps

