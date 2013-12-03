/* knj.c: check for 2-Byte Kanji (CP 932, SJIS) codes.

   Copyright 2010, 2011 Akira Kakuto.
   Copyright 2013 TANAKA Takuji.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library; if not, see <http://www.gnu.org/licenses/>.  */

#include <kpathsea/config.h>
#include <kpathsea/debug.h>

int is_cp932_system, file_system_codepage;

int isknj(int c)
{
  c &= 0xff;
  switch (is_cp932_system) {
  case 932:
    return((c>=0x81 && c<=0x9f) || (c>=0xe0 && c<=0xfc));
  case 936:
    return(c>=0x81 && c<=0xfe);
  case 950:
    return((c>=0xa1 && c<=0xc6) || (c>=0xc9 && c<=0xf9));
  default:
    return(0);
  }
}

int isknj2(int c)
{
  c &= 0xff;
  switch (is_cp932_system) {
  case 932:
    return(c>=0x40 && c<=0xfc && c!=0x7f);
  case 936:
    return(c>=0x40 && c<=0xfe && c!=0x7f);
  case 950:
    return((c>=0x40 && c<=0x7e) || (c>=0xa1 && c<=0xfe));
  default:
    return(0);
  }
}

/*
  Get wide string from multibyte string.
*/
wchar_t *
get_wstring_from_mbstring(int cp, const char *mbstr, wchar_t *wstr)
{
  int len;

  len = MultiByteToWideChar(cp, 0, mbstr, -1, wstr, 0);
  if (len==0) {
    FATAL("cannot convert string to wide string");
  }
  if (wstr==NULL) {
    wstr = xmalloc(sizeof(wchar_t)*(len+1));
  }
  len = MultiByteToWideChar(cp, 0, mbstr, -1, wstr, len+1);
  if (len==0) {
    FATAL("cannot convert multibyte string to wide string");
  }
  return wstr;
}

/*
  Get multibyte string from wide string.
*/
char *
get_mbstring_from_wstring(int cp, const wchar_t *wstr, char *mbstr)
{
  int len;

  len = WideCharToMultiByte(cp, 0, wstr, -1, mbstr, 0, NULL, NULL);
  if (len==0) {
    FATAL("cannot convert string to wide string");
  }
  if (mbstr==NULL) {
    mbstr = xmalloc(len+1);
  }
  len = WideCharToMultiByte(cp, 0, wstr, -1, mbstr, len+1, NULL, NULL);
  if (len==0) {
    FATAL("cannot convert wide string to multibyte string");
  }
  return mbstr;
}

/*
  xfopen by file system codepage
*/
FILE *
fsyscp_xfopen (const char *filename, const char *mode)
{
    FILE *f;
    wchar_t *fnamew, modew[4];
#if defined (KPSE_COMPAT_API)
    kpathsea kpse;
#endif
    assert(filename && mode);

    fnamew = get_wstring_from_fsyscp(filename, fnamew=NULL);
    get_wstring_from_fsyscp(mode, modew);
    f = _wfopen(fnamew, modew);
    if (f == NULL)
        FATAL_PERROR(filename);
#if defined (KPSE_COMPAT_API)
    kpse = kpse_def;
    if (KPATHSEA_DEBUG_P (KPSE_DEBUG_FOPEN)) {
        DEBUGF_START ();
        fprintf (stderr, "fsyscp_xfopen(%s [", filename);
        WriteConsoleW( GetStdHandle( STD_ERROR_HANDLE ), fnamew, wcslen( fnamew ), NULL, NULL );
        fprintf (stderr, "], %s) => 0x%lx\n", mode, (unsigned long) f);
        DEBUGF_END ();
    }
#endif
    free(fnamew);

    return f;
}
