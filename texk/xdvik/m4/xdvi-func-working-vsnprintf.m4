dnl ### Check for a working implementation of (v)snprintf()
dnl ### which should either return a negative result or the size actually needed
dnl ### (latter is C99 standard).
AC_DEFUN([XDVI_FUNC_WORKING_VSNPRINTF],
[AC_CACHE_CHECK([for a working implementation of (v)snprintf()],
xdvi_cv_func_good_vsnprintf,
[AC_TRY_RUN(
[
#ifdef IRIX
#define _XOPEN_SOURCE 500
#endif
#include <stdio.h>
int main()
{
  char s[2];
  int retval = snprintf(s, 2, "test");
  if (retval > -1 && retval != 4) /* neither negative nor desired size, not OK */
    return 1;
  return 0;
}],
xdvi_cv_func_good_vsnprintf=yes,
xdvi_cv_func_good_vsnprintf=no,
# safe value for cross-compiling
xdvi_cv_func_good_vsnprintf=no)])
if test $xdvi_cv_func_good_vsnprintf = yes; then
  AC_DEFINE([HAVE_GOOD_VSNPRINTF], 1, [Define if the vsnprintf function works.])
fi])

