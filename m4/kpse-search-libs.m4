# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2014 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_SEARCH_LIBS(VARIABLE, FUNCTION, SEARCH-LIBS, [ACTION-IF-NOT-FOUND])
# ------------------------------------------------------------------------
# Search for a library defining FUNCTION, if it's not already available and
# set the output variable VARIABLE to the required lib.
# If FUNCTION is not found, the shell code ACTION-IF-NOT-FOUND if given or
# AC_MSG_ERROR with a suitable text is executed.
AC_DEFUN([KPSE_SEARCH_LIBS], [dnl
kpse_search_save_LIBS=$LIBS
AC_SEARCH_LIBS([$2], [$3])
LIBS=$kpse_search_save_LIBS
AS_CASE([$ac_cv_search_$2],
        ["none required"], [],
        [no], [m4_ifval([$4], [$4], [AC_MSG_ERROR([Sorry, did not find $2()])])],
        [$1=$ac_cv_search_$2])
AC_SUBST([$1])[]dnl
]) # KPSE_SEARCH_LIBS
