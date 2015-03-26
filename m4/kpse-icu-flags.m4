# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2009-2015 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_ICU_FLAGS([MORE-ICU-LIBS], [ICU_CONFIG_ARGS])
# --------------------------------------------------
# Provide the configure option '--with-system-icu' (if in the TL tree).
#
# ICU_CONFIG_ARGS: icu-config arguments for additional icu libraries. 
#
# Set the make variables ICU_INCLUDES and ICU_LIBS to the CPPFLAGS and
# LIBS required for the icu libraries in libs/icu/ of the TL tree.
AC_DEFUN([KPSE_ICU_FLAGS],
[m4_pushdef([kpse_icu_config_args], [$2])[]dnl
_KPSE_ICU_FLAGS([icuxxx], [], [$1])[]dnl
m4_popdef([kpse_icu_config_args])[]dnl
]) # KPSE_ICU_FLAGS

# _KPSE_ICU_FLAGS(LIBNAME, OPTIONS, [MORE-ICU-LIBS])
# --------------------------------------------------
# Internal subroutine.
#
# LIBNAME and OPTIONS as for _KPSE_LIB_FLAGS().
# MORE-ICU-LIBS: icu libraries from the TL tree in addition to icuuc and icudata.
m4_define([_KPSE_ICU_FLAGS], [dnl
_KPSE_LIB_FLAGS([icu], [$1], [$2],
                [-DU_STATIC_IMPLEMENTATION -IBLD/libs/icu/include],
                m4_bpatsubst([$3 icuuc icudata],
                             [icu\([18a-z]*\)],
                             [BLD/libs/icu/icu-build/lib/libicu\1.a]),
                [],
                [], [${top_builddir}/../../libs/icu/include/unicode/uversion.h])[]dnl
]) # _KPSE_ICU_FLAGS

# KPSE_ICU_OPTIONS([WITH-SYSTEM])
# -------------------------------
AC_DEFUN([KPSE_ICU_OPTIONS], [dnl
m4_ifval([$1],
         [AC_ARG_WITH([system-icu],
                      AS_HELP_STRING([--with-system-icu],
                                     [use installed ICU headers and libraries (requires icu-config)]))])[]dnl
]) # KPSE_ICU_OPTIONS

# KPSE_ICU_SYSTEM_FLAGS
# ---------------------
AC_DEFUN([KPSE_ICU_SYSTEM_FLAGS], [dnl
AC_REQUIRE([AC_CANONICAL_HOST])[]dnl
AC_CHECK_TOOL([ICU_CONFIG], [icu-config], [false])[]dnl
if $ICU_CONFIG --version >/dev/null 2>&1; then
  ICU_INCLUDES=`$ICU_CONFIG --cppflags`
  ICU_LIBS=`$ICU_CONFIG --ldflags-searchpath m4_ifset([kpse_icu_config_args],
                                                      [kpse_icu_config_args ])--ldflags-libsonly --ldflags-system`
elif test "x$need_icu:$with_system_icu" = xyes:yes; then
  AC_MSG_ERROR([did not find icu-config required for system icu libraries])
fi
]) # KPSE_ICU_SYSTEM_FLAGS
