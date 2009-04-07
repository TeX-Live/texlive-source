dnl ### Check for whether setsid() is allowed within vfork()
dnl (Mac OS X 10.3 (Panther, 11/2003) is one O/S which does not allow this.)

AC_DEFUN([XDVI_FUNC_SETSID_IN_VFORK],
[if test $ac_cv_func_vfork_works = yes; then
AC_CACHE_CHECK([for whether setsid() is allowed within vfork()],
xdvi_cv_setsid_in_vfork,
[AC_TRY_RUN(
[/* Test adapted from Gnu autoconf */
/* Thanks to Paul Eggert for this test.  */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_VFORK_H
#include <vfork.h>
#endif
int
main() {
  pid_t parent = getpid ();
  pid_t child;

  child = vfork ();

  if (child == 0) {
    if (setsid () == -1)
      _exit(1);
    _exit(0);
  } else {
    int status;

    while (wait(&status) != child)
      ;
    exit(
	 /* Was there some problem with vforking?  */
	 child < 0

	 /* Did the child fail?  (This shouldn't happen.)  */
	 || status
	 );
  }
}],
xdvi_cv_setsid_in_vfork=yes,
xdvi_cv_setsid_in_vfork=no,
# safe value for cross-compiling
xdvi_cv_setsid_in_vfork=no)])
if test $xdvi_cv_setsid_in_vfork = yes; then
  AC_DEFINE([HAVE_GOOD_SETSID_VFORK], 1,
            [Define if your system allows setsid() within vfork().])
fi]
fi)

