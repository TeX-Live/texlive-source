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
#include <sys/stat.h>
#include <zzip/__mkdir.h>
#include <zzip/__string.h>
#include <zzip/__fnmatch.h>
#include <zzip/__debug.h>
#include "unzzipcat-zip.h"
#include "unzzip-states.h"

#ifdef ZZIP_HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef ZZIP_HAVE_IO_H
#include <io.h>
#endif

/* Functions in unzzip.c: */
extern int exitcode(int);
extern FILE* create_fopen(char*, char*, int);

static void unzzip_cat_file(ZZIP_DIR* disk, char* name, FILE* out)
{
    ZZIP_FILE* file = zzip_fopen(name, "rb");
    if (file) 
    {
	char buffer[1024]; int len;
	while ((len = zzip_fread (buffer, 1, 1024, file))) 
	{
	    fwrite (buffer, 1, len, out);
	}
	
	zzip_fclose (file);
    }
}

static int unzzip_cat (int argc, char ** argv, int extract)
{
    int done = 0;
    int argn;
    ZZIP_DIR* disk;
    
    if (argc == 1)
    {
        printf (__FILE__ " version " ZZIP_PACKAGE_NAME " " ZZIP_PACKAGE_VERSION "\n");
        return EXIT_OK; /* better provide an archive argument */
    }
    
    disk = zzip_opendir (argv[1]);
    if (! disk) {
        DBG3("opendir failed [%i] %s", errno, strerror(errno));
	perror(argv[1]);
	return exitcode(errno);
    }

    if (argc == 2)
    {  /* list all */
	ZZIP_DIRENT* entry = 0;
	while((entry = zzip_readdir(disk)))
	{
	    char* name = entry->d_name;
	    FILE* out = stdout;
	    if (extract) out = create_fopen(name, "wb", 1);
	    if (! out) {
	        if (errno != EISDIR) done = EXIT_ERRORS;
	        continue;
	    }
	    unzzip_cat_file (disk, name, out);
	    if (extract) fclose(out);
	}
	DBG2("readdir done %s", strerror(errno));
    }
    else
    {   /* list only the matching entries - in order of zip directory */
	ZZIP_DIRENT* entry = 0;
	while((entry = zzip_readdir(disk)))
	{
	    char* name = entry->d_name;
	    for (argn=1; argn < argc; argn++)
	    {
		if (! _zzip_fnmatch (argv[argn], name, 
		    _zzip_FNM_NOESCAPE|_zzip_FNM_PATHNAME|_zzip_FNM_PERIOD))
	        {
	             FILE* out = stdout;
	             char* zip_name = argv[1];
	             int zip_name_len = strlen(zip_name);
	             int name_len = strlen(name);
	             char* mix_name = malloc(zip_name_len + 2 + name_len);
	             if (zip_name_len > 4 && !strcmp(zip_name+zip_name_len-4, ".zip"))
	                 zip_name_len -= 4;
	             memcpy(mix_name, zip_name, zip_name_len);
	             mix_name[zip_name_len] = '/';
	             strcpy(mix_name + zip_name_len + 1, name);
	             if (extract) out = create_fopen(name, "wb", 1);
	             if (! out) {
	                 if (errno != EISDIR) done = EXIT_ERRORS;
	                 continue;
	             }
		     fprintf(stderr, "%s %s -> %s\n", zip_name, name, mix_name);
		     /* 'test1.zip' 'README' -> 'test1/README' */
		     unzzip_cat_file (disk, mix_name, out);
		     if (extract) fclose(out);
		     break; /* match loop */
	        }
	    }
	}
    }
    zzip_closedir(disk);
    return done;
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
