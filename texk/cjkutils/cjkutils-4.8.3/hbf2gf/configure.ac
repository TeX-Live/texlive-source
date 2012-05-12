dnl This file is part of the CJK package ver. 4.8.3

dnl Copyright (C) 1994-2012  Werner Lemberg <wl@gnu.org>

dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or
dnl (at your option) any later version.

dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.

dnl You should have received a copy of the GNU General Public License
dnl along with this program in doc/COPYING; if not, write to the Free
dnl Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
dnl MA 02110-1301 USA

dnl Process this file with autoconf to produce a configure script.
AC_INIT
AC_CONFIG_SRCDIR([hbf.h])


dnl check host
AC_CANONICAL_TARGET([])


dnl Checks for programs.
AC_PROG_CC


dnl Checks for header files.
AC_HEADER_STDC

AC_ARG_WITH(kpathsea-include,
            [  --with-kpathsea-include=DIR
                          location of the kpathsea include dir],[
  if test x$withval = xyes; then
    AC_MSG_WARN(Usage is: --with-kpathsea-include=includedir)
  else
    if test x$withval = xno; then
      AC_MSG_WARN(Usage is: --with-kpathsea-include=includedir)
    else
      kpathsea_include=$withval
    fi
  fi
])


dnl Checks for libraries.
AC_ARG_WITH(kpathsea-lib,
            [  --with-kpathsea-lib=DIR
                          location of the kpathsea lib dir],[
  if test x$withval = xyes; then
    AC_MSG_WARN(Usage is: --with-kpathsea-lib=libdir)
  else
    if test x$withval = xno; then
      AC_MSG_WARN(Usage is: --with-kpathsea-lib=libdir)
    else
      kpathsea_lib=$withval
    fi
  fi
])


if test -n "$kpathsea_include" -o -n "$kpathsea_lib"; then
  LIBS="$LIBS -L$kpathsea_lib"
  CPPFLAGS="$CPPFLAGS -I$kpathsea_include -I$srcdir"

  dnl the function kpse_set_program_name() is available since kpathsea 3.2
  AC_CHECK_LIB(kpathsea, kpse_set_program_name, , AC_DEFINE(OLD_KPATHSEA))

  AC_CHECK_LIB(kpathsea, kpse_init_prog, ,[
               AC_MSG_ERROR([Can't find kpathsea library! Use --with-kpathsea-lib option.])])

  dnl the header file kpathsea.h doesn't exist in kpathsea 2.6 and before
  AC_CHECK_HEADER(kpathsea/kpathsea.h, , AC_DEFINE(VERY_OLD_KPATHSEA))

  AC_CHECK_HEADER(kpathsea/c-auto.h, ,[
                  AC_MSG_ERROR([Can't find kpathsea include files! Use --with-kpathsea-include option.])])
else
  CPPFLAGS="$CPPFLAGS -I$srcdir"
fi


dnl Get compiler flags right.
if test "x$CC" = xgcc; then
	XX_CFLAGS="-Wall"
else
	case "$host" in
  		alpha-dec-osf*)
        	XX_CFLAGS="-std1 -O2 -g3"
       		;;
  		*)
          	XX_CFLAGS=
       		;;
	esac
fi
AC_SUBST(XX_CFLAGS)


dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_STRUCT_TM


dnl Checks for library functions.
AC_FUNC_VPRINTF
AC_CHECK_FUNCS(strdup strstr strtol strtoul)

dnl we need this for hbf.c
if test "x$ac_cv_func_strdup" = xno; then
  no_strdup=-DNO_STRDUP
fi
AC_SUBST(no_strdup)

AC_CHECK_PROG(RM, rm, rm)
AC_PROG_INSTALL

if test -z "$kpathsea_include" -a -z "$kpathsea_lib"; then
  AC_MSG_WARN([

    hbf2gf will be compiled without file search library support!
    For kpathsea support use the --with-kpathsea-lib and
    --with-kpathsea-include options.
              ])
fi

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
