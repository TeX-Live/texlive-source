dnl $Id$
dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright 2015-2020 Karl Berry <tex-live@tug.org>
dnl   Copyright 2012-2015 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define cairo_version
AC_INIT([cairo (TeX Live)], cairo_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([cairo-src/src/cairo.h])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIRS([../../m4 m4])

AC_USE_SYSTEM_EXTENSIONS

KPSE_BASIC([cairo])

AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_LN_S
CAIRO_BIGENDIAN

KPSE_COMPILER_VISIBILITY

cairo_attribute_flag=
if test "x$GCC" = xyes; then
  AS_CASE([`$CC -dumpversion`],
          [4.[[01]].* ], [],
          [4.* | 5.*], [cairo_attribute_flag=-Wno-attributes])
fi
AC_SUBST([CAIRO_ATTRIBUTE_FLAG], [$cairo_attribute_flag])

AC_CHECK_SIZEOF([void *])
AC_CHECK_SIZEOF([int])
AC_CHECK_SIZEOF([long])
AC_CHECK_SIZEOF([long long])
AC_CHECK_SIZEOF([size_t])

dnl Checks for precise integer types
AC_CHECK_HEADERS([sys/int_types.h])
AC_CHECK_TYPES([uint64_t, uint128_t, __uint128_t])

AC_CONFIG_HEADERS([config.h cairo-features.h])

AM_CONDITIONAL([build], [test "x$enable_build" != xno])
AC_SEARCH_LIBS([erf], [m])

CAIRO_FEATURES([dnl
[0, [fc font]],
[0, [ft font]],
[0, [quartz font]],
[1, [user font]],
[0, [win32 font]],
dnl
[0, [egl functions]],
[0, [glx functions]],
[0, [gobject functions]],
[0, [png functions]],
[0, [wgl functions]],
[0, [win32 functions]],
[0, [xcb shm functions]],
dnl
[1, [image surface]],
[1, [mime surface]],
[1, [observer surface]],
[1, [recording surface]],
[1, [pdf surface]],
[0, [ps surface]],
[0, [quartz surface]],
[0, [script surface]],
[0, [svg surface]],
[0, [win32 surface]],
[0, [xcb surface]],
[0, [xlib surface]],
[0, [xlib xrender surface]],
])

AM_CONDITIONAL([CAIRO_HAS_QUARTZ_IMAGE_SURFACE], [false])
AM_CONDITIONAL([CAIRO_HAS_OS2_SURFACE], [false])
AM_CONDITIONAL([CAIRO_HAS_GL_SURFACE], [false])

KPSE_PIXMAN_FLAGS

if test "x$enable_build" != xno || test -f config.force; then

KPSE_ADD_FLAGS([pixman])
AC_CHECK_FUNC([pixman_version_string], , [AC_ERROR([pixman not found])])
KPSE_RESTORE_FLAGS

echo timestamp >config.force
fi

AC_SUBST([CAIRO_TREE], [cairo-src])

AC_CONFIG_FILES([Makefile cairo/Makefile])

AC_OUTPUT
