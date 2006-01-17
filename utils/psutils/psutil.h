/* psutil.h
 * Copyright (C) Angus J. C. Duggan 1991-1995
 * See file LICENSE for details.
 *
 * utilities for PS programs
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "config.h"

/* types for describing document; this is a step towards version 2 */
typedef long Fileptr ;

/* paper size structure; configurability and proper paper resources will have
   to wait until version 2 */
typedef struct papersize {
   char *name;		/* name of paper size */
   int width, height;	/* width, height in points */
} Paper ;
#define PaperName(p) ((p)->name)
#define PaperWidth(p) ((p)->width)
#define PaperHeight(p) ((p)->height)

/* Definitions for functions found in psutil.c */
extern Paper *findpaper(char *name);
extern FILE *seekable(FILE *fp);
extern void writepage(int p);
extern void seekpage(int p);
extern void writepageheader(char *label, int p);
extern void writepagesetup(void);
extern void writepagebody(int p);
extern void writeheader(int p);
extern int writepartprolog(void);
extern void writeprolog(void);
extern void writesetup(void);
extern void writetrailer(void);
extern void writeemptypage(void);
extern void scanpages(void);
extern void writestring(char *s);

/* These variables are imported from the client program (e.g. psbook, psnup,
   etc.) */
extern char *program ;
extern int pages;
extern int verbose;
extern FILE *infile;
extern FILE *outfile;
extern char pagelabel[BUFSIZ];
extern int pageno;
