# Autoconf macros for xdvik.
# Copyright (C) 2002 - 2009 Paul Vojta <xdvi-core@lists.sourceforge.net>
# Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# XDVI_LINKER_MULTIPLE_DEFNS
# --------------------------
# Check if the --allow-multiple-definition linker flag is
# available (assuming that we need to use it if it is).
# If it isn't, check if the linker accepts multiple definitions,
# and if it doesn't, don't define LD_ALLOWS_MULTIPLE_DEFINITIONS.

AC_DEFUN([XDVI_LINKER_MULTIPLE_DEFNS],
[AC_CACHE_CHECK(
   [if linker supports multiple definitions and for required flags],
   [xdvi_cv_linker_multiple_defns],
   [xdvi_ld_save_LDFLAGS=$LDFLAGS
    LDFLAGS="-Xlinker --allow-multiple-definition"
    AC_LINK_IFELSE(
      [AC_LANG_PROGRAM([[#include <stdio.h>]], [[void foo(void);]])],
      [xdvi_cv_linker_multiple_defns=$LDFLAGS],
      [xdvi_ld_save_LIBS=$LIBS
       xdvi_ld_save_CFLAGS=$CFLAGS
       xdvi_ld_save_CPPFLAGS=$CPPFLAGS

       LIBS="$X_PRE_LIBS -lXt -lX11 $X_EXTRA_LIBS $LIBS"
       CFLAGS="$X_CFLAGS $CFLAGS"
       CPPFLAGS="$X_CFLAGS $CPPFLAGS"
       LDFLAGS="$X_LIBS $xdvi_ld_save_LDFLAGS"

       AC_LINK_IFELSE(
         [AC_LANG_PROGRAM(
            [[#include <X11/Intrinsic.h>

              XtIntervalId XtAppAddTimeOut(XtAppContext app,
                                           unsigned long interval,
                                           XtTimerCallbackProc proc,
                                           XtPointer closure)
              {
                  (void)app; (void)interval; (void)proc; (void)closure;
                  return (XtIntervalId)0;
              }]],
            [[XtIntervalId i = 0; XtRemoveTimeOut(i);]])],
         [xdvi_cv_linker_multiple_defns="none required"],
         [xdvi_cv_linker_multiple_defns=unsupported])

       LIBS=$xdvi_ld_save_LIBS
       CFLAGS=$xdvi_ld_save_CFLAGS
       CPPFLAGS=$xdvi_ld_save_CPPFLAGS])
    LDFLAGS=$xdvi_ld_save_LDFLAGS])

x_linker_options=""
AS_CASE([$xdvi_cv_linker_multiple_defns],
        ["none required"], [AC_DEFINE([LD_ALLOWS_MULTIPLE_DEFINITIONS], 1,
                                      [Define if your system allows multiple definitions of functions.])],
        [unsupported],  [AC_MSG_WARN([Linker does not allow multiple definitions.
  *****************************************************************
  * Warning: Your linker does not allow multiple definitions.     *
  * This does not make xdvik unusable, but it will cause problems *
  * with event handling: Some widgets, e.g. the print log window, *
  * tooltips, statusline messages and hyperlink location markers  *
  * will not be updated until the mouse is moved.                 *
  *****************************************************************])],
        [x_linker_options=$xdvi_cv_linker_multiple_defns
         AC_DEFINE([LD_ALLOWS_MULTIPLE_DEFINITIONS], 1)])
AC_SUBST([x_linker_options])
]) # XDVI_LINKER_MULTIPLE_DEFNS
