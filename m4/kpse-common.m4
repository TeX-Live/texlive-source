# $Id$
# Public macros for the TeX Live (TL) tree.
# Copyright 1995-2009, 2018 Karl Berry <tex-live@tug.org>
# Copyright 2009-2015 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_LIBS_PREPARE
# -----------------
# Enforce inclusion of this file.
AC_DEFUN([KPSE_LIBS_PREPARE], [])

# _KPSE_INIT()
# ------------
# Initialize infrastructure for libraries and programs in the TL tree.
# If in the TL tree, define kpse_TL as relative path to the TL root.
AC_DEFUN([_KPSE_INIT],
[##tldbg $0: Initialize TL infrastructure.
m4_syscmd([test -f ../../texk/kpathsea/doc/kpathsea.texi])[]dnl
m4_if(m4_sysval, [0], [m4_define([kpse_TL], [../../])])[]dnl
m4_ifdef([kpse_TL],
[kpse_BLD=`(cd "./kpse_TL()." && pwd)`
kpse_SRC=`(cd "$srcdir/kpse_TL()." && pwd)`
])[]dnl
]) # _KPSE_INIT

# KPSE_INIT()
# -----------
# Initialize, if not automatically done so via KPSE_*_FLAGS
AC_DEFUN([KPSE_INIT],
[AC_REQUIRE([_KPSE_INIT])])

# _KPSE_USE_LIBTOOL()
# -------------------
AC_DEFUN([_KPSE_USE_LIBTOOL],
[echo 'tldbg:[$0] called, libtool package version: LT_PACKAGE_VERSION' >&AS_MESSAGE_LOG_FD
AC_PROVIDE_IFELSE([LT_INIT], ,
                  [m4_fatal([$0: requires libtool])])[]dnl
LT_OUTPUT
#
m4_append([AC_LANG(C)],
[ac_link="./libtool --mode=link --tag=CC $ac_link"
])[]dnl
#
AC_PROVIDE_IFELSE([AC_PROG_CXX],
[m4_append([AC_LANG(C++)],
[ac_link="./libtool --mode=link --tag=CXX $ac_link"
])])[]dnl
AC_LANG(_AC_LANG)[]dnl
#
# am_prog_ar needed for dvisvgm, at least.
AM_PROG_AR
]) # _KPSE_USE_LIBTOOL

# _KPSE_LIB_FLAGS(LIBDIR, LIBNAME, OPTIONS,
#                 TL-INCLUDES, TL-LIBS, TL-EXTRA,
#                 [REBUILD-SRC-DEPENDENCIES],
#                 [REBUILD-BLD-DEPENDENCIES])
# -----------------------------------------------
# For generic libraries in libs/LIBDIR.
# Provide the configure options '--with-system-LIBDIR' (if in the TL tree)
# and, if applicable '--with-LIBDIR-includes' and '--with-LIBDIR-libdir'.
# Options:
#          lt - this is a Libtool library
#          tree - only use library from the TL tree
#
# Set the make variables LIBDIR_INCLUDES and LIBDIR_LIBS to the CPPFLAGS and
# LIBS required for the library in the directory libs/LIBDIR/ of the TL tree.
# The resulting values may be used by configure tests; thus they must not
# use, e.g., ${srcdir} or ${top_builddir}.
#
# If an installed (system) version of the library has been selected or is
# implied, then execute KPSE_LIBDIR_SYSTEM_FLAGS to set the two variables.
#
# Otherwise, set LIBDIR_INCLUDES based on the value of TL_INCLUDES and
# LIBDIR_LIBS based on the value of TL_LIBS; the optional shell code
# TL-EXTRA may modifiy these values.
# If OPTIONS specifies a Libtool library, then arrange that future configure
# test use Libtool. 
# Furthermore, set LIBDIR_DEPEND and LIBDIR_RULE to values suitable as
# dependency and (multiline) Make rule to rebuild the library (assuming that
# '${top_srcdir}/../../' and '${top_builddir}/../../' point to the root of
# the TL tree).
AC_DEFUN([_KPSE_LIB_FLAGS],
[AC_REQUIRE([_KPSE_INIT])[]dnl
##tldbg $0: Setup $1 (-l$2) flags.
echo 'tldbg:[$0] called: libdir=[$1], libname=[$2], options=[$3], tlincl=[$4], tllib=[$5], tlextra=[$6], rebuildsrcdeps=[$7], rebuildblddeps=[$8].' >&AS_MESSAGE_LOG_FD
m4_ifdef([kpse_TL],
         [_KPSE_LIB_FLAGS_TL($@)],
         [_KPSE_LIB_FLAGS_STANDALONE($@)])[]dnl
AC_SUBST(AS_TR_CPP($1)[_INCLUDES])[]dnl
AC_SUBST(AS_TR_CPP($1)[_LIBS])[]dnl
AC_SUBST(AS_TR_CPP($1)[_DEPEND])[]dnl
AC_SUBST(AS_TR_CPP($1)[_RULE])[]dnl
m4_provide_if([AM_INIT_AUTOMAKE], [_AM_SUBST_NOTMAKE(AS_TR_CPP($1)[_RULE])])[]dnl
]) # _KPSE_LIB_FLAGS

