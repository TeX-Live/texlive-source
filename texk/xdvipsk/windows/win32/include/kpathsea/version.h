/* version.h: version number for libkpathsea.

   Copyright 2009 Peter Breitenlohner.

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

#ifndef KPATHSEA_VERSION_H
#define KPATHSEA_VERSION_H

#include <kpathsea/c-proto.h>
#include <kpathsea/types.h>

#ifdef __cplusplus
extern "C" {
#endif

extern KPSEDLL const char *kpathsea_version_string;

extern KPSEDLL const char *kpathsea_bug_address;

#ifdef __cplusplus
}
#endif

#endif /* not KPATHSEA_VERSION_H */
