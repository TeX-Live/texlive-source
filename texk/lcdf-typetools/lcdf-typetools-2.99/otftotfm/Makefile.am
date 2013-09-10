## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = otftotfm
man_MANS = otftotfm.1

otftotfm_SOURCES = \
	automatic.cc automatic.hh \
	dvipsencoding.cc dvipsencoding.hh \
	glyphfilter.cc glyphfilter.hh \
	metrics.cc metrics.hh \
	otftotfm.cc otftotfm.hh \
	secondary.cc secondary.hh \
	uniprop.cc uniprop.hh \
	util.cc util.hh
EXTRA_otftotfm_SOURCES = kpseinterface.c kpseinterface.h

otftotfm_LDADD = ../libefont/libefont.a ../liblcdf/liblcdf.a
otftotfm_DEPENDENCIES = ../libefont/libefont.a ../liblcdf/liblcdf.a

AM_CPPFLAGS = -I$(srcdir)/../include
CLEANFILES = @TEMPLATE_OBJS@

EXTRA_DIST = otftotfm.1

if have_kpathsea
otftotfm_SOURCES += kpseinterface.c kpseinterface.h
AM_CPPFLAGS += $(KPATHSEA_INCLUDES)
otftotfm_LDADD += $(KPATHSEA_LIBS)
otftotfm_DEPENDENCIES += $(KPATHSEA_DEPEND)
endif have_kpathsea

## Rebuild libkpathsea in a TeX Live build
@KPATHSEA_RULE@
