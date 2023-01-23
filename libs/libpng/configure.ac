dnl $Id$
dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright 2016-2023 Karl Berry <tex-live@tug.org>
dnl   Copyright 2009-2015 Peter Breitenlohner <tex-live@tug.org>
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

# Because we use $host_cpu below.
KPSE_CANONICAL_HOST

AC_CONFIG_HEADERS([config.h])

AC_PROG_CC
AM_PROG_AS
AC_PROG_RANLIB
AC_PROG_LN_S

KPSE_COMPILER_VISIBILITY

# For GCC 5 the default mode for C is -std=gnu11 instead of -std=gnu89
# In pngpriv.h we request just the POSIX 1003.1 and C89 APIs by defining _POSIX_SOURCE to 1
# This is incompatible with the new default mode, so we test for that and force the 
# "-std=c89" compiler option:
AC_MSG_CHECKING([if we need to force back C standard to C89])
AC_COMPILE_IFELSE(
   [AC_LANG_PROGRAM([
      [#define _POSIX_SOURCE 1]
      [#include <stdio.h>]
   ])],
   AC_MSG_RESULT(no),[
      if test "x$GCC" != "xyes"; then
         AC_MSG_ERROR(
            [Forcing back to C89 is required but the flags are only known for GCC])
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

# emacs-page
# ARM
# ===
#
# ARM NEON (SIMD) support.

AC_ARG_ENABLE([arm-neon],
   AS_HELP_STRING([[[--enable-arm-neon]]],
      [Enable ARM NEON optimizations: =no/off, check, api, yes/on:]
      [no/off: disable the optimizations; check: use internal checking code]
      [(deprecated and poorly supported); api: disable by default, enable by]
      [a call to png_set_option; yes/on: turn on unconditionally.]
      [If not specified: determined by the compiler.]),
   [case "$enableval" in
      no|off)
         # disable the default enabling on __ARM_NEON__ systems:
         AC_DEFINE([PNG_ARM_NEON_OPT], [0],
                   [Disable ARM Neon optimizations])
         # Prevent inclusion of the assembler files below:
         enable_arm_neon=no ;;
      check)
         AC_DEFINE([PNG_ARM_NEON_CHECK_SUPPORTED], [],
                   [Check for ARM Neon support at run-time]);;
      api)
         AC_DEFINE([PNG_ARM_NEON_API_SUPPORTED], [],
                   [Turn on ARM Neon optimizations at run-time]);;
      yes|on)
         AC_DEFINE([PNG_ARM_NEON_OPT], [2],
                   [Enable ARM Neon optimizations])
         AC_MSG_WARN([--enable-arm-neon: please specify 'check' or 'api', if]
            [you want the optimizations unconditionally pass -mfpu=neon]
            [to the compiler.]);;
      *)
         AC_MSG_ERROR([--enable-arm-neon=${enable_arm_neon}: invalid value])
   esac])

# Add ARM-specific files to all builds where the host_cpu is arm ('arm*') or
# where ARM optimizations were explicitly requested (this allows a fallback
#  if a future host CPU does not match 'arm*')

AM_CONDITIONAL([PNG_ARM_NEON],
   [test "$enable_arm_neon" != 'no' &&
    case "$host_cpu" in
      arm*|aarch64*) : ;;
      *)    test "$enable_arm_neon" != '' ;;
    esac])

AM_COND_IF([PNG_ARM_NEON],
  AC_MSG_NOTICE([[arm_neon opts on (host_cpu=$host_cpu)]]),
  AC_MSG_NOTICE([[arm_neon opts off (host_cpu=$host_cpu)]]),
)

# emacs-page
# PowerPC
# ===
#
# PowerPC VSX (SIMD) support.

AC_ARG_ENABLE([powerpc-vsx],
AS_HELP_STRING([[[--enable-powerpc-vsx]]],
      [Enable POWERPC VSX optimizations: =no/off, check, api, yes/on:]
      [no/off: disable the optimizations; check: use internal checking code]
      [api: disable by default, enable by a call to png_set_option]
      [yes/on: turn on unconditionally.]
      [If not specified: determined by the compiler.]),
   [case "$enableval" in
      no|off)
         # disable the default enabling on __ppc64__ systems:
         AC_DEFINE([PNG_POWERPC_VSX_OPT], [0],
                   [Disable POWERPC VSX optimizations])
         # Prevent inclusion of the platform-specific files below:
         enable_powerpc_vsx=no ;;
      check)
         AC_DEFINE([PNG_POWERPC_VSX_CHECK_SUPPORTED], [],
                   [Check for POWERPC VSX support at run-time])
         AC_MSG_WARN([--enable-powerpc-vsx Please check contrib/powerpc/README file]
            [for the list of supported OSes.]);;
      api)
         AC_DEFINE([PNG_POWERPC_VSX_API_SUPPORTED], [],
                   [Turn on POWERPC VSX optimizations at run-time]);;
      yes|on)
         AC_DEFINE([PNG_POWERPC_VSX_OPT], [2],
                   [Enable POWERPC VSX optimizations])
         AC_MSG_WARN([--enable-powerpc-vsx: please specify 'check' or 'api', if]
            [you want the optimizations unconditionally pass '-maltivec -mvsx']
            [or '-mcpu=power8' to the compiler.]);;
      *)
         AC_MSG_ERROR([--enable-powerpc-vsx=${enable_powerpc_vsx}: invalid value])
   esac])

# Add PowerPC-specific files to all builds where $host_cpu is powerpc('powerpc*')
# or where PowerPC optimizations were explicitly requested (this allows a fallback
# if a future host CPU does not match 'powerpc*')

AM_CONDITIONAL([PNG_POWERPC_VSX],
   [test "$enable_powerpc_vsx" != 'no' &&
    case "$host_cpu" in
      powerpc*|ppc64*) : ;;
    esac])
AM_COND_IF([PNG_POWERPC_VSX],
  AC_MSG_NOTICE([[powerpc_vsx opts on (host_cpu=$host_cpu)]]),
  AC_MSG_NOTICE([[powerpc_vsx opts off (host_cpu=$host_cpu)]]),
)
AC_CONFIG_FILES([Makefile include/Makefile])

AC_OUTPUT
