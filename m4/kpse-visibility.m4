# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2013, 2014 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_COMPILER_VISIBILITY
# ------------------------
# Set up compiler flags for C and C++ to hide external symbols.
# This macro determines and substitutes VISIBILITY_CFLAGS for the C compiler
# and, if applicable, VISIBILITY_CXXFLAGS for the C++ compiler. To activate
# them a Makefile.am must use them, e.g., in AM_CFLAGS or AM_CXXFLAGS.
AC_DEFUN([KPSE_COMPILER_VISIBILITY],
[dnl arrange that AC_PROG_CC uses _KPSE_VISIBILITY_CFLAGS etc.
AC_PROVIDE_IFELSE([AC_PROG_CC],
                  [_KPSE_VISIBILITY_CFLAGS],
                  [m4_define([AC_PROG_CC],
                             m4_defn([AC_PROG_CC])[_KPSE_VISIBILITY_CFLAGS
])])
AC_PROVIDE_IFELSE([AC_PROG_CXX],
                  [_KPSE_VISIBILITY_CXXFLAGS],
                  [m4_define([AC_PROG_CXX],
                             m4_defn([AC_PROG_CXX])[_KPSE_VISIBILITY_CXXFLAGS
])])
]) # KPSE_COMPILER_VISIBILITY

_KPSE_VISIBILITY_CFLAGS
# ---------------------
# Internal subroutine.
# Set up compiler flags for C to hide external symbols.
AC_DEFUN([_KPSE_VISIBILITY_CFLAGS], [dnl
AC_REQUIRE([AC_PROG_CC])[]dnl
_KPSE_VISIBILITY_FLAGS([C], [cflags])
m4_define([_KPSE_VISIBILITY_CFLAGS], [])[]dnl
]) # _KPSE_VISIBILITY_CFLAGS

_KPSE_VISIBILITY_CXXFLAGS
# -----------------------
# Internal subroutine.
# Set up compiler flags for C++ to hide external symbols.
AC_DEFUN([_KPSE_VISIBILITY_CXXFLAGS], [dnl
AC_REQUIRE([AC_PROG_CXX])[]dnl
_KPSE_VISIBILITY_FLAGS([C++], [cxxflags])
m4_define([_KPSE_VISIBILITY_CXXFLAGS], [])[]dnl
]) # _KPSE_VISIBILITY_CFLAGS

_KPSE_VISIBILITY_FLAGS(LANG, TAG)
# -------------------------------
# Internal subroutine.
# Set up compiler flags for C or C++ to hide external symbols.
m4_define([_KPSE_VISIBILITY_FLAGS], [dnl
AC_CACHE_CHECK(AS_TR_CPP($2) [for $1 to hide external symbols],
               [kpse_cv_visibility_$2],
               [dnl
AC_LANG_PUSH([$1])
kpse_cv_visibility_$2=unknown
kpse_save_flags=$AS_TR_CPP($2)
AC_LANG_CONFTEST([AC_LANG_SOURCE([[#include <stdio.h>
                                   extern void foo(void);
                                   void foo(void){printf("foo\n");}]])])
# FIXME: Add tests for non-GNU compilers
for kpse_flag in '-fvisibility=hidden -fvisibility-inlines-hidden' '-fvisibility=hidden'; do
  AS_TR_CPP($2)="$kpse_save_flags -Werror $kpse_flag"
  AC_COMPILE_IFELSE([], [kpse_cv_visibility_$2=$kpse_flag; break])
done
AC_LANG_POP([$1])
])
AS_TR_CPP($2)=$kpse_save_flags
AS_CASE([$kpse_cv_visibility_$2],
        [unknown], [],
        [AC_SUBST([VISIBILITY_]AS_TR_CPP($2), [$kpse_cv_visibility_$2])])
]) # _KPSE_VISIBILITY_FLAGS

