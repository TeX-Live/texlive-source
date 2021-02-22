## Makefile.am for the TeX Live subdirectory texk/makejvfk/
##
## Copyright (C) 2010-2015 Peter Breitenlohner <tex-live@tug.org>
## Copyright (C) 2016-2018 Japanese TeX Development Community <issue@texjp.org>
## You may freely use, modify and/or distribute this file.
##
AM_CPPFLAGS = $(PTEXENC_INCLUDES) $(KPATHSEA_INCLUDES)
AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = makejvf

makejvf_SOURCES = \
	main.c makejvf.h tfmread.c tool.c \
	uniblock.c uniblock.h usrtable.c usrtable.h version.h write.c

$(makejvf_OBJECTS): $(KPATHSEA_DEPEND) $(PTEXENC_DEPEND)

LDADD = $(PTEXENC_LIBS) $(KPATHSEA_LIBS)

## Rebuild libkpathsea & libptexenc
@KPATHSEA_RULE@
@PTEXENC_RULE@

EXTRA_DIST = COPYRIGHT COPYRIGHT.jis Changes.txt README.txt

dist_check_SCRIPTS = makejvf.test

TESTS = makejvf.test
makejvf.log: makejvf$(EXEEXT)

dist_man1_MANS = makejvf.1

EXTRA_DIST += \
	tests/min10.tfm \
	tests/min10.vf \
	tests/rml.tfm \
	tests/rmlv.tfm \
	tests/tmin10.tfm \
	tests/tmin10.vf \
	tests/testnewu.tfm \
	tests/testnewu.vf \
	tests/gkhugeok.tfm \
	tests/gkhugeok.vf \
	tests/skipjfmp.tfm \
	tests/skipjfmp.vf \
	tests/skipjfmp_omit.vf

DISTCLEANFILES = min10.vf rml.tfm rmlv.tfm tmin10.vf testnewu.vf \
	gkhugeok.vf skipjfmp.vf urml.tfm

