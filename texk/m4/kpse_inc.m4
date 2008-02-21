# Private macros for the kpathsea library.
# Copyright (C) 1995 - 2008 Karl Berry, Peter Breitenlohner <tex-live@tug.org>
# Free Software Foundation, Inc.
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 2

# KPSE_CONFIG_FILES(FILE..., [COMMANDS], [INIT-CMDS])
# ---------------------------------------------------
# Specify output files that are configured with AC_SUBST.
# In a first step, each input file is converted into an auxiliary file
# with all `kpse_include' directives resolved.
# In a second step each auxiliary file is passed to AC_CONFIG_FILES
# with the (optional) COMMANDS to be run after config.status creates
# the output file.
#
# Each FILE is specified as OUTPUT[:[INPUT][:[AUX]]], i.e., the syntax is
# similar to that for AC_CONFIG_FILES but the meaning is quite different,
# there is one unique INPUT for each OUTPUT.  If omitted, INPUT defaults
# to OUTPUT.in and AUX to OUTPUT.aux.
AC_DEFUN([KPSE_CONFIG_FILES],
[AC_FOREACH([AC_File], [$1],
  [_KPSE_CONFIG_FILE([$2], m4_bpatsubst(AC_File, [:], [,]))])dnl
_AC_CONFIG_COMMANDS_INIT([$3])dnl
])

# _KPSE_CONFIG_FILE([COMMANDS], OUTPUT, [INPUT], [AUX])
# -----------------------------------
# See above.
m4_define([_KPSE_CONFIG_FILE],
[AC_CONFIG_FILES(m4_ifval([$4], [$4], [$2].aux):m4_ifval([$3], [$3], [$2].in),
  [# Discard the file just generated and first resolve kpse_include's
  _KPSE_INCLUDE(m4_ifval([$4], [$4], [$2].aux), m4_ifval([$3], [$3], [$2].in))])dnl
AC_CONFIG_FILES([$2]:m4_ifval([$4], [$4], [$2].aux), [$1])dnl
])

# _KPSE_INCLUDE(DEST, SOURCE)
# ---------------------------
# Create DEST from SOURCE with all occurrences of
#      kpse_include INCLUDE_FILE
# replaced by the contents of INCLUDE_FILE if that file exists.
# Note: this doesn't work recursively, one level of inclusion only!
m4_define([_KPSE_INCLUDE],
[kpse_input=`case $2 in
  [[\\/$]]*) # Absolute
    test -f "$2" || AC_MSG_ERROR([cannot find input file: $2])
    echo "$2";;
  *) # Relative
    if test -f "$2"; then
      # Build tree
      echo "$2"
    elif test -f "$srcdir/$2"; then
      # Source tree
      echo "$srcdir/$2"
    else
      # /dev/null tree
      AC_MSG_ERROR([cannot find input file: $2])
    fi;;
  esac` || AS_EXIT([1])
# Replace lines of the form "kpse_include foo" with the contents of foo:
# first, from the kpse_include lines construct a list of file names.
# From that list, we construct a second list of those files that exist.
# Then we construct a list of sed commands for including those files,
# and a sed command that removes the kpse_include lines. We don't attempt
# to read non-existent files because some (buggy) versions of sed choke
# on this. Have to use sed because old (Ultrix, SunOs) awk does not support
# getline or system. (Anyway, configure scripts aren't supposed to use awk.)
# Can't use only one -e and commands {dr foo} because foo has to be last.
# Be careful, because the filename may contain /.
# Be careful with whitespace; we need to use echo.
kpse_file_substs=`sed -n '/^kpse_include/s%[[ 	]]*\(.*\)%\1%p' "$kpse_input"`
if test x"$kpse_file_substs" = x; then
  AC_MSG_WARN([No need to use \`[KPSE_CONFIG_FILES]' for input \`$2'
               without any \`kpse_include' directives])
fi
# Create the sed command line ...
# Input lines containing "@configure_input@" are output twice,
# first with our modifications and then unmodified.
kpse_substs="sed -e '/@configure_input@/ { h; \
  s,@configure_input@,$1.  Generated from `echo $2 |
  sed 's,.*/,,'` by configure.,; G; }'"
for kpse_inc in $kpse_file_substs; do
  kpse_read=`case $kpse_inc in
    [[\\/$]]*) # Absolute
      if test -f "$kpse_inc"; then
        echo "$kpse_inc"
      fi;;
    *) # Relative
      if test -f "$kpse_inc"; then
        # Build tree
        echo "$kpse_inc"
      elif test -f "$srcdir/$kpse_inc"; then
        # Source tree
        echo "$srcdir/$kpse_inc"
      fi;;
  esac`
  if test -n "$kpse_read"; then
    kpse_mung=`echo $kpse_inc | sed 's,/,%,g'`
    kpse_substs="$kpse_substs -e '/^kpse_include $kpse_mung$/r $kpse_read'"
  fi
done
# ... and fix the whitespace and escaped slashes.
kpse_substs=`echo "$kpse_substs" | sed -e 's,%,\\\/,g' \
  -e 's/kpse_include /kpse_include[[ 	]]*/g'`
dnl The parens around the eval prevent an "illegal io" in Ultrix sh.
(eval $kpse_substs -e '/^kpse_include/d' "$kpse_input") > $1
])

