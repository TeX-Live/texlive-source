##
ACLOCAL_AMFLAGS = -I ../../m4

INCLUDES = $(KPATHSEA_INCLUDES)
AM_CPPFLAGS = -DUNIX -DKPATHSEA

LDADD = $(KPATHSEA_LIBS)

bin_PROGRAMS = disdvi dvi2tty

disdvi_SOURCES = \
	commands.h \
	disdvi.c

disdvi_DEPENDENCIES = $(KPATHSEA_DEPEND)

dvi2tty_SOURCES = \
	commands.h \
	dvi2tty.c \
	dvi2tty.h \
	dvistuff.c
	
dvi2tty_DEPENDENCIES = $(KPATHSEA_DEPEND)

dist_man1_MANS = dvi2tty.1

## ##################
## Not yet used

EXTRA_DIST = \
	DVI.format \
	MANIFEST \
	README.ORG \
	dvi2tty.patch

