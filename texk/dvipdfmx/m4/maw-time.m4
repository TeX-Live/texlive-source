# maw-time.m4 - timezone tests
# Copyright (C) 1998 - 2008 Mark A. Wicks <mwicks@kettering.edu>
# Kettering University
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 1

# MAW_EXT_TIMEZONE
# ----------------
AC_DEFUN([MAW_EXT_TIMEZONE],
[AC_MSG_CHECKING([whether time.h defines timezone as an external variable])
 AC_TRY_LINK(
  [#include <time.h>],
  [-timezone;],
  [AC_MSG_RESULT(yes)
   AC_DEFINE(HAVE_TIMEZONE, [], [external timezone variable])],
  [AC_MSG_RESULT(no)])
])# MAW_EXT_TIMEZONE

# MAW_TZ_HAS_TM_GMTOFF
# --------------------
AC_DEFUN([MAW_TZ_HAS_TM_GMTOFF],
[AC_MSG_CHECKING([whether struct tz has tm_gmtoff as a member])
 AC_TRY_COMPILE(
  [#include <time.h>],
  [struct tm *tp; tp->tm_gmtoff],
  [AC_MSG_RESULT(yes)
   AC_DEFINE(HAVE_TM_GMTOFF, [], [struct tz has tm_gmtoff])],
  [AC_MSG_RESULT(no)])
])# MAW_TZ_HAS_TM_GMTOFF

