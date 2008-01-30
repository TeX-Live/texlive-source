# Private macro for the kpathsea library.
# by Karl Berry <karl@freefriends.org>
# Copyright (C) 199? - 2008
# Free Software Foundation, Inc.
#
# This file is free software; the Free Software Foundation
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

