/* Copyright (c) 1994-1999  All rights reserved. */
/* Written from scratch July 30, 1994, by A. P. Smith. */
/* Open up the ~/.mime-types and ~/.mailcap files to check what
   viewers to try to call on a new file (if they don't exist look for
   MIMELIBDIR/mime.types and MAILCAPLIBDIR/mailcap). 

   Patch by Allin Cottrell (cottrell@ricardo.ecn.wfu.edu) to
   invokeviewer applied 30/11/98.

   Patched further in january 1999 by Nicolai Langfeldt
   (janl@math.uio.no) to allow saner mime typing.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL PAUL VOJTA OR ANYONE ELSE BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
#define HAVE_BOOLEAN

#include "xdvi-config.h"
#if defined(HTEX) || defined(XHDVI)

#include <kpathsea/c-ctype.h>
#include <kpathsea/c-fopen.h>
#include <kpathsea/variable.h>

#ifndef MIMELIBDIR
#define MIMELIBDIR "/usr/local/etc"
#endif
#ifndef MAILCAPLIBDIR
#define MAILCAPLIBDIR "/usr/local/etc"
#endif


typedef struct _mimemap {
	char *content_type;
	char *extensions;
} Mimemap;

Mimemap mime_defaults[] = {
{ "application/octet-stream",    "bin", },
{ "application/oda",             "oda", },
{ "application/pdf",             "pdf", },
{ "application/postscript",      "ai eps epsf ps", },
{ "application/rtf",             "rtf", },
{ "application/x-mif",           "mif", },
{ "application/x-csh",           "csh", },
{ "application/x-dvi",           "dvi Dvi DVI", },
{ "application/x-hdf",           "hdf", },
{ "application/x-latex",         "latex", },
{ "application/x-netcdf",        "nc cdf", },
{ "application/x-sh",            "sh", },
{ "application/x-tcl",           "tcl", },
{ "application/x-tex",           "tex", },
{ "application/x-texinfo",       "texinfo texi", },
{ "application/x-troff",         "t tr roff", },
{ "application/x-troff-man",     "man", },
{ "application/x-troff-me",      "me", },
{ "application/x-troff-ms",      "ms", },
{ "application/x-wais-source",   "src", },
{ "application/zip",             "zip", },
{ "application/x-bcpio",         "bcpio", },
{ "application/x-cpio",          "cpio", },
{ "application/x-gtar",          "gtar", },
{ "application/x-shar",          "shar", },
{ "application/x-sv4cpio",       "sv4cpio", },
{ "application/x-sv4crc",        "sv4crc", },
{ "application/x-tar",           "tar", },
{ "application/x-ustar",         "ustar", },
{ "audio/basic",                 "au snd", },
{ "audio/x-aiff",                "aif aiff aifc", },
{ "audio/x-wav",                 "wav", },
{ "image/gif",                   "gif", },
{ "image/ief",                   "ief", },
{ "image/jpeg",                  "jpeg jpg jpe", },
{ "image/tiff",                  "tiff tif", },
{ "image/x-cmu-raster",          "ras", },
{ "image/x-portable-anymap",     "pnm", },
{ "image/x-portable-bitmap",     "pbm", },
{ "image/x-portable-graymap",    "pgm", },
{ "image/x-portable-pixmap",     "ppm", },
{ "image/x-rgb",                 "rgb", },
{ "image/x-xbitmap",             "xbm", },
{ "image/x-xpixmap",             "xpm", },
{ "image/x-xwindowdump",         "xwd", },
{ "text/html",                   "html htm sht shtml", },
{ "text/plain",                  "txt", },
{ "text/richtext",               "rtx", },
{ "text/tab-separated-values",   "tsv", },
{ "text/x-setext",               "etx", },
{ "video/mpeg",                  "mpeg mpg mpe", },
{ "video/quicktime",             "qt mov", },
{ "video/x-msvideo",             "avi", },
{ "video/x-sgi-movie",           "movie", },
{ "application/gzip",		"gz", } ,
{ "application/compress",	"Z", } ,
{ "application/bzip",		"bz", } ,
{ "application/bzip2",		"bz2", } ,
};

Mimemap *curmimemap = mime_defaults;
int nmime, maxnmime = 0;

typedef struct _mailcap {
	char *content_type;
	char *viewer;
	Boolean needs_base;
} Mailcap;

Mailcap mailcap_defaults[] = {
{ "audio/*",                    "showaudio %s", False, },
{ "image/*",                    "xv %s", False, },
{ "video/mpeg",                 "mpeg_play %s", False, },
{ "application/pdf",            "acroread %s", False, },
{ "application/postscript",     "ghostview %s", False, },
{ "application/x-dvi",          "xdvi -base %s %s", True, },
};

Mailcap *curmailcap = mailcap_defaults;
int nmailcap, maxnmailcap = 0;

#define LINE 1024
#define MIMESTEP 50

int
parsemimes ()
{
	int i;
	char buf[LINE], *cp, *cp2;
	FILE *strm;
	static int already_called = 0;

	if (already_called) return 1;
	already_called = 1;

	sprintf(buf, "%s/.mime-types", getenv("HOME"));
	if ((strm = xfopen_local(buf, FOPEN_R_MODE)) == NULL) {
	  string mimelibdir = kpse_var_value ("MIMELIBDIR");
	  if (!mimelibdir)
	    mimelibdir = MIMELIBDIR;
	  sprintf(buf, "%s/mime.types", mimelibdir);
	  if ((strm = xfopen_local(buf, FOPEN_R_MODE)) == NULL) {
	          nmime = sizeof(mime_defaults)/sizeof(Mimemap);
	          return 0;
	  }
	}
	curmimemap = xmalloc(MIMESTEP*sizeof(Mimemap));
	maxnmime = MIMESTEP;
	for (i=0; i < maxnmime; i++) {
		curmimemap[i].content_type = curmimemap[i].extensions = NULL;
	}
	nmime = 0;
	while (fgets(buf, LINE, strm) != NULL) {
		cp = buf;
		while (isspace(*cp)) cp++;
		if (*cp == '#') continue;
		if ((cp2 = strchr(cp, ' ')) == NULL)
			if ((cp2 = strchr(cp, '\t')) == NULL) continue;
		*cp2 = '\0'; /* Terminate cp string */
		cp2++;
		while (isspace(*cp2)) cp2++;
		if (*cp2 == '\0') continue; /* No extensions list */
		if (nmime >= maxnmime) {
			maxnmime += MIMESTEP;
			curmimemap = (Mimemap *) realloc(curmimemap,
						maxnmime*sizeof(Mimemap));
			for (i=nmime; i < maxnmime; i++) {
				curmimemap[i].content_type =
				    curmimemap[i].extensions = NULL;
			}
		}
		MyStrAllocCopy(&(curmimemap[nmime].content_type), cp);
		MyStrAllocCopy(&(curmimemap[nmime].extensions), cp2);
		nmime++;
	}
	return 1;
}

