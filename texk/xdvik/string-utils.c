/*
 * miscellaneous string utility functions
 * 
 * Copyright (c) 2001-2004 the xdvik development team
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
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL PAUL VOJTA OR ANY OTHER AUTHOR OF THIS SOFTWARE BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*  #define MYDEBUG 1 */

#include "xdvi-config.h"
#include "xdvi.h"
#include "string-utils.h"
#include "util.h"

#include <string.h>
#include <stdio.h>
#include <locale.h>
#include <ctype.h>

/*------------------------------------------------------------
 *  str_is_prefix
 *
 *  Purpose:
 *	Return True if <str1> is a prefix of <str2>, else False.
 *
 *	If `case_sensitive' is set to False, it performs matching
 *	in a case-insensitive manner; in that case, str1 should
 *	be lowercase.
 *------------------------------------------------------------*/

Boolean
str_is_prefix(const char *str1, const char *str2, Boolean case_sensitive)
{
    int i;

    for (i = 0; *str1 != '\0' && *str2 != '\0'; i++) {
	if ((case_sensitive && *str1 != *str2) ||
	    (!case_sensitive && *str1 != tolower((int)*str2)))
	    return False;
	str1++;
	str2++;
    }
    return *str1 == '\0';
}



/*------------------------------------------------------------
 *  str_is_suffix
 *
 *  Purpose:
 *	Return True if str1 is a suffix of str2, else False.
 *	E.g. returns true if str1 = ".tex", str2 = "test.tex".
 *
 *	If `case_sensitive' is set to False, it performs matching
 *	in a case-insensitive manner; in that case, str1 should
 *	be lowercase.
 *------------------------------------------------------------*/

Boolean
str_is_suffix(const char *str1, const char *str2, Boolean case_sensitive)
{
    int len1 = strlen(str1);
    int len2 = strlen(str2);

    while (len2 > len1) {
	str2++;
	len2--;
    }
    if (case_sensitive)
	return strcmp(str1, str2) == 0;
    else
	/* also compare terminating 0; second string
	   is assumed to be all-lowercase! */
	return memicmp(str2, str1, len1 + 1) == 0;
}

/*
 * Like strstr(), but does case-insensitive matching: Brute-force algorithm
 * to find the first occurrence of subsrtring `needle' (which should be all-lowercase)
 * in string `haystack', ignoring case in (i.e. lowercasing) haystack. The terminating
 * '\0' characters are not compared.
 * Returns a pointer to the beginning of the substring if found, NULL else.
 *
 * This code was derived from public domain code (Stristr.c on www.snippets.org).
 * Currently unused.
 */
char *
my_stristr(const char *haystack, const char *needle)
{
    const char *curr;

    for (curr = haystack; *curr != '\0'; curr++) {
	const char *ptr1, *ptr2;
	/* search for first character */
	for (; ((*curr != '\0') && (tolower((int)*curr) != *needle)); curr++) { ; }

	if (*curr == '\0') /* not found */
	    return NULL;

	/* now compare other characters */
	ptr1 = needle;
	ptr2 = curr;
	while (tolower((int)*ptr2) == *ptr1) {
	    ptr2++;
	    ptr1++;
	    /* success if at end of needle */
	    if (*ptr1 == '\0')
		return (char *)curr;
	}
    }
    return NULL;
}

/*
  expand filename to include full path name;
  returns result in a freshly allocated string.
*/
char *
expand_filename(const char *filename, expandPathTypeT type)
{
    char *path_name = NULL;

    if (*filename == '/') /* already an absolute path */
	return xstrdup(filename);
    
    if (type == USE_CWD_PATH) {
	size_t path_name_len = 512;
	size_t len = strlen(filename) + 1;

	/* append to cwd if it's not already a full path */
	if (filename[0] != '/') {
	    for (;;) {
		char *tmp;
		path_name = xrealloc(path_name, path_name_len);
		if ((tmp = getcwd(path_name, path_name_len)) == NULL && errno == ERANGE) {
		    path_name_len *= 2;
		}
		else {
		    path_name = tmp;
		    break;
		}
	    }
	    len += strlen(path_name) + 1;	/* 1 for '/' */
	    path_name = xrealloc(path_name, len);
	    strcat(path_name, "/");
	    strcat(path_name, filename);
	}
	
	TRACE_HTEX((stderr,
		    "Expanding filename |%s| with CWD gives |%s|",
		    filename, path_name == NULL ? "<NULL>" : path_name));
	return path_name ? path_name : xstrdup(filename);
    }
    else {
	ASSERT(globals.dvi_file.dirname != NULL, "globals.dvi_file.dirname should have been initialized before");
	path_name = xstrdup(globals.dvi_file.dirname);
	path_name = xstrcat(path_name, filename);
	TRACE_HTEX((stderr,
		    "Expanding filename |%s| with globals.dvi_file.dirname |%s| gives |%s|",
		    filename, globals.dvi_file.dirname, path_name));
	return path_name;
    }
}

