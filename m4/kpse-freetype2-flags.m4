# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2009, 2010 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 0

# KPSE_FREETYPE2_FLAGS
# --------------------
# Provide the configure option '--with-system-freetype2' (if in the TL tree).
#
# Set the make variables FREETYPE2_INCLUDES and FREETYPE2_LIBS to the CPPFLAGS and
# LIBS required for the `-lfreetype' library in libs/freetype2/ of the TL tree.
AC_DEFUN([KPSE_FREETYPE2_FLAGS],
[AC_REQUIRE([KPSE_ZLIB_FLAGS])[]dnl
AC_REQUIRE([_KPSE_CHECK_FT2_CONFIG])[]dnl
_KPSE_LIB_FLAGS([freetype2], [freetype], [lt],
                [-IBLD/libs/freetype2/freetype2 -IBLD/libs/freetype2],
                [BLD/libs/freetype2/libfreetype.la], [],
                [], [${top_builddir}/../../libs/freetype2/ft2build.h])[]dnl
]) # KPSE_FREETYPE2_FLAGS

# KPSE_FREETYPE2_OPTIONS([WITH-SYSTEM])
# -------------------------------------
AC_DEFUN([KPSE_FREETYPE2_OPTIONS],
[m4_ifval([$1],
          [AC_ARG_WITH([system-freetype2],
                       AS_HELP_STRING([--with-system-freetype2],
                                      [use installed freetype2 headers and library (requires freetype-config)]))])[]dnl
]) # KPSE_FREETYPE2_OPTIONS

# KPSE_FREETYPE2_SYSTEM_FLAGS
# ---------------------------
AC_DEFUN([KPSE_FREETYPE2_SYSTEM_FLAGS],
[AC_REQUIRE([_KPSE_CHECK_FT2_CONFIG])[]dnl
if $FT2_CONFIG --ftversion >/dev/null 2>&1; then
  FREETYPE2_INCLUDES=`$FT2_CONFIG --cflags`
  FREETYPE2_LIBS=`$FT2_CONFIG --libs`
elif test "x$need_freetype2:$with_system_freetype2" = xyes:yes; then
  AC_MSG_ERROR([did not find freetype-config required for system freetype2 library])
fi
]) # KPSE_FREETYPE2_SYSTEM_FLAGS

# _KPSE_CHECK_FT2_CONFIG
# ----------------------
# Check for freetype-config
AC_DEFUN([_KPSE_CHECK_FT2_CONFIG],
[AC_REQUIRE([AC_CANONICAL_HOST])[]dnl
AC_CHECK_TOOL([FT2_CONFIG], [freetype-config], [false])[]dnl
]) # _KPSE_CHECK_FT2_CONFIG
