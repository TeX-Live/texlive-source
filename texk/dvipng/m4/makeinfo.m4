# Autoconf macros for dvipng.
# Copyright (C) 2002-2008 Jan-Åke Larsson <jan-ake.larsson@liu.se>
# Copyright (C) 2008-2013 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.
#
# Extracted from dvipng-1.9/aclocal.m4 and adapted for use in TeX Live.

# MAKEINFO_CHECK_MACROS(MACRO ...)
# --------------------------------
# For each MACRO check if makeinfo understands @MACRO{}.
# Prepend '-D no-MACRO' for each MACRO  not understood to the
# output variable AM_MAKEINFOFLAGS.
AC_DEFUN([MAKEINFO_CHECK_MACROS], [dnl
if test -n "$MAKEINFO" -a "$MAKEINFO" != ":"; then
  for ac_macro in $1; do
    _MAKEINFO_CHECK_MACRO([$ac_macro])
  done
fi
AC_SUBST([AM_MAKEINFOFLAGS])
])# MAKEINFO_CHECK_MACROS

# _MAKEINFO_CHECK_MACRO(MACRO)
# ----------------------------
# Internal subroutine.  Check if makeinfo understands @MACRO{}
# and prepend '-D no-MACRO' to AM_MAKEINFOFLAGS if not.
m4_define([_MAKEINFO_CHECK_MACRO], [dnl
AC_MSG_CHECKING([if $MAKEINFO understands @$1{}])
echo \\\\input texinfo > conftest.texi
echo @$1{test} >> conftest.texi
if $MAKEINFO conftest.texi > /dev/null 2> /dev/null; then
  AC_MSG_RESULT([yes])	
else  
  AC_MSG_RESULT([no])	
  AM_MAKEINFOFLAGS="-D no-$1 $AM_MAKEINFOFLAGS"
fi
rm -f conftest.texi conftest.info
])# _MAKEINFO_CHECK_MACRO