/* expand filename to include `.dvi' extension and full path name;
   returns malloc()ed string (caller is responsible for free()ing).
*/
char *
filename_append_dvi(const char *filename)
{
    char *expanded_filename = NULL;
    const char *orig_filename = filename;
    size_t len;
    char *p;
    
    /* skip over `file:' prefix if present */
    if (str_is_prefix("file:", filename, True)) {
	filename += strlen("file:");
	if (str_is_prefix("//", filename, True)) { /* is there a hostname following? */
	    char *tmp = strchr(filename+2, '/'); /* skip over host name */
	    if (tmp == NULL) {
		XDVI_WARNING((stderr, "Malformed hostname part in filename `%s'; not expanding file name",
			      orig_filename));
	    }
	    else {
		/* deal with multiple `//' after "file://localhost";
		   while the RFC seems to suggest that file://localhost/foo/bar defines a path
		   `foo/bar', most browsers (and actually, also libwww) will treat this path as absolute:
		   `/foo/bar'. So we treat
		   file://localhost//foo/bar
		   and
		   file://localhost/foo/bar
		   as equivalent.
		*/
		while (*(tmp+1) == '/') 
		    tmp++;
		filename = tmp;
	    }
	}
    }

    len = strlen(filename) + 5; /* 5 in case we need to add `.dvi\0' */

    expanded_filename = xmalloc(len);

    strcpy(expanded_filename, filename);

    /* Append ".dvi" extension if no extension is present.
       Only look at the filename part when trying to find a `.'.
    */
    if ((p = strrchr(expanded_filename, '/')) == NULL) {
	p = expanded_filename;
    }
    if ((p = strrchr(p, '.')) == NULL) {
	TRACE_HTEX((stderr, "appending .dvi extension to filename |%s|", expanded_filename));
	strcat(expanded_filename, ".dvi");
    }
    return expanded_filename;
}

char *
expand_filename_append_dvi(const char *filename, expandPathTypeT type, Boolean must_exist)
{
    char canonical_path[MAXPATHLEN + 1];
    char *normalized_fname = filename_append_dvi(filename);
    char *expanded_fname = expand_filename(normalized_fname, type);
    if (must_exist) {
	char *canonical_name = REALPATH(expanded_fname, canonical_path);
	free(normalized_fname);
	free(expanded_fname);
	return xstrdup(canonical_name);
    }
    else {
	free(normalized_fname);
	return expanded_fname;
    }
}

char *
format_arg(const char *fmt, const char *arg, int *match)
{
    char *tmp = xmalloc(strlen(fmt) + strlen(arg) + 1);
    if (strchr(fmt, '%') != NULL) {
	sprintf(tmp, fmt, arg);
	*match = 1;
    }
    else {
	strcpy(tmp, fmt);
	/* NOTE: don't reset *match to 0, leave that to caller */
    }
    return tmp;
}

/* escape single `%' characters in arg and return newly allocated string */
char *
escape_format_arg(const char *arg)
{
    char *ret, *ptr;
    ASSERT(arg != NULL, "");
    ret = xmalloc(strlen(arg) * 2 + 1); /* more than enuff */
    
    ptr = ret;
    while (*arg != '\0') {
	/* need to escape? */
	if (*arg == '%') { /* && (ptr == ret
			    || (ptr > ret && *(arg - 1) != '%'))) { */
	    *ptr++ = '%';
	}
	*ptr++ = *arg++;
    }
    *ptr = '\0';

    /* trim return buffer */
    return xrealloc(ret, strlen(ret) + 1);
}

char *
unquote_arg(const char *fmt, const char *arg, int *match, int *len)
{
    char *ptr;
    char c;
    
    c = fmt[0];
    fmt++;
    if ((ptr = strchr(fmt, c)) != NULL) {
	*ptr++ = '\0';
	while (*ptr == ' ' || *ptr == '\t') {
	    ptr++;
	}
	*len = ptr - fmt;
	return format_arg(fmt, arg, match);
    }
    else {
	*len = strlen(fmt);
	XDVI_WARNING((stderr, "Ignoring lonesome quote in string %s", fmt - 1));
	return format_arg(fmt, arg, match);
    }
}

/* Chop `source' into chunks separated by `sep', and save these
 * to newly allocated return list. The end of the list is indicated
 * by a NULL element (i.e. the returned list will always contain at
 * least 1 element). The caller is responsible for free()ing the returned
 * list.
 *
 * If `do_unquote' is True, separators inside single or double quotation marks will not be
 * treated as boundaries. The quotation marks surrounding the chunk will be removed
 * as well in that case.
 */
char **
get_separated_list(const char *source, const char *sep, Boolean do_unquote)
{
    /* allocate at least list of size 1, for NULL termination below */
    char **chunks = xmalloc(sizeof *chunks);
    const char *b_ptr, *e_ptr;
    size_t i = 0;
    
    b_ptr = source;

    while (*b_ptr != '\0' && strchr(sep, *b_ptr) != NULL)
	b_ptr++;
    
    for (i = 0; *b_ptr != '\0'; i++) {
	char *quote;
	char quotechar = 0;
	size_t len, chunk_len, alloc_len = 0;
	
	if ((len = strcspn(b_ptr, sep)) == 0) /* at end */
	    break;

	/* check for quoted chunk, in which case we don't want to treat
	   spaces as chunk separators */
	if (do_unquote && (quote = strchr("'\"", *b_ptr)) != NULL) {
	    const char *curr = b_ptr + 1;
	    quotechar = *quote;

	    for (;;) { /* find end of quote */
		char *maybe_end = strchr(curr, quotechar);
		if (maybe_end != NULL) {
		    if (maybe_end - curr > 0 &&
			*(maybe_end - 1) == '\\') { /* escaped quote */
			curr = ++maybe_end;
		    }
		    else { /* found */
			b_ptr++;
			len = maybe_end - b_ptr;
			break;
		    }
		}
		else { /* not found end - warn, and forget about this quote */
		    XDVI_WARNING((stderr, "Unmatched quote character in string `%s'", b_ptr));
		    break;
		}
	    }
	}
	
	e_ptr = b_ptr + len;
	chunk_len = e_ptr - b_ptr;
	while (i + 1 >= alloc_len) {
	    alloc_len++;
	}
	chunks = xrealloc(chunks, alloc_len * sizeof *chunks);
	chunks[i] = xmalloc(chunk_len + 1);
	memcpy(chunks[i], b_ptr, chunk_len);
	chunks[i][chunk_len] = '\0';

	/* skip trailing quotechar and spaces */
	b_ptr = e_ptr;
	if (do_unquote && quotechar != 0 && *b_ptr == quotechar)
	    b_ptr++;
	while (*b_ptr != '\0' && strchr(sep, *b_ptr) != NULL)
	    b_ptr++;
    }
    /* terminate list with NULL element */
    chunks[i] = NULL;
    return chunks;
}

const char *
find_format_str(const char *input, const char *fmt)
{
    const char *ptr = input;
    while ((ptr = strstr(ptr, fmt)) != NULL) {
	if (ptr > input && *(ptr - 1) == '\\') {
	    ptr++;
	    continue;
	}
	else
	    return ptr;
    }
    return NULL;
}

/* return directory component of `path', or NULL if path is a filename only */
char *
get_dir_component(const char *path)
{
    char *ret = NULL;
    char *p;
    
    if ((p = strrchr(path, '/')) != NULL) {
	/* copy, chop off filename (but not the '/') */
	ret = xstrdup(path);
	*(ret + (p - path) + 1) = '\0';
	TRACE_CLIENT((stderr, "get_dir_component of |%s| is |%s|\n", path, ret));
    }
    return ret;
}

