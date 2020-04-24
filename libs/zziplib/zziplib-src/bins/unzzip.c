/*
 *	Copyright (c) 2003 Guido Draheim <guidod@gmx.de>
 *      Use freely under the restrictions of the ZLIB license.
 *
 *      This file is used as an example to clarify zzip api usage.
 */

#include <sys/stat.h>
#include <zzip/zzip.h>
#include <zzip/__string.h>
#include <zzip/__mkdir.h>
#include <zzip/__debug.h>
#include <zzip/file.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unzzipcat-zip.h"
#include "unzzipdir-zip.h"
#include "unzzip-states.h"

static const char usage[] = 
{
    "unzzip <dir>.. \n"
    "  - unzzip the files contained in a zip archive.\n"
    "  -p            print content of files to pipe\n"
    "  -l            list names in archive (short format)\n"
};

static int unzzip_version(void)
{
    printf (__FILE__ " version " ZZIP_PACKAGE_NAME " " ZZIP_PACKAGE_VERSION "\n");
    return 0;
}

static int unzzip_help(void)
{
    printf (usage);
    return 0;
}

/* Functions used by unzzipcat-*.c: */
int exitcode(int e)
{   
    switch (e) 
    {
        case ZZIP_NO_ERROR:
            return EXIT_OK;
        case ZZIP_OUTOFMEM: /* out of memory */
            return EXIT_ENOMEM;
        case ZZIP_DIR_OPEN: /* failed to open zipfile, see errno for details */
            return EXIT_ZIP_NOT_FOUND;
        case ZZIP_DIR_STAT: /* failed to fstat zipfile, see errno for details */
        case ZZIP_DIR_SEEK: /* failed to lseek zipfile, see errno for details */
        case ZZIP_DIR_READ: /* failed to read zipfile, see errno for details */
        case ZZIP_DIR_TOO_SHORT:
        case ZZIP_DIR_EDH_MISSING:
            return EXIT_FILEFORMAT;
        case ZZIP_DIRSIZE:
            return EXIT_EARLY_END_OF_FILE; 
        case ZZIP_ENOENT:
            return EXIT_FILE_NOT_FOUND; 
        case ZZIP_UNSUPP_COMPR:
            return EXIT_UNSUPPORTED_COMPRESSION;
        case ZZIP_CORRUPTED:
        case ZZIP_UNDEF:
        case ZZIP_DIR_LARGEFILE:
            return EXIT_FILEFORMAT;
    }
    return EXIT_ERRORS;
}

/*
 * NAME: remove_dotdotslash
 * PURPOSE: To remove any "../" components from the given pathname
 * ARGUMENTS: path: path name with maybe "../" components
 * RETURNS: Nothing, "path" is modified in-place
 * NOTE: removing "../" from the path ALWAYS shortens the path, never adds to it!
 *	Also, "path" is not used after creating it.
 *	So modifying "path" in-place is safe to do.
 */
static inline void
remove_dotdotslash(char *path)
{
    /* Note: removing "../" from the path ALWAYS shortens the path, never adds to it! */
    char *dotdotslash;
    int warned = 0;

    dotdotslash = path;
    while ((dotdotslash = strstr(dotdotslash, "../")) != NULL)
    {
        /*
         * Remove only if at the beginning of the pathname ("../path/name")
         * or when preceded by a slash ("path/../name"),
         * otherwise not ("path../name..")!
         */
        if (dotdotslash == path || dotdotslash[-1] == '/')
        {
            char *src, *dst;
            if (!warned)
            {
                /* Note: the first time through the pathname is still intact */
                fprintf(stderr, "Removing \"../\" path component(s) in %s\n", path);
                warned = 1;
            }
            /* We cannot use strcpy(), as there "The strings may not overlap" */
            for (src = dotdotslash+3, dst=dotdotslash; (*dst = *src) != '\0'; src++, dst++)
                ;
        }
        else
            dotdotslash +=3;	/* skip this instance to prevent infinite loop */
    }
}

static void makedirs(const char* name)
{
      char* p = strrchr(name, '/');
      if (p) {
          char* dir_name = _zzip_strndup(name, p-name);
          makedirs(dir_name);
          free (dir_name);
      } 
      if (_zzip_mkdir(name, 0775) == -1 && errno != EEXIST)
      {
          DBG3("while mkdir %s : %s", name, strerror(errno));
      }
      errno = 0;
}

FILE* create_fopen(char* name, char* mode, int subdirs)
{
   char name_stripped[PATH_MAX];

   strncpy(name_stripped, name, PATH_MAX);
   remove_dotdotslash(name_stripped);

   if (subdirs)
   {
      char* p = strrchr(name_stripped, '/');
      if (p) {
          char* dir_name = _zzip_strndup(name_stripped, p-name);
          makedirs(dir_name); 
          free (dir_name);
      }
   }
   return fopen(name_stripped, mode);
}

int 
main (int argc, char ** argv)
{
    int argn;
    int exitcode = 0;
    zzip_error_t error;

    if (argc <= 1 || ! strcmp (argv[1], "--help"))
    {
        return unzzip_help();
    }
    if (! strcmp (argv[1], "--version"))
    {
        return unzzip_version();
    }
    if (! strcmp (argv[1], "-l") || ! strcmp(argv[1], "--list"))
    {
        argc -= 1; argv += 1;
        return unzzip_show_list(argc, argv);
    }
    if (! strcmp (argv[1], "-v") || ! strcmp(argv[1], "--versions"))
    {
        if (argc == 2)
            return unzzip_version(); /* compatible with info-zip */
        argc -= 1; argv += 1;
        return unzzip_long_list(argc, argv);
    }
    if (! strcmp (argv[1], "-p") || ! strcmp(argv[1], "--pipe"))
    {
        argc -= 1; argv += 1;
        return unzzip_print(argc, argv);
    }

    if (! strcmp (argv[1], "-"))
    {
        fprintf(stderr, "unknown option %s", argv[1]);
        return EXIT_INVALID_OPTION;
    }
 
    return unzzip_extract(argc, argv);
} 

/* 
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */
