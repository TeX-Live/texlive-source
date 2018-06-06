dnl $Id$
dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright 2016-2017 Karl Berry <tex-live@tug.org>
dnl   Copyright 2012-2015 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define harfbuzz_version
AC_INIT([harfbuzz (TeX Live)], harfbuzz_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([harfbuzz-src/src/harfbuzz.pc.in])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

KPSE_BASIC([harfbuzz], no-define)

KPSE_LIB_VERSION([harfbuzz])[]dnl
AC_SUBST([HB_VERSION_MAJOR], [harfbuzz_major])[]dnl
AC_SUBST([HB_VERSION_MINOR], [harfbuzz_minor])[]dnl
AC_SUBST([HB_VERSION_MICRO], [harfbuzz_micro])[]dnl
AC_SUBST([HB_VERSION], [harfbuzz_version])[]dnl

AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB
AC_PROG_LN_S

# Theoretically harfbuzz does not require C++11?
AX_CXX_COMPILE_STDCXX([11],,[optional])

KPSE_COMPILER_VISIBILITY

KPSE_GRAPHITE2_FLAGS
KPSE_ICU_FLAGS

# Functions and headers
AC_CHECK_FUNCS(atexit mprotect sysconf getpagesize mmap isatty newlocale strtod_l)

save_libs="$LIBS"
LIBS="$LIBS -lm"
AC_CHECK_FUNCS([round], ,[AC_CHECK_DECLS([round], , ,[#include <math.h>])])
LIBS="$save_libs"

AC_CHECK_HEADERS(unistd.h sys/mman.h xlocale.h stdbool.h)

# Compiler flags
AC_CANONICAL_HOST
AC_CHECK_ALIGNOF([struct{char;}])
if test "x$GCC" = "xyes"; then

	# Make symbols link locally
	LDFLAGS="$LDFLAGS -Bsymbolic-functions"

	# Make sure we don't link to libstdc++
	CXXFLAGS="$CXXFLAGS -fno-rtti -fno-exceptions"

	# Assorted warnings
	CXXFLAGS="$CXXFLAGS -Wcast-align"

	case "$host" in
		arm-*-*)
			if test "x$ac_cv_alignof_struct_char__" != x1; then
				# Request byte alignment
				CXXFLAGS="$CXXFLAGS -mstructure-size-boundary=8"
			fi
		;;
	esac
fi
AM_CONDITIONAL(HAVE_GCC, test "x$GCC" = "xyes")

AC_SUBST([AM_CXXFLAGS])

AC_CONFIG_HEADERS([config.h])

AM_CONDITIONAL([build], [test "x$enable_build" != xno])

if test "x$enable_build" != xno || test -f config.force; then

echo timestamp >config.force
fi

if :; then
  AC_DEFINE([HAVE_OT], 1, [Use native OpenType Layout backend])
  AC_DEFINE([HAVE_ICU], 1, [Use ICU library])
  AC_DEFINE([HAVE_GRAPHITE2], 1, [Use Graphite library])
else
dnl add all these to config.h.in but do not define them in config.h
  AC_DEFINE([HAVE_PTHREAD], 1, [Use POSIX threads])
  AC_DEFINE([HAVE_UCDN], 1, [Use UCDN Unicode functions])
  AC_DEFINE([HAVE_GLIB], 1, [Use glib2 library])
  AC_DEFINE([HAVE_GTHREAD], 1, [Use gthread2 library])
  AC_DEFINE([HAVE_GOBJECT], 1, [Use gobject2 library])
  AC_DEFINE([HAVE_CAIRO], 1, [Use cairo graphics library])
  AC_DEFINE([HAVE_CAIRO_FT], 1, [Use cairo-ft support in cairo graphics library])
  AC_DEFINE([HAVE_UNISCRIBE], 1, [Use Uniscribe backend])
  AC_DEFINE([HAVE_CORETEXT], 1, [Use Core Text backend])
  AC_DEFINE([HAVE_INTEL_ATOMIC_PRIMITIVES], 1, [Use Intel __sync_* atomic primitives])
fi

AC_SUBST([HARFBUZZ_TREE], [harfbuzz-src])

if test -f $srcdir/$HARFBUZZ_TREE/src/hb-version.h; then
  AC_MSG_ERROR([Sorry, you must remove the file $HARFBUZZ_TREE/src/hb-version.h])
fi

AC_CONFIG_FILES([Makefile
		 include/Makefile
	         hb-version.h:harfbuzz-src/src/hb-version.h.in])

AC_OUTPUT
