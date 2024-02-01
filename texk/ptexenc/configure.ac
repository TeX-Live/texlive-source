dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2010-2015 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define ptexenc_version
AC_INIT([ptexenc], ptexenc_version, [tex-k@tug.org])
AC_PREREQ([2.63])
AC_CONFIG_SRCDIR([ptexenc.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

AC_SUBST([PTEXENCVERSION], [ptexenc_version])
KPSE_LT_VERSION([ptexenc])

dnl Common code for all programs (or libraries) using libkpathsea.
KPSE_COMMON([ptexenc])

KPSE_KPATHSEA_FLAGS

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
#define PTEXENCVERSION "ptexenc version] ptexenc_version["])
AH_BOTTOM([#endif /* !PTEXENC_C_AUTO_H */])

m4_include([../../version.ac])[] dnl define tex_live_version
AC_DEFINE([TL_VERSION], ["TeX Live tex_live_version()"],
          [Define to the current TeX Live version string.])

AC_CONFIG_FILES([Makefile ptexenc.pc])

AC_OUTPUT
