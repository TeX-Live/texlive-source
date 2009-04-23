# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 0

# KPSE_XPDF_FLAGS
# ---------------
# Set the make variables XPDF_INCLUDES and XPDF_LIBS to the CPPFLAGS and
# LIBS required for the `-lxpdf' library in libs/xpdf/ of the TL tree.
AC_DEFUN([KPSE_XPDF_FLAGS],
[_KPSE_LIB_FLAGS([xpdf], [xpdf], [tree],
                 [-DPDF_PARSER_ONLY -IBLD/libs/xpdf -IBLD/libs/xpdf/goo -IBLD/libs/xpdf/fofi -IBLD/libs/xpdf/xpdf],
                 [BLD/libs/xpdf/libxpdf.a], [],
                 [], [${top_builddir}/../../libs/xpdf/xpdf/Stream.h])[]dnl
]) # KPSE_XPDF_FLAGS
