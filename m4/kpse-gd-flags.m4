# Public macros for the teTeX / TeX live (TL) tree.
# Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 0

# KPSE_GD_FLAGS([SHELL-CODE=DEFAULT-SHELL-CODE])
# ----------------------------------------------
# Provide the configure options '--with-system-gd' (if in the TL tree),
# '--with-gd-includes', and '--with-gd-libdir'.
#
# Set the make variables GD_INCLUDES and GD_LIBS to the CPPFLAGS and
# LIBS required for the `-lgd' library in libs/gd/ of the TL tree.
AC_DEFUN([KPSE_GD_FLAGS],
[AC_REQUIRE([KPSE_LIBPNG_FLAGS])[]dnl
_KPSE_LIB_FLAGS([gd], [gd],
                 [$1],
                 [-IBLD/libs/gd], [BLD/libs/gd/libgd.a], [],
                 [], [${top_builddir}/../../libs/libpng/gd.h])[]dnl
]) # KPSE_GD_FLAGS

# KPSE_GD_SYSTEM_FLAGS
# --------------------
AC_DEFUN([KPSE_GD_SYSTEM_FLAGS],
[_KPSE_LIB_FLAGS_SYSTEM([gd], [gd])[]dnl
]) # KPSE_GD_SYSTEM_FLAGS
