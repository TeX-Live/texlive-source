#ifndef _MSTYPES_H
#define _MSTYPES_H
/********
This header contains the MS Win32 specific types that are still used 
throughout the Graphite code.

It is intended to allow Graphite to build on non-Win32 platforms.  
  
  **Do NOT include this when building against WIN32**
  
  TSE - 15/07/2003
********/

#if defined(_WIN32)
  #error Do not include this header when building against Win32 APIs
#else

#if defined(GR_NAMESPACE)
namespace gr
{
#endif

typedef wchar_t  OLECHAR;

typedef signed long	    HRESULT;

inline long InterlockedIncrement(long *const intr_lck) {
	return ++*intr_lck;
}

inline long InterlockedDecrement(long *const intr_lck) {
	return --*intr_lck;
}

#if defined(GR_NAMESPACE)
}
#endif

#endif // defined(_WIN32)
#endif // include guard
