# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2011, 2012 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_TL_PLATFORM
# ----------------
# Determine the TeX Live platform name.
AC_DEFUN([KPSE_TL_PLATFORM],
[AC_REQUIRE([AC_CANONICAL_HOST])[]dnl
AC_REQUIRE([KPSE_CHECK_WIN32])[]dnl
AC_REQUIRE([KPSE_CHECK_PERL])[]dnl
AC_ARG_VAR([TL_PLATFORM], [TeX Live platform name [autodetected]])
AC_MSG_CHECKING([for TeX Live platform name])
if test "X$TL_PLATFORM" = X; then
  if test "x$kpse_cv_have_win32" = xno; then
    TL_PLATFORM=`$PERL -I"$srcdir" -MTeXLive::TLUtils -e '
      print TeXLive::TLUtils::platform_name("'"$ac_cv_host"'");'`
  else
    TL_PLATFORM=win32
  fi
fi
AC_MSG_RESULT([$TL_PLATFORM])
]) # KPSE_TL_PLATFORM

