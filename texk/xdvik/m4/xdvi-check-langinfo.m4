# Autoconf macros for xdvik.
# Copyright (C) 2004 - 2009 Stefan Ulrich <xdvi-core@lists.sourceforge.net>
# Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# XDVI_CHECK_LANGINFO
# -------------------
# Check for nl_langinfo(), <langinfo.h>, and if nl_langinfo(CODESET)
# is working.
AC_DEFUN([XDVI_CHECK_LANGINFO],
[AC_CHECK_FUNCS([nl_langinfo])
AC_CHECK_HEADERS([langinfo.h])
if test "x$ac_cv_func_nl_langinfo:$ac_cv_header_langinfo_h" = xyes:yes; then
  AC_CACHE_CHECK([if nl_langinfo(CODESET) is working],
                 [xdvi_cv_nl_langinfo_codeset],
                 [AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <langinfo.h>]],
                                                  [[char* cs = nl_langinfo(CODESET);]])],
                                 [xdvi_cv_nl_langinfo_codeset=yes],
                                 [xdvi_cv_nl_langinfo_codeset=no])])
  AS_IF([test "x$xdvi_cv_nl_langinfo_codeset" = xyes],
        [AC_DEFINE([HAVE_WORKING_NL_LANGINFO_CODESET], 1,
                   [Define if the CODESET argument to nl_langinfo works.])])
fi
]) # XDVI_CHECK_LANGINFO
