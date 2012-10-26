## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = t1dotlessj
man_MANS = t1dotlessj.1

t1dotlessj_SOURCES = t1dotlessj.cc

t1dotlessj_LDADD = ../libefont/libefont.a ../liblcdf/liblcdf.a

INCLUDES = -I$(srcdir)/../include
CLEANFILES = @TEMPLATE_OBJS@

EXTRA_DIST = t1dotlessj.1
