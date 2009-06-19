# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 0

# KPSE_COMPILER_WARNINGS
# ----------------------
# Set up compiler warnings for C and C++.
# This macro determines and substitutes WARNING_CFLAGS for the C compiler
# and, if applicable, WARNING_CXXFLAGS for the C++ compiler. To activate
# them a Makefile.am must use them, e.g., in AM_CFLAGS or AM_CXXFLAGS.
AC_DEFUN([KPSE_COMPILER_WARNINGS],
[AC_REQUIRE([_KPSE_COMPILER_WARNINGS_OPTION])[]dnl
KPSE_WARNING_CFLAGS
dnl arrange that AC_PROG_CXX uses _KPSE_WARNING_CXXFLAGS.
AC_PROVIDE_IFELSE([AC_PROG_CXX],
                  [_KPSE_WARNING_CXXFLAGS],
                  [m4_define([AC_PROG_CXX],
                             m4_defn([AC_PROG_CXX])[_KPSE_WARNING_CXXFLAGS])])
]) # KPSE_COMPILER_WARNINGS

KPSE_WARNING_CFLAGS
# -----------------
# Determine and substitute WARNING_CFLAGS for C compiler.
AC_DEFUN([KPSE_WARNING_CFLAGS],
[AC_REQUIRE([_KPSE_COMPILER_WARNINGS_OPTION])[]dnl
AC_REQUIRE([AC_PROG_CC])[]dnl
AC_CACHE_CHECK([what warning flags to pass to the C compiler],
               [kpse_cv_warning_cflags],
               [dnl
if test "x$enable_compiler_warnings" = xno; then
  kpse_cv_warning_cflags=
elif test "x$GCC" = xyes; then
  kpse_cv_warning_cflags="-Wall -Wunused"
  AS_CASE([`$CC -dumpversion`],
          [3.4.* | 4.*],
          [kpse_cv_warning_cflags="$kpse_cv_warning_cflags -Wdeclaration-after-statement"])
  AS_CASE([`$CC -dumpversion`],
          [3.@<:@234@:>@.* | 4.*],
          [kpse_cv_warning_cflags="$kpse_cv_warning_cflags -Wno-unknown-pragmas"])
  if test "x$enable_compiler_warnings" != xmin; then
    kpse_cv_warning_cflags="$kpse_cv_warning_cflags -Wmissing-prototypes -Wmissing-declarations"
    if test "x$enable_compiler_warnings" != xyes; then
      kpse_cv_warning_cflags="$kpse_cv_warning_cflags -Wimplicit -Wparentheses -Wreturn-type"
      kpse_cv_warning_cflags="$kpse_cv_warning_cflags -Wswitch -Wtrigraphs -Wshadow -Wpointer-arith"
      kpse_cv_warning_cflags="$kpse_cv_warning_cflags -Wcast-qual -Wcast-align -Wwrite-strings"
      AS_CASE([`$CC -dumpversion`],
              [3.4.* | 4.*],
              [kpse_cv_warning_cflags="$kpse_cv_warning_cflags -Wold-style-definition"])
    fi
  fi
else
  : # FIXME: warning flags for non-gcc compilers
fi])
WARNING_CFLAGS=$kpse_cv_warning_cflags
AC_SUBST([WARNING_CFLAGS])
]) # KPSE_WARNING_CFLAGS

# KPSE_WARNING_CXXFLAGS
# ---------------------
# Determine and substitute WARNING_CXXFLAGS for C++ compiler.
AC_DEFUN([KPSE_WARNING_CXXFLAGS],
[AC_REQUIRE([_KPSE_COMPILER_WARNINGS_OPTION])[]dnl
AC_REQUIRE([AC_PROG_CXX])[]dnl
_KPSE_WARNING_CXXFLAGS
]) # KPSE_WARNING_CXXFLAGS

# _KPSE_COMPILER_WARNINGS_OPTION
# ------------------------------
# Internal subroutine.
# Provide configure option `--enable-compiler-warnings=[no|min|yes|max]'
# to enable various degrees of compiler warnings.
AC_DEFUN([_KPSE_COMPILER_WARNINGS_OPTION],
[AC_ARG_ENABLE([compiler-warnings],
                AS_HELP_STRING([--enable-compiler-warnings=@<:@no|min|yes|max@:>@],
                               [Turn on compiler warnings @<:@default: yes if maintainer-mode,
                                min otherwise@:>@]))[]dnl
AS_CASE([$enable_compiler_warnings],
        [no | min | yes | max], [],
        [AS_IF([test "x$enable_maintainer_mode" = xyes],
               [enable_compiler_warnings=yes],
               [enable_compiler_warnings=min])])
]) # _KPSE_COMPILER_WARNINGS_OPTION

# _KPSE_WARNING_CXXFLAGS
# ----------------------
# Internal subroutine.
# Determine and substitute WARNING_CXXFLAGS for C++ compiler.
m4_define([_KPSE_WARNING_CXXFLAGS],
[AC_CACHE_CHECK([what warning flags to pass to the C++ compiler],
                [kpse_cv_warning_cxxflags],
                [dnl
if test "x$enable_compiler_warnings" = xno; then
  kpse_cv_warning_cxxflags=
elif test "x$GXX" = xyes; then
  kpse_cv_warning_cxxflags="-Wall -Wunused"
  AS_CASE([`$CXX -dumpversion`],
          [3.@<:@234@:>@.* | 4.*],
          [kpse_cv_warning_cxxflags="$kpse_cv_warning_cxxflags -Wno-unknown-pragmas"])
  if test "x$enable_compiler_warnings" != xmin; then
    if test "x$enable_compiler_warnings" != xyes; then
      kpse_cv_warning_cxxflags="$kpse_cv_warning_cxxflags -Wimplicit -Wparentheses -Wreturn-type"
      kpse_cv_warning_cxxflags="$kpse_cv_warning_cxxflags -Wswitch -Wtrigraphs -Wshadow -Wpointer-arith"
      kpse_cv_warning_cxxflags="$kpse_cv_warning_cxxflags -Wcast-qual -Wcast-align -Wwrite-strings"
    fi
  fi
else
  : # FIXME: warning flags for non-g++ compilers
fi])
WARNING_CXXFLAGS=$kpse_cv_warning_cxxflags
AC_SUBST([WARNING_CXXFLAGS])
m4_define([_KPSE_WARNING_CXXFLAGS], [])[]dnl
]) # _KPSE_WARNING_CXXFLAGS

