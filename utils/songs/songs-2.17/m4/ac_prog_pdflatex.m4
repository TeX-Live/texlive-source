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
# AC_PROG_PDFLATEX
#
# Test for pdflatex
# and set $pdflatex to the correct value.
#
#
dnl @synopsis AC_PROG_PDFLATEX
dnl
dnl This macro test if pdflatex is installed. If pdflatex
dnl is installed, it set $pdflatex to the right value
dnl
dnl @version 1.3
dnl @author Mathieu Boretti boretti@eig.unige.ch
dnl
AC_DEFUN([AC_PROG_PDFLATEX],[
AC_CHECK_PROGS(pdflatex,[pdflatex],no)
export pdflatex;
if test $pdflatex = "no" ;
then
	AC_MSG_ERROR([Unable to find a PDFLaTeX application]);
fi
AC_SUBST(pdflatex)
])
