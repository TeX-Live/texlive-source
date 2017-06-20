/*
 *	Copyright (c) 2003 Guido Draheim <guidod@gmx.de>
 *      Use freely under the restrictions of the ZLIB license.
 *
 *      This file is used as an example to clarify zzip api usage.
 */

#include <zzip/lib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unzzip.h"

#ifdef ZZIP_HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef ZZIP_HAVE_IO_H
#include <io.h>
#endif

#ifdef ZZIP_HAVE_FNMATCH_H
#include <fnmatch.h>
#else
#define fnmatch(x,y,z) strcmp(x,y)
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif


int 
unzzip_list (int argc, char ** argv)
{
    int argn;
    ZZIP_DIR* disk;
    
    if (argc == 1)
    {
        printf (__FILE__" version "ZZIP_PACKAGE" "ZZIP_VERSION"\n");
        return -1; /* better provide an archive argument */
    }
    
    disk = zzip_opendir (argv[1]);
    if (! disk) {
	perror(argv[1]);
	return -1;
    }

    if (argc == 2)
    {  /* list all */
	ZZIP_DIRENT* entry = 0;
	while((entry = zzip_readdir(disk)))
	{
	    char* name = entry->d_name;
	    long long csize = entry->d_csize;
	    long long usize = entry->st_size;
	    char* defl = entry->d_compr ? "deflated" : "stored";
            printf ("%lli/%lli %s %s \n", usize, csize, defl, name);
	}
    }
    else
    {   /* list only the matching entries - in order of zip directory */
	ZZIP_DIRENT* entry = 0;
	while((entry = zzip_readdir(disk)))
	{
	    char* name = entry->d_name;
	    for (argn=1; argn < argc; argn++)
	    {
		if (! fnmatch (argv[argn], name, 
			       FNM_NOESCAPE|FNM_PATHNAME|FNM_PERIOD))
		{
		    long long csize = entry->d_csize;
		    long long usize = entry->st_size;
		    char* defl = entry->d_compr ? "deflated" : "stored";
            	    printf ("%lli/%lli %s %s \n", usize, csize, defl, name);
		    break; /* match loop */
		}
	    }
	}
    }
    zzip_closedir(disk);
    return 0;
} 

/* 
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */
