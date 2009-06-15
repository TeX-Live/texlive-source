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

# Common code for all programs using libkpathsea.
KPSE_COMMON([dvipng])
# Configure options for dvipng also shown at the TeX Live top-level.
m4_include([ac/dvipng.ac])

if test "x$enable_debug" != xno; then
  enable_debug=yes
  AC_DEFINE([DEBUG], 1, [Define as 1 to get the debug (-d) option.])
fi

if test "x$enable_timing" = xyes; then
  AC_DEFINE([TIMING], 1, [Define as 1 to get execution time output.])
fi

# Checks for programs.
# For a native TeX Live build '--with-gs' is ignored.
AS_IF([test "x$enable_native_texlive_build" = xyes],
      [with_gs=])
AS_CASE([$with_gs],
        ["" | yes | no], [AC_CHECK_PROG([GS], [gs], [gs])],
        [AC_PATH_PROG([GS], ["$with_gs"])])
AS_IF([test -n "$GS"],
      [GS_CHECK_DEVICES],
      [AC_MSG_WARN([Cannot find GhostScript in your PATH])
       GS=gs])
AC_DEFINE_UNQUOTED([GS_PATH], ["$GS"], [Define as the path to GhostScript.])

# Checks for libraries.
AC_SEARCH_LIBS([pow], [m])
AC_SEARCH_LIBS([basename], [gen])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h sys/time.h])
AC_HEADER_SYS_WAIT
AC_HEADER_TIME
AC_HEADER_STDBOOL

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_INT64_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_ALLOCA
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_STRTOD
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([dup2 memset munmap pow putenv strchr strrchr strtol strstr])

if test "x$enable_timing" = xyes; then
  AC_CHECK_FUNCS([ftime gettimeofday])
fi

# Documentation-related checks.
AC_PATH_PROG([MAKEINFO], [makeinfo], [:])
MAKEINFO_CHECK_MACROS([acronym env option])
AC_PATH_PROG([INSTALL_INFO], [install-info], [:], [$PATH /usr/sbin /sbin])

# SELFAUTO
# When dvipng is built as part of the TeX Live tree, the option
# '--enable-texlive-build' is automatically passed from the top-level.
# In that case we silently assume '--disable-selfauto-set'.
test "x$enable_texlive_build" = xyes && enable_selfauto_set=no
AC_ARG_ENABLE([selfauto-set], 
  AC_HELP_STRING([--enable-selfauto-set],
  [This option will make the final binary explicitly set the
   $SELFAUTO... variables to make it look as dvipng is installed in the
   main texmf tree, even if it isn't. This is necessary when texmf.cnf
   only uses $SELFAUTO... variables and dvipng is not installed in the
   texmf tree. Otherwise, dvipng may not be able to find virtual
   fonts, or psfonts.map. To find out, first build the binary and do
   'make test'. If the test fails, you need this switch.]),
  [ if test "$enableval" = yes ; then
      AC_MSG_CHECKING([for \$SELFAUTOLOC])
      SELFAUTOLOC=`kpsewhich -expand-var=\\\$SELFAUTOLOC`
      AC_DEFINE_UNQUOTED([ENV_SELFAUTOLOC], ["SELFAUTOLOC=$SELFAUTOLOC"],
	[The environment setting for $SELFAUTOLOC])
      AC_MSG_RESULT([$SELFAUTOLOC])
      AC_MSG_CHECKING([for \$SELFAUTODIR])
      SELFAUTODIR=`kpsewhich -expand-var=\\\$SELFAUTODIR`
      AC_DEFINE_UNQUOTED([ENV_SELFAUTODIR], ["SELFAUTODIR=$SELFAUTODIR"],
	[The environment setting for $SELFAUTODIR])
      AC_MSG_RESULT([$SELFAUTODIR])
      AC_MSG_CHECKING([for \$SELFAUTOPARENT])
      SELFAUTOPARENT=`kpsewhich -expand-var=\\\$SELFAUTOPARENT`
      AC_DEFINE_UNQUOTED([ENV_SELFAUTOPARENT], ["SELFAUTOPARENT=$SELFAUTOPARENT"],
	[The environment setting for $SELFAUTOPARENT])
      AC_MSG_RESULT([$SELFAUTOPARENT])
    fi ],
  [AC_MSG_CHECKING([for texmf.cnf])
  TEXMF_CNF=`kpsewhich texmf.cnf`
  AC_MSG_RESULT([$TEXMF_CNF])
  AC_PATH_PROG([KPSEWHICH], [kpsewhich])
  AC_MSG_CHECKING([for psfonts.map])
  cp $KPSEWHICH .
  PSFONTS_MAP=`./kpsewhich psfonts.map`
  rm -f ./kpsewhich
  if test -n "$PSFONTS_MAP"; then
    AC_MSG_RESULT([$PSFONTS_MAP])
  else
    AC_MSG_RESULT([not found from outside the texmf tree])
    AC_MSG_CHECKING([for \$SELFAUTO in texmf.cnf])
    if grep SELFAUTO "$TEXMF_CNF" > /dev/null 2> /dev/null; then
      AC_MSG_RESULT([yes
***************************************************************
texmf.cnf is using \$SELFAUTO... variables. If you are going to
install dvipng outside the texmf tree, you may need to use
--enable-selfauto-set. To find out, do 'make ; make test'. If the test
is unsuccessful, add the mentioned switch and rebuild.
***************************************************************])
    else
      AC_MSG_RESULT([no])
    fi
  fi])

