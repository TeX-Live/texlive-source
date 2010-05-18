dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009, 2010 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
dnl *********************************************************************
dnl
dnl  Adapted for TeX Live from xindy-2.4-rc1/configure.ac
dnl  Copyright (C) 2004-2005 by Gour.
dnl  Copyright (C) 2008,2009 by Joachim Schrod.
dnl
dnl *********************************************************************
dnl
AC_PREREQ([2.65])
dnl Get version string from file; need to discard training newline
dnl Note: patsubst() or translit() don't work here, it's not possible
dnl to specify substitution of \n, so we use substr. This won't work
dnl if file VERSION is missing its trailing newline...
m4_define([VERSION_NL], m4_include([xindy/VERSION]))
m4_define([xindy_version], m4_substr(VERSION_NL, 0, m4_eval(m4_len(VERSION_NL)-1)))
AC_INIT([xindy for TeX Live], xindy_version, [tex-k@tug.org])
AC_CONFIG_SRCDIR([xindy/ChangeLog.Gour])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

AM_INIT_AUTOMAKE([foreign no-define])
AM_MAINTAINER_MODE

# Configure options for xindy also shown at the TeX Live top-level.
m4_include([ac/xindy.ac])

AS_CASE([$enable_xindy_rules],
        [yes | no], ,
        [AS_IF([test "x$enable_native_texlive_build" = xyes],
               [enable_xindy_rules=no],
               [enable_xindy_rules=yes])])
AS_CASE([$enable_xindy_docs],
        [yes | no], ,
        [AS_IF([test "x$enable_native_texlive_build" = xyes],
               [enable_xindy_docs=no],
               [enable_xindy_docs=yes])])

# Installation directories for TeX Live.
memdir='$(bindir)'
pkglibdir='${prefix}/texmf/xindy'
docdir='${prefix}/texmf/doc/xindy'
AC_SUBST([memdir])
AC_SUBST([pkglibdir])
AM_CONDITIONAL([TEXLIVE_BUILD], [test "x$enable_texlive_build" = xyes])

# Checks for programs.
AC_PROG_CC
AC_PROG_LEX
AC_PROG_LN_S

# Check system type
AC_CANONICAL_HOST

KPSE_COMPILER_WARNINGS

AC_CHECK_PROGS([CLISP], [clisp], [no])
AC_CHECK_PROGS([PERL], [perl], [no])
AC_CHECK_PROGS([LATEX], [latex elatex lambda], [no])
AC_CHECK_PROGS([PDFLATEX], [pdflatex], [no])

if test "x$enable_build" != xno || test -f config.force; then

if test "x$CLISP" = xno; then
  AC_MSG_ERROR([CLISP is required to build and run xindy])
fi

if test "x$EXEEXT" = x.exe; then
  XINDY_RUN=xindy-lisp.exe
  LISP_RUN=lisp.exe
else
  XINDY_RUN=xindy.run
  LISP_RUN=lisp.run
fi

if test "x$with_clisp_runtime" != xsystem; then
  AC_MSG_CHECKING([for clisp runtime])
  if test "x$with_clisp_runtime" = xdefault; then
    LISP_RUN=`echo '(format t "~a" *lib-directory*)' | $CLISP -q -norc -`base/$LISP_RUN
  else
    LISP_RUN=$with_clisp_runtime
  fi
  AC_MSG_RESULT([$LISP_RUN])
  if test ! -f "$LISP_RUN"; then
    AC_MSG_ERROR([no such file: "$LISP_RUN"])
  fi
fi

AC_SUBST([XINDY_RUN])
AC_SUBST([LISP_RUN])

if test "x$enable_xindy_rules:$PERL" = xyes:no; then
  AC_MSG_ERROR([PERL is required to build xindy rules])
fi

if test "x$enable_xindy_rules:$LATEX" = xyes:no; then
  AC_MSG_ERROR([LATEX is required to build xindy rules])
fi

if test "x$enable_xindy_rules:$enable_xindy_docs:$PDFLATEX" = xyes:yes:no; then
  AC_MSG_ERROR([PDFLATEX is required to build xindy docs])
fi

if test "x$with_recode" = xyes; then
  AC_CHECK_PROG([CONVERT_FROM_UTF8], [recode], [recode UTF-8..])
  if test "x$CONVERT_FROM_UTF8" != 'xrecode UTF-8..'; then
    AC_MSG_ERROR([--with-xindy-recode specified, but `recode' not found])
  fi
else
  CONVERT_FROM_UTF8='iconv -f UTF-8 -t #' # May need the trailing blank
fi
AC_SUBST([CONVERT_FROM_UTF8])

# No checks for header files. We use stdio.h, unistd.h, and string.h.
# Today, they are universally available if a C compiler is installed.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
# The code in tex2indy.l simply assumes that strtoul() is there, would
# need a replacement.
AC_CHECK_FUNCS([strtoul], [],
               [AC_MSG_ERROR([strtoul() is needed])])

echo timestamp >config.force
fi

AM_CONDITIONAL([CLISP_RUNTIME], [test "x$with_clisp_runtime" != xsystem])
AM_CONDITIONAL([BUILDRULES], [test "x$enable_xindy_rules" = xyes])
AM_CONDITIONAL([BUILDDOCS], [test "x$enable_xindy_docs" = xyes])

AC_CONFIG_FILES([Makefile \
		xindy/Makefile \
		xindy/src/Makefile \
		xindy/tex2xindy/Makefile \
		xindy/modules/Makefile \
		xindy/modules/base/Makefile \
		xindy/modules/class/Makefile \
		xindy/modules/lang/Makefile \
		xindy/modules/lang/german/Makefile \
		xindy/modules/lang/latin/Makefile \
		xindy/modules/ord/Makefile \
		xindy/modules/rules/Makefile \
		xindy/modules/styles/Makefile \
		xindy/user-commands/Makefile \
		xindy/make-rules/Makefile \
		xindy/make-rules/alphabets/Makefile \
		xindy/make-rules/alphabets/albanian/Makefile \
		xindy/make-rules/alphabets/belarusian/Makefile \
		xindy/make-rules/alphabets/bulgarian/Makefile \
		xindy/make-rules/alphabets/croatian/Makefile \
		xindy/make-rules/alphabets/czech/Makefile \
		xindy/make-rules/alphabets/danish/Makefile \
		xindy/make-rules/alphabets/dutch/Makefile \
		xindy/make-rules/alphabets/english/Makefile \
		xindy/make-rules/alphabets/esperanto/Makefile \
		xindy/make-rules/alphabets/estonian/Makefile \
		xindy/make-rules/alphabets/finnish/Makefile \
		xindy/make-rules/alphabets/french/Makefile \
		xindy/make-rules/alphabets/general/Makefile \
		xindy/make-rules/alphabets/georgian/Makefile \
		xindy/make-rules/alphabets/german/Makefile \
		xindy/make-rules/alphabets/greek/Makefile \
		xindy/make-rules/alphabets/gypsy/Makefile \
		xindy/make-rules/alphabets/hausa/Makefile \
		xindy/make-rules/alphabets/hebrew/Makefile \
		xindy/make-rules/alphabets/hungarian/Makefile \
		xindy/make-rules/alphabets/icelandic/Makefile \
		xindy/make-rules/alphabets/italian/Makefile \
		xindy/make-rules/alphabets/klingon/Makefile \
		xindy/make-rules/alphabets/kurdish/Makefile \
		xindy/make-rules/alphabets/latin/Makefile \
		xindy/make-rules/alphabets/latvian/Makefile \
		xindy/make-rules/alphabets/lithuanian/Makefile \
		xindy/make-rules/alphabets/lower-sorbian/Makefile \
		xindy/make-rules/alphabets/macedonian/Makefile \
		xindy/make-rules/alphabets/mongolian/Makefile \
		xindy/make-rules/alphabets/norwegian/Makefile \
		xindy/make-rules/alphabets/polish/Makefile \
		xindy/make-rules/alphabets/portuguese/Makefile \
		xindy/make-rules/alphabets/romanian/Makefile \
		xindy/make-rules/alphabets/russian/Makefile \
		xindy/make-rules/alphabets/serbian/Makefile \
		xindy/make-rules/alphabets/slovak/Makefile \
		xindy/make-rules/alphabets/slovenian/Makefile \
		xindy/make-rules/alphabets/spanish/Makefile \
		xindy/make-rules/alphabets/swedish/Makefile \
		xindy/make-rules/alphabets/test1/Makefile \
		xindy/make-rules/alphabets/turkish/Makefile \
		xindy/make-rules/alphabets/ukrainian/Makefile \
		xindy/make-rules/alphabets/upper-sorbian/Makefile \
		xindy/make-rules/alphabets/vietnamese/Makefile \
		xindy/make-rules/inputenc/Makefile \
		xindy/make-rules/styles/Makefile \
		xindy/doc/Makefile \
	    	xindy/doc/style-tutorial/Makefile \
])

AC_OUTPUT
