# acinclude.m4

#************************************************************************
#
#  Part of the dvipng distribution
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as
#  published by the Free Software Foundation, either version 3 of the
#  License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful, but
#  WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this program. If not, see
#  <http://www.gnu.org/licenses/>.
#
#  Copyright (C) 2002-2015,2019 Jan-Åke Larsson
#
#************************************************************************


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
dnl Check for enc, cmap, sfd formats
dnl
AC_DEFUN([AC_HAS_KPSE_ENC_FORMATS],
 [AC_MSG_CHECKING([for kpse_enc_format])
  AC_TRY_COMPILE([
    #include <stdio.h>
    #include <kpathsea/kpathsea.h>],
    [kpse_enc_format;kpse_cmap_format;kpse_sfd_format],
 [AC_MSG_RESULT(yes)
  AC_DEFINE(HAVE_KPSE_ENC_FORMATS, 1, 
	[Define to 1 if your kpathsea has kpse_enc_format])],
 [AC_MSG_RESULT(no)])])


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
