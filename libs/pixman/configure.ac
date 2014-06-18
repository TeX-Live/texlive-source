dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2012-2014 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define pixman_version
AC_INIT([pixman (TeX Live)], pixman_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([pixman-]pixman_version[/pixman/pixman.h])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

KPSE_BASIC([pixman])

KPSE_LIB_VERSION([pixman])[]dnl
AC_SUBST([PIXMAN_VERSION_MAJOR], [pixman_major])[]dnl
AC_SUBST([PIXMAN_VERSION_MINOR], [pixman_minor])[]dnl
AC_SUBST([PIXMAN_VERSION_MICRO], [pixman_micro])[]dnl

test_CFLAGS=${CFLAGS+set} # We may override autoconf default CFLAGS.

AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_LN_S
AC_CHECK_FUNCS([getisax])
AC_C_BIGENDIAN
AC_C_INLINE

AC_CONFIG_HEADERS([config.h])

AM_CONDITIONAL([build], [test "x$enable_build" != xno])

if :; then
  AC_DEFINE([PIXMAN_NO_TLS], 1, [We do not want threading])
else
dnl add all these to config.h.in but do not define them in config.h
  AC_DEFINE([USE_LOONGSON_MMI], 1, [use Loongson Multimedia Instructions])
  AC_DEFINE([USE_X86_MMX], 1, [use x86 MMX compiler intrinsics])
  AC_DEFINE([USE_SSE2], 1, [use SSE2 compiler intrinsics])
  AC_DEFINE([USE_SSSE3], 1, [use SSSE3 compiler intrinsics])
  AC_DEFINE([USE_VMX], 1, [use VMX compiler intrinsics])
  AC_DEFINE([USE_ARM_SIMD], 1, [use ARM SIMD assembly optimizations])
  AC_DEFINE([USE_ARM_NEON], 1, [use ARM NEON assembly optimizations])
  AC_DEFINE([USE_ARM_IWMMXT], 1, [use ARM IWMMXT compiler intrinsics])
  AC_DEFINE([USE_MIPS_DSPR2], 1, [use MIPS DSPr2 assembly optimizations])
  AC_DEFINE([USE_GCC_INLINE_ASM], 1, [use GNU-style inline assembler])
  AC_DEFINE([PIXMAN_TIMERS], 1, [enable TIMER_BEGIN/TIMER_END macros])
  AC_DEFINE([TOOLCHAIN_SUPPORTS_ATTRIBUTE_CONSTRUCTOR], 1,
            [Whether the tool chain supports __attribute__((constructor))])
  AC_DEFINE([HAVE_FLOAT128], [], [Whether the tool chain supports __float128])
  AC_DEFINE([HAVE_BUILTIN_CLZ], [], [Whether the compiler supports __builtin_clz])
fi

dnl Check for missing sqrtf() as, e.g., for Solaris 9
AC_SEARCH_LIBS([sqrtf], [m], ,
               [AC_DEFINE([sqrtf], [sqrt],
                          [Define to sqrt if you do not have the `sqrtf' function.])
                AC_SEARCH_LIBS([sqrt], [m])])

dnl PIXMAN_LINK_WITH_ENV(env-setup, program, true-action, false-action)
dnl
dnl Compiles and links the given program in the environment setup by env-setup
dnl and executes true-action on success and false-action on failure.
AC_DEFUN([PIXMAN_LINK_WITH_ENV],[dnl
	save_CFLAGS="$CFLAGS"
	save_LDFLAGS="$LDFLAGS"
	save_LIBS="$LIBS"
	CFLAGS=""
	LDFLAGS=""
	LIBS=""
	$1
	CFLAGS="$save_CFLAGS $CFLAGS"
	LDFLAGS="$save_LDFLAGS $LDFLAGS"
	LIBS="$save_LIBS $LIBS"
	AC_LINK_IFELSE(
		[AC_LANG_SOURCE([$2])],
		[pixman_cc_stderr=`test -f conftest.err && cat conftest.err`
		 pixman_cc_flag=yes],
		[pixman_cc_stderr=`test -f conftest.err && cat conftest.err`
		 pixman_cc_flag=no])

	if test "x$pixman_cc_stderr" != "x"; then
		pixman_cc_flag=no
	fi

	if test "x$pixman_cc_flag" = "xyes"; then
		ifelse([$3], , :, [$3])
	else
		ifelse([$4], , :, [$4])
	fi
	CFLAGS="$save_CFLAGS"
	LDFLAGS="$save_LDFLAGS"
	LIBS="$save_LIBS"
])

KPSE_CHECK_SIZE_MAX

dnl Find a -Werror for catching warnings.
WERROR=
for w in -Werror -errwarn; do
    if test "z$WERROR" = "z"; then
        AC_MSG_CHECKING([whether the compiler supports $w])
        PIXMAN_LINK_WITH_ENV(
		[CFLAGS=$w],
		[int main(int c, char **v) { (void)c; (void)v; return 0; }],
		[WERROR=$w; yesno=yes], [yesno=no])
	AC_MSG_RESULT($yesno)
    fi
done

dnl PIXMAN_CHECK_CFLAG(flag, [program])
dnl  Adds flag to CFLAGS if the given program links without warnings or errors.
AC_DEFUN([PIXMAN_CHECK_CFLAG], [dnl
	AC_MSG_CHECKING([whether the compiler supports $1])
	PIXMAN_LINK_WITH_ENV(
		[CFLAGS="$WERROR $1"],
		[$2
		 int main(int c, char **v) { (void)c; (void)v; return 0; }
		],
		[_yesno=yes],
		[_yesno=no])
	if test "x$_yesno" = xyes; then
	   CFLAGS="$CFLAGS $1"
	fi
	AC_MSG_RESULT($_yesno)
])

AC_CHECK_SIZEOF([long])

# Checks for Sun Studio compilers
AC_CHECK_DECL([__SUNPRO_C], [SUNCC="yes"], [SUNCC="no"])
AC_CHECK_DECL([__amd64], [AMD64_ABI="yes"], [AMD64_ABI="no"])

# Default CFLAGS to -O -g rather than just the -g from AC_PROG_CC
# if we're using Sun Studio and neither the user nor a config.site
# has set CFLAGS.
if test $SUNCC = yes &&			\
   test "x$test_CFLAGS" = "x" &&	\
   test "$CFLAGS" = "-g"
then
  CFLAGS="-O -g"
fi

# Check for dependencies

PIXMAN_CHECK_CFLAG([-Wall])
PIXMAN_CHECK_CFLAG([-fno-strict-aliasing])

AC_SUBST([PIXMAN_TREE], [pixman-]pixman_version)

if test -f $srcdir/$PIXMAN_TREE/pixman/pixman-version.h; then
  AC_MSG_ERROR([Sorry, you must remove the file $PIXMAN_TREE/pixman/pixman-version.h])
fi

AC_CONFIG_FILES([Makefile
		 include/Makefile
	         pixman-version.h:pixman-]pixman_version[/pixman/pixman-version.h.in])

AC_OUTPUT
