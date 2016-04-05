# Autoconf macros for xdvik.
# Copyright (C) 2014 Paul Vojta <xdvi-core@lists.sourceforge.net>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# XDVI_XINPUT_2_1
# ---------------
# Check for whether XInput version 2.1 is supported in the X libraries.

dnl ### Check for XInput 2.1 functionality.
AC_DEFUN([XDVI_XINPUT_2_1], [dnl
AC_ARG_ENABLE([xi2-scrolling],
AS_HELP_STRING([--enable-xi2-scrolling],
[Use XInput 2.1 'smooth scrolling' if available (default: disabled
 for native TL build)]))
AS_CASE([$enable_xi2_scrolling],
        [yes | no], [:],
        [AS_IF([test "x$enable_native_texlive_build" = xyes],
               [enable_xi2_scrolling=no],
               [enable_xi2_scrolling=yes])])
if test "x$enable_xi2_scrolling" = xyes; then
AC_CACHE_CHECK([for XInput version 2.1 or higher],
               [xdvi_cv_xinput_2_1], [dnl
xdvi_xi2_save_LIBS=$LIBS
xdvi_xi2_save_CFLAGS=$CFLAGS
xdvi_xi2_save_CPPFLAGS=$CPPFLAGS
xdvi_xi2_save_LDFLAGS=$LDFLAGS
LIBS="$X_PRE_LIBS -lXi $X_EXTRA_LIBS $LIBS"
CFLAGS="$X_CFLAGS $CFLAGS"
CPPFLAGS="$X_CFLAGS $CPPFLAGS"
LDFLAGS="$X_LIBS $LDFLAGS"
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <X11/extensions/XInput2.h>]],
                                [[Display *DISP; int ndevices;
XIDeviceInfo *info = XIQueryDevice(DISP, XIAllDevices, &ndevices);
#if (XI_2_Major < 2 || (XI_2_Major == 2 && XI_2_Minor < 1))
choke me "XInput version is < 2.1"
#endif]])],
               [xdvi_cv_xinput_2_1=yes],
               [xdvi_cv_xinput_2_1=no])
LIBS=$xdvi_xi2_save_LIBS
CFLAGS=$xdvi_xi2_save_CFLAGS
CPPFLAGS=$xdvi_xi2_save_CPPFLAGS
LDFLAGS=$xdvi_xi2_save_LDFLAGS])
if test "x$xdvi_cv_xinput_2_1" = xyes; then
  AC_DEFINE([HAVE_XI21], 1,
    [Define if your system has XInput version 2.1 or higher.])
  X_PRE_LIBS="$X_PRE_LIBS -lXi"
fi
fi])
