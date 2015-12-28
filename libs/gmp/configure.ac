dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2014 Taco Hoekwater <taco@metatex.org>
dnl   Copyright (C) 2014 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define gmp_version
AC_INIT([gmp (TeX Live)], gmp_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([gmp-src/gmp-impl.h])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIRS([../../m4 m4])

KPSE_BASIC([gmp])

AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_LN_S

KPSE_COMPILER_VISIBILITY

AC_PROG_CC_C99
CFLAGS="$CFLAGS -DNO_ASM"

AC_CONFIG_HEADERS([config.h])

KPSE_CANONICAL_HOST

AC_CHECK_FUNCS([memset raise strchr])

GMP_C_ATTRIBUTE_CONST
GMP_C_ATTRIBUTE_MALLOC
GMP_C_ATTRIBUTE_MODE
GMP_C_ATTRIBUTE_NORETURN

AC_CHECK_TYPE([long long])

AC_CHECK_SIZEOF([void *])
AC_CHECK_SIZEOF([unsigned short])
AC_CHECK_SIZEOF([unsigned])
AC_CHECK_SIZEOF([unsigned long])
AC_CHECK_SIZEOF([unsigned long long])

AS_CASE([$host],
        [x86_64-*-cygwin], [gmp_limb=longlong], [dnl
AS_IF([test $ac_cv_sizeof_void_p -gt $ac_cv_sizeof_unsigned_long], [dnl
AS_IF([test $ac_cv_sizeof_void_p -le $ac_cv_sizeof_unsigned_long_long],
      [gmp_limb=longlong],
      [gmp_limb=unknown])
])
])
AS_CASE([$gmp_limb],
        [longlong],
          [DEFN_LONG_LONG_LIMB="@%:@define _LONG_LONG_LIMB 1"
           gmp_limb_size=$ac_cv_sizeof_unsigned_long_long],
        [DEFN_LONG_LONG_LIMB="/* @%:@undef _LONG_LONG_LIMB */"
         gmp_limb_size=$ac_cv_sizeof_unsigned_long])
AC_SUBST([DEFN_LONG_LONG_LIMB])
AC_SUBST([GMP_LIMB_BITS], [`expr 8 \* $gmp_limb_size`])

AC_SUBST([GMP_NAIL_BITS], 0)

dnl  KPSE_GMP_H
dnl  ----------
dnl  Create a temporary file gmp-tmp.h to be used instead of gmp.h, since that
dnl  file isn't generated until the end of the configure.
AC_DEFUN([KPSE_GMP_H], [dnl
cat >gmp-tmp.h <<KPSE_EOF
#define __GMP_WITHIN_CONFIGURE 1   /* ignore template stuff */
#define GMP_NAIL_BITS $GMP_NAIL_BITS
#define GMP_LIMB_BITS 123
$DEFN_LONG_LONG_LIMB
#include "$srcdir/gmp-src/gmp-h.in"
KPSE_EOF
AC_CONFIG_COMMANDS_POST([rm -rf gmp-tmp.h])
]) # KPSE_GMP_H
KPSE_GMP_H

AC_CHECK_SIZEOF([mp_limb_t], , [#include "gmp-tmp.h"])

GMP_H_EXTERN_INLINE

GMP_FUNC_ALLOCA

GMP_C_DOUBLE_FORMAT

if test "x$enable_build" != xno || test -f config.force; then

AS_CASE([$gmp_limb],
        [unknown], [AC_MSG_ERROR([Sorry, no suitable limb size])])

if test "x$ac_cv_sizeof_mp_limb_t" != "x$gmp_limb_size"; then
  AC_MSG_ERROR([Oops, mp_limb_t doesn't seem to work])
fi

echo timestamp >config.force
fi

AM_CONDITIONAL([build], [test "x$enable_build" != xno])

AC_SUBST([GMP_TREE], [gmp-src])

AC_SUBST([HAVE_HOST_CPU_FAMILY_power], 0)
AC_SUBST([HAVE_HOST_CPU_FAMILY_powerpc], 0)
AC_SUBST([DEFN_LONG_LONG_LIMB])
AC_SUBST([LIBGMP_DLL], 0)

AC_CONFIG_FILES([Makefile include/Makefile gmp.h:gmp-src/gmp-h.in])

dnl The subdirectory native must be configured for the build system.
KPSE_NATIVE_SUBDIRS([native])

AC_OUTPUT
