dnl ### for Xpm libraries and headers.
dnl Put Xpm include directory in xpm_includes,
dnl put Xpm library directory in xpm_libraries,
dnl and add appropriate flags to X_CFLAGS and X_LIBS.
dnl
AC_DEFUN([XDVI_FIND_XPM],
[
AC_REQUIRE([AC_PATH_XTRA])
xpm_includes=
xpm_libraries=
AC_ARG_WITH([xpm],
[  --without-xpm           Do not use the Xpm library (will disable the toolbar)])
dnl Treat --without-xpm like
dnl --without-xpm-includes --without-xpm-libraries.
if test "$with_xpm" = "no"
then
xpm_includes=no
xpm_libraries=no
fi
AC_ARG_WITH([xpm-includes],
[  --with-xpm-include=DIR
                          Specify the location of Xpm include files],
[xpm_includes="$withval"])
AC_ARG_WITH([xpm-libraries],
[  --with-xpm-libdir=DIR
                          Specify the location of Xpm libraries],
[xpm_libraries="$withval"])
AC_MSG_CHECKING(for Xpm)
#
#
# Search the include files.  XPM can either be in <X11/xpm.h> (as in X11R6),
# which is dealt with by AC_CHECK_HEADERS(X11/xpm.h), or in <xpm.h> if installed locally;
# this is what this test is for.
#
if test "$xpm_includes" = ""; then
AC_CACHE_VAL(xdvi_cv_xpm_includes,
[
xdvi_xpm_save_LIBS="$LIBS"
xdvi_xpm_save_CFLAGS="$CFLAGS"
xdvi_xpm_save_CPPFLAGS="$CPPFLAGS"
xdvi_xpm_save_LDFLAGS="$LDFLAGS"
#
LIBS="$X_PRE_LIBS -lXpm -lXt -lX11 $X_EXTRA_LIBS $LIBS"
CFLAGS="$X_CFLAGS $CFLAGS"
CPPFLAGS="$X_CFLAGS $CPPFLAGS"
LDFLAGS="$X_LIBS $LDFLAGS"
#
AC_TRY_COMPILE([
#include <X11/xpm.h>
],[int a;],
[
# X11/xpm.h is in the standard search path.
xdvi_cv_xpm_includes="default"
],
[
# X11/xpm.h is not in the standard search path.
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
    	xdvi_cv_xpm_includes="$dir"
    	break
    elif test -f "$dir/xpm.h"; then
    	xdvi_cv_xpm_includes="$dir"
    	break
    fi
done
])
#
LIBS="$xdvi_xpm_save_LIBS"
CFLAGS="$xdvi_xpm_save_CFLAGS"
CPPFLAGS="$xdvi_xpm_save_CPPFLAGS"
LDFLAGS="$xdvi_xpm_save_LDFLAGS"
])
xpm_includes="$xdvi_cv_xpm_includes"
fi

dnl Second arg of AC_CACHE_VAL shouldn't contain any AC_DEFINE's,
dnl moved them out.	Peter Breitenlohner <peb@mppmu.mpg.de>
dnl
if test "x$xpm_includes" != x; then
    if test "x$xpm_includes" = xdefault; then
	AC_DEFINE([HAVE_X11_XPM_H], 1, [Define if you have the <X11/xpm.h> header file.])
    elif test -f "$xpm_includes/X11/xpm.h"; then
	AC_DEFINE([HAVE_X11_XPM_H], 1)
    elif test -f "$xpm_includes/xpm.h"; then
	AC_DEFINE([HAVE_XPM_H], 1,
	          [Define if you have the <xpm.h> header file (not in X11, e.g. Solaris 5.8).])
    fi
fi

#
#
# Now for the libraries.
#
if test "$xpm_libraries" = ""; then
AC_CACHE_VAL(xdvi_cv_xpm_libraries,
[
xdvi_xpm_save_LIBS="$LIBS"
xdvi_xpm_save_CFLAGS="$CFLAGS"
xdvi_xpm_save_CPPFLAGS="$CPPFLAGS"
xdvi_xpm_save_LDFLAGS="$LDFLAGS"
#
LIBS="$X_PRE_LIBS -lXpm -lXt -lX11 $X_EXTRA_LIBS $LIBS"
CFLAGS="$X_CFLAGS $CFLAGS"
CPPFLAGS="$X_CFLAGS $CPPFLAGS"
LDFLAGS="$X_LIBS $LDFLAGS"
xdvi_cv_xpm_libraries=
#
#
# We use XtToolkitInitialize() here since it takes no arguments
# and thus also works with a C++ compiler.
AC_TRY_LINK([
#include <X11/Intrinsic.h>
#include <X11/xpm.h>
],[XtToolkitInitialize();],
[
# libxpm.a is in the standard search path.
xdvi_cv_xpm_libraries="default"
],
[
# libXpm.a is not in the standard search path.
# Locate it and put its directory in `xpm_libraries'
#
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
        xdvi_cv_xpm_libraries="$dir"
        break
    fi
done
])
#
LIBS="$xdvi_xpm_save_LIBS"
CFLAGS="$xdvi_xpm_save_CFLAGS"
CPPFLAGS="$xdvi_xpm_save_CPPFLAGS"
LDFLAGS="$xdvi_xpm_save_LDFLAGS"
])
#
xpm_libraries="$xdvi_cv_xpm_libraries"
fi
#
# Add Xpm definitions to X flags
#
if test "x$xpm_includes" != "xdefault" && test "$xpm_includes" != "" \
    && test "$xpm_includes" != "$x_includes" && test "$xpm_includes" != "no"
then
    X_CFLAGS="-I$xpm_includes $X_CFLAGS"
fi

if test "x$xpm_libraries" != "xdefault" && test "$xpm_libraries" != "" \
    && test "$xpm_libraries" != "$x_libraries" && test "$xpm_libraries" != "no"
then
case "$X_LIBS" in
  *-R\ *) X_LIBS="-L$xpm_libraries -R $xpm_libraries $X_LIBS";;
  *-R*)   X_LIBS="-L$xpm_libraries -R$xpm_libraries $X_LIBS";;
  *)      X_LIBS="-L$xpm_libraries $X_LIBS";;
esac
fi

#
#
x_xpm_libs="-lXpm"
#
# Now check the results of headers and libraries and set USE_XPM to 0
# if one of them hadn't been found.
#
AC_DEFINE([USE_XPM], 1, [Define if you want to use the Xpm library])
xpm_libraries_result="$xpm_libraries"
xpm_includes_result="$xpm_includes"
if test "x$xpm_libraries_result" = "xdefault" ; then
  xpm_libraries_result="in default path"
elif test "$xpm_libraries_result" = no || test "x$xpm_libraries_result" = "x"; then
  xpm_libraries_result="(none)"
  AC_DEFINE([USE_XPM], 0)
  x_xpm_libs=""
fi

if test "x$xpm_includes_result" = "xdefault"; then
  xpm_includes_result="in default path"
elif test "$xpm_includes_result" = no || test "x$xpm_includes_result" = "x"; then
  AC_DEFINE(USE_XPM, 0)
  xpm_includes_result="(none)"
fi

AC_SUBST(x_xpm_libs)
AC_MSG_RESULT(
  [libraries $xpm_libraries_result, headers $xpm_includes_result])
])dnl



