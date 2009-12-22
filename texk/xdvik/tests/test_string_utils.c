#include "xdvi-config.h"
#include "xdvi.h"
#include <stdarg.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "run_tests.h"

#include "string-utils.h"

#define DEBUG 0

static Boolean test_get_separated_list(int verbose)
{
    char *test_list1[] = {
	"This is the first line",
	"This is the second",
	"This, is the third",
	"The last one",
	NULL
    };
    char *test_list2[] = {
	"This",
	"is",
	"another",
	"first",
	"line",
	NULL
    };
    const char *teststring1 = "This is the first line,This is the second,\"This, is the third\",The last one";
    const char *teststring2 = "This\nis another\tfirst  line";

    char **res_list1 = get_separated_list(teststring1, ",", True);
    char **res_list2 = get_separated_list(teststring2, " \t\n", True);
    /* TODO: deallocate? */
    return test_str_list_equality(verbose, test_list1, res_list1) && test_str_list_equality(verbose, test_list2, res_list2);
}

static Boolean test_str_is_prefix(int verbose)
{
    static struct stringtest {
	char *str1;
	char *str2;
	Boolean case_sensitive;
	Boolean result;
    } tests[] = {
	{ "foo", "foobar", True, True },
	{ "foo", "Foobar", True, False },
	{ "foo", "Foobar", False, True },
	{ "foo", "FOObar", False, True },
	{ "foo", "foo", True, True },
	{ "f", "foo", True, True },
	{ "b", "foo", True, False },
	{ "foo", "b", True, False },
	{ "", "foo", True, True },
    };

    size_t i;
    Boolean result = True;
    for (i = 0; i < (sizeof tests / sizeof tests[0]); i++) {
	if (verbose) {
	    INFO((stderr, "str_is_prefix(%s,%s,%d) -> %d == %d?\n",
		  tests[i].str1,
		  tests[i].str2,
		  tests[i].case_sensitive,
		  str_is_prefix(tests[i].str1, tests[i].str2, tests[i].case_sensitive),
		  tests[i].result));
	}
	if (str_is_prefix(tests[i].str1, tests[i].str2, tests[i].case_sensitive) != tests[i].result) {
	    result = False;
	}
    }
    return result;
}

static Boolean test_str_is_suffix(int verbose)
{
    static struct stringtest {
	char *str1;
	char *str2;
	Boolean case_sensitive;
	Boolean result;
    } tests[] = {
	{ "bar", "foobar", True, True },
	{ "bar", "FooBar", True, False },
	{ "bar", "fooBAR", False, True },
	{ "bar", "fooBaR", False, True },
	{ "bar", "bar", True, True },
	{ "r", "bar", True, True },
	{ "x", "bar", True, False },
	{ "bar", "r", True, False },
	{ "", "bar", True, True },
	{ "", "", True, True },
    };

    size_t i;
    Boolean result = True;
    for (i = 0; i < (sizeof tests / sizeof tests[0]); i++) {
	if (verbose) {
	    INFO((stderr, "str_is_suffix(%s,%s,%d) -> %d == %d?\n",
		  tests[i].str1,
		  tests[i].str2,
		  tests[i].case_sensitive,
		  str_is_suffix(tests[i].str1, tests[i].str2, tests[i].case_sensitive),
		  tests[i].result));
	}
	if (str_is_suffix(tests[i].str1, tests[i].str2, tests[i].case_sensitive) != tests[i].result) {
	    result = False;
	}
    }
    return result;
}

static Boolean test_is_spaces_only(int verbose)
{
    static struct stringtest {
	char *str1;
	Boolean result;
    } tests[] = {
	{ "bar", False },
	{ " \t\n ", True },
	{ " a ", False },
	{ "", True },
    };

    size_t i;
    Boolean result = True;
    for (i = 0; i < (sizeof tests / sizeof tests[0]); i++) {
	if (verbose) {
	    INFO((stderr, "is_spaces_only(%s) -> %d == %d?\n",
		  tests[i].str1,
		  is_spaces_only(tests[i].str1),
		  tests[i].result));
	}
	if (is_spaces_only(tests[i].str1) != tests[i].result) {
	    result = False;
	}
    }
    return result;
}

static Boolean test_my_stristr(int verbose)
{
    static struct stringtest {
	char *str1;
	char *str2;
	char *result;
    } tests[] = {
	{ "paloozah", "ooz", "oozah" },
	{ "paloozah", "pa", "paloozah" },
	{ "paloozah", "par", NULL },
	{ "paloozah", "x", NULL },
	{ "PALOOZAH", "ooz", "OOZAH" },
	{ "PALOOZAH", "ah", "AH" },
	{ "PALOOZAH", "h", "H" },
	{ "h", "h", "h" },
	{ "H", "h", "H" },
	{ "", "h", NULL },
    };

    size_t i;
    Boolean result = True;
    for (i = 0; i < (sizeof tests / sizeof tests[0]); i++) {
	char *res_str = my_stristr(tests[i].str1, tests[i].str2);
	/* We want to verify that my_stristr(STR1, STR2) is either NULL or
	 * a substring of STR1, but, e.g., `"oozah" == "paloozah" + 3' may
	 * or may not be true (depending on compiler optimizations).  */
	char *test_str = tests[i].result ?
			 tests[i].str1 + strlen(tests[i].str1) - strlen(tests[i].result) :
			 NULL;
	if (verbose) {
	    INFO((stderr, "my_stristr(%s, %s) -> %s == %s?\n",
		  tests[i].str1,
		  tests[i].str2,
		  res_str,
		  test_str));
	}
	if (res_str != test_str) {
	    result = False;
	}
    }
    return result;
}

static Boolean test_format_arg(int verbose)
{
    static struct stringtest {
	char *str1;
	char *arg;
	char *result;
    } tests[] = {
	{ "netscape(%s, same-window)", "file.html", "netscape(file.html, same-window)" },
	{ "netscape(same-window)", "file.html", "netscape(same-window)" },
	{ "%s", "a", "a" },
	{ "", "", "" },
    };

    size_t i;
    int tmpcnt = 0;
    Boolean result = True;
    for (i = 0; i < (sizeof tests / sizeof tests[0]); i++) {
	if (verbose) {
	    INFO((stderr, "format_arg(%s) -> %s == %s, %d\n",
		  tests[i].str1,
		  format_arg(tests[i].str1, tests[i].arg, &tmpcnt),
		  tests[i].result,
		  tmpcnt));
	}
	if (!test_str_equality(verbose,
			       format_arg(tests[i].str1, tests[i].arg, &tmpcnt),
			       tests[i].result)) {
	    result = False;
	}
    }
    /* tempcnt shouldn't be reset to 0 by format_arg() */
    if (tmpcnt != 1) {
	ERROR((stderr, "tmpcnt should be 1\n"));
	result = False;
    }
    return result;
}

static Boolean test_escape_format_arg(int verbose)
{
    static struct stringtest {
	char *str;
	char *result;
    } tests[] = {
	{ "netscape(%s, same-window)", "netscape(%%s, same-window)" },
	{ "%%s", "%%%%s" },
	{ "%d%s", "%%d%%s" },
	{ "a", "a" },
	{ "", "" },
    };

    size_t i;
    Boolean result = True;
    for (i = 0; i < (sizeof tests / sizeof tests[0]); i++) {
	if (verbose) {
	    INFO((stderr, "escape_format_arg(%s) -> %s == %s\n",
		  tests[i].str,
		  escape_format_arg(tests[i].str),
		  tests[i].result));
	}
	if (!test_str_equality(verbose,
			       escape_format_arg(tests[i].str),
			       tests[i].result)) {
	    result = False;
	}
    }
    return result;
}

static Boolean test_filename_append_dvi(int verbose)
{
    static struct stringtest {
	char *str1;
	char *str2;
    } tests[] = {
	{ "file:///tmp/foo.bar", "/tmp/foo.bar" },
	{ "file:///tmp/foo", "/tmp/foo.dvi" },
	/* the following two mimick firefox behaviour with double/single slashes */
	{ "file://tmp/foo", "/foo.dvi" },
	{ "file:/tmp/foo", "/tmp/foo.dvi" },
	{ "file://localhost/tmp/foo", "/tmp/foo.dvi" },
	{ "~/tmp/foo", "~/tmp/foo.dvi" },
    };

    size_t i;
    Boolean result = True;
    for (i = 0; i < (sizeof tests / sizeof tests[0]); i++) {
	char *tmp = filename_append_dvi(tests[i].str1);
	if (verbose) {
	    INFO((stderr, "filename_append_dvi(%s) -> %s == %s?\n",
		  tests[i].str1,
		  tmp,
		  tests[i].str2));
	}
	if (!test_str_equality(verbose, tmp, tests[i].str2)) {
	    result = False;
	}
	free(tmp);
    }
    return result;
}

static Boolean test_canonicalize_path(int verbose)
{
    static struct stringtest {
	char *str1;
	char *str2;
    } tests[] = {
	{ "/foo/a/bz.to/bar.dvi", "/foo/a/bz.to/bar.dvi" },
	{ "/foo/../bar.dvi", "/bar.dvi" },
	{ "/foo/../bar.dvi", "/bar.dvi" },
	{ "/foo/./roo/./bar.dvi", "/foo/roo/bar.dvi" },
	{ "/foo/./roo/../bar.dvi", "/foo/bar.dvi" },
    };

    size_t i;
    Boolean result = True;
    for (i = 0; i < (sizeof tests / sizeof tests[0]); i++) {
	if (verbose) {
	    INFO((stderr, "canonicalize_path(%s) -> %s == %s?\n",
		  tests[i].str1,
		  canonicalize_path(tests[i].str1),
		  tests[i].str2));
	}
	if (!test_str_equality(verbose, canonicalize_path(tests[i].str1), tests[i].str2)) {
	    result = False;
	}
    }
    return result;
}



static Boolean test_expand_filename(int verbose)
{
    char cwd[10240];
    char *currpath, *testpath1, *testpath2;

    currpath = xstrdup(getcwd(cwd, 10240));
    testpath1 = expand_filename(__FILE__, USE_CWD_PATH);
    globals.dvi_file.dirname = xstrdup(cwd);
    globals.dvi_file.dirname = xstrcat(globals.dvi_file.dirname, "/");
    testpath2 = expand_filename(__FILE__, USE_DVI_PATH);

    if (__FILE__[0] == '/') {
	free(currpath);
	currpath = xstrdup(__FILE__);
    } else {
	currpath = xstrcat(currpath, "/");
	currpath = xstrcat(currpath, __FILE__);
    }

    return test_str_equality(verbose, testpath1, currpath) \
	&& test_str_equality(verbose, testpath2, currpath);
}


static Boolean test_get_extension(int verbose)
{
    const char *test_strings[][2] = {
	{ "/dev/fd/3", NULL },
	{ "/home/user/file.dvi", ".dvi" },
	{ "/home/user/file.ps", ".ps" },
	{ "/home/user.bar/file.dvi", ".dvi" },
	{ "/home/user.bar/file", NULL },
	{ "/home/user/file", NULL },
	{ NULL, NULL }
    };

    size_t i;
    
    for (i = 0; test_strings[i][0] != NULL; i++) {
	const char *tmp = get_extension(test_strings[i][0]);
	if (tmp == NULL && test_strings[i][1] == NULL) {
	    if (verbose)
		INFO((stderr, "Checking extension of |%s| OK: <NULL> == <NULL>\n", test_strings[i][0]));
	}
	else if (tmp == NULL && test_strings[i][1] != NULL) {
	    if (verbose)
		ERROR((stderr, "extension of |%s| gave <NULL> - should be |%s|\n",
		       test_strings[i][0], test_strings[i][1]));
	    return False;
	}
	else if (tmp != NULL && test_strings[i][1] == NULL) {
	    if (verbose)
		ERROR((stderr, "extension of |%s| gave |%s| - should be <NULL>\n",
		       test_strings[i][0], tmp));
	    return False;
	}
	else {
	    if (strcmp(tmp, test_strings[i][1]) != 0) {
		if (verbose)
		    ERROR((stderr, "ERROR: extension of |%s| gave |%s| - should be |%s|\n",
			   test_strings[i][0], tmp, test_strings[i][1]));
		return False;
	    }
	    else {
		if (verbose)
		    INFO((stderr, "Checking extension of |%s| OK: |%s| == |%s|\n",
			  test_strings[i][0], tmp, test_strings[i][1]));
	    }
	}
    }

    return True;
}

static Boolean test_length_of_int(int verbose)
{
    static struct stringtest {
	int d;
	int len;
    } tests[] = {
	{ 0, 1 },
	{ 1, 1 },
	{ 11, 2 },
	{ 111, 3 },
	{ 100, 3 },
	{ 100000, 6 },
	{ 999999, 6 },
	{ 1000000, 7 },
	{ 101, 3 },
	{ 001, 1 },
    };

    size_t i;
    Boolean result = True;
    for (i = 0; i < (sizeof tests / sizeof tests[0]); i++) {
	if (verbose) {
	    INFO((stderr, "length_of_int(%d) -> %d == %d?\n",
		  tests[i].d,
		  length_of_int(tests[i].d),
		  tests[i].len));
	}
	if (length_of_int(tests[i].d) != tests[i].len) {
	    result = False;
	}
    }
    return result;
}

void register_all_from_test_string_utils(void)
{
    register_test(test_str_is_prefix, "str_is_prefix()");
    register_test(test_str_is_suffix, "str_is_suffix()");
    register_test(test_is_spaces_only, "is_spaces_only()");
    register_test(test_my_stristr, "my_stristr()");
    register_test(test_filename_append_dvi, "filename_append_dvi()");
    /* TODO: expand_filename_append_dvi */
    register_test(test_canonicalize_path, "canonicalize_path()");
    register_test(test_format_arg, "format_arg()");
    register_test(test_escape_format_arg, "escape_format_arg()");
    register_test(test_expand_filename, "expand_filename()");
    register_test(test_get_separated_list, "get_separated_list");
    register_test(test_length_of_int, "length_of_int()");
    register_test(test_get_extension, "Filename extensions");
}
