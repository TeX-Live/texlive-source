/*
 *	Copyright (c) 2003 Guido Draheim <guidod@gmx.de>
 *      Use freely under the restrictions of the ZLIB license.
 *
 *      This file is used as an example to clarify zzipmmap api usage.
 */

#include <zzip/memdisk.h>
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

static void unzzip_mem_entry_fprint(ZZIP_MEM_DISK* disk, 
				  ZZIP_MEM_ENTRY* entry, FILE* out)
{
    ZZIP_DISK_FILE* file = zzip_mem_entry_fopen (disk, entry);
    if (file) 
    {
	char buffer[1024]; int len;
	while ((len = zzip_mem_disk_fread (buffer, 1024, 1, file)))
	    fwrite (buffer, len, 1, out);
	
	zzip_mem_disk_fclose (file);
    }
}

static void unzzip_mem_disk_cat_file(ZZIP_MEM_DISK* disk, char* name, FILE* out)
{
    ZZIP_DISK_FILE* file = zzip_mem_disk_fopen (disk, name);
    if (file) 
    {
	char buffer[1024]; int len;
	while ((len = zzip_mem_disk_fread (buffer, 1, 1024, file))) 
	{
	    fwrite (buffer, 1, len, out);
	}
	
	zzip_mem_disk_fclose (file);
    }
}

static FILE* create_fopen(char* name, char* mode, int subdirs)
{
   if (subdirs)
   {
      char* p = strrchr(name, '/');
      if (p) {
          char* dir_name = strndup(name, p-name);
          // makedirs(dir_name); // TODO
          free (dir_name);
      }
   }
   return fopen(name, mode);      
}


static int unzzip_cat (int argc, char ** argv, int extract)
{
    int argn;
    ZZIP_MEM_DISK* disk;

    if (argc == 1)
    {
	printf (__FILE__" version "ZZIP_PACKAGE" "ZZIP_VERSION"\n");
	return -1; /* better provide an archive argument */
    }

    disk = zzip_mem_disk_open (argv[1]);
    if (! disk) {
	perror(argv[1]);
	return -1;
    }

    if (argc == 2)
    {  /* print directory list */
	ZZIP_MEM_ENTRY* entry = zzip_mem_disk_findfirst(disk);
	fprintf(stderr, "..%p\n", entry);
	for (; entry ; entry = zzip_mem_disk_findnext(disk, entry))
	{
	    char* name = zzip_mem_entry_to_name (entry);
	    FILE* out = stdout;
	    if (extract) out = create_fopen(name, "wb", 1);
	    unzzip_mem_disk_cat_file (disk, name, out);
	    if (extract) fclose(out);
	}
	return 0;
    }

    if (argc == 3 && !extract)
    {  /* list from one spec */
	ZZIP_MEM_ENTRY* entry = 0;
	while ((entry = zzip_mem_disk_findmatch(disk, argv[2], entry, 0, 0)))
	{
	     unzzip_mem_entry_fprint (disk, entry, stdout);
	}

	return 0;
    }

    for (argn=1; argn < argc; argn++)
    {   /* list only the matching entries - each in order of commandline */
	ZZIP_MEM_ENTRY* entry = zzip_mem_disk_findfirst(disk);
	for (; entry ; entry = zzip_mem_disk_findnext(disk, entry))
	{
	    char* name = zzip_mem_entry_to_name (entry);
	    if (! fnmatch (argv[argn], name, 
			   FNM_NOESCAPE|FNM_PATHNAME|FNM_PERIOD))
	    {
	        FILE* out = stdout;
	        if (extract) out = create_fopen(name, "wb", 1);
		unzzip_mem_disk_cat_file (disk, name, out);
		if (extract) fclose(out);
		break; /* match loop */
	    }
	}
    }
    return 0;
} 

int unzzip_print (int argc, char ** argv)
{
    return unzzip_cat(argc, argv, 0);
}

int unzzip_extract (int argc, char ** argv)
{
    return unzzip_cat(argc, argv, 1);
}

/* 
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */
