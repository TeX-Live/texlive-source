# cho-kpse.m4 - kpathsea features
# Copyright (C) 1998 - 2008 Jin-Hwan Cho <chofchof@ktug.or.kr>
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 1

# CHO_CHECK_KPSE_SUPPORT(PACKAGE-NAME)
# ------------------------------------
# Check for kpathsea features
AC_DEFUN([CHO_CHECK_KPSE_SUPPORT],
[AC_MSG_CHECKING([whether you have kpathsea headers and they know about the required file formats])
 AC_TRY_COMPILE(
  [#include <stdio.h>
#include <kpathsea/kpathsea.h>],
  [kpse_tex_ps_header_format;
   kpse_type1_format;
   kpse_vf_format;
   kpse_ofm_format;
   kpse_ovf_format;
   kpse_truetype_format;],
  [AC_MSG_RESULT(yes)
   AC_CHECK_LIB([kpathsea], [xbasename],
    [],
    [AC_MSG_ERROR([This version of $1 requires xbasename() in kpathsea library.])])],
  [AC_MSG_RESULT(no)
   AC_MSG_ERROR([This version of $1 requires that kpathsea and its headers be installed.
If you are sure they are installed and in a standard place, maybe you need a
newer version of kpathsea?  You also might try setting the environment
variable CPPFLAGS (or CFLAGS) with -I pointing to the directory containing
the file "tex-file.h"])])
])# CHO_CHECK_KPSE_SUPPORT

# CHO_CHECK_KPSE_TDS_VERSION
# --------------------------
# Check for enc, cmap, sfd formats
AC_DEFUN([CHO_CHECK_KPSE_TDS_VERSION],
[AC_MSG_CHECKING([whether kpathsea library supports TDS version 1.1 installation])
 AC_TRY_COMPILE(
  [#include <stdio.h>
#include <kpathsea/kpathsea.h>
#include <kpathsea/tex-file.h>],
  [kpse_enc_format;
   kpse_cmap_format;
   kpse_sfd_format;
   kpse_opentype_format;],
  [AC_MSG_RESULT(yes)
   AC_DEFINE([__TDS_VERSION__], [0x200406L], [Define if your libkpathsea supports enc formats])
   AM_CONDITIONAL([TDS_VERSION_11], [true])],
  [AC_MSG_RESULT(no)
   AC_DEFINE([__TDS_VERSION__], [0x200302L], [Define if your libkpathsea supports enc formats])
   AM_CONDITIONAL([TDS_VERSION_11], [false])])
])# CHO_CHECK_KPSE_TDS_VERSION

