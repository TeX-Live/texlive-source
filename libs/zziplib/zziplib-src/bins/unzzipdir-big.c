/*
 *	Copyright (c) 2003 Guido Draheim <guidod@gmx.de>
 *      Use freely under the restrictions of the ZLIB license.
 *
 *      This file is used as an example to clarify zzipfseeko api usage.
 */

#define _ZZIP_ENTRY_STRUCT 1

#include <zzip/fseeko.h>
#include <zzip/fetch.h>
#include <zzip/__fnmatch.h>
#include <stdlib.h>
#include <string.h>
#include "unzzipdir-zip.h"
#include "unzzip-states.h"

static const char* comprlevel[] = {
    "stored",   "shrunk",   "redu:1",   "redu:2",   "redu:3",   "redu:4",
    "impl:N",   "toknze",   "defl:N",   "defl:B",   "impl:B" };

static int exitcode(int e)
{
    return EXIT_ERRORS;
}

static int 
unzzip_list (int argc, char ** argv, int verbose)
{
    int argn;
    FILE* disk;

    disk = fopen (argv[1], "rb");
    if (! disk) {
	perror(argv[1]);
	return exitcode(errno);
    }

    if (argc == 2)
    {  /* print directory list */
	ZZIP_ENTRY* entry = zzip_entry_findfirst(disk);
	for (; entry ; entry = zzip_entry_findnext(entry))
	{
	    char* name = zzip_entry_strdup_name (entry);
	    unsigned compr = zzip_entry_compr(entry);
            const char* defl = (compr < sizeof(comprlevel)) ? comprlevel[compr] : "(redu)";
	    printf (" %s %s\n", defl, name);
	    free (name);
	}
	return 0;
    }

    for (argn=1; argn < argc; argn++)
    {   /* list only the matching entries - each in order of commandline */
	ZZIP_ENTRY* entry = zzip_entry_findfirst(disk);
	for (; entry ; entry = zzip_entry_findnext(entry))
	{
	    char* name = zzip_entry_strdup_name (entry);
	    unsigned compr = zzip_entry_compr(entry);
            const char* defl = (compr < sizeof(comprlevel)) ? comprlevel[compr] : "(redu)";
	    printf (" %s %s\n", defl, name);
	    free (name);
	}
    }
    return 0;
} 

int 
unzzip_long_list (int argc, char ** argv)
{
    return unzzip_list(argc, argv, 1);
}

int 
unzzip_show_list (int argc, char ** argv)
{
    return unzzip_list(argc, argv, 0);
}

/* 
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */
