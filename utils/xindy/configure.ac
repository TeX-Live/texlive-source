dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009-2014 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
dnl *********************************************************************
dnl
dnl  Adapted for TeX Live from xindy-src/configure.ac
dnl  Copyright (C) 2004-2005 by Gour.
dnl  Copyright (C) 2008,2009 by Joachim Schrod.
dnl
dnl *********************************************************************
dnl
m4_include([version.ac])[] dnl define xindy_version
AC_INIT([xindy (TeX Live)], xindy_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([xindy-src/ChangeLog.Gour])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

KPSE_BASIC([xindy], [no-define])

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
pkglibdir='$(datarootdir)/texmf-dist/xindy'
docdir='$(datarootdir)/texmf-dist/doc/xindy'
AC_SUBST([memdir])
AC_SUBST([pkglibdir])
AM_CONDITIONAL([TEXLIVE_BUILD], [test "x$enable_texlive_build" = xyes])

# Checks for programs.
AC_PROG_CC
AC_PROG_LEX
AC_PROG_LN_S

# Check system type
AC_CANONICAL_HOST

AC_CHECK_PROGS([CLISP], [clisp], [no])
KPSE_CHECK_PERL
KPSE_CHECK_LATEX
KPSE_CHECK_PDFLATEX

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
    # Convert dos line endings from cygwin clisp.
    LISP_RUN=`echo '(format t "~a" *lib-directory*)' \
              | $CLISP -q -norc - | tr -d '\r'`base/$LISP_RUN
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

KPSE_COND_WIN32_WRAP

AC_SUBST([XINDY_TREE], [xindy-src])

AM_CONDITIONAL([CLISP_RUNTIME], [test "x$with_clisp_runtime" != xsystem])
AM_CONDITIONAL([BUILDRULES], [test "x$enable_xindy_rules" = xyes])
AM_CONDITIONAL([BUILDDOCS], [test "x$enable_xindy_docs" = xyes])

AC_CONFIG_FILES([Makefile \
		xindy-src/Makefile \
		xindy-src/src/Makefile \
		xindy-src/tex2xindy/Makefile \
		xindy-src/modules/Makefile \
		xindy-src/user-commands/Makefile \
		xindy-src/make-rules/Makefile \
		xindy-src/make-rules/alphabets/Makefile \
		xindy-src/make-rules/inputenc/Makefile \
		xindy-src/doc/Makefile \
])

AC_OUTPUT
