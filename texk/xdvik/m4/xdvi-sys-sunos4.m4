dnl ### Check for SunOS 4

AC_DEFUN([XDVI_SYS_SUNOS_4],
[AC_CACHE_CHECK([for SunOS 4], xdvi_cv_sys_sunos_4,
[case "`(uname -sr) 2>/dev/null`" in
"SunOS 4."*)
  xdvi_cv_sys_sunos_4=yes ;;
*) xdvi_cv_sys_sunos_4=no ;;
esac])
if test $xdvi_cv_sys_sunos_4 = yes; then
  AC_DEFINE([SUNOS4], 1, [Define if you are using SunOS 4.x.])
fi])

