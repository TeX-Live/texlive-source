dnl @synopsis AC_PROG_PDFLATEX
dnl
dnl This macro test if pdflatex is installed. If pdflatex is installed,
dnl set $PDFLATEX to the right value
dnl
dnl @category LaTeX
dnl @author Mathieu Boretti <boretti@bss-network.com>
dnl @version 2005-01-21
dnl @license GPLWithACException
dnl
dnl Modified 2014-05-30 by Peter Breitenlohner <tex-live@tug.org>

AC_DEFUN([AC_PROG_PDFLATEX], [dnl
AC_ARG_VAR([PDFLATEX], [pdfLaTeX command])
AC_CHECK_PROGS([PDFLATEX], [pdflatex], [no])
if test "x$PDFLATEX" = xno
then
	AC_MSG_ERROR([Unable to find a PDFLaTeX application])
fi
export PDFLATEX
AC_SUBST([PDFLATEX])
])
