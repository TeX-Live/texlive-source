dnl ### Check for at-least-pretend Streams capability

AC_DEFUN([XDVI_SYS_STREAMS],
[AC_CACHE_CHECK([for stropts.h and isastream()], xdvi_cv_sys_streams,
[AC_TRY_LINK(
[#include <stropts.h>
], [#ifndef I_SETSIG
choke me
#else
isastream(0);
#endif], xdvi_cv_sys_streams=yes, xdvi_cv_sys_streams=no)])
if test $xdvi_cv_sys_streams = yes; then
  AC_DEFINE([HAVE_STREAMS], 1, [Define if your system has STREAMS (and if X uses it).])
fi])


