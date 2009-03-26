dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([TeX Live], [2009test-1.5], [tex-k@tug.org])
AC_PREREQ([2.63])
AC_CONFIG_SRCDIR([m4/kpse-pkgs.m4])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

dnl ## --------------------------------------------------- ##
dnl ## Setup of the teTeX / TeX Live (TL) infrastructure   ##
dnl ##    done in ./, libs/, utils/, and texk/             ##
dnl ## --------------------------------------------------- ##
dnl
dnl 1. Define three lists of TeXk programs, utility programs, and
dnl    TL libraries.  Hopefully these list occur nowhere else.
dnl
dnl 2. For each program PROG in the list of utility programs:
dnl    sinclude utils/PROG/ac/withenable.ac
dnl      providing the configure option --disable-PROG or --enable-PROG
dnl    if PROG is to be built:
dnl      set need_LIB=yes for each required TL library LIB (if any)
dnl
dnl 3. For each program PROG in the list of TeXk programs:
dnl    sinclude texk/PROG/ac/withenable.ac
dnl      providing the configure option --disable-PROG or --enable-PROG
dnl    if PROG is to be built:
dnl      set need_LIB=yes for each required TL library LIB (if any)
dnl
dnl 4. Sinclude texk/kpathsea/ac/withenable.ac
dnl
dnl 5. For each library LIB in the list of TL libraries:
dnl    sinclude libs/LIB/ac/withenable.ac
dnl      usually providing the configure options --with-system-LIB,
dnl        --with-LIB-includes=DIR, and --with-LIB-libdir=DIR
dnl     for each required TL library SUBLIB (if any):
dnl        disallow with_system_LIB=yes and with_system_SUBLIB=no
dnl
KPSE_SETUP([])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([TeX Live top-level])

AC_PATH_XTRA

AC_PROG_CXX

dnl Although quite unusual, it is possible to build Web2C (TeX & Co)
dnl using installed (system) kpathsea headers and library.
dnl In that case we need the location of <kpathsea/paths.h>.
if test "x$enable_web2c" = xyes && test "x$with_system_kpathsea" = xyes; then
  if test "x$with_kpathsea_includes" = x; then
    list="/usr/include /usr/local/include"
  else
    list=$with_kpathsea_includes
  fi
  found=no
  for ac_dir in $list; do
    if test -r "$ac_dir/kpathsea/paths.h"; then
      found=yes
      break
    fi
  done
  if test "x$found" = xno; then
    AC_MSG_NOTICE([You requested to build `web2c' using an installed `kpathsea' version,])
    AC_MSG_NOTICE([    which requires to locate the <kpathsea/paths.h> header file.])
    AC_MSG_ERROR([Sorry, not found under any of: $list *****])
  fi
fi

dnl We check that all required system libraries are usable, and may
dnl reduce frustration if anything is wrong by doing this first.
syslib_status=yes
KPSE_ALL_SYSTEM_FLAGS
KPSE_FOR_PKGS([syslib],
[m4_sinclude(m4_if(Kpse_Pkg, [kpathsea], [texk/],
                                         [libs/])Kpse_Pkg[/ac/]Kpse_Pkg[.ac])])[]dnl
if test "x$syslib_status" = xno; then
  AC_MSG_ERROR([some requested system libraries failed])
fi

AM_CONDITIONAL([build_kpathsea],
               [test "x$need_kpathsea" = xyes && test "x$with_system_kpathsea" != xyes])

AC_CONFIG_SUBDIRS([texk/kpathsea libs utils texk])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT

AS_IF([test "x$cross_compiling" = xyes],
      [AS_IF([test "x$build" = x],
             [AC_MSG_ERROR([when cross-compiling you must specify both --host and --build.])])
       msg_compiling="cross-compiling on $build"],
      [msg_compiling="compiling"])
AC_MSG_RESULT([
** Configuration summary for $PACKAGE_STRING:

   $msg_compiling for $host
   prefix:               $prefix
   enable_multiplatform: $enable_multiplatform

** Next run `make' to configure and build each library and program.
])
