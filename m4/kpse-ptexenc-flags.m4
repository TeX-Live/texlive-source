# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2010-2014 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_PTEXENC_FLAGS
# ------------------
# Provide the configure options '--with-system-ptexenc' (if in the TL tree),
# '--with-ptexenc-includes', and '--with-ptexenc-libdir'.
#
# Set the make variables PTEXENC_INCLUDES and PTEXENC_LIBS to the CPPFLAGS and
# LIBS required for the `-lptexenc' library in texk/ptexenc/ of the TL tree.
AC_DEFUN([KPSE_PTEXENC_FLAGS], [dnl
AC_REQUIRE([KPSE_KPATHSEA_FLAGS])[]dnl
_KPSE_TEXLIB_FLAGS([ptexenc], [ptexenc], [lt],
                   [-IBLD/texk/ptexenc -ISRC/texk/ptexenc], [BLD/texk/ptexenc/libptexenc.la], [],
                   [${top_srcdir}/../ptexenc/*.c ${top_srcdir}/../ptexenc/ptexenc/*.h])[]dnl
]) # KPSE_PTEXENC_FLAGS

# KPSE_PTEXENC_OPTIONS([WITH-SYSTEM])
# -----------------------------------
AC_DEFUN([KPSE_PTEXENC_OPTIONS], [_KPSE_LIB_OPTIONS([ptexenc], [$1])])

# KPSE_PTEXENC_SYSTEM_FLAGS
# -------------------------
AC_DEFUN([KPSE_PTEXENC_SYSTEM_FLAGS], [_KPSE_LIB_FLAGS_SYSTEM([ptexenc], [ptexenc])])
