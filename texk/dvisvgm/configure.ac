dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
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
m4_define([dvisvgm_version], [0.8.7])[]dnl using unmodified dvisvgm source tree
AC_INIT([dvisvgm], dvisvgm_version, [tex-k@tug.org])
AC_PREREQ([2.63])
AC_CONFIG_SRCDIR([dvisvgm-]dvisvgm_version[/src/dvisvgm.cpp])
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

# For the moment we just disable the PostScript support.
AC_DEFINE([DISABLE_GS], 1,
          [Set to 1 if PostScript support should be disabled])

AC_DEFINE_UNQUOTED([TARGET_SYSTEM], ["$target"],
                   [The machine triplet of the target system])

AC_SEARCH_LIBS([sqrt], [m])

KPSE_KPATHSEA_FLAGS
KPSE_ZLIB_FLAGS
KPSE_FREETYPE2_FLAGS

if test "x$enable_build" != xno || test -f config.force; then

# Checks for more libraries.
KPSE_ADD_FLAGS([zlib])
AC_CHECK_FUNC([gzopen],
              [AC_DEFINE([HAVE_LIBZ], 1,
                         [Define to 1 if you have the `z' library (-lz).])],
              [AC_MSG_ERROR([cannot find/use zlib])])

KPSE_ADD_FLAGS([freetype2])
AC_CHECK_FUNC([FT_Init_FreeType],
              [AC_DEFINE([HAVE_LIBFREETYPE], 1,
                         [Define to 1 if you have the `freetype2' library (-lfreetype).])],
              [AC_MSG_ERROR([cannot find/use libfreetype])])

KPSE_ADD_FLAGS([kpathsea])
AC_CHECK_FUNC([kpse_set_progname],
              [AC_DEFINE([HAVE_LIBKPATHSEA], 1,
                         [Define to 1 if you have the `kpathsea' library (-lkpathsea).])],
              [AC_MSG_ERROR([cannot find/use libkpathsea])])

KPSE_RESTORE_FLAGS

echo timestamp >config.force
fi

DVISVGM_TREE=[dvisvgm-]dvisvgm_version
AC_SUBST([DVISVGM_TREE])

AC_LANG_PUSH([C++])
CPPFLAGS="-I$srcdir/$DVISVGM_TREE/src $CPPFLAGS"

AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include "iapi.h"]],
                                   [[typedef struct gs_main_inst_s gs_main_inst; gs_main_inst *inst;]])],
                  [gs_old_api=yes],
                  [gs_old_api=no])
AC_MSG_RESULT([gs_old_api=$gs_old_api])
AC_LANG_POP([C++])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
