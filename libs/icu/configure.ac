dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009-2015 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define icu_version
AC_INIT([icu (TeX Live)], icu_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([icu-src/source/common/unicode/utypes.h])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

KPSE_BASIC([icu])

KPSE_CANONICAL_HOST

AM_CONDITIONAL([build], [test "x$enable_build" != xno])

AM_CONDITIONAL([cross], [test "x$cross_compiling" = xyes])

AC_PROG_CXX
AC_PROG_LN_S

ICU_TREE=[icu-src]
AC_SUBST([ICU_TREE])

# Prepare configure arguments for ICU.
# Can not share the cache file for various reasons.
icu_config="'\${abs_srcdir}'/$ICU_TREE/source/configure --enable-static --disable-shared"
icu_config="$icu_config --disable-extras --disable-samples --disable-tests"
if test "x$cache_file" != x/dev/null; then
  icu_config="$icu_config -C"
fi
# Add '--disable-dyload': neither XeTeX nor BibTeXU use plugins, new in ICU 4.4.
icu_config="$icu_config --disable-dyload --disable-layout"
AC_SUBST([icu_config])

# Add '--disable-strict' but not for solaris: required for MinGW32, otherwise reduce warnings.
if test "x$cross_compiling" = xyes; then
  icu_native_args="--build=$build_alias --host=$build_alias --disable-icuio"
  AS_CASE([$build_os],
          [solaris*], [],
            [icu_native_args="--disable-strict $icu_native_args"])
  icu_build_args="--build=$build_alias --host=$host_alias --with-cross-build='\${abs_builddir}'/icu-native"
else
  icu_build_args="--build=$kpse_build_alias"
  icu_native_args=
fi
AS_CASE([$host_os],
        [solaris*], [],
          [icu_build_args="--disable-strict $icu_build_args"])
eval "set x $ac_configure_args"
shift
for icu_arg
do
  case $icu_arg in
  BUILD*=*) icu_arg=`echo "$icu_arg" | sed 's/^BUILD//'`; icu_list=native ;;
  *) icu_list=build ;;
  esac
  case $icu_arg in
  CC=* | CFLAGS=* | CPPFLAGS=* | CXX=* | CXXFLAGS=* | LDFLAGS=*) ;;
  *) continue ;;
  esac
  case $icu_arg in
  *\'*) icu_arg=`echo "$icu_arg" | sed "s/'/'\\\\\\\\''/g"` ;;
  esac
  eval icu_${icu_list}_args="\"\$icu_${icu_list}_args '\$icu_arg'\""
done
AC_SUBST([icu_build_args])
AC_SUBST([icu_native_args])

AC_CONFIG_FILES([Makefile include/unicode/Makefile])

AC_OUTPUT
