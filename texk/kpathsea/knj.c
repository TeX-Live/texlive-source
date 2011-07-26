/* knj.c: check for 2-Byte Kanji (CP 932, SJIS) codes.

   Copyright 2010, 2011 Akira Kakuto.

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

KPSEDLL int is_cp932_system;

int isknj(int c)
{
  c &= 0xff;
  return((c>=0x81 && c<=0x9f) || (c>=0xe0 && c<=0xfc));
}

int isknj2(int c)
{
  c &= 0xff;
  return(c>=0x40 && c<=0xfc && c!=0x7f);
}
