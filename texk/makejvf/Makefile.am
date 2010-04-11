## Makefile.am for the TeX Live subdirectory texk/makejvfk/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

INCLUDES = $(KPATHSEA_INCLUDES) $(PTEXENC_INCLUDES)
AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = makejvf

makejvf_SOURCES = \
	main.c  makejvf.h  tfmread.c  tool.c  write.c

$(makejvf_OBJECTS): $(KPATHSEA_DEPEND) $(PTEXENC_DEPEND)

LDADD = $(PTEXENC_LIBS) $(KPATHSEA_LIBS)

## Rebuild libkpathsea & libptexenc
@PTEXENC_RULE@

EXTRA_DIST = COPYRIGHT COPYRIGHT.jis Changes.txt README.txt

