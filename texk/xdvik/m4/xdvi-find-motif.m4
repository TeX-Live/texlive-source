# Autoconf macros for xdvik.
# Copyright (C) 2001 - 2009 Marcin Dalecki <xdvi-core@lists.sourceforge.net>
# Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# XDVI_FIND_MOTIF
# ---------------
# Check for Motif libraries and headers.
# Put Motif include directory in motif_include,
# put Motif library directory in motif_libdir,
# and add appropriate flags to X_CFLAGS and X_LIBS.
# If default_toolkit is not `none', use Xaw as toolkit if Motif isn't found.
AC_DEFUN([XDVI_FIND_MOTIF],
[AC_REQUIRE([AC_PATH_XTRA])
motif_include=
motif_libdir=
AC_ARG_WITH([motif-include],
            AS_HELP_STRING([--with-motif-include=DIR],
                           [Specify the location of Motif include files])
            [motif_include=$withval])
AC_ARG_WITH([motif-libdir],
            AS_HELP_STRING([--with-motif-libdir=DIR],
                           [Specify the location of Motif libraries])
            [motif_libdir=$withval])
AC_MSG_CHECKING([for Motif])
#
# Search the include files.
#
if test "$motif_include" = ""; then
  _XDVI_FIND_MOTIF_INCLUDES
fi
#
# Now for the libraries.
#
if test "$motif_libdir" = ""; then
  _XDVI_FIND_MOTIF_LIBRARIES
fi
# Add Motif definitions to X flags
#
if test "$motif_include" != "" && test "$motif_include" != "$x_includes" && test "$motif_include" != "no"
then
X_CFLAGS="-I$motif_include $X_CFLAGS"
fi
if test "$motif_libdir" != "" && test "$motif_libdir" != "$x_libraries" && test "$motif_libdir" != "no"
then
case "$X_LIBS" in
  *-R\ *) X_LIBS="-L$motif_libdir -R $motif_libdir $X_LIBS";;
  *-R*)   X_LIBS="-L$motif_libdir -R$motif_libdir $X_LIBS";;
  *)      X_LIBS="-L$motif_libdir $X_LIBS";;
esac
fi
#
#
motif_libdir_result="$motif_libdir"
motif_include_result="$motif_include"
test "$motif_libdir_result" = "" && 
  motif_libdir_result="in default path" && AC_DEFINE([MOTIF], 1, [Define to use the Motif toolkit.])
test "$motif_include_result" = "" && 
  motif_include_result="in default path" && AC_DEFINE([MOTIF], 1)
if test "$motif_libdir_result" = "no"; then
    if test "$default_toolkit" = "none"; then
        motif_libdir_result="(none)"
    else
	motif_libdir_result=""
    fi
fi
if test "$motif_include_result" = "no"; then
    if test "$default_toolkit" = "none"; then
        motif_include_result="(none)"
    else
	motif_include_result=""
    fi
fi

if test "$motif_include_result" != "" && test "$motif_libdir_result" != ""; then
  AC_MSG_RESULT([libraries $motif_libdir_result, headers $motif_include_result])
  prog_extension="motif"
  AC_DEFINE([MOTIF], 1)
  x_tool_libs="-lXm"
  # now warn if we're using LessTif (see LESSTIF-BUGS for why ...)
  _XDVI_CHECK_LESSTIF
  # Check whether to compile for Motif with Xaw Panner.
  _XDVI_CHECK_PANNER
else
    AC_MSG_RESULT([not found, using Xaw])
    with_xdvi_x_toolkit="xaw"
fi

]) # XDVI_FIND_MOTIF

