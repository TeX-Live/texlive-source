# Autoconf macros for xdvik.
# Copyright (C) 2012 Paul Vojta <xdvi-core@lists.sourceforge.net>
# Adapted from xterm, Copyright 1997-2010,2011 by Thomas E. Dickey
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# XDVI_FUNC_XKB_BELL
# -------------------------
# Check for whether the XkbBell() extension is present in the X libraries.

dnl ### Check for XkbBell() functionality.  Adapted from xterm's version.
AC_DEFUN([XDVI_FUNC_XKB_BELL],[
AC_CACHE_CHECK([for XKB Bell extension], xdvi_cv_func_xkb_bell,
[xdvi_save_LIBS=$LIBS
LIBS="$X_PRE_LIBS"$1" $LIBS $X_LIBS -lX11"
AC_TRY_LINK([
#include <X11/X.h>
#include <X11/XKBlib.h>		/* has the prototype */
],[
	Atom y;
	XkbBell((Display *)0, (Window)0, 0, y);
],[xdvi_cv_func_xkb_bell=yes],[xdvi_cv_func_xkb_bell=no])
LIBS=$xdvi_save_LIBS])
if test "$xdvi_cv_func_xkb_bell" = yes; then
  X_PRE_LIBS="$X_PRE_LIBS"$1
  AC_DEFINE([HAVE_XKB_BELL_EXT], 1, [Define if your system has XkbBell().])
fi])
