# Autoconf macros for the GNU MPFR Library.
# Copyright (C) 2000-2013 Free Software Foundation, Inc.
# Contributed by the AriC and Caramel projects, INRIA.
#
# Copyright (C) 2014 Peter Breitenlohner <tex-live@tug.org>
# Extracted from mpfr-src/acinclude.m4 and adapted for TeX Live.
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# MPFR_C_LONG_DOUBLE_FORMAT
# -------------------------
# Determine the format of a long double.
#
# The object file is grepped, so as to work when cross compiling.  A
# start and end sequence is included to avoid false matches, and
# allowance is made for the desired data crossing an "od -b" line
# boundary.  The test number is a small integer so it should appear
# exactly, no rounding or truncation etc.
#
# "od -b" is supported even by Unix V7, and the awk script used doesn't
# have functions or anything, so even an "old" awk should suffice.
#
# The 10-byte IEEE extended format is generally padded to either 12 or 16
# bytes for alignment purposes.  The SVR4 i386 ABI is 12 bytes, or i386
# gcc -m128bit-long-double selects 16 bytes.  IA-64 is 16 bytes in LP64
# mode, or 12 bytes in ILP32 mode.  The first 10 bytes is the relevant
# part in all cases (big and little endian).
#
# Enhancements:
#
# Could match more formats, but no need to worry until there's code
# wanting to use them.
#
# Don't want to duplicate the double matching from GMP_C_DOUBLE_FORMAT,
# perhaps we should merge with that macro, to match data formats
# irrespective of the C type in question.  Or perhaps just let the code
# use DOUBLE macros when sizeof(double)==sizeof(long double).

