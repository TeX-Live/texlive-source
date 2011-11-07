##                                               -*- Autoconf -*-
## Use `autoreconf' in top-level directory to recreate all
## autoconf/automake files.
## Or, call `autoconf' to create only configure.

## Copyright (C) 2004-2005 by Gour.
## Copyright (C) 2008,2009 by Joachim Schrod.
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

AC_PREREQ(2.59)
dnl Get version string from file; need to discard training newline
dnl Note: patsubst() or translit() don't work here, it's not possible
dnl to specify substitution of \n, so we use substr. This won't work
dnl if file VERSION is missing its trailing newline...
m4_define([VERSION_NL],m4_include([VERSION]))
m4_define([XINDY_VERSION],m4_substr(VERSION_NL,0,m4_eval(m4_len(VERSION_NL)-1)))
AC_INIT([xindy], XINDY_VERSION, [xindy-discuss@lists.sourceforge.net])

# Automake's dist target needs all to-be-distributed files flagged in
# Makefile.am files; we have not done that yet. Since we don't do C
# programs, automake's defaults are not usable for us, too.
AM_INIT_AUTOMAKE([foreign no-define])

# Installation directories for standalone xindy.
memdir='$(pkglibdir)'
AC_SUBST([memdir])
AM_CONDITIONAL([TEXLIVE_BUILD], [false])

# Test for building make-rules
AC_ARG_ENABLE(make-rules, AS_HELP_STRING([--enable-make-rules ],[build and install make-rules package (default is YES)]),[BUILDRULES=$enableval],[BUILDRULES=yes])
test "$BUILDRULES" = "yes"  &&  AC_PROG_LATEX
AM_CONDITIONAL(BUILDRULES, test "$BUILDRULES" = "yes")

# Test for building Documentation
AC_ARG_ENABLE(docs, AS_HELP_STRING([--enable-docs ],[build and install documentation (default is YES)]),[BUILDDOCS=$enableval],[BUILDDOCS=yes])
test "$BUILDDOCS" = "yes"  &&  AC_PROG_PDFLATEX
AM_CONDITIONAL(BUILDDOCS, test "$BUILDDOCS" = "yes")

# Checks for programs:
# make and install are needed in all Makefiles.
AC_PROG_MAKE_SET
AC_PROG_INSTALL
# CLISP for src/
AC_PATH_PROG([CLISP],[clisp])
AC_PATH_PROG([PERL], [perl])
# flex for tex2xindy
# FIXME: lex actually must really be flex. But there is no AC_PROG_FLEX!?
AC_PROG_LEX
AC_PROG_CC
AC_PROG_LN_S

# Check system type. This is needed to detect mingw32 environment,
# then CLISP .mem files must not be compressed.
AC_CANONICAL_HOST

# On some systems without working iconv, we must use recode.
AC_ARG_WITH([xindy-recode],
            AS_HELP_STRING([--with-xindy-recode],
                           [Use `recode' instead of `iconv' @<:@default is NO@:>@]))
if test "x$with_xindy_recode" != xyes; then
  CONVERT_FROM_UTF8='iconv -f UTF-8 -t #' # May need the trailing blank
else
  AC_CHECK_PROG(CONVERT_FROM_UTF8, recode, [recode UTF-8..])
  if test "x$CONVERT_FROM_UTF8" != 'xrecode UTF-8..'; then
    AC_MSG_ERROR([--with-xindy-recode specified, but `recode' not found])
  fi
fi
AC_SUBST([CONVERT_FROM_UTF8])

# Check that CLISP was found.
# FIXME: Should we also check for latex in the buildrules case?
# What about a C compiler? install?
test "$CLISP"  ||  AC_MSG_ERROR([CLISP is needed to build and run xindy])

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
		make-rules/alphabets/persian/Makefile \
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
	    	doc/style-tutorial/Makefile
])

AC_OUTPUT
