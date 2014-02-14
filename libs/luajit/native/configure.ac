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
AC_CONFIG_SRCDIR([../LuaJIT-]luajit_version[/src/luajit.h])
AC_CONFIG_AUX_DIR([../../../build-aux])
AC_CONFIG_MACRO_DIR([../../../m4])

KPSE_BASIC([luajit native])

AC_PROG_CC
AC_PROG_LN_S

KPSE_LUAJIT_DEFINES

lj_save_LIBS=$LIBS
AC_SEARCH_LIBS([pow], [m])
LIBS=$lj_save_LIBS
AS_CASE([$ac_cv_search_pow],
        ["none required"], [],
        [no], [AC_MSG_ERROR([Sorry, did not find pow()])],
        [MATH_LIB=$ac_cv_search_pow])
AC_SUBST([MATH_LIB])

AC_SUBST([DASM_ARCH])

AC_SUBST([LUAJIT_TREE], [../LuaJIT-]luajit_version)

dnl Not used but avoids putting a zillion useless -D's on the command line.
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
