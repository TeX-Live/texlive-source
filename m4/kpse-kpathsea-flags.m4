# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2009-2015 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_KPATHSEA_FLAGS([OPTIONS])
# ------------------------------
# Provide the configure options '--with-system-kpathsea' (if in the TL tree).
# Options:
#          no-debug - add '-DNO_DEBUG' to KPATHSEA_INCLUDES even if not needed
#
# Set the make variables KPATHSEA_INCLUDES and KPATHSEA_LIBS to the CPPFLAGS and
# LIBS required for the `-lkpathsea' library in texk/kpathsea/ of the TL tree.
AC_DEFUN([KPSE_KPATHSEA_FLAGS], [dnl
AC_REQUIRE([KPSE_SAVE_FLAGS])[]dnl
_KPSE_TEXLIB_FLAGS([kpathsea], [kpathsea], [lt],
                   [-IBLD/texk -ISRC/texk], [BLD/texk/kpathsea/libkpathsea.la], [],
                   [${top_srcdir}/../kpathsea/*.[ch]],
                   [${top_builddir}/../kpathsea/paths.h])
m4_if(m4_index([ $1 ], [ no-debug ]), [-1],
      [_KPSE_CHECK_KPSE_DEBUG],
      [KPATHSEA_INCLUDES="$KPATHSEA_INCLUDES -DNO_DEBUG"])
]) # KPSE_KPATHSEA_FLAGS

# KPSE_KPATHSEA_OPTIONS([WITH-SYSTEM])
# ------------------------------------
AC_DEFUN([KPSE_KPATHSEA_OPTIONS], [_KPSE_LIB_OPTIONS([kpathsea], [$1], [pkg-config])])

# _KPSE_CHECK_KPSE_DEBUG
# ----------------------
# Internal subroutine to check if libkpathsea supports debugging.
m4_define([_KPSE_CHECK_KPSE_DEBUG], [dnl
AC_CACHE_CHECK([if libkpathsea supports debugging],
               [kpse_cv_kpse_debug],
               [KPSE_ADD_FLAGS([kpathsea])
                AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <kpathsea/kpathsea.h>]],
                                                [[FILE *f = fopen("f", "r")]])],
                               [kpse_cv_kpse_debug=yes],
                               [kpse_cv_kpse_debug=no])
                KPSE_RESTORE_FLAGS])
AS_IF([test "x$kpse_cv_kpse_debug" != xyes],
      [KPATHSEA_INCLUDES="$KPATHSEA_INCLUDES -DNO_DEBUG"])[]dnl
]) # _KPSE_CHECK_KPSE_DEBUG

# KPSE_KPATHSEA_SYSTEM_FLAGS
# --------------------------
AC_DEFUN([KPSE_KPATHSEA_SYSTEM_FLAGS], [dnl
_KPSE_PKG_CONFIG_FLAGS([kpathsea], [kpathsea])])

# KPSE_CHECK_KPSE_FORMAT(FORMAT,
#                        [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# ----------------------------------------------------------------
# Check whether kpathsea declares the kpse_FORMAT_format.
AC_DEFUN([KPSE_CHECK_KPSE_FORMAT], [dnl
AC_CACHE_CHECK([whether kpathsea declares the kpse_$1_format],
               [kpse_cv_have_$1_format],
               [AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <kpathsea/kpathsea.h>]],
                                                [[kpse_$1_format]])],
                               [kpse_cv_have_$1_format=yes],
                               [kpse_cv_have_$1_format=no])])
AS_IF([test "x$kpse_cv_have_$1_format" = xyes], [$2], [$3])
]) # KPSE_CHECK_KPSE_FORMAT

# KPSE_CHECK_XBASENAME([ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# --------------------------------------------------------------
# Check whether kpathsea declares xbasename().
AC_DEFUN([KPSE_CHECK_XBASENAME], [dnl
AC_CHECK_DECL([xbasename], [$1], [$2], [[#include <kpathsea/kpathsea.h>]])
]) # KPSE_CHECK_XBASENAME
