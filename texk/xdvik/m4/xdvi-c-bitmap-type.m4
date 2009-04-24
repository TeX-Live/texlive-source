# Autoconf macros for xdvik.
# Copyright (C) 1999 - 2009 Paul Vojta <xdvi-core@lists.sourceforge.net>
# Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# XDVI_C_BITMAP_TYPE
# ------------------
# Determine integer type to use for bitmaps.
# Uses AC_CHECK_SIZEOF(TYPE) and thus works when cross compiling.
AC_DEFUN([XDVI_C_BITMAP_TYPE],
[AC_CHECK_SIZEOF([unsigned long])[]dnl
 AC_CHECK_SIZEOF([unsigned int])[]dnl
 AC_CHECK_SIZEOF([unsigned short])[]dnl
 AC_CHECK_SIZEOF([unsigned char])[]dnl
AC_MSG_CHECKING([for integer type to use in bitmaps])
AC_CACHE_VAL([xdvi_cv_bitmap_type],
[AS_IF([(test $ac_cv_sizeof_unsigned_long = 4 || test $ac_cv_sizeof_unsigned_long = 2) \
        && test $ac_cv_sizeof_unsigned_long != $ac_cv_sizeof_unsigned_int],
         [xdvi_cv_bitmap_type="BMTYPE=long BMBYTES=$ac_cv_sizeof_unsigned_long"],
       [test $ac_cv_sizeof_unsigned_int = 4 || test $ac_cv_sizeof_unsigned_int = 2],
         [xdvi_cv_bitmap_type="BMTYPE=int BMBYTES=$ac_cv_sizeof_unsigned_int"],
       [test $ac_cv_sizeof_unsigned_short = 4 || test $ac_cv_sizeof_unsigned_short = 2],
         [xdvi_cv_bitmap_type="BMTYPE=short BMBYTES=$ac_cv_sizeof_unsigned_short"],
         [xdvi_cv_bitmap_type="BMTYPE=char BMBYTES=$ac_cv_sizeof_unsigned_cher"])])
eval "$xdvi_cv_bitmap_type"
AC_DEFINE_UNQUOTED([BMTYPE], [$BMTYPE],
                   [Define to determine the integer type to be used in bitmaps.
                    The type used will be "unsigned BMTYPE".])
AC_DEFINE_UNQUOTED([BMBYTES], [$BMBYTES], [Define to the length (in bytes) of type BMTYPE.])
AC_MSG_RESULT([unsigned $BMTYPE, size = $BMBYTES])
]) # XDVI_C_BITMAP_TYPE
