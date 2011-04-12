# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2009, 2010 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 0

# KPSE_POPPLER_FLAGS
# ---------------
# Provide the configure option '--with-system-poppler' (if in the TL tree).
#
# Set the make variables XPDF_INCLUDES and XPDF_LIBS to the CPPFLAGS and
# LIBS required for the `-lxpdf' library in libs/poppler/ of the TL tree.
AC_DEFUN([KPSE_POPPLER_FLAGS],
[_KPSE_LIB_FLAGS([poppler], [poppler], [],
                 [-DPDF_PARSER_ONLY -IBLD/libs/poppler -IBLD/libs/poppler/goo -IBLD/libs/poppler/poppler],
                 [BLD/libs/poppler/libpoppler.a], [],
                 [], [${top_builddir}/../../libs/poppler/poppler/Stream.h])[]dnl
]) # KPSE_POPPLER_FLAGS

# KPSE_POPPLER_OPTIONS([WITH-SYSTEM])
# --------------------------------
AC_DEFUN([KPSE_POPPLER_OPTIONS],
[m4_ifval([$1],
          [AC_ARG_WITH([system-poppler],
                       AS_HELP_STRING([--with-system-poppler],
                                      [use installed poppler headers and library instead of poppler
                                       (requires pkg-config)]))])[]dnl
]) # KPSE_POPPLER_OPTIONS

# KPSE_POPPLER_SYSTEM_FLAGS
# ----------------------
AC_DEFUN([KPSE_POPPLER_SYSTEM_FLAGS],
[AC_REQUIRE([_KPSE_CHECK_PKG_CONFIG])[]dnl
if $PKG_CONFIG poppler --atleast-version=0.10; then
  POPPLER_VERSION='-DPOPPLER_VERSION=\"'`$PKG_CONFIG poppler --modversion`'\"'
  POPPLER_INCLUDES="$POPPLER_VERSION `$PKG_CONFIG poppler --cflags`"
  POPPLER_LIBS=`$PKG_CONFIG poppler --libs`
elif test "x$need_poppler:$with_system_poppler" = xyes:yes; then
  AC_MSG_ERROR([did not find poppler-0.10 or better])
fi
]) # KPSE_POPPLER_SYSTEM_FLAGS
