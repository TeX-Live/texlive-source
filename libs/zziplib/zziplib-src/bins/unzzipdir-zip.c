/*
 *	Copyright (c) 2003 Guido Draheim <guidod@gmx.de>
 *      Use freely under the restrictions of the ZLIB license.
 *
 *      This file is used as an example to clarify zzip api usage.
 */

#include <zzip/lib.h>
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

static int 
unzzip_list (int argc, char ** argv, int verbose)
{
    int argn;
    ZZIP_DIR* disk;
    zzip_error_t error;
    
    if (argc == 1)
    {
        printf (__FILE__" version "ZZIP_PACKAGE" "ZZIP_VERSION"\n");
        return EXIT_OK; /* better provide an archive argument */
    }
    
    disk = zzip_dir_open (argv[1], &error);
    if (! disk) {
	perror(argv[1]);
	return exitcode(error);
    }

    if (argc == 2)
    {  /* list all */
	ZZIP_DIRENT entry;
	while(zzip_dir_read(disk, &entry))
	{
	    char* name = entry.d_name;
	    long long usize = entry.st_size;
	    if (!verbose)
	    {
		printf ("%22lli %s\n", usize, name);
	    } else
	    {
		long long csize = entry.d_csize;
		unsigned compr = entry.d_compr;
		const char* defl = (compr < sizeof(comprlevel)) ? comprlevel[compr] : "(redu)";
		printf ("%lli/%lli %s %s\n", usize, csize, defl, name);
	    }
	}
    }
    else
    {   /* list only the matching entries - in order of zip directory */
	ZZIP_DIRENT entry;
	while(zzip_dir_read(disk, &entry))
	{
	    char* name = entry.d_name;
	    for (argn=1; argn < argc; argn++)
	    {
		if (! _zzip_fnmatch (argv[argn], name, 
		      _zzip_FNM_NOESCAPE|_zzip_FNM_PATHNAME|_zzip_FNM_PERIOD))
		{
		    long long usize = entry.st_size;
		    if (!verbose)
		    {
			printf ("%22lli %s\n", usize, name);
		    } else
		    {
			long long csize = entry.d_csize;
			unsigned compr = entry.d_compr;
			const char* defl = (compr < sizeof(comprlevel)) ? comprlevel[compr] : "(redu)";
			printf ("%lli/%lli %s %s\n", usize, csize, defl, name);
		    }
		    break; /* match loop */
		}
	    }
	}
    }
    zzip_dir_close(disk);
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
