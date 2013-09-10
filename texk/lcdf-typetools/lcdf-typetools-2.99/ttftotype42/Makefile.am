## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = ttftotype42
man_MANS = ttftotype42.1

ttftotype42_SOURCES = ttftotype42.cc

ttftotype42_LDADD = ../libefont/libefont.a ../liblcdf/liblcdf.a

AM_CPPFLAGS = -I$(srcdir)/../include
CLEANFILES = @TEMPLATE_OBJS@

EXTRA_DIST = ttftotype42.1
