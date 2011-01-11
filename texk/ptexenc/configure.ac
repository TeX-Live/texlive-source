dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2010 Peter Breitenlohner <tex-live@tug.org>
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

m4_define([ptexenc_lib_major], [1])[]dnl
m4_define([ptexenc_lib_minor], [2])[]dnl
m4_define([ptexenc_lib_micro], [0])[]dnl
m4_define([ptexenc_lib_devel], [])[]dnl or "dev"
m4_define([ptexenc_version], [ptexenc_lib_major.ptexenc_lib_minor.ptexenc_lib_micro[]ptexenc_lib_devel])[]dnl

AC_INIT([ptexenc], [ptexenc_version], [tex-k@tug.org])
AC_PREREQ([2.63])
AC_CONFIG_SRCDIR([ptexenc.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

AC_SUBST([PTEXENCVERSION], [ptexenc_version])
AC_DEFINE([PTEXENCVERSION], ["ptexenc version ptexenc_version"])
AC_SUBST([PTEXENC_LT_VERSINFO],
         [m4_eval(ptexenc_lib_major+ptexenc_lib_minor):ptexenc_lib_micro:ptexenc_lib_minor])

dnl Common code for all programs (or libraries) using libkpathsea.
KPSE_COMMON([ptexenc])

KPSE_KPATHSEA_FLAGS

dnl Default pTeX character encoding.
AC_ARG_WITH([ptexenc-enc],
            AS_HELP_STRING([--with-ptexenc-enc=ENC],
                           [Default pTeX character encoding
                            (ENC=JIS/EUC/SJIS/UTF8, default UTF8)]))
AC_MSG_CHECKING([default pTeX character encoding])
AS_CASE([$with_ptexenc_enc],
        [jis|JIS], [DEFAULT_KANJI_ENC=ENC_JIS],
        [euc|EUC], [DEFAULT_KANJI_ENC=ENC_EUC],
        [sjis|SJIS], [DEFAULT_KANJI_ENC=ENC_SJIS],
        [''|utf8|UTF8], [DEFAULT_KANJI_ENC=ENC_UTF8],
            [DEFAULT_KANJI_ENC=ENC_UTF8
             AC_MSG_WARN([Unknown character encoding \`$with_ptexenc_enc'])])
AC_MSG_RESULT([$DEFAULT_KANJI_ENC])
AC_DEFINE_UNQUOTED([DEFAULT_KANJI_ENC], [$DEFAULT_KANJI_ENC],
                   [Default kanji encoding.])

dnl Using iconv for kanji <=> unicode conversion
AC_ARG_ENABLE([kanji-iconv],
              AS_HELP_STRING([--enable-kanji-iconv],
                             [Use iconv for kanji <=> unicode conversion]))

if test "x$enable_kanji_iconv" = "xyes"; then
AC_DEFINE([KANJI_ICONV], 1,
          [Define to 1 if you use iconv for kanji <=> unicode conversion.])
AC_CHECK_LIB([iconv], [iconv_open], ,[AC_CHECK_LIB([iconv], [libiconv_open])])
AC_CHECK_HEADERS([iconv.h])
AC_CHECK_FUNCS([iconv])
AC_CHECK_FUNCS([atexit on_exit], [break])
fi

dnl Write output here, instead of putting a zillion -D's on the command line.
AC_CONFIG_HEADERS([ptexenc/c-auto.h:c-auto.in],
 [sed -e 's/^#define PACKAGE/#define PTEXENC_PACKAGE/' \
      -e 's/^#define VERSION/#define PTEXENC_VERSION/' ptexenc/c-auto.h >c-auto.tmp \
    && mv -f c-auto.tmp ptexenc/c-auto.h])

AH_TOP([/* ptexenc/c-auto.h: defines for ptexenc, as determined by configure. */
/* Guard against double inclusion. */
#ifndef PTEXENC_C_AUTO_H
#define PTEXENC_C_AUTO_H

/* ptexenc: the version string. */
#define PTEXENCVERSION "REPLACE-WITH-PTEXENCVERSION"])
AH_BOTTOM([#endif /* !PTEXENC_C_AUTO_H */])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
