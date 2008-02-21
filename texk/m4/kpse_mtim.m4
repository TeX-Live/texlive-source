# Private macro for the kpathsea library.
# Copyright (C) 1995 - 2008 Karl Berry <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 1

# KPSE_STRUCT_ST_MTIM
# -------------------
# Test for member st_mtim in struct stat
# Should use AC_CHECK_MEMBER or AC_CHECK_MEMBERS
AC_DEFUN([KPSE_STRUCT_ST_MTIM],
[AC_CACHE_CHECK([for st_mtim in struct stat], ac_cv_struct_st_mtim,
[AC_TRY_COMPILE([#include <sys/types.h>
#include <sys/stat.h>], [struct stat s; s.st_mtim;],
ac_cv_struct_st_mtim=yes, ac_cv_struct_st_mtim=no)])
if test $ac_cv_struct_st_mtim = yes; then
  AC_DEFINE(HAVE_ST_MTIM)
fi
])

