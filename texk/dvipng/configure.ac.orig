# configure.ac

# Adapted for dvipng-1.9 / teTeX by Thomas Esser.
# Adapted for dvipng-1.10 / TeX live by Peter Breitenlohner.
# All settings here are our fault, don't blame the dvipng maintainers for it.

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
#  Copyright (C) 2002-2008 Jan-Åke Larsson
#
#************************************************************************

# Process this file with autoconf to produce a configure script.
AC_INIT([dvipng], [1.11], [dvipng@nongnu.org])
AC_CONFIG_SRCDIR([dvipng.c])

AC_ARG_ENABLE(debug, 
  AC_HELP_STRING([--disable-debug],[Compile without debug (-d) option]), 
    [ if test "$enableval" = yes ; then
      AC_DEFINE(DEBUG, 1, [Define as 1 to get the debug (-d) option.])
      fi ],	
    [ enable_debug="yes";
      AC_DEFINE(DEBUG, 1, [Define as 1 to get the debug (-d) option.])])
AC_ARG_ENABLE(timing, 
  AC_HELP_STRING([--enable-timing],[Output execution time of dvipng]), 
   [ if test "$enableval" = yes ; then
     AC_DEFINE(TIMING, 1, [Define as 1 to get execution time output.])
     fi ])

# Checks for programs.
AC_PROG_MAKE_SET
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_ARG_WITH(gs,
  AC_HELP_STRING([--with-gs=/PATH/TO/gs],[Hard-wire the location of GhostScript]), 
  [if test "x$withval" = xno ; then
    GS=no
  else
    AC_PATH_PROG(GS,["$withval"])
    GS_CHECK_DEVICES
  fi],
  [AC_CHECK_PROG(GS,gs,gs)
  if test -n "$GS"; then
    GS_CHECK_DEVICES
  else
    GS="gs"
    AC_MSG_WARN([Cannot find GhostScript in your PATH])
  fi
])
AC_DEFINE_UNQUOTED(GS_PATH, "$GS", [Define as the path to GhostScript.])

ac_cv_lib_t1_T1_InitLib=yes
PSFONTS_O=""
if test "$ac_cv_lib_t1_T1_InitLib" = yes; then
  PSFONTS_O="t1.o"
fi
AC_SUBST(PSFONTS_O)

ac_have_freetype2="yes"    # we have ft2
if test "$ac_have_freetype2" = yes; then
  # Have to set compiler and linker flags in Makefile.in, it seems.
	PSFONTS_O="$PSFONTS_O sfd.o ft.o"	
	AC_DEFINE(HAVE_FT2, 1, [Define to 1 if you have freetype2])
	AC_DEFINE(HAVE_FT_LIBRARY_VERSION)
fi

if test -n "$PSFONTS_O"; then
  PSFONTS_O="$PSFONTS_O enc.o fontmap.o tfm.o"
