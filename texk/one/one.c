#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <zlib.h>
#include <png.h>

int
main (int argc, char **argv)
{
  printf ("Hello, this is progone,"
            " using libpng %d (" PNG_LIBPNG_VER_STRING ") and zlib %s (" ZLIB_VERSION ")\n",
          png_access_version_number (), zlibVersion ());
  return 0;
}