/*
  If `src' is an absolute path, compare it with `target', ignoring `.tex' extension in
  both strings. Else, compare the concatenation of `src' with `dvi_path' and `target',
  in the same way.
  Since efficiency is a concern here, we don't copy any strings; instead, we loop through
  `src' and `target' from the end of both strings (which makes expanding `../' easier, and
  will terminate earlier for non-equal files), replacing `src' with `dvi_path' when
  dropping off the beginning of `src'.
*/
Boolean
src_compare(const char *src, int src_len, const char *target, const char *dvi_path, size_t dvi_path_len)
{
    int i, j;
    Boolean matched = True;

    /* This macro sets the `src' pointer to `dvi_path' after having
       dropped off the beginning of src, or returns False if dvi_path
       is NULL (which either means that `src' was an absolute path, or
       that the dvi_path has been used up already).
    */
#define CHK_USE_DIR(i)								\
	if (i == -1) {								\
	    if (dvi_path == NULL) /* already done */				\
		return False;							\
	    src = dvi_path;							\
	    dvi_path = NULL;							\
	    i = dvi_path_len - 1;						\
	    MYTRACE((stderr, "swapped, now using: |%s| of len %d", src, i));	\
	}

    /* ignore path in both filenames if one of them does not contain a path */
    {
	char *src_p = strrchr(src, '/');
	char *target_p = strrchr(target, '/');

	if (src_p == NULL) {
	    dvi_path = NULL;
	    if (target_p != NULL)
		target = target_p + 1;
	}

	if (target_p == NULL) {
	    dvi_path = NULL;
	    if (src_p != NULL)
		src = src_p + 1;
	}
    }

    /* don't prepend dvi_path if src is absolute */
    if (*src == '/') { 
	dvi_path = NULL;
    }
    
    /* skip `.tex' suffix in strings if present */
    i = src_len;
    MYTRACE((stderr, "len of |%s| %d", src, i));
    if (i >= 4 && strcmp(src + (i - 4), ".tex") == 0) {
	MYTRACE((stderr, "src |%s| has .tex suffix!", src));
	i -= 4;
    }

    j = strlen(target);
    MYTRACE((stderr, "len of |%s| %d", target, j));
    if (j >= 4 && strcmp(target + (j - 4), ".tex") == 0) {
	MYTRACE((stderr, "target |%s| has .tex suffix!", target));
	j -= 4;
    }

    /* start with index of last char */
    i--;
    j--;

    while (i >= 0 && j >= 0) {
	int skip_dirs = 0;
	/*  	fprintf(stderr, "check: %d[%c]\n", i, src[i]); */
	while (src[i] == '.' && src[i + 1] == '/') { /* normalize `../' and `./' */
	    MYTRACE((stderr, "check2: %d[%c]", i, src[i]));

	    if (i >= 2 && src[i - 1] == '.' && src[i - 2] == '/') {
		MYTRACE((stderr, "case /.."));
		i -= 3;
		skip_dirs++;
	    }
	    else if (i == 1) { /* `../' or `/./' at start of src */
		if (src[0] == '.') { /* `../' */
		    MYTRACE((stderr, "../ at start"));
		    i -= 2;
		    skip_dirs++;
		}
		else if (src[0] == '/') { /* `/./' */
		    MYTRACE((stderr, "/./ at start"));
		    i -= 1;
		}
		else /* something else */
		    break;
	    }
	    else if (i == 0 || (i > 1 && src[i - 1] == '/')) { /* ./ at start, or /./ somewhere */
		i -= 1;
	    }
	    else /* might be `abc./' or `abc../' (strange but legal) */
		break;

	    CHK_USE_DIR(i);
	    while (i >= 0 && src[i] == '/') i--;
	    CHK_USE_DIR(i);
	    
	    while (src[i] != '.' && skip_dirs-- > 0) { /* unless there are subsequent `../' */
		/* skip directories backwards */
		while (i >= 0 && src[i] != '/') {
		    MYTRACE((stderr, "non-slash: %d,%c", i, src[i]));
		    i--;
		}
		CHK_USE_DIR(i);
		while (i >= 0 && src[i] == '/') {
		    MYTRACE((stderr, "slash: %d,%c", i, src[i]));
		    i--;
		}
		CHK_USE_DIR(i);
		MYTRACE((stderr, "skipped backwards: %d,%c", i, src[i]));
	    }
	}

	/* skip multiple '/'. No need to fall off the beginning of src and use
	   CHK_USE_DIR() here, since with (multiple) '/' at the beginning, src
	   must be an absolute path. */
	while (i > 0 && src[i] == '/' && src[i - 1] == '/') {
	    i--;
	}
	
	MYTRACE((stderr, "comparing: %d[%c] - %d[%c]", i, src[i], j, target[j]));
	if (src[i] != target[j]) {
	    matched = False;
	    break;
	}
	if (i == 0 && j == 0) /* at start of both strings */
	    break;
	i--;
	j--;
	CHK_USE_DIR(i);
    }

    if (i != 0 || j != 0) /* not at start of both strings, no match */
	return False;
    else
	return matched;
#undef CHK_USE_DIR
}


