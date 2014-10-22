# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2014 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_DLL_NAME([M32], [M64], [C32], [C64])
# -----------------------------------------
# Specify the DLL name for 32- and/or 64-bit MinGW and/or Cywin.
AC_DEFUN([KPSE_DLL_NAME], [dnl
AC_PROVIDE_IFELSE([LT_INIT], ,
                  [m4_fatal([$0: requires libtool])])[]dnl
AC_PROVIDE_IFELSE([LT_OUTPUT],
                  [m4_fatal([$0: too late])])[]dnl
AC_CHECK_SIZEOF([void *])
AS_CASE([$host:$ac_cv_sizeof_void_p],
        [*-*-mingw*:4], [kpse_dll_name=$1],
        [*-*-mingw*:8], [kpse_dll_name=$2],
        [*-*-cygwin:4], [kpse_dll_name=$3],
        [*-*-cygwin:8], [kpse_dll_name=$4],
        [kpse_dll_name=])
_LT_CONFIG_SAVE_COMMANDS([## $0: Specify the DLL name
if test "x$kpse_dll_name" != x; then
  $SED '/^soname_spec=/a\
## $0: Specify the DLL name\
soname_spec='$kpse_dll_name'.dll\
' "$ofile" >"$cfgfile"
  mv "$cfgfile" "$ofile" ||
    (rm -f "$ofile" && cp "$cfgfile" "$ofile" && rm -f "$cfgfile")
  chmod +x "$ofile"
fi], [kpse_dll_name='$kpse_dll_name'])
]) # KPSE_DLL_NAME

