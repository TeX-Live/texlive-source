# Public macros for the teTeX / TeX Live (TL) tree.
# Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 0

# KPSE_TECKIT_FLAGS([SHELL-CODE=DEFAULT-SHELL-CODE])
# --------------------------------------------------
# Provide the configure options '--with-system-teckit' (if in the TL tree),
# '--with-teckit-includes', and '--with-teckit-libdir'.
#
# Set the make variables TECKIT_INCLUDES and TECKIT_LIBS to the CPPFLAGS and
# LIBS required for the `-lTECkit' library in libs/teckit/ of the TL tree.
AC_DEFUN([KPSE_TECKIT_FLAGS],
[AC_REQUIRE([KPSE_ZLIB_FLAGS])[]dnl
_KPSE_LIB_FLAGS([teckit], [TECkit],
                 [$1],
                 [-ISRC/libs/teckit/source/Public-headers],
                 [BLD/libs/teckit/libTECkit.a], [],
                 [], [])[]dnl
]) # KPSE_TECKIT_FLAGS

# KPSE_TECKIT_SYSTEM_FLAGS
# ------------------------
AC_DEFUN([KPSE_TECKIT_SYSTEM_FLAGS],
[_KPSE_LIB_FLAGS_SYSTEM([teckit], [TECkit])[]dnl
]) # KPSE_TECKIT_SYSTEM_FLAGS
