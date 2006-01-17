dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.53)
AC_INIT([texi2html], [1.76], [users@texi2html.cvshome.org])
AM_INIT_AUTOMAKE([gnu 1.7 dist-bzip2])

dnl Disable Autoconf, Automake, and some other maintainer tools without the
dnl --enable-maintainer-mode argument.
AM_MAINTAINER_MODE

dnl Misc variable settings
PACKAGE_DATE=`"$srcdir"/mdate-sh "$srcdir"/configure.ac`
AC_SUBST([PACKAGE_DATE])
INIT=$srcdir/texi2html.init
AC_SUBST_FILE([INIT])
MYSIMPLE=$srcdir/MySimple.pm
AC_SUBST_FILE([MYSIMPLE])
T2H_I18N=$srcdir/T2h_i18n.pm
AC_SUBST_FILE([T2H_I18N])
T2H_L2H_FILE=$srcdir/l2h.init
AC_SUBST_FILE([T2H_L2H_FILE])
T2H_TRANSLATIONS_FILE=$srcdir/translations.pl
AC_SUBST_FILE([T2H_TRANSLATIONS_FILE])
AC_ARG_WITH([encode],
           AC_HELP_STRING([--with-encode],
                           [use encode (default: detected)]),
            [if test $withval = 'no'; then
              USE_UNICODE=0
             else
              USE_UNICODE=1
             fi],
            [
            if perl -e "use Encode; use Unicode::Normalize;" > /dev/null 2>&1; 
              then USE_UNICODE=1
            else
              USE_UNICODE=0
            fi
            ])
AC_SUBST([USE_UNICODE])

dnl Checks for programs.
AC_ARG_VAR([PERL], [The path to the `perl' executable.])
AC_PATH_PROG([PERL], [perl], [/usr/bin/env perl], [/opt/perl5/bin:"$PATH"])

AC_PROG_INSTALL

dnl Checks for libraries.

dnl Checks for header files.

dnl Checks for typedefs, structures, and compiler characteristics.

dnl Checks for library functions.
# For teTeX and TeX Live.

AC_CANONICAL_HOST
AC_ARG_ENABLE(multiplatform,
  [  --enable-multiplatform  put executables in bin/PLATFORM])
test "x$enable_multiplatform" = xyes \
&& test "x$bindir" = 'x${exec_prefix}/bin' \
&& bindir="$bindir/$host"

AC_CONFIG_FILES([texi2html:texi2html.pl], [chmod +x texi2html])
AC_CONFIG_FILES([check_texinfo.pl], [chmod +x check_texinfo.pl])

dnl The bulk config files.
AC_CONFIG_FILES([\
                 Makefile \
                 Tests/Makefile \
                 doc/Makefile \
                 texi2html.1 \
                 texi2html.spec \
                 ])
AC_OUTPUT
