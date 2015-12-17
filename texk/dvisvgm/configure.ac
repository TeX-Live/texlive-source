dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009-2014 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
dnl *********************************************************************
dnl
dnl  Adapted for TeX Live from dvisvgm-0.8.7/configure.ac
dnl  Copyright (C) 2005-2009 Martin Gieseking <martin.gieseking@uos.de>
dnl
dnl *********************************************************************
dnl
m4_include([version.ac])[] dnl define dvisvgm_version
AC_INIT([dvisvgm (TeX Live)], dvisvgm_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([dvisvgm-src/src/dvisvgm.cpp])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

# Common code for all programs using libkpathsea.
KPSE_COMMON([dvisvgm])

dnl Include additional code for dvisvgm.
m4_include([ac/dvisvgm.ac])

AC_CONFIG_HEADERS([config.h])

AC_PROG_CXX
AC_PROG_CXXCPP
KPSE_CXX_HACK

AC_DEFINE_UNQUOTED([TARGET_SYSTEM], ["$host"],
                   [The machine triplet of the target system.])

AC_SEARCH_LIBS([sqrt], [m])

AC_CHECK_FUNCS([ftime gettimeofday])
AC_CHECK_HEADERS([sys/time.h sys/timeb.h])

KPSE_COND_WIN32

KPSE_KPATHSEA_FLAGS
KPSE_ZLIB_FLAGS
KPSE_FREETYPE2_FLAGS
KPSE_POTRACE_FLAGS

KPSE_SAVE_FLAGS

AS_IF([test "x$with_libgs" = xno], [libgs_mode=no],
      [dnl
if test "x$with_system_libgs" = xyes; then
  libgs_mode=link
  AS_CASE([$with_libgs_includes],
          ["" | yes | no], [LIBGS_INCLUDES=],
            [LIBGS_INCLUDES="-I$with_libgs_includes"])
  AS_CASE([$with_libgs_libdir],
          ["" | yes | no], [LIBGS_LIBS=-lgs],
            [LIBGS_LIBS="-L$with_libgs_libdir -lgs"])
  KPSE_ADD_FLAGS([libgs])
  AC_CHECK_HEADER([ghostscript/iapi.h])
  AC_CHECK_FUNC([gsapi_new_instance])
  if test "x$ac_cv_header_ghostscript_iapi_h:$ac_cv_func_gsapi_new_instance" != xyes:yes; then
    AC_MSG_ERROR([cannot find/use libgs])
  fi
  AC_CACHE_CHECK([for Ghostscript version 8.31 or later],
                 [kpse_cv_new_gsapi],
                 [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <ghostscript/iapi.h>]],
                                                     [[gs_main_inst *inst;]])],
                                    [kpse_cv_new_gsapi=no],
                                    [kpse_cv_new_gsapi=yes])])
  if test "x$kpse_cv_new_gsapi" != xyes; then
    AC_MSG_ERROR([dvisvgm requires Ghostscript version 8.31 or later])
  fi
  AC_DEFINE([HAVE_LIBGS], 1,
            [Define to 1 if you have the `gs' library (-lgs).])
else
  libgs_mode=load
  LIBGS_INCLUDES=
  LIBGS_LIBS=
  # Windows (native or MinGW32) has neither <dlfcn.h> nor dlopen().
  if test "x$kpse_cv_have_win32" = xno; then
    # FreeBSD neither has nor requires libdl.
    AC_SEARCH_LIBS([dlopen], [dl])
    AS_CASE([$ac_cv_search_dlopen],
            [no | "none required"], [],
                [LIBGS_LIBS=$ac_cv_search_dlopen])
    AC_CHECK_HEADER([dlfcn.h])
    AC_CHECK_FUNC([dlopen])
    if test "x$ac_cv_header_dlfcn_h:$ac_cv_func_dlopen" != xyes:yes; then
      libgs_mode=no
      KPSE_RESTORE_FLAGS
      LIBGS_LIBS=
    fi
  fi
fi])
if test "x$libgs_mode" = xno; then
  AC_DEFINE([DISABLE_GS], 1,
            [Set to 1 if PostScript support should be disabled.])
fi
AC_SUBST([LIBGS_INCLUDES])
AC_SUBST([LIBGS_LIBS])
AM_CONDITIONAL([dlloader], [test "x$libgs_mode" = xload])

KPSE_SAVE_FLAGS

if test "x$enable_build" != xno || test -f config.force; then

# Checks for more libraries.
KPSE_ADD_FLAGS([zlib])
AC_CHECK_FUNC([gzopen], [],
              [AC_MSG_ERROR([cannot find/use zlib])])

KPSE_ADD_FLAGS([freetype2])
AC_CHECK_FUNC([FT_Init_FreeType], [],
              [AC_MSG_ERROR([cannot find/use libfreetype])])

KPSE_ADD_FLAGS([kpathsea])
AC_CHECK_FUNC([kpse_set_program_name], [],
              [AC_MSG_ERROR([cannot find/use libkpathsea])])

# Check if the kpathsea headers are C++ safe.
AC_LANG_PUSH([C++])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <kpathsea/kpathsea.h>]],
                                [[printf("%s\n", concat("one", "two"))]])],
               [],
               [AC_MSG_ERROR([Sorry, your kpathsea headers are too old])
                AC_DEFINE([KPSE_CXX_UNSAFE], 1,
                          [Define to 1 if the kpathsea headers are not C++ safe.])])
AC_LANG_POP([C++])

echo timestamp >config.force
fi

KPSE_RESTORE_FLAGS

AC_DEFINE([HAVE_LIBZ], 1, [Define to 1 if you have the `z' library (-lz).])
AC_DEFINE([HAVE_LIBFREETYPE], 1, [Define to 1 if you have the `freetype2' library (-lfreetype).])
AC_DEFINE([HAVE_LIBKPATHSEA], 1, [Define to 1 if you have the `kpathsea' library (-lkpathsea).])
if false; then
  AC_DEFINE([KPSE_CXX_UNSAFE], 1, [Define to 1 if the kpathsea headers are not C++ safe.])
fi

AC_SUBST([DVISVGM_TREE], [dvisvgm-src])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
