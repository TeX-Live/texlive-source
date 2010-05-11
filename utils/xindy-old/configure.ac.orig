##                                               -*- Autoconf -*-
## Use `autoreconf' in top-level directory to recreate all
## autoconf/automake files.
## Or, call `autoconf' to create only configure.

## Copyright (C) 2004-2005 by Gour.
## Copyright (C) 2008 by Joachim Schrod.
##
## This program is free software; you can redistribute it and/or
## modify it under the terms of the GNU General Public License as
## published by the Free Software Foundation; either version 2 of the
## License, or (at your option) any later version.
##
## This program is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
## General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program in the file LICENSE; if not, write to the
## Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
## Boston, MA 02110-1301 USA.

AC_PREREQ(2.60)
AC_INIT([xindy], [2.3], [xindy-discuss@lists.sourceforge.net])

AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE

# Checks for programs.
AC_PROG_CC
AM_PROG_LEX
dnl test "x$LEX" != "x:" || AC_MSG_ERROR([flex or lex is required, but not found])
AC_PROG_INSTALL
AC_PROG_MAKE_SET

AC_ARG_ENABLE(make-rules, AS_HELP_STRING([--enable-make-rules],[build and install make-rules package (default is YES)]))
AC_ARG_ENABLE(docs, AS_HELP_STRING([--enable-docs],[build and install documentation (default is YES)]))
AC_ARG_ENABLE(tetex-build, AS_HELP_STRING([--enable-tetex-build],[Build as part of a TeX Live installation]))
AC_ARG_WITH(clisp, AS_HELP_STRING([--with-clisp=...],[Use the specified location of the clisp executable (default is search in PATH)]))
AC_ARG_WITH(xindy-recode, AS_HELP_STRING([--with-xindy-recode],[Use `recode' instead of `iconv' (default is NO)]))

if test "x$with_xindy_recode" = xno; then
  CONVERT_FROM_UTF8="iconv -f UTF-8 -t "
else
  AC_CHECK_PROG(CONVERT_FROM_UTF8, recode, [recode UTF-8..])
  if test "x$CONVERT_FROM_UTF8" != "xrecode UTF-8.."; then
    AC_MSG_ERROR([--with-xindy-recode specified, but `recode' not found])
  fi
fi
AC_SUBST(CONVERT_FROM_UTF8)

if test "x$enable_tetex_build" = xyes -a "x$datadir" = "x$prefix"; then
  AC_MSG_NOTICE([Using installation layout for TeX Live])
  datadir="$prefix/texmf"
  docdir="$prefix/texmf/doc/$PACKAGE_NAME"
  mandir="$prefix/texmf/doc/man"
  if test -z "$enable_docs"; then
    AC_MSG_NOTICE([Disabling docs building])
    enable_docs=no;
  fi
  if test -z "$enable_make_rules"; then
    AC_MSG_NOTICE([Disabling make-rules building])
    enable_make_rules=no;
  fi
else
  enable_tetex_build=no
fi
AM_CONDITIONAL([TETEX_BUILD],[test "x$enable_tetex_build" = xyes])

if test "x$enable_make_rules" != xno; then
    AC_PROG_LATEX
fi

AM_CONDITIONAL([BUILDRULES], [test "x$enable_make_rules" != xno])

if test "x$enable_docs" != xno; then
    AC_PROG_PDFLATEX
fi

AM_CONDITIONAL([BUILDDOCS], [test "x$enable_docs" != xno])

CLISP=
if test "x$with_clisp" = xyes; then
  AC_PATH_PROG(CLISP, clisp)
elif test "x$with_clisp" = xno; then
  AC_MSG_ERROR([--without-clisp specified, but clisp is required])
elif test -n "$with_clisp"; then
  CLISP="$with_clisp"
else
  AC_PATH_PROG(CLISP, clisp)
fi

if test -n "$CLISP"; then
  test -x "$CLISP" || AC_MSG_ERROR([clisp is not executable: $CLISP])
  AC_MSG_CHECKING([for clisp library directory])
  CLISP_LIB=`$CLISP -q -norc -x '(progn (princ *lib-directory*) (values))'`
  AC_MSG_RESULT([$CLISP_LIB])
  test -f "$CLISP_LIB/clisp-link" || AC_MSG_ERROR([clisp is not usable: expected file is not found: $CLISP_LIB/clisp-link])
  test -d "$CLISP_LIB/full" || AC_MSG_ERROR([clisp is not usable: expected directory is not found: $CLISP_LIB/full])
  test -d "$CLISP_LIB/linkkit" || AC_MSG_ERROR([clisp is not usable: expected directory is not found: $CLISP_LIB/linkkit])
else
  AC_MSG_ERROR([clisp is required but not found in PATH.
Please install clisp or specify the clisp executable location
via the --with-clisp=... option.])
fi
AC_SUBST(CLISP)
AC_SUBST(CLISP_LIB)
dnl CLISP_LIB may end with a backslash (on mingw)
_AM_SUBST_NOTMAKE(CLISP_LIB)

if test "x$EXEEXT" = "x.exe"; then
  XINDY_RUN=xindy-lisp.exe
  LISP_RUN=lisp.exe
else
  XINDY_RUN=xindy.run
  LISP_RUN=lisp.run
fi
AC_SUBST(XINDY_RUN)
AC_SUBST(LISP_RUN)

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h regex.h],[],[AC_MSG_ERROR([required C header file is missing])])

dnl check system type
AC_CANONICAL_HOST
AC_SUBST(host_cpu)
AC_SUBST(host_os)

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_FUNCS([strrchr strtoul])

AC_CONFIG_FILES([Makefile \
		rte/Makefile \
		rte/ordrules/Makefile \
		rte/ordrules/version.lisp \
		binaries/Makefile \
		tex2xindy/Makefile \
		modules/Makefile \
		modules/base/Makefile \
		modules/class/Makefile \
		modules/lang/Makefile \
		modules/lang/german/Makefile \
		modules/lang/latin/Makefile \
		modules/ord/Makefile \
		modules/rules/Makefile \
		modules/styles/Makefile \
		user-commands/Makefile \
		make-rules/Makefile \
		make-rules/alphabets/Makefile \
		make-rules/alphabets/albanian/Makefile \
		make-rules/alphabets/belarusian/Makefile \
		make-rules/alphabets/bulgarian/Makefile \
		make-rules/alphabets/croatian/Makefile \
		make-rules/alphabets/czech/Makefile \
		make-rules/alphabets/danish/Makefile \
		make-rules/alphabets/dutch/Makefile \
		make-rules/alphabets/english/Makefile \
		make-rules/alphabets/esperanto/Makefile \
		make-rules/alphabets/estonian/Makefile \
		make-rules/alphabets/finnish/Makefile \
		make-rules/alphabets/french/Makefile \
		make-rules/alphabets/general/Makefile \
		make-rules/alphabets/georgian/Makefile \
		make-rules/alphabets/german/Makefile \
		make-rules/alphabets/greek/Makefile \
		make-rules/alphabets/gypsy/Makefile \
		make-rules/alphabets/hausa/Makefile \
		make-rules/alphabets/hebrew/Makefile \
		make-rules/alphabets/hungarian/Makefile \
		make-rules/alphabets/icelandic/Makefile \
		make-rules/alphabets/italian/Makefile \
		make-rules/alphabets/klingon/Makefile \
		make-rules/alphabets/kurdish/Makefile \
		make-rules/alphabets/latin/Makefile \
		make-rules/alphabets/latvian/Makefile \
		make-rules/alphabets/lithuanian/Makefile \
		make-rules/alphabets/lower-sorbian/Makefile \
		make-rules/alphabets/macedonian/Makefile \
		make-rules/alphabets/mongolian/Makefile \
		make-rules/alphabets/norwegian/Makefile \
		make-rules/alphabets/polish/Makefile \
		make-rules/alphabets/portuguese/Makefile \
		make-rules/alphabets/romanian/Makefile \
		make-rules/alphabets/russian/Makefile \
		make-rules/alphabets/serbian/Makefile \
		make-rules/alphabets/slovak/Makefile \
		make-rules/alphabets/slovenian/Makefile \
		make-rules/alphabets/spanish/Makefile \
		make-rules/alphabets/swedish/Makefile \
		make-rules/alphabets/test1/Makefile \
		make-rules/alphabets/turkish/Makefile \
		make-rules/alphabets/ukrainian/Makefile \
		make-rules/alphabets/upper-sorbian/Makefile \
		make-rules/alphabets/vietnamese/Makefile \
		make-rules/inputenc/Makefile \
		make-rules/styles/Makefile \
		doc/Makefile \
	    	doc/style-tutorial/Makefile \
		src/Makefile
])

AC_OUTPUT
