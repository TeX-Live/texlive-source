# Autoconf macros for xdvik.
# Copyright (C) 2013 Paul Vojta <xdvi-core@lists.sourceforge.net>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# XDVI_GS_LIB_PATH
# ----------------
# Find the path to use for GS_LIB.

dnl ### Determine the path to use for GS_LIB.

AC_DEFUN([XDVI_GS_LIB_PATH],
[AC_ARG_WITH(default-gs-lib-path, [  --with-default-gs-lib-path=PATH
                          set default path for finding font aliases to PATH],
[AC_DEFINE_UNQUOTED([DEFAULT_GS_LIB_PATH], "$withval",
        [Define to set the default path for ghostscript-style font searching.])
],
[AC_CACHE_CHECK([for the path to be used for Ghostscript searches],
xdvi_cv_gs_lib_path,
[if gs -h >/dev/null 2>&1; then
  ac_tmp="`gs -h \
    | sed \
      -e '1,/Search path:/d' \
      -e '/For more information/,$d' \
      -e '/Initialization files are compiled/d' \
      -e 's/$/\/\/\//' \
      -e 's/^   //' \
    | tr '\n' '/'`"
  # Solaris 9 sed doesn't handle incomplete lines at eof
  xdvi_cv_gs_lib_path=`echo "$ac_tmp" \
    | sed -e 's/\/\/\/\// /g' -e 's/ *$//' -e 's/ : /:/g'`
else
  xdvi_cv_gs_lib_path=/usr/local/share/ghostscript/fonts:/usr/local/lib/ghostscript/fonts:/usr/share/ghostscript/fonts:/var/lib/ghostscript/fonts:/usr/share/cups/fonts:/usr/share/fonts
  AC_MSG_WARN(Could not determine Ghostscript search path; using $xdvi_cv_gs_lib_path)
fi])
AC_DEFINE_UNQUOTED([DEFAULT_GS_LIB_PATH], "$xdvi_cv_gs_lib_path")
])])
