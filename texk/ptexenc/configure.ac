dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl --------------------------------------------------------
dnl Keep ptexenc shared and package library versions the same:
dnl   Only bug fixes:
dnl     ptexenc_lib_micro += 1;
dnl   Adding new interfaces (backwards compatible)
dnl     ptexenc_lib_minor += 1;
dnl     ptexenc_lib_micro = 0;
dnl   Modifying or revoking interfaces (not backwards compatible)
dnl     ptexenc_lib_major += 1;
dnl     ptexenc_lib_minor = 0;
dnl     ptexenc_lib_micro = 0;
dnl
dnl   After a release, set ptexenc_lib_dev to "dev", to distinguish
dnl   binaries built between releases.

m4_define([ptexenc_lib_major], [2])[]dnl
m4_define([ptexenc_lib_minor], [0])[]dnl
m4_define([ptexenc_lib_micro], [2])[]dnl
m4_define([ptexenc_lib_devel], [])[]dnl or "dev"
m4_define([ptexenc_version], [ptexenc_lib_major.ptexenc_lib_minor.ptexenc_lib_micro[]ptexenc_lib_devel])[]dnl

AC_INIT([ptexenc], [1.1.0])
AC_PREREQ([2.63])
AC_CONFIG_SRCDIR([ptexenc.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

AC_SUBST([PTEXENCVERSION], [ptexenc_version])
AC_DEFINE([PTEXENCVERSION], ["ptexenc version ptexenc_version"])
AC_SUBST([PTEXENC_LT_VERSNUM], [ptexenc_lib_major:ptexenc_lib_minor:ptexenc_lib_micro])

dnl Common code for all programs using libptexenc.
KPSE_COMMON([ptexenc])

dnl Replacement functions that may be required on ancient broken system.
#AC_CHECK_DECLS([putenv, strstr])
#AC_REPLACE_FUNCS([putenv strcasecmp strtol strstr])

KPSE_KPATHSEA_FLAGS

dnl withenable.ac: --with and --enable options.
AC_MSG_CHECKING(default pTeX character encode)
AC_ARG_WITH(ptexenc-enc,
[  --with-ptexenc-enc=ENC       Default pTeX character encode
                           (ENC=JIS/EUC/SJIS/UTF8, default UTF8)],
[case $withval in
      jis|JIS) PTEXENC=ENC_JIS;;
      euc|EUC) PTEXENC=ENC_EUC;;
    sjis|SJIS) PTEXENC=ENC_SJIS;;
    utf8|UTF8) PTEXENC=ENC_UTF8;;
            *) PTEXENC=ENC_UTF8;
	       AC_MSG_WARN(Unknown --with-ptexenc-enc argument \`$withval';);;
 esac; AC_MSG_RESULT($PTEXENC)
 AC_DEFINE_UNQUOTED([DEFAULT_KANJI_ENC],[$PTEXENC],[Default kanji encode])],
 [AC_MSG_RESULT(ENC_UTF8)
 AC_DEFINE_UNQUOTED([DEFAULT_KANJI_ENC],[ENC_UTF8],[Default kanji encode])])

AC_ARG_ENABLE(kanji-iconv,
[  --enable-kanji-iconv     use iconv for kanji <=> unicode conversion],
   , enable_kanji_iconv=no)

if test "x$enable_kanji_iconv" = "xyes"; then
AC_DEFINE([KANJI_ICONV], 1,
 [Define 1 if you use iconv for kanji <=> unicode conversion])
AC_CHECK_LIB(iconv, iconv_open,,AC_CHECK_LIB(iconv, libiconv_open))
AC_CHECK_HEADERS(iconv.h)
AC_CHECK_FUNCS(iconv)
AC_CHECK_FUNCS(atexit on_exit, break)
fi


dnl Write output here, instead of putting a zillion -D's on the command line.
AC_CONFIG_HEADERS([c-auto.h:c-auto.in],
 [sed -e 's/^#define PACKAGE/#define PTEXENC_PACKAGE/' \
      -e 's/^#define VERSION/#define PTEXENC_VERSION/' c-auto.h >c-auto.tmp && mv -f c-auto.tmp c-auto.h])

AH_TOP([/* c-auto.h: defines for ptexenc, as determined by configure. */
/* Guard against double inclusion. */
#ifndef PTEXENC_C_AUTO_H
#define PTEXENC_C_AUTO_H

/* ptexenc: the version string. */
#define PTEXENCVERSION "REPLACE-WITH-PTEXENCVERSION"])
AH_BOTTOM([#endif /* !PTEXENC_C_AUTO_H */])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
