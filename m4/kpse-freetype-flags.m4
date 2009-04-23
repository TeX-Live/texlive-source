# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 0

# KPSE_FREETYPE_FLAGS
# -------------------
# Provide the configure options '--with-system-freetype' (if in the TL tree),
# '--with-freetype-includes', and '--with-freetype-libdir'.
#
# Set the make variables FREETYPE_INCLUDES and FREETYPE_LIBS to the CPPFLAGS and
# LIBS required for the `-lttf' library in libs/freetype/ of the TL tree.
AC_DEFUN([KPSE_FREETYPE_FLAGS],
[_KPSE_LIB_FLAGS([freetype], [ttf], [],
                 [-IBLD/libs/freetype/freetype], [BLD/libs/freetype/libttf.a], [],
                 [], [${top_builddir}/../../libs/freetype/freetype/freetype.h])[]dnl
]) # KPSE_FREETYPE_FLAGS

# KPSE_FREETYPE_OPTIONS([WITH-SYSTEM])
# ------------------------------------
AC_DEFUN([KPSE_FREETYPE_OPTIONS], [_KPSE_LIB_OPTIONS([freetype], [$1])])

# KPSE_FREETYPE_SYSTEM_FLAGS
# --------------------------
AC_DEFUN([KPSE_FREETYPE_SYSTEM_FLAGS], [_KPSE_LIB_FLAGS_SYSTEM([freetype], [ttf])])
