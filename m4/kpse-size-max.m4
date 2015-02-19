# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2014 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_CHECK_SIZE_MAX
# -------------------
# Define SIZE_MAX if necessary, e.g., for Solaris 9.
AC_DEFUN([KPSE_CHECK_SIZE_MAX], [dnl
AC_CHECK_DECL([SIZE_MAX], , [dnl
AC_DEFINE([SIZE_MAX], [((size_t)-1)],
          [Define to `((size_t)-1)' if <stdint.h> does not define it.])])[]dnl
]) # KPSE_CHECK_SIZE_MAX
