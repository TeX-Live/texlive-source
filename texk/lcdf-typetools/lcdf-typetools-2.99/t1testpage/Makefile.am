## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = t1testpage
man_MANS = t1testpage.1

t1testpage_SOURCES = t1testpage.cc

t1testpage_LDADD = ../libefont/libefont.a ../liblcdf/liblcdf.a

AM_CPPFLAGS = -I$(srcdir)/../include
CLEANFILES = @TEMPLATE_OBJS@

EXTRA_DIST = t1testpage.1
