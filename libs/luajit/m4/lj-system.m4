# Autoconf macros for luajit.
# Copyright (C) 2014, 2015 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# _LJ_ARCH
# --------
# Internal subroutine.
# Preprocess <lj_arch.h> and extract system characteristics.
m4_define([_LJ_ARCH], [dnl
rm -f dynasm_flags native_flags
AC_MSG_CHECKING([for architecture])
cp conftest.i system_flags
LUAJIT_CFLAGS='-fomit-frame-pointer'
AS_IF([grep 'LJ_TARGET_X64 ' conftest.i >/dev/null 2>&1],
        [LJARCH=x64],
      [grep 'LJ_TARGET_X86 ' conftest.i >/dev/null 2>&1],
        [LJARCH=x86
         LUAJIT_CFLAGS="$LUAJIT_CFLAGS -march=i686 -msse -msse2 -mfpmath=sse"],
      [grep 'LJ_TARGET_ARM ' conftest.i >/dev/null 2>&1],
        [LJARCH=arm],
      [grep 'LJ_TARGET_ARM64 ' conftest.i >/dev/null 2>&1],
        [LJARCH=arm64
         AS_IF([test "x$LJHOST" = xiOS],
               [LUAJIT_CFLAGS='-fno-omit-frame-pointer'])],
      [grep 'LJ_TARGET_PPC ' conftest.i >/dev/null 2>&1],
        [LJARCH=ppc
         AS_IF([grep 'LJ_LE 1' conftest.i >/dev/null 2>&1],
                 [echo '-DLJ_ARCH_ENDIAN=LUAJIT_LE' >>native_flags],
                 [echo '-DLJ_ARCH_ENDIAN=LUAJIT_BE' >>native_flags])],
      [grep 'LJ_TARGET_MIPS ' conftest.i >/dev/null 2>&1],
        [LJARCH=mips
         AS_IF([grep 'MIPSEL ' conftest.i >/dev/null 2>&1],
                 [echo '-D__MIPSEL__=1' >>native_flags])],
        [AC_MSG_ERROR([Sorry, unsupported architecture])])
AS_IF([grep 'LJ_TARGET_PS3 1'conftest.i >/dev/null 2>&1],
        [LJHOST='PS3'
         echo '-D__CELLOS_LV2__' >>native_flags
         LUAJIT_DEFINES="$LUAJIT_DEFINES -DLUAJIT_USE_SYSMALLOC"])
AS_IF([grep 'LJ_NO_UNWIND 1' conftest.i >/dev/null 2>&1],
        [echo '-D NO_UNWIND' >>dynasm_flags
         echo '-DLUAJIT_NO_UNWIND' >>native_flags])
echo "-DLUAJIT_TARGET=LUAJIT_ARCH_$LJARCH" >>native_flags
AS_IF([grep 'LJ_ARCH_BITS 64' conftest.i >/dev/null 2>&1],
        [echo '-D P64' >>dynasm_flags
         AS_IF([test "x$LJHOST" = xLinux],
               [LUAJIT_DEFINES="$LUAJIT_DEFINES -DMAP_32BIT=0x40"])])
AS_IF([grep 'LJ_HASJIT 1' conftest.i >/dev/null 2>&1],
        [echo '-D JIT' >>dynasm_flags])
AS_IF([grep 'LJ_HASFFI 1' conftest.i >/dev/null 2>&1],
        [echo '-D FFI' >>dynasm_flags])
AS_IF([grep 'LJ_DUALNUM 1' conftest.i >/dev/null 2>&1],
        [echo '-D DUALNUM' >>dynasm_flags])
AS_IF([grep 'LJ_ARCH_HASFPU 1' conftest.i >/dev/null 2>&1],
        [echo '-D FPU' >>dynasm_flags
         echo '-DLJ_ARCH_HASFPU=1' >>native_flags],
        [echo '-DLJ_ARCH_HASFPU=0' >>native_flags])
AS_IF([grep 'LJ_ABI_SOFTFP 1' conftest.i >/dev/null 2>&1],
        [echo '-DDLJ_ABI_SOFTFP=1' >>native_flags],
        [echo '-D HFABI' >>dynasm_flags
         echo '-DLJ_ABI_SOFTFP=0' >>native_flags])
echo '-D VER='`grep 'LJ_ARCH_VERSION ' conftest.i 2>&1 | \
               sed 's/^.*LJ_ARCH_VERSION //'` >>dynasm_flags
AS_IF([test "x$LJHOST" = xWindows],
      [echo '-D WIN' >>dynasm_flags])
DASM_ARCH=$LJARCH
AS_CASE([$LJARCH],
        [x64], [AS_IF([grep 'LJ_FR2 1' conftest.i >/dev/null 2>&1],
                      [], [DASM_ARCH=x86])],
        [arm], [AS_IF([test "x$LJHOST" = xiOS],
                      [echo '-D IOS' >>dynasm_flags])],
        [ppc], [AS_IF([grep 'LJ_ARCH_SQRT 1' conftest.i >/dev/null 2>&1],
                      [echo '-D SQRT' >>dynasm_flags])
                AS_IF([grep 'LJ_ARCH_ROUND 1' conftest.i >/dev/null 2>&1],
                      [echo '-D ROUND' >>dynasm_flags])
                AS_IF([grep 'LJ_ARCH_PPC32ON64 1' conftest.i >/dev/null 2>&1],
                      [echo '-D GPR64' >>dynasm_flags])
                AS_IF([grep 'LJ_ARCH_PPC64 ' conftest.i >/dev/null 2>&1],
                      [DASM_ARCH=ppc64])
                AS_IF([test "x$LJHOST" = xPS3],
                      [echo '-D PPE -D TOC' >>dynasm_flags])])
AS_CASE([$LJHOST],
        [Windows], [echo '-DLUAJIT_OS=LUAJIT_OS_WINDOWS' >>native_flags],
        [Darwin | iOS], [echo '-DLUAJIT_OS=LUAJIT_OS_OSX' >>native_flags],
        [Linux], [echo '-DLUAJIT_OS=LUAJIT_OS_LINUX' >>native_flags],
                 [echo '-DLUAJIT_OS=LUAJIT_OS_OTHER' >>native_flags])
AC_MSG_RESULT([$LJHOST $LJARCH $DASM_ARCH])
]) # _LJ_ARCH

# LJ_ARCH
# -------
# Determine host and build system characteristics.
AC_DEFUN([LJ_ARCH], [dnl
lj_save_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$CPPFLAGS -I$srcdir/LuaJIT-src/src $LUAJIT_DEFINES -dM"
AC_PREPROC_IFELSE([AC_LANG_SOURCE([[#include <lj_arch.h>]])],
                  [lj_cpp=ok; _LJ_ARCH])
CPPFLAGS=$lj_save_CPPFLAGS
AC_SUBST([LJARCH])
AC_SUBST([DASM_ARCH])
]) # LJ_ARCH

