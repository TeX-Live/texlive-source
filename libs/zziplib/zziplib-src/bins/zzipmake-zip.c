/*
 *	Copyright (c) 2003 Guido Draheim <guidod@gmx.de>
 *      Use freely under the restrictions of the ZLIB license.
 *
 *      This file is used as an example to clarify zzip api usage.
 *                        (the write-api is work in progress, beware)
 */

#define _ZZIP_WRITE_SOURCE

#include <zzip/write.h>
#include <stdio.h>
#include <string.h>
#include "zzipmake-zip.h"

#ifdef ZZIP_HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef ZZIP_HAVE_IO_H
#include <io.h>
#endif

int rezzip_make (int argc, char ** argv)
{
    int argn;
    int exitcode = 0;
    ZZIP_DIR * dir;

    dir = zzip_dir_creat(argv[1], 0755);
    if (! dir)
    {
	fprintf (stderr, "did not creat %s: \n", argv[1]);
	perror(argv[1]);
	if (1)
	    return 1;
	else
	    fprintf (stderr, "(ignored)\n");
    }
    
    for (argn=2; argn < argc; argn++)
    {
	int input = open (argv[argn], O_RDONLY);
	if (input == -1)
	{
	    perror (argv[argn]);
	    continue;
	}
	else
	{
	    char buf[17]; zzip_ssize_t n;
	    ZZIP_FILE* output = zzip_file_creat (dir, argv[argn], 0755);
	    if (! output)
	    {
		fprintf (stderr, "|did not open %s: \n", argv[argn]);
		fprintf (stderr, "|%s: %s\n", argv[argn], 
			 zzip_strerror_of(dir));
		continue;
	    }

	    while ((n = read (input, buf, 16)))
	    {
		zzip_write (output, buf, n);
	    }
	    zzip_close (output);
        }
	close (input);
    }
    zzip_dir_close(dir);
    
    return exitcode;
} 

/* 
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */
