## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = t1lint
man_MANS = t1lint.1

t1lint_SOURCES = cscheck.hh cscheck.cc \
	t1lint.cc

t1lint_LDADD = ../libefont/libefont.a ../liblcdf/liblcdf.a

INCLUDES = -I$(srcdir)/../include
CLEANFILES = @TEMPLATE_OBJS@

EXTRA_DIST = t1lint.1
