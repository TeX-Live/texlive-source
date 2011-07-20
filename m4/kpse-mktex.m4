# Private macros for the TeX Live (TL) tree.
# Copyright (C) 2011 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_MKTEX_PREPARE
# ------------------
# AC_DEFUN'ed so it can enforce inclusion of this file.
AC_DEFUN([KPSE_MKTEX_PREPARE], [])

# KPSE_WITH_MKTEX(PROG, YES-OR-NO, TEXT, STEM)
# --------------------------------------------
# Provide configure options --enable-mktex*-default and normalize result.
m4_define([KPSE_WITH_MKTEX],
[AC_ARG_ENABLE([$1-default],
               AS_HELP_STRING([--]m4_if([$2], [yes], [dis], [en])[able-$1-default],
                              m4_if([$2], [yes],
                                    [do not ])[run $1 if $3 missing],
                              m4_eval(kpse_indent_26+2)))[]dnl
]) # KPSE_WITH_MKTEX

# KPSE_MKTEX_DEFINE(PROG, YES-OR-NO, TEXT, STEM)
# ----------------------------------------------
# Normalize --enable-mktex*-default configure option and build defines.
m4_define([KPSE_MKTEX_DEFINE],
[AS_CASE([$enable_$1_default],
         [yes | no], ,
         [enable_$1_default=$2])
AS_IF([test "x$enable_$1_default" = xyes],
      [AC_DEFINE([MAKE_$4_BY_DEFAULT], 1,
                 [Define to 1 if you want to run $1 if $3 is missing,
                  and to 0 if you don't])],
      [AC_DEFINE([MAKE_$4_BY_DEFAULT], 0)])
]) # KPSE_MKTEX_DEFINE
