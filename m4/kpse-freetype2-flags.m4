# Public macros for the teTeX / TeX Live (TL) tree.
# Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 0

# KPSE_FREETYPE2_FLAGS([SHELL-CODE=DEFAULT-SHELL-CODE])
# -----------------------------------------------------
# Provide the configure options '--with-system-freetype2' (if in the TL tree).
#
# Set the make variables FREETYPE2_INCLUDES and FREETYPE2_LIBS to the CPPFLAGS and
# LIBS required for the `-lfreetype' library in libs/freetype2/ of the TL tree.
dnl AC_DEFUN([KPSE_FREETYPE2_FLAGS],
dnl [AC_REQUIRE([_KPSE_CHECK_FT2_CONFIG])[]dnl
dnl _KPSE_LIB_FLAGS([freetype2], [freetype],
dnl                 [$1],
dnl                 [-IBLD/libs/freetype2/freetype2 -IBLD/libs/freetype2],
dnl                 [BLD/libs/freetype2/libfreetype.a], [],
dnl                 [], [${top_builddir}/../../libs/freetype2/ft2build.h])[]dnl
dnl ]) # KPSE_FREETYPE2_FLAGS
AC_DEFUN([KPSE_FREETYPE2_FLAGS],
[AC_REQUIRE([_KPSE_CHECK_FT2_CONFIG])[]dnl
_KPSE_LIB_FLAGS([freetype2], [freetype],
                [$1],
                [cat BLD/libs/freetype2/ft-],
                [BLD/libs/freetype2/libfreetype.a], [],
                [], [${top_builddir}/../../libs/freetype2/ft2build.h])[]dnl
if test "x$with_system_freetype2" = xno; then
  FREETYPE2_LIBS='`'"$FREETYPE2_INCLUDES"'libs`'
  FREETYPE2_INCLUDES='`'"$FREETYPE2_INCLUDES"'includes`'
fi
]) # KPSE_FREETYPE2_FLAGS

# KPSE_FREETYPE2_SYSTEM_FLAGS
# ---------------------------
AC_DEFUN([KPSE_FREETYPE2_SYSTEM_FLAGS],
[AC_REQUIRE([_KPSE_CHECK_FT2_CONFIG])[]dnl
if test -n `$FT2_CONFIG --ftversion 2>/dev/null`; then
  FREETYPE2_INCLUDES=`$FT2_CONFIG --cflags`
  FREETYPE2_LIBS=`$FT2_CONFIG --libs`
else
  AC_MSG_ERROR([Can not use installed freetype2 headers and library])
fi
]) # KPSE_FREETYPE2_SYSTEM_FLAGS

# _KPSE_CHECK_FT2_CONFIG
# ----------------------
# Check for freetype-config
AC_DEFUN([_KPSE_CHECK_FT2_CONFIG],
[AC_REQUIRE([AC_CANONICAL_HOST])[]dnl
AC_CHECK_TOOL([FT2_CONFIG], [freetype-config], [:])[]dnl
]) # _KPSE_CHECK_FT2_CONFIG
