
#include <kpathsea/kpathsea.h>

#include "dirutil.h"

/* check a directory */
int
is_dir (char *buff)
{
  struct stat stats;

  return stat (buff, &stats) == 0 && S_ISDIR (stats.st_mode);
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
