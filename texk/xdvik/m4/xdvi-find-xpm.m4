# Autoconf macros for xdvik.
# Copyright (C) 2001 - 2009 Marcin Dalecki <xdvi-core@lists.sourceforge.net>
# Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# XDVI_FIND_XPM
# -------------
# Put Xpm include directory in xpm_includes,
# put Xpm library directory in xpm_libraries,
# define x_xpm_libs,
# and add appropriate flags to X_CFLAGS and X_LIBS.
AC_DEFUN([XDVI_FIND_XPM],
[AC_REQUIRE([AC_PATH_XTRA])
AC_ARG_WITH([xpm],
            AS_HELP_STRING([--without-xpm],
                           [Do not use the Xpm library (will disable the toolbar)]))[]dnl
AC_ARG_WITH([xpm-includes],
            AS_HELP_STRING([--with-xpm-include=DIR],
                           [Specify the location of Xpm include files]),
            [xpm_includes=$withval], [xpm_includes=])[]dnl
AC_ARG_WITH([xpm-libraries],
            AS_HELP_STRING([--with-xpm-libdir=DIR],
                           [Specify the location of Xpm libraries]),
            [xpm_libraries=$withval], [xpm_libraries=])[]dnl
dnl Treat --without-xpm like
dnl --without-xpm-includes --without-xpm-libraries.
if test "x$with_xpm" = xno; then
  xpm_includes=no
  xpm_libraries=no
fi
AC_MSG_CHECKING([for Xpm])
#
# Check the headers.
#
if test "x$xpm_includes" = x; then
  _XDVI_FIND_XPM_INCLUDES
fi
if test "x$xpm_includes" = xdefault; then
  AC_DEFINE([HAVE_X11_XPM_H], 1, [Define if you have the <X11/xpm.h> header file.])
elif test -f "$xpm_includes/X11/xpm.h"; then
  AC_DEFINE([HAVE_X11_XPM_H], 1)
elif test -f "$xpm_includes/xpm.h"; then
  AC_DEFINE([HAVE_XPM_H], 1,
            [Define if you have the <xpm.h> header file (not in X11, e.g. Solaris 5.8).])
fi
#
# Add Xpm definition to X_CFLAGS (and remember previous value)
#
xdvi_xpm_save_X_CFLAGS=$X_CFLAGS
if test "x$xpm_includes" != xdefault \
    && test "x$xpm_includes" != "x$x_includes" && test "x$xpm_includes" != xno
then
  X_CFLAGS="-I$xpm_includes $X_CFLAGS"
fi
#
# Check the libraries.
#
if test "x$xpm_libraries" = x; then
  _XDVI_FIND_XPM_LIBRARIES
fi
#
# Report the results of headers and libraries.
#
xdvi_use_xpm=yes
#
#
xpm_libraries_result=$xpm_libraries
if test "x$xpm_libraries_result" = xdefault; then
  xpm_libraries_result="in default path"
elif test "x$xpm_libraries_result" = xno; then
  xpm_libraries_result="(none)"
  xdvi_use_xpm=no
fi
#
xpm_includes_result=$xpm_includes
if test "x$xpm_includes_result" = xdefault; then
  xpm_includes_result="in default path"
elif test "x$xpm_includes_result" = xno; then
  xpm_includes_result="(none)"
  xdvi_use_xpm=no
fi
#
AC_MSG_RESULT([libraries $xpm_libraries_result, headers $xpm_includes_result])
#
if test "x$xdvi_use_xpm" = xyes; then
  #
  # Add Xpm definition to X_LIBS
  #
  if test "x$xpm_libraries" != xdefault \
      && test "x$xpm_libraries" != "x$x_libraries" && test "x$xpm_libraries" != xno
  then
    case "$X_LIBS" in
      *-R\ *) X_LIBS="-L$xpm_libraries -R $xpm_libraries $X_LIBS";;
      *-R*)   X_LIBS="-L$xpm_libraries -R$xpm_libraries $X_LIBS";;
      *)      X_LIBS="-L$xpm_libraries $X_LIBS";;
    esac
  fi
  #
  AC_DEFINE([USE_XPM], 1, [Define if you want to use the Xpm library])
  x_xpm_libs="-lXpm"
else
  # Restore previous X_CFLAGS
  X_CFLAGS=$xdvi_xpm_save_X_CFLAGS
  AC_DEFINE([USE_XPM], 0)
  x_xpm_libs=
fi
AC_SUBST([x_xpm_libs])
]) # XDVI_FIND_XPM

