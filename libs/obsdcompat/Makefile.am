## Makefile.am for the TeX Live subdirectory libs/obsdcompat/
##
## Copyright (C) 2009, 2012 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

# Rebuild
.PHONY: rebuild
rebuild: all

noinst_LIBRARIES = libopenbsd-compat.a

libopenbsd_compat_a_SOURCES = \
	defines.h \
	includes.h \
	openbsd-compat.h \
	strlcat.c \
	strlcpy.c \
	strsep.c \
	strtoll.c \
	strtonum.c \
	strtoul.c

