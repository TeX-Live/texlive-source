/* getdestdir.h

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

   from mktexpk:
   argv[0] = "Dummy", argv[1] = "pk", argv[2] = path, argv[3] = mode
   from mktextfm:
   argv[0] = "Dummy", argv[1] = "tfm", argv[2] = path
*/

extern char *getdestdir (int ac, char **av);
