/*
 *	Copyright (c) 2003 Guido Draheim <guidod@gmx.de>
 *      Use freely under the restrictions of the ZLIB license.
 *
 *      This file is used as an example to clarify zzip api usage.
 */

#include <zzip/zzip.h>
#include <stdio.h>
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
    printf (__FILE__" version "ZZIP_PACKAGE" "ZZIP_VERSION"\n");
    return 0;
}

static int unzzip_help(void)
{
    printf (usage);
    return 0;
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
