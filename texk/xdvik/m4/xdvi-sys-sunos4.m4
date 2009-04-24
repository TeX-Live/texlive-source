# Autoconf macros for xdvik.
# Copyright (C) 1999 - 2009 Paul Vojta <xdvi-core@lists.sourceforge.net>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# XDVI_SYS_SUNOS_4
# ----------------
# Check for SunOS 4.
AC_DEFUN([XDVI_SYS_SUNOS_4],
[AC_CACHE_CHECK([for SunOS 4],
                [xdvi_cv_sys_sunos_4],
                [AS_CASE(["`(uname -sr) 2>/dev/null`"],
                         ["SunOS 4."*],
                         [xdvi_cv_sys_sunos_4=yes],
                         [xdvi_cv_sys_sunos_4=no])])
if test "x$xdvi_cv_sys_sunos_4" = xyes; then
  AC_DEFINE([SUNOS4], 1, [Define if you are using SunOS 4.x.])
fi
]) # XDVI_SYS_SUNOS_4
