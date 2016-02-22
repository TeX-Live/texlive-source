##                                               -*- Autoconf -*-
## Use `autoreconf' in top-level directory to recreate all
## autoconf/automake files.
## Or, call `autoconf' to create only configure.

## Copyright (C) 2004-2005 by Gour.
## Copyright (C) 2008-2014 by Joachim Schrod.
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
## along with this program in the file COPYING; if not, write to the
## Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
## Boston, MA 02110-1301 USA.

AC_PREREQ(2.68)
dnl Get version string from file; need to discard training newline.
m4_define([XINDY_VERSION], m4_chomp(m4_include([VERSION])))
AC_INIT([xindy], XINDY_VERSION, [xindy-discuss@lists.sourceforge.net])

# Automake's dist target needs all to-be-distributed files flagged in
# Makefile.am files; we have not done that yet. Since we don't do C
# programs, automake's defaults are not usable for us, too.
#
# FIXME: Now make dist works, thanks to Peter's patches. Shouldn't I
# need to change here now?
AM_INIT_AUTOMAKE([foreign no-define])

# Installation directory for standalone xindy, different when part of TeX Live.
memdir='$(pkglibdir)'
AC_SUBST([memdir])
AM_CONDITIONAL([TEXLIVE_BUILD], [false])

# Test for building make-rules
AC_ARG_ENABLE([make-rules],
              AS_HELP_STRING([--enable-make-rules],
                             [build and install make-rules package (default is YES)]),
              [BUILDRULES=$enableval],
              [BUILDRULES=yes])
AS_IF([test "$BUILDRULES" = "yes"], [AC_PROG_LATEX])
AM_CONDITIONAL([BUILDRULES], [test "x$BUILDRULES" = xyes])

# Test for building Documentation
AC_ARG_ENABLE([docs],
              AS_HELP_STRING([--enable-docs],
                             [build and install documentation (default is NO)]),
              [BUILDDOCS=$enableval],
              [BUILDDOCS=no])
AS_IF([test "$BUILDDOCS" = "yes"], [AC_PROG_PDFLATEX])
AM_CONDITIONAL([BUILDDOCS], [test "x$BUILDDOCS" = xyes])

# Checks for programs:
# make and install are needed in all Makefiles.
AC_PROG_MAKE_SET
AC_PROG_INSTALL
# CLISP for src/
AC_ARG_VAR([CLISP], [Common Lisp interpreter and compiler command])
AC_PATH_PROGS([CLISP], [clisp], [no])
AC_ARG_VAR([PERL], [Perl interpreter command])
AC_PATH_PROGS([PERL], [perl perl5], [no])
# flex for tex2xindy
# FIXME: lex actually must really be flex. But there is no AC_PROG_FLEX!?
AC_PROG_LEX
AC_PROG_CC
AC_PROG_LN_S

# Check system type. This is needed to detect mingw32 environment,
# then CLISP .mem files must not be compressed.
AC_CANONICAL_HOST

# Check that CLISP was found.
if test "x$CLISP" = xno; then
	AC_MSG_ERROR([CLISP is needed to build and run xindy])
fi

# Check that PERL was found.
if test "x$PERL" = xno; then
	AC_MSG_ERROR([PERL is needed to build and run xindy])
fi

# No checks for header files. We use stdio.h, unistd.h, and string.h.
# Today, they are universally available if a C compiler is installed.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
# The code in tex2indy.l simply assumes that strtoul() is there, would
# need a replacement.
AC_CHECK_FUNCS([strtoul],[],AC_MSG_ERROR([strtoul() is needed]))

AC_CONFIG_FILES([Makefile \
		src/Makefile \
		tex2xindy/Makefile \
		modules/Makefile \
		user-commands/Makefile \
		make-rules/Makefile \
		make-rules/alphabets/Makefile \
		make-rules/inputenc/Makefile \
		doc/Makefile \
])

AC_OUTPUT
