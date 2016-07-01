/* knj.h: check for 2-Byte Kanji (CP 932, SJIS) codes.

   Copyright 2010, 2014, 2016 Akira Kakuto.
   Copyright 2013, 2014 TANAKA Takuji.

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

#ifdef __cplusplus
extern "C" {
#endif

extern KPSEDLL wchar_t* get_wstring_from_mbstring(int cp, const char *mbstr, wchar_t *wstr);
extern KPSEDLL char* get_mbstring_from_wstring(int cp, const wchar_t *wstr, char *mbstr);
extern KPSEDLL int kpathsea_isknj(kpathsea kpse, int c);
extern KPSEDLL int kpathsea_isknj2(kpathsea kpse, int c);
extern KPSEDLL FILE* kpathsea_fsyscp_xfopen(kpathsea kpse, const char *filename, const char *mode);
extern KPSEDLL FILE* kpathsea_fsyscp_fopen(kpathsea kpse, const char *filename, const char *mode);
extern KPSEDLL FILE* kpathsea_fsyscp_popen(kpathsea kpse, const char *command, const char *mode);
extern KPSEDLL int kpathsea_fsyscp_spawnvp(kpathsea kpse, int mode, const char *command, const char* const *argv);
extern KPSEDLL int kpathsea_fsyscp_system(kpathsea kpse, const char *cmd);
extern KPSEDLL int kpathsea_get_command_line_args_utf8(kpathsea kpse, const char *enc, int *p_ac, char ***p_av);
extern KPSEDLL int kpathsea_win32_getc(kpathsea kpse, FILE *fp);
extern KPSEDLL int kpathsea_win32_ungetc(kpathsea kpse, int c, FILE *fp);
extern KPSEDLL int kpathsea_win32_fputs(kpathsea kpse, const char *str, FILE *fp);
extern KPSEDLL int kpathsea_win32_puts(kpathsea kpse, const char *str);
extern KPSEDLL int kpathsea_win32_vfprintf(kpathsea kpse, FILE *fp, const char *format, va_list argp);
extern KPSEDLL int kpathsea_win32_putc(kpathsea kpse, int c, FILE *fp);
extern KPSEDLL int kpathsea_IS_KANJI(kpathsea kpse, const char *p);
extern KPSEDLL char *kpathsea_get_fsyscp_from_wstring(kpathsea kpse, const wchar_t *w,char *mb);
extern KPSEDLL wchar_t *kpathsea_get_wstring_from_fsyscp(kpathsea kpse, const char *mb,wchar_t *w);

#if defined (KPSE_COMPAT_API)
#define is_cp932_system kpse_def->Is_cp932_system
#define file_system_codepage kpse_def->File_system_codepage

extern KPSEDLL int isknj(int c);
extern KPSEDLL int isknj2(int c);
extern KPSEDLL FILE* fsyscp_xfopen(const char *filename, const char *mode);
extern KPSEDLL FILE* fsyscp_fopen(const char *filename, const char *mode);
extern KPSEDLL FILE* fsyscp_popen(const char *command, const char *mode);
extern KPSEDLL int fsyscp_spawnvp(int mode, const char *command, const char* const *argv);
extern KPSEDLL int fsyscp_system(const char *cmd);
extern KPSEDLL int get_command_line_args_utf8(const char *enc, int *p_ac, char ***p_av);
extern KPSEDLL int win32_getc(FILE *fp);
extern KPSEDLL int win32_ungetc(int c, FILE *fp);
extern KPSEDLL int win32_fputs(const char *str, FILE *fp);
extern KPSEDLL int win32_puts(const char *str);
extern KPSEDLL int win32_vfprintf(FILE *fp, const char *format, va_list argp);
extern KPSEDLL int win32_putc(int c, FILE *fp);
extern KPSEDLL int IS_KANJI(const char *p);
extern KPSEDLL char *get_fsyscp_from_wstring(const wchar_t *w,char *mb);
extern KPSEDLL wchar_t *get_wstring_from_fsyscp(const char *mb,wchar_t *w);
#endif
#ifdef __cplusplus
}
#endif

/* Get wide string from multibyte string in UTF-8 */
#define get_wstring_from_utf8(mb,w) get_wstring_from_mbstring(CP_UTF8,mb,w)
/* Get multibyte string in UTF-8 from wide string */
#define get_utf8_from_wstring(w,mb) get_mbstring_from_wstring(CP_UTF8,w,mb)

#endif /* not KPATHSEA_KNJ_H */
