# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_GD_FLAGS
# -------------
# Provide the configure options '--with-system-gd' (if in the TL tree),
# '--with-gd-includes', and '--with-gd-libdir'.
#
# Set the make variables GD_INCLUDES and GD_LIBS to the CPPFLAGS and
# LIBS required for the `-lgd' library in libs/gd/ of the TL tree.
AC_DEFUN([KPSE_GD_FLAGS], [dnl
AC_REQUIRE([KPSE_CHECK_WIN32])[]dnl
AC_REQUIRE([KPSE_LIBPNG_FLAGS])[]dnl
AC_REQUIRE([KPSE_FREETYPE2_FLAGS])[]dnl
_KPSE_LIB_FLAGS([gd], [gd], [],
                [-IBLD/libs/gd/include], [BLD/libs/gd/libgd.a],
                [test "x$kpse_cv_have_win32" = xno || GD_INCLUDES="$GD_INCLUDES -DBGDWIN32 -DNONDLL"],
                [], [${top_builddir}/../../libs/gd/include/gd.h])[]dnl
]) # KPSE_GD_FLAGS

# KPSE_GD_OPTIONS([WITH-SYSTEM])
# ------------------------------
AC_DEFUN([KPSE_GD_OPTIONS], [_KPSE_LIB_OPTIONS([gd], [$1])])

# KPSE_GD_SYSTEM_FLAGS
# --------------------
AC_DEFUN([KPSE_GD_SYSTEM_FLAGS], [_KPSE_LIB_FLAGS_SYSTEM([gd], [gd])])
