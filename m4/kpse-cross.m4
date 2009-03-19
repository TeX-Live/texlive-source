# Private macros for the kpathsea library.
# Copyright (C) 1995 - 2009 Karl Berry <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 3

# KPSE_CROSS_PATH_PROG(RESULT, CROSS_PROG, NORMAL_PROG)
# -----------------------------------------------------
# Find a program when cross-compiling, or use a default when not.
# RESULT = variable which records the outcome
# CROSS_PROG = program to look for when cross-compiling
# NORMAL_PROG = program to use when not cross-compiling
# Example: KPSE_CROSS_PATH_PROG([TANGLE], [tangle], [./tangle]) sets
# 'TANGLE' to the program 'tangle' found in PATH when cross-compiling,
# and to './tangle' if not.
AC_DEFUN([KPSE_CROSS_PATH_PROG],
  [if test "x$cross_compiling" = xyes; then
AC_PATH_PROG([$1], [$2])
if test -z "${$1}"; then
  AC_MSG_ERROR([$2 was not found but is required when cross-compiling.
  Install $2 or set \$$1 to the full pathname.])
fi
else
  $1=$3
fi
]) # KPSE_CROSS_PATH_PROG

# KPSE_CROSS_BUILD_VAR(STEM, PROG)
# --------------------------------
# Set BUILD$1 to $2 when cross-compiling, to $($1) if not.
# Example: KPSE_CROSS_BUILD_VAR([CC], [cc]) sets 'BUILDCC' to 'cc'
# if cross-compiling, and to '$(CC)' if not.
AC_DEFUN([KPSE_CROSS_BUILD_VAR],
  [if test "x$cross_compiling" = xyes; then
  if test -z "${BUILD$1}"; then
    BUILD$1='$2'
  fi
  AC_MSG_RESULT([setting \$BUILD$1 to ${BUILD$1}])
else
  if test -n "${BUILD$1}"; then
    AC_MSG_WARN([\$BUILD$1 set but not cross-compiling.])
  fi
  BUILD$1='$($1)'
fi
AC_SUBST([BUILD$1])
]) # KPSE_CROSS_BUILD_VAR

