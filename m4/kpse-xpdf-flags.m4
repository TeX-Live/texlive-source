# $Id$
# Public macros for the TeX Live (TL) tree.
# Copyright 2015-2020 Karl Berry <tex-live@tug.org>
# Copyright 2009-2015 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.
# 
# Support for our semi-homegrown libs/xpdf library. This is derived
# from xpdf source code, but xpdf does not distribute it as a library.
# It is used by pdftex (and nothing else) to read PDF images.
# Other engines use the semi-homegrown pplib library (q.v.) for that.

# The well-known poppler library is also originally derived from xpdf
# source code, but has been greatly revised and extended. TL used to
# (sort of) support poppler as the system xpdf, but after the TL 2020
# release we dropped this, because we switched XeTeX to use pplib, and
# nothing else used poppler. (No engines ever used poppler to generate
# their PDF output).
# 
# poppler is aggressively developed, with requirements for new compilers
# and language versions. That's fine for them, but since we don't need
# anything new, it has become too time-consuming and problematic to
# continue to support it in the TL sources, when we don't have any
# requirement for it.

# KPSE_XPDF_FLAGS
# ---------------
# Provide the configure option '--with-system-xpdf' (if in the TL tree).
#
# Set the make variables XPDF_INCLUDES and XPDF_LIBS to the CPPFLAGS and
# LIBS required for the `-lxpdf' library in libs/xpdf/ of the TL tree.
AC_DEFUN([KPSE_XPDF_FLAGS],
[AC_REQUIRE([KPSE_CHECK_WIN32])[]dnl
_KPSE_LIB_FLAGS([xpdf], [xpdf], [],
                [-DPDF_PARSER_ONLY -IBLD/libs/xpdf -IBLD/libs/xpdf/goo -IBLD/libs/xpdf/xpdf],
                [BLD/libs/xpdf/libxpdf.a], [],
                [], [${top_builddir}/../../libs/xpdf/xpdf/Stream.h])[]dnl
test "x$kpse_cv_have_win32" = xno || XPDF_LIBS="$XPDF_LIBS -lgdi32"
]) # KPSE_XPDF_FLAGS

# KPSE_XPDF_OPTIONS([WITH-SYSTEM]) -- as above, no more poppler.
# Keep this macro's expansion as a valid shell command, though.
# since it is used internally in the configure scripts.
# --------------------------------
AC_DEFUN([KPSE_XPDF_OPTIONS], [dnl
: "kpse_xpdf_options - no-op"
]) # KPSE_XPDF_OPTIONS
dnl [m4_ifval([$1],
dnl           [AC_ARG_WITH([system-xpdf],
dnl                        AS_HELP_STRING([--with-system-xpdf],
dnl                                       [use installed poppler headers and library instead of xpdf library from TL (requires pkg-config)]))])[]dnl
dnl ]) # KPSE_XPDF_OPTIONS

# KPSE_XPDF_SYSTEM_FLAGS -- as above, no more poppler.
# ----------------------
AC_DEFUN([KPSE_XPDF_SYSTEM_FLAGS], [dnl
: "kpse_xpdf_system_flags - no-op"
]) # KPSE_XPDF_SYSTEM_FLAGS
dnl _KPSE_PKG_CONFIG_FLAGS([xpdf], [poppler], [0.12])
dnl POPPLER_VERSION='-DPOPPLER_VERSION=\"'`$PKG_CONFIG poppler --modversion`'\"'
dnl XPDF_INCLUDES="$POPPLER_VERSION $XPDF_INCLUDES"
dnl ]) # KPSE_XPDF_SYSTEM_FLAGS
