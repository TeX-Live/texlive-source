# maw-macos.m4 - Mac OS X ApplicationServices framework
# Copyright (C) 2005 - 2008 Jin-Hwan Cho <chofchof@ktug.or.kr>
# Some Affiliation ?????????????
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 1

# CHO_HAS_APP_SERVICES
# --------------------
# Check for ApplicationServices
AC_DEFUN([CHO_HAS_APP_SERVICES],
[AC_MSG_CHECKING([for Mac OS X ApplicationServices framework])
 AC_TRY_COMPILE(
  [#include <ApplicationServices/ApplicationServices.h>],
  [ATSFontRef fontRef;],
  [have_app_services=yes
   AC_DEFINE([HAVE_APP_SERVICES], 1, [Define if you have Mac OS X ApplicationServices])
   LIBS="$LIBS -framework ApplicationServices"],
  [have_app_services=no])
 AC_MSG_RESULT([$have_app_services])
])# CHO_HAS_APP_SERVICES

