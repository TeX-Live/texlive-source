# Private macros for the kpathsea library.
# Copyright (C) 2003 - 2009 Karl Berry <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 1

# KPSE_PROG_LEX()
# ---------------
# Like AC_PROG_LEX, with the flag '-l' for flex.
AC_DEFUN([KPSE_PROG_LEX],
[AC_PROG_LEX
# Work around a problem with Flex Version 2.5.31 which needs -l flag.
# Since all recent versions of flex support -l, don't check for the
# specific version, but check that at least "--version" is supported.
# We also want to catch LEX=/some/where/flex, so:
case $LEX in
  *flex) $LEX --version >/dev/null 2>&1 && LEX="$LEX -l" ;;
esac
]) # KPSE_PROG_LEX

