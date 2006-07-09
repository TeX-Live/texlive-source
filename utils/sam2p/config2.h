#ifndef CONFIG2_H
#define CONFIG2_H 1

#include <assert.h> /* NDEBUG==1 means: no assertions */
#if NDEBUG
#  define ASSERT_SIDE(x) x /* assert with side effect */
#  define ASSERT_SIDE2(x,y) x /* assert with side effect */
#else
#  define ASSERT_SIDE(x) assert(x)
#  define ASSERT_SIDE2(x,y) assert(x y)
#endif

#if _MSC_VER > 1000
#  include "config-vc6.h"
#else
#  include "config.h"
#endif

#if 0
/* vvv i386 Debian slink gcc */
#define SIZEOF_SHORT 2
#define SIZEOF_INT 4
#define SIZEOF_LONG 4
#define SIZEOF_LONG_LONG 8
#endif

#if SIZEOF_CHAR_P!=SIZEOF_LONG && SIZEOF_INT>=4
  typedef unsigned slen_t;
  typedef signed slendiff_t;
# define SIZEOF_SLEN_T SIZEOF_INT
# define SLEN_P ""
#else
  typedef unsigned long slen_t;
  typedef signed   long slendiff_t;
# define SIZEOF_SLEN_T SIZEOF_LONG
# define SLEN_P "l"
#endif

#if SIZEOF___INT64>SIZEOF_LONG
#  define HAVE_LONG_LONG 1
#  define PTS_CFG_LONGEST __int64 /* may be used as signed CFG_LONGEST */
#  define SIZEOF_CFG_LONGEST SIZEOF___INT64
#elif SIZEOF_LONG_LONG>SIZEOF_LONG
#  define HAVE_LONG_LONG 1
#  define PTS_CFG_LONGEST long long /* may be used as signed CFG_LONGEST */
#  define SIZEOF_CFG_LONGEST SIZEOF_LONG_LONG
#elif SIZEOF_VERY_LONG>SIZEOF_LONG
#  define HAVE_LONG_LONG 1
#  define PTS_CFG_LONGEST very long /* may be used as signed CFG_LONGEST */
#  define SIZEOF_CFG_LONGEST SIZEOF_VERY_LONG
#else
#  undef HAVE_LONG_LONG
#  define PTS_CFG_LONGEST long
#  define SIZEOF_CFG_LONGEST SIZEOF_LONG
#endif

#define NULLP ((void*)0)

#define USE_IN_TIFF 1
#define USE_IN_JPEG 1
#define USE_IN_PNG 1
#define USE_IN_PCX 1
#define USE_IN_XPM 1
/* #define USE_IN_GIF 1 -- defined in */
#define USE_IN_PNM 1
#define USE_IN_BMP 1
#define USE_IN_PIP 1 /* simple paletted format defined by pts, but not implemented */
#define USE_IN_TGA 1
#define USE_IN_LBM 1
#define USE_IN_JAI 1
#define USE_IN_PDF 1
#define USE_IN_PS 1
/* #define USE_OUT_GIF 1 -- moved to config.h */
#define USE_IN_XPM_MAPPING 1
#define USE_BIG_MEMORY 1
#define USE_DICT_MAPPING 1
/* at Tue Jan 18 16:18:34 CET 2005 */
#define USE_ZLIB_MEM 1

/** Quotes a filename to be displayed in error messages */ 
/* #define FNQ(str) (str) */
#define FNQ(filename) SimBuffer::B().appendFnq(SimBuffer::Static(filename))
#define FNQ2(beg,len) SimBuffer::B().appendFnq(SimBuffer::Static(beg,len),true)
#define FNQ2STDOK(beg,len) SimBuffer::B().appendFnq(SimBuffer::Static(beg,len),false)


#define param_assert assert

#undef  __VA_START_STDC__
#ifdef __STDC__
# define __VA_START_STDC__ 1
#endif
#if _MSC_VER > 1000
# undef  __PROTOTYPES__
# define __PROTOTYPES__ 1
# undef  __VA_START_STDC__
# define __VA_START_STDC__ 1
# pragma warning(disable: 4127) /* conditional expression is constant */
# pragma warning(disable: 4244) /* =' : conversion from 'int ' to 'unsigned char ', possible loss of data */
# pragma warning(disable: 4250) /* 'SimBuffer::B' : inherits 'SimBuffer::Flat::getLength' via dominance */
# pragma warning(disable: 4514) /* 'each_char' : unreferenced inline function has been removed */
# pragma warning(disable: 4512) /* 'PipeE' : assignment operator could not be generated */
# pragma warning(disable: 4310) /* cast truncates constant value */
# pragma warning(disable: 4355) /* 'this' : used in base member initializer list */
#endif

#if __VA_START_STDC__
# define PTS_va_start(ap, fmt) va_start(ap, fmt)
#else
# define PTS_va_start(ap, fmt) va_start(ap)
#endif

#if HAVE_STATIC_CONST
# define BEGIN_STATIC_ENUM(basetype,type) typedef basetype type; static const type
# define BEGIN_STATIC_ENUM1(type) static const type
# define END_STATIC_ENUM() ;
#else /* VC6.0 */
# define BEGIN_STATIC_ENUM(basetype,type) enum type {
# define BEGIN_STATIC_ENUM1(type) enum {
# define END_STATIC_ENUM() };
#endif

#if ((defined(__STDC__) || defined(__PROTOTYPES__)) && !defined(NO_PROTO)) || defined(__cplusplus)
# define _(args) args
# define OF(args) args
# define ___(arg2s,arg1s,argafter) arg2s /* Dat: no direct comma allowed in args :-( */
#else
# define _(args) ()
# define OF(args) ()
# define ___(arg2s,arg1s,argafter) arg1s argafter /* Dat: no direct comma allowed in args :-( */
#endif

#if SIZEOF_BOOL!=1
#define bool PTS_bool
#define true 1
#define false 0
typedef unsigned char bool;
#endif

#ifdef const
#  undef const
#  define PTS_const
#  undef HAVE_CONST 
#else
#  define PTS_const const
#  define HAVE_CONST 1
#endif

#ifdef volatile
#  undef volatile
#  define PTS_volatile
#  undef HAVE_VOLATILE
#else
#  define PTS_volatile volatile
#  define HAVE_VOLATILE 1
#endif

#ifdef NEED_STRING_H
/* We use: memset(), memmove(), strncpy(), memcpy(), memcmp() */
#if HAVE_MEMCMPY_BUILTIN
#  undef memcpy
#  define memcpy dummy_memcpy
#  undef memcmp
#  define memcmp dummy_memcmp
#endif
#ifdef HAVE_strcpy_in_string
#  include <string.h>
#elif HAVE_strcpy_in_strings
#  include <strings.h>
#else
#  error no strcpy
#endif
#if HAVE_MEMCMPY_BUILTIN
#  undef memcmp
#  undef memcpy
#endif
#endif

#ifdef NEED_MALLOC
#if HAVE_malloc_in_stdlib
#  include <stdlib.h>
#elif HAVE_PTS_STDC && HAVE_malloc_in_malloc
#  include <malloc.h>
#elif HAVE_PTS_STDC
#  error malloc() not found
#elif HAVE_PROTOTYPES
  void *malloc(PTS_size_t size);
  void *realloc(void *ptr, PTS_size_t size);
  void free(void *ptr);
#else
  void *malloc();
  void *realloc();
  void free();
#endif
#endif

#ifdef NEED_ALLOCA
/* Make alloca work the best possible way.
 * Imp: AIX requires this to be the 1st thing in the file??
 */
#ifdef __GNUC__
# ifndef atarist
#  ifndef alloca
#   define alloca __builtin_alloca
#  endif
# endif /* atarist */
#elif defined(__BORLANDC__)
# include <malloc.h>
#elif HAVE_ALLOCA_H
# include <alloca.h>
#elif defined(_AIX)
 #pragma alloca /* indented to avoid choking of pre-ANSI compilers */
#elif !defined alloca /* predefined by HP cc +Olibcalls */
char *alloca ();
#endif
#endif /* NEED_ALLOCA */

#ifdef NEED_SIGN_EXTEND_CHAR
/* We remove any previous definition of `SIGN_EXTEND_CHAR',
   since ours (we hope) works properly with all combinations of
   machines, compilers, `char' and `unsigned char' argument types.
   (Per Bothner suggested the basic approach.)  */
