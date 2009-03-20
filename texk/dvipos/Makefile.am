##
ACLOCAL_AMFLAGS = -I ../../m4

INCLUDES = $(KPATHSEA_INCLUDES)
AM_CPPFLAGS = -DKPATHSEA

bin_PROGRAMS = dvipos

dvipos_SOURCES = \
	dvicore.c \
	dvicore.h \
	dvipos.c \
	tfm.c \
	tfm.h \
	utils.c \
	utils.h

dvipos_DEPENDENCIES = $(KPATHSEA_DEPEND)

LDADD = $(KPATHSEA_LIBS)

## Rebuild libkpathsea
@KPATHSEA_RULE@

dist_man1_MANS = dvipos.1

## Not used
##
EXTRA_DIST = miktex.mak

