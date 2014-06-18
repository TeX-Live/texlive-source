/*
 * xpdfopen.h: declare constants of possible use for xpdfopen programs.
 *
 * Copyright (C) 2010--2014 Jim Diamond <jim.diamond@acadiau.ca>
 *
 * You may freely use, modify and/or distribute this file.
 */

#ifndef XPDFOPEN_H
#define XPDFOPEN_H

#define     VERSION           "0.86"

/*
 * <prog>_WIN_NAME is the name of the window when no file is open.
 * <prog>_WIN_NAME_FMT is the (sprintf) format to generate the name
 *                           when a file is being displayed.
 */

#define     AR5_WIN_NAME	    "Acrobat Reader"	/* Is this correct? */
#define     AR5_WIN_NAME_FMT	    "%s"		/* Is this correct? */

#define	    AR7_WIN_NAME	    "Adobe Reader"
#define     AR7_WIN_NAME_FMT	    AR7_WIN_NAME " - %s"

#define	    AR8_WIN_NAME	    "Adobe Reader"
#define     AR8_WIN_NAME_FMT	    "%s - " AR8_WIN_NAME

#define	    AR9_WIN_NAME	    "Adobe Reader"
#define     AR9_WIN_NAME_FMT	    "%s - " AR9_WIN_NAME

#define	    XPDF_WIN_NAME	    "Xpdf"
#define     XPDF_WIN_NAME_FMT	    XPDF_WIN_NAME ": %s"

#define	    EVINCE_WIN_NAME	    "evince"
#define     EVINCE_WIN_NAME_FMT	    "%s"

#define	    OKULAR_WIN_NAME	    "Okular"
#define     OKULAR_WIN_NAME_FMT	    "%s - " OKULAR_WIN_NAME

#endif
