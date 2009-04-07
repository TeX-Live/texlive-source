dnl ### Check for poll()

AC_DEFUN([XDVI_FUNC_POLL],
[AC_CACHE_CHECK([for poll.h and poll()], xdvi_cv_func_poll,
[AC_TRY_LINK(
[#include <poll.h>
], [poll((struct pollfd *) 0, 0, 0);],
xdvi_cv_func_poll=yes, xdvi_cv_func_poll=no)])
if test $xdvi_cv_func_poll = yes; then
  AC_DEFINE([HAVE_POLL], 1, [Define if your system has <poll.h> and poll().])
else
  AC_CHECK_HEADERS(sys/select.h select.h)
fi])


