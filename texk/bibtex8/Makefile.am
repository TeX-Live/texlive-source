##
ACLOCAL_AMFLAGS = -I ../../m4

INCLUDES = $(KPATHSEA_INCLUDES)
AM_CPPFLAGS = -DUNIX -DKPATHSEA

bin_PROGRAMS = bibtex8

bibtex8_SOURCES = \
	bibtex-1.c \
	bibtex-2.c \
	bibtex-3.c \
	bibtex-4.c \
	bibtex.c \
	bibtex.h \
	datatype.h \
	gblprocs.h \
	gblvars.h \
	utils.c \
	utils.h \
	version.h \
	sysdep.h

bibtex8_DEPENDENCIES = $(KPATHSEA_DEPEND)

LDADD = $(KPATHSEA_LIBS)	

## Rebuild libkpathsea
@KPATHSEA_RULE@

## Not (yet) used
##
EXTRA_DIST = \
	00readme.txt \
	HISTORY \
	bt371csf.zip \
	csfile.txt \
	dos-dj.mak \
	dos-emx.mak \
	file_id.diz \
	getopt.c \
	getopt.h \
	getopt1.c \
	os2.mak \
	unix.mak \
	csf/00readme.txt \
	csf/88591lat.csf \
	csf/88591sca.csf \
	csf/COPYING \
	csf/HISTORY \
	csf/ascii.csf \
	csf/cp437lat.csf \
	csf/cp850lat.csf \
	csf/cp850sca.csf \
	csf/cp866rus.csf \
	csf/csfile.txt \
	csf/file_id.diz

## Eventually delete these files
##

EXTRA_DIST += Makefile.in.orig configure.in.orig

