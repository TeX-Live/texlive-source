/* $Id: dmp_ffile.c,v 1.4 2005/08/24 10:54:02 taco Exp $
 *
 * File searching for dmp. This is a unified source file implementing two
 * different strategies: kpathsea lookups vs. recursive searching.
 *
 * There is a #define to switch between the two personalities, with
 * kpathsea the default option
 *
 *  Public domain.  
 */

#include <stdlib.h>
#include <stdio.h>

#include "dmp.h"

#ifndef FFSEARCH

#include "config.h"
#include <kpathsea/tex-file.h>
#include <kpathsea/c-ctype.h>
#include <kpathsea/c-pathch.h>

static int
 kpse_types[5] = { 0,
    kpse_tfm_format,		/* tex fonts */
    kpse_mpsupport_format,	/* mapfile */
    kpse_mpsupport_format,	/* charlib */
    kpse_troff_font_format
};


void
fsearch_init(int argc, char **argv)
{
    kpse_set_progname(argv[0]);
}

FILE *
fsearch(char *nam, char *ext, int path_type)
{
    {
	FILE *f = NULL;
	int format = kpse_types[path_type];
	string fname = kpse_find_file(nam, format, true);
	const_string mode = kpse_format_info[format].binmode
	    ? FOPEN_RBIN_MODE : FOPEN_R_MODE;
	if (fname) {
	    f = xfopen(fname, mode);
	}
	if (f == NULL)
	    quit("Cannot find ", nam, ext);
	return f;
    }

}

#else

void pathexpand(char *buf, int dirlen, int bufsize);

/* Where dmp looks for tex font metric files */
#define TEXFONTS ".:/usr/lib/tex/fonts/tfm"

/* Where dmp looks for troff font tables */
#define TRFONTS "/usr/share/groff/current/font/devps"

/* Where dmp looks for font map data and spec chars */
#define MPPOOL ".:/usr/lib/mp"


char *tfmpath = TEXFONTS;	/* directories to search for TFM files */
char *dbpath = MPPOOL;		/* directories for font map data */
char *charpath = MPPOOL;	/* directories for spec chars */
char *trpath = TRFONTS;		/* search path for troff font data */

/**************************************************************
			Search Paths
***************************************************************/

#ifdef MS_DOS
#define PATH_DELIM ';'
#else
#ifdef WIN32
#define PATH_DELIM ';'
#else
#define PATH_DELIM ':'
#endif
#endif

void
fsearch_init(int argc, char **argv)
{
    char *envpath;

    if ((envpath = getenv("TEXFONTS")) != NULL)
	tfmpath = envpath;
    if ((envpath = getenv("MPPOOL")) != NULL) {
	dbpath = envpath;
	charpath = envpath;
    }
    if ((envpath = getenv("TRFONTS")) != NULL)
	trpath = envpath;
}


FILE *
fsearch(char *nam, char *ext, int path_type)
{
    FILE *f;
    char *s, *p;
    int dirlen;
    char *path;
    if (path_type == TEXFONTS_TYPE) {
	path = tfmpath;
    } else if (path_type == DB_TYPE) {
	path = dbpath;
    } else if (path_type == CHARLIB_TYPE) {
	path = charpath;
	/* somewhat sneaky trick to unify 'fsearch' API with          
	   the smallest possible code impact */
	ext = nam;
	nam = "charlib/";
    } else if (path_type == TRFONTS_TYPE) {
	path = trpath;
    }

    if (nam[0] == '/')		/* file name has absolute path */
	path = NULL;
    do {
	s = &strpool[poolsize];
	if (path != NULL) {
	    while ((*path != PATH_DELIM) && (*path != '\0'))
		add_to_pool(*path++);
	    if (*path == '\0')
		path = NULL;
	    else
		path++;
	    add_to_pool('/');
	}
	dirlen = &strpool[poolsize] - s;
	for (p = nam; *p != '\0'; p++)
	    add_to_pool(*p);
	for (p = ext; *p != '\0'; p++)
	    add_to_pool(*p);
	add_to_pool('\0');
	pathexpand(s, dirlen, &strpool[POOLMAX] - s);
	f = fopen(s, "r");
	poolsize = s - &strpool[0];
    } while (f == NULL && path != NULL);
    if (f == NULL)
	quit("Cannot find ", nam, ext);
    return f;
}



/**********************************************************
 You might have to modify the following includes and defines.
 They are supposed to declare opendir(), readdir(), closedir()
 and define how to access the result of readdir().
 **********************************************************/
#ifdef BSD
#include <sys/types.h>
#include <sys/dir.h>
#define DIRentry  struct direct	/* readdir() returns pointer to this */
#define de_name   d_name	/* field in DIRentry for file name */

#else
/* This is for SYSV and _POSIX_SOURCE; I don't know about other systems
 */
