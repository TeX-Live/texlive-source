# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2013, 2014 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_LIBPAPER_FLAGS
# -------------------
# Provide the configure options '--with-system-libpaper' (if in the TL tree),
# '--with-libpaper-includes', and '--with-libpaper-libdir'.
#
# Set the make variables LIBPAPER_INCLUDES and LIBPAPER_LIBS to the
# CPPFLAGS and LIBS required for the `-lpaper' library in libs/libspaper/
# of the TL tree.
AC_DEFUN([KPSE_LIBPAPER_FLAGS], [dnl
AC_REQUIRE([KPSE_SAVE_FLAGS])[]dnl
_KPSE_LIB_FLAGS([libpaper], [paper], [],
                [-IBLD/libs/libpaper/include], [BLD/libs/libpaper/libpaper.a], [],
                [], [${top_builddir}/../../libs/libpaper/include/paper.h])[]dnl
]) # KPSE_LIBPAPER_FLAGS

# KPSE_LIBPAPER_OPTIONS([WITH-SYSTEM])
# ------------------------------------
AC_DEFUN([KPSE_LIBPAPER_OPTIONS], [_KPSE_LIB_OPTIONS([libpaper], [$1])])

# KPSE_LIBPAPER_SYSTEM_FLAGS
# --------------------------
AC_DEFUN([KPSE_LIBPAPER_SYSTEM_FLAGS], [_KPSE_LIB_FLAGS_SYSTEM([libpaper], [paper])])