fi
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_HEADER_TIME
AC_HEADER_STDBOOL
AC_CHECK_HEADERS([inttypes.h fcntl.h sys/time.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_PID_T
AC_TYPE_SIZE_T

#if test "$ac_cv_header_inttypes_h" = yes; then
  # Sometimes we want to use gcc -ansi -pedantic as a portability test
  # The typedef of int64_t is not in the system header file in that
  # case. Then, #define int64_t as "long long", which is non-ansi, but
  # is present in most modern compilers. Using a #define rather than a
  # typedef can be a problem, but in dvipng int64_t only is used as
  # typecast, and there are no problems.

#  MY_CHECK_TYPE(int64_t, long long)
#  MY_CHECK_TYPE(uint64_t, unsigned long long)
#fi

# Checks for library functions.
AC_FUNC_ALLOCA
AC_FUNC_MMAP
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([dup2 memset munmap pow putenv strchr strrchr strtol strstr])
if test "$enable_timing" = "yes"; then
  AC_CHECK_FUNCS([ftime gettimeofday])
fi

AC_SEARCH_LIBS([pow], [m])
AC_SEARCH_LIBS([basename], [gen])

# Documentation-related checks
AC_PATH_PROG(MAKEINFO, makeinfo, :)
MAKEINFO_CHECK_MACROS(acronym env option)
AC_PATH_PROG(INSTALL_INFO, install-info, :, $PATH /usr/sbin /sbin)

sinclude(../../libs/zlib/withenable.ac)
sinclude(../../libs/zlib/zlib.ac)
sinclude(../../libs/libpng/withenable.ac)
sinclude(../../libs/libpng/libpng.ac)
sinclude(../../libs/t1lib/withenable.ac)
sinclude(../../libs/t1lib/t1lib.ac)
sinclude(../../libs/freetype2/withenable.ac)
sinclude(../../libs/freetype2/freetype2.ac)
sinclude(../../libs/gd/withenable.ac)
sinclude(../../libs/gd/gd.ac)
sinclude(../kpathsea/withenable.ac)

if test "$with_system_freetype2" = yes; then
  OLD_LIBS=$LIBS
  LIBS="$LDFREETYPE2 $LIBS"
  AC_CHECK_FUNCS([FT_Library_Version])
  LIBS=$OLD_LIBS
else
  AC_DEFINE([HAVE_FT_LIBRARY_VERSION])
fi

if test "$with_system_gd" = yes; then
  OLD_LIBS=$LIBS
  LIBS="$LDGD $LIBS"
  AC_CHECK_FUNCS([gdImageCreateTrueColor gdImageCreateFromJpeg gdImagePngEx gdImageCreateFromPngPtr gdImageGif])
  LIBS=$OLD_LIBS
else
  AC_DEFINE([HAVE_GDIMAGECREATETRUECOLOR])
  AC_DEFINE([HAVE_GDIMAGEPNGEX])
  AC_DEFINE([HAVE_GDIMAGECREATEFROMPNGPTR])
  ac_cv_func_gdImageGif=yes
  AC_DEFINE([HAVE_GDIMAGEGIF])
fi

AC_DEFINE([HAVE_GD_H])
AC_DEFINE([HAVE_LIBGD])

if test "$ac_cv_func_gdImageGif" = "yes"; then
   INSTALL_BIN_TARGET="install-dvigif"
else
   INSTALL_BIN_TARGET="install-dvipng"
fi
AC_SUBST(INSTALL_BIN_TARGET)

AC_DEFINE([HAVE_T1LIB_H], 1,
          [Define to 1 if you have the <t1lib.h> header file.])
AC_DEFINE([HAVE_LIBT1], 1,
          [Define to 1 if you have the `t1' library (-lt1).])

AC_DEFINE([HAVE_PNG_H], 1,
          [Define to 1 if you have the <png.h> header file.])
AC_DEFINE([HAVE_LIBPNG], 1,
          [Define to 1 if you have the `png' library (-lpng).])

AC_DEFINE([HAVE_LIBZ])

# Allow shared or static libkpathsea
LIBS="-L../kpathsea/.libs -lkpathsea $LIBS"
AC_DEFINE([HAVE_KPATHSEA_KPATHSEA_H], 1,
          [Define to 1 if you have the <kpathsea/kpathsea.h> header file.])
AC_DEFINE([HAVE_KPSE_ENC_FORMATS], 1,
          [Define to 1 if your kpathsea has kpse_enc_format.])
AC_DEFINE([HAVE_LIBKPATHSEA], 1,
          [Define to 1 if you have the `kpathsea' library (-lkpathsea).])

#
# Check for ApplicationServices framework (Mac OS X)
#
AC_DEFUN([AC_HAS_APP_SERVICES], [
 _cppflags=$CPPFLAGS _ldflags=$LDFLAGS
 AC_MSG_CHECKING([for Mac OS X ApplicationServices framework])
 AC_TRY_COMPILE(
 [
#include <ApplicationServices/ApplicationServices.h>
 ], [
ATSFontRef fontRef;
 ],
 [ AC_MSG_RESULT(yes)
   AC_DEFINE(HAVE_APP_SERVICES, 1, [Define if you have ApplicationServices (Mac OS X)])
   LIBS="$LIBS -framework ApplicationServices"
 ],
 [ AC_MSG_RESULT(no) ]
 )
])

AC_HAS_APP_SERVICES

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
