/*
 * externs.h: This file defines or declares (depending on whether
 * 	      DEFINE_EXTERNS is defined) all of the global variables
 *	      and constants used by pdfopen and pdfclose.
 *
 * Copyright (C) 2014 Jim Diamond <jim.diamond@acadiau.ca>
 * You may freely use, modify and/or distribute this file.
 *
 * This file new in Version 0.84 of pdfopen (2014/05/16).
 */


#ifndef EXTERNS_H
#define EXTERNS_H

#ifdef DEFINE_EXTERNS
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN char * progname;

#endif
