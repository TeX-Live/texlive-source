## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS = foreign

noinst_LIBRARIES = liblcdf.a

liblcdf_a_SOURCES = bezier.cc \
	clp.c \
	error.cc \
	filename.cc \
	globmatch.cc \
	landmark.cc \
	md5.c \
	permstr.cc \
	point.cc \
	slurper.cc \
	straccum.cc \
	string.cc \
	strtonum.c \
	transform.cc \
	vectorv.cc

if FIXLIBC
liblcdf_a_SOURCES += fixlibc.c
endif FIXLIBC

liblcdf_a_LIBADD = @TEMPLATE_OBJS@
CLEANFILES = @TEMPLATE_OBJS@

INCLUDES = -I$(srcdir)/../include

EXTRA_DIST = fixlibc.c
