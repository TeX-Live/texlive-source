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

AC_PREREQ(2.59)
AC_INIT([xindy], [2.3], [xindy-discuss@lists.sourceforge.net])

## make dist doesn't work yet, since configure makes CLISP.
AM_INIT_AUTOMAKE([foreign no-dist])

# Checks for programs.
# FIXME: not used: CXX, CPP, YACC, LN_S. Why are they tested here?
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_LEX
AC_PROG_YACC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_GCC_TRADITIONAL

dnl test for building make-rules
AC_ARG_ENABLE(make-rules, AS_HELP_STRING([--enable-make-rules ],[build and install make-rules package (default is YES)]),[BUILDRULES=$enableval],[BUILDRULES=yes])

if test "$BUILDRULES" = "yes"; then
    AC_PROG_LATEX
fi

AM_CONDITIONAL(BUILDRULES, test "$BUILDRULES" = "yes")

dnl test for building Documentation
AC_ARG_ENABLE(docs, AS_HELP_STRING([--enable-docs ],[build and install documentation (default is YES)]),[BUILDDOCS=$enableval],[BUILDDOCS=yes])

if test "$BUILDDOCS" = "yes"; then
    AC_PROG_PDFLATEX
fi

AM_CONDITIONAL(BUILDDOCS, test "$BUILDDOCS" = "yes")

dnl use external or internal CLISP
AC_ARG_ENABLE(external-clisp, AS_HELP_STRING([--enable-external-clisp],[Use an external CLISP instead of the one in rte/]),[EXT_CLISP=$enableval],[EXT_CLISP=no])

dnl test for CLISP
AC_ARG_ENABLE(clisp-path, AS_HELP_STRING([--clisp-path],[Where to search the external CLISP]),[CLISP_PATH=$enableval],[CLISP_PATH=$(which clisp)])

dnl test for CLISP linkkit
AC_ARG_ENABLE(clisp-dir, AS_HELP_STRING([--clisp-dir],[Where to seach the external CLISP library]),[CLISP_DIR=$enableval],[CLISP_DIR=/usr/lib/clisp])

if test "$EXT_CLISP" != no; then
    AM_CONDITIONAL(EXT_CLISP, test -x "$CLISP_PATH" -a -x "$CLISP_DIR/clisp-link" -a -d "$CLISP_DIR/full" -a -d "$CLISP_DIR/linkkit")
    AC_SUBST(CLISP_PATH)
    AC_SUBST(CLISP_DIR)

    if test -n "$EXT_CLISP_TRUE"; then
        if test "$EXT_CLISP" = yes; then
            echo Help me! I should use an external CLISP, but I can\'t find it. >&2
            exit 1
        else
            echo Using internal clisp
            EXT_CLISP=no
        fi
    else
        echo Using external clisp
    fi
else
    AM_CONDITIONAL(EXT_CLISP, false)
fi

# Checks for libraries.
AC_CHECK_LIB([fl], [main])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h])

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
	    	doc/style-tutorial/Makefile
])

AC_OUTPUT
