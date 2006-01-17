#ifndef _SYSTEM_H_

#define _SYSTEM_H_

#include <kpathsea/c-auto.h>
#include <kpathsea/kpathsea.h>

#ifdef WIN32
#  undef ERROR
#  undef NO_ERROR
#  undef RGB
#  undef CMYK
#  undef SETLINEJOIN
#  undef SETMITERLIMIT
#  undef SETLINECAP
#  pragma warning(disable : 4101 4018)
#else
#  define __cdecl
#  define CDECL
#endif /* WIN32 */

#endif /* _SYSTEM_H_ */



