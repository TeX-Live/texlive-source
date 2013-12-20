/* Copyright (C) 2013 Kevin W. Hamlen
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301, USA.
 *
 * The latest version of this program can be obtained from
 * http://songs.sourceforge.net.
 */

#ifndef CHARS_H
#define CHARS_H

#if HAVE_CONFIG_H
#  include "config.h"
#else
#  include "vsconfig.h"
#endif

#if HAVE_STDLIB_H
#  include <stdlib.h>
#endif
#if HAVE_WCHAR_H
#  include <wchar.h>
#endif
#if HAVE_WCTYPE_H
#  include <wctype.h>
#endif

#if HAVE_WCHAR_T

typedef wchar_t WCHAR;

#define wc_lit(c) (L ## c)
#define wc_isalpha(c) iswalpha((c))
#define wc_ispunct(c) iswpunct((c))
#define wc_isupper(c) iswupper((c))
#define wc_isspace(c) iswspace((c))
#define wc_isdigit(c) iswdigit((c))
#define wc_tolower(c) towlower((c))
#define wc_toupper(c) towupper((c))

#define ws_lit(s) ws_lit2(s)
#define ws_lit2(s) (L ## s)
#define ws_strlen(p) wcslen((p))
#define ws_strcmp(p1,p2) wcscmp((p1),(p2))
#define ws_strncmp(p1,p2,n) wcsncmp((p1),(p2),(n))
#define ws_strcpy(p1,p2) wcscpy((p1),(p2))
#define ws_strncpy(p1,p2,n) wcsncpy((p1),(p2),(n))
#define ws_memmove(p1,p2,n) wmemmove((p1),(p2),(n))
#define ws_strchr(p,c) wcschr((p),(c))
#define ws_strspn(p1,p2) wcsspn((p1),(p2))
#define ws_strpbrk(p1,p2) wcspbrk((p1),(p2))
#define ws_strtol(p1,p2,n) wcstol((p1),(p2),(n))
#define ws_coll(p1,p2) wcscoll((p1),(p2))
#define ws_fprintf1(fh,fs,x) fwprintf((fh),(fs),(x))
#define ws_fprintf2(fh,fs,x,y) fwprintf((fh),(fs),(x),(y))
#define ws_fprintf3(fh,fs,x,y,z) fwprintf((fh),(fs),(x),(y),(z))
#define ws_fgets(p,n,fh) fgetws((p),(n),(fh))
#define ws_fputs(s,fh) fputws((s),(fh))
#define ws_fputc(c,fh) fputwc((c),(fh))

#else

#include <ctype.h>
#if HAVE_STRING_H
#  include <string.h>
#elif HAVE_STRINGS_H
#  include <strings.h>
#endif

typedef char WCHAR;

#define wc_lit(c) (c)
#define wc_isalpha(c) isalpha((c))
#define wc_ispunct(c) ispunct((c))
#define wc_isupper(c) isupper((c))
#define wc_isspace(c) isspace((c))
#define wc_isdigit(c) isdigit((c))
#define wc_tolower(c) tolower((c))
#define wc_toupper(c) toupper((c))

#define ws_lit(s) (s)
#define ws_strlen(p) strlen((p))
#define ws_strcmp(p1,p2) strcmp((p1),(p2))
#define ws_strncmp(p1,p2,n) strncmp((p1),(p2),(n))
#define ws_strcpy(p1,p2) strcpy((p1),(p2))
#define ws_strncpy(p1,p2,n) strncpy((p1),(p2),(n))
#define ws_memmove(p1,p2,n) memmove((p1),(p2),(n))
#define ws_strchr(p,c) strchr((p),(c))
#define ws_strspn(p1,p2) strspn((p1),(p2))
#define ws_strpbrk(p1,p2) strpbrk((p1),(p2))
#define ws_strtol(p1,p2,n) strtol((p1),(p2),(n))
#define ws_coll(p1,p2) strcoll((p1),(p2))
#define ws_fprintf1(fh,fs,x) fprintf((fh),(fs),(x))
#define ws_fprintf2(fh,fs,x,y) fprintf((fh),(fs),(x),(y))
#define ws_fprintf3(fh,fs,x,y,z) fprintf((fh),(fs),(x),(y),(z))
#define ws_fgets(p,n,fh) fgets((p),(n),(fh))
#define ws_fputs(s,fh) fputs((s),(fh))
#define ws_fputc(c,fh) fputc((c),(fh))

#endif

#define wc_null wc_lit('\0')
#define wc_hyphen wc_lit('-')
#define wc_apostrophe wc_lit('\'')
#define wc_backquote wc_lit('`')
#define wc_backslash wc_lit('\\')
#define wc_lbrace wc_lit('{')
#define wc_rbrace wc_lit('}')
#define wc_comma wc_lit(',')
#define wc_period wc_lit('.')
#define wc_space wc_lit(' ')
#define wc_newline wc_lit('\n')
#define wc_cr wc_lit('\r')
#define wc_asterisk wc_lit('*')
#define wc_hash wc_lit('#')
#define wc_pipe wc_lit('|')
#define wc_colon wc_lit(':')
#define wc_semicolon wc_lit(';')
#define wc_percent wc_lit('%')
#define wc_tilda wc_lit('~')

#endif

