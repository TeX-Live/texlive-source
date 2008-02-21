# Private macros for the kpathsea library.
# Copyright (C) 1995 - 2008 Karl Berry <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 2

# AC_UNSET_CC
# -----------
# Unset CC to run configure with cross compiler.
AC_DEFUN([AC_UNSET_CC], [
ZZ=
if test "$cross_compiling" = yes &&
   (test "x$CC" = "xdos-gcc" || test "x$CC" = "xi386-mingw32-gcc" || test "x$CC" = "xgnuwin32gcc") ; then
ZZ=$CC
unset CC
cross_compiling=no
fi
])

# AC_RESET_CC
# -----------
# Restore CC that has been unset by AC_UNSET_CC
AC_DEFUN([AC_RESET_CC], [
if test "x$ZZ" = "xdos-gcc" || test "x$ZZ" = "xi386-mingw32-gcc" || test "x$ZZ" = "xgnuwin32gcc" ; then
CC=$ZZ
cross_compiling=yes
fi
])

