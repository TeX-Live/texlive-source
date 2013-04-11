## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = t1rawafm
man_MANS = t1rawafm.1

t1rawafm_SOURCES = t1rawafm.cc

t1rawafm_LDADD = ../libefont/libefont.a ../liblcdf/liblcdf.a

INCLUDES = -I$(srcdir)/../include
CLEANFILES = @TEMPLATE_OBJS@

EXTRA_DIST = t1rawafm.1
