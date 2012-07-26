dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009-2012 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
dnl *********************************************************************
dnl
dnl  Adapted for TeX Live from dvipng-1.12/configure.ac
dnl  Copyright (C) 2002-2008 Jan-Åke Larsson
dnl
dnl *********************************************************************
dnl
m4_define([dvipng_version], [1.14])[]dnl using unmodified dvipng source tree
AC_INIT([dvipng], dvipng_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([dvipng-]dvipng_version[/dvipng.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

# Common code for all programs using libkpathsea.
KPSE_COMMON([dvipng])
# Configure options for dvipng also shown at the TeX Live top-level.
m4_include([ac/dvipng.ac])

AC_CANONICAL_HOST

AM_CONDITIONAL([cross], [test "x$cross_compiling" = xyes])

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

# SELFAUTO -- not used when built as part of the TeX Live tree.

# We have to check properties of libraries, either installed (system)
# libraries or unistalled (possibly libtool) ones from the TeX Live tree.
# Thus we can not use, e.g., AC_CHECK_LIB(LIB, FUNCTION)

KPSE_KPATHSEA_FLAGS
KPSE_ZLIB_FLAGS
KPSE_LIBPNG_FLAGS
KPSE_T1LIB_FLAGS
KPSE_FREETYPE2_FLAGS
KPSE_GD_FLAGS

# Assume we have t1lib and freetype2.
have_t1=yes
have_ft2=yes

if test "x$enable_build" != xno || test -f config.force; then

# Checks for more libraries.
KPSE_ADD_FLAGS([zlib])
AC_CHECK_FUNC([deflate],
              [AC_DEFINE([HAVE_LIBZ], 1,
                         [Define to 1 if you have the `z' library (-lz).])])

KPSE_ADD_FLAGS([libpng])
AC_CHECK_FUNC([png_read_image],
              [AC_DEFINE([HAVE_LIBPNG], 1,
                         [Define to 1 if you have the `png' library (-lpng).])],
              [AC_MSG_ERROR([cannot find/use libpng])])

KPSE_ADD_FLAGS([t1lib])
AC_CHECK_FUNC([T1_InitLib],
              [AC_DEFINE([HAVE_LIBT1], 1,
                         [Define to 1 if you have the `t1' library (-lt1).])],
              [have_t1=no])

KPSE_ADD_FLAGS([freetype2])
AC_CHECK_FUNC([FT_Init_FreeType],
              [AC_DEFINE([HAVE_FT2], 1,
                         [Define to 1 if you have freetype2.])],
              [have_ft2=no])

KPSE_ADD_FLAGS([gd])
AC_CHECK_FUNC([gdImageCreate],
              [AC_DEFINE([HAVE_LIBGD], 1,
                         [Define to 1 if you have the `gd' library (-lgd).])],
              [AC_MSG_ERROR([cannot find/use libgd
This drawing library can be downloaded at http://www.boutell.com/gd])])

KPSE_ADD_FLAGS([kpathsea])
AC_CHECK_FUNC([kpse_set_program_name],
              [AC_DEFINE([HAVE_LIBKPATHSEA], 1,
                         [Define to 1 if you have the `kpathsea' library (-lkpathsea).])],
              [AC_MSG_ERROR([cannot find/use libkpathsea])])

# We need enc, cmap, and sfd formats.
# Introduced together with opentype format (Dec 2003).
KPSE_CHECK_KPSE_FORMAT([opentype],
                       [AC_DEFINE([HAVE_KPSE_ENC_FORMATS], 1,
                                  [Define to 1 if your kpathsea has kpse_enc_format.])])

# Checks for more header files.
AC_CHECK_HEADERS([gd.h png.h kpathsea/kpathsea.h], ,
                 [AC_MSG_ERROR([cannot find/use $ac_header])])
AC_CHECK_HEADERS([t1lib.h])

# Checks for more library functions.
AC_CHECK_FUNCS([gdImageCreateTrueColor gdImageCreateFromJpeg gdImagePngEx gdImageCreateFromPngPtr gdImageGif FT_Library_Version])

KPSE_RESTORE_FLAGS

echo timestamp >config.force

fi

AM_CONDITIONAL([have_t1], [test "x$have_t1" = xyes])
AM_CONDITIONAL([have_ft2], [test "x$have_ft2" = xyes])
AM_CONDITIONAL([have_ft2_or_t1], [test "x$have_ft2:$have_t1" != xno:no])
AM_CONDITIONAL([have_gif], [test "x$ac_cv_func_gdImageGif" = xyes])

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

AC_CONFIG_HEADERS([config.h])

AC_SUBST([DVIPNG_TREE], [dvipng-]dvipng_version)

KPSE_CHECK_WIN32
AM_CONDITIONAL([WIN32], [test "x$kpse_cv_have_win32" != xno])

AC_CONFIG_FILES([Makefile help/Makefile doc/Makefile])

AC_OUTPUT
