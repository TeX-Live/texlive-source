# Public macros for the TeX Live (TL) tree.
# Copyright (C) 1998 - 2008 Mark A. Wicks <mwicks@kettering.edu>
# Kettering University
# Copyright (C) 2008 - 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 2

# MAW_EXT_TIMEZONE
# ----------------
AC_DEFUN([MAW_EXT_TIMEZONE],
[AC_CACHE_CHECK([whether time.h defines timezone as an external variable],
                [maw_cv_ext_timezone],
                [AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <time.h>]],
                                                 [[-timezone;]])],
                                [maw_cv_ext_timezone=yes],
                                [maw_cv_ext_timezone=no])])
AS_IF([test "x$maw_cv_ext_timezone" = xyes],
      [AC_DEFINE([HAVE_TIMEZONE], 1,
                 [Define if <time.h> has timezone as an external variable.])])
])# MAW_EXT_TIMEZONE

# MAW_TM_HAS_TM_GMTOFF
# --------------------
AC_DEFUN([MAW_TM_HAS_TM_GMTOFF],
[AC_CHECK_MEMBER([struct tm.tm_gmtoff],
                 [AC_DEFINE([HAVE_TM_GMTOFF], 1,
                            [Define if struct tm has tm_gmtoff as a member.])], ,
                 [[#include <time.h>]])
])# MAW_TM_HAS_TM_GMTOFF

