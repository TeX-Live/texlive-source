dnl
dnl acinclude.m4 for OBC and friends
dnl 
dnl This file is part of the Oxford Oberon-2 compiler
dnl Copyright (c) 2006--2020 J. M. Spivey
dnl All rights reserved
dnl
dnl Redistribution and use in source and binary forms, with or without
dnl modification, are permitted provided that the following conditions
dnl are met: 
dnl
dnl 1. Redistributions of source code must retain the above copyright
dnl    notice, this list of conditions and the following disclaimer.
dnl 2. Redistributions in binary form must reproduce the above
dnl    copyright notice, this list of conditions and the following
dnl    disclaimer in the documentation and/or other materials provided
dnl    with the distribution. 
dnl 3. The name of the author may not be used to endorse or promote products
dnl    derived from this software without specific prior written permission.
dnl
dnl THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
dnl OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
dnl WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
dnl ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
dnl DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
dnl DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
dnl GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
dnl INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
dnl WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
dnl NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
dnl SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
dnl

dnl See if indexed jumps will compile
AC_DEFUN([AC_C_INDEXED_JUMPS],
  [AC_CACHE_CHECK(for indexed jumps, ac_cv_c_indexed_jumps,
      [AC_TRY_COMPILE(, [void *a[] = { &&b, &&c }; b: goto *a[1]; c:;],
        ac_cv_c_indexed_jumps=yes, ac_cv_c_indexed_jumps=no)])
    if test $ac_cv_c_indexed_jumps = yes; then
      AC_DEFINE(HAVE_INDEXED_JUMPS, 1, 
        [Define if indexed jumps work.])
    fi])

dnl See if the C compiler understands __attribute__ ((unused))
AC_DEFUN([AC_C_UNUSED],
  [AC_CACHE_CHECK(for 'unused' attribute, ac_cv_unused_attr,
      [AC_TRY_COMPILE(, [int __attribute__ ((unused)) n],
	ac_cv_unused_attr=yes, ac_cv_unused_attr=no)])
    if test $ac_cv_unused_attr = yes; then 
      UNUSED="__attribute__ ((unused))"
    fi
    AC_DEFINE_UNQUOTED(UNUSED, $UNUSED, [Attribute for unused variables])])

dnl See if the C compiler understands __attribute__ ((used))
AC_DEFUN([AC_C_USED],
  [AC_CACHE_CHECK(for 'used' attribute, ac_cv_used_attr,
      [AC_TRY_COMPILE(, [int __attribute__ ((used)) n],
	ac_cv_used_attr=yes, ac_cv_used_attr=no)])
    if test $ac_cv_used_attr = yes; then 
      USED="__attribute__ ((used))"
    fi
    AC_DEFINE_UNQUOTED(USED, $USED, [Attribute for used variables])])

dnl See if the C compiler understands __attribute__ ((noreturn))
AC_DEFUN([AC_C_NORETURN],
  [AC_CACHE_CHECK(for 'noreturn' attribute, ac_cv_noreturn_attr,
      [AC_COMPILE_IFELSE(
        [AC_LANG_SOURCE(
          [[void __attribute__ ((noreturn)) f(void) { while (1) ; }]])],
	ac_cv_noreturn_attr=yes, ac_cv_noreturn_attr=no)])
    if test $ac_cv_noreturn_attr = yes; then 
      NORETURN="__attribute__ ((noreturn))"
    fi
    AC_DEFINE_UNQUOTED(NORETURN, $NORETURN, 
    		       [Attribute for functions that don't return])])

dnl test if C compiler understands -rdynamic
AC_DEFUN([AC_TEST_RDYNAMIC],
  [AC_CACHE_CHECK(if ${CC-cc} understands -rdynamic, ac_cv_rdynamic,
    [echo 'main() { return 0; }' >conftest.c
      if AC_TRY_COMMAND(${CC-cc} $CFLAGS -o conftest${ac_exeext} -rdynamic
						conftest.c 2>conftest.err) \
			&& ! grep -q 'unrecognized option' conftest.err 
	then ac_cv_rdynamic=yes; else ac_cv_rdynamic=no; fi
      rm -f conftest.c conftest.err conftest${ac_exeext}])
  if test $ac_cv_rdynamic = yes; then $1; fi])

dnl Find the page size (check sys/param.h if this doesn't work)
AC_DEFUN([AC_FIND_PAGESIZE],
  [AC_CHECK_FUNCS(getpagesize)
    AC_CACHE_CHECK(page size, ac_cv_pagesize,
      [AC_TRY_RUN([#include <stdio.h>
	#ifndef HAVE_GETPAGESIZE
	#ifdef HAVE_UNISTD_H
	#include <unistd.h>
	#endif
	#ifdef _SC_PAGESIZE
	#define getpagesize() sysconf(_SC_PAGESIZE)
	#endif
	#endif
	main() {
	  FILE *f = fopen("conftestval", "w");
	  if (f == NULL) exit(1);
	  /* No newline here, in case a CRLF creeps in and cygwin chokes */
	  fprintf(f, "%d", getpagesize());
	  exit(0);
	}], ac_cv_pagesize=`cat conftestval`,
        ac_cv_pagesize=4096, ac_cv_pagesize=4096)])
    AC_DEFINE_UNQUOTED(PAGESIZE, $ac_cv_pagesize, [Page size])
    AC_SUBST(PAGESIZE, $ac_cv_pagesize)

    # Compute log2(page size)
    tmpa=1; tmpb=0
    while test $tmpa -lt $ac_cv_pagesize; do
      tmpa=`expr 2 \* $tmpa`; tmpb=`expr $tmpb + 1`
    done
    AC_DEFINE_UNQUOTED(LOG_PAGESIZE, $tmpb, [Log2 of page size])])
