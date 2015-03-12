# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2010-2015 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_ZZIPLIB_FLAGS
# ------------------
# Provide the configure options '--with-system-zziplib' (if in the TL tree),
# '--with-zziplib-includes', and '--with-zziplib-libdir'.
#
# Set the make variables ZZIPLIB_INCLUDES and ZZIPLIB_LIBS to the CPPFLAGS and
# LIBS required for the `-lpng' library in libs/zziplib/ of the TL tree.
AC_DEFUN([KPSE_ZZIPLIB_FLAGS], [dnl
AC_REQUIRE([KPSE_ZLIB_FLAGS])[]dnl
_KPSE_LIB_FLAGS([zziplib], [zzip], [],
                [-IBLD/libs/zziplib/include], [BLD/libs/zziplib/libzzip.a], [],
                [], [${top_builddir}/../../libs/zziplib/include/zzip/zzip.h])[]dnl
]) # KPSE_ZZIPLIB_FLAGS

# KPSE_ZZIPLIB_OPTIONS([WITH-SYSTEM])
# -----------------------------------
AC_DEFUN([KPSE_ZZIPLIB_OPTIONS], [_KPSE_LIB_OPTIONS([zziplib], [$1], [pkg-config])])

# KPSE_ZZIPLIB_SYSTEM_FLAGS
# -------------------------
AC_DEFUN([KPSE_ZZIPLIB_SYSTEM_FLAGS], [dnl
_KPSE_PKG_CONFIG_FLAGS([zziplib], [zziplib], [0.12])])
