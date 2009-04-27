# Autoconf macros for xdvik.
# Copyright (C) 2003 - 2009 Stefan Ulrich <xdvi-core@lists.sourceforge.net>
# Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

##

dnl ### Check for Xaw version
AC_DEFUN([XDVI_CHECK_XAW], [_XDVI_CHECK_XAW_HEADERS])

# _XDVI_CHECK_XAW_HEADERS
# -----------------------
# Check for Xaw headers.
m4_define([_XDVI_CHECK_XAW_HEADERS],
[save_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$CPPFLAGS $X_CFLAGS"
AC_MSG_CHECKING([for Xaw library])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include <X11/Xfuncs.h>
#include <X11/Intrinsic.h>
#include <X11/Xaw/Form.h>]])],
                  [xdvi_have_xaw=yes],
                  [xdvi_have_xaw=no])
AC_MSG_RESULT([$xdvi_have_xaw])
#
if test "x$xdvi_have_xaw" = xyes; then
 _XDVI_CHECK_XAW_VERSION
fi
CPPFLAGS=$save_CPPFLAGS
]) # _XDVI_CHECK_XAW_HEADERS

# _XDVI_CHECK_XAW_VERSION
# -----------------------
# Check Xaw version.
m4_define([_XDVI_CHECK_XAW_VERSION],
[
save_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$CPPFLAGS $X_CFLAGS"
AC_CHECK_MEMBER([SimpleClassPart.extension],
                [],
                [AC_DEFINE([HAVE_OLD_XAW], 1,
                           [Define if you have an old version of the Xaw library])],
                [[
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xos.h>
#include <X11/Xaw/SimpleP.h>]])
CPPFLAGS=$save_CPPFLAGS
]) # _XDVI_CHECK_XAW_VERSION
