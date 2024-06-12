dnl
dnl ChkTeX, configuration file.
dnl Copyright (C) 1995-96 Jens T. Berger Thielemann
dnl
dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or
dnl (at your option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program; if not, write to the Free Software
dnl Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
dnl
dnl Contact the author at:
dnl             Jens Berger
dnl             Spektrumvn. 4
dnl             N-0666 Oslo
dnl             Norway
dnl             E-mail: <jensthi@ifi.uio.no>
dnl
dnl


dnl Process this file with autoconf to produce a configure script.
AC_INIT([ChkTeX],[1.7.9])
AC_SUBST(SCRIPTS)
AC_SUBST(BUILT_SCRIPTS)
AC_SUBST(CLEAN)
CLEAN=

AC_ARG_ENABLE(pcre,[
    --enable-pcre:             Use PCRE to allow regular expressions in
                               user warnings.  (Default:  yes, if available).],,
enable_pcre=yes)

AC_ARG_ENABLE(posixre,[
    --enable-posixre:          Use POSIX extended regular expressions in
                               user warnings.  (Default:  yes, unless using PCRE).],,
enable_posixre=yes)

AC_ARG_ENABLE(lacheck-replace,[
    --enable-lacheck-replace:  Installs  a lacheck replacement.  (Default:  no).],,
enable_lacheck_replace=no)

AC_ARG_ENABLE(debug-info,[
    --enable-debug-info:       Compile in miscellaneous runtime debugging
			       information.  (Default: yes).],,
enable_debug_info=yes)

AC_ARG_ENABLE(coverage-testing,[
    --enable-coverage-testing: Activate coverage testing with gcov. (Default: no)],,
enable_coverage_testing=no)

AC_MSG_CHECKING(whether lacheck replacement should be installed)
if test "$enable_lacheck_replace" = "yes"; then
  SCRIPTS="$SCRIPTS lacheck"
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
fi

AC_MSG_CHECKING(whether runtime debug info should be used)
if test "$enable_debug_info" = "yes"; then
  AC_MSG_RESULT(yes)
else
  AC_DEFINE(STRIP_DEBUG, 1, Strip debug info)
  AC_MSG_RESULT(no)
fi

AC_MSG_CHECKING(whether coverage testing is enabled)
if test "$enable_coverage_testing" = "yes"; then
  AC_MSG_RESULT(yes)
  CFLAGS="${CFLAGS} -fprofile-arcs -ftest-coverage -O0"
  CLEAN="${CLEAN} \$(wildcard *.bb *.bbg *.da *.gcov)"
else
  AC_MSG_RESULT(no)
fi


dnl Check compiler and flags
AC_PROG_CC
AX_CFLAGS_STRICT_PROTOTYPES
VL_PROG_CC_WARNINGS

dnl Checks for programs.
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MKDIR_P
AC_PATH_PROGS(PERL5, perl5 perl, no, $PATH:/usr/bin:/usr/local/bin)
if test "$PERL5" != no; then
  AC_MSG_CHECKING(whether perl is v5 or higher)
  if $PERL5 -e 'exit($] >= 5)'; then
    PERL5=no
    AC_MSG_RESULT(no)
  else
    AC_MSG_RESULT(yes)
  fi
fi

if test "$PERL5" != no; then
    BUILT_SCRIPTS="$BUILT_SCRIPTS deweb"
    SCRIPTS="$SCRIPTS chkweb"
else
    AC_MSG_WARN(chkweb will not be installed since you do not have perl5.)
fi

AC_CHECK_PROGS(GROFF, 'groff -man -P-bu -Tlatin1' 'nroff -man', 'nroff -man')

AC_PATH_PROG(PDFLATEX, pdflatex, no)
AC_PATH_PROG(LATEX, latex, no)
dnl if test "$LATEX" = no; then
dnl	AC_MSG_ERROR([No LaTeX on the system, cannot find latex program.])
dnl fi

AC_PATH_PROG(DVIPS, dvips)

AC_PATH_PROG(LYNX, lynx)
AC_PATH_PROG(LATEX2HTML, latex2html)

dnl Checks for libraries.
AC_CHECK_LIB(termlib, tgetent)
AC_CHECK_LIB(termcap, tgetent)

AC_HEADER_DIRENT
AC_CHECK_HEADERS(limits.h stat.h strings.h stdarg.h dnl
sys/stat.h termcap.h termlib.h unistd.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT64_T

dnl Checks for library functions.
AC_FUNC_VPRINTF
AC_CHECK_FUNCS(access closedir fileno isatty opendir readdir stat dnl
strcasecmp strdup strlwr strtol)

dnl Checks for declarations.
AC_CHECK_DECLS([stpcpy])

dnl Check for PCRE regular expression support
AC_MSG_CHECKING(whether PCRE regular expressions are enabled)
AC_MSG_RESULT($enable_pcre)
if test "$enable_pcre" = "yes"; then
  AC_CHECK_PROG(WITH_PCRE, pcre-config, yes, no)
  if test "$WITH_PCRE" = "yes"; then
    AC_DEFINE(HAVE_PCRE, 1, Whether PCRE can be used for user warnings.)
    CFLAGS="${CFLAGS} $(pcre-config --cflags-posix)"
    LDFLAGS="${LDFLAGS} $(pcre-config --libs-posix)"
  else
    AC_DEFINE(HAVE_PCRE, 0)
    AC_MSG_WARN(PCRE library not found.)
  fi
else
    AC_DEFINE(HAVE_PCRE, 0)
    AC_MSG_WARN(PCRE regular expressions not enabled.)
fi

dnl Check for POSIX extended regular expression support
AC_MSG_CHECKING(for POSIX extended regular expressions)
AC_EGREP_CPP([yes],
  [#include <regex.h>
  #ifdef REG_EXTENDED
  yes
  # endif
  ], posix_ere=yes, posix_ere=no)
AC_MSG_RESULT($posix_ere)
if test "$WITH_PCRE" = "yes"; then
    AC_DEFINE(HAVE_POSIX_ERE, 0, Whether POSIX ERE can be used for user warnings.)
else
  if test "$enable_posixre" = "yes"; then
    if test "$posix_ere" = "yes"; then
      AC_DEFINE(HAVE_POSIX_ERE, 1)
    else
      AC_DEFINE(HAVE_POSIX_ERE, 0)
    fi
  else
    AC_DEFINE(HAVE_POSIX_ERE, 0)
    AC_MSG_WARN(Regular expressions not enabled.)
  fi
fi


AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([stamp-h], [echo timestamp > stamp-h])
AC_CONFIG_FILES([Makefile deweb rmeheader ChkTeX.tex])
AC_OUTPUT
