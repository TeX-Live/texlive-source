##
ACLOCAL_AMFLAGS = -I ../../m4

INCLUDES = $(KPATHSEA_INCLUDES) $(LIBPNG_INCLUDES) $(ZLIB_INCLUDES)
AM_CPPFLAGS = -DHAVE_KPATHSEA

## FIXME: don't install ebb (use compatible version in dvipdfmx)

bin_PROGRAMS = dvipdfm ebb

dvipdfm_SOURCES = \
	colors.h \
	colorsp.c \
	colorsp.h \
	dvi.c \
	dvi.h \
	dvicodes.h \
	dvipdfm.c \
	dvipdfm.h \
	encodings.c \
	encodings.h \
	error.h \
	epdf.c \
	epdf.h \
	htex.c \
	htex.h \
	jpeg.c \
	jpeg.h \
	macglyphs.h \
	mem.c \
	mem.h \
	mfileio.c \
	mfileio.h \
	mpost.c \
	mpost.h \
	numbers.c \
	numbers.h \
	pdfdev.c \
	pdfdev.h \
	pdfdoc.c \
	pdfdoc.h \
	pdflimits.h \
	pdfobj.c \
	pdfobj.h \
	pdfparse.c \
	pdfparse.h \
	pdfspecial.c \
	pdfspecial.h \
	pkfont.c \
	pkfont.h \
	pngimage.c \
	pngimage.h \
	psimage.c \
	psimage.h \
	psspecial.c \
	psspecial.h \
	standardenc.h \
	system.h \
	t1crypt.c \
	t1crypt.h \
	tfm.c \
	tfm.h \
	thumbnail.c \
	thumbnail.h \
	tpic.c \
	tpic.h \
	ttf.h \
	twiddle.h \
	type1.c \
	type1.h \
	vf.c \
	vf.h \
	winansi.h

if HAVE_TTF
dvipdfm_SOURCES += ttf.c
endif HAVE_TTF

dvipdfm_DEPENDENCIES = $(KPATHSEA_DEPEND) $(LIBPNG_DEPEND) $(ZLIB_DEPEND)

ebb_SOURCES = \
	ebb.c \
	error.h \
	jpeg.c \
	jpeg.h \
	macglyphs.h \
	mem.c \
	mem.h \
	mfileio.c \
	mfileio.h \
	numbers.c \
	numbers.h \
	pdflimits.h \
	pdfobj.c \
	pdfobj.h \
	pdfparse.c \
	pdfparse.h \
	pngimage.c \
	pngimage.h \
	system.h \
	twiddle.h

ebb_DEPENDENCIES = $(KPATHSEA_DEPEND)

LDADD = $(KPATHSEA_LIBS) $(LIBPNG_LIBS) $(ZLIB_LIBS)

## Rebuild libkpathsea
@KPATHSEA_RULE@
## Rebuild libpng
@LIBPNG_RULE@
## Rebuild libz
@ZLIB_RULE@

dist_bin_SCRIPTS = dvipdft

dist_man1_MANS = dvipdfm.1 dvipdft.1 ebb.1

EXTRA_DIST = \
	ChangeLog.TL \
	Credits \
	INSTALL-BINARY \
	Makefile.in.dist \
	OBTAINING \
	README.FIRST \
	acconfig.h.dist \
	acconfig.h.in \
	acinclude.m4.orig \
	ascii85.c \
	cmr.map \
	config \
	config-dist \
	t1fonts.map \
	version \
	doc/dvipdfm.tex \
	doc/mwicks.bb \
	doc/mwicks.jpeg \
	doc/transistor.bb \
	doc/transistor.eps \
	doc/transistor.fig \
	doc/transistor.pdf

## Eventually delete these files
##
EXTRA_DIST += Makefile.in.orig configure.in.orig

