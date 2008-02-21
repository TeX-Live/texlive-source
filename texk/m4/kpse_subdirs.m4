# Private macros for the kpathsea library.
# Copyright (C) 2008 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 3

# KPSE_WITH_SUBDIRS(PKGLIST)
# -----------------------
# Specify optional packages to be built.  For each PKG in PKGLIST expand
#	AC_ARG_WITH([PKG], [  --without-PKG           do not build the PKG package])
# to produce standardized help messages.
AC_DEFUN([KPSE_WITH_SUBDIRS],
[AC_FOREACH([AC_Pkg], [$1],
  [AC_ARG_WITH(AC_Pkg,
    AS_HELP_STRING([--without-[]]AC_Pkg, [do not build the ]AC_Pkg[ package]))])
])

# KPSE_CONFIG_SUBDIRS(PKGLIST, [DIR])
# -----------------------------------
# Specify optional subdirectories to be configured.
# First initialize the shell variable ESUBDIRS (to empty).  Then for
# each PKG in PKGLIST, test if [DIR/]PKG exists as subdirectory of
# $srcdir and the value of the shell variable with_PKG is not "no";
# if so append this subdirectory to the list of subdirectories to be
# configured and to the shell variable ESUBDIRS.
# Finally ESUBDIRS is AC_SUBSTed.
#
# Examples: KPSE_CONFIG_SUBDIRS([dialog lcdf-typetools], [utils]) is
# roughly equivalent to the shell code
#	ESUBDIRS=
#	  test -d utils || mkdir utils
#	  for pkg in dialog lcdf-typetools; do
#	    if test -d $srcdir/utils/$pkg; then
#	      if eval "test \"`echo '$with_'${pkg}|sed 's/-/_/g'`\" != no"; then
#	        ESUBDIRS="$ESUBDIRS utils/$pkg"
#	        AC_CONFIG_SUBDIRS([utils/$pkg])
#	      fi
#	    fi
#	  done
#	AC_SUBST(ESUBDIRS)
# whereas KPSE_CONFIG_SUBDIRS([afm2pl bibtex8]) corresponds to
#	ESUBDIRS=
#	for pkg in afm2pl bibtex8; do
#	  if test -d $srcdir/utils/$pkg; then
#	    if eval "test \"x`echo '$with_'${pkg}`\" != xno"; then
#	      ESUBDIRS="$ESUBDIRS $pkg"
#	      AC_CONFIG_SUBDIRS([$pkg])
#	    fi
#	  fi
#	done
#	AC_SUBST(ESUBDIRS)
#
# The main difference is that these macros replace the shell loop by
# an M4 loop and therefore M4 variables (i.e., shell literals) are
# passed as arguments to AC_CONFIG_SUBDIRS, as required by modern
# versions of Autoconf and Automake.
AC_DEFUN([KPSE_CONFIG_SUBDIRS],
[ESUBDIRS=
m4_ifval([$2], [test -d $2 || mkdir $2])
AC_FOREACH([AC_Pkg], [$1],
  [_KPSE_CONFIG_SUBDIR(m4_ifval([$2], [$2/])AC_Pkg,
     [with_]m4_bpatsubst(AC_Pkg, [-], [_]))])
AC_SUBST(ESUBDIRS)dnl
])

# _KPSE_CONFIG_SUBDIR(DIR, WITH_VAR)
# ----------------------------------
# See above; test if $srcdir/DIR exists and $WITH_VAR != no; if so
# append DIR to the list of subdirectories to be configured.
m4_define([_KPSE_CONFIG_SUBDIR],
[if test -d $srcdir/$1; then
  if test "x$[]$2" != xno; then
    ESUBDIRS="$ESUBDIRS $1"
    AC_CONFIG_SUBDIRS([$1])dnl
  fi
fi
])

