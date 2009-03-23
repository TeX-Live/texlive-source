##
ACLOCAL_AMFLAGS = -I ../../m4

noinst_LIBRARIES = libopenbsd-compat.a

compat_sources = \
	bsd-asprintf.c \
	bsd-snprintf.c

openbsd_sources = \
	strlcat.c \
	strlcpy.c \
	strsep.c \
	strtoll.c \
	strtonum.c \
	strtoul.c

libopenbsd_compat_a_SOURCES = \
	$(compat_sources) \
	$(openbsd_sources) \
	defines.h \
	includes.h \
	openbsd-compat.h

## Eventually delete these files
##
EXTRA_DIST = libobsd-compat.ac

