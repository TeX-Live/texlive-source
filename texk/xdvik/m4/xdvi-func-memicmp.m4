dnl ### Check for memicmp(), which some installations have in string.h
AC_DEFUN([XDVI_FUNC_MEMICMP],
[AC_CACHE_CHECK([for memicmp], xdvi_cv_memicmp,
[AC_TRY_LINK(
[#include <string.h>
], [(void)memicmp((char *)NULL, (char *)NULL, 0);],
xdvi_cv_memicmp=yes, xdvi_cv_memicmp=no)])
if test $xdvi_cv_memicmp = yes; then
  AC_DEFINE([HAVE_MEMICMP], 1, [Define if the memicmp() function is in <string.h>])
fi])

