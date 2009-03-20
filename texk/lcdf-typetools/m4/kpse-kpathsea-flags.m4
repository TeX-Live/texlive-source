# Public macros for the teTeX / TeX Live (TL) tree.
# Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 0

# KPSE_KPATHSEA_FLAGS([SHELL-CODE=DEFAULT-SHELL-CODE])
# ----------------------------------------------------
# Provide the configure options '--with-system-kpathsea' (if in the TL tree),
# '--with-kpathsea-includes', and '--with-kpathsea-libdir'.
#
# Set the make variables KPATHSEA_INCLUDES and KPATHSEA_LIBS to the CPPFLAGS and
# LIBS required for the `-lkpathsea' library in texk/kpathsea/ of the TL tree.
AC_DEFUN([KPSE_KPATHSEA_FLAGS],
[_KPSE_LIB_FLAGS([kpathsea], [kpathsea],
                 [$1],
                 [-IBLD/texk -ISRC/texk],
                 [BLD/texk/kpathsea/libkpathsea.la], [lt],
                 [${top_srcdir}/../kpathsea/*.[ch]],
                 [${top_builddir}/../kpathsea/paths.h])[]dnl
]) # KPSE_KPATHSEA_FLAGS

# KPSE_KPATHSEA_SYSTEM_FLAGS
# --------------------------
AC_DEFUN([KPSE_KPATHSEA_SYSTEM_FLAGS],
[_KPSE_LIB_FLAGS_SYSTEM([kpathsea], [kpathsea])[]dnl
]) # KPSE__SYSTEM_FLAGS
