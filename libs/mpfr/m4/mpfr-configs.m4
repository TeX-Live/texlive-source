# Autoconf macros for the GNU MPFR Library.
# Copyright (C) 2000-2013 Free Software Foundation, Inc.
# Contributed by the AriC and Caramel projects, INRIA.
#
# Copyright (C) 2014, 2015 Peter Breitenlohner <tex-live@tug.org>
# Extracted from mpfr-3.1.2/acinclude.m4 and adapted for TeX Live.
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

dnl ------------------------------------------------------------
dnl You must put in MPFR_CONFIGS everything which configure MPFR
dnl except:
dnl   -everything dealing with CC and CFLAGS in particular the ABI
dnl   but the IEEE-754 specific flags must be set here.
dnl   -GMP's linkage.
dnl   -Libtool stuff.
dnl   -Handling of special arguments of MPFR's configure.
AC_DEFUN([MPFR_CONFIGS], [dnl
AC_REQUIRE([AC_HEADER_TIME])
AC_REQUIRE([AC_CANONICAL_HOST])

dnl Check for wide characters (wchar_t and wint_t)
AC_CHECK_HEADERS([wchar.h])

dnl We have stdarg.h
AC_DEFINE([HAVE_STDARG], 1, [Define if stdarg])

AC_CHECK_HEADERS([sys/time.h])

dnl Check how to get `alloca'
AC_FUNC_ALLOCA

KPSE_CHECK_SIZE_MAX

dnl va_copy macro
AC_MSG_CHECKING([how to copy va_list])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <stdarg.h>
]], [[
   va_list ap1, ap2;
   va_copy(ap1, ap2);
]])], [
   AC_MSG_RESULT([va_copy])
   AC_DEFINE([HAVE_VA_COPY], 1, [Define to 1 if you have the `va_copy' function.])
], [AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <stdarg.h>
]], [[
   va_list ap1, ap2;
   __va_copy(ap1, ap2);
]])], [AC_DEFINE([HAVE___VA_COPY], 1, [Define to 1 if you have the `__va_copy' function.])
       AC_MSG_RESULT([__va_copy])],
   [AC_MSG_RESULT([memcpy])])])

dnl Check for IEEE-754 switches on Alpha
AS_CASE([$host],
        [alpha*-*-*], [
  AC_CACHE_CHECK([for IEEE-754 switches], [mpfr_cv_ieee_switches], [
  saved_CFLAGS=$CFLAGS
  if test -n "$GCC"; then
    mpfr_cv_ieee_switches="-mfp-rounding-mode=d -mieee-with-inexact"
  else
    mpfr_cv_ieee_switches="-fprm d -ieee_with_inexact"
  fi
  CFLAGS="$CFLAGS $mpfr_cv_ieee_switches"
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([])],
    [], [mpfr_cv_ieee_switches=none])
  CFLAGS=$saved_CFLAGS
  ])
  test "x$mpfr_cv_ieee_switches" = xnone || CFLAGS="$CFLAGS $mpfr_cv_ieee_switches"
])

dnl check for long long
AC_CHECK_TYPE([long long int],
   [AC_DEFINE([HAVE_LONG_LONG], 1, [Define if compiler supports long long])])

dnl intmax_t is C99
AC_CHECK_TYPES([intmax_t])
if test "x$ac_cv_type_intmax_t" = xyes; then
  AC_CACHE_CHECK([for working INTMAX_MAX], [mpfr_cv_have_intmax_max], [
    saved_CPPFLAGS=$CPPFLAGS
    CPPFLAGS="$CPPFLAGS -I$srcdir/mpfr-]mpfr_version[/src"
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include "mpfr-intmax.h"]],
                                       [[intmax_t x = INTMAX_MAX;]])],
      [mpfr_cv_have_intmax_max=yes], [mpfr_cv_have_intmax_max=no])
    CPPFLAGS=$saved_CPPFLAGS
  ])
  if test "x$mpfr_cv_have_intmax_max" = xyes; then
    AC_DEFINE([MPFR_HAVE_INTMAX_MAX], 1, [Define if you have a working INTMAX_MAX.])
  fi
fi

KPSE_SEARCH_LIBS([MPFR_LIBM], [pow], [m])

dnl Check for fesetround
AC_CACHE_CHECK([for fesetround], [mpfr_cv_have_fesetround], [
saved_LIBS=$LIBS
LIBS="$LIBS $MPFR_LIBM"
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <fenv.h>]], [[fesetround(FE_TONEAREST);]])],
  [mpfr_cv_have_fesetround=yes], [mpfr_cv_have_fesetround=no])
LIBS=$saved_LIBS
])
if test "x$mpfr_cv_have_fesetround" = xyes; then
  AC_DEFINE([MPFR_HAVE_FESETROUND], 1,
            [Define if you have the `fesetround' function via the <fenv.h> header file.])
fi

dnl Check for gcc float-conversion bug; if need be, -ffloat-store is used to
dnl force the conversion to the destination type when a value is stored to
dnl a variable (see ISO C99 standard 5.1.2.3#13, 6.3.1.5#2, 6.3.1.8#2). This
dnl is important concerning the exponent range. Note that this doesn't solve
dnl the double-rounding problem.
if test -n "$GCC"; then
  AC_CACHE_CHECK([for gcc float-conversion bug], [mpfr_cv_gcc_floatconv_bug], [
  saved_LIBS=$LIBS
  LIBS="$LIBS $MPFR_LIBM"
  AC_TRY_RUN([
#include <float.h>
#ifdef MPFR_HAVE_FESETROUND
#include <fenv.h>
#endif
static double get_max (void);
int main() {
  double x = 0.5;
  double y;
  int i;
  for (i = 1; i <= 11; i++)
    x *= x;
  if (x != 0)
    return 1;
#ifdef MPFR_HAVE_FESETROUND
  /* Useful test for the G4 PowerPC */
  fesetround(FE_TOWARDZERO);
  x = y = get_max ();
  x *= 2.0;
  if (x != y)
    return 1;
#endif
  return 0;
}
static double get_max (void) { static volatile double d = DBL_MAX; return d; }
  ], [mpfr_cv_gcc_floatconv_bug="no"],
     [mpfr_cv_gcc_floatconv_bug="yes, use -ffloat-store"],
     [mpfr_cv_gcc_floatconv_bug="cannot test, use -ffloat-store"])
  LIBS=$saved_LIBS
  ])
  test "x$mpfr_cv_gcc_floatconv_bug" = xno || CFLAGS="$CFLAGS -ffloat-store"
fi

dnl Check if denormalized numbers are supported
AC_CACHE_CHECK([for denormalized numbers], [mpfr_cv_have_denorms], [
AC_TRY_RUN([
#include <math.h>
#include <stdio.h>
int main() {
  double x = 2.22507385850720138309e-308;
  fprintf (stderr, "%e\n", x / 2.0);
  return 2.0 * (x / 2.0) != x;
}
], [mpfr_cv_have_denorms=yes], [mpfr_cv_have_denorms=no], [mpfr_cv_have_denorms=no])
])
if test "x$mpfr_cv_have_denorms" = xyes; then
  AC_DEFINE([HAVE_DENORMS], 1, [Define if denormalized floats work.])
fi

dnl Check the FP division by 0 fails (e.g. on a non-IEEE-754 platform).
dnl In such a case, MPFR_ERRDIVZERO is defined to disable the tests
dnl involving a FP division by 0.
dnl For the developers: to check whether all these tests are disabled,
dnl configure MPFR with "-DMPFR_TEST_DIVBYZERO=1 -DMPFR_ERRDIVZERO=1".
AC_CACHE_CHECK([if the FP division by 0 fails], [mpfr_cv_errdivzero], [
AC_TRY_RUN([
int main() {
  volatile double d = 0.0, x;
  x = 0.0 / d;
  x = 1.0 / d;
  return 0;
}
], [mpfr_cv_errdivzero="no"],
   [mpfr_cv_errdivzero="yes"],
   [mpfr_cv_errdivzero="cannot test, assume no"])
])
if test "x$mpfr_cv_errdivzero" = xyes; then
  AC_DEFINE([MPFR_ERRDIVZERO], 1, [Define if the FP division by 0 fails.])
  AC_MSG_WARN([The floating-point division by 0 fails instead of])
  AC_MSG_WARN([returning a special value: NaN or infinity. Tests])
  AC_MSG_WARN([involving a FP division by 0 will be disabled.])
fi

dnl Check whether NAN != NAN (as required by the IEEE-754 standard,
dnl but not by the ISO C standard). For instance, this is false with
dnl MIPSpro 7.3.1.3m under IRIX64. By default, assume this is true.
AC_CACHE_CHECK([if NAN == NAN], [mpfr_cv_nanisnan], [
AC_TRY_RUN([
#include <stdio.h>
#include <math.h>
#ifndef NAN
# define NAN (0.0/0.0)
#endif
int main() {
  double d;
  d = NAN;
  return d != d;
}
], [mpfr_cv_nanisnan="yes"],
   [mpfr_cv_nanisnan="no"],
   [mpfr_cv_nanisnan="cannot test, assume no"])
])
if test "x$mpfr_cv_nanisnan" = xyes; then
  AC_DEFINE([MPFR_NANISNAN], 1, [Define if NAN == NAN.])
  AC_MSG_WARN([The test NAN != NAN is false. The probable reason is that])
  AC_MSG_WARN([your compiler optimizes floating-point expressions in an])
  AC_MSG_WARN([unsafe way because some option, such as -ffast-math or])
  AC_MSG_WARN([-fast (depending on the compiler), has been used.  You])
  AC_MSG_WARN([should NOT use such an option, otherwise MPFR functions])
  AC_MSG_WARN([such as mpfr_get_d and mpfr_set_d may return incorrect])
  AC_MSG_WARN([results on special FP numbers (e.g. NaN or signed zeros).])
  AC_MSG_WARN([If you did not use such an option, please send us a bug])
  AC_MSG_WARN([report so that we can try to find a workaround for your])
  AC_MSG_WARN([platform and/or document the behavior.])
fi

dnl Check if the chars '0' to '9' are consecutive values
AC_MSG_CHECKING([if charset has consecutive values])
AC_RUN_IFELSE([AC_LANG_PROGRAM([[
char *number = "0123456789";
char *lower  = "abcdefghijklmnopqrstuvwxyz";
char *upper  = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
]],[[
 int i;
 unsigned char *p;
 for (p = (unsigned char*) number, i = 0; i < 9; i++)
   if ( (*p)+1 != *(p+1) ) return 1;
 for (p = (unsigned char*) lower, i = 0; i < 25; i++)
   if ( (*p)+1 != *(p+1) ) return 1;
 for (p = (unsigned char*) upper, i = 0; i < 25; i++)
   if ( (*p)+1 != *(p+1) ) return 1;
]])], [AC_MSG_RESULT([yes])],[
 AC_MSG_RESULT([no])
 AC_DEFINE([MPFR_NO_CONSECUTIVE_CHARSET], 1, [Charset is not consecutive])
], [AC_MSG_RESULT([cannot test])])

dnl Now try to check the long double format
MPFR_C_LONG_DOUBLE_FORMAT

])
dnl end of MPFR_CONFIGS


dnl  MPFR_LD_SEARCH_PATHS_FIRST
dnl  --------------------------

AC_DEFUN([MPFR_LD_SEARCH_PATHS_FIRST],
[AS_CASE([$LDFLAGS],
         [*-Wl,-search_paths_first*], [],
     [AC_MSG_CHECKING([if the compiler understands -Wl,-search_paths_first])
      saved_LDFLAGS=$LDFLAGS
      LDFLAGS="-Wl,-search_paths_first $LDFLAGS"
      AC_LINK_IFELSE([AC_LANG_PROGRAM([[]], [[]])],
       [AC_MSG_RESULT([yes])],
       [AC_MSG_RESULT([no])
        LDFLAGS=$saved_LDFLAGS])])
])
