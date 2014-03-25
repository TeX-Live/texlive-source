# Private macros for the kpathsea library.
# Copyright (C) 1995-2014 Karl Berry <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_CROSS_PATH_PROG(RESULT, CROSS_PROG, NORMAL_PROG)
# -----------------------------------------------------
# Find a program when cross-compiling, or use a default when not.
# RESULT = variable which records the outcome
# CROSS_PROG = program to look for when cross-compiling
# NORMAL_PROG = program to use when not cross-compiling
# Example: KPSE_CROSS_PATH_PROG([TANGLE], [tangle], [./tangle]) sets
# 'TANGLE' to the program 'tangle' found in PATH when cross-compiling,
# and to './tangle' if not.
AC_DEFUN([KPSE_CROSS_PATH_PROG], [dnl
AS_IF([test "x$cross_compiling" = xyes], [dnl
AC_PATH_PROG([$1], [$2])
AS_IF([test -z "${$1}"],
 [AC_MSG_ERROR([$2 was not found but is required when cross-compiling.
  Install $2 or set \$$1 to the full pathname.])])],
[$1=$3])
]) # KPSE_CROSS_PATH_PROG

