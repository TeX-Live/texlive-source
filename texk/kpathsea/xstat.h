/* xstat.h: stat with error checking.

   Copyright 1992, 1993, 1994, 2008, 2010, 2011 Karl Berry.

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

#ifndef KPATHSEA_XSTAT_H
#define KPATHSEA_XSTAT_H

#ifdef MAKE_KPSE_DLL /* libkpathsea internal only */

#include <kpathsea/c-proto.h>
#include <kpathsea/c-stat.h>
#include <kpathsea/types.h>

/* Two files are indistinguishable if they are on the same device
   and have the same inode.  This checks two stat buffers for that.  Cf.
   the `same_file_p' routine in file-p.c, declared in kpathlib.h.  */
#define SAME_FILE_P(s1, s2) \
  ((s1).st_ino == (s2).st_ino && (s1).st_dev == (s2).st_dev)

/* Does stat(2) on PATH, and aborts if the stat fails.  */
extern struct stat xstat (const_string path);

/* Ditto, for lstat(2) (except that lstat might not exist).  */
#ifdef S_ISLNK
extern struct stat xlstat (const_string path);
#else
#define xlstat xstat
#endif

#endif /* MAKE_KPSE_DLL */

#endif /* not KPATHSEA_XSTAT_H */
