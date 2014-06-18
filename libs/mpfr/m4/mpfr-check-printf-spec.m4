# Autoconf macros for the GNU MPFR Library.
# Copyright (C) 2000-2013 Free Software Foundation, Inc.
# Contributed by the AriC and Caramel projects, INRIA.
#
# Copyright (C) 2014 Peter Breitenlohner <tex-live@tug.org>
# Extracted from mpfr-3.1.2/acinclude.m4 and adapted for TeX Live.
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# MPFR_CHECK_PRINTF_SPEC
# ----------------------
# Check if gmp_printf supports some optional length modifiers.
# Defined symbols are negative to shorten the gcc command line.
AC_DEFUN([MPFR_CHECK_PRINTF_SPEC], [
AC_REQUIRE([MPFR_CONFIGS])dnl
if test "x$ac_cv_type_intmax_t" = xyes; then
 MPFR_FUNC_GMP_PRINTF_SPEC([jd], [intmax_t], [
#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#endif
#ifdef HAVE_STDINT_H
# include <stdint.h>
#endif
         ], ,
         [AC_DEFINE([NPRINTF_J], 1, [gmp_printf cannot read intmax_t])])
fi

MPFR_FUNC_GMP_PRINTF_SPEC([hhd], [char], , ,
         [AC_DEFINE([NPRINTF_HH], 1, [gmp_printf cannot use `hh' length modifier])])

MPFR_FUNC_GMP_PRINTF_SPEC([lld], [long long int], , ,
         [AC_DEFINE([NPRINTF_LL], 1, [gmp_printf cannot read long long int])])

MPFR_FUNC_GMP_PRINTF_SPEC([Lf], [long double], , ,
         [AC_DEFINE([NPRINTF_L], 1, [gmp_printf cannot read long double])])

MPFR_FUNC_GMP_PRINTF_SPEC([td], [ptrdiff_t], [
#if defined (__cplusplus)
#include <cstddef>
#else
#include <stddef.h>
#endif
    ], ,
    [AC_DEFINE([NPRINTF_T], 1, [gmp_printf cannot read ptrdiff_t])])
]) # MPFR_CHECK_PRINTF_SPEC


# MPFR_FUNC_GMP_PRINTF_SPEC(SPEC, TYPE, [INCLUDES], [IF-TRUE], [IF-FALSE])
# ------------------------------------------------------------------------
# Check if gmp_sprintf supports the conversion specification 'spec'
# with type 'type'.
# Expand 'if-true' if printf supports 'spec', 'if-false' otherwise.
m4_define([MPFR_FUNC_GMP_PRINTF_SPEC], [
AC_MSG_CHECKING([if gmp_printf supports "%$1"])
AC_RUN_IFELSE([AC_LANG_PROGRAM([[
#include <stdio.h>
$3
#include <gmp.h>
]], [[
  char s[256];
  $2 a = 17;

  if (gmp_sprintf (s, "(%0.0$1)(%d)", a, 42) == -1) return 1;
  return (strcmp (s, "(17)(42)") != 0);
]])],
  [AC_MSG_RESULT([yes])
  $4],
  [AC_MSG_RESULT([no])
  $5])
]) # MPFR_FUNC_GMP_PRINTF_SPEC
