/* mktex.h

   Copyright 2000, 2015 Akira Kakuto.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library; if not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MKTEX_H
#define MKTEX_H

static inline void
normalize (char *p) {
  for (; *p; p++) {
    if (IS_KANJI(p))
      p++;
    else if (*p == '\\')
      *p = '/';
  }
}

/* dirutil.c */
extern int is_dir (char *buff);
extern int make_dir (char *buff);
extern int make_dir_p (char *buff);

/* getdestdir.c

   from mktexmf:
   argv[0] = "Dummy", argv[1] = "source", argv[2] = path
   from mktexpk:
   argv[0] = "Dummy", argv[1] = "pk", argv[2] = path, argv[3] = mode
   from mktextfm:
   argv[0] = "Dummy", argv[1] = "tfm", argv[2] = path
*/
extern char *getdestdir (int ac, char **av);

/* mkpaths.c */
extern char **mkpaths (int *numptr);

/* mktexupd.c*/
extern void mktexupd (char *s);

#endif