# _KPSE_TEXLIB_FLAGS(LIBDIR, LIBNAME, OPTIONS,
#                    TL-INCLUDES, TL-LIBS, TL-EXTRA,
#                    [REBUILD-SRC-DEPENDENCIES],
#                    [REBUILD-BLD-DEPENDENCIES])
# -----------------------------------------------
# As above, but for TeX specific libraries in texk/LIBDIR.
AC_DEFUN([_KPSE_TEXLIB_FLAGS],
[m4_pushdef([Kpse_TeX_Lib], [])_KPSE_LIB_FLAGS($@)m4_popdef([Kpse_TeX_Lib])])

# _KPSE_LIB_FLAGS_TL(LIBDIR, LIBNAME, OPTIONS,
#                    TL-INCLUDES, TL-LIBS, TL-EXTRA,
#                    [REBUILD-SRC-DEPENDENCIES],
#                    [REBUILD-BLD-DEPENDENCIES])
# --------------------------------------------------
# Internal subroutine for use of _KPSE_LIB_FLAGS inside the TL tree.
m4_define([_KPSE_LIB_FLAGS_TL],
[##tldbg $0: $1 ($2) $3.
m4_if(m4_index([ $3 ], [ lt ]), [-1], ,
       [AC_REQUIRE([_KPSE_USE_LIBTOOL])])[]dnl m4_if
m4_if(m4_index([ $3 ], [ tree ]), [-1],
[KPSE_]AS_TR_CPP([$1])[_OPTIONS([with-system])[]dnl
if test "x$with_system_[]AS_TR_SH($1)" = xyes; then
  ]AS_TR_CPP([kpse-$1-system-flags])[[]dnl
else
])[]dnl m4_if
  AS_TR_CPP($1)[_INCLUDES=]m4_bpatsubst(m4_bpatsubst(["$4"], [SRC], [$kpse_SRC]),
                                                             [BLD], [$kpse_BLD])
  AS_TR_CPP($1)[_LIBS=]m4_bpatsubst(["$5"], [BLD], [$kpse_BLD])m4_ifval([$6], [[
  $6]])
  AS_TR_CPP($1)[_DEPEND=]m4_ifdef([Kpse_TeX_Lib],
                                  [m4_bpatsubst(['$5'], [BLD/texk],
                                                [${top_builddir}/..])m4_pushdef([Kpse_Lib_Bld],
                                                                                [..])],
                                  [m4_bpatsubst(['$5'], [BLD],
                                                [${top_builddir}/../..])m4_pushdef([Kpse_Lib_Bld],
                                                                                   [../../libs])])
  AS_TR_CPP($1)[_RULE='# Rebuild lib$2
$(]AS_TR_CPP($1)[_DEPEND):]m4_ifval([$7],
                                    [[ $7]])m4_ifval([$8], [[ $8
	cd ${top_builddir}/]Kpse_Lib_Bld[/$1 && $(MAKE) $(AM_MAKEFLAGS) rebuild
$8:]])[
	cd ${top_builddir}/]Kpse_Lib_Bld[/$1 && $(MAKE) $(AM_MAKEFLAGS) rebuild']m4_popdef([Kpse_Lib_Bld])
m4_if(m4_index([ $3 ], [ tree ]), [-1],
      [fi
])[]dnl m4_if
]) # _KPSE_LIB_FLAGS_TL

# _KPSE_LIB_FLAGS_STANDALONE(LIBDIR, LIBNAME, OPTIONS)
# ----------------------------------------------------
# Internal subroutine for standalone use of _KPSE_LIB_FLAGS.
m4_define([_KPSE_LIB_FLAGS_STANDALONE],
[##tldbg $0: $1 ($2) $3.
m4_if(m4_index([ $3 ], [ tree ]), [-1],
[KPSE_]AS_TR_CPP([$1])[_OPTIONS([])]dnl
[KPSE_]AS_TR_CPP([$1])[_SYSTEM_FLAGS],
[m4_fatal([$0: not in TL tree])])[]dnl m4_if
]) # _KPSE_LIB_FLAGS_STANDALONE

# _KPSE_LIB_OPTIONS(LIBDIR, [WITH-SYSTEM], [CONFIG-PROG])
# -------------------------------------------------------
# Internal subroutine: default configure options for system library,
# including '--with-system-LIBDIR' if WITH-SYSTEM is nonempty.
m4_define([_KPSE_LIB_OPTIONS], [m4_ifval([$2], [dnl
AC_ARG_WITH([system-$1],
            AS_HELP_STRING([--with-system-$1],
                           [use installed $1 headers and library]m4_ifval([$3], [ (requires $3)])))[]dnl
m4_ifval([$3], [], [dnl
AC_ARG_WITH([$1-includes],
            AS_HELP_STRING([--with-$1-includes=DIR],
                           [$1 headers installed in DIR]))[]dnl
AC_ARG_WITH([$1-libdir],
            AS_HELP_STRING([--with-$1-libdir=DIR],
                           [$1 library installed in DIR]))[]dnl
])])]) # _KPSE_LIB_OPTIONS

# _KPSE_LIB_FLAGS_SYSTEM(LIBDIR, LIBNAME)
# ---------------------------------------
# Internal subroutine: default flags for system library.
m4_define([_KPSE_LIB_FLAGS_SYSTEM],
[##tldbg $0: $1 ($2).
if test "x$with_[]AS_TR_SH($1)_includes" != x && test "x$with_[]AS_TR_SH($1)_includes" != xyes; then
  AS_TR_CPP($1)_INCLUDES="-I$with_[]AS_TR_SH($1)_includes"
fi
AS_TR_CPP($1)_LIBS="-l$2"
if test "x$with_[]AS_TR_SH($1)_libdir" != x && test "x$with_[]AS_TR_SH($1)_libdir" != xyes; then
  AS_TR_CPP($1)_LIBS="-L$with_[]AS_TR_SH($1)_libdir $AS_TR_CPP($1)_LIBS"
fi
]) # _KPSE_LIB_FLAGS_SYSTEM

# KPSE_SAVE_FLAGS
# ---------------
# Save values of CPPFLAGS and LIBS.
AC_DEFUN([KPSE_SAVE_FLAGS],
[kpse_save_CPPFLAGS=$CPPFLAGS
kpse_save_LIBS=$LIBS
]) # KPSE_SAVE_FLAGS

# KPSE_RESTORE_FLAGS
# ------------------
# Restore values of CPPFLAGS and LIBS.
AC_DEFUN([KPSE_RESTORE_FLAGS],
[AC_REQUIRE([KPSE_SAVE_FLAGS])[]dnl
CPPFLAGS=$kpse_save_CPPFLAGS
LIBS=$kpse_save_LIBS
]) # KPSE_RESTORE_FLAGS

# KPSE_ADD_FLAGS(LIBDIR)
# ----------------------
# Add flags for LIBDIR to values of CPPFLAGS and LIBS.
AC_DEFUN([KPSE_ADD_FLAGS],
[AC_REQUIRE([KPSE_SAVE_FLAGS])[]dnl
eval CPPFLAGS=\"$[]AS_TR_CPP($1)_INCLUDES \$CPPFLAGS\"
eval LIBS=\"$[]AS_TR_CPP($1)_LIBS \$LIBS\"
]) # KPSE_ADD_FLAGS

# KPSE_BASIC(PACKAGE-NAME, [MORE-AUTOMAKE-OPTIONS])
#--------------------------------------------------
# Common Autoconf code for all libraries and programs.
#
# Initialization of Automake, compiler warnings, etc.
AC_DEFUN([KPSE_BASIC], [dnl
##tldbg $0: Remember $1 ($2) as Kpse_Package (for future messages).
m4_define([Kpse_Package], [$1])
dnl
AM_INIT_AUTOMAKE([foreign silent-rules subdir-objects]m4_ifval([$2], [ $2]))
AM_MAINTAINER_MODE
dnl
dnl Check whether prototypes work.
AC_CACHE_CHECK([whether the compiler accepts prototypes],
               [kb_cv_c_prototypes],
               [AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <stdarg.h>]],
                                                [[extern void foo(int i,...);]])],
                               [kb_cv_c_prototypes=yes],
                               [kb_cv_c_prototypes=no])])
