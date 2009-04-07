dnl ### Check for whether the C compiler does string concatenation

AC_DEFUN([XDVI_CC_CONCAT],
[AC_CACHE_CHECK([whether C compiler supports string concatenation], xdvi_cv_cc_concat,
[AC_TRY_COMPILE(
[#include <stdio.h>
], [puts("Testing" " string" " concatenation");
], [xdvi_cv_cc_concat=yes], [xdvi_cv_cc_concat=no])])
if test $xdvi_cv_cc_concat = yes; then
  AC_DEFINE([HAVE_CC_CONCAT], 1, [Define if your C compiler can do string concatenation])
fi])


