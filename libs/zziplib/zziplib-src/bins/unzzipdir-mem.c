/*
 *	Copyright (c) 2003 Guido Draheim <guidod@gmx.de>
 *      Use freely under the restrictions of the ZLIB license.
 *
 *      This file is used as an example to clarify zzipmmap api usage.
 */

#include <zzip/memdisk.h>
#include <zzip/__debug.h>
#include <zzip/__fnmatch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unzzipdir-zip.h"
#include "unzzip-states.h"

#ifdef ZZIP_HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef ZZIP_HAVE_IO_H
#include <io.h>
#endif

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
    ZZIP_MEM_DISK* disk;
    
    if (argc == 1)
    {
        printf (__FILE__" version "ZZIP_PACKAGE" "ZZIP_VERSION"\n");
        return EXIT_OK; /* better provide an archive argument */
    }
    
    disk = zzip_mem_disk_open (argv[1]);
    if (! disk) {
        DBG3("disk_open failed [%i] %s", errno, strerror(errno));
	perror(argv[1]);
	return exitcode(errno);
    }

    if (argc == 2)
    {  /* list all */
	ZZIP_MEM_ENTRY* entry = zzip_mem_disk_findfirst(disk);
	DBG2("findfirst %p", entry);
	for (; entry ; entry = zzip_mem_disk_findnext(disk, entry))
	{
	    char* name = zzip_mem_entry_to_name (entry);
	    long long usize = entry->zz_usize;
	    if (!verbose)
	    {
		printf ("%22lli %s\n", usize, name);
	    } else 
	    {
		long long csize = entry->zz_csize;
		unsigned compr = entry->zz_compr;
        	const char* defl = (compr < sizeof(comprlevel)) ? comprlevel[compr] : "(redu)";
		printf ("%lli/%lli %s %s\n", csize, usize, defl, name);
	    }
	}
    }
    else if (argc == 3)
    {  /* list from one spec */
	ZZIP_MEM_ENTRY* entry = 0;
	while ((entry = zzip_mem_disk_findmatch(disk, argv[2], entry, 0, 0)))
	{
	    char* name = zzip_mem_entry_to_name (entry);
	    long long usize = entry->zz_usize;
	    if (!verbose)
	    {
		printf ("%22lli %s\n", usize, name);
	    } else 
	    {
		long long csize = entry->zz_csize;
		unsigned compr = entry->zz_compr;
		const char* defl = (compr < sizeof(comprlevel)) ? comprlevel[compr] : "(redu)";
		printf ("%lli/%lli %s %s\n", csize, usize, defl, name);
	    }
	}
    }
    else
    {   /* list only the matching entries - in order of zip directory */
	ZZIP_MEM_ENTRY* entry = zzip_mem_disk_findfirst(disk);
	for (; entry ; entry = zzip_mem_disk_findnext(disk, entry))
	{
	    char* name = zzip_mem_entry_to_name (entry);
	    for (argn=1; argn < argc; argn++)
	    {
		if (! _zzip_fnmatch (argv[argn], name, 
		      _zzip_FNM_NOESCAPE|_zzip_FNM_PATHNAME|_zzip_FNM_PERIOD))
		{
		    char* name = zzip_mem_entry_to_name (entry);
		    long long usize = entry->zz_usize;
		    if (!verbose)
		    {
			printf ("%22lli %s\n", usize, name);
		    } else 
		    {
			long long csize = entry->zz_csize;
			unsigned compr = entry->zz_compr;
			const char* defl = (compr < sizeof(comprlevel)) ? comprlevel[compr] : "(redu)";
	    		printf ("%lli/%lli %s %s\n", csize, usize, defl, name);
	    	    }
		    break; /* match loop */
		}
	    }
	}
    }
    zzip_mem_disk_close(disk);
    return EXIT_OK;
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
