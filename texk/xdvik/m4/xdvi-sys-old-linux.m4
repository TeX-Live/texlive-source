dnl ### Check for certain broken versions of Linux

AC_DEFUN([XDVI_SYS_OLD_LINUX],
[AC_CACHE_CHECK([for certain old versions of Linux], xdvi_cv_sys_old_linux,
[case "`(uname -sr) 2>/dev/null`" in
"Linux 2."[[01]].* | "Linux 2.2."[[0-8]] | "Linux 2.2."[[0-8]]-*)
  xdvi_cv_sys_old_linux=yes ;;
*) xdvi_cv_sys_old_linux=no ;;
esac])
if test $xdvi_cv_sys_old_linux = yes; then
  AC_DEFINE([FLAKY_SIGPOLL], 1,
            [Define if you are using Linux 2.1.xxx -- 2.2.8, or if you find it necessary.])
fi])

