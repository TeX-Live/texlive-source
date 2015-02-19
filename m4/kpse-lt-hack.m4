# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2009, 2010 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_ENABLE_LT_HACK
# -------------------
# Provide the configure option '--enable-libtool-hack'.
AC_DEFUN([KPSE_ENABLE_LT_HACK],
[AC_ARG_ENABLE([libtool-hack],
               AS_HELP_STRING([--enable-libtool-hack],
                              [ignore libtool dependency_libs]))[]dnl
]) # KPSE_ENABLE_LT_HACK

# KPSE_LT_HACK()
# --------------
# Try to ignore libtool dependency_libs when possible, e.g., on systems
# using ELF shared libraries.
AC_DEFUN([KPSE_LT_HACK],
[AC_REQUIRE([KPSE_ENABLE_LT_HACK])[]dnl
AC_PROVIDE_IFELSE([LT_INIT], ,
                  [m4_fatal([$0: requires libtool])])[]dnl
AC_PROVIDE_IFELSE([LT_OUTPUT],
                  [m4_fatal([$0: too late])])[]dnl
_LT_CONFIG_SAVE_COMMANDS([## $0: Prevent libtool from linking dependency_libs
if test "x$enable_libtool_hack" = xyes; then
  $SED '/# Convert "-framework/i\
	## $0: ignore dependency_libs\
	test "X$installed" = Xyes && dependency_libs=\
' "$ofile" >"$cfgfile"
  mv "$cfgfile" "$ofile" ||
    (rm -f "$ofile" && cp "$cfgfile" "$ofile" && rm -f "$cfgfile")
  chmod +x "$ofile"
fi], [enable_libtool_hack='$enable_libtool_hack'])
]) # KPSE_LT_HACK

