# Public macros for the TeX Live (TL) subdirectory utils/xindy/.
# Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 0

# TL_MSG_ERROR(ERROR, [EXIT-STATUS = 1])
# --------------------------------------
# Same as AC_MSG_ERROR(ERROR, EXIT-STATUS), except when configuring with
# --disable-build (in the TL tree when building xindy has been disabled).
#
# The new (2009) TL build system requires all directories to be configured
# for the benefit of 'dist*' Make targets.  When building xindy has been
# disabled, configuring must not fail because, e.g., clisp is missing.
AC_DEFUN([TL_MSG_ERROR],
[AS_IF([test "x$enable_build" = xno],
       [AC_MSG_WARN([building xindy has been disabled and would fail because
$1])],
       [AC_MSG_ERROR($@)])
]) # TL_MSG_ERROR
