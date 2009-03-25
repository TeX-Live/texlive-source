# gs-device.m4 - Macro to check for GS devices.
## Copyright (C) 20xx Some One <someone@somewhere.net>
## Some affiliation
##
## Temporary notice:
## I extracted this macro from aclocal.m4 in the dvipng-1.9 distribution
## and renamed it in order not to violate the Autoconf (AC_) namespace.
## 26 Mar 2008, Peter Breitenlohner <tex-live@tug.org>
##
## This file is free software; the copyright holder gives unlimited
## permission to copy and/or distribute it, with or without
## modifications, as long as this notice is preserved.

# serial 1

# xx_GS_HAS_DEVICE(DEVICE, ACTION-IF-FAILED)
# ------------------------------------------
# Check if GS has the device DEVICE and execute the shell code
# ACTION-IF-FAILED if not.
AC_DEFUN([xx_GS_HAS_DEVICE],
 [AC_MSG_CHECKING([whether $GS has the $1 device])
  if $GS -h | grep $1 >/dev/null; then
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
    $2
  fi
])# xx_GS_HAS_DEVICE

