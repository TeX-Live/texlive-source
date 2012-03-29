/*
 * utils.c: Items common to pdfopen and pdfclose.
 *
 * Copyright (C) 2010, 2012 Jim Diamond <jim.diamond@acadiau.ca>
 * You may freely use, modify and/or distribute this file.
 */

#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    "utils.h"
#include    "xpdfopen.h"

/*
 * Allocate memory and create a window name.
 * Return a pointer to the new string or NULL on failure.
 * 
 * The caller is responsible for free()ing the memory allocated here.
 *
 * Note: acroread (at least AR 7, 8, 9) uses only the basename in the
 *       window title.
 *	 xpdf uses the whole file name.
 *	 Other PDF viewers may need other treatment.
 */

char *
make_window_name(const char * fmt, const char * filename)
{
    char * window_name;
    const char * title_name;

    title_name = filename;
    if (strrchr(title_name, '/') != NULL
	&& strncasecmp(fmt, XPDF_WIN_NAME, strlen(XPDF_WIN_NAME)))
	title_name = strrchr(title_name, '/') + 1;

    window_name = malloc(strlen(fmt) + strlen(title_name) + 1);
    if (window_name != NULL)
	sprintf(window_name, fmt, title_name);
    else
	fprintf(stderr, "%s: out of memory\n", progname);

    return window_name;
}
