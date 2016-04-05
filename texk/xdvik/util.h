/*
 * Copyright (c) 2002-2015 the xdvik development team
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * PAUL VOJTA OR ANY OTHER AUTHOR OF THIS SOFTWARE BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <stdio.h>
#include "version.h"
#include "kpathsea/c-stat.h"
#include "kpathsea/hash.h"
#include "kpathsea/tex-file.h"

#if HAVE_POLL
# include <poll.h>
# define XIO_IN POLLIN
# define XIO_OUT POLLOUT
#else
# if HAVE_SYS_SELECT_H
#  include <sys/select.h>
# else
#  if HAVE_SELECT_H
#   include <select.h>
#  endif
# endif
# define XIO_IN 1
# define XIO_OUT 2
#endif

#include "events.h" /* for child proc stuff */

FILE *try_fopen(const char *fname, const char *mode);
FILE *try_fdopen(int fd, const char *mode);
int try_open(const char *fname, int flags);
int try_open_mode(const char *fname, int flags, mode_t mode);

extern int xdvi_temp_fd(char **tempfilename);
extern void xdvi_assert(const char *version,
			const char *filename,
			int lineno,
			Boolean condition,
			const char *fmt,
			...);

typedef void (*child_exited_proc)(int status, struct xchild *this);

extern void handle_child_exit(int status, struct xchild *this);
extern char *read_child_error(int fd, void *data);
extern Boolean fork_process(const char *file, Boolean redirect_stdout,
			    const char *dirname,
			    childProcT exit_proc, void *data,
			    int killsig,
			    char *const argv[]);
extern void prep_fd(int fd, wide_bool noblock);

struct bitmap; /* forward declaration */
extern void alloc_bitmap(struct bitmap *);

extern char *my_realpath(const char *path, char *real);
#ifdef HAVE_REALPATH
#include <limits.h>
# define REALPATH realpath
#else
# define REALPATH my_realpath
#endif

char *expand_homedir(const char *path);
void set_dvi_name_expand(const char *new_filename);
void set_dvi_name(char *new_filename);
FILE *XFOPEN(const char *path, const char *mode);


#ifndef HAVE_MEMICMP
extern int memicmp(const char *, const char *, size_t);
#endif

/* NOTE: all of the following are already defined by kpathsea. */
/*  extern void *xmalloc(unsigned); */
/*  extern void *xrealloc(void *, unsigned); */
/*  extern char *xstrdup(const char *); */
/*  extern void xputenv(const char *, const char *); */

extern char *xmemdup(const char *, size_t);

/* like xstrdup, but only copy len characters and zero-terminate at next index (allocates len+1 characters) */
extern char *xstrndup(const char *str, size_t len);

extern char *xt_strdup(const char *); /* like xstrdup, but with XtMalloc() */

extern char *xstrcat(char *str1, const char *str2);
extern int xpipe(int *);
extern void close_a_file(void);
extern unsigned long get_bytes(FILE *, int);
extern long get_lbytes(FILE *, int);

extern void do_abort(void);

/* various levels of warning/error messages */
extern void xdvi_info(const char *fmt, ...);
extern void xdvi_warning(const char *fmt, ...);
extern void xdvi_error(const char *fmt, ...);
extern void xdvi_fatal(const char *fmt, ...);
extern void xdvi_abort(const char *fmt, ...);

extern Boolean pointerlocate(int *, int *);
extern unsigned long parse_debugging_string(const char *arg);
extern unsigned long parse_debugging_option(const char *ptr);

extern int get_avg_font_width(XFontStruct *font);
extern char **split_line(const char *line, char sep, size_t begin, size_t end, size_t *ret_items);
extern char *find_file(const char *filename, struct stat *statbuf, kpse_file_format_type pathinfo);
extern char **src_format_arguments(char **argv, const char *filename, int lineno, int colno);

/*
  hashtable wrapper functions, mostly used by dvi-draw.c to
  map filenames to integers. This uses the hashtable implementation
  from kpathsea, which is reasonably fast.
*/

/*
  We use this dummy wrapper stuct, which we cast to void *, to get integer
  values into/from the hashtable (natively, kpahtsea only supports string
  values).
*/
struct str_int_hash_item {
    int value;
};


typedef hash_table_type hashTableT; /* from kpathsea */
extern Boolean find_str_int_hash(hashTableT *hashtable, const char *key, size_t *val);
extern void put_str_int_hash(hashTableT *hashtable, const char *key, size_t val);


/*
 *	AVL tree structures.
 */

#define	AVL_COMMON							\
	const char	*key;		/* key */			\
	int		key_len;	/* length of key */		\
	int		bal;		/* AVL balancing information */	\
	struct avl	*left;						\
	struct avl	*right

struct avl {		/* generic data structure */
	AVL_COMMON;
};

extern struct avl *avladd(const char *, size_t, struct avl **, size_t);


extern Boolean copy_file(const char *from, const char *to);
extern Boolean copy_fp(FILE *in, FILE *out);

extern const char *get_text_encoding(void);
extern char *iconv_convert_string(const char *from_enc, const char *to_enc, const char *str);

extern void xdvi_bell(void);

/* Various error reporting macros.
   The reasons why these are macros are:
   - possibility to use __FILE__, __LINE__
   - (more importantly:) gcc can't do type checking on generic vararg
     macros, but does so for the printf() functions.
*/

/*
 *	Print an informative message to stdout,
 *	unless the resource `hush_stdout' is set.
 */
#define XDVI_INFO(X) do {					\
	fprintf(stdout, "%s: Info: ", globals.program_name);	\
	fflush(stdout); /* in case following goes to stderr accidentally ... */ \
	fprintf X;						\
	fprintf(stdout, "\n");					\
	fflush(stdout); /* to make sure it doesn't get intermingled with stderr */ \
} while(0)


/*
 *	Print a warning message to stderr.
 *	This can't be shut off by `hush_stdout'.
 */
#define XDVI_WARNING(X) do {					\
	fprintf(stderr, "%s: Warning: ", globals.program_name);	\
	fprintf X;						\
	fprintf(stderr, "\n");					\
} while(0)

/*
 *	Print an un-typed message to stderr, without starting a newline.
 *	This can't be shut off by `hush_stdout'.
 */
#define XDVI_MSG(X) do {					\
	fprintf(stderr, "%s: ", globals.program_name);		\
	fprintf X;						\
} while(0)


/*
 *	Print an error message to stderr.
 *	This can't be shut off by `hush_stdout'.
 */
#define XDVI_ERROR(X) do {					\
	fprintf(stderr, "%s: Error: ", globals.program_name);	\
	fprintf X;						\
	fprintf(stderr, "\n");					\
} while(0)

/*
 *	Print an error message and quit. Use this only with extreme care,
 *	if recovery is really impossible!
 */
#define XDVI_FATAL(X) do {						\
	fprintf(stderr, "%s: Fatal error: ", globals.program_name);	\
	fprintf X;							\
	fprintf(stderr, "\n");						\
	xdvi_exit(EXIT_FAILURE);					\
} while(0) 

/*
 *	Print an error message and abort. Use this instead of assertions
 *	if you want to give a more informative message.
 */
#define XDVI_ABORT(X) do {					\
	fprintf(stderr, "%s %s: %s:%d: Shouldn't happen: ",	\
			globals.program_name,			\
			XDVI_VERSION_INFO,			\
			__FILE__, __LINE__);			\
	fprintf X;						\
	fprintf(stderr, "\n");					\
	do_abort();						\
} while(0)

#endif /* UTIL_H_ */
