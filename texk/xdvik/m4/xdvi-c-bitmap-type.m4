dnl ### Determine integer type to use for bitmaps

AC_DEFUN([XDVI_C_BITMAP_TYPE],
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
AC_DEFINE_UNQUOTED([BMTYPE], [$BMTYPE],
                   [Define to determine the integer type to be used in bitmaps.
                    The type used will be "unsigned BMTYPE".])
AC_DEFINE_UNQUOTED([BMBYTES], [$BMBYTES], [Define to the length (in bytes) of type BMTYPE.])
AC_MSG_RESULT([unsigned $BMTYPE, size = $BMBYTES])])


