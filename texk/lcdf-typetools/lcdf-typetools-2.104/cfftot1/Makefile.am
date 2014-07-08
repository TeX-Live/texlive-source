## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = cfftot1
man_MANS = cfftot1.1

cfftot1_SOURCES = cfftot1.cc \
	maket1font.cc maket1font.hh

cfftot1_LDADD = ../libefont/libefont.a ../liblcdf/liblcdf.a

AM_CPPFLAGS = -I$(srcdir)/../include
CLEANFILES = @TEMPLATE_OBJS@

EXTRA_DIST = cfftot1.1
