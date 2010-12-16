# gs-device.m4 - Macros to check for GS devices.
## Copyright (C) 20xx Some One <someone@somewhere.net>
## Some affiliation
##
## Notice:
## I extracted these macros from aclocal.m4 in the dvipng-1.12 distribution
## and renamed one in order not to violate the Autoconf (AC_) namespace.
## 26 Mar 2008, Peter Breitenlohner <tex-live@tug.org>
##
## This file is free software; the copyright holder gives unlimited
## permission to copy and/or distribute it, with or without
## modifications, as long as this notice is preserved.

# serial 1

# GS_CHECK_DEVICES
# ----------------
# Check GS (ghostscript) devices.
AC_DEFUN([GS_CHECK_DEVICES],
[GS_WARN=
_GS_HAS_DEVICE([pngalpha],
               [GS_WARN="Your EPS inclusions will be cropped to the
        boundingbox, and rendered on an opaque background.
        Upgrade GhostScript to avoid this."
        _GS_HAS_DEVICE([png16m],
                       [GS_WARN="Your EPS inclusions may not work.
        Upgrade/install GhostScript to avoid this."])])
if test -n "$GS_WARN"; then
  AC_MSG_WARN([$GS_WARN])
fi
]) # GS_CHECK_DEVICES

# _GS_HAS_DEVICE(DEVICE, ACTION-IF-FAILED)
# ----------------------------------------
# Internal subroutine.  Check if GS has the device DEVICE and
# execute the shell code ACTION-IF-FAILED if not.
m4_define([_GS_HAS_DEVICE],
[AC_MSG_CHECKING([whether $GS has the $1 device])
if $GS -h | grep $1 >/dev/null; then
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])
  $2
fi
])# _GS_HAS_DEVICE

