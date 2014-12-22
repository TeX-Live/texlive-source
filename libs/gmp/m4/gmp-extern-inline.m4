# Autoconf macros for the GNU MP Library.
# Copyright (C) 2000-2014 Free Software Foundation, Inc.
#
# Copyright (C) 2014 Peter Breitenlohner <tex-live@tug.org>
# Extracted from gmp-6.0.0/acinclude.m4 and adapted for TeX Live.
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

#  GMP_H_EXTERN_INLINE
#  -------------------
#  If the compiler has an "inline" of some sort, check whether the
#  #ifdef's in gmp.h recognise it.

AC_DEFUN([GMP_H_EXTERN_INLINE], [dnl
AC_REQUIRE([AC_C_INLINE])
AS_CASE([$ac_cv_c_inline],
        [no], [],
        [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#define __GMP_WITHIN_CONFIGURE_INLINE 1
#include "gmp-tmp.h"
#ifndef __GMP_EXTERN_INLINE
die die die
#endif]],
                                            [])],
                           [], [dnl
AS_CASE([$ac_cv_c_inline],
        [yes], [tmp_inline=inline],
               [tmp_inline=$ac_cv_c_inline])
AC_MSG_WARN([gmp.h doesnt recognise compiler "$tmp_inline", inlines will be unavailable])])])
])


