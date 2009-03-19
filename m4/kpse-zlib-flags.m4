# Public macros for the teTeX / TeX live (TL) tree.
# Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 0

# KPSE_ZLIB_FLAGS([SHELL-CODE=DEFAULT-SHELL-CODE])
# ------------------------------------------------
# Provide the configure options '--with-system-zlib' (if in the TL tree),
# '--with-zlib-includes', and '--with-zlib-libdir'.
#
# Set the make variables ZLIB_INCLUDES and ZLIB_LIBS to the CPPFLAGS and
# LIBS required for the `-lz' library in libs/zlib/ of the TL tree.
AC_DEFUN([KPSE_ZLIB_FLAGS],
[_KPSE_LIB_FLAGS([zlib], [z],
                 [$1],
                 [-IBLD/libs/zlib], [BLD/libs/zlib/libz.a], [],
                 [], [${top_builddir}/../../libs/zlib/zlib.h ${top_builddir}/../../libs/zlib/zconf.h])[]dnl
]) # KPSE_ZLIB_FLAGS

# KPSE_ZLIB_SYSTEM_FLAGS
# ----------------------
AC_DEFUN([KPSE_ZLIB_SYSTEM_FLAGS],
[_KPSE_LIB_FLAGS_SYSTEM([zlib], [z])[]dnl
]) # KPSE_ZLIB_SYSTEM_FLAGS
