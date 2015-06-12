# Private macros for the TeX Live (TL) tree.
# Copyright (C) 2009-2015 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_WEB2C_PREPARE
# ------------------
# AC_DEFUN'ed so it can enforce inclusion of this file.
AC_DEFUN([KPSE_WEB2C_PREPARE], [])

# KPSE_WITH_XTEX(PROG, BUILD-OR-NO, SYNC-OR-NO, TEXT, REQUIRED-LIBS)
# ------------------------------------------------------------------
# Provide and normalize the configure options --enable-*tex.
m4_define([KPSE_WITH_XTEX], [dnl
AC_ARG_ENABLE([$1],
              AS_HELP_STRING([--]m4_if([$2], [yes], [dis], [en])[able-$1],
                             m4_if([$2], [yes],
                                   [do not ])[compile and install $4],
                             kpse_indent_26))[]dnl
AS_CASE([$enable_$1],
        [yes | no], ,
        [enable_$1=$2])
m4_ifval([$3], [
AC_ARG_ENABLE([$1-synctex],
              AS_HELP_STRING([--]m4_if([$3], [yes], [dis], [en])[able-$1-synctex],
                             [build $4 with]m4_if([$3], [yes], [out])[ SyncTeX support],
                             m4_eval(kpse_indent_26+2)))[]dnl
])[]dnl m4_ifval
m4_ifval([$5], [
test "x$enable_web2c:$enable_$1" = xyes:yes && {
AC_FOREACH([Kpse_Lib], [$5], [  need_[]AS_TR_SH(Kpse_Lib)=yes
])}
])[]dnl m4_ifval
]) # KPSE_WITH_XTEX

# KPSE_XTEX_COND(PROG, BUILD-OR-NO, SYNC-OR-NO, TEXT, REQUIRED-LIBS)
# ------------------------------------------------------------------
# Normalize --enable-*tex-synctex configure option and build conditionals.
m4_define([KPSE_XTEX_COND], [dnl
AM_CONDITIONAL(AS_TR_CPP($1), [test "x$enable_$1" = xyes])[]dnl
m4_ifval([$3], [
AS_CASE([$enable_native_texlive_build:$enable_$1_synctex],
        [yes:$3 | no:yes | no:no], ,
        [AS_IF([test -z "$enable_$1_synctex"],
               [AC_MSG_NOTICE([Assuming `--enable-$1-synctex=$3'])],
               [AC_MSG_WARN([Enforcing `--enable-$1-synctex=$3' (native TeX Live build)])])
         enable_$1_synctex=$3])
AM_CONDITIONAL(AS_TR_CPP($1)[_SYNCTEX], [test "x$enable_$1_synctex" = xyes])[]dnl
])[]dnl m4_ifval
]) # KPSE_XTEX_COND

# KPSE_WITH_XMF(PROG, BUILD-OR-NO, NOWIN-OR-NO, TEXT, REQUIRED-LIBS)
# ------------------------------------------------------------------
# Provide and normalize the configure options --enable-mf*.
m4_define([KPSE_WITH_XMF], [dnl
AC_ARG_ENABLE([$1],
              AS_HELP_STRING([--]m4_if([$2], [yes], [dis], [en])[able-$1],
                             m4_if([$2], [yes],
                                   [do not ])[compile and install $4],
                             kpse_indent_26))[]dnl
AS_CASE([$enable_$1],
        [yes | no], ,
        [enable_$1=$2])
m4_ifval([$3], [
AC_ARG_ENABLE([$1-nowin],
              AS_HELP_STRING([--]m4_if([$3], [yes], [dis], [en])[able-$1-nowin],
                             m4_if([$3], [yes],
                                   [do not ])[build a separate non-window-capable $4],
                             m4_eval(kpse_indent_26+2)))[]dnl
AS_CASE([$enable_$1_nowin],
        [yes | no], ,
        [enable_$1_nowin=$3])
])[]dnl m4_ifval
m4_ifval([$5], [
test "x$enable_web2c:$enable_$1" = xyes:yes && {
AC_FOREACH([Kpse_Lib], [$5], [  need_[]AS_TR_SH(Kpse_Lib)=yes
])}
])[]dnl m4_ifval
]) # KPSE_WITH_XMF

# KPSE_XMF_COND(PROG, BUILD-OR-NO, NOWIN-OR-NO, TEXT, REQUIRED-LIBS)
# ------------------------------------------------------------------
# Build conditionals and adjust with_x.
m4_define([KPSE_XMF_COND], [dnl
AM_CONDITIONAL(AS_TR_CPP($1), [test "x$enable_$1" != xno])[]dnl
test "x$enable_$1" = xyes && : ${with_x=yes}
m4_ifval([$3], [
AM_CONDITIONAL(AS_TR_CPP($1)[N], [test "x$enable_$1_nowin" != xno])[]dnl
])[]dnl m4_ifval
]) # KPSE_XMF_COND

# KPSE_WITH_MFWIN(WINDOW, DEFINE, TEXT)
# -------------------------------------
# Provide configure options --enable-*win.
m4_define([KPSE_WITH_MFWIN], [dnl
AC_ARG_ENABLE([$1win],
              AS_HELP_STRING([--enable-$1win],
                             [include $3 window support],
                             m4_eval(kpse_indent_26+2)))[]dnl
]) # KPSE_WITH_MFWIN

# KPSE_MFWIN_DEFINE(WINDOW, DEFINE, TEXT)
# ---------------------------------------
# Defines for enable-*win
m4_define([KPSE_MFWIN_DEFINE], [dnl
AS_IF([test "x$enable_$1win" = xyes],
      [AC_DEFINE([$2WIN], ,
                 [metafont: Define to include $3 window support.])])
]) # KPSE_MFWIN_DEFINE
