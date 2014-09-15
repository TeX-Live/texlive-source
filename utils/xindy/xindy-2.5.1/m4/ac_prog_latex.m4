dnl @synopsis AC_PROG_LATEX
dnl
dnl This macro test if latex is installed. If latex is installed,
dnl set $LATEX to the right value
dnl
dnl @category LaTeX
dnl @author Mathieu Boretti <boretti@bss-network.com>
dnl @version 2005-01-21
dnl @license GPLWithACException
dnl
dnl Modified 2014-05-30 by Peter Breitenlohner <tex-live@tug.org>

AC_DEFUN([AC_PROG_LATEX], [dnl
AC_ARG_VAR([LATEX], [LaTeX command])
AC_CHECK_PROGS([LATEX], [latex elatex lambda], [no])
if test "x$LATEX" = xno
then
	AC_MSG_ERROR([Unable to find a LaTeX application])
fi
export LATEX
AC_SUBST([LATEX])
])
