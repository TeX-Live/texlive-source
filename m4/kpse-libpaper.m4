# Public macros for the TeX Live (TL) tree.
# Copyright (C) 1998-2008 Jin-Hwan Cho <chofchof@ktug.or.kr>
# Copyright (C) 2008-2013 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_LIBPAPER
# -------------
# Provide the configure options '--with-paper-includes', '--with-paper-libdir'
# and check for the ortional libpaper.  If found define HAVE_LIBPAPER.
# Set the make variables PAPER_INCLUDES and PAPER_LIBS to the CPPFLAGS and
# LIBS required for the installed (system) library.
AC_DEFUN([KPSE_LIBPAPER], [dnl
AC_REQUIRE([KPSE_SAVE_FLAGS])
AC_ARG_WITH([paper-includes],
            AS_HELP_STRING([--with-paper-includes=DIR],
                           [paper headers installed in DIR]))[]dnl
AC_ARG_WITH([paper-libdir],
            AS_HELP_STRING([--with-paper-libdir=DIR],
                           [paper library installed in DIR]))[]dnl
AS_CASE([$with_paper_includes],
        ["" | yes], [PAPER_INCLUDES=],
                    [PAPER_INCLUDES="-I$with_paper_includes"])
AS_CASE([$with_paper_libdir],
        ["" | yes], [PAPER_LIBS=],
                    [PAPER_LIBS="-L$with_paper_libdir"])
CPPFLAGS="$CPPFLAGS $PAPER_INCLUDES"
LIBS="$LIBS $PAPER_LIBS"
AC_MSG_CHECKING([for paper header files])
result=no
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <paper.h>]],
                                   [[struct paper *p;]])],
                  [AC_SEARCH_LIBS([paperpswidth], [paper])
                   AS_CASE([$ac_cv_search_paperpswidth],
                           [no], [:],
                           ["none required"], [result=yes],
                                              [result=yes
                                               PAPER_LIBS="$PAPER_LIBS $ac_cv_search_paperpswidth"])])
AS_CASE([$result],
        [no], [PAPER_INCLUDES= PAPER_LIBS=],
              [AC_DEFINE([HAVE_LIBPAPER], 1, [Define if you have libpaper])])
AC_MSG_RESULT([$result])
AC_SUBST([PAPER_INCLUDES])
AC_SUBST([PAPER_LIBS])
KPSE_RESTORE_FLAGS
])# KPSE_LIBPAPER

