dnl 
dnl ice_find_athena.m4
dnl 
dnl -----------------------------------------------------------
dnl 
dnl  * To: autoconf-collection@peti.gmd.de
dnl  * Subject: ice_find_athena.m4
dnl  * From: Andreas Zeller <zeller@ips.cs.tu-bs.de>
dnl  * Date: Sat, 29 Nov 1997 17:14:09 +0100 (MET)
dnl  * Sender: autoconf-collection-owner@peti.gmd.de
dnl 
dnl -----------------------------------------------------------
dnl 
dnl ICE_FIND_ATHENA
dnl ---------------
dnl
dnl Find Athena libraries and headers.
dnl Put Athena include directory in athena_includes,
dnl put Athena library directory in athena_libraries,
dnl and add appropriate flags to X_CFLAGS and X_LIBS.
dnl
dnl
AC_DEFUN(ICE_FIND_ATHENA,
[
AC_REQUIRE([AC_PATH_XTRA])
athena_includes=
athena_libraries=
AC_ARG_WITH(athena,
[  --without-athena        do not use Athena widgets])
dnl Treat --without-athena like
dnl --without-athena-includes --without-athena-libraries.
if test "$with_athena" = "no"
then
athena_includes=no
athena_libraries=no
fi
AC_ARG_WITH(athena-includes,
[  --with-athena-includes=DIR   Athena include files are in DIR],
athena_includes="$withval")
AC_ARG_WITH(athena-libraries,
[  --with-athena-libraries=DIR  Athena libraries are in DIR],
athena_libraries="$withval")
AC_MSG_CHECKING(for Athena)
#
#
# Search the include files.
#
if test "$athena_includes" = ""; then
AC_CACHE_VAL(ice_cv_athena_includes,
[
ice_athena_save_LIBS="$LIBS"
ice_athena_save_CFLAGS="$CFLAGS"
ice_athena_save_CPPFLAGS="$CPPFLAGS"
ice_athena_save_LDFLAGS="$LDFLAGS"
#
LIBS="$X_PRE_LIBS -lXaw -lXmu -lXext -lXt -lX11 $X_EXTRA_LIBS $LIBS"
CFLAGS="$X_CFLAGS $CFLAGS"
CPPFLAGS="$X_CFLAGS $CPPFLAGS"
LDFLAGS="$X_LIBS $LDFLAGS"
#
AC_TRY_COMPILE([
#include <X11/Intrinsic.h>
#include <X11/Xaw/Text.h>
],[int a;],
[
# X11/Xaw/Text.h is in the standard search path.
ice_cv_athena_includes=
],
[
# X11/Xaw/Text.h is not in the standard search path.
# Locate it and put its directory in `athena_includes'
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
if test -f "$dir/X11/Xaw/Text.h"; then
ice_cv_athena_includes="$dir"
break
fi
done
])
#
LIBS="$ice_athena_save_LIBS"
CFLAGS="$ice_athena_save_CFLAGS"
CPPFLAGS="$ice_athena_save_CPPFLAGS"
LDFLAGS="$ice_athena_save_LDFLAGS"
])
athena_includes="$ice_cv_athena_includes"
fi
#
#
# Now for the libraries.
#
if test "$athena_libraries" = ""; then
AC_CACHE_VAL(ice_cv_athena_libraries,
[
ice_athena_save_LIBS="$LIBS"
ice_athena_save_CFLAGS="$CFLAGS"
ice_athena_save_CPPFLAGS="$CPPFLAGS"
ice_athena_save_LDFLAGS="$LDFLAGS"
#
LIBS="$X_PRE_LIBS -lXaw -lXmu -lXext -lXt -lX11 $X_EXTRA_LIBS $LIBS"
CFLAGS="$X_CFLAGS $CFLAGS"
CPPFLAGS="$X_CFLAGS $CPPFLAGS"
LDFLAGS="$X_LIBS $LDFLAGS"
#
AC_TRY_LINK([
#include <X11/Intrinsic.h>
#include <X11/Xaw/Text.h>
],[XtToolkitInitialize();],
[
# libXaw.a is in the standard search path.
ice_cv_athena_libraries=
],
[
# libXaw.a is not in the standard search path.
# Locate it and put its directory in `athena_libraries'
#
#
# /usr/lib/Motif* are used on HP-UX (Motif).
# /usr/lib/X11* are used on HP-UX (X and Athena).
# /usr/dt is used on Solaris (Motif).
# /usr/openwin is used on Solaris (X and Athena).
# Other directories are just guesses.
for dir in "$x_libraries" "${prefix}/lib" /usr/lib /usr/local/lib \
           /usr/lib/Motif2.0 /usr/lib/Motif1.2 /usr/lib/Motif1.1 \
           /usr/lib/X11R6 /usr/lib/X11R5 /usr/lib/X11R4 /usr/lib/X11 \
           /usr/dt/lib /usr/openwin/lib \
           /usr/dt/*/lib /opt/*/lib /usr/lib/Motif* \
           "${prefix}"/*/lib /usr/*/lib /usr/local/*/lib \
           "${prefix}"/lib/* /usr/lib/* /usr/local/lib/*; do
if test -d "$dir" && test "`ls $dir/libXaw.* 2> /dev/null`" != ""; then
ice_cv_athena_libraries="$dir"
break
fi
done
])
#
LIBS="$ice_athena_save_LIBS"
CFLAGS="$ice_athena_save_CFLAGS"
CPPFLAGS="$ice_athena_save_CPPFLAGS"
LDFLAGS="$ice_athena_save_LDFLAGS"
])
#
athena_libraries="$ice_cv_athena_libraries"
fi
# Add Athena definitions to X flags
#
if test "$athena_includes" != "" && test "$athena_includes" != "$x_includes" && test "$athena_includes" != "no"
then
X_CFLAGS="-I$athena_includes $X_CFLAGS"
fi
if test "$athena_libraries" != "" && test "$athena_libraries" != "$x_libraries" && test "$athena_libraries" != "no"
then
case "$X_LIBS" in
  *-R\ *) X_LIBS="-L$athena_libraries -R $athena_libraries $X_LIBS";;
  *-R*)   X_LIBS="-L$athena_libraries -R$athena_libraries $X_LIBS";;
  *)      X_LIBS="-L$athena_libraries $X_LIBS";;
esac
fi
#
#
athena_libraries_result="$athena_libraries"
athena_includes_result="$athena_includes"
test "$athena_libraries_result" = "" &&
  athena_libraries_result="in default path"
test "$athena_includes_result" = "" &&
  athena_includes_result="in default path"
test "$athena_libraries_result" = "no" &&
  athena_libraries_result="(none)"
test "$athena_includes_result" = "no" &&
  athena_includes_result="(none)"
AC_MSG_RESULT(
  [libraries $athena_libraries_result, headers $athena_includes_result])
])dnl

