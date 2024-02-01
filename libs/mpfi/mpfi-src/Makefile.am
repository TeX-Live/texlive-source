# Copyright 2000-2020 Free Software Foundation, Inc.
# This Makefile.am is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.


# Since we do not require GNU "make", let's select the POSIX behavior to
# uniformize the behavior a bit more with various "make" implementations
# and ease maintenance. This target should be removed only if one day,
# we need to require GNU "make".
.POSIX:

AUTOMAKE_OPTIONS = gnu

# ACLOCAL_AMFLAGS will be fully deprecated in Automake 2.0;
# AC_CONFIG_MACRO_DIRS (introduced in Automake 1.13) is now used instead,
# but we still set ACLOCAL_AMFLAGS to avoid a warning message from
# libtoolize and in case some developer needs to switch back to an
# old Automake version.
#ACLOCAL_AMFLAGS = -I m4
ACLOCAL_AMFLAGS = 

include_HEADERS = src/mpfi.h src/mpfi_io.h
EXTRA_DIST = mpfi_config.h.in

SUBDIRS = doc src tests 

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = mpfi.pc

nobase_dist_doc_DATA = AUTHORS COPYING COPYING.LESSER NEWS TODO 

# Various checks for "make dist" / "make distcheck".
# Warning! With "make distcheck", neither the top-level directory nor
# the tools directory is writable.
# no checks for the time being...

.PHONY: 
