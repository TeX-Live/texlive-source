dnl ### Check whether iconv takes a 'const char **' or a 'char **' input argument.
dnl ### According to IEEE 1003.1, `char **' is correct, but e.g. librecode
dnl ### uses `const char **'.
dnl ### We use C++'s built-in function overloading to distinguish between the two.
AC_DEFUN([XDVI_ICONV_CHAR_PPTR_TYPE],
[
AC_LANG_SAVE AC_LANG_CPLUSPLUS
# AC_LANG_PUSH(C++) # in newer autoconf
#
xdvi_iconv_save_libs="$LIBS"
xdvi_iconv_save_cxxflags="$CXXFLAGS"
xdvi_iconv_save_ldflags="$LDFLAGS"
#
LIBS="$LIBS $iconv_libs"
CXXFLAGS="$CXXFLAGS $iconv_includes"
LDFLAGS="$LDFLAGS $iconv_libpath"
#
AC_MSG_CHECKING([for iconv input type])
AC_CACHE_VAL(xdvi_cv_iconv_char_pptr_type,
[AC_TRY_RUN(
[
/* iconv() definitions may differ depending on following macros ... */
#ifdef __hpux
/* On HP-UX 10.10 B and 20.10, compiling with _XOPEN_SOURCE + ..._EXTENDED
 * leads to poll() not realizing that a file descriptor is writable in psgs.c.
 */
# define _HPUX_SOURCE	1
#else
# ifndef _XOPEN_SOURCE
# define _XOPEN_SOURCE	600
# endif
# define _XOPEN_SOURCE_EXTENDED	1
# define __EXTENSIONS__	1	/* needed to get struct timeval on SunOS 5.5 */
# define _SVID_SOURCE	1	/* needed to get S_IFLNK in glibc */
# define _BSD_SOURCE	1	/* needed to get F_SETOWN in glibc-2.1.3 */
#endif

#include <iconv.h>
    
    // Define two iconv_adapter() functions, one with a const char **, the other with
    // a char ** argument. The compiler will figure out which one to use, and we can
    // check the exit value to see which it was.
    // Idea taken from http://gcc.gnu.org/ml/libstdc++/2000-11/msg00127.html

    typedef const char ** ConstCharPtrT;
    typedef char ** CharPtrT;

    inline int
    iconv_adapter(size_t(*iconv_func)(iconv_t, ConstCharPtrT, size_t *, char**, size_t*),
                  iconv_t cd, char **inbuf, size_t *inbytesleft,
                  char **outbuf, size_t *outbytesleft)
    {
        return 1; // 1 = false = const char **
    }
    
    
    inline int
    iconv_adapter(size_t(*iconv_func)(iconv_t, CharPtrT, size_t *, char**, size_t*),
                  iconv_t cd, char **inbuf, size_t *inbytesleft,
                  char **outbuf, size_t *outbytesleft)
    {
        return 0; // 0 = true = char **
    }
    
    
    int main(void)
    {
        iconv_t testconv = (iconv_t)-1;
        char *ptr1 = 0;
        char *ptr2 = 0;
        size_t len1 = 0;
        size_t len2 = 0;
        
        return iconv_adapter(iconv, testconv,
    			    (char**)&ptr1, &len1,
    		  	    (char**)&ptr2, &len2);
    }
],
[xdvi_cv_iconv_char_pptr_type="char_pptr"],
[xdvi_cv_iconv_char_pptr_type="const_char_pptr"],
# `correct' default for cross-compiling ...
[xdvi_cv_iconv_char_pptr_type="char_pptr"])])
#
LIBS="$xdvi_iconv_save_libs"
CXXFLAGS="$xdvi_iconv_save_cxxflags"
LDFLAGS="$xdvi_iconv_save_ldflags"
#
if test $xdvi_cv_iconv_char_pptr_type = "const_char_pptr"; then
  AC_DEFINE([ICONV_CHAR_PPTR_TYPE], [const char **],
            [Define the type of the iconv input string (char ** or const char **)])
  AC_MSG_RESULT([const char **])
else
  AC_DEFINE([ICONV_CHAR_PPTR_TYPE], [char **])
  AC_MSG_RESULT([char **])
fi
AC_LANG_RESTORE]
#AC_LANG_POP(C++) # in newer autoconf
)

