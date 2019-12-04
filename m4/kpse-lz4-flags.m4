# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2013, 2014 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_LZ4_FLAGS
# ----------------
# Set the make variables LZ4_INCLUDES and LZ4_LIBS to the CPPFLAGS and
# LIBS required for the `-llz4' library in libs/lz4/ of the TL tree.
AC_DEFUN([KPSE_LZ4_FLAGS], [dnl
echo 'tldbg:[$0] called.' >&AS_MESSAGE_LOG_FD
_KPSE_LIB_FLAGS([lz4], [lz4], [tree],
                [-IBLD/libs/lz4/lz4-src/lib], [BLD/libs/lz4/liblz4.a], [],
                [], [${top_builddir}/../../libs/lz4/lz4-src/lib/lz4.h])[]dnl
]) # KPSE_LZ4_FLAGS
