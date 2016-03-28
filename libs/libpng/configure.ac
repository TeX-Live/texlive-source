dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2016 Karl Berry <tex-live@tug.org>
dnl   Copyright (C) 2009-2015 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define libpng_version
AC_INIT([libpng (TeX Live)], libpng_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([libpng-src/pngget.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

KPSE_BASIC([libpng])

AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_LN_S

KPSE_COMPILER_VISIBILITY

AC_CONFIG_HEADERS([config.h])

# For GCC 5 the default mode for C is -std=gnu11 instead of -std=gnu89
# In pngpriv.h we request just the POSIX 1003.1 and C89 APIs by defining _POSIX_SOURCE to 1
# This is incompatible with the new default mode, so we test for that and force the 
AC_MSG_CHECKING([if we need to force back C standard to C89])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([#define _POSIX_SOURCE 1
#include <stdio.h>
])],AC_MSG_RESULT(no),[
if test "x$GCC" != "xyes"; then
  AC_MSG_ERROR([Forcing back to C89 is required but the flags are unknown for other compilers than GCC])
fi
AC_MSG_RESULT(yes)
CFLAGS="$CFLAGS -std=c89"
])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([malloc.h stdlib.h string.h strings.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_STRUCT_TM
AC_C_RESTRICT

# Checks for library functions.
AC_FUNC_STRTOD
AC_CHECK_FUNCS([memset], , [AC_ERROR([memset not found in libc])])
AC_SEARCH_LIBS([pow], [m], , [AC_ERROR([cannot find pow])])

KPSE_ZLIB_FLAGS

AM_CONDITIONAL([build], [test "x$enable_build" != xno])

if test "x$enable_build" != xno || test -f config.force; then

KPSE_ADD_FLAGS([zlib])
AC_CHECK_FUNC([zlibVersion], , [AC_ERROR([zlib not found])])
KPSE_RESTORE_FLAGS

echo timestamp >config.force
fi

AC_SUBST([LIBPNG_TREE], [libpng-src])

AC_SUBST([LIBPNG_DEFINES], ['-DPNG_CONFIGURE_LIBPNG -DPNG_NO_MMX_CODE'])

AC_CONFIG_FILES([Makefile include/Makefile])

AC_OUTPUT
