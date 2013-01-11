# longlong.m4 extracted from types.m4 for Autoconf 2.69.
#
# Copyright (C) 2000-2002, 2004-2012 Free Software Foundation, Inc.

# The file types.m4 is part of Autoconf.  This program is free
# software; you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the
# Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# Under Section 7 of GPL version 3, you are granted additional
# permissions described in the Autoconf Configure Script Exception,
# version 3.0, as published by the Free Software Foundation.
#
# You should have received a copy of the GNU General Public License
# and a copy of the Autoconf Configure Script Exception along with
# this program; see the files COPYINGv3 and COPYING.EXCEPTION
# respectively.  If not, see <http://www.gnu.org/licenses/>.

# Written by David MacKenzie, with help from
# Franc,ois Pinard, Karl Berry, Richard Pixley, Ian Lance Taylor,
# Roland McGrath, Noah Friedman, david d zuhn, and many others.

# _AC_TYPE_LONG_LONG_SNIPPET
# --------------------------
# Expands to a C program that can be used to test for simultaneous support
# of 'long long' and 'unsigned long long'. We don't want to say that
# 'long long' is available if 'unsigned long long' is not, or vice versa,
# because too many programs rely on the symmetry between signed and unsigned
# integer types (excluding 'bool').
AC_DEFUN([_AC_TYPE_LONG_LONG_SNIPPET],
[
  AC_LANG_PROGRAM(
    [[/* For now, do not test the preprocessor; as of 2007 there are too many
	 implementations with broken preprocessors.  Perhaps this can
	 be revisited in 2012.  In the meantime, code should not expect
	 #if to work with literals wider than 32 bits.  */
      /* Test literals.  */
      long long int ll = 9223372036854775807ll;
      long long int nll = -9223372036854775807LL;
      unsigned long long int ull = 18446744073709551615ULL;
      /* Test constant expressions.   */
      typedef int a[((-9223372036854775807LL < 0 && 0 < 9223372036854775807ll)
		     ? 1 : -1)];
      typedef int b[(18446744073709551615ULL <= (unsigned long long int) -1
		     ? 1 : -1)];
      int i = 63;]],
    [[/* Test availability of runtime routines for shift and division.  */
      long long int llmax = 9223372036854775807ll;
      unsigned long long int ullmax = 18446744073709551615ull;
      return ((ll << 63) | (ll >> 63) | (ll < i) | (ll > i)
	      | (llmax / ll) | (llmax % ll)
	      | (ull << 63) | (ull >> 63) | (ull << i) | (ull >> i)
	      | (ullmax / ull) | (ullmax % ull));]])
])


# AC_TYPE_LONG_LONG_INT
# ---------------------
AC_DEFUN([AC_TYPE_LONG_LONG_INT],
[
  AC_REQUIRE([AC_TYPE_UNSIGNED_LONG_LONG_INT])
  AC_CACHE_CHECK([for long long int], [ac_cv_type_long_long_int],
     [ac_cv_type_long_long_int=yes
      if test "x${ac_cv_prog_cc_c99-no}" = xno; then
	ac_cv_type_long_long_int=$ac_cv_type_unsigned_long_long_int
	if test $ac_cv_type_long_long_int = yes; then
	  dnl Catch a bug in Tandem NonStop Kernel (OSS) cc -O circa 2004.
	  dnl If cross compiling, assume the bug is not important, since
	  dnl nobody cross compiles for this platform as far as we know.
	  AC_RUN_IFELSE(
	    [AC_LANG_PROGRAM(
	       [[@%:@include <limits.h>
		 @%:@ifndef LLONG_MAX
		 @%:@ define HALF \
			  (1LL << (sizeof (long long int) * CHAR_BIT - 2))
		 @%:@ define LLONG_MAX (HALF - 1 + HALF)
		 @%:@endif]],
	       [[long long int n = 1;
		 int i;
		 for (i = 0; ; i++)
		   {
		     long long int m = n << i;
		     if (m >> i != n)
		       return 1;
		     if (LLONG_MAX / 2 < m)
		       break;
		   }
		 return 0;]])],
	    [],
	    [ac_cv_type_long_long_int=no],
	    [:])
	fi
      fi])
  if test $ac_cv_type_long_long_int = yes; then
    AC_DEFINE([HAVE_LONG_LONG_INT], [1],
      [Define to 1 if the system has the type `long long int'.])
  fi
])


# AC_TYPE_UNSIGNED_LONG_LONG_INT
# ------------------------------
AC_DEFUN([AC_TYPE_UNSIGNED_LONG_LONG_INT],
[
  AC_CACHE_CHECK([for unsigned long long int],
    [ac_cv_type_unsigned_long_long_int],
    [ac_cv_type_unsigned_long_long_int=yes
     if test "x${ac_cv_prog_cc_c99-no}" = xno; then
       AC_LINK_IFELSE(
	 [_AC_TYPE_LONG_LONG_SNIPPET],
	 [],
	 [ac_cv_type_unsigned_long_long_int=no])
     fi])
  if test $ac_cv_type_unsigned_long_long_int = yes; then
    AC_DEFINE([HAVE_UNSIGNED_LONG_LONG_INT], [1],
      [Define to 1 if the system has the type `unsigned long long int'.])
  fi
])
