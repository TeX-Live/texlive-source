
#include <kpathsea/kpathsea.h>

#include "dirutil.h"

/* check a directory */
int
is_dir (char *buff)
{
  HANDLE h;
  WIN32_FIND_DATA w32fd;

  if (((h = FindFirstFile (buff, &w32fd))
       != INVALID_HANDLE_VALUE) &&
      (w32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
    FindClose (h);
    return (1);
  } else {
    FindClose (h);
    return (0);
  }
}

/* make a directory */
int
make_dir (char *buff)
{
  if (_mkdir (buff)) {
    fprintf(stderr, "mkdir %s error.\n", buff);
    return (1);
  }
  if (_chmod (buff, _S_IREAD | _S_IWRITE)) {
    fprintf(stderr, "chmod %s failed.\n", buff);
    return (1);
  }
  return (0);
}

int
make_dir_p(char *buff)
{
  int  ret = 0;
  int  i = 0;
  char *p = buff;

  while (1) {
    if(*p == '\0') {
      ret = 0;
      if(!is_dir(buff)) {
        if(make_dir(buff)) {
          ret = 1;
        }
      }
      break;
    }
    if(*p == '/' && (i > 0 && *(p-1) != ':')) {
      *p = '\0';
      if(!is_dir(buff)) {
        if(make_dir(buff)) {
          ret = 1;
          *p = '/';
          break;
        }
      }
      *p = '/';
    }
    p++;
    i++;
  }
  return ret;
}
