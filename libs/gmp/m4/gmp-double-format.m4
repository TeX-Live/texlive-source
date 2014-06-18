# Autoconf macros for the GNU MP Library.
# Copyright (C) 2000-2014 Free Software Foundation, Inc.
#
# Copyright (C) 2014 Peter Breitenlohner <tex-live@tug.org>
# Extracted from gmp-6.0.0/acinclude.m4 and adapted for TeX Live.
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# GMP_C_DOUBLE_FORMAT
# -------------------
# Determine the floating point format.
#
# The object file is grepped, in order to work when cross compiling.  A
# start and end sequence is included to avoid false matches, and allowance
# is made for the desired data crossing an "od -b" line boundary.  The test
# number is a small integer so it should appear exactly, no rounding or
# truncation etc.
#
# "od -b", incidentally, is supported even by Unix V7, and the awk script
# used doesn't have functions or anything, so even an "old" awk should
# suffice.
#
# The C code here declares the variable foo as extern; without that, some
# C++ compilers will not put foo in the object file.

AC_DEFUN([GMP_C_DOUBLE_FORMAT], [dnl
AC_REQUIRE([AC_PROG_CC])
AC_REQUIRE([AC_PROG_AWK])
AC_CACHE_CHECK([format of `double' floating point],
               [gmp_cv_c_double_format],
[gmp_cv_c_double_format=unknown
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
struct foo {
  char    before[8];
  double  x;
  char    after[8];
};
extern struct foo foo;
struct foo foo = {
  { '\001', '\043', '\105', '\147', '\211', '\253', '\315', '\357' },
  -123456789.0,
  { '\376', '\334', '\272', '\230', '\166', '\124', '\062', '\020' },
};
]])],
   [cat >conftest.awk <<\EOF
_gmp_double_awk
EOF
  gmp_cv_c_double_format=`od -b conftest.$ac_objext | $AWK -f conftest.awk`
  AS_CASE([$gmp_cv_c_double_format],
          [unknown*],
            [echo "cannot match anything, conftest.$ac_objext contains" >&AS_MESSAGE_LOG_FD
             od -b conftest.$ac_objext >&AS_MESSAGE_LOG_FD])],
   [AC_MSG_WARN([oops, cannot compile test program])])
rm -f conftest.awk])

AH_VERBATIM([HAVE_DOUBLE],
[/* Define one of the following to 1 for the format of a `double'.
   If your format is not among these choices, or you don't know what it is,
   then leave all undefined.
   IEEE_LITTLE_SWAPPED means little endian, but with the two 4-byte halves
   swapped, as used by ARM CPUs in little endian mode.  */
#undef HAVE_DOUBLE_IEEE_BIG_ENDIAN
#undef HAVE_DOUBLE_IEEE_LITTLE_ENDIAN
#undef HAVE_DOUBLE_IEEE_LITTLE_SWAPPED
#undef HAVE_DOUBLE_VAX_D
#undef HAVE_DOUBLE_VAX_G
#undef HAVE_DOUBLE_CRAY_CFP])

AS_CASE([$gmp_cv_c_double_format],
        ["IEEE big endian"],
          [AC_DEFINE([HAVE_DOUBLE_IEEE_BIG_ENDIAN], 1)],
        ["IEEE little endian"],
          [AC_DEFINE([HAVE_DOUBLE_IEEE_LITTLE_ENDIAN], 1)],
        ["IEEE little endian, swapped halves"],
          [AC_DEFINE([HAVE_DOUBLE_IEEE_LITTLE_SWAPPED], 1)],
        ["VAX D"],
          [AC_DEFINE([HAVE_DOUBLE_VAX_D], 1)],
        ["VAX G"],
          [AC_DEFINE([HAVE_DOUBLE_VAX_G], 1)],
        ["Cray CFP"],
          [AC_DEFINE([HAVE_DOUBLE_CRAY_CFP], 1)],
        ["bad ARM software floats"], [],
        [unknown*],
          [AC_MSG_WARN([Could not determine float format.])
           AC_MSG_WARN([Conversions to and from "double" may be slow.])],
        [AC_MSG_WARN([oops, unrecognised float format: $gmp_cv_c_double_format])])
]) # GMP_C_DOUBLE_FORMAT

# _gmp_double_awk
# ---------------
# Internal subroutine defining contents of conftest.awk.
m4_define([_gmp_double_awk], [[
BEGIN {
  found = 0
}

{
  for (f = 2; f <= NF; f++)
    {
      for (i = 0; i < 23; i++)
        got[i] = got[i+1];
      got[23] = $f;

      # match the special begin and end sequences
      if (got[0] != "001") continue
      if (got[1] != "043") continue
      if (got[2] != "105") continue
      if (got[3] != "147") continue
      if (got[4] != "211") continue
      if (got[5] != "253") continue
      if (got[6] != "315") continue
      if (got[7] != "357") continue
      if (got[16] != "376") continue
      if (got[17] != "334") continue
      if (got[18] != "272") continue
      if (got[19] != "230") continue
      if (got[20] != "166") continue
      if (got[21] != "124") continue
      if (got[22] != "062") continue
      if (got[23] != "020") continue

      saw = " (" got[8] " " got[9] " " got[10] " " got[11] " " got[12] " " got[13] " " got[14] " " got[15] ")"

      if (got[8]  == "000" &&  \
          got[9]  == "000" &&  \
          got[10] == "000" &&  \
          got[11] == "124" &&  \
          got[12] == "064" &&  \
          got[13] == "157" &&  \
          got[14] == "235" &&  \
          got[15] == "301")
        {
          print "IEEE little endian"
          found = 1
          exit
        }

      # Little endian with the two 4-byte halves swapped, as used by ARM
      # when the chip is in little endian mode.
      #
      if (got[8]  == "064" &&  \
          got[9]  == "157" &&  \
          got[10] == "235" &&  \
          got[11] == "301" &&  \
          got[12] == "000" &&  \
          got[13] == "000" &&  \
          got[14] == "000" &&  \
          got[15] == "124")
        {
          print "IEEE little endian, swapped halves"
          found = 1
          exit
        }

      # gcc 2.95.4 on one GNU/Linux ARM system was seen generating 000 in
      # the last byte, whereas 124 is correct.  Not sure where the bug
      # actually lies, but a running program didn't seem to get a full
      # mantissa worth of working bits.
      #
      # We match this case explicitly so we can give a nice result message,
      # but we deliberately exclude it from the normal IEEE double setups
      # since it's too broken.
      #
      if (got[8]  == "064" &&  \
          got[9]  == "157" &&  \
          got[10] == "235" &&  \
          got[11] == "301" &&  \
          got[12] == "000" &&  \
          got[13] == "000" &&  \
          got[14] == "000" &&  \
          got[15] == "000")
        {
          print "bad ARM software floats"
          found = 1
          exit
        }

      if (got[8]  == "301" &&  \
          got[9]  == "235" &&  \
          got[10] == "157" &&  \
          got[11] == "064" &&  \
          got[12] == "124" &&  \
          got[13] == "000" &&  \
          got[14] == "000" &&  \
	  got[15] == "000")
        {
          print "IEEE big endian"
          found = 1
          exit
        }

      if (got[8]  == "353" &&  \
          got[9]  == "315" &&  \
          got[10] == "242" &&  \
          got[11] == "171" &&  \
          got[12] == "000" &&  \
          got[13] == "240" &&  \
          got[14] == "000" &&  \
          got[15] == "000")
        {
          print "VAX D"
          found = 1
          exit
        }

      if (got[8]  == "275" &&  \
          got[9]  == "301" &&  \
          got[10] == "064" &&  \
          got[11] == "157" &&  \
          got[12] == "000" &&  \
          got[13] == "124" &&  \
          got[14] == "000" &&  \
          got[15] == "000")
        {
          print "VAX G"
          found = 1
          exit
        }

      if (got[8]  == "300" &&  \
          got[9]  == "033" &&  \
          got[10] == "353" &&  \
          got[11] == "171" &&  \
          got[12] == "242" &&  \
          got[13] == "240" &&  \
          got[14] == "000" &&  \
          got[15] == "000")
        {
          print "Cray CFP"
          found = 1
          exit
        }
    }
}

END {
  if (! found)
    print "unknown", saw
}
]])