/*
  Contributed by ZLB: Return a canonicalized version of path (with `../'
  and `./' resolved and `//' reduced to '/') in a freshly malloc()ed
  buffer, which the caller is responsible for free()ing. Cannot fail.
*/
char *
canonicalize_path(const char *path)
{
    char *p, *q, *start;
    char c;
    size_t len = strlen(path);

    assert(path != NULL);
    assert(*path == '/');

    start = q = p = xstrdup(path);

    /* len is the length of string */
    while (p < start + len) {
	if ((c = p[1]) == '/') {
	    /* remove multiple '/' in pathname */
	    memmove(p + 1, p + 2, len - (p + 2 - start) + 1);
	    len--;
	    continue;
	}
	else if (c == '.') {
	    if ((c = p[2]) == '/') {
		/* p = '/.' in pathname */
		memmove(p + 1, p + 3, len - (p + 3 - start) + 1);
		len -= 2;
		continue;
	    }
	    else if (c == '.' && ((c = p[3]) == '/' || c == '\0')) {
		/* p == "/.." */
		memmove(q, p + 3, len - (p + 3 - start) + 1);
		len -= (p - q) + 3;
		p = q;
		/* check if the new dirname at p is "//" or './' or '../' */
		if ((c = p[1]) == '/')
		    continue;
		else if (c == '.') {
		    if ((c = p[2]) == '/')
			continue;
		    else if (c == '.' && ((c = p[3]) == '/' || c == '\0')) {
			while (--q >= start && *q != '/');
			if (q < start)
			    q = start;
			continue;
		    }
		}
	    }
	}

	/* search next '/' */
	q = p;
	while (++p <= start + len && *p != '/');
    }

    return start;
}

/* Escape all of the following characters in str:
   ` \ ; ( )
   making it safe to pass str to a shell. Return result in a newly
   allocated string, which the caller is responsible to free() after use.
*/
char *
shell_escape_string(const char *str)
{
    size_t len = strlen(str);
    char *new_str = xmalloc(len * 2 + 1); /* safe amount, since each char will be doubled at most */
    
    const char *src_ptr = str;
    char *target_ptr = new_str;
    while (*src_ptr != '\0') {
	if (*src_ptr == '\\'
	    || *src_ptr == '`'
	    || *src_ptr == '('
	    || *src_ptr == ')'
	    || *src_ptr == ';') {
#if 0
	    /* only if not yet escaped? */
 	    && (src_ptr == str || (src_ptr > str && *(src_ptr - 1) != '\\'))) {
#endif
	    *target_ptr++ = '\\';
	}
	*target_ptr++ = *src_ptr++;
    }
    *target_ptr = '\0'; /* terminate */
    return new_str;
}

/* Get a pointer to the extension of the filename 'fname'
   (ie. into the existing string),
   or NULL if fname doens't have an extension.
 */
const char *
get_extension(const char *fname)
{
    char *sep, *tmp;
    /* does filename have a directory component?
       If so, be careful with dots within this component.
    */
    if ((sep = strrchr(fname, '/')) != NULL) {
	tmp = sep;
	if ((sep = strrchr(tmp, '.')) != NULL) {
	    return sep;
	}
	else {
	    return NULL;
	}
    }
    else if ((sep = strrchr(fname, '.')) != NULL) {
	return sep;
    }
    else {
	return NULL;
    }
}

void
replace_extension(const char *fname, const char *extension,
		  char *buf, size_t buf_len)
{
    char *sep;
    if ((sep = strrchr(fname, '.')) != NULL) {
	size_t len = strlen(extension);
	if (len + (sep - fname) > buf_len)
	    return;
	memcpy(buf, fname, sep - fname);
	strcpy(buf + (sep - fname), extension);
    }
    return;
}

#if 0
/*
 * Estimate the string length needed for %p conversion. Currently unused,
 * since we use the more general VSNPRINTF() approach.
 */
#define PTR_CONVERSION_LEN_GUESS sizeof(void *) * CHAR_BIT
#endif

   
/*
 * Return a formatted string in newly allocated memory.
 */
char *
get_string_va(const char *fmt, ...)
{
    char *buf = NULL;
    XDVI_GET_STRING_ARGP(buf, fmt);
    return buf;
}

/* Wrapper for atof() for strtod()-like error checking,
 * with an XDVI_WARNING if the conversion of <str> wasn't complete.
 */
double
my_atof(const char *str)
{
    char *ptr;
    double f;
    
    f = strtod(str, (char **)&ptr);
    if (*ptr != '\0') {
	XDVI_WARNING((stderr, "strtod: incomplete conversion of %s to %f", str, f));
    }
    return f;
}

/* return length of a string representation of the integer n */
int
length_of_int(int n)
{
    int ret = 1;

    if (n < 0) {
	ret++;
	n *= -1;
    }
    while (n >= 10) {
	n /= 10;
	ret++;
    }

    return ret;
}

Boolean
is_spaces_only(const char *ptr)
{
    for (; *ptr; ptr++) {
	if (!isspace((int)*ptr))
	    return False;
    }
    return True;
}