# _XDVI_FIND_MOTIF_INCLUDES
# -------------------------
# Search the Xm include files.
m4_define([_XDVI_FIND_MOTIF_INCLUDES],
[AC_CACHE_VAL([xdvi_cv_motif_include],
[
xdvi_motif_save_LIBS="$LIBS"
xdvi_motif_save_CFLAGS="$CFLAGS"
xdvi_motif_save_CPPFLAGS="$CPPFLAGS"
xdvi_motif_save_LDFLAGS="$LDFLAGS"
#
LIBS="$X_PRE_LIBS -lXm -lXt -lX11 $X_EXTRA_LIBS $LIBS"
CFLAGS="$X_CFLAGS $CFLAGS"
CPPFLAGS="$X_CFLAGS $CPPFLAGS"
LDFLAGS="$X_LIBS $LDFLAGS"
#
AC_TRY_COMPILE([#include <Xm/Xm.h>],[int a;],
[
# Xm/Xm.h is in the standard search path.
xdvi_cv_motif_include=
],
[
# Xm/Xm.h is not in the standard search path.
# Locate it and put its directory in `motif_include'
#
# /usr/include/Motif* are used on HP-UX (Motif).
# /usr/include/X11* are used on HP-UX (X and Athena).
# /usr/dt is used on Solaris (Motif).
# /usr/openwin is used on Solaris (X and Athena).
# Other directories are just guesses.
for dir in "$x_includes" "${prefix}/include" /usr/include /usr/local/include \
           /usr/include/Motif2.0 /usr/include/Motif1.2 /usr/include/Motif1.1 \
           /usr/include/X11R6 /usr/include/X11R5 /usr/include/X11R4 \
           /usr/dt/include /usr/openwin/include \
           /usr/dt/*/include /opt/*/include /usr/include/Motif* \
	   "${prefix}"/*/include /usr/*/include /usr/local/*/include \
	   "${prefix}"/include/* /usr/include/* /usr/local/include/*; do
    if test -f "$dir/Xm/Xm.h"; then
        xdvi_cv_motif_include="$dir"
        break
    fi
done
if test "$xdvi_cv_motif_include" = ""; then
xdvi_cv_motif_include=no
fi
])
#
LIBS="$xdvi_motif_save_LIBS"
CFLAGS="$xdvi_motif_save_CFLAGS"
CPPFLAGS="$xdvi_motif_save_CPPFLAGS"
LDFLAGS="$xdvi_motif_save_LDFLAGS"
])
motif_include="$xdvi_cv_motif_include"
]) # _XDVI_FIND_MOTIF_INCLUDES

# _XDVI_FIND_MOTIF_LIBRARIES
# --------------------------
# Search the Xm library.
m4_define([_XDVI_FIND_MOTIF_LIBRARIES],
[AC_CACHE_VAL([xdvi_cv_motif_libdir],
[
xdvi_motif_save_LIBS="$LIBS"
xdvi_motif_save_CFLAGS="$CFLAGS"
xdvi_motif_save_CPPFLAGS="$CPPFLAGS"
xdvi_motif_save_LDFLAGS="$LDFLAGS"
#
LIBS="$X_PRE_LIBS -lXm -lXt -lX11 $X_EXTRA_LIBS $LIBS"
CFLAGS="$X_CFLAGS $CFLAGS"
CPPFLAGS="$X_CFLAGS $CPPFLAGS"
LDFLAGS="$X_LIBS $LDFLAGS"
#
# We use XtToolkitInitialize() here since it takes no arguments
# and thus also works with a C++ compiler.
AC_TRY_LINK(
[#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
],[XtToolkitInitialize();],
[
# libXm.a is in the standard search path.
xdvi_cv_motif_libdir=
],
[
# libXm.a is not in the standard search path.
# Locate it and put its directory in `motif_libdir'
#
# /usr/lib/Motif* are used on HP-UX (Motif).
# /usr/lib/X11* are used on HP-UX (X and Athena).
# /usr/dt is used on Solaris (Motif).
# /usr/lesstif is used on Linux (Lesstif).
# /usr/openwin is used on Solaris (X and Athena).
# Other directories are just guesses.
for dir in "$x_libraries" "${prefix}/lib" /usr/lib /usr/local/lib \
	   /usr/lib/Motif2.0 /usr/lib/Motif1.2 /usr/lib/Motif1.1 \
	   /usr/lib/X11R6 /usr/lib/X11R5 /usr/lib/X11R4 /usr/lib/X11 \
           /usr/dt/lib /usr/openwin/lib \
	   /usr/dt/*/lib /opt/*/lib /usr/lib/Motif* \
           /usr/lesstif*/lib /usr/lib/Lesstif* \
	   "${prefix}"/*/lib /usr/*/lib /usr/local/*/lib \
	   "${prefix}"/lib/* /usr/lib/* /usr/local/lib/*; do
    if test -d "$dir" && test "`ls $dir/libXm.* 2> /dev/null`" != ""; then
        xdvi_cv_motif_libdir="$dir"
        break
    fi
done
if test "$xdvi_cv_motif_libdir" = ""; then
xdvi_cv_motif_libdir=no
fi
])
#
LIBS="$xdvi_motif_save_LIBS"
CFLAGS="$xdvi_motif_save_CFLAGS"
CPPFLAGS="$xdvi_motif_save_CPPFLAGS"
LDFLAGS="$xdvi_motif_save_LDFLAGS"
])
#
motif_libdir="$xdvi_cv_motif_libdir"
]) # _XDVI_FIND_MOTIF_LIBRARIES

# _XDVI_CHECK_LESSTIF
# -------------------
# Check if using LessTif and warn if so.
m4_define([_XDVI_CHECK_LESSTIF],
[AC_MSG_CHECKING([for LessTif])
save_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $X_CFLAGS"
AC_TRY_COMPILE([
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>],[const char *p = LesstifVERSION_STRING;
],[
# yes, we're running LessTif
AC_MSG_RESULT([yes])
AC_MSG_WARN([LessTif header detected.
  *****************************************************************
  * Warning: You are using LessTif instead of OpenMotif.          *
  * Some GUI elements might be broken; please see the file        *
  *                                                               *
  * texk/xdvik/LESSTIF-BUGS                                       *
  *                                                               *
  * for more information.                                         *
  *****************************************************************])
],[
# no, not running LessTif
AC_MSG_RESULT([no])
])
CPPFLAGS=$save_CPPFLAGS
]) # _XDVI_CHECK_LESSTIF

# _XDVI_CHECK_PANNER
# ------------------
# Check whether to compile for Motif with Xaw Panner.
m4_define([_XDVI_CHECK_PANNER],
[AC_MSG_CHECKING([whether to compile in panner (requires Xaw)])
save_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$CPPFLAGS $X_CFLAGS"
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xfuncs.h>
#include <X11/Intrinsic.h>
#include <X11/Xaw/Reports.h>]])],
                 [xdvi_use_xaw_panner=yes],
                 [xdvi_use_xaw_panner=no])
CPPFLAGS=$save_CPPFLAGS
AC_MSG_RESULT([$xdvi_use_xaw_panner])
if test "x$xdvi_use_xaw_panner" = xyes; then
  AC_DEFINE([USE_XAW_PANNER], 1, [Define to use Xaw panner.])
fi
]) # _XDVI_CHECK_PANNER
