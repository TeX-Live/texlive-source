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
# AC_PROG_LATEX
#
# Test for latex or elatax or lambda
# and set $latex to the correct value.
#
#
dnl @synopsis AC_PROG_LATEX
dnl
dnl This macro test if latex is installed. If latex
dnl is installed, it set $latex to the right value
dnl
dnl @version 1.3
dnl @author Mathieu Boretti boretti@eig.unige.ch
dnl
AC_DEFUN([AC_PROG_LATEX],[
AC_CHECK_PROGS(latex,[latex elatex lambda],no)
export latex;
if test $latex = "no" ;
then
	AC_MSG_ERROR([Unable to find a LaTeX application]);
fi
AC_SUBST(latex)
])
