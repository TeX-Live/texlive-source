# Public macros for the TeX Live (TL) tree - freetype2.
# Copyright 2015-2019 Karl Berry <tex-live@tug.org>
# Copyright 2009-2014 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_FREETYPE2_FLAGS
# --------------------
# Provide the configure option '--with-system-freetype2' (if in the TL tree).
#
# Set the make variables FREETYPE2_INCLUDES and FREETYPE2_LIBS to the
# CPPFLAGS and LIBS required for the `-lfreetype' library in
# libs/freetype2/ of the TL tree.
# 
AC_DEFUN([KPSE_FREETYPE2_FLAGS], [dnl
AC_REQUIRE([KPSE_ZLIB_FLAGS])[]dnl
_KPSE_LIB_FLAGS([freetype2], [freetype], [],
                [-IBLD/libs/freetype2/freetype2],
                [BLD/libs/freetype2/libfreetype.a], [], [],
                [${top_builddir}/../../libs/freetype2/freetype2/ft2build.h])[]dnl
]) # KPSE_FREETYPE2_FLAGS

# KPSE_FREETYPE2_OPTIONS([WITH-SYSTEM])
# -------------------------------------
AC_DEFUN([KPSE_FREETYPE2_OPTIONS], [dnl
_KPSE_LIB_OPTIONS([freetype2], [$1], [freetype-config])])

# KPSE_FREETYPE2_SYSTEM_FLAGS
# ---------------------------
# try pkg-config if freetype-config isn't available.
AC_DEFUN([KPSE_FREETYPE2_SYSTEM_FLAGS], [dnl
AC_REQUIRE([AC_CANONICAL_HOST])[]dnl
AC_CHECK_TOOL([FT2_CONFIG], [freetype-config], [false])[]dnl
AC_CHECK_TOOL([PKG_CONFIG], [pkg-config], [false])[]dnl
if $FT2_CONFIG --ftversion >/dev/null 2>&1; then
  FREETYPE2_INCLUDES=`$FT2_CONFIG --cflags`
  FREETYPE2_LIBS=`$FT2_CONFIG --libs`
elif $PKG_CONFIG --libs freetype2 >/dev/null 2>&1; then
  FREETYPE2_INCLUDES=`$PKG_CONFIG --cflags freetype2`
  FREETYPE2_LIBS=`$PKG_CONFIG --libs freetype2`
elif test "x$need_freetype2:$with_system_freetype2" = xyes:yes; then
  AC_MSG_ERROR([did not find freetype-config required for system freetype2 library])
fi
]) # KPSE_FREETYPE2_SYSTEM_FLAGS