# We have to check properties of libraries, either installed (system)
# libraries or unistalled (possibly libtool) ones from the TeX Live tree.
# Thus we can not use, e.g., AC_CHECK_LIB(LIB, FUNCTION)

KPSE_KPATHSEA_FLAGS
KPSE_ZLIB_FLAGS
KPSE_LIBPNG_FLAGS
KPSE_T1LIB_FLAGS
KPSE_FREETYPE2_FLAGS
KPSE_GD_FLAGS

# Checks for more libraries.
KPSE_ADD_FLAGS([zlib])
AC_CHECK_FUNC([deflate],
              [AC_DEFINE([HAVE_LIBZ], 1,
                         [Define to 1 if you have the `z' library (-lz).])])

KPSE_ADD_FLAGS([libpng])
AC_CHECK_FUNC([png_read_image],
              [AC_DEFINE([HAVE_LIBPNG], 1,
                         [Define to 1 if you have the `png' library (-lpng).])],
              [KPSE_MSG_ERROR([dvipng], [cannot find/use libpng])])

KPSE_ADD_FLAGS([freetype2])
KPSE_ADD_FLAGS([gd])
AC_CHECK_FUNC([gdImageCreate],
              [AC_DEFINE([HAVE_LIBGD], 1,
                         [Define to 1 if you have the `gd' library (-lgd).])],
              [KPSE_MSG_ERROR([dvipng], [cannot find/use libgd
This drawing library can be downloaded at http://www.boutell.com/gd])])

KPSE_ADD_FLAGS([kpathsea])
AC_CHECK_FUNC([kpse_set_progname],
              [AC_DEFINE([HAVE_LIBKPATHSEA], 1,
                         [Define to 1 if you have the `kpathsea' library (-lkpathsea).])],
              [KPSE_MSG_ERROR([dvipng], [cannot find/use libkpathsea])])

# We need enc, cmap, and sfd formats.
# Introduced together with opentype format (Dec 2003).
KPSE_CHECK_KPSE_FORMAT([opentype],
                       [AC_DEFINE([HAVE_KPSE_ENC_FORMATS], 1,
                                  [Define to 1 if your kpathsea has kpse_enc_format.])])

KPSE_ADD_FLAGS([t1lib])
AC_CHECK_FUNC([T1_InitLib],
              [have_ft2_or_t1=yes
               AC_DEFINE([HAVE_LIBT1], 1,
                         [Define to 1 if you have the `t1' library (-lt1).])],
              [have_ft2_or_t1=no])
AM_CONDITIONAL([have_t1], [test "x$ac_cv_func_T1_InitLib" = xyes])

KPSE_ADD_FLAGS([freetype2])
AC_CHECK_FUNC([FT_Init_FreeType],
              [have_ft2_or_t1=yes
               AC_DEFINE([HAVE_FT2], 1,
                         [Define to 1 if you have freetype2.])])
AM_CONDITIONAL([have_ft2], [test "x$ac_cv_func_FT_Init_FreeType" = xyes])
AM_CONDITIONAL([have_ft2_or_t1], [test "x$have_ft2_or_t1" = xyes])

# Checks for more header files.
AC_CHECK_HEADERS([gd.h png.h kpathsea/kpathsea.h], ,
                 [KPSE_MSG_ERROR([dvipng], [cannot find/use $ac_header])])
AC_CHECK_HEADERS([t1lib.h])

# Checks for more library functions.
AC_CHECK_FUNCS([gdImageCreateTrueColor gdImageCreateFromJpeg gdImagePngEx gdImageCreateFromPngPtr gdImageGif FT_Library_Version])

AM_CONDITIONAL([have_gif], [test "x$ac_cv_func_gdImageGif" = xyes])

KPSE_RESTORE_FLAGS

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
   FreeType font rendering available:       $ac_cv_func_FT_Init_FreeType
   Support for subfonts (CJK-LaTeX):        $ac_cv_func_FT_Init_FreeType
   T1lib font rendering available:          $ac_cv_func_T1_InitLib
])

AC_CONFIG_HEADER([config.h])

AC_CONFIG_FILES([Makefile help/Makefile doc/Makefile])

AC_OUTPUT
