/* knj.h: check for 2-Byte Kanji (CP 932, SJIS) codes.

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

#ifndef KPATHSEA_KNJ_H
#define KPATHSEA_KNJ_H

extern KPSEDLL int is_cp932_system;

extern KPSEDLL int isknj(int c);
extern KPSEDLL int isknj2(int c);

/* True if P points to a 2-Byte Kanji (CP 932, SJIS) code.  */
#define IS_KANJI(p) is_cp932_system && isknj(*(p)) && isknj2(*(p+1))

#endif /* not KPATHSEA_KNJ_H */
