# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2009, 2010 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 0

# KPSE_GRAPHITE_FLAGS
# -------------------
# Provide the configure option '--with-system-graphite' (if in the TL tree).
#
# Set the make variables GRAPHITE_INCLUDES and GRAPHITE_LIBS to the CPPFLAGS
# and LIBS required for the `-lgraphite' library in libs/graphite/ of the TL tree.
AC_DEFUN([KPSE_GRAPHITE_FLAGS],
[AC_REQUIRE([KPSE_ZLIB_FLAGS])[]dnl
_KPSE_LIB_FLAGS([graphite], [graphite], [],
                [-IBLD/libs/graphite/include], [BLD/libs/graphite/libgraphite.a], [],
                [], [${top_builddir}/../../libs/graphite/include/graphite/GrClient.h])[]dnl
]) # KPSE_GRAPHITE_FLAGS

# KPSE_GRAPHITE_OPTIONS([WITH-SYSTEM])
# ------------------------------------
AC_DEFUN([KPSE_GRAPHITE_OPTIONS],
[m4_ifval([$1],
          [AC_ARG_WITH([system-graphite],
                       AS_HELP_STRING([--with-system-graphite],
                                      [use installed silgraphite headers and library
                                       (requires pkg-config)]))])[]dnl
]) # KPSE_GRAPHITE_OPTIONS

# KPSE_GRAPHITE_SYSTEM_FLAGS
# --------------------------
AC_DEFUN([KPSE_GRAPHITE_SYSTEM_FLAGS],
[AC_REQUIRE([_KPSE_CHECK_PKG_CONFIG])[]dnl
if $PKG_CONFIG silgraphite --atleast-version=2.3; then
  GRAPHITE_INCLUDES="`$PKG_CONFIG silgraphite --cflags`"
  GRAPHITE_LIBS=`$PKG_CONFIG silgraphite --libs`
elif test "x$need_graphite:$with_system_graphite" = xyes:yes; then
  AC_MSG_ERROR([did not find silgraphite 2.3 or better])
fi
]) # KPSE_GRAPHITE_SYSTEM_FLAGS