# _XDVI_FIND_XPM_INCLUDES
# -----------------------
# Search the Xpm include files.
# They can either be in <X11/xpm.h> (as in X11R6), which is dealt with
# by AC_CHECK_HEADERS([X11/xpm.h]),
# or in <xpm.h> if installed locally; this is what this test is for.
m4_define([_XDVI_FIND_XPM_INCLUDES],
[AC_CACHE_VAL([xdvi_cv_xpm_includes],
[xdvi_xpm_save_CFLAGS=$CFLAGS
xdvi_xpm_save_CPPFLAGS=$CPPFLAGS
#
CFLAGS="$X_CFLAGS $CFLAGS"
CPPFLAGS="$X_CFLAGS $CPPFLAGS"
#
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <X11/xpm.h>]],
                                   [[int a;]])],
                  [# X11/xpm.h is in the standard search path.
                   xdvi_cv_xpm_includes=default],
                  [# X11/xpm.h is not in the standard search path.
                   xdvi_cv_xpm_includes=no
# Locate it and put its directory in `xpm_includes'
#
# /usr/include/Motif* are used on HP-UX (Motif).
# /usr/include/X11* are used on HP-UX (X and Xaw).
# /usr/dt is used on Solaris (Motif).
# /usr/openwin is used on Solaris (X and Xaw).
# Other directories are just guesses.
for dir in "$x_includes" "${prefix}/include" /usr/include /usr/local/include \
	   /usr/X11/include /usr/X11R5/include /usr/X11R6/include \
           /usr/include/Motif2.0 /usr/include/Motif1.2 /usr/include/Motif1.1 \
           /usr/include/X11R6 /usr/include/X11R5 /usr/include/X11R4 \
           /usr/dt/include /usr/openwin/include \
           /usr/dt/*/include /opt/*/include /usr/include/Motif* \
	   "${prefix}"/*/include /usr/*/include /usr/local/*/include \
	   "${prefix}"/include/* /usr/include/* /usr/local/include/*
do
    if test -f "$dir/X11/xpm.h"; then
    	xdvi_cv_xpm_includes=$dir
    	break
    elif test -f "$dir/xpm.h"; then
    	xdvi_cv_xpm_includes=$dir
    	break
    fi
done])
#
CFLAGS=$xdvi_xpm_save_CFLAGS
CPPFLAGS=$xdvi_xpm_save_CPPFLAGS])
#
xpm_includes=$xdvi_cv_xpm_includes
]) # _XDVI_FIND_XPM_INCLUDES

# _XDVI_FIND_XPM_LIBRARIES
# ------------------------
# Search the Xpm library.
m4_define([_XDVI_FIND_XPM_LIBRARIES],
[AC_CACHE_VAL([xdvi_cv_xpm_libraries],
[xdvi_xpm_save_LIBS=$LIBS
xdvi_xpm_save_CFLAGS=$CFLAGS
xdvi_xpm_save_CPPFLAGS=$CPPFLAGS
xdvi_xpm_save_LDFLAGS=$LDFLAGS
#
LIBS="$X_PRE_LIBS -lXpm -lXt -lX11 $X_EXTRA_LIBS $LIBS"
CFLAGS="$X_CFLAGS $CFLAGS"
CPPFLAGS="$X_CFLAGS $CPPFLAGS"
LDFLAGS="$X_LIBS $LDFLAGS"
#
# We use XtToolkitInitialize() here since it takes no arguments
# and thus also works with a C++ compiler.
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <X11/Intrinsic.h>
#include <X11/xpm.h>]],
                                [[XtToolkitInitialize();]])],
               [# libxpm.a is in the standard search path.
                xdvi_cv_xpm_libraries=default],
               [# libXpm.a is not in the standard search path.
                xdvi_cv_xpm_libraries=no
# Locate it and put its directory in `xpm_libraries'
#
# /usr/lib/Motif* are used on HP-UX (Motif).
# /usr/lib/X11* are used on HP-UX (X and Xpm).
# /usr/dt is used on Solaris (Motif).
# /usr/openwin is used on Solaris (X and Xpm).
# Other directories are just guesses.
for dir in "$x_libraries" "${prefix}/lib" /usr/lib /usr/local/lib \
	   /usr/lib/Motif2.0 /usr/lib/Motif1.2 /usr/lib/Motif1.1 \
	   /usr/lib/X11R6 /usr/lib/X11R5 /usr/lib/X11R4 /usr/lib/X11 \
           /usr/dt/lib /usr/openwin/lib \
	   /usr/dt/*/lib /opt/*/lib /usr/lib/Motif* \
	   "${prefix}"/*/lib /usr/*/lib /usr/local/*/lib \
	   "${prefix}"/lib/* /usr/lib/* /usr/local/lib/*
do
    if test -d "$dir" && test "`ls $dir/libXpm.* 2> /dev/null`" != ""; then
        xdvi_cv_xpm_libraries=$dir
        break
    fi
done])
#
LIBS=$xdvi_xpm_save_LIBS
CFLAGS=$xdvi_xpm_save_CFLAGS
CPPFLAGS=$xdvi_xpm_save_CPPFLAGS
LDFLAGS=$xdvi_xpm_save_LDFLAGS])
#
xpm_libraries="$xdvi_cv_xpm_libraries"]) # _XDVI_FIND_XPM_LIBRARIES
