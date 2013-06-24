# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2013 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# CAIRO_FEATURES(FEATURE1, FEATURE2, ...)
# ---------------------------------------
# Where each feature has the form '[1-or-o], [text]'. 
dnl
AC_DEFUN([CAIRO_FEATURES], [dnl
m4_foreach([Cairo_Feature], [$1],
           [m4_ifset([Cairo_Feature],
                     [_CAIRO_FEATURE(Cairo_Feature)])])[]dnl
]) # CAIRO_FEATURES

# _CAIRO_FEATURE(COND, TEXT)
# --------------------------
m4_define([_CAIRO_FEATURE], [dnl
if test "x$1" = x1; then
  AC_DEFINE([CAIRO_HAS_]AS_TR_CPP($2), [1],
            [Define to 1 if $2 enabled.])
fi
AM_CONDITIONAL([CAIRO_HAS_]AS_TR_CPP($2), [test "x$1" = x1])
]) # _CAIRO_FEATURE