AC_DEFUN([MPFR_C_LONG_DOUBLE_FORMAT], [dnl
AC_REQUIRE([AC_PROG_CC])
AC_REQUIRE([AC_PROG_AWK])
AC_CACHE_CHECK([format of `long double' floating point],
               [mpfr_cv_c_long_double_format],
[mpfr_cv_c_long_double_format=unknown
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
/* "before" is 16 bytes to ensure there's no padding between it and "x".
   We're not expecting any "long double" bigger than 16 bytes or with
   alignment requirements stricter than 16 bytes.  */
struct {
  char         before[16];
  long double  x;
  char         after[8];
} foo = {
  { '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
    '\001', '\043', '\105', '\147', '\211', '\253', '\315', '\357' },
  -123456789.0,
  { '\376', '\334', '\272', '\230', '\166', '\124', '\062', '\020' }
};
]])],
   [cat >conftest.awk <<\EOF
_mpfr_long_double_awk
EOF
    mpfr_cv_c_long_double_format=`od -b conftest.$ac_objext | $AWK -f conftest.awk`
    AS_CASE([$mpfr_cv_c_long_double_format],
            [unknown*],
              [echo "cannot match anything, conftest.$ac_objext contains" >&AS_MESSAGE_LOG_FD
               od -b conftest.$ac_objext >&AS_MESSAGE_LOG_FD])],
   [AC_MSG_WARN([oops, cannot compile test program])])
rm -f conftest.awk])

AH_VERBATIM([HAVE_LDOUBLE],
[/* Define one of the following to 1 for the format of a `long double'.
   If your format is not among these choices, or you don't know what it is,
   then leave all undefined.
   IEEE_EXT is the 10-byte IEEE extended precision format.
   IEEE_QUAD is the 16-byte IEEE quadruple precision format.
   LITTLE or BIG is the endianness.  */
#undef HAVE_LDOUBLE_IEEE_EXT_LITTLE
#undef HAVE_LDOUBLE_IEEE_QUAD_LITTLE
#undef HAVE_LDOUBLE_IEEE_QUAD_BIG])

AS_CASE([$mpfr_cv_c_long_double_format],
        ["IEEE extended, little endian"],
          [AC_DEFINE([HAVE_LDOUBLE_IEEE_EXT_LITTLE], 1)],
        ["IEEE quad, big endian"], 
          [AC_DEFINE([HAVE_LDOUBLE_IEEE_QUAD_BIG], 1)],
        ["IEEE quad, little endian"],
          [AC_DEFINE([HAVE_LDOUBLE_IEEE_QUAD_LITTLE], 1)],
        ["possibly double-double, big endian"],
          [# Since we are not sure, we do not want to define a macro.
           AC_MSG_WARN([This format is known on GCC/PowerPC platforms,])
           AC_MSG_WARN([but due to GCC PR26374, we can't test further.])
           AC_MSG_WARN([You can safely ignore this warning, though.])],
        [unknown* | "not available"], [],
        [AC_MSG_WARN([oops, unrecognised float format: $mpfr_cv_c_long_double_format])])
]) # MPFR_C_LONG_DOUBLE_FORMAT

# _mpfr_long_double_awk
# ---------------------
# Internal subroutine defining contents of conftest.awk.
m4_define([_mpfr_long_double_awk], [[
BEGIN {
  found = 0
}

# got[] holds a sliding window of bytes read the input.  got[0] is the most
# recent byte read, and got[31] the oldest byte read, so when looking to
# match some data the indices are "reversed".
#
{
  for (f = 2; f <= NF; f++)
    {
      # new byte, shift others up
      for (i = 31; i >= 0; i--)
        got[i+1] = got[i];
      got[0] = $f;

      # end sequence
      if (got[7] != "376") continue
      if (got[6] != "334") continue
      if (got[5] != "272") continue
      if (got[4] != "230") continue
      if (got[3] != "166") continue
      if (got[2] != "124") continue
      if (got[1] != "062") continue
      if (got[0] != "020") continue

      # start sequence, with 8-byte body
      if (got[23] == "001" && \
          got[22] == "043" && \
          got[21] == "105" && \
          got[20] == "147" && \
          got[19] == "211" && \
          got[18] == "253" && \
          got[17] == "315" && \
          got[16] == "357")
        {
          saw = " (" got[15] \
                 " " got[14] \
                 " " got[13] \
                 " " got[12] \
                 " " got[11] \
                 " " got[10] \
                 " " got[9]  \
                 " " got[8] ")"

          if (got[15] == "301" && \
              got[14] == "235" && \
              got[13] == "157" && \
              got[12] == "064" && \
              got[11] == "124" && \
              got[10] == "000" && \
              got[9] ==  "000" && \
              got[8] ==  "000")
            {
              print "IEEE double, big endian"
              found = 1
              exit
            }

          if (got[15] == "000" && \
              got[14] == "000" && \
              got[13] == "000" && \
              got[12] == "124" && \
              got[11] == "064" && \
              got[10] == "157" && \
              got[9] ==  "235" && \
              got[8] ==  "301")
            {
              print "IEEE double, little endian"
              found = 1
              exit
            }
        }

      # start sequence, with 12-byte body
      if (got[27] == "001" && \
          got[26] == "043" && \
          got[25] == "105" && \
          got[24] == "147" && \
          got[23] == "211" && \
          got[22] == "253" && \
          got[21] == "315" && \
          got[20] == "357")
        {
          saw = " (" got[19] \
                 " " got[18] \
                 " " got[17] \
                 " " got[16] \
                 " " got[15] \
                 " " got[14] \
                 " " got[13] \
                 " " got[12] \
                 " " got[11] \
                 " " got[10] \
                 " " got[9]  \
                 " " got[8] ")"

          if (got[19] == "000" && \
              got[18] == "000" && \
              got[17] == "000" && \
              got[16] == "000" && \
              got[15] == "240" && \
              got[14] == "242" && \
              got[13] == "171" && \
              got[12] == "353" && \
              got[11] == "031" && \
              got[10] == "300")
            {
              print "IEEE extended, little endian"
              found = 1
              exit
            }

          if (got[19] == "300" && \
              got[18] == "031" && \
              got[17] == "000" && \
              got[16] == "000" && \
              got[15] == "353" && \
              got[14] == "171" && \
              got[13] == "242" && \
              got[12] == "240" && \
              got[11] == "000" && \
              got[10] == "000" && \
              got[09] == "000" && \
              got[08] == "000")
            {
              # format found on m68k
              print "IEEE extended, big endian"
              found = 1
              exit
            }
        }

      # start sequence, with 16-byte body
      if (got[31] == "001" && \
          got[30] == "043" && \
          got[29] == "105" && \
          got[28] == "147" && \
          got[27] == "211" && \
          got[26] == "253" && \
          got[25] == "315" && \
          got[24] == "357")
        {
          saw = " (" got[23] \
                 " " got[22] \
                 " " got[21] \
                 " " got[20] \
                 " " got[19] \
                 " " got[18] \
                 " " got[17] \
                 " " got[16] \
                 " " got[15] \
                 " " got[14] \
                 " " got[13] \
                 " " got[12] \
                 " " got[11] \
                 " " got[10] \
                 " " got[9]  \
                 " " got[8] ")"

          if (got[23] == "000" && \
              got[22] == "000" && \
              got[21] == "000" && \
              got[20] == "000" && \
              got[19] == "240" && \
              got[18] == "242" && \
              got[17] == "171" && \
              got[16] == "353" && \
              got[15] == "031" && \
              got[14] == "300")
            {
              print "IEEE extended, little endian"
              found = 1
              exit
            }

          if (got[23] == "300" && \
              got[22] == "031" && \
              got[21] == "326" && \
              got[20] == "363" && \
              got[19] == "105" && \
              got[18] == "100" && \
              got[17] == "000" && \
              got[16] == "000" && \
              got[15] == "000" && \
              got[14] == "000" && \
              got[13] == "000" && \
              got[12] == "000" && \
              got[11] == "000" && \
              got[10] == "000" && \
              got[9]  == "000" && \
              got[8]  == "000")
            {
              # format used on HP 9000/785 under HP-UX
              print "IEEE quad, big endian"
              found = 1
              exit
            }

          if (got[23] == "000" && \
              got[22] == "000" && \
              got[21] == "000" && \
              got[20] == "000" && \
              got[19] == "000" && \
              got[18] == "000" && \
              got[17] == "000" && \
              got[16] == "000" && \
              got[15] == "000" && \
              got[14] == "000" && \
              got[13] == "100" && \
              got[12] == "105" && \
              got[11] == "363" && \
              got[10] == "326" && \
              got[9]  == "031" && \
	      got[8]  == "300")
            {
              print "IEEE quad, little endian"
              found = 1
              exit
            }

          if (got[23] == "301" && \
              got[22] == "235" && \
              got[21] == "157" && \
              got[20] == "064" && \
              got[19] == "124" && \
              got[18] == "000" && \
              got[17] == "000" && \
              got[16] == "000" && \
              got[15] == "000" && \
              got[14] == "000" && \
              got[13] == "000" && \
              got[12] == "000" && \
              got[11] == "000" && \
              got[10] == "000" && \
              got[9]  == "000" && \
              got[8]  == "000")
            {
              # format used on 32-bit PowerPC (Mac OS X and Debian GNU/Linux)
              print "possibly double-double, big endian"
              found = 1
              exit
            }
        }
    }
}

END {
  if (! found)
    print "unknown", saw
}
]])
