/* Copyright (C) 1997 DJ Delorie, see COPYING.DJ for details */
/* Copyright (C) 1995 DJ Delorie, see COPYING.DJ for details */
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>

#define TOLOWER(c) (isascii(c) && isupper(c) ? tolower (c) : (c))

long
pathconf(const char *path, int name)
{
  switch (name)
  {
  case _PC_LINK_MAX:		return LINK_MAX;
  case _PC_MAX_CANON:		return MAX_CANON;
  case _PC_MAX_INPUT:		return MAX_INPUT;
  case _PC_NAME_MAX: case _PC_PATH_MAX:
    {
      int name_max, path_max;
      int e = errno;
      char *lfnenv = getenv ("LFN");

      if (!lfnenv || TOLOWER (*lfnenv) != 'n')
	{
	  errno = 0;
	  _get_volume_info (path, &name_max, &path_max, 0);
	  if (!errno)
	    {
	      errno = e;
	      return (name == _PC_NAME_MAX) ? name_max : path_max;
	    }
	}
      return (name == _PC_NAME_MAX) ? NAME_MAX : PATH_MAX;
    }
  case _PC_PIPE_BUF:		return PIPE_BUF;
  case _PC_CHOWN_RESTRICTED:	return _POSIX_CHOWN_RESTRICTED;
  case _PC_NO_TRUNC:		return _POSIX_NO_TRUNC;
  case _PC_VDISABLE:		return _POSIX_VDISABLE;

  default:
    errno = EINVAL;
    return -1;
  }
}
