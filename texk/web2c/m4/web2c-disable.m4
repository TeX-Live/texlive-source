# Autoconf macros for web2c.
# Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# WEB2C_DISABLE(PROGRAM, PROBLEM)
# ------------------------------
# Disable building PROGRAM due to PROBLEM, usually missing libraries.
# Terminate, via KPSE_MSG_WARN, if `--disable-missing' was given.
AC_DEFUN([WEB2C_DISABLE],
[AS_IF([test "x$enable_$1" = xyes],
       [KPSE_MSG_WARN([Sorry, $2: disabling $1])
                       enable_$1=no])
]) # WEB2C_DISABLE

