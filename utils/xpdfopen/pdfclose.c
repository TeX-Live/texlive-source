/*
 * pdfclose.c
 * 
 * Modified by Jim Diamond (jim.diamond@acadiau.ca) 2010/04/21 (V 0.80) 
 *                         and again 2011/05/17 (V 0.81, V 0.82)
 * and Peter Breitenlohner <tex-live@tug.org> (2009).
 *
 * This program does not take a viewer argument, and just tries to close
 * any known viewer displaying the given file.
 *
 * V 0.82
 */

#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <unistd.h>

#include    "sendx.h"
#include    "xpdfopen.h"
#include    "utils.h"

#define     VERSION           "0.83"

char * progname;		/* set in main(); NOT static! */



static void
usage(void)
{
    fprintf(stderr, "%s version %s\n", progname, VERSION);
    fprintf(stderr, "Usage: %s <file.pdf>\n", progname);
}



static int
try_name(const char * filename, const char * title_format)
{
    char * winname;
    int ret ;

    winname = make_window_name(title_format, filename);
    if (winname == NULL)
	return 99;

    ret = sendx_control_token("W", winname);
    free(winname);
 
    return ret;
}



int
main (int argc, char * argv[])
{
    progname = argv[0];

    if (argc != 2)
    {
	usage();
	return EXIT_FAILURE;
    }
    
    if (try_name(argv[1], AR9_WIN_NAME_FMT))  	 /* both AR8 and AR9 */
	if (try_name(argv[1], XPDF_WIN_NAME_FMT))
	    if (try_name(argv[1], AR7_WIN_NAME_FMT))
		if (try_name(argv[1], AR5_WIN_NAME_FMT))
		   if (try_name(argv[1], EVINCE_WIN_NAME_FMT))
		       return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
