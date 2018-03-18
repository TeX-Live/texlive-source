/*
 *	Copyright (c) 2003 Guido Draheim <guidod@gmx.de>
 *      Use freely under the restrictions of the ZLIB license.
 *
 *      This file is used as an example to clarify zzipfseeko api usage.
 */

#include <zzip/fseeko.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <zzip/__mkdir.h>
#include <zzip/__string.h>
#include <zzip/__debug.h>
#include <zzip/__fnmatch.h>
#include "unzzipcat-zip.h"
#include "unzzip-states.h"

static int exitcode(int e)
{
    return EXIT_ERRORS;
}

static void unzzip_big_entry_fprint(ZZIP_ENTRY* entry, FILE* out)
{
    ZZIP_ENTRY_FILE* file = zzip_entry_fopen (entry, 0);
    if (file) 
    {
	char buffer[1024]; int len;
	while ((len = zzip_entry_fread (buffer, 1024, 1, file)))
	{
	    DBG2("entry read %i", len);
	    fwrite (buffer, len, 1, out);
	}
	DBG2("entry done %s", strerror(errno));
	zzip_entry_fclose (file);
    } else
    {
        DBG2("could not open entry: %s", strerror(errno));
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

static FILE* create_fopen(char* name, char* mode, int subdirs)
{
   if (subdirs)
   {
      char* p = strrchr(name, '/');
      if (p) {
          char* dir_name = _zzip_strndup(name, p-name);
          makedirs(dir_name); 
          free (dir_name);
      }
   }
   return fopen(name, mode);      
}


static int unzzip_cat (int argc, char ** argv, int extract)
{
    int done = 0;
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
	    FILE* out = stdout;
	    if (! name) {
	        done = EXIT_WARNINGS;
	        continue;
	    }
	    if (extract) out = create_fopen(name, "wb", 1);
	    if (! out) {
	        if (errno != EISDIR) done = EXIT_ERRORS;
	        continue;
	    }
	    unzzip_cat_file (disk, name, out);
	    if (extract) fclose(out);
	    free (name);
	}
	return done;
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
	    DBG3(".. check '%s' to zip '%s'", argv[argn], name);
	    if (! _zzip_fnmatch (argv[argn], name, 
		_zzip_FNM_NOESCAPE|_zzip_FNM_PATHNAME|_zzip_FNM_PERIOD))
	    {
	        FILE* out = stdout;
	        if (extract) out = create_fopen(name, "wb", 1);
		if (! out) {
		    if (errno != EISDIR) done = EXIT_ERRORS;
		    continue;
		}
		unzzip_cat_file (disk, name, out);
		if (extract) fclose(out);
		break; /* match loop */
	    }
	    free (name);
	}
    }
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
