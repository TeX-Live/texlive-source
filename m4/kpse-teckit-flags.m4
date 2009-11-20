# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 0

# KPSE_TECKIT_FLAGS
# -----------------
# Provide the configure options '--with-system-teckit' (if in the TL tree),
# '--with-teckit-includes', and '--with-teckit-libdir'.
#
# Set the make variables TECKIT_INCLUDES and TECKIT_LIBS to the CPPFLAGS and
# LIBS required for the `-lTECkit' library in libs/teckit/ of the TL tree.
AC_DEFUN([KPSE_TECKIT_FLAGS],
[AC_REQUIRE([KPSE_ZLIB_FLAGS])[]dnl
_KPSE_LIB_FLAGS([teckit], [TECkit], [],
                [-IBLD/libs/teckit/include], [BLD/libs/teckit/libTECkit.a], [],
                [], [${top_builddir}/../../libs/teckit/include/teckit/TECkit_Common.h])[]dnl
]) # KPSE_TECKIT_FLAGS

# KPSE_TECKIT_OPTIONS([WITH-SYSTEM])
# ----------------------------------
AC_DEFUN([KPSE_TECKIT_OPTIONS], [_KPSE_LIB_OPTIONS([teckit], [$1])])

# KPSE_TECKIT_SYSTEM_FLAGS
# ------------------------
AC_DEFUN([KPSE_TECKIT_SYSTEM_FLAGS], [_KPSE_LIB_FLAGS_SYSTEM([teckit], [TECkit])])
