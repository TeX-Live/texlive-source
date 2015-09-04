# Private macros for the TeX Live (TL) tree.
# Copyright (C) 2011-2015 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_LIB_VERSION(NAME)
# ----------------------
# Split NAME_version into NAME_major, NAME_minor, and NAME_micro
# (ignoring an optional suffix starting with a non-digit).
AC_DEFUN([KPSE_LIB_VERSION],
[m4_bpatsubst($1_version, [^\([0-9]+\).\([0-9]+\).\([0-9]+\).*$],
              [m4_define([$1_major], [\1])m4_define([$1_minor], [\2])m4_define([$1_micro], [\3])])[]dnl
]) # KPSE_LIB_VERSION

# KPSE_LT_VERSION(NAME)
# ---------------------
# Split NAME_version as above and define NAME_LT_VERSINFO.
AC_DEFUN([KPSE_LT_VERSION],
[KPSE_LIB_VERSION([$1])
AC_SUBST(AS_TR_CPP($1)[_LT_VERSINFO],
         [m4_eval($1_major+$1_minor):$1_micro:$1_minor])[]dnl
]) # KPSE_LT_VERSION

