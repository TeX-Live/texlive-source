dnl ### Determine integer type to use for bitmaps

AC_DEFUN(XDVI_C_BITMAP_TYPE,
[AC_MSG_CHECKING(for integer type to use in bitmaps)
AC_CACHE_VAL(xdvi_cv_bitmap_type,
[AC_TRY_RUN(
[#include <stdio.h>
main()
{
  FILE *f=fopen("conftestval", "w");
  if (!f) exit(1);
  if ((sizeof(unsigned long) == 4 || sizeof(unsigned long) == 2)
    && sizeof(unsigned long) != sizeof(unsigned int))
      fprintf(f, "BMTYPE=long BMBYTES=%d\n", sizeof(unsigned long));
  if (sizeof(unsigned int) == 4 || sizeof(unsigned int) == 2)
    fprintf(f, "BMTYPE=int BMBYTES=%d\n", sizeof(unsigned int));
  else if (sizeof(unsigned short) == 4 || sizeof(unsigned short) == 2)
    fprintf(f, "BMTYPE=short BMBYTES=%d\n", sizeof(unsigned short));
  else fprintf(f, "BMTYPE=char BMBYTES=%d\n", sizeof(unsigned char));
  exit(0);
}],
xdvi_cv_bitmap_type="`cat conftestval`",
AC_MSG_ERROR(could not determine integer type for bitmap))])
eval "$xdvi_cv_bitmap_type"
AC_DEFINE_UNQUOTED(BMTYPE, $BMTYPE)
AC_DEFINE_UNQUOTED(BMBYTES, $BMBYTES)
AC_MSG_RESULT([unsigned $BMTYPE, size = $BMBYTES])])


dnl ### Check for at-least-pretend Streams capability

AC_DEFUN(XDVI_SYS_STREAMS,
[AC_CACHE_CHECK([for stropts.h and isastream()], xdvi_cv_sys_streams,
[AC_TRY_LINK(
[#include <stropts.h>
], [#ifndef I_SETSIG
choke me
#else
isastream(0);
#endif], xdvi_cv_sys_streams=yes, xdvi_cv_sys_streams=no)])
if test $xdvi_cv_sys_streams = yes; then
  AC_DEFINE(HAVE_STREAMS)
fi])


dnl ### Check for poll()

AC_DEFUN(XDVI_FUNC_POLL,
[AC_CACHE_CHECK([for poll.h and poll()], xdvi_cv_func_poll,
[AC_TRY_LINK(
[#include <poll.h>
], [poll((struct pollfd *) 0, 0, 0);],
xdvi_cv_func_poll=yes, xdvi_cv_func_poll=no)])
if test $xdvi_cv_func_poll = yes; then
  AC_DEFINE(HAVE_POLL)
else
  AC_CHECK_HEADERS(sys/select.h select.h)
fi])


dnl ### Check for SunOS 4

AC_DEFUN(XDVI_SYS_SUNOS_4,
[AC_CACHE_CHECK([for SunOS 4], xdvi_cv_sys_sunos_4,
[case "`(uname -sr) 2>/dev/null`" in
"SunOS 4."*)
  xdvi_cv_sys_sunos_4=yes ;;
*) xdvi_cv_sys_sunos_4=no ;;
esac])
if test $xdvi_cv_sys_sunos_4 = yes; then
  AC_DEFINE(SUNOS4)
fi])
