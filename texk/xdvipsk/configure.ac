dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright 2016-2024 Karl Berry <tex-live@tug.org>
dnl   Copyright 2009-2015 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([dvipsk (TeX Live)], [2024.1], [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([xdvips.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

# check for C++ compiler and the library compiler
AC_PROG_CXX
AC_PROG_RANLIB

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([xdvipsk])
KPSE_CANONICAL_HOST
AC_CANONICAL_HOST
AM_CONDITIONAL([cross], [test "x$cross_compiling" = xyes])

ARCH_LIBS_DIR="win32"
AS_CASE([$host_os],
        [linux*],
        [
           AS_CASE([$host_cpu],
                   [x86_64], [ARCH_LIBS_DIR="x86_64-linux"],
                   [i?86], [ARCH_LIBS_DIR="i386-linux"])
        ],
        [darwin*],
        [
           AS_CASE([$host_cpu],
                   [x86_64], [ARCH_LIBS_DIR="x86_64-darwin"],
                   [i?86], [ARCH_LIBS_DIR="universal-darwin"])
        ],
)
AC_SUBST([ARCH_LIBS_DIR])

KPSE_LUA53_DEFINES

AC_CONFIG_HEADERS([c-auto.h:c-auto.in])

AC_CHECK_SIZEOF([int])

AC_SEARCH_LIBS([pow], [m])

m4_include([../../version.ac])[] dnl define tex_live_version
AC_DEFINE([TL_VERSION], ["TeX Live tex_live_version()"],
          [Define to the current TeX Live version string.])

KPSE_KPATHSEA_FLAGS
KPSE_LUA53_FLAGS
KPSE_ZLIB_FLAGS
KPSE_LIBPNG_FLAGS
KPSE_FREETYPE2_FLAGS

echo timestamp >config.force

# distribute additional compiler and linker flags
# --> set these variables instead of CXXFLAGS or LDFLAGS
AC_SUBST([AM_CXXFLAGS])
AC_SUBST([AM_LDFLAGS])
AC_SUBST([LIBS])

AC_CONFIG_FILES([Makefile])

# The subdirectory squeeze must be configured for the build system.
##KPSE_NATIVE_SUBDIRS([squeeze])
KPSE_NATIVE_SUBDIRS()

AC_CHECK_LIB(stdc++, main,,AC_MSG_ERROR(xdvipsk requires libstdc++))

AC_OUTPUT
