# $Id$
# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2022-2024 Luigi Scarso <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_MPFI_FLAGS
# ---------------
# Provide the configure options '--with-system-mpfi' (if in the TL tree),
# '--with-mpfi-includes', and '--with-mpfi-libdir'.
#
# Set the make variables MPFI_INCLUDES and MPFI_LIBS to the CPPFLAGS and
# LIBS required for the `-lmpfi' library in libs/mpfi/ of the TL tree.
AC_DEFUN([KPSE_MPFI_FLAGS], [dnl
AC_REQUIRE([KPSE_MPFR_FLAGS])[]dnl
AC_REQUIRE([KPSE_GMP_FLAGS])[]dnl
_KPSE_LIB_FLAGS([mpfi], [mpfi], [],
                [-IBLD/libs/mpfi/include], [BLD/libs/mpfi/libmpfi.a], [],
                [], [${top_builddir}/../../libs/mpfi/include/mpfi.h])[]dnl
]) # KPSE_MPFI_FLAGS

# KPSE_MPFI_OPTIONS([WITH-SYSTEM])
# --------------------------------
AC_DEFUN([KPSE_MPFI_OPTIONS], [_KPSE_LIB_OPTIONS([mpfi], [$1])])

# KPSE_MPFI_SYSTEM_FLAGS
# ----------------------
AC_DEFUN([KPSE_MPFI_SYSTEM_FLAGS], [_KPSE_LIB_FLAGS_SYSTEM([mpfi], [mpfi])])
