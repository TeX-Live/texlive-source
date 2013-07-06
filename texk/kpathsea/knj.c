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

int is_cp932_system;

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
