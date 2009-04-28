# Public macros for the TeX Live (TL) tree.
# Copyright (C) 1997 Karl Berry <karl@cs.umb.edu>
# Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
#     with help from Taco Hoekwater <taco@luatex.org>
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 0

# KPSE_CHECK_SOCKET_LIBS
# ----------------------
# Set ac_cv_search_connect as AC_SEARCH_LIBS([connect], [LIB...])
# would do.
# -lsocket is needed on Solaris, at least.  Maybe -lnsl on SCO, too?
# See AC_PATH_XTRA.
# For WIN32 systems we need -lwsock32 but AC_SEARCH_LIBS would fail.
AC_DEFUN([KPSE_CHECK_SOCKET_LIBS],
[AC_REQUIRE([KPSE_CHECK_WIN32])
AS_IF([test "x$kpse_cv_have_win32" = xno],
      [kpse_save_LIBS=$LIBS
       AC_SEARCH_LIBS([connect], [socket nsl])
       LIBS=$kpse_save_LIBS],
      [AC_CHECK_LIB([wsock32], [main],
                    [ac_cv_search_connect=-lwsock32],
                    [ac_cv_search_connect=no])])
]) # KPSE_CHECK_SOCKET_LIBS
