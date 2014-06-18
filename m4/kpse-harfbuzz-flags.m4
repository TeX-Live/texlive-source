# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2012-2014 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_HARFBUZZ_FLAGS
# -------------------
# Provide the configure options '--with-system-harfbuzz' (if in the TL tree).
#
# Set the make variables HARFBUZZ_INCLUDES and HARFBUZZ_LIBS to the CPPFLAGS and
# LIBS required for the `-lharfbuzz' library in libs/harfbuzz/ of the TL tree.
AC_DEFUN([KPSE_HARFBUZZ_FLAGS],
[AC_REQUIRE([KPSE_GRAPHITE2_FLAGS])[]dnl
AC_REQUIRE([KPSE_ICU_FLAGS])[]dnl
_KPSE_LIB_FLAGS([harfbuzz], [harfbuzz], [],
                [-IBLD/libs/harfbuzz/include], [BLD/libs/harfbuzz/libharfbuzz.a], [],
                [], [${top_builddir}/../../libs/harfbuzz/include/hb.h])[]dnl
]) # KPSE_HARFBUZZ_FLAGS

# KPSE_HARFBUZZ_OPTIONS([WITH-SYSTEM])
# ------------------------------------
AC_DEFUN([KPSE_HARFBUZZ_OPTIONS], [_KPSE_LIB_OPTIONS([harfbuzz], [$1], [pkg-config])])

# KPSE_HARFBUZZ_SYSTEM_FLAGS
# --------------------------
AC_DEFUN([KPSE_HARFBUZZ_SYSTEM_FLAGS],
[AC_REQUIRE([_KPSE_CHECK_PKG_CONFIG])[]dnl
if $PKG_CONFIG harfbuzz-icu; then
  HARFBUZZ_INCLUDES=`$PKG_CONFIG harfbuzz-icu --cflags`
  HARFBUZZ_LIBS=`$PKG_CONFIG harfbuzz-icu --libs`
elif test "x$need_harfbuzz:$with_system_harfbuzz" = xyes:yes; then
  AC_MSG_ERROR([did not find harfbuzz-icu])
fi
]) # KPSE_HARFBUZZ_SYSTEM_FLAGS
