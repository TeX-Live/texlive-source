# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2013 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_PAPER_FLAGS
# ---------------
# Provide the configure options '--with-system-paper' (if in the TL tree),
# '--with-paper-includes', and '--with-paper-libdir'.
#
# Set the make variables PAPER_INCLUDES and PAPER_LIBS to the CPPFLAGS and
# LIBS required for the `-lz' library in libs/paper/ of the TL tree.
AC_DEFUN([KPSE_PAPER_FLAGS], [dnl
AC_REQUIRE([KPSE_SAVE_FLAGS])[]dnl
_KPSE_LIB_FLAGS([paper], [paper], [],
                [-IBLD/libs/paper/include], [BLD/libs/paper/libpaper.a], [],
                [], [${top_builddir}/../../libs/paper/include/paper.h])[]dnl
]) # KPSE_PAPER_FLAGS

# KPSE_PAPER_OPTIONS([WITH-SYSTEM])
# --------------------------------
AC_DEFUN([KPSE_PAPER_OPTIONS], [_KPSE_LIB_OPTIONS([paper], [$1])])

# KPSE_PAPER_SYSTEM_FLAGS
# ----------------------
AC_DEFUN([KPSE_PAPER_SYSTEM_FLAGS], [_KPSE_LIB_FLAGS_SYSTEM([paper], [paper])])
