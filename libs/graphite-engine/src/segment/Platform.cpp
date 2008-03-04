#include "GrPlatform.h"

#ifdef _WIN32
#include "windows.h"

namespace gr
{

size_t Platform_UnicodeToANSI(const utf16 * prgchwSrc, size_t cchwSrc, char * prgchsDst, size_t cchsDst)
{
	return ::WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)prgchwSrc, cchwSrc, prgchsDst, cchsDst, NULL, NULL);
}

size_t Platform_AnsiToUnicode(const char * prgchsSrc, size_t cchsSrc, utf16 * prgchwDst, size_t cchwDst)
{
	return ::MultiByteToWideChar(CP_ACP, 0, prgchsSrc, cchsSrc, (LPWSTR)prgchwDst, cchwDst);
}

size_t Platform_8bitToUnicode(int nCodePage, const char * prgchsSrc, int cchsSrc,
	utf16 * prgchwDst, int cchwDst)
{
	return ::MultiByteToWideChar(nCodePage, 0, prgchsSrc, cchsSrc, (LPWSTR)prgchwDst, cchwDst);
}

size_t utf8len(const char *s)
{
	return mbstowcs(NULL,s,0);
}

utf16 *utf16cpy(utf16 *dest, const utf16 *src)
{
	return (utf16*)wcscpy((wchar_t*)dest, (const wchar_t*)src);
}

utf16 *utf16ncpy(utf16 *dest, const utf16 *src, size_t n)
{
	return (utf16*)wcsncpy((wchar_t*)dest, (const wchar_t*)src, n);
}

utf16 *utf16ncpy(utf16 *dest, const char *src, size_t n)
{
	#ifdef UTF16DEBUG
	std::cerr << "utf16ncpy8: " << src << std::endl;
	#endif
	Platform_AnsiToUnicode(src, strlen(src), dest, n);
	return dest;
}


size_t utf16len(const utf16 *s)
{
	return wcslen((const wchar_t*)s);
}

int utf16cmp(const utf16 *s1, const utf16 *s2)
{
return wcscmp((const wchar_t*)s1, (const wchar_t*)s2);
}

int utf16ncmp(const utf16 *s1, const utf16 *s2, size_t n)
{
	return wcsncmp((const wchar_t*)s1, (const wchar_t*)s2, n);
}

int utf16cmp(const utf16 *s1, const char *s2)
{
	while (*s1 && s2)
	{
		if (*s1 < *s2)
		{
			return -1;
		}
		if (*s1 > *s2)
		{
			return 1;
		}
		*s1++;
		*s2++;
	}
	if (*s1) return -1;
	else if (*s2) return 1;
	return 0;
}

}
#else // not _WIN32

#include <iostream>
#include <stdlib.h>
#include <string.h>

#ifndef HAVE_FABSF
float fabsf(float x)
{
    return (x < 0.0f) ? -x : x;
}
#endif

namespace gr
{

size_t utf16len(const utf16 *s)
{
	// assumes NULL terminated strings
	const utf16 *start = s;
	for (; *s; ++s);
	
	return s - start;
}

} // namespace gr
#endif // _WIN32