#include <dirent.h>
#define DIRentry  struct dirent	/* readdir() returns pointer to this */
#define de_name   d_name	/* field in DIRentry for file name */
#endif


/**********************************************************
 Nothing below here should need changing
 **********************************************************/

#include <stdio.h>

#include <string.h>

extern void exit();
extern int access();


#ifdef PEDEBUG
/* The PEDEBUG flag is meant for seperate debugging of this file.  Do not
   set it when compiling MetaPost.
*/
int
myaccess(nam, mode)
{
    int r;
    r = access(nam, mode);
    printf("can%s access %s\n", (r == 0 ? "" : "not"), nam);
    return r;
}
#else
#define myaccess   access
#endif


void
move_str(dest, src)		/* strcpy() for overlapping strings */
char *dest, *src;
{
    char *s, *d;
    int n;

    if (dest < src) {
	while (*src != '\0')
	    *dest++ = *src++;
	*dest = '\0';
    } else {
	n = strlen(src);
	d = dest + n;
	s = src + n;
	*d = '\0';
	while (s != src)
	    *--d = *--s;
    }
}


/* Find the first "//" within the first dirlen characters of buf[], or return
 * zero to indicate failure.
 */
char *
find_dblslash(buf, dirlen)
char *buf;
int dirlen;
{
    char *p;
    int k;

    k = 0;
    dirlen--;
    while ((p = strchr(buf + k, '/')) != 0) {
	if (p - buf >= dirlen)
	    return 0;
	if (p[1] == '/')
	    return p;
	k = 1 + p - buf;
    }
    return 0;
}


/* Pretend the string ends at endp and call opendir().
 */
DIR *
pe_opendir(buf, endp)
char *buf, *endp;
{
    DIR *dirp;
    char s;

    s = *endp;
    *endp = '\0';
    dirp = opendir(buf);
#ifdef PEDEBUG
    printf("%s directory %s\n", (dirp == 0 ? "cannot open" : "opening"),
	   buf);
#endif
    *endp = s;
    return dirp;
}


/* A file name begins at buf and has a slash at slash followed by a final
 * component of length tail.  Append one or more intermediate directories and
 * then the final component so as to make an accessible path name.  If this can
 * be done, return the length of the path name; otherwise return something
 * negative and leave buf set to a bad path name.  On exit, *tailp is pointed
 * to the tail string.  Buffer overflow happens when the tail string would have
 * to start at a position greater than lim.
 * The algorithm is depth-first search with pruning based on the idea that
 * directories contain files or subdirectories, but not both.
 */
int
expand_dblslash(buf, slash, tail, lim, tailp)
char *buf, *slash, *lim;
int tail;
char **tailp;
{
    DIR *dirp;
    DIRentry *de;
    char *t, *tt;		/* old and new positions of tail string */
    char *ds;
    int r;

    t = slash + 1;
    *tailp = t;
    dirp = pe_opendir(buf, slash);
    if (dirp == 0)
	return -2;
    if (myaccess(buf, 04) == 0) {
	closedir(dirp);
	return (slash - buf) + 1 + tail;
    }
    while ((de = readdir(dirp)) != 0) {
	ds = de->de_name;
	if (ds[0] == '.' && (ds[1] == 0 || ds[1] == '.' && ds[2] == 0))
	    continue;
	tt = slash + 2 + strlen(ds);
	if (tt > lim) {
	    *slash = '\0';
	    fprintf(stderr, "> %s/%s/%s\n", buf, de->de_name, t);
	    fprintf(stderr, "! Fatal error: %s\n",
		    "// expansion makes path name too long");
	    exit(1);
	}
	move_str(tt, t);
	strcpy(slash + 1, ds);
	tt[-1] = '/';
	r = expand_dblslash(buf, tt - 1, tail, lim, &t);
	if (r != -1) {
	    *tailp = t;
	    closedir(dirp);
	    return (r < 0) ? -1 : r;
	}
    }
    closedir(dirp);
    *tailp = t;
    return -1;
}



/* Assume that buf points to an array of length at least bufsize that
 * contains a path name.  Modify the path name by expanding "//" if it occurs
 * in the first dirlen characters.  The expansions consist of zero or more
 * intermediate directories.  If the expansion fails, buf will contain an
 * invalid path name.
 */
void
pathexpand(buf, dirlen, bufsize)
char *buf;			/* input path */
int dirlen;			/* limit search to buf[0]..buf[dirlen-1] */
int bufsize;			/* number of character in buf array */
{
    char *p;
    int tail, junk;

    if ((p = find_dblslash(buf, dirlen)) != 0) {
	tail = strlen(p + 1);
	move_str(p, p + 1);
	expand_dblslash(buf, p, tail, buf + bufsize - tail - 1, &junk);
    }
}

#endif				/* FFSEARCH */
