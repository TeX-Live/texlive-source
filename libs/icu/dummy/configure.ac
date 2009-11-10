dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([TeX Live libs icu dummy], [2009], [tex-k@tug.org])
AC_PREREQ([2.63])
AC_CONFIG_SRCDIR([../../../build-aux/missing])
AC_CONFIG_AUX_DIR([../../../build-aux])
AC_CONFIG_MACRO_DIR([../../../m4])

AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE
KPSE_CANONICAL_HOST

# Prepare configure arguments for ICU.
# Can not share the cache file for various reasons.
icu_configure_args=
icu_skip=
eval "set x $ac_configure_args"
shift
for icu_arg
do
  if test -n "$icu_skip"; then
    icu_skip=
    continue
  fi
  case $icu_arg in
  -build | --build | --buil | --bui | --bu)
    icu_skip=yes; continue ;;
  -build=* | --build=* | --buil=* | --bui=* | --bu=*)
    continue ;;

  -cache-file | --cache-file | --cache-fil | --cache-fi \
  | --cache-f | --cache- | --cache | --cach | --cac | --ca | --c)
    icu_skip=yes; continue ;;
  -cache-file=* | --cache-file=* | --cache-fil=* | --cache-fi=* \
  | --cache-f=* | --cache-=* | --cache=* | --cach=* | --cac=* | --ca=* | --c=*)
    continue ;;

  --config-cache | -C)
    continue ;;

  -host | --host | --hos | --ho)
    icu_skip=yes; continue ;;
  -host=* | --host=* | --hos=* | --ho=*)
    continue ;;

  -disable-shared | --disable-shared | -disable-static | --disable-static \
  | -enable-shared | --enable-shared | -enable-static | --enable-static \
  | -enable-shared=* | --enable-shared=* | -enable-static=* | --enable-static=*)
    continue ;;

  *\'*)
    icu_arg=`echo "$icu_arg" | sed "s/'/'\\\\\\\\''/g"` ;;
  esac
  icu_configure_args="$icu_configure_args '$icu_arg'"
done
icu_configure_args="$icu_configure_args --enable-static --disable-shared\
 --disable-extras --disable-samples --disable-tests"
if test "x$cache_file" != x/dev/null; then
  icu_configure_args="$icu_configure_args -C"
fi

echo "$icu_configure_args" | sed 's,/icu/dummy,/icu/ICUSUBDIR,g' >../icusubdir-conf.args

# Additional configure arguments for native and host
if test "x$cross_compiling" = xyes; then
  icu_native_args="'--build=$build_alias' '--host=$build_alias'\
 CC='$BUILDCC' CPPFLAGS='$BUILDCPPFLAGS' CFLAGS='$BUILDCFLAGS'\
 CXX='$BUILDCXX' CXXFLAGS='$BUILDCXXFLAGS' LDFLAGS='$BUILDLDFLAGS'"
  echo "$icu_native_args" >../icu-native.args
  native_dir=`pwd | sed 's,/icu/dummy,/icu/icu-native,'`
  icu_build_args="'--host=$host_alias' '--build=$build_alias'\
 '--with-cross-build=$native_dir'"
else
  icu_build_args="'--build=$kpse_build_alias'"
fi
echo "$icu_build_args" >../icu-build.args

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
