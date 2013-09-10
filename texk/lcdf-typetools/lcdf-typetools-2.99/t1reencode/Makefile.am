## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = t1reencode
man_MANS = t1reencode.1

t1reencode_SOURCES = t1reencode.cc \
	util.cc util.hh

t1reencode_LDADD = ../libefont/libefont.a ../liblcdf/liblcdf.a

AM_CPPFLAGS = -I$(srcdir)/../include
CLEANFILES = @TEMPLATE_OBJS@

EXTRA_DIST = t1reencode.1
