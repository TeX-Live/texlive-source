/* file-p.c: file predicates.

   Copyright 1992, 1993, 1994, 2008, 2015, 2016 Karl Berry.

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

#include <kpathsea/xstat.h>


/* Test whether FILENAME1 and FILENAME2 are actually the same file.  If
   stat fails on either of the names, we return false, without error.  */

boolean
same_file_p (const_string filename1,  const_string filename2)
{
#ifdef _WIN32
    intptr_t handle1, handle2;
    FILE *f1, *f2;
    BY_HANDLE_FILE_INFORMATION fileinfo1, fileinfo2;

    f1 = fopen(filename1, "r");
    if(!f1) return false;
    f2 = fopen(filename2, "r");
    if(!f2) {
      fclose(f1);
      return false;
    }

    handle1 = _get_osfhandle(fileno(f1));
    handle2 = _get_osfhandle(fileno(f2));

    if (!GetFileInformationByHandle((HANDLE)handle1, &fileinfo1)) {
      fclose(f1);
      fclose(f2);
      return false;
    }

    if (!GetFileInformationByHandle((HANDLE)handle2, &fileinfo2)) {
      fclose(f1);
      fclose(f2);
      return false;
    }

    fclose(f1);
    fclose(f2);
  
    if (fileinfo1.dwVolumeSerialNumber == fileinfo2.dwVolumeSerialNumber &&
      fileinfo1.nFileIndexHigh == fileinfo2.nFileIndexHigh &&
      fileinfo1.nFileIndexLow == fileinfo2.nFileIndexLow)
      return true;
    else
      return false;
#else
    struct stat sb1, sb2;
    /* These are put in variables only so the results can be inspected
       under gdb.  */
    int r1 = stat (filename1, &sb1);
    int r2 = stat (filename2, &sb2);

    return r1 == 0 && r2 == 0 ? SAME_FILE_P (sb1, sb2) : false;
#endif
}
