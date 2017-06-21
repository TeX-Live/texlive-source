/*
 *	Copyright (c) 2003 Guido Draheim <guidod@gmx.de>
 *      Use freely under the restrictions of the ZLIB license.
 *
 *      This file is used as an example to clarify zzipfseeko api usage.
 */

#include <zzip/fseeko.h>
#include <stdlib.h>
#include <string.h>
#include "unzzip.h"

#ifdef ZZIP_HAVE_FNMATCH_H
#include <fnmatch.h>
#else
#define fnmatch(x,y,z) strcmp(x,y)
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif

#ifdef DEBUG
#define debug1(msg) do { fprintf(stderr, "%s : " msg "\n", __func__); } while(0)
#define debug2(msg, arg1) do { fprintf(stderr, "%s : " msg "\n", __func__, arg1); } while(0)
#define debug3(msg, arg1, arg2) do { fprintf(stderr, "%s : " msg "\n", __func__, arg1, arg2); } while(0)
#else
#define debug1(msg) 
#define debug2(msg, arg1) 
#define debug3(msg, arg1, arg2) 
#endif

static void unzzip_big_entry_fprint(ZZIP_ENTRY* entry, FILE* out)
{
    ZZIP_ENTRY_FILE* file = zzip_entry_fopen (entry, 0);
    if (file) 
    {
	char buffer[1024]; int len;
	while ((len = zzip_entry_fread (buffer, 1024, 1, file)))
	{
	    debug2("entry read %i", len);
	    fwrite (buffer, len, 1, out);
	}
	debug2("entry done %s", strerror(errno));
	zzip_entry_fclose (file);
    } else
    {
        debug2("could not open entry: %s", strerror(errno));
    }
}

static void unzzip_cat_file(FILE* disk, char* name, FILE* out)
{
    ZZIP_ENTRY_FILE* file = zzip_entry_ffile (disk, name);
    if (file) 
    {
	char buffer[1024]; int len;
	while ((len = zzip_entry_fread (buffer, 1024, 1, file)))
	    fwrite (buffer, len, 1, out);
	
	zzip_entry_fclose (file);
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
    FILE* disk;

    disk = fopen (argv[1], "rb");
    if (! disk) {
	perror(argv[1]);
	return -1;
    }

    if (argc == 2)
    {  /* print directory list */
	ZZIP_ENTRY* entry = zzip_entry_findfirst(disk);
	for (; entry ; entry = zzip_entry_findnext(entry))
	{
	    char* name = zzip_entry_strdup_name (entry);
	    FILE* out = stdout;
	    if (extract) out = create_fopen(name, "wb", 1);
	    unzzip_cat_file (disk, name, out);
	    if (extract) fclose(out);
	    free (name);
	}
	return 0;
    }

    if (argc == 3 && !extract)
    {  /* list from one spec */
	ZZIP_ENTRY* entry = 0;
	while ((entry = zzip_entry_findmatch(disk, argv[2], entry, 0, 0)))
	{
	     unzzip_big_entry_fprint (entry, stdout);
	}
	return 0;
    }

    for (argn=1; argn < argc; argn++)
    {   /* list only the matching entries - each in order of commandline */
	ZZIP_ENTRY* entry = zzip_entry_findfirst(disk);
	for (; entry ; entry = zzip_entry_findnext(entry))
	{
	    char* name = zzip_entry_strdup_name (entry);
	    debug3(".. check '%s' to zip '%s'", argv[argn], name);
	    if (! fnmatch (argv[argn], name, 
			   FNM_NOESCAPE|FNM_PATHNAME|FNM_PERIOD))
	    {
	        FILE* out = stdout;
	        if (extract) out = create_fopen(name, "wb", 1);
		unzzip_cat_file (disk, name, out);
		if (extract) fclose(out);
		break; /* match loop */
	    }
	    free (name);
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