int
parsemailcap()
{
	int i;
	char buf[LINE], *cp, *cp2;
	FILE *strm;
	static int already_called = 0;

	if (already_called) return 1;
	already_called = 1;
	sprintf(buf, "%s/.mailcap", getenv("HOME"));
	if ((strm = xfopen_local(buf, FOPEN_R_MODE)) == NULL) {
          string mailcaplibdir = kpse_var_value ("MAILCAPLIBDIR");
	  if (!mailcaplibdir)
            mailcaplibdir = MAILCAPLIBDIR;
          sprintf(buf, "%s/mailcap", MAILCAPLIBDIR);
          if ((strm = xfopen_local(buf, FOPEN_R_MODE)) == NULL) {
                  nmailcap = sizeof(mailcap_defaults)/sizeof(Mailcap);
                  return 0;
          }
	}
	curmailcap = (Mailcap *) malloc(MIMESTEP*sizeof(Mailcap));
	maxnmailcap = MIMESTEP;
	for (i=0; i < maxnmailcap; i++) {
		curmailcap[i].content_type = curmailcap[i].viewer = NULL;
	}
	nmailcap = 0;
	while (fgets(buf, LINE, strm) != NULL) {
		cp = strrchr(buf, '\n');
		if (cp != NULL) *cp = '\0'; /* Get rid of trailing \n */
		cp = buf;
		while (isspace(*cp)) cp++;
		if (*cp == '#') continue;
		if ((cp2 = strchr(cp, ';')) == NULL) continue;
		*cp2 = '\0'; /* Terminate cp string */
		cp2++;
		while (isspace(*cp2)) cp2++;
		if (*cp2 == '\0') continue; /* No viewer info? */
		if (nmailcap >= maxnmailcap) {
			maxnmailcap += MIMESTEP;
			curmailcap = (Mailcap *) realloc(curmailcap,
						maxnmailcap*sizeof(Mailcap));
			for (i=nmailcap; i < maxnmailcap; i++) {
				curmailcap[i].content_type =
				    curmailcap[i].viewer = NULL;
			}
		}
		MyStrAllocCopy(&(curmailcap[nmailcap].content_type), cp);
		MyStrAllocCopy(&(curmailcap[nmailcap].viewer), cp2);
		curmailcap[nmailcap].needs_base = False;
		if ((cp = strstr(curmailcap[nmailcap].viewer, "%s")) == NULL)
				continue; /* Not valid viewer info? */
		if (strstr(cp+2, "%s") != NULL) /* Needs two strings */
			curmailcap[nmailcap].needs_base = True;
		nmailcap++;
	}
	return 0;
}


char *figure_mime_type(filename)
     char *filename;

{
  /*
    Separated this code from invoke_viewer on the rationale that If
    we have all this fancy code to determine mime types WE'LL DAMN
    WELL USE IT! 

    BUG: It would be better yet to use the code to do the same things
    in libwww but I'd rather not have to figure out libwww.

    -janl 21/1/1999 
  */

  int i, j;
  char *extension, *cp;
  char *content_type = NULL;
  char *viewer = NULL;
  char viewingcommand[LINE];
  /* It's reasonable to assume that an extensionles file is a dvi file
     when the application name is xdvi and the applicaton name _is_
     xdvi! */
  static char *default_type="application/x-dvi";

  /* First check for the mailcap and mime files */
  (void) parsemimes();
  (void) parsemailcap();

  if (debug & DBG_HYPER) 
    fprintf(stderr,"figure_mime_type: Called to find type of %s\n",filename);

  /* See if this is a directory */
  if (filename[strlen(filename)-1]=='/') {
    if (debug & DBG_HYPER)
      fprintf(stderr,"It's a directory, returning www/unknown\n");
    return "www/unknown";
  }

  /* See if filename extension is on the mime list: */
  extension = strrchr(filename,'.');
  if (extension == NULL) {
    if (debug & DBG_HYPER)
      fprintf(stderr,"No extension, defaulting to %s\n",default_type);
    return default_type;
  }
  extension++;
  for (i=0; i < nmime; i++) { /* Compare with space sep. list of exts */
    cp = curmimemap[i].extensions;
    while ((cp = strstr(cp, extension)) != NULL) {
      if ((cp - curmimemap[i].extensions > 0) &&
	  (cp[-1] != ' ')) {
	cp ++;
	continue;
      }
      cp += strlen(extension);
      if ((*cp != ' ') && (*cp != '\0')) continue;
      content_type = curmimemap[i].content_type;
      break;
    }
    if (content_type != NULL) break;
  }

  /* no such mime extension */
  if (content_type == NULL) content_type=default_type;

  if (debug & DBG_HYPER)
    fprintf(stderr,"The type of %s is %s\n",filename,content_type);
  
  return content_type;
}


int
invokeviewer(filename)
   char *filename;
{
	int i, j;
	char *content_type = NULL;
	char *viewer = NULL;
	char viewingcommand[LINE];

	content_type=figure_mime_type(filename);

	for (i=0; i < nmailcap; i++) {
		if (!strcmp(curmailcap[i].content_type, content_type)) {
			viewer = curmailcap[i].viewer;
			j = i;
			break;
		}
	}
	if (viewer == NULL) return 0;
	/* Turn the (possibly URL) filename into a local file */
	i = fetch_relative_url(URLbase, filename, temporary_dir);
	if (i < 0) return 0;
	wait_for_urls();
	if (curmailcap[j].needs_base) {
		sprintf(viewingcommand, viewer,
			 (URLbase == NULL)? "none":filelist[i].url, filelist[i].file);
	} else {
		sprintf(viewingcommand, viewer, filelist[i].file);
	}
#ifndef WIN32
	strcat(viewingcommand, " &");
#endif
	if (debug & DBG_HYPER) {
		fprintf(stderr, "Executing: %s\n", viewingcommand);
	}
#ifdef WIN32
	win32_system(viewingcommand);
#else
	system(viewingcommand);
#endif
	return 1;
}
#endif /* HTEX || XHDVI */
