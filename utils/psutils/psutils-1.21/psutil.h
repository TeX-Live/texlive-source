/* psutil.h
 * PSUtils utility functions
 *
 * (c) Reuben Thomas 2012
 * (c) Angus J. C. Duggan 1991-1997
 * See file LICENSE for details.
 */
#define _FILE_OFFSET_BITS 64
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>

/* message flags */
#define MESSAGE_NL	1	/* Newline before message if necessary */
#define MESSAGE_PROGRAM	2	/* announce program name */
#define MESSAGE_EXIT	4	/* do not return */

/* message types */
#define FATAL		(MESSAGE_EXIT|MESSAGE_PROGRAM|MESSAGE_NL)
#define WARN		(MESSAGE_NL|MESSAGE_PROGRAM)
#define LOG		0

/* change mode of a file pointer on Windows */
#if defined(WIN32)
#include <io.h>
#include <fcntl.h>
#define SET_BINARY(fp) (void)_setmode(fileno(fp), _O_BINARY)
#undef off_t
#undef ftello
#undef fseeko
#if defined(__MINGW32__)
#define off_t off64_t
#define ftello ftello64
#define fseeko fseeko64
#else
#define off_t __int64
#define ftello _ftelli64
#define fseeko _fseeki64
#endif
#else /* !WIN32 */
#define SET_BINARY(fp) (void)0
#endif

/* Definitions for functions found in psutil.c */
extern void usage(void);
extern void message(int flags, const char *format, ...);
extern void argerror(void);
extern void set_paper_size(const char *paper_name);
const struct paper *get_paper(const char *paper_name);
extern FILE *seekable(FILE *fp);
extern void writepage(int p);
extern void seekpage(int p);
extern void writepageheader(const char *label, int p);
extern void writepagesetup(void);
extern void writepagebody(int p);
extern void writeheader(int p, off_t *ignorelist);
extern void writeheadermedia(int p, off_t *ignorelist, double width, double height);
extern int writepartprolog(void);
extern void writeprolog(void);
extern void writesetup(void);
extern void writetrailer(void);
extern void writeemptypage(void);
extern void scanpages(off_t *sizeheaders);
extern void writestring(const char *s);

/* These variables are exported to the client program */
extern char *program ;
extern int pages;
extern int verbose;
extern FILE *infile;
extern FILE *outfile;
extern char pagelabel[BUFSIZ];
extern int pageno;

/* Variables imported from the client program */
extern const char *syntax;
extern const char *argerr_message;

/* Useful macros */
#define MIN(x,y) ((x) > (y) ? (y) : (x))
#define COPYRIGHT_STRING "(c) Reuben Thomas <rrt@sc3d.org> 2012\n(c) Angus J. C. Duggan 1991-1997\nSee file LICENSE for details.\n"
