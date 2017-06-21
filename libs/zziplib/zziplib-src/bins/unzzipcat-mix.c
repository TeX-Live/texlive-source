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

#if !defined(ZZIP_HAVE_STRNDUP)
static char *
strndup(char *p, size_t maxlen)
{
    char *r;
    if (!p)
	return NULL;
    r = malloc(maxlen + 1);
    if (!r)
	return NULL;
    strncpy(r, p, maxlen);
    r[maxlen] = '\0';
    return r;
}
#endif

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
	    FILE* out = stdout;
	    if (extract) out = create_fopen(name, "wb", 1);
	    unzzip_cat_file (disk, name, out);
	    if (extract) fclose(out);
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
