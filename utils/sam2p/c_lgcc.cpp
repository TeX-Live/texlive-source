/* c_lgcc.cpp -- make C++ programs linkable with gcc
 * by pts@fazekas.hu at Tue Sep  3 18:31:58 CEST 2002
 *
 * This file is not sam2p-specific. See also AC_PTS_GCC_LINKS_CXX in file
 * aclocal.m4.
 */

#include <stdio.h>
#include <stdlib.h>

/* vvv make these a macro to make them fast */
#define XMALLOC_CODE() (unsigned len) { \
  void *p=malloc(len); \
  if (p==0) { fputs("out of memory\n",stderr); abort(); } \
  return p; \
}
#define XFREE_CODE() (void* p) { if (p!=0) free(p); }

/* Sat Jul  6 16:39:19 CEST 2002
 * empirical checkerg++ helper routines for gcc version 2.95.2 20000220 (Debian GNU/Linux)
 */
#ifdef __CHECKER__
void* __builtin_vec_new XMALLOC_CODE()
void  __builtin_vec_delete XFREE_CODE()
void* __builtin_new XMALLOC_CODE()
void  __builtin_delete XFREE_CODE()
void  __rtti_user() { abort(); }
void  __rtti_si() { abort(); }
void  terminate() { abort(); }
/* void* __pure_virtual=0; -- doesn't work */
extern "C" void __pure_virtual(); void __pure_virtual() { abort(); }

#else

/* Tue Sep  3 18:24:26 CEST 2002
 * empirical g++-3.2 helper routines for gcc version 3.2.1 20020830 (Debian prerelease)
 */
void* operator new      XMALLOC_CODE()
void* operator new[]    XMALLOC_CODE()
void  operator delete   XFREE_CODE()
void  operator delete[] XFREE_CODE()
void* __cxa_pure_virtual=0;

#endif
