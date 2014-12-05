# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2014 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_LUAJIT_FLAGS
# -----------------
# Set the make variables LUAJIT_INCLUDES and LUAJIT_LIBS to the CPPFLAGS and
# LIBS required for the `-ltexluajit' library in libs/luajit/ of the TL tree.
AC_DEFUN([KPSE_LUAJIT_FLAGS], [dnl
_KPSE_LIB_FLAGS([luajit], [texluajit], [lt tree],
                [-IBLD/libs/luajit/include], [BLD/libs/luajit/libtexluajit.la], [],
                [], [${top_builddir}/../../libs/luajit/include/luajit.h])[]dnl
]) # KPSE_LUAJIT_FLAGS

# KPSE_LUAJIT_DEFINES
# -------------------
# Set the make variable LUAJIT_DEFINES to the CPPFLAGS required when
# compiling or using the `-ltexluajit' library.
# Set the make variable LUAJIT_LDEXTRA to the LDFLAGS required when
# linking with the `-lluajit' library.
AC_DEFUN([KPSE_LUAJIT_DEFINES], [dnl
AC_REQUIRE([AC_CANONICAL_HOST])[]dnl
AC_SUBST([LUAJIT_DEFINES], ['-DLUAJIT_ENABLE_LUA52COMPAT -DLUAI_HASHLIMIT=6'])
AS_CASE([$host_os:$host_cpu],
        [*darwin*:x86_64], [LUAJIT_LDEXTRA='-pagezero_size 10000 -image_base 100000000'])
AC_SUBST([LUAJIT_LDEXTRA])
]) # KPSE_LUAJIT_DEFINES