if test "x$kb_cv_c_prototypes" = xno; then
  AC_MSG_ERROR([Sorry, your compiler does not understand prototypes.])
fi
dnl
dnl Enable flags for compiler warnings
KPSE_COMPILER_WARNINGS
]) # KPSE_BASIC

# KPSE_COMMON(PACKAGE-NAME, [MORE-AUTOMAKE-OPTIONS])
# --------------------------------------------------
# Common Autoconf code for all programs using libkpathsea.
# Originally written by Karl Berry as texk/kpathsea/common.ac.
#
AC_DEFUN([KPSE_COMMON], [dnl
##tldbg $0: $1 ($2).
KPSE_BASIC($@)
dnl
LT_PREREQ([2.2.6])
LT_INIT([win32-dll])
dnl
AC_SYS_LARGEFILE
AC_FUNC_FSEEKO
dnl
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_FUNC_CLOSEDIR_VOID
AC_CHECK_HEADERS([assert.h float.h limits.h pwd.h stdlib.h sys/param.h])
dnl
dnl Replacement functions that may be required on ancient broken system.
AC_CHECK_FUNCS([putenv])
dnl
dnl More common functions
AC_CHECK_FUNCS([getcwd getwd memcmp memcpy mkstemp mktemp strchr strrchr])
dnl
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_INT64_T
AC_TYPE_UINT64_T
AS_CASE([:$ac_cv_c_int64_t:$ac_cv_c_int64_t:],
        [*':no:'*], [AC_MSG_ERROR([Sorry, your compiler does not support 64-bit integer types.])])
dnl
dnl Check if <ctype.h> declares isascii.
AC_CHECK_DECLS([isascii], [], [], [[#include <ctype.h>]])
dnl
dnl Check whether struct stat provides high-res time.
AC_CHECK_MEMBERS([struct stat.st_mtim])
]) # KPSE_COMMON

# KPSE_MSG_WARN(PROBLEM)
# ----------------------
# Same as AC_MSG_WARN, but terminate if `--disable-missing' was given.
AC_DEFUN([KPSE_MSG_WARN],
[AC_REQUIRE([_KPSE_MSG_WARN_PREPARE])[]dnl
AC_MSG_WARN([$1])
AS_IF([test "x$enable_missing" = xno],
      [AC_MSG_ERROR([terminating.])])
]) # KPSE_MSG_WARN

# _KPSE_MSG_WARN_PREPARE
# ----------------------
# Internal subroutine.
AC_DEFUN([_KPSE_MSG_WARN_PREPARE],
[AC_ARG_ENABLE([missing],
               AS_HELP_STRING([--disable-missing],
                              [terminate if a requested program or feature must
                               be disabled, e.g., due to missing libraries]))[]dnl
]) # _KPSE_MSG_WARN_PREPARE

# _KPSE_CHECK_PKG_CONFIG
# ----------------------
# Check for pkg-config
AC_DEFUN([_KPSE_CHECK_PKG_CONFIG], [dnl
AC_REQUIRE([AC_CANONICAL_HOST])[]dnl
AC_CHECK_TOOL([PKG_CONFIG], [pkg-config], [false])[]dnl
]) # _KPSE_CHECK_PKG_CONFIG

# _KPSE_PKG_CONFIG_FLAGS(PACKAGE-NAME, PKG_CONFIG_NAME, [AT_LEAST])
# -----------------------------------------------------------------
# Use pkg-config to determine INCLUDES and LIBS for a system library.
AC_DEFUN([_KPSE_PKG_CONFIG_FLAGS], [dnl
AC_REQUIRE([_KPSE_CHECK_PKG_CONFIG])[]dnl  
if $PKG_CONFIG $2[]m4_ifval([$3], [ --atleast-version=$3]); then
  AS_TR_CPP($1)_INCLUDES=`$PKG_CONFIG $2 --cflags`
  AS_TR_CPP($1)_LIBS=`$PKG_CONFIG $2 --libs`
elif test "x$need_[]AS_TR_SH($1):$with_system_[]AS_TR_SH($1)" = xyes:yes; then
  AC_MSG_ERROR([did not find $2[]m4_ifval([$3], [ $3 or better])]) 
fi
]) # _KPSE_PKG_CONFIG_FLAGS

# KPSE_CANONICAL_HOST
# -------------------
# Require both --host and --build for cross compilations; set kpse_build_alias.
AC_DEFUN([KPSE_CANONICAL_HOST], [dnl
AC_REQUIRE([AC_CANONICAL_HOST])[]dnl
AS_IF([test "x$host_alias" != x && test "x$build_alias" = x],
      [AC_MSG_ERROR([when cross-compiling you must specify both --host and --build.])])
eval kpse_build_alias=\${build_alias-$build}
]) # KPSE_CANONICAL_HOST

# KPSE_NATIVE_SUBDIRS(DIR ...)
# ----------------------------
# Similar to AC_CONFIG_SUBDIRS for subdirectories configured for the build system.
# When cross compiling, the subdirectories need a different cache file.
AC_DEFUN([KPSE_NATIVE_SUBDIRS], [dnl
AC_REQUIRE([KPSE_CANONICAL_HOST])[]dnl
AC_CONFIG_SUBDIRS($@)
AC_CONFIG_COMMANDS_POST([dnl
AS_IF([test "x$cross_compiling" = xyes], [dnl
AS_IF([test "x$cache_file" != x/dev/null],
      [cache_file=config.cache])
ac_configure_args="$ac_configure_args --host='$kpse_build_alias' \
CC='$BUILDCC' CFLAGS='$BUILDCFLAGS' \
CPPFLAGS='$BUILDCPPFLAGS' LDFLAGS='$BUILDLDFLAGS'"])])
]) # KPSE_NATIVE_SUBDIRS
