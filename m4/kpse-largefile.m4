# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2013 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_LARGEFILE(MAKE-VAR,[EXTRA-DEFINE])
# ---------------------------------------
# Append -D's required for largefile support to MAKE-VAR.
AC_DEFUN([KPSE_LARGEFILE], [dnl
AC_REQUIRE([AC_SYS_LARGEFILE])[]dnl
AC_REQUIRE([AC_FUNC_FSEEKO])[]dnl
if test "x$enable_largefile" != xno; then
  AS_CASE([$ac_cv_sys_file_offset_bits],
          [no], [],
          [unknown], [AS_CASE([$ac_cv_sys_large_files],
                              [no | unknown], [],
                              [$1="$$1 -D_LARGE_FILES"])],
          [$1="$$1 m4_ifval([$2], [-D$2 ])-D_FILE_OFFSET_BITS=64"])
fi
AS_CASE([$ac_cv_sys_largefile_source],
        [no | unknown], [],
        [$1="$$1 -D_LARGEFILE_SOURCE"])
]) # KPSE_LARGEFILE
