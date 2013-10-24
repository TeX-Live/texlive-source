# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_ZLIB_FLAGS
# ---------------
# Provide the configure options '--with-system-zlib' (if in the TL tree),
# '--with-zlib-includes', and '--with-zlib-libdir'.
#
# Also test if <zlib.h> defines 'z_const' and define ZLIB_CONST or z_const.
#
# Set the make variables ZLIB_INCLUDES and ZLIB_LIBS to the CPPFLAGS and
# LIBS required for the `-lz' library in libs/zlib/ of the TL tree.
AC_DEFUN([KPSE_ZLIB_FLAGS], [dnl
AC_REQUIRE([KPSE_SAVE_FLAGS])[]dnl
_KPSE_LIB_FLAGS([zlib], [z], [],
                [-IBLD/libs/zlib/include], [BLD/libs/zlib/libz.a], [],
                [], [${top_builddir}/../../libs/zlib/include/zconf.h])[]dnl
AC_CACHE_CHECK([if <zlib.h> defines 'z_const'],
               [kpse_cv_have_decl_z_const],
               [KPSE_ADD_FLAGS([zlib])
                AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <zlib.h>]],
                                                   [[z_const char * foo();]])],
                                  [kpse_cv_have_decl_z_const=yes],
                                  [kpse_cv_have_decl_z_const=no])
                KPSE_RESTORE_FLAGS])
AS_CASE([$kpse_cv_have_decl_z_const],
        [yes], [AC_DEFINE([ZLIB_CONST], [1], [Define to 1 if <zlib.h> declares 'z_const'.])],
        [AC_DEFINE([z_const], [], [Define as empty if not declared in <zlib.h>.])])
]) # KPSE_ZLIB_FLAGS

# KPSE_ZLIB_OPTIONS([WITH-SYSTEM])
# --------------------------------
AC_DEFUN([KPSE_ZLIB_OPTIONS], [_KPSE_LIB_OPTIONS([zlib], [$1])])

# KPSE_ZLIB_SYSTEM_FLAGS
# ----------------------
AC_DEFUN([KPSE_ZLIB_SYSTEM_FLAGS], [_KPSE_LIB_FLAGS_SYSTEM([zlib], [z])])
