# Public macros for the teTeX / TeX Live (TL) tree.
# Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 0

# KPSE_KPATHSEA_FLAGS([SHELL-CODE=DEFAULT-SHELL-CODE])
# ----------------------------------------------------
# Provide the configure options '--with-system-kpathsea' (if in the TL tree),
# '--with-kpathsea-includes', and '--with-kpathsea-libdir'.
#
# Set the make variables KPATHSEA_INCLUDES and KPATHSEA_LIBS to the CPPFLAGS and
# LIBS required for the `-lkpathsea' library in texk/kpathsea/ of the TL tree.
AC_DEFUN([KPSE_KPATHSEA_FLAGS],
[_KPSE_LIB_FLAGS([kpathsea], [kpathsea],
                 [$1],
                 [-IBLD/texk -ISRC/texk],
                 [BLD/texk/kpathsea/libkpathsea.la], [lt],
                 [${top_srcdir}/../kpathsea/*.[ch]],
                 [${top_builddir}/../kpathsea/paths.h])[]dnl
]) # KPSE_KPATHSEA_FLAGS

# KPSE_KPATHSEA_SYSTEM_FLAGS
# --------------------------
AC_DEFUN([KPSE_KPATHSEA_SYSTEM_FLAGS],
[_KPSE_LIB_FLAGS_SYSTEM([kpathsea], [kpathsea])[]dnl
]) # KPSE__SYSTEM_FLAGS

# KPSE_CHECK_KPSE_FORMAT(FORMAT,
#                        [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# ----------------------------------------------------------------
# Check whether kpathsea declares the kpse_FORMAT_format.
AC_DEFUN([KPSE_CHECK_KPSE_FORMAT],
[AC_CACHE_CHECK([whether kpathsea declares the kpse_$1_format],
                [kpse_cv_have_$1_format],
                [AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <kpathsea/kpathsea.h>]],
                                                 [[kpse_$1_format]])],
                                [kpse_cv_have_$1_format=yes],
                                [kpse_cv_have_$1_format=no])])
AS_IF([test "x$kpse_cv_have_$1_format" = xyes], [$2], [$3])[]dnl
]) # KPSE_CHECK_KPSE_FORMAT

# KPSE_CHECK_XBASENAME([ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# --------------------------------------------------------------
# Check whether kpathsea declares xbasename().
AC_DEFUN([KPSE_CHECK_XBASENAME],
[AC_CHECK_DECL([xbasename], [$1], [$2],
               [[#include <kpathsea/kpathsea.h>]])
]) # KPSE_CHECK_XBASENAME
