# Public macros for the teTeX / TeX Live (TL) tree.
# Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 0

# KPSE_ICU_FLAGS()
# ----------------
# Set the make variables ICU_INCLUDES and ICU_LIBS to
# the CPPFLAGS and LIBS required for the icu-xetex libraries in
# libs/icu/ of the TL tree.
AC_DEFUN([KPSE_ICU_FLAGS],
[_KPSE_LIB_FLAGS([icu], [sicuxxx], [],
                 [-IBLD/libs/icu/icu-build/common -ISRC/libs/icu/icu-xetex/common -ISRC/libs/icu/icu-xetex/layout],
                 [BLD/libs/icu/icu-build/lib/libsicuuc.a BLD/libs/icu/icu-build/lib/libsicule.a BLD/libs/icu/icu-build/lib/libsicudata.a],
                 [tree], [],
                 [${top_builddir}/../../libs/icu/icu-build/common/unicode/platform.h])[]dnl
]) # KPSE_ICU_FLAGS
