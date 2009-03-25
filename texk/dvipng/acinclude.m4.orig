# acinclude.m4 extracted from aclocal.m4 of dvipng release 1.10.

# Copyright 1996, 1997, 1998, 1999, 2000, 2001, 2002
# Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.

dnl
dnl MAKEINFO_CHECK_MACRO( MACRO, [ACTION-IF-FOUND 
dnl					[, ACTION-IF-NOT-FOUND]])
dnl
AC_DEFUN([MAKEINFO_CHECK_MACRO],
[if test -n "$MAKEINFO" -a "$makeinfo" != ":"; then
  AC_MSG_CHECKING([for @$1{}])
  echo \\\\input texinfo > conftest.texi
  echo @$1{test} >> conftest.texi
  if $MAKEINFO conftest.texi > /dev/null 2> /dev/null; then
    AC_MSG_RESULT(yes)	
    ifelse([$2], , :, [$2])
  else  
    AC_MSG_RESULT(no)	
    ifelse([$3], , :, [$3])
  fi
  rm -f conftest.texi conftest.info
fi
])

dnl
dnl MAKEINFO_CHECK_MACROS( MACRO ... [, ACTION-IF-FOUND 
dnl					[, ACTION-IF-NOT-FOUND]])
dnl
AC_DEFUN([MAKEINFO_CHECK_MACROS],
[for ac_macro in $1; do
    MAKEINFO_CHECK_MACRO($ac_macro, $2, 
	[MAKEINFO_MACROS="-D no-$ac_macro $MAKEINFO_MACROS"
	$3])dnl
  done
AC_SUBST(MAKEINFO_MACROS)
])


dnl 
dnl Check devices for GS
dnl AC_GS_HAS_DEVICE(DEVICE,ACTION-IF-FAILED)
dnl
AC_DEFUN([AC_GS_HAS_DEVICE],
 [AC_MSG_CHECKING([whether $GS has the $1 device])
  if $GS -h | grep $1 >/dev/null; then
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
    $2
  fi
])

dnl
dnl GS_CHECK_DEVICES
dnl
AC_DEFUN([GS_CHECK_DEVICES],
 [GS_WARN=""
  AC_GS_HAS_DEVICE(pngalpha,
    [GS_WARN="Your EPS inclusions will be cropped to the 
              boundingbox, and rendered on an opaque background. 
              Upgrade GhostScript to avoid this."
     AC_GS_HAS_DEVICE(png16m,
       [GS_WARN="Your EPS inclusions may not work.
                 Upgrade/install GhostScript to avoid this."])])
  if test -n "$GS_WARN"; then
    AC_MSG_WARN([$GS_WARN])
  fi
])
