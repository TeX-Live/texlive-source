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

AC_SEARCH_LIBS([sqrt], [m])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
