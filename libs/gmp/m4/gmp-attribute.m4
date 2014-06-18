# Autoconf macros for the GNU MP Library.
# Copyright (C) 2000-2014 Free Software Foundation, Inc.
#
# Copyright (C) 2014 Peter Breitenlohner <tex-live@tug.org>
# Extracted from gmp-6.0.0/acinclude.m4 and adapted for TeX Live.
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# GMP_C_ATTRIBUTE_CONST
# ---------------------
AC_DEFUN([GMP_C_ATTRIBUTE_CONST],
[AC_CACHE_CHECK([whether gcc __attribute__ ((const)) works],
                [gmp_cv_c_attribute_const],
[AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[int foo (int x) __attribute__ ((const));]])],
  [gmp_cv_c_attribute_const=yes], [gmp_cv_c_attribute_const=no])
])
if test $gmp_cv_c_attribute_const = yes; then
  AC_DEFINE([HAVE_ATTRIBUTE_CONST], 1,
  [Define to 1 if the compiler accepts gcc style __attribute__ ((const))])
fi
]) # GMP_C_ATTRIBUTE_CONST


# GMP_C_ATTRIBUTE_MALLOC
# ----------------------
# gcc 2.95.x accepts __attribute__ ((malloc)) but with a warning that
# it's ignored.  Pretend it doesn't exist in this case, to avoid that
# warning.
AC_DEFUN([GMP_C_ATTRIBUTE_MALLOC],
[AC_CACHE_CHECK([whether gcc __attribute__ ((malloc)) works],
                [gmp_cv_c_attribute_malloc],
[gmp_cv_c_attribute_malloc=no
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[void *foo (int x) __attribute__ ((malloc));]])],
  [AS_IF([grep "attribute directive ignored" conftest.err >/dev/null],
         [],
         [gmp_cv_c_attribute_malloc=yes])])
])
if test $gmp_cv_c_attribute_malloc = yes; then
  AC_DEFINE([HAVE_ATTRIBUTE_MALLOC], 1,
  [Define to 1 if the compiler accepts gcc style __attribute__ ((malloc))])
fi
]) # GMP_C_ATTRIBUTE_MALLOC


# GMP_C_ATTRIBUTE_MODE
# --------------------
# Introduced in gcc 2.2, but perhaps not in all Apple derived versions.
AC_DEFUN([GMP_C_ATTRIBUTE_MODE],
[AC_CACHE_CHECK([whether gcc __attribute__ ((mode (XX))) works],
                [gmp_cv_c_attribute_mode],
[AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[typedef int SItype __attribute__ ((mode (SI)));]])],
  [gmp_cv_c_attribute_mode=yes], [gmp_cv_c_attribute_mode=no])
])
if test $gmp_cv_c_attribute_mode = yes; then
  AC_DEFINE([HAVE_ATTRIBUTE_MODE], 1,
  [Define to 1 if the compiler accepts gcc style __attribute__ ((mode (XX)))])
fi
]) # GMP_C_ATTRIBUTE_MODE


# GMP_C_ATTRIBUTE_NORETURN
# ------------------------
AC_DEFUN([GMP_C_ATTRIBUTE_NORETURN],
[AC_CACHE_CHECK([whether gcc __attribute__ ((noreturn)) works],
                [gmp_cv_c_attribute_noreturn],
[AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[void foo (int x) __attribute__ ((noreturn));]])],
  [gmp_cv_c_attribute_noreturn=yes], [gmp_cv_c_attribute_noreturn=no])
])
if test $gmp_cv_c_attribute_noreturn = yes; then
  AC_DEFINE([HAVE_ATTRIBUTE_NORETURN], 1,
  [Define to 1 if the compiler accepts gcc style __attribute__ ((noreturn))])
fi
]) # GMP_C_ATTRIBUTE_NORETURN
