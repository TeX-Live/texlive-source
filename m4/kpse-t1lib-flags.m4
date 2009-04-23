# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 0

# KPSE_T1LIB_FLAGS
# ----------------
# Provide the configure options '--with-system-t1lib' (if in the TL tree),
# '--with-t1lib-includes', and '--with-t1lib-libdir'.
#
# Set the make variables T1LIB_INCLUDES and T1LIB_LIBS to the CPPFLAGS and
# LIBS required for the `-lt1' library in libs/t1lib/ of the TL tree.
AC_DEFUN([KPSE_T1LIB_FLAGS],
[_KPSE_LIB_FLAGS([t1lib], [t1], [],
                 [-IBLD/libs/t1lib], [BLD/libs/t1lib/libt1.a], [],
                 [], [${top_builddir}/../../libs/t1lib/t1lib.h])[]dnl
]) # KPSE_T1LIB_FLAGS

# KPSE_T1LIB_OPTIONS([WITH-SYSTEM])
# ---------------------------------
AC_DEFUN([KPSE_T1LIB_OPTIONS], [_KPSE_LIB_OPTIONS([t1lib], [$1])])

# KPSE_T1LIB_SYSTEM_FLAGS
# -----------------------
AC_DEFUN([KPSE_T1LIB_SYSTEM_FLAGS], [_KPSE_LIB_FLAGS_SYSTEM([t1lib], [t1])])
