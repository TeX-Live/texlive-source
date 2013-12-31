/* knj.h: check for 2-Byte Kanji (CP 932, SJIS) codes.

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

#ifndef KPATHSEA_KNJ_H
#define KPATHSEA_KNJ_H

#include <kpathsea/c-proto.h>
#include <kpathsea/types.h>

#ifdef __cplusplus
extern "C" {
#endif

extern KPSEDLL int is_cp932_system;
extern KPSEDLL int file_system_codepage;

extern KPSEDLL int isknj(int c);
extern KPSEDLL int isknj2(int c);

extern KPSEDLL wchar_t* get_wstring_from_mbstring(int cp, const char *mbstr, wchar_t *wstr);
extern KPSEDLL char* get_mbstring_from_wstring(int cp, const wchar_t *wstr, char *mbstr);
extern KPSEDLL FILE* fsyscp_xfopen(const char *filename, const char *mode);
extern KPSEDLL FILE* fsyscp_fopen(const char *filename, const char *mode);
extern KPSEDLL void get_command_line_args_utf8(const_string enc, int *p_ac, string **p_av);

#ifdef __cplusplus
}
#endif

/* True if P points to a 2-Byte Kanji (CP 932, SJIS) code.  */
#define IS_KANJI(p) is_cp932_system && isknj(*(p)) && isknj2(*(p+1))

/* Get wide string from multibyte string in UTF-8 */
#define get_wstring_from_utf8(mb,w) get_wstring_from_mbstring(CP_UTF8,mb,w)
/* Get multibyte string in UTF-8 from wide string */
#define get_utf8_from_wstring(w,mb) get_mbstring_from_wstring(CP_UTF8,w,mb)
/* Get wide string from multibyte string in file system codepage */
#define get_wstring_from_fsyscp(mb,w) get_wstring_from_mbstring(file_system_codepage,mb,w)
/* Get multibyte string in file system codepage from wide string */
#define get_fsyscp_from_wstring(w,mb) get_mbstring_from_wstring(file_system_codepage,w,mb)

#endif /* not KPATHSEA_KNJ_H */
