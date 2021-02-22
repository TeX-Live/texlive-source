dnl $Id$
dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright 2021 Bob Tennent <rdt@queensu.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_define([xml2pmx_version], [2021-02-07])[]dnl using unmodified xml2pmx source
AC_INIT([xml2pmx (TeX Live)], xml2pmx_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([xml2pmx-src/obx.h])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])
AC_DEFINE(REVID, "xml2pmx", [Build id for runtime])

KPSE_BASIC([xml2pmx])
AC_CONFIG_HEADERS([config.h])[]

# Copied from xml2pmx-src/configure.ac.
# Checks for programs.
AC_PROG_CC
AC_C_CONST
AC_C_BIGENDIAN
AC_C_INLINE
AC_UNISTD_H

# defined in acinclude.m4.
AC_C_UNUSED
AC_C_USED
AC_C_NORETURN

AC_DEFINE(PAGESIZE, 4096, [Notional page size for garbage collector])
AC_DEFINE(LOG_PAGESIZE, 12, [log2 of PAGESIZE])

# M64X32 allows Keiko programs (with 4-byte pointers) to run on machines
# with a 64-bit address space.
AC_CHECK_SIZEOF([int *])
if test $ac_cv_sizeof_int_p = 8; then
  AC_DEFINE(M64X32, 1, [Define for 64-bit runtime with 32-bit pointers])
fi

# Indexed jumps allow the interpreter to run faster, but it works
# without them.
# AC_C_INDEXED_JUMPS

# SEGMEM allows malloc to be used for all dynamic storage allocation
AC_DEFINE(SEGMEM, 1, [Define to simulate segmented memory])

AC_DEFINE(PRELOAD, 1, [Define to use preloaded image])
AC_DEFINE(MAGIC, "OBCX", [Magic number for executables])
AC_DEFINE(SIG, 0x00030290, [OBC version signature])
AC_DEFINE(DEBUG, 1, [Debug])

# trailing tl stuff.
AC_PROG_MAKE_SET
KPSE_COND_WIN32_WRAP
AC_SUBST([XML2PMX_TREE], [xml2pmx-src])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
