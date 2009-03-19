# Public macros for the teTeX / TeX live (TL) tree.
# Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 0

# KPSE_LIBPNG_FLAGS([SHELL-CODE=DEFAULT-SHELL-CODE])
# --------------------------------------------------
# Provide the configure options '--with-system-libpng' (if in the TL tree),
# '--with-libpng-includes', and '--with-libpng-libdir'.
#
# Set the make variables LIBPNG_INCLUDES and LIBPNG_LIBS to the CPPFLAGS and
# LIBS required for the `-lpng' library in libs/libpng/ of the TL tree.
AC_DEFUN([KPSE_LIBPNG_FLAGS],
[AC_REQUIRE([KPSE_ZLIB_FLAGS])[]dnl
_KPSE_LIB_FLAGS([libpng], [png],
                 [$1],
                 [-IBLD/libs/libpng], [BLD/libs/libpng/libpng.a], [],
                 [], [${top_builddir}/../../libs/libpng/png.h ${top_builddir}/../../libs/libpng/pngconf.h])[]dnl
]) # KPSE_LIBPNG_FLAGS

# KPSE_LIBPNG_SYSTEM_FLAGS
# ------------------------
AC_DEFUN([KPSE_LIBPNG_SYSTEM_FLAGS],
[_KPSE_LIB_FLAGS_SYSTEM([libpng], [png])[]dnl
]) # KPSE_LIBPNG_SYSTEM_FLAGS
