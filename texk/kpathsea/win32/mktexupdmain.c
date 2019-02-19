/* mktexupdmain.c

   Copyright 2000, 2019 Akira Kakuto.

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

#include <kpathsea/kpathsea.h>
#include "mktex.h"

#define TBUF 512

int main(int argc, char **argv)
{
  char dir[TBUF];
  char file[TBUF];
  char path[TBUF];
  size_t i;

  kpse_set_program_name (argv[0], NULL);
  if(argc != 3) {
    fprintf(stderr, "%s:: usage: %s DIR FILE\n", argv[0], argv[0]);
    return 1;
  }
  if (strlen(argv[1]) + strlen(argv[2]) > TBUF - 2) {
    fprintf (stderr, "Too long a string.\n");
    exit (100);
  }
  strcpy(dir, argv[1]);
  strcpy(file, argv[2]);
  normalize (dir);
  i = strlen(dir);
  while(dir[i-1] == '/')
    i--;
  dir[i] = '\0';
  strcpy(path, dir);
  strcat(path, "/");
  strcat(path, file);
  mktexupd(path);
  return 0;
}
