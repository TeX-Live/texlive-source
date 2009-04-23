# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 0

# KPSE_CHECK_WIN32
# ----------------
# Check for WIN32 and distinguish between MINGW32 and native.
AC_DEFUN([KPSE_CHECK_WIN32],
[AC_CACHE_CHECK([for native WIN32 or MINGW32],
                [kpse_cv_have_win32],
                [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#ifndef WIN32
  choke me
#endif]],                                           [[]])],
                                   [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#ifndef __MINGW32__
  choke me
#endif]],                                                              [[]])],
                                                      [kpse_cv_have_win32=mingw32],
                                                      [kpse_cv_have_win32=native])],
                                   [kpse_cv_have_win32=no])])
]) # KPSE_CHECK_WIN32
