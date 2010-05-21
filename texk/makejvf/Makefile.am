## Makefile.am for the TeX Live subdirectory texk/makejvfk/
##
## Copyright (C) 2010 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

INCLUDES = $(PTEXENC_INCLUDES) $(KPATHSEA_INCLUDES)
AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = makejvf

makejvf_SOURCES = \
	main.c  makejvf.h  tfmread.c  tool.c  write.c

$(makejvf_OBJECTS): $(KPATHSEA_DEPEND) $(PTEXENC_DEPEND)

LDADD = $(PTEXENC_LIBS) $(KPATHSEA_LIBS)

## Rebuild libkpathsea & libptexenc
@KPATHSEA_RULE@
@PTEXENC_RULE@

EXTRA_DIST = COPYRIGHT COPYRIGHT.jis Changes.txt README.txt

dist_check_SCRIPTS = makejvf.test

TESTS = makejvf.test

EXTRA_DIST += \
	tests/min10.tfm \
	tests/min10.vf \
	tests/rml.tfm \
	tests/rmlv.tfm \
	tests/tmin10.tfm \
	tests/tmin10.vf

DISTCLEANFILES = min10.vf rml.tfm rmlv.tfm tmin10.vf

## Eventually delete this file
##
EXTRA_DIST += \
	Makefile.orig