#undef SIGN_EXTEND_CHAR
#if HAVE_PTS_STDC
# define SIGN_EXTEND_CHAR(c) ((signed char)(c))
#else  /* not __STDC__ */
/* As in Harbison and Steele.  */
# define SIGN_EXTEND_CHAR(c) ((((unsigned char)(c)) ^ 128) - 128)
#endif
#endif

/* by pts@fazekas.hu at Fri Mar 22 17:01:27 CET 2002 */
#if defined(HAVE_PTS_POPEN_)
#  define HAVE_PTS_POPEN 1
#  define CFG_PTS_POPEN_B ""
#elif defined(HAVE_PTS_POPEN_B)
#  define HAVE_PTS_POPEN 1
#  define CFG_PTS_POPEN_B "b"
#else
#  undef HAVE_PTS_POPEN
#endif

#undef NEED_LONG_LONG
#undef NEED_LONG_DOUBLE

/** Filename, directory and quoting conventions for the host OS.
 * 0: UNIX
 * 1: DOS/Win3.1
 * 2: WinNT
 * 3: Win9X
 * 4: VMS
 * 5: Mac
 * 6...
 *
 * Imp: implement other than UNIX
 */
#define COTY_UNIX  0
#define COTY_DOS   1
#define COTY_WINNT 2
#define COTY_WIN9X 3
#define COTY_VMS   4
#define COTY_MAC   5
#define COTY_OTHER 6

#if HAVE_PTS_SYSTEMF_UNIX
#  define OS_COTY COTY_UNIX
#else
#  if HAVE_PTS_SYSTEMF_WIN32
#    define OS_COTY COTY_WIN9X
#  else
#    define OS_COTY COTY_OTHER
#  endif
#endif

/* vvv Imp: autodetect this */
#if defined(__MSDOS__) || defined(__CYGWIN__) || defined(__MINGW32__) || _MSC_VER > 1000
#  define HAVE_DOS_BINARY 1
#else
#  undef  HAVE_DOS_BINARY
#endif

/* #define PTS_dynamic_cast(ptrtype,ptr) dynamic_cast<ptrtype>(ptr) -- would include special G++ symbols into .o */
#define PTS_dynamic_cast(ptrtype,ptr) ((ptrtype)(ptr))

/** Suppress effect of -Wcast-align warning */
#define PTS_align_cast(ptrtype,ptr) ((ptrtype)(void*)(ptr))
/** Suppress effect of -Wcast-align warning */
#define PTS_calign_cast(ptrtype,ptr) ((ptrtype)(void const*)(ptr))


#undef HAVE_LZW
#if USE_BUILTIN_LZW
#  define HAVE_LZW 1
#else
#  define HAVE_LZW 1 /* try to use external lzw_codec */
#endif
/* #define USE_BUILTIN_FAXE 1 */ /* in config.h */
#undef  USE_BUILTIN_FAXD /* sure */
/* #define USE_BUILTIN_FAXD 1 */ /* in config.h */

#ifndef __cplusplus
#undef true
#undef false
#ifdef __BEOS__
typedef unsigned char bool;
#else
typedef int bool;
#endif
#define true ((bool)1)
#define false ((bool)0)
#endif

/* by pts@fazekas.hu at Tue Sep  3 18:47:47 CEST 2002 */
#if OBJDEP && defined(__cplusplus) && HAVE_PTS_C_LGCC_CPP_REQUIRED
#  warning REQUIRES: c_lgcc.o
#endif
#if OBJDEP && defined(__cplusplus) && HAVE_PTS_C_LGCC3_CPP_REQUIRED
#  warning REQUIRES: c_lgcc3.o
#endif

/* at Wed Aug 21 18:28:24 CEST 2002 */
#if HAVE_getc_in_stdio
#  define MACRO_GETC getc
#else
#  if HAVE_fgetc_in_stdio
#    define MACRO_GETC fgetc
#  else
#    error getc() or fgetc() is required
#  endif
#endif
#if HAVE_putc_in_stdio
#  define MACRO_PUTC putc
#else
#  if HAVE_fputc_in_stdio
#    define MACRO_PUTC fputc
#  else
#    error putc() or fputc() is required
#  endif
#endif



#endif /* config2.h */
