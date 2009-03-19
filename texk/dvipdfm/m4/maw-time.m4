# maw-time.m4 - timezone tests
# Copyright (C) 1998 - 2009 Mark A. Wicks <mwicks@kettering.edu>
# Kettering University
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 2

# MAW_EXT_TIMEZONE
# ----------------
AC_DEFUN([MAW_EXT_TIMEZONE],
[AC_MSG_CHECKING([whether time.h defines timezone as an external variable])
 AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <time.h>]],
                                 [[-timezone;]])],
                [AC_MSG_RESULT([yes])
                 AC_DEFINE([HAVE_TIMEZONE], 1,
                           [Define if <time.h> has timezone as an external variable.])],
                [AC_MSG_RESULT([no])])
])# MAW_EXT_TIMEZONE

# MAW_TM_HAS_TM_GMTOFF
# --------------------
AC_DEFUN([MAW_TM_HAS_TM_GMTOFF],
[AC_MSG_CHECKING([whether struct tm has tm_gmtoff as a member])
 AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <time.h>]],
                                 [[struct tm *tp; tp->tm_gmtoff;]])],
                [AC_MSG_RESULT([yes])
                 AC_DEFINE([HAVE_TM_GMTOFF], 1,
                           [Define if struct tm has tm_gmtoff as a member.])],
                [AC_MSG_RESULT([no])])
])# MAW_TM_HAS_TM_GMTOFF

