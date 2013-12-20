#
#   Copyright (C) 2004  Boretti Mathieu
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#
# AC_PROG_TEXHASH
#
# Test for texhash
# and set $texhash to the correct value.
#
#
dnl @synopsis AC_PROG_TEXHASH
dnl
dnl This macro test if texhash is installed. If texhash
dnl is installed, it set $texhash to the right value
dnl
dnl @version 1.3
dnl @author Mathieu Boretti boretti@eig.unige.ch
dnl
AC_DEFUN([AC_PROG_TEXHASH],[
AC_CHECK_PROGS(texhash,[texhash],no)
export texhash;
if test $texhash = "no" ;
then
	AC_MSG_ERROR([Unable to find the texhash application]);
fi
AC_SUBST(texhash)
])
