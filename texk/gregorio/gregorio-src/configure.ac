dnl Gregorio
dnl Copyright (C) 2006-2015 The Gregorio Project (see CONTRIBUTORS.md)
dnl
dnl This file is part of Gregorio.
dnl 
dnl Gregorio is free software: you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation, either version 3 of the License, or
dnl (at your option) any later version.
dnl
dnl Gregorio is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with Gregorio.  If not, see <http://www.gnu.org/licenses/>.

AC_INIT([gregorio],[4.0.1],[gregorio-devel@gna.org])
FILENAME_VERSION="4_0_1"
AC_SUBST(FILENAME_VERSION)
MK=""
AC_SUBST(MK)
AC_PREREQ(2.59)
AC_CONFIG_SRCDIR([src/gregorio-utils.c])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([-Wall -Wno-portability subdir-objects foreign dist-bzip2 no-dist-gzip])

AC_PROG_CC_C89
dnl AM_PROG_CC_C_O is deprecated since Automake 1.14, to be removed in the future
AM_PROG_CC_C_O
AC_PROG_CPP
AM_PROG_LEX
AC_PROG_YACC
AC_CHECK_TOOL([RC], [windres], [no])
AM_CONDITIONAL([HAVE_RC], [test x$RC != xno])

AX_CHECK_COMPILE_FLAG([-std=gnu89], [CFLAGS+=" -std=gnu89"])
AX_CHECK_COMPILE_FLAG([-pedantic-errors], [CFLAGS+=" -pedantic-errors"])
AX_CHECK_COMPILE_FLAG([-fstack-protector-strong], [CFLAGS+=" -fstack-protector-strong"])
AX_CHECK_COMPILE_FLAG([-fPIE], [CFLAGS+=" -fPIE"])
AX_CHECK_COMPILE_FLAG([-Wformat], [CFLAGS+=" -Wformat"])
AX_CHECK_COMPILE_FLAG([-Werror=format-security], [CFLAGS+=" -Werror=format-security"])
AX_CHECK_COMPILE_FLAG([-Wstrict-prototypes], [CFLAGS+=" -Wstrict-prototypes"])
AX_CHECK_COMPILE_FLAG([-Wall], [CFLAGS+=" -Wall"])
AX_CHECK_COMPILE_FLAG([-Wextra], [CFLAGS+=" -Wextra"])
CPPFLAGS+=" -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=2 "
AX_CHECK_LINK_FLAG([-Wl,-z,relro], [LDFLAGS+=" -Wl,-z,relro"])
AX_CHECK_LINK_FLAG([-Wl,-z,now], [LDFLAGS+=" -Wl,-z,now"])
AX_CHECK_LINK_FLAG([-fPIE], [LDFLAGS+=" -fPIE"])
AX_CHECK_LINK_FLAG([-pie], [LDFLAGS+=" -pie"])

AC_HEADER_STDC

# linux has integer types in stdint.h, solaris, vms in inttypes.h
AC_CHECK_HEADERS([stdint.h])
AC_CHECK_HEADERS([stdalign.h])

AC_DEFINE([_GNU_SOURCE], [1], [Always define this.])

AC_C_BIGENDIAN

AC_ARG_WITH([kpathsea], AS_HELP_STRING([--with-kpathsea], [Build with kpathsea.]))

AS_IF([test "x$with_kpathsea" = "xyes"], [
    PKG_PROG_PKG_CONFIG
    PKG_CHECK_MODULES([KPSE], [kpathsea], AC_DEFINE([USE_KPSE], [1], [Use kpathsea]),
      dnl if pkg-config doesn't know kpathsea, try -lkpathsea (the most common case)
      AC_CHECK_LIB([kpathsea], [kpse_set_program_name], [
          AC_DEFINE([USE_KPSE], [1], [Use kpathsea])
          KPSE_LIBS=" -lkpathsea"
      ], [
          AC_MSG_ERROR([unable to find the kpathsea library])
      ])
    )
])

AC_CONFIG_HEADERS([src/config_.h])
AC_CONFIG_FILES([
    Makefile
    src/Makefile
    examples/Makefile
    contrib/Makefile
    doc/Makefile
    tex/Makefile
    fonts/Makefile
])
AC_OUTPUT
