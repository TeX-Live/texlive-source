# $Id$
# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2022-2023 Luigi Scarso <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 0

# KPSE_POTRACE_FLAGS
# ------------------
# Provide the configure options '--with-system-potrace' (if in the TL tree),
# '--with-potrace-includes', and '--with-potrace-libdir'.
#
# Set the make variables POTRACE_INCLUDES and POTRACE_LIBS to the CPPFLAGS and
# LIBS required for the `-lpotrace' library in libs/potrace/ of the TL tree.
AC_DEFUN([KPSE_POTRACE_FLAGS], [dnl
AC_REQUIRE([KPSE_SAVE_FLAGS])[]dnl
_KPSE_LIB_FLAGS([potrace], [potrace], [],
                [-IBLD/libs/potrace/include], [BLD/libs/potrace/libpotrace.a], [],
                [], [${top_builddir}/../../libs/potrace/include/potrace.h])[]dnl
]) # KPSE_POTRACE_FLAGS

# KPSE_POTRACE_OPTIONS([WITH-SYSTEM])
# ----------------------------------
AC_DEFUN([KPSE_POTRACE_OPTIONS], [_KPSE_LIB_OPTIONS([potrace], [$1])])

# KPSE_POTRACE_SYSTEM_FLAGS
# ------------------------
AC_DEFUN([KPSE_POTRACE_SYSTEM_FLAGS], [_KPSE_LIB_FLAGS_SYSTEM([potrace], [potrace])])
