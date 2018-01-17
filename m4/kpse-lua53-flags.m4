# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2013, 2014 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_LUA53_FLAGS
# ----------------
# Set the make variables LUA53_INCLUDES and LUA53_LIBS to the CPPFLAGS and
# LIBS required for the `-ltexlua53' library in libs/lua53/ of the TL tree.
AC_DEFUN([KPSE_LUA53_FLAGS], [dnl
_KPSE_LIB_FLAGS([lua53], [texlua53], [lt tree],
                [-IBLD/libs/lua53/include], [BLD/libs/lua53/libtexlua53.la], [],
                [], [${top_builddir}/../../libs/lua53/include/lua.h])[]dnl
]) # KPSE_LUA53_FLAGS

# KPSE_LUA53_DEFINES
# ------------------
# Set the make variable LUA53_DEFINES to the CPPFLAGS required when
# compiling or using the `-ltexlua53' library.
AC_DEFUN([KPSE_LUA53_DEFINES], [dnl
AC_REQUIRE([KPSE_CHECK_WIN32])[]dnl
AC_SUBST([LUA53_DEFINES], ['-DLUA_COMPAT_MODULE -DLUA_COMPAT_5_2 -DLUAI_HASHLIMIT=6'])
if test "x$kpse_cv_have_win32" = xno; then
  LUA53_DEFINES="$LUA53_DEFINES -DLUA_USE_POSIX"
  AC_SEARCH_LIBS([dlopen], [dl])
  if test "x$ac_cv_search_dlopen" != xno; then
    AC_CHECK_HEADER([dlfcn.h],
                    [LUA53_DEFINES="$LUA53_DEFINES -DLUA_USE_DLOPEN"],
                    [], [AC_INCLUDES_DEFAULT])
  fi
fi
]) # KPSE_LUA53_DEFINES
