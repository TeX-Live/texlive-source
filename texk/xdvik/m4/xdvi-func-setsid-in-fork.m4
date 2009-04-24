# Autoconf macros for xdvik.
# Copyright (C) 2002 - 2009 Paul Vojta <xdvi-core@lists.sourceforge.net>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# XDVI_FUNC_SETSID_IN_VFORK
# -------------------------
# Check for whether setsid() is allowed within vfork()
# (Mac OS X 10.3 (Panther, 11/2003) is one O/S which does not allow this).
AC_DEFUN([XDVI_FUNC_SETSID_IN_VFORK],
[AS_IF([test "x$ac_cv_func_vfork_works" = xyes],
[AC_CACHE_CHECK([for whether setsid() is allowed within vfork()],
                [xdvi_cv_setsid_in_vfork],
                [AC_RUN_IFELSE([AC_LANG_PROGRAM([[
/* Test adapted from Gnu autoconf */
/* Thanks to Paul Eggert for this test.  */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_VFORK_H
#include <vfork.h>
#endif]],
                                                [[
  pid_t parent = getpid ();
  pid_t child;

  child = vfork ();

  if (child == 0) {
    if (setsid () == -1)
      _exit(1);
    _exit(0);
  } else {
    int status;

    while (wait(&status) != child)
      ;
    exit(
	 /* Was there some problem with vforking?  */
	 child < 0

	 /* Did the child fail?  (This shouldn't happen.)  */
	 || status
	 );
  }]])],
                               [xdvi_cv_setsid_in_vfork=yes],
                               [xdvi_cv_setsid_in_vfork=no],
                               [xdvi_cv_setsid_in_vfork=no # safe value for cross-compiling])])
if test "x$xdvi_cv_setsid_in_vfork" = xyes; then
  AC_DEFINE([HAVE_GOOD_SETSID_VFORK], 1,
            [Define if your system allows setsid() within vfork().])
fi])
]) # XDVI_FUNC_SETSID_IN_VFORK
