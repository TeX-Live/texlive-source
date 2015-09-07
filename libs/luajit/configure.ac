dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2014, 2015 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define luajit_version
AC_INIT([luajit for TeX Live], luajit_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([LuaJIT-]luajit_version[/src/luajit.h])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIRS([../../m4 m4])

KPSE_BASIC([luajit])

KPSE_CANONICAL_HOST

AC_PROG_CC
AC_PROG_CC_C99
AM_PROG_AS
AC_PROG_RANLIB
AC_PROG_LN_S

AC_SUBST([LUAJITVERSION], [luajit_version])
KPSE_LT_VERSION([luajit])

LT_INIT([win32-dll])

KPSE_DLL_NAME([texluajit], [texluajit])

AC_CHECK_SIZEOF([void *])
ac_configure_args="$ac_configure_args HOST_PTR_SIZE='$ac_cv_sizeof_void_p'"

AC_SEARCH_LIBS([sqrt], [m])
AC_SEARCH_LIBS([dlopen], [dl])

KPSE_LUAJIT_DEFINES
KPSE_LARGEFILE([LUAJIT_DEFINES])

AS_CASE([$host_os],
        [cygwin* | mingw* | msys*], [LJHOST='Windows'],
        [darwin*], [LJHOST='Darwin'],
        [solaris* | sunos*], [LJHOST='SunOS'],
        [linux*], [LJHOST='Linux'],
                  [LJHOST='Other'])
LJ_ARCH()
AS_CASE([$LJHOST],
        [Windows], [LJVM_MODE=peobj
                    LUAJIT_CFLAGS=-malign-double],
        [Darwin | iOS], [LJVM_MODE=machasm],
                        [LJVM_MODE=elfasm])
AM_CONDITIONAL([PEOBJ], [test "x$LJVM_MODE" = xpeobj])

AC_SUBST([LUAJIT_CFLAGS])
AC_SUBST([LJHOST])
AC_SUBST([LJVM_MODE])

AS_CASE([$host_os:$host_cpu],
        [*darwin*:x86_64], [LIBLUAJIT_LDEXTRA='-image_base 7fff04c4a000'])
AC_SUBST([LIBLUAJIT_LDEXTRA])

AM_CONDITIONAL([build], [test "x$enable_build" != xno])

if test "x$enable_build" != xno || test -f config.force; then

AS_IF([test "x$lj_cpp" != xok],
      [AC_MSG_ERROR([Sorry, can not preprocess <lj_arch.h>])])

echo timestamp >config.force
fi

AC_SUBST([LUAJIT_TREE], [LuaJIT-]luajit_version)

dnl Not used but avoids putting a zillion useless -D's on the command line.
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_FILES([Makefile include/Makefile texluajit.pc])

dnl Pass DASM_ARCH to the subdirectory native,
dnl which must be configured for the build system.
AC_CONFIG_COMMANDS_POST([ac_configure_args="$ac_configure_args DASM_ARCH=$DASM_ARCH"])
KPSE_NATIVE_SUBDIRS([native])

AC_OUTPUT
