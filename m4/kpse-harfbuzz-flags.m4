# $Id$
# Public macros for the TeX Live (TL) tree.
# Copyright 2015-2019 Karl Berry <tex-live@tug.org>
# Copyright 2012-2015 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_HARFBUZZ_FLAGS
# -------------------
# Provide the configure options '--with-system-harfbuzz' (if in the TL tree).
# 
# Set the make variables HARFBUZZ_INCLUDES and HARFBUZZ_LIBS to the
# CPPFLAGS and LIBS required for the `-lharfbuzz' library in
# libs/harfbuzz/ of the TL tree.
# 
# We used to require icu here, but LuaTeX (2019) wants only harfbuzz,
# not icu, and everything in our tree that wants icu requires it
# explicitly. So no need.
# 
AC_DEFUN([KPSE_HARFBUZZ_FLAGS], [dnl
AC_REQUIRE([KPSE_GRAPHITE2_FLAGS])[]dnl
_KPSE_LIB_FLAGS([harfbuzz], [harfbuzz], [],
                [-IBLD/libs/harfbuzz/include],
                [BLD/libs/harfbuzz/libharfbuzz.a], [], [],
                [${top_builddir}/../../libs/harfbuzz/include/hb.h])[]dnl
]) # KPSE_HARFBUZZ_FLAGS

# KPSE_HARFBUZZ_OPTIONS([WITH-SYSTEM])
# ------------------------------------
AC_DEFUN([KPSE_HARFBUZZ_OPTIONS], [dnl
_KPSE_LIB_OPTIONS([harfbuzz], [$1], [pkg-config])])

# KPSE_HARFBUZZ_SYSTEM_FLAGS
# --------------------------
# We used to ask for harfbuzz-icu here, that is, we called
# pkg-config harfbuzz-icu instead of just harfbuzz. But we never
# actually used the ICU support builtin to harfbuzz; Jonathan Kew had a
# theory many years ago (private email, 21 October 2009) to switch xetex
# to using that, and thus eliminate the need for xetex to link with icu,
# but it never came to fruition. So, since LuaTeX (2019) wants harfbuzz
# without icu, back to just harfbuzz
AC_DEFUN([KPSE_HARFBUZZ_SYSTEM_FLAGS], [dnl
_KPSE_PKG_CONFIG_FLAGS([harfbuzz], [harfbuzz])])
