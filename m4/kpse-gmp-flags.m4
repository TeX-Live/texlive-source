# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2014 Taco Hoekwater <taco@metatex.org>
# Copyright (C) 2014 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_GMP_FLAGS
# -----------------
# Provide the configure options '--with-system-gmp' (if in the TL tree),
# '--with-gmp-includes', and '--with-gmp-libdir'.
#
# Set the make variables GMP_INCLUDES and GMP_LIBS to the CPPFLAGS and
# LIBS required for the `-lgmp' library in libs/gmp/ of the TL tree.
AC_DEFUN([KPSE_GMP_FLAGS], [dnl
_KPSE_LIB_FLAGS([gmp], [gmp], [],
                [-IBLD/libs/gmp/include], [BLD/libs/gmp/libgmp.a], [],
                [], [${top_builddir}/../../libs/gmp/include/gmp.h])[]dnl
]) # KPSE_GMP_FLAGS

# KPSE_GMP_OPTIONS([WITH-SYSTEM])
# ----------------------------------
AC_DEFUN([KPSE_GMP_OPTIONS], [_KPSE_LIB_OPTIONS([gmp], [$1])])

# KPSE_GMP_SYSTEM_FLAGS
# ------------------------
AC_DEFUN([KPSE_GMP_SYSTEM_FLAGS], [_KPSE_LIB_FLAGS_SYSTEM([gmp], [gmp])])
