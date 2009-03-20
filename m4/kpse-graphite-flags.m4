# Public macros for the teTeX / TeX Live (TL) tree.
# Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 0

# KPSE_GRAPHITE_FLAGS([SHELL-CODE=DEFAULT-SHELL-CODE])
# ----------------------------------------------------
# Provide the configure options '--with-system-graphite' (if in the TL tree),
# '--with-graphite-includes', and '--with-graphite-libdir'.
#
# Set the make variables GRAPHITE_INCLUDES and GRAPHITE_LIBS to the CPPFLAGS
# and LIBS required for the `-lgraphite' library in libs/graphite/ of the TL tree.
AC_DEFUN([KPSE_GRAPHITE_FLAGS],
[AC_REQUIRE([KPSE_ZLIB_FLAGS])[]dnl
_KPSE_LIB_FLAGS([graphite], [graphite],
                 [$1],
                 [-ISRC/libs/graphite/include],
                 [BLD/libs/graphite/libgraphite.a], [],
                 [], [])[]dnl
]) # KPSE_GRAPHITE_FLAGS

# KPSE_GRAPHITE_SYSTEM_FLAGS
# --------------------------
AC_DEFUN([KPSE_GRAPHITE_SYSTEM_FLAGS],
[_KPSE_LIB_FLAGS_SYSTEM([graphite], [graphite])[]dnl
]) # KPSE_GRAPHITE_SYSTEM_FLAGS
