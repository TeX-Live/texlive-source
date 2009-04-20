# Public macros for the teTeX / TeX Live (TL) tree.
# Copyright (C) 1998 - 2008 Jin-Hwan Cho <chofchof@ktug.or.kr>
# Copyright (C) 2008 - 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 2

# CHO_CHECK_KPSE_SUPPORT(PACKAGE-NAME)
# ------------------------------------
# Check for required file formats and xbasename.
# We need tex-ps_header, type1, vf, ofm, ovf, and truetype;
# of these truetype was introduced last (Dec 1997).
AC_DEFUN([CHO_CHECK_KPSE_SUPPORT],
[KPSE_CHECK_KPSE_FORMAT([truetype], ,
                        [KPSE_MSG_ERROR([$1],
                                        [Required file formats not found in Kpathsea header files.

This version of $1 requires that kpathsea and its headers be available.
If you are sure they are installed and in a standard place, maybe you need a
newer version of kpathsea?  You also might try setting the environment
variable CPPFLAGS (or CFLAGS) with -I pointing to the directory containing
the file "kpathsea/kpathsea.h"

])])
KPSE_CHECK_XBASENAME([],
                     [KPSE_MSG_ERROR([$1],
                                     [This version of $1 requires xbasename in libkpathsea.])])
]) # CHO_CHECK_KPSE_SUPPORT

# CHO_CHECK_KPSE_TDS_VERSION
# --------------------------
# Check for TDS version 1.1 support.
# We need cmap, enc, opentype, and sfd formats (Dec 2003).
AC_DEFUN([CHO_CHECK_KPSE_TDS_VERSION],
[KPSE_CHECK_KPSE_FORMAT([opentype],
                        [tds_version_11=yes
                         AC_DEFINE([__TDS_VERSION__], [0x200406L],
                                   [Define as 0x200406L if your libkpathsea supports enc formats,
                                    or as 0x200302L otherwise.])],
                        [tds_version_11=no
                         AC_DEFINE([__TDS_VERSION__], [0x200302L])])
AC_MSG_CHECKING([whether libkpathsea supports TDS version 1.1 installation])
AC_MSG_RESULT([$tds_version_11])
AM_CONDITIONAL([TDS_VERSION_11], [test "x$tds_version_11" = xyes])
]) # CHO_CHECK_KPSE_TDS_VERSION

