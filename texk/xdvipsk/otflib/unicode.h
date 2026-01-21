/* This is xdvipsk, an eXtended version of dvips(k) by Tomas Rokicki.

	Copyright (C) 2016 by VTeX Ltd (www.vtex.lt),
	the xdvipsk project team - Sigitas Tolusis and Arunas Povilaitis.

    Program original code copyright (C) 2007-2014 by Jin-Hwan Cho and 
	Shunsaku Hirata, the dvipdfmx project team.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
*/

#ifndef _UNICODE_H_
#define _UNICODE_H_

/* UCS -> UTF-8 */
#define UCStoUTF8B1(x)  (0xc0 + (((x) >>  6) & 0x1f))
#define UCStoUTF8B2(x)  (0x80 + (((x)      ) & 0x3f))

#define UCStoUTF8C1(x)  (0xe0 + (((x) >> 12) & 0x0f))
#define UCStoUTF8C2(x)  (0x80 + (((x) >>  6) & 0x3f))
#define UCStoUTF8C3(x)  (0x80 + (((x)      ) & 0x3f))

#define UCStoUTF8D1(x)  (0xf0 + (((x) >> 18) & 0x07))
#define UCStoUTF8D2(x)  (0x80 + (((x) >> 12) & 0x3f))
#define UCStoUTF8D3(x)  (0x80 + (((x) >>  6) & 0x3f))
#define UCStoUTF8D4(x)  (0x80 + (((x)      ) & 0x3f))

/* UTF-32 over U+FFFF -> UTF-16 surrogate pair */
#define UTF32toUTF16HS(x)  (0xd800 + (((x-0x10000) >> 10) & 0x3ff))
#define UTF32toUTF16LS(x)  (0xdc00 + (  x                 & 0x3ff))

extern unsigned int* uni32_2_utf16(unsigned long ucode, int* p_ucnt);
extern unsigned long *utf16_2_uni32(const unsigned int *tounicode, int ucnt, int *p_uni32_cnt);

extern int UC_sput_UTF16BE  (long ucv, unsigned char **dstpp, unsigned char *endptr);
extern int UC_is_valid      (long ucv);

#endif /* _UNICODE_H_ */
