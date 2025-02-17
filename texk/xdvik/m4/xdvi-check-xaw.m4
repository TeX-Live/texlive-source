# Autoconf macros for xdvik.
# Copyright (C) 2003 - 2009 Stefan Ulrich <xdvi-core@lists.sourceforge.net>
# Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# XDVI_CHECK_XAW_HEADERS
# ----------------------
# Check for Xaw headers and library version.
AC_DEFUN([XDVI_CHECK_XAW_HEADERS],
[save_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$CPPFLAGS $X_CFLAGS"
AC_CACHE_CHECK([for Xaw headers],
               [xdvi_cv_xaw_headers],
               [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xfuncs.h>
#include <X11/Intrinsic.h>
#include <X11/Xaw/Form.h>]])],
                                  [xdvi_cv_xaw_headers=yes],
                                  [xdvi_cv_xaw_headers=no])])
xdvi_have_xaw=$xdvi_cv_xaw_headers
#
if test "x$xdvi_have_xaw" = xyes; then
 _XDVI_CHECK_XAW_VERSION
fi
CPPFLAGS=$save_CPPFLAGS
]) # XDVI_CHECK_XAW_HEADERS

# _XDVI_CHECK_XAW_VERSION
# -----------------------
# Check Xaw version.
m4_define([_XDVI_CHECK_XAW_VERSION],
[AC_CHECK_MEMBER([SimpleClassPart.extension],
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
]) # _XDVI_CHECK_XAW_VERSION

# XDVI_CHECK_XAW_LIBRARY
# ----------------------
# Check for Xaw library.
# If found, set prog_extension and x_tool_libs, and define XAW.
AC_DEFUN([XDVI_CHECK_XAW_LIBRARY],
[AC_REQUIRE([XDVI_CHECK_XAW_HEADERS])
if test "x$xdvi_have_xaw" = xyes; then
  # First try without libXp
  AC_CHECK_LIB([Xaw], [XawInitializeWidgetSet],
      [x_tool_libs="-lXaw"],
      [# libXaw without libXp failed
       if test "x$x_xp_lib" = x; then
         xdvi_have_xaw=no
       else
         # Now try with libXp
         AC_CHECK_LIB([Xaw], [XawTextReplace],
             [x_tool_libs="-lXaw $x_xp_lib"],
             [xdvi_have_xaw=no],
             [$x_xp_lib $x_xmu_lib -lXt $X_PRE_LIBS $x_ext_lib $x_xpm_libs -lX11 $X_EXTRA_LIBS $XLFLAG])
       fi],
      [$x_xmu_lib -lXt $X_PRE_LIBS $x_ext_lib $x_xpm_libs -lX11 $X_EXTRA_LIBS $XLFLAG])
fi
if test "x$xdvi_have_xaw" = xyes; then
  prog_extension="xaw"
  AC_DEFINE([XAW], 1, [Define to use the Xaw toolkit.])

  # We need the type of the "list" parameter to XawListChange
  # (the second parameter), for use in gui/pagesel.c. See more comments there.
  #
  AC_MSG_CHECKING([for type of XawListChange list parameter])
  # Although we could try to use the compiler to discern the location of
  # <X11/Xaw/List.h>, that seems likely to have its own problems. Let's
  # try just assuming the standard location.
  #
  listh=/usr/include/X11/Xaw/List.h

  # Clearly this simple search is subject to plenty of problems in
  # theory, but in practice, if no one gratuitously changes the
  # formatting in Xaw/List.h, it should be ok? The line intended to be
  # matched looks like
  #   String                      *list, [except with tabs]
  # or
  #   _Xconst char *list,
  # or who knows what else. Hopefully it will be the complete type.
  #
  list_type=`sed -n 's/list,$//p' $listh`
  #
  # The type ordinarily has lots of whitespace, as in the String line above.
  # People will uselessly complain about that, so reduce it to a single space:
  tab='	' # that's a tab character
  # The outer square brackets are the Autoconf quote pair;
  # the inner square brackets are the normal regexp character class.
  #echo "got '$list_type' from $listh, tab=$tab." >/tmp/lh
  list_type=`echo "$list_type" | sed ["s/[ $tab][ $tab]*/ /g"]`
  #echo "new '$list_type' from $listh" >>/tmp/lh
  #
  if test -z "$list_type"; then
    # But if we didn't find anything, default to the type of our
    # page_labels member in pagesel.c, which has been the traditional
    # effective behavior.
    list_type="char **"
  fi
  AC_MSG_RESULT([$list_type])
  #debug: echo "got '$list_type' from $listh" >/tmp/lh
  #
  # autoconf internally quotes the shell variable value with
  # ac_define_unquoted; we don't use shell "quotes" here since the final
  # value must not have any quotes, being a type that we want to use in
  # a cast, not a string constant.
  AC_DEFINE_UNQUOTED([XAWLISTCHANGELISTTYPE], $list_type,
                     [type of XawListChange list parameter])
else
  AC_MSG_ERROR([Sorry, you will need at least the Xaw header/library files to compile xdvik.])
fi
]) # XDVI_CHECK_XAW_LIBRARY
