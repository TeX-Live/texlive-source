dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_define([dvipng_version], [1.12])
dnl
#************************************************************************
#
#  Part of the dvipng distribution
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as
#  published by the Free Software Foundation, either version 3 of the
#  License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful, but
#  WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this program. If not, see
#  <http://www.gnu.org/licenses/>.
#
#  Copyright (C) 2002-2009 Jan-Åke Larsson
#
#************************************************************************
#
#  Adapted for TeX Live
#  All settings here are our fault, don't blame the dvipng maintainer for it.
#
#************************************************************************
AC_INIT([dvipng], dvipng_version, [tex-k@tug.org])
AC_PREREQ([2.63])
AC_CONFIG_SRCDIR([dvipng.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([dvipng])

dnl Checks for programs.
dnl FIXME
AC_DEFINE_UNQUOTED([GS_PATH], ["gs"], [Define as the path to GhostScript.])

dnl Checks for libraries.
AC_SEARCH_LIBS([pow], [m])
AC_SEARCH_LIBS([basename], [gen])

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h sys/time.h])
AC_HEADER_SYS_WAIT
AC_HEADER_TIME
AC_HEADER_STDBOOL

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_PID_T
AC_TYPE_SIZE_T

dnl Checks for library functions.
AC_FUNC_ALLOCA
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_STRTOD
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([dup2 memset munmap pow putenv strchr strrchr strtol strstr])

KPSE_KPATHSEA_FLAGS
KPSE_ZLIB_FLAGS
KPSE_LIBPNG_FLAGS
KPSE_T1LIB_FLAGS
KPSE_GD_FLAGS
KPSE_FREETYPE2_FLAGS

dnl Checks for more libraries.
KPSE_ADD_FLAGS([zlib])
AC_CHECK_FUNC([deflate],
              [AC_DEFINE([HAVE_LIBZ], 1,
                         [Define to 1 if you have the `z' library (-lz).])])

KPSE_ADD_FLAGS([libpng])
AC_CHECK_FUNC([png_read_image],
              [AC_DEFINE([HAVE_LIBPNG], 1,
                         [Define to 1 if you have the `png' library (-lpng).])],
              [AC_MSG_ERROR([cannot find/use libpng])])

KPSE_ADD_FLAGS([gd])
AC_CHECK_FUNC([gdImageCreate],
              [AC_DEFINE([HAVE_LIBGD], 1,
                         [Define to 1 if you have the `gd' library (-lgd).])],
              [AC_MSG_ERROR([cannot find/use libgd
This drawing library can be downloaded at http://www.boutell.com/gd])])

KPSE_ADD_FLAGS([kpathsea])
AC_CHECK_FUNC([kpse_set_progname],
              [AC_DEFINE([HAVE_LIBKPATHSEA], 1,
                         [Define to 1 if you have the `kpathsea' library (-lkpathsea).])],
              [AC_MSG_ERROR([cannot find/use libkpathsea])])

KPSE_ADD_FLAGS([t1lib])
dnl AC_CHECK_FUNC([T1_InitLib],
dnl               [AC_DEFINE([HAVE_LIBT1], 1,
dnl                          [Define to 1 if you have the `t1' library (-lt1).])])

KPSE_ADD_FLAGS([freetype2])

dnl Checks for more header files.
AC_CHECK_HEADERS([gd.h png.h kpathsea/kpathsea.h], ,
                 [AC_MSG_ERROR([cannot find/use $ac_header])])
AC_CHECK_HEADERS([t1lib.h])

dnl Checks for library functions.
AC_CHECK_FUNCS([gdImageCreateTrueColor gdImageCreateFromJpeg gdImagePngEx gdImageCreateFromPngPtr gdImageGif FT_Library_Version])

KPSE_RESTORE_FLAGS

DVIPNG_VERSION=dvipng_version
AC_SUBST([DVIPNG_VERSION])

AC_MSG_RESULT([
** Configuration summary for $PACKAGE_STRING:

   The -d (debug) switch is enabled:        $enable_debug
   Your gd is new enough (>=2.0) to enable
     the --truecolor switch, full alpha 
     transparency, proper rescaling of 
     included bitmaps, and jpeg inclusion:  $ac_cv_func_gdImageCreateTrueColor
   Your gd has jpeg inclusion enabled:      $ac_cv_func_gdImageCreateFromJpeg
   Your gd is new enough (>=2.0.12) to 
     enable transparent backgrounds for EPS
     inclusion and the -z (compression)
     switch:                                $ac_cv_func_gdImagePngEx
   Your gd is new enough (>=2.0.21) to
     allow image creation from memory       $ac_cv_func_gdImageCreateFromPngPtr
   Your gd is new enough (>=2.0.28) to
     enable gif inclusion and output 
     (dvigif):                              $ac_cv_func_gdImageGif
   FreeType font rendering available:       $ac_have_freetype2
   Support for subfonts (CJK-LaTeX):        $ac_have_freetype2
   T1lib font rendering available:          $ac_cv_lib_t1_T1_InitLib
])

AC_CONFIG_HEADER([config.h])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
