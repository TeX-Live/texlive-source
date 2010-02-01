# Autoconf macros for zziplib.
# Copyright (C) 2006, 2009 Guido U. Draheim <guidod@gmx.de>
# Copyright (C) 2010 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# While the x86 CPUs allow access to memory objects to be unaligned
# it happens that most of the modern designs require objects to be
# aligned - or they will fail with a buserror. That mode is quite known
# by big-endian machines (sparc, etc) however the alpha cpu is little-
# endian.

AC_DEFUN([ZZIPLIB_CHECK_ALIGNED_ACCESS],
[AC_REQUIRE([AC_CANONICAL_HOST])
AC_CACHE_CHECK([if pointers to integers require aligned access],
               [zziplib_cv_aligned_access],
               [AC_RUN_IFELSE([AC_LANG_PROGRAM([[
#include <stdio.h>
#include <stdlib.h>]],
                                               [[
char* string = malloc(40);
int i;
for (i=0; i < 40; i++) string[i] = i;
{
   void* s = string;
   int* p = s+1;
   int* q = s+2;
   if (*p == *q) { return 1; }
}]])],
                              [zziplib_cv_aligned_access=no],
                              [zziplib_cv_aligned_access=yes],
                              [AS_CASE([$host_cpu],
                                       [alpha*|arm*|bfin*|hp*|mips*|sh*|sparc*|ia64|nv1],
                                         [zziplib_cv_aligned_access=yes],
                                       [zziplib_cv_aligned_access=no])])])
if test "x$zziplib_cv_aligned_access" = xyes ; then
  AC_DEFINE([HAVE_ALIGNED_ACCESS_REQUIRED], 1,
            [Define if pointers to integers require aligned access])
fi
]) # ZZIPLIB_CHECK_ALIGNED_ACCESS

