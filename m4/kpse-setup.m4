# Private macros for the teTeX / TeX Live (TL) tree.
# Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 0

# KPSE_SETUP(TOP-LEVEL)
# ---------------------
# Initialize path prefix kpse_TL to top-level teTeX / TeX Live (TL) directory.
# Sinclude all withenable.ac files providing:
#   configure options --with-system-LIB, --with-LIB-includes, and --with-LIB-libdir
#     for libraries
#   configure option --without-PKG for programs
#   library dependencies for programs and libraries
AC_DEFUN([KPSE_SETUP],
[m4_define([kpse_TL], [$1])[]dnl
m4_define([kpse_indent_26], [28])[]dnl
m4_define([kpse_indent_28], [30])[]dnl
KPSE_WEB2C_PREPARE
m4_sinclude(kpse_TL[ac/withenable.ac])
m4_sinclude(kpse_TL[utils/ac/withenable.ac])
KPSE_FOR_PKGS([utils], [m4_sinclude(kpse_TL[utils/]Kpse_Pkg[/ac/withenable.ac])])
m4_sinclude(kpse_TL[texk/ac/withenable.ac])
KPSE_FOR_PKGS([texk], [m4_sinclude(kpse_TL[texk/]Kpse_Pkg[/ac/withenable.ac])])
m4_sinclude(kpse_TL[texk/kpathsea/ac/withenable.ac])
m4_sinclude(kpse_TL[libs/ac/withenable.ac])
KPSE_FOR_PKGS([libs], [m4_sinclude(kpse_TL[libs/]Kpse_Pkg[/ac/withenable.ac])])
]) # KPSE_SETUP

