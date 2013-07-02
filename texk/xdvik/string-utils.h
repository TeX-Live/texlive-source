#ifndef STRING_UTILS_H_
#define STRING_UTILS_H_

#include <stdarg.h>
#include "my-vsnprintf.h"

extern Boolean str_is_prefix(const char *, const char *, Boolean case_sensitive);
extern Boolean str_is_suffix(const char *, const char *, Boolean case_sensitive);
extern Boolean is_spaces_only(const char *);

extern char *my_stristr(const char *haystack, const char *needle);

typedef enum { USE_DVI_PATH, USE_CWD_PATH } expandPathTypeT;

/* remove `file:[//localhost/]' prefix from filename, and append `.dvi'
   extension if no extension is present */
extern char *filename_append_dvi(const char *fname);
/* prepend a path of type `type' (either CWD, or directory of dvi file)
   to `fname' */
extern char *expand_filename(const char *fname, expandPathTypeT type);

/* do both of the above */
extern char *expand_filename_append_dvi(const char *fname, expandPathTypeT type, Boolean must_exist);

/* normalize `../' `./' `//' from path */
extern char *canonicalize_path(const char *path);

extern char *format_arg(const char *fmt, const char *arg, int *match);
extern char *escape_format_arg(const char *arg);
extern char *unquote_arg(const char *fmt, const char *arg, int *match, int *len);
extern char *get_dir_component(const char *path);
extern char **get_separated_list(const char *source, const char *sep, Boolean do_unquote);
extern const char *find_format_str(const char *input, const char *fmt);
extern Boolean src_compare(const char *src, int src_len, const char *target, const char *path, size_t path_len);
extern char *shell_escape_string(const char *str);
extern void replace_extension(const char *fname, const char *extension, char *buf, size_t name_len);
extern const char *get_extension(const char *fname);
extern int length_of_int(int n);

extern char *get_string_va(const char *fmt, ...);
extern double my_atof(const char *str);

/*
 * Print `fmt' to a dynamically allocated string `buf'. Uses VSNPRINTF
 * in a way that should work with all of glibc <= 2.0.6, >= 2.1, and
 * the implementation in my-vsnprintf.c. We cannot invoke va_start()/va_end()
 * in a wrapper function since we may need to invoke vsnprintf() several
 * times, and each call invalidates argp; that's why this is a macro ...
 */
#define XDVI_GET_STRING_ARGP(buf, fmt)								\
    do {											\
        va_list argp;										\
	int CHUNK_LEN = 128;									\
	int curr_len = CHUNK_LEN, need_len;							\
												\
	buf = NULL;										\
												\
	for (;;) {										\
	     /* see man page for vsnprintf: for glibc 2.0, <need_len> is */			\
	     /* -1 if <buf> had been trucated to <curr_len>; for glibc 2.1, */			\
	     /* it's the total length of <fmt>; so the following works */			\
	     /* with both versions: */								\
	    buf = xrealloc(buf, curr_len);							\
												\
	    va_start(argp, fmt);								\
	    need_len = VSNPRINTF(buf, curr_len, fmt, argp);					\
            va_end(argp);									\
												\
	    if (need_len > -1 && need_len < curr_len) {	/* <curr_len> was large enough */	\
		break;										\
	    }											\
	    if (need_len > -1) {	/* now need_len + 1 is the exact space needed */	\
		curr_len = need_len + 1;							\
	    }											\
	    else {	/* try again with larger <len> */					\
		curr_len += CHUNK_LEN;								\
	    }											\
	}											\
    } while (0)											\

#endif /* STRING_UTILS_H_ */
