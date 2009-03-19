# Public macros for the teTeX / TeX live (TL) tree.
# Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 0

# KPSE_FREETYPE2_FLAGS([SHELL-CODE=DEFAULT-SHELL-CODE])
# -----------------------------------------------------
# Provide the configure options '--with-system-freetype2' (if in the TL tree),
# '--with-freetype2-includes', and '--with-freetype2-libdir'.
#
# Set the make variables FREETYPE2_INCLUDES and FREETYPE2_LIBS to the CPPFLAGS and
# LIBS required for the `-lfreetype' library in libs/freetype2/ of the TL tree.
AC_DEFUN([KPSE_FREETYPE2_FLAGS],
[AC_REQUIRE([KPSE_ZLIB_FLAGS])[]dnl
_KPSE_LIB_FLAGS([freetype2], [freetype],
                 [$1],
                 [-IBLD/libs/freetype2/freetype2 -IBLD/libs/freetype2],
                 [BLD/libs/freetype2/libfreetype.a], [],
                 [], [${top_builddir}/../../libs/freetype2/ft2build.h])[]dnl
]) # KPSE_FREETYPE2_FLAGS

# KPSE_FREETYPE2_SYSTEM_FLAGS
# ---------------------------
AC_DEFUN([KPSE_FREETYPE2_SYSTEM_FLAGS],
[if test "x$with_freetype2_includes" != x && test "x$with_freetype2_includes" != xyes; then
  FREETYPE2_INCLUDES="-I$with_freetype2_includes/freetype2 -I$with_freetype2_includes"
fi
FREETYPE2_LIBS="-lfreetype"
if test "x$with_freetype2_libdir" != x && test "x$with_freetype2_libdir" != xyes; then
  FREETYPE2_LIBS="-L$with_freetype2_libdir $FREETYPE2_LIBS"
fi
]) # KPSE_FREETYPE2_SYSTEM_FLAGS
