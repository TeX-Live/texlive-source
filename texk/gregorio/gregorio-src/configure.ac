dnl Gregorio
dnl Copyright (C) 2006-2019 The Gregorio Project (see CONTRIBUTORS.md)
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

AC_INIT([gregorio],[5.2.1],[https://github.com/gregorio-project/gregorio/issues],[gregorio],[http://gregorio-project.github.io/])
FILENAME_VERSION="5_2_1"
AC_SUBST(FILENAME_VERSION)
AC_DEFINE_UNQUOTED(FILENAME_VERSION, "$FILENAME_VERSION", [version suitable for file names])
MK=""
AC_SUBST(MK)
AC_PREREQ(2.59)
AC_CONFIG_SRCDIR([src/gregorio-utils.c])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([-Wall -Wno-portability subdir-objects foreign dist-bzip2 no-dist-gzip])

AC_PROG_CC
dnl AM_PROG_CC_C_O is deprecated since Automake 1.14, to be removed in the future
AM_PROG_CC_C_O
AC_PROG_CPP
AM_PROG_LEX
AC_PROG_YACC
AC_CHECK_TOOL([RC], [windres], [no])
AM_CONDITIONAL([HAVE_RC], [test x$RC != xno])

AC_CHECK_ALIGNOF(uint32_t)

AX_CHECK_COMPILE_FLAG([-std=gnu89], [CFLAGS+=" -std=gnu89"])
dnl until flex gets their act together, use pedantic instead of pedantic-errors
AX_CHECK_COMPILE_FLAG([-pedantic], [CFLAGS+=" -pedantic"])
dnl AX_CHECK_COMPILE_FLAG([-pedantic-errors], [CFLAGS+=" -pedantic-errors"])
AX_CHECK_COMPILE_FLAG([-fstack-protector-strong], [CFLAGS+=" -fstack-protector-strong"])
AX_CHECK_COMPILE_FLAG([-fPIE], [CFLAGS+=" -fPIE"])
AX_CHECK_COMPILE_FLAG([-Wformat=2], [CFLAGS+=" -Wformat=2"])
AX_CHECK_COMPILE_FLAG([-Werror=format-security], [CFLAGS+=" -Werror=format-security"])
AX_CHECK_COMPILE_FLAG([-Wstrict-prototypes], [CFLAGS+=" -Wstrict-prototypes"])
AX_CHECK_COMPILE_FLAG([-Wdeclaration-after-statement], [CFLAGS+=" -Wdeclaration-after-statement"])
AX_CHECK_COMPILE_FLAG([-Wall], [CFLAGS+=" -Wall"])
AX_CHECK_COMPILE_FLAG([-Wextra], [CFLAGS+=" -Wextra"])
AX_CHECK_COMPILE_FLAG([-Wno-gnu-statement-expression], [CFLAGS+=" -Wno-gnu-statement-expression"])
AX_CHECK_LINK_FLAG([-Wl,-z,relro], [LDFLAGS+=" -Wl,-z,relro"])
AX_CHECK_LINK_FLAG([-Wl,-z,now], [LDFLAGS+=" -Wl,-z,now"])
AX_CHECK_LINK_FLAG([-fPIE], [LDFLAGS+=" -fPIE"])
AX_CHECK_LINK_FLAG([-pie], [LDFLAGS+=" -pie"])

AC_HEADER_STDC

gl_FUNC_REALPATH_WORKS

dnl linux has integer types in stdint.h, solaris, vms in inttypes.h
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

AC_ARG_ENABLE([debug], AS_HELP_STRING([--enable-debug@<:@=sanitize,coverage@:>@], [Enable debugging options [with additional sanitize options].]), [
    AS_IF([test "x$enableval" != "xno"], [
        dnl AX_CHECK_COMPILE_FLAG([-g], [CFLAGS+=" -g"])
        AX_CHECK_COMPILE_FLAG([-Og], [CFLAGS+=" -Og"], [
            AX_CHECK_COMPILE_FLAG([-O0], [CFLAGS+=" -O0"])
        ])
        AS_IF([test "x$enableval" = "xsanitize"], [
            AX_CHECK_COMPILE_FLAG([-fsanitize=address,undefined], [CFLAGS+=" -fsanitize=address,undefined"])
        ])
        AS_IF([test "x$enableval" = "xcoverage"], [
            AX_CHECK_COMPILE_FLAG([--coverage], [CFLAGS+=" --coverage"])
            AX_CHECK_LINK_FLAG([--coverage], [LDFLAGS+=" --coverage"])
        ])
    ], [
        CPPFLAGS+=" -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=2 "
    ])
], [
    CPPFLAGS+=" -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=2 "
])

AC_ARG_ENABLE([version-in-exe], AS_HELP_STRING([--disable-version-in-exe], [Disable version in executable name.]), [
    AS_IF([test "x$enableval" != "xno"], [
        GREGORIO_EXE_SUFFIX="-$FILENAME_VERSION"
    ], [
        GREGORIO_EXE_SUFFIX=""
    ])
], [
    GREGORIO_EXE_SUFFIX="-$FILENAME_VERSION"
])
AC_SUBST(GREGORIO_EXE_SUFFIX)

AC_CONFIG_HEADERS([src/config_.h])
AC_CONFIG_FILES([
    Makefile
    src/Makefile
    examples/Makefile
    contrib/Makefile
    contrib/TeXShop/Makefile
    doc/Makefile
    tex/Makefile
    fonts/Makefile
])
AC_OUTPUT
