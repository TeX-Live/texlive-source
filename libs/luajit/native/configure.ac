dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2014 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([../version.ac])[] dnl define luajit_version
AC_INIT([luajit native], luajit_version, [tex-k@tug.org])
AC_PREREQ([2.63])
AC_CONFIG_SRCDIR([../LuaJIT-src/src/luajit.h])
AC_CONFIG_AUX_DIR([../../../build-aux])
AC_CONFIG_MACRO_DIR([../../../m4])

KPSE_BASIC([luajit native])

AC_PROG_CC
AC_PROG_LN_S

AC_CHECK_SIZEOF([void *])
AS_IF([test "x$ac_cv_sizeof_void_p" != "x$HOST_PTR_SIZE"], [dnl
HOST_PTR_BITS=`expr 8 \* $HOST_PTR_SIZE`
AC_MSG_WARN([pointer size mismatch in cross-build; use a ${HOST_PTR_BITS}bit compiler BULDCC.])
AS_CASE([$HOST_PTR_BITS],
        [32|64], [CC="$CC -m$HOST_PTR_BITS"; AC_MSG_WARN([trying: $CC])])])

KPSE_LUAJIT_DEFINES

KPSE_SEARCH_LIBS([MATH_LIB], [pow], [m])

AC_SUBST([DASM_ARCH])

AC_SUBST([LUAJIT_TREE], [../LuaJIT-src])

dnl Not used but avoids putting a zillion useless -D's on the command line.
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