# KPSE_WITH_PROG(PROG, REQUIRED-LIBS, OPTIONS, [COMMENT])
# -------------------------------------------------------
# Provide the configure option --enable-PROG if the option `without' is
# specified, or -disable-PROG otherwise.
# Define the list of libraries required from the TL tree (if any).
# Options: 
#          without - do not build by default
AC_DEFUN([KPSE_WITH_PROG],
[m4_pushdef([Kpse_with], m4_if(m4_index([ $3 ], [ without ]), [-1], [yes], [no]))[]dnl
AC_ARG_ENABLE([$1],
              AS_HELP_STRING([[--]m4_if(Kpse_with, [yes], [dis], [en])[able-$1]],
                              m4_if(Kpse_with, [yes],
                                    [do not ])[build the $1 ]m4_ifval([$4],
                                                                      [($4) ])[package]))[]dnl
case $enable_[]AS_TR_SH($1) in #(
  yes|no);; #(
  *) AC_MSG_NOTICE([Assuming `--]m4_if(Kpse_with, [no], [dis], [en])[able-$1'])
     enable_[]AS_TR_SH($1)=Kpse_with
     ac_configure_args="$ac_configure_args '--[]m4_if(Kpse_with, [no], [dis], [en])able-$1'";;
esac
m4_popdef([Kpse_with])[]dnl
m4_ifval([$2], [
test "x$enable_[]AS_TR_SH($1)" = xno || {
AC_FOREACH([Kpse_Lib], [$2], [  need_[]AS_TR_SH(Kpse_Lib)=yes
])}
])[]dnl m4_ifval
]) # KPSE_WITH_PROG

# KPSE_WITH_LIB(LIB, REQUIRED-LIBS, OPTIONS, [COMMENT-SYS],
                [COMMENT-INC], [COMMENT-LIB])
# ---------------------------------------------------------
# Unless the option `tree' is specified, provide the configure options
# --with-system-LIB, --with-LIB-includes, and --with-LIB-libdir.
# Define the list of libraries required from the TL tree (if any).
# Options: 
#          tree - only use library from the TL tree
# The last three arguments are optional additions to the help texts.
#
# At the top-level we build a (reversed) list of potential system libraries.
AC_DEFUN([KPSE_WITH_LIB],
[m4_if(m4_index([ $3 ], [ tree ]), [-1],
[AC_ARG_WITH([system-$1],
             [AS_HELP_STRING([--with-system-$1],
                             [use installed $1 headers and library]m4_ifval([$4], [ ($4)]))])[]dnl
AC_ARG_WITH([$1-includes],
            [AS_HELP_STRING([--with-$1-includes=DIR],
                            [$1 headers installed in DIR]m4_ifval([$5], [ ($5)]))])[]dnl
AC_ARG_WITH([$1-libdir],
            [AS_HELP_STRING([--with-$1-libdir=DIR],
                            [$1 library installed in DIR]m4_ifval([$6], [ ($6)]))])[]dnl
if test "x$with_system_[]AS_TR_SH($1)" = x; then
  if test -f $srcdir/kpse_TL[]m4_if([$1], [kpathsea], [texk], [libs])/$1/configure; then
    AC_MSG_NOTICE([Assuming `$1' headers and library from TL tree])
    with_system_[]AS_TR_SH($1)=no
  else
    AC_MSG_NOTICE([Assuming installed `$1' headers and library])
    with_system_[]AS_TR_SH($1)=yes
  fi
  ac_configure_args="$ac_configure_args '--with-system-$1=$with_system_[]AS_TR_SH($1)'"
m4_ifset([kpse_TL], [], dnl top level only
[elif test "x$with_system_[]AS_TR_SH($1)" = xyes; then
  AC_MSG_NOTICE([Using installed `$1' headers and library])
else
  AC_MSG_NOTICE([Using `$1' headers and library from TL tree])
  if test "x$with_system_[]AS_TR_SH($1)" != xno; then
    with_system_[]AS_TR_SH($1)=no
    ac_configure_args="$ac_configure_args '--without-system-$1'"
  fi
m4_define([kpse_syslib_pkgs], [$1]m4_ifval([kpse_syslib_pkgs],
                                           [ _m4_defn([kpse_syslib_pkgs])]))[]dnl
])[]dnl m4_ifset
fi
m4_ifval([$2],
[if test "x$with_system_[]AS_TR_SH($1)" = xyes; then
AC_FOREACH([Kpse_Lib], [$2],
[  if test "x$with_system_[]AS_TR_SH(Kpse_Lib)" = x; then
    AC_MSG_NOTICE([  ->  installed `AS_TR_SH(Kpse_Lib)' headers and library])
    with_system_[]AS_TR_SH(Kpse_Lib)=yes
    ac_configure_args="$ac_configure_args '--with-system-Kpse_Lib'"
  elif test "x$with_system_[]AS_TR_SH(Kpse_Lib)" != xyes; then
    AC_MSG_ERROR([Sorry, `--with-system-$1' requires `--with-system-Kpse_Lib'])
  fi
])fi
])[]dnl m4_ifval
])[]dnl m4_if
m4_ifval([$2], [
test "x$need_[]AS_TR_SH($1)" = xyes && {
AC_FOREACH([Kpse_Lib], [$2], [  need_[]AS_TR_SH(Kpse_Lib)=yes
])}
])[]dnl m4_ifval
]) # KPSE_WITH_LIB

# Initialize the list of potential system libraries.
m4_define([kpse_syslib_pkgs], [])

# KPSE_CHECK_LIB(LIB, REQUIRED-FUNCTION..., REQUIRED-HEADER...)
# -------------------------------------------------------------
# When the user requests to use an installed version of a required library,
# check that the flags derived from --with-LIB-includes and --with-LIB-libdir
# provide the required functions and headers.
AC_DEFUN([KPSE_CHECK_LIB],
[if test "x$need_[]AS_TR_SH($1):$with_system_[]AS_TR_SH($1)" = 'xyes:yes'; then
  AC_MSG_NOTICE([checking requested system `$1' library...])
  CPPFLAGS="$AS_TR_CPP($1)_INCLUDES $CPPFLAGS"
  LIBS="$AS_TR_CPP($1)_LIBS $LIBS"
  AC_CHECK_FUNCS([$2], , [syslib_status=no])
  AC_CHECK_HEADERS([$3], , [syslib_status=no])
fi
]) # KPSE_CHECK_LIB

# KPSE_CHECK_LIBXX(LIB, REQUIRED-FUNCTION..., REQUIRED-HEADER...)
# -------------------------------------------------------------
# As above, but for C++.
AC_DEFUN([KPSE_CHECK_LIBXX],
[AC_REQUIRE([AC_PROG_CXX])[]dnl
AC_LANG_PUSH([C++])[]dnl
KPSE_CHECK_LIB($@)[]dnl
AC_LANG_POP([C++])[]dnl
]) # KPSE_CHECK_LIBXX

