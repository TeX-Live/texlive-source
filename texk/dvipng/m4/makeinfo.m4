# makeinfo.m4 - Macros to determine makeinfo features.
## Copyright (C) 20xx Some One <someone@somewhere.net>
## Some affiliation
##
## Temporary notice:
## I extracted these macros from aclocal.m4 in the dvipng-1.9 distribution
## and slightly modified them for use with Automake.
## 26 Mar 2008, Peter Breitenlohner <tex-live@tug.org>
##
## This file is free software; the copyright holder gives unlimited
## permission to copy and/or distribute it, with or without
## modifications, as long as this notice is preserved.

# serial 1

# MAKEINFO_CHECK_MACRO( MACRO, [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# ----------------------------------------------------------------------
# Check if makeinfo understands the macro @MACRO{}.  Execute the shell
# code ACTION-IF-FOUND if so, or ACTION-IF-NOT-FOUND otherwise.
AC_DEFUN([MAKEINFO_CHECK_MACRO],
[if test -n "$MAKEINFO" -a "$MAKEINFO" != ":"; then
  AC_MSG_CHECKING([if $MAKEINFO understands @$1{}])
  echo \\\\input texinfo > conftest.texi
  echo @$1{test} >> conftest.texi
  if $MAKEINFO conftest.texi > /dev/null 2> /dev/null; then
    AC_MSG_RESULT(yes)	
    m4_ifval([$2], [$2
])[]dnl
  else  
    AC_MSG_RESULT(no)	
    m4_ifval([$3], [$3
])[]dnl
  fi
  rm -f conftest.texi conftest.info
fi
])# MAKEINFO_CHECK_MACRO

# MAKEINFO_CHECK_MACROS( MACRO ..., [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# ----------------------------------------------------------------------------
# For each MACRO check if makeinfo understands @MACRO{}.  Execute the shell
# code ACTION-IF-FOUND for each macro understood, or ACTION-IF-NOT-FOUND
# for each macro not understood.  Prepend '-D no-MACRO' for each MACRO 
#not understood to the output variable AM_MAKEINFOFLAGS.
AC_DEFUN([MAKEINFO_CHECK_MACROS],
[for ac_macro in $1; do
    MAKEINFO_CHECK_MACRO([$ac_macro], [$2], 
	[AM_MAKEINFOFLAGS="-D no-$ac_macro $AM_MAKEINFOFLAGS"
    $3])
done
AC_SUBST([AM_MAKEINFOFLAGS])
])# MAKEINFO_CHECK_MACROS

