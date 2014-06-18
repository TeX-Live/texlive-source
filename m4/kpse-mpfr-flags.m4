# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2014 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_MPFR_FLAGS
# ---------------
# Provide the configure options '--with-system-mpfr' (if in the TL tree),
# '--with-mpfr-includes', and '--with-mpfr-libdir'.
#
# Set the make variables MPFR_INCLUDES and MPFR_LIBS to the CPPFLAGS and
# LIBS required for the `-lmpfr' library in libs/mpfr/ of the TL tree.
AC_DEFUN([KPSE_MPFR_FLAGS], [dnl
AC_REQUIRE([KPSE_GMP_FLAGS])[]dnl
_KPSE_LIB_FLAGS([mpfr], [mpfr], [],
                [-IBLD/libs/mpfr/include], [BLD/libs/mpfr/libmpfr.a], [],
                [], [${top_builddir}/../../libs/mpfr/include/mpfr.h])[]dnl
]) # KPSE_MPFR_FLAGS

# KPSE_MPFR_OPTIONS([WITH-SYSTEM])
# --------------------------------
AC_DEFUN([KPSE_MPFR_OPTIONS], [_KPSE_LIB_OPTIONS([mpfr], [$1])])

# KPSE_MPFR_SYSTEM_FLAGS
# ----------------------
AC_DEFUN([KPSE_MPFR_SYSTEM_FLAGS], [_KPSE_LIB_FLAGS_SYSTEM([mpfr], [mpfr])])
