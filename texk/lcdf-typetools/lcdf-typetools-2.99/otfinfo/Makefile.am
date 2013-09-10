## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = otfinfo
man_MANS = otfinfo.1

otfinfo_SOURCES = \
	otfinfo.cc

otfinfo_LDADD = ../libefont/libefont.a ../liblcdf/liblcdf.a
otfinfo_DEPENDENCIES = ../libefont/libefont.a ../liblcdf/liblcdf.a

AM_CPPFLAGS = -I$(srcdir)/../include
CLEANFILES = @TEMPLATE_OBJS@

EXTRA_DIST = otfinfo.1
