AC_DEFUN([AC_PROG_CC_STRICT_PROTOTYPES], [
  pushdef([CV], ac_cv_prog_cc_strict_prototypes)dnl
  hard=$2
  if test -z "$hard"; then
    msg="C to warn at nonstrict prototypes"
  else
    msg="C to require strict prototypes"
  fi
  AC_CACHE_CHECK($msg, CV, [
  cat > conftest.c <<EOF
#include <sys/signal.h>
int main (void)
{
   if (signal (SIGINT, SIG_IGN) == SIG_DFL) return 0;
   return 1;
}
EOF

  dnl GCC
  if test "$GCC" = "yes"; then
    if test -z "$hard"; then
      CV="-Wstrict-prototypes"
    else
      CV="-fstrict-prototypes -Wstrict-prototypes"
    fi

    if test -n "`${CC-cc} -c $CV conftest.c 2>&1`" ; then
      CV="suppressed...sys/stat.h"
    fi

  dnl Solaris C compiler

  dnl HP-UX C compiler

  dnl Digital Unix C compiler

  dnl C for AIX Compiler

  dnl IRIX C compiler

  fi
  rm -f conftest.*
  ])
  if test -z "[$]$1" ; then
    if test -n "$CV" ; then
      case "$CV" in
        *...*) $1="" ;; # known but suppressed
        *)  $1="$CV" ;;
      esac
    fi
  fi
  AC_SUBST($1)
  popdef([CV])dnl
])



