# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2009, 2012 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_OBSDCOMPAT_FLAGS
# ---------------------
# Set the make variables OBSDCOMPAT_INCLUDES and OBSDCOMPAT_LIBS to
# the CPPFLAGS and LIBS required for the `-lobsdcompat' library in
# libs/obsdcompat/ of the TL tree.
AC_DEFUN([KPSE_OBSDCOMPAT_FLAGS],
[_KPSE_LIB_FLAGS([obsdcompat], [openbsd-compat], [tree],
                 [-IBLD/libs/obsdcompat -ISRC/libs/obsdcompat],
                 [BLD/libs/obsdcompat/libopenbsd-compat.a], [],
                 [${top_srcdir}/../../libs/obsdcompat/*.[ch]])[]dnl
]) # KPSE_OBSDCOMPAT_FLAGS
