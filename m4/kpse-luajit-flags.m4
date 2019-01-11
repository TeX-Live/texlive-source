# $Id$
# Public macros for the TeX Live (TL) tree.
# Copyright 2019 Karl Berry <tex-live@tug.org>
# Copyright 2014 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_LUAJIT_FLAGS
# -----------------
# Set the make variables LUAJIT_INCLUDES and LUAJIT_LIBS to the CPPFLAGS and
# LIBS required for the `-ltexluajit' library in libs/luajit/ of the TL tree.
AC_DEFUN([KPSE_LUAJIT_FLAGS], [dnl
echo 'tldbg:[$0] called.' >&AS_MESSAGE_LOG_FD
_KPSE_LIB_FLAGS([luajit], [texluajit], [lt tree],
                [-IBLD/libs/luajit/include],
                [BLD/libs/luajit/libtexluajit.la],
                [],
                [],
                [${top_builddir}/../../libs/luajit/include/luajit.h])[]dnl
#
# checking for openbsd for additional jit libraries needed, which is the
# case for clang; no point in going to the trouble elsewhere since no
# other system needs it.
case $build_os in
openbsd*)
AC_MSG_CHECKING([on openbsd if additional jit libraries are needed])
AC_LINK_IFELSE(
  [AC_LANG_PROGRAM(
    [[#include "stdint.h"
      typedef struct _Unwind_Context _Unwind_Context;
      extern uintptr_t _Unwind_GetCFA(_Unwind_Context *);]],
    [[_Unwind_Context *ctx;
      _Unwind_GetCFA(ctx);]]
  )],
  [AC_MSG_RESULT([no])],
  [
    jitlibs="-lc++abi -lpthread"
    save_LIBS=$LIBS
    LIBS="$LIBS $jitlibs"
    AC_LINK_IFELSE(
      [AC_LANG_PROGRAM(
        [[#include "stdint.h"
          typedef struct _Unwind_Context _Unwind_Context;
          extern uintptr_t _Unwind_GetCFA(_Unwind_Context *);]],
        [[_Unwind_Context *ctx;
          _Unwind_GetCFA(ctx);]]
      )],
      [
        AC_MSG_RESULT(["$jitlibs"])
        LIBLUAJIT_LDEXTRA="$LIBLUAJIT_LDEXTRA $jitlibs"
      ],
      [AC_MSG_FAILURE([luajit could not be linked])]
    )
    LIBS=$save_LIBS
  ]
)
  ;;
esac
echo 'tldbg:[$0] done.' >&AS_MESSAGE_LOG_FD
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
